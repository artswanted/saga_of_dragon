#include "stdafx.h"
#include "Variant\constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant\PgActionResult.h"
#include "Variant\PgControlDefMgr.h"
#include "PgAIActKeepDistance.h"
#include "PgAIActChaseEnemy.h"
#include "PgAIAction.h"

/////////////////////////////////////////////////////////////
// PgAIActKeepDistance
/////////////////////////////////////////////////////////////

bool PgAIActKeepDistance::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);

	switch(eNextAction)
	{
	case EAI_ACTION_FIRESKILL:
		{
			if(pkUnit->GetAttackDelay() <= 0 && pkGround->CanAttackTarget(pkUnit))
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_PATROL:
		{
			if(!pkGround->IsTargetInRange(pkUnit, 210))
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_RETREAT:
		{
			if(pkUnit->GetAI()->GetEvent() == EAI_EVENT_CANNOTMOVE_FORWORD)
			{
				pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_NONE);
				bResult = true;
			}
		}break;
	case EAI_ACTION_AROUND:
		{
			if(pkUnit->GetState() == US_AROUND_ENEMY)
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

void PgAIActKeepDistance::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, "ENTERED");
#endif
	pkUnit->SetAbil(AT_WAYPOINT_GROUP, 0);
	pkUnit->SetAbil(AT_WAYPOINT_INDEX, 0);

	if(ePrevAction != EAI_ACTION_CHASEENEMY)
	{
		int const iMoveSpeed = pkUnit->GetAbil(AT_CHASE_MOVESPEED);
		// AT_CHASE_MOVESPEED abil이 정의되어 있지 않으면, 속도 변화는 없다.
		if (iMoveSpeed > 0)
		{
			pkUnit->AddAbil(AT_R_MOVESPEED, iMoveSpeed);
			pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
		}
	}

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	pkUnit->GetSkill()->Reserve(pkGround->GetReserveAISkill(pkUnit), true);
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"SkillNum["<<pkUnit->GetSkill()->GetSkillNo()<<L"]");
#endif

	pkUnit->SetState(US_CHASE_ENEMY);

	if(BM::Rand_Index(2) == 0)
	{
		// X축만 맞추거나
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget_X);
	}
	else
	{
		// 거리를 유지하거나
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_KeepDistance);		
	}

	// 굳이 새로 패킷을 줄 필요는 없다. GoalPos를 잡은 뒤에 패킷을 주면 됨.
	pkUnit->GoalPos(POINT3::NullData());
//	BM::GUID const & rkGuid = pkUnit->GetTarget();
//	PgActionResultVector kResultVec;
//	PgActionResult* pkResult = kResultVec.GetResult(pkUnit->GetTarget(), true);
//	pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec);
}

void PgAIActKeepDistance::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	// 이동속도 원위치
	int const iMoveSpeed = pkUnit->GetAbil(AT_CHASE_MOVESPEED);
	if (iMoveSpeed > 0)
	{
		pkUnit->AddAbil(AT_R_MOVESPEED, 0-iMoveSpeed);
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);	// 원래 속도대로
	}
}

void PgAIActKeepDistance::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	EChasePattern eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(PgAIActChaseEnemy::IsMoveState(eChaseState) && !UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene(), true))
	{
		// 더이상 전진할 수 없다
		pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
		//INFO_LOG(BM::LOG_LV9, _T("[%s] EAI_EVENT_CANNOTMOVE_FORWORD"), __FUNCTIONW__);
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Ended);
		eChaseState = ECPattern_Ended;
	}

	int iLoopCount = 0;	// 만약을 위해 Loop 회수를 세어보자
	EChasePattern eOldState = ECPattern_ProwlInit;
	while (eOldState != eChaseState)	// 한번의 Tick에서 다음 행동을 결정하기 위함...
	{
#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] ChaseState[%d], LoopCount[%d]"), __FUNCTIONW__, eChaseState, iLoopCount);
#endif
		switch(eChaseState)
		{
		case ECPattern_GotoTarget_X:
			{
				PgAIActKeepDistance::DoAction_ECPattern_GotoTarget_X(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_KeepDistance:
			{
				PgAIActKeepDistance::DoAction_ECPattern_KeepDistance(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_Ended:
			{
				pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget);
				pkUnit->GoalPos(POINT3::NullData());
			}break;
		default:
			{
				//INFO_LOG(BM::LOG_LV5, _T("[%s] Unknown AT_CAHSE_PATTERN value [%d]"), __FUNCTIONW__, (int)eChaseState);
				pkUnit->SetTarget(BM::GUID::NullData());	// Ending of Chase
			}break;
		}
		eOldState = eChaseState;
		eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);
		if( ++iLoopCount >= 10 )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Too many looping ChaseState["<<eChaseState<<L"]");
		}
	}
}

void PgAIActKeepDistance::DoAction_ECPattern_KeepDistance(CUnit* pkUnit, EAIActionType eAction, int iActionType,
													  DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	bool bResult = true;
	POINT3 const& ptGoalPos = pkUnit->GoalPos();
	POINT3 const& ptUnitPos = pkUnit->GetPos();
	if( ptGoalPos == POINT3::NullData() )
	{
		// 적과의 거리를 유지함.
		static int const iMinRange = 30;
		CSkill *pkSkill = pkUnit->GetSkill();
		int const iSkillNo = pkSkill->GetSkillNo();
		int iMaxRange = 0;
		if(iSkillNo)
		{
			int const iSkillRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
			iMaxRange = static_cast<int>((iSkillRange * 0.8f) + (BM::Rand_Index(iSkillRange) * 0.2f));
		}

		iMaxRange = NiMax(iMinRange, iMaxRange);

		POINT3 ptTargetPos;
		if(!pkGround->GetAroundVector(pkUnit, pkUnit->GetTarget(), static_cast<float>(iMaxRange), true, ptTargetPos))
		{
			bResult = false;
		}
		
		if(bResult && !SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTargetPos))
		{
			bResult = false;
		}

		if(false == bResult)
		{
			bResult = true;
			if(!pkGround->GetAroundVector(pkUnit, pkUnit->GetTarget(), static_cast<float>(iMaxRange), false, ptTargetPos))
			{
				bResult = false;
			}
			
			if(bResult && !SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTargetPos))
			{
				bResult = false;
			}
		}

		if(bResult)
		{
			POINT3 const kDiff = ptTargetPos - ptUnitPos;
			NxVec3 kDiffVec(kDiff.x, kDiff.y, 0.0f);

			if(kDiffVec.magnitudeSquared() > 25.0f)
			{
				// Client에 이동명령 보낸다.
				pkUnit->SetState(US_MOVE);
				pkUnit->SetSync(true);
			}
		}
		else
		{
			// 목표 좌표로 못가는 경우 Around로 돌리자.
			//INFO_LOG(BM::LOG_LV9, _T("[%s] Goal Position is invalid! (%.4f, %.4f, %.4f) => To Around"), __FUNCTIONW__, ptGoalPos.x, ptGoalPos.y, ptGoalPos.z);
			pkUnit->SetState(US_AROUND_ENEMY);
		}
		return;
	}

	// GoalPos에 도착하였는가?
	float const fDistQ = GetDistanceQ(ptUnitPos, ptGoalPos);
	if (fDistQ < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		// 도착하였다. --> 다음 Chase 상태 결정해야 한다.
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget_X);
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}

void PgAIActKeepDistance::DoAction_ECPattern_GotoTarget_X(CUnit* pkUnit, EAIActionType eAction, int iActionType,
														DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	// 타겟으로 가는데, X축을 먼저 맞춤( ==> Y축으로 걸어감)
	POINT3 ptGoalPos = pkUnit->GoalPos();
	POINT3 ptUnitPos = pkUnit->GetPos();
	if( ptGoalPos == POINT3::NullData() )
	{
		// 처음 들어온 상태 --> GoalPos 설정해 주어야 한다.
		// 몬스터와 X축으로 평행한 GoalPos를 설정하면 된다.
		POINT3 ptTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget(), true);
		NxVec3 kVec1(ptTargetPos.x-ptUnitPos.x, ptTargetPos.y-ptUnitPos.y, ptTargetPos.z-ptUnitPos.z);
		POINT3BY ptNormalBy = pkGround->GetPathNormal(pkUnit->GetTarget());
		NxVec3 kNormalVec(ptNormalBy.x, ptNormalBy.y, 0);
		kNormalVec.normalize();

		float fNormalLen = kNormalVec.dot(kVec1);
		if (fNormalLen < 15)	// 평행이동할 거리가 너무 짧으니, 거리를 유지한다.
		{
			pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_KeepDistance);
			return;
		}
		kNormalVec.multiply(kNormalVec.dot(kVec1), kNormalVec);
		ptGoalPos.x = ptUnitPos.x + kNormalVec.x; 
		ptGoalPos.y = ptUnitPos.y + kNormalVec.y; 
		ptGoalPos.z = ptUnitPos.z + kNormalVec.z;
		if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptGoalPos))
		{
			// Client에 이동명령 보낸다.
			pkUnit->SetState(US_MOVE);
			pkUnit->SetSync(true);
		}
		else
		{
			//INFO_LOG(BM::LOG_LV9, _T("[%s] Goal Position is invalid! (%.4f, %.4f, %.4f)"), __FUNCTIONW__, ptGoalPos.x, ptGoalPos.y, ptGoalPos.z);
			pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
		}
		return;
	}

	// GoalPos에 도착하였는가?
	float fDistQ = GetDistanceQ(pkUnit->GetPos(), ptGoalPos);
	if (fDistQ < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		// 도착하였다. --> 다음 Chase 상태 결정해야 한다.
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_KeepDistance);
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}