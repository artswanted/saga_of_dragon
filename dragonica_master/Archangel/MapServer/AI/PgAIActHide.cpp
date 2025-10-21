#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActHide.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

/////////////////////////////////////////////////////////////
// PgAIActHide
////////////////////////////////////////////////////////////
bool PgAIActHide::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_CHASEENEMY:
		{
			// Hide/unHide Ani 시간이 있기 때문에 이 시간동안은 다른 상태로 변이 금지
			if (pkUnit->GetDelay() <= 0 &&  pkUnit->GetState() == US_HIDE_OUT)
			{
				//PgGround* pkGround = (PgGround*) pkActArg->Get(ACTARG_GROUND);
				//NxVec3 kNormalVec;
				//bResult = pkGround->FindEnemy(pkUnit, true, kNormalVec);
				bResult = true;
			}
		}break;
	case EAI_ACTION_FIRESKILL:
		{
			// Hide/unHide Ani 시간이 있기 때문에 이 시간동안은 다른 상태로 변이 금지
			if (pkUnit->GetDelay() <= 0 &&  pkUnit->GetState() == US_HIDE_OUT)
			{
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				bResult = pkGround->CanAttackTarget(pkUnit);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<eNextAction<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return bResult;
}

void PgAIActHide::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{

	pkUnit->SetState(US_HIDE_IN);
	int iDelay = pkUnit->GetAbil(AT_HIDE_ANI_TIME);
	if (iDelay <= 0)
	{
		iDelay = AI_HIDE_DELAY_TIME;	// 기본 1초
	}
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED -- US_HIDE_IN Time["<<iDelay<<L"]");
#endif
	pkUnit->SetDelay(iDelay);
	pkUnit->SetSync(true);
}

void PgAIActHide::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActHide::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	if (pkUnit->GetState() == US_HIDE_OUT)
	{
		return;
	}
	//NxVec3 kNormalVec;
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	bool bOut = false;
	if (pkGround->CanAttackTarget(pkUnit))
	{
		bOut = true;
	}
	else 
	{
		UNIT_PTR_ARRAY kUnitArray;
		if ( pkGround->FindEnemy( pkUnit, kUnitArray ) )
		{
			bOut = true;
		}
		
	}

	if (bOut)
	{
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"US_HIDE_IN");
#endif
		pkUnit->SetState(US_HIDE_OUT);
		int iDelay = pkUnit->GetAbil(AT_HIDE_ANI_TIME);
		if (iDelay <= 0)
		{
			iDelay = AI_HIDE_DELAY_TIME;	// 기본 1초
		}
		pkUnit->SetDelay(iDelay);
		pkUnit->SetSync(true);
	}

}

