#ifndef LOGIN_LOGINSERVER_PGLOGINUSERMGR_H
#define LOGIN_LOGINSERVER_PGLOGINUSERMGR_H

#include "Variant/gm_const.h"
#include "HellDart/PgIPChecker.h"

#define DELETE_SESSION_DELAY 3000

typedef struct tagWaitAutoLogin
{
	tagWaitAutoLogin( SReqSwitchReserveMember const &_kRSRM )
		:	kRSRM(_kRSRM)
		,	dwRegistTime(0)
	{}

	tagWaitAutoLogin()
		:	dwRegistTime(0)
	{}

	SReqSwitchReserveMember	kRSRM;
	DWORD					dwRegistTime;
}SWaitAutoLogin;

class PgLoginUserMgr
	:	public PgIPChecker
{
	typedef enum EValue
	{
		MAX_AUTH_PROCESS_TIME = 10000,
	}eValue;
	typedef std::map< CEL::SESSION_KEY, SAuthInfo >		CONT_LOGIN_USER;// 로그인 처리중인 세션.
	typedef std::map< BM::GUID,	SWaitAutoLogin >		CONT_WAIT_AUTOLOGIN_USER;// AutoLogin 대기 유저
public:
	PgLoginUserMgr();
	virtual ~PgLoginUserMgr();

public:
	HRESULT ProcessAuth(SClientTryLogin const &kCTL, CEL::CSession_Base* pkSession, std::wstring const &kAddonString);//ID 패스 검증.
	HRESULT ProcessAuth_NC(SClientTryLogin const &kCTL, CEL::CSession_Base* pkSession, std::wstring const &kSessKey);//ID 패스 검증.
	HRESULT ProcessAutoAuth( BM::GUID const &kAuthKey, CEL::CSession_Base* pkSession );
	bool SetLoginState( CONT_LOGIN_USER::key_type const &kSessionKey, ELoginState const eLoginState, bool const bNewLogin = true );
	bool ProcessLogin(SClientTryLogin const& rkCTL, CEL::CSession_Base* pkSession );//렐름과 채널을 선택한. 실제 로그인
	HRESULT UpdateAuthInfo(SAuthInfo const &kAuthInfo);
	bool Clear( CEL::CSession_Base* pkSession );	

	BM::GUID InsertAutoLoginWait( SWaitAutoLogin const &kWAL );

	HRESULT GmWaitLogin(SAuthInfo &kGM, CEL::CSession_Base* pkSession);	//GM이 로그인 시도.
	void ProcessGmLogin(BM::Stream * const pkPacket); //GM 로그인 완료
	void ClearGmUserData();
	bool DeleteGmWaitData(std::wstring const &rkID);
	bool DeleteGmWaitData(CEL::CSession_Base const * pkSession);
	bool DeleteGmLoinedData(BM::GUID const &rkGuid);
	bool DeleteGmLoinedData(CEL::CSession_Base const * pkSession);
	
	bool SendToGmUser(BM::GUID kMemberId, BM::Stream const&rkPacket);

	size_t GetCounter()const;

	void DisplayState()const;
	void Tick();
	size_t GetTryLoginUserCount(void)const;

protected:
	eTryLoginResult ProcessAuth_Common(SClientTryLogin &kCTL, CEL::CSession_Base* pkSession, BM::Stream const *pkAddPacket );//ID 패스 검증.
	bool InsertAuth( CEL::CSession_Base const * pkSession, SAuthInfo &kAuthInfo );

protected:
	CONT_LOGIN_USER				m_kContTryLoginUser;//로그인 시도 유저.
	CONT_WAIT_AUTOLOGIN_USER	m_kContWaitAutoLogin;
	CONT_GM_AUTH_DATA_ID		m_kContGmWaitLogin;
	CONT_GM_AUTH_DATA			m_kContGmLogined;
	mutable Loki::Mutex m_kMutex;
};

inline size_t PgLoginUserMgr::GetTryLoginUserCount(void)const
{
	BM::CAutoMutex kLock( m_kMutex );
	return m_kContTryLoginUser.size();
}

#define g_kLoginUserMgr SINGLETON_STATIC(PgLoginUserMgr)

#endif // LOGIN_LOGINSERVER_PGLOGINUSERMGR_H