#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActCallHelp.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

/////////////////////////////////////////////////////////////
// PgAIActCallHelp
////////////////////////////////////////////////////////////
bool PgAIActCallHelp::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_FIRESKILL:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			bResult = pkGround->CanAttackTarget(pkUnit);
		}break;
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_IDLE:
		{
			bResult = true;
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<eNextAction<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return bResult;
}

void PgAIActCallHelp::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED Delay["<<pkUnit->GetDelay()<<L"]");
#endif
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	pkGround->AI_CallHelp(pkUnit, pkUnit->GetDetectRange() * 1.5f);
}

void PgAIActCallHelp::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActCallHelp::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

