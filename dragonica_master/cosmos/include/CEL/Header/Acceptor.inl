//Connector.inl 과 같은 부분이 있음. 수정할떄 주의할것
template < typename T_SESSION_MGR >
CAcceptor< T_SESSION_MGR >::CAcceptor( INIT_CORE_DESC const& rInitInfo, size_t const page_pool_size )
	:	CWorker_Base(rInitInfo)
	,	m_page_pool_size(page_pool_size)
	,	m_kSessionMgr(m_kInitInfo)
{
	Allow(false);
}

template < typename T_SESSION_MGR >
CAcceptor< T_SESSION_MGR >::~CAcceptor(void)
{
//! ACE_Proactor* pProactor = this->proactor();
	proactor(NULL);
//! SAFE_DELETE( pProactor );
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::VClose()
{
	Allow(false);
	m_kSessionMgr.TerminateAll();

	cancel();
	return true;
}

template < typename T_SESSION_MGR >
typename CAcceptor< T_SESSION_MGR >::SERVICE_HANDLER* CAcceptor< T_SESSION_MGR >::make_handler(void)
{	//! Default behavior
	SERVICE_HANDLER *pTHandler = m_kSessionMgr.AcquireHandler();//가져간 세션이 직접 지우도록.
	
	if(pTHandler)
	{
		SESSION_KEY kSessionKey = pTHandler->SessionKey();
		kSessionKey.WorkerGuid(WorkerGuid());
		pTHandler->SessionKey(kSessionKey);

		return pTHandler;
	}
	assert(NULL);
	return NULL;
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::IsAllow()const
{
	return ((m_lIsAllow)?true:false);
}

template < typename T_SESSION_MGR >
void CAcceptor< T_SESSION_MGR >::Allow(bool const bIsAllowAccept)
{
	::InterlockedExchange( &m_lIsAllow, bIsAllowAccept); 
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::VSend(BM::GUID const& rSessionGuid,  BM::Stream const& rPacket)
{
	return m_kSessionMgr.VSend( rSessionGuid, rPacket );
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::VTerminate(BM::GUID const& rkSessionGuid)
{
	return m_kSessionMgr.Terminate(rkSessionGuid);
}

///////////이 아래만 커넥터랑 다름
template < typename T_SESSION_MGR >
int CAcceptor< T_SESSION_MGR >::validate_connection (const ACE_Asynch_Accept::Result& result, ACE_INET_Addr const& remote, ACE_INET_Addr const& local)
{//! SSL 을 사용하여 해킹을 방지 한다.
	if( !IsAllow() )
	{//러턴 콜을 해줘야 할거 같은데
//		m_kSessionMgr.OnConnectFail( remote, result.act() );
		return -1;
	}

	if( result.success() )
	{
		return 1;
	}
	else
	{
	}
//	m_kSessionMgr.OnConnectFail( remote, result.act() );
	return -1;
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::VInit(BM::GUID const& rGuid, ADDR_INFO const& rBindAddr, ADDR_INFO const& rNATAddr, bool const bIsAllow)
{//! 프로엑터 셋팅.
//	ACE_Proactor* pProactor = new ACE_Proactor;
	ACE_Proactor* pProactor = ACE_Proactor::instance();
	assert(pProactor && "Can't create New Proactor");
	this->proactor( pProactor );

//! if(pProactor)
//! {
		m_BindAddr.set( rBindAddr.wPort, rBindAddr.IP().c_str());
		m_NAT_Addr.set( rNATAddr.wPort, rNATAddr.IP().c_str() );
		WorkerGuid(rGuid);
		
		m_kSessionMgr.WorkerGuid( WorkerGuid() );

		Allow( bIsAllow );
		return VOpen();
//! }
	return false;
}

template < typename T_SESSION_MGR >
std::string CAcceptor< T_SESSION_MGR >::IP( bool const bIsBindIP = false ) const
{//! 기본 NAT(최종 IP)
	ACE_INET_Addr const& rAddr = ((bIsBindIP)?(m_BindAddr):(m_NAT_Addr));
	char const* pTemp = rAddr.get_host_addr();
	if( pTemp )
	{
		return std::string(pTemp);
	}
	return std::string("NONE IP");
}

template < typename T_SESSION_MGR >
WORD const CAcceptor< T_SESSION_MGR >::Port(bool const bIsBindPort = false ) const
{//! 기본 NAT(최종 PORT)
	ACE_INET_Addr const& rAddr = ((bIsBindPort)?(m_BindAddr):(m_NAT_Addr));
	return rAddr.get_port_number();
}

template < typename T_SESSION_MGR >
bool CAcceptor< T_SESSION_MGR >::VOpen()
{
	if(-1 == ACE_Asynch_Acceptor< SERVICE_HANDLER >::open(m_BindAddr, m_page_pool_size, 1, ACE_DEFAULT_BACKLOG, 1, proactor(), 1 ))//! nonPage Pool 때문에 블럭 사이즈를 설정 하도록 한다.
	{
		CEL_LOG( BM::LOG_LV0, _T("Can't Bind Acceptor Addr:") << m_kInitInfo.kBindAddr.ToString());
		return false;
	}
	Allow( true );

	return true;
}

template < typename T_SESSION_MGR >
HRESULT CAcceptor< T_SESSION_MGR >::Statistics(BM::GUID const& rSessionGuid,  SSessionStatistics &rkOutResult)const
{
	return m_kSessionMgr.Statistics(rSessionGuid, rkOutResult);
}

template < typename T_SESSION_MGR >
void CAcceptor< T_SESSION_MGR >::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{//IP, Port
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, _T("===== Acceptor State Display =====")));
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV2, (BM::vstring) _T("Bind Addr: [") << m_kInitInfo.kBindAddr.ToString()<< _T("] Nat Addr: [") << m_kInitInfo.kNATAddr.ToString()));
	m_kSessionMgr.VDisplayState(rkLogWorker, iLogType);
}

