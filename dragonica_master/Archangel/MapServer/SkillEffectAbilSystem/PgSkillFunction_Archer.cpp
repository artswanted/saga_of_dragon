#include "stdafx.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/PgPartyMgr.h"
#include "Variant/PgClassDefMgr.h"
#include "Global.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Archer.h"
#include "PgEffectFunction.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"


///////////////////////////////////////////////////////////
//  PgBeautifulGirlSkillFunction - 멋진 언니
///////////////////////////////////////////////////////////
int PgBeautifulGirlSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, SkillNo="<<iSkillNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	if(!pkSkill)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find CSkillDef, SkillNo="<<iSkillNo);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return -1"));
		return -1;
	}

	int const iDistance = pkSkill->GetAbil(AT_DISTANCE);

	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = true;
	kCreateInfo.kGuid.Generate();

	POINT3 ptEntityPos = pkUnit->GetPos();
	::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), pkUnit->FrontDirection(), static_cast<float>(iDistance), ptEntityPos);
	// 바닥에 밀착 시킨다
	NxRay kRay(NxVec3(ptEntityPos.x, ptEntityPos.y, ptEntityPos.z+20), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		ptEntityPos.z = kHit.worldImpact.z;
	}
	kCreateInfo.ptPos = ptEntityPos;
	
	CUnit* pkGirl = pkGround->CreateEntity(pkUnit, &kCreateInfo, _T("BeautifulGirl"));
	if(pkGirl)
	{
		pkGirl->AddEffect(pkSkill->GetEffectNo(), 0, pArg, pkUnit);
	}
	
	return 1;
}

bool PgBeautifulGirlSkillFunction::SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray)
{
	return true;
}

///////////////////////////////////////////////////////////
//  PgBleedingSkillFunction
///////////////////////////////////////////////////////////
int PgBleedingSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.	
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid())
		{
			// Damage가 있고 공격 받을 수 있는 상태 일 때
			if(pkAResult->GetValue() && (0 == pkTarget->GetAbil(AT_CANNOT_DAMAGE)) && (false == (*unit_itor).bReference))
			{
				int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

				if(iRandValue < iEffectPercent)
				{
					int const iEffectNo = pkSkill->GetEffectNo();
					if(iEffectNo)
					{
						pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
					}
				}
			}

			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));

		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgMPZeroTrapSkillFunction
///////////////////////////////////////////////////////////
int PgMPZeroTrapSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResult1100028011(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid())
		{
			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, iSkillNo, pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgFlashBangSkillFunction
///////////////////////////////////////////////////////////
int PgFlashBangSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 100); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResult105300501(iSkillNo, pkUnit, *pkUnitArray, pkResult);
	
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && !pkAResult->GetMissed() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}
		}
		++unit_itor;
	}

	return 1;
}


///////////////////////////////////////////////////////////
//  PgAtropineSkillFunction
///////////////////////////////////////////////////////////
int PgAtropineSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	if(pkUnitArray->empty())
	{
		return 0;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);

	int const iEffectNo = pkSkill->GetEffectNo();
	
	//지울수 있는 Effect 개수
	int const iDeleteCount = pkSkill->GetAbil(AT_COUNT);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			{// 독은 모두 해제 하고
				pkTarget->DeleteEffect(iEffectNo);
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}

			{// 디버프를
				PgUnitEffectMgr const& rkEffectMgr = pkTarget->GetEffectMgr();
				int iCount = 0;
				ContEffectItor kItor;
				rkEffectMgr.GetFirstEffect(kItor);
				CEffect* pkEffect = NULL;
				while ((pkEffect = rkEffectMgr.GetNextEffect(kItor)) != NULL)
				{
					if(iDeleteCount <= iCount)
					{//지울 수 있는 최대한 만큼 지우는데 
						break;
					}
					if(pkEffect)
					{
						int const iBuffType = pkEffect->GetAbil(AT_TYPE);
						if(EFFECT_TYPE_CURSED == iBuffType)
						{// 디버프 중에
							if(0 == pkEffect->GetAbil(AT_CURE_NOT_DELETE))
							{// 큐어로 지울수 있는 것들만 지운다
								pkTarget->DeleteEffect(pkEffect->GetEffectNo());
								++iCount;
							}
						}
					}
				}
			}
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgMultiShotSkillFunction
///////////////////////////////////////////////////////////
int PgMultiShotSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	if (!pkSkill) { return -1; }
	
	int const iEffectNo = pkSkill->GetEffectNo();
	if(iEffectNo)
	{
		pkUnit->AddEffect(iEffectNo, 0, pArg, pkUnit);
	}

	if (!pkUnitArray || pkUnitArray->empty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);
	if(0 == iEffectPercent)
	{
		iEffectPercent = ABILITY_RATE_VALUE;
	}
	
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	SkillFuncUtil::OnAddSkillLinkagePoint(pkUnit, pkSkill, pkGround);					// 연계 포인트를 누적할 수 있다면
	SkillFuncUtil::UseSkillLinkagePoint(pkUnit, pkSkill, pkGround);	// 연계 포인트를 사용하는 스킬이라면

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 0); //Target이 유효한 타겟인지 검사한다.
	bool bReturn = CS_GetSkillResultDefault(iSkillNo, pkUnit, *pkUnitArray, pkResult);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), false);
		if(pkAResult && !pkAResult->GetInvalid() && pkAResult->GetValue() && !pkAResult->GetMissed())
		{
			int iEffectNo = pkSkill->GetAbil(AT_EFFECTNUM1);

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
									// 더 강력한 버프를 거는 경우
									int const iChildEffectNo = pkLearnedSkillDef->GetAbil(AT_EFFECTNUM1);
									if(0 < iChildEffectNo)
									{
										iEffectNo = iChildEffectNo;
									}
									int const iChildPercent = pkLearnedSkillDef->GetAbil(AT_PERCENTAGE);
									if(0 < iChildPercent)
									{
										iEffectPercent = iChildPercent;
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

			int const iRandValue = BM::Rand_Index(ABILITY_RATE_VALUE);

			if(iRandValue <= iEffectPercent)
			{
				if(iEffectNo)
				{
					pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
				}
			}

			::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkAResult, pkSkill->No(), pArg, ::GetTimeStampFromActArg(*pArg, __FUNCTIONW__));
		}

		++unit_itor;
	}

	return 1;
}

///////////////////////////////////////////////////////////
//  PgAutoShootSystemFunction
///////////////////////////////////////////////////////////
int	PgAutoShootSystemFunction::SkillFire( CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult )
{
	int const iRet = PgDefaultOnlyAddeffectSkillFunction::SkillFire( pkUnit, iSkillNo, pArg, pkUnitArray, pkResult );
	if ( 1 == iRet )
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
			iDistance = 50;
		}

		Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

		// 엔티티를 만들어야 한다
		SCreateEntity kCreateInfo;
		kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
		kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
		kCreateInfo.bUniqueClass = true;
		kCreateInfo.kGuid.Generate();
		kCreateInfo.ptPos = pkUnit->GetPos();

		::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), eFrontDir, static_cast<float>(iDistance), kCreateInfo.ptPos );

		// 바닥에 밀착 시킨다
		NxRaycastHit kHit;
		NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
		NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
		if(pkHitShape)
		{
			kCreateInfo.ptPos.z = kHit.worldImpact.z;
		}

		pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("AutoShootSystem") );
		return iRet;
	}
	return iRet;
}


///////////////////////////////////////////////////////////
//  PgCarpetBombingFunction
///////////////////////////////////////////////////////////
float const PgCarpetBombingFunction::ms_fDistance = -196.0f;
float const PgCarpetBombingFunction::ms_fDistance2 = 70.0f;

int PgCarpetBombingFunction::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	size_t iCreateEntitySize = 1;
	int const iChildBaseSkillNo = pkSkill->GetAbil(AT_CHILD_SKILL_NUM_01);
	if ( iChildBaseSkillNo )
	{
		PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
		if ( pkPlayer )
		{
			PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
			if ( pkPlayerSkill )
			{
				int const iUpgradeSkill = pkPlayerSkill->GetLearnedSkill(iChildBaseSkillNo);
				if( iUpgradeSkill )
				{
					iCreateEntitySize = 2;
				}
			}
		}
	}

	Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());

	// 엔티티를 만들어야 한다
	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = false;
	kCreateInfo.kGuid.Generate();
	kCreateInfo.ptPos = pkUnit->GetPos();

	::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), eFrontDir, ms_fDistance, kCreateInfo.ptPos );

	NxRaycastHit kHit;
	if ( 2 == iCreateEntitySize  )
	{
		SCreateEntity kCreateInfo2 = kCreateInfo;
		kCreateInfo2.kGuid.Generate();

		::GetDistanceToPosition( pkGround->GetPathRoot(), kCreateInfo.ptPos, ::GetCrossDirection( eFrontDir, false ), ms_fDistance2, kCreateInfo2.ptPos );
		::GetDistanceToPosition( pkGround->GetPathRoot(), kCreateInfo.ptPos, ::GetCrossDirection( eFrontDir, true ), ms_fDistance2, kCreateInfo.ptPos );

		// 바닥에 밀착 시킨다
		NxRay kRay( NxVec3(kCreateInfo2.ptPos.x, kCreateInfo2.ptPos.y, kCreateInfo2.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
		NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
		if(pkHitShape)
		{
			kCreateInfo2.ptPos.z = kHit.worldImpact.z;
		}

		pkGround->CreateEntity( pkUnit, &kCreateInfo2, _T("CarpetBombing2") );
	}

	// 바닥에 밀착 시킨다
	NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
	NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
	if(pkHitShape)
	{
		kCreateInfo.ptPos.z = kHit.worldImpact.z;
	}

	pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("CarpetBombing") );
	return 1;
}

///////////////////////////////////////////////////////////
//  PgValcan300Function
///////////////////////////////////////////////////////////
int PgValcan300Function::SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
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

	if(0 < pkSkill->GetAbil(AT_TARTGET_RE_POS))
	{// 위치 조정이 존재한다면
		POINT3 kPos = pkUnit->GetPos();
		kPos.z+=20.0f; // 조금 위로 올리고
		UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
		while(pkUnitArray->end() != unit_itor)
		{
			CUnit* pkTarget = (*unit_itor).pkUnit;
			if(pkTarget->GetID() != pkUnit->GetID())
			{// 자기 자신은 제외
				pkGround->SendToPosLoc( pkTarget, kPos , MMET_Normal );
			}
			++unit_itor;
		}
	}

	// 이 스킬이 소환하는 것을 소환하고
	int iResult = MakeSummonUnit(pkUnit, pkSkill, pArg, pkUnitArray, pkResult);
	
	int const iAnotherSummonSkillNo = pkSkill->GetAbil(AT_ADDITIONAL_SUMMON_SKILL_NO);	// iAnotherSummonSkillNo은 ParentSkillNo(1lv스킬번호)이어야함
	if(0 < iAnotherSummonSkillNo)
	{// 추가 적으로 소환할게 있다면
		int iResultSummonSkillNo = 0;
		int const iMustLearnSkillNo = pkSkill->GetAbil(AT_ADDITIONAL_SUMMON_MUST_LEARN_SKILL_NO);
		if(0 < iMustLearnSkillNo
			&&  pkUnit->IsUnitType(UT_PLAYER)
			)
		{// 추가적인 소환을 위해 배워야 하는 스킬이 있었다면
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			int const iLearnSkillNo = pkPlayer->GetMySkill()->GetLearnedSkill(iMustLearnSkillNo);
			if( 0 < iLearnSkillNo)
			{// 배웠는지 확인해서
				CSkillDef const* pkMustLearnSkill = kSkillDefMgr.GetDef(iLearnSkillNo);
				BYTE const byLevel = pkMustLearnSkill->GetLevel(); // 해당 레벨에 대응 되는 스킬번호를
				iResultSummonSkillNo = iAnotherSummonSkillNo+byLevel-1; // 만들고
			}
		}
		else
		{
			iResultSummonSkillNo = iAnotherSummonSkillNo;
		}

		// we have a case when sub skill isn't learned, but we try to get skill with 0 skill No
		if (iResultSummonSkillNo == 0)
			return iResult;

		// 소환 한다
		CSkillDef const* pkAnotherSummonSkill = kSkillDefMgr.GetDef( iResultSummonSkillNo );
		iResult = MakeSummonUnit(pkUnit, pkAnotherSummonSkill, pArg, pkUnitArray, pkResult);
	}
	
	return iResult;
}

int PgValcan300Function::MakeSummonUnit( CUnit *pkUnit, CSkillDef const* pkDefSkill, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround* pkGround = NULL;
	if(pArg)
	{
		pArg->Get(ACTARG_GROUND, pkGround);
	}

	if( !pkGround )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Cannot find Ground SkillNo = ") << pkDefSkill->No());
		return -1;
	}

	if ( !pkDefSkill )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("CSkillDef is NULL SkillNo = ") << pkDefSkill->No());
		return -1;
	}

	int iDistance = pkDefSkill->GetAbil(AT_DISTANCE);
	if ( 0 == iDistance )
	{
		iDistance = 50;
	}
	
	int iDistance2 = pkDefSkill->GetAbil(AT_DISTANCE2);

	Direction const eFrontDir = ((DIR_NONE==pkUnit->FrontDirection()) ? DIR_RIGHT : pkUnit->FrontDirection());
	Direction const eRightDir = GetCrossDirection(eFrontDir, true);

	// 엔티티를 만들어야 한다(GUID는 아래에서 생성함)
	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = pkDefSkill->GetAbil(AT_CLASS);
	kCreateInfo.kClassKey.nLv = pkDefSkill->GetAbil(AT_LEVEL);
	kCreateInfo.bUniqueClass = false;
	kCreateInfo.ptPos = pkUnit->GetPos();
	
	{
		::GetDistanceToPosition( pkGround->GetPathRoot(), pkUnit->GetPos(), eFrontDir, static_cast<float>(iDistance), kCreateInfo.ptPos );
		if (0 != iDistance2 )
		{
			POINT3 kPos = kCreateInfo.ptPos;
			::GetDistanceToPosition( pkGround->GetPathRoot(), kPos, eRightDir, static_cast<float>(iDistance2), kCreateInfo.ptPos );
		}
	}
	

	if(pkDefSkill->GetAbil(AT_SKILL_SUMMON_NEAR_BY_ENEMY)) //적 위치에 소환할 것인가
	{
		UNIT_PTR_ARRAY kTargetArray;
		pkGround->GetUnitTargetList(pkUnit, kTargetArray, ESTARGET_ENEMY, iDistance);
		if(!kTargetArray.empty())
		{
			int const iMaxTarget = 1;
			UNIT_PTR_ARRAY::iterator iterTarget = kTargetArray.begin();
			if(iterTarget != kTargetArray.end())
			{
				CUnit* pkTargetUnit = (*iterTarget).pkUnit;
				kCreateInfo.ptPos = pkTargetUnit->GetPos();
			}

		}
	}

	{// 바닥에 밀착 시킨다
		NxRaycastHit kHit;
		NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
		NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
		if(pkHitShape)
		{
			kCreateInfo.ptPos.z = kHit.worldImpact.z;
		}
	}

	//CUnit* pkCaller = pkUnit;

	////타입이 펫이라면
	//if(pkUnit->IsUnitType(UT_PET))
	//{		
	//	if(pkGround)
	//	{
	//		CUnit* pkCallerUnit = pkGround->GetUnit(pkUnit->Caller());

	//		//실제 user를 얻어온다.
	//		if(pkCallerUnit)
	//		{
	//			//실제 유저를 Entity의 Caller로 설정해준다.
	//			pkCaller = pkCallerUnit;
	//		}
	//	}
	//}

	VEC_UNIT kContCreatedEntities;
	{/// 1개 혹은 다수개의 Entity를 생성하는 부분
		CUnit* pkEntity = NULL;
		int iSkillActorCnt = pkDefSkill->GetAbil(AT_CREATE_SKILLACTOR_CNT);
		if(1 < iSkillActorCnt)
		{// 소환 하는 액터가 2개 이상이라면
			POINT3 kRightVec(0,0,0);
			{// 플레이어의 오른쪽 방향을 구하고
				POINT3 kTempFrontVec = pkUnit->GetDirectionVector(eFrontDir);;
				POINT3 kUpVec(0.0f,0.0f,1.0f);
				kRightVec = kUpVec.Cross(kTempFrontVec);
			}

			if(0 != iSkillActorCnt%2)
			{// 홀수 이면, 플레이어 앞에 하나 생성하고, 
				kCreateInfo.kGuid.Generate();		// GUID 생성
				pkEntity = pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Entity") );
				kContCreatedEntities.push_back(pkEntity);
				--iSkillActorCnt;
			}		
			// 위 아래로 이격 시킬 거리를 가져와
			int iDist = pkDefSkill->GetAbil(AT_VERTICAL_DISTANCE);
			if(0 == iDist)
			{
				iDist = 50;
			}
			POINT3 const kBasePos = kCreateInfo.ptPos;
			int iNextDist = iDist;
			for(int i=0; i< iSkillActorCnt; i+=2)
			{// Tip : 소환 유닛은 기본적으로 최대 8개로 강제 세팅 되어있다.( CUnit::GetMaxSummonUnitCount() )
				int const&  iCurDist = iNextDist;
				{// 플레이어 오른쪽에 생성하고
					kCreateInfo.kGuid.Generate();	// GUID 생성
					kCreateInfo.ptPos = kBasePos + (kRightVec*static_cast<float>(iCurDist));
					pkEntity = pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Entity") );
					kContCreatedEntities.push_back(pkEntity);
					{// 바닥에 밀착 시킨다
						NxRaycastHit kHit;
						NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
						NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
						if(pkHitShape)
						{
							kCreateInfo.ptPos.z = kHit.worldImpact.z;
						}
					}
				}
				{// 플레이어 왼쪽에 생성 하고
					kCreateInfo.kGuid.Generate();	// GUID 생성
					kCreateInfo.ptPos = kBasePos - (kRightVec*iCurDist);
					pkEntity = pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Entity") );
					kContCreatedEntities.push_back(pkEntity);
					{// 바닥에 밀착 시킨다
						NxRaycastHit kHit;
						NxRay kRay( NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+20.0f ), NxVec3(0, 0, -1.0f) );
						NxShape *pkHitShape = pkGround->PhysXScene()->GetPhysXScene()->raycastClosestShape(kRay, NX_STATIC_SHAPES, kHit, -1, 200.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_IMPACT);
						if(pkHitShape)
						{
							kCreateInfo.ptPos.z = kHit.worldImpact.z;
						}
					}
				}
				iNextDist += iDist; // 이격 거리 갱신
			}
		}
		else
		{// 일반 스킬
			kCreateInfo.kGuid.Generate();			// GUID 생성
			pkEntity = pkGround->CreateEntity( pkUnit, &kCreateInfo, _T("Entity") );
			kContCreatedEntities.push_back(pkEntity);
		}
	}

	{/// 생성된 Entity들에 대한 후 처리를 진행하는 부분
		int const iLearnCheckSkillNo = pkDefSkill->GetAbil(AT_SKILL_CUSTOM_CHECK_LEARN_THIS_SKILL_NO);
		int iSkillCustomValue = 0;
		if(0 < iLearnCheckSkillNo 
			&& pkUnit->IsUnitType(UT_PLAYER)
			)
		{// 플레이어가
			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if(pkPlayer)
			{
				PgMySkill* pkPlayerSkill = pkPlayer->GetMySkill();
				if(pkPlayerSkill)
				{// 소환체(현재는 트랩만) 감추기 스킬을 배웠다면
					if(0 < pkPlayerSkill->GetLearnedSkill(iLearnCheckSkillNo))
					{// 스킬에 저장된 AT_SKILL_CUSTOM_VALUE값을, 생성된 Entity에게 전달하기 위해 저장해두고
						iSkillCustomValue = pkDefSkill->GetAbil(AT_SKILL_CUSTOM_VALUE);
					}
				}
			}
		}

		VEC_UNIT::iterator kUnit_itor = kContCreatedEntities.begin();
		while(kContCreatedEntities.end() != kUnit_itor)
		{// 생성된 Entity들에 대한 처리를 한꺼번에 진행한다
			CUnit* pkCreatedEntity = (*kUnit_itor);
			if(pkCreatedEntity)
			{
				if(pkUnit->IsUnitType(UT_PET))
				{
					pkCreatedEntity->SetAbil(AT_CALLER_IS_PET, 1);
					//pkCreatedEntity->Caller(pkCaller->GetID());	// 이렇게 의도하던 코드가 있었으나 사용하지 않는상태라 코드 만들어놓고 일단 주석처리.
				}
				if(0 < iSkillCustomValue)
				{// AT_SKILL_CUSTOM_VALUE값이 존재 한다면 Unit에 세팅하고 BroadCast All 한다.
					OnSetAbil(pkCreatedEntity, AT_SKILL_CUSTOM_VALUE_STORAGE, iSkillCustomValue);
				}
			}
			++kUnit_itor;
		}
	}

	return 1;
}