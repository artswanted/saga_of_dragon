#ifndef CONSENT_CONSENTSERVER_NETWORK_SESSIONMGR_H
#define CONSENT_CONSENTSERVER_NETWORK_SESSIONMGR_H

#pragma pack(1)

typedef enum : BYTE
{
	EConnectState_Disconnected		= 0,
	EConnectState_TryConnect	= 1,
	EConnectState_Connected		= 2,
	EConnectState_Ready,
} EConnectState;

typedef struct _AddrInfo{
	_AddrInfo()
		: byState(EConnectState_Disconnected)
	{};
	_AddrInfo(std::wstring const & rkIP, WORD const wPort)
		: byState(EConnectState_Disconnected), kAddr(rkIP, wPort)
	{};

	_AddrInfo const& operator =(_AddrInfo const & rhs)
	{
		byState = rhs.byState;
		kAddr = rhs.kAddr;
		return (*this);
	}

	BYTE byState;
	CEL::ADDR_INFO kAddr;

} AddrInfo;

typedef struct _SessionInfo
{
	_SessionInfo() {};
	_SessionInfo(_SessionInfo const &rhs)
	{
		kAddress = rhs.kAddress;
		kSessionKey = rhs.kSessionKey;
		eNationCode = rhs.eNationCode;
		pkSession = rhs.pkSession;
	}

	explicit _SessionInfo(AddrInfo const &rkAddr, BM::GUID const & kConnector)
	{
		kAddress = rkAddr;
		kSessionKey.WorkerGuid(kConnector);
		pkSession = NULL;
		eNationCode = LOCAL_MGR::NC_NOT_SET;
	}

	_SessionInfo const& operator =(_SessionInfo const &rhs)
	{
		kAddress = rhs.kAddress;
		kSessionKey = rhs.kSessionKey;
		pkSession = rhs.pkSession;
		eNationCode = rhs.eNationCode;
		return (*this);
	}

	AddrInfo kAddress;
	CEL::SESSION_KEY kSessionKey;
	LOCAL_MGR::NATION_CODE eNationCode;
	CEL::CSession_Base* pkSession;
} SessionInfo;

#pragma pack()

template<typename T_SESSION_KEY = short>
class PgSessionMgr
{
public:
	PgSessionMgr();
	virtual ~PgSessionMgr();

	typedef typename T_SESSION_KEY SessionKey;

	// Public 함수는 무조건 Locked_XXX
	void Locked_TryConnect();
	void Locked_OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	//void Locked_OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	bool Locked_SendToServer( T_SESSION_KEY const &kKey, BM::Stream const &kPacket )const;

protected:
	void OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	virtual void Timer5s() = 0;
	bool AddAddress(T_SESSION_KEY const& kKey, BM::GUID const & kConnector, std::wstring const & rkIP, WORD const wPort);
	void RemoveAddress( T_SESSION_KEY const& kKey );
	bool TryConnect(void);
	bool SendToServer( T_SESSION_KEY const &kKey, BM::Stream const &kPacket )const;

	bool OnConnected( CEL::CSession_Base *pkSession );
	void OnDisconnected( CEL::CSession_Base *pkSession );
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);

	SessionInfo* const GetSessionInfo(CEL::ADDR_INFO const & rkAddr);
	SessionInfo* const GetSessionInfo(CEL::CSession_Base const * const pkSession);
	bool GetSessionKey(CEL::CSession_Base const * const pkSession, T_SESSION_KEY &rkOutKey ) const;
	HRESULT SendPacket(T_SESSION_KEY kKey, BM::Stream const& rkPacket);

	void LoadImmigration( BM::GUID const &kConnectorID, std::wstring const &wstrConfigFileName ); 

protected:

	typedef std::map<SessionKey, SessionInfo> CONT_SESSION;	// <T_SESSION_KEY, SessionInfo>
	CONT_SESSION m_kSession;

	// Lock 은 Drived Class 에서 처리하라.
	mutable Loki::Mutex m_kMutex;
};

#include "SessionMgr.hpp"

//#define g_kSessionMgr SINGLETON_STATIC(PgSessionMgr)

#endif // CONSENT_CONSENTSERVER_NETWORK_SESSIONMGR_H