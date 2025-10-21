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

namespace NiManagedToolInterface
{
	public __gc class MAudioMan;
	public __gc class MEffectMan;
    public __gc class MSharedData
    {
    public:
		typedef std::map<int, NiActorManagerPtr> ActorManagerContainer;

        static const unsigned int BACKGROUND_INDEX = 0;
        static const unsigned int CHARACTER_INDEX = 1;
        static const unsigned int MAIN_CAMERA_INDEX = 2;
        static const unsigned int ORBIT_INDEX = 3;
        static const unsigned int USER_INDEX_BASE = 4;

        // Events.
        __event void OnSceneAdded(unsigned int uiIndex, NiAVObject* pkScene);
        __event void OnSceneRemoved(unsigned int uiIndex,
            NiAVObject* pkScene);
        __event void OnSceneChanged(unsigned int uiIndex,
            NiAVObject* pkOldScene, NiAVObject* pkNewScene);
        __event void OnSceneArrayCompacted();
        __event void OnActorManagerChanged(NiActorManager* pkOldActorManager,
            NiActorManager* pkNewActorManager);
        __event void OnKFMToolChanged(NiKFMTool* pkOldKFMTool,
            NiKFMTool* pkNewKFMTool);

        // Properties.
        __property static MSharedData* get_Instance();

        static void Init();
        static void Shutdown();

        // Locks and unlocks data access.
        void Lock();
        void Unlock();

        // Clears the values for all data members.
        void DeleteContents();

        // Scene array access.
        unsigned int GetSceneArrayCount();
        unsigned int AddScene(NiAVObject* pkScene);
        void RemoveScene(unsigned int uiIndex);
        NiAVObject* GetScene(unsigned int uiIndex);
        void SetScene(unsigned int uiIndex, NiAVObject* pkScene);
        void CompactSceneArray();

        // Actor manager access.
        void SetActorManager(NiActorManager* pkActorManager);
        NiActorManager* GetActorManager();

		// Supplement ActorManager
		void AddActorManager(int iAMPos, NiActorManagerPtr pkAM);
		NiActorManager *GetActorManager(int iAMPos);
		void RemoveActorManager(int iAMPos);
		void RemoveAllActorManager();
		void UpdateSupplementActorManager(float fAccumTime);
		void SetTargetAnimation(NiActorManager::SequenceID eSequence);

        // KFMTool access.
        void SetKFMTool(NiKFMTool* pkKFMTool);
        NiKFMTool* GetKFMTool();

		// MAudioMan access.
		MAudioMan *GetAudioMan();
		void SetAudioMan(MAudioMan *pkAudioMan);

		// MEffectMan access.
		MEffectMan *GetEffectMan();
		void SetEffectMan(MEffectMan *pkEffectMan);

    protected:
        MSharedData();

        NiTPrimitiveArray<NiAVObject*>* m_aScenes;
        NiActorManager* m_pkActorManager;
        NiKFMTool* m_pkKFMTool;

        unsigned int m_uiLockCount;
		ActorManagerContainer *m_pkSupplementAMContainer;

		MAudioMan *m_pkAudioMan;
		MEffectMan *m_pkEffectMan;

        static MSharedData* ms_pkThis = NULL;
    };
}
