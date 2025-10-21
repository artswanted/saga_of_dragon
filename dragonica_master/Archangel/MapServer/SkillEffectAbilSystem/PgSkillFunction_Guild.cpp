#include "stdafx.h"
#include "PgSkillFunction.h"
#include "PgSkillFunction_Guild.h"
#include "Variant/PgActionResult.h"
#include "Variant/PgTotalObjectMgr.h"
#include "Global.h"

int PgCoupleLoverSkillFunction::SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult)
{
	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkill = kSkillDefMgr.GetDef(iSkillNo);
	
	//커플 스킬은 2명이 아니라면 효과를 얻지 못한다.
	if(2 > pkUnitArray->size())
	{
		return 1;
	}

	UNIT_PTR_ARRAY::iterator unit_itor = pkUnitArray->begin();
	while(pkUnitArray->end() != unit_itor)
	{
		CUnit* pkTarget = (*unit_itor).pkUnit;
		PgActionResult* pkAResult = pkResult->GetResult(pkTarget->GetID(), true);
		if(pkAResult && !pkAResult->GetInvalid() && (false == (*unit_itor).bReference))
		{
			int const iEffectNo = pkSkill->GetEffectNo();
			if(iEffectNo)
			{
				pkTarget->AddEffect(iEffectNo, 0, pArg, pkUnit);
			}
		}

		++unit_itor;
	}

	return 1;
}
