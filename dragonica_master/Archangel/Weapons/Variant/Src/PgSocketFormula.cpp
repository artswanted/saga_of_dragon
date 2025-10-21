#include "stdafx.h"
#include "Itemdefmgr.h"
#include "PgControlDefMgr.h"
#include "PgSocketFormula.h"
#include "PgItemRarityUpgradeFormula.h"
#include "lohengrin/variablecontainer.h"

 
int PgSocketFormula::GetMonsterCardSocketStateIndex(SEnchantInfo const kEhtInfo, int const iOrderIndex)
{
	switch( iOrderIndex )
	{
	case SII_FIRST:
		{
			if( (GSS_GEN_SUCCESS == kEhtInfo.GenSocketState()) && (0 == kEhtInfo.MonsterCard()) )
			{
				return SII_FIRST;
			}
		}break;
	case SII_SECOND:
		{
			if( (GSS_GEN_SUCCESS == kEhtInfo.GenSocketState2()) && (0 == kEhtInfo.MonsterCard2()) )
			{
				return SII_SECOND;
			}
		}break;
	case SII_THIRD:
		{
			if( (GSS_GEN_SUCCESS == kEhtInfo.GenSocketState3()) && (0 == kEhtInfo.MonsterCard3()) )
			{
				return SII_THIRD;
			}
		}break;
	default:
		{
		}break;
	}

	return SII_NONE;
}

int PgSocketFormula::GetEquipType(int const iItemNo)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const * pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( pkItemDef )
	{
		int const iEquipPos = pkItemDef->GetAbil(AT_EQUIP_LIMIT);
		if( EQUIP_WEAPON_TYPE & iEquipPos )
		{
			return static_cast<int>(EEQUIP_WEAPON);
		}
		else if( EQUIP_ARMOR_TYPE & iEquipPos )
		{
			return static_cast<int>(EEQUIP_ARMOR);
		}
		else if( EQUIP_ACC_TYPE & iEquipPos )
		{
			return static_cast<int>(EEQUIP_ACC);
		}
	}

	return static_cast<int>(EEQUIP_NONE);
}

int PgSocketFormula::GetCreateSocketRate(PgBase_Item const & kItem, int iSocket_Order)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pDef || (false == pDef->CanEquip()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM const * pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int iEquipPos = GetEquipType(kItem.ItemNo()); // 함수 사용
	int iLevelLimit = pDef->GetAbil(AT_LEVELLIMIT);

	CONT_DEF_SOCKET_ITEM::key_type kKey(iEquipPos, iLevelLimit, iSocket_Order);

	CONT_DEF_SOCKET_ITEM::const_iterator const_iter = pCont->find(kKey);
	if(const_iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM::mapped_type kValue = const_iter->second;

	return static_cast<__int64>(kValue.iSuccessRate);
}

__int64 PgSocketFormula::GetCreateNeedCost(PgBase_Item const & kItem, int iSocket_Order)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pDef || (false == pDef->CanEquip()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM const * pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int iEquipPos = GetEquipType(kItem.ItemNo()); // 함수 사용
	int iLevelLimit = pDef->GetAbil(AT_LEVELLIMIT);

	CONT_DEF_SOCKET_ITEM::key_type kKey(iEquipPos, iLevelLimit, iSocket_Order);

	CONT_DEF_SOCKET_ITEM::const_iterator const_iter = pCont->find(kKey);
	if(const_iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM::mapped_type kValue = const_iter->second;

	return static_cast<__int64>(kValue.iNeedMoney);

	//double const kRate = PgItemRarityUpgradeFormula::GetRarityUpgradeCostRate(pDef->EquipPos());
	//return static_cast<__int64>(pow(static_cast<double>(pDef->GetAbil(AT_LEVELLIMIT)),2) * 12 * kRate);
}

int PgSocketFormula::GetCreateNeedSoul(PgBase_Item const & kItem, int iSocket_Order)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM const * pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int iEquipPos = GetEquipType(kItem.ItemNo()); // 함수 사용
	int iLevelLimit = pDef->GetAbil(AT_LEVELLIMIT);

	CONT_DEF_SOCKET_ITEM::key_type kKey(iEquipPos, iLevelLimit, iSocket_Order);

	CONT_DEF_SOCKET_ITEM::const_iterator const_iter = pCont->find(kKey);
	if(const_iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM::mapped_type kValue = const_iter->second;

	return static_cast<int>(kValue.iSoulItemCount);

	//double const kRate = PgItemRarityUpgradeFormula::GetRarityUpgradeCostRate(pDef->EquipPos());
	//return static_cast<int>( pow( static_cast<double>(pDef->GetAbil(AT_LEVELLIMIT)), 1.2 ) * kRate );
}

__int64 PgSocketFormula::GetRemoveNeedCost(PgBase_Item const & kItem, int iSocket_Order)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pDef || (false == pDef->CanEquip()))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM const * pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int iEquipPos = GetEquipType(kItem.ItemNo()); // 함수 사용
	int iLevelLimit = pDef->GetAbil(AT_LEVELLIMIT);

	CONT_DEF_SOCKET_ITEM::key_type kKey(iEquipPos, iLevelLimit, iSocket_Order);

	CONT_DEF_SOCKET_ITEM::const_iterator const_iter = pCont->find(kKey);
	if(const_iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM::mapped_type kValue = const_iter->second;

	return static_cast<int>(kValue.iNeedMoney);

	//double const kRate = PgItemRarityUpgradeFormula::GetRarityUpgradeCostRate(pDef->EquipPos());
	//return static_cast<__int64>(pow(static_cast<double>(pDef->GetAbil(AT_LEVELLIMIT)),2) * 20 * kRate);
}

int PgSocketFormula::GetRemoveNeedSoul(PgBase_Item const & kItem, int iSocket_Order)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pDef = kItemDefMgr.GetDef(kItem.ItemNo());
	if(!pDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM const * pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	int iEquipPos = GetEquipType(kItem.ItemNo()); // 함수 사용
	int iLevelLimit = pDef->GetAbil(AT_LEVELLIMIT);

	CONT_DEF_SOCKET_ITEM::key_type kKey(iEquipPos, iLevelLimit, iSocket_Order);

	CONT_DEF_SOCKET_ITEM::const_iterator const_iter = pCont->find(kKey);
	if(const_iter == pCont->end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CONT_DEF_SOCKET_ITEM::mapped_type kValue = const_iter->second;

	return static_cast<int>(kValue.iSoulItemCount);

	//double const kRate = PgItemRarityUpgradeFormula::GetRarityUpgradeCostRate(pDef->EquipPos());
	//return static_cast<int>( pow(static_cast<double>(pDef->GetAbil(AT_LEVELLIMIT)),1.3) * kRate );
}

int PgSocketFormula::GetCreateSocketItemOrder(PgBase_Item const & kItem)
{
	if(!kItem.IsEmpty())
	{
		SEnchantInfo const& kEhtInfo = kItem.EnchantInfo();

		switch( kEhtInfo.GenSocketState() )
		{
		case GSS_GEN_NONE:
			{
				return 1;
			}break;
		case GSS_GEN_FAIL:
			{
				// 소켓을 추가로 뚫을 수 없다
				return 0;
			}break;
		default:
			{
			}break;
		}
		// 2, 3 번째 소켓을 검사
		switch( kEhtInfo.GenSocketState2() )
		{
		case GSS_GEN_NONE:
			{
				return 2;
			}break;
		case GSS_GEN_FAIL:
			{
				// 소켓을 추가로 뚫을 수 없다
				return 0;
			}break;
		default:
			{
			}break;
		}

		switch( kEhtInfo.GenSocketState3() )
		{
		case GSS_GEN_NONE:
			{
				return 3;
			}break;
		case GSS_GEN_FAIL:
		default:
			{
				// 소켓을 추가로 뚫을 수 없다
				return 0;
			}break;
		}
	}

	return 0;
}

bool PgSocketFormula::GetEnchantInfoIndex(PgBase_Item const & kItem, int const iSocketIndex, int& iGenSocketState, int& iGenSocketCard)
{
	if( kItem.IsEmpty() )
	{
		return false;
	}

	iGenSocketState = 0;
	iGenSocketCard = 0;

	SEnchantInfo const& kEhtInfo = kItem.EnchantInfo();

	switch( iSocketIndex )
	{
	case SII_FIRST:
		{
			iGenSocketState = kEhtInfo.GenSocketState();
			iGenSocketCard = kEhtInfo.MonsterCard();
		}break;
	case SII_SECOND:
		{
			iGenSocketState = kEhtInfo.GenSocketState2();
			iGenSocketCard = kEhtInfo.MonsterCard2();
		}break;
	case SII_THIRD:
		{
			iGenSocketState = kEhtInfo.GenSocketState3();
			iGenSocketCard = kEhtInfo.MonsterCard3();
		}break;
	default:
		{
			return false;
		}break;
	}
	return true;
}

int PgSocketFormula::GetRemoveSocketItemOrder(PgBase_Item const & kItem, int const iSocketIndex)	// 몬스터 카드 지우기
{
	if(!kItem.IsEmpty())
	{		
		int iGenSocketState = 0;
		int iGenSocketCard = 0;

		if( 0 == iSocketIndex )
		{
			for(int i=SII_FIRST; i<=SII_THIRD; ++i)
			{
				if( true == GetEnchantInfoIndex(kItem, i, iGenSocketState, iGenSocketCard) )
				{
					if( (GSS_GEN_SUCCESS == iGenSocketState) && (0 != iGenSocketCard) )
					{
						return i;
					}
				}			
			}
		}
		else
		{
			if( true == GetEnchantInfoIndex(kItem, iSocketIndex, iGenSocketState, iGenSocketCard) )
			{
				if( (GSS_GEN_SUCCESS == iGenSocketState) && (0 != iGenSocketCard) )
				{
					return iSocketIndex;
				}
			}
		}
	}

	return 0;
}

int PgSocketFormula::GetResetSocketItemOrder(PgBase_Item const & kItem)	// 막힌 소켓 뚫기
{
	if(!kItem.IsEmpty())
	{
		SEnchantInfo const& kEhtInfo = kItem.EnchantInfo();

		// 3, 2, 1 번째 소켓을 먼저 검사
		switch( kEhtInfo.GenSocketState3() )
		{
		case GSS_GEN_NONE:
			{
				// 다음 체크
			}break;
		case GSS_GEN_FAIL:
			{
				// 소켓을 리셋 해야 된다
				return 3;
			}break;
		default:
			{
			}break;
		}

		switch( kEhtInfo.GenSocketState2() )
		{
		case GSS_GEN_FAIL:
			{
				// 소켓을 리셋 해야 된다
				return 2;
			}break;
		default:
			{
			}break;
		}
		//

		switch( kEhtInfo.GenSocketState() )
		{
		case GSS_GEN_FAIL:
			{
				// 소켓을 리셋 해야 된다
				return 1;
			}break;
		default:
			{
			}break;
		}
	}

	return 0;
}

bool PgSocketFormula::GetExtractionItemCount(CUnit * pkPlayer, CItemDef const *pItemDef, int& iCashCount, int& iNeedCount)
{
	iCashCount = std::max(iCashCount,0);
	iNeedCount = 0;

	if(NULL == pItemDef)
	{
		return false;
	}

	int iInGameItemCount = pItemDef->GetAbil(AT_SOCET_CARD_EXTRACTION_ITEM_COUNT);
	iCashCount = std::min(iCashCount, pItemDef->GetAbil(AT_SOCET_CARD_EXTRACTION_CASH_ITEM_COUNT));

	int iCashItemCount = pItemDef->GetAbil(AT_SOCET_CARD_EXTRACTION_CASH_ITEM_COUNT);
	iNeedCount = (iInGameItemCount * (iCashItemCount - iCashCount)) / iCashItemCount;
	return true;
}