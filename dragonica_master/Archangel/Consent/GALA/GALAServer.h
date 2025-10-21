#ifndef CONSENT_CONSENTSERVER_LINKAGE_GALA_GALASERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_GALA_GALASERVER_H

#include "AilePack\Gala_Constant.h"
#include "Constant.h"
#include "PgSiteMemberMgr.h"

#pragma pack(1)

#pragma pack()

class PgGalaServer : public PgSiteMemberMgr
{
public :
	PgGalaServer();
	virtual ~PgGalaServer();

	static const short GTDX_SERVER_SITE_NUM = -1;

	// STATIC FUNCTION
	static const int LOGIN_ANSWER_MIN_LINE = 9;
	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromGTXD( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromGTXD( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromGTXD(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 60000;

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

	typedef struct _SGTDXRequest
	{
		explicit _SGTDXRequest(std::wstring const& _ID, DWORD const _ReqKey, GALA::EUSER_REQ_TYPE const _Type, PACKET_ID_TYPE const _ReqPacket, SERVER_IDENTITY_SITE const& _SI, BM::Stream const * pkAddonPacket = NULL)
			: kFromSI(_SI)
		{
			strID = _ID;
			dwRequestKey = _ReqKey;
			eReqType = _Type;
			ui64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
			wReqPacketType = _ReqPacket;
			if (pkAddonPacket != NULL)
			{
				kAddonPacket.Push(*pkAddonPacket);
			}
		}

		_SGTDXRequest const operator=(_SGTDXRequest const& rhs)
		{
			strID = rhs.strID;
			dwRequestKey = rhs.dwRequestKey;
			eReqType = rhs.eReqType;
			ui64ExpireTime = rhs.ui64ExpireTime;
			wReqPacketType = rhs.wReqPacketType;
			kAddonPacket = rhs.kAddonPacket;
			kFromSI = rhs.kFromSI;
		}

		std::wstring strID;
		DWORD dwRequestKey;
		GALA::EUSER_REQ_TYPE eReqType;
		ACE_UINT64 ui64ExpireTime;
		PACKET_ID_TYPE wReqPacketType;
		SERVER_IDENTITY_SITE kFromSI;
		BM::Stream kAddonPacket;
	} SGTDXRequest;


	typedef std::map<BM::GUID, SUserInfo*> CONT_REQUEST;	// <RequestGuid, SUserInfo>
	typedef std::map<std::wstring, SUserInfo*> CONT_ID;	// <RequestGuid, SUserInfo>
	typedef std::map<std::wstring, SGTDXRequest> CONT_GTDX_REQUEST;	// <UserID, SGTDXRequest>

	void Locked_OnConnectFromGTXD( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromGTXD( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromGTXD(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	bool Locked_OnRegistConnector(CEL::SRegistResult const &rkArg);
	void Locked_OnConnectFromHttp( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromHttp( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);

	void Locked_TestHttp();
	void Locked_Timer5s();

protected:
	std::wstring URLEncoding( std::wstring& kSource );
	char ConvertToHex( char code ) { return "0123456789abcdef"[code & 0xf]; }

	virtual void Timer5s();
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_BUYCASHITEM(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_EXPANDTIEM(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_SENDGIFT(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_BUYCASHITEM_ROLLBACK(LOCAL_MGR::NATION_CODE const eNation, SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);

	void SetAuthServer(std::wstring const &strAddress, WORD wPort);
	void SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult);
	bool SendLoginRequest(CEL::CSession_Base *pkSession);
	void ParsingLoginAnswer(BM::GUID const& rkRequest);
	void RemoveUserInfo(BM::GUID const& rkRequest);
	bool SendToGTDX( BM::Stream const &kPacket )const;
	DWORD GetGTDXRequestKey() { return ++m_dwLastGTDXRequestKey; } 
	template<typename T_PACK_STRUCT>
	HRESULT RequestGTDX(T_PACK_STRUCT& kInfo, PACKET_ID_TYPE const wReqPacket, SERVER_IDENTITY_SITE const& kFromSI, BM::Stream const* pkAddonPacket = NULL);
	bool AddGTDXRequest(GALA::EUSER_REQ_TYPE const eType, std::wstring const& strID, DWORD const dwRequestKey, PACKET_ID_TYPE const wReqPacket, SERVER_IDENTITY_SITE const& kFromSI, 
		BM::Stream const * pkAddonPacket);
	template<typename T_RETURN, typename T_ERASE_KEY>
	T_RETURN RemoveGTDXRequest(T_ERASE_KEY const& kKey);
	void OnGTDXReceive_ServerState( BM::Stream &kPacket);
	template<typename T_PACK_STRUCT>
	void OnGTDXReceive( BM::Stream &kPacket);

	void SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BALANCE const &rkResult);
	void SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BUY const &rkResult);
	void SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_GIFT const &rkResult);
	void SendGTDXResult(SGTDXRequest const& rkReq, GALA::BILL_PACK_BUY_CNL const &rkResult);
	bool SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket);

	static bool GetGameCode(LOCAL_MGR::NATION_CODE const eCode, std::string& rkOutCode);
	static bool GetRequestCode(GALA::EUSER_REQ_TYPE const eReqCode, std::string& rkOutCode);
	static GALA::EGTDX_ERROR_CODE GetErrorCode(HRESULT const hResult);
	static bool GetGTDXNationCode(LOCAL_MGR::NATION_CODE const eCode, LPTSTR lpszCode, size_t iCodeSize);

private:
	CLASS_DECLARATION_S(BM::GUID, HttpConnector);
	CEL::ADDR_INFO m_kHttpAddress;
	std::wstring m_strHttpHostname;

	CONT_REQUEST m_kUserByRequest;
	CONT_ID	m_kUserByID;
	typedef std::deque<BM::GUID> LIST_REQUEST;	// <RequestGuid>
	LIST_REQUEST m_kRequestList;	// LOGIN request guid list

	// GTDX .....
	DWORD m_dwLastGTDXRequestKey;
	CONT_GTDX_REQUEST m_kGTDXRequest;
};

#define g_kGala SINGLETON_STATIC(PgGalaServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_GALA_GALASERVER_H