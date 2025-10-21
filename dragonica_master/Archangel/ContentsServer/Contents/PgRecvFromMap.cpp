#include "Stdafx.h"
#include "PgRecvFromMap.h"
#include "PgGmProcessMgr.h"
#include "PgCouponEventDoc.h"
#include "PgServerSetMgr.h"
#include "JobDispatcher.h"
#include "PgIndunPartyMgr.h"
#include "PgAchievement.h"
#include "Variant/PgBattlePassMgr.h"

void CALLBACK Recv_PT_A_N_WRAPPED_PACKET( PACKET_ID_TYPE const usType, BM::Stream * const pkPacket )
{
	switch( usType )
	{
	case PT_M_N_NOTIFY_FIRST_LOGIN:
		{
			g_kRealmUserMgr.Recv_PT_M_N_NOTIFY_FIRST_LOGIN(*pkPacket);
		}break;
	case PT_M_N_TRY_TAKE_COUPON:
		{//쿠폰 발급 //쿠폰 타입도 있어야겠네.
			BM::Stream kCouponPacket;
			kCouponPacket.Push(E_CouponEvent_TakeCoupon);
			kCouponPacket.Push(*pkPacket);
			SendToCouponEventDoc(kCouponPacket);
			//연계 함수 만들어서 아이템 지급기까지.
		}break;
	case PT_M_A_GODCMD:
		{
			EGMCmdType eGodCommandType = GMCMD_NONE;
			pkPacket->Pop(eGodCommandType);
			OnRecvGodCommand( eGodCommandType, pkPacket );
		}break;
	case PT_M_N_NFY_NOTICE_PACKET:
		{
			E_NOTICE_TYPE	kType;
			VEC_GUID		kGuidList;
			pkPacket->Pop( kType );
			pkPacket->Pop( kGuidList );

			VEC_GUID::const_iterator guid_itr = kGuidList.begin();
			for ( ; guid_itr!=kGuidList.end() ; ++guid_itr )
			{
				BM::Stream kSendPacket;
				kSendPacket.Push(E_NoticeMsg_SendPacket);
				kSendPacket.Push(kType);
				kSendPacket.Push(*guid_itr);
				kSendPacket.Push(*pkPacket);
				SendToNotice(kSendPacket);
			}

		}break;
	case PT_C_O_ANS_SEND_GM_CHAT:
		{
			BM::Stream kPacket(PT_C_O_ANS_SEND_GM_CHAT);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	case PT_GM_A_REQ_GMCOMMAND:
		{
			g_kGMProcessMgr.RecvGMCommand(pkPacket);
		}break;
	case PT_M_O_REQ_PLAYERTIME_DEFEND:
		{
			BM::GUID kCharacterGuid;
			pkPacket->Pop(kCharacterGuid);

			g_kRealmUserMgr.Locked_UpdatePlayerTime(kCharacterGuid, pkPacket);
		}break;
	case PT_A_NFY_USER_DISCONNECT:
		{
			g_kRealmUserMgr.Locked_RecvPT_A_NFY_USER_DISCONNECT(pkPacket, true);
		}break;
	case PT_C_M_REQ_CLIENT_CUSTOMDATA:
		{
			g_kRealmUserMgr.Locked_RecvPT_C_M_REQ_CLIENT_CUSTOMDATA(pkPacket);
		}break;
	case PT_C_T_REQ_SAVE_OPTION:
		{
			BM::GUID kCharacterGuid;
			ClientOption kOption;
			pkPacket->Pop(kCharacterGuid);
			if(!pkPacket->Pop(kOption))
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot read ClientOption from packet Character=") << kCharacterGuid);
				break;
			}
			g_kRealmUserMgr.Locked_Recv_PT_C_T_REQ_SAVE_OPTION(kCharacterGuid, kOption);
		}break;
	case PT_N_T_REQ_JOBSKILL3_HISTORYITEM:
		{
			BM::GUID kCharacterGuid;
			int iSaveIdx = 0;
			if(pkPacket->Pop(kCharacterGuid))
			if(pkPacket->Pop(iSaveIdx))
			{
				g_kRealmUserMgr.Locked_Recv_PT_N_T_REQ_JOBSKILL3_HISTORYITEM(kCharacterGuid, iSaveIdx);
			}
		}break;
	case PT_C_T_REQ_SAVE_SKILLSET:
		{
			BM::GUID kCharacterGuid;
			pkPacket->Pop(kCharacterGuid);
			
			PgSaveSkillSetPacket kSavePacket;
			if(!kSavePacket.ReadFromPacket(*pkPacket))
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot read SkillSet from packet Character=") << kCharacterGuid);
				break;
			}
			g_kRealmUserMgr.Locked_Recv_PT_C_T_REQ_SAVE_SKILLSET(kCharacterGuid, kSavePacket.ContSkillSet());
		}break;
	case PT_M_N_REQ_SAVE_PENALTY:
		{
			BM::GUID kCharGuid;
			WORD wType = 0;
			int iValue = 0;
			int iPenaltyTimeSec = 0;
			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( wType );
			pkPacket->Pop( iValue );
			pkPacket->Pop( iPenaltyTimeSec );

			__int64 i64Time = g_kEventView.GetLocalSecTime( CGameTime::SECOND );

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SAVE_PENALTY, L"EXEC [dbo].[up_SavePenalty]");
			kQuery.InsertQueryTarget( kCharGuid );
			kQuery.QueryOwner( kCharGuid );

			kQuery.PushStrParam( kCharGuid );
			kQuery.PushStrParam( wType );
			kQuery.PushStrParam( iValue );

			BM::DBTIMESTAMP_EX kDBTime;
			CGameTime::SecTime2DBTimeEx( i64Time, kDBTime, CGameTime::SECOND );
			kQuery.PushStrParam( kDBTime );

			i64Time += static_cast<__int64>( iPenaltyTimeSec );
			CGameTime::SecTime2DBTimeEx( i64Time, kDBTime, CGameTime::SECOND );
			kQuery.PushStrParam( kDBTime );

			g_kCoreCenter.PushQuery( kQuery );
		}break;
	case PT_M_N_REQ_CHECK_PENALTY:
		{
			size_t const iRDPos = pkPacket->RdPos();

			short nChannel = 0;
			SGroundKey kGndkey;
			BM::GUID kCharGuid;
			WORD wType = 0;
			pkPacket->Pop( nChannel );
			pkPacket->Pop( kGndkey );
			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( wType );

			BM::DBTIMESTAMP_EX kDBTime;
			g_kEventView.GetLocalTime( kDBTime );

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_CHECK_PENALTY, L"EXEC [dbo].[up_CheckPenalty]");
			kQuery.InsertQueryTarget( kCharGuid );
			kQuery.QueryOwner( kCharGuid );

			kQuery.PushStrParam( kCharGuid );
			kQuery.PushStrParam( wType );
			kQuery.PushStrParam( kDBTime );

			pkPacket->RdPos( iRDPos );
			kQuery.contUserData.Swap( *pkPacket );

			g_kCoreCenter.PushQuery( kQuery );
		}break;
	case PT_M_I_GMCMD_PLAYERPLAYTIMESTEP:
	case PT_M_I_GMCMD_SETPLAYERPLAYTIME:
		{
			g_kRealmUserMgr.Locked_RecvTaskProcessPlayerPlayTime(usType, pkPacket);
		}break;
	case PT_A_N_REQ_COPY_THAT:
		{
			BM::GUID kCharGuid;
			std::wstring kCharName;

			pkPacket->Pop( kCharGuid );
			pkPacket->Pop( kCharName );

			if( 0 < kCharName.size() )
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_ADMIN_GM_COPYTHAT, L"EXEC [dbo].[UP_Admin_GM_CopyThat]" );
				kQuery.PushStrParam( kCharName );
				kQuery.InsertQueryTarget( BM::GUID::Create() );
				kQuery.QueryOwner( kCharGuid );
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case PT_M_T_REQ_REGIST_CHAT_BLOCK:
		{
			g_kRealmUserMgr.Locked_RegistChatBlock(pkPacket);
		}break;
	case PT_M_T_REQ_UNREGIST_CHAT_BLOCK:
		{
			g_kRealmUserMgr.Locked_UnRegistChatBlock(pkPacket);
		}break;
	case PT_M_T_REQ_CHANGE_CHAT_BLOCK_OPTION:
		{
			g_kRealmUserMgr.Locked_ChangeChatBlockOption(pkPacket);
		}break;
	case PT_M_N_PRE_MAKE_PARTY_EVENT_GROUND:
		{
			int ChannelNo = 0;
			pkPacket->Pop(ChannelNo);

			BM::Stream ToCenterPacket(PT_N_T_PRE_MAKE_PARTY_EVENT_GROUND);
			
			ToCenterPacket.Push(*pkPacket);

			SendToChannelContents(ChannelNo, PMET_PARTY, ToCenterPacket);
		}break;
	case PT_M_N_REQ_JOIN_ANOTHER_CHANNEL_PARTY:
		{
			int ChannelNo = 0;
			int GroundNo = 0;
			pkPacket->Pop(ChannelNo);
			pkPacket->Pop(GroundNo);

			BM::Stream ToMapPacket(PT_N_M_NFY_JOIN_ANOTHER_CHANNEL_PARTY);
			ToMapPacket.Push(*pkPacket);

			SGroundKey Key(GroundNo);
			SendToGround(ChannelNo, Key, ToMapPacket);
		}break;
	case PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE:
		{
			BM::Stream ToCenterPacket(PT_N_T_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToCenterPacket.Push(*pkPacket);

			SendToChannel(-1, ToCenterPacket);
		}break;
	case PT_M_N_NFY_EVENT_GROUND_USER_COUNT_MODIFY:
		{
			BM::Stream ToCenterPacket(PT_N_T_NFY_EVENT_GROUND_USER_COUNT_MODIFY);
			ToCenterPacket.Push(*pkPacket);

			SendToChannel(-1, ToCenterPacket);
		}break;
	case PT_C_M_REQ_INDUN_PARTY_LIST:
		{
			BM::GUID kCharGuid;
			BM::GUID kKeyGuid;
			int Type = 0;
			VEC_INT kContMapNo;
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kKeyGuid);
			pkPacket->Pop(Type);
			pkPacket->Pop(kContMapNo);

			g_kIndunPartyMgr.GetList(kCharGuid, kKeyGuid, Type, kContMapNo);
		}break;
	case PT_M_T_ANS_INDUN_PARTY_LIST:
		{
			g_kIndunPartyMgr.SyncIndunParty(pkPacket);
		}break;
	case PT_C_M_REQ_PARTY_LIST:
		{
			short nAliveChannelCount = 0;
			CONT_SERVER_HASH kServerInfo;
			g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CENTER, kServerInfo);
			CONT_SERVER_HASH::const_iterator server_it = kServerInfo.begin();
			while(server_it != kServerInfo.end())
			{
				if( (*server_it).second.pkSession && PUBLIC_CHANNEL_NUM!=(*server_it).second.nChannel )
				{
					++nAliveChannelCount;
				}
				++server_it;
			}
			
			BM::Stream kPacket(PT_C_M_REQ_PARTY_LIST);
			kPacket.Push(nAliveChannelCount);
			kPacket.Push(*pkPacket);
			
			server_it = kServerInfo.begin();
			while(server_it != kServerInfo.end())
			{
				if( (*server_it).second.pkSession )
				{
					SendToGlobalPartyMgr((*server_it).second.nChannel, kPacket);
				}
				++server_it;
			}
		}break;
	case PT_M_N_NFY_GENERIC_ACHIEVEMENT_NOTICE:
		{
			EAchievementType AchiType;
			pkPacket->Pop(AchiType);
			g_kAchievementMgr.Broadcast(AchiType, *pkPacket);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Unknow PacketType[") << usType << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

bool OnRecvGodCommand( EGMCmdType const eGodCommandType, BM::Stream * const pkPacket )
{
	//INFO_LOG(BM::LOG_LV5,_T("[%s] GodCmd Type[%d]"),__FUNCTIONW__,eGodCommandType);
	switch(eGodCommandType)
	{
	case GMCMD_MAPMOVE_RECALL:
	case GMCMD_MAPMOVE_TARGET:
	case GMCMD_MAPMOVE_TARGET_TOMAP:
		{
			BM::GUID kReqCharGuid;
			std::wstring wstrCharName;
			pkPacket->Pop( kReqCharGuid );
			pkPacket->Pop( wstrCharName );
			// 여기로 오면 채널에서 찾을 수 없는 경우이다.
			// 응답으로 채널정보만 알려주고 아무것도 하지 않아야 한다.

			BM::Stream kFailedPacket( PT_A_C_ANS_GODCMD_FAILED, eGodCommandType );
			kFailedPacket.Push( wstrCharName );

			SContentsUser kContentsUserInfo;
			if ( S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo( wstrCharName, kContentsUserInfo ) )
			{
				// 다른채널에 있네
				kFailedPacket.Push( kContentsUserInfo.sChannel );
			}
			else
			{
				// 로그인 하지 않았네
				kFailedPacket.Push( static_cast<short>(-1) );
			}

			g_kRealmUserMgr.Locked_SendToUser( kReqCharGuid, kFailedPacket, false );
		}break;
	case GMCMD_KICKUSER:
		{
			BM::GUID kReqCharGuid;
			std::wstring wstrKickUserName;
			pkPacket->Pop( kReqCharGuid );
			pkPacket->Pop( wstrKickUserName );

			SContentsUser kContentsUserInfo;
			if ( S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo( wstrKickUserName, kContentsUserInfo ) )
			{
				RealmUserManagerUtil::ProcessKickUser( ECK_GMC, kReqCharGuid, kContentsUserInfo );
			}
			else
			{
				BM::Stream kFailedPacket( PT_A_C_ANS_GODCMD_FAILED, eGodCommandType );
				kFailedPacket.Push( wstrKickUserName );
				kFailedPacket.Push( static_cast<short>(-1) );// 로그인 하지 않았네
				g_kRealmUserMgr.Locked_SendToUser( kReqCharGuid, kFailedPacket, false );
			}
		}break;
	case GODCMD_MTS_UM_ARTICLE_STATE:
		{
			BM::GUID kMemberGuid;
			int iGroundNo = 0;
			BYTE bState = 0;

			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(iGroundNo);
			pkPacket->Pop(bState);

			BM::Stream kGodPacket(PT_M_I_UM_DEV_REQ_SET_ARTICLE_STATE, kMemberGuid );
			kGodPacket.Push(bState);

			SendToItem(g_kProcessCfg.ServerIdentity(),SGroundKey(iGroundNo), kGodPacket );
		}break;
	case GODCMD_MTS_PVP_ROOMMODIFY:
		{
		}break;
	case GMCMD_EMPORIA_OPEN:
		{
			BM::GUID kEmporiaID;
			SYSTEMTIME kSystemTime;
			pkPacket->Pop( kEmporiaID );
			pkPacket->Pop( kSystemTime );
			BM::DBTIMESTAMP_EX kDBTime( kSystemTime );

			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, eGodCommandType );
			kPacket.Push(BM::GUID::NullData());
			kPacket.Push( kEmporiaID );
			kPacket.Push( kDBTime );
			kPacket.Push( *pkPacket );
			::SendToRealmContents( PMET_EMPORIA, kPacket );

		}break;
	case GMCMD_EMPORIA_CLOSE:
	case GMCMD_EMPORIA_BATTLE_START:
		{
			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, eGodCommandType );
			kPacket.Push(BM::GUID::NullData());
			kPacket.Push( *pkPacket );
			::SendToRealmContents( PMET_EMPORIA, kPacket );

		}break;
	case GMCMD_CASHITEMGIFT_ADD:
		{
			BM::GUID kGmCharGuid;
			int iArticleIdx = 0;
			BYTE bTimeType = 2;//일단위
			int iUseTime = 0;
			pkPacket->Pop(kGmCharGuid);
			pkPacket->Pop(iArticleIdx);
			pkPacket->Pop(bTimeType);
			pkPacket->Pop(iUseTime);

			TABLE_LOCK(CONT_DEF_CASH_SHOP_ARTICLE) kObjLock;
			g_kTblDataMgr.GetContDef(kObjLock);
			CONT_DEF_CASH_SHOP_ARTICLE const * pCont = kObjLock.Get();

			if(!pCont)
			{
				g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99004, EL_Warning, false );
				return true;
			}

			CONT_DEF_CASH_SHOP_ARTICLE::const_iterator iter = pCont->find(iArticleIdx);
			if(pCont->end() == iter)
			{
				g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99005, EL_Warning, false );
				return true;
			}

			CONT_DEF_CASH_SHOP_ARTICLE::mapped_type const & kArticle = (*iter).second;		
			
			CONT_CASH_SHOP_ITEM_PRICE::const_iterator priceiter = kArticle.kContCashItemPrice.find(TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(bTimeType,iUseTime));
			if(priceiter == kArticle.kContCashItemPrice.end())
			{
				g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 99005, EL_Warning, false );
				return true;
			}

			std::wstring kStartDate;
			std::wstring kEndDate;
			std::wstring kSender;
			std::wstring kMemo;
			std::wstring kMailTitle;
			std::wstring kMailBody;
			pkPacket->Pop(kStartDate);
			pkPacket->Pop(kEndDate);
			pkPacket->Pop(kSender);
			pkPacket->Pop(kMemo);
			pkPacket->Pop(kMailTitle);
			pkPacket->Pop(kMailBody);

			/*
			0x000001		캐릭터단위
			0x000002		레벨제한
			0x??0000		최소레벨
			0x00??00		최대레벨
			*/
			int iOption = 0;
			pkPacket->Pop(iOption);

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GMCMD_CASHITEMGIFT_INSERT, _T("EXEC [DBO].[UP_GMCMD_CashItemGift_Insert]"));
			kQuery.PushStrParam(iArticleIdx);
			kQuery.PushStrParam(bTimeType);
			kQuery.PushStrParam(iUseTime);
			kQuery.PushStrParam(kStartDate);
			kQuery.PushStrParam(kEndDate);
			kQuery.PushStrParam(kSender);
			kQuery.PushStrParam(kMemo);
			kQuery.PushStrParam(kMailTitle);
			kQuery.PushStrParam(kMailBody);
			kQuery.PushStrParam(iOption);

			kQuery.contUserData.Push(kGmCharGuid);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case GMCMD_CASHITEMGIFT_DEL:
		{
			BM::GUID kGmCharGuid;
			int iEventNo = 0;
			pkPacket->Pop(kGmCharGuid);
			pkPacket->Pop(iEventNo);

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_GMCMD_CASHITEMGIFT_DELETE, _T("EXEC [DBO].[UP_GMCMD_CashItemGift_Delete]"));
			kQuery.PushStrParam(iEventNo);
			kQuery.contUserData.Push(kGmCharGuid);
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case GMCMD_PVPLEAGUE_SETEVENT:
		{
			BM::Stream kPacket( PT_A_N_REQ_PVPLEAGUE_GM_COMMAND, eGodCommandType );
			kPacket.Push( *pkPacket );
			::SendToRealmContents( PMET_PVP_LEAGUE, kPacket );
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV2, __FL__ << _T("Type<") << eGodCommandType << _T("> Error!!") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return false;
		}break;
	}
	return true;
}