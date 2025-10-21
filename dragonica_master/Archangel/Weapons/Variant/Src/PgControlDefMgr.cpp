#include "StdAfx.h"
#include "PgControlDefMgr.h"

/////////////////////////////////////////////////////////////
// PgDefMgrHolder
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// PgControlDefMgr
/////////////////////////////////////////////////////////////
PgControlDefMgr::PgControlDefMgr(void)
{
}

PgControlDefMgr::~PgControlDefMgr(void)
{
	Release();
}

bool PgControlDefMgr::Update(SReloadDef& rkDefData, int iUpdateType)
{
	BM::CAutoMutex kWLock(m_kRwLock, true);

	bool bReturn = true;
	INFO_LOG(BM::LOG_LV7, __FL__<<L"Begin :: UpdateType["<<iUpdateType<<L"]");
	if (IsIncludeDefMgr(iUpdateType, EDef_MonsterDefMgr) 
		&& rkDefData.pContDefMonster != NULL 
		&& rkDefData.pContDefMonsterAbil != NULL 
		&& rkDefData.pkDefStrings != NULL)
	{
		bReturn = bReturn && m_kCMonsterDefMgr.Build(*rkDefData.pContDefMonster, *rkDefData.pContDefMonsterAbil, *rkDefData.pkDefStrings);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_SkillDefMgr) 
		&& rkDefData.pContDefSkill != NULL 
		&& rkDefData.pContDefSkillAbil != NULL)
	{
		bReturn = bReturn && m_kCSkillDefMgr.Build(*rkDefData.pContDefSkill, *rkDefData.pContDefSkillAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemDefMgr) 
		&& rkDefData.pContDefItem != NULL 
		&& rkDefData.pContDefItemAbil != NULL
		&& rkDefData.pkDefItemResConvert != NULL )
	{
		m_kCItemDefMgr.ContResConvert( rkDefData.pkDefItemResConvert );
		bReturn = bReturn && m_kCItemDefMgr.Build(*rkDefData.pContDefItem, *rkDefData.pContDefItemAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_RareDefMgr) 
		&& rkDefData.pContDefRare != NULL 
		&& rkDefData.pContDefItemAbil != NULL)
	{
		bReturn = bReturn && m_kCRareDefMgr.Build(*rkDefData.pContDefRare, *rkDefData.pContDefItemAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemEnchantDefMgr) 
		&& rkDefData.pContDefItemEnchant != NULL 
		&& rkDefData.pContDefItemAbil != NULL)
	{
		bReturn = bReturn && m_kCItemEnchantDefMgr.Build(*rkDefData.pContDefItemEnchant, *rkDefData.pContDefItemAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ClassDefMgr) 
		&& rkDefData.pkDefClass != NULL 
		&& rkDefData.pkDefClassAbil != NULL)
	{
		bReturn = bReturn && m_kPgClassDefMgr.Build(rkDefData.pkDefClass, rkDefData.pkDefClassAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemBagMgr) 
		&& rkDefData.pkItemBag != NULL
		&& rkDefData.pkItemBagElements != NULL
		&& rkDefData.pkSuccessRateControl != NULL
		&& rkDefData.pkCountControl != NULL
		&& rkDefData.pkMoneyControl != NULL
		&& rkDefData.pkItemBagGroup != NULL)
	{
		bReturn = bReturn && m_kCItemBagMgr.Build(*rkDefData.pkItemBagGroup, *rkDefData.pkItemBag, *rkDefData.pkItemBagElements, *rkDefData.pkSuccessRateControl, *rkDefData.pkCountControl,
			*rkDefData.pkMoneyControl, *rkDefData.pkItemContainer);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_StoreMgr) 
	&& rkDefData.pkShopInGame != NULL
	&& rkDefData.pkShopInEmporia != NULL
	&& rkDefData.pkShopInStock != NULL
	&& rkDefData.pkShopNpcGuid != NULL
	&& rkDefData.pkJobSkillShop != NULL)
	{
		bReturn = bReturn && m_kPgStoreMgr.Build(m_kStoreValueKey, *rkDefData.pkShopInEmporia, *rkDefData.pkShopInGame, *rkDefData.pkShopInStock, *rkDefData.pkJobSkillShop, *rkDefData.pkShopNpcGuid);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemOptionMgr) 
		&& rkDefData.pkItemOption != NULL)
	{
		bReturn = bReturn && m_kPgItemOptionMgr.Build(*rkDefData.pkItemOption);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_SpendMoneyMgr) 
		&& rkDefData.pkSpendMoney != NULL )
	{
		bReturn = bReturn && m_kPgDefSpendMoneyMgr.Build(rkDefData.pkSpendMoney);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemSetDefMgr) 
		&& rkDefData.pkItemSet != NULL
		&& rkDefData.pkItemOptionAbil != NULL)
	{
		bReturn = bReturn && m_kCItemSetDefMgr.Build(*rkDefData.pkItemSet, *rkDefData.pkItemOptionAbil, m_kCItemDefMgr);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_DynamicDefMgr) 
		&& rkDefData.pkDynamicAbilRate != NULL
		&& rkDefData.pkDynamicAbilRateBag != NULL)
	{
		bReturn = bReturn && m_kPgDynamicDefMgr.Build(*rkDefData.pkDynamicAbilRateBag, *rkDefData.pkDynamicAbilRate);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_EffectDefMgr) 
		&& rkDefData.pkContDefEffect != NULL
		&& rkDefData.pkContDefEffectAbil != NULL)
	{
		bReturn = bReturn && m_kCEffectDefMgr.Build( *rkDefData.pkContDefEffect, *rkDefData.pkContDefEffectAbil);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_ItemMakingDefMgr) 
		&& rkDefData.pkDefItemMaking != NULL
		&& rkDefData.pkDefResultControl != NULL)
	{
		bReturn = bReturn && m_kCItemMakingDefMgr.Build(rkDefData.pkDefItemMaking, rkDefData.pkDefResultControl);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_MonsterBag) 
		&& rkDefData.pkRegenPPoint != NULL
		&& rkDefData.pContDefMonsterBagControl != NULL
		&& rkDefData.pContDefMonsterBag != NULL
		&& rkDefData.pkMonsterBagElements != NULL)
	{
		bReturn = bReturn && m_kPgMonsterBag.Build(rkDefData.pkRegenPPoint, rkDefData.pContDefMonsterBagControl, rkDefData.pContDefMonsterBag,
			rkDefData.pkMonsterBagElements);

		if(true == bReturn 
			&& NULL != rkDefData.pkContMapItemBag)
		{
			bReturn = bReturn && (S_OK == m_kCItemDefMgr.BuildDropPos(m_kCMonsterDefMgr, m_kCItemBagMgr, m_kPgMonsterBag, *rkDefData.pkContMapItemBag));
		}

		if(true == bReturn
			&& NULL != rkDefData.pkMissionRoot
			&& NULL != rkDefData.pkMissionResult
			&& rkDefData.pkMissionCandi)
		{
			bReturn = bReturn && m_kCItemDefMgr.BuildMissionItemDropPos(*rkDefData.pkMissionRoot, *rkDefData.pkMissionResult, *rkDefData.pkMissionCandi,m_kCItemBagMgr);
		}
	}	

	if (IsIncludeDefMgr(iUpdateType, EDef_ObjectDefMgr)
		&&	rkDefData.pkDefObject != NULL
		&&	rkDefData.pkDefObjectAbil != NULL
		&&	rkDefData.pkDefStrings != NULL)
	{
		bReturn = bReturn && m_kPgObjectUnitDefMgr.Build( *rkDefData.pkDefObject, *rkDefData.pkDefObjectAbil, *rkDefData.pkDefStrings );

		if(true == bReturn
			&& NULL != rkDefData.pkRegenPPoint
			&& NULL != rkDefData.pkDefObjectBag
			&& NULL != rkDefData.pkDefObjectBagElements)
		{
			bReturn = bReturn && m_kCItemDefMgr.BuildObjectItemDropPos(*rkDefData.pkRegenPPoint, *rkDefData.pkDefObjectBag, *rkDefData.pkDefObjectBagElements, m_kCItemBagMgr, m_kPgObjectUnitDefMgr);
		}
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_PropertyMgr)
		&&	rkDefData.pkDefProperty != NULL)
	{
		bReturn = bReturn && m_kPgDefPropertyMgr.Build(*rkDefData.pkDefProperty);
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_GroundEffect)
		&& IsIncludeDefMgr(iUpdateType, EDef_OnlyFirst)
		&& rkDefData.pkDefMapEffect != NULL)
	{
		bReturn = bReturn && m_kPgGroundEffectMgr.Build( *rkDefData.pkDefMapEffect, *rkDefData.pkContMap );
	}

	if (IsIncludeDefMgr(iUpdateType, EDef_RecommendationItem)
		&& rkDefData.pkDefRecommendationItem
		)
	{
		bReturn = bReturn && m_kPgDefRecommendationItemMgr.Build(*rkDefData.pkDefRecommendationItem);
	}

	if (	IsIncludeDefMgr(iUpdateType, EDef_Pet)
		&&	rkDefData.pkDefClassPet 
		&&	rkDefData.pkDefClassPetLevel
		&&	rkDefData.pkDefClassPetSkill
		&&	rkDefData.pkDefClassPetAbil
		&&	rkDefData.pkDefPetHatch
		&&	rkDefData.pkDefPetBonusStatus
		)
	{
		bReturn = bReturn && m_kPgClassPetDefMgr.Build( *rkDefData.pkDefClassPet, *rkDefData.pkDefClassPetLevel, *rkDefData.pkDefClassPetSkill, *rkDefData.pkDefClassPetAbil, 80 );

		if ( rkDefData.pkDefPetHatch->size() && rkDefData.pkDefPetBonusStatus->size() )
		{
			bReturn = bReturn && m_kPgPetHatchMgr.Build( m_kPgClassPetDefMgr, *(rkDefData.pkDefPetHatch), *(rkDefData.pkDefPetBonusStatus) );
		}
	}

	if (	IsIncludeDefMgr(iUpdateType, EDef_MapDefMgr)
		&& rkDefData.pkDefMap
		&& rkDefData.pkDefMapAbil
		)
	{
		bReturn = bReturn && m_kPgDefMapMgr.Build(*(rkDefData.pkDefMap), *(rkDefData.pkDefMapAbil));
	}

	if (	IsIncludeDefMgr(iUpdateType, EDef_PremiumMgr)
		&& rkDefData.pkDefPremium
		&& rkDefData.pkDefPremiumType
		)
	{
		bReturn = bReturn && m_kPgDefPremiumMgr.Build(*(rkDefData.pkDefPremium), *(rkDefData.pkDefPremiumType));
	}

	INFO_LOG(BM::LOG_LV7, __FL__<<L"Finished...");
	return bReturn;
}

/*
template<typename T_MGR>
const T_MGR& PgControlDefMgr::GetCustomDef(EDefMgr_Flag eMgrFlag, T_MGR* pkNothing) const
{
	CONT_CUSTOM_MGR::const_iterator itor = m_kCustomMgr.find(eMgrFlag);
	if (itor != m_kCustomMgr.end())
	{
		const PgDefMgrHolder<T_MGR>* pkMgr = dynamic_cast<PgDefMgrHolder<T_MGR>*> itor->second;
		if (pkMgr != NULL)
		{
			return pkMgr->GetDef();
		}
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] Cannot Get Custeom Def Type[%d]"), __FUNCTIONW__);
	return NULL;
}


template<typename T_MGR>
void PgControlDefMgr::AddCustomDef(EDefMgr_Flag eType)
{
	BM::CAutoMutex kWLock(m_kRwLock, true);
	CONT_CUSTOM_MGR::iterator itor = m_kCustomMgr.find(eType);
	if (itor != m_kCustomMgr.end())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[%s] DefManager already exist Type[%d]"), __FUNCTIONW__, eType);
		return;
	}
	PgDefMgrBase* pkBase = new PgDefMgrHolder<T_MGR>(eType);
	m_kCustomMgr.insert(std::make_pair(eType, pkBase));
}
*/

void PgControlDefMgr::Release()
{
	BM::CAutoMutex kWLock(m_kRwLock, true);
	CONT_CUSTOM_MGR::iterator itor = m_kCustomMgr.begin();
	while (itor != m_kCustomMgr.end())
	{
		delete itor->second;
		++itor;
	}

	m_kCustomMgr.clear();
}



namespace ControlDefUtil
{
	int const iClientLoadDef = PgControlDefMgr::EDef_MonsterDefMgr
			| PgControlDefMgr::EDef_ItemDefMgr
			| PgControlDefMgr::EDef_SkillDefMgr
			| PgControlDefMgr::EDef_ClassDefMgr
			| PgControlDefMgr::EDef_RareDefMgr
			| PgControlDefMgr::EDef_ItemEnchantDefMgr
			| PgControlDefMgr::EDef_EffectDefMgr
			| PgControlDefMgr::EDef_ItemOptionMgr
			| PgControlDefMgr::EDef_MonsterBag
			| PgControlDefMgr::EDef_ItemMakingDefMgr
			| PgControlDefMgr::EDef_ItemBagMgr
			| PgControlDefMgr::EDef_ItemSetDefMgr
			| PgControlDefMgr::EDef_SpendMoneyMgr
			| PgControlDefMgr::EDef_ObjectDefMgr
			| PgControlDefMgr::EDef_PropertyMgr
			| PgControlDefMgr::EDef_RecommendationItem
			| PgControlDefMgr::EDef_Pet
			| PgControlDefMgr::EDef_Default
			| PgControlDefMgr::EDef_OnlyFirst
			| PgControlDefMgr::EDef_GenPointMgr
			| PgControlDefMgr::EDef_PremiumMgr
			| PgControlDefMgr::EDef_MapDefMgr;
}