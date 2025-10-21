#include "StdAfx.h"
#include "PgNoticeAction.h"

PgNoticeAction::PgNoticeAction(void)
{
}

PgNoticeAction::~PgNoticeAction(void)
{
}


void PgNoticeAction::SetNoticeData(int iReceiver, unsigned short usRealm, unsigned short usChannel, 
	unsigned short usGround, std::wstring const &rkContents)
{
	m_iReceiver = iReceiver;
	m_usRealm = usRealm;
	m_usChannel = usChannel;
	m_usGround = usGround;
	m_wstrText = rkContents;
}

bool PgNoticeAction::Send(int iServerType)
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kPacket(PT_A_SEND_NOTICE);
	kPacket.Push(m_iReceiver);
	kPacket.Push(m_usRealm);
	kPacket.Push(m_usChannel);
	kPacket.Push(m_usGround);
	kPacket.Push(m_wstrText);

	CONT_SERVER_HASH kServerInfo;
	g_kProcessCfg.Locked_GetServerInfo(CEL::ST_NONE, kServerInfo);
	CONT_SERVER_HASH::iterator Itr = kServerInfo.begin();
	while(Itr != kServerInfo.end())
	{
		CONT_SERVER_HASH::mapped_type kElement = Itr->second;
		CONT_SERVER_HASH::key_type kKey = Itr->first;
		switch(iServerType)
		{
		case CEL::ST_IMMIGRATION:
				if(kElement.nServerType == iServerType)
				{
					return g_kProcessCfg.Locked_SendToServer(kKey, kPacket);
				}break;
		case CEL::ST_GMSERVER:
			{
				if(kElement.nServerType == CEL::ST_GMSERVER)
					return g_kProcessCfg.Locked_SendToServer(kKey, kPacket);
			}break;
		case CEL::ST_CONTENTS:
			{
				if(m_usRealm == kElement.nRealm)
				{
					return g_kProcessCfg.Locked_SendToServer(kKey, kPacket);
				}
			}break;
		default:break;
		}
		++Itr;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgNoticeAction::Recv(BM::Stream *pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	pkPacket->Pop(m_iReceiver);
	pkPacket->Pop(m_usRealm);
	pkPacket->Pop(m_usChannel);
	pkPacket->Pop(m_usGround);
	pkPacket->Pop(m_wstrText);

	CONT_SERVER_HASH kServerInfo;
	SERVER_IDENTITY kSI = g_kProcessCfg.ServerIdentity();
	if(m_iReceiver == ERT_USER)
	{
		switch(kSI.nServerType)
		{
		case CEL::ST_LOGIN:
			{
				Send(CEL::ST_IMMIGRATION);
			}break;
		case CEL::ST_IMMIGRATION:
			{
				BM::Stream kPacket(PT_T_C_NFY_NOTICE);
				kPacket.Push(m_iReceiver);
				kPacket.Push(m_usRealm);
				kPacket.Push(m_usChannel);
				kPacket.Push(m_usGround);
				kPacket.Push(m_wstrText);
				g_kProcessCfg.Locked_SendToContentsServer(m_usRealm, kPacket);
			}break;
/*
		case CEL::ST_CONTENTS:
			{

			}break;
		case CEL::ST_CENTER:
			{
				BM::Stream kPacket(PT_T_C_NFY_NOTICE);
				kPacket.Push(m_wstrText);
				// Contents Server에게도 보내자
//				SendToContents(kSI, kPacket);
				// 모든 맵서버로 패킷을 보내자.
				g_kProcessCfg.Locked_SendToServerType(CEL::ST_MAP, kPacket);
//				INFO_LOG(BM::LOG_LV7, _T("Notice Send to MapServer : %s"), m_wstrText.c_str());
			}break;
*/
		default: 
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}break;
		}
	}
	else if(m_iReceiver == ERT_GM)
	{
		switch(kSI.nServerType)
		{
		case CEL::ST_GMSERVER:
			{
				//GM서버가 목적지!!! 나머지는 GM서버 내에서 처리하자
			}break;
		case CEL::ST_IMMIGRATION:
			{
				Send(CEL::ST_GMSERVER);
			}break;
		case CEL::ST_CONTENTS:
			{
				Send(CEL::ST_IMMIGRATION);
			}break;
		default: 
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}break;
		}
	}
	return true;
}