#include "stdafx.h"
#include "PgRecvFromLogin.h"
#include "PgPetitionDataMgr.h"
#include "PgGmTask.h"

void CALLBACK OnConnectToLogin(CEL::CSession_Base *pkSession)
{	//!  세션락
	const bool bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{	// 이쯤에서 
		return;
	}
	else
	{
		g_kProcessCfg.Locked_ConnectLogin();
		return;
	}
}

void CALLBACK OnDisConnectToLogin(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV7, __FL__ << _T("Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	g_kProcessCfg.Locked_ConnectLogin();
}

void CALLBACK OnRecvFromLogin(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	unsigned short usType = 0;
	pkPacket->Pop(usType);

	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{//	
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("PT_A_S_ANS_GREETING [ServerIP: ") << pkSession->Addr().IP().c_str() << _T("]") );

			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))//이 세션이 Login 의 세션
			{
				if(SetSendWrapper(kRecvSI))
				{
					const SERVER_IDENTITY &kSI = g_kProcessCfg.ServerIdentity();
					TBL_SERVERLIST kServerInfo;
					if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kServerInfo))
					{
//						g_kPetitionDataMgr.ReflashAllPetitionData();
						return;
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Login Server No Acceptor info") );
						return;
					}
				}
			}
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
		}break;
	case PT_O_G_REQ_GMCOMMAND:
		{
			PgGMTask::RecvGMToolProcess(pkSession, pkPacket);
		}break;
	case PT_O_N_REQ_CHECK_LOGINED_USER:
		{
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	case PT_O_C_REQ_SEND_GM_CHAT:
		{
			unsigned short usRealmNo = 0;
			std::wstring kPlayerName;
			std::wstring kGmName;
			std::wstring kText;
			BM::GUID kGmGuid;
			BM::GUID kPetitionGuid;

			pkPacket->Pop(usRealmNo);
			pkPacket->Pop(kPlayerName);
			pkPacket->Pop(kGmGuid);
			pkPacket->Pop(kPetitionGuid);
			pkPacket->Pop(kGmName);
			pkPacket->Pop(kText);
			
			//일단 채팅 로그 남기는건 나중에
			BM::Stream kPacket(usType);
			kPacket.Push(usRealmNo);
			kPacket.Push(kPlayerName);
			kPacket.Push(kGmGuid);
			kPacket.Push(kPetitionGuid);
			kPacket.Push(kGmName);
			kPacket.Push(kText);
			SendToImmigration(kPacket);
			
		}break;
	case PT_O_C_REQ_GM_CHAT_END:
		{
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unknown Packet Type[") << usType << _T("]") );
		}break;
	}
}
