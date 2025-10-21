#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Monster.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "Global.h"
#include "PgGround.h"
#include "PgAction.h"

inline int DefaultValueIsZero(int const iValue, int const iDefault)
{
	if(0==iValue)
	{
		return iDefault;
	}
	return iValue;
}

ECREATE_HP_TYPE GetCreateHPType(CSkillDef const* pkSkillDef)
{
	if(!pkSkillDef)
	{
		return ECHT_NONE;
	}

	ECREATE_HP_TYPE eType = static_cast<ECREATE_HP_TYPE>(pkSkillDef->GetAbil(AT_CREATE_HP_TYPE));
	if(ECHT_NONE == eType)
	{
		EPlayContentsType const ePlayType = static_cast<EPlayContentsType>(pkSkillDef->GetAbil(AT_PLAY_CONTENTS_TYPE));
		switch(ePlayType)
		{
		case EPCT_ELGA_EYE_MON:
			{
				eType = ECHT_ELGA_EYE_MON;
			}break;
		}
	}
	return eType;
}

char const szMapDummyName[] = "MapDummy";

void GetDefaultEffectNo(VEC_INT & rkContEffect, CSkillDef const* pkSkill, CUnit const* pkUnit, PgGround const* pkGround)
{
	if(!pkSkill)
	{
		return;
	}

	if(pkSkill->GetEffectNo())
	{
		rkContEffect.push_back( pkSkill->GetEffectNo() );
	}

	for(int i=0; i<EFFECTNUM_MAX; ++i)
	{
		if(int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1+i))
		{
			rkContEffect.push_back(iAddEffect);
		}
		else
		{
			break;
		}
	}

	if(pkUnit && pkGround)
	{
		if(pkUnit->IsUnitType(UT_SUMMONED))
		{
			GET_DEF(CSkillDefMgr, kSkillDefMgr);

			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkGround->GetUnit(pkUnit->Caller()));
			PgMySkill* pkPlayerSkill = pkPlayer ? pkPlayer->GetMySkill() : NULL;
			if(pkPlayerSkill)
			{
				for(int i=0; i<CHILD_SKILL_MAX; ++i)
				{
					//영향 받는 베이스 스킬
					int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01+i);
					if(1 > iChildBaseSkillNo) { break; }

					//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
					int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
					if(1 > iLearnedChildSkillNo) { break; }

					CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
					if(pkLearnedSkillDef)
					{
						for(int i=0; i<EFFECTNUM_MAX; ++i)
						{
							if(int const iAddEffect = pkLearnedSkillDef->GetAbil(AT_EFFECTNUM1+i))
							{
								rkContEffect.push_back(iAddEffect);
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	//중복 제거
	if(!rkContEffect.empty())
	{
		VEC_INT::iterator new_end = std::unique(rkContEffect.begin(),rkContEffect.end());
		rkContEffect.erase(new_end,rkContEffect.end());
	}
}

int DefaultMonsterSkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult, bool bEffect)
{	//기본적으로 데미지주고 100% 확률로 Effect주는 함수
	if(!pkUnit)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iIndex = 0;
	
	if (!pkUnitArray || pkUnitArray->empty())
	{
		return iIndex;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(!pkTarget)
		{
			++unit_itor;
			continue;
		}

		bool const bTargetSelf = (ESTARGET_SELF&pkSkill->GetTargetType()) && (pkTarget->GetID()==pkUnit->GetID());
		if( !bTargetSelf && !pkUnit->IsTarget(pkTarget, true, pkSkill->GetTargetType()) )
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
		}
	}

	CUnit * pkCaller = NULL;
	if(pkUnit->IsUnitType(UT_SUMMONED))
	{
		pkCaller = pkGround->GetUnit(pkUnit->Caller());
	}
	bool bRet = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult, pkCaller);

	//이펙트 얻기
	VEC_INT kContEffect;
	GetDefaultEffectNo(kContEffect, pkSkill, pkUnit, pkGround);

	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//정리된 목록으로 데미지주기
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID());
		if (pkActionResult && !pkActionResult->GetInvalid())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkActionResult, iSkillNo, pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
			int const iDamage = pkActionResult->GetValue(); // DoFinalDamage에 의해 데미지가 변경 되었을 수 있음
			if (bEffect && iDamage)
			{
				for(VEC_INT::const_iterator c_it=kContEffect.begin(); c_it!=kContEffect.end(); ++c_it)
				{
					pkTarget->AddEffect((*c_it), 0, pArg, pkUnit);
				}
				pkTarget->SetTarget(pkUnit->GetID()); 
			}
			++iIndex;
		}
		++unit_itor;
	}

	return iIndex;
}

///////////////////////////////////////////////////////////
//  PgTargetLinkSkillFunction
///////////////////////////////////////////////////////////
int PgTargetLinkSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgTargetLinkSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	if ( pkUnitArray->empty() )
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 const kPos = pkUnit->GetPos();
	int const iRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM2);
	UNIT_PTR_ARRAY kNew;
	pkGround->GetUnitInRange(kPos, iRange, UT_MONSTER, kNew, static_cast<int>(AI_Z_LIMIT));

	if ( kNew.empty() )
	{
		return -1;	//주변에 잡몹이 없음
	}

	int const iEffectNo = pkSkill->GetEffectNo();

	UNIT_PTR_ARRAY::iterator slave_itor = kNew.begin();
	UNIT_PTR_ARRAY::const_iterator target_itor = pkUnitArray->begin();
	BM::GUID const &rkGuid = (*target_itor).pkUnit->GetID();
	int iIndex = 0;
	while(kNew.end()!=slave_itor)
	{
		CUnit* pkSlave = (*slave_itor).pkUnit;
		if (0<iEffectNo && pkSlave && pkSlave->GetID() != pkUnit->GetID())	//쓴 놈은 안결려야지
		{
			pkSlave->AddEffect(iEffectNo, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
			pkSlave->SetTarget(rkGuid); 
			++iIndex;
		}
		++slave_itor;
	}
	
	return iIndex;
}

///////////////////////////////////////////////////////////
//  PgTargetLin2kSkillFunction
///////////////////////////////////////////////////////////
int PgTargetLink2SkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgTargetLink2SkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	if ( pkUnitArray->empty() )
	{
		return 0;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 const kPos = pkUnit->GetPos();
	int const iRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM2);
	UNIT_PTR_ARRAY kNew;
	pkGround->GetUnitInRange(kPos, iRange, UT_MONSTER, kNew, static_cast<int>(AI_Z_LIMIT));

	if ( kNew.empty() )
	{
		return -1;	//주변에 잡몹이 없음
	}

	int const iEffectNo = pkSkill->GetEffectNo();

	UNIT_PTR_ARRAY::iterator slave_itor = kNew.begin();
	UNIT_PTR_ARRAY::const_iterator target_itor = pkUnitArray->begin();
	BM::GUID const &rkGuid = (*target_itor).pkUnit->GetID();
	int iIndex = 0;
	while(kNew.end()!=slave_itor)
	{
		CUnit* pkSlave = (*slave_itor).pkUnit;
		if (pkSlave)
		{
			BM::GUID const &rkSlaveTarget = pkSlave->GetTarget();
			if (0<iEffectNo && BM::GUID::IsNotNull(rkSlaveTarget) && pkSlave->GetID() != pkUnit->GetID())	//쓴 놈은 안결려야지
			{
				pkSlave->AddEffect(iEffectNo, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
				pkSlave->SetTarget(rkGuid); 
				++iIndex;
			}
		}
		++slave_itor;
	}
	
	return iIndex;
}

///////////////////////////////////////////////////////////
//  PgDamageAndEffectSkillFunction
///////////////////////////////////////////////////////////
int PgDamageAndEffectSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgDamageAndEffectSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

///////////////////////////////////////////////////////////
//  PgDashAfterAttackSkillFunction
///////////////////////////////////////////////////////////
bool PgDashAfterAttackSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	if( POINT3::NullData() == kTargetPos )
	{
		return false;
	}
	POINT3 const& rkUnitPos = pkUnit->GetPos();
	float fDistance = GetDistance(rkUnitPos, kTargetPos);
	//돌격 거리 계산하기
	int iMinDist = 30;
	float fRange = 30.0f;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	
	if ( pkSkill )
	{
		//돌진이 가능한 거리인지 검사
		int iMinDistDef = pkSkill->GetAbil(AT_SKILL_MIN_RANGE);
		if (0 < iMinDistDef)
		{
			iMinDist = iMinDistDef;
		}

		int const iAbil = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
		if ( 0 < iAbil )
		{
			fRange = float(iAbil - 1);
		}
	} 

	if (iMinDist > fDistance)
	{
		return false;
	}

	if ( fDistance >= fRange )
	{
		fDistance = fDistance - fRange - 1;
	}

	NxVec3 kVision(0,0,0);
	pkGround->GetVisionDirection(pkUnit, kVision);
	if(kVision.isZero())
	{
		return false;
	}
	kVision *= (NxReal)fDistance;
	kTargetPos.x = rkUnitPos.x+kVision.x; 
	kTargetPos.y = rkUnitPos.y+kVision.y; 
	kTargetPos.z = rkUnitPos.z+kVision.z;

	if(fDistance <= 5.0f || !SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 10.0f, (float)iMinDist, EFlags_MinDistance|EFlags_CheckFromGround))
	{
		return false;
	}

	return true;
}

int PgDashAfterAttackSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	if( POINT3::NullData() == kTargetPos )
	{
		return -1;
	}
	POINT3 const& rkUnitPos = pkUnit->GetPos();
	float fDistance = GetDistance(rkUnitPos, kTargetPos);
	POINT3 kDir(kTargetPos - rkUnitPos);
	kDir.Normalize();

	//돌격 거리 계산하기
	float fRange = 30.0f;
	CSkill* pkSkill = pkUnit->GetSkill();
	if ( pkSkill )
	{
		int const iAbil = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
		if ( 0 < iAbil )
		{
			fRange = float(iAbil - 1);
		}
	}

	if ( fDistance >= fRange )
	{
		fDistance = fDistance - fRange - 1;
	}

	NxVec3 kVision(0,0,0);
	pkGround->GetVisionDirection(pkUnit, kVision);
	kVision *= (NxReal)fDistance;
	kTargetPos.x = rkUnitPos.x+kVision.x; 
	kTargetPos.y = rkUnitPos.y+kVision.y; 
	kTargetPos.z = rkUnitPos.z+kVision.z;

	//돌진이 가능한 거리인지 검사
	int iMinDist = 30;
	CSkillDef const* pkSkillDef = pkSkill->GetSkillDef();
	
	if (pkSkillDef)
	{
		int iMinDistDef = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
		if (0 < iMinDistDef)
		{
			iMinDist = iMinDistDef;
		}
	}
	if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 10.0f, (float)iMinDist, EFlags_MinDistance|EFlags_SetGoalPos|EFlags_CheckFromGround))
	{
		//유효한 위치가 아니므로 취소
		kTargetPos = rkUnitPos;
		kTargetPos.z+=20.0f;	//약간 올리자
		pkPacket->Push(kTargetPos);

		return -1;
	}

	kTargetPos.z+=20.0f;	//약간 올리자
	pkPacket->Push(kTargetPos);

	return 1;
}

int PgDashAfterAttackSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgDashAttackSkillFunction
///////////////////////////////////////////////////////////
bool PgDashAttackSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	if( POINT3::NullData() == kTargetPos )
	{
		return false;
	}
	POINT3 const& rkUnitPos = pkUnit->GetPos();
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( !pkSkillDef )
	{
		return false;
	}

	float fDistance = 220;	//돌격 거리 계산
	if (pkSkillDef)
	{
		int iDist = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		if (0 < iDist)
		{
			fDistance = (float)iDist;
		}	
	}

	NxVec3 kVision(0,0,0);
	pkGround->GetVisionDirection(pkUnit, kVision, true);
	if(kVision.isZero())
	{
		return false;
	}
	kVision *= (NxReal)fDistance;
	kTargetPos.x = rkUnitPos.x+kVision.x; 
	kTargetPos.y = rkUnitPos.y+kVision.y; 
	kTargetPos.z = rkUnitPos.z+kVision.z;

	int iMinDist = 30;	//최소거리
	int iMinDistDef = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
	if (0 < iMinDistDef)
	{
		iMinDist = iMinDistDef;
	}

	//돌진이 가능한 거리인지 검사
	POINT3 ptSaveGoalPos = pkUnit->GoalPos();
	if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, (float)iMinDist, EFlags_MinDistance|EFlags_CheckFromGround|EFlags_SetGoalPos))
	{
		pkUnit->GoalPos(ptSaveGoalPos);
		return false;
	}

	kTargetPos = pkUnit->GoalPos();	//SetValidGoalPos함수 내부에서 정확하게 다시 GoalPos가 계산되므로
	pkUnit->GoalPos(ptSaveGoalPos);

	//커스텀 어빌에다 임시 저장
	pkUnit->SetAbil(AT_CUSTOMDATA7, static_cast<int>(kTargetPos.x));
	pkUnit->SetAbil(AT_CUSTOMDATA8, static_cast<int>(kTargetPos.y));
	pkUnit->SetAbil(AT_CUSTOMDATA9, static_cast<int>(kTargetPos.z));

	return true;
}
int PgDashAttackSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	int iRet = 1;
	POINT3 kTargetPos(static_cast<float>(pkUnit->GetAbil(AT_CUSTOMDATA7)), static_cast<float>(pkUnit->GetAbil(AT_CUSTOMDATA8)), static_cast<float>(pkUnit->GetAbil(AT_CUSTOMDATA9)));
	
	//돌진이 가능한 거리인지 검사
	if (POINT3::NullData() == kTargetPos || !SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0.0f, EFlags_SetGoalPos|EFlags_CheckFromGround))
	{
		//유효한 위치가 아니므로 최동돌진위치를 타겟위치 잡는다
		pkUnit->GoalPos(pkUnit->GetPos());
		iRet = -1;
	}

	pkPacket->Push(pkUnit->GoalPos());

	//썻으니까 초기화 해주자
	pkUnit->SetAbil(AT_CUSTOMDATA7, 0);
	pkUnit->SetAbil(AT_CUSTOMDATA8, 0);
	pkUnit->SetAbil(AT_CUSTOMDATA9, 0);

	return iRet;
}

int PgDashAttackSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgEnergyExplosionSkillFunction
///////////////////////////////////////////////////////////
bool PgEnergyExplosionSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkillDef)
	{
		return false;
	}

	int const iMonNo = pkSkillDef->GetAbil(AT_CANT_SKILL_ALIVE_UNIT_NO);
	if( iMonNo )
	{
		CUnit* pkTarget = pkGround->GetUnitByClassNo(iMonNo, UT_MONSTER);
		if( pkTarget )
		{
			return false;
		}
	}

	BM::GUID const& rkTargetGuid = pkUnit->GetTarget();
	POINT3 kTargetPos = pkGround->GetUnitPos(rkTargetGuid);

	float const fMinDist = static_cast<float>(pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE));

	//갈 수 없는 곳이면 시전하지 말자
	if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, fMinDist, EFlags_MinDistance|EFlags_CheckFromGround))
	{
		return false;
	}

	NxCapsule kCapsule;//캡슐로도 충돌처리를 해보자
	kCapsule.p0.set(pkUnit->GetPos().x, pkUnit->GetPos().y, pkUnit->GetPos().z);
	kCapsule.p1 = kCapsule.p0;	//어짜피 위에서 아래로 쏠거라 같아도 됨
	kCapsule.radius = PG_CHARACTER_CAPSULE_RADIUS * 1.5f;//약간의 거리를 더 둬야 안전하다
	NxSweepQueryHit kResult;
	pkGround->GetPhysXScene()->GetPhysXScene()->linearCapsuleSweep(kCapsule, NxVec3(0.0f, 0.0f, -1.0f), NX_SF_STATICS, NULL, 1, &kResult, NULL);
	if(kResult.hitShape)
	{
		if( 50.0f < fabs(kResult.point.z - pkUnit->GetPos().z) )//높이차가 일정이상 나게되면 공중에 걸렸다는 얘기가 됨
		{
			return false;
		}
	}

	return fabs(kTargetPos.z - pkUnit->GetPos().z)<150;	//150높이
}

int PgEnergyExplosionSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 kTargetPos;
	BM::Stream kForceParam;
	if(S_OK == pArg->Get(ACTARG_FORCE_SKILL_PARAM, kForceParam))
	{
		size_t iSize = 0;
		kForceParam.Pop(iSize);
		kForceParam.Pop(kTargetPos);
	}
	else
	{
		kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget(), true);
		if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos) || POINT3(0,0,0) == kTargetPos)
		{
			kTargetPos = pkUnit->GetPos();
		}
	}

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}
	
	//kTargetPos.z+=20;
	pkPacket->Push(kTargetPos);
	pkUnit->GoalPos(kTargetPos);

	return 1;
}

int PgEnergyExplosionSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

///////////////////////////////////////////////////////////
//  PgEarthQuakeSkillFunction
///////////////////////////////////////////////////////////
int PgEarthQuakeSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	CUnit* pkTarget = pkGround->GetUnit(pkUnit->GetTarget());
	if (!pkTarget)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);

	float fDistance = 260.0f;	//거리 계산
	if (pkSkillDef)
	{
		int iDist = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		if (0 < iDist)
		{
			fDistance = (float)iDist;
		}	
	}

	POINT3 kTargetPos;
	NxVec3 kVision;
	pkGround->GetVisionDirection(pkUnit, kVision);
	kVision *= (NxReal)fDistance;
	POINT3 const& rkUnitPos = pkUnit->GetPos();
	kTargetPos.x = rkUnitPos.x+kVision.x; 
	kTargetPos.y = rkUnitPos.y+kVision.y; 
	kTargetPos.z = rkUnitPos.z+kVision.z+20; 

	pkUnit->GoalPos(kTargetPos);

	return 1;
}

int PgEarthQuakeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgMonsterHealSkillFunction
///////////////////////////////////////////////////////////
int PgMonsterHealSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

bool PgMonsterHealSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	int const iHP = pkUnit->GetAbil(AT_HP);
	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

	return iMaxHP >= iHP*2;
}

int	 PgMonsterHealSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	int const iAdd = pkSkillDef->GetAbil(AT_HP);
	int iIndex = 0;
	if (0 < SkillFuncUtil::OnModifyHP(pkUnit, NULL, 0, iAdd, pkUnit, NULL) )
	{ 		
		++iIndex;
	}

	return iIndex;
}

///////////////////////////////////////////////////////////
//  PgMonsterMassiveHealSkillFunction
///////////////////////////////////////////////////////////
int PgMonsterMassiveHealSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int	 PgMonsterMassiveHealSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{ 
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	int const iAdd = pkSkillDef->GetAbil(AT_HP);
	if(0 == iAdd)
	{
		return 0;
	}

	int iIndex = 0;

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();

	while(pkUnitArray->end() != unit_itor)
	{
		CUnit *pkTarget = (*unit_itor).pkUnit;
		if (pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if (pkAResult)
			{
				SkillFuncUtil::OnModifyHP(pkUnit, NULL, 0, iAdd, pkUnit, NULL);
				++iIndex;
			}
		}
		++unit_itor;
	}

	return iIndex;
}

///////////////////////////////////////////////////////////
//  PgSummonThornSkillFunction
///////////////////////////////////////////////////////////
int PgSummonThornSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgSummonThornSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	UNIT_PTR_ARRAY kNewArray;
	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	int const iFound = pkGround->GetEntity(pkUnit->GetID(), iClass, kNewArray);
	if ( iFound > 0 )	//이미 엔티티가 있다
	{
		return 0;
	}

	UNIT_PTR_ARRAY::const_iterator unit_itor = pkUnitArray->begin();
	POINT3 kTargetPos = pkUnit->GetPos();

	if (pkUnitArray->end() != unit_itor)
	{
		kTargetPos = (*unit_itor).pkUnit->GetPos();
	}

	//엔티티 만들기
	SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(1,pkSkillDef->GetAbil(AT_LEVEL))) );
	kCreateInfo.bUniqueClass = false;

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}
	kCreateInfo.ptPos = kTargetPos;

	CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("SummonThorn"));
	if (pkEntity)
	{
		int const iPercent = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
		int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
		pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  6000932 PgSummonThornsSkillFunction
///////////////////////////////////////////////////////////
bool PgSummonThornsSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return false;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	UNIT_PTR_ARRAY kNewArray;
	return 0 >= pkGround->GetEntity(pkUnit->GetID(), pkSkillDef->GetAbil(AT_CLASS), kNewArray);	//이미 소한한 엔티티가 없을때만
}
int PgSummonThornsSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	CUnit::DequeTarget const& rkDeque = pkUnit->GetTargetList();
	CUnit::DequeTarget::const_iterator target_it = rkDeque.begin();
	int iCount = 0;
	std::list<BM::GUID> kGuidList;
	while(target_it!=rkDeque.end())
	{
		CUnit const* pkTarget = pkGround->GetUnit((*target_it).kGuid);
		if (pkTarget)
		{ 
			kGuidList.push_back(pkTarget->GetID());
			++iCount;
		}
		++target_it;
	}

	pkPacket->Push(static_cast<ESendSkillPacketType>(SSPT_SKILL_PACKET_GUID));
	pkPacket->Push(iCount);	//몇개

	std::list<BM::GUID>::const_iterator guid_it = kGuidList.begin();	
	while(guid_it!=kGuidList.end())
	{
		pkPacket->Push((*guid_it));
		++guid_it;
	}

	return iCount;
}

int PgSummonThornsSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return -1;
	}

	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	int const iLevel = pkSkillDef->GetAbil(AT_LEVEL);
	int const iMagic = pkUnit->GetAbil(AT_MAGIC_ATTACK_MAX) * pkSkillDef->GetAbil(AT_MAGIC_DMG_PER) / ABILITY_RATE_VALUE;

	UNIT_PTR_ARRAY::const_iterator unit_itor = pkUnitArray->begin();
	int iCount = 0;
	while(unit_itor!=pkUnitArray->end())
	{
		const CUnit* pkTargetUnit = (*unit_itor).pkUnit;
		if(pkTargetUnit)
		{
			POINT3 kTargetPos = pkTargetUnit->GetPos();
			NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
			NxRaycastHit kHit;
			NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
			if(pkHitShape)
			{
				kTargetPos.z = kHit.worldImpact.z;
				//엔티티 만들기
				SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(pkSkillDef->GetAbil(AT_LEVEL),1)) );
				kCreateInfo.bUniqueClass = false;
				kCreateInfo.ptPos = kTargetPos;
				CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("SummonThorns"));
				if (pkEntity)
				{
					pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
					++iCount;
				}
			}
		}
		++unit_itor;
	}
	
	return iCount;
}

///////////////////////////////////////////////////////////
//  PgSummonEntitySkillFunction
///////////////////////////////////////////////////////////
int PgSummonEntitySkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgSummonEntitySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	UNIT_PTR_ARRAY kNewArray;
	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	int const iFound = pkGround->GetEntity(pkUnit->GetID(), iClass, kNewArray);
	if ( iFound > 0 )	//이미 엔티티가 있다
	{
		return 0;
	}

	POINT3 kTargetPos = pkUnit->GetPos();
	int const iDistance = PgAdjustSkillFunc::GetAttackRange(pkUnit,pkSkillDef);

	NxVec3 kVision;
	pkGround->GetVisionDirection(pkUnit, kVision);
	kVision *= (NxReal)iDistance;
	kTargetPos.x += kVision.x; 
	kTargetPos.y += kVision.y; 
	kTargetPos.z += kVision.z;

	//엔티티 만들기
	SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(1,pkSkillDef->GetAbil(AT_LEVEL))) );
	kCreateInfo.bUniqueClass = false;

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}
	kCreateInfo.ptPos = kTargetPos;

	CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("SummonThorn"));
	if (pkEntity)
	{
		int const iPercent = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
		int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
		pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  PgSummonShotEntitySkillFunction
///////////////////////////////////////////////////////////
int PgSummonShotEntitySkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	POINT3 kTargetPos;
	if(int const iDistance = pkSkillDef->GetAbil(AT_DISTANCE))
	{
		//Distance 위치로
		kTargetPos = pkUnit->GetPos();

		NxVec3 kVision;
		pkGround->GetVisionDirection(pkUnit, kVision);

		//소환 위치 : PC의 전방
		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z), kVision);
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit, iDistance);
		if(pkHitShape)
		{
			kTargetPos.x = kHit.worldImpact.x;
			kTargetPos.y = kHit.worldImpact.y;
		}
		else
		{
			kVision *= (NxReal)iDistance;
			kTargetPos.x = kTargetPos.x+kVision.x;
			kTargetPos.y = kTargetPos.y+kVision.y;
			kTargetPos.z = kTargetPos.z+kVision.z;
		}
	}
	else
	{
		//Target 위치로
		CUnit const* pkTarget = pkGround->GetUnit(pkUnit->GetTarget());
		if(pkTarget)
		{
			kTargetPos = pkTarget->GetPos();
		}
	}

	if(POINT3::NullData() != kTargetPos)
	{
		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(pkHitShape)
		{
			kTargetPos.z = kHit.worldImpact.z;
		}

		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, kTargetPos.x);
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+1, kTargetPos.y);
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+2, kTargetPos.z);

		pkPacket->Push(static_cast<ESendSkillPacketType>(SSPT_SKILL_PACKET_POSITION));
		pkPacket->Push(static_cast<int>(1));	//몇개
		pkPacket->Push(kTargetPos);
	}

	return 1;
}

int PgSummonShotEntitySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 kTargetPos;
	kTargetPos.x = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	kTargetPos.y = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+1);
	kTargetPos.z = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+2);
	if(POINT3::NullData()==kTargetPos)
	{
		return -1;
	}

	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	EUnitType const eUnitType = static_cast<EUnitType>(pkSkillDef->GetAbil(AT_MON_SKILL_UNIT_TYPE));

	if(UT_ENTITY==eUnitType)
	{
		//엔티티 만들기
		SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(1,pkSkillDef->GetAbil(AT_LEVEL))) );
		kCreateInfo.ptPos = kTargetPos;

		CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("ShotEntity"));
		if (pkEntity)
		{
			int const iPercent = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
			int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
			pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
		}
	}
	else if( iClass )
	{
		BM::GUID kMonsterGuid;
		TBL_DEF_MAP_REGEN_POINT kRegenInfo;
		kRegenInfo.iMapNo = pkGround->GetGroundNo();
		kRegenInfo.pt3Pos = kTargetPos;
		pkGround->InsertMonster( kRegenInfo, iClass, kMonsterGuid, pkUnit );
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  PgSummonMonsterSkillFunction
///////////////////////////////////////////////////////////
POINT3 GetRamdomPos(PgGround * pkGround, POINT3 const& rkPos, int const iDistance)
{
	if(!pkGround)
	{
		return rkPos;
	}

	POINT3 kCreatePos = rkPos;
	kCreatePos.x += BM::Rand_Range(iDistance/2) * (0==BM::Rand_Index(2) ? 1 : -1);
	kCreatePos.y += BM::Rand_Range(iDistance/2) * (0==BM::Rand_Index(2) ? 1 : -1);

	{
		POINT3 kDirPos = kCreatePos-rkPos;
		NxVec3 kDirVec(kDirPos.x,kDirPos.y,0);
		kDirVec.normalize();
		kDirVec.z = kCreatePos.z;

		NxRay kRay(NxVec3(rkPos.x, rkPos.y, rkPos.z), kDirVec);
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit, (iDistance/2)+10);
		if(pkHitShape)
		{
			kCreatePos.x = kHit.worldImpact.x - kDirVec.x;
			kCreatePos.y = kHit.worldImpact.y - kDirVec.y;
		}
	}

	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(kCreatePos.x, kCreatePos.y, kCreatePos.z+10), NxVec3(0,0,-1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kCreatePos.z = kHit.worldImpact.z;
	}
	return kCreatePos;
}

int PgSummonMonsterSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgSummonMonsterSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	VEC_INT kContMonBagControl;
	if(int const iMonBagControl = pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL))
	{
		kContMonBagControl.push_back(iMonBagControl);
	}
	for(int i=AT_SKILL_SUMMON_MONBAGCONTROL_01; i<AT_SKILL_SUMMON_MONBAGCONTROL_10; ++i)
	{
		if(int const iMonBagControl = pkSkillDef->GetAbil(i))
		{
			kContMonBagControl.push_back(iMonBagControl);
		}
	}

	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );

	int const iDistance = pkSkillDef->GetAbil(AT_DISTANCE);
	int iMax = pkSkillDef->GetAbil(AT_COUNT);
	if ( 0>=iMax )
	{
		iMax = 5;//기본값 5
	}

	typedef std::map<int, int> ContMon;
	ContMon kContMon;

	if ( pContMonBagControl && pContMonBag && pContMonBagElement )
	{
		for(VEC_INT::const_iterator c_it=kContMonBagControl.begin(); c_it!=kContMonBagControl.end(); ++c_it)
		{
			int const iMonBagControl = (*c_it);
			CONT_DEF_MONSTER_BAG_CONTROL::const_iterator control_itr = pContMonBagControl->find(iMonBagControl);
			if ( control_itr != pContMonBagControl->end() )
			{
				int const iMonBagNo = control_itr->second.aBagElement[pkGround->GetMapLevel()];
				CONT_DEF_MONSTER_BAG::const_iterator bag_itr = pContMonBag->find( iMonBagNo );
				if ( bag_itr != pContMonBag->end() )
				{
					CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itr = pContMonBagElement->find(bag_itr->second.iElementNo);
					if( element_itr != pContMonBagElement->end() )
					{
						BM::GUID kMonGuid;

						TBL_DEF_MAP_REGEN_POINT kGenPoint;
						kGenPoint.cBagControlType = ERegenBag_Type_Monster;
						kGenPoint.dwPeriod = 0;
						kGenPoint.iBagControlNo = 0;/*없어도 되지 않을까?*/
						kGenPoint.iMapNo = pkGround->GetGroundNo();
						kGenPoint.iMoveRange = pkUnit->GetAbil(AT_MOVE_RANGE);
						kGenPoint.iPointGroup = 1000;
						int const iTunningNo = GetMonsterBagTunningNo(bag_itr->second.iTunningNo_Min, bag_itr->second.iTunningNo_Max);
						if( iTunningNo )
						{
							kGenPoint.iTunningNo = iTunningNo;
						}
						else
						{
							kGenPoint.iTunningNo = pkGround->GroundTunningNo();						
						}

						PgCreateSpreadPos kPosAction(pkUnit->GetPos());
						POINT3BY OrientedBy = pkUnit->GetOrientedVector();
						POINT3 Oriented(OrientedBy.x, OrientedBy.y, OrientedBy.z);
						Oriented.Normalize();
						kPosAction.AddFrontRange(pkGround->PhysXScene()->GetPhysXScene(), Oriented, static_cast<float>(pkSkillDef->GetAbil(AT_CREATE_ENTITY_RANGE)));
						kPosAction.AddDir( pkGround->PhysXScene()->GetPhysXScene(), Oriented, 200 );

						TBL_DEF_MONSTER_BAG_ELEMENTS const &kElement = element_itr->second;

						for ( int i=0; i<MAX_SUCCESS_RATE_ARRAY; ++i )
						{
							if ( kElement.aElement[i] )
							{
								auto ib = kContMon.insert(std::make_pair(kElement.aElement[i], 1));
								if(false==(ib.second))
								{
									++((*(ib.first)).second);
								}
							}
						}
		
						UNIT_PTR_ARRAY kArray;
						ContMon::const_iterator con_it = kContMon.begin();
						while(con_it!=kContMon.end())
						{
							kArray.clear();
							int const iNow = pkGround->GetUnitByClassNo((*con_it).first, UT_MONSTER, kArray);
							int iCallCount = std::max(0, iMax-iNow);				//최대-최소
							iCallCount = std::min(iCallCount, (*con_it).second);	//소환해야 될 갯수
							for( int i = 0; i<iCallCount; ++i)
							{
								if(iDistance)
								{
									kGenPoint.pt3Pos = GetRamdomPos(pkGround, pkUnit->GetPos(), iDistance);
								}
								else
								{
									kPosAction.PopPos( kGenPoint.pt3Pos );
								}
								pkGround->InsertMonster( kGenPoint, (*con_it).first, kMonGuid, pkUnit );
							}
							++con_it;
						}
					}
				}
			}
		}
		return 0;
	}

	return -1;
}


///////////////////////////////////////////////////////////
//  PgRandomSummonMonsterSkillFunction
///////////////////////////////////////////////////////////
int PgRandomSummonMonsterSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgRandomSummonMonsterSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iMonsterNo = pkSkillDef->GetAbil(AT_MON_SKILL_MONSTER_NO);
	int const iMonsterCount = pkSkillDef->GetAbil(AT_COUNT);
	if(0==iMonsterNo || 0==iMonsterCount)
	{
		return -1;
	}

	int iDistance = pkSkillDef->GetAbil(AT_DISTANCE);
	if(0==iDistance)
	{
		iDistance = AI_MONSTER_MIN_DISTANCE_CHASE_Q;
	}

	TBL_DEF_MAP_REGEN_POINT kRegenInfo;
	kRegenInfo.iMapNo = pkGround->GetGroundNo();
	BM::GUID kMonsterGuid;
	for( int i=0; i<iMonsterCount; ++i)
	{
		kRegenInfo.pt3Pos = GetRamdomPos(pkGround, pkUnit->GetPos(), iDistance);
		pkGround->InsertMonster( kRegenInfo, iMonsterNo, kMonsterGuid, pkUnit );
	}
	return -1;
}

///////////////////////////////////////////////////////////
//  PgKamikazeSkillFunction
///////////////////////////////////////////////////////////
void PgKamikazeSkillFunction_DoAction(CUnit * pkUnit, CSkillDef const* pkSkill, PgGround* pkGround)
{
	if(!pkUnit || !pkSkill || !pkGround)
	{
		return;
	}

	EPlayContentsType const eType = static_cast<EPlayContentsType>(pkSkill->GetAbil(AT_PLAY_CONTENTS_TYPE));
	switch(eType)
	{
	case EPCT_ELGA_EYE_MON:
		{
			CUnit * pkCaller = pkGround->GetUnit(pkUnit->Caller());
			if(pkCaller)
			{
				pkCaller->SetAbil(AT_ELGA_EYE_MON_HP, pkUnit->GetAbil(AT_HP));
				pkCaller->SetAbil(AT_ELGA_EYE_MON_HP_TIME, BM::GetTime32());
			}
		}break;
	}
}

int PgKamikazeSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgKamikazeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if (!pkUnit)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	int iTargetMeEffectNo = 0;
	if(pkUnit->IsUnitType(UT_SUMMONED))
	{
		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkGround->GetUnit(pkUnit->Caller()));
		PgMySkill* pkPlayerSkill = pkPlayer ? pkPlayer->GetMySkill() : NULL;
		if(pkPlayerSkill)
		{
			for(int i=0; i<CHILD_SKILL_MAX; ++i)
			{
				//영향 받는 베이스 스킬
				int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01+i);
				if(1 > iChildBaseSkillNo) { break; }

				//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
				int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
				if(1 > iLearnedChildSkillNo) { break; }

				CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
				if(pkLearnedSkillDef)
				{
					iTargetMeEffectNo = pkLearnedSkillDef->GetAbil(AT_TARGET_ME_EFFECTNUM);
					break;
				}
			}
		}
	}

	if(iTargetMeEffectNo)
	{
		pkUnit->AddEffect(iTargetMeEffectNo, 0, pArg, pkUnit);
	}

	int iRet = DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
	pkUnit->SetTarget(pkUnit->GetID());

	PgKamikazeSkillFunction_DoAction(pkUnit, pkSkill, pkGround);

	PgSkillHelpFunc::SkillOnDie(pkUnit, pkSkill->GetAbil(AT_SKILL_ON_DIE), true);
	return iRet;
}

///////////////////////////////////////////////////////////
//  60911031 PgEntityKamikazeSkillFunction
///////////////////////////////////////////////////////////
int PgEntityKamikazeSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgEntityKamikazeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		return -1;
	}

	int iRet = -1;
	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
	if(!pkEntity)
	{
		return -1;
	}

	iRet = DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
	pkEntity->SetTarget(pkUnit->GetID());
	int iDelTime = pkSkill->GetAbil(AT_ANIMATION_TIME);
	if(0 >= iDelTime)
	{
		iDelTime = 1000;// 없으면 1초
	}
	
	pkEntity->LifeTime(iDelTime);

	return iRet;
}

///////////////////////////////////////////////////////////
//  PgLavalonBlazeFunction
///////////////////////////////////////////////////////////
int PgLavalonBlazeFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	struct SBlazePos
	{
		SBlazePos()
			: fStart(0.f), fEnd(0.f)
		{
		}
		explicit SBlazePos(float const Start, float const End)
			: fStart(Start), fEnd(End)
		{}
		SBlazePos(SBlazePos const& rhs)
			: fStart(rhs.fStart), fEnd(rhs.fEnd)
		{}
		SBlazePos const& operator = (SBlazePos const& rhs)
		{
			fStart = rhs.fStart;
			fEnd = rhs.fEnd;
			return *this;
		}
		float fStart;
		float fEnd;
	};

	//				1
	//     	--------------------
	//    	|					|
	//	4	|					|    2
	//		|					|
	//		|					|
	//		|					|
	//     	---------------------
	//				3
	//static int const kStartLoc[] =	{1, 3, 4, 2};
	//static int const kEndLoc[] =		{3, 1, 2, 4};
	static SBlazePos const pHorizon[] =		{	SBlazePos(2.02f,4.02f), SBlazePos(2.14f,4.14f), SBlazePos(2.28f,4.28f), SBlazePos(2.42f,4.42f), SBlazePos(2.56f,4.56f),
												SBlazePos(2.70f,4.70f), SBlazePos(2.84f,4.84f), SBlazePos(2.98f,4.98f), };
	static SBlazePos const pVertical[] =	{	SBlazePos(1.02f,3.02f), SBlazePos(1.1f,3.1f), SBlazePos(1.2f,3.2f), SBlazePos(1.3f,3.3f), SBlazePos(1.4f,3.4f), 
												SBlazePos(1.5f,3.5f), SBlazePos(1.6f,3.6f), SBlazePos(1.7f,3.7f), SBlazePos(1.8f,3.8f), SBlazePos(1.9f,3.9f), SBlazePos(1.98f,3.98f) };

	typedef std::vector< SBlazePos > ContBlazePos;
	ContBlazePos kHorizon(pHorizon, pHorizon + PgArrayUtil::GetArrayCount(pHorizon));
	ContBlazePos kVertical(pVertical, pVertical + PgArrayUtil::GetArrayCount(pVertical));

	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
	int const iCurrentHP = pkUnit->GetAbil(AT_HP);

	float const fPercent = (float)iCurrentHP / (float)iMaxHP;
	//int const iNbLine = 5 - (int)(2 * fPercent);
	size_t const iHorizonPosCount = PgArrayUtil::GetArrayCount(pHorizon);
	size_t const iVerticalPosCount = PgArrayUtil::GetArrayCount(pVertical);

	int const iSelectedLine = BM::Rand_Range(1);
	ContBlazePos &kPosList = (0 == iSelectedLine)? kHorizon: kVertical; // 수평 / 수직
	size_t const& iSelectLineCount = (0 == iSelectedLine)? iHorizonPosCount: iVerticalPosCount;

	//size_t const iMaxResult = std::min(static_cast< size_t >((iSelectLineCount - iSelectLineCount * fPercent) + 3), iSelectLineCount);
	size_t const iMinResult = std::min(static_cast< size_t >((iSelectLineCount - iSelectLineCount * fPercent) + 3), iSelectLineCount-2);

	int const iNbLine = BM::Rand_Range(iSelectLineCount, iMinResult); // 1 ~ PosSize()

	int iLineIdx = 0;
	pkPacket->Push(iNbLine);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, iNbLine);
	
	std::random_shuffle(kPosList.begin(), kPosList.end());

	int const iSelectedDir = BM::Rand_Range(1); // 정방향 / 역방향
	while( iNbLine > iLineIdx )
	{
		//float const fStartPos = kStartLoc[iSelectedIndex] + BM::Rand_Range(999,1)*0.001f;
		//float const fEndPos = kEndLoc[iSelectedIndex] + BM::Rand_Range(999,1)*0.001f;

		float fStartPos = kPosList.at(iLineIdx).fStart;
		float fEndPos = kPosList.at(iLineIdx).fEnd;

		if( 0 != iSelectedDir )
		{
			std::swap(fStartPos, fEndPos);
		}

		pkPacket->Push(fStartPos);
		pkPacket->Push(fEndPos);
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + iLineIdx*2 + 1, (int)(fStartPos*1000.0f));
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + iLineIdx*2 + 2, (int)(fEndPos*1000.0f));
		
		++iLineIdx;
	}

	return 1;
}

int PgLavalonBlazeFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	int const iDetectRange = 50;

	POINT3 kTopLeft = pkGround->GetNodePosition("pt_blaze_01");		// Top Left
	POINT3 kTopRight = pkGround->GetNodePosition("pt_blaze_02");	// Top Right
	POINT3 kBottomLeft = pkGround->GetNodePosition("pt_blaze_03");	// Bottom Left
	POINT3 kBottomRight = pkGround->GetNodePosition("pt_blaze_04");	// Bottm Right

	POINT3 kUpperBase = kTopRight - kTopLeft;
	POINT3 kRightBase = kTopRight - kBottomRight;
	POINT3 kBottomBase = kBottomRight - kBottomLeft;
	POINT3 kLeftBase = kTopLeft - kBottomLeft;

	POINT3 kBasePos[]	= { POINT3(0,0,0), kTopLeft, kBottomRight, kBottomLeft, kBottomLeft };	//루아가 1BASE라서 [0]에 추가
	POINT3 kAlpha[] = { POINT3(0,0,0), kUpperBase, kRightBase, kBottomBase, kLeftBase};

	int iNbLine = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	pkUnitArray->clear();
	int iIndex = 0;
	while(iIndex<iNbLine)
	{
		float fMetaForStart = (float)(pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + iIndex * 2 + 1)) * 0.001f;
		float fMetaForEnd = (float)(pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + iIndex * 2 + 2)) * 0.001f;
		
		POINT3 kStartPos = kAlpha[(int)fMetaForStart]*(fMetaForStart - (int)fMetaForStart);
		kStartPos+=kBasePos[(int)fMetaForStart];
		kStartPos.z = 0.0f;

		POINT3 kEndPos = kAlpha[(int)fMetaForEnd]*(fMetaForEnd - (int)fMetaForEnd);
		kEndPos+=kBasePos[(int)fMetaForEnd];
		kEndPos.z = 0.0f;

		pkGround->GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, *pkUnitArray);
	
		++iIndex;
	}

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgLavalonMeteorFunction
///////////////////////////////////////////////////////////
int PgLavalonMeteorFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	//Meteor가 뿌려질 좌표.
	POINT3 kTopLeft = pkGround->GetNodePosition("pt_blaze_01");		// Top Left
	POINT3 kTopRight = pkGround->GetNodePosition("pt_blaze_02");	// Top Right
	POINT3 kBottomLeft = pkGround->GetNodePosition("pt_blaze_03");	// Bottom Left
	POINT3 kBottomRight = pkGround->GetNodePosition("pt_blaze_04");	// Bottm Right	

	//위 좌표는 반드시 의미에 부합해야 한다.
	POINT3 kFirstPos = kTopRight - kTopLeft;
	kFirstPos = kFirstPos*(BM::Rand_Range(999)*0.001f);
	kFirstPos+=kTopLeft;

	POINT3 kSecondPos = kBottomRight - kBottomLeft;
	kSecondPos = kSecondPos*(BM::Rand_Range(999)*0.001f);
	kSecondPos+=kBottomLeft;

	POINT3 kFinalPos = kFirstPos - kSecondPos;
	kFinalPos = kFinalPos*(BM::Rand_Range(999)*0.001f);
	kFinalPos+=kSecondPos;

	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, (int)kFinalPos.x);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+1, (int)kFinalPos.y);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+2, (int)kFinalPos.z);

	pkPacket->Push((int)kFinalPos.x);
	pkPacket->Push((int)kFinalPos.y);
	pkPacket->Push((int)kFinalPos.z);
	pkPacket->Push((int)0);

	return 1;
}

int PgLavalonMeteorFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	/*void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	int iPosX = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	int iPosY = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+1);
	int iPosZ = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+2);

	POINT3 kCollisionPos((float)iPosX, (float)iPosY, 0.0f);

	int const kDetectRange = 55;

	pkUnitArray->clear();	//타겟을 새로 잡아야 한다
	pkGround->GetUnitInRange(kCollisionPos, kDetectRange, UT_PLAYER, *pkUnitArray);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();

	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (!pkTarget || !pkUnit->IsTarget(pkTarget, true) || pkTarget->GetAbil(AT_CANNOT_DAMAGE) != 0)
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
		}
	}

	CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int iIndex = 0;

	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID());
		if (pkActionResult && !pkActionResult->GetInvalid())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkActionResult, iSkillNo, pArg);			
			++iIndex;
		}
		++unit_itor;
	}
	
	return iIndex;*/

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgLavalonMeteorBlueFunction
///////////////////////////////////////////////////////////
int PgLavalonMeteorBlueFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	//Meteor가 뿌려질 좌표.
	POINT3 kTopLeft = pkGround->GetNodePosition("pt_blaze_01");		// Top Left
	POINT3 kTopRight = pkGround->GetNodePosition("pt_blaze_02");	// Top Right
	POINT3 kBottomLeft = pkGround->GetNodePosition("pt_blaze_03");	// Bottom Left
	POINT3 kBottomRight = pkGround->GetNodePosition("pt_blaze_04");	// Bottm Right	

	POINT3 kCasterPos = pkUnit->GetPos();	//파란색은 캐릭터를 노린다
	POINT3 kFirstPos = kCasterPos + POINT3(60,60,60);
	POINT3 kSecondPos = kCasterPos - POINT3(60,60,60);

	POINT3 kFinalPos = kFirstPos - kSecondPos;
	kFinalPos = kFinalPos*(BM::Rand_Range(999)*0.001f);
	kFinalPos+=kSecondPos;

	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, (int)kFinalPos.x);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+1, (int)kFinalPos.y);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+2, (int)kFinalPos.z);

	pkPacket->Push((int)kFinalPos.x);
	pkPacket->Push((int)kFinalPos.y);
	pkPacket->Push((int)kFinalPos.z);
	pkPacket->Push((int)1);

	return 1;
}

int PgLavalonMeteorBlueFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
/*	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	int iPosX = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	int iPosY = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+1);
	int iPosZ = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+2);

	POINT3 kCollisionPos((float)iPosX, (float)iPosY, 0.0f);

	int const kDetectRange = 55;

	pkUnitArray->clear();	//타겟을 새로 잡아야 한다
	pkGround->GetUnitInRange(kCollisionPos, kDetectRange, UT_PLAYER, *pkUnitArray);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();

	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (!pkTarget || !pkUnit->IsTarget(pkTarget, true) || pkTarget->GetAbil(AT_CANNOT_DAMAGE) != 0)
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
		}
	}

	CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int iIndex = 0;

	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID());
		if (pkActionResult && !pkActionResult->GetInvalid())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkActionResult, iSkillNo, pArg);			
			++iIndex;
		}
		++unit_itor;
	}
	
	return iIndex;*/

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgLavalonBreathSkillFunction
///////////////////////////////////////////////////////////
int PgLavalonBreathSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgLavalonBreathSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 kP1, kP2, kP3, kP4;
	if( 6000900 == iSkillNo )		// 라발론 시점 중앙
	{
		kP1 = pkGround->GetNodePosition("breath start03");
		kP2 = pkGround->GetNodePosition("breath start04");
		kP3 = pkGround->GetNodePosition("breath end03");
		kP4 = pkGround->GetNodePosition("breath end04");
	}
	else if( 6000902 == iSkillNo )		// 라발론 시점 오른족
	{
		kP1 = pkGround->GetNodePosition("breath start01");
		kP2 = pkGround->GetNodePosition("breath start02");
		kP3 = pkGround->GetNodePosition("breath end01");
		kP4 = pkGround->GetNodePosition("breath end02");
	}
	else if( 6000901 == iSkillNo )	// 라발론 시점 왼쪽
	{
		kP1 = pkGround->GetNodePosition("breath start05");
		kP2 = pkGround->GetNodePosition("breath start06");
		kP3 = pkGround->GetNodePosition("breath end05");
		kP4 = pkGround->GetNodePosition("breath end06");
	}
	kP1.z = kP2.z = kP3.z = kP4.z = 0.f;

//	CAUTION_LOG(BM::LOG_LV1, BM::vstring("SkillNo: ") << iSkillNo);

	pkGround->GetUnitIn2DRectangle(kP1, kP2, kP3, kP4, UT_PLAYER, *pkUnitArray);
	
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  6031105 PgHomingSkillFunction
///////////////////////////////////////////////////////////
int PgHomingSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	CUnit* pkTarget = pkGround->GetUnit(pkUnit->GetTarget());
	if (pkTarget)	//타겟이 있으면 유도로 쏘자
	{
		pkPacket->Push(static_cast<ESendSkillPacketType>(SSPT_SKILL_PACKET_GUID));
		pkPacket->Push(static_cast<int>(1));	//몇개
		pkPacket->Push(pkUnit->GetTarget());
	}
	return 1;
}

int PgHomingSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}
///////////////////////////////////////////////////////////
//  6000944 PgMultiHomingSkillFunction
///////////////////////////////////////////////////////////
int PgMultiHomingSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	CUnit::DequeTarget& rkTargetList = pkUnit->GetTargetList();
	CUnit::DequeTarget::iterator itr = rkTargetList.begin();
	int iSize = rkTargetList.size();
	if(iSize > 0)
	{
		pkPacket->Push(static_cast<ESendSkillPacketType>(SSPT_SKILL_PACKET_GUID));
		pkPacket->Push(iSize);
		for(;itr != rkTargetList.end(); ++itr)
		{
			CUnit* pkTarget = pkGround->GetUnit(itr->kGuid);
			if(pkTarget)
			{
				pkPacket->Push(itr->kGuid);
			}
			else
			{
				pkPacket->Push(BM::GUID::NullData());
			}
		}
	}

	return 1;
}

int PgMultiHomingSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

///////////////////////////////////////////////////////////
//  PgForceFireSkillFunction
///////////////////////////////////////////////////////////
bool PgForceFireSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	if(0>=pkSkill->GetAbil(AT_MON_SKILL_CALLBY_SKILL))	//강제스킬번호가 없으면 무용지물
	{
		return false;
	}

	return true;
}
int PgForceFireSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgForceFireSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	typedef std::list< int > ContStackSkill;
	ContStackSkill kNewStack;
	int iForceReserveSkillNo = 0;

	int const iForceSkill = pkSkill->GetAbil(AT_MON_SKILL_CALLBY_SKILL);
	if( iForceSkill )
	{
		CSkillDef const* pkForceSkill = kSkillDefMgr.GetDef(iForceSkill);
		if( !pkForceSkill )
		{
			return -1;
		}
		
		switch( pkForceSkill->GetAbil(AT_SKILL_CHOOSE_TYPE) )
		{
		case EAI_CHOOSE_SKILL_SEQUENSE:
			{
				// 시퀀스 스킬이면 풀어서 넣어주자
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_10);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_09);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_08);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_07);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_06);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_05);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_04);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_03);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_02);
				std::back_inserter(kNewStack) = pkForceSkill->GetAbil(AT_MON_SKILL_01);

				ContStackSkill::iterator erase_iter = std::remove(kNewStack.begin(), kNewStack.end(), 0);
				if( kNewStack.end() != erase_iter )
				{
					kNewStack.erase(erase_iter, kNewStack.end());
				}

				if( 1 < kNewStack.size() )
				{
					iForceReserveSkillNo = kNewStack.back();
					kNewStack.pop_back();
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV1, __FL__ << _T(" Monster[") << pkUnit->GetAbil(AT_CLASS) << _T("] Force Skill[") << pkForceSkill->No() << _T("] is type [SKILL_SEQUENSE] but set skill reserved count is ") << kNewStack.size() );
					if( !kNewStack.empty() )
					{
						iForceReserveSkillNo = kNewStack.front();
					}
				}
			}break;
		default:
			{
				iForceReserveSkillNo = iForceSkill;
			}break;
		}
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (!pkTarget || !pkUnit->IsTarget(pkTarget, true, pkSkill->GetTargetType()) || pkSkill->GetAbil(AT_GRADE) != pkTarget->GetAbil(AT_GRADE)
			|| (pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO) != 0 && pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO) != pkTarget->GetAbil(AT_CLASS)))
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			if( false==kNewStack.empty() )
			{
				pkTarget->GetSkill()->SwapSkillStack(kNewStack);
			}

			if( iForceReserveSkillNo )
			{
				EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkill->GetAbil(AT_FORCESKILL_FLAG));
				pkTarget->GetSkill()->ForceReserve(iForceReserveSkillNo, eFlag);
				pkTarget->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
			}

			if( pkSkill->GetEffectNo() )
			{
				pkTarget->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit);
			}
			++unit_itor;
		}
	}
	return pkUnitArray->size();
}


///////////////////////////////////////////////////////////
//  PgForceSequenseFireSkillFunction
///////////////////////////////////////////////////////////
bool PgForceSequenseFireSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		return false;
	}

	return true;
}
int PgForceSequenseFireSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgForceSequenseFireSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_01));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_02));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_03));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_04));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_05));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_06));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_07));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_08));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_09));
	pkUnit->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_10));
	pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);

	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;

		if(pkTarget)
		{
			for(int i=0; i<EFFECTNUM_MAX; ++i)
			{
				int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1+i);
				if(0 < iAddEffect)
				{
					pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
				}
				else
				{
					break;
				}
			}
			pkTarget->SetTarget(pkUnit->GetID());
		}
		++unit_itor;
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  PgTargetForceSequenseFireSkillFunction
///////////////////////////////////////////////////////////
bool PgTargetForceSequenseFireSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		return false;
	}

	return true;
}
int PgTargetForceSequenseFireSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgTargetForceSequenseFireSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;

		if(pkTarget)
		{
			EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkill->GetAbil(AT_FORCESKILL_FLAG));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_01), eFlag);
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_02));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_03));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_04));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_05));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_06));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_07));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_08));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_09));
			pkTarget->GetSkill()->ForceReserve(pkSkill->GetAbil(AT_MON_SKILL_10));
			pkTarget->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);

			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}
		}
		++unit_itor;
	}
	return 1;
}

int PgTargetByDummySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	if(!pkUnitArray) {return -1;}

	int iDetectRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM2);
	if (0>=iDetectRange)
	{
		iDetectRange = 50;
	}

	BM::vstring kStartNode(szMapDummyName);
	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	kStartNode+=iNum;
	BM::vstring kEndNode(szMapDummyName);
	kEndNode+=(iNum+1);

	POINT3 kStartPos = pkGround->GetNodePosition(MB(kStartNode));		// Top Left
	POINT3 kEndPos = pkGround->GetNodePosition(MB(kEndNode));	// Top Right

	pkUnitArray->clear();

	pkGround->GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, *pkUnitArray);

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);;
}

int PgTargetByDummySkill2Function::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	if(!pkUnitArray) {return -1;}

	int iDetectRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM2);
	if (0>=iDetectRange)
	{
		iDetectRange = 50;
	}

	BM::vstring kStartNode(szMapDummyName);
	kStartNode+=pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);

	
	POINT3 kStartPos = pkGround->GetNodePosition(MB(kStartNode));	

	pkUnitArray->clear();

	if (0==kStartPos.x && 0==kStartPos.y && 0==kStartPos.z)	//못찾을 경우
	{
		kStartPos = pkUnit->GetPos();
	}

	pkGround->GetUnitInRange(kStartPos, iDetectRange, UT_PLAYER, *pkUnitArray);

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);;
}
int PgTeleportToDummySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	BM::vstring kTargetNode(szMapDummyName);
	kTargetNode+=pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);

	POINT3 kTargetPos = pkGround->GetNodePosition(MB(kTargetNode));	

	if (0==kTargetPos.x && 0==kTargetPos.y && 0==kTargetPos.z)	//못찾을 경우
	{
		kTargetPos = pkUnit->GetPos();
	}

	if(0 < pkSkill->GetEffectNo() )
	{
		SEffectCreateInfo kCreate;
		kCreate.eType = EFFECT_TYPE_NORMAL;
		kCreate.iEffectNum = pkSkill->GetEffectNo();
		kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
		pkUnit->AddEffect(kCreate);
	}

	pkUnit->SetPos(kTargetPos);

	return 1;
}

// 6074304 폭발 점멸(순간이동)
int PgExplosionBlinkFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);

	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		return -1;
	}

	int const iDistance = pkSkill->GetAbil(AT_DISTANCE);

	POINT3 kTargetPos = pkUnit->GetPos();

	int iChoose = BM::Rand_Index(2);
	Direction eDir = DIR_RIGHT;
	if(iChoose == 0)
	{
		eDir = DIR_LEFT;
	}

	GetDistanceToPosition( pkGround->GetPathRoot(),  pkUnit->GetPos(), eDir, static_cast<float>(iDistance), kTargetPos );

	if( SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0, EFlags_SetGoalPos|EFlags_CheckFromGround) )
	{
		pkUnit->SetPos(pkUnit->GoalPos());
	}
	else
	{
		pkUnit->GoalPos(pkUnit->GetPos());
	}


	pkUnitArray->clear();
	int iDetectRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	if (0>=iDetectRange)
	{
		iDetectRange = 100;
	}
	POINT3 kBlinkPos = pkUnit->GetPos();

	pkGround->GetUnitInRange(kBlinkPos, iDetectRange, UT_PLAYER, *pkUnitArray);

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return 0;
	}

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

// SkillNo 6000955
int PgClearAllEffectFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if (!pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}


	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (pkTarget)
		{
			if(pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO) != 0)
			{
				if(pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO) == pkTarget->GetAbil(AT_CLASS))
				{
					pkTarget->ClearAllEffect();
				}
			}
			else 
			{
				if(pkUnit->IsTarget(pkTarget, false, pkSkill->GetTargetType()))
				{
					pkTarget->ClearAllEffect();
				}
			}
		}
		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgMassiveProjectile2SkillFunction
///////////////////////////////////////////////////////////
bool PgMassiveProjectile2SkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	BM::GUID const& rkTargetGuid = pkUnit->GetTarget();
	POINT3 kTargetPos = pkGround->GetUnitPos(rkTargetGuid);

	int iMinDist = 0;
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if( NULL!=pkSkillDef )
	{
		iMinDist = pkSkillDef->GetAbil(AT_SKILL_MIN_RANGE);
	}

	//갈 수 없는 곳이면 시전하지 말자
	if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, (float)iMinDist, EFlags_MinDistance|EFlags_CheckFromGround))
	{
		return false;
	}

	return abs(kTargetPos.z - pkUnit->GetPos().z)<150;	//150높이
}

int PgMassiveProjectile2SkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	CUnit::DequeTarget const& rkList = pkUnit->GetTargetList();
	CUnit::DequeTarget::const_iterator it = rkList.begin();
	pkPacket->Push(std::min(static_cast<int>(rkList.size()), pkSkillDef->GetAbil(AT_MAX_TARGETNUM)));
	int iCount = 0;
	while(it != rkList.end() && iCount < pkSkillDef->GetAbil(AT_MAX_TARGETNUM))
	{
		POINT3 kTargetPos = pkGround->GetUnitPos((*it).kGuid);
		if(POINT3::NullData()==kTargetPos)
		{
			kTargetPos = pkUnit->GetPos();
		}
		pkPacket->Push(kTargetPos);
		++iCount;
		++it;
	}

	return iCount;
}

int PgMassiveProjectile2SkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

///////////////////////////////////////////////////////////
//  PgDarkBreathFunction
///////////////////////////////////////////////////////////
int PgDarkBreathFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	// 가로 4 세로 16 순간 데미지
	// 12개 위치중 한번에 6개 위치를 대미지 지역, 2가지 패턴
	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
	int const iCurrentHP = pkUnit->GetAbil(AT_HP);

	// 퍼센트, 가로 최소-최대, 세로 최소-최대
	// 30% 이상, 1-1, 2-4
	// 29-20, 1-2, 4-6
	// 19-10, 2-3, 7-9
	// 9-0, 3-4, 10-12
	float const fPercent = static_cast<float>(iCurrentHP) / static_cast<float>(iMaxHP);
	int iHor, iVer;
	if(fPercent >= 0.3)
	{
		iHor = 1;
		iVer = BM::Rand_Range(4,2);
	}
	else if(fPercent >= 0.2)
	{
		iHor = BM::Rand_Range(2,1);
		iVer = BM::Rand_Range(14,12);
	}
	else if(fPercent >= 0.1)
	{
		iHor = BM::Rand_Range(3,2);
		iVer = BM::Rand_Range(17,15);
	}
	else
	{
		iHor = BM::Rand_Range(4,3);
		iVer = BM::Rand_Range(20,18);
	}

	static int const pHorizon[] = {1,2,3,4};
	static int const pVertical[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

	typedef std::vector<int> ContInt;
	ContInt kHorizon(pHorizon, pHorizon + PgArrayUtil::GetArrayCount(pHorizon));
	ContInt kVertical(pVertical, pVertical + PgArrayUtil::GetArrayCount(pVertical));

	std::random_shuffle(kHorizon.begin(), kHorizon.end());
	std::random_shuffle(kVertical.begin(), kVertical.end());

	pkPacket->Push(iHor);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, iHor);
	for(int ih=0; ih<iHor; ++ih)
	{
		pkPacket->Push(kHorizon.at(ih));
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + ih + 1, kHorizon.at(ih));
	}

	pkPacket->Push(iVer);
	pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + iHor + 1, iVer);
	for(int iv=0; iv<iVer; ++iv)
	{
		pkPacket->Push(kVertical.at(iv));
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + iHor + iv + 2, kVertical.at(iv));
	}



	return 1;
}

int PgDarkBreathFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		return -1;
	}

	int const iDetectRange = 38;

	POINT3 kTopLeft = pkGround->GetNodePosition("breath_L_top");			// left top
	POINT3 kTopRight = pkGround->GetNodePosition("breath_R_top");			// right top
	POINT3 kBottomLeft = pkGround->GetNodePosition("breath_L_bottom");		// left bottom
	POINT3 kBottomRight = pkGround->GetNodePosition("breath_R_bottom");		// right bottom

	POINT3 kUpperBase = (kTopRight + kTopLeft) / 2;
	POINT3 kRightBase = (kTopRight + kBottomRight) / 2;
	POINT3 kBottomBase = (kBottomRight + kBottomLeft) / 2;
	POINT3 kLeftBase = (kTopLeft + kBottomLeft) / 2;

	float fHorLen = kUpperBase.y - kBottomBase.y;
	float fVerLen = kRightBase.x - kLeftBase.x;

	POINT3 kLBpos(kLeftBase.x, kBottomBase.y, kBottomBase.z);
	POINT3 kLTpos(kLeftBase.x, kUpperBase.y, kBottomBase.z);
	POINT3 kRBpos(kRightBase.x, kBottomBase.y, kBottomBase.z);
	
	int const iHorMaxCount = 4;
	int const iVerMaxCount = 20;

	// 좌우 길이가 같다.
	float const fLen = fHorLen / iHorMaxCount;
	float const fHalfLen = fLen / 2;
	
	pkUnitArray->clear();
	int iHor = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	for(int ih=0; ih<iHor; ++ih)
	{
		int iPos = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + ih + 1);
		POINT3 kOffset(0, fLen*iPos-fHalfLen, 0);
		POINT3 kStartPos = kLBpos + kOffset;
		POINT3 kEndPos = kRBpos + kOffset;
		pkGround->GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, *pkUnitArray);
	}
	int iVer = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + iHor + 1);
	for(int iv=0; iv<iVer; ++iv)
	{
		int iPos = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + iHor + iv + 2);
		POINT3 kOffset(fLen*iPos-fHalfLen, 0, 0);
		POINT3 kStartPos = kLBpos + kOffset;
		POINT3 kEndPos = kLTpos + kOffset;
		pkGround->GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, *pkUnitArray);
	}
	pkUnitArray->sort();
	pkUnitArray->unique();

	{// 엔터티 생성, 데미지 안개 
		int const iSelect = BM::Rand_Range(2,1);
		for(int i=1; i<=6; ++i)
		{
			BM::vstring kNodeName("mist");
			kNodeName+=iSelect;
			kNodeName+="_";
			kNodeName+=i;

			POINT3 kNodePos = pkGround->GetNodePosition(MB(kNodeName));

			SCreateEntity kCreateInfo;
			kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
			kCreateInfo.kClassKey.nLv = 1;
			kCreateInfo.bUniqueClass = false;
			kCreateInfo.kGuid.Generate();

			// 바닥에 밀착 시킨다
			NxRay kRay(NxVec3(kNodePos.x, kNodePos.y, kNodePos.z+20), NxVec3(0, 0, -1.0f));
			NxRaycastHit kHit;
			NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
			if(pkHitShape)
			{
				kNodePos.z = kHit.worldImpact.z;
			}
			kCreateInfo.ptPos = kNodePos;
		
			CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, kNodeName);
			if(pkEntity)
			{
				int const iMagic = pkUnit->GetAbil(AT_MAGIC_ATTACK_MAX) * pkSkill->GetAbil(AT_MAGIC_DMG_PER) / ABILITY_RATE_VALUE;
				pkEntity->SetAbil(AT_ATTR_ATTACK, 1000);
			}
		}
	}

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

// 6074305 블랙홀
int PgBlackholeFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SKillNo="<<iSkillNo);
		return -1;
	}

	int const iDistance = pkSkill->GetAbil(AT_DISTANCE);

	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = 1;
	kCreateInfo.bUniqueClass = true;
	kCreateInfo.kGuid.Generate();

	POINT3 ptEntityPos = pkUnit->GetPos();
	POINT3 kTargetPos = ptEntityPos;
	//GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), pkUnit->FrontDirection(), static_cast<float>(iDistance), ptEntityPos);
	NxVec3 kVision;
	pkGround->GetVisionDirection(pkUnit, kVision);
	kVision *= (NxReal)iDistance;
	kTargetPos.x = ptEntityPos.x+kVision.x; 
	kTargetPos.y = ptEntityPos.y+kVision.y; 
	kTargetPos.z = ptEntityPos.z+kVision.z;
	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}
	else
	{
		kTargetPos = ptEntityPos;
	}
	kCreateInfo.ptPos = kTargetPos;

		CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, _T("Blackhole"));

	return 1;
}

///////////////////////////////////////////////////////////
//  PgSummonBoneFunction
///////////////////////////////////////////////////////////
int PgSummonBoneFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	// 3 * 15 칸, 3중에 랜덤으로 2개를 선택 총 30칸에 대미지
	int const MAX_LINE_COUNT = 15;
	int const MAX_SELECT_COUNT = 3;
	int iIndex = AT_SKILL_CUSTOM_DATA_01;
	for(int i=0; i<MAX_LINE_COUNT; ++i)
	{
		int iRand = BM::Rand_Index(MAX_SELECT_COUNT);
		for( int j=0; j<MAX_SELECT_COUNT; ++j )
		{
			if( iRand != j )
			{
				pkPacket->Push(j);
				pkUnit->SetAbil(iIndex, j);
				++iIndex;
			}
		}
	}
	return 1;
}
int PgSummonBoneFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if( !pkGround )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Ground SkillNo = ") << iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	int const MAX_LINE_COUNT = 15;
	int const MAX_SELECT_COUNT = 3;

	POINT3 kTopLeft = pkGround->GetNodePosition("breath_L_top");			// left top
	POINT3 kTopRight = pkGround->GetNodePosition("breath_R_top");			// right top
	POINT3 kBottomLeft = pkGround->GetNodePosition("breath_L_bottom");		// left bottom
	POINT3 kBottomRight = pkGround->GetNodePosition("breath_R_bottom");		// right bottom

	POINT3 kUpperBase = (kTopRight + kTopLeft) / 2;
	POINT3 kRightBase = (kTopRight + kBottomRight) / 2;
	POINT3 kBottomBase = (kBottomRight + kBottomLeft) / 2;
	POINT3 kLeftBase = (kTopLeft + kBottomLeft) / 2;
	
	float fUnitLength = (kUpperBase.y - kBottomBase.y ) / MAX_SELECT_COUNT;
	float fUnitLengthHalf = fUnitLength / 2;

	UNIT_PTR_ARRAY kUnitArray;
	int iIndex = AT_SKILL_CUSTOM_DATA_01;
	for( int i=0; i<MAX_LINE_COUNT; ++i )
	{
		for( int j=0; j<MAX_SELECT_COUNT-1; ++j )
		{
			int iPos = pkUnit->GetAbil(iIndex);
			++iIndex;
			POINT3 kP1, kP2, kP3, kP4;
			kP1 = kP2 = kP3 = kP4 = kUpperBase;

			kP1.x = kRightBase.x - ((i+1) * fUnitLength);
			kP1.y = kUpperBase.y - (iPos * fUnitLength);

			kP2.x = kRightBase.x - (i * fUnitLength);
			kP2.y = kUpperBase.y - (iPos * fUnitLength);

			kP3.x = kRightBase.x - ((i+1) * fUnitLength);
			kP3.y = kUpperBase.y - ((iPos+1) * fUnitLength);

			kP4.x = kRightBase.x - (i * fUnitLength);
			kP4.y = kUpperBase.y - ((iPos+1) * fUnitLength);
			pkGround->GetUnitIn2DRectangle(kP1, kP2, kP3, kP4, UT_PLAYER, kUnitArray);
		}
	}

	pkUnitArray->clear();
	if( !kUnitArray.empty() )
	{
		UNIT_PTR_ARRAY::iterator itr = kUnitArray.begin();
		for( ; itr != kUnitArray.end(); ++itr )
		{
			UNIT_PTR_ARRAY::iterator find_iter = std::find(pkUnitArray->begin(), pkUnitArray->end(), itr->pkUnit);
			if( find_iter == pkUnitArray->end() )
			{
				pkUnitArray->Add(itr->pkUnit);
			}
		}
	}

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgSummonBoneFunction
///////////////////////////////////////////////////////////
int PgSummonBone2Function::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	//int const iStartPos = BM::Rand_Range(3, 1);
	//pkPacket->Push(iStartPos);
	//pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, iStartPos);


	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	float fDistance = 1000.0f;
	if (pkSkillDef)
	{
		int iDist = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
		if (0 < iDist)
		{
			fDistance = (float)iDist;
		}	
	}

	//int const iStartPos = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	// 맵상에 더미를 찾는다, 3개
	int const iStartPos = BM::Rand_Range(3, 1);
	pkPacket->Push(iStartPos);
	
	BM::vstring kStartNode("bone_start");
	kStartNode+=iStartPos;
	POINT3 kStartPos = pkGround->GetNodePosition(MB(kStartNode));
	pkUnit->StartPos(kStartPos);

	POINT3 kGoalPos = kStartPos;
	kGoalPos.x-=fDistance;
	pkUnit->GoalPos(kGoalPos);

	return 1;
}
int PgSummonBone2Function::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	pkUnitArray->clear();
	int iDetectRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	if (0>=iDetectRange)
	{
		iDetectRange = 100;
	}
	POINT3 kTargetPos = pkUnit->Projectile(0).ptCurrent;

	pkGround->GetUnitInRange(kTargetPos, iDetectRange, UT_PLAYER, *pkUnitArray);

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return 0;
	}
	
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  60009881 PgForceMoveFunction
///////////////////////////////////////////////////////////
int PgForceMoveFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	pArg->Get(ACTARG_GROUND, pkGround);

	if( !pkGround )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Ground SkillNo = ") << iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	int const iMoveDir = pkSkill->GetAbil(AT_MON_SKILL_FORCE_MOVE_DIR);
	if( 0 == iMoveDir )
	{
		return 1;
	}

	POINT3 ptMovingVec;
	if( 1 == iMoveDir)
	{
		ptMovingVec = POINT3(-1, 0, 0);
	}
	else
	{
		ptMovingVec = POINT3(1, 0, 0);
	}

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);
	if ( 0 == iDistance )
	{
		iDistance = 100;
	}

	POINT3BY kPathNormalBy = pkUnit->PathNormal();
	POINT3 kPathNormal(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	kPathNormal.Normalize();

	POINT3 kOrientedVec(0,0,0);
	if(kPathNormal == POINT3(0, 1, 0) || kPathNormal == POINT3(0, -1, 0))
	{
		kOrientedVec.Set(ptMovingVec.x, 0.0f, 0.0f);
	}
	else
	{
		// OrientedVector 계산.
		ptMovingVec.z = 0;
		kOrientedVec = kPathNormal.Cross(POINT3(0.0f ,0.0f ,1.0f));	// Right Vector
		POINT3 kCrossed = kPathNormal.Cross(ptMovingVec);
		kCrossed.Normalize();

		if(kCrossed == POINT3(0.0f, 0.0f, 1.0f))
		{
			kOrientedVec *= -1.0f;
		}
	}

	kOrientedVec.Normalize();
	kOrientedVec*=static_cast<float>(iDistance);
	POINT3 kTargetPos;
	POINT3 const& rkUnitPos = pkUnit->GetPos();
	kTargetPos.x = rkUnitPos.x + kOrientedVec.x;
	kTargetPos.y = rkUnitPos.y + kOrientedVec.y;
	kTargetPos.z = rkUnitPos.z + kOrientedVec.z;

	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}

	pkUnit->SetPos(kTargetPos);

	//SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos);

	pkUnitArray->clear();
	int iDetectRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	if (0>=iDetectRange)
	{
		iDetectRange = 100;
	}
	pkGround->GetUnitInRange(kTargetPos, iDetectRange, UT_PLAYER, *pkUnitArray);
	
	DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget && 0 < pkSkill->GetEffectNo() )
		{	
			pkTarget->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
			//SActArg kActArg;
			//pkTarget->AddEffect(EFFECT_TYPE_NORMAL, pkSkill->GetEffectNo(), 0, &kActArg);
		}
		++unit_itor;
	}


	return 1;
}

///////////////////////////////////////////////////////////
//  6089400 PgMoveToTargetDummyFunction
///////////////////////////////////////////////////////////
int	PgMoveToTargetDummyFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround )
	{
		return 0;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return 0;
	}

	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	if( 0 == iNum)
	{
		return 0;
	}

	BM::vstring kNode("bone_start");
	kNode+=iNum;
	
	POINT3 kNodePos = pkGround->GetNodePosition(MB(kNode));
	pkPacket->Push(kNodePos);
	pkUnit->GoalPos(kNodePos);
	return 1;
}

bool PgMoveToTargetDummyFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround )
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	if( 0 == iNum)
	{
		return false;
	}

	BM::vstring kNode("bone_start");
	kNode+=iNum;
	
	
	POINT3 kNodePos = pkGround->GetNodePosition(MB(kNode));
	if (0==kNodePos.x && 0==kNodePos.y && 0==kNodePos.z)	//못찾을 경우
	{
		return false;
	}

	return SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kNodePos, AI_Z_LIMIT, 0, EFlags_None);
}

int PgMoveToTargetDummyFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return pkUnit->GetAbil(AT_R_PHY_ATTACK_MAX) ? DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true) : 1;
}

///////////////////////////////////////////////////////////
//  6089401 PgHealToTargetMonsterFunction
///////////////////////////////////////////////////////////
void PgHealToTargetMonsterFunction_DoAction(CUnit * pkUnit, CUnit * pkTarget, CSkillDef const* pkSkill, int const iAddRate, SActArg const* pArg)
{
	if(!pkUnit || !pkTarget || !pkSkill || 0==iAddRate)
	{
		return;
	}

	int const iCur = pkTarget->GetAbil(AT_HP);

	if(0 < SkillFuncUtil::OnModifyHP(pkTarget, NULL, iAddRate, 0, pkUnit, NULL) )
	{
		int iAddEffect = 0;
		for(int i=0; i<EFFECTNUM_MAX; ++i)
		{
			int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1+i);
			if(iAddEffect)
			{
				pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
			}
			else
			{
				break;
			}
		}
	}

	EPlayContentsType const eType = static_cast<EPlayContentsType>(pkSkill->GetAbil(AT_PLAY_CONTENTS_TYPE));
	switch(eType)
	{
	case EPCT_ELGA_EYE_MON:
		{
			pkUnit->SetAbil(AT_ELGA_EYE_MON_HP, iCur);
			pkUnit->SetAbil(AT_ELGA_EYE_MON_HP_TIME, BM::GetTime32());
		}break;
	}
}

bool PgHealToTargetMonsterFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
    EUnitType eUnitType = static_cast<EUnitType>(pkSkill->GetAbil(AT_MON_SKILL_UNIT_TYPE));
    if( 0==eUnitType )
    {
        eUnitType = UT_MONSTER;
    }

	CUnit* pkTarget = NULL;
    if( iMonNo )
    {
        pkTarget = pkGround->GetUnitByClassNo(iMonNo, eUnitType);
    }
    else
    {
        pkTarget = pkGround->GetUnitByType(eUnitType);
    }

	if(NULL == pkTarget)
	{
		return false;
	}

	int const iRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
	float fDistance = GetDistance(pkUnit->GetPos(), pkTarget->GetPos());
	if( iRange < fDistance )
	{
		return false;
	}
	return true;
}
int PgHealToTargetMonsterFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

    if( pkUnitArray->empty() )
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
    EUnitType eUnitType = static_cast<EUnitType>(pkSkill->GetAbil(AT_MON_SKILL_UNIT_TYPE));
    if( 0==eUnitType )
    {
        eUnitType = UT_MONSTER;
    }

	bool bFind = false;
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
        bool bFindType = false;
        bool bFindClass = false;
		bool bMyControl = false;
        if( pkTarget )
        {
            bFindType = pkTarget->IsUnitType(eUnitType);
            bFindClass = iMonNo ? pkTarget->GetAbil(AT_CLASS)==iMonNo : true;
			bMyControl = pkTarget->Caller()==pkUnit->GetID();
        }

		// 해당 몬스터만 남긴다.
        if( !pkTarget || !(bFindType && bFindClass) || (UT_SUMMONED==eUnitType && false==bMyControl))
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
			bFind = true;
		}
	}

	if( !bFind )
	{
		return -1;
	}

	if(0 < pkSkill->GetEffectNo() )
	{
		pkUnit->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit);
	}

	int iAddRate = pkSkill->GetAbil(AT_MON_ADD_HP_RATE);
	if( iAddRate == 0 )//  디폴트는 1% 로 하자.
	{
		iAddRate = 100;
	}

	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgHealToTargetMonsterFunction_DoAction(pkUnit, pkTarget, pkSkill, iAddRate, pArg);
		++unit_itor;
	}

	pkUnit->SetTarget(pkUnit->GetID());
	return 1;
}


///////////////////////////////////////////////////////////
//  PgHealToTargetMonster_2_Function
//  TargetType가 자신인 경우 100%확률로 스킬이 발동된다고 가정을 하고 어빌을 통해서 대상을 다시 재검색함
///////////////////////////////////////////////////////////
bool PgHealToTargetMonster_2_Function::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	if(ESTARGET_SELF&pkSkill->GetTargetType())
	{
		return true;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
    EUnitType eUnitType = static_cast<EUnitType>(pkSkill->GetAbil(AT_MON_SKILL_UNIT_TYPE));
    if( 0==eUnitType )
    {
        eUnitType = UT_MONSTER;
    }

	CUnit* pkTarget = NULL;
    if( iMonNo )
    {
        pkTarget = pkGround->GetUnitByClassNo(iMonNo, eUnitType);
    }
    else
    {
        pkTarget = pkGround->GetUnitByType(eUnitType);
    }

	if(NULL == pkTarget)
	{
		return false;
	}

	int const iRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
	float fDistance = GetDistance(pkUnit->GetPos(), pkTarget->GetPos());
	if( iRange < fDistance )
	{
		return false;
	}
	return true;
}
int PgHealToTargetMonster_2_Function::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	EUnitType const eUnitType = static_cast<EUnitType>(pkSkill->GetAbil(AT_MON_SKILL_UNIT_TYPE));
	if(ESTARGET_SELF&pkSkill->GetTargetType())
	{
		pkUnitArray->clear();
		pkGround->GetUnitInRange(pkUnit->GetPos(), PgAdjustSkillFunc::GetAttackRange(pkUnit, pkSkill), eUnitType, *pkUnitArray);
	}

	if(0 < pkSkill->GetEffectNo() )
	{
		pkUnit->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit);
	}

	int const iAddRate = pkSkill->GetAbil(AT_MON_ADD_HP_RATE);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID(), true);	//CS_SkillResult를 안쓰니까 여기서 직접 만들어줘야 함
		if (pkActionResult)
		{
			PgHealToTargetMonsterFunction_DoAction(pkUnit, pkTarget, pkSkill, iAddRate, pArg);
			AddDamageEffectAndChangeBlockValue(pkUnit, pkTarget, pkSkill, pkActionResult);
		}
		++unit_itor;
	}

	pkUnit->SetTarget(pkUnit->GetID());

	PgSkillHelpFunc::SkillOnDie(pkUnit, pkSkill->GetAbil(AT_SKILL_ON_DIE), true);
	return 1;
}

///////////////////////////////////////////////////////////
//  6000982 PgSummonMonsterRegenPointFunction
///////////////////////////////////////////////////////////
int PgSummonMonsterRegenPointFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	std::vector<int> vecBagControl;
	int iMonBagControl = pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL);
	if( iMonBagControl )
	{
		vecBagControl.push_back(iMonBagControl);
	}
	for( int i=AT_SKILL_SUMMON_MONBAGCONTROL_01; i<=AT_SKILL_SUMMON_MONBAGCONTROL_10; ++i)
	{
		int iMonBagControl = pkSkillDef->GetAbil(i);
		if( iMonBagControl )
		{
			vecBagControl.push_back(iMonBagControl);
		}
	}
	if( vecBagControl.empty() )
	{
		return -1;
	}


	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pContMapRegenPoint = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );
	g_kTblDataMgr.GetContDef(pContMapRegenPoint );

	if ( pContMonBagControl && pContMonBag && pContMonBagElement )
	{
		ECREATE_HP_TYPE const eCreate_HP_Type = GetCreateHPType(pkSkillDef);
		for( unsigned int i=0; i<vecBagControl.size(); ++i)
		{
			int iBag = vecBagControl.at(i);
			CONT_DEF_MONSTER_BAG_CONTROL::const_iterator control_itr = pContMonBagControl->find(iBag);
			if ( control_itr != pContMonBagControl->end() )
			{
				int const iMonBagNo = control_itr->second.aBagElement[pkGround->GetMapLevel()];
				CONT_DEF_MONSTER_BAG::const_iterator bag_itr = pContMonBag->find( iMonBagNo );
				if ( bag_itr != pContMonBag->end() )
				{
					CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itr = pContMonBagElement->find(bag_itr->second.iElementNo);
					if( element_itr != pContMonBagElement->end() )
					{
						BM::GUID kMonGuid;

						TBL_DEF_MAP_REGEN_POINT kGenPoint;
						kGenPoint.cBagControlType = ERegenBag_Type_Monster;
						kGenPoint.dwPeriod = 0;
						kGenPoint.iBagControlNo = 0;/*없어도 되지 않을까?*/
						kGenPoint.iMapNo = pkGround->GetGroundNo();
						kGenPoint.iMoveRange = pkUnit->GetAbil(AT_MOVE_RANGE);
						kGenPoint.iPointGroup = 1000;
						int const iTunningNo = GetMonsterBagTunningNo(bag_itr->second.iTunningNo_Min, bag_itr->second.iTunningNo_Max);
						if(iTunningNo)
						{
							kGenPoint.iTunningNo = iTunningNo;
						}
						else
						{
							kGenPoint.iTunningNo =  pkGround->GroundTunningNo();
						}

						TBL_DEF_MONSTER_BAG_ELEMENTS const &kElement = element_itr->second;

						int iIndex = 0;
						if( !kElement.aElement[iIndex] )
						{
							return -1;
						}

						int const iMapNo = pkGround->GetGroundNo();

						CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pContMapRegenPoint->begin();
						while(regen_itor != pContMapRegenPoint->end())
						{//지금맵용 리젠포인트. 셋팅.
							if( iMapNo == regen_itor->second.iMapNo)
							{
								if(iMonBagNo == regen_itor->second.iBagControlNo)
								{
									kGenPoint.pt3Pos = regen_itor->second.pt3Pos;
									kGenPoint.cDirection = regen_itor->second.cDirection;
									pkGround->InsertMonster( kGenPoint, kElement.aElement[iIndex], kMonGuid, pkUnit, false, 0, eCreate_HP_Type );
									++iIndex;
									if( !kElement.aElement[iIndex] )
									{
										break;
									}
								}
							}
							++regen_itor;
						}

						if(0 < pkSkillDef->GetEffectNo() )
						{
							pkUnit->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
							//SActArg kActArg;
							//pkUnit->AddEffect(EFFECT_TYPE_NORMAL, pkSkillDef->GetEffectNo(), 0, &kActArg);
						}

					}
				}
			}
		}
		return 0;
	}

	return -1;
}

///////////////////////////////////////////////////////////
//  3531030 PgSummonMonsterRegenPointChooseFunction
///////////////////////////////////////////////////////////
void PgSummonMonsterRegenPointChooseFunction::GetChoose(EBagControlChooseType const eChooseType, CONT_BAGCONTROL & vecBagControl)
{
	switch(eChooseType)
	{
	case BCCT_RANDOM:
		{
			/*int iTotal = 0;
			for(CONT_BAGCONTROL::const_iterator c_it=vecBagControl.begin(); c_it!=vecBagControl.end(); ++c_it)
			{
				iTotal += (*c_it).second;
			}*/
			
			int const iMaxSize = vecBagControl.size();
			if(iMaxSize)
			{
				CONT_BAGCONTROL::value_type kValue = vecBagControl.at( BM::Rand_Index(iMaxSize) );
				
				vecBagControl.clear();
				vecBagControl.push_back(kValue);
			}
		}break;
	}
}

int PgSummonMonsterRegenPointChooseFunction::GetBagControl(CSkillDef const* pkSkillDef, CONT_BAGCONTROL & vecBagControl)
{//Begin에서 불름
	if ( !pkSkillDef )
	{
		return 0;
	}

	int iTotalRate = 0;
	if(int iMonBagControl = pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL) )
	{
		int const iRate = pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL_RATE);
		iTotalRate += iRate;

		vecBagControl.push_back(std::make_pair(iMonBagControl,iRate));
	}
	for( int i=AT_SKILL_SUMMON_MONBAGCONTROL_01; i<=AT_SKILL_SUMMON_MONBAGCONTROL_10; ++i)
	{
		if(int iMonBagControl = pkSkillDef->GetAbil(i) )
		{
			int const iRate = pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL_RATE_01+i);
			iTotalRate += iRate;

			vecBagControl.push_back(std::make_pair(iMonBagControl,iRate));
		}
	}
	return iTotalRate;
}

int PgSummonMonsterRegenPointChooseFunction::GetBagControl(CUnit const* pkUnit, CONT_BAGCONTROL & vecBagControl)
{//Fire에서 불름
	if ( !pkUnit )
	{
		return 0;
	}

	if(int iMonBagControl = pkUnit->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL) )
	{
		vecBagControl.push_back(std::make_pair(iMonBagControl,0));
	}
	for( int i=AT_SKILL_SUMMON_MONBAGCONTROL_01; i<=AT_SKILL_SUMMON_MONBAGCONTROL_10; ++i)
	{
		if(int iMonBagControl = pkUnit->GetAbil(i) )
		{
			vecBagControl.push_back(std::make_pair(iMonBagControl,0));
		}
	}
	return 0;
}

void PgSummonMonsterRegenPointChooseFunction::ClearBagControl(CUnit * pkUnit)
{
	if(pkUnit)
	{
		pkUnit->SetAbil(AT_SKILL_SUMMON_MONBAGCONTROL, 0);
		for(int i=AT_SKILL_SUMMON_MONBAGCONTROL_01; i<=AT_SKILL_SUMMON_MONBAGCONTROL_10; ++i)
		{
			pkUnit->SetAbil(i, 0);
		}
	}
}

void PgSummonMonsterRegenPointChooseFunction::SetBagControl(CUnit * pkUnit, int const iIdx, int const iBag)
{
	if(pkUnit)
	{
		if(0 == iIdx)
		{
			pkUnit->SetAbil(AT_SKILL_SUMMON_MONBAGCONTROL, iBag);
		}
		else
		{
			int const iIndex = AT_SKILL_SUMMON_MONBAGCONTROL_01 + (iIdx-1);
			pkUnit->SetAbil(iIndex, iBag);
		}
		
	}
}

int PgSummonMonsterRegenPointChooseFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pContMapRegenPoint = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );
	g_kTblDataMgr.GetContDef(pContMapRegenPoint );

	if ( !pContMonBagControl || !pContMonBag || !pContMonBagElement || !pContMapRegenPoint )
	{
		return -1;
	}

	CONT_BAGCONTROL vecBagControl;
	GetBagControl(pkSkillDef, vecBagControl);
	GetChoose(static_cast<EBagControlChooseType>(pkSkillDef->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL_TYPE)), vecBagControl);
	if( vecBagControl.empty() )
	{
		return -1;
	}

	ClearBagControl(pkUnit);

	// 타겟 전송 수량(몇개가 될지 모르므로 임시로 설정하고 추후 수정)
	size_t const iWRPos = pkPacket->WrPos();
	size_t iWRSize = 0;
	pkPacket->Push( iWRSize );

	//
	int iIdx = 0;
	int const iMapNo = pkGround->GetGroundNo();
	for( unsigned int i=0; i<vecBagControl.size(); ++i)
	{
		CONT_BAGCONTROL::value_type const& kBag = vecBagControl.at(i);
		CONT_DEF_MONSTER_BAG_CONTROL::const_iterator control_itr = pContMonBagControl->find(kBag.first);
		if ( control_itr != pContMonBagControl->end() )
		{
			int const iMonBagNo = control_itr->second.aBagElement[pkGround->GetMapLevel()];
			CONT_DEF_MONSTER_BAG::const_iterator bag_itr = pContMonBag->find( iMonBagNo );
			if ( bag_itr != pContMonBag->end() )
			{
				CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itr = pContMonBagElement->find(bag_itr->second.iElementNo);
				if( element_itr != pContMonBagElement->end() )
				{
					TBL_DEF_MONSTER_BAG_ELEMENTS const &kElement = element_itr->second;
					int iIndex = 0;
					if( !kElement.aElement[iIndex] )
					{
						break;
					}

					CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pContMapRegenPoint->begin();
					while(regen_itor != pContMapRegenPoint->end())
					{//지금맵용 리젠포인트. 셋팅.
						if(iMapNo == regen_itor->second.iMapNo)
						if(iMonBagNo == regen_itor->second.iBagControlNo)
						{
							pkPacket->Push(regen_itor->second.pt3Pos);
							
							++iIndex;
							if( !kElement.aElement[iIndex] )
							{
								break;
							}
						}
						++regen_itor;
					}

					SetBagControl(pkUnit, iIdx, kBag.first);
					++iIdx;
				}
			}
		}
	}

	// 실제 타겟 전송 수량
	iWRSize = pkPacket->WrPos() - iWRPos - sizeof(iWRSize);
	iWRSize /= sizeof(POINT3);
	pkPacket->ModifyData( iWRPos, &iWRSize, sizeof(iWRSize) );

	return iWRSize;
}

int PgSummonMonsterRegenPointChooseFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pContMapRegenPoint = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );
	g_kTblDataMgr.GetContDef(pContMapRegenPoint );

	if ( !pContMonBagControl || !pContMonBag || !pContMonBagElement || !pContMapRegenPoint )
	{
		return -1;
	}

	CONT_BAGCONTROL vecBagControl;
	GetBagControl(pkUnit, vecBagControl);
	if( vecBagControl.empty() )
	{
		return -1;
	}

	int const iMapNo = pkGround->GetGroundNo();
	int iInsertMonsterCount = 0;
	ECREATE_HP_TYPE const eCreate_HP_Type = GetCreateHPType(pkSkillDef);
	for( unsigned int i=0; i<vecBagControl.size(); ++i)
	{
		CONT_BAGCONTROL::value_type const& kBag = vecBagControl.at(i);
		CONT_DEF_MONSTER_BAG_CONTROL::const_iterator control_itr = pContMonBagControl->find(kBag.first);
		if ( control_itr != pContMonBagControl->end() )
		{
			int const iMonBagNo = control_itr->second.aBagElement[pkGround->GetMapLevel()];
			CONT_DEF_MONSTER_BAG::const_iterator bag_itr = pContMonBag->find( iMonBagNo );
			if ( bag_itr != pContMonBag->end() )
			{
				CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itr = pContMonBagElement->find(bag_itr->second.iElementNo);
				if( element_itr != pContMonBagElement->end() )
				{
					TBL_DEF_MONSTER_BAG_ELEMENTS const &kElement = element_itr->second;

					int iIndex = 0;
					if( !kElement.aElement[iIndex] )
					{
						return -1;
					}
					
					BM::GUID kMonGuid;
					TBL_DEF_MAP_REGEN_POINT kGenPoint;
					kGenPoint.cBagControlType = ERegenBag_Type_Monster;
					kGenPoint.dwPeriod = 0;
					kGenPoint.iBagControlNo = 0;/*없어도 되지 않을까?*/
					kGenPoint.iMapNo = iMapNo;
					kGenPoint.iMoveRange = pkUnit->GetAbil(AT_MOVE_RANGE);
					kGenPoint.iPointGroup = 1000;
					int const iTunningNo = GetMonsterBagTunningNo(bag_itr->second.iTunningNo_Min, bag_itr->second.iTunningNo_Max);
					if(iTunningNo)
					{
						kGenPoint.iTunningNo = iTunningNo;
					}
					else
					{
						kGenPoint.iTunningNo =  pkGround->GroundTunningNo();
					}

					CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pContMapRegenPoint->begin();
					while(regen_itor != pContMapRegenPoint->end())
					{//지금맵용 리젠포인트. 셋팅.
						if(iMapNo == regen_itor->second.iMapNo)
						if(iMonBagNo == regen_itor->second.iBagControlNo)
						{
							kGenPoint.pt3Pos = regen_itor->second.pt3Pos;
							kGenPoint.cDirection = regen_itor->second.cDirection;
							pkGround->InsertMonster( kGenPoint, kElement.aElement[iIndex], kMonGuid, pkUnit, false, 0, eCreate_HP_Type );
							++iInsertMonsterCount;
							++iIndex;
							if( !kElement.aElement[iIndex] )
							{
								break;
							}
						}
						++regen_itor;
					}
				}
			}
		}
	}

	EPlayContentsType const eType = static_cast<EPlayContentsType>( pkSkillDef->GetAbil(AT_PLAY_CONTENTS_TYPE) );
	if(EPCT_ELGA == eType)
	{
		pkUnit->SetAbil(AT_ELGA_STONE_COUNT, iInsertMonsterCount);
	}

	if(iInsertMonsterCount)
	{
		if(0 < pkSkillDef->GetEffectNo() )
		{
			pkUnit->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit);
		}
	}

	return 0;
}


///////////////////////////////////////////////////////////
//  6000982 PgSummonMonsterRegenPoint_Loop_Function
///////////////////////////////////////////////////////////
int PgSummonMonsterRegenPoint_Loop_Function::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iUseCount = pkUnit->GetAbil(AT_AI_SKILL_USE_COUNT);
	if(iUseCount > 10)
	{
		return -1;
	}
	int const iMonBagControl = pkSkillDef->GetAbil(0==iUseCount ? AT_SKILL_SUMMON_MONBAGCONTROL : AT_SKILL_SUMMON_MONBAGCONTROL_01+(iUseCount-1));
	if( 0==iMonBagControl )
	{
		return -1;
	}

	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pContMapRegenPoint = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );
	g_kTblDataMgr.GetContDef(pContMapRegenPoint );

	if ( !pContMonBagControl || !pContMonBag || !pContMonBagElement )
	{
		return -1;
	}

	ECREATE_HP_TYPE const eCreate_HP_Type = GetCreateHPType(pkSkillDef);
	CONT_DEF_MONSTER_BAG_CONTROL::const_iterator control_itr = pContMonBagControl->find(iMonBagControl);
	if ( control_itr == pContMonBagControl->end() )
	{
		return -1;
	}

	int const iMonBagNo = control_itr->second.aBagElement[pkGround->GetMapLevel()];
	CONT_DEF_MONSTER_BAG::const_iterator bag_itr = pContMonBag->find( iMonBagNo );
	if ( bag_itr == pContMonBag->end() )
	{
		return -1;
	}

	CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator element_itr = pContMonBagElement->find(bag_itr->second.iElementNo);
	if( element_itr == pContMonBagElement->end() )
	{
		return -1;
	}

	int iTunningNo = GetMonsterBagTunningNo(bag_itr->second.iTunningNo_Min, bag_itr->second.iTunningNo_Max);
	if(0==iTunningNo)
	{
		iTunningNo =  pkGround->GroundTunningNo();
	}

	TBL_DEF_MONSTER_BAG_ELEMENTS const &kElement = element_itr->second;
	int const iMapNo = pkGround->GetGroundNo();
	CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pContMapRegenPoint->begin();
	while(regen_itor != pContMapRegenPoint->end())
	{//지금맵용 리젠포인트. 셋팅.
		if( iMapNo == regen_itor->second.iMapNo
		 && iMonBagNo == regen_itor->second.iBagControlNo )
		{
			BM::GUID kMonGuid;
			TBL_DEF_MAP_REGEN_POINT kGenPoint = regen_itor->second;
			kGenPoint.iTunningNo = iTunningNo;

			for(int i=0; i<MAX_SUCCESS_RATE_ARRAY; ++i)
			{
				if(kElement.aElement[i])
				{
					pkGround->InsertMonster( kGenPoint, kElement.aElement[i], kMonGuid, pkUnit, false, 0, eCreate_HP_Type );
				}
			}
		}
		++regen_itor;
	}

	if(0 < pkSkillDef->GetEffectNo() )
	{
		pkUnit->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
	}
	return 0;
}

///////////////////////////////////////////////////////////
//  6000982 PgAddEffectByEntitySkillFunction
///////////////////////////////////////////////////////////
bool PgAddEffectByEntitySkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	pkUnitArray->clear();
	pkGround->GetUnitInRange(pkUnit->GetPos(), pkSkill->GetAbil(AT_ATTACK_RANGE), UT_PLAYER, *pkUnitArray);
	if(pkUnitArray->empty())	//근처에 플레이어가 있으면 발동하자
	{
		return false;
	}

	pkUnitArray->clear();
	pkGround->GetUnitInRange(pkUnit->GetPos(), pkSkill->GetAbil(AT_ATTACK_RANGE), UT_MONSTER, *pkUnitArray);
	if(pkUnitArray->empty())	//근처에 몬스터가 있어야 발동
	{
		return false;
	}

	return true;
}

int PgAddEffectByEntitySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iDist = pkSkill->GetAbil(AT_ATTACK_RANGE);

	pkUnitArray->clear();
	pkGround->GetUnitInRange(pkUnit->GetPos(), iDist, UT_MONSTER, *pkUnitArray);
	if(pkUnitArray->empty())	//근처에 몬스터가 있어야 발동
	{
		return -1;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	int const iMaxTarget = std::max(1, pkSkill->GetAbil(AT_MAX_TARGETNUM));
	int const iEffectNo = pkSkill->GetEffectNo();
	UNIT_PTR_ARRAY kRealArray;
	POINT3 const& rkCasterPos = pkUnit->GetPos();
	std::map<float, CUnit*> kUnitDistList;
	while(pkUnitArray->end() != unit_itor)	//최대 타겟수만큼만
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget && !pkTarget->GetEffect(iEffectNo) && pkTarget->GetID() != pkUnit->GetID())
		{
			kUnitDistList.insert(std::make_pair(POINT3::Distance(rkCasterPos, pkTarget->GetPos()), pkTarget));
		}
		++unit_itor;
	}

	int iCount = 0;
	std::map<float, CUnit*>::iterator dist_itor = kUnitDistList.begin();
	while(kUnitDistList.end() != dist_itor && iCount<iMaxTarget)	//최대 타겟수만큼만
	{
		(*dist_itor).second->AddEffect(iEffectNo, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
		++dist_itor;
		++iCount;
	}		

	return iCount;
}

///////////////////////////////////////////////////////////
// 6089300 PgAddRemoveEffectTargetMonsterFunction
///////////////////////////////////////////////////////////
bool PgAddRemoveEffectTargetMonsterFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	return true;
}
int PgAddRemoveEffectTargetMonsterFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if( !pkGround )
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		return -1;
	}

	if(0 == pkSkill->GetEffectNo() )
	{
		return -1;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
	if( 0 == iMonNo )
	{
		return -1;
	}

	CUnit* pkTarget = NULL;
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		if( (*unit_itor).pkUnit && (*unit_itor).pkUnit->GetAbil(AT_CLASS) == iMonNo )
		{
			pkTarget = (*unit_itor).pkUnit;
			break;
		}

		++unit_itor;
	}

	EUnitType const eUnitType = static_cast<EUnitType>(pkSkill->GetAbil(AT_MON_SKILL_UNIT_TYPE));
	if(NULL==pkTarget)
	{
		pkTarget = pkGround->GetUnitByClassNo( iMonNo, eUnitType );
	}

	if( NULL == pkTarget )
	{
		return -1;
	}

	int const iRemove = pkSkill->GetAbil(AT_MON_ADD_REMOVE);
	int const iReiterate = pkSkill->GetAbil(AT_MON_EFFECT_REITERATE);
	int const iEffectNo = pkSkill->GetEffectNo();

	// 타겟에 걸려있는 이펙트를 찾는다.
	int iFind = 0;
	ContEffectItor kItor;
	CEffect* pkEffect = NULL;
	PgUnitEffectMgr const& rkEffectMgr = pkTarget->GetEffectMgr();
	rkEffectMgr.GetFirstEffect(kItor);
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	while (0 == iFind && (pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
	{
		int const iKey = pkEffect->GetKey();
		for( int i = iEffectNo; i < iEffectNo + iReiterate; ++i )
		{
			if( iKey == i )
			{
				iFind = iKey;
				break;
			}
		}
	}

	if( iFind )
	{
		if( iRemove )
		{
			pkTarget->DeleteEffect(iFind);
			if( iFind != iEffectNo )
			{
				pkTarget->AddEffect(iFind-1, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
			}
		}
		else
		{
			if( iFind < (iEffectNo+iReiterate-1))// 중복 횟수를 넘어가면 안된다.
			{
				pkTarget->DeleteEffect(iFind);
				pkTarget->AddEffect(iFind+1, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
			}

		}
	}
	else
	{
		if( 0 == iRemove )
		{
			pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
		}
	}

	return 1;
}

// SkillNo 가장 먼 더미 위치로 순간이동한다.
bool PgTeleportMostDistantFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	// 더미가 있는지 검사
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	for(int i=1; i<=iNum; ++i)
	{
		BM::vstring kDummy(szMapDummyName);
		kDummy+=i;
		POINT3 const kPos = pkGround->GetNodePosition(MB(kDummy));
		if( kPos == POINT3::NullData() )
		{
			return false;
		}
	}

	return true;
}
int	PgTeleportMostDistantFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}
int	PgTeleportMostDistantFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	POINT3 const& kUnitPos = pkUnit->GetPos();

	int iSelect = 0;
	float fDistance = 0;
	POINT3 kResultPos;
	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	for(int i=1; i<=iNum; ++i)
	{
		BM::vstring kDummy(szMapDummyName);
		kDummy+=i;
		POINT3 const kPos = pkGround->GetNodePosition(MB(kDummy));
		float fDis = POINT3::Distance(kUnitPos, kPos);
		if( fDis > fDistance )
		{
			iSelect = i;
			fDistance = fDis;
			kResultPos = kPos;
		}
	}

	if( 0 != iSelect )
	{
		pkUnit->SetPos(kResultPos);
	}

	return 1;
}

// SkillNo 6000854 발사체, 서버에서 타겟, 발사체 갯수 지정
bool PgTargetLocProjectileFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int iPNum = pkSkill->GetAbil(AT_MON_PROJECTILE_NUM);
	if( 0 >= iPNum )
	{
		return false;
	}

	return true;
}
int PgTargetLocProjectileFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iPNum = pkSkill->GetAbil(AT_MON_PROJECTILE_NUM);
	int const iIsRandom = pkSkill->GetAbil(AT_MON_PROJECTILE_RANDOM);
	if( iIsRandom )
	{
		iPNum = BM::Rand_Range(iPNum, 1);
	}

	CUnit::DequeTarget const& rkDeque = pkUnit->GetTargetList();
	CUnit::DequeTarget::const_iterator target_it = rkDeque.begin();
	int iCount = 0;
	std::vector<BM::GUID> kGuidList;
	while(target_it!=rkDeque.end())
	{
		CUnit const* pkTarget = pkGround->GetUnit((*target_it).kGuid);
		if (pkTarget)
		{ 
			kGuidList.push_back(pkTarget->GetID());
			++iCount;
		}
		++target_it;
	}

	std::random_shuffle(kGuidList.begin(), kGuidList.end());
	iPNum = __min(iPNum, iCount);

	pkPacket->Push(iPNum);
	for(int i=0; i<iPNum; ++i)
	{
		pkPacket->Push(kGuidList.at(i));
	}

	return 1;
}
int PgTargetLocProjectileFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return 1;
}

// SkillNo 6000852 발사체의 갯수, 방향을 결정해서 직선으로 서버에서 발사
bool PgFireProjectileServerFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	// 어빌 디비값 확인
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iPCount = pkSkill->GetAbil(AT_MON_PROJECTILE_NUM);
	if(!iPCount)
	{
		return false;
	}
	int const iAlreadySet = pkSkill->GetAbil(AT_MON_PROJECTILE_ALREADY_SET);
	if(!iAlreadySet)
	{
		return false;
	}
	// AT_MON_PROJECTILE_ANGLE 은 체크하지 않는다.

	return true;
}
int PgFireProjectileServerFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}
	// 발사 거리
	float fDistance = 1000.0f;
	if (pkSkill)
	{
		int iDist = pkSkill->GetAbil(AT_ATTACK_RANGE);
		if (0 < iDist)
		{
			fDistance = static_cast<float>(iDist);
		}
	}
	// 속도
	int const iSpeed = pkUnit->GetSkill()->GetAbil(AT_MOVESPEED);
	// 방향
	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	POINT3 kDir = kTargetPos - pkUnit->GetPos();
	kDir.Normalize();
	pkPacket->Push(kTargetPos.x);
	pkPacket->Push(kTargetPos.y);
	pkPacket->Push(kTargetPos.z);
	// 갯수, 각도
	int const iPCount = pkSkill->GetAbil(AT_MON_PROJECTILE_NUM);
	int const iAngle = pkSkill->GetAbil(AT_MON_PROJECTILE_ANGLE);
	// 첫번째 각도를 기준으로 각도만큼 회전
	int const iFirstAngle = (iPCount - 1) * iAngle / 2;
	// 라디안
	float const fFirstAngle = NI_HALF_PI * static_cast<float>(iFirstAngle) / 90.0f;
	float const fAngle = NI_HALF_PI * static_cast<float>(iAngle) / 90.0f;
	// 회전
	float const fFirstCos = NiCos(fFirstAngle);
	float const fFirstSin = NiSin(fFirstAngle);
	POINT3 kFirst;
	kFirst.x = (kDir.x * fFirstCos) + (kDir.y * fFirstSin);
	kFirst.y = (kDir.x * fFirstSin) + (kDir.y * fFirstCos);
	kFirst.Normalize();
	// 첫번째 발사체 셋팅
	POINT3 kStartPos = pkUnit->GetPos();
	POINT3 kEndPos = kStartPos + (kFirst * fDistance);
	pkUnit->Projectile(0).Begin(kStartPos, kEndPos, iSpeed);

	// 이후
	for(int i=1; i<iPCount; ++i)
	{
		float const fCos = NiCos(-(fAngle*i));
		float const fSin = NiSin(-(fAngle*i));
		POINT3 kRot;
		kRot.x = (kFirst.x * fCos) + (kFirst.y + fSin);
		kRot.y = (kFirst.x + fSin) + (kFirst.y + fCos);
		kRot.Normalize();

		kEndPos = kStartPos + (kRot * fDistance);
		pkUnit->Projectile(i).Begin(kStartPos, kEndPos, iSpeed);
	}

	return 1;
}
int PgFireProjectileServerFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

// SkillNo 6091302 몬스터를 중심으로 X 자 형태의 데미지를 준다.
int PgCrossAttackFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iDist = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	int iWidth = pkSkill->GetAbil(AT_2ND_AREA_PARAM2);
	POINT3 kUnitPos = pkUnit->GetPos();

	// 네 군데의 좌표를 찾는다.
	POINT3BY kPathNormalBy = pkUnit->PathNormal();
	POINT3 kPathNormal(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	kPathNormal.Normalize();

	int iAngle = pkSkill->GetAbil(AT_MON_CROSS_ATTACK_ANGLE);
	int iCount = pkSkill->GetAbil(AT_COUNT);
	if(0>=iCount)
	{
		iCount = 2;//최소 2줄
	}
	//INFO_LOG(BM::LOG_LV5, "\niCount "<<iCount<<" Path X "<<kPathNormal.x<<" Y "<<kPathNormal.y<<" Z "<<kPathNormal.z);
	float fInterAngle = NI_PI/180.0f*static_cast<float>(360.0f/iCount);	//사이 각도
	float fAngle = (0!=iAngle ? NI_PI/180.0f * static_cast<float>(iAngle) : 0);	//시작 각도
	
	NiPoint3 kNiNormal(kPathNormal.x, kPathNormal.y, kPathNormal.z);
	NiPoint3 kNiUnitPos(pkUnit->GetPos().x, pkUnit->GetPos().y, pkUnit->GetPos().z);
	if(fAngle)
	{
		NiQuaternion kStartQuat(-fAngle, NiPoint3::UNIT_Z);
		NiMatrix3 kRot;
		kStartQuat.ToRotation(kRot);

		kNiNormal = kRot * kNiNormal;
	}

	pkUnitArray->clear();
	kNiNormal*=static_cast<float>(iDist);
	kPathNormal.Set(kNiNormal.x, kNiNormal.y, kNiNormal.z);

	NiMatrix3 kRot;
	NiPoint3 kNiTmp;
	for(int i = 0; i < iCount; ++i)
	{
		POINT3 kTemp = pkUnit->GetPos() + kPathNormal;

		//INFO_LOG(BM::LOG_LV5, "kPathNormal X "<<kTemp.x<<" Y "<<kTemp.y<<" fAngle "<<fAngle<<" Inter "<<fInterAngle);
		pkGround->GetUnitInWidthFromLine(pkUnit->GetPos(), kTemp, iWidth, UT_PLAYER, *pkUnitArray);//먼저 찾고

		NiQuaternion kQuat(-fInterAngle*(i+1), NiPoint3::UNIT_Z);
		kQuat.ToRotation(kRot);
		kNiTmp = kRot * kNiNormal;
		kPathNormal.Set(kNiTmp.x, kNiTmp.y, kNiTmp.z);
	}

	pkUnitArray->sort();
	pkUnitArray->unique();

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

// SkillNo 6091103 이동후 트랩을 설치한다(더미 위치에 순서대로)
bool PgMoveAndTrapFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iClass = pkSkill->GetAbil(AT_CLASS);
	UNIT_PTR_ARRAY kUnitArray;
	int iCount = pkGround->GetEntity(pkUnit->GetID(), iClass, kUnitArray);
	if( 9 == iCount )// 풀인 경우
	{
		return false;
	}

	return true;
}
int PgMoveAndTrapFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}
	// 설치되어 있는 트랩을 조사한다.
	// 첫번째 빈자리를 찾는다
	int const iClass = pkSkill->GetAbil(AT_CLASS);
	UNIT_PTR_ARRAY kUnitArray;
	int iCount = pkGround->GetEntity(pkUnit->GetID(), iClass, kUnitArray);
	if( 0 == iCount)// 하나도 없는 경우
	{
		pkPacket->Push(1);
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, 1);
	}
	else if( 9 == iCount )// 풀인 경우
	{
		pkPacket->Push(0);
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, 0);
	}
	else// 빈 자리를 찾아야 한다.
	{
		typedef std::vector<int> ContInt;
		ContInt kList;

		UNIT_PTR_ARRAY::iterator unit_itor = kUnitArray.begin();
		while(kUnitArray.end() != unit_itor)
		{
			CUnit* pkUnitEntity = (*unit_itor).pkUnit;
			if(pkUnitEntity)
			{
				std::wstring strName = pkUnitEntity->Name();
				std::wstring strNum = strName.substr(strName.length()-1, strName.length());
				int iNum = _wtoi(strNum.c_str());
				kList.push_back(iNum);
			}
			++unit_itor;
		}
		std::sort(kList.begin(), kList.end());
		int iFind = 0;
		for( int i = 1; i <= iCount; ++i)
		{
			if( kList.at(i-1) != i )
			{
				iFind = i;
				break;
			}
		}

		if( iFind )
		{
			pkPacket->Push(iFind);
			pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, iFind);
		}
		else
		{
			pkPacket->Push(iCount+1);
			pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01, iCount+1);
		}
	}

	return 1;
}
int PgMoveAndTrapFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	// 이동 한다
	// 트랩(엔터티)을 생성한다
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iPosNum = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01);
	if( 0 == iPosNum)
	{
		return -1;
	}

	BM::vstring kTargetNode(szMapDummyName);
	kTargetNode+=iPosNum;

	POINT3 kTargetPos = pkGround->GetNodePosition(MB(kTargetNode));	

	if (0==kTargetPos.x && 0==kTargetPos.y && 0==kTargetPos.z)	//못찾을 경우
	{
		//return -1;
		kTargetPos = pkUnit->GetPos();	//몾찾으면 제자리에
	}

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}

	pkUnit->SetPos(kTargetPos);// 이동

	//엔티티 만들기
	int const iClass = pkSkill->GetAbil(AT_CLASS);
	SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(1,pkSkill->GetAbil(AT_LEVEL))) );
	kCreateInfo.bUniqueClass = false;

	kCreateInfo.ptPos = kTargetPos;

	CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, kTargetNode);
	if (pkEntity)
	{
		int const iPercent = pkSkill->GetAbil(AT_MAGIC_DMG_PER);
		int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
		pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
	}

	return 1;
}

// SkillNo 6091304 안전영역을 만들고, 그곳을 제외한 부분을 공격한다.
int PgAttackWithoutSafetyZoneFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	// 안전영역을 생성한다. 엔터티 생성, 위치 기억
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}
	// 블록에 인덱스를 줘서 찾는다. 0 ~
	int const iXBlockCount = std::max(1, DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X), 6));
	int const iYBlockCount = std::max(1, DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y), 6));
	int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;
	int const iSafetyZoneCount = DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_SAFETY_ZONE_COUNT), 4);

	int i;
	std::vector<int> kBlock;
	std::vector<int>::iterator itorBlock;
	while( kBlock.size() < static_cast<size_t>(iSafetyZoneCount) )
	{
		int iRand = BM::Rand_Index(iMaxMapBlockCount);
		if( kBlock.empty() )
		{
			kBlock.push_back(iRand);
		}
		else
		{
			itorBlock = std::find(kBlock.begin(), kBlock.end(), iRand);
			if( itorBlock == kBlock.end() )
			{
				kBlock.push_back(iRand);
			}
		}
	}
	
	//for(i = 0; i < iMaxMapBlockCount; ++i)
	//{
	//	kBlock.push_back(i);
	//}

	//std::random_shuffle(kBlock.begin(), kBlock.end());
	for(i = 0; i < iSafetyZoneCount; ++i )
	{
		pkPacket->Push(kBlock.at(i));
		pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01 + i, kBlock.at(i));
	}

	BM::vstring kDummy1(szMapDummyName);
	kDummy1+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1), 1);
	BM::vstring kDummy2(szMapDummyName);
	kDummy2+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM2), 2);
	BM::vstring kDummy3(szMapDummyName);
	kDummy3+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM3), 3);
	BM::vstring kDummy4(szMapDummyName);
	kDummy4+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM4), 4);

	// 더미 위치
	POINT3 kTopLeft = pkGround->GetNodePosition(MB(kDummy1));			// left top
	POINT3 kTopRight = pkGround->GetNodePosition(MB(kDummy2));			// right top
	POINT3 kBottomLeft = pkGround->GetNodePosition(MB(kDummy3));		// left bottom
	POINT3 kBottomRight = pkGround->GetNodePosition(MB(kDummy4));		// right bottom

	// x, y 전체 길이, 블록당 길이
	float const fXTotalLength = kBottomRight.x - kBottomLeft.x;
	float const fYTotalLength = kTopLeft.y - kBottomLeft.y;
	float const fXLength = fXTotalLength / iXBlockCount;
	float const fYLength = fYTotalLength / iYBlockCount;
	
	for( i = 0; i < iSafetyZoneCount; ++i )// 엔터티 생성
	{
		int iY = kBlock.at(i) / iXBlockCount;
		int iX = kBlock.at(i) % iXBlockCount;
		float fY = kBottomLeft.y + (fYLength / 2) + (iY * fYLength);
		float fX = kBottomLeft.x + (fXLength / 2) + (iX * fXLength);

		POINT3 kTargetPos(fX, fY, kBottomLeft.z);

		int const iClass = pkSkill->GetAbil(AT_CLASS);
		SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, std::max(1,pkSkill->GetAbil(AT_LEVEL))) );
		kCreateInfo.bUniqueClass = false;

		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(pkHitShape)
		{
			kTargetPos.z = kHit.worldImpact.z;
		}
		kCreateInfo.ptPos = kTargetPos;

		BM::vstring kName("SafetyZone");
		kName+=(i);
		CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, kName);
		if (pkEntity)
		{
			int const iPercent = pkSkill->GetAbil(AT_MAGIC_DMG_PER);
			int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
			pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
		}
	}

	return 1;
}
int PgAttackWithoutSafetyZoneFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	// 안전영역을 제외한곳에 통상 데미지를 준다.
	// SkillBegin 에서 찾은 인덱스로 다시 위치를 계산한다.
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int const iXBlockCount = std::max(1, DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X), 6));
	int const iYBlockCount = std::max(1, DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y), 6));
	int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;
	int const iSafetyZoneCount = DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_SAFETY_ZONE_COUNT), 4);

	BM::vstring kDummy1(szMapDummyName);
	kDummy1+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1), 1);
	BM::vstring kDummy2(szMapDummyName);
	kDummy2+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM2), 2);
	BM::vstring kDummy3(szMapDummyName);
	kDummy3+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM3), 3);
	BM::vstring kDummy4(szMapDummyName);
	kDummy4+= DefaultValueIsZero(pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM4), 4);

	// 더미 위치
	POINT3 kTopLeft = pkGround->GetNodePosition(MB(kDummy1));			// left top
	POINT3 kTopRight = pkGround->GetNodePosition(MB(kDummy2));			// right top
	POINT3 kBottomLeft = pkGround->GetNodePosition(MB(kDummy3));		// left bottom
	POINT3 kBottomRight = pkGround->GetNodePosition(MB(kDummy4));		// right bottom

	// x, y 전체 길이, 블록당 길이
	float const fXTotalLength = kBottomRight.x - kBottomLeft.x;
	float const fYTotalLength = kTopLeft.y - kBottomLeft.y;
	float const fXLength = fXTotalLength / iXBlockCount;
	float const fYLength = fYTotalLength / iYBlockCount;

	// 안전지대의 유저를 찾는다.
	int iRange = static_cast<int>(fXLength);
	UNIT_PTR_ARRAY kNew;
	for( int i=0; i<iSafetyZoneCount; ++i )
	{
		int iPos = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01 + i);

		int iY = iPos / iXBlockCount;
		int iX = iPos % iXBlockCount;
		float fY = kBottomLeft.y + (fYLength / 2) + (iY * fYLength);
		float fX = kBottomLeft.x + (fXLength / 2) + (iX * fXLength);

		POINT3 kPos(fX, fY, kBottomLeft.z);
		
		pkGround->GetUnitInRange(kPos, iRange, UT_PLAYER, kNew, static_cast<int>(AI_Z_LIMIT));
	}

	// 안전지대 유저를 타겟에서 제외한다.
	UNIT_PTR_ARRAY::iterator unit_itor_safe = kNew.begin();
	while(kNew.end() != unit_itor_safe)
	{
		if((*unit_itor_safe).pkUnit)
		{
			UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
			while(pkUnitArray->end() != unit_itor)
			{
				if((*unit_itor).pkUnit && (*unit_itor).pkUnit->GetID() == (*unit_itor_safe).pkUnit->GetID())
				{
					unit_itor = pkUnitArray->erase(unit_itor);
				}
				else
				{
					++unit_itor;
				}
			}
		}
		++unit_itor_safe;
	}
	
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

// SkillNo 6091370 지정된 더미지역에 엔티티 소환, 어빌에 따라 랜덤하게 소환하지 않음
typedef std::set<int> SET_INT;
bool IsNextZone(int const iIdx, PgGround* pkGround, CSkillDef const* pkSkill)
{
	if(!pkGround || !pkSkill)
	{
		return false;
	}

	BM::vstring kDummy[4];
	kDummy[0] = szMapDummyName;		kDummy[0] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 0);
	kDummy[1] = szMapDummyName;		kDummy[1] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 1);
	kDummy[2] = szMapDummyName;		kDummy[2] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 2);
	kDummy[3] = szMapDummyName;		kDummy[3] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 3);

	// 더미 위치
	POINT3 const kTopLeft	  = pkGround->GetNodePosition(MB(kDummy[0]));		// left top
	POINT3 const kTopRight	  = pkGround->GetNodePosition(MB(kDummy[1]));		// right top
	POINT3 const kBottomLeft  = pkGround->GetNodePosition(MB(kDummy[2]));		// left bottom
	POINT3 const kBottomRight = pkGround->GetNodePosition(MB(kDummy[3]));		// right bottom

	if(POINT3::NullData() == kTopLeft
	|| POINT3::NullData() == kTopRight
	|| POINT3::NullData() == kBottomLeft
	|| POINT3::NullData() == kBottomRight)
	{
		return false;
	}

	return true;
}

bool GetContEmptyZone(CSkillDef const* pkSkill, SET_INT & rkContBlock)
{
	if(!pkSkill)
	{
		return false;
	}

	int const iXBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X));
	int const iYBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y));
	int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;
	int const iEmptyZoneCount = std::min(pkSkill->GetAbil(AT_SKILL_MAP_SAFETY_ZONE_COUNT), iMaxMapBlockCount-1);

	rkContBlock.clear();
	while( rkContBlock.size() < static_cast<size_t>(iEmptyZoneCount) )
	{
		rkContBlock.insert( BM::Rand_Index(iMaxMapBlockCount) );
	}
	return true;
}

POINT3 GetEntityZoneLoc(int const iIdx, PgGround* pkGround, CSkillDef const* pkSkill, int const iEntityZone)
{
	if(!pkGround || !pkSkill)
	{
		return POINT3::NullData();
	}

	BM::vstring kDummy[4];
	kDummy[0] = szMapDummyName;		kDummy[0] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 0);
	kDummy[1] = szMapDummyName;		kDummy[1] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 1);
	kDummy[2] = szMapDummyName;		kDummy[2] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 2);
	kDummy[3] = szMapDummyName;		kDummy[3] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 3);

	// 더미 위치
	POINT3 const kTopLeft	  = pkGround->GetNodePosition(MB(kDummy[0]));		// left top
	POINT3 const kTopRight	  = pkGround->GetNodePosition(MB(kDummy[1]));		// right top
	POINT3 const kBottomLeft  = pkGround->GetNodePosition(MB(kDummy[2]));		// left bottom
	POINT3 const kBottomRight = pkGround->GetNodePosition(MB(kDummy[3]));		// right bottom

	if(POINT3::NullData() == kTopLeft
	|| POINT3::NullData() == kTopRight
	|| POINT3::NullData() == kBottomLeft
	|| POINT3::NullData() == kBottomRight)
	{
		return POINT3::NullData();
	}

	int const iXBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X));
	int const iYBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y));
	int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;

	POINT3 kTmpPosX = kBottomLeft - kBottomRight;
	kTmpPosX.z = 0;
	POINT3 kTmpPosY = kBottomLeft - kTopLeft;
	kTmpPosY.z = 0;
	kTmpPosY.Normalize();

	bool const bPositiveNumX = POINT3(0,0,0) < kTmpPosX;
	bool const bPositiveNumY = POINT3(0,0,0) < kTmpPosY;
	float const fXTotalLength = POINT3::Distance(kBottomLeft, kBottomRight);
	float const fYTotalLength = POINT3::Distance(kBottomLeft, kTopLeft);
	int const iXBlockMax = kTmpPosY.x ? iYBlockCount : iXBlockCount;
	int const iYBlockMax = kTmpPosY.x ? iXBlockCount : iYBlockCount;
	float const fXLength = fXTotalLength / iXBlockCount * (bPositiveNumX ? -1 : 1);
	float const fYLength = fYTotalLength / iYBlockCount * (bPositiveNumY ? -1 : 1);

	for(int y=0; y<iYBlockCount; ++y)// 엔터티 생성
	{
		for(int x=0; x<iXBlockCount; ++x)
		{
			int const idx = (iXBlockCount*y) + x;
			if(iEntityZone == idx)
			{
				int const iX = kTmpPosY.x ? kBottomLeft.y : kBottomLeft.x;
				int const iY = kTmpPosY.x ? kBottomLeft.x : kBottomLeft.y;
				float const fX = iX + (fXLength / 2) + (x * fXLength);
				float const fY = iY + (fYLength / 2) + (y * fYLength);

				POINT3 kTargetPos(fX, fY, kBottomLeft.z);
				if(kTmpPosY.x)
				{
					std::swap(kTargetPos.x, kTargetPos.y);
				}
				NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
				NxRaycastHit kHit;
				NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
				if(pkHitShape)
				{
					kTargetPos.z = kHit.worldImpact.z;
				}
				return kTargetPos;
			}
		}
	}
	return POINT3::NullData();
}

bool CreateEntityZone(int const iIdx, SET_INT const& rkContBlock, PgGround* pkGround, CUnit * pkUnit, CSkillDef const* pkSkill)
{
	if(!pkGround || !pkUnit || !pkSkill)
	{
		return false;
	}

	BM::vstring kDummy[4];
	kDummy[0] = szMapDummyName;		kDummy[0] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 0);
	kDummy[1] = szMapDummyName;		kDummy[1] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 1);
	kDummy[2] = szMapDummyName;		kDummy[2] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 2);
	kDummy[3] = szMapDummyName;		kDummy[3] += pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM1 + (iIdx*4) + 3);

	// 더미 위치
	POINT3 const kTopLeft	  = pkGround->GetNodePosition(MB(kDummy[0]));		// left top
	POINT3 const kTopRight	  = pkGround->GetNodePosition(MB(kDummy[1]));		// right top
	POINT3 const kBottomLeft  = pkGround->GetNodePosition(MB(kDummy[2]));		// left bottom
	POINT3 const kBottomRight = pkGround->GetNodePosition(MB(kDummy[3]));		// right bottom

	if(POINT3::NullData() == kTopLeft
	|| POINT3::NullData() == kTopRight
	|| POINT3::NullData() == kBottomLeft
	|| POINT3::NullData() == kBottomRight)
	{
		return false;
	}

	int const iXBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X));
	int const iYBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y));
	int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;

	POINT3 kTmpPosX = kBottomLeft - kBottomRight;
	kTmpPosX.z = 0;
	POINT3 kTmpPosY = kBottomLeft - kTopLeft;
	kTmpPosY.z = 0;
	kTmpPosY.Normalize();

	bool const bPositiveNumX = POINT3(0,0,0) < kTmpPosX;
	bool const bPositiveNumY = POINT3(0,0,0) < kTmpPosY;
	float const fXTotalLength = POINT3::Distance(kBottomLeft, kBottomRight);
	float const fYTotalLength = POINT3::Distance(kBottomLeft, kTopLeft);
	int const iXBlockMax = kTmpPosY.x ? iYBlockCount : iXBlockCount;
	int const iYBlockMax = kTmpPosY.x ? iXBlockCount : iYBlockCount;
	float const fXLength = fXTotalLength / iXBlockCount * (bPositiveNumX ? -1 : 1);
	float const fYLength = fYTotalLength / iYBlockCount * (bPositiveNumY ? -1 : 1);

	int const iClass = pkSkill->GetAbil(AT_CLASS);
	int iLifeTime = pkSkill->GetAbil(AT_LIFETIME);
	SCreateEntity kCreateInfo( BM::GUID::NullData(), SClassKey(iClass, std::max(1,pkSkill->GetAbil(AT_LEVEL))) );
	for(int y=0; y<iYBlockCount; ++y)// 엔터티 생성
	{
		for(int x=0; x<iXBlockCount; ++x)
		{
			int const idx = (iXBlockCount*y) + x;
			if(rkContBlock.find(idx) != rkContBlock.end())
			{
				continue;
			}

			int const iX = kTmpPosY.x ? kBottomLeft.y : kBottomLeft.x;
			int const iY = kTmpPosY.x ? kBottomLeft.x : kBottomLeft.y;
			float const fX = iX + (fXLength / 2) + (x * fXLength);
			float const fY = iY + (fYLength / 2) + (y * fYLength);

			POINT3 kTargetPos(fX, fY, kBottomLeft.z);
			if(kTmpPosY.x)
			{
				std::swap(kTargetPos.x, kTargetPos.y);
			}
			NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
			NxRaycastHit kHit;
			NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
			if(pkHitShape)
			{
				kTargetPos.z = kHit.worldImpact.z;
			}

			kCreateInfo.kGuid.Generate();
			kCreateInfo.ptPos = kTargetPos;
			kCreateInfo.iLifeTime = iLifeTime;

			BM::vstring kName(L"EntityZone");
			kName += idx;

			CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, kName);
			if (pkEntity)
			{
				int const iPercent = pkSkill->GetAbil(AT_MAGIC_DMG_PER);
				int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
				pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
			}
		}
		iLifeTime += pkSkill->GetAbil(AT_LIFETIME_DELAY);
	}
	return true;
}

bool PgDummyEntityZoneFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iMonNo = pkSkill->GetAbil(AT_CANT_SKILL_ALIVE_UNIT_NO);
	if( iMonNo )
	{
		CUnit* pkTarget = pkGround->GetUnitByClassNo(iMonNo, UT_MONSTER);
		if( pkTarget )
		{
			//엘가3차 공중폭격시 눈알이 소환되어있는경우 스킬이 발동되면 안됨
			return false;
		}
	}

	return true;
}

int PgDummyEntityZoneFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	if (!pkPacket)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int const iZoneType = pkSkill->GetAbil(AT_SKILL_SEND_ZONE_TYPE);

	SET_INT kContEmptyZone;

	int iZoneIdx = 0;
	int iDataIdx = 0;
	do {
		if( GetContEmptyZone(pkSkill, kContEmptyZone) )
		{
			if(0==iZoneType)
			{//안전지대 존 정보
				size_t const iSize = kContEmptyZone.size();
				
				pkPacket->Push( iSize );
				pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx, iSize);		++iDataIdx;

				for(SET_INT::const_iterator c_it=kContEmptyZone.begin(); c_it!=kContEmptyZone.end(); ++c_it)
				{
					pkPacket->Push( GetEntityZoneLoc(iZoneIdx, pkGround, pkSkill, (*c_it)) );
					pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx, (*c_it));		++iDataIdx;
				}
			}
			else
			{//엔티티 생선 존 정보
				int const iXBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_X));
				int const iYBlockCount = std::max(1, pkSkill->GetAbil(AT_SKILL_MAP_BLOCKCOUNT_Y));
				int const iMaxMapBlockCount = iXBlockCount * iYBlockCount;

				size_t const iSize = iMaxMapBlockCount-kContEmptyZone.size();

				pkPacket->Push( iSize );
				pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx, kContEmptyZone.size());		++iDataIdx;
				
				for(int i=0; i<iMaxMapBlockCount; ++i)
				{
					SET_INT::const_iterator c_it = kContEmptyZone.find(i);
					if(c_it==kContEmptyZone.end())
					{
						pkPacket->Push( GetEntityZoneLoc(iZoneIdx, pkGround, pkSkill, i) );
					}
					else
					{
						pkUnit->SetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx, (*c_it));		++iDataIdx;
					}
				}
			}
		}
		else
		{
			break;
		}

		++iZoneIdx;
	} while( IsNextZone(iZoneIdx, pkGround, pkSkill) );
	return 1;
}
int PgDummyEntityZoneFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iZoneIdx = 0;
	int iDataIdx = 0;
	do {
		SET_INT kContEmptyZone;

		int iEmptyZoneCount = pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx);		++iDataIdx;
		for(int i=0; i<iEmptyZoneCount; ++i)
		{
			kContEmptyZone.insert(pkUnit->GetAbil(AT_SKILL_CUSTOM_DATA_01+iDataIdx));	++iDataIdx;
		}
		CreateEntityZone(iZoneIdx, kContEmptyZone, pkGround, pkUnit, pkSkill);

		++iZoneIdx;
	} while( IsNextZone(iZoneIdx, pkGround, pkSkill) );

	return 1;
}

// SkillNo 60913041 유닛 주위에 유저가 있을시 타겟 몬스터를 공격한다.
bool PgCheckUserAndAttackTargetMonster::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
	if( 0 == iMonNo )
	{
		return false;
	}

	CUnit* pkTarget = pkGround->GetUnitByClassNo(iMonNo, UT_MONSTER);
	if(NULL == pkTarget)
	{
		return false;
	}

	POINT3 const& rkPos = pkUnit->GetPos();
	int const iRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	UNIT_PTR_ARRAY kNew;
	pkGround->GetUnitInRange(rkPos, iRange, UT_PLAYER, kNew, static_cast<int>(AI_Z_LIMIT));
	if( kNew.empty() )
	{
		return false;
	}

	pkUnitArray->clear();
	pkUnitArray->Add(pkTarget);

	return true;
}
int	 PgCheckUserAndAttackTargetMonster::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}
int	 PgCheckUserAndAttackTargetMonster::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int const iMonNo = pkSkill->GetAbil(AT_MON_SKILL_MONSTER_NO);
	if( 0 == iMonNo )
	{
		return -1;
	}

	// 다시 검사
	POINT3 const& rkPos = pkUnit->GetPos();
	int const iRange = pkSkill->GetAbil(AT_2ND_AREA_PARAM1);
	UNIT_PTR_ARRAY kNew;
	pkGround->GetUnitInRange(rkPos, iRange, UT_PLAYER, kNew, static_cast<int>(AI_Z_LIMIT));
	if( kNew.empty() )
	{
		return -1;
	}
	
	CUnit* pkTargetMon = pkGround->GetUnitByClassNo(iMonNo, UT_MONSTER);

	if(pkTargetMon)
	{
		int iAddRate = pkSkill->GetAbil(AT_MON_ADD_HP_RATE);
		if( 0 == iAddRate )//  디폴트는 1% 로 하자.
		{
			iAddRate = 100;
		}
		SkillFuncUtil::OnModifyHP(pkTargetMon, NULL, iAddRate, 0, pkUnit, NULL);
	}

	return 1;
}

int	 PgDeleteEffectSkillFuntion::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		return -1;
	}

	if(!pkUnitArray || pkUnitArray->empty())
	{
		return -1;
	}
		
	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (pkTarget == NULL || pkTarget->GetAbil(AT_HP) <= 0)	// 죽었다면 일단 false
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
		}
	}	

	//정리된 목록으로 버프 해제시킴.
	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;

		if(pkTarget)
		{//! 대상에게 이펙트가 걸려있다면 해제
			int iCount = 0;
			while(iCount < EFFECTNUM_MAX)
			{
				int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1+iCount);
				if(iEffectNo)
				{
					pkTarget->DeleteEffect(iEffectNo);
				}
				else
				{
					break;
				}
				++iCount;
			}
		}
		++unit_itor;
	}

	return 1;
}

// SkillNo 6091502 타겟들을 다른 더미위치로 랜덤하게 이동 시킨다.
bool PgRandomTeleportTargettoDummyFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	// 더미 있는지 검사
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iMaxNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	if( 0 >= iMaxNum )
	{
		return false;
	}
	for( int i=1; i<=iMaxNum; ++i )
	{
		BM::vstring kDummy(szMapDummyName);
		kDummy+=i;
		POINT3 kPos = pkGround->GetNodePosition(MB(kDummy));
		if( kPos == POINT3::NullData() )
		{
			return false;
		}
	}
	
	return true;
}
int PgRandomTeleportTargettoDummyFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}
int PgRandomTeleportTargettoDummyFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int const iMaxNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	std::vector<int> kPos;
	for(int i = 1; i <= iMaxNum; ++i)
	{
		kPos.push_back(i);
	}
	std::random_shuffle(kPos.begin(), kPos.end());

	int const iRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
	UNIT_PTR_ARRAY kNew;
	pkGround->GetUnitInRange(pkUnit->GetPos(), iRange, UT_PLAYER, kNew);

	int iCount = 0;
	UNIT_PTR_ARRAY::iterator unit_itor = kNew.begin();
	while(unit_itor != kNew.end() )
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if( pkTarget )
		{
			BM::vstring kDummy(szMapDummyName);
			kDummy+=kPos.at(iCount);
			POINT3 const kPos = pkGround->GetNodePosition(MB(kDummy));
			
			if( 0 < pkSkill->GetEffectNo() )
			{
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_NORMAL;
				kCreate.iEffectNum = pkSkill->GetEffectNo();
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				pkTarget->AddEffect(kCreate);
			}

			pkGround->SendToPosLoc( pkTarget, kPos, MMET_Normal );
			if( iMaxNum > iCount+1 )
			{
				++iCount;
			}
		}
		
		++unit_itor;
	}

	return 1;
}

// SkillNo 6091505 타겟을 더미위치로 이동 시킨다.
bool PgTeleportTargettoDummyFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	// 더미 있는지 검사
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return false;
	}

	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	if( 0 >= iNum )
	{
		return false;
	}

	BM::vstring kDummy(szMapDummyName);
	kDummy+=iNum;
	POINT3 kPos = pkGround->GetNodePosition(MB(kDummy));
	if( kPos == POINT3::NullData() )
	{
		return false;
	}
	
	return true;
}
int PgTeleportTargettoDummyFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}
int PgTeleportTargettoDummyFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	CUnit* pkTarget = pkGround->GetUnit(pkUnit->GetTarget());
	if(!pkTarget )
	{
		return -1;
	}

	int const iNum = pkSkill->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	BM::vstring kDummy(szMapDummyName);
	kDummy+=iNum;
	POINT3 const kPos = pkGround->GetNodePosition(MB(kDummy));
	pkGround->SendToPosLoc( pkTarget, kPos, MMET_Normal );

	return 1;
}

// SkillNo 6000954 타겟유닛 위치로 순간이동 시킨다.
bool PgTeleportTargetFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	if( POINT3::NullData() == kTargetPos )
	{
		return false;
	}

	return true;
}
int PgTeleportTargetFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}
int PgTeleportTargetFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround || !pkUnitArray)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	POINT3 kTargetPos = pkGround->GetUnitPos(pkUnit->GetTarget());
	if( POINT3::NullData() == kTargetPos )
	{
		return -1;
	}

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);
	if(0==iDistance)
	{
		iDistance = 20.f;
	}
	int const iChoose = BM::Rand_Index(2);
	Direction const eDir = (iChoose == 0) ? DIR_LEFT : DIR_RIGHT;

	GetDistanceToPosition( pkGround->GetPathRoot(), kTargetPos, eDir, static_cast<float>(iDistance), kTargetPos );

	if( SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0, EFlags_SetGoalPos|EFlags_CheckFromGround) )
	{
		pkUnit->SetPos(kTargetPos);
	}

	int const iForceSkillNo = pkSkill->GetAbil(AT_MON_ADD_FORCESKILL);
	if(iForceSkillNo)
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkill->GetAbil(AT_FORCESKILL_FLAG));
		pkUnit->GetSkill()->ForceReserve(iForceSkillNo, eFlag);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}

	return 1;
}

// 6000956 폭발 점멸(순간이동, 캐스팅때 이동위치를 클라에 전달)
// PgTeleportTargetDamageFunction의 경우 이동후 공격범위를 지정하기 위해서 
// AT_FIRE_TYPE=561을 EFireType_AttackToGoalPos=64(0x0040)로 설정되어야 함
int PgTeleportTargetDamageFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	POINT3 kTargetPos;
	BM::Stream kForceParam;
	if(S_OK == pArg->Get(ACTARG_FORCE_SKILL_PARAM, kForceParam))
	{
		size_t iSize = 0;
		kForceParam.Pop(iSize);
		kForceParam.Pop(kTargetPos);
	}
	else
	{
		kTargetPos = pkGround->GetUnitPos( pkUnit->GetTarget() );
	}

	if(kTargetPos==POINT3::NullData())
	{
		pkPacket->Push(pkUnit->GetPos());
		pkUnit->GoalPos(pkUnit->GetPos());
	}
	else
	{
		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(pkHitShape)
		{
			kTargetPos.z = kHit.worldImpact.z;
		}

		pkPacket->Push(kTargetPos);
		pkUnit->GoalPos(kTargetPos);
	}

	return 1;
}

int PgTeleportTargetDamageFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		return -1;
	}

	POINT3 kTargetPos = pkUnit->GoalPos();
	if( SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0, EFlags_SetGoalPos|EFlags_CheckFromGround) )
	{
		pkUnit->SetPos(kTargetPos);
	}

	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
}

int PgSetHpSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	if (!pkSkill)
	{
		return -1;
	}

	int iIndex = 0;
	
	if (!pkUnitArray || pkUnitArray->empty())
	{
		return iIndex;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (!pkTarget || !pkUnit->IsTarget(pkTarget, true))
		{
			unit_itor = pkUnitArray->erase(unit_itor);
		}
		else
		{
			++unit_itor;
		}
	}

	int const iEffectNo = pkSkill->GetEffectNo();

	unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)	//정리된 목록으로 데미지주기
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkActionResult = pkResult->GetResult(pkTarget->GetID(), true);	//CS_SkillResult를 안쓰니까 여기서 직접 만들어줘야 함
		if (pkActionResult)
		{
			if(0<pkSkill->GetAbil(AT_HP))
			{
				pkTarget->SetAbil(AT_HP, pkSkill->GetAbil(AT_HP));
			}

			if(0<pkSkill->GetAbil(AT_MP))
			{
				pkTarget->SetAbil(AT_MP, pkSkill->GetAbil(AT_MP), true, true);
			}
			AddDamageEffectAndChangeBlockValue(pkUnit, pkTarget, pkSkill, pkActionResult);
			if ( 0<iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit, EFFECT_TYPE_NORMAL);
				int iSkillindex = 0;
				while(iSkillindex < EFFECTNUM_MAX)		//추가이펙트 걸기
				{
					int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1 + iSkillindex);
					if(0 < iAddEffect)
					{
						pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
					}
					else
					{
						break;
					}
					++iSkillindex;
				}
				pkTarget->SetTarget(pkUnit->GetID()); 
			}
			++iIndex;
		}
		++unit_itor;
	}

	return iIndex;
}

///////////////////////////////////////////////////////////
//  6052630 PgMoveToFarhestTargetFunction
///////////////////////////////////////////////////////////
bool PgMoveToFarhestTargetFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	CUnit::DequeTarget const& rkTargetList = pkUnit->GetTargetList();
	if(1>=static_cast<int>(rkTargetList.size()))	{return false;}

	return true;
}

int	PgMoveToFarhestTargetFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = reinterpret_cast<PgGround*>(pkVoid);
	if (!pkGround )
	{
		return 0;
	}

	POINT3 kNodePos = pkUnit->GetPos();
	POINT3 kUnitPos = pkUnit->GetPos();

	CUnit::DequeTarget const& rkTargetList = pkUnit->GetTargetList();
	float fMaxDist = 0;
	
	NxRaycastHit kHit;
	CUnit::DequeTarget::const_iterator itr = rkTargetList.begin();
	for(;itr != rkTargetList.end(); ++itr)
	{
		CUnit* pkTarget = pkGround->GetUnit(itr->kGuid);
		if(NULL==pkTarget)	{continue;}

		POINT3 kTargetPos = pkTarget->GetPos();
		float fLen = POINT3::Distance(kUnitPos, kTargetPos);
		if(fLen>fMaxDist)
		{
			if(SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, AI_Z_LIMIT, 0, EFlags_SetGoalPos))
			{
				kNodePos = kTargetPos;
				fMaxDist = fLen;
			}
		}
	}

	pkPacket->Push(kNodePos);
	return 1;
}

int PgMoveToFarhestTargetFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return 1;//pkUnit->GetAbil(AT_R_PHY_ATTACK_MAX) ? DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true) : 1;
}

///////////////////////////////////////////////////////////
//  PgSummonEntitiesSkillFunction
///////////////////////////////////////////////////////////
bool PgSummonEntitiesSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return false;
	}

	UNIT_PTR_ARRAY kNewArray;
	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	int const iFound = pkGround->GetEntity(pkUnit->GetID(), iClass, kNewArray);
	if ( 0 < iFound || 0==iClass)	//이미 엔티티가 있다 혹은 엔티티의 클래스 어빌이 없다.
	{
		return false;
	}
	return true;
}

int PgSummonEntitiesSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkillDef)
	{
		return -1;
	}

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	POINT3 kCenter = pkUnit->GetPos();	//기본적으로 캐릭 중심.

	int const iNodeNo = pkSkillDef->GetAbil(AT_SKILL_MAP_DUMMY_NUM);
	if(iNodeNo)
	{
		BM::vstring kTargetNode(szMapDummyName);
		kTargetNode+=iNodeNo;
		POINT3 const& rkNodePos = pkGround->GetNodePosition(szMapDummyName);	
		if(POINT3::NullData()!=rkNodePos)
		{
			kCenter = rkNodePos;
		}
	}

	int const iNum = std::max(1, pkSkillDef->GetAbil(AT_MAX_TARGETNUM));
	int const iClass = pkSkillDef->GetAbil(AT_CLASS);
	int iRange = pkSkillDef->GetAbil(AT_ATTACK_RANGE);
	int iLevel =  std::max(1, pkSkillDef->GetAbil(AT_LEVEL));
	if(0>=iRange)
	{
		iRange = 300;
	}
	int iEntityCount = 0;
	for(int i = 0; i<iNum; ++i, ++iEntityCount)
	{
		SCreateEntity kCreateInfo( BM::GUID::Create(), SClassKey(iClass, iLevel) );
		kCreateInfo.bUniqueClass = false;

		POINT3 kTempPos = kCenter;
		kTempPos.x += BM::Rand_Range(iRange, -iRange);
		kTempPos.y += BM::Rand_Range(iRange, -iRange);

		// 바닥에 밀착 시킨다
		NxRay kRay(NxVec3(kTempPos.x, kTempPos.y, kTempPos.z+20), NxVec3(0, 0, -1.0f));
		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(NULL==pkHitShape)	{continue;}
		kTempPos.z = kHit.worldImpact.z;

		kCreateInfo.ptPos = kTempPos;

		CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("PgSummonEntitiesSkillFunction"));
		if(pkEntity)
		{
			int iPercent = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
			int iMagic = (int)((float)pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * (float)iPercent / ABILITY_RATE_VALUE);
			pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
		}
	}

	return iEntityCount;
}

///////////////////////////////////////////////////////////
//  PgUnitForceFileSkillFunction
///////////////////////////////////////////////////////////
bool PgUnitForceFileSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	return pkUnit->GetAbil(AT_MON_ADD_FORCESKILL);
}

int PgUnitForceFileSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iForceSkillNo = pkUnit->GetAbil(AT_MON_ADD_FORCESKILL);
	if(iForceSkillNo)
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkillDef->GetAbil(AT_FORCESKILL_FLAG));
		pkUnit->GetSkill()->ForceReserve(iForceSkillNo, eFlag);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}
	return 1;
}


///////////////////////////////////////////////////////////
//  PgRollingTargetingSkillFunction
///////////////////////////////////////////////////////////
bool PgRollingTargetingSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	BM::GUID const& rkTargetGuid = pkUnit->GetTarget();
	POINT3 kTargetPos = pkGround->GetUnitPos(rkTargetGuid);

	//갈 수 없는 곳이면 시전하지 말자
	if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0.0f, EFlags_CheckFromGround))
	{
		return false;
	}

	return abs(kTargetPos.z - pkUnit->GetPos().z)<150;	//150높이
}

int PgRollingTargetingSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	// 타겟 전송 수량(몇개가 될지 모르므로 임시로 설정하고 추후 수정)
	size_t const iWRPos = pkPacket->WrPos();
	size_t iWRSize = 0;
	pkPacket->Push( iWRSize );

	{
		CUnit::DequeTarget const& rkList = pkUnit->GetTargetList();
		CUnit::DequeTarget::const_iterator it = rkList.begin();

		int iCount = 0;
		POINT3 const kUnitPos = pkUnit->GetPos();
		POINT3 kStartPos = kUnitPos;
		while(it != rkList.end() && iCount < pkSkillDef->GetAbil(AT_SKILL_GOALPOS_COUNT))
		{
			POINT3 kTargetPos = pkGround->GetUnitPos((*it).kGuid);
			
			pkUnit->SetPos(kStartPos);
			if(POINT3::NullData()!=kTargetPos
			&& SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkUnit, kTargetPos, 20.0f, 0.0f, EFlags_CheckFromGround))
			{
				pkPacket->Push(kTargetPos);
				if(0==iCount)
				{
					pkUnit->GoalPos(kTargetPos);
				}
				else
				{
					pkUnit->NextGoalPos_Add(kTargetPos);
				}
				
				kStartPos = kTargetPos;
				
				++iCount;
			}
			++it;
		}
		pkUnit->SetPos(kUnitPos);
	}

	// 실제 타겟 전송 수량
	iWRSize = pkPacket->WrPos() - iWRPos - sizeof(iWRSize);
	iWRSize /= sizeof(POINT3);
	pkPacket->ModifyData( iWRPos, &iWRSize, sizeof(iWRSize) );

	return iWRSize;
}

int PgRollingTargetingSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	return DefaultMonsterSkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult, true);
}

//엘가 전용 스킬
int PgElgaSummonEyeSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	//int const iSummonEyeNo = pkSkillDef->GetAbil(AT_MON_SKILL_01);
	int const iSummonCrystalNo = pkSkillDef->GetAbil(AT_MON_SKILL_02);

	//눈동자 실제 소환
	UNIT_PTR_ARRAY kUnitArray;
	pkGround->AI_GetSkillTargetList(pkUnit, iSkillNo, kUnitArray, true, pArg);

	/*PgActionResultVector kResult;
	PgSummonMonsterRegenPointFunction kSummonEye;
	kSummonEye.SkillFire(pkUnit, iSummonEyeNo, pArg, &kUnitArray, &kResult);*/

	//수정 소환 캐스팅
	PgSummonMonsterRegenPointChooseFunction kSummonCrystal;
	kSummonCrystal.SkillBegin(pkUnit, iSummonCrystalNo, pArg, iStatus, pkPacket);

	return 0;
}

int PgElgaSummonEyeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iDestoryEyeNo = pkSkillDef->GetAbil(AT_MON_SKILL_03);
	int const iSummonCrystalNo = pkSkillDef->GetAbil(AT_MON_SKILL_02);

	//눈동자 파괴
	CSkillDef const* pkEyeSkillDef = kSkillDefMgr.GetDef(iDestoryEyeNo);
	if ( pkEyeSkillDef )
	{
		UNIT_PTR_ARRAY kUnitArray;
		int const iMon = pkEyeSkillDef->GetAbil(AT_MON_SKILL_MONSTER_NO);
		pkGround->GetUnitByClassNo(iMon, UT_MONSTER, kUnitArray);
	
		UNIT_PTR_ARRAY::iterator unit_itor = kUnitArray.begin();
		while(kUnitArray.end() != unit_itor)
		{
			if(CUnit * pkTarget = (*unit_itor).pkUnit)
			{
				pkTarget->SetAbil(AT_VOLUNTARILY_DIE, 1, true, true);
			}
			++unit_itor;
		}

		PgHealToTargetMonsterFunction kDestoryEye;
		kDestoryEye.SkillFire(pkUnit, iDestoryEyeNo, pArg, &kUnitArray, pkResult);
	}

	//수정 소환
	PgSummonMonsterRegenPointChooseFunction kSummonCrystal;
	kSummonCrystal.SkillFire(pkUnit, iSummonCrystalNo, pArg, pkUnitArray, pkResult);

	return 0;
}

//
int PgElga03_GroggySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iUseCount = pkUnit->GetAbil(AT_AI_SKILL_USE_COUNT);
	if(0==iUseCount)
	{
		pkUnit->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit);
	}
	else if(1==iUseCount)
	{
	}
	else
	{
		PgDefaultDamageAndEffectSkillFunction2 kSkillFun;
		kSkillFun.SkillFire(pkUnit, iSkillNo, pArg, pkUnitArray, pkResult);
	}

	return 0;
}

int PgComboAttackSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iUseCount = pkUnit->GetAbil(AT_AI_SKILL_USE_COUNT);
	if(iUseCount < 10)
	{
		int const iCountSkillNo = pkUnit->GetAbil(AT_MON_SKILL_01 + iUseCount);
		if(iCountSkillNo > 0)
		{
			PgDefaultDamageAndEffectSkillFunction2 kSkillFun;
			kSkillFun.SkillFire(pkUnit, iCountSkillNo, pArg, pkUnitArray, pkResult);
		}
	}

	return 0;
}

bool PgTeamPlaySkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return false;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return false;
	}

	int const iClassNo = pkSkillDef->GetAbil(AT_CLASS);
	if(0 == iClassNo)
	{
		return false;
	}

	EUnitType eUnitType = static_cast<EUnitType>(pkSkillDef->GetAbil(AT_MON_SKILL_UNIT_TYPE));
	if(UT_NONETYPE == eUnitType)
	{
		eUnitType = UT_MONSTER;
	}

	UNIT_PTR_ARRAY kTargetArray;
	pkGround->GetUnitByClassNo(iClassNo, eUnitType, kTargetArray);
	for(UNIT_PTR_ARRAY::iterator target_it=kTargetArray.begin(); kTargetArray.end()!=target_it; ++target_it)
	{
		if(CUnit* pkTarget = (*target_it).pkUnit)
		{
			if( pkTarget->GetAttackDelay()
			|| (EAI_ACTION_BLOWUP==pkTarget->GetAI()->eCurrentAction) )
			{
				return false;
			}
		}
	}
	return true;
}

int PgTeamPlaySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//pkUnitarray : 팀플레이가 공격할 타겟리스트

	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	EUnitType eUnitType = static_cast<EUnitType>(pkSkillDef->GetAbil(AT_MON_SKILL_UNIT_TYPE));
	if(UT_NONETYPE == eUnitType)
	{
		eUnitType = UT_MONSTER;
	}

	VEC_INT kContClassNo;
	int iClassNo = pkSkillDef->GetAbil(AT_CLASS);
	if(iClassNo)
	{
		kContClassNo.push_back(iClassNo);
	}

	for(int i = 0; i < 10; ++i)
	{
		iClassNo = pkSkillDef->GetAbil(AT_CLASS_01 + i);
		if(iClassNo)
		{
			kContClassNo.push_back(iClassNo);
		}
		else
		{
			break;
		}
	}

	if( kContClassNo.empty() )
	{
		return -1;
	}

	UNIT_PTR_ARRAY kTargetArray;
	kTargetArray.Add(pkUnit);
	for(VEC_INT::const_iterator no_it = kContClassNo.begin(); no_it != kContClassNo.end(); ++no_it)
	{
		pkGround->GetUnitByClassNo((*no_it), eUnitType, kTargetArray);
	}

	//
	EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkillDef->GetAbil(AT_FORCESKILL_FLAG));
	BM::Stream kParam;
	kParam.Push(pkUnitArray->size());
	for(UNIT_PTR_ARRAY::iterator target_itor=pkUnitArray->begin(); target_itor!=pkUnitArray->end(); ++target_itor)
	{
		if((*target_itor).pkUnit)
		{
			kParam.Push((*target_itor).pkUnit->GetPos());
		}
		else
		{
			kParam.Push(POINT3::NullData());
		}
	}

	//
	int iCount = 0;
	for(UNIT_PTR_ARRAY::iterator unit_itor=kTargetArray.begin(); (kTargetArray.end()!=unit_itor) && (iCount<10); ++unit_itor, ++iCount)
	{
		if(CUnit* pkTarget = (*unit_itor).pkUnit)
		{
			if(int const iForceSkillNo = pkSkillDef->GetAbil(AT_MON_SKILL_01+iCount))
			{
				pkTarget->GetSkill()->ForceReserve(iForceSkillNo, eFlag, kParam);
				pkTarget->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
				pkTarget->SetTargetList(*pkUnitArray);
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

int PgHydraEarthWrathSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	VEC_INT kContClass;
	for(int i=AT_CLASS_01; i<=AT_CLASS_10; ++i)
	{
		if(int const iClassNo = pkSkillDef->GetAbil(i))
		{
			kContClass.push_back(iClassNo);
		}
		else
		{
			break;
		}
	}

	int const iLevel = std::max(1,pkSkillDef->GetAbil(AT_LEVEL));
	int const iUnitShift = pkSkillDef->GetAbil(AT_DISTANCE);

	POINT3 kDir = pkGround->GetUnitPos(pkUnit->GetTarget()) - pkUnit->GetPos();
	kDir.z = 0;
	kDir.Normalize();

	POINT3 kTargetPos = pkUnit->GetPos();

	pkPacket->Push(SSPT_SKILL_PACKET_POSITION);
	pkPacket->Push(kContClass.size());	//몇개

	int iCount = 0;
	for(VEC_INT::const_iterator c_it = kContClass.begin(); c_it != kContClass.end(); ++c_it)
	{
		++iCount;
		kTargetPos += (kDir * (iUnitShift * iCount));

		NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

		NxRaycastHit kHit;
		NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
		if(pkHitShape)
		{
			kTargetPos.z = kHit.worldImpact.z;
		}

		pkUnit->GetSkill()->CustomData()->Push((*c_it));
		pkUnit->GetSkill()->CustomData()->Push(kTargetPos);
		pkPacket->Push(kTargetPos);
	}
	return 1;
}

int PgHydraEarthWrathSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	void* pkVoid = NULL;
	pArg->Get(ACTARG_GROUND, pkVoid);
	PgGround* pkGround = (PgGround*)pkVoid;
	if (!pkGround)
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const *pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		return -1;
	}

	int const iLevel = std::max(1,pkSkillDef->GetAbil(AT_LEVEL));
	while(pkUnit->GetSkill()->CustomData()->RemainSize() > 0)
	{
		//엔티티 만들기
		SCreateEntity kCreateInfo;
		kCreateInfo.kGuid.Generate();
		kCreateInfo.kClassKey.nLv = iLevel;
		
		bool bSuccess = true;
		bSuccess = bSuccess && pkUnit->GetSkill()->CustomData()->Pop(kCreateInfo.kClassKey.iClass);
		bSuccess = bSuccess && pkUnit->GetSkill()->CustomData()->Pop(kCreateInfo.ptPos);
		if(false == bSuccess)
		{
			break;
		}

		CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, UNI("EarthWarath"));
		if (pkEntity)
		{
			int const iPercent = pkSkillDef->GetAbil(AT_MAGIC_DMG_PER);
			int const iMagic = pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * iPercent / ABILITY_RATE_VALUE;
			pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
		}
	}
	return 1;
}
