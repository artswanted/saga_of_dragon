#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction_MapEntity.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PublicMap/PgWarGround.h"
#include "PublicMap/PgPvPSetting.h"

template<typename T>
T DefaultValue(T const& _def, T const& _value)
{
	if(0 == _value)
	{
		return _def;
	}
	return _value;
}

///////////////////////////////////////////////////////////
//  PgDontJumpFunction
///////////////////////////////////////////////////////////
//void PgDontJumpFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
//{
////	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
////
////	PgGround* pkGround = NULL;
////	pkArg->Get(ACTARG_GROUND, pkGround);
////
////	if(!pkGround)
////	{
////		INFO_LOG(BM::LOG_LV6, _T("[%s][%d] Cannot find Ground"), __FUNCTIONW__, pkEffect->GetEffectNo());
////		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
////		return;
////	}
////
////	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
////
////	UNIT_PTR_ARRAY kUnitArray;
////	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
////	if(pkEntity)
////	{
////		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_PLAYER );
////	}
////		
////	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
////	while(kUnitArray.end() != target_itor)
////	{			
////		//Effect의 Target List에 없으면
////		if(!pkEffect->IsTarget((*target_itor).pkUnit->GetID()))
////		{
////			CUnit* pkTarget = (*target_itor).pkUnit;
////			if(pkTarget)
////			{
////				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit, EFFECT_TYPE_GROUND);
////				pkTarget->AddEffectQueue(kData);
////
////				pkEffect->AddTarget(pkTarget->GetID());
//////					INFO_LOG(BM::LOG_LV6, _T("[%s][%d] AddTarget %s "), __FUNCTIONW__, pkEffect->GetEffectNo(), pkTarget->Name().c_str());
////			}
////		}
////
////		++target_itor;
////	}
//}

void PgDontJumpFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
//int PgDontJumpFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
//{
//	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
//
//	PgGround* pkGround = NULL;
//	pkArg->Get(ACTARG_GROUND, pkGround);
//
//	if(!pkGround)
//	{
//		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
//		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
//		return ECT_MUSTDELETE;
//	}
//
//	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);
//
//	UNIT_PTR_ARRAY kUnitArray;
//	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
//	if(pkEntity)
//	{
//		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_PLAYER );
//	}
//		
//	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
//	while(kUnitArray.end() != target_itor)
//	{			
//		//Effect의 Target List에 없으면
//		if(!pkEffect->IsTarget((*target_itor).pkUnit->GetID()))
//		{
//			CUnit* pkTarget = (*target_itor).pkUnit;
//			if(pkTarget)
//			{
//				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit, EFFECT_TYPE_GROUND);
//				pkTarget->AddEffectQueue(kData);
//
//				pkEffect->AddTarget(pkTarget->GetID());
////					INFO_LOG(BM::LOG_LV6, _T("[%s][%d] AddTarget %s "), __FUNCTIONW__, pkEffect->GetEffectNo(), pkTarget->Name().c_str());
//			}
//		}
//
//		++target_itor;
//	}
//	return ECT_DOTICK;
//}

///////////////////////////////////////////////////////////
//  PgParelPoisonAura - Effect Function 파렐경 맵 엔티티가 사용하는 이펙트
///////////////////////////////////////////////////////////
void PgParelPoisonAura::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}
void PgParelPoisonAura::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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

int PgParelPoisonAura::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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

	int iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);

	// 이 이펙트를 쓰는 경우는 타겟팅이 0이면 20명까지 타겟팅 하도록 해준다.
	if(0 == iMaxTarget)
	{
		iMaxTarget = 20;
	}

	UNIT_PTR_ARRAY kUnitArray;
	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
	if(pkEntity)
	{
		CEffectDef const* pkEffectDef = pkEffect->GetEffectDef();
		bool const bCallTimeCheck = pkEffectDef ? pkEffectDef->GetInterval()>500 : true;
		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_PLAYER|UT_SUMMONED, false, bCallTimeCheck);
	}
		
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		bool bAddEffect = false;

		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget)
		{
			// 죽은 경우라면 이펙트 리스트에서 삭제 시켜 준다.
			if(pkTarget->IsState(US_DEAD))
			{
				pkEffect->DeleteTarget(pkTarget->GetID());
				++target_itor;
				continue;
			}

			////Effect의 Target List에 없으면
			//if(!pkEffect->IsTarget(pkTarget->GetID()))
			//{
			//	bAddEffect = true;
			//}
			////Effect의 Target List에는 있으나
			//else
			//{			
			//	// 실제 이펙트는 걸려 있지 않은 경우(이펙트가 풀린 경우)
			//	CEffect* pkEffect2 = pkTarget->GetEffect(iEffectNo);
			//	if(NULL == pkEffect2)
			//	{
			//		bAddEffect = true;					
			//	}				
			//}

			//if(bAddEffect)
			if(false==pkTarget->GetEffect(iEffectNo))
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
				pkTarget->AddEffectQueue(kData);
				pkEffect->AddTarget(pkTarget->GetID());
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
					pkTarget->AddEffectQueue(kData);
				}
				else
				{
					INFO_LOG(BM::LOG_LV6, __FL__<<L"Effect["<<pkEffect->GetEffectNo()<<L"] Cannot Find Unit["<<(*target_guid_itor)<<L"] ");
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
//  PgParelPoison - 파렐경 맵에 있는 독
///////////////////////////////////////////////////////////
void PgParelPoison::EffectBegin(CUnit* pkUnit, CEffect*pkEffect, SActArg const *pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

void PgParelPoison::EffectEnd(CUnit* pkUnit, CEffect*pkEffect, SActArg const *pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgParelPoison::EffectTick(CUnit* pkUnit, CEffect*pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	//이펙트 중화 어빌값이 같을 경우 효과를 받지 않게 해준다.
	if(pkEffect->GetEffectNo() == pkUnit->GetAbil(AT_EXCEPT_EFFECT_TICK))
	{
		return ECT_DOTICK;
	}

	if(pkUnit->IsDead())
	{
		return ECT_DOTICK;
	}

	int const iAddHP = pkEffect->GetAbil(AT_HP);
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

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgChaosMapAuraEffect - Effect Function 카오스 맵 버프 이펙트(카오스 맵에 있는 유저에게 디버프를 걸어준다.
///////////////////////////////////////////////////////////
void PgChaosMapAuraEffect::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}
void PgChaosMapAuraEffect::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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

int PgChaosMapAuraEffect::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	int const iDebuffEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	int const iResistEffectNo = pkEffect->GetAbil(AT_EXCEPT_CHAOS_EFFECT_TICK);

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
	{// Entity의 범위내에 있는 Player들을 AT_MAX_TARGETNUM만큼 얻어와서
		pkGround->FindEnemy(pkEntity, kUnitArray, iMaxTarget, UT_PLAYER );
	}
		
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(!pkTarget)
		{
			++target_itor;
			continue;
		}
		bool const bHaveResistBuff = pkTarget->GetEffect(iResistEffectNo) ? true : false;

		if( !pkEffect->IsTarget(pkTarget->GetID())	 )
		{// 범위에 들어온 새로운 Target이 있으면 (관리 List에 없으므로)
			// 관리 List에 추가하고
			pkEffect->AddTarget(pkTarget->GetID());

			CEffect* pkUnitDebuff= pkTarget->GetEffect(iDebuffEffectNo);
			if(!pkUnitDebuff			// 디버프가 없고
				&& !bHaveResistBuff		// 저항 버프가 없다면
				)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iDebuffEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
				pkTarget->AddEffectQueue(kData);
			}
		}
		else
		{// 관리 List에 존재하는 Target이
			CEffect* pkUnitEffect = pkTarget->GetEffect(iDebuffEffectNo);
			if(!pkUnitEffect)		
			{// 디버프가 걸려있지 않고
				if(!bHaveResistBuff)
				{// 저항버프가 없다면, 디버프를 걸어주고
					EffectQueueData kData(EQT_ADD_EFFECT, iDebuffEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
					pkTarget->AddEffectQueue(kData);
				}
			}
			else
			{// 걸려 있지만
				if( true == bHaveResistBuff )
				{// 대상이 저항버프를 가지고있다면 디버프를 제거해주고
					EffectQueueData kData(EQT_DELETE_EFFECT, iDebuffEffectNo);
					pkTarget->AddEffectQueue(kData);
				}
			}
		}
		++target_itor;
	}

	VEC_GUID& rkTargtList = pkEffect->GetTargetList();
	if( rkTargtList.empty() )
	{
		return ECT_DOTICK;
	}

	VEC_GUID::iterator target_guid_itor = rkTargtList.begin();
	while(target_guid_itor != rkTargtList.end())
	{// 새로 영역에 들어온 Target이 있는지
		bool bFind = false;
		for(UNIT_PTR_ARRAY::const_iterator unit_itor = kUnitArray.begin(); unit_itor != kUnitArray.end(); ++unit_itor)
		{
			CUnit* pkTarget = (*unit_itor).pkUnit;
			if( !pkTarget )
			{
				continue;
			}
			if( pkTarget->GetID() != (*target_guid_itor) )
			{
				continue;
			}			
			 // 확인한 후
			bFind = true;
			break;
		}
		if( !bFind )
		{// 디버프 범위를 벗어난 Target들은( 관리List에 있으나, 새로잡은 TargetList엔 없는)
			CUnit* pkTarget = pkGround->GetUnit( (*target_guid_itor) );
			if( pkTarget )
			{// 디버프를 제거해주고
				EffectQueueData kData(EQT_DELETE_EFFECT, iDebuffEffectNo);
				pkTarget->AddEffectQueue(kData);
			}
			else
			{
				INFO_LOG(BM::LOG_LV6, L"Effect["<<pkEffect->GetEffectNo()<<L"] Cannot Find Unit["<<(*target_guid_itor)<<L"] ");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkTarget is NULL"));
			}
			// 관리List에서 제거해준다
			target_guid_itor = rkTargtList.erase(target_guid_itor);
		}
		else
		{
			++target_guid_itor;
		}
	}
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgChaosMapEffect - 카오스맵 능력치 저하 디버프
///////////////////////////////////////////////////////////
void PgChaosMapEffect::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	// 카오스 버프를 막아주는 버프가 없을 경우 효과를 걸어준다.
	if(pkEffect->GetEffectNo() != pkUnit->GetAbil(AT_EXCEPT_CHAOS_EFFECT_TICK))
	{
		//능력치 저하가 일어났다는 표시
		pkEffect->SetValue(1);	// PgChaosMapAuraEffect의 Tick에서 Value값을 사용하던 코드가 필요없어서 삭제하였음. 이 부분은 다른 곳에서 영향을 받을 수 있기 때문에 남겨둠.
		PgDefaultEffectFunction::EffectBegin(pkUnit, pkEffect, pkArg);
	}
}

void PgChaosMapEffect::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());

	//능력치 저하가 일어 났으므로 능력치 저하 해제
	if(1 == pkEffect->GetValue()) // PgChaosMapAuraEffect의 Tick에서 Value값을 사용하던 코드가 필요없어서 삭제하였음. 이 부분은 다른 곳에서 영향을 받을 수 있기 때문에 남겨둠.
	{
		PgDefaultEffectFunction::EffectEnd(pkUnit, pkEffect, pkArg);
	}
}

int PgChaosMapEffect::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());	

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgElga_room_of_heart - Effect Function 엘가 심장의 방 엔티티가 사용하는 이펙트
///////////////////////////////////////////////////////////
void PgElga_room_of_heart::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
	if(pkEffect->GetAbil(AT_DEFAULT_EFFECT))
	{
		pkUnit->AddEffect(pkEffect->GetAbil(AT_DEFAULT_EFFECT), 0, pkArg, pkUnit);
	}
}
void PgElga_room_of_heart::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	VEC_GUID& rkTargetList = pkEffect->GetTargetList();

	VEC_GUID::const_iterator target_itor = rkTargetList.begin();
	while(rkTargetList.end() != target_itor)
	{		
		CUnit* pkTarget = pkGround->GetUnit((*target_itor));
		if(pkTarget)
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
			pkTarget->AddEffectQueue(kData);
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgElga_room_of_heart::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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
		return ECT_MUSTDELETE;
	}

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	//int const iElgaClass = pkEffect->GetAbil(AT_CLASS);//엘가 클래스번호
	//int const iElgaEffect = pkEffect->GetAbil(AT_EFFECTNUM2);//엘가가 걸린 이펙트번호
	//CUnit * pkElgaUnit = pkGround->GetBossMonster( iElgaClass );

	////엘가가 스턴상태인지 확인
	//if(pkElgaUnit && pkElgaUnit->GetEffect(iElgaEffect))
	//{//엘가가 스턴상태인경우
	//	//플레이어들에게 이펙트를 검
	//}
	//else
	//{
	//	//플레이어들에게 이펙트를 제거하고, 방에서 내쫒음..
	//}

	UNIT_PTR_ARRAY kUnitArray;
	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
	if(pkEntity)
	{
		pkGround->GetUnitInCube( pkEntity->VisualFieldMin(), pkEntity->VisualFieldMax(), pkEntity->GetPos(), UT_PLAYER, kUnitArray );
	}


	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget)
		{
			// 실제 이펙트는 걸려 있지 않은 경우(이펙트가 풀린 경우)
			CEffect* pkEffect2 = pkTarget->GetEffect(iEffectNo);
			if(NULL == pkEffect2)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
				pkTarget->AddEffectQueue(kData);
				pkEffect->AddTarget(pkTarget->GetID());
			}
		}

		++target_itor;
	}

	//이펙트에 걸려있는 타겟들
	VEC_GUID& rkTargtList = pkEffect->GetTargetList();
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
				EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
				pkTarget->AddEffectQueue(kData);
			}
			else
			{
				INFO_LOG(BM::LOG_LV6, __FL__<<L"Effect["<<pkEffect->GetEffectNo()<<L"] Cannot Find Unit["<<(*target_guid_itor)<<L"] ");
			}
			target_guid_itor = rkTargtList.erase(target_guid_itor);
		}
		else
		{
			++target_guid_itor;
		}
	}

	return ECT_DOTICK;
}



///////////////////////////////////////////////////////////
//  PgTeleportToDummyEffect - 범위의 플레이어를 해당 지역으로 워프시킴
///////////////////////////////////////////////////////////
void PgTeleportToDummyEffect::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}
void PgTeleportToDummyEffect::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}

int PgTeleportToDummyEffect::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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
		return ECT_MUSTDELETE;
	}

	if(pkUnit->GetAbil(AT_PAUSE_TELEPORT_TO_DUMMY_EFFECT))
	{
		return ECT_DOTICK;
	}

	UNIT_PTR_ARRAY kUnitArray;
	PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit);
	if(pkEntity)
	{
		pkGround->GetUnitInCube( pkEntity->VisualFieldMin(), pkEntity->VisualFieldMax(), pkEntity->GetPos(), UT_PLAYER, kUnitArray );
	}

	BM::vstring kTargetNode(L"char_spawn_");
	kTargetNode += pkEffect->GetAbil(AT_SKILL_MAP_DUMMY_NUM1);

	POINT3 kTargetPos = pkGround->GetNodePosition(MB(kTargetNode));

	if (0==kTargetPos.x && 0==kTargetPos.y && 0==kTargetPos.z)	//못찾을 경우
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Target Dummy , EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Target Dummy is NULL"));
		return ECT_MUSTDELETE;
	}

	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget)
		{
			if(pkTarget->IsUnitType(UT_PLAYER))
			{
				pkGround->SendToPosLoc(pkTarget, kTargetPos, MMET_Normal);
			}
			else
			{
				pkTarget->SetPos(kTargetPos);
			}
		}
		++target_itor;
	}

	return ECT_DOTICK;
}




///////////////////////////////////////////////////////////
//  PgKingOfHill_Guardian - Effect Function 점령전 가디언의 범위안 플레이어 점수 계산되는 이펙트
///////////////////////////////////////////////////////////
inline int GetOccupyPoint_NotIdleRate(PgWarGround * pkWarGnd)
{
	if(pkWarGnd)
	{
		return pkWarGnd->Get_KOH_OccupyPoint_NotIdleRate();
	}
	return 0;
}

void PgKingOfHill_Guardian::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	pkUnit->AddAbil(AT_RELEASE_DEL_EFFECT, 1);
}

void PgKingOfHill_Guardian::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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
		return;
	}

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	VEC_GUID& rkTargetList = pkEffect->GetTargetList();

	VEC_GUID::const_iterator target_itor = rkTargetList.begin();
	while(rkTargetList.end() != target_itor)
	{		
		CUnit* pkTarget = pkGround->GetUnit((*target_itor));
		if(pkTarget)
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
			pkTarget->AddEffectQueue(kData);
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgKingOfHill_Guardian::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	UNIT_PTR_ARRAY kUnitArray;
	if(PgEntity* pkEntity = dynamic_cast<PgEntity*>(pkUnit))
	{
		pkGround->GetUnitInDistance( pkEntity->GetPos(), pkEntity->GetAbil(AT_DISTANCE), UT_PLAYER, kUnitArray, 50.f );
	}

	int const iTeam = pkUnit->GetAbil(AT_TEAM);
	int const iLevel = pkUnit->GetAbil(AT_LEVEL);
	int const iOldPoint = pkUnit->GetAbil(AT_OCCUPY_POINT);
	int const iMaxPoint = pkUnit->GetAbil(AT_MAX_OCCUPY_POINT);

	SET_GUID kContNewTarget;
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		CUnit* pkTarget = (*target_itor).pkUnit;
		if(pkTarget && pkTarget->IsAlive())
		{
			bool bNotEffect = false;		//최대치에 도달했으면 이펙트를 걸지도 않고 걸려있는 이펙트를 제거한다.
			if(iLevel>2 && iTeam==pkTarget->GetAbil(AT_TEAM))
			{
				if(iTeam==TEAM_RED)
				{
					bNotEffect = iOldPoint >= iMaxPoint;
				}
				else
				{
					bNotEffect = iOldPoint <= -iMaxPoint;
				}
			}

			// 실제 이펙트는 걸려 있지 않은 경우(이펙트가 풀린 경우)
			CEffect* pkEffect2 = pkTarget->GetEffect(iEffectNo);
			if(NULL == pkEffect2 && false==bNotEffect)
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
				pkTarget->AddEffectQueue(kData);
				pkEffect->AddTarget(pkTarget->GetID());
				kContNewTarget.insert(pkTarget->GetID());
			}
			if(bNotEffect && pkEffect2)
			{
				EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
				pkTarget->AddEffectQueue(kData);
				VEC_GUID& rkTargtList = pkEffect->GetTargetList();
				VEC_GUID::iterator find_it = std::find(rkTargtList.begin(), rkTargtList.end(), pkTarget->GetID());
				if(find_it != rkTargtList.end())
				{
					rkTargtList.erase(find_it);
				}
			}
		}

		++target_itor;
	}
	

	static int const iNotIdlePointRate = GetOccupyPoint_NotIdleRate(dynamic_cast<PgWarGround*>(pkGround));

	//이펙트에 걸려있는 타겟들
	int iRedPoint = 0;
	int iBluePoint = 0;
	UNIT_PTR_ARRAY kRedTeam;
	UNIT_PTR_ARRAY kBlueTeam;
	CUnit * pkTarget = NULL;
	VEC_GUID& rkTargtList = pkEffect->GetTargetList();
	VEC_GUID::iterator target_guid_itor = rkTargtList.begin();
	while(target_guid_itor != rkTargtList.end())
	{
		//포인트 계산(신규로 잡힌 플레이어는 제외)
		pkTarget = pkGround->GetUnit(*target_guid_itor);
		if(pkTarget && pkTarget->IsAlive())
		{
			SET_GUID::const_iterator c_it = kContNewTarget.find(pkTarget->GetID());
			if(c_it==kContNewTarget.end())
			{
				int iOccupyPoint = pkTarget->GetAbil(AT_C_OCCUPY_POINT);
				if(pkTarget->GetAbil(AT_PREV_ACTION_ID)!=100000250)	//a_domination인 경우는 100% 그렇지 않으면 깍여서 들어가기..
				{
					iOccupyPoint = SRateControl::GetValueRate<int>(iOccupyPoint, iNotIdlePointRate);
				}

				int const iTeam = pkTarget->GetAbil(AT_TEAM);
				if(TEAM_RED == iTeam)
				{
					iRedPoint += iOccupyPoint;
					kRedTeam.Add(pkTarget);
				}
				else if(TEAM_BLUE == iTeam)
				{
					iBluePoint += iOccupyPoint;
					kBlueTeam.Add(pkTarget);
				}
			}
		}

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
			if(pkTarget)
			{
				EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
				pkTarget->AddEffectQueue(kData);
			}
			else
			{
				INFO_LOG(BM::LOG_LV6, __FL__<<L"Effect["<<pkEffect->GetEffectNo()<<L"] Cannot Find Unit["<<(*target_guid_itor)<<L"] ");
			}
			target_guid_itor = rkTargtList.erase(target_guid_itor);
		}
		else
		{
			++target_guid_itor;
		}
	}

	//
	int iPoint = iOldPoint + (iRedPoint-iBluePoint);
	if(ENTITY_SEC_KTH_BASE_HILL==pkUnit->GetAbil(AT_ENTITY_SECOND_TYPE) && 2>iLevel)
	{
		int const iMinPoint = pkUnit->GetAbil(AT_MIN_OCCUPY_POINT);
		if(TEAM_RED==iTeam && iPoint < iMinPoint)
		{
			iPoint = iMinPoint;
		}
		else if(TEAM_BLUE==iTeam && iPoint > iMinPoint)
		{
			iPoint = iMinPoint;
		}
	}
	if(iPoint > iMaxPoint)
	{
		iPoint = iMaxPoint;
	}
	else if(iPoint < -iMaxPoint)
	{
		iPoint = -iMaxPoint;
	}

	if(iOldPoint != iPoint)
	{
		pkUnit->SetAbil(AT_OCCUPY_POINT, iPoint, true, true);

		// 아래는 결산 점수 계산.
		if( (iOldPoint < iMaxPoint) && (iOldPoint > -iMaxPoint) )
		{
			int TickPoint = iRedPoint - iBluePoint;
			if( iOldPoint + TickPoint > iMaxPoint )
			{
				TickPoint = iMaxPoint - iOldPoint;
			}

			if( iOldPoint + TickPoint < -iMaxPoint )
			{
				TickPoint = iMaxPoint + iOldPoint;
			}

			UNIT_PTR_ARRAY * pkUnitList = NULL;
			if( TickPoint > 0 )
			{
				pkUnitList = &kRedTeam;
			}
			else
			{
				TickPoint *= -1; // 양수로 변환.
				pkUnitList = &kBlueTeam;
			}

			if( pkUnitList && pkUnitList->size() )
			{
				TickPoint /= pkUnitList->size();	// 올라간 점령 점수를 인원수로 나누고.
				PgWarGround * pkWarGnd = dynamic_cast<PgWarGround*>(pkGround);
				if(pkWarGnd)
				{
					int DefValue = pkWarGnd->GetKOH_CaptureDecValue(PVP_ST_CAPTURE_DEC_VALUE);
					float GetPoint = static_cast<float>(TickPoint) / (DefValue); // 최종 유저가 가져갈 점수 계산.

					int const Lv1Exp = pkWarGnd->GetKOH_EntityLevelExp(PVP_ST_NEXT_EXP_ENTITY_LV1);
					if( (iOldPoint < Lv1Exp && Lv1Exp < iPoint)
						|| (iOldPoint > -Lv1Exp && -Lv1Exp > iPoint) )
					{	// 중립 진지에서 자신의 진영으로 점령함.
						UNIT_PTR_ARRAY::const_iterator first_itor = pkUnitList->begin();
						if( first_itor != pkUnitList->end() )
						{
							pkWarGnd->SendNfyMessage( first_itor->pkUnit, 74100);
						}
					}

					if(  (iOldPoint >= Lv1Exp && Lv1Exp > iPoint)
						|| (iOldPoint <= -Lv1Exp && -Lv1Exp < iPoint) )
					{	// 다른 진영에서 중립 진지로 만듬.
						UNIT_PTR_ARRAY::const_iterator first_itor = pkUnitList->begin();
						if( first_itor != pkUnitList->end() )
						{
							pkWarGnd->SendNfyMessage( first_itor->pkUnit, 74101);
						}
					}

					CUnit * pTarget = NULL;
					UNIT_PTR_ARRAY::const_iterator target_itor = pkUnitList->begin();
					while(pkUnitList->end() != target_itor)
					{
						pTarget = (*target_itor).pkUnit;
						if( pTarget )
						{
							pkWarGnd->KOH_CpatureStrongHold(pTarget, GetPoint);
						}
						++target_itor;
					}
				}
			}
		}
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgEffectApplyArea - 지역 적용 이펙트
///////////////////////////////////////////////////////////
void PgEffectApplyArea::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s][%d]"), __FUNCTIONW__, pkEffect->GetEffectNo());
}
void PgEffectApplyArea::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
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

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	VEC_GUID& rkTargetList = pkEffect->GetTargetList();

	VEC_GUID::const_iterator target_itor = rkTargetList.begin();
	while(rkTargetList.end() != target_itor)
	{
		if(CUnit * pkTarget = pkGround->GetUnit( *target_itor ))
		{
			EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
			pkTarget->AddEffectQueue(kData);
		}

		++target_itor;
	}

	rkTargetList.clear();
}

int PgEffectApplyArea::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
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
		return ECT_MUSTDELETE;
	}

	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	int const iTargetType = pkEffect->GetAbil(AT_TARGET_TYPE);
	if((0 == iEffectNo) || (ESTARGET_NONE == iTargetType))
	{
		return ECT_MUSTDELETE;
	}

	int const iMaxTarget = DefaultValue<int>(20, pkEffect->GetAbil(AT_MAX_TARGETNUM));
	int const iDetectRange = DefaultValue<int>(50, pkEffect->GetAbil(AT_DETECT_RANGE));

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitTargetList(pkUnit, kUnitArray, iTargetType, iDetectRange);
		
	UNIT_PTR_ARRAY::const_iterator target_itor = kUnitArray.begin();
	while(kUnitArray.end() != target_itor)
	{
		if(CUnit * pkTarget = (*target_itor).pkUnit)
		{
			// 죽은 경우라면 이펙트 리스트에서 삭제 시켜 준다.
			if(pkTarget->IsState(US_DEAD))
			{
				pkEffect->DeleteTarget(pkTarget->GetID());

				EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
				pkTarget->AddEffectQueue(kData);

				++target_itor;
				continue;
			}

			if(false==pkTarget->GetEffect(iEffectNo))
			{
				EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID(), EFFECT_TYPE_GROUND);
				pkTarget->AddEffectQueue(kData);
				pkEffect->AddTarget(pkTarget->GetID());
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
			if(false == bFind)
			{
				if(CUnit * pkTarget = pkGround->GetUnit(*target_guid_itor))
				{
					EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
					pkTarget->AddEffectQueue(kData);
				}
				else
				{
					INFO_LOG(BM::LOG_LV6, __FL__<<L"Effect["<<pkEffect->GetEffectNo()<<L"] Cannot Find Unit["<<(*target_guid_itor)<<L"] ");
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