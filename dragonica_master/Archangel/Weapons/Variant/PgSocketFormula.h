#ifndef WEAPON_VARIANT_PGSOCKETFORMULA_H
#define WEAPON_VARIANT_PGSOCKETFORMULA_H

class PgSocketFormula
{
public:
	typedef enum eSocketItemIndex
	{
		SII_NONE = 0,			// 소켓이 아무것도 안되어져 있는 상태
		SII_FIRST,
		SII_SECOND,
		SII_THIRD,
		SII_FOURTH,
	}ESocketItemIndex;

	PgSocketFormula(){}
	virtual ~PgSocketFormula(){}
public:
	static int GetMonsterCardSocketStateIndex(SEnchantInfo const kEhtInfo, int const iOrderIndex);
	static int GetEquipType(int const iItemNo);
	static int GetCreateSocketRate(PgBase_Item const & kItem, int iSocket_Order);
	static __int64 GetCreateNeedCost(PgBase_Item const & kItem, int iSocket_Order);
	static int GetCreateNeedSoul(PgBase_Item const & kItem, int iSocket_Order);
	static __int64 GetRemoveNeedCost(PgBase_Item const & kItem, int iSocket_Order);
	static int GetRemoveNeedSoul(PgBase_Item const & kItem, int iSocket_Order);
	static int GetCreateSocketItemOrder(PgBase_Item const & kItem);
	static int GetRemoveSocketItemOrder(PgBase_Item const & kItem, int const iSocketIndex);
	static int GetResetSocketItemOrder(PgBase_Item const & kItem);
	static bool GetEnchantInfoIndex(PgBase_Item const & kItem, int const iSocketIndex, int& iGenSocketState, int& iGenSocketCard);

	//소켓추출
	static bool GetExtractionItemCount(CUnit * pkPlayer, CItemDef const *pItemDef, int& iCashCount, int& iNeedCount);
};

#endif // WEAPON_VARIANT_PGSOCKETFORMULA_H