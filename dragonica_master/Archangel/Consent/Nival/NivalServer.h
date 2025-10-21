#ifndef CONSENT_CONSENTSERVER_LINKAGE_NIVAL_NIVALSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_NIVAL_NIVALSERVER_H

#include "constant.h"
#include "PgSiteMemberMgr.h"
#include "AilePack/constant.h"
#include "AilePack/Nival_constant.h"

#pragma pack(1)

#pragma pack()

class PgNivalServer : public PgSiteMemberMgr
{
public :
	PgNivalServer();
	virtual ~PgNivalServer();

	// STATIC FUNCTION
	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromAuthHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromCashHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromCashHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromCashHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static bool GetXMLFromHttpMsg(std::string const& rkInput, std::string& rkXML);
	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 120000;
	static const int NIVAL_DRAGONICA_GAMEID = 8;
	static const std::string NIVAL_GAMEPASSWORD;;
	static const int NIVAL_ROLLBACK_ITEMID = 999999;


	typedef struct _SUserInfo
	{
		explicit _SUserInfo(short const &_Site, SAuthRequest const& _AuthInfo)
		{
			kAuthInfo = _AuthInfo;
			sSite = _Site;
			i64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
			i64Cash = 0;
		}

		_SUserInfo const operator=(_SUserInfo const& rhs)
		{
			kAuthInfo = rhs.kAuthInfo;
			sSite = rhs.sSite;
			i64ExpireTime = rhs.i64ExpireTime;
			i64Cash = rhs.i64Cash;
			strAnswer = rhs.strAnswer;
		}

		SAuthRequest kAuthInfo;
		short sSite;	// 접속중인 Site
		ACE_UINT64 i64ExpireTime;
		__int64 i64Cash;
		std::string strAnswer;
	} SUserInfo;

	typedef enum
	{
		EREQ_TYPE_NONE = 0,
		EREQ_TYPE_LOGIN = 1,
		EREQ_TYPE_GET_USERMONEY,
		EREQ_TYPE_REMOVE_USERMONEY,
		EREQ_TYPE_TRANSFER_USERMONEY,
		EREQ_TYPE_ADD_USERMONEY,
		EREQ_TYPE_MAX = EREQ_TYPE_ADD_USERMONEY+1,
	} EREQ_TYPE;

	typedef struct _SCashRequest
	{
		explicit _SCashRequest(SERVER_IDENTITY_SITE const& _Site, std::wstring const& _AccountID, EREQ_TYPE _Type, CASH::SCashCost const& _Cost, int const _ItemNo, 
			CEL::ADDR_INFO const _Addr ,PACKET_ID_TYPE const _Packet, BM::Stream const* pkAddon)
			: kSite(_Site), strAccountID(_AccountID), eReqType(_Type), kCost(_Cost), iItemID(_ItemNo), kAddress(_Addr), wReqPacket(_Packet)
		{
			BM::vstring::ConvToLWR(strAccountID);	// Nival은 소문자로 처리 되어야 한다.
			kReqGuid = BM::GUID::Create();
			if (pkAddon != NULL)
			{
				kAddonPacket.Push(*pkAddon);
			}
			i64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
		}

		explicit _SCashRequest(_SCashRequest const& rhs)
			: kSite(rhs.kSite), strAccountID(rhs.strAccountID), eReqType(rhs.eReqType), kCost(rhs.kCost), iItemID(rhs.iItemID), kAddress(rhs.kAddress), wReqPacket(rhs.wReqPacket)
		{
			kReqGuid = rhs.kReqGuid;
			kAddonPacket = rhs.kAddonPacket;
			i64ExpireTime = rhs.i64ExpireTime;
			strAnswer = rhs.strAnswer;
		}

		_SCashRequest const& operator=(_SCashRequest const& rhs)
		{
			strAccountID = rhs.strAccountID;
			kSite = rhs.kSite;
			kReqGuid = rhs.kReqGuid;
			eReqType = rhs.eReqType;
			kCost = rhs.kCost;
			kAddonPacket = rhs.kAddonPacket;
			i64ExpireTime = rhs.i64ExpireTime;
			iItemID = rhs.iItemID;
			kAddress = rhs.kAddress;
			wReqPacket = rhs.wReqPacket;
			strAnswer = rhs.strAnswer;
			return (*this);
		}

		std::wstring strAccountID;
		BM::GUID kReqGuid;
		EREQ_TYPE eReqType;
		CASH::SCashCost kCost;
		BM::Stream kAddonPacket;
		SERVER_IDENTITY_SITE kSite;
		ACE_UINT64 i64ExpireTime;
		int iItemID;
		CEL::ADDR_INFO kAddress;
		PACKET_ID_TYPE wReqPacket;
		std::string strAnswer;
	} SCashRequest;

	typedef std::map<BM::GUID, SUserInfo*> CONT_REQUEST;	// <RequestGuid, SUserInfo>
	typedef std::map<std::wstring, SUserInfo*> CONT_ID;	// <RequestGuid, SUserInfo>
	typedef std::map<BM::GUID, SCashRequest*> CONT_CASHREQUEST;	// <RequestGuid, SCashRequest*>

	bool Locked_OnRegistConnector(CEL::SRegistResult const &rkArg);
	void Locked_OnConnectFromAuthHttp( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void Locked_OnConnectFromCashHttp( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromCashHttp( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromCashHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	void Locked_Timer5s();

protected:
	virtual void Timer5s();
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_BUYCASHITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_EXPANDTIEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_SENDGIFT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_CASH_ROLLBACK(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_OM_BUY_ARTICLE(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_ADDCASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_OM_REG_ARTICLE(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);

	void SetAuthServer(std::wstring const &strAddress, WORD wPort);
	bool SendLoginRequest(CEL::CSession_Base *pkSession);
	void ParsingLoginAnswer(BM::GUID const& rkRequest);
	void RemoveUserInfo(BM::GUID const& rkRequest);
	void SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult);
	ETryLoginResult MakeLoginRequestBody(SUserInfo const& rkUser, std::string& rkOutBody);
	template<typename T>
	HRESULT GetXMLValue(TiXmlNode const* pkNode, std::string const& kItemName, T& rkOut)
	{
		if (pkNode == NULL)
		{
			rkOut = T();
			return E_FAIL;
		}
		TiXmlNode const* pkFindNode = pkNode->FirstChild(kItemName.c_str());
		if (pkFindNode == NULL)
		{
			rkOut = T();
			return E_FAIL;
		}
		TiXmlElement const* pkFindElement = pkFindNode->ToElement();
		if (pkFindElement == NULL)
		{
			rkOut = T();
			return E_FAIL;
		}
		char const * pkFindStr = pkFindElement->GetText();
		if (pkFindStr == NULL)
		{
			rkOut = T();
			return E_FAIL;
		}

		BM::vstring vFind(pkFindStr);
		T kTemp(vFind);
		rkOut = kTemp;
		return S_OK;
	}

	template<typename T>
	HRESULT GetCDATAValue(std::wstring const& kData, T& rkOut)
	{
		std::wstring const strHead(_T("![CDATA["));
		std::wstring const strTail(_T("]]"));
		std::wstring::size_type iBegin = kData.find(strHead);
		if (iBegin == std::wstring::npos)
		{
			return E_FAIL;
		}
		iBegin += strHead.length();
		std::wstring::size_type iEnd = kData.find_last_of(strTail);
		if (iEnd == std::wstring::npos)
		{
			return E_FAIL;
		}
		if (iBegin >= iEnd || iBegin >= kData.length())
		{
			return E_FAIL;
		}

		BM::vstring vFind(kData.substr(iBegin, iEnd-iBegin));
		T kTemp(vFind);
		rkOut = kTemp;
		return S_OK;
	}
	ETryLoginResult GetLoginResult(NIVAL::EAUTH_ERROR const eError) const;
	HRESULT LoadXMLFiles();
	HRESULT LoadXMLFile(EREQ_TYPE const eType, std::wstring const& strFilename);
	bool GetRequestXml(EREQ_TYPE const eType, std::string& rkOut) const;
	bool AddCashRequest(SCashRequest const& rkReq);
	void SetCashServer(std::wstring const &strAddress, WORD wPort);
	void ParsingCashAnswer(BM::GUID const& rkRequest);
	bool SendCashRequest(CEL::CSession_Base *pkSession);
	bool RemoveCashRequest(CONT_CASHREQUEST::key_type const& rkKey);
	HRESULT ConvertXMLVariable(std::string& kXML, SCashRequest const& rkReq, SSiteUserInfo const& rkUserInfo) const;
	void SendCashResult(NIVAL::SCashResult const& kResult);
	bool SendCashResult(SERVER_IDENTITY_SITE const& rkFromSI, BM::Stream const& rkPacket);
	void OnCashRecvGetUserMoney(SCashRequest const* pkReq, std::string const& strXML);
	void OnCashRecvRemoveUserMoney(SCashRequest const* pkReq, std::string const& strXML);
	void OnCashRecvTransferUserMoney(SCashRequest const* pkReq, std::string const& strXML);
	void OnCashRecvAddUserMoney(SCashRequest const* pkReq, std::string const& strXML);

private:
	// Login HTTP
	CLASS_DECLARATION_S(BM::GUID, AuthHttpConnector);
	CEL::ADDR_INFO m_kAuthHttpAddress;
	std::wstring m_strAuthHttpHostname;

	// Cash HTTP
	CLASS_DECLARATION_S(BM::GUID, CashHttpConnector);
	CEL::ADDR_INFO m_kCashHttpAddress;
	std::wstring m_strCashHttpHostname;


	CONT_REQUEST m_kUserByRequest;
	CONT_ID	m_kUserByID;
	typedef std::deque<BM::GUID> LIST_REQUEST;	// <RequestGuid>
	LIST_REQUEST m_kLoginRequestList;	// LOGIN request guid list
	typedef std::vector<std::string> VECTOR_REQXML;
	VECTOR_REQXML m_kReqXml;
	CONT_CASHREQUEST m_kCashRequest;
	LIST_REQUEST m_kCashRequestList;	// Cash request guid list
};



#define g_kNival SINGLETON_STATIC(PgNivalServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_NIVAL_NIVALSERVER_H