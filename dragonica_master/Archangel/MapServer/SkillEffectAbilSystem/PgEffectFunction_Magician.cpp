#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_Magician.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"

#include "PgSkillFunction_Magician.h"

///////////////////////////////////////////////////////////
//  PgTransformationEffectFunction - 마녀의 저주
///////////////////////////////////////////////////////////
void PgTransformationEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	
	OnAddAbil(pkUnit, AT_R_MOVESPEED,	pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK, pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,  pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM, pkEffect->GetAbil(AT_CANNOT_USEITEM));

	//Effect가 걸릴때 hp를 백업
	OnSetAbil(pkUnit, AT_HP_BACKUP, pkUnit->GetAbil(AT_HP));

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

	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL,	AUTOHEAL_PLAYER);
	pkUnit->SetAbil(AT_AI_SYNC, 1);
}

void PgTransformationEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,	-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,	-pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,	-pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,-pkEffect->GetAbil(AT_CANNOT_USEITEM));

	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL, pkUnit->GetAbil(AT_ENABLE_AUTOHEAL_BACKUP));
	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL_BACKUP, 0);
	OnSetAbil(pkUnit, AT_HP_BACKUP, 0);
	pkUnit->SetAbil(AT_AI_SYNC, 1);
}

int PgTransformationEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//Effect가 걸렸을때 보다 HP가 작다면
	if(pkUnit->GetAbil(AT_HP_BACKUP) > pkUnit->GetAbil(AT_HP))
	{
		return ECT_MUSTDELETE;
	}
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBurberrianEffectFunction - 바바리안
///////////////////////////////////////////////////////////
void PgBurberrianEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iSpeed = pkUnit->GetAbil(AT_MOVESPEED);
	OnAddAbil(pkUnit, AT_R_MOVESPEED,	-iSpeed);
	CEffect* pkCasterEffect = pkUnit->GetEffect(pkEffect->GetEffectNo());
	pkCasterEffect->SetValue(iSpeed);

	OnAddAbil(pkUnit, AT_FROZEN, 1);
}

void PgBurberrianEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iSpeed = pkEffect->GetValue();
	OnAddAbil(pkUnit, AT_R_MOVESPEED,	iSpeed);
	
	OnAddAbil(pkUnit, AT_FROZEN, -1);
}

int PgBurberrianEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgManaShieldEffectFunction - 마나실드
///////////////////////////////////////////////////////////
void PgManaShieldEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_PDMG_DEC_RATE_2ND, pkEffect->GetAbil(AT_PHY_DMG_DEC));
	OnAddAbil(pkUnit, AT_MDMG_DEC_RATE_2ND, pkEffect->GetAbil(AT_MAGIC_DMG_DEC));
	OnAddAbil(pkUnit, AT_DMG_CONSUME_MP, pkEffect->GetAbil(AT_DMG_CONSUME_MP));
	OnSetAbil(pkUnit, AT_DMG_DEC_RATE_COUNT, -1);
}

void PgManaShieldEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_PDMG_DEC_RATE_2ND, -pkEffect->GetAbil(AT_PHY_DMG_DEC));
	OnAddAbil(pkUnit, AT_MDMG_DEC_RATE_2ND, -pkEffect->GetAbil(AT_MAGIC_DMG_DEC));
	OnAddAbil(pkUnit, AT_DMG_CONSUME_MP, -pkEffect->GetAbil(AT_DMG_CONSUME_MP));
	OnSetAbil(pkUnit, AT_DMG_DEC_RATE_COUNT, 0);
}

int PgManaShieldEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgConcentrationEffectFunction - 정신일도하사불성
///////////////////////////////////////////////////////////
void PgConcentrationEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_INT, pkEffect->GetAbil(AT_INT));
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MAX, pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MIN, pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX));
}

void PgConcentrationEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_INT, -pkEffect->GetAbil(AT_INT));
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MAX, -pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MIN, -pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX));

	int const iNowMP = pkUnit->GetAbil(AT_MP);
	int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

	if(iNowMP > iMaxMP)
	{
		int const iNewMP = iMaxMP;
		OnSetAbil(pkUnit, AT_MP, iNewMP);
	}
}

int PgConcentrationEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgDiseaseEffectFunction - 감염
///////////////////////////////////////////////////////////
void PgDiseaseEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_HP,	pkEffect->GetAbil(AT_R_MAX_HP));
	OnAddAbil(pkUnit, AT_R_MAX_MP,	pkEffect->GetAbil(AT_R_MAX_MP));

	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
	if(iNowHP > iMaxHP)
	{
		OnSetAbil(pkUnit, AT_HP, iMaxHP);
	}

	int const iNowMP = pkUnit->GetAbil(AT_MP);
	int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
	if(iNowMP > iMaxMP)
	{
		OnSetAbil(pkUnit, AT_MP, iMaxMP);
	}
}

void PgDiseaseEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_HP,	-pkEffect->GetAbil(AT_R_MAX_HP));
	OnAddAbil(pkUnit, AT_R_MAX_MP,	-pkEffect->GetAbil(AT_R_MAX_MP));
}

int PgDiseaseEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgMirrorImageEffectFunction - 미러이미지
///////////////////////////////////////////////////////////
void PgMirrorImageEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iAddCount = pkEffect->GetAbil(AT_BARRIER_100PERECNT_COUNT);
	OnSetAbil2(pkUnit, AT_BARRIER_100PERECNT_COUNT,		 iAddCount);
	OnSetAbil2(pkUnit, AT_PREV_BARRIER_100PERECNT_COUNT, iAddCount);

	//Player 주위를 회전하는 상태이상 이펙트 추가
	int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(0 < iAddEffect)
	{
		EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, iAddCount, pkArg, pkUnit->GetID());
		pkUnit->AddEffectQueue(kData);
	}
}

void PgMirrorImageEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnSetAbil2(pkUnit, AT_BARRIER_100PERECNT_COUNT,		 0);
	OnSetAbil2(pkUnit, AT_PREV_BARRIER_100PERECNT_COUNT, 0);

	int const iDelEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(0 < iDelEffect)
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, iDelEffect);
		pkUnit->AddEffectQueue(kData);
	}
}

int PgMirrorImageEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iValue = pkUnit->GetAbil(AT_BARRIER_100PERECNT_COUNT);
	int const iPrevValue = pkUnit->GetAbil(AT_PREV_BARRIER_100PERECNT_COUNT);

	//숫자의 변화가 생겼으면
	if(iValue != iPrevValue)
	{
		OnSetAbil(pkUnit, AT_PREV_BARRIER_100PERECNT_COUNT, iValue);

		int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
		if(0 < iAddEffect)
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, iValue, pkArg, pkUnit->GetID());
			pkUnit->AddEffectQueue(kData);
		}

		if(2 == iValue)
		{
			int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM4);
			if(0 < iAddEffect)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, iValue, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);
			}
		}
		else if(1 == iValue)
		{
			int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM3);
			if(0 < iAddEffect)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, iValue, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);
			}
		}
		else if(0 == iValue)
		{
			int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM2);
			if(0 < iAddEffect)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, iValue, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);
			}
		}
	}

	if(0 == iValue)
	{
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgThunderBreakEffectFunction - 썬더 브레이크
///////////////////////////////////////////////////////////
void PgThunderBreakEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgThunderBreakEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgThunderBreakEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		//맵 이동시 Ground는 NULL인데, 맵 이동 후에도 존재 해야 한다.
		//INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		return ECT_DOTICK;
	}

	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	int const iUnitType = pkUnit->UnitType();

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_ENEMY, iRange, static_cast<int>(AI_Z_LIMIT));

	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
	int iPercent = pkEffect->GetAbil(AT_MAGIC_DMG_PER);
	
	// + 인 경우는 스킬의 최종 데미지를 감소 시키는 경우
	if(0 < pkUnit->GetAbil(AT_MAGIC_DMG_PER))
	{
		iPercent = static_cast<int>(iPercent * static_cast<float>(pkUnit->GetAbil(AT_MAGIC_DMG_PER)) / ABILITY_RATE_VALUE_FLOAT);
	}
	// - 인 경우는 스킬의 최종 데미지를 1로 만들어 버리는 경우
	else if(0 > pkUnit->GetAbil(AT_MAGIC_DMG_PER))
	{
		iPercent += pkUnit->GetAbil(AT_MAGIC_DMG_PER);
	}

	int iCount = 0;

	int const iHitRate = pkUnit->GetAbil(AT_C_HIT_SUCCESS_VALUE);

	for(UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin(); itor != kUnitArray.end(); ++itor)
	{
		CUnit* pkTarget = (*itor).pkUnit;
		if(pkTarget)
		{
			if(pkUnit->IsTarget(pkTarget, false, ESTARGET_ENEMY))
			{
				int iMagicDamage2 = static_cast<int>(static_cast<float>(pkUnit->GetAbil(AT_C_MAGIC_ATTACK)) * static_cast<float>(iPercent) / ABILITY_RATE_VALUE_FLOAT);
				//기본적인 공격자의 공격력 + 스킬 공격력
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;

				int const iMin = pkUnit->GetAbil(AT_C_MAGIC_ATTACK_MIN);
				int const iMax = pkUnit->GetAbil(AT_C_MAGIC_ATTACK_MAX);
				int iMagicDamage = 0;

				// 최종공격력(물리) = 기본공격력 + 아이템공격력 + 물리후방공격력
				// 최종공격력(마법) = (pkUnit + 아이템공격력) * 1.5		
				int const iAddDmg = pkUnit->GetAbil(AT_C_ABS_ADDED_DMG_MAGIC);

				iMagicDamage = int((iMin + iRandValue % __max(1, iMax-iMin)) * 1.5f) + iAddDmg;

				if(0 < iMagicDamage)
				{
					// 레벨차이에 의한 확률 적용
					int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
					int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
					if (iDecHitRate > iRandValue)
					{
						int const iAddEffectNo = pkEffect->GetAbil(AT_MON_SKILL_01);
						
						pkUnit->UseRandomSeedType(false);
						iMagicDamage = CS_CheckDamage(pkUnit, pkTarget, iMagicDamage, false, NULL, iPercent, false);
						pkUnit->UseRandomSeedType(true);
						DoTickDamage(pkUnit, pkTarget, iMagicDamage, pkEffect->GetEffectNo(), iAddEffectNo, pkArg, true);
					}
					else
					{
						// 썬더브레이크 실패
						int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
						if(0 < iAddEffect)
						{
							EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
							pkTarget->AddEffectQueue(kData);
						}
					}
				}
			}
		}		

		++iCount;

		if(iMaxTarget <= iCount)
		{
			break;
		}		
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgChainLightingEffectFunction - 체인 라이트닝
///////////////////////////////////////////////////////////
void PgChainLightingEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgChainLightingEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgChainLightingEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());

	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot Casting Unit->Player dynamic_cast EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
		return ECT_MUSTDELETE;
	}

	int const iActionID = pkPlayer->GetActionID();
	if(iActionID != pkEffect->GetValue())
	{
		//체인 라이트닝 발사가 중지 되었을 경우
		return ECT_MUSTDELETE;
	}

	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if(-iNeedMp > iNowMp)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}	

	pkUnit->SetAbil(AT_MP, iNeedMp + iNowMp, true, true);

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgHPRestoreEffectFunction - HP리스토어
///////////////////////////////////////////////////////////
void PgHPRestoreEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//스킬에 의해서 걸린 것이 아니라 버프로 걸리는 경우라면 회복량을 직접 세팅해 주어야 한다.
	if(0 == pkEffect->GetValue())
	{
		int iAddHPValue = CalcSlowHealValue(pkEffect->GetAbil(AT_R_MAX_HP), pkUnit);
		if(0 < iAddHPValue)
		{
			pkEffect->SetValue(iAddHPValue);
		}
	}
}

void PgHPRestoreEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgHPRestoreEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());
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
		return ECT_DOTICK;
	}

	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
	int const iNowHP = pkUnit->GetAbil(AT_HP);

	if(iMaxHP == iNowHP)
	{
		return ECT_DOTICK;
	}

	//Skill에서 처음에 Effect에 세팅한 값을 더해 준다.
	//OnSetAbil(pkUnit, AT_HP, __min(iMaxHP, iNowHP + pkEffect->GetValue()));
	SkillFuncUtil::OnModifyHP(pkUnit, pkEffect, 0, pkEffect->GetValue(), pkUnit, pkGround);

	int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(0 < iAddEffect)
	{
		EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
		pkUnit->AddEffectQueue(kData);
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBarrierEffectFunction - 배리어
///////////////////////////////////////////////////////////
void PgBarrierEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnSetAbil2(pkUnit, AT_BARRIER_AMOUNT, pkEffect->GetAbil(AT_BARRIER_AMOUNT) + pkUnit->GetAbil(AT_ADD_BARRIER_AMOUNT));
}

void PgBarrierEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnSetAbil2(pkUnit, AT_BARRIER_AMOUNT, 0);
}

int PgBarrierEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick E03ffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());

	if(pkUnit->GetAbil(AT_BARRIER_AMOUNT) <= 0)
	{
		return ECT_MUSTDELETE;
	}	
	
	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgSantuaryEffectFunction - 생츄어리 설치
///////////////////////////////////////////////////////////
void PgSantuaryEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	pkEffect->SetValue(pkEffect->GetAbil(AT_COUNT)); //회복할 수 있는 Tick Count를 세팅
}

void PgSantuaryEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgSantuaryEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());
	
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		//맵 이동시 Ground는 NULL인데, 맵 이동 후에도 존재 해야 한다.
		//INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		return ECT_DOTICK;
	}

	//회복 횟수가 다 되었다면 해제 시킨다.
	if(0 == pkEffect->GetValue())
	{
		pkUnit->SetAbil(AT_HP, 0);

		CUnit* pkCaster = pkGround->GetUnit(pkUnit->Caller());
		if(pkCaster)
		{
			int const iParentSkillNo = pkEffect->GetAbil(AT_PARENT_SKILL_NUM);
			if(0 < iParentSkillNo)
			{
				//pkCaster->DeleteEffect(iParentSkillNo);
				EffectQueueData kData(EQT_DELETE_EFFECT, iParentSkillNo);
				pkCaster->AddEffectQueue(kData);				
			}
		}

		return ECT_MUSTDELETE;
	}

	
	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_PARTY, iRange, static_cast<int>(AI_Z_LIMIT));
	if(BM::GUID::IsNotNull(pkUnit->Caller()))
	{
		if(CUnit* pPlayer = pkGround->GetUnit(pkUnit->Caller()))
		{// 소환자가
			if(ERange_OK==::IsInRange(pkUnit->GetPos(), pPlayer->GetPos(), iRange, static_cast<int>(AI_Z_LIMIT)))
			{// 범위에 포함된다면
				bool bExistCaller = false;
				for(UNIT_PTR_ARRAY::iterator kItor = kUnitArray.begin(); kItor != kUnitArray.end(); ++kItor)
				{// 중복 검사를 해서
					CUnit const* pkTarget = (*kItor).pkUnit;
					if( pkTarget
						&& pkTarget->GetID() == pPlayer->GetID() )
					{
						bExistCaller = true;
						break;
					}
				}
				if( false == bExistCaller )
				{// 중복되지 않았다면 포함시켜 준다
					kUnitArray.Add(pPlayer);
				}
			}
		}
	}

	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
	int iAddHeal = pkEffect->GetAbil(AT_HP);//생추어리
	WORD iAbilType = AT_HP;
	WORD iAbilType2 = AT_C_MAX_HP;

	if(0 == iAddHeal) // 아크 생추어리 일 경우 어빌 및 값을 변경 한다.
	{
		iAbilType = AT_MP;
		iAbilType2 = AT_C_MAX_MP;
		iAddHeal = pkEffect->GetAbil(AT_MP); //아크 생추어리
	}

	int iCount = 0;

	for(UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin(); itor != kUnitArray.end(); ++itor)
	{
		CUnit* pkTarget = (*itor).pkUnit;
		if(pkTarget)
		{
			if(false == pkUnit->IsTarget(pkTarget, false, ESTARGET_ENEMY)) //적이 아닐 경우라면
			{
				int const iCurValue = pkTarget->GetAbil(iAbilType);
				int const iMax = pkTarget->GetAbil(iAbilType2);
				
				//타겟이 잡혔더라도 HP가 꽉 차있다면 타겟팅에서 제외 한다.
				if(iMax == iCurValue)
				{
					continue;
				}

				if(pkTarget->IsDead())
				{
					continue;
				}

				bool bAddEffect = false;
				switch(iAbilType)
				{
				case AT_HP:
					{// 생츄어리
						bAddEffect = 0 < SkillFuncUtil::OnModifyHP(pkTarget, pkEffect, 0, iAddHeal, pkUnit, pkGround);
					}break;
				case AT_MP:
					{// 아크 생츄어리
						bAddEffect = 0 < SkillFuncUtil::OnModifyMP(pkTarget, pkEffect, 0, iAddHeal, pkUnit, pkGround);
					}break;
				}
				if(bAddEffect)
				{
					int iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
					if(0 < iAddEffect)
					{
						EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
						pkTarget->AddEffectQueue(kData);
					}
				}
				++iCount;
			}
		}

		if(iMaxTarget <= iCount)
		{
			break;
		}
	}

	if(0 < iCount)
	{
		//한명이라도 회복 했다면 회복 가능 횟수를 줄여 준다.
		pkEffect->SetValue(pkEffect->GetValue() - 1);
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgCureEffectFunction - 큐어
///////////////////////////////////////////////////////////
void PgCureEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//큐어 이펙트는 큐어 스킬에서 걸지 않고 이펙트에서 직접 걸도록 되어있다.
	//이미 서비스 하고 있는 기존 국가들 때문에 기존 코드를 고치게 되면 DB및 다 수정되어야 한다.
	//새로 추가해서 나만 큐어가 걸리는 기능을 추가
	
	int const iEffectNo = pkEffect->GetEffectNo();

	// 이펙트 번호와 같은 번호의 스킬 번호를 얻어서 정보를 얻어 온다.
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iEffectNo);

	if(pkSkill)
	{
		//지울수 있는 Effect 개수
		int const iDeleteCount = pkSkill->GetAbil(AT_COUNT);

		PgUnitEffectMgr const& rkEffectMgr = pkUnit->GetEffectMgr();

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
						// 이펙트는 이펙트에서 지우면 안된다. 꼭 큐에 넣어서 업데이트시에 지워주어야 한다.
						EffectQueueData kData(EQT_DELETE_EFFECT, pkEffect->GetEffectNo());
						pkUnit->AddEffectQueue(kData);

						++iCount;
					}
				}
			}
		}
	}
}

void PgCureEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgCureEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_MUSTDELETE;
}
