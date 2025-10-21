#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_StatusEffect.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"

///////////////////////////////////////////////////////////
//  PgStunStatusFunction - 기절 (상태이상)
///////////////////////////////////////////////////////////
void PgStunStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,	pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_FROZEN,		pkEffect->GetAbil(AT_FROZEN));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,	pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,	pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,pkEffect->GetAbil(AT_CANNOT_USEITEM));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,pkEffect->GetAbil(AT_CANNOT_CASTSKILL));

	// Damage 받아도 깨어 날 수 없다.
	OnSetAbil(pkUnit, AT_FROZEN_DMG_WAKE, 0);
}

void PgStunStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_FROZEN,			-pkEffect->GetAbil(AT_FROZEN));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,		-pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,		-pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,	-pkEffect->GetAbil(AT_CANNOT_USEITEM));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,	-pkEffect->GetAbil(AT_CANNOT_CASTSKILL));

	// Damage 받아도 깨어 날 수 없다.
	OnSetAbil(pkUnit, AT_FROZEN_DMG_WAKE, 1);
}

int PgStunStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgElgaStunStatusFunction - 엘가기절 (상태이상)
///////////////////////////////////////////////////////////
//AT_MON_SKILL_MONSTER_NO			= 3322,		//맵앤티티 번호(심장의 방 진입을 관리하는 엔티티)
//AT_MON_SKILL_TARGET_NO			= 3325,		//심장몬스터 번호
//AT_MON_SKILL_01					= 3010,		//심장스킬, 그냥 내쫓기
//AT_MON_SKILL_02					= 3011,		//심장스킬, 심장 폭발 내쫓기

bool IsElgaStateGood(CUnit * pkUnit, CEffect* pkEffect)
{
	if(!pkUnit || !pkEffect)
	{
		return false;
	}

	static int const DEFALUT_SUMMOND_HEART_RATE = 3000;
	int const iHPRate = (pkUnit->GetAbil(AT_HP) / static_cast<float>(pkUnit->GetAbil(AT_C_MAX_HP)))*ABILITY_RATE_VALUE;
	int iRate = pkEffect->GetAbil(AT_ELGA_SUMMOND_HEART_RATE);
	if(0==iRate)
	{
		iRate = DEFALUT_SUMMOND_HEART_RATE;
	}
	return iHPRate > iRate;
}

void PgElgaStunStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	PgStunStatusFunction::EffectBegin(pkUnit, pkEffect, pkArg);
	
	//1. InsertMonster(심장)
	//2. 맵엔티티 밖으로 내보내기 이펙트 일시정지
	
	pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_PARENT);

	//InsertMonster(심장)
	CONT_DEF_MONSTER_BAG_CONTROL const *pContMonBagControl = NULL;
	CONT_DEF_MONSTER_BAG const *pContMonBag = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS const *pContMonBagElement = NULL;
	CONT_DEF_MAP_REGEN_POINT const *pContMapRegenPoint = NULL;
	g_kTblDataMgr.GetContDef(pContMonBagControl );
	g_kTblDataMgr.GetContDef(pContMonBag );
	g_kTblDataMgr.GetContDef(pContMonBagElement );
	g_kTblDataMgr.GetContDef(pContMapRegenPoint );

	int iBag = pkEffect->GetAbil(AT_SKILL_SUMMON_MONBAGCONTROL);
	if(iBag && pContMonBagControl && pContMonBag && pContMonBagElement && pContMapRegenPoint)
	{
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
						return;
					}

					int const iMapNo = pkGround->GetGroundNo();

					CONT_DEF_MAP_REGEN_POINT::const_iterator regen_itor = pContMapRegenPoint->begin();
					while(regen_itor != pContMapRegenPoint->end())
					{//지금맵용 리젠포인트. 셋팅.
						if(iMapNo == regen_itor->second.iMapNo)
						{
							if(iMonBagNo == regen_itor->second.iBagControlNo)
							{
								kGenPoint.pt3Pos = regen_itor->second.pt3Pos;
								kGenPoint.cDirection = regen_itor->second.cDirection;
								pkGround->InsertMonster( kGenPoint, kElement.aElement[iIndex], kMonGuid, pkUnit);

								if(CUnit * pkChild = pkGround->GetUnit(kMonGuid))
								{
									pkChild->SetAbil(AT_HP, pkUnit->GetAbil(AT_HP));
									pkChild->SetAbil(AT_MAX_HP, pkUnit->GetAbil(AT_MAX_HP));
									pkChild->SetAbil(AT_C_MAX_HP, pkUnit->GetAbil(AT_C_MAX_HP));

									pkUnit->AddSummonUnit(kMonGuid, pkChild->GetAbil(AT_CLASS));
									pkChild->Caller(pkUnit->GetID());
									pkChild->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_CHILDREN);
								}

								++iIndex;
								if( !kElement.aElement[iIndex] )
								{
									break;
								}
							}
						}
						++regen_itor;
					}
				}
			}
		}
	}

	//맵엔티티 밖으로 내보내기 이펙트 일시정지
	int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(iAddEffect)
	{
		int const iMapEntity = pkEffect->GetAbil(AT_MON_SKILL_MONSTER_NO);
		UNIT_PTR_ARRAY kUnitArray;
		pkGround->GetEntity(BM::GUID::NullData(), iMapEntity, kUnitArray);
		 
		UNIT_PTR_ARRAY::iterator unit_itor = kUnitArray.begin();
		while(kUnitArray.end() != unit_itor)
		{
			if(unit_itor->pkUnit)
			{
				unit_itor->pkUnit->AddEffect(iAddEffect, 0, pkArg, pkUnit);
			}

			++unit_itor;
		}
	}
}

void PgElgaStunStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	PgStunStatusFunction::EffectEnd(pkUnit, pkEffect, pkArg);

	pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_NONE);

	int const iHeartMon = pkEffect->GetAbil(AT_MON_SKILL_TARGET_NO);
	VEC_SUMMONUNIT kContSummonUnit = pkUnit->GetSummonUnit();
	CUnit * pkChild = NULL;
	for(VEC_SUMMONUNIT::const_iterator c_it=kContSummonUnit.begin(); c_it!=kContSummonUnit.end(); ++c_it)
	{
		pkChild = pkGround->GetUnit((*c_it).kGuid);
		if(pkChild && pkChild->IsUnitType(UT_MONSTER) && pkChild->GetAbil(AT_CLASS)==iHeartMon)
		{
			pkChild->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_NONE);
			pkUnit->DeleteSummonUnit(pkChild->GetID());
		}
	}
	

	//1. 맵엔티티 밖으로 내보내기 이펙트 일시정지 해제(이건 심장이 스킬을 발동하고 나서 풀어주는 형태로 바꿔야겠다)
	//2. 심장에게 주변 캐릭터 죽이기 스킬발동, 심장은 사라짐
	//3. 뼈 소환

	//맵엔티티 밖으로 내보내기 이펙트 일시정지 해제
	/*int const iMapEntity = pkEffect->GetAbil(AT_MON_SKILL_MONSTER_NO);
	CUnit * pkMapEntity = pkGround->GetUnitByClassNo(iMapEntity, UT_ENTITY);
	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(iMapEntity && pkMapEntity && iEffectNo)
	{
		pkMapEntity->DeleteEffect(iEffectNo);
	}*/

	//심장에게 주변 캐릭터 죽이기 스킬발동
	CUnit * pkHeartMon = pkGround->GetUnitByClassNo(iHeartMon, UT_MONSTER);
	int const iSkillNo = pkEffect->GetAbil( IsElgaStateGood(pkUnit, pkEffect) ? AT_MON_SKILL_01 : AT_MON_SKILL_02);
	if(pkHeartMon && iSkillNo)
	{
		pkHeartMon->SetAbil(AT_CANNOT_DAMAGE, 1, true, true);
		pkHeartMon->GetSkill()->ForceReserve(iSkillNo);
		pkHeartMon->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}

	//뼈 소환
	if(int const iForceSkillNo = pkEffect->GetAbil(AT_MON_ADD_FORCESKILL))
	{
		pkUnit->GetSkill()->ForceReserve(iForceSkillNo,EFSF_NOW_FIRESKILL_CANCLE);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}
}

int PgElgaStunStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return PgStunStatusFunction::EffectTick(pkUnit, pkEffect, pkArg, dwElapsed);
}

///////////////////////////////////////////////////////////
//  PgBleedStatusFunction - 출혈 (상태이상)
///////////////////////////////////////////////////////////
void PgBleedStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_HP, pkEffect->GetAbil(AT_R_MAX_HP));

	int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int iNewHP = __min(iNowHP, iMaxHP);

	OnSetAbil(pkUnit, AT_HP, iNewHP);
}

void PgBleedStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_HP, -pkEffect->GetAbil(AT_R_MAX_HP));
}

int PgBleedStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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
//  PgCurseStatusFunction - 저주 (상태이상)
///////////////////////////////////////////////////////////
void PgCurseStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_STR, pkEffect->GetAbil(AT_R_STR));
	OnAddAbil(pkUnit, AT_R_INT, pkEffect->GetAbil(AT_R_INT));
	OnAddAbil(pkUnit, AT_R_CON, pkEffect->GetAbil(AT_R_CON));
	OnAddAbil(pkUnit, AT_R_DEX, pkEffect->GetAbil(AT_R_DEX));
	OnAddAbil(pkUnit, AT_R_MOVESPEED,		pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	pkEffect->GetAbil(AT_R_ATTACK_SPEED));

	int iAdd = pkEffect->GetAbil(AT_R_PHY_ATTACK_MAX);
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MAX, iAdd);
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MIN, iAdd);
	iAdd = pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX);
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MAX, iAdd);
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MIN, iAdd);
}	

void PgCurseStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_STR, -pkEffect->GetAbil(AT_R_STR));
	OnAddAbil(pkUnit, AT_R_INT, -pkEffect->GetAbil(AT_R_INT));
	OnAddAbil(pkUnit, AT_R_CON, -pkEffect->GetAbil(AT_R_CON));
	OnAddAbil(pkUnit, AT_R_DEX, -pkEffect->GetAbil(AT_R_DEX));
	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		-pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	-pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	-pkEffect->GetAbil(AT_R_ATTACK_SPEED));

	int iAdd = pkEffect->GetAbil(AT_R_PHY_ATTACK_MAX);
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MAX, -iAdd);
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MIN, -iAdd);
	iAdd = pkEffect->GetAbil(AT_R_MAGIC_ATTACK_MAX);
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MAX, -iAdd);
	OnAddAbil(pkUnit, AT_R_MAGIC_ATTACK_MIN, -iAdd);
}

int PgCurseStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgColdStatusFunction - 결빙 (상태이상)
///////////////////////////////////////////////////////////
void PgColdStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MOVESPEED,		pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	pkEffect->GetAbil(AT_R_ATTACK_SPEED));
}

void PgColdStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		-pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	-pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	-pkEffect->GetAbil(AT_R_ATTACK_SPEED));
}

int PgColdStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgSilenceStatusFunction - 침묵 (상태이상)
///////////////////////////////////////////////////////////
void PgSilenceStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	pkEffect->GetAbil(AT_R_ATTACK_SPEED));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,	pkEffect->GetAbil(AT_CANNOT_CASTSKILL));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,	pkEffect->GetAbil(AT_CANNOT_ATTACK));
}

void PgSilenceStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	-pkEffect->GetAbil(AT_R_ATTACK_SPEED));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,	-pkEffect->GetAbil(AT_CANNOT_CASTSKILL));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,		-pkEffect->GetAbil(AT_CANNOT_ATTACK));
}

int PgSilenceStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgPoisonStatusFunction - 중독 (상태이상)
///////////////////////////////////////////////////////////
void PgPoisonStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgPoisonStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgPoisonStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
	int const iNowHP = pkUnit->GetAbil(AT_HP);
	int iNewHP = __max(1, iNowHP + iAddHP);
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

	// HP가 1이면 중단
	if(1 >= iNewHP)
	{
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgSleepStatusFunction - 수면 (상태이상)
///////////////////////////////////////////////////////////
void PgSleepStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,		pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,	pkEffect->GetAbil(AT_CANNOT_USEITEM));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,	pkEffect->GetAbil(AT_CANNOT_CASTSKILL));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	pkEffect->GetAbil(AT_R_ATTACK_SPEED));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,		pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_FROZEN,			pkEffect->GetAbil(AT_FROZEN));

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

	OnSetAbil(pkUnit, AT_HP_BACKUP, pkUnit->GetAbil(AT_HP));
}

void PgSleepStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP,		-pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_CANNOT_USEITEM,	-pkEffect->GetAbil(AT_CANNOT_USEITEM));
	OnAddAbil(pkUnit, AT_CANNOT_CASTSKILL,	-pkEffect->GetAbil(AT_CANNOT_CASTSKILL));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	-pkEffect->GetAbil(AT_R_ATTACK_SPEED));
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK,		-pkEffect->GetAbil(AT_CANNOT_ATTACK));
	OnAddAbil(pkUnit, AT_FROZEN,			-pkEffect->GetAbil(AT_FROZEN));

	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL, pkUnit->GetAbil(AT_ENABLE_AUTOHEAL_BACKUP));
	OnSetAbil(pkUnit, AT_ENABLE_AUTOHEAL_BACKUP, 0);
	OnSetAbil(pkUnit, AT_HP_BACKUP, 0);
}

int PgSleepStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(pkEffect->GetAbil(AT_FROZEN_DMG_WAKE))
	{
		//데미지를 입었다면 수면상태가 해제 된다.
		if(pkUnit->GetAbil(AT_HP_BACKUP) > pkUnit->GetAbil(AT_HP))
		{
			return ECT_MUSTDELETE;
		}
	}
	//INFO_LOG(BM::LOG_LV0, _T("[%s] Don't call me Tick EffectNo[%d] "), __FUNCTIONW__, pkEffect->GetEffectNo());
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgFleshtoStoneStatusFunction - 석화 (상태이상)
///////////////////////////////////////////////////////////
void PgFleshtoStoneStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
}

void PgFleshtoStoneStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE,		-pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_R_MAGIC_DEFENCE,	-pkEffect->GetAbil(AT_R_MAGIC_DEFENCE));
}

int PgFleshtoStoneStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBurnStatusFunction - 화상 (상태이상)
///////////////////////////////////////////////////////////
void PgBurnStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_MP, pkEffect->GetAbil(AT_R_MAX_MP));

	int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);
	int const iNowMP = pkUnit->GetAbil(AT_MP);
	int iNewMP = __min(iNowMP, iMaxMP);

	OnSetAbil(pkUnit, AT_MP, iNewMP);
}

void PgBurnStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MAX_MP, -pkEffect->GetAbil(AT_R_MAX_MP));
}

int PgBurnStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	//MP -
	{
		int const iAddMP = pkEffect->GetAbil(AT_MP);
		int const iNowMP = pkUnit->GetAbil(AT_MP);
		int iNewMP = __max(0, iNowMP + iAddMP);
		int const iDelta = iNewMP - iNowMP;
		BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
		kPacket.Push(pkUnit->GetID());
		kPacket.Push((short)AT_MP);
		kPacket.Push(iNewMP);
		kPacket.Push(pkEffect->GetCaster());
		kPacket.Push(pkEffect->GetEffectNo());
		kPacket.Push(iDelta);
		pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);

		OnSetAbil(pkUnit, AT_MP, iNewMP);
	}

	//HP -
	{
		int const iAddHP = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		int iNewHP = __max(0, iNowHP + iAddHP);
		int const iDelta = iNewHP - iNowHP;
		BM::Stream kPacket2(PT_M_C_NFY_ABILCHANGED);
		kPacket2.Push(pkUnit->GetID());
		kPacket2.Push((short)AT_HP);
		kPacket2.Push(iNewHP);
		kPacket2.Push(pkEffect->GetCaster());
		kPacket2.Push(pkEffect->GetEffectNo());
		kPacket2.Push(iDelta);
		pkUnit->Send(kPacket2, E_SENDTYPE_BROADALL);

		OnSetAbil(pkUnit, AT_HP, iNewHP);
		if(0 == iNewHP)
		{
			pkUnit->SetTarget(pkEffect->GetCaster());
			return ECT_MUSTDELETE;
		}
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBlindStatusFunction - 실명 (상태이상)
///////////////////////////////////////////////////////////
void PgBlindStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_HIT_SUCCESS_VALUE,		pkEffect->GetAbil(AT_R_HIT_SUCCESS_VALUE));
	OnAddAbil(pkUnit, AT_R_DODGE_SUCCESS_VALUE,	pkEffect->GetAbil(AT_R_DODGE_SUCCESS_VALUE));
}

void PgBlindStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_HIT_SUCCESS_VALUE,		-pkEffect->GetAbil(AT_R_HIT_SUCCESS_VALUE));
	OnAddAbil(pkUnit, AT_R_DODGE_SUCCESS_VALUE,	-pkEffect->GetAbil(AT_R_DODGE_SUCCESS_VALUE));
}

int PgBlindStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV0, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgSlowStatusFunction - 슬로우 (상태이상)
///////////////////////////////////////////////////////////
void PgSlowStatusFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg)
{
	INFO_LOG(BM::LOG_LV9, __FL__<<L"EffectNo="<<pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	pkEffect->GetAbil(AT_R_ATTACK_SPEED));
}

void PgSlowStatusFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg)
{
	INFO_LOG(BM::LOG_LV9, __FL__<<L"EffectNo="<<pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_MOVESPEED,		-pkEffect->GetAbil(AT_R_MOVESPEED));
	OnAddAbil(pkUnit, AT_R_ATTACK_SPEED,	-pkEffect->GetAbil(AT_R_ATTACK_SPEED));
}

int PgSlowStatusFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgMissionPenaltyFunction
///////////////////////////////////////////////////////////
void PgMissionPenaltyFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int iPenaltyHP = pkEffect->GetAbil(AT_PENALTY_HP_RATE);
	if (0!=iPenaltyHP)
	{
		int iHP = pkUnit->GetAbil(AT_HP);
		iHP+=(int)((float)(iHP*iPenaltyHP)*0.0001f);
		pkUnit->SetAbil(AT_HP, iHP, true, false);
	}

	int iPenaltyMP = pkEffect->GetAbil(AT_PENALTY_MP_RATE);
	if (0!=iPenaltyMP)
	{
		int iMP = pkUnit->GetAbil(AT_MP);
		iMP+=(int)((float)(iMP*iPenaltyMP)*0.0001f);
		pkUnit->SetAbil(AT_MP, iMP, true, false);
	}

	int iPenaltyEXP = pkEffect->GetAbil(AT_ADD_EXPERIENCE_RATE);
	if (0!=iPenaltyEXP)
	{
		pkUnit->AddAbil(AT_ADD_EXPERIENCE_RATE, iPenaltyEXP);
	}
}

void PgMissionPenaltyFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int iPenaltyEXP = pkEffect->GetAbil(AT_ADD_EXPERIENCE_RATE);
	if (0!=iPenaltyEXP)
	{
		pkUnit->AddAbil(AT_ADD_EXPERIENCE_RATE, -iPenaltyEXP);
	}
}
int PgMissionPenaltyFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgItemEffectFunction
///////////////////////////////////////////////////////////
void PgItemEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{ 
	int const iAbilArray[6] = { AT_R_MAX_HP, AT_R_MAX_MP, AT_STR, AT_INT, AT_CON, AT_DEX };
	WORD wType[6] = {0,};
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iCount = 0;
	for(int i = 0; i < 6; ++i)
	{
		int const iAdd = pkEffect->GetAbil(iAbilArray[i]);
		if (0!=iAdd)
		{
			rkMgr.AddAbil(iAbilArray[i], iAdd);
			pkUnit->NftChangedAbil(iAbilArray[i]);
			wType[iCount] = (WORD)iAbilArray[i];
			++iCount;
		}
	}

	if (0 < iCount)
	{
		pkUnit->SendAbiles(wType, iCount, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	}
}

void PgItemEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int const iAbilArray[6] = { AT_R_MAX_HP, AT_R_MAX_MP, AT_STR, AT_INT, AT_CON, AT_DEX };
	WORD wType[6] = {0,};
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iCount = 0;
	for(int i = 0; i < 6; ++i)
	{
		int const iAdd = pkEffect->GetAbil(iAbilArray[i]);
		if (0!=iAdd)
		{
			rkMgr.AddAbil(iAbilArray[i], -iAdd);
			pkUnit->NftChangedAbil(iAbilArray[i]);
			wType[iCount] = (WORD)iAbilArray[i];
			++iCount;
		}
	}

	if (0 < iCount)
	{
		pkUnit->SendAbiles(wType, iCount, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	}
}
int PgItemEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgCannotDamageFunction
///////////////////////////////////////////////////////////
void PgCannotDamageFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
//	int iValue = pkEffect->GetAbil(AT_CANNOT_DAMAGE);
	OnAddAbil( pkUnit, AT_CANNOT_DAMAGE, 1, E_SENDTYPE_NONE );
}

void PgCannotDamageFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
//	int iValue = pkEffect->GetAbil(AT_CANNOT_DAMAGE);
	OnAddAbil( pkUnit, AT_CANNOT_DAMAGE, -1, E_SENDTYPE_NONE );
}
int PgCannotDamageFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgSafetyCapFunction
///////////////////////////////////////////////////////////
void PgSafetyCapFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//공격불가 / 피격불가
	OnAddAbil(pkUnit, AT_CANNOT_DAMAGE, 1);
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK, 1);
}

void PgSafetyCapFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	OnAddAbil(pkUnit, AT_CANNOT_DAMAGE, -1);
	OnAddAbil(pkUnit, AT_CANNOT_ATTACK, -1);
}
int PgSafetyCapFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgDefenceCorrectionFunction
///////////////////////////////////////////////////////////
void PgDefenceCorrectionFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//HP
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();
	int const iAdd = pkEffect->GetAbil(AT_R_MAX_HP);
	rkMgr.AddAbil(AT_R_MAX_HP, iAdd);
	pkUnit->NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);

	int const iLevel = pkUnit->GetAbil(AT_LEVEL);
	int const iHandicap = (int)((5.0f - ((float)iLevel)/50.0f)*1000.0f);
	pkEffect->SetActArg( AT_HANDYCAP, iHandicap );

	//Defence
	if ( 0!=iHandicap )
	{
		rkMgr.AddAbil(AT_R_PHY_DEFENCE, iHandicap);
		rkMgr.AddAbil(AT_R_MAGIC_DEFENCE, iHandicap);
		pkUnit->NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
		pkUnit->NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	}
}

void PgDefenceCorrectionFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();
	int const iAdd = pkEffect->GetAbil(AT_R_MAX_HP) * -1;
	rkMgr.AddAbil(AT_R_MAX_HP, iAdd);
	pkUnit->NftChangedAbil(AT_R_MAX_HP, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);

	//Defence
	int iHandicap = 0;
	pkEffect->GetActArg(AT_HANDYCAP, iHandicap);
	if ( 0!=iHandicap )
	{
		rkMgr.AddAbil(AT_R_PHY_DEFENCE, -iHandicap);
		rkMgr.AddAbil(AT_R_MAGIC_DEFENCE, -iHandicap);
		pkUnit->NftChangedAbil(AT_R_PHY_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
		pkUnit->NftChangedAbil(AT_R_MAGIC_DEFENCE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	}
}
int PgDefenceCorrectionFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgLovePowerFunction
///////////////////////////////////////////////////////////
void PgLovePowerFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_R_STR);
	rkMgr.AddAbil(AT_R_STR, iAdd);
	pkUnit->NftChangedAbil(AT_R_STR, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}

void PgLovePowerFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_R_STR);
	rkMgr.AddAbil(AT_R_STR, -iAdd);
	pkUnit->NftChangedAbil(AT_R_STR, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}
int PgLovePowerFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgReviveEffectFunction - 소생
///////////////////////////////////////////////////////////
void PgReviveEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int const iAddCount = pkEffect->GetAbil(AT_REVIVE_COUNT);
	OnSetAbil2(pkUnit, AT_REVIVE_COUNT,		 iAddCount);
	OnSetAbil2(pkUnit, AT_PREV_REVIVE_COUNT, iAddCount);
	OnSetAbil2(pkUnit, AT_REVIVE_EFFECT,	 pkEffect->GetAbil(AT_EFFECTNUM1));
}

void PgReviveEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	OnSetAbil2(pkUnit, AT_REVIVE_COUNT,		 0);
	OnSetAbil2(pkUnit, AT_PREV_REVIVE_COUNT, 0);
}

int PgReviveEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	int const iValue = pkUnit->GetAbil(AT_REVIVE_COUNT);
	int const iPrevValue = pkUnit->GetAbil(AT_PREV_REVIVE_COUNT);

	//숫자의 변화가 생겼으면
	if(iValue != iPrevValue)
	{
		OnSetAbil(pkUnit, AT_PREV_REVIVE_COUNT, iValue);

	}

	if(0 == iValue)
	{
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAddEffectDurationTimeFunction - 이펙트 유지시간 증감
///////////////////////////////////////////////////////////
EAbilType GetEffectType(BYTE const Type)
{
	return (EAbilType)(Type == EFFECT_TYPE_CURSED ? AT_ADD_CURSED_DURATIONTIME : AT_ADD_BLESSED_DURATIONTIME);
}
void PgAddEffectDurationTimeFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	int const iAddDuration = pkEffect->GetAbil(AT_ADD_EFFECT_DURATIONTIME);
	EAbilType const eType = GetEffectType(pkEffect->GetType());
	OnAddAbil(pkUnit, eType,	iAddDuration, E_SENDTYPE_BROADALL_EFFECTABIL);

	PgUnitEffectMgr & rkEffectMgr = pkUnit->GetEffectMgr();

	ContEffectItor kItor;
	rkEffectMgr.GetFirstEffect(kItor);
	CEffect* pkApplyEffect = NULL;
	while ((pkApplyEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
	{
		if(pkEffect->GetEffectNo() == pkApplyEffect->GetEffectNo())
		{
			continue;
		}

		if(pkEffect->GetType() != pkApplyEffect->GetType())
		{
			continue;
		}

		if(int const iDurationTime = pkApplyEffect->GetDurationTime())
		{
			if( pkApplyEffect->GetTime() > (iDurationTime+iAddDuration) )
			{// 유지시간이 0이하가 되면 이펙트를 삭제
				EffectQueueData kData(EQT_DELETE_EFFECT, pkApplyEffect->GetEffectNo());
				pkUnit->AddEffectQueue(kData);
				continue;
			}

			if(0 != iAddDuration)
			{// 걸린다면 추가해준다
				pkApplyEffect->SetAddDurationTime( pkApplyEffect->GetAddDurationTime() + iAddDuration );
				pkApplyEffect->SetEndTime( pkApplyEffect->GetEndTime() + iAddDuration );

				if(pkGround)
				{
					BM::Stream kPacket(PT_M_C_NFY_ADD_EFFECT_DURATIONTIME);
					kPacket.Push(pkUnit->GetID());
					kPacket.Push(pkApplyEffect->GetEffectNo());
					kPacket.Push(pkApplyEffect->GetEndTime());
					pkGround->Broadcast(kPacket);
				}
			}
		}
	}
}

void PgAddEffectDurationTimeFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int const iAddDuration = pkEffect->GetAbil(AT_ADD_EFFECT_DURATIONTIME);
	EAbilType const eType = GetEffectType(pkEffect->GetType());
	OnAddAbil(pkUnit, eType,	-iAddDuration, E_SENDTYPE_BROADALL_EFFECTABIL);
}
