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

#include "MToolbarListener.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace System::Windows::Forms;

//---------------------------------------------------------------------------
MToolbarListener::MToolbarListener() : m_fLightOptimizeThreshold(0.05f)
{
    m_pkTranslationName = NiNew NiFixedString("Translation");
    m_pkRotationName = NiNew NiFixedString("Rotation");
    m_pkStaticName = NiNew NiFixedString("Static");
    m_pkOrthoWidthName = NiNew NiFixedString("Orthographic Frustum Width");
    m_pkFOVName = NiNew NiFixedString("Field of View");
    m_pkAspectName = NiNew NiFixedString("Aspect Ratio");
    m_pkOrthoName = NiNew NiFixedString("Orthographic");
    m_pkNearClipName = NiNew NiFixedString("Near Clipping Plane");
    m_pkFarClipName = NiNew NiFixedString("Far Clipping Plane");
    m_pkLightTypeName = NiNew NiFixedString("Light Type");
    m_pkAttenuationConstantName = NiNew NiFixedString(
        "Attenuation (Constant)");
    m_pkAttenuationLinearName = NiNew NiFixedString("Attenuation (Linear)");
    m_pkAttenuationQuadraticName = NiNew NiFixedString(
        "Attenuation (Quadratic)");
    m_pkOuterSpotAngleName = NiNew NiFixedString("Spot Angle (Outer)");
    m_pkInnerSpotAngleName = NiNew NiFixedString("Spot Angle (Inner)");
    m_pkSpotExponentName = NiNew NiFixedString("Spot Exponent");
    m_pkLightDirectionName = NiNew NiFixedString("Direction");
    m_pkAffectedEntitiesName = NiNew NiFixedString("Affected Entities");
    m_pkSceneRootName = NiNew NiFixedString("Scene Root Pointer");

    m_pkUpAxis = NiNew NiPoint3(0.0f, 0.0f, 1.0f);
}
//---------------------------------------------------------------------------
void MToolbarListener::RegisterSettings()
{
    MVerifyValidInstance;

    // register the event handler for changed settings
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MToolbarListener::OnSettingChanged);

	MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED, m_bTranslateSnapEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::ROTATION_SNAP_ENABLED,
        m_bRotateSnapEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::SCALE_SNAP_ENABLED,
        m_bScaleSnapEnabled, pmHandler);

    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED, 
        m_bTranslatePrecisionEnabled, pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SCALE_PRECISION_ENABLED, m_bScalePrecisionEnabled, 
        pmHandler);

    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED, m_bAlignToSurfaceEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED, m_bSnapToSurfaceEnabled, 
        pmHandler);
    MSettingsHelper::GetStandardSetting(
        MSettingsHelper::SNAP_TO_POINT_ENABLED, m_bSnapToPointEnabled, 
        pmHandler);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::VIEW_UNDOABLE,
        m_bViewUndoable, pmHandler);
    // uncomment these lines to expose up axis as a settable option
    // this may expose bugs as alternate up axes are not officially supported
    //MSettingsHelper::GetStandardSetting(MSettingsHelper::UP_AXIS,
    //    *m_pkUpAxis, pmHandler);
    MSettingsHelper::GetStandardSetting(MSettingsHelper::ORBIT_DISTANCE, 
        m_fDefaultOrbitDistance, pmHandler);

	MSettingsHelper::RegisterSetting(ms_strLightThresholdName, 
        ms_strLightThresholdDescription, ms_strLightThresholdOption, 
        m_fLightOptimizeThreshold, SettingsCategory::PerUser, true, pmHandler);
}
//---------------------------------------------------------------------------
void MToolbarListener::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bTranslateSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ROTATION_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bRotateSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_SNAP_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bScaleSnapEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bAlignToSurfaceEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToSurfaceEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SNAP_TO_POINT_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bSnapToPointEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bTranslatePrecisionEnabled = *pbVal;
        }
    }
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SCALE_PRECISION_ENABLED))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bScalePrecisionEnabled = *pbVal;
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
    else if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
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
    else if (strSetting->Equals(ms_strLightThresholdName) && 
        (eCategory == SettingsCategory::PerUser))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fLightOptimizeThreshold = *pfVal;
        }
    }
}
//---------------------------------------------------------------------------
void MToolbarListener::ZoomExtentsHelper(MViewport* pmViewport, 
    NiBound* pkBound, bool bTransition)
{
    NiMatrix3 kRotation;
    NiFrustum kFrustum;
    NiPoint3 kDestPoint;
    MEntity* pmCamera = pmViewport->CameraEntity;
    if (pmCamera != NULL)
    {
        NiEntityInterface* pkEntity = pmCamera->GetNiEntityInterface();
        pkEntity->GetPropertyData(*m_pkRotationName, kRotation);
    }
    kFrustum = pmViewport->GetNiCamera()->GetViewFrustum();
    kDestPoint = NiViewMath::PanTo(*pkBound, kRotation, kFrustum);

    NiBound kBoundWithGrid = 
        *(MFramework::Instance->BoundManager->GetToolSceneBound(pmViewport));
    if (kFrustum.m_bOrtho)
    {
        NiPoint3 kLook;
        kRotation.GetCol(0, kLook);
        float fAspect;
        fAspect = kFrustum.m_fRight / kFrustum.m_fTop;

        // add the grid if it is static
        kDestPoint = kDestPoint - (kBoundWithGrid.GetRadius() + 
            (kDestPoint - kBoundWithGrid.GetCenter()).Dot(kLook)) * kLook;
        if (pkBound->GetRadius() > 0.0f)
        {
            if (fAspect >= 1.0f)
            {
                // if we have a wide aspect, fit the top of the frustum to
                // the bounds
                kFrustum.m_fTop = pkBound->GetRadius();
                kFrustum.m_fBottom = -kFrustum.m_fTop;
                kFrustum.m_fRight = kFrustum.m_fTop * fAspect;
                kFrustum.m_fLeft = -kFrustum.m_fRight;
            }
            else
            {
                // if we have a tall aspect, fit the right to the bounds
                kFrustum.m_fRight = pkBound->GetRadius();
                kFrustum.m_fLeft = -kFrustum.m_fRight;
                kFrustum.m_fTop = kFrustum.m_fRight / fAspect;
                kFrustum.m_fBottom = -kFrustum.m_fTop;
            }
        }
    }

    if (bTransition)
    {
        MFramework::Instance->CameraManager->TransitionCamera(pmViewport, 
            &kDestPoint, &kRotation, &kFrustum);
    }
    else
    {
        pmCamera->SetPropertyData(*m_pkTranslationName, new 
            MPoint3(kDestPoint), 0, true);
        pmCamera->SetPropertyData(*m_pkRotationName, new 
            MMatrix3(kRotation), 0, true);
        float fFOV = (NiASin(kFrustum.m_fRight) * 2.0f) * 360.0f /
            NI_TWO_PI;
        float fAspect = (kFrustum.m_fRight / kFrustum.m_fTop);
        float fWidth = (kFrustum.m_fRight - kFrustum.m_fLeft);
        pmCamera->SetPropertyData(*m_pkFOVName, __box(fFOV), 0, true);
        pmCamera->SetPropertyData(*m_pkAspectName, __box(fAspect), 0, 
            true);
        pmCamera->SetPropertyData(*m_pkOrthoWidthName, __box(fWidth), 0, 
            true);
        pmCamera->SetPropertyData(*m_pkOrthoName,
            __box(kFrustum.m_bOrtho), 0, true);
        pmCamera->SetPropertyData(*m_pkNearClipName, 
            __box(kFrustum.m_fNear), 0, true);
        pmCamera->SetPropertyData(*m_pkFarClipName,
            __box(kFrustum.m_fFar), 0, true);
    }
}
//---------------------------------------------------------------------------
void MToolbarListener::DisablePhysXPickSnapHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

	m_bDisablePhysXPickEnabled = !m_bDisablePhysXPickEnabled;
	MFramework::Instance->PickUtility->SetObserveAppCullFlag(m_bDisablePhysXPickEnabled);
}
//---------------------------------------------------------------------------
void MToolbarListener::TranslateSnapHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bTranslateSnapEnabled = !m_bTranslateSnapEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED), 
        __box(m_bTranslateSnapEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::TRANSLATION_SNAP_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::RotateSnapHandler(Object* pmObject, EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bRotateSnapEnabled = !m_bRotateSnapEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::ROTATION_SNAP_ENABLED), 
        __box(m_bRotateSnapEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::ROTATION_SNAP_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::ScaleSnapHandler(Object* pmObject, EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bScaleSnapEnabled = !m_bScaleSnapEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::SCALE_SNAP_ENABLED), 
        __box(m_bScaleSnapEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::SCALE_SNAP_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::TranslatePrecisionHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bTranslatePrecisionEnabled = !m_bTranslatePrecisionEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED), 
        __box(m_bTranslatePrecisionEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::TRANSLATION_PRECISION_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::ScalePrecisionHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bScalePrecisionEnabled = !m_bScalePrecisionEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::SCALE_PRECISION_ENABLED), 
        __box(m_bScalePrecisionEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::SCALE_PRECISION_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::SnapToSurfaceHandler(Object* pmObject, EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bSnapToSurfaceEnabled = !m_bSnapToSurfaceEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED), 
        __box(m_bSnapToSurfaceEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::SNAP_TO_SURFACE_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::SnapToPointHandler(Object* pmObject, EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bSnapToPointEnabled = !m_bSnapToPointEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::SNAP_TO_POINT_ENABLED), 
        __box(m_bSnapToPointEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::SNAP_TO_POINT_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::AlignToSurfaceHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    m_bAlignToSurfaceEnabled = !m_bAlignToSurfaceEnabled;
    SettingsService->SetSettingsObject(
        MSettingsHelper::GetSettingName(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED), 
        __box(m_bAlignToSurfaceEnabled), 
        MSettingsHelper::GetSettingCategory(
        MSettingsHelper::ALIGN_TO_SURFACE_ENABLED));
}
//---------------------------------------------------------------------------
void MToolbarListener::LookAtSelectionHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    if ((SelectionService->NumSelectedEntities > 0) && 
        (!MFramework::Instance->CameraManager->Transitioning))
    {
        // rotate the view to look at selection
        MCameraManager* pmCameraManager = MFramework::Instance->
            CameraManager;
        MViewport* pmActiveViewport = MFramework::Instance->ViewportManager
            ->ActiveViewport;
        MEntity* pmCamera = pmActiveViewport->CameraEntity;
        NiCamera* pkCamera = pmActiveViewport->GetNiCamera();

        NiMatrix3 kNewRotation;
        NiPoint3 kCenter;
        NiPoint3 kCameraPos;
        NiFrustum kFrustum;

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

            bool bViewUndoable = true;
            __box bool* pbViewUndoable = dynamic_cast<__box bool*>(
                SettingsService->GetSettingsObject(
                MSettingsHelper::GetSettingName(
                MSettingsHelper::VIEW_UNDOABLE),
                MSettingsHelper::GetSettingCategory(
                MSettingsHelper::VIEW_UNDOABLE)));
            if (pbViewUndoable != NULL)
            {
                bViewUndoable = *pbViewUndoable;
            }

            pmActiveViewport->SetCamera(pmCameraManager
                ->GetStandardCameraEntity(pmActiveViewport,
                MCameraManager::StandardCamera::User), bViewUndoable);
            pmCamera = pmActiveViewport->CameraEntity;
            pkCamera = pmActiveViewport->GetNiCamera();
            pkEntity = pmCamera->GetNiEntityInterface();

            pkEntity->SetPropertyData(*m_pkTranslationName, kTranslation);
            pkEntity->SetPropertyData(*m_pkRotationName, kRotation);
            pkEntity->SetPropertyData(*m_pkOrthoWidthName, fWidth);

            pkCamera->SetTranslate(kTranslation);
            pkCamera->SetRotate(kRotation);
        }

        SelectionService->SelectionCenter->ToNiPoint3(kCenter);

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
}
//---------------------------------------------------------------------------
void MToolbarListener::MoveToSelectionHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    if (!MFramework::Instance->CameraManager->Transitioning)
    {
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

        ZoomExtentsHelper(MFramework::Instance->ViewportManager->
            ActiveViewport, &kBound, true);
    }
}
//---------------------------------------------------------------------------
void MToolbarListener::ZoomExtentsAllHandler(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    // first compute the bound
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

    CommandService->BeginUndoFrame("Zoom Extents All");
    for (unsigned int ui = 0; ui < 
        MFramework::Instance->ViewportManager->ViewportCount; ui++)
    {
        MViewport* pmCurrentViewport;
        pmCurrentViewport = 
            MFramework::Instance->ViewportManager->GetViewport(ui);
        if (MFramework::Instance->CameraManager->IsStandardCamera(
            pmCurrentViewport, pmCurrentViewport->CameraEntity))
        {
            ZoomExtentsHelper(pmCurrentViewport, &kBound, false);
        }
    }
    CommandService->EndUndoFrame(m_bViewUndoable);
}
//---------------------------------------------------------------------------
void MToolbarListener::OptimizeLightHandler(Object* pmObject, EventArgs* mArgs)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame("Optimize Selected Lights");

    MEntity* amSelectedEntities[] = SelectionService->GetSelectedEntities();
    ArrayList* pmSelectedLights = new ArrayList(amSelectedEntities->Length);
    for (int i = 0; i < amSelectedEntities->Count; i++)
    {
        AddLightsToArray(amSelectedEntities[i], pmSelectedLights);
    }

    for (int i = 0; i < pmSelectedLights->Count; i++)
    {
        MEntity* pmLight = static_cast<MEntity*>(pmSelectedLights->Item[i]);

        // check type of light
        String* strType = dynamic_cast<String*>(pmLight->GetPropertyData(
            *m_pkLightTypeName));
        if (strType->Equals(NiLightComponent::LT_POINT) || 
            strType->Equals(NiLightComponent::LT_SPOT))
        {
            // if point light, iterate over all entities and get their 
            // bounds. check if the closest point on the bounds is within 
            // the attenuation threshold

            NiEntityInterface* pkLightEntity;
            pkLightEntity = pmLight->GetNiEntityInterface();
            NiAVObject* pkLightRoot;
            pkLightRoot = pmLight->GetSceneRootPointer(0);
            float fConstantAttenuation;
            pkLightEntity->GetPropertyData(*m_pkAttenuationConstantName,
                fConstantAttenuation);
            float fLinearAttenuation;
            pkLightEntity->GetPropertyData(*m_pkAttenuationLinearName,
                fLinearAttenuation);
            float fQuadraticAttenuation;
            pkLightEntity->GetPropertyData(
                *m_pkAttenuationQuadraticName, fQuadraticAttenuation);
            
            // clear out the light's affected entity list
            unsigned int uiInitialCount;
            unsigned int uiFinalCount;
            pkLightEntity->GetElementCount(*m_pkAffectedEntitiesName, 
                uiInitialCount);
            for (unsigned int j = 0; j < uiInitialCount; j++)
            {
                pmLight->SetPropertyData(*m_pkAffectedEntitiesName, 
                    (MEntity*)NULL, j, true);
            }

            MEntity* amEntities[];
            amEntities = MFramework::Instance->Scene->GetEntities();
            for (int j = 0; j < amEntities->Count; j++)
            {
                if ((MFramework::Instance->LightManager->EntityIsLight(
                    amEntities[j])) || 
                    (MFramework::Instance->CameraManager->EntityIsCamera(
                    amEntities[j])) ||
                    !(amEntities[j]->HasProperty(*m_pkSceneRootName)))
                { 
                    continue;
                }

                NiBound kBound;
                NiTPrimitiveArray<const NiBound*> kBoundArray;
                
                int iRootCount = 0;
                iRootCount = amEntities[j]->GetElementCount(
                    *m_pkSceneRootName);
                for (int k = 0; k < iRootCount; k++)
                {
                    NiAVObject* pkSceneRoot;
                    pkSceneRoot = amEntities[j]->GetSceneRootPointer(k);
                    if (pkSceneRoot)
                    {
                        kBoundArray.Add(&pkSceneRoot->GetWorldBound());
                    }
                }
                kBound.ComputeMinimalBound(kBoundArray);

                float fDistanceToBound;
                fDistanceToBound = (pkLightRoot->GetTranslate() - 
                    kBound.GetCenter()).Length() - kBound.GetRadius();
                fDistanceToBound = (fDistanceToBound > 0) ? 
                    fDistanceToBound : 0.0f;

                float fAttenuation = 0;
				float fDenominator = (fConstantAttenuation + 
					fLinearAttenuation * fDistanceToBound + 
					fQuadraticAttenuation * fDistanceToBound * fDistanceToBound);
				if(fDenominator > 0)
				{
					fAttenuation = 1 / fDenominator;
				}

                // if we have a spotlight, we must attenuate more
                if (strType->Equals(NiLightComponent::LT_SPOT))
                {
                    NiPoint3 kDelta = kBound.GetCenter() - 
                        pkLightRoot->GetTranslate();
                    if (kDelta.SqrLength() > kBound.GetRadius() * 
                        kBound.GetRadius())
                    {
                        // collect the necessary spotlight parameters
                        // put them in radians as necessary
                        float fOuterSpotAngle;
                        pkLightEntity->GetPropertyData(
                            *m_pkOuterSpotAngleName, fOuterSpotAngle);
                        fOuterSpotAngle *= (NI_TWO_PI / 360.0f);
                        float fInnerSpotAngle;
                        pkLightEntity->GetPropertyData(
                            *m_pkInnerSpotAngleName, fInnerSpotAngle);
                        fInnerSpotAngle *= (NI_TWO_PI / 360.0f);
                        float fSpotExponent;
                        pkLightEntity->GetPropertyData(
                            *m_pkSpotExponentName, fSpotExponent);
                        NiPoint3 kLightDirection;
                        pkLightEntity->GetPropertyData(
                            *m_pkLightDirectionName, kLightDirection);

                        // fCos2 is the spot influence at the closest
                        // point on the bound to light direction
                        NiPoint3 kLightVector;
                        float fAlpha;
                        fAlpha = kLightDirection.Dot(kDelta);
                        kLightVector = pkLightRoot->GetTranslate() + 
                            kLightDirection * fAlpha;
                        kLightVector = kLightVector - kBound.GetCenter();
                        if (kLightVector.SqrLength() > kBound.GetRadius() *
                            kBound.GetRadius())
                        {
                            kLightVector *= kBound.GetRadius() / 
                                kLightVector.Length();
                        }
                        kLightVector = (kBound.GetCenter() + kLightVector)
                            - pkLightRoot->GetTranslate();
                        kLightVector.Unitize();
                        float fCos2 = kLightDirection.Dot(kLightVector);
                        float fCosInner = NiCos(fInnerSpotAngle);
                        float fCosOuter = NiCos(fOuterSpotAngle);
                        fCos2 = ((fCos2 - fCosOuter) / 
                            (fCosInner - fCosOuter));
                        if (fCos2 < 0.0f)
                            fCos2 = 0.0f;
                        if (fCos2 > 1.0f)
                            fCos2 = 1.0f;

                        // fCos1 is the spot influence at the closest
                        // point on the bound to the light location
                        kDelta.Unitize();
                        float fCos1 = kLightDirection.Dot(kDelta);
                        fCos1 = ((fCos1 - fCosOuter) / 
                            (fCosInner - fCosOuter));
                        if (fCos1 < 0.0f)
                            fCos1 = 0.0f;
                        if (fCos1 > 1.0f)
                            fCos1 = 1.0f;


                        float fCosAlpha;
                        fCosAlpha = (fCos1 > fCos2) ? fCos1 : fCos2;
                        fCosAlpha = pow(fCosAlpha, fSpotExponent);
                        fAttenuation *= fCosAlpha;
                    }
                }

                if (fAttenuation > m_fLightOptimizeThreshold)
                {
                    // add this light to the affected entity list
                    unsigned int uiAffectedCount;
                    pkLightEntity->GetElementCount(
                        *m_pkAffectedEntitiesName, uiAffectedCount);
                    pmLight->SetPropertyData(
                        *m_pkAffectedEntitiesName, amEntities[j], 
                        uiAffectedCount, true);
                }
            }
            pkLightEntity->GetElementCount(*m_pkAffectedEntitiesName, 
                uiFinalCount);
            String* strMessage = String::Format("The light {0} went "
                "from affecting {1} entities to {2} entities.", 
                pmLight->Name, __box(uiInitialCount), 
                __box(uiFinalCount));
            MessageService->AddMessage(MessageChannelType::General, 
                strMessage);
        }
        // if ambient or directional, do nothing
    }
    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MToolbarListener::CopyLightPropertyByPickHandler(Object* pmObject, EventArgs* mArgs)
{
	MVerifyValidInstance;

	//클립보드에 복사된 데이터를 가져오기 위한 변수
	String *pkClipData = Clipboard::GetText();
	String *pkRetStr = "\r\n";
	ArrayList *pkEntityNameList = new ArrayList;

	//클립보드가 비어있거나 텍스트 데이터가 아니면 에러
	if((false==Clipboard::ContainsText()) || (0==pkClipData->Length))
	{
		::MessageBox(NULL, "클립보드가 비어있습니다", "오류", NULL);
		return;
	}
	Clipboard::Clear();

	if( 1 != SelectionService->NumSelectedEntities)
	{
		::MessageBox(NULL, "복사 원본이 될 엔티티는 하나만 선택되어야 합니다.", "오류", NULL);
		return;
	}

	//클립보드에 있는 데이터를 "\r\n"문자열을 경계로 한개씩 잘라냄
	while( -1 != pkClipData->IndexOf(pkRetStr) )
	{
		int iIndex = pkClipData->IndexOf(pkRetStr);
		String *pkStr = NULL;
		if(-1 != iIndex)
		{
			pkStr = pkClipData->Substring(0, iIndex);
			pkEntityNameList->Add(pkStr);
			pkClipData = pkClipData->Remove(0, iIndex+pkRetStr->Length);
		}
	}
	if(0<pkClipData->Length)
	{
		pkEntityNameList->Add(pkClipData);
	}

	
	//라이트속성 복사원본이 될 엔티티를 얻어오고
	MEntity* pkSelection[];
	pkSelection = SelectionService->GetSelectedEntities();
		
	//클립보드에 복사된 엔티티갯수만큼 할당
	MEntity* pkSelectionFromClipboard[] = new MEntity*[pkEntityNameList->Count];

	//할당된 엔티티리스트에 일치하는 엔티티 얻어오기
	for(int iIndex=0;iIndex<pkEntityNameList->Count;++iIndex)
	{
		pkSelectionFromClipboard[iIndex] = MFramework::Instance->Scene->GetEntityByName(dynamic_cast<String*>(pkEntityNameList->Item[iIndex]));
	}
	
	//모든 엔티티에 대해 마지막 엔티티의 라이트 설정을 적용
	CommandService->BeginUndoFrame("Copy Light Property By Pick");
	for(int iIndex=0; iIndex<pkEntityNameList->Count; ++iIndex)
	{//처음부터 N-1번째까지의 오브젝트에 대해
		//N번째 오브젝트의 라이트 설정을 그대로 적용한다.
		if(NULL != pkSelectionFromClipboard[iIndex])
		{
			MFramework::Instance->LightManager->ApplySameLight(pkSelection[0], pkSelectionFromClipboard[iIndex]);
		}
	}	
	CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MToolbarListener::AddLightsToArray(MEntity* pmEntity,
    ArrayList* pmLights)
{
    MVerifyValidInstance;

    if (MFramework::Instance->LightManager->EntityIsLight(pmEntity))
    {
        pmLights->Add(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MToolbarListener::SelectAffectingLightsHandler(Object* pmObject,
    EventArgs* mArgs)
{
    // for all entities in selection, search for that entity in the affected
    // list for each light. If the entity is found, add the light to the 
    // lights to select list
    ArrayList* pmNewSelection = new ArrayList;
    MEntity* amSelection[];
    amSelection = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amSelection->Count; i++)
    {
        MEntity* amLights[];
        amLights = MFramework::Instance->LightManager->GetSceneLights();
        for (int j = 0; j < amLights->Count; j++)
        {
            MEntity* pmCurrentLight = amLights[j];

            // find out of the selected entity is in the light's affected list
            int iAffectedCount = pmCurrentLight->GetElementCount(
                *m_pkAffectedEntitiesName);
            for (int k = 0; k < iAffectedCount; k++)
            {
                MEntity* pmAffected = dynamic_cast<MEntity*>(amLights[j]->
                    GetPropertyData(*m_pkAffectedEntitiesName, k));
                if (pmAffected == amSelection[i])
                {
                    MEntity* pmEntityToSelect = pmCurrentLight;

                    if (!pmNewSelection->Contains(pmEntityToSelect))
                    {
                        pmNewSelection->Add(pmEntityToSelect);
                    }
                }
            }
        }
    }
    CommandService->BeginUndoFrame("Select lights affecting selection");
    // use all entities in pmNewSelection as selection
    MEntity* amNewSelection[] = 
        static_cast<MEntity*[]>(pmNewSelection->ToArray(__typeof(MEntity)));
    SelectionService->ReplaceSelection(amNewSelection);
    CommandService->EndUndoFrame(true);
}
void MToolbarListener::CameraChangeHandler(Object* pmObject, EventArgs* mArgs)
{
	MFramework::Instance->CameraManager->ChangeCamera();//!/
}
//---------------------------------------------------------------------------
void MToolbarListener::DisablePhysXPickSnapValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

	pmState->Checked = MFramework::Instance->PickUtility->GetObserveAppCullFlag();
}
//---------------------------------------------------------------------------
void MToolbarListener::TranslateSnapValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bTranslateSnapEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::RotateSnapValidator(Object* pmSender,
    UIState* pmState) 
{
    MVerifyValidInstance;

    pmState->Checked = m_bRotateSnapEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::ScaleSnapValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bScaleSnapEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::TranslatePrecisionValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bTranslatePrecisionEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::ScalePrecisionValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bScalePrecisionEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::SnapToSurfaceValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bSnapToSurfaceEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::SnapToPointValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = m_bSnapToPointEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::AlignToSurfaceValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Enabled = m_bSnapToSurfaceEnabled;
    pmState->Checked = m_bAlignToSurfaceEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::LookAtSelectionValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Enabled = (SelectionService->NumSelectedEntities > 0);
}
//---------------------------------------------------------------------------
void MToolbarListener::OptimizeLightValidator(Object* pmSender, 
    UIState* pmState)
{
    MVerifyValidInstance;

    bool bEnabled = false;
    MEntity* amSelection[] = SelectionService->GetSelectedEntities();
    for (int i = 0; i < amSelection->Count; i++)
    {
        if (MFramework::Instance->LightManager->EntityIsLight(amSelection[i]))
        {
            bEnabled = true;
        }
    }
    pmState->Enabled = bEnabled;
}
//---------------------------------------------------------------------------
void MToolbarListener::CopyLightPropertyByPickValidator(Object* pmobject, UIState* pmState)
{
	MVerifyValidInstance;

	pmState->Enabled = (0 < SelectionService->NumSelectedEntities);
	//pmState->Checked = m_bPickLightEnabled;
/*
	if((true == m_bPickLightEnabled) && (0 == m_pkSelection->Count))
	{
		m_pkSelection = SelectionService->GetSelectedEntities();
	}*/
}
//---------------------------------------------------------------------------
void MToolbarListener::SelectAffectingLightValidator(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Enabled = (0 < SelectionService->NumSelectedEntities);
}
void MToolbarListener::CameraChangeValidator(Object* pmSender, UIState* pmState)
{
	MVerifyValidInstance;

}
//---------------------------------------------------------------------------
IInteractionModeService* MToolbarListener::get_InteractionModeService()
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
ISelectionService* MToolbarListener::get_SelectionService()
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
ICommandService* MToolbarListener::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found.");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
ISettingsService* MToolbarListener::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MToolbarListener::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found.");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
IEntityPathService* MToolbarListener::get_EntityPathService()
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
IRenderingModeService* MToolbarListener::get_RenderingModeService()
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
IMessageService* MToolbarListener::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service "
            "not found.");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
void MToolbarListener::Do_Dispose(bool bDisposing)
{
    NiDelete m_pkTranslationName;
    NiDelete m_pkRotationName;
    NiDelete m_pkStaticName;
    NiDelete m_pkOrthoWidthName;
    NiDelete m_pkFOVName;
    NiDelete m_pkAspectName; 
    NiDelete m_pkOrthoName;
    NiDelete m_pkNearClipName;
    NiDelete m_pkFarClipName;
    NiDelete m_pkLightTypeName;
    NiDelete m_pkAttenuationConstantName;
    NiDelete m_pkAttenuationLinearName;
    NiDelete m_pkAttenuationQuadraticName;
    NiDelete m_pkOuterSpotAngleName;
    NiDelete m_pkInnerSpotAngleName;
    NiDelete m_pkSpotExponentName;
    NiDelete m_pkLightDirectionName;
    NiDelete m_pkAffectedEntitiesName;
    NiDelete m_pkSceneRootName;
    NiDelete m_pkUpAxis;
}
//---------------------------------------------------------------------------
