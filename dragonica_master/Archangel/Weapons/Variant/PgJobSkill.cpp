#include "stdafx.h"
#include "PgJobSkill.h"

#include "PgPlayer.h"
#include "tabledatamanager.h"
#include "PgJobSkillExpertness.h"
#include "PgJobSkillTool.h"

__int64 const BASIC_LEARN_COST = 2;
float const ENCREASE_COST_RATE = 2.5f;

EJOBSKILL_LEARN_RET JobSkill_LearnUtil::IsEnableLearnJobSkill(PgPlayer const* pPlayer, int const iJobSkillNo)
{
	if(!pPlayer)
	{
		return JSLR_ERR;
	}
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	if(0 == pkContDefJobSkill->size())
	{
		return JSLR_ERR;
	}
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Skill = pkContDefJobSkill->find(iJobSkillNo);
	if(iter_Skill == pkContDefJobSkill->end() )
	{//직업스킬이 아니다
		return JSLR_ERR;
	}
	CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Skill).second;
	PgJobSkillExpertness const& rkJobSkillExpertness = pPlayer->JobSkillExpertness();
	bool const bHaveSkill = rkJobSkillExpertness.IsHave( iJobSkillNo );
	if( bHaveSkill )
	{//이미 배웠다.
		return JSLR_ALREADY_BUY;
	}
	return JSLR_OK;
}

int JobSkill_LearnUtil::GetHaveJobSkillTypeCount(PgPlayer *pPlayer, EJobSkillType const eType)
{
	if(!pPlayer)
	{
		return 0;
	}
	PgMySkill* pMySkill = pPlayer->GetMySkill();
	if(!pMySkill)
	{
		return 0;
	}	
	int iCount = 0;
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	if(0 == pkContDefJobSkill->size())
	{
		return 0;
	}

	bool bMainSkill = IsMainJobSkill(eType);

	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Skill = pkContDefJobSkill->begin();
	for(iter_Skill; iter_Skill != pkContDefJobSkill->end(); ++iter_Skill)
	{
		CONT_DEF_JOBSKILL_SKILL::key_type const &rkJobSkillNo = (*iter_Skill).first;
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Skill).second;

		if( bMainSkill == IsMainJobSkill(rkJobSkillInfo.eJobSkill_Type) )
		{
			bool const bHaveSkill = pMySkill->IsExist(rkJobSkillNo);
			if(bHaveSkill)
			{
				++iCount;
			}
		}
	}
	return iCount;
}

int JobSkill_LearnUtil::GetHaveJobSkillTypeCount(PgPlayer *pPlayer, int const iJobSkillNo)//같은 타입스킬 몇개 가지고 있냐
{
	if(!pPlayer)
	{
		return 0;
	}
	PgMySkill* pMySkill = pPlayer->GetMySkill();
	if(!pMySkill)
	{
		return 0;
	}	
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	if(0 == pkContDefJobSkill->size())
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Skill = pkContDefJobSkill->find(iJobSkillNo);
	if( pkContDefJobSkill->end() == iter_Skill )
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Skill).second;
	EJobSkillType const eOrgType = rkJobSkillInfo.eJobSkill_Type;

	int iCount = 0;

	bool bMainSkill = IsMainJobSkill(eOrgType);

	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Comp = pkContDefJobSkill->begin();
	for(iter_Comp; iter_Comp != pkContDefJobSkill->end(); ++iter_Comp)
	{
		CONT_DEF_JOBSKILL_SKILL::key_type const &rkJobSkillNo_Comp = (*iter_Comp).first;
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo_Comp = (*iter_Comp).second;
		if( bMainSkill == IsMainJobSkill(rkJobSkillInfo.eJobSkill_Type) )
		{
			bool const bHaveSkill = pMySkill->IsExist(rkJobSkillNo_Comp);
			if(bHaveSkill)
			{
				++iCount;
			}
		}
	}
	return iCount;
}

int JobSkill_LearnUtil::GetHaveJobSkillTypeCount_Detail(PgPlayer *pPlayer, int const iJobSkillNo)//같은 채집타입을 몇개 가지고 있냐
{
	if(!pPlayer)
	{
		return 0;
	}
	PgMySkill* pMySkill = pPlayer->GetMySkill();
	if(!pMySkill)
	{
		return 0;
	}	
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	if(0 == pkContDefJobSkill->size())
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Skill = pkContDefJobSkill->find(iJobSkillNo);
	if( pkContDefJobSkill->end() == iter_Skill )
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Skill).second;
	EJobSkillType const eOrgType = rkJobSkillInfo.eJobSkill_Type;

	int iCount = 0;

	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Comp = pkContDefJobSkill->begin();
	for(iter_Comp; iter_Comp != pkContDefJobSkill->end(); ++iter_Comp)
	{
		CONT_DEF_JOBSKILL_SKILL::key_type const &rkJobSkillNo_Comp = (*iter_Comp).first;
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo_Comp = (*iter_Comp).second;
		if( eOrgType == rkJobSkillInfo_Comp.eJobSkill_Type)
		{
			bool const bHaveSkill = pMySkill->IsExist(rkJobSkillNo_Comp);
			if(bHaveSkill)
			{
				++iCount;
			}
		}
	}
	return iCount;
}

bool JobSkill_LearnUtil::IsEnableUseJobSkill(PgPlayer const *pPlayer, int const iJobSkillNo)
{
	if(!pPlayer)
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	if(0 == pkContDefJobSkill->size())
	{
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_Skill = pkContDefJobSkill->find(iJobSkillNo);
	if(iter_Skill == pkContDefJobSkill->end() )
	{//직업스킬이 아니다
		return false;
	}
	CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkillInfo = (*iter_Skill).second;

	if( 0 != rkJobSkillInfo.i01NeedParent_JobSkill_No)
	{//선행 스킬이 필요 할 경우(보조 스킬의 경우)
		PgJobSkillExpertness const& rkJobSkillExpertness = pPlayer->JobSkillExpertness();
		bool const bIsHave = rkJobSkillExpertness.IsHave( rkJobSkillInfo.i01NeedParent_JobSkill_No );
		if( !bIsHave )
		{//선행스킬 안배웟다.
			return false;
		}
		else
		{//배웠는데,
			int const iExpertness = rkJobSkillExpertness.Get(rkJobSkillInfo.i01NeedParent_JobSkill_No);
			if( rkJobSkillInfo.i01NeedParent_JobSkill_Expertness > iExpertness)
			{//숙련도가 부족하다.
				return false;
			}
		}
	}
	else
	{//선행 스킬이 필요 없을 경우(주 스킬의 경우)
		int const iClass = pPlayer->GetAbil(AT_CLASS);
		if( UCLASS_THIEF >= iClass )
		{//1차전직 안했다.
			return false;
		}
		int const iProbabilyty = pPlayer->JobSkillExpertness().Get(iJobSkillNo);
		if( !iProbabilyty )
		{
			return false;
		}
	}
	return true;
}

bool JobSkill_LearnUtil::IsMainJobSkill(EJobSkillType const eType) //주스킬이냐
{
	if( eType == JST_1ST_MAIN
	|| eType == JST_2ND_MAIN
	|| eType == JST_3RD_MAIN)
	{
		return true;
	}
	return false;
}

bool JobSkill_Util::IsJobSkill_Tool(int const iItemNo)
{
	CONT_DEF_JOBSKILL_TOOL const *pkDefJobSkillTool = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillTool);
	CONT_DEF_JOBSKILL_TOOL::const_iterator find_iter = pkDefJobSkillTool->find(iItemNo);
	if( find_iter != pkDefJobSkillTool->end())
	{
		return true;
	}
	return false;
}

bool JobSkill_Util::IsUseableJobSkill_Tool(PgPlayer const *pPlayer, int const iItemNo)
{	
	if(!pPlayer)
	{
		return false;
	}
	bool bResult = false;
	CONT_DEF_JOBSKILL_TOOL::mapped_type kJobTool;
	if( true == JobSkillToolUtil::GetToolInfo(iItemNo, kJobTool) )
	{//채집 도구의 경우
		if(pPlayer)
		{
			bResult = (JobSkill_LearnUtil::IsEnableUseJobSkill(pPlayer, kJobTool.i01Need_Skill_No));
		}
		if( !bResult )
		{//필요스킬이 없는경우, 도구 타입이 만능도구면 OK
			bResult = ( (ETOOL_ALL==kJobTool.iToolType) || (ETOOL_ALL_SUB ==kJobTool.iToolType) );
		}
	}
	return bResult;
}
bool JobSkill_Util::IsJobSkill_SaveIndex(int const iItemNo)
{
	CONT_DEF_JOBSKILL_SAVEIDX const* pkDefJobSkillSaveIdx = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillSaveIdx);
	CONT_DEF_JOBSKILL_SAVEIDX::const_iterator iter = pkDefJobSkillSaveIdx->begin();
	while( iter != pkDefJobSkillSaveIdx->end() )
	{
		if( iter->second.iBookItemNo == iItemNo )
		{
			return true;
		}
		++iter;
	}
	return false;
}
bool JobSkill_Util::GetJobSkill_SaveIndex(int const iItemNo, CONT_DEF_JOBSKILL_SAVEIDX::mapped_type& rkSaveIdx)
{
	CONT_DEF_JOBSKILL_SAVEIDX const* pkDefJobSkillSaveIdx = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillSaveIdx);
	CONT_DEF_JOBSKILL_SAVEIDX::const_iterator iter = pkDefJobSkillSaveIdx->begin();
	while( iter != pkDefJobSkillSaveIdx->end() )
	{
		if( iter->second.iBookItemNo == iItemNo )
		{
			rkSaveIdx = (*iter).second;
			return true;
		}
		++iter;
	}
	return false;
}
bool JobSkill_Util::IsJobSkill_Item(int const iItemNo)
{
	CONT_DEF_JOBSKILL_ITEM_UPGRADE const* pkDefJobSkill_ItemUpgrade = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill_ItemUpgrade);
	CONT_DEF_JOBSKILL_ITEM_UPGRADE::const_iterator find_iter = pkDefJobSkill_ItemUpgrade->find(iItemNo);
	if( pkDefJobSkill_ItemUpgrade->end() != find_iter)
	{
		return true;
	}
	return false;
}
bool JobSkill_Util::GetJobSkill_Item(int const iItemNo, CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type &rkItemUpgrade)
{
	CONT_DEF_JOBSKILL_ITEM_UPGRADE const* pkDefJobSkill_ItemUpgrade = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill_ItemUpgrade);
	CONT_DEF_JOBSKILL_ITEM_UPGRADE::const_iterator find_iter = pkDefJobSkill_ItemUpgrade->find(iItemNo);
	if( pkDefJobSkill_ItemUpgrade->end() != find_iter)
	{
		rkItemUpgrade = (*find_iter).second;
		return true;
	}
	return false;
}

EJobSkillType JobSkill_Util::GetJobSkillType(int const iSkillNo)
{
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	
	CONT_DEF_JOBSKILL_SKILL::const_iterator find_iter = pkContDefJobSkill->find(iSkillNo);
	if(find_iter != pkContDefJobSkill->end() )
	{
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkill = (*find_iter).second;

		return rkJobSkill.eJobSkill_Type;
	}

	return JST_NONE;

}

eJobSkillMaterialType JobSkill_Util::GetJobSkillMaterialType(int const iItemNo)
{
	CONT_DEF_JOBSKILL_ITEM_UPGRADE const * pkContDefJobSkillItemUpgrade;
	g_kTblDataMgr.GetContDef(pkContDefJobSkillItemUpgrade);
	CONT_DEF_JOBSKILL_ITEM_UPGRADE::const_iterator find_iter = pkContDefJobSkillItemUpgrade->find(iItemNo);

	if( find_iter != pkContDefJobSkillItemUpgrade->end() )
	{
		CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type const& rkDataOut = (*find_iter).second;
		return static_cast<eJobSkillMaterialType>(rkDataOut.iResourceType);
	}
	return JSMRT_ERR;
}

int JobSkill_Util::GetJobSkill_SubSkill(int const iMainSkillNo)
{
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_jobskill = pkContDefJobSkill->begin();
	while(iter_jobskill != pkContDefJobSkill->end() )
	{
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkill = (*iter_jobskill).second;
		if( (JST_1ST_SUB == rkJobSkill.eJobSkill_Type)
			&& (iMainSkillNo == rkJobSkill.i01NeedParent_JobSkill_No) )
		{
			return (*iter_jobskill).first;
		}
		++iter_jobskill;
	}
	return 0;
}

int JobSkill_Util::GetJobSkill_MainSkill(int const iSubSkillNo)
{
	CONT_DEF_JOBSKILL_SKILL const* pkContDefJobSkill;
	g_kTblDataMgr.GetContDef(pkContDefJobSkill);
	
	CONT_DEF_JOBSKILL_SKILL::const_iterator iter_jobskill = pkContDefJobSkill->find(iSubSkillNo);
	if(iter_jobskill != pkContDefJobSkill->end() )
	{
		CONT_DEF_JOBSKILL_SKILL::mapped_type const &rkJobSkill = (*iter_jobskill).second;
		return rkJobSkill.i01NeedParent_JobSkill_No;
	}
	return 0;
}

//
SJobSkillSaveIdx const * const JobSkill_Third::GetJobSkillSaveIdx(int const iSaveIdx)
{
	CONT_DEF_JOBSKILL_SAVEIDX const* pkContDefSaveIdx = NULL;
	g_kTblDataMgr.GetContDef(pkContDefSaveIdx);
	if( !pkContDefSaveIdx )
	{
		return NULL;
	}

	CONT_DEF_JOBSKILL_SAVEIDX::const_iterator c_iter = pkContDefSaveIdx->find(iSaveIdx);
	if( c_iter == pkContDefSaveIdx->end() )
	{
		return NULL;
	}
	
	return &(c_iter->second);
}

int JobSkill_Third::GetResourceProbabilityUp(PgInventory * const pkInv, CONT_JS3_RESITEM_INFO const& kContResItemInfo)
{
	int iRate = 0;
	if( !pkInv )
	{
		return iRate;
	}

	PgBase_Item kItem;
	CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type kItemUpgrade;
	CONT_JS3_RESITEM_INFO::const_iterator c_iter = kContResItemInfo.begin();
	while(c_iter != kContResItemInfo.end())
	{
		if( JobSkill_Util::GetJobSkill_Item((*c_iter).first, kItemUpgrade) )
		{
			iRate += kItemUpgrade.iResourceProbabilityUp * (*c_iter).second;
		}

		++c_iter;
	}

	return iRate;
}


int JobSkill_Third::GetJobSkill3ProbabilityNo(int const iItemNo)
{
	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( pkDefJobSkillRecipe )
	{
		CONT_DEF_JOBSKILL_RECIPE::const_iterator iter = pkDefJobSkillRecipe->find(iItemNo);
		if( iter != pkDefJobSkillRecipe->end() )
		{
			return (*iter).second.kProbability.iNo;
		}
	}
	return 0;
}

int JobSkill_Third::GetJobSkill3NeedProductPoint(int const iItemNo)
{
	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( pkDefJobSkillRecipe )
	{
		CONT_DEF_JOBSKILL_RECIPE::const_iterator iter = pkDefJobSkillRecipe->find(iItemNo);
		if( iter != pkDefJobSkillRecipe->end() )
		{
			return (*iter).second.iNeedProductPoint;
		}
	}
	return 0;
}

int JobSkill_Third::GetJobSkill3ExpertnessGain(int const iRecipeItemNo)
{
	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( pkDefJobSkillRecipe )
	{
		CONT_DEF_JOBSKILL_RECIPE::const_iterator iter = pkDefJobSkillRecipe->find(iRecipeItemNo);
		if( iter != pkDefJobSkillRecipe->end() )
		{
			return (*iter).second.iExpertnessGain;
		}
	}
	return 0;
}

HRESULT JobSkill_Third::CheckNeedSkill(PgPlayer const * pkPlayer, int const iSaveIdx)
{
	if( !pkPlayer )
	{
		return E_FAIL;
	}

	CONT_DEF_JOBSKILL_SAVEIDX const* pkContDefSaveIdx = NULL;
	g_kTblDataMgr.GetContDef(pkContDefSaveIdx);
	if( !pkContDefSaveIdx )
	{
		return E_FAIL;
	}

	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( !pkDefJobSkillRecipe )
	{
		return E_FAIL;
	}

	CONT_DEF_JOBSKILL_SAVEIDX::const_iterator save_it = pkContDefSaveIdx->find(iSaveIdx);
	if( save_it == pkContDefSaveIdx->end() )
	{
		return E_FAIL;
	}
	
	int const iNeedSkillNo = (*save_it).second.iNeedSkillNo01;

	if( !JobSkillSaveIdxUtil::IsUseableSaveIdx(pkPlayer, iSaveIdx) )
	{
		return E_JS3_CREATEITEM_NOT_LEARN_NEEDSKILL;
	}

	if( false==JobSkill_LearnUtil::IsEnableUseJobSkill(pkPlayer, iNeedSkillNo) )
	{
		return E_JS3_CREATEITEM_NOT_LEARN_NEEDSKILL;
	}

	return S_OK;
}

HRESULT JobSkill_Third::CheckNeedProductPoint(PgPlayer * pkPlayer, int const iRecipeItemNo)
{//1차 피로도로를 체크하도록 수정한다.
	if( !pkPlayer )
	{
		return E_FAIL;
	}
	PgJobSkillExpertness const &rkExpertness = pkPlayer->JobSkillExpertness();
	int const iMaxProductPoint = JobSkillExpertnessUtil::GetMaxExhaustion_1ST(pkPlayer->GetPremium(), rkExpertness.GetAllSkillExpertness());
	int const iCurProductPoint = iMaxProductPoint - rkExpertness.CurExhaustion();
	int const iNeedProductPoint = GetJobSkill3NeedProductPoint(iRecipeItemNo);
	if(iNeedProductPoint > iCurProductPoint)
	{
		return E_JS3_CREATEITEM_NOT_ENOUGH_NEED_PRODUCTPOINT;
	}
	return S_OK;
}

bool JobSkill_Third::UseResItem(int const iDefResourceGroupNo, int const iDefGrade, int const iItemNo, int const iResourceGroupNo, int const iGrade)
{
	if(0==iDefGrade)
	{
		if(iDefResourceGroupNo==iItemNo)
		{
			return true;
		}
	}
	else
	{
		if( (iDefResourceGroupNo == iResourceGroupNo)
			&& (iDefGrade <= iGrade) )
		{
			return true;
		}
	}

	return false;
}

bool JobSkill_Third::GetContGroupItemNo(int const iGroupNo, int const iGrade, VEC_INT & kContItemNo)
{
	CONT_DEF_JOBSKILL_ITEM_UPGRADE const* pkDefJobSkill_ItemUpgrade = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkill_ItemUpgrade);
	if(!pkDefJobSkill_ItemUpgrade)
	{
		return false;
	}

	CONT_DEF_JOBSKILL_ITEM_UPGRADE::const_iterator find_iter = pkDefJobSkill_ItemUpgrade->begin();
	while( pkDefJobSkill_ItemUpgrade->end() != find_iter)
	{
		if( (iGroupNo == (*find_iter).second.iResourceGroupNo)
		&& (iGrade <= (*find_iter).second.iGrade)
		&& (0 == (*find_iter).second.iUpgradeCount) )
		{
			kContItemNo.push_back((*find_iter).second.iItemNo);
		}
		++find_iter;
	}

	return false==kContItemNo.empty();
}

HRESULT JobSkill_Third::CheckNeedItems(PgInventory * const pkInv, int const iRecipeItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo)
{
	if( !pkInv )
	{
		return E_FAIL;
	}
	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( !pkDefJobSkillRecipe )
	{
		return E_FAIL;
	}

	CONT_DEF_JOBSKILL_RECIPE::mapped_type kRecipe;
	if( false==GetJobSkill3_Recipe(iRecipeItemNo, kRecipe) )
	{
		return E_JS3_CREATEITEM_NOT_FOUND_ITEM;
	}

	typedef std::map<int,int> CONT_MAP;
	CONT_MAP kContHaveItem;
	CONT_JS3_RESITEM_INFO::const_iterator resitem_info = kContResItemInfo.begin();
	while(resitem_info != kContResItemInfo.end())
	{
		int const iItemNo = (*resitem_info).first;
		kContHaveItem.insert( std::make_pair(iItemNo, pkInv->GetTotalCount(iItemNo)) );
		++resitem_info;
	}

	CONT_DEF_JOBSKILL_ITEM_UPGRADE::mapped_type kItemUpgrade;
	resitem_info = kContResItemInfo.begin();
	while(resitem_info != kContResItemInfo.end())
	{
		int const iItemNo = (*resitem_info).first;
		int const iCount = (*resitem_info).second;
		if(iCount < 0 )
		{
			return E_FAIL;
		}

		JobSkill_Util::GetJobSkill_Item(iItemNo, kItemUpgrade);
		for(int i=0; i<MAX_JS3_RECIPE_RES; ++i)
		{
			if( kRecipe.kResource[i].iCount>0
			&&	UseResItem(kRecipe.kResource[i].iGroupNo, kRecipe.kResource[i].iGrade, iItemNo, kItemUpgrade.iResourceGroupNo, kItemUpgrade.iGrade) )
			{
				kContHaveItem[iItemNo] -= iCount;
				kRecipe.kResource[i].iCount -= iCount;
				break;
			}
		}

		if(kContHaveItem[iItemNo] < 0)
		{
			break;
		}

		++resitem_info;
	}

	for(int i=0; i<MAX_JS3_RECIPE_RES; ++i)
	{
		if(0 != kRecipe.kResource[i].iCount)
		{
			return E_JS3_CREATEITEM_NOT_ENOUGH_NEED_ITEM;
		}
	}

	return S_OK;
}

HRESULT JobSkill_Third::CheckInvenSize(PgInventory * const pkInv, int const iRecipeItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo)
{
	if( !pkInv )
	{
		return E_FAIL;
	}

	int const iResourceProbabilityUp = GetResourceProbabilityUp(pkInv, kContResItemInfo);

	GET_DEF(CItemDefMgr, kItemDefMgr);

	CONT_JS3_RESULT_ITEM kContItem;
	bool bRemainResultItem = false;
	GetResProbabilityItem(iRecipeItemNo, iResourceProbabilityUp, kContItem, bRemainResultItem);

	SItemPos kNewPos;
	for(CONT_JS3_RESULT_ITEM::const_iterator it=kContItem.begin(); it!=kContItem.end(); ++it)
	{
		CItemDef const* pItemDef = kItemDefMgr.GetDef( (*it).second.iResultNo );
		if(pItemDef)
		{
			EInvType const eInvType = static_cast<EInvType>(pItemDef->PrimaryInvType());
			if(false==pkInv->GetFirstEmptyPos(eInvType, kNewPos))
			{
				return E_JS3_CREATEITEM_NOT_ENOUGH_INVENTORY;
			}
		}
	}

	return S_OK;
}

bool JobSkill_Third::GetJobSkill3_Recipe(int const iItemNo, CONT_DEF_JOBSKILL_RECIPE::mapped_type & rkOutRecipe)
{
	CONT_DEF_JOBSKILL_RECIPE const* pkDefJobSkillRecipe = NULL;
	g_kTblDataMgr.GetContDef(pkDefJobSkillRecipe);
	if( pkDefJobSkillRecipe )
	{
		CONT_DEF_JOBSKILL_RECIPE::const_iterator iter = pkDefJobSkillRecipe->find(iItemNo);
		if( iter != pkDefJobSkillRecipe->end() )
		{
			rkOutRecipe = (*iter).second;
			return true;
		}
	}
	return false;
}

HRESULT JobSkill_Third::GetTotalProbability(int const iRecipeItemNo)
{
	int const iNo = GetJobSkill3ProbabilityNo(iRecipeItemNo);

	CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkJobskillProbabilityBag = NULL;
	g_kTblDataMgr.GetContDef( pkJobskillProbabilityBag );
	if( pkJobskillProbabilityBag )
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator bag_it = pkJobskillProbabilityBag->find(iNo);
		if( bag_it != pkJobskillProbabilityBag->end() )
		{
			return (*bag_it).second.iTotalProbability;
		}
	}
	return 0;
}

HRESULT JobSkill_Third::GetResProbabilityItem(PgInventory *const pkInv, int const iItemNo, CONT_JS3_RESITEM_INFO const& kContResItemInfo, JobSkill_Third::CONT_JS3_RESULT_ITEM & rkContItem, bool & rbRemainResultItem)
{
	if( !pkInv )
	{
		return E_FAIL;
	}
	int iResourceProbabilityUp = GetResourceProbabilityUp(pkInv, kContResItemInfo);
	return GetResProbabilityItem(iItemNo, iResourceProbabilityUp, rkContItem, rbRemainResultItem);
}

HRESULT JobSkill_Third::GetResProbabilityItem(int const iItemNo, int const iResourceProbabilityUp, JobSkill_Third::CONT_JS3_RESULT_ITEM & rkContItem, bool & rbRemainResultItem)
{
	int iTempResourceProbabilityUp = iResourceProbabilityUp;
	int const iNo = GetJobSkill3ProbabilityNo(iItemNo);
	int const iTotalProbability = GetTotalProbability(iItemNo);

	CONT_DEF_JOBSKILL_PROBABILITY_BAG const* pkJobskillProbabilityBag = NULL;
	g_kTblDataMgr.GetContDef( pkJobskillProbabilityBag );
	if( pkJobskillProbabilityBag )
	{
		CONT_DEF_JOBSKILL_PROBABILITY_BAG::const_iterator bag_it = pkJobskillProbabilityBag->find(iNo);
		if( bag_it != pkJobskillProbabilityBag->end() )
		{
			CONT_DEF_JOBSKILL_PROBABILITY_BAG::mapped_type const& rkDefProbBag = (*bag_it).second;
			CONT_PROBABILITY::const_iterator c_it = rkDefProbBag.kContProbability.begin();
			while(c_it != rkDefProbBag.kContProbability.end())
			{
				rkContItem.push_back( std::make_pair((*c_it).iProbability,(*c_it)) );
				++c_it;
			}
		}
	}

	//재료 확률값 적용
	JobSkill_Third::CONT_JS3_RESULT_ITEM::value_type kValue;
	for(JobSkill_Third::CONT_JS3_RESULT_ITEM::iterator it=rkContItem.begin(); it!=rkContItem.end(); /*No ++it*/)
	{
		int iRate = (*it).first - iTempResourceProbabilityUp;
		iTempResourceProbabilityUp -= (*it).first;
		if(iRate > 0)
		{
			(*it).first = iRate;
			break;
		}
		else
		{
			kValue = (*it);
			it = rkContItem.erase(it);
		}
	}

	if(rkContItem.empty() && kValue.second.IsRight())
	{
		rkContItem.push_back(kValue);
	}

	//10000에 맞추기 : 넘는 값 제거
	int iTotalRate = 0;
	for(JobSkill_Third::CONT_JS3_RESULT_ITEM::iterator it=rkContItem.begin(); it!=rkContItem.end(); ++it)
	{
		iTotalRate += (*it).first;
		int const iOver = iTotalProbability-iTotalRate;
		if(iOver < 0)
		{
			(*it).first += iOver;
			
			//10000을 넘은 이후 목표아이템 제거
			if((*it).first != 0)
			{
				++it;
			}
			rbRemainResultItem = (it != rkContItem.end());
			rkContItem.erase(it,rkContItem.end());
			break;
		}
	}
	
	//10000에 맞추기 : 모자라는 값 채우기
	int const iIdx = rkContItem.size()-1;
	int const iMod = iTotalProbability-iTotalRate;
	if(iIdx>=0 && iMod>0)
	{
		rkContItem.at(iIdx).first += iMod;
	}

	return S_OK;
}