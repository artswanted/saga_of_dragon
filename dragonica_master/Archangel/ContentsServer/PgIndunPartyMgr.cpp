#include "StdAfx.h"
#include "PgIndunPartyMgr.h"
#include "Lohengrin/VariableContainer.h"
#include "constant.h"
#include "PgServerSetMgr.h"

//파티리스트를 구하는데 캐싱된 정보를 먼저 보내고
//일정 시간 이상 지난 정보는 리플레시 시키고
//리플레시 요청 정보가 일정 딜레이 후에도 도착하지 않으면 온 정보만 보내주도록 처리

int const DEFAULT_SENDING_DELAY_TIME = 5000;

PgIndunPartyMgr::PgIndunPartyMgr(void)
{
	m_kCasingTime = 100;
	m_kDelayTime = 1000;
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_IndunParty_List_CasingTime, m_kCasingTime);
	g_kVariableContainer.Get(EVar_Kind_Contents, EVar_IndunParty_List_DelayTime, m_kDelayTime);
}

PgIndunPartyMgr::~PgIndunPartyMgr(void)
{
}

void PgIndunPartyMgr::GetList(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, VEC_INT const& kContMapNo)
{
	BM::CAutoMutex kLock(m_kMutex);
	for(VEC_INT::const_iterator mapno_it=kContMapNo.begin(); mapno_it!=kContMapNo.end(); ++mapno_it)
	{
		CONT_INDUN_PARTY_LIST::iterator party_it = m_kContIndunPartyList.find((*mapno_it));
		if(party_it != m_kContIndunPartyList.end())
		{
			DWORD const kNowTime = BM::GetTime32();
			if(kNowTime - (*party_it).second.dwTime <= m_kCasingTime)
			{
				(*party_it).second.dwTime = kNowTime;
				Send(kCharGuid, kKeyGuid, Type, (*party_it).second.kContChannel);
				return;
			}
		}

		SendRefresh(kCharGuid, kKeyGuid, Type, (*mapno_it));
	}
}

void PgIndunPartyMgr::Send(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, CONT_INDUN_PARTY_CHANNEL const& kCont)
{
	BM::Stream kPacket(PT_M_C_ANS_INDUN_PARTY_LIST);
	kPacket.Push(kKeyGuid);
	kPacket.Push(Type);
	PU::TWriteTableArray_AMM(kPacket, kCont);
	g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
}

void PgIndunPartyMgr::Send(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, short const sChannel, int const Type, CONT_INDUN_PARTY const& kCont)
{
	CONT_INDUN_PARTY_CHANNEL kContCahnnel;
	kContCahnnel.insert(std::make_pair(sChannel, kCont));

	BM::Stream kPacket(PT_M_C_ANS_INDUN_PARTY_LIST);
	kPacket.Push(kKeyGuid);
	kPacket.Push(Type);
	PU::TWriteTableArray_AMM(kPacket, kContCahnnel);
	g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
}

void PgIndunPartyMgr::SendRefresh(BM::GUID const& kCharGuid, BM::GUID const& kKeyGuid, int const Type, int const iMapNo)
{
	BM::Stream kPacket(PT_M_T_REQ_INDUN_PARTY_LIST);
	kPacket.Push(kCharGuid);
	kPacket.Push(kKeyGuid);
	kPacket.Push(Type);
	kPacket.Push(iMapNo);
	SendToServerType(CEL::ST_CENTER, kPacket);

	SSendRefresh kValue;
	kValue.kCharGuid = kCharGuid;
	kValue.iMapNo = iMapNo;
	kValue.dwTime = BM::GetTime32();
	kValue.Type = Type;

	BM::CAutoMutex kLock(m_kMutex);
	m_kContSendRefresh.insert(std::make_pair(kKeyGuid, kValue));
}

void PgIndunPartyMgr::Update()
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SEND_REFRESH::iterator it = m_kContSendRefresh.begin();
	while(it != m_kContSendRefresh.end())
	{
		if(false==(*it).second.bSending)
		{
			if(BM::GetTime32() - (*it).second.dwTime >= m_kDelayTime)
			{
				static CONT_INDUN_PARTY_CHANNEL const NullData;
				CONT_INDUN_PARTY_CHANNEL const* pkContChannel = &NullData;

				CONT_INDUN_PARTY_LIST::const_iterator list_it = m_kContIndunPartyList.find((*it).second.iMapNo);
				if(list_it != m_kContIndunPartyList.end())
				{
					pkContChannel = &(*list_it).second.kContChannel;
				}
				Send((*it).second.kCharGuid, (*it).first, (*it).second.Type, *pkContChannel);
				it = m_kContSendRefresh.erase(it);
				continue;
			}
		}
		else
		{
			if(BM::GetTime32() - (*it).second.dwTime >= DEFAULT_SENDING_DELAY_TIME)
			{
				it = m_kContSendRefresh.erase(it);
				continue;
			}
		}
		++it;
	}
}

void PgIndunPartyMgr::SyncIndunParty(BM::Stream * pkPacket)
{
	if(!pkPacket)
	{
		return;
	}

	BM::GUID kCharGuid;
	BM::GUID kKeyGuid;
	int Type = 0;
	short sChannel = 0;
	int iMapNo = 0;
	CONT_INDUN_PARTY kCont;
	pkPacket->Pop(kCharGuid);
	pkPacket->Pop(kKeyGuid);
	pkPacket->Pop(Type);
	pkPacket->Pop(sChannel);
	pkPacket->Pop(iMapNo);
	PU::TLoadArray_M(*pkPacket, kCont);
	

	BM::CAutoMutex kLock(m_kMutex);
	auto kPair = m_kContIndunPartyList.insert(std::make_pair(iMapNo,SPartyInfo()));
	CONT_INDUN_PARTY_LIST::mapped_type & kValue = kPair.first->second;

	auto kPair2 = kValue.kContChannel.insert(std::make_pair(sChannel, kCont));
	if(false==kPair2.second)
	{
		kPair2.first->second.swap(kCont);
	}

	CONT_SEND_REFRESH::iterator refresh_it = m_kContSendRefresh.find(kKeyGuid);
	if(refresh_it != m_kContSendRefresh.end())
	{
		(*refresh_it).second.bSending = true;
		Send(kCharGuid, kKeyGuid, sChannel, Type, kPair2.first->second);
	}
}