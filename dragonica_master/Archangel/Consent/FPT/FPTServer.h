#ifndef CONSENT_CONSENTSERVER_LINKAGE_FPT_FPTSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_FPT_FPTSERVER_H

#include "constant.h"
#include "PgSiteMemberMgr.h"
#include "AilePack/constant.h"
#include "AilePack/FPT_constant.h"

#pragma pack(1)

#pragma pack()

class PgFPTServer : public PgSiteMemberMgr
{
public :
	PgFPTServer();
	virtual ~PgFPTServer();

	// STATIC FUNCTION
	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromAuthHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static bool GetXMLFromHttpMsg(std::string const& rkInput, std::string& rkXML);
	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 120000;
	static const std::string FPT_DRAGONICA_GAMEID;
	static const std::string FPT_GAMEPASSWORD;;


	typedef struct _SUserInfo
	{
		explicit _SUserInfo(short const &_Site, SAuthRequest const& _AuthInfo)
		{
			kAuthInfo = _AuthInfo;
			sSite = _Site;
			i64ExpireTime = BM::GetTime64() + MAX_REQUEST_WAITINGTIME;
			i64Cash = 0;
			kReqGuid.Generate();
		}

		_SUserInfo const operator=(_SUserInfo const& rhs)
		{
			kAuthInfo = rhs.kAuthInfo;
			sSite = rhs.sSite;
			i64ExpireTime = rhs.i64ExpireTime;
			i64Cash = rhs.i64Cash;
			kReqGuid = rhs.kReqGuid;
			strAnswer = rhs.strAnswer;
		}

		SAuthRequest kAuthInfo;
		short sSite;	// БўјУБЯАО Site
		ACE_UINT64 i64ExpireTime;
		__int64 i64Cash;
		BM::GUID kReqGuid;
		std::string strAnswer;
	} SUserInfo;

	typedef enum
	{
		EREQ_TYPE_NONE = 0,
		EREQ_TYPE_LOGIN = 1,
		EREQ_TYPE_MAX = EREQ_TYPE_LOGIN+1,
	} EREQ_TYPE;

	typedef std::map<BM::GUID, SUserInfo*> CONT_REQUEST;	// <RequestGuid, SUserInfo>
	typedef std::map<std::wstring, SUserInfo*> CONT_ID;	// <RequestGuid, SUserInfo>

	bool Locked_OnRegistConnector(CEL::SRegistResult const &rkArg);
	void Locked_OnConnectFromAuthHttp( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromAuthHttp( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromAuthHttp(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);

	void Locked_Timer5s();

protected:
	virtual void Timer5s();
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_GALA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_NFY_LOGOUT_LOGIN(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

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

	ETryLoginResult GetLoginResult(FPT::EAUTH_ERROR const eError) const;
	HRESULT LoadXMLFiles();
	HRESULT LoadXMLFile(EREQ_TYPE const eType, std::wstring const& strFilename);
	bool GetRequestXml(EREQ_TYPE const eType, std::string& rkOut) const;

private:
	// Login HTTP
	CLASS_DECLARATION_S(BM::GUID, AuthHttpConnector);
	CEL::ADDR_INFO m_kAuthHttpAddress;
	std::wstring m_strAuthHttpHostname;


	CONT_REQUEST m_kUserByRequest;
	CONT_ID	m_kUserByID;
	typedef std::deque<BM::GUID> LIST_REQUEST;	// <RequestGuid>
	LIST_REQUEST m_kLoginRequestList;	// LOGIN request guid list
	typedef std::vector<std::string> VECTOR_REQXML;
	VECTOR_REQXML m_kReqXml;
};



#define g_kFPT SINGLETON_STATIC(PgFPTServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_FPT_FPTSERVER_H