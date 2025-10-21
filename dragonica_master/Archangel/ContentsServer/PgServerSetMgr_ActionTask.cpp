#include "StdAfx.h"
#include "Lohengrin\ErrorCode.h"
#include "Variant\PgLogUtil.h"
#include "Variant\PgQuestInfo.h"
#include "Variant/PgSystemInvManager.h"
#include "Variant/PgQuestResetSchedule.h"
#include "Variant/PgEmporia.h"
#include "Variant/Global.h"
#include "PgServerSetMgr.h"
//#include "./Item/PgWorldItemManager.h"
#include "Contents/PgQuestRandom.h"
#include "Contents/PgQuestMng.h"
#include "Item/PgPostManager.h"
#include "global.h"
#include "PgActionEventProcess.h"
#include "Transaction.h"
#include "Item/PgCashShopManager.h"
#include "Item/CashManager.h"
#include "lohengrin\variablecontainer.h"
#include "PgGuildMgr.h"
#include "JobWorker.h"
#include "JobDispatcher.h"

void OnCheckAchievenmentRelation(BM::GUID const& kOwnerGuid, SPMOD_Complete_Achievement const& kData, PgAchievements const* pkAchievements);

namespace RealmUserManagerUtil
{
	void Push_DISCT_MODIFY_QUEST_EXT(SPMO const& kOrder, DB_ITEM_STATE_CHANGE_ARRAY &rkChangeArray, PgMyQuest const* pkMyQuest)
	{
		if( pkMyQuest )
		{
			tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_EXT, kOrder.Cause(), kOrder.OwnerGuid());
			kDBData.kAddonData.Push(pkMyQuest->DayLoopQuestTime());
			kDBData.kAddonData.Push(pkMyQuest->ContRandomQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedRandomQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedRandomQuestPlayerLevel());
			kDBData.kAddonData.Push(pkMyQuest->ContTacticsQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedTacticsQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedTacticsQuestPlayerLevel());
			kDBData.kAddonData.Push(pkMyQuest->ContWantedQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedWantedQuest());
			kDBData.kAddonData.Push(pkMyQuest->BuildedWantedQuestPlayerLevel());
			kDBData.kAddonData.Push(pkMyQuest->ContWantedQuestClearCount());
			kDBData.kAddonData.Push(pkMyQuest->ExistCanBuildWantedQuest());

			rkChangeArray.push_back(kDBData);
		}
	}

	void Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(SPMO const& kOrder, DB_ITEM_STATE_CHANGE_ARRAY &rkChangeArray, PgDoc_Player* pkCopyPlayer)
	{
		tagDBItemStateChange kDBData(DISCT_JOBSKILL_SAVE_EXHAUSTION, kOrder.Cause(), kOrder.OwnerGuid());
		kDBData.kAddonData.Push( pkCopyPlayer->JobSkillExpertness().CurExhaustion() );
		kDBData.kAddonData.Push( pkCopyPlayer->JobSkillExpertness().CurBlessPoint() );
		kDBData.kAddonData.Push( pkCopyPlayer->JobSkillExpertness().LastResetExhaustionTime() );
		kDBData.kAddonData.Push( pkCopyPlayer->JobSkillExpertness().LastResetBlessPointTime() );

		rkChangeArray.push_back(kDBData);
	}

	void SaveSkill(DB_ITEM_STATE_CHANGE const& kElement, CONT_DOC_PLAYER::mapped_type const& rkSecond)
	{
		SPlayerBinaryData kBinaryData;
		rkSecond->GetMySkill()->Save(MAX_DB_SKILL_SIZE, kBinaryData.abySkills);

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Skill2]");
		kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
		kQuery.PushStrParam(kElement.RetOwnerGuid());
		kQuery.PushStrParam(kBinaryData.abySkills, MAX_DB_SKILL_SIZE);

		g_kCoreCenter.PushQuery(kQuery);
	}
	void SaveSkillExt(DB_ITEM_STATE_CHANGE const& kElement, CONT_DOC_PLAYER::mapped_type const& rkSecond)
	{
		SPlayerBinaryData kBinaryData;
		rkSecond->GetMySkill()->SaveExtend(MAX_DB_SKILL_EXTEND_SIZE, kBinaryData.abySkillExtends);

		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_SkillExtend]");
		kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
		kQuery.PushStrParam(kElement.RetOwnerGuid());
		kQuery.PushStrParam(static_cast<BYTE>(false));//
		kQuery.PushStrParam(kBinaryData.abySkillExtends, MAX_DB_SKILL_EXTEND_SIZE);

		g_kCoreCenter.PushQuery(kQuery);
	}

	typedef std::map< SModifyOrderOwner, DB_ITEM_STATE_CHANGE_ARRAY > RESULT_HASH;
	bool IsPublicChannelMap(int const iGroundNo)
	{
		const CONT_DEFMAP* pkContDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkContDefMap);

		if(!pkContDefMap)
		{
			return false;
		}

		CONT_DEFMAP::const_iterator itor = pkContDefMap->find(iGroundNo);
		if (itor != pkContDefMap->end())
		{
			const TBL_DEF_MAP& rkDefMap = itor->second;
			if( 0 != (rkDefMap.iAttr & GATTR_FLAG_PUBLIC_CHANNEL) )
			{
				return true;
			}
		}

		return false;
	}

	void ProcessModifyItem_SendResult(CONT_DOC_PLAYER const &kContCopyPlayer, EItemModifyParentEventType const kCause, HRESULT const hRet, BM::Stream const& rkAddonPacket, RESULT_HASH const& rkResult)
	{
		RESULT_HASH::const_iterator iter = rkResult.begin();
		while(rkResult.end() != iter)
		{
			RESULT_HASH::key_type const & rkKey = (*iter).first;

			switch( rkKey.eOwnerType )
			{
			case OOT_Guild:
				{
					CONT_DOC_PLAYER::const_iterator player_iter = kContCopyPlayer.find(rkKey);
					if(player_iter != kContCopyPlayer.end())
					{
						PgDoc_Player const * const pkPlayer = (*player_iter).second;
						RESULT_HASH::mapped_type const &rkElement = (*iter).second; // 길드금고의 변화한 아이템들(골드포함)

						BM::Stream kPacket(PT_A_N_REQ_MODIFY_GUILD_ITEM);
						kPacket.Push(rkKey.kOwnerGuid);
						kPacket.Push(rkElement.size());

						DB_ITEM_STATE_CHANGE_ARRAY::const_iterator loop_iter = rkElement.begin();
						while( rkElement.end() != loop_iter )
						{
							DB_ITEM_STATE_CHANGE_ARRAY::value_type const& kDBItemChangeState = (*loop_iter);
							kDBItemChangeState.WriteToPacket(kPacket);

							++loop_iter;
						}

						if( !rkAddonPacket.IsEmpty() )
						{
							kPacket.Push(true);
							kPacket.Push(rkAddonPacket);
						}
						else
						{
							kPacket.Push(false);
						}
						SendToRealmContents( PMET_GUILD, kPacket );
					}
				}break;
			case OOT_Player:
			default:
				{
					CONT_DOC_PLAYER::const_iterator player_iter = kContCopyPlayer.find(rkKey);
					if(player_iter != kContCopyPlayer.end())
					{
						PgDoc_Player const * const pkPlayer = (*player_iter).second;

						RESULT_HASH::mapped_type const &rkElement = (*iter).second;

						// 여기에 들어있는 second는 플레이어이지만 rkKey는 펫의 GUID일 수도 있으니 참고하라
						BM::Stream kOutPacket(PT_I_M_ANS_MODIFY_ITEM);
						kOutPacket.Push(kCause);
						kOutPacket.Push(hRet);//자체 내역 성공 여부
						kOutPacket.Push(rkKey.kOwnerGuid);//Caster GUID(pkPlayer->GetID()로 하면 절대 안된다.
						kOutPacket.Push(pkPlayer->GetID());// Owner GUID(Pet인경우는 Caster랑 Owner랑 틀리다.)
						PU::TWriteArray_M( kOutPacket, rkElement );//Array

						if( rkAddonPacket.IsEmpty() )
						{
							kOutPacket.Push((bool)false);
						}
						else
						{
							kOutPacket.Push((bool)true);
							kOutPacket.Push(rkAddonPacket);
						}

						if(true == RealmUserManagerUtil::IsPublicChannelMap(pkPlayer->GroundKey().GroundNo()))
						{
							::SendToGround( CProcessConfig::GetPublicChannel(), pkPlayer->GroundKey(), kOutPacket );
						}
						else
						{
							::SendToGround( pkPlayer->GetChannel(), pkPlayer->GroundKey(), kOutPacket );
						}
					}
					else
					{
						// 못 찾으면 Pet 일 껄?

					}
				}break;
			}

			++iter;
		}
	}
};

bool const PgRealmUserManager::BuildLogType(EItemModifyParentEventType const kCause,PgDoc_Player * const pkPlayer, PgContLogMgr & kLogContMgr, int const iPlayerIndex, BM::Stream const* pkAddonPacket)
{
	if(!pkPlayer)
	{
		return false;
	}

	PgLogCont kLogCont;

	switch(kCause)
	{
	case CIE_GambleMachine_Mixup:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Gamblemachine_Mixup);
		}break;
	case CIE_Inventory_Sort:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Modify);
		}break;
	case CIE_GambleMachine_Shop:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Gamblemachine_Cash);
		}break;
	case CIE_GambleMachine:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Gamblemachine);
		}break;
	case CIE_MixupItem:
		{
			kLogCont.LogMainType(ELogMain_MixupItem);
			kLogCont.LogSubType(ELogSub_Item_Creative);
		}break;
	case CIE_Home_Equip:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Item_Equip);			
		}break;
	case CIE_Home_UnEquip:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Item_Unequip);			
		}break;
	case CIE_Home_Modify:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Modify);
		}break;
	case CIE_Home_Bidding:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Bidding);
		}break;
	case CIE_Home_Auction_Reg:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Auction_Reg);
		}break;
	case CIE_Home_Auction_Unreg:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Auction_Unreg);
		}break;
	case CIE_Home_Auction_End:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Auction_End);
		}break;
	case CIE_Home_Attachment:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Attachment);
		}break;
	case CIE_Home_Noti_PayTex:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Noti_Tex);
		}break;
	case CIE_Home_PayTex:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Home_Pay_Tex);
		}break;
	case CIE_Home_Item_Modify:
		{
			kLogCont.LogMainType(ELogMain_MyHome);
			kLogCont.LogSubType(ELogSub_Item_Modify);
		}break;
	case CIE_CCE_Reward:
		{
			kLogCont.LogMainType(ELogMain_CCEReward);
			kLogCont.LogSubType(ELogSub_CCE_Reward);
		}break;
	case CIE_CS_Add_TimeLimit:
		{
			kLogCont.LogMainType(ELogMain_Cash);
			kLogCont.LogSubType(ELogSub_Cash_AddTime);
		}break;
	case CIE_Gen_Socket:
		{
			kLogCont.LogMainType(ELogMain_Socket);
			kLogCont.LogSubType(ELogSub_Socket_Create);
		}break;
	case CIE_Set_MonsterCard:
		{
			kLogCont.LogMainType(ELogMain_Socket);
			kLogCont.LogSubType(ELogSub_Socket_Modify);
		}break;
	case CIE_Remove_MonsterCard:
		{
			kLogCont.LogMainType(ELogMain_Socket);
			kLogCont.LogSubType(ELogSub_Socket_Remove);
		}break;
	case CIE_Del_MonsterCard:
		{
			kLogCont.LogMainType(ELogMain_Socket);
			kLogCont.LogSubType(ELogSub_Socket_Delete);
		}break;
	case CIE_EXTRACTION_MonsterCard:
		{
			kLogCont.LogMainType(ELogMain_Socket);
			kLogCont.LogSubType(ELogSub_Socket_Extraction);
		}break;
	case CIE_GemStore_Buy:
		{
			kLogCont.LogMainType(ELogMain_GemStore);
			kLogCont.LogSubType(ELogSub_GemStore_Buy);
		}break;
	case CIE_CollectAntique:
		{
			int iMenu = 0;
			int iItemNo = 0;
			int iIndex = 0;
			int iResultItemNo = 0;
			if( pkAddonPacket )
			{
				size_t iSize = 0;
				BM::Stream::DEF_STREAM_TYPE wType = 0;
				BM::Stream kAddonPacket(*pkAddonPacket);
				kAddonPacket.Pop(iSize);
				kAddonPacket.Pop(wType);
				kAddonPacket.Pop(iMenu);
				kAddonPacket.Pop(iItemNo);
				kAddonPacket.Pop(iIndex);
				kAddonPacket.Pop(iResultItemNo);
			}

			kLogCont.LogMainType(ELogMain_GemStore);
			kLogCont.LogSubType(EGT_ANTIQUE==iMenu ? ELogSub_CollectAntique : ELogSub_ItemShop);

			PgLog kNew(ELOrderMain_Item, ELOrderSub_Success);
			kNew.Set(0, iIndex);
			kNew.Set(1, iItemNo);
			kNew.Set(2, iResultItemNo);
			kLogCont.Add(kNew);
		}break;
	case CIE_ExchangeGem:
		{
			kLogCont.LogMainType(ELogMain_GemStore);
			kLogCont.LogSubType(ELogSub_ExchangeGem);
		}break;
	case CIE_Convert_Item:
		{
			kLogCont.LogMainType(ELogMain_ConvertItem);
			kLogCont.LogSubType(ELogSub_ConvertItem);
		}break;
	case MCE_Loot:
	case CIE_Loot:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Take_To_Monster);			
		}break;
	case MCE_BuyItem:
	case CIE_CoinChange_Buy:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Buy);			
		}break;
	case MCE_SellItem:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Sell);			
		}break;
	case CIE_Sys2Inv:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_SysInv);
		}break;
	case CIE_Delete_SysItem:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_SysInv);
		}break;
	case CIE_Make:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Creative);
		}break;
	case CIE_BasicOptionAmp:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Enchent);
		}break;
	case CIE_EnchantLvUp:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Enchent);

			int iItemNo = 0;
			EPlusItemUpgradeResult eResult = PIUR_NONE;
			int iOldEnchantLv = 0;
			int iNewEnchantLv = 0;
			if( pkAddonPacket )
			{
				size_t iSize = 0;
				BM::Stream kAddonPacket(*pkAddonPacket);
				BM::Stream::DEF_STREAM_TYPE wType = 0;
				BM::Stream kTempPacket;
				kAddonPacket.Pop( iSize );
				kAddonPacket.Pop( wType );
				kAddonPacket.Pop( kTempPacket.Data() );
				kAddonPacket.Pop( iItemNo );
				kAddonPacket.Pop( eResult );
				kAddonPacket.Pop( iOldEnchantLv );
				kAddonPacket.Pop( iNewEnchantLv );
			}

			switch( eResult ) // 이벤트에 대한 추가 로그
			{
			case PIUR_SUCCESS:
			case PIUR_NORMAL_FAILED:
			case PIUR_PANALTY_FAILED:
			case PIUR_DESTROY_FAILED:
				{
					PgLog kNewLog(ELOrderMain_Item_EnchantLvUp, ELOrderSub_None);
					kNewLog.Set(PgLogUtil::AtIndex(1), iItemNo);
					kNewLog.Set(PgLogUtil::AtIndex(2), static_cast< int >(eResult));
					kNewLog.Set(PgLogUtil::AtIndex(3), iOldEnchantLv);
					kNewLog.Set(PgLogUtil::AtIndex(4), iNewEnchantLv);
					kLogCont.Add(kNewLog);
				}break;
			default:
				{
				}break;
			}
		}break;
	case CIE_SoulCraft:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Enchent);

			int iItemNo = 0;
			EItemRarityUpgradeResult eResult = IRUR_NONE;
			E_ITEM_GRADE eOld = IG_NORMAL;
			E_ITEM_GRADE eNew = IG_NORMAL;
			if( pkAddonPacket )
			{
				size_t iSize = 0;
				BM::Stream kAddonPacket(*pkAddonPacket);
				BM::Stream::DEF_STREAM_TYPE wType = 0;
				BM::Stream kTempPacket;
				kAddonPacket.Pop( iSize );
				kAddonPacket.Pop( wType );
				kAddonPacket.Pop( kTempPacket.Data() );
				kAddonPacket.Pop( iItemNo );
				kAddonPacket.Pop( eResult );
				kAddonPacket.Pop( eOld );
				kAddonPacket.Pop( eNew );
			}

			switch( eResult ) // 이벤트에 대한 추가 로그
			{
			case IRUR_SUCCESS:
			case IRUR_FAIL:
			case IRUR_FAIL_AND_BROKEN:
				{
					PgLog kNewLog(ELOrderMain_Item_SoulCraft, ELOrderSub_None);
					kNewLog.Set(PgLogUtil::AtIndex(1), iItemNo);
					kNewLog.Set(PgLogUtil::AtIndex(2), static_cast< int >(eResult));
					kNewLog.Set(PgLogUtil::AtIndex(3), static_cast< int >(eOld));
					kNewLog.Set(PgLogUtil::AtIndex(4), static_cast< int >(eNew));
					kLogCont.Add(kNewLog);
				}break;
			default:
				{
				}break;
			}
		}break;
	case CIE_TimeOut:
	case IMPET_MapmoveDeleteItem:
	case CIE_Dump:
	case CIE_Disassemble_Result:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Destroy);
		}break;
	case CIE_Repair:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Repair);
		}break;
	case CIE_Exchange:
		{
			// 거래로그 남기기
			//	같은 Log GUID 로 거래성공 LogCont 가 2개 생김 (Player 당 한개씩)
			//	각 LogCont에 상대방의 정보값이 남아야 함.
			kLogCont.LogMainType(ELogMain_Contents_Transaction);
			kLogCont.LogSubType(ELogSub_Transaction);
			if (iPlayerIndex > 0)
			{
				if (iPlayerIndex > 1)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("player Counter Cannot be over 2, Count=") << iPlayerIndex);
				}
				else
				{
					VEC_GUID kCharGuidVec;
					kLogContMgr.GetLogCont(kCharGuidVec);
					if (kCharGuidVec.size() != 1)
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("PlayerGuidVector SIZE should be 1, Size=") << kCharGuidVec.size());
					}
					else
					{
						BM::GUID& rk1stPlayerGuid = kCharGuidVec.at(0);
						PgLogCont * const pkLogCont = kLogContMgr.GetLogCont(rk1stPlayerGuid);
						if (pkLogCont == NULL)
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Cannot find LogCont CharGuid=") << rk1stPlayerGuid);
						}
						else
						{
							PgLog kNew(ELOrderMain_Transaction, ELOrderSub_Complete);
							// 1st Player에 상대방 정보 기록하기
							kNew.Set(0, pkPlayer->Name());	// 거래 대상 Name
							kNew.Set(1, std::wstring(_T("1")));	// 거래 성공 여부 (1:성공)
							kNew.Set(2, pkPlayer->GetID().str());	// 거래 대상 GUID
							kNew.Set(3, kLogCont.LogKey().str());	// 거래 대상 Log Guid
							pkLogCont->Add(kNew);

							// 2nd Player에 상대방 정보 기록하기
							kNew.Set(0, pkLogCont->Name());
							kNew.Set(1, std::wstring(_T("1")));
							kNew.Set(2, pkLogCont->CharacterKey().str());
							kNew.Set(3, pkLogCont->LogKey().str());
							kLogCont.Add(kNew);
						}
					}
				}
			}
		}break;
	case CIE_Divide:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Divide);
		}break;
	case MCE_EXP:
	case MCE_Skill:
	case MCE_Fran:
	case MCE_MarryMoney:
	case CIE_CoupleLearnSkill:
	case MCE_SpendMoney:
	case MCE_EffectControl:
	case IMEPT_LEARNSKILL:
		{
			kLogCont.LogMainType(ELogMain_User_Character);
			kLogCont.LogSubType(ELogSub_Character_Change_Info);
		}break;
	case CIE_Post_Mail_Send:
		{
			kLogCont.LogMainType(ELogMain_Contents_Mail);
			kLogCont.LogSubType(ELogSub_Mail_Forwarding);
		}break;
	case CIE_Post_Mail_Item_Recv:
		{
			kLogCont.LogMainType(ELogMain_Contents_Mail);
			kLogCont.LogSubType(ELogSub_Mail_RecvItem);
		}break;
	case IMEPT_QUEST_DIALOG_Accept:
	case IMEPT_QUEST_SHARE:
	case IMEPT_QUEST_BATTLE_PASS:
		{
			kLogCont.LogMainType(ELogMain_Contents_Quest);
			kLogCont.LogSubType(ELogSub_Quest_Start);
		}break;
	case IMEPT_QUEST: // 퀘스트 플레이 도중에
	case CIE_QuestItem:
	case IMEPT_QUEST_DIALOG:
	case IMEPT_QUEST_BUILD_RANDOM_BY_ITEM:
	case IMEPT_QUEST_BUILD_RANDOM:
	case IMEPT_QUEST_BUILD_TACTICS_RANDOM_BY_ITEM:
		{
			kLogCont.LogMainType(ELogMain_Contents_Quest);
			kLogCont.LogSubType(ELogSub_Quest_ChangeInfo);
		}break;
	case IMEPT_QUEST_DIALOG_End: // 퀘스트 완료
	case IMEPT_QUEST_REMOTE_COMPLETE:
		{
			kLogCont.LogMainType(ELogMain_Contents_Quest);
			kLogCont.LogSubType(ELogSub_Quest_End);
		}break;
	case IMEPT_QUEST_DROP: // 포기
		{
			kLogCont.LogMainType(ELogMain_Contents_Quest);
			kLogCont.LogSubType(ELogSub_Quest_Drop);
		}break;
	case IMEPT_EVENTQUEST:
		{
			kLogCont.LogMainType(ELogMain_EventQuest);
			kLogCont.LogSubType(ELogSub_EventQuest);
		}break;
	//case IMEPT_GUILD:
	//	{
	//		kLog.LogMainType(ELogMain_Contents_Guild);
	//		kLog.LogSubType(ELogSub_None);
	//	}break;
	case MCE_CreateGuild:
	case MCE_FailCreateGuild:
		{
			kLogCont.LogMainType(ELogMain_Contents_Guild);
			kLogCont.LogSubType(ELogSub_Guild_Create);
		}break;
	case CIE_GuildLevelUp:
		{
			kLogCont.LogMainType(ELogMain_Contents_Guild);
			kLogCont.LogSubType(ELogSub_Change_Level);
		}break;
	case CIE_GuildInventoryCreate:
		{
			kLogCont.LogMainType(ELogMain_Contents_Guild);
			kLogCont.LogSubType(ELogSub_Guild_InventoryCreate);
		}break;
	case CIE_GuildLearnSkill:
		{
			kLogCont.LogMainType(ELogMain_Contents_Guild);
			kLogCont.LogSubType(ELogSub_Guild_Change_Info);
		}break;
	case CIE_CS_Buy:
		{
			kLogCont.LogMainType(ELogMain_Cash);
			kLogCont.LogSubType(ELogSub_Cash_Buy);
		}break;
	case CIE_CS_Gift:
		{
			kLogCont.LogMainType(ELogMain_Cash);
			kLogCont.LogSubType(ELogSub_Cash_Present);
		}break;
	case CIE_CS_Gift_Recv:
		{
			kLogCont.LogMainType(ELogMain_Cash);
			kLogCont.LogSubType(ELogSub_Cash_Receipt);
		}break;
	case CIE_Coupon:
		{
			kLogCont.LogMainType(ELogMain_Coupon);
			kLogCont.LogSubType(ELogSub_Coupon);
		}break;
	case MIE_UseItem:
	case CIE_OpenChest:
	case CIE_OpenPack:
	case CIE_OpenPack2:
	case CIE_HiddenPack:
	case CIE_Rollback:
	case CIE_Locked_Chest:
	case CIE_Open_Gamble:
	case CIE_Use_ExpCard:
	case CIE_Item_Enchant_Shift:
	case CIE_JOBSKILL3_CREATE_ITEM:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Use);			
		}break;
	case CIE_UM_Market_Open:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Open);
		}break;
	case CIE_UM_Article_Reg:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Regist_Product);
		}break;
	case CIE_UM_Article_Dereg:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Recall_Product);
		}break;
	case CIE_UM_Article_Buy:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Buy_Product);
		}break;
	case CIE_UM_Dealing_Read:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Recall_Money);
		}break;
	case CIE_UM_Modify_Market:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_ChangeInfo);
		}break;
	case CIE_UM_Market_Remove:
		{
			kLogCont.LogMainType(ELogMain_Contents_OpenMarket);
			kLogCont.LogSubType(ELogSub_OpenMarket_Off);
		}break;
	case CIE_Equip:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Equip);			
		}break;
	case CIE_UnEquip:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Unequip);			
		}break;
	case CIE_MoveFromInvToGuildInv:
	case CIE_MoveFromGuildInvToGuildInv:
	case CIE_MoveToSafe:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Give_Bond);			
		}break;
	case CIE_MoveFromGuildInvToInv:
	case CIE_MoveToInv:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Item_Take_Bond);			
		}break;
	case CIE_OpenEventItemReward:
	case CIE_SendEventItemReward:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_EventItemReward);
		}break;
	case CIE_OpenTreasureChestReward:
	case CIE_SendTreasureChestReward:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_TreasureChest);
		}break;
	case CIE_Manufacture:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Manufacture);
		}break;
	case CIE_Constellation:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_Constellation);
		}break;
	case ELogSub_PetProduceItem:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_PetProduceItem);
		}break;
	case CIE_Mission:
	case CIE_Mission1:
	case CIE_Mission2:
	case CIE_Mission3:
	case CIE_Mission4:
	case CIE_Mission_GadaCoin:
	case CIE_Mission_Rank:
	case CIE_Mission_DefenceWin:
		{
			kLogCont.LogMainType(ELogMain_Contents_MIssion);
			kLogCont.LogSubType(ELogSub_Mission_End);
		}break;
	case CIE_ACTIVE_STATUS_SET:
		{
			kLogCont.LogMainType(ELogMain_User_Character);
			kLogCont.LogSubType(ELogSub_Character_Change_Info);
		}break;
	case CAE_Achievement:
		{
			kLogCont.LogMainType(ELogMain_Contents_Achievements);
			kLogCont.LogSubType(ELogSub_Achievements);
		}break;
	case CIE_JOBSKILL_LEARN:
		{
			kLogCont.LogMainType(ELogMain_JobSkill);
			kLogCont.LogSubType(ELogSub_None);
			
			if( pkAddonPacket )
			{
				PgLog kNew(ELOrderMain_Skill, ELOrderSub_Learn);
				kLogCont.Add(kNew);
			}
		}break;
	case CIE_JOBSKILL_DELETE:
		{
			kLogCont.LogMainType(ELogMain_JobSkill);
			kLogCont.LogSubType(ELogSub_None);
			
			if( pkAddonPacket )
			{
				PgLog kNew(ELOrderMain_Skill, ELOrderSub_Delete);
				kLogCont.Add(kNew);
			}
		}break;
	case CIE_UserQuestComplete:
		{
			kLogCont.LogMainType(ELogMain_Contents_Quest);
			kLogCont.LogSubType(ELogSub_UserQuestComplete);
		}break;
	case CIE_ExtractElement:
		{
			kLogCont.LogMainType(ELogMain_Contents_Item);
			kLogCont.LogSubType(ELogSub_ExtractElement);
		}break;
	case IMEPT_WORLD_MAP:
	case CPE_Event:
	case CAE_HiddenOpen:
	case CIE_PREMIUM_SERVICE:
	case CIE_Mouse_Event:
		{
			// 로그 사용하지 않는 타입
		}break;
	default:
		{
			if( 0 != (g_kProcessCfg.RunMode()&CProcessConfig::E_RunMode_Debug) )
			{
				CAUTION_LOG(BM::LOG_LV2, L"Unknown loged transaction type:"<<static_cast<int>(kCause));
			}
			return false;
		}break;
	}

	//kLogCont.LogKey(BM::GUID::Create());	// 위에서 LogGUID를 사용하고 있으므로, 여기서 변경하지 않도록 하자.
	kLogCont.MemberKey(pkPlayer->GetMemberGUID());
	kLogCont.UID(pkPlayer->UID());
	kLogCont.CharacterKey(pkPlayer->GetID());
	kLogCont.ID(pkPlayer->MemberID());
	kLogCont.Name(pkPlayer->Name());
	kLogCont.RealmNo(g_kProcessCfg.RealmNo());
	kLogCont.ChannelNo(pkPlayer->GetChannel());
	kLogCont.Class(pkPlayer->GetAbil(AT_CLASS));
	kLogCont.Level(pkPlayer->GetAbil(AT_LEVEL));
	kLogCont.GroundNo(pkPlayer->GroundKey().GroundNo());
	kLogCont.UnitGender(static_cast<EUnitGender>(pkPlayer->GetAbil(AT_GENDER)));

	kLogContMgr.AddContLog(kLogCont);
	return true;
}

eWriteType CustomWriteType(EItemModifyParentEventType const kCause)
{
	switch(kCause)
	{
	case CIE_UM_Modify_Market:			{ return WT_OP_COPY_PREMIUM;						}break;
	case IMEPT_QUEST_DIALOG_Accept:		{ return WT_OP_COPY_PREMIUM;						}break;
	case CIE_PREMIUM_SERVICE:			{ return WT_OP_COPY_PREMIUM;						}break;
	}
	return WT_NONE;
}

HRESULT PgRealmUserManager::Locked_ProcessModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder,
												SRealmGroundKey const &kGndKey,	BM::Stream const &kAddonPacket, BM::Stream & rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	HRESULT hRet = S_OK;

	CONT_DOC_PLAYER kContCopyPlayer;
	CONT_DOC_PLAYER kContOrgPlayer;
	PgContLogMgr kContLogMgr;
	int iLogContCount = 0;

	typedef std::map< SModifyOrderOwner, SCenterPlayer_Lock > CONT_CENTERPLAYER_LOCK;
	CONT_CENTERPLAYER_LOCK kContCenterPlayerLock;

	CONT_PLAYER_MODIFY_ORDER::const_iterator itor = kContOrder.begin();
	for( ; itor != kContOrder.end() ; ++itor )
	{
		SPMO const &kOrder = (*itor);

		SCenterPlayer_Lock kLockPlayer;
		if( !GetInfo(kOrder.ModifyOwner(), kLockPlayer, false) || (kLockPlayer.pkCenterPlayerData->pkPlayer == NULL) )
		{
			if (!GetOfflineInfo(kOrder.ModifyOwner(), kLockPlayer) || (kLockPlayer.pkCenterPlayerData->pkPlayer == NULL) )
			{
				ProcessModifyPlayer_LogOut( kCause, kOrder, kGndKey );
				hRet = E_FAIL;
				continue;
			}
		}

		PgDoc_Player* const pkOrgPlayer = kLockPlayer.pkCenterPlayerData->pkPlayer;
		PgDoc_Player* pkCopyPlayer = kLockPlayer.pkCenterPlayerData->pkBackupPlayer;
		pkOrgPlayer->CopyTo(*pkCopyPlayer, WT_DEFAULT_WITH_EXCLUDE|WT_OP_MISSION|CustomWriteType(kCause));

		auto kInsert = kContCopyPlayer.insert(std::make_pair(kOrder.ModifyOwner(), pkCopyPlayer));
		if ( kInsert.second )
		{
			kContOrgPlayer.insert(std::make_pair(kOrder.ModifyOwner(), pkOrgPlayer));
			kContCenterPlayerLock.insert(std::make_pair(kOrder.ModifyOwner(), kLockPlayer));

			BuildLogType(kCause, pkOrgPlayer, kContLogMgr, iLogContCount++, &kAddonPacket);
		}
	}


	//
	SProcessModifyResult kResult(S_OK, S_OK);
	if( S_OK == hRet )
	{
		kResult = ProcessModifyPlayer(kCause, kContOrder, kGndKey, kContCopyPlayer, kContOrgPlayer, kAddonPacket, rkPacket,kContLogMgr);

		if( S_OK == kResult.hResult )
		{
			// 성공했으므로 kContCopyPlayer 정보가 실제 Player 정보가 되도록 한다.
			CONT_DOC_PLAYER::iterator itor_p = kContCopyPlayer.begin();
			while( kContCopyPlayer.end() != itor_p )
			{
				CONT_CENTERPLAYER_LOCK::iterator itor_find = kContCenterPlayerLock.find(itor_p->first);
				if( kContCenterPlayerLock.end() == itor_find )
				{
					// 이럴수가 !!! 위에는 있었는데, 여기와서 없다니... 대책없네 ㅡㅡ;;
					VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Cannot find Player[") << itor_p->first.kOwnerGuid.str().c_str() << _T("]") );
					itor_p = kContCopyPlayer.erase(itor_p);
					continue;
				}

				PgDoc_Player* pkTempPlayer = itor_find->second.pkCenterPlayerData->pkPlayer;
				itor_find->second.pkCenterPlayerData->pkPlayer = itor_p->second;
				itor_find->second.pkCenterPlayerData->pkBackupPlayer = pkTempPlayer;
				++itor_p;
			}

			kContLogMgr.SendLog();
		}
	}
    else
    {
        kResult = ProcessFailModifyPlayer(kCause, kContOrder, kGndKey, kContCopyPlayer, kContOrgPlayer, kAddonPacket, rkPacket,kContLogMgr);
    }

	ProcessModify_End(kResult, kCause, kContOrder, kGndKey, kAddonPacket, rkPacket);

	return kResult.hResult;
}

void PgRealmUserManager::ProcessModifyPlayer_D_ResultProcess(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
															 BM::Stream const &kAddonPacket, BM::Stream & rkPacket,HRESULT const hRet)
{

	switch(kCause)
	{
	case CIE_Post_Mail_Item_Recv:
		{
			if(hRet != S_OK)
			{
				EItemModifyParentEventType kLocalCause;
				BM::GUID	kOwnerGuid,
							kMailGuid;

				rkPacket.Pop(kLocalCause);
				rkPacket.Pop(kOwnerGuid);
				rkPacket.Pop(kMailGuid);

				g_kPostMgr.Locked_ReleaseMail(kMailGuid);

				BM::Stream kPacket(PT_I_M_POST_ANS_MAIL_MODIFY);
				kPacket.Push(kOwnerGuid);
				kPacket.Push(PMSR_INVENTORY_FULL);
				SendToUserGround(kOwnerGuid,kPacket,false,true);
			}
		}break;
	case CIE_UM_Dealing_Read:
	case CIE_UM_Article_CashBack:
		{
			BM::GUID kOwnerGuid;
			rkPacket.Pop(kOwnerGuid);
			if(hRet != S_OK)
			{
				BM::Stream kPacket(PT_M_C_UM_ANS_DEALINGS_READ);
				kPacket.Push(UMR_INVENTORY_FULL);
				SendToUser(kOwnerGuid,kPacket,false);
			}
		}break;
	case CIE_UM_Article_Buy:
		{
			if(hRet != S_OK && hRet != E_SYSTEM_ERROR)
			{
				EItemModifyParentEventType eTempCause;
				rkPacket.Pop(eTempCause);
				size_t const iRdPos = rkPacket.RdPos();
				PgTranPointer kTran(rkPacket);
				rkPacket.RdPos(iRdPos);
				PgTransaction* pkTran = kTran.GetTran();

				if (pkTran != NULL)
				{
					BM::Stream kPacket(PT_M_C_UM_ANS_ARTICLE_BUY);
					kPacket.Push(UMR_INVENTORY_FULL);
					SendToUser(pkTran->CharacterGuid(), kPacket, false);
				}
			}
		}break;
	case CIE_CS_Buy:
		{
			if(hRet != S_OK && hRet != E_SYSTEM_ERROR)
			{
				/*
				EItemModifyParentEventType kLocalCause;
				BM::GUID	kOwnerGuid;
				rkPacket.Pop(kLocalCause);
				rkPacket.Pop(kOwnerGuid);
				*/
				EItemModifyParentEventType eTempCause;
				rkPacket.Pop(eTempCause);
				size_t const iRdPos = rkPacket.RdPos();
				PgTranPointer kTran(rkPacket);
				rkPacket.RdPos(iRdPos);
				
				PgCashShopBuyTran* pkTran = dynamic_cast<PgCashShopBuyTran*>(kTran.GetTran());
				if (pkTran != NULL)
				{
					BM::Stream kPacket(PT_M_C_CS_ANS_BUY_ARTICLE);
					kPacket.Push(CSR_BUY_INVENTORY_FULL);
					kPacket.Push(pkTran->BuyType());
					SendToUser(pkTran->CharacterGuid(), kPacket, false);
				}
			}
		}break;
	case CIE_CS_Gift:
		{
		}break;
	case CIE_CS_Gift_Recv:
		{
			if(hRet != S_OK)
			{
				EItemModifyParentEventType kLocalCause;
				BM::GUID	kOwnerGuid;
				rkPacket.Pop(kLocalCause);
				rkPacket.Pop(kOwnerGuid);

                g_kRealmUserMgr.Locked_LeaveEvent(kOwnerGuid);

				BM::Stream kPacket(PT_M_C_CS_ANS_RECV_GIFT);
				kPacket.Push(CSR_GIFT_INVENTORY_FULL);
				SendToUser(kOwnerGuid,kPacket,false);
			}
		}break;
	case CIE_CS_Add_TimeLimit:
		{
			if(hRet != S_OK && hRet != E_SYSTEM_ERROR)
			{
				EItemModifyParentEventType eTempCause;
				rkPacket.Pop(eTempCause);
				size_t const iRdPos = rkPacket.RdPos();
				PgTranPointer kTran(rkPacket);
				rkPacket.RdPos(iRdPos);
				PgTransaction* pkTran = kTran.GetTran();

				BM::Stream kPacket(PT_M_C_CS_ANS_ADD_TIMELIMIT);
				kPacket.Push(CSR_INVALID_ARTICLE);
				SendToUser(pkTran->CharacterGuid(),kPacket,false);
			}
		}break;
    case CIE_EmporiaReserve:
		{
            ProcessEmporiaReserve(hRet, kAddonPacket);
		}break;
    case CIE_EmporiaThrow:
        {
            BM::Stream kAddPacket = kAddonPacket;
            int iSize = 0;
            kAddPacket.Pop(iSize);

            SGuild_Inventory_Log kLog;
            kLog.ReadFromPacket(kAddPacket);

            BM::GUID kReqCharGuid;
            BM::GUID kGuildMasterGuid;
            BYTE byType = GCR_None;
            __int64 i64Money = 0i64;
            kAddPacket.Pop(kReqCharGuid);
            kAddPacket.Pop(kGuildMasterGuid);
            kAddPacket.Pop(byType);
            kAddPacket.Pop(i64Money);

            if( S_OK == hRet )// 엠포리아 도전포기
            {
                if(GCR_None != byType)
                {
                    BM::Stream kPacket(PT_C_N_REQ_GUILD_COMMAND, kGuildMasterGuid);
                    kPacket.Push((BYTE)GC_EmporiaThrow);
                    kPacket.Push(i64Money);
                    SendToGuildMgr(kPacket);
                }
            }
            else
            {
                CAUTION_LOG(BM::LOG_LV1, __FL__ << L"[Emporia-Throw: CharGuid<" << kReqCharGuid << L">] Failed");
                BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
                kAnsPacket.Push((BYTE)GCR_Failed);
                SendToUser(kReqCharGuid,kAnsPacket,false);
            }
        }break;
	}
}

HRESULT PgRealmUserManager::OnProcessAhievement2Inv(SPMO const &kOrder, PgDoc_Player * pkCopyPlayer, DB_ITEM_STATE_CHANGE_ARRAY & kChangeArray, PgContLogMgr & kContLogMgr)
{
	SPlayerModifyOrderData_Achievement2Inv kData;
	kOrder.Read(kData);

	PgBase_Item kOldItem;
	SItemPos const kItemPos(IT_FIT,EQUIP_POS_MEDAL);
	if(S_OK == pkCopyPlayer->GetInven()->GetItem(kItemPos, kOldItem))	// 만약 이전에 장착 했던 아이템이 있다면 먼저 업적으로 되돌려 보낸다.
	{
		HRESULT const hRet = pkCopyPlayer->ItemProcess(SPMO(IMET_MODIFY_COUNT, pkCopyPlayer->GetID(), SPMOD_Modify_Count( kOldItem, kItemPos,0,true)), kChangeArray, kContLogMgr);
		if(S_OK != hRet)
		{
			return hRet;
		}

		CONT_DEF_ITEM2ACHIEVEMENT const *pkContDef = NULL;
		g_kTblDataMgr.GetContDef(pkContDef);
		if(!pkContDef)
		{
			return E_FAIL;
		}

		CONT_DEF_ITEM2ACHIEVEMENT::const_iterator iter = pkContDef->find(kOldItem.ItemNo());	// 훈장 아이템이 아니면 여기서 리턴
		if(iter == pkContDef->end())
		{
			return E_FAIL;
		}
		
		int const iSaveIdx = (*iter).second.iSaveIdx;

		if(pkCopyPlayer->GetAchievements()->CheckHasItem(iSaveIdx) || !pkCopyPlayer->GetAchievements()->ResetItem(iSaveIdx,false))
		{
			return E_FAIL;
		}

		tagDBItemStateChange kDBChange(DISCT_INV2ACHIEVEMENT, IMET_INV2ACHIEVEMENT, kOrder.OwnerGuid());
		kDBChange.kAddonData.Push(iSaveIdx);
		kChangeArray.push_back(kDBChange);
	}

	if(!pkCopyPlayer->GetAchievements()->CheckHasItem(kData.SaveIdx()) || !pkCopyPlayer->GetAchievements()->ResetItem(kData.SaveIdx(),true))
	{
		return E_FAIL;
	}

	tagDBItemStateChange kDBChange(DISCT_ACHIEVEMENT2INV, kOrder.Cause(), kOrder.OwnerGuid());
	kDBChange.kAddonData.Push(kData.SaveIdx());
	kChangeArray.push_back(kDBChange);

	return S_OK;
}

PgRealmUserManager::SProcessModifyResult PgRealmUserManager::ProcessModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		CONT_DOC_PLAYER const &kContCopyPlayer, CONT_DOC_PLAYER const &kContOrgPlayer, BM::Stream const &kAddonPacket, BM::Stream & rkPacket,PgContLogMgr & kContLogMgr)
{
	HRESULT hRet = S_OK;

	DB_ITEM_STATE_CHANGE_ARRAY kChangeArray;

////////////////////////////////////////////////////////////////////
	typedef RealmUserManagerUtil::RESULT_HASH RESULT_HASH;
	RESULT_HASH kRetHash;
	CONT_DOC_PLAYER kContResultPlayer = kContCopyPlayer;// ProcessModifyPlayer_D에서 kContResultPlayer에 Pet의 정보가 들어올 수도 있다.

	hRet = ProcessModifyPlayer_D(kCause, kContOrder, kContResultPlayer, kAddonPacket, rkPacket, kChangeArray, kContLogMgr);

	ProcessModifyPlayer_D_ResultProcess( kCause, kContOrder, kGndKey, kAddonPacket, rkPacket, hRet );

	HRESULT hFailedReturn = S_OK;
	if(S_OK == hRet)//카피에다 시도.
	{
		//DB작업.
		if( (false == kChangeArray.empty())
			&& (S_OK != ProcessModifyItemToDB(kCause,kChangeArray, rkPacket, kContCopyPlayer)))		// -> 결국 이건. -> 큐잉을 하던지...
		{//
			hRet = E_FAIL;
			kChangeArray.clear();
			VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T("DB Work Must Success.") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return SProcessModifyResult(E_FAIL, S_OK);
		}

		//////
		// CopyPlayer에 시도한 것을 실제Player정보로 사용할 것이므로, OrgPlayer에 시도할 필요 없다.
		//hRet = ProcessModifyItem_D(kContOrder, kContOrgInv, kAddonPacket, rkPacket, kChangeArray);
		//////

		if(S_OK == hRet)//카피에다 시도.
		{
			//이건 맵서버가 쪼개서 써도 될거 같긴한데...
			DB_ITEM_STATE_CHANGE_ARRAY::const_iterator ret_itor = kChangeArray.begin();
			while(ret_itor != kChangeArray.end() )
			{//받을 사람 기준으로 정리
				DB_ITEM_STATE_CHANGE_ARRAY::value_type const& rkChangeInfo = (*ret_itor);
				auto insert_ret = kRetHash.insert(std::make_pair( SModifyOrderOwner(rkChangeInfo.CharacterGuid(), rkChangeInfo.OwnerType()), DB_ITEM_STATE_CHANGE_ARRAY()));
				(*insert_ret.first).second.push_back(*ret_itor);

				++ret_itor;
			}
		}
		else
		{//카피에 시도 했는데 안될리가 없음.
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Work Is Must Success.") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ProcessModifyItemToDB is E_FAIL"));
		}
	}
	else
	{
		CONT_GEN_SYS_ITEM kGenSysItem;

		CONT_FAIL_LOG_MGR kContFailLogMgr;

		CONT_PLAYER_MODIFY_ORDER::const_iterator ret_iter = kContOrder.begin();
		HRESULT hTempResult = E_FAIL;
		while(kContOrder.end() != ret_iter)
		{
			CONT_PLAYER_MODIFY_ORDER::value_type const& rkOrder = (*ret_iter);

			hTempResult = ProcessModifyItemFail(kCause, rkOrder, kGenSysItem, kContOrgPlayer, kContFailLogMgr);

			auto kRet = kRetHash.insert(std::make_pair(rkOrder.ModifyOwner(), DB_ITEM_STATE_CHANGE_ARRAY()));

			if (hFailedReturn != S_OK && hFailedReturn != hTempResult)
			{
				// 이런 일이 있으면 안된다. 성공하려면 모두 성공, 실패하려면 모두 실패
				// 로그가 보인다면, 로직을 바꾸어야 한다.
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("ProcessModifyItemFail return=") << hTempResult << _T(", Cause=") << kCause << _T(", OrderCount=") << kContOrder.size());
			}
			hFailedReturn = hTempResult;
			++ret_iter;
		}

		for(CONT_FAIL_LOG_MGR::const_iterator fail_log_it=kContFailLogMgr.begin(); fail_log_it!=kContFailLogMgr.end(); ++fail_log_it)
		{
			(*fail_log_it).second.SendLog();
		}

		CONT_GEN_SYS_ITEM::const_iterator gen_iter = kGenSysItem.begin();
		while(gen_iter != kGenSysItem.end())
		{
			if(!(*gen_iter).second.empty())
			{
				BM::Stream kPacket(PT_N_C_NOTY_GEN_SYSTEM_INVENTORY);
				
				PU::TWriteArray_M(kPacket, (*gen_iter).second);

				SendToUser((*gen_iter).first,kPacket,false);
			}
			++gen_iter;
		}
	}

	if (kCause != CNE_CONTENTS_EVENT)
	{
		RealmUserManagerUtil::ProcessModifyItem_SendResult( kContResultPlayer, kCause, hRet, kAddonPacket, kRetHash);
	}
	return SProcessModifyResult(hRet, hFailedReturn);
}

PgRealmUserManager::SProcessModifyResult PgRealmUserManager::ProcessFailModifyPlayer(EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		CONT_DOC_PLAYER const &kContCopyPlayer, CONT_DOC_PLAYER const &kContOrgPlayer, BM::Stream const &kAddonPacket, BM::Stream & rkPacket,PgContLogMgr & kContLogMgr)
{
	HRESULT hRet = S_OK;
	HRESULT hFailedReturn = S_OK;

    switch(kCause)
    {
    case CIE_EmporiaReserve:
        {
            hRet = E_FAIL;
            ProcessEmporiaReserve(hRet, kAddonPacket, GCR_NotInvAuth);
		}break;
	case CIE_GateWayUnLock:
		{
			hRet = E_FAIL;

			RealmUserManagerUtil::RESULT_HASH::mapped_type kElement;
			BM::Stream kOutPacket(PT_I_M_ANS_MODIFY_ITEM);
			kOutPacket.Push(kCause);
			kOutPacket.Push(hRet);
			kOutPacket.Push(BM::GUID::NullData());//Caster GUID(pkPlayer->GetID()로 하면 절대 안된다.
			kOutPacket.Push(BM::GUID::NullData());// Owner GUID(Pet인경우는 Caster랑 Owner랑 틀리다.)
			PU::TWriteArray_M( kOutPacket, kElement );//Array

			if( kAddonPacket.IsEmpty() )
			{
				kOutPacket.Push((bool)false);
			}
			else
			{
				kOutPacket.Push((bool)true);
				kOutPacket.Push(kAddonPacket);
			}

			if(true == RealmUserManagerUtil::IsPublicChannelMap(kGndKey.GroundKey().GroundNo()))
			{
				::SendToGround( CProcessConfig::GetPublicChannel(), kGndKey.GroundKey(), kOutPacket );
			}
			else
			{
				::SendToGround( kGndKey.Channel(), kGndKey.GroundKey(), kOutPacket );
			}
        }break;
    }
	return SProcessModifyResult(hRet, hFailedReturn);
}

HRESULT PgRealmUserManager::ProcessModifyPlayer_D(EItemModifyParentEventType const kCause
												, CONT_PLAYER_MODIFY_ORDER const &kContOrder
												, CONT_DOC_PLAYER &kContTarget
												, BM::Stream const &kAddonPacket
												, BM::Stream & rkPacket
												, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray
												, PgContLogMgr & kContLogMgr)
{
	HRESULT hRet = S_OK;
	bool bIsRaiseError = false;

	int iIndex = 0;
	CONT_PLAYER_MODIFY_ORDER::const_iterator order_itor =  kContOrder.begin();
	while(order_itor !=  kContOrder.end())
	{
		SPMO const &kOrder = (*order_itor);

		CONT_DOC_PLAYER::iterator kFindItor = kContTarget.find(kOrder.ModifyOwner());
		if (kFindItor == kContTarget.end())
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Cannot find CopyInventory Owner[") << kOrder.OwnerGuid().str().c_str() << _T("]") );
			hRet = E_FAIL;
			bIsRaiseError = true;
			break;
		}
		PgDoc_Player* pkCopyPlayer = kFindItor->second;

		if(!pkCopyPlayer)
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"pkCopyPlayer is NULL!!!!");
			return E_FAIL;
		}


#ifdef _USE_JOBCHECKER
		pkCopyPlayer->AddJobChecker( kOrder.Cause() );
#endif

		HRESULT hSubRet = E_FAIL;
		
		switch(kOrder.Cause())
		{
		case IMET_GAMBLE_MIXUP_INSERT:
			{
				SPMOD_Gamble_Insert kData;
				kOrder.Read(kData);

				hSubRet = S_OK;

				if(true != g_kGambleMachine.CheckEnableMixupPoint(kData.MixPoint()))
				{
					hSubRet = E_GAMBLEMACHINE_INVALID_MIXPOINT;
					break;
				}

				if(true == m_kMixupUserMgr.IsGambling(kOrder.OwnerGuid()))
				{
					hSubRet = E_GAMBLEMACHINE_ALREADY_MIXUP;
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_MIXUP_INSERT, kOrder.Cause(), kOrder.OwnerGuid());
				kData.WriteToPacket(kDBChange.kAddonData);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_GAMBLE_MIXUP_MODIFY:
			{
				hSubRet = S_OK;

				CONT_DOC_PLAYER::iterator kFindItor = kContTarget.find(kOrder.ModifyOwner()); //종족 정보를 얻기 위해 플레이어를 얻어온다.
				if (kFindItor == kContTarget.end())
				{
					return E_FAIL;
				}

				SGambleUserInfo kGambleInfo;
				if(false == m_kMixupUserMgr.Get(kOrder.OwnerGuid(), kGambleInfo))
				{
					hSubRet = E_GAMBLEMACHINE_NOT_MIXUP;
					break;
				}

				if(0 >= kGambleInfo.iRouletteCount)
				{
					hSubRet = E_GAMBLEMACHINE_NOT_HAVE_ROULETTE_MIXUP;
					break;
				}

				--kGambleInfo.iRouletteCount;

				CONT_GAMBLEMACHINERESULT kContResult;
				CONT_GAMBLEITEM kContDumy;
				bool bBroadcast = false;
				hSubRet = g_kGambleMachine.GetMixupRoulette(kGambleInfo.iMixPoint, kContResult, kContDumy, kFindItor->second->UnitRace(), bBroadcast);
				if(S_OK != hSubRet)
				{
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_MIXUP_MODIFY, kOrder.Cause(), kOrder.OwnerGuid());
				PU::TWriteArray_M(kDBChange.kAddonData, kContResult);
				kDBChange.kAddonData.Push(bBroadcast);
				kDBChange.kAddonData.Push(kGambleInfo.iRouletteCount);
				PU::TWriteArray_A(kDBChange.kAddonData, kContDumy);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_GAMBLE_MIXUP_REMOVE:
			{
				hSubRet = S_OK;

				SGambleUserInfo kGambleInfo;
				if(false == m_kMixupUserMgr.Get(kOrder.OwnerGuid(), kGambleInfo))
				{
					hSubRet = E_GAMBLEMACHINE_NOT_MIXUP;
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_MIXUP_REMOVE, kOrder.Cause(), kOrder.OwnerGuid());
				PU::TWriteArray_M(kDBChange.kAddonData, kGambleInfo.kContResult);
				kDBChange.kAddonData.Push(kGambleInfo.bBroadcast);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_GAMBLE_INSERT:
			{
				SPMOD_Gamble_Insert kData;
				kOrder.Read(kData);

				hSubRet = S_OK;

				if(true == m_kGambleUserMgr.IsGambling(kOrder.OwnerGuid()))
				{
					hSubRet = E_GAMBLEMACHINE_ALREADY_GAMBLING;
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_INSERT, kOrder.Cause(), kOrder.OwnerGuid());
				kData.WriteToPacket(kDBChange.kAddonData);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_GAMBLE_MODIFY:
			{
				hSubRet = S_OK;
				CONT_DOC_PLAYER::iterator kFindItor = kContTarget.find(kOrder.ModifyOwner()); //종족 정보를 얻기 위해 플레이어를 얻어온다.
				if (kFindItor == kContTarget.end())
				{
					return E_FAIL;
				}

				SGambleUserInfo kGambleInfo;
				if(false == m_kGambleUserMgr.Get(kOrder.OwnerGuid(), kGambleInfo))
				{
					hSubRet = E_GAMBLEMACHINE_NOT_GAMBLING;
					break;
				}

				if(0 >= kGambleInfo.iRouletteCount)
				{
					hSubRet = E_GAMBLEMACHINE_NOT_HAVE_ROULETTE;
					break;
				}

				--kGambleInfo.iRouletteCount;

				CONT_GAMBLEMACHINERESULT kContResult;
				CONT_GAMBLEITEM kContDumy;
				bool bBroadcast = false;
				hSubRet = g_kGambleMachine.GetGambleRoulette(GCG_CASHSHOP, kContResult, kContDumy, kFindItor->second->UnitRace(), bBroadcast);
				if(S_OK != hSubRet)
				{
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_MODIFY, kOrder.Cause(), kOrder.OwnerGuid());
				PU::TWriteArray_M(kDBChange.kAddonData, kContResult);
				kDBChange.kAddonData.Push(bBroadcast);
				kDBChange.kAddonData.Push(kGambleInfo.iRouletteCount);
				PU::TWriteArray_A(kDBChange.kAddonData, kContDumy);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_GAMBLE_REMOVE:
			{
				hSubRet = S_OK;

				SGambleUserInfo kGambleInfo;
				if(false == m_kGambleUserMgr.Get(kOrder.OwnerGuid(), kGambleInfo))
				{
					hSubRet = E_GAMBLEMACHINE_NOT_GAMBLING;
					break;
				}

				tagDBItemStateChange kDBChange(DISCT_GAMBLE_REMOVE, kOrder.Cause(), kOrder.OwnerGuid());
				PU::TWriteArray_M(kDBChange.kAddonData, kGambleInfo.kContResult);
				kDBChange.kAddonData.Push(kGambleInfo.bBroadcast);
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_EXTEND_CHARACTER_NUM:
			{
				SMOD_Extend_CharacterNum kData;
				kOrder.Read(kData);

				int iMax = 0;
				if(S_OK != g_kVariableContainer.Get(EVar_Kind_DBManager,EVar_DB_MaxExtendCreateCharacterNum,iMax))
				{
					iMax = DEFAULT_EXTEND_CHARACTER_COUNT;
				}

				BYTE const bNewExtendNum = static_cast<BYTE>(pkCopyPlayer->ExtendCharacterNum() + kData.ExtendNum());

				if(static_cast<BYTE>(iMax) < bNewExtendNum)
				{
					hSubRet = E_CANNOT_EXTEND_CHARACTER_NUM;
					
					BM::Stream kPacket( PT_T_C_ANS_ITEM_USE_RESULT);
					kPacket.Push(hSubRet);
					SendToUser(kOrder.OwnerGuid(),kPacket,false);

					break;
				}

				pkCopyPlayer->ExtendCharacterNum(bNewExtendNum);

				hSubRet = S_OK;

				tagDBItemStateChange kDBChange(DISCT_EXTEND_CHARACTER_NUM, kOrder.Cause(), kOrder.OwnerGuid());
				kData.WriteToPacket(kDBChange.kAddonData);
				kDBChange.kAddonData.Push(pkCopyPlayer->GetMemberGUID());
				kChangeArray.push_back(kDBChange);
			}break;
		case IMET_MYHOME_SET_OWNER_INFO:
		case IMET_MYHOME_TEX_TIME:
		case IMET_MYHOME_AUCTION_TIME:
		case IMET_MYHOME_STYLE_MODIFY:
		case IMET_MYHOME_PAY_TEX:
		case IMET_MYHOME_ATTACHMENT:
		case IMET_MYHOME_RETURN_ITEM:
		case IMET_MYHOME_NOTI_PAY_TEX:
		case IMET_MYHOME_AUCTION_UNREG:
		case IMET_MYHOME_AUCTION_END:
		case IMET_MYHOME_BIDDING:
		case IMET_MYHOME_VISITFLAG:
		case IMET_MYHOME_AUCTION_REG:
		case IMET_MYHOME_SET_HOME_STATE:
		case IMET_MYHOME_RESERVE:
		case IMET_MYHOME_SET_HOMEADDR:
		case IMET_SIDEJOB_INSERT:
		case IMET_SIDEJOB_REMOVE:
		case IMET_SIDEJOB_COMPLETE:
		case IMET_MYHOME_SET_EQUIP_COUNT:
		case IMET_SIDEJOB_MODIFY:
		case IMET_ADD_MYHOME:
		case IMET_MYHOME_SET_VISITLOGCOUNT:
		case IMET_MYHOME_ADD_VISITLOGCOUNT:
		case IMET_MYHOME_REMOVE:
			{
				hSubRet = m_kMyHomeMgr.ProcessModifyPlayer(kCause, kOrder, pkCopyPlayer, kAddonPacket, rkPacket, kChangeArray, kContLogMgr);
			}break;
		case IMET_OPEN_MARKET:
		case IMET_ADD_ARTICLE:
		case IMET_REMOVE_ARTICLE:
		case IMET_BUY_ARTICLE:
		case IMET_REMOVE_DEALING:
		case IMET_MODIFY_MARKET_INFO:
		case IMET_MODIFY_MARKET_STATE:
		case IMET_REMOVE_MARKET:
			{
				hSubRet = m_kOpenMarketMgr.Locked_ProcessModifyPlayer_OpenMarket(kCause, kOrder, pkCopyPlayer, kAddonPacket, rkPacket, kChangeArray, kContLogMgr);
			}
			break;
		case IMET_CREATE_MAIL_BYGUID:
			{
				SSendMailInfo kMailData;
				kOrder.Read(kMailData);

				hSubRet = S_OK;

				tagDBItemStateChange kDBChange(DISCT_CREATE_MAIL_BYGUID, kOrder.Cause(), kOrder.OwnerGuid());
			
				kMailData.WriteToPacket(kDBChange.kAddonData);

				BM::GUID kMailGuid;// 로그 기록을 위해 메일 고유 아이디를 외부에서 제공한다.
				kMailGuid = BM::GUID::Create();

				kDBChange.kAddonData.Push(kMailGuid);

				kChangeArray.push_back(kDBChange);

				std::wstring strMail = kMailData.MailTitle() + _T("\r") + kMailData.MailText();
				PgLog kLog(ELOrderMain_Mail,ELOrderSub_Forwarding);
				kLog.Set(0,kMailData.TargetName());
				kLog.Set(1,strMail);
				kLog.Set(0,static_cast<int>(kMailData.PaymentType()));
				kLog.Set(0,kMailData.Money());
				kLog.Set(2,kMailGuid.str());

				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}break;
		case IMET_ADD_EMOTION:
			{
				SPMOD_AddEmotion kData;
				kOrder.Read(kData);

				PgBitArray<MAX_DB_EMOTICON_SIZE> kCont;

				if(false == pkCopyPlayer->GetContEmotion(kData.Type(),kCont))
				{
					hSubRet = E_EM_INVALID_TYPE;
				}
				else
				{
					if(true == kCont.Get(kData.GroupNo()))
					{
						hSubRet = E_EM_ALREADY_HAVE;
					}
					else
					{
						if(true == kCont.Set(kData.GroupNo(),true))
						{
							hSubRet = S_OK;
							pkCopyPlayer->SetContEmotion(kData.Type(),kCont);
							tagDBItemStateChange kDBChange(DISCT_ADD_EMOTION,kOrder.Cause(), kOrder.OwnerGuid());
							kData.WriteToPacket(kDBChange.kAddonData);
							kChangeArray.push_back(kDBChange);
						}
						else
						{
							hSubRet = E_EM_INVALID_EMOTION;
						}
					}
				}
			}break;
		case IMET_CREATE_PORTAL:
			{
				SMOD_Portal_Create kData;
				kOrder.Read(kData);

				CONT_USER_PORTAL const & kContPortal = pkCopyPlayer->ContPortal();
				if(kContPortal.size() >= MAX_USER_PORTAL_NUM)
				{
					hSubRet = E_PORTAL_FULL;
				}
				else
				{
					CONT_USER_PORTAL::mapped_type kPortal(kData.Comment(),kData.GroundNo(),kData.Pos());
					if(false == pkCopyPlayer->InsertPortal(kData.Guid(),kPortal))
					{
						hSubRet = E_PORTAL_ALREADY_EXISTS;
					}
					else
					{
						hSubRet = S_OK;
						tagDBItemStateChange kDBChange(DISCT_CREATE_PORTAL,kOrder.Cause(), kOrder.OwnerGuid());
						kData.WriteToPacket(kDBChange.kAddonData);
						kChangeArray.push_back(kDBChange);
					}
				}
			}break;
		case IMET_DELETE_PORTAL:
			{
				SMOD_Portal_Delete kData;
				kOrder.Read(kData);

				if(false == pkCopyPlayer->RemovePortal(kData.Guid()))
				{
					hSubRet = E_NOT_FOUND_PORTAL;
				}
				else
				{
					hSubRet = S_OK;
					tagDBItemStateChange kDBChange(DISCT_DELETE_PORTAL,kOrder.Cause(), kOrder.OwnerGuid());
					kData.WriteToPacket(kDBChange.kAddonData);
					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_CREATE_CHARACTERCARD:
			{
				SMOD_CreateCard kData;
				kOrder.Read(kData);

				//PgCharacterCard kCard(kOrder.OwnerGuid(),kData.Year(),kData.Sex(),kData.Local(),0);

				PgCharacterCard kCard;
				if(true == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_ALREADY_CREATED;
				}
				else
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_CREATE_CHARACTERCARD,kOrder.Cause(), kOrder.OwnerGuid());
					kData.WriteToPacket(kDBChange.kAddonData);

					PgCharacterCard kTmpCard(kOrder.OwnerGuid(),kData.Year(),kData.Sex(),kData.Local(),0,kData.Comment(),true,kData.Constellation(),kData.Blood(),kData.Hobby(),kData.Style(),0);
					CONT_CARD_ABIL::mapped_type kAbil;
					if(true == kTmpCard.GetCardAbil(kAbil))
					{
						// 혹시라도 여러개 만들걸 대비 하자. 안하면 ㅠㅠ;; 
						VEC_CARD_ABIL kContAbil;
						kContAbil.push_back(kAbil);
						pkCopyPlayer->SetCardAbil(kContAbil);
						kDBChange.kAddonData.Push(kContAbil);
					}

					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_MODIFY_CHARACTERCARD:
			{
				SMOD_ModifyCard kData;
				kOrder.Read(kData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_NOT_FOUND_CARD;
				}
				else
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_MODIFY_CHARACTERCARD,kOrder.Cause(), kOrder.OwnerGuid());
					kData.WriteToPacket(kDBChange.kAddonData);

					PgCharacterCard kTmpCard(kOrder.OwnerGuid(),kData.Year(),kData.Sex(),kData.Local(),0,kData.Comment(),true,kData.Constellation(),kData.Blood(),kData.Hobby(),kData.Style(),0);
					CONT_CARD_ABIL::mapped_type kAbil;
					if(true == kTmpCard.GetCardAbil(kAbil))
					{
						// 혹시라도 여러개 만들걸 대비 하자. 안하면 ㅠㅠ;; 
						VEC_CARD_ABIL kContAbil;
						kContAbil.push_back(kAbil);
						pkCopyPlayer->SetCardAbil(kContAbil);
						kDBChange.kAddonData.Push(kContAbil);
					}

					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_ADD_POPULARPOINT:
			{
				SMOD_AddPopularPoint kData;
				kOrder.Read(kData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_NOT_FOUND_CARD;
				}
				else
				{
					hSubRet = S_OK;

					__int64 const i64NewPP = kCard.PopularPoint() + kData.Point();
					__int64 const i64TodayPP = kCard.TodayPopularPoint() + kData.Point();

					tagDBItemStateChange kDBChange(DISCT_MODIFY_POPULARPOINT,kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(i64NewPP);
					kDBChange.kAddonData.Push(i64TodayPP);
					kDBChange.kAddonData.Push(kData.Owner());
					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_SET_TODAYPOPULARPOINT:
			{
				SMOD_AddPopularPoint kData;
				kOrder.Read(kData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_NOT_FOUND_CARD;
				}
				else
				{
					hSubRet = S_OK;

					__int64 const i64NewPP = kCard.PopularPoint();
					__int64 const i64TodayPP = kData.Point();

					tagDBItemStateChange kDBChange(DISCT_MODIFY_POPULARPOINT,kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(i64NewPP);
					kDBChange.kAddonData.Push(i64TodayPP);
					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_MODIFY_REFRESHDATE:
			{
				SMOD_RefreshDate kData;
				kOrder.Read(kData);

				tagDBItemStateChange kDBChange(DISCT_MODIFY_REFRESHDATE,kOrder.Cause(), kOrder.OwnerGuid());
				kDBChange.kAddonData.Push(kData.Date());
				kChangeArray.push_back(kDBChange);

				hSubRet = S_OK;
			}break;
		case IMET_SET_RECOMMENDPOINT:
			{
				SMOD_SetRecommendPoint kData;
				kOrder.Read(kData);

				pkCopyPlayer->RecommendPoint(kData.RP());

				tagDBItemStateChange kDBChange(DISCT_MODIFY_RECOMMENDPOINT,kOrder.Cause(), kOrder.OwnerGuid());
				kDBChange.kAddonData.Push(kData.RP());
				kChangeArray.push_back(kDBChange);

				hSubRet = S_OK;
			}break;
		case IMET_ADD_RECOMMENDPOINT:
			{
				SMOD_AddRecommendPoint kData;
				kOrder.Read(kData);

				int const iNewVal = pkCopyPlayer->RecommendPoint() + kData.RP();
				if(0 > iNewVal)
				{
					hSubRet = E_NOT_ENOUGH_RP;
				}
				else
				{
					pkCopyPlayer->RecommendPoint(iNewVal);
					tagDBItemStateChange kDBChange(DISCT_MODIFY_RECOMMENDPOINT,kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(iNewVal);
					kChangeArray.push_back(kDBChange);
					hSubRet = S_OK;
				}
			}break;
		case IMET_MODIFY_CARD_COMMENT:
			{
				SMOD_Modify_Comment kData;
				kOrder.Read(kData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_NOT_FOUND_CARD;
				}
				else
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_MODIFY_CARDCOMMENT,kOrder.Cause(), kOrder.OwnerGuid());
					kData.WriteToPacket(kDBChange.kAddonData);
					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_MODIFY_CARD_STATE:
			{
				SMOD_Modify_CardState kData;
				kOrder.Read(kData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kOrder.OwnerGuid(),kCard))
				{
					hSubRet = E_NOT_FOUND_CARD;
				}
				else
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_MODIFY_CARDSTATE,kOrder.Cause(), kOrder.OwnerGuid());
					kData.WriteToPacket(kDBChange.kAddonData);
					kChangeArray.push_back(kDBChange);
				}
			}break;
		case IMET_GUILD_INV_EXTEND_LINE:
		case IMET_GUILD_INV_EXTEND_TAB:
			{
				SMOD_InvExtend kData;
				kOrder.Read(kData);

				BM::GUID kCharGuid;

				BM::Stream kTempPacket(kAddonPacket);
				BM::Stream::STREAM_DATA kTempData;
				kTempPacket.Pop(kTempData);
				BM::Stream kPacket;
				kPacket.Push(&kTempData.at(0), kTempData.size() * sizeof(BM::Stream::STREAM_DATA::value_type));
				kPacket.Pop(kCharGuid);

				kData.InvType(IT_NONE);
				const int iMaxInvIndex = 4;
				
				for( int iCur = 1; iCur <= iMaxInvIndex; ++iCur ) // 길드금고1~4 중에서 확장 가능한 금고를 찾는다.
				{
					int const iInvSize = pkCopyPlayer->GetInven()->GetMaxIDX( static_cast<EInvType>( iCur ) );
					
					if( kOrder.Cause() == IMET_GUILD_INV_EXTEND_LINE )
					{// 한 줄 확장(8칸)
						if( iInvSize < MAX_GUILD_IDX_EXTEND // 한 탭에 최대 확장 할 수 있는 칸
						&&	iInvSize != 0 ) // 0칸이라는 것은 탭 확장을 안했다는 것: 그럼 라인 늘리는 아이템 사용 못함.
						{
							kData.InvType(static_cast<EInvType>( iCur));
							break;
						}
					}
					else if( kOrder.Cause() == IMET_GUILD_INV_EXTEND_TAB )
					{// 탭 하나 추가 하는 아이템
						if( iInvSize == MAX_GUILD_IDX_EXTEND ) // 라인을 최대로 늘린 금고가 존재 해야 다음 탭을 추가 할 수 있다.
						{
							int const iNextInvSize =  pkCopyPlayer->GetInven()->GetMaxIDX( static_cast<EInvType>( iCur + 1 ) );
							if( iNextInvSize < MAX_GUILD_IDX_EXTEND 
							&& iCur < iMaxInvIndex )
							{
								kData.InvType(static_cast<EInvType>( iCur + 1 ));
								break;
							}
						}
					}
				}
				if( IT_NONE == kData.InvType() )
				{
					hSubRet = E_CANNOT_EXTEND;
				}
				else
				{
					int const iOldMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX( kData.InvType() );

					hSubRet = S_OK;
					if( kOrder.Cause() == IMET_GUILD_INV_EXTEND_LINE )
					{
						if( false == pkCopyPlayer->GetInven()->ExtendMaxIdx( kData.InvType(), kData.ExtendNum() ) )
						{
							hSubRet = E_CANNOT_GUILD_INVENTORY_EXTEND_LINE;
						}
					}
					else if( kOrder.Cause() == IMET_GUILD_INV_EXTEND_TAB )
					{
						if( false == pkCopyPlayer->GetInven()->InvExtend( kData.InvType(), kData.ExtendNum() ) )
						{
							hSubRet = E_CANNOT_GUILD_INVENTORY_EXTEND_TAB;
						}
					}

					if( S_OK == hSubRet )
					{
						const __int64 iCause = kOrder.Cause() | IMC_GUILD_INV;

						tagDBItemStateChange kDBChange( DISCT_GUILD_INV_EXTEND, iCause, kOrder.OwnerGuid() );
						kDBChange.kAddonData.Push(hSubRet);
						kDBChange.kAddonData.Push( kOrder.Cause() ); // 줄 확장인지? 탭 확장인지?
						kDBChange.kAddonData.Push( kData.InvType() );
						kDBChange.kAddonData.Push( kData.ExtendNum() );
						kChangeArray.push_back( kDBChange );

						int const iNewMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX( kData.InvType() );

						PgLog kLog(ELOrderMain_Inventory, ELOrderSub_Modify);
						kLog.Set(0,static_cast<int>(kData.InvType()));
						kLog.Set(1,static_cast<int>(kData.ExtendNum()));
						kLog.Set(2,iOldMaxIDX);
						kLog.Set(3,iNewMaxIDX);

						kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
					}
					else
					{
						BM::Stream kPacket(PT_N_C_NFY_GUILD_INV_EXTEND);
						kPacket.Push(hSubRet); // 실패
						SendToUser(kCharGuid,kPacket,false);
					}
				}				
			}break;		
		case IMET_INVENTORY_EXTEND:
			{
				SMOD_InvExtend kData;
				kOrder.Read(kData);

				int const iOldMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX(kData.InvType());

				if(true == pkCopyPlayer->GetInven()->InvExtend(kData.InvType(),kData.ExtendNum()))
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_INVENTORY_EXTEND,kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(kData.InvType());
					kDBChange.kAddonData.Push(kData.ExtendNum());
					kChangeArray.push_back(kDBChange);

					int const iNewMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX(kData.InvType());

					PgLog kLog(ELOrderMain_Inventory,ELOrderSub_Modify);
					kLog.Set(0,static_cast<int>(kData.InvType()));
					kLog.Set(1,static_cast<int>(kData.ExtendNum()));
					kLog.Set(2,iOldMaxIDX);
					kLog.Set(3,iNewMaxIDX);

					kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
				}
				else
				{
					hSubRet = E_CANNOT_EXTEND;
				}
			}break;
		case IMET_EXTEND_MAX_IDX:
			{
				SMOD_InvExtend kData;
				kOrder.Read(kData);

				int const iOldMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX(kData.InvType());

				/*BYTE kReductionNum = 0;

				switch(g_kLocal.ServiceRegion())
				{ // 확장 인벤토리 사용 안함
				case LOCAL_MGR::NC_DEVELOP:
				case LOCAL_MGR::NC_THAILAND:
				case LOCAL_MGR::NC_INDONESIA:
				case LOCAL_MGR::NC_JAPAN:
				case LOCAL_MGR::NC_TAIWAN:
				case LOCAL_MGR::NC_CHINA:
				case LOCAL_MGR::NC_RUSSIA:
				case LOCAL_MGR::NC_KOREA:
				case LOCAL_MGR::NC_EU:
				case LOCAL_MGR::NC_FRANCE:
				case LOCAL_MGR::NC_GERMANY:
				case LOCAL_MGR::NC_SINGAPORE:
				case LOCAL_MGR::NC_PHILIPPINES:
					{

					}break;
				default:
					{
						kReductionNum = pkCopyPlayer->GetInven()->GetInvExtendSize(kData.InvType());
					}break;
				}

				if(0 < kReductionNum)
				{
					hSubRet = E_CANNOT_EXTEND;
				}
				else*/
				{
					if(true == pkCopyPlayer->GetInven()->ExtendMaxIdx(kData.InvType(),kData.ExtendNum()))
					{
						hSubRet = S_OK;

						tagDBItemStateChange kDBChange(DISCT_EXTEND_MAX_IDX,kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kData.InvType());
						kDBChange.kAddonData.Push(kData.ExtendNum());
						kChangeArray.push_back(kDBChange);

						int const iNewMaxIDX = pkCopyPlayer->GetInven()->GetMaxIDX(kData.InvType());

						PgLog kLog(ELOrderMain_Inventory,ELOrderSub_Modify);
						kLog.Set(0,static_cast<int>(kData.InvType()));
						kLog.Set(1,static_cast<int>(kData.ExtendNum()));
						kLog.Set(2,iOldMaxIDX);
						kLog.Set(3,iNewMaxIDX);

						kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
					}
					else
					{
						hSubRet = E_CANNOT_EXTEND;
					}
				}
			}break;
		case IMET_SET_DEFAULT_ITEM:
			{
				SMOD_SetDefaultItem kData;
				kOrder.Read(kData);
				if(true == pkCopyPlayer->SetDefaultItem(kData.EquipPos(),kData.ItemNo()))
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_SET_DEFAULT_ITEM,kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(kData.EquipPos());
					kDBChange.kAddonData.Push(kData.ItemNo());
					kChangeArray.push_back(kDBChange);
				}
				else
				{
					hSubRet = E_FAIL;
				}
			}break;
		case IMET_ADD_UNBIND_DATE:
			{
				SMOD_AddUnbindDate kData;
				kOrder.Read(kData);

				PgInventory* pkInv = pkCopyPlayer->GetInven();
				PgBase_Item kItem;
				if(S_OK != pkInv->GetItem(kData.ItemPos(),kItem))
				{
					hSubRet = E_FAIL;
				}
				else
				{
					__int64 const i64Time = g_kEventView.GetLocalSecTime(CGameTime::SECOND) + (kData.DelayDays() * 24 * 3600);
					BM::DBTIMESTAMP_EX kDBTimeEx;
					CGameTime::SecTime2DBTimeEx(i64Time, kDBTimeEx);

					if(pkInv->AddUnbindDate(kItem.Guid(), kDBTimeEx))
					{
						hSubRet = S_OK;
						tagDBItemStateChange kDBChange(DISCT_ADD_UNBIND_DATE, kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kItem.Guid());
						kDBChange.kAddonData.Push(kData.DelayDays());
						kDBChange.kAddonData.Push(kDBTimeEx);
						kChangeArray.push_back(kDBChange);
					}
				}
			}break;
		case IMET_DEL_UNBIND_DATE:
			{
				SMOD_DelUnbindDate kData;
				kOrder.Read(kData);

				PgInventory* pkInv = pkCopyPlayer->GetInven();
				if(true == pkInv->DelUnbindDate(kData.ItemGuid()))
				{
					hSubRet = S_OK;

					tagDBItemStateChange kDBChange(DISCT_DEL_UNBIND_DATE, kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(kData.ItemGuid());
					kChangeArray.push_back(kDBChange);
				}
				else
				{
					hSubRet = E_FAIL;
				}
			}break;
		case IMET_SET_RENTALSAFETIME:
			{
				tagPlayerModifyOrderData_ModifyRentalSafeTime kData;

				kOrder.Read(kData);

				PgInventory* pkInv = pkCopyPlayer->GetInven();

				hSubRet = pkInv->CheckEnableUseRentalSafe(kData.InvType());

				if(E_ENABLE_TIMEOUT == hSubRet)
				{
					__int64 i64RentalTime = g_kEventView.GetLocalSecTime() + kData.UseTime();

					hSubRet = pkInv->SetRentalSafeEnableTime(kData.InvType(),i64RentalTime);

					if(S_OK == hSubRet)
					{
						BM::DBTIMESTAMP_EX kDBTime;
						CGameTime::SecTime2DBTimeEx(i64RentalTime,kDBTime);
						tagDBItemStateChange kDBChange(DISCT_SET_RENTALSAFETIME, kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kData.InvType());
						kDBChange.kAddonData.Push(kDBTime);
						kDBChange.kAddonData.Push(pkInv->MemberGuid());
						kChangeArray.push_back(kDBChange);
					}
				}
			}break;
		case IMET_SET_ACHIEVEMENT_TIMELIMIT:
			{
				SOD_ModifyAchievementTimeLimit kData;
				kOrder.Read(kData);

				if(false == pkCopyPlayer->GetAchievements()->IsComplete(kData.SaveIdx()))
				{
					hSubRet = E_FAIL;
				}
				else
				{
					__int64 const i64TimeLimit = g_kEventView.GetLocalSecTime() + kData.UseTime();
					BM::DBTIMESTAMP_EX kDbTime;
					CGameTime::SecTime2DBTimeEx(i64TimeLimit, kDbTime);
					BM::PgPackedTime const kPackedTime = BM::PgPackedTime(kDbTime);

					pkCopyPlayer->GetAchievements()->SetAchievementTimeLimit(kData.SaveIdx(),kPackedTime);

					tagDBItemStateChange kDBChange(DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY, kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(kData.SaveIdx());
					kDBChange.kAddonData.Push(kPackedTime);
					kChangeArray.push_back(kDBChange);

					hSubRet = S_OK;
				}
			}break;
		case IMET_SET_ACHIEVEMENT:
			{
				SPlayerModifyOrderData_ModifyAchievement kData;
				kOrder.Read(kData);

				if(kData.SaveValue())
				{
					pkCopyPlayer->GetAchievements()->Complete(kData.SaveIdx());
				}
				else
				{
					pkCopyPlayer->GetAchievements()->Reset(kData.SaveIdx());
					if(true == pkCopyPlayer->GetAchievements()->ResetAchievementTimeLimit(kData.SaveIdx()))
					{
						tagDBItemStateChange kDBChange(DISCT_ACHIEVEMENT_TIMELIMIT_DELETE, kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kData.SaveIdx());
						kChangeArray.push_back(kDBChange);
					}
					else
					{
						CAUTION_LOG(BM::LOG_LV5, _T("Fail ResetAchievementTimeLimit CharGuid<") << pkCopyPlayer->GetID() << _T("> SaveIdx<") << kData.SaveIdx() << _T("> Value<") << kData.SaveValue() << _T(">"));
					}
				}

				tagDBItemStateChange kDBChange(DISCT_SET_ACHIEVEMENT, kOrder.Cause(), kOrder.OwnerGuid());
				kDBChange.kAddonData.Push(kData.SaveIdx());
				kDBChange.kAddonData.Push(kData.SaveValue());

				kChangeArray.push_back(kDBChange);
				hSubRet = S_OK;
			}break;
		case IMET_ACHIEVEMENT2INV:
			{
				hSubRet = OnProcessAhievement2Inv(kOrder, pkCopyPlayer, kChangeArray, kContLogMgr);
			}break;
		case IMET_INV2ACHIEVEMENT:
			{
				SPlayerModifyOrderData_Achievement2Inv kData;
				kOrder.Read(kData);

				if(pkCopyPlayer->GetAchievements()->CheckHasItem(kData.SaveIdx()) || !pkCopyPlayer->GetAchievements()->ResetItem(kData.SaveIdx(),false))
				{
					hSubRet = E_FAIL;
				}
				else
				{
					tagDBItemStateChange kDBChange(DISCT_INV2ACHIEVEMENT, kOrder.Cause(), kOrder.OwnerGuid());
					kDBChange.kAddonData.Push(kData.SaveIdx());
					kChangeArray.push_back(kDBChange);
					hSubRet = S_OK;
				}
			}break;
		case IMET_SAVE_ACHIEVEMENTS:
			{
				pkCopyPlayer->OnAchievementAbilSaveDB(kChangeArray);
				hSubRet = S_OK;
			}break;
		case IMET_COMPLETE_ACHIEVEMENT:
			{
				SPMOD_Complete_Achievement kData;
				kOrder.Read(kData);

				if(pkCopyPlayer->GetAchievements()->IsComplete(kData.SaveIdx())	|| !pkCopyPlayer->GetAchievements()->Complete(kData.SaveIdx()))
				{
					hSubRet = E_FAIL;
				}
				else
				{
					{
						tagDBItemStateChange kDBChange(DISCT_COMPLETE_ACHIEVEMENT, kOrder.Cause(), kOrder.OwnerGuid());
						kData.WriteToPacket(kDBChange.kAddonData);
						kChangeArray.push_back(kDBChange);
					}

					pkCopyPlayer->GetAchievements()->AddAchievementPoint(static_cast< EAchievementsCategory >(kData.Category()), kData.RankPoint());

					{
						tagDBItemStateChange kDBChange(DISCT_MODIFY_ACHIEVEMENT_RANK, kOrder.Cause(), kOrder.OwnerGuid());
						kData.WriteToPacket(kDBChange.kAddonData);
						kChangeArray.push_back(kDBChange);
					}

					if(0 < kData.UseTime())
					{
						__int64 const i64TimeLimit = g_kEventView.GetLocalSecTime() + kData.UseTime();
						BM::DBTIMESTAMP_EX kDbTime;
						CGameTime::SecTime2DBTimeEx(i64TimeLimit, kDbTime);
						BM::PgPackedTime const kPackedTime = BM::PgPackedTime(kDbTime);

						pkCopyPlayer->GetAchievements()->SetAchievementTimeLimit(kData.SaveIdx(),kPackedTime);

						tagDBItemStateChange kDBChange(DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY, kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kData.SaveIdx());
						kDBChange.kAddonData.Push(kPackedTime);
						kChangeArray.push_back(kDBChange);
					}

					pkCopyPlayer->OnAchievementAbilSaveDB(kChangeArray);
					
					// 연관 업적 체크
					OnCheckAchievenmentRelation(kOrder.OwnerGuid(), kData, pkCopyPlayer->GetAchievements());

					hSubRet = S_OK;
				}
			}break;
		case IMET_SET_HIDDEN_OPEN:
			{
				SPMOD_Complete_HiddenOpen kData;
				kOrder.Read(kData);

				BM::DBTIMESTAMP_EX kLastHiddenUpdate;

				pkCopyPlayer->GetHiddenOpen()->Init();
				pkCopyPlayer->GetHiddenOpen()->GetDateBuffer(kLastHiddenUpdate);

				tagDBItemStateChange kDBChange(DISCT_SET_HIDDEN_OPEN, kOrder.Cause(), kOrder.OwnerGuid());
				kDBChange.kAddonData.Push( kLastHiddenUpdate );

				kChangeArray.push_back(kDBChange);
				hSubRet = S_OK;
			}break;
		case IMET_COMPLETE_HIDDEN_OPEN:
			{
				SPMOD_Complete_HiddenOpen kData;
				kOrder.Read(kData);

				if(pkCopyPlayer->GetHiddenOpen()->IsComplete(kData.SaveIdx())	|| !pkCopyPlayer->GetHiddenOpen()->Complete(kData.SaveIdx()))
				{
					hSubRet = E_FAIL;
				}
				else
				{
					{
						tagDBItemStateChange kDBChange(DISCT_COMPLETE_HIDDEN_OPEN, kOrder.Cause(), kOrder.OwnerGuid());
						kDBChange.kAddonData.Push(kData.SaveIdx());
						kChangeArray.push_back(kDBChange);
					}
					hSubRet = S_OK;
				}
			}break;
		case IMET_ADD_CASH:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot use this function ... IMET_ADD_CASH"));
			}break;
		case IMET_CASH_SHOP_GIFT:
			{
				SPlayerModifyOrderData_ModifyCashShopGift kData;
				kOrder.Read(kData);
				size_t const iRdPos = rkPacket.RdPos();
				EItemModifyParentEventType kTempCause;
				rkPacket.Pop(kTempCause);	// 필요없는 값이지만, Job에서 공통으로 이렇게 사용하고 있으니...기존로직을 따른다.
				PgTranPointer kTran(rkPacket);
				rkPacket.RdPos(iRdPos);
				if (CSR_SUCCESS == PgCashShopMgr::OnProcessReqGiftArticle(kTran))
				{
					hSubRet = S_OK;
				}
			}break;
		case IMET_CREATE_MAIL:
			{
				SSendMailInfo kData;
				kOrder.Read(kData);

				tagDBItemStateChange kDBChange(DISCT_CREATE_MAIL, kOrder.Cause(), kOrder.OwnerGuid());
				
				kData.WriteToPacket(kDBChange.kAddonData);

				BM::GUID kMailGuid;// 로그 기록을 위해 메일 고유 아이디를 외부에서 제공한다.
				kMailGuid = BM::GUID::Create();

				kDBChange.kAddonData.Push(kMailGuid);

				kChangeArray.push_back(kDBChange);

				hSubRet = S_OK;//메일 보내는 부분은 무조건 성공 한다.

				std::wstring strMail = kData.MailTitle() + _T("\r") + kData.MailText();
				PgLog kLog(ELOrderMain_Mail,ELOrderSub_Forwarding);
				kLog.Set(0,kData.TargetName());
				kLog.Set(1,strMail);
				kLog.Set(0,static_cast<int>(kData.PaymentType()));
				kLog.Set(0,kData.Money());
				kLog.Set(2,kMailGuid.str());

				kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
			}break;		
		case IMET_ADD_ABIL:
			{
				SPMOD_AddAbil kData;
				kOrder.Read(kData);

				if ( kData.AbilType() > 0 && kData.Value() != 0i64 )
				{
					if( pkCopyPlayer->AddAbil(kData.AbilType(), static_cast<int>(kData.Value())))
					{
						hSubRet = S_OK;//AddAbil은 무조건 성공.

						pkCopyPlayer->UpdateModifyAchievementsAbil(kData.AbilType());
					}

					bool const bQuery = pkCopyPlayer->IsQuery(kData.AbilType());

					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.IsQuery(bQuery);

					kDBData.kAddonData.Push(kData.AbilType());
					kDBData.kAddonData.Push( static_cast<__int64>(pkCopyPlayer->GetAbil(kData.AbilType())));
					kChangeArray.push_back(kDBData);

					if ( bQuery )
					{
						pkCopyPlayer->ClearChecker(kData.AbilType());
					}

					if(kData.AbilType() == AT_SP)
					{
						int const iNowSP = pkCopyPlayer->GetAbil(AT_SP);
						int const iOldSP = iNowSP - static_cast<int>(kData.Value());
						//SP 로그
						PgLog kLog(ELOrderMain_SP, ELOrderSub_Modify);
						kLog.Set(0, iOldSP);
						kLog.Set(1, iNowSP);
						kLog.Set(2, static_cast<int>(kData.Value()));
						kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
					}
				}
				else
				{
					INFO_LOG( BM::LOG_LV3, __FL__ << _T("[IMET_ADD_ABIL] Invalid AbilType=") << kData.AbilType() << _T(", Value=") << kData.Value()
						<< _T(", C-G=") << kOrder.OwnerGuid());
				}
			}break;
		case IMET_SET_ABIL:
			{
				SPMOD_AddAbil kData;
				kOrder.Read(kData);

				int const iOldClass = pkCopyPlayer->GetAbil(AT_CLASS);

				if( pkCopyPlayer->SetAbil( kData.AbilType(),static_cast<int>(kData.Value())) )
				{
					pkCopyPlayer->UpdateModifyAchievementsAbil(kData.AbilType());

					hSubRet = S_OK;

					bool const bQuery = pkCopyPlayer->IsQuery(kData.AbilType());

					tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.IsQuery(bQuery);

					kDBData.kAddonData.Push(kData.AbilType());
					kDBData.kAddonData.Push(kData.Value());

					if ( bQuery )
					{
						pkCopyPlayer->ClearChecker(kData.AbilType());
					}

					switch(kData.AbilType())
					{
					case AT_LEVEL:
						{
							PgActionCouponEvent kEvent(ET_CHANGE_LEVEL,pkCopyPlayer);

							kDBData.kAddonData.Push(pkCopyPlayer->GetMemberGUID());
							kDBData.kAddonData.Push(pkCopyPlayer->PgPlayerPlayTime::GetTotalConnSec_Character());
							kDBData.kAddonData.Push(pkCopyPlayer->GetAbil(AT_LEVEL_RANK));
						}break;
					case AT_CLASS:
						{
							PgActionCouponEvent kEvent(ET_CHANGE_CLASS,pkCopyPlayer);

							// 전직 로그
							int const iClass = pkCopyPlayer->GetAbil(AT_CLASS);
							int const iLevel = pkCopyPlayer->GetAbil(AT_LEVEL);

							PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Change_Info);
							kLogCont.MemberKey(pkCopyPlayer->GetMemberGUID());
							kLogCont.UID(pkCopyPlayer->UID());
							kLogCont.CharacterKey(pkCopyPlayer->GetID());
							kLogCont.ID(pkCopyPlayer->MemberID());
							kLogCont.Name(pkCopyPlayer->Name());
							kLogCont.Class(static_cast<short>(iClass));
							kLogCont.Level(static_cast<short>(iLevel));
							kLogCont.ChannelNo(pkCopyPlayer->GetChannel());
							kLogCont.GroundNo(pkCopyPlayer->GetAbil(AT_RECENT));

							GET_DEF(PgClassDefMgr, kClassDefMgr);
							SPlayerBasicInfo kBasicInfo;
							kClassDefMgr.GetAbil(SClassKey(iClass, iLevel), kBasicInfo);
							std::wstring kState;
							wchar_t wstrBuff[100];
							swprintf_s(wstrBuff, L"%hd / %hd / %hd / %hd / %hd / %hd", kBasicInfo.sInitStr,	kBasicInfo.sInitInt, 
								kBasicInfo.sInitDex, kBasicInfo.sInitCon, kBasicInfo.sPhysicsDefence, kBasicInfo.sMagicDefence);
							kState = wstrBuff;

							PgLog kLog(ELOrderMain_ChangeJob, ELOrderSub_Modify);
							kLog.Set(1, kState);
							kLog.Set(0, iOldClass);
							kLog.Set(2, pkCopyPlayer->GetAbil(AT_GENDER) );
							kLog.Set(0, pkCopyPlayer->GetAbil64(AT_EXPERIENCE));
							kLog.Set(4, pkCopyPlayer->GetTotalConnSec_Character() );

							kLogCont.Add(kLog);
							kLogCont.Commit();
						}break;
					case AT_SP:
						{
							// SP 수정 로그(GM 커맨드로 셋팅했을때)
							int const iOldSP = pkCopyPlayer->GetAbil(AT_SP);
							int const iNowSP = static_cast<int>(kData.Value());
							//SP 로그
							PgLog kLog(ELOrderMain_SP, ELOrderSub_Modify);
							kLog.Set(0, iOldSP);
							kLog.Set(1, iNowSP);
							kLog.Set(2, 0);		//SP를 GM 커맨드로 셋팅 했을때 이기 때문에 차는 항상 0이다.
							kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
						}break;
					case AT_STRATEGYSKILL_OPEN:
						{
							if(pkCopyPlayer->IsOpenStrategySkill(ESTT_SECOND))
							{
								int iReturnSP1 = 0, iReturnSP2 = 0;
								pkCopyPlayer->SetStrategyMySkill(pkCopyPlayer->GetMySkill());
								pkCopyPlayer->GetStrategyMySkill()->EraseSkill(SDT_Normal, &iReturnSP1);
								pkCopyPlayer->GetStrategyMySkill()->EraseSkill(SDT_Special, &iReturnSP2);
								pkCopyPlayer->GetStrategyMySkill()->ContSkillSet(CONT_USER_SKILLSET());

								int const iMySP = pkCopyPlayer->GetAbil(AT_SP) + iReturnSP1 + iReturnSP2;
								pkCopyPlayer->SetStrategySP(iMySP);
							}
						}break;
					}

					kChangeArray.push_back(kDBData);
				}

			}break;
		case IMET_ADD_ABIL64EX:
			{
				SPMOD_AddAbilEX kData;
				kOrder.Read(kData);

				if ( kData.Value() != 0i64 )
				{
					bool bQuery = false;
					hSubRet = OnProcessDBAddAbil64(pkCopyPlayer, kData.AbilType(), kData.Value(), kContLogMgr, kChangeArray, bQuery);
					if(S_OK == hSubRet)
					{
						__int64 iOrgValue = kData.OrgValue();
						__int64 iExtraValue = kData.ExtraValue();
						if( AT_EXPERIENCE==kData.AbilType() )
						{
							int iAddRate = 0;
							if( S_PST_AddExperience const* pkPremiumAddExp = pkCopyPlayer->GetPremium().GetType<S_PST_AddExperience>() )
							{
								iAddRate += pkPremiumAddExp->iRate;
							}
							if( 0 != iAddRate )
							{
								iOrgValue += SRateControl::GetValueRate<int>( iOrgValue, iAddRate );
								iOrgValue = __max(iOrgValue,0);

								iExtraValue += SRateControl::GetValueRate<int>( iExtraValue, iAddRate );
								iExtraValue = __max(iExtraValue,0);
							}
							pkCopyPlayer->CalcExpRate(iOrgValue);
							pkCopyPlayer->CalcExpRate(iExtraValue);
						}
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUEEX, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.IsQuery(bQuery);
						kDBData.kAddonData.Push(kData.AbilType());
						kDBData.kAddonData.Push( pkCopyPlayer->GetAbil64(kData.AbilType()) );
						kDBData.kAddonData.Push( iOrgValue );
						kDBData.kAddonData.Push( iExtraValue );
						kChangeArray.push_back(kDBData);
					}
				}
				else
				{
					INFO_LOG( BM::LOG_LV0, __FL__ << _T("AddAbil64[") << kData.AbilType() << _T("]'s Value is 0 / OrderGuid[") << kOrder.OwnerGuid().str().c_str() << _T("]") );
				}
			}break;
		case IMET_ADD_ABIL64:
			{
				SPMOD_AddAbil kData;
				kOrder.Read(kData);

				if ( kData.Value() != 0i64 )
				{
					bool bQuery = false;
					hSubRet = OnProcessDBAddAbil64(pkCopyPlayer, kData.AbilType(), kData.Value(), kContLogMgr, kChangeArray, bQuery);
					if(S_OK == hSubRet)
					{
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.IsQuery(bQuery);
						kDBData.kAddonData.Push(kData.AbilType());
						kDBData.kAddonData.Push( pkCopyPlayer->GetAbil64(kData.AbilType()) );
						kChangeArray.push_back(kDBData);
					}
				}
				else
				{
					INFO_LOG( BM::LOG_LV0, __FL__ << _T("AddAbil64[") << kData.AbilType() << _T("]'s Value is 0 / OrderGuid[") << kOrder.OwnerGuid().str().c_str() << _T("]") );
				}
			}break;
		case IMET_SET_ABIL64:
			{
				SPMOD_AddAbil kData;
				kOrder.Read(kData);
				switch(kData.AbilType())
				{
				case AT_EXPERIENCE:
					{
						__int64 i64OldExp = pkCopyPlayer->GetAbil64(AT_EXPERIENCE);
						hSubRet = pkCopyPlayer->SetExp(kData.Value(), kChangeArray);
						if (hSubRet == S_OK)
						{
							//경험치 로그
							PgLog kExpLog(ELOrderMain_Exp, ELOrderSub_Modify);
							__int64 i64Exp = pkCopyPlayer->GetAbil64(AT_EXPERIENCE);
							kExpLog.Set(0, i64Exp);					// 현재 경험치
							kExpLog.Set(1, i64OldExp);					// 이전 경험치
							kExpLog.Set(2, i64Exp - i64OldExp);		// 경험치 차
							kContLogMgr.AddLog(kOrder.OwnerGuid(),kExpLog);
						}
					}break;
				case AT_FRAN:
					{
						__int64 i64OldFran = pkCopyPlayer->GetAbil64(AT_FRAN);
						hSubRet = pkCopyPlayer->SetFran(kData.Value());
						if (hSubRet == S_OK)
						{
							// Fran 로그
							PgLog kFranLog(ELOrderMain_Exp, ELOrderSub_Modify);
							__int64 i64Fran = pkCopyPlayer->GetAbil64(AT_FRAN);
							kFranLog.Set(3, i64OldFran);
							kFranLog.Set(4, i64Fran - i64OldFran);
							kContLogMgr.AddUpdateLog(kOrder.OwnerGuid(), kFranLog);
						}
					}break;
				case AT_TACTICS_EXP:
					{
						__int64 i64OldExp = pkCopyPlayer->GetAbil64(AT_TACTICS_EXP);
						hSubRet = pkCopyPlayer->SetTacticsExp(kData.Value(), kChangeArray);
						//용병경험치 로그
						PgLog kExpLog(ELOrderMain_TacticsExp, ELOrderSub_Modify);
						__int64 i64Exp = pkCopyPlayer->GetAbil64(AT_TACTICS_EXP);
						kExpLog.Set(0, i64Exp);					// 현재 경험치
						kExpLog.Set(1, i64OldExp);					// 이전 경험치
						kExpLog.Set(2, i64Exp - i64OldExp);		// 경험치 차
						kContLogMgr.AddLog(kOrder.OwnerGuid(),kExpLog);					
					}break;
				default:
					{
						if(pkCopyPlayer->SetAbil64(kData.AbilType(), kData.Value()))
						{
							hSubRet = S_OK;
						}
						else
						{
							hSubRet = E_FAIL;
						}
					}break;
				}
				
				bool const bQuery = pkCopyPlayer->IsQuery(kData.AbilType());

				tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, kOrder.Cause(), kOrder.OwnerGuid());
				kDBData.IsQuery(bQuery);

				kDBData.kAddonData.Push(kData.AbilType());
				kDBData.kAddonData.Push(static_cast<__int64>(kData.Value()));

				kChangeArray.push_back(kDBData);

				if ( bQuery )
				{
					pkCopyPlayer->ClearChecker(kData.AbilType());
					// LOG

					EOrderMainType eOrderMain = ELOrderMain_None;
					switch(kData.AbilType())
					{
					case AT_EXPERIENCE:
						{
							eOrderMain = ELOrderMain_Exp;
						}break;
					case AT_FRAN:
						{
							eOrderMain = ELOrderMain_Fran;
						}break;
					}

					if (eOrderMain != ELOrderMain_None)
					{
						PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_DBSave, pkCopyPlayer->GetMemberGUID(), pkCopyPlayer->GetID(), pkCopyPlayer->MemberID(),
							pkCopyPlayer->Name(), pkCopyPlayer->GetAbil(AT_CLASS), pkCopyPlayer->GetAbil(AT_LEVEL), 0);
						PgLog kLog2(eOrderMain, ELOrderSub_DBSave);
						kLog2.Set(0, pkCopyPlayer->GetAbil64(kData.AbilType()));
						kLogCont.Add(kLog2);
						kLogCont.Commit();
					}
				}
			}break;
		case IMET_SET_SKILLEXTEND:
			{
				SPMOD_SetSkillExtend kData;
				kOrder.Read(kData);

				hSubRet = S_OK;

				PgMySkill* pkMySkill = pkCopyPlayer->GetMySkill();

				if(false == pkMySkill->SetExtendLevel(kData.SkillNo(), kData.ExtendLevel()))
				{
					hSubRet = E_FAIL;
					break;
				}

				PgMySkill* pkMySecondSkill = pkCopyPlayer->GetStrategyMySkill();
				pkMySecondSkill ? pkMySecondSkill->SetExtendLevel(kData.SkillNo(), kData.ExtendLevel()) : 0;

				tagDBItemStateChange kDBData(DISCT_MODIFY_SKILLEXTEND, kOrder.Cause(), kOrder.OwnerGuid());
				kData.WriteToPacket(kDBData.kAddonData);
				kChangeArray.push_back(kDBData);//리셋 타입은1
			}break;
		case IMET_GM_INIT_SKILL:
			{
				SPlayerStrategySkillData kData;
				kOrder.m_kPacket.Pop( kData );

				PgMySkill* pkMySkill = pkCopyPlayer->GetMySkill();
				if( pkMySkill )
				{
					pkMySkill->Init();
					pkMySkill->Add(MAX_DB_SKILL_SIZE, kData.abySkills);
					pkMySkill->InitSkillExtend(MAX_DB_SKILL_EXTEND_SIZE, kData.abySkillExtends);

					tagDBItemStateChange kDBData(DISCT_MODIFY_GM_INIT_SKILL, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push(kData);
					kChangeArray.push_back(kDBData);

					hSubRet = S_OK;
				}
			}break;
		case IMET_SET_STRATEGYSKILL_TABNO:
			{
				if( pkCopyPlayer->GetQInven() )
				{
					SPMOD_ADD_StrategySkillTabChange kData( *pkCopyPlayer->GetQInven() );
					kOrder.Read(kData);

					if( pkCopyPlayer->SwapMySkill(static_cast<ESkillTabType>(kData.iValue)) )
					{
						hSubRet = S_OK;
						tagDBItemStateChange kDBData(DISCT_MODIFY_STRATEGYSKILL, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.kAddonData.Push(kData.iValue);
						kDBData.kAddonData.Push(pkCopyPlayer->GetAbil(AT_SP));
						pkCopyPlayer->GetMySkill()->WriteToPacket(WT_DEFAULT, kDBData.kAddonData);
						pkCopyPlayer->GetQInven()->WriteToPacket(kDBData.kAddonData);
						kChangeArray.push_back(kDBData);
					}
				}
			}break;
		case IMET_ADD_SKILL:
			{
				SPMOD_AddSkill kData;
				kOrder.Read(kData);
				int iMySP = 0;
		 
				PgMySkill* pkMySkill = pkCopyPlayer->GetMySkill();
				if(kData.IsReset())
				{
					int iReturnSP1 = 0, iReturnSP2 = 0;
					pkMySkill->EraseSkill(SDT_Normal, &iReturnSP1);	// Skill 지우고
					pkMySkill->EraseSkill(SDT_Special, &iReturnSP2);
					iMySP = pkCopyPlayer->GetAbil(AT_SP) + iReturnSP1 + iReturnSP2;
					pkCopyPlayer->SetAbil(AT_SP, iMySP);	// SP 회복시켜 주고
					hSubRet = S_OK;
				
					{
						tagDBItemStateChange kDBData(DISCT_MODIFY_SKILL, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.kAddonData.Push(static_cast<int>(1));
						kDBData.kAddonData.Push(static_cast<int>(0));
						kChangeArray.push_back(kDBData);//리셋 타입은1
					}

					{
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, kOrder.OwnerGuid());
						kDBData.kAddonData.Push(static_cast<int>(AT_SP));
						kDBData.kAddonData.Push(static_cast<__int64>(iMySP));
						kChangeArray.push_back(kDBData);
					}
				}
				else
				{
					int const m_iLevel = pkCopyPlayer->GetAbil(AT_LEVEL);
					int const m_iClass = pkCopyPlayer->GetAbil(AT_CLASS);
					iMySP = pkCopyPlayer->GetAbil(AT_SP);
					ELearnSkillRet iRet = pkMySkill->LearnSkillCheck(kData.SkillNo(), m_iLevel, m_iClass);
					if( iRet == LS_RET_SUCCEEDED )
					{
						hSubRet = pkMySkill->LearnNewSkill(kData.SkillNo());
					}
					else
					{
						hSubRet = E_FAIL;
					}

					tagDBItemStateChange kDBData(DISCT_MODIFY_SKILL, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push(static_cast<int>(0));
					kDBData.kAddonData.Push(kData.SkillNo());

					kChangeArray.push_back(kDBData);
				}

				//스킬 습득 로그
				PgLog kLog(ELOrderMain_Skill, ELOrderSub_Modify);
				kLog.Set(0, kData.SkillNo());

				GET_DEF( CSkillDefMgr, kSkillDefMgr );
				CSkillDef const *pkDef = kSkillDefMgr.GetDef(kData.SkillNo());
				if (pkDef == NULL)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Cannot find SkillDef SkillNo=") << kData.SkillNo());
				}
				else
				{
					kLog.Set(1, pkDef->GetAbil(AT_LEVEL));
					kLog.Set(2, iMySP);
					kLog.Set(3, kData.IsReset());
					kLog.Set(4, pkCopyPlayer->GetAbil(AT_GENDER) );
					kLog.Set(0, static_cast< __int64 >(pkDef->GetAbil(AT_SKILL_DIVIDE_TYPE)));
					kContLogMgr.AddLog(kOrder.OwnerGuid(),kLog);
				}
			}break;
		case IMET_DEL_SKILL:
			{
				SPMOD_DelSkill kData;
				kOrder.Read(kData);
				PgMySkill* pkMySkill = pkCopyPlayer->GetMySkill();
				if( 0 != kData.SkillNo() )
				{
					bool const bRet = pkMySkill->Delete(kData.SkillNo() );
					if( true == bRet)
					{
						tagDBItemStateChange kDBData(DISCT_DELETE_SKILL, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.kAddonData.Push(static_cast<int>(1));
						kDBData.kAddonData.Push(static_cast<int>(kData.SkillNo()));
						kChangeArray.push_back(kDBData);//리셋 타입은1

						hSubRet = S_OK;
					}
					else
					{
						hSubRet = E_FAIL;
					}
				}
			}break;
		case IMET_ADD_INGQUEST:
			{
				SPMOD_AddIngQuest kData;
				kOrder.Read(kData);

				switch( kData.TargetState() )
				{
				case QS_Begin:
					{
						if( !pkCopyPlayer->IsQuestBegin() )
						{
							hSubRet = E_CANT_DEFEND_REWORD_FAIL;
							break;
						}

						if( pkCopyPlayer->AddIngQuest(kData.QuestID()) )
						{
							hSubRet = S_OK;
							tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_ING, kOrder.Cause(), kOrder.OwnerGuid());
							kDBData.kAddonData.Push(kData.QuestID());
							kDBData.kAddonData.Push(kData.TargetState());
							kChangeArray.push_back(kDBData);

							TBL_DEF_QUEST_REWARD const* pkQuestDef = NULL;
							if( g_kQuestMng.GetQuestDef(kData.QuestID(), pkQuestDef) )
							{
								PgQuestLogUtil::IngLog(kContLogMgr, ELOrderSub_Start, kOrder.OwnerGuid(), kData.QuestID(), static_cast< int >(pkQuestDef->iDBQuestType), pkQuestDef->iLevelMin);
							}
						}
						else
						{
							hSubRet = E_CANT_BEGINQUEST;
						}
						// 추가
					}break;
				case QS_Finished:
					{
						if( !pkCopyPlayer->IsQuestFinish() )
						{
							hSubRet = E_CANT_DEFEND_REWORD_FAIL;
							break;
						}
					}//No break;
				case QS_None:				
					{
						ContQuestID kVec;
						int iSelectedID = 0;
						bool const bComplete = QS_Finished == kData.TargetState();
						// 삭제
						if( pkCopyPlayer->DelIngQuest(kData.QuestID(), bComplete, kVec, iSelectedID) )
						{
							int const iClass = pkCopyPlayer->GetAbil(AT_CLASS);
							int const iLevel = pkCopyPlayer->GetAbil(AT_LEVEL);
							hSubRet = S_OK;

							tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_ING, kOrder.Cause(), kOrder.OwnerGuid());
							kDBData.kAddonData.Push(kData.QuestID());
							kDBData.kAddonData.Push(kData.TargetState());
							kChangeArray.push_back(kDBData);

							if( bComplete )
							{
								int iClearQuest = 0;
								{
									EQuestType eQuestType = static_cast<EQuestType>( PgQuestInfoUtil::GetQuestDefType( kData.QuestID() ) );
									
									if( QT_Wanted == eQuestType )
									{
										PgMyQuest const* pkMyQuest = pkCopyPlayer->GetMyQuest();
										if( pkMyQuest )
										{
											CONT_DEF_QUEST_WANTED const *pkContWantedQuest = NULL;
											g_kTblDataMgr.GetContDef( pkContWantedQuest );
											if( pkContWantedQuest )
											{
												int iRewardClearCount = 0;

												CONT_DEF_QUEST_WANTED::const_iterator find_iter = pkContWantedQuest->find( kData.QuestID() );
												if( pkContWantedQuest->end() != find_iter )
												{
													CONT_DEF_QUEST_WANTED::mapped_type const& rkQuestWanted = (*find_iter).second;
													iRewardClearCount = rkQuestWanted.iClearCount;
												}
												
												{// 퀘스트 클리어 카운트 1증가													
													pkCopyPlayer->AddWantedQuestClearCount( kData.QuestID() );
													tagDBItemStateChange kDBQuest(DISCT_UPDATE_QUEST_CLEAR_COUNT, kOrder.Cause(), kOrder.OwnerGuid());
													kDBQuest.kAddonData.Push(kData.QuestID());
													kChangeArray.push_back(kDBQuest);
												}

												if( pkCopyPlayer->GetWantedQuestClearCount(kData.QuestID()) >= iRewardClearCount )
												{//이번에 진짜 완료다.
													iClearQuest = 1;
													tagDBItemStateChange kDBQuest(DISCT_MODIFY_QUEST_END, kOrder.Cause(), kOrder.OwnerGuid());
													kDBQuest.kAddonData.Push(kData.QuestID());
													kDBQuest.kAddonData.Push(iClearQuest);
													kChangeArray.push_back(kDBQuest);

													PgQuestLogUtil::IngLog(kContLogMgr, ELOrderSub_Delete, kOrder.OwnerGuid(), kData.QuestID(), iClass, iLevel, PgQuestInfoUtil::GetQuestDefType(kData.QuestID()));
													PgQuestLogUtil::EndLog(kContLogMgr, ELOrderSub_End, kOrder.OwnerGuid(), kData.QuestID());
												}
												else
												{
													PgQuestLogUtil::IngLog( kContLogMgr, ELOrderSub_Delete, kOrder.OwnerGuid(), kData.QuestID(), iClass, iLevel, PgQuestInfoUtil::GetQuestDefType(kData.QuestID()) );
												}
											}
										}
									}
									else
									{
										iClearQuest = 1;
										tagDBItemStateChange kDBQuest(DISCT_MODIFY_QUEST_END, kOrder.Cause(), kOrder.OwnerGuid());
										kDBQuest.kAddonData.Push(kData.QuestID());
										kDBQuest.kAddonData.Push(iClearQuest);
										kChangeArray.push_back(kDBQuest);

										PgQuestLogUtil::IngLog(kContLogMgr, ELOrderSub_Delete, kOrder.OwnerGuid(), kData.QuestID(), iClass, iLevel, PgQuestInfoUtil::GetQuestDefType(kData.QuestID()));
										PgQuestLogUtil::EndLog(kContLogMgr, ELOrderSub_End, kOrder.OwnerGuid(), kData.QuestID());
									}
								}

								iClearQuest = 1;
								ContQuestID::const_iterator loop_iter = kVec.begin();
								while( kVec.end() != loop_iter )
								{
									int const iQuestID = (*loop_iter);
									tagDBItemStateChange kDBQuest(DISCT_MODIFY_QUEST_END, kOrder.Cause(), kOrder.OwnerGuid());
									kDBQuest.kAddonData.Push(iQuestID);
									kDBQuest.kAddonData.Push(iClearQuest);
									kChangeArray.push_back(kDBQuest);

									PgQuestLogUtil::EndLog(kContLogMgr, ELOrderSub_End, kOrder.OwnerGuid(), iQuestID);
									++loop_iter;
								}

								if( iSelectedID )
								{
									iClearQuest = 0;
									tagDBItemStateChange kDBQuest(DISCT_MODIFY_QUEST_END, kOrder.Cause(), kOrder.OwnerGuid());
									kDBQuest.kAddonData.Push(iSelectedID);
									kDBQuest.kAddonData.Push(iClearQuest);
									kChangeArray.push_back(kDBQuest);

									PgQuestLogUtil::EndLog(kContLogMgr, ELOrderSub_DeEnd, kOrder.OwnerGuid(), iSelectedID);
								}
							}
							else
							{
								PgQuestLogUtil::IngLog(kContLogMgr, ELOrderSub_Delete, kOrder.OwnerGuid(), kData.QuestID(), iClass, iLevel);
							}
						}
						else
						{
							hSubRet = E_CANT_DELQUEST;
						}
					}break;
				case QS_Ing:
				case QS_End:
				case QS_Failed:
					{
						int iPreQuestState = QS_None;
						SUserQuestState const* pkPreState = pkCopyPlayer->GetQuestState(kData.QuestID());
						if( pkPreState )
						{
							iPreQuestState = pkPreState->byQuestState;
						}

						if( pkCopyPlayer->SetIngQuest(kData.QuestID(), kData.TargetState()) )
						{
							tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_ING, kOrder.Cause(), kOrder.OwnerGuid());
							kDBData.kAddonData.Push(kData.QuestID());
							kDBData.kAddonData.Push(kData.TargetState());
							kChangeArray.push_back(kDBData);
							hSubRet = S_OK;

							PgQuestLogUtil::IngLog(kContLogMgr, ELOrderSub_ChangeState, kOrder.OwnerGuid(), kData.QuestID(), iPreQuestState, kData.TargetState());
						}
						else
						{
							hSubRet = E_CANT_UPDATEQUEST;
						}
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Can't handling event type: ") << kData.TargetState() );
					}break;
				}
			}break;
		case IMET_CLEAR_ING_QUEST:
			{
				SPMOD_AddIngQuest kData;
				kOrder.Read(kData);

				pkCopyPlayer->ClearIngQuest();

				tagDBItemStateChange kDBData(DISCT_MODIFY_CLEAR_ING_QUEST, kOrder.Cause(), kOrder.OwnerGuid());
				kChangeArray.push_back(kDBData);

				hSubRet = S_OK;
			}break;
			// 나중에 사용할 일 있을거야.
		/*case IMET_INIT_WANTED_QUEST:
			{// 현상수배 퀘스트 클리어 카운트 초기화
				pkCopyPlayer->InitWantedQuestClearCount();
				tagDBItemStateChange kDBQuest(DISCT_INIT_QUEST_CLEAR_COUNT, kOrder.Cause(), kOrder.OwnerGuid());
				kDBQuest.kAddonData.Push(kOrder.OwnerGuid());
				kChangeArray.push_back(kDBQuest);
				hSubRet = S_OK;
			}break;
		*/
		case IMET_ADD_INGQUEST_PARAM:
			{
				SPMOD_IngQuestParam kData;
				kOrder.Read(kData);

				int iLastParam = 0, iPreParam = 0;;
				if( pkCopyPlayer->AddIngQuestParam(kData.QuestID(), kData.ParamNo(), kData.Value(), kData.MaxValue(), kData.Set(), iLastParam, iPreParam) )
				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_PARAM, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push(kData.QuestID());
					kDBData.kAddonData.Push(static_cast<int>(kData.ParamNo()));
					kDBData.kAddonData.Push(iLastParam);
					kChangeArray.push_back(kDBData);
					hSubRet = S_OK;

					PgQuestLogUtil::ParamLog(kContLogMgr, ELOrderSub_Modify, kOrder.OwnerGuid(), kData.QuestID(), kData.ParamNo(), iPreParam, iLastParam);
				}
				else
				{
					hSubRet = E_CANT_UPDATEQUESTPARAM;
				}
			}break;
		case IMET_ADD_ENDQUEST:
			{
				SPMOD_SetEndQuest kData;
				kOrder.Read(kData);

				bool bRun = !kData.NeedPrevStatus();
				if( kData.NeedPrevStatus() )
				{
					PgMyQuest const* pkMyQuest = pkCopyPlayer->GetMyQuest();
					if( pkMyQuest )
					{
						bRun = (pkMyQuest->IsEndedQuest(kData.QuestID()) == kData.PrevClear());
					}
				}

				if( bRun )
				{
					pkCopyPlayer->AddEndQuest(kData.QuestID(), kData.TargetClear());

					tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_END, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push(kData.QuestID());
					kDBData.kAddonData.Push(static_cast<int>(kData.TargetClear()));
					kChangeArray.push_back(kDBData);

					PgQuestLogUtil::EndLog(kContLogMgr, ((kData.TargetClear())? ELOrderSub_End: ELOrderSub_DeEnd), kOrder.OwnerGuid(), kData.QuestID());
					
					hSubRet = S_OK;
				}
			}break;
		case IMET_PRE_CHECK_WANTED_QUEST:
			{	
				ContQuestID kVec;
				g_kQuestMng.GetQuestWanted( pkCopyPlayer->GetAbil(AT_LEVEL), kVec);
				if( kVec.empty() )
				{
					pkCopyPlayer->SetExistCanBuildWantedQuest(false);
				}
				else
				{
					pkCopyPlayer->SetExistCanBuildWantedQuest(true);
				}

				RealmUserManagerUtil::Push_DISCT_MODIFY_QUEST_EXT( kOrder, kChangeArray, pkCopyPlayer->GetMyQuest() );
				hSubRet = S_OK;

			}break;
		case IMET_BUILD_DAYLOOP:
		case IMET_BUILD_DAYLOOP_FORCE:
		case IMET_BUILD_WEEKLOOP:
			{
				SPMOD_AddPacket kData;
				kOrder.Read(kData);
				int iFlag = BLQF_ALL;
				if(false==kData.IsEmpty())
				{
					kData.m_kPacket.Pop(iFlag);
				}

				// 하루가 지났다, 등등에 퀘스트 실패 처리 때만
				ContQuestID kResetSchedule;
				bool const bForce = (IMET_BUILD_DAYLOOP_FORCE == kOrder.Cause());
				DB_ITEM_STATE_CHANGE_ARRAY kTempChangeArray;
				DocPlayerUtil::SOrderInfo kTempOrderInfo(kTempChangeArray, kOrder, kContLogMgr);
				if( pkCopyPlayer->ProcessBuildLoopQuest(kResetSchedule, bForce, &kTempOrderInfo, iFlag) )
				{
					int const iClearQuest = 1;
					int const iNotClearQuest = 0;

					//// Daily Quest
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, g_kQuestMng.DayQuestAll(), iNotClearQuest, kContLogMgr);
					//// Weekly Quest
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, g_kQuestMng.WeekQuestAll(), iNotClearQuest, kContLogMgr);
					//// Random Quest
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, g_kQuestMng.QuestRandomAll(), iClearQuest, kContLogMgr);
					//// Random Tactics Quest
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, g_kQuestMng.QuestTacticsRandomAll(), iClearQuest, kContLogMgr);
					//// Random Wanted Quest
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, g_kQuestMng.QuestWantedAll(), iClearQuest, kContLogMgr);
					//// Reset Schedule
					//DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, kResetSchedule, iNotClearQuest, kContLogMgr);

					RealmUserManagerUtil::Push_DISCT_MODIFY_QUEST_EXT(kOrder, kChangeArray, pkCopyPlayer->GetMyQuest());
					kChangeArray.insert(kChangeArray.end(), kTempChangeArray.begin(), kTempChangeArray.end());
				}

				hSubRet = S_OK;
			}break;
		case IMET_BUILD_RANDOM_QUEST:
		case IMET_BUILD_TACTICS_QUEST:
		case IMET_BUILD_WANTED_QUEST:
			{
				PgMyQuest const* pkMyQuest = pkCopyPlayer->GetMyQuest();
				if( pkMyQuest )
				{
					switch( kOrder.Cause() )
					{
					case IMET_BUILD_RANDOM_QUEST:
						{
							if( false == pkMyQuest->BuildedRandomQuest()
							||	RandomQuest::IsCanRebuild(pkMyQuest, QT_Random) )
							{
								pkCopyPlayer->StartRandomQuest();

								int const iNotClearQuest = 0;
								DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, pkMyQuest->ContRandomQuest(), iNotClearQuest, kContLogMgr);
								hSubRet = S_OK;
							}
						}break;
					case IMET_BUILD_TACTICS_QUEST:
						{
							if( false == pkMyQuest->BuildedTacticsQuest()
							||	RandomQuest::IsCanRebuild(pkMyQuest, QT_RandomTactics) )
							{
								pkCopyPlayer->StartTactcisRandomQuest();

								int const iNotClearQuest = 0;
								DocPlayerUtil::CreateQuestEndOrder(kChangeArray, kOrder, pkMyQuest->ContTacticsQuest(), iNotClearQuest, kContLogMgr);
								hSubRet = S_OK;
							}
						}break;
					case IMET_BUILD_WANTED_QUEST:
						{
							if( false == pkMyQuest->BuildedWantedQuest() )
							{
								pkCopyPlayer->StartWantedQuest();

								int const iNotClearQuest = 0;
								DocPlayerUtil::CreateQuestEndOrder( kChangeArray, kOrder, pkMyQuest->ContWantedQuest(), iNotClearQuest, kContLogMgr );
								hSubRet = S_OK;
							}
						}break;
					default:
						{
							hSubRet = E_FAIL;
						}break;
					}
					if( S_OK == hSubRet )
					{
						RealmUserManagerUtil::Push_DISCT_MODIFY_QUEST_EXT(kOrder, kChangeArray, pkMyQuest);
					}
				}
			}break;
		case IMET_SET_GUID:
			{
				SPMOD_SetGuid kData;
				kOrder.Read(kData);

				PgMySkill *pkMySkill = pkCopyPlayer->GetMySkill();

				switch( kData.Type() )
				{
				case SGT_Guild:
					{
						pkCopyPlayer->GuildGuid( kData.Guid() );

						if( pkMySkill
						&&	BM::GUID::IsNull(kData.Guid()) )
						{
							pkMySkill->EraseSkill(SDT_Guild);
						}
					}break;
				case SGT_Couple:
					{
						pkCopyPlayer->CoupleGuid( kData.Guid() );
						if(	pkMySkill
						&&	BM::GUID::IsNull(kData.Guid()) )
						{
							pkMySkill->EraseSkill(SDT_Couple);
						}
					}break;
				case SGT_CoupleColor:
					{
						//if( BM::GUID::IsNotNull(kData.Guid()) )
						{
							pkCopyPlayer->ForceSetCoupleColorGuid( kData.Guid() );

							tagDBItemStateChange kDBDaba(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
							kDBDaba.kAddonData.Push(static_cast<int>(CP_ColorGuid));
							kDBDaba.kAddonData.Push(kOrder.OwnerGuid());
							kDBDaba.kAddonData.Push(kData.Guid());		
							kChangeArray.push_back(kDBDaba);
						}
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("Can't handling event type: ") << kData.Type() );
					}break;
				}

				tagDBItemStateChange kDBDaba(DISCT_MODIFY_SET_GUID, kOrder.Cause(), kOrder.OwnerGuid());
				kDBDaba.kAddonData.Push(kData.Type());
				kDBDaba.kAddonData.Push(kData.Guid());

				kChangeArray.push_back(kDBDaba);
				hSubRet = S_OK;
			}break;
		//case IMET_SET_ENDQUEST:
		//	{
		//		//kOrder.Read(kData);
		//		//kChangeArray.push_back(tagDBItemStateChange(DISCT_MODIFY_SKILL, kOrder.Cause(), kOrder.OwnerGuid(), 0, kData.SkillNo()));
		//		hSubRet = S_OK;
		//	}break;
		case IMET_SET_DATE_CONTENTS:
			{
				SPMOD_DateContents kData;

				kOrder.Read( kData );

				pkCopyPlayer->SetDateContents(kData.Type(), kData.DateTime());

				tagDBItemStateChange kDBData(DISCT_MODIFY_SET_DATE_CONTENTS, kOrder.Cause(), kOrder.OwnerGuid());
				kDBData.kAddonData.Push(kData.Type());
				kDBData.kAddonData.Push(kData.DateTime());

				kChangeArray.push_back(kDBData);
				hSubRet = S_OK;
			}break;
		case IMET_ADD_WORLD_MAP://플레이어 고치고 들어옴.
			{
				tagPlayerModifyOrderData_AddWorldMap kData;
				kOrder.Read(kData);
				
				tagDBItemStateChange kDBData(DISCT_MODIFY_WORLD_MAP, kOrder.Cause(), kOrder.OwnerGuid());
				kDBData.kAddonData.Push(kData.MapNo());

				kChangeArray.push_back(kDBData);
				hSubRet = S_OK;

				pkCopyPlayer->UpdateWorldMap(kData.MapNo());
			}break;
		case IMET_ADD_RANK_POINT:
			{
				SPMOD_AddRankPoint kData;
				kOrder.Read(kData);

				
				if ( kData.Value() > 0 )
				{
					bool bInsert = true;

					if ( E_RANKPOINT_MAXCOMBO == kData.Type() )
					{
						int const iAddValue = kData.Value() - pkCopyPlayer->GetAbil( AT_MAX_COMBO ) ;
						if ( 0 < iAddValue )
						{
							pkCopyPlayer->SetAbil( AT_MAX_COMBO, kData.Value() );
							kData.Value( iAddValue );
						}
						else
						{
							kData.Value( 0 );
							bInsert = false;
						}
					}

					if ( true == bInsert )
					{
						tagDBItemStateChange kDBData( DISCT_MODIFY_ADD_RANK_POINT, kOrder.Cause(), kOrder.OwnerGuid());
						kDBData.kAddonData.Push(kData.Type());
						kDBData.kAddonData.Push(kData.Value());
						kChangeArray.push_back(kDBData);
						hSubRet = S_OK;	
					}
				}						
			}break;
		case IMET_ADD_PVP_RANK_RECORD:
			{
				SPMOD_SAbil kData;
				kOrder.Read(kData);
				
				SDoc_PvPReport kPvpReport(*pkCopyPlayer);

				hSubRet = S_OK;
				SPMOD_SAbil kReqData( kData.Type() );
				switch(kData.Type())
				{
				case GAME_DRAW:
				case GAME_WIN:
				case GAME_LOSE:
				case GAME_DRAW_EXERCISE:
				case GAME_WIN_EXERCISE:
				case GAME_LOSE_EXERCISE:
					{	
						kPvpReport.m_kPvpReport.m_iRecords[kData.Type()] += static_cast<int>(kData.Value());
						kReqData.Value( static_cast<__int64>(kPvpReport.m_kPvpReport.m_iRecords[kData.Type()]) );
					}break;
				case GAME_POINT:
					{//Point 처리.
						kPvpReport.m_kPvpReport.m_iPoint += static_cast<int>(kData.Value());
						kReqData.Value( static_cast<__int64>(kPvpReport.m_kPvpReport.m_iPoint) );
					}break;
				case GAME_KILL:
					{
						kPvpReport.m_kPvpReport.m_iKill += kData.Value();
						kReqData.Value( kPvpReport.m_kPvpReport.m_iKill );
					}break;
				case GAME_DEATH:
					{
						kPvpReport.m_kPvpReport.m_iDeath += kData.Value();
						kReqData.Value( kPvpReport.m_kPvpReport.m_iDeath );
					}break;
				default:
					{
						hSubRet = E_FAIL;
					}break;
				}

				if ( S_OK == hSubRet )
				{
					tagDBItemStateChange kDBData(DISCT_MODIFY_SET_PVP_RECORD, kOrder.Cause(), kOrder.OwnerGuid());
					kReqData.WriteToPacket( kDBData.kAddonData );

					kChangeArray.push_back(kDBData);
				}
			}break;
		case IMET_PET:
			{
				hSubRet = E_FAIL;

				switch ( kCause )
				{
				case CIE_HatchPet:
					{
						// 펫 부화
						SPMOD_HatchPet kData;
						kOrder.Read( kData );

	//					if ( 2 > pkCopyPlayer->GetInven()->GetPetItemCount() )
						{
							// 1 실제로 아이템을 깔 수 있는지 검사한다.
							hSubRet = pkCopyPlayer->ItemProcess( SPMO( IMET_MODIFY_COUNT, kOrder.OwnerGuid(), kData.kSPMOD_MC ), kChangeArray, kContLogMgr );
							if ( S_OK == hSubRet )
							{
								SPMOD_Insert_Fixed kAddItem( kData.kPetItem, SItemPos(), true );
								hSubRet = pkCopyPlayer->ItemProcess( SPMO( IMET_INSERT_FIXED, pkCopyPlayer->GetID(), kAddItem ), kChangeArray, kContLogMgr );				
							}
						}
					}break;//CIE_HatchPet
				case CIE_RenamePet:
					{
						SPMOD_RenamePet kData;
						kOrder.Read( kData );

						PgInventory* pkInv = pkCopyPlayer->GetInven();

						PgBase_Item kPetItem;
						hSubRet = pkInv->GetItem( kData.kPetItemPos, kPetItem );
						if ( S_OK == hSubRet )
						{
							PgItem_PetInfo *pkPetInfo = NULL;
							if ( true == kPetItem.GetExtInfo( pkPetInfo ) )
							{
								pkPetInfo->Name( kData.wstrPetName );

								DB_ITEM_STATE_CHANGE kDB( DISCT_RENAME_PET, IMET_PET, pkCopyPlayer->GetID() );
								kDB.ItemWrapper( PgItemWrapper( kPetItem, kData.kPetItemPos ) );

								hSubRet = pkInv->SwapItem( kData.kPetItemPos, kPetItem, true );

								if ( S_OK == hSubRet )
								{
									kChangeArray.push_back( kDB );
								}
							}
							else
							{
								hSubRet = E_FAIL;
							}
						}
					}break;//CIE_RenamePet
				case CIE_Equip:
				case CIE_UnEquip:
					{
						hSubRet = ProcessUnEquipPet( pkCopyPlayer, kOrder.m_kPacket, kChangeArray, kContLogMgr );
					}break;
				case MCE_EXP:
				case CIE_SetAbilPet:
				default:
					{
						SPMOD_AddAbilPet kData;
						kOrder.Read( kData );
						hSubRet = ProcessModifyPetAbil( pkCopyPlayer, kData, kChangeArray, kContLogMgr );
					}break;//CIE_SetAbilPet
				}
			}break;//IMET_PET
		case IMET_PET_ITEM:
			{
				SPMOD_Modify_Pos_Pet kData;
				kOrder.Read( kData );

				PgInventory * pkTargetInven = pkCopyPlayer->GetPetInven( kData.PetID() );
				if ( pkTargetInven )
				{
					PgInventory * pkCasterInven = ( kData.CasterIsPlayer() ? pkCopyPlayer->GetInven() : pkTargetInven );
					if ( pkCasterInven )
					{
						__int64 iCause = kOrder.Cause();

						SItemPos const &rkCasterPos = kData.OrgPos();
						SItemPos const &rkTargetPos = kData.TgtPos();

						PgItemWrapper kCasterItem;
						PgItemWrapper kTargetItem;
						if (	S_OK == pkCasterInven->PopItem( iCause, rkCasterPos, kCasterItem, false, kChangeArray, kContLogMgr ) 
							&&	S_OK == pkTargetInven->PopItem( iCause, rkTargetPos, kTargetItem, false, kChangeArray, kContLogMgr )
							)
						{
							if (	!kCasterItem.IsEmpty() 
								||	!kTargetItem.IsEmpty()
								)
							{
								if (	S_OK == pkCasterInven->PopItem( iCause, rkCasterPos, kCasterItem, true, kChangeArray, kContLogMgr ) 
									&&	S_OK == pkTargetInven->PopItem( iCause, rkTargetPos, kTargetItem, true, kChangeArray, kContLogMgr )
									)
								{
									// 실제 ItemGuid와 MapServer가 요청한 ItemGuid를 한번 더 검사한다.
									//	이유 : 검사하지 않을 경우, 장착할 수 없는 아이템을 장착할 수 있는 버그 발생 (ContentsServer가 느릴 경우)
									BM::GUID const& rkOrgGuid = kData.OrgItemGuid();
									BM::GUID const& rkTgtGuid = kData.TgtItemGuid();
									if (rkOrgGuid.IsNotNull() && rkOrgGuid != kCasterItem.Guid()) { return E_FAIL; }
									if (rkTgtGuid.IsNotNull() && rkTgtGuid != kTargetItem.Guid()) { return E_FAIL; }

									if( S_OK != PgInventory::ItemContainer::MergeItem(kCasterItem, kTargetItem, iCause) )
									{//머징이 안되면 //위치 교환
										kCasterItem.Pos(rkTargetPos);
										kTargetItem.Pos(rkCasterPos);
									}

									if(		S_OK == pkTargetInven->OverWriteItem(IMET_PET_ITEM, kCasterItem, kChangeArray, kContLogMgr)
										&&	S_OK == pkCasterInven->OverWriteItem(IMET_PET_ITEM, kTargetItem, kChangeArray, kContLogMgr)
										)
									{
										hSubRet = S_OK;

										kContTarget.insert( std::make_pair( SModifyOrderOwner(kData.PetID(), OOT_Player), pkCopyPlayer ) );
									}
								}
							}
						}
					}
				}
			}break;//IMET_PET_ITEM
		case IMET_PET_DUMP:
			{
				BM::GUID kPetID;
				if ( true == kOrder.m_kPacket.Pop( kPetID ) )
				{
					pkCopyPlayer->RemovePetInfo( kPetID );

					DB_ITEM_STATE_CHANGE kDBData( DISCT_REMOVE_PET, IMET_PET_DUMP, pkCopyPlayer->GetID() );
					kDBData.kAddonData.Push( kPetID );
					kChangeArray.push_back( kDBData );

					hSubRet = S_OK;
				}
			}break;//IMET_PET_DUMP
		case IMET_EMPORIA_FUNCTION:
			{
				tagDBItemStateChange kDBData(DISCT_EMPORIAFUNC_UPDATE, kOrder.Cause(), kOrder.OwnerGuid());
				kDBData.kAddonData.Push( kOrder.m_kPacket);

				kChangeArray.push_back(kDBData);

				hSubRet = S_OK;
			}break;
		case IMET_END_MISSION_EVENT:
			{
				SPMOD_AddMissionEvent kData;
				kOrder.Read(kData);
				
				int const iMissionValue = kData.Value();
				int iValue = 0;
				int iAbilType = AT_MISSION_EVENT;

				switch( kData.Type() )
				{
				case E_MISSION_EVENT_SET:
					{
						if( 0 < iMissionValue )
						{
							iValue = pkCopyPlayer->GetAbil( AT_MISSION_EVENT );
							iValue |= (iMissionValue);
							hSubRet = S_OK;
						}
					}break;
				case E_MISSION_EVENT_RESET:
					{
						iValue = iMissionValue;
						hSubRet = S_OK;
					}break;
				case E_MISSION_ABIL_AWAKE_STATE:
					{
						if( 0 <= iMissionValue )
						{
							iAbilType = AT_AWAKE_STATE;

							iValue = iMissionValue;
							hSubRet = S_OK;
						}
					}break;
				case E_MISSION_ABIL_AWAKE_VALUE:
					{
						if( 0 <= iMissionValue )
						{
							iAbilType = AT_AWAKE_VALUE;

							iValue = iMissionValue;
							hSubRet = S_OK;
						}
					}break;
				default:
					{
						hSubRet = S_FALSE;
					}break;
				}

				if( S_OK == hSubRet )
				{
					pkCopyPlayer->SetAbil( iAbilType, iValue );

					tagDBItemStateChange kDBData( DISCT_MODIFY_MISSION_EVENT, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push(iValue);
					kChangeArray.push_back(kDBData);					
				}
			}break;
		case IMET_SET_SWEETHEART:
			{
				SPMOD_SetSweetHeart kData;
				kOrder.Read(kData);
				
				switch( kData.Type() )
				{
				case CC_Ans_SweetHeart_Complete:
					{
						tagDBItemStateChange kSweetHeartDBData(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
						kSweetHeartDBData.kAddonData.Push(static_cast<int>(CP_Status));
						kSweetHeartDBData.kAddonData.Push(kData.Guid());
						kSweetHeartDBData.kAddonData.Push(static_cast<int>( (CoupleS_Normal | CoupleS_SweetHeart) ));
						kChangeArray.push_back(kSweetHeartDBData);
						hSubRet = S_OK;
					}break;
				case CC_SweetHeartQuest_Cancel:
					{
						int iQuestID = kData.Value();

						if( 0 == iQuestID )
						{
							// 커플이 깨졌으니 다 지워버려라.
							pkCopyPlayer->RemoveCoupleQuest(kChangeArray, kOrder);
							{
								tagDBItemStateChange kSweetHeartDBData(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
								kSweetHeartDBData.kAddonData.Push(static_cast<int>(CP_CouplePanaltyLimitDate));
								kSweetHeartDBData.kAddonData.Push(kData.Guid());
								kSweetHeartDBData.kAddonData.Push( kData.Date() ); // Null 일경우가 있다.
								kChangeArray.push_back(kSweetHeartDBData);
							}
						}

						ContQuestID kVec;
						int iSelectedID = 0;
						if( pkCopyPlayer->DelIngQuest(iQuestID, false, kVec, iSelectedID) )
						{
							tagDBItemStateChange kDBData(DISCT_MODIFY_QUEST_ING, kOrder.Cause(), kOrder.OwnerGuid());
							kDBData.kAddonData.Push(iQuestID);
							kDBData.kAddonData.Push(QS_None);
							kChangeArray.push_back(kDBData);
							iQuestID = 0; //초기화(지울 퀘스트 번호 저장은 이곳을 안온다. 로긴해서 초기화 되면 여기를 온다.)
						}

						{
							tagDBItemStateChange kSweetHeartDBData(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
							kSweetHeartDBData.kAddonData.Push(static_cast<int>(CP_DeleteQuest));
							kSweetHeartDBData.kAddonData.Push(kData.Guid());
							if( pkCopyPlayer->GetID() != kData.Guid() )
							{
								kSweetHeartDBData.kAddonData.Push(iQuestID);
							}
							else
							{
								kSweetHeartDBData.kAddonData.Push(0);
							}
							kChangeArray.push_back(kSweetHeartDBData);
						}

						{
							tagDBItemStateChange kSweetHeartDBData(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
							kSweetHeartDBData.kAddonData.Push(static_cast<int>(CP_SweetHeartQuestTime));
							kSweetHeartDBData.kAddonData.Push(kData.Guid());
							kSweetHeartDBData.kAddonData.Push(0);								
							kChangeArray.push_back(kSweetHeartDBData);
						}

						hSubRet = S_OK;
						// 퀘스트 정보 갱신
						// DB퀘스트 정보 갱신
					}break;	
				case CC_CoupleSkill:
					{
						int iSkillNo = kData.Value();
						if( 0 <= iSkillNo )
						{
							tagDBItemStateChange kSweetHeartDBData(DISCT_MODIFY_SWEETHEART, kOrder.Cause(), kOrder.OwnerGuid());
							kSweetHeartDBData.kAddonData.Push(static_cast<int>(CP_LearnSkill));
							kSweetHeartDBData.kAddonData.Push(kData.Guid());
							kSweetHeartDBData.kAddonData.Push(iSkillNo);
							kChangeArray.push_back(kSweetHeartDBData);
						}
						hSubRet = S_OK;
					}break;
				default:
					{
						hSubRet = S_FALSE;
					}break;
				}
			}break;
		case IMET_ACTIVE_STATUS:
			{
				hSubRet = S_OK;
				SPMO_ActiveStatus kData;
				kOrder.Read(kData);
				int const iOldBonusStatus = pkCopyPlayer->GetAbil(AT_STATUS_BONUS);
				int iBonusStatus = iOldBonusStatus;

				PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_Change_Info, pkCopyPlayer->GetMemberGUID(), pkCopyPlayer->GetID(), pkCopyPlayer->MemberID(),
					pkCopyPlayer->Name(), pkCopyPlayer->GetAbil(AT_CLASS), pkCopyPlayer->GetAbil(AT_LEVEL), 0);
				SClassKey kClassKey(pkCopyPlayer->GetAbil(AT_CLASS), pkCopyPlayer->GetAbil(AT_LEVEL));
				GET_DEF(PgClassDefMgr, kClassDef);
				if (hSubRet == S_OK && kData.Str() != 0)
				{
					int const iCurrent = pkCopyPlayer->GetAbil(AT_STR_ADD);
					if (kData.Str() == -1)
					{
						pkCopyPlayer->SetAbil(AT_STR_ADD, 0);
						iBonusStatus += iCurrent;

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_STR_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_STR_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Str, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(0));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(-iCurrent));
						kLogCont.Add(kLog);
					}
					else if (kData.Str() > 0 && iCurrent + kData.Str() <= kClassDef.GetAbil(kClassKey, AT_STR_ADD_LIMIT))
					{
						pkCopyPlayer->SetAbil(AT_STR_ADD, iCurrent+kData.Str());
						iBonusStatus -= kData.Str();

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_STR_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_STR_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Str, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(iCurrent+kData.Str()));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(kData.Str()));
						kLogCont.Add(kLog);
					}
					else
					{
						hSubRet = E_LIMITED_STR;
					}
				}
				if (hSubRet == S_OK && kData.Int() != 0)
				{
					int const iCurrent = pkCopyPlayer->GetAbil(AT_INT_ADD);
					if (kData.Int() == -1)
					{
						pkCopyPlayer->SetAbil(AT_INT_ADD, 0);
						iBonusStatus += iCurrent;
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_INT_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_INT_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Int, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(0));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(-iCurrent));
						kLogCont.Add(kLog);
					}
					else if (kData.Int() > 0 &&iCurrent + kData.Int() <= kClassDef.GetAbil(kClassKey, AT_INT_ADD_LIMIT))
					{
						pkCopyPlayer->SetAbil(AT_INT_ADD, iCurrent+kData.Int());
						iBonusStatus -= kData.Int();

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_INT_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_INT_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Int, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(iCurrent+kData.Int()));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(kData.Int()));
						kLogCont.Add(kLog);
					}
					else
					{
						hSubRet = E_LIMITED_INT;
					}
				}
				if (hSubRet == S_OK && kData.Con() != 0)
				{
					int const iCurrent = pkCopyPlayer->GetAbil(AT_CON_ADD);
					if (kData.Con() == -1)
					{
						pkCopyPlayer->SetAbil(AT_CON_ADD, 0);
						iBonusStatus += iCurrent;
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_CON_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_CON_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Con, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(0));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(1, static_cast<__int64>(-iCurrent));
						kLogCont.Add(kLog);
					}
					else if (kData.Con() > 0 &&iCurrent + kData.Con() <= kClassDef.GetAbil(kClassKey, AT_CON_ADD_LIMIT))
					{
						pkCopyPlayer->SetAbil(AT_CON_ADD, iCurrent+kData.Con());
						iBonusStatus -= kData.Con();

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_CON_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_CON_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Con, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(iCurrent+kData.Con()));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(kData.Con()));
						kLogCont.Add(kLog);
					}
					else
					{
						hSubRet = E_LIMITED_CON;
					}
				}
				if (hSubRet == S_OK && kData.Dex() != 0)
				{
					int const iCurrent = pkCopyPlayer->GetAbil(AT_DEX_ADD);
					if (kData.Dex() == -1)
					{
						pkCopyPlayer->SetAbil(AT_DEX_ADD, 0);
						iBonusStatus += iCurrent;
						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_DEX_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_DEX_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Dex, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(0));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(1, static_cast<__int64>(-iCurrent));
						kLogCont.Add(kLog);
					}
					else if (kData.Dex() > 0 &&iCurrent + kData.Dex() <= kClassDef.GetAbil(kClassKey, AT_DEX_ADD_LIMIT))
					{
						pkCopyPlayer->SetAbil(AT_DEX_ADD, iCurrent+kData.Dex());
						iBonusStatus -= kData.Dex();

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_DEX_ADD);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_DEX_ADD));
						kChangeArray.push_back(kDBData);

						PgLog kLog(ELOrderMain_AS_Dex, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(iCurrent+kData.Dex()));
						kLog.Set(1, static_cast<__int64>(iCurrent));
						kLog.Set(2, static_cast<__int64>(kData.Dex()));
						kLogCont.Add(kLog);
					}
					else
					{
						hSubRet = E_LIMITED_DEX;
					}
				}

				if (hSubRet == S_OK)
				{
					if (iBonusStatus < 0)
					{
						hSubRet = E_NOT_ENOUGH;
					}
					else
					{
						pkCopyPlayer->SetAbil(AT_STATUS_BONUS, iBonusStatus);

						tagDBItemStateChange kDBData(DISCT_MODIFY_VALUE, IMET_SET_ABIL, pkCopyPlayer->GetID());
						kDBData.kAddonData.Push(AT_STATUS_BONUS);
						kDBData.kAddonData.Push((__int64)pkCopyPlayer->GetAbil(AT_STATUS_BONUS));
						kChangeArray.push_back(kDBData);
						
						PgLog kLog(ELOrderMain_AS_BonusStatus, ELOrderSub_Modify);
						kLog.Set(0, static_cast<__int64>(iBonusStatus));
						kLog.Set(1, static_cast<__int64>(iOldBonusStatus));
						kLog.Set(2, static_cast<__int64>(iBonusStatus-iOldBonusStatus));
						kLogCont.Add(kLog);

						PgLogCont* const pkLogCont = kContLogMgr.GetLogCont(pkCopyPlayer->GetID());
						if (pkLogCont == NULL)
						{
							kContLogMgr.AddContLog(kLogCont);
						}
						else
						{
							*pkLogCont = kLogCont;
						}
					}
				}
			}break;
		case IMET_TRANSTOWER_SAVE_RECENT:
			{			
				SRecentInfo kRecentInfo;
				kOrder.Read(kRecentInfo);

				switch ( kCause )
				{
				case CIE_TransTower_Save_Recent:
					{
						pkCopyPlayer->TransTowerSaveRct( kRecentInfo );

						DB_ITEM_STATE_CHANGE kDB( DISCT_TRANSTOWER_SAVE_RECENT, IMET_TRANSTOWER_SAVE_RECENT, pkCopyPlayer->GetID() );
						kRecentInfo.WriteToPacket( kDB.kAddonData );

						kChangeArray.push_back( kDB );

						hSubRet = S_OK;
					}break;
				case CIE_TransTower_Open_Map:
					{
						if ( true == pkCopyPlayer->UpdateWorldMap( kRecentInfo.iMapNo ) )
						{
							tagDBItemStateChange kDBData(DISCT_MODIFY_WORLD_MAP, IMET_TRANSTOWER_SAVE_RECENT, kOrder.OwnerGuid());
							kDBData.kAddonData.Push(kRecentInfo.iMapNo);

							kChangeArray.push_back(kDBData);
							hSubRet = S_OK;
						}
					}break;
				}
			}break;//IMET_TRANSTOWER_SAVE_RECENT
		case IMET_ADD_ABIL64_MAPUSE:
			{
				SPMOD_AddAbil kData;
				kOrder.Read(kData);

				if ( kData.Value() != 0i64 )
				{
					// Abil값을 맵서버로 전송하여, 맵서버에서 값을 변경하도록 한다.
					tagDBItemStateChange kDBData(DISCT_ADD_ABIL, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.IsQuery(false);
					kDBData.kAddonData.Push(kData.AbilType());
					kDBData.kAddonData.Push(kData.Value());
					kChangeArray.push_back(kDBData);
				}
				else
				{
					INFO_LOG( BM::LOG_LV0, __FL__ << _T("AddAbil64[") << kData.AbilType() << _T("]'s Value is 0 / OrderGuid[") << kOrder.OwnerGuid().str().c_str() << _T("]") );
				}
				hSubRet = S_OK;
			}break;
		case IMET_JOBSKILL_SET_EXPERTNESS:
			{
				SPMOD_JobSkillExpertness kData;
				kOrder.Read(kData);

				if( 0 < kData.SkillNo()
				&&	JobSkillExpertnessUtil::Set(pkCopyPlayer->JobSkillExpertness(), kData.SkillNo(), kData.Expertness()) )
				{
					tagDBItemStateChange kDBData(DISCT_JOBSKILL_SET_SKILL_EXPERTNESS, kOrder.Cause(), kOrder.OwnerGuid());
					kData.Expertness( pkCopyPlayer->JobSkillExpertness().Get(kData.SkillNo()) );
					kData.WriteToPacket(kDBData.kAddonData);

					kChangeArray.push_back(kDBData);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_ADD_EXPERTNESS:
			{
				SPMOD_JobSkillExpertness kData;
				kOrder.Read(kData);

				if( 0 < kData.SkillNo()
				&&	0 != kData.Expertness()
				&&	JobSkillExpertnessUtil::Inc(pkCopyPlayer->GetAbil(AT_LEVEL), pkCopyPlayer->JobSkillExpertness(), kData.SkillNo(), kData.Expertness()) )
				{
					tagDBItemStateChange kDBData(DISCT_JOBSKILL_SET_SKILL_EXPERTNESS, kOrder.Cause(), kOrder.OwnerGuid());
					kData.Expertness( pkCopyPlayer->JobSkillExpertness().Get(kData.SkillNo()) );
					kData.WriteToPacket(kDBData.kAddonData);

					kChangeArray.push_back(kDBData);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_DEL_EXPERTNESS:
			{
				SPMOD_JobSkillExpertness kData;
				kOrder.Read(kData);

				if( 0 < kData.SkillNo()
				&&	JobSkillExpertnessUtil::Del(pkCopyPlayer->JobSkillExpertness(), kData.SkillNo()) )
				{
					tagDBItemStateChange kDBData(DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS, kOrder.Cause(), kOrder.OwnerGuid());
					kDBData.kAddonData.Push( kData.SkillNo() );

					kChangeArray.push_back(kDBData);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_ADD_EXHAUSTION:
			{
				SPMOD_JobSkillExhaustion kData;
				kOrder.Read(kData);

				if( 0 != kData.Exhaustion()
				&&	JobSkillExpertnessUtil::AddExhaustion(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), kData.Exhaustion()) )
				{
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_ADD_BLESSPOINT:
			{
				SPMOD_JobSkillExhaustion kData;
				kOrder.Read(kData);

				if( 0 != kData.Exhaustion()
				&&	JobSkillExpertnessUtil::AddBlessPoint(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), kData.Exhaustion()) )
				{
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);

					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_RESET_EXHAUSTION:
			{
				SPMOD_JobSkillExhaustion kData;
				kOrder.Read(kData);

				if( JobSkillExpertnessUtil::ResetExhaustion(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), kData.Exhaustion()) )
				{
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_INIT_EXHAUSTION:
			{
				if( JobSkillExpertnessUtil::InitExhaustion(pkCopyPlayer->JobSkillExpertness()) )
				{
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);
					hSubRet = S_OK;
				}
			}break;
		case IMET_JOBSKILL_INIT_BLESSPOINT:
			{
				if( JobSkillExpertnessUtil::InitBlessPoint(pkCopyPlayer->JobSkillExpertness()) )
				{
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);
					hSubRet = S_OK;
				}
			}break;
		case IMET_DAILY_REWARD_UPDATE:
			{
				tagDBItemStateChange kDBData(DISCT_DAILY_REWARD_UPDATE, kOrder.Cause(), pkCopyPlayer->GetMemberGUID());
				SPMOD_DailyReward kData;
				kOrder.Read(kData);
				kData.WriteToPacket(kDBData.kAddonData);
				kChangeArray.push_back(kDBData);
				hSubRet = S_OK;
			}break;
		case IMET_PREMIUM_SERVICE_INSERT:
			{
				SPMOD_PremiumService kData;
				kOrder.Read(kData);

				GET_DEF(PgDefPremiumMgr, kDefPremium);
				SPremiumData const* pkDefPremium = kDefPremium.GetDef(kData.ServiceNo());

				if( pkDefPremium
				 && false==pkCopyPlayer->GetPremium().IsUserService() )
				{
					BM::Stream kNullData;
					if( 0==pkCopyPlayer->GetPremium().SetupService(kData.ServiceNo(), pkCopyPlayer, kNullData) )
					{
						BM::Stream kCustomData;
						pkCopyPlayer->GetPremium().ToDB(kCustomData);

						//디비업데이트
						tagDBItemStateChange kDBData(DISCT_PREMIUM_SERVICE_INSERT, kOrder.Cause(), pkCopyPlayer->GetMemberGUID());
						kDBData.CharacterGuid(kOrder.OwnerGuid());
						kDBData.kAddonData.Push( kData.ServiceNo() );
						kDBData.kAddonData.Push( pkCopyPlayer->GetPremium().StartDate() );
						kDBData.kAddonData.Push( pkCopyPlayer->GetPremium().EndDate() );
						kDBData.kAddonData.Push( kCustomData.Data() );
						kChangeArray.push_back(kDBData);

						hSubRet = S_OK;

						//Log
						PgLogCont kLogCont(ELogMain_Premium, ELogSub_Service);
						kLogCont.MemberKey(pkCopyPlayer->GetMemberGUID());
						kLogCont.CharacterKey(kOrder.OwnerGuid());
						kLogCont.ID( pkCopyPlayer->MemberID() );
						kLogCont.Name( pkCopyPlayer->Name() );
						kLogCont.Class( pkCopyPlayer->GetAbil( AT_CLASS ) );
						kLogCont.Level( pkCopyPlayer->GetAbil( AT_LEVEL ) );
						kLogCont.ChannelNo( pkCopyPlayer->GetChannel() );
						kLogCont.GroundNo( pkCopyPlayer->GetAbil( AT_RECENT ) );

						PgLog kLog(ELOrderMain_Service, ELOrderSub_Start);
						kLog.Set(0, kData.ServiceNo());
						kLog.Set(1, pkDefPremium->wUseDate);
						kLog.Set(0, static_cast<std::wstring>(pkCopyPlayer->GetPremium().StartDate()) );
						kLog.Set(1, static_cast<std::wstring>(pkCopyPlayer->GetPremium().EndDate()) );
						kLogCont.Add(kLog);
						kLogCont.Commit();
					}
				}

				if(S_OK==hSubRet)
				{
					int iMaxExhaustion = 0;
					int iMaxBlessPoint = 0;
					if( S_PST_JS1_MaxExhaustion const* pkPremium = pkCopyPlayer->GetPremium().GetType<S_PST_JS1_MaxExhaustion>() )
					{
						iMaxExhaustion = pkPremium->iMax;
					}
					if( S_PST_JS2_MaxBless const* pkPremium = pkCopyPlayer->GetPremium().GetType<S_PST_JS2_MaxBless>() )
					{
						iMaxBlessPoint = pkPremium->iMax;
					}

					JobSkillExpertnessUtil::AddExhaustion(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), iMaxExhaustion);
					JobSkillExpertnessUtil::AddBlessPoint(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), iMaxBlessPoint);
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);
				}
			}break;
		case IMET_PREMIUM_SERVICE_MODIFY:
			{
				SPMOD_PremiumServiceModify kData;
				kOrder.Read(kData);

				GET_DEF(PgDefPremiumMgr, kDefPremium);
				SPremiumData const* pkDefPremium = kDefPremium.GetDef(kData.ServiceNo());
				if( pkDefPremium
				 && pkCopyPlayer->GetPremium().IsUserService()
				 && (pkCopyPlayer->GetPremium().GetServiceNo() == kData.ServiceNo()))
				{
					//시간 수정
					pkCopyPlayer->GetPremium().AddEndDate( kData.UseDate() );

					//디비업데이트
					tagDBItemStateChange kDBData(DISCT_PREMIUM_SERVICE_MODIFY, kOrder.Cause(), pkCopyPlayer->GetMemberGUID());
					kDBData.CharacterGuid(kOrder.OwnerGuid());
					kDBData.kAddonData.Push( kData.ServiceNo() );
					kDBData.kAddonData.Push( pkCopyPlayer->GetPremium().StartDate() );
					kDBData.kAddonData.Push( pkCopyPlayer->GetPremium().EndDate() );
					kChangeArray.push_back(kDBData);
					
					hSubRet = S_OK;

					//Log
					PgLogCont kLogCont(ELogMain_Premium, ELogSub_Service);
					kLogCont.MemberKey(pkCopyPlayer->GetMemberGUID());
					kLogCont.CharacterKey(kOrder.OwnerGuid());
					kLogCont.ID( pkCopyPlayer->MemberID() );
					kLogCont.Name( pkCopyPlayer->Name() );
					kLogCont.Class( pkCopyPlayer->GetAbil( AT_CLASS ) );
					kLogCont.Level( pkCopyPlayer->GetAbil( AT_LEVEL ) );
					kLogCont.ChannelNo( pkCopyPlayer->GetChannel() );
					kLogCont.GroundNo( pkCopyPlayer->GetAbil( AT_RECENT ) );

					PgLog kLog(ELOrderMain_Service, ELOrderSub_Modify);
					kLog.Set(0, kData.ServiceNo());
					kLog.Set(1, kData.UseDate());
					kLog.Set(0, static_cast<std::wstring>(pkCopyPlayer->GetPremium().StartDate()) );
					kLog.Set(1, static_cast<std::wstring>(pkCopyPlayer->GetPremium().EndDate()) );
					kLogCont.Add(kLog);
					kLogCont.Commit();
				}
			}break;
		case IMET_PREMIUM_SERVICE_REMOVE:
			{
				if( int const iServiceNo = pkCopyPlayer->GetPremium().GetServiceNo() )
				{
					BM::DBTIMESTAMP_EX const kStartDate = pkCopyPlayer->GetPremium().StartDate();
					BM::DBTIMESTAMP_EX const kEndDate = pkCopyPlayer->GetPremium().EndDate();
					int iMaxExhaustion = 0;
					int iMaxBlessPoint = 0;
					if( S_PST_JS1_MaxExhaustion const* pkPremium = pkCopyPlayer->GetPremium().GetType<S_PST_JS1_MaxExhaustion>() )
					{
						iMaxExhaustion = pkPremium->iMax;
					}
					if( S_PST_JS2_MaxBless const* pkPremium = pkCopyPlayer->GetPremium().GetType<S_PST_JS2_MaxBless>() )
					{
						iMaxBlessPoint = pkPremium->iMax;
					}
					pkCopyPlayer->GetPremium().Clear();

					//디비업데이트
					tagDBItemStateChange kDBData(static_cast<EDBItemStateChangeType>(DISCT_PREMIUM_SERVICE_REMOVE), kOrder.Cause(), pkCopyPlayer->GetMemberGUID());
					kDBData.CharacterGuid(kOrder.OwnerGuid());
					kChangeArray.push_back(kDBData);
					
					JobSkillExpertnessUtil::AddExhaustion(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), -iMaxExhaustion);
					JobSkillExpertnessUtil::AddBlessPoint(pkCopyPlayer->GetPremium(), pkCopyPlayer->JobSkillExpertness(), -iMaxBlessPoint);
					RealmUserManagerUtil::Push_DISCT_JOBSKILL_SAVE_EXHAUSTION(kOrder, kChangeArray, pkCopyPlayer);

					hSubRet = S_OK;

					//Log
					PgLogCont kLogCont(ELogMain_Premium, ELogSub_Service);
					kLogCont.MemberKey(pkCopyPlayer->GetMemberGUID());
					kLogCont.CharacterKey(kOrder.OwnerGuid());
					kLogCont.ID( pkCopyPlayer->MemberID() );
					kLogCont.Name( pkCopyPlayer->Name() );
					kLogCont.Class( pkCopyPlayer->GetAbil( AT_CLASS ) );
					kLogCont.Level( pkCopyPlayer->GetAbil( AT_LEVEL ) );
					kLogCont.ChannelNo( pkCopyPlayer->GetChannel() );
					kLogCont.GroundNo( pkCopyPlayer->GetAbil( AT_RECENT ) );

					PgLog kLog(ELOrderMain_Service, ELOrderSub_End);
					kLog.Set(0, iServiceNo);
					kLog.Set(0, static_cast<std::wstring>(kStartDate) );
					kLog.Set(1, static_cast<std::wstring>(kEndDate) );
					kLogCont.Add(kLog);
					kLogCont.Commit();
				}
			}break;
		case IMET_PREMIUM_ARTICLE_MODIFY:
			{
				SPMOD_PremiumArticle kData;
				kOrder.Read(kData);

				switch(kData.ArticleType())
				{
				case PST_QUEST_ONCE_MORE:
					{
						if(S_PST_QuestOnceMore * pkPremium = pkCopyPlayer->GetPremium().SetType<S_PST_QuestOnceMore>())
						{
							EQuestType eType = QT_None;
							kData.m_kPacket.Pop(eType);
							if(pkPremium->IsCheck(eType))
							{
								pkPremium->AddQuestType(eType);

								BM::PgPackedTime kNextTime;
								if(pkPremium->GetNextTime(eType, kNextTime))
								{
									hSubRet = S_OK;

									kData.m_kPacket.PosAdjust();
									tagDBItemStateChange kDBData(DISCT_PREMIUM_ARTICLE_MODIFY, kOrder.Cause(), pkCopyPlayer->GetID());
									kDBData.kAddonData.Push(kData.ArticleType());
									kDBData.kAddonData.Push(eType);
									kDBData.kAddonData.Push(kNextTime);
									kChangeArray.push_back(kDBData);
								}
							}
						}
					}break;
				}
			}break;
		default:
			{
				if ( IMC_MODIFY_PET_INV & kOrder.Cause() )
				{
					BM::GUID kPetID;
					kOrder.m_kPacket.Pop( kPetID );
					PgDoc_PetInfo *pkDocPetInfo = pkCopyPlayer->LoadDocPetInfo( kPetID, false );
					if ( pkDocPetInfo )
					{
						hSubRet = pkDocPetInfo->ItemProcess( kOrder, kChangeArray, kContLogMgr );
						kContTarget.insert( std::make_pair( SModifyOrderOwner(kPetID, OOT_Player), pkCopyPlayer ) );
					}
				}
				else
				{
					hSubRet = pkCopyPlayer->ItemProcess( kOrder, kChangeArray, kContLogMgr );
				}
			}break;
		}

		if(S_OK != hSubRet)
		{
			hRet = hSubRet;
			bIsRaiseError = true;
//			CAUTION_LOG(BM::LOG_LV5, _T("[%s-%d] ProcessFailed... Index[%d]"), __FUNCTIONW__, __LINE__, iIndex);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hRet);
			break;
		}

		++iIndex;
		++order_itor;
	}

	return hRet;
}

void PgRealmUserManager::ProcessUpdateCacheDB( PgDoc_Player *pkDocPlayer )
{
	DB_ITEM_STATE_CHANGE_ARRAY kChangeArray;
	
	pkDocPlayer->GetQuery(kChangeArray);
	pkDocPlayer->OnAchievementAbilSaveDB(kChangeArray);

	if ( kChangeArray.size() )
	{
		BM::Stream kPacket;
		CONT_DOC_PLAYER kContCopyPlayer;
		kContCopyPlayer.insert( std::make_pair( SModifyOrderOwner(pkDocPlayer->GetID(), OOT_Player), pkDocPlayer) );
		ProcessModifyItemToDB(SYS_UpdateCacheDB,kChangeArray, kPacket, kContCopyPlayer );

		pkDocPlayer->ClearAllChecker();
	}
}

HRESULT PgRealmUserManager::ProcessModifyItemToDB(EItemModifyParentEventType const kEventCause,DB_ITEM_STATE_CHANGE_ARRAY const &kChangeArray, BM::Stream & rkPacket, CONT_DOC_PLAYER const &kContOrgInv)
{
	typedef std::set< BM::GUID > ContSave;
	ContSave kIngSave, kEndSave;

	CEL::DB_QUERY_TRAN kContItemModifyQuery;
	kContItemModifyQuery.DBIndex(DT_PLAYER);
	kContItemModifyQuery.QueryType(DQT_UPDATE_USER_ITEM);

	DB_ITEM_STATE_CHANGE_ARRAY::const_iterator itor = kChangeArray.begin();
	while(itor!= kChangeArray.end())
	{
		DB_ITEM_STATE_CHANGE const &kElement = (*itor);
		if( false == kElement.IsQuery() )
		{
			++itor;
			continue;
		}
		
		switch(kElement.State())
		{
		case DISCT_REMOVE_SYSTEM://기록 안함,
			{
				g_kSysInvMgr.RemoveSysItem(kElement.RetOwnerGuid(),kElement.PrevItemWrapper().Guid());
				++itor;
				continue;
			}break;
		case DISCT_REMOVE_IMAGE://기록 안함,
		case DISCT_MODIFY_SET_GUID://기록 안함, (각각 컨텐츠에서 개별적으로 관리)
		case DISCT_CREATE_PET:
		default:
			{
				++itor;
				continue;
			}break;
		case DISCT_CREATE:
		case DISCT_MODIFY:
		case DISCT_REMOVE:
		case DISCT_MODIFY_MONEY:
		case DISCT_MODIFY_GUILD_MONEY:
		case DISCT_MODIFY_CP:
		case DISCT_MODIFY_VALUE://아래쪽으로 통과.
		case DISCT_MODIFY_VALUEEX:
		case DISCT_MODIFY_ADD_RANK_POINT:// -> 얘는 저장 어떻게 할지 고민좀..(중요데이터가 아니라서 실시간 안해도됨)
		case DISCT_MODIFY_SET_PVP_RECORD:
		case DISCT_MODIFY_SKILL:
		case DISCT_MODIFY_WORLD_MAP:
		case DISCT_MODIFY_QUEST_ING:
		case DISCT_MODIFY_CLEAR_ING_QUEST:
		case DISCT_MODIFY_QUEST_PARAM:
		case DISCT_MODIFY_QUEST_END:
		case DISCT_MODIFY_QUEST_EXT:
		case DISCT_UPDATE_QUEST_CLEAR_COUNT:
		case DISCT_INIT_QUEST_CLEAR_COUNT:
		case DISCT_CREATE_MAIL:
		case DISCT_ADD_CASH:
		case DISCT_SET_ACHIEVEMENT:
		case DISCT_ACHIEVEMENT2INV:
		case DISCT_COMPLETE_ACHIEVEMENT:
		case DISCT_SET_RENTALSAFETIME:
		case DISCT_OPEN_MARKET:
		case DISCT_MODIFY_MARKET_INFO:
		case DISCT_MODIFY_MARKET_STATE:
		case DISCT_ADD_DEALING:
		case DISCT_REMOVE_DEALING:
		case DISCT_REMOVE_MARKET:
		case DISCT_ADD_ARTICLE:
		case DISCT_REMOVE_ARTICLE:
		case DISCT_INV2ACHIEVEMENT:
		case DISCT_ADD_UNBIND_DATE:
		case DISCT_DEL_UNBIND_DATE:
		case DISCT_SET_DEFAULT_ITEM:
		case DISCT_INVENTORY_EXTEND:
		case DISCT_MODIFY_REFRESHDATE:
		case DISCT_MODIFY_RECOMMENDPOINT:
		case DISCT_CREATE_CHARACTERCARD:
		case DISCT_MODIFY_CHARACTERCARD:
		case DISCT_MODIFY_POPULARPOINT:
		case DISCT_MODIFY_CARDCOMMENT:
		case DISCT_MODIFY_CARDSTATE:
		case DISCT_CREATE_PORTAL:
		case DISCT_DELETE_PORTAL:
		case DISCT_ADD_EMOTION:
		case DISCT_HATCH_PET:
		case DISCT_REMOVE_PET:
		case DISCT_RENAME_PET:
		case DISCT_SETABIL_PET:
		case DISCT_MODIFY_MISSION_EVENT:
		case DISCT_EMPORIAFUNC_UPDATE:
		case DISCT_EXTEND_MAX_IDX:
		case DISCT_GUILD_INV_EXTEND:		
		case DISCT_MODIFY_SWEETHEART:
		case DISCT_MYHOME_MODIFY:
		case DISCT_CREATE_MAIL_BYGUID:
		case DISCT_MYHOME_INVITATION_CLEAR:
		case DISCT_EXTEND_CHARACTER_NUM:
		case DISCT_REMOVE_ITEM_EXTEND_DATA:
		case DISCT_MODIFY_ACHIEVEMENT_RANK:
		case DISCT_MODIFY_SET_DATE_CONTENTS:
		case DISCT_SET_HIDDEN_OPEN:
		case DISCT_COMPLETE_HIDDEN_OPEN:
		case DISCT_SIDEJOB_INSERT:
		case DISCT_SIDEJOB_REMOVE:
		case DISCT_SIDEJOB_MODIFY:
		case DISCT_SIDEJOB_EQUIPCOUNT:
		case DISCT_GAMBLE_INSERT:
		case DISCT_GAMBLE_MODIFY:
		case DISCT_GAMBLE_REMOVE:
		case DISCT_ADD_HOME:
		case DISCT_SET_MYHOME_VISITLOGCOUNT:
		case DISCT_ADD_MYHOME_VISITLOGCOUNT:
		case DISCT_GAMBLE_MIXUP_INSERT:
		case DISCT_GAMBLE_MIXUP_MODIFY:
		case DISCT_GAMBLE_MIXUP_REMOVE:
		case DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY:
		case DISCT_ACHIEVEMENT_TIMELIMIT_DELETE:
		case DISCT_MODIFY_SKILLEXTEND:
		case DISCT_MYHOME_REMOVE:
		case DISCT_MODIFY_GM_INIT_SKILL:
		case DISCT_JOBSKILL_SET_SKILL_EXPERTNESS:
		case DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS:
		case DISCT_JOBSKILL_SAVE_EXHAUSTION:
		case DISCT_DELETE_SKILL:
		case DISCT_PREMIUM_SERVICE_INSERT:
		case DISCT_PREMIUM_SERVICE_MODIFY:
		case DISCT_PREMIUM_SERVICE_REMOVE:
		case DISCT_PREMIUM_ARTICLE_MODIFY:
		case DISCT_DAILY_REWARD_UPDATE:
			{
			}break;
		}

		PgItemWrapper const *pkWrapper = &kElement.ItemWrapper();

		// 			CONT_DOC_PLAYER::iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
		// 			if(inviter == kContOrgInv.end())
		// 			{
		// 				kOwnerName = L"";
		// 			}
		// 			else
		// 			{
		// 				kOwnerName = (*inviter).second->Name();
		// 			}

		BM::Stream kAddonData = kElement.kAddonData;

		switch(kElement.State())
		{
		case DISCT_MYHOME_REMOVE:
			{
				SMOD_MyHome_MyHome_Set_Addr kData;
				kData.ReadFromPacket(kAddonData);
				BM::GUID kHomeGuid;
				kAddonData.Pop(kHomeGuid);

				if(false == m_kMyHomeMgr.RemoveMyHome(kData.StreetNo(), kData.HouseNo()))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_MYHOME_REMOVE,L"EXEC [dbo].[up_User_MyHome_Sell]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.QueryOwner(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kData.StreetNo());
				kQuery.PushStrParam(kData.HouseNo());
				kQuery.contUserData.Push(kHomeGuid);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ADD_HOME:
			{
				SMOD_Add_MyHome kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_ADD_MYHOME,L"EXEC [dbo].[up_User_MyHome_Buy]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.QueryOwner(kElement.RetOwnerGuid());		
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kData.BuildingNo());
				kQuery.PushStrParam(static_cast<__int64>(kData.Cost()));
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_GAMBLE_MIXUP_INSERT:
			{
				SPMOD_Gamble_Insert kData;
				kData.ReadFromPacket(kAddonData);
				m_kMixupUserMgr.Add(kElement.RetOwnerGuid(), kData.RouletteCount(), kData.MixPoint());
			}break;
		case DISCT_GAMBLE_MIXUP_MODIFY:
			{
				CONT_GAMBLEMACHINERESULT kContResult;
				CONT_GAMBLEITEM kContDumy;
				bool bBroadcast = false;

				SGambleUserInfo kGambleInfo;
				m_kMixupUserMgr.Get(kElement.RetOwnerGuid(), kGambleInfo);

				kGambleInfo.kContResult.clear();
				PU::TLoadArray_M(kAddonData, kGambleInfo.kContResult);
				kAddonData.Pop(kGambleInfo.bBroadcast);
				kAddonData.Pop(kGambleInfo.iRouletteCount);

				m_kMixupUserMgr.Set(kElement.RetOwnerGuid(), kGambleInfo);
			}break;
		case DISCT_GAMBLE_MIXUP_REMOVE:
			{
				m_kMixupUserMgr.Remove(kElement.RetOwnerGuid());
			}break;
		case DISCT_GAMBLE_INSERT:
			{
				SPMOD_Gamble_Insert kData;
				kData.ReadFromPacket(kAddonData);
				m_kGambleUserMgr.Add(kElement.RetOwnerGuid(), kData.RouletteCount(), kData.MixPoint());
			}break;
		case DISCT_GAMBLE_MODIFY:
			{
				CONT_GAMBLEMACHINERESULT kContResult;
				CONT_GAMBLEITEM kContDumy;
				bool bBroadcast = false;

				SGambleUserInfo kGambleInfo;
				m_kGambleUserMgr.Get(kElement.RetOwnerGuid(), kGambleInfo);

				kGambleInfo.kContResult.clear();
				PU::TLoadArray_M(kAddonData, kGambleInfo.kContResult);
				kAddonData.Pop(kGambleInfo.bBroadcast);
				kAddonData.Pop(kGambleInfo.iRouletteCount);

				m_kGambleUserMgr.Set(kElement.RetOwnerGuid(), kGambleInfo);
			}break;
		case DISCT_GAMBLE_REMOVE:
			{
				m_kGambleUserMgr.Remove(kElement.RetOwnerGuid());
			}break;
		case DISCT_REMOVE_ITEM_EXTEND_DATA:
			{
				BM::GUID kItemGuid;
				EItemExtendDataType kType = IEDT_NONE;
				kAddonData.Pop(kItemGuid);
				kAddonData.Pop(kType);

				switch(kType)
				{
				case IEDT_EXPCARD:
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_ExpCard_Delete]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.QueryOwner(kElement.RetOwnerGuid());		
						kQuery.PushStrParam(kItemGuid);
						kContItemModifyQuery.push_back(kQuery);
					}break;
				case IEDT_HOME:
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_Equip_Delete]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.QueryOwner(kElement.RetOwnerGuid());		
						kQuery.PushStrParam(kItemGuid);
						kContItemModifyQuery.push_back(kQuery);
					}break;
				case IEDT_MONSTERCARD:
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_MonsterCard_Delete]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.QueryOwner(kElement.RetOwnerGuid());		
						kQuery.PushStrParam(kItemGuid);
						kContItemModifyQuery.push_back(kQuery);
					}break;
				}
			}break;
		case DISCT_EXTEND_CHARACTER_NUM:
			{
				SMOD_Extend_CharacterNum kData;
				kData.ReadFromPacket(kAddonData);
				BM::GUID kMemberGuid;
				kAddonData.Pop(kMemberGuid);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_Member_ShareData_ExtendCharacterNum]");
				kQuery.InsertQueryTarget(kMemberGuid);
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.QueryOwner(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kMemberGuid);
				kQuery.PushStrParam(kData.ExtendNum());
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MYHOME_INVITATION_CLEAR:
			{
				short siStreetNo = 0;
				int	  iHouseNo = 0;

				kAddonData.Pop(siStreetNo);
				kAddonData.Pop(iHouseNo);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_Invitation_Clear]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.QueryOwner(kElement.RetOwnerGuid());
				kQuery.PushStrParam(siStreetNo);
				kQuery.PushStrParam(iHouseNo);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ADD_MYHOME_VISITLOGCOUNT:
			{
				SMOD_MyHome_Sync_VisitLogCount kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(),kMyHome))
				{
					return E_FAIL;
				}

				SMYHOME kOldMyHome = kMyHome;

				kMyHome.iTotalVisitLogCount += kData.VisitLogCount();

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_MYHOME_MODIFY,L"EXEC [dbo].[up_User_MyHome_Base_Modify]");
				kQuery.InsertQueryTarget(kMyHome.kHomeInSideGuid);
				kQuery.QueryOwner(kMyHome.kHomeInSideGuid);
				MYHOMEUTIL::MakeQuery(kMyHome, kOldMyHome, kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_SET_MYHOME_VISITLOGCOUNT:
			{
				SMOD_MyHome_Sync_VisitLogCount kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(),kMyHome))
				{
					return E_FAIL;
				}

				SMYHOME kOldMyHome = kMyHome;

				kMyHome.iTotalVisitLogCount = kData.VisitLogCount();

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_MYHOME_MODIFY,L"EXEC [dbo].[up_User_MyHome_Base_Modify]");
				kQuery.InsertQueryTarget(kMyHome.kHomeInSideGuid);
				kQuery.QueryOwner(kMyHome.kHomeInSideGuid);
				MYHOMEUTIL::MakeQuery(kMyHome, kOldMyHome, kQuery);
				g_kCoreCenter.PushQuery(kQuery);

			}break;
		case DISCT_SIDEJOB_EQUIPCOUNT:
			{
				SMOD_MyHome_Sync_EquipItemCount kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(),kMyHome))
				{
					return E_FAIL;
				}

				kMyHome.iEquipItemCount = kData.EquipCount();

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}
			}break;
		case DISCT_SIDEJOB_INSERT:
			{
				BM::GUID kOwnerGuid;
				kAddonData.Pop(kOwnerGuid);
				SMOD_MyHome_SideJob_Insert kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(),kMyHome))
				{
					return E_FAIL;
				}

				kMyHome.kContSideJob.insert(std::make_pair(kData.SideJob(),SMYHOME_SIDE_JOB( kData.JobType(), kData.JobRate(), kData.EndTime())));

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_SideJob_Insert]");
				kQuery.InsertQueryTarget(kOwnerGuid);
				kQuery.QueryOwner(kOwnerGuid);
				kQuery.PushStrParam(kOwnerGuid);
				kQuery.PushStrParam(static_cast<int>(kData.SideJob()));
				kQuery.PushStrParam(static_cast<BM::DBTIMESTAMP_EX>(kData.EndTime()));
				kQuery.PushStrParam(static_cast<int>(kData.JobType()));
				kQuery.PushStrParam(static_cast<int>(kData.JobRate()));
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_SIDEJOB_REMOVE:
			{
				BM::GUID kOwnerGuid;
				kAddonData.Pop(kOwnerGuid);
				SMOD_MyHome_SideJob_Remove kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(), kMyHome))
				{
					return E_FAIL;
				}

				kMyHome.kContSideJob.erase(kData.SideJob());

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_SideJob_Remove]");
				kQuery.InsertQueryTarget(kOwnerGuid);
				kQuery.QueryOwner(kOwnerGuid);
				kQuery.PushStrParam(kOwnerGuid);
				kQuery.PushStrParam(static_cast<int>(kData.SideJob()));
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_SIDEJOB_MODIFY:
			{
				BM::GUID kOwnerGuid;
				kAddonData.Pop(kOwnerGuid);
				SMOD_MyHome_SideJob_Modify kData;
				kData.ReadFromPacket(kAddonData);

				SMYHOME kMyHome;
				if(false == m_kMyHomeMgr.GetHome(kData.StreetNo(),kData.HouseNo(),kMyHome))
				{
					return E_FAIL;
				}

				CONT_MYHOME_SIDE_JOB::iterator iter = kMyHome.kContSideJob.find(kData.SideJob());
				if(iter == kMyHome.kContSideJob.end())
				{
					return E_FAIL;
				}

				(*iter).second.i64TotalSellCost += kData.SellCost();

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_SideJob_Modify]");
				kQuery.InsertQueryTarget(kOwnerGuid);
				kQuery.QueryOwner(kOwnerGuid);
				kQuery.PushStrParam(kOwnerGuid);
				kQuery.PushStrParam(static_cast<int>(kData.SideJob()));
				kQuery.PushStrParam(kData.SellCost());
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MYHOME_MODIFY:
			{
				if(CIE_Home_Unit_Sync == kEventCause)
				{
					break;
				}

				SMYHOME kMyHome;
				kMyHome.ReadFromPacket(kAddonData);

				SMYHOME kOldMyHome;
				if(false == m_kMyHomeMgr.GetHome(kMyHome.siStreetNo,kMyHome.iHouseNo,kOldMyHome))
				{
					return E_FAIL;
				}

				if(false == m_kMyHomeMgr.SetHome(kMyHome))
				{
					return E_FAIL;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_MYHOME_MODIFY,L"EXEC [dbo].[up_User_MyHome_Base_Modify]");
				kQuery.InsertQueryTarget(kMyHome.kHomeInSideGuid);
				kQuery.QueryOwner(kMyHome.kHomeInSideGuid);
				MYHOMEUTIL::MakeQuery(kMyHome, kOldMyHome, kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ADD_EMOTION:
			{
				SPMOD_AddEmotion kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery;
				BYTE abyData[MAX_DB_EMOTICON_SIZE];

				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter != kContOrgInv.end() )
				{
					switch(kData.Type())
					{
					case ET_EMOTICON:
						{
							kQuery.Set(DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Emoticon]");
							inviter->second->Emoticon().Get(abyData);
						}break;
					case ET_EMOTION:
						{
							kQuery.Set(DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Emotion]");
							inviter->second->Emotion().Get(abyData);
						}break;
					case ET_BALLOON:
						{
							kQuery.Set(DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Balloon]");
							inviter->second->Balloon().Get(abyData);
						}break;
					default:
						{
							CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_ADD_EMOTION] Fail Invalid Emotion Type:") << kData.Type());
							return E_FAIL;
						}break;
					}

					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(abyData,MAX_DB_EMOTICON_SIZE);
					g_kCoreCenter.PushQuery(kQuery);
				}
				else
				{
					return E_FAIL;
				}
			}break;
		case DISCT_CREATE_PORTAL:
			{
				SMOD_Portal_Create kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UserPortal_Create]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kData.Guid());
				kQuery.PushStrParam(kData.Comment());
				kQuery.PushStrParam(kData.GroundNo());
				kQuery.PushStrParam(kData.Pos().x);
				kQuery.PushStrParam(kData.Pos().y);
				kQuery.PushStrParam(kData.Pos().z);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_DELETE_PORTAL:
			{
				SMOD_Portal_Delete kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UserPortal_Delete]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kData.Guid());
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_CARDCOMMENT:
			{
				SMOD_Modify_Comment kData;
				kData.ReadFromPacket(kAddonData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kElement.RetOwnerGuid(), kCard))
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_MODIFY_CARDCOMMENT] Fail Not Found Character Card GUID") << kElement.RetOwnerGuid());
					return E_FAIL;
				}

				kCard.Comment(kData.Comment());

				m_kCardMgr.Locked_SetCard(kCard);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CharacterCard_Modify]");
				kCard.MakeUpdateQuery(kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_CARDSTATE:
			{
				SMOD_Modify_CardState kData;
				kData.ReadFromPacket(kAddonData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kElement.RetOwnerGuid(), kCard))
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_MODIFY_CARDSTATE] Fail Not Found Character Card GUID") << kElement.RetOwnerGuid());
					return E_FAIL;
				}

				kCard.Enable(kData.Enable());

				m_kCardMgr.Locked_SetCard(kCard);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CharacterCard_Modify]");
				kCard.MakeUpdateQuery(kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_CREATE_CHARACTERCARD:
			{
				SMOD_CreateCard kData;
				kData.ReadFromPacket(kAddonData);
				PgCharacterCard kCard(kElement.RetOwnerGuid(),kData.Year(),kData.Sex(),kData.Local(),0,kData.Comment(),true,kData.Constellation(),kData.Blood(),kData.Hobby(),kData.Style(),kData.BGndNo());

				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter != kContOrgInv.end() )
				{
					kCard.Name( inviter->second->Name());
				}
				else
				{
					kCard.Name(std::wstring());
				}

				if(false == m_kCardMgr.Locked_RegCard(kCard))
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_CREATE_CHARACTERCARD] Fail Already Created Character Card GUID") << kElement.RetOwnerGuid());
					return E_FAIL;
				}

				inviter->second->IsCreateCard(true);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CharacterCard_Create]");
				kCard.MakeUpdateQuery(kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_CHARACTERCARD:
			{
				SMOD_ModifyCard kData;
				kData.ReadFromPacket(kAddonData);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kElement.RetOwnerGuid(), kCard))
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_MODIFY_CHARACTERCARD] Fail Not Found Character Card GUID") << kElement.RetOwnerGuid());
					return E_FAIL;
				}

				kCard.Year(kData.Year());
				kCard.Sex(kData.Sex());
				kCard.Local(kData.Local());
				kCard.Comment(kData.Comment());
				kCard.Constellation(kData.Constellation());
				kCard.Blood(kData.Blood());
				kCard.Hobby(kData.Hobby());
				kCard.Style(kData.Style());
				kCard.BGndNo(kData.BGndNo());

				m_kCardMgr.Locked_SetCard(kCard);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CharacterCard_Modify]");
				kCard.MakeUpdateQuery(kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_POPULARPOINT:
			{
				__int64 i64NewPP = 0;
				__int64 i64TodayPP = 0;

				kAddonData.Pop(i64NewPP);
				kAddonData.Pop(i64TodayPP);

				PgCharacterCard kCard;
				if(false == m_kCardMgr.Locked_GetCard(kElement.RetOwnerGuid(), kCard))
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("[DISCT_MODIFY_CHARACTERCARD] Fail Not Found Character Card GUID") << kElement.RetOwnerGuid());
					return E_FAIL;
				}

				kCard.PopularPoint(i64NewPP);
				kCard.TodayPopularPoint(i64TodayPP);

				m_kCardMgr.Locked_SetCard(kCard);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CharacterCard_Modify]");
				kCard.MakeUpdateQuery(kQuery);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_REFRESHDATE:
			{
				int iModifyDate;
				kAddonData.Pop(iModifyDate);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RefreshDate]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iModifyDate);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_RECOMMENDPOINT:
			{
				int iRecommendPoint = 0;
				kAddonData.Pop(iRecommendPoint);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RecommendPoint]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iRecommendPoint);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_GUILD_INV_EXTEND:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Guild));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}
				
				BYTE abySetValue[MAX_DB_INVEXTEND_SIZE] = {0, };
			
				PgInventory* pkInv = g_kGuildMgr.GetInven( kElement.RetOwnerGuid() );
				if( pkInv )
				{
					HRESULT hRet;
					__int64 iCause;
					EInvType kInvType;
					BYTE kReductionNum;

					kAddonData.Pop(hRet);
					kAddonData.Pop(iCause);
					kAddonData.Pop(kInvType);
					kAddonData.Pop(kReductionNum);

					std::wstring wstrQueryName;
					if( IMET_GUILD_INV_EXTEND_LINE == iCause )
					{							
						if( false == pkInv->ExtendMaxIdx( kInvType, kReductionNum ) ) // kReductionNum 더함
						{
							CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("return E_FAIL"));
							return E_FAIL;
						}

						if( false == (*inviter).second->GetInven()->GetExtendIdx2Bin(abySetValue) )
						{
							CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("Not Match Size DR2_User::TB_Guild_Basic_Info::GuildInvExtendIdx"));
							return E_FAIL;
						}

						wstrQueryName = L"EXEC [dbo].[up_Update_TB_Guild_Basic_Info_InvExtend_Line]";
					}
					else if( IMET_GUILD_INV_EXTEND_TAB == iCause )
					{							
						if( false == pkInv->InvExtend( kInvType, kReductionNum ) ) // kReductionNum 뺌
						{
							CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("return E_FAIL"));
							return E_FAIL;
						}

						if( false == (*inviter).second->GetInven()->GetReductionInfo2Bin(abySetValue) )
						{
							CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("Not Match Size DR2_User::TB_Guild_Basic_Info::GuildInvExtend"));
							return E_FAIL;
						}

						wstrQueryName = L"EXEC [dbo].[up_Update_TB_Guild_Basic_Info_InvExtend_Tab]";
					}

					{// 길드금고 확장 브로드 캐스팅
						BM::Stream kPacket(PT_N_C_NFY_GUILD_INV_EXTEND);						
						kPacket.Push(kElement.RetOwnerGuid());
						kPacket.Push(hRet);
						kPacket.Push(iCause);
						kPacket.Push(kInvType);
						kPacket.Push(kReductionNum);
						SendToRealmContents( PMET_GUILD, kPacket );
					}

					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_GUILD_INVENTORY_EXTEND, wstrQueryName);
						kQuery.QueryOwner(kElement.RetOwnerGuid());
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.PushStrParam(kElement.RetOwnerGuid());							
						kQuery.PushStrParam(abySetValue, MAX_DB_INVEXTEND_SIZE);
						g_kCoreCenter.PushQuery(kQuery);
					}						
				}
			}break;		
		case DISCT_INVENTORY_EXTEND:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE];
				if(true == (*inviter).second->GetInven()->GetReductionInfo2Bin(abyInvExtern))
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_InvExtend]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(abyInvExtern,MAX_DB_INVEXTEND_SIZE);
					g_kCoreCenter.PushQuery(kQuery);
				}
				else
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("Not Match Size DR2_User::TB_CharacterSub::InvExtend"));
					return E_FAIL;
				}
			}break;
		case DISCT_EXTEND_MAX_IDX:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				BYTE abyInvExtern[MAX_DB_INVEXTEND_SIZE];
				if(true == (*inviter).second->GetInven()->GetExtendIdx2Bin(abyInvExtern))
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_ExtendMaxIdx]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(abyInvExtern,MAX_DB_INVEXTEND_SIZE);
					g_kCoreCenter.PushQuery(kQuery);
				}
				else
				{
					CAUTION_LOG(BM::LOG_LV0,__FL__ << _T("Not Match Size DR2_User::TB_CharacterSub::ExtendMaxIdx"));
					return E_FAIL;
				}
			}break;
		case DISCT_SET_DEFAULT_ITEM:
			{
				int iEquipPos = 0,
					iItemNo = 0;

				kAddonData.Pop(iEquipPos);
				kAddonData.Pop(iItemNo);

				std::wstring kFieldName;

				switch(iEquipPos)
				{
				case EQUIP_POS_HAIR_COLOR:
					{
						kFieldName = _T("HairColor");
					}break;
				case EQUIP_POS_HAIR:
					{
						kFieldName = _T("HairStyle");
					}break;
				case EQUIP_POS_FACE:
					{
						kFieldName = _T("Face");
					}break;
				case EQUIP_POS_SHIRTS:
					{
						kFieldName = _T("Jacket");
					}break;
				case EQUIP_POS_PANTS:
					{
						kFieldName = _T("Pants");
					}break;
				case EQUIP_POS_BOOTS:
					{
						kFieldName = _T("Shoes");
					}break;
				case EQUIP_POS_GLOVE:
					{
						kFieldName = _T("Gloves");
					}break;
				default:
					{
						// 여기 파츠는 기본 아이템이 없다.
						return E_FAIL;
					}break;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Basic]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kFieldName);
				kQuery.PushStrParam(iItemNo);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ADD_UNBIND_DATE:
			{
				BM::GUID kItemGuid;
				BYTE kDelayDays = 0;
				kAddonData.Pop(kItemGuid);
				kAddonData.Pop(kDelayDays);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Add_UnbindDate]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kItemGuid);
				kQuery.PushStrParam(kDelayDays);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_DEL_UNBIND_DATE:
			{
				BM::GUID kItemGuid;
				kAddonData.Pop(kItemGuid);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Del_UnbindDate]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kItemGuid);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_OPEN_MARKET:
		case DISCT_ADD_ARTICLE:
		case DISCT_REMOVE_ARTICLE:
		case DISCT_ADD_DEALING:
		case DISCT_MODIFY_MARKET_INFO:
		case DISCT_MODIFY_MARKET_STATE:
		case DISCT_REMOVE_DEALING:
		case DISCT_REMOVE_MARKET:
			{
				HRESULT hMyResult = m_kOpenMarketMgr.Locked_ProcessModifyItemToDB_OpenMarket(kEventCause, kElement, kAddonData, kContItemModifyQuery);
				if (hMyResult != S_OK)
				{
					return hMyResult;
				}
			}break;
		case DISCT_SET_RENTALSAFETIME:
			{
				EInvType kInvType;
				BM::DBTIMESTAMP_EX kDBTime;
				kAddonData.Pop(kInvType);
				kAddonData.Pop(kDBTime);

				switch(kInvType)
				{
				case IT_RENTAL_SAFE1:
				case IT_RENTAL_SAFE2:
				case IT_RENTAL_SAFE3:
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RentalDate]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.PushStrParam(kElement.RetOwnerGuid());

						std::wstring wstrInvType;
						switch(kInvType)
						{
						case IT_RENTAL_SAFE1:{wstrInvType = L"RentalSafeDate1";}break;
						case IT_RENTAL_SAFE2:{wstrInvType = L"RentalSafeDate2";}break;
						case IT_RENTAL_SAFE3:{wstrInvType = L"RentalSafeDate3";}break;
						}

						kQuery.PushStrParam(wstrInvType);
						kQuery.PushStrParam(kDBTime);
						g_kCoreCenter.PushQuery(kQuery);
					}break;
				case IT_SHARE_RENTAL_SAFE1:
				case IT_SHARE_RENTAL_SAFE2:
				case IT_SHARE_RENTAL_SAFE3:
				case IT_SHARE_RENTAL_SAFE4:
					{
						BM::GUID kMemberGuid;
						kAddonData.Pop(kMemberGuid);

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_Member_ShareData_RentalDate]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.InsertQueryTarget(kMemberGuid);
						kQuery.PushStrParam(kMemberGuid);

						std::wstring wstrInvType;
						switch(kInvType)
						{
						case IT_SHARE_RENTAL_SAFE1:{wstrInvType = L"ShareRentalTime1";}break;
						case IT_SHARE_RENTAL_SAFE2:{wstrInvType = L"ShareRentalTime2";}break;
						case IT_SHARE_RENTAL_SAFE3:{wstrInvType = L"ShareRentalTime3";}break;
						case IT_SHARE_RENTAL_SAFE4:{wstrInvType = L"ShareRentalTime4";}break;
						}

						kQuery.PushStrParam(wstrInvType);
						kQuery.PushStrParam(kDBTime);
						g_kCoreCenter.PushQuery(kQuery);
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_SET_RENTALSAFETIME [") << kInvType << _T("]") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						return E_FAIL;
					}break;
				}
			}break;
		case DISCT_SET_ACHIEVEMENT:
		case DISCT_ACHIEVEMENT2INV:
		case DISCT_COMPLETE_ACHIEVEMENT:
		case DISCT_INV2ACHIEVEMENT:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				CONT_ACHIEVEMENTS kAchievements;
				inviter->second->GetAchievements()->GetBuffer(kAchievements);

				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Change_Character_Achievements]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam( &(kAchievements.at(0)), kAchievements.size() );
					g_kCoreCenter.PushQuery(kQuery);
				}
			}break;
		case DISCT_SET_HIDDEN_OPEN:
			{
				BM::DBTIMESTAMP_EX kLastHiddenUpdate;

				kAddonData.Pop( kLastHiddenUpdate );

				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				CONT_HIDDENOPEN kHiddenOpen;

				inviter->second->GetHiddenOpen()->GetBuffer(kHiddenOpen);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Change_Character_HiddenOpen]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam( &(kHiddenOpen.at(0)), kHiddenOpen.size() );
				kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kLastHiddenUpdate));

				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_COMPLETE_HIDDEN_OPEN:
			{
				BM::DBTIMESTAMP_EX kLastHiddenUpdate;
				g_kEventView.GetLocalTime( kLastHiddenUpdate );

				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				CONT_HIDDENOPEN kHiddenOpen;
				inviter->second->GetHiddenOpen()->GetBuffer(kHiddenOpen);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Change_Character_HiddenOpen]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam( &(kHiddenOpen.at(0)), kHiddenOpen.size() );
				kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kLastHiddenUpdate));					

				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ACHIEVEMENT_TIMELIMIT_DELETE:
			{
				int iSaveIdx = 0;
				kAddonData.Pop( iSaveIdx );
				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UserAchievement_TimeLimit_Delete]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iSaveIdx);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ACHIEVEMENT_TIMELIMIT_MODIFY:
			{
				int iSaveIdx = 0;
				BM::PgPackedTime kPackedTime;
				kAddonData.Pop(iSaveIdx);
				kAddonData.Pop(kPackedTime);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_UserAchievement_TimeLimit_Modify]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iSaveIdx);
				kQuery.PushStrParam(static_cast<int>(kPackedTime.GetTime()));
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_ACHIEVEMENT_RANK:
			{
				SPMOD_Complete_Achievement kData;
				kData.ReadFromPacket(kAddonData);
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_UserAchievementRank]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kData.Category());
					kQuery.PushStrParam(kData.RankPoint());
					g_kCoreCenter.PushQuery(kQuery);
				}

				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_ACHIEVEMENT_FIST,L"EXEC [dbo].[up_Update_UserAchievementFirstComplete]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.QueryOwner(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kData.SaveIdx());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					g_kCoreCenter.PushQuery(kQuery);
				}

				if(true == g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA))
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Notify_Achievement_Complete]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.QueryOwner(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kData.SaveIdx());
					g_kCoreCenter.PushQuery(kQuery);
				}

			}break;
		case DISCT_REMOVE:
			{
				pkWrapper = &kElement.PrevItemWrapper();

				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Remove]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.QueryOwner(kElement.RetOwnerGuid());
					kQuery.PushStrParam(pkWrapper->Guid());

					kContItemModifyQuery.push_back(kQuery);
				}

				CONT_ITEMEXTENDDATA const & kCont = pkWrapper->Cont();
				for(CONT_ITEMEXTENDDATA::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
				{
					switch((*iter).first)
					{
					case IEDT_EXPCARD:
						{
							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_ExpCard_Delete]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());		
							kQuery.PushStrParam(pkWrapper->Guid());
							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_HOME:
						{
							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_Equip_Delete]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());		
							kQuery.PushStrParam(pkWrapper->Guid());
							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_MONSTERCARD:
						{
							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_MonsterCard_Delete]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());		
							kQuery.PushStrParam(pkWrapper->Guid());
							kContItemModifyQuery.push_back(kQuery);
						}break;
					}
				}

				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());

				if ( 1 == kChangeArray.size() )
				{
					// 트랜잭션 처리가 1개라면 아이템을 버린거다.(암튼 아이템을 지우는거다.)
					if ( UIT_STATE_PET == pkWrapper->State() )
					{
						// 펫아이템이네... 펫정보를 지워주자.
						CEL::DB_QUERY kPetDelQuery( DT_PLAYER, DQT_REMOVE_PET, L"EXEC [dbo].[up_DeletePet]" );
						kPetDelQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kPetDelQuery.QueryOwner(kElement.RetOwnerGuid());
						kPetDelQuery.PushStrParam(pkWrapper->Guid());

						kContItemModifyQuery.push_back(kPetDelQuery);
					}
				}
			}break;
		case DISCT_CREATE:
			{
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Create]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());

					kQuery.PushStrParam(pkWrapper->Guid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(pkWrapper->Pos().x);
					kQuery.PushStrParam(pkWrapper->Pos().y);
					kQuery.PushStrParam(pkWrapper->ItemNo());

					kQuery.PushStrParam(pkWrapper->Count());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_1());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_2());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_3());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_4());
					kQuery.PushStrParam(pkWrapper->State());
					kQuery.PushStrParam(static_cast<BM::DBTIMESTAMP_EX>(pkWrapper->CreateDate()));

					// Stat track by reOiL
					const bool bHasStatTrack = pkWrapper->StatTrackInfo().HasStatTrack();
					kQuery.PushStrParam(bHasStatTrack ? AT_STAT_TRACK : 0); // Type01
					kQuery.PushStrParam(bHasStatTrack ? 1 : 0);				// Value01
					kQuery.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_MON : 0); // Type02
					kQuery.PushStrParam(pkWrapper->StatTrackInfo().MonsterKillCount());	   // Value02
					kQuery.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_PLAYER : 0); // Type03
					kQuery.PushStrParam(pkWrapper->StatTrackInfo().PlayerKillCount());		  // Value03
					kQuery.PushStrParam(0); // Type04
					kQuery.PushStrParam(0); // Value04
					kQuery.PushStrParam(0); // Type05
					kQuery.PushStrParam(0); // Value05
					
					kContItemModifyQuery.push_back(kQuery);
				}

				CONT_ITEMEXTENDDATA const & kCont = pkWrapper->Cont();
				for(CONT_ITEMEXTENDDATA::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
				{
					switch((*iter).first)
					{
					case IEDT_EXPCARD:
						{
							SExpCard kExpCard;
							(*iter).second.Get(kExpCard);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_ExpCard_Create]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(kExpCard.CurExp());
							kQuery.PushStrParam(kExpCard.MaxExp());
							kQuery.PushStrParam(kExpCard.ExpPer());

							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_HOME:
						{
							SHomeItem kHomeItem;
							(*iter).second.Get(kHomeItem);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_Equip_Create]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(kHomeItem.Pos().x);
							kQuery.PushStrParam(kHomeItem.Pos().y);
							kQuery.PushStrParam(kHomeItem.Pos().z);
							kQuery.PushStrParam(kHomeItem.Dir());
							kQuery.PushStrParam(kHomeItem.ParentGuid());
							kQuery.PushStrParam(kHomeItem.LinkIdx());

							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_MONSTERCARD:
						{
							SMonsterCardTimeLimit kCard;
							(*iter).second.Get(kCard);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_MonsterCard_Create]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kCard.LimitTime()));

							kContItemModifyQuery.push_back(kQuery);
						}break;
					}
				}

				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_HATCH_PET:
			{
				kContItemModifyQuery.QueryType(DQT_HATCH_PET);

				PgItem_PetInfo *pkPetInfo = NULL;
				if ( true == pkWrapper->GetExtInfo( pkPetInfo ) )
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_HATCH_PET, L"EXEC [dbo].[up_Hatch_Pet]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());

					kQuery.PushStrParam(pkWrapper->Guid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(pkWrapper->Pos().x);
					kQuery.PushStrParam(pkWrapper->Pos().y);
					kQuery.PushStrParam(pkWrapper->ItemNo());

					kQuery.PushStrParam(pkWrapper->Count());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_1());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_2());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_3());
					kQuery.PushStrParam(pkWrapper->EnchantInfo().Field_4());
					kQuery.PushStrParam(pkWrapper->State());
					kQuery.PushStrParam(BM::DBTIMESTAMP_EX(pkWrapper->CreateDate()));

					kQuery.PushStrParam( pkPetInfo->Name() );
					kQuery.PushStrParam( pkPetInfo->ClassKey().iClass );
					kQuery.PushStrParam( pkPetInfo->ClassKey().nLv );
					kQuery.PushStrParam( pkPetInfo->Exp() );

					kQuery.PushStrParam( pkPetInfo->GetAbil( AT_MP ) );
					kQuery.PushStrParam( pkPetInfo->GetState_Health() );
					kQuery.PushStrParam( pkPetInfo->GetState_Mental() );
					kQuery.PushStrParam( pkPetInfo->GetAbil( AT_COLOR_INDEX ) );

					kQuery.contUserData.Push( pkWrapper->Guid() );

					kContItemModifyQuery.push_back(kQuery);
					kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!" );
				}

			}break;
		case DISCT_REMOVE_PET:
			{
				BM::GUID kPetID;
				kAddonData.Pop( kPetID );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_REMOVE_PET, L"EXEC [dbo].[up_DeletePet]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());

				kQuery.PushStrParam( kPetID );

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_RENAME_PET:
			{
				PgItem_PetInfo *pkPetInfo = NULL;
				if ( true == pkWrapper->GetExtInfo( pkPetInfo ) )
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_RENAME_PET, L"EXEC [dbo].[up_Update_Pet_Name]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());

					kQuery.PushStrParam( pkWrapper->Guid() );
					kQuery.PushStrParam( pkPetInfo->Name() );

					kContItemModifyQuery.push_back(kQuery);
					kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
				}
			}break;
		case DISCT_SETABIL_PET:
			{
				SPMOD_AddAbilPet::CONT_ABILLIST kAbilList;
				kAddonData.Pop( kAbilList );

				SPMOD_AddAbilPet::CONT_ABILLIST::const_iterator abil_itr = kAbilList.begin();
				for ( ; abil_itr != kAbilList.end() ; ++abil_itr )
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_SETABIL_PET, L"EXEC [dbo].[up_Update_Pet]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());

					kQuery.PushStrParam( pkWrapper->Guid() );

					switch ( abil_itr->Type() )
					{
					case AT_LEVEL:			{kQuery.PushStrParam( std::wstring(L"Lv") );		}break;
					case AT_CLASS:			{kQuery.PushStrParam( std::wstring(L"Class") );		}break;
					case AT_EXPERIENCE:		{kQuery.PushStrParam( std::wstring(L"Exp") );		}break;
					case AT_MP:				{kQuery.PushStrParam( std::wstring(L"MP") );		}break;
					case AT_HEALTH:			{kQuery.PushStrParam( std::wstring(L"Health") );	}break;
					case AT_MENTAL:			{kQuery.PushStrParam( std::wstring(L"Mental") );	}break;
					case AT_COLOR_INDEX:	{kQuery.PushStrParam( std::wstring(L"Color") );		}break;
					case AT_MON_SKILL_01:	{kQuery.PushStrParam( std::wstring(L"Skill01"));	}break;
					case AT_MON_SKILL_02:	{kQuery.PushStrParam( std::wstring(L"Skill02"));	}break;
					case AT_MON_SKILL_03:	{kQuery.PushStrParam( std::wstring(L"Skill03"));	}break;
					default:
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_SETABIL_PET [") << abil_itr->Type() << _T("]") );
							return E_FAIL;
						}break;
					}

					kQuery.PushStrParam( abil_itr->Value() );

					kContItemModifyQuery.push_back(kQuery);
				}

				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_EMPORIAFUNC_UPDATE:
			{
				SEmporiaKey kEmporiaKey;
				short nFuncNo = 0;
				__int64 i64ExtValue = 0i64;
				kAddonData.Pop( kEmporiaKey );
				kAddonData.Pop( nFuncNo );
				kAddonData.Pop( i64ExtValue );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_EMPORIA_FUNCTION, _T("EXEC [dbo].[up_UpdateEmporiaFunction]") );
				kQuery.InsertQueryTarget( kEmporiaKey.kID );
				kQuery.QueryOwner( kElement.RetOwnerGuid() );

				kQuery.PushStrParam( kEmporiaKey.kID );
				kQuery.PushStrParam( kEmporiaKey.byGrade );
				kQuery.PushStrParam( nFuncNo );
				kQuery.PushStrParam( i64ExtValue );

				kQuery.contUserData.Push( kEmporiaKey );
				kQuery.contUserData.Push( nFuncNo );
				kQuery.contUserData.Push( i64ExtValue );

				g_kCoreCenter.PushQuery( kQuery );
			}break;
		case DISCT_MODIFY:
			{
				{
					CEL::DB_QUERY kQueryCreate( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Item_Modify]");
					kQueryCreate.InsertQueryTarget(kElement.RetOwnerGuid());
					kQueryCreate.PushStrParam(pkWrapper->Guid());
					kQueryCreate.PushStrParam(kElement.RetOwnerGuid());
					kQueryCreate.PushStrParam(pkWrapper->Pos().x);
					kQueryCreate.PushStrParam(pkWrapper->Pos().y);
					kQueryCreate.PushStrParam(pkWrapper->ItemNo());

					kQueryCreate.PushStrParam(pkWrapper->Count());
					kQueryCreate.PushStrParam(pkWrapper->EnchantInfo().Field_1());
					kQueryCreate.PushStrParam(pkWrapper->EnchantInfo().Field_2());
					kQueryCreate.PushStrParam(pkWrapper->EnchantInfo().Field_3());
					kQueryCreate.PushStrParam(pkWrapper->EnchantInfo().Field_4());
					kQueryCreate.PushStrParam(pkWrapper->State());
					kQueryCreate.PushStrParam(static_cast<BM::DBTIMESTAMP_EX>(pkWrapper->CreateDate()));

					// Stat track by reOiL
					const bool bHasStatTrack = pkWrapper->StatTrackInfo().HasStatTrack();
					kQueryCreate.PushStrParam(bHasStatTrack ? AT_STAT_TRACK : 0); // Type01
					kQueryCreate.PushStrParam(bHasStatTrack ? 1 : 0);				// Value01
					kQueryCreate.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_MON : 0); // Type02
					kQueryCreate.PushStrParam(pkWrapper->StatTrackInfo().MonsterKillCount());	   // Value02
					kQueryCreate.PushStrParam(bHasStatTrack ? AT_STAT_TRACK_KILL_COUNT_PLAYER : 0); // Type03
					kQueryCreate.PushStrParam(pkWrapper->StatTrackInfo().PlayerKillCount());		  // Value03
					kQueryCreate.PushStrParam(0); // Type04
					kQueryCreate.PushStrParam(0); // Value04
					kQueryCreate.PushStrParam(0); // Type05
					kQueryCreate.PushStrParam(0); // Value05
					
					kContItemModifyQuery.push_back(kQueryCreate);
				}

				CONT_ITEMEXTENDDATA const & kCont = pkWrapper->Cont();
				for(CONT_ITEMEXTENDDATA::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
				{
					switch((*iter).first)
					{ 
					case IEDT_EXPCARD:
						{
							SExpCard kExpCard;
							(*iter).second.Get(kExpCard);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_ExpCard_Modify]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(kExpCard.CurExp());
							kQuery.PushStrParam(kExpCard.MaxExp());
							kQuery.PushStrParam(kExpCard.ExpPer());

							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_HOME:
						{
							SHomeItem kHomeItem;
							(*iter).second.Get(kHomeItem);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_User_MyHome_Equip_Modify]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(kHomeItem.Pos().x);
							kQuery.PushStrParam(kHomeItem.Pos().y);
							kQuery.PushStrParam(kHomeItem.Pos().z);
							kQuery.PushStrParam(kHomeItem.Dir());
							kQuery.PushStrParam(kHomeItem.ParentGuid());
							kQuery.PushStrParam(kHomeItem.LinkIdx());

							kContItemModifyQuery.push_back(kQuery);
						}break;
					case IEDT_MONSTERCARD:
						{
							SMonsterCardTimeLimit kCard;
							(*iter).second.Get(kCard);

							CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_MonsterCard_Modify]");
							kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
							kQuery.QueryOwner(kElement.RetOwnerGuid());

							kQuery.PushStrParam(pkWrapper->Guid());
							kQuery.PushStrParam(BM::DBTIMESTAMP_EX(kCard.LimitTime()));

							kContItemModifyQuery.push_back(kQuery);
						}break;
					}
				}

				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_MODIFY_GUILD_MONEY:
			{
				__int64 iMoney = 0;
				kAddonData.Pop(iMoney);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_GuildMoney_Modify]" );
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iMoney);

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_MODIFY_MONEY:
			{
				__int64 iMoney = 0;
				kAddonData.Pop(iMoney);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Money_Modify]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iMoney);

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_MODIFY_CP:
			{
				int iCP = 0;
				kAddonData.Pop(iCP);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_CP_Modify]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(iCP);

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_MODIFY_ADD_RANK_POINT:
			{
				int iType = E_RANKPOINT_NONE;
				int iAddValue = 0;
				kAddonData.Pop(iType);
				kAddonData.Pop(iAddValue);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_RankPoint2]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());

				switch(iType)
				{
				case E_RANKPOINT_MAXCOMBO:
					{
						kQuery.PushStrParam(std::wstring(_T("MaxCombo")));
					}break;
				case E_RANKPOINT_FOODMAKE:
					{
						kQuery.PushStrParam(std::wstring(_T("FoodMakeCount")));
					}break;
				case E_RANKPOINT_ENCHANTSUCCEED:
					{
						kQuery.PushStrParam(std::wstring(_T("EnchantSucceedCount")));
					}break;
				case E_RANKPOINT_ENCHANTFAILED:
					{
						kQuery.PushStrParam(std::wstring(_T("EnchantFailedCount")));
					}break;
				case E_RANKPOINT_RECVMAIL_COUNT:
					{
						kQuery.PushStrParam( std::wstring( _T("RecvMailCount")) );
					}break;
				case E_RANKPOINT_USEPHOENIXFEATHER_COUNT:
					{
						kQuery.PushStrParam( std::wstring(_T("UsePhoenixFeatherCount")) );
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_MODIFY_SET_RANK_POINT [") << iType << _T("]") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						return E_FAIL;
					}break;
				}

				kQuery.PushStrParam(iAddValue);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_SET_PVP_RECORD:
			{
				SPMOD_SAbil kData;
				kData.ReadFromPacket( kAddonData );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Extern]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());

				switch( kData.Type() )
				{
				case GAME_DRAW:
					{
						kQuery.PushStrParam(std::wstring(_T("PvP_Draw")));
					}break;
				case GAME_WIN:
					{
						kQuery.PushStrParam(std::wstring(_T("PvP_Win")));
					}break;
				case GAME_LOSE:
					{
						kQuery.PushStrParam(std::wstring(_T("PvP_Lose")));
					}break;
				case GAME_DRAW_EXERCISE:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Draw_Exercise"));
					}break;
				case GAME_WIN_EXERCISE:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Win_Exercise"));
					}break;
				case GAME_LOSE_EXERCISE:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Lose_Exercise"));
					}break;
				case GAME_POINT:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Point"));
					}break;
				case GAME_KILL:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Kill"));
					}break;
				case GAME_DEATH:
					{
						kQuery.PushStrParam(std::wstring(L"PvP_Death"));
					}break;
				default:
					{
						CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_MODIFY_SET_PVP_RECORD [") << kData.Type() << _T("]") );
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
						return E_FAIL;
					}break;
				}

				kQuery.PushStrParam( kData.Value() );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_GM_INIT_SKILL:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				RealmUserManagerUtil::SaveSkill(kElement, inviter->second);
				RealmUserManagerUtil::SaveSkillExt(kElement, inviter->second);
			}break;
		case DISCT_MODIFY_SKILLEXTEND:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				RealmUserManagerUtil::SaveSkillExt(kElement, inviter->second);
			}break;
		case DISCT_MODIFY_SKILL:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				RealmUserManagerUtil::SaveSkill(kElement, inviter->second);
			}break;
		case DISCT_DELETE_SKILL:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				RealmUserManagerUtil::SaveSkill(kElement, inviter->second);
			}break;
		case DISCT_MODIFY_WORLD_MAP:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				PgUserMapInfo<DWORD> kWorldMapFlag;
				inviter->second->GetWorldMap(kWorldMapFlag);

				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SAVECHARACTER_MAPINFO, L"EXEC [dbo].[up_SaveCharacterMapInfo]" );
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());

				BYTE *pData = (BYTE*)(&kWorldMapFlag);
				kQuery.PushStrParam(pData, sizeof(kWorldMapFlag));
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_UPDATE_QUEST_CLEAR_COUNT:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				int iQuestID = 0;
				kAddonData.Pop( iQuestID );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_QUEST_CLEAR_COUNT,L"EXEC [dbo].[up_Update_User_WantedQuest_ClearCount]");
				kQuery.InsertQueryTarget(inviter->first.kOwnerGuid);
				kQuery.PushStrParam(inviter->first.kOwnerGuid);
				kQuery.PushStrParam(iQuestID);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_INIT_QUEST_CLEAR_COUNT:
			{// 현상 수배 퀘스트 클리어 카운트 초기화
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				BM::GUID OwnerGuid;
				kAddonData.Pop( OwnerGuid );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_INIT_QUEST_CLEAR_COUNT,L"EXEC [dbo].[up_UserWantedQuest_Init]");
				kQuery.InsertQueryTarget(inviter->first.kOwnerGuid);
				kQuery.PushStrParam(OwnerGuid);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_MODIFY_QUEST_ING:
		case DISCT_MODIFY_QUEST_PARAM:
		case DISCT_MODIFY_QUEST_EXT:
		case DISCT_MODIFY_CLEAR_ING_QUEST:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				if( kIngSave.end() == kIngSave.find( inviter->first.kOwnerGuid ) ) // 한번만!
				{
					BYTE aryIngQuest[MAX_DB_INGQUEST_SIZE] = {0, };
					(*inviter).second->GetMyQuest()->WriteIngToBuff(PgQuestIngBuffer(aryIngQuest));
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_ING_QUEST, L"EXEC [dbo].[UP_Change_Character_IngQuest]");
					kQuery.InsertQueryTarget(inviter->first.kOwnerGuid);
					kQuery.PushStrParam( inviter->first.kOwnerGuid );
					kQuery.PushStrParam( aryIngQuest, MAX_DB_INGQUEST_SIZE );
					kQuery.QueryOwner( (*inviter).second->GetMemberGUID() );
					g_kCoreCenter.PushQuery(kQuery);

					kIngSave.insert( inviter->first.kOwnerGuid );
				}
			}break;
		case DISCT_MODIFY_QUEST_END:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				if( kEndSave.end() == kEndSave.find( inviter->first.kOwnerGuid ) ) // 한번만!
				{
					BYTE aryEndQuest[MAX_DB_ENDQUEST_SIZE] = {0, };
					BYTE aryEndQuest2[MAX_DB_ENDQUEST_SIZE] = {0, };
					BYTE aryEndQuestExt[MAX_DB_ENDQUEST_EXT_SIZE] = {0, };
					BYTE aryEndQuestExt2[MAX_DB_ENDQUEST_EXT2_SIZE] = {0, };

					(*inviter).second->GetMyQuest()->WriteEndToBuff(PgQuestEndBuffer(aryEndQuest), PgQuestEndBuffer(aryEndQuest2), PgQuestEndExtBuffer(aryEndQuestExt), PgQuestEndExt2Buffer(aryEndQuestExt2));
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_END_QUEST, L"EXEC [dbo].[UP_Change_Character_EndQuest]");
					kQuery.PushStrParam( inviter->first.kOwnerGuid );
					kQuery.InsertQueryTarget( inviter->first.kOwnerGuid );
					kQuery.PushStrParam( aryEndQuest, MAX_DB_ENDQUEST_SIZE );
					kQuery.PushStrParam( aryEndQuest2, MAX_DB_ENDQUEST_SIZE );
					kQuery.PushStrParam( aryEndQuestExt, MAX_DB_ENDQUEST_EXT_SIZE );
					kQuery.PushStrParam( aryEndQuestExt2, MAX_DB_ENDQUEST_EXT2_SIZE );
					kQuery.QueryOwner( (*inviter).second->GetMemberGUID() );
					g_kCoreCenter.PushQuery(kQuery);

					kEndSave.insert( inviter->first.kOwnerGuid );
				}
			}break;
		case DISCT_MODIFY_SET_DATE_CONTENTS:
			{
				EDateContentsType eType = DCT_NONE;
				BM::DBTIMESTAMP_EX kDateTime;

				kAddonData.Pop( eType );
				kAddonData.Pop( kDateTime );

				std::wstring kFieldName;
				switch( eType )
				{
				case DCT_GUILD_LEAVE_DATE:	{ kFieldName.swap( std::wstring(L"f_GuildLeaveDate") ); }break;
				default:
					{
					}break;
				}

				if( !kFieldName.empty() )
				{
					CEL::DB_QUERY kQuery(DT_PLAYER, DQT_UPDATE_DATE_CONTENTS, L"dbo.[UP_UPDATE_TB_UserCharacter_DateContents]");
					kQuery.PushStrParam( kElement.RetOwnerGuid() );
					kQuery.PushStrParam( kFieldName );
					kQuery.PushStrParam( kDateTime );
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					g_kCoreCenter.PushQuery(kQuery);
				}
			}break;
		case DISCT_MODIFY_VALUEEX:
		case DISCT_MODIFY_VALUE:
			{
				int iAbilType = 0;
				__int64 i64AbilValue = 0i64;

				kAddonData.Pop(iAbilType);
				kAddonData.Pop(i64AbilValue);

				if((AT_ACHIEVEMENT_DATA_MIN < iAbilType) && (iAbilType < AT_ACHIEVEMENT_DATA_MAX))
				{
					CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[UP_Update_UserAchievements]");
					kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
					kQuery.PushStrParam(kElement.RetOwnerGuid());
					kQuery.PushStrParam(iAbilType);
					kQuery.PushStrParam(static_cast<int>(i64AbilValue));
					g_kCoreCenter.PushQuery(kQuery);
				}
				else
				{
					std::wstring kFieldName;

					switch(iAbilType)
					{
					case AT_EXPERIENCE:		{	kFieldName = L"EXP";				}break;
					case AT_SP:				{	kFieldName = L"ExtraSkillPoint";	}break;
					case AT_CP:				{	kFieldName = L"CP";					}break;
					case AT_FRAN:			{	kFieldName = L"FRAN";				}break;
					case AT_TACTICS_EXP:	{	kFieldName = L"TacticsExp";			}break;
					case AT_TACTICS_LEVEL:	{	kFieldName = L"TacticsLevel";		}break;
					case AT_CLASS:
						{
							kFieldName = L"Class";

							if( true == g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) )
							{
								__int64 i64OldAbilValue = 0i64;
								if ( true == kAddonData.Pop(i64OldAbilValue) )
								{
									CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Notify_Class_Changed]");
									kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
									kQuery.PushStrParam(kElement.RetOwnerGuid());
									kQuery.PushStrParam(i64OldAbilValue);
									kQuery.PushStrParam(i64AbilValue);
									g_kCoreCenter.PushQuery(kQuery);
								}
							}
						}break;
					case AT_STATUS_BONUS:	{	kFieldName = L"BonusStatus";		}break;
					case AT_STR_ADD:		{	kFieldName = L"AddedSTR";			}break;
					case AT_INT_ADD:		{	kFieldName = L"AddedINT";			}break;
					case AT_DEX_ADD:		{	kFieldName = L"AddedDEX";			}break;
					case AT_CON_ADD:		{	kFieldName = L"AddedCON";			}break;
					case AT_STRATEGYSKILL_OPEN:	{ kFieldName = L"SkillTabOpen";		}break;
					case AT_LEVEL:
						{
							BM::GUID kMemberGuid;
							__int64 i64PlayTimeSec = 0i64;
							int iEffectLv = 0;
							if (	true == kAddonData.Pop(kMemberGuid) 
								&&	true == kAddonData.Pop(i64PlayTimeSec) 
								&&	true == kAddonData.Pop(iEffectLv) )
							{
								// 레벨업인 경우에는 플레이 시간을 저장해야 한다.
								CEL::DB_QUERY kQuery( DT_PLAYER, DQT_RECORD_LEVELUP, L"EXEC [dbo].[up_Update_UserCharacter_LevelUp]" );
								kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
								kQuery.QueryOwner(kElement.RetOwnerGuid());
								kQuery.PushStrParam(kMemberGuid);
								kQuery.PushStrParam(kElement.RetOwnerGuid());
								kQuery.PushStrParam(i64AbilValue);
								kQuery.PushStrParam(i64PlayTimeSec);

								kQuery.contUserData.Push( iEffectLv );
								g_kCoreCenter.PushQuery(kQuery);

								if( g_kEventView.VariableCont().iReviveFeather_LevelUp_Use )
								{// 레벨업하면 부활 깃털 지급
									CONT_DEF_SPECIFIC_REWARD const *pkContSpecificReward = NULL;
									g_kTblDataMgr.GetContDef( pkContSpecificReward );

									if( pkContSpecificReward )
									{// i64AbilValue == 새로운 레벨
										CONT_DEF_SPECIFIC_REWARD::const_iterator iter = pkContSpecificReward->find( POINT2( AT_SPECIFICR_EWARD_REVIVE_FEATHER_LEVELUP, static_cast<int>(i64AbilValue) ) );
										if( iter != pkContSpecificReward->end() )
										{
											CONT_DEF_SPECIFIC_REWARD::mapped_type const& kValue = (*iter).second;
											CONT_ITEM_CREATE_ORDER kOrder;
											if( OrderCreateItem(kOrder, kValue.iRewardItemNo, kValue.i64RewardCount) )
											{
												SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
												pkActionOrder->InsertTarget(kElement.RetOwnerGuid());
												pkActionOrder->kCause = CIE_Revive_Feather_LevelUp;
												for(CONT_ITEM_CREATE_ORDER::const_iterator c_it=kOrder.begin(); c_it!=kOrder.end(); ++c_it)
												{
													pkActionOrder->kContOrder.push_back( SPMO(IMET_INSERT_FIXED, kElement.RetOwnerGuid(), SPMOD_Insert_Fixed(*c_it, SItemPos(), true)) );
												}
												g_kJobDispatcher.VPush(pkActionOrder);

												BM::DBTIMESTAMP_EX kNowTime;
												kNowTime.SetLocalTime();
												CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_SPECIFIC_REWARD, L"EXEC [dbo].[up_Update_SpecificReward]" );
												kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
												kQuery.PushStrParam( static_cast<int>(AT_SPECIFICR_EWARD_REVIVE_FEATHER_LEVELUP) );
												kQuery.PushStrParam( kElement.RetOwnerGuid() );						
												kQuery.PushStrParam( kNowTime );
												g_kCoreCenter.PushQuery(kQuery);
											}
										}
									}
								}

								if( true == g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) )
								{
									__int64 i64OldAbilValue = 0i64;
									if ( true == kAddonData.Pop(i64OldAbilValue) )
									{
										CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Notify_Level_Changed]");
										kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
										kQuery.PushStrParam(kElement.RetOwnerGuid());
										kQuery.PushStrParam(i64OldAbilValue);
										kQuery.PushStrParam(i64AbilValue);
										g_kCoreCenter.PushQuery(kQuery);
									}
								}
							}
							else
							{
								CAUTION_LOG( BM::LOG_LV0, __FL__ << L"Error!!!" );
							}
						}break;
					case AT_LAST_VILLAGE:
					case AT_RECENT:
						{
							// 여기서는 업데이트 하지 않는다.
							INFO_LOG( BM::LOG_LV0, __FL__ << _T("Not Use iAbilType =") << iAbilType );
							CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Not Use iAbilType =") << iAbilType );
						}break;
					case AT_MYHOME_TODAYHIT:
					case AT_MYHOME_TOTALHIT:
					case AT_MYHOME_VISITLOGCOUNT:
					case AT_LEVEL_RANK:
						{
							// 이건 원래 Pass!!
							break;
						}break;
					default:
						{
							CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Invalid AbilType=") << iAbilType << _T(", Value64=") << i64AbilValue);
						}break;
					}

					if ( !kFieldName.empty() )
					{
						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM,L"EXEC [dbo].[up_Update_TB_UserCharacter_Field2]");
						kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
						kQuery.QueryOwner(kElement.RetOwnerGuid());
						kQuery.PushStrParam(kElement.RetOwnerGuid());
						kQuery.PushStrParam(kFieldName);
						kQuery.PushStrParam(i64AbilValue);
						g_kCoreCenter.PushQuery(kQuery);
					}
				}	
			}break;
		case DISCT_CREATE_MAIL_BYGUID:
			{
				SSendMailInfo kData;
				BM::GUID kMailGuid;

				kData.ReadFromPacket( kAddonData );	
				kAddonData.Pop(kMailGuid);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_ADD_MAIL,L"EXEC [dbo].[up_Post_Create_UserMail_ByGuid]");
				kQuery.InsertQueryTarget(kMailGuid);
				kQuery.QueryOwner(kMailGuid);

				kQuery.PushStrParam(kMailGuid);
				kQuery.PushStrParam(kData.FromGuid());
				kQuery.PushStrParam(kData.TargetGuid());
				kQuery.PushStrParam(kData.MailTitle());
				kQuery.PushStrParam(kData.MailText());

				kQuery.PushStrParam(kData.ItemGuid());

				kQuery.PushStrParam(kData.Money());
				kQuery.PushStrParam(kData.PaymentType());
				kQuery.PushStrParam(kData.FromName());

				kData.WriteToPacket(kQuery.contUserData);	// log 용 메일 데이터

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_CREATE_MAIL:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), OOT_Player));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}

				SSendMailInfo kData;
				BM::GUID kMailGuid;

				kData.ReadFromPacket( kAddonData );	
				kAddonData.Pop(kMailGuid);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_POST_ADD_MAIL,L"EXEC [dbo].[up_Post_Create_UserMail]");
				kQuery.InsertQueryTarget(inviter->second->GetID());
				kQuery.QueryOwner(inviter->second->GetID());

				kQuery.PushStrParam(kMailGuid);
				kQuery.PushStrParam(kData.FromGuid());
				kQuery.PushStrParam(kData.TargetName());
				kQuery.PushStrParam(kData.MailTitle());
				kQuery.PushStrParam(kData.MailText());

				kQuery.PushStrParam(kData.ItemGuid());

				kQuery.PushStrParam(kData.Money());
				kQuery.PushStrParam(kData.PaymentType());
				kQuery.PushStrParam(kData.FromName());

				kData.WriteToPacket(kQuery.contUserData);	// log 용 메일 데이터

				kContItemModifyQuery.push_back(kQuery);
				kContItemModifyQuery.QueryOwner(kElement.RetOwnerGuid());
			}break;
		case DISCT_MODIFY_MISSION_EVENT:
			{
				int iAbilValue = 0;
				kAddonData.Pop(iAbilValue);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_Extern]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam(kElement.RetOwnerGuid());
				kQuery.PushStrParam(std::wstring(_T("Mission_Event")));
				kQuery.PushStrParam(iAbilValue);
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_ADD_CASH:
			{
				__int64 iAddCash = 0;
				PgTranPointer kTran(kAddonData);
				kAddonData.Pop(iAddCash);
				PgTransaction* pkTran = kTran.GetTran();
				switch (kTran.GetTransactionType())
				{
				case ECASH_TRAN_OPENMARKET_SELLCASH:
				case ECASH_TRAN_OPENMARKET_UNREGCASH:
					//case ECASH_TRAN_OPENMARKET_PAYBACK:
				case ECASH_TRAN_OPENMARKET_BUYCASH:
					{
						PgOpenMarketCashRegTran* pkTran = dynamic_cast<PgOpenMarketCashRegTran*>(kTran.GetTran());
						if (pkTran == NULL)
						{
							BM::vstring kTranLog;
							kTran.Log(kTranLog);
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("dynamic_cast<PgOpenMarketCashRegTran*> failed Tran=") << kTranLog);
							return E_FAIL;
						}
						pkTran->Cost(-iAddCash);
					}break;
				default:
					{
						BM::vstring kTranLog;
						kTran.Log(kTranLog);
						VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Transaction invalid TranType =") << kTranLog);
						return E_FAIL;
					}break;

				}
				kTran.RequestCurerntLimitSell();
			}break;

		case DISCT_MODIFY_SWEETHEART:
			{
				BM::GUID kGuid = BM::GUID::NullData();
				int iValue = 0;
				int iType = 0;

				kAddonData.Pop(iType);
				kAddonData.Pop(kGuid);

				BM::PgPackedTime kValueDate;
				switch( iType )
				{
				case CP_CouplePanaltyLimitDate:
					{
						kAddonData.Pop(kValueDate);

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Couple_Proc_UpdateDateInfo]");
						kQuery.InsertQueryTarget(kGuid);
						kQuery.PushStrParam(kGuid);

						if( BM::GUID::IsNotNull(kGuid) )
						{
							BM::PgPackedTime const kEmptyDate;

							if( !(kEmptyDate == kValueDate) ) { kQuery.PushStrParam( BM::DBTIMESTAMP_EX(kValueDate) ); }
							g_kCoreCenter.PushQuery(kQuery);
						}
					}break;
				case CP_ColorGuid:
					{
						BM::GUID kColorGuid;

						kAddonData.Pop(kColorGuid);

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Couple_Proc_UpdateColorInfo]");
						kQuery.InsertQueryTarget(kGuid);
						kQuery.PushStrParam(kGuid);

						//if( BM::GUID::IsNotNull(kGuid) )
						{
							kQuery.PushStrParam( kColorGuid );
							g_kCoreCenter.PushQuery(kQuery);
						}
					}break;
				default:
					{
						kAddonData.Pop(iValue);

						CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Couple_Proc_UpdateInfo]");
						kQuery.InsertQueryTarget(kGuid);
						kQuery.PushStrParam(kGuid);

						if( BM::GUID::IsNotNull(kGuid) )
						{
							switch(iType)
							{
							case CP_DeleteQuest:
								{
									kQuery.PushStrParam(std::wstring(_T("DeleteQuest")));			
								}break;
							case CP_SweetHeartQuestTime:
								{
									kQuery.PushStrParam(std::wstring(_T("SweetHeartQuestTime")));
								}break;
							case CP_Status:
								{
									kQuery.PushStrParam(std::wstring(_T("Status")));
								}break;
							case CP_LearnSkill:
								{
									kQuery.PushStrParam(std::wstring(_T("LearnSkill")));
								}break;
							default:
								{
									CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_MODIFY_SWEETHEART [") << iType << _T("]") );
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
									return E_FAIL;
								}break;
							}
							kQuery.PushStrParam(iValue);
							g_kCoreCenter.PushQuery(kQuery);
						}
					}break;
				}
			}break;
		case DISCT_JOBSKILL_SET_SKILL_EXPERTNESS:
			{
				CONT_DOC_PLAYER::const_iterator inviter = kContOrgInv.find(SModifyOrderOwner(kElement.RetOwnerGuid(), kElement.OwnerType()));
				if ( inviter == kContOrgInv.end() )
				{
					return E_FAIL;
				}
				// 직업기술 숙련도를 저장하자
				SPMOD_JobSkillExpertness kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP, L"EXEC [dbo].[UP_UserJobSkill_Expertness_Update]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( kData.SkillNo() );
				kQuery.PushStrParam( kData.Expertness() );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_JOBSKILL_DEL_SKILL_EXPERTNESS:
			{
				int iSkillNo = 0;
				kAddonData.Pop( iSkillNo );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP, L"EXEC [dbo].[UP_UserJobSkill_Expertness_Delete]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( iSkillNo );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_JOBSKILL_SAVE_EXHAUSTION:
			{
				int iCurExhaustion = 0, iCurBlessPoint = 0;
				BM::DBTIMESTAMP_EX kLastResetExhaustionTime;
				BM::DBTIMESTAMP_EX kLastResetBlessPointTime;
				kAddonData.Pop( iCurExhaustion );
				kAddonData.Pop( iCurBlessPoint );
				kAddonData.Pop( kLastResetExhaustionTime );
				kAddonData.Pop( kLastResetBlessPointTime );

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_JOBSKILL_INFO_NO_OP, L"EXEC [dbo].[UP_UserJobSkill_Exhaustion_Update]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( iCurExhaustion );
				kQuery.PushStrParam( iCurBlessPoint );
				kQuery.PushStrParam( kLastResetExhaustionTime );
				kQuery.PushStrParam( kLastResetBlessPointTime );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_DAILY_REWARD_UPDATE:
			{
				SPMOD_DailyReward kData;
				kData.ReadFromPacket(kAddonData);

				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_DailyRewardUpdate]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( kData.m_kDailyReward.DayNo() );
				kQuery.PushStrParam( kData.m_kDailyReward.RewardDate() );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_PREMIUM_SERVICE_INSERT:
		case DISCT_PREMIUM_SERVICE_MODIFY:
			{
				int iServiceNo = 0;
				BM::DBTIMESTAMP_EX kStartDate;
				BM::DBTIMESTAMP_EX kEndDate;
				kAddonData.Pop( iServiceNo );
				kAddonData.Pop( kStartDate );
				kAddonData.Pop( kEndDate );

				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_PremiumService_Update]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( iServiceNo );
				kQuery.PushStrParam( kStartDate );
				kQuery.PushStrParam( kEndDate );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_PREMIUM_SERVICE_REMOVE:
			{
				SYSTEMTIME kLocalTime;
				g_kEventView.GetLocalTime(&kLocalTime);

				int iServiceNo = 0;
				BM::DBTIMESTAMP_EX kStartDate = BM::DBTIMESTAMP_EX(kLocalTime);
				BM::DBTIMESTAMP_EX kEndDate = kStartDate;

				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_PREMIUM_SERVICE, L"EXEC [dbo].[UP_PremiumService_Update]");
				kQuery.InsertQueryTarget(kElement.RetOwnerGuid());
				kQuery.PushStrParam( kElement.RetOwnerGuid() );
				kQuery.PushStrParam( iServiceNo );
				kQuery.PushStrParam( kStartDate );
				kQuery.PushStrParam( kEndDate );
				g_kCoreCenter.PushQuery(kQuery);
			}break;
		case DISCT_PREMIUM_ARTICLE_MODIFY:
			{
			}break;
		default:
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
				return E_FAIL;
			}break;
		}

		++itor;
	}

	return ProcessModifyToDBSub( rkPacket, kContItemModifyQuery );
}

void PgRealmUserManager::ProcessModifyItemFailSub(SPMO const & rkOrder, CONT_ITEM_CREATE_ORDER & rkCreateItem, PgContLogMgr & kContLogMgr)
{
	g_kSysInvMgr.ClearTimeOutSysItem(rkOrder.OwnerGuid());

	switch(rkOrder.Cause() & IMET_CMP_BASE)
	{
	case IMET_INSERT_FIXED:
		{
			SPMOD_Insert_Fixed kData;
			rkOrder.Read(kData);
			g_kSysInvMgr.AddSysItem(rkOrder.OwnerGuid(),kData.Item());
			rkCreateItem.push_back(kData.Item());

			{
				PgItemWrapper kCasterWrapper(kData.Item(),SItemPos());
				PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
			}

		}break;
	case IMET_ADD_ANY:
		{
			tagPlayerModifyOrderData_Add_Any kData;
			rkOrder.Read(kData);
			PgBase_Item kItem;
			if(S_OK == CreateSItem(kData.ItemNo(), kData.AddCount(),GIOT_NONE, kItem))
			{
				g_kSysInvMgr.AddSysItem(rkOrder.OwnerGuid(),kItem);
				rkCreateItem.push_back(kItem);

				PgItemWrapper kCasterWrapper(kItem,SItemPos());
				PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
			}
		}break;
	}
}

void PgRealmUserManager::ProcessModifyItemFail_GambleMachine_CashShop(SPMO const & rkOrder, PgContLogMgr & kContLogMgr)
{
	std::wstring wstrFrom, wstrTitle, wstrText;
	GetDefString(SHOP_GAMBLEMACHINE_RESULT_FROM, wstrFrom);
	GetDefString(SHOP_GAMBLEMACHINE_RESULT_TITLENO, wstrTitle);
	GetDefString(SHOP_GAMBLEMACHINE_RESULT_TEXTNO, wstrText);

	switch(rkOrder.Cause() & IMET_CMP_BASE)
	{
	case IMET_INSERT_FIXED:
		{
			SPMOD_Insert_Fixed kData;
			rkOrder.Read(kData);

			g_kPostMgr.PostSystemMailByGuid(rkOrder.OwnerGuid(), wstrFrom, wstrTitle, wstrText, kData.Item(), 0);

			BM::Stream kPacket(PT_M_C_NOTI_ROULETTE_RESULT_MAIL);
			kData.Item().WriteToPacket(kPacket);
			SendToUser(rkOrder.OwnerGuid(),kPacket,false);

			PgItemWrapper kCasterWrapper(kData.Item(),SItemPos());
			PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
		}break;
	case IMET_ADD_ANY:
		{
			tagPlayerModifyOrderData_Add_Any kData;
			rkOrder.Read(kData);

			PgBase_Item kItem;
			if(S_OK == CreateSItem(kData.ItemNo(), kData.AddCount(),GIOT_NONE, kItem))
			{
				g_kPostMgr.PostSystemMailByGuid(rkOrder.OwnerGuid(), wstrFrom, wstrTitle, wstrText, kItem, 0);

				BM::Stream kPacket(PT_M_C_NOTI_ROULETTE_RESULT_MAIL);
				kItem.WriteToPacket(kPacket);
				SendToUser(rkOrder.OwnerGuid(),kPacket,false);

				PgItemWrapper kCasterWrapper(kItem,SItemPos());
				PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
			}
		}break;
	}
}

void PgRealmUserManager::ProcessModifyItemFail_GambleMachine_Mixup(SPMO const & rkOrder, PgContLogMgr & kContLogMgr)
{
	std::wstring wstrFrom, wstrTitle, wstrText;
	GetDefString(SHOP_GAMBLEMACHINE_MIXUP_RESULT_FROM, wstrFrom);
	GetDefString(SHOP_GAMBLEMACHINE_MIXUP_RESULT_TITLENO, wstrTitle);
	GetDefString(SHOP_GAMBLEMACHINE_MIXUP_RESULT_TEXTNO, wstrText);

	switch(rkOrder.Cause() & IMET_CMP_BASE)
	{
	case IMET_INSERT_FIXED:
		{
			SPMOD_Insert_Fixed kData;
			rkOrder.Read(kData);

			g_kPostMgr.PostSystemMailByGuid(rkOrder.OwnerGuid(), wstrFrom, wstrTitle, wstrText, kData.Item(), 0);

			BM::Stream kPacket(PT_M_C_NOTI_ROULETTE_MIXUP_RESULT_MAIL);
			kData.Item().WriteToPacket(kPacket);
			SendToUser(rkOrder.OwnerGuid(),kPacket,false);

			PgItemWrapper kCasterWrapper(kData.Item(),SItemPos());
			PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
		}break;
	case IMET_ADD_ANY:
		{
			tagPlayerModifyOrderData_Add_Any kData;
			rkOrder.Read(kData);

			PgBase_Item kItem;
			if(S_OK == CreateSItem(kData.ItemNo(), kData.AddCount(),GIOT_NONE, kItem))
			{
				g_kPostMgr.PostSystemMailByGuid(rkOrder.OwnerGuid(), wstrFrom, wstrTitle, wstrText, kItem, 0);

				BM::Stream kPacket(PT_M_C_NOTI_ROULETTE_MIXUP_RESULT_MAIL);
				kItem.WriteToPacket(kPacket);
				SendToUser(rkOrder.OwnerGuid(),kPacket,false);

				PgItemWrapper kCasterWrapper(kItem,SItemPos());
				PgItemLogUtil::CreateLog(rkOrder.OwnerGuid(),kContLogMgr,kCasterWrapper);
			}
		}break;
	}
}

HRESULT PgRealmUserManager::ProcessModifyItemFail(EItemModifyParentEventType const kCause, SPMO const & rkOrder, CONT_GEN_SYS_ITEM & rkGenSysItem, CONT_DOC_PLAYER const &kContOrgInv, CONT_FAIL_LOG_MGR & kContFailLogMgr)
{
	auto kRet = rkGenSysItem.insert(std::make_pair(rkOrder.OwnerGuid(),CONT_ITEM_CREATE_ORDER()));
	PgContLogMgr & kContLogMgr = kContFailLogMgr[EFLT_SYS_INV];	// 실패 했을 경우 시스템 인벤토리에 생성되는 정보

	HRESULT hResult = E_FAIL;
	switch(kCause)
	{
	case CIE_OpenChest:
	case CIE_OpenPack:
	case CIE_OpenPack2:
	case CIE_HiddenPack:
	case CIE_Coupon:
	case CIE_Mission:
	case CIE_Mission1:
	case CIE_Mission2:
	case CIE_Mission3:
	case CIE_Mission4:
	case CIE_Mission_GadaCoin:
	case CIE_Mission_Rank:
	case CIE_Mission_DefenceWin:
	case CIE_PVP:
	case CIE_KillCount:
	case CIE_Event:
	case CIE_GodCmd:
	case CIE_Locked_Chest:
	case CIE_Open_Gamble:
	case CIE_GambleMachine:
	case IMEPT_BATTLESQUARE_REWARD:
	case CIE_HiddenReword:
	case CIE_HiddenRewordItem:
	case CIE_Revive_Feather_Login:
	case CIE_Revive_Feather_LevelUp:
	case CIE_Login_Event:
	case CIE_JOBSKILL:
	case CIE_ExtractElement:
	case CIE_Manufacture:
	case CIE_PetProduceItem:
	case CIE_RaceEvent:
	case CIE_Constellation:
	case CIE_EmporiaFunction:
		{
			// 실패시 시스템 인벤처리
			CONT_DOC_PLAYER::const_iterator iter = kContOrgInv.find(rkOrder.ModifyOwner());
			if(iter != kContOrgInv.end())
			{
				BuildLogType(CIE_Sys2Inv, (*iter).second, kContLogMgr);
			}

			ProcessModifyItemFailSub(rkOrder,(*kRet.first).second, kContLogMgr);
			hResult = E_ADDED_SYSTEM_INVEN;
		}break;
	case CIE_GambleMachine_Shop:
		{
			// 실패시 시스템 인벤처리
			CONT_DOC_PLAYER::const_iterator iter = kContOrgInv.find(rkOrder.ModifyOwner());
			if(iter != kContOrgInv.end())
			{
				BuildLogType(CIE_GambleMachine_Shop, (*iter).second, kContLogMgr);
			}

			ProcessModifyItemFail_GambleMachine_CashShop(rkOrder, kContLogMgr);
			hResult = S_OK;
		}break;
	case CIE_GambleMachine_Mixup:
		{
			// 실패시 시스템 인벤처리
			CONT_DOC_PLAYER::const_iterator iter = kContOrgInv.find(rkOrder.ModifyOwner());
			if(iter != kContOrgInv.end())
			{
				BuildLogType(CIE_GambleMachine_Shop, (*iter).second, kContLogMgr);
			}

			ProcessModifyItemFail_GambleMachine_Mixup(rkOrder, kContLogMgr);
			hResult = S_OK;
		}break;
	case CIE_SendEventItemReward:
	case CIE_SendTreasureChestReward:
		{
			PgContLogMgr & kContResultLogMgr = kContFailLogMgr[EFLT_RESULT];

			PgContLogMgr kContTmp;	// 임시로 하나 만들어서 로그를 추출하고 시스템 인벤과 처리 로그에 머지시킴
			// 실패시 시스템 인벤처리
			CONT_DOC_PLAYER::const_iterator iter = kContOrgInv.find(rkOrder.ModifyOwner());
			if(iter != kContOrgInv.end())
			{
				BuildLogType(CIE_Sys2Inv, (*iter).second, kContTmp);
				BuildLogType(CIE_Sys2Inv, (*iter).second, kContLogMgr);
				BuildLogType(kCause, (*iter).second, kContResultLogMgr);
			}

			ProcessModifyItemFailSub(rkOrder,(*kRet.first).second, kContTmp);

			kContLogMgr.MergeLogCont( kContTmp );
			kContResultLogMgr.MergeLogCont( kContTmp );
			hResult = E_ADDED_SYSTEM_INVEN;
		}break;
	}
	return hResult;
}

HRESULT PgRealmUserManager::ProcessModifyToDBSub(BM::Stream & rkPacket, CEL::DB_QUERY_TRAN & kContItemQuery)
{
	if( !rkPacket.RemainSize() )
	{
		kContItemQuery.DBIndex(DT_PLAYER);
		kContItemQuery.QueryType(DQT_UPDATE_USER_ITEM);
		return g_kCoreCenter.PushQuery(kContItemQuery);
	}

	EItemModifyParentEventType kCause;
	rkPacket.Pop(kCause);

	switch(kCause)
	{
	case CIE_Post_Mail_Item_Recv:
		{	
			BM::GUID kOwnerGuid,kMailGuid;
			BYTE bPaymentBit = 0;
			rkPacket.Pop(kOwnerGuid);
			rkPacket.Pop(kMailGuid);
			rkPacket.Pop(bPaymentBit);
			
			CEL::DB_QUERY kQuery;
			kQuery.InsertQueryTarget(kOwnerGuid);
			kQuery.contUserData.Push(kMailGuid);
			kQuery.contUserData.Push(rkPacket);

			if(0 == bPaymentBit)
			{
				kQuery.Set(DT_PLAYER, DQT_POST_GET_MAIL_ITEM,L"EXEC [dbo].[up_Post_RecvAnnex]");
			}
			else
			{
				kQuery.Set(DT_PLAYER, DQT_POST_GET_MAIL_ITEM,L"EXEC [dbo].[up_Post_RecvAnnex_Payment2]");
			}

			kQuery.QueryOwner(kOwnerGuid);

			kQuery.PushStrParam(kMailGuid);
			kQuery.PushStrParam(static_cast<BYTE>(PMMT_GET_ANNEX));
			kQuery.PushStrParam(kOwnerGuid);

			kContItemQuery.push_back(kQuery);

			kContItemQuery.DBIndex(DT_PLAYER);
			kContItemQuery.QueryType(DQT_POST_GET_MAIL_ITEM);
			kContItemQuery.QueryOwner(kOwnerGuid);

			kContItemQuery.contUserData.Push(kMailGuid);
			kContItemQuery.contUserData.Push(rkPacket);

			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_CS_Buy:
		{
			/*
			size_t const iRdPos = rkPacket.RdPos();
			PgTranPointer kTran(rkPacket);
			rkPacket.RdPos(iRdPos);
			PgCashShopBuyTran* pkTran = dynamic_cast<PgCashShopBuyTran*>(kTran.GetTran());
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("dynamic_cast<PgCashShopBuyTran*> failed pointer=") << kTran.GetTran());
				return E_SYSTEM_ERROR;
			}
			if ( !pkTran->PayCash() )
			{
				return E_SYSTEM_ERROR;
			}
			*/
			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_CS_Gift_Recv:
		{
			BM::GUID	kOwnerGuid,
						kGiftGuid;

			rkPacket.Pop(kOwnerGuid);
			rkPacket.Pop(kGiftGuid);

			CEL::DB_QUERY kQueryGift(DT_PLAYER,DQT_CS_RECV_GIFT,L"EXEC [dbo].[UP_CS_DELETEGIFT]");
			kQueryGift.InsertQueryTarget(kGiftGuid);
			kQueryGift.InsertQueryTarget(kOwnerGuid);
			kQueryGift.PushStrParam(kGiftGuid);

			kContItemQuery.DBIndex(DT_PLAYER);
			kContItemQuery.QueryType(DQT_CS_RECV_GIFT);
			kContItemQuery.QueryOwner(kOwnerGuid);

			kContItemQuery.push_back(kQueryGift);

			kContItemQuery.contUserData.Push(kGiftGuid);

			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_CS_Add_TimeLimit:
		{
			/*
			size_t const iRdPos = rkPacket.RdPos();
			PgTranPointer kTran(rkPacket);
			rkPacket.RdPos(iRdPos);
			PgExpandTimeLimitTran* pkTran = dynamic_cast<PgExpandTimeLimitTran*>(kTran.GetTran());
			if (pkTran == NULL)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("dynamic_cast<PgExpandTimeLimitTran*> failed pointer=") << kTran.GetTran());
				return E_SYSTEM_ERROR;
			}

			if ( !pkTran->PayCash() )
			{
				return E_SYSTEM_ERROR;
			}
			*/
			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_UM_Article_Buy:
	case CIE_CS_Gift:
	case CIE_Coupon:
	case CIE_Cash_Pack:
		{
			kContItemQuery.DBIndex(DT_PLAYER);
			kContItemQuery.QueryType(DQT_UPDATE_USER_ITEM);
			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_UM_Article_CashReg:
		{
			kContItemQuery.DBIndex(DT_PLAYER);
			kContItemQuery.QueryType(DQT_UM_ARTICLE_REG);
			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	case CIE_UserQuestComplete:
		{
			BM::GUID kCharGuid;
			int iQuestID = 0;
			rkPacket.Pop(kCharGuid);
			rkPacket.Pop(iQuestID);
			
			CEL::DB_QUERY kQuery;
			kQuery.Set(DT_PLAYER, DQT_UPDATE_USER_QUEST_COMPLETE,L"EXEC [dbo].[up_UserQuestComplete_Update]");
			kQuery.InsertQueryTarget(kCharGuid);
			kQuery.QueryOwner(kCharGuid);
			kQuery.PushStrParam(kCharGuid);
			kQuery.PushStrParam(iQuestID);

			kContItemQuery.push_back(kQuery);

			kContItemQuery.DBIndex(DT_PLAYER);
			kContItemQuery.QueryType(DQT_UPDATE_USER_QUEST_COMPLETE);
			kContItemQuery.QueryOwner(kCharGuid);

			return g_kCoreCenter.PushQuery(kContItemQuery);
		}break;
	default:
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return E_FAIL;
		}break;
	}

	return S_OK;
}

void PgRealmUserManager::ProcessModifyPlayer_LogOut( EItemModifyParentEventType const kCause, SPMO const &kOrder, SRealmGroundKey const &kGndKey )
{
	switch( kOrder.Cause() )
	{
	case IMET_ADD_PVP_RANK_RECORD:
		{
			SPMOD_SAbil kData;
			kOrder.Read(kData);

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_UpdateAdd_TB_UserCharacter_Extern]");
			kQuery.InsertQueryTarget(kOrder.OwnerGuid());
			kQuery.PushStrParam(kOrder.OwnerGuid());

			switch(kData.Type())
			{
			case GAME_DRAW:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Draw"));
				}break;
			case GAME_WIN:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Win"));
				}break;
			case GAME_LOSE:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Lose"));
				}break;
			case GAME_POINT:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Point"));
				}break;
			case GAME_KILL:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Kill"));
				}break;
			case GAME_DEATH:
				{
					kQuery.PushStrParam(std::wstring(L"PvP_Death"));
				}break;
			default:
				{
					CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Unhandled DISCT_MODIFY_SET_PVP_RECORD [") << kData.Type() << _T("]") );
					return;
				}break;
			}

			kQuery.PushStrParam(kData.Value());
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case IMET_ADD_RANK_POINT:
		{
			SPMOD_AddRankPoint kData;
			kOrder.Read(kData);

			const int iValue = kData.Value();

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_UPDATE_USER_ITEM, L"EXEC [dbo].[up_Update_TB_UserCharacter_Point_LogOut]");
			kQuery.InsertQueryTarget( kOrder.OwnerGuid() );
			kQuery.PushStrParam( kOrder.OwnerGuid() );

			switch( kData.Type() )
			{
			case E_RANKPOINT_MAXCOMBO:
				{
					kQuery.PushStrParam( std::wstring( _T("MaxCombo") ) );
				}break;
			default:
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Player Info Cause[") << kCause << _COMMA_ << kOrder.Cause() << _T("] Ground[") << kGndKey.str()
						<<_T("] CharacterGuid[") << kOrder.OwnerGuid() << _T("]") ); 
					return ;
				}break;
			}
			kQuery.PushStrParam(iValue);
			g_kCoreCenter.PushQuery(kQuery);
		}
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Player Info Cause[") << kCause << _COMMA_ << kOrder.Cause() << _T("] Ground[") << kGndKey.str()
				<<_T("] CharacterGuid[") << kOrder.OwnerGuid() << _T("]") ); 
		}break;
	}
}

void PgRealmUserManager::ProcessModify_End(SProcessModifyResult const& kResult, EItemModifyParentEventType const kCause, CONT_PLAYER_MODIFY_ORDER const &kContOrder, SRealmGroundKey const &kGndKey,
		BM::Stream const &kAddonPacket, BM::Stream & rkPacket)
{
	switch (kCause)
	{
	case CIE_CS_Gift:
		{
			// SendGIFT 는 DB Query 한번 더 한 후에 종료 해야 한다. (DQT_CS_ADD_GIFT)
			if (CashResult(kResult) == S_OK)
			{
				return;
			}
		}	// break 없음.
	case CIE_CS_Buy:
	case CIE_CS_Add_TimeLimit:
	case CIE_UM_Article_Buy:
	case CIE_Cash_Pack:
	case CIE_Coupon:
	case CIE_UM_Article_CashBack:
	case CIE_CASH_USE_RCV:
	case CIE_UM_Article_CashReg:
		{
			rkPacket.PosAdjust();
			EItemModifyParentEventType eTempCause;
			rkPacket.Pop(eTempCause);
			PgTranPointer kTran(rkPacket);
			kTran.Finish(CashResult(kResult));
		}break;
	}
}

HRESULT PgRealmUserManager::ProcessUnEquipPet( PgDoc_Player *pkDocPlayer, BM::Stream &kPacket, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr &kContLogMgr )
{
	SItemPos kItemPos;
	BM::GUID kItemGuid;
	int iMP = 0;
	CSkill::MAP_COOLTIME kSkillCoolTimeMap;
	kPacket.Pop(kItemPos);
	kPacket.Pop(kItemGuid);
	kPacket.Pop(iMP);
	PU::TLoadTable_AA( kPacket, kSkillCoolTimeMap );

	PgDoc_PetInfo *pkDocPetInfo = pkDocPlayer->LoadDocPetInfo( kItemGuid, false );
	if ( pkDocPetInfo )
	{
		pkDocPetInfo->SetSkillCoolTimeMap( kSkillCoolTimeMap );
	}

	DB_ITEM_STATE_CHANGE kDBData( DISCT_REMOVE_PET, IMET_PET, pkDocPlayer->GetID() );
	kDBData.kAddonData.Push( kItemGuid );
	kDBData.IsQuery(false);// 주의!!! 반드시 false(true로 하면 펫의 아이템 다 지워 진다!!)
	kChangeArray.push_back( kDBData );
	
	SPMOD_AddAbilPet kAddAbilData( kItemGuid, kItemPos );
	kAddAbilData.kAbilList.push_back( SPMOD_SAbil( AT_MP, static_cast<__int64>(iMP) ) );
	return ProcessModifyPetAbil( pkDocPlayer, kAddAbilData, kChangeArray, kContLogMgr );
}

HRESULT PgRealmUserManager::ProcessModifyPetAbil( PgDoc_Player *pkDocPlayer, SPMOD_AddAbilPet &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray, PgContLogMgr &kContLogMgr )
{
	PgInventory* pkInv = pkDocPlayer->GetInven();

	PgItemWrapper kPetItemWrapper;
	HRESULT hRet = pkInv->PopItem( IMET_PET, kData.PetItemPos(), kPetItemWrapper, false, kChangeArray, kContLogMgr );
	if (	S_OK == hRet
		&&	kData.PetID() == kPetItemWrapper.Guid() )
	{
		PgItem_PetInfo *pkPetInfo = NULL;
		if ( true == kPetItemWrapper.GetExtInfo( pkPetInfo ) )
		{
			bool bIsLvUp = false;
			bool bIsEnchant = false;

			SPMOD_AddAbilPet::CONT_ABILLIST::iterator abil_itr = kData.kAbilList.begin();
			while ( abil_itr != kData.kAbilList.end() )
			{
				__int64 const i64Value = abil_itr->Value();
				bool bRet = false;

				switch ( abil_itr->Type() )
				{
				case AT_EXPERIENCE:
					{
						abil_itr->Value( pkPetInfo->Exp() + i64Value );
						pkPetInfo->Exp( abil_itr->Value() );

						// 레벨업을 할지 체크
						SClassKey kNewClassKey;
						GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
						bIsLvUp = kClassPetDefMgr.IsLvUp( pkPetInfo->ClassKey(), pkPetInfo->Exp(), kNewClassKey );
						if ( true == bIsLvUp )
						{
							pkPetInfo->ClassKey( kNewClassKey );

							// 아이템 인첸트정보(RareOption)이 바껴야 한다.
							PgClassPetDef kPetDef;
							if ( true == kClassPetDefMgr.GetDef( kNewClassKey, &kPetDef ) )
							{
								SEnchantInfo kEnchantInfo = kPetItemWrapper.EnchantInfo();
								if ( true == kPetDef.GetPetItemOption( kEnchantInfo ) )
								{
									if ( kEnchantInfo != kPetItemWrapper.EnchantInfo() )
									{
										kPetItemWrapper.EnchantInfo( kEnchantInfo );
										bIsEnchant = true;
									}
								}
							}

							bRet = true;
							pkDocPlayer->ClearChecker(QCHK_EXPERANCE_PET);
						}
						else
						{
							pkDocPlayer->AddChecker(QCHK_EXPERANCE_PET);
							bRet = pkDocPlayer->IsQuery(QCHK_EXPERANCE_PET);

							if ( !bRet )
							{// Map Server, Client에는 통보를 해주어야 하니까
								DB_ITEM_STATE_CHANGE kDB_NoQuery( DISCT_SETABIL_PET, IMET_PET, pkDocPlayer->GetID() );
								kDB_NoQuery.IsQuery(false);
								kDB_NoQuery.ItemWrapper( kPetItemWrapper );
								kDB_NoQuery.kAddonData.Push( static_cast<size_t>(1) );
								kDB_NoQuery.kAddonData.Push( *abil_itr );
								kChangeArray.push_back(kDB_NoQuery);
							}
							else
							{
								pkDocPlayer->ClearChecker(QCHK_EXPERANCE_PET);
							}
						}
					}break;
				case AT_CLASS:
				case AT_LEVEL:
					{
						SClassKey kNewClassKey = pkPetInfo->ClassKey();
						if ( AT_CLASS == abil_itr->Type() )
						{
							kNewClassKey.iClass = static_cast<int>(i64Value);
						}
						else
						{
							kNewClassKey.nLv = static_cast<short>(i64Value);
						}

						if ( kNewClassKey != pkPetInfo->ClassKey() )
						{
							pkPetInfo->ClassKey( kNewClassKey );

							PgClassPetDef kPetDef;
							GET_DEF( PgClassPetDefMgr, kClassPetDefMgr);
							if ( !kClassPetDefMgr.GetDef( kNewClassKey, &kPetDef ) )
							{
								return E_FAIL;
							}

							// 아이템 인첸트정보(RareOption)이 바껴야 한다.
							SEnchantInfo kEnchantInfo = kPetItemWrapper.EnchantInfo();
							if ( true == kPetDef.GetPetItemOption( kEnchantInfo ) )
							{
								if ( kEnchantInfo != kPetItemWrapper.EnchantInfo() )
								{
									kPetItemWrapper.EnchantInfo( kEnchantInfo );
									bIsEnchant = true;
								}
							}

							bRet = true;
						}
					}break;
				case AT_COLOR_INDEX:
				case AT_MP:
					{
						// 이타입은 SetAbil이다~~
						bRet = pkPetInfo->SetAbil( abil_itr->Type(), static_cast<int>(i64Value) );
					}break;
				case AT_HEALTH:
					{
						pkPetInfo->SetState_Health( static_cast<int>(i64Value) );
						bRet = true;
					}break;
				case AT_MENTAL:
					{
						pkPetInfo->SetState_Mental( static_cast<int>(i64Value) );
						bRet = true;
					}break;
				case AT_MON_SKILL_01:
				case AT_MON_SKILL_02:
				case AT_MON_SKILL_03:
					{
						size_t const iIndex = ( abil_itr->Type() - AT_MON_SKILL_01 );
						PgItem_PetInfo::SStateValue kStateValue;
						kStateValue.Set( static_cast<int>(i64Value) );
						bRet = pkPetInfo->SetSkill( iIndex, kStateValue );
					}break;
				default:
					{
						bRet = pkPetInfo->AddAbil( abil_itr->Type(), static_cast<int>(i64Value) );
						if ( true == bRet ) 
						{
							abil_itr->Value( static_cast<__int64>(pkPetInfo->GetAbil(abil_itr->Type())) );
						}
					}break;
				}

				if ( true == bRet )
				{
					++abil_itr;
				}
				else
				{
					abil_itr = kData.kAbilList.erase( abil_itr );
				}
			}

			if ( kData.kAbilList.size() )
			{	// 이게 사이즈가 없어도 실패처리 하면 안된다.
				// DB부하를 줄이기 위해 같은값 또 저장하면 저장처리를 없애는 경우가 있다.
				DB_ITEM_STATE_CHANGE kDB( DISCT_SETABIL_PET, IMET_PET, pkDocPlayer->GetID() );
				kDB.ItemWrapper( kPetItemWrapper );

				if ( true == bIsLvUp )
				{
					kData.kAbilList.push_back( SPMOD_SAbil( AT_LEVEL, static_cast<__int64>(pkPetInfo->ClassKey().nLv) ) );
				}

				if ( true == bIsEnchant )
				{
					hRet = pkInv->OverWriteItem( IMET_MODIFY_ENCHANT, kPetItemWrapper, kChangeArray, kContLogMgr );
				}
				else
				{
					hRet = pkInv->SwapItem( kData.PetItemPos(), kPetItemWrapper, true );
				}

				if ( S_OK == hRet )
				{					
					kDB.kAddonData.Push( kData.kAbilList );
					kChangeArray.push_back( kDB );
				}
			}
			else
			{
				pkInv->SwapItem( kPetItemWrapper.Pos(), kPetItemWrapper, true );
			}
		}
		else
		{
			hRet = E_FAIL;
		}
	}
	else
	{
		hRet = E_FAIL;
	}

	return hRet;
}

HRESULT PgRealmUserManager::OnProcessDBAddAbil64(PgDoc_Player * pkCopyPlayer, int const iAbilType, __int64 const i64AbilValue, PgContLogMgr & rkContLogMgr, DB_ITEM_STATE_CHANGE_ARRAY & kChangeArray, bool& bIsQuery)
{
	HRESULT hSubRet = E_FAIL;
	switch(iAbilType)
	{
	case AT_EXPERIENCE:
		{
			__int64 i64OldExp = pkCopyPlayer->GetAbil64(AT_EXPERIENCE);
			hSubRet = pkCopyPlayer->AddExp(i64AbilValue, kChangeArray);
			//경험치 로그
			PgLog kExpLog(ELOrderMain_Exp, ELOrderSub_Modify);
			__int64 i64Exp = pkCopyPlayer->GetAbil64(AT_EXPERIENCE);
			kExpLog.Set(0, i64Exp);					// 현재 경험치
			kExpLog.Set(1, i64OldExp);					// 이전 경험치
			kExpLog.Set(2, i64Exp - i64OldExp);		// 경험치 차
			rkContLogMgr.AddLog(pkCopyPlayer->GetID(),kExpLog);
		}break;
	case AT_FRAN:
		{
			__int64 i64OldFran = pkCopyPlayer->GetAbil64(AT_FRAN);
			hSubRet = pkCopyPlayer->AddFran(i64AbilValue);
			// Fran 로그
			PgLog kFranLog(ELOrderMain_Exp, ELOrderSub_Modify);
			__int64 i64Fran = pkCopyPlayer->GetAbil64(AT_FRAN);
			kFranLog.Set(3, i64OldFran);
			kFranLog.Set(4, i64Fran - i64OldFran);
			rkContLogMgr.AddUpdateLog(pkCopyPlayer->GetID(), kFranLog);
		}break;
	case AT_TACTICS_EXP:
		{
			__int64 i64OldExp = pkCopyPlayer->GetAbil64(AT_TACTICS_EXP);
			hSubRet = pkCopyPlayer->AddTacticsExp(i64AbilValue, kChangeArray);
			//용병경험치 로그
			PgLog kExpLog(ELOrderMain_TacticsExp, ELOrderSub_Modify);
			__int64 i64Exp = pkCopyPlayer->GetAbil64(AT_TACTICS_EXP);
			kExpLog.Set(0, i64Exp);					// 현재 경험치
			kExpLog.Set(1, i64OldExp);					// 이전 경험치
			kExpLog.Set(2, i64Exp - i64OldExp);		// 경험치 차
			rkContLogMgr.AddLog(pkCopyPlayer->GetID(),kExpLog);					
		}break;
	default:
		{
			if(pkCopyPlayer->AddAbil64(iAbilType, i64AbilValue))
			{
				hSubRet = S_OK;//AddAbil은 무조건 성공.
			}
		}break;
	}

	bIsQuery = pkCopyPlayer->IsQuery(iAbilType);

	if ( bIsQuery )
	{
		pkCopyPlayer->ClearChecker(iAbilType);
		// LOG
		EOrderMainType eOrderMain = ELOrderMain_None;
		switch(iAbilType)
		{
		case AT_EXPERIENCE:
			{
				eOrderMain = ELOrderMain_Exp;
			}break;
		case AT_FRAN:
			{
				eOrderMain = ELOrderMain_Fran;
			}break;
		}
		if (eOrderMain != ELOrderMain_None)
		{
			PgLogCont kLogCont(ELogMain_User_Character, ELogSub_Character_DBSave, pkCopyPlayer->GetMemberGUID(), pkCopyPlayer->GetID(), pkCopyPlayer->MemberID(),
				pkCopyPlayer->Name(), pkCopyPlayer->GetAbil(AT_CLASS), pkCopyPlayer->GetAbil(AT_LEVEL), 0);
			PgLog kLog2(eOrderMain, ELOrderSub_DBSave);
			kLog2.Set(0, pkCopyPlayer->GetAbil64(iAbilType));
			kLogCont.Add(kLog2);
			kLogCont.Commit();
		}
	}

	return hSubRet;
}


void PgRealmUserManager::ProcessEmporiaReserve(HRESULT const hRet, BM::Stream kAddonPacket, BYTE byErrorType)
{
    int iSize = 0;
    kAddonPacket.Pop(iSize);

    SGuild_Inventory_Log kLog;
    kLog.ReadFromPacket(kAddonPacket);

    if( S_OK == hRet )// 엠포리아 도전
    {
	    BM::Stream kNPacket( PT_N_N_REQ_EMPORIA_BATTLE_RESERVE, false );
	    kNPacket.Push(kAddonPacket);
	    ::SendToRealmContents( PMET_EMPORIA, kNPacket );
    }
    else// 길드 골드가 부족하여 신청할 수 없다.
    {
        BM::GUID kReqCharGuid;
        kAddonPacket.Pop(kReqCharGuid);
        CAUTION_LOG(BM::LOG_LV1, __FL__ << L"[Emporia-ReserveBattle: CharGuid<" << kReqCharGuid << L">] Failed - Not enough money");
	    BM::Stream kAnsPacket( PT_N_C_ANS_GUILD_COMMAND, (BYTE)GC_ReserveBattleForEmporia );
	    kAnsPacket.Push(byErrorType);
        SendToUser(kReqCharGuid,kAnsPacket,false);
    }
}

// 연관 업적 체크
void OnCheckAchievenmentRelation(BM::GUID const& kOwnerGuid, SPMOD_Complete_Achievement const& kData, PgAchievements const* pkAchievements)
{
	if(kData.GroupNo() <= 0 || !pkAchievements)
	{
		return;
	}

	const CONT_DEF_ACHIEVEMENTS_SAVEIDX* pkContDef = NULL;
	g_kTblDataMgr.GetContDef( pkContDef );
	if( !pkContDef )
	{
		return;
	}

	// 1. 해당 GroupNo의 "모두달성" 업적을 찾음
	CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator c_find_iter = pkContDef->begin();
	while( c_find_iter != pkContDef->end() )
	{
		if( c_find_iter->second.iGroupNo == kData.GroupNo()
		&&	c_find_iter->second.iType == AT_ACHIEVEMENT_RELATION_CLAER )
		{
			break; // 이 때 c_find_iter는 연관업적 달성 업적
		}

		++c_find_iter;
	}

	if(c_find_iter == pkContDef->end())
	{
		return;
	}

	if( c_find_iter->second.iSaveIdx == kData.SaveIdx() )	
	{
		return;
	}
	
	// 2. 지금 달성되는 업적의 GroupNo와 같은 업적들의 달성 여부를 체크
	bool bSuccessAchievementRelationClear = true;
	CONT_DEF_ACHIEVEMENTS_SAVEIDX::const_iterator c_loop_iter = pkContDef->begin();
	while( c_loop_iter != pkContDef->end() )
	{
		if( c_loop_iter->first == kData.SaveIdx()							// 해당 업적은 이제 완료 될꺼니까 체크 안함.
		||	c_loop_iter->second.iType == AT_ACHIEVEMENT_RELATION_CLAER )	// 이건 당연히 체크하면 안되고.
		{
			++c_loop_iter;
			continue;
		}

		if( c_loop_iter->second.iGroupNo == kData.GroupNo() )
		{
			if( !pkAchievements->IsComplete( c_loop_iter->first ) )
			{// 하나라도 달성하지 않았으면 쫑
				bSuccessAchievementRelationClear = false;
				break;
			}
		}

		++c_loop_iter;
	}

	// 3. 연관업적 모두 달성하였으므로 "모두달성" 업적을 줌
	if( bSuccessAchievementRelationClear )
	{
		if(SActionOrder * pkActionOrder = PgJobWorker::AllocJob())
		{
			pkActionOrder->InsertTarget( kOwnerGuid );
			pkActionOrder->kCause = CAE_Achievement;
			pkActionOrder->kContOrder.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT, kOwnerGuid,
				SPMOD_Complete_Achievement(c_find_iter->first, c_find_iter->second.iCategory, c_find_iter->second.iRankPoint, c_find_iter->second.iUseTime, c_find_iter->second.iGroupNo)));
			g_kJobDispatcher.VPush(pkActionOrder);
		}
	}
}