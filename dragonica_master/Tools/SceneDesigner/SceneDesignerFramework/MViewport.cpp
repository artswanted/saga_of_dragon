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
#include "SceneDesignerFrameworkPCH.h"

#include "MViewport.h"
#include "MFramework.h"
#include "MEntityFactory.h"
#include "MEventManager.h"
#include "MSceneFactory.h"
#include "ServiceProvider.h"
#include "MUtility.h"
#include "MCameraManager.h"
#include "MChangeViewportCameraCommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MViewport::MViewport() : m_fLeft(0.0f), m_fRight(1.0f), m_fTop(1.0f),
    m_fBottom(0.0f), m_pkBorder(NULL), m_pkBorderMaterial(NULL),
    m_pkHighlightColor(NULL), m_pkScreenConsole(NULL)
{
    InitToolScene();
    CreateBorderGeometry();
    m_pmUnselectedEntities = new ArrayList();
}
//---------------------------------------------------------------------------
MViewport::MViewport(float fLeft, float fRight, float fTop, float fBottom) :
     m_fLeft(fLeft), m_fRight(fRight), m_fTop(fTop), m_fBottom(fBottom),
     m_pkBorder(NULL), m_pkBorderMaterial(NULL), m_pkHighlightColor(NULL),
     m_pkScreenConsole(NULL)
{
    InitToolScene();
    CreateBorderGeometry();
    m_pmUnselectedEntities = new ArrayList();
}
//---------------------------------------------------------------------------
MViewport::MViewport(float fLeft, float fRight, float fTop, float fBottom,
    MEntity* pmCamera) : m_fLeft(fLeft), m_fRight(fRight), m_fTop(fTop),
    m_fBottom(fBottom), m_pkBorder(NULL), m_pkBorderMaterial(NULL),
    m_pkHighlightColor(NULL), m_pkScreenConsole(NULL)
{
    InitToolScene();
    CreateBorderGeometry();
    InternalSetCamera(pmCamera);
    m_pmUnselectedEntities = new ArrayList();
}
//---------------------------------------------------------------------------
void MViewport::_SDMInit()
{
    ms_pkTranslationName = NiNew NiFixedString("Translation");
    ms_pkRotationName = NiNew NiFixedString("Rotation");
    ms_pkNearClipName = NiNew NiFixedString("Near Clipping Plane");
    ms_pkFarClipName = NiNew NiFixedString("Far Clipping Plane");
}
//---------------------------------------------------------------------------
void MViewport::_SDMShutdown()
{
    NiDelete ms_pkTranslationName;
    NiDelete ms_pkRotationName;
    NiDelete ms_pkNearClipName;
    NiDelete ms_pkFarClipName;
}
//---------------------------------------------------------------------------
void MViewport::Do_Dispose(bool bDisposing)
{
    MDisposeRefObject(m_pkScreenConsole);
    NiDelete m_pkHighlightColor;
    MDisposeRefObject(m_pkBorder);

    if (bDisposing)
    {
        if (m_pmCamera != NULL)
        {
            m_pmCamera->Dispose();
        }
    }
}
//---------------------------------------------------------------------------
MEntity* MViewport::get_CameraEntity()
{
    MVerifyValidInstance;

    return m_pmCamera;
}
//---------------------------------------------------------------------------
NiCamera* MViewport::GetNiCamera()
{
    MVerifyValidInstance;

    if (m_pmCamera != NULL)
    {
        NiAVObject* pkNiCam = m_pmCamera->GetSceneRootPointer(0);
        //return NiDynamicCast(NiCamera, pkNiCam);
		NiCamera* pkCamera = MFramework::Instance->PgUtil->FindCamera(pkNiCam);//!/ 오브젝트에 카메라를 재귀로찾는다.
		return pkCamera;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
void MViewport::SetCamera(MEntity* pmCamera, bool bUndoable)
{
    MVerifyValidInstance;

    CommandService->ExecuteCommand(new MChangeViewportCameraCommand(this,
        pmCamera), bUndoable);
}
//---------------------------------------------------------------------------
void MViewport::InternalSetCamera(MEntity* pmCamera)
{
    MVerifyValidInstance;

    MAssert(pmCamera != NULL, "Null camera provided to function!");
    MAssert(MCameraManager::EntityIsCamera(pmCamera), "Invalid camera entity "
        "provided to function!");

    m_pmCamera = pmCamera;
    UpdateCameraViewport(GetNiCamera());
    UpdateScreenConsole();
}
//---------------------------------------------------------------------------
void MViewport::Update(float fTime)
{
    MVerifyValidInstance;

    if (m_pmToolScene != NULL)
    {
        m_pmToolScene->Update(fTime, MFramework::Instance
            ->ExternalAssetManager);
    }
    if (m_pmRenderingMode != NULL)
    {
        m_pmRenderingMode->Update(fTime);
    }
}
//---------------------------------------------------------------------------
void MViewport::UpdateClippingPlanes()
{
    NiBound* pkBound;
    pkBound = MFramework::Instance->BoundManager->GetToolSceneBound(this);
    if (pkBound)
    {
        // only standard cameras get automatic clipping plane management
        if (MFramework::Instance->CameraManager->IsStandardCamera(this,
            m_pmCamera))
        {
            NiCamera* pkCamera = NiDynamicCast(NiCamera, 
                m_pmCamera->GetSceneRootPointer(0));
            if (!pkCamera)
                return;
            // get the most up-to-date camera position for calculation
            NiEntityInterface* pkEntity = m_pmCamera->GetNiEntityInterface();
            if (!pkEntity)
                return;
            pkCamera->FitNearAndFarToBound(*pkBound);
            float fNear, fFar;
            fNear = pkCamera->GetViewFrustum().m_fNear;
            fFar = pkCamera->GetViewFrustum().m_fFar;
            pkEntity->SetPropertyData(*ms_pkNearClipName, fNear);
            pkEntity->SetPropertyData(*ms_pkFarClipName, fFar);
            m_pmCamera->Update(MFramework::Instance->TimeManager->CurrentTime, 
                MFramework::Instance->ExternalAssetManager);
        }
    }
}
//---------------------------------------------------------------------------
void MViewport::RenderUnselectedEntities()
{
    MVerifyValidInstance;

    if (m_pmRenderingMode == NULL)
    {
        // If no rendering mode is set, select the first one available in
        // the rendering mode service.
        IRenderingMode* amRenderingModes[] =
            RenderingModeService->GetRenderingModes();
        for (int i = 0; i < amRenderingModes->Length; i++)
        {
            IRenderingMode* pmRenderingMode = amRenderingModes[i];
            if (pmRenderingMode->DisplayToUser)
            {
                m_pmRenderingMode = pmRenderingMode;
                break;
            }
        }

        // If there are no rendering modes available, don't render.
        if (m_pmRenderingMode == NULL)
        {
            return;
        }
    }

    // Get viewport camera.
    NiCamera* pkCamera = GetNiCamera();
    if (!pkCamera)
    {
        // Don't render if there is no camera.
        return;
    }

    // Set viewport on camera.
    UpdateCameraViewport(pkCamera);

    // Set camera on rendering context.
    MRenderingContext* pmRenderingContext = MRenderer::Instance
        ->RenderingContext;
    pmRenderingContext->GetRenderingContext()->m_pkCamera = pkCamera;

    // Begin rendering.
    m_pmRenderingMode->Begin(pmRenderingContext);

    // Render tool scene.
    m_pmRenderingMode->Render(ToolScene->GetEntities(), pmRenderingContext);

    // Render main scene.
    MEntity* amSceneEntities[] = MFramework::Instance->Scene->GetEntities();
    m_pmUnselectedEntities->Clear();
    for (int i = 0; i < amSceneEntities->Length; i++)
    {
        MEntity* pmSceneEntity = amSceneEntities[i];
		//MPgExtEntity::eExtEntityType eType = MFramework::Instance->ExtEntity->IsExtEntityType(pmSceneEntity);
		//if(MPgExtEntity::eExtEntityType::ExtEntityType_Monster == eType)
		//{
		//	pmSceneEntity->GetNiEntityInterface()->SetPropertyData("Nif File Path", "E:\\MyWorks\\2. ProjectD\\Dragonica_Exe\\SFreedom_Dev\\Data\\3_World\\Palettes\\monster_target.nif");
		//	pmSceneEntity->GetNiEntityInterface()->
		//}
		if (!SelectionService->IsEntitySelected(pmSceneEntity))
        {
            m_pmUnselectedEntities->Add(pmSceneEntity);
        }
    }
    m_pmRenderingMode->Render(static_cast<MEntity*[]>(m_pmUnselectedEntities
        ->ToArray(__typeof(MEntity))), pmRenderingContext);

    // Render proxy scene.
    m_pmRenderingMode->Render(MProxyManager::Instance->ProxyScene
        ->GetEntities(), pmRenderingContext);

    // End rendering.
    m_pmRenderingMode->End(pmRenderingContext);
}

//---------------------------------------------------------------------------
void MViewport::RenderSelectedEntities()
{
    MVerifyValidInstance;

    if (m_pmRenderingMode == NULL)
    {
        // If no rendering mode is set, select the first one available in
        // the rendering mode service.
        IRenderingMode* amRenderingModes[] =
            RenderingModeService->GetRenderingModes();
        for (int i = 0; i < amRenderingModes->Length; i++)
        {
            IRenderingMode* pmRenderingMode = amRenderingModes[i];
            if (pmRenderingMode->DisplayToUser)
            {
                m_pmRenderingMode = pmRenderingMode;
                break;
            }
        }

        // If there are no rendering modes available, don't render.
        if (m_pmRenderingMode == NULL)
        {
            return;
        }
    }

    // Get viewport camera.
    NiCamera* pkCamera = GetNiCamera();
    if (!pkCamera)
    {
        // Don't render if there is no camera.
        return;
    }

    // Set viewport on camera.
    UpdateCameraViewport(pkCamera);

    // Set camera on rendering context.
    MRenderingContext* pmRenderingContext = MRenderer::Instance
        ->RenderingContext;

	pmRenderingContext->GetRenderingContext()->m_pkCamera = pkCamera;

    // Begin rendering.
    m_pmRenderingMode->Begin(pmRenderingContext);

    // Render selected entities.
	
    m_pmRenderingMode->Render(SelectionService->GetSelectedEntities(),
        pmRenderingContext);

	// End rendering.
    m_pmRenderingMode->End(pmRenderingContext);
}


//---------------------------------------------------------------------------
void MViewport::RenderGizmo()
{
    MVerifyValidInstance;

    // Get active interaction mode.
    IInteractionMode* pmInteractionMode = InteractionModeService->ActiveMode;
    if (pmInteractionMode != NULL)
    {
        // Get viewport camera.
        NiCamera* pkCamera = GetNiCamera();
        if (!pkCamera)
        {
            // Don't render if there is no camera.
            return;
        }

        // Set viewport on camera.
        UpdateCameraViewport(pkCamera);

        // Set camera on rendering context.
        MRenderingContext* pmRenderingContext = MRenderer::Instance
            ->RenderingContext;
        pmRenderingContext->GetRenderingContext()->m_pkCamera = pkCamera;

        // Render gizmo.
        pmInteractionMode->RenderGizmo(pmRenderingContext);
    }
}
//---------------------------------------------------------------------------
void MViewport::RenderScreenElements(bool bActiveViewport)
{
    MVerifyValidInstance;

    // Render border.
    if (bActiveViewport)
    {
        if (!m_pkHighlightColor)
        {
            RegisterForHighlightColorSetting();
        }
        m_pkBorderMaterial->SetEmittance(*m_pkHighlightColor);
    }
    else
    {
        m_pkBorderMaterial->SetEmittance(NiColor::BLACK);
    }
    NiRenderer* pkRenderer = MRenderer::Instance->RenderingContext
        ->GetRenderingContext()->m_pkRenderer;
    pkRenderer->SetScreenSpaceCameraData();
    m_pkBorder->RenderImmediate(pkRenderer);

    // Render screen console.
    if (m_pkScreenConsole)
    {
        NiScreenTexture* pkConsoleTexture = m_pkScreenConsole
            ->GetActiveScreenTexture();
        if (pkConsoleTexture)
        {
            pkConsoleTexture->Draw(pkRenderer);
        }
    }
}
//---------------------------------------------------------------------------
float MViewport::get_Left()
{
    MVerifyValidInstance;

    return m_fLeft;
}
//---------------------------------------------------------------------------
float MViewport::get_Right()
{
    MVerifyValidInstance;

    return m_fRight;
}
//---------------------------------------------------------------------------
float MViewport::get_Top()
{
    MVerifyValidInstance;

    return m_fTop;
}
//---------------------------------------------------------------------------
float MViewport::get_Bottom()
{
    MVerifyValidInstance;

    return m_fBottom;
}
//---------------------------------------------------------------------------
bool MViewport::SetViewportValues(float fLeft, float fRight, float fTop,
    float fBottom)
{
    MVerifyValidInstance;

    if (fLeft < 0.0f || fLeft > 1.0f || fRight < 0.0f || fRight > 1.0f ||
        fTop < 0.0f || fTop > 1.0f || fBottom < 0.0f || fBottom > 1.0f)
    {
        return false;
    }

    m_fLeft = fLeft;
    m_fRight = fRight;
    m_fTop = fTop;
    m_fBottom = fBottom;

    UpdateCameraViewport(GetNiCamera());
    UpdateBorderGeometry();
    UpdateScreenConsole();

    return true;
}
//---------------------------------------------------------------------------
int MViewport::get_Width()
{
    MVerifyValidInstance;

    return (int) ((m_fRight - m_fLeft) * MRenderer::Instance->Width);
}
//---------------------------------------------------------------------------
int MViewport::get_Height()
{
    MVerifyValidInstance;

    return (int) ((m_fTop - m_fBottom) * MRenderer::Instance->Height);
}
//---------------------------------------------------------------------------
MScene* MViewport::get_ToolScene()
{
    MVerifyValidInstance;

    return m_pmToolScene;
}
//---------------------------------------------------------------------------
IRenderingMode* MViewport::get_RenderingMode()
{
    MVerifyValidInstance;

    return m_pmRenderingMode;
}
//---------------------------------------------------------------------------
void MViewport::set_RenderingMode(IRenderingMode* pmRenderingMode)
{
    MVerifyValidInstance;

    m_pmRenderingMode = pmRenderingMode;
    MEventManager::Instance->RaiseViewportRenderingModeChanged(this,
        m_pmRenderingMode);
}
//---------------------------------------------------------------------------
System::Drawing::Rectangle MViewport::get_CameraNameRect()
{
    MVerifyValidInstance;

    System::Drawing::Rectangle mRect(0, 0, 0, 0);
    if (m_pkScreenConsole)
    {
        const char* pcCameraName = m_pkScreenConsole->GetLine(0);
        MAssert(pcCameraName != NULL, "Null screen console text!");
        size_t stCharCount = strlen(pcCameraName);

        NiScreenConsole::NiConsoleFont* pkFont = m_pkScreenConsole->GetFont();
        MAssert(pkFont != NULL, "Null screen console font!");

        mRect.X = (m_fLeft * MRenderer::Instance->Width) + ms_iScreenTextureOffsetX;
        mRect.Y = ((1-m_fTop) * MRenderer::Instance->Height);
        mRect.Width = pkFont->m_uiCharSpacingX * stCharCount;
        mRect.Height = pkFont->m_uiCharSpacingY * 2;
    }

    return mRect;
}
//---------------------------------------------------------------------------
void MViewport::InitToolScene()
{
    MVerifyValidInstance;

    NiScene* pkScene = NiNew NiScene("Tool Scene", 10);
    m_pmToolScene = MSceneFactory::Instance->Get(pkScene);
}
//---------------------------------------------------------------------------
void MViewport::CreateBorderGeometry()
{
    MVerifyValidInstance;

    // Create NiLines geometry.
    unsigned short usNumVertices = 4;
    NiPoint3* pkVertices = NiNew NiPoint3[usNumVertices];
    NiBool* pbFlags = NiAlloc(NiBool, usNumVertices);
    for (unsigned int ui = 0; ui < usNumVertices; ui++)
    {
        pbFlags[ui] = true;
    }
    m_pkBorder = NiNew NiLines(usNumVertices, pkVertices, NULL, NULL, 0,
        NiGeometryData::NBT_METHOD_NONE, pbFlags);
    MInitRefObject(m_pkBorder);
    m_pkBorder->SetConsistency(NiGeometryData::MUTABLE);
    UpdateBorderGeometry();

    // Attach material property.
    m_pkBorderMaterial = NiNew NiMaterialProperty();
    m_pkBorderMaterial->SetEmittance(NiColor::BLACK);
    m_pkBorder->AttachProperty(m_pkBorderMaterial);

    // Attach vertex color property.
    NiVertexColorProperty* pkVertexColorProperty = NiNew
        NiVertexColorProperty();
    pkVertexColorProperty->SetSourceMode(
        NiVertexColorProperty::SOURCE_IGNORE);
    pkVertexColorProperty->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_pkBorder->AttachProperty(pkVertexColorProperty);

    // Attach z-buffer property.
    NiZBufferProperty* pkZBufferProperty = NiNew NiZBufferProperty();
    pkZBufferProperty->SetZBufferTest(false);
    pkZBufferProperty->SetZBufferWrite(false);
    m_pkBorder->AttachProperty(pkZBufferProperty);

    // Perform initial update.
    m_pkBorder->Update(0.0f);
    m_pkBorder->UpdateProperties();
    m_pkBorder->UpdateEffects();
    m_pkBorder->UpdateNodeBound();
}
//---------------------------------------------------------------------------
void MViewport::UpdateBorderGeometry()
{
    MVerifyValidInstance;

    if (!m_pkBorder)
    {
        return;
    }

    // Adjust left and top locations to ensure that the lines are visible.
    float fLeft = (m_fLeft == 0.0f ? 0.001f : m_fLeft);
    float fTop = (m_fTop == 1.0f ? 0.999f : m_fTop);

    NiPoint3* pkVertices = m_pkBorder->GetVertices();
    pkVertices[0] = NiPoint3(fLeft, 1.0f - fTop, 0.0f);
    pkVertices[1] = NiPoint3(fLeft, 1.0f - m_fBottom, 0.0f);
    pkVertices[2] = NiPoint3(m_fRight, 1.0f - m_fBottom, 0.0f);
    pkVertices[3] = NiPoint3(m_fRight, 1.0f - fTop, 0.0f);
    m_pkBorder->GetModelData()->MarkAsChanged(NiGeometryData::VERTEX_MASK);
}
//---------------------------------------------------------------------------
void MViewport::CreateScreenConsole()
{
    MVerifyValidInstance;

    if (!m_pkScreenConsole)
    {
        m_pkScreenConsole = NiNew NiScreenConsole();
        MInitRefObject(m_pkScreenConsole);

        const char* pcPath = MStringToCharPointer(String::Concat(
            MFramework::Instance->AppStartupPath, "Data\\"));
        m_pkScreenConsole->SetDefaultFontPath(pcPath);
        MFreeCharPointer(pcPath);

        m_pkScreenConsole->SetFont(m_pkScreenConsole->CreateConsoleFont());
        UpdateScreenConsole();
        m_pkScreenConsole->Enable(true);
    }
}
//---------------------------------------------------------------------------
void MViewport::ReloadScreenConsolePixelData()
{
    MVerifyValidInstance;

    if (m_pkScreenConsole)
    {
        NiScreenTexture* pkConsoleTexture = m_pkScreenConsole
            ->GetActiveScreenTexture();
        if (pkConsoleTexture)
        {
            NiSourceTexture* pkSourceTexture = NiDynamicCast(NiSourceTexture,
                pkConsoleTexture->GetTexture());
            if (pkSourceTexture)
            {
                pkSourceTexture->LoadPixelDataFromFile();
            }
        }
    }
}
//---------------------------------------------------------------------------
void MViewport::UpdateScreenConsole()
{
    MVerifyValidInstance;

    if (m_pkScreenConsole)
    {
        m_pkScreenConsole->SetDimensions(NiPoint2((float) Width, (float)
            Height));
        m_pkScreenConsole->SetOrigin(NiPoint2(ms_iScreenTextureOffsetX +
            m_fLeft * MRenderer::Instance->Width, (1-m_fTop) *
            MRenderer::Instance->Height));

        m_pkScreenConsole->SetCamera(GetNiCamera());
        if (m_pmCamera != NULL)
        {
            const char* pcCameraName = MStringToCharPointer(m_pmCamera->Name);
            m_pkScreenConsole->SetLine(pcCameraName, 0);
			if(!strcmp(pcCameraName, "Perspective"))
			{
				int nTriCount = 0;
				int nObjCount = 0;
				MFramework::Instance->PgUtil->GetAllTriangleCount(nTriCount, nObjCount);
				char szTmp[100]={0,};
				sprintf(szTmp, "O:%d, T:%d", nObjCount, nTriCount);
				m_pkScreenConsole->SetLine(szTmp, 1);
			}
			MFreeCharPointer(pcCameraName);
        }
        else
        {
            m_pkScreenConsole->SetLine(NULL, 0);
        }
        m_pkScreenConsole->RecreateText();
    }
}
//---------------------------------------------------------------------------
void MViewport::UpdateCameraViewport(NiCamera* pkCamera)
{
    MVerifyValidInstance;

    if (pkCamera)
    {
        NiRect<float> kViewport(m_fLeft, m_fRight, m_fTop, m_fBottom);
        pkCamera->SetViewPort(kViewport);
    }
}
//---------------------------------------------------------------------------
void MViewport::RegisterForHighlightColorSetting()
{
    MVerifyValidInstance;

    m_pkHighlightColor = NiNew NiColor();

    SettingsService->RegisterSettingsObject(ms_strHighlightColorSettingName,
        __box(Color::Yellow), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MViewport::OnHighlightColorChanged));
    OnHighlightColorChanged(NULL, NULL);

    OptionsService->AddOption(ms_strHighlightColorOptionName,
        SettingsCategory::PerUser, ms_strHighlightColorSettingName);
    OptionsService->SetHelpDescription(ms_strHighlightColorOptionName,
        "The color with which the active viewport is highlighted.");
}
//---------------------------------------------------------------------------
void MViewport::OnHighlightColorChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    __box Color* pmColor = dynamic_cast<__box Color*>(
        SettingsService->GetSettingsObject(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser));
    if (pmColor != NULL)
    {
        *m_pkHighlightColor = NiColor(MUtility::RGBToFloat((*pmColor).R),
            MUtility::RGBToFloat((*pmColor).G),
            MUtility::RGBToFloat((*pmColor).B));
    }
}
//---------------------------------------------------------------------------
IRenderingModeService* MViewport::get_RenderingModeService()
{
    if (ms_pmRenderingModeService == NULL)
    {
        ms_pmRenderingModeService = MGetService(IRenderingModeService);
        MAssert(ms_pmRenderingModeService != NULL, "Rendering mode service "
            "not found!");
    }
    return ms_pmRenderingModeService;
}
//---------------------------------------------------------------------------
IInteractionModeService* MViewport::get_InteractionModeService()
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
ISettingsService* MViewport::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MViewport::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
ISelectionService* MViewport::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ICommandService* MViewport::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found.");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
