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

#include "stdafx.h"
#include "MRenderer.h"
#include "MSharedData.h"
#include <NiD3DShaderFactory.h>
#include <NiD3DUtility.h>
#include <NiMaterialToolkit.h>
#include <NiMaterialHelpers.h>
#include <NiVersion.h>

using namespace NiManagedToolInterface;
using namespace System::Threading;

#define USE_SHADER_SYSTEM

MColor::MColor(float fr, float fg, float fb)
{
    r = fr;
    g = fg;
    b = fb;
}
//---------------------------------------------------------------------------
MRenderer::MRenderer() : m_bRendererCreated(false), m_bNeedsRecreate(false)
{
    ActiveCamera = NiExternalNew MCamera();
    m_pkRenderer = NULL;
    m_pkStatManager = NULL;
    m_strAppStartupPath = NULL;
    m_pkScreenConsole = NULL;
    m_pkColor = NiExternalNew MColor(0.0f, 0.0f, 0.0f);
    m_pcActiveMode = NiExternalNew char[512];
    NiSprintf(m_pcActiveMode, 512, "Sequence");
    m_bFrameRateEnabled = true;
    m_bActiveModeEnabled = true;
    m_bLODStatsEnabled = true;
    NiD3DUtility::SetLogEnabled(false);

    m_pkVisible = NiNew NiVisibleArray(1024, 1024);
    m_pkCuller = NiNew NiCullingProcess(m_pkVisible);
}
//---------------------------------------------------------------------------
MRenderer::~MRenderer()
{
}
//---------------------------------------------------------------------------
void MRenderer::Lock()
{
    if (m_uiLockCount == 0)
    {
        Monitor::Enter(this);
    }
    m_uiLockCount++;
}
//---------------------------------------------------------------------------
void MRenderer::Unlock()
{
    m_uiLockCount--;
    if (m_uiLockCount == 0)
    {
        Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
bool MRenderer::Create(IntPtr hWnd, IntPtr hRootWnd)
{
    if (m_pkRenderer != NULL)
        return false;

    m_hRendererWnd = (HWND) hWnd.ToInt32();
    m_hRootWnd =  (HWND) hRootWnd.ToInt32();

    try 
    {
        // Try catch is just for extra precaution in case something
        // goes wrong in the creation of the Direct3D renderer.
        m_pkRenderer = NiDX9Renderer::Create(0, 0,
            NiDX9Renderer::USE_MULTITHREADED, m_hRendererWnd, m_hRootWnd);
        
        if (!m_pkRenderer)
        {
            return false;
        }
    }
    catch (Exception*)
    {
        return false;
    }

     // Add an alpha accumulator to the the renderer.
    NiAlphaAccumulator* pkAlpha = NiNew NiAlphaAccumulator;
    m_pkRenderer->SetSorter(pkAlpha);

    m_pkRenderer->IncRefCount();

    // Background color for renderer
    NiColor kColor;
    kColor.r = m_pkColor->r;
    kColor.g = m_pkColor->g;
    kColor.b = m_pkColor->b;
    m_pkRenderer->SetBackgroundColor(kColor);

    CreateShaderSystem();

    if (m_pkRendererCreatedDelegate != NULL)
    {
        m_pkRendererCreatedDelegate();
    }
   
    HMODULE hCgCheck = LoadLibrary("CgD3D9.dll");
    if (hCgCheck)
    {
        FreeLibrary(hCgCheck);

        // Load Cg Shader Library, if it exists
        const char* const pcCgShaderLibName = "NiCgShaderLib"
            "DX9"
            NI_DLL_SUFFIX
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
    CreateScreenConsole();
    EnableFrameRate(m_bFrameRateEnabled);
    EnableLODStats(m_bLODStatsEnabled);
    EnableActiveMode(m_bActiveModeEnabled);
/*    

    ReInitializeScene();

    m_pkInputManager->CreateInputSystem( m_pkRenderer, false, 
        (HINSTANCE)ghModule, m_hRootWnd);

*/
    m_bRendererCreated = true;
    return true;
}
//---------------------------------------------------------------------------
bool MRenderer::ReCreate()
{
    // We must have a renderer first.
    if (!m_pkRenderer)
    {
        return false;
    }

    m_bNeedsRecreate = true;

    Lock();

    NiDX9Renderer* pkDX9Renderer = NiSmartPointerCast(NiDX9Renderer,
        m_pkRenderer);
    assert(pkDX9Renderer);
    NiDX9Renderer::RecreateStatus eStatus = pkDX9Renderer->Recreate(0, 0,
        NiDX9Renderer::USE_MULTITHREADED, m_hRendererWnd);
	if (eStatus != NiDX9Renderer::RECREATESTATUS_OK)
    {
        Unlock();
        return false;
    }

    if (m_pkRendererReCreatedDelegate != NULL)
    {
        m_pkRendererReCreatedDelegate();
    }
    Unlock();

    m_bNeedsRecreate = false;
    
    return true;
}
//---------------------------------------------------------------------------
void MRenderer::Destroy()
{
    Lock();

    if (m_pkScreenConsole)
    {
        EnableFrameRate(false);
        EnableLODStats(false);
        EnableActiveMode(false);

        m_pkScreenConsole->DecRefCount();
        m_pkScreenConsole = NULL;
    }

    if (ActiveCamera)
        ActiveCamera->DeleteContents();
    ActiveCamera = NULL;
    
    if (m_hNiCgShaderLib)
        FreeLibrary(m_hNiCgShaderLib);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (pkData)
    {
        for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
        {
            NiAVObject* pkObject = pkData->GetScene(ui);
            if (pkObject)
            {
                m_pkRenderer->PurgeAllRendererData(pkObject);
            }
        }
    }

    NiActorManager* pkManager = pkData->GetActorManager();
    if (pkManager && pkManager->GetNIFRoot())
    {
        m_pkRenderer->PurgeAllRendererData(pkManager->GetNIFRoot());
    }
    pkData->Unlock();
    
    NiMaterialToolkit::DestroyToolkit();

    if (m_pkRenderer)
    {
        m_pkRenderer->DecRefCount();
        m_pkRenderer = NULL;
    }

    m_pkStatManager = NULL;
    m_pkRenderer = NULL;
    ActiveCamera = NULL;
    m_strAppStartupPath = NULL;
    NiScreenConsole::SetDefaultFontPath(NULL);

    NiDelete m_pkCuller;
    m_pkCuller = NULL;
    NiDelete m_pkVisible;
    m_pkVisible = NULL;


    Unlock();
}
//---------------------------------------------------------------------------
void MRenderer::DeleteContents()
{
    Destroy();
}
//---------------------------------------------------------------------------
void MRenderer::ClearClickSwap()
{
    if (!m_bRendererCreated || m_bNeedsRecreate)
    {
        return;
    }

    Lock();
    if (m_pkRenderer && ActiveCamera)
    {
        NiCamera* pkCamera = ActiveCamera->GetCamera();
        if (pkCamera)
        {
            MSharedData* pkData = MSharedData::Instance;
            pkData->Lock();

            UpdateScreenConsole();
            
            m_pkStatManager->StartClearTimer();
            Clear(pkCamera);
            m_pkStatManager->StopClearTimer();

            m_pkStatManager->StartClickTimer();
            Click(pkCamera);
            m_pkStatManager->StopClickTimer();
            
            m_pkStatManager->StartSwapTimer();
            SwapBuffers(pkCamera);
            m_pkStatManager->StopSwapTimer();

            m_pkStatManager->SampleFrameRate();
            pkData->Unlock();
        }
    }
    Unlock();
}
//---------------------------------------------------------------------------
void MRenderer::Clear(NiCamera* pkCamera)
{
    assert(m_uiLockCount > 0);
    const NiRect<float>& kViewPort = pkCamera->GetViewPort();
}
//---------------------------------------------------------------------------
void MRenderer::Click(NiCamera* pkCamera)
{
    assert(m_uiLockCount > 0);
    
    m_pkRenderer->BeginFrame();
    m_pkRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);

    m_pkRenderer->SetCameraData(pkCamera);

    // Store the current sorter
    NiAccumulatorPtr spOriginalSorter = m_pkRenderer->GetSorter();
    
    // Start the sorter if it exists
    if (spOriginalSorter)
        spOriginalSorter->StartAccumulating(pkCamera);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (pkData)
    {
        for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
        {
            NiAVObject* pkObject = pkData->GetScene(ui);
            
            if (pkObject == NULL || !pkObject->IsVisualObject())
                continue;

            try
            {
                NiDrawScene(pkCamera, pkObject, *m_pkCuller);
            }
            catch (Exception*)
            {
                char acString[1024];
                NiSprintf(acString, 1024, 
					"Exception in CullShow. SceneID: %d", ui);
                MLogger::LogDebug(acString);
            }
        }
    }
    pkData->Unlock();

    
    // If there was an active sorter during the rendering pass, close it
    if (m_pkRenderer->GetSorter())
        m_pkRenderer->GetSorter()->FinishAccumulating();

    // restore the original sorter (in case we NULLed it out)
    m_pkRenderer->SetSorter(spOriginalSorter);

    NiScreenTexture* pkScrTex = m_pkScreenConsole->GetActiveScreenTexture();
    if (pkScrTex)
        pkScrTex->Draw(m_pkRenderer);
    
    m_pkRenderer->EndUsingRenderTargetGroup();
    m_pkRenderer->EndFrame();
}
//---------------------------------------------------------------------------
void MRenderer::SwapBuffers(NiCamera* pkCamera)
{
    assert(m_uiLockCount > 0);
    if (m_pkRenderer)
        m_pkRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
void MRenderer::CreateShaderSystem()
{
    NiMaterialToolkit* pkMaterialKit = NiMaterialToolkit::CreateToolkit();

#ifdef USE_SHADER_SYSTEM

	char* pcMaterialPath = "./Shaders";

    if (pcMaterialPath == NULL)
    {
        const char* pcWarning = "The environment variable "
            "NDL_SHADER_LIBRARY_PATH must\nbe defined for this application"
            " to properly execute.";
        MessageBox(NULL, 
            pcWarning,
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
        MLogger::LogWarning(pcWarning);
    }
    else
    {
        pkMaterialKit->LoadFromDLL(pcMaterialPath);
        char acMaterialDir[2048];
        NiSprintf(acMaterialDir, 2048, "%s%s", pcMaterialPath,
            "\\Data\\DX9\\");
        NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acMaterialDir);

        MLogger::LogGeneral("Loaded Material Library");
    }
#endif
}
//---------------------------------------------------------------------------
void MRenderer::ReloadShaders()
{
#ifdef USE_SHADER_SYSTEM
    Lock();
    MLogger::LogGeneral("Reloading Materials");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    // Unregister shaders in all of the scenes
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            NiMaterialHelpers::UnRegisterMaterials(pkScene, 
                m_pkRenderer, true);
        }
    }
    NiMaterialToolkit::UnloadShaders();
    // Purge all existing renderer data for the scene
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            m_pkRenderer->PurgeAllRendererData(pkScene);
        }
    }

    // Reload screen texture pixel data to avoid crash during rendering.
    if (m_pkScreenConsole)
    {
        NiScreenTexture* pkScrTex = m_pkScreenConsole
            ->GetActiveScreenTexture();
        if (pkScrTex)
        {
            NiSourceTexture* pkTexture = NiDynamicCast(NiSourceTexture,
                pkScrTex->GetTexture());
            if (pkTexture)
            {
                pkTexture->LoadPixelDataFromFile();
            }
        }
    }

    NiMaterialToolkit::ReloadShaders();

    // Re-register all existing shaders.
    bool bSet = false;
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            NiMaterialHelpers::RegisterMaterials(pkScene, m_pkRenderer);
            bSet = true;
        }
    }

    // If we don't have a scene graph, we still need to 
    // register the shader library directories. 
    if (!bSet)
    {
        NiMaterialToolkit::UpdateMaterialDirectory();
    }

    pkData->Unlock();
    Unlock();
#endif
}
//---------------------------------------------------------------------------
void MRenderer::Init(String* strAppStartupPath, 
    MStatisticsManager* pkStatManager)
{
    m_strAppStartupPath = strAppStartupPath;
    m_pkStatManager = pkStatManager;
}
//---------------------------------------------------------------------------
void MRenderer::Shutdown()
{
}
//---------------------------------------------------------------------------
void MRenderer::EnableFrameRate(bool bEnable)
{
    assert (m_pkScreenConsole != NULL);
    m_bFrameRateEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::EnableLODStats(bool bEnable)
{
    assert (m_pkScreenConsole != NULL);
    m_bLODStatsEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::EnableActiveMode(bool bEnable)
{
    assert (m_pkScreenConsole != NULL);
    m_bActiveModeEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::SetActiveModeString(String* strActiveMode)
{
    const char* pcMode = MStringToCharPointer(strActiveMode);
    NiSprintf(m_pcActiveMode, 512, "%s", pcMode);
    MFreeCharPointer(pcMode);
}
//---------------------------------------------------------------------------
void MRenderer::CreateScreenConsole()
{
    if(!m_pkScreenConsole)
    {
        m_pkScreenConsole = NiNew NiScreenConsole();
        m_pkScreenConsole->IncRefCount();
        m_pkScreenConsole->SetCamera(ActiveCamera->GetCamera());

        const char* pcPath = MStringToCharPointer(String::Concat(
            m_strAppStartupPath, "Data\\"));
        NiScreenConsole::SetDefaultFontPath(pcPath);
        MFreeCharPointer(pcPath);

        m_pkScreenConsole->SetDimensions(NiPoint2(800.0f, 600.0f));
        m_pkScreenConsole->SetOrigin(NiPoint2(9.0f, 0.0f));
        m_pkScreenConsole->SetFont(m_pkScreenConsole->CreateConsoleFont());
        m_pkScreenConsole->Enable(true);
    }
}
//---------------------------------------------------------------------------
void MRenderer::UpdateScreenConsole()
{
    if(m_pkScreenConsole && ActiveCamera->GetCamera())
    {
        // Peformance Stats
        char acString[256];

        if (m_bActiveModeEnabled)
        {
            NiSprintf(acString, 256, "Display Mode: %s", m_pcActiveMode);
        }
        else
        {
            acString[0] = '\0';
        }
        m_pkScreenConsole->SetLine(acString, 0);

        float fFrameRate = m_pkStatManager->GetFrameRate();
        if (m_bFrameRateEnabled)
        {
            NiSprintf(acString, 256, "FPS: %.0f", fFrameRate);
        }
        else
        {
            acString[0] = '\0';
        }
        m_pkScreenConsole->SetLine(acString,1);

        if (m_bLODStatsEnabled)
        {
            MSharedData* pkSharedData = MSharedData::Instance;
            pkSharedData->Lock();

            NiActorManager* pkActorManager = 
                pkSharedData->GetActorManager();
            if (!pkActorManager)
            {
                acString[0] = '\0';
            }
            else
            {
                NiBoneLODController* pkBoneLOD = 
                    pkActorManager->GetBoneLODController();
                if (pkBoneLOD && pkBoneLOD->GetNumberOfBoneLODs() > 0)
                {
                    NiSprintf(acString, 256, "LOD: %d of %d", 
                        pkBoneLOD->GetBoneLOD(), 
                        pkBoneLOD->GetNumberOfBoneLODs() - 1);
                }
                else
                {
                    acString[0] = '\0';
                }
            }
            pkSharedData->Unlock();
        }
        else
        {
            acString[0] = '\0';
        }
        m_pkScreenConsole->SetLine(acString, 2);
        m_pkScreenConsole->RecreateText();
    }
    else
        m_pkScreenConsole->SetCamera(NULL);
}
//---------------------------------------------------------------------------
HWND MRenderer::GetRenderWindowHandle()
{
    return m_hRendererWnd;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
void MRenderer::add_OnRendererCreatedEvent(RendererCreatedEvent* pkEvent)
{
    m_pkRendererCreatedDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::remove_OnRendererCreatedEvent(RendererCreatedEvent* pkEvent)
{
    m_pkRendererCreatedDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::raise_OnRendererCreatedEvent()
{
    if (m_pkRendererCreatedDelegate)
    {
        m_pkRendererCreatedDelegate();
    }
}
//---------------------------------------------------------------------------
void MRenderer::add_OnRendererReCreatedEvent(RendererReCreatedEvent* pkEvent)
{
    m_pkRendererReCreatedDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::remove_OnRendererReCreatedEvent(
    RendererReCreatedEvent* pkEvent)
{
    m_pkRendererReCreatedDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::raise_OnRendererReCreatedEvent()
{
    if (m_pkRendererReCreatedDelegate != NULL)
    {
        m_pkRendererReCreatedDelegate();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
bool MRenderer::get_Active()
{
    return m_pkRenderer != NULL;
}
//---------------------------------------------------------------------------
int MRenderer::get_Width()
{
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
MColor* MRenderer::get_Color()
{
    return m_pkColor;
}
//---------------------------------------------------------------------------
void MRenderer::set_Color(MColor* pkColor)
{
    m_pkColor = pkColor;

    if (m_pkRenderer)
    {
        NiColor kColor;
        kColor.r = pkColor->r;
        kColor.g = pkColor->g;
        kColor.b = pkColor->b;
        Lock();
        m_pkRenderer->SetBackgroundColor(kColor);
        Unlock();
    }
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowFrameRate()
{
    return m_bFrameRateEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowFrameRate(bool bEnable)
{
    EnableFrameRate(bEnable);
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowLODStats()
{
    return m_bLODStatsEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowLODStats(bool bEnable)
{
    EnableLODStats(bEnable);
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowActiveMode()
{
    return m_bActiveModeEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowActiveMode(bool bEnable)
{
    EnableActiveMode(bEnable);
}
//---------------------------------------------------------------------------
Int32 MRenderer::get_RenderWindowHandle()
{
	return (Int32)(m_hRendererWnd);
}
//---------------------------------------------------------------------------
void MRenderer::set_RenderWindowHandle(Int32 handle)
{
	m_hRendererWnd = (HWND)(handle);
}
//---------------------------------------------------------------------------
