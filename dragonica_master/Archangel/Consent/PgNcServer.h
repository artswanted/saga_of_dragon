#ifndef CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCSERVER_H

#include "constant.h"
#include "PgSiteMemberMgr.h"
#include "AilePack/constant.h"
#include "AilePack/NC_Constant.h"
#include "NC/AdminGateway.h"

namespace NCMonUtil
{
	extern bool bUseMonitor;
}

class PgNcServer 
	: public PgSiteMemberMgr
{
public :
	PgNcServer();
	virtual ~PgNcServer();

	static void Excute(const wchar_old* req, wchar_old* res);
	static bool InitAdminGateWay();
	static void UnloadAdminGateWay();
	// Immigration
	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	// SA
	static void CALLBACK OnConnectFromSA( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromSA( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromSA(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 120000;
	static const int GUSID_LENGTH = 16;
	static const short SA_SERVER_SITE_NUM = -1;

	// Immigration
	bool Locked_OnRegistConnector(CEL::SRegistResult const &rkArg);
	void Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void Locked_OnConnectFromImmigration( CEL::CSession_Base *pkSession );

	// SA
	void Locked_OnConnectFromSA( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromSA( CEL::CSession_Base *pkSession );
	void Locked_OnRecvFromSA( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket );

	void Locked_Timer5s();
	void Locked_SetServerState(BM::Stream * const pkPacket);
	void Locked_GetServerState(BM::vstring &rkOut, const int iRealmNo );

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
		short sSite;	// 접속중인 Site
		ACE_UINT64 i64ExpireTime;
		__int64 i64Cash;
		BM::GUID kReqGuid;
		std::string strAnswer;		
	} SUserInfo;

	typedef std::map<std::string, SUserInfo*> CONT_USER_BY_GUSID;
	typedef std::map<unsigned int, SUserInfo*> CONT_USER_BY_USERID;
	typedef std::map<long, NC::SNcCashTranInfo> CONT_CASH_REQ_KEY;

	typedef std::map<int, BM::vstring> CONT_SERVERSTATE_STR;

	CONT_USER_BY_GUSID	m_kContUserByGusId;
	CONT_USER_BY_USERID	m_kContUserByUserId;

	void SendNotifyUserKick(BYTE* pbyGusId, int iUID, BYTE byReason);
protected:
	virtual void Timer5s();

	unsigned long GetCashRequestKey();

	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_NC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);
	void RecvPT_IM_CN_NFY_LOGOUT_NC(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	// cash
	void RecvPT_A_CN_REQ_QUERY_CASH(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_BUYCASHITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);	
	void RecvPT_A_CN_REQ_SENDGIFT(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);
	void RecvPT_A_CN_REQ_DELETE_ITEM(SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket);

	void SendCashResult(BM::Stream& rkPacket);
	void SendLoginResult(CEL::CSession_Base *pkSession, SUserInfo const& kUser, ETryLoginResult const eResult, BYTE* pbyGusID = NULL);	

	CLASS_DECLARATION_S(unsigned int, GameID);
	CLASS_DECLARATION_S(unsigned int, ServerID);
	CLASS_DECLARATION_S(unsigned short, PointCategory);	
	CLASS_DECLARATION_S(unsigned short, CashId);
	CLASS_DECLARATION_S(unsigned short, HappyCoinId);
	CLASS_DECLARATION_S(unsigned short, UnitCoinId);

	CLASS_DECLARATION_S(unsigned int, ItemCount);

private:
	volatile long m_lCashRequestKey;
	CONT_CASH_REQ_KEY m_kContNcCashReqkey;
	CONT_SERVERSTATE_STR m_kContServerStateStr;
	mutable Loki::Mutex m_kMutexMonitor;
};

#define g_kNc SINGLETON_STATIC(PgNcServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_NC_PGNCSERVER_H