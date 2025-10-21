#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_Fighter.h"
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
//  PgPaladinAura - Effect Function 성기사 오라
///////////////////////////////////////////////////////////
void PgPaladinAura::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}
void PgPaladinAura::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
		CUnit* pkTarget = pkGround->GetUnit(*target_itor);
		if(pkTarget)
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
			pkTarget->AddEffectQueue(kData);
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgPaladinAura::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	int const iAuraEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);

	if(-iNeedMp > iNowMp
		&& UT_ENTITY != pkUnit->UnitType()
		)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}	

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		//맵이동시 Ground는 NULL이 되는데, 맵 이동 후에도 버프는 걸려 있어야 한다.
		//INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_DOTICK;
	}

	POINT3 const &kCasterPos = pkUnit->GetPos();
	int const iDetectRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	
	CUnit const* const pkOrigUnit = pkUnit;
	if(CUnit* pkCaller = pkGround->GetUnit(pkUnit->Caller()))
	{
		pkUnit = pkCaller;
	}

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_PARTY | ESTARGET_SELF); // 파티는 나를 제외한 파티원을 얻어온다.

	if(!kUnitArray.empty())
	{
		UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin();
		while(kUnitArray.end() != itor)
		{
			CUnit* pkTarget = (*itor).pkUnit;
			if(pkTarget)
			{
				//Effect의 Target List에 없으면
				if(!pkEffect->IsTarget(pkTarget->GetID()))
				{
					//Effect의 사정거리 안에 들어 오면 추가
					if(ERange_OK==::IsInRange(kCasterPos, pkTarget->GetPos(), iDetectRange, static_cast<int>(AI_Z_LIMIT)))
					{
						SActArg kArg = *pkArg;
						kArg.Set(ACTARG_TOGGLESKILL, 0);
						EffectQueueData kData(EQT_ADD_EFFECT, iAuraEffectNo, 0, &kArg, pkUnit->GetID());
						pkTarget->AddEffectQueue(kData);

						pkEffect->AddTarget(pkTarget->GetID());
					}
				}
				//있으면
				else
				{
					//Effect의 사정거리를 벗어나면 삭제
					if(ERange_OK!=::IsInRange(kCasterPos, pkTarget->GetPos(), iDetectRange, static_cast<int>(AI_Z_LIMIT)))
					{
						pkEffect->DeleteTarget(pkTarget->GetID());						
						EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
						pkTarget->AddEffectQueue(kData);
					}
					//Effect의 사정거리 안에 있으면
					else
					{
						// 이미 Target 에 있는데 또 Add 해주어야 하나?
						//	이유는 이미 더 높은 Level의 Effect가 존재했다면, TargetList에 이미 존재하지만
						//	실제로 Effect가 높은 Level의 Effect만 존재한다.
						//	이때, 높은 Effect의 효과가 사라지면, 낮은 Level의 Effect 효과를 나타내 주기 위해
						//	계속적으로 Add 해주고 있다.
						bool bAddEffect = false;
						CEffect const* pkTargetEffect = pkTarget->GetEffect(iAuraEffectNo, true);
						//이펙트를 못 찾으면 추가
						if(NULL == pkTargetEffect)
						{
							bAddEffect = true;
						}
						else
						{
							int const iAddEffectLevel = CEffect::GetLevel(iAuraEffectNo);
							// 새로 걸어야 하는 이펙트가 이전에 걸려있는 이펙트보다 높은 레벨의 이펙트인 경우에 추가한다.
							if(iAddEffectLevel > pkTargetEffect->Level())
							{
								bAddEffect = true;
							}
						}

						if(bAddEffect)
						{
							SActArg kArg = *pkArg;
							kArg.Set(ACTARG_TOGGLESKILL, 0);
							EffectQueueData kData(EQT_ADD_EFFECT, iAuraEffectNo, 0, &kArg, pkUnit->GetID());

							pkTarget->AddEffectQueue(kData);
						}
					}
				}
			}
			
			++itor;
		}
	}
	else
	{
		if(!pkEffect->IsTarget(pkUnit->GetID()))
		{
			SActArg kArg = *pkArg;
			kArg.Set(ACTARG_TOGGLESKILL, 0);
			EffectQueueData kData(EQT_ADD_EFFECT, iAuraEffectNo, 0, &kArg, pkUnit->GetID());
						
			pkUnit->AddEffectQueue(kData);

			pkEffect->AddTarget(pkUnit->GetID());
		}
	}

	if(UT_ENTITY != pkOrigUnit->UnitType())
	{
		pkUnit->SetAbil(AT_MP, iNeedMp + iNowMp, true, true);
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgRevengeEffectFunction - Revenge
///////////////////////////////////////////////////////////
void PgRevengeEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_DMG_REFLECT_HITRATE,	pkEffect->GetAbil(AT_DMG_REFLECT_HITRATE));
	OnAddAbil(pkUnit, AT_DMG_REFLECT_RATE,		pkEffect->GetAbil(AT_DMG_REFLECT_RATE));
}

void PgRevengeEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_DMG_REFLECT_HITRATE,	-pkEffect->GetAbil(AT_DMG_REFLECT_HITRATE));
	OnAddAbil(pkUnit, AT_DMG_REFLECT_RATE,		-pkEffect->GetAbil(AT_DMG_REFLECT_RATE));
}

int PgRevengeEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	int const iNeedMP = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	if(iNeedMP + iNowMp < 0)
	{
		return ECT_MUSTDELETE;
	}

	OnSetAbil(pkUnit, AT_MP, iNowMp + iNeedMP);

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAnnihilationEffectFunction - Annihilation
///////////////////////////////////////////////////////////
void PgAnnihilationEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnSetAbil(pkUnit, AT_1ST_ATTACK_ADDED_RATE, pkEffect->GetAbil(AT_R_PHY_ATTACK));
	OnSetAbil(pkUnit, AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, pkEffect->GetAbil(AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM));
	OnAddAbil(pkUnit, AT_CRITICAL_SUCCESS_VALUE, pkEffect->GetAbil(AT_CRITICAL_SUCCESS_VALUE));
}

void PgAnnihilationEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnSetAbil(pkUnit, AT_1ST_ATTACK_ADDED_RATE, 0);
	OnSetAbil(pkUnit, AT_1ST_ATTACK_ADDED_RATE_APPLY_NUM, 0);
	OnAddAbil(pkUnit, AT_CRITICAL_SUCCESS_VALUE, -pkEffect->GetAbil(AT_CRITICAL_SUCCESS_VALUE));
}

int PgAnnihilationEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(0 == pkUnit->GetAbil(AT_1ST_ATTACK_ADDED_RATE))
	{
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgHolyArmorEffectFunction - HolyArmor
///////////////////////////////////////////////////////////
void PgHolyArmorEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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

	POINT3 const kCasterPos = pkUnit->GetPos();
	int const iAuraEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	int const iDetectRange = pkEffect->GetAbil(AT_DETECT_RANGE);

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, ESTARGET_PARTY); // 파티는 나를 제외한 파티원을 얻어온다.
	if(!kUnitArray.empty())
	{
		UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin();
		while(kUnitArray.end() != itor)
		{
			CUnit* pkTarget = (*itor).pkUnit;
			if(pkTarget)
			{
				//Effect의 Target List에 없으면
				if(!pkEffect->IsTarget(pkTarget->GetID()))
				{
					//Effect의 사정거리 안에 들어 오면 추가
					if(ERange_OK==::IsInRange(kCasterPos, pkTarget->GetPos(), iDetectRange, static_cast<int>(AI_Z_LIMIT)))
					{
						SActArg kArg = *pkArg;
						kArg.Set(ACTARG_TOGGLESKILL, 0);
						EffectQueueData kData(EQT_ADD_EFFECT, iAuraEffectNo, 0, &kArg, pkUnit->GetID());
						
						pkTarget->AddEffectQueue(kData);

						pkEffect->AddTarget(pkTarget->GetID());
						pkTarget->SetDeliverDamageUnit(pkUnit->GetID());
					}
				}
			}
			++itor;
		}
	}
}

void PgHolyArmorEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
		CUnit* pkTarget = pkGround->GetUnit(*target_itor);
		if(pkTarget)
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
			pkTarget->AddEffectQueue(kData);
			pkTarget->SetDeliverDamageUnit(BM::GUID::NullData());
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgHolyArmorEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iNeedMp = pkEffect->GetAbil(AT_MP);
	int const iNowMp = pkUnit->GetAbil(AT_MP);

	int const iAuraEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);

	if(-iNeedMp > iNowMp)
	{
		//INFO_LOG(BM::LOG_LV9, _T("[%s][%d] NeedMP : %d / MP : %d"), __FUNCTIONW__, pkEffect->GetEffectNo(), iNeedMp, iNowMp);
		return ECT_MUSTDELETE;
	}	

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		//맵이동시 Ground는 NULL이 되는데, 맵 이동 후에도 버프는 걸려 있어야 한다.
		//INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_DOTICK;
	}

	POINT3 const kCasterPos = pkUnit->GetPos();
	int const iDetectRange = pkEffect->GetAbil(AT_DETECT_RANGE);

	UNIT_PTR_ARRAY kNewUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kNewUnitArray, ESTARGET_PARTY); // 파티를 얻어오면 나는 제외 된 파티원을 얻어오게 되어있다. 나를 타겟에 넣고싶을땐 SELF 포함해야한다.

	//사정거리를 벗어난 파티원은 제거 한다.
	UNIT_PTR_ARRAY::iterator itor = kNewUnitArray.begin();
	while(kNewUnitArray.end() != itor)
	{
		CUnit* pkTarget = (*itor).pkUnit;
		if(pkTarget)
		{
			//Effect의 사정거리를 벗어나면 삭제 죽어도 삭제
			if(ERange_OK!=::IsInRange(kCasterPos, pkTarget->GetPos(), iDetectRange, 250) || pkTarget->IsDead())
			{
				pkEffect->DeleteTarget(pkTarget->GetID());
				EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
				pkTarget->AddEffectQueue(kData);
				pkTarget->SetDeliverDamageUnit(BM::GUID::NullData());
			}
		}
		else //타겟을 찾을 수없으면 맵에서 벗어난 것
		{
			pkEffect->DeleteTarget(pkTarget->GetID());
		}
		++itor;
	}

	//파티를 벗어난 유저도 버프를 제거 한다.
	VEC_GUID& rkTargetList = pkEffect->GetTargetList();

	VEC_GUID::iterator target_itor = rkTargetList.begin();
	while(rkTargetList.end() != target_itor)
	{
		bool bDeleteUnit = false;
		for(UNIT_PTR_ARRAY::iterator findItor = kNewUnitArray.begin(); findItor != kNewUnitArray.end(); ++findItor)
		{
			CUnit* pkFindTarget = (*findItor).pkUnit;
			if(pkFindTarget)
			{
				// 타겟에 있는데 실제로는 이펙트를 가지고 있지 않을경우 삭제
				CEffect* pkEffect = pkFindTarget->GetEffect(iAuraEffectNo);
				if(NULL == pkEffect)
				{
					bDeleteUnit = false;
					break;
				}
				// 만일 가지고 있다면
				else
				{
					if(pkFindTarget->GetID() == (*target_itor))
					{
						bDeleteUnit = true;
						break;
					}
				}
			}
		}

		//새로 잡은 유닛 리스트에서 찾지 못한 경우는 파티를 벗어난 경우다.
		if(false == bDeleteUnit)
		{
			CUnit* pkTarget = pkGround->GetUnit(*target_itor);
			if(pkTarget)
			{
				CEffect* pkEffect = pkTarget->GetEffect(iAuraEffectNo);
				if(pkEffect)
				{
					EffectQueueData kData(EQT_DELETE_EFFECT, iAuraEffectNo);
					pkTarget->AddEffectQueue(kData);
				}
				pkTarget->SetDeliverDamageUnit(BM::GUID::NullData());
			}

			target_itor = rkTargetList.erase(target_itor);
			continue;
		}

		++target_itor;
	}

	//성스러운 오라를 받고 있는 사람이 없으면 삭제
	if(0 == pkEffect->GetTargetListSize())
	{
		return ECT_MUSTDELETE;
	}

	pkUnit->SetAbil(AT_MP, iNeedMp + iNowMp, true, true);

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAuraTargetEffectFunction - AuraTarget
///////////////////////////////////////////////////////////
int PgAuraTargetEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
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

	CUnit* pkCaster = NULL;
	if(BM::GUID::IsNotNull(pkUnit->GetDeliverDamageUnit())) //성스러운 보호 일경우
	{
		pkCaster = pkGround->GetUnit(pkUnit->GetDeliverDamageUnit());
	}
	else //다른 오라 일 경우
	{
		pkCaster = pkGround->GetUnit(pkEffect->GetCaster());
	}

	//캐스터를 찾을 수없다면 
	if(!pkCaster) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkCaster is NULL"));
		return ECT_MUSTDELETE;
	}

	//캐스터가 사망 한 경우
	if(pkCaster && pkCaster->IsDead())
	{
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgBloodyLoreEffectFunction - a_Bloody Lore
///////////////////////////////////////////////////////////
void PgBloodyLoreEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MAX, pkEffect->GetAbil(AT_R_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MIN, pkEffect->GetAbil(AT_R_PHY_ATTACK_MIN));
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE, pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP, pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_PHY_DMG_PER2, pkEffect->GetAbil(AT_PHY_DMG_PER2));
}

void PgBloodyLoreEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MAX, -pkEffect->GetAbil(AT_R_PHY_ATTACK_MAX));
	OnAddAbil(pkUnit, AT_R_PHY_ATTACK_MIN, -pkEffect->GetAbil(AT_R_PHY_ATTACK_MIN));
	OnAddAbil(pkUnit, AT_R_PHY_DEFENCE, -pkEffect->GetAbil(AT_R_PHY_DEFENCE));
	OnAddAbil(pkUnit, AT_CANNOT_EQUIP, -pkEffect->GetAbil(AT_CANNOT_EQUIP));
	OnAddAbil(pkUnit, AT_PHY_DMG_PER2, -pkEffect->GetAbil(AT_PHY_DMG_PER2));
}

int PgBloodyLoreEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(pkUnit->GetAbil(AT_HP) < (pkUnit->GetAbil(AT_C_MAX_HP) * 0.1f))
	{
		return ECT_MUSTDELETE;
	}

/*	//HP를 절대치로 깍는다.
	int const iAddHP = pkEffect->GetAbil(AT_HP);
	if(iAddHP)
	{
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		int iNewHP = __max(0, iNowHP + iAddHP);

		BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
		kPacket.Push(pkUnit->GetID());
		kPacket.Push((short)AT_HP);
		kPacket.Push(iNewHP);
		kPacket.Push(pkEffect->GetCaster());
		kPacket.Push(pkEffect->GetEffectNo());
		pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);

		OnSetAbil(pkUnit, AT_HP, iNewHP);

		if(0 == iNewHP)
		{
			pkUnit->SetTarget(pkEffect->GetCaster());
			return ECT_MUSTDELETE;
		}
	}
*/ // 밑에 코드 사용 (%로 바뀜)

	//최대 HP의 %로 HP를 깍는다.
	int const iAddHP2 = pkEffect->GetAbil(AT_R_MAX_HP);
	if(iAddHP2)
	{
		int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
		int const iNowHP = pkUnit->GetAbil(AT_HP);
		int iNewHP = static_cast<int>(static_cast<float>(iMaxHP) * (static_cast<float>(iAddHP2) / ABILITY_RATE_VALUE_FLOAT));
		iNewHP = __max(0, iNowHP + iNewHP);
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
	}
	
	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgTauntEffectFunction - 도발 
///////////////////////////////////////////////////////////
int PgTauntEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if( !pkGround || !pkUnit || !pkEffect )
	{
		return ECT_DOTICK;
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
		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, pkGround);

		int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
		int iCount = 0;

		int const iTargetAddEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);

		GET_DEF( CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(pkEffect->GetEffectNo());

		UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();
		while(kTargetArray.end() != unit_itor)
		{
			CUnit* pkTarget = (*unit_itor).pkUnit;
			if(pkUnit->IsTarget(pkTarget, true, ESTARGET_ENEMY, pkSkillDef))
			{
				pkTarget->AddEffect( iTargetAddEffectNo, 0, &kArg, pkUnit, EFFECT_TYPE_CURSED );
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