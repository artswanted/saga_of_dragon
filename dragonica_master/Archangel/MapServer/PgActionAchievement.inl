template<typename T_Compare, typename T_ValueCtrl, typename T_CustomCtrl>
bool PgCheckAchievements<T_Compare,T_ValueCtrl,T_CustomCtrl>::FindCompleteAchievement(CUnit * pkUser,int const iType,int const iValue, __int64 i64CustomValue, CONT_ENABLE_COMPLETE_ACHIEVEMENTS & kDefAchievement)
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

	CONT_DEF_CONT_ACHIEVEMENTS::const_iterator contiter = pkContDef->find(iType);
	if(contiter == pkContDef->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEF_ACHIEVEMENTS const & kCont = (*contiter).second;
	PgAchievements const * const pkContAchievements = pkPlayer->GetAchievements();

	BM::DBTIMESTAMP_EX kCurTime;
	g_kEventView.GetLocalTime(kCurTime);

	for(CONT_DEF_ACHIEVEMENTS::const_iterator iter = kCont.begin();iter != kCont.end();++iter)
	{
		if(false == (*iter).second.CheckInTime(static_cast<BM::PgPackedTime>(kCurTime)))
		{
			continue;
		}

		if(true == T_Compare::CheckComplete((*iter).first,iValue))
		{
			if(((*iter).second.iSaveIdx >= 0) && pkContAchievements->IsComplete((*iter).second.iSaveIdx))
			{
				continue;
			}

			if( i64CustomValue ) // CustomValue가 설정되어 있으면 검사해야 한다.
			{
				if( !T_CustomCtrl::CheckComplete( (*iter).second.i64CustomValue, i64CustomValue) )
				{
					continue;
				}
			}

			kDefAchievement.push_back((*iter).second);
			continue;
		}
		break;
	}

	if(!kDefAchievement.empty())
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

template<typename T_Compare, typename T_ValueCtrl, typename T_CustomCtrl>
bool PgCheckAchievements<T_Compare,T_ValueCtrl,T_CustomCtrl>::DoAction(CUnit* pUser, CUnit* pkTargetUnit)
{
	if(!pUser)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int const iValue = T_ValueCtrl::GetValue(m_iType,pUser);

	CONT_ENABLE_COMPLETE_ACHIEVEMENTS kDefAchievement;

	if(!FindCompleteAchievement(pUser,m_iType,iValue,m_i64CustomValue,kDefAchievement))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_PLAYER_MODIFY_ORDER kCont;

	for(CONT_ENABLE_COMPLETE_ACHIEVEMENTS::iterator iter = kDefAchievement.begin();iter != kDefAchievement.end();++iter)
	{
		kCont.push_back(SPMO(IMET_COMPLETE_ACHIEVEMENT,pUser->GetID(),SPMOD_Complete_Achievement((*iter).iSaveIdx, (*iter).iCategory, (*iter).iRankPoint, (*iter).iUseTime, (*iter).iGroupNo)));
	}

	PgAction_ReqModifyItem kItemModifyAction(CAE_Achievement, m_kGndKey, kCont,BM::Stream(),true);

	return kItemModifyAction.DoAction(pUser, pkTargetUnit);
}