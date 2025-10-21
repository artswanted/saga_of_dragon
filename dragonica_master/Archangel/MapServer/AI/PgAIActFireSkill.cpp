#include "stdafx.h"
#include "Variant\constant.h"
#include "PgAIActFireSkill.h"
#include "Variant\PgActionResult.h"
#include "PgGround.h"
#include "PgAiAction.h"
#include "Global.h"

int const DEFAULT_COOL_TIME = 600;
int const DEFAULT_ANIMATION_TIME = 550;
int GetAISkillFireTerm(CUnit * pkUnit)
{
	int iTerm = pkUnit->GetSkill()->GetAbil(AT_AI_SKILL_FIRE_TERM);
	int const iUseCount = pkUnit->GetAbil(AT_AI_SKILL_USE_COUNT);
	if(iUseCount < 10)
	{
		iTerm += pkUnit->GetSkill()->GetAbil(AT_AI_SKILL_FIRE_TERM_00+iUseCount);
	}
	return iTerm;
}

/////////////////////////////////////////////////////////////
// PgAIActFireSkill
/////////////////////////////////////////////////////////////
bool PgAIActFireSkill::CanTransition(CUnit* pkUnit, EAIActionType eCurrentAction, int iActionType, EAIActionType eNextAction, SActArg* pkActArg)
{
	bool bResult = false;
	int iDelay = pkUnit->GetDelay();
	EUnitState eUnitState = pkUnit->GetState();
	
	switch(eNextAction)
	{
	case EAI_ACTION_DELAY:
		{
			// Casting중일 때 얻어맞으면, Cast시간만 늘어날 뿐 Delay Action으로 전이되지는 않는다.
			if(eUnitState == US_ATTACK && iDelay == -1)
			{
				int iAnimTime = pkUnit->GetSkill()->GetAbil(AT_ANIMATION_TIME);
				if(iAnimTime == 0)
				{
					iAnimTime = DEFAULT_ANIMATION_TIME;
				}
				pkUnit->SetDelay(iAnimTime);
				//INFO_LOG(BM::LOG_LV5, _T("___________[%s] Action Time : %d"), __FUNCTIONW__, iAnimTime);

				bResult = true;
			}
		}break;
	case EAI_ACTION_CHASEENEMY:
	case EAI_ACTION_AROUND:
		{
			if(eUnitState == US_CHASE_ENEMY)
			{
				pkUnit->SetDelay(0);
				pkUnit->SetAttackDelay(0);
				bResult = true;
			}
		}break;
	//case EAI_ACTION_IDLE:
	//	{
	//	}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Unknown NextAction ["<<eNextAction<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
		}break;
	}
#ifdef AI_DEBUG
	if (bResult)
	{
		INFO_LOG(BM::LOG_LV9, __FL__<<L"go New AI state eNextAction["<<eNextAction<<L"]"));
	}
#endif
	return bResult;
}

void PgAIActFireSkill::OnEnter(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType ePrevAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	/*
	static DWORD dwdfdTime = 0;
	DWORD dwNow = BM::GetTime32();
	if (dwNow - dwdfdTime < 1000)
	{
		INFO_LOG(BM::LOG_LV9, _T("[%s] Too fast FireSkill"), __FUNCTIONW__);
	}
	dwdfdTime = dwNow;
	*/
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED TIME["<<BM::GetTime32()<<L"]");
#endif
	pkUnit->SetAbil(AT_AI_FIRESKILL_ENTER, 1);
	pkUnit->SetAbil(AT_AI_SKILL_USE_COUNT, 0);
	pkUnit->SetAbil(AT_AI_SKILL_FIRE_TIME, 0);
	pkUnit->NextGoalPos_Clear();
	pkUnit->GetSkill()->CustomData()->Clear();

	bool bDamageAction = true;
	if(pkUnit->GetSkill()->IsForceSkill())
	{
		if(pkUnit->GetSkill()->IsForceSetFlag(EFSF_NO_DMGACT))
		{
			bDamageAction = false;
		}
		BM::Stream kForceParam = pkUnit->GetSkill()->GetForceSkillParam();
		pkUnit->GetSkill()->DoForceSkill();

		if(false == kForceParam.IsEmpty())
		{
			pkActArg->Set(ACTARG_FORCE_SKILL_PARAM, kForceParam);
		}
	}

	if(pkUnit->GetAbil(AT_FIRESKILL_NO_DAMAGEACTION) || false==bDamageAction)
	{
		pkUnit->AddAbil(AT_DAMAGEACTION_TYPE, E_DMGACT_AI_FIRESKILL);
		pkUnit->SendAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL);
	}

	const CSkill* pkSkill = pkUnit->GetSkill();
	if (pkSkill == NULL)
	{
		INFO_LOG(BM::LOG_LV0, __FL__<<L"pkUnit->GetSkill() is NULL Class["<<pkUnit->GetAbil(AT_CLASS)<<L"], Guid["<<pkUnit->GetID()<<L"]");
		pkUnit->SetDelay(0);
		return;
	}
	pkUnit->SetAbil(AT_AI_FIRE_LAST_SKILLNO, pkSkill->GetSkillNo());

	ESkillFireType const eFireType = (ESkillFireType) pkSkill->GetAbil(AT_FIRE_TYPE);
	if (eFireType & EFireType_Moving)
	{
		// 이동속도 변경
		pkUnit->AddAbil(AT_R_MOVESPEED, pkSkill->GetAbil(AT_R_MOVESPEED));
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST);	// 클라이언트에서 직접 이동속도는 조절한다.
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Add MoveSpeed 1 MoveSpeed[%d] Now[%d]"), __FUNCTIONW__, pkSkill->GetAbil(AT_R_MOVESPEED), pkUnit->GetAbil(AT_C_MOVESPEED));
	}

	if(int const iAddEffect = pkSkill->GetAbil(AT_AI_FIRE_ONENTER_ADDEFFECT))
	{
		pkUnit->AddEffect(iAddEffect, 0, pkActArg, pkUnit);
	}
	if(int const iDelEffect = pkSkill->GetAbil(AT_AI_FIRE_ONENTER_DELEFFECT))
	{
		pkUnit->DeleteEffect(iDelEffect);
	}

	ESkillCastType const eCastType = (ESkillCastType) pkSkill->GetAbil(AT_CASTTYPE);
	if( eCastType == E_SCAST_CASTSHOT)
	{
		// 캐스팅 시간 동안 FireSkill Pattern에 머물러 있어야 함
		int iCastTime = pkSkill->GetAbil(AT_CAST_TIME);
		if(pkSkill->GetAbil(AT_APPLY_ATTACK_SPEED) == 1)
		{
			float const fAttackSpeed = pkUnit->GetAbil(AT_C_ATTACK_SPEED)/ABILITY_RATE_VALUE_FLOAT;
			iCastTime = iCastTime / fAttackSpeed;
		}
		pkUnit->SetDelay(iCastTime);
		pkUnit->SetAttackDelay(0);
		pkUnit->SetState(US_SKILL_CAST);
		PgGround* pkGround = NULL;
		pkActArg->Get(ACTARG_GROUND, pkGround);

		BM::Stream kPacket;
		if ( pkGround->AI_SkillCasting(pkUnit, pkActArg, kPacket) == false )
		{
			// ERROR
			pkUnit->SetState(US_CHASE_ENEMY);
			pkUnit->SetDelay(0);
			return;
		}
		PgActionResultVector kResultVec;
		// 어째든 TargetGuid는 보내줘야 한다.
		if(0 == pkSkill->GetAbil(AT_FIRESKILL_NO_TARGET))
		{
			CUnit::DequeTarget const& rkTargetList = pkUnit->GetTargetList();
			CUnit::DequeTarget::const_iterator target_it = rkTargetList.begin();
			while(target_it != rkTargetList.end())
			{
				kResultVec.GetResult((*target_it).kGuid, true);
				++target_it;
			}
		}

		pkGround->SendNfyAIAction(pkUnit, pkUnit->GetState(), &kResultVec, (kPacket.Size() > 0) ? &kPacket : NULL);

	}
	else	// eCastType == E_SCAST_INSTANT
	{
		pkUnit->SetDelay(0);	// Instant Skill 이므로 Cast Delay가 없다.
		pkUnit->SetAttackDelay(0);
		pkUnit->SetState(US_SKILL_CAST);	// Casting완료!
	}
}

void PgAIActFireSkill::OnLeave(CUnit* pkUnit, EAIActionType eAction, int iActionType, EAIActionType eNextAction, DWORD dwElapsedTime, SActArg* pkActArg)
{
#ifdef AI_DEBUG
	INFO_LOG(BM::LOG_LV9, __FL__<<L"ENTERED TIME["<<BM::GetTime32()<<L"]");
#endif

	// 이동 속도 정상화
	ESkillFireType const eFireType = (ESkillFireType) pkUnit->GetSkill()->GetAbil(AT_FIRE_TYPE);
	if (eFireType & EFireType_Moving)
	{
		// 이동속도 변경
		pkUnit->AddAbil(AT_R_MOVESPEED, 0-pkUnit->GetSkill()->GetAbil(AT_R_MOVESPEED));
		pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADCAST);	// 클라이언트에서 직접 이동속도는 조절한다.
		//INFO_LOG(BM::LOG_LV9, _T("[%s] Minus MoveSpeed MoveSpeed[%d] Now[%d]"), __FUNCTIONW__, pkUnit->GetSkill()->GetAbil(AT_R_MOVESPEED), pkUnit->GetAbil(AT_C_MOVESPEED));
	}
	if(pkUnit->GetAbil(AT_AI_FIRE_ONLEAVE_NO_CLEAR_TARGETLIST))
	{
		pkUnit->SetAbil(AT_AI_FIRE_ONLEAVE_NO_CLEAR_TARGETLIST, 0);
	}
	else
	{
		if(false == pkUnit->GetSkill()->IsForceSetFlag(EFSF_NO_SET_TARGETLIST))
		{
			pkUnit->ClearTargetList();
		}
	}


	if(pkUnit->GetAbil(AT_DAMAGEACTION_TYPE)&E_DMGACT_AI_FIRESKILL)
	{
		pkUnit->AddAbil(AT_DAMAGEACTION_TYPE, -E_DMGACT_AI_FIRESKILL);
		pkUnit->SendAbil(AT_DAMAGEACTION_TYPE, E_SENDTYPE_BROADALL);
	}

	int const iForceEffectNo = pkUnit->GetSkill()->GetAbil(AT_REMOVE_SKILL_FORCEEFFECT);
	if(0 < iForceEffectNo)
	{
		CEffect* pkEffect = pkUnit->AddEffect(iForceEffectNo, 0, pkActArg, pkUnit);
		if( !pkEffect )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Null ForceEffectNo <"<<iForceEffectNo<<L"> SkillNo<"<<pkUnit->GetSkill()->GetSkillNo()<<L">");
		}
	}

	pkUnit->SetAbil(AT_AI_FIRESKILL_ENTER, 0);
}

void FireProjectile(CUnit* pkUnit)
{
	if( 0 == pkUnit->GetSkill()->GetAbil(AT_MON_PROJECTILE_ALREADY_SET) )
	{
		if(pkUnit->StartPos() != POINT3::NullData())
		{
			pkUnit->Projectile(0).Begin(pkUnit->StartPos(), pkUnit->GoalPos(), pkUnit->GetSkill()->GetAbil(AT_MOVESPEED));
			pkUnit->StartPos(POINT3::NullData());
		}
		else
		{
			pkUnit->Projectile(0).Begin(pkUnit->GetPos(), pkUnit->GoalPos(), pkUnit->GetSkill()->GetAbil(AT_MOVESPEED));
		}
	}
}

void PgAIActFireSkill::DoAction(CUnit* pkUnit, EAIActionType eAction, int iActionType, DWORD dwElapsedTime, SActArg* pkActArg)
{
	ESkillStatus eSkillStatus = ESS_NONE;

	EUnitState eUnitState = pkUnit->GetState();
	ESkillFireType const eFireType = (ESkillFireType) pkUnit->GetSkill()->GetAbil(AT_FIRE_TYPE);
	if (eUnitState == US_SKILL_CAST && pkUnit->GetDelay() <= 0 && pkUnit->GetAttackDelay() <= 0)
	{
		// Fire 한 상태
		if (eFireType & EFireType_Dur_GoalPos)	// 일정시간 Fire 상태 유지하는 스킬이다.
		{
			// SkillFire 상태 유지 하기 위해 DelayTime 을 세팅한다.
			pkUnit->SetDelay(8000);	// 충분한 시간으로 세팅하고, GoalPos에 도착하면 DelayTime(0) 하자
			pkUnit->SetState(US_SKILL_FIRE);
			if( 0 == (eFireType & EFireType_Moving) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"Skill("<<pkUnit->GetSkill()->GetSkillNo()<<L"):AT_FIRE_TYPE must set EFireType_Moving bit");
				pkUnit->SetDelay(0);
				pkUnit->SetAttackDelay(0);
			}
		}
		else if (eFireType & EFireType_TimeDelay)
		{
			//  일정시간 지난 후 Damage를 줘야 한다.
			int iDelayTime = pkUnit->GetSkill()->GetAbil(AT_MAINTENANCE_TIME2);
			if(0==iDelayTime)
			{
				iDelayTime = pkUnit->GetSkill()->GetAbil(AT_MAINTENANCE_TIME);
			}

			pkUnit->SetDelay(iDelayTime);
			pkUnit->SetState(US_FIRE_WAITING);
		}
		else if (eFireType & EFireType_Dur_Time)
		{
			// SkillFire 상태 유지 하기 위해 DelayTime 을 세팅한다.
			// SkillDef 에서 Delay 시간 얻어 오자.
			pkUnit->SetDelay(pkUnit->GetSkill()->GetAbil(AT_MAINTENANCE_TIME));
			// 진짜 Delay 종료는, Unit이 움직인 후, GoalPos에 도착했으면 SetDelay(0)을 해 주자.
			pkUnit->SetState(US_SKILL_FIRE);
		}


		if( (eFireType & EFireType_Projectile) && !(eFireType & EFireType_TimeDelay))
		{
			// 발사체 발사~~~
			FireProjectile(pkUnit);

			pkUnit->SetDelay(5000);	// 발사체 날아가는 시간이 얼마일지 모르니, 충분한 시간을 준다.
			pkUnit->SetState(US_SKILL_FIRE);
		}

		if (eFireType & EFireType_JumpGoalPos)
		{
			// GoalPos 위치로 Caster를 이동시킨다.
			pkUnit->SetPos(pkUnit->GoalPos());
		}

		eSkillStatus = (eFireType & EFireType_TimeDelay) ? ESS_TIMEWAITING : ESS_FIRE;
	}
	else if (eUnitState == US_SKILL_FIRE)
	{
		PgGround* pkGround = NULL;
		pkActArg->Get(ACTARG_GROUND, pkGround);
		// Target을 다시 잡고 공격 Damage를 줘야 한다.
		eSkillStatus = ESS_TARGETLISTMODIFY;
		//INFO_LOG(BM::LOG_LV9, _T("[%s] US_SKILL_FIRE 01"), __FUNCTIONW__);
		if (eFireType & EFireType_Moving)
		{
			// Caster를 GoalPos로 이동시켜야 한다. (GoalPos값은 Script:Skill_Fire 에서 세팅해야 한다.)
			if(!UnitMoved(pkUnit, pkUnit->GoalPos(), dwElapsedTime, pkGround->PhysXScene()->GetPhysXScene()))
			{
				pkUnit->GoalPos(pkUnit->GetPos());
				pkUnit->SetDelay(0);
			}
			//INFO_LOG(BM::LOG_LV9, _T("[%s] US_SKILL_FIRE 02 MoveSpeed[%d]"), __FUNCTIONW__, pkUnit->GetAbil(AT_C_MOVESPEED));
			if ((eFireType & EFireType_Dur_GoalPos) && GetDistanceQ(pkUnit->GetPos(), pkUnit->GoalPos()) < AI_GOALPOS_ARRIVE_DISTANCE_Q)
			{
				if(pkUnit->IsNextGoalPos())
				{
					pkUnit->GoalPos(pkUnit->NextGoalPos());
					pkUnit->SetDelay( std::max<int>(pkUnit->GetDelay(), 4000) );	//4000은 매직값
				}
				else
				{
					// GoalPos 도착 했으므로, SkillFire 상태 중지 한다.
					pkUnit->SetDelay(0);
					pkUnit->SetAttackDelay(0);
				}
			}
		}
		else if (eFireType & EFireType_Projectile)
		{
			// 발사체를 이동시킨다.
			int iPCount = pkUnit->GetSkill()->GetAbil(AT_MON_PROJECTILE_NUM);
			if( 0 == iPCount )
			{
				iPCount = 1;
			}
			for(int i=0; i<iPCount; ++i)
			{
				SProjectileInfo& rkProjectile = pkUnit->Projectile(i);
				if(!MovingSomething(rkProjectile.ptCurrent, rkProjectile.ptTarget, rkProjectile.iMoveSpeed, dwElapsedTime)
					|| !rkProjectile.Move(dwElapsedTime))
				{
					// GoalPos 도착했으므로 SkillFire 상태 중지
					pkUnit->SetDelay(0);
					pkUnit->SetAttackDelay(0);
					break;
				}
			}
			
			//INFO_LOG(BM::LOG_LV9, _T("[%s] Projectile Pos[%6.1f, %6.1f, %6.1f]"), __FUNCTIONW__, rkProjectile.ptCurrent.x, rkProjectile.ptCurrent.y, rkProjectile.ptCurrent.z);
		}
	}
	else if (eUnitState == US_FIRE_WAITING)
	{
		if (pkUnit->GetDelay() <= 0)	//딜레이가 끝나면
		{
			if (eFireType & EFireType_Dur_Time)
			{
				// SkillFire 상태 유지 하기 위해 DelayTime 을 세팅한다.
				// SkillDef 에서 Delay 시간 얻어 오자.
				pkUnit->SetDelay(pkUnit->GetSkill()->GetAbil(AT_MAINTENANCE_TIME));
				// 진짜 Delay 종료는, Unit이 움직인 후, GoalPos에 도착했으면 SetDelay(0)을 해 주자.
				pkUnit->SetState(US_SKILL_FIRE);

				if(eFireType & EFireType_Projectile)
				{
					// 발사체 발사~~~
					FireProjectile(pkUnit);
				}
			}
			else
			{
				pkUnit->SetDelay(0);
				pkUnit->SetState(US_SKILL_FIRE);		//PT_M_C_NFY_ACTION2패킷에 ESS_TARGETLISTMODIFY를 넣기 위해서 유닛 상태를 바꾼다
				eSkillStatus = ESS_TARGETLISTMODIFY;	//AI_SkillFire를 하기 위해서 상태를 임시로 변경
				// 이번에 진짜로 Damage를 줘야 한다.
				if(eFireType & EFireType_Projectile)
				{
					// 발사체 발사~~~
					FireProjectile(pkUnit);
					pkUnit->SetDelay(5000);	// 발사체 날아가는 시간이 얼마일지 모르니, 충분한 시간을 준다.
				}
			}
		}
	}

	if (eSkillStatus == ESS_FIRE || eSkillStatus == ESS_TARGETLISTMODIFY || (eSkillStatus == ESS_TIMEWAITING && eUnitState == US_SKILL_CAST))
	{																		//막 캐스팅이 끝난 딜레이가 있는 스킬도 일단 AI_SkillFire를 타서 클라에 공격해라고 해 줌
		PgGround* pkGround = NULL;
		pkActArg->Get(ACTARG_GROUND, pkGround);

		bool bUseFileSkill = true;
		if(int const iMaxCount = pkUnit->GetSkill()->GetAbil(AT_AI_SKILL_MAX_COUNT))
		{
			int const iUseCount = pkUnit->GetAbil(AT_AI_SKILL_USE_COUNT);
			bUseFileSkill = (iUseCount < iMaxCount);
		}
		if(bUseFileSkill)
		{
			if(int const iTerm = GetAISkillFireTerm(pkUnit))
			{
				bUseFileSkill = iTerm < BM::GetTime32() - pkUnit->GetAbil(AT_AI_SKILL_FIRE_TIME);
			}
		}

		if (bUseFileSkill && !pkGround->AI_SkillFire(pkUnit, pkActArg, eSkillStatus))
		{
			// TODO : Skill Fire에 실패 했을 때 Chase하게 해야 함.
			/*pkUnit->SetState(US_CHASE_ENEMY);
			pkUnit->SetDelay(0);
			pkUnit->SetAttackDelay(0);
			//INFO_LOG(BM::LOG_LV0, _T("[%s] Failed to cast SkillFire(%d)"), __FUNCTIONW__, pkUnit->GetSkill()->GetSkillNo());
			return;*/
		}
	}

	if (pkUnit->GetDelay() <= 0 && pkUnit->GetAttackDelay() <= 0)
	{
		// SkillFire Delay
		// 이 시간동안 한동안 공격 안함.
		int iCoolTime = NiMax(pkUnit->GetAbil(AT_DEFAULT_ATTACK_DELAY), pkUnit->GetSkill()->GetAbil(AT_DEFAULT_ATTACK_DELAY));
		if(iCoolTime == 0)
		{
			iCoolTime = DEFAULT_COOL_TIME;
		}

		// 애니가 끝난 후 부터, 쿨타임이 시작된다.
		int iAnimTime = pkUnit->GetSkill()->GetAbil(AT_ANIMATION_TIME);
		if(iAnimTime == 0)
		{
			iAnimTime = DEFAULT_ANIMATION_TIME;
		}
		if(pkUnit->GetSkill()->GetAbil(AT_APPLY_ATTACK_SPEED) == 1)
		{
			float const fAttackSpeed = pkUnit->GetAbil(AT_C_ATTACK_SPEED)/ABILITY_RATE_VALUE_FLOAT;
			iAnimTime = iAnimTime / fAttackSpeed;
		}

		if(US_DEAD!=pkUnit->GetState())
		{
			pkUnit->SetState(US_ATTACK);
		}
		pkUnit->SetAttackDelay(iCoolTime + iAnimTime);
		pkUnit->SetDelay(-1);

		EUnitDieType const eAfterDieType = static_cast<EUnitDieType>(pkUnit->GetSkill()->GetAbil(AT_MON_SKILL_AFTER_DIE));
		if( (pkUnit->GetAbil(AT_USE_SKILL_ON_DIE) && !pkUnit->GetSkill()->IsForceSkill())
		 || eAfterDieType )
		{
			pkUnit->SetAbil(AT_DIE_TYPE, pkUnit->GetAbil(AT_DIE_TYPE)|eAfterDieType);
			pkUnit->SetAbil(AT_HP, 0);
			pkUnit->SetAttackDelay(0);
		}
	}
}

