#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActDelay.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAIAction.h"
#include "Variant\PgActionResult.h"

/////////////////////////////////////////////////////////////
// PgAIActDelay
////////////////////////////////////////////////////////////
bool PgAIActDelay::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	if(pkUnit->GetDelay() > 0)
	{
		return false;
	}

	bool bResult = false;
	bool const bHasTarget = (pkUnit->GetTarget() != BM::GUID::NullData());
	bool const bTargetIsMe = pkUnit->GetTarget() == pkUnit->GetID();
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);

	switch(eNextAction)
	{
	case EAI_ACTION_FIRESKILL:
		{
			//타겟이 나면
			//새로 플레이어를 검색해서 잡자
			if(bTargetIsMe)
			{	
				UNIT_PTR_ARRAY kUnitArray;
				bResult = pkGround->FindEnemy( pkUnit, kUnitArray, 5, UT_PLAYER );
				if(bResult)
				{
					pkUnit->SetTargetList(kUnitArray);
				}
			}
			if (bHasTarget && 
				pkGround->CanAttackTarget(pkUnit))
			{
				//pkUnit->GetSkill()->Reserve(pkGround->GetReserveAISkill(pkUnit));
				bResult = true;
			}
		}break;
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_AROUND:
	case EAI_ACTION_KEEP_DISTANCE:
		{
			if(bHasTarget)
			{
				bResult = !bTargetIsMe;//pkUnit->GetTarget() != pkUnit->GetID();
			}
			else
			{
				UNIT_PTR_ARRAY kUnitArray;
				bResult = pkGround->FindEnemy( pkUnit, kUnitArray );
			}

			if(EAI_ACTION_CHASEENEMY==eNextAction && bResult)
			{
				pkUnit->SetDelay(0);
			}
		}break;
	case EAI_ACTION_IDLE:	//타겟이 있으면 그놈을 봐야지
	case EAI_ACTION_PATROL:
		{
			UNIT_PTR_ARRAY kUnitArray;
			bResult = !pkGround->FindEnemy( pkUnit, kUnitArray);
			if(!bResult)	//적이 없으면
			{
				if(true==g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_PATROL))
				{
					pkUnit->GetAI()->SetEvent(pkUnit->GetTarget(), EAI_EVENT_RETURN_WAYPOINT);
				}
			}
		}break;
	/*case EAI_ACTION_IDLE:
		{
			bResult = true;
		}break;*/
	case EAI_ACTION_RETREAT:
		{
			bResult = true;
		}break;
    case EAI_ACTION_ROAD:
		{
			int const iGroup = pkUnit->GetAbil(AT_ROADPOINT_GROUP);
			bResult = (0 != iGroup);
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

void PgAIActDelay::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	pkUnit->GetSkill()->Reserve(0);
	pkUnit->SetState(US_IDLE);
	if(ePrevAction == EAI_ACTION_CHASEENEMY || ePrevAction == EAI_ACTION_AROUND)
	{
		// 멈추어 주어야 함.
		PgGround* pkGround = NULL;
		pkActArg->Get(ACTARG_GROUND, pkGround);

		PgActionResultVector kResultVec;
		// 어째든 TargetGuid는 보내줘야 한다.
		PgActionResult* pkResult = kResultVec.GetResult(pkUnit->GetTarget(), true);
		pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec, 0);
		pkUnit->SetState(US_CHASE_ENEMY);
	}
}

void PgAIActDelay::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	// nothing to do	
}

void PgAIActDelay::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	// nothing to do
}

