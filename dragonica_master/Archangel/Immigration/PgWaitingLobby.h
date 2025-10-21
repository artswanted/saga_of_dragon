#ifndef IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_MAINOBJ_PGWAITINGLOBBY_H
#define IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_MAINOBJ_PGWAITINGLOBBY_H

#include "Loki/Threads.h"
#include "Loki/Singleton.h"

typedef enum eWaitMemberReturn
{
	WMR_NONE = 0,
	WMR_AUTH = 1,
	WMR_SWITCH,
	WMR_LOGINED,
} EWaitMemberReturn;

#pragma pack(1)
typedef struct tagLoginedUser//! 센터서버에 등록되는 유저 정보
{
	tagLoginedUser()
	{
		Clear();
	}

	void Clear()
	{
		guidMember = BM::GUID::NullData();
		ucGender = 0;
		kSwitchServer.Clear();
		kGroundKey.Set(0, BM::GUID::NullData());
		guidCharacter = BM::GUID::NullData();
		strID = L"";

		i64TotalConnSec = 0;
		iAccConSec = 0;
		iAccDisSec = 0;
		gusID[0] = 0;
	}

	tagLoginedUser( tagSwitchPlayerData const &rhs )
	{
		Clear();
		strID = rhs.ID();
		guidMember = rhs.guidMember;
		ucGender = rhs.ucGender;
		kSwitchServer = rhs.kSwitchServer;
		addrRemote = rhs.addrRemote;
		i64TotalConnSec = rhs.i64TotalConnSec;
		dtLastLogin = rhs.dtLastLogin;
		dtLastLogout = rhs.dtLastLogout;
		dtNowDbTime = rhs.dtNowDbTime;
		::memcpy( gusID, rhs.gusID, sizeof(gusID));
		i64TotalConnSec = rhs.i64TotalConnSec;
		iAccConSec = rhs.iAccConnSec;
		iAccDisSec = rhs.iAccDisConnSec;
	}
	
	BM::GUID guidMember;	//member의 guid
	unsigned char ucGender; //남녀 구분
	SERVER_IDENTITY kSwitchServer;	//사용중인 switch의 번호
	
	SGroundKey kGroundKey;	//사용중인 맵 정보

	std::wstring strID;			//시도ID
	CEL::ADDR_INFO addrRemote;	//유저의주소.
	
	BM::GUID guidCharacter;	//선택된 캐릭터의의 guid

	__int64 i64TotalConnSec;
	int		iAccConSec;
	int		iAccDisSec;

	BM::DBTIMESTAMP_EX dtLastLogin;
	BM::DBTIMESTAMP_EX dtLastLogout;
	BM::DBTIMESTAMP_EX dtNowDbTime;

	BYTE gusID[16];//NC만 사용
}SLoginedUser;
#pragma pack()

class PgWaitingLobby
{
	static DWORD const WATINGUSER_MAX_DELAYTIME = ( RESERVEMEMBER_TIMEOUT * 2 );

	friend struct ::Loki::CreateStatic< PgWaitingLobby >;

	typedef std::map< CEL::SESSION_KEY, SAuthInfo > CONT_WAIT_AUTH;//ID 인증 대기줄 key = ID
	typedef std::map< BM::GUID, SReqSwitchReserveMember > CONT_SWITCH_WAIT;// 스위치에 애 할당해달라고 보내놓은 대기줄( first = 유저Member Key로 )
	
	typedef std::map< BM::GUID, SLoginedUser* > CONT_CENTER_PLAYER_BY_KEY;// first key : member guid
	typedef std::map< std::wstring, SLoginedUser* > CONT_CENTER_PLAYER_BY_ID;

protected:
	PgWaitingLobby(void);
	virtual ~PgWaitingLobby(void);

public:
	void Recv_PT_T_IM_REQ_SWITCH_USER_ADD(SSwitchPlayerData const &kSPD, CEL::CSession_Base *pkSession);//스위치에 접속해서 로그인했음.
	bool Recv_PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED( BM::GUID const &kMemberGuid, ETryLoginResult const eRet );//스위치 할당 실패

	void ProcessClearAuthWaiting(CEL::SESSION_KEY const &kWaiterKey, BM::GUID const &kMemberGuid, ELoginState eLoginState );
	short ProcessLogout( BM::GUID const &kMemberGuid, bool const bSendContents=false, bool const bAutoLogin=false);
	bool ProcessUserTerminate( std::wstring const &wstrID, EClientDisconnectedCause const kCause, bool const bDisConnected = true, BYTE byReason = 0);
	void ProcessPlayTime(BM::GUID const &rkMemberGuid, BM::Stream * const pkPacket);
	void ProcessResetPlayTime();
	void ProcessSetPlayTime(std::wstring const & kID, int const iAccConSec, int const iAccDisSec);
	void ProcessNotifyCash(BM::Stream* const pkPacket);

	std::wstring kSesskey;
	CEL::ADDR_INFO kAddr;

	HRESULT ProcessTryAuth( SAuthInfo& kAuthInfo, bool const bIsAutoLogin, std::wstring const &kStrAddonMsg = L"" );
	ETryLoginResult ProcessTryLogin( SAuthInfo const &rkAuthInfo, bool const bRecvConsent );	//로그인 시도. --> TryLoginResult 를 날릴 수 있다.

	ETryLoginResult RecvPT_CN_IM_ANS_LOGIN_AUTH_NC(BM::Stream* const pkPacket); //로그인 시도. --> TryLoginResult 를 날릴 수 있다.
	ETryLoginResult RecvPT_CN_IM_ANS_LOGIN_AUTH_GRAVITY(BM::Stream* const pkPacket); //로그인 시도. --> TryLoginResult 를 날릴 수 있다.
	void RecvPT_CN_IM_ANS_LOGIN_GALA(BM::Stream* const pkPacket);


	void CheckTimeOverUser();

	//bool GetAuthWaitWithQueryGuid(BM::GUID const& rkQueryGuid, SAuthInfo& rkInfo);
	//bool IsAuthWaitWithQueryGuid( BM::GUID const& rkQueryGuid )const;

	HRESULT Q_DQT_TRY_AUTH(CEL::DB_RESULT &rkResult);//--> TryLoginResult 를 날릴 수 있다.
	void Q_DQT_DISCONNECT_OLDLOGIN( CEL::DB_RESULT &rkResult );
	HRESULT Q_DQT_CREATE_ACCOUNT(CEL::DB_RESULT &rkResult);//--> TryLoginResult 를 날릴 수 있다.
	bool Q_DQT_UPDATE_MEMBER_LOGOUT(CEL::DB_RESULT &rkResult);
	bool Q_DQT_UPDATE_CONNECTION_CHANNEL(CEL::DB_RESULT &rkResult);
	bool Q_DQT_CLEAR_CONNECTION_CHANNEL(CEL::DB_RESULT &rkResult);
	bool Q_DQT_MEMBER_LOGINED(CEL::DB_RESULT &rkResult);
	bool Q_DQT_SAVE_MEMBER_1ST_LOGINED(CEL::DB_RESULT& rkResult);
	bool Q_DQT_USER_CREATE_ACCOUNT_GALA( CEL::DB_RESULT &rkResult );
	bool Q_DQT_UPDATE_MEMBER_PW( CEL::DB_RESULT &rkResult );
	bool Q_DQT_LOAD_DEF_PLAYERPLAYTIME(CEL::DB_RESULT &rkResult);
	bool Q_DQT_UPDATE_RESETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult);
	HRESULT Q_DQT_TRY_AUTH_CHECKPW( CEL::DB_RESULT &rkResult);
	bool Q_DQT_TRY_AUTH_CHECKPW_OLDLOGIN(CEL::DB_RESULT &rkResult);

	void DisconnectServer(SERVER_IDENTITY const &kRecvSI);
	void RecvPT_T_IM_NFY_USERLIST( SERVER_IDENTITY const &kRealmSI, VEC_SwitchPlayerData const &kPlayerVector );

//	bool FindConnectUser(BM::GUID const &kMemberId, SLoginedUser &rkOut);
//	bool FindConnectUser(std::wstring const &kId, SLoginedUser &rkOut);

	bool IsConnectUser( BM::GUID const &kMemberID )const;
	bool IsConnectUser( std::wstring const &kID )const;
	bool GetConnectUserSwitch( BM::GUID const &kMemberGUID, SERVER_IDENTITY &kSwitchInfo )const;
	bool GetConnectUserSwitch( std::wstring const &kID, SERVER_IDENTITY &kSwitchInfo, BM::GUID &rkOutMemberGuid )const;
	bool GetMemberGuid( std::wstring const &kID, BM::GUID &kMemberGuid )const;

	bool InitUserConnectionChannel(short RealmNo,  short ChannelNo, bool bDoNow = true);
	void DisplayState();

	bool SendTryLoginResult(wchar_t const* szFunc, size_t const iLine, SAuthInfo const& rkAuthInfo, int const eRet, std::wstring const& strErrorMsg = std::wstring())const;

	void Locked_WriteToPacket_LoginUserInfoToConsent( BM::Stream &rkPacket )const;
	size_t Locked_GetLoginUserCount(void)const;

protected:
	HRESULT AddLoginedUser(SSwitchPlayerData const &kSPD, bool bRealLogin = true); 

	HRESULT InsertAuthWait( CONT_WAIT_AUTH::key_type const &kKey, CONT_WAIT_AUTH::mapped_type const &kElement );
	void ClearUserData( CONT_CENTER_PLAYER_BY_KEY::mapped_type &pData );
	void NCLogOut( BYTE* pbyGusId ); // NC전용: Consent를 통해 GSM 서버에 LogOut을 알림
	void GravityLogOut( const std::wstring& kUserId );

	CLASS_DECLARATION_S_NO_SET(bool, IsServerOpen);
	CLASS_DECLARATION(bool, m_bNewTryLoginDisconnectAll, IsNewTryLoginDisconnectAll);
	CLASS_DECLARATION(bool, m_bUseJoinSite, UseJoinSite)// 사이트를 조인(멤버디비공유 중복로그인 체크를 해야 함)

	CLASS_DECLARATION(short, m_nHaveExtVar, HaveExtVar);
	CLASS_DECLARATION(bool, m_bUseExtVar, UseExtVar);
public:
	void SetBlockedLogin( bool const bBlocked );
	bool IsBlockedLogin(void){return m_bBlockedLogin;}

protected:
	mutable Loki::Mutex m_kWaitMutex;
	CONT_WAIT_AUTH m_kContAuthWait;//멤버 ID 인증 대기줄
	CONT_SWITCH_WAIT m_kContSwitchWait;//스위치 할당을 기다리는 줄
	
//	mutable Loki::Mutex m_kMutexLoginedUser;// LOCK(m_kContLoginedUserKey + m_kContLoginedUserID)
	CONT_CENTER_PLAYER_BY_KEY m_kContLoginedUserKey;//사용중인 사람.
	CONT_CENTER_PLAYER_BY_ID m_kContLoginedUserID;//사용중인 사람.

	bool m_bBlockedLogin;
	BM::GUID const m_kMgrGuid;
};

inline size_t PgWaitingLobby::Locked_GetLoginUserCount(void)const
{
	BM::CAutoMutex kLock( m_kWaitMutex );
	return m_kContLoginedUserKey.size();
}

typedef Loki::SingletonHolder< PgWaitingLobby, ::Loki::CreateStatic > Singleton_WaitingLobby;

#define g_kWaitingLobby Singleton_WaitingLobby::Instance()

#endif // IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_MAINOBJ_PGWAITINGLOBBY_H