#ifndef CONSENT_CONSENTSERVER_LINKAGE_COMMON_PGSITEMEMVERMGR_H
#define CONSENT_CONSENTSERVER_LINKAGE_COMMON_PGSITEMEMVERMGR_H

#include "PgRecvFromMMC.h"
#include "SessionMgr.h"

#pragma pack(1)

typedef enum eSiteUserState
{
	ESITE_USER_NONE			= 0x00,
	ESITE_USER_AUTH			= 0x01,
	ESITE_USER_LOGIN		= 0x02,
	ESITE_USER_MOBILELOCK	= 0x04,
	ESITE_USER_CHECK		= ESITE_USER_AUTH|ESITE_USER_LOGIN,
}ESITEUSER_STATE;

typedef enum eSiteUserStateRet
{
	ESITE_USER_NOTFOUND			= 0x00,	// 유저를 찾을 수 없다.
	ESITE_USER_SUCCESS			= 0x01,	// 성공
	ESITE_USER_SUCCESS_REMOVE	= 0x02, // 성공했고 지워졌다.
	ESITE_USER_HAVESTATE		= 0x03,	// 이미 현재 상태이다.
	ESITE_USER_DIFFERENTSITE	= 0x04,	// 인증된 사이트가 상이하다.
}ESITEUSER_STATE_RESULT;

typedef struct tagSiteUserInfo
{
	tagSiteUserInfo()
	{
		nSite = 0;
		byState = ESITE_USER_NONE;
	}
	explicit tagSiteUserInfo( short const _nSite, SAuthSyncData const &kAuthSyncData )
		:	nSite(_nSite)
		,	strID(kAuthSyncData.strID)
		,	kReqGuid(kAuthSyncData.kRequestGuid)
		,	byState(ESITE_USER_LOGIN)
	{}

	explicit tagSiteUserInfo( short const _nSite, SAuthInfo const &kAuthInfo )
		:	nSite(_nSite)
		,	strID(kAuthInfo.ID())
		,	kReqGuid(kAuthInfo.guidQuery)	
		,	byState(ESITE_USER_AUTH)
	{}

	tagSiteUserInfo& operator=(tagSiteUserInfo const& rhs)
	{
		strID = rhs.strID;
		nSite = rhs.nSite;
		kReqGuid = rhs.kReqGuid;
		byState = rhs.byState;
		return *this;
	}

	std::wstring strID;	// Account ID
	short nSite;
	BM::GUID kReqGuid;	// Request ID
	BYTE byState;	// Auth상태이냐?
} SSiteUserInfo;

#pragma pack()

class PgSiteMemberMgr : public PgSessionMgr<short>
{
	typedef std::map< std::wstring, SSiteUserInfo >		CONT_USER_ID;

public :
	static const short IMM_SERVER_SITE_NUM_BEGIN = 1;
	static const short IMM_SERVER_SITE_NUM_END = 100;
	static const short IMM_SERVER_SITE_NUM_ERROR = 0;

	PgSiteMemberMgr(void);
	virtual ~PgSiteMemberMgr(void);

	bool Locked_SendToImmigrationOne( BM::Stream const &kPacket )const;
	bool Locked_OnRegistConnector(CEL::SRegistResult const &rkArg);
	void Locked_OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );

	size_t Locked_GetLoginUserCount(void)const;

protected:
	bool IsServiceReady() { return m_eState == ECN_STATE_READYTOSERVICE; }
	virtual void Timer5s();
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void BroadCast_Immigration( BM::Stream const &kPacket )const;
	SessionKey GetSessionKey( CEL::CSession_Base *pkSession )const;

	// 상속금지
	bool AddUserID( CONT_USER_ID::key_type const &kID, CONT_USER_ID::mapped_type const &kElement, PgSiteMemberMgr::SessionKey &rkOutOldServerNo );
	bool RemoveUserID( CONT_USER_ID::key_type const &kID );
	bool GetUserInfo( CONT_USER_ID::key_type const &kID, CONT_USER_ID::mapped_type &rkOutElement )const;
	bool IsHaveUserID( CONT_USER_ID::key_type const &kID )const;
	ESITEUSER_STATE_RESULT SetUserState( SessionKey const nSessionKey, std::wstring const &wstrID, BYTE const byState, bool const bAdd );

	void RecvSyncUserList( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void RemoveSiteUserList( SessionKey const nSite );


	void Recv_PT_IM_CN_REQ_LOGIN_AUTH( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void Recv_PT_IM_CN_NFY_LOGOUT_AUTH( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	ETryLoginResult Recv_PT_IM_CN_REQ_LOGIN_LOGIN( SessionKey const nSessionKey, SAuthInfo const &kAuthInfo, BYTE const byState=ESITE_USER_LOGIN );
	void Recv_PT_IM_CN_ANS_LOGIN_LOGIN_FAILED( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	void Recv_PT_IM_CN_ANS_USER_TERMINATE_ERROR( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket );
	bool SendToImmigration(std::wstring const& rkAccountID, BM::Stream const& rkPacket);

protected:
	static ACE_UINT64 const IMMIGRATION_TRY_CONNECT_TIMEOUT = 60000i64;	// 60sec 동안 Immigration 초기화 기다린다.
	typedef enum
	{
		ECN_STATE_NONE = 0,
		ECN_STATE_INIT = 1,
		ECN_STATE_READYTOSERVICE = 2,
	} EConsentServerState;
	EConsentServerState m_eState;
	bool m_bDisconnectDuplicatedUser;

private:// private!!!
	//mutable Loki::Mutex	m_kMutex_ID;// m_kContUserID에서만 사용
	CONT_USER_ID	m_kContUserID;
};

inline size_t PgSiteMemberMgr::Locked_GetLoginUserCount(void)const
{
	BM::CAutoMutex kLock( m_kMutex );
	return m_kContUserID.size();
}

//#define g_kSiteMemberMgr SINGLETON_STATIC(PgSiteMemberMgr)

#endif // CONSENT_CONSENTSERVER_LINKAGE_COMMON_PGSITEMEMVERMGR_H