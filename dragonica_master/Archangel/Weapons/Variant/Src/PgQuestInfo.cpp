#include "stdafx.h"

#include "BM/Guid.h"
#include "BM/vstring.h"
#include "BM/LocalMgr.h"
#include "Lohengrin/Loggroup.h"
#include "Lohengrin/GameTime.h"
#include "Lohengrin/VariableContainer.h"
#include "DefAbilType.h"
#include "Global.h"
#include "TableDataManager.h"
#include "PgStringUtil.h"
#include "PgControlDefMgr.h"
#include "Item.h"
#include "Effect.h"

#include "PgQuestInfo.h"
#include "PgMyQuest.h"
#include "PgPlayer.h"
#include "Variant/PgJobSkill.h"

namespace RandomQuest
{
	bool IsCanRebuild(PgMyQuest const* pkMyQuest, int const iQuestType)
	{
		if( pkMyQuest )
		{
			switch( iQuestType )
			{
			case QT_Random:
				{
					return (pkMyQuest->RandomQuestCompleteCount() == MAX_QUEST_RANDOM_EXP_COUNT) &&	pkMyQuest->BuildedRandomQuest();
				}break;
			case QT_RandomTactics:
				{
					return (pkMyQuest->TacticsQuestCompleteCount() == MAX_QUEST_RANDOM_EXP_COUNT) && pkMyQuest->BuildedTacticsQuest();
				}break;
			default:
				{
				}break;
			}
		}
		return false;
	}
	bool GetRebuildItem(PgPlayer* pkPlayer, int const iQuestType, int const iUICType, PgBase_Item& rkOutItem, SItemPos& rkOutPos)
	{
		if( !pkPlayer )
		{
			return false;
		}

		PgInventory* pkInven = pkPlayer->GetInven();
		if( !pkInven )
		{
			return false;
		}
		ContHaveItemNoCount kContItemNoCount;
		GET_DEF(CItemDefMgr, kItemDefMgr);
		if( S_OK == pkInven->GetItems(static_cast< EUseItemCustomType >(iUICType), kContItemNoCount, true) )
		{
			ContHaveItemNoCount::const_iterator iter = kContItemNoCount.begin();
			while( kContItemNoCount.end() != iter )
			{
				CItemDef const* pkDef = kItemDefMgr.GetDef((*iter).first);
				if( pkDef )
				{
					if( iQuestType == pkDef->GetAbil(AT_USE_ITEM_CUSTOM_VALUE_1)
					&&	0 < (*iter).second )
					{
						SItemPos kItemPos;
						if( S_OK == pkInven->GetFirstItem((*iter).first, kItemPos) )
						{
							PgBase_Item kItem;
							if( S_OK == pkInven->GetItem(kItemPos, kItem) )
							{
								if( iUICType != UICT_REBUILD_RAND_QUEST // 기간제 찾기가 아니고
								||	(iUICType == UICT_REBUILD_RAND_QUEST && kItem.EnchantInfo().IsTimeLimit()) ) // 기간제 찾기일 땐, 기간 설정 안된 아이템은 못 쓴다
								{
									rkOutPos = kItemPos;
									rkOutItem = kItem;
									return true;
								}
							}
						}
					}
				}
				++iter;
			}
		}
		return false;
	}
	bool IsHasRebuildItem(PgPlayer* pkPlayer, int const iQuestType, int const iUICType)
	{
		PgBase_Item kItem;
		SItemPos kPos;
		return GetRebuildItem(pkPlayer, iQuestType, iUICType, kItem, kPos);
	}
}

namespace EventTaiwanEffectQuest
{
	int const iNotHaveQuestMsg = 700503;
	int const iCantDupRewardEffectMsg = 700504;
	int const iCantDupExpPotionMsg = 700508;
	int const iEventEffectNo = 408201;
	int const iLinkedQuestID[] = {1959, 1960, 1961, 1962, 1963, 1964, 1965};
	int GetEnableCount(PgMyQuest const* pkMyQuest)
	{
		if( !pkMyQuest )
		{
			return 0;
		}

		int iCount = 0;
		int const* pkBegin = iLinkedQuestID;
		while( PgArrayUtil::IsInArray(pkBegin, iLinkedQuestID) )
		{
			if( pkMyQuest->IsEndedQuest( *pkBegin ) )
			{
				++iCount;
			}
			++pkBegin;
		}
		return iCount;
	}

	int GetEnableQuestID(PgMyQuest const* pkMyQuest)
	{
		if( !pkMyQuest )
		{
			return 0;
		}

		int const* pkBegin = iLinkedQuestID;
		while( PgArrayUtil::IsInArray(pkBegin, iLinkedQuestID) )
		{
			if( pkMyQuest->IsEndedQuest( *pkBegin ) )
			{
				return *pkBegin;
			}
			++pkBegin;
		}
		return 0;
	}
};


//
namespace PgQuestInfoUtil
{
	int GetHaveCoupleQuestID(PgMyQuest const* pkMyQuest)
	{
		CONT_DEF_QUEST_REWARD const* pkDefReward = NULL;
		g_kTblDataMgr.GetContDef(pkDefReward);
		
		if( !pkDefReward )
		{
			return 0;
		}

		if( !pkMyQuest )
		{
			return 0;
		}
		
		ContUserQuestState kVec;
		if( !pkMyQuest->GetQuestList(kVec) )
		{
			return 0;
		}

		ContUserQuestState::const_iterator iter = kVec.begin();
		while( kVec.end() != iter )
		{
			int const iQusetID = (*iter).iQuestID;
			switch( GetQuestDefType(iQusetID, pkDefReward) )
			{
			case QT_Couple:
			case QT_SweetHeart:
				{
					return iQusetID;
				}break;
			default:
				{
				}break;
			}

			++iter;
		}
		return 0;
	}

	int GetQuestDefType(int const iQuestID)
	{
		CONT_DEF_QUEST_REWARD const* pkDefReward = NULL;
		g_kTblDataMgr.GetContDef(pkDefReward);
		return GetQuestDefType(iQuestID, pkDefReward);
	}
	int GetQuestDefType(int const iQuestID, CONT_DEF_QUEST_REWARD const* pkDefReward)
	{
		if( pkDefReward )
		{
			CONT_DEF_QUEST_REWARD::const_iterator find_iter = pkDefReward->find( iQuestID );
			if( pkDefReward->end() != find_iter )
			{
				return (*find_iter).second.iDBQuestType;
			}
		}
		return QT_None;
	}

	bool IsDailyUIQuest(EQuestType const eType) // NPC 얼굴을 쓰는 퀘스트가 아닌, 일일 퀘스트들
	{
		switch( eType )
		{
		case QT_Day:
		case QT_GuildTactics:
		case QT_Random:
		case QT_RandomTactics:
		case QT_Wanted:
			{
				return true;
			}break;
		default:
			{
			}break;
		}
		return false;
	}
	bool IsDailyUIQuest(PgQuestInfo const* pkQuestInfo)
	{
		if( pkQuestInfo )
		{
			return IsDailyUIQuest(pkQuestInfo->Type());
		}
		return false;
	}
	bool IsDailySystemQuest(EQuestType const eType)
	{
		switch( eType )
		{
		case QT_GuildTactics:
		case QT_Day:
		case QT_Normal_Day:
		case QT_Event_Normal_Day:
		case QT_Random:
		case QT_RandomTactics:
		case QT_Wanted:
			{
				return true;
			}break;
		default:
			{
			}break;
		}
		return false;
	}
	bool IsDailySystemQuest(PgQuestInfo const* pkQuestInfo)
	{
		if( pkQuestInfo )
		{
			return IsDailySystemQuest(pkQuestInfo->Type());
		}
		return false;
	}
	bool IsWeeklySystemQuest(EQuestType const eType)
	{
		switch( eType )
		{
		case QT_Week:
			{
				return true;
			}break;
		}
		return false;
	}
	bool IsWeeklySystemQuest(PgQuestInfo const* pkQuestInfo)
	{
		if( pkQuestInfo )
		{
			return IsWeeklySystemQuest(pkQuestInfo->Type());
		}
		return false;
	}
	int GetQuestRewardGuildExp(PgQuestInfo const* pkQuestInfo, PgMyQuest const* pkMyQuest, int const iAddCompleteCount)
	{
		if( !pkQuestInfo
		||	!pkMyQuest )
		{
			return 0;
		}

		switch( pkQuestInfo->Type() )
		{
		case QT_RandomTactics:
			{
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP const* pkDefQuestExp = NULL;
				g_kTblDataMgr.GetContDef(pkDefQuestExp);
				if( !pkDefQuestExp )
				{
					return 0;
				}
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP::const_iterator find_iter = pkDefQuestExp->find(pkMyQuest->BuildedTacticsQuestPlayerLevel());
				if( pkDefQuestExp->end() == find_iter )
				{
					return 0;
				}
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP::mapped_type const& rkRandomQuestExp = (*find_iter).second;
				int iIndex = std::min< int >(pkMyQuest->TacticsQuestCompleteCount()+iAddCompleteCount, MAX_QUEST_RANDOM_EXP_COUNT-1);
				iIndex = std::max< int >(0, iIndex);
				return rkRandomQuestExp.aiGuildExp[iIndex];
			}break;
		case QT_Random:
		default:
			{
				// 없음
			}break;
		}
		return pkQuestInfo->m_kReward.iGuildExp;
	}
	int GetQuestRewardExp(PgQuestInfo const* pkQuestInfo, PgMyQuest const* pkMyQuest, int const iAddCompleteCount)
	{
		if( !pkQuestInfo
		||	!pkMyQuest )
		{
			return 0;
		}

		switch( pkQuestInfo->Type() )
		{
		case QT_Random:
			{
				CONT_DEF_QUEST_RANDOM_EXP const* pkDefRandomQuestExp = NULL;
				g_kTblDataMgr.GetContDef(pkDefRandomQuestExp);
				if( !pkDefRandomQuestExp )
				{
					return 0;
				}

				CONT_DEF_QUEST_RANDOM_EXP::const_iterator find_iter = pkDefRandomQuestExp->find(pkMyQuest->BuildedRandomQuestPlayerLevel());
				if( pkDefRandomQuestExp->end() == find_iter )
				{
					return 0;
				}
				CONT_DEF_QUEST_RANDOM_EXP::mapped_type const& rkRandomQuestExp = (*find_iter).second;

				int iIndex = std::min< int >(pkMyQuest->RandomQuestCompleteCount()+iAddCompleteCount, MAX_QUEST_RANDOM_EXP_COUNT-1);
				iIndex = std::max< int >(0, iIndex);
				return rkRandomQuestExp.aiExp[iIndex];
			}break;
		case QT_RandomTactics:
			{
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP const* pkDefQuestExp = NULL;
				g_kTblDataMgr.GetContDef(pkDefQuestExp);
				if( !pkDefQuestExp )
				{
					return 0;
				}
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP::const_iterator find_iter = pkDefQuestExp->find(pkMyQuest->BuildedTacticsQuestPlayerLevel());
				if( pkDefQuestExp->end() == find_iter )
				{
					return 0;
				}
				CONT_DEF_QUEST_RANDOM_TACTICS_EXP::mapped_type const& rkRandomQuestExp = (*find_iter).second;
				int iIndex = std::min< int >(pkMyQuest->TacticsQuestCompleteCount()+iAddCompleteCount, MAX_QUEST_RANDOM_EXP_COUNT-1);
				iIndex = std::max< int >(0, iIndex);
				return rkRandomQuestExp.aiExp[iIndex];
			}break;
		default:
			{
				// 없음
			}break;
		}
		return pkQuestInfo->m_kReward.iExp;
	}

	EQuestBeginLimit CheckBeginQuest(PgPlayer const *pkPlayer, int const iQuestID, PgQuestInfo const* pkQuestInfo)
	{
		if( !pkPlayer || !pkQuestInfo )
		{
			return QBL_Error;
		}

		PgMyQuest const* pkUserQuest = pkPlayer->GetMyQuest();
		if( !pkUserQuest )
		{
			return QBL_Error;
		}

		SUserQuestState const *pkQuest = pkUserQuest->Get(iQuestID);
		if( pkQuest )//이미 수행중인 퀘스트이다
		{
			return QBL_Begined;
		}

		if( pkUserQuest->IsEndedQuest(iQuestID) )
		{
			return QBL_Ended;
		}

		if( !pkQuestInfo->m_kLimit.CheckTime() )
		{
			return QBL_Time;
		}

		bool const bEnableClass = pkQuestInfo->m_kLimit.CheckClass(pkPlayer->GetAbil(AT_CLASS));
		if( !bEnableClass )
		{
			return QBL_Class;
		}

		switch( pkQuestInfo->Type() )
		{
		case QT_Couple:
		case QT_SweetHeart:
			{
				if( BM::GUID::IsNull(pkPlayer->CoupleGuid()) )
				{
					return QBL_Couple;
				}

				if( QT_Couple == pkQuestInfo->Type()
				&&	0 == (pkPlayer->GetCoupleStatus() & CoupleS_Normal) ) // 커플
				{
					return QBL_Couple;
				}
				if( QT_SweetHeart == pkQuestInfo->Type()
				&&	0 == (pkPlayer->GetCoupleStatus() & CoupleS_SweetHeart) ) // 부부
				{
					return QBL_Couple;
				}
			}break;
		case QT_RandomTactics:
		case QT_GuildTactics:
			{
				if( BM::GUID::IsNull(pkPlayer->GuildGuid()) )
				{
					return QBL_Guild;
				}
			}break;
		default:
			{
			}break;
		}

		// 파티설정
		//int const iPartyMemberCount = g_kLocalPartyMgr.GetMemberCount(pkPlayer->PartyGuid());
		//if( iPartyMemberCount < pkQuestInfo->m_kLimit.iMinParty
		//||	iPartyMemberCount > pkQuestInfo->m_kLimit.iMaxParty )
		//{
		//	m_kLimit = QBL_Party;
		//	return false;
		//}

		ContQuestID const& rkNotQuestLimit = pkQuestInfo->m_kLimit_NotQuest;
		ContQuestID::const_iterator notquest_iter = rkNotQuestLimit.begin();
		while(rkNotQuestLimit.end() != notquest_iter)
		{
			ContQuestID::value_type const iNotQuestID = (*notquest_iter);
			bool const bIsIngQuest = pkUserQuest->IsIngQuest(iNotQuestID);
			bool const bIsEndQuest = pkUserQuest->IsEndedQuest(iNotQuestID);
			if( bIsIngQuest//퀘스트를 수행 하고 있거나
			||	bIsEndQuest )//완료 해버렸으면 안된다
			{
				return QBL_Quest;
			}
			++notquest_iter;
		}

		{//OR Quest
			ContQuestID const& rkPreQuestLimit = pkQuestInfo->m_kLimit_PreQuestOr;
			bool bQuestResult = rkPreQuestLimit.empty();
			ContQuestID::const_iterator prequest_iter = rkPreQuestLimit.begin();
			while(rkPreQuestLimit.end() != prequest_iter)//선행퀘스트를 완료 하지 않으면 못받는다.
			{
				ContQuestID::value_type const iPreQuestID = (*prequest_iter);
				bQuestResult = bQuestResult || pkUserQuest->IsEndedQuest(iPreQuestID);
				++prequest_iter;
			}
			if( !rkPreQuestLimit.empty()
			&&	!bQuestResult )
			{
				return QBL_Quest;
			}
		}
		{//AND Quest
			ContQuestID const& rkPreQuestLimit = pkQuestInfo->m_kLimit_PreQuestAnd;
			ContQuestID::const_iterator prequest_iter = rkPreQuestLimit.begin();
			while(rkPreQuestLimit.end() != prequest_iter)//선행퀘스트를 완료 하지 않으면 못받는다.
			{
				ContQuestID::value_type const iPreQuestID = (*prequest_iter);
				bool bEndQuest = pkUserQuest->IsEndedQuest(iPreQuestID);
				if( !bEndQuest )
				{
					return QBL_Quest;
				}
				++prequest_iter;
			}
		}

		ContQuestID const& rkIngQuestLimit = pkQuestInfo->m_kLimit_IngQuest;
		ContQuestID::const_iterator ingquest_iter = rkIngQuestLimit.begin();
		bool bIngQuest = false;
		while(rkIngQuestLimit.end() != ingquest_iter)//진행중인 퀘스트가 하나라도
		{
			ContQuestID::value_type const iIngQuestID = (*ingquest_iter);
			bIngQuest = bIngQuest || pkUserQuest->IsIngQuest(iIngQuestID);
			++ingquest_iter;
		}
		if( !rkIngQuestLimit.empty()
		&&	!bIngQuest )
		{
			return QBL_Quest;
		}

		PgInventory *pkInven = const_cast<PgPlayer*>(pkPlayer)->GetInven();
		if( !pkInven )
		{
			return QBL_Anyway;
		}

		ContQuestLimitItem const& rkAndItem = pkQuestInfo->m_kLimit_ItemHaveAnd;//And Item List
		ContQuestLimitItem::const_iterator anditem_iter = rkAndItem.begin();
		while(rkAndItem.end() != anditem_iter)
		{
			ContQuestLimitItem::value_type const& rkItemLimit = (*anditem_iter);
			size_t const iItemCount = pkInven->GetTotalCount(rkItemLimit.iItemNo);
			if( !PgQuestInfoUtil::CheckQuestItemLimitOperator(rkItemLimit.eOperatorType, rkItemLimit.iCount, iItemCount) )
			{
				return QBL_Item;
			}
			++anditem_iter;
		}

		//Or Item List
		bool bFindItem = false;
		ContQuestLimitItem const& rkOrItem = pkQuestInfo->m_kLimit_ItemHaveOr;//And Item List
		ContQuestLimitItem::const_iterator oritem_iter = rkOrItem.begin();
		while(rkOrItem.end() != oritem_iter)
		{
			ContQuestLimitItem::value_type const& rkItemLimit = (*oritem_iter);
			size_t const iItemCount = pkInven->GetTotalCount(rkItemLimit.iItemNo);
			bFindItem = bFindItem || CheckQuestItemLimitOperator(rkItemLimit.eOperatorType, rkItemLimit.iCount, iItemCount);//퀘스트를 수행할 조건이 된다.
			++oritem_iter;
		}
		if( !bFindItem
		&&	!rkOrItem.empty() )
		{
			return QBL_Item;
		}

		// 펫 퀘스트
		int iPetLevel = 0;
		PgBase_Item kPetItem;
		if( S_OK == pkInven->GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem) )
		{
			PgItem_PetInfo* pkPetExtInfo = NULL;
			if( kPetItem.GetExtInfo(pkPetExtInfo) )
			{
				SClassKey const kOrgClassKey = pkPetExtInfo->ClassKey();
				GET_DEF( PgClassPetDefMgr, kClassPetDefMgr );
				PgClassPetDef kPetDef;
				if( kClassPetDefMgr.GetDef( kOrgClassKey, &kPetDef ) )
				{
					if( EPET_TYPE_2 == kPetDef.GetPetType() || EPET_TYPE_3 == kPetDef.GetPetType() )
					{
						iPetLevel = kOrgClassKey.nLv;
					}
					if( !pkQuestInfo->m_kLimit.CheckPetType(kPetDef.GetPetType()) )
					{
						return QBL_PetType;
					}
					BYTE byGrade = static_cast<BYTE>(kPetDef.GetClassGrade(kOrgClassKey.iClass));
					if( !pkQuestInfo->m_kLimit.CheckPetClassGrade(byGrade))
					{
						return QBL_PetClassGrade;
					}
					if(!pkQuestInfo->m_kLimit.CheckPetCashType(kPetItem))
					{
						return QBL_PetCashType;
					}
				}
			}
		}
		if( !pkQuestInfo->m_kLimit.CheckPetLevel(iPetLevel) )
		{
			return QBL_PetLevel;
		}

		// 마이홈
		if( QET_MYHOME_INVITEUSERCOUNT == pkQuestInfo->m_kDepend_MyHome.iType )
		{
			if( pkPlayer->HomeAddr().IsNull() )
			{
				return QBL_MyHome;
			}
		}

		if( QT_RandomTactics != pkQuestInfo->Type() 
			&&  QT_Random != pkQuestInfo->Type() 
			&&  QT_Wanted != pkQuestInfo->Type())
		{
			// 최종적으로 레벨 체크
			int const iLevel = pkPlayer->GetAbil(AT_LEVEL);
			if( !pkQuestInfo->m_kLimit.CheckLevel(iLevel) )
			{
				int const iMinLevelDiff = pkQuestInfo->m_kLimit.iMinLevel - iLevel;
				if( 1 == iMinLevelDiff )
				{
					return QBL_Level_One;//1렙만 더 업해라
				}
				else
				{
					return QBL_Level;//레벨 안되
				}
			}
		}

		if( pkQuestInfo->m_kLimit.kSkillAndLimit.size() != 0 
			|| pkQuestInfo->m_kLimit.kSkillOrLimit.size() != 0 )
		{
			PgMySkill const*pkMySkill = pkPlayer->GetMySkill();
			if(!pkMySkill)
			{
				return QBL_SkillHave;
			}

			ContHaveSkill::const_iterator itor_andskill = pkQuestInfo->m_kLimit.kSkillAndLimit.begin();
			while( pkQuestInfo->m_kLimit.kSkillAndLimit.end() != itor_andskill)
			{
				if( JobSkillUtil::IsJobSkill( (*itor_andskill) )  )
				{
					if( !JobSkill_LearnUtil::IsEnableUseJobSkill( pkPlayer, (*itor_andskill) ) )
					{
						return QBL_SkillHave;
					}
				}
				else if( !pkMySkill->IsExist( *itor_andskill ) )
				{
					return QBL_SkillHave;
				}
				++itor_andskill;
			}

			bool bHaveSkill = false;
			if( 0 == pkQuestInfo->m_kLimit.kSkillOrLimit.size() )
			{//or조건이 없으면 true
				bHaveSkill = true;
			}
			ContHaveSkill::const_iterator itor_orskill = pkQuestInfo->m_kLimit.kSkillOrLimit.begin();
			while( pkQuestInfo->m_kLimit.kSkillOrLimit.end() != itor_orskill)
			{
				if( JobSkillUtil::IsJobSkill( (*itor_orskill) )  )
				{
					if( JobSkill_LearnUtil::IsEnableUseJobSkill( pkPlayer, (*itor_orskill) ) )
					{
						bHaveSkill = true;
						break;
					}
				}
				else if( pkMySkill->IsExist( *itor_orskill ) )
				{
					bHaveSkill = true;
					break;
				}
				++itor_orskill;
			}
			if( !bHaveSkill )
			{
				return QBL_SkillHave;
			}
		}

		return QBL_None;
	}

	void MakeDayLoopQuestTime(BM::PgPackedTime &rkFailPrevTime, BM::PgPackedTime &rkNextTime)
	{
		// 새벽 0시 0분 0초의 기준 시간을 만들어 준다
		// 현재시간이 (yy년 mm월 dd일, hh시 mm분 ss초)라 가정 하고
		// PrevFailTime은 (yy년 mm월 dd일, 0시 0분 0초) --> 금일의 새벽 0시 0분을 가르킨다.
		// NextTime은 (yy'년 mm'월 dd+1일, 0시 0분 0초) --> 1일 앞의 새벽 0시 0분을 가르킨다.
		BM::PgPackedTime const kLocalTime = BM::PgPackedTime::LocalTime();
		rkFailPrevTime = rkNextTime = kLocalTime;
		CGameTime::AddTime(rkNextTime, CGameTime::OneDay);
		rkFailPrevTime.Hour(0), rkFailPrevTime.Min(0), rkFailPrevTime.Sec(0);
		rkNextTime.Hour(0), rkNextTime.Min(0), rkNextTime.Sec(0);
	}

	void MakeWeekLoopQuestTime(BM::PgPackedTime &rkFailPrevTime, BM::PgPackedTime &rkNextTime)
	{
		BM::PgPackedTime const kLocalTime = BM::PgPackedTime::LocalTime();
		rkFailPrevTime = kLocalTime;

		// Step 1: 현재 년도까지의 총 일 수를 구한다.
		int Days = rkFailPrevTime.Day();

		int Month = rkFailPrevTime.Month() - 1;
		while( 0 < Month )
		{
			Days += GetDaysinMonth(rkFailPrevTime.Year(), Month);
			--Month;
		}

		int const Years = 2000 + rkFailPrevTime.Year() - 1;
		Days += ((Years * 365) + (Years / 4) - (Years / 100) + (Years / 400));

		// Step 2: 오늘의 요일을 계산해서 한주의 구간을 구한다. ( 0 : 월 ~ 6 : 일 )
		int DayofWeek = (Days % 7) - 1;
		if( -1 == DayofWeek )
		{
			DayofWeek = 6;
		}

		// Step 3: 오늘 날짜와 요일을 기준으로 한 주의 구간을 계산한다.
		int NowYear = rkFailPrevTime.Year();
		int NowMonth = rkFailPrevTime.Month();
		int NowDay = rkFailPrevTime.Day();

		if( NowDay > 6 )
		{// 현재 날짜가 7일 이상일 때
			rkFailPrevTime.Day((DWORD)(NowDay - DayofWeek));
		}
		else
		{
			if( NowMonth > 1 )
			{// 이번달이 2월 이상일 때
				rkFailPrevTime.Month((DWORD)(NowMonth - 1));
				int PrevMonthDays = GetDaysinMonth(NowYear, NowMonth - 1);
				PrevMonthDays -= (DayofWeek - NowDay);
				rkFailPrevTime.Day((DWORD)(PrevMonthDays));
			}
			else
			{
				rkFailPrevTime.Year((DWORD)(NowYear - 1));
				rkFailPrevTime.Month(12);
				int PrevMonthDays = 31;
				PrevMonthDays -= (DayofWeek - NowDay);
				rkFailPrevTime.Day((DWORD)(PrevMonthDays));
			}
		}
		
		rkNextTime = rkFailPrevTime;
		CGameTime::AddTime(rkNextTime, CGameTime::OneWeek);
		rkFailPrevTime.Hour(0), rkFailPrevTime.Min(0), rkFailPrevTime.Sec(0);
		rkNextTime.Hour(0), rkNextTime.Min(0), rkNextTime.Sec(0);
	}

	int GetDaysinMonth(int const Year, int const Month)
	{
		int Days = 0;
		bool LeapYear = false;
		if( ((0 == Year % 4) && (0 != Year % 100)) || (0 == Year % 400) )
		{// 윤년계산
			LeapYear = true;
		}

		switch( Month )
		{
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			{
				Days = 31;
			}break;
		case 4: case 6: case 9: case 11:
			{
				Days = 30;
			}break;
		case 2:
			{
				if( LeapYear )
				{
					Days = 29;
				}
				else
				{
					Days = 28;
				}
			}break;
		}
		return Days;
	}

	bool NeedRefreshDayQuest(PgMyQuest const& rkMyQuest, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime)
	{
		return !(rkFailPrevTime < rkMyQuest.DayLoopQuestTime() && rkMyQuest.DayLoopQuestTime() < rkNextTime);
	}

	typedef struct tagPredQuestNpcGuid
	{
		inline explicit tagPredQuestNpcGuid(BM::GUID const& rkNpcGuid)
			:m_kNpcGuid(rkNpcGuid)
		{
		}

		inline tagPredQuestNpcGuid(tagPredQuestNpcGuid const& rhs)
			:m_kNpcGuid(rhs.m_kNpcGuid)
		{
		}

		inline bool operator () (ContQuestNpc::value_type const& rhs) const
		{
			return m_kNpcGuid == rhs.kNpcGuid;
		}
	private:
		BM::GUID const& m_kNpcGuid;
	} SPredQuestNpcGuid;

	inline void DiffQuestNpc(int const iQuestID, ContQuestNpc const& rkLeft, ContQuestNpc const& rkRight, TCHAR const* szLeftName, TCHAR const* szRightName, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
	{
		ContQuestNpc::const_iterator loop_iter = rkLeft.begin();
		while( rkLeft.end() != loop_iter )
		{
			ContQuestNpc::value_type const& rkQuestNpc = (*loop_iter);
			if( rkRight.end() == std::find_if(rkRight.begin(), rkRight.end(), SPredQuestNpcGuid(rkQuestNpc.kNpcGuid)) )
			{
				if( !szLeftName
				||	!szRightName )
				{
					rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("not find [Left NPC Guid:")) << rkQuestNpc.kNpcGuid << _T("] [Right NpcGuid] daily Quest[ID:") << iQuestID << _T("]") );
					PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
				}
				else
				{
					rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("not find [") ) << szLeftName << _T(" NPC Guid:") << rkQuestNpc.kNpcGuid << _T("] [") << szRightName << _T("Right NpcGuid] daily Quest[ID:") << iQuestID << _T("]") );
					PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
				}
			}
			++loop_iter;
		}
	}

	int ConvertIndex(int const iObjectNo)
	{
		return iObjectNo - 1;
	}
	bool VerifyIndexNotZero(int const iObjectNo)
	{
		return -1 != iObjectNo;
	}
	bool VerifyIndex(int const iObjectNo)
	{
		return !CheckIndex(iObjectNo); // 인덱스 밖으로 벚어 나는지
	}
	bool CheckIndex(int const iObjectNo)
	{
		return 0 <= iObjectNo && QUEST_PARAMNUM > iObjectNo; // 인덱스 안에 들어 있는지
	}

	bool IsLoadXmlType(int const iQuestType)
	{
		switch( iQuestType )
		{
		case QT_Deleted:
		case QT_Dummy:
			{
				return false;
			}break;
		default:
			{
			}break;
		}
		return true;
	}
	bool IsCompleteQuestOrder(int const iOrderCause)
	{
		switch( iOrderCause )
		{
		case IMEPT_QUEST_DIALOG_End:
		case IMEPT_QUEST_REMOTE_COMPLETE:
			{
				return true;
			}
		default:
			{
			}break;
		}
		return false;
	}




	//
	PgTextReplacer::PgTextReplacer(std::wstring const& rkOrgText)
		: m_kResult(rkOrgText)
	{
		g_kTblDataMgr.GetContDef(m_pkDefMonster);
		g_kTblDataMgr.GetContDef(m_pkDefItem);
		g_kTblDataMgr.GetContDef(m_pkDefMap);
		g_kTblDataMgr.GetContDef(m_pkDefString);
		g_kTblDataMgr.GetContDef(m_pkDefMissionRoot);
	}

	PgTextReplacer::~PgTextReplacer()
	{
	}

	bool PgTextReplacer::ParseElement(std::wstring const& rkInput, std::wstring &kResult)
	{
		if( rkInput.empty() )
		{
			return false;
		}

		bool bRet = true;
		ContStr kTempVec;
		PgStringUtil::BreakSep(rkInput, kTempVec, _T("="));
		if( 2 != kTempVec.size() )
		{
			kResult = std::wstring( _T("wrong Replacer Argument Count [") ) + rkInput + _T("]");
			AddParsedError( rkInput, kResult );
			return false;
		}

		bool bNeedDelWhiteChar = false;
		int iTextNo = 0;
		std::wstring const& rkHeadName = kTempVec.at(0);
		std::wstring const& rkValue = kTempVec.at(1);

		int const iNo = PgStringUtil::SafeAtoi(rkValue);
		if( rkHeadName == _T("ITEM") )
		{
			CONT_DEFITEM::const_iterator iter = m_pkDefItem->find( iNo );
			if( m_pkDefItem->end() != iter )
			{
				iTextNo = (*iter).second.NameNo;
			}
		}
		else if( rkHeadName == _T("MON") )
		{
			CONT_DEFMONSTER::const_iterator iter = m_pkDefMonster->find( iNo );
			if( m_pkDefMonster->end() != iter )
			{
				iTextNo = (*iter).second.Name;
			}
		}
		else if( rkHeadName == _T("MAP") )
		{
			CONT_DEFMAP::const_iterator iter = m_pkDefMap->find( iNo );
			if( m_pkDefMap->end() != iter )
			{
				iTextNo = (*iter).second.NameNo;
			}
		}
		else if( rkHeadName == _T("MISSION") )
		{
			CONT_DEF_MISSION_ROOT::const_iterator iter = m_pkDefMissionRoot->begin();
			while( m_pkDefMissionRoot->end() != iter )
			{
				CONT_DEF_MISSION_ROOT::mapped_type const& rkMissionRoot = (*iter).second;
				if( rkMissionRoot.iKey == iNo )
				{
					iTextNo = rkMissionRoot.iMissionNo;
				}
				++iter;
			}
		}
		else if( rkHeadName == _T("NPC") )
		{
			// 임시로 NPC ClassNo == DefStringNo 와 같다는 규칙을 이용
			CONT_DEFSTRINGS::const_iterator iter = m_pkDefString->find( iNo );
			if( m_pkDefString->end() != iter )
			{
				iTextNo = iNo;
				bNeedDelWhiteChar = true;
			}
		}
		else if( rkHeadName == _T("DEFSTR") )
		{
			CONT_DEFSTRINGS::const_iterator iter = m_pkDefString->find( iNo );
			if( m_pkDefString->end() != iter )
			{
				iTextNo = iNo;
			}
		}
		else if( rkHeadName == _T("TEXT") )
		{
			ConvertText(iNo, kResult); // 상속 전용
			return true;
		}
		else if( rkHeadName == _T("C") )
		{
			kResult = _T("{") + rkInput + _T("/}"); // 변환 하지 않는다.
			return true;
		}
		else if( rkHeadName == _T("O") )
		{
			kResult = _T("{") + rkInput + _T("/}"); // 변환 하지 않는다.
			return true;
		}
		else if( rkHeadName == _T("T") )
		{
			kResult = _T("{") + rkInput + _T("/}"); // 변환 하지 않는다.
			return true;
		}
		else
		{
			kResult = std::wstring( _T("wrong Replacer Type[") ) + rkHeadName + _T("]");
			AddParsedError( rkInput, kResult );
			return false;
		}

		if( !iTextNo )
		{
			kResult = std::wstring( _T("Can't Find Def[") + rkInput + _T("]") );
			AddParsedError( rkInput, kResult );
			return false;
		}

		std::wstring const* pkDefStr = NULL;
		CONT_DEFSTRINGS::const_iterator find_iter = m_pkDefString->find(iTextNo);
		if( m_pkDefString->end() == find_iter )
		{
			kResult = std::wstring( _T("Can't Find DefStr[") + rkInput + _T("]") );
			AddParsedError( rkInput, kResult );
			return false;
		}

		kResult = (*find_iter).second.strText;
		if( bNeedDelWhiteChar )
		{
			PgStringUtil::EraseStr(kResult, std::wstring(L"\n"), kResult);
			PgStringUtil::EraseStr(kResult, std::wstring(L"\r"), kResult);
		}
		return true;
	}

	void PgTextReplacer::Parse()
	{
		std::wstring kTest = m_kResult;
		std::wstring const kStartHead(_T("{"));
		std::wstring const kEndHead(_T("}"));
		size_t iPos = kTest.find(kStartHead);
		while( iPos != std::wstring::npos )
		{
			size_t iEndPos = kTest.find(kEndHead, iPos);
			if( iEndPos != std::wstring::npos )
			{
				++iEndPos; // EndPos는 '{' 의 바로 앞 위치, 그래서 +1

				std::wstring kReplacer = kTest.substr(iPos, iEndPos - iPos); // {} 사이 단어를 추출
				kReplacer = kReplacer.substr( 1, kReplacer.size() - 2 ); // 앞뒤에 { }를 제거

				ContStr kVec;
				PgStringUtil::BreakSep(kReplacer, kVec, _T("/")); // '/' 문자로 여러개를 쓸수 있다
				kReplacer.clear();

				ContStr::iterator iter = kVec.begin();
				while( kVec.end() != iter )
				{
					ContStr::value_type const& rkStr = (*iter);
					std::wstring kRet;
					ParseElement(rkStr, kRet);

					kReplacer += kRet;
					++iter;
				}

				kTest = kTest.substr(0, iPos) + kReplacer + kTest.substr(iEndPos); // '{'앞 + 치환결과 + '}'뒤
				iPos += kReplacer.size();
			}
			else
			{
				AddParsedError(kTest, std::wstring(L"is not terminated '}'"));
				break;
			}

			iPos = kTest.find(kStartHead, iPos); // 중간부터 검사
		}

		m_kResult = kTest; // 결과 저장
	}

	bool CheckQuestItemLimitOperator(EQuestItemLimitOperator const eType, size_t const iInfoValue, size_t const iCurCount)
	{
		switch( eType ) // 설명과 반대로 구현 해야 한다, 실행 할 수 없는 조건이니깐
		{
		case QILO_LESSEQUAL:
			{
				if( iInfoValue < iCurCount )
				{
					return false;
				}
			}break;
		case QILO_EQUAL:
			{
				if( iInfoValue != iCurCount )
				{
					return false;
				}
			}break;
		case QILO_GREATEQUAL:
			{
				if( iInfoValue > iCurCount )
				{
					return false;
				}
			}break;
		}
		return true;
	}

	int FindEventNo(ContQuestNpc const& rkCont, BM::GUID const& rkGuid)
	{
		ContQuestNpc::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			ContQuestNpc::value_type const& rkVal = (*iter);
			if( rkGuid == rkVal.kNpcGuid )
			{
				return rkVal.iEventNo;
			}
			++iter;
		}
		return 0;
	}

	// for Realm quest
	tagVerifyRealmQuestInfo::tagVerifyRealmQuestInfo()
		: kRealmQuestID(), iQuestID(0)
	{
	}
	tagVerifyRealmQuestInfo::tagVerifyRealmQuestInfo(std::wstring const& rkRealmQuestID, int const& riQuestID)
		: kRealmQuestID(rkRealmQuestID), iQuestID(riQuestID)
	{
	}
	tagVerifyRealmQuestInfo::tagVerifyRealmQuestInfo(tagVerifyRealmQuestInfo const& rhs)
		: kRealmQuestID(rhs.kRealmQuestID), iQuestID(rhs.iQuestID)
	{
	}
	tagVerifyRealmQuestInfo::~tagVerifyRealmQuestInfo()
	{
	}
	bool tagVerifyRealmQuestInfo::operator ==(std::wstring const& rhs) const
	{
		return kRealmQuestID == rhs;
	}
	CONT_REALM_QUEST_ID kContRealmQuestID;
	void GetVerifyRealmQuestID(CONT_REALM_QUEST_ID& rkOut)
	{
		rkOut.swap(kContRealmQuestID);
	}
	void AddVerifyRealmQuestID(int const iQuestID, std::wstring const& rkRealmQuestID)
	{
		kContRealmQuestID.push_back( CONT_REALM_QUEST_ID::value_type(rkRealmQuestID, iQuestID) );
	}

	void MakeOrderQuestDependAbilType(CUnit const* pkCaster, tagQuestDependAbil const& rkAbilEvent, CONT_PLAYER_MODIFY_ORDER& rkContOutOrder)
	{
		if( pkCaster )
		{
			switch( rkAbilEvent.iAbilType )
			{
			case AT_CP:
				{
					rkContOutOrder.push_back( SPMO(IMET_ADD_CP, pkCaster->GetID(), SPMOD_Add_CP(-rkAbilEvent.iAbilValue)) );//필요 CP 빼기.
				}break;
			}
		}
	}

	typedef std::set< EAbilType > CONT_QUEST_DEPEND_ACHIEVEMENTS_ABIL;
	CONT_QUEST_DEPEND_ACHIEVEMENTS_ABIL kContQuestDependAchievementsAbil;

	bool IsNfyQuestDependAbilType(int const iAbilType)
	{
		switch( iAbilType )
		{
		case AT_CP:
		case AT_C_STR:
		case AT_C_INT:
		case AT_C_CON:
		case AT_C_DEX:
		case AT_C_MAX_HP:
		case AT_C_MAX_MP:
			{
			}break;
		default:
			{
				if( kContQuestDependAchievementsAbil.end() != kContQuestDependAchievementsAbil.find( static_cast< EAbilType >(iAbilType) ) )
				{
					return true; // 업적
				}
				return false;
			}break;
		}
		return true;
	}

	bool IsQuestDependAbilType(int const iAbilType)
	{
		if( IsNfyQuestDependAbilType(iAbilType) )
		{
			return true;
		}
		switch( iAbilType )
		{
		case AT_LEVEL:
			{
			}break;
		default:
			{
				return false;
			}break;
		}
		return true;
	}
}



//
namespace PgQuestInfoVerifyUtil
{
#define __ToText( _T )		L#_T

	std::wstring GetText(EErrorType const eType)
	{
		switch(eType)
		{
		case ET_None:			{ return std::wstring( __ToText(ET_None) ); }break;
		case ET_ParseError:		{ return std::wstring( __ToText(ET_ParseError) ); }break;
		case ET_DataError:		{ return std::wstring( __ToText(ET_DataError) ); }break;
		case ET_LogicalError:	{ return std::wstring( __ToText(ET_LogicalError) ); }break;
		default:
			{
			}break;
		}
		return std::wstring( _T("Unknown") );
	}

	//
	SQuestInfoError::SQuestInfoError(PgQuestInfo const* pkQuestInfo)
		: eType(ET_None), kFilePathName(pkQuestInfo->XmlFileName()), iQuestID(pkQuestInfo->ID()), szFunc(0), iLine(0), kErrorMsg()
	{
	}

	SQuestInfoError::SQuestInfoError(SQuestInfoError const& rhs)
		: eType(rhs.eType), kFilePathName(rhs.kFilePathName), iQuestID(rhs.iQuestID), szFunc(rhs.szFunc), iLine(rhs.iLine), kErrorMsg(rhs.kErrorMsg)
	{
	}

	SQuestInfoError::SQuestInfoError(PgQuestInfo const* pkQuestInfo, EErrorType const Type , TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg)
		: eType(Type), kFilePathName(pkQuestInfo->XmlFileName()), iQuestID(pkQuestInfo->ID()), szFunc(Func), iLine(Line), kErrorMsg(ErrorMsg)
	{
	}

	SQuestInfoError::SQuestInfoError(EErrorType const Type ,std::string const FilePathName, int const QuestID, TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg)
		: eType(Type), kFilePathName(FilePathName), iQuestID(QuestID), szFunc(Func), iLine(Line), kErrorMsg(ErrorMsg)
	{
	}

	void SQuestInfoError::Set(EErrorType const Type ,TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg)
	{
		eType = Type;
		szFunc = Func;
		iLine = Line;
		kErrorMsg = ErrorMsg;
	}


	//
	LPOUTPUTERRORFUNC g_lpAddErrorFunc = NULL; // Default Set
	LPOUTPUTERRORFUNC g_lpOutputErrorFunc = NULL; // Default Set
	Loki::Mutex m_kMutex;
	ContErrorMsg g_kErrorMsg;

	void CALLBACK AddError(SQuestInfoError const& rkNewError)
	{
		if( g_lpAddErrorFunc )
		{
			g_lpAddErrorFunc(rkNewError);
		}
		else
		{
			BM::CAutoMutex kLock(m_kMutex);
			g_kErrorMsg.push_back( rkNewError );
		}
	}

	void CALLBACK OutputError(SQuestInfoError const& rkError)
	{
		if( g_lpOutputErrorFunc )
		{
			g_lpOutputErrorFunc(rkError);
		}
		else
		{
			// 함수 위치, 라인 표시
			//CAUTION_LOG(BM::LOG_LV1, PgQuestInfoVerifyUtil::GetText(rkError.eType) << _T("-F[") << rkError.kFilePathName << _T("]-[Q") << rkError.iQuestID << _T("]") << __FL2__(rkError.szFunc, rkError.iLine) << _T(" ") << rkError.kErrorMsg);

			// 함수 위치, 라인 생략
			CAUTION_LOG(BM::LOG_LV1, L"["<<PgQuestInfoVerifyUtil::GetText(rkError.eType) << _T("]-F[") << rkError.kFilePathName << _T("]-[Q") << rkError.iQuestID << _T("] ") << rkError.kErrorMsg);
		}
	}

	bool VerifyEventArgumentCount(PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, size_t const iRightCount, size_t const iCurCount, char const* szElementName, std::wstring const& rkType, std::wstring const& rkValue, wchar_t const* szDesc)
	{
		if( iRightCount != iCurCount )
		{
			rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong Argument Count [TYPE='")<<rkType<<L"' VALUE='"<<rkValue<<L"'], Example) <"<<szElementName<<L" TYPE='"<<rkType<<L"' VALUE='"<<szDesc<<L"'> ... </"<<szElementName<<L">" );
			PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
			return false;
		}
		return true;
	}


	//
	class PgQuestInfoVerifier
	{
		typedef std::set< int > ContDialogSet; // 번외로 사용되는 DialogID
	public:
		PgQuestInfoVerifier(PgQuestInfo const* pkQuestInfo, ContQuestInfo const& rkXmlInfoMap, int const iLimitKillCount)
			: m_pkQuestInfo(pkQuestInfo), m_kXmlInfoMap(rkXmlInfoMap), m_iLimitKillCount(iLimitKillCount)
		{
			g_kTblDataMgr.GetContDef(m_pkDefItem);
			g_kTblDataMgr.GetContDef(m_pkDefMap);
			g_kTblDataMgr.GetContDef(m_pkDefMonster);
			g_kTblDataMgr.GetContDef(m_pkDefItemRarityControl);
			g_kTblDataMgr.GetContDef(m_pkDefEffect);
			g_kTblDataMgr.GetContDef(m_pkDefClass);
			g_kTblDataMgr.GetContDef(m_pkDefMissionRoot);
			g_kTblDataMgr.GetContDef(m_pkDefQuestReward);
			g_kTblDataMgr.GetContDef(m_pkDefObjects);
			g_kTblDataMgr.GetContDef(m_pkDefAchievements);
			//CONT_DEF_MONSTER_BAG const* pkDefmonster;
			//CONT_DEF_MONSTER_BAG_ELEMENTS const* pkDefMonsterEle;


			//
			{
				ContQuestDialog::const_iterator iter = Dialog().begin();
				while( Dialog().end() != iter )
				{
					ContQuestDialog::mapped_type const& rkDialog = (*iter).second;
					ContQuestSelect::const_iterator sel_iter = rkDialog.kSelectList.begin();
					while( rkDialog.kSelectList.end() != sel_iter )
					{
						ContQuestSelect::value_type const& rkSelect = (*sel_iter);
						int const iSelectID = GetNextDialogID(rkSelect.iSelectID);
						m_kRefDialog.insert( iSelectID );

						++sel_iter;
					}
					++iter;
				}
			}
		}
		~PgQuestInfoVerifier()
		{
		}

		void Verify()
		{
			{
				CheckQuestID( GetR().m_kLimit_PreQuestAnd, BM::vstring(_T("LIMIT And EndQuest")) );
				CheckQuestID( GetR().m_kLimit_PreQuestOr, BM::vstring(_T("LIMIT Or EndQuest")) );
				CheckQuestID( GetR().m_kLimit_IngQuest, BM::vstring(_T("LIMIT Ing Quest")) );
				CheckQuestID( GetR().m_kLimit_NotQuest, BM::vstring(_T("LIMIT Not Quest")) );
				CheckSQuestLimitItem( GetR().m_kLimit_ItemHaveAnd, BM::vstring(_T("LIMIT And Item")) );
				CheckSQuestLimitItem( GetR().m_kLimit_ItemHaveOr, BM::vstring(_T("LIMIT Or Item")) );
			}
			{
				ContLocationID::const_iterator iter = GetR().m_kLimit_Location.begin();
				while( GetR().m_kLimit_Location.end() != iter )
				{
					if( 0 == (*iter) )
					{
						AddError( SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't LOCATION is 0"))) );
					}
					++iter;
				}
			}

			{
				ContQuestDependItem::const_iterator iter = Item().begin();
				while( Item().end() != iter )
				{
					VerifyItemEvent( (*iter).second );
					++iter;
				}
			}
			{
				ContQuestDependClass::const_iterator iter = Class().begin();
				while( Class().end() != iter )
				{
					VerifyClassEvent( (*iter) );
					++iter;
				}
			}
			{
				ContQuestDependAbil::const_iterator iter = Abil().begin();
				while( Abil().end() != iter )
				{
					VerifyAbilEvent( (*iter) );
					++iter;
				}
			}
			{
				ContQuestDependGlobal::const_iterator iter = Global().begin();
				while( Global().end() != iter )
				{
					VerifyGlobalEvent( (*iter) );
					++iter;
				}
			}
			{
				ContQuestDependNpc::const_iterator iter = Npc().begin();
				while( Npc().end() != iter )
				{
					VerifyNpcEvent( (*iter).second );
					++iter;
				}
			}
			{
				ContQuestDependMission::const_iterator iter = Mission().begin();
				while( Mission().end() != iter )
				{
					VerifyMissionEvent( (*iter) );
					++iter;
				}
			}
			{
				ContQuestMonster::const_iterator iter = Monster().begin();
				while( Monster().end() != iter )
				{
					VerifyMonsterEvent( (*iter).second );
					++iter;
				}
			}
			{
				ContQuestGround::const_iterator iter = Ground().begin();
				while( Ground().end() != iter )
				{
					VerifyGroundEvent( (*iter).second );
					++iter;
				}
			}
			{
				ContQuestDependPvp::const_iterator iter = GetR().m_kDepend_Pvp.begin();
				while( GetR().m_kDepend_Pvp.end() != iter )
				{
					SQuestDependPvp const& rkPvpEvent = (*iter);
					BM::vstring const kTempHeader = BM::vstring(_T("EVENT PVP Type[")) << rkPvpEvent.iType << _T("]");
					CheckObjectNo(rkPvpEvent.iObjectNo, kTempHeader);

					++iter;
				}
			}
			{
				SQuestDependCouple const& rkCoupleEvent = GetR().m_kDepend_Couple;
				if( rkCoupleEvent.iType )
				{
					BM::vstring const kTempHeader = BM::vstring(_T("EVENT Couple Type[")) << rkCoupleEvent.iType << _T("]");
					CheckObjectNo(rkCoupleEvent.iObjectNo, kTempHeader);
				}
			}
			{
				int const iCurKillCount = GetR().m_kDepend_KillCount.iKillCount;
				if( m_iLimitKillCount < iCurKillCount )
				{
					AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring( _T(", EVENT KillCount = ") )<<iCurKillCount<<L" is Wrong, 0 < x <"<<m_iLimitKillCount));
				}
			}

			{
				typedef std::set< int > ContClass;
				typedef std::map< int, int > ContClassCount;

				ContClass kClasses;
				{
					CONT_DEFCLASS::const_iterator iter = m_pkDefClass->begin();
					while( m_pkDefClass->end() != iter )
					{
						kClasses.insert( (*iter).second.iClass );
						++iter;
					}
				}
				
				{
					ContClassCount kMale, kFemale;
					ContRewardVec::const_iterator reward1_iter = Reward().kClassRewardSet1.begin();
					while( Reward().kClassRewardSet1.end() != reward1_iter )
					{
						ContRewardVec::value_type const& rkReward = (*reward1_iter);
						CheckReward( rkReward, 1 );

						ContClass::const_iterator class_iter = kClasses.begin();
						while( kClasses.end() != class_iter )
						{
							int const iClassNo = (*class_iter);
							if( rkReward.IsCanClass(UG_MALE, iClassNo) )
							{
								auto kRet = kMale.insert( std::make_pair(iClassNo, 1) );
								if( !kRet.second )
								{
									AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward1 is have the Duplicate [Male] Class No[")<<iClassNo<<L"]"));
								}
							}
							if( rkReward.IsCanClass(UG_FEMALE, iClassNo) )
							{
								auto kRet = kFemale.insert( std::make_pair(iClassNo, 1) );
								if( !kRet.second )
								{
									AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward1 is have the Duplicate [Female] Class No[")<<iClassNo<<L"]"));
								}
							}
							++class_iter;
						}
						++reward1_iter;
					}

					if( kMale.size() != kFemale.size() )
					{
						AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward1 is have Unmatched reward count [Male:")<<kMale.size()<<L"] [Female:"<<kFemale.size()<<L"]"));
					}
				}
				{
					ContClassCount kMale, kFemale;
					ContRewardVec::const_iterator reward2_iter = Reward().kClassRewardSet2.begin();
					while( Reward().kClassRewardSet2.end() != reward2_iter )
					{
						ContRewardVec::value_type const& rkReward = (*reward2_iter);
						CheckReward( rkReward, 2 );

						ContClass::const_iterator class_iter = kClasses.begin();
						while( kClasses.end() != class_iter )
						{
							int const iClassNo = (*class_iter);
							if( rkReward.IsCanClass(UG_MALE, iClassNo) )
							{
								auto kRet = kMale.insert( std::make_pair(iClassNo, 1) );
								if( !kRet.second )
								{
									AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward2 is have the Duplicate [Male] Class No[")<<iClassNo<<L"]"));
								}
							}
							if( rkReward.IsCanClass(UG_FEMALE, iClassNo) )
							{
								auto kRet = kFemale.insert( std::make_pair(iClassNo, 1) );
								if( !kRet.second )
								{
									AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward2 is have the Duplicate [Female] Class No[")<<iClassNo<<L"]"));
								}
							}
							++class_iter;
						}
						++reward2_iter;
					}

					if( kMale.size() != kFemale.size() )
					{
						AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L", Reward1 is have Unmatched reward count [Male:")<<kMale.size()<<L"] [Female:"<<kFemale.size()<<L"]"));
					}
				}
			}

			{
				ContQuestDialog::const_iterator iter = Dialog().begin();
				while( Dialog().end() != iter )
				{
					VerifyDialog( (*iter).second );
					++iter;
				}
			}

			if( 0 != GetR().CompleteNextQuestID()
			&&	-1 != GetR().CompleteEventScriptID() )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Error: Quest [ID: ")) << ID() << _T("] Can't have two complete event type, COMPLETE_EVENT_SCRIPT_ID[") << GetR().CompleteEventScriptID() << _T(" and COMPLETE_NEXT_QUEST_ID[") << GetR().CompleteNextQuestID() << _T("]") ));
			}

			{// Reward Effect
				BM::vstring const kTempHeader = BM::vstring(L"Quest [ID: ") << ID() << L"]";
				ContRewardEffectVec::const_iterator effect_iter = Reward().kEffect.begin();
				while( Reward().kEffect.end() != effect_iter )
				{
					CheckEffect( (*effect_iter), kTempHeader);
					++effect_iter;
				}
			}
			// Reward RealmQuestID
			if( !GetR().m_kReward.kRealmQuetID.empty() )
			{
				PgQuestInfoUtil::AddVerifyRealmQuestID(GetR().ID(), GetR().m_kReward.kRealmQuetID);
			}

			switch( Type() )
			{
			case QT_Normal:
			case QT_Event_Normal:
			case QT_Scenario:
			case QT_ClassChange:
			case QT_Loop:
			case QT_Event_Loop:
			case QT_Scroll:
			case QT_MissionQuest:
			case QT_Dummy:
			case QT_Normal_Day:
			case QT_Event_Normal_Day:
			case QT_Couple:
			case QT_SweetHeart:
			case QT_Soul:
			case QT_Week:
			case QT_BattlePass:
				{
				}break;
			case QT_GuildTactics:
			case QT_Day:
			case QT_Random:
			case QT_RandomTactics:
			case QT_Wanted:
				{
					SQuestDialog const* pkInfoDialog = NULL;
					if( !GetR().GetInfoDialog(pkInfoDialog) )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't Find Info dialog, Quest[ID:")) << ID() << _T("]") ));
					}
					//if( 1 < m_kDialog.size() )
					//{
					//	CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Have not Other dialog at daily quest, Quest[ID:") << ID() << _T("]"));
					//}

					//if( !m_kNpc_Agent.empty() )
					//{
					//	CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Can't have Agnet NPC, daily Quest[ID:") << ID() << _T("]"));
					//}
					if( ClientNPC().size() != PayerNPC().size() )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("not same NPC count Client[")) << GetR().m_kNpc_Client.size() << _T("] != Payer[") << GetR().m_kNpc_Payer.size() << _T("], daily Quest[ID:") << ID() << _T("]") ));
					}
					{
						PgQuestInfoUtil::DiffQuestNpc(ID(), ClientNPC(), PayerNPC(), _T("Client"), _T("Payer"), PgQuestInfoVerifyUtil::SQuestInfoError(GetP()));
						PgQuestInfoUtil::DiffQuestNpc(ID(), PayerNPC(), ClientNPC(), _T("Payer"), _T("Client"), PgQuestInfoVerifyUtil::SQuestInfoError(GetP()));
					}
					if( Limit().iTacticsLevel )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't have [Tactics Level], daily Quest[ID:")) << ID() << _T("]") ));
					}
					if( 0 < Reward().SizeRewardSet2() )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't have Reward item set 2, daily Quest[ID:")) << ID() << _T("]") ));
					}
				}break;
			case QT_Deleted:
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("reserved delete quest xml [ID:")) << Type() << _T("]") ));
				}break;
			default:
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Unknown quest type [ID:")) << Type() << _T("]") ));
				}break;
			}
		}

	private:
		PgQuestInfo const& GetR() const
		{
			return *m_pkQuestInfo;
		}
		PgQuestInfo const* GetP() const
		{
			return m_pkQuestInfo;
		}

		int ID() const								{ return GetR().ID(); }
		int Type() const							{ return GetR().Type(); }
		ContQuestMonster const& Monster()			{ return GetR().m_kDepend_Monster; }
		ContQuestDependItem const& Item()			{ return GetR().m_kDepend_Item; }
		ContQuestDependMission const& Mission()		{ return GetR().m_kDepend_Mission; }
		ContQuestDependClass const& Class()			{ return GetR().m_kDepend_Class; }
		ContQuestDependAbil const& Abil()			{ return GetR().m_kDepend_Abil; }
		ContQuestDependNpc const& Npc()				{ return GetR().m_kDepend_Npc; }
		ContQuestDependGlobal const& Global()		{ return GetR().m_kDepend_Global; }
		ContQuestGround const& Ground()				{ return GetR().m_kDepend_Ground; }
		SQuestReward const& Reward()				{ return GetR().m_kReward; }
		SQuestLimitInfo const& Limit()				{ return GetR().m_kLimit; }
		ContQuestNpc const& ClientNPC()				{ return GetR().m_kNpc_Client; }
		ContQuestNpc const& AgentNPC()				{ return GetR().m_kNpc_Agent; }
		ContQuestNpc const& PayerNPC()				{ return GetR().m_kNpc_Payer; }
		ContQuestDialog const& Dialog()				{ return GetR().m_kDialog; }

		int GetNextDialogID(int iSelectID)
		{
			if( QRDID_ACCEPT_Start <= iSelectID
			&&	QRDID_ACCEPT_End >= iSelectID )
			{
				iSelectID = iSelectID - QRDID_ACCEPT_Start;
			}
			else if( QRDID_REJECT_Start <= iSelectID
			&&		QRDID_REJECT_End >= iSelectID )
			{
				iSelectID = iSelectID - QRDID_REJECT_Start;
			}
			else if( QRDID_COMPLETE_Start <= iSelectID
			&&		QRDID_COMPLETE_End >= iSelectID )
			{
				iSelectID = iSelectID - QRDID_COMPLETE_Start;
			}
			else if( QRDID_REJECTCOMPLETE_Start <= iSelectID
			&&		QRDID_REJECTCOMPLETE_End >= iSelectID )
			{
				iSelectID = iSelectID - QRDID_REJECTCOMPLETE_Start;
			}
			else
			{
			}
			return iSelectID;
		}
		void CheckQuestID(ContQuestID const& rkVec, BM::vstring const& rkHeaderMsg)
		{
			ContQuestID::const_iterator iter = rkVec.begin();
			while( rkVec.end() != iter )
			{
				CheckQuestID( (*iter), BM::vstring(rkHeaderMsg) << _T("[") << (*iter) << _T("]") );
				++iter;
			}
		}
		void CheckQuestID(int const iQuestID, BM::vstring const& rkHeaderMsg)
		{
			if( m_pkDefQuestReward->end() == m_pkDefQuestReward->find(iQuestID) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is QuestID["<<iQuestID<<L"] Can't find from TB_DefQuestReward")));
			}
			if( 0 > iQuestID )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is QuestID["<<iQuestID<<L"] Can't small than the 0")));
			}
			int const iLimitQuestID = MAX_DB_ENDQUEST_ALL_SIZE * 8;
			if( iLimitQuestID <= iQuestID )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is QuestID["<<iQuestID<<L"], Can't equal or over than the (1 <= QuestID <")<<iLimitQuestID<<L")"));
			}
		}
		void CheckItem(int const iItemNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iItemNo )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is ItemNo Can't a Zero(0)")));
			}
			if( m_pkDefItem->end() == m_pkDefItem->find(iItemNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is ItemNo["<<iItemNo<<L"] Can't find from TB_DefItem")));
			}
		}
		void CheckGround(int const iGroundNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iGroundNo )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is GroundNo Can't a Zero(0)")));
			}
			if( m_pkDefMap->end() == m_pkDefMap->find(iGroundNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is GroundNo["<<iGroundNo<<L"] Can't find from TB_DefMap")));
			}
		}
		void CheckMonster(int const iMonsterNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iMonsterNo )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is MonsterNo Can't a Zero(0)")));
			}
			if( m_pkDefMonster->end() == m_pkDefMonster->find(iMonsterNo)
			&&	m_pkDefObjects->end() == m_pkDefObjects->find(iMonsterNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is MonsterNo["<<iMonsterNo<<L"] Can't find from TB_DefMonster")));
			}
		}
		void CheckMonster100(int const iMonsterNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iMonsterNo )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is MonsterNo Can't a Zero(0)")));
			}

			const int iBaseMonsterNo = iMonsterNo / 100 * 100;

			if( m_pkDefMonster->end() == m_pkDefMonster->find(iBaseMonsterNo)
			&&	m_pkDefObjects->end() == m_pkDefObjects->find(iBaseMonsterNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is MonsterNo["<<iBaseMonsterNo<<L"] Can't find from TB_DefMonster")));
			}
		}
		void CheckCharacterClass(int const iClassNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iClassNo )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is CHARACTER ClassNo Can't a Zero(0)")));
			}
			if( m_pkDefClass->end() == m_pkDefClass->find(iClassNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is CHARACTER ClassNo["<<iClassNo<<L"] Can't find from TB_DefClass")));
			}
		}
		void CheckMission(int const iMissionKey, BM::vstring const& rkHeaderMsg)
		{
			CONT_DEF_MISSION_ROOT::const_iterator iter = m_pkDefMissionRoot->begin();
			while( m_pkDefMissionRoot->end() != iter )
			{
				if( (*iter).second.iKey == iMissionKey )
				{
					break;
				}
				++iter;
			}

			if( m_pkDefMissionRoot->end() == iter )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is MISSION Key["<<iMissionKey<<L"] Can't find from TB_DefMission_LevelRoot")));
			}
		}
		void CheckSQuestLimitItem(ContQuestLimitItem const& rkVec, BM::vstring const& rkHeaderMsg)
		{
			ContQuestLimitItem::const_iterator iter = rkVec.begin();
			while( rkVec.end() != iter )
			{
				BM::vstring const kTempHeader = BM::vstring(_T("Item [")) << (*iter).iItemNo << _T("] Count[") << (*iter).iCount << _T("] OperatorType[") << static_cast< int >((*iter).eOperatorType) << _T("]");
				CheckSQuestLimitItem((*iter), kTempHeader);
				++iter;
			}
		}
		void CheckSQuestItem(ContQuestItem const& rkVec, BM::vstring const& rkHeaderMsg)
		{
			ContQuestItem::const_iterator iter = rkVec.begin();
			while( rkVec.end() != iter )
			{
				BM::vstring const kTempHeader = BM::vstring(_T("Item [")) << (*iter).iItemNo << _T("] Count[") << (*iter).iCount << _T("] RarityType[") << (*iter).iRarityType << _T("] Seal[") << (*iter).bSeal << _T("]");
				CheckSQuestItem( (*iter), BM::vstring(rkHeaderMsg) << _T(", ") << kTempHeader );
				++iter;
			}
		}
		void CheckSQuestLimitItem(SQuestLimitItem const& rkItem, BM::vstring const& rkHeaderMsg)
		{
			if( QILO_NONE == rkItem.eOperatorType )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", Can't Limit operator is None OPERATOR=\" <= / == / >= \"") ));
			}
			CheckSQuestItem(rkItem, rkHeaderMsg);
		}
		void CheckSQuestItem(SQuestItem const& rkItem, BM::vstring const& rkHeaderMsg)
		{
			CheckItem( rkItem.iItemNo, rkHeaderMsg );
			CheckItemRarity( rkItem.iRarityType, rkHeaderMsg );
			if( 0 == rkItem.iCount )
			{
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", Can't Item count 0") ));
			}
			if( 0 != rkItem.iTimeValue )
			{
				if( 0 > rkItem.iTimeValue
				||	0x1FF < rkItem.iTimeValue )
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", Time Values is wrong, (0, 1~511)") ));
				}
			}
		}
		void CheckEffect(int const iEffectNo, BM::vstring const& rkHeaderMsg)
		{
			if( m_pkDefEffect->end() == m_pkDefEffect->find(iEffectNo) )
			{
				BM::vstring const kTempHeader = BM::vstring(rkHeaderMsg) << L", Can't Find EffectNo[" << iEffectNo << L"] From [TB_DefEffect]";
				PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, kTempHeader));
			}
		}
		void CheckItemRarity(int const iItemRarityNo, BM::vstring const& rkHeaderMsg)
		{
			if( 0 == iItemRarityNo )
			{
				return;
			}

			if( m_pkDefItemRarityControl->end() == m_pkDefItemRarityControl->find(iItemRarityNo) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", is ItemRarity["<<iItemRarityNo<<L"] Can't find from Def")));
			}
		}
		void CheckReward(SQuestRewardSet const& rkReward, int const iNo)
		{
			tagQuestRewardSet::ContClassNo::const_iterator class_iter = rkReward.kCanClass.begin();
			while( rkReward.kCanClass.end() != class_iter )
			{
				CheckCharacterClass( (*class_iter), BM::vstring(_T("Reward Set[")) << iNo << _T("] Class[") << (*class_iter) << _T("]") );
				++class_iter;
			}

			switch( GetR().Type() )
			{
			case QT_Couple:
			case QT_SweetHeart:
				{
					if( GetR().IsCanRemoteComplete() // 원격 완료와 퀘스트 선택 보상은 사용 할 수 없다 (커플/결혼 퀘스트에 한해서)
					&&	rkReward.bSelective )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( GetP(), PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(L"Couple or Marry Quest is can't use Selective Reward Set") ));
					}
				}break;
			default:
				{
				}break;
			}

			ContQuestItem::const_iterator item_iter = rkReward.kItemList.begin();
			while( rkReward.kItemList.end() != item_iter )
			{
				BM::vstring const kTempHeader = BM::vstring(_T("Reward Set[")) << iNo << _T("] ItemNo[") << (*item_iter).iItemNo << _T("] Count[") << (*item_iter).iCount << _T("] RarityType[") << (*item_iter).iRarityType << _T("] Seal[") << (*item_iter).bSeal << _T("]");
				CheckSQuestItem( (*item_iter), kTempHeader );
				++item_iter;
			}
		}
		void CheckDialog(int const iDialogID, BM::vstring const& rkHeaderMsg)
		{
			if( Dialog().end() == Dialog().find(iDialogID)
			&&	m_kRefDialog.end() == m_kRefDialog.find(iDialogID) )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", Can't find DIALOG from XML")));
			}
		}
		void CheckNpcEvent(int const iEventNo, BM::vstring const& rkHeaderMsg)
		{
			{
				ContQuestNpc::const_iterator iter = ClientNPC().begin();
				while( ClientNPC().end() != iter )
				{
					if( (*iter).iEventNo == iEventNo )
					{
						return;
					}
					++iter;
				}
			}
			{
				ContQuestNpc::const_iterator iter = AgentNPC().begin();
				while( AgentNPC().end() != iter )
				{
					if( (*iter).iEventNo == iEventNo )
					{
						return;
					}
					++iter;
				}
			}
			{
				ContQuestNpc::const_iterator iter = PayerNPC().begin();
				while( PayerNPC().end() != iter )
				{
					if( (*iter).iEventNo == iEventNo )
					{
						return;
					}
					++iter;
				}
			}

			AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", Can't find NPC EventNo from XML")));
		}
		void CheckAbilType(int const iAbilType, BM::vstring const& rkHeaderMsg)
		{
			CONT_DEF_ACHIEVEMENTS::const_iterator achievements_iter = m_pkDefAchievements->begin();
			while( m_pkDefAchievements->end() != achievements_iter )
			{
				if( (*achievements_iter).second.iType == iAbilType )
				{
					break;
				}
				++achievements_iter;
			}
			bool const bIsCanAchievementTankPointAbil =	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_1)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_2)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_3)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_4)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_5)
													||	(iAbilType == AT_ACHIEVEMENT_TOTAL_POINT_CATEGORY_6);
			if( !PgQuestInfoUtil::IsQuestDependAbilType(iAbilType)
			&&	!bIsCanAchievementTankPointAbil
			&&	m_pkDefAchievements->end() == achievements_iter ) // 프로그래머가 임의 지정한 어빌도 아니고, 업적도 아니면 에러
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", can't use abil type [")<<iAbilType<<L"]"));
			}
			else
			{
				if( bIsCanAchievementTankPointAbil
				||	m_pkDefAchievements->end() != achievements_iter ) // 업적 어빌이면, 사용 가능 테이블에 등록
				{
					PgQuestInfoUtil::kContQuestDependAchievementsAbil.insert( static_cast< EAbilType >(iAbilType) );
				}
			}
		}
		void CheckObjectNo(int const iObjectNo, BM::vstring const& rkHeaderMsg)
		{
			if( iObjectNo + 1 )
			{
				if( 0 == GetR().m_kObject.GetEndCount(iObjectNo) )
				{
					AddError( SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(rkHeaderMsg) << _T(", target Object[") << iObjectNo << _T("] is a COUNT 0") ) );
				}
			}
		}
		void VerifyMissionEvent(SQuestDependMission const& rkEventMission)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("MISSION EVENT MissionKey[")) << rkEventMission.iMissionKey << _T("]");
			switch( rkEventMission.iType )
			{
			case QET_MISSION_Rank:
			case QET_MISSION_RankN:
			case QET_MISSION_Percent:
			case QET_MISSION_Clear:
			case QET_MISSION_ClearN:
			case QET_MISSION_DEFENCE_CLEAR:
			case QET_MISSION_STRATEGY_DEFENCE_CLEAR:
			default:
				{
				}break;
			}
			CheckMission(rkEventMission.iMissionKey, kTempHeader);
			CheckObjectNo(rkEventMission.iObjectNo, kTempHeader);
		}
		void VerifyNpcEvent(SQuestDependNpc const& rkEventNpc)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("NPC EVENT EventNo[")) << rkEventNpc.iEventNo << _T("] DialogID[") << rkEventNpc.iDialogID << _T("] ObjectNo[") << rkEventNpc.iObjectNo+1 << _T("]");
			switch( rkEventNpc.iType )
			{
			case QET_NPC_Client:
			case QET_NPC_Ing_Dlg:
			//case QET_NPC_Agent: //QET_NPC_Ing_Dlg 와 같다
			case QET_NPC_Payer:
				{
					// Verify Dialog Event
					// NPC Guid설정과 EventNo와 QuestStatus와 Dialog 전부 정상적으로 진행 할 수 있는가?
					// 
				}break;
			case QET_NPC_GiveItem:
			case QET_NPC_RemoveItem:
			case QET_NPC_CheckItem:
				{
					BM::vstring const kTempItemHeader = BM::vstring(kTempHeader) << _T(" GiveItem[No: ") << rkEventNpc.kItem.iItemNo << _T(", Count:") << rkEventNpc.kItem.iCount << _T(", Rarity:") << rkEventNpc.kItem.iRarityType << _T(", Seal:") << rkEventNpc.kItem.bSeal << _T("]");
					CheckSQuestItem( rkEventNpc.kItem, kTempItemHeader );
				}break;
			case QET_NPC_CheckEffect:
			case QET_NPC_GiveEffect:
				{
					CheckEffect( rkEventNpc.kItem.iItemNo, kTempHeader );
				}break;
			case QET_NPC_CompleteQuest:
				{
					BM::vstring const kTempQuestHeader = BM::vstring(kTempHeader) << L" Quest["<<rkEventNpc.iVal1<<L"]";
					CheckQuestID( rkEventNpc.iVal1, kTempQuestHeader );
				}break;
			case QET_NPC_IncParam:
			case QET_NPC_CheckGold:
			case QET_NPC_AddGold:
			default:
				{
				}break;
			}

			CheckDialog( rkEventNpc.iDialogID, kTempHeader );
			CheckNpcEvent( rkEventNpc.iEventNo, kTempHeader );
			CheckObjectNo( rkEventNpc.iObjectNo, kTempHeader );

			switch( rkEventNpc.iType )
			{
			case QET_NPC_CheckItem:
			case QET_NPC_CheckGold:
				{
					CheckDialog( rkEventNpc.iVal1, BM::vstring(kTempHeader) << _T(" Depend Dialog [") << rkEventNpc.iVal1 << _T("]") );
				}break;
			default:
				{
				}break;
			}
		}
		void VerifyClassEvent(SQuestDependClass const& rkEventClass)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("CLASS EVENT ClassNo[")) << rkEventClass.iClassNo << _T("]");
			switch( rkEventClass.iType )
			{
			case QET_CLASS_Change:
			default:
				{
				}break;
			}
			CheckCharacterClass(rkEventClass.iClassNo, kTempHeader);
			CheckObjectNo( rkEventClass.iObjectNo, kTempHeader );
		}
		void VerifyAbilEvent(SQuestDependAbil const& rkEventAbil)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("ABIL EVENT AbilType[")) << rkEventAbil.iAbilType << _T("] Value["<<rkEventAbil.iAbilValue<<L"]");
			switch( rkEventAbil.iType )
			{
			case QET_ABIL_GREATER_EQUAL:
			default:
				{
				}break;
			}
			CheckAbilType(rkEventAbil.iAbilType, kTempHeader);
			if( 0 >= rkEventAbil.iAbilValue )
			{
				AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(kTempHeader) << _T(", wrong abil value [")<<rkEventAbil.iAbilValue<<L"]"));
			}
		}
		void VerifyGlobalEvent( SQuestDependGlobal const& rkEventGlobal )
		{
			BM::vstring const kTempHeader = BM::vstring(_T("GLOBAL EVENT Type[")) << rkEventGlobal.iType << _T("]");
			switch( rkEventGlobal.iType )
			{
			case QET_GLOBAL_Combo_IncParam:
			default:
				{
				}break;
			}

			CheckObjectNo(rkEventGlobal.iObjectNo, kTempHeader );
		}
		void VerifyItemEvent(SQuestDependItem const& rkEventItem)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("ITEM EVENT ItemNo[")) << rkEventItem.iItemNo << _T("]");
			switch( rkEventItem.iType )
			{
			case QET_ITEM_ChangeCount:
			case QET_ITEM_NotChangeCount:
			case QET_ITEM_Durability_Check:
			case QET_ITEM_Plus_Check:
			case QET_ITEM_Check_Pet:
			default:
				{
				}break;
			}
			CheckItem(rkEventItem.iItemNo, kTempHeader);
			CheckObjectNo(rkEventItem.iObjectNo, kTempHeader);
		}
		void VerifyMonsterEvent(SQuestMonster const& rkEventMonster)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("MONSTER EVENT MonsterNo[")) << rkEventMonster.iClassNo << _T("]");
			switch( rkEventMonster.iType )
			{
			case QET_MONSTER_IncParam_In:
			case QET_MONSTER_IncParam100_In:
				{
					CheckGround(rkEventMonster.iTargetGroundNo, kTempHeader);
				}break;
			case QET_MONSTER_InstDrop:
			case QET_MONSTER_InstDrop100:	
				{
					CheckItem(rkEventMonster.iItemNo, kTempHeader);
				}break;
			case QET_MONSTER_InstDrop_M:
			case QET_MONSTER_InstDrop100_M:
			case QET_MONSTER_InstDrop_MN:
			case QET_MONSTER_InstDrop100_MN:
				{
					CheckMission(rkEventMonster.iMissionKey, kTempHeader);
					CheckItem(rkEventMonster.iItemNo, kTempHeader);
				}break;
			case QET_MONSTER_InstDrop_In:
			case QET_MONSTER_InstDrop100_In:
				{
					CheckGround(rkEventMonster.iTargetGroundNo, kTempHeader);
					CheckItem(rkEventMonster.iItemNo, kTempHeader);
				}break;
			case QET_MONSTER_IncParam_M:
			case QET_MONSTER_IncParam100_M:
			case QET_MONSTER_IncParam_MN:
			case QET_MONSTER_IncParam100_MN:
				{
					CheckMission(rkEventMonster.iMissionKey, kTempHeader);
				}break;
			case QET_MONSTER_IncParam:
			case QET_MONSTER_IncParam100:
			case QET_MONSTER_ComboCheck:
			default:
				{
				}break;
			}

			switch( rkEventMonster.iType )
			{
			case QET_MONSTER_InstDrop:
			case QET_MONSTER_InstDrop100:
			case QET_MONSTER_InstDrop_In:
			case QET_MONSTER_InstDrop_M:
			case QET_MONSTER_InstDrop100_In:
			case QET_MONSTER_InstDrop100_M:
			case QET_MONSTER_InstDrop_MN:
			case QET_MONSTER_InstDrop100_MN:
				{
					if( rkEventMonster.iVal3 < rkEventMonster.iVal4 )
					{
						AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(kTempHeader) << _T(", Wrong Item Count [Max:")<<rkEventMonster.iVal3<<L", Min:"<<rkEventMonster.iVal4<<L"]"));
					}
				}break;
			}

			switch( rkEventMonster.iType )
			{
			case QET_MONSTER_IncParam100:
			case QET_MONSTER_InstDrop100:
			case QET_MONSTER_IncParam100_In:
			case QET_MONSTER_InstDrop100_In:
			case QET_MONSTER_InstDrop100_M:
			case QET_MONSTER_IncParam100_M:
			case QET_MONSTER_IncParam100_MN:
			case QET_MONSTER_InstDrop100_MN:
				{
					CheckMonster100(rkEventMonster.iClassNo, kTempHeader);
				}break;
			default:
				{
					CheckMonster(rkEventMonster.iClassNo, kTempHeader);
				}break;
			}
			
			CheckObjectNo(rkEventMonster.iObjectNo, kTempHeader);
		}
		void VerifyGroundEvent(SQuestGround const& rkEventGround)
		{
			BM::vstring const kTempHeader = BM::vstring( _T("GROUND EVENT GroundNo[")) << rkEventGround.iGroundNo << _T("]");
			switch( rkEventGround.iType )
			{
			case QET_GROUND_ComboCheck:
			case QET_GROUND_KillMonster:
			case QET_GROUND_IndunResult:
				{
				}break;
			case QET_GROUND_MonsterDrop:
				{
					CheckItem(rkEventGround.iItemNo, kTempHeader);

					if( rkEventGround.iVal3 < rkEventGround.iVal4 )
					{
						AddError(SQuestInfoError(GetP(), ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(kTempHeader) << _T(", Wrong Item Count [Max:")<<rkEventGround.iVal3<<L", Min:"<<rkEventGround.iVal4<<L"]"));
					}
				}break;
			default:
				{
				}break;
			}

			CheckGround(rkEventGround.iGroundNo, kTempHeader);
			CheckObjectNo(rkEventGround.iObjectNo, kTempHeader);
		}
		void VerifyDialog(SQuestDialog const& rkDialog)
		{
			BM::vstring const kTempHeader = BM::vstring(_T("DIALOG ID[")) << rkDialog.iDialogID << _T("] Type[") << rkDialog.eType << _T("]");

			ContQuestSelect::const_iterator select_iter = rkDialog.kSelectList.begin();
			while( rkDialog.kSelectList.end() != select_iter )
			{
				VerifyDialogSelect((*select_iter), kTempHeader);
				++select_iter;
			}
		}
		void VerifyDialogSelect(SQuestSelect const& rkDialogSelect, BM::vstring const& rhHeaderMsg)
		{
			BM::vstring const kTempHeader = BM::vstring(rhHeaderMsg) << _T(" SELECT ID[") << rkDialogSelect.iSelectID << _T("] Type[") << rkDialogSelect.eType << _T("]");
			CheckDialog( GetNextDialogID(rkDialogSelect.iSelectID), kTempHeader );
		}

	private:
		PgQuestInfo const* m_pkQuestInfo;	// Quest Info
		ContDialogSet m_kRefDialog;			// Internal Making Def

		// All Quest
		ContQuestInfo const& m_kXmlInfoMap; // Ref

		// All Def
		CONT_DEFITEM const* m_pkDefItem;
		CONT_DEFMAP const* m_pkDefMap;
		CONT_DEFMONSTER const* m_pkDefMonster;
		CONT_DEF_OBJECT const* m_pkDefObjects;
		CONT_DEF_ITEM_RARITY_CONTROL const* m_pkDefItemRarityControl;
		CONT_DEFEFFECT const* m_pkDefEffect;
		CONT_DEFCLASS const* m_pkDefClass;
		CONT_DEF_MISSION_ROOT const* m_pkDefMissionRoot;
		CONT_DEF_QUEST_REWARD const* m_pkDefQuestReward;
		CONT_DEF_ACHIEVEMENTS const* m_pkDefAchievements;

		int const m_iLimitKillCount;
	};

	void VerifyXmlToDef(ContQuestInfo const& rkXmlInfoMap)
	{
		CONT_DEF_QUEST_REWARD const* pkDefReward = NULL;
		g_kTblDataMgr.GetContDef(pkDefReward);

		{
			ContQuestInfo::const_iterator iter = rkXmlInfoMap.begin();
			while( rkXmlInfoMap.end() != iter )
			{
				ContQuestInfo::key_type const& rkKey = (*iter).first;
				ContQuestInfo::mapped_type const& rkQuestInfo = (*iter).second;
				if( pkDefReward->end() == pkDefReward->find(rkKey) )
				{
					// XML 은 있지만 Def에 없는것
					AddError(SQuestInfoError(rkQuestInfo, ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't find QUESTID[")) << rkKey << _T("] from TB_DefQuestReward") ));
				}
				++iter;
			}
		}

		{
			CONT_DEF_QUEST_REWARD::const_iterator iter = pkDefReward->begin();
			while( pkDefReward->end() != iter )
			{
				CONT_DEF_QUEST_REWARD::key_type const& rkKey = (*iter).first;
				CONT_DEF_QUEST_REWARD::mapped_type const& rkDef = (*iter).second;
				if( PgQuestInfoUtil::IsLoadXmlType(rkDef.iDBQuestType) )
				{
					if( rkXmlInfoMap.end() == rkXmlInfoMap.find(rkKey) )
					{
						// Def엔 있지만 XML이 없는것
						AddError(SQuestInfoError(ET_DataError, std::string(), rkKey, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't find XML File, QUESTID[")) << rkKey << _T("]") ));
					}
				}
				else
				{
					if( rkXmlInfoMap.end() != rkXmlInfoMap.find(rkKey) )
					{
						// XML이 없어야 한다.
						AddError(SQuestInfoError(ET_DataError, std::string(), rkKey, __FUNCTIONW__, __LINE__, BM::vstring(_T("is QUESTID[")) << rkKey << _T("], have not XML File, marked deleted quest TYPE[-1]") ));
					}
				}
				++iter;
			}
		}
	}

	//
	void Verify(PgQuestInfo const* pkQuestInfo, ContQuestInfo const& rkXmlInfoMap, int const iMaxKillCount)
	{
		//if( !pkQuestInfo )
		//{
		//	return;
		//}
		PgQuestInfoVerifier kVerifier(pkQuestInfo, rkXmlInfoMap, iMaxKillCount);
		kVerifier.Verify();
	}

	bool DisplayResult()
	{
		if( !g_kErrorMsg.empty() )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("------------------ Quest Error Display Start ------------------"));
			std::for_each(g_kErrorMsg.begin(), g_kErrorMsg.end(), OutputError);
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("------------------  Quest Error Display End  ------------------"));
			g_kErrorMsg.clear();
			return false; // Can't Pass, have a error
		}
		return true; // Pass
	}
}


///////////////////////////////////////
tagQuestBasicInfo::tagQuestBasicInfo()
{
	Clear();	
}

tagQuestBasicInfo::tagQuestBasicInfo(int const iID, int const iGroup)
	:iQuestID(iID), iGroupNo(iGroup), iDifficult(0), iOrderIndex(0)
{
	eType = QT_None;
}

void tagQuestBasicInfo::Clear()
{
	eType = QT_Normal;
	iQuestID = 0;
	iGroupNo = 0;
	iDifficult = 0;
	iOrderIndex = 0;
}


///////////////////////////////////////
tagQuestLimitTime::tagQuestLimitTime(char const* szStart, char const* szEnd, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	StrToTime(szStart, kTimeStart);
	StrToTime(szEnd, kTimeEnd);
	bool const bWrongLimit = Less(kTimeStart, kTimeEnd);
	if( !bWrongLimit )
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Wrong Quest Limit Time Start[")) << Str(kTimeStart) << _T("] -> End[") << Str(kTimeEnd) << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}
}

tagQuestLimitTime::tagQuestLimitTime(SYSTEMTIME const& rkStartTime, SYSTEMTIME const& rkEndTime)
	:kTimeStart(rkStartTime), kTimeEnd(rkEndTime)
{
}

void tagQuestLimitTime::Clear()
{
	ZeroMemory(&kTimeStart, sizeof(kTimeStart));
	ZeroMemory(&kTimeEnd, sizeof(kTimeEnd));
}
void tagQuestLimitTime::StrToTime(char const* szTimeString, SYSTEMTIME& rkOutTime)//"200x-xx-xx xx:xx:xx"
{
	if( !szTimeString ) return;
	::ZeroMemory(&rkOutTime, sizeof(SYSTEMTIME));

	int iYear = 0, iMonth = 0, iDay = 0, iHour = 0, iMinute = 0, iSecond = 0;//, iMilliseconds = 0, iDayOfWeek = 0;
	sscanf_s(szTimeString, "%4d-%2d-%2d %2d:%2d:%2d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
	rkOutTime.wYear = (unsigned short)iYear;
	rkOutTime.wMonth = (unsigned short)iMonth;
	rkOutTime.wDay = (unsigned short)iDay;
	rkOutTime.wHour = (unsigned short)iHour;
	rkOutTime.wMinute = (unsigned short)iMinute;
	rkOutTime.wSecond = (unsigned short)iSecond;
}

std::wstring tagQuestLimitTime::Str(SYSTEMTIME const& rkTime)
{
	TCHAR szTemp[MAX_PATH] = {0, };
	_stprintf_s(szTemp, MAX_PATH, _T("%04d-%02d-%02d %02d:%02d:%02d"), rkTime.wYear, rkTime.wMonth, rkTime.wDay, rkTime.wHour, rkTime.wMinute, rkTime.wSecond);
	return std::wstring(szTemp);
}

bool tagQuestLimitTime::IsCan(SYSTEMTIME const& rkCurTime) const
{
	// Cur > Start
	// Cur < End
	// Start <= Cur <= end
	if( Greater(rkCurTime, kTimeStart) //Set Start <= Cur 시작 시간 보다 이후이면서
	&&	Less(rkCurTime, kTimeEnd) )//Set End >= Cur 끝나는 시간 보다 이전 일 때
	{
		return true;//진행 가능
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;//진행 불가
}

//아래 이후는 좌항이 항상 현재 시간값이다
bool tagQuestLimitTime::Less(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight)//Left < Right(config)
{
	if( rkRight.wYear && rkNow.wYear < rkRight.wYear ) return true;
	if( rkRight.wYear && rkNow.wYear > rkRight.wYear ) return false;
	if( rkRight.wMonth && rkNow.wMonth < rkRight.wMonth ) return true;
	if( rkRight.wMonth && rkNow.wMonth > rkRight.wMonth ) return false;
	if( rkRight.wDay && rkNow.wDay < rkRight.wDay ) return true;
	if( rkRight.wDay && rkNow.wDay > rkRight.wDay ) return false;
	if( rkRight.wHour && rkNow.wHour < rkRight.wHour ) return true;
	if( rkRight.wHour && rkNow.wHour > rkRight.wHour ) return false;
	if( rkRight.wMinute && rkNow.wMinute < rkRight.wMinute ) return true;
	if( rkRight.wMinute && rkNow.wMinute > rkRight.wMinute ) return false;
	if( rkRight.wSecond && rkNow.wSecond < rkRight.wSecond ) return true;
	if( rkRight.wSecond && rkNow.wSecond > rkRight.wSecond ) return false;
	return true;// rkRight == 0
}

bool tagQuestLimitTime::Greater(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight)//Left > Right(Config)
{
	return !Less(rkNow, rkRight);
}

bool tagQuestLimitTime::Equal(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight)//Left == Right
{
	bool bRet = false;
	if( rkRight.wYear ) { bRet = rkNow.wYear == rkRight.wYear && bRet; }
	if( rkRight.wMonth ) { bRet = rkNow.wMonth == rkRight.wMonth && bRet; }
	if( rkRight.wDay ) { bRet = rkNow.wDay == rkRight.wDay && bRet; }
	if( rkRight.wHour ) { bRet = rkNow.wHour == rkRight.wHour && bRet; }
	if( rkRight.wMinute ) { bRet = rkNow.wMinute == rkRight.wMinute && bRet; }
	if( rkRight.wSecond ) { bRet = rkNow.wSecond == rkRight.wSecond && bRet; }
	return bRet;
}

bool tagQuestLimitTime::LessEqual(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight)//Left <= Right
{
	if( Equal(rkNow, rkRight) )
	{
		return true;
	}
	return Less(rkNow, rkRight);
}

bool tagQuestLimitTime::GreaterEqual(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight)//Left >= Right
{
	if( Equal(rkNow, rkRight) )
	{
		return true;
	}
	return !Less(rkNow, rkRight);
}


////////////////////////////////////////////////
tagQuestLimitInfo::tagQuestLimitInfo()
{
	Clear();
}

void tagQuestLimitInfo::Clear()
{
	iClassFlag = 0;
	iMinLevel = iMaxLevel = 0;
	iLimitMaxLevel = 0;
	iMinParty = iMaxParty = 0;
	iTacticsLevel = 0;
	kLimitTime.clear();
	kDayOfWeek.clear();
	kPetType.clear();
	kPetClassGrade.clear();
	iPetLevelMin = iPetLevelMax = 0;
	ePetCashType = CT_All;
}

void tagQuestLimitInfo::AddClass(int iClassNo)
{
	__int64 iTemp = 1;
	iTemp = (iTemp << iClassNo);
	iClassFlag = iClassFlag | iTemp;
}

bool tagQuestLimitInfo::CheckLevel(int const iLevel) const
{
	if( iLevel < iMinLevel )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if( 0 != iLimitMaxLevel
	&&	iLevel > iLimitMaxLevel )
	{
		return false;
	}
	return true;
}
bool tagQuestLimitInfo::CheckPetLevel(int const iPetLevel) const
{
	if(iPetLevelMin == 0 && iPetLevelMax == 0)
	{ //최소 & 최대 레벨 제한이 없으면 레벨을 체크하지 않고 무조건 통과시키자. (퀘스트 xml에 제한 걸리지 않은 경우이다)
		return true;
	}

	if( iPetLevelMin > iPetLevel || iPetLevelMax < iPetLevel )
	{
		return false;
	}
	return true;
}
bool tagQuestLimitInfo::CheckPetClassGrade(BYTE const byPetClassLevel) const
{
	if(kPetClassGrade.empty())
	{
		return true;
	}

	const ContPetClassGrade::const_iterator iterFound = 
		std::find(kPetClassGrade.begin(), kPetClassGrade.end(), byPetClassLevel);
	if(iterFound == kPetClassGrade.end())
	{
		return false;
	}
	return true;
}
bool tagQuestLimitInfo::CheckClass(int const iClassNo) const
{
	__int64 iTemp = 1;
	iTemp = (iTemp << iClassNo);
	__int64 iRet = (iClassFlag&iTemp);
	return (0 != iRet);
}

bool tagQuestLimitInfo::CheckPetType(int const iPetType) const
{
	if(kPetType.empty()) //제한 타입이 없다면 타입을 체크하지 않고 무조건 통과시키자. (퀘스트 xml에 제한 걸리지 않은 경우이다)
	{
		return true;
	}

	const ContPetType::const_iterator iterFound = 
		std::find(kPetType.begin(), kPetType.end(), static_cast<BYTE>(iPetType));
	if(iterFound == kPetType.end())
	{
		return false;
	}
	return true;
}

bool tagQuestLimitInfo::CheckPetCashType(const PgBase_Item &kItem) const
{
	if(ePetCashType == CT_All) { return true; } //검사하지 않음

	bool bCashItem = CheckIsCashItem(kItem);
	if(
		(ePetCashType == CT_Ingame && bCashItem == false) ||
		(ePetCashType == CT_Cash && bCashItem == true)
		)
	{
		return true;
	}
	return false;
}

void tagQuestLimitInfo::AddTime(const SQuestLimitTime& rkNewLimit)
{
	kLimitTime.push_back(rkNewLimit);
}

bool tagQuestLimitInfo::CheckTime() const
{
	SYSTEMTIME kCurSysTime;
	::GetLocalTime(&kCurSysTime);

	if( !kDayOfWeek.empty()
	&&	kDayOfWeek.end() == kDayOfWeek.find(kCurSysTime.wDayOfWeek) )
	{
		return false;
	}
	return CheckTime(kCurSysTime);
}

bool tagQuestLimitInfo::CheckTime(SYSTEMTIME const& rkCurrentTime) const
{
	if( kLimitTime.empty() )
	{
		return true;
	}

	ContQuestTimeLimit::const_iterator time_iter = kLimitTime.begin();
	while(kLimitTime.end() != time_iter)
	{
		const ContQuestTimeLimit::value_type& rkCurLimit = (*time_iter);
		if( rkCurLimit.IsCan(rkCurrentTime) )
		{
			return true;
		}
		++time_iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}


/////////////////////////////////////////////////
tagQuestItem::tagQuestItem()
	:iItemNo(0), iCount(0), iRarityType(GIOT_NONE), bSeal(false), iTimeType(UIT_MIN), iTimeValue(0)
{
}

tagQuestItem::tagQuestItem(int const ItemNo, int const Count)
	:iItemNo(ItemNo), iCount(Count), iRarityType(GIOT_NONE), bSeal(false), iTimeType(UIT_MIN), iTimeValue(0)
{
}

tagQuestItem::tagQuestItem(int const ItemNo, int const Count, int const RarityType, bool const Seal, int const TimeType, int const TimeValue)
	:iItemNo(ItemNo), iCount(Count), iRarityType(RarityType), bSeal(Seal), iTimeType(TimeType), iTimeValue(TimeValue)
{
}

void tagQuestItem::Clear()
{
	iItemNo = 0;
	iCount = 0;
	iRarityType = GIOT_NONE;
	bSeal = false;
	iTimeType = UIT_MIN;
	iTimeValue = 0;
}

bool tagQuestItem::operator == (const tagQuestItem& rhs) const
{
	return (iItemNo == rhs.iItemNo) && (iCount == rhs.iCount);
}
bool tagQuestItem::operator == (int const rhs) const
{
	return (iItemNo == rhs);
}

/////////////////////////////////////////////////
tagQuestLimitItem::tagQuestLimitItem(int const ItemNo, int const Count, EQuestItemLimitOperator const OperatorType)
	: tagQuestItem(ItemNo, Count), eOperatorType(OperatorType)
{
}

void tagQuestLimitItem::Clear()
{
	tagQuestItem::Clear();
	eOperatorType = QILO_NONE;
}

bool tagQuestLimitItem::operator == (tagQuestLimitItem const& rhs) const
{
	return tagQuestItem::operator ==(rhs) && eOperatorType == rhs.eOperatorType;
}


/////////////////////////////////////////////////
tagChangeClassInfo::tagChangeClassInfo()
{
	Clear();
}

tagChangeClassInfo::tagChangeClassInfo(int const ChangeClassNo, int const PrevClassNo, int const MinLevel)
{
	Set(ChangeClassNo, PrevClassNo, MinLevel);
}

void tagChangeClassInfo::Set(int const ChangeClassNo, int const PrevClassNo, int const MinLevel)
{
	iChangeClassNo = ChangeClassNo;
	iPrevClassNo = PrevClassNo;
	iMinLevel = MinLevel;
}

void tagChangeClassInfo::Clear()
{
	iChangeClassNo = 0;
	iPrevClassNo = 0;
	iMinLevel = 0;
}


/////////////////////////////////////////////////
tagQuestRewardSet::tagQuestRewardSet()
{
	Clear();
}

void tagQuestRewardSet::Clear()
{
	bSelective = false;
	kCanClass.clear();
	kItemList.clear();
	iGender = 0;
}

bool tagQuestRewardSet::AddRewardItem(const SQuestItem& rkRewardItem)
{
	if( MAX_QUEST_REWARD_ITEM == kItemList.size() )
	{
		assert(0 && kItemList.size());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	std::back_inserter(kItemList) = rkRewardItem;
	return true;
}

bool tagQuestRewardSet::AddClass(int const iClassNo)
{
	ContClassNo::iterator find_iter = std::find(kCanClass.begin(), kCanClass.end(), iClassNo);
	if( kCanClass.end() != find_iter )
	{
		assert(0 && iClassNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	std::back_inserter(kCanClass) = iClassNo;
	return true;
}

bool tagQuestRewardSet::IsCanClass(int const Gender, int const iClassNo) const
{
	bool bCanGender = true;
	if( 0 != iGender
	&&	iGender != Gender )
	{
		bCanGender = false;
	}

	bool bCanClass = true;
	if( !kCanClass.empty() )
	{
		bCanClass = kCanClass.end() != std::find(kCanClass.begin(), kCanClass.end(), iClassNo);
	}
	
	return bCanGender && bCanClass;
}


/////////////////////////////////////////////////
tagQuestReward::tagQuestReward()
{
	iChangeLevel = 0;
	Clear();
}

tagQuestReward::tagQuestReward(int const Exp, int const Money, int const TacticsExp, int const GuildExp, std::wstring const& rkRealmQuestID, int iLevelPer_Exp_1000, int iLevelPer_Money_1000 )
{
	Clear();
	iExp = Exp;
	iMoney = Money;
	iTacticsExp = TacticsExp;
	iGuildExp = GuildExp;
	kRealmQuetID = rkRealmQuestID;
	iChangeLevel = 0;
}

bool tagQuestReward::AddRewardSet(int const iSetNo, SQuestRewardSet const &rkRewardSet, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	switch( iSetNo )
	{
	case 1:
		{
			std::back_inserter(kClassRewardSet1) = rkRewardSet;
		}break;
	case 2:
		{
			std::back_inserter(kClassRewardSet2) = rkRewardSet;
		}break;
	default:
		{
			rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Wrong Reward Set No: ")) << iSetNo );
			PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}

void tagQuestReward::Clear()
{
	iExp = iMoney = iTacticsExp = iGuildExp = iSkillNo = 0;
	kClassRewardSet1.clear();
	kClassRewardSet2.clear();
	kChangeClass.Clear();
	kEffect.clear();
	kAddEndQuest.clear();
	kRealmQuetID.clear();
	iLevelPer_Exp_1000 = 0;			//천분률
	iLevelPer_Money_1000 = 0;		//천분률
	kUnlockCharacters = false;
}

ContRewardVec::value_type const* tagQuestReward::GetRewardSet1(int const iGender, int const iClassNo) const
{
	return GetRewardSet(kClassRewardSet1, iGender, iClassNo);
}

ContRewardVec::value_type const* tagQuestReward::GetRewardSet2(int const iGender, int const iClassNo) const
{
	return GetRewardSet(kClassRewardSet2, iGender, iClassNo);
}

ContRewardVec::value_type const* tagQuestReward::GetRewardSet(ContRewardVec const &rkVec, int const iGender, int const iClassNo) const
{
	typedef std::vector< SQuestRewardSet const * > ContSetVec;
	ContSetVec kTempVec;

	ContRewardVec::const_iterator iter = rkVec.begin();
	while(rkVec.end() != iter)
	{
		ContRewardVec::value_type const &rkRewardSet = (*iter);
		if( rkRewardSet.IsCanClass(iGender, iClassNo) )
		{
			std::back_inserter(kTempVec) = &(*iter);
		}
		++iter;
	}
	
	if( kTempVec.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}

	if( 1 < kTempVec.size() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"RewardSet 1[ClassNo:"<<iClassNo<<L"] Count["<<kTempVec.size()<<L"] > 1");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return kTempVec.front();
}


/////////////////////////////////////////////////
tagQuestObject::tagQuestObject()
{
	Clear();
}

void tagQuestObject::Clear()
{
	memset(iObjectCount, 0, sizeof(iObjectCount));
}

int tagQuestObject::GetEndCount(int const iObjectNo) const
{
	if( PgQuestInfoUtil::CheckIndex(iObjectNo) )
	{
		return iObjectCount[iObjectNo];
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}


/////////////////////////////////////////////////
tagQuestNpc::tagQuestNpc()
{
	iEventNo = 0;
	eMark = QS_None;
}

//tagQuestNpc::tagQuestNpc(BM::GUID const &rkGuid, int const EventNo, const EQuestState Mark)
//{
//	kNpcGuid = rkGuid;
//	iEventNo = EventNo;
//	eMark = Mark;
//}

tagQuestNpc::tagQuestNpc(BM::GUID const &rkGuid, int const EventNo, char const* szMark, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	kNpcGuid = rkGuid;
	iEventNo = EventNo;
	if( !szMark )
	{
		eMark = QS_None;
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Quest Mark is NULL, Npc[")) << rkGuid << _T("] EventNo[") << EventNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}
	else if( 0 == strcmp("BEGIN", szMark) )
	{
		eMark = QS_Begin;
	}
	else if( 0 == strcmp("ING", szMark) )
	{
		eMark = QS_Ing;
	}
	else if( 0 == strcmp("END", szMark) )
	{
		eMark = QS_End;
	}
	else if( 0 == strcmp("NONE", szMark) )
	{
		eMark = QS_None;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong Quest Mark[")) << szMark << _T("], Npc[") << rkGuid << _T("] EventNo[") << EventNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}
}


/////////////////////////////////////////////////
tagQuestBasicDepend::tagQuestBasicDepend(int const ObjectNo)
	:iType(0), iObjectNo(ObjectNo)
{
}


/////////////////////////////////////////////////
tagQuestGround::tagQuestGround(int const GroundNo, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iGroundNo(GroundNo)
{
}

bool tagQuestGround::Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));

	if( _T("COMBOCHECK") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Combo / Inc value") )
		{
			iType = QET_GROUND_ComboCheck;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else if( _T("KILLMONSTER") == rkType )
	{
		iType = QET_GROUND_KillMonster;
		iVal1 = PgStringUtil::SafeAtoi(rkValue);
	}
	else if( _T("MONSTERDROP") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 5, kVec.size(), szElementName, rkType, rkValue, L"Max rate / Success rate / Max count / Min count / ItemNo") )
		{
			iType = QET_GROUND_MonsterDrop;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[1]);
			iVal3 = PgStringUtil::SafeAtoi(kVec[2]);
			iVal4 = PgStringUtil::SafeAtoi(kVec[3]);
			iItemNo = PgStringUtil::SafeAtoi(kVec[4]);
		}
	}
	else if ( _T("INDUN_RESULT") == rkType )
	{
		iType = QET_GROUND_IndunResult;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong GROUND event TYPE[")) << rkType << L"] VALUE["<<rkValue<<L"], GroundNo[" << iGroundNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_GROUND_ComboCheck:
	case QET_GROUND_KillMonster:
	case QET_GROUND_MonsterDrop:
	case QET_GROUND_IndunResult:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", GROUND event TYPE[") << rkType << _T("] GroundNo[") << iGroundNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}
	return true;
}


/////////////////////////////////////////////////
tagQuestMonster::tagQuestMonster(int const ClassNo, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iClassNo(ClassNo), iVal1(0), iVal2(0), iVal3(0), iVal4(0), iItemNo(0), iTargetGroundNo(0), iMissionKey(-1), iMissionLevel(-1), iStageCount(0)
{//미션키와 미션레벨을 -1로 초기화 하는 이유는 0 값이 사용되고 있기 때문. 숙련된용자의길의 미션키가 0이고 아케이드모드의 레벨이 0이다.
}

bool tagQuestMonster::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));

	if( _T("INCPARAM") == rkType || _T("INCPARAM100") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Inc value") )
		{
			if( _T("INCPARAM100") == rkType )
			{
				iType = QET_MONSTER_IncParam100;
			}
			else
			{
				iType = QET_MONSTER_IncParam;
			}
			
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("INSTDROP") == rkType || _T("INSTDROP100") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 5, kVec.size(), szElementName, rkType, rkValue, L"Max rate / Success rate / Max count / Min count / ItemNo") )
		{
			if( _T("INSTDROP100") == rkType )
			{
				iType = QET_MONSTER_InstDrop100;
			}
			else
			{
				iType = QET_MONSTER_InstDrop;
			}
			
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[1]);
			iVal3 = PgStringUtil::SafeAtoi(kVec[2]);
			iVal4 = PgStringUtil::SafeAtoi(kVec[3]);
			iItemNo = PgStringUtil::SafeAtoi(kVec[4]);
		}
	}
	else if( _T("INCPARAM_IN") == rkType || _T("INCPARAM100_IN") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Target GroundNo / Inc value") )
		{
			if( _T("INCPARAM100_IN") == rkType )
			{
				iType = QET_MONSTER_IncParam100_In;
			}
			else
			{
				iType = QET_MONSTER_IncParam_In;
			}
			
			iTargetGroundNo = PgStringUtil::SafeAtoi(kVec[0]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else if( _T("INSTDROP_IN") == rkType || _T("INSTDROP100_IN") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 6, kVec.size(), szElementName, rkType, rkValue, L"Target GroundNo / Max rate / Success rate / Max count / Min count / ItemNo") )
		{
			if( _T("INSTDROP100_IN") == rkType )
			{
				iType = QET_MONSTER_InstDrop100_In;
			}
			else
			{
				iType = QET_MONSTER_InstDrop_In;
			}
			
			iTargetGroundNo = PgStringUtil::SafeAtoi(kVec[0]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[1]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[2]);
			iVal3 = PgStringUtil::SafeAtoi(kVec[3]);
			iVal4 = PgStringUtil::SafeAtoi(kVec[4]);
			iItemNo = PgStringUtil::SafeAtoi(kVec[5]);
		}
	}
	else if( _T("INSTDROP_M") == rkType || _T("INSTDROP100_M") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 7, kVec.size(), szElementName, rkType, rkValue, L"MissionKey /MissionLevel / Max rate / Success rate / Max count / Min count / ItemNo") )
		{
			if( _T("INSTDROP100_M") == rkType )
			{
				iType = QET_MONSTER_InstDrop100_M;
			}
			else
			{
				iType = QET_MONSTER_InstDrop_M;
			}

			iMissionKey = PgStringUtil::SafeAtoi(kVec[0]);
			iMissionLevel = PgStringUtil::SafeAtoi(kVec[1]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[2]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[3]);
			iVal3 = PgStringUtil::SafeAtoi(kVec[4]);
			iVal4 = PgStringUtil::SafeAtoi(kVec[5]);
			iItemNo = PgStringUtil::SafeAtoi(kVec[6]);
		}
	}
	else if( _T("INCPARAM_M") == rkType || _T("INCPARAM100_M") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 3, kVec.size(), szElementName, rkType, rkValue, L"MissionKey / MissionLevel / Inc Value") )
		{
			if( _T("INCPARAM100_M") == rkType )
			{
				iType = QET_MONSTER_IncParam100_M;
			}
			else
			{
				iType = QET_MONSTER_IncParam_M;
			}

			iMissionKey = PgStringUtil::SafeAtoi(kVec[0]);
			iMissionLevel = PgStringUtil::SafeAtoi(kVec[1]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[2]);
		}
	}
	else if( _T("INSTDROP_MN") == rkType || _T("INSTDROP100_MN") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 8, kVec.size(), szElementName, rkType, rkValue, L"MissionKey / MissionLevel / Stage Count / Max rate / Success rate / Max count / Min count / ItemNo") )
		{
			if( _T("INSTDROP100_MN") == rkType )
			{
				iType = QET_MONSTER_InstDrop100_MN;
			}
			else
			{
				iType = QET_MONSTER_InstDrop_MN;
			}

			iMissionKey = PgStringUtil::SafeAtoi(kVec[0]);
			iMissionLevel = PgStringUtil::SafeAtoi(kVec[1]);
			iStageCount = PgStringUtil::SafeAtoi(kVec[2]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[3]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[4]);
			iVal3 = PgStringUtil::SafeAtoi(kVec[5]);
			iVal4 = PgStringUtil::SafeAtoi(kVec[6]);
			iItemNo = PgStringUtil::SafeAtoi(kVec[7]);
		}
	}
	else if( _T("INCPARAM_MN") == rkType || _T("INCPARAM100_MN" == rkType ) )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 4, kVec.size(), szElementName, rkType, rkValue, L"MissionKey / MissionLevel / Stage Count / Inc Value ") )
		{
			if( _T("INCPARAM100_MN") == rkType )
			{
				iType = QET_MONSTER_IncParam100_MN;
			}
			else
			{
				iType = QET_MONSTER_IncParam_MN;
			}

			iMissionKey = PgStringUtil::SafeAtoi(kVec[0]);
			iMissionLevel = PgStringUtil::SafeAtoi(kVec[1]);
			iStageCount = PgStringUtil::SafeAtoi(kVec[2]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[3]);
		}
	}
	else if( _T("COMBOCHECK") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Target Combo / Inc Value") )
		{
			iType = QET_MONSTER_ComboCheck;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong MONSTER event TYPE[")) << rkType << L"] VALUE["<<rkValue<<L"]" );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
		//{
		//	if( PgQuestInfoUtil::VerifyIndexNotZero(iObjectNo) )
		//	{
		//		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] only = 0, MONSTER event TYPE[") << rkType << _T("] MonsterNo[") << iClassNo << ("]") );
		//		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
		//		return false;
		//	}
		//}break;
	case QET_MONSTER_InstDrop:
	case QET_MONSTER_InstDrop100:
	case QET_MONSTER_InstDrop_In:
	case QET_MONSTER_InstDrop_M:
	case QET_MONSTER_InstDrop100_M:
	case QET_MONSTER_InstDrop_MN:
	case QET_MONSTER_InstDrop100_MN:
	case QET_MONSTER_IncParam:
	case QET_MONSTER_IncParam100:
	case QET_MONSTER_IncParam_In:
	case QET_MONSTER_IncParam100_In:
	case QET_MONSTER_IncParam_M:
	case QET_MONSTER_IncParam100_M:
	case QET_MONSTER_IncParam_MN:
	case QET_MONSTER_IncParam100_MN:
	case QET_MONSTER_ComboCheck:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", MONSTER event TYPE[") << rkType << _T("] MonsterNo[") << iClassNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}


/////////////////////////////////////////////////
tagQuestDependItem::tagQuestDependItem()
	:SQuestBasicDepend(0), iItemNo(0), iVal1(0)
{
}

tagQuestDependItem::tagQuestDependItem(int const ItemNo, int const ObjectNo, int const Val1)
	:SQuestBasicDepend(ObjectNo), iItemNo(ItemNo), iVal1(Val1)
{
}

bool tagQuestDependItem::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("CHANGE_COUNT") == rkType )
	{
		iType = QET_ITEM_ChangeCount;
	}
	else if( _T("NOT_CHANGE_COUNT") == rkType )
	{
		iType = QET_ITEM_NotChangeCount;
	}
	else if( _T("DURABILITY_CHECK") == rkType )
	{
		iType = QET_ITEM_Durability_Check;
	}
	else if( _T("PLUS_CHECK") == rkType )
	{
		iType = QET_ITEM_Plus_Check;
	}
	else if( _T("PET_CHECK") == rkType )
	{
		iType = QET_ITEM_Check_Pet;
		iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong ITEM event TYPE[")) << rkType << _T("], ItemNo[") << iItemNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}
	
	switch( iType )
	{
	case QET_ITEM_ChangeCount:
	case QET_ITEM_NotChangeCount:
	case QET_ITEM_Durability_Check:
	case QET_ITEM_Plus_Check:
	case QET_ITEM_Check_Pet:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", ITEM event TYPE[") << rkType << _T("] ItemNo[") << iItemNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}
	return true;
}


/////////////////////////////////////////////////
tagQuestLocation::tagQuestLocation()
	:SQuestBasicDepend(0), iLocationNo(0), iGroundNo(0)
{
}

tagQuestLocation::tagQuestLocation(int const LocationNo, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iLocationNo(LocationNo), iGroundNo(ObjectNo)
{
}

bool tagQuestLocation::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));

	if( _T("LOCATION_ENTER") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"GroundNo") )
		{
			iType = QET_LOCATION_LocationEnter;
			iGroundNo = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong LOCATION event TYPE[")) << rkType << L"], VALUE["<<rkValue<<L"]" );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_LOCATION_LocationEnter:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", LOCATION event TYPE[") << rkType << _T("] LocationNo[") << iLocationNo << _T("] GroundNo[") << iGroundNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}


/////////////////////////////////////////////////
tagQuestDependNpc::tagQuestDependNpc()
	:SQuestBasicDepend(0), iEventNo(0), iDialogID(0), iVal1(0), kStr(0), iGold(0), kItem(0, 0)
{
}

tagQuestDependNpc::tagQuestDependNpc(int const EventNo, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iEventNo(EventNo), iDialogID(0), iVal1(0), kStr(), iGold(0), kItem(0, 0)
{
}

bool tagQuestDependNpc::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("CLIENT") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID") )
		{
			iType = QET_NPC_Client;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("ING_DLG") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID") )
		{
			iType = QET_NPC_Ing_Dlg;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("PAYER") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID") )
		{
			iType = QET_NPC_Payer;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("INCPARAM") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / Inc value") )
		{
			iType = QET_NPC_IncParam;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else if( _T("CHECKGOLD") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 6, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / [<,>,==,<=,>=,!=] / Gold / Silver / Bronze / failed Dialog ID") )
		{
			iType = QET_NPC_CheckGold;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kStr = kVec[1];
			iGold = (10000*PgStringUtil::SafeAtoi(kVec[2])) + (100*PgStringUtil::SafeAtoi(kVec[3])) + PgStringUtil::SafeAtoi(kVec[4]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[5]);
		}
	}
	else if( _T("GIVEITEM") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 3, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / ItemNo / Count") )
		{
			iType = QET_NPC_GiveItem;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kItem = SQuestItem(PgStringUtil::SafeAtoi(kVec[1]), PgStringUtil::SafeAtoi(kVec[2]));
		}
	}
	else if( _T("CHECKITEM") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 4, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / ItemNo / Count / failed Dialog ID") )
		{
			iType = QET_NPC_CheckItem;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kItem = SQuestItem(PgStringUtil::SafeAtoi(kVec[1]), PgStringUtil::SafeAtoi(kVec[2]));
			iVal1 = PgStringUtil::SafeAtoi(kVec[3]);
		}
	}
	else if( _T("CHECKEFFECT") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 4, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / EffectNo / Have(1) or NoHave(0) / failed Dialog ID") )
		{
			iType = QET_NPC_CheckEffect;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kItem = SQuestItem(PgStringUtil::SafeAtoi(kVec[1]), PgStringUtil::SafeAtoi(kVec[2]));
			iVal1 = PgStringUtil::SafeAtoi(kVec[3]);
		}
	}
	else if( _T("REMOVEITEM") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 3, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / ItemNo / Count") )
		{
			iType = QET_NPC_RemoveItem;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kItem = SQuestItem(PgStringUtil::SafeAtoi(kVec[1]), PgStringUtil::SafeAtoi(kVec[2]));
		}
	}
	else if( _T("COMPLETEQUEST") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / QuestID") )
		{
			iType = QET_NPC_CompleteQuest;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			iVal1 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else if( _T("ADDGOLD") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 5, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / [+,-] / Gold / Silver / Bronze") )
		{
			iType = QET_NPC_AddGold;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			iGold = (10000*PgStringUtil::SafeAtoi(kVec[2])) + (100*PgStringUtil::SafeAtoi(kVec[3])) + PgStringUtil::SafeAtoi(kVec[4]);
			if( kVec[1] == _T("-") )
			{
				iGold = -iGold;
			}
		}
	}
	else if( _T("GIVEEFFECT") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Dialog ID / EffectNo") )
		{
			iType = QET_NPC_GiveEffect;
			iDialogID = PgStringUtil::SafeAtoi(kVec[0]);
			kItem.iItemNo = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong NPC event TYPE[")) << rkType << _T("], EventNo[") << iEventNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_NPC_Client:
	case QET_NPC_Agent:
	case QET_NPC_Payer:
	case QET_NPC_CheckGold:
	case QET_NPC_GiveItem:
	case QET_NPC_Ing_Dlg:
	case QET_NPC_CheckItem:
	case QET_NPC_RemoveItem:
	case QET_NPC_CompleteQuest:
	case QET_NPC_AddGold:
	case QET_NPC_GiveEffect:
	case QET_NPC_CheckEffect:
		{
			if( PgQuestInfoUtil::VerifyIndexNotZero(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] only = 0, NPC event TYPE[") << rkType << _T("] EventNo[") << iEventNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
			}
		}break;
	case QET_NPC_IncParam:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", NPC event TYPE[") << rkType << _T("] EventNo[") << iEventNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}


/////////////////////////////////////////////////
tagQuestDependMission::tagQuestDependMission()
	:SQuestBasicDepend(0), iMissionKey(-1), iVal1(0), iVal2(0)
{//미션키 -1로 초기화 하는 이유는 0 값이 사용되고 있기 때문. 숙련된용자의길의 미션키가 0이다.
}

tagQuestDependMission::tagQuestDependMission(int const MissionKey, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iMissionKey(MissionKey), iVal1(0), iVal2(0)
{
}

bool tagQuestDependMission::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("RANK") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Mission Level / Rank") )
		{
			iType = QET_MISSION_Rank;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else if( _T("PERCENT") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Clear Percent") )
		{
			iType = QET_MISSION_Percent;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("CLEAR") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Mission Level") )
		{
			iType = QET_MISSION_Clear;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("DEFENCE_STAGE_CLEAR") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Mission Level") )
		{
			iType = QET_MISSION_DEFENCE_CLEAR;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("STRATEGY_DEFENCE_STAGE_CLEAR") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 1, kVec.size(), szElementName, rkType, rkValue, L"Mission Level") )
		{
			iType = QET_MISSION_STRATEGY_DEFENCE_CLEAR;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
		}
	}
	else if( _T("RANKN") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 3, kVec.size(), szElementName, rkType, rkValue, L"Mission Level / Mission Stage Count / Rank") )
		{
			iType = QET_MISSION_RankN;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iStageCount = PgStringUtil::SafeAtoi(kVec[1]);
			iVal2 = PgStringUtil::SafeAtoi(kVec[2]);
		}
	}
	else if( _T("CLEARN") == rkType )
	{
		if( PgQuestInfoVerifyUtil::VerifyEventArgumentCount(rkErrorInfo, 2, kVec.size(), szElementName, rkType, rkValue, L"Mission Level / Mission Stage Count") )
		{
			iType = QET_MISSION_ClearN;
			iVal1 = PgStringUtil::SafeAtoi(kVec[0]);
			iStageCount = PgStringUtil::SafeAtoi(kVec[1]);
		}
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong MISSION event TYPE[")) << rkType << _T("], MissionKey[") << iMissionKey << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_MISSION_Rank:
	case QET_MISSION_RankN:
	case QET_MISSION_Percent:
	case QET_MISSION_Clear:
	case QET_MISSION_ClearN:
	case QET_MISSION_DEFENCE_CLEAR:
	case QET_MISSION_STRATEGY_DEFENCE_CLEAR:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", MISSION event TYPE[") << rkType << _T("] MissionKey[") << iMissionKey << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}


/////////////////////////////////////////////////
tagQuestDependPvp::tagQuestDependPvp()
	:SQuestBasicDepend(0)
{
}

tagQuestDependPvp::tagQuestDependPvp(int const ObjectNo)
	:SQuestBasicDepend(ObjectNo)
{
}

bool tagQuestDependPvp::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("WIN") == rkType )
	{
		iType = QET_PVP_Win;
	}
	else if( _T("LOSE") == rkType )
	{
		iType = QET_PVP_Lose;
	}
	else if( _T("DRAW") == rkType )
	{
		iType = QET_PVP_Draw;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong PVP event TYPE[")) << rkType << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_PVP_Win:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", PVP event TYPE[") << rkType << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}

bool tagQuestDependPvp::Empty() const
{
	return iType == QET_None;
}

/////////////////////////////////////////////////
tagQuestDependCouple::tagQuestDependCouple()
	:SQuestBasicDepend(0), iTime(0)
{
}

tagQuestDependCouple::tagQuestDependCouple(int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iTime(0)
{
}

bool tagQuestDependCouple::Empty() const
{
	return iType == QET_None;
}

/////////////////////////////////////////////////
tagQuestDependClass::tagQuestDependClass()
	:SQuestBasicDepend(0), iClassNo(0)
{
}

tagQuestDependClass::tagQuestDependClass(int const ClassNo, int const ObjectNo)
	:SQuestBasicDepend(ObjectNo), iClassNo(ClassNo)
{
}

bool tagQuestDependClass::Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("CHANGE") == rkType )
	{
		iType = QET_CLASS_Change;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong CLASS event TYPE[")) << rkType << _T("], ClassNo[") << iClassNo << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_CLASS_Change:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", CLASS event TYPE[") << rkType << _T("] ClassNo[") << iClassNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}

/////////////////////////////////////////////////
tagQuestDependKillCount::tagQuestDependKillCount()
	: SQuestBasicDepend(0), iKillCount(0)
{
}
tagQuestDependKillCount::tagQuestDependKillCount(int const KillCount, int const ObjectNo)
	: SQuestBasicDepend(ObjectNo), iKillCount(KillCount)
{
}
bool tagQuestDependKillCount::Build(PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	iType = QET_KillCount;

	if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", KILLCOUNT event KILL[") << iKillCount << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

/////////////////////////////////////////////////
tagQuestDependMyHome::tagQuestDependMyHome()
	: SQuestBasicDepend(0), iCount(0)
{
}
tagQuestDependMyHome::tagQuestDependMyHome(int const Count, int const ObjectNo)
	: SQuestBasicDepend(ObjectNo), iCount(Count)
{
}
bool tagQuestDependMyHome::Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	iType = QET_MYHOME_INVITEUSERCOUNT;

	if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", VISITORCOUNT event [") << iCount << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

/////////////////////////////////////////////////
tagQuestDependGlobal::tagQuestDependGlobal()
	: SQuestBasicDepend(0), iVal1(0), iVal2(0)
{
}

tagQuestDependGlobal::tagQuestDependGlobal( int const iObjectNo )
: SQuestBasicDepend( iObjectNo ), iVal1(0), iVal2(0)
{
}

bool tagQuestDependGlobal::Build( std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName )
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("COMBOCHECK") == rkType )
	{
		iType = QET_GLOBAL_Combo_IncParam;
		iVal1 = PgStringUtil::SafeAtoi(kVec[0]);	// Combo Count
		iVal2 = PgStringUtil::SafeAtoi(kVec[1]);	// Inc Value
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong GLOBAL event TYPE[")) << rkType << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_GLOBAL_Combo_IncParam:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", GLOBAL event TYPE[") << rkType << _T("]" ) );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}

/////////////////////////////////////////////////
tagQuestDependAbil::tagQuestDependAbil()
	: SQuestBasicDepend(0), iAbilType(0), iAbilValue(0)
{
}
tagQuestDependAbil::tagQuestDependAbil(int const AbilType, int const ObjectNo)
	: SQuestBasicDepend(ObjectNo), iAbilType(AbilType), iAbilValue(0)
{
}
bool tagQuestDependAbil::Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName)
{
	VEC_WSTRING kVec;
	PgStringUtil::BreakSep(rkValue, kVec, _T("/"));
	if( _T("GREATER_EQUAL") == rkType )
	{
		iType = QET_ABIL_GREATER_EQUAL;
		iAbilValue = PgStringUtil::SafeAtoi(kVec[0]);
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong PLAYER_ABIL event TYPE[")) << rkType << _T("], AbilType[") << iAbilType << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	switch( iType )
	{
	case QET_ABIL_GREATER_EQUAL:
	default:
		{
			if( PgQuestInfoUtil::VerifyIndex(iObjectNo) )
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong OBJECTNO[")) << iObjectNo+1 << _T("] 1 <= x <= ") << QUEST_PARAMNUM << _T(", PLAYER_ABIL event TYPE[") << rkType << _T("], AbilType[") << iAbilType << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );

				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	}

	return true;
}


/////////////////////////////////////////////////
tagQuestSelect::tagQuestSelect()
{
	Clear();
}

tagQuestSelect::tagQuestSelect(std::wstring const &rkType, int const iID, int const iTTW, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	Clear();
	Set(rkType, iID, iTTW, rkErrorInfo);
}

tagQuestSelect::tagQuestSelect(const EQuestSelectType Type, int const iID, int const iTTW)
{
	Clear();
	Set(Type, iID, iTTW);
}

void tagQuestSelect::Set(std::wstring const &rkType, int const iID, int const iTTW, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	EQuestSelectType eType = QST_Normal;
	if( _T("ACCEPT") == rkType )
	{
		eType = QST_Accept;
	}
	else if( _T("REJECT") == rkType )
	{
		eType = QST_Reject;
	}
	else if( _T("COMPLETE") == rkType )
	{
		eType = QST_Complete;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong Dialog SELECT TYPE[")) << rkType << _T("], SelectID[") << iID << _T("] TTW[") << iTTW << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	Set(eType, iID, iTTW);
}

void tagQuestSelect::Set(const EQuestSelectType Type, int const iID, int const iTTW)
{
	eType = Type;
	iSelectID = iID;
	iSelectTextNo = iTTW;
}

void tagQuestSelect::Clear()
{
	eType = QST_None;
	iSelectID = 0;
	iSelectTextNo = 0;
}


/////////////////////////////////////////////////
tagQuestDialogText::tagQuestDialogText()
{
	Clear();
}

tagQuestDialogText::tagQuestDialogText(int const TextNo)
{
	Clear();
	iTextNo = TextNo;
}

void tagQuestDialogText::Clear()
{
	iTextNo = 0;
	kFaceID.clear();
	iFaceAniID = 0;
	kModelID.clear();
	iStringNameID = 0;
	iTextNameID = 0;
	kStitchImageID.clear();
	kCamPos = POINT3(0.f, 0.f, 0.f);
	iMovieNo = 0;
}


/////////////////////////////////////////////////
tagQuestDialog::tagQuestDialog()
{
	Clear();
}

tagQuestDialog::tagQuestDialog(std::wstring const &rkType, int const iID, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	Clear();
	Set(rkType, iID, rkErrorInfo);
}

tagQuestDialog::tagQuestDialog(const EQuestDialogType Type, int const iID)
{
	Clear();
	Set(Type, iID);
}

void tagQuestDialog::Set(std::wstring const &rkType,  int const iID, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	EQuestDialogType eType = QDT_Normal;
	if( _T("PROLOG") == rkType )
	{
		eType = QDT_Prolog;
	}
	else if( _T("COMPLETE") == rkType )
	{
		eType = QDT_Complete;
	}
	else if( _T("INFO") == rkType )
	{
		eType = QDT_Info;
	}
	else if( _T("MENU") == rkType )
	{
		eType = QDT_Talk_Menu;
	}
	else if( _T("WANTED") == rkType )
	{
		eType = QDT_Wanted;
	}
	else if( _T("WANTED_COMPLETE") == rkType )
	{
		eType = QDT_Wanted_Complete;
	}
	else if( _T("BOARD") == rkType )
	{
		eType = QDT_VillageBoard;
	}
	//else if( _T("BOARD_COMPLETE") == rkType )
	//{
	//	eType = QDT_Village_Complete;
	//}
	else if( _T("MOVIE") == rkType )
	{
		eType = QDT_Movie;
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong DIALOG TYPE[")) << rkType << _T("], DialogID[") << iID << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
	}

	Set(eType, iID);
}

bool tagQuestDialog::Parse(TiXmlElement const* pkDialogNode, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo)
{
	int const iDialogID = PgStringUtil::SafeAtoi(pkDialogNode->Attribute("ID"));
	char const* szDilaogType =  pkDialogNode->Attribute("TYPE");
	if(szDilaogType)
	{
		std::wstring const kType = PgStringUtil::SafeUni(szDilaogType);
		Set(kType, iDialogID, rkErrorInfo);
	}
	else
	{
		Set(QDT_Normal, iDialogID);
	}

	TiXmlElement const* pkChildNode = pkDialogNode->FirstChildElement();
	while(pkChildNode)
	{
		if( !ParseSub(pkChildNode, rkErrorInfo) )
		{
			return false;
		}
		pkChildNode = pkChildNode->NextSiblingElement();
	}
	return true;
}
bool tagQuestDialog::ParseSub(TiXmlElement const* pkChildNode, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, bool const bIsNullPass)
{
	if( !pkChildNode )
	{
		return bIsNullPass;
	}
	char const* szName = pkChildNode->Value();
	if( 0 == strcmp(szName, "BODY") )//Body Text
	{
		int const iDialogTextNo = PgStringUtil::SafeAtoi(pkChildNode->Attribute("TEXT"));
		char const *szFaceID = pkChildNode->Attribute("FACE");
		char const *szFaceAniID = pkChildNode->Attribute("FACE_ANI");
		char const *szCenterModelID = pkChildNode->Attribute("CENTER_MODEL");
		char const *szTextNameID = pkChildNode->Attribute("TEXT_NAME");
		char const *szStringNameID = pkChildNode->Attribute("STRING_NAME");
		char const *szStitchImageID = pkChildNode->Attribute("STITCH_IMAGE_ID");
		char const *szCamPos = pkChildNode->Attribute("CAM_POS");
		int const iDialogMovieNo = PgStringUtil::SafeAtoi(pkChildNode->Attribute("MOVIE_NO"));

		SQuestDialogText kNewDialogText(iDialogTextNo);
		kNewDialogText.kFaceID = PgStringUtil::SafeUni(szFaceID);
		kNewDialogText.iFaceAniID = PgStringUtil::SafeAtoi(szFaceAniID);
		kNewDialogText.kModelID = PgStringUtil::SafeUni(szCenterModelID);
		kNewDialogText.iTextNameID = PgStringUtil::SafeAtoi(szTextNameID);
		kNewDialogText.iStringNameID = PgStringUtil::SafeAtoi(szStringNameID);
		kNewDialogText.kStitchImageID = (szStitchImageID)? UNI(szStitchImageID): std::wstring();
		kNewDialogText.iMovieNo = iDialogMovieNo;
		if( szCamPos )
		{
			VEC_STRING kVec;
			PgStringUtil::BreakSep(std::string(szCamPos), kVec, "/");
			if( 3 == kVec.size() )
			{
				kNewDialogText.kCamPos = POINT3( PgStringUtil::SafeAtof(kVec.at(0)), PgStringUtil::SafeAtof(kVec.at(1)), PgStringUtil::SafeAtof(kVec.at(2)) );
			}
			else
			{
				rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong CAMERA POS X/Y/Z [")) << szCamPos << _T("], DialogID[") << iDialogID << _T("] TextNo[") << iDialogTextNo << _T("]") );
				PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
			}
		}
		AddText(kNewDialogText);
	}
	else if( 0 == strcmp(szName, "SELECT") )//Select
	{
		int const iSelectID = PgStringUtil::SafeAtoi(pkChildNode->Attribute("ID"));
		int const iSelectTextNo = PgStringUtil::SafeAtoi(pkChildNode->Attribute("TEXT"));
		char const* szSelectType = pkChildNode->Attribute("TYPE");

		SQuestSelect kNewSelect;
		if( szSelectType )
		{
			std::wstring const kType = PgStringUtil::SafeUni(szSelectType);
			kNewSelect.Set(kType, iSelectID, iSelectTextNo, rkErrorInfo);
		}
		else
		{
			kNewSelect.Set(QST_Normal, iSelectID, iSelectTextNo);
		}

		AddSelect(kNewSelect);
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseSub( PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChildNode), rkErrorInfo, true );
	}
	else
	{
		rkErrorInfo.Set( PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("wrong xml Element[")) << szName << _T("]") );
		PgQuestInfoVerifyUtil::AddError( rkErrorInfo );
		return false;
	}
	return true;
}

void tagQuestDialog::Set(const EQuestDialogType iType, int const iID)
{
	eType = iType;
	iDialogID = iID;
}

void tagQuestDialog::Clear()
{
	eType = QDT_None;
	iDialogID = 0;
	kDialogText.clear();
	kSelectList.clear();
}

void tagQuestDialog::AddSelect(const SQuestSelect& rkSelect)
{
	kSelectList.push_back(rkSelect);
}

void tagQuestDialog::AddText(const SQuestDialogText& rkQuestText)
{
	kDialogText.push_back(rkQuestText);
}

//
tagQuestDelItem::tagQuestDelItem(int const ItemNo, int const Count, int const ObjectNo, int PetLv)
	: iItemNo(ItemNo), iCount(Count), iObjectNo(ObjectNo), iPetLv(PetLv)
{
}
tagQuestDelItem::~tagQuestDelItem()
{
}
bool tagQuestDelItem::operator == (int const rhs) const
{
	return iItemNo == rhs;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////




/////////////////////////////////////////////////
PgQuestInfo::PgQuestInfo()
{
	Clear();
}

PgQuestInfo::~PgQuestInfo()
{
}

void PgQuestInfo::Clear()
{
	m_kBasic.Clear();
	m_kReward.Clear();

	m_ObjectType = QUEST_OBJ_NOSTEP;
	m_kObject.Clear();					//목표

	m_kLimit.Clear();
	m_kLimit_PreQuestAnd.clear();			//먼저 완료 해야만 하는 퀘스트
	m_kLimit_PreQuestOr.clear();			//먼저 완료 해야만 하는 퀘스트
	m_kLimit_IngQuest.clear();			//진행 중이어야만 하는 퀘스트
	m_kLimit_NotQuest.clear();			//진행 중이거나, 완료하면 안되는 퀘스트
	m_kLimit_ItemHaveAnd.clear();		//AND로 엮인 아이템
	m_kLimit_ItemHaveOr.clear();		//OR로 엮인 아이템
	m_kLimit_Location.clear();	

	m_kNpc_Client.clear();
	m_kNpc_Agent.clear();
	m_kNpc_Payer.clear();

	m_kDepend_Npc.clear();
	m_kDepend_Monster.clear();
	m_kDepend_Item.clear();
	m_kDepend_Location.clear();
	m_kDepend_Mission.clear();
	m_kDepend_Pvp.clear();
	m_kDepend_Couple = SQuestDependCouple(0);
	m_kDepend_Class.clear();
	m_kDepend_Abil.clear();
	m_kDepend_Global.clear();
	m_kDepend_Time = 0;
	m_kDepend_KillCount = SQuestDependKillCount();
	m_kDepend_MyHome = SQuestDependMyHome();

	m_iTitleTextNo = 0;
	m_iGroupTextNo = 0;

	m_iObjectTextNo1 = 0;
	m_iObjectTextNo2 = 0;
	m_iObjectTextNo3 = 0;
	m_iObjectTextNo4 = 0;
	m_iObjectTextNo5 = 0;

	m_kDialog.clear();

	m_kDropDeleteItem.clear();
	m_kCompleteDeleteItem.clear();

	//	이벤트 스크립트 ID 초기화
	m_kAcceptEventScriptID = -1;
	m_kCompleteEventScriptID = -1;
	m_kCompleteNextQuestID = 0;

	m_kXmlFileName.clear();

	m_kIsCanShare = true;
	m_byIsCanRemoteComplete = QRCM_NOT_USE;
	m_bIsCanRemoteAccept = false;
}

void PgQuestInfo::ParseError(TiXmlNode const* pkNode, TCHAR const* szFunc, size_t const iLine)
{
	char const* szName = pkNode->Value();
	int const iRow = pkNode->Row();
	int const iColumn = pkNode->Column();
	int const iQuestID = m_kBasic.iQuestID;
	PgQuestInfoVerifyUtil::AddError( PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_ParseError, szFunc, iLine, BM::vstring(_T("Unknown Element Name:[")) << szName << _T("] Row:[") << iRow << _T("] Column:[") << iColumn << _T("]")) );
}

void PgQuestInfo::Build()
{
	wchar_t const* szCompletQuestHeaderMsg = L"complete quest remove ";
	wchar_t const* szDropQuestHeaderMsg = L"drop quest remove";
	struct SAddCompleteDelItem
	{
		SAddCompleteDelItem(PgQuestInfo const* pkQuestInfo, CONT_DEL_QUEST_ITEM& rkCont, CONT_DEL_QUEST_ITEM::value_type const& rkNewElement)
			: m_pkQuestInfo(pkQuestInfo), m_rkCont(rkCont), m_rkNewElement(rkNewElement)
		{
		}
		void Do(wchar_t const* szHeaderMsg, wchar_t const* szFunc, size_t const iLine)
		{
			if( m_rkNewElement.iItemNo )
			{
				CONT_DEL_QUEST_ITEM::iterator find_iter = std::find(m_rkCont.begin(), m_rkCont.end(), m_rkNewElement.iItemNo);
				if( m_rkCont.end() == find_iter )
				{
					m_rkCont.push_back(m_rkNewElement);
				}
				else
				{
					if( szHeaderMsg )
					{
						if( m_rkNewElement.iCount != (*find_iter).iCount
						||	m_rkNewElement.iObjectNo != (*find_iter).iObjectNo )
						{
							BM::vstring const kErrorMsg = BM::vstring(_T("Duplicate "))<<szHeaderMsg<<" Item[ObjectNo:"<<m_rkNewElement.iObjectNo<<L" ItemNo:"<<m_rkNewElement.iItemNo<<L" Count:"<<m_rkNewElement.iCount<<L"], Duplicate Info[ObjcetNo:"<<(*find_iter).iObjectNo<<L" ItemNo:"<<(*find_iter).iItemNo<<L" Count:"<<(*find_iter).iCount<<L"]";
							PgQuestInfoVerifyUtil::AddError( PgQuestInfoVerifyUtil::SQuestInfoError(m_pkQuestInfo, PgQuestInfoVerifyUtil::ET_ParseError, szFunc, iLine, kErrorMsg) );
						}
					}
				}
			}
		}
	private:
		PgQuestInfo const* m_pkQuestInfo;
		CONT_DEL_QUEST_ITEM& m_rkCont;
		CONT_DEL_QUEST_ITEM::value_type const m_rkNewElement;
	};

	// 관련된 아이템은 완료시에만 삭제
	ContQuestDependItem::iterator item_iter = m_kDepend_Item.begin();
	while(m_kDepend_Item.end() != item_iter)
	{
		ContQuestDependItem::mapped_type const& rkElement = (*item_iter).second;
		int const iMaxCount = GetParamEndCondition(rkElement.iObjectNo);
		if(iMaxCount)
		{
			if(QET_ITEM_Check_Pet == rkElement.iType)
			{
				SAddCompleteDelItem kAddDelItem(this, m_kCompleteDeleteItem, CONT_DEL_QUEST_ITEM::value_type(rkElement.iItemNo, iMaxCount, rkElement.iObjectNo, rkElement.iVal1));
				kAddDelItem.Do( szCompletQuestHeaderMsg, __FUNCTIONW__, __LINE__ );
			}
			else if( QET_ITEM_ChangeCount == rkElement.iType )
			{
				SAddCompleteDelItem kAddDelItem(this, m_kCompleteDeleteItem, CONT_DEL_QUEST_ITEM::value_type(rkElement.iItemNo, iMaxCount, rkElement.iObjectNo));
				kAddDelItem.Do( szCompletQuestHeaderMsg, __FUNCTIONW__, __LINE__ );
			}
		}
		++item_iter;
	}

	// 몬스터가 주는 아이템은 완료시에만 삭제
	ContQuestMonster::iterator monster_iter = m_kDepend_Monster.begin();
	while(m_kDepend_Monster.end() != monster_iter)
	{
		const ContQuestMonster::mapped_type& rkElement = (*monster_iter).second;
		if( QET_MONSTER_InstDrop == rkElement.iType
		||	QET_MONSTER_InstDrop100 == rkElement.iType
		||	QET_MONSTER_InstDrop_M == rkElement.iType
		||  QET_MONSTER_InstDrop100_M == rkElement.iType
		||	QET_MONSTER_InstDrop_In == rkElement.iType
		||	QET_MONSTER_InstDrop_MN == rkElement.iType
		||	QET_MONSTER_InstDrop100_MN == rkElement.iType )
		{
			int const iMaxCount = GetParamEndCondition(rkElement.iObjectNo);
			if( iMaxCount )
			{
				SAddCompleteDelItem kAddDelItem(this, m_kCompleteDeleteItem, CONT_DEL_QUEST_ITEM::value_type(rkElement.iItemNo, iMaxCount, rkElement.iObjectNo));
				kAddDelItem.Do( szCompletQuestHeaderMsg, __FUNCTIONW__, __LINE__ );
			}
		}
		++monster_iter;
	}

	// 몬스터가 주는 아이템은 완료시에만 삭제2
	ContQuestGround::iterator ground_iter = m_kDepend_Ground.begin();
	while( m_kDepend_Ground.end() != ground_iter )
	{
		const ContQuestGround::mapped_type& rkElement = (*ground_iter).second;
		if( QET_GROUND_MonsterDrop == rkElement.iType )
		{
			int const iMaxCount = GetParamEndCondition(rkElement.iObjectNo);
			if( iMaxCount )
			{
				SAddCompleteDelItem kAddDelItem(this, m_kCompleteDeleteItem, CONT_DEL_QUEST_ITEM::value_type( rkElement.iItemNo, iMaxCount, rkElement.iObjectNo ) );
				kAddDelItem.Do( szCompletQuestHeaderMsg, __FUNCTIONW__, __LINE__ );
			}
		}

		++ground_iter;
	}

	// 타입에 의한 파티 공유 제한
	switch( Type() )
	{
	case QT_Random:
	case QT_RandomTactics:
	case QT_Wanted:
	case QT_Scroll:
		{
			m_kIsCanShare = false; // 랜덤 퀘스트는 공유 할 수 없다
		}break;
	default:
		{
			// 제한 없음 (기본 공유 사용)
		}break;
	}

	//모든 퀘스트 원격 가능 여부는 디비에서 제어하도록 수정 됨(2012. 06. 18 - 퀘스트 원격 수락 시스템)
	// 타입에 의한 원격 완료 제한
	switch( Type() )
	{
	case QT_Couple:
	case QT_SweetHeart:
	case QT_Random: // 랜덤 퀘스트는 무조건 가능
	case QT_RandomTactics:
	case QT_Wanted:
	case QT_Scroll:
	case QT_Scenario:	
	case QT_Soul:
	case QT_BattlePass:
		{
			m_byIsCanRemoteComplete = QRCM_USE; // 커플 퀘스트는 원격 완료 가능해
		}break;
	//case QT_ClassChange:
		//{
		//	m_byIsCanRemoteComplete = QRCM_NOT_USE; // 시나리오, 전직 퀘스트는 원격 완료 절대 안되
		//}break;
	default:
		{
		}break;
	}

	// NPC가 주는 아이템은 포기/완료 모두 삭제
	ContQuestDependNpc::iterator npc_iter = m_kDepend_Npc.begin();
	while(m_kDepend_Npc.end() != npc_iter)
	{
		const ContQuestDependNpc::mapped_type& rkElement = (*npc_iter).second;
		switch( rkElement.iType )
		{
		case QET_NPC_GiveItem:
			{
				if( rkElement.kItem.iCount )
				{
					{
						SAddCompleteDelItem kAddDelItem(this, m_kCompleteDeleteItem, CONT_DEL_QUEST_ITEM::value_type(rkElement.kItem.iItemNo, static_cast< int >(rkElement.kItem.iCount), 0));
						kAddDelItem.Do( NULL, __FUNCTIONW__, __LINE__ );
					}
					{
						SAddCompleteDelItem kAddDelItem(this, m_kDropDeleteItem, CONT_DEL_QUEST_ITEM::value_type(rkElement.kItem.iItemNo, static_cast< int >(rkElement.kItem.iCount), 0));
						kAddDelItem.Do( NULL, __FUNCTIONW__, __LINE__ );
					}
				}
			}break;
		default:
			{
			}break;
		}

		//모든 퀘스트 원격 가능 여부는 디비에서 제어하도록 수정 됨(2012. 06. 18 - 퀘스트 원격 수락 시스템)
		//// 이벤트 타입에 의한 원격완료/공유 제한
		//switch( rkElement.iType )
		//{
		//case QET_NPC_GiveItem:
		//case QET_NPC_RemoveItem:
		//	{
		//		if( QRCM_USE == m_byIsCanRemoteComplete )
		//		{
		//			m_byIsCanRemoteComplete = QRCM_NOT_USE; // 아이템 주고, 뺏는 퀘스트는 원격 완료 안되
		//		}
		//	} // no break;
		//case QET_NPC_IncParam:
		//case QET_NPC_CheckGold:
		//case QET_NPC_CheckItem:
		//case QET_NPC_CompleteQuest:
		//case QET_NPC_AddGold:
		//case QET_NPC_CheckEffect:
		//case QET_NPC_GiveEffect:
		//	{
		//		m_kIsCanShare = false;
		//	}break;
		//case QET_NPC_Payer:
		//	{
		//		switch( Type() )
		//		{
		//		case QT_Couple:
		//		case QT_SweetHeart:
		//			{
		//				if( m_byIsCanRemoteComplete )
		//				{
		//					m_byIsCanRemoteComplete = QRCM_NOT_USE; // 하지만 완료 NPC 설정하면 원격 완료 안되
		//				}
		//			}break;
		//		default:
		//			{ // 다른 퀘스트는 문제 없어
		//			}break;
		//		}
		//	}break;
		//default:
		//	{
		//	}break;
		//}
		
		++npc_iter;
	}
}

void PgQuestInfo::BuildFromDef()
{
	//미리 Table.bin이 로드되었다면 여기서 처리 (map server는 따로 Build 한다)
	const CONT_DEF_QUEST_REWARD* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);
	if( !pkQuestReward )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkQuestReward is NULL"));
		return;
	}

	CONT_DEF_QUEST_REWARD::const_iterator reward_iter = pkQuestReward->find(m_kBasic.iQuestID);
	if( pkQuestReward->end() == reward_iter )
	{
		PgQuestInfoVerifyUtil::AddError( PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_DataError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't find QuestID[")) << m_kBasic.iQuestID << _T("], Can't find from TB_DefQuest_Reward")) );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find QuestReward"));
		return;
	}
	
	CONT_DEF_QUEST_REWARD::mapped_type const &rkElement = (*reward_iter).second;
	BuildFromDef(rkElement);
	return;
}

void PgQuestInfo::BuildFromDef(TBL_DEF_QUEST_REWARD const &rkQuestReward)
{
	m_kBasic.iGroupNo = rkQuestReward.iGroupNo;
	m_kBasic.eType = (EQuestType)rkQuestReward.iDBQuestType;
	m_kBasic.iDifficult = rkQuestReward.iDifficult;
	m_kBasic.iOrderIndex = rkQuestReward.iOrderIndex;

	m_kReward.iExp = rkQuestReward.iExp;
	m_kReward.iMoney = rkQuestReward.iGold;
	m_kReward.iTacticsExp = rkQuestReward.iTacticsExp;
	m_kReward.iGuildExp = rkQuestReward.iGuildExp;
	m_kReward.iLevelPer_Exp_1000 = rkQuestReward.iRewardPerLevel_Exp;
	m_kReward.iLevelPer_Money_1000 = rkQuestReward.iRewardPerLevel_Money;

	m_kLimit.iMinLevel = rkQuestReward.iLevelMin;
	m_kLimit.iMaxLevel = rkQuestReward.iLevelMax;
	m_kLimit.iLimitMaxLevel = rkQuestReward.iLimitMaxLevel;
	m_kLimit.iTacticsLevel = rkQuestReward.iTacticsLevel;
	m_kLimit.iClassFlag = rkQuestReward.i64ClassFlag | (rkQuestReward.i64DraClassLimit << DRAGONIAN_LSHIFT_VAL);

	VEC_WSTRING kVecPreQuestAnd;
	VEC_WSTRING kVecPreQuestOr;
	VEC_WSTRING kVecNotQuest;
	
	PgStringUtil::BreakSep(rkQuestReward.kPreQuestAnd, kVecPreQuestAnd, L"/");
	VEC_WSTRING::const_iterator And_iter = kVecPreQuestAnd.begin();
	while( kVecPreQuestAnd.end() != And_iter )
	{
		int const iCurQuestID = PgStringUtil::SafeAtoi( (*And_iter) );
		if( iCurQuestID != 0 )
		{
			m_kLimit_PreQuestAnd.push_back(iCurQuestID);
		}
		
		++And_iter;
	}

	PgStringUtil::BreakSep(rkQuestReward.kPreQuestOr, kVecPreQuestOr, L"/");
	VEC_WSTRING::const_iterator Or_iter = kVecPreQuestOr.begin();
	while( kVecPreQuestOr.end() != Or_iter )
	{
		int const iCurQuestID = PgStringUtil::SafeAtoi( (*Or_iter) );
		if( iCurQuestID != 0 )
		{
			m_kLimit_PreQuestOr.push_back(iCurQuestID);
		}

		++Or_iter;
	}

	PgStringUtil::BreakSep(rkQuestReward.kNotQuest, kVecNotQuest, L"/");
	VEC_WSTRING::const_iterator Not_iter = kVecNotQuest.begin();
	while( kVecNotQuest.end() != Not_iter )
	{
		int const iCurQuestID = PgStringUtil::SafeAtoi( (*Not_iter) );
		if( iCurQuestID != 0 )
		{
			m_kLimit_NotQuest.push_back(iCurQuestID);
		}

		++Not_iter;
	}

	m_kLimit.iMinParty = rkQuestReward.iMinParty;
	m_kLimit.iMaxParty = rkQuestReward.iMaxParty;

	m_byIsCanRemoteComplete = rkQuestReward.byIsCanRemoteComplete;
	m_kIsCanShare = rkQuestReward.bIsCanShare;

	m_iTitleTextNo = rkQuestReward.iTitleText;
	m_iGroupTextNo = rkQuestReward.iGroupName;
	m_bIsCanRemoteAccept = rkQuestReward.bIsCanRemoteAccept;

	m_kIsPartyQuest = m_kLimit.iMinParty > 1;
}


bool PgQuestInfo::ParseXml(TiXmlElement *pkNode, int iQuestId)
{
	if( !pkNode )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kBasic.iQuestID = iQuestId;
	if( !m_kBasic.iQuestID )
	{
		PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError( this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Can't Quest ID = 0")) ));
		return false;
	}

	TiXmlElement *pkChild = pkNode->FirstChildElement();
	while( pkChild )
	{
		char const* szName = pkChild->Value();
		char const* szValue = pkChild->GetText();
		if( !ParseXml_Sub(pkChild) )
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(L"wrong ELEMENT[")<<szName<<L"]" ));
			return false;
		}

		pkChild = pkChild->NextSiblingElement();
	}

	BuildFromDef();

	return true;
}

bool PgQuestInfo::ParseXml_Sub(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return false;
	}

	char const* szName = pkChild->Value();
	char const* szValue = pkChild->GetText();

	bool bRet = true;
	if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Sub(PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true);
	}
	else if( 0 == strcmp(szName, "ID") )
	{
		// do Nothing
	}
	else if( 0 == strcmp(szName, "GROUPNO") )
	{
	}
	else if( 0 == strcmp(szName, "TITLE") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "GROUPNAME") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "LIMITS") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Limit(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "CLIENTS")
		||	0 == strcmp(szName, "AGENTS")
		||	0 == strcmp(szName, "PAYERS") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Npc(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "EVENTS") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Event(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "OBJECTS") )
	{
		TiXmlAttribute const *pAttr = pkChild->FirstAttribute();
		while(pAttr)
		{
			if(strcmp(pAttr->Name(), "TYPE") == 0)
			{
				std::string strValue(pAttr->Value());
				if(strValue == "NOSTEP")
				{
					m_ObjectType = QUEST_OBJ_NOSTEP;
				}
				else if(strValue == "STEPBYSTEP")
				{
					m_ObjectType = QUEST_OBJ_STEPBYSTEP;
				}
			}
			pAttr = pAttr->Next();
		}

		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Object(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "REWARD") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Reward(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "DIALOGS") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_Dialog(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "ACCEPT_EVENT_SCRIPT_ID") )
	{
		m_kAcceptEventScriptID = PgStringUtil::SafeAtoi(szValue);
	}
	else if( 0 == strcmp(szName, "COMPLETE_EVENT_SCRIPT_ID") )
	{
		m_kCompleteEventScriptID = PgStringUtil::SafeAtoi(szValue);
	}
	else if( 0 == strcmp(szName, "COMPLETE_NEXT_QUEST_ID") )
	{
		m_kCompleteNextQuestID = PgStringUtil::SafeAtoi(szValue);
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return bRet;
}

bool PgQuestInfo::ParseXml_Limit(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}

	char const* szName = pkChild->Value();
	bool bRet = true;
	if( 0 == strcmp(szName, "LEVEL") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "PARTY") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "TIME") )
	{
		char const* szStart = pkChild->Attribute("START");
		char const* szEnd = pkChild->Attribute("END");
		SQuestLimitTime kNewTimeLimit(szStart, szEnd, PgQuestInfoVerifyUtil::SQuestInfoError(this));
		m_kLimit.AddTime(kNewTimeLimit);
	}
	else if( 0 == strcmp(szName, "DAYOFWEEK") )
	{
		char const* szText = pkChild->GetText();
		if( NULL == szText )
		{
			return false;
		}

		std::string const kTempStr(pkChild->GetText());
		if( kTempStr.empty() )
		{
			PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Empty DayOfWeek[")) << szText << _T("]") ));
		}
		else
		{
			VEC_STRING kVec;
			PgStringUtil::BreakSep(kTempStr, kVec, "/");

			VEC_STRING::const_iterator loop_iter = kVec.begin();
			while( kVec.end() != loop_iter )
			{
				int iDayOfWeek = PgStringUtil::SafeAtoi((*loop_iter));
				int const iInputStartDayOfWeek = 1;
				int const iInputEndDayOfWeek = 7;
				if( iInputStartDayOfWeek <= iDayOfWeek
				&&	iInputEndDayOfWeek >= iDayOfWeek )
				{
					--iDayOfWeek; // 일요일은 0 ~ 금요일 6
					if( m_kLimit.kDayOfWeek.end() != m_kLimit.kDayOfWeek.find(iDayOfWeek) )
					{
						PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Duplicate DayOfWeek["))<<iDayOfWeek<<L"] In[" << szText << L"]" ));
					}
					m_kLimit.kDayOfWeek.insert(iDayOfWeek);
				}
				else
				{
					PgQuestInfoVerifyUtil::AddError(PgQuestInfoVerifyUtil::SQuestInfoError(this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(_T("Wrong DayOfWeek["))<<iDayOfWeek<<L"] [1~7] In[" << szText << L"]" ));
				}
				++loop_iter;
			}
		}
	}
	else if( 0 == strcmp(szName, "CLASSES") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "ITEMS") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "QUESTS") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "ING_QUESTS") )
	{
		TiXmlElement const* pkSubChild = pkChild->FirstChildElement();
		while(pkSubChild)
		{
			bRet = ParseXml_LimitIngQuest(pkSubChild) && bRet;
			pkSubChild = pkSubChild->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, "NOT_QUESTS") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "LOCATIONS") )
	{
		// 임시
	}
	else if( 0 == strcmp(szName, "PET_LEVEL") )
	{
		m_kLimit.iPetLevelMin = PgStringUtil::SafeAtoi(pkChild->Attribute("MIN"));
		m_kLimit.iPetLevelMax = PgStringUtil::SafeAtoi(pkChild->Attribute("MAX"));
	}
	else if( 0 == strcmp(szName, "PET_CLASS") )
	{
		TiXmlAttribute const *pkAttr = pkChild->FirstAttribute();
		while(pkAttr)
		{
			if(strcmp(pkAttr->Name(), "TYPE") == 0)
			{
				VEC_STRING kVec;
				PgStringUtil::BreakSep(pkAttr->Value(), kVec, "/");
				for(VEC_STRING::const_iterator iterStr = kVec.begin(); iterStr != kVec.end(); ++iterStr)
				{
					m_kLimit.kPetType.push_back( PgStringUtil::SafeAtoi(*iterStr) );
				}
			}
			else if(strcmp(pkAttr->Name(), "LEVEL") == 0)
			{
				VEC_STRING kVec;
				PgStringUtil::BreakSep(pkAttr->Value(), kVec, "/");
				for(VEC_STRING::const_iterator iterStr = kVec.begin(); iterStr != kVec.end(); ++iterStr)
				{
					m_kLimit.kPetClassGrade.push_back( PgStringUtil::SafeAtoi(*iterStr) );
				}
			}
			else if(strcmp(pkAttr->Name(), "CASH_TYPE") == 0)
			{
				std::string strValue(pkAttr->Value());
				if(strValue == "INGAME")
				{
					m_kLimit.ePetCashType = CT_Ingame;
				}
				else if(strValue == "CASH")
				{
					m_kLimit.ePetCashType = CT_Cash;
				}
				else if(strValue == "ALL")
				{
					m_kLimit.ePetCashType = CT_All;
				}
			}

			pkAttr = pkAttr->Next();
		}
	}
	else if( 0 == strcmp(szName, "SKILLS") )
	{
		TiXmlElement const *pkNode = pkChild->FirstChildElement();
		while(pkNode)
		{
			TiXmlAttribute const* pAttr = pkNode->FirstAttribute();
			if( pAttr )
			{
				if( strcmp(pAttr->Value(), "AND") == 0 )
				{
					VEC_STRING kVec;
					PgStringUtil::BreakSep(pkNode->GetText(), kVec, "/");
					for(VEC_STRING::const_iterator iterStr = kVec.begin(); iterStr != kVec.end(); ++iterStr)
					{
						m_kLimit.kSkillAndLimit.push_back(PgStringUtil::SafeAtoi(*iterStr) );
					}
				}
				else if( strcmp(pAttr->Value(), "OR") == 0 )
				{
					VEC_STRING kVec;
					PgStringUtil::BreakSep(pkNode->GetText(), kVec, "/");
					for(VEC_STRING::const_iterator iterStr = kVec.begin(); iterStr != kVec.end(); ++iterStr)
					{
						m_kLimit.kSkillOrLimit.push_back(PgStringUtil::SafeAtoi(*iterStr) );
					}
				}
			}
			pkNode = pkNode->NextSiblingElement();
		}
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Limit( PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild) );
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !bRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//실패
	}
	return true;
}

bool PgQuestInfo::ParseXml_Event(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}

	char const* szName = pkChild->Value();

	int const iObjectNo = PgQuestInfoUtil::ConvertIndex( PgStringUtil::SafeAtoi(pkChild->Attribute("OBJECTNO")) ); // -1, 0~4
	int const iNo = PgStringUtil::SafeAtoi(pkChild->GetText());
	std::wstring const kType = PgStringUtil::SafeUni(pkChild->Attribute("TYPE"));
	std::wstring const kValue = PgStringUtil::SafeUni(pkChild->Attribute("VALUE"));

	if( 0 == strcmp(szName, "MONSTER") )
	{
		ContQuestMonster::mapped_type kMonster(iNo, iObjectNo);
		kMonster.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);

		if( _T("INCPARAM100_IN") == kType || 
			_T("INSTDROP100_IN") == kType || 
			_T("INCPARAM100") == kType ||	
			_T("INSTDROP100") == kType || 
			_T("INSTDROP100_M") == kType || 
			_T("INCPARAM100_M") == kType || 
			_T("INSTDROP100_MN") == kType|| 
			_T("INCPARAM100_MN") == kType)
		{
			for( int i = 0; i < 100; ++i )
			{
				int const iClassNo = iNo + i;
				kMonster.iClassNo = iClassNo;
				m_kDepend_Monster.insert( std::make_pair( iNo + i, kMonster ) );
			}
		}
		else
		{
			m_kDepend_Monster.insert( std::make_pair(iNo, kMonster) );
		}
	}
	else if( 0 == strcmp(szName, "GROUND") )
	{
		ContQuestGround::mapped_type kGround(iNo, iObjectNo);
		kGround.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_Ground.insert( std::make_pair(iNo, kGround) );
	}
	else if( 0 == strcmp(szName, "ITEM") )
	{
		ContQuestDependItem::mapped_type kItem(iNo, iObjectNo);
		kItem.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_Item.insert( std::make_pair(iNo, kItem) );
	}
	else if( 0 == strcmp(szName, "LOCATION") )
	{
		ContQuestLocation::value_type kLocation(iNo, iObjectNo);
		kLocation.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_Location.push_back(kLocation);
	}
	else if( 0 == strcmp(szName, "TIME") )
	{
		m_kDepend_Time = iNo; // n분에서 __int64초로 변환
	}
	else if( 0 == strcmp(szName, "KILLCOUNT") )
	{
		SQuestDependKillCount kKillCount(iNo, iObjectNo);
		kKillCount.Build(PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_KillCount = kKillCount;
	}
	else if( 0 == strcmp(szName, "MYHOME") )
	{
		SQuestDependMyHome kMyHome(iNo, iObjectNo);
		kMyHome.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_MyHome = kMyHome;
	}
	else if( 0 == strcmp(szName, "NPC") )
	{
		ContQuestDependNpc::mapped_type kNpc(iNo, iObjectNo);
		if( kNpc.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName) )
		{
			m_kDepend_Npc.insert( std::make_pair(iNo,  kNpc) );
			//이건 Client/Agent/Payer에서 다 된다
		}
	}
	else if( 0 == strcmp(szName, "MISSION") )
	{
		ContQuestDependMission::value_type kMission(iNo, iObjectNo);
		kMission.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);

		m_kDepend_Mission.push_back(kMission);
	}
	else if( 0 == strcmp(szName, "PVP") )
	{
		ContQuestDependPvp::value_type kTemp(iObjectNo);
		if( kTemp.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName) )
		{
			m_kDepend_Pvp.push_back( kTemp );
		}
	}
	else if( 0 == strcmp(szName, "COUPLE_PLAYTIME") )
	{
		m_kDepend_Couple.iObjectNo = iObjectNo;
		m_kDepend_Couple.iTime = iNo;
		//m_kDepend_Couple.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this));
	}
	else if( 0 == strcmp(szName, "CLASS") )
	{
		ContQuestDependClass::value_type kClass(iNo, iObjectNo);
		kClass.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_Class.push_back(kClass);
	}
	else if( 0 == strcmp(szName, "ABIL") )
	{
		ContQuestDependAbil::value_type kAbil(iNo, iObjectNo);
		kAbil.Build(kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName);
		m_kDepend_Abil.push_back( kAbil );
	}
	else if( 0 == strcmp(szName, "GLOBAL") )
	{
		ContQuestDependGlobal::value_type kGlobal( iObjectNo );
		kGlobal.Build( kType, kValue, PgQuestInfoVerifyUtil::SQuestInfoError(this), szName );
		m_kDepend_Global.push_back( kGlobal );
	}
	else if( 0 == strcmp( szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME ) )
	{
		TiXmlNode const* pkFindLocalNode = PgXmlLocalUtil::FindInLocal( g_kLocal, pkChild, "PgQuestInfo" );
		if( pkFindLocalNode )
		{
			TiXmlElement const* pkResultElement = pkFindLocalNode->FirstChildElement();
			if( pkResultElement )
			{	
				while(pkResultElement)
				{
					ParseXml_Event(pkResultElement);
					pkResultElement = pkResultElement->NextSiblingElement();
				}
			}
		}
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

void PgQuestInfo::AddNpcEvent(ContQuestNpc& rkDest, TiXmlElement const* pkChild, char const* szDefaultMark)
{
	if( szDefaultMark
	&&	pkChild )
	{
		int const iEventNo = PgStringUtil::SafeAtoi(pkChild->Attribute("EVENTNO"));
		char const* szText = pkChild->GetText();
		char const* szMark = pkChild->Attribute("MARK");
		BM::GUID kGuid;
		if( szText )
		{
			kGuid = BM::GUID(PgStringUtil::SafeUni(szText));
		}
		if( !szMark )
		{
			szMark = szDefaultMark;
		}

		SQuestNpc kNewEvent(kGuid, iEventNo, szMark, PgQuestInfoVerifyUtil::SQuestInfoError(this));
		rkDest.push_back(kNewEvent);
	}
}
bool PgQuestInfo::ParseXml_Npc(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}

	char const* szName = pkChild->Value();
	if( 0 == strcmp(szName, "CLIENT") )
	{
		AddNpcEvent(m_kNpc_Client, pkChild, "BEGIN");
	}
	else if( 0 == strcmp(szName, "AGENT") )
	{
		AddNpcEvent(m_kNpc_Agent, pkChild, "ING");
	}
	else if( 0 == strcmp(szName, "PAYER") )
	{
		AddNpcEvent(m_kNpc_Payer, pkChild, "END");
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Npc( PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true );
	}
	else
	{
		PgQuestInfoVerifyUtil::AddError( PgQuestInfoVerifyUtil::SQuestInfoError( this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong NPC Event Name[")<<szName<<L"]") );
		return false;
	}
	return true;
}

bool PgQuestInfo::ParseXml_Object(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if(!pkChild)
	{
		return bIsNullPass;
	}
	char const* szName = pkChild->Value();
	int const iCount = PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT"));
	int const iObjectText = PgStringUtil::SafeAtoi(pkChild->Attribute("TEXT"));

	if( 0 == strcmp(szName, "OBJECT1") )
	{
		m_iObjectTextNo1 = iObjectText;
		m_kObject.iObjectCount[0] = iCount;
	}
	else if( 0 == strcmp(szName, "OBJECT2") )
	{
		m_iObjectTextNo2 = iObjectText;
		m_kObject.iObjectCount[1] = iCount;
	}
	else if( 0 == strcmp(szName, "OBJECT3") )
	{
		m_iObjectTextNo3 = iObjectText;
		m_kObject.iObjectCount[2] = iCount;
	}
	else if( 0 == strcmp(szName, "OBJECT4") )
	{
		m_iObjectTextNo4 = iObjectText;
		m_kObject.iObjectCount[3] = iCount;
	}
	else if( 0 == strcmp(szName, "OBJECT5") )
	{
		m_iObjectTextNo5 = iObjectText;
		m_kObject.iObjectCount[4] = iCount;
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Object( PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true );
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( 0 > iCount
	||	UCHAR_MAX < iCount )
	{
		PgQuestInfoVerifyUtil::AddError( PgQuestInfoVerifyUtil::SQuestInfoError( this, PgQuestInfoVerifyUtil::ET_ParseError, __FUNCTIONW__, __LINE__, BM::vstring(L"Wrong Object[")<<szName<<L"] Count["<<iCount<<L"] can use the [0~255] value ") );
	}

	return true;
}

bool PgQuestInfo::ParseXml_LimitIngQuest(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}
	char const* szName = pkChild->Value();
	if( 0 == strcmp(szName, "QUEST") )
	{
		int const iQuestID = PgStringUtil::SafeAtoi(pkChild->GetText());
		if( iQuestID )
		{
			m_kLimit_IngQuest.push_back(iQuestID);
		}
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_LimitIngQuest(PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true);
	}
	else
	{
		return false;
	}
	return true;
}

bool PgQuestInfo::ParseXml_Reward(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}
	char const* szName = pkChild->Value();
	if( 0 == strcmp(szName, "SELECT1") )
	{
		int const iSetNo = 1;
		SQuestRewardSet kNewRewardSet;
		kNewRewardSet.bSelective = (0 != PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT")));
		TiXmlElement const* pkNode = pkChild->FirstChildElement();
		while(pkNode)
		{
			if( !ParseXml_Reward_Set(pkNode, kNewRewardSet) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			pkNode = pkNode->NextSiblingElement();
		}
		m_kReward.AddRewardSet(iSetNo, kNewRewardSet, PgQuestInfoVerifyUtil::SQuestInfoError(this));
	}
	else if( 0 == strcmp(szName, "SELECT2") )
	{
		int const iSetNo = 2;
		SQuestRewardSet kNewRewardSet;
		kNewRewardSet.bSelective = (0 != PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT")));
		TiXmlElement const* pkNode = pkChild->FirstChildElement();
		while(pkNode)
		{
			if( !ParseXml_Reward_Set(pkNode, kNewRewardSet) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			pkNode = pkNode->NextSiblingElement();
		}
		m_kReward.AddRewardSet(iSetNo, kNewRewardSet, PgQuestInfoVerifyUtil::SQuestInfoError(this));
	}
	else if( 0 == strcmp(szName, "EFFECT") )
	{
		int const iEffectNo = PgStringUtil::SafeAtoi(pkChild->GetText());
		if( iEffectNo
		&&	m_kReward.kEffect.end() == std::find(m_kReward.kEffect.begin(), m_kReward.kEffect.end(), iEffectNo) )
		{
			std::back_inserter(m_kReward.kEffect) = iEffectNo;
		}
	}
	else if( 0 == strcmp(szName, "ADD_END_QUEST") )
	{
		ContQuestID& rkVec = m_kReward.kAddEndQuest;
		int const iAddEndQuestID = PgStringUtil::SafeAtoi(pkChild->GetText());
		if( 0 != iAddEndQuestID
		&&	rkVec.end() == std::find(rkVec.begin(), rkVec.end(), iAddEndQuestID) )
		{
			std::back_inserter(rkVec) = iAddEndQuestID;
		}
	}
	else if( 0 == strcmp(szName, "CHANGECLASS") )//전직
	{
		int const iClassNo = PgStringUtil::SafeAtoi(pkChild->GetText());
		int const iPrevClass = PgStringUtil::SafeAtoi(pkChild->Attribute("PREVCLASS"));
		int const iMinLevel = PgStringUtil::SafeAtoi(pkChild->Attribute("MINLEVEL"));
		m_kReward.kChangeClass = SChangeClassInfo(iClassNo, iPrevClass, iMinLevel);
		m_kBasic.eType = QT_ClassChange;//전직 퀘스트로 형태 변화
	}
	else if( 0 == strcmp(szName, "LEARN_SKILL") )
	{
		int const iSkillNo = PgStringUtil::SafeAtoi(pkChild->GetText());
		m_kReward.iSkillNo = iSkillNo;
	}
	else if( 0 == strcmp(szName, "REALM_QUEST_ID") )
	{
		m_kReward.kRealmQuetID = PgStringUtil::SafeUni(pkChild->GetText());
	}
	else if( 0 == strcmp(szName, "EXP") )	{ /* none (DB값 사용) */ }
	else if( 0 == strcmp(szName, "GOLD") )	{ /* none (DB값 사용) */ }
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Reward(PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true);
	}
	else if( 0 == strcmp(szName, "CHANGE_LEVEL") )	
	{ 
		int const iChangeLevel = PgStringUtil::SafeAtoi(pkChild->GetText());
		m_kReward.iChangeLevel = iChangeLevel;
	}
	else if ( 0 == strcmp(szName, "UNLOCK_CHARACTERS") )
	{
		m_kReward.kUnlockCharacters = true;
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		return false;
	}
	return true;
}

bool PgQuestInfo::ParseXml_Reward_Set(TiXmlElement const* pkChild, SQuestRewardSet& rkRewardSet, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}

	char const* szName = pkChild->Value();
	if( 0 == strcmp(szName, "ITEM") )
	{
		int const iItemNo = PgStringUtil::SafeAtoi(pkChild->GetText());
		int const iCount = PgStringUtil::SafeAtoi(pkChild->Attribute("COUNT"));
		int const iRarityType = PgStringUtil::SafeAtoi(pkChild->Attribute("RARITY"));
		bool const bSeal = PgStringUtil::SafeAtob(pkChild->Attribute("SEAL"));
		EUseItemType const eTimeType = static_cast< EUseItemType >(PgStringUtil::SafeAtoi(pkChild->Attribute("TIMETYPE")));
		int const iTimeValue = PgStringUtil::SafeAtoi(pkChild->Attribute("TIME"));

		SQuestItem const kItem(iItemNo, iCount, iRarityType, bSeal, eTimeType, iTimeValue);
		if( !rkRewardSet.AddRewardItem(kItem) )
		{
			ParseError(pkChild, __FUNCTIONW__, __LINE__);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	else if( 0 == strcmp(szName, "CLASS") )
	{
		typedef std::list< std::string > Contstr;
		Contstr kList;
		BM::vstring::CutTextByKey(std::string(pkChild->GetText()), std::string("/"), kList);
		
		Contstr::const_iterator it = kList.begin();
		for(;it!=kList.end();++it)
		{
			int const iClassNo = PgStringUtil::SafeAtoi((*it));

			if( !rkRewardSet.AddClass(iClassNo) )
			{
				ParseError(pkChild, __FUNCTIONW__, __LINE__);
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}
	else if( 0 == strcmp(szName, "GENDER") )
	{
		//int const iClassNo = PgStringUtil::SafeAtoi(pkGenderNode->GetText());
		if( 0 != rkRewardSet.iGender)
		{
			ParseError(pkChild, __FUNCTIONW__, __LINE__);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( 0 == strcmp(pkChild->GetText(), "FEMALE") )
		{
			rkRewardSet.iGender = UG_FEMALE;
		}
		else if( 0 == strcmp(pkChild->GetText(), "MALE") )
		{
			rkRewardSet.iGender = UG_MALE;
		}
		else
		{
			ParseError(pkChild, __FUNCTIONW__, __LINE__);
			return false;
		}
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Reward_Set(PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), rkRewardSet, true);
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		return false;
	}
	return true;
}

bool PgQuestInfo::ParseXml_Dialog(TiXmlElement const* pkChild, bool const bIsNullPass)
{
	if( !pkChild )
	{
		return bIsNullPass;
	}

	char const* szName = pkChild->Value();
	if( 0 == strcmp(szName, "DIALOG") )
	{
		SQuestDialog kNewQuestDialog;
		if( !kNewQuestDialog.Parse(pkChild, PgQuestInfoVerifyUtil::SQuestInfoError(this)) )
		{
			ParseError( pkChild , __FUNCTIONW__, __LINE__);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		m_kDialog.insert(std::make_pair(kNewQuestDialog.iDialogID, kNewQuestDialog));//삽입
	}
	else if( 0 == strcmp(szName, PgXmlLocalUtil::LOCAL_ELEMENT_NAME) )
	{
		return ParseXml_Dialog(PgXmlLocalUtil::FindInLocalResult(g_kLocal, pkChild), true);
	}
	else
	{
		ParseError(pkChild, __FUNCTIONW__, __LINE__);
		return false;
	}
	return true;
}

int PgQuestInfo::GetParamEndCondition(int iParamNo) const
{
	return m_kObject.GetEndCount(iParamNo);
}

int PgQuestInfo::GetObjectText(int iParamNo)const
{
	int iRetTextNo = 0;
	switch(iParamNo)
	{
	case 0:		{ iRetTextNo = m_iObjectTextNo1; }break;
	case 1:		{ iRetTextNo = m_iObjectTextNo2; }break;
	case 2:		{ iRetTextNo = m_iObjectTextNo3; }break;
	case 3:		{ iRetTextNo = m_iObjectTextNo4; }break;
	case 4:		{ iRetTextNo = m_iObjectTextNo5; }break;
	default:
		{
		}break;
	}
	return iRetTextNo;
}



bool PgQuestInfo::GetDialog(int const iDialogID, SQuestDialog const *& pkOut) const
{
	ContQuestDialog::const_iterator iter = m_kDialog.find(iDialogID);
	if( m_kDialog.end() != iter )
	{
		pkOut = &(*iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgQuestInfo::FindDialog(int const iDialogID) const
{
	const SQuestDialog* pkQuestDialog = NULL;
	return GetDialog(iDialogID, pkQuestDialog);
}

bool PgQuestInfo::GetInfoDialog(const SQuestDialog*& pkOut) const
{
	ContQuestDialog::const_iterator iter = m_kDialog.begin();
	while(m_kDialog.end() != iter)
	{
		const ContQuestDialog::mapped_type &rkElement = (*iter).second;
		if( QDT_Info == rkElement.eType )
		{
			pkOut = &rkElement;
			return true;
		}

		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int PgQuestInfo::FindNpcEventNo(BM::GUID const& rkGuid, EQuestState const eState) const
{
	switch( eState )
	{
	case QS_Begin:
		{
			return PgQuestInfoUtil::FindEventNo(m_kNpc_Client, rkGuid);
		}break;
	case QS_Ing:
		{
			return PgQuestInfoUtil::FindEventNo(m_kNpc_Agent, rkGuid);
		}break;
	case QS_End:
		{
			return PgQuestInfoUtil::FindEventNo(m_kNpc_Payer, rkGuid);
		}break;
	default:
		{
		}break;
	}
	return 0;
}

int PgQuestInfo::GetNpcDialog(int const iEventNo, const EQuestState eState) const
{
	ContQuestDependNpc::const_iterator iter = m_kDepend_Npc.find(iEventNo);
	while(m_kDepend_Npc.end() != iter && (*iter).first == iEventNo)
	{
		const ContQuestDependNpc::mapped_type& rkElement = (*iter).second;
		EQuestEventType eType = QET_None;
		switch(eState)
		{
		case QS_Begin:
			{
				eType = QET_NPC_Client;
			}break;
		case QS_Ing:
			{
				eType = QET_NPC_Ing_Dlg;
			}break;
		case QS_End:
			{
				eType = QET_NPC_Payer;
			}break;
		default:
			{
			}break;
		}

		if( eType != QET_None
		&&	eType == rkElement.iType
		&&	iEventNo == rkElement.iEventNo )
		{
			return rkElement.iDialogID;
		}

		++iter;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}
bool PgQuestInfo::GetFirstPayerNpc(BM::GUID& rkOut) const
{
	if( !m_kNpc_Payer.empty() )
	{
		ContQuestNpc::value_type const& rkNpcQuest = (*m_kNpc_Payer.begin());
		rkOut = rkNpcQuest.kNpcGuid;
		return true;
	}
	return false;
}
bool PgQuestInfo::GetFirstClientNpc(BM::GUID& rkOut) const
{
	if( !m_kNpc_Client.empty() )
	{
		ContQuestNpc::value_type const& rkNpcQuest = (*m_kNpc_Client.begin());
		rkOut = rkNpcQuest.kNpcGuid;
		return true;
	}
	return false;
}