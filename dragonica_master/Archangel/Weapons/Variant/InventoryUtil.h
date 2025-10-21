#ifndef WEAPON_VARIANT_USERINFO_INVENTORYUTIL_H
#define WEAPON_VARIANT_USERINFO_INVENTORYUTIL_H

namespace PgInventoryUtil
{
	// Util Class
	class PgFitItem
	{
		typedef PgInventory::ItemContainer ItemContainer;
	public:
		PgFitItem(ItemContainer const& rkCont);
		~PgFitItem();

	private:
		CLASS_DECLARATION_S_NO_SET(CONT_HAVE_ITEM_DATA, FitItem);
	};

	// Util Function
	void GetOverSkill(CONT_SKILL_OPT & rkOverSkillOpt, int const iOverSkillNo, int const iAddLevel);
	HRESULT ApplyAbil(CAbilObject* pkTarget, PgInventory::ItemContainer const* pCont, PgInventory::CONT_SETITEM_NO& rkFullSetCont, CONT_SKILL_OPT& rkOverSkillOpt, 
		PgInventory::CONT_ITEMACTIONEFFECT & rkItemActionEffect, CUnit const * pkUnit, CAbilObject*  pkSaveSubtractAbil=NULL);

	bool FilterPercentAbil(WORD const wAbilType);

	bool CheckDisableItem(PgBase_Item const & kItem,CUnit const * pkUnit);	
};

#endif // WEAPON_VARIANT_USERINFO_INVENTORYUTIL_H