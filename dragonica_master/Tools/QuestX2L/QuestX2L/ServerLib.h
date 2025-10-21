#pragma once

#include "Variant/MonsterDefMgr.h"
#include "Variant/DefAbilType.h"
#include "Variant/PetDefMgr.h"
#include "Variant/TableDataManager.h"

const extern	CONT_DEFSKILL	*g_pDefSkill;
const extern	CONT_DEFUPGRADECLASS	*g_pDefUpgradeClass;

//extern CMonsterDefMgr g_MonsterMgr;
extern bool LoadTBData(const std::wstring &strFolder);
//extern bool GetAbilName(const WORD AbilType, const wchar_t *&pString);
//extern bool GetItemName(const DWORD dwItemNo, const wchar_t *&pString);
//extern bool GetEffectName(const DWORD dwEffectID, const wchar_t *&pString);
//extern bool GetDefString(const int iTextNo, const wchar_t *&pString);
//
//extern	int	GetClassUpgradeOrder(int iLastClassID,int *pkout_UpgradeOrderArray);