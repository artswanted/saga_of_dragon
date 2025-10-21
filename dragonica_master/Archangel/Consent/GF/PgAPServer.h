#pragma once

typedef enum
{
	LST_NONE			= 0,
	LST_BILLING_AP		= 1,
	LST_MOBILE_LOCK		= 2,
}ELINKAGE_SERVER_TYPE;

typedef enum
{
	REQ_ORDER_SUCCESS				= S_OK,
	REQ_ORDER_SERVERERROR			= E_HANDLE,
	REQ_ORDER_DATAERROR_STRINGSIZE	= E_ABORT,
	REQ_ORDER_DATAERROR_PACKETSIZE	= E_FAIL,	
}E_REQUEST_ORDER_RET;

typedef struct tagLinkageServerInfo
{
	tagLinkageServerInfo()
		:	kType(LST_NONE)
		,	bAccept(true)
	{}

	void SetAccept( bool const bTrue )
	{
		bAccept = bTrue;
		if ( !bAccept )
		{
			wstrID.clear();
			wstrPW.clear();
		}
	}

	ELINKAGE_SERVER_TYPE	kType;
	CEL::ADDR_INFO			kAddr;
	std::wstring			wstrID;
	std::wstring			wstrPW;

	bool					bAccept;
}SLinkageServerInfo;

class PgAPServer_Base
{
public:
	explicit PgAPServer_Base( CEL::CSession_Base * const pkSession, SLinkageServerInfo const &kServerInfo );
	explicit PgAPServer_Base( PgAPServer_Base const & );
	virtual ~PgAPServer_Base( void ){}

public:
	SLinkageServerInfo const &GetServerInfo()const{return m_kServerInfo;}
	BM::GUID GetSessionID()const;

	bool Send( BM::Stream const &kPacket )const;

public:
	virtual HRESULT ProcessPacket( BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream &rkPacket ) = 0;
	virtual void Update(){}

protected:
	CEL::CSession_Base * const	m_pkSession;
	SLinkageServerInfo const	m_kServerInfo;

private:
	// 사용금지
	PgAPServer_Base(void);
	PgAPServer_Base& operator = ( PgAPServer_Base & );
};

inline bool PgAPServer_Base::Send( BM::Stream const &kPacket )const
{
	using namespace CEL;
	return m_pkSession->VSend( kPacket );
}

inline BM::GUID PgAPServer_Base::GetSessionID()const
{
	if ( m_pkSession )
	{
		return m_pkSession->SessionKey().SessionGuid();
	}

	assert(false);
	return BM::GUID::NullData();
}

class PgAPServer
{
public:
	typedef std::list< SLinkageServerInfo >			CONT_LINKAGE_SERVER_LIST;
	typedef std::map< BM::GUID, PgAPServer_Base* >	CONT_LINKAGE_SERVER_ACCESSLIST;

public:
	PgAPServer();
	~PgAPServer();

public:
	void ReadFromConfig( LPCTSTR lpszFileName );
	HRESULT	Locked_Add( SLinkageServerInfo const &kInfo );

	HRESULT Locked_Connected( CEL::CSession_Base * const pkSession, CEL::ADDR_INFO const &kAddr, bool const bAccept );
	HRESULT	Locked_Logined( CEL::CSession_Base * const pkSession, SLinkageServerInfo const &kInfo );
	HRESULT Locked_DisConnected( BM::GUID const &kSessionID, bool &bOutIsAccept );

	HRESULT Locked_IsConnected( BM::GUID const &kSessionID )const;
	HRESULT Locked_GetServeInfo( BM::GUID const &kSessionID, SLinkageServerInfo &rkOutInfo )const;
	HRESULT Locked_ProcessPacket( BM::GUID const &kSessionID, BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream &rkPacket )const;

	bool Locked_IsConnectToServer( CEL::ADDR_INFO const &kAddrInfo )const;

	void Locked_ConnectToAllLinkAgeServer( BM::GUID const &kConnectorGuid );
	BM::GUID const Locked_GetConnectWorkGuid()const{return m_kConnectWorkGuid;}

	void Locked_Tick(void);

	void SetServiceHandlerType( CEL::E_SERVIEC_HANDLER_TYPE const kType ){ m_kServiceHandlerType = kType; }
	CEL::E_SERVIEC_HANDLER_TYPE GetServiceHandlerType()const{return m_kServiceHandlerType;}

private:
	HRESULT Add( SLinkageServerInfo const &kInfo );

	HRESULT Connected( CEL::CSession_Base * const pkSession, SLinkageServerInfo const &kInfo );
	HRESULT DisConnected( BM::GUID const &kSessionID, bool &bOutIsAccept );
	HRESULT IsConnected( BM::GUID const &kSessionID )const;

private:
	mutable ACE_RW_Thread_Mutex		m_kRWMutex;

	CEL::E_SERVIEC_HANDLER_TYPE		m_kServiceHandlerType;
	CONT_LINKAGE_SERVER_LIST		m_kContList;
	CONT_LINKAGE_SERVER_ACCESSLIST	m_kContAccessList;
	BM::GUID						m_kConnectWorkGuid;
};

#define g_kAPServer SINGLETON_STATIC(PgAPServer)
