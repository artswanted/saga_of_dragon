#ifndef MAP_MAPSERVER_OBJECT_ITEM_PGITEMTRADEMGR_H
#define MAP_MAPSERVER_OBJECT_ITEM_PGITEMTRADEMGR_H

#include "variant/item.h"
#include "BM/PgTask.h"

typedef std::map<BM::GUID, STradeInfo*> TradeItemCont;
typedef BM::TObjectPool< STradeInfo >	ITEM_TRADE_POOL;

typedef std::map<const BM::GUID, const STradeInfo*> TradeItemContConst;

typedef std::map<const BM::GUID, DWORD const> TradeTimeCont;

class CItemTradeMgr
{
public:
	CItemTradeMgr();
	virtual ~CItemTradeMgr();
	void Init();
	void Clear();

	const BM::GUID NewItemTrade(BM::GUID const &kFrom, BM::GUID const &kTo);
	bool DelItemTrade(BM::GUID const &kTradeGuid);
	bool DelItemTradeByChar(BM::GUID const &kCharGuid);
	bool FindTrade(BM::GUID const &kGuid, STradeInfo &rkDest) const;
	bool FindTradeByChar(BM::GUID const &kGuid, STradeInfo &rkDest) const;

	bool ModifyTrade(const STradeInfo &rkDest);

	DWORD const FineTradeEndTime(const BM::GUID kCharacterGuid) const;

	void Tick(DWORD const dwTime);

	ETRADERESULT CheckTradeItem(CONT_TRADE_ITEM const &rkList, CUnit* pkUnit) const;

private:
	mutable Loki::Mutex		m_kLock;
	TradeItemCont			m_kTradeItemCont;
	TradeItemContConst		m_kTradeItemContByChar;
	ITEM_TRADE_POOL			m_kItemTradePool;
	TradeTimeCont			m_kTradeTimeCont;
};

#define g_kItemTradeMgr SINGLETON_STATIC(CItemTradeMgr)

#endif // MAP_MAPSERVER_OBJECT_ITEM_PGITEMTRADEMGR_H