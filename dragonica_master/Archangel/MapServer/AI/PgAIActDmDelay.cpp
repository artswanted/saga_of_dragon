#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActDmDelay.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

extern bool CheckRetreatHP(CUnit* pkUnit);
/////////////////////////////////////////////////////////////
// PgAIActDmDelay
////////////////////////////////////////////////////////////
bool PgAIActDmDelay::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;

	if(pkUnit->GetDelay() > 0)
	{
		return false;
	}

	switch(eNextAction)
	{
	case EAI_ACTION_CALLHELP:
	case EAI_ACTION_RETREAT:
		{
			bResult = CheckRetreatHP(pkUnit);
		}break;
	case EAI_ACTION_FIRESKILL:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			bResult = pkGround->CanAttackTarget(pkUnit);
		}break;
	case EAI_ACTION_CHASEENEMY:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			bResult = pkGround->IsTargetInRange(pkUnit);
			if(bResult)
			{
				pkUnit->SetDelay(0);
			}
		}break;
	case EAI_ACTION_IDLE:
		{
			bResult = true;
		}break;
	case EAI_ACTION_WAIT_COOLTIME:
		{
			if ( !pkUnit->GetSkill()->Reserve(0) )
			{
				bResult = true;
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

void PgAIActDmDelay::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED Delay["<<pkUnit->GetDelay()<<L"]");
#endif
	pkUnit->SetState(US_IDLE);
	// Delay Time 은 이미 설정되었다.
	pkUnit->SetSync(true);
	pkUnit->SetAbil(AT_CHASE_PATTERN, 1);
}

void PgAIActDmDelay::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActDmDelay::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

