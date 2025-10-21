#ifndef CONSENT_CONSENTSERVER_LINKAGE_GF_PGGFSERVER_H
#define CONSENT_CONSENTSERVER_LINKAGE_GF_PGGFSERVER_H

#include "PgSiteMemberMgr.h"

#pragma pack(1)
struct SMobileAuthElement
{
	static ACE_UINT64 const	MOBILE_AUTH_MAXTIME = 1000 * 100;

	SMobileAuthElement()
		:	u64TimeOutTime(0)
		,	nSite(0)
	{}

	explicit SMobileAuthElement( SAuthInfo const &_kAuthInfo, short const _nSite )
		:	kAuthInfo(_kAuthInfo)
		,	nSite(_nSite)
	{
		u64TimeOutTime = BM::GetTime64() + MOBILE_AUTH_MAXTIME;
	}

	short			nSite;
	ACE_UINT64 		u64TimeOutTime;
	SAuthInfo		kAuthInfo;
};
#pragma pack()

template< typename T_ID, typename T_ELEMENT >
class PgMobileAuth
{
public:
	typedef typename std::map< typename T_ID, typename T_ELEMENT >	CONT;

public:
	PgMobileAuth(void){}
	~PgMobileAuth(void){}

	HRESULT Add( T_ID const &kID, T_ELEMENT const &kElement, bool bUpdate )
	{
		BM::CAutoMutex kLock(m_kMutex);
		auto kPair = m_kCont.insert( std::make_pair( kID, kElement ) );
		if ( !kPair.second )
		{
			if ( true == bUpdate )
			{
				kPair.first->second = kElement;
				return S_FALSE;
			}
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT Remove( T_ID const &kID )
	{
		BM::CAutoMutex kLock(m_kMutex);
		return ( (0 == m_kCont.erase( kID )) ? E_FAIL : S_OK );
	}

	HRESULT Pop( T_ID const &kID, T_ELEMENT &rkOutElement )
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT::iterator itr = m_kCont.find( kID );
		if ( itr != m_kCont.end() )
		{
			rkOutElement = itr->second;
			m_kCont.erase( itr );
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT Get( T_ID const &kID, T_ELEMENT &rkOutElement )const
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT::const_iterator itr = m_kCont.find( kID );
		if ( itr != m_kCont.end() )
		{
			rkOutElement = itr->second;
			return S_OK;
		}
		return E_FAIL;
	}

	Loki::Mutex &GetLock()const{return m_kMutex;}
	typename CONT::const_iterator Begin()const{return m_kCont.begin();}
	typename CONT::const_iterator End()const{return m_kCont.end();}

private:
	mutable Loki::Mutex	m_kMutex;
	CONT				m_kCont;

private://사용금지
	PgMobileAuth( PgMobileAuth const & );
	PgMobileAuth& operator = ( PgMobileAuth const & );
};

class PgGFServer : public PgSiteMemberMgr
{
public :
	PgGFServer(void);
	virtual ~PgGFServer(void);

	static const short MOBILELOCK_SERVER_NUM = -2;

	static void CALLBACK OnConnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisconnectFromImmigration( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	static void CALLBACK OnConnectFromMobileLock( CEL::CSession_Base *pkSession );
	static void CALLBACK OnDisConnectFromMobileLock( CEL::CSession_Base *pkSession );
	static void CALLBACK OnRecvFromMobileLock(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket);

	bool Locked_OnRegistConnector( CEL::SRegistResult const &rkArg );

	void Locked_OnConnectFromMobileLock( CEL::CSession_Base *pkSession );
	void Locked_OnDisconnectFromMobileLock( CEL::CSession_Base *pkSession );
	void Locked_CheckMobileLockWait();
	void Locked_RecvFromMobileLock( BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket );
	void Locked_Timer5s();
	void Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);

	void Locked_LoadFromConfig(void);
protected:
	void RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket);
	void LoadFromConfig(void);

private: 
	PgMobileAuth< BM::GUID, SMobileAuthElement >		m_kContMobileLockWait;	// first - Session GUID
	bool												m_bUseMobileLock;
};

#define g_kGFServer SINGLETON_STATIC(PgGFServer)

#endif // CONSENT_CONSENTSERVER_LINKAGE_GF_PGGFSERVER_H