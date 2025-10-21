#include "stdafx.h"
#include "Variant\constant.h"
#include "Global.h"
#include "PgAIActRetreat.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAIAction.h"

/////////////////////////////////////////////////////////////
// PgAIActPatrol
/////////////////////////////////////////////////////////////
bool PgAIActRetreat::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{

	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_IDLE:
		{
			int iDelayTime = pkUnit->GetDelay();
			if (iDelayTime <= 0)
			{
				bResult = true;
			}
			else if (GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos()) < 900)	
			{
				// TargetPos에 도착했다면 그만 전진해야 한다.
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

void PgAIActRetreat::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif
	switch(iActionType)
	{
	case 2:
		{
			OnEnter2(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg);
			return;
		}break;
	}
	PgWayPointMng* pkWayPointMng = NULL;
	pkActArg->Get(ACTARG_WAYPOINTMNG, pkWayPointMng);
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(pkGround)
	{
		pkWayPointMng->FindWayPos(pkUnit, pkGround);
	}
	int iMin = pkUnit->GetAbil(AT_RETREAT_MIN_TIME);
	int iMax = pkUnit->GetAbil(AT_RETREAT_MAX_TIME);
	if (iMin == 0)
	{
		iMin = 5000;
	}
	if (iMax < iMin)
	{
		iMax = iMin + 3000;
	}

	POINT3 kGoalPos = pkUnit->GoalPos();
	
	if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kGoalPos))
	{
		pkUnit->SetState(US_MOVE);
		pkUnit->SetDelay(iMin + BM::Rand_Index(iMax-iMin+1));
		pkUnit->SetSync(true);
	}
	else
	{
		pkUnit->SetDelay(0);	// Immediately End(Retreat)
		//INFO_LOG(BM::LOG_LV5, _T("[%s] Failed to Retreat (There is no valid location)!"), __FUNCTIONW__);
	}
}

void PgAIActRetreat::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	switch(iActionType)
	{
	case 2:
		{
			// MoveSpeed를 원래대로 돌려 놓는다.
			int iMoveSpeed = pkUnit->GetAbil(AT_RETREAT_MOVESPEED);
			if (iMoveSpeed > 0)
			{
				pkUnit->AddAbil(AT_R_MOVESPEED, 0-iMoveSpeed);
				pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
			}
#ifdef AI_DEBUG
			INFO_LOG(BM::LOG_LV9, __FL__<<L"MoveSpeed("<<pkUnit->GetAbil(AT_C_MOVESPEED)<<L")");
#endif
		}break;
	}
}

void PgAIActRetreat::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	switch(iActionType)
	{
	case 2:
		{
			DoAction2(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg);
			return;
		}break;
	}
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene());
}

void PgAIActRetreat::OnEnter2(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	POINT3 ptTargetPos;
	if (!pkGround->GetTargetPos(pkUnit, ptTargetPos))
	{
		pkUnit->SetDelay(0);
		return;
	}
	pkUnit->SetAbil(AT_WAYPOINT_GROUP, 0);
	PgWayPointMng* pkWayPointMng = NULL;
	pkActArg->Get(ACTARG_WAYPOINTMNG, pkWayPointMng);
	if (!pkWayPointMng->GetRunawayPos(pkUnit, ptTargetPos))
	{
		// 더이상 도망갈 곳이 없다.
		pkUnit->SetDelay(0);
		return;
	}
	int iMin = pkUnit->GetAbil(AT_RETREAT_MIN_TIME);
	int iMax = pkUnit->GetAbil(AT_RETREAT_MAX_TIME);
	if (iMin == 0)
	{
		iMin = 5000;
	}
	if (iMax < iMin)
	{
		iMax = iMin + 3000;
	}
	int iMoveSpeed = pkUnit->GetAbil(AT_RETREAT_MOVESPEED);
	if (iMoveSpeed > 0)
	{
		pkUnit->AddAbil(AT_R_MOVESPEED, iMoveSpeed);
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
	}

	POINT3 kGoalPos = pkUnit->GoalPos();
	
	if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kGoalPos))
	{
		pkUnit->SetState(US_MOVE);
		pkUnit->SetDelay(iMin + BM::Rand_Index(iMax-iMin+1));
		pkUnit->SetAbil(AT_C_MOVESPEED, iMoveSpeed, false, false);	// MoveSpeed값은 Action Packet 에서 보낸다.
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"MoveSpeed("<<iMoveSpeed<<L")");
#endif
		pkUnit->SetSync(true);
	}
	else
	{
		pkUnit->SetDelay(0);	// Immediately End(Patrol)
	}

}

void PgAIActRetreat::DoAction2(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene());

	if (GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos()) < 400)
	{
		POINT3 ptTargetPos;
		if (!pkGround->GetTargetPos(pkUnit, ptTargetPos))
		{
			pkUnit->SetDelay(0);
			return;
		}
		PgWayPointMng* pkWayPointMng = NULL;
		pkActArg->Get(ACTARG_WAYPOINTMNG, pkWayPointMng);
		if (!pkWayPointMng->GetRunawayPos(pkUnit, ptTargetPos))
		{
			// 더이상 도망갈 곳이 없다.
			pkUnit->SetDelay(0);
			return;
		}
		pkUnit->SetSync(true);
	}
}