#include "stdafx.h"
#include "PgCharacterCardMgr.h"
#include "JobDispatcher.h"

void PgCharacterCardMgr::InsertSearchKey(PgCharacterCard const & kCard)
{
	int const iAge = kCard.Year()/10;
	m_kContSearchMgr[static_cast<EUnitGender>(kCard.Sex())].kCont[iAge].kCont[kCard.Local()].kCont.insert(kCard.OwnerGuid());
}

void PgCharacterCardMgr::RemoveSearchKey(PgCharacterCard const & kCard)
{
	int const iAge = kCard.Year()/10;
	m_kContSearchMgr[static_cast<EUnitGender>(kCard.Sex())].kCont[iAge].kCont[kCard.Local()].kCont.erase(kCard.OwnerGuid());
}

void PgCharacterCardMgr::FindCard(BM::GUID const & kReqGuid,EUnitGender const kSex,int const iAge,int const iLocal,CONT_MATCH_CARD_INFO & kContCard)
{
	PgCharacterCard kReqCard;
	if(false == GetCard(kReqGuid,kReqCard))
	{
		return;
	}

	CONT_CARDID kContCardId;
	SAGE_NODE & kAge = m_kContSearchMgr[kSex].kCont[iAge];
	if(0 < iLocal)
	{
		kContCardId = kAge.kCont[iLocal].kCont;
	}
	else
	{
		for(CONT_LOCAL::const_iterator iter = kAge.kCont.begin();iter != kAge.kCont.end();++iter)
		{
			kContCardId.insert((*iter).second.kCont.begin(),(*iter).second.kCont.end());
		}
	}

	for(CONT_CARDID::const_iterator iter = kContCardId.begin();iter != kContCardId.end();++iter)
	{
		if((*iter) == kReqGuid)
		{
			continue;
		}

		if(MAX_MATCH_CARD_VIEW_NUM <= kContCard.size())
		{
			return;
		}

		PgCharacterCard kCard;
		if(false == GetCard((*iter),kCard))
		{
			continue;
		}

		if(false == kCard.Enable())
		{
			continue;
		}

		int const iMatchPoint = static_cast<int>((50 - (kReqCard.Year() - kCard.Year())) + (((kReqCard.PopularPoint() * 5)/100) * ((kCard.PopularPoint() * 2)/100)));

		kContCard.push_back(SMATCH_CARD_INFO(kCard.OwnerGuid(),kCard.Name(),kCard.Sex(),kCard.Year(),iMatchPoint));
	}
}

bool const PgCharacterCardMgr::Locked_RegCard(PgCharacterCard const & kCard)
{
	BM::CAutoMutex kLock(m_kMutex);
	if(false == m_kContCard.insert(std::make_pair(kCard.OwnerGuid(),kCard)).second)
	{
		return false;
	}

	InsertSearchKey(kCard);
	return true;
}

bool const PgCharacterCardMgr::Locked_UnRegCard(BM::GUID const & kCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_CHARACTER_CARD::iterator iter = m_kContCard.find(kCharGuid);
	if(iter == m_kContCard.end())
	{
		return false;
	}

	RemoveSearchKey((*iter).second);

	m_kContCard.erase(iter);
	return true;
}

bool const PgCharacterCardMgr::GetCard(BM::GUID const & kCharGuid,PgCharacterCard & kCard)
{
	CONT_CHARACTER_CARD::iterator iter = m_kContCard.find(kCharGuid);
	if(iter == m_kContCard.end())
	{
		return false;
	}
	kCard = (*iter).second;
	return true;
}

bool const PgCharacterCardMgr::Locked_GetCard(BM::GUID const & kCharGuid,PgCharacterCard & kCard)
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetCard(kCharGuid,kCard);
}


void PgCharacterCardMgr::Locked_RecvPT_M_I_REQ_CHARACTER_CARD_INFO(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID	kOwnerGuid,
				kTargetGuid;

	pkPacket->Pop(kOwnerGuid);
	pkPacket->Pop(kTargetGuid);
	PgCharacterCard kCard;

	BM::Stream kPacket(PT_M_C_ANS_CHARACTER_CARD_INFO);
	kPacket.Push(kTargetGuid);

	if(false == GetCard(kTargetGuid,kCard))
	{
		kPacket.Push(static_cast<HRESULT>(E_NOT_FOUND_CARD));
		SendToUser(kOwnerGuid,kPacket,false);
	}

	if(false == kCard.Enable() && (kOwnerGuid != kTargetGuid))
	{
		kPacket.Push(static_cast<HRESULT>(E_DISABLE_CARD));
		SendToUser(kOwnerGuid,kPacket,false);
		return;
	}

	kPacket.Push(static_cast<HRESULT>(S_OK));
	kCard.WriteToPacket(kPacket);
	SendToUser(kOwnerGuid,kPacket,false);
}


void PgCharacterCardMgr::Locked_RecvPT_M_I_REQ_SEARCH_MATCH_CARD(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::GUID kGuid;
	BYTE kSex;
	BYTE kAge;
	int iLocal;

	pkPacket->Pop(kGuid);
	pkPacket->Pop(kSex);
	pkPacket->Pop(kAge);
	pkPacket->Pop(iLocal);

	CONT_MATCH_CARD_INFO kContCard;
	FindCard(kGuid,static_cast<EUnitGender>(kSex),static_cast<int>(kAge),iLocal,kContCard);

	BM::Stream kPacket(PT_M_C_ANS_SEARCH_MATCH_CARD);
	PU::TWriteArray_M(kPacket,kContCard);
	SendToUser(kGuid,kPacket,false);
}

void PgCharacterCardMgr::SendToUser(BM::GUID const &kGuidKey, BM::Stream const &rkPacket, bool const IsMemberGuid)
{
	if (kGuidKey == BM::GUID::NullData())
	{
		return;
	}

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kGuidKey);
	pkActionOrder->kCause = CNE_CONTENTS_SENDTOUSER;
	
	ContentsActionEvent_SendPacket kEvent(ECEvent_SendToUser);
	kEvent.MemberGuid(IsMemberGuid);
	kEvent.Guid(kGuidKey);
	kEvent.SendType(ContentsActionEvent_SendPacket::E_SendUser_ToOneUser);
	kEvent.m_kPacket.Push(rkPacket);

	SPMO kOrder(IMET_CONTENTS_EVENT, kGuidKey, kEvent);
	pkActionOrder->kContOrder.push_back(kOrder);
	g_kJobDispatcher.VPush(pkActionOrder);
}

bool const PgCharacterCardMgr::Locked_SetCard(PgCharacterCard const & kCard)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CHARACTER_CARD::iterator iter = m_kContCard.find(kCard.OwnerGuid());
	if(iter == m_kContCard.end())
	{
		return false;
	}

	RemoveSearchKey((*iter).second);

	(*iter).second = kCard;

	InsertSearchKey(kCard);
	return true;
}
