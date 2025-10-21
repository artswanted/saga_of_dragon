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

#pragma once

#include "MSceneGraph.h"
#include "MAnimation.h"
#include "MRenderer.h"
#include "MTimeManager.h"
#include "MMessageManager.h"
#include "MStatisticsManager.h"
#include "MUIManager.h"
#include "MAudioMan.h"
#include "MEffectMan.h"
#include "MAttachPointList.H"

namespace NiManagedToolInterface
{
    // MFramework class.
    public __gc class MFramework
    {
    public:
        // Properties.
        __property static MFramework* get_Instance();
        __property MSceneGraph* get_SceneGraph();
        __property MAnimation* get_Animation();
        __property MRenderer* get_Renderer();
        __property MTimeManager* get_Clock();
        __property MMessageManager* get_Output();
        __property MUIManager* get_Input();
		__property MAudioMan* get_AudioMan();
		__property MEffectMan *get_EffectMan();
        __property String* get_AppStartupPath();
        __property MAttachPointList* get_AttachPointList();

        static void Init(String* strAppStartupPath);
        static void Shutdown();
        static bool InstanceIsValid();

        void DeleteContents();
        void Update();

		unsigned int m_uiInitialCount;
		unsigned int m_uiFinalCount;

    protected:
        MFramework();

        String* m_strAppStartupPath;
        bool m_bUpdating;

        // Static this pointer for Singleton.
        static MFramework* ms_pkThis = NULL;
        MSceneGraph* m_pkSceneGraph;
        MAnimation* m_pkAnimation;
        MRenderer* m_pkRenderer;
        MTimeManager* m_pkTimeManager;
        MMessageManager* m_pkMessageManager;
        MStatisticsManager* m_pkStatManager;
        MUIManager* m_pkUIManager;
		MAudioMan* m_pkAudioMan;
		MEffectMan* m_pkEffectMan;
		MAttachPointList*	m_pkAttachPointList;
    };
}
