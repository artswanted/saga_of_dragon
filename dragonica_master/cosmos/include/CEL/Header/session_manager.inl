
template < typename T_SESSION >
CSession_Manager< T_SESSION >::CSession_Manager(INIT_CORE_DESC const& rInfo)
	:	m_rInitInfo(rInfo)
{
	m_mapSession.clear();
}

template < typename T_SESSION >
CSession_Manager< T_SESSION >::~CSession_Manager(void)
{
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::OnOpen(SERVICE_HANDLER * const pServiceHandler)
{//! 접속 인증. 세션 등록.
//! std::cout << "NC_OPEN S" << std::endl;
//!	ACE_Write_Guard< MUTEX > guard( m_session_mgr_mutex );
	BM::CAutoMutex kLock(m_kMutex, true);

	auto ret = m_mapSession.insert( CONT_SESSION::value_type( pServiceHandler->SessionKey().SessionGuid(), pServiceHandler ) );
	if( true == ret.second )//! Insert 로 바꾸면 ATS 때문에 데드락이 걸린다.	
	{//! 이거 Insert 시에 이미 핸들이 있으면?.
		goto __SUCCESS;
	}
	return;
__SUCCESS:
	{
//! 	std::cout << "NC_OPEN E" << std::endl;
		return;
	}
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::OnClose(SERVICE_HANDLER * const pServiceHandler)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	
	CONT_SESSION::key_type const& kSessionGuid = pServiceHandler->SessionKey().SessionGuid();
	
	m_mapSession.erase(kSessionGuid);//! 우선 제거
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::VUpdate( BM::CSubject<eNOTIFY_CAUSE> * const pChangedSubject, eNOTIFY_CAUSE rkNfy)
{
	SERVICE_HANDLER * const pServiceHandler = static_cast< SERVICE_HANDLER * const >(pChangedSubject);
	
	switch(rkNfy)
	{
	case NC_OPEN:
		{
			OnOpen( pServiceHandler );
		}break;
	case NC_CLOSE:
		{
			OnClose( pServiceHandler );
		}break;
	////	NC_ONSEND(handle_write_stream 이벤트에다가는 뭘 걸지 말것. 데드락 생길 소지가 있음
	////	put 과 handle_write_stream 이 그렇슴.
	default:
		{
		}break;
	}
}

template < typename T_SESSION >
bool CSession_Manager< T_SESSION >::VSend( typename CONT_SESSION::key_type const& rHashKey,  BM::Stream const& rPacket )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SESSION::iterator itor = m_mapSession.find(rHashKey);
	
	if(itor != m_mapSession.end() )
	{
		CONT_SESSION::mapped_type &pkSession = (*itor).second;
		return pkSession->VSend(rPacket);
	}
	return false;
}

template < typename T_SESSION >
bool CSession_Manager< T_SESSION >::Terminate( typename CONT_SESSION::key_type const& rHashKey )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SESSION::iterator itor = m_mapSession.find(rHashKey);
	
	if(itor != m_mapSession.end() )
	{
		CONT_SESSION::mapped_type &pkSession = (*itor).second;
		bool const bRet = pkSession->VTerminate();
		return bRet;
	}

	return false;
}

template < typename T_SESSION >
bool CSession_Manager< T_SESSION >::TerminateAll()
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SESSION::iterator itor = m_mapSession.begin();
	
	while(itor != m_mapSession.end() )
	{
		CONT_SESSION::mapped_type &pkSession = (*itor).second;
		pkSession->VTerminate( );
		++itor;
	}
	return true;
}

template < typename T_SESSION >
typename CSession_Manager< T_SESSION >::SERVICE_HANDLER* CSession_Manager< T_SESSION >::AcquireHandler()//! Onclise 에서 알아서 돌려줌.
{
	T_SESSION *pkHandler = new T_SESSION(m_rInitInfo);
	if(pkHandler)
	{
		BM::CObserver< eNOTIFY_CAUSE > *pkObserver = dynamic_cast< BM::CObserver< eNOTIFY_CAUSE >* >(this);
		pkHandler->VAttach( pkObserver );//! 세션 메니저를 붙여준다.Open 시에 매니저가 얘를 등록 한다.
	}

	return pkHandler;//! 		return new SERVICE_HANDLER;//! 서비스 핸들러가 재사용이 안됨.
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::OnConnectFail( ACE_INET_Addr const& remote, void const *pAct )
{
	T_SESSION kTempSession;
	if( pAct )
	{
		SConnectInfo* pkConnectionInfo = (SConnectInfo*)pAct;

		CEL::SESSION_KEY kKey(pkConnectionInfo->guidConnector, pkConnectionInfo->guidSession);

		kTempSession.SessionKey( kKey );
		kTempSession.Addr(pkConnectionInfo->addr);

		SAFE_DELETE(pkConnectionInfo);//CoreCenter에서 Connect 걸떄 new 한 부분.
	}

	if( m_rInitInfo.OnSessionOpen )
	{
//		CEL_LOG( BM::LOG_LV6, L"Try Connect Failed " << kTempSession.Addr().ToString());
		m_rInitInfo.OnSessionOpen( &kTempSession );
	}
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::CheckKeepAlive(DWORD const dwSec)
{
	BM::CAutoMutex kLock(m_kMutex);
	
	CONT_SESSION::iterator session_itor = m_mapSession.begin();

	ACE_Time_Value const now_time = ACE_OS::gettimeofday();
		 
	while(session_itor != m_mapSession.end())
	{
		ACE_Time_Value const& rkTime = (*session_itor).second->LastNetworkActTime();

		ACE_Time_Value ret_time = now_time - rkTime;
		if( ret_time.sec() > dwSec)
		{
			(*session_itor).second->VTerminate();
		}

		++session_itor;
	}
}

template < typename T_SESSION >
HRESULT CSession_Manager< T_SESSION >::Statistics(typename CONT_SESSION::key_type const& rHashKey,  SSessionStatistics &rkOutResult)const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SESSION::const_iterator itor = m_mapSession.find(rHashKey);
	if(itor != m_mapSession.end() )
	{
		CONT_SESSION::mapped_type const& pkSession = (*itor).second;
		return pkSession->Statistics(rkOutResult);
	}

	return E_FAIL;
}

template < typename T_SESSION >
void CSession_Manager< T_SESSION >::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	BM::CAutoMutex kLock(m_kMutex);
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV2,  (BM::vstring) _T("Session Count [") << m_mapSession.size() <<  _T("]") ));
	
}