#include "StdAfx.h"
#include "PgElitePattenManager.h"
#include "PgElitePattenState.h"
#include "PgGround.h"

PgElitePattenManager::PgElitePattenManager()
{
}

PgElitePattenManager::~PgElitePattenManager()
{
}

bool PgElitePattenManager::Run(PgGround* pkGround, CUnit* pkUnit, DWORD const dwElapsed)
{
	if(!pkGround || !pkUnit || EAI_ACTION_OPENING==pkUnit->GetAI()->eCurrentAction)
	{
		return false;
	}

	EElitePattenStateType const eCurrentState = static_cast<EElitePattenStateType>(pkUnit->GetAbil(AT_ELITEPATTEN_STATE));
	if(EPS_NONE==eCurrentState)
	{
		return false;
	}

	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, pkGround);

	EElitePattenStateType const eInitState = static_cast<EElitePattenStateType>(pkUnit->GetAbil(AT_ELITEPATTEN_INITSTATE));
	if( EPS_NONE==eInitState )
	{
		if( pkUnit->GetCountAbil(AT_CANNOT_DAMAGE, AT_CF_ELITEPATTEN) )
		{
			pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_ELITEPATTEN, false, E_SENDTYPE_BROADALL );// 타겟팅 안되도록 설정 해제
		}

		DoAction(pkUnit, eCurrentState, dwElapsed, kActArg);
	}

	EElitePattenStateType const eNextState = static_cast<EElitePattenStateType>(pkUnit->GetAbil(AT_ELITEPATTEN_NEXTSTATE));
	if( EPS_NONE!=eNextState && eNextState!=eCurrentState )
	{
		pkUnit->AddCountAbil( AT_CANNOT_DAMAGE, AT_CF_ELITEPATTEN, true, E_SENDTYPE_BROADALL );// 타겟팅 안되도록 설정
		OnLeave(pkUnit, eCurrentState, eNextState, dwElapsed, kActArg);
		pkUnit->SetAbil(AT_ELITEPATTEN_INITSTATE, eNextState);
		OnEnter(pkUnit, eNextState, eCurrentState, dwElapsed, kActArg);
	}

	if( EPS_NONE!=eNextState )
	{
		pkUnit->SetAbil(AT_ELITEPATTEN_NEXTSTATE, EPS_NONE);
	}

	if( EPS_NONE!=eInitState )
	{
		int iEffectNo = pkUnit->GetAbil(AT_EP_USE_EFFECT_NO);
		if( 0==iEffectNo || pkUnit->FindEffect(pkUnit->GetAbil(AT_EP_USE_EFFECT_NO)) )
		{
			pkUnit->SetAbil(AT_ELITEPATTEN_INITSTATE, EPS_NONE);
			pkUnit->SetAbil(AT_ELITEPATTEN_STATE, eInitState, true, true);
		}
	}
	return true;
}

void PgElitePattenManager::OnEnter(CUnit* pkUnit, EElitePattenStateType const eCurrentState, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg& rkActArg)
{
	pfnDoEnt pkFunc = 0;
	switch(eCurrentState)
	{
	case EPS_NORMAL: {pkFunc = &PgElitePattenNormal::OnEnter;}break;
	case EPS_FRENZY: {pkFunc = &PgElitePattenFrenzy::OnEnter;}break;
	case EPS_GROGGY: {pkFunc = &PgElitePattenGroggy::OnEnter;}break;
	case EPS_INIT:   {pkFunc = &PgElitePattenInit::OnEnter;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown ElitePattenStateType ["<<eCurrentState<<L"]");
		}break;
	}

	if( NULL==pkFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFunc is 0"));
		return;
	}

	(*pkFunc)(pkUnit, ePrevState, dwElapsedTime, &rkActArg);
}

void PgElitePattenManager::OnLeave(CUnit* pkUnit, EElitePattenStateType const eCurrentState, EElitePattenStateType const eNextState, DWORD const dwElapsedTime, SActArg& rkActArg)
{
	pfnDoAct pkFunc = 0;
	switch(eCurrentState)
	{
	case EPS_NORMAL: {pkFunc = &PgElitePattenNormal::OnLeave;}break;
	case EPS_FRENZY: {pkFunc = &PgElitePattenFrenzy::OnLeave;}break;
	case EPS_GROGGY: {pkFunc = &PgElitePattenGroggy::OnLeave;}break;
	case EPS_INIT:   {pkFunc = &PgElitePattenInit::OnLeave;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown ElitePattenStateType ["<<eCurrentState<<L"]");
		}break;
	}

	if( NULL==pkFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFunc is 0"));
		return;
	}

	(*pkFunc)(pkUnit, dwElapsedTime, &rkActArg);
}

void PgElitePattenManager::DoAction(CUnit* pkUnit, EElitePattenStateType const eCurrentState, DWORD const dwElapsedTime, SActArg& rkActArg)
{
	pfnDoAct pkFunc = 0;
	switch(eCurrentState)
	{
	case EPS_NORMAL: {pkFunc = &PgElitePattenNormal::DoAction;}break;
	case EPS_FRENZY: {pkFunc = &PgElitePattenFrenzy::DoAction;}break;
	case EPS_GROGGY: {pkFunc = &PgElitePattenGroggy::DoAction;}break;
	case EPS_INIT:   {pkFunc = &PgElitePattenInit::DoAction;}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Unknown ElitePattenStateType ["<<eCurrentState<<L"]");
		}break;
	}

	if( NULL==pkFunc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkFunc is 0"));
		return;
	}

	(*pkFunc)(pkUnit, dwElapsedTime, &rkActArg);
}

int const MAX_GROGGY_RELAX_TIME = 2000;
void PgElitePattenManager::OnDamaged(CUnit* pkUnit, int const iPoint, CSkillDef const *pkSkillDef)
{
	if( !pkUnit )
	{
		return;
	}

	EElitePattenStateType const eCurrentState = static_cast<EElitePattenStateType>(pkUnit->GetAbil(AT_ELITEPATTEN_STATE));
	if( EPS_NORMAL==eCurrentState )
	{
		int const iMax = std::min(pkUnit->GetAbil(AT_FRENZY_NOW_FIRE)+iPoint,pkUnit->GetAbil(AT_FRENZY_MAX_FIRE));
		pkUnit->SetAbil(AT_FRENZY_NOW_FIRE, iMax);
	}

	if( pkSkillDef && EPS_FRENZY==eCurrentState )
	{
		//축적
		int const iNow = pkUnit->GetAbil(AT_GROGGY_NOW);
		int const iAdd = GetGroggyPoint(pkSkillDef);
		bool const bSend = pkSkillDef->GetAbil(AT_COMBO_HIT_COUNT) > 1 ? false : true;
		if( iAdd )
		{
			pkUnit->SetAbil(AT_GROGGY_NOW, std::min(pkUnit->GetAbil(AT_GROGGY_MAX), iNow+iAdd), bSend, bSend);
		}


		//완화
		int const iGroggyRate = pkSkillDef->GetAbil(AT_DEC_GROGGYPOINT_RATE);
		int const iOldRate = pkUnit->GetAbil(AT_C_DEC_GROGGYPOINT_RATE);
		if(iOldRate < iGroggyRate)
		{
			pkUnit->SetAbil(AT_DEC_GROGGYPOINT_RATE, iGroggyRate);
			pkUnit->NftChangedAbil(AT_DEC_GROGGYPOINT_RATE, E_SENDTYPE_BROADALL);
			pkUnit->SetAbil(AT_GROGGY_RELAX_TIME, MAX_GROGGY_RELAX_TIME);
		}
	}
}