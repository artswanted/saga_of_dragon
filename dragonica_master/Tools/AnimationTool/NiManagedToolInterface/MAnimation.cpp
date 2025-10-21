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
#include "MAnimation.h"
#include "MSharedData.h"
#include "MSceneGraph.h"
#include "MAudioMan.h"
#include "MEffectMan.h"
#include "PgItem.h"
#include "PgEffectProcessorFadeIn.H"
#include "PgEffectProcessorFadeOut.H"

using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MAnimation::MAnimation(MTimeManager* pkClock) : m_pkClock(pkClock),
    m_fTimeIncrement(0.0333f), m_mapIDToSequence(NULL),
    m_mapIDToSequenceGroup(NULL), m_eMode(PlaybackMode::Uninitialized),
    m_pkModeData(NULL), m_fLastTime(-NI_INFINITY), m_fStartTime(-NI_INFINITY),
    m_fTransitionStartTime(INVALID_TIME), m_strKFMFilename(NULL),
    m_fDefaultSeqGroupWeight(1.0f), m_uiDefaultSeqGroupPriority(0),
    m_fDefaultSeqGroupEaseIn(0.25f), m_fDefaultSeqGroupEaseOut(0.25f),
    m_pkCallbackObject(NULL), m_pkEventCallback(NULL)
{
	m_aAllItems = NiExternalNew ArrayList(0);
	m_aFolders = NiExternalNew ArrayList(0);
    m_aSequences = NiExternalNew ArrayList(0);
    m_aSequenceGroups = NiExternalNew ArrayList(0);
	m_mapFolderToItems = NiExternalNew Hashtable();
    m_mapIDToSequence = NiExternalNew Hashtable();
    m_mapIDToSequenceGroup = NiExternalNew Hashtable();
    m_pkCallbackObject = NiNew CallbackObject;
	m_pkEventCallback = NiNew EventCallbackObject;
    m_aUnresolvedSequences = NiExternalNew ArrayList();
	m_pkItemMan = NiExternalNew PgItemMan();

    __hook(&MTimeManager::OnRunUpTime, m_pkClock, 
		&NiManagedToolInterface::MAnimation::OnRunUpTime);
    m_bNeedToSave = false;
    m_bCanSave = false;

	Initialize();
}
bool MAnimation::Initialize()
{
	return true;
}
//---------------------------------------------------------------------------
void MAnimation::Shutdown()
{
	DetachAllPartsFromModel();

    m_bNeedToSave = false;
    m_bCanSave = false;
    NiDelete m_pkCallbackObject;
	NiDelete m_pkEventCallback;
    m_pkCallbackObject = NULL;
	m_pkEventCallback = NULL;

	// PartList를 XML로 저장한다.
	SaveItemList();
	ClearAllItemArray();
    ClearSequenceArray();
    ClearSequenceGroupArray();

	NiExternalDelete m_pkItemMan;
}
//---------------------------------------------------------------------------
NiActorManager* MAnimation::CreateActorManager()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = NiActorManager::Create(
        pkData->GetKFMTool(),
        pkData->GetKFMTool()->GetBaseKFMPath(), true);
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);
        pkData->Unlock();
        return NULL;
    }
    pkData->SetActorManager(pkActorManager);
    pkData->SetScene(MSharedData::CHARACTER_INDEX,
        pkActorManager->GetNIFRoot());
    m_bNeedToSave = true;
    pkData->Unlock();

    OnActorManagerCreated();
    return pkActorManager;
}
//---------------------------------------------------------------------------
void MAnimation::Update(float fTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        switch (m_eMode)
        {
            case PlaybackMode::Transition:
            {
                MTransition* pkTransition = static_cast<MTransition*>(
                    m_pkModeData);
                if (pkActorManager->GetTargetAnimation() ==
                    pkTransition->SrcID && fTime > m_fTransitionStartTime)
                {
                    bool bSuccess = SetTargetAnimation(pkTransition->DesID);
                    MAssert(bSuccess);
                    pkActorManager->SetCallbackObject(m_pkCallbackObject);
                    pkActorManager->RegisterCallback(
                        NiActorManager::END_OF_SEQUENCE, pkTransition->DesID);
                }
                else
                {
                    float fEventTime;
                    if (m_pkCallbackObject->CallbackReceived(fEventTime))
                    {
                        bool bSuccess = SetPlaybackMode(
                            PlaybackMode::Transition, pkTransition);
                        MAssert(bSuccess);
                        pkData->Unlock();
                        return;
                    }
                }

                break;
            }
        }

        pkActorManager->Update(fTime);
    }

	// 다른 애니도 플레이.
	pkData->UpdateSupplementActorManager(fTime);

    NiAVObject* pkScene = pkData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkScene)
    {
        pkScene->Update(fTime);
    }

    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
        {
            MSequence* pkSequence = static_cast<MSequence*>(m_pkModeData);
            float fScaledTime = pkSequence->LastScaledTime;
            if (m_fStartTime == -NI_INFINITY)
            {
                pkSequence->StoreAccum();
                m_fStartTime = fTime;
            }
            if (fTime == m_fStartTime + pkSequence->Length)
            {
                fScaledTime = pkSequence->EndKeyTime;
            }
            if (m_fLastTime == -NI_INFINITY)
            {
                m_fLastTime = fTime - m_fStartTime; 
            }

            if (fScaledTime < m_fLastTime)
            {
                // We have looped in the engine.

                if (pkSequence->Loop)
                {
                    if (m_fLastTime >= pkSequence->Length)
                    {   
                        // Because of existence of phase, we may not really
                        // have gone a full cycle... we want to detect when
                        // ourlasttime exceeds or equals the length and then
                        // reset.    
                        pkSequence->StoreAccum();
                        m_fStartTime += pkSequence->Length;
                    }
                }
            }
            m_fLastTime = fTime - m_fStartTime; 

            break;
        }
        case PlaybackMode::Transition:
        {
            MAssert(pkActorManager != NULL);
            MSequence* pkSequence = GetSequence(pkActorManager
                ->GetTargetAnimation());
            MAssert(pkSequence != NULL);

            if (m_fLastTime == -NI_INFINITY)
            {
                m_fLastTime = fTime;
            }
            if (m_fStartTime == -NI_INFINITY)
            {
                m_fStartTime = fTime;
            }
            if (fTime < m_fLastTime)
            {
                // We have looped in Engine

                if (m_fLastTime >= pkSequence->Length)
                {
                    // Because of existence of phase, we may not really 
                    // have gone a full cycle... we want to detect when our
                    // lasttime exceeds or equals the length and then reset.

                    pkActorManager->GetControllerManager()
                        ->ClearCumulativeAnimations();
                    m_fStartTime = fTime;
                }
            }
            m_fLastTime = fTime;

            break;
        }
    }

	// Audio System Update
	NiMilesAudioSystem* pkAudioSystem = (NiMilesAudioSystem *)NiAudioSystem::GetAudioSystem();	
	if(pkAudioSystem && pkAudioSystem->GetHWnd())
	{
		pkAudioSystem->Update(fTime, true);
	}

	// Effect System Update
	MEffectMan *pkEffectMan = pkData->GetEffectMan();
	if(pkEffectMan)
	{
		pkEffectMan->Update(m_fLastTime);
	}

    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::NewKFM()
{
	DetachAllPartsFromModel();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

	m_eMode = PlaybackMode::None;
	
    pkData->SetScene(MSharedData::CHARACTER_INDEX, NULL);
    pkData->SetActorManager(NULL);
    pkData->SetKFMTool(NULL);
    NiKFMTool* pkKFMTool = NiNew NiKFMTool;
    pkData->SetKFMTool(pkKFMTool);
    ClearSequenceArray();
    ClearSequenceGroupArray();
    HandleUnresolvedSequences();
    BuildSequenceArray();
    BuildSequenceGroupArray();

    SetInitialTransitionTimes();

    m_strKFMFilename = NULL;
    m_bCanSave = false;
    m_bNeedToSave = true;
    pkData->Unlock();

    OnNewKFM();
}
//---------------------------------------------------------------------------
bool MAnimation::LoadKFM(String* strFilename)
{
    if (strFilename == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_FILENAME_EMPTY);
        return false;
    }

	DetachAllPartsFromModel();

    const char* pcFilename = MStringToCharPointer(strFilename);
    NiKFMToolPtr spKFMTool = NiNew NiKFMTool;
    NiKFMTool::KFM_RC eRC = spKFMTool->LoadFile(pcFilename);
    
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString( NiKFMTool::LookupReturnCode(eRC));
        MFreeCharPointer(pcFilename);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();


    NiActorManager* pkActorManager = 
        NiActorManager::Create(spKFMTool, pcFilename, true);

    bool bChangedNIF = false;
    if (!pkActorManager)
    {
        // If the actor manager was not created, the NIF file could not be
        // loaded, either because it is missing or corrupt.
        //
        // Give the user a chance find the right NIF file
        ModelNIFFailedLoadArgs* pkArgs = NiExternalNew ModelNIFFailedLoadArgs;
        pkArgs->strNIFFilename = spKFMTool->GetFullModelPath();

        pkData->Unlock();
        OnModelNIFLoadFailed(pcFilename, pkArgs);
        pkData->Lock();

        if (!pkArgs->bCancel)
        {
            const char* pcModelPath = 
                MStringToCharPointer(pkArgs->strNIFFilename);
            spKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            const char* pcModelRoot = 
                MStringToCharPointer(pkArgs->strModelRootName);
            spKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

            pkActorManager = 
                NiActorManager::Create(spKFMTool, pcFilename, true);
            bChangedNIF = true;
        }
    }

    MFreeCharPointer(pcFilename);
    
    if (!pkActorManager)
    {
        // If the actor manager was not created, the NIF file could not be
        // loaded, either because it is missing or corrupt.
        SetLastErrorCode(MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF);
        SetLastErrorString(
            String::Concat("Unable to load character NIF file at ",
            spKFMTool->GetFullModelPath(), "."));
        pkData->Unlock();
        return false;
    }

    NiAVObject* pkNIFRoot = pkActorManager->GetNIFRoot();
    if (!pkNIFRoot)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_NIF_ROOT_EXIST);
        pkData->Unlock();
        return false;
    }

    m_bCanSave = true;
    m_bNeedToSave = bChangedNIF;

	pkActorManager->SetCallbackAVObject(m_pkEventCallback);
    pkData->SetKFMTool(spKFMTool);
    pkData->SetActorManager(pkActorManager);
    pkData->SetScene(MSharedData::CHARACTER_INDEX, pkNIFRoot);

    HandleUnresolvedSequences();
    
    BuildSequenceArray();
    BuildSequenceGroupArray();

    SetInitialTransitionTimes();
   
    pkData->Unlock();
    m_strKFMFilename = strFilename;

    MLogger::LogGeneral(String::Concat("Loading: ", m_strKFMFilename));
    OnKFMLoaded();
    SetPlaybackMode(None, NULL);
    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::SaveKFM(String* strFilename)
{
    if (strFilename == String::Empty)
    {
         SetLastErrorCode(MANIMATION_ERR_FILENAME_EMPTY);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastError(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }
    const char* pcFilename = MStringToCharPointer(strFilename);
    NiKFMTool::KFM_RC eRC = pkKFMTool->SaveFile(pcFilename);
    MFreeCharPointer(pcFilename);
    pkData->Unlock();

    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        return false;
    }

    m_bNeedToSave = false;

    m_strKFMFilename = strFilename;

    MLogger::LogGeneral(String::Concat("Saving: ", m_strKFMFilename));
    OnKFMSaved();

    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::LoadAndAddKF(String* strSequencePath)
{
    ArrayList* pkList = GetSequencesNamesFromFile(strSequencePath);
    if (pkList == NULL)
        return false;

    MLogger::LogGeneral(String::Concat("Loading KF: ", strSequencePath));

    for (int i = 0; i < pkList->Count; i++)
    {
        String* strSeq = dynamic_cast<String*>(pkList->get_Item(i));
        if (GetSequenceByName(strSeq) != NULL)
            continue;

        if (AddSequence(FindUnusedSequenceID(), strSequencePath, i) == NULL)
            return false;

        m_bNeedToSave = true;
    }

    return true;
}
//---------------------------------------------------------------------------
void MAnimation::ReloadNIF()
{
    SetPlaybackMode(PlaybackMode::None, NULL);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkAM = pkData->GetActorManager();
    NiAVObject* pkRoot = pkData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkAM)
    {
        if (pkAM->ReloadNIFFile())
        {
            pkData->SetScene(MSharedData::CHARACTER_INDEX,
                pkAM->GetNIFRoot());
            OnActorManagerCreated();
        }
    }

    pkData->Unlock();
}

//---------------------------------------------------------------------------
ArrayList* MAnimation::GetSequencesNamesFromFile(String* strSequencePath)
{
    if (strSequencePath == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_KF_FILENAME_EMPTY);
        return NULL;
    }
    
    const char* pcFilename = MStringToCharPointer(strSequencePath);
    ArrayList* pkList = NULL; 
    
    {
        NiStream kStream;
        if (NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcFilename) == 0)
        {
            SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
            SetLastErrorString(kStream.GetLastErrorMessage());
            return NULL;
        }

        pkList = NiExternalNew ArrayList(kStream.GetObjectCount());
        for (unsigned int ui = 0; ui < kStream.GetObjectCount(); ui++)
        {
            NiControllerSequencePtr spSequence = NiControllerSequence
                ::CreateSequenceFromFile(kStream, ui);
            if (spSequence == NULL)
            {
                SetLastErrorCode(MANIMATION_ERR_SEQUENCE_CAN_NOT_BE_LOADED);
                SetLastErrorString(String::Concat("The sequence at ",
                    ui.ToString(), " could not be loaded."));
                return NULL;
            }
            pkList->Add(NiExternalNew String(spSequence->GetName()));
        }
    }
    return pkList;
}
//---------------------------------------------------------------------------
bool MAnimation::SetModelPathAndRoot(String* strModelPath,
    String* strModelRoot)
{
    if (strModelPath == NULL || strModelPath == String::Empty)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return false;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }

    // If model root passed into this function is empty, grab the name of
    // the first object in the NIF file.
    if (strModelRoot == NULL || strModelRoot == String::Empty)
    {
        const char* pcNIFPath = MStringToCharPointer(strModelPath);
        NiStream kStream;
        int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream,
            pcNIFPath);
        MFreeCharPointer(pcNIFPath);
        if (iSuccess == 0)
        {
            SetLastErrorCode(MANIMATION_ERR_FAILED_LOADING_NIF);
            pkData->Unlock();
            return false;
        }
        NiObjectNET* pkRoot = NiDynamicCast(NiObjectNET,
            kStream.GetObjectAt(0));
        if (!pkRoot)
        {
            SetLastErrorCode(MANIMATION_ERR_NIF_OBJECT_NOT_NIOBJECTNET);
            pkData->Unlock();
            return false;
        }
        if (!pkRoot->GetName())
        {
            SetLastErrorCode(MANIMATION_ERR_NIF_NIF_OBJECT_NO_NAME_FOR_FILE);
            pkData->Unlock();
            return false;
        }
        strModelRoot = pkRoot->GetName();
    }

    String* strOldModelPath = pkKFMTool->GetModelPath();
    const char* pcModelPath = MStringToCharPointer(strModelPath);
    pkKFMTool->SetModelPath(pcModelPath);
    MFreeCharPointer(pcModelPath);

    String* strOldModelRoot = pkKFMTool->GetModelRoot();
    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    pkKFMTool->SetModelRoot(pcModelRoot);
    MFreeCharPointer(pcModelRoot);

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkActorManager = CreateActorManager();
        if (!pkActorManager)
        {
            SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);

            pcModelPath = MStringToCharPointer(strOldModelPath);
            pkKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            pcModelRoot = MStringToCharPointer(strOldModelRoot);
            pkKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

            pkData->Unlock();
            return false;
        }
    }
    else
    {
        if (pkActorManager->ReloadNIFFile())
        {
            pkData->SetScene(MSharedData::CHARACTER_INDEX,
                pkActorManager->GetNIFRoot());
        }
        else
        {
            SetLastErrorCode(MANIMATION_ERR_FAILED_TO_RELOAD_NIF);

            pcModelPath = MStringToCharPointer(strOldModelPath);
            pkKFMTool->SetModelPath(pcModelPath);
            MFreeCharPointer(pcModelPath);

            pcModelRoot = MStringToCharPointer(strOldModelRoot);
            pkKFMTool->SetModelRoot(pcModelRoot);
            MFreeCharPointer(pcModelRoot);

            pkData->Unlock();
            return false;
        }
    }

    m_bCanSave = true;
    m_bNeedToSave = true;
    pkData->Unlock();

    MLogger::LogGeneral(String::Concat("Model Path: ", strModelPath));
    MLogger::LogGeneral(String::Concat("Model Root: ", strModelRoot));

    OnModelPathAndRootChanged(strModelPath, strModelRoot);
    return true;
}

bool MAnimation::AddTextKeyEvent(String *pkText, float fTime)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	// 현재 Playback모드로 진행중인 Sequence가 있는지 확인한다.
	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetText(pcText);
	pkTextKey->SetTime(fTime);

	if(pkKFMTool->AddAnimationEvent(NiNew NiAnimationEventTextKey(pkTextKey->Copy()),pkSeq->get_SequenceID()) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		pkData->Unlock();
		return false;
	}

	pkSeq->GetControllerSequence()->AddTextKeys(pkTextKey, 1);
	pkSeq->RefreshTextKeys();
	pkData->GetActorManager()->RegisterCallback(NiActorManager::TEXT_KEY_EVENT, pkSeq->get_SequenceID(), pcText);

	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	pkData->Unlock();

	return true;
}

bool MAnimation::ModifyEventTime(String *pkText, float fTime, float fNewTime, EventType eEvent)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetTime(fTime);
	pkTextKey->SetText(pcText);

	NiActorManager::EventType kEventType;
	switch(eEvent)
	{
	case	TextKeyEvent:
		{
			kEventType = NiActorManager::TEXT_KEY_EVENT;
		}
		break;
	case	EffectEvent:
		{
			kEventType = NiActorManager::EFFECT_EVENT;
		}
		break;
	case	SoundEvent:
		{
			kEventType = NiActorManager::AUDIO_EVENT;
		}
		break;
	case	FadeOutEffectEvent:
		{
			kEventType = NiActorManager::FADEOUTEFFECT_EVENT;
		}
		break;
	}

	if(pkKFMTool->ModifyAnimationEvent(kEventType,pkTextKey,pkSeq->SequenceID,fNewTime) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		pkData->Unlock();
		return false;
	}

	pkSeq->GetControllerSequence()->ModifyTextKey(pkTextKey, fNewTime);
	pkSeq->RefreshTextKeys();
	
	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	pkData->Unlock();

	return true;
}

bool MAnimation::AddEffectEvent(String *pkText, float fTime, 
			String *pkEffectName, String *pkAttachPoint,
			float fScale,
			float fFadeInTime, bool bUseFadeIn,
			float fFadeOutTimeWhenSequenceChanged, 
			bool bUseFadeOutWhenSequenceChanged)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	// 현재 Playback모드로 진행중인 Sequence가 있는지 확인한다.
	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	const char *pcEffectName = MStringToCharPointer(pkEffectName);
	const char *pcAttachPoint = MStringToCharPointer(pkAttachPoint);

	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetTime(fTime);
	pkTextKey->SetText(pcText);

	NiAnimationEventEffect *pkEffect = NiNew NiAnimationEventEffect(pkTextKey->Copy(), 
		pcEffectName, pcAttachPoint, fScale,fFadeInTime,bUseFadeIn,
		fFadeOutTimeWhenSequenceChanged,bUseFadeOutWhenSequenceChanged
		);
	
	if(pkKFMTool->AddAnimationEvent(pkEffect,pkSeq->get_SequenceID()) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		MFreeCharPointer(pcEffectName);
		MFreeCharPointer(pcAttachPoint);

		pkData->Unlock();

		return false;
	}

	// KFM에 있는 Textkey를 NiSequenceController에 추가한다.
	NiControllerSequence *pkSequence = pkSeq->GetControllerSequence();
	pkSequence->AddTextKeys(pkTextKey, 1);

	// Callback Event가 일어나도록 Event를 등록한다.
	pkData->GetActorManager()->AddAdditionalEvent(NiActorManager::EFFECT_EVENT, pcText, pkSeq->get_SequenceID(), pkSequence, pkEffect);

	// MSequence가 가지고 있는 TextKey를 갱신한다.
	pkSeq->RefreshTextKeys();

	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	MFreeCharPointer(pcEffectName);
	MFreeCharPointer(pcAttachPoint);

	pkData->Unlock();

	return true;
}

bool MAnimation::AddFadeOutEffectEvent(String *pkText, float fTime, 
			String *pkEffectName, float fFadeOutTime)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	// 현재 Playback모드로 진행중인 Sequence가 있는지 확인한다.
	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	const char *pcEffectName = MStringToCharPointer(pkEffectName);

	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetTime(fTime);
	pkTextKey->SetText(pcText);

	NiAnimationEventFadeOutEffect *pkFadeOutEffect = NiNew NiAnimationEventFadeOutEffect(pkTextKey->Copy(), 
		pcEffectName, fFadeOutTime);
	
	if(pkKFMTool->AddAnimationEvent(pkFadeOutEffect,pkSeq->get_SequenceID()) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		MFreeCharPointer(pcEffectName);

		pkData->Unlock();

		return false;
	}

	// KFM에 있는 Textkey를 NiSequenceController에 추가한다.
	NiControllerSequence *pkSequence = pkSeq->GetControllerSequence();
	pkSequence->AddTextKeys(pkTextKey, 1);

	// Callback Event가 일어나도록 Event를 등록한다.
	pkData->GetActorManager()->AddAdditionalEvent(NiActorManager::FADEOUTEFFECT_EVENT, pcText, pkSeq->get_SequenceID(), pkSequence, pkFadeOutEffect);

	// MSequence가 가지고 있는 TextKey를 갱신한다.
	pkSeq->RefreshTextKeys();

	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	MFreeCharPointer(pcEffectName);

	pkData->Unlock();

	return true;
}
bool MAnimation::AddSoundEvent(String *pkText, float fTime,
			String *pkSoundName, float fVolume, 
			float fMinDist, float fMaxDist)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	// 현재 Playback모드로 진행중인 Sequence가 있는지 확인한다.
	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	const char *pcSoundName = MStringToCharPointer(pkSoundName);

	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetTime(fTime);
	pkTextKey->SetText(pcText);

	NiAnimationEventSound	*pkSound = NiNew NiAnimationEventSound(pkTextKey->Copy(), 
		pcSoundName, fVolume, fMinDist, fMaxDist);
	
	if(pkKFMTool->AddAnimationEvent(pkSound,pkSeq->get_SequenceID()) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		MFreeCharPointer(pcSoundName);

		pkData->Unlock();
		return false;
	}

	// KFM에 있는 Textkey를 NiSequenceController에 추가한다.
	NiControllerSequence *pkSequence = pkSeq->GetControllerSequence();
	pkSequence->AddTextKeys(pkTextKey, 1);

	// Callback Event가 일어나도록 Event를 등록한다.
	pkData->GetActorManager()->AddAdditionalEvent(NiActorManager::AUDIO_EVENT, pcText, pkSeq->get_SequenceID(), pkSequence, pkSound);

	// MSequence가 가지고 있는 TextKey를 갱신한다.
	pkSeq->RefreshTextKeys();

	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	MFreeCharPointer(pcSoundName);
	pkData->Unlock();

	return true;
}

bool MAnimation::RemoveEvent(String *pkText, float fTime, EventType eEvent)
{
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool* pkKFMTool = pkData->GetKFMTool();
	if (!pkKFMTool)
	{
		SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
		pkData->Unlock();
		return false;
	}

	MSequence *pkSeq = get_ActiveSequence();
	if(!pkSeq)
	{
		pkData->Unlock();
		return false;
	}

	const char *pcText = MStringToCharPointer(pkText);
	NiTextKey *pkTextKey = NiNew NiTextKey;
	pkTextKey->SetTime(fTime);
	pkTextKey->SetText(pcText);

	NiActorManager::EventType kEventType;
	switch(eEvent)
	{
	case	TextKeyEvent:
		{
			kEventType = NiActorManager::TEXT_KEY_EVENT;
		}
		break;
	case	EffectEvent:
		{
			kEventType = NiActorManager::EFFECT_EVENT;
		}
		break;
	case	SoundEvent:
		{
			kEventType = NiActorManager::AUDIO_EVENT;
		}
		break;
	case	FadeOutEffectEvent:
		{
			kEventType = NiActorManager::FADEOUTEFFECT_EVENT;
		}
		break;
	}

	if(pkKFMTool->RemoveAnimationEvent(kEventType,pkTextKey,pkSeq->SequenceID) != NiKFMTool::KFM_SUCCESS)
	{
		MFreeCharPointer(pcText);
		pkData->Unlock();
		return false;
	}

	NiControllerSequence *pkControllerSeq = pkSeq->GetControllerSequence();
	pkControllerSeq->RemoveTextKey(pkTextKey);
	pkSeq->RefreshTextKeys();

	NiDelete pkTextKey;
	MFreeCharPointer(pcText);
	pkData->Unlock();

	return true;
}
////////////////////////////////////////////////////////////////////////////////////
// AddToItemList 
// Desc;
// 같은 아이템(똑같은 Xml을 쓰는 아이템)을 같은 폴더에 넣지만 않으면 상관없다.
// (다른 폴더에는 같은 아이템을 넣어도 둘 다 존재한다)
// /////////////////////////////////////////////////////////////////////////////////
bool MAnimation::AddToItemList(String *strPartPath, String *strItemFolder)
{
	const char* pcNIFPath = MStringToCharPointer(strPartPath);

	// 인자로 넘어오는 것은 Xml Path(절대 경로)이다.
	// Item List에 Item을 추가한다.

	MItem *pkMItem = NiExternalNew MItem(strPartPath, m_pkItemMan);

	// 폴더로 ArrayList를 찾는다.
	ArrayList* pkItems = GetItemsByFolder(strItemFolder);
	if(!pkItems)
	{
		// 없으면 새로 등록
		pkItems = NiExternalNew ArrayList();
		m_mapFolderToItems->Add(strItemFolder, pkItems);

		// 폴더 리스트에 등록한다.
		m_aFolders->Add(strItemFolder);
	}

	// 폴더 리스트에 아이템을 추가한다.
	if(!GetItemByPath(pkItems, strPartPath))
	{
		pkItems->Add(pkMItem);
		pkItems->TrimToSize();

		m_aAllItems->Add(pkMItem);
		m_aAllItems->TrimToSize();
	}
	else
	{
		NiExternalDelete pkMItem;
		return false;
	}
	return true;
}

bool MAnimation::RemoveFromItemList(String *strPartPath, String *strItemFolder)
{
	// 폴더에 들어있는 아이템들을 가져온다.
	ArrayList* pkItems = GetItemsByFolder(strItemFolder);
	if(!pkItems)
	{
		return false;
	}

	// 위에서 가져온 아이템들의 목록에서, Path에 해당하는 아이템을 가져온다.
	MItem *pkMItem = GetItemByPath(pkItems, strPartPath);
	if(!pkMItem)
	{
		return false;
	}

	pkItems->Remove(pkMItem);
	pkItems->TrimToSize();
	m_aAllItems->Remove(pkMItem);
	m_aAllItems->TrimToSize();

	const char* pcNIFPath = MStringToCharPointer(strPartPath);
	m_pkItemMan->RemoveItem(pcNIFPath);
	MFreeCharPointer(pcNIFPath);
	
	return true;
}

bool MAnimation::AttachParts(MItem *pkParts, int iGender, int iClass)
{
	if(!pkParts || pkParts->get_Attached())
	{
		return false;
	}

	// Xml을 아직 파싱하지 않았다면 
	PgItem *pkItem = pkParts->GetItem(iGender, iClass);
	if(!pkItem)
	{
		// Xml을 파싱한다.
		if(!pkParts->ParseItemXml(iGender, iClass, false))
		{
			return false;			
		}
		pkItem = pkParts->GetItem(iGender, iClass);
	}

	if(!AttachParts(pkItem))
	{
		return false;
	}

	pkParts->SetItem(pkItem);
	return true;
}

bool MAnimation::AttachParts(PgItem *pkItem)
{
	if(!pkItem)
	{
		return false;
	}

	// 모델의 루트를 가져온다.
	MSharedData* pkData = MSharedData::Instance;
	if(!pkData)
	{
		return false;
	}

	pkData->Lock();
	NiActorManager *pkAM = pkData->GetActorManager();
	if(!pkAM)
	{
		NiMessageBox("모델이 존재하지 않으므로, 파츠를 붙일 수 없습니다(An actor manager doesn't exist)", "Character Tool : Error Message");
		SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
		return false;
    }
	NiNode *pkModelRoot = (NiNode *)pkAM->GetNIFRoot();

	int iItemPos = pkItem->ItemPos();

	// 이미 같은 종류의 아이템이 붙어 있다면 Detach시킨다.
	PgItem *pkAlreadyAttachedItem = m_pkItemMan->GetAttachedItem(iItemPos);
	if(pkAlreadyAttachedItem)
	{
		DetachParts(pkAlreadyAttachedItem);
	}

	pkData->Unlock();

	bool bRet;

	if(iItemPos < NiPow(2,10))
	{
		// 아이템 포지션이 10미만이면, 그냥 Attach하는 아이템이다.
		bRet = AttachNoSkinningParts(pkItem, pkModelRoot, 0);
	}
	else
	{
		// 아이템 포지션이 2^10이상이면, Skinning하여 Attach하는 아이템이다.
		bRet = (AttachSkinningParts(pkItem, pkModelRoot) > 0 ? true : false);
	}

	if(bRet)
	{
		// Attach한 아이템은 ItemMan에 등록한다.
		m_pkItemMan->RegisterAttachedItem(pkItem);
		if(pkModelRoot && pkModelRoot->GetObjectByName("Base_Shape"))
		{
			pkModelRoot->GetObjectByName("Base_Shape")->SetAppCulled(true);
		}
	}

	return bRet;
}

int MAnimation::AttachSkinningParts(PgItem *pkItem, NiNode *pkModelRoot)
{
	if(!pkItem)
	{
		return false;
	}

	NiNodePtr pkAttachSrcRoot = pkItem->GetMeshRoot();

	// Attach된 Node가 1개 이상이면 그 아이템은 잘 Attach된 것이다.
	return SkinningParts(pkItem, pkAttachSrcRoot, pkModelRoot, true);
}

int MAnimation::SkinningParts(PgItem *pkItem, NiNode *pkItemRoot, NiNode *pkModelRoot, bool bAttach)
{
	if(!pkItemRoot)
	{
		return 0;
	}

	int iRetAttachedNode = 0;
	int iAttachedThisTime = 0;
	int iChildCount = pkItemRoot->GetArrayCount();
	NiAVObject *pkAttachingNode = 0;

	for(int iChildIndex = 0; iChildIndex<iChildCount; ++iChildIndex)
	{
		NiNode *pkChild = (NiNode *)pkItemRoot->GetAt(iChildIndex);
		if(!pkChild)
		{
			continue;
		}

		// Geometry정보이고, Hide되어 있지 않으면
		if(NiIsKindOf(NiGeometry, pkChild) && !pkChild->GetAppCulled())
		{
			NiGeometry *pkGeometry = (NiGeometry *)pkChild;
			
			NiSkinInstance* pkSkin = pkGeometry->GetSkinInstance();
			if(pkSkin)
			{
				NiSkinData* pkSkinData = pkSkin->GetSkinData();
				unsigned int uiBoneCount = pkSkinData->GetBoneCount();
				NiAVObject* pkRootParent = pkSkin->GetRootParent();
				NiAVObject*const* pkBones = pkSkin->GetBones();

				NiAVObject* pkDup = NULL;

				for(unsigned int ui = 0; ui < uiBoneCount; ui++)
				{
					NiAVObject* pkCurBone = pkBones[ui];
					pkDup = pkModelRoot->GetObjectByName(pkCurBone->GetName());
					if(pkDup)
					{
						pkSkin->SetBone(ui, pkDup);
					}
				}

				pkDup = pkModelRoot->GetObjectByName(pkRootParent->GetName());
				if(!pkDup)
				{
					continue;
				}
				pkSkin->SetRootParent(pkDup);
			}
			
			// 붙이려는 Geometry의 가장 가까운 부모의 이름과 같은 노드를
			// 모델의 Tree에서 찾아서, 그 노드에 붙인다.
			bool bFindNewParent = false;
			NiAVObject *pkCandidateNode = pkItemRoot;			
			while(!bFindNewParent && pkCandidateNode && pkCandidateNode->GetParent())
			{
				if(pkModelRoot->GetObjectByName(pkCandidateNode->GetParent()->GetName()))
				{
					bFindNewParent = true;
					break;
				}
				pkCandidateNode = pkCandidateNode->GetParent();
			}

			// 붙이려는 노드의 부모를 원래의 모델에서 찾았으면, 노드를 붙일 준비를 한다.
			if(bFindNewParent)
			{
				++iAttachedThisTime;
				pkItem->AddAttachedNode(pkCandidateNode);
				pkAttachingNode = pkCandidateNode;
			}
		}
		else if(NiIsKindOf(NiNode, pkChild)/* && pkChild->GetName() != "char_root"*/)
		{
			bool bChildAttach = true;
			// 노드일 경우 재귀적으로 검색(DFS)
			if(bAttach && iAttachedThisTime)
			{
				// 현재 노드의 레벨에서, Parts에 추가 되었다면 그 자식은 Attach하지 않는다.
				// (Bone만 Set하면 된다)
				bChildAttach = false;
			}
			iRetAttachedNode += SkinningParts(pkItem, pkChild, pkModelRoot, bChildAttach);
		}
	}

	if(bAttach && iAttachedThisTime > 0 && pkAttachingNode)
	{	
		NiNode *pkDup = (NiNode *)pkModelRoot->GetObjectByName(pkAttachingNode->GetParent()->GetName());
		if(pkDup)
		{
			NiNode *pkParent = pkItemRoot->GetParent();
			unsigned int uiArrayCount = pkParent->GetArrayCount();
			for(unsigned int index = 0; index<uiArrayCount; ++index)
			{
				NiAVObject *pkChild = pkParent->GetAt(index);
				if(NiIsKindOf(NiTextureEffect, pkChild))
				{
					// TextureEffect도 같이 붙여준다.
					pkModelRoot->AttachChild(pkChild);
					
					// TextureEffect도 AttachedNode에 추가한다.
					pkItem->AddAttachedNode(pkChild);
				}
			}

			pkDup->AttachChild(pkAttachingNode);
			pkDup->UpdateEffects();
			pkDup->UpdateProperties();
			pkDup->Update(0.0f);
			
			iRetAttachedNode += iAttachedThisTime;
		}
	}

	return iRetAttachedNode;
}

//! 파츠를 액터의 특정 더미에 붙인다.(이때의 파츠는 스키닝을 하지 않는 파츠다)
bool MAnimation::AttachNoSkinningParts(PgItem *pkItem, NiNode *pkModelRoot, const char *pcTargetDummy)
{
	int iItemPos = 0;
	if(pcTargetDummy == 0)
	{
		pcTargetDummy = pkItem->GetTargetPoint();
		if(!pcTargetDummy)
		{
			return false;
		}
	}

	//	pcPartsNifPath 의 Scene Root 를 pcTargetDummy 에 Attach 시킨다.
	NiNode *pkItemRoot = pkItem->GetMeshRoot();

	NiNode *pkDestNode = NiDynamicCast(NiNode, pkModelRoot->GetObjectByName(pcTargetDummy));
	if(!pkDestNode)
	{
		assert(!"pcTargetDummy dummy not found from dest");
		return false;
	}

	NiNode *pkSrcNode = NiDynamicCast(NiNode, pkItemRoot->GetObjectByName(pcTargetDummy));
	if(!pkSrcNode)
	{
		assert(!"pcTargetDummy dummy not found from src");
		return false;
	}

	pkSrcNode->SetRotate(NiQuaternion::IDENTITY);
	pkSrcNode->SetTranslate(NiPoint3::ZERO);
	if(iItemPos == 4)
	{
		//NiNode *pkFace = (NiNode *)pkSrcNode->GetAt(0);
		//pkFace->GetAt(0)->SetName("PgMorphTarget");
	}

	NiNode *pkAttachingNode = pkSrcNode->GetParent();
	pkAttachingNode->SetName("AttachedParts");
	
	
	pkDestNode->AttachChild(pkAttachingNode);
	pkDestNode->UpdateProperties();
	pkDestNode->UpdateEffects();
	pkDestNode->Update(0.0f, true);

	if(iItemPos == 4)
	{
		MSharedData *pkData = MSharedData::Instance;
		pkData->Lock();
		pkData->GetActorManager()->ChangeNIFRoot(pkModelRoot);
		pkData->Unlock();
	}

	// Attached Node에 추가한다.
	pkItem->AddAttachedNode(pkAttachingNode);

	PgItem *pkAdditionalItem = pkItem->GetAdditionalItem();
	if(pkAdditionalItem)
	{
		AttachNoSkinningParts(pkAdditionalItem, pkModelRoot, 0);
	}

	if(pkItem->IsAvailableAnimation())
	{
		MSharedData *pkData = MSharedData::Instance;
		pkData->Lock();
		
		NiActorManagerPtr spNewAM = pkItem->GetActorManager();
		NiAVObject *pkNewRoot = spNewAM ->GetNIFRoot();
		pkData->AddActorManager(pkItem->ItemPos(), spNewAM);
		pkData->Unlock();
	}
	
	return true;
}

//! 파츠를 액터에서 떼어낸다.
bool MAnimation::DetachParts(MItem *pkParts, int iGender, int iClass)
{
	if(!pkParts || pkParts->get_Attached() == false)
	{
		return false;
	}
	
	return DetachParts(pkParts->GetItem(iGender, iClass));
}

bool MAnimation::DetachParts(PgItem *pkItem)
{
	if(!pkItem)
	{
		return false;
	}
	
	PgItem *pkAdditionalItem = pkItem->GetAdditionalItem();
	if(pkAdditionalItem)
	{
		DetachParts(pkAdditionalItem);
	}

	NiNode *pkItemRoot = pkItem->GetMeshRoot();

	// 붙어있던 모든 노드들을 Detach시킨다.
	NiAVObject *pkAttachedNode = pkItem->GetAttachedNode();
	while(pkAttachedNode)
	{
		NiNode *pkParent = pkAttachedNode->GetParent();
		if(pkParent)
		{
			pkParent->DetachChild(pkAttachedNode);
			pkParent->UpdateProperties();
			pkParent->UpdateEffects();
			pkParent->Update(0.0f);

			// Skinning한 아이템이면, 아이템을 로딩한 원래대로 돌려준다.
			if(pkItemRoot && pkItem->ItemPos() > 1024 )
			{
				UnskinningParts(pkItemRoot, pkAttachedNode);
				// 원래 아이템 Root에 각 파츠를 붙인다.
				NiNode *pkOriginalParentNode = (NiNode *)pkItemRoot->GetObjectByName(pkParent->GetName());
				pkOriginalParentNode->AttachChild(pkAttachedNode, true);
				pkOriginalParentNode->UpdateProperties();
				pkOriginalParentNode->UpdateEffects();
				pkOriginalParentNode->Update(0.0f);
			}
			
		}
		pkItem->RemoveAttachedNode(pkAttachedNode);
		pkAttachedNode = pkItem->GetAttachedNode();
	}

	// Attach됐던 아이템을 등록해제한다.
	m_pkItemMan->UnregisterAttachedItem(pkItem);

	// AM이 붙어 있는 아이템이었다면 같이 뗴어준다.
	if(pkItem->IsAvailableAnimation())
	{
		MSharedData* pkData = MSharedData::Instance;
		pkData->Lock();
		pkData->RemoveActorManager(pkItem->ItemPos());
		pkData->Unlock();
	}
	return true;
}

void MAnimation::UnskinningParts(NiNode *pkItemRoot, NiAVObject *pkAttachedNode)
{
	// Unskinning한다.
	NiNode *pkAttachedRoot = (NiNode *)pkAttachedNode;
	unsigned int uiArrayCnt = pkAttachedRoot->GetArrayCount();
	for(unsigned int uiIndex = 0; uiArrayCnt > uiIndex; ++uiIndex)
	{
		NiAVObject *pkChild = pkAttachedRoot->GetAt(uiIndex);
		if(NiIsKindOf(NiGeometry, pkChild) && !pkChild->GetAppCulled())
		{
			NiGeometry *pkGeometry = (NiGeometry *)pkChild;
			NiSkinInstance *pkSkin = pkGeometry->GetSkinInstance();
			if(!pkSkin)
			{
				continue;
			}

			NiSkinData *pkSkinData = pkSkin->GetSkinData();
			unsigned int uiBoneCount = pkSkinData->GetBoneCount();
			NiAVObject *pkRootParent = pkSkin->GetRootParent();
			NiAVObject *const *pkBones = pkSkin->GetBones();

			NiAVObject* pkDup = NULL;
			for(unsigned int ui = 0; ui < uiBoneCount; ui++)
			{
				NiAVObject* pkCurBone = pkBones[ui];
				pkDup = pkItemRoot->GetObjectByName(pkCurBone->GetName());
				if(pkDup)
				{
					pkSkin->SetBone(ui, pkDup);
				}
			}

			pkDup = pkItemRoot->GetObjectByName(pkRootParent->GetName());
			if(!pkDup)
			{
				continue;
			}

			pkSkin->SetRootParent(pkDup);
		}
		else if(NiIsKindOf(NiNode, pkChild))
		{
			UnskinningParts(pkItemRoot, pkChild);
		}
	}
}

bool MAnimation::DetachAllPartsFromModel()
{
	const int iMaxItemPos = 32;
	for(int iItemPos = 0; iItemPos<iMaxItemPos; ++iItemPos)
	{
		PgItem *pkItem = m_pkItemMan->GetAttachedItem((int)NiPow(2.0f, (float)iItemPos));
		if(pkItem)
		{
			DetachParts(pkItem);
		}
	}

	return true;
}

bool MAnimation::SaveItemList()
{
	//TiXmlDocument kDoc;
	//TiXmlDeclaration *pkDecl = NiExternalNew TiXmlDeclaration("1.0", "euc-kr", "");
	//kDoc.LinkEndChild(pkDecl);

	//TiXmlElement *pkPartsElement = NiExternalNew TiXmlElement("ITEMS");
	//kDoc.LinkEndChild(pkPartsElement);

	//int iFolderCount = m_aFolders->Count;
	//for(int iFolderIdx = 0; iFolderIdx<iFolderCount; ++iFolderIdx)
	//{
	//	ArrayList *pkFolder = dynamic_cast<ArrayList *>(m_aFolders->get_Item(iFolderIdx));
	//	if(pkFolder)
	//	{
	//		int iItemCount = pkFolder->Count;
	//		for(int iItemIdx = 0; iItemIdx<iItemCount; ++iItemIdx)
	//		{
	//			MItem *pkItem = dynamic_cast<MItem *>(pkFolder->get_Item(iItemIdx));
	//			const char* pcItemName = MStringToCharPointer(pkItem->get_Name());
	//			const char* pcItemPath = MStringToCharPointer(pkItem->get_Path());
	//			
	//			TiXmlElement *pkItem = NiExternalNew TiXmlElement("ITEM");
	//			pkItem->SetAttribute("NAME", pcPartName);
	//			TiXmlText *pkText = NiExternalNew TiXmlText(pcPartPath);
	//			pkItem->LinkEndChild(pkText);
	//			pkPartsElement->LinkEndChild(pkItem);

	//			MFreeCharPointer(pcItemName);
	//			MFreeCharPointer(pcItemPath);
	//		}
	//	}
	//}

	//kDoc.SaveFile("c:\\PartsList.xml");
	return true;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::GetModelRootConflicts()
{
    ArrayList* pkConflictingNodeNames = NiExternalNew ArrayList(0);

    String* strModelPath = this->ModelPath;
    String* strModelRoot = this->ModelRoot;
    if (strModelPath == String::Empty || strModelPath == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return pkConflictingNodeNames;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    NiAVObject* pkRoot = pkActorManager->GetNIFRoot();
    NiAVObject* pkNewRoot = pkRoot->GetObjectByName(pcModelRoot);

    if (pkNewRoot == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH);
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    if (Sequences == NULL || Sequences->Count == 0)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    for (int i = 0; i < Sequences->Count; i++)
    {
        NiControllerSequence* pkSequence = (static_cast<MSequence*>(
            Sequences->Item[i]))->GetControllerSequence();
        
        for (unsigned int j = 0; j < pkSequence->GetArraySize(); j++)
        {
            NiInterpolator* pkInterp = pkSequence->GetInterpolatorAt(j);
            if (!pkInterp)
                continue;
            NiControllerSequence::IDTag* pkTag = pkSequence->GetIDTag(j);
            if (pkTag == NULL || !pkTag->GetAVObjectName().Exists())
                continue;

            NiAVObject* pkObj = pkNewRoot->GetObjectByName(
                pkTag->GetAVObjectName());
            if (pkObj == NULL)
            {
                String* strName = NiExternalNew String(
                    pkTag->GetAVObjectName());
                if (!pkConflictingNodeNames->Contains(strName))
                    pkConflictingNodeNames->Add(strName);  
            }
        }
    }

    MFreeCharPointer(pcModelRoot);
    pkData->Unlock();

    return pkConflictingNodeNames;
} 
//---------------------------------------------------------------------------
ArrayList* MAnimation::GetModelRootConflictsForSequence(
    MSequence* pkMSequence)
{
    ArrayList* pkConflictingNodeNames = NiExternalNew ArrayList(0);

    String* strModelPath = this->ModelPath;
    String* strModelRoot = this->ModelRoot;
    if (strModelPath == String::Empty || strModelPath == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_PATH_EMPTY);
        return pkConflictingNodeNames;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    const char* pcModelRoot = MStringToCharPointer(strModelRoot);
    NiAVObject* pkRoot = pkActorManager->GetNIFRoot();
    NiAVObject* pkNewRoot = pkRoot->GetObjectByName(pcModelRoot);

    if (pkNewRoot == NULL)
    {
        SetLastErrorCode(MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH);
        pkData->Unlock();
        return pkConflictingNodeNames;
    }

    NiControllerSequence* pkSequence = pkMSequence->GetControllerSequence();
    
    for (unsigned int j = 0; j < pkSequence->GetArraySize(); j++)
    {
        NiInterpolator* pkInterp = pkSequence->GetInterpolatorAt(j);
        if (!pkInterp)
            continue;
        NiControllerSequence::IDTag* pkTag = pkSequence->GetIDTag(j);
        if (pkTag == NULL || !pkTag->GetAVObjectName().Exists())
            continue;

        NiAVObject* pkObj = pkNewRoot->GetObjectByName(
            pkTag->GetAVObjectName());
        if (pkObj == NULL)
        {
            String* strName = NiExternalNew String(pkTag->GetAVObjectName());
            if (!pkConflictingNodeNames->Contains(strName))
                pkConflictingNodeNames->Add(strName);  
        }
    }

    MFreeCharPointer(pcModelRoot);
    pkData->Unlock();

    return pkConflictingNodeNames;
}
//---------------------------------------------------------------------------
MSequence* MAnimation::AddSequence(unsigned int uiSequenceID,
    String* strFilename, int iAnimIndex)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    const char* pcFilename = MStringToCharPointer(strFilename);
    NiKFMTool::KFM_RC eRC = pkKFMTool->AddSequence(uiSequenceID,
        pcFilename, iAnimIndex);
    MFreeCharPointer(pcFilename);
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager && !pkActorManager->LoadSequence(uiSequenceID))
    {
        eRC = pkKFMTool->RemoveSequence(uiSequenceID);
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        assert(eRC == NiKFMTool::KFM_SUCCESS);
        pkData->Unlock();
        return NULL;
    }
    else if (pkActorManager == NULL)
    {
        pkActorManager = CreateActorManager();
        if (!pkActorManager)
        {
            SetLastErrorCode(MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED);
            pkData->Unlock();
            return NULL;
        }
    }
    
    m_bNeedToSave = true;
    pkData->Unlock();

    AddToSequenceArray(NiExternalNew MSequence(this, 
        pkKFMTool->GetSequence(uiSequenceID),
        pkActorManager->GetSequence(uiSequenceID)), true);

    MSequence* pkSequence = GetSequence(uiSequenceID);
    assert(pkSequence);

    MLogger::LogGeneral(String::Concat("Adding Sequence: ",
        pkSequence->Name));

    OnSequenceAdded(pkSequence);
    //AddDefaultTransitionsForSequence(pkSequence);

    return pkSequence;
}
//---------------------------------------------------------------------------
void MAnimation::AddDefaultTransitionsForSequence(MSequence* pkSequence)
{
    unsigned int uiSrcID = pkSequence->SequenceID;
    for (int i = 0; i < this->Sequences->Count; i++)
    {
        MSequence* pkDesSequence = static_cast<MSequence*>(
            this->Sequences->Item[i]);
        unsigned int uiDesID = pkDesSequence->SequenceID;
        if (uiDesID != uiSrcID)
        {
            if (pkSequence->CanSyncTo(pkDesSequence))
            {
                if (pkSequence->GetTransition(uiDesID) == NULL)
                {
                    AddTransition(uiSrcID, uiDesID,
                        MTransition::TransitionType::Trans_DefaultSync,
                        this->DefaultSyncTransDuration);
                }
                if (pkDesSequence->GetTransition(uiSrcID) == NULL)
                {
                    AddTransition(uiDesID, uiSrcID,
                        MTransition::TransitionType::Trans_DefaultSync,
                        this->DefaultSyncTransDuration);
                }
            }
            else
            {
                if (pkSequence->GetTransition(uiDesID) == NULL)
                {
                    AddTransition(uiSrcID, uiDesID,
                        MTransition::TransitionType::Trans_DefaultNonSync,
                        this->DefaultNonSyncTransDuration);
                }
                if (pkDesSequence->GetTransition(uiSrcID) == NULL)
                {
                    AddTransition(uiDesID, uiSrcID,
                        MTransition::TransitionType::Trans_DefaultNonSync,
                        this->DefaultNonSyncTransDuration);
                }
            }

            //SetInitialTransitionTimeRowCol(pkSequence);
        }
    }

    MLogger::LogGeneral(String::Concat("Adding default transitions"
        " for sequence ", pkSequence->Name));

}
//---------------------------------------------------------------------------
void MAnimation::RemoveSequence(unsigned int uiSequenceID)
{
    MSequence* pkSequence = GetSequence(uiSequenceID);
    if (!pkSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SEQUENCE_DOES_NOT_EXIST);
        return;
    }

    RemoveTransitionsContainingSequence(uiSequenceID);

    for (int i = 0; i < m_aSequenceGroups->Count; i++)
    {
        MSequenceGroup* pkTemp = dynamic_cast<MSequenceGroup*>(
            m_aSequenceGroups->get_Item(i));
        pkTemp->RemoveSequence(uiSequenceID);
    }

    SetPlaybackMode(PlaybackMode::None, NULL);

    RemoveFromSequenceArray(pkSequence);
    pkSequence->DeleteContents();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    assert(pkKFMTool);
    NiKFMTool::KFM_RC eRC = pkKFMTool->RemoveSequence(uiSequenceID);
    assert(eRC == NiKFMTool::KFM_SUCCESS);

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        pkActorManager->UnloadSequence(uiSequenceID);
    }

    m_bNeedToSave = true;
    pkData->Unlock();

    OnSequenceRemoved(uiSequenceID);

    if (Sequences == NULL || Sequences->Count == 0)
        ReloadNIF();
}
//---------------------------------------------------------------------------
void MAnimation::RemoveTransitionsContainingSequence(
    unsigned int uiSequenceID)
{
    if (m_aSequences == NULL)
    {
        return;
    }

    ArrayList* aToRemove = NiExternalNew ArrayList();
    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);
        if (pkSequence->Transitions == NULL)
        {
            continue;
        }
        for (int j = 0; j < pkSequence->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkSequence->Transitions[j];
            assert(pkTransition != NULL);
            if (pkTransition->SrcID == uiSequenceID ||
                pkTransition->DesID == uiSequenceID)
            {
                aToRemove->Add(pkTransition);
            }
            else if (pkTransition->Type ==
                MTransition::TransitionType::Trans_Chain)
            {
                if (pkTransition->ChainInfo == NULL)
                {
                    continue;
                }
                for (int k = 0; k < pkTransition->ChainInfo->Count; k++)
                {
                    MTransition::MChainInfo* pkChainInfo =
                        pkTransition->ChainInfo[k];
                    if (pkChainInfo->SequenceID == uiSequenceID)
                    {
                        aToRemove->Add(pkTransition);
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < aToRemove->Count; i++)
    {
        MTransition* pkTransToRemove = dynamic_cast<MTransition*>(
            aToRemove->Item[i]);

        RemoveTransition(pkTransToRemove->SrcID, pkTransToRemove->DesID);
    }
}
//---------------------------------------------------------------------------
void MAnimation::RemoveChainsContainingTransitions(
    unsigned int uiSrcID, unsigned int uiDesID)
{
    if (m_aSequences == NULL)
    {
        return;
    }

    ArrayList* aToRemove = NiExternalNew ArrayList();
    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);
        if (pkSequence->Transitions == NULL)
        {
            continue;
        }
        for (int j = 0; j < pkSequence->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkSequence->Transitions[j];
            assert(pkTransition != NULL);

            if (pkTransition->Type != 
                MTransition::TransitionType::Trans_Chain)
                continue;

            unsigned int uiChainCnt = pkTransition->ChainInfo->Count;

            if (uiChainCnt == 0)
                continue;

            // Now cycle through the chain to see if we can find 
            // the transition.

            // check first...
            if (pkTransition->SrcID == uiSrcID)
            {
                if (pkTransition->ChainInfo[0]->SequenceID == uiDesID)
                {
                    aToRemove->Add(pkTransition);
                    continue;
                }
            }

            // check last...
            if (pkTransition->ChainInfo[uiChainCnt-1]->SequenceID == uiSrcID)
            {
                if (pkTransition->DesID == uiDesID)
                {
                    aToRemove->Add(pkTransition);
                    continue;
                }
            }

            // check inbetween...
            for (unsigned int k = 1; k < uiChainCnt-1; k++)
            {
                MTransition::MChainInfo* pkChainInfo =
                    pkTransition->ChainInfo[k];

                // If we find the same pair, report this chain
                if (pkChainInfo->SequenceID == uiSrcID)
                {                        
                    if (pkTransition->ChainInfo[k+1]->SequenceID == uiDesID)
                    {
                        aToRemove->Add(pkTransition);
                        break;
                    }
                }
            }
        }
    }

    for (int l = 0; l < aToRemove->Count; l++)
    {
        MTransition* pkTransToRemove = dynamic_cast<MTransition*>(
            aToRemove->Item[l]);
        RemoveTransition(pkTransToRemove->SrcID, pkTransToRemove->DesID);
    }
}
//---------------------------------------------------------------------------
NiKFMTool::Transition* MAnimation::ChangeTransitionType(unsigned int uiSrcID,
    unsigned int uiDesID, NiKFMTool::Transition* pkTransition,
    NiKFMTool::TransitionType eNewType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    NiKFMTool::KFM_RC eRC = pkKFMTool->UpdateTransition(uiSrcID, uiDesID,
        eNewType, pkTransition->GetDuration());
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    pkTransition = pkKFMTool->GetTransition(uiSrcID, uiDesID);
    if (!pkTransition)
    {
        SetLastErrorCode(MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST);
        pkData->Unlock();
        return NULL;
    }

    pkData->Unlock();

    return pkTransition;
}
//---------------------------------------------------------------------------
MTransition* MAnimation::AddTransition(unsigned int uiSrcID,
    unsigned int uiDesID, MTransition::TransitionType eType, float fDuration)
{
    MSequence* pkSrcSequence = GetSequence(uiSrcID);
    MSequence* pkDesSequence = GetSequence(uiDesID);
    if (!pkSrcSequence || !pkDesSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SRC_OR_DEST_DOES_NOT_EXIST);
        return NULL;
    }

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    NiKFMTool::KFM_RC eRC = pkKFMTool->AddTransition(uiSrcID, uiDesID,
        MTransition::TranslateTransitionType(eType), fDuration);
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    m_bNeedToSave = true;
    pkData->Unlock();

    pkSrcSequence->RebuildTransitionsArray();
    MTransition* pkTransition = pkSrcSequence->GetTransition(uiDesID);
    assert(pkTransition != NULL);

    if (eType == MTransition::Trans_DelayedBlend)
    {
        pkTransition->StoredType = eType;
    }

    OnTransitionAdded(pkTransition);
    return pkTransition;
}
//---------------------------------------------------------------------------
void MAnimation::RemoveTransition(unsigned int uiSrcID, unsigned int uiDesID)
{
    MSequence* pkSrcSequence = GetSequence(uiSrcID);
    if (!pkSrcSequence)
    {
        SetLastErrorCode(MANIMATION_ERR_SRC_SEQUENCE_DOES_NOT_EXIST);
        return;
    }

    MTransition* pkTransition = pkSrcSequence->GetTransition(uiDesID);
    if (!pkTransition)
    {
        SetLastErrorCode(MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST);
        return;
    }
   
    if (pkTransition->Type != MTransition::TransitionType::Trans_Chain)
    {
        RemoveChainsContainingTransitions(
            pkTransition->SrcID, pkTransition->DesID);
    }

    m_bNeedToSave = true;
    OnTransitionRemoved(pkTransition);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    assert(pkKFMTool);
    NiKFMTool::KFM_RC eRC = pkKFMTool->RemoveTransition(uiSrcID, uiDesID);
    assert(eRC == NiKFMTool::KFM_SUCCESS);
    
    pkData->Unlock();

    pkSrcSequence->RebuildTransitionsArray();
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::AddSequenceGroup(unsigned int uiGroupID,
    String* strName)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }

    const char* pcName = MStringToCharPointer(strName);
    NiKFMTool::KFM_RC eRC = pkKFMTool->AddSequenceGroup(uiGroupID, pcName);
    MFreeCharPointer(pcName);
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        pkData->Unlock();
        return NULL;
    }

    m_bNeedToSave = true;
    pkData->Unlock();

    AddToSequenceGroupArray(NiExternalNew MSequenceGroup(this,
        pkKFMTool->GetSequenceGroup(uiGroupID)), true);
    MSequenceGroup* pkGroup = GetSequenceGroup(uiGroupID);
    assert(pkGroup);

    OnSequenceGroupAdded(pkGroup);
    MLogger::LogGeneral(String::Concat("Adding Sequence Group: ", 
        pkGroup->Name));
    return pkGroup;
}
//---------------------------------------------------------------------------
void MAnimation::RemoveSequenceGroup(unsigned int uiGroupID)
{
    MSequenceGroup* pkGroup = GetSequenceGroup(uiGroupID);
    if (!pkGroup)
    {
        return;
    }

    MLogger::LogGeneral(String::Concat("Removing Sequence Group: ", 
        pkGroup->Name));
    SetPlaybackMode(PlaybackMode::None, NULL);

    RemoveFromSequenceGroupArray(pkGroup);
    OnSequenceGroupRemoved(pkGroup);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    assert(pkKFMTool);
    NiKFMTool::KFM_RC eRC = pkKFMTool->RemoveSequenceGroup(uiGroupID);
    assert(eRC == NiKFMTool::KFM_SUCCESS);

    m_bNeedToSave = true;
    pkGroup->DeleteContents();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
MSequence* MAnimation::GetSequence(unsigned int uiSequenceID)
{
    if (m_mapIDToSequence == NULL)
    {
        return NULL;
    }

    return dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]);
}
//---------------------------------------------------------------------------
MSequence* MAnimation::GetSequenceByName(String* strName)
{
    if ( m_aSequences != NULL)
    {
        for (int i = 0; i < m_aSequences->Count; i++)
        {
            MSequence* pkSequence = dynamic_cast<MSequence*>(
                m_aSequences->get_Item(i));
            if (pkSequence && pkSequence->Name->CompareTo(strName) == 0)
                return pkSequence;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::GetSequenceGroup(unsigned int uiGroupID)
{
    if (m_mapIDToSequenceGroup == NULL)
    {
        return NULL;
    }

    return dynamic_cast<MSequenceGroup*>(m_mapIDToSequenceGroup->Item
        [__box(uiGroupID)]);
}
//---------------------------------------------------------------------------
bool MAnimation::SetPlaybackMode(PlaybackMode eMode, Object* pkModeData)
{
    float fMinTime = 0;

    PlaybackMode eOldMode = m_eMode;
    Object* pkOldModeData = m_pkModeData;

    switch (eMode)
    {
        case PlaybackMode::None:
            if (pkModeData != NULL)
            {
                return false;
            }
            ResetAnimations();
            break;
        case PlaybackMode::Sequence:
        {
            MSequence* pkSequence = dynamic_cast<MSequence*>(pkModeData);
            if (pkSequence == NULL)
            {
                return false;
            }
            ResetAnimations();
			MSharedData *pkData = MSharedData::Instance;
			pkData->Lock();
			pkData->GetEffectMan()->DetachAllEffects();
			pkData->GetActorManager()->RefreshAdditionalEvent(
				pkSequence->SequenceID, pkSequence->GetControllerSequence());
			pkData->Unlock();
            bool bSuccess = ActivateSequence(pkSequence->SequenceID);
            if (!bSuccess)
            {
                return false;
            }
            fMinTime = pkSequence->BeginKeyTime;
            break;
        }
        case PlaybackMode::SequenceGroup:
            if (dynamic_cast<MSequenceGroup*>(pkModeData) == NULL)
            {
                return false;
            }
            ResetAnimations();
            break;
        case PlaybackMode::Transition:
        {
            MTransition* pkTransition = dynamic_cast<MTransition*>(
                pkModeData);
            if (pkTransition == NULL)
            {
                return false;
            }
            ResetAnimations(true);
            bool bSuccess = SetTargetAnimation(pkTransition->SrcID);
            if (!bSuccess)
            {
                return false;
            }
            MSequence* pkSrcSequence = GetSequence(pkTransition->SrcID);
            MSequence* pkDesSequence = GetSequence(pkTransition->DesID);
            MAssert(pkSrcSequence != NULL && pkDesSequence != NULL,
                "Transition sequence not found.");

            if (eMode != eOldMode || pkModeData != pkOldModeData)
            {
                // We assume that the worst case for a frame is
                // at IncrementTime - we subtract off this amount
                // to ensure that the transition will get triggered.
                //m_fTransitionStartTime =
                //    (pkSrcSequence->EndKeyTime / pkSrcSequence->Frequency) -
                //    IncrementTime;
            }

            fMinTime = pkSrcSequence->BeginKeyTime;
            break;
        }
        case PlaybackMode::Interactive:
            if (pkModeData != NULL)
            {
                return false;
            }
            ResetAnimations();
            break;
    }

    m_eMode = eMode;
    m_pkModeData = pkModeData;
    m_fLastTime = -NI_INFINITY;
    m_fStartTime = -NI_INFINITY;
    m_pkClock->ResetTime(fMinTime);
    Update(fMinTime);

    if (m_eMode != eOldMode || m_pkModeData != pkOldModeData)
    {
        if (m_eMode != PlaybackMode::Transition)
            m_fTransitionStartTime = INVALID_TIME;

        OnPlaybackModeChanged(m_eMode);
    }

    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID)
{
    return ActivateSequence(uiSequenceID, 0, 1.0f, 0.0f);
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID, int iPriority,
    float fWeight, float fEaseInTime)
{
    return ActivateSequence(uiSequenceID, iPriority, fWeight, fEaseInTime,
        NiKFMTool::SYNC_SEQUENCE_ID_NONE);
}
//---------------------------------------------------------------------------
bool MAnimation::ActivateSequence(unsigned int uiSequenceID, int iPriority,
    float fWeight, float fEaseInTime, unsigned int uiSyncToSequenceID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    bool bSuccess = pkActorManager->ActivateSequence(uiSequenceID, iPriority,
        true, fWeight, fEaseInTime, uiSyncToSequenceID);

	// 다른 애니도 플레이함
	pkData->SetTargetAnimation(uiSequenceID);

    pkData->Unlock();

    OnSequenceActivated(dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]));

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MAnimation::DeactivateSequence(unsigned int uiSequenceID)
{
    return DeactivateSequence(uiSequenceID, 0.0f);
}
//---------------------------------------------------------------------------
bool MAnimation::DeactivateSequence(unsigned int uiSequenceID,
    float fEaseOutTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    bool bSuccess = pkActorManager->DeactivateSequence(uiSequenceID,
        fEaseOutTime);

	// 다른 애니도 플레이함
	for(int iItemPos=0; iItemPos<31; ++iItemPos)
	{
		NiActorManager *pkAM = pkData->GetActorManager(iItemPos);
		if(pkAM)
		{
			pkAM->DeactivateSequence(uiSequenceID, fEaseOutTime);
		}
	}

    pkData->Unlock();

    OnSequenceDeactivated(dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]));

    return bSuccess;
}
//---------------------------------------------------------------------------
bool MAnimation::SetTargetAnimation(unsigned int uiSequenceID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    bool bSuccess = pkActorManager->SetTargetAnimation(uiSequenceID);

	// 다른 애니도 플레이함
	pkData->SetTargetAnimation(uiSequenceID);

    pkData->Unlock();

    OnTargetAnimationSet(dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiSequenceID)]));

    return bSuccess;
}
//---------------------------------------------------------------------------
void MAnimation::RunUpTime(float fTime)
{
    m_pkClock->RunUpTime(fTime);
}
//---------------------------------------------------------------------------
void MAnimation::ResetAnimations()
{
    ResetAnimations(false);
}
//---------------------------------------------------------------------------
void MAnimation::ResetAnimations(bool bResetAccum)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return;
    }
    pkActorManager->Reset();
    pkActorManager->ClearAllRegisteredCallbacks();
    pkActorManager->SetCallbackObject(NULL);

	// 다른 AM도 깔끔하게 정리하자.
	for(int iItemPos=0; iItemPos<31; ++iItemPos)
	{
		NiActorManager *pkAM = pkData->GetActorManager(iItemPos);
		if(pkAM)
		{
			pkAM->Reset();
			pkAM->ClearAllRegisteredCallbacks();
			pkAM->SetCallbackObject(0);
		}
	}
	
	if (bResetAccum)
    {
        pkActorManager->GetControllerManager()->ClearCumulativeAnimations();
    }
    pkData->Unlock();

    OnResetAnimations();
}
//---------------------------------------------------------------------------
bool MAnimation::ChangeSequenceID(unsigned int uiOldID, unsigned int uiNewID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return false;
    }
    NiKFMTool::KFM_RC eRC = pkActorManager->ChangeSequenceID(uiOldID,
        uiNewID);

    // We must now cycle through the MTransitions and update them
    ArrayList* pkSeqList = get_Sequences();
    if (pkSeqList != NULL)
    {
        for (int i = 0; i < pkSeqList->Count; i++)
        {
            MSequence* pkSeq = dynamic_cast<MSequence*>(
                pkSeqList->get_Item(i));
            if (pkSeq == NULL)
            {
                continue;
            }

            // Now get the MTransition list
            MTransition* pkTranList[] = pkSeq->get_Transitions();

            for (int t=0; t<pkTranList->Count; t++)
            {
                MTransition* pkTran = dynamic_cast<MTransition*>(
                    pkTranList->get_Item(t));

                if (pkTran == NULL)
                {
                    continue;
                }

                pkTran->ChangeSequenceID(uiOldID, uiNewID);
            }
        }
    }


    pkData->Unlock();
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString( NiKFMTool::LookupReturnCode(eRC));
        return false;
    }

    m_bNeedToSave = true;
    MSequence* pkMSequence = dynamic_cast<MSequence*>(m_mapIDToSequence->Item
        [__box(uiOldID)]);
    m_mapIDToSequence->Remove(__box(uiOldID));
    m_mapIDToSequence->Add(__box(uiNewID), pkMSequence);
    MLogger::LogGeneral(String::Concat("Changing Sequence ID: ", 
        pkMSequence->Name));

    return true;
}
//---------------------------------------------------------------------------
bool MAnimation::ChangeGroupID(unsigned int uiOldID, unsigned int uiNewID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return false;
    }
    NiKFMTool::KFM_RC eRC = pkKFMTool->UpdateGroupID(uiOldID, uiNewID);
    pkData->Unlock();
    if (eRC != NiKFMTool::KFM_SUCCESS)
    {
        SetLastErrorCode(MANIMATION_ERR_KFMTOOL_ERROR);
        SetLastErrorString(pkKFMTool->LookupReturnCode(eRC));
        return false;
    }

    m_bNeedToSave = true;
    MSequenceGroup* pkMGroup = dynamic_cast<MSequenceGroup*>(
        m_mapIDToSequenceGroup->Item[__box(uiOldID)]);
    m_mapIDToSequenceGroup->Remove(__box(uiOldID));
    m_mapIDToSequenceGroup->Add(__box(uiNewID), pkMGroup);
    MLogger::LogGeneral(String::Concat("Changing Group ID: ", 
        pkMGroup->Name));

    return true;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::FindUnusedGroupID()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return 0;
    }
    unsigned int uiGroupID = pkKFMTool->FindUnusedGroupID();
    pkData->Unlock();
    return uiGroupID;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::FindUnusedSequenceID()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return 0;
    }
    unsigned int uiID = pkKFMTool->FindUnusedSequenceID();
    pkData->Unlock();
    return uiID;
}
//---------------------------------------------------------------------------
String* MAnimation::FindUnusedSequenceName(String* strBaseName)
{
    Object* pkClone = strBaseName->Clone();
    String* strNewName = dynamic_cast<String*>(pkClone);
    int iPostScript = 1;
    if (m_aSequences != NULL && strBaseName != NULL)
    {
        bool bFoundUnique = false;
        while (!bFoundUnique)
        {
            bool bFoundMatch = false;
            int iCount = m_aSequences->Count;
            for (int i = 0; i < iCount; i++)
            {
                if (dynamic_cast<MSequence*>(
                    m_aSequences->get_Item(i)) != NULL)
                {
                    if (String::Compare(dynamic_cast<MSequence*>(
                        m_aSequences->get_Item(i))->Name, strNewName) == 0)
                    {
                        strNewName = String::Concat(strBaseName, " ", 
                            iPostScript.ToString());
                        bFoundMatch = true;
                        iPostScript++;
                        break;
                    }
                }
            }

            if (!bFoundMatch)
                bFoundUnique = true;
        }
    }

    return strNewName;
}
//---------------------------------------------------------------------------
String* MAnimation::FindUnusedSequenceGroupName(String* strBaseName)
{
    Object* pkClone = strBaseName->Clone();
    String* strNewName = dynamic_cast<String*>(pkClone);
    int iPostScript = 1;
    if (m_aSequenceGroups != NULL && strBaseName != NULL)
    {
        bool bFoundUnique = false;
        while (!bFoundUnique)
        {
            bool bFoundMatch = false;
            int iCount = m_aSequenceGroups->Count;
            for (int i = 0; i < iCount; i++)
            {
                if (dynamic_cast<MSequenceGroup*>(
                    m_aSequenceGroups->get_Item(i)) != NULL)
                {
                    if (String::Compare(dynamic_cast<MSequenceGroup*>(
                        m_aSequenceGroups->get_Item(i))->Name,
                        strNewName) == 0)
                    {
                        strNewName = String::Concat(strBaseName, " ", 
                            iPostScript.ToString());
                        iPostScript++;
                        bFoundMatch = true;
                        break;
                    }
                }
            }

            if (!bFoundMatch)
                bFoundUnique = true;
        }
    }

    return strNewName;
}
//---------------------------------------------------------------------------
void MAnimation::AddToSequenceArray(MSequence* pkSequence, 
    bool bTrimToSize)
{
    m_aSequences->Add(pkSequence);
    if (bTrimToSize)
        m_aSequences->TrimToSize();
    m_mapIDToSequence->Add(__box(pkSequence->SequenceID), pkSequence);
}
//---------------------------------------------------------------------------
void MAnimation::AddToSequenceGroupArray(MSequenceGroup* pkSequenceGroup,
    bool bTrimToSize)
{
    m_aSequenceGroups->Add(pkSequenceGroup);
    if (bTrimToSize)
        m_aSequenceGroups->TrimToSize();
    m_mapIDToSequenceGroup->Add(__box(pkSequenceGroup->GroupID), 
        pkSequenceGroup);
}
//---------------------------------------------------------------------------
MItem *MAnimation::GetItemByPath(ArrayList *pkItems, String *pkPath)
{
	if(!pkItems || !pkPath)
	{
		return 0;
	}

    for (int i = 0; i < pkItems->Count; i++)
    {
        MItem* pkItem = dynamic_cast<MItem*>(
            pkItems->get_Item(i));

        if (pkItem && pkItem->get_Path()->CompareTo(pkPath) == 0)
		{
			return pkItem;
		}
    }

	return 0;
}
//---------------------------------------------------------------------------
ArrayList *MAnimation::GetItemsByFolder(String *strItemFolder)
{
	if(!m_mapFolderToItems)
	{
		return 0;
	}

	ArrayList* pkItems = dynamic_cast<ArrayList *>(m_mapFolderToItems->get_Item(strItemFolder));
	return pkItems;
}
//---------------------------------------------------------------------------
void MAnimation::RemoveFromSequenceArray(MSequence* pkSequence)
{
    m_aSequences->Remove(pkSequence);
    m_aSequences->TrimToSize();
    m_mapIDToSequence->Remove(__box(pkSequence->SequenceID));
    
}
//---------------------------------------------------------------------------
void MAnimation::RemoveFromSequenceGroupArray(MSequenceGroup* pkSequenceGroup)
{
    m_aSequenceGroups->Remove(pkSequenceGroup);
    m_aSequenceGroups->TrimToSize();
    m_mapIDToSequenceGroup->Remove(__box(pkSequenceGroup->GroupID));
}
//---------------------------------------------------------------------------
void MAnimation::BuildSequenceArray()
{
    ClearSequenceArray();
    m_mapIDToSequence->Clear();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        SetLastErrorCode(MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST);
        pkData->Unlock();
        return;
    }

    unsigned int* puiSequenceIDs;
    unsigned int uiNumIDs;
    pkKFMTool->GetSequenceIDs(puiSequenceIDs, uiNumIDs);

    m_aSequences = NiExternalNew ArrayList(uiNumIDs);
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        unsigned int uiSequenceID = puiSequenceIDs[ui];
        NiKFMTool::Sequence* pkSequence = pkKFMTool->GetSequence(
            uiSequenceID);
        assert(pkSequence);
        NiControllerSequence* pkControllerSequence = pkActorManager
            ->GetSequence(uiSequenceID);
        assert(pkControllerSequence);
        MSequence* pkMSequence = NiExternalNew MSequence(this, pkSequence,
            pkControllerSequence);
        AddToSequenceArray(pkMSequence, false);
    }
    m_aSequences->TrimToSize();
    pkData->Unlock();

    NiFree(puiSequenceIDs);
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeRowCol(MSequence* pkSequence)
{
    // This function is used when adding a new kf file incrementally.
    // If a number of kf's have been added all at once, then the
    // SetIntialTransitionTimes function should be used.
    SetInitialTransitionTimeRow(pkSequence);
    SetInitialTransitionTimeCol(pkSequence);
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeCol(MSequence* pkSequence)
{

    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSrcSeq = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);

        for (int j=0; j < pkSrcSeq->Transitions->Count; j++)
        {
            MTransition* pkTransition = pkSrcSeq->Transitions[j];
            assert(pkTransition != NULL);

            if (pkTransition->Source == NULL)
                continue;

            if (pkTransition->Destination->SequenceID != 
                pkSequence->SequenceID)
                continue;

            pkTransition->DefaultTransitionTime = 
                pkTransition->Source->Length;
        }
    }
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimeRow(MSequence* pkSequence)
{
    if (pkSequence->Transitions == NULL)
        return;

    for (int j=0; j < pkSequence->Transitions->Count; j++)
    {
        MTransition* pkTransition = pkSequence->Transitions[j];
        assert(pkTransition != NULL);

        if (pkTransition->Source == NULL)
            return;

        pkTransition->DefaultTransitionTime = pkSequence->Length;
    }
}
//---------------------------------------------------------------------------
void MAnimation::SetInitialTransitionTimes()
{
    // This function is used when adding a number of kf's at once.
    // If adding only one kf at at time, then SetInitialTransitionTimeRowCol
    // should be used.

    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSequence = dynamic_cast<MSequence*>(
            m_aSequences->Item[i]);

        SetInitialTransitionTimeRow(pkSequence);
    }
}
//---------------------------------------------------------------------------
void MAnimation::BuildSequenceGroupArray()
{
    ClearSequenceGroupArray();
    m_mapIDToSequenceGroup->Clear();

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }

    unsigned int* puiGroupIDs;
    unsigned int uiNumIDs;
    pkKFMTool->GetGroupIDs(puiGroupIDs, uiNumIDs);

    m_aSequenceGroups = NiExternalNew ArrayList(uiNumIDs);
    for (unsigned int ui = 0; ui < uiNumIDs; ui++)
    {
        unsigned int uiGroupID = puiGroupIDs[ui];
        NiKFMTool::SequenceGroup* pkGroup = pkKFMTool->GetSequenceGroup(
            uiGroupID);
        assert(pkGroup);
        MSequenceGroup* pkMGroup = NiExternalNew MSequenceGroup(this,
            pkGroup);
        AddToSequenceGroupArray(pkMGroup, false);
    }
    m_aSequenceGroups->TrimToSize();
    pkData->Unlock();

    NiFree(puiGroupIDs);
}
//---------------------------------------------------------------------------
void MAnimation::ClearAllItemArray()
{
	if(m_aFolders && m_mapFolderToItems)
	{
		for (int i = 0; i < m_aFolders->Count; i++)
		{
			String *pkFolderPath = dynamic_cast<String *>(m_aFolders->get_Item(i));
			if(pkFolderPath)
			{
				ArrayList* pkItems = dynamic_cast<ArrayList*>(m_mapFolderToItems->get_Item(pkFolderPath));
				if(pkItems)
				{
					pkItems->Clear();
				}
			}
		}
		m_aFolders->Clear();
		m_mapFolderToItems->Clear();
	}

    if (m_aAllItems)
    {
		for (int i = 0; i < m_aAllItems->Count; i++)
		{
			MItem* pkItem = dynamic_cast<MItem*>(m_aAllItems->get_Item(i));
			if(pkItem)
			{
				NiExternalDelete pkItem;		
			}
		}
		m_aAllItems->Clear();
    }
}
//---------------------------------------------------------------------------
void MAnimation::ClearSequenceArray()
{
    if (m_aSequences == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequences->Count; i++)
    {
        MSequence* pkSeq = dynamic_cast<MSequence*>(
            m_aSequences->get_Item(i));

        if (pkSeq)
            pkSeq->DeleteContents();
    }
    m_aSequences->Clear();

	if(m_mapIDToSequence == 0)
	{
		return;
	}
	m_mapIDToSequence->Clear();
}
//---------------------------------------------------------------------------
void MAnimation::ClearSequenceGroupArray()
{
    if (m_aSequenceGroups == NULL)
    {
        return;
    }

    for (int i = 0; i < m_aSequenceGroups->Count; i++)
    {
        MSequenceGroup* pkGroup = dynamic_cast<MSequenceGroup*>(
            m_aSequenceGroups->get_Item(i));

        if (pkGroup)
            pkGroup->DeleteContents();
    }
    m_aSequenceGroups->Clear();

	if(m_mapIDToSequenceGroup == NULL)
	{
		return;
	}
	
	m_mapIDToSequenceGroup->Clear();
}
//---------------------------------------------------------------------------
float MAnimation::FindTimeForAnimationToComplete(
    MSequence* pkSequence, float fBeginTime)
{
    assert(pkSequence);
    assert(fBeginTime > pkSequence->EndKeyTimeDivFreq);

    if (fBeginTime < pkSequence->EndKeyTimeDivFreq)
        return (pkSequence->EndKeyTimeDivFreq - fBeginTime);

    return 0;
}
//---------------------------------------------------------------------------
MCompletionInfo* MAnimation::FindTimeForAnimationToCompleteTransition(
    MTransition* pkTransition, float fBeginTime, float fDesiredTransitionTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return NULL;
    }
    NiActorManager::CompletionInfoPtr spCompletionInfo = pkActorManager
        ->FindTimeForAnimationToCompleteTransition(
        pkTransition->SrcID, pkTransition->DesID,
        pkTransition->GetTransition(), fBeginTime, fDesiredTransitionTime);

    pkData->Unlock();

    if (spCompletionInfo != NULL)
    {
        return NiExternalNew MCompletionInfo(spCompletionInfo);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MAnimation::OnRunUpTime(float fEndTime)
{
    if (m_fStartTime == -NI_INFINITY)
    {
        return;
    }

    // Update at start time.
    float fStartTime = m_fStartTime;
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            // Reset last time.
            m_fLastTime = -NI_INFINITY;
            break;
        case PlaybackMode::Transition:
        {
            // Reset playback mode.
            bool bSuccess = SetPlaybackMode(PlaybackMode::Transition,
                m_pkModeData);
            MAssert(bSuccess);
            break;
        }
    }
    Update(fStartTime);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    // Restore accumulation.
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            static_cast<MSequence*>(m_pkModeData)->RestoreAccum();
            break;
    }

    pkData->Unlock();

    // Update at rest of times.
    for (float fTime = m_fStartTime + m_fTimeIncrement; fTime < fEndTime;
        fTime += m_fTimeIncrement)
    {
        Update(fTime);
    }
    Update(fEndTime);
}
//---------------------------------------------------------------------------
float MAnimation::GetSequenceWeight(unsigned int uiSequenceID)
{
    float fWeight = 0.0f;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        NiControllerSequence* pkSequence = pkActorManager->GetSequence(
            (NiActorManager::SequenceID) uiSequenceID);
        if (pkSequence)
        {
            fWeight = pkSequence->GetSequenceWeight();
        }
    }
    pkData->Unlock();
    return fWeight;
}
//---------------------------------------------------------------------------
void MAnimation::SetSequenceWeight(unsigned int uiSequenceID, float fWeight)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (pkActorManager)
    {
        NiControllerSequence* pkSequence = pkActorManager->GetSequence(
            (NiActorManager::SequenceID) uiSequenceID);
        if (pkSequence)
        {
            NiControllerManager* pkControllerManager = 
                pkActorManager->GetControllerManager();
            if (pkControllerManager)
                pkControllerManager->SetSequenceWeight(pkSequence, fWeight);

        }
    }
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::HandleUnresolvedSequences()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();

    // Collect all unresolved sequences
    m_aUnresolvedSequences = NiExternalNew ArrayList();
    if (pkKFMTool && pkActorManager)
    {
        unsigned int* puiSequenceIDs = NULL;
        unsigned int uiNumIDs = 0;

        pkKFMTool->GetSequenceIDs(puiSequenceIDs, uiNumIDs);
        for (unsigned int ui = 0; ui < uiNumIDs; ui++)
        {
            unsigned int uiSeqID = puiSequenceIDs[ui];
            NiControllerSequence* pkSeq = 
                pkActorManager->GetSequence(uiSeqID);
            
            if (pkSeq == NULL)
            {
                NiKFMTool::Sequence* pkKFMSequence = pkKFMTool->GetSequence(
                    uiSeqID);
                assert(pkKFMSequence);

                MSequence::MInvalidSequenceInfo* pkInfo = NiExternalNew 
                    MSequence::MInvalidSequenceInfo();
                pkInfo->SequenceID = uiSeqID;
                pkInfo->Filename = pkKFMTool->GetFullKFFilename(uiSeqID);
                pkInfo->OriginalFilename = 
                    pkKFMTool->GetFullKFFilename(uiSeqID);
                pkInfo->AnimIndex = pkKFMSequence->GetAnimIndex();
                m_aUnresolvedSequences->Add(pkInfo);  
            }
        }
        NiFree(puiSequenceIDs);
    }

    // Allow the user to find the missing kf files
    bool bCancel = false;
    if (m_aUnresolvedSequences->Count != 0)
    {
        KFFFailedLoadArgs* pkArgs = NiExternalNew KFFFailedLoadArgs();
        pkArgs->aMissingSequences = m_aUnresolvedSequences;
        pkArgs->bCancel = false;

        OnKFLoadFailed(pkArgs);

        bCancel = pkArgs->bCancel;
    }

    // Try to reload if possible, otherwise remove the sequence
    ArrayList* pkSuccessfullyReloaded = NiExternalNew ArrayList();
    for (int ij = 0; ij < m_aUnresolvedSequences->Count; ij++)
    {
        MSequence::MInvalidSequenceInfo* pkInfo = 
            dynamic_cast<MSequence::MInvalidSequenceInfo*>(
            m_aUnresolvedSequences->get_Item(ij));
        if (pkInfo)
        {
            bool bRemove = true;
            if (!bCancel && String::Compare(pkInfo->Filename,
                pkInfo->OriginalFilename) != 0)
            {
                NiKFMTool::Sequence* pkKFMSequence = pkKFMTool->GetSequence(
                    pkInfo->SequenceID);
                assert(pkKFMSequence);

                const char* pcNewFilename = 
                    MStringToCharPointer(pkInfo->Filename);
                pkKFMSequence->SetFilename(pcNewFilename);
                MFreeCharPointer(pcNewFilename);

                if (pkActorManager->LoadSequence(pkInfo->SequenceID))
                {
                    pkSuccessfullyReloaded->Add(pkInfo);
                    bRemove = false;
                }
            }

            if (bRemove)
            {
                unsigned int uiSeqID = pkInfo->SequenceID;
                NiKFMTool::KFM_RC eRC = pkKFMTool->RemoveSequence(uiSeqID);
                assert(eRC == NiKFMTool::KFM_SUCCESS);
            }
        }
    }

    // If we successfully reloaded a file, remove it from
    // the unresolved list
    for (int k = 0; k < pkSuccessfullyReloaded->Count; k++)
    {
        m_aUnresolvedSequences->Remove(pkSuccessfullyReloaded->get_Item(k));
    }

    m_aUnresolvedSequences->TrimToSize();
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceGroupModifiedEvent(
    MSequenceGroupEventArgs* pkArgs, MSequenceGroup* pkSequenceGroup)
{
    m_bNeedToSave = true;
    OnSequenceGroupModified(pkArgs, pkSequenceGroup);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceGroupSequenceInfoModifiedEvent(
    MSequenceGroup::MSequenceInfo::PropertyType ePropChanged, 
    MSequenceGroup::MSequenceInfo* pkSequenceInfo, 
    MSequenceGroup* pkSequenceGroup)
{
    m_bNeedToSave = true;
    OnSequenceGroup_SequenceInfoModifiedEvent(ePropChanged, 
        pkSequenceInfo, pkSequenceGroup);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowSequenceModifiedEvent(
    MSequence::PropertyType ePropType,
    MSequence* pkSequence)
{
    m_bNeedToSave = true;
    OnSequenceModified(ePropType, pkSequence);
}
//---------------------------------------------------------------------------
void MAnimation::ThrowTransitionModifiedEvent(
    MTransition::PropertyType ePropChanged, MTransition* pkTransition)
{
    m_bNeedToSave = true;
    OnTransitionModified(ePropChanged, pkTransition);
}
//---------------------------------------------------------------------------
MAnimation::MANIMATION_RC MAnimation::GetLastErrorCode()
{
    return m_LastErrorCode;
}
//---------------------------------------------------------------------------
void MAnimation::SetLastErrorCode(MANIMATION_RC rc)
{
    m_LastErrorCode = rc;
    SetLastErrorString(LookupReturnCode(rc));
}
//---------------------------------------------------------------------------
String* MAnimation::GetLastErrorString()
{
    return m_LastErrorString;
}
//---------------------------------------------------------------------------
void MAnimation::SetLastErrorString(String* strError)
{
    m_LastErrorString = strError;
    MLogger::LogWarning(strError);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_Sequences()
{
    return m_aSequences;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_SequenceGroups()
{
    return m_aSequenceGroups;
}
//---------------------------------------------------------------------------
String* MAnimation::get_ModelPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strModelPath = pkKFMTool->GetModelPath();
    pkData->Unlock();

    return strModelPath;
}
//---------------------------------------------------------------------------
String* MAnimation::get_FullModelPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strFullModelPath = pkKFMTool->GetFullModelPath();
    pkData->Unlock();

    return strFullModelPath;
}
//---------------------------------------------------------------------------
String* MAnimation::get_ModelRoot()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strModelRoot = pkKFMTool->GetModelRoot();
    pkData->Unlock();

    return strModelRoot;
}
//---------------------------------------------------------------------------
String* MAnimation::get_KFMFilename()
{
    return m_strKFMFilename;
}
//---------------------------------------------------------------------------
String* MAnimation::get_BaseKFMPath()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strKFMPath = pkKFMTool->GetBaseKFMPath();
    pkData->Unlock();
    return strKFMPath;
}
//---------------------------------------------------------------------------
void MAnimation::set_BaseKFMPath(String* strPath)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return;
    }
    m_bNeedToSave = true;
    const char* pcPath = MStringToCharPointer(strPath);
    pkKFMTool->SetBaseKFMPath(pcPath);
    MFreeCharPointer(pcPath);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupEaseIn()
{
    return m_fDefaultSeqGroupEaseIn;
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupEaseOut()
{
    return m_fDefaultSeqGroupEaseOut;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::get_DefaultSeqGroupPriority()
{
    return m_uiDefaultSeqGroupPriority;
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSeqGroupWeight()
{
    return m_fDefaultSeqGroupWeight;
}
//---------------------------------------------------------------------------
MAnimation::PlaybackMode MAnimation::get_Mode()
{
    return m_eMode;
}
//---------------------------------------------------------------------------
float MAnimation::get_CurrentTime()
{
    float fCurrentTime = 0.0f;
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
            fCurrentTime = m_pkClock->CurrentTime - m_fStartTime;
            break;
        case PlaybackMode::Transition:
            fCurrentTime = m_pkClock->CurrentTime;
            break;
    }

    return fCurrentTime;
}
//---------------------------------------------------------------------------
void MAnimation::set_CurrentTime(float fCurrentTime)
{
    switch (m_eMode)
    {
        case PlaybackMode::Sequence:
			m_pkClock->CurrentTime = m_fStartTime + fCurrentTime;
            break;
        case PlaybackMode::Transition:
            m_pkClock->CurrentTime = fCurrentTime;
            break;
    }

	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	NiActorManager *pkActorManager = pkData->GetActorManager();
	if(pkActorManager)
	{
		pkActorManager->RebuildTimeline();
	}
	
	MEffectMan *pkEffectMan = pkData->GetEffectMan();
	if(pkEffectMan && m_pkClock->StartTime == fCurrentTime)
	{
		pkEffectMan->DetachAllEffects();
	}
	pkData->Unlock();
}
//---------------------------------------------------------------------------
float MAnimation::get_IncrementTime()
{
    return m_fTimeIncrement;
}
//---------------------------------------------------------------------------
void MAnimation::set_IncrementTime(float fTime)
{
    m_fTimeIncrement = fTime;
}
//---------------------------------------------------------------------------
MSequence* MAnimation::get_ActiveSequence()
{
    if (m_eMode != PlaybackMode::Sequence)
    {
        return NULL;
    }

    return static_cast<MSequence*>(m_pkModeData);
}
//---------------------------------------------------------------------------
MSequenceGroup* MAnimation::get_ActiveSequenceGroup()
{
    if (m_eMode != PlaybackMode::SequenceGroup)
    {
        return NULL;
    }

    return static_cast<MSequenceGroup*>(m_pkModeData);
}
//---------------------------------------------------------------------------
MTransition* MAnimation::get_ActiveTransition()
{
    if (m_eMode != PlaybackMode::Transition)
    {
        return NULL;
    }

    return static_cast<MTransition*>(m_pkModeData);
}
//---------------------------------------------------------------------------
float MAnimation::get_TransitionStartTime()
{
    if (m_eMode != PlaybackMode::Transition)
    {
        MAssert(false, "Invalid PlaybackMode for TransitionStartTime "
            "access", "TransitionStartTime can only be accessed in "
            "Transition playback mode.");
        return INVALID_TIME;
    }

    return m_fTransitionStartTime;
}
//---------------------------------------------------------------------------
ArrayList* MAnimation::get_UnresolvedSequenceInfo()
{
    return m_aUnresolvedSequences;
}
//---------------------------------------------------------------------------
void MAnimation::set_TransitionStartTime(float fTransitionStartTime)
{
    if (m_eMode != PlaybackMode::Transition)
    {
        MAssert(false, "Invalid PlaybackMode for TransitionStartTime "
            "access", "TransitionStartTime can only be accessed in "
            "Transition playback mode.");
        return;
    }

    m_fTransitionStartTime = fTransitionStartTime;
}
//---------------------------------------------------------------------------
unsigned int MAnimation::get_LoopCounterRange()
{
    return m_pkCallbackObject->GetCounterRange();
}
//---------------------------------------------------------------------------
void MAnimation::set_LoopCounterRange(unsigned int uiLoopCounterRange)
{
    m_pkCallbackObject->SetCounterRange(uiLoopCounterRange);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MAnimation::get_DefaultSyncTransType()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    MTransition::TransitionType eType = MTransition::TranslateTransitionType(
        pkData->GetKFMTool()->GetDefaultSyncTransType(), NULL);
    pkData->Unlock();

    return eType;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultSyncTransType(MTransition::TransitionType eType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    NiKFMTool::KFM_RC eRC = pkData->GetKFMTool()->SetDefaultSyncTransType(
        MTransition::TranslateTransitionType(eType));
    assert(eRC == NiKFMTool::KFM_SUCCESS);
    pkData->Unlock();

    m_bNeedToSave = true;
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultSync);
}
//---------------------------------------------------------------------------
MTransition::TransitionType MAnimation::get_DefaultNonSyncTransType()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    MTransition::TransitionType eType = MTransition::TranslateTransitionType(
        pkData->GetKFMTool()->GetDefaultNonSyncTransType(), NULL);
    pkData->Unlock();

    return eType;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultNonSyncTransType(
    MTransition::TransitionType eType)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    NiKFMTool::KFM_RC eRC = pkData->GetKFMTool()->SetDefaultNonSyncTransType(
        MTransition::TranslateTransitionType(eType));
    assert(eRC == NiKFMTool::KFM_SUCCESS);
    pkData->Unlock();

    m_bNeedToSave = true;
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultNonSync);
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultSyncTransDuration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    float fDuration = pkData->GetKFMTool()->GetDefaultSyncTransDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultSyncTransDuration(float fDuration)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    pkData->GetKFMTool()->SetDefaultSyncTransDuration(fDuration);
    pkData->Unlock();

    m_bNeedToSave = true;
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultSync);
}
//---------------------------------------------------------------------------
float MAnimation::get_DefaultNonSyncTransDuration()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    float fDuration = pkData->GetKFMTool()->GetDefaultNonSyncTransDuration();
    pkData->Unlock();

    return fDuration;
}
//---------------------------------------------------------------------------
void MAnimation::set_DefaultNonSyncTransDuration(float fDuration)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    assert(pkData->GetKFMTool());
    pkData->GetKFMTool()->SetDefaultNonSyncTransDuration(fDuration);
    pkData->Unlock();

    m_bNeedToSave = true;
    OnDefaultTransitionSettingsChanged(MTransition::Trans_DefaultNonSync);
}
//---------------------------------------------------------------------------
MSequence* MAnimation::get_TargetAnimation()
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiActorManager* pkActorManager = pkData->GetActorManager();
    if (!pkActorManager)
    {
        pkData->Unlock();
        return NULL;
    }
    unsigned int uiSequenceID = pkActorManager->GetTargetAnimation();
    pkData->Unlock();

    return GetSequence(uiSequenceID);
}
//---------------------------------------------------------------------------
bool MAnimation::get_NeedToSave()
{
    return m_bNeedToSave;
}
//---------------------------------------------------------------------------
bool MAnimation::get_CanSave()
{
    return this->m_bCanSave;
}
//---------------------------------------------------------------------------
inline const char* MAnimation::LookupReturnCode(
    MAnimation::MANIMATION_RC eReturnCode)
{
    switch (eReturnCode)
    {
        case MANIMATION_ERR_KFMTOOL_ERROR:
            return "KFMTool Error";
        case MANIMATION_ERR_CREATE_ACTOR_MANAGER_FAILED:
            return "The actor manager could not be created.";
            // SetLastError("Failed to create NiActorManager!");
        case MANIMATION_ERR_FILENAME_EMPTY:
            return "The filename is empty.";
        case MANIMATION_ERR_KF_FILENAME_EMPTY:
            return "The KF filename was empty.";
        case MANIMATION_ERR_NO_NIF_ROOT_EXIST:
            return "The NIF root does not exist.";
        case MANIMATION_ERR_NO_KFMTOOL:
            return "The KFMTool does not exist.";
        case MANIMATION_ERR_MODEL_PATH_EMPTY:
            return "The model path is empty.";
        case MANIMATION_ERR_FAILED_LOADING_NIF:
            return "Unable to load NIF file.";
        case MANIMATION_ERR_NIF_OBJECT_NOT_NIOBJECTNET:
            return "First object in NIF file is not an NiObjectNET.";
        case MANIMATION_ERR_NIF_NIF_OBJECT_NO_NAME_FOR_FILE:
            return "First object in NIF file does not have a name.";
        case MANIMATION_ERR_FAILED_TO_RELOAD_NIF:
            return "Failed to reload the NIF file.";
        case MANIMATION_ERR_MODEL_ROOT_NIF_MISMATCH:
            return "Model root does not match NIF file";
        case MANIMATION_ERR_SEQUENCE_DOES_NOT_EXIST:
            return "The sequence did not exist";
        case MANIMATION_ERR_SRC_SEQUENCE_DOES_NOT_EXIST:
            return "The source sequence did not exist";
        case MANIMATION_ERR_SRC_OR_DEST_DOES_NOT_EXIST:
            return "The source or destination sequences do not exist.";
        case MANIMATION_ERR_TRANSITION_DOES_NOT_EXIST:
            return "The transition does not exist";
        case MANIMATION_ERR_ACTORMANAGER_DOES_NOT_EXIST:
            return "The actor manager does not exist.";

        case MANIMATION_ERR_FAILED_TO_LOAD_CHARACTER_NIF:
            return "Unable to load character NIF file";
        case MANIMATION_ERR_SEQUENCE_CAN_NOT_BE_LOADED:
            return "Sequence could not be loaded";
    }
    // unknown type
    MAssert(false);
    return NULL;
}
//---------------------------------------------------------------------------
String* MAnimation::GetFullKFFilename(unsigned int uiSequenceID)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    NiKFMTool* pkKFMTool = pkData->GetKFMTool();
    if (!pkKFMTool)
    {
        SetLastErrorCode(MANIMATION_ERR_NO_KFMTOOL);
        pkData->Unlock();
        return NULL;
    }
    String* strFullKFFilename = pkKFMTool->GetFullKFFilename(uiSequenceID);
    pkData->Unlock();

    return strFullKFFilename;
}
//---------------------------------------------------------------------------
void MAnimation::EventCallbackObject::EventActivated(NiActorManager *pkManager, 
													 NiActorManager::EventType kEventType,
													 NiActorManager::SequenceID eSequenceID, 
													 float fCurrentTime, 
													 float fEventTime, 
				NiAnimationEvent *pkAnimationEvent)
{
	switch(kEventType)
	{
	case	NiActorManager::FADEOUTEFFECT_EVENT:
		{
			DoFadeOutEffectEvent(pkAnimationEvent);
		}
		break;
	case	NiActorManager::EFFECT_EVENT:
		{
			DoEffectEvent(pkAnimationEvent,pkManager,fEventTime);
		}
		break;
	case	NiActorManager::AUDIO_EVENT:
		{
			DoAudioEvent(pkAnimationEvent,pkManager);
		}
		break;
	}
}
void	MAnimation::EventCallbackObject::DoAudioEvent(NiAnimationEvent *pkAnimationEvent,NiActorManager *pkActorManager)
{
	NiAnimationEventSound	*pkSoundData = NiDynamicCast(NiAnimationEventSound,pkAnimationEvent);
	if(!pkSoundData)
	{
		return;
	}

	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	MAudioMan *pkAudioMan = pkData->GetAudioMan();
	NiAudioSourcePtr pkAudioSource = pkAudioMan->GetAudioSource(pkSoundData->GetName(),
		pkSoundData->GetVolume(), pkSoundData->GetMinDist(), pkSoundData->GetMaxDist());
	if(pkAudioSource)
	{
		if(pkAudioSource->GetStatus() == NiAudioSource::PLAYING)
		{
			pkAudioSource->Stop();
		}
		pkAudioSource->Play();
		NiNode *pkNode = (NiNode *)pkActorManager->GetNIFRoot();
		pkNode->AttachChild(pkAudioSource);
	}

	pkData->Unlock();
}
void	MAnimation::EventCallbackObject::DoEffectEvent(NiAnimationEvent *pkAnimationEvent,NiActorManager *pkActorManager,float fEventTime)
{
	NiAnimationEventEffect	*pkEffectData = NiDynamicCast(NiAnimationEventEffect,pkAnimationEvent);
	if(!pkEffectData)
	{
		return;
	}

	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	MEffectMan *pkEffectMan = pkData->GetEffectMan();
	PgEffect *pkEffect = pkEffectMan->GetEffect(pkEffectData);
	if(pkEffect)
	{
		NiAVObject *pkModelRoot = pkActorManager->GetNIFRoot();
		NiNode *pkTargetPoint = (NiNode *)pkModelRoot->GetObjectByName(pkEffectData->GetAttachPointName());
		if(!pkTargetPoint)
		{
			return;
		}

		pkEffect->SetBeginTime(fEventTime);
		pkEffect->GetEffectRoot()->AddExtraData(PgEffect::EXTRA_DATA_EFFECT_TEXT_KEY.c_str(),NiNew NiStringExtraData(pkAnimationEvent->GetTextKey()->GetText()));

		pkEffectMan->AttachTo(pkEffect, pkTargetPoint);

		if(pkEffectData->GetUseFadeIn() && pkEffectData->GetFadeInTime()>0)
		{
			pkEffect->SetProcessor(NiNew PgEffectProcessorFadeIn(pkEffectData->GetFadeInTime()));
		}
		pkEffect->Update(fEventTime);
	}

	pkData->Unlock();
}
void	MAnimation::EventCallbackObject::DoFadeOutEffectEvent(NiAnimationEvent *pkAnimationEvent)
{
	NiAnimationEventFadeOutEffect	*pkFadeOutEffectData = NiDynamicCast(NiAnimationEventFadeOutEffect,pkAnimationEvent);
	if(!pkFadeOutEffectData)
	{
		return;
	}
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();
	MEffectMan *pkEffectMan = pkData->GetEffectMan();
	PgEffect *pkEffect = pkEffectMan->FindEffectWithTextKey((char const*)pkFadeOutEffectData->GetEffectName());
	if(pkEffect)
	{
		pkEffect->SetProcessor(NiNew PgEffectProcessorFadeOut(pkFadeOutEffectData->GetFadeOutTime()));
	}

	pkData->Unlock();
}

//---------------------------------------------------------------------------
MAnimation::EventCallbackObject::~EventCallbackObject()
{
}
//---------------------------------------------------------------------------
