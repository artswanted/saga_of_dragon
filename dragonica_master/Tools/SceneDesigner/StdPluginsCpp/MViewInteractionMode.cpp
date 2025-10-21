// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MViewInteractionMode.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::
    StandardServices;

//---------------------------------------------------------------------------
void MViewInteractionMode::SetupOrbit(int iX, int iY)
{
    MCameraManager* pmCameraManager = MFramework::Instance->
        CameraManager;
    MViewport* pmActiveViewport = MFramework::Instance->ViewportManager
        ->ActiveViewport;
    MEntity* pmCamera = pmActiveViewport->CameraEntity;
    NiCamera* pkCamera = pmActiveViewport->GetNiCamera();

    if (SelectionService->NumSelectedEntities > 0)
    {
        NiPoint3 kCenter;
        SelectionService->SelectionCenter->ToNiPoint3(kCenter);
        NiPoint3 kDelta = pkCamera->GetTranslate() - kCenter;
        if ((kDelta.x > 0.1f) || (kDelta.x < -0.1f) ||
            (kDelta.y > 0.1f) || (kDelta.y < -0.1f) ||
            (kDelta.z > 0.1f) || (kDelta.z < -0.1f))
        {
            *m_pkOrbitCenter = kCenter;
            return;
        }
    }

    // we will pick to figure the appropriate orbit distance
    NiPoint3 kOrigin, kDir;
    NiViewMath::MouseToRay((float) iX, (float) iY,
        MFramework::Instance->ViewportManager->ActiveViewport->Width,
        MFramework::Instance->ViewportManager->ActiveViewport->Height,
        MFramework::Instance->ViewportManager->ActiveViewport->GetNiCamera(),
        kOrigin, kDir);

    if (MFramework::Instance->PickUtility->PerformPick(
        MFramework::Instance->Scene, kOrigin, kDir, true))
    {
        const NiPick* pkPick = 
            MFramework::Instance->PickUtility->GetNiPick();
        const NiPick::Results& kPickResults = pkPick->GetResults();
        NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
        if (pkPickRecord)
        {
            *m_pkOrbitCenter = pkPickRecord->GetIntersection();
        }
    }
    else if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        NiBound kBound = 
            *(MFramework::Instance->BoundManager->GetSceneBound());
        *m_pkOrbitCenter = kBound.GetCenter();
    }
    else
    {
        *m_pkOrbitCenter = kOrigin + m_fDefaultOrbitDistance * kDir;
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::PanToSelection()
{
    MVerifyValidInstance;

    NiBound kBound;
    NiTPrimitiveArray<const NiBound*> kSelectionBounds;

    MEntity* amEntities[];
    if (SelectionService->NumSelectedEntities > 0)
    {
        amEntities = SelectionService->GetSelectedEntities();
    }
    else
    {
        amEntities = MFramework::Instance->Scene->GetEntities();
    }
    NiBound* akBounds = NiNew NiBound[amEntities->Count];
    for (int i = 0; i < amEntities->Count; i++)
    {
        if (amEntities[i] != NULL)
        {
            NiAVObject* pkScene = amEntities[i]->GetSceneRootPointer(0);
            if (pkScene)
            {
                pkScene->UpdateNodeBound();
                akBounds[i] = pkScene->GetWorldBound();
                if (akBounds[i].GetRadius() <= 0.0f)
                {
                    akBounds[i].SetRadius(m_fDefaultOrbitDistance);
                    akBounds[i].SetCenter(pkScene->GetTranslate());
                }
                kSelectionBounds.Add(&akBounds[i]);
            }
        }
    }
    if (kSelectionBounds.GetEffectiveSize() == 0)
    {
        kBound.SetCenterAndRadius(NiPoint3::ZERO, m_fDefaultOrbitDistance);
    }
    else
    {
        kBound.ComputeMinimalBound(kSelectionBounds);
    }
    NiDelete[] akBounds;

    NiMatrix3 kRotation;
    NiFrustum kFrustum;
    NiPoint3 kDestPoint;
    MEntity* pmCamera = MFramework::Instance->ViewportManager->
        ActiveViewport->CameraEntity;
    if (pmCamera != NULL)
    {
        NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
        pkEntity->GetPropertyData(*m_pkRotationName, kRotation);
    }
    kFrustum = MFramework::Instance->ViewportManager->ActiveViewport->
        GetNiCamera()->GetViewFrustum();
    kDestPoint = NiViewMath::PanTo(kBound, kRotation,
        kFrustum);
    if (kFrustum.m_bOrtho)
    {
        NiPoint3 kLook;
        kRotation.GetCol(0, kLook);
        float fAspect;
        fAspect = kFrustum.m_fRight / kFrustum.m_fTop;
        NiBound kToolBound = 
            *(MFramework::Instance->BoundManager->GetToolSceneBound(
            MFramework::Instance->ViewportManager->ActiveViewport));
        kDestPoint = kDestPoint - (kToolBound.GetRadius() + 
            (kDestPoint - kToolBound.GetCenter()).Dot(kLook)) * kLook;
        if (kBound.GetRadius() > 0.0f)
        {
            if (fAspect >= 1.0f)
            {
                // if we have a wide aspect, fit the top of the frustum to
                // the bounds
                kFrustum.m_fTop = kBound.GetRadius();
                kFrustum.m_fBottom = -kFrustum.m_fTop;
                kFrustum.m_fRight = kFrustum.m_fTop * fAspect;
                kFrustum.m_fLeft = -kFrustum.m_fRight;
            }
            else
            {
                // if we have a tall aspect, fit the right to the bounds
                kFrustum.m_fRight = kBound.GetRadius();
                kFrustum.m_fLeft = -kFrustum.m_fRight;
                kFrustum.m_fTop = kFrustum.m_fRight / fAspect;
                kFrustum.m_fBottom = -kFrustum.m_fTop;
            }
        }
    }
    MFramework::Instance->CameraManager->TransitionCamera(
        MFramework::Instance->ViewportManager->ActiveViewport, &kDestPoint, 
        &kRotation, &kFrustum);
}
//---------------------------------------------------------------------------
void MViewInteractionMode::LookAtSelection()
{
    MVerifyValidInstance;

    // rotate the view to look at selection
    NiMatrix3 kNewRotation;
    NiPoint3 kCenter;
    NiPoint3 kCameraPos;
    NiFrustum kFrustum;

    UserCameraSwitchHelper();
    SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    MEntity* pmCamera = MFramework::Instance->ViewportManager->
        ActiveViewport->CameraEntity;
    if (pmCamera != NULL)
    {
        NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
        pkEntity->GetPropertyData(*m_pkTranslationName, kCameraPos);
    }
    kFrustum = MFramework::Instance->ViewportManager->ActiveViewport->
        GetNiCamera()->GetViewFrustum();
    kNewRotation = NiViewMath::LookAt(kCenter, kCameraPos, *m_pkUpAxis);
    MFramework::Instance->CameraManager->TransitionCamera(
        MFramework::Instance->ViewportManager->ActiveViewport, &kCameraPos, 
        &kNewRotation, &kFrustum);
}
//---------------------------------------------------------------------------
void MViewInteractionMode::LookAtSelectionPalette()
{
	if (SelectionService->GetSelectedEntities()->Length != 1)
	{
		return;
	}
	MEntity *pkEntity = SelectionService->GetSelectedEntities()[0];
	if (!pkEntity)
	{
		return;
	}
	if (!pkEntity->MasterEntity)
	{
		return;
	}

	IEntityPathService *pathService = MGetService(IEntityPathService);
	MPalette *pkPalette = pathService->FindPaletteContainingEntity(pkEntity->MasterEntity);
	if (pkPalette)
	{
		MFramework::Instance->PaletteManager->ActivePalette = pkPalette;
		pkPalette->ActiveEntity = pkEntity->MasterEntity;
	}
}
//---------------------------------------------------------------------------
void MViewInteractionMode::AdjustOrthoDistance()
{
    // first, find the closest point between camera POV and bound center
    NiCamera* pkCamera;
    MViewport* pmPort = MFramework::Instance->ViewportManager->ActiveViewport;
    pkCamera = pmPort->GetNiCamera();

    NiPoint3 kOrigin;
    kOrigin = pkCamera->GetTranslate();

    NiMatrix3 kRotation;
    kRotation = pkCamera->GetRotate();

    NiPoint3 kCenter;
    kCenter = MFramework::Instance->BoundManager->GetToolSceneBound(pmPort)->
        GetCenter();

    float fRadius;
    fRadius = MFramework::Instance->BoundManager->GetToolSceneBound(pmPort)->
        GetRadius();

    NiPoint3 kDir;
    kRotation.GetCol(0, kDir);

    // project a pt onto the plane tangent to the sphere and our kDir
    NiPoint3 kTangentPoint;
    kTangentPoint = kCenter - fRadius * kDir;
    float fDistance = (kTangentPoint - kOrigin).Dot(kDir);
    NiPoint3 kDestination;
    kDestination = kOrigin + fDistance * kDir;
    // now move the camera to that location and adjust our orbit distance
    NiEntityInterface* pkEntity = pmPort->CameraEntity->GetNiEntityInterface();
    pkEntity->SetPropertyData(*m_pkTranslationName, kDestination);
}
//---------------------------------------------------------------------------
void MViewInteractionMode::CommitZoom()
{
    MVerifyValidInstance;
    // This method takes the changes in Zoom and makes a command out of them
    m_bZooming = false;

    if (!m_bOrthographic)
    {
        // first store the initial translation
        NiPoint3 kTranslate;
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->GetPropertyData(*m_pkTranslationName, 
                kTranslate);
            pkEntity->SetPropertyData(*m_pkTranslationName, 
                *m_pkStartPosition);

            bool bUndo;
            bUndo = !(MFramework::Instance->CameraManager->IsStandardCamera(
                MFramework::Instance->ViewportManager->ActiveViewport,
                pmCamera));

            CommandService->BeginUndoFrame("Viewport: Zoom view");
            pmCamera->SetPropertyData(*m_pkTranslationName, 
                new MPoint3(kTranslate), true);
            CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
        }
    }
    else
    {
        float fCurrentWidth;
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->GetPropertyData(*m_pkOrthoWidthName, 
                fCurrentWidth);
            pkEntity->SetPropertyData(*m_pkOrthoWidthName,
                m_fStartOrthoWidth);

            bool bUndo;
            bUndo = !(MFramework::Instance->CameraManager->IsStandardCamera(
                MFramework::Instance->ViewportManager->ActiveViewport,
                pmCamera));

            CommandService->BeginUndoFrame("Viewport: Zoom view");
            pmCamera->SetPropertyData(*m_pkOrthoWidthName,
                __box(fCurrentWidth), true);
            CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
        }
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::UserCameraSwitchHelper()
{
    MCameraManager* pmCameraManager = MFramework::Instance->
        CameraManager;
    MViewport* pmActiveViewport = MFramework::Instance->ViewportManager
        ->ActiveViewport;
    MEntity* pmCamera = pmActiveViewport->CameraEntity;
    NiCamera* pkCamera = pmActiveViewport->GetNiCamera();

    // this stores the camera we started from for undo purposes
    m_pmOriginalCamera = NULL;

    // if we are using a standard ortho cam, we must switch to user
    if (pmCameraManager->IsAxisAlignedStandardCamera(pmActiveViewport,
        pmCamera))
    {
        NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();

        NiPoint3 kTranslation;
        pkEntity->GetPropertyData(*m_pkTranslationName, kTranslation);
        NiMatrix3 kRotation;
        pkEntity->GetPropertyData(*m_pkRotationName, kRotation);
        float fWidth;
        pkEntity->GetPropertyData(*m_pkOrthoWidthName, fWidth);

        m_pmOriginalCamera = pmActiveViewport->CameraEntity;
        pmActiveViewport->SetCamera(pmCameraManager
            ->GetStandardCameraEntity(pmActiveViewport,
            MCameraManager::StandardCamera::User), false);
        pmCamera = pmActiveViewport->CameraEntity;
        pkCamera = pmActiveViewport->GetNiCamera();
        pkEntity = pmCamera->GetNiEntityInterface();

        pkEntity->SetPropertyData(*m_pkTranslationName, kTranslation);
        pkEntity->SetPropertyData(*m_pkRotationName, kRotation);
        pkEntity->SetPropertyData(*m_pkOrthoWidthName, fWidth);

        pkCamera->SetTranslate(kTranslation);
        pkCamera->SetRotate(kRotation);
    }

}
//---------------------------------------------------------------------------
void MViewInteractionMode::GetPickDistance(NiPoint3* pkOrigin, NiPoint3* pkDir)
{
    if (MFramework::Instance->PickUtility->PerformPick(
        MFramework::Instance->Scene, *pkOrigin, *pkDir, true))
    {
        const NiPick* pkPick = 
            MFramework::Instance->PickUtility->GetNiPick();
        const NiPick::Results& kPickResults = pkPick->GetResults();
        NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
        if (pkPickRecord)
        {
            m_bPickHit = true;
            m_fPickDistance = pkPickRecord->GetDistance();
        }
    }
    else
    {
        m_bPickHit = false;
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::PrepareClonedLight(MEntity* pmLight)
{
    MVerifyValidInstance;

    MAssert(MLightManager::EntityIsLight(pmLight), "Non-light entity "
        "provided to function!");

    // Remove and add back all of its affected entities so that they will
    // show up in the undo stack.

    MAssert(pmLight->HasProperty(ms_strAffectedEntitiesName), String::Format(
        "\"{0}\" property not found!", ms_strAffectedEntitiesName));
    unsigned int uiAffectedEntitiesCount = pmLight->GetElementCount(
        ms_strAffectedEntitiesName);

    // Store existing affected entities.
    MEntity* amAffectedEntities[] = new MEntity*[uiAffectedEntitiesCount];
    for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
    {
        amAffectedEntities[ui] = dynamic_cast<MEntity*>(
            pmLight->GetPropertyData(ms_strAffectedEntitiesName, ui));
    }

    // Clear affected entities. This is not undoable.
    for (unsigned int ui = 0; ui < uiAffectedEntitiesCount; ui++)
    {
        pmLight->SetPropertyData(ms_strAffectedEntitiesName, NULL, ui, false);
    }

    // Restore affected entities. This is undoable.
    for (int i = 0; i < amAffectedEntities->Length; i++)
    {
        pmLight->SetPropertyData(ms_strAffectedEntitiesName,
            amAffectedEntities[i], i, true);
    }

    // Update the light.
    pmLight->Update(MFramework::Instance->TimeManager->CurrentTime,
        MFramework::Instance->ExternalAssetManager);
}
//---------------------------------------------------------------------------
MViewInteractionMode::MViewInteractionMode() : m_pkOrthoWidthName(NULL),
    m_pkTranslationName(NULL), m_pkRotationName(NULL), m_pkScaleName(NULL), 
    m_pkStaticName(NULL), m_fZoomStartTime(0.0f), m_fStartOrthoWidth(100.0f),
    m_bZooming(false), m_fDefaultOrbitDistance(100.0f),
    m_fMouseWheelScalar(0.1f), m_fPanScalar(0.5f), 
    m_fMouseLookScalar(0.01f), m_bViewUndoable(false), m_bRightDown(false), 
    m_bMiddleDown(false), m_bLeftDown(false), m_fLeftClickTime(0.0f),
    m_fMiddleClickTime(0.0f), m_fRightClickTime(0.0f), 
    m_fDoubleClickTime(0.2f), m_iLastX(0), m_iLastY(0)
{
    m_pkUpAxis = NiNew NiPoint3(0.0f, 0.0f, 1.0f);
    m_pkStartRotation = NiNew NiMatrix3;
    m_pkStartPosition = NiNew NiPoint3;
    m_pkOrbitCenter = NiNew NiPoint3();
    m_pkOrthoWidthName = NiNew NiFixedString("Orthographic Frustum Width");
    m_pkTranslationName = NiNew NiFixedString("Translation");
    m_pkRotationName = NiNew NiFixedString("Rotation");
    m_pkScaleName = NiNew NiFixedString("Scale");
    m_pkStaticName = NiNew NiFixedString("Static");
    m_pkNearClipName = NiNew NiFixedString("Near Clipping Plane");
    m_pkFarClipName = NiNew NiFixedString("Far Clipping Plane");

    __hook(&MEventManager::NewSceneLoaded, MFramework::Instance->EventManager,
        &MViewInteractionMode::OnNewSceneLoaded);
}
//---------------------------------------------------------------------------
void MViewInteractionMode::Do_Dispose(bool bDisposing)
{
    NiDelete m_pkUpAxis;
    NiDelete m_pkStartRotation;
    NiDelete m_pkStartPosition;
    NiDelete m_pkOrbitCenter;
    NiDelete m_pkOrthoWidthName;
    NiDelete m_pkTranslationName;
    NiDelete m_pkRotationName;
    NiDelete m_pkScaleName;
    NiDelete m_pkStaticName;
    NiDelete m_pkNearClipName;
    NiDelete m_pkFarClipName;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::RegisterSettings()
{
    // register the event handler for changed settings
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MViewInteractionMode::OnSettingChanged);

    // uncomment these lines to expose up axis as a settable option
    // this may expose bugs as alternate up axes are not officially supported
    //MSettingsHelper::GetStandardSetting(MSettingsHelper::UP_AXIS, 
    //    *m_pkUpAxis, pmHandler);
    
    MSettingsHelper::GetStandardSetting(MSettingsHelper::ORBIT_DISTANCE, 
        m_fDefaultOrbitDistance, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::DOLLY_SCALAR, 
        m_fMouseWheelScalar, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::PAN_SCALAR, 
        m_fPanScalar, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::MOUSE_LOOK_SCALAR,
        m_fMouseLookScalar, pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::VIEW_UNDOABLE,
        m_bViewUndoable, pmHandler);
}
//---------------------------------------------------------------------------
void MViewInteractionMode::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::UP_AXIS))
    {
        Object* pmObj;
        MPoint3* pmUpAxis;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        pmUpAxis = dynamic_cast<MPoint3*>(pmObj);
        if (pmUpAxis != NULL)
        {
            pmUpAxis->ToNiPoint3(*m_pkUpAxis);
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ORBIT_DISTANCE))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fDefaultOrbitDistance = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::DOLLY_SCALAR))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fMouseWheelScalar = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::PAN_SCALAR))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fPanScalar = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::MOUSE_LOOK_SCALAR))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fMouseLookScalar = *pfVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::VIEW_UNDOABLE))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bViewUndoable = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::OnNewSceneLoaded(MScene* pmScene)
{
    m_fLeftClickTime = 0.0f;
    m_fMiddleClickTime = 0.0f;
    m_fRightClickTime = 0.0f;
}
//---------------------------------------------------------------------------
IMessageService* MViewInteractionMode::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
IInteractionModeService* MViewInteractionMode::get_InteractionModeService()
{
    if (ms_pmInteractionModeService == NULL)
    {
        ms_pmInteractionModeService = MGetService(IInteractionModeService);
        MAssert(ms_pmInteractionModeService != NULL, "Interaction mode "
            "service not found!");
    }
    return ms_pmInteractionModeService;
}
//---------------------------------------------------------------------------
ISelectionService* MViewInteractionMode::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ICommandService* MViewInteractionMode::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found.");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
ISettingsService* MViewInteractionMode::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MViewInteractionMode::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found.");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
IEntityPathService* MViewInteractionMode::get_EntityPathService()
{
    if (ms_psEntityPathService == NULL)
    {
        ms_psEntityPathService = MGetService(IEntityPathService);
        MAssert(ms_psEntityPathService != NULL, 
            "Entity Path service not found.");
    }
    return ms_psEntityPathService;
}
//---------------------------------------------------------------------------
IRenderingModeService* MViewInteractionMode::get_RenderingModeService()
{
    if (ms_pmRenderingModeService == NULL)
    {
        ms_pmRenderingModeService = MGetService(IRenderingModeService);
        MAssert(ms_pmRenderingModeService != NULL, "Rendering mode service "
            "not found.");
    }
    return ms_pmRenderingModeService;
}
//---------------------------------------------------------------------------
IRenderingMode* MViewInteractionMode::get_GhostRenderingMode()
{
    if (ms_pmGhostRenderingMode == NULL)
    {
        ms_pmGhostRenderingMode = RenderingModeService
            ->GetRenderingModeByName("Ghost");
        MAssert(ms_pmGhostRenderingMode != NULL, "Ghost rendering mode not "
            "found!");
    }
    return ms_pmGhostRenderingMode;
}
//---------------------------------------------------------------------------
String* MViewInteractionMode::get_Name()
{
    MVerifyValidInstance;

    return "GamebryoView";
}
//---------------------------------------------------------------------------
Cursor* MViewInteractionMode::get_MouseCursor()
{
    return Cursors::Default;
}
//---------------------------------------------------------------------------
bool MViewInteractionMode::Initialize()
{
    MVerifyValidInstance;

    // view interaction mode has no dependencies
    return true;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::Update(float fTime)
{
    MVerifyValidInstance;
    if (m_bZooming && (fTime > (m_fZoomStartTime + 0.5f)))
    {
        // if we zoomed, but aren't zooming anymore, make a command
        CommitZoom();
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseEnter()
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseLeave()
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseHover()
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseDown(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    float fCurrentTime = MFramework::Instance->TimeManager->CurrentTime;
    // if we were zooming via the mousewheel, commit the change
    if (m_bZooming)
    {
        CommitZoom();
    }

    m_iLastX = iX;
    m_iLastY = iY;
    m_iStartX = iX;
    m_iStartY = iY;

    if (eType == MouseButtonType::LeftButton)
    {
        m_bLeftDown = true;
        if ((fCurrentTime - m_fLeftClickTime) < m_fDoubleClickTime)
            DoubleClick(eType, iX, iY);
        m_fLeftClickTime = fCurrentTime;
    }
    else if (eType == MouseButtonType::RightButton || 
		(eType == MouseButtonType::MiddleButton && (GetAsyncKeyState(VK_MENU) & 0x8000)) )
    {
        m_bRightDown = true;
        if ((fCurrentTime - m_fRightClickTime) < m_fDoubleClickTime)
            DoubleClick(eType, iX, iY);
        m_fRightClickTime = fCurrentTime;

        // store the initial rotation of the camera
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->GetPropertyData(*m_pkRotationName, *m_pkStartRotation);
        }

        m_bRotating = false;
    }
    else if (eType == MouseButtonType::MiddleButton)
    {
        m_bMiddleDown = true;
        if ((fCurrentTime - m_fMiddleClickTime) < m_fDoubleClickTime)
            DoubleClick(eType, iX, iY);
        m_fMiddleClickTime = fCurrentTime;

        // store the initial position of the camera
        MEntity* pmCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->CameraEntity;
        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->GetPropertyData(*m_pkTranslationName, 
                *m_pkStartPosition);
        }

        NiCamera* pkCamera = MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera();
        if (pkCamera->GetViewFrustum().m_bOrtho)
        {
            AdjustOrthoDistance();
        }

        if (m_bRightDown)
        {
            SetupOrbit(iX, iY);
        }
        else
        {
            m_bPickHit = true;
            m_fPickDistance = MFramework::Instance->ViewportManager->ActiveViewport->
				GetNiCamera()->GetTranslate().Length();
			m_fPickDistance = abs(m_fPickDistance);

			// 항상 비율에 맞게 이동이 된다 : 원래버젼
            //NiPoint3 kOrigin, kDir;
            //NiViewMath::MouseToRay((float) iX, (float) iY,
            //    MFramework::Instance->ViewportManager->ActiveViewport->Width,
            //    MFramework::Instance->ViewportManager->ActiveViewport->Height,
            //    MFramework::Instance->ViewportManager->ActiveViewport->
            //    GetNiCamera(), kOrigin, kDir);
            //GetPickDistance(&kOrigin, &kDir);
}
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    // check if our transformation has changed 'significantly'
    bool bTranslationChanged = false;
    bool bRotationChanged = false;
    NiEntityInterface* pkEntity;
    NiPoint3 kTranslation;
    NiMatrix3 kRotation;
    NiPoint3 kDeltaTranslation;
    float fEulerX;
    float fEulerY;
    float fEulerZ;
    float fDeltaEulerX;
    float fDeltaEulerY;
    float fDeltaEulerZ;

    MEntity* pmCamera = MFramework::Instance->ViewportManager->
        ActiveViewport->CameraEntity;
    if (pmCamera != NULL)
    {
        pkEntity = pmCamera->GetNiEntityInterface();
        pkEntity->GetPropertyData(*m_pkTranslationName, kTranslation);
        pkEntity->GetPropertyData(*m_pkRotationName, kRotation);

        kDeltaTranslation = *m_pkStartPosition - kTranslation;
        if ((kDeltaTranslation.x > 0.01f) || (kDeltaTranslation.x < -0.01f) ||
            (kDeltaTranslation.y > 0.01f) || (kDeltaTranslation.y < -0.01f) ||
            (kDeltaTranslation.z > 0.01f) || (kDeltaTranslation.z < -0.01f))
        {
            bTranslationChanged = true;
        }

        m_pkStartRotation->ToEulerAnglesXYZ(fEulerX, fEulerY, fEulerZ);
        kRotation.ToEulerAnglesXYZ(fDeltaEulerX, fDeltaEulerY, fDeltaEulerZ);
        fDeltaEulerX = fDeltaEulerX - fEulerX;
        fDeltaEulerY = fDeltaEulerY - fEulerY;
        fDeltaEulerZ = fDeltaEulerZ - fEulerZ;
        if ((fDeltaEulerX > 0.01f) || (fDeltaEulerX < -0.01f) ||
            (fDeltaEulerY > 0.01f) || (fDeltaEulerY < -0.01f) ||
            (fDeltaEulerZ > 0.01f) || (fDeltaEulerZ < -0.01f))
        {
            bRotationChanged = true;
        }
    }

    NiCamera* pkCamera = MFramework::Instance->ViewportManager->ActiveViewport
        ->GetNiCamera();
    assert(pkCamera);
    if ((eType == MouseButtonType::RightButton && m_bMiddleDown) ||
        (eType == MouseButtonType::MiddleButton && m_bRightDown) ||
        (eType == MouseButtonType::RightButton && m_bRightDown &&
            pkCamera->GetViewFrustum().m_bOrtho) && (bRotationChanged))
    {
        // we have completed a mouse orbit, register the translation and
        // rotation with the camera entity
        CommandService->BeginUndoFrame("Viewport: Orbit view");

        // first set the original values on the interface so the undo
        // can record the original and changed state
        pkEntity->SetPropertyData(*m_pkRotationName, *m_pkStartRotation);
        pkEntity->SetPropertyData(*m_pkTranslationName, 
            *m_pkStartPosition);
        if (m_pmOriginalCamera != NULL)
        {
            MFramework::Instance->ViewportManager->ActiveViewport
                ->SetCamera(m_pmOriginalCamera, false);
        }

        // set the entity properties so an undo-able command is issued
        pmCamera->SetPropertyData(*m_pkTranslationName, new 
            MPoint3(kTranslation), true);
        pmCamera->SetPropertyData(*m_pkRotationName, 
            new MMatrix3(kRotation), true);
        MFramework::Instance->ViewportManager->ActiveViewport
            ->SetCamera(pmCamera, true);

        bool bUndo;
        bUndo = !(MFramework::Instance->CameraManager->IsStandardCamera(
            MFramework::Instance->ViewportManager->ActiveViewport, pmCamera));

        CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
        // to prevent registering two commands, release the buttons states
        m_bMiddleDown = false;
        m_bRightDown = false;
    }
    else if (eType == MouseButtonType::RightButton && m_bRightDown && 
        bRotationChanged)
    {
        CommandService->BeginUndoFrame("Viewport: Mouse look");

        // first set the original values on the interface so the undo
        // can record the original and changed state
        pkEntity->SetPropertyData(*m_pkRotationName, *m_pkStartRotation);

        // set the entity properties so an undoable command is issued
        pmCamera->SetPropertyData(*m_pkRotationName, 
            new MMatrix3(kRotation), true);

        bool bUndo;
        bUndo = !(MFramework::Instance->CameraManager->IsStandardCamera(
            MFramework::Instance->ViewportManager->ActiveViewport, pmCamera));

        CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
    }
    else if (eType == MouseButtonType::MiddleButton && m_bMiddleDown && 
        bTranslationChanged)
    {
        CommandService->BeginUndoFrame("Viewport: Pan view");
        // first set the original values on the interface so the undo
        // can record the original and changed state
        pkEntity->SetPropertyData(*m_pkTranslationName, 
            *m_pkStartPosition);

        // set the entity properties so an undoable command is issued
        pmCamera->SetPropertyData(*m_pkTranslationName, new 
            MPoint3(kTranslation), true);

        bool bUndo;
        bUndo = !(MFramework::Instance->CameraManager->IsStandardCamera(
            MFramework::Instance->ViewportManager->ActiveViewport, pmCamera));

        CommandService->EndUndoFrame(bUndo || m_bViewUndoable);
    }

    if (eType == MouseButtonType::LeftButton)
    {
        m_bLeftDown = false;
    }
    else if (eType == MouseButtonType::RightButton)
    {
        m_bRightDown = false;
    }
    else if (eType == MouseButtonType::MiddleButton)
    {
        m_bMiddleDown = false;
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseMove(int iX, int iY)
{
    MVerifyValidInstance;

	if (iX == -999999 && iY == -999999)
	{
		PanToSelection();
		return;
	}

    NiCamera* pkCamera;
    MEntity* pmCamera;
    int iDX;
    int iDY;

    pkCamera = MFramework::Instance->ViewportManager->ActiveViewport
        ->GetNiCamera();
    pmCamera = MFramework::Instance->ViewportManager->ActiveViewport->
        CameraEntity;
    m_bOrthographic = pkCamera->GetViewFrustum().m_bOrtho;
    iDX = iX - m_iLastX;
    iDY = iY - m_iLastY;
    m_iLastX = iX;
    m_iLastY = iY;

    if (m_bRightDown)
    {
        if ((!m_bRotating) && ((m_iStartX != iX) || (m_iStartY != iY)))
        {
            // if this is our first pixel move
            NiCamera* pkCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->GetNiCamera();
            UserCameraSwitchHelper();
            if (pkCamera->GetViewFrustum().m_bOrtho)
            {
                AdjustOrthoDistance();
                SetupOrbit(iX, iY);
            }

            if (m_bMiddleDown)
            {
                SetupOrbit(iX, iY);
            }
            m_bRotating = true;
        }
        else if (m_bRotating)
        {
            if ((m_bMiddleDown) || (m_bOrthographic))
            {
                NiPoint3 kNewLocation;
                NiMatrix3 kNewRotation;
                NiViewMath::Orbit((float)iDX * m_fMouseLookScalar, 
                    (float)iDY * m_fMouseLookScalar, pkCamera->GetTranslate(), 
                    pkCamera->GetRotate(), *m_pkOrbitCenter, *m_pkUpAxis, 
                    kNewLocation, kNewRotation);
                if (pmCamera != NULL)
                {
                    NiEntityInterface* pkEntity = 
                        pmCamera->GetNiEntityInterface();
                    pkEntity->SetPropertyData(*m_pkTranslationName, 
                        kNewLocation);
                    pkEntity->SetPropertyData(*m_pkRotationName, kNewRotation);
                }
            }
            else
            {
                // if only right is pressed, do mouselook

                // if we are using a standard ortho cam, we must switch to user
                MCameraManager* pmCameraManager = MFramework::Instance->
                    CameraManager;
                MViewport* pmActiveViewport = 
                    MFramework::Instance->ViewportManager->ActiveViewport;
                NiMatrix3 kNewRotation;
                kNewRotation = NiViewMath::Look((float)iDX * 
                    m_fMouseLookScalar, (float)iDY * m_fMouseLookScalar, 
                    pkCamera->GetRotate(), *m_pkUpAxis);

                if (pmCamera != NULL)
                {
                    NiEntityInterface* pkEntity = 
                        pmCamera->GetNiEntityInterface();
                    pkEntity->SetPropertyData(*m_pkRotationName, kNewRotation);
                }
            }
        }
    }
    else if (m_bMiddleDown)
    {
        // if only middle is pressed, pan view
        NiPoint3 kNewTranslation;
        float fPanSpeedX, fPanSpeedY;
        NiFrustum kFrustum = MFramework::Instance->ViewportManager->
            ActiveViewport->GetNiCamera()->GetViewFrustum();
        if ((m_bPickHit) && (!kFrustum.m_bOrtho))
        {
            // if there was a pick result, pan proportional to the distance
            fPanSpeedX = (float)(iDX) * m_fPickDistance * 2.0f / 
                MFramework::Instance->ViewportManager->ActiveViewport->Width;
            fPanSpeedY = (float)(iDY) * m_fPickDistance * 2.0f / 
                MFramework::Instance->ViewportManager->ActiveViewport->Height;
        }
        else if (m_bPickHit)
        {
            // if there was a pick result in ortho pan proportional to
            // the frustum width
            fPanSpeedX = (float)(iDX) * kFrustum.m_fRight * 2.0f / 
                MFramework::Instance->ViewportManager->ActiveViewport->Width;
            fPanSpeedY = (float)(iDY) * kFrustum.m_fTop * 2.0f / 
                MFramework::Instance->ViewportManager->ActiveViewport->Height;
        }
        else
        {
            fPanSpeedX = (float)(iDX) * m_fPanScalar;
            fPanSpeedY = (float)(iDY) * m_fPanScalar;
        }
        kNewTranslation = NiViewMath::Pan(fPanSpeedX, fPanSpeedY,
            pkCamera->GetTranslate(), pkCamera->GetRotate());

        if (pmCamera != NULL)
        {
            NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
            pkEntity->SetPropertyData(*m_pkTranslationName, 
                kNewTranslation);
        }
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::MouseWheel(int iDelta)
{
    MVerifyValidInstance;

	if(MFramework::Instance->CameraManager->IsSceneCamera())//!/
		return;


    if (!m_bRightDown && !m_bMiddleDown)
    {
        NiCamera* pkCamera;

        pkCamera = MFramework::Instance->ViewportManager->ActiveViewport
            ->GetNiCamera();
        m_bOrthographic = pkCamera->GetViewFrustum().m_bOrtho;

        if (!m_bOrthographic)
        {
            // first store the initial translation
            NiPoint3 kTranslate;
            float fDollySpeed;
            NiPoint3 kOrigin, kDir;
            kOrigin = pkCamera->GetTranslate();
            kDir = pkCamera->GetWorldDirection();
            m_bPickHit = false;
            if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000))
                GetPickDistance(&kOrigin, &kDir);
            if (m_bPickHit)
            {
                // if we picked, make the speed proportional to distance
                fDollySpeed = m_fPickDistance * (1.0f - NiPow(0.999f, 
                    (float)(iDelta)));
            }
            else
            {
                fDollySpeed = (float)(iDelta) * m_fMouseWheelScalar;
                if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
                    fDollySpeed *= 5.0f;
            }
            kTranslate = NiViewMath::Dolly(fDollySpeed, 
                pkCamera->GetTranslate(), pkCamera->GetRotate());

            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
                if (!m_bZooming)
                {
                    // if we are not already zooming, store the initial 
                    // translation state
                    pkEntity->GetPropertyData(*m_pkTranslationName, 
                        *m_pkStartPosition);
                }
                pkEntity->SetPropertyData(*m_pkTranslationName, kTranslate);
            }
        }
        else
        {
            NiFrustum kFrustum;
            kFrustum = NiViewMath::OrthoZoom((float)iDelta * 
                m_fMouseWheelScalar, pkCamera->GetViewFrustum());

            MEntity* pmCamera = MFramework::Instance->ViewportManager->
                ActiveViewport->CameraEntity;
            if (pmCamera != NULL)
            {
                NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
                if (!m_bZooming)
                {
                    // if we are not already zooming, store the initial 
                    // ortho view width state
                    float fOrthoWidth;
                    pkEntity->GetPropertyData(*m_pkOrthoWidthName, 
                        fOrthoWidth);
                    m_fStartOrthoWidth = fOrthoWidth;
                }
                pkEntity->SetPropertyData(*m_pkOrthoWidthName,
                    (kFrustum.m_fRight - kFrustum.m_fLeft));
            }
        }

        m_bZooming = true;
        m_fZoomStartTime = MFramework::Instance->TimeManager->CurrentTime;
    }
}
//---------------------------------------------------------------------------
void MViewInteractionMode::DoubleClick(MouseButtonType eType, int iX, int iY)
{
    MVerifyValidInstance;

    if ((eType == MouseButtonType::MiddleButton) &&
        (!MFramework::Instance->CameraManager->Transitioning))
    {
        m_bMiddleDown = false;
        PanToSelection();
    }
    else if (eType == MouseButtonType::RightButton)
    {
        if ((SelectionService->NumSelectedEntities > 0) && 
            (!MFramework::Instance->CameraManager->Transitioning))
        {
            m_bRightDown = false;
            LookAtSelection();
        }
    }
	else if (eType == MouseButtonType::LeftButton)
	{
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000 &&
			SelectionService->NumSelectedEntities == 1)
		{
			// 선택한 물체의 팔레트를 보여주기.
			LookAtSelectionPalette();
		}
	}
}
//---------------------------------------------------------------------------
void MViewInteractionMode::DoubleClick()
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
