#include "stdafx.h"
#include "PgSiteMemberMgr.h"

PgSiteMemberMgr::PgSiteMemberMgr(void)
:	m_eState(ECN_STATE_INIT)
,	m_bDisconnectDuplicatedUser(false)
{
}

PgSiteMemberMgr::~PgSiteMemberMgr(void)
{

}

bool PgSiteMemberMgr::Locked_SendToImmigrationOne( BM::Stream const &kPacket )const
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SESSION::const_iterator itr_session = m_kSession.begin();
	for ( ; itr_session != m_kSession.end() ; ++itr_session )
	{
		if (	IMM_SERVER_SITE_NUM_BEGIN <= itr_session->first
			&&	IMM_SERVER_SITE_NUM_END	>= itr_session->first
			)
		{
			if (	itr_session->second.kAddress.byState >= EConnectState_Connected 
				&&	itr_session->second.pkSession )
			{
				if ( true == itr_session->second.pkSession->VSend( kPacket ) )
				{
					return true;
				}
			}
		}
	}
	return false;
}

void PgSiteMemberMgr::RecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	switch ( usType )
	{
	case PT_A_S_ANS_GREETING:
		{
			SERVER_IDENTITY kRecvSI;
			kRecvSI.ReadFromPacket(*pkPacket);

			{
				g_kProcessCfg.Locked_OnGreetingServer(kRecvSI, pkSession);
				SessionInfo* pkInfo = GetSessionInfo(pkSession->Addr());
				if ( pkInfo )
				{
					INFO_LOG( BM::LOG_LV6, _T("Ready To Immigration Addr=") << pkSession->Addr().ToString() );
					pkInfo->kAddress.byState = EConnectState_Ready;
					pkInfo->pkSession = pkSession;
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot find Immigration Addr=") << pkSession->Addr().ToString());
				}

				/*
				if( !m_bNowSyncImmigration )
				{
					BM::Stream kUnLockPacket( PT_CN_IM_NFY_LOGINBLOCKED, false );
					pkSession->VSend( kUnLockPacket );
				}
				*/
				LOCAL_MGR::NATION_CODE eCode = LOCAL_MGR::NC_NOT_SET;
				pkPacket->Pop(eCode);
				if (pkInfo)
				{
					pkInfo->eNationCode = eCode;
				}
			}

			RecvSyncUserList( pkSession, pkPacket );
		}break;
	case PT_IM_CN_REQ_LOGIN_AUTH:
		{
			INFO_LOG(BM::LOG_LV7, __FL__ << L"PT_IM_CN_REQ_LOGIN_AUTH Begin");
			Recv_PT_IM_CN_REQ_LOGIN_AUTH( pkSession, pkPacket );
			INFO_LOG(BM::LOG_LV7, __FL__ << L"PT_IM_CN_REQ_LOGIN_AUTH End");
		}break;
	case PT_IM_CN_REQ_LOGIN_LOGIN:
		{
			SessionKey const nSessionKey = GetSessionKey( pkSession );
			if ( IMM_SERVER_SITE_NUM_ERROR != nSessionKey )
			{
				SAuthInfo kAuthInfo;
				kAuthInfo.ReadFromPacket(*pkPacket);
				ETryLoginResult eRet = Recv_PT_IM_CN_REQ_LOGIN_LOGIN( nSessionKey, kAuthInfo );

				BM::Stream kAnsPacket( PT_CN_IM_ANS_LOGIN_LOGIN, eRet );
				kAuthInfo.WriteToPacket(kAnsPacket);
				pkSession->VSend( kAnsPacket );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!") );
			}
		}break;
	case PT_IM_CN_ANS_LOGIN_LOGIN_FAILED:
		{
			Recv_PT_IM_CN_ANS_LOGIN_LOGIN_FAILED( pkSession,  pkPacket );
		}break;
	case PT_IM_CN_NFY_LOGOUT_AUTH:
		{
			Recv_PT_IM_CN_NFY_LOGOUT_AUTH( pkSession, pkPacket );
		}break;
	case PT_IM_CN_ANS_USER_TERMINATE_ERROR:
		{
			Recv_PT_IM_CN_ANS_USER_TERMINATE_ERROR( pkSession, pkPacket );
		}break;
	case PT_IM_CN_NFY_LOGOUT_LOGIN:
		{
			std::wstring wstrID;
			if ( true == pkPacket->Pop( wstrID ) )
			{
				RemoveUserID( wstrID );
			}
		}break;
	default:
		{
			PgSessionMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	}
}

void PgSiteMemberMgr::BroadCast_Immigration( BM::Stream const &kPacket )const
{
	CONT_SESSION::const_iterator itr_session = m_kSession.begin();
	for ( ; m_kSession.end() != itr_session ; ++itr_session )
	{
		if (	(IMM_SERVER_SITE_NUM_BEGIN <= itr_session->first)
			&&	(IMM_SERVER_SITE_NUM_END >= itr_session->first)
			)
		{
			if (	itr_session->second.kAddress.byState >= EConnectState_Connected 
				&&	itr_session->second.pkSession )
			{
				itr_session->second.pkSession->VSend( kPacket );
			}
		}
	}
}

PgSiteMemberMgr::SessionKey PgSiteMemberMgr::GetSessionKey( CEL::CSession_Base *pkSession )const
{
	SessionKey nSessionKey = IMM_SERVER_SITE_NUM_ERROR;
	PgSessionMgr::GetSessionKey( pkSession, nSessionKey );
	return nSessionKey;
}

bool PgSiteMemberMgr::Locked_OnRegistConnector( CEL::SRegistResult const &rkArg )
{
	BM::CAutoMutex kLock(m_kMutex);
	std::wstring wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_Config.ini"));

	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unknown ServerType : ") << rkArg.iIdentityValue << _T(">") );
			return false;
		}break;
	}
	return true;
}

bool PgSiteMemberMgr::AddUserID( CONT_USER_ID::key_type const &kID, CONT_USER_ID::mapped_type const &kElement, SessionKey &rkOutOldServerNo )
{
	auto kPair = m_kContUserID.insert( std::make_pair( kID, kElement ) );
	if ( !kPair.second )
	{
		if ( kElement.nSite == kPair.first->second.nSite )
		{
			kPair.first->second = kElement;

			// 사이트가 같으면 성공시켜 주어야 한다.
			CAUTION_LOG( BM::LOG_LV5, __FL__ << kID << L" Synchronization Error!!" );
			return true;
		}

		rkOutOldServerNo = kPair.first->second.nSite;
	}
	return kPair.second;
}

bool PgSiteMemberMgr::RemoveUserID( CONT_USER_ID::key_type const &kID )
{
	return 0 != m_kContUserID.erase( kID );
}

bool PgSiteMemberMgr::GetUserInfo( CONT_USER_ID::key_type const &kID, CONT_USER_ID::mapped_type &rkOutElement )const
{
	CONT_USER_ID::const_iterator itr = m_kContUserID.find( kID );
	if ( itr != m_kContUserID.end() )
	{
		rkOutElement = itr->second;
		return true;
	}
	return false;
}

bool PgSiteMemberMgr::IsHaveUserID( CONT_USER_ID::key_type const &kID )const
{
	CONT_USER_ID::const_iterator itr = m_kContUserID.find( kID );
	if ( itr != m_kContUserID.end() )
	{
		return true;
	}
	return false;
}

void PgSiteMemberMgr::RemoveSiteUserList( SessionKey const nSite )
{
	INFO_LOG( BM::LOG_LV6, _T("[SyncUserList] Begin... Site<") << nSite << _T(">") );
	size_t iRemoveUserCount = 0;

	CONT_USER_ID::iterator user_itr = m_kContUserID.begin();
	while ( user_itr!=m_kContUserID.end() )
	{
		if ( nSite == user_itr->second.nSite )
		{
			user_itr = m_kContUserID.erase( user_itr );
			++iRemoveUserCount;
		}
		else
		{
			++user_itr;
		}
	}
	INFO_LOG( BM::LOG_LV6, _T("[SyncUserList] End... Result UserCount<") << iRemoveUserCount << _T(">") );

}

void PgSiteMemberMgr::RecvSyncUserList( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	SessionKey const nSessionKey = GetSessionKey( pkSession );
	if ( IMM_SERVER_SITE_NUM_ERROR == nSessionKey )
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Session<") << pkSession->Addr().ToString() << _T(">") );
		return;
	}

	CONT_USER_ID kContTemp;

	size_t iSize = 0;
	pkPacket->Pop( iSize );

	if ( IsServiceReady() && iSize > 0)
	{
		// 이쪽으로 들어오면 안된다.............
		// ServiceReady 상태 일때는 Immigration의 유저정보가 비어 있어야 한다.
		// 왜냐하면...
		//	1. ImmigrationServer crash등으로 다시 시작할 때는 Site전체가 다시 시작되어야 한다.(전제조건) -> LoginedUser는 없다.
		//	2. ConsentServer가 서비스중인 Site전체 유저 정보를 받고 난 후 ServiceReady 상태가 된다.
		VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Recevied LoginedUser info from Imm in the ServiceReady status Site=") << nSessionKey);
	}

	SAuthSyncData kAuthSyncData;
	while ( iSize-- )
	{
		kAuthSyncData.ReadFromPacket( *pkPacket );
		SSiteUserInfo kSiteUserInfo( nSessionKey, kAuthSyncData );
		kContTemp.insert( std::make_pair( kSiteUserInfo.strID, kSiteUserInfo ) );
	}

	if ( kContTemp.size() )
	{
		INFO_LOG( BM::LOG_LV6, _T("[SyncUserList] Begin... Site<") << nSessionKey << _T("> Addr<") << pkSession->Addr().ToString() << _T("> ") );

		size_t iSameCount = 0;
		size_t iErrorCount = 0;
		size_t iNewLoginCount = 0;
		size_t iNewLogOutCount = 0;

		// 먼저 돌면서 있는 놈부터 체크해보자
		CONT_USER_ID::iterator user_itr2;
		CONT_USER_ID::iterator user_itr = m_kContUserID.begin();
		while ( user_itr!=m_kContUserID.end() )
		{
			SSiteUserInfo &kOldSiteUserInfo = user_itr->second;

			user_itr2 = kContTemp.find( kOldSiteUserInfo.strID );
			if ( user_itr2 != kContTemp.end() )
			{
				// 이놈 있네 ㅋㅋㅋ
				if ( nSessionKey == kOldSiteUserInfo.nSite )
				{
					kOldSiteUserInfo.byState = ESITE_USER_LOGIN;
					kContTemp.erase( user_itr2 );// 이목록에서는 지워
					++iSameCount;
				}
				else
				{
					// 심각한 에런데... 이러면 안된다.. 1818
					VERIFY_INFO_LOG( false, BM::LOG_LV0, _T(" Critical Error!! User<") << kOldSiteUserInfo.strID << _T("> ConsentSite<") << kOldSiteUserInfo.nSite << _T("> NewSite<") << nSessionKey << _T(">") );
					kContTemp.erase( user_itr2 );// 이목록에서는 지워
					++iErrorCount;
				}
				++user_itr;// ++
			}
			else
			{
				if ( nSessionKey == kOldSiteUserInfo.nSite )
				{
					// 이놈 없네.... 로그아웃 시키자.
					INFO_LOG( BM::LOG_LV5, _T("* LogOut User<") << kOldSiteUserInfo.strID << _T(">") );
					user_itr = m_kContUserID.erase( user_itr );
					++iNewLogOutCount;
				}
				else
				{
					++user_itr;// ++
				}
			}
		}

		user_itr2 = kContTemp.begin();
		for ( ; user_itr2!= kContTemp.end() ; ++user_itr2 )
		{
			// 새로 로그인한 놈들 추가해주자. 근데 이것도 문제가 있는데....
			SSiteUserInfo &kNewSiteUserInfo = user_itr2->second;
			CAUTION_LOG( BM::LOG_LV5, _T("* LogIn User<") << kNewSiteUserInfo.strID << _T(">") );
			m_kContUserID.insert( std::make_pair( user_itr2->first, user_itr2->second ) );
			++iNewLoginCount;
		}

		INFO_LOG( BM::LOG_LV6, _T("[SyncUserList] End... Result : Same<") << iSameCount << _T("> LogOut<") << iNewLogOutCount << _T("> LogIn<") << iNewLoginCount << _T("> Error<") << iErrorCount << _T(">") );
	}
}

void PgSiteMemberMgr::Recv_PT_IM_CN_REQ_LOGIN_AUTH( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	SAuthInfo kAuthInfo;
	if ( false == kAuthInfo.ReadFromPacket(*pkPacket) ) 
	{
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		CAUTION_LOG(BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size());
		return;
	}
	
	SessionKey const nSessionKey = GetSessionKey( pkSession );
	if ( IMM_SERVER_SITE_NUM_ERROR == nSessionKey )
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Session<") << pkSession->Addr().ToString() << _T(">") );
		return;
	}

	ETryLoginResult eRet = E_TLR_BLOCKED_SERVER_LOGIN;
	if ( true == IsServiceReady())
	{
		SSiteUserInfo kSiteUserInfo( nSessionKey, kAuthInfo );
		SessionKey nSessionKey_Old = 0;

		if ( true == AddUserID( kSiteUserInfo.strID, kSiteUserInfo, nSessionKey_Old ) )
		{
			eRet = E_TLR_SUCCESS_AUTH;
		}
		else
		{
			INFO_LOG( BM::LOG_LV7, __FL__ << _T("Already requested user ID=") << kSiteUserInfo.strID );
			eRet = E_TLR_ALREADY_TRY;

			// 동기화 문제가 생길 수 있기때문에...
			// 확인을 해봐야 한다(진짜 로그인해 있는지)
			BM::Stream kDisConnectPacket( PT_CN_IM_NFY_USER_TERMINATE, static_cast<EClientDisconnectedCause>(CDC_TryLoginFromOther) );
			kDisConnectPacket.Push( kSiteUserInfo.strID );
			kDisConnectPacket.Push( m_bDisconnectDuplicatedUser );
			SendToServer( nSessionKey_Old, kDisConnectPacket );
		}
	}

	BM::Stream kAnsPacket( PT_CN_IM_ANS_LOGIN_AUTH, eRet );
	kAuthInfo.WriteToPacket(kAnsPacket);
	pkSession->VSend( kAnsPacket );
}

void PgSiteMemberMgr::Recv_PT_IM_CN_NFY_LOGOUT_AUTH( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	std::wstring wstrID;
	if ( true == pkPacket->Pop(wstrID) )
	{
		SessionKey const nSessionKey = GetSessionKey( pkSession );
		if ( IMM_SERVER_SITE_NUM_ERROR != nSessionKey )
		{
			SetUserState( nSessionKey, wstrID, ESITE_USER_AUTH, false );
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Session<") << pkSession->Addr().ToString() << _T(">") );
		}	
	}
	else
	{
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ );
	}
}

ESITEUSER_STATE_RESULT PgSiteMemberMgr::SetUserState( SessionKey const nSessionKey, std::wstring const &wstrID, BYTE const byState, bool const bAdd )
{
	CONT_USER_ID::iterator itr = m_kContUserID.find( wstrID );
	if ( itr != m_kContUserID.end() )
	{
		SSiteUserInfo& kSiteUserInfo = itr->second;
		if ( nSessionKey == kSiteUserInfo.nSite )
		{
			if ( true == bAdd )
			{
				if ( byState == (byState & kSiteUserInfo.byState) )
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Have State<") << byState << _T("> User ID = ") << wstrID );
					return ESITE_USER_HAVESTATE;
				}
				else
				{
					kSiteUserInfo.byState |= byState;
				}
			}
			else
			{
				if ( byState & kSiteUserInfo.byState )
				{
					BYTE const byState2 = (~byState);
					kSiteUserInfo.byState &= byState2;
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Have Not State<") << byState << _T("> User ID = ") << wstrID );
					return ESITE_USER_HAVESTATE;
				}

				if ( ESITE_USER_NONE == kSiteUserInfo.byState )
				{
					m_kContUserID.erase( itr );
					return ESITE_USER_SUCCESS_REMOVE;
				}
			}
			return ESITE_USER_SUCCESS;
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Login ReqSite<") << nSessionKey << _T("> OrgSite<") << kSiteUserInfo.nSite << _T("> User ID = ") << wstrID );
			return ESITE_USER_DIFFERENTSITE;
		}
	}
	return ESITE_USER_NOTFOUND;
}

ETryLoginResult PgSiteMemberMgr::Recv_PT_IM_CN_REQ_LOGIN_LOGIN( SessionKey const nSessionKey, SAuthInfo const &kAuthInfo, BYTE const byState )
{
	if (false == IsServiceReady())
	{
		return E_TLR_BLOCKED_SERVER_LOGIN;
	}

	std::wstring const wstrID( kAuthInfo.ID() );
	switch ( SetUserState( nSessionKey, wstrID, byState, true ) )
	{
	case ESITE_USER_NOTFOUND:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("NotFound User ID<") << wstrID << _T(">") );

/*
 			// 여기서 못찾을 수도 있다...
			// Immigration에서 Auth정보를 클리어하면 그럴 수 있다...
			// 일단 없으니까 성공시켜 주자.
			SSiteUserInfo kSiteUserInfo( nSessionKey, kAuthInfo );
			kSiteUserInfo.byState = byState;
			SessionKey nSessionKey_Old = 0;
			if ( !AddUserID( kSiteUserInfo.strID, kSiteUserInfo, false, nSessionKey_Old ) )
			{
				if ( true == m_bDisconnectDuplicatedUser )
				{
					// 중복 로그인 접속을 끊어 버려
					BM::Stream kDisConnectPacket( PT_CN_IM_NFY_USER_TERMINATE, static_cast<EClientDisconnectedCause>(CDC_TryLoginFromOther) );
					kDisConnectPacket.Push( wstrID );
					SendToServer( nSessionKey_Old, kDisConnectPacket );
				}

				return E_TLR_ALREADY_LOGINED;
			}
*/
		}break;
	case ESITE_USER_SUCCESS:
		{
			return E_TLR_SUCCESS;
		}break;
	case ESITE_USER_HAVESTATE:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Login Yet!! User ID = ") << wstrID );
		}break;
	case ESITE_USER_DIFFERENTSITE:
		{
			return E_TLR_ALREADY_LOGINED;
		}break;
	case ESITE_USER_SUCCESS_REMOVE:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!! User ID = ") << wstrID );
		}break;
	}

	return E_TLR_FAILED;
}

void PgSiteMemberMgr::Recv_PT_IM_CN_ANS_LOGIN_LOGIN_FAILED( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	std::wstring wstrID;
	if ( true == pkPacket->Pop(wstrID) )
	{
		SessionKey const nSessionKey = GetSessionKey( pkSession );
		if ( IMM_SERVER_SITE_NUM_ERROR == nSessionKey )
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Session<") << pkSession->Addr().ToString() << _T(">") );
			return;
		}

		SetUserState( nSessionKey, wstrID, ESITE_USER_LOGIN, false );
	}
	else
	{
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ );
	}
}

void PgSiteMemberMgr::Recv_PT_IM_CN_ANS_USER_TERMINATE_ERROR( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	std::wstring wstrID;
	if ( true == pkPacket->Pop(wstrID) )
	{
		SessionKey const nSessionKey = GetSessionKey( pkSession );
		if ( IMM_SERVER_SITE_NUM_ERROR == nSessionKey )
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Session<") << pkSession->Addr().ToString() << _T(">") );
			return;
		}

		SetUserState( nSessionKey, wstrID, ESITE_USER_CHECK, false );
	}
	else
	{
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ );
	}
}

void PgSiteMemberMgr::Timer5s()
{
	static ACE_UINT64 ui64Begin = 0;
	if (ui64Begin == 0i64)
	{
		// 처음 실행
		ui64Begin = BM::GetTime64();
	}
	else
	{
		if (m_eState != ECN_STATE_READYTOSERVICE)
		{
			if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
			{
				INFO_LOG(BM::LOG_LV6, __FL__ << _T("........... CONSENT SERVICE READY ............"));
				m_eState = ECN_STATE_READYTOSERVICE;
			}
			else
			{
				ACE_UINT64 ui64Now = BM::GetTime64();
				if (ui64Now - ui64Begin > IMMIGRATION_TRY_CONNECT_TIMEOUT)
				{
					INFO_LOG(BM::LOG_LV6, __FL__ << _T("........... CONSENT SERVICE READY ............"));
					m_eState = ECN_STATE_READYTOSERVICE;
				}
			}
		}
	}
}

void PgSiteMemberMgr::Locked_OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);

	g_kProcessCfg.Locked_OnDisconnectServer( pkSession );

	// ImmigrationServer 접속 종료되면, 해당 유저 정보 모두 clear
	short sSite = GetSessionKey(pkSession);

	RemoveSiteUserList( sSite );
	PgSessionMgr::OnDisconnected(pkSession);
}

bool PgSiteMemberMgr::SendToImmigration(std::wstring const& rkAccountID, BM::Stream const& rkPacket)
{
	CONT_USER_ID::const_iterator itr = m_kContUserID.find( rkAccountID );
	if ( itr != m_kContUserID.end() )
	{
		return SendToServer((*itr).second.nSite, rkPacket);
	}
	return false;
}