#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_Thief.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "Variant/PgActionResult.h"

///////////////////////////////////////////////////////////
//  PgStripWeaponEffectFunction - 스트립 웨폰
///////////////////////////////////////////////////////////
void PgStripWeaponEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, pkEffect->GetAbil(AT_R_PHY_ATTACK));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, pkEffect->GetAbil(AT_R_PHY_ATTACK));
}

void PgStripWeaponEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, -pkEffect->GetAbil(AT_R_PHY_ATTACK));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, -pkEffect->GetAbil(AT_R_PHY_ATTACK));
}

int PgStripWeaponEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBurrowEffectFunction - 버로우
///////////////////////////////////////////////////////////
void PgBurrowEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_UNIT_HIDDEN,		1);
	OnAddAbil(pkUnit, AT_LOCK_HIDDEN_ATTACK,1);	
	int iAbil = pkUnit->GetAbil(AT_ENABLE_AUTOHEAL);
	//기존의 오토힐 상태를 저장해둔다.
	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL_BACKUP, iAbil);
	//Hp 자동 회복이 되는 상태이면 제거
	if(iAbil & AUTOHEAL_HP)
	{
		OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL, iAbil-AUTOHEAL_HP);
		iAbil -= AUTOHEAL_HP;
	}

	//Mp 자동 회복이 되는 상태이면 제거
	if(iAbil & AUTOHEAL_MP)
	{
		OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL, iAbil-AUTOHEAL_MP);
	}

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-ABILITY_RATE_VALUE);
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,	1);
}

void PgBurrowEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_UNIT_HIDDEN,		-1);
	OnAddAbil(pkUnit, AT_LOCK_HIDDEN_ATTACK,-1);	
	OnAddAbil(pkUnit, AT_R_MOVESPEED,		ABILITY_RATE_VALUE);
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,	-1);
	
	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL, pkUnit->GetAbil(AT_ENABLE_AUTOHEAL_BACKUP));
	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL_BACKUP, 0);
}

int PgBurrowEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		//INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if(pkUnit->GetAbil(AT_UNIT_HIDDEN) == 0)
	{
		return ECT_MUSTDELETE;
	}

	if(-iNeedMp > iNowMp)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}

	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMp);

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgExitEffectFunction - 비상탈출
///////////////////////////////////////////////////////////
void PgExitEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_HP_RECOVERY, pkEffect->GetAbil(AT_HP_RECOVERY));
	OnAddAbil(pkUnit, AT_CANNOT_DAMAGE, 1);
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK, 1);
}

void PgExitEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_HP_RECOVERY, -pkEffect->GetAbil(AT_HP_RECOVERY));
	OnAddAbil(pkUnit, AT_CANNOT_DAMAGE, -1);
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK, -1);
}

int PgExitEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgShadowWalkEffectFunction - 쉐도우 워크
///////////////////////////////////////////////////////////
void PgShadowWalkEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED, pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_UNIT_HIDDEN, 1);
	OnSetAbil(pkUnit, AT_ENABLE_CHECK_ATTACK, pkEffect->GetAbil(AT_ENABLE_CHECK_ATTACK));
}

void PgShadowWalkEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED, -pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_UNIT_HIDDEN, -1);
	OnAddAbil_Attack(pkUnit, AT_ENABLE_CHECK_ATTACK, -pkEffect->GetAbil(AT_ENABLE_CHECK_ATTACK));
}

int PgShadowWalkEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if( 0 == pkUnit->GetAbil(AT_UNIT_HIDDEN) )
	{
		return ECT_MUSTDELETE;
	}

	if( 0 < pkUnit->GetAbil(AT_ENABLE_CHECK_ATTACK) )
	{
		if(0 < pkUnit->GetAbil(AT_CHECK_ATTACK_COUNT))
		{
			return ECT_MUSTDELETE;
		}
	}

	if(-iNeedMp > iNowMp)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}

	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMp);
	
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgChangeNinjaEffectFunction - 닌자변신
///////////////////////////////////////////////////////////
int PgChangeNinjaEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if(-iNeedMp > iNowMp)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}

	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMp);

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgProtectEdgeEffectFunction - 프로텍트엣지
///////////////////////////////////////////////////////////
void PgProtectEdgeEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit
		&& !pkEffect
		)
	{
		return;
	}
	
	int iPercent = 0;	// 데미지 증가분
	int iMinDmg = 0;	// 최소 데미지
	int iMaxDmg = 0;	// 최대 데미지
	CalcParamValue( pkUnit, pkEffect, pkArg, iPercent, iMinDmg, iMaxDmg);	// 미리 계산해서 저장해둠

	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+0, iPercent);
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+1, iMinDmg);
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+2, iMaxDmg);
}

int PgProtectEdgeEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{	
	if( !pkUnit 
		|| !pkEffect 
		|| !pkArg
		)
	{
		return ECT_MUSTDELETE;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if(!pkGround )
	{
		return ECT_MUSTDELETE;
	}

	int iRange = pkEffect->GetAbil(AT_ATTACK_RANGE);
	if( !iRange )
	{
		iRange = 70;
	}
	UNIT_PTR_ARRAY kTargetArray;
	pkGround->GetUnitTargetList(pkUnit, kTargetArray, ESTARGET_ENEMY, iRange, static_cast<int>(AI_Z_LIMIT));

	if( !kTargetArray.empty() )
	{
		int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
		
		// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
		int const iDmgType = pkEffect->GetAbil(AT_EFFECT_DMG_TYPE);
		int iPercent = 0;
		int iMin = 0;
		int iMax = 0;

		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+0, iPercent);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+1, iMin);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+2, iMax);

		int iCount = 0;
		int const iHitRate = pkUnit->GetAbil(AT_C_HIT_SUCCESS_VALUE);

		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(pkEffect->GetEffectNo());

		UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();
		while(kTargetArray.end() != unit_itor)
		{
			CUnit* pkTarget = (*unit_itor).pkUnit;
			if(pkUnit->IsTarget(pkTarget, true, ESTARGET_ENEMY, pkSkillDef))
			{
				int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
				int iDamage = (iMax && iMin) ? iMin + (iRandValue % __max(1, iMax-iMin)) : 0;

				// 레벨차이에 의한 확률 적용
				iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
				int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
				if (iDecHitRate > iRandValue)
				{
					pkUnit->UseRandomSeedType(false);
					if(0 < iDamage)
					{
						iDamage = CS_CheckDamage(pkUnit, pkTarget, iDamage, true, NULL, iPercent, false);
					}
					pkUnit->UseRandomSeedType(true);
					
					if(0 < iDamage 
						&& IsDamageAction(pkTarget, pkSkillDef) 
						) 
					{// 이펙트에서 주는 것은 새로 데미지 액션을 검사해야 함
						if(pkTarget->UnitType() != UT_PLAYER)
						{// Player가 아닌 녀석들만 데미지 액션을 하게 설정한다
							pkTarget->AddEffect(DMG_REACTION_EFFECT_NO, 0, pkArg, pkUnit); // 데미지 액션 이펙트
							SetDamageDelay(pkSkillDef, pkUnit, pkTarget);
						}
					}
					int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
					DoTickDamage(pkUnit, pkTarget, iDamage, pkEffect->GetEffectNo(), iAddEffect, pkArg);
				}
			}

			++unit_itor;
			++iCount;
			if(iMaxTarget <= iCount)
			{
				break;
			}		
		}
	}

	return ECT_DOTICK;
}

bool PgProtectEdgeEffectFunction::CalcParamValue(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, int& riOutPercent, int& riMinDmg, int& riMaxDmg)
{// 물리 데미지와, 마법 데미지가 뒤섞여 있는 상태에 대한 상황을 처리할수 없다. 만약 그런 상황이 온다면 코드를 수정해야함
	if(!pkUnit
		&& !pkEffect
		)
	{
		return false;
	}

	int iDmgType = 0;
	GetParamFromEffect(pkEffect, iDmgType, riOutPercent);
	if( 0 == riOutPercent )
	{
		riOutPercent = static_cast<int>(ABILITY_RATE_VALUE_FLOAT);
	}

	{
		PgPlayer* pkPlayer = NULL;
		pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if(pkPlayer)
		{// 플레이어가
			PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
			if(pkPlayerSkill)
			{
				GET_DEF( CSkillDefMgr, kSkillDefMgr);
				GET_DEF(CEffectDefMgr, kEffectDefMgr);
				for(int i=AT_CHILD_SKILL_NUM_01; i <= AT_CHILD_SKILL_NUM_10; ++i)
				{// 추가로 영향 받는 스킬들을
					int const iChildSkillNo = pkEffect->GetAbil(i);
					if(0 >= iChildSkillNo) { break; }

					int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildSkillNo);
					if(0 >= iLearnedChildSkillNo) { break; }
					
					// 배웠다면
					CSkillDef const* pkChildSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo);
					if(!pkChildSkillDef) { break; }

					int const iChildEffectNo = pkChildSkillDef->GetAbil(AT_EFFECTNUM1);	// 일단 1개만 쓴다
					if(0 >= iChildEffectNo) { break; }
					
					int iChildDmgType=0;
					int iChildPercent=0;
					CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iChildEffectNo);
					GetParamFromEffect(pkEffectDef, iChildDmgType, iChildPercent);
					if(0 >= iDmgType)
					{ 
						iDmgType = iChildDmgType;
					}
					riOutPercent += iChildPercent;
				}
			}
		}
	}

	switch(iDmgType)
	{
	case 1:
		{// 물리
			riMinDmg = pkUnit->GetAbil(AT_C_PHY_ATTACK_MIN);
			riMaxDmg = pkUnit->GetAbil(AT_C_PHY_ATTACK_MAX);

			if(0 < pkUnit->GetAbil(AT_PHY_DMG_PER))
			{
				riOutPercent = static_cast<int>(riOutPercent * static_cast<float>(pkUnit->GetAbil(AT_PHY_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
			}
			else if(0 > pkUnit->GetAbil(AT_PHY_DMG_PER))
			{// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
				riOutPercent += pkUnit->GetAbil(AT_PHY_DMG_PER);
			}
			riOutPercent += ( riOutPercent * pkUnit->GetAbil(AT_PHY_DMG_PER2) ) / ABILITY_RATE_VALUE;
		}break;
	case 2:
		{// 마법
			riMinDmg = pkUnit->GetAbil(AT_C_MAGIC_ATTACK_MIN);
			riMaxDmg = pkUnit->GetAbil(AT_C_MAGIC_ATTACK_MAX);

			if(0 < pkUnit->GetAbil(AT_MAGIC_DMG_PER))
			{
				riOutPercent = static_cast<int>(riOutPercent * static_cast<float>(pkUnit->GetAbil(AT_MAGIC_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
			}
			else if(0 > pkUnit->GetAbil(AT_MAGIC_DMG_PER))
			{// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
				riOutPercent += pkUnit->GetAbil(AT_MAGIC_DMG_PER);
			}
			riOutPercent += ( riOutPercent * pkUnit->GetAbil(AT_MAGIC_DMG_PER2) ) / ABILITY_RATE_VALUE;
		}break;
	}
	return true;
}

bool PgProtectEdgeEffectFunction::GetParamFromEffect(CEffect const* pkEffect, int& riDmgType, int& riPercent)
{
	if(!pkEffect)
	{
		return false;
	}
	riDmgType = pkEffect->GetAbil(AT_EFFECT_DMG_TYPE);
	switch(riDmgType)
	{
	case 1:
		{// 물리
			riPercent = pkEffect->GetAbil(AT_PHY_DMG_PER);
		}break;
	case 2:
		{// 마법
			riPercent = pkEffect->GetAbil(AT_MAGIC_DMG_PER);
		}break;
	default:
		{
			riPercent = 0;
		}break;
	}	
	return true;
}

bool PgProtectEdgeEffectFunction::GetParamFromEffect(CEffectDef const* pkEffect, int& riDmgType, int& riPercent)
{
	if(!pkEffect)
	{
		return false;
	}
	riDmgType = pkEffect->GetAbil(AT_EFFECT_DMG_TYPE);
	switch(riDmgType)
	{
	case 1:
		{// 물리
			riPercent = pkEffect->GetAbil(AT_PHY_DMG_PER);
		}break;
	case 2:
		{// 마법
			riPercent = pkEffect->GetAbil(AT_MAGIC_DMG_PER);
		}break;
	default:
		{
			riPercent = 0;
		}break;
	}	
	return true;
}
///////////////////////////////////////////////////////////
//  PgHPMPRestoreEffectFunction - 
///////////////////////////////////////////////////////////
void PgHPMPRestoreEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit 
		|| !pkEffect
		)
	{
		return;
	}
	
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef
		&& 0 == pkDef->GetInterval()
		)
	{
		int const iAddMPRate = pkEffect->GetAbil(AT_ADD_MP_FROM_MAX_MP_RATE);
		int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
		int const iCurMP = pkUnit->GetAbil(AT_MP);
		if((0>iAddMPRate && iCurMP>0) || (0<iAddMPRate && iMaxMP>iCurMP))
		{
			SkillFuncUtil::OnModifyMP(pkUnit, pkEffect, iAddMPRate, 0, pkUnit, pkGround);
		}

		int const iAddHPRate = pkEffect->GetAbil(AT_ADD_HP_FROM_MAX_HP_RATE);
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iCurHP = pkUnit->GetAbil(AT_HP);
		if((0>iAddHPRate && iCurHP>0) || (0<iAddHPRate && iMaxHP>iCurHP))
		{
			SkillFuncUtil::OnModifyHP(pkUnit, pkEffect,iAddHPRate, 0, pkUnit, pkGround);
		}
	}
	int const iSpecipicIdle = pkEffect->GetAbil(AT_SKILL_SPECIFIC_IDLE);
	if(0 < iSpecipicIdle)
	{
		OnAddAbil(pkUnit, AT_SKILL_SPECIFIC_IDLE, iSpecipicIdle);
	}
}

int PgHPMPRestoreEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		return ECT_DOTICK;
	}

	if(pkUnit->IsDead())
	{
		return ECT_MUSTDELETE;
	}

	int const iAddMPRate = pkEffect->GetAbil(AT_ADD_MP_FROM_MAX_MP_RATE);
	if(0 < iAddMPRate)
	{
		int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
		int const iCurMP = pkUnit->GetAbil(AT_MP);
		if(iMaxMP > iCurMP)
		{
			SkillFuncUtil::OnModifyMP(pkUnit, pkEffect, iAddMPRate, 0, pkUnit, pkGround);
		}
	}

	int const iAddHPRate = pkEffect->GetAbil(AT_ADD_HP_FROM_MAX_HP_RATE);
	if(0 < iAddHPRate)
	{
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iCurHP = pkUnit->GetAbil(AT_HP);
		if(iMaxHP > iCurHP)
		{
			SkillFuncUtil::OnModifyHP(pkUnit, pkEffect,iAddHPRate, 0, pkUnit, pkGround);
		}
	}

	return ECT_DOTICK;
}

void PgHPMPRestoreEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if(!pkUnit 
		|| !pkEffect
		)
	{
		return;
	}

	int const iSpecipicIdle = pkEffect->GetAbil(AT_SKILL_SPECIFIC_IDLE);
	if(0 < iSpecipicIdle)
	{
		OnAddAbil(pkUnit, AT_SKILL_SPECIFIC_IDLE, -iSpecipicIdle);
	}
}