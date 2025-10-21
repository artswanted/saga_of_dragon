#include "StdAfx.h"
#include "Lohengrin/GMCommand.h"
#include "PgGMProcessMgr.h"
#include "PgServerSetMgr.h"
#include "JobDispatcher.h"
#include "PgSendWrapper.h"
#include "Variant/PgStringUtil.h"
#include "PgDBProcess.h"
#include <hotmeta/hotmeta.h>

PgGMProcessMgr::PgGMProcessMgr(void)
{
}

PgGMProcessMgr::~PgGMProcessMgr(void)
{
}

void PgGMProcessMgr::FirstAnalysisGmOder(SGmOrder &rkOrderData)
{
	switch(rkOrderData.Type)
	{
	case ORDER_DELETE_CASH_GIFT:
		{
			CEL::DB_QUERY kQueryGift(DT_PLAYER,DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[UP_CS_DELETEGIFT]");
			kQueryGift.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryGift.QueryOwner(rkOrderData.kCmdGuid);
			kQueryGift.PushStrParam(rkOrderData.kGuid[0]);
			g_kCoreCenter.PushQuery(kQueryGift);
		}break;
	case ORDER_RETURN_MAIL:
		{
			CEL::DB_QUERY kQueryGift(DT_PLAYER,DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[up_Post_ConvertToReturnMail]");
			kQueryGift.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryGift.QueryOwner(rkOrderData.kCmdGuid);
			kQueryGift.PushStrParam(rkOrderData.kGuid[0]);
			kQueryGift.PushStrParam(BM::GUID::Create());
			g_kCoreCenter.PushQuery(kQueryGift);
		}break;
	case ORDER_MARKET_CLOSE:
		{
			g_kRealmUserMgr.Locked_RecvORDER_MARKET_CLOSE(rkOrderData.kGuid[0]);
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_CLEAR_MISSION_RANK:
		{
			const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;
			g_kTblDataMgr.GetContDef(pContDefMission);
			if(NULL == pContDefMission)
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				return;
			}

			int iMissionKey = rkOrderData.iValue[0];

			if(iMissionKey >= 0)
			{
				CONT_DEF_MISSION_ROOT::const_iterator mission_itor = pContDefMission->find(rkOrderData.iValue[0]);
				if(mission_itor == pContDefMission->end())
				{
					SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
					return;
				}

				iMissionKey = (*mission_itor).second.iKey;
			}

			CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[up_ClearMissionRank]");
			kQueryItem.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryItem.QueryOwner(rkOrderData.kCmdGuid);
			kQueryItem.PushStrParam(iMissionKey);
			g_kCoreCenter.PushQuery(kQueryItem);
		}break;
	case ORDER_CREATE_GROUP_MAIL:
		{
			std::wstring const kToken(L"\r\n");
			size_t kOffset = 0;
			std::wstring kFrom, kTitle;
			std::wstring kTmp = rkOrderData.wsString1;

			size_t kIndex = kTmp.find(kToken.c_str(),kOffset);
			if(std::wstring::npos == kIndex)
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				return;
			}

			kFrom.assign(kTmp,kOffset,kIndex - kOffset);
			kOffset = kIndex + kToken.length();

			kTitle.assign(kTmp,kOffset,kTmp.length() - kOffset);

			CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[up_Admin_CreateGroupMail]");
			kQueryItem.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryItem.QueryOwner(rkOrderData.kCmdGuid);

			kQueryItem.PushStrParam(rkOrderData.kGuid[0]);	// 메일 발송 GUID
			kQueryItem.PushStrParam(kTitle);				// 타이틀
			kQueryItem.PushStrParam(rkOrderData.wsString2);	// 텍스트
			kQueryItem.PushStrParam(rkOrderData.biValue[1]);// 클레스
			kQueryItem.PushStrParam(rkOrderData.iValue[1]);	// 최소 랩
			kQueryItem.PushStrParam(rkOrderData.iValue[2]);	// 최대 랩
			kQueryItem.PushStrParam(rkOrderData.iValue[3]);	// 아이템 번호
			kQueryItem.PushStrParam(rkOrderData.iValue[4]);	// 아이템 카운트
			kQueryItem.PushStrParam(rkOrderData.biValue[0]);// 지급 머니
			kQueryItem.PushStrParam(rkOrderData.dtTime[0]);	// 발송 시작 시간
			kQueryItem.PushStrParam(rkOrderData.dtTime[1]);	// 발송 종료 시간
			kQueryItem.PushStrParam(kFrom);					// 메일 발송자 이름 GUID

			g_kCoreCenter.PushQuery(kQueryItem);
		}break;
	case ORDER_DELETE_GROUP_MAIL:
		{
			CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[up_Admin_DeleteGroupMail]");
			kQueryItem.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryItem.QueryOwner(rkOrderData.kCmdGuid);
			kQueryItem.PushStrParam(rkOrderData.kGuid[0]);	// 삭제할 메일 GUID
			g_kCoreCenter.PushQuery(kQueryItem);
		}break;
	case ORDER_CREATE_MAIL:
		{
			std::wstring const kToken(L"\r\n");
			size_t kOffset = 0;
			std::wstring kFrom,kTo,kTitle,kText;
			std::wstring kTmp = rkOrderData.wsString1;

			size_t kIndex = kTmp.find(kToken.c_str(),kOffset);
			if(std::wstring::npos == kIndex)
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				return;
			}

			kFrom.assign(kTmp,kOffset,kIndex - kOffset);
			kOffset = kIndex + kToken.length();

			kIndex = kTmp.find(kToken.c_str(),kOffset);
			if(std::wstring::npos == kIndex)
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				return;
			}

			kTo.assign(kTmp,kOffset,kIndex - kOffset);
			kOffset = kIndex + kToken.length();

			kTitle.assign(kTmp,kOffset,kTmp.length() - kOffset);

			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL_ORDER;
			pkActionOrder->kPacket2nd.Push(kFrom);
			pkActionOrder->kPacket2nd.Push(kTo);
			pkActionOrder->kPacket2nd.Push(kTitle);
			pkActionOrder->kPacket2nd.Push(rkOrderData.wsString2);
			pkActionOrder->kPacket2nd.Push(rkOrderData.iValue[2]);
			pkActionOrder->kPacket2nd.Push(static_cast<short>(rkOrderData.iValue[3]));
			pkActionOrder->kPacket2nd.Push(rkOrderData.iValue[1]);
			pkActionOrder->kPacket2nd.Push(rkOrderData.kCmdGuid);

			g_kJobDispatcher.VPush(pkActionOrder);
		}break;
	case ORDER_DELETE_MAIL:
		{
			CEL::DB_QUERY kQueryItem( DT_PLAYER, DQT_CHANGE_GM_LEVEL,L"EXEC [dbo].[up_Change_UserMail_Delete]");
			kQueryItem.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryItem.QueryOwner(rkOrderData.kCmdGuid);

			kQueryItem.PushStrParam(rkOrderData.kGuid[0]);
			kQueryItem.PushStrParam(static_cast<int>(0));

			g_kCoreCenter.PushQuery(kQueryItem);
		}break;
	case ORDER_CHANGE_NAME: 
	case ORDER_CHANGE_STATE:
	case ORDER_CHANGE_ITEM_INFO:
	case ORDER_CHANGE_SKILL_INFO:
	case ORDER_CHANGE_POS:
	case ORDER_CHANGE_QUEST_INFO:
	case ORDER_CHANGE_CHARACTER_FACE:
	case ORDER_CHANGE_CP:
	case ORDER_CHANGE_GM_LEVEL:
	case ORDER_CHANGE_ACHIEVEMENT:
	case ORDER_CHANGE_QUEST_ENDED:
		{
			//유저가 접속중인지 확인
			SContentsUser kPlayer;
			if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(rkOrderData.kGuid[0], true, kPlayer))
			{// 유저가 접속중일 경우 오더 실패하고 에러코드(GE_USING_ID) 업데이트
				SendOrderFailed(rkOrderData.kCmdGuid, GE_USING_ID);
			}
			else
			{
				//바로 DB에 쓴다.
				AnalysisOderData(rkOrderData);
			}
		}break;
	case ORDER_CHANGE_GUILD_INFO:
		{
			BM::GUID const kGuildGuid(rkOrderData.wsString2);
			{//Rename
				PgStringUtil::Trim<std::wstring>( rkOrderData.wsString1, L" " );
				BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW);
				kPacket.Push(BM::GUID::NullData());
				kPacket.Push( static_cast<BYTE>(GC_GM_Rename) );
				kPacket.Push(kGuildGuid);
				kPacket.Push(rkOrderData.wsString1);
				SendToGuildMgr(kPacket);
			}
			{//ChangeOwner
				BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW);
				kPacket.Push(BM::GUID::NullData());
				kPacket.Push( static_cast<BYTE>(GC_GM_ChangeOwner) );
				kPacket.Push( kGuildGuid ); // GuildGuid
				kPacket.Push( rkOrderData.kGuid[0] ); // Old OwnerGuid
				kPacket.Push( rkOrderData.kGuid[1] ); // New OwnerGuid
				SendToGuildMgr(kPacket);
			}
			{//Level
				PgStringUtil::Trim<std::wstring>( rkOrderData.wsString1, L" " );
				BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW);
				kPacket.Push(BM::GUID::NullData());
				kPacket.Push( static_cast<BYTE>(GC_GM_SetLv) );
				kPacket.Push(kGuildGuid);
				kPacket.Push(rkOrderData.iValue[0]);
				SendToGuildMgr(kPacket);
			}
			{//Exp
				PgStringUtil::Trim<std::wstring>( rkOrderData.wsString1, L" " );
				BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW);
				kPacket.Push(BM::GUID::NullData());
				kPacket.Push( static_cast<BYTE>(GC_GM_SetExp) );
				kPacket.Push(kGuildGuid);
				kPacket.Push(rkOrderData.biValue[1]);
				SendToGuildMgr(kPacket);
			}
			{//Money
				PgStringUtil::Trim<std::wstring>( rkOrderData.wsString1, L" " );
				BM::Stream kPacket(PT_T_N_ANS_GUILD_COMMAND_RAW);
				kPacket.Push(BM::GUID::NullData());
				kPacket.Push( static_cast<BYTE>(GC_GM_SetMoney) );
				kPacket.Push(kGuildGuid);
				kPacket.Push(rkOrderData.biValue[0]);
				SendToGuildMgr(kPacket);
			}

			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_CHANGE_PARTY_NAME:
		{
//			INFO_LOG(BM::LOG_LV5, _T("[%s][ORDER_CHANGE_PARTY_NAME] Cannot Handle ChannelContents...."), __FUNCTIONW__);
			
			BM::Stream kPacket(PT_C_N_REQ_PARTY_RENAME_GM, (BYTE)rkOrderData.iValue[0]);
			switch(rkOrderData.iValue[0])
			{
			case 1: kPacket.Push(rkOrderData.kGuid[1]);break;
			case 2: kPacket.Push(rkOrderData.kGuid[0]);break;
			case 3: kPacket.Push(rkOrderData.wsString1);break;
			default: break;
			}
			kPacket.Push(rkOrderData.wsString2);
			kPacket.Push(0);

			SContentsUser kPlayer;
			if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(rkOrderData.kGuid[0], true, kPlayer))
			{
				SendToGlobalPartyMgr(kPlayer.sChannel, kPacket);
			}

			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);			
		}break;
	case ORDER_EMPORIA_OPEN:
		{
			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, GMCMD_EMPORIA_OPEN );
			kPacket.Push( rkOrderData.kCmdGuid );
			kPacket.Push( rkOrderData.kGuid[0]  );
			kPacket.Push( rkOrderData.dtTime[0] );
			kPacket.Push( rkOrderData.iValue[0] );
			kPacket.Push( rkOrderData.iValue[1] );
			kPacket.Push( rkOrderData.iValue[2] );
			SendToRealmContents( PMET_EMPORIA, kPacket );
		}break;
	case ORDER_EMPORIA_CLOSE:
		{
			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, GMCMD_EMPORIA_CLOSE );
			kPacket.Push( rkOrderData.kCmdGuid );
			kPacket.Push( rkOrderData.kGuid[0]  );
			SendToRealmContents( PMET_EMPORIA, kPacket );
		}break;
	case ORDER_EMPORIA_STARTTIME_CHANGE:
		{
			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, GMCMD_EMPORIA_BATTLE_START );
			kPacket.Push( rkOrderData.kCmdGuid );
			kPacket.Push( rkOrderData.kGuid[0] );
			kPacket.Push( rkOrderData.dtTime[0] );
			SendToRealmContents( PMET_EMPORIA, kPacket );
		}break;
	case ORDER_EMPORIA_OWNER_CHANGE:
		{
			BM::Stream kPacket( PT_A_N_REQ_EMPORIA_GM_COMMAND, GMCMD_EMPORIA_OWNER_CHANGE );
			kPacket.Push( rkOrderData.kCmdGuid );
			kPacket.Push( rkOrderData.kGuid[0]  );
			kPacket.Push( rkOrderData.kGuid[1]  );// GuildID
			kPacket.Push( rkOrderData.wsString1 );// GuildName
			kPacket.Push( rkOrderData.iValue[0] );// GuildEmblem
			kPacket.Push( rkOrderData.iValue[1] );// Emporia Grade
			SendToRealmContents( PMET_EMPORIA, kPacket );
		}break;
	case ORDER_OXQUIZEVENT_RELOAD:
		{
			BM::Stream kPacket(PT_REQ_GMCMD_OXQUIZ_RELOAD);

			SERVER_IDENTITY kSI;
			SGroundKey kGndKey;
			kSI.WriteToPacket(kPacket);
			kPacket.Push(kGndKey);
			rkOrderData.WriteToPacket(kPacket);

			SendToOXGuizEvent(kPacket);
		}break;
	case ORDER_LUCKYSTAREVENT_RELOAD:
		{
			BM::Stream kPacket(PT_REQ_GMCMD_LUCKYSTAR_RELOAD);
			rkOrderData.WriteToPacket(kPacket);

			SendToLuckyStarEvent(kPacket);
		}break;
	case ORDER_LUCKYSTAR_UPDATE_EVENT:
		{
			BM::Stream kPacket(PT_REQ_GMCMD_LUCKYSTAR_UPDATE);
			rkOrderData.WriteToPacket(kPacket);
			kPacket.Push( rkOrderData.iValue[0] );
			kPacket.Push( rkOrderData.iValue[1] );

			SendToLuckyStarEvent(kPacket);			
		}break;
	case ORDER_EVENT_QUEST_RELOAD:
		{
			BM::Stream kPacket(PT_N_N_REQ_RELOAD_EVENT_QUEST);
			::SendToRealmContents(PMET_EVENTQUEST, kPacket);
			//SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_BATTLE_SQUARE_RELOAD:
		{
			BM::Stream kPacket(PT_A_N_REQ_RELOAD_BS_GAME);
			rkOrderData.WriteToPacket(kPacket);
			::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
		}break;
	case ORDER_SUSPEND_BATTLESQUARE:
		{
			BM::Stream kPacket(PT_A_N_NFY_BS_SUSPEND);
			rkOrderData.WriteToPacket(kPacket);
			::SendToRealmContents(PMET_BATTLESQUARE, kPacket);
		}break;
	case ORDER_CASHSHOP_TABLE_RELOAD:
		{
			BM::Stream kPacket(PT_A_N_REQ_RELOAD_DATA);
			kPacket.Push(rkOrderData.kCmdGuid);
			kPacket.Push(rkOrderData.kCmdGuid);
			SendToRealmContents( PMET_RELOAD_DATA, kPacket );
		}break;
	case ORDER_GAMBLEMACHINE_RELOAD:
		{
			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_DEF_GAMBLEMACHINE, _T("EXEC [dbo].[up_LoadDefGambleMachine]"));
			kQuery.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.contUserData.Push(true);
			g_kCoreCenter.PushQuery(kQuery);
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_EVENT_ITEM_REWARD_RELOAD:
		{
			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_EVENT_ITEM_REWARD, _T("EXEC [dbo].[up_LoadEventItemReward]"));
			kQuery.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.contUserData.Push(true);
			g_kCoreCenter.PushQuery(kQuery);
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_RELOAD_META:
		{
			hotmeta::load();
		}break;
	case ORDER_CHANGE_PLAYERPLAYTIME:
		{
			switch( rkOrderData.iValue[0] )
			{
			case 2:
				{//리셋
					if( !rkOrderData.wsString1.empty() )
					{
						BM::Stream kPacket;						
						kPacket.Push( BM::GUID::NullData() );
						kPacket.Push( rkOrderData.wsString1 );
						kPacket.Push( rkOrderData.iValue[1] );
						kPacket.Push( rkOrderData.iValue[2] );
						kPacket.Push( static_cast<bool>(rkOrderData.iValue[3]) );
						g_kRealmUserMgr.Locked_RecvTaskProcessPlayerPlayTime(PT_M_I_GMCMD_SETPLAYERPLAYTIME, &kPacket);
					}
					SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
				}break;
			default:
				{
					SendOrderFailed(rkOrderData.kCmdGuid, GE_DATA_NOT_FOUND);
				}break;
			}
		}break;
	case ORDER_TREASURE_CHEST_RELOAD:
		{
			//보물상자 테이블은 LOCAL과 DEF에서 동일한 디자인으로 2개가 정의되어있다
			//로드할 때 하나로 통합된다.
			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LOAD_TREASURE_CHEST, _T("EXEC [dbo].[up_LoadTreasureChest]"));
			kQuery.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.contUserData.Push(true);
			g_kCoreCenter.PushQuery(kQuery);
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);

			CEL::DB_QUERY kQueryDef( DT_DEF, DQT_LOAD_TREASURE_CHEST, _T("EXEC [dbo].[up_LoadDefTreasureChest]"));
			kQueryDef.InsertQueryTarget(rkOrderData.kCmdGuid);
			kQueryDef.QueryOwner(rkOrderData.kCmdGuid);
			kQueryDef.contUserData.Push(true);
			g_kCoreCenter.PushQuery(kQueryDef);
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_PREMIUM_SERVICE_INSERT:
		{
			SContentsUser kPlayer;
			if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(rkOrderData.kGuid[0], true, kPlayer))
			{
				CONT_PLAYER_MODIFY_ORDER		kOrder;
				kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_INSERT, kPlayer.kCharGuid, SPMOD_PremiumService(rkOrderData.iValue[0])));

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(rkOrderData.kGuid[0]);
				pkActionOrder->kCause = CIE_PREMIUM_SERVICE;
				pkActionOrder->kContOrder = kOrder;
				g_kJobDispatcher.VPush(pkActionOrder);
			}
			else
			{ //바로 DB에 쓴다.
				AnalysisOderData(rkOrderData);
			}
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_PREMIUM_SERVICE_MODIFY:
		{
			SContentsUser kPlayer;
			SUserInfoExt kExt;
			if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(rkOrderData.kGuid[0], true, kPlayer, &kExt))
			{
				int const iUseDate = rkOrderData.iValue[1] * 24 * 60;
				CONT_PLAYER_MODIFY_ORDER		kOrder;
				kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_MODIFY, kPlayer.kCharGuid, SPMOD_PremiumServiceModify(kExt.iPremiumNo,iUseDate)));

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(rkOrderData.kGuid[0]);
				pkActionOrder->kCause = CIE_PREMIUM_SERVICE;
				pkActionOrder->kContOrder = kOrder;
				g_kJobDispatcher.VPush(pkActionOrder);
			}
			else
			{ //바로 DB에 쓴다.
				AnalysisOderData(rkOrderData);
			}
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	case ORDER_PREMIUM_SERVICE_REMOVE:
		{
			SContentsUser kPlayer;
			if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(rkOrderData.kGuid[0], true, kPlayer))
			{
				CONT_PLAYER_MODIFY_ORDER		kOrder;
				kOrder.push_back(SPMO(IMET_PREMIUM_SERVICE_REMOVE, kPlayer.kCharGuid));

				SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
				pkActionOrder->InsertTarget(rkOrderData.kGuid[0]);
				pkActionOrder->kCause = CIE_PREMIUM_SERVICE;
				pkActionOrder->kContOrder = kOrder;
				g_kJobDispatcher.VPush(pkActionOrder);
			}
			else
			{ //바로 DB에 쓴다.
				AnalysisOderData(rkOrderData);
			}
			SendOrderState(rkOrderData.kCmdGuid, OS_DONE);
		}break;
	default:
		{
			//이거 외의 넘이 들어왔으면 문제 있는거임.
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Invalid OrderType [") << rkOrderData.Type << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}

bool PgGMProcessMgr::AnalysisOderData(SGmOrder &rkOrderData)
{
	switch(rkOrderData.Type)
	{
	case ORDER_CHANGE_NAME: 
		{
			// GMLevel을 먼저 얻어 오고, GMLevel 있으면 CharacterName FilterString 적용하자.
			CEL::DB_QUERY kQuery(DT_MEMBER, DQT_GMORDER_SELECT_MEMBER_GMLEVEL, L"EXEC [dbo].[up_CS_SelectMemberGMLevel]" );
			rkOrderData.WriteToPacket(kQuery.contUserData);

			kQuery.InsertQueryTarget(rkOrderData.kGuid[0]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberGuid
			g_kCoreCenter.PushQuery(kQuery);

			/*
			std::wstring kNewName = rkOrderData.wsString1;
			if( false == g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, kNewName)
			||	true == g_kFilterString.Filter(kNewName, false, FST_ALL) )
			{
				SendOrderState(rkOrderData.kCmdGuid, OS_RESERVE);
				break;
			}

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CHARACTER_NAME, L"EXEC [dbo].[UP_Change_Character_Name2]" );
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberGuid
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// CharacterGuid
			kQuery.PushStrParam(rkOrderData.wsString1);	// NewName
			g_kCoreCenter.PushQuery(kQuery);
			*/
		}break;
	case ORDER_CHANGE_STATE:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CHARACTER_STATE, L"EXEC [dbo].[UP_Change_Character_State2]" );
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);					//MemberId
			kQuery.PushStrParam(rkOrderData.kGuid[1]);					//CharacterId
			kQuery.PushStrParam((BYTE)rkOrderData.iValue[0]);			//Gender
			kQuery.PushStrParam(rkOrderData.iValue[1]);					//Race
			kQuery.PushStrParam((BYTE)rkOrderData.iValue[2]);			//Class
			kQuery.PushStrParam((short)rkOrderData.iValue[3]);			//Lv
			kQuery.PushStrParam(rkOrderData.biValue[0]);				//Exp
			kQuery.PushStrParam(rkOrderData.biValue[1]);				//Money
			kQuery.PushStrParam((int)rkOrderData.biValue[2]);			//HP
			kQuery.PushStrParam((int)rkOrderData.biValue[3]);			//MP
			kQuery.PushStrParam((short)rkOrderData.iValue[4]);			//Skill Point
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ORDER_CHANGE_POS:
		{
			CONT_DEFMAP const* pkDefMap = NULL;
			g_kTblDataMgr.GetContDef(pkDefMap);
			if( !pkDefMap )
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				break;
			}

			CONT_DEFMAP::const_iterator map_iter = pkDefMap->find(rkOrderData.iValue[0]);
			if( pkDefMap->end() == map_iter )
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				break;
			}

			if( 0 != ((*map_iter).second.iAttr & GKIND_INDUN) )
			{
				SendOrderFailed(rkOrderData.kCmdGuid, GE_SYSTEM_ERR);
				break;
			}

			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CHARACTER_POS, L"EXEC [dbo].[UP_Change_Character_Pos2]" );
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberGuid
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character Guid
			kQuery.PushStrParam(rkOrderData.iValue[0]);	// RecentMap
			kQuery.PushStrParam(rkOrderData.iValue[1]);	// RecentLocX
			kQuery.PushStrParam(rkOrderData.iValue[2]);	// RecentLocY
			kQuery.PushStrParam(rkOrderData.iValue[3]); // RecentLocZ
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ORDER_CHANGE_GM_LEVEL:
		{
// 			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_GM_LEVEL, L"EXEC [dbo].[UP_Change_GM_Level2]" );
// 			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
// 			kQuery.QueryOwner(rkOrderData.kCmdGuid);
// 			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberGuid
// 			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// CharacterGuid
// 			kQuery.PushStrParam((BYTE)rkOrderData.iValue[0]);	// GMLevel
// 			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ORDER_CHANGE_ITEM_INFO:
		{
		}break;
	case ORDER_CHANGE_SKILL_INFO:
		{
			if(0 == rkOrderData.iValue[0])
			{
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_SKILL_INFO, L"EXEC [dbo].[up_Change_Character_Skill]");
				kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
				kQuery.QueryOwner(rkOrderData.kCmdGuid);
				kQuery.PushStrParam(rkOrderData.wsString2);	// ProcMethod
 				kQuery.PushStrParam(rkOrderData.wsString1);	// SkillNotext
				kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberID
				kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character ID
				kQuery.PushStrParam((short)rkOrderData.iValue[0]);	// SkillPoint
				g_kCoreCenter.PushQuery(kQuery);
			}
			else
			{
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SELECT_CHARA_SKILL,L"EXEC dbo.[UP_SelectCharacterSkill]");
				kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
				kQuery.QueryOwner(rkOrderData.kCmdGuid);
				kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character ID
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case ORDER_CHANGE_QUEST_INFO:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_QUEST_INFO, L"EXEC [dbo].[up_Change_Character_IngQuest]");
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character Guid
			kQuery.PushStrParam(rkOrderData.wsString1, false);	// IngQuest
			g_kCoreCenter.PushQuery(kQuery); 
		}break;
	case ORDER_CHANGE_QUEST_ENDED:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_QUEST_ENDED, L"EXEC [dbo].[UP_Change_Character_EndQuest3]");
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[1]); // Character Guid
			kQuery.PushStrParam(rkOrderData.wsString1, false); // EndQuest
			kQuery.PushStrParam(rkOrderData.wsString2, false); // EndQuest2
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ORDER_CHANGE_CHARACTER_FACE:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CHARACTER_FACE, L"EXEC [dbo].[up_Change_Character_Face]");
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// Member Guid
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character Guid
			kQuery.PushStrParam(rkOrderData.iValue[0]);	// hariColor
			kQuery.PushStrParam(rkOrderData.iValue[1]);	// HariStyle
			kQuery.PushStrParam(rkOrderData.iValue[2]);	// Face
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case ORDER_CHANGE_CP:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CP, L"EXEC [dbo].[up_Change_Character_CP2]");
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[0]);	// MemberGuid
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character ID
			kQuery.PushStrParam(rkOrderData.iValue[0]);	// iCP
			g_kCoreCenter.PushQuery(kQuery);			
		}break;
	case ORDER_CHANGE_ACHIEVEMENT:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_ACHIEVEMENT, L"EXEC [dbo].[up_Change_Character_Achievenment]");
			kQuery.InsertQueryTarget(rkOrderData.kGuid[1]);
			kQuery.QueryOwner(rkOrderData.kCmdGuid);
			kQuery.PushStrParam(rkOrderData.kGuid[1]);	// Character ID
			kQuery.PushStrParam(rkOrderData.wsString1);	// 업적 바이너리
			g_kCoreCenter.PushQuery(kQuery);			
		}break;
	case ORDER_PREMIUM_SERVICE_INSERT:
		{
			GET_DEF(PgDefPremiumMgr, kDefPremium);
			SPremiumData const* pkDefPremium = kDefPremium.GetDef(rkOrderData.iValue[0]);
			if(pkDefPremium)
			{
				SYSTEMTIME kLocalTime;
				g_kEventView.GetLocalTime(&kLocalTime);
				BM::DBTIMESTAMP_EX kStartDate = BM::DBTIMESTAMP_EX(kLocalTime);
				CGameTime::AddTime(kLocalTime, pkDefPremium->wUseDate * CGameTime::OneDay );
				BM::DBTIMESTAMP_EX kEndDate = BM::DBTIMESTAMP_EX(kLocalTime);

				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_PremiumService_Update]");
				kQuery.InsertQueryTarget(rkOrderData.kGuid[0]);
				kQuery.PushStrParam( rkOrderData.kGuid[0] );
				kQuery.PushStrParam( rkOrderData.iValue[0] );
				kQuery.PushStrParam( kStartDate );
				kQuery.PushStrParam( kEndDate );
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case ORDER_PREMIUM_SERVICE_MODIFY:
	case ORDER_PREMIUM_SERVICE_REMOVE:
		{
			CEL::DB_QUERY kQueryPremium(DT_MEMBER, DQT_GET_MEMBER_PREMIUM_SERVICE, L"EXEC [dbo].[up_GetPremiumService]");
			kQueryPremium.QueryOwner(rkOrderData.kGuid[0]);
			kQueryPremium.InsertQueryTarget(rkOrderData.kGuid[0]);
			kQueryPremium.PushStrParam( rkOrderData.kGuid[0] );	// 인자값:멤버GUID
			kQueryPremium.contUserData.Push(EPQT_GM_ORDER);
			kQueryPremium.contUserData.Push(rkOrderData.Type);
			kQueryPremium.contUserData.Push(rkOrderData.iValue[0]);
			g_kCoreCenter.PushQuery(kQueryPremium);
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Invalid OrderType [") << rkOrderData.Type << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return true;
}

void PgGMProcessMgr::RecvGMCommand(BM::Stream * const pkPacket )
{
	EGMCommandType eCommand;
	BM::GUID kReqGuid;
	pkPacket->Pop(eCommand);
	pkPacket->Pop(kReqGuid);
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Command[%d], ReqGuid[%s]"), __FUNCTIONW__, eCommand, kReqGuid.str().c_str());
	switch(eCommand)
	{
	case EGMC_MUTE:
		{
			std::wstring wstrName;
			int iSecTime = 0;
			pkPacket->Pop(wstrName);
			pkPacket->Pop(iSecTime);// 이값이 0이면 영구 채팅 금지, -1이면 해제

			//DB에 저장(그러나 일단은 Pass)

			SContentsUser kUserInfo;
			if(g_kRealmUserMgr.Locked_GetPlayerInfo(wstrName, kUserInfo) == S_OK)
			{
				if( !kUserInfo.kGndKey.IsEmpty() )
				{
					BM::Stream kPacket(PT_T_M_REQ_USER_MUTE);
					kPacket.Push(kUserInfo.kCharGuid);
					kPacket.Push(iSecTime);
					::SendToGround( kUserInfo.sChannel, kUserInfo.kGndKey, kPacket );
				}
			}
		}break;
	case EGMC_KICKUSER:
		{
			int iCase;
			BM::GUID kMemberID;
			pkPacket->Pop(iCase);
			pkPacket->Pop(kMemberID);

			SContentsUser kContUser;
			if ( S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kMemberID, true, kContUser) )
			{
				RealmUserManagerUtil::ProcessKickUser( iCase, kReqGuid, kContUser );
			}
			else
			{
				INFO_LOG( BM::LOG_LV7, __FL__ << _T("[EGMC_KICKUSER] Cannot find user MemberID<") << kMemberID << _T(">") );
				break;
			}
			
		}break;
	case EGMC_FREEZEACCOUNT:
		{
			BM::Stream kPacket(PT_GM_A_REQ_GMCOMMAND, eCommand);
			kPacket.Push(kReqGuid);
			kPacket.Push(*pkPacket);
			SendToImmigration(kPacket);
		}break;
	case EGMC_NOTICE:
		{
			std::wstring wstrContent;
			short sChannelNo = 0;

			pkPacket->Pop(sChannelNo);
			pkPacket->Pop(wstrContent);

			BM::Stream kPacket(PT_T_C_NFY_NOTICE);
			kPacket.Push(wstrContent);
			::SendToChannel( sChannelNo, kPacket );
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unknown GMCommand received CommandType[") << (int)eCommand << _T("]") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
}


void PgGMProcessMgr::SendOrderState(BM::GUID const &rkOrderId, unsigned short usState)
{
	BM::Stream kPacket(PT_A_GM_ANS_GMORDER);
	kPacket.Push(rkOrderId);
	kPacket.Push(usState);
	SendToImmigration(kPacket);
}

void PgGMProcessMgr::SendOrderFailed(BM::GUID const &rkOrderId, E_GM_ERR const eErrorCode)
{
	BM::Stream kPacket(PT_A_GM_ANS_FAILED_GMORDER);
	kPacket.Push(rkOrderId);
	kPacket.Push(eErrorCode);
	SendToImmigration(kPacket);
}


bool PgGMProcessMgr::CommonQueryResult( CEL::DB_RESULT &rkResult )
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
//		INFO_LOG(BM::LOG_LV7, _T("[%s]-[%d] Success"), __FUNCTIONW__, __LINE__);
		int iError = S_OK;
		
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iError);			++kIter;

		}

		if(S_OK == iError)
		{
			SendOrderState(rkResult.QueryOwner(), OS_DONE);
		}
		else
		{
			SendOrderFailed(rkResult.QueryOwner(), GE_QUERY_FAILED);
		}
		return true;
	}
	CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("[eRet = ") << rkResult.eRet << _T("]Query:[") << rkResult.Command() << _T("] Failed") );
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("[eRet = ") << rkResult.eRet << _T("]Query:[") << rkResult.Command() << _T("] Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_NAME( CEL::DB_RESULT &rkResult )
{
	return CommonQueryResult(rkResult);
}


bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_STATE( CEL::DB_RESULT &rkResult )
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_POS( CEL::DB_RESULT &rkResult )
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_GM_LEVEL(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}


bool PgGMProcessMgr::Q_DQT_CHANGE_CHARACTER_FACE(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_QUEST_INFO(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_SKILL_INFO(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_CP(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}

bool PgGMProcessMgr::Q_DQT_CHANGE_ACHIEVEMENT(CEL::DB_RESULT &rkResult)
{
	return CommonQueryResult(rkResult);
}


bool PgGMProcessMgr::ExecuteGmOrder(BM::GUID const &rkCmdGuid)
{
	CONT_GMORDER::iterator Itr = m_kContGMOrder.find(rkCmdGuid);
	if(Itr == m_kContGMOrder.end())
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Find not  GmOrder Data [OrderGuid: ") << rkCmdGuid.str().c_str() << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Find GmOrder"));
		return false;
	}

	CONT_GMORDER::mapped_type pkElement = Itr->second;
	if(pkElement)
	{
		SGmOrder kOrder= *pkElement;
		AnalysisOderData(kOrder);
		m_kContGMOrder.erase(rkCmdGuid);
		SAFE_DELETE(pkElement);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgGMProcessMgr::OnGMKickUser(bool bSuccess, int const iCase, BM::GUID const & rkOrderGuid, BM::GUID const &rkMemberGuid)
{
	if (bSuccess)
	{
		if(iCase == ECK_ORDER)
		{
			SendOrderState(rkOrderGuid, OS_NONE);
		}
		else
		{
			BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_KICKUSER);
			kPacket.Push(rkOrderGuid);
			kPacket.Push(GE_SUCCESS);
			SendToImmigration(kPacket);
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! bSuccess is false"));
		if(iCase == ECK_ORDER)
		{
			SendOrderFailed(rkOrderGuid, GE_SYSTEM_ERR);
		}
		else
		{
			BM::Stream kPacket(PT_A_GM_ANS_GMCOMMAND, EGMC_KICKUSER);
			kPacket.Push(rkOrderGuid);
			kPacket.Push(GE_ALREADY_EXIST);
			SendToImmigration(kPacket);
		}
	}
}

bool PgGMProcessMgr::Q_DQT_GMORDER_SELECT_MEMBER_GMLEVEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		BM::GUID kMemberGuid;
		BYTE byGMLevel;

		itor->Pop(kMemberGuid);	++itor;
		itor->Pop(byGMLevel); ++itor;
		SGmOrder kOrder;
		kOrder.ReadFromPacket(&rkResult.contUserData);
		std::wstring kNewName = kOrder.wsString1;
		if (byGMLevel == 0)
		{
			// GMLevel 이 없을 때만, CharacterName 검사 하자.
			if( false == g_kUnicodeFilter.IsCorrect(UFFC_CHARACTER_NAME, kNewName)
			||	true == g_kFilterString.Filter(kNewName, false, FST_ALL) )
			{
				SendOrderFailed(kOrder.kCmdGuid, GE_FORBIDDEN_NAME);
				return true;
			}
		}

		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_CHANGE_CHARACTER_NAME, L"EXEC [dbo].[UP_Change_Character_Name2]" );
		kQuery.InsertQueryTarget(kOrder.kGuid[1]);
		kQuery.QueryOwner(kOrder.kCmdGuid);
		kQuery.PushStrParam(kOrder.kGuid[0]);	// MemberGuid
		kQuery.PushStrParam(kOrder.kGuid[1]);	// CharacterGuid
		kQuery.PushStrParam(kOrder.wsString1);	// NewName
		g_kCoreCenter.PushQuery(kQuery);
		SendOrderState(kOrder.kCmdGuid, OS_DONE);
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("DB Query Failed Error=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
	return false;
}