#include "stdafx.h"
#include "PgAction.h"
#include "PgActionAchievement.h"

bool PgAddAchievementValue::FindAchievementType(int const iType, CUnit * pkUnit)
{
	CONT_DEF_CONT_ACHIEVEMENTS const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);

	if(!pkContDef || !pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_CONT_ACHIEVEMENTS::const_iterator contiter = pkContDef->find(iType);
	if(contiter == pkContDef->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(m_iValue >= std::numeric_limits<int>::max())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	BM::DBTIMESTAMP_EX kCurTime;
	g_kEventView.GetLocalTime(kCurTime);

	CONT_DEF_CONT_ACHIEVEMENTS::mapped_type const & kCont = (*contiter).second;

	int iCount = 0;

	for(CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::const_iterator aiter = kCont.begin();aiter != kCont.end();++aiter)
	{
		CONT_DEF_CONT_ACHIEVEMENTS::mapped_type::mapped_type const & kEntity = (*aiter).second;

		bool const bIsComplete = pkPlayer->GetAchievements()->IsComplete(kEntity.iSaveIdx);
		if(true == bIsComplete)
		{
			continue;
		}

		bool const bIsInTime = kEntity.CheckInTime(static_cast<BM::PgPackedTime>(kCurTime));
		if(false == bIsInTime)
		{
			continue;
		}

		++iCount;
	}

	return (0 < iCount);
}

bool PgAddAchievementValue::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(!FindAchievementType(m_iType, pUser))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(false == m_kSyncDB) // db 저장이 필요 없는 업적 수치 누적 (한번 접속으로 업적을 달성해야만 하는 종류의 업적들에 해당된다.)
	{
		PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
		if(pkPlayer)
		{
			pkPlayer->AddAbil(m_iType, m_iValue);
			PgCheckAchievements<> kCheckAchievements(m_iType, m_kGndKey);
			kCheckAchievements.DoAction(pkPlayer, NULL);
		}
		return true;
	}

	// db 저장 되어야 하는 업적들(두고 두고 누적해서 획득 되는 업적들)
	CONT_PLAYER_MODIFY_ORDER kCont;
	kCont.push_back(SPMO(IMET_ADD_ABIL,pUser->GetID(),SPMOD_AddAbil(m_iType,m_iValue)));
	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, m_kGndKey, kCont, BM::Stream(), true);
	return kItemModifyAction.DoAction(pUser, pkTargetUnit);
}

bool PgSetAchievementValue::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(!FindAchievementType(m_iType, pUser))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kCont;

	kCont.push_back(SPMO(IMET_SET_ABIL,pUser->GetID(),SPMOD_AddAbil(m_iType,m_iValue)));

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, m_kGndKey, kCont, BM::Stream(), true);

	return kItemModifyAction.DoAction(pUser, pkTargetUnit);
}

E_ACHIEVEMENT_RESULT PgAchievementToItem::Process(CUnit* pUser)
{
	CONT_DEF_ACHIEVEMENTS const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);

	if(!pkContDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return AR_INVALID_ACHIEVEMENT"));
		return AR_INVALID_ACHIEVEMENT;
	}

	CONT_DEF_ACHIEVEMENTS::const_iterator iter = pkContDef->find(m_iAchievementIdx);
	if(iter == pkContDef->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return AR_INVALID_ACHIEVEMENT"));
		return AR_INVALID_ACHIEVEMENT;
	}

	TBL_DEF_ACHIEVEMENTS const & kDefAchievement = (*iter).second;

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer *>(pUser);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return AR_INVALID_PLAYER"));
		return AR_INVALID_PLAYER;
	}

	PgAchievements const * pkAchievements = pkPlayer->GetAchievements();

	if(!pkAchievements->CheckHasItem(kDefAchievement.iSaveIdx))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return AR_NOT_HAVE_ITEM"));
		return AR_NOT_HAVE_ITEM;
	}

	PgBase_Item kItem;
	if(S_OK != CreateSItem(kDefAchievement.iItemNo, 1,GIOT_NONE,kItem))
	{
		return AR_INVALID_ACHIEVEMENT;
	}

	BM::PgPackedTime kTimeLimit;
	if(true == pkAchievements->GetAchievementTimeLimit(kDefAchievement.iSaveIdx,kTimeLimit))
	{
		__int64 i64LimitTime = 0;
		CGameTime::DBTimeEx2SecTime(static_cast<BM::DBTIMESTAMP_EX>(kTimeLimit),i64LimitTime);
		i64LimitTime -= kDefAchievement.iUseTime;
		BM::DBTIMESTAMP_EX kGenTime;
		CGameTime::SecTime2DBTimeEx(i64LimitTime,kGenTime);

		int const iDay = std::min<int>((kDefAchievement.iUseTime/(24*60*60)),MAX_CASH_ITEM_TIMELIMIT);
		int const iHour = std::min<int>((kDefAchievement.iUseTime/(60*60)),MAX_CASH_ITEM_TIMELIMIT);
		int const iMin = std::min<int>((kDefAchievement.iUseTime/(60)),MAX_CASH_ITEM_TIMELIMIT);

		if(0 < iDay)
		{
			kItem.SetUseTime(UIT_DAY,iDay);
		}
		else if(0 < iHour)
		{
			kItem.SetUseTime(UIT_HOUR,iHour);
		}
		else
		{
			kItem.SetUseTime(UIT_MIN,iMin);
		}

		kItem.CreateDate(static_cast<BM::PgPackedTime>(kGenTime));
	}

	CONT_PLAYER_MODIFY_ORDER kCont;
	kCont.push_back(SPMO(IMET_ACHIEVEMENT2INV,pUser->GetID(),SPlayerModifyOrderData_Achievement2Inv(kDefAchievement.iSaveIdx)));
	kCont.push_back(SPMO(IMET_INSERT_FIXED,pUser->GetID(),SPMOD_Insert_Fixed(kItem, SItemPos(IT_FIT,EQUIP_POS_MEDAL))));

	BM::Stream kPacket;
	kPacket.Push(kDefAchievement.iSaveIdx);

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement2Inv, m_kGndKey, kCont);

	kItemModifyAction.DoAction(pUser, NULL);

	return AR_SUCCESS;
}

bool PgAchievementToItem::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if(0 < pUser->GetAbil(AT_CANNOT_EQUIP_ACHIEVEMENT))
	{
		pUser->SendWarnMessage(20026); //장착/해제 할 수 없는 상태입니다.
		return false;
	}

	E_ACHIEVEMENT_RESULT kError = Process(pUser);

	if(AR_SUCCESS == kError)
	{
		return true;
	}

	BM::Stream kPacket(PT_M_C_ANS_ACHIEVEMENT_TO_ITEM);
	kPacket.Push(kError);
	pUser->Send(kPacket);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgItemToAchievement::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		return false;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pUser);
	if(!pkPlayer)
	{
		return false;
	}

	CONT_DEF_ITEM2ACHIEVEMENT const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);

	if(!pkContDef)
	{
		return false;
	}

	CONT_DEF_ITEM2ACHIEVEMENT::const_iterator iter = pkContDef->find(m_kItem.ItemNo());	// 훈장 아이템이 아니면 여기서 리턴
	if(iter == pkContDef->end())
	{
		return false;
	}

	if(0 < pkPlayer->GetAbil(AT_CANNOT_EQUIP_ACHIEVEMENT))
	{
		pkPlayer->SendWarnMessage(20026); //장착/해제 할 수 없는 상태입니다.
		return true;
	}

	int const iSaveIdx = (*iter).second.iSaveIdx;

	if(pkPlayer->GetAchievements()->CheckHasItem(iSaveIdx))	// 이미 북에 업적 아이템을 가지고 있다면 다시 장착 할 수 없다. 어디서 훔쳐온거지?? ㅡㅡ;;
	{
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kCont;
	kCont.push_back(SPMO(IMET_MODIFY_COUNT,pUser->GetID(),SPMOD_Modify_Count(m_kItem,m_kItemPos,0,true))); // 인벤에서 아이템 삭제
	kCont.push_back(SPMO(IMET_INV2ACHIEVEMENT,pUser->GetID(),SPlayerModifyOrderData_Achievement2Inv(iSaveIdx))); // 북에 아이템 플레그 셋

	BM::Stream kPacket;
	kPacket.Push(iSaveIdx);

	PgAction_ReqModifyItem kItemModifyAction(CAE_Inv2Achievement, m_kGndKey, kCont);
	kItemModifyAction.DoAction(pUser, NULL);

	return true;
}

bool PgCheckAchievements_Attr5Element::CheckComplete(int const iValue, CUnit* pkUser)const
{
	if(pkUser)
	{
		if( iValue > pkUser->GetAbil(m_bAttack ? AT_C_ATTACK_ADD_FIRE : AT_C_RESIST_ADD_FIRE) )
		{
			return false;
		}
		if( iValue > pkUser->GetAbil(m_bAttack ? AT_C_ATTACK_ADD_ICE : AT_C_RESIST_ADD_ICE) )
		{
			return false;
		}
		if( iValue > pkUser->GetAbil(m_bAttack ? AT_C_ATTACK_ADD_NATURE : AT_C_RESIST_ADD_NATURE) )
		{
			return false;
		}
		if( iValue > pkUser->GetAbil(m_bAttack ? AT_C_ATTACK_ADD_CURSE : AT_C_RESIST_ADD_CURSE) )
		{
			return false;
		}
		if( iValue > pkUser->GetAbil(m_bAttack ? AT_C_ATTACK_ADD_DESTROY :AT_C_RESIST_ADD_DESTROY ) )
		{
			return false;
		}
		return true;

	}
	return false;
}

bool PgCheckAchievements_Attr5Element::DoAction(CUnit* pkUser, CUnit* pkTargetUnit)
{
	if(!pkUser)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUser);
	if(!pkPlayer)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_CONT_ACHIEVEMENTS const *pkContDef = NULL;
	g_kTblDataMgr.GetContDef(pkContDef);
	if(!pkContDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_CONT_ACHIEVEMENTS::const_iterator contiter = pkContDef->find(m_iType);
	if(contiter == pkContDef->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_ACHIEVEMENTS const & kCont = (*contiter).second;
	PgAchievements const * const pkContAchievements = pkPlayer->GetAchievements();

	BM::DBTIMESTAMP_EX kCurTime;
	g_kEventView.GetLocalTime(kCurTime);

	CONT_ENABLE_COMPLETE_ACHIEVEMENTS kDefAchievement;
	for(CONT_DEF_ACHIEVEMENTS::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
	{
		if(false == (*iter).second.CheckInTime(static_cast<BM::PgPackedTime>(kCurTime)))
		{
			continue;
		}

		if(true == CheckComplete((*iter).first, pkUser))
		{
			if(((*iter).second.iSaveIdx >= 0) && pkContAchievements->IsComplete((*iter).second.iSaveIdx))
			{
				continue;
			}

			kDefAchievement.push_back((*iter).second);
			continue;
		}
		break;
	}

	if(kDefAchievement.empty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	for(CONT_ENABLE_COMPLETE_ACHIEVEMENTS::iterator iter = kDefAchievement.begin();iter != kDefAchievement.end();++iter)
	{
		kOrder.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT,pkUser->GetID(),SPMOD_Complete_Achievement((*iter).iSaveIdx, (*iter).iCategory, (*iter).iRankPoint, (*iter).iUseTime, (*iter).iGroupNo)));
	}

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, pkPlayer->GroundKey(), kOrder,BM::Stream(),true);
	return kItemModifyAction.DoAction(pkUser, pkTargetUnit);
}