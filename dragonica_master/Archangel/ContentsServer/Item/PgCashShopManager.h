#ifndef CONTENTS_CONTENTSSERVER_ITEM_PGCASHSHOPMANAGER_H
#define CONTENTS_CONTENTSSERVER_ITEM_PGCASHSHOPMANAGER_H

class PgTranPointer;
class PgTransaction;

typedef struct tagCASHSHOPUSER
{
	CLASS_DECLARATION_S(BM::GUID,CashShopGuid); //캐시샵 진입시 마다 지급되는 구매용 guid
	CLASS_DECLARATION_S(__int64,TotalUsedCash);	//총 사용한 캐시
	CLASS_DECLARATION_S(__int64,RecvPointCash_100);// 100 포인트 마다 보너스 포인트를 지급 받은 금액 기록
	CLASS_DECLARATION_S(__int64,RecvPointCash_1000);// 1000 포인트 마다 보너스 포인트 지급 받은 금액 기록
	tagCASHSHOPUSER():m_kTotalUsedCash(0),m_kRecvPointCash_100(0),m_kRecvPointCash_1000(0){}
}SCASHSHOPUSER;

class PgCashShopUserMgr
{
	typedef std::map<BM::GUID,SCASHSHOPUSER> CONT_SHOPUSER;
public:
	PgCashShopUserMgr(){}
	~PgCashShopUserMgr(){}

public:

	bool Locked_AddUser(SCASHSHOPUSER const & kUser);
	bool Locked_RemoveUser(BM::GUID const & kShopUserGuid);
	bool Locked_GetUser(BM::GUID const & kShopUserGuid,SCASHSHOPUSER & kUser) const;
	bool Locked_SetUser(SCASHSHOPUSER const & kUser);

private:
	mutable Loki::Mutex m_kMutex;
	CONT_SHOPUSER m_kCont;
};

class PgCashShopMgr
{
public:
	PgCashShopMgr(){}
	~PgCashShopMgr(){}

	bool Locked_HandleRecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	ECashShopResult OnProcessReqBuyArticle(EItemModifyParentEventType const kCause,__int64 const i64Cash,__int64 const i64Bonus,BM::GUID const & kOwnerGuId, PgTranPointer &rkTran);
	ECashShopResult OnProcessReqGiftArticle(EItemModifyParentEventType const kCause,__int64 const i64Cash,__int64 const i64Bonus,BM::GUID const & kOwnerGuId, PgTranPointer &rkTran);
	
	ECashShopResult OnProcessReqRecvGift(SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey,BM::GUID const & kOwnerGuId,SCASHGIFTINFO const & kGift);
	ECashShopResult OnProcessReqAddTimeLimit(EItemModifyParentEventType const kCause,__int64 const i64Cash,__int64 const i64Bonus,BM::GUID const & kOwnerGuid,
		PgTranPointer &rkTran);

	bool UpdateCashShopRank(BM::GUID const & kOwnerGuId,BYTE const bState,__int64 const i64Cash,int const iMode);
	bool UpdateUseCash(BM::GUID const & kOwnerGuid,BM::GUID const & kShopUserGuid,__int64 const i64Cost);

	static ECashShopResult OnProcessReqGiftArticle(PgTranPointer &rkTran);
protected:

	void OnRecvPT_M_I_CS_REQ_MODIFY_VISABLE_RANK(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_BUY_ARTICLE(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_SEND_GIFT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_ENTER_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_EXIT_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_RECV_GIFT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);
	void OnRecvPT_M_I_CS_REQ_ADD_TIMELIMIT(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

	void OnRecvPT_M_I_CS_REQ_SIMPLE_ENTER_CASHSHOP(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket);

private:

	__int64 const CalcAddBonus(__int64 const i64Cost,int const iMileage);
	__int64 const CalcLocalAddBonus(BM::GUID const & kOwnerGuid,BM::GUID const & kShopUserGuid,__int64 const i64Cost);
	bool	IsEnableUseBonus(__int64 const i64BonusUse);

	PgCashShopUserMgr m_kShopUserMgr;
};

#define g_kCashShopMgr SINGLETON_STATIC(PgCashShopMgr)

#endif // CONTENTS_CONTENTSSERVER_ITEM_PGCASHSHOPMANAGER_H