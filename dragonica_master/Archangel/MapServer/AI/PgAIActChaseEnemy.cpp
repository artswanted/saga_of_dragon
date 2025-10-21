#include "stdafx.h"
#include "Variant\constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant\PgActionResult.h"
#include "PgAIActChaseEnemy.h"
#include "PgAIAction.h"
#include "AI/PgTargettingManager.h"

/////////////////////////////////////////////////////////////
// PgAIActChaseEnemy
/////////////////////////////////////////////////////////////
//float PgAIActChaseEnemy::m_fProwlAreaRange = 120.0f;
//float PgAIActChaseEnemy::m_fProwlAreaRangeQ = PgAIActChaseEnemy::m_fProwlAreaRange * PgAIActChaseEnemy::m_fProwlAreaRange;
DWORD PgAIActChaseEnemy::m_dwGoalPosRefreshInternval = 1000;

int GetChaseMoveSpeed(CUnit * pkUnit, bool const bRunMove)
{
	if(!pkUnit)
	{
		return 0;
	}

	int iMoveSpeed = 0;
	if(bRunMove == false)
	{
		iMoveSpeed = pkUnit->GetAbil(AT_AI_RUN_ACTION_MOVESPEED);
		pkUnit->SetAbil(AT_AI_RUN_ACTION_MOVESPEED, 0);
	}
	else
	{
		iMoveSpeed = pkUnit->GetAbil(AT_CHASE_MOVESPEED);
		if(0 < pkUnit->GetAbil(AT_PROVOKE_EFFECT_NO))
		{
			int const iAddSpeed = pkUnit->GetAbil(AT_R_PROVOKE_MOVE_SPEED);
			iMoveSpeed += iAddSpeed;
		}
		pkUnit->SetAbil(AT_AI_RUN_ACTION_MOVESPEED, iMoveSpeed);
	}

	return iMoveSpeed;
}

bool PgAIActChaseEnemy::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);

	switch(eNextAction)
	{
	case EAI_ACTION_FIRESKILL:
		{
			if(pkUnit->GetAttackDelay() <= 0)
			{
				EChasePattern eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);

				if (0<pkUnit->GetAbil(AT_PROVOKE_EFFECT_NO) && ECPattern_Ended!=eChaseState && pkUnit->GoalPos()!=POINT3::NullData())
				{
					bResult = false;
				}
				else if (pkGround->CanAttackTarget(pkUnit))
				{
					bResult = true;
				}
				else
				{
					// 현재 선택한 스킬은 공격을 할 수 없으므로 다른 스킬을 선택할 수 있도록 Reserve된 스킬을 삭제한다.
					int const iSkillNo = pkUnit->GetSkill()->GetSkillNo();
					//INFO_LOG(BM::LOG_LV5, _T("[%d] skill can't attack"), iSkillNo);
					if (iSkillNo > 0)
					{
						unsigned long ulCheckTime = pkUnit->GetSkill()->GetCheckTime();

						if (ulCheckTime == 0)
						{
							//INFO_LOG(BM::LOG_LV5, _T("[%d] skill can't attack, so set timer"), iSkillNo);
							pkUnit->GetSkill()->SetCheckTime(BM::GetTime32());
						}
						else if (BM::GetTime32() - ulCheckTime > 1000)
						{
							//INFO_LOG(BM::LOG_LV5, _T("[%d] skill can't attack, so clear reserved skill"), iSkillNo);
							pkUnit->GetSkill()->Reserve(0);
							pkUnit->GetSkill()->SetCheckTime(0);
						}
					}
				}
			}
		}break;
	case EAI_ACTION_PATROL:
    case EAI_ACTION_ROAD:
	case EAI_ACTION_FOLLOW_CALLER:
		{
			// 타겟이 점프 중임을 알아야 한다. (점프 중인지, 단에 올라갔는지 구분이 안됨)
			if(!pkGround->IsTargetInRange(pkUnit, 100, IsDetailAI(pkUnit)))
			{
				bResult = true;
			}

			if(bResult)
			{
				UNIT_PTR_ARRAY kArray;
				bResult = !pkGround->FindEnemy(pkUnit, kArray, 5, UT_PLAYER, true, false);
			}
		}break;
	case EAI_ACTION_RETREAT:
		{
			if(pkUnit->GetState() == US_RETREAT)
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

void PgAIActChaseEnemy::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif
	pkUnit->SetAbil(AT_WAYPOINT_GROUP, 0);
	pkUnit->SetAbil(AT_WAYPOINT_INDEX, 0);

//	if(ePrevAction != EAI_ACTION_CHASEENEMY)
//	{
		int const iMoveSpeed = GetChaseMoveSpeed(pkUnit, true);
		// AT_CHASE_MOVESPEED abil이 정의되어 있지 않으면, 속도 변화는 없다.
		if (iMoveSpeed > 0)
		{
			pkUnit->AddAbil(AT_R_MOVESPEED, iMoveSpeed);
			pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
		}
//	}


	if(EAI_ACTION_CHASEENEMY==pkUnit->GetAbil(AT_AI_TYPE_NO_DAMAGEACTION))
	{
		pkUnit->AddAbil(AT_DAMAGEACTION_TYPE, E_DMGACT_AI_CHASE_ENEMY);
		pkUnit->SendAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL);
	}

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	pkUnit->GetSkill()->Reserve(pkGround->GetReserveAISkill(pkUnit), true);
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"SkillNum["<<pkUnit->GetSkill()->GetSkillNo()<<L"]");
#endif

	pkUnit->SetState(US_CHASE_ENEMY);

	pkUnit->SetAbil(AT_CHASE_PATTERN, BM::Rand_Index(2) == 0 ? ECPattern_GotoTarget : ECPattern_GotoTarget_X);// 바로 가거나. X축 맞추고 가거나.

	if(0 == pkUnit->GetAbil(AT_AI_CHASE_GOAL_POS))
	{
		// 굳이 새로 패킷을 줄 필요는 없다. GoalPos를 잡은 뒤에 패킷을 주면 됨.
		pkUnit->GoalPos(POINT3::NullData());
	}
	else
	{
		pkUnit->SetState(US_MOVE);
		pkUnit->SetSync(true);
	}
	pkUnit->SetAbil(AT_AI_CHASE_GOAL_POS, 0);
	pkUnit->GetSkill()->SetCheckTime(0);

	g_kTargettingManager.FindTarget(pkUnit, pkUnit->GetSkill()->GetSkillDef(), pkUnit->GetTargetList(), pkGround);
	pkUnit->SetReserveTargetToDeque();//예약된 타겟이 있으면 제일 앞에 추가해 주자
//	BM::GUID const & rkGuid = pkUnit->GetTarget();
//	PgActionResultVector kResultVec;
//	PgActionResult* pkResult = kResultVec.GetResult(pkUnit->GetTarget(), true);
//	pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec);
}

void PgAIActChaseEnemy::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	// 이동속도 원위치
	int const iMoveSpeed = GetChaseMoveSpeed(pkUnit, false);
	if (iMoveSpeed > 0)
	{
		pkUnit->AddAbil(AT_R_MOVESPEED, 0-iMoveSpeed);
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);	// 원래 속도대로
	}

	if(pkUnit->GetAbil(AT_DAMAGEACTION_TYPE)&E_DMGACT_AI_CHASE_ENEMY)
	{
		pkUnit->AddAbil(AT_DAMAGEACTION_TYPE, -E_DMGACT_AI_CHASE_ENEMY);
		pkUnit->SendAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL);
	}
	pkUnit->SetAbil(AT_AI_TYPE_NO_DAMAGEACTION, 0);

	pkUnit->GetSkill()->SetCheckTime(0);
}

void PgAIActChaseEnemy::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	if(pkUnit->GetDelay() > 0)
	{
		return;
	}
	EChasePattern eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(IsMoveState(eChaseState) && !UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene(), true))
	{
		// 더이상 전진할 수 없다
		pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
		//INFO_LOG(BM::LOG_LV9, _T("[%s] EAI_EVENT_CANNOTMOVE_FORWORD"), __FUNCTIONW__);
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Ended);
		eChaseState = ECPattern_Ended;
	}
	else if(0 >= pkUnit->GetAbil(AT_C_MOVESPEED))
	{
		pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Ended);
		eChaseState = ECPattern_Ended;
	}

	//int iLoopCount = 0;	// 만약을 위해 Loop 회수를 세어보자
	//EChasePattern eOldState = ECPattern_ProwlInit;
//	while (eOldState != eChaseState)	// 한번의 Tick에서 다음 행동을 결정하기 위함...
	{
#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] ChaseState[%d], LoopCount[%d]"), __FUNCTIONW__, eChaseState, iLoopCount);
#endif
		switch(eChaseState)
		{
		case ECPattern_GotoTarget_X:
			{
				PgAIActChaseEnemy::DoAction_ECPattern_GotoTarget_X(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_GotoTarget:
			{
				PgAIActChaseEnemy::DoAction_ECPattern_GotoTarget(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_Ended:
			{
				PgAIActChaseEnemy::DoAction_ECPattern_Ended(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		default:
			{
				//INFO_LOG(BM::LOG_LV5, _T("[%s] Unknown AT_CAHSE_PATTERN value [%d]"), __FUNCTIONW__, (int)eChaseState);
				pkUnit->SetTarget(BM::GUID::NullData());	// Ending of Chase
			}break;
		}
		//eOldState = eChaseState;
		//eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);
		//VERIFY_INFO_LOG_RUN(break;, VPARAM(++iLoopCount < 10, BM::LOG_LV5, _T("[%s] Too many looping ChaseState[%d]"), __FUNCTIONW__, (int)eChaseState));
	}
}

void PgAIActChaseEnemy::DoAction_ECPattern_GotoTarget_X(CUnit* pkUnit, EAIActionType eAction, int iActionType,
														DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	// 타겟으로 가는데, X축을 먼저 맞춤( ==> Y축으로 걸어감)
	POINT3 ptGoalPos = pkUnit->GoalPos();
	POINT3 const& ptUnitPos = pkUnit->GetPos();
	if( ptGoalPos == POINT3::NullData() )
	{
		// 처음 들어온 상태 --> GoalPos 설정해 주어야 한다.
		// 몬스터와 X축으로 평행한 GoalPos를 설정하면 된다.
		POINT3 const ptTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget(), IsDetailAI(pkUnit));
		NxVec3 const kVec1(ptTargetPos.x-ptUnitPos.x, ptTargetPos.y-ptUnitPos.y, ptTargetPos.z-ptUnitPos.z);
		POINT3BY const& ptNormalBy = pkGround->GetPathNormal(pkUnit->GetTarget());
		NxVec3 kNormalVec(ptNormalBy.x, ptNormalBy.y, 0);
		kNormalVec.normalize();

		float const fNormalLen = kNormalVec.dot(kVec1);
		if (abs(fNormalLen) < 15)	// 평행이동할 거리가 너무 짧으니, Target에게 바로 가도록 한다.
		{
			pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget);
			return;
		}
		kNormalVec.multiply(fNormalLen, kNormalVec);
		ptGoalPos.x = ptUnitPos.x + kNormalVec.x; 
		ptGoalPos.y = ptUnitPos.y + kNormalVec.y; 
		ptGoalPos.z = ptUnitPos.z + kNormalVec.z;
		if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptGoalPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
		{
			// Client에 이동명령 보낸다.
			pkUnit->SetState(US_MOVE);
			pkUnit->SetSync(true);
		}
		else
		{
			//INFO_LOG(BM::LOG_LV0, _T("[%s] Goal Position is invalid! (%.4f, %.4f, %.4f)"), __FUNCTIONW__, ptGoalPos.x, ptGoalPos.y, ptGoalPos.z);
			//pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
			pkUnit->SetState(US_RETREAT);
			if(true==g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_PATROL))
			{
				pkUnit->GetAI()->SetEvent(pkUnit->GetTarget(), EAI_EVENT_RETURN_WAYPOINT);
			}
		}
		return;
	}

	// GoalPos에 도착하였는가?
	float const fDistQ = GetDistanceQ(pkUnit->GetPos(), ptGoalPos);
	if (fDistQ < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		// 도착하였다. --> 다음 Chase 상태 결정해야 한다.
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget_X);
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}

void PgAIActChaseEnemy::DoAction_ECPattern_GotoTarget(CUnit* pkUnit, EAIActionType eAction, int iActionType,
													  DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	// Target으로 직접 간다.
	bool bResult = true;
	POINT3 const& ptGoalPos = pkUnit->GoalPos();
	POINT3 const& ptUnitPos = pkUnit->GetPos();
	if( ptGoalPos == POINT3::NullData() )
	{
		// 바로 공격하러 감.
		int const iMinDistance = GetMinDistanceFromTarget(pkUnit);//__max(AI_MONSTER_MIN_DISTANCE_FROM_TARGET, pkUnit->GetAbil(AT_MON_MIN_RANGE));
		POINT3 ptTargetPos;
		if(!pkGround->GetAroundVector(pkUnit, pkUnit->GetTarget(), (float)iMinDistance, true, ptTargetPos))
		{
			bResult = false;
		}
		
		if(bResult)
		{
			if(!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTargetPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
			{
				bResult = false;
				if(pkGround->GetAroundVector(pkUnit, pkUnit->GetTarget(), (float)iMinDistance, false, ptTargetPos))//Near위치가 에러면 Far위치로 다시 검사 하자
				{
					bResult = SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTargetPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround);
				}
			}
		}

		if (!bResult && GetDistanceQ(ptUnitPos, ptTargetPos) < AI_GOALPOS_ARRIVE_DISTANCE_Q*0.5f)
		{
			bResult = true;
		}

		if(bResult)
		{
			POINT3 const kDiff = ptTargetPos - ptUnitPos;
			NxVec3 kDiffVec(kDiff.x, kDiff.y, 0.0f);

			if(kDiffVec.magnitudeSquared() > AI_MONSTER_MIN_DISTANCE_CHASE_Q)
			{
				// Client에 이동명령 보낸다.
				pkUnit->SetState(US_MOVE);
				pkUnit->SetSync(true);
			}
		}
		else
		{
			// TODO : 목표 좌표로 못가는 경우 처리 해주어야 함
			//INFO_LOG(BM::LOG_LV9, _T("[%s] Goal Position is invalid! (%.4f, %.4f, %.4f)"), __FUNCTIONW__, ptGoalPos.x, ptGoalPos.y, ptGoalPos.z);
			//pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
			if(pkUnit->GetTarget().IsNotNull())
			{
				pkUnit->SetState(US_RETREAT);
				if(true==g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_PATROL))
				{
					pkUnit->GetAI()->SetEvent(pkUnit->GetTarget(), EAI_EVENT_RETURN_WAYPOINT);
				}
			}
		}
		return;
	}

	// GoalPos에 도착하였는가?
	float const fDistQ = GetDistanceQ(ptUnitPos, ptGoalPos);
	if (fDistQ < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		// 도착하였다. --> 다음 Chase 상태 결정해야 한다.
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Ended);
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}

void PgAIActChaseEnemy::DoAction_ECPattern_Ended(CUnit* pkUnit, EAIActionType eAction, int iActionType,
													  DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_GotoTarget);
	pkUnit->GoalPos(POINT3::NullData());
}

bool PgAIActChaseEnemy::IsMoveState(EChasePattern eChasePattern)
{
	bool bMove = true;
	switch(eChasePattern)
	{
	case ECPattern_InProwlArea:
	case ECPattern_BattleIdle:
	case ECPattern_Ended:
		bMove = false;
		break;
	}
	return bMove;
}

//void PgAIActChaseEnemy::DoActionDefault(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
//{
//	PgGround* pkGround = NULL;
//	pkActArg->Get(ACTARG_GROUND, pkGround);
//	int iChaseDone = pkUnit->GetAbil(AT_CHASE_DONE);
//	int const iCheckTargetTerm = 500;
//
//	if(pkUnit->GetAI()->GetEvent() == EAI_EVENT_DAMAGED)
//	{
//		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_APPROACH_TARGET);
//		iChaseDone = 1;
//	}
//	
//	if(iChaseDone == 0)
//	{
//		// Now Chasing
//		if(!UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene(), true))
//		{
//			// 더이상 전진할 수 없다
//			pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
//			INFO_LOG(BM::LOG_LV9, _T("[%s] EAI_EVENT_CANNOTMOVE_FORWORD"), __FUNCTIONW__);
//			pkUnit->SetAbil(AT_CHASE_DONE, 1);
//			//pkUnit->SetAbil(AT_CHASE_PATTERN, 1);
//		}
//		else
//		{
//			POINT3 kDistVec = pkUnit->GetPos() - pkUnit->GoalPos();
//			NxVec3 kLengthVec(kDistVec.x, kDistVec.y, 0);
//
//			if(kLengthVec.magnitude() <= 2.0f)
//			{
//				pkUnit->SetAbil(AT_CHASE_DONE, 1);
//				POINT3 kGoalPoint = pkUnit->GoalPos();
//				kGoalPoint.z = pkUnit->GetPos().z;
//				pkUnit->SetPos(kGoalPoint);
//			}
//		}
//
//		int iChaseTime = pkUnit->GetAbil(AT_CHASE_ELAPSED_TIME);
//		int iElapsedChaseTime = iChaseTime + dwElapsedTime;
//
//		DWORD dwGoalPosTime = (DWORD) pkUnit->GetAbil(AT_GOALPOS_SET_TIME);
//		if (BM::GetTime32() - dwGoalPosTime > m_dwGoalPosRefreshInternval)
//		{
//			// GoalPos 값을 다시 update 시켜주자.
//			BM::GUID kTargetGuid = pkUnit->GetTarget();
//			if(kTargetGuid == BM::GUID::NullData())
//			{
//				INFO_LOG(BM::LOG_LV9, _T("Chasing Target is Null"));
//				return;
//			}
//			POINT3 ptTarget = pkGround->GetUnitPos(kTargetGuid);
//			POINT3 ptRelativeGoalPos = pkUnit->RelativeGoalPos();
//			POINT3 ptCurGoalPos = pkUnit->GoalPos();
//			POINT3 ptNewGoalPos = ptTarget + ptRelativeGoalPos;
//			/*POINT3 ptDiff = ptCurGoalPos - ptNewGoalPos;
//			NxVec3 kDiff(ptDiff.x, ptDiff.y, ptDiff.z);
//			if(kDiff.magnitude() > 10.0f)
//			{
//				POINT3 ptCompareGoalPos1 = pkUnit->GetPos();
//				POINT3 ptCompareGoalPos2 = ptNewGoalPos;
//				ptCompareGoalPos1.z = ptCompareGoalPos2.z = 0;
//				if(ptCompareGoalPos1 != ptCompareGoalPos2)
//				{
//					if(ptCompareGoalPos1.x < ptTarget.x)
//					{
//						ptNewGoalPos.x *= 1.0f;
//					}
//					else
//					{
//						ptNewGoalPos.x *= (-1.0f);
//					}*/
//
//					pkUnit->GoalPos(ptNewGoalPos);
//					pkGround->SendNftChaseEnemy(pkUnit);
////				}
////			}
//		}
//
//		// iCheckTargetTerm보다 크면, 타겟 위치에 대하여 Goal Pos를 다시 잡아준다.
//		/*
//		if(iElapsedChaseTime > iCheckTargetTerm)
//		{
//			BM::GUID kTargetGuid = pkUnit->GetTarget();
//			if(kTargetGuid == BM::GUID::NullData())
//			{
//				INFO_LOG(BM::LOG_LV9, _T("Chasing Target is Null"));
//				return;
//			}
//
//			POINT3 ptTarget = pkGround->GetUnitPos(kTargetGuid);
//			POINT3 ptRelativeGoalPos = pkUnit->RelativeGoalPos();
//			POINT3 ptCurGoalPos = pkUnit->GoalPos();
//			POINT3 ptNewGoalPos = ptTarget + ptRelativeGoalPos;
//			POINT3 ptDiff = ptCurGoalPos - ptNewGoalPos;
//			NxVec3 kDiff(ptDiff.x, ptDiff.y, ptDiff.z);
//			if(kDiff.magnitude() > 10.0f)
//			{
//				pkUnit->GoalPos(ptNewGoalPos);
//				pkGround->SendNftChaseEnemy(pkUnit);
//			}
//
//			iElapsedChaseTime = 0;
//		}
//		*/
//		
//		pkUnit->SetAbil(AT_CHASE_ELAPSED_TIME, iElapsedChaseTime);
//	}
//	else
//	{
//		// End of chase.
//		// Check unit has got target,
//		// Or must be set a new target Pos.
//		BM::GUID kTargetGuid = pkUnit->GetTarget();
//		if(kTargetGuid == BM::GUID::NullData())
//		{
//			INFO_LOG(BM::LOG_LV9, _T("Chasing Target is Null"));
//			return;
//		}
//
//		POINT3 ptUnitPos = pkUnit->GetPos();
//		POINT3 ptTarget = pkGround->GetUnitPos(kTargetGuid);
//		
//		NxVec3 kUnitPos(ptUnitPos.x, ptUnitPos.y, ptUnitPos.z);
//		NxVec3 kTargetPos(ptTarget.x, ptTarget.y, ptTarget.z);
//		float fDistFromGoalPos = (kUnitPos - kTargetPos).magnitude();
//
//		//if(fDistFromGoalPos < fMaxGoalDistance && fDistFromGoalPos > fMinGoalDistance)
//		//{
//		//	// Arrived at the goal position.
//		//	return;
//		//}
//		
//		EChasePattern eChasePattern = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);		// 1 : go a shortway, 2 : go a longway
//		POINT3BY kPathNormalBy = pkGround->GetPathNormal(kTargetGuid);
//		NxVec3 kPathNormal(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);	// front direction
//		kPathNormal.normalize();
//
//		NxVec3 kRightVec = kPathNormal.cross(NxVec3(0,0,1));
//
//		NxVec3 kFirst = kTargetPos + kRightVec * 45.0f;
//		NxVec3 kSecond = kTargetPos - kRightVec * 45.0f;
//
//		NxVec3 kNearTarget;
//		NxVec3 kFarTarget;
//
//		INFO_LOG(BM::LOG_LV9, _T("----------- [Chase Pattern] PathNormal : %.3f, %.3f, %.3f (Unit : %p) -------------"), kPathNormal.x, kPathNormal.y, kPathNormal.z, pkUnit);
//		
//		if((kFirst - kUnitPos).magnitudeSquared() > (kSecond - kUnitPos).magnitudeSquared())
//		{
//			kFarTarget = kFirst;
//			kNearTarget = kSecond;
//		}
//		else
//		{
//			kFarTarget = kSecond;
//			kNearTarget = kFirst;
//		}
//
//		NxVec3 kObliqueVector = kUnitPos - kTargetPos;
//		kObliqueVector.z = 0.0f;
//		NxVec3 kProjectionVec = kObliqueVector.dot(kRightVec) / kRightVec.dot(kRightVec) * kRightVec;
//		NxVec3 kPerpendicularVec = kProjectionVec - kObliqueVector;
//
//		if(kPerpendicularVec.magnitude() <= 0.001f)
//		{
//			// 평행할 때는 PathNormal방향으로 움직인다.
//			kPerpendicularVec =  kPathNormal;
//			kPerpendicularVec *= 50.0f;
//		}
//
//		NxVec3 kNormalizedPerpendicularVec(kPerpendicularVec);
//		NxVec3 kGoalPos(0.0f, 0.0f, 0.0f);
//
//		kNormalizedPerpendicularVec.normalize();
//		kNearTarget += kNormalizedPerpendicularVec * NxReal(-15 + BM::Rand_Index(30));
//		kFarTarget += kNormalizedPerpendicularVec * NxReal(-15 + BM::Rand_Index(30));
//
//		char szStr[2048];
//		NxVec3 kFarDirection = kFarTarget - kUnitPos;
//		sprintf_s(szStr, 2048, "---------------@ChaseEnemy => ChasePatter [%d]@ -------------	\
//							\n\tkUnit : %.2f, %.2f, %.2f\t\tTarget : %.2f, %.2f, %.2f \
//							\n\tkObliqueVec : %.2f, %.2f, %.2f\t\t kProjection : %.2f, %.2f, %.2f\
//							\n\tkPerpendicular : %.2f, %.2f, %.2f\t\t kAwayVec : %.2f, %.2f, %.2f\n",
//							eChasePattern, kUnitPos.x, kUnitPos.y, kUnitPos.z, kTargetPos.x, kTargetPos.y, kTargetPos.z,
//							kObliqueVector.x, kObliqueVector.y, kObliqueVector.z, kProjectionVec.x, kProjectionVec.y, kProjectionVec.z,
//							kPerpendicularVec.x, kPerpendicularVec.y, kPerpendicularVec.z, (kFarDirection - kPerpendicularVec).x, (kFarDirection - kPerpendicularVec).y, (kFarDirection - kPerpendicularVec).z
//							);
//							
//		OutputDebugStringA(szStr);
//
//		switch(eChasePattern)
//		{
//		case ECPattern_MoonWalk:// Away from target
//			{
//				NxVec3 kFarDirection = kFarTarget - kUnitPos;
//				NxVec3 kAwayVec = (kFarDirection - kPerpendicularVec);
//				NxVec3 kNormalizedAwayVec(kAwayVec);
//				kNormalizedAwayVec.normalize();
//
//				kGoalPos = kUnitPos - kAwayVec + kNormalizedAwayVec * NxReal(BM::Rand_Index(100));
//				INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern 0 => Away From Target] GoalPos : %.3f, %.3f, %.3f"), kGoalPos.x, kGoalPos.y, kGoalPos.z);
//			}break;
//		case ECPattern_Y_FAR_MOVE:	// Away from perpendicular
//			{
//				if(kPerpendicularVec.magnitude() <= 50)
//				{
//					kPerpendicularVec.normalize();
//					kPerpendicularVec *= NxReal(BM::Rand_Index(60));
//					kGoalPos = kUnitPos - kPerpendicularVec;
//				}
//							
//				INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern 1 => Away from perpendicular] GoalPos : %.3f, %.3f, %.3f"), kGoalPos.x, kGoalPos.y, kGoalPos.z);
//			}break;
//		case ECPattern_X_GOBACK:	// walk to far point (only horizontal)
//			{
//				NxVec3 kFarDirection = kFarTarget - kUnitPos;
//				NxVec3 kAwayVec = (kFarDirection - kPerpendicularVec);
//				NxVec3 kNormalziedAwayVec(kAwayVec);
//				kNormalziedAwayVec.normalize();
//
//				kGoalPos = kUnitPos + kAwayVec + kNormalziedAwayVec * NxReal(-30 + BM::Rand_Index(150));
//				INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern 2 => Walk to far point(only horizontal)] GoalPos : %.3f, %.3f, %.3f"), kGoalPos.x, kGoalPos.y, kGoalPos.z);
//			}break;
//		case ECPattern_Y_GOBACK: // walk to perpendicular
//			{
//				kGoalPos = kUnitPos + kPerpendicularVec;
//				INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern 3 => Walk to perpendicular] GoalPos : %.3f, %.3f, %.3f"), kGoalPos.x, kGoalPos.y, kGoalPos.z);
//
//				//int iRand = rand() % 500;
//				//if(iRand > 250)
//				//{
//				//	iChasePattern = 0;
//				//}
//			}break;
//		case ECPattern_APPROACH_TARGET:
//			{
//				//iChasePattern = 0;
//				
//
//				//if(kPerpendicularVec.magnitude() < 0.000001f)
//				//{
//				//	// TODO : 몬스터와 Target의 좌표가, 일직선 상에 있어서 그렇다. 어디로 움직여야 할지???
//				//	// 어쨌거나, Perpendicular Vector를 찾아야 함.
//				//	INFO_LOG(BM::LOG_LV9, _T("[PgAiActChaseEnemy.DoAction] The perpendicular vector is zero, passing the chracter!!"));
//				//	pkUnit->SetAbil(AT_CHASE_PATTERN, iChasePattern - 1);
//				//	pkUnit->SetAbil(AT_CHASE_DONE, 1);
//				//	return;
//				//}
//				//else
//				//{
//				kGoalPos = kNearTarget;
//				INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern 4 => Walk to near point] GoalPos : %.3f, %.3f, %.3f"), kGoalPos.x, kGoalPos.y, kGoalPos.z);
//				//}
//			}break;
//		default:
//			{
//				pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_APPROACH_TARGET);
//				pkUnit->SetAbil(AT_CHASE_DONE, 0);
//
//				//if(BM::Rand_Index(1000) > 900)
//				//{
//				//	pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Y_FAR_MOVE);	// 뒤로 돌아 가겠다.
//				//	INFO_LOG(BM::LOG_LV9, _T("[New Chase Pattern 1 => Away from perpendicular]"));
//				//}
//				//else
//				//{
//				//	pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Y_GOBACK);		// 그냥 앞으로 다가가겠다(X축을 나란하게 먼저 맞추고 Target에게 다가가기)
//				//	INFO_LOG(BM::LOG_LV9, _T("[New Chase Pattern 3 => Walk to perpendicular]"));
//				//}
//				return;
//			}break;
//		}
//		pkUnit->SetAbil(AT_CHASE_PATTERN, int(eChasePattern)+1);
//
//		if(kGoalPos != NxVec3(0.0f, 0.0f, 0.0f))
//		{
//			pkUnit->SetAbil(AT_CHASE_DONE, 0);
//
//			NxVec3 kRelativeGoal = kGoalPos - kTargetPos;
//			pkUnit->RelativeGoalPos(POINT3(kRelativeGoal.x, kRelativeGoal.y, kRelativeGoal.z));
//
//			POINT3 ptComparePos1 = pkUnit->GetPos();
//			POINT3 ptComparePos2(kGoalPos.x, kGoalPos.y, kGoalPos.z);
//			ptComparePos1.z = ptComparePos2.z = 0;
//			if(ptComparePos1 != ptComparePos2)
//			{
//				//pkUnit->GoalPos(POINT3(kGoalPos.x, kGoalPos.y, kGoalPos.z + 25));
//				if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, POINT3(kGoalPos.x, kGoalPos.y, kGoalPos.z + 25)))
//				{
//					//pkGround->SendNftChaseEnemy(pkUnit);
//					pkUnit->SetState(US_MOVE);
//					pkUnit->SetSync(true);
//					//INFO_LOG(BM::LOG_LV9, _T("[Chase Pattern %d]_______Send Nfy Chase Enemy!!! GoalPos : %.3f, %.3f, %.3f"), int(eChasePattern)+1, kGoalPos.x, kGoalPos.y, kGoalPos.z);
//				}
//			}
//		}
//	}
//}
