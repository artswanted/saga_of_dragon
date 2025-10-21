#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActIdle.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAIAction.h"

//FindEnemy : 스킬이 아닌 적을 쫒아가거나 할 때 사용
//CanAttackTarget : 스킬을 통해서 체크할 때 사용

/////////////////////////////////////////////////////////////
// PgAIActIdle
////////////////////////////////////////////////////////////
bool PgAIActIdle::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	switch(eNextAction)
	{
	//case EAI_ACTION_OPENING:
	//	return false;
	//	break;
	case EAI_ACTION_PATROL:
        {
            bResult = (pkUnit->GetDelay() <= 0);
        }break;
    case EAI_ACTION_ROAD:
		{
            int const iGroup = pkUnit->GetAbil(AT_ROADPOINT_GROUP);
			bResult = iGroup && (pkUnit->GetDelay() <= 0);
		}break;
	case EAI_ACTION_AROUND:
	case EAI_ACTION_CHASEENEMY:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);

			UNIT_PTR_ARRAY kUnitArray;
			bResult = pkGround->FindEnemy( pkUnit, kUnitArray );
			if(bResult)
			{
				pkUnit->SetDelay(0);
			}
			//bResult = pkGround->IsTargetInRange(pkUnit);
		}break;
	case EAI_ACTION_HIDE:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_FIRESKILL:
	case EAI_ACTION_FIRESKILL_DELAY:
		{
			UNIT_PTR_ARRAY kUnitArray;
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			bResult = pkGround->CanAttackTarget(pkUnit);
		}break;
	case EAI_ACTION_FOLLOW_CALLER:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);
			if(pkGround)
			{
				bResult = (false==pkGround->IsInReturnZone(pkUnit));
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

void PgAIActIdle::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif

	pkUnit->SetTarget(BM::GUID::NullData());

	int iDelay = pkUnit->GetAbil(AT_IDLE_TIME);
	if (iDelay == 0)
	{
		iDelay = 5000 + BM::Rand_Index(5000);
	}
	pkUnit->SetState(US_IDLE);
	pkUnit->SetDelay(iDelay);
	pkUnit->SetSync(true);
}

void PgAIActIdle::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActIdle::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

