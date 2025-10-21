#ifndef WEAPON_VARIANT_PGHIDDENREWORDITEMMGR_H
#define WEAPON_VARIANT_PGHIDDENREWORDITEMMGR_H

#include "Lohengrin/packetstruct.h"
#include "BM/twrapper.h"
#include "unit.h"

typedef std::vector< PgBase_Item > RESULT_SITEM_BAG_LIST_CONT;	

class PgHiddenRewordItemMgrImpl
{
public:

	PgHiddenRewordItemMgrImpl(){}
	~PgHiddenRewordItemMgrImpl(){}

public:
	void Clear();
	void Init();

	int const GetMyItem_ViewCount(int const iLevel, int const iItemCount);
	int const GetMyItem_ViewItemNo(int const iLevel, int const iItemCount, bool bItemCount = false);
	bool GiveHiddenRewordItem(CONT_PLAYER_MODIFY_ORDER& kOrder, RESULT_SITEM_BAG_LIST_CONT& kContItem, CUnit* pkUnit, int const iItemNo, int const iItemNeedCount, int iNeedItemNo);
private:
};

class PgHiddenRewordItemMgr : public TWrapper<PgHiddenRewordItemMgrImpl>
{
public:
	PgHiddenRewordItemMgr(){Clear();}
	~PgHiddenRewordItemMgr(){}

	void Clear();
	void Init();
	int const GetMyItem_ViewCount(int const iLevel, int const iItemCount);
	int const GetMyItem_ViewItemNo(int const iLevel, int const iItemCount, bool bItemCount = false);
	bool GiveHiddenRewordItem(CONT_PLAYER_MODIFY_ORDER& kOrder, RESULT_SITEM_BAG_LIST_CONT& kContItem, CUnit* pkUnit, int const iItemNo, int const iItemNeedCount, int iNeedItemNo);
};

#endif // WEAPON_VARIANT_PGHIDDENREWORDITEMMGR_H