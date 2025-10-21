#include "stdafx.h"
#include "PgSkillFunction_Summoner.h"
#include "PgGround.h"
#include "PgAction.h"
#include "Global.h"

extern int GetMinDistanceFromTarget(CUnit* pkUnit);

class PgSummonPatten
{
public:
	enum E_SUMMON_PATTEN_TYPE
	{
		ESMP_NONE				= 0,
		ESMP_FRONT				= 1,
		ESMP_FRONT_ANGLE		= 2,
	};
	static float const DEFAULT_DISTANCE;

	PgSummonPatten(CUnit * pkUnit, PgGround * pkGround)
		: m_fDistance(DEFAULT_DISTANCE)
		, m_iPopCount(0)
		, m_pkUnit(pkUnit)
		, m_pkGround(pkGround)
	{
	}

	POINT3 GetPos(E_SUMMON_PATTEN_TYPE const eType)
	{
		POINT3 kTargetPos;
		if(NULL==m_pkUnit || NULL==m_pkGround)
		{
			return kTargetPos;
		}

		kTargetPos = m_pkUnit->GetPos();
		switch(eType)
		{
		case ESMP_FRONT:
			{
				NxVec3 kVision;
				m_pkGround->GetVisionDirection(m_pkUnit, kVision);

				//소환 위치 : PC의 전방
				NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z), kVision);
				NxRaycastHit kHit;
				NxShape *pkHitShape = m_pkGround->RayCast(kRay, kHit, m_fDistance);
				if(pkHitShape)
				{
					kTargetPos.x = kHit.worldImpact.x;
					kTargetPos.y = kHit.worldImpact.y;
				}
				else
				{
					kVision *= (NxReal)m_fDistance;
					kTargetPos.x = kTargetPos.x+kVision.x;
					kTargetPos.y = kTargetPos.y+kVision.y;
					kTargetPos.z = kTargetPos.z+kVision.z;
				}
			}break;	
		case ESMP_FRONT_ANGLE:
			{
				POINT3 kVision = m_pkUnit->GetDirectionVector( m_pkUnit->FrontDirection() );
				kVision *= m_fDistance;

				////회전
				float const fAngle = 27.f;
				float const fAskew = 0.1f;	//비스듬
				float const fDeg = (NI_PI/180.0f) * (fAngle * ceil(m_iPopCount/2.f)) + fAskew;
				NiQuaternion kStartQuat(fDeg * (m_iPopCount%2 ? 1 : -1), NiPoint3::UNIT_Z);
				NiMatrix3 kRot;
				kStartQuat.ToRotation(kRot);
				
				NiPoint3 kDirVec(kVision.x, kVision.y, 0);
				kDirVec = kRot * kDirVec;

				kTargetPos += POINT3(kDirVec.x, kDirVec.y, 0);
				kTargetPos.z += 10;
			}break;
		}

		++m_iPopCount;
		return kTargetPos;
	}

	void SetDistance(float const kValue)
	{
		m_fDistance = kValue;
	}

private:
	CUnit * m_pkUnit;
	PgGround * m_pkGround;
	int m_iPopCount;
	float m_fDistance;
};
float const PgSummonPatten::DEFAULT_DISTANCE = 10.0f;

///////////////////////////////////////////////////////////
//  PgSummonFunction
///////////////////////////////////////////////////////////
int PgSummonFunction::SkillFire( CUnit *pUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pGround = NULL;
	pArg->Get(ACTARG_GROUND, pGround);

	if( !pGround )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Ground SkillNo = ") << iSkillNo);
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	int iSummonedClassNo = pSkill->GetAbil(AT_CLASS);
	if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUnit))
	{//페시브 스킬
		PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();

		int const iPassiveSkillNo = pSkill->GetAbil(AT_CHILD_SKILL_NUM_01);
		if(0 < iPassiveSkillNo)
		{
			//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
			int const iLearnedSkillNo = pkPlayerSkill->GetLearnedSkill(iPassiveSkillNo);
			if(0 < iLearnedSkillNo)
			{
				if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedSkillNo))
				{
					iSummonedClassNo = pkLearnedSkillDef->GetAbil(AT_CLASS);
				}
			}
		}
	}
	if(0==iSummonedClassNo)
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("SummonedClassNo is NULL : ") << iSkillNo);
		return -1;
	}

	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(pSkill->GetEffectNo());

	SCreateSummoned kCreateInfo;
	kCreateInfo.kClassKey.iClass = iSummonedClassNo;
	kCreateInfo.kClassKey.nLv = pSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = (pSkill->GetAbil(AT_CREATE_UNIQUE_SUMMONED)>0);
	kCreateInfo.sOverlapClass = static_cast<short>(pSkill->GetAbil(AT_CREATE_SUMMONED_MAX_COUNT));
	kCreateInfo.sNeedSupply = static_cast<short>(pSkill->GetAbil(AT_CREATE_SUMMONED_SUPPLY));
	kCreateInfo.iLifeTime = pkEffectDef ? pkEffectDef->GetDurationTime() : 0;

	PgSummonPatten kSummonPatten(pUnit, pGround);
	kSummonPatten.SetDistance(70.f);

	int iRet = 0;
	int const iCount = std::max(pSkill->GetAbil(AT_COUNT), 1);
	for(int i=0; i<iCount; ++i)
	{
		kCreateInfo.kGuid.Generate();			// GUID 생성
		CUnit * pSummoned = pGround->CreateSummoned(pUnit, &kCreateInfo, L"Summoned", SSummonedMapMoveData::NullData(), kSummonPatten.GetPos(PgSummonPatten::ESMP_FRONT_ANGLE));
		if(pSummoned)
		{
			pSummoned->AddEffect(pSkill->GetEffectNo(), 0, pArg, pUnit);
			++iRet;
		}
	}
	return iRet;
}

///////////////////////////////////////////////////////////
//  PgReGainSummonedSkillFunction
///////////////////////////////////////////////////////////
bool PgReGainSummonedSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
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

	if (pkUnit->GetSummonUnitCount() == 0)
	{
		return false;
	}
	return true;
}

bool PgReGainSummonedSkillFunction::DoAction(CUnit * pkSummoned, int const iDieSkillNo, int & iAddHp, int & iAddMp)
{
	if(!pkSummoned)
	{
		return false;
	}

	if( !pkSummoned->GetAbil(AT_SUMMONED_REGAIN_HP_RATE) 
	|| !pkSummoned->GetAbil(AT_SUMMONED_REGAIN_MP_RATE) )
	{
		return false;
	}

	int const iRegainType = pkSummoned->GetAbil(AT_SUMMONED_REGAIN_ENABLE);
	int const iSummonedHp = pkSummoned->GetAbil(AT_HP);
	if ( iRegainType & AUTOHEAL_HP )
	{
		iAddHp += iSummonedHp * (pkSummoned->GetAbil(AT_SUMMONED_REGAIN_HP_RATE)/ABILITY_RATE_VALUE_FLOAT);
	}

	if ( iRegainType & AUTOHEAL_MP )
	{
		iAddMp += iSummonedHp * (pkSummoned->GetAbil(AT_SUMMONED_REGAIN_MP_RATE)/ABILITY_RATE_VALUE_FLOAT);
	}

	PgSkillHelpFunc::SkillOnDie(pkSummoned, iDieSkillNo, true);
	return true;
}

int PgReGainSummonedSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if(!pkUnit)
	{
		return -1;
	}

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

	if (!pkUnitArray)
	{
		return -1;
	}

	int iDieSkillNo =  pkSkillDef->GetAbil(AT_SKILL_ON_DIE);
	if(pkUnit->IsUnitType(UT_PLAYER))
	{
		if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit))
		{//페시브 스킬
			PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();

			int const iPassiveSkillNo = pkSkillDef->GetAbil(AT_CHILD_SKILL_NUM_01);
			if(0 < iPassiveSkillNo)
			{
				//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
				int const iLearnedSkillNo = pkPlayerSkill->GetLearnedSkill(iPassiveSkillNo);
				if(0 < iLearnedSkillNo)
				{
					if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedSkillNo))
					{
						iDieSkillNo = pkLearnedSkillDef->GetAbil(AT_SKILL_ON_DIE);
					}
				}
			}
		}
	}

	int iAddHp = 0;
	int iAddMp = 0;
	int iSummonedCount = 0;
	if(0==pkUnitArray->size())
	{//대상이 없으면 전체를 대상으로 리 게인 시킴
		PgSummoned * pkSummoned = NULL;
		VEC_SUMMONUNIT const& kContSummonUnit = pkUnit->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			pkSummoned = dynamic_cast<PgSummoned*>(pkGround->GetUnit(c_it->kGuid));
			if(pkSummoned && (false == pkSummoned->IsNPC()))
			{
				DoAction(pkSummoned, iDieSkillNo, iAddHp, iAddMp);
				pkSummoned->SetAbil(AT_SKILL_SUMMONED_PARENT_SKILL, iSkillNo, 1, true);
				++iSummonedCount;
			}
		}
	}
	else
	{
		CUnit * pkSummoned = NULL;
		UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
		while(pkUnitArray->end() != unit_itor)
		{
			pkSummoned = (*unit_itor).pkUnit;
			if(pkSummoned && pkSummoned->IsUnitType(UT_SUMMONED) && pkUnit->IsSummonUnit(pkSummoned->GetID()))
			{
				DoAction(pkSummoned, iDieSkillNo, iAddHp, iAddMp);
				pkSummoned->SetAbil(AT_SKILL_SUMMONED_PARENT_SKILL, iSkillNo, 1, true);
				++iSummonedCount;
			}
			++unit_itor;
		}
	}

	SkillFuncUtil::OnModifyHP(pkUnit, NULL, 0, iAddHp, pkUnit, pkGround);
	SkillFuncUtil::OnModifyMP(pkUnit, NULL, 0, iAddMp, pkUnit, pkGround);

	if((iAddHp || iAddMp) && pkSkillDef->GetEffectNo())
	{
		pkUnit->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit);
	}

	return iSummonedCount;
}

///////////////////////////////////////////////////////////
//  PgReCallSummonedSkillFunction
///////////////////////////////////////////////////////////
bool PgReCallSummonedSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
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

	if (pkUnit->GetSummonUnitCount() == 0)
	{
		return false;
	}
	return true;
}

bool PgReCallSummonedSkillFunction::DoAction(PgGround const * pkGround, CSkillDef const* pkSkillDef, CUnit const * pkCaller, CUnit* pkSummoned, SActArg const* pArg)
{
	if(!pkGround || !pkSkillDef || !pkCaller || !pkSummoned)
	{
		return false;
	}

	if(!pkSummoned->IsUnitType(UT_SUMMONED))
	{
		return false;
	}

	if(dynamic_cast<PgSummoned*>(pkSummoned)->IsNPC())
	{
		return false;
	}

	EFollowPattern const ePattern = static_cast<EFollowPattern>(pkSummoned->GetAbil(AT_FOLLOW_PATTERN));
	Direction const eDir = (EFPTN_BACK==ePattern) ? DIR_LEFT : DIR_RIGHT;
	int iReturnZoneRange = pkSummoned->GetAbil(AT_RETURNZONE_RANGE);
	if(0==iReturnZoneRange)
	{
		iReturnZoneRange = AI_MONSTER_MIN_DISTANCE_CHASE_Q;
	}
	/*float const fDistance = BM::Rand_Range(15, iReturnZoneRange);

	POINT3 kTargetPos;
	GetDistanceToPosition( pkGround->GetPathRoot(), pkCaller->GetPos(), eDir, fDistance, kTargetPos );

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}*/

	POINT3BY const& kPathNormalBy = pkSummoned->PathNormal();
	POINT3 Oriented(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	Oriented.Normalize();

	PgCreateSpreadPos kAction(pkCaller->GetPos());
	kAction.AddDir(pkGround->PhysXScene()->GetPhysXScene(), Oriented, iReturnZoneRange);

	POINT3 kTargetPos;
	kAction.PopPos(kTargetPos);

	pkSummoned->SetPos(kTargetPos);
	pkSummoned->GoalPos(POINT3::NullData());
	if(int const iWarpNo = pkSummoned->GetAbil(AT_WARP_SKILL_NO))
	{
		pkSummoned->GetSkill()->ForceReserve(iWarpNo, EFSF_NOW_FIRESKILL_CANCLE);
		pkSummoned->GetAI()->SetEvent(pkCaller->GetID(), EAI_EVENT_FORCE_SKILL);
	}
	else
	{
		pkSummoned->GetAI()->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
	}

	if(pkSkillDef->GetEffectNo())
	{
		pkSummoned->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkCaller);
	}
	return true;
}

int PgReCallSummonedSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	if (!pkUnitArray)
	{
		return -1;
	}

	int iSummonedCount = 0;
	if(0==pkUnitArray->size())
	{//대상이 없으면 전체를 대상에게 적용, 타겟에게만 적용하는것이 옳은게 아닐까?
		VEC_SUMMONUNIT const& kContSummonUnit = pkUnit->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			if(DoAction(pkGround, pkSkillDef, pkUnit, pkGround->GetUnit((*c_it).kGuid), pArg))
			{
				++iSummonedCount;
			}
		}
	}
	else
	{
		UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
		while(pkUnitArray->end() != unit_itor)
		{
			if(DoAction(pkGround, pkSkillDef, pkUnit, (*unit_itor).pkUnit, pArg))
			{
				++iSummonedCount;
			}
			++unit_itor;
		}
	}

	return iSummonedCount;
}

///////////////////////////////////////////////////////////
//  PgEffectTargetSummonedSkillFunction
///////////////////////////////////////////////////////////
bool PgEffectTargetSummonedSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
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

	if (pkSkillDef->GetEffectNo() == 0)
	{
		return false;
	}
	return true;
}

bool PgEffectTargetSummonedSkillFunction::DoAction(SActArg const* pArg, CSkillDef const* pkSkillDef, CUnit * pkCaller, CUnit * pkSummoned)
{
	if(!pArg || !pkSkillDef || !pkCaller || !pkSummoned)
	{
		return false;
	}

	if(!pkSummoned->IsUnitType(UT_SUMMONED))
	{
		return false;
	}

	if(pkSkillDef->GetEffectNo())
	{
		pkCaller->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkCaller);
	}

	for(int i=0; i<EFFECTNUM_MAX; ++i)
	{
		int const iAddEffect = pkSkillDef->GetAbil(AT_EFFECTNUM1+i);
		if(0 < iAddEffect)
		{
			pkSummoned->AddEffect(iAddEffect, 0, pArg, pkCaller);
		}
		else
		{
			break;
		}
	}

	int const iForceSkillNo = pkSkillDef->GetAbil(AT_MON_ADD_FORCESKILL);
	if(iForceSkillNo)
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkSkillDef->GetAbil(AT_FORCESKILL_FLAG));
		pkSummoned->GetSkill()->ForceReserve(iForceSkillNo,eFlag);
		pkSummoned->GetAI()->SetEvent(pkCaller->GetID(), EAI_EVENT_FORCE_SKILL);
	}
	pkSummoned->SetAbil(AT_SKILL_SUMMONED_PARENT_SKILL, pkSkillDef->No(), 1, true);
	return true;
}

int PgEffectTargetSummonedSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	if (!pkUnitArray)
	{
		return -1;
	}

	int iSummonedCount = 0;
	if(0==pkUnitArray->size())
	{//대상이 없으면 전체를 대상에게 적용, 타겟에게만 적용하는것이 옳은게 아닐까?
		VEC_SUMMONUNIT const& kContSummonUnit = pkUnit->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			if(DoAction(pArg, pkSkillDef, pkUnit, pkGround->GetUnit((*c_it).kGuid)))
			{
				++iSummonedCount;
			}
		}
	}
	else
	{
		UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
		while(pkUnitArray->end() != unit_itor)
		{
			if(DoAction(pArg, pkSkillDef, pkUnit, (*unit_itor).pkUnit))
			{
				++iSummonedCount;
			}
			++unit_itor;
		}
	}

	return iSummonedCount;
}

///////////////////////////////////////////////////////////
//  PgAttackSummonedSkillFunction
///////////////////////////////////////////////////////////
//AT_MON_SKILL_SECOND_01			= 3310,	// 집중 딜링 할 몬스터 찾는 스킬
//AT_EFFECTNUM1						= 2051,	// 딜링 당할 우선순위를 높이는 이펙트
//AT_EFFECTNUM2						= 2052,	// 우선순위가 높은 집중 딜링 몬스터를 찾는 이펙트를 검(AT_PROVOKE_EFFECT_NO이 값이 세팅되어야 함)
bool PgAttackSummonedSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	return true;
}

bool PgAttackSummonedSkillFunction::DoAction(CUnit * pkCaller, CUnit * pkSummoned, CUnit * pkMonster, SActArg const* pArg, CSkillDef const* pkSkillDef)
{
	if(!pkCaller || !pkSummoned || !pkMonster || !pArg || !pkSkillDef)
	{
		return false;
	}

	if(!pkSummoned->IsUnitType(UT_SUMMONED))
	{
		return false;
	}

	if(dynamic_cast<PgSummoned*>(pkSummoned)->IsNPC())
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

	
	//3. 서버에서 M을 캐스터로 S에게 우선순위가 높은 M을 찾는 이펙트를 검
	if(int const iEffectNo = pkSkillDef->GetAbil(AT_EFFECTNUM2))
	{
		pkSummoned->AddEffect(iEffectNo, 0, pArg, pkMonster);
	}

	//4. 서버에서 M을 쫓아가도록 S의 AI를 변경함
	pkSummoned->ClearTargetList();
	pkSummoned->SetTarget(pkMonster->GetID());

	float const fDistQ = GetDistanceQ(pkSummoned->GetPos(), pkMonster->GetPos());
	if(fDistQ > AI_GOALPOS_ARRIVE_DISTANCE_Q)
	{
		int const iMinDistance = GetMinDistanceFromTarget(pkSummoned);
		POINT3 ptTargetPos;
		if(!pkGround->GetAroundVector(pkSummoned, pkMonster->GetID(), (float)iMinDistance, true, ptTargetPos))
		{
			ptTargetPos = pkMonster->GetPos();
		}
	
		if (!SetValidGoalPos(pkGround->GetPhysXScene()->GetPhysXScene(), pkSummoned, ptTargetPos, 20.0f, (float)iMinDistance, EFlags_MinDistance|EFlags_CheckFromGround|EFlags_SetGoalPos))
		{
			pkSummoned->SetPos(ptTargetPos);
		}
		else
		{
			pkSummoned->GetAI()->SetEvent(pkMonster->GetID(), EAI_EVENT_CHASE, EAI_EVENT_PARAM_CHASE_UNCONDITIONALLY|EAI_EVENT_PARAM_CHASE_NO_DAMAGEACTION|EAI_EVENT_PARAM_CHASE_GOAL_POS);
		}
	}
	return true;
}

int PgAttackSummonedSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	if (!pkUnitArray)
	{
		return -1;
	}

	/*
	1. 서버에서 집중 딜링 당할 몬스터(M)를 선택한다
	2. 서버에서 M에게 본인의 우선순위를 높이는 이펙트를 건다
	3. 서버에서 M을 캐스터로 S에게 우선순위가 높은 M을 찾는 이펙트를 검
	4. 서버에서 M을 쫓아가도록 S의 AI를 변경함
	*/	

	//1. 서버에서 집중 딜링 당할 몬스터(M)를 선택한다
	CUnit* pkMonster = NULL;
	UNIT_PTR_ARRAY kMonsterArray;
	if(pkGround->AI_GetSkillTargetList(pkUnit, pkSkillDef->GetAbil(AT_MON_SKILL_SECOND_01), kMonsterArray, true, pArg))
	{
		UNIT_PTR_ARRAY::const_iterator unit_itor = kMonsterArray.begin();
		if(kMonsterArray.end() != unit_itor)
		{
			pkMonster = (*unit_itor).pkUnit;
		}
	}

	if(!pkMonster)
	{
		return -1;
	}

	//2. 서버에서 M에게 본인의 우선순위를 높이는 이펙트를 건다
	if(int const iMonEffectNo = pkSkillDef->GetAbil(AT_EFFECTNUM1))
	{
		pkMonster->AddEffect(iMonEffectNo, 0, pArg, pkUnit);
	}

	//3. 서버에서 M을 캐스터로 S에게 우선순위가 높은 M을 찾는 이펙트를 검
	//4. 서버에서 M을 쫓아가도록 S의 AI를 변경함

	int iSummonedCount = 0;
	if(0==pkUnitArray->size())
	{//대상이 없으면 전체를 대상에게 적용, 타겟에게만 적용하는것이 옳은게 아닐까?
		VEC_SUMMONUNIT const& kContSummonUnit = pkUnit->GetSummonUnit();
		for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
		{
			if(DoAction(pkUnit, pkGround->GetUnit((*c_it).kGuid), pkMonster, pArg, pkSkillDef))
			{
				++iSummonedCount;
			}
		}
	}
	else
	{
		UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
		while(pkUnitArray->end() != unit_itor)
		{
			if(DoAction(pkUnit, (*unit_itor).pkUnit, pkMonster, pArg, pkSkillDef))
			{
				++iSummonedCount;
			}
			++unit_itor;
		}
	}

	return iSummonedCount;
}
