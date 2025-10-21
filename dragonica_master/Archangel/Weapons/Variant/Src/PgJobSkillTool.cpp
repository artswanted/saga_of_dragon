#include "stdafx.h"
#include "Item.h"
#include "PgPlayer.h"
#include "PgJobskill.h"
#include "PgJobSkillExpertness.h"
#include "PgJobSkillTool.h"
#include "tabledatamanager.h"

bool JobSkillToolUtil::IsCanEquip(PgPlayer* pkPlayer, PgBase_Item const &rkToolItem, int &iErrorRet)
{//착용 가능한 도구인가?

	CONT_DEF_JOBSKILL_TOOL::mapped_type kToolInfo;
	if( !GetToolInfo( rkToolItem.ItemNo(), kToolInfo ) )
	{// 도구 정보를 찾을 수 없음
		iErrorRet = 20024;//아이템 정보가 없습니다.
		return false;
	}

	if( !JobSkillUtil::CheckJobSkillExpertness(*pkPlayer, kToolInfo.i01Need_Skill_No, kToolInfo.i01Need_Skill_Expertness) )
	{
		iErrorRet = -1;
		return false;
	}

	if( !JobSkillUtil::CheckJobSkillExpertness(*pkPlayer, kToolInfo.i02Need_Skill_No, kToolInfo.i02Need_Skill_Expertness) )
	{
		iErrorRet = -1;
		return false;
	}
	if( !JobSkillUtil::CheckJobSkillExpertness(*pkPlayer, kToolInfo.i03Need_Skill_No, kToolInfo.i03Need_Skill_Expertness) )
	{
		iErrorRet = -1;
		return false;
	}
	return true;
}

int JobSkillToolUtil::GetToolType(int const iItemNo)
{//도구의 타입을 가져옴. 0이면 도구가 아님.
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const* pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( pkItemDef )
	{
		return pkItemDef->GetAbil(AT_JOBSKILL_TOOL_TYPE);
	}

	return 0;
}

bool JobSkillToolUtil::GetToolInfo(int const iItemNo, CONT_DEF_JOBSKILL_TOOL::mapped_type &kOutValue)
{//도구 정보 가져온다
	CONT_DEF_JOBSKILL_TOOL const * pkContDefJobSkillToolUtil = NULL;
	g_kTblDataMgr.GetContDef( pkContDefJobSkillToolUtil );
	if( pkContDefJobSkillToolUtil )
	{
		CONT_DEF_JOBSKILL_TOOL::const_iterator toolItr = pkContDefJobSkillToolUtil->find( iItemNo );
		if( pkContDefJobSkillToolUtil->end() != toolItr )
		{
			kOutValue = toolItr->second;
			return true;
		}
	}

	return false;
}

int JobSkillToolUtil::GetToolGatherType(int const iItemNo)
{
	CONT_DEF_JOBSKILL_TOOL::mapped_type kValue;
	if( GetToolInfo(iItemNo, kValue) )
	{
		return kValue.iGatherType;
	}
	return 0;
}

bool JobSkillToolUtil::CheckNeedSkill(int const iJobSkillNo, int const iJobSkillExpertness, PgJobSkillExpertness const &rkJobSkillExpertness)
{//IsCanEquip에서 사용. 착용 가능 스킬을 배웠는지 체크
	if( iJobSkillNo && !rkJobSkillExpertness.IsHave(iJobSkillNo) )
	{
		return false;
	}
	else
	{
		if( iJobSkillExpertness > rkJobSkillExpertness.Get(iJobSkillNo) )
		{
			return false;
		}
	}

	return true;
}


int JobSkillToolUtil::GetResultCount(int const iItemNo)
{//도구로 얻을수 있는 채집물 횟수(갯수가 아님! 이거 만큼 확률 계산을 통해 최종 갯수를 구하게 됨)
	CONT_DEF_JOBSKILL_TOOL const * pkContDefJobSkillToolUtil = NULL;
	g_kTblDataMgr.GetContDef( pkContDefJobSkillToolUtil );
	if( pkContDefJobSkillToolUtil )
	{
		CONT_DEF_JOBSKILL_TOOL::const_iterator toolItr = pkContDefJobSkillToolUtil->find( iItemNo );
		if( pkContDefJobSkillToolUtil->end() != toolItr )
		{
			return BM::Rand_Range((toolItr->second).iGetCount, (toolItr->second).iGetCountMax);
		}
	}

	return 0;
}

DWORD JobSkillToolUtil::CalcOptionTurnTime(int const iItemNo, DWORD const dwDurTime)
{//도구 능력치에 따라 변경되는 truntime을 계산하여 리턴
	CONT_DEF_JOBSKILL_TOOL const * pkContDefJobSkillToolUtil = NULL;
	g_kTblDataMgr.GetContDef( pkContDefJobSkillToolUtil );
	if( pkContDefJobSkillToolUtil )
	{
		CONT_DEF_JOBSKILL_TOOL::const_iterator toolItr = pkContDefJobSkillToolUtil->find( iItemNo );
		if( pkContDefJobSkillToolUtil->end() != toolItr )
		{
			DWORD const dwOptionTime = dwDurTime * ( (toolItr->second).iOption_TurnTime / ABILITY_RATE_VALUE );
			return dwDurTime - dwOptionTime;
		}
	}

	return 0;
}
int JobSkillToolUtil::GetUseSkill(PgPlayer * pkPlayer, int const iGatherType, int const iMainJobSkillNo, int const iSubJobSkillNo)
{//GatherType에 사용가능한 도구가 있는지 확인하여 스킬 번호를 리턴
	if( !pkPlayer )
	{
		return 0;
	}

	PgInventory * pkInv = pkPlayer->GetInven();
	if( !pkInv )
	{
		return 0;
	}

	PgBase_Item kMainItem;
	PgBase_Item kSubItem;
	pkInv->GetItem( SItemPos(IT_FIT, EQUIP_POS_SHEILD), kSubItem);
	pkInv->GetItem( SItemPos(IT_FIT, EQUIP_POS_WEAPON), kMainItem);
	if( kMainItem.IsEmpty())
	{//에러 - 착용해라
		pkPlayer->SendWarnMessage(25017);
		return 0;
	}
	//GM커맨드 확인
	bool const bUseGodCmd = pkPlayer->GetAbil(AT_GM_JOBSKILL_USE_SUBTOOL);

	int iOutJobSkillNo = 0;
	CONT_DEF_JOBSKILL_TOOL::mapped_type kMainTool;
	CONT_DEF_JOBSKILL_TOOL::mapped_type kSubTool;
	if( GetToolInfo(kSubItem.ItemNo(), kSubTool)
		&&	(iGatherType == kSubTool.iGatherType || ETOOL_ALL_SUB == kSubTool.iToolType ) )
	{
		if( JobSkill_LearnUtil::IsEnableUseJobSkill( const_cast<PgPlayer const*>(pkPlayer), iSubJobSkillNo ) )
		{
			CONT_DEF_JOBSKILL_SKILLEXPERTNESS::value_type kExpertness;
			JobSkillExpertnessUtil::FindExpertnessArea(iSubJobSkillNo, pkPlayer->JobSkillExpertness().Get(iMainJobSkillNo), kExpertness);
			int const iProbabilyty = kExpertness.iSkill_Probability;
			int const iRandRet = BM::Rand_Range(JSE_SKILL_PROBABILITY_DEVIDE);
			if( (iProbabilyty >= iRandRet) || (bUseGodCmd) ) 
			{// 보조스킬은 발동확률이 존재한다 갓커맨드 사용시 확률 무시한다.
				return iSubJobSkillNo;
			}
		}
	}
	if( GetToolInfo(kMainItem.ItemNo(), kMainTool)
		&&	(iGatherType == kMainTool.iGatherType || ETOOL_ALL == kMainTool.iToolType ) )
	{
		return iMainJobSkillNo;
	}

	return 0;
}

int JobSkillToolUtil::GetUseItem(PgPlayer * pkPlayer, int const iGatherType, int const iUseSkillNo)
{//사용가능한 툴 아이템 번호를 리턴함
	if( !pkPlayer )
	{
		return false;
	}

	PgInventory * pkInv = pkPlayer->GetInven();
	if( !pkInv )
	{
		return false;
	}
	
	PgBase_Item kMainItem;
	PgBase_Item kSubItem;
	pkInv->GetItem( SItemPos(IT_FIT, EQUIP_POS_SHEILD), kSubItem);
	pkInv->GetItem( SItemPos(IT_FIT, EQUIP_POS_WEAPON), kMainItem);
	if( kSubItem.IsEmpty() && kMainItem.IsEmpty())
	{
		//에러 - 착용해라
	}

	int iOutJobSkillNo = 0;
	CONT_DEF_JOBSKILL_TOOL::mapped_type kMainTool;
	CONT_DEF_JOBSKILL_TOOL::mapped_type kSubTool;
	if( GetToolInfo(kSubItem.ItemNo(), kSubTool)
		&&	(iGatherType == kSubTool.iGatherType || ETOOL_ALL_SUB == kSubTool.iToolType ) )
	{
		CONT_DEF_JOBSKILL_SKILL const* pkDefJSSkill = NULL;
		g_kTblDataMgr.GetContDef(pkDefJSSkill);
		switch( JobSkillUtil::GetJobSkillType(iUseSkillNo, *pkDefJSSkill) )
		{
		case JST_1ST_MAIN: {
			}break;
		case JST_1ST_SUB: {
				return kSubItem.ItemNo();
			}break;
		}
	}
	if( GetToolInfo(kMainItem.ItemNo(), kMainTool)
		&&	(iGatherType == kMainTool.iGatherType || ETOOL_ALL == kMainTool.iToolType ) )
	{
		return kMainItem.ItemNo();
	}

	return 0;
}