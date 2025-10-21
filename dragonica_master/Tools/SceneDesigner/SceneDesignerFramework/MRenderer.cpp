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

#include "MRenderer.h"
#include <NiMaterialToolkit.h>
#include <NiD3DUtility.h>
#include "TexColorNoAlphaShader.h"
#include "MFramework.h"
#include "MUtility.h"
#include "MViewportManager.h"
#include "ServiceProvider.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MRenderer::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MRenderer();
    }
}
//---------------------------------------------------------------------------
void MRenderer::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MRenderer::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MRenderer* MRenderer::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MRenderer::MRenderer() : m_pkRenderer(NULL), m_hRendererWnd(0),
    m_pkVisibleArray(NULL), m_pkCullingProcess(NULL), m_hNiCgShaderLib(0),
    m_pkTexture1(NULL), m_pkTexture2(NULL), m_pkTarget1(NULL),
    m_pkTarget2(NULL), m_pkScreenQuad(NULL), m_pkScreenQuadTexProp(NULL),
    m_pkScreenQuadAlphaProp(NULL), m_pkScreenQuadVertexColorProp(NULL),
    m_pkScreenQuadStencilProp(NULL), m_pkScreenQuadShaderMaterial(NULL),
    m_pkHighlightColor(NULL), m_bNeedsRecreate(false), m_bNeedsPrecache(false)
{
    m_mBackgroundColor = Color::FromArgb(255, 128, 128, 128);

    m_pmRenderingContext = new MRenderingContext(
        NiNew NiEntityRenderingContext());

    m_pkVisibleArray = NiNew NiVisibleArray(1024, 1024);
    m_pkCullingProcess = NiNew NiCullingProcess(m_pkVisibleArray);

    m_pmRenderingContext->GetRenderingContext()->m_pkCullingProcess =
        m_pkCullingProcess;

    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter());

    // Turn off debug output messages from shader system for speed of
    // debugging purposes.
    NiD3DUtility::SetLogEnabled(false);
//    NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_0, false);
  //  NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_1, false);

    __hook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MRenderer::OnNewSceneLoaded);
    __hook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
        &MRenderer::OnEntityAddedToScene);
    __hook(&MEventManager::EntityPropertyChanged, MEventManager::Instance,
        &MRenderer::OnEntityPropertyChanged);
}
//---------------------------------------------------------------------------
void MRenderer::Do_Dispose(bool bDisposing)
{
	NiDelete m_pkBrightBloom;

    if (bDisposing)
    {
        m_pmRenderingContext->Dispose();
    }

    if (m_hNiCgShaderLib)
    {
        FreeLibrary(m_hNiCgShaderLib);
    }

    NiDelete m_pkHighlightColor;
    NiDelete m_pkCullingProcess;
    NiDelete m_pkVisibleArray;

    MDisposeRefObject(m_pkTexture1);
    MDisposeRefObject(m_pkTexture2);
    MDisposeRefObject(m_pkTarget1);
    MDisposeRefObject(m_pkTarget2);
    MDisposeRefObject(m_pkScreenQuad);
    MDisposeRefObject(m_pkScreenQuadTexProp);
    MDisposeRefObject(m_pkScreenQuadAlphaProp);
    MDisposeRefObject(m_pkScreenQuadVertexColorProp);
    MDisposeRefObject(m_pkScreenQuadStencilProp);
    MDisposeRefObject(m_pkScreenQuadShaderMaterial);

    NiMaterialToolkit::DestroyToolkit();

    MDisposeRefObject(m_pkRenderer);

    __unhook(&MEventManager::NewSceneLoaded, MEventManager::Instance,
        &MRenderer::OnNewSceneLoaded);
    __unhook(&MEventManager::EntityAddedToScene, MEventManager::Instance,
        &MRenderer::OnEntityAddedToScene);
    __unhook(&MEventManager::EntityPropertyChanged, MEventManager::Instance,
        &MRenderer::OnEntityPropertyChanged);
}
//---------------------------------------------------------------------------
void MRenderer::OnNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene && pmScene->EntityCount > 0)
    {
        PerformPrecache(pmScene);
    }
}
//---------------------------------------------------------------------------
void MRenderer::OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (MFramework::Instance->Scene == pmScene)
    {
        PerformPrecache(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MRenderer::OnEntityPropertyChanged(MEntity* pmEntity,
    String* strPropertyName, unsigned int uiPropertyIndex, bool bInBatch)
{
    MVerifyValidInstance;

    if (pmEntity->IsExternalAssetPath(strPropertyName, uiPropertyIndex))
    {
        if (MFramework::Instance->Scene->IsEntityInScene(pmEntity))
        {
            // Perform an initial update here to force the entity to load
            // the external asset.
            pmEntity->Update(MTimeManager::Instance->CurrentTime,
                MFramework::Instance->ExternalAssetManager);

            // Precache new external asset.
            PerformPrecache(pmEntity);
        }
        else
        {
            MEntity* amDependentEntities[] = MFramework::Instance->Scene
                ->GetDependentEntities(pmEntity);
            for (int i = 0; i < amDependentEntities->Length; i++)
            {
                MEntity* pmDependentEntity = amDependentEntities[i];
                if (MFramework::Instance->Scene->IsEntityInScene(
                    pmDependentEntity))
                {
                    // Perform an initial update here to force the entity to
                    // load the external asset.
                    pmDependentEntity->Update(
                        MTimeManager::Instance->CurrentTime,
                        MFramework::Instance->ExternalAssetManager);

                    // Precache new external asset.
                    PerformPrecache(pmDependentEntity);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
Color MRenderer::get_BackgroundColor()
{
    MVerifyValidInstance;

    return m_mBackgroundColor;
}
//---------------------------------------------------------------------------
void MRenderer::set_BackgroundColor(Color mBackgroundColor)
{
    MVerifyValidInstance;

    m_mBackgroundColor = mBackgroundColor;
    if (m_pkRenderer != NULL)
    {
        NiColor kBackgroundColor(MUtility::RGBToFloat(mBackgroundColor.R),
            MUtility::RGBToFloat(mBackgroundColor.G),
            MUtility::RGBToFloat(mBackgroundColor.B));
        m_pkRenderer->SetBackgroundColor(kBackgroundColor);
    }
}
//---------------------------------------------------------------------------
int MRenderer::get_Width()
{
    MVerifyValidInstance;

    if (m_pkRenderer)
    {
        return m_pkRenderer->GetDefaultBackBuffer()->GetWidth();
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
int MRenderer::get_Height()
{
    MVerifyValidInstance;

    if (m_pkRenderer)
    {
        return m_pkRenderer->GetDefaultBackBuffer()->GetHeight();
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
MRenderingContext* MRenderer::get_RenderingContext()
{
    MVerifyValidInstance;

    return m_pmRenderingContext;
}
//---------------------------------------------------------------------------
bool MRenderer::Create(IntPtr hWnd)
{
    MVerifyValidInstance;

    if (m_pkRenderer != NULL)
    {
        return false;
    }

    m_hRendererWnd = (HWND) hWnd.ToInt32();

    try 
    {
        // Try catch is just for extra precaution in case something
        // goes wrong in the creation of the Direct3D renderer.
        m_pkRenderer = NiDX9Renderer::Create(0, 0, NiDX9Renderer::USE_STENCIL,
            m_hRendererWnd, m_hRendererWnd);

        if (m_pkRenderer == NULL)
        {
            return false;
        }

        m_pmRenderingContext->GetRenderingContext()->m_pkRenderer =
            m_pkRenderer;
        MInitRefObject(m_pkRenderer);
    }
    catch (Exception*)
    {
        return false;
    }

    // Background color for renderer
    NiColor kBackgroundColor(MUtility::RGBToFloat(m_mBackgroundColor.R),
        MUtility::RGBToFloat(m_mBackgroundColor.G),
        MUtility::RGBToFloat(m_mBackgroundColor.B));
    m_pkRenderer->SetBackgroundColor(kBackgroundColor);

    CreateShaderSystem();

    HMODULE hCgCheck = LoadLibrary("CgD3D9.dll");
    if (hCgCheck)
    {
        FreeLibrary(hCgCheck);

        // Load Cg Shader Library, if it exists
        const char* const pcCgShaderLibName = "NiCgShaderLib"
            "DX9"
            "21"
            "VC71"
#if defined(_DEBUG)
            "D"
#endif //#if defined(_DEBUG)
            ".dll";

        m_hNiCgShaderLib = LoadLibrary(pcCgShaderLibName);
        if (m_hNiCgShaderLib)
        {
            unsigned int (*pfnGetCompilerVersionFunc)(void) =
                (unsigned int (*)(void))GetProcAddress(m_hNiCgShaderLib, 
                "GetCompilerVersion");
            if (pfnGetCompilerVersionFunc)
            {
                unsigned int uiPluginVersion = pfnGetCompilerVersionFunc();
                if (uiPluginVersion != (_MSC_VER))
                {
                    FreeLibrary(m_hNiCgShaderLib);
                }
            }
        }
    }

    ReloadShaders();

    float fAspectRatio = (float) Width / Height;
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewport* pmViewport = MViewportManager::Instance->GetViewport(ui);
        MCameraManager::Instance->SetAspectRatioOnStandardCameras(pmViewport,
            fAspectRatio);
        pmViewport->CreateScreenConsole();
    }

    CreateRenderedTextures();
    CreateScreenQuad();

    if (m_bNeedsPrecache)
    {
        PerformPrecache(MFramework::Instance->Scene);
    }

    NiDX9Renderer* pkDX9Renderer = NiDynamicCast(NiDX9Renderer, m_pkRenderer);
    if (pkDX9Renderer)
    {
        m_pkRenderer->SetErrorShader(pkDX9Renderer->GetLegacyDefaultShader());
    }

	m_pkBrightBloom = (PgBrightBloom*)new PgBrightBloom(true);
	m_pkBrightBloom->CreateEffect();
	m_pkBrightBloom->SetParam("Brightness", 10.0f);
	m_pkBrightBloom->SetParam("BlurWidth", 1.0f);
	m_pkBrightBloom->SetParam("SceneIntensity", 0.65f);
	m_pkBrightBloom->SetParam("GlowIntensity", 0.45f);
	m_pkBrightBloom->SetParam("HighlightIntensity", 1.0f);

	return true;
}
//---------------------------------------------------------------------------
bool MRenderer::Recreate()
{
    MVerifyValidInstance;

    // We must have a renderer first.
    if (m_pkRenderer == NULL)
    {
        return false;
    }

    m_bNeedsRecreate = true;

    NiDX9Renderer* pkDX9Renderer = NiDynamicCast(NiDX9Renderer, m_pkRenderer);
    assert(pkDX9Renderer);
    NiDX9Renderer::RecreateStatus eStatus = pkDX9Renderer->Recreate(0, 0,
        NiDX9Renderer::USE_STENCIL, m_hRendererWnd);
    if (eStatus != NiDX9Renderer::RECREATESTATUS_OK)
    {
        return false;
    }

    float fAspectRatio = (float) Width / Height;
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewport* pmViewport = MViewportManager::Instance->GetViewport(ui);
        MCameraManager::Instance->SetAspectRatioOnStandardCameras(pmViewport,
            fAspectRatio);
        pmViewport->UpdateScreenConsole();
    }

    MViewportManager::Instance->OnResize();

	m_pkBrightBloom->CreateEffect();
    CreateRenderedTextures();

    m_bNeedsRecreate = false;

    return true;
}
//---------------------------------------------------------------------------
void MRenderer::Render()
{
    MVerifyValidInstance;

    if (m_pkRenderer == NULL || m_bNeedsRecreate)
    {
        return;
    }

	MRenderingContext* pmRenderingContext = MRenderer::Instance->RenderingContext;

    // Allow the viewports to do any pre-processing
    MViewportManager::Instance->PreRender();

    // Begin the rendering frame.
    m_pkRenderer->BeginFrame();

	// Render unselected entities to the first rendered texture.
	m_pkRenderer->BeginUsingRenderTargetGroup(m_pkTarget1, NiRenderer::CLEAR_ALL);
    MViewportManager::Instance->RenderUnselectedEntities();
	MFramework::Instance->ZoneControl->Render(pmRenderingContext);
    m_pkRenderer->EndUsingRenderTargetGroup();

    // Render selected entities to the second rendered texture.
    if (SelectionService->NumSelectedEntities > 0)
    {
        // Clear second rendered texture using a shader that uses the colors
        // from the unselected entities texture but with an alpha value of
        // 0.0 for each pixel.
        NiColorA kOldClearColor;
        m_pkRenderer->GetBackgroundColor(kOldClearColor);
        m_pkRenderer->SetBackgroundColor(NiColorA(0.0f, 0.0f, 0.0f, 0.0f));
        m_pkRenderer->BeginUsingRenderTargetGroup(m_pkTarget2, NiRenderer::CLEAR_BACKBUFFER);
		m_pkRenderer->SetBackgroundColor(kOldClearColor);
        m_pkRenderer->SetScreenSpaceCameraData();
        m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture1);
        const NiMaterial* pkOldMaterial = m_pkScreenQuad->GetActiveMaterial();
		m_pkScreenQuad->SetActiveMaterial(m_pkScreenQuadShaderMaterial);
		m_pkScreenQuad->Draw(m_pkRenderer);
        m_pkScreenQuad->SetActiveMaterial(pkOldMaterial);
        
		// Render selected entities to cleared second texture.
        MViewportManager::Instance->RenderSelectedEntities();
		MFramework::Instance->DrawSoundAffectArea->Render(pmRenderingContext);
        m_pkRenderer->EndUsingRenderTargetGroup();
    }

    // Switch to default render target group.
	
	//블룸 데이터를 m_pkBrightBloom 에 넣기.
	MPgBrightBloom *MpkBloom = MFramework::Instance->BrightBloom;
	if (MpkBloom->IsUpdated)
	{
		m_pkBrightBloom->SetParam("BlurWidth", MpkBloom->Blur);
		m_pkBrightBloom->SetParam("Brightness", MpkBloom->Brightness);
		m_pkBrightBloom->SetParam("SceneIntensity", MpkBloom->SceneIntensity);
		m_pkBrightBloom->SetParam("GlowIntensity", MpkBloom->GlowIntensity);
		m_pkBrightBloom->SetParam("HighlightIntensity", MpkBloom->HighLightIntensity);
		MpkBloom->IsUpdated = false;
	}
	// 블룸 활성화가 아닐 때
	if (MpkBloom->IsActive)
		m_pkRenderer->BeginUsingRenderTargetGroup(m_pkBrightBloom->GetRenderTargetGroup(), NiRenderer::CLEAR_ALL);
	else
		m_pkRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);
	
	m_pkRenderer->SetScreenSpaceCameraData();

    // Render screen quad with first texture.
    m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture1);
    m_pkScreenQuadAlphaProp->SetAlphaBlending(false);
    m_pkScreenQuad->Draw(m_pkRenderer);

    // Render screen quad with second texture.
    if (SelectionService->NumSelectedEntities > 0)
    {
		// Render quad with standard alpha blending.
	    m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture2);
        m_pkScreenQuadStencilProp->SetStencilOn(true);
        m_pkScreenQuadStencilProp->SetStencilFunction(NiStencilProperty::TEST_ALWAYS);
        m_pkScreenQuadStencilProp->SetStencilReference(1);
        m_pkScreenQuadStencilProp->SetStencilPassAction(NiStencilProperty::ACTION_REPLACE);
		m_pkScreenQuadAlphaProp->SetAlphaBlending(true);
        m_pkScreenQuadAlphaProp->SetSrcBlendMode(NiAlphaProperty::ALPHA_SRCALPHA);
        m_pkScreenQuadAlphaProp->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        m_pkScreenQuadAlphaProp->SetAlphaTesting(true);
		m_pkScreenQuad->Draw(m_pkRenderer);
        m_pkScreenQuadAlphaProp->SetAlphaTesting(false);

        // Render quad with vertex colors using the stencil buffer to
        // highlight the selected entities.
		m_pkScreenQuadTexProp->SetBaseTexture(NULL);
        NiTexturingProperty::ApplyMode eOldApplyMode = m_pkScreenQuadTexProp->GetApplyMode();
        m_pkScreenQuadTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
		m_pkScreenQuadStencilProp->SetStencilFunction(NiStencilProperty::TEST_EQUAL);
        m_pkScreenQuadStencilProp->SetStencilReference(1);
        m_pkScreenQuadStencilProp->SetStencilPassAction(NiStencilProperty::ACTION_KEEP);
		NiVertexColorProperty::SourceVertexMode eOldSourceMode = m_pkScreenQuadVertexColorProp->GetSourceMode();
        NiVertexColorProperty::LightingMode eOldLightingMode = m_pkScreenQuadVertexColorProp->GetLightingMode();
        m_pkScreenQuadVertexColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
        m_pkScreenQuadVertexColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
		m_pkScreenQuad->Draw(m_pkRenderer);
        m_pkScreenQuadVertexColorProp->SetLightingMode(eOldLightingMode);
        m_pkScreenQuadVertexColorProp->SetSourceMode(eOldSourceMode);
        m_pkScreenQuadTexProp->SetApplyMode(eOldApplyMode);
        m_pkScreenQuadStencilProp->SetStencilOn(false);
	}

	// Render gizmo and screen elements for each viewport.
    MViewportManager::Instance->RenderGizmo();
    MViewportManager::Instance->RenderScreenElements();

	
    // Set camera on rendering context.
	MFramework::Instance->Monster->Render(pmRenderingContext);
	MFramework::Instance->WayPoint->Render(pmRenderingContext);
	MFramework::Instance->Path->Render(pmRenderingContext);
	//MFramework::Instance->MonArea->Render(pmRenderingContext);

    // End the rendering frame.
	 m_pkRenderer->EndUsingRenderTargetGroup();
	
	// 블룸 활성화를 하면 아래 코드를 수행함
	if (MpkBloom->IsActive)
		m_pkBrightBloom->RenderFrame(0.0f, m_pkRenderer, m_pkRenderer->GetDefaultRenderTargetGroup(), 0);
	
	m_pkRenderer->EndFrame();

    // Display the frame.
    m_pkRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
void MRenderer::CreateShaderSystem()
{
    NiMaterialToolkit* pkShaderKit = NiMaterialToolkit::CreateToolkit();

//#if _MSC_VER >= 1400
    size_t iSize = 0;
    //getenv_s(&iSize, NULL, 0, "NDL_SHADER_LIBRARY_PATH");
    //assert(iSize > 0);

    //char* pcShaderPath = NiExternalNew char[iSize];

	char* pcShaderPath = NiExternalNew char[255];
	::GetCurrentDirectory(255, pcShaderPath);
	//sprintf_s(pcShaderPath, 3, ".\\");

    if (pcShaderPath)
    {
        //getenv_s(&iSize, pcShaderPath, iSize, "NDL_SHADER_LIBRARY_PATH");
    }
//#else
//    char* pcShaderPath = getenv("NDL_SHADER_LIBRARY_PATH");
//#endif

    if (pcShaderPath == NULL)
    {
        const char* pcWarning = "The environment variable "
            "NDL_SHADER_LIBRARY_PATH must\nbe defined for this application"
            " to properly execute.";
        ::MessageBox(NULL, pcWarning, "Missing Environment Variable",
            MB_OK | MB_ICONERROR);
        MessageService->AddMessage(MessageChannelType::Errors, pcWarning);
    }
    else
    {
        pkShaderKit->LoadFromDLL(pcShaderPath);
        char acShaderDir[2048];
        NiSprintf(acShaderDir, 2048, "%s%s", pcShaderPath, "\\Data\\DX9\\");
        NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acShaderDir);
    }
}
//---------------------------------------------------------------------------
void MRenderer::ReloadShaders()
{
    // Unregister shaders in the main scene.
    MScene* pmScene = MFramework::Instance->Scene;
    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::UnRegisterMaterials(pkSceneRoot, 
                    m_pkRenderer, true);
            }
        }
    }
    NiMaterialToolkit::UnloadShaders();

    // Purge all existing renderer data for the main scene.
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                m_pkRenderer->PurgeAllRendererData(pkSceneRoot);
            }
        }
    }

    // Reload screen texture pixel data to avoid crash during rendering.
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewportManager::Instance->GetViewport(ui)
            ->ReloadScreenConsolePixelData();
    }

    NiMaterialToolkit::ReloadShaders();

    // Re-register all existing shaders for main scene.
    bool bSet = false;
    pmScene = MFramework::Instance->Scene;
    amEntities = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::RegisterMaterials(pkSceneRoot,
                    m_pkRenderer);
                bSet = true;
            }
        }
    }

    // Re-register all existing shaders for the tool scene.
    pmScene = MFramework::Instance->Scene;
    amEntities = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::RegisterMaterials(pkSceneRoot,
                    m_pkRenderer);
                bSet = true;
            }
        }
    }

    // If we don't have any entities, we still need to register the shader
    // library directories.
    if (!bSet)
    {
        NiMaterialToolkit::UpdateMaterialDirectory();
    }
}
//---------------------------------------------------------------------------
void MRenderer::CreateRenderedTextures()
{
    MVerifyValidInstance;

    MAssert(m_pkRenderer != NULL, "Renderer not created!");

    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;

    MDisposeRefObject(m_pkTexture1);
    m_pkTexture1 = NiRenderedTexture::Create(Width, Height, m_pkRenderer,
        kPrefs);
    MInitRefObject(m_pkTexture1);

    MDisposeRefObject(m_pkTexture2);
    m_pkTexture2 = NiRenderedTexture::Create(Width, Height, m_pkRenderer,
        kPrefs);
    MInitRefObject(m_pkTexture2);

    NiDepthStencilBuffer* pkDSB = m_pkRenderer->GetDefaultRenderTargetGroup()
        ->GetDepthStencilBuffer();
    assert(pkDSB);

    MDisposeRefObject(m_pkTarget1);
    m_pkTarget1 = NiRenderTargetGroup::Create(m_pkTexture1->GetBuffer(),
        m_pkRenderer, pkDSB);
    MInitRefObject(m_pkTarget1);

    MDisposeRefObject(m_pkTarget2);
    m_pkTarget2 = NiRenderTargetGroup::Create(m_pkTexture2->GetBuffer(),
        m_pkRenderer, pkDSB);
    MInitRefObject(m_pkTarget2);
}
//---------------------------------------------------------------------------
void MRenderer::CreateScreenQuad()
{
    MVerifyValidInstance;

    if (m_pkScreenQuad)
    {
        return;
    }

    MAssert(m_pkRenderer != NULL, "Renderer not created!");

    // Create screen quad.
    m_pkScreenQuad = NiNew NiScreenElements(NiNew NiScreenElementsData(false,
        true, 1));
    MInitRefObject(m_pkScreenQuad);

    // Add single polygon.
    int iPolygon = m_pkScreenQuad->Insert(4);
    MAssert(iPolygon == 0, "Unexpected screen quad polygon index!");
    m_pkScreenQuad->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_pkScreenQuad->UpdateBound();
    m_pkScreenQuad->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    SetScreenQuadVertexColors();

    // Create and add texturing property.
    m_pkScreenQuadTexProp = NiNew NiTexturingProperty();
    MInitRefObject(m_pkScreenQuadTexProp);
    m_pkScreenQuadTexProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    m_pkScreenQuadTexProp->SetBaseMap(NiNew NiTexturingProperty::Map());
    m_pkScreenQuadTexProp->SetBaseFilterMode(
        NiTexturingProperty::FILTER_NEAREST);
    m_pkScreenQuadTexProp->SetBaseClampMode(
        NiTexturingProperty::CLAMP_S_CLAMP_T);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadTexProp);

    // Create and add alpha property.
    m_pkScreenQuadAlphaProp = NiNew NiAlphaProperty();
    MInitRefObject(m_pkScreenQuadAlphaProp);
    m_pkScreenQuadAlphaProp->SetAlphaBlending(false);
    m_pkScreenQuadAlphaProp->SetAlphaTesting(false);
    m_pkScreenQuadAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
    m_pkScreenQuadAlphaProp->SetTestRef(0);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadAlphaProp);

    // Create and add vertex color property.
    m_pkScreenQuadVertexColorProp = NiNew NiVertexColorProperty();
    MInitRefObject(m_pkScreenQuadVertexColorProp);
    m_pkScreenQuadVertexColorProp->SetSourceMode(
        NiVertexColorProperty::SOURCE_IGNORE);
    m_pkScreenQuadVertexColorProp->SetLightingMode(
        NiVertexColorProperty::LIGHTING_E_A_D);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadVertexColorProp);

    // Create and add stencil property.
    m_pkScreenQuadStencilProp = NiNew NiStencilProperty();
    MInitRefObject(m_pkScreenQuadStencilProp);
    m_pkScreenQuadStencilProp->SetStencilOn(false);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadStencilProp);

    // Create and add z-buffer property.
    NiZBufferProperty* pkZBufferProp = NiNew NiZBufferProperty();
    pkZBufferProp->SetZBufferTest(false);
    pkZBufferProp->SetZBufferWrite(false);
    m_pkScreenQuad->AttachProperty(pkZBufferProp);

    // Create screen quad shader material.
    m_pkScreenQuadShaderMaterial = NiSingleShaderMaterial::Create(
        NiNew TexColorNoAlphaShader());
    MInitRefObject(m_pkScreenQuadShaderMaterial);
    // Don't set it as active yet
    m_pkScreenQuad->ApplyMaterial(m_pkScreenQuadShaderMaterial);

    // Perform initial update.
    m_pkScreenQuad->Update(0.0f);
    m_pkScreenQuad->UpdateProperties();
    m_pkScreenQuad->UpdateEffects();
    m_pkScreenQuad->UpdateNodeBound();
}
//---------------------------------------------------------------------------
void MRenderer::SetScreenQuadVertexColors()
{
    MVerifyValidInstance;

    if (m_pkScreenQuad)
    {
        if (!m_pkHighlightColor)
        {
            RegisterForHighlightColorSetting();
            RegisterForTransparencySetting();
        }

        m_pkScreenQuad->SetColors(0, *m_pkHighlightColor, *m_pkHighlightColor,
            *m_pkHighlightColor, *m_pkHighlightColor);
    }
}
//---------------------------------------------------------------------------
void MRenderer::PerformPrecache(MScene* pmScene)
{
    MAssert(pmScene != NULL, "Null scene provided to function!");

    if (m_pkRenderer)
    {
        MEntity* amAllEntitiesInScene[] = pmScene->GetEntities();  
        for (int i = 0; i < amAllEntitiesInScene->Length; i++)
        {
            PrecacheEntity(amAllEntitiesInScene[i]);
        }
        m_bNeedsPrecache = false;
    }
    else
    {
        m_bNeedsPrecache = true;
    }
}
//---------------------------------------------------------------------------
void MRenderer::PerformPrecache(MEntity* pmEntity)
{
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (m_pkRenderer)
    {
        PrecacheEntity(pmEntity);
        m_bNeedsPrecache = false;
    }
    else
    {
        m_bNeedsPrecache = true;
    }
}
//---------------------------------------------------------------------------
void MRenderer::PrecacheEntity(MEntity* pmEntity)
{
    MAssert(pmEntity != NULL, "Null entity provided to function!");

    for (unsigned int ui = 0; ui < pmEntity->GetSceneRootPointerCount(); ui++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
        if (pkSceneRoot)
        {
            RecursivePrecacheScene(pkSceneRoot);
        }
    }
}
//---------------------------------------------------------------------------
void MRenderer::RecursivePrecacheScene(NiAVObject* pkObject)
{
    assert(pkObject);

    // Apply the "Scene" shader to the geometry
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeometry = (NiGeometry*)pkObject;
        NiGeometryData* pkData = pkGeometry->GetModelData();
        if (pkData)
            pkData->SetKeepFlags(NiGeometryData::KEEP_ALL);
        m_pkRenderer->PrecacheGeometry(pkGeometry, 0, 0);
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // Recurse to children
        NiNode* pkNode = (NiNode*)pkObject;
        unsigned int uiChildCount = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiChildCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
            {
                RecursivePrecacheScene(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MRenderer::RegisterForHighlightColorSetting()
{
    MVerifyValidInstance;

    if (!m_pkHighlightColor)
    {
        m_pkHighlightColor = NiNew NiColorA();
    }

    SettingsService->RegisterSettingsObject(ms_strHighlightColorSettingName,
        __box(Color::Yellow), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MRenderer::OnHighlightColorChanged));
    OnHighlightColorChanged(NULL, NULL);

    OptionsService->AddOption(ms_strHighlightColorOptionName,
        SettingsCategory::PerUser, ms_strHighlightColorSettingName);
    OptionsService->SetHelpDescription(ms_strHighlightColorOptionName,
        "The color with which selected entities are highlighted.");
}
//---------------------------------------------------------------------------
void MRenderer::OnHighlightColorChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    __box Color* pmColor = dynamic_cast<__box Color*>(
        SettingsService->GetSettingsObject(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser));
    if (pmColor != NULL)
    {
        m_pkHighlightColor->r = MUtility::RGBToFloat((*pmColor).R);
        m_pkHighlightColor->g = MUtility::RGBToFloat((*pmColor).G);
        m_pkHighlightColor->b = MUtility::RGBToFloat((*pmColor).B);
        SetScreenQuadVertexColors();
    }
}
//---------------------------------------------------------------------------
void MRenderer::RegisterForTransparencySetting()
{
    MVerifyValidInstance;

    if (!m_pkHighlightColor)
    {
        m_pkHighlightColor = NiNew NiColorA();
    }

    SettingsService->RegisterSettingsObject(ms_strTransparencySettingName,
        __box(0.25f), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strTransparencySettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MRenderer::OnTransparencyChanged));
    OnTransparencyChanged(NULL, NULL);

    OptionsService->AddOption(ms_strTransparencyOptionName,
        SettingsCategory::PerUser, ms_strTransparencySettingName);
    OptionsService->SetHelpDescription(ms_strTransparencyOptionName,
        "The transparency of the selection highlighting. This should be a "
        "number between 0 and 1, where 0 is transparent and 1 is opaque.");
}
//---------------------------------------------------------------------------
void MRenderer::OnTransparencyChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    __box float* pmTransparency = dynamic_cast<__box float*>(
        SettingsService->GetSettingsObject(ms_strTransparencySettingName,
        SettingsCategory::PerUser));
    if (pmTransparency != NULL)
    {
        m_pkHighlightColor->a = *pmTransparency;
        SetScreenQuadVertexColors();
    }
}
//---------------------------------------------------------------------------
IRenderingModeService* MRenderer::get_RenderingModeService()
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
IInteractionModeService* MRenderer::get_InteractionModeService()
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
IMessageService* MRenderer::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service "
            "not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
ISelectionService* MRenderer::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service "
            "not found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ISettingsService* MRenderer::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MRenderer::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
