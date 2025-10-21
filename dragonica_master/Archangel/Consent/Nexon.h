#ifndef CONSENT_CONSENTSERVER_LINKAGE_NEXON_NEXON_H
#define CONSENT_CONSENTSERVER_LINKAGE_NEXON_NEXON_H

#include "BM/TWrapper.h"
#include "AilePack\Gala_Constant.h"
#include "Constant.h"
#include "PgSiteMemberMgr.h"
#include "AilePack/Gala_Constant.h"
#include "BM/PgMsgWorker.h"
#include "BM/PgMsgDispatcher.h"

extern bool InitNexonPassport();

extern void CALLBACK OnConnectFromImmigrationForJapan( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromImmigrationForJapan( CEL::CSession_Base *pkSession );
extern void CALLBACK OnRecvFromImmigrationForJapan(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

extern void CALLBACK OnConnectFromNexonCash( CEL::CSession_Base *pkSession );
extern void CALLBACK OnDisconnectFromNexonCash( CEL::CSession_Base *pkSession );
extern void CALLBACK OnRecvFromNexonCash(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

typedef struct tagNexonAuthInfo
{
	int iSiteNo;
	SAuthRequest kAuthReq;
	std::wstring kStrPassport; 

	typedef std::list< BM::GUID > CONT_WORKDATA_TARGET;
	CONT_WORKDATA_TARGET m_kContTarget;
}SNexonAuthInfo;

extern ETryLoginResult CheckNexonPassport(SNexonAuthInfo *pkWorkData);

class CNXLoginMsgWorker
	: public PgMsgWorker< SNexonAuthInfo >
{
public:
	CNXLoginMsgWorker()
	{
	}
	~CNXLoginMsgWorker()
	{
	}

	virtual HRESULT VProcess(SNexonAuthInfo *pkWorkData);
	
};

extern PgMsgDispatcher< SNexonAuthInfo, CNXLoginMsgWorker> g_kNxLoginDis;


typedef enum eJAPAN_CASH_HEADER
	:	BYTE
{
	XXX_Item = 0,
	XXX_Coupon = 1,

	// 아이템 구매 패킷
	Remain_Cash = 0,
	Purchase = 1,
	Purchase_Ok = 2,
		Purchase_Confirm = 3,
		Purchase_Cancel = 4,
		No_Remain = 5,
	DB_Error = 6,
	Gift = 7,
	Gift_Limit_Over = 8,
		Gift_Not_Ready = 9,
		Gift_Under_Age = 10,

	// 쿠폰 패킷
	Check_Coupon = 0,
	Use_Coupon = 1,
	Check_Cafe_Coupon = 2,
	Use_Cafe_Coupon = 3,

	kValidCoupon = 0,
	kUsedCoupon = 1,
	kExpiredCoupon = 2,
	kReUsableCoupon = 3, 
	kInvalidCoupon = 4,
	kInvalidDomain = 5,
	kInvalidServer = 6, 
	kError = 7,
}E_JAPAN_CASH_HEADER;

extern HRESULT NP_Push(BM::Stream &kPacket, BYTE byValue);
extern HRESULT NP_Push(BM::Stream &kPacket, short nValue);
extern HRESULT NP_Push(BM::Stream &kPacket, int iValue);
extern HRESULT NP_Push(BM::Stream &kPacket, __int64 iValue);
extern HRESULT NP_Push(BM::Stream &kPacket, std::string &strValue);
extern HRESULT NP_Push(BM::Stream &kPacket, std::wstring &strValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, BYTE &byValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, short &nValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, int &iValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, __int64 &iValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, std::string &strValue);
extern HRESULT NP_Pop(BM::Stream &kPacket, std::wstring &strValue);

static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 60000;
static const short NexonCash_SERVER_SITE_NUM = -1;
static const int GameCode = 0x01008206;

typedef struct tagSNexonCashRequest
{
	tagSNexonCashRequest()
	{
		m_bIsSimulate = false;
		wReqPacketType = 0;
//		SERVER_IDENTITY_SITE kFromSI;
	//	BM::Stream kAddonPacket;

		m_eMainType = XXX_Item;
		m_eSubType = XXX_Item;
//		BM::Stream m_kGDPacket;//요구 패킷
		m_dwRequestKey = 0;
//		std::wstring m_strMemberID;
//		std::wstring m_strCharName;
		ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;

	}

	explicit tagSNexonCashRequest(
		eJAPAN_CASH_HEADER const eMainType, eJAPAN_CASH_HEADER const eSubType, DWORD const kReqKey, 
		/*BYTE const byProductType(안씀),*/ const __int64 order_id, int const item_id , int const price, 
		std::wstring const& member_id, std::wstring const& char_name, 
		BYTE const domaintype, short const server_type, std::wstring const& recver_id, /*상대 SSN 안씀*/

		PACKET_ID_TYPE const ret_type, SERVER_IDENTITY_SITE const& _SI,
		BM::GUID const &kTranKey, BM::Stream const * pkAddonPacket = NULL)
		: kFromSI(_SI)
	{
		//구입용 초기화 넥슨 규격
		//	[XXX_Item][Purchase]{request_id}
		//	|product_type|<orderid>{item_id}{price}(Nexon_id)(gameid)[domaintype]|servertype|

		//선물용 초기화 넥슨 규격
		//	[XXX_Item][Gift]{request_id}
		//	|product_type|<orderid>{item_id}{price}(Nexon_id)(gameid)[domaintype]|servertype|
		//	(선물을 받을 유저 gameid)(선물할 유저의 SSN

		m_strMemberID = member_id;
		m_strCharName = char_name;
		m_dwRequestKey = kReqKey;

		m_eMainType = eMainType;
		m_eSubType = eSubType;
		m_kTranKey = kTranKey;

		ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
		wReqPacketType = ret_type;

		if (pkAddonPacket != NULL){m_kOrgPacket.Push(*pkAddonPacket);}

		if(0 == price)
		{
			m_bIsSimulate = true;
		}
		else
		{
			m_bIsSimulate = false;
		}

		if( XXX_Item == eMainType
		&&	( Purchase == eSubType || Gift == eSubType ) )
		{
			NP_Push(m_kGDPacket, (BYTE)eMainType);
			NP_Push(m_kGDPacket, (BYTE)eSubType);
			NP_Push(m_kGDPacket, (int)m_dwRequestKey);
			NP_Push(m_kGDPacket, (short)58);//product_type);//넥슨 고정값
			NP_Push(m_kGDPacket, (__int64)order_id);//orderid);//안씀
			NP_Push(m_kGDPacket, (int)item_id);
			NP_Push(m_kGDPacket, (int)price);
			NP_Push(m_kGDPacket, (std::string)(MB(m_strMemberID)));
			NP_Push(m_kGDPacket, (std::string)MB(m_strCharName));//캐릭명
			NP_Push(m_kGDPacket, (BYTE)((domaintype*10) + server_type));//
			NP_Push(m_kGDPacket, (short)1);//서버타입
			if(Gift == eSubType)
			{
				NP_Push(m_kGDPacket, (std::string)(MB(recver_id)));//캐릭명
				NP_Push(m_kGDPacket, (std::string)("9909091"));//SSN
			}
		}
	}

	explicit tagSNexonCashRequest(
		eJAPAN_CASH_HEADER const eMainType, eJAPAN_CASH_HEADER const eSubType,
		DWORD const kReqKey, std::wstring const& owner_id, 
		 
		PACKET_ID_TYPE const ret_type, SERVER_IDENTITY_SITE const& _SI,
		BM::Stream const * pkAddonPacket = NULL)
		: kFromSI(_SI)
	{
		//	포인트 확인 넥슨 규격
		//	 [XXX_Item] [Remain_Cash] {Request_Id} (Nexon_id)

		m_strMemberID = owner_id;
		m_dwRequestKey = kReqKey;

		m_eMainType = eMainType;
		m_eSubType = eSubType;

		ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
		wReqPacketType = ret_type;
		if (pkAddonPacket != NULL)
		{
			m_kOrgPacket.Push(*pkAddonPacket);
		}

		m_bIsSimulate = false;

		if( XXX_Item == eMainType
		&&	Remain_Cash == eSubType)
		{
			NP_Push(m_kGDPacket, (BYTE)XXX_Item);
			NP_Push(m_kGDPacket, (BYTE)Remain_Cash);
			NP_Push(m_kGDPacket, (int)m_dwRequestKey);
			NP_Push(m_kGDPacket, (std::string)(MB(m_strMemberID)));
		}
	}

	explicit tagSNexonCashRequest(
		eJAPAN_CASH_HEADER const eMainType, eJAPAN_CASH_HEADER const eSubType,
		DWORD const kReqKey, BYTE const db_id, int const tran_id, 
		 
		PACKET_ID_TYPE const ret_type, SERVER_IDENTITY_SITE const& _SI,
		BM::Stream const * pkAddonPacket = NULL)
		: kFromSI(_SI)
	{
		//	커밋 트랜잭션
		//	[XXX_Item] [Purchase_Confirm] {Request_Id} [DB_Id] {Transaction_Id}
		//	[XXX_Item] [Purchase_Cancel] {Request_Id} [DB_Id] {Transaction_Id}
		m_dwRequestKey = kReqKey;
		wReqPacketType = ret_type;

		m_eMainType = eMainType;
		m_eSubType = eSubType;
		m_kDBID = db_id;
		m_kJapanTranID = tran_id;

		ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
		wReqPacketType = ret_type;
		if (pkAddonPacket != NULL)
		{
			m_kOrgPacket.Push(*pkAddonPacket);
		}

		m_bIsSimulate = false;

		if( XXX_Item == eMainType
		&&	(Purchase_Cancel == eSubType || Purchase_Confirm == eSubType) )
		{
			NP_Push(m_kGDPacket, (BYTE)XXX_Item);
			NP_Push(m_kGDPacket, (BYTE)eSubType);
			NP_Push(m_kGDPacket, (int)m_dwRequestKey);
			NP_Push(m_kGDPacket, (BYTE)m_kDBID);
			NP_Push(m_kGDPacket, (int)m_kJapanTranID);
		}
	}

	PACKET_ID_TYPE wReqPacketType;
	SERVER_IDENTITY_SITE kFromSI;

	eJAPAN_CASH_HEADER m_eMainType;
	eJAPAN_CASH_HEADER m_eSubType;

	ACE_UINT64 ui64ExpireTime;
	
	DWORD m_dwRequestKey;
	std::wstring m_strMemberID;
	std::wstring m_strCharName;

	BM::GUID m_kCharacterGUID;
	BYTE m_kDBID;
	int m_kJapanTranID;//일본 서버에서 오는 트랜잭션 ID

	BM::GUID m_kTranKey;

	BM::Stream m_kOrgPacket;//요청시 원본 패킷

	BM::Stream m_kGDPacket;//요구 패킷(캐쉬 데몬에 이거로 요청)

	bool m_bIsSimulate;
}SNexonCashRequest;

class PgJapanServerImpl
	: public PgSiteMemberMgr
{
	friend class CAutoTran;
public:
	PgJapanServerImpl();
	virtual ~PgJapanServerImpl();

public:
	// STATIC FUNCTION
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
	typedef enum
	{
		ELOGINSTEP_TRY_HTTP_CONNECTION = 0,
		ELOGINSTEP_REQ_LOGIN,
		ELOGINSTEP_HTTP_DISCONNECT,
		ELOGINSTEP_RECV_CASH_BALANCE,
	} ELOGINSTEP;
#endif
	typedef struct _SUserInfo
	{
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
		typedef std::vector<DWORD> VEC_LOGINTIME;
#endif
		explicit _SUserInfo(short const &_Site, SAuthRequest const& _AuthInfo, LOCAL_MGR::NATION_CODE const _NationCode,
			GALA::EUSER_REQ_TYPE const _ReqType)
		{
			kAuthInfo = _AuthInfo;
			sSite = _Site;
			eReqType = _ReqType;
			eNationCode = _NationCode;
			i64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
			i64Cash = 0;
		}

		_SUserInfo const operator=(_SUserInfo const& rhs)
		{
			kAuthInfo = rhs.kAuthInfo;
			sSite = rhs.sSite;
			eReqType = rhs.eReqType;
			eNationCode = rhs.eNationCode;
			i64ExpireTime = rhs.i64ExpireTime;
			strAnswer = rhs.strAnswer;
			i64Cash = rhs.i64Cash;
		}

		SAuthRequest kAuthInfo;
		short sSite;	// 접속중인 Site
		GALA::EUSER_REQ_TYPE eReqType;
		LOCAL_MGR::NATION_CODE eNationCode;
		ACE_UINT64 i64ExpireTime;
		std::string strAnswer;
		std::string strErrorMsg;
		__int64 i64Cash;
#ifdef GALA_LOG_LOGIN_ELAPSEDTIME
		VEC_LOGINTIME kLoginLog;
#endif
	} SUserInfo;

//	typedef std::map< std::wstring, SUserInfo* > CONT_ID;	// <RequestGuid, SUserInfo>
	typedef std::map< DWORD, SNexonCashRequest > CONT_JAPAN_CASH_REQUEST;	// <RequestID, SNexonCashRequest>
	typedef std::map< BM::GUID, SNexonCashRequest > CONT_FINAL_COMMIT;	// <RequestID, SNexonCashRequest>

	bool OnRegistConnector(CEL::SRegistResult const &rkArg);

	void OnConnectFromNexonCash( CEL::CSession_Base *pkSession );
	void OnDisconnectFromNexonCash( CEL::CSession_Base *pkSession );

	void OnRecvFromNexonCash(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);

	virtual void Timer5s();
	bool CashSimulate(SNexonCashRequest &kReqInfo);

	void SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult);//외부에서 씀.

protected:
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_JAPAN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_NFY_TRAN_COMMIT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket, bool const bIsCommit);
	void RecvPT_A_CN_REQ_SENDGIFT(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvUseCash(BM::Stream::DEF_STREAM_TYPE const wSubType, LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);

//	void SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult);
	bool SendToNexonCash( BM::Stream const &kPacket )const;
	DWORD GetNexonCashRequestKey();

	bool AddNexonCashRequest(SNexonCashRequest const &kReqInfo);
	bool RemoveNexonCashRequest(DWORD const dwReqKey);

	bool RemoveFinalCommit(BM::GUID const &kTranKey);

	bool SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket);
	bool SendCashFailResult(SNexonCashRequest const &kNexonReq, WORD const wRetCode = GALA::EGTDX_ERROR_BILLINGSERVER_ERROR);
private:
	// NexonCash .....
	volatile long m_dwLastNexonCashRequestKey;
	CONT_JAPAN_CASH_REQUEST m_kNexonCashRequest;
	CONT_FINAL_COMMIT m_kContFinalTran;
};

class CAutoTran;
class PgJapanServerWrapper
	: public TWrapper< PgJapanServerImpl >
{
	friend class CAutoTran;
public:
	PgJapanServerWrapper()
	{
	}

	virtual ~PgJapanServerWrapper()
	{
	}

	void OnConnectFromImmigration( CEL::CSession_Base *pkSession )
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->Locked_OnConnectFromImmigration(pkSession);
	}

	void OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->Locked_OnDisconnectFromImmigration(pkSession);
	}

	void OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE usType, BM::Stream * const pkPacket)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->OnRecvFromImmigration( pkSession, usType, pkPacket );
	}

	void OnConnectFromNexonCash( CEL::CSession_Base *pkSession )
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->OnConnectFromNexonCash(pkSession);
	}

	void OnDisconnectFromNexonCash( CEL::CSession_Base *pkSession )
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->OnDisconnectFromNexonCash(pkSession);
	}

	void OnRecvFromNexonCash(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->OnRecvFromNexonCash( pkSession, pkPacket );
	}

	bool OnRegistConnector(CEL::SRegistResult const &rkArg)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->OnRegistConnector(rkArg);
	}

	void TryConnect()
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->Locked_TryConnect();
	}
	
	void Timer5s()
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		Instance()->Timer5s();
	}

	bool CashSimulate(SNexonCashRequest &kReqInfo)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
		return Instance()->CashSimulate(kReqInfo);
	}

	void SendLoginResult(CEL::CSession_Base *pkSession, PgJapanServerImpl::SUserInfo const& kUser, ETryLoginResult const eResult)
	{
		BM::CAutoMutex lock(m_kMutex_Wrapper_);
		Instance()->SendLoginResult(pkSession, kUser, eResult);
	}
};

class CAutoTran
{
public:
	CAutoTran(PgJapanServerImpl &kOwner)
		:	m_rkOwnerImpl(kOwner)
	{
		m_bIsFinalCommit = false;
	}

	~CAutoTran()
	{
		if(m_bIsFinalCommit)//파이널 커밋 값은 무조건 없에야함.(휘발성 데이터이므로)
		{
			m_rkOwnerImpl.RemoveFinalCommit(m_kNexonReq.m_kTranKey);
		}

		if(m_rkOwnerImpl.AddNexonCashRequest(m_kNexonReq))
		{
			if(m_kNexonReq.m_bIsSimulate)
			{
				if(m_rkOwnerImpl.CashSimulate(m_kNexonReq))
				{
					return;
				}
			}
			else
			{
				if(m_rkOwnerImpl.SendToNexonCash(m_kNexonReq.m_kGDPacket))
				{
					return;
				}
			}
		}
		m_rkOwnerImpl.RemoveNexonCashRequest(m_kNexonReq.m_dwRequestKey);//무조건 지움.
		m_rkOwnerImpl.SendCashFailResult(m_kNexonReq);// 실패 결과 전송해야함.
		//넥슨 요청 큐에 넣기 시도. (문제는 리퀘스트 ID가 할때마다 달라진다는거고..) 먼저 넣은거 보다 뒤에꺼가 빨리 올 수 있을지도 모르겠긴 하다
		//보내기		PgSessionMgr::SendToServer(NexonCash_SERVER_SITE_NUM, m_kGDPacket);
		//보내기 실패이면 시스템 에러를 리턴 해서 응답 하도록 해줘야함.	return E_SYSTEM_ERROR
	}

	void PrepareData(SNexonCashRequest const &kNexonReq)
	{
		m_kNexonReq = kNexonReq;
	}

	void SetFinalCommit(){m_bIsFinalCommit = true;}
public:
	// 값 셋팅.
	// 
	// -> 등록 자동화, 
	// -> 보내기 기능
	// ->
	PgJapanServerImpl &m_rkOwnerImpl;
	SNexonCashRequest m_kNexonReq;
	bool m_bIsFinalCommit;//마지막 커밋은 따로 처리해야함.
};

#define g_kJapanDaemon SINGLETON_STATIC(PgJapanServerWrapper)

#endif // CONSENT_CONSENTSERVER_LINKAGE_NEXON_NEXON_H