#include "StdAfx.h"
#include "PgElitePattenState.h"

void ElitePattenStateChange(CUnit* pkUnit, EElitePattenStateType const eType)
{
	if( !pkUnit )
	{
		return;
	}

	pkUnit->SetAbil(AT_ELITEPATTEN_NEXTSTATE, eType);
}

void AddUseEffect(CUnit* pkUnit, SActArg* pkArg, int const iEffectNo)
{
	if(pkUnit && 0<iEffectNo)
	{
		pkUnit->SetAbil(AT_EP_USE_EFFECT_NO, iEffectNo);
		//Skill종료되고 강제로 버프가 작동해야함
		//EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo, 0, pkArg, pkUnit->GetID());
		//pkUnit->AddEffectQueue(kData);
	}
}

void RemoveUseEffect(CUnit* pkUnit)
{
	if(pkUnit)
	{
		int const iDelEffect = pkUnit->GetAbil(AT_EP_USE_EFFECT_NO);
		if(0 < iDelEffect)
		{
			pkUnit->DeleteEffect(iDelEffect);
			pkUnit->SetAbil(AT_EP_USE_EFFECT_NO,0);
		}
	}
}

void ProcessForceSkill(CUnit * pkUnit, SActArg* pkArg, int const iSkillNo, bool const bForceBlowup=true)
{
	if( !pkUnit || iSkillNo==0)
	{
		return;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if( pkSkill )
	{
		int iForceSetFlag = (!bForceBlowup ? EFSF_NOT_RUN_BLOWUP : EFSF_NONE);
		iForceSetFlag |= EFSF_NO_DMGACT;
		pkUnit->GetSkill()->ForceReserve(iSkillNo, static_cast<EForceSetFlag>(iForceSetFlag));
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);

		//버프 적용
		int const iAddEffect = pkSkill->GetAbil(AT_REMOVE_SKILL_FORCEEFFECT);
		AddUseEffect(pkUnit, pkArg, iAddEffect);
	}
}

int GetFrenzyTime(int const iWeight, int const iLevel)
{
	int iMax = 0;
	int iTime = 0;
	switch(iWeight)
	{
	case 0:
		{
			iMax = 35;
			iTime = iLevel - 6;
		}break;
	case 1:
		{
			iMax = 40;
			iTime = iLevel + 0;
		}break;
	case 3:
		{
			iMax = 55;
			iTime = iLevel + 8;
		}break;
	case 2:
	default:
		{
			iMax = 45;
			iTime = iLevel + 2;
		}break;
	}

	return std::min<int>(iTime,iMax) * 1000;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PgElitePattenInit
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgElitePattenInit::PgElitePattenInit()
{
}

PgElitePattenInit::~PgElitePattenInit()
{
}

void PgElitePattenInit::OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg)
{
}

void PgElitePattenInit::DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	ElitePattenStateChange(pkUnit, EPS_NORMAL);
}

void PgElitePattenInit::OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PgElitePattenNormal
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgElitePattenNormal::PgElitePattenNormal()
{
}

PgElitePattenNormal::~PgElitePattenNormal()
{
}

void PgElitePattenNormal::OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}
	pkUnit->SetAbil(AT_FRENZY_NOW_FIRE,0);

	int const iSkillNo = pkUnit->GetAbil(AT_NORMAL_SKILL_NO);
	if(EPS_INIT!=ePrevState)
	{
		ProcessForceSkill(pkUnit, pkArg, iSkillNo);
	}
	else
	{
		GET_DEF(CSkillDefMgr, kSkillDefMgr);
		CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
		if( pkSkill )
		{
			//버프 적용
			int const iAddEffect = pkSkill->GetAbil(AT_REMOVE_SKILL_FORCEEFFECT);
			if( iAddEffect )
			{
				pkUnit->SetAbil(AT_EP_USE_EFFECT_NO, iAddEffect);
				EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
				pkUnit->AddEffectQueue(kData);
			}
		}
	}
}

void PgElitePattenNormal::DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}

	if(pkUnit->GetAbil(AT_FRENZY_NOW_FIRE) >= pkUnit->GetAbil(AT_FRENZY_MAX_FIRE))
	{
		ElitePattenStateChange(pkUnit, EPS_FRENZY);
	}
}

void PgElitePattenNormal::OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	//버프 해제
	RemoveUseEffect(pkUnit);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PgElitePattenFrenzy
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgElitePattenFrenzy::PgElitePattenFrenzy()
{
}

PgElitePattenFrenzy::~PgElitePattenFrenzy()
{
}

void PgElitePattenFrenzy::OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}

	//스킬발동
	ProcessForceSkill(pkUnit, pkArg, pkUnit->GetAbil(AT_FRENZY_SKILL_NO), false);

	pkUnit->SetAbil(AT_GROGGY_NOW,0,true,true);
	int const iMaxGroggy = pkUnit->GetAbil(AT_GROGGY_MAX);
	int const iFrenzyRecovery = std::max(pkUnit->GetAbil(AT_FRENZY_RECOVERY),1);
	pkUnit->SetAbil(AT_FRENZY_TIME, GetFrenzyTime(pkUnit->GetAbil(AT_MONSTER_TUNNING_WEIGHT), pkUnit->GetAbil(AT_LEVEL)), true, true);
}

void PgElitePattenFrenzy::DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}

	//
	int const iNowFrenzyTime = std::max<int>(pkUnit->GetAbil(AT_FRENZY_TIME)-dwElapsedTime, 0);
	pkUnit->SetAbil(AT_FRENZY_TIME, iNowFrenzyTime);
	if(0 >= iNowFrenzyTime)
	{
		ElitePattenStateChange(pkUnit, EPS_NORMAL);
	}

	//
	int const iNow = pkUnit->GetAbil(AT_GROGGY_NOW);
	int const iMax = pkUnit->GetAbil(AT_GROGGY_MAX);
	if(iNow >= iMax)
	{
		ElitePattenStateChange(pkUnit, EPS_GROGGY);
	}
	
	//적용된 완화Abil Time체크
	pkUnit->UpdateGroggyRelaxTime(dwElapsedTime);

	//그로기 포인트 자동감소(분노 자동감소값 사용)
	pkUnit->AutoGroggy(dwElapsedTime);
}

void PgElitePattenFrenzy::OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	//버프 해제
	RemoveUseEffect(pkUnit);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PgElitePattenGroggy
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PgElitePattenGroggy::PgElitePattenGroggy()
{
}

PgElitePattenGroggy::~PgElitePattenGroggy()
{
}

void PgElitePattenGroggy::OnEnter(CUnit* pkUnit, EElitePattenStateType const ePrevState, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}

	pkUnit->SetAbil(AT_GROGGY_NOW, pkUnit->GetAbil(AT_GROGGY_MAX), true, true);

	//스킬발동
	ProcessForceSkill(pkUnit, pkArg, pkUnit->GetAbil(AT_GROGGY_SKILL_NO));
}

void PgElitePattenGroggy::DoAction(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	if( !pkUnit )
	{
		return;
	}

	//적용된 완화Abil Time체크
	pkUnit->UpdateGroggyRelaxTime(dwElapsedTime);

	//그로기 포인트 자동감소(그로기 자동감소값 사용)
	pkUnit->AutoGroggy(dwElapsedTime);

	if(0 >= pkUnit->GetAbil(AT_GROGGY_NOW))
	{
		ElitePattenStateChange(pkUnit, EPS_NORMAL);
	}
}

void PgElitePattenGroggy::OnLeave(CUnit* pkUnit, DWORD const dwElapsedTime, SActArg* pkArg)
{
	//버프 해제
	RemoveUseEffect(pkUnit);
}
