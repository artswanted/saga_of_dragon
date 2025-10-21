#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/constant.h"
#include "Variant/PgPartyMgr.h"
#include "PgAIAction.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

/*
PgAIAction::PgAIAction(void)
{
}

PgAIAction::~PgAIAction(void)
{
}

void PgAIAction::SetMonAreaMng(PgMonAreaMng* pkAreaMng)
{
	m_pkMonAreaMng = pkAreaMng;
}
*/

/*
bool IsTargetAvalible(CUnit* pkUnit, PgGround* pkGround)
{
	// Testing
	return false;
	//
	BM::GUID const & kTargetGuid = pkUnit->GetTarget();
	if (kTargetGuid == BM::GUID::NullData())
	{
		return false;
	}
	CUnit* pkTarget = pkGround->GetUnit(kTargetGuid);
	if (pkTarget == NULL)
	{
		pkUnit->SetTarget(BM::GUID::NullData());
		return false;
	}
	if (ERange_OK!=::IsInRange(pkGround->GetUnitPos(pkUnit), pkGround->GetUnitPos(pkTarget), pkUnit->GetAbil(AT_DETECT_RANGE), 30))
	{
		pkUnit->SetTarget(BM::GUID::NullData());
		return false;
	}
	return true;
}
*/

bool UnitMoved(CUnit* pkUnit, POINT3 const& rkTargetPos, DWORD dwMoveTime, NxScene *pkScene, bool bCheckGround)
{
	int const iMoveSpeed = pkUnit->GetAbil(AT_C_MOVESPEED);
	if( iMoveSpeed < 0 )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"MoveSpeed is < 0 Guid["<<pkUnit->GetID()<<L"], Class["<<pkUnit->GetAbil(AT_CLASS)<<L"], MoveSpeed["<<iMoveSpeed<<L"]");
	}
	if (/*iMoveSpeed <= 0 || */(rkTargetPos.x == 0.0f && rkTargetPos.y == 0.0f && rkTargetPos.z == 0.0f))
	{
		return true;
	}
	
	POINT3 const& ptCurrentPos = pkUnit->GetPos();
	NxVec3 kCurPos(ptCurrentPos.x, ptCurrentPos.y, ptCurrentPos.z);
	NxVec3 kTargetPos(rkTargetPos.x, rkTargetPos.y, rkTargetPos.z);
	NxVec3 kDirVec = kTargetPos - kCurPos;
	kDirVec.z = 0;

	if(kDirVec.magnitudeSquared() < 10)
	{
		pkUnit->SetPos(rkTargetPos);
		return true;
	}

	kDirVec.normalize();
	NxVec3 kResultVec = kDirVec * NxReal(iMoveSpeed * dwMoveTime / 1000.0f) + kCurPos;
	if((kTargetPos - kCurPos).magnitudeSquared() < (kResultVec - kCurPos).magnitudeSquared())
	{
		// 타겟이 GoalPos를 지나치면, GoalPos로 되돌려줌.
		kResultVec = kTargetPos;
	}

	NxRay kRay(NxVec3(kResultVec.x, kResultVec.y, ptCurrentPos.z+30), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 90.0f, NX_RAYCAST_SHAPE | NX_RAYCAST_IMPACT);
	if(!pkHitShape)
	{
		pkUnit->GoalPos(POINT3(0,0,0));
		pkUnit->SetAbil(AT_CHASE_DONE, true);
		return false;
	}

//	INFO_LOG(BM::LOG_LV9, _T("[%s] BeforePos[%4.1f, %4.1f, %4.1f], TargetPos[%4.1f, %4.1f, %4.1f], AfterPos[%4.1f, %4.1f, %4.1f]"),
//		__FUNCTIONW__, ptCurrentPos.x, ptCurrentPos.y, ptCurrentPos.z, rkTargetPos.x, rkTargetPos.y, rkTargetPos.z,
//		ptNewPos.x, ptNewPos.y, ptNewPos.z);
	pkUnit->SetPos(POINT3(kResultVec.x, kResultVec.y, kHit.worldImpact.z));

	return true;
}

// [RETURN]
//	true : 계속 날아 갈 수 있다.
//	false : 목표지점에 도착했다.
bool MovingSomething(POINT3& ptCurrent, POINT3 const& ptTarget, int const iMoveSpeed, DWORD const dwElapsedTime)
{
	NxVec3 kCurrent(ptCurrent.x, ptCurrent.y, ptCurrent.z);
	NxVec3 kTarget(ptTarget.x, ptTarget.y, ptTarget.z);

	NxVec3 kDirVec = kTarget - kCurrent;
	kDirVec.z = 0;
	if (kDirVec.magnitudeSquared() < 100)
	{
		ptCurrent.x = ptTarget.x; ptCurrent.y = ptTarget.y; ptCurrent.z = ptTarget.z;
		return false;	// 도착했다.
	}

	kDirVec.normalize();
	NxVec3 kResultVec = kDirVec * NxReal(iMoveSpeed * dwElapsedTime / 1000.0f) + kCurrent;

	if((kTarget - kCurrent).magnitudeSquared() < (kResultVec - kCurrent).magnitudeSquared())
	{
		// 타겟이 GoalPos를 지나치면, GoalPos로 되돌려줌.
		ptCurrent.x = ptTarget.x; ptCurrent.y = ptTarget.y; ptCurrent.z = ptTarget.z;
		return false;	// 도착했다.
	}
	
	ptCurrent.x = kResultVec.x; ptCurrent.y = kResultVec.y; ptCurrent.z = kResultVec.z;
	return true;// 계속 날아가야 한다.
}

int GetMinDistanceFromTarget(CUnit* pkUnit)
{
	int iMinDistance = pkUnit->IsUnitType(UT_SUMMONED) ? AI_MONSTER_MIN_DISTANCE_CHASE_Q : AI_MONSTER_MIN_DISTANCE_FROM_TARGET;
	if (pkUnit)
	{
		CSkill* pkSkill = pkUnit->GetSkill();
		if (pkSkill)
		{
			int iAbil = pkSkill->GetAbil(AT_MON_MIN_RANGE);
			if (0<iAbil)
			{
				iMinDistance = iAbil;
			}
			else
			{
				iAbil = pkSkill->GetAbil(AT_SKILL_MIN_RANGE);
				if (0<iAbil)
				{
					iMinDistance = iAbil;
				}
				else
				{
					iAbil = pkUnit->GetAbil(AT_MON_MIN_RANGE);
					if (0<iAbil)
					{
						iMinDistance = iAbil;
					}
				}
			}
		}
	}
	return iMinDistance;
}

bool IsDetailAI(CUnit *pkUnit)
{
	static int iValue = -1;	//매번 찾지 않을려고
	bool bReturn = false;
	if(0>iValue && S_OK != g_kVariableContainer.Get(EVar_Kind_AI, UseDetailAI, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find 'UseDetailAI'");
		bReturn = false;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bReturn is false!"));
	}
	
	bReturn = !iValue;

	if(iValue && pkUnit)	//정예, 잡몹을 구분할려면
	{
		bReturn = pkUnit->GetAbil(AT_GRADE)>EMGRADE_NORMAL && pkUnit->GetAbil(AT_GRADE)<=EMGRADE_MONSTERMAX;
	}

	return bReturn;
}