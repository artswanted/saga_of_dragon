#include "stdafx.h"
#include "Variant\constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant\PgActionResult.h"
#include "PgAIActAround.h"
#include "PgAIAction.h"

/////////////////////////////////////////////////////////////
// PgAIActAround
/////////////////////////////////////////////////////////////
//float PgAIActAround::m_fProwlAreaRange = 160.0f;	
//float PgAIActAround::m_fProwlAreaRangeQ = PgAIActAround::m_fProwlAreaRange * PgAIActAround::m_fProwlAreaRange;
DWORD PgAIActAround::m_dwGoalPosRefreshInternval = 1000;

bool PgAIActAround::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	EUnitState eState = pkUnit->GetState();

	switch(eNextAction)
	{
	case EAI_ACTION_DELAY:
		{
			bResult = true;
		}break;
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_KEEP_DISTANCE:
		{
			if(eState == US_CHASE_ENEMY)
			{
				pkUnit->SetDelay(0);
				bResult = true;
			}
		}break;
	case EAI_ACTION_FIRESKILL:
		{
			if(pkUnit->GetAttackDelay() <= 0 && pkGround->CanAttackTarget(pkUnit))
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_PATROL:
		{
			if (!pkGround->IsTargetInRange(pkUnit, 210, IsDetailAI(pkUnit)))
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_RETREAT:
		{
			if (pkUnit->GetAI()->GetEvent() == EAI_EVENT_CANNOTMOVE_FORWORD)
			{
				pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_NONE);
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

void PgAIActAround::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif
	pkUnit->SetAbil(AT_WAYPOINT_GROUP, 0);
	pkUnit->SetAbil(AT_WAYPOINT_INDEX, 0);

	/*
	int iMoveSpeed = pkUnit->GetAbil(AT_CHASE_MOVESPEED);
	// AT_CHASE_MOVESPEED abil이 정의되어 있지 않으면, 속도 변화는 없다.
	if (iMoveSpeed > 0)
	{
		pkUnit->AddAbil(AT_R_MOVESPEED, iMoveSpeed);
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV9, _T("[%s] Chase Begin Speed=%d"), __FUNCTIONW__, pkUnit->GetAbil(AT_C_MOVESPEED));
#endif
	}
	*/

	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	pkUnit->GetSkill()->Reserve(pkGround->GetReserveAISkill(pkUnit), true);
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"SkillNum["<<pkUnit->GetSkill()->GetSkillNo()<<L"]");
#endif

	BM::GUID const & rkGuid = pkUnit->GetTarget();
	POINT3 ptTarget = pkGround->GetUnitPos(rkGuid, true);

	//먼저 유효한 위치인지 검사하자
	if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTarget, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
	{
		// 일단 Prowl Area 영역 안으로 ..
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_AccessProwlArea);
	}
	else
	{
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_Ended);//유효한 위치가 아님
	}

	pkUnit->SetState(US_AROUND_ENEMY);
	PgActionResultVector kResultVec;
	PgActionResult* pkResult = kResultVec.GetResult(pkUnit->GetTarget(), true);
	pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec);
}

void PgAIActAround::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);

	/*
	if(eNextAction != EAI_ACTION_CHASEENEMY)
	{
		// 이동속도 원위치
		// AT_CHASE_MOVESPEED abil이 정의되어 있지 않으면, 속도 변화는 없다.
		int iMoveSpeed = pkUnit->GetAbil(AT_CHASE_MOVESPEED);
		if (iMoveSpeed > 0)
		{
			pkUnit->AddAbil(AT_R_MOVESPEED, 0-iMoveSpeed);
			pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL);	// 원래 속도대로
		}
	}
	*/

	if(eNextAction == EAI_ACTION_DELAY && pkGround)
	{
		pkUnit->SetState(US_IDLE);
		PgActionResultVector kResultVec;
		PgActionResult* pkResult = kResultVec.GetResult(pkUnit->GetTarget(), true);
		pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec);
	}
}

void PgAIActAround::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
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

	int iLoopCount = 0;	// 만약을 위해 Loop 회수를 세어보자
	EChasePattern eOldState = ECPattern_ProwlInit;
//	while (eOldState != eChaseState)	// 한번의 Tick에서 다음 행동을 결정하기 위함...
	{
#ifdef AI_DEBUG
		//INFO_LOG(BM::LOG_LV9, _T("[%s] ChaseState[%d], LoopCount[%d]"), __FUNCTIONW__, eChaseState, iLoopCount);
#endif
		switch(eChaseState)
		{
		case ECPattern_AccessProwlArea:		// 배회 위치를 잡음 
			{
				PgAIActAround::DoAction_ECPattern_AccessProwlArea(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_TakeProwl:			// 배회할 위치 잡아서, 움직이기 시작
			{
				PgAIActAround::DoAction_ECPattern_TakeProwl(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		case ECPattern_Ended:				// Around를 끝냄.
			{
				PgAIActAround::DoAction_ECPattern_Ended(pkUnit, eAction, iActionType, dwElapsedTime, pkActArg, pkGround);
			}break;
		default:
			{
				//INFO_LOG(BM::LOG_LV5, _T("[%s] Unknown AT_CAHSE_PATTERN value [%d]"), __FUNCTIONW__, (int)eChaseState);
				pkUnit->SetTarget(BM::GUID::NullData());	// Ending of Chase
			}break;
		}
		eOldState = eChaseState;
		eChaseState = (EChasePattern) pkUnit->GetAbil(AT_CHASE_PATTERN);
		//VERIFY_INFO_LOG_RUN(break;, VPARAM(++iLoopCount < 10, BM::LOG_LV5, _T("[%s] Too many looping ChaseState[%d]"), __FUNCTIONW__, (int)eChaseState));
	}
}

void PgAIActAround::DoAction_ECPattern_AccessProwlArea(CUnit* pkUnit, EAIActionType eAction, int iActionType,
														   DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	DWORD const dwGoalPosTime = (DWORD) pkUnit->GetAbil(AT_GOALPOS_SET_TIME);
	if (BM::GetTime32() - dwGoalPosTime > m_dwGoalPosRefreshInternval)
	{
		// GoalPos 값을 다시 update 시켜주자.
		POINT3 ptTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget(), true);
		SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptTargetPos);
		pkGround->SendNftChaseEnemy(pkUnit);
	}

	float fRange = static_cast<float>(pkUnit->GetAbil(AT_PROWLAREA_RANGE));
	if (fRange == 0)
	{
		fRange = 160.0f;	// Default value
	}
	float const fDistQ = GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos());
	if (fDistQ < fRange * fRange)
	{
		int iBoldness = pkUnit->GetAbil(AT_BOLDNESS_RATE);
		iBoldness = (iBoldness == 0) ? MAKE_ABIL_RATE(35) : iBoldness;
		iBoldness += (int)(pkGround->GetAdditionalAttackProb(pkUnit));
#ifdef AI_DEBUG
		INFO_LOG(BM::LOG_LV9, __FL__<<L"Boldness["<<iBoldness<<L"]");
#endif

		if(BM::Rand_Index(ABILITY_RATE_VALUE) <= iBoldness)
		{	// 공격 시작 (Chase)
			pkUnit->SetState(US_CHASE_ENEMY);
		}
		else
		{
			// ProwlArea로 들어왔으므로, 다음 행동 시작
			pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_TakeProwl);
			pkUnit->GoalPos(POINT3::NullData());
			// 원래는 다음Tick에서 다음행동 하도록 하면 프로그램은 깔끔하지만,
			// 클라이언트에 움직임을 멈추도록 해야 하기 때문에 여기에서 바로 다음 행동을 결정하도록 한다.
		}
	}
}

void PgAIActAround::DoAction_ECPattern_TakeProwl(CUnit* pkUnit, EAIActionType eAction, int iActionType,
													 DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	POINT3 ptGoalPos = pkUnit->GoalPos();
	POINT3 const& ptUnitPos = pkUnit->GetPos();
	if( ptGoalPos == POINT3::NullData() )
	{
		// 배회할 곳을 랜덤하게 정함
		ptGoalPos = GetRandomProwlArea(pkUnit, pkGround);
		
		// 목표 지점까지 가는데 Target에 걸치면, 비켜서 가도록 한다.

		// GoalPos로 바로 가야 할지, (X/Y축) 평행이동해야 할지 결정한다.
		//POINT3 ptTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
		POINT3 ptTargetPos;
		int const iMinDistance = GetMinDistanceFromTarget(pkUnit);//__max(pkUnit->GetAbil(AT_MON_MIN_RANGE), AI_MONSTER_MIN_DISTANCE_FROM_TARGET);
		pkGround->GetAroundVector(pkUnit, pkUnit->GetTarget(), (float)iMinDistance, true, ptTargetPos);
		// TargetPos--UnitPos--GoalPos이 예각이면 평행이동하자.
		NxVec3 kVec1(ptUnitPos.x-ptGoalPos.x, ptUnitPos.y-ptGoalPos.y, ptUnitPos.z-ptGoalPos.z);
		NxVec3 kVec2(ptUnitPos.x-ptTargetPos.x, ptUnitPos.y-ptTargetPos.y, ptUnitPos.z-ptTargetPos.z);
		NxVec3 kVec1Unit = kVec1;
		NxVec3 kVec2Unit = kVec2;
		kVec1Unit.normalize();
		kVec2Unit.normalize();
		float const fDotProduct = kVec1Unit.dot(kVec2Unit);
		float const fRad = NiACos(fDotProduct);
		if (fRad < 45 * NI_PI / 180.0f && fRad > -45 * NI_PI / 180.0f ) // 약 45도 이하.
		{
			POINT3BY ptNormalBy = pkGround->GetPathNormal(pkUnit->GetTarget());
			NxVec3 kNormalVec(ptNormalBy.x, ptNormalBy.y, 0);
			kNormalVec.normalize();
			if (BM::Rand_Index(2) == 0)
			{
				// X축 평행 이동
				float fLength = kNormalVec.dot(kVec1);
				fLength = (fLength >= 0) ? __max(20, fLength) : __min(-20, fLength);
				kNormalVec.multiply(fLength, kNormalVec);
				kVec1.z = 0;
				kNormalVec = kNormalVec - kVec1;
				ptGoalPos.x = ptUnitPos.x + kNormalVec.x;
				ptGoalPos.y = ptUnitPos.y + kNormalVec.y;
				ptGoalPos.z = ptUnitPos.z + kNormalVec.z;
			}
			else
			{
				// Y축 평행 이동
				float fLength = kNormalVec.dot(kVec1);
				fLength = (fLength >= 0) ? __max(20, fLength) : __min(-20, fLength);
				kNormalVec.multiply(fLength, kNormalVec);
				ptGoalPos.x = ptUnitPos.x + kNormalVec.x; ptGoalPos.y = ptUnitPos.y + kNormalVec.y; ptGoalPos.z = ptUnitPos.z + kNormalVec.z;
			}
		}
//		INFO_LOG(BM::LOG_LV9, _T("[%s] CurPos[%4.1f, %4.1f, %4.1f], GoalPos[%4.1f, %4.1f, %4.1f]"), __FUNCTIONW__, ptUnitPos.x, ptUnitPos.y,
//			ptUnitPos.z, ptGoalPos.x, ptGoalPos.y, ptGoalPos.z);

		if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptGoalPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
		{
			// Client에 이동명령 보낸다.
			pkUnit->SetState(US_MOVE);
			pkUnit->SetSync(true);
		}
		else
		{	//갈수 있는 곳이 아님
			pkUnit->SetSync(false);	//혹시나 해서
			INFO_LOG(BM::LOG_LV7, __FUNCTIONW__<<" SetValidGoalPos Fail. ptGoalPos X : "<<ptGoalPos.x<<" Y : "<<ptGoalPos.y<<" Z : "<<ptGoalPos.z<<" GUID : "<<pkUnit->GetID().str());
			pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_CANNOTMOVE_FORWORD);
			pkUnit->SetState(US_RETREAT);
		}
		return;
	}
	
	float const fDistQ = GetDistanceQ(ptUnitPos, ptGoalPos);
	if (fDistQ < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		// GoalPos에 도착.
		/*
		int iBoldness = pkUnit->GetAbil(AT_BOLDNESS_RATE);
		iBoldness = (iBoldness == 0) ? MAKE_ABIL_RATE(35) : iBoldness;
		iBoldness += (int)(pkGround->GetAdditionalAttackProb(pkUnit));

		if (BM::Rand_Index(ABILITY_RATE_VALUE) <= iBoldness)
		{
			// 공격 시작 (Chase)
			pkUnit->SetState(US_CHASE_ENEMY);
		}
		*/
		
		pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_AccessProwlArea);
		pkUnit->GoalPos(POINT3::NullData());
		return;
	}
}

void PgAIActAround::DoAction_ECPattern_Ended(CUnit* pkUnit, EAIActionType eAction, int iActionType,
													  DWORD dwElapsedTime, SActArg* pkActArg, PgGround* pkGround)
{
	pkUnit->SetAbil(AT_CHASE_PATTERN, ECPattern_InProwlArea);
	pkUnit->GoalPos(POINT3::NullData());
}

POINT3 PgAIActAround::GetRandomProwlArea(CUnit* pkUnit, PgGround* pkGround)
{
	POINT3 ptTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget(), true);

	bool bFind = false;
	int iCount = 0;
	int iSign = -1;
	POINT3 ptFindPos;
	
	static int const iDefaultDetectRange = 120;
	int const iDetectRange = NiMax(pkUnit->GetDetectRange(), iDefaultDetectRange);

	while (!bFind && ++iCount < 10)
	{
		// 대략적으로 아래와 같이 위치를 잡음 (Target과의 폭은 Abil 뺄 것임)
		// +--------------------------------+
		// |								|
		// |	+----------------------+	|
		// |	|					   |	|
		// |	|					   |	|
		// |	|			*		   |	|
		// |	|		  Target	   |	|
		// |	|					   |	|
		// |	|					   |	|
		// |	+----------------------+	|
		// |		   Prowl Area			|
		// +--------------------------------+

		iSign = (BM::Rand_Index(2) == 0 ? 1 : -1);
		ptFindPos.x = ptTargetPos.x + (iSign * (BM::Rand_Index(iDetectRange) / 2.0f)) + (iSign * iDetectRange / 2.0f);
		iSign = (BM::Rand_Index(2) == 0 ? 1 : -1);
		ptFindPos.y = ptTargetPos.y + (iSign * (BM::Rand_Index(iDetectRange) * 2.0f / 3.0f)) + (iSign * iDetectRange / 3.0f);
		ptFindPos.z = ptTargetPos.z + 5.0f;	//SetValidGoalPos안에서 이미 일정량 Z를 올려주므로 여기선 조금만 올리자

		if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, ptFindPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
		{
			bFind = true;
		}
	}

	return (bFind == true) ? pkUnit->GoalPos() : POINT3::NullData();
}

bool PgAIActAround::IsMoveState(EChasePattern eChasePattern)
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