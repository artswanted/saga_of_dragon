#include "stdafx.h"
#include "Variant\Constant.h"
#include "PgAIManager.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"


void ForceSkillCanAttackTarget(CUnit * pkUnit, SActArg * pkActArg);
void ForceSkillCanAttackTarget(CUnit * pkUnit, int const iSkillNo, SActArg * pkActArg);


PgAIManager::PgAIManager(void)
{
	m_pkGround = NULL;
}

PgAIManager::~PgAIManager(void)
{
}

bool PgAIManager::Init(PgGround* pkGround)
{
	if( !pkGround )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"pkGround is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	m_pkGround = pkGround;
	// MONAREA :
	//m_pkMonAreaMng = pkGround->GetMonAreaMng();
	if( !m_kActionMng.Init(pkGround) )	
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV4, "ActionMng Init failed"); 
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("ActionMng Init Failed!"));
	}
	return true;
}
bool PgAIManager::ReloadWaypoint(PgGround& rkGround)
{
	m_pkGround = &rkGround;
	return m_kActionMng.Reload(rkGround);
}

// AI function....
//	STAGE 1 : 다음 액션으로 갈 수 있는가 검사
//  STAGE 2 : DoAction
//	STAGE 3 : 갈 수 있다면, 다음 Action으로 전이
//	STAGE 4 : 동기화 패킷 만들어 보내기
bool PgAIManager::RunAI(CUnit* pkUnit, DWORD dwElapsed)
{
#ifdef AI_DEBUG
//	INFO_LOG(BM::LOG_LV9, _T("[%s] Elapsed[%d]"), __FUNCTIONW__, dwElapsed);
	//if (pkUnit->IsUnitType(UT_MONSTER))
	//{
	//	return true;
	//}
#endif
	int iDelay = pkUnit->GetDelay();
	int iAttackDelay = pkUnit->GetAttackDelay();

	// Delay : 몬스터는 아무런 행동도 해서는 안되는 시간
	// AttackDelay : 몬스터는 단지 공격을 할 수 없을 뿐
	pkUnit->SetDelay(std::max((int)(iDelay - dwElapsed), 0));
	pkUnit->SetAttackDelay(std::max((int)(iAttackDelay - dwElapsed), 0));

	SActArg kActArg;
	PgGroundUtil::SetActArgGround(kActArg, m_pkGround);
	//kActArg.Set(ACTARG_DELAYTIME, (void*)iDelay);

	SUnit_AI_Info * pkInfo = pkUnit->GetAI();
	int const iActionType = pkInfo->GetActionType(pkInfo->eCurrentAction);
	DoCurrentAction(pkUnit, pkInfo->eCurrentAction, iActionType, dwElapsed, &kActArg);

	EAIActionType eNextAction = GetNextActionType(pkUnit, pkInfo, &kActArg);
	eNextAction = GetCheckForceSkill(pkUnit, eNextAction, &kActArg);

	if (eNextAction != EAI_ACTION_NONE)
	{
		int const iNewActionType = pkInfo->GetActionType(eNextAction);
		BeginNewAction(pkUnit, eNextAction, iNewActionType, dwElapsed, &kActArg);
	}

	return true;
}

EAIActionType PgAIManager::GetNextActionType(CUnit* pkUnit, SUnit_AI_Info * pkAI, SActArg* pkActArg)
{
	// Event 를 검사한다.
	EAIActionType eNextAction = CheckAIEvent(pkUnit, pkAI, pkActArg);
	if (eNextAction != EAI_ACTION_NONE)
	{
		return eNextAction;
	}

	// Pattern 을 검사한다.
	SUnit_AI_Info * pkInfo = pkUnit->GetAI();
	SPatternActionInfo const * pkNextAction = NULL;
	PgAIPattern::VECTOR_PATTERN_ACTION_INFO const * pkNextActionVec = NULL;
	
	if(g_kPatternMng.GetNextAction(pkInfo->sPattern, pkInfo->eCurrentAction, pkNextActionVec) && NULL!=pkNextActionVec)
	{
		// Action Transition 이 가능한가 점검한다.
		size_t const action_size = pkNextActionVec->size();
		for(size_t i=0; i< action_size; i++)
		{
			SPatternActionInfo const * pkPatternAction = pkNextActionVec->at(i);
			int const iActionType = pkInfo->GetActionType(pkInfo->eCurrentAction);
			if (m_kActionMng.CanTransition(pkUnit, pkInfo->eCurrentAction, iActionType, pkPatternAction->eTo, pkActArg))
			{
				pkNextAction = pkPatternAction;
				break;
			}
		}
	}
	return (pkNextAction == NULL) ? EAI_ACTION_NONE : pkNextAction->eTo;
}

EAIActionType PgAIManager::GetCheckForceSkill(CUnit * pkUnit, EAIActionType const eNextAction, SActArg* pkActArg)
{
	if(EAI_ACTION_FIRESKILL==eNextAction)
	{
		return eNextAction;
	}

	if( pkUnit->GetSkill()->IsForceSetFlag(EFSF_NOT_RUN_BLOWUP) )
	{
		if( EAI_ACTION_BLOWUP==pkUnit->GetAI()->eCurrentAction && eNextAction==EAI_ACTION_NONE )
		{
		}
		else
		{
			ForceSkillCanAttackTarget(pkUnit, pkActArg);
			return EAI_ACTION_FIRESKILL;
		}
	}
	else if( 0==pkUnit->GetAbil(AT_AI_FIRESKILL_ENTER) && 0==pkUnit->GetAttackDelay() && pkUnit->GetSkill()->IsForceSkill() )
	{
		ForceSkillCanAttackTarget(pkUnit, pkActArg);
		return EAI_ACTION_FIRESKILL;
	}

	return eNextAction;
}

void PgAIManager::BeginNewAction(CUnit* pkUnit, EAIActionType eNewAction, int iActionType, DWORD dwElapsed, SActArg* pkActArg)
{
	const SUnit_AI_Info* pkInfo = pkUnit->GetAI();
	if (pkInfo->eCurrentAction != EAI_ACTION_NONE)
	{
		m_kActionMng.OnLeave(pkUnit, pkInfo->eCurrentAction, pkInfo->GetActionType(pkInfo->eCurrentAction), eNewAction, dwElapsed, pkActArg);
	}
	m_kActionMng.OnEnter(pkUnit, eNewAction, iActionType, pkInfo->eCurrentAction, dwElapsed, pkActArg);
	pkUnit->SetAICurrentAction(eNewAction);
}

void PgAIManager::DoCurrentAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsed, SActArg* pkActArg)
{
	m_kActionMng.DoAction(pkUnit, eAction, iActionType, dwElapsed, pkActArg);
}

void PgAIManager::Release()
{
	m_pkGround = NULL;
	m_kActionMng.Release();
}

EAIActionType PgAIManager::CheckAIEvent(CUnit* pkUnit, SUnit_AI_Info * pkAI, SActArg* pkActArg)
{
	EAIActionType eNextAction = EAI_ACTION_NONE;

	PgGround *pkGround = 0;
	pkActArg->Get(ACTARG_GROUND, pkGround);
	if(!pkGround)
	{
		return eNextAction;
	}

	switch(pkAI->GetEvent())
	{
	case EAI_EVENT_CANNOTMOVE_FORWORD:
		{
			eNextAction = EAI_ACTION_RETREAT;
		}break;
	case EAI_EVENT_DAMAGED:
		{
			//INFO_LOG(BM::LOG_LV5, __FUNCTION__<<" EAI_EVENT_DAMAGED");
			if(0>=pkUnit->StandUpTime())
			{
				int iRate = pkUnit->GetAbil(AT_AI_TARGETTING_RATE);
				if(0 == iRate)
				{
					iRate = ABILITY_RATE_VALUE;//기본적으로 30%
				}

				if (iRate > BM::Rand_Index(ABILITY_RATE_VALUE))
				{
					BM::GUID kGuid;
					pkUnit->GetTargetFromAggro(kGuid);	//어그로가 제일 높은 애
					if(BM::GUID::IsNull(kGuid))	//없으면 날 때린놈
					{
						kGuid = pkAI->GetEventCaster();
					}								//있으면 어그로가 제일 높은 애
					CUnit* pkCaster = pkGround->GetUnit(kGuid);
					kGuid = pkCaster ? pkCaster->Caller() : BM::GUID::NullData();

					pkUnit->SetTarget(kGuid);
					//INFO_LOG(BM::LOG_LV5, "EAI_EVENT_DAMAGED GUID : "<<kGuid);
				}	

				EAIActionType eActionType = pkUnit->GetAI()->eCurrentAction;
				EUnitState eUnitState = pkUnit->GetState();
				//INFO_LOG(BM::LOG_LV3,"EAI_EVENT_DAMAGED eActionType "<<eActionType<<" No "<<pkUnit->GetSkill()->GetSkillNo()<<" UnitState "<<eUnitState);
				if(US_ATTACK!=eUnitState)
				{
					if(eActionType == EAI_ACTION_FIRESKILL && eUnitState == US_SKILL_FIRE)	//공격도중 맞는다고 해서 AI가 바뀌면 이동스킬 사용시 난감함
					{
						pkAI->SetEvent(BM::GUID::NullData(), EAI_EVENT_NONE);
					}
					else if(eActionType != EAI_ACTION_DELAY)
					{
						//INFO_LOG(BM::LOG_LV5, __FUNCTION__<<" eNextAction = EAI_ACTION_DELAY");
						eNextAction = EAI_ACTION_DELAY;
					}
					else
					{
						if(g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_CHASEENEMY))
						{
							pkUnit->SetState(US_CHASE_ENEMY);
							eNextAction = EAI_ACTION_CHASEENEMY;
						}
						else
						{
							eNextAction = EAI_ACTION_DELAY;
						}
					}
				}
			}

			//if(pkGround->CanAttackTarget(pkUnit))
			//{
			//	if(pkUnit->GetAI()->eCurrentAction != EAI_ACTION_FIRESKILL)
			//	{
			//		eNextAction = EAI_ACTION_FIRESKILL;
			//	}
			//}
			//else
			//{
			//	eNextAction = EAI_ACTION_CHASEENEMY;
			//}
		}break;
	case EAI_EVENT_CHASE:
		{
//			if(0>=pkUnit->StandUpTime())
			{
				if (pkUnit->GetTarget() == BM::GUID::NullData())
				{
					pkUnit->SetTarget(pkAI->GetEventCaster());
				}

				if(pkUnit->GetAI()->eCurrentAction != EAI_ACTION_FIRESKILL)	//Chase가 있는 놈들만 Chase하자
				{
					if(g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_CHASEENEMY))
					{
						eNextAction = EAI_ACTION_CHASEENEMY;
					}
				}

				if(pkAI->IsEventParam(EAI_EVENT_PARAM_CHASE_UNCONDITIONALLY))
				{
					if(g_kPatternMng.IsPatternExist(pkUnit->GetAbil(AT_AI_TYPE), EAI_ACTION_CHASEENEMY))
					{
						eNextAction = EAI_ACTION_CHASEENEMY;
					}
				}

				if(eNextAction==EAI_ACTION_CHASEENEMY && pkAI->IsEventParam(AT_AI_TYPE_NO_DAMAGEACTION))
				{
					pkUnit->SetAbil(AT_AI_TYPE_NO_DAMAGEACTION, EAI_ACTION_CHASEENEMY);
				}

				if(pkAI->IsEventParam(EAI_EVENT_PARAM_CHASE_GOAL_POS))
				{
					pkUnit->SetAbil(AT_AI_CHASE_GOAL_POS,1);
				}
			}
		}break;
	case EAI_EVENT_BLOWUP:
		{
			eNextAction = EAI_ACTION_BLOWUP;
			if (pkUnit->GetTarget() == BM::GUID::NullData())
			{
				pkUnit->SetTarget(pkAI->GetEventCaster());
			}
			pkUnit->SetDelay(__max(1000, pkUnit->GetDelay()) );
			pkUnit->StandUpTime(pkUnit->GetDelay());
			pkUnit->SetAbil(AT_POSTURE_STATE, 1);
			pkUnit->SetAbil(AT_DAMAGE_EFFECT_DURATION, __max(1000, pkUnit->GetAbil(AT_DAMAGE_EFFECT_DURATION)));
			//pkGround->GetUnit(pkAI->GetEventCaster())->SendWarnMessageStr((BM::vstring)__FL__<<" EAI_EVENT_BLOWUP", EL_Normal);
		}break;
	case EAI_EVENT_CALLED_HELP:
		{
			eNextAction = EAI_ACTION_PATROL;
		}break;
	case EAI_EVENT_DIE_SKILL:
		{
			int const iDieSkillNo = pkUnit->GetAbil(AT_SKILL_ON_DIE);
			if (pkUnit->GetSkill() && iDieSkillNo)
			{
				pkUnit->SetAbil(AT_USE_SKILL_ON_DIE, 1, true, true);
				pkUnit->GetSkill()->ClearForceReserve();
				pkUnit->GetSkill()->Reserve(iDieSkillNo, true);
				CSkillDef const* pkSkillDef = pkUnit->GetSkill()->GetSkillDef();
				int const iAnimationTime = pkSkillDef ? pkSkillDef->m_iAnimationTime : 0;
				if(0>=iAnimationTime)
				{
					pkUnit->SetDelay(0);
					pkUnit->SetAttackDelay(0);
				}

				ForceSkillCanAttackTarget(pkUnit, iDieSkillNo, pkActArg);
				eNextAction = EAI_ACTION_FIRESKILL;
			}
		}break;
	case EAI_EVENT_FORCE_SKILL:
		{
			//지금 발동중인 스킬이 캔슬되도 되는가?
			if( pkUnit->GetAbil(AT_AI_FIRESKILL_ENTER) || pkUnit->GetAttackDelay() )
			{
				bool bIgnore = false;
				//if(pkUnit->GetAttackDelay())
				{
					GET_DEF(CSkillDefMgr, kSkillDefMgr);
					if( pkUnit->GetSkill()->GetAbil(AT_IGNORE_NOW_FIRESKILL_CANCEL)
					 || (pkUnit->GetAttackDelay() && kSkillDefMgr.GetAbil(pkUnit->GetAbil(AT_AI_FIRE_LAST_SKILLNO), AT_IGNORE_NOW_FIRESKILL_CANCEL)) )
					{
						bIgnore = true;
					}
				}

				if( pkUnit->GetSkill()->IsForceSetFlag(EFSF_NOW_FIRESKILL_CANCLE)
				 && false==bIgnore )
				{
					pkUnit->SetAbil(AT_AI_FIRESKILL_ENTER, 0);
					pkUnit->SetAttackDelay(0);
				}
			}

			//현재 스킬이 발동중이면 Fire는 기다리게 된다
			if(0==pkUnit->GetAbil(AT_AI_FIRESKILL_ENTER) && 0==pkUnit->GetAttackDelay())
			{
				if( pkUnit->GetSkill()->IsForceSetFlag(EFSF_NOT_RUN_BLOWUP) )
				{
					pkAI->SetEvent(BM::GUID::NullData(), EAI_EVENT_NONE);
				}
				else
				{
					ForceSkillCanAttackTarget(pkUnit, pkActArg);
					eNextAction = EAI_ACTION_FIRESKILL;
				}
			}
		}break;
	case EAI_EVENT_RETURN_WAYPOINT:
		{
			PgMonster* pkMonster = dynamic_cast<PgMonster*>(pkUnit);
			if(pkMonster)
			{
				pkMonster->AddIgnoreTarget(pkGround->GetUnit(pkAI->GetEventCaster()), BM::GetTime32());
			}
			eNextAction = EAI_ACTION_PATROL;
		}break;
	case EAI_EVENT_IDLE:
		{
			eNextAction = EAI_ACTION_IDLE;
		}break;
	}

	if (eNextAction != EAI_ACTION_NONE)
	{
		pkAI->ClearEvent();
	}
	return eNextAction;
}

bool CheckRetreatHP(CUnit* pkUnit)
{
	if(pkUnit)
	{
		int iCheckHP = pkUnit->GetAbil(AT_R_RETREAT_HP);
		if (iCheckHP > 0)
		{
			int const iMaxHP = pkUnit->GetAbil(AT_C_MAX_HP);
			int const iHP = pkUnit->GetAbil(AT_HP);
			if ((iMaxHP * iCheckHP / 1000) > iHP)
			{
				return true;
			}
		}
	}

	return false;
}

void ForceSkillCanAttackTarget(CUnit * pkUnit, SActArg * pkActArg)
{
	int const iSkillNo = pkUnit->GetSkill()->GetForceSkillNo();
	if(pkUnit->GetSkill()->IsForceSetFlag(EFSF_NO_SET_TARGETLIST))
	{
		return;
	}

	ForceSkillCanAttackTarget(pkUnit, iSkillNo, pkActArg);
}

void ForceSkillCanAttackTarget(CUnit * pkUnit, int const iSkillNo, SActArg * pkActArg)
{
	if(!pkActArg)
	{
		return;
	}

	PgGround *pkGround = 0;
	pkActArg->Get(ACTARG_GROUND, pkGround);

	if(pkGround)
	{
		UNIT_PTR_ARRAY kTargetArray;
		if (pkGround->AI_GetSkillTargetList(pkUnit, iSkillNo, kTargetArray, false, pkActArg))
		{
			pkUnit->SetTargetList(kTargetArray);
			pkUnit->SetAbil(AT_AI_FIRE_ONLEAVE_NO_CLEAR_TARGETLIST, 1);
		}
	}
}