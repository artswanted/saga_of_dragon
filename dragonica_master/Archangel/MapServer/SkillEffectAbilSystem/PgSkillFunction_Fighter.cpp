#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Fighter.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgEffectFunction.h"

///////////////////////////////////////////////////////////
//  PgHammerCrushSkillFunction
///////////////////////////////////////////////////////////
int PgHammerCrushSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed())
		{
			// Damage가 있고 공격 받을 수 있는 상태 일 때
			if(pkAResult->GetValue() && (0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)) && !pkAResult->GetEndure() && (false == (*unit_itor).bReference))
			{
				int iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
				int iChildSkillEffectPercent = 0;
				int iChildSkillEffectNo = 0;

				CUnit* pkUnit2 = pkUnit;

				//Entity일 경우 부모의 Unit을 찾아서 Unit2에 세팅해야 한다.
				BM::GUID const& kParentGUID = pkUnit->Caller();
				if(BM::GUID::IsNotNull(kParentGUID) && pkUnit->GetID() != kParentGUID)
				{
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
									{// 연관 스킬에서 대상에게 걸어야 하는 이펙트가 존재 한다면
										int const iChildEffectNo = pkLearnedSkillDef->GetAbil(AT_EFFECTNUM1);
										if(0 < iChildEffectNo)
										{
											iChildSkillEffectNo = iChildEffectNo;
										}
										int const iChildPercent = pkLearnedSkillDef->GetAbil(AT_PERCENTAGE);
										if(0 < iChildPercent)
										{// 값이 설정 되어있다면 연관 스킬의 것으로
											iChildSkillEffectPercent = iChildPercent;
										}
										else
										{// 값이 없다면, 현재 사용하는 스킬것으로 사용함
											iChildSkillEffectPercent = iEffectPercent;
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

				{// 사용한 스킬에서 걸어주는 이펙트
					int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
					if(iRandValue <= iEffectPercent)
					{
						if(iEffectNo)
						{
							pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
						}
					}
				}
				{// 연관된 스킬에서 얻어온 이펙트
					int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
					if(iRandValue <= iChildSkillEffectPercent)
					{
						if(iChildSkillEffectNo)
						{
							pkTarget->AddEffect(iChildSkillEffectNo, 0, pArg, pkUnit);
						}
					}
				}
			}

			// 이펙트를 먼저 걸어주고 데미지를 준다.
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
			
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgArmorBreakSkillFunction
///////////////////////////////////////////////////////////
int PgArmorBreakSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	
	
	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed())
		{
			int const iDamage = pkAResult->GetValue();	//DoFinalDamage에 의해 데미지가 변경 될 수 있지만

			if(iDamage && (0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)) && (false == (*unit_itor).bReference))
			{// 데미지 유무 만 판단 하므로 , 위에서 먼저 데미지를 얻어와도 상관없음
				int const iEffectNo = pkSkill->GetEffectNo();
				if(iEffectNo)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}

			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgStormbladeSkillFunction
///////////////////////////////////////////////////////////
int PgStormbladeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);
	
	if(!pkUnitArray->empty())
	{
		//시전자에게 이펙트가 걸려야 한다.
		int const iEffectNo = pkSkill->GetEffectNo();
		if(iEffectNo)
		{
			pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
		}
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
//  PgSwordMasterySkillFunction
///////////////////////////////////////////////////////////
int PgSwordMasterySkillFunction::SkillPassive(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkUnit->CheckPassive(pkSkill->No(), pArg))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	return SkillPCheck(pkUnit, pkSkill, pArg);
}

int PgSwordMasterySkillFunction::SkillPCheck(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	bool bOn = false;
	int const iEffectNo = pkSkill->GetEffectNo();
	PgUnitEffectMgr& rkEffectMgr = pkUnit->GetEffectMgr();

	if(rkEffectMgr.FindInGroup(iEffectNo, true))
	{
		// 이미 존재하는 Effect 
		bOn = true;
	}

	if(!pkUnit->CheckPassive(pkSkill->No(), pArg))
	{
		if(bOn)
		{
			SkillEnd(pkUnit, pkSkill, pArg);
		}
		return -1;
	}

	if(!bOn)
	{
		SkillBegin(pkUnit, pkSkill, pArg);
	}

	return 0;
}

void PgSwordMasterySkillFunction::SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg) // Passive용 Begin
{
	int const iEffectNo = pkSkill->GetEffectNo();

	int const iAdd = pkSkill->GetAbil(AT_PHY_ATTACK);
	CEffect* pkEffect = pkUnit->AddEffect(iEffectNo, iAdd, pArg, pkUnit);
	if(pkEffect)
	{
		pkEffect->SetActArg(ACTARG_WEAPONLIMIT, pkSkill->GetAbil(AT_WEAPON_LIMIT));
	}
}

void PgSwordMasterySkillFunction::SkillEnd(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	int const iEffectNo = pkSkill->GetEffectNo();
	pkUnit->DeleteEffect(iEffectNo);
}

///////////////////////////////////////////////////////////
//  PgRouletteSkillFunction
///////////////////////////////////////////////////////////
int PgRouletteSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	bool bReturn = CS_GetSkillResult106000101(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iDice = pkAResult->GetAbil(AT_DICE_VALUE);
			int const iEffectAbilNo = AT_EFFECTNUM1 + iDice;
			int const iEffectNo = pkSkill->GetAbil(iEffectAbilNo);

			if(0 < iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgStumblebumSkillFunction
///////////////////////////////////////////////////////////
int PgStumblebumSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}
			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
			if(iRandValue <= iEffectPercent)
			{
				int const iEffectNo2 = pkSkill->GetAbil(AT_EFFECTNUM1);
				if(iEffectNo2)
				{
					pkTarget->AddEffect(iEffectNo2, 0, pArg, pkUnit);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgJointBreakSkillFunction
///////////////////////////////////////////////////////////
int PgJointBreakSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}

			// Damage가 있고 공격 받을 수 있는 상태 일 때
			if((0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)))
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

				if(iRandValue <= iEffectPercent)
				{
					for(int i = 0; i <= EFFECTNUM_MAX; ++i)
					{
						int iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1 + i);

						CUnit* pkUnit2 = pkUnit;

						//Entity일 경우 부모의 Unit을 찾아서 Unit2에 세팅해야 한다.
						BM::GUID const& kParentGUID = pkUnit->Caller();
						if(BM::GUID::IsNotNull(kParentGUID) && pkUnit->GetID() != kParentGUID)
						{
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
												// 더 강력한 버프를 거는 경우
												int iChildEffectNo = pkLearnedSkillDef->GetAbil(AT_EFFECTNUM1 + i);
												if(0 < iChildEffectNo)
												{
													iEffectNo = iChildEffectNo;
													break;
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

						if(0 < iEffectNo)
						{
							pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
						}
					}
				}

				if(bSkillDamage)
				{
					::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgBugsLifeSkillFunction
///////////////////////////////////////////////////////////
int PgBugsLifeSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	int iNowHP = pkUnit->GetAbil(AT_HP);
	//스킬을 사용하려는 HP보다 큰 경우
	if(iNowHP > pkUnit->GetAbil(AT_C_MAX_HP) * (static_cast<float>(pkSkill->GetAbil(AT_R_MAX_HP)) / ABILITY_RATE_VALUE_FLOAT))
	{
		return -1;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			//최대 HP의 특정 % 만큼 HP를 회복 시킨다.
			//int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
			//int iNowHP = static_cast<int>(iMaxHP * (static_cast<float>(pkSkill->GetAbil(AT_R_HP_RECOVERY)) / ABILITY_RATE_VALUE_FLOAT));
			//OnSetAbil(pkUnit, AT_HP, iNowHP);
			SkillFuncUtil::OnModifyHP(pkUnit, NULL, pkSkill->GetAbil(AT_R_HP_RECOVERY), 0, pkUnit, pkGround);
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}
