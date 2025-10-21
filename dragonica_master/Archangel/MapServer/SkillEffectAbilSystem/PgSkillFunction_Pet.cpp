#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Pet.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Variant/Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "Global.h"
#include "PgGround.h"
#include "PgEffectFunction.h"

///////////////////////////////////////////////////////////
//  PgGoldRushSkillFunction
///////////////////////////////////////////////////////////
int PgGoldRushSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	PgGround *pkGround = GetGroundPtr( pArg );
	if( !pkGround )
	{
		return -1;
	}

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);	

	int const iEffectPercent = pkSkill->GetAbil(AT_PERCENTAGE);

	int const iHitRate = ABILITY_RATE_VALUE; //힛트 확률은 100%

	VEC_GUID kVecGuid;
	kVecGuid.push_back( pkUnit->Caller() );

	PgLogCont kLogCont(ELogMain_Contents_Pet, ELogSub_Pet );
	kLogCont.MemberKey( pkGround->GroundKey().Guid() );// GroundGuid
	kLogCont.CharacterKey( pkUnit->GetID() );
	kLogCont.GroundNo( pkGround->GetGroundNo() );// GroundNo
	kLogCont.Name( pkUnit->Name() );
	kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

	CheckTagetVaild(pkUnit, pkUnitArray, pkSkill, 1000); //Target이 유효한 타겟인지 검사한다.

	int const iGoldDropMin = pkSkill->GetAbil(AT_GOLD_RUSH_DROPMONEY_VALUE_MIN);
	int const iGoldDropMax = pkSkill->GetAbil(AT_GOLD_RUSH_DROPMONEY_VALUE_MAX);
	int const iRandomValue = abs(iGoldDropMax - iGoldDropMin);

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(pkTarget)
		{
			pkUnit->OnTargetDamaged(pkTarget->GetID());

			PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
			if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference) && (false == (*unit_itor).bRestore))
			{
				if(!pkAResult->GetMissed())
				{
					int iResult = 0;
					if(0 != iRandomValue)
					{
						iResult = BM::Rand_Index(iRandomValue);
					}

					__int64 i64Money = iGoldDropMin + iResult;
					if ( 0i64 < i64Money )
					{
						//타겟 위치에 돈 뿌려주기~
						pkGround->InsertItemBox( pkTarget->GetPos(), kVecGuid, NULL, PgBase_Item::NullData(), i64Money, kLogCont );
					}
				}
			}
		}

		++unit_itor;
	}
	
	kLogCont.Commit();
	return 1;
}

int PgPetProduceItem::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(iSkillNo);	
	if (NULL == pkSkillDef)
	{
		return -1;
	}
	PgGround *pkGround = GetGroundPtr(pArg);
	if(NULL == pkGround)
	{
		return -1;
	}
	if(true == pkUnitArray->empty())
	{
		return 1;
	}

	int const iParentSkillNo = pkSkillDef->GetParentSkill() > 0 ? pkSkillDef->GetParentSkill() : iSkillNo;
	int const iParentEffectNo = GET_BASICSKILL_NUM(iParentSkillNo);	// 같은 이펙트 레벨 단위의 숫자를 검증하기 위한 1레벨 이펙트 번호
	
	bool bFoundEffect = false;
	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		if(NULL == pkTarget)
		{
			++unit_itor;
			continue;
		}

		bFoundEffect = pkTarget->GetEffect(iParentEffectNo, true) != NULL;
		if(false == bFoundEffect)
		{
			if(true == ProduceItem(pkUnit, pkTarget, pkSkillDef, pkGround))
			{ //성공이면 대상에게 이펙트를 걸어줘야 함 (쿨타임의 의미)
				pkTarget->AddEffect(pkSkillDef->GetEffectNo(), 0, pArg, pkUnit);				
			}
			else { return -1; }
		}
		else
		{ //대상 유닛에 버프가 이미 걸려있으면 실패처리
			//pkTarget->SendWarnMessage(790254); // 아직 사용할 수 없습니다.
			return 1;
		}
		++unit_itor;
	}

	return 1;
}

bool PgPetProduceItem::ProduceItem(CUnit* pkCaster, CUnit* pkTargetUnit, CSkillDef const* pkSkillDef, PgGround* pkGround)
{
	if(NULL == pkCaster || NULL == pkTargetUnit || NULL == pkSkillDef || NULL == pkGround) { return false; }
	if(pkTargetUnit->UnitType() != UT_PLAYER) { return false; }

	int iItemBagGrpNo = pkSkillDef->GetAbil(AT_ITEM_BAG_PRIMARY);
	if(0 == iItemBagGrpNo) { return false; }

//	PgAction_PopItemBag kAction(CIE_PetProduceItem, pkGround->GroundKey(), 0, iItemBagGrpNo, 0, NULL, 0);
//	return kAction.DoAction(pkTargetUnit, NULL);
	PgItemBag kItemBag;
	int iResultItemNo = 0;
	int iResultItemCount = 0;
	GET_DEF(CItemBagMgr, kItemBagMgr);
	kItemBagMgr.GetItemBagByGrp(iItemBagGrpNo, pkTargetUnit->GetAbil(AT_LEVEL), kItemBag);
	kItemBag.PopItem(pkTargetUnit->GetAbil(AT_LEVEL), iResultItemNo, iResultItemCount);

	CONT_PLAYER_MODIFY_ORDER kOrderModify;
	SPMOD_Add_Any AddData(iResultItemNo,iResultItemCount);
	SPMO MODS_Add(IMET_ADD_ANY,pkTargetUnit->GetID(),AddData);
	kOrderModify.push_back(MODS_Add);

	PgAction_ReqModifyItem kItemModifyAction(CIE_PetProduceItem, pkGround->GroundKey(), kOrderModify);
	return kItemModifyAction.DoAction(pkCaster, pkTargetUnit);

}

