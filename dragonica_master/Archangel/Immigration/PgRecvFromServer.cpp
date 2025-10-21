#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Lohengrin/Packettype2.h"
#include "HellDart/PgIPChecker.h"
#include "FCS/AntiHack.h"
#include "Variant/PgMCtrl.h"
#include "Variant/gm_const.h"
#include "Variant/PgNoticeAction.h"
#include "PgWaitingLobby.h"
#include "PgRecvFromLog.h"
#include "PgSiteMgr.h"
#include "RecvFromConsent.h"
#include "ImmTask.h"

extern PgIPChecker g_kIPChecker;

extern void Recv_PT_A_S_REQ_GREETING(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
extern bool OnRecvFromMMC(CEL::CSession_Base * const pkSession, WORD const wkType, BM::Stream * const pkPacket);
//extern void CALLBACK OnRecvFromServer3( CEL::CSession_Base *pkSession, unsigned short usType, BM::Stream * const pkPacket );

void SendMCC_NfyServerConnect(SERVER_IDENTITY const &kSI, bool const bIsReadyToService)
{
	BM::Stream kMMCPacket(PT_IMM_MCC_NFY, IMCT_STATE_NFY);
	kMMCPacket.Push(bIsReadyToService);
	kSI.WriteToPacket(kMMCPacket);//이 서버 살았어. Disconnect 가 먼저 들어오면 으쨔??
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_MACHINE_CONTROL, kMMCPacket);
}

void CALLBACK OnAcceptFromServer( CEL::CSession_Base *pkSession )
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Send EncryptKey") );

		BM::Stream kPacket(PT_A_ENCRYPT_KEY);
		kPacket.Push(kEncodeKey);

		pkSession->VSend(kPacket, false);
	}
	else
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Get EncryptKey Failed. Session Terminate!") );
		pkSession->VTerminate();
	}
}

void CALLBACK OnConnectToServer( CEL::CSession_Base *pkSession )
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if( bIsSucc )
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Other Server Connected Address[") << pkSession->Addr().ToString().c_str() << _T("]") );
		return;
	}
	else
	{
		INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Other Server Connected Failed") );
	}
}

void CALLBACK OnDisConnectToServer( CEL::CSession_Base *pkSession )
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Server Connection Close") );
	
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);

	if(pkSession->m_kSessionData.Size())
	{
		SERVER_IDENTITY kRecvSI;
		kRecvSI.ReadFromPacket(pkSession->m_kSessionData);

//		SendMCC_NfyServerConnect(kRecvSI, false);

		g_kWaitingLobby.DisconnectServer(kRecvSI);
	}
}

bool CALLBACK OnRecvFromLogin(unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	switch(usType)
	{
	case PT_L_IM_TRY_AUTH:
		{
			SServerTryLogin kSTL;
			std::wstring kAddonString;
			if(	true == kSTL.ReadFromPacket(*pkPacket) 
			&&	true == pkPacket->Pop(kAddonString))
			{
				SAuthInfo kAuthInfo( kSTL );
				HRESULT const hRet = g_kWaitingLobby.ProcessTryAuth( kAuthInfo, false, kAddonString );
				INFO_LOG(BM::LOG_LV7, __F_P2__(PT_L_IM_TRY_AUTH) << L"[g_kWaitingLobby.ProcessTryAuth] Result["<<hRet<<L"] Addr["<<kAuthInfo.addrRemote.ToString()<<L"] ID["<<kAuthInfo.ID()<<L"]");
			}
		}break;
	case PT_L_IM_TRY_AUTH_AUTO:
		{
			SAuthInfo kAutoInfo;
			if( true == kAutoInfo.ReadFromPacket(*pkPacket) )
			{
				HRESULT const hRet = g_kWaitingLobby.ProcessTryAuth( kAutoInfo, true);
				INFO_LOG(BM::LOG_LV7, __F_P2__(PT_L_IM_TRY_AUTH_AUTO) << L"[g_kWaitingLobby.ProcessTryAuth] Result["<<hRet<<L"] Addr["<<kAutoInfo.addrRemote.ToString()<<L"] ID["<<kAutoInfo.ID()<<L"]");
			}
		}break;
	case PT_L_IM_TRY_LOGIN:
		{	
			SAuthInfo kAuthInfo;
			if( true == kAuthInfo.ReadFromPacket(*pkPacket) )
			{
				g_kWaitingLobby.ProcessTryLogin( kAuthInfo, false );
			}
		}break;
	case PT_L_IM_WAITING_USER_CLEAR:
		{
			CEL::SESSION_KEY kWaiterKey;
			BM::GUID kMemberGuid;
			ELoginState eLoginState;
			pkPacket->Pop(kWaiterKey);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(eLoginState);
			
			g_kWaitingLobby.ProcessClearAuthWaiting(kWaiterKey, kMemberGuid, eLoginState);
		}break;
	case PT_L_IM_ANS_RESERVEMEMBER_LOGIN:
		{
			SERVER_IDENTITY kContentsServer;
			pkPacket->Pop(kContentsServer);

			BM::Stream kPacket( PT_IM_N_ANS_RESERVEMEMBER_LOGIN );
			kPacket.Push( *pkPacket );
			g_kProcessCfg.Locked_SendToServer( kContentsServer, kPacket );
		}break;
	default:
		{
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			return false;
		}break;
	}
	return true;
}

bool CALLBACK OnRecvFromContents(unsigned short usType, CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	switch(usType)
	{
	case PT_A_A_WRAPPER:
		{
			SERVER_IDENTITY kSI;
			pkPacket->Pop(kSI);
			BM::Stream kNewPacket;
			kNewPacket.Push(*pkPacket);
			if (kSI.nServerNo == 0)
			{
				SendToServerType((CEL::E_SESSION_TYPE)kSI.nServerType, kNewPacket);
			}
			else
			{
				SendToServer(kSI, kNewPacket);
			}
		}break;
	case PT_A_A_WRAPPER_CHANNEL_EXCEPTME:
		{
			// kSI.nServerNo == 0 라면
			//	kSI.nChannel/kSI.nServerType의 모든 서버로 전송한다.
			//	자기자신에게는 다시 보내지 않는다.
			// kSI.nServerNo != 0 라면
			//  kSI 서버로 전달한다.
			SERVER_IDENTITY kSI;
			pkPacket->Pop(kSI);
			BM::Stream kNewPacket;
			kNewPacket.Push(*pkPacket);
			if (kSI.nServerNo == 0)
			{
//				SendToChannelServerType((CEL::E_SESSION_TYPE)kSI.nServerType, kSI.nChannel, kNewPacket);
			}
			else
			{
				SendToServer(kSI, kNewPacket);
			}
		}break;
	case PT_T_IM_REQ_SWITCH_USER_ADD:
		{//스위치 결과
			SSwitchPlayerData kSPD;
			kSPD.ReadFromPacket(*pkPacket);
			g_kWaitingLobby.Recv_PT_T_IM_REQ_SWITCH_USER_ADD(kSPD, pkSession);
		}break;
	case PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED:
		{// 스위치 할당을 실패했다.
			ETryLoginResult eRet = E_TLR_NONE;
			BM::GUID kMemberGuid;
			pkPacket->Pop(eRet);
			pkPacket->Pop(kMemberGuid);
			g_kWaitingLobby.Recv_PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED( kMemberGuid, eRet );
		}break;
	case PT_N_A_NFY_CHANNEL_ALIVE:
		{
			SERVER_IDENTITY  kRecvSI;
			bool bAlive;
			size_t iRdPos = pkPacket->RdPos();
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAlive);

			if ( true == bAlive )
			{
				INFO_LOG( BM::LOG_LV6, _T("* Realm<") << kRecvSI.nRealm << _T("> Channel<") << kRecvSI.nChannel << _T("> Alive!!") );
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, _T("* Realm<") << kRecvSI.nRealm << _T("> Channel<") << kRecvSI.nChannel << _T("> Dead!!") );
			}

			g_kRealmMgr.AliveChannel(kRecvSI.nRealm, kRecvSI.nChannel, bAlive);
	
			pkPacket->RdPos(iRdPos);
			BM::Stream kHPacket(usType);
			kHPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_LOGIN, kHPacket);
		}break;
	case PT_A_A_NFY_REALM_STATE:
		{
			size_t iRdPos = pkPacket->RdPos();
			SERVER_IDENTITY kRecvSI;
			short sRealmState;
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(sRealmState);
			g_kRealmMgr.SetRealmState(kRecvSI.nRealm, sRealmState);

			pkPacket->RdPos(iRdPos);
			BM::Stream kBPacket(PT_A_A_NFY_REALM_STATE);
			kBPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_LOGIN, kBPacket);
		}break;
		// send to ConsentServer
	case PT_N_X_REQ_CASH:
		{
			BM::Stream kCPacket(usType);
			kCPacket.Push(*pkPacket);
			if (false == SendToConsentServer(kCPacket))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("[PT_N_X_REQ_CASH] Cannot send packet to ConsentServer"));
			}
		}break;
	case PT_M_I_GMCMD_PLAYERPLAYTIMESTEP:
		{
			int iStep = 0;
			pkPacket->Pop(iStep);
			if(0 == iStep)
			{
				g_kWaitingLobby.ProcessResetPlayTime();
			}
			else if(1 == iStep)
			{
				CEL::DB_QUERY kQuery(DT_MEMBER, DQT_LOAD_DEF_PLAYERPLAYTIME, _T("EXEC [dbo].[up_LoadDefPlayerPlayTime]"));
				kQuery.contUserData.Push(true);
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case PT_M_I_GMCMD_SETPLAYERPLAYTIME:
		{
			std::wstring kID;
			int iAccConSec = 0;
			int iAccDicSec = 0;
			pkPacket->Pop(kID);
			pkPacket->Pop(iAccConSec);
			pkPacket->Pop(iAccDicSec);

			g_kWaitingLobby.ProcessSetPlayTime(kID,iAccConSec,iAccDicSec);
		}break;
	default:
		{
			//CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			return false;
		}break;
	}
	return true;
}

void CALLBACK OnRecvFromServer(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	static int i = 0;
	
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);
	//std::cout<< "Received Packet Type(" << usType <<") Size["<< pkPacket->Size() << "]"<< std::endl;
	INFO_LOG( BM::LOG_LV9, __FL__ << _T("OnRecvFromServer Start PacketType[") << usType << _T("], Size[") << pkPacket->Size() << _T("]") );
	int iLoginServerNo = -1;
	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_REQ_GREETING:
		{
			Recv_PT_A_S_REQ_GREETING(pkSession, pkPacket);
		}break;
	case PT_T_IM_NFY_USER_LOGOUT:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
			g_kWaitingLobby.ProcessPlayTime(kMemberGuid, pkPacket);
		}break;
 	case PT_A_NFY_USER_DISCONNECT:
 		{
			size_t iRdPos = pkPacket->RdPos();
 			BM::GUID kMemberGuid;
 			BM::GUID kCharacterGuid;
			SERVER_IDENTITY kSI;
			bool bAutoLogin = false;
 			pkPacket->Pop(kMemberGuid);
 			pkPacket->Pop(kCharacterGuid);
			kSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAutoLogin);

 			short const nRealmNo = g_kWaitingLobby.ProcessLogout(kMemberGuid, false, bAutoLogin);
			if ( nRealmNo != kSI.nRealm )
			{
				INFO_LOG( BM::LOG_LV0, __FL__ << _T("[PT_A_NFY_USER_DISCONNECT] Disconnect RealmInfo Different[") << kSI.nRealm << _T("-") << nRealmNo << _T("] MemberGUID[") << kMemberGuid.str().c_str() << _T("]") );
				::SendDisConnectUser( CDC_Normal, kSI, kMemberGuid );
			}

			if ( true == bAutoLogin )
			{
				BM::Stream kLPacket( PT_IM_L_REQ_RESERVEMEMBER_LOGIN );
				kSI.WriteToPacket(kLPacket);
				kLPacket.Push( kMemberGuid );
				kLPacket.Push( *pkPacket );
				g_kProcessCfg.Locked_SendToServerType( CEL::ST_LOGIN, kLPacket, true );
			}
 		}break;
	case PT_T_IM_NFY_RESERVED_SWITCH_INFO://센터가 로그인으로 주려고
		{//그냥 전달만 하고. 진짜 들어왔을때 waitingLobby 를 갱신 해주도록. contSwitch -> Logined 로
			SSwitchReserveResult kSRR;
			kSRR.ReadFromPacket(*pkPacket);
	
			BM::Stream kPacket(PT_IM_L_NFY_RESERVED_SWITCH_INFO);
			kSRR.WriteToPacket(kPacket);

			SendToServer(kSRR.kLoginServer, kPacket);
		}break;
	case PT_T_IM_NFY_USERLIST:
		{
			SERVER_IDENTITY  kRealmSI;
			VEC_SwitchPlayerData kPlayerVector;
			pkPacket->Pop( kRealmSI );
			PU::TLoadArray_M(*pkPacket, kPlayerVector);

			g_kWaitingLobby.RecvPT_T_IM_NFY_USERLIST( kRealmSI, kPlayerVector );

			INFO_LOG( BM::LOG_LV6, _T("* Realm<") << kRealmSI.nRealm << _T("> Alive!!") );

			// Realm이 살아났다.
			g_kRealmMgr.AliveChannel( kRealmSI.nRealm, -1, true);
			kRealmSI.nChannel = -1;	// Realm의 상태라는 뜻.
			BM::Stream kBPacket(PT_N_A_NFY_CHANNEL_ALIVE);
			kRealmSI.WriteToPacket(kBPacket);
			kBPacket.Push(static_cast<bool>(true));
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_LOGIN, kBPacket);	// 모든 LoginServer 로
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kBPacket);	// 모든 ContentsServer 로

			/*
			size_t const iDeadRealmCount = g_kRealmMgr.GetDeadRealmCount();
			SEventMessage kMsg(EIMM_OBJ_CONSENT, PgConsentControl::ECONSENT_2ND_UPDATECHECKTIME);
			bool const bEnd = ( 0 == iDeadRealmCount );
			kMsg.Push(bEnd);
			g_kImmTask.PutMsg(kMsg);
			*/
		}break;
	case PT_O_N_REQ_CHECK_LOGINED_USER:
	case PT_O_C_REQ_SEND_GM_CHAT:
	case PT_O_C_REQ_GM_CHAT_END:
		{
			unsigned short usRealmNo = 0;
			pkPacket->Pop(usRealmNo);
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			g_kProcessCfg.Locked_SendToContentsServer(usRealmNo, kPacket);			
		}break;
	case PT_GM_A_REQ_GMCOMMAND:
	case PT_GM_A_REQ_GMORDER:
	case PT_A_GM_ANS_GMORDER:
	case PT_A_GM_ANS_FAILED_GMORDER:
	case PT_A_GM_ANS_GMCOMMAND:
	case PT_N_O_ANS_CHECK_LOGINED_USER:
	case PT_C_O_ANS_SEND_GM_CHAT:
	case PT_C_GM_REQ_RECEIPT_PETITION:
	case PT_C_GM_REQ_REMAINDER_PETITION:
		{
			SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_PACKET);
			kMsg.Push(usType);
			kMsg.Push(*pkPacket);
			g_kImmTask.PutMsg(kMsg);
		}break;
	case PT_GM_C_ANS_WAIT_RECEIPT:
	case PT_GM_C_ANS_REMAINDER_PETITION:
	case PT_GM_C_ANS_RECEIPT_PETITION:
		{
			unsigned short usRealmNo = 0;
			pkPacket->Pop(usRealmNo);
			//Contents서버로 보낸다
			BM::Stream kPacket(usType);
			kPacket.Push(*pkPacket);
			g_kProcessCfg.Locked_SendToContentsServer(usRealmNo, kPacket);
		}break;
	case PT_MCTRL_A_MMC_ANS_SERVER_COMMAND:
		{
			BM::Stream kPacket(PT_MCTRL_A_MMC_ANS_SERVER_COMMAND);
			kPacket.Push(*pkPacket);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_MACHINE_CONTROL, *pkPacket);
		}break;
	case PT_A_SEND_NOTICE:
		{
			PgNoticeAction kNotice;
			kNotice.Recv(pkPacket);
		}break;
	case PT_A_IM_WRAPPED_PACKET:
		{
			OnRecvFromServer( pkSession, pkPacket );
		}break;
	case PT_A_A_SERVER_SHUTDOWN:
		{
			SERVER_IDENTITY kSI;
			size_t iRdPos = pkPacket->RdPos();
			kSI.ReadFromPacket(*pkPacket);
			g_kWaitingLobby.DisconnectServer(kSI);

//			if ( g_kWaitingLobby.DisconnectServer(kSI) )
//			{
// 				pkPacket->RdPos(iRdPos);
// 				BM::Stream kFPacket(usType);
// 				kFPacket.Push(*pkPacket);
// 				SendToContentsServer(kSI.nRealm, kFPacket);
//			}
		}break;
	default:
		{
			if(OnRecvFromLogin(usType, pkSession, pkPacket)){return;}
			if(OnRecvFromContents(usType, pkSession, pkPacket)){return;}
			if(OnRecvFromMMC(pkSession, usType, pkPacket)){return;}
			if(OnRecvFromConsent(pkSession, usType, pkPacket)){return;}
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}break;
	}
	
	INFO_LOG( BM::LOG_LV9, __FL__ << _T("OnRecvFromServer End PacketType[") << usType << _T(", Size[") << pkPacket->Size() << _T("]") );
}

void Recv_PT_A_S_REQ_GREETING(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
 	SERVER_IDENTITY kRecvSI;
	kRecvSI.ReadFromPacket(*pkPacket);
	
	std::wstring strVersion;
	pkPacket->Pop(strVersion);

	if(kRecvSI.nServerType != CEL::ST_MACHINE_CONTROL
		&&	!CProcessConfig::IsCorrectVersion( strVersion, false ) )
	{
		//VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] Incorrect Version[%s]"), __FUNCTIONW__, __LINE__, strVersion.c_str());
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Incorrect Version[") << strVersion.c_str() << _T("]") );
		pkSession->VTerminate();
		return;
	}

	if(S_OK != g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession))
	{
		//VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s]-[%d] Add Server Session Failed Realm[%d] Chn[%d] No[%d] Type[%d] "), __FUNCTIONW__, __LINE__, kRecvSI.nRealm, kRecvSI.nChannel, kRecvSI.nServerNo, kRecvSI.nServerType);
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
		pkSession->VTerminate();//세션 끊음.
		return;
	}

	kRecvSI.WriteToPacket(pkSession->m_kSessionData);//이 세션의 SERVER_IDENTITY 기록

	BM::Stream kSendPacket(PT_A_S_ANS_GREETING);

	SERVER_IDENTITY const &kSendSI = g_kProcessCfg.ServerIdentity();
	//모든 서버에 보낼 내용
	//나의 서버 구분자.
	//전체 서버 리스트.
	kSendSI.WriteToPacket(kSendPacket);
	g_kProcessCfg.Locked_Write_ServerList( kSendPacket, kRecvSI.nRealm );

	switch(kRecvSI.nServerType)
	{
	case CEL::ST_MACHINE_CONTROL:
//		{// 
//		}break;
	case CEL::ST_CONTENTS:
		{
			g_kRealmMgr.WriteToPacket(kSendPacket, 0, ERealm_SendType_Server);
			g_kDefPlayTime.WriteToPacket(kSendPacket);
		}break;
	case CEL::ST_GMSERVER:
	case CEL::ST_LOG:
		{
			g_kRealmMgr.WriteToPacket(kSendPacket, 0, ERealm_SendType_Server);
		}break;
	case CEL::ST_LOGIN:
		{
			g_kRealmMgr.WriteToPacket(kSendPacket, 0, ERealm_SendType_Server );
			g_kIPChecker.WriteToPacket(kSendPacket);

			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_PATCH_VERSION_EDIT, L"EXEC [dbo].[up_GetLastPatchVersion2]");
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case CEL::ST_CONSENT:
		{
			if(	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_TAIWAN)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_RUSSIA)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_VIETNAM)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA)
			&&	!g_kLocal.IsServiceRegion(LOCAL_MGR::NC_USA) )
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot connect ConsentServer ServiceRegion=") << g_kLocal.ServiceRegion());
				pkSession->VTerminate();
				return;
			}
			else
			{
				BM::Stream kCNPacket(PT_A_S_ANS_GREETING);
				kSendSI.WriteToPacket(kCNPacket);
				kCNPacket.Push(g_kLocal.NationCode());
				g_kWaitingLobby.Locked_WriteToPacket_LoginUserInfoToConsent(kCNPacket);
				pkSession->VSend(kCNPacket);
				return;// return!!!!!!
				/*
				SEventMessage kMsg(EIMM_OBJ_CONSENT, PgConsentControl::ECONSENT_2ND_SETCONNECT);
				g_kImmTask.PutMsg(kMsg);
				return;// return!!!!!!
				*/
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect Server Type") );
			pkSession->VTerminate();//세션 끊음.
			return;
		}break;
	}

	//SendMCC_NfyServerConnect(kRecvSI, true);

	pkSession->VSend(kSendPacket);

	// kSendPacket 보내고 나서 해야 할 일이 있다.
	switch(kRecvSI.nServerType)
	{
	case CEL::ST_LOG:
		{
			if(kRecvSI.nRealm == kSendSI.nRealm
				&& kRecvSI.nChannel == kSendSI.nChannel)
			{
				CEL::INIT_CORE_DESC kLogConnectorInit;
				kLogConnectorInit.OnSessionOpen	= OnConnectToLog;
				kLogConnectorInit.OnDisconnect	= OnDisConnectToLog;
				kLogConnectorInit.OnRecv			= OnRecvFromLog;
				kLogConnectorInit.bIsImmidiateActivate = true;
				kLogConnectorInit.kOrderGuid.Generate();
				kLogConnectorInit.IdentityValue(CEL::ST_LOG);
				kLogConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);
				g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLogConnectorInit);
			}
		}break;
	default:
		{
		}break;
	}
}

