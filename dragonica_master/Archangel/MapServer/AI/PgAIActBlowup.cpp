#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActBlowup.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

extern bool CheckRetreatHP(CUnit* pkUnit);
/////////////////////////////////////////////////////////////
// PgAIActBlowup
////////////////////////////////////////////////////////////
bool PgAIActBlowup::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	if (0 < pkUnit->GetDelay() || pkUnit->StandUpTime())
	{
		return false;
	}
	bool bResult = false;
	switch(eNextAction)
	{
	case EAI_ACTION_CALLHELP:
	case EAI_ACTION_RETREAT:
		{
			bResult = CheckRetreatHP(pkUnit);
		}break;
	case EAI_ACTION_FIRESKILL:
		{
			//if (pkUnit->GetDelay() <= 0)
			{
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				bResult = pkGround->CanAttackTarget(pkUnit);
			}
		}break;
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_KEEP_DISTANCE:
		{
			//if (pkUnit->GetDelay() <= 0)
			{
				PgGround* pkGround = NULL;
				pkActArg->Get(ACTARG_GROUND, pkGround);
				bResult = pkGround->IsTargetInRange(pkUnit);
			}
		}break;
    case EAI_ACTION_IDLE:
		{
			//if (pkUnit->GetDelay() <= 0)
			{
				bResult = true;
			}
		}break;
	case EAI_ACTION_ROAD:
		{
			int const iGroup = pkUnit->GetAbil(AT_ROADPOINT_GROUP);
			bResult = (0 != iGroup);
		}break;
	case EAI_ACTION_WAIT_COOLTIME:
		{
			//if (pkUnit->GetDelay() <= 0)
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

void PgAIActBlowup::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED Delay["<<pkUnit->GetDelay()<<L"]");
#endif
	pkUnit->SetSync(false);	//블로우업은 싱크를 시키면 안된다
	pkUnit->SetState(US_IDLE);
	// Delay Time 은 이미 설정되었다.
	//pkUnit->SetSync(true);
}

void PgAIActBlowup::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);

	int const iEffectNo = pkUnit->GetAbil(AT_AI_BLOWUP_EFFECT);
	if(0 < iEffectNo)
	{
		CEffect* pkEffect = pkUnit->AddEffect(iEffectNo, 0, pkActArg, pkUnit);
		if( !pkEffect )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Null EffectNo <"<<iEffectNo<<L">");
		}
	}
}

void PgAIActBlowup::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	//INFO_LOG(BM::LOG_LV5, _T("[%s] not implemented"), __FUNCTIONW__);
}

