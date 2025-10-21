#ifndef CONTENTS_CONTENTSSERVER_SERVERSET_TRANSACTION_H
#define CONTENTS_CONTENTSSERVER_SERVERSET_TRANSACTION_H

#include "AilePack/constant.h"
#include "AilePack/Gala_constant.h"

typedef enum
{
	ECASH_TRAN_NONE = 0,
	ECASH_TRAN_SEND_GIFT = 1,
	ECASH_TRAN_EXPAND_TIMELIMIT = 2,
	ECASH_TRAN_CASHSHOP_BUY = 3,
	ECASH_TRAN_CASHSHOP_ENTER = 4,
	ECASH_TRAN_OPENMARKET_ENTER = 5,
	ECASH_TRAN_OPENMARKET_BUY = 6,
	ECASH_TRAN_ADD_CASH = 7,
	ECASH_TRAN_OPENMARKET_SELLCASH = 8,		// OpenMarket 에 Cash 등록
	ECASH_TRAN_OPENMARKET_UNREGCASH = 9,	// OpenMarket 에 등록된 Cash 등록 해지
	//ECASH_TRAN_OPENMARKET_PAYBACK = 10,		// OpenMarket 에서 팔린 Money -> Cash로 수입얻기
	ECASH_TRAN_OPENMARKET_BUYCASH = 11,		// OpenMarket 에서 Cash 구매
} ECASH_TRAN_TYPE;

typedef enum : BYTE
{
	ECASH_LOG_STATE_CASHUSE = 0,	// Cash decrease
	ECASH_LOG_STATE_CASHADD = 1,		// Cash increase
	ECASH_LOG_STATE_ROLLBACK = 2,	// Cash rollback
} ECASH_LOG_STATE;

class PgTransaction
{
public:
	explicit PgTransaction(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgTransaction(BM::Stream & _Packet);
	PgTransaction();
	virtual ~PgTransaction();

	PgTransaction const& operator=(PgTransaction const & rhs);

public:

	typedef enum
	{	// bit mask
		ETranState_None				= 0x0000,
		ETranState_CashModified		= 0x0001,
		ETranState_TranSuccess		= 0x0002,
		ETranState_CommitTry		= 0x0100,
		ETranState_Commit_End		= 0x0200,
		ETranState_Rollback_End		= 0x1000,
	} ETranState;

	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_NONE; }
	//virtual bool RequestCurerntCash() { return false; }
	virtual bool PayCash() { return false; }

	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual void Delete() = 0;	// 상속받은 개체는 지우는 방식이 다를수 있기 때문에 만든 함수
	virtual bool Commit(bool const bIsCommit, BM::Stream::DEF_STREAM_TYPE const kSubType = 0);
	virtual bool Rollback() = 0;
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder) { return false; };
	virtual void Log(BM::vstring &vLogString) const;

	BM::Stream& ResultPacket() { return m_kResultPacket; }
	void AddedPacketAssign(BM::Stream const& kPacket, bool const bPosAdjust = false);
	BM::Stream& AddedPacket() { return m_kAddedPacket; }

private:
	// 호출하면 안되는 함수들
	static PgTransaction* New(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);


private:
	CLASS_DECLARATION_S(BM::GUID, TransactionKey);
	CLASS_DECLARATION_S(BM::GUID, MemberGuid);
	CLASS_DECLARATION_S(BM::GUID, CharacterGuid);
	CLASS_DECLARATION_S(SGroundKey, GroundKey);
	CLASS_DECLARATION_S(SERVER_IDENTITY, TargetSI);
	CLASS_DECLARATION_S(EItemModifyParentEventType, EventType);
	CLASS_DECLARATION_S(DWORD, UID);
	CLASS_DECLARATION_S(std::wstring, AccountID);
	CLASS_DECLARATION_S(CEL::ADDR_INFO, RemoteAddr);
	CLASS_DECLARATION_S(BM::GUID, ShopGuid);
	CLASS_DECLARATION_S(__int64, InitCash);
	CLASS_DECLARATION_S(__int64, InitMileage);
	CLASS_DECLARATION_S(__int64, CurCash);
	CLASS_DECLARATION_S(__int64, CurMileage);
	CLASS_DECLARATION_S(__int64, AddedMileage);
	CLASS_DECLARATION_S(__int64, OpenMarketDealingCash);
	CLASS_DECLARATION_REF(PgLogCont, m_kLogCont, LogCont);
	CLASS_DECLARATION_S(int, TranState);	// ETranState bitflag
	CLASS_DECLARATION_S(std::wstring, ChargeNo);	// GALA 용 Cash 결제Code
	CLASS_DECLARATION_S(unsigned __int64, WarehouseNo);	// NC용 : 처리가 완료되었을 때 이값을 꼭 전달해 줘야 함. 그렇지 않으면 SA서버에서 처리가 안된것으로 판단해서 중복으로 아이템 전달이 됨.		
	CLASS_DECLARATION_S(int, ItemLimitCount);		// 한정판매 아이템 남은 개수
	CLASS_DECLARATION_S(int, ItemBuyLimitCount);	// 한정판매 아이템을 구매할수 있는 개수
	CLASS_DECLARATION_S(int, ItemBuyCount);	// 한정판매 아이템을 구매한 개수
	CLASS_DECLARATION_S(int, ItemIdx);		// 한정판매 아이템인 경우만 저장
	CLASS_DECLARATION_S(int, ItemTotalCount);	// 한정판매 전체 팔수 있는 수치(최초 수량)

protected:
	UINT64 m_i64CreateTime;
	BM::Stream m_kResultPacket;
	BM::Stream m_kAddedPacket;
};

class PgSendGiftTran
	: public PgTransaction
{
public :
	explicit PgSendGiftTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgSendGiftTran(BM::Stream & _Packet);
	~PgSendGiftTran();

	/*
	static PgSendGiftTran* New(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event));
	*/
	PgSendGiftTran const& operator=(PgSendGiftTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_SEND_GIFT; }
	//virtual bool RequestCurerntCash();
	virtual bool PayCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);

protected:
	virtual void Delete();

private:
	PgSendGiftTran();

private:
	CLASS_DECLARATION_S(std::wstring, CharacterName);
	CLASS_DECLARATION_S(__int64, Cost);
	CLASS_DECLARATION_S(__int64, BonusUse);
	CLASS_DECLARATION_S(__int64, Bonus);
	CLASS_DECLARATION_S(std::wstring, SenderName);
	CLASS_DECLARATION_S(std::wstring, ArticleName);
	CLASS_DECLARATION_S(std::wstring, ReceiverName);
	CLASS_DECLARATION_S(int, ArticleIndex);
	CLASS_DECLARATION_S(int, PriceIdx);
	CLASS_DECLARATION_S(int, TimeType);
	CLASS_DECLARATION_S(int, UseTime);
	CLASS_DECLARATION_S(std::wstring, Comment);
	CLASS_DECLARATION_S(DWORD, ReceiverUID);
};

class PgExpandTimeLimitTran
	: public PgTransaction
{
public :
	explicit PgExpandTimeLimitTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgExpandTimeLimitTran(BM::Stream & _Packet);
	~PgExpandTimeLimitTran();

	PgExpandTimeLimitTran const& operator=(PgExpandTimeLimitTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_EXPAND_TIMELIMIT; }
	//virtual bool RequestCurerntCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool PayCash();
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;

	//bool RequestPayCash(CONT_PLAYER_MODIFY_ORDER const& rkOrder);
	void SetBuyItem(CONT_BUYITEM const& rkBuyItem) { m_kBuyItem = rkBuyItem; }

protected:
	virtual void Delete();

private:
	PgExpandTimeLimitTran();

private:
	CLASS_DECLARATION_S(std::wstring, CharacterName);
	CLASS_DECLARATION_S(__int64, Cost);
	CLASS_DECLARATION_S(__int64, BonusUse);
	CLASS_DECLARATION_S(__int64, Bonus);
	CONT_BUYITEM m_kBuyItem;
};

class PgCashShopBuyTran
	: public PgTransaction
{
public :
	explicit PgCashShopBuyTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgCashShopBuyTran(BM::Stream & _Packet);
	~PgCashShopBuyTran();

	PgCashShopBuyTran const& operator=(PgCashShopBuyTran const& rhs);

	void AddedPacketAssign(BM::Stream const& kPacket, bool const bPosAdjust = false);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_CASHSHOP_BUY; }
	//virtual bool RequestCurerntCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool PayCash();
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;

	void SetBuyItem(CONT_BUYITEM const& rkBuyItem) { m_kBuyItem = rkBuyItem; }

protected:
	virtual void Delete();

private:
	PgCashShopBuyTran();

private:
	CLASS_DECLARATION_S(std::wstring, CharacterName);
	CLASS_DECLARATION_S(__int64, Cost);
	CLASS_DECLARATION_S(__int64, BonusUse);
	CLASS_DECLARATION_S(__int64, Bonus);
	CLASS_DECLARATION_S(BYTE, BuyType);
	CONT_BUYITEM m_kBuyItem;
};

class PgEnterCashShopTran
	: public PgTransaction
{
public :
	explicit PgEnterCashShopTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgEnterCashShopTran(BM::Stream & _Packet);
	~PgEnterCashShopTran();

	PgEnterCashShopTran const& operator=(PgEnterCashShopTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_CASHSHOP_ENTER; }
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual void Log(BM::vstring &vLogString) const;

protected:
	virtual void Delete();
	virtual bool Rollback() { return false; }	// Nothing to do

private:
	PgEnterCashShopTran();

private:

};

class PgAddCashTran
	: public PgTransaction
{
public :
	explicit PgAddCashTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgAddCashTran(BM::Stream & _Packet);
	~PgAddCashTran();

	PgAddCashTran const& operator=(PgAddCashTran const& rhs);

public:

	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_ADD_CASH; }
	//virtual bool RequestCurerntCash(){return false;}
	virtual bool PayCash();
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;

protected:
	virtual void Delete();

private:
	PgAddCashTran();

private:

	CLASS_DECLARATION_S(std::wstring, CharacterName);
	CLASS_DECLARATION_S(__int64, Cash);
};

class PgEnterOpenMarketTran
	: public PgTransaction
{
public :
	explicit PgEnterOpenMarketTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgEnterOpenMarketTran(BM::Stream & _Packet);
	~PgEnterOpenMarketTran();

	PgEnterOpenMarketTran const& operator=(PgEnterOpenMarketTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_OPENMARKET_ENTER; }
	//virtual bool RequestCurerntCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual void Log(BM::vstring &vLogString) const;

protected:
	virtual void Delete();
	virtual bool Rollback() { return false; }

private:
	PgEnterOpenMarketTran();

private:

};

class PgOpenMarketBuyTran
	: public PgTransaction
{
public :
	explicit PgOpenMarketBuyTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgOpenMarketBuyTran(BM::Stream & _Packet);
	~PgOpenMarketBuyTran();

	PgOpenMarketBuyTran const& operator=(PgOpenMarketBuyTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return ECASH_TRAN_OPENMARKET_BUY; }
	//virtual bool RequestCurerntCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool PayCash();
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;

protected:
	virtual void Delete();

private:
	PgOpenMarketBuyTran();

private:
	CLASS_DECLARATION_S(BM::GUID, MarketGuid);
	CLASS_DECLARATION_S(BM::GUID, ArticleGuid);
	CLASS_DECLARATION_S(WORD, BuyNum);
	CLASS_DECLARATION_S(std::wstring, BuyerName);
	CLASS_DECLARATION_S(__int64, Cost);
	CLASS_DECLARATION_S(__int64, BonusUse);
	CLASS_DECLARATION_S(__int64, Bonus);
};

class PgOpenMarketCashRegTran
	: public PgTransaction
{
public :
	explicit PgOpenMarketCashRegTran(BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgOpenMarketCashRegTran(BM::Stream & _Packet);
	~PgOpenMarketCashRegTran();

	PgOpenMarketCashRegTran const& operator=(PgOpenMarketCashRegTran const& rhs);

public:
	virtual ECASH_TRAN_TYPE GetTransactionType() const { return TranType(); }
	//virtual bool RequestCurerntCash();
	virtual bool PayCash();
	virtual void WriteToPacket(BM::Stream & kPacket) const;
	virtual bool ReadFromPacket(BM::Stream & kPacket);
	virtual bool Rollback();
	virtual void Log(BM::vstring &vLogString) const;
	virtual bool GetOrder(CONT_PLAYER_MODIFY_ORDER& rkOrder);

protected:
	virtual void Delete();

private:
	PgOpenMarketCashRegTran();

private:
	CLASS_DECLARATION_S(__int64, Cost);
	CLASS_DECLARATION_S(ECASH_TRAN_TYPE, TranType);
};


class PgTranPointer
{
public:
	explicit PgTranPointer(PgTransaction* _Tran);
	explicit PgTranPointer(ECASH_TRAN_TYPE const _Tran, BM::GUID const& _Member, BM::GUID const& _Character, SGroundKey const& _Ground,
		SERVER_IDENTITY _Target, EItemModifyParentEventType const _Event);
	explicit PgTranPointer(BM::Stream & _Packet);
	explicit PgTranPointer(BM::Stream const& _Packet);
	~PgTranPointer();

public:
	// operator overloading 하지 말고, 별도의 함수로 호출하도록 하라
	PgTransaction* GetTran();
	PgTransaction* const GetTran() const;
	void WriteToPacket(BM::Stream & kPacket) const;
	bool ReadFromPacket(BM::Stream & kPacket);
	ECASH_TRAN_TYPE GetTransactionType();
	BM::GUID TransactionKey();
	bool Finish(HRESULT const hResult);
	bool Commit();
	bool Rollback();
	void Log(BM::vstring& rkLog) const;
	bool SendLog();

public:
	bool RequestCurerntCash() const;
	bool RecvCurerntLimitSell(ECashShopResult const eResult, int const iItemTotalCount, int const iItemLimitCount, int const iItemBuyLimitCount, int const iItemBuyCount);
	bool RequestCurerntLimitSell() const;
	bool RecvCurrentCash(ECashShopResult const eResult, __int64 const i64Cash, __int64 const i64Mileage);
	bool RequestPayCash() const;
	bool RecvPayCash(ECashShopResult const eResult, __int64 const i64CashCur, __int64 const i64MileageCur, __int64 const i64AddedMileage);
	bool RequestItemModify(CONT_PLAYER_MODIFY_ORDER const& rkOrder);

protected:
	bool PushOrderJob(EItemModifyParentEventType const eCause, CONT_PLAYER_MODIFY_ORDER const* const pkOrder = NULL, BM::Stream * const pkAddonPacket = NULL,
		BM::Stream * const pk2ndPacket = NULL) const;


private:
	// Don't call this function
	PgTranPointer(PgTranPointer const& rhs);
	PgTranPointer const& operator=(PgTranPointer const& rhs);

private:
	PgTransaction* m_pkTransaction;
} ;

// cash 관련 함수
namespace TRAN_HELPER
{
	HRESULT RecvCashBalance(PgTranPointer &rkTran);
	/*
	HRESULT RecvCashModify(ECashShopResult const eReturn, BM::GUID const& kCharacterGuid,BM::GUID const & kShopGuid, EItemModifyParentEventType const kCause,
		std::wstring const& kCharName, __int64 const i64Cash, __int64 i64Mileage, __int64 const i64OldCash, __int64 const i64OldMileage,
		int const i64UsedCash, int const i64UsedMileage);
		*/
	HRESULT RecvCashModify(ECashShopResult const eResult, PgTranPointer &rkTran, __int64 const i64CashCur, __int64 const i64MileageCur, __int64 const i64AddedMileage);
};


// GALA 전용 Cash global 함수
namespace GALA
{
	bool SendCashRequest(BM::Stream const& kPacket);

	template <typename T_TRAN>
	bool RequestCurrentCash(T_TRAN const& kTran)
	{
		BM::Stream kQPacket(PT_A_CN_REQ_QUERY_CASH);
		kQPacket.Push(kTran.AccountID());
		kQPacket.Push(kTran.UID());
		kTran.RemoteAddr().WriteToPacket(kQPacket);
		kTran.WriteToPacket(kQPacket);
		return SendCashRequest(kQPacket);
	}

	ECashShopResult GetCashShopResult(EGTDX_ERROR_CODE const eCode);
	bool SendCashRollback(PgTransaction const& rkTransaction);
	bool CashTransaction(PgTransaction const& rkTran, bool const bRollback, __int64 const i64Cash, __int64 const i64Mileage, __int64 const i64MileageAdd);

	void OnReceiveCashResult(BM::Stream* const pkPacket);
};

namespace NIVAL
{
	bool SendCashRollback(PgTransaction const& rkTransaction, CASH::SCashCost const& rkCost);
	bool SendCashRequest(BM::Stream const& kPacket);
	void OnReceiveCashResult(BM::Stream* const pkPacket);
};

namespace NC
{
	void OnReceiveCashResult(BM::Stream* const pkPacket);
}

namespace GRAVITY
{
	void OnReceiveCashResult(BM::Stream* const pkPacket);
}

#endif // CONTENTS_CONTENTSSERVER_SERVERSET_TRANSACTION_H