#ifndef MAP_MAPSERVER_ACTION_ACTION_PGACTIONUSERMARKET_H
#define MAP_MAPSERVER_ACTION_ACTION_PGACTIONUSERMARKET_H

__int64 const MARKET_GAME_COST_MAX = 99999999999;
int const MARKET_CASH_COST_MAX = 10000000;

class PgUMReqArticleReg
	:	public PgUtilAction
{
public:
	
	PgUMReqArticleReg(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqArticleReg(){}

public:

	EUserMarketResult	OnProcess(CUnit* pkCaster);
	virtual bool		DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqArticleDereg
	:	public PgUtilAction
{
public:
	
	PgUMReqArticleDereg(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqArticleDereg(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqArticleQuery
	:	public PgUtilAction
{
public:
	
	PgUMReqArticleQuery(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqArticleQuery(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqMyMarketQuery
	:	public PgUtilAction
{
public:
	
	PgUMReqMyMarketQuery(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqMyMarketQuery(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqMyVendorQuery
	:	public PgUtilAction
{
public:
	
	PgUMReqMyVendorQuery(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqMyVendorQuery(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqVendorEnter
	:	public PgUtilAction
{
public:
	
	PgUMReqVendorEnter(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqVendorEnter(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqArticleBuy
	:	public PgUtilAction
{
public:
	
	PgUMReqArticleBuy(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqArticleBuy(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqDealingsRead
	:	public PgUtilAction
{
public:
	
	PgUMReqDealingsRead(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqDealingsRead(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqQueryMinimumCost
	:	public PgUtilAction
{
public:
	
	PgUMReqQueryMinimumCost(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqQueryMinimumCost(){}

public:

	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqMarketOpen
	:	public PgUtilAction
{
public:
	
	PgUMReqMarketOpen(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqMarketOpen(){}

public:

	EUserMarketResult Process(CUnit* pkCaster);
	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	SGroundKey const m_kGndKey;
	BM::Stream & m_krPacket;
};

class PgUMReqUseMarketModifyItem
	:	public PgUtilAction
{
public:
	
	PgUMReqUseMarketModifyItem(SGroundKey const & kGndKey, BM::Stream & kPacket);
	virtual ~PgUMReqUseMarketModifyItem(){}

public:

	EUserMarketResult Process(CUnit* pkCaster);
	virtual bool DoAction(CUnit* pkCaster,CUnit* pkNoting);

private:

	BM::Stream & m_rkPacket;
	SGroundKey const m_kGndKey;
};

#endif // MAP_MAPSERVER_ACTION_ACTION_PGACTIONUSERMARKET_H