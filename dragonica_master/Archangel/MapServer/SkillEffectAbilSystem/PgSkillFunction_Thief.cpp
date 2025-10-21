#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Thief.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Item.h"
#include "Variant/PgPartyMgr.h"
#include "Global.h"
#include "PgAction.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

///////////////////////////////////////////////////////////
//  PgBeautifulGirlSkillFunction - 스트립 웨폰
///////////////////////////////////////////////////////////
int PgStripWeaponSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);
	
	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			if(pkTarget->IsUnitType(UT_PLAYER))
			{
				if(iRandValue < iEffectPercent)
				{
					PgInventory* pkInventory = pkTarget->GetInven();
					if (pkInventory == NULL)
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
						return -1;
					}
					SItemPos kPos(IT_FIT, (BYTE)EQUIP_POS_WEAPON);
					PgBase_Item kItem;
					if (S_OK != pkInventory->GetItem(kPos, kItem))
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
						return -1;
					}
					SItemPos kTargetPos(0, 0);
					if (! pkInventory->GetFirstEmptyPos(IT_EQUIP, kTargetPos))
					{
						INFO_LOG(BM::LOG_LV8, __FL__<<L"Cannot UnEquipItem because no empty slot in IT_EQUIP inventory Unit["<<pkTarget->GetID()<<L"]");
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
						return -1;
					}
					PgGround* pkGround = NULL;
					if(pArg)
					{
						pArg->Get(ACTARG_GROUND, pkGround);
					}

					if( !pkGround )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__<<L"kGround is NULL");
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
						return -1;
					}

					PgAction_MoveItem kAction(kPos, kTargetPos, pkGround, 0 );
					kAction.DoAction( pkTarget, NULL );
					return 1;
				}
			}
			else
			{
				if(iRandValue < iEffectPercent * 2)
				{
					int const iEffectNo = pkSkill->GetEffectNo();
					if(iEffectNo)
					{
						pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
					}
				}
				else
				{
					pkAResult->SetMissed(true);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgThiefDefaultHitSkillFunction - 도둑 기본 타 (2/4타 제외)
///////////////////////////////////////////////////////////
int PgThiefDefaultHitSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	
	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed() && (false == (*unit_itor).bReference))
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));			
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgThiefDefault24HitSkillFunction - 도둑 기본 2/4타 공격
///////////////////////////////////////////////////////////
int PgThiefDefault24HitSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	
	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResult103201201(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed() && (false == (*unit_itor).bReference))
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));			
		}

		++unit_itor;
	}

	return 1;
}


///////////////////////////////////////////////////////////
//  PgSleepSmellSkillFunction
///////////////////////////////////////////////////////////
int PgSleepSmellSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);

		int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
		int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
		if (iDecHitRate < iRandValue)
		{
			// 타격 실패 (Missed)
			pkAResult->SetMissed(true);
			++unit_itor;
			continue;
		}

		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
			if(iEffectNo)
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgShoutSkillFunction
///////////////////////////////////////////////////////////
int PgShoutSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);

			int iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE) % ABILITY_RATE_VALUE;
			int iDecHitRate = CalcDecHitRate(pkUnit, pkTarget, iHitRate);
			if (iDecHitRate < iRandValue)
			{
				// 타격 실패 (Missed)
				pkAResult->SetMissed(true);
				++unit_itor;
				continue;
			}

			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);
				if(iRandValue <= iEffectPercent)
				{
					int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
					if(iEffectNo)
					{
						pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
					}
				}
				else
				{
					pkAResult->SetMissed(true);
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgDexteritySkillFunction
///////////////////////////////////////////////////////////
int PgDexteritySkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}

	if(!pkUnitArray->empty())
	{
		//시전자에게 이펙트가 걸려야 한다.
		int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
		if(iEffectNo)
		{
			pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
		}
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{

			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
			{
				int const iEffectNo = pkSkill->GetEffectNo();
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

				if(iEffectNo)
				{
					if(iRandValue <= iEffectPercent)
					{
						pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
					}
				}

				CUnit* pkUnit2 = pkUnit;
				//Entity일 경우 부모의 Unit을 찾아서 Unit2에 세팅해야 한다.
				BM::GUID const& kParentGUID = pkUnit->Caller();
				if(BM::GUID::IsNotNull(kParentGUID) && pkUnit->GetID() != kParentGUID)
				{
					PgGround* pkGround = NULL;
					if(pArg)
					{
						pArg->Get(ACTARG_GROUND, pkGround);
					}

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
		}//if(pkTarget)
		++unit_itor;
	}//while

	return 1;
}

///////////////////////////////////////////////////////////
//  PgPhantomCloneSkillFunction
///////////////////////////////////////////////////////////
int PgPhantomCloneSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	if(pkUnitArray->empty())
	{
		return 1;
	}

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);
	if(0 >= iDistance)
	{
		iDistance = 70;
	}

	int nCreateSize = pkUnitArray->size();
	if(3 > nCreateSize)
	{
		nCreateSize = 3;
	}

	float fAngle = NI_PI * 2 / nCreateSize;

	for(int  i = 0; i < nCreateSize; ++i)
	{
		NiMatrix3 kRot;
		kRot.MakeZRotation(fAngle * i);
		NiPoint3 kPos(static_cast<float>(iDistance), 0.0f, 0.0f);

		kPos = kRot * kPos;

		POINT3 ptPos(kPos.x,kPos.y,kPos.z);

		Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

		// 엔티티를 만들어야 한다
		SCreateEntity kCreateInfo;
		kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
		kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
		kCreateInfo.bUniqueClass = false;
		kCreateInfo.kGuid.Generate();
		kCreateInfo.ptPos = pkUnit->GetPos() + ptPos;
		::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos() + ptPos, eFrontDir, 0, kCreateInfo.ptPos );

		// 바닥에 밀착 시킨다
		NxRaycastHit kHit;
		NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
		NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
		if(pkHitShape)
		{
			kCreateInfo.ptPos.z = kHit.worldImpact.z;
		}
		else
		{
			kCreateInfo.ptPos = pkUnit->GetPos();
		}

		pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Phantom") );
	}
	return 1;
}

///////////////////////////////////////////////////////////
//  PgShadowBladeFunction
///////////////////////////////////////////////////////////
int PgShadowBladeFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << iSkillNo);
		return -1;
	}

	int iDistance = pkSkill->GetAbil(AT_DISTANCE);
	if ( 0 == iDistance )
	{
		iDistance = 20;
	}

	float fDistance = static_cast<float>(iDistance);

	Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

	// 엔티티를 만들어야 한다
	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = false;
	kCreateInfo.kGuid.Generate();
	kCreateInfo.ptPos = pkUnit->GetPos();
	::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), eFrontDir, fDistance, kCreateInfo.ptPos );

	// 바닥에 밀착 시킨다
	NxRaycastHit kHit;
	NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
	NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		kCreateInfo.ptPos.z = kHit.worldImpact.z;
	}

	pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("ShadowBlade") );

	return 1;
}
