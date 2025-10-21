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
#include "MFramework.h"
#include "MSharedData.h"
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MFramework::MFramework() : m_bUpdating(false)
{
    m_pkMessageManager = NiExternalNew MMessageManager();
    m_pkSceneGraph = NiExternalNew MSceneGraph;
    m_pkRenderer = NiExternalNew MRenderer;
    m_pkTimeManager = NiExternalNew MTimeManager();
	m_pkAudioMan = NiExternalNew MAudioMan();
	m_pkEffectMan = NiExternalNew MEffectMan();
    m_pkAnimation = NiExternalNew MAnimation(m_pkTimeManager);
    m_pkStatManager = NiExternalNew MStatisticsManager();
    m_pkUIManager = NiExternalNew MUIManager();
    m_pkUIManager->SetCamera(m_pkRenderer->ActiveCamera);
    m_strAppStartupPath = NULL;
	m_pkAttachPointList = NiExternalNew MAttachPointList();
}
//---------------------------------------------------------------------------
void MFramework::Init(String* strAppStartupPath)
{
    if (!ms_pkThis)
    {
        NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
            NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),
            false)
#else
            NiExternalNew NiStandardAllocator()
#endif
            );

        NiInit(pkInitOptions);

        MSharedData::Init();
        ms_pkThis = NiExternalNew MFramework;

	    // for determining if there are 'object leaks'
	    ms_pkThis->m_uiInitialCount = NiRefObject::GetTotalObjectCount();
        ms_pkThis->m_strAppStartupPath = strAppStartupPath;
        
        // Seed the start time of the application
        MUtility::GetCurrentTimeInSec();
        NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
        ms_pkThis->m_pkRenderer->Init(strAppStartupPath, 
            ms_pkThis->m_pkStatManager);
        ms_pkThis->m_pkUIManager->Init();
		ms_pkThis->m_pkAudioMan->Init();
		ms_pkThis->m_pkEffectMan->Init();
		ms_pkThis->m_pkAttachPointList->Init(strAppStartupPath);
    }
}
//---------------------------------------------------------------------------
void MFramework::Shutdown()
{
    if (ms_pkThis)
    {

		ms_pkThis->m_pkAttachPointList->Shutdown();
        ms_pkThis->m_pkUIManager->Shutdown();
        ms_pkThis->m_pkStatManager->ResetRoots();
        ms_pkThis->m_pkRenderer->Shutdown();
		ms_pkThis->m_pkAnimation->Shutdown();
		ms_pkThis->m_pkAudioMan->ShutDown();
		ms_pkThis->m_pkEffectMan->ShutDown();
        ms_pkThis->DeleteContents();

        MSharedData::Shutdown();

		// for determining if there are 'object leaks'
		ms_pkThis->m_uiFinalCount = NiRefObject::GetTotalObjectCount();
		//char acMsg[256];
		//NiSprintf(acMsg, 256,
		//	"\nGamebryo NiRefObject counts:  initial = %u, final = %u. ", 
		//	ms_pkThis->m_uiInitialCount, ms_pkThis->m_uiFinalCount);
		//NiMessageBox(acMsg, "Appz");
		//if (m_uiFinalCount > m_uiInitialCount)
		//{
		//    unsigned int uiDiff = m_uiFinalCount - m_uiInitialCount;
		//    NiSprintf(acMsg, 256, "Application is leaking %u objects\n\n", uiDiff);
		//    NiMessageBox(acMsg, "Appz");
		//}
		//else
		//{
		//    NiMessageBox("Application has no object leaks.\n\n", "Appz");
		//}

        ms_pkThis = NULL;
        NiImageConverter::SetImageConverter(NULL);

        const NiInitOptions* pkInitOptions = NiStaticDataManager
            ::GetInitOptions();
        NiShutdown();
        NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
        NiExternalDelete pkInitOptions;
        NiExternalDelete pkAllocator;
    }
}
//---------------------------------------------------------------------------
bool MFramework::InstanceIsValid()
{
    return (ms_pkThis != NULL);
}
//---------------------------------------------------------------------------
void MFramework::DeleteContents()
{
    m_pkUIManager->DeleteContents();
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    pkData->DeleteContents();
    pkData->Unlock();
    m_pkUIManager->DeleteContents();
    m_pkTimeManager = NULL;
	m_pkEffectMan->DeleteContents();
	m_pkAudioMan->DeleteContents();
    m_pkRenderer->DeleteContents();
}
//---------------------------------------------------------------------------
void MFramework::Update()
{
    if (m_bUpdating)
    {
        return;
    }

    m_bUpdating = true;

    m_pkTimeManager->UpdateTime();
    float fTime = m_pkTimeManager->CurrentTime;
    m_pkAnimation->Update(fTime);
    m_pkUIManager->Update();
    m_pkRenderer->ClearClickSwap();

    m_bUpdating = false;
}
//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
MFramework* MFramework::get_Instance()
{
    assert(ms_pkThis != NULL);
    return ms_pkThis;
}
//---------------------------------------------------------------------------
MSceneGraph* MFramework::get_SceneGraph()
{
    return m_pkSceneGraph;
}
//---------------------------------------------------------------------------
MAnimation* MFramework::get_Animation()
{
    return m_pkAnimation;
}
//---------------------------------------------------------------------------
MRenderer* MFramework::get_Renderer()
{
    return m_pkRenderer;
}
//---------------------------------------------------------------------------
MTimeManager* MFramework::get_Clock()
{
    return m_pkTimeManager;
}
//---------------------------------------------------------------------------
MMessageManager* MFramework::get_Output()
{
    return m_pkMessageManager;
}
//---------------------------------------------------------------------------
MUIManager* MFramework::get_Input()
{
    return m_pkUIManager;
}
//---------------------------------------------------------------------------
String* MFramework::get_AppStartupPath()
{
    return m_strAppStartupPath;
}
//---------------------------------------------------------------------------
MAudioMan *MFramework::get_AudioMan()
{
	return m_pkAudioMan;
}
//---------------------------------------------------------------------------
MEffectMan *MFramework::get_EffectMan()
{
	return m_pkEffectMan;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MAttachPointList *MFramework::get_AttachPointList()
{
	return m_pkAttachPointList;
}
//---------------------------------------------------------------------------
