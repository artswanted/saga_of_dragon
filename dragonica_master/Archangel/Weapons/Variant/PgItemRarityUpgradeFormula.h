#ifndef WEAPON_VARIANT_ITEM_PGITEMRARITYUPGRADEFORMULA_H
#define WEAPON_VARIANT_ITEM_PGITEMRARITYUPGRADEFORMULA_H

#include "BM/stlsupport.h"
#include "Lohengrin/dbtables.h"
#include "item.h"
#include "itemDefMgr.h"
#include "tabledatamanager.h"
#include "pgcontroldefmgr.h"

class CUnit;

class PgItemRarityUpgradeFormula
{
public:
	PgItemRarityUpgradeFormula();
	~PgItemRarityUpgradeFormula(){}

public:

// 인첸트 필요 재료 계산 함수들
	static bool		const GetNeedCrystalInfo(PgBase_Item const & kItem, int const iElement, int & iCrystalNo, int & iCrystalCount, bool bIsSecond = false);
	static int		const GetExceptionRate(E_ITEM_GRADE const eItemGrade, PgBase_Item const & kItem);
	static int		const GetNeedSoulCount(E_ITEM_GRADE const eItemGrade, PgBase_Item const & kItem,CUnit const * pkUnit, int const iAddDecSoulRate = 0);
	static __int64	const GetNeedEnchantCost(E_ITEM_GRADE const eItemGrade, PgBase_Item const & kItem);
	static int		const GetEnchantBonusRate(short const siCount);

//	
	static bool GetMaxGradeLevel( E_ITEM_GRADE const kItemGrade, bool const bIsPet, int &iOutLevel );

// 소을크래프트 필요 재료 계산 함수들
	static double	const GetRarityUpgradeCostRate(int iEquipPos);
	static int		const GetItemRarityContorolType(EItemModifyParentEventType const kCause);
	static int		const GetBrokenRate(E_ITEM_GRADE const eItemGrade);
	static __int64	const GetPlusUpgradeCost(PgBase_Item const & rkItem);


// 소울크래프트 옵션 증폭 필요 재료 계산 함수들
	static SDefBasicOptionAmp const * GetBasicOptionAmp(EEquipType const eEquipType, int const iLv, int const iTargetLv);
	static SDefBasicOptionAmp const * GetBasicOptionAmp(EEquipType const eEquipType, int const iLv, int const iTargetLv, CONT_DEFBASICOPTIONAMP const * pkDef);

	static void ApplyBasicOptionAmp(EEquipType const eEquipType, int const iLevel, int const iBasicAmpLv, CAbilObject & kOptBasicAbil, CONT_DEFBASICOPTIONAMP const * pkDefBasicOptionAmp=NULL);
	static int GetBasicOptionAmpRate(WORD const wType, EEquipType const eEquipType, int const iLevel, int const iBasicAmpLv);

	static SDefItemAmplify_Specific const * ItemAmplifyRateSpecific(int const iEquipType);
	static SDefItemAmplify_Specific const * ItemAmplifyRateSpecific(int const iEquipType, CONT_DEFITEM_AMP_SPECIFIC const * pkDef);

// 인첸트 전이
	static TBL_DEF_ITEMENCHANTSHIFT const * GetEnchantShiftPlusInfo(int const iItemNo, int const iLv, int const iTargetLv);
};

#endif // WEAPON_VARIANT_ITEM_PGITEMRARITYUPGRADEFORMULA_H