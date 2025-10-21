#include "stdafx.h"
#include "lohengrin/DBTables.h"
#include "variant/constant.h"
#include "variant/item.h"
#include "variant/MonsterDefMgr.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/pgitemrarityupgradeformula.h"
#include "Global.h"
#include "constant.h"
#include "PgAction.h"
#include "PgQuest.h"
#include "PgGround.h"
#include "PgGroundMgr.h"
#include "PgActionQuest.h"
#include "PgAction.h"

namespace PgActionQuestUtil
{
	void AfterBeginCheckQuest(CUnit* pkCaster, int const iQuestID, SGroundKey const& rkGndKey, int const iMissionStageCount)
	{
		PgQuestInfo const* pkQuestInfo = NULL;
		if( !g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
		{
			return ;
		}

		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
		if( !pkCaster )
		{
			return ;
		}

		PgInventory* pkInven = pkPlayer->GetInven();
		if( !pkInven )
		{
			return ;
		}

		CONT_PLAYER_MODIFY_ORDER kOrder;
		//시작 후 처리
		PgCheckQuestComplete kCheckComplete(iQuestID, kOrder, NULL);
		bool const bCanComplete = kCheckComplete.DoAction(pkCaster, NULL);
		if( !bCanComplete )
		{
			ContQuestDependItem const& rkDependItem = pkQuestInfo->m_kDepend_Item;
			ContQuestDependItem::const_iterator item_iter = rkDependItem.begin();
			while(rkDependItem.end() != item_iter)
			{
				ContQuestDependItem::mapped_type const& rkElement = (*item_iter).second;

				if( (QET_ITEM_ChangeCount == rkElement.iType)
					|| (QET_ITEM_NotChangeCount == rkElement.iType) )
				{
					size_t const iCurItemCount = pkInven->GetTotalCount(rkElement.iItemNo);
					if( iCurItemCount )
					{
						PgAction_SetQuestParam kAction_SetParamCount(rkGndKey, iQuestID, (BYTE)rkElement.iObjectNo, iCurItemCount);
						kAction_SetParamCount.DoAction(pkCaster, NULL);
					}
				}
				else if( QET_ITEM_Check_Pet == rkElement.iType )
				{
					PgBase_Item kPetItem;
					if(true == PgQuestActionUtil::GetQuestPetFromInv(kPetItem, pkPlayer, rkElement.iVal1, rkElement.iItemNo, true))
					{
						kOrder.clear();
						SPMOD_AddIngQuest kAddQuestData(iQuestID, QS_End);
						kOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkCaster->GetID(), kAddQuestData) );

						SPMOD_IngQuestParam kQuestParam(iQuestID, (BYTE)rkElement.iObjectNo, (BYTE)pkQuestInfo->GetParamEndCondition(rkElement.iObjectNo), (BYTE)pkQuestInfo->GetParamEndCondition(rkElement.iObjectNo), true);
						kOrder.push_back( SPMO(IMET_ADD_INGQUEST_PARAM, pkCaster->GetID(), kQuestParam) );

						PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, rkGndKey, kOrder);
						kItemModifyAction.DoAction(pkCaster, NULL);
					}
				}
				else if ( QET_ITEM_Plus_Check == rkElement.iType )
				{
					SItemPos kItemPos;
					if (pkInven->GetFirstItem(rkElement.iItemNo, kItemPos) == S_OK)
					{
						PgBase_Item kItem;
						int iMaxPlusLevel = -1;
						do
						{
							if ( pkInven->GetItem(kItemPos, kItem) == S_OK )
								iMaxPlusLevel = __max(iMaxPlusLevel, kItem.EnchantInfo().PlusLv());
						} while( pkInven->GetNextItem(rkElement.iItemNo, kItemPos) == S_OK );

						if (iMaxPlusLevel >= 0)
						{
							PgAction_SetQuestParam kSetParam(rkGndKey, iQuestID, rkElement.iObjectNo, iMaxPlusLevel);
							kSetParam.DoAction(pkCaster, NULL);
						}
					}
				}
				++item_iter;
			}
		}

		if( !bCanComplete )
		{
			typedef std::set< int > ContCheckedMissionKey;
			//미션 체크
			const ContQuestDependMission& rkQuestMission = pkQuestInfo->m_kDepend_Mission;
			ContQuestDependMission::const_iterator mission_iter = rkQuestMission.begin();
			//int iCurMissionKey = 0;
			ContCheckedMissionKey kCheckSet;
			while(rkQuestMission.end() != mission_iter)
			{
				const ContQuestDependMission::value_type& rkElement = (*mission_iter);
				switch( rkElement.iType )
				{
				case QET_MISSION_Percent:
					{
						if( kCheckSet.end() == kCheckSet.find(rkElement.iMissionKey) )
						{
							PgAction_QuestMissionUtil::ProcessMissionPercent(rkGndKey, pkQuestInfo->ID(), rkElement, pkPlayer);
							kCheckSet.insert(rkElement.iMissionKey);
						}
					}break;
				case QET_MISSION_Clear:
					{
						PgAction_QuestMissionUtil::ProcessMissionClear(rkGndKey, pkQuestInfo->ID(), rkElement, pkPlayer, rkElement.iVal1, MRANK_NONE);
					}break;
				case QET_MISSION_ClearN:
					{
						PgAction_QuestMissionUtil::ProcessMissionClearN(rkGndKey, pkQuestInfo->ID(), rkElement, pkPlayer, rkElement.iVal1, MRANK_NONE, iMissionStageCount);
					}break;
				case QET_MISSION_Rank:
				case QET_MISSION_RankN:
				case QET_MISSION_DEFENCE_CLEAR:
				case QET_MISSION_STRATEGY_DEFENCE_CLEAR:
				default:
					{
						// do nothing
					}break;
				}
				++mission_iter;
			}

			//전직 체크
			PgAction_ChangeClassCheck kChangeClassCheck(rkGndKey);
			kChangeClassCheck.DoAction(pkCaster, NULL);
		}
		if( !bCanComplete )
		{
			PgAction_QuestKillCount kAction(rkGndKey, pkCaster->GetAbil(AT_MONSTER_KILL_COUNT));
			kAction.DoAction(pkCaster, NULL);
		}
		if( !bCanComplete )
		{
			PgAction_QuestAbil kQuestAction(rkGndKey);
			kQuestAction.DoAction(pkCaster, NULL);
		}

		if( !kOrder.empty() )
		{
			PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, rkGndKey, kOrder);
			kItemModifyAction.DoAction(pkCaster, NULL);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//		PgCheckQuest : how many him(npc) have a can begin quest
//////////////////////////////////////////////////////////////////////////////
PgCheckQuest::PgCheckQuest(ContSimpleQuest &rkOutVec)
	:m_kQuestVector(rkOutVec)
{
}

bool PgCheckQuest::DoAction(CUnit* pkUser, CUnit* pkTargetUnit)
{
	if( !pkUser )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"CUnit* pkUser is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkTargetUnit )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"CUnit* pkTargetUnit is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't PLAYER Unit, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgNpc* pkNpc = dynamic_cast<PgNpc*>(pkTargetUnit);
	if( !pkNpc )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't NPC Unit, GUID: " << pkTargetUnit->GetID() << L", Name: " << pkTargetUnit->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kQuestMan.CheckQuest(pkPC, pkTargetUnit->GetID(), m_kQuestVector);
	
	return !m_kQuestVector.empty();
}

//////////////////////////////////////////////////////////////////////////////
//		PgCheckQuestBegin : Can i begin this quest ?
//////////////////////////////////////////////////////////////////////////////
PgCheckQuestBegin::PgCheckQuestBegin(int const iQuestID)
	:m_iQuestID(iQuestID), m_kLimit(QBL_None)
{
}

bool PgCheckQuestBegin::DoAction(CUnit* pkUser, CUnit* Nothing)
{
	// 퀘스트 시작 조건에 맞는지 검사
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Somthing wrong");
	}

	if( !pkUser )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPlayer )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't PLAYER Unit, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	bool const bFindQuest = g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo);
	if( !bFindQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't find Quest ID[" << m_iQuestID << L"] Info");
		m_kLimit = QBL_Error;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	m_kMaxLevelLimit = pkQuestInfo->m_kLimit.iMaxLevel;//최대 레벨 기억
	m_kLimit = PgQuestInfoUtil::CheckBeginQuest(const_cast<PgPlayer const *>(pkPlayer), m_iQuestID, pkQuestInfo);

	switch( m_kLimit )
	{
	case QBL_None:
		{
		}break;
	case QBL_Level:
		{	
			if( QT_RandomTactics != pkQuestInfo->Type() 
			 && QT_Random != pkQuestInfo->Type() 
			 && QT_Wanted != pkQuestInfo->Type() )
			{//일일퀘스트,랜덤퀘스트는 레벨 체크 하지 말자...
				return false;
			}
		}break;
	case QBL_Error:
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << L"is Pointer is NULL, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		}//No Break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgCheckQuestComplete : Can I finish Quest ?
//////////////////////////////////////////////////////////////////////////////
PgCheckQuestComplete::PgCheckQuestComplete(int const iQuestID, CONT_PLAYER_MODIFY_ORDER &rkOrder, SUserQuestState const *pkNextState)
	:m_iQuestID(iQuestID), m_kOrder(rkOrder), m_pkNextState(pkNextState)
{
}

bool PgCheckQuestComplete::DoAction(CUnit* pkUser, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Somthing wrong");
	}

	if( !pkUser )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't Player Unit, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"is PgMyQuest Pointer is NULL, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
	if( !pkState )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( m_pkNextState )
	{
		pkState = m_pkNextState;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	bool const bFindQuest = g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo);
	if( !bFindQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't find Quest ID[" << m_iQuestID << L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return DoAction(pkUser->GetID(), pkState, pkQuestInfo, m_kOrder);
}

bool PgCheckQuestComplete::DoAction(BM::GUID const &rkCharGuid, SUserQuestState const *pkState, PgQuestInfo const *pkQuestInfo, CONT_PLAYER_MODIFY_ORDER &rkOrder)
{
	if( !pkState )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BYTE const cOldState = pkState->byQuestState;
	for(int iCur=0; QUEST_PARAMNUM>iCur; ++iCur)
	{
		int const iEndCount = pkQuestInfo->GetParamEndCondition(iCur);
		if( iEndCount > pkState->byParam[iCur] ) //하나라도 안되면
		{
			if( QS_End == pkState->byQuestState )
			{
				SPMOD_AddIngQuest kAddQuestData(pkState->iQuestID, QS_Ing);
				rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, rkCharGuid, kAddQuestData) );
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;//Can't Comlete
		}
	}

	if( QS_End != pkState->byQuestState )
	{
		SPMOD_AddIngQuest kAddQuestData(pkState->iQuestID, QS_End);
		rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, rkCharGuid, kAddQuestData) );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_SetQuestParam : Set my Quest State Param Count
//////////////////////////////////////////////////////////////////////////////
PgAction_SetQuestParam::PgAction_SetQuestParam(SGroundKey const &rkGndKey, int const iQusetID, int const iParamNo, size_t const iTargetCount)
	:m_kGndKey(rkGndKey), m_iQuestID(iQusetID), m_iParamNo(iParamNo), m_iTargetCount(iTargetCount)
{
}

PgAction_SetQuestParam::~PgAction_SetQuestParam()
{
}

bool PgAction_SetQuestParam::DoAction(CUnit* pkUser, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Somthing wrong");
	}

	if( !pkUser )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( PgQuestInfoUtil::VerifyIndex(m_iParamNo) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"invalid Quest Param No: " << m_iParamNo << L", GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't Player Unit, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"is PgMyQuest Pointer is NULL, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
	if( !pkState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't Find user ing quest[GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name() << L"] QuestID: " << m_iQuestID );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	
	// m_iParamNo 몇번째 파라메터를
	// iResultCount 결과 갯수
	// m_sQuestID 퀘스트 아이디

	PgAction_SetQuestParam::DoAction(pkUser->GetID(), pkState, m_iParamNo, m_iTargetCount, kOrder);

	if( !kOrder.empty() )
	{
		PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, m_kGndKey, kOrder);
		kItemModifyAction.DoAction(pkUser, NULL);
	}
	return true;
}

bool PgAction_SetQuestParam::DoAction(BM::GUID const &rkCharGuid, SUserQuestState const *pkState, int const iParamNo, size_t const iParamCount, CONT_PLAYER_MODIFY_ORDER &rkOrder)
{
	if( !pkState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"SUserQuestState const *pkState is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( PgQuestInfoUtil::VerifyIndex(iParamNo) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"invalid Quest Param No: " << iParamNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( QS_Failed == pkState->byQuestState )
	{
		// 실패 퀘스트는 상태 없데이트 없다
		return true;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	bool const bFindQuest = g_kQuestMan.GetQuest(pkState->iQuestID, pkQuestInfo);
	if( !bFindQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID[" << pkState->iQuestID << L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	size_t const byMax = pkQuestInfo->GetParamEndCondition(iParamNo);

	int const iResultCount = __min(iParamCount, byMax);
	if( iResultCount == pkState->byParam[iParamNo] )//상태가 변경 되면
	{
		return true;
	}

	SUserQuestState kUpdatedState = *pkState;
	kUpdatedState.byParam[iParamNo] = (BYTE)iResultCount;

	PgCheckQuestComplete::DoAction(rkCharGuid, &kUpdatedState, pkQuestInfo, rkOrder);

	SPMOD_IngQuestParam kAddQuestData(pkState->iQuestID, (BYTE)iParamNo, (BYTE)iResultCount, byMax, true);
	rkOrder.push_back( SPMO(IMET_ADD_INGQUEST_PARAM, rkCharGuid, kAddQuestData) );
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_IncQuestParam : Increase quest param state
//////////////////////////////////////////////////////////////////////////////
PgAction_IncQuestParam::PgAction_IncQuestParam(SGroundKey const &rkGndKey, int const iQusetID, int const iParamNo, size_t const iIncVal)
	:m_kGndKey(rkGndKey), m_iQuestID(iQusetID), m_iParamNo(iParamNo), m_iIncVal(iIncVal)
{
	if( PgQuestInfoUtil::VerifyIndex(iParamNo) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Over Quest ParamNo "<<iParamNo);
	}
}

PgAction_IncQuestParam::~PgAction_IncQuestParam()
{
}

bool PgAction_IncQuestParam::DoAction(CUnit* pkUser, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkUser )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_iIncVal )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkUser->GetID()<<L", Name: "<<pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkUser->GetID()<<L", Name: "<<pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
	if( !pkState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't Find user ing quest[GUID: "<<pkUser->GetID()<<L", Name: "<<pkUser->Name()<<L"] QuestID: "<<m_iQuestID);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	bool bFindQuest = g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo);
	if( !bFindQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<m_iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	DoAction(pkQuestInfo, pkState, m_iParamNo, m_iIncVal, pkUser, kOrder);

	if( !kOrder.empty() )
	{
		PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, m_kGndKey, kOrder);
		kItemModifyAction.DoAction(pkUser, NULL);
	}
	return true;
}

bool PgAction_IncQuestParam::DoAction(PgQuestInfo const *pkQuestInfo, SUserQuestState const *pkState, int const iParamNo, int const iIncVal, CUnit* pkUser, CONT_PLAYER_MODIFY_ORDER &rkOrder)
{
	if( !pkState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"SUserQuestState const *pkState is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkQuestInfo )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<_T("Can't find Quest ID["<<pkState->iQuestID<<L"] Info"));
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( QS_Failed == pkState->byQuestState )
	{
		// 실패 퀘스트는 상태 없데이트 없다
		return true;
	}

	int const iMax = pkQuestInfo->GetParamEndCondition(iParamNo);
	int const iParamCount = pkState->byParam[iParamNo];
	int iResultCount = iParamCount + iIncVal;
	if( iMax > iParamCount )//아직 증가할 여유가 있으면
	{
		iResultCount = (iMax < iResultCount)? iMax: iResultCount;

		BYTE const iResultIncValue = (UCHAR_MAX < iResultCount)? UCHAR_MAX: (BYTE)iResultCount;

		SUserQuestState kUpdatedState = *pkState;
		kUpdatedState.byParam[iParamNo] = (BYTE)iResultIncValue;

		PgCheckQuestComplete kCheckAction(pkQuestInfo->ID(), rkOrder, &kUpdatedState);
		kCheckAction.DoAction(pkUser, NULL);

		SPMOD_IngQuestParam kAddQuestData(pkQuestInfo->ID(), (BYTE)iParamNo, (BYTE)iIncVal, (BYTE)iMax, false);
		rkOrder.push_back( SPMO(IMET_ADD_INGQUEST_PARAM, pkUser->GetID(), kAddQuestData) );
	}
	return true;
}

PgAction_GlobalQuestCheck::PgAction_GlobalQuestCheck( int const iCurGroundNo, SGroundKey const &rkGndKey, unsigned int iCombo )
:m_iGroundNo(iCurGroundNo), m_kGndKey(rkGndKey), m_iCombo(iCombo)
{
}

bool PgAction_GlobalQuestCheck::DoAction( CUnit* pkCaster, CUnit* pkTarget )
{
	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkTarget )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkTarget is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory const *pkMyInven = pkPC->GetInven();
	if( !pkMyInven )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"PgInventory Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContUserQuestState kVec;
	if( !pkMyQuest->GetQuestList(kVec) )
	{
		return true;
	}

	ContUserQuestState::const_iterator iter = kVec.begin();
	while( kVec.end() != iter )
	{
		ContUserQuestState::value_type const& rkState = (*iter);

		PgQuestInfo const* pkQuestInfo = NULL;
		if( g_kQuestMan.GetQuest(rkState.iQuestID, pkQuestInfo) )
		{
			ContQuestDependGlobal const& rkDependGlobal = pkQuestInfo->m_kDepend_Global;
			ContQuestDependGlobal::const_iterator global_iter = rkDependGlobal.begin();
			while( rkDependGlobal.end() != global_iter )
			{
				ContQuestDependGlobal::value_type const& rkGlobalEvent = (*global_iter);
				switch( rkGlobalEvent.iType )
				{
				case QET_GLOBAL_Combo_IncParam:
					{
						if( m_iCombo &&	(long long)rkGlobalEvent.iVal1 <= (long long)m_iCombo )
						{
							PgAction_IncQuestParam kActionQuest(m_kGndKey, rkState.iQuestID, rkGlobalEvent.iObjectNo, rkGlobalEvent.iVal2);
							kActionQuest.DoAction( pkCaster, NULL );
						}
					}break;					
				default:
					{
					}break;
				}
				
				++global_iter;
			}
		}

		++iter;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_MonsterQuestCheck
//////////////////////////////////////////////////////////////////////////////
PgAction_PetQuestCheck::PgAction_PetQuestCheck(SGroundKey const &rkGndKey, int const iQuestID)
	: m_kGndKey(rkGndKey), m_iQuestID(iQuestID)
{
}

bool PgAction_PetQuestCheck::DoAction(CUnit* pkUser, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << L"Somthing wrong");
	}

	if( !pkUser )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
/*
	if( PgQuestInfoUtil::VerifyIndex(m_iParamNo) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"invalid Quest Param No: " << m_iParamNo << L", GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name() );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
*/
	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkUser);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"isn't Player Unit, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"is PgMyQuest Pointer is NULL, GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
	if( !pkState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__ << L"Can't Find user ing quest[GUID: " << pkUser->GetID() << L", Name: " << pkUser->Name() << L"] QuestID: " << m_iQuestID );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory const *pkMyInven = pkPC->GetInven();
	if( !pkMyInven )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"PgInventory Pointer is NULL, GUID: "<<pkUser->GetID()<<L", Name: "<<pkUser->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	
	// m_iParamNo 몇번째 파라메터를
	// iResultCount 결과 갯수
	// m_sQuestID 퀘스트 아이디

	//PgAction_SetQuestParam::DoAction(pkUser->GetID(), pkState, m_iParamNo, m_iTargetCount, kOrder);


/*
	PgQuestInfo const *pkQuestInfo = NULL;
	bool bFindQuest = g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo);
	if( false == bFindQuest || NULL == pkQuestInfo)
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<m_iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iPetLevel = 0;
	PgBase_Item kPetItem;
	if( S_OK == pkInven->GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kPetItem) )
	{
		PgItem_PetInfo* pkPetExtInfo = NULL;
		if( true == kPetItem.GetExtInfo(pkPetExtInfo) )
		{
			//pkPetExtInfo->ClassKey().nLv;
		}
	}
*/
	if( !kOrder.empty() )
	{
		PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, m_kGndKey, kOrder);
		kItemModifyAction.DoAction(pkUser, NULL);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_MonsterQuestCheck
//////////////////////////////////////////////////////////////////////////////
PgAction_MonsterQuestCheck::PgAction_MonsterQuestCheck(int const iCurGroundNo, SGroundKey const &rkGndKey, unsigned int iCombo,
		int const iMissionKey, int const iMissionLevel, int const iMssionStageCount, PgGround const *pkGround, CUnit const* pkKiller)
	:m_iGroundNo(iCurGroundNo), m_kGndKey(rkGndKey), m_iCombo(iCombo), m_iMissionKey(iMissionKey),
		m_iMissionLevel(iMissionLevel), m_iMissionStageCount(iMssionStageCount), m_pkGround(pkGround), m_pkKiller(pkKiller)
{
}

bool PgAction_MonsterQuestCheck::CanPartyPass(PgPlayer *pkPC, int iQuestID)
{
	if (m_pkKiller == (CUnit*)pkPC)
	{
		return true;
	}

	PgPlayer const *pkKiller = dynamic_cast<PgPlayer const*>(m_pkKiller);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkKiller->GetID()<<L", Name: "<<pkKiller->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkKillerQuest = pkKiller->GetMyQuest();
	if( !pkKillerQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkKiller->GetID()<<L", Name: "<<pkKiller->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if (!pkKillerQuest->Get(iQuestID))
	{
		return false;
	}

	return true;
}

bool PgAction_MonsterQuestCheck::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkTarget )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkTarget is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory const *pkMyInven = pkPC->GetInven();
	if( !pkMyInven )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"PgInventory Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iMonClassID = pkTarget->GetAbil(AT_CLASS);
	ContQuestID const* pkVec = NULL;
	if( !g_kQuestMan.GetMonsterQuest(iMonClassID, pkVec) )
	{
		return true; // 퀘스트와 관련없는 몬스터
	}

	ContQuestID::const_iterator quest_iter = pkVec->begin();
	for(; pkVec->end() != quest_iter; ++quest_iter)
	{
		CONT_ITEM_CREATE_ORDER kOrderList;
		int const iQuestID = (*quest_iter);
		SUserQuestState const *pkUserState = pkMyQuest->Get(iQuestID);
		PgQuestInfo const * pkQuestInfo = NULL;
		if(	pkUserState
		&&	QS_Failed != pkUserState->byQuestState
		&&	g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
		{
			// if ( pkQuestInfo->IsPartyQuest()
			// 	&& (pkPC->PartyGuid().IsNull()
			// 		|| m_pkGround->GetPartyMemberCount(pkPC->PartyGuid()) < pkQuestInfo->m_kLimit.iMinParty)
			// 		|| !CanPartyPass(pkPC, iQuestID) )
			// {
			// 	continue;
			// }

			const ContQuestMonster& rkDependMonster = pkQuestInfo->m_kDepend_Monster;
			ContQuestMonster::const_iterator mon_iter = rkDependMonster.find(iMonClassID);
			while(rkDependMonster.end() != mon_iter && (*mon_iter).first == iMonClassID )
			{
				const ContQuestMonster::mapped_type& rkElement = (*mon_iter).second;
				switch(rkElement.iType)
				{
				case QET_MONSTER_IncParam_MN:
				case QET_MONSTER_IncParam100_MN:
					{
						if( m_iMissionStageCount != rkElement.iStageCount )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched Mission Stage Count"));
							break;//미션 맵장수가 일치하지 않으면 통과
						}
					}//break; 없는게 맞다
				case QET_MONSTER_IncParam_M:
				case QET_MONSTER_IncParam100_M:
					{
						if( (m_iMissionKey != rkElement.iMissionKey)
							|| (m_iMissionLevel != rkElement.iMissionLevel) )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched MissinKey and MissionLevel"));
							break;//미션과 미션난이도가 같지 않으면 통과
						}
					}//break; 없는게 맞다
				case QET_MONSTER_IncParam_In:
				case QET_MONSTER_IncParam100_In:
					{
						if( QET_MONSTER_IncParam_In == rkElement.iType ||
							QET_MONSTER_IncParam100_In == rkElement.iType )
						{//위에 브레이크 풀린것들은 패스하자. 아래 케이스문을 지나야 하기 떄문에 넣음
							if( m_iGroundNo != rkElement.iTargetGroundNo )
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched GroundNo"));
								break;//그라운드가 같지 않으면 통과
							}
						}
					}//break; 없는게 맞다
				case QET_MONSTER_IncParam:
				case QET_MONSTER_IncParam100:
					{
						PgAction_IncQuestParam kActionQuest(m_kGndKey, iQuestID, rkElement.iObjectNo, rkElement.iVal1);
						kActionQuest.DoAction(pkCaster, NULL);
					}break;
				case QET_MONSTER_InstDrop_MN:
				case QET_MONSTER_InstDrop100_MN:
					{
						if( m_iMissionStageCount != rkElement.iStageCount )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched Mission Stage Count"));
							break;//미션 맵장수가 일치하지 않으면 통과
						}
					}//break; 없는게 맞다
				case QET_MONSTER_InstDrop_M:
				case QET_MONSTER_InstDrop100_M:
					{
						if( (m_iMissionKey != rkElement.iMissionKey)
							|| (m_iMissionLevel != rkElement.iMissionLevel) )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched MissinKey and MissionLevel"));
							break;//미션과 미션난이도가 같지 않으면 통과
						}
					}//break; 없는게 맞다
				case QET_MONSTER_InstDrop_In:
				case QET_MONSTER_InstDrop100_In:
					{
						if( QET_MONSTER_InstDrop_In == rkElement.iType ||
							QET_MONSTER_InstDrop100_In == rkElement.iType )
						{//위에 브레이크 풀린것들은 패스하자. 아래 케이스문을 지나야 하기 떄문에 넣음.
							if( m_iGroundNo != rkElement.iTargetGroundNo )
							{
								LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Matched GroundNo"));
								break;//그라운드가 같지 않으면 통과
							}
						}
					}//break; 없는게 맞다
				case QET_MONSTER_InstDrop:
				case QET_MONSTER_InstDrop100:
					{
						int const iParamMaxCondition = pkQuestInfo->GetParamEndCondition(rkElement.iObjectNo);
						if( iParamMaxCondition <= pkUserState->byParam[rkElement.iObjectNo] )
						{
							break;//
						}

						int const iResultRand = BM::Rand_Index(rkElement.iVal1);
						if( rkElement.iVal2 <= iResultRand )
						{
							int const iCount = BM::Rand_Range(rkElement.iVal3, rkElement.iVal4);
							if( iCount )
							{
								PgBase_Item kItem;
								if(SUCCEEDED( CreateSItem(rkElement.iItemNo, iCount, GIOT_QUEST, kItem) ))
								{
									kOrderList.push_back(kItem);
								}
							}
						}
					}break;
				case QET_MONSTER_ComboCheck:
					{
						if( m_iCombo
						&&	rkElement.iVal1 <= m_iCombo )
						{
							PgAction_IncQuestParam kActionQuest(m_kGndKey, iQuestID, rkElement.iObjectNo, rkElement.iVal2);
							kActionQuest.DoAction(pkCaster, NULL);
						}
					}break;
				}

				++mon_iter;
			}
		}

		if( !kOrderList.empty() )
		{
			PgAction_CreateItem kCreateAction(CIE_QuestItem, m_kGndKey, kOrderList);
			kCreateAction.DoAction(pkCaster, NULL);
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_GroundQuestCheck : Ground to Quest check
//////////////////////////////////////////////////////////////////////////////
PgAction_GroundQuestCheck::PgAction_GroundQuestCheck(int const iCurGroundNo, SGroundKey const &rkGndKey, unsigned int iCombo, unsigned int eState)
	: m_iGroundNo(iCurGroundNo), m_kGndKey(rkGndKey), m_iCombo(iCombo), m_kIndunState(eState)
{	
}

bool PgAction_GroundQuestCheck::DoAction(CUnit* pkCaster, CUnit* pkTarget)
{
	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkTarget )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkTarget is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory const *pkMyInven = pkPC->GetInven();
	if( !pkMyInven )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"PgInventory Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID const* pkVec = NULL;
	if( !g_kQuestMan.GetGroundQuest(m_iGroundNo, pkVec) )
	{
		return true; // 퀘스트와 관련없는 그라운드
	}

	ContQuestID::const_iterator quest_iter = pkVec->begin();
	while( pkVec->end() != quest_iter )
	{
		CONT_ITEM_CREATE_ORDER kOrderList;

		int const iQuestID = (*quest_iter);
		SUserQuestState const *pkUserState = pkMyQuest->Get(iQuestID);
		PgQuestInfo const * pkQuestInfo = NULL;
		if(	pkUserState
		&&	QS_Failed != pkUserState->byQuestState
		&&	g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
		{
			ContQuestGround const& rkDependGround = pkQuestInfo->m_kDepend_Ground;
			ContQuestGround::const_iterator ground_iter = rkDependGround.find(m_iGroundNo);
			while(rkDependGround.end() != ground_iter )
			{
				const ContQuestGround::mapped_type& rkElement = (*ground_iter).second;
				switch( rkElement.iType )
				{
				case QET_GROUND_ComboCheck:
					{
						if( m_iCombo
						&&	rkElement.iVal1 <= m_iCombo )
						{
							PgAction_IncQuestParam kActionQuest(m_kGndKey, iQuestID, rkElement.iObjectNo, rkElement.iVal2);
							kActionQuest.DoAction(pkCaster, NULL);
						}
					}break;
				case QET_GROUND_KillMonster:
					{
						PgAction_IncQuestParam kActionQuest(m_kGndKey, iQuestID, rkElement.iObjectNo, rkElement.iVal1);
						kActionQuest.DoAction(pkCaster, NULL);
					}break;
				case QET_GROUND_MonsterDrop:
					{
						int const iParamMaxCondition = pkQuestInfo->GetParamEndCondition(rkElement.iObjectNo);
						if( iParamMaxCondition <= pkUserState->byParam[rkElement.iObjectNo] )
						{
							break;
						}

						int const iResultRand = BM::Rand_Index( rkElement.iVal1 ); // MaxRate
						if( rkElement.iVal2 <= iResultRand )
						{
							int const iCount = BM::Rand_Range( rkElement.iVal3, rkElement.iVal4 );
							if( iCount )
							{
								PgBase_Item kItem;
								if( SUCCEEDED( CreateSItem( rkElement.iItemNo, iCount, GIOT_QUEST, kItem ) ) )
								{
									kOrderList.push_back(kItem);
								}
							}
						}
					}break;
				case QET_GROUND_IndunResult:
					{
						if (m_kIndunState == INDUN_STATE_RESULT)
						{
							PgAction_IncQuestParam kActionQuest(m_kGndKey, iQuestID, rkElement.iObjectNo, 1);
							kActionQuest.DoAction(pkCaster, NULL);
						}
					}break;
				}

				++ground_iter;
			}
		}

		if( !kOrderList.empty() )
		{
			PgAction_CreateItem kCreateAction(CIE_QuestItem, m_kGndKey, kOrderList);
			kCreateAction.DoAction(pkCaster, NULL);
		}
		++quest_iter;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_GiveQuestReward : give quest reward (Exp / Gold)
//////////////////////////////////////////////////////////////////////////////
PgAction_GiveQuestReward::PgAction_GiveQuestReward(int const iQuestID, unsigned int const iSelect1, unsigned int const iSelect2, CONT_PLAYER_MODIFY_ORDER &rkOrder)
	: m_iQuestID(iQuestID), m_iSelect1(iSelect1), m_iSelect2(iSelect2), m_kOrder(rkOrder)
{
}

void PgAction_GiveQuestReward::MakeRewardOrder(ContQuestItem const& rkRewardItem, bool const bSelective, unsigned int const iSelect, CONT_ITEM_CREATE_ORDER& rkOrderList)
{
	if( rkRewardItem.size() )
	{
		if( bSelective )//선택 아이템
		{
			if( iSelect
			&&	rkRewardItem.size() >= iSelect )
			{
				PgBase_Item kItem;
				ContQuestItem::value_type const& rkElement = rkRewardItem.at(iSelect-1);
				if(SUCCEEDED( CreateSItem(rkElement.iItemNo, rkElement.iCount, rkElement.iRarityType, kItem, rkElement.bSeal) ))
				{
					if( 0 != rkElement.iTimeValue )
					{
						kItem.SetUseTime( rkElement.iTimeType, rkElement.iTimeValue );
					}
					rkOrderList.push_back(kItem);
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Wrong quest reward item["<<rkElement.iItemNo<<L"] count["<<rkElement.iCount<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CreateSItem Failed!"));
				}
			}
		}
		else//선택 없음
		{
			ContQuestItem::const_iterator item_iter = rkRewardItem.begin();
			while(rkRewardItem.end() != item_iter)
			{
				PgBase_Item kItem;
				ContQuestItem::value_type const& rkElement = (*item_iter);
				if(SUCCEEDED( CreateSItem(rkElement.iItemNo, rkElement.iCount, rkElement.iRarityType, kItem, rkElement.bSeal) ))
				{
					if( 0 != rkElement.iTimeValue )
					{
						kItem.SetUseTime( rkElement.iTimeType, rkElement.iTimeValue );
					}
					rkOrderList.push_back(kItem);
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Wrong quest reward item["<<rkElement.iItemNo<<L"] count["<<rkElement.iCount<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CreateSItem Failed!"));
				}
				++item_iter;
			}
		}
	}
}

namespace PgQuestInfoUtil
{
	void MakeOrderQuestDependAbilType(CUnit const* pkCaster, tagQuestDependAbil const& rkAbilEvent, CONT_PLAYER_MODIFY_ORDER& rkContOutOrder);
}
void PgAction_GiveQuestReward::CreateReward(PgQuestInfo const *pkQuestInfo, PgPlayer *pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder)
{
	//CONT_PLAYER_MODIFY_ORDER kOrder;

	//보상 패킷 제작
	SQuestReward const &rkQuestReward = pkQuestInfo->m_kReward;
	int const iClassNo = pkPC->GetAbil(AT_CLASS);
	int const iGender = pkPC->GetAbil(AT_GENDER);
	ContRewardVec::value_type const *pkRewardSet1 = pkQuestInfo->m_kReward.GetRewardSet1(iGender, iClassNo);
	ContRewardVec::value_type const *pkRewardSet2 = pkQuestInfo->m_kReward.GetRewardSet2(iGender, iClassNo);

	CONT_ITEM_CREATE_ORDER kItemCreateOrder;
	if( pkRewardSet1 )//Select 1
	{
		MakeRewardOrder(pkRewardSet1->kItemList, pkRewardSet1->bSelective, m_iSelect1, kItemCreateOrder);
	}
	if( pkRewardSet2 )//Select 2
	{
		MakeRewardOrder(pkRewardSet2->kItemList, pkRewardSet2->bSelective, m_iSelect2, kItemCreateOrder);
	}

	//피로도 적용
	int iMoney = rkQuestReward.iMoney;
	pkPC->CalcMoneyRate(iMoney);

	CONT_DEF_QUEST_LEVEL_REWARD const *pDef_LevelReward = NULL;
	g_kTblDataMgr.GetContDef(pDef_LevelReward);
	if( pDef_LevelReward )
	{
		CONT_DEF_QUEST_LEVEL_REWARD::const_iterator kitor_Reward = pDef_LevelReward->find( pkPC->GetAbil(AT_LEVEL) );
		if( pDef_LevelReward->end() != kitor_Reward )
		{
			
			float const fRate = pkQuestInfo->m_kReward.iLevelPer_Money_1000 / 1000.f;
			iMoney += (int)((*kitor_Reward).second.iMoney * fRate);
		}
	}
	//돈.
	SPMOD_Add_Money kAddMoneyData(iMoney);
	SPMO kIMO(IMET_ADD_MONEY, pkPC->GetID(), kAddMoneyData);
	rkOrder.push_back(kIMO);

	SItemPos kItemPos;//Temp Value
	CONT_ITEM_CREATE_ORDER::iterator craete_order_itor = kItemCreateOrder.begin();
	while(craete_order_itor != kItemCreateOrder.end())
	{
		tagPlayerModifyOrderData_Insert_Fixed kAddData((*craete_order_itor), kItemPos, true);
		SPMO kIMO(IMET_INSERT_FIXED, pkPC->GetID(), kAddData);

		rkOrder.push_back(kIMO);
		++craete_order_itor;
	}

	
	SChangeClassInfo const &rkRewardChangeClass = pkQuestInfo->m_kReward.kChangeClass;
	int const iCurClass = pkPC->GetAbil(AT_CLASS);
	if( rkRewardChangeClass.iChangeClassNo
	&&	rkRewardChangeClass.iMinLevel
	&&	rkRewardChangeClass.iPrevClassNo )
	{
		int const iCurLevel = pkPC->GetAbil(AT_LEVEL);
		if( rkRewardChangeClass.iMinLevel <= iCurLevel
		&&	rkRewardChangeClass.iPrevClassNo == iCurClass )
		{
			SPMOD_AddAbil kSetClassData(AT_CLASS, rkRewardChangeClass.iChangeClassNo);
			rkOrder.push_back(SPMO(IMET_SET_ABIL, pkPC->GetID(), kSetClassData));
		}
	}

	int iRewardExp = PgQuestInfoUtil::GetQuestRewardExp(pkQuestInfo, pkPC->GetMyQuest());
	if( pDef_LevelReward )
	{
		CONT_DEF_QUEST_LEVEL_REWARD::const_iterator kitor_Reward = pDef_LevelReward->find( pkPC->GetAbil(AT_LEVEL) );
		if( pDef_LevelReward->end() != kitor_Reward )
		{
			float const fRate = pkQuestInfo->m_kReward.iLevelPer_Exp_1000 / 1000.f;
			iRewardExp += (int)((*kitor_Reward).second.iExp * fRate);
		}
	}
	if( iRewardExp )
	{
		SPMOD_AddAbil kAddAbilData(AT_EXPERIENCE, iRewardExp);
		rkOrder.push_back( SPMO(IMET_ADD_ABIL64, pkPC->GetID(), kAddAbilData) );

		if (pkPC->GetAbil64(AT_REST_EXP_ADD_MAX) > 0)
		{
			SPMOD_AddAbil kAddRestData(AT_REST_EXP_ADD_MAX, iRewardExp);
			rkOrder.push_back( SPMO(IMET_ADD_ABIL64_MAPUSE, pkPC->GetID(), kAddRestData) );
		}
	}

	if( rkQuestReward.iTacticsExp )
	{
		SPMOD_AddAbil kAddAbilData(AT_TACTICS_EXP, rkQuestReward.iTacticsExp);
		rkOrder.push_back( SPMO(IMET_ADD_ABIL64, pkPC->GetID(), kAddAbilData) );
	}

	ContQuestDependAbil::const_iterator abil_iter = pkQuestInfo->m_kDepend_Abil.begin();
	while( pkQuestInfo->m_kDepend_Abil.end() != abil_iter ) // 능력치 연관된것에 따른 오더 조치
	{
		PgQuestInfoUtil::MakeOrderQuestDependAbilType(pkPC, (*abil_iter), rkOrder);
		++abil_iter;
	}

	ContQuestID const& rkAddEndIDVec = rkQuestReward.kAddEndQuest; // 추가 완료 처리
	if( !rkAddEndIDVec.empty() )
	{
		ContQuestID::const_iterator id_iter = rkAddEndIDVec.begin();
		while( rkAddEndIDVec.end() != id_iter )
		{
			int const iAddEndQuestID = (*id_iter);
			if( iAddEndQuestID )
			{
				SPMOD_SetEndQuest kAddQuestData(iAddEndQuestID, true);
				rkOrder.push_back( SPMO(IMET_ADD_ENDQUEST, pkPC->GetID(), kAddQuestData) );
			}
			++id_iter;
		}
	}

	if( 0 < rkQuestReward.iSkillNo )
	{
		int iSkillNo = rkQuestReward.iSkillNo;

		typedef std::list<int> COUPLE_SKILL_LIST;
		BM::Stream kPacket(PT_U_G_NFY_QUESTREWARD_ADDSKILL, (BYTE)CC_Nfy_AddSkill);
		kPacket.Push( (BYTE)CoupleCR_Success );
		kPacket.Push( pkPC->GetID() );
		COUPLE_SKILL_LIST kStack;
		kStack.push_back(iSkillNo);
		PU::TWriteArray_A(kPacket, kStack);
		pkPC->VNotify( &kPacket );
	}
	if( 0 < rkQuestReward.iChangeLevel )
	{
		__int64 i64AddExp = 0;
		__int64 const i64Exp = pkPC->GetAbil64(AT_EXPERIENCE);
		int const iClass = pkPC->GetAbil(AT_CLASS);
		CONT_DEFCLASS const* pkClassDef = NULL;
		g_kTblDataMgr.GetContDef(pkClassDef);
		CONT_DEFCLASS::const_iterator itor_Class = pkClassDef->begin();
		while( pkClassDef->end() != itor_Class )
		{
			if( iClass == (*itor_Class).second.iClass
				&& rkQuestReward.iChangeLevel == static_cast<int>((*itor_Class).second.sLevel) )
			{
				i64AddExp = (*itor_Class).second.i64Experience - i64Exp;
				break;
			}
			++itor_Class;
		}
		if( 0 != i64AddExp )
		{
			SPMOD_AddAbil kAddExpData(AT_EXPERIENCE, i64AddExp);
			rkOrder.push_back(SPMO(IMET_ADD_ABIL64, pkPC->GetID(), kAddExpData));
		}
	}
}

bool PgAction_GiveQuestReward::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	SUserQuestState const *pkUserState = pkMyQuest->Get(m_iQuestID);
	if( !pkUserState )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't Find user ing quest[GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name()<<L"] QuestID: "<<m_iQuestID);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgCheckQuestComplete kCheck(m_iQuestID, m_kOrder, NULL);
	bool const bCanComplete = kCheck.DoAction(pkCaster, NULL);
	if( !bCanComplete )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgQuestInfo const* pkQuestInfo = NULL;
	bool const bFindQuest = g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo);
	if( !bFindQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<m_iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CreateReward(pkQuestInfo, pkPC, m_kOrder);
	return true;
}



namespace PgAction_QuestMissionUtil
{
	inline bool GetPlayerMissionData(PgPlayer* pkPlayer, int const iMissionKey, PgPlayer_MissionData const*& pkMissionData, TCHAR const* szFunc, size_t const iLine)
	{
		pkMissionData = pkPlayer->GetMissionData( static_cast< unsigned int >(iMissionKey) );
		if( !pkMissionData )
		{
			CAUTION_LOG(BM::LOG_LV1,  __FL2__(szFunc, iLine) << _T("] ") << _T("PgPlayer_MissionData Pointer is NULL, GUID:") << pkPlayer->GetID() << _T(", Name: ") << pkPlayer->Name() << _T("MissionKey: ") << iMissionKey);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		return true;
	}

	bool ProcessMissionPercent(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		PgPlayer_MissionData const * pkMissionData = NULL;
		if( !GetPlayerMissionData(pkPlayer, rkElement.iMissionKey, pkMissionData, __FUNCTIONW__, __LINE__) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		size_t const iTotalMission = g_kMissionContMgr.GetTotalMissionCount( rkElement.iMissionKey );
		if( !iTotalMission )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("Total Mission Count is Zero MissionKey: ") << rkElement.iMissionKey);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false; // Zero divided error
		}

		size_t iClearCount = 0, iCur = 0;
		for( ; MAX_MISSION_LEVEL > iCur; ++iCur )
		{
			iClearCount += pkMissionData->GetClearStageCount(iCur);
		}

		int const iClearPercent = (int)((iClearCount * 100) / iTotalMission);// 이 플레이어 달성율
		if( rkElement.iVal1 <= iClearPercent )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}
		return true;
	}

	bool ProcessMissionRank(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		if( rkElement.iVal1 == iMissionLevel
		&&	rkElement.iVal2 == eClearRank )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}
		return true;
	}

	bool ProcessMissionRankN(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank, int const iMssionStgaeCount)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		if( rkElement.iVal1 == iMissionLevel
		&&	rkElement.iVal2 >= eClearRank
		&&	rkElement.iStageCount == iMssionStgaeCount )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}

		return true;
	}

	bool ProcessMissionClear(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		PgPlayer_MissionData const * pkMissionData = NULL;
		if( !GetPlayerMissionData(pkPlayer, rkElement.iMissionKey, pkMissionData, __FUNCTIONW__, __LINE__) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		bool bClear = (MRANK_NONE != eClearRank);
		if( !bClear )
		{
			bClear = pkMissionData->IsClearLevel( static_cast< size_t >(rkElement.iVal1) );
		}
		//else
		//{
		//	bClear = true;
		//}

		if( rkElement.iVal1 == iMissionLevel
		&&	bClear )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}
		return true;
	}

	bool ProcessMissionClearN(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, int const iMissionLevel, EMissionRank const eClearRank, int const iMssionStgaeCount)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		PgPlayer_MissionData const * pkMissionData = NULL;
		if( !GetPlayerMissionData(pkPlayer, rkElement.iMissionKey, pkMissionData, __FUNCTIONW__, __LINE__) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		bool bClear = (MRANK_NONE != eClearRank);
		if( !bClear )
		{
			bClear = pkMissionData->IsClearLevel( static_cast< size_t >(rkElement.iVal1) );
		}
		//else
		//{
		//	bClear = true;
		//}

		if( rkElement.iVal1 == iMissionLevel
		&&	rkElement.iStageCount == iMssionStgaeCount
		&&	bClear )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}

		return true;
	}

	bool ProcessMissionDefenceClear(SGroundKey const& rkGndKey, int const iQuestID, SQuestDependMission const& rkElement, PgPlayer* pkPlayer, size_t const iNowStage)
	{
		if( 0 == iQuestID )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("QuestID is 0"));
			return false;
		}

		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("PgPlayer* pkPlayer is NULL"));
			return false;
		}

		int const iCurState = static_cast< int >(iNowStage + 1);
		if( rkElement.iVal1 <= iCurState )
		{
			PgAction_IncQuestParam kActionQuest(rkGndKey, iQuestID, rkElement.iObjectNo, 1);
			kActionQuest.DoAction(pkPlayer, NULL);
		}
		return true;
	}

	bool ProcessQuestMission(int const MissionMode, SGroundKey const& rkGndKey, SMissionKey const& rkMissionKey, PgPlayer* pkPlayer, EMissionRank const eRank, size_t const iMissionStageCount, size_t const iNowStage, bool const bProcessOnlyDefence)
	{
		if( !pkPlayer )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("isn't Player Unit, GUID: ") << pkPlayer->GetID() << _T(", Name: ") << pkPlayer->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PgMyQuest const *pkMyQuest = pkPlayer->GetMyQuest();
		if( !pkMyQuest )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__ << _T("is PgMyQuest Pointer is NULL, GUID: ") << pkPlayer->GetID() << _T(", Name: ") << pkPlayer->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		ContUserQuestState kStateVec;
		bool const bGetQuestRet = pkMyQuest->GetQuestList(kStateVec);
		if( !bGetQuestRet )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		//[특정 미션] [목표 달성율] 퀘스트 체크
		ContUserQuestState::iterator iter = kStateVec.begin();
		while(kStateVec.end() != iter)
		{
			ContUserQuestState::value_type const& rkQuestState = (*iter);
			int const iQuestID = rkQuestState.iQuestID;
			
			PgQuestInfo const* pkQuestInfo = NULL;
			if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
			{
				ContQuestDependMission const& rkDependMission = pkQuestInfo->m_kDepend_Mission;
				ContQuestDependMission::const_iterator mission_iter = rkDependMission.begin();
				while( rkDependMission.end() != mission_iter )
				{
					ContQuestDependMission::value_type const& rkElement = (*mission_iter);
					if( rkMissionKey.iKey == rkElement.iMissionKey )
					{
						switch( rkElement.iType )
						{
						case QET_MISSION_Percent:
							{
								if( false == bProcessOnlyDefence )
								{
									ProcessMissionPercent(rkGndKey, iQuestID, rkElement, pkPlayer);
								}
							}break;
						case QET_MISSION_Rank:
							{
								if( false == bProcessOnlyDefence )
								{
									ProcessMissionRank(rkGndKey, iQuestID, rkElement, pkPlayer, rkMissionKey.iLevel, eRank);
								}
							}break;
						case QET_MISSION_RankN:
							{
								if( false == bProcessOnlyDefence )
								{
									ProcessMissionRankN(rkGndKey, iQuestID, rkElement, pkPlayer, rkMissionKey.iLevel, eRank, iMissionStageCount);
								}
							}break;
						case QET_MISSION_Clear:
							{
								if( false == bProcessOnlyDefence )
								{
									ProcessMissionClear(rkGndKey, iQuestID, rkElement, pkPlayer, rkMissionKey.iLevel, eRank);
								}
							}break;
						case QET_MISSION_ClearN:
							{
								if( false == bProcessOnlyDefence )
								{
									ProcessMissionClearN(rkGndKey, iQuestID, rkElement, pkPlayer, rkMissionKey.iLevel, eRank, iMissionStageCount);
								}
							}break;
						case QET_MISSION_DEFENCE_CLEAR:
							{
								if( MO_DEFENCE == MissionMode )
								{// QET_MISSION_DEFENCE_CLEAR 타입은 F6 클리어에만 완료 된다.
									ProcessMissionDefenceClear(rkGndKey, iQuestID, rkElement, pkPlayer, iNowStage);
								}
							}break;
						case QET_MISSION_STRATEGY_DEFENCE_CLEAR:
							{
								if( MO_DEFENCE7 == MissionMode )
								{// QET_MISSION_STRATEGY_DEFENCE_CLEAR 타입은 F7 클리어에만 완료 된다.
									ProcessMissionDefenceClear(rkGndKey, iQuestID, rkElement, pkPlayer, iNowStage);
								}
							}break;
						default:
							{
							}break;
						}
					}
					++mission_iter;
				}
			}

			++iter;
		}
		return true;
	}
};




//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestMissionPercent
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestMissionPercent::PgAction_QuestMissionPercent(SGroundKey const &rkGndKey, int const iMissionKey)
	:m_kGndKey(rkGndKey), m_iMissionKey(iMissionKey)
{
}

bool PgAction_QuestMissionPercent::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContUserQuestState kStateVec;
	bool const bGetQuestRet = pkMyQuest->GetQuestList(kStateVec);
	if( !bGetQuestRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//[특정 미션] [목표 달성율] 퀘스트 체크
	ContUserQuestState::iterator iter = kStateVec.begin();
	while(kStateVec.end() != iter)
	{
		const ContUserQuestState::value_type& rkQuestState = (*iter);
		int const iQuestID = rkQuestState.iQuestID;
		
		PgQuestInfo const* pkQuestInfo = NULL;
		if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
		{
			const ContQuestDependMission& rkDependMission = pkQuestInfo->m_kDepend_Mission;
			ContQuestDependMission::const_iterator mission_iter = rkDependMission.begin();
			while(rkDependMission.end() != mission_iter)
			{
				const ContQuestDependMission::value_type& rkElement = (*mission_iter);
				if( rkElement.iMissionKey == m_iMissionKey
				&&	rkElement.iType == QET_MISSION_Percent )
				{
					PgAction_QuestMissionUtil::ProcessMissionPercent(m_kGndKey, iQuestID, rkElement, pkPC);
				}
				
				++mission_iter;
			}

		}

		++iter;
	}
	return true;
}




//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestPvP
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestPvP::PgAction_QuestPvP(SGroundKey const &rkGndKey, BYTE const byWinLose)
	:m_kGndKey(rkGndKey), m_byWinLose(byWinLose)
{
}

bool PgAction_QuestPvP::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//if( m_byWinLose != GAME_WIN_EXERCISE ) // 현재는 승만 저장한다
	//{
	//	return true;
	//}

	ContUserQuestState kStateVec;
	bool const bGetListRet = pkMyQuest->GetQuestList(kStateVec);
	if( bGetListRet )
	{
		ContUserQuestState::const_iterator quest_iter = kStateVec.begin();
		while(kStateVec.end() != quest_iter)
		{
			const ContUserQuestState::value_type& rkUserState = (*quest_iter);
			int const iQuestID = rkUserState.iQuestID;

			SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
			if( pkState )
			{
				PgQuestInfo const *pkQuestInfo = NULL;
				if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
				{
					ContQuestDependPvp::const_iterator pvp_iter = pkQuestInfo->m_kDepend_Pvp.begin();
					while( pkQuestInfo->m_kDepend_Pvp.end() != pvp_iter )
					{
						bool bAddParam = false;
						ContQuestDependPvp::value_type const& rkDependPvp = (*pvp_iter);
						switch( rkDependPvp.iType )
						{
						case QET_PVP_Win:
							{
								bAddParam = m_byWinLose == GAME_WIN_EXERCISE;
							}break;
						case QET_PVP_Lose:
							{
								bAddParam = m_byWinLose == GAME_LOSE_EXERCISE;
							}break;
						case QET_PVP_Draw:
							{
								bAddParam = m_byWinLose == GAME_DRAW_EXERCISE;
							}break;
						default:
							{
								// none
							}break;
						}

						if( bAddParam )
						{
							int const iParamNo = rkDependPvp.iObjectNo;
							PgAction_IncQuestParam kQuestAction(m_kGndKey, iQuestID, iParamNo, 1);
							kQuestAction.DoAction(pkCaster, NULL);
						}
						++pvp_iter;
					}
				}
			}
			++quest_iter;
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestRepair
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestRepair::PgAction_QuestRepair(SGroundKey const &rkGndKey, PgBase_Item const &rkItem)
	:m_kGndKey(rkGndKey), m_kItem(rkItem)
{
}

bool PgAction_QuestRepair::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	if( !m_kItem.ItemNo() )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Repair ItemID is 0, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID const* pkQuestIDVec = NULL;
	bool const bQuestIDRet = g_kQuestMan.GetItemToQuestRepair(m_kItem.ItemNo(), pkQuestIDVec);
	if( !bQuestIDRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pItemDef = kItemDefMgr.GetDef(m_kItem.ItemNo());
	if( !pItemDef )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Item Def Pointer is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iDefaultDur = pItemDef->MaxAmount();
	if( iDefaultDur != m_kItem.Count() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID::const_iterator iter = pkQuestIDVec->begin();
	for(; pkQuestIDVec->end() != iter; ++iter)
	{
		ContQuestID::value_type const& iQuestID = (*iter);
		bool const bIngQuest = pkMyQuest->IsIngQuest(iQuestID);
		if( !bIngQuest )
		{
			continue;
		}

		PgQuestInfo const* pkQuestInfo = NULL;
		bool const bFindQuest = g_kQuestMan.GetQuest(iQuestID, pkQuestInfo);
		if( !bFindQuest )
		{
			continue;
		}

		ContQuestDependItem const& rkDependItem = pkQuestInfo->m_kDepend_Item;
		ContQuestDependItem::const_iterator item_iter = rkDependItem.find(m_kItem.ItemNo());
		while(rkDependItem.end() != item_iter && (*item_iter).first == m_kItem.ItemNo() )
		{
			ContQuestDependItem::mapped_type const& rkDependItem = (*item_iter).second;
			if( QET_ITEM_Durability_Check == rkDependItem.iType )
			{
				PgAction_IncQuestParam kIncreaseParam(m_kGndKey, iQuestID, rkDependItem.iObjectNo);
				kIncreaseParam.DoAction(pkCaster, NULL);
			}
			++item_iter;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestEnchant
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestUpgrade::PgAction_QuestUpgrade(SGroundKey const &rkGndKey, PgBase_Item const &rkItem)
	:m_kGndKey(rkGndKey), m_kItem(rkItem)
{
}

bool PgAction_QuestUpgrade::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_kItem.ItemNo() )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Enchant itemID is 0, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_kItem.EnchantInfo().IsPlus() )//업그레이드 된 것인가?
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID const* pkQuestIDVec = NULL;
	bool const bQuestIDRet = g_kQuestMan.GetItemToQuestEnchant(m_kItem.ItemNo(), pkQuestIDVec);
	if( !bQuestIDRet )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID::const_iterator iter = pkQuestIDVec->begin();
	for(; pkQuestIDVec->end() != iter; ++iter)
	{
		ContQuestID::value_type const& iQuestID = (*iter);
		bool const bIngQuest = pkMyQuest->IsIngQuest(iQuestID);
		if( !bIngQuest )
		{
			continue;
		}

		PgQuestInfo const* pkQuestInfo = NULL;
		bool const bFindQuest = g_kQuestMan.GetQuest(iQuestID, pkQuestInfo);
		if( !bFindQuest )
		{
			continue;
		}

		ContQuestDependItem const& rkDependItem = pkQuestInfo->m_kDepend_Item;
		ContQuestDependItem::const_iterator item_iter = rkDependItem.find(m_kItem.ItemNo());
		while(rkDependItem.end() != item_iter && (*item_iter).first == m_kItem.ItemNo() )
		{
			ContQuestDependItem::mapped_type const& rkDependItem = (*item_iter).second;
			if( QET_ITEM_Plus_Check == rkDependItem.iType )
			{
				PgAction_SetQuestParam kSetParam(m_kGndKey, iQuestID, rkDependItem.iObjectNo, m_kItem.EnchantInfo().PlusLv());
				kSetParam.DoAction(pkCaster, NULL);
			}
			++item_iter;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestEnchant
//////////////////////////////////////////////////////////////////////////////
PgAction_ChangeClassCheck::PgAction_ChangeClassCheck(SGroundKey const &rkGndKey)
	:m_kGndKey(rkGndKey)
{
}

bool PgAction_ChangeClassCheck::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		return false;
	}

	ContUserQuestState kUserStateVec;
	bool const bUserQuest = pkMyQuest->GetQuestList(kUserStateVec);
	if( !bUserQuest )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iClassNo = pkPC->GetAbil(AT_CLASS);
	ContUserQuestState::iterator userquest_iter = kUserStateVec.begin();
	while(kUserStateVec.end() != userquest_iter)
	{
		const ContUserQuestState::value_type& rkState = (*userquest_iter);
		if( QS_End != rkState.byQuestState
		&&	QS_Failed != rkState.byQuestState )
		{
			PgQuestInfo const* pkQuestInfo = NULL;
			bool const bFindQuest = g_kQuestMan.GetQuest(rkState.iQuestID, pkQuestInfo);
			if( bFindQuest
			&&	pkQuestInfo )
			{
				const ContQuestDependClass& rkQuestClass = pkQuestInfo->m_kDepend_Class;
				ContQuestDependClass::const_iterator class_iteer = rkQuestClass.begin();
				while( rkQuestClass.end() != class_iteer )
				{
					const ContQuestDependClass::value_type& rkElement = (*class_iteer);
					if( iClassNo == rkElement.iClassNo
					&&	rkElement.iType == QET_CLASS_Change )
					{
						PgAction_IncQuestParam kIncParam(m_kGndKey, pkQuestInfo->ID(), rkElement.iObjectNo, 1);
						kIncParam.DoAction(pkCaster, Nothing);
					}
					++class_iteer;
				}
			}
		}
		++userquest_iter;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestDialogEvent
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestDialogEvent::PgAction_QuestDialogEvent(SGroundKey const& rkGndKey, BM::GUID const & rkObjectGuid, int const iQuestID, int const iEventNo, int const iDialogID, CONT_PLAYER_MODIFY_ORDER &rkOrder, CONT_QUEST_EFFECT& rkOut,PgGround * const pkGround)
	:m_kGndKey(rkGndKey), m_kObjectGuid(rkObjectGuid), m_iQuestID(iQuestID), m_iEventNo(iEventNo), m_iDialogID(iDialogID), m_kOrder(rkOrder), m_kContQuestEffect(rkOut),m_pkGround(pkGround)
{
	m_kItemResultFailDialogID = QRDID_Close;
}

void PgAction_QuestDialogEvent::NfyAddEffectToMap(CUnit* pkUnit, CONT_QUEST_EFFECT const& rkContEffect)
{
	if( !pkUnit )
	{
		return;
	}

	if( rkContEffect.empty() )
	{
		return;
	}

	CONT_QUEST_EFFECT::const_iterator iter = rkContEffect.begin();
	while( rkContEffect.end() != iter )
	{
		BM::Stream kPacket(PT_U_G_RUN_ACTION);
		kPacket.Push( static_cast< short >(GAN_AddEffect) );
		kPacket.Push( (*iter) );
		pkUnit->VNotify( &kPacket );
		++iter;
	}
}

bool PgAction_QuestDialogEvent::DoAction(CUnit* pkCaster, CUnit* pkNPCUnit)
{
	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_iQuestID )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"QuestID is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_iEventNo )
	{
		std::wstring const kNpcName( ((pkNPCUnit)? pkNPCUnit->Name(): std::wstring()) );
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"NPC[GUID: "<<m_kObjectGuid<<L", Name: "<<kNpcName<<L"] EventNo is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_iDialogID )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Quest[ID: "<<m_iQuestID<<L"] Dialog ID is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const PgMyQuest* pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgQuestInfo const* pkQuestInfo = NULL;
	if( !g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<m_iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkQuestInfo->IsCanRemoteComplete() )
	{
		if( !pkNPCUnit )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkNPCUnit is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	ContQuestDependNpc const& rkDependNPC = pkQuestInfo->m_kDepend_Npc;
	ContQuestDependNpc::const_iterator npc_iter = rkDependNPC.find(m_iEventNo);
	//if( rkDependNPC.end() == npc_iter )
	//{
	//	VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("[%s]-[%d] Wrong Quest[%d] NPC EventNo[%d]"), __FUNCTIONW__, __LINE__, m_iQuestID, m_iEventNo);
	//	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	//	return false;
	//}

	bool bCanResume = true;
	bool bReserveDelItem = false, bReserveAddItem = false, bReserveAddGold = false;

	while(rkDependNPC.end() != npc_iter && (*npc_iter).first == m_iEventNo)
	{
		const ContQuestDependNpc::mapped_type& rkElement = (*npc_iter).second;
		if( m_iDialogID == rkElement.iDialogID )
		{
			switch(rkElement.iType)
			{
			case QET_NPC_Client:
			case QET_NPC_Agent:
			case QET_NPC_Payer:
			case QET_NPC_Ing_Dlg:
				{
					//아무 작업 없다
				}break;
			case QET_NPC_IncParam:
				{
					if( bCanResume )
					{
						SUserQuestState kEmptyState(m_iQuestID, QS_Ing);
						SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
						if( !pkState )
						{
							pkState = &kEmptyState;
						}
						
						PgQuestInfo const* pQuestInfo = NULL;
						if( false == g_kQuestMan.GetQuest(m_iQuestID, pQuestInfo) )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						if( QUEST_OBJ_STEPBYSTEP == pQuestInfo->m_ObjectType )
						{
							if( rkElement.iObjectNo )
							{
								if( 0 == pkState->byParam[rkElement.iObjectNo - 1] )
								{
									BM::Stream Packet(PT_M_C_ANS_STEPBYSTEP_QUEST);
									Packet.Push(m_iQuestID);
									
									int LastClearObejct = 0;
									for( int index = 0; index < QUEST_PARAMNUM; ++index )
									{
										if( 0 == pkState->byParam[index] )
										{
											LastClearObejct = index;
											break;
										}
									}

									Packet.Push(LastClearObejct);
									pkCaster->Send(Packet);
									
									LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
									return false;
								}
							}
						}
						PgAction_IncQuestParam::DoAction(pkQuestInfo, pkState, rkElement.iObjectNo, rkElement.iVal1, pkCaster, m_kOrder);
					}
				}break;
			case QET_NPC_CheckGold:
				{
					if( bCanResume )
					{
						__int64 const iCurMoney = pkCaster->GetAbil64(AT_MONEY);
						bool bCompareRet = false;
						if( rkElement.kStr == _T("<") )
						{
							bCompareRet = iCurMoney < rkElement.iGold;
						}
						else if( rkElement.kStr == _T(">") )
						{
							bCompareRet = iCurMoney > rkElement.iGold;
						}
						else if( rkElement.kStr == _T("==") )
						{
							bCompareRet = iCurMoney < rkElement.iGold;
						}
						else if( rkElement.kStr == _T("<=") )
						{
							bCompareRet = iCurMoney <= rkElement.iGold;
						}
						else if( rkElement.kStr == _T(">=") )
						{
							bCompareRet = iCurMoney >= rkElement.iGold;
						}
						else if( rkElement.kStr == _T("!=") )
						{
							bCompareRet = iCurMoney != rkElement.iGold;
						}

						if( !bCompareRet )
						{
							PgAction_AnsQuestDialog kActionQuest(m_kGndKey, m_kObjectGuid, m_iQuestID, m_iDialogID, rkElement.iVal1, 0, 0);
							kActionQuest.DoAction(pkCaster, pkNPCUnit);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
				}break;
			case QET_NPC_CheckItem:
				{
					if( bCanResume )
					{
						PgInventory* pkInven = pkPC->GetInven();
						if( !pkInven )
						{
							CAUTION_LOG(BM::LOG_LV1, __FL__<<L"[%s]-[%d] is PgMyQuest Pointer is NULL, GUID: "<<pkPC->GetID()<<L", Name: "<<pkPC->Name());
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						size_t const iCurCount = pkInven->GetTotalCount(rkElement.kItem.iItemNo);
						if( rkElement.kItem.iCount > iCurCount )
						{
							PgAction_AnsQuestDialog kActionQuest(m_kGndKey, m_kObjectGuid, m_iQuestID, m_iDialogID, rkElement.iVal1, 0, 0);
							kActionQuest.DoAction(pkCaster, pkNPCUnit);
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}
					}
				}break;
			case QET_NPC_GiveItem:
				{
					if( bCanResume )
					{
						PgBase_Item kNewItem;
						if( S_OK == CreateSItem(rkElement.kItem.iItemNo, rkElement.kItem.iCount,GIOT_NONE, kNewItem) )
						{
							SItemPos kItemPos;//Temp Value

							tagPlayerModifyOrderData_Insert_Fixed kAddData(kNewItem, kItemPos, true);//1씩 감소.
							SPMO kIMO(IMET_INSERT_FIXED, pkCaster->GetID(), kAddData);

							m_kOrder.push_back(kIMO);

							bReserveAddItem = true;
							m_kItemResultFailDialogID = QRDID_FullInven;
						}
					}
				}break;
			case QET_NPC_RemoveItem:
				{
					if( bCanResume )
					{
						PgInventory* pkInven = pkPC->GetInven();
						if( !pkInven )
						{
							CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkPC->GetID()<<L", Name: "<<pkPC->Name());
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						int const iItemNo = rkElement.kItem.iItemNo;
						size_t const iTotalCount = pkInven->GetTotalCount(iItemNo);
						if( (int)iTotalCount < rkElement.kItem.iCount )
						{
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
							return false;
						}

						int const iNeedCount = __min((int)iTotalCount, rkElement.kItem.iCount);
						if( iNeedCount )
						{
							CONT_PLAYER_MODIFY_ORDER kOrder;
							tagPlayerModifyOrderData_Add_Any kDelData(iItemNo, -1);
							SPMO kIMO(IMET_ADD_ANY, pkPC->GetID(), kDelData);
							kOrder.push_back(kIMO);

							bReserveDelItem = true;
							m_kItemResultFailDialogID = QRDID_LessItemCount;
						}
					}
				}break;
			case QET_NPC_CompleteQuest:
				{
					if( bCanResume )
					{
						SPMOD_SetEndQuest kAddQuestData(rkElement.iVal1, true);
						m_kOrder.push_back( SPMO(IMET_ADD_ENDQUEST, pkPC->GetID(), kAddQuestData) );
					}
				}break;
			case QET_NPC_AddGold:
				{
					if( bCanResume )
					{
						CONT_PLAYER_MODIFY_ORDER kOrder;
						SPMOD_Add_Money kDelMoneyData(rkElement.iGold);//필요머니 빼기.
						SPMO kIMO(IMET_ADD_MONEY, pkPC->GetID(), kDelMoneyData);
						kOrder.push_back(kIMO);

						bReserveAddGold = true;
					}
				}break;
			case QET_NPC_CheckEffect:
				{
					if( bCanResume )
					{
						bool bCheckRet = false;
						CEffect const* pkEffect = pkPC->GetEffect(rkElement.kItem.iItemNo);
						if( rkElement.kItem.iCount == 0 )
						{
							bCheckRet = pkEffect == NULL; // 없으면, FailedDialog로
						}
						else
						{
							bCheckRet = pkEffect != NULL; // 있으면, FailedDialog로
						}

						if( bCheckRet )
						{
							PgAction_AnsQuestDialog kActionQuest(m_kGndKey, m_kObjectGuid, m_iQuestID, m_iDialogID, rkElement.iVal1, 0, 0,m_pkGround);
							kActionQuest.DoAction(pkCaster, pkNPCUnit);
							return false;
						}
					}
				}break;
			case QET_NPC_GiveEffect:
				{
					if( bCanResume )
					{
						m_kContQuestEffect.push_back( rkElement.kItem.iItemNo );
					}
				}break;
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Wrong Quest["<<m_iQuestID<<L"] npc event type["<<rkElement.iType<<L"]");
				}break;
			}
		}
		++npc_iter;
	}

	if( bReserveDelItem
	&&	bReserveAddItem )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Can't Del & Add Item Once Quest["<<m_iQuestID<<L"] Dialog["<<m_iDialogID<<L"] Event["<<m_iEventNo<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}



//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestDeleteItem
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestDeleteItem::PgAction_QuestDeleteItem(CONT_DEL_QUEST_ITEM const& rkCont, CONT_PLAYER_MODIFY_ORDER &rkOrder)
	:m_kCont(rkCont), m_kOrder(rkOrder)
{
}

bool PgAction_QuestDeleteItem::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( m_kCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory* pkInven = pkPC->GetInven();
	if( !pkInven )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEL_QUEST_ITEM::const_iterator item_iter = m_kCont.begin();
	while(m_kCont.end() != item_iter)
	{
		if((*item_iter).iPetLv > 0)
		{ //펫 퀘스트 아이템의 경우 레벨을 체크해서 삭제한다
			CONT_DEL_QUEST_ITEM::value_type const& rkElement = (*item_iter);
			SItemPos kItemPos;
			if( S_OK == pkInven->GetFirstItem(rkElement.iItemNo, kItemPos) )
			{
				PgBase_Item kItem;
				do
				{
					if(S_OK == pkInven->GetItem(kItemPos, kItem) && true == PgQuestActionUtil::CheckCanClearPetItem(kItem, rkElement.iPetLv, rkElement.iItemNo) )
					{ //레벨이 충족하는 위치의 아이템을 찾아 제거
						SPMOD_Modify_Count kDelData(kItem, kItemPos, 0, true);//삭제.
						m_kOrder.push_back( SPMO(IMET_MODIFY_COUNT, pkPC->GetID(), kDelData) );
						break;
					}
				}while( E_FAIL != pkInven->GetNextItem(rkElement.iItemNo, kItemPos) );
			}
		}
		else
		{
			CONT_DEL_QUEST_ITEM::value_type const& rkElement = (*item_iter);
			int const iTotalCount = static_cast< int >(pkInven->GetTotalCount(rkElement.iItemNo));
			if( rkElement.iCount <= iTotalCount )
			{
				int const iNeedCount = std::min(iTotalCount, rkElement.iCount);
				if( iNeedCount )
				{
					tagPlayerModifyOrderData_Add_Any kDelData(rkElement.iItemNo, -iNeedCount);
					m_kOrder.push_back( SPMO(IMET_ADD_ANY, pkPC->GetID(), kDelData) );
				}
			}
		}
		++item_iter;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////////
//		PgAction_AnsQuestDialog
//////////////////////////////////////////////////////////////////////////////
PgAction_AnsQuestDialog::PgAction_AnsQuestDialog(SGroundKey const& rkGndKey, BM::GUID const & rkObjectGuid, int const iQuestID, int const iPreDialogID, int const iNextDialogID, int const iSelect1, int const iSelect2,PgGround * const pkGround)
	:m_kGndKey(rkGndKey),  m_kObjectGuid(rkObjectGuid), m_iQuestID(iQuestID), m_iPreDialogID(iPreDialogID), m_iNextDialogID(iNextDialogID), m_iSelect1(iSelect1), m_iSelect2(iSelect2),m_pkGround(pkGround)
{
}

bool PgAction_AnsQuestDialog::DoAction(CUnit* pkCaster, CUnit* pkObjectUnit)
{
	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !m_iQuestID )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Quest ID is 0");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer *pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgQuestInfo const *pkQuestInfo = NULL;
	if( !g_kQuestMan.GetQuest(m_iQuestID, pkQuestInfo) )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"Can't find Quest ID["<<m_iQuestID<<L"] Info");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	std::wstring const kNpcName( ((pkObjectUnit)? pkObjectUnit->Name(): std::wstring(L"Remote")) );
	if( !pkQuestInfo->IsCanRemoteComplete() )
	{
		//
		if( !pkObjectUnit )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkObjectUnit is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PgNpc *pkNpc = dynamic_cast<PgNpc*>(pkObjectUnit);
		if( !pkNpc )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgNPC Unit, GUID: "<<m_kObjectGuid<<L", Name: "<<kNpcName);
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( 0 == m_iNextDialogID )
	{
		::NfyShowQuestDialog(pkPC, m_kObjectGuid, QSDT_NormalDialog, m_iQuestID, 0);
		return true;
	}

	EQuestState eState = QS_None;
	{
		SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
		if( pkState )//내 퀘스트에 있으면 진행중, 완료 둘중에 하나다
		{
			eState = (EQuestState)pkState->byQuestState;
		}
		else//없으면서 시작 할 수 있으면 시작시킨다.
		{
			eState = QS_Begin;
		}
	}


	//
	EItemModifyParentEventType eModType = IMEPT_QUEST_DIALOG;
	unsigned short sAddonPacketType = PT_M_I_REQ_QUEST_EVENT;
	int iFailedDialogID = QRDID_Close;
	int iNextDialogID = m_iNextDialogID;

	//
	bool bRet = true;
	CONT_PLAYER_MODIFY_ORDER kOrder;
	if( 0 == iNextDialogID )
	{
	}
	else if( QRDID_StartDIalogID < iNextDialogID//통상 진행
	&&		QRDID_MaxDialogID > iNextDialogID )
	{
	}
	else if( QRDID_ACCEPT_Start <= iNextDialogID//수락
	&&		QRDID_ACCEPT_End >= iNextDialogID )
	{
		eModType = IMEPT_QUEST_DIALOG_Accept;
		sAddonPacketType = PT_M_I_REQ_QUEST_ACCEPT;
		bRet = DoAccept(pkQuestInfo, eState, pkPC, kOrder, iNextDialogID, iFailedDialogID);
	}
	else if( QRDID_REJECT_Start <= iNextDialogID//수락 거부
	&&		QRDID_REJECT_End >= iNextDialogID )
	{
		bRet = DoReject(pkQuestInfo, eState, pkPC, kOrder, iNextDialogID, iFailedDialogID);
	}
	else if( QRDID_COMPLETE_Start <= iNextDialogID//완료 허락
	&&		QRDID_COMPLETE_End >= iNextDialogID )
	{
		eModType = IMEPT_QUEST_DIALOG_End;
		sAddonPacketType = PT_M_I_REQ_QUEST_REWARD;
		bRet = DoComplete(pkQuestInfo, eState, pkPC, kOrder, iNextDialogID, iFailedDialogID);
	}
	else if( QRDID_REJECTCOMPLETE_Start <= iNextDialogID//완료 거부
	&&		QRDID_REJECTCOMPLETE_End >= iNextDialogID )
	{
		bRet = DoRejectComplete(pkQuestInfo, eState, pkPC, kOrder, iNextDialogID, iFailedDialogID);
	}
	else
	{
#ifndef _MDo_
		::NfyShowQuestDialog(pkPC, m_kObjectGuid, QSDT_NormalDialog, m_iQuestID, QRDID_Close);
		pkPC->SendWarnMessage(700137);
#endif
		CAUTION_LOG(BM::LOG_LV0, __FL__<<L"Wrong Next Quest["<<m_iQuestID<<L"] DialogID["<<iNextDialogID<<L"] ");
		assert(false);
	}

	if( !bRet )
	{
		::NfyShowQuestDialog(pkPC, m_kObjectGuid, QSDT_ErrorDialog, m_iQuestID, iNextDialogID);
		return false;
	}

	// 위에서 상태가 변경되었을 수 있다.
	{
		SUserQuestState const *pkState = pkMyQuest->Get(m_iQuestID);
		if( pkState )
		{
			eState = (EQuestState)pkState->byQuestState;
		}
	}

	CONT_QUEST_EFFECT kContAddEffect;
	int const iNpcEventNo = g_kQuestMan.GetNPCEventNo(m_kObjectGuid, m_iQuestID, eState);
	if( !iNpcEventNo )
	{
//의미없는 디버그 메세지 보내지 않도록 수정
//#ifndef _MDo_
//
//		if( 0 != iNextDialogID
//		&&	pkQuestInfo->FindDialog(iNextDialogID) 
//		&&	false == pkQuestInfo->IsCanRemoteAccept())
//		{
//			pkPC->SendWarnMessage(700135);
//		}
//#endif
		iNextDialogID = QRDID_Close;
	}
	else
	{
		//Dialog Event
		PgAction_QuestDialogEvent kActionQuest(m_kGndKey, m_kObjectGuid, m_iQuestID, iNpcEventNo, iNextDialogID, kOrder, kContAddEffect);
		if( !kActionQuest.DoAction(pkPC, pkObjectUnit) )
		{
			::NfyShowQuestDialog(pkPC, m_kObjectGuid, QSDT_ErrorDialog, m_iQuestID, 0);
			return false;
		}

		if( kActionQuest.ItemResultFailDialogID() )
		{
			iFailedDialogID = kActionQuest.ItemResultFailDialogID();
		}
	}

	if( kOrder.empty() )
	{
		BM::GUID const kPetGUID = pkPC->SelectedPetID();
		if(kPetGUID.IsNotNull())
		{
			if(m_pkGround)
			{
				CUnit* pkPet = m_pkGround->GetUnit(kPetGUID);
				if(pkPet)
				{
					if(pkPet->GetAbil(AT_MOUNTED_PET_SKILL))
					{
						pkPC->SendWarnMessage(451101);
						return false;
					}
				}
			}
		}

		// 아무 작업도 없다
		// 이펙트 주고
		PgAction_QuestDialogEvent::NfyAddEffectToMap(pkPC, kContAddEffect);
		// 다음 대화로 바로 고고
		::NfyShowQuestDialog(pkPC, m_kObjectGuid, QSDT_NormalDialog, m_iQuestID, iNextDialogID);
	}
	else
	{
		BM::Stream kAddonPacket(sAddonPacketType);
		kAddonPacket.Push( m_kObjectGuid );
		kAddonPacket.Push( m_iQuestID );
		kAddonPacket.Push( iNextDialogID );
		kAddonPacket.Push( iFailedDialogID );
		kAddonPacket.Push( kNpcName );
		kAddonPacket.Push( kContAddEffect );

		PgAction_ReqModifyItem kItemModifyAction(eModType, m_kGndKey, kOrder, kAddonPacket);
		kItemModifyAction.DoAction(pkCaster, NULL);
	}
	return true;
}

bool PgAction_AnsQuestDialog::DoAccept(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID)
{
	if( QRDID_ACCEPT_Start == iNextDialogID )
	{
		iNextDialogID = QRDID_Accepted;
	}
	else
	{
		iNextDialogID = iNextDialogID - QRDID_ACCEPT_Start;
	}

	//다음 진행상태를 예측
	int const iNpcNextEventNo = g_kQuestMan.GetNPCEventNo(m_kObjectGuid, m_iQuestID, eState);
	if( !iNpcNextEventNo )
	{
		//다음 대화가 없을 수도 있다
	}

	PgCheckQuestBegin kCheck(m_iQuestID);
	bool const bCanBegin = kCheck.DoAction(pkPC, NULL);
	if( !bCanBegin )
	{
		iNextDialogID = QRDID_MaxQuestSlot;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgInventory *pkInven = pkPC->GetInven();
	if( !pkInven )
	{
		iNextDialogID = QRDID_FullInven;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !pkQuestInfo->m_kLimit.CheckTime() )
	{
		iNextDialogID = QRDID_TimeLimited;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( iNpcNextEventNo
	&&	iNextDialogID )
	{
		//CanQuestTalk 정보
		ContQuestDependNpc const &rkDependNPC = pkQuestInfo->m_kDepend_Npc;
		ContQuestDependNpc::const_iterator eventNPC_iter = rkDependNPC.find(iNpcNextEventNo);
		while(rkDependNPC.end() != eventNPC_iter && (*eventNPC_iter).first == iNpcNextEventNo)
		{
			ContQuestDependNpc::mapped_type const &rkElement = (*eventNPC_iter).second;
			if( rkElement.iDialogID == iNextDialogID
			&&	QET_NPC_GiveItem == rkElement.iType )//
			{
				PgBase_Item kGiveItem;
				if( S_OK == CreateSItem(rkElement.kItem.iItemNo, rkElement.kItem.iCount,GIOT_NONE, kGiveItem) )
				{
					HRESULT const hCanInsertRet = pkInven->CanInsert(kGiveItem);
					if( S_OK != hCanInsertRet )
					{
						iNextDialogID = QRDID_FullInven;
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
						return false;
					}
				}
			}
			++eventNPC_iter;
		}
	}
	
	//
	//실제 퀘스트 시작 주문
	SPMOD_AddIngQuest kAddQuestData(m_iQuestID, QS_Begin);
	rkOrder.push_back(SPMO(IMET_ADD_INGQUEST, pkPC->GetID(), kAddQuestData));

	eState = QS_Ing;

	iFailedDialogID = QRDID_MaxQuestSlot;

	return true;
}

bool PgAction_AnsQuestDialog::DoReject(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID)
{
	if( QRDID_REJECT_Start == m_iNextDialogID )
	{
		iNextDialogID = QRDID_Rejected;
	}
	else
	{
		iNextDialogID = m_iNextDialogID - QRDID_REJECT_Start;
	}
	return true;
}

bool PgAction_AnsQuestDialog::DoComplete(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID)
{
	if( QRDID_COMPLETE_Start == iNextDialogID )
	{
		iNextDialogID = QRDID_Completed;
	}
	else
	{
		iNextDialogID = iNextDialogID - QRDID_COMPLETE_Start;
	}

	if( QS_Failed == eState )
	{
		PgAction_QuestDeleteItem kQuestAction(pkQuestInfo->m_kDropDeleteItem, rkOrder); // 관련 아이템 삭제
		kQuestAction.DoAction(pkPC, NULL);
		
		SPMOD_AddIngQuest kAddQuestData(m_iQuestID, QS_None); // 포기 처리
		rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPC->GetID(), kAddQuestData) );

		iNextDialogID = QRDID_FailedComplete;
	}
	else
	{
		if(false == CheckCanDeletePet(pkQuestInfo, pkPC))
		{ //만약 이게 펫퀘스트라면 펫 아이템을 삭제해야 하므로 착용 여부를 체크해주자
			//rkOrder.clear(); // 모든 조작을 초기화
			iNextDialogID = QRDID_FailedComplete;
			iFailedDialogID = QRDID_FailedComplete;
			return true;
		}

		PgAction_QuestDeleteItem kQuestAction(pkQuestInfo->m_kCompleteDeleteItem, rkOrder); // 관련 아이템 삭제
		kQuestAction.DoAction(pkPC, NULL);

		EQuestType eQuestType = static_cast<EQuestType>( PgQuestInfoUtil::GetQuestDefType( m_iQuestID ) );

		if( QT_Wanted == eQuestType )
		{
			CONT_DEF_QUEST_WANTED const *pkContWantedQuest = NULL;
			g_kTblDataMgr.GetContDef( pkContWantedQuest );
			if( pkContWantedQuest )
			{
				int iRewardClearCount = 0;

				CONT_DEF_QUEST_WANTED::const_iterator find_iter = pkContWantedQuest->find( m_iQuestID );
				if( pkContWantedQuest->end() != find_iter )
				{
					CONT_DEF_QUEST_WANTED::mapped_type const& rkQuestWanted = (*find_iter).second;
					iRewardClearCount = rkQuestWanted.iClearCount;

					if( pkPC->GetWantedQuestClearCount(m_iQuestID) >= ( iRewardClearCount - 1 ) )
					{//이번에 완료하면 보상 카운트가 되는거다. 추가보상 세팅해줘.
						int iItemNo = rkQuestWanted.iItemNo;
						int iItemCount = rkQuestWanted.iItemCount;
						int const iBaseClass = pkPC->GetAbil(AT_BASE_CLASS);
						if( UCLASS_SHAMAN == iBaseClass ||
							UCLASS_DOUBLE_FIGHTER == iBaseClass )
						{
							iItemNo = rkQuestWanted.iDra_ItemNo;
							iItemCount = rkQuestWanted.iDra_ItemCount;
						}

						PgBase_Item kRewardItem;
						if(SUCCEEDED( CreateSItem( iItemNo, iItemCount, GIOT_NONE, kRewardItem) ) )
						{
							SItemPos kItemPos;//Temp Value

							tagPlayerModifyOrderData_Insert_Fixed kAddData(kRewardItem, kItemPos, true);
							SPMO kIMO(IMET_INSERT_FIXED, pkPC->GetID(), kAddData);

							rkOrder.push_back(kIMO);
						}
					}
				}
			}
		}

		SPMOD_AddIngQuest kAddQuestData(m_iQuestID, QS_Finished); // 완료 처리
		rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkPC->GetID(), kAddQuestData) );		

		PgAction_GiveQuestReward kRewardAction(m_iQuestID, m_iSelect1, m_iSelect2, rkOrder); // 보상
		if( !kRewardAction.DoAction(pkPC, NULL) )
		{
			rkOrder.clear(); // 모든 조작을 초기화

			PgCheckQuestComplete kCheck(m_iQuestID, rkOrder, NULL);
			kCheck.DoAction(pkPC, NULL); // 상태를 End -> Ing 로 만들고

			iNextDialogID = QRDID_FailedComplete;
		}

		iFailedDialogID = QRDID_FullInven;
	}

	return true;
}

bool PgAction_AnsQuestDialog::CheckCanDeletePet(PgQuestInfo const *pkQuestInfo, PgPlayer* pkPC)
{
	CONT_DEL_QUEST_ITEM::const_iterator iterItem = pkQuestInfo->m_kCompleteDeleteItem.begin();
	for(; iterItem != pkQuestInfo->m_kCompleteDeleteItem.end(); ++iterItem)
	{
		ContQuestDependItem::const_iterator itFind = pkQuestInfo->m_kDepend_Item.find( (*iterItem).iItemNo );
		if(itFind != pkQuestInfo->m_kDepend_Item.end())
		{
			ContQuestDependItem::mapped_type const& kQuestDependItem = itFind->second;
			if(QET_ITEM_Check_Pet == kQuestDependItem.iType)
			{ //완료(삭제) 예정인 퀘스트가 펫 타입의 퀘스트라면
				PgBase_Item kItem;
				//착용 위치가 아닌 캐쉬인벤에서만 검색
				if(false == PgQuestActionUtil::GetQuestPetFromInv(kItem, pkPC, kQuestDependItem.iVal1, kQuestDependItem.iItemNo, false))
				{ //완료할 펫이 착용 중이면 완료 불가
					return false;
				}
			}
		}

	}
	return true;
}

bool PgAction_AnsQuestDialog::DoRejectComplete(PgQuestInfo const *pkQuestInfo, EQuestState &eState, PgPlayer* pkPC, CONT_PLAYER_MODIFY_ORDER &rkOrder, int &iNextDialogID, int &iFailedDialogID)
{
	if( QRDID_REJECTCOMPLETE_Start == m_iNextDialogID )
	{
		iNextDialogID = QRDID_RejectComplete;
	}
	else
	{
		iNextDialogID = iNextDialogID - QRDID_REJECTCOMPLETE_Start;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestKillCount
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestKillCount::PgAction_QuestKillCount(SGroundKey const& rkGndKey, int const iCurKillCount)
	: m_kGndKey(rkGndKey), m_iCurKillCount(iCurKillCount)
{
}

PgAction_QuestKillCount::~PgAction_QuestKillCount()
{
}

bool PgAction_QuestKillCount::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContQuestID const kVec(g_kQuestMan.GetKillCountQuest());
	ContQuestID::const_iterator quest_iter = kVec.begin();
	while( kVec.end() != quest_iter )
	{
		int const iQuestID = (*quest_iter);
		SUserQuestState const* pkState = pkMyQuest->Get(iQuestID);
		if( NULL != pkState
		&&	QS_Failed != pkState->byQuestState
		&&	QS_End != pkState->byQuestState )
		{
			PgQuestInfo const* pkQuestInfo = NULL;
			if( g_kQuestMan.GetQuest(iQuestID, pkQuestInfo) )
			{
				SQuestDependKillCount const& rkDependKillCount = pkQuestInfo->m_kDepend_KillCount;
				if( rkDependKillCount.iKillCount <= m_iCurKillCount )
				{
					PgAction_IncQuestParam kIncreaseParam(m_kGndKey, iQuestID, rkDependKillCount.iObjectNo);
					kIncreaseParam.DoAction(pkCaster, NULL);
				}
			}
		}

		++quest_iter;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestMyHome
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestMyHome::PgAction_QuestMyHome(SGroundKey const& rkGndKey, int const iTotalVisitorCount)
	: m_kGndKey(rkGndKey), m_iTotalVisitorCount(iTotalVisitorCount)
{
}

PgAction_QuestMyHome::~PgAction_QuestMyHome()
{
}

bool PgAction_QuestMyHome::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContUserQuestState kVec;
	if( !pkMyQuest->GetQuestList(kVec) )
	{
		return true;
	}

	ContUserQuestState::const_iterator iter = kVec.begin();
	while( kVec.end() != iter )
	{
		ContUserQuestState::value_type const& rkState = (*iter);

		PgQuestInfo const* pkQuestInfo = NULL;
		if( g_kQuestMan.GetQuest(rkState.iQuestID, pkQuestInfo) )
		{
			SQuestDependMyHome const& rkDependMyHome = pkQuestInfo->m_kDepend_MyHome;
			if( 0 != rkDependMyHome.iCount
			&&	m_iTotalVisitorCount >= rkDependMyHome.iCount )
			{
				PgAction_IncQuestParam kIncreaseParam(m_kGndKey, rkState.iQuestID, rkDependMyHome.iObjectNo);
				kIncreaseParam.DoAction(pkCaster, NULL);
			}
		}
		++iter;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgAction_QuestAbil
//////////////////////////////////////////////////////////////////////////////
PgAction_QuestAbil::PgAction_QuestAbil(SGroundKey const& rkGndKey, int const iAbilType)
	: m_kGndKey(rkGndKey), m_iAbilType(iAbilType)
{
}
PgAction_QuestAbil::~PgAction_QuestAbil()
{
}

bool PgAction_QuestAbil::DoAction(CUnit* pkCaster, CUnit* Nothing)
{
	if( 0 != m_iAbilType
	&&	!PgQuestInfoUtil::IsQuestDependAbilType(m_iAbilType) )
	{
		return false;
	}

	if( Nothing )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Somthing wrong");
	}

	if( !pkCaster )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
	if( !pkPC )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
	if( !pkMyQuest )
	{
		CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	ContUserQuestState kVec;
	if( !pkMyQuest->GetQuestList(kVec) )
	{
		return true;
	}

	ContUserQuestState::const_iterator iter = kVec.begin();
	while( kVec.end() != iter )
	{
		ContUserQuestState::value_type const& rkState = (*iter);

		PgQuestInfo const* pkQuestInfo = NULL;
		if( g_kQuestMan.GetQuest(rkState.iQuestID, pkQuestInfo) )
		{
			ContQuestDependAbil const& rkDependAbil = pkQuestInfo->m_kDepend_Abil;
			ContQuestDependAbil::const_iterator abil_iter = rkDependAbil.begin();
			while( rkDependAbil.end() != abil_iter )
			{
				ContQuestDependAbil::value_type const& rkAbilEvent = (*abil_iter);
				if( (0 == m_iAbilType) || (m_iAbilType == rkAbilEvent.iAbilType) )
				{
					int const iAbilValue = pkCaster->GetAbil(rkAbilEvent.iAbilType);
					switch( rkAbilEvent.iType )
					{
					case QET_ABIL_GREATER_EQUAL:
						{
							bool const bSubjectClear = (iAbilValue >= rkAbilEvent.iAbilValue);
							if( bSubjectClear )
							{
								if( rkState.byParam[rkAbilEvent.iObjectNo] < pkQuestInfo->GetParamEndCondition(rkAbilEvent.iObjectNo) )
								{
									PgAction_SetQuestParam kSetParam(m_kGndKey, rkState.iQuestID, rkAbilEvent.iObjectNo, 1);
									kSetParam.DoAction(pkCaster, NULL);
								}
							}
							else
							{
								if( 0 != rkState.byParam[rkAbilEvent.iObjectNo] )
								{
									PgAction_SetQuestParam kSetParam(m_kGndKey, rkState.iQuestID, rkAbilEvent.iObjectNo, 0);
									kSetParam.DoAction(pkCaster, NULL);
								}
							}
						}break;
					default:
						{
						}break;
					}
				}
				++abil_iter;
			}
		}
		++iter;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//		PgQuestActionUtil
//////////////////////////////////////////////////////////////////////////////
namespace PgQuestActionUtil
{
	void CheckItemQuestOrder(SGroundKey const &rkGndKey, ContItemNoSet const &rkItemSet, CUnit* pkCaster)
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;

		CheckItemQuest(rkItemSet, pkCaster, kOrder);
		CheckItemQuestPet(rkItemSet, pkCaster, kOrder, rkGndKey);

		if( !kOrder.empty() )
		{
			PgAction_ReqModifyItem kItemModifyAction(IMEPT_QUEST, rkGndKey, kOrder);
			kItemModifyAction.DoAction(pkCaster, NULL);
		}
	}

	void CheckItemQuest(ContItemNoSet const &rkItemSet, CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &rkOrder)
	{
		if( !pkCaster )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
			return;
		}

		PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
		if( !pkPC )
		{
//			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
			return;
		}

		PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
		if( !pkMyQuest )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkMyQuest is NULL"));
			return;
		}

		PgInventory const *pkInven = pkPC->GetInven();
		if( !pkInven )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgInventory is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkInven is NULL"));
			return;
		}

		ContQuestID::const_iterator questid_iter;
		ContItemNoSet::const_iterator item_iter = rkItemSet.begin();
		while( rkItemSet.end() != item_iter)//변경 정보에 대해서 퀘스트 검사
		{
			const ContItemNoSet::value_type& iItemNo = (*item_iter);
			if( !iItemNo )
			{
				goto __PASS_LOOP;
			}

			ContQuestID const* pkVec = NULL;
			bool const bFindQuest = g_kQuestMan.GetItemToQuest(iItemNo, pkVec);
			if( !bFindQuest )
			{
				goto __PASS_LOOP;
			}

			size_t const iCurItemCount = pkInven->GetTotalCount( iItemNo );

			questid_iter = pkVec->begin();
			for(; pkVec->end() != questid_iter; ++questid_iter)
			{
				ContQuestID::value_type const& iQuestID = (*questid_iter);
				SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
				if( !pkState )
				{
					continue;
				}

				PgQuestInfo const* pkQuestInfo = NULL;
				bool const bFindQuest = g_kQuestMan.GetQuest(iQuestID, pkQuestInfo);
				if( !bFindQuest )
				{
					continue;
				}

				ContQuestDependItem const& rkDependItem = pkQuestInfo->m_kDepend_Item;
				ContQuestDependItem::const_iterator item_iter = rkDependItem.find( iItemNo );
				while(rkDependItem.end() != item_iter && (*item_iter).first == iItemNo )
				{
					ContQuestDependItem::mapped_type const& rkDependItem = (*item_iter).second;
					if( (QET_ITEM_ChangeCount == rkDependItem.iType)
						|| (QET_ITEM_NotChangeCount == rkDependItem.iType) )
					{
						PgAction_SetQuestParam::DoAction(pkCaster->GetID(), pkState, rkDependItem.iObjectNo, iCurItemCount, rkOrder);
					}
					++item_iter;
				}
			}

	__PASS_LOOP:
			++item_iter;
		}
	}
	void CheckItemQuestPet(ContItemNoSet const &rkItemSet, CUnit* pkCaster, CONT_PLAYER_MODIFY_ORDER &rkOrder, SGroundKey const &rkGndKey)
	{ //펫 아이템 번호, 레벨 조건을 체크하는 퀘스트 처리
		if( !pkCaster )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"CUnit* pkCaster is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
			return;
		}

		PgPlayer* pkPC = dynamic_cast<PgPlayer*>(pkCaster);
		if( !pkPC )
		{
//			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"isn't Player Unit, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPC is NULL"));
			return;
		}

		PgMyQuest const *pkMyQuest = pkPC->GetMyQuest();
		if( !pkMyQuest )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgMyQuest Pointer is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkMyQuest is NULL"));
			return;
		}

		PgInventory *pkInven = pkPC->GetInven();
		if( !pkInven )
		{
			CAUTION_LOG(BM::LOG_LV1, __FL__<<L"is PgInventory is NULL, GUID: "<<pkCaster->GetID()<<L", Name: "<<pkCaster->Name());
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkInven is NULL"));
			return;
		}

		ContQuestID::const_iterator questid_iter;
		ContItemNoSet::const_iterator item_iter = rkItemSet.begin();
		while( rkItemSet.end() != item_iter)//변경 정보에 대해서 퀘스트 검사
		{
			const ContItemNoSet::value_type& iItemNo = (*item_iter);
			if( !iItemNo )
			{
				goto __PASS_LOOP;
			}

			ContQuestID const* pkVec = NULL;
			bool const bFindQuest = g_kQuestMan.GetItemToQuestPet(iItemNo, pkVec);
			if( !bFindQuest )
			{
				goto __PASS_LOOP;
			}

			questid_iter = pkVec->begin();
			for(; pkVec->end() != questid_iter; ++questid_iter)
			{
				ContQuestID::value_type const& iQuestID = (*questid_iter);
				SUserQuestState const *pkState = pkMyQuest->Get(iQuestID);
				if( !pkState )
				{
					continue;
				}

				PgQuestInfo const* pkQuestInfo = NULL;
				bool const bFindQuest = g_kQuestMan.GetQuest(iQuestID, pkQuestInfo);
				if( !bFindQuest )
				{
					continue;
				}

				ContQuestDependItem const& rkDependItem = pkQuestInfo->m_kDepend_Item;
				ContQuestDependItem::const_iterator item_iter = rkDependItem.find( iItemNo );
				while(rkDependItem.end() != item_iter && (*item_iter).first == iItemNo )
				{
					ContQuestDependItem::mapped_type const& rkDependItem = (*item_iter).second;
					if( QET_ITEM_Check_Pet == rkDependItem.iType )
					{
						int iPetLevel = 0;
						PgBase_Item kPetItem;
						if(true == GetQuestPetFromInv(kPetItem, pkCaster, rkDependItem.iVal1, rkDependItem.iItemNo, true))
						{ //펫착용 위치와 캐쉬 인벤을 전부 뒤져서 완료 조건에 맞는 펫이 있는지 검사
							//조건이 만족되면 퀘스트를 완료 가능한 상태로 변경
							SPMOD_AddIngQuest kAddQuestData(iQuestID, QS_End);
							rkOrder.push_back( SPMO(IMET_ADD_INGQUEST, pkCaster->GetID(), kAddQuestData) );

							SPMOD_IngQuestParam kQuestParam(iQuestID, (BYTE)rkDependItem.iObjectNo, (BYTE)pkQuestInfo->GetParamEndCondition(rkDependItem.iObjectNo), (BYTE)pkQuestInfo->GetParamEndCondition(rkDependItem.iObjectNo), true);
							rkOrder.push_back( SPMO(IMET_ADD_INGQUEST_PARAM, pkCaster->GetID(), kQuestParam) );
						}
						else
						{ //없으면 Ing 상태로 변경
							SPMOD_IngQuestParam kQuestParam(iQuestID, (BYTE)rkDependItem.iObjectNo, (BYTE)0, (BYTE)pkQuestInfo->GetParamEndCondition(rkDependItem.iObjectNo), true);
							rkOrder.push_back( SPMO(IMET_ADD_INGQUEST_PARAM, pkCaster->GetID(), kQuestParam) );
						}
					}
					++item_iter;
				}

			}

	__PASS_LOOP:
			++item_iter;
		}
	}

	bool GetQuestPetFromInv(PgBase_Item& rkOutFirstItem, CUnit* pkCaster, int iLv, int iItemNo, bool bIncludeEquipPos)
	{
		if(NULL == pkCaster || iItemNo == 0) { return false; }

		PgInventory* pkInv = pkCaster->GetInven();
		if(NULL == pkInv) { return false; }

		CONT_HAVE_ITEM_DATA_ALL kContItem;
		if( S_OK == pkInv->GetItems(IT_CASH, kContItem) && kContItem.empty() == false )
		{
			CONT_HAVE_ITEM_DATA_ALL::iterator iterItem = kContItem.begin();
			for(; iterItem != kContItem.end(); ++iterItem)
			{
				if(true == CheckCanClearPetItem(*iterItem, iLv, iItemNo))
				{
					rkOutFirstItem = *iterItem;
					return true;
				}
			}
		}

		PgBase_Item kEquipPetItem;
		if(
			true == bIncludeEquipPos &&
			S_OK == pkInv->GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, kEquipPetItem) &&
			true == CheckCanClearPetItem(kEquipPetItem, iLv, iItemNo)
			)
		{
			rkOutFirstItem = kEquipPetItem;
			return true;
		}
		return false;
	}

	bool CheckCanClearPetItem(PgBase_Item& kSrcItem, int iClearLv, int iItemNo)
	{
		PgItem_PetInfo* pkPetExtInfo = NULL;
		if(
			kSrcItem.ItemNo() == iItemNo &&
			true == kSrcItem.GetExtInfo(pkPetExtInfo) &&
			iClearLv <=  pkPetExtInfo->ClassKey().nLv
			)
		{
			return true;
		}

		return false;
	}

}