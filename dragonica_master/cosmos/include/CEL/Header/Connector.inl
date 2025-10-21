//Connector.inl 과 같은 부분이 있음. 수정할떄 주의할것
template < typename T_SESSION_MGR >
CConnector< T_SESSION_MGR >::CConnector(INIT_CORE_DESC const& rInitInfo)
	:	CWorker_Base(rInitInfo)
	,	m_kSessionMgr(m_kInitInfo)
{
	Allow(false);
}


template < typename T_SESSION_MGR >
CConnector< T_SESSION_MGR >::~CConnector(void)
{
//! ACE_Proactor* pProactor = this->proactor();
	proactor(NULL);
//! SAFE_DELETE( pProactor );
}

template < typename T_SESSION_MGR >
bool CConnector< T_SESSION_MGR >::VClose()
{
	m_kSessionMgr.TerminateAll();
/*	if(proactor())
	{
		cancel();
		proactor()->proactor_end_event_loop();//! 엑셉터정지.
	}
	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
	if( 0 == iRet )
	{
		ACE_Proactor* pProactor = proactor();
		SAFE_DELETE(pProactor);
		proactor(pProactor);
		return true;
	}
*/	
	cancel();
	return true;
	
	return false;
}

template < typename T_SESSION_MGR >
typename  CConnector< T_SESSION_MGR >::SERVICE_HANDLER* CConnector< T_SESSION_MGR >::make_handler(void)//! 생성 할때 sendpool 과 recv 풀을 전달 해야 할것으로 보인다.
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
bool CConnector< T_SESSION_MGR >::IsAllow()const
{
	return ((m_lIsAllow)?true:false);
}

template < typename T_SESSION_MGR >
void CConnector< T_SESSION_MGR >::Allow(bool const bIsAllowConnect)
{
	::InterlockedExchange( &m_lIsAllow, bIsAllowConnect); 
}

template < typename T_SESSION_MGR >
bool CConnector< T_SESSION_MGR >::VSend(BM::GUID const& rSessionGuid,  BM::Stream const& rPacket)
{
	return m_kSessionMgr.VSend( rSessionGuid, rPacket );
}

template < typename T_SESSION_MGR >
bool CConnector< T_SESSION_MGR >::VTerminate(BM::GUID const& rkSessionKey)
{
	return m_kSessionMgr.Terminate(rkSessionKey);
}

///////////이 아래만 엑섭터랑 다름
template < typename T_SESSION_MGR >
int CConnector< T_SESSION_MGR >::validate_connection (const ACE_Asynch_Connect::Result& result, ACE_INET_Addr const& remote, ACE_INET_Addr const& local)
{//! SSL 을 사용하여 해킹을 방지 한다.
	if( result.success() )
	{
//! 			switch( PermissionMgr().IsAccord(remote) )
//! 			{
//! 			case AP_BLOCK:
//! 				{
//! 					MAIN_LOG( LOG_LV1, "Is Blocked Addr [%s:%d]", remote.get_host_addr() , remote.get_port_number() );	
//! 					return -1;
//! 				}break;
//! 			case AP_ALLOW:
//! 			default:
//! 				{
			return 1;
//! 				}break;
//! 			}
	}
	else
	{
		m_kSessionMgr.OnConnectFail( remote, result.act() );
		return -1;
	}
}

template < typename T_SESSION_MGR >
bool CConnector< T_SESSION_MGR >::VInit(BM::GUID const& rGuid, ADDR_INFO const& rBindAddr, ADDR_INFO const& rNATAddr, bool const bIsAllow)//! Init 시에 Proactor 의 인스턴스가 생성 된다. 따로 생성 할 수도 있다.
{//! 프로엑터 셋팅. //! open 메소드를 재정의 하는것이 아니므로 함수명 바꾸지 말 것.
//	ACE_Proactor* pProactor = new ACE_Proactor;
	ACE_Proactor* pProactor = ACE_Proactor::instance();
	
	assert(pProactor && "Can't Create New Proactor");
	this->proactor( pProactor );

	if(pProactor)
	{
		if(-1 == ACE_Asynch_Connector< SERVICE_HANDLER >::open(1, pProactor))
		{
			return false;
		}
		WorkerGuid(rGuid);
		m_kSessionMgr.WorkerGuid( WorkerGuid() );
		Allow(true);
		return true;
	} //! if(pProactor)
	return false;
}

template < typename T_SESSION_MGR >
void CConnector< T_SESSION_MGR >::VConnect(ADDR_INFO const& rAddr, void *pAct)
{
	Connect( rAddr.IP(), rAddr.wPort, pAct);
}

template < typename T_SESSION_MGR >
int CConnector< T_SESSION_MGR >::Connect(std::wstring const& wstrRemoteIP, WORD const wPort,  void const *pAct )
{
	ACE_INET_Addr Addr(wPort, wstrRemoteIP.c_str());
	return Connect(Addr, pAct);
}

template < typename T_SESSION_MGR >
int CConnector< T_SESSION_MGR >::Connect(ACE_INET_Addr const& Addr, void const *pAct )
{
	if( !IsAllow() )
	{
		CEL_LOG( BM::LOG_LV0, _T("Connector Can't work!!"));
		return -2;
	}

	int const iRet = this->connect(Addr, (ACE_INET_Addr const&)ACE_Addr::sap_any, 1, pAct);
	if( -1 == iRet)
	{
		CEL_LOG( BM::LOG_LV0, _T("Try Connect Failed!!"));
	}
	return iRet;
}

template < typename T_SESSION_MGR >
HRESULT CConnector< T_SESSION_MGR >::Statistics(BM::GUID const& rSessionGuid,  SSessionStatistics &rkOutResult)const
{
	return m_kSessionMgr.Statistics(rSessionGuid, rkOutResult);
}

template < typename T_SESSION_MGR >
void CConnector< T_SESSION_MGR >::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, _T("===== Connector State Display =====")));
	m_kSessionMgr.VDisplayState(rkLogWorker, iLogType);
}