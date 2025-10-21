#include "stdafx.h"
#include "PgAction.h"
#include "PgGround.h"
#include "PgActionJobSkill.h"
#include "Variant/PgJobSkill.h"
#include "Variant/PgJobSkillExpertness.h"

//PgAction_JobSkill_AddExpertness
bool PgAction_JobSkill_AddExpertness::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	if( !pkCaster )
	{
		return false;
	}
	if( pkNothing )
	{
		return false;
	}

	PgPlayer const * pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
	if( !pkPlayer )
	{
		return false;
	}

	if( !JobSkill_LearnUtil::IsEnableUseJobSkill(pkPlayer, m_iSkillNo) )
	{
		return false;
	}

	CONT_DEF_JOBSKILL_SKILL const* pkDefJobSkill = NULL;
	CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkDefJobSkillExpertness = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill);
	g_kTblDataMgr.GetContDef(pkDefJobSkillExpertness);

	int const iExpertness = pkPlayer->JobSkillExpertness().Get(m_iSkillNo);
	int const iMaxExpertness = JobSkillExpertnessUtil::GetMaxExpertness(pkPlayer->GetAbil(AT_LEVEL), m_iSkillNo, *pkDefJobSkill, *pkDefJobSkillExpertness);
	int const iOrgAddExpertness = JobSkillExpertnessUtil::GetExpertnessUp(m_iSkillNo, iExpertness, *pkDefJobSkill, *pkDefJobSkillExpertness);
	int iAddExpertness = iOrgAddExpertness;

	CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kExpertness;
	bool const bLastExpertnessArea = JobSkillExpertnessUtil::FindExpertnessArea(m_iSkillNo, iExpertness, *pkDefJobSkill, *pkDefJobSkillExpertness, kExpertness);
	
	int iDiffExpertness = 0;
	if(0 < m_iBase_Expertness)
	{// 기준숙련도(장소/아이템)에 따른 획득 숙련도 량 변경
		int const iDevideBase = 100;
		int const iDiffExpertness = std::max(0, (iExpertness - m_iBase_Expertness) / JSE_PROBABILITY_UPRATE_DEVIDE);
		switch( iDiffExpertness )
		{
		case 0:		{ /* 100% */ }break;
		case 1:		{ iAddExpertness = iAddExpertness * PgGround::ms_JobSkillBaseExpDiff[0] / iDevideBase; }break;
		case 2:		{ iAddExpertness = iAddExpertness * PgGround::ms_JobSkillBaseExpDiff[1] / iDevideBase; }break;
		case 3:		{ iAddExpertness = iAddExpertness * PgGround::ms_JobSkillBaseExpDiff[2] / iDevideBase; }break;
		case 4:		{ iAddExpertness = iAddExpertness * PgGround::ms_JobSkillBaseExpDiff[3] / iDevideBase; }break;
		default:
			{
				iAddExpertness = 0;
			}break;
		}
	}

	{
		if( 0 < iAddExpertness
		&&	0 < m_iAddExpertnessRate )
		{
			iAddExpertness = iAddExpertness * (ABILITY_RATE_VALUE + m_iAddExpertnessRate) / ABILITY_RATE_VALUE;
		}
		if( 0 < iAddExpertness 
		&&	0 < m_iExpertnessUpVolume )
		{
			iAddExpertness = iAddExpertness * m_iExpertnessUpVolume / ABILITY_RATE_VALUE;
		}
	}

	if(iMaxExpertness < (iExpertness + iAddExpertness) )
	{
		iAddExpertness = iMaxExpertness - iExpertness;
		if( 0 != iAddExpertness && bLastExpertnessArea)
		{//숙련도가 최대치에 도달하였습니다.
			BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_FULL_EXPERTNESS);
			kPacket.Push( m_iSkillNo );
			pkCaster->Send( kPacket );
		}
	}
	if( iMaxExpertness <= iExpertness && !bLastExpertnessArea)
	{//숙련도 오르지 않음 -> 캐릭터 레벨 부족
		BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_NEED_CHAR_LEVEL);
		kPacket.Push( m_iSkillNo );
		pkCaster->Send( kPacket );
		return false;
	}
	if( 0 != pkCaster->GetAbil(AT_DEBUG_JOBSKILL) )
	{
		PgPlayer * pSendPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if(pSendPlayer)
		{
			pSendPlayer->SendWarnMessageStrDebug( BM::vstring() <<L"[DEBUG] CurExp:"<<iExpertness<<L" BaseExp:"<<m_iBase_Expertness<<L", OrgAddExp:"<<iOrgAddExpertness<<L" BaseExpDiff:"<<iDiffExpertness<<L", Result:"<<iAddExpertness);
		}
	}
	if( 0 >= iAddExpertness )
	{
		return true;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	// add
	SPMOD_JobSkillExpertness kAddExpertnessData(m_iSkillNo, iAddExpertness);
	kOrder.push_back(SPMO(IMET_JOBSKILL_ADD_EXPERTNESS, pkPlayer->GetID(), kAddExpertnessData));

	// Remove Expertness(sample)
	//SPMOD_JobSkillExpertness kAddExpertnessData(m_iSkillNo, 0);
	//kOrder.push_back(SPMO(IMET_JOBSKILL_DEL_EXPERTNESS, pkPlayer->GetID(), kAddExpertnessData));

	PgAction_ReqModifyItem kItemModifyAction(m_kCause, m_kGndKey, kOrder, m_kPacket);
	kItemModifyAction.DoAction(pkCaster, NULL);
	return true;
}

//PgAction_JobSkill3_AddExpertness
bool PgAction_JobSkill3_AddExpertness::DoAction(CUnit* pkCaster, CUnit* /*pkNothing*/)
{
	if( !pkCaster )
	{
		return false;
	}

	PgPlayer const * pkPlayer = dynamic_cast< PgPlayer const * >(pkCaster);
	if( !pkPlayer )
	{
		return false;
	}

	CONT_DEF_JOBSKILL_RECIPE::mapped_type rkDefRecipe;
	if( false==JobSkill_Third::GetJobSkill3_Recipe(m_iRecipeItemNo, rkDefRecipe) )
	{
		return false;
	}
	if( !JobSkill_LearnUtil::IsEnableUseJobSkill(pkPlayer, rkDefRecipe.iNeedSkillNo) )
	{
		return false;
	}

	CONT_DEF_JOBSKILL_SKILL const* pkDefJobSkill = NULL;
	CONT_DEF_JOBSKILL_SKILLEXPERTNESS const* pkDefJobSkillExpertness = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill);
	g_kTblDataMgr.GetContDef(pkDefJobSkillExpertness);

	int const iExpertness = pkPlayer->JobSkillExpertness().Get(rkDefRecipe.iNeedSkillNo);
	int const iMaxExpertness = JobSkillExpertnessUtil::GetMaxExpertness(pkPlayer->GetAbil(AT_LEVEL), rkDefRecipe.iNeedSkillNo, *pkDefJobSkill, *pkDefJobSkillExpertness);
	int const iOrgAddExpertness = JobSkillExpertnessUtil::GetExpertnessUp(rkDefRecipe.iNeedSkillNo, iExpertness, *pkDefJobSkill, *pkDefJobSkillExpertness);
	float const fRate = JobSkill_Third::GetJobSkill3ExpertnessGain(m_iRecipeItemNo)/ABILITY_RATE_VALUE_FLOAT;

	int iAddExpertness = iOrgAddExpertness * fRate;

	if(iMaxExpertness <= iExpertness)
	{//숙련도가 최대치에 도달하였습니다.
		int const iFilter = pkCaster->GetAbil(AT_CHECK_OVERLAP_MSG);
		bool const bCheckOverlapMsg = iFilter & COLT_ERROR_JS3_CREATE_EXPERTNESS_MAX;
		if(false==bCheckOverlapMsg)
		{
			pkCaster->SetAbil(AT_CHECK_OVERLAP_MSG, iFilter|COLT_ERROR_JS3_CREATE_EXPERTNESS_MAX);
			BM::Stream kPacket(PT_M_C_NFY_JOBSKILL_ERROR, JSEC_FULL_EXPERTNESS);
			kPacket.Push( rkDefRecipe.iNeedSkillNo );
			pkCaster->Send( kPacket );
		}
		return false;
	}
	if(iMaxExpertness < (iExpertness + iAddExpertness) )
	{
		iAddExpertness = iMaxExpertness - iExpertness;
	}
	if( 0 != pkCaster->GetAbil(AT_DEBUG_JOBSKILL) )
	{
		PgPlayer * pSendPlayer = dynamic_cast<PgPlayer*>(pkCaster);
		if(pSendPlayer)
		{
			pSendPlayer->SendWarnMessageStrDebug( BM::vstring() <<L"[DEBUG] CurExp:"<<iExpertness<<L", OrgAddExp:"<<iOrgAddExpertness<<L", Rate:"<<fRate<<L", Result:"<<iAddExpertness);
		}
	}
	if( 0 >= iAddExpertness )
	{
		return true;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;

	// add
	SPMOD_JobSkillExpertness kAddExpertnessData(rkDefRecipe.iNeedSkillNo, iAddExpertness);
	kOrder.push_back(SPMO(IMET_JOBSKILL_ADD_EXPERTNESS, pkPlayer->GetID(), kAddExpertnessData));

	// Remove Expertness(sample)
	//SPMOD_JobSkillExpertness kAddExpertnessData(m_iSkillNo, 0);
	//kOrder.push_back(SPMO(IMET_JOBSKILL_DEL_EXPERTNESS, pkPlayer->GetID(), kAddExpertnessData));

	PgAction_ReqModifyItem kItemModifyAction(m_kCause, m_kGndKey, kOrder, m_kPacket);
	kItemModifyAction.DoAction(pkCaster, NULL);
	return true;
}

//PgAction_JobSkill_Learn
bool PgAction_JobSkill_Learn::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	if( !pkCaster )
	{
		return false;
	}
	if( pkNothing )
	{
		return false;
	}

	PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
	if( !pkPlayer )
	{
		return false;
	}

	CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefJSSkill);
	if( !pkDefJSSkill )
	{
		return false;
	}
	bool bResult = false;
	CONT_PLAYER_MODIFY_ORDER kOrder;
	CONT_DEF_JOBSKILL_SKILL::const_iterator itor_DefJobSkill = pkDefJSSkill->begin();
	while( pkDefJSSkill->end() != itor_DefJobSkill )
	{
		int const iSkillNo = itor_DefJobSkill->first;
		if( JSLR_OK != JobSkill_LearnUtil::IsEnableLearnJobSkill(pkPlayer, iSkillNo) )
		{
			++itor_DefJobSkill;
			continue;
		}
		SPMOD_AddSkill kAddSkillData(iSkillNo);//꼭 골드를 먼저 빼라.
		//kOrder.push_back(SPMO(IMET_ADD_SKILL, pkCaster->GetID(), kAddSkillData));
		//최초 배우면 숙련도 100 증가(실수치는 곱하기 1만 = 100만)
		SPMOD_JobSkillExpertness kSetExpertnessData(iSkillNo, 1000000);
		kOrder.push_back(SPMO(IMET_JOBSKILL_SET_EXPERTNESS, pkPlayer->GetID(), kSetExpertnessData));

		if( 0 == JobSkill_LearnUtil::GetHaveJobSkillTypeCount_Detail(pkPlayer, iSkillNo) )
		{
			switch( JobSkillUtil::GetJobSkillType(iSkillNo, *pkDefJSSkill) )
			{
			case JST_1ST_MAIN: //첫 1차 스킬 배울때 시간, 피로도 초기화
				{
					kOrder.push_back(SPMO(IMET_JOBSKILL_INIT_EXHAUSTION, pkPlayer->GetID()));
					bResult = true;
				}break;
			}
		}
		++itor_DefJobSkill;
	}

	PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL_LEARN, m_kGndKey, kOrder);
	kItemModifyAction.DoAction(pkCaster, NULL);

	return bResult;
}


//PgAction_JobSkill_Delete
bool PgAction_JobSkill_Delete::DoAction(CUnit* pkCaster, CUnit* pkNothing)
{
	if( !pkCaster )
	{
		return false;
	}
	if( pkNothing )
	{
		return false;
	}

	PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pkCaster);
	if( !pkPlayer )
	{
		return false;
	}

	PgMySkill* pMySkill = pkPlayer->GetMySkill();
	if(!pMySkill)
	{
		return false;
	}
	
	CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
	g_kTblDataMgr.GetContDef(pkDefJSSkill);
	if( !pkDefJSSkill )
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::const_iterator itor_DefJobSkill = pkDefJSSkill->begin();
	while( pkDefJSSkill->end() != itor_DefJobSkill )
	{
		CONT_PLAYER_MODIFY_ORDER kOrder;
		int const iSkillNo = itor_DefJobSkill->first;
		//스킬 삭제
		bool bLearned = pMySkill->IsExist(iSkillNo);
		if( bLearned )
		{
			SPMOD_DelSkill kDelSkillData( SDT_Job, iSkillNo);
			kOrder.push_back(SPMO(IMET_DEL_SKILL, pkCaster->GetID(), kDelSkillData));
		}

		PgJobSkillExpertness const pExpertness = pkPlayer->JobSkillExpertness();
		bool bHaveExp = pExpertness.IsHave(iSkillNo);
		if(bHaveExp)
		{
			//숙련도 삭제
			SPMOD_JobSkillExpertness kSetExpertnessData(iSkillNo, 0);
			kOrder.push_back(SPMO(IMET_JOBSKILL_DEL_EXPERTNESS, pkPlayer->GetID(), kSetExpertnessData));
		}
		//관련 보조 스킬 삭제
		CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
		g_kTblDataMgr.GetContDef(pkContDefJobSkill);
		CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Job = pkContDefJobSkill->begin();
		while(iter_Job != pkContDefJobSkill->end())
		{
			CONT_DEF_JOBSKILL_SAVEIDX::key_type const &rkJobSkillKey = (*iter_Job).first;
			CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Job).second;
			if( iSkillNo == rkJobSkillInfo.i01NeedParent_JobSkill_No ||
				iSkillNo == rkJobSkillInfo.i02NeedParent_JobSkill_No ||
				iSkillNo == rkJobSkillInfo.i03NeedParent_JobSkill_No)
			{
				bLearned = pMySkill->IsExist(rkJobSkillKey);
				if( bLearned )
				{
					//스킬 삭제
					SPMOD_DelSkill kDelSkillData( SDT_Job, rkJobSkillKey);
					kOrder.push_back(SPMO(IMET_DEL_SKILL, pkCaster->GetID(), kDelSkillData));
				}

				bHaveExp = pExpertness.IsHave(rkJobSkillKey);
				if(bHaveExp)
				{
					//숙련도 삭제
					SPMOD_JobSkillExpertness kSetExpertnessData(rkJobSkillKey, 0);
					kOrder.push_back(SPMO(IMET_JOBSKILL_DEL_EXPERTNESS, pkPlayer->GetID(), kSetExpertnessData));
				}
			}
			++iter_Job;
		}
		{
			if( 1 == JobSkill_LearnUtil::GetHaveJobSkillTypeCount_Detail(pkPlayer, iSkillNo) )
			{
				switch( JobSkillUtil::GetJobSkillType(iSkillNo, *pkContDefJobSkill) )
				{
				case JST_1ST_MAIN: //마지막 1차 스킬 삭제시, 피로도, 시간 초기화
					{
						kOrder.push_back(SPMO(IMET_JOBSKILL_INIT_EXHAUSTION, pkPlayer->GetID()));
					}break;
				}
			}
		}

		BM::Stream kPacket;
		kPacket.Push(iSkillNo);
		PgAction_ReqModifyItem kItemModifyAction(CIE_JOBSKILL_DELETE, m_kGndKey, kOrder, kPacket);
		kItemModifyAction.DoAction(pkCaster, NULL);
		++itor_DefJobSkill;
	}

	return true;
}
