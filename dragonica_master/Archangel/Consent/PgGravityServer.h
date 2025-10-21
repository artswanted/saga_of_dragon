#ifndef CONSENT_CONSENTSERVER_LINKAGE_GRAVITY_PGGRAVITYSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_GRAVITY_PGGRAVITYSERVER_H

#include "PgSiteMemberMgr.h"
#include "AilePack/Gravity_Constant.h"

class PgGravityServer
	: public PgSiteMemberMgr
{
public:

	static const ACE_UINT64 MAX_REQUEST_WAITINGTIME = 120000;

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
	
	typedef std::map<std::wstring, SUserInfo*> CONT_USER_BY_USERID;
	typedef std::map<unsigned long, GRAVITY::SGravityCashTranInfo> CONT_CASH_TRAN_INFO;
	typedef std::vector< std::string > CONT_USERBLOCK_INFO;

private:
	CONT_USER_BY_USERID	m_kContUserByUserId;
	CONT_CASH_TRAN_INFO	m_kContCashTranInfo;
	CONT_USERBLOCK_INFO	m_kContUserBlockInfo;
	volatile long		m_TranId;

public:
	PgGravityServer();
	virtual ~PgGravityServer();

	void SendLoginResult( CEL::CSession_Base* pkSession, const SUserInfo& rkUser, ETryLoginResult const eResult );
	void Locked_Timer5s();
	bool Locked_OnRegistConnector( const CEL::SRegistResult& rkArg );

	// Immgration
	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base* pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base* pkSession );
	static void CALLBACK OnRecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void Locked_OnConnectFromImmigration( CEL::CSession_Base* pkSession );
	void Locked_OnDisconnectFromImmigration( CEL::CSession_Base* pkSession );
	void Locked_OnRecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream* const pkPacket );

	// Auth
	static void CALLBACK OnConnectFromGravityAuth( CEL::CSession_Base* pkSession );
	static void CALLBACK OnDisconnectFromGravityAuth( CEL::CSession_Base* pkSession );
	static void CALLBACK OnRecvFromGravityAuth( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void Locked_OnConnectFromGravityAuth( CEL::CSession_Base* pkSession );
	void Locked_OnDisconnectFromGravityAuth( CEL::CSession_Base* pkSession );
	void Locked_OnRecvFromGravityAuth( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );

	// Item
	static void CALLBACK OnConnectFromGravityItem( CEL::CSession_Base* pkSession );
	static void CALLBACK OnDisconnectFromGravityItem( CEL::CSession_Base* pkSession );
	static void CALLBACK OnRecvFromGravityItem( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void Locked_OnConnectFromGravityItem( CEL::CSession_Base* pkSession );
	void Locked_OnDisconnectFromGravityItem( CEL::CSession_Base* pkSession );
	void Locked_OnRecvFromGravityItem( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );

	// GM
	static void CALLBACK OnConnectFromGravityGM( CEL::CSession_Base* pkSession );
	static void CALLBACK OnDisconnectFromGravityGM( CEL::CSession_Base* pkSession );
	static void CALLBACK OnRecvFromGravityGM( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void Locked_OnConnectFromGravityGM( CEL::CSession_Base* pkSession );
	void Locked_OnDisconnectFromGravityGM( CEL::CSession_Base* pkSession );
	void Locked_OnRecvFromGravityGM( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	
protected:
	void Timer5s();
	void KeepAlive();
	unsigned long GetTranId();
	void RemoveUserInfo( const std::wstring& rkUserId );
	void RemoveUserInfoAll();	
	ECashShopResult GetCashResult( const GRAVITY::EGravityItemResult eResult );

	void RecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream* const pkPacket );

	void RecvPT_IM_CN_NFY_BLOCKUSER_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void RecvPT_IM_CN_NFY_LOGOUT_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void RecvPT_IM_CN_REQ_LOGIN_AUTH_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket );
	void RecvPT_A_CN_REQ_QUERY_CASH( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket );
	void RecvPT_A_CN_REQ_BUYCASHITEM( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket );
	void RecvPT_A_CN_REQ_SENDGIFT( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket );	
};

#define g_kGravity SINGLETON_STATIC( PgGravityServer )

#endif // CONSENT_CONSENTSERVER_LINKAGE_GRAVITY_PGGRAVITYSERVER_H