#include "stdafx.h"
#include "PgSkillFunction.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgEffectFunction.h"

///////////////////////////////////////////////////////////
//  PgISkillFunction
///////////////////////////////////////////////////////////
PgISkillFunction::PgISkillFunction()
{}

PgISkillFunction::~PgISkillFunction()
{
	Release();
}

void PgISkillFunction::Init()
{
}

void PgISkillFunction::Release()
{
}

void PgISkillFunction::Build()
{
}

int PgISkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return 1;
}

int PgISkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return 1;
}

int PgISkillFunction::SkillToggle(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return 1;
}

int	PgISkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return 1;
}

bool PgISkillFunction::SkillFail(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return false;
}

bool PgISkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	if (pkUnit->UnitType() == UT_PLAYER)
	{//플레이어일때만 문제가 된다
//		INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	}
	return true;
}

int PgISkillFunction::SkillPassive(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, iSkillNo);
	return 1;
}

int PgISkillFunction::SkillPCheck(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, pkSkill->No());
	return 1;
}

void PgISkillFunction::SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, pkSkill->No());
}
void PgISkillFunction::SkillEnd(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, pkSkill->No());
}

int PgISkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg)
{
//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't call me SkillNo[%d] "), __FUNCTIONW__, pkSkill->No());
	return 0;
}

void PgISkillFunction::CheckTagetVaild(CUnit* pkUnit, UNIT_PTR_ARRAY* pkUnitArray, CSkillDef const* pkSkill, int const iDmgCheckTime)
{
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	int iRealTime = iDmgCheckTime;
	if(pkSkill)
	{
		int iInter = pkSkill->GetAbil(AT_DAMAGECHECK_INTER);
		if(0 < iInter)
		{
			iRealTime = iInter;
		}
	}
	while(pkUnitArray->end() != unit_itor)	//먼저 타겟이 되는지 검사하고 타겟이 안되는 놈들은 목록에서 제거
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (!pkTarget || !pkUnit->IsTarget(pkTarget, true, ESTARGET_ENEMY, pkSkill, iRealTime))
		{
			(*unit_itor).bRestore = true;
			++unit_itor;
		}
		else
		{
			++unit_itor;
		}
	}
}

///////////////////////////////////////////////////////////
//  PgDefaultSkillFunction
///////////////////////////////////////////////////////////
PgDefaultSkillFunction::PgDefaultSkillFunction()
{}

PgDefaultSkillFunction::~PgDefaultSkillFunction()
{}

int PgDefaultSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream* pkPacket)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	int iBasicSkill = kSkillDefMgr.GetCallSkillNum(iSkillNo);

//	INFO_LOG(BM::LOG_LV6, _T("[%s] Don't Call Function [Skill_Begin%d][%d]"), __FUNCTIONW__, iBasicSkill,iSkillNo);
	return 1;
}

int PgDefaultSkillFunction::SkillToggle(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo); 
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}
	if(!pkGround)
	{
		return 0;
	}
	
	PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);

	int const iEffectNo = pkSkill->GetEffectNo();
	if(bToggleOn)
	{
		pArg->Set(ACTARG_TOGGLESKILL, pkSkill->No());
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
		{// 연관된 자식 스킬의 이펙트가 걸려있는지 확인한 후 걸어주고
			SkillFuncUtil::DoChildSkillEffect(pkPlayer, iSkillNo, false, pArg);
		}
		if( 0 < pkSkill->GetAbil( AT_DEL_SUB_PLAYER )
			&& UT_PLAYER == pkUnit->UnitType() 
			)
		{
			if(pkPlayer)
			{
				pkGround->DeleteSubPlayer( pkPlayer->SubPlayerID() );
			}
		}

	}
	else
	{
		pkUnit->DeleteEffect(iEffectNo, true);
		
		{// 연관된 자식 스킬의 이펙트가 걸려있는지 확인한후에 Delete 시켜주고
			SkillFuncUtil::DoChildSkillEffect(pkPlayer, iSkillNo, true, NULL);
		}

		int const iAddEfefect = pkSkill->GetAbil(AT_ADD_EFFECT_WHEN_SKILL_END);
		if(0 < iAddEfefect)
		{
			pkUnit->AddEffect(iAddEfefect, 0, pArg, pkUnit);
		}

		// Client에게 알려주기
		BM::Stream kTPacket(PT_M_C_NFY_TOGGLSKILL_ON_OFF, iSkillNo);
		kTPacket.Push(false);
		kTPacket.Push(pkUnit->GetID());
		pkUnit->Send(kTPacket, E_SENDTYPE_BROADALL|E_SENDTYPE_MUSTSEND);

		if(pkPlayer
			&& 0 < pkSkill->GetAbil( AT_DEL_SUB_PLAYER ) 
			)
		{
			BM::GUID kGuid;
			kGuid.Generate();
			pkGround->CreateSubPlayer(pkPlayer, kGuid);
		}
	}
	return 1;
}

int PgDefaultSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	//아무 일도 하지 않음
	
	return 1;
}

bool  PgDefaultSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	//Lua Script Function Skill_CanReserveDefault 내용없음
	return true;
}

int PgDefaultSkillFunction::SkillPassive(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	
	if(!pkUnit->CheckPassive(pkSkill->No(), pArg))
	{
		return -1;
	}

	return SkillPCheck(pkUnit, pkSkill, pArg);
}

int PgDefaultSkillFunction::SkillPCheck(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	bool bOn = false;
	int const iEffectNo = pkSkill->GetEffectNo();
	PgUnitEffectMgr& rkEffectMgr = pkUnit->GetEffectMgr();

	if(rkEffectMgr.FindInGroup(iEffectNo, true))
	{
		// 이미 존재하는 Effect 
		bOn = true;
	}

	if(!pkUnit->CheckPassive(pkSkill->No(), pArg))
	{
		if(bOn)
		{
			SkillEnd(pkUnit, pkSkill, pArg);
		}
		return -1;
	}

	if(!bOn)
	{
		SkillBegin(pkUnit, pkSkill, pArg);
	}
	return 0;
}

void PgDefaultSkillFunction::SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg) // Passive용 Begin
{
	int const iEffectNo = pkSkill->GetEffectNo();

	if(0 < iEffectNo)
	{
		CEffect* pkEffect = pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
		if(pkEffect)
		{
			pkEffect->SetActArg(ACTARG_WEAPONLIMIT, pkSkill->GetAbil(AT_WEAPON_LIMIT));
		}
	}
}

void PgDefaultSkillFunction::SkillEnd(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg)
{
	int const iEffectNo = pkSkill->GetEffectNo();
	pkUnit->DeleteEffect(iEffectNo);
}

///////////////////////////////////////////////////////////
//  PgDefaultOnlyAddeffectSkillFunction
///////////////////////////////////////////////////////////
int PgDefaultOnlyAddeffectSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}
	
	int const iAddMaxHPRate = pkSkill->GetAbil(AT_R_MAX_HP);	// MAXHP의 비율로 HP를 회복함

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%	
	
	int const iEffectNo = pkSkill->GetEffectNo();
	GET_DEF(CEffectDefMgr, kEffectDefMgr);
	CEffectDef const* pkEffect = kEffectDefMgr.GetDef(iEffectNo);
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	CUnit* pkUnit2 = pkUnit;
	//Entity일 경우 부모의 Unit을 찾아서 Unit2에 세팅해야 한다.
	BM::GUID const& kParentGUID = pkUnit->Caller();
	if(BM::GUID::IsNotNull(kParentGUID) && pkUnit->GetID() != kParentGUID)
	{
		if(pkGround)
		{
			CUnit* pkParentUnit = pkGround->GetUnit(kParentGUID);
			if(pkParentUnit)
			{
				pkUnit2 = pkParentUnit;
			}
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"kGround is NULL");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		}
	}
	
	// HP 회복 시켜주는 어빌이 있을경우 자신에게 적용
	//SkillFuncUtil::OnModifyHP()

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		// 참고용 유닛은 리스트에서 지나친다.
		if(true == (*unit_itor).bReference
			|| NULL == (*unit_itor).pkUnit
			)
		{
			++unit_itor;
			continue;
		}

		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		
		SkillFuncUtil::OnModifyHP(pkTarget, NULL, iAddMaxHPRate, 0, pkUnit, pkGround);
		
		if(pkEffect)
		{
			// 디버프 타입일 경우에는 미스 확률을 적용 시킨다.
			if(EFFECT_TYPE_CURSED == pkEffect->GetType())
			{
				int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
				int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
				if (iDecHitRate < iRandValue)
				{
					// 타격 실패 (Missed)
					if(pkTarget != pkUnit)
					{
						pkAResult->SetMissed(true);
						++unit_itor;
						continue;
					}
				}
			}

			if(pkAResult && !pkAResult->GetInvalid())
			{			
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

				if(iRandValue <= iEffectPercent)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}

				//캐스터가 PgPlayer라면
				if(PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit2))
				{
					int iSkillindex = 0;
					while(CHILD_SKILL_MAX > iSkillindex)
					{
						//영향 받는 베이스 스킬
						int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01 + iSkillindex);
						if(0 < iChildBaseSkillNo)
						{
							if(PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill())
							{
								//해당 스킬의 실제 레벨에 해당되는 스킬을 얻어 온다.
								int const iLearnedChildSkillNo = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
								if(0 < iLearnedChildSkillNo)
								{
									if(CSkillDef const* pkLearnedSkillDef = kSkillDefMgr.GetDef(iLearnedChildSkillNo))
									{
										// 추가 버프를 거는 경우
										int iChildEffectNo = pkLearnedSkillDef->GetEffectNo();
										if(0 < iChildEffectNo)
										{
											if(iRandValue <= iEffectPercent)
											{
												pkTarget->AddEffect(iChildEffectNo, 0, pArg, pkUnit);
											}
										}
									}
								}
							}
						}
						else
						{
							break;
						}
						++iSkillindex;
					}			
				}
			}
		}		

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgDefaultOnlyDamageSkillFunction
///////////////////////////////////////////////////////////
int PgDefaultOnlyDamageSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkill) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1; 
	}
	
	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}

	if(!pkUnitArray || pkUnitArray->empty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	SkillFuncUtil::OnAddSkillLinkagePoint(pkUnit, pkSkill, pkGround);					// 연계 포인트를 누적할 수 있다면
	SkillFuncUtil::UseSkillLinkagePoint(pkUnit, pkSkill, pkGround);		// 연계 포인트를 사용하는 스킬이라면

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);	

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{		
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && pkAResult->GetValue() && !pkAResult->GetMissed())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}
	SkillFuncUtil::AddEffectWhenHit(pkSkill, pArg, pkUnit); // 빗나감과 관계없이 이펙트 걸어 이펙트를 걸어주고

	int const iDelEffectNo = pkSkill->GetAbil(AT_DELETE_EFFECT_NO);
	if(0 < iDelEffectNo)
	{// 연계 스킬 정보가 들어있는 이펙트를 삭제해야 함
		pkUnit->DeleteEffect(iDelEffectNo, true);
	}

	return 1;
}

bool  PgDefaultOnlyDamageSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	return true;
}

int PgDefaultOnlyDamageSkillFunction::SkillToggle(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo); 

	int const iEffectNo = pkSkill->GetEffectNo();
	if(bToggleOn)
	{
		pArg->Set(ACTARG_TOGGLESKILL, pkSkill->No());
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}
	else
	{
		pkUnit->DeleteEffect(iEffectNo, true);
		int const iAddEfefect = pkSkill->GetAbil(AT_ADD_EFFECT_WHEN_SKILL_END);
		if(0 < iAddEfefect)
		{
			pkUnit->AddEffect(iAddEfefect, 0, pArg, pkUnit);
		}

		// Client에게 알려주기
		BM::Stream kTPacket(PT_M_C_NFY_TOGGLSKILL_ON_OFF, iSkillNo);
		kTPacket.Push(false);
		kTPacket.Push(pkUnit->GetID());
		pkUnit->Send(kTPacket, E_SENDTYPE_BROADALL|E_SENDTYPE_MUSTSEND);
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  PgDefaultOnlyDamageMonSkillFunction
///////////////////////////////////////////////////////////
int PgDefaultOnlyDamageMonSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgDefaultOnlyDamageMonSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill) 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1; 
	}
	
	if (!pkUnitArray || pkUnitArray->empty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && pkAResult->GetValue())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgDefaultOnlyAddeffectMonSkillFunction
///////////////////////////////////////////////////////////
int PgDefaultOnlyAddeffectMonSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgDefaultOnlyAddeffectMonSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid())
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);

					int iSkillindex = 0;
					while(iSkillindex < EFFECTNUM_MAX)		//추가이펙트 걸기
					{
						int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1 + iSkillindex);
						if(0 < iAddEffect)
						{
							pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
						}
						else
						{
							break;
						}
						++iSkillindex;
					}
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgDefaultDamageAndEffectSkillFunction
//  특징 : 이펙트 Skill Effect는 시전자에게, AT_EFFECTNUM는 상대방에게 걸림(AT_PERCENTAGE 확률에 따라 적용)
///////////////////////////////////////////////////////////
int PgDefaultDamageAndEffectSkillFunction::DoAction(CUnit * pkUnit, CSkillDef const* pkSkill, SActArg const* pArg, UNIT_PTR_ARRAY * pkUnitArray, PgActionResultVector * pkResult)
{
	if(!pkUnit || !pkSkill || !pArg || !pkUnitArray || !pkResult)
	{
		return 0;
	}

	int iIndex = 0;
	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(pkSkill->No(), pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && pkAResult->GetValue())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
			int const iDamage = pkAResult->GetValue();	// DoFinalDamage에 의해 데미지가 변경 되었을수 있음
			if( iDamage )
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					for(int i=0; i<EFFECTNUM_MAX; ++i)
					{
						int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1 + i);
						if(0 < iAddEffect)
						{
							pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
						}
						else
						{
							break;
						}
					}
				}
				pkTarget->SetTarget(pkUnit->GetID());
			}
			++iIndex;
		}

		++unit_itor;
	}

	return iIndex;
}

int PgDefaultDamageAndEffectSkillFunction::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgDefaultDamageAndEffectSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return 0;
	}

	if (!pkResult)
	{
		return 0;
	}

	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}

	return PgDefaultDamageAndEffectSkillFunction::DoAction(pkUnit, pkSkill, pArg, pkUnitArray, pkResult);
}


///////////////////////////////////////////////////////////
//  PgDefaultDamageAndEffectSkillFunction2
//  특징 : Begin때 이펙트 Skill Effect는 시전자에게
//		   Fire때 데미지와 AT_EFFECTNUM를 상대방에게 검(AT_PERCENTAGE 확률에 따라 적용)
///////////////////////////////////////////////////////////
int PgDefaultDamageAndEffectSkillFunction2::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}
	return 1;
}

int PgDefaultDamageAndEffectSkillFunction2::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return 0;
	}

	if (!pkResult)
	{
		return 0;
	}

	return PgDefaultDamageAndEffectSkillFunction::DoAction(pkUnit, pkSkill, pArg,  pkUnitArray, pkResult);
}


///////////////////////////////////////////////////////////
//  PgDefaultDamageAndEffectSkillFunction3
//  특징 : Fire때 Target이 없어도 시전자에게 이펙트 적용
///////////////////////////////////////////////////////////
int PgDefaultDamageAndEffectSkillFunction3::SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket)
{
	return 1;
}

int PgDefaultDamageAndEffectSkillFunction3::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}

	if (!pkUnitArray || pkUnitArray->empty())
	{
		return 0;
	}

	if (!pkResult)
	{
		return 0;
	}

	return PgDefaultDamageAndEffectSkillFunction::DoAction(pkUnit, pkSkill, pArg, pkUnitArray, pkResult);
}

///////////////////////////////////////////////////////////
//  PgDefaultOnly_Add_EffectSkillFunction
///////////////////////////////////////////////////////////
int PgDefaultOnly_Add_EffectSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if(!pkSkill)
	{
		return 0;
	}

	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}


	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		// 참고용 유닛은 리스트에서 지나친다.
		if(true == (*unit_itor).bReference
			|| NULL == (*unit_itor).pkUnit
			)
		{
			++unit_itor;
			continue;
		}

		CUnit* pkTarget = (*unit_itor).pkUnit;
		for(int i=0; i<EFFECTNUM_MAX; ++i)
		{
			int const iAddEffect = pkSkill->GetAbil(AT_EFFECTNUM1+i);
			if(0 < iAddEffect)
			{
				pkTarget->AddEffect(iAddEffect, 0, pArg, pkUnit);
			}
			else
			{
				break;
			}
		}
		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgRePositionFunction
///////////////////////////////////////////////////////////
Direction const PgRePositionFunction::ms_eDir[8] = {DIR_LEFT,DIR_RIGHT,DIR_UP,DIR_DOWN,DIR_LEFT_UP,DIR_RIGHT_UP,DIR_LEFT_DOWN,DIR_RIGHT_DOWN};

int PgRePositionFunction::SkillFire( CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if( !pkGround )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Ground SkillNo = ") << iSkillNo);
		return -1;
	}

	if( (pkGround->GetAttr() & GATTR_MYHOME) == GATTR_MYHOME )// 마이홈에서는 사용할수 없다
	{
		return 0;
	}
	// 포지션을 다시 리셋 해준다.
	NxVec3 nx3Pos( pkUnit->GetPos().x, pkUnit->GetPos().y, pkUnit->GetPos().z + 1.0f );
	NxScene* pkScene = pkGround->PhysXScene()->GetPhysXScene();
	NxVec3 nxZRay(0, 0, -1.0f);
	NxVec3 ptRayDir;

	for ( int i = 0; i < 8; ++i )
	{
		if ( true == ::GetAxisVector3( pkGround->GetPathRoot(), pkUnit->GetPos(), ms_eDir[i], ptRayDir ) )
		{
			NxRaycastHit kHit;
			NxRay kRay( nx3Pos + ( ptRayDir * 3.0f ), nxZRay );//조금 이동시켜
			NxShape *pkHitShape = pkScene->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, 0xFFFFFFFF, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT );
			if ( pkHitShape )
			{
				// 옆에 걸리는 벽이 있는지 레이를 쏴보아야 한다.(조금 뒤로 해서)
				NxRay kRay2( kRay.orig + ( ptRayDir * -5.0f ), ptRayDir );
				pkHitShape = pkScene->raycastClosestShape( kRay2, NX_STATIC_SHAPES, kHit, 0xFFFFFFFF, 15.0f );
				if ( !pkHitShape )
				{
					POINT3 pt3Pos( kRay.orig.x, kRay.orig.y, kRay.orig.z+20.0f );// 조금 위로 올리자.
					pkGround->SendToPosLoc( pkUnit, pt3Pos, MMET_Normal );
					return 1;
				}
			}
		}
	}

	{
		POINT3 pt3Pos;
		HRESULT hRet = E_FAIL;
		switch( pkGround->GetAttr() )
		{
		case GATTR_BATTLESQUARE:
			{
				int const iTeam = pkUnit->GetAbil(AT_TEAM);
				hRet = pkGround->FindSpawnLoc( 0, pt3Pos, true, iTeam );
			}break;
		default:
			{
				hRet = pkGround->FindSpawnLoc( 1, pt3Pos, true, E_SPAWN_DEFAULT );
			}break;
		}
		if( S_OK == hRet )
		{
			pkGround->SendToPosLoc( pkUnit, pt3Pos, MMET_Normal );
		}
	}
	return 1;
}


void PgCoupleVicinityEffectFunction::SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pkArg)
{// 패시브 스킬임
	if(!pkUnit
		|| !pkSkill
		|| !pkArg
		|| UT_PLAYER != pkUnit->UnitType()
		)
	{
		return;
	}

	int const iEffectNo = pkSkill->GetEffectNo();
	if(0 >= iEffectNo)
	{
		return;
	}
	PgPlayer * pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
	if(!pkPlayer)
	{
		return;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if(!pkGround)
	{
		return;
	}

	BM::GUID kCoupleGuid = pkPlayer->GetCoupleGuid();
	if(!BM::GUID::IsNotNull(kCoupleGuid))
	{
		return;
	}

	CUnit *pkCoupleUnit = pkGround->GetUnit(kCoupleGuid);
	if(!pkCoupleUnit)
	{
		return;
	}
	if( kCoupleGuid != pkCoupleUnit->GetID() )
	{
		return;
	}
	POINT3 const &rkCurPos = pkPlayer->GetPos();
	POINT3 const &rkCouplePos = pkCoupleUnit->GetPos();
	float const fDistance = POINT3::Distance(rkCurPos, rkCouplePos);
	if( COUPLE_PASSIVE_HPMP_INCREASE_EFFECT_ABLE_DIST >= fDistance )
	{// 이펙트 거는 거리 이내 이면 이펙트를 건다
		PgDefaultSkillFunction::SkillBegin(pkUnit, pkSkill, pkArg);
	}
}