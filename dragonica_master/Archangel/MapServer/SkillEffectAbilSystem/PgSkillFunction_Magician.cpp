#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Magician.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "Global.h"
#include "PgGround.h"
#include "PgEffectFunction.h"

///////////////////////////////////////////////////////////
//  PgTransformationSkillFunction
///////////////////////////////////////////////////////////
int PgTransformationSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			//int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
			//int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
			//if (iDecHitRate < iRandValue)
			//{
			//	// 타격 실패 (Missed)
			//	pkAResult->SetMissed(true);
			//	++unit_itor;
			//	continue;
			//}

			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					int const iEffectNo = pkSkill->GetEffectNo();
					if(!pkTarget->GetEffect(iEffectNo, true))
					{
						if(iEffectNo)
						{
							pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
						}
					}
				}
				else
				{
					pkAResult->SetMissed(true);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgDetectionSkillFunction
///////////////////////////////////////////////////////////
int PgDetectionSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if(pkUnitArray->empty())
	{
		return 1;
	}
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		return 1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	int const iRange = pkSkill->GetAbil(AT_ATTACK_RANGE);
	if(!iRange)
	{
		INFO_LOG(BM::LOG_LV8, __FL__<<L"Skill["<<iSkillNo<<L"]AT_ATTACK_RANGE : "<<iRange);
	}

	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);

	if(!iEffectPercent)
	{
		INFO_LOG(BM::LOG_LV8, __FL__<<L"Skill["<<iSkillNo<<L"] AT_PERCENTAGE : "<<iRange);
	}

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_ENEMY, iRange, static_cast<int>(AI_Z_LIMIT));

	int const iMaxTarget = pkSkill->GetAbil(AT_MAX_TARGETNUM);
	int iNowTarget = 0;

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{			
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget)
		{
			int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
			int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
			if (iDecHitRate < iRandValue)
			{
				// 타격 실패 (Missed)
				++target_itor;
				continue;
			}

			int iRandValue2 = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;

			if(iRandValue2 <= iEffectPercent && (false == (*target_itor).bReference))
			{
				pkTarget->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit);
				int const iNewEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
				if(iNewEffectNo)
				{
					pkTarget->AddEffect(iNewEffectNo, 0, pArg, pkUnit);
				}
			}
		}

		if(iMaxTarget <= iNowTarget)
		{
			break;
		}
		++iNowTarget;
		++target_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgHealisSelfSkillFunction
///////////////////////////////////////////////////////////
int PgHealisSelfSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if(pkUnitArray->empty())
	{
		return 1;
	}
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		return 1;
	}

	if ( true == pkUnit->IsAlive() )
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

		int const iAdd = pkSkill->GetAbil(AT_MAGIC_ATTACK);
		int const iConvertHealRate = pkSkill->GetAbil(AT_I_MAGIC_ATTACK_ADD_RATE);

		int const iHP = pkUnit->GetAbil(AT_HP);
		int const iMagicAttack = pkUnit->GetAbil(AT_C_MAGIC_ATTACK);
		__int64 i64ConvertedHealAmount = 0;
		{
			i64ConvertedHealAmount = iMagicAttack;
			i64ConvertedHealAmount *= iConvertHealRate;
			i64ConvertedHealAmount /= ABILITY_RATE_VALUE;
			i64ConvertedHealAmount += iAdd;
		}
		int const iResult = static_cast<int>(i64ConvertedHealAmount);

		SkillFuncUtil::OnModifyHP(pkUnit, NULL, 0, iResult, pkUnit, pkGround);
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgWideHealSkillFunction
///////////////////////////////////////////////////////////
int PgWideHealSkillFunction::SkillFire(CUnit* pkCaster, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{	
	if(pkUnitArray->empty())
	{
		return 1;
	}
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	
	__int64 i64Result = 0;
	{// 시전자의 최대 HP를 얻어와
		int const iAddHP = pkSkill->GetAbil(AT_HP);
		int const iMaxHPRate = pkSkill->GetAbil(AT_R_MAX_HP);
		
		i64Result = pkCaster->GetAbil(AT_C_MAX_HP);
		i64Result*=iMaxHPRate;
		i64Result/=ABILITY_RATE_VALUE;
		i64Result+=iAddHP;
	}
	
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = unit_itor->pkUnit;
		if ( true == pkTarget->IsAlive() )
		{// 살아있을 때만 Heal을 해야 한다.
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				SkillFuncUtil::OnModifyHP(pkTarget, NULL, 0, static_cast<int>(i64Result), pkCaster, pkGround);
			}
		}
		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgCureSkillFunction
///////////////////////////////////////////////////////////
int PgCureSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{	
	// 큐어 공식이 바뀌면 PgCureEffectFunction 이 클래스 함수도 같이 수정해주어야 한다.
	// 기존 큐어를 고치게 되면 기존 국가들 때문에 Merge가 힘들어진다. 새로 추가 한다.
	// 기존 모든 국가가 수정될 경우 다시 작업

	if(pkUnitArray->empty())
	{
		return 1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	//지울수 있는 Effect 개수
	int const iDeleteCount = pkSkill->GetAbil(AT_COUNT);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				PgUnitEffectMgr const& rkEffectMgr = pkTarget->GetEffectMgr();

				int iCount = 0;
				ContEffectItor kItor;
				rkEffectMgr.GetFirstEffect(kItor);
				CEffect* pkEffect = NULL;
				while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
				{
					//지울 수 잇는 최대치를 넘어가면 중단
					if(iDeleteCount <= iCount)
					{
						break;
					}
					if(pkEffect)
					{
						int const iBuffType = pkEffect->GetAbil(AT_TYPE);
						if(EFFECT_TYPE_CURSED == iBuffType)
						{
							if(0 == pkEffect->GetAbil(AT_CURE_NOT_DELETE))
							{
								pkTarget->DeleteEffect(pkEffect->GetEffectNo());
								++iCount;
							}
						}
					}
				}
			}

			++unit_itor;
		}
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgQuagmireSkillFunction - 콰그마이어
///////////////////////////////////////////////////////////
int PgQuagmireSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	int const iDistance = pkSkill->GetAbil(AT_DISTANCE);

	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = true;
	kCreateInfo.kGuid.Generate();

	POINT3 ptEntityPos = pkUnit->GetPos();
	GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), pkUnit->FrontDirection(), static_cast<float>(iDistance), ptEntityPos);
	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(ptEntityPos.x, ptEntityPos.y, ptEntityPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		ptEntityPos.z = kHit.worldImpact.z;
	}
	kCreateInfo.ptPos = ptEntityPos;
	
	CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, _T("Quagmire"));

	return 1;
}

///////////////////////////////////////////////////////////
//  PgQuagmireSetSkillFunction - 콰그마이어 설치
///////////////////////////////////////////////////////////
int PgQuagmireSetSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	
	bool bSkillDamage = IsHaveSkillDamage(pkSkill);
	CS_GetSkillResult105300501(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		// 참고용 유닛은 리스트에서 지나친다.
		if(true == (*unit_itor).bReference)
		{
			++unit_itor;
			continue;
		}

		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);

		int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
		int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
		if (iDecHitRate < iRandValue)
		{
			// 타격 실패 (Missed)
			if(pkTarget != pkUnit)
			{
				pkAResult->SetMissed(true);
				++unit_itor;
				continue;
			}
		}

		if(pkAResult && !pkAResult->GetInvalid())
		{			
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

			if(iRandValue <= iEffectPercent)
			{
				for(int i = 0; i < EFFECTNUM_MAX; ++i)
				{
					int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1 + i);
					if(iEffectNo)
					{
						pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
					}
					else
					{
						break;
					}
				}
			}

			CUnit* pkUnit2 = pkUnit;
			//Entity일 경우 부모의 Unit을 찾아서 Unit2에 세팅해야 한다.
			BM::GUID const& kParentGUID = pkUnit->Caller();
			if(BM::GUID::IsNotNull(kParentGUID) && pkUnit->GetID() != kParentGUID)
			{
				PgGround* pkGround = NULL;
				if(pArg)
				{
					pArg->Get(ACTARG_GROUND, pkGround);
				}

				if(pkGround)
				{
					CUnit* pkParentUnit = pkGround->GetUnit(kParentGUID);
					if(pkParentUnit)
					{
						pkUnit2 = pkParentUnit;
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"kGround is NULL");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
				}						
			}

			//캐스터가 PgPlayer라면
			if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit2))
			{
				int iSkillindex = 0;
				while(CHILD_SKILL_MAX > iSkillindex)
				{
					//영향 받는 베이스 스킬
					int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
					if(0 < iChildBaseSkillNo)
					{
						if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
						{
							//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
							int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
							if(0 < iLearnedChildSkillNo)
							{
								if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
								{
									// 추가 버프를 거는 경우
									int iChildEffectNo = pkLearnedSkillDef->GetEffectNo();
									if(0 < iChildEffectNo)
									{
										if(iRandValue <= iEffectPercent)
										{
											pkTarget->AddEffect(iChildEffectNo, 0, pArg, pkUnit);
										}
									}
								}
							}
						}
					}
					else
					{
						break;
					}
					++iSkillindex;
				}			
			}
		}

		if(bSkillDamage)
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgResurrectionSkillFunction
///////////////////////////////////////////////////////////
int PgResurrectionSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int const iHPRate = pkSkill->GetAbil(AT_R_MAX_HP);
	int const iMPRate = pkSkill->GetAbil(AT_R_MAX_MP);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			if ( SYNC_TYPE_SEND_ADD & pkTarget->GetSyncType() )
			{
				// 싱크 AddUnit을 보내는 놈만 살려야 한다.

				PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
				if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
				{
					if(pkTarget->GetAbil(AT_HP) <= 0 && !pkTarget->IsTarget(pkUnit, false, ESTARGET_ENEMY))
					{
						int const iHP = (int)(pkTarget->GetAbil(AT_C_MAX_HP) * (float)iHPRate / ABILITY_RATE_VALUE);
						// MP는 현재값보다, 새로운 값이 더 클때만 올려 준다.
						int iMP = (int)(pkTarget->GetAbil(AT_C_MAX_MP) * (float)iMPRate / ABILITY_RATE_VALUE);
						int const iCurMP = pkTarget->GetAbil(AT_MP);
						if(iCurMP > iMP)						
						{					
							iMP = iCurMP;
						}
						OnSetAbil(pkTarget, AT_REVIVED_BY_OTHER, 1);
						OnSetAbil(pkTarget, AT_HP_RESERVED, iHP);
						OnSetAbil(pkTarget, AT_MP_RESERVED, iMP);

						// 살려주는 (Me -> Target)
						pkTarget->ClearTargetList(); // 일단 다 지워
						pkTarget->SetTarget(pkUnit->GetID()); // 살려준 놈만 설정(로그 출력 용)
						//if(pkGround)
						//{// SubPlayer를 소유하고 있는 클래스일경우
						//	int const iClass = pkTarget->GetAbil(AT_CLASS);
						//	if( IsClass_OwnSubPlayer(iClass) )
						//	{
						//		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkTarget);
						//		if(pkPlayer)
						//		{
						//			BM::GUID kGuid = pkPlayer->SubPlayerID();
						//			if( kGuid.IsNull() )
						//			{// SubPlayer를 생성함
						//				kGuid.Generate();
						//			}
						//			pkGround->CreateSubPlayer(pkPlayer, kGuid);
						//		}
						//	}
						//}
					}
				}
			}
			else
			{
				// PvP에서 옵져버가 된 경우도 여기에 걸린다.
				pkResult->Remove( pkTarget->GetID() );
				unit_itor = pkUnitArray->erase( unit_itor );
				continue;
			}
		}
		++unit_itor;
	}

	return 1;
}


///////////////////////////////////////////////////////////
//  PgBlizzardSkillFunction - 블리자드
///////////////////////////////////////////////////////////
int PgBlizzardSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	int const iDistance = pkSkill->GetAbil(AT_DISTANCE);

	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = true;
	kCreateInfo.kGuid.Generate();

	POINT3 ptEntityPos = pkUnit->GetPos();
	GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), pkUnit->FrontDirection(), static_cast<float>(iDistance), ptEntityPos);
	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(ptEntityPos.x, ptEntityPos.y, ptEntityPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		ptEntityPos.z = kHit.worldImpact.z;
	}
	kCreateInfo.ptPos = ptEntityPos;
	
	CUnit* pkEntity = pkGround->CreateEntity(pkUnit, &kCreateInfo, _T("Blizzard"));
	if(pkEntity)
	{
		int iPercent = pkSkill->GetAbil(AT_MAGIC_DMG_PER);
		int iMagic = (int)((float)pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * (float)iPercent / ABILITY_RATE_VALUE);
		pkEntity->SetAbil(AT_ATTR_ATTACK, iMagic);
	}
	
	return 1;
}

///////////////////////////////////////////////////////////
//  PgChainLightingSkillFunction - 체인라이트닝
///////////////////////////////////////////////////////////
int PgChainLightingSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		return -1;
	}

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int const iEffectNo = pkSkill->GetEffectNo();	
	CEffect const* pkEffect = pkUnit->GetEffect(iEffectNo, true);

	if(NULL == pkEffect)
	{
		//Effect가 없으면 추가해준다.
		//공격스킬이지만 계속적으로 공격하는 스킬이라 MP를 소모시키는 Effect가 필요함.
		pkUnit->AddEffect(iEffectNo, iSkillNo, pArg, pkUnit);
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}


///////////////////////////////////////////////////////////
//  PgHPRestoreSkillFunction
///////////////////////////////////////////////////////////
int PgHPRestoreSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int const iAddHPValue = (int)((float)pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * ((float)pkSkill->GetAbil(AT_R_MAGIC_ATTACK_MAX) / ABILITY_RATE_VALUE));

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, iAddHPValue, pArg, pkUnit);
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgMPTransitionSkillFunction
///////////////////////////////////////////////////////////
int PgMPTransitionSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int const iCaster_MaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
	int const iNeedMP = pkSkill->GetAbil(AT_NEED_MAX_R_MP);
	int iPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iPercent)
	{
		iPercent = ABILITY_RATE_VALUE;
	}
	int iValue = static_cast<int>(static_cast<float>(iCaster_MaxMP) * (static_cast<float>(iNeedMP) / ABILITY_RATE_VALUE_FLOAT));

	if(ABILITY_RATE_VALUE != iPercent)
	{
		iValue = static_cast<int>(static_cast<float>(iValue) * (static_cast<float>(iPercent) / ABILITY_RATE_VALUE_FLOAT));
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				int const iMaxMP = pkTarget->GetAbil(AT_C_MAX_MP);
				int const iNowMP = pkTarget->GetAbil(AT_MP);
				if(iMaxMP > iNowMP)
				{
					OnSetAbil(pkTarget, AT_MP, __min(iMaxMP, iNowMP + iValue));
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgResourceConverterSkillFunction
///////////////////////////////////////////////////////////
int PgResourceConverterSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int const iNeedHP = pkSkill->GetAbil(AT_NEED_HP);
	int const iValue = (int)((float)iNeedHP * ((float)pkSkill->GetAbil(AT_PERCENTAGE) / ABILITY_RATE_VALUE));

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				int const iMaxMP = pkTarget->GetAbil(AT_C_MAX_MP);
				int const iNowMP = pkTarget->GetAbil(AT_MP);
				if(iMaxMP > iNowMP)
				{
					OnSetAbil(pkTarget, AT_MP, __min(iMaxMP, iNowMP + iValue));
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgSlowHealSkillFunction
///////////////////////////////////////////////////////////

int CalcSlowHealValue(int const iHealPercent, CUnit const* pkUnit)
{
	if(pkUnit)
	{
		//새로운 공식 : MAXHP의 % 회복
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int iAddHPValue = static_cast<int>(static_cast<float>(iMaxHP) * (static_cast<float>(iHealPercent) / ABILITY_RATE_VALUE_FLOAT));

		return iAddHPValue;
	}

	return 0;
}

int PgSlowHealSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	//공식 : 현재 마법공격력이 100 이고 30%만큼에 해당되는 값을 틱당 10%씩만 회복하고 싶다고 하면 (100 * 0.3) * 0.1 = 3;
	//공식 : (현재 마법 공격력의 일정 % ) * % = 틱당 회복되는 양
	//int iAddHPValue = (int)((float)pkUnit->GetAbil(AT_C_MAGIC_ATTACK) * ((float)pkSkill->GetAbil(AT_R_MAGIC_ATTACK_MAX) / ABILITY_RATE_VALUE));
	//iAddHPValue = (int)((float)iAddHPValue * ((float)pkSkill->GetAbil(AT_PERCENTAGE) / ABILITY_RATE_VALUE));

	int iAddHPValue = CalcSlowHealValue(pkSkill->GetAbil(AT_R_MAX_HP), pkUnit);
	
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, iAddHPValue, pArg, pkUnit);
			}
		}
		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgBarrierSkillFunction
///////////////////////////////////////////////////////////
int PgBarrierSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

			if(iEffectNo)
			{
				if(iRandValue <= iEffectPercent)
				{
					if(pkTarget->GetEffect(iEffectNo, true))
					{
						pkTarget->DeleteEffect(iEffectNo);
					}

					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}			
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgSnowBluesFunction
///////////////////////////////////////////////////////////
int PgSnowBluesFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);

	Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

	// 엔티티를 만들어야 한다
	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = false;
	kCreateInfo.kGuid.Generate();
	kCreateInfo.ptPos = pkUnit->GetPos();
	::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), eFrontDir, static_cast<float>(-iDistance), kCreateInfo.ptPos );

	// 바닥에 밀착 시킨다
	NxRaycastHit kHit;
	NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
	NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		kCreateInfo.ptPos.z = kHit.worldImpact.z;
	}

	pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("SnowBlues") );
	return 1;
}

///////////////////////////////////////////////////////////
//  PgMagmaPistonFunction
///////////////////////////////////////////////////////////
int PgMagmaPistonFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);
	if ( 0 == iDistance )
	{
		iDistance = 50;
	}

	float fDistance = static_cast<float>(iDistance);

	// 위치 설정
	POINT3BY kPathNormalBy = pkUnit->PathNormal();
	POINT3 kUp(kPathNormalBy.x, kPathNormalBy.y, kPathNormalBy.z);
	kUp.Normalize();
	POINT3 kDown(-kUp.x,-kUp.y,-kUp.z);
	POINT3 kRight = kUp.Cross(POINT3(0,0,1));
	POINT3 kLeft(-kRight.x,-kRight.y,-kRight.z);

	//POINT3 kStartPos = pkUnit->GetPos() + (kDown * fDistance);
	POINT3 kStartPos = pkUnit->GetPos();
	POINT3 kTemp;
	std::vector<POINT3> vecPos;
	kTemp = kStartPos + (kRight * fDistance);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kLeft * fDistance);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kUp * fDistance * 2);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kDown * fDistance * 2);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kUp * fDistance * 2) + (kRight * fDistance * 2);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kUp * fDistance * 2) + (kLeft * fDistance * 2);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kDown * fDistance * 2) + (kRight * fDistance * 2);
	vecPos.push_back(kTemp);

	kTemp = kStartPos + (kDown * fDistance * 2) + (kLeft * fDistance * 2);
	vecPos.push_back(kTemp);


	int iCreateSize = 8;
	for( int i=0; i<iCreateSize; ++i)
	{
		Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

		// 엔티티를 만들어야 한다
		SCreateEntity kCreateInfo;
		kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
		kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
		kCreateInfo.bUniqueClass = false;
		kCreateInfo.kGuid.Generate();
		kCreateInfo.ptPos = vecPos.at(i);

		// 바닥에 밀착 시킨다
		NxRaycastHit kHit;
		NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
		NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
		if(pkHitShape)
		{
			kCreateInfo.ptPos.z = kHit.worldImpact.z;
		}
		else
		{
			continue;
		}

		CUnit* pkEntity = pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Magma") );
		if( pkEntity )
		{
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			OnSetAbil(pkEntity, AT_TIME, iRandValue);
		}
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgHealingWaveFunction
///////////////////////////////////////////////////////////
int	 PgHealingWaveFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		return -1;
	}
	
	int iPower = pkUnit->GetAbil(AT_C_MAX_HP);
	iPower = static_cast<int>(static_cast<float>(iPower) * static_cast<float>(pkSkill->GetAbil(AT_R_MAX_HP)) / ABILITY_RATE_VALUE_FLOAT);

//	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int const iEffectNo = pkSkill->GetEffectNo();	
	CEffect const* pkEffect = pkUnit->GetEffect(iEffectNo, true);

	if(NULL == pkEffect)
	{
		//Effect가 없으면 추가해준다.
		//계속적으로 힐해주는 스킬이라 MP를 소모시키는 Effect가 필요함.
		pkUnit->AddEffect(iEffectNo, iSkillNo, pArg, pkUnit);
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if ( true == pkUnit->IsAlive() )
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				SkillFuncUtil::OnModifyHP(pkTarget, NULL, 0, iPower, pkUnit, pkGround);
				//int const iHP = pkTarget->GetAbil(AT_HP);
				//int const iMax = pkTarget->GetAbil(AT_C_MAX_HP);
				//int const iNewHP = __min(iMax, iHP+iPower);
				//int const iDelta = iNewHP - iHP;

				//BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
				//kPacket.Push(pkTarget->GetID());
				//kPacket.Push((short)AT_HP);
				//kPacket.Push(iNewHP);
				//kPacket.Push(pkUnit->GetID());
				//kPacket.Push(iSkillNo);
				//kPacket.Push(iDelta);
				//pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);

				//if(iHP != iNewHP)
				//{
				//	OnSetAbil(pkTarget, AT_HP, iNewHP);
				//}	
			}
		}
		++unit_itor;
	}

	return 1;
}