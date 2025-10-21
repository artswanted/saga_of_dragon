#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActOpening.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"


/////////////////////////////////////////////////////////////
// PgAIActOpening
////////////////////////////////////////////////////////////
bool PgAIActOpening::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	int iDelayTime = pkUnit->GetDelay();

	if (iDelayTime <= 0 && eNextAction == EAI_ACTION_IDLE)
	{
		if(pkUnit->GetSkill() && pkUnit->GetAbil(AT_MON_SKILL_ON_FIRST))
		{
			pkUnit->GetSkill()->PushSkillStack(pkUnit->GetAbil(AT_MON_SKILL_ON_FIRST));
		}
		return true;
	}
	return false;
}

void PgAIActOpening::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED");
#endif
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
	pkUnit->SetState(US_IDLE);
	pkUnit->SetSync(false);
}

void PgAIActOpening::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActOpening::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}
