#ifndef WEAPON_VARIANT_ITEM_ITEMBAGMGR_H
#define WEAPON_VARIANT_ITEM_ITEMBAGMGR_H

#include "ItemBag.h"

class CItemBagMgr
{
private:
	class PgBagByLevelFinder
	{
	public:
		PgBagByLevelFinder( short const nLevel ):m_nLevel(nLevel){}
		~PgBagByLevelFinder(){}

		bool operator > ( PgItemBagByLevel const &rhs )const
		{
			return m_nLevel > rhs.GetLevel();
		}

		bool operator < ( PgItemBagByLevel const &rhs )const
		{
			return m_nLevel < rhs.GetLevel();
		}

	private:
		short const m_nLevel;
	};

	typedef BM::PgApproximateVector< PgItemBagByLevel, std::less< PgItemBagByLevel > >	T_BAG_ELEMENT;
	typedef std::map< int, T_BAG_ELEMENT >												CONT_ITEM_BAG;//BagNo

public:
	CItemBagMgr();
	virtual ~CItemBagMgr();
	
	void swap(CItemBagMgr &rhs);

	virtual void Clear();

	bool Build(
		CONT_DEF_ITEM_BAG_GROUP const &rkItemBagGroup,
		CONT_DEF_ITEM_BAG const &rkDefItemBag,
		CONT_DEF_ITEM_BAG_ELEMENTS const &rkBagElements,
		CONT_DEF_SUCCESS_RATE_CONTROL const &rkRaiseControl,
		CONT_DEF_COUNT_CONTROL const &rkCountControl,
		CONT_DEF_DROP_MONEY_CONTROL const &rkMoneyControl,
		CONT_DEF_ITEM_CONTAINER const &rkContItemContainer);

	HRESULT GetItemBagByGrp(int const iBagGroupNo, short nLevel, PgItemBag &rkItemBag)const;
	HRESULT GetItemBagNoByGrp(int const iBagGroupNo, short nLevel, int& riOutItemBagNo)const;
	HRESULT GetItemBag(int const iBagNo, short nLevel, PgItemBag &rkItemBag)const;
	
	HRESULT GetItemBagElements(int const iBagNo, TBL_DEF_ITEM_BAG_ELEMENTS &rkItemBagElements)const;
	HRESULT GetSuccesRateControl(int const iNo, TBL_DEF_SUCCESS_RATE_CONTROL &rkSuccesRateControl)const;
	HRESULT GetCountControl(int const iNo, TBL_DEF_COUNT_CONTROL &rkCountControl)const;
	HRESULT EnumerateItemBag(int const iBagNo,CONT_SET_DATA & kCont)const;
	HRESULT EnumerateItemBagGroup(int const iBagGroupNo,CONT_SET_DATA & kCont)const;
	HRESULT EnumerateItemContainer(int const iContainerNo,CONT_SET_DATA & kCont)const;

protected:
	CONT_ITEM_BAG m_kItemBag;//빌드된 백.
	
	CONT_DEF_ITEM_BAG_GROUP m_kContDefItemBagGroup;
	CONT_DEF_ITEM_BAG m_kContDefItemBag;
	CONT_DEF_ITEM_BAG_ELEMENTS m_kContDefBagElements;
	CONT_DEF_SUCCESS_RATE_CONTROL m_kContDefRaiseControl;
	CONT_DEF_COUNT_CONTROL m_kContDefCountControl;
	CONT_DEF_DROP_MONEY_CONTROL m_kMoneyControl;
	CONT_DEF_ITEM_CONTAINER m_kContDefItemContainer;
};

//#define g_kItemBagMgr SINGLETON_STATIC(CItemBagMgr)

#endif // WEAPON_VARIANT_ITEM_ITEMBAGMGR_H