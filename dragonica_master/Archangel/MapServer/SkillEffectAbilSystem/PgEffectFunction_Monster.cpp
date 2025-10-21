#include "stdafx.h"
#include "PgEffectAbilTable.h"
#include "PgEffectFunction.h"
#include "Variant/Global.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"
#include "PgEffectFunction_Monster.h"
#include "SkillEffectAbilSystem/PgSkillAbilHandleManager.h"

///////////////////////////////////////////////////////////
//  PgVenomFunction
///////////////////////////////////////////////////////////
int PgVenomFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	int const iHP = pkUnit->GetAbil(AT_HP);
	int const iDec = GetTick_AT_HP_Value(pkEffect, pkEffect->GetActArg(), pkUnit);
	int const iNew = iHP + iDec;
	int const iDelta = iNew - iHP;

	BM::Stream kPacket(PT_M_C_NFY_ABILCHANGED);
	kPacket.Push(pkUnit->GetID());
	kPacket.Push((short)AT_HP);
	kPacket.Push(iNew);
	kPacket.Push(pkEffect->GetCaster());
	kPacket.Push(pkEffect->GetEffectNo());
	kPacket.Push(iDelta);
	pkUnit->Send(kPacket, E_SENDTYPE_BROADALL);
	if (0 >= iNew)
	{
		pkUnit->SetTarget(pkEffect->GetCaster());
		pkUnit->SetAbil(AT_HP, 0, false, false);
		return ECT_MUSTDELETE;
	}

	pkUnit->SetAbil(AT_HP, iNew, false, false);
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgIncreaseSightFunction
///////////////////////////////////////////////////////////
void PgIncreaseSightFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();
	//시야 증가
	int iAdd = pkEffect->GetAbil(AT_DETECT_RANGE);
	rkMgr.AddAbil(AT_DETECT_RANGE, iAdd);
	pkUnit->NftChangedAbil(AT_DETECT_RANGE, E_SENDTYPE_NONE);

	iAdd = pkEffect->GetAbil(AT_CHASE_RANGE);
	rkMgr.AddAbil(AT_CHASE_RANGE, iAdd);
	pkUnit->NftChangedAbil(AT_CHASE_RANGE, E_SENDTYPE_NONE);

}

void PgIncreaseSightFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();
	//시야 감소
	int iAdd = pkEffect->GetAbil(AT_DETECT_RANGE);
	rkMgr.AddAbil(AT_DETECT_RANGE, -iAdd);
	pkUnit->NftChangedAbil(AT_DETECT_RANGE, E_SENDTYPE_NONE);

	iAdd = pkEffect->GetAbil(AT_CHASE_RANGE);
	rkMgr.AddAbil(AT_CHASE_RANGE, -iAdd);
	pkUnit->NftChangedAbil(AT_CHASE_RANGE, E_SENDTYPE_NONE);
}
int PgIncreaseSightFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgBlockingFunction
///////////////////////////////////////////////////////////
void PgBlockingFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_DODGE_SUCCESS_VALUE);
	rkMgr.AddAbil(AT_DODGE_SUCCESS_VALUE, iAdd);
	pkUnit->NftChangedAbil(AT_DODGE_SUCCESS_VALUE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}

void PgBlockingFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_DODGE_SUCCESS_VALUE);
	rkMgr.AddAbil(AT_DODGE_SUCCESS_VALUE, -iAdd);
	pkUnit->NftChangedAbil(AT_DODGE_SUCCESS_VALUE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}
int PgBlockingFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgNetBinderFunction
///////////////////////////////////////////////////////////
void PgNetBinderFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_R_MOVESPEED);
	rkMgr.AddAbil(AT_R_MOVESPEED, iAdd);
	rkMgr.SetAbil(AT_NOT_MOVE, 1);
	pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	pkUnit->NftChangedAbil(AT_NOT_MOVE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}

void PgNetBinderFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgUnitEffectMgr& rkMgr = pkUnit->GetEffectMgr();

	int iAdd = pkEffect->GetAbil(AT_R_MOVESPEED);
	rkMgr.AddAbil(AT_R_MOVESPEED, -iAdd);
	rkMgr.SetAbil(AT_NOT_MOVE, 0);
	pkUnit->NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
	pkUnit->NftChangedAbil(AT_NOT_MOVE, E_SENDTYPE_BROADALL|E_SENDTYPE_EFFECTABIL);
}
int PgNetBinderFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgLavalonBreathFunction
///////////////////////////////////////////////////////////
int PgLavalonBreathFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return -1;
	}

	int const iDetectRange = 120;
	float fMetaForStart = 0.5f;
	float fMetaForEnd = 0.5f;
	int const iEffectNo = pkEffect->GetEffectNo();

	if ( 6000902 == iEffectNo )		//왼쪽
	{
		fMetaForStart = 0.45f;
		fMetaForEnd = 0.17f;
	}
	else if ( 6000901 == iEffectNo )	//오른쪽
	{
		fMetaForStart = 0.55f;
		fMetaForEnd = 0.83f;
	}

	POINT3 kTopLeft = pkGround->GetNodePosition("pt_blaze_01");			// Top Left
	POINT3 kTopRight = pkGround->GetNodePosition("pt_blaze_02");		// Top Right
	POINT3 kBottomLeft = pkGround->GetNodePosition("pt_blaze_03");		// Bottom Left
	POINT3 kBottomRight = pkGround->GetNodePosition("pt_blaze_04");		// Bottm Right

	POINT3 kUpperBase = kTopRight-kTopLeft;
	POINT3 kRightBase = kTopRight-kBottomRight;
	POINT3 kBottomBase = kBottomRight-kBottomLeft;
	POINT3 kLeftBase = kTopLeft-kBottomLeft;

	POINT3 kStartPos = kUpperBase*fMetaForStart;
	kStartPos+=(kTopLeft);
	kStartPos.z = 0;

	POINT3 kEndPos = kBottomBase*fMetaForEnd;
	kEndPos+=(kBottomLeft);
	kEndPos.z = 0;

	UNIT_PTR_ARRAY kTargetArray;

	pkGround->GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, kTargetArray);
	PgActionResultVector kActionResult;

	bool bRet = CS_GetSkillResultDefault(iEffectNo, pkUnit, kTargetArray, &kActionResult);	//iEffectNo 위험하다 스킬넘버랑 이펙트 넘버랑 다르면 큰일
	
	UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();

	while(kTargetArray.end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if (pkTarget)
		{
			PgActionResult* pkResult = kActionResult.GetResult(pkTarget->GetID());
			if (pkResult && !pkResult->IsEmpty())
			{
				::AdjustFinalDamageAndApply(pkUnit, pkTarget, pkResult, iEffectNo, pkArg, ::GetTimeStampFromActArg(*pkArg, __FUNCTIONW__));
			}
		}
		++unit_itor;
	}

	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgAutoTickEffectFunction
///////////////////////////////////////////////////////////
void PgAutoTickEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

void PgAutoTickEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}
int PgAutoTickEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	int const iMaxTarget = pkEffect->GetAbil(AT_MAX_TARGETNUM);

	for( int i = 0; i < 10; ++i)
	{
		int iAddSkillNo = pkEffect->GetAbil(AT_MON_SKILL_01+i);
		if (0<iAddSkillNo)
		{
			POINT3 kPos = pkUnit->GetPos();
			UNIT_PTR_ARRAY kTargetArray;
			int const iRange = pkEffect->GetAbil(AT_DETECT_RANGE);
			//EUnitType eType = pkUnit->UnitType();
			pkGround->GetUnitInRange(kPos, iRange, UT_PLAYER, kTargetArray, static_cast<int>(AI_Z_LIMIT));
			UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();
			int iIndex = 0;
			while(kTargetArray.end() != unit_itor && iIndex < iMaxTarget)
			{
				CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iAddSkillNo);	
				if (pkSkill && pkUnit->IsTarget((*unit_itor).pkUnit, false, ESTARGET_ENEMY))
				{
					int iPhyPercent = 0;
					int iDamage = 0;
					if (pkSkill->IsSkillAtt(SAT_PHYSICS))
					{
						iPhyPercent = pkSkill->GetAbil(AT_PHY_DMG_PER);
						iDamage += pkUnit->GetAbil(AT_C_PHY_ATTACK)*iPhyPercent/ABILITY_RATE_VALUE;
					}

					int iMagicPercent = 0;
					if (pkSkill->IsSkillAtt(SAT_MAGIC))
					{
						iMagicPercent = pkSkill->GetAbil(AT_MAGIC_DMG_PER);
						iDamage += pkUnit->GetAbil(AT_C_MAGIC_ATTACK)*iMagicPercent/ABILITY_RATE_VALUE;
					}

					//if (iDamage)
					{
						::DoTickDamage(pkUnit, (*unit_itor).pkUnit, iDamage, pkEffect->GetEffectNo(), pkSkill->GetEffectNo(), pkArg);
					}
				}
				++iIndex;
				++unit_itor;
			}
		}

	}
	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgDependAbilTickEffectFunction
///////////////////////////////////////////////////////////
PgDependAbilTickEffectFunction::PgDependAbilTickEffectFunction()
	: PgDefaultEffectFunction(), m_sDependAbilType(AT_ADDED_GAUGE_VALUE), m_iTargetValue(0)
{
}

void PgDependAbilTickEffectFunction::Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef)
{
	PgDefaultEffectFunction::Build(pkAbilTable, pkDef);


	// 빌드 이후 몇개는 지운다
	CONT_ABIL_TYPE_VAULE_TABLE::iterator loop_iter = m_kTable.begin();
	while( m_kTable.end() != loop_iter )
	{
		CONT_ABIL_TYPE_VAULE& rkList = (*(*loop_iter).second);
		CONT_ABIL_TYPE_VAULE::iterator sub_iter = rkList.begin();
		while( rkList.end() != sub_iter )
		{
			WORD const wType = (*sub_iter).m_wType;
			if( AT_MON_REMOVE_EFFECT_FORCESKILL == wType
			||	m_sDependAbilType == wType )
			{
				sub_iter = rkList.erase(sub_iter);
			}
			else
			{
				++sub_iter;
			}
		}
		++loop_iter;
	}
	//m_kTable.erase(AT_MON_REMOVE_EFFECT_FORCESKILL); // 해당 어빌은 Plus/Minus 하지 않음
	//m_kTable.erase(m_sDependAbilType); // 이펙트 삭제시 의존되는 어빌 번호는 빼주지 않는다. (이미 0값)
}

void PgDependAbilTickEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgDefaultEffectFunction::EffectBegin(pkUnit, pkEffect, pkArg);

	pkUnit->SetAbil(m_sDependAbilType, pkEffect->GetAbil(m_sDependAbilType));
}

void PgDependAbilTickEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgDefaultEffectFunction::EffectEnd(pkUnit, pkEffect, pkArg);
}

int PgDependAbilTickEffectFunction::EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	if( m_iTargetValue >= pkUnit->GetAbil(m_sDependAbilType) ) // 의존된 어빌이 값이 다 떨어지면 자신을 지우자
	{
		return ECT_MUSTDELETE;
	}
	return ECT_DOTICK;
}

void PgMetaMorphosisEffectFunction::EffectBegin(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
	int const iClass = pkEffect->GetAbil(AT_MON_TRANSFORM_CLASS);
	if(0<iClass)
	{
		int const iCount = pkUnit->GetAbil(AT_MON_TRANSFORM_COUNT);
		for(int i = 0; i<iCount; ++i)
		{
			if(iClass == pkUnit->GetAbil(AT_MON_TRANSFORM_CLASS_01 + i) )	//기존에 있는 이펙트일 경우
			{
				INFO_LOG(BM::LOG_LV3, __FL__<<"Aleady MetaMorphosed Class! ClassNo : "<<iClass<<" EffectID : "<<pkEffect->GetEffectNo());
				return;	//절대 이런일 생기면 안됨
			}
		}
		pkUnit->SetAbil(AT_MON_TRANSFORM_CLASS_01 + iCount, iClass );//변신클래스 저장
		pkUnit->AddAbil(AT_MON_TRANSFORM_COUNT, 1);	//변신 횟수 증가
	}

	PgDefaultEffectFunction::EffectBegin(pkUnit, pkEffect, pkArg);
}

void PgMetaMorphosisEffectFunction::EffectEnd(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
	int const iClass = pkEffect->GetAbil(AT_MON_TRANSFORM_CLASS);
	if(0<iClass)
	{
		bool bFound = false;
		int const iCount = pkUnit->GetAbil(AT_MON_TRANSFORM_COUNT);
		for(int i = 0; i<iCount; ++i)	//
		{
			if(iClass == pkUnit->GetAbil(AT_MON_TRANSFORM_CLASS_01 + i) )	//몇번째 변신이 해제되었는가
			{
				pkUnit->SetAbil(AT_MON_TRANSFORM_CLASS_01 + i, 0);
				bFound = true;
			}
			else if(bFound)
			{
				pkUnit->SetAbil(AT_MON_TRANSFORM_CLASS_01 + i - 1, pkUnit->GetAbil(AT_MON_TRANSFORM_CLASS_01 + i));	//하나씩 앞으로 당겨 주자. 변신이 10번 이상이면 어떻하나?
				pkUnit->SetAbil(AT_MON_TRANSFORM_CLASS_01 + i, 0);
			}
		}

		if(bFound)	//변신이 진짜로 풀렸다
		{
			pkUnit->AddAbil(AT_MON_TRANSFORM_COUNT, -1);//변신 횟수 감소
		}
	}

	int const iForceFireSkillNo = pkEffect->GetAbil(AT_MON_REMOVE_EFFECT_FORCESKILL);
	if( 0 < iForceFireSkillNo
		&&	pkUnit->IsInUnitType(UT_MONSTER) )
	{
		EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkEffect->GetAbil(AT_FORCESKILL_FLAG));
		pkUnit->GetSkill()->ForceReserve(iForceFireSkillNo, eFlag);
		pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
	}

	PgDefaultEffectFunction::EffectEnd(pkUnit, pkEffect, pkArg);
}

///////////////////////////////////////////////////////////
//  PgHardDungeonEffectFunction
///////////////////////////////////////////////////////////
void PgHardDungeonEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgDefaultEffectFunction::EffectBegin(pkUnit, pkEffect, pkArg);

	//기본적은 능력치를 다 올려 준 후에 HP/MP를 Max치에 맞추어 준다.
	OnAddAbil(pkUnit, AT_HP, pkUnit->GetAbil(AT_C_MAX_HP));
	OnAddAbil(pkUnit, AT_MP, pkUnit->GetAbil(AT_C_MAX_MP));
}

///////////////////////////////////////////////////////////
//  PgEffectHealToTargetFunction
///////////////////////////////////////////////////////////
void PgEffectHealToTargetFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	EPlayContentsType const eType = static_cast<EPlayContentsType>( pkEffect->GetAbil(AT_PLAY_CONTENTS_TYPE) );
	switch(eType)
	{
	case EPCT_ELGA:
		{
			pkUnit->SetAbil(AT_ELGA_BONE_MON_HP, 0);
		}break;
	}
}

void PgEffectHealToTargetFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	int const iMonNo = pkEffect->GetAbil(AT_MON_SKILL_MONSTER_NO);
    EUnitType eUnitType = static_cast<EUnitType>(pkEffect->GetAbil(AT_MON_SKILL_UNIT_TYPE));
    if( 0==eUnitType )
    {
        eUnitType = UT_MONSTER;
    }

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->GetUnitByClassNo( iMonNo, eUnitType, kUnitArray );

	int iAddRate = pkEffect->GetAbil(AT_MON_ADD_HP_RATE);
	if( iAddRate == 0 )//  디폴트는 1% 로 하자.
	{
		iAddRate = 100;
	}

	EPlayContentsType const eType = static_cast<EPlayContentsType>( pkEffect->GetAbil(AT_PLAY_CONTENTS_TYPE) );

	UNIT_PTR_ARRAY::iterator unit_itor = kUnitArray.begin();
	while(kUnitArray.end() != unit_itor)
	{
		if(CUnit* pkTarget = (*unit_itor).pkUnit)
		{
			int const iMax = pkTarget->GetAbil(AT_C_MAX_HP);
			int const iCur = pkTarget->GetAbil(AT_HP);
			int const iHeal = static_cast<int>(iMax * (static_cast<float>(iAddRate) / 10000.0f));
			int iRet = __min(iMax, iCur+iHeal);
			if( iAddRate < 0 )
			{
				iRet = __max(0, iCur+iHeal);
			}
            iRet = (iRet > 0) ? iRet : 0;

			pkTarget->SetAbil(AT_HP, iRet, 0!=iRet);
			DoAction_Target(eType, pkGround, pkEffect, pkTarget, pkUnit, iCur);
		}
		++unit_itor;
	}
	DoAction_Caller(eType, pkGround, pkEffect, pkUnit);
}

void PgEffectHealToTargetFunction::DoAction_Target(EPlayContentsType const eType, PgGround* pkGround, CEffect* pkEffect, CUnit* pkTarget, CUnit* pkCaller, int const iOriginalHP)
{
	if( !pkGround || !pkEffect || !pkTarget || !pkCaller )
	{
		return;
	}

	switch(eType)
	{
	case EPCT_ELGA:
		{
			CUnit * pkOwner = pkGround->GetUnit(pkCaller->Caller());
			if(pkOwner && 0!=iOriginalHP)
			{
				//실제 확률은 DoAction_Caller에서 계산됨
				int const iValue = pkOwner->GetAbil(AT_ELGA_BONE_MON_HP);
				pkOwner->SetAbil(AT_ELGA_BONE_MON_HP, iValue+1);
			}
		}break;
	}
}

void PgEffectHealToTargetFunction::DoAction_Caller(EPlayContentsType const eType, PgGround* pkGround, CEffect* pkEffect, CUnit* pkCaller)
{
	if( !pkGround || !pkEffect || !pkCaller )
	{
		return;
	}

	switch(eType)
	{
	case EPCT_ELGA:
		{
			int const iForceSkillNo = pkEffect->GetAbil(AT_MON_SKILL_01);
			CUnit * pkOwner = pkGround->GetUnit(pkCaller->Caller());
			if( iForceSkillNo && pkOwner )
			{
				int const iMaxCount = pkOwner->GetAbil(AT_ELGA_STONE_COUNT);
				int const iValue = pkOwner->GetAbil(AT_ELGA_BONE_MON_HP);
				int const iKill = std::max<int>(0, iMaxCount-iValue);
				int iApplyRate = pkOwner->GetAbil(AT_ELGA_STONE_RATE);
				if(0==iApplyRate)
				{
					iApplyRate = 1000;//10%
				}

				int const iRate = ABILITY_RATE_VALUE_FLOAT - (iKill * iApplyRate);
				pkOwner->SetAbil(AT_ELGA_BONE_MON_HP, std::max(0,iRate));

				EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkEffect->GetAbil(AT_FORCESKILL_FLAG));
				pkOwner->GetSkill()->ForceReserve(iForceSkillNo, eFlag);
				pkOwner->GetAI()->SetEvent(pkOwner->GetID(), EAI_EVENT_FORCE_SKILL);
			}
		}break;
	}
}

int PgEffectHealToTargetFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	EPlayContentsType const eType = pkEffect ? static_cast<EPlayContentsType>(pkEffect->GetAbil(AT_PLAY_CONTENTS_TYPE)) : EPCT_NONE;
	switch(eType)
	{
	case EPCT_ELGA:
		{
			//수정이 살아있는지 체크 모두 죽었으면 이펙트는 종료
			PgGround* pkGround = NULL;
			if(pkArg)
			{
				pkArg->Get(ACTARG_GROUND, pkGround);
			}
			if (!pkGround || !pkEffect)
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
				return ECT_MUSTDELETE;
			}

			int const iMonNo = pkEffect->GetAbil(AT_MON_SKILL_MONSTER_NO);
			
			UNIT_PTR_ARRAY kUnitArray;
			pkGround->GetUnitByClassNo( iMonNo, UT_MONSTER, kUnitArray );

			if(kUnitArray.empty())
			{
				return ECT_MUSTDELETE;
			}
		}break;
	}
	return ECT_DOTICK;
}

///////////////////////////////////////////////////////////
//  PgElgaDealingTimeEffectFunction
///////////////////////////////////////////////////////////
void PgElgaDealingTimeEffectFunction::EffectBegin(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
	pkUnit->SetAbil(AT_USE_ACCUMULATE_DAMAGED, 1);
	pkUnit->SetAbil(AT_ACCUMULATE_DAMAGED_NOW, 0, true, true);

	if( int const iEffectNo = pkEffect->GetAbil(AT_DELETE_EFFECT_NO))
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
		pkUnit->AddEffectQueue(kData);		//방어버프해제, 그로기 상태에서 회복할때 다시 방어버프를 검
	}
}

void PgElgaDealingTimeEffectFunction::EffectEnd(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
	pkUnit->SetAbil(AT_USE_ACCUMULATE_DAMAGED, 0);
}

int PgElgaDealingTimeEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if(!pkUnit || !pkEffect)
	{
		return ECT_MUSTDELETE;
	}

	int const iNow = pkUnit->GetAbil(AT_ACCUMULATE_DAMAGED_NOW);
	int const iMax = pkEffect->GetAbil(AT_ACCUMULATE_DAMAGED_MAX);
	if(iNow>=iMax)
	{
		if(int const iForceFireSkillNo = pkEffect->GetAbil(AT_MON_ADD_FORCESKILL))
		{
			int const iFlag = EFSF_CLEAR_RESERVE | EFSF_NOW_FIRESKILL_CANCLE;
			pkUnit->GetSkill()->ForceReserve(iForceFireSkillNo, static_cast<EForceSetFlag>(iFlag));
			pkUnit->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
		}
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgTargetToDeleteEffectFunction
///////////////////////////////////////////////////////////
void PgTargetToDeleteEffectFunction::EffectBegin(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	int const iClass = pkEffect->GetAbil(AT_MON_SKILL_TARGET_NO);
	EUnitType const eUnitType = static_cast<EUnitType>(pkEffect->GetAbil(AT_MON_SKILL_UNIT_TYPE));
	CUnit * pkTarget = pkGround->GetUnitByClassNo(iClass, eUnitType);
	int const iEffectNo = pkEffect->GetAbil(AT_EFFECTNUM1);
	if(pkTarget && iEffectNo)
	{
		EffectQueueData kData(EQT_DELETE_EFFECT, iEffectNo);
		pkTarget->AddEffectQueue(kData);
	}
}

void PgTargetToDeleteEffectFunction::EffectEnd(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
}

int PgTargetToDeleteEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	return ECT_MUSTDELETE;
}

///////////////////////////////////////////////////////////
//  PgRangeBySkillFireEffectFunction
///////////////////////////////////////////////////////////
void PgRangeBySkillFireEffectFunction::EffectBegin(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
}

void PgRangeBySkillFireEffectFunction::EffectEnd(CUnit *pkUnit, CEffect *pkEffect, const SActArg *pkArg)
{
}

int PgRangeBySkillFireEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}
	
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	int const iSkillNo = pkEffect->GetAbil(AT_MON_SKILL_01);
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if ( !pkSkillDef )
	{
		INFO_LOG(BM::LOG_LV3, __FL__<<"Not Find SkillNo: "<<iSkillNo<<", EffectID: "<<pkEffect->GetEffectNo());
		return ECT_MUSTDELETE;
	}

	int const iClassNo = pkEffect->GetAbil(AT_CLASS);
	if(iClassNo <= 0)
	{
		INFO_LOG(BM::LOG_LV3, __FL__<<"Not Find Class: "<<iClassNo<<", EffectID: "<<pkEffect->GetEffectNo());
		return ECT_MUSTDELETE;
	}

	UNIT_PTR_ARRAY kTargetArray;
	pkGround->GetUnitByClassNo(iClassNo, UT_MONSTER, kTargetArray);

	UNIT_PTR_ARRAY::iterator unit_itor = kTargetArray.begin();
	while(kTargetArray.end() != unit_itor)
	{
		if(CUnit* pkTarget = (*unit_itor).pkUnit)
		{
			UNIT_PTR_ARRAY kSkillTargetArray;
			if (pkGround->AI_GetSkillTargetList(pkTarget, iSkillNo, kSkillTargetArray, false, pkArg))
			{
				if(g_kSkillAbilHandleMgr.SkillCanReserve(pkTarget, iSkillNo, pkArg, &kSkillTargetArray))
				{
					if(std::find(kSkillTargetArray.begin(), kSkillTargetArray.end(), pkUnit) != kSkillTargetArray.end())
					{
						EForceSetFlag const eFlag = static_cast<EForceSetFlag>(pkEffect->GetAbil(AT_FORCESKILL_FLAG));
						pkTarget->GetSkill()->ForceReserve(iSkillNo, eFlag);
						pkTarget->GetAI()->SetEvent(pkUnit->GetID(), EAI_EVENT_FORCE_SKILL);
						return ECT_MUSTDELETE;
					}
				}
			}
		}
		++unit_itor;
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgDistanceWallFunction - Effect Function 유닛과 유닛2의 거리에 장벽을 만들어 검출되는 타겟에게 데미지를 줌
///////////////////////////////////////////////////////////
void PgDistanceWallFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

void PgDistanceWallFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
}

int PgDistanceWallFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	if ( pkUnit->IsDead() )
	{
		return ECT_MUSTDELETE;
	}

	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	CUnit* pkUnit2 = pkGround->GetUnitByClassNo(pkEffect->GetAbil(AT_CLASS), UT_MONSTER);
	if(NULL == pkUnit2 || pkUnit2->IsDead())
	{
		return ECT_MUSTDELETE;
	}

	int const iSkillNo = pkEffect->GetAbil(AT_MON_SKILL_01);
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);
	if(NULL == pkSkillDef)
	{
		INFO_LOG(BM::LOG_LV3, __FL__<<"Not Find SkillNo: "<<iSkillNo<<", EffectID: "<<pkEffect->GetEffectNo());
		return ECT_MUSTDELETE;
	}

	float const fDistance = ::GetDistance(pkUnit->GetPos(), pkUnit2->GetPos());
	SAddRange const kAddRange(abs(fDistance),0,0,0);

	POINT3 kDir = pkUnit2->GetPos() - pkUnit->GetPos();
	kDir.Normalize();

	UNIT_PTR_ARRAY kUnitArray;
	pkGround->AI_GetSkillTargetList(pkUnit, iSkillNo, kUnitArray, true, pkArg, &kDir, kAddRange);

	for(UNIT_PTR_ARRAY::iterator itor = kUnitArray.begin(); itor != kUnitArray.end(); ++itor)
	{
		CUnit* pkTarget = (*itor).pkUnit;
		if(pkTarget)
		{
			PgActionResult kResult;
			kResult.Init();

			bool const bUseRandomSeedType = pkUnit->UseRandomSeedType();
			pkUnit->UseRandomSeedType(false);
			CS_GetDmgResult(iSkillNo, pkUnit, pkTarget, ABILITY_RATE_VALUE, 0, &kResult);
			pkUnit->UseRandomSeedType(bUseRandomSeedType);

			DoTickDamage(pkUnit, pkTarget, kResult.GetValue(), 0, pkSkillDef->GetEffectNo(), pkArg, false);
		}
	}

	return ECT_DOTICK;
}


///////////////////////////////////////////////////////////
//  PgRandomAttachEffectFunction - 랜덤하게 이펙트를 붙임
///////////////////////////////////////////////////////////
void PgRandomAttachEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	int const iRateTblNo = pkEffect->GetAbil(AT_ITEM_BAG_SUCCESS_RATE_CONTROL);

	size_t iRetIDX = 0;
	if( ::RouletteRate(iRateTblNo, iRetIDX, EFFECTNUM_MAX) )
	{
		if(int const iAddEffect = pkEffect->GetAbil(AT_EFFECTNUM1 + iRetIDX))
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iAddEffect, 0, pkArg, pkUnit->GetID());
			pkUnit->AddEffectQueue(kData);
		}
	}
}


///////////////////////////////////////////////////////////
//  PgSummonEntityEffectFunction - 엔티티 소환
///////////////////////////////////////////////////////////
void PgSummonEntityEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	PgGround* pkGround = NULL;
	pkArg->Get(ACTARG_GROUND, pkGround);
	if (!pkGround)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return;
	}

	int const iClass = pkEffect->GetAbil(AT_CLASS);
	int const iLevel = std::max(1, pkEffect->GetAbil(AT_LEVEL));

	POINT3 kTargetPos = pkUnit->GetPos();

	NxVec3 kVision;
	pkGround->GetVisionDirection(pkUnit, kVision);
	if(int const iDistance = pkEffect->GetAbil(AT_DISTANCE))
	{
		kVision *= (NxReal)iDistance;
	}
	kTargetPos.x += kVision.x;
	kTargetPos.y += kVision.y;
	kTargetPos.z += kVision.z;

	NxRay kRay(NxVec3(kTargetPos.x, kTargetPos.y, kTargetPos.z+20), NxVec3(0, 0, -1.0f));

	NxRaycastHit kHit;
	NxShape *pkHitShape = pkGround->RayCast(kRay, kHit);
	if(pkHitShape)
	{
		kTargetPos.z = kHit.worldImpact.z;
	}

	//엔티티 만들기
	GET_DEF(PgClassDefMgr, kClassDefMgr);
	SClassKey const kKey(iClass, iLevel);
	SCreateEntity kCreateInfo( BM::GUID::Create(), kKey );
	kCreateInfo.ptPos = kTargetPos;
	kCreateInfo.bEternalLife = (0 == kClassDefMgr.GetAbil(kKey, AT_LIFETIME));

	pkGround->CreateEntity(pkUnit, &kCreateInfo, L"EffectSummon");
}


///////////////////////////////////////////////////////////
//  PgChangeAIEffectFunction - Effect Function 조건에 따라 AI를 변경함
///////////////////////////////////////////////////////////
enum E_CHANGE_AI_CONDITION
{
	ECAIC_NONE				= 0,
	ECAIC_WHILE				= 1,	//이펙트가 적용되는 동안
};

template<typename T>
inline void SetAIType(CUnit * pkUnit, T const& pkDef)
{
	if(pkUnit && pkDef)
	{
		int iValue = pkDef->GetAbil(AT_AI_TYPE);

		SUnit_AI_Info * pkAI = pkUnit->GetAI();
		EAIActionType const eCurAction = pkAI->eCurrentAction;
		pkAI->Clear();
		pkAI->SetPattern((iValue>0) ? iValue : 1);
		pkAI->SetCurrentAction(eCurAction);
		pkAI->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);

		//오프닝 중이면 굳이 Idle로 변경할 필요가 없으므로
		if(EAI_ACTION_OPENING != eCurAction)
		{
			pkAI->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
		}

		for(int i=1; i<EAI_ACTION_MAX; ++i)
		{
			iValue = pkDef->GetAbil(AT_AI_ACTIONTYPE_MIN + i);
			if (iValue != 0)
			{
				pkAI->AddActionType((EAIActionType)i, iValue);
			}
		}
	}
}

inline void SetAIType(CUnit * pkUnit, SClassKey const& kKey)
{
	if(pkUnit)
	{
		GET_DEF(PgClassDefMgr, kClassDefMgr);

		int iValue = kClassDefMgr.GetAbil(kKey, AT_AI_TYPE);

		SUnit_AI_Info * pkAI = pkUnit->GetAI();
		EAIActionType const eCurAction = pkAI->eCurrentAction;
		pkAI->Clear();
		pkAI->SetPattern((iValue>0) ? iValue : 1);
		pkAI->SetCurrentAction(eCurAction);

		//오프닝 중이면 굳이 Idle로 변경할 필요가 없으므로
		if(EAI_ACTION_OPENING != eCurAction)
		{
			pkAI->SetEvent(BM::GUID::NullData(), EAI_EVENT_IDLE);
		}

		for(int i=1; i<EAI_ACTION_MAX; ++i)
		{
			iValue = kClassDefMgr.GetAbil(kKey, AT_AI_ACTIONTYPE_MIN+i);
			if (iValue != 0)
			{
				pkAI->AddActionType((EAIActionType)i, iValue);
			}
		}
	}
}

void PgChangeAIEffectFunction::EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	switch(pkEffect->GetAbil(AT_TYPE))
	{
	case ECAIC_WHILE:
	default:
		{
			SetAIType(pkUnit, pkEffect);
		}break;
	}
}
void PgChangeAIEffectFunction::EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg)
{
	switch(pkEffect->GetAbil(AT_TYPE))
	{
	case ECAIC_WHILE:
	default:
		{
			int iType = 0;
			CAbilObject * pkDef = NULL;
			if( pkUnit->IsUnitType(UT_MONSTER) )
			{
				GET_DEF(CMonsterDefMgr, kMonsterDefMgr);
				const CMonsterDef* pkMonsterDef = kMonsterDefMgr.GetDef(pkUnit->GetAbil(AT_CLASS));

				SetAIType(pkUnit, pkMonsterDef);
			}
			else
			{
				SClassKey const kKey(pkUnit->GetAbil(AT_CLASS),pkUnit->GetAbil(AT_LEVEL));
				SetAIType(pkUnit, kKey);
			}
		}break;
	}
}

int PgChangeAIEffectFunction::EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed)
{
	PgGround* pkGround = NULL;
	if(pkArg)
	{
		pkArg->Get(ACTARG_GROUND, pkGround);
	}

	if(!pkGround)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot find Ground, EffectNo="<<pkEffect->GetEffectNo());
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkGround is NULL"));
		return ECT_MUSTDELETE;
	}

	return ECT_DOTICK;
}
