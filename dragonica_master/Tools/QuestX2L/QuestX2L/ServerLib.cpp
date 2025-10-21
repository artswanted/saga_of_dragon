#include "stdafx.h"
#include "ServerLib.h"
#include "Variant/MonsterDefMgr.h"
#include "Variant/PetDefMgr.h"
//#include "PgUIScene.h"
#include "Variant/PgClassDefMgr.h"
#include "Variant/ItemDefMgr.h"
#include "Variant/PgItemOptionMgr.h"
#include "Variant/PgMonsterBag.h"
#include "Variant/ItemBagMgr.h"
#include "Variant/ItemMakingDefMgr.h"
//#include "PgPvP.h"

//CMonsterDefMgr g_MonsterMgr;

///*
const CONT_DEFABILTYPE* g_pAbilType = NULL;
const CONT_DEFSKILL	*g_pDefSkill = NULL;
const CONT_DEFUPGRADECLASS	*g_pDefUpgradeClass = NULL;

bool LoadTBData(const std::wstring &strFolder)
{
	const bool bRet = g_kTblDataMgr.LoadDump(strFolder);

	if( bRet )
	{
		TB_REF_COUNT tbRef;

		const CONT_DEFABILTYPE* pContAbilType = NULL;
		const CONT_DEFMONSTER* pContDefMonster = NULL;
		const CONT_DEFMONSTERABIL* pContDefMonsterAbil = NULL;
		const CONT_DEFSKILL* pContDefSkill = NULL;
		const CONT_DEFSTRINGS* pContDefStrings = NULL;

		const CONT_DEFPET* pkDefPet = NULL;
		const CONT_DEFPETABIL* pkDefPetAbil = NULL;
		const CONT_DEFCLASS2* pkDefClass2 = NULL;
		const CONT_DEFCLASS2ABIL* pkDefClass2Abil = NULL;
		const CONT_DEFITEMENCHANT* pContDefItemEnchant = NULL;

		const CONT_DEFUPGRADECLASS	*pContDefUpgradeClass = NULL;
		const CONT_DEFITEMRARE* pContDefRare = NULL;
		const CONT_DEFEFFECT* pContEffect = NULL;
		const CONT_DEFEFFECTABIL* pContEffectAbil = NULL;
		const CONT_DEFSKILLABIL* pContDefSkillAbil = NULL;

		const CONT_DEFITEM* pContDefItem = NULL;
		const CONT_DEFITEMABIL* pContDefItemAbil = NULL;
		const CONT_DEFRES* pContDefRes = NULL;

		const CONT_DEF_ITEM_BAG* pkItemBag = NULL;
		const CONT_DEF_ITEM_BAG_ELEMENTS* pkBagElements = NULL;
		const CONT_DEF_COUNT_CONTROL* pkCountControl = NULL;
		const CONT_DEF_DROP_MONEY_CONTROL* pkMoneyControl = NULL;
		const CONT_DEF_SUCCESS_RATE_CONTROL* pkSuccessRateControl = NULL;

		const CONT_TBL_DEF_ITEM_OPTION *pkItemOption = NULL;
		const CONT_TBL_DEF_ITEM_OPTION_ABIL *pkItemOptionAbil = NULL;
		const CONT_TBL_DEF_ITEM_OPTION_CANDIDATE *pkItemOptionCandidate = NULL;
		const CONT_TBL_DEF_ITEM_OPTION_SELECTOR *pkItemOptionSelector = NULL;

		const CONT_DEF_MAP_REGEN_POINT *pkMapRegenPoint = NULL;
		const CONT_DEF_MONSTER_BAG_CONTROL *pkMonsterBagControl = NULL;
		const CONT_DEF_MONSTER_BAG *pkMonsterBag = NULL;
		const CONT_DEF_MONSTER_BAG_ELEMENTS *pkMonsterBagElements = NULL;
		const CONT_DEF_PVP_GROUNDMODE *pkPvPGroundMode = NULL;

		const CONT_DEFITEMMAKING *pkDefItemMaking = NULL;
		const CONT_DEFRESULT_CONTROL *pkDefResultControl = NULL;
		const CONT_DEF_MISSION_ROOT *pkDefMissionRoot = NULL;
		const CONT_DEF_MISSION_CANDIDATE *pkDefMissionCandidate = NULL;

		g_kTblDataMgr.GetContDef(tbRef,pContAbilType);
		g_kTblDataMgr.GetContDef(tbRef,pContDefMonster);
		g_kTblDataMgr.GetContDef(tbRef,pContDefMonsterAbil);
		g_kTblDataMgr.GetContDef(tbRef,pContDefSkill);
		g_kTblDataMgr.GetContDef(tbRef,pContDefSkillAbil);

		g_kTblDataMgr.GetContDef(tbRef,pContDefItem);
		g_kTblDataMgr.GetContDef(tbRef,pContDefItemAbil);
		g_kTblDataMgr.GetContDef(tbRef,pContDefRes);
		g_kTblDataMgr.GetContDef(tbRef,pContDefStrings);
		g_kTblDataMgr.GetContDef(tbRef,pkDefPet);

		g_kTblDataMgr.GetContDef(tbRef,pkDefPetAbil);
		g_kTblDataMgr.GetContDef(tbRef,pkDefClass2);
		g_kTblDataMgr.GetContDef(tbRef,pkDefClass2Abil);
		g_kTblDataMgr.GetContDef(tbRef,pContDefItemEnchant);
		g_kTblDataMgr.GetContDef(tbRef,pContDefUpgradeClass);

		g_kTblDataMgr.GetContDef(tbRef,pContDefRare);
		g_kTblDataMgr.GetContDef(tbRef,pContEffect);
		g_kTblDataMgr.GetContDef(tbRef,pContEffectAbil);

		g_kTblDataMgr.GetContDef(tbRef, pkItemBag);
		g_kTblDataMgr.GetContDef(tbRef, pkBagElements);
		g_kTblDataMgr.GetContDef(tbRef, pkCountControl);
		g_kTblDataMgr.GetContDef(tbRef, pkMoneyControl);
		g_kTblDataMgr.GetContDef(tbRef, pkSuccessRateControl);

		g_kTblDataMgr.GetContDef(tbRef, pkItemOption);
		g_kTblDataMgr.GetContDef(tbRef, pkItemOptionAbil);
		g_kTblDataMgr.GetContDef(tbRef, pkItemOptionCandidate);
		g_kTblDataMgr.GetContDef(tbRef, pkItemOptionSelector);

		g_kTblDataMgr.GetContDef(tbRef, pkItemOptionAbil);
		g_kTblDataMgr.GetContDef(tbRef, pkItemOptionCandidate);

		g_kTblDataMgr.GetContDef(tbRef, pkMapRegenPoint);
		g_kTblDataMgr.GetContDef(tbRef, pkMonsterBagControl);
		g_kTblDataMgr.GetContDef(tbRef, pkMonsterBag);
		g_kTblDataMgr.GetContDef(tbRef, pkMonsterBagElements);
		g_kTblDataMgr.GetContDef(tbRef, pkPvPGroundMode);

		g_kTblDataMgr.GetContDef(tbRef, pkDefItemMaking);
		g_kTblDataMgr.GetContDef(tbRef, pkDefResultControl);
		g_kTblDataMgr.GetContDef(tbRef, pkDefMissionRoot);
		g_kTblDataMgr.GetContDef(tbRef, pkDefMissionCandidate);

		g_pAbilType = pContAbilType;
		g_pDefSkill = pContDefSkill;
		g_pDefUpgradeClass = pContDefUpgradeClass;

		g_MonsterDef.Build(*pContDefMonster, *pContDefMonsterAbil);//빌드
		g_kItemDefMgr.Build(*pContDefItem, *pContDefItemAbil);//빌드
		g_SkillDefMgr.Build(*pContDefSkill, *pContDefSkillAbil);//빌드
		g_ClassDef.Build(pkDefClass2, pkDefClass2Abil);
		g_PetDef.Build(pkDefPet, pkDefPetAbil);
		g_RareDefMgr.Build( *pContDefRare, *pContDefItemAbil );

		g_ItemEnchantDefMgr.Build( *pContDefItemEnchant, *pContDefItemAbil );
		g_kEffectDefMgr.Build(*pContEffect, *pContEffectAbil);

		g_kItemOptMgr.Build(pkItemOption, pkItemOptionAbil, pkItemOptionCandidate, pkItemOptionSelector, pkSuccessRateControl);

		g_MonsterBag.Build(pkMapRegenPoint, pkMonsterBagControl, pkMonsterBag, pkMonsterBagElements);
		//lwPvP::Build(pkPvPGroundMode);

		g_ItemMakingDefMgr.Build(pkDefItemMaking, pkDefResultControl);// ItemMaking Build
		g_kItemBagMgr.Build(*pkItemBag, *pkBagElements, *pkSuccessRateControl, *pkCountControl, *pkMoneyControl, NULL );

		//CONT_DEFRES::const_iterator res_itr = pContDefRes->begin();
		//float fTime = NiGetCurrentTimeInSec();
		//while(res_itr != pContDefRes->end())
		//{
		//	SUVInfo kUVInfo( (*res_itr).second.U, (*res_itr).second.V, (*res_itr).second.UVIndex);
		//	g_kUIScene.AddIconRsc( (*res_itr).second.ResNo, (*res_itr).second.strIconPath, kUVInfo);
		//	PgXmlLoader::PrepareXmlDocument(MB((*res_itr).second.strXmlPath));
		//	++res_itr;
		//}
		//fTime = NiGetCurrentTimeInSec() - fTime;
		//NILOG(PGLOG_STAT, "AddIconRsc %f time \n", fTime);
	}

	return bRet;
}
//*/
//
//bool GetAbilName(const WORD AbilType, const wchar_t *&pString)
//{
//	if(!g_pAbilType)
//	{
//		assert(g_pAbilType);
//		return false;
//	}
//
//	CONT_DEFABILTYPE::const_iterator itor = g_pAbilType->find(AbilType);
//	if(itor != g_pAbilType->end())
//	{
//		return GetDefString((*itor).second.NameNo, pString);
//	}
////	assert(NULL && "Can't Find Abil Type");
//	return false;
//}
//bool GetEffectName(const DWORD dwEffectID, const wchar_t *&pString)
//{
//	const	CEffectDef	*pkEffectDef = g_kEffectDefMgr.GetDef(dwEffectID);
//	if(!pkEffectDef) return false;
//
//	return	GetDefString(pkEffectDef->m_iName, pString);
//}
//
//bool GetItemName(const DWORD dwItemNo, const wchar_t *&pString)
//{
//	const CItemDef *pkItemDef = g_kItemDefMgr.GetDef(dwItemNo);
//
//	if(pkItemDef)
//	{
//		return GetDefString(pkItemDef->NameNo(), pString);
//	}
//	return false;
//}
//
//
//int	GetClassUpgradeOrder(int iLastClassID,int *pkout_UpgradeOrderArray)
//{
//	int	iOrderCount = 0;
//
//	TBL_DEF_EFFGRADECLASS	*pUpgradeClassData = NULL;
//	for(int i=0;i<10;i++)
//	{
//		for(CONT_DEFUPGRADECLASS::const_iterator itor = g_pDefUpgradeClass->begin(); itor != g_pDefUpgradeClass->end();itor++)
//		{
//			pUpgradeClassData = (TBL_DEF_EFFGRADECLASS*)&(itor->second);
//			if(pUpgradeClassData->iClass == iLastClassID)
//			{
//				if(pkout_UpgradeOrderArray)
//					*(pkout_UpgradeOrderArray+pUpgradeClassData->byGrade) = iLastClassID;
//				if(pUpgradeClassData->byGrade == 0) return	iOrderCount;
//
//				iLastClassID = pUpgradeClassData->iParentClass;
//				iOrderCount++;
//				break;
//			}
//		}
//	}
//	return	iOrderCount;	//	전직 횟수
//}
//
//bool GetDefString(const int iTextNo, const wchar_t *&pString)
//{
//	TB_REF_COUNT tbRef;
//	const CONT_DEFSTRINGS* pContDefStrings = NULL;
//	g_kTblDataMgr.GetContDef(tbRef, pContDefStrings);
//
//	CONT_DEFSTRINGS::const_iterator str_itor = pContDefStrings->find(iTextNo);
//	if(str_itor != pContDefStrings->end())
//	{
//		pString = (*str_itor).second.strText.c_str();
////		--> 아래같은 코드가 어딧냐? 임시로 돌아가는거 모르나?
////		static std::wstring wstrCantFind = "Can't Find String"; 
////		wstrCantFind =  (std::wstring)( (std::wstring)_T("StringNo:") + (const std::wstring)(BM::vstring)iTextNo);
////		pString = wstrCantFind;
////		assert(str_itor != g_pDefStrings->end());
//		return true;
//	}
//	return false;
//}