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

#include "MSceneObject.h"
#include "MSequence.h"
#include "MItem.h"
#include "PgItemMan.h"
#include "MSequenceGroup.h"
#include "MTimeManager.h"
#include "MCompletionInfo.h"

using namespace System::Collections;

namespace NiManagedToolInterface
{

    public __gc class ModelNIFFailedLoadArgs
    {
        public:
            String* strNIFFilename;
            String* strModelRootName;
            bool bCancel;
    };

    public __gc class KFFFailedLoadArgs
    {
        public:
            ArrayList* aMissingSequences;
            bool bCancel;
    };

    public __gc class MAnimation
    {
    public:
        static const float INVALID_TIME = -NI_INFINITY;

        __value enum MANIMATION_RC
        {
            MANIMATION_ERR_KFMTOOL_ERROR = 0,
            MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED,
            MANIMATION_ERR_FILENAME_EMPTY,
            MANIMATION_ERR_KF_FILENAME_EMPTY,
            MANIMATION_ERR_NO_NIF_ROOT_EXIST,
            MANIMATION_ERR_NO_KFMTOOL,
            MANIMATION_ERR_MODEL_PATH_EMPTY,
            MANIMATION_ERR_FAILED_LOADING_NIF,
            MANIMATION_ERR_NIF_OBJECT_NOT_NIOBJECTNET,
            MANIMATION_ERR_NIF_NIF_OBJECT_NO_NAME_FOR_FILE,
            MANIMATION_ERR_FAILED_TO_RELOAD_NIF,
            MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH,
            MANIMATION_ERR_SEQUENCE_DOES_NOT_EXIST,
            MANIMATION_ERR_SRC_SEQUENCE_DOES_NOT_EXIST,
            MANIMATION_ERR_SRC_OR_DEST_DOES_NOT_EXIST,
            MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST,
            MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST,
            MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF,
            MANIMATION_ERR_SEQUENCE_CAN_NOT_BE_LOADED
        };

        __value enum PlaybackMode
        {
            Uninitialized,
            None,
            Sequence,
            SequenceGroup,
            Transition,
            Interactive
        };

		__value enum EventType
		{
			TextKeyEvent,
			EffectEvent,
			SoundEvent
		};

        // Events
        
        __event void OnModelNIFLoadFailed(String* strKFMFilename,
            ModelNIFFailedLoadArgs* pkArgs);
        __event void OnKFLoadFailed(KFFFailedLoadArgs* pkArgs);
        __event void OnNewKFM();
        __event void OnKFMLoaded();
        __event void OnKFMSaved();
        __event void OnModelPathAndRootChanged(String* strNewModelPath,
            String* strNewModelRoot);
        __event void OnSequenceAdded(MSequence* pkNewSequence);
        __event void OnSequenceRemoved(unsigned int uiSequenceID);
        __event void OnSequenceModified(MSequence::PropertyType ePropChanged,
            MSequence* pkSequence);
        __event void OnTransitionAdded(MTransition* pkNewTransition);
        __event void OnTransitionRemoved(
            MTransition* pkTransitionToBeRemoved);
        __event void OnSequenceGroupAdded(MSequenceGroup* pkNewGroup);
        __event void OnSequenceGroupRemoved(
            MSequenceGroup* pkGroupToBeRemoved);
        __event void OnSequenceGroupModified(MSequenceGroupEventArgs* pkArgs,
            MSequenceGroup* pkSequenceGroup);
        __event void OnSequenceGroup_SequenceInfoModifiedEvent(
            MSequenceGroup::MSequenceInfo::PropertyType ePropChanged, 
            MSequenceGroup::MSequenceInfo* pkSequenceInfo,
            MSequenceGroup* pkSequenceGroup);
        __event void OnPlaybackModeChanged(PlaybackMode eMode);
        __event void OnSequenceActivated(MSequence* pkSequence);
        __event void OnSequenceDeactivated(MSequence* pkSequence);
        __event void OnTargetAnimationSet(MSequence* pkSequence);
        __event void OnResetAnimations();
        __event void OnTransitionModified(
            MTransition::PropertyType ePropChanged,
            MTransition* pkTransition);
        __event void OnDefaultTransitionSettingsChanged(
            MTransition::TransitionType eChangedType);
        __event void OnActorManagerCreated();

        // General properties.
        __property ArrayList* get_Sequences();
        __property ArrayList* get_SequenceGroups();
        __property String* get_ModelPath();
        __property String* get_FullModelPath();
        __property String* get_ModelRoot();
        __property String* get_KFMFilename();
        __property String* get_BaseKFMPath();
        __property void set_BaseKFMPath(String* strPath);
		__property float get_DefaultSeqGroupWeight();
		__property unsigned int get_DefaultSeqGroupPriority();
		__property float get_DefaultSeqGroupEaseIn();
		__property float get_DefaultSeqGroupEaseOut();
        __property bool get_NeedToSave();
        __property bool get_CanSave();
        __property ArrayList* get_UnresolvedSequenceInfo();
	
        // PlaybackMode properties.
        __property PlaybackMode get_Mode();
        __property float get_CurrentTime();
        __property void set_CurrentTime(float fCurrentTime);
        __property float get_IncrementTime();
        __property void set_IncrementTime(float fTime);

        // PlaybackMode::Sequence properties.
        __property MSequence* get_ActiveSequence();

        // PlaybackMode::SequenceGroup properties.
        __property MSequenceGroup* get_ActiveSequenceGroup();

        // PlaybackMode::Transition properties.
        __property MTransition* get_ActiveTransition();
        __property float get_TransitionStartTime();
        __property void set_TransitionStartTime(float fTransitionStartTime);
        __property unsigned int get_LoopCounterRange();
        __property void set_LoopCounterRange(unsigned int uiLoopCounterRange);
        __property MTransition::TransitionType get_DefaultSyncTransType();
        __property void set_DefaultSyncTransType(
            MTransition::TransitionType eType);
        __property MTransition::TransitionType get_DefaultNonSyncTransType();
        __property void set_DefaultNonSyncTransType(
            MTransition::TransitionType eType);
        __property float get_DefaultSyncTransDuration();
        __property void set_DefaultSyncTransDuration(float fDuration);
        __property float get_DefaultNonSyncTransDuration();
        __property void set_DefaultNonSyncTransDuration(float fDuration);

        // PlaybackMode::Interactive properties.
        __property MSequence* get_TargetAnimation();
        
        // Constructor.
        MAnimation(MTimeManager* pkClock);

		// Initializer
		bool Initialize();

        void Shutdown();

        void Update(float fTime);

        static const char* LookupReturnCode(MANIMATION_RC eReturnCode);

        // Streaming functions.
        void NewKFM();
        bool LoadKFM(String* strFilename);
        bool SaveKFM(String* strFilename);
        bool LoadAndAddKF(String* strFilename);
        void ReloadNIF();
        ArrayList* GetSequencesNamesFromFile(String* strFilename);

        // Editing functions.
        bool SetModelPathAndRoot(String* strModelPath, String* strModelRoot);
		bool AddToItemList(String *strPartPath, String *strItemFolder);
		bool RemoveFromItemList(String *strPartPath, String *strItemFolder);

        ArrayList* GetModelRootConflicts();
        ArrayList* GetModelRootConflictsForSequence(MSequence* pkSequence);
        MSequence* AddSequence(unsigned int uiSequenceID, 
            String* strFilename, int iAnimIndex);
        void RemoveSequence(unsigned int uiSequenceID);
        MTransition* AddTransition(unsigned int uiSrcID, unsigned int uiDesID,
            MTransition::TransitionType eType, float fDuration);
        void RemoveTransition(unsigned int uiSrcID, unsigned int uiDesID);
        MSequenceGroup* AddSequenceGroup(unsigned int uiGroupID,
            String* strName);
        void RemoveSequenceGroup(unsigned int uiGroupID);
        void RemoveChainsContainingTransitions(
            unsigned int uiSrcID, unsigned int uiDesID);
        NiKFMTool::Transition* ChangeTransitionType(unsigned int uiSrcID,
            unsigned int uiDesID, NiKFMTool::Transition* pkTransition,
            NiKFMTool::TransitionType eNewType);

        // Member access.
        MSequence* GetSequence(unsigned int uiSequenceID);
        MSequence* GetSequenceByName(String* strName);
        MSequenceGroup* GetSequenceGroup(unsigned int uiGroupID);

		// Parts
		bool AttachParts(MItem *pkParts, int iGender, int iClass);
		bool AttachParts(PgItem *pkItem);
		int AttachSkinningParts(PgItem *pkItem, NiNode *pkModelRoot);
		bool AttachNoSkinningParts(PgItem *pkParts, NiNode *pkModelRoot, const char *pcTargetDummy);
		bool DetachParts(MItem *pkParts, int iGender, int iClass);
		bool DetachParts(PgItem *pkItem);
		int SkinningParts(PgItem *pkParts, NiNode *pkItemRoot, NiNode *pkModelRoot, bool bAttach);
		void UnskinningParts(NiNode *pkItemRoot, NiAVObject *pkAttachedNode);

		// TextKey
		bool AddTextKeyEvent(String *pkText, float fTime);
		bool AddEffectEvent(String *pkText, float fTime, 
			String *pkEffectName, String *pkAttachPoint,
			float fScale, bool bAttach, bool bLoop);
		bool AddSoundEvent(String *pkText, float fTime,
			String *pkSoundName, float fVolume, 
			float fMinDist, float fMaxDist);
		bool ModifyEventTime(String *pkText, 
			float fTime, float fNewTime, EventType eEvent);
		bool RemoveEvent(String *pkText,
			float fTime, EventType eEvent);

		ArrayList *GetItemsByFolder(String *strItemFolder);

		bool DetachAllPartsFromModel();
		bool SaveItemList();
        
        // Playback modes.
        bool SetPlaybackMode(PlaybackMode eMode, Object* pkModeData);

        // Activation functions.
        bool ActivateSequence(unsigned int uiSequenceID);
        bool ActivateSequence(unsigned int uiSequenceID, int iPriority,
            float fWeight, float fEaseInTime);
        bool ActivateSequence(unsigned int uiSequenceID, int iPriority,
            float fWeight, float fEaseInTime, unsigned int uiSyncToSequenceID);

        // Deactivation functions.
        bool DeactivateSequence(unsigned int uiSequenceID);
        bool DeactivateSequence(unsigned int uiSequenceID,
            float fEaseOutTime);

        // Query runtime functions
        float GetSequenceWeight(unsigned int uiSequenceID);
        void SetSequenceWeight(unsigned int uiSequenceID, float fWeight);

        // Set target animation of NiActorManager.
        bool SetTargetAnimation(unsigned int uiSequenceID);

        // Turn off all sequences.
        void ResetAnimations();

        // Turn off all sequences and optionally reset accumulated transforms.
        void ResetAnimations(bool bResetAccum);

        // Force a runup
        void RunUpTime(float fTime);

        // Create unique methods
        String* FindUnusedSequenceName(String* strBaseName);
        String* FindUnusedSequenceGroupName(String* strBaseName);       
        unsigned int FindUnusedGroupID();
        unsigned int FindUnusedSequenceID();

        float FindTimeForAnimationToComplete(MSequence* pkSequence, 
            float fBeginTime);
        MCompletionInfo* FindTimeForAnimationToCompleteTransition(
            MTransition* pkTransition, float fBeginTime, 
            float fDesiredTransitionTime);

        // Throw Events (Emergent internal use only)
		void ThrowSequenceModifiedEvent(MSequence::PropertyType,
            MSequence* pkSequence);
        void ThrowSequenceGroupModifiedEvent(MSequenceGroupEventArgs* pkArgs,
            MSequenceGroup* pkSequenceGroup);
        void ThrowSequenceGroupSequenceInfoModifiedEvent(
            MSequenceGroup::MSequenceInfo::PropertyType ePropChanged, 
            MSequenceGroup::MSequenceInfo* pkSequenceInfo, 
            MSequenceGroup* pkSequenceGroup);
        void ThrowTransitionModifiedEvent(
            MTransition::PropertyType ePropChanged, 
            MTransition* pkTransition);
        bool ChangeSequenceID(unsigned int uiOldID, unsigned int uiNewID);
        bool ChangeGroupID(unsigned int uiOldID, unsigned int uiNewID);

        String* GetLastErrorString();
        MANIMATION_RC GetLastErrorCode();

        String* GetFullKFFilename(unsigned int uiSequenceID);

        void AddDefaultTransitionsForSequence(MSequence* pkSequence);
        void RemoveTransitionsContainingSequence(unsigned int uiSequenceID);

		void SetInitialTransitionTimes();

    protected:   
        NiActorManager* CreateActorManager();

        void SetLastErrorString(String* strError);
        void SetLastErrorCode(MANIMATION_RC rc);

        String* m_LastErrorString;
        MANIMATION_RC m_LastErrorCode;
        
		MItem *GetItemByPath(ArrayList *pkItems, String *pkPath);

        void AddToSequenceArray(MSequence* pkSequence, bool bTrimToSize);
        void AddToSequenceGroupArray(MSequenceGroup* pkSequenceGroup, 
            bool bTrimToSize);
        void RemoveFromSequenceArray(MSequence* pkSequence);
        void RemoveFromSequenceGroupArray(MSequenceGroup* pkSequenceGroup);
        void BuildSequenceArray();
        void BuildSequenceGroupArray();

        void SetInitialTransitionTimeRowCol(MSequence* pkSequence);
        void SetInitialTransitionTimeRow(MSequence* pkSequence);
        void SetInitialTransitionTimeCol(MSequence* pkSequence);


		void ClearAllItemArray();
        void ClearSequenceArray();
        void ClearSequenceGroupArray();

        void HandleUnresolvedSequences();
        void OnRunUpTime(float fEndTime);

		//! Item
		PgItemMan *m_pkItemMan;

		ArrayList* m_aAllItems;
		ArrayList* m_aFolders;
		Hashtable* m_mapFolderToItems;
		Hashtable* m_mapPathToItem;

		MTimeManager* m_pkClock;
        float m_fTimeIncrement;

		ArrayList* m_aSequences;
        Hashtable* m_mapIDToSequence;

        ArrayList* m_aSequenceGroups;
        Hashtable* m_mapIDToSequenceGroup;

        ArrayList* m_aUnresolvedSequences;

        PlaybackMode m_eMode;
        Object* m_pkModeData;
        float m_fLastTime;
        float m_fStartTime;
        float m_fTransitionStartTime;

        String* m_strKFMFilename;

		float m_fDefaultSeqGroupWeight;
		unsigned int m_uiDefaultSeqGroupPriority;
		float m_fDefaultSeqGroupEaseIn;
		float m_fDefaultSeqGroupEaseOut;
    
        bool m_bNeedToSave;
        bool m_bCanSave;

        __nogc class CallbackObject : public NiActorManager::CallbackObject
        {
        public:
            CallbackObject() : m_bCallbackReceived(false), m_uiCounterRange(1)
            {
                m_uiCounter = m_uiCounterRange;
            }

            virtual void AnimActivated(NiActorManager* pkManager,
                NiActorManager::SequenceID eSequenceID, float fCurrentTime,
                float fEventTime){}
            virtual void AnimDeactivated(NiActorManager* pkManager,
                NiActorManager::SequenceID eSequenceID, float fCurrentTime,
                float fEventTime){}
            virtual void TextKeyEvent(NiActorManager* pkManager,
                NiActorManager::SequenceID eSequenceID, 
                const NiFixedString& kTextKey,
                const NiTextKeyMatch* pkMatchObject, float fCurrentTime,
                float fEventTime){}

            virtual void EndOfSequence(NiActorManager* pkManager,
                unsigned int uiSequenceID, float fCurrentTime,
                float fEventTime)
            {
                if (pkManager->GetTransitionState() ==
                    NiActorManager::NO_TRANSITION)
                {
                    if (--m_uiCounter == 0)
                    {
                        m_bCallbackReceived = true;
                        m_fEventTime = fEventTime;
                        m_uiCounter = m_uiCounterRange;
                    }
                }
            }

            bool CallbackReceived(float& fEventTime)
            {
                bool bCallbackReceived = m_bCallbackReceived;
                if (bCallbackReceived)
                {
                    fEventTime = m_fEventTime;
                }
                m_bCallbackReceived = false;
                return bCallbackReceived;
            }

            unsigned int GetCounterRange() const
            {
                return m_uiCounterRange;
            }

            void SetCounterRange(unsigned int uiCounterRange)
            {
                m_uiCounterRange = uiCounterRange;
            }

        protected:
            bool m_bCallbackReceived;
            float m_fEventTime;
            unsigned int m_uiCounter;
            unsigned int m_uiCounterRange;
        };
        CallbackObject* m_pkCallbackObject;

		__nogc class EventCallbackObject : public NiActorManager::CallbackAVObject
		{
		public:
			EventCallbackObject()
			{
			}

			virtual ~EventCallbackObject();
			virtual void AudioActivated(NiActorManager *pkManager,
				NiActorManager::SequenceID eSequenceID, float fCurrentTime, float fEventTime, 
				NiKFMTool::Sound *pkSoundData);
			virtual void EffectActivated(NiActorManager *pkManager, 
				NiActorManager::SequenceID eSequenceID, float fCurrentTime, float fEventTime, 
				NiKFMTool::Effect *pkEffectData);

		protected:
			float m_fEventTime;
		};
		EventCallbackObject *m_pkEventCallback;
    };
}
