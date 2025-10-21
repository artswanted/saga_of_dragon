#include "stdafx.h"
#include "Variant/Global.h"
#include "GFBillingAPPacketDef.h"
#include "PgBillingAPServer.h"

PgBillingAPServer::PgBillingAPServer(void)
:	m_pkServerSession(NULL)
,	m_bAuthSuccess(false)
,	m_dwLastRecvPingTime(0)
{

}

PgBillingAPServer::~PgBillingAPServer(void)
{

}

void CALLBACK PgBillingAPServer::OnAcceptFromBillingAP( CEL::CSession_Base *pkSession )
{
	g_kBillingAPServer.Locked_OnAcceptServer( pkSession );
}

void CALLBACK PgBillingAPServer::OnDisConnectFromBillingAP( CEL::CSession_Base *pkSession )
{
	g_kBillingAPServer.Locked_OnDisconnectServer( pkSession );
}

void CALLBACK PgBillingAPServer::OnRecvFromBillingAP(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	BM::Stream::DEF_STREAM_TYPE kType = 0;
	if ( true == pkPacket->Pop( kType ) )
	{
		INFO_LOG( BM::LOG_LV8, _T("[Recv Billing Packet Type<") << kType << _T(">") );
		g_kBillingAPServer.RecvFromServer( pkSession, kType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}

void PgBillingAPServer::Locked_OnAcceptServer( CEL::CSession_Base *pkSession )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( m_pkServerSession )
	{
		DWORD const dwCurTime = BM::GetTime32();
		if ( ms_dwMaxDelayPingTime < ::DifftimeGetTime( m_dwLastRecvPingTime, dwCurTime ) )
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Accepted BillingAP Error Old Server(LaziPing) : ") << m_pkServerSession->Addr().ToString() << _T(" Terminate!!") );
			m_pkServerSession->VTerminate();

			m_pkServerSession = NULL;
			m_bAuthSuccess = false;
		}
		else
		{
			// 접속을 끊어야 한다.
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Accepted BillingAP Error This Server : ") << pkSession->Addr().ToString() << _T(" Terminate!!") );
			pkSession->VTerminate();
			return;
		}
	}

	std::wstring const wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_AP_Config.ini") );
	if ( !ReadFromConfig( wstrFilename.c_str() ) )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("ReadFromConfig<") << wstrFilename << _T("> Failed!! Server : ") << pkSession->Addr().ToString() << _T(" Terminate!!") );
		pkSession->VTerminate();
		return;
	}

	if ( m_kServerInfo.kAddr.ip.s_addr != pkSession->Addr().ip.s_addr )
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Accepte IP Error : ") << pkSession->Addr().ToString() << _T(" Terminate!!") );
		pkSession->VTerminate();
		return;
	}

	m_pkServerSession = pkSession;
}

void PgBillingAPServer::Locked_OnDisconnectServer( CEL::CSession_Base *pkSession )
{
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("DisConnected : ") << pkSession->Addr().ToString() );

	BM::CAutoMutex kLock( m_kMutex );
	if ( m_pkServerSession == pkSession )
	{
		m_pkServerSession = NULL;
		m_bAuthSuccess = false;
	}
}

bool PgBillingAPServer::ReadFromConfig( LPCTSTR lpszFileName )
{
	m_kServerInfo = SLinkageServerInfo();

	TCHAR wszTemp[MAX_PATH] = {0,};
	::GetPrivateProfileString( _T("BILLINGAP"), _T("IP"), _T("0.0.0.0"), wszTemp, MAX_PATH, lpszFileName );
	m_kServerInfo.kAddr.Set( wszTemp, 0 );

	::GetPrivateProfileString( _T("BILLINGAP"), _T("ID"), _T(""), wszTemp, MAX_PATH, lpszFileName );
	m_kServerInfo.wstrID = wszTemp;

	::GetPrivateProfileString( _T("BILLINGAP"), _T("PW"), _T(""), wszTemp, MAX_PATH, lpszFileName );
	m_kServerInfo.wstrPW = wszTemp;
	
	return true;
}

void PgBillingAPServer::RecvFromServer( CEL::CSession_Base *pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket )
{
	{
		BM::CAutoMutex kLock( m_kMutex );
		if ( pkSession != m_pkServerSession )
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("No Permission Server : ") << pkSession->Addr().ToString() << _T(" Terminate!!") );
			pkSession->VTerminate();
			return;
		}
	}

	if ( true == m_bAuthSuccess )
	{
		// 여기서는 락을 않잡게 해놔야 한다!!!!!
		int iRet = GFAP_RET_SUCCESS;

		switch( usType )
		{
		case PT_LINKAGE_BASE:
			{// Ping
				m_dwLastRecvPingTime = BM::GetTime32();

				pkPacket->PosAdjust();
				pkSession->VSend( *pkPacket );
			}break;
		case PT_GF_AP_REQ_OVERLAP_ACCOUNT:
			{
				S_PT_GF_AP_REQ_OVERLAP_ACCOUNT kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_OVERLAP_ACCOUNT, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_OVERLAP_ACCOUNT kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_OVERLAP_ACCOUNT );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_CREATE_ACCOUNT:
			{
				S_PT_GF_AP_REQ_CREATE_ACCOUNT kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_CREATE_ACCOUNT, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_CREATE_ACCOUNT kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_CREATE_ACCOUNT );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_CHECK_ACCOUNT:
			{
				S_PT_GF_AP_REQ_CHECK_ACCOUNT kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_CHECK_ACCOUNTPW, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_CHECK_ACCOUNT kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_CHECK_ACCOUNT );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_GET_GAMEPOINT:
			{
				S_PT_GF_AP_REQ_GET_GAMEPOINT kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_GET_CASH, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_GET_GAMEPOINT kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_GET_GAMEPOINT );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_ADD_GAMEPOINT:
			{
				S_PT_GF_AP_REQ_ADD_GAMEPOINT kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_ADD_CASH, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_ADD_GAMEPOINT kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_ADD_GAMEPOINT );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_MODIFY_PASSWORD:
			{
				S_PT_GF_AP_REQ_MODIFY_PASSWORD kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_MODIFY_PASSWORD, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_MODIFY_PASSWORD kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_MODIFY_PASSWORD );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_USE_COUPON:
			{
				S_PT_GF_AP_REQ_USE_COUPON kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_USE_COUPON, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_USE_COUPON kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_USE_COUPON );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_MODIFY_MOBILELOCK:
			{
				S_PT_GF_AP_REQ_MODIFY_MOBILELOCK kRecvData;
				iRet = this->ProcessPacketAction( GFAP_F_MODIFY_MOBILELOCK, *pkPacket, kRecvData );
				if ( GFAP_RET_SUCCESS != iRet )
				{
					S_PT_AP_GF_ANS_MODIFY_MOBILELOCK kFail( kRecvData, iRet );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_MODIFY_MOBILELOCK );
					kFail.WriteToPacket( kFailedPacket );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		case PT_GF_AP_REQ_SET_TABLEDATA:
			{
				pkPacket->PosAdjust();
				if ( !g_kGFServer.Locked_SendToImmigrationOne( *pkPacket ) )
				{
					int iRet = GFAP_RET_SERVERERROR;
					BM::GUID kRequestID;
					pkPacket->Pop( kRequestID );

					BM::Stream kFailedPacket( PT_AP_GF_ANS_SET_TABLEDATA, kRequestID );
					kFailedPacket.Push( iRet );
					pkSession->VSend( kFailedPacket );
				}
			}break;
		default:
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unknown PacketType<") << usType << _T(" Server<") << m_kServerInfo.kAddr.ToString() << _T(">") );
			}break;
		}
	}
	else
	{
		BM::CAutoMutex kLock( m_kMutex );
		if ( PT_GF_AP_REQ_SERVER_LOGIN == usType )
		{
			S_PT_GF_AP_REQ_SERVER_LOGIN kRecvData;
			if ( true == kRecvData.ReadFromPacket( *pkPacket ) )
			{
				if ( true == m_kServerInfo.IsAccess( pkSession->Addr(), kRecvData.wszAccount, kRecvData.wszPassword ) )
				{
					INFO_LOG( BM::LOG_LV6, _T("[APServerLogin] Success Server<") << pkSession->Addr().ToString() << _T(">") );
					m_bAuthSuccess = true;
					m_dwLastRecvPingTime = BM::GetTime32();

					BM::Stream kAnsPacket( PT_AP_GF_ANS_SERVER_LOGIN_SUCCESS );
					pkSession->VSend( kAnsPacket );
				}
				else
				{
					INFO_LOG( BM::LOG_LV5, _T("[APServerLogin] Failed Server ID<") << kRecvData.wszAccount << _T("> PW<") << kRecvData.wszPassword << _T("> Address<") << pkSession->Addr().ToString() << _T("> Terminate!!") );
					pkSession->VTerminate();
				}
			}
			else
			{
				INFO_LOG( BM::LOG_LV5, _T("[APServerLogin] PacketError Server<") << pkSession->Addr().ToString() << _T("> Terminate!!") );
				pkSession->VTerminate();
			}
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("No ServerLogin PacketType<") << usType << _T("> Server : ") << pkSession->Addr().ToString() );
		}
	}
}

HRESULT PgBillingAPServer::Locked_OnRecvFromImmigration( BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream &kPacket )
{
	BM::CAutoMutex kLock(m_kMutex);

	switch ( usType )
	{
	case PT_IM_AP_ANS_ORDER:
		{
			BM::GUID kOrderID;
			int iOrderType = 0;
			kPacket.Pop( kOrderID );// SessionID가 저장되어있다.
			if ( true == kPacket.Pop( kOrderID ) )
			{
				if ( true == kPacket.Pop( iOrderType ) )
				{
					switch ( iOrderType )
					{
					case GFAP_F_OVERLAP_ACCOUNT:
						{// 계정체크
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_OVERLAP_ACCOUNT >( PT_AP_GF_ANS_OVERLAP_ACCOUNT, kOrderID, kPacket );
						}break;
					case GFAP_F_CREATE_ACCOUNT:
						{// 계정생성
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_CREATE_ACCOUNT >( PT_AP_GF_ANS_CREATE_ACCOUNT, kOrderID, kPacket );
						}break;
					case GFAP_F_CHECK_ACCOUNTPW:
						{// 계정비밀번호+Password
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_CHECK_ACCOUNT >( PT_AP_GF_ANS_CHECK_ACCOUNT, kOrderID, kPacket );
						}break;
					case GFAP_F_GET_CASH:
						{// 캐쉬 검색
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_GET_GAMEPOINT >( PT_AP_GF_ANS_GET_GAMEPOINT, kOrderID, kPacket );
						}break;
					case GFAP_F_ADD_CASH:
						{// 캐쉬 추가
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_ADD_GAMEPOINT >( PT_AP_GF_ANS_ADD_GAMEPOINT, kOrderID, kPacket );
						}break;
					case GFAP_F_MODIFY_PASSWORD:
						{// 패스워드 수정
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_MODIFY_PASSWORD >( PT_AP_GF_ANS_MODIFY_PASSWORD, kOrderID, kPacket );
						}break;
					case GFAP_F_USE_COUPON:
						{// 쿠폰 사용
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_USE_COUPON >( PT_AP_GF_ANS_USE_COUPON, kOrderID, kPacket );
						}break;
					case GFAP_F_MODIFY_MOBILELOCK:
						{// 모바일락 사용 여부 수정
							return this->Locked_ProcessReplyOrder< S_PT_AP_GF_ANS_MODIFY_MOBILELOCK >( PT_AP_GF_ANS_MODIFY_MOBILELOCK, kOrderID, kPacket );
						}break;
					default:
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ _T("Unknow OrderType<") << iOrderType << _T(">") );
						}break;
					}
				}
			}
		}break;
	case PT_AP_GF_ANS_SET_TABLEDATA:
		{
			kPacket.PosAdjust();
			Locked_Send( kPacket );
		}break;
	default:
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Unknown PacketType<") << usType << _T(">") );
		}break;
	}
	
	return E_FAIL;
}
