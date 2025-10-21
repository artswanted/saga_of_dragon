#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "FCS/AntiHack.h"
#include "Variant/PgNoticeAction.h"
#include "Contents/Global.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromImmigration.h"
#include "PgGMProcessMgr.h"
#include "PgRecvFromManagementServer.h"
#include "PgServerSetMgr.h"
#include "Transaction.h"
#include "AilePack/NC_Constant.h"

bool RegistAcceptor(TBL_SERVERLIST const &rkServerInfo)
{
	CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrServerBind;
	CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrServerNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;//Already In Use
		}

		CEL::INIT_CORE_DESC kInit;
		kInit.kBindAddr = rkBindAddr;
		kInit.kNATAddr = rkNATAddr;

		kInit.OnSessionOpen	= OnAcceptFromServer;
		kInit.OnDisconnect	= OnDisconnectFromServer;
		kInit.OnRecv			= OnRecvFromServer;
		kInit.bIsImmidiateActivate	= true;
		kInit.ServiceHandlerType(CEL::SHT_SERVER);
		
		INFO_LOG( BM::LOG_LV6, __FL__ << _T("Try Regist Acceptor [") << rkBindAddr.ToString().c_str() << _T("]") );

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);

		return true;
	}
	
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Incorrect Acceptor Addr [") << rkBindAddr.ToString().c_str() << _T("]") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void CALLBACK OnConnectFromImmigration(CEL::CSession_Base *pkSession)
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	if(bIsSucc)
	{	
		INFO_LOG( BM::LOG_LV6, __FL__ << _T("Connect Success [") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
		INFO_LOG( BM::LOG_LV1, __FL__ << _T("Failed Connect to Immigration") );
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectImmigration();//접속 실패. 재접속 시도
	}
}

void CALLBACK OnDisconnectFromImmigration(CEL::CSession_Base *pkSession)
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);

	g_kProcessCfg.Locked_ConnectImmigration();//접속 끊겼으므로 이때부터 재접을 시도.
	g_kRealmMgr.UpdatePrimeChannel(0, 0);
}

void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	//INFO_LOG( BM::LOG_LV9, _T("[%s]-[%d] Recved Start[%d]"), __FUNCTIONW__, __LINE__, usType );

	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			HRESULT const hRet = g_kProcessCfg.Locked_Read_ServerList(*pkPacket);
			if ( SUCCEEDED(hRet) )
			{
				if ( S_OK == hRet )
				{
					CONT_SERVER_HASH kContServerHash;
					CONT_MAP_CONFIG kContMapCfg_Static;
					CONT_MAP_CONFIG kContMapCfg_Mission;

					g_kProcessCfg.Locked_GetServerInfo(CEL::ST_MAP,kContServerHash);
					g_kProcessCfg.Locked_GetMapServerCfg( &kContMapCfg_Static, &kContMapCfg_Mission );
					//g_kRealmUserMgr.Locked_Build( kContServerHash, kContMapCfg_Static, kContMapCfg_Mission );
				}

				if( S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession) )
				{
					if(SetSendWrapper(kRecvSI))
					{
						TBL_SERVERLIST kServerInfo;
						if(S_OK == g_kProcessCfg.Locked_GetServerInfo(g_kProcessCfg.ServerIdentity(), kServerInfo))//내가 있는가.
						{
							RegistAcceptor(kServerInfo);
						}
					}
				}

				g_kRealmMgr.ReadFromPacket(*pkPacket);
				g_kDefPlayTime.ReadFromPacket(*pkPacket);
				OnPT_A_S_ANS_GREETING(pkSession, kRecvSI);
				g_kProcessCfg.Locked_ConnectLog();

				std::wstring wConfig = g_kProcessCfg.ConfigDir() + _T("Contents_config.ini");
				ReadRealmStateFromIni(wConfig.c_str());
			}
		}break;
	case PT_IM_N_REQ_RESERVE_SWITCH_MEMBER:
		{//스위치 할당 요구 패킷
//			센터에게 보내서 처리
			//PT_N_T_REQ_RESERVE_SWITCH_MEMBER

			SReqSwitchReserveMember kRSRM;
			if (!kRSRM.ReadFromPacket(*pkPacket))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("PT_IM_N_REQ_RESERVE_SWITCH_MEMBER : Cannot Read(SReqSwitchReserveMember) from packet"));
				break;
			}

			g_kRealmUserMgr.Locked_Process_InsertSwitchWaitUser(kRSRM);
		}break;
	case PT_IM_N_ANS_RESERVEMEMBER_LOGIN:
		{
			SERVER_IDENTITY kSwitchServer;
			pkPacket->Pop( kSwitchServer );

			BM::Stream kPacket( PT_N_S_ANS_RESERVEMEMBER_LOGIN, kSwitchServer  );
			kPacket.Push( *pkPacket );
			::SendToCenter( kSwitchServer.nChannel, kPacket);
		}break;
	case PT_IM_T_ANS_CHANNEL_INFORMATION:
		{
			BM::GUID kMemberGuid;
			if ( pkPacket->Pop(kMemberGuid) )
			{
				BM::Stream kPacket( PT_T_C_ANS_CHANNEL_INFORMATION );
				kPacket.Push(*pkPacket);
				g_kRealmUserMgr.Locked_SendToUser( kMemberGuid, kPacket, true );
			}
		}break;
	case PT_T_A_NFY_USERCOUNT:
		{
			short sRealm = 0;
			short sChannel = 0;
			int iMax = 0;
			int iCurrent = 0;
			size_t iRdPos = pkPacket->RdPos();
			pkPacket->Pop(sRealm);
			pkPacket->Pop(sChannel);
			pkPacket->Pop(iMax);
			pkPacket->Pop(iCurrent);
			if (sRealm != g_kProcessCfg.RealmNo() || sChannel != g_kProcessCfg.ChannelNo())
			{
				// 자기 자신에 대한 정보는 저장할 필요 없다.
				g_kRealmMgr.UpdateUserCount(sRealm, sChannel, iMax, iCurrent);

				// 다른 Center에 Broadcasting
				pkPacket->RdPos(iRdPos);
				BM::Stream kDPacket(PT_T_A_NFY_USERCOUNT);
				kDPacket.Push(*pkPacket);
				SendToServerType(CEL::ST_CENTER, kDPacket);
			}
		}break;
	case PT_GM_A_REQ_GMCOMMAND:
		{
			g_kGMProcessMgr.RecvGMCommand(pkPacket);
		}break;
	case PT_GM_A_REQ_GMORDER:
		{
			SGmOrder kOrderData;
			kOrderData.ReadFromPacket(pkPacket); 
			g_kGMProcessMgr.FirstAnalysisGmOder(kOrderData);
		}break;
	case PT_GM_C_ANS_WAIT_RECEIPT:
	case PT_GM_C_ANS_REMAINDER_PETITION:
	case PT_GM_C_ANS_RECEIPT_PETITION:
		{
			//유저에게로 보낸다
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);

			SContentsUser kUserInfo;
			g_kRealmUserMgr.Locked_GetPlayerInfo(kMemberGuid, true, kUserInfo);

			if(!kUserInfo.Empty())
			{
				BM::Stream kPacket(usType);
				kPacket.Push(*pkPacket);	
				g_kRealmUserMgr.Locked_SendToUser(kUserInfo.kMemGuid, kPacket);
			}
		}break;
	case PT_O_N_REQ_CHECK_LOGINED_USER:
		{
			std::wstring kPlayerName;
			BM::GUID kGmGuid;
			SContentsUser rkUserInfo;
			bool bIsLogined = true;

			pkPacket->Pop(kPlayerName);
			pkPacket->Pop(kGmGuid);

			BM::Stream kPacket(PT_N_O_ANS_CHECK_LOGINED_USER);
			if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo(kPlayerName, rkUserInfo))
			{
				bIsLogined = false;
			}
			kPacket.Push(kGmGuid);
			kPacket.Push(bIsLogined);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	case PT_O_C_REQ_SEND_GM_CHAT:
		{
			std::wstring kPlayerName;
			BM::GUID kGmGuid;
			SContentsUser rkUserInfo;
			bool bIsLogined = true;

			pkPacket->Pop(kPlayerName);
			pkPacket->Pop(kGmGuid);

			if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo(kPlayerName, rkUserInfo))
			{
				//그 순간 나갔으니 다시 패킷을 돌린다.
				BM::Stream kPacket(PT_N_O_ANS_CHECK_LOGINED_USER);
				bool bIsLogined = false;
				kPacket.Push(kGmGuid);
				kPacket.Push(bIsLogined);
				kPacket.Push(*pkPacket);
				SendToImmigration(kPacket);
			}

			//유저한테 보냄.
			BM::Stream kPacket(PT_O_C_NFY_GMCHAT);
			kPacket.Push(kGmGuid);	// GM guid
			kPacket.Push(*pkPacket);
			g_kRealmUserMgr.Locked_SendToUser(rkUserInfo.kMemGuid, kPacket);
		}break;
	case PT_O_C_REQ_GM_CHAT_END:
		{
			std::wstring kPlayerName;
			BM::GUID kGmGuid;
			SContentsUser rkUserInfo;

			pkPacket->Pop(kPlayerName);
			pkPacket->Pop(kGmGuid);
			if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo(kPlayerName, rkUserInfo))
			{
				//그 순간 나갔으니 다시 패킷을 돌린다.
				BM::Stream kPacket(PT_N_O_ANS_CHECK_LOGINED_USER);
				bool bIsLogined = false;
				kPacket.Push(kGmGuid);
				kPacket.Push(bIsLogined);
				kPacket.Push(*pkPacket);
				SendToImmigration(kPacket);
			}
			//유저한테 보냄.
			BM::Stream kPacket(PT_O_C_REQ_GM_CHAT_END);
			g_kRealmUserMgr.Locked_SendToUser(rkUserInfo.kMemGuid, kPacket);
		}break;
		/*
	case PT_IM_T_NFY_CHANNEL_ALIVE:
		{
			SERVER_IDENTITY kSI;
			short int sPrimeChannel;
			bool bAlive;
			pkPacket->Pop(kSI);
			pkPacket->Pop(bAlive);
			pkPacket->Pop(sPrimeChannel);
			g_kRealmMgr.AliveChannel(kSI.nRealm, kSI.nChannel, bAlive);
			g_kRealmMgr.UpdatePrimeChannel(kSI.nRealm, sPrimeChannel);
		}break;
		*/
	case PT_T_T_SEND_TO_SWITCH_USER:
		{
			// Switch User 에게 보내야 할 패킷이다.
			bool bIsMemberGuid;
			BM::GUID kGuid;
			pkPacket->Pop(bIsMemberGuid);
			pkPacket->Pop(kGuid);
			g_kRealmUserMgr.Locked_SendToUser(kGuid, *pkPacket, bIsMemberGuid);
		}break;
	case PT_T_T_SEND_TO_USER_MAP:
		{
			// User가 속한 MapServer 로 보내야 하는 패킷
			BM::GUID kMemberGuid;
			bool bIsGndWrap;
			pkPacket->Pop(kMemberGuid);	// MemberGuid
			pkPacket->Pop(bIsGndWrap);
			g_kRealmUserMgr.Locked_SendToUserGround(kMemberGuid, *pkPacket, true, bIsGndWrap);
		}break;
	case PT_A_SEND_NOTICE:
		{
			PgNoticeAction kNotice;
			kNotice.Recv(pkPacket);
		}break;
	case PT_IM_T_ANS_SWITCH_USER_ADD:
		{
			HRESULT hRet;
			SSwitchPlayerData kSPD;
			pkPacket->Pop(hRet);
			kSPD.ReadFromPacket(*pkPacket);

			g_kRealmUserMgr.Locked_Recv_PT_IM_T_ANS_SWITCH_USER_ADD(hRet, kSPD);
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			BYTE cCause = CDC_Normal;
			pkPacket->Pop( cCause );
			
			if ( CDC_Normal == cCause )
			{
				g_kRealmUserMgr.Locked_RecvPT_A_NFY_USER_DISCONNECT( pkPacket, false );
			}
			else
			{
				BM::GUID kMemberGuid;
				BM::GUID kCharacterGuid;
				BYTE byReason = 0;
				SERVER_IDENTITY kSI;
				pkPacket->Pop(kMemberGuid);
				pkPacket->Pop(kCharacterGuid);
				pkPacket->Pop(byReason);
				kSI.ReadFromPacket(*pkPacket);

				BM::Stream kNfyPacket( PT_A_S_NFY_USER_DISCONNECT, cCause );
				kNfyPacket.Push( kMemberGuid );
				kNfyPacket.Push( byReason );
				::SendToCenter( kSI.nChannel, kNfyPacket );
			}
		}break;
//	case PT_A_A_SERVER_SHUTDOWN:
//		{
// 			SERVER_IDENTITY kSI;
// 			size_t iRdPos = pkPacket->RdPos();
// 			kSI.ReadFromPacket(*pkPacket);
// 
// 			g_kRealmUserMgr.Locked_Recv_PT_A_A_SERVER_SHUTDOWN(kSI);
// 			{
// 				pkPacket->RdPos(iRdPos);
// 				BM::Stream kFPacket(usType);
// 				kFPacket.Push(*pkPacket);
// 				g_kProcessCfg.Locked_SendToChannelServerType( CEL::ST_CENTER, kFPacket, kSI.nChannel, true );
// 			}
//		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			OnRecvFromMCtrl(usType, pkPacket, pkSession);
		}break;
	case PT_N_A_NFY_CHANNEL_ALIVE:
		{
			SERVER_IDENTITY kChannelSI;
			bool bAlive = true;
			kChannelSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAlive);
			g_kRealmMgr.AliveChannel(kChannelSI.nRealm, kChannelSI.nChannel, bAlive);
			BM::Stream kCPacket(PT_N_A_NFY_CHANNEL_ALIVE);
			kChannelSI.WriteToPacket(kCPacket);
			kCPacket.Push(bAlive);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, kCPacket);
		}break;
	case PT_IM_A_NFY_ANTIHACK_CONTROL:
		{
			size_t iRdPos = pkPacket->RdPos();
			g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);

			BM::Stream kWPacket(PT_IM_A_NFY_ANTIHACK_CONTROL);
			pkPacket->RdPos(iRdPos);
			kWPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_CENTER, kWPacket);
		}break;
	case PT_X_N_ANS_CASH:
		{
			LOCAL_MGR::NATION_CODE const eNation = static_cast<LOCAL_MGR::NATION_CODE>(g_kLocal.ServiceRegion());
			switch (eNation)
			{
			case LOCAL_MGR::NC_EU:
			case LOCAL_MGR::NC_JAPAN:
				{
					GALA::OnReceiveCashResult(pkPacket);
				}break;
			case LOCAL_MGR::NC_RUSSIA:
				{
					NIVAL::OnReceiveCashResult(pkPacket);
				}break;
			case LOCAL_MGR::NC_KOREA:
				{
					NC::OnReceiveCashResult(pkPacket);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					GRAVITY::OnReceiveCashResult( pkPacket );
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("PT_X_N_ANS_CASH : Unknown Serveice Region=") << static_cast<int>(eNation));
				}break;
			}
		}break;
	case PT_CN_A_NFY_CASH_NC:
		{
			BM::GUID kMemberGuid;
			unsigned short usPointId = 0;
			unsigned __int64 i64Point = 0i64;
			unsigned __int64 uiPointDifference = 0i64;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(usPointId);
			pkPacket->Pop(i64Point);
			pkPacket->Pop(uiPointDifference);

			INFO_LOG(BM::LOG_LV7, __FL__ << _T("Recv PT_CN_A_NFY_CASH_NC Point ID:") << usPointId << _T(", Point Amount: ") << i64Point);

			SContentsUser kUser;
			::GetPlayerByGuid(kMemberGuid, true, kUser);

			BM::Stream kPacket(PT_I_M_CS_NOTI_CASH_MODIFY_NC);
			kPacket.Push( kUser.kCharGuid );
			kPacket.Push( usPointId );
			kPacket.Push( i64Point );
			kPacket.Push( uiPointDifference );
				
			g_kRealmUserMgr.Locked_SendToUserGround(kUser.kCharGuid, kPacket, false, true);
		}break;
	case PT_CN_A_NFY_ITEM:
		{// 현재 SA 프로토콜로 구현 불가
		}break;
	case PT_IM_A_NFY_CHANNEL_NOTICE:
		{
			size_t const iRdPos = pkPacket->RdPos();
			g_kRealmMgr.ReadFromPacket(*pkPacket);
			pkPacket->RdPos(iRdPos);
			BM::Stream kDPacket(PT_IM_A_NFY_CHANNEL_NOTICE);
			kDPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_CENTER, kDPacket);
		}break;
	case PT_SYNC_DEF_PLAYERPLAYTIME:
		{
			g_kDefPlayTime.ReadFromPacket(*pkPacket);

			BM::Stream kPacket(PT_SYNC_DEF_PLAYERPLAYTIME);
			g_kDefPlayTime.WriteToPacket(kPacket);
			SendToServerType(CEL::ST_CENTER, kPacket);
		}break;
	case PT_I_M_GMCMD_RESETPLAYERPLAYTIME:
	case PT_I_M_GMCMD_SETPLAYERPLAYTIME:
		{
			g_kRealmUserMgr.Locked_RecvTaskProcessPlayerPlayTime(usType, pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	
	//INFO_LOG( BM::LOG_LV9, _T("[%s]-[%d ] Recved End [%d]"), __FUNCTIONW__, __LINE__, usType );
}

void OnPT_A_S_ANS_GREETING(CEL::CSession_Base *pkSession, SERVER_IDENTITY const &rkServerIdentity)
{
	switch(rkServerIdentity.nServerType)
	{
	case CEL::ST_IMMIGRATION:
		{
			// Imm 에게 현재 접속자 정보를 전송해 준다.
			BM::Stream kLPacket( PT_T_IM_NFY_USERLIST, g_kProcessCfg.ServerIdentity() );
			g_kRealmUserMgr.Locked_WriteToPacket_User(kLPacket, PgRealmUserManager::ESSMP_SSwitchPlayerData);
			pkSession->VSend(kLPacket);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}