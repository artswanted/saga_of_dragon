#include "stdafx.h"
#include "PgRecvFromImmigration.h"
#include "PgRecvFromLogin.h"
//#include "PgRecvFromUser.h"
//#include "PgRecvFromCenter.h"
#include "PgPetitionDataMgr.h"
#include "Variant/PgMctrl.h"
#include "Lohengrin/PgRealmManager.h"
#include "PgGmTask.h"


void CALLBACK OnConnectToImmigration(CEL::CSession_Base *pkSession)
{	//!  세션락
	const bool bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{	// 이쯤에서 
		return;
	}
	else
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Failed Connect to Immigration Server...RETRY...Address[") << pkSession->Addr().IP().c_str() << _T(":") << pkSession->Addr().wPort << _T("]") );
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectImmigration();
		return;
	}
}

void CALLBACK OnDisConnectToImmigration(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV7, __FL__ << _T("Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	g_kProcessCfg.Locked_ConnectImmigration();
}

void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
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
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("PT_A_S_ANS_GREETING") );

			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			if(SUCCEEDED(g_kProcessCfg.Locked_Read_ServerList(*pkPacket)))
			{
				g_kRealmMgr.ReadFromPacket(*pkPacket);
				
				//로그인 서버들에게 접속시도!!
				if(S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))//이 세션이 IMMIGRATION 의 세션
				{
					if(SetSendWrapper(kRecvSI))
					{
//						g_kRealmMgr.ReadFromPacket(*pkPacket);//로그인서버는 처음에 렐름 정보 받아놔야함.

						const SERVER_IDENTITY &kSI = g_kProcessCfg.ServerIdentity();
						TBL_SERVERLIST kServerInfo;
						if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kServerInfo))
						{
							g_kProcessCfg.Locked_ConnectLogin();
							g_kProcessCfg.Locked_ConnectLog();
							ReadyToService((CEL::eSessionType)kRecvSI.nServerType);
							return;
						}
						else
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Immigration Server No Acceptor info") );
							return;
						}
					}
				}
			}
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
		}break;
	case PT_A_GM_ANS_GMORDER:
		{
			BM::GUID kOrderCmd;
			unsigned short usState;
			pkPacket->Pop(kOrderCmd);
			pkPacket->Pop(usState);
			g_kGMTask.Locked_SendOrderState(kOrderCmd, static_cast<EOrderState>(usState));
		}break;
	case PT_A_GM_ANS_FAILED_GMORDER:
		{
			BM::GUID kOrderCmd;
			int iErrorCode;
			pkPacket->Pop(kOrderCmd);
			pkPacket->Pop(iErrorCode);
			g_kGMTask.Locked_FailedOrder(kOrderCmd, static_cast<E_GM_ERR>(iErrorCode));
		}break;
	case PT_N_O_ANS_CHECK_LOGINED_USER:
		{
			BM::GUID kGMGUid;
			pkPacket->Pop(kGMGUid);

			BM::Stream kPacket(usType);
			kPacket.Push(kGMGUid);
			kPacket.Push(*pkPacket);			
			g_kGMUserMgr.SendToLogin(kGMGUid, kPacket);
		}break;
	case PT_C_O_ANS_SEND_GM_CHAT:
		{
			BM::GUID kGMGUid;
			BM::GUID kPetitionGuid;
			std::wstring kSender;
			std::wstring kText;

			pkPacket->Pop(kGMGUid);
			pkPacket->Pop(kPetitionGuid);
			pkPacket->Pop(kSender);
			pkPacket->Pop(kText);

			//채팅 로그 저장하는거 추가해야함.

			BM::Stream kPacket(usType);
			kPacket.Push(kGMGUid);
			kPacket.Push(kPetitionGuid);
			kPacket.Push(kSender);
			kPacket.Push(kText);			
			g_kGMUserMgr.SendToLogin(kGMGUid, kPacket);
		}break;
	case PT_A_GM_ANS_GMCOMMAND:
		{
			g_kGMTask.RecvGMLocaleProcess(pkPacket);
		}break;
	case PT_C_GM_REQ_RECEIPT_PETITION:
		{
			g_kPetitionDataMgr.ReceiptPetition(pkPacket);
		}break;
	case PT_C_GM_REQ_REMAINDER_PETITION:
		{
			if(g_kPetitionDataMgr.bCheckReceip())
			{
				//우선 아직 처리되지 않은 진정이 있는지 확인한다.
				g_kPetitionDataMgr.Select_PetitionState(pkPacket);
			}
			else
			{
				unsigned short usRealm = 0;
				unsigned short usChannel = 0;
				BM::GUID kMemberGuid;

				pkPacket->Pop(usRealm);
				pkPacket->Pop(usChannel);
				pkPacket->Pop(kMemberGuid);
				g_kPetitionDataMgr.RemainderPetition(usRealm, kMemberGuid);
			}	
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unknown Packet Type[") << usType << _T("]") );
		}break;
	}
}