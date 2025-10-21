#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_Item.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PublicMap/PgWarGround.h"
#include "PgGroundTrigger.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"

///////////////////////////////////////////////////////////
//  PgGasMaskEffectFunction - Effect Function 파렐경 맵 엔티티가 사용하는 이펙트
///////////////////////////////////////////////////////////
void PgGasMaskEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	OnSetAbil(pkUnit, AT_EXCEPT_EFFECT_TICK, pkEffect->GetAbil(AT_EXCEPT_EFFECT_TICK));
}
void PgGasMaskEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	
	OnSetAbil(pkUnit, AT_EXCEPT_EFFECT_TICK, 0);
}

int PgGasMaskEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgPotionEffectFunction - Effect Function Tick당 Hp/mp를 회복시켜주는 포션 아이템
///////////////////////////////////////////////////////////
void PgPotionEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	//이곳에 들어오는 CEffect는 CItemEffect 이다.
	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef)
	{
		int const iAddHP = pkDef->GetAbil(AT_HP);
		int iAddHPCount = pkDef->GetDurationTime() / pkDef->GetInterval();
		int iAddHPAmount = 0;
		{
			//회복되어야 하는 총 량
			iAddHPAmount = iAddHP * iAddHPCount;
			iAddHPAmount = iAddHPAmount + (iAddHPAmount * pkUnit->GetAbil(AT_C_HP_POTION_ADD_RATE) / ABILITY_RATE_VALUE);
			
			int const iCurHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

			//회복되어야 하는 양이 MAX_HP보다 클 경우
			if(iAddHPAmount + pkUnit->GetAbil(AT_HP_RECOVERY_TICK_ITEM_AMOUNT) + iCurHP > iMaxHP)
			{
				//회복되어야 하는 양 = MAX_HP - CUR_HP
				iAddHPAmount = iMaxHP - iCurHP;
				OnSetAbil2(pkUnit, AT_HP_RECOVERY_TICK_ITEM_AMOUNT, iAddHPAmount);
			}
			else
			{
				OnAddAbil(pkUnit, AT_HP_RECOVERY_TICK_ITEM_AMOUNT, iAddHPAmount);
			}
		}

		int const iAddMP = pkDef->GetAbil(AT_MP);
		int iAddMPCount = pkDef->GetDurationTime() / pkDef->GetInterval();			
		int iAddMPAmount = 0;
		{
			//회복되어야 하는 총 량
			iAddMPAmount = iAddMP * iAddMPCount;
			iAddMPAmount = iAddMPAmount + (iAddMPAmount * pkUnit->GetAbil(AT_C_MP_POTION_ADD_RATE) / ABILITY_RATE_VALUE);
			
			int const iCurMP = pkUnit->GetAbil(AT_MP);
			int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

			//회복되어야 하는 양이 MAX_HP보다 클 경우
			if(iAddMPAmount + pkUnit->GetAbil(AT_MP_RECOVERY_TICK_ITEM_AMOUNT) + iCurMP > iMaxMP)
			{
				//회복되어야 하는 양 = MAX_MP - CUR_MP
				iAddMPAmount = iMaxMP - iCurMP;
				OnSetAbil2(pkUnit, AT_MP_RECOVERY_TICK_ITEM_AMOUNT, iAddMPAmount);
			}
			else
			{
				OnAddAbil(pkUnit, AT_MP_RECOVERY_TICK_ITEM_AMOUNT, iAddMPAmount);
			}
			
		}

		//남은 HP 회복량을 저장 한다.
		int const iValue = MAKELONG(__min(iAddHPAmount, 0xFFFF), __min(iAddMPAmount, 0xFFFF));

		pkEffect->SetValue(iValue);
	}
}

void PgPotionEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	int iValue = pkEffect->GetValue();
	int iHPValue = LOWORD(iValue);
	int iMPValue = HIWORD(iValue);

	//남은양이 -로 되는 경우
	if(pkUnit->GetAbil(AT_HP_RECOVERY_TICK_ITEM_AMOUNT) - iHPValue < 0)
	{
		OnSetAbil2(pkUnit, AT_HP_RECOVERY_TICK_ITEM_AMOUNT, 0);
	}
	else
	{
		OnAddAbil(pkUnit, AT_HP_RECOVERY_TICK_ITEM_AMOUNT, -iHPValue);
	}

	if(pkUnit->GetAbil(AT_MP_RECOVERY_TICK_ITEM_AMOUNT) - iMPValue < 0)
	{
		OnSetAbil2(pkUnit, AT_MP_RECOVERY_TICK_ITEM_AMOUNT, 0);
	}
	else
	{
		OnAddAbil(pkUnit, AT_MP_RECOVERY_TICK_ITEM_AMOUNT, -iMPValue);
	}
}

int PgPotionEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef)
	{
		int iValue = pkEffect->GetValue();
		int iHPValue = LOWORD(iValue);
		int iMPValue = HIWORD(iValue);

		//Tick 당 HP / MP 회복 시켜 줄 경우
		int iAddHP = pkDef->GetAbil(AT_HP);
		iAddHP = iAddHP + (iAddHP * pkUnit->GetAbil(AT_C_HP_POTION_ADD_RATE) / ABILITY_RATE_VALUE);
		if(iAddHP)
		{
			int const iNowHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

			if(iMaxHP == iNowHP)
			{
				return ECT_MUSTDELETE;
			}			

			//최대 HP를 넘어가는 경우 더해지는 양이 바뀐다.
			if(iMaxHP < iNowHP + iAddHP)
			{
				iAddHP = iNowHP + iAddHP - iMaxHP;
			}

			int const iNewHP = __min(iMaxHP, iNowHP + iAddHP);

			OnAddAbil(pkUnit, AT_HP_RECOVERY_TICK_ITEM_AMOUNT, -iAddHP);
			iHPValue -= iAddHP;

			OnSetAbil(pkUnit, AT_HP, iNewHP);
		}

		int iAddMP = pkDef->GetAbil(AT_MP);
		iAddMP = iAddMP + (iAddMP * pkUnit->GetAbil(AT_C_MP_POTION_ADD_RATE) / ABILITY_RATE_VALUE);
		if(iAddMP)
		{
			int const iNowMP = pkUnit->GetAbil(AT_MP);
			int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

			if(iMaxMP == iNowMP)
			{
				return ECT_MUSTDELETE;
			}
			
			//최대 MP를 넘어가는 경우 더해지는 양이 바뀐다.
			if(iMaxMP < iNowMP + iAddMP)
			{
				iAddMP = iNowMP + iAddMP - iMaxMP;		
			}

			int const iNewMP = __min(iMaxMP, iNowMP + iAddMP);
			
			OnAddAbil(pkUnit, AT_MP_RECOVERY_TICK_ITEM_AMOUNT, -iAddMP);
			iMPValue -= iAddMP;

			OnSetAbil(pkUnit, AT_MP, iNewMP);
		}

		iValue = MAKELONG(iHPValue, iMPValue);
		pkEffect->SetValue(iValue);
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgGaugeDecreaseValEffectFunction - 틱당 목표 게이지 그룹의 Value를 낮춘다
///////////////////////////////////////////////////////////
void PgGaugeDecreaseValEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//m_kPreCasterPos = pkUnit->GetPos();
}

void PgGaugeDecreaseValEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgGaugeDecreaseValEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	//if( m_kPreCasterPos != pkUnit->GetPos() ) // 움직이면 해제!
	//{
	//	return ECT_MUSTDELETE;
	//}

	POINT3 kPos = pkUnit->GetPos();
	UNIT_PTR_ARRAY kTargetArray;
	int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);
	int const iGaugeGroupNo = pkEffect->GetAbil(AT_ADDED_GAUGE_GROUP);
	int const iTargetGrade = pkEffect->GetAbil(AT_GRADE);
	int const iLimitAxisZ = 30;

	pkGround->GetUnitInRange(kPos, iRange, static_cast< EUnitType >((UT_NONETYPE == iTargetGrade)? UT_ALLUNIT: iTargetGrade), kTargetArray, iLimitAxisZ);
	UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();
	while( kTargetArray.end() != unit_itor )
	{
		CUnit* pkTargetUnit = (*unit_itor).pkUnit;
		if(pkTargetUnit)
		{
			PgUnitEffectMgr const& rkEffectMgr = pkTargetUnit->GetEffectMgr();
			if( iGaugeGroupNo == rkEffectMgr.GetAbil(AT_ADDED_GAUGE_GROUP) )
			{
				int const iGaugeValue = pkTargetUnit->GetAbil(AT_ADDED_GAUGE_VALUE);
				if( 0 < iGaugeValue )
				{
					int const iDecreaseVal = std::max(1, pkEffect->GetAbil(AT_ADDED_GAUGE_VALUE));
					int const iTargetValue = std::max(0, iGaugeValue - iDecreaseVal);
					pkTargetUnit->SetAbil(AT_ADDED_GAUGE_VALUE, iTargetValue, true, true);
				}
			}
		}
		++unit_itor;
	}
	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgRestEffectFunction - Effect Function 의자 휴식
///////////////////////////////////////////////////////////
void PgRestEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	if( pkUnit )
	{
		POINT3 kPos = pkUnit->GetPos();

		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+10, (int)kPos.x);
		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+11, (int)kPos.y);
		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+12, (int)kPos.z);
	}

	//이곳에 들어오는 CEffect는 CItemEffect 이다.
	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef)
	{
		int const iAddHP = pkDef->GetAbil(AT_HP);
		if(0 < iAddHP)
		{
			int const iCurHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

			int const iNewHp = __min(iMaxHP, iCurHP + iAddHP);
			OnSetAbil(pkUnit, AT_HP, iNewHp);

		}		
	
		int const iAddMP = pkDef->GetAbil(AT_MP);
		if(0 < iAddMP)
		{
			int const iCurMP = pkUnit->GetAbil(AT_MP);
			int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

			int const iNewMP = __min(iMaxMP, iCurMP + iAddMP);
			OnSetAbil(pkUnit, AT_MP, iNewMP);
		}
	}
}

void PgRestEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	
}
int PgRestEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if( pkUnit )
	{
		int ix = 0,iy = 0, iz = 0;
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+10, ix);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+11, iy);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+12, iz);

		if( (abs(ix-pkUnit->GetPos().x) >= 10) || (abs(iy-pkUnit->GetPos().y) >= 10) ) // 움직이면 해제!
		{
			return ECT_MUSTDELETE;
		}
	}

	//이곳에 들어오는 CEffect는 CItemEffect 이다.
	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef)
	{
		bool bAddEffect = false;
		int const iAddHP = pkDef->GetAbil(AT_HP);
		if(0 < iAddHP)
		{
			int const iCurHP = pkUnit->GetAbil(AT_HP);
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);

			int const iNewHp = __min(iMaxHP, iCurHP + iAddHP);
			OnSetAbil(pkUnit, AT_HP, iNewHp);

			if( iCurHP != iNewHp )
			{
				bAddEffect = true;
			}
		}		
	
		int const iAddMP = pkDef->GetAbil(AT_MP);
		if(0 < iAddMP)
		{
			int const iCurMP = pkUnit->GetAbil(AT_MP);
			int const iMaxMP = pkUnit->GetAbil(AT_C_MAX_MP);

			int const iNewMP = __min(iMaxMP, iCurMP + iAddMP);
			OnSetAbil(pkUnit, AT_MP, iNewMP);

			if( iCurMP != iNewMP )
			{
				bAddEffect = true;
			}
		}

		if( true == bAddEffect )
		{
			GET_DEF(CItemDefMgr, kItemDefMgr);
			int const iAddEffect = kItemDefMgr.GetAbil(pkEffect->GetKey(), AT_EFFECTNUM2);
			if( iAddEffect > 0 )
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);
			}
		}
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgLoveBalloonEffectFunction - Effect Function 사랑의 열기구 이펙트를 실제로 걸어주는 이펙트
///////////////////////////////////////////////////////////
void PgLoveBalloonEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
		return ;
	}

	int const iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM1);

	if(0 < iNewEffect)
	{
		CUnit* pkTarget = pkGround->GetUnit(pkUnit->GetCoupleGuid());

		EffectQueueData kData(EQT_ADD_EFFECT, iNewEffect, 0, pkArg, pkUnit->GetID());
		if(pkTarget)
		{			
			pkTarget->AddEffectQueue(kData);
			pkUnit->AddEffectQueue(kData);
		}
		else
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, pkEffect->GetKey(), 0, pkArg, pkUnit->GetID());
			pkUnit->AddEffectQueue(kData);
		}
	}
}
void PgLoveBalloonEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
		return ;
	}

	int const iNewEffect = pkEffect->GetAbil(AT_EFFECTNUM1);

	if(0 < iNewEffect)
	{
		CUnit* pkTarget = pkGround->GetUnit(pkUnit->GetCoupleGuid());

		EffectQueueData kData(EQT_DELETE_EFFECT, iNewEffect, 0, pkArg, pkUnit->GetID());
		if(pkTarget)
		{		
			pkTarget->AddEffectQueue(kData);			
		}

		pkUnit->AddEffectQueue(kData);
	}
}

int PgLoveBalloonEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	INFO_LOG(BM::LOG_LV6, __FL__<<L"Don't call me Tick EffectNo["<<pkEffect->GetEffectNo()<<L"] ");
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgCoupleRingEffectFunction - Effect Function 커플링 이펙트
///////////////////////////////////////////////////////////
void PgCoupleRingEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgCoupleRingEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgCoupleRingEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if( pkUnit )
	{
		BM::GUID const &kCoupleGuid = pkUnit->GetCoupleGuid();

		if( BM::GUID::IsNotNull(kCoupleGuid) )
		{
			PgGround* pkGround = NULL;
			if(pkArg)
			{
				pkArg->Get(ACTARG_GROUND, pkGround);
			}
			if (!pkGround)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
				return ECT_MUSTDELETE;
			}
			
			CUnit *pkCoupleUnit = pkGround->GetUnit(kCoupleGuid);
			if( pkCoupleUnit )
			{
				if( pkUnit->GetID() == pkCoupleUnit->GetCoupleGuid() )
				{
					int iRingItem[] = {60002620, 60002630, 0};
					bool bRingEquip[] = {false, false};

					PgBase_Item kMyRingItem;
					if( SUCCEEDED(pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_L), kMyRingItem)) )
					{
					}
					PgBase_Item kMyRingItem2;
					if( SUCCEEDED(pkUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_R), kMyRingItem2)) )
					{
					}
					PgBase_Item kCoupleRingItem;
					if( SUCCEEDED(pkCoupleUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_L), kCoupleRingItem)) )
					{
					}
					PgBase_Item kCoupleRingItem2;
					if( SUCCEEDED(pkCoupleUnit->GetInven()->GetItem(SItemPos(IT_FIT, EQUIP_POS_RING_R), kCoupleRingItem2)) )
					{
					}

					int i = 0;
					while( iRingItem[i] )
					{
						if( (iRingItem[i] == kMyRingItem.ItemNo()) || (iRingItem[i] == kMyRingItem2.ItemNo()) )
						{
							bRingEquip[0] = true;
						}
						++i;
					}

					i = 0;
					while( iRingItem[i] )
					{
						if( (iRingItem[i] == kCoupleRingItem.ItemNo()) || (iRingItem[i] == kCoupleRingItem2.ItemNo()) )
						{
							bRingEquip[1] = true;
						}
						++i;
					}

					if( (true == bRingEquip[0]) && (true == bRingEquip[1])  )
					{
						CEffect *pkEffectCouple = pkCoupleUnit->GetEffect(pkEffect->GetEffectNo());
						if( !pkEffectCouple )
						{
							EffectQueueData kData(EQT_ADD_EFFECT, pkEffect->GetEffectNo(), 0, pkArg, pkCoupleUnit->GetID());
							pkCoupleUnit->AddEffectQueue(kData);
						}

						return ECT_DOTICK;
					}
				}
			}
		}
		return ECT_MUSTDELETE;
	}
	return ECT_DOTICK;
}

void PgRestExpEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	if( pkUnit )
	{
		POINT3 kPos = pkUnit->GetPos();
		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+10, (int)kPos.x);
		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+11, (int)kPos.y);
		pkEffect->SetActArg(ACT_ARG_CUSTOMDATA1+12, (int)kPos.z);
	}
}

void PgRestExpEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgRestExpEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if( pkUnit )
	{
		int ix = 0,iy = 0, iz = 0;
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+10, ix);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+11, iy);
		pkEffect->GetActArg(ACT_ARG_CUSTOMDATA1+12, iz);

		if( (abs(ix-pkUnit->GetPos().x) >= 10) || (abs(iy-pkUnit->GetPos().y) >= 10) ) // 움직이면 해제!
		{
			return ECT_MUSTDELETE;
		}
	}

	//이곳에 들어오는 CEffect는 CItemEffect 이다.
	CEffectDef const* pkDef = pkEffect->GetEffectDef();
	if(pkDef)
	{
		float const fAddExpFromEffect = pkDef->GetAbil(AT_BONUS_EXP_RATE_EFFECT) / 1000000.0f;
		if(0 < fAddExpFromEffect)
		{
			GET_DEF(PgClassDefMgr, kClassDef);
			int const iClass = pkUnit->GetAbil(AT_CLASS);
			__int64 const i64LvExp = kClassDef.GetExperience4Levelup(SClassKey(iClass, pkUnit->GetAbil(AT_LEVEL)));
			__int64 const i64CurExp = pkUnit->GetAbil64(AT_EXPERIENCE);
			int const iMaxExperienceRate = g_kEventView.VariableCont().iExpAdd_MaxExperienceRate;
			__int64 const i64MaxExpAdd = std::min<__int64>( kClassDef.GetMaxExperience(iClass), i64CurExp + static_cast<__int64>(iMaxExperienceRate / 100.0f * i64LvExp));
			__int64 i64NewExpAdd = std::max((pkUnit->GetAbil64(AT_REST_EXP_ADD_MAX) - i64CurExp), 0i64);

			i64NewExpAdd += i64CurExp;
			i64NewExpAdd += std::max(1i64, static_cast<__int64>(i64LvExp * fAddExpFromEffect));
			i64NewExpAdd = std::min(i64MaxExpAdd, i64NewExpAdd);	//최대값 검사
			pkUnit->SetAbil64(AT_REST_EXP_ADD_MAX, i64NewExpAdd, true);
		}
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgKingOfHill_Bomberman - Effect Function 점령전 봄버맨
///////////////////////////////////////////////////////////
void PgKingOfHill_Bomberman::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	OnSetAbil(pkUnit, AT_DISPLAY_MINIMAP_EFFECT, pkEffect->GetAbil(AT_DISPLAY_MINIMAP_EFFECT));
	if( !pkUnit )
	{
		return;
	}

	PgGround * pGround = NULL;
	if( pkArg )
	{
		pkArg->Get(ACTARG_GROUND, pGround);
	}

	PgWarGround * pWarGnd = dynamic_cast<PgWarGround*>(pGround);
	if( !pWarGnd )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pGround is NULL"));
		return;
	}

	pWarGnd->SendNfyMessage(pkUnit, 74102);
}

void PgKingOfHill_Bomberman::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	OnSetAbil(pkUnit, AT_DISPLAY_MINIMAP_EFFECT, 0);
}

int PgKingOfHill_Bomberman::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(!pkUnit)
	{
		return ECT_MUSTDELETE;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	PgWarGround* pkWarGnd = dynamic_cast<PgWarGround*>(pkGround);
	if(!pkWarGnd)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	CUnit * pkTriggerUnit = NULL;
	int const iTeam = pkUnit->GetAbil(AT_TEAM);
	CONT_GTRIGGER const& kContTrigger = pkWarGnd->GetContTrigger();
	for(CONT_GTRIGGER::const_iterator trigger_itr=kContTrigger.begin(); trigger_itr!=kContTrigger.end(); ++trigger_itr)
	{
		if(GTRIGGER_TYPE_KING_OF_HILL == trigger_itr->second->GetType())
		if(PgGTrigger_KingOfHill * pkTmpTrigger = dynamic_cast<PgGTrigger_KingOfHill*>(trigger_itr->second))
		{
			if(CUnit * pkTmpUnit = pkGround->GetUnit(pkTmpTrigger->GetUnitGuid()))
			{
				int const iTriggerTeam = pkTmpUnit->GetAbil(AT_TEAM);
				if(TEAM_NONE!=iTriggerTeam && iTriggerTeam!=iTeam)
				{
					float const fDistanceQ = GetDistanceQ(pkTmpUnit->GetPos(), pkUnit->GetPos());
					int const iRange = pkTmpUnit->GetAbil(AT_DISTANCE);
					if(fDistanceQ <= (float)iRange * iRange)
					{
						POINT3 const ptDist = pkTmpUnit->GetPos() - pkUnit->GetPos();
						if (abs(ptDist.z) > 50)
						{
							continue;
						}

						pkTriggerUnit = pkTmpUnit;
						break;
					}
				}
			}
		}
	}
	if(pkTriggerUnit)
	{
		if(int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1))
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkTriggerUnit->GetID());
			pkUnit->AddEffectQueue(kData);
			pkWarGnd->SendNfyMessage(pkUnit, 74103);
		}
		return ECT_MUSTDELETE;
	}
	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgFilterExceptEffectFunction - 지속적인 디버프 제거기능
///////////////////////////////////////////////////////////
int PgFilterExceptEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(!pkUnit || !pkArg)
	{
		return ECT_MUSTDELETE;
	}

	PgGround* pkGround = NULL;
	pkArg->Get(ACTARG_GROUND, pkGround);
	if(!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	for(int i=0; i<10; ++i)
	{
		int const iExcept = pkEffect->GetAbil(AT_FILTER_EXCEPT_01+i);
		if(0==iExcept)
		{
			break;
		}

		//같은 종류의 스킬이 있을 경우 삭제
		if(CEffect* pkEffect = pkUnit->GetEffect(iExcept, true))
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iExcept);
			pkUnit->AddEffectQueue(kData);
		}
	}

	return PgDefaultEffectFunction::EffectTick(pkUnit, pkEffect, pkArg, dwElapsed);
}
