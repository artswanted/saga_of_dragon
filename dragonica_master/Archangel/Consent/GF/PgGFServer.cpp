#include "stdafx.h"
#include "Lohengrin/Packetstruct_GFAP.h"
#include "GFMobileLockPacketDef.h"
#include "PgMsgWorker.h"
#include "PgGFServer.h"

PgGFServer::PgGFServer(void)
:	m_bUseMobileLock(true)
{
//	중복로그인시에 유저를 끊어버릴라면 여기 주석을 풀어라
	m_bDisconnectDuplicatedUser = true;
}

PgGFServer::~PgGFServer(void)
{
}

void CALLBACK PgGFServer::OnConnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kGFServer.Locked_OnConnectFromImmigration(pkSession);
}

void CALLBACK PgGFServer::OnDisconnectFromImmigration( CEL::CSession_Base *pkSession )
{
	g_kGFServer.Locked_OnDisconnectFromImmigration(pkSession);
}

void CALLBACK PgGFServer::OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kGFServer.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void CALLBACK PgGFServer::OnConnectFromMobileLock( CEL::CSession_Base *pkSession )
{
	g_kGFServer.Locked_OnConnectFromMobileLock( pkSession );
}

void CALLBACK PgGFServer::OnDisConnectFromMobileLock( CEL::CSession_Base *pkSession )
{
	g_kGFServer.Locked_OnDisconnectFromMobileLock( pkSession );
}

void CALLBACK PgGFServer::OnRecvFromMobileLock(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if ( true == pkPacket->Pop(usType) )
	{
		g_kGFServer.Locked_RecvFromMobileLock( usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgGFServer::Locked_OnConnectFromMobileLock( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnConnected(pkSession);
}

void PgGFServer::Locked_OnDisconnectFromMobileLock( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected(pkSession);
}

bool PgGFServer::Locked_OnRegistConnector( CEL::SRegistResult const &rkArg )
{
	BM::CAutoMutex kLock(m_kMutex);

	std::wstring const wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_AP_Config.ini") );

	switch (rkArg.iIdentityValue)
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, wstrFilename );
		}break;
	case CEL::ST_EXTERNAL1:
		{
			TCHAR chValue[100] = {0,};
			int iValue = 0;
			::GetPrivateProfileString(_T("MOBILELOCK"), _T("IP"), _T("0.0.0.0"), chValue, sizeof(chValue), wstrFilename.c_str());
			iValue = ::GetPrivateProfileInt(_T("MOBILELOCK"), _T("PORT"), 0, wstrFilename.c_str() );
			AddAddress( MOBILELOCK_SERVER_NUM, rkArg.guidObj, chValue, static_cast<WORD>(iValue) );

			LoadFromConfig();
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue );
			return false;
		}break;
	}
	return true;
}

void PgGFServer::Locked_CheckMobileLockWait()
{
	BM::CAutoMutex kLock( m_kContMobileLockWait.GetLock() );

	ACE_UINT64 const u64NowTime = BM::GetTime64();

	PgMobileAuth< BM::GUID, SMobileAuthElement >::CONT::const_iterator itr = m_kContMobileLockWait.Begin();
	for ( ; itr != m_kContMobileLockWait.End() ; ++itr )
	{
		if ( itr->second.u64TimeOutTime > u64NowTime )
		{
		}
		else
		{
			// Time Out
			CAUTION_LOG( BM::LOG_LV5, _T("[MobileLock] Wait TimeOver UserID = ") << itr->second.kAuthInfo.ID() );

			SMsg *pkMsg = new SMsg;
			if ( pkMsg )
			{
				S_PT_GF_AP_ANS_MOBILE_AUTH kData;
				::wcscpy_s( kData.wszAccount, MAX_STRING_LEN_MOBILELOCK, itr->second.kAuthInfo.ID().c_str() );
				kData.byRet = GFAP_FUNCTION::GF_MOBILE_AUTH_TIMEOUT;
				kData.kSessionID = itr->second.kAuthInfo.SessionKey().SessionGuid();

				pkMsg->m_kContTarget.insert( kData.kSessionID );
				pkMsg->m_kPacket.Push( PT_GF_AP_ANS_MOBILE_AUTH );
				kData.WriteToPacket( pkMsg->m_kPacket );

				g_kMsgDisPatcher.VPush( pkMsg );
			}
		}
	}
}

void PgGFServer::Locked_RecvFromMobileLock( BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	switch( usType )
	{
	case PT_GF_AP_ANS_MOBILE_AUTH:
		{
			S_PT_GF_AP_ANS_MOBILE_AUTH kRecvData;
			if ( true == kRecvData.ReadFromPacket( *pkPacket ) )
			{
				SMobileAuthElement kElement;
				if ( S_OK == m_kContMobileLockWait.Pop( kRecvData.kSessionID, kElement ) )
				{
					ETryLoginResult eRet = E_TLR_MOBILE_LOCK_FAILED;

					switch( kRecvData.byRet )
					{
					case GFAP_FUNCTION::GF_MOBILE_AUTH_SUCCESS:
						{
//							if ( ESITE_USER_SUCCESS == SetUserState( kElement.nSite, std::wstring(kElement.kAuthInfo.szID), ESITE_USER_MOBILELOCK, false ) )
//							{
								eRet = E_TLR_SUCCESS;
//							}
						}break;
					case GFAP_FUNCTION::GF_MOBILE_AUTH_OVERLAPPING:
						{
							eRet = E_TLR_MOBILE_LOCK_FAILED_OVERLAP1;
						}break;
					case GFAP_FUNCTION::GF_MOBILE_AUTH_OVERLAPPING2:
						{
							eRet = E_TLR_MOBILE_LOCK_FAILED_OVERLAP2;
						}break;
					default:
						{
							eRet = E_TLR_MOBILE_LOCK_FAILED_TIMEOUT;
						}break;
					}
					
// 					if ( E_TLR_SUCCESS != eRet )
// 					{
// 						// 정보는 없애야 한다.
// 						SetUserState( kElement.nSite, std::wstring(kElement.kAuthInfo.szID), ESITE_USER_MOBILELOCK|ESITE_USER_LOGIN, false );
// 					}

					BM::Stream kRetPacket( PT_CN_IM_ANS_LOGIN_LOGIN, eRet );
					kElement.kAuthInfo.WriteToPacket(kRetPacket);
					SendToServer( kElement.nSite, kRetPacket );
				}
				else
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not Found SessionID<") << kRecvData.kSessionID << _T(">") );
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, _T("[PT_GF_AP_ANS_MOBILE_AUTH] Packet Error!! Packet Size<") << pkPacket->Size() << _T(">") );
			}
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Unknown packet type=") << usType );
		}break;
	}
}

void PgGFServer::RecvFromImmigration( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	switch( usType )
	{
	case PT_IM_CN_REQ_LOGIN_LOGIN:
		{
			SessionKey const nSessionKey = GetSessionKey( pkSession );
			if ( IMM_SERVER_SITE_NUM_ERROR != nSessionKey )
			{
				SAuthInfo kAuthInfo;
				kAuthInfo.ReadFromPacket(*pkPacket);

				// 여기로 오면 무조건 모바일 락이다.
/*
				bool const bMobileLock = ((true == kAuthInfo.bNewLogin) && (EXT_VAR_MOBILELOCK & kAuthInfo.nExtVar) && (true==m_bUseMobileLock) );

				BYTE const byState = ( bMobileLock ? ESITE_USER_LOGIN|ESITE_USER_MOBILELOCK : ESITE_USER_LOGIN );

				ETryLoginResult eRet = Recv_PT_IM_CN_REQ_LOGIN_LOGIN( nSessionKey, kAuthInfo, byState );
				if ( true == bMobileLock )
				{
					if ( E_TLR_SUCCESS == eRet )
					{
*/
						ETryLoginResult eRet = E_TLR_MOBILE_LOCK;
						
						SMobileAuthElement kMobileAuthElement( kAuthInfo, nSessionKey );

						// 모바일 락을 해야 한다.
						SYSTEMTIME kSystemTime;
						::GetSystemTime( &kSystemTime );

						S_PT_AP_GF_REQ_MOBILE_AUTH kReqData( &kSystemTime, &(kAuthInfo.ID()), kMobileAuthElement.kAuthInfo.SessionKey().SessionGuid() );

						BM::Stream kReqPacket( PT_AP_GF_REQ_MOBILE_AUTH );
						kReqData.WriteToPacket( kReqPacket );
						if ( true == Locked_SendToServer( MOBILELOCK_SERVER_NUM, kReqPacket ) )
						{
							m_kContMobileLockWait.Add( kMobileAuthElement.kAuthInfo.SessionKey().SessionGuid(), kMobileAuthElement, true );
						}
						else
						{
//							SetUserState( nSessionKey, std::wstring(kAuthInfo.szID), byState, false );
							CAUTION_LOG( BM::LOG_LV5, _T("[PT_IM_AP_REQ_MOBILE_AUTH] Failed MobileLock Server Dissconnect PassMobile Lock ID<") << kAuthInfo.ID() << _T(">") );
							eRet = E_TLR_MOBILE_LOCK_FAILED;
						}
//					}
//				}

				BM::Stream kAnsPacket( PT_CN_IM_ANS_LOGIN_LOGIN, eRet );
				kAuthInfo.WriteToPacket(kAnsPacket);
				pkSession->VSend( kAnsPacket );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!") );
			}
		}break;

	case PT_IM_CN_REQ_LOGIN_AUTH:
	case PT_IM_CN_NFY_LOGOUT_AUTH:
	case PT_IM_CN_ANS_USER_TERMINATE_ERROR:
	case PT_IM_CN_NFY_LOGOUT_LOGIN:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Packet Type " << usType << L" Error!!" );
		}break;
	case PT_IM_CN_ANS_LOGIN_LOGIN_FAILED:
		{
			// 이 패킷이 올수는 있지만 아무것도 하지 않는다.
		}break;
	case PT_IM_AP_ANS_ORDER:
		{
			SMsg *pkMsg = new SMsg;
			if ( pkMsg )
			{
				if ( true == pkPacket->Pop( pkMsg->m_kSessionID ) )
				{
					pkMsg->m_kContTarget.insert( BM::GUID::Create() );
					pkMsg->m_kPacket.Swap(*pkPacket);
					pkMsg->m_kPacket.PosAdjust();
					g_kMsgDisPatcher.VPush( pkMsg );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Msg Create Failed") );
			}
		}break;
	case PT_AP_GF_ANS_SET_TABLEDATA:
		{
			SMsg *pkMsg = new SMsg;
			if ( pkMsg )
			{
				BM::GUID kRequestID;
				pkPacket->Pop( kRequestID );
				
				pkMsg->m_kContTarget.insert( kRequestID );
				pkMsg->m_kPacket.Swap(*pkPacket);
				pkMsg->m_kPacket.PosAdjust();
				g_kMsgDisPatcher.VPush( pkMsg );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Msg Create Failed") );
			}
		}break;
	default:
		{
			PgSiteMemberMgr::RecvFromImmigration(pkSession, usType, pkPacket);
		}break;
	}
}

void PgGFServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock(m_kMutex);

	Timer5s();
}

void PgGFServer::Locked_OnRecvFromImmigration(CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration(pkSession, usType, pkPacket);
}

void PgGFServer::Locked_LoadFromConfig(void)
{
	BM::CAutoMutex kLock( m_kMutex );
	LoadFromConfig();
}

void PgGFServer::LoadFromConfig(void)
{
	std::wstring const wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_AP_Config.ini") );
	int const iValue = ::GetPrivateProfileInt(_T("MOBILELOCK"), _T("USE"), 1, wstrFilename.c_str() );
	m_bUseMobileLock = (0 < iValue);

	if ( true == m_bUseMobileLock )
	{
		INFO_LOG( BM::LOG_LV5, L"[MobileLock] Use!!" );
	}
	else
	{
		INFO_LOG( BM::LOG_LV5, L"[MobileLock] Not Use!!" );
	}
}
