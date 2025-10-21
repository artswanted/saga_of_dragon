#include "stdafx.h"
#include "Variant\constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant\PgActionResult.h"
#include "PgAIActFollowCaller.h"
#include "PgAIAction.h"
#include "AI/PgTargettingManager.h"

/////////////////////////////////////////////////////////////
// PgAIActFollowCaller
/////////////////////////////////////////////////////////////
int const WALKINGZONE_SPEED = 0;
int const RUNNINGZONE_SPEED = 10000;
void SetGoalPos(CUnit * pkUnit, PgGround * pkGround);
void GetBottomPos(POINT3 & kTargetPos, PgGround * pkGround);
void SetReturnZoneSpeed(CUnit * pkUnit, bool const bSend=false);
void SetZoneSpeed(CUnit * pkUnit, int const iSpeed);

bool PgAIActFollowCaller::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(!pkGround)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Null Ground");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Null Ground"));
		return bResult;
	}

	switch(eNextAction)
	{
	case EAI_ACTION_IDLE:
		{
			if( pkGround->IsInReturnZone(pkUnit)
			 || pkUnit->GoalPos()==POINT3::NullData()
			 || pkUnit->GoalPos()==pkUnit->GetPos() )
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_FIRESKILL:
		{
			bResult = pkGround->CanAttackTarget(pkUnit);
		}break;
	case EAI_ACTION_CHASEENEMY:
		{
			UNIT_PTR_ARRAY kUnitArray;
			bResult = pkGround->FindEnemy( pkUnit, kUnitArray );
			if(bResult)
			{
				pkUnit->SetDelay(0);
			}
		}break;
	case EAI_ACTION_RETREAT:
		{
			bResult = CheckRetreatHP(pkUnit);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<eNextAction<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return bResult;
}

void PgAIActFollowCaller::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(!pkGround)
	{
		return;
	}

	if(pkGround->IsInReturnZone(pkUnit))
	{
		return;
	}

	if(!pkUnit || false==pkUnit->IsHaveCaller())
	{
		return;
	}

	SetGoalPos(pkUnit, pkGround);
}

void PgAIActFollowCaller::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	
	SetReturnZoneSpeed(pkUnit, true);
}

void PgAIActFollowCaller::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(!pkGround)
	{
		return;
	}

	if(pkGround->IsInReturnZone(pkUnit))
	{
		return;
	}

	CUnit * pkCaller = pkGround->GetUnit(pkUnit->Caller());
	if(!pkCaller)
	{
		return;
	}

	if( (pkCaller->MoveDir() != pkUnit->MoveDir())
	 || (POINT3::Distance(pkCaller->GetPos(), pkUnit->GoalPos()) <= AI_MONSTER_MIN_DISTANCE_CHASE_Q) )
	{
		SetGoalPos(pkUnit, pkGround);
	}

	if(pkGround->IsInWalkingZone(pkUnit))
	{
		int const iWalkngZoneSpeed = pkUnit->GetAbil(AT_WALKINGZONE_SPEED_RATE);
		if(iWalkngZoneSpeed != pkUnit->GetAbil(AT_AI_RUN_ACTION_MOVESPEED))
		{
			SetZoneSpeed(pkUnit, iWalkngZoneSpeed);
		}
	}

	if(pkGround->IsInRunningZone(pkUnit))
	{
		int const iWalkngZoneSpeed = pkUnit->GetAbil(AT_WALKINGZONE_SPEED_RATE);
		int iRunningZoneSpeed = pkUnit->GetAbil(AT_RUNNINGZONE_SPEED_RATE);
		if(0==iRunningZoneSpeed)
		{
			iRunningZoneSpeed = RUNNINGZONE_SPEED;
		}
		if(iWalkngZoneSpeed < pkUnit->GetAbil(AT_AI_RUN_ACTION_MOVESPEED))
		{
			SetZoneSpeed(pkUnit, iRunningZoneSpeed);
		}
	}

	if( !pkGround->IsInRunningZone(pkUnit)
	 || !UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene(), true) )
	{
		POINT3 kTargetPos = pkCaller->GetPos();
		GetBottomPos(kTargetPos, pkGround);
		pkUnit->SetPos( kTargetPos );
		pkUnit->GoalPos(POINT3::NullData());

		if(int const iWarpNo = pkUnit->GetAbil(AT_WARP_SKILL_NO))
		{
			pkUnit->GetSkill()->ForceReserve(iWarpNo, EFSF_NOW_FIRESKILL_CANCLE);
			pkUnit->GetAI()->SetEvent(pkCaller->GetID(), EAI_EVENT_FORCE_SKILL);
		}
	}

	float const fDistQ = GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos());
	if (fDistQ < AI_MONSTER_MIN_DISTANCE_CHASE_Q)
	{
		// 도착하였다.
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}

void SetGoalPos(CUnit * pkUnit, PgGround * pkGround)
{
	if(!pkUnit || !pkGround)
	{
		return;
	}

	CUnit * pkCaller = pkGround->GetUnit(pkUnit->Caller());
	if(!pkCaller)
	{
		return;
	}

	if( pkCaller->GetCountAbil( AT_CANNOT_DAMAGE, AT_CF_HYPERMOVE ) )
	{//대점프 이동중이면 따라가지 않음
		return;
	}

	EFollowPattern const ePattern = static_cast<EFollowPattern>(pkUnit->GetAbil(AT_FOLLOW_PATTERN));
	Direction const eDir = (EFPTN_BACK==ePattern) ? DIR_LEFT : DIR_RIGHT;
	int iReturnZoneRange = pkUnit->GetAbil(AT_RETURNZONE_RANGE);
	if(0==iReturnZoneRange)
	{
		iReturnZoneRange = AI_MONSTER_MIN_DISTANCE_CHASE_Q;
	}

	POINT3BY const& kPathNormalBy = pkUnit->PathNormal();
	POINT3 Oriented(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	Oriented.Normalize();

	PgCreateSpreadPos kAction(pkCaller->GetPos());
	kAction.AddDir(pkGround->PhysXScene()->GetPhysXScene(), Oriented, iReturnZoneRange);

	POINT3 kTargetPos;
	kAction.PopPos(kTargetPos, 4);

	//float const fDistance = BM::Rand_Range(15, iReturnZoneRange);

	//POINT3 kTargetPos;
	//GetDistanceToPosition( pkGround->GetPathRoot(), pkCaller->GetPos(), eDir, fDistance, kTargetPos );
	if(false==SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
	{
		kTargetPos = pkCaller->GetPos();
		GetBottomPos(kTargetPos, pkGround);
		pkUnit->GoalPos(kTargetPos);
	}
	
	if(pkGround->IsInWalkingZone(pkUnit))
	{
		int const iWalkngZoneSpeed = pkUnit->GetAbil(AT_WALKINGZONE_SPEED_RATE);
		SetZoneSpeed(pkUnit, iWalkngZoneSpeed);
	}
	else if(pkGround->IsInRunningZone(pkUnit))
	{
		int iRunningZoneSpeed = pkUnit->GetAbil(AT_RUNNINGZONE_SPEED_RATE);
		if(0==iRunningZoneSpeed)
		{
			iRunningZoneSpeed = RUNNINGZONE_SPEED;
		}
		SetZoneSpeed(pkUnit, iRunningZoneSpeed);
	}

	pkUnit->MoveDir(pkCaller->MoveDir());

	pkUnit->SetState(US_MOVE);
	pkUnit->SetSync(true);
}

void GetBottomPos(POINT3 & kTargetPos, PgGround * pkGround)
{
	if(pkGround)
	{
		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(pkHitShape)
		{
			kTargetPos.z = kHit.worldImpact.z;
		}
	}
}

void SetReturnZoneSpeed(CUnit * pkUnit, bool const bSend)
{
	if(!pkUnit)
	{
		return;
	}

	int const iMoveSpeed = pkUnit->GetAbil(AT_AI_RUN_ACTION_MOVESPEED);
	if(iMoveSpeed > 0)
	{
		pkUnit->SetAbil(AT_AI_RUN_ACTION_MOVESPEED, 0);
		pkUnit->AddAbil(AT_R_MOVESPEED, -iMoveSpeed);
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, bSend ? E_SENDTYPE_BROADALL : E_SENDTYPE_NONE);
	}
}

void SetZoneSpeed(CUnit * pkUnit, int const iSpeed)
{
	if(!pkUnit)
	{
		return;
	}
	SetReturnZoneSpeed(pkUnit, false);

	pkUnit->SetAbil(AT_AI_RUN_ACTION_MOVESPEED, iSpeed);
	pkUnit->AddAbil(AT_R_MOVESPEED, iSpeed);
	pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
}