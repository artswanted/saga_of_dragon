#include "stdafx.h"
#include "Variant/Global.h"
#include "PgRecvFromImmigration.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromUser.h"
#include "PgLoginUserMgr.h"
#include "PgSendWrapper.h"
#include "Variant/PgMctrl.h"
#include "PgPatchVersionManager.h"
#include "Variant/PgPlayTime.h"

bool RegistAcceptor(const TBL_SERVERLIST &rkServerInfo)
{
	CEL::ADDR_INFO rkBindAddr= rkServerInfo.addrUserBind;
	CEL::ADDR_INFO rkNATAddr = rkServerInfo.addrUserNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;//Already In Use
		}

		//User Accept
		CEL::INIT_CORE_DESC kInit;
		kInit.kBindAddr = rkBindAddr;
		kInit.kNATAddr = rkNATAddr;

		kInit.OnSessionOpen	= OnAcceptFromUser;
		kInit.OnDisconnect	= OnDisConnectToUser;
		kInit.OnRecv			= OnRecvFromUser;
		kInit.bIsImmidiateActivate	= true;

		
		INFO_LOG(BM::LOG_LV6, __FL__ << L"Try Regist UserAcceptor [" << C2L(rkBindAddr) << L"]");

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);
	}

	rkBindAddr = rkServerInfo.addrServerBind;
	rkNATAddr = rkServerInfo.addrServerNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;
		}

		//Server Accept(Gm, Log)
		CEL::INIT_CORE_DESC kGMAcceptorInit;
		kGMAcceptorInit.kBindAddr = rkBindAddr;
		kGMAcceptorInit.kNATAddr = rkNATAddr;
		kGMAcceptorInit.OnSessionOpen	= OnAcceptFromServer;
		kGMAcceptorInit.OnDisconnect	= OnDisConnectToServer;
		kGMAcceptorInit.OnRecv			= OnRecvFromServer;
		kGMAcceptorInit.ServiceHandlerType(CEL::SHT_SERVER);
		kGMAcceptorInit.bIsImmidiateActivate	= true;

		INFO_LOG(BM::LOG_LV6, __FL__ << L"Try Regist Gm Server Acceptor [" << C2L(rkBindAddr) << L"]");

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kGMAcceptorInit);
	}

	return true;
	
	
//	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Incorrect Acceptor Addr [" << rkBindAddr << L"]");
//	return false;
}

void CALLBACK OnConnectToImmigration(CEL::CSession_Base *pkSession)
{	//!  세션락
	if( !pkSession )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L" Session is NULL");
		return;
	}

	bool const bIsSucc = pkSession->IsAlive();
	if(bIsSucc)
	{	
		INFO_LOG(BM::LOG_LV6, __FL__ << L"Connect Success [" << C2L(pkSession->Addr()) << L"]");
	}
	else
	{
		INFO_LOG(BM::LOG_LV1, __FL__ << L"Failed Connect to Immigration");
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectImmigration();//접속 실패. 재접속 시도
	}
}

void CALLBACK OnDisConnectToImmigration(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG(BM::LOG_LV6, __FL__ << L"Close Session Success");
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);

	g_kProcessCfg.Locked_ConnectImmigration();//접속 끊겼으므로 이때부터 재접을 시도.
}

void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{	// 세션키로 해당 채널을 찾아 메시지를  multiplexing 함.
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	//INFO_LOG( BM::LOG_LV7, __FL__ << L"Recved Start[" << usType << L"]") );

	switch( usType )
	{
	case PT_IMM_LOGIN_PATCH_VERSION_EDIT:
		{
			int iMajor = 0,
				iMinor = 0,
				iPatch = 0;
			pkPacket->Pop(iMajor);
			pkPacket->Pop(iMinor);
			pkPacket->Pop(iPatch);
			g_kPatchVersionMgr.SetPatchVersion(iMajor,iMinor,iPatch);
		}break;
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			HRESULT const hRet = g_kProcessCfg.Locked_Read_ServerList(*pkPacket);
			if( SUCCEEDED(hRet) )
			{
				g_kRealmMgr.ReadFromPacket(*pkPacket);//로그인서버는 처음에 렐름 정보 받아놔야함.
				g_kLoginUserMgr.ReadFromPacket(*pkPacket, true);// IP Checker

				if( S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession) )//이 세션이 IMMIGRATION 의 세션
				{
					if( SetSendWrapper(kRecvSI) )
					{
						SERVER_IDENTITY const &kSI = g_kProcessCfg.ServerIdentity();
						TBL_SERVERLIST kServerInfo;
						if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kServerInfo))
						{
							RegistAcceptor(kServerInfo);
							g_kProcessCfg.Locked_ConnectLog();
						//	ReadyToService(CEL::ST_IMMIGRATION);
							return;
						}
						else
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"Login Server No Acceptor info");
							LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
							return;
						}
					}
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			// IMM 서버에 재접하는 경우 일 수 있다.
			//VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Add Server Session Failed Realm[" << kRecvSI.nRealm << L"] Chn[" << kRecvSI.nChannel << "] No[" << kRecvSI.nServerNo << L"] Type[" << kRecvSI.nServerType << L"]");
		}break;
	case PT_IM_L_RELOAD_ACCEPT_IP:
		{
			INFO_LOG(BM::LOG_LV3, __FL__ << L"[PT_IM_L_RELOAD_ACCEPT_IP]");
			g_kLoginUserMgr.ReadFromPacket(*pkPacket, false);// IP Checker
		}break;
	case PT_IM_L_ADD_ACCEPT_IP:
		{
			bool bIsBlockIP = false;
			SIpRange kNewRange, kCheckRange;

			pkPacket->Pop( bIsBlockIP );
			kNewRange.ReadFromPacket( *pkPacket );

			g_kLoginUserMgr.RegIPRange(kNewRange, bIsBlockIP, kCheckRange);
		}break;
	case PT_IM_L_NFY_TERMINATE_USER:
		{
			SAuthInfo kAuthInfo;			
			int iRet = 0;

			kAuthInfo.ReadFromPacket(*pkPacket);
			pkPacket->Pop(iRet);
			
			BM::Stream kPacket(PT_L_C_NFY_TERMINATE_USER);
			kPacket.Push(iRet);
			kPacket.SetStopSignal(true);
			g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );
		}break;
	case PT_IM_L_TRY_LOGIN_RESULT:
		{
			SAuthInfo kAuthInfo;
			std::wstring strMsg;
			int iRet = 0;
			kAuthInfo.ReadFromPacket(*pkPacket);
			if(pkPacket->Pop(iRet))
			{
				pkPacket->Pop(strMsg);
				switch(iRet)
				{
				case E_TLR_SUCCESS_AUTH://AUTH 성공
					{	// 채널 목록, 스위치 목록 -> Create.
						if(S_OK == g_kLoginUserMgr.UpdateAuthInfo(kAuthInfo))
						{
							BM::Stream kPacket(PT_L_C_NFY_REALM_LIST);
							g_kRealmMgr.WriteToPacket(kPacket, 0, ERealm_SendType_Client_Realm); // 새로운
							//g_kRealmMgr.WriteToPacket(kPacket, 0, ERealm_SendType_Client_Channel); // 기존 로직
							g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );	//성공때는 끊지 않음

							if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) && !PgPlayerPlayTime::IsAdult(kAuthInfo.dtUserBirth))
							{//! 생년월일이 NULL 이거나 미성년자라면, 피로도 적용
								BM::Stream kPacket(PT_L_C_NFY_ACTIVE_FATIGUE);
								g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );
							}
							{//마지막 로그아웃한 정보 받아서 보내주자(신규 캐선창에 들어갈 정보)
								BM::Stream kPacket(PT_L_C_NFY_LAST_LOGOUT_TIME);
								kPacket.Push(kAuthInfo.dtLastLogout);
								g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );
							}
						}
						else
						{//
							CAUTION_LOG(BM::LOG_LV6, __FL__ << L"Recved Try Login Result ID[" << kAuthInfo.ID() << L"] Ret[" << iRet << L"] -> can't Update Auth");
							LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
						}
					}break;
				case E_TLR_SUCCESS://로그인까지 OK 떨어지면
					{	
						U_STATE_LOG(BM::LOG_LV1, L"Recved Try Login Result ID[" << kAuthInfo.ID() << L"] Ret[" << iRet << L"]");
					}break;
				case E_TLR_NO_SWITCH:
				case E_TLR_CHANNEL_NOT_SERVICE_NOW:
				case E_TLR_CHANNEL_FULLUSER:
					{// StopSignal을 보내면 안되는 경우는 여기에 정의.
						g_kLoginUserMgr.SetLoginState( kAuthInfo.SessionKey(), ELogin_None, (E_TLR_CHANNEL_NOT_SERVICE_NOW == iRet)  );

						U_STATE_LOG(BM::LOG_LV1, L"Recved Try Login Result ID[" << kAuthInfo.ID() << L"] Ret[" << iRet << L"]");
						BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, iRet ); 
						kPacket.Push(strMsg);
						g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );
					}break;
				case E_TLR_MOBILE_LOCK:
					{// StopSignal을 보내면 안되면서 User 상태도 바꾸면 안되는 경우는 여기에 정의 
						U_STATE_LOG(BM::LOG_LV1, L"Recved Try Login Result ID[" << kAuthInfo.ID() << L"] Ret[" << iRet << L"]");

						BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, iRet ); 
						kPacket.Push(strMsg);
						g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );
					}break;
				default:
					{
						U_STATE_LOG(BM::LOG_LV1, L"Recved Try Login Result ID[" << kAuthInfo.ID() << L"] Ret[" << iRet << L"]");
						BM::Stream kPacket(PT_L_C_TRY_LOGIN_RESULT, iRet );
						kPacket.Push(strMsg);
						kPacket.SetStopSignal(true);
						g_kCoreCenter.Send( kAuthInfo.SessionKey(), kPacket );//인증 실패. 접속 종료
					}break;
				}
			}
		}break;
	case PT_IM_L_NFY_RESERVED_SWITCH_INFO:
		{
			SSwitchReserveResult kSRR;
			kSRR.ReadFromPacket(*pkPacket);
			
			//어떤 결과든. 전송하고 로그인 접속 종료
			BM::Stream kPacket(PT_L_C_NFY_RESERVED_SWITCH_INFO);
			kSRR.WriteToPacket(kPacket);
			kPacket.SetStopSignal(true);

			g_kCoreCenter.Send(kSRR.SessionKey(), kPacket);

			INFO_LOG(BM::LOG_LV6, __FL__ << L"Switch Info[" << C2L(kSRR.addrSwitch) << L"] Send to User[" << kSRR.ID() << L"]");

		}break;
	case PT_IM_L_REQ_RESERVEMEMBER_LOGIN:
		{
			SERVER_IDENTITY kSwitchServer;
			BM::GUID guidMember;
			SERVER_IDENTITY kContentsServer;
			SWaitAutoLogin kWAL;
			pkPacket->Pop(kSwitchServer);
			pkPacket->Pop(guidMember);
			pkPacket->Pop(kContentsServer);
			kWAL.kRSRM.ReadFromPacket(*pkPacket);

			BM::GUID kOrderGuid = g_kLoginUserMgr.InsertAutoLoginWait( kWAL );

			BM::Stream kAnsPacket( PT_L_IM_ANS_RESERVEMEMBER_LOGIN, kContentsServer );
			kAnsPacket.Push(kSwitchServer);
			kAnsPacket.Push(guidMember);
			kAnsPacket.Push(kOrderGuid);
			::SendToImmigration( kAnsPacket );
		}break;
	//case PT_R_L_NFY_SITE_STATE:
	//	{
	//		g_kRealmMgr.ReadFromPacket(*pkPacket);
	//	}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_T_A_NFY_USERCOUNT:
		{
			short sRealm = 0;
			short sChannel = 0;
			int iMax = 0;
			int iCurrent = 0;
			pkPacket->Pop(sRealm);
			pkPacket->Pop(sChannel);
			pkPacket->Pop(iMax);
			pkPacket->Pop(iCurrent);
			g_kRealmMgr.UpdateUserCount(sRealm, sChannel, iMax, iCurrent);
		}break;
	case PT_N_A_NFY_CHANNEL_ALIVE:
		{
			SERVER_IDENTITY  kRecvSI;
			bool bAlive;
			size_t iRdPos = pkPacket->RdPos();
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAlive);
			g_kRealmMgr.AliveChannel(kRecvSI.nRealm, kRecvSI.nChannel, bAlive);
		}break;
	case PT_A_A_NFY_REALM_STATE:
		{
			SERVER_IDENTITY kRecvSI;
			short sRealmState;
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(sRealmState);
			g_kRealmMgr.SetRealmState(kRecvSI.nRealm, sRealmState);
		}break;
	case PT_IM_A_NFY_CHANNEL_NOTICE:
		{
			g_kRealmMgr.ReadFromPacket(*pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	}

//	PgChannel * pChann = g_kRealmMgr.Get(pkSession->SessionKey());
//	if(pChann) 
//	{
//		pChann->OnRecvPacket(pkSession, pkPacket); 
//		return;
//	}
	//INFO_LOG(BM::LOG_LV0, __FL__ << L"Can't Find Channel");
}
