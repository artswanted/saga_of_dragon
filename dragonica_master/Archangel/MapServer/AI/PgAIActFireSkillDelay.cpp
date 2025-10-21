#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActFireSkillDelay.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"


/////////////////////////////////////////////////////////////
// PgAIActFireSkillDelay
/////////////////////////////////////////////////////////////
bool PgAIActFireSkillDelay::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV9, _T("[%s] Checking NextAction[%d]"), __FUNCTIONW__, (int)eNextAction);
	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_FIRESKILL:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				bResult = pkGround->CanAttackTarget(pkUnit);
			}
		}break;
	case EAI_ACTION_CHASEENEMY:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				bResult = pkGround->IsTargetInRange(pkUnit);
			}
		}break;
	case EAI_ACTION_PATROL:
    case EAI_ACTION_ROAD:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_IDLE:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_WAIT_COOLTIME:
		{
			if (pkUnit->GetDelay() <= 0)
			{
				if ( !pkUnit->GetSkill()->Reserve(0) )
				{
					bResult = true;
				}
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<eNextAction<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
	return bResult;
}

void PgAIActFireSkillDelay::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//bool bPhy = ((pkUnit->GetSkill()->GetAbil(AT_SKILL_ATT) & SAT_PHYSICS) == 0) ? false : true ;//쓰는데가 없어서 막음

	// SkillDelay 시간이 설정 안되있으므로
	//int iSkillDelay = (bPhy) ? pkUnit->GetAbil(AT_C_ATTACK_SPEED) : pkUnit->GetSkill()->GetAbil(AT_ATT_DELAY);
	int const iSkillDelay = pkUnit->GetSkill()->GetAbil(AT_ATT_DELAY);
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED DelayTime["<<iSkillDelay<<L"]");
#endif
	//int iSkillDelay = 2000;
	if (iSkillDelay > (int)dwElapsedTime)
	{
		pkUnit->SetDelay(iSkillDelay-dwElapsedTime);
	}
	else
	{
		pkUnit->SetDelay(0);
	}
	pkUnit->SetState(US_IDLE);
	pkUnit->GetSkill()->Reserve(0);	// Skill초기화
}

void PgAIActFireSkillDelay::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

void PgAIActFireSkillDelay::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

