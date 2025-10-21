#ifndef WEAPON_VARIANT_ITEM_PGSTORE_H
#define WEAPON_VARIANT_ITEM_PGSTORE_H

#include "constant.h"
#include "Lohengrin/dbtables.h"

#include "item.h"
#include "ItemDefMgr.h"

typedef enum 
{
	STORE_TYPE_DEFAULT	= 0,
	STORE_TYPE_STOCK	= 1,	// 재고?? 주식?
	STORE_TYPE_GAMBLE	= 2,	
    STORE_TYPE_EMPORIA	= 3,
	STORE_TYPE_JOBKSILL	= 4,
}E_STORETYPE;

class PgStore
{
public:
	typedef TBL_SHOP_IN_GAME GOODS;
	typedef std::list< GOODS > CONT_GODDS;
	typedef std::vector< PgBase_Item > CONT_GODDS_VIEW;

public:	
	explicit PgStore(BYTE const byType = STORE_TYPE_DEFAULT);
	virtual ~PgStore();

	PgStore& operator = (PgStore const &rhs);

	void WriteToPacket(BM::Stream &kPacket)const;
	void ReadFromPacket(BM::Stream &kPacket);
	
	PgBase_Item const* GetViewItem(int const iIDX)const;
	HRESULT GetGoods(int const iItemNo, CONT_GODDS::value_type &rkOutGoods)const;
	HRESULT GetGoodsToIdx(int const Idx, CONT_GODDS::value_type &rkOutGoods)const;
	HRESULT AddGoods(const TBL_SHOP_IN_GAME &rkTbl);//테이블 데이터 넣는거.
	void BuildByView(int const iView, __int64 const iOrderBy = __int64(0));
	void BuildByJobSkill(int const iMain, int const iSub);
	void BuildAll();
	int GetGoodsViewSize() const;
	void Init(BM::GUID const &kShopGuid);
	void swap(PgStore &rRight);
	int GetGoodsCountByView(int const iView) const;
	bool IsHaveCPItem();	//CP로 판매되는 아이템을 들고 있냐?
	bool IsHaveCoinItem();
	BM::GUID ShopGuid(){return m_kShopGuid;}
	BYTE GetType()const{return m_byType;}
	
protected:	
	mutable Loki::Mutex m_kMutex;

	CONT_GODDS m_kGoods;
	CONT_GODDS_VIEW m_kGoodsView;
	BM::GUID m_kShopGuid;
	BYTE	m_byType;//E_STORETYPE 사용
};

#endif // WEAPON_VARIANT_ITEM_PGSTORE_H