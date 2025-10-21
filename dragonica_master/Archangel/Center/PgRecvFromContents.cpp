#include "stdafx.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgNoticeAction.h"
#include "Variant/PgEventView.h"
#include "variant/pggamblemachine.h"
#include "Variant/PgPvPRanking.h"
#include "FCS/AntiHack.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromContents.h"
//#include "PgGMProcessMgr.h"
#include "Contents/PgMissionMgr.h"
#include "PgPvPLobbyMgr.h"
#include <hotmeta/hotmeta.h>
#include "Variant/PgBattlePassMgr.h"

extern void BuildDef();
extern bool CALLBACK OnRecvFromMCtrl(CEL::CSession_Base * const pkSession, WORD wkType, BM::Stream * const pkPacket);


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

void CALLBACK OnConnectFromContents(CEL::CSession_Base *pkSession)
{	//!  јјјЗ¶ф
	bool const bIsSucc = pkSession->IsAlive();
	if(bIsSucc)
	{	
		INFO_LOG( BM::LOG_LV6, __FL__ << _T("Connect Success [") << pkSession->Addr().ToString().c_str() << _T("]") );
	}
	else
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Failed Connect to Contents") );
		g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
		g_kProcessCfg.Locked_ConnectContents();//БўјУ ЅЗЖР. АзБўјУ ЅГµµ
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Failed Connect to Contents"));
	}
}

void CALLBACK OnDisconnectFromContents(CEL::CSession_Base *pkSession)
{//!  јјјЗ¶ф
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Close Session Success") );
	g_kProcessCfg.Locked_OnDisconnectServer(pkSession);
	
	// ёрµз ЅєА§ДЎЗСЕЧ CLEARЗП¶у°н ѕЛ·Б¶у
	if( !pkSession->m_kSessionData.IsEmpty() )
	{
		SERVER_IDENTITY kRecvSI;
		kRecvSI.ReadFromPacket(pkSession->m_kSessionData);
		BM::Stream kPacket( PT_A_A_SERVER_SHUTDOWN );
		kRecvSI.WriteToPacket( kPacket );
		::SendToServerType( CEL::ST_SWITCH, kPacket );
	}

	g_kProcessCfg.Locked_ConnectContents();//БўјУ Іч°еАё№З·О АМ¶§єОЕН АзБўА» ЅГµµ.
	g_kRealmMgr.UpdatePrimeChannel(0, 0);
}

void CALLBACK OnRecvFromContents(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	pkPacket->Pop(usType);

	switch( usType )
	{
	case PT_N_A_NFY_BATTLE_PASS_INFO:
		{
			g_kBattlePassMgr.Locked_ReadFromPacket(*pkPacket);
		}break;
	case hotmeta::PT_I_M_HOTMETA_SYNC:
		{
			SendToServerType(CEL::ST_MAP,BM::Stream(usType,*pkPacket));
			hotmeta::apply(*pkPacket);
		}break;
	case PT_SYNC_EVENT_ITEM_REWARD:
		{
			SendToServerType(CEL::ST_MAP,BM::Stream(usType,*pkPacket));
			CONT_EVENT_ITEM_REWARD kCont;
			PU::TLoadTable_AM(*pkPacket,kCont);
			g_kTblDataMgr.SetContDef(kCont);
		}break;
	case PT_SYNC_GAMBLEMACHINE:
		{
			SendToServerType(CEL::ST_MAP,BM::Stream(PT_SYNC_GAMBLEMACHINE,*pkPacket));
			g_kGambleMachine.ReadFromPacket(*pkPacket);
		}break;
	case PT_I_M_REQ_HOME_CREATE:
		{
			g_kServerSetMgr.Locked_Recv_PT_I_M_REQ_HOME_CREATE(pkPacket);
		}break;
	case PT_I_M_REQ_HOME_DELETE:
		{
			g_kServerSetMgr.Locked_Recv_PT_I_M_REQ_HOME_DELETE(pkPacket);
		}break;
	case PT_I_M_MACRO_CHECK_TABLE_SYNC:
		{
			BM::Stream kPacket(usType,*pkPacket);
			SendToServerType(CEL::ST_MAP, kPacket);
		}break;
	case PT_M_T_REQ_INDUN_PARTY_LIST:
	case PT_M_C_NFY_GAMBLEMACHINE_MIXUP_RESULT:
	case PT_M_C_NOTI_ACHIEVEMENT_COMPLETE_FIRST:
	case PT_M_C_NFY_GENERIC_ACHIEVEMENT_NOTICE:
	case PT_M_C_NFY_GAMBLEMACHINE_RESULT:
	case PT_M_C_NFY_ITEM_MAKING_SUCCESS:
	case PT_M_C_NOTI_OPEN_GAMBLE:
	case PT_M_C_NFY_USEITEM:
	case PT_M_C_NOTI_OXQUIZ_OPEN:
	case PT_M_C_NOTI_OXQUIZ_LOCK:
	case PT_I_M_NOTI_OXQUIZ_NOTI:
	case PT_M_C_LUCKYSTAR_RESULT:
	case PT_M_C_LUCKYSTAR_CLOSE:
	case PT_M_C_NFY_EVENT_QUEST_INFO:
	case PT_M_C_NFY_EVENT_QUEST_NOTICE:
	case PT_N_C_NFY_BS_NOTICE:
	case PT_N_M_NFY_ALL_USER_ADDEFFECT:
	case PT_N_M_NFY_ALL_GROUND_WORLD_ENVIRONMENT_STATUS:
	case PT_N_C_NFY_REALM_QUEST_INFO:
	case PT_N_C_NFY_REALM_QUEST_REWARD:
	case PT_M_C_NOTI_EVENT_ITEM_REWARD:
	case PT_M_C_NOTI_TREASURE_CHEST:
		{
			BM::Stream kPacket(usType,*pkPacket);
			SendToServerType(CEL::ST_MAP, kPacket);
		}break;
	case PT_A_A_NFY_BS_GAME_INFO:
	case PT_N_C_NFY_BS_GND_NOTICE:
		{
			int iGroundNo = 0;
			pkPacket->Pop( iGroundNo );

			BM::Stream kPacket(usType, *pkPacket);
			g_kServerSetMgr.Locked_SendToGround(SGroundKey(iGroundNo), kPacket, true);
		}break;
	case PT_T_A_COUPON_EVENT_SYNC:
		{
			BM::Stream kPacket(usType,*pkPacket);
			SendToServerType(CEL::ST_MAP, kPacket);
		}break;
	case PT_M_T_REQ_SMS:
		{
			BM::Stream kPacket(PT_T_C_NFY_BULLHORN,*pkPacket);
			SendToServerType(CEL::ST_MAP, kPacket);
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
				BM::GUID kStoreValueKey;
				pkPacket->Pop(kStoreValueKey);
					
				INFO_LOG(BM::LOG_LV7, _T("Recv StoreKey(Ans Greeting) ::")<< g_kControlDefMgr.StoreValueKey());
				g_kControlDefMgr.StoreValueKey(kStoreValueKey);
				g_kTblDataMgr.PacketToData(*pkPacket);
				
				BuildDef();
				if ( S_OK == hRet )
				{
					CONT_SERVER_HASH kContServerHash;
					CONT_MAP_CONFIG kContMapCfg_Static;
					CONT_MAP_CONFIG kContMapCfg_Mission;

					g_kProcessCfg.Locked_GetServerInfo(CEL::ST_MAP,kContServerHash);
					g_kProcessCfg.Locked_GetMapServerCfg( &kContMapCfg_Static, &kContMapCfg_Mission );
					g_kServerSetMgr.Locked_Build( kContServerHash, kContMapCfg_Static, kContMapCfg_Mission );
				}

				if( S_OK == g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession) )
				{
					if(SetSendWrapper(kRecvSI))
					{
						TBL_SERVERLIST kServerInfo;
						if(S_OK == g_kProcessCfg.Locked_GetServerInfo(g_kProcessCfg.ServerIdentity(), kServerInfo))//і»°Ў АЦґВ°Ў.
						{
							RegistAcceptor(kServerInfo);
						}
					}
				}

				g_kRealmMgr.ReadFromPacket(*pkPacket);
				g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);
				g_kGambleMachine.ReadFromPacket(*pkPacket);
				g_kDefPlayTime.ReadFromPacket(*pkPacket);

				OnPT_A_S_ANS_GREETING(pkSession, kRecvSI);
				g_kProcessCfg.Locked_ConnectLog();

				{
					CONT_DEF_FILTER_UNICODE kFilterList;
					PU::TLoadArray_M(*pkPacket, kFilterList);
					CONT_DEF_FILTER_UNICODE::const_iterator filter_iter = kFilterList.begin();
					while( kFilterList.end() != filter_iter )
					{
						CONT_DEF_FILTER_UNICODE::value_type const& rkFilter = (*filter_iter);
						g_kUnicodeFilter.AddRange(rkFilter.iFuncCode, rkFilter.bFilterType, rkFilter.cStart, rkFilter.cEnd);
						++filter_iter;
					}
				}

				if ( true == g_kProcessCfg.IsPublicChannel() )
				{
					BM::Stream kPvPPacket( PT_T_N_REQ_GET_PVPLOBBY_INFO, true );
					::SendToContents( kPvPPacket );
				}
			}
		}break;
	case PT_T_A_EVENT_SYNC:
		{
			SendToServerType(CEL::ST_MAP, *pkPacket);
		}break;
	case PT_N_M_NFY_SYNC_CASHSHOP_ON_OFF:
		{
			SendToServerType(CEL::ST_MAP, *pkPacket);
		}break;
	case PT_N_A_NFY_GAMETIME:
		{
			switch ( g_kEventView.ReadFromPacket(*pkPacket) )
			{
			case E_SYNCHED_FIRST_GAMETIME:
			case E_SYNCHED_AGAIN_GAMETIME:
				{
					BM::Stream kPacket( PT_A_N_REQ_GAMETIME );
					pkSession->VSend(kPacket);
				}break;
			case E_SYNCHED_GAMETIME:
				{
					// MapServer GameTime АМ БЯ°ЈїЎ єЇ°жµЗёй, GameTimeА» »зїлЗПґВ Hacking °Л»з ·ОБчїЎ №®Б¦°Ў №Я»эЗСґЩ.
					/*
					BM::Stream kSyncPacket( PT_N_A_NFY_GAMETIME );
					g_kEventView.WriteToPacket( true, kSyncPacket );
					SendToServerType( CEL::ST_MAP, kSyncPacket );
					*/
				}break;
			}
		}break;
	case PT_A_N_REQ_GAMETIME:
		{
			SYSTEMTIME kNowTime;
			g_kEventView.GetLocalTime( &kNowTime );

			TCHAR chTime[100];
			CGameTime::SystemTime2String( kNowTime, chTime, 100 );

			INFO_LOG( BM::LOG_LV3,_T("[TEST] Date ") << chTime << _T(" And ") << BM::GetTime32() );
		}break;
	case PT_N_T_REQ_RESERVE_SWITCH_MEMBER:
		{//ЅєА§ДЎ ЗТґз їд±ё ЖРЕ¶
			if ( !g_kProcessCfg.IsPublicChannel() )
			{
				SReqSwitchReserveMember kRSRM;
				kRSRM.ReadFromPacket(*pkPacket);
				
				bool const bIsIgnoreMax = ((kRSRM.byGMLevel > 0)?true:false);

				SERVER_IDENTITY kSwitchSI;
				if ( g_kSwitchAssignMgr.GetBalanceSwitch( kSwitchSI, bIsIgnoreMax) )
				{
					BM::Stream kJPacket(PT_T_S_REQ_RESERVE_MEMBER);
					kRSRM.WriteToPacket(kJPacket);
					if( !::SendToServer( kSwitchSI, kJPacket ) )
					{
						BM::Stream kIMPacket( PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED, E_TLR_NO_SWITCH );
						kIMPacket.Push( kRSRM.guidMember );
						::SendToImmigration( kIMPacket );
					}
				}
				else
				{
					BM::Stream kIMPacket( PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED, E_TLR_CHANNEL_FULLUSER );
					kIMPacket.Push( kRSRM.guidMember );
					::SendToImmigration( kIMPacket );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("[PT_N_T_REQ_RESERVE_SWITCH_MEMBER] Public Channel No Recv Plz") );
			}
		}break;
	case PT_N_S_ANS_RESERVEMEMBER_LOGIN:
		{
			SERVER_IDENTITY kSwitchServer;
			pkPacket->Pop( kSwitchServer );

			BM::Stream kPacket( PT_N_S_ANS_RESERVEMEMBER_LOGIN );
			kPacket.Push( *pkPacket );

			g_kProcessCfg.Locked_SendToServer( kSwitchServer, kPacket );
		}break;
	case PT_IM_T_ANS_CHANNEL_INFORMATION:
		{
			BM::GUID kMemberGuid;
			if ( pkPacket->Pop(kMemberGuid) )
			{
				BM::Stream kPacket(PT_T_C_ANS_CHANNEL_INFORMATION );
				kPacket.Push(*pkPacket);
				g_kServerSetMgr.Locked_SendToUser( kMemberGuid, kPacket, true );
			}
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
			if (sRealm != g_kProcessCfg.RealmNo() || sChannel != g_kProcessCfg.ChannelNo())
			{
				// АЪ±в АЪЅЕїЎ ґлЗС Б¤єёґВ АъАеЗТ ЗКїд ѕшґЩ.
				g_kRealmMgr.UpdateUserCount(sRealm, sChannel, iMax, iCurrent);
			}
		}break;
	case PT_T_C_NFY_NOTICE:
		{
			BM::Stream kPacket(PT_T_C_NFY_NOTICE);
			kPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_MAP, kPacket);
		}break;
/*
	case PT_IM_T_NFY_PRIME_CHANNEL:
		{
			short int sRealm, sNewPrimeChannel;
			pkPacket->Pop(sRealm);
			pkPacket->Pop(sNewPrimeChannel);
			g_kRealmMgr.UpdatePrimeChannel(sRealm, sNewPrimeChannel);
			g_kSyncObjControl.IsControlMgr((sRealm == g_kProcessCfg.RealmNo() && sNewPrimeChannel == g_kProcessCfg.ChannelNo()) ? true : false);
		}break;
*/
	case PT_T_T_SEND_TO_SWITCH_USER:
		{
			// Switch User їЎ°Ф єёі»ѕЯ ЗТ ЖРЕ¶АМґЩ.
			bool bIsMemberGuid;
			BM::GUID kGuid;
			pkPacket->Pop(bIsMemberGuid);
			pkPacket->Pop(kGuid);
			g_kServerSetMgr.Locked_SendToUser(kGuid, *pkPacket, bIsMemberGuid);
		}break;
	case PT_T_T_SEND_TO_USER_MAP:
		{
			// User°Ў јУЗС MapServer ·О єёі»ѕЯ ЗПґВ ЖРЕ¶
			BM::GUID kMemberGuid;
			bool bIsGndWrap;
			pkPacket->Pop(kMemberGuid);	// MemberGuid
			pkPacket->Pop(bIsGndWrap);
			g_kServerSetMgr.Locked_SendToUserGround(kMemberGuid, *pkPacket, true, bIsGndWrap);
		}break;
	case PT_A_SEND_NOTICE:
		{
			PgNoticeAction kNotice;
			kNotice.Recv(pkPacket);
		}break;
	case PT_T_S_ANS_CHARACTER_LIST:
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket(*pkPacket);
			
			BM::Stream kSwitchPacket(PT_T_S_ANS_CHARACTER_LIST, *pkPacket);
			
			g_kProcessCfg.Locked_SendToServer(kSI, kSwitchPacket);
		}break;
	case PT_T_S_ANS_FIND_CHARACTOR_EXTEND_SLOT:
		{
			SERVER_IDENTITY kSI;
			kSI.ReadFromPacket(*pkPacket);
			
			BM::Stream kSwitchPacket(PT_T_S_ANS_FIND_CHARACTOR_EXTEND_SLOT, *pkPacket);
			
			g_kProcessCfg.Locked_SendToServer(kSI, kSwitchPacket);
		}break;
	case PT_N_T_WRAPPED_TO_SWITCH:
		{
			SERVER_IDENTITY kSI;
			BM::GUID kMemberGuid;
			kSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kMemberGuid);
			BM::Stream kOrgPacket;
			kOrgPacket.Push(*pkPacket);

			g_kProcessCfg.Locked_SendToServer(kSI, kOrgPacket);
		}break;
	case PT_A_GND_WRAPPED_PACKET:
		{
			SGroundKey kKey;
			bool bIsGndWrap = true;
			pkPacket->Pop( kKey );
			pkPacket->Pop( bIsGndWrap );
			g_kServerSetMgr.Locked_SendToGround(kKey, *pkPacket, bIsGndWrap);
		}break;
	case PT_A_GND_WRAPPED_PACKET_DIRECT:
		{//°°АєДЪµе µО±єµҐАУ
			SERVER_IDENTITY kSI;
			SGroundKey kKey;
			kSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(kKey);

			g_kServerSetMgr.Locked_SendToGround(kKey, *pkPacket, true);
		}break;
	case PT_A_C_WRAPPED_PACKET:
		{
			BM::GUID kMemberGuid;
			pkPacket->Pop(kMemberGuid);
		
			g_kServerSetMgr.Locked_SendToUser(kMemberGuid, *pkPacket);
		}break;
	case PT_A_CNGND_WRAPPED_PACKET:
		{
			SGroundKey kGroundKey;
			bool bWrapper = true;
			kGroundKey.ReadFromPacket( *pkPacket );
			pkPacket->Pop( bWrapper );

			if ( pkPacket->RemainSize() >= sizeof(BM::Stream::DEF_STREAM_TYPE) )
			{
				BM::Stream kWPacket;
				kWPacket.Push( *pkPacket );
				g_kServerSetMgr.Locked_SendToGround( kGroundKey, kWPacket, bWrapper );
			}
		}break;
	case PT_N_T_NFY_SELECT_CHARACTERS:
		{
			g_kServerSetMgr.Locked_Recv_PT_N_T_NFY_SELECT_CHARACTER( pkPacket );
		}break;
	case PT_N_T_RES_MAP_MOVE:	//ContentsServer : MapMoveїдГ» ЖРЕ¶
		{
			g_kServerSetMgr.Locked_Recv_PT_N_T_RES_MAP_MOVE(pkPacket);
		}break;
	case PT_N_T_REQ_CREATE_PUBLICMAP:
		{
			EContentsMessageType kMsgType = PMET_NONE;
			SGroundMakeOrder kGndMakeOrder;
			pkPacket->Pop( kMsgType );
			kGndMakeOrder.ReadFromPacket( *pkPacket );

			SERVER_IDENTITY kSI;
			HRESULT const hRet = g_kServerSetMgr.Locked_GroundLoadBalance( kGndMakeOrder.kKey, kSI );
			if ( S_OK == hRet )
			{
				pkPacket->PosAdjust();
				::SendToServer( kSI, *pkPacket );
			}
			else
			{
				BM::Stream kAnsPacket( PT_T_N_ANS_CREATE_PUBLICMAP, hRet );
				kGndMakeOrder.WriteToPacket( kAnsPacket );
				kAnsPacket.Push( *pkPacket );
				::SendToRealmContents( kMsgType, kAnsPacket );
			}
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			SERVER_IDENTITY  kRecvSI;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharacterGuid);
			kRecvSI.ReadFromPacket(*pkPacket);
			
			g_kServerSetMgr.Locked_ProcessRemoveUser( kMemberGuid, false, true );
			if ( !g_kProcessCfg.IsPublicChannel() )
			{
				g_kSwitchAssignMgr.RemoveAssignSwitch( kRecvSI, kMemberGuid );
			}
		}// break; //no break
	case PT_A_S_NFY_USER_DISCONNECT:
		{
			pkPacket->PosAdjust();
			::SendToServerType(CEL::ST_SWITCH, *pkPacket);
		}break;
	case PT_A_A_SERVER_SHUTDOWN:
		{
			SERVER_IDENTITY  kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);
			g_kServerSetMgr.Locked_ProcessRemoveUser(kRecvSI);
		}break;
	case PT_N_A_NFY_CHANNEL_ALIVE:
		{
			SERVER_IDENTITY  kRecvSI;
			bool bAlive;
			size_t iRdPos = pkPacket->RdPos();
			kRecvSI.ReadFromPacket(*pkPacket);
			pkPacket->Pop(bAlive);
			g_kRealmMgr.AliveChannel(kRecvSI.nRealm, kRecvSI.nChannel, bAlive);

			//pkPacket->RdPos(iRdPos);
			//BM::Stream kGPacket(usType);
			//kGPacket.Push(*pkPacket);
			//SendToServerType(CEL::ST_LOGIN, kGPacket);
		}break;
	case PT_A_CN_WRAPPED_PACKET:
		{
			EContentsMessageType eType;
			if ( true == pkPacket->Pop(eType) )
			{
				int iSecondType = 0;
				pkPacket->Pop( iSecondType );
				SendToChannelContents(eType, *pkPacket, iSecondType);
			}	
		}break;
	case PT_N_C_NFY_NOTICE_PACKET:
		{
			SendToServerType( CEL::ST_MAP, BM::Stream(PT_N_C_NFY_NOTICE_PACKET, *pkPacket) );
		}break;
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
		{
			OnRecvFromMCtrl(pkSession, usType, pkPacket);
		}break;
	case PT_IM_A_NFY_ANTIHACK_CONTROL:
		{
			size_t iRdPos = pkPacket->RdPos();
			g_kAntiHackMgr.Locked_ReadFromPacket(*pkPacket);

			BM::Stream kYPacket(PT_IM_A_NFY_ANTIHACK_CONTROL);
			pkPacket->RdPos(iRdPos);
			kYPacket.Push(*pkPacket);
			SendToServerType(CEL::ST_MAP, kYPacket);
			kYPacket.PosAdjust();
			SendToServerType(CEL::ST_SWITCH, kYPacket);
		}break;
	case PT_A_A_NFY_REFRESH_DB_DATA:
		{
			pkPacket->PosAdjust();

			g_kProcessCfg.Locked_SendToServerType(CEL::ST_SWITCH, *pkPacket);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_MAP, *pkPacket);
		}break;
	case PT_N_T_ANS_GET_PVPLOBBY_INFO:
		{
			if ( true == g_kProcessCfg.IsPublicChannel() )
			{
				CONT_DEF_PLAYLIMIT_INFO kContLobbyInfo;
				VEC_PVP_RANKING kVecPvPRanking;
				PU::TLoadTable_AA( *pkPacket, kContLobbyInfo );
				PU::TLoadArray_M( *pkPacket, kVecPvPRanking );

				CONT_DEF_PLAYLIMIT_INFO::const_iterator itr = kContLobbyInfo.begin();
				for ( ; itr != kContLobbyInfo.end() ; ++itr )
				{
					g_kPvPLobbyMgr.AddLobby( itr->first, itr->second );
				}
				
				g_kPvPRankingMgr.Init( kVecPvPRanking );
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV0, __FL__ << L"Recv BadPacket<PT_N_T_ANS_GET_PVPLOBBY_INFO>" ); 
			}
		}break;
	case PT_IM_A_NFY_CHANNEL_NOTICE:
		{
			//size_t const iRdPos = pkPacket->RdPos();
			g_kRealmMgr.ReadFromPacket(*pkPacket);
			//pkPacket->RdPos(iRdPos);
			//BM::Stream kDPacket(PT_IM_A_NFY_CHANNEL_NOTICE);
			//kDPacket.Push(*pkPacket);
			//SendToServerType(CEL::ST_CENTER, kDPacket);
		}break;
	case PT_SYNC_DEF_PLAYERPLAYTIME:
		{
			g_kDefPlayTime.ReadFromPacket(*pkPacket);
			pkPacket->PosAdjust();
			SendToServerType(CEL::ST_MAP,*pkPacket);
		}break;
	case PT_M_M_UPDATE_PLAYERPLAYTIME:
		{
			g_kServerSetMgr.Locked_Recv_PT_M_M_UPDATE_PLAYERPLAYTIME(pkPacket);
		}break;
	case PT_N_T_NFY_TRADE:
		{
			BM::Stream kPacket(PT_N_T_NFY_TRADE, *pkPacket);
			::SendToServerType(CEL::ST_MAP, kPacket);
		}break;
	case PT_N_T_NFY_RECOVERY_STRATEGY_FATIGUABILITY:
		{	// Аь·« ЗЗ·Оµµ ГК±вИ­.
			BM::Stream NfyPacket(PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY, *pkPacket);
			::SendToServerType(CEL::ST_MAP, NfyPacket);
		}break;
	case PT_N_T_PRE_MAKE_PARTY_EVENT_GROUND:
		{
			
		}break;
	case PT_N_T_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE:
		{
			BM::Stream ToMapPacket(PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE, *pkPacket);
			::SendToServerType(CEL::ST_MAP, ToMapPacket);
		}break;
	case PT_N_T_NFY_EVENT_GROUND_USER_COUNT_MODIFY:
		{
			BM::Stream ToMapPacket(PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY, *pkPacket);
			::SendToServerType(CEL::ST_MAP, ToMapPacket);
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
			// Imm їЎ°Ф ЗцАз БўјУАЪ Б¤єёё¦ АьјЫЗШ БШґЩ.
			BM::Stream kLPacket(PT_T_IM_NFY_USERLIST);
//			g_kServerSetMgr.Locked_WriteToPacket_User(kLPacket, PgServerSetMgr::ESSMP_SSwitchPlayerData);
			pkSession->VSend(kLPacket);
		}break;
	case CEL::ST_CONTENTS:
		{
			BM::Stream kPacket( PT_A_N_REQ_GAMETIME );
			pkSession->VSend( kPacket );
			pkSession->VSend(BM::Stream(PT_A_N_REQ_BATTLE_PASS_INFO));
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

void BuildDef()
{//
	SReloadDef kReloadDef;
	int iLoadDef = 
		PgControlDefMgr::EDef_MonsterDefMgr
		|	PgControlDefMgr::EDef_SkillDefMgr
		|	PgControlDefMgr::EDef_ItemDefMgr
		|	PgControlDefMgr::EDef_EffectDefMgr
		|	PgControlDefMgr::EDef_RareDefMgr
		|	PgControlDefMgr::EDef_ItemEnchantDefMgr
		|	PgControlDefMgr::EDef_ClassDefMgr
		|	PgControlDefMgr::EDef_ItemBagMgr
		|	PgControlDefMgr::EDef_StoreMgr
		|	PgControlDefMgr::EDef_SpendMoneyMgr
		|	PgControlDefMgr::EDef_DynamicDefMgr
		|	PgControlDefMgr::EDef_ItemOptionMgr
		|	PgControlDefMgr::EDef_GenPointMgr
		|	PgControlDefMgr::EDef_ExtMgr
		|	PgControlDefMgr::EDef_ItemSetDefMgr
		|	PgControlDefMgr::EDef_ObjectDefMgr
		|	PgControlDefMgr::EDef_PropertyMgr
		|	PgControlDefMgr::EDef_GroundEffect
		|	PgControlDefMgr::EDef_OnlyFirst
		|	PgControlDefMgr::EDef_RecommendationItem
		|	PgControlDefMgr::EDef_PremiumMgr
		|	PgControlDefMgr::EDef_Default;

	g_kTblDataMgr.GetReloadDef(kReloadDef, iLoadDef);
	if (!g_kControlDefMgr.Update(kReloadDef, iLoadDef))
	{
		ASSERT_LOG(false, BM::LOG_LV2, __FL__ << _T("Def Build failed"));
	}

	g_kMissionMgr.Build(*kReloadDef.pkMissionResult, *kReloadDef.pkMissionCandi, *kReloadDef.pkMissionRoot);
}



