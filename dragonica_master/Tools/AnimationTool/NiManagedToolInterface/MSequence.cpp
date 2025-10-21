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
#include "MSequence.h"
#include "MSharedData.h"
#include "MAnimation.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
// MSequence::MTextKey
//---------------------------------------------------------------------------
MSequence::MTextKey::MTextKey(float fTime, String *pkKeyName)
{
	m_fTime = fTime;
	m_strText = pkKeyName;
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MTextKey(NiTextKey* pkKey)
{
	m_fTime = pkKey->GetTime();
	m_strText = NiExternalNew String(pkKey->GetText());
}
//---------------------------------------------------------------------------
MSequence::MTextKey::~MTextKey()
{
}
//---------------------------------------------------------------------------
void MSequence::MTextKey::set_Time(float fTime)
{
	m_fTime = fTime;
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::get_Time()
{
	return m_fTime;
}
//---------------------------------------------------------------------------
String* MSequence::MTextKey::get_Text()
{
	return m_strText;
}
//---------------------------------------------------------------------------
Object *MSequence::MTextKey::get_ExtraData()
{
	return m_pkExtraData;
}
//---------------------------------------------------------------------------
void MSequence::MTextKey::set_ExtraData(Object *pkObject)
{
	m_pkExtraData = pkObject;
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MAudioData::MAudioData(String *pkName, float fVol,
											float fMin, float fMax)
{
	const char *pcName = MStringToCharPointer(pkName);
	m_pkSound = NiNew NiAnimationEventSound(0, NiFixedString(pcName), fVol, fMin, fMax);
	m_bManualDelete = true;
	MFreeCharPointer(pcName);
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MAudioData::MAudioData(NiAnimationEventSound *pkSound) :
	m_pkSound(pkSound)
{
	m_bManualDelete = false;
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MAudioData::~MAudioData()
{
	/*if(m_pkSound && m_bManualDelete)
	{
		NiDelete m_pkSound;
	}*/
}
//---------------------------------------------------------------------------
String *MSequence::MTextKey::MAudioData::get_SoundName()
{
	return m_pkSound->GetName();
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::MAudioData::get_Volume()
{
	return m_pkSound->GetVolume();
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::MAudioData::get_MaxDistance()
{
	return m_pkSound->GetMaxDist();
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::MAudioData::get_MinDistance()
{
	return m_pkSound->GetMinDist();
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MEffectData::MEffectData(String *pkName, String *pkAttachPoint, 
											  float fScale, bool bAttach, bool bLoop)
{
	const char *pcName = MStringToCharPointer(pkName);
	const char *pcAttachPoint = MStringToCharPointer(pkAttachPoint);
	m_pkEffect = NiNew NiAnimationEventEffect(0, NiFixedString(pcName), NiFixedString(pcAttachPoint), fScale, bAttach, bLoop, 0, false);
	m_bManualDelete = true;
	MFreeCharPointer(pcName);
	MFreeCharPointer(pcAttachPoint);
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MEffectData::MEffectData(NiAnimationEventEffect *pkEffect) :
	m_pkEffect(pkEffect)
{
	m_bManualDelete = false;
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MEffectData::~MEffectData()
{
	//if(m_pkEffect && m_bManualDelete)
	//{
	//	NiDelete m_pkEffect;
	//}
}
//---------------------------------------------------------------------------
String *MSequence::MTextKey::MEffectData::get_EffectName()
{
	return m_pkEffect->GetEffectName();
}
//---------------------------------------------------------------------------
String *MSequence::MTextKey::MEffectData::get_AttachPoint()
{
	return m_pkEffect->GetAttachPointName();
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::MEffectData::get_Scale()
{
	return m_pkEffect->GetScale();
}
//---------------------------------------------------------------------------
bool MSequence::MTextKey::MEffectData::get_UseFadeIn()
{
	return	m_pkEffect->GetUseFadeIn();
}
float MSequence::MTextKey::MEffectData::get_FadeInTime()
{
	return	m_pkEffect->GetFadeInTime();
}
bool MSequence::MTextKey::MEffectData::get_UseFadeOutWhenSequenceChanged()
{
	return	m_pkEffect->GetUseFadeOutWhenSequenceChanged();
}
float MSequence::MTextKey::MEffectData::get_FadeOutTimeWhenSequenceChanged()
{
	return	m_pkEffect->GetFadeOutTimeWhenSequenceChanged();
}

//---------------------------------------------------------------------------
//	MFadeOutEffectData
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MSequence::MTextKey::MFadeOutEffectData::MFadeOutEffectData(String *pkEffectName, float fFadeOutTime)
{
	const char *pcEffectName = MStringToCharPointer(pkEffectName);
	m_pkFadeOutEffect = NiNew NiAnimationEventFadeOutEffect(0, NiFixedString(pcEffectName), fFadeOutTime);
	m_bManualDelete = true;
	MFreeCharPointer(pcEffectName);
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MFadeOutEffectData::MFadeOutEffectData(NiAnimationEventFadeOutEffect *pkFadeOutEffect) :
	m_pkFadeOutEffect(pkFadeOutEffect)
{
	m_bManualDelete = false;
}
//---------------------------------------------------------------------------
MSequence::MTextKey::MFadeOutEffectData::~MFadeOutEffectData()
{
	//if(m_pkFadeOutEffect && m_bManualDelete)
	//{
	//	NiDelete m_pkFadeOutEffect;
	//}
}
//---------------------------------------------------------------------------
String *MSequence::MTextKey::MFadeOutEffectData::get_EffectName()
{
	return m_pkFadeOutEffect->GetEffectName();
}
//---------------------------------------------------------------------------
float MSequence::MTextKey::MFadeOutEffectData::get_FadeOutTime()
{
	return m_pkFadeOutEffect->GetFadeOutTime();
}
//---------------------------------------------------------------------------
// MSequence::MSequenceInfo
//---------------------------------------------------------------------------
MSequence::MInterpolatorInfo::MInterpolatorInfo(NiInterpolator* pkInterp, 
    NiControllerSequence::IDTag* pkTag)
{
    m_pkInterpolator = pkInterp;
    m_pkIDTag = pkTag;
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_RTTI()
{
    return NiExternalNew String(m_pkInterpolator->GetRTTI()->GetName());
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_AVObjectName()
{
    return NiExternalNew String(m_pkIDTag->GetAVObjectName());
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_PropertyType()
{
    return NiExternalNew String(m_pkIDTag->GetPropertyType());
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_ControllerType()
{
    return NiExternalNew String(m_pkIDTag->GetCtlrType());
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_ControllerID()
{
    return NiExternalNew String(m_pkIDTag->GetCtlrID());
}
//---------------------------------------------------------------------------
String* MSequence::MInterpolatorInfo::get_InterpolatorID()
{
    return NiExternalNew String(m_pkIDTag->GetInterpolatorID());
}
//---------------------------------------------------------------------------
unsigned short MSequence::MInterpolatorInfo::GetKeyChannelCount()
{
    if (NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return  ((NiBSplineInterpolator*)  m_pkInterpolator)->
            GetChannelCount();
    }

    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedInterpolator*) m_pkInterpolator)->
            GetKeyChannelCount();
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MInterpolatorInfo::GetKeyCount(
    unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return  ((NiBSplineInterpolator*)  m_pkInterpolator)->
            GetControlPointCount(usChannel);
    }

    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedInterpolator*) m_pkInterpolator)->
            GetKeyCount(usChannel);      
    }
}
//---------------------------------------------------------------------------
MSequence::MInterpolatorInfo::KeyType 
MSequence::MInterpolatorInfo::GetKeyType(unsigned short usChannel)
{
    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return NUMKEYTYPES;
    }
    else
    {
        return (MSequence::MInterpolatorInfo::KeyType)
            ((NiKeyBasedInterpolator*) m_pkInterpolator)->GetKeyType(
            usChannel);      
    }
}
//---------------------------------------------------------------------------
MSequence::MInterpolatorInfo::KeyContent 
MSequence::MInterpolatorInfo::GetKeyContent(unsigned short usChannel)
{
    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return NUMKEYCONTENTS;
    }
    else
    {
        return (MSequence::MInterpolatorInfo::KeyContent)
            ((NiKeyBasedInterpolator*) m_pkInterpolator)->
            GetKeyContent(usChannel);  
    }

}
//---------------------------------------------------------------------------
bool MSequence::MInterpolatorInfo::GetChannelPosed(unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return  NiVirtualBoolBugWrapper::NiBSplineInterpolator_GetChannelPosed(
            (NiBSplineInterpolator*) m_pkInterpolator, usChannel) ?
            true : false;       
    }

    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return NiVirtualBoolBugWrapper::NiKeyBasedInterpolator_GetChannelPosed(
            (NiKeyBasedInterpolator*) m_pkInterpolator, usChannel) ?
            true : false;      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MInterpolatorInfo::GetDimension(
    unsigned short usChannel)
{
    if (!NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return ((NiBSplineInterpolator*) m_pkInterpolator)->
            GetDimension(usChannel);      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MInterpolatorInfo::GetDegree(
    unsigned short usChannel)
{
    if (!NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return ((NiBSplineInterpolator*) m_pkInterpolator)->
            GetDegree(usChannel);      
    }
}
//---------------------------------------------------------------------------
unsigned int MSequence::MInterpolatorInfo::GetAllocatedSize(
    unsigned short usChannel)
{
    if (NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return ((NiBSplineInterpolator*) m_pkInterpolator)->
            GetAllocatedSize(usChannel);      
    }

    if (!NiIsKindOf(NiKeyBasedInterpolator, m_pkInterpolator))
    {
        return 0;
    }
    else
    {
        return ((NiKeyBasedInterpolator*) m_pkInterpolator)->
            GetAllocatedSize(usChannel);      
    }
}
//---------------------------------------------------------------------------
bool MSequence::MInterpolatorInfo::IsBSplineInterpolator()
{
    if (NiIsKindOf(NiBSplineInterpolator, m_pkInterpolator))
    {
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// MSequence
//---------------------------------------------------------------------------
MSequence::MSequence(MAnimation* pkAnimation, NiKFMTool::Sequence* pkSequence,
    NiControllerSequence* pkControllerSequence) : m_pkAnimation(pkAnimation), 
    m_pkSequence(pkSequence), m_pkControllerSequence(pkControllerSequence),
    m_aTransitions(NULL), m_pkStoredAccum(NULL)
{
    assert(m_pkSequence && m_pkControllerSequence && m_pkAnimation);

    RebuildTransitionsArray();
    m_pkStoredAccum = NiNew NiQuatTransform;
    m_bDisposed = false;
    FillArrays();

    DefaultPositionTime = 0;
}
//---------------------------------------------------------------------------
void MSequence::DeleteContents()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    m_pkAnimation = NULL;
    NiDelete m_pkStoredAccum;
    m_pkStoredAccum = NULL;

    if (m_aTransitions != NULL)
    {
        for (int i = 0; i < m_aTransitions->Length; i++)
        {
            m_aTransitions[i]->DeleteContents();
        }
        m_aTransitions = NULL;
    }
    m_bDisposed = true;
}
//---------------------------------------------------------------------------
int MSequence::CompareTo(Object* pkObject)
{
    MSequence* pkSeq = dynamic_cast<MSequence*>(pkObject);
    assert(pkSeq);
    return this->Name->CompareTo(pkSeq->Name);
}
//---------------------------------------------------------------------------
void MSequence::RebuildTransitionsArray()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    unsigned int uiCount = m_pkSequence->GetTransitions().GetCount();
    MTransition* aNewTransitions[] = NULL;
    if (uiCount > 0)
    {
        aNewTransitions = NiExternalNew MTransition*[uiCount];
        int uiIndex = 0;
        NiTMapIterator pos = m_pkSequence->GetTransitions().GetFirstPos();
        while (pos)
        {
            unsigned int uiID;
            NiKFMTool::Transition* pkTransition;
            m_pkSequence->GetTransitions().GetNext(pos, uiID, pkTransition);

            MTransition* pkMTransition = NULL;
            if (m_aTransitions != NULL)
            {
                for (int i = 0; i < m_aTransitions->Length; i++)
                {
                    if (m_aTransitions[i]->DesID != uiID)
                        continue;

                    if (m_aTransitions[i]->GetTransition() == pkTransition)
                    {
                        pkMTransition = m_aTransitions[i];
                        break;
                    }
                }
            }

            if (pkMTransition == NULL)
            {
                pkMTransition = NiExternalNew MTransition(m_pkAnimation,
                    this->SequenceID, uiID, pkTransition);
            }

            aNewTransitions[uiIndex++] = pkMTransition;
        }
    }
    m_aTransitions = aNewTransitions;

    pkData->Unlock();
}
//---------------------------------------------------------------------------
MTransition* MSequence::GetTransition(unsigned int uiDesID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    
    if (m_aTransitions == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < m_aTransitions->Length; i++)
    {
        MTransition* pkTransition = m_aTransitions[i];
        if (pkTransition->DesID == uiDesID)
        {
            return pkTransition;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
float MSequence::GetKeyTimeAt(String* strKeyText)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    const char* pcKeyText = MStringToCharPointer(strKeyText);
    float fKeyTime = m_pkControllerSequence->GetKeyTimeAt(pcKeyText);
    MFreeCharPointer(pcKeyText);

    pkData->Unlock();

    if (fKeyTime == NiControllerSequence::INVALID_TIME)
    {
        return INVALID_TIME;
    }

    return fKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::GetKeyTimeAtDivFreq(String* strKeyText)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    const char* pcKeyText = MStringToCharPointer(strKeyText);
    float fKeyTime = m_pkControllerSequence->GetKeyTimeAtDivFreq(pcKeyText);
    MFreeCharPointer(pcKeyText);

    pkData->Unlock();

    if (fKeyTime == NiControllerSequence::INVALID_TIME)
    {
        return INVALID_TIME;
    }

    return fKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::TimeDivFreq(float fTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (fTime != NiKFMTool::MAX_DURATION)
    {
        fTime = m_pkControllerSequence->TimeDivFreq(fTime);
    }
    pkData->Unlock();

    return fTime;
}
//---------------------------------------------------------------------------
float MSequence::TimeMultFreq(float fTime)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (fTime != NiKFMTool::MAX_DURATION)
    {
        fTime = m_pkControllerSequence->TimeMultFreq(fTime);
    }
    pkData->Unlock();

    return fTime;
}
//---------------------------------------------------------------------------
void MSequence::StoreAccum()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkControllerSequence->GetOwner()->GetAccumulatedTransform(
        *m_pkStoredAccum);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::RestoreAccum()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkControllerSequence->GetOwner()->SetAccumulatedTransform(
        *m_pkStoredAccum);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
bool MSequence::IsDeleted()
{
    return m_bDisposed;
}
//---------------------------------------------------------------------------
String* MSequence::ToString()
{
    return Name;
}
//---------------------------------------------------------------------------
bool MSequence::CanSyncTo(MSequence* pkSequence)
{
    bool bReturn = false;
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (m_pkControllerSequence)
    {
        bReturn = m_pkControllerSequence->CanSyncTo(
           pkSequence->m_pkControllerSequence);
    }
    pkData->Unlock();
    return bReturn;
}
//---------------------------------------------------------------------------
float MSequence::FindCorrespondingMorphFrame(MSequence* pkPartnerSequence,
    float fFrameTime)
{
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fMorphFrame = m_pkControllerSequence->FindCorrespondingMorphFrame(
        pkPartnerSequence->GetControllerSequence(), fFrameTime);
    pkData->Unlock();

    return fMorphFrame;
}
//---------------------------------------------------------------------------
NiControllerSequence* MSequence::GetControllerSequence()
{
    return m_pkControllerSequence;
}
//---------------------------------------------------------------------------
void MSequence::RefreshTextKeys()
{
	unsigned int uiSequenceID = m_pkSequence->GetSequenceID();
	MSharedData *pkData = MSharedData::Instance;
	pkData->Lock();

	NiKFMTool *pkKFMTool = pkData->GetKFMTool();

	// TODO : Null을 넣으면, 메모리 릭이 일어나는지 안일어나는지 체크!
	m_aTextKeys = NULL;
	NiTextKeyExtraData* pkTextKeyExtraData = 
		m_pkControllerSequence->GetTextKeys();
	unsigned int uiNumKeys = 0;
	if (pkTextKeyExtraData)
	{
		NiTextKey* pkTextKeys = pkTextKeyExtraData->GetKeys(uiNumKeys);
		if (uiNumKeys > 0)
		{
			m_aTextKeys = NiExternalNew MTextKey*[uiNumKeys];
			for (unsigned int ui = 0; ui < uiNumKeys; ui++)
			{
				 m_aTextKeys[ui] = NiExternalNew MTextKey(&pkTextKeys[ui]);

				 for(unsigned int uiType = 0;uiType<NiActorManager::EVENT_TYPE_MAX;++uiType)
				 {
					 NiAnimationEventSet	*pkEventSet = pkKFMTool->GetAnimationEventSet(uiType,uiSequenceID);
					 if(!pkEventSet || pkEventSet->GetSize() == 0)
					 {
						 continue;
					 }

					 // Effect
					 for (unsigned int uiIdx = 0; uiIdx < pkEventSet->GetSize(); ++uiIdx)
					 {
						 NiAnimationEvent *pkEvent = pkEventSet->GetAt(uiIdx);
						 if(m_aTextKeys[ui]->Time != pkEvent->GetTextKey()->GetTime())
						 {
							 continue;
						 }

						 switch(uiType)
						 {
						 case	NiActorManager::EFFECT_EVENT:
							 {
								 MTextKey::MEffectData *pkEffectData = NiExternalNew MTextKey::MEffectData(NiDynamicCast(NiAnimationEventEffect,pkEvent));
								 m_aTextKeys[ui]->set_ExtraData(pkEffectData);
							 }
							 break;
						 case	NiActorManager::AUDIO_EVENT:
							 {
								 MTextKey::MAudioData *pkSoundData = NiExternalNew MTextKey::MAudioData(NiDynamicCast(NiAnimationEventSound,pkEvent));
								 m_aTextKeys[ui]->set_ExtraData(pkSoundData);
							 }
							 break;
						 case	NiActorManager::TEXT_KEY_EVENT:
							 {
								 m_aTextKeys[ui]->set_ExtraData(S"NotNull");
							 }
							 break;
						 case	NiActorManager::FADEOUTEFFECT_EVENT:
							 {
								 MTextKey::MFadeOutEffectData *pkFadeOutEffectData = NiExternalNew MTextKey::MFadeOutEffectData(NiDynamicCast(NiAnimationEventFadeOutEffect,pkEvent));
								 m_aTextKeys[ui]->set_ExtraData(pkFadeOutEffectData);

							 }
							 break;
						 }

						 break;
					 }
				 }
			}
		}
	}


	pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::FillArrays()
{
	System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    assert(m_pkControllerSequence);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    ClearArrays();
    unsigned int uiCount = m_pkControllerSequence->GetIDTagCount();
    if (uiCount > 0)
    {
		m_aInterpolatorInfo = NiExternalNew MInterpolatorInfo*[uiCount];
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            if (m_pkControllerSequence->GetInterpolatorAt(ui) != NULL &&
                m_pkControllerSequence->GetIDTag(ui) != NULL)
            {
                m_aInterpolatorInfo[ui] = NiExternalNew MInterpolatorInfo(
                    m_pkControllerSequence->GetInterpolatorAt(ui), 
                    m_pkControllerSequence->GetIDTag(ui));
            }
            else
            {
                m_aInterpolatorInfo[ui] = NULL;
            }
        }
    }

	RefreshTextKeys();
	//NiTextKeyExtraData* pkTextKeyExtraData = 
	//	m_pkControllerSequence->GetTextKeys();
	//unsigned int uiNumKeys = 0;
	//if (pkTextKeyExtraData)
	//{
	//	NiTextKey* pkTextKeys = pkTextKeyExtraData->GetKeys(uiNumKeys);
	//	if (uiNumKeys > 0)
	//	{
	//		m_aTextKeys = NiExternalNew MTextKey*[uiNumKeys];
	//		for (unsigned int ui = 0; ui < uiNumKeys; ui++)
	//		{
	//			 m_aTextKeys[ui] = NiExternalNew MTextKey(&pkTextKeys[ui]);
	//		}
	//	}
	//}
    pkData->Unlock();
}
//---------------------------------------------------------------------------
void MSequence::ClearArrays()
{
	System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
	m_aInterpolatorInfo = NULL;
    m_aTextKeys = NULL;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
unsigned int MSequence::get_SequenceID()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiSequenceID = m_pkSequence->GetSequenceID();
    pkData->Unlock();

    return uiSequenceID;
}
//---------------------------------------------------------------------------
void MSequence::set_SequenceID(unsigned int uiSequenceID)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    unsigned int uiOldID = m_pkSequence->GetSequenceID();
    pkData->Unlock();

    assert(m_pkAnimation);
    if (m_pkAnimation->ChangeSequenceID(uiOldID, uiSequenceID))
    {
        m_pkAnimation->ThrowSequenceModifiedEvent(Prop_SequenceID, this);
    }
}
//---------------------------------------------------------------------------
String* MSequence::get_Name()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strName = m_pkControllerSequence->GetName();
    pkData->Unlock();

    return strName;
}
//---------------------------------------------------------------------------
String* MSequence::get_Filename()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    String* strFilename = m_pkSequence->GetFilename();
    pkData->Unlock();

    return strFilename;
}
//---------------------------------------------------------------------------
void MSequence::set_Filename(String* strFilename)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    const char* pcFilename = MStringToCharPointer(strFilename);
    m_pkSequence->SetFilename(pcFilename);
    MFreeCharPointer(pcFilename);
    pkData->Unlock();

    assert(m_pkAnimation);
    m_pkAnimation->ThrowSequenceModifiedEvent(Prop_Filename, this);
}
//---------------------------------------------------------------------------
int MSequence::get_AnimIndex()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    int iAnimIndex = m_pkSequence->GetAnimIndex();
    pkData->Unlock();

    return iAnimIndex;
}
//---------------------------------------------------------------------------
void MSequence::set_AnimIndex(int iAnimIndex)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkSequence->SetAnimIndex(iAnimIndex);
    pkData->Unlock();

    assert(m_pkAnimation);
    m_pkAnimation->ThrowSequenceModifiedEvent(Prop_AnimIndex, this);
}
//---------------------------------------------------------------------------
bool MSequence::get_Loop()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    bool bLoop = (m_pkControllerSequence->GetCycleType() ==
        NiTimeController::LOOP);
    pkData->Unlock();

    return bLoop;
}
//---------------------------------------------------------------------------
float MSequence::get_Frequency()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fFrequency = m_pkControllerSequence->GetFrequency();
    pkData->Unlock();

    return fFrequency;
}
//---------------------------------------------------------------------------
#ifdef NDL_DEBUG
void MSequence::set_Frequency(float fFrequency)
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkControllerSequence->SetFrequency(fFrequency);
    pkData->Unlock();
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
float MSequence::get_BeginKeyTimeDivFreq()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fBeginKeyTime = m_pkControllerSequence->GetBeginKeyTimeDivFreq();
    pkData->Unlock();

    return fBeginKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::get_EndKeyTimeDivFreq()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fEndKeyTime = m_pkControllerSequence->GetEndKeyTimeDivFreq();
    pkData->Unlock();

    return fEndKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::get_DefaultPositionTime()
{
    return m_fDefaultPositionTime;
}
//---------------------------------------------------------------------------
void MSequence::set_DefaultPositionTime(float fTime)
{
    m_fDefaultPositionTime = fTime;
}
//---------------------------------------------------------------------------
float MSequence::get_BeginKeyTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fBeginKeyTime = m_pkControllerSequence->GetBeginKeyTime();
    pkData->Unlock();

    return fBeginKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::get_EndKeyTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fEndKeyTime = m_pkControllerSequence->GetEndKeyTime();
    pkData->Unlock();

    return fEndKeyTime;
}
//---------------------------------------------------------------------------
float MSequence::get_Length()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fLength = m_pkControllerSequence->GetLength();
    pkData->Unlock();
    return fLength;
}
//---------------------------------------------------------------------------
float MSequence::get_LastTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fLastTime = m_pkControllerSequence->GetLastTime();
    pkData->Unlock();

    return fLastTime;
}
//---------------------------------------------------------------------------
float MSequence::get_LastScaledTime()
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    float fLastScaledTime = m_pkControllerSequence->GetLastScaledTime();
    pkData->Unlock();

    return fLastScaledTime;
}
//---------------------------------------------------------------------------
MTransition* MSequence::get_Transitions()[]
{
    System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aTransitions;
}
//---------------------------------------------------------------------------
MSequence::AnimState MSequence::GetAnimState()
{
	System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
	NiControllerSequence::AnimState eState = 
		m_pkControllerSequence->GetState();
    pkData->Unlock();
	return (MSequence::AnimState)eState;
}
//---------------------------------------------------------------------
MSequence::MInterpolatorInfo* MSequence::get_InterpolatorInfo()[]
{
	System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aInterpolatorInfo;
}
//---------------------------------------------------------------------------
MSequence::MTextKey* MSequence::get_TextKeys()[]
{
	System::Diagnostics::Debug::Assert(!m_bDisposed, 
        "Cannot access a previously disposed object");
    return m_aTextKeys;
}
//---------------------------------------------------------------------------
String*	MSequence::SaveCopy(String *strNewName)
{
	m_pkAnimation->ResetAnimations();

	String *pkFullPath = m_pkAnimation->GetFullKFFilename(SequenceID);
	const char *strFullPath = MStringToCharPointer(pkFullPath);
	std::string kFilename(strFullPath);
	std::string kSeqName = (char const*)m_pkControllerSequence->GetName();

	std::string::size_type kLoc = kFilename.rfind(kSeqName);
	if(std::string::npos == kLoc)
	{
		return NULL;
	}

	std::string kNewFileName = kFilename.substr(0,kLoc);

	const char *pcNewName = MStringToCharPointer(strNewName);
	m_pkControllerSequence->SetName(pcNewName);

	kNewFileName += std::string(pcNewName);
	kNewFileName += std::string(".kf");

	MFreeCharPointer(pcNewName);
	MFreeCharPointer(strFullPath);

	NiStream	kStream;

	kStream.InsertObject(m_pkControllerSequence);
	bool bResult = kStream.Save(kNewFileName.c_str());

	m_pkControllerSequence->SetName(kSeqName.c_str());
	
	return NiExternalNew String(kNewFileName.c_str()); 
}