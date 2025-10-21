#include "stdafx.h"
#include "Variant/constant.h"
#include "Variant/PgPartyMgr.h"
#include "PgAIActWaitCT.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAIAction.h"

/////////////////////////////////////////////////////////////
// PgAIActIdle
////////////////////////////////////////////////////////////
bool PgAIWaitCoolTime::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] ENTERNED") __FUNCTIONW__);
	if (!pkUnit->GetSkill()->Reserve(0))
	{
		return false;
	}
	bool bResult = false;
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Checking NextAction[%d]"), __FUNCTIONW__, (int)eNextAction);
	switch(eNextAction)
	{
	//case EAI_ACTION_OPENING:
	//	return false;
	//	break;
	case EAI_ACTION_IDLE:
		{
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"EAI_ACTION_IDLE");
#endif
			if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_PATROL:
	case EAI_ACTION_ROAD:
		{
			bResult = (pkUnit->GetDelay() <= 0);
		}break;
	case EAI_ACTION_CHASEENEMY:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);

			UNIT_PTR_ARRAY kUnitArray;
			bResult = pkGround->FindEnemy( pkUnit, kUnitArray );
		}break;
	case EAI_ACTION_HIDE:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_FIRESKILL:
		{
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"EAI_ACTION_FIRESKILL");
#endif
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);

			UNIT_PTR_ARRAY kUnitArray;
			if ( pkGround->FindEnemy(pkUnit, kUnitArray) )
			{
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

void PgAIWaitCoolTime::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif
}

void PgAIWaitCoolTime::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] ENTERNED") __FUNCTIONW__);
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIWaitCoolTime::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

