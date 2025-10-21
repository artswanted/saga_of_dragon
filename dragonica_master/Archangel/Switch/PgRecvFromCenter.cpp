#include "stdafx.h"
#include "FCS/AntiHack.h"
#include "Variant/PgMCtrl.h"
#include "PgRecvFromCenter.h"
#include "PgRecvFromMap.h"
#include "PgRecvFromUser.h"
#include "PgHub.h"
#include "PgReloadableDataMgr.h"

bool RegistAcceptor(TBL_SERVERLIST const &rkServerInfo)
{
{
	//User Acceptor
	CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrUserBind;
	CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrUserNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;//Already In Use
		}

		CEL::INIT_CORE_DESC kInit;
		kInit.kBindAddr = rkBindAddr;
		kInit.kNATAddr = rkNATAddr;

		kInit.OnSessionOpen	= OnAcceptFromUser;
		kInit.OnDisconnect	= OnDisconnectFromUser;
		kInit.OnRecv			= OnRecvFromUser;
		kInit.bIsImmidiateActivate	= true;
		kInit.dwBandwidthLimit = 1024 * 15; // 15 KB

		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Try Regist Acceptor [") << rkBindAddr.ToString().c_str() << _T("]") );

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);

		g_kHub.AcceptorAddr(rkNATAddr);//유저에게 줄 주소는 NAT 로 갖고 있어야 한다
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect Acceptor Addr [") << rkBindAddr.ToString().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}
}
{
	CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrServerBind;
	CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrServerNat;

	if(rkBindAddr.wPort)
	{
		if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			return false;//Already In Use
		}

		CEL::INIT_CORE_DESC kInit;
		kInit.kBindAddr = rkBindAddr;
		kInit.kNATAddr = rkNATAddr;

		kInit.OnSessionOpen		= OnAcceptFromMap;
		kInit.OnDisconnect		= OnDisconnectFromMap;
		kInit.OnRecv			= OnRecvFromMap;
		kInit.bIsImmidiateActivate	= true;
		kInit.ServiceHandlerType(CEL::SHT_SERVER);//서버용

		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Try Regist Acceptor [") << rkBindAddr.ToString().c_str() << _T("]") );

		g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Incorrect Acceptor Addr [") << rkBindAddr.ToString().c_str() << _T("]") );
	}
}
	return false;
}

void CALLBACK OnAcceptFromCenter( CEL::CSession_Base *pkSession )
{	//!  세션락
	std::vector<char> kEncodeKey;
	if(S_OK == pkSession->VGetEncodeKey(kEncodeKey))
	{
		//INFO_LOG(BM::LOG_LV6, _T("[%s]-[%d] Send EncryptKey"), __FUNCTIONW__, __LINE__);

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

void CALLBACK OnConnectFromCenter( CEL::CSession_Base *pkSession )
{	//!  세션락
	bool const bIsSucc = pkSession->IsAlive();
	
	if( bIsSucc )
	{
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Connect Success [") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
		bool const bPublic = pkSession->Addr() == g_kProcessCfg.PublicCenterServerAddr();
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		if ( bPublic )
		{
			g_kProcessCfg.Locked_ConnectPublicCenter();
			INFO_LOG( BM::LOG_LV1, __FL__ << _T(" Failed Connect to Public Center") );
			return;
		}

		INFO_LOG( BM::LOG_LV1, __FL__ << _T(" Failed Connect to Center") );
		g_kProcessCfg.Locked_ConnectCenter();
	}
}

void CALLBACK OnDisconnectFromCenter( CEL::CSession_Base *pkSession )
{//!  세션락
	INFO_LOG( BM::LOG_LV6, __FL__ << _T(" Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);//
	
	bool const bPublic = pkSession->Addr() == g_kProcessCfg.PublicCenterServerAddr();
	g_kHub.Locked_OnDisconnectFromCenter( bPublic );

	if ( bPublic )
	{
		g_kProcessCfg.Locked_ConnectPublicCenter();
	}
	else
	{
		g_kProcessCfg.Locked_ConnectCenter();
	}
}

void CALLBACK OnRecvFromCenter(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	PACKET_ID_TYPE usType = 0;
	pkPacket->Pop(usType);

	//std::cout<< "Received Packet T:[" << usType <<"] Size["<< pkPacket->Size() << "]"<< std::endl;
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

			if ( kRecvSI.nChannel == g_kProcessCfg.ChannelNo() )
			{
				HRESULT const hRet = g_kProcessCfg.Locked_Read_ServerList(*pkPacket);
				if( SUCCEEDED(hRet) )
				{
					if( S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession) )//이 세션이 CENTER 의 세션
					{
						if( SetSendWrapper(kRecvSI) )
						{
							SERVER_IDENTITY const &kSI = g_kProcessCfg.ServerIdentity();
							TBL_SERVERLIST kServerInfo;
							if(S_OK == g_kProcessCfg.Locked_GetServerInfo(kSI, kServerInfo))
							{
								g_kProcessCfg.Locked_ConnectPublicCenter();

								g_kReloadMgr.SyncProcess(pkPacket);

								RegistAcceptor(kServerInfo);
//								ReadyToService();
								LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
								return;
							}
							else
							{
								VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Login Server No Acceptor info") );
								LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
								return;
							}
						}
					}
				}
				g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);
			}
			else if ( CProcessConfig::IsPublicChannel(kRecvSI.nChannel) )
			{
				if ( S_OK == g_kProcessCfg.Locked_OnGreetingServer( kRecvSI, pkSession ) )
				{
					g_kPublicCenterSI = kRecvSI;
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
					return;
				}
			}

//			VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] Add Server Session Failed Realm[%d] Chn[%d] No[%d] Type[%d] "), __FUNCTIONW__, __LINE__, kRecvSI.nRealm, kRecvSI.nChannel, kRecvSI.nServerNo, kRecvSI.nServerType);
			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Add Server Session Failed Realm[") << kRecvSI.nRealm << _T("] Chn[") << kRecvSI.nChannel << _T("] No[") << kRecvSI.nServerNo << _T("] Type[") << kRecvSI.nServerType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	case PT_A_A_SERVER_SHUTDOWN:
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket( *pkPacket );
			if( kSI.nServerType == CEL::ST_CONTENTS )
			{
				g_kHub.Locked_OnDisconnectFromCenter(false);
			}
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			SERVER_IDENTITY  kRecvSI;
			bool bAutoLogin = false;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAutoLogin);

			if ( !bAutoLogin )
			{
				g_kHub.Locked_RemoveMember( CDC_Normal, kMemberGuid );
			}
		}break;
	case PT_N_S_ANS_RESERVEMEMBER_LOGIN:
		{
			BM::GUID guidMember;
			BM::GUID guidOrder;
			pkPacket->Pop( guidMember );
			pkPacket->Pop( guidOrder );

			BM::Stream kPacket( PT_S_C_ANS_AUTOLOGIN, guidOrder );
			g_kHub.Locked_RemoveMember( CDC_ChangeChannel, guidMember, kPacket );
		}break;
	case PT_A_S_NFY_USER_DISCONNECT:
		{
			BM::GUID kMemberGuid;
			BYTE cCause = 0;
			BYTE byReason = 0;
			pkPacket->Pop( cCause );
			pkPacket->Pop( kMemberGuid );
			pkPacket->Pop( byReason );

			g_kHub.Locked_RemoveMember( static_cast<EClientDisconnectedCause>(cCause), kMemberGuid, byReason );
		}break;
	case PT_T_S_REQ_RESERVE_MEMBER://유저 할당
		{
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("<06> Reserve User Order From Center") );

			SReqSwitchReserveMember kRSRM;
			kRSRM.ReadFromPacket(*pkPacket);

			if ( !g_kHub.Locked_ProcessReserveMember(kRSRM) )
			{
				// 할당에 실패하면 일단 끊어
				pkSession->VTerminate();
			}
		}break;
	case PT_T_S_ANS_CHARACTER_LIST:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
			BM::Stream kPacket(PT_S_C_ANS_CHARACTER_LIST, *pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("PT_T_S_ANS_CHARACTER_LIST : Member=") << kMemberGuid );
		}break;
	case PT_T_S_ANS_FIND_CHARACTOR_EXTEND_SLOT:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
			BM::Stream kPacket(PT_S_C_ANS_FIND_CHARACTOR_EXTEND_SLOT, *pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("PT_T_S_ANS_FIND_CHARACTOR_EXTEND_SLOT : Member=") << kMemberGuid );
		}break;
	case PT_N_C_ANS_CHECK_CHARACTERNAME_OVERLAP:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);

			BM::Stream kPacket(PT_N_C_ANS_CHECK_CHARACTERNAME_OVERLAP, *pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
		}break;
	case PT_T_S_ANS_CREATE_CHARACTER:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);

			BM::Stream kPacket(PT_S_C_ANS_CREATE_CHARACTER, *pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
		}break;
	case PT_T_C_ANS_SELECT_CHARACTER:	
		{
			ESelectCharacterRet kRet = E_SCR_SUCCESS;
			BM::GUID kMemberGuid;
			
			pkPacket->Pop(kRet);
			pkPacket->Pop(kMemberGuid);

			BM::Stream kPacket(usType);
			kPacket.Push(kRet);
			kPacket.Push(*pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
		}break;
	case PT_N_C_ANS_REALM_MERGE:
		{
			BM::GUID kMemberGuid;
			EUserCharacterRealmMergeReturn kRet = UCRMR_None;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kRet);

			BM::Stream kPacket(usType);
			kPacket.Push(kRet);
			kPacket.Push(*pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
		}break;
	case PT_T_S_ANS_DELETE_CHARACTER:
		{
			BM::GUID kMemberGuid;
			//bool bIsDeleted = false;

			pkPacket->Pop(kMemberGuid);
			//pkPacket->Pop(bIsDeleted);

			BM::Stream kPacket(PT_S_C_ANS_DELETE_CHARACTER);
			kPacket.Push(*pkPacket);
			g_kHub.Locked_SendPacketToUser(kMemberGuid, kPacket);
		}break;
	case PT_T_S_NFY_CHARACTER_MAP_MOVE:
		{
			SERVER_IDENTITY kSI;
			SGroundKey kGndKey;
			BM::GUID kMemberGuid;
			BM::GUID kCharGuid;
			pkPacket->Pop(kSI);
			pkPacket->Pop(kGndKey);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharGuid);
			if( !g_kHub.Locked_ChangeMemberMapServerNo( kSI, kGndKey, kMemberGuid, kCharGuid ) )
			{
				INFO_LOG( BM::LOG_LV6, __FL__ << _T(" ChangeMemberMapServerNo Failed") );
			}
			//이게성공 하고 -> 유저한테.. 쏴주기. -> 번호 바뀌쓰~
		}break;
	case PT_S_S_SEND_TO_SWITCH_USER:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);

			BM::Stream kPacket;
			kPacket.Push(*pkPacket);
			
			g_kHub.Locked_SendPacketToUser( kMemberGuid, kPacket );
		}break;
	//case PT_T_S_SND_DISCONNNECT_USER: // 패킷 보내 오는 곳이 없다.
	//	{
	//		BM::GUID kMemberGuid;
	//		pkPacket->Pop(kMemberGuid);			
	//		g_kHub.Locked_RemoveMember(kMemberGuid);
	//		INFO_LOG(BM::LOG_LV6, _T("[%s] Disconnected with User[%s]"), __FUNCTIONW__, kMemberGuid.str().c_str());
	//	}break;
	case PT_T_S_ANS_LOGINED_PLAYER:
		{
			HRESULT hRet;
			BM::GUID kMemberGuid, kSwitchKey;
			pkPacket->Pop(hRet);
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kSwitchKey);
			
			g_kHub.Locked_Recv_PT_T_S_ANS_LOGINED_PLAYER(hRet, kMemberGuid, kSwitchKey);
		}break;
	//case PT_T_S_REQ_CHARACTER_REMOVE: // 패킷 보내 오는 곳이 없다.
	//	{
	//		BM::GUID kMemberGuid;
	//		BM::GUID kCharacterGuid;
	//		pkPacket->Pop(kMemberGuid);
	//		pkPacket->Pop(kCharacterGuid);
	//		g_kHub.Locked_RemoveMember(kMemberGuid);
	//	}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			MMCCommandProcess(pkPacket);
		}break;
	case PT_IM_A_NFY_ANTIHACK_CONTROL:
		{
			g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);
		}break;
	case PT_A_A_NFY_REFRESH_DB_DATA:
		{
			g_kReloadMgr.SyncProcess(pkPacket);
		}break;
	case PT_M_L_TRY_LOGIN_CHANNELMAPMOVE:
		{
			g_kHub.Locked_RegistChannelMapMoveUser(pkPacket);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV5, __FL__ << _T("unhandled packet Type[") << usType << _T("]"));
		}break;
	}
}

int GetConnectionUserCount()
{
	return g_kHub.Locked_GetSwitchConnectionPlayerCount();
}