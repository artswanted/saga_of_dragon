#include "stdafx.h"
#include "Variant/constant.h"
#include "Variant/PgPartyMgr.h"
#include "Global.h"
#include "PgAIActPatrol.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgAIAction.h"

/////////////////////////////////////////////////////////////
// PgAIActPatrol
/////////////////////////////////////////////////////////////
bool PgAIActPatrol::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{

	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_DELAY:
		{
			bResult = true;
		}break;
	case EAI_ACTION_IDLE:
		{
			int iDelayTime = pkUnit->GetDelay();
			if (iDelayTime <= 0)
			{
				bResult = true;
			}
			//else if (GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos()) < 900)
			//{
				// TargetPos에 도착했다면 그만 전진해야 한다.
			//	bResult = true;
			//}
		}break;
	case EAI_ACTION_AROUND:
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_KEEP_DISTANCE:
		{
			PgGround* pkGround = NULL;
			pkActArg->Get(ACTARG_GROUND, pkGround);

			UNIT_PTR_ARRAY kUnitArray;
			bResult = pkGround->FindEnemy( pkUnit, kUnitArray, IsDetailAI(pkUnit) );
			if(bResult)
			{
				pkUnit->SetDelay(0);
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<static_cast<int>(eNextAction)<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return bResult;
}

void PgAIActPatrol::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif

	pkUnit->SetTarget(BM::GUID::NullData());
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
	/*
	// MONAREA :
	pkUnit->SetState(US_MOVE);
	POINT3 pt3GoalPos;
	int iArea = m_kMonAreaMng.GetRandomPos(iCurrentArea, pkMonster->GetPos(), pt3GoalPos);
	pkMonster->GoalPos(pt3GoalPos);
	int iMin = pkUnit->GetAbil(AT_PATROL_MIN_TIME);
	int iMax = pkUnit->GetAbil(AT_PATROL_MAX_TIME);
	if (iMin == 0)
	{
		iMin = AI_MONSTER_MAX_MOVETIME;
	}
	if (iMax < iMin)
	{
		iMax = iMin + 3000;
	}
	pkMonster->SetDelay(iMin + BM::Rand_Index(iMax-iMin+1));
	*/
	if (pkUnit->GetAbil(AT_WAYPOINT_GROUP) < 0)	// CallHelp를 받은 상태이며 GoalPos()로 이동하여야 한다.
	{
		return;
	}
	PgWayPointMng* pkWayPointMng = NULL;
	pkActArg->Get(ACTARG_WAYPOINTMNG, pkWayPointMng);
	
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(pkGround)
	{
		if(!pkWayPointMng->FindWayPos(pkUnit, pkGround))
		{
			pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
			return;
		}
	}
	int iMin = pkUnit->GetAbil(AT_PATROL_MIN_TIME);
	int iMax = pkUnit->GetAbil(AT_PATROL_MAX_TIME);
	if (iMin == 0)
	{
		iMin = AI_MONSTER_MIN_MOVETIME;
	}
	if (iMax < iMin)
	{
		iMax = iMin + 3000;
	}

	if(pkGround)
	{
		POINT3 pt3GoalPos = pkUnit->GoalPos();
		if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, pt3GoalPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos|EFlags_CheckFromGround))
		{
			pkUnit->SetState(US_MOVE);
			pkUnit->SetDelay(iMin + BM::Rand_Index(iMax-iMin+1));
			pkUnit->SetSync(true);
		}
		else
		{
			pkUnit->SetDelay(0);	// Immediately End(Patrol)
		}
	}
}

void PgAIActPatrol::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActPatrol::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
	PgGround* pkGround = NULL;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	// TODO : 몬스터의 WayPoint가 잘못된 곳에 배치되어 빠지는 AI가 존재한다.
	//	이것이 수정될 때 까지 UnitMoved(...,true)로 바닥을 한번더 검사하도록 한다.
	//if (! UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene()))
	if (! UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene(), true))
	{
		//POINT3 const& rkPos = pkUnit->GetPos();
		//POINT3 const& rkTrgPos = pkUnit->GoalPos();
		//int const iClass = pkUnit->GetAbil(AT_CLASS);
		//std::wstring const& rkName = pkUnit->Name();
		//SGroundKey const& rkGndKey = pkGround->GroundKey();
		//INFO_LOG(BM::LOG_LV1, _T("[%s]-[%d] Can't find Gnd[%d, %s] Unit[%s, %d] Pos[%f, %f, %f] -> Target Pos[%f, %f, %f] "),
		//	__FUNCTIONW__, __LINE__,
		//	rkGndKey.GroundNo(), rkGndKey.Guid().str().c_str(),
		//	rkName.c_str(), iClass, rkPos.x, rkPos.y, rkPos.z,
		//	rkTrgPos.x, rkTrgPos.y, rkTrgPos.z);
		// 밑에 빠질것 같아...조심해
		pkUnit->SetDelay(0);
	}

	if (GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos()) < AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		PgWayPointMng* pkWayPointMng = NULL;
		pkActArg->Get(ACTARG_WAYPOINTMNG, pkWayPointMng);
		PgGround* pkGround = NULL;
		pkActArg->Get(ACTARG_GROUND, pkGround);
		bool bFindWayTemp = false;
		if(pkGround)
		{
			bFindWayTemp = pkWayPointMng->FindWayPos(pkUnit, pkGround);
		}
		bool const bFindWay = bFindWayTemp;
		pkUnit->SetSync(bFindWay);
		PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkUnit);
		if(pkMonster)
		{
			pkMonster->ClearIgnoreTarget();
		}

		if(!bFindWay)
		{
			pkUnit->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
		}
	}
}

