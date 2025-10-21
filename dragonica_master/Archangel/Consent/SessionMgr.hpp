
template<typename T_SESSION_KEY>
PgSessionMgr<T_SESSION_KEY>::PgSessionMgr()
{
}

template<typename T_SESSION_KEY>
PgSessionMgr<T_SESSION_KEY>::~PgSessionMgr()
{
}

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::AddAddress(T_SESSION_KEY const& kKey, BM::GUID const & kConnector, std::wstring const & rkIP, WORD const wPort)
{
	AddrInfo kAddress(rkIP, wPort);
	if (false == kAddress.kAddr.IsCorrect())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Invalid Address SessionKey=") << kKey << _T(", Address=") << rkIP << _T(", Port=") << wPort);
		return false;
	}

	auto ibRet = m_kSession.insert(std::make_pair(kKey, SessionInfo(kAddress, kConnector)));
	if (false == ibRet.second)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Duplicate site info SessionKey=") << kKey << _T(", Address=") << rkIP << _T(", Port=") << wPort);
		return false;
	}
	INFO_LOG(BM::LOG_LV6, __FL__ << _T("Address SessionKey=") << kKey << _T(", Added =") << rkIP << _COMMA_ << wPort << _T(", Connector=") << kConnector);
	return true;
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::RemoveAddress( T_SESSION_KEY const& kKey )
{
	m_kSession.erase( kKey );
}

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::TryConnect()
{
	CONT_SESSION::iterator itor_session = m_kSession.begin();
	for ( ; m_kSession.end() != itor_session ; ++itor_session ) 
	{
		if ( (*itor_session).second.kAddress.byState == EConnectState_Disconnected )
		{
			if ( g_kCoreCenter.Connect((*itor_session).second.kAddress.kAddr, (*itor_session).second.kSessionKey) )
			{
				(*itor_session).second.kAddress.byState = EConnectState_TryConnect;
			}
			else
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Try connect failed Address=") << (*itor_session).second.kAddress.kAddr.ToString());
			}
		}
	}
	return true;
}

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::SendToServer( T_SESSION_KEY const &kKey, BM::Stream const &kPacket )const
{
	CONT_SESSION::const_iterator itr_session = m_kSession.find( kKey );
	if ( itr_session != m_kSession.end() )
	{
		if (	itr_session->second.kAddress.byState >= EConnectState_Connected 
			&&	itr_session->second.pkSession )
		{
			return itr_session->second.pkSession->VSend( kPacket );
		}
	}
	return false;
}

template<typename T_SESSION_KEY>
SessionInfo* const PgSessionMgr<T_SESSION_KEY>::GetSessionInfo(CEL::ADDR_INFO const & rkAddr)
{
	CONT_SESSION::iterator itor_session = m_kSession.begin();
	while (m_kSession.end() != itor_session)
	{
		if ((*itor_session).second.kAddress.kAddr == rkAddr)
		{
			return &((*itor_session).second);
		}
		++itor_session;
	}
	return NULL;
}

template<typename T_SESSION_KEY>
SessionInfo* const PgSessionMgr<T_SESSION_KEY>::GetSessionInfo(CEL::CSession_Base const * const pkSession)
{
	CONT_SESSION::iterator itor_session = m_kSession.begin();
	while (m_kSession.end() != itor_session)
	{
		if ((*itor_session).second.pkSession == pkSession)
		{
			return &((*itor_session).second);
		}
		++itor_session;
	}
	return NULL;
}

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::OnConnected( CEL::CSession_Base *pkSession )
{
	bool const bIsSucc = pkSession->IsAlive();
	SessionInfo* pkInfo = GetSessionInfo(pkSession->Addr());
	if ( pkInfo )
	{
		if ( true == bIsSucc )
		{
			INFO_LOG(BM::LOG_LV6, _T("[OnConnected] Succee Address=") << pkSession->Addr().ToString());
			pkInfo->kAddress.byState = EConnectState_Connected;
			pkInfo->pkSession = pkSession;
		}
		else
		{
			INFO_LOG(BM::LOG_LV5, _T("[OnConnected] Failed Address=") << pkSession->Addr().ToString());
			pkInfo->kAddress.byState = EConnectState_Disconnected;
		}
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, _T("[OnConnected] Cannot find Session Address=") << pkSession->Addr().ToString());
		return false;
	}

	return bIsSucc;
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::OnDisconnected( CEL::CSession_Base *pkSession )
{
	INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DISCONNECTED...Address=") << pkSession->Addr().ToString());
	SessionInfo* pkInfo = GetSessionInfo(pkSession->Addr());
	if (pkInfo == NULL)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find Session, Addr=") << pkSession->Addr().ToString());
		return;
	}
	pkInfo->kAddress.byState = EConnectState_Disconnected;
	pkInfo->pkSession = NULL;
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	switch( usType )
	{
	case PT_A_ENCRYPT_KEY:
		{
			CProcessConfig::Recv_PT_A_ENCRYPT_KEY(pkSession, pkPacket);
		}break;
		/*
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);
			SessionInfo* pkInfo = GetSessionInfo(pkSession->Addr());
			if ( pkInfo )
			{
				pkInfo->kAddress.byState = EConnectState_Ready;
				pkInfo->pkSession = pkSession;
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find Immigration Addr=") << pkSession->Addr().ToString());
			}
		}break;
		*/
	case PT_MCTRL_MMC_A_NFY_SERVER_COMMAND:
	case PT_MMC_CONSENT_NFY_INFO:
		{
			OnRecvFromMCtrl( usType, pkPacket, pkSession );
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << usType);
		}break;
	}
}

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::GetSessionKey(CEL::CSession_Base const * const pkSession, T_SESSION_KEY &rkOutKey ) const
{
	CONT_SESSION::const_iterator itor_session = m_kSession.begin();
	while (m_kSession.end() != itor_session)
	{
		if ((*itor_session).second.pkSession == pkSession)
		{
			rkOutKey = itor_session->first;
			return true;
		}
		++itor_session;
	}
	return false;
}

template<typename T_SESSION_KEY>
HRESULT PgSessionMgr<T_SESSION_KEY>::SendPacket(T_SESSION_KEY kKey, BM::Stream const& rkPacket)
{
	CONT_SESSION::const_iterator itor_session = m_kSession.find(kKey);
	if (m_kSession.end() != itor_session)
	{
		if ((*itor_session).second.pkSession != NULL)
		{
			if ((*itor_session).second.pkSession->VSend(rkPacket))
			{
				return S_OK;
			}
			
		}
	}
	return E_FAIL;
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::LoadImmigration( BM::GUID const &kConnectorID, std::wstring const &wstrConfigFileName )
{
	std::wstring const wSection(_T("IMMIGRATION_"));

	short sSiteNo = 0;
	TCHAR chValue[100] = {0,};
	int iValue = 0;

	for (int i=1; i<100; ++i)	// SITE 100개 이상 받을수 없다.
	{
		BM::vstring vSection( wSection );
		vSection += i;

		if (0 == ::GetPrivateProfileString( vSection.operator wchar_t const*(), _T("IP"), NULL, chValue, sizeof(chValue), wstrConfigFileName.c_str()))
		{
			break;
		}

		sSiteNo = ::GetPrivateProfileInt(vSection.operator wchar_t const*(), _T("SITE"), 0, wstrConfigFileName.c_str());
		iValue = ::GetPrivateProfileInt(vSection.operator wchar_t const*(), _T("PORT"), 0, wstrConfigFileName.c_str());
		if ( sSiteNo <= 0 || iValue <= 0 )
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Immigration Address is invalid Site=") << sSiteNo << _T(", Address=") << chValue << _T(", Port=") << iValue);
		}
		else
		{
			AddAddress(SessionKey(sSiteNo), kConnectorID, chValue, static_cast<WORD>(iValue) );
		}
	}
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::Locked_TryConnect()
{
	BM::CAutoMutex kLock(m_kMutex);
	TryConnect();
}

template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::Locked_OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	OnConnected(pkSession);
}

/*
template<typename T_SESSION_KEY>
void PgSessionMgr<T_SESSION_KEY>::Locked_OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	OnDisconnected(pkSession);
}
*/

template<typename T_SESSION_KEY>
bool PgSessionMgr<T_SESSION_KEY>::Locked_SendToServer( T_SESSION_KEY const &kKey, BM::Stream const &kPacket )const
{
	BM::CAutoMutex kLock(m_kMutex);
	return SendToServer(kKey, kPacket);
}
