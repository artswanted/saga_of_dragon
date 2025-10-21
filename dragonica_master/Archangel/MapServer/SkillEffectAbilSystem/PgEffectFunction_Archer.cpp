#include "stdafx.h"
#include "Variant/Global.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "Global.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_Archer.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PgEffectFunction.h"

///////////////////////////////////////////////////////////
//  PgBeautifulGirlFunction - 멋진 언니
///////////////////////////////////////////////////////////
void PgBeautifulGirlFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgBeautifulGirlFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgBeautifulGirlFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE;
	}

	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_ENEMY, iRange, static_cast<int>(AI_Z_LIMIT));

	for(UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin(); itor != kUnitArray.end(); ++itor)
	{
		CUnit* pkMonster = (*itor).pkUnit;
		if(pkMonster)
		{
			pkMonster->ClearTargetList();
			pkMonster->SetTarget(pkUnit->GetID());			
		}		
	}	

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgIncreasedStatusFunction - 버드 워칭
///////////////////////////////////////////////////////////
void PgIncreasedStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_STR, pkEffect->GetAbil(AT_STR));
	OnAddAbil(pkUnit, AT_DEX, pkEffect->GetAbil(AT_STR));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
}

void PgIncreasedStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_STR, -pkEffect->GetAbil(AT_STR));
	OnAddAbil(pkUnit, AT_DEX, -pkEffect->GetAbil(AT_STR));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, -pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, -pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
}

int PgIncreasedStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBowMasteryFunction - 보우 마스터리
///////////////////////////////////////////////////////////
void PgBowMasteryFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
}

void PgBowMasteryFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_PHY_ATTACK_MAX, -pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_PHY_ATTACK_MIN, -pkEffect->GetAbil(AT_PHY_ATTACK_MAX));
}

int PgBowMasteryFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBleedingFunction - 모서리로 찍으세요
///////////////////////////////////////////////////////////
void PgBleedingFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgBleedingFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgBleedingFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int iNewHP = __max(0, iNowHP + iAddHP);
	int const iDelta = iNewHP - iNowHP;

	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkUnit->GetID());
	kPacket.Push((short)AT_HP);
	kPacket.Push(iNewHP);
	kPacket.Push(pkEffect->GetCaster());
	kPacket.Push(pkEffect->GetEffectNo());
	kPacket.Push(iDelta);
	pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);	

	OnSetAbil(pkUnit, AT_HP, iNewHP);

	if(0 == iNewHP)
	{
		pkUnit->SetTarget(pkEffect->GetCaster());
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAtropineFireFunction - Effect Function 아트로핀 발동
///////////////////////////////////////////////////////////
void PgAtropineFireFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iUseCount = pkUnit->GetAbil(AT_ATROPINE_USE_COUNT); // 타겟의 현재 아트로핀 누적 횟수를 얻어 온다.
	
	int iNewEffect = 0;
	int iDeleteEffect = 0;
	switch(iUseCount)
	{
	case 0:{ iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM1);}break; // 첫번째 사용
	case 1:
		{
			iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM2);
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect);
			if(pkEffectDef)
			{
				iDeleteEffect = pkEffectDef->GetAbil(AT_EFFECTNUM1);
			}

		}break; // 두번째 사용
	case 2:
		{
			iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM3);
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect);
			if(pkEffectDef)
			{
				iDeleteEffect = pkEffectDef->GetAbil(AT_EFFECTNUM1);
			}
		}break; // 세번째 사용
	case 3:
		{
			iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM4);
			GET_DEF(CEffectDefMgr, kEffectDefMgr);
			CEffectDef const* pkEffectDef = kEffectDefMgr.GetDef(iNewEffect);
			if(pkEffectDef)
			{
				iDeleteEffect = pkEffectDef->GetAbil(AT_EFFECTNUM1);
			}
		}break; // 네번째 사용
	default:
		{
			INFO_LOG(BM::LOG_LV9, __FL__<<L"Not Match NewAddEffectNo["<<iNewEffect<<L"]");
		}break;
	}

	if(0 < iDeleteEffect)
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, iDeleteEffect);
		pkUnit->AddEffectQueue(kData);
	}
	
	if(0 < iNewEffect)
	{
		EffectQueueData kData(EQT_ADD_EFFECT, iNewEffect, 0, pkArg, pkUnit->GetID());
		pkUnit->AddEffectQueue(kData);
	}
}

void PgAtropineFireFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

///////////////////////////////////////////////////////////
//  PgAtropineEffectFunction - Effect Function 아트로핀 효과
///////////////////////////////////////////////////////////
void PgAtropineEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iAddCount = pkEffect->GetAbil(AT_COUNT);

	pkUnit->GetEffectMgr().AddAbil(AT_ATROPINE_USE_COUNT, iAddCount);
	pkUnit->NftChangedAbil(AT_ATROPINE_USE_COUNT, E_SENDTYPE_NONE);
	
	int const iUseCount = pkUnit->GetAbil(AT_ATROPINE_USE_COUNT);

	if(3 <= iUseCount) // 세번째 / 네번째 사용 할때
	{
		int const iAddHP = pkEffect->GetAbil(AT_R_MAX_HP);
		OnAddAbil(pkUnit, AT_R_MAX_HP, iAddHP);
		
		// 현재 Hp 감소
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iRetHP = __min(iNowHP, iMaxHP);

		if(iNowHP > iRetHP)
		{
			//지금 피가 Max보다 크기 때문에
			OnSetAbil(pkUnit, AT_HP, iRetHP);
		}
	}
	
	{// 디버프 해제
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
}

void PgAtropineEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iNowCount = pkUnit->GetAbil(AT_ATROPINE_USE_COUNT); // 타겟의 현재 아트로핀 누적 횟수를 얻어 온다.

	if(3 <= iNowCount) // 세번째 / 네번째 사용
	{
		int const iAdd = pkEffect->GetAbil(AT_R_MAX_HP);
		OnAddAbil(pkUnit, AT_R_MAX_HP, -iAdd);
	}

	//아트로핀 카운터를 없앤다.
	int const iCount = pkEffect->GetAbil(AT_COUNT);
	pkUnit->GetEffectMgr().AddAbil(AT_ATROPINE_USE_COUNT, -iCount);
	pkUnit->NftChangedAbil(AT_ATROPINE_USE_COUNT, E_SENDTYPE_NONE);
}

///////////////////////////////////////////////////////////
//  PgDonTouchMeFunction - Effect Function 건들지마라
///////////////////////////////////////////////////////////
void PgDonTouchMeFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

void PgDonTouchMeFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	if ( pkUnit->IsDead() )
	{
		return;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ;
	}

	POINT3 const kPos = pkUnit->GetPos();	
	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_ENEMY, iRange, static_cast<int>(AI_Z_LIMIT));

	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
	int const iCount = pkEffect->GetAbil(AT_COUNT);
	int iNowCount = 0;

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	for(UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin(); itor != kUnitArray.end(); ++itor)
	{
		CUnit* pkTarget = (*itor).pkUnit;
		if(pkTarget)
		{
			if(pkUnit->IsTarget(pkTarget, false, ESTARGET_ENEMY)) // 적일 경우
			{
				int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
				int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
				if (iDecHitRate < iRandValue)
				{// 타격 실패 (Missed)					
					pkTarget->AddEffect(MISS_EFFECT_NO, 0 ,pkArg, pkUnit);
					continue;
				}

				
				int iDamage = pkUnit->GetAbil(AT_HP)*  iCount; // 남아있는 HP *  Count = 데미지
				int iPercent = ABILITY_RATE_VALUE;

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
				
				// Proman : 3153 중국 요청시 제거
				//if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_CHINA) )
				//{
				//	iDamage = static_cast<int>(iDamage * static_cast<float>(iPercent) / ABILITY_RATE_VALUE_FLOAT);
				//	iDamage = __max(1, iDamage);
				//}
				//else
				{
					pkUnit->UseRandomSeedType(false);
					iDamage = CS_CheckDamage(pkUnit, pkTarget, iDamage, false, NULL, iPercent, false);
					pkUnit->UseRandomSeedType(true);
				}

				DoTickDamage(pkUnit, pkTarget, iDamage, pkEffect->GetEffectNo(), 0, pkArg);
			
				++iNowCount;

				if(iNowCount >= iMaxTarget)
				{
					break;
				}				
			}
		}
	}

	// 현재 HP / MP를 effect에 세팅된 값으로 세팅한다.
	OnSetAbil(pkUnit, AT_HP, ((CEffect*)pkEffect)->GetAbil(AT_HP));
	OnSetAbil(pkUnit, AT_MP, ((CEffect*)pkEffect)->GetAbil(AT_MP));

	int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);

	if(0 < iAddEffect)
	{
		EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
		pkUnit->AddEffectQueue(kData);
	}
}

///////////////////////////////////////////////////////////
//  PgCamouflageEffectFunction - 카모플라쥬
///////////////////////////////////////////////////////////
void PgCamouflageEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_UNIT_HIDDEN , 1);
	OnAddAbil(pkUnit, AT_UNLOCK_HIDDEN_MOVE , 1);

	POINT3 kPos = pkUnit->GetPos();

	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1, (int)kPos.x);
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+1, (int)kPos.y);
	pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+2, (int)kPos.z);
}

void PgCamouflageEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_UNIT_HIDDEN , -1);
	OnAddAbil(pkUnit, AT_UNLOCK_HIDDEN_MOVE , -1);
}

int PgCamouflageEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if(0 > iNeedMp + iNowMp)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE;
	}

	if(0 >= pkUnit->GetAbil(AT_UNIT_HIDDEN))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE;
	}
	POINT3 kPos = pkUnit->GetPos();
	int ix = 0,iy = 0, iz = 0;
	pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1, ix);
	pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+1, iy);
	pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+2, iz);

	if((int)kPos.x != ix) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE; 
	}
	if((int)kPos.y != iy) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE; 
	}
	if((int)kPos.z != iz) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE; 
	}

	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMp);
	
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgWPGranadeFireEffectFunction - WP그레네이드 투척
///////////////////////////////////////////////////////////
void PgWPGranadeFireEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgWPGranadeFireEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgWPGranadeFireEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ECT_MUSTDELETE"));
		return ECT_MUSTDELETE;
	}

	int const iDetectRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
	int iCount = 0;

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_ENEMY, iDetectRange, static_cast<int>(AI_Z_LIMIT));
		
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget)
		{
			if(pkUnit->IsTarget(pkTarget, false, ESTARGET_ENEMY))
			{
				int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_NORMAL);
				pkTarget->AddEffectQueue(kData);

				int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkTarget);
				int const iNowHP = pkTarget->GetAbil(AT_HP);
				int iNewHP = __max(0, iNowHP + iAddHP);
				int const iDelta = iNewHP - iNowHP;

				BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
				kPacket.Push(pkTarget->GetID());
				kPacket.Push((short)AT_HP);
				kPacket.Push(iNewHP);
				kPacket.Push(pkEffect->GetCaster());
				kPacket.Push(pkEffect->GetEffectNo());
				kPacket.Push(iDelta);
				pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);

				OnSetAbil(pkTarget, AT_HP, iNewHP);
				if(0 == iNewHP)
				{
					pkUnit->SetTarget(pkEffect->GetCaster());
				}

				++iCount;

				if(iCount >= iMaxTarget)
				{
					break;
				}
			}
		}

		++target_itor;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgSmokeGranadeActiveEffectFunction - 스모크 그레네이드 발동 
///////////////////////////////////////////////////////////
void PgSmokeGranadeActiveEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgSmokeGranadeActiveEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ;
	}

	int const iAuraEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	VEC_GUID& rkTargetList = pkEffect->GetTargetList();

	VEC_GUID::const_iterator target_itor = rkTargetList.begin();
	while(rkTargetList.end() != target_itor)
	{		
		CUnit* pkTarget = pkGround->GetUnit((*target_itor));
		if(pkTarget)
		{
			//pkTarget->DeleteEffect(iAuraEffectNo);
			EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
			pkTarget->AddEffectQueue(kData);
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgSmokeGranadeActiveEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	int const iDetectRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);

	UNIT_PTR_ARRAY kUnitArray;
	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
	if(pkEntity)
	{
		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_PLAYER );
		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_MONSTER );
	}
		
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{			
		//Effect의 Target List에 없으면
		if(!pkEffect->IsTarget((*target_itor).pkUnit->GetID()))
		{
			CUnit* pkTarget = (*target_itor).pkUnit;
			if(pkTarget)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);

				pkEffect->AddTarget(pkTarget->GetID());

//				INFO_LOG(BM::LOG_LV6, _T("[%s][%d] AddTarget %s "), __FUNCTIONW__, pkEffect->GetEffectNo(), pkTarget->Name().c_str());
			}
		}

		++target_itor;
	}

	//이펙트에 걸려있는 타겟들
	VEC_GUID& rkTargtList = pkEffect->GetTargetList();

	if(!rkTargtList.empty())
	{
		VEC_GUID::iterator target_guid_itor = rkTargtList.begin();
		while(target_guid_itor != rkTargtList.end())
		{
			bool bFind = false;
			//새로 잡은 타겟 리스트에 이펙트에 걸려있는 유저가 있는지 검색
			for(UNIT_PTR_ARRAY::const_iterator unit_itor = kUnitArray.begin(); unit_itor != kUnitArray.end(); ++unit_itor)
			{
				if((*unit_itor).pkUnit)
				{
					if((*unit_itor).pkUnit->GetID() == (*target_guid_itor))
					{
						bFind = true;
						break;
					}
				}
			}

			//범위를 벗어났으면 제거
			if(!bFind)
			{
				CUnit* pkTarget = pkGround->GetUnit(*target_guid_itor);
				if(pkTarget)
				{
					//pkTarget->DeleteEffect(iEffectNo);
					EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
					pkUnit->AddEffectQueue(kData);
				}
				else
				{
					INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot Find Unit["<<(*target_guid_itor)<<L"], EffectNo="<<pkEffect->GetEffectNo());
				}
				target_guid_itor = rkTargtList.erase(target_guid_itor);
			}
			else
			{
				++target_guid_itor;
			}
		}
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgAcidArrowEffectFunction - 애시드 애로우
///////////////////////////////////////////////////////////
void PgAcidArrowEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE, pkEffect->GetAbil(AT_R_PHY_DEFENCE));
}

void PgAcidArrowEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE, -pkEffect->GetAbil(AT_R_PHY_DEFENCE));
}

int PgAcidArrowEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int const iNewHP = __max(0, iNowHP + iAddHP);
	int const iDelta = iNewHP - iNowHP;

	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkUnit->GetID());
	kPacket.Push((short)AT_HP);
	kPacket.Push(iNewHP);
	kPacket.Push(pkEffect->GetCaster());
	kPacket.Push(pkEffect->GetEffectNo());
	kPacket.Push(iDelta);
	pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);

	OnSetAbil(pkUnit, AT_HP, iNewHP);

	if(0 == iNewHP)
	{
		pkUnit->SetTarget(pkEffect->GetCaster());
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}
