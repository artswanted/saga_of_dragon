#include "StdAfx.h"
#include "PgGravityServer.h"
#include "md5.h"
#include "AilePack/constant.h"

using namespace GRAVITY;

//////////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK PgGravityServer::OnConnectFromGravityAuth( CEL::CSession_Base* pkSession )
{	
	g_kGravity.Locked_OnConnectFromGravityAuth( pkSession );
}

void CALLBACK PgGravityServer::OnDisconnectFromGravityAuth( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnDisconnectFromGravityAuth( pkSession );
}

void CALLBACK PgGravityServer::OnRecvFromGravityAuth( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	g_kGravity.Locked_OnRecvFromGravityAuth( pkSession, pkPacket );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK PgGravityServer::OnConnectFromGravityItem( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnConnectFromGravityItem( pkSession );
}

void CALLBACK PgGravityServer::OnDisconnectFromGravityItem( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnDisconnectFromGravityItem( pkSession );
}

void CALLBACK PgGravityServer::OnRecvFromGravityItem( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	g_kGravity.Locked_OnRecvFromGravityItem( pkSession, pkPacket );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CALLBACK PgGravityServer::OnConnectFromGravityGM( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnConnectFromGravityGM( pkSession );
}

void CALLBACK PgGravityServer::OnDisconnectFromGravityGM( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnDisconnectFromGravityGM( pkSession );
}

void CALLBACK PgGravityServer::OnRecvFromGravityGM( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	g_kGravity.Locked_OnRecvFromGravityGM( pkSession, pkPacket );
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CALLBACK PgGravityServer::OnConnectFromImmigration( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnConnectFromImmigration( pkSession );
}

void CALLBACK PgGravityServer::OnDisconnectFromImmigration( CEL::CSession_Base* pkSession )
{
	g_kGravity.Locked_OnDisconnectFromImmigration( pkSession );
}

void CALLBACK PgGravityServer::OnRecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	if( pkPacket->Pop( usType ) )
	{
		g_kGravity.Locked_OnRecvFromImmigration( pkSession, usType, pkPacket );
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV0, __FL__, << _T("None Packet Type Session Addr = ") << pkSession->Addr().ToString() );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


PgGravityServer::PgGravityServer()
:m_TranId(0)
{	
	m_kContUserByUserId.clear();
	m_kContCashTranInfo.clear();
	m_kContUserBlockInfo.reserve(10);
}

PgGravityServer::~PgGravityServer()
{	
	RemoveUserInfoAll();
	m_kContCashTranInfo.clear();
	m_kContUserBlockInfo.clear();
}

unsigned long PgGravityServer::GetTranId()
{
	return ::InterlockedIncrement(&m_TranId);
}

void PgGravityServer::Locked_Timer5s()
{
	BM::CAutoMutex kLock( m_kMutex );
	Timer5s();
	KeepAlive();
}

void PgGravityServer::Timer5s()
{
	PgSiteMemberMgr::Timer5s();
}

void PgGravityServer::KeepAlive()
{	
	char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
	sprintf( acPacketType, "%04d", EGC_KEEP_ALIVE_REQ );

	BM::Stream kPacket;
	kPacket.Push(acPacketType, DEF_PACKET_SIZE);

	SendToServer( c_iGravityAuthServerNum, kPacket );
	SendToServer( c_iGravityItemServerNum, kPacket );
}

bool PgGravityServer::Locked_OnRegistConnector( const CEL::SRegistResult& rkArg )
{
	BM::CAutoMutex kLock(m_kMutex);

	std::wstring kFileName( g_kProcessCfg.ConfigDir() + _T("Consent_Gravity_Config.ini") );
	switch( rkArg.iIdentityValue )
	{
	case CEL::ST_IMMIGRATION:
		{
			LoadImmigration( rkArg.guidObj, kFileName );
		}break;
	case CEL::ST_EXTERNAL1: // Auth
		{
			TCHAR wszIP[MAX_PATH] = { 0, };
			int iPort = 0;
			GetPrivateProfileString( _T("GRAVITY_AUTH"), _T("IP"), _T("0,0,0,0"), wszIP, sizeof(wszIP), kFileName.c_str() );
			iPort = GetPrivateProfileInt( _T("GRAVITY_AUTH"), _T("PORT"), 0, kFileName.c_str() );
			AddAddress( GRAVITY::c_iGravityAuthServerNum, rkArg.guidObj, wszIP, static_cast<DWORD>(iPort) );
		}break;
	case CEL::ST_EXTERNAL2: // Item
		{
			TCHAR wszIP[MAX_PATH] = { 0, };
			int iPort = 0;
			GetPrivateProfileString( _T("GRAVITY_ITEM"), _T("IP"), _T("0,0,0,0"), wszIP, sizeof(wszIP), kFileName.c_str() );
			iPort = GetPrivateProfileInt( _T("GRAVITY_ITEM"), _T("PORT"), 0, kFileName.c_str() );
			AddAddress( GRAVITY::c_iGravityItemServerNum, rkArg.guidObj, wszIP, static_cast<DWORD>(iPort) );
		}break;
	case CEL::ST_EXTERNAL3: // GM
		{
			TCHAR wszIP[MAX_PATH] = { 0, };
			int iPort = 0;
			GetPrivateProfileString( _T("GRAVITY_GM"), _T("IP"), _T("0,0,0,0"), wszIP, sizeof(wszIP), kFileName.c_str() );
			iPort = GetPrivateProfileInt( _T("GRAVITY_GM"), _T("PORT"), 0, kFileName.c_str() );
			AddAddress( GRAVITY::c_iGravityGMServerNum, rkArg.guidObj, wszIP, static_cast<DWORD>(iPort) );
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Unknown ServerType=") << rkArg.iIdentityValue);
			return false;
		}break;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void PgGravityServer::Locked_OnConnectFromImmigration( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnConnected( pkSession );
}

void PgGravityServer::Locked_OnDisconnectFromImmigration( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected( pkSession );
}

void PgGravityServer::Locked_OnRecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock(m_kMutex);
	RecvFromImmigration( pkSession, usType, pkPacket );
}

void PgGravityServer::RecvFromImmigration( CEL::CSession_Base* pkSession, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream* const pkPacket )
{
	switch( usType )
	{
	case PT_IM_CN_REQ_LOGIN_AUTH_GRAVITY:
		{// 로그인 인증
			RecvPT_IM_CN_REQ_LOGIN_AUTH_GRAVITY( pkSession, pkPacket );
		}break;
	case PT_IM_CN_NFY_LOGOUT_GRAVITY:
	{
		RecvPT_IM_CN_NFY_LOGOUT_GRAVITY( pkSession, pkPacket );
	}break;
	case PT_IM_CN_NFY_BLOCKUSER_GRAVITY:
		{// CM툴 유저 블록을 빌링 서버에 전송
			RecvPT_IM_CN_NFY_BLOCKUSER_GRAVITY( pkSession, pkPacket );
		}break;
	case PT_N_X_REQ_CASH:
		{
			SERVER_IDENTITY kFromServerIdentity;
			kFromServerIdentity.ReadFromPacket(*pkPacket);			
			SERVER_IDENTITY_SITE kServerIdentitySite( kFromServerIdentity, GetSessionKey( pkSession ) );

			BM::Stream::DEF_STREAM_TYPE wSubType;
			pkPacket->Pop(wSubType);
			switch( wSubType )
			{
			case PT_A_CN_REQ_QUERY_CASH:
				{// 캐시 잔액 조회
					RecvPT_A_CN_REQ_QUERY_CASH( kServerIdentitySite, pkPacket );
				}break;
			case PT_A_CN_REQ_BUYCASHITEM:	// 아이템 구매
			case PT_A_CN_REQ_EXPANDTIEM:	// 기간연장
				{
					RecvPT_A_CN_REQ_BUYCASHITEM( kServerIdentitySite, pkPacket );
				}break;
			case PT_A_CN_REQ_SENDGIFT:
				{
					RecvPT_A_CN_REQ_SENDGIFT( kServerIdentitySite, pkPacket );
				}break;			
			default:
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Unknown type [PT_N_X_REQ_CASH]->") << wSubType);
				}break;
			}
		}
	default:
		{
			PgSiteMemberMgr::RecvFromImmigration( pkSession, usType, pkPacket );
		}break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void PgGravityServer::Locked_OnConnectFromGravityGM( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	if( PgSessionMgr::OnConnected(pkSession) )
	{
		CONT_USERBLOCK_INFO::iterator userBlock_iter = m_kContUserBlockInfo.begin();
		while( m_kContUserBlockInfo.end() != userBlock_iter )
		{
			CONT_USERBLOCK_INFO::value_type kBlockUserInfo = (*userBlock_iter);

			char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
			sprintf( acPacketType, "%04d", EGC_USER_BLOCK_REQ );

			BM::Stream kPacket;
			kPacket.Push( acPacketType, DEF_PACKET_SIZE );
			kPacket.Push( acGroupNum, DEF_GROUPNUM_SIZE );
			kPacket.Push( acHyper, DEF_HYPER_SIZE );// 첫번째 data 이후에 | 로 구분.
			kPacket.Push( acGameNum, DEF_GAMENUM_SIZE );
			kPacket.Push( acHyper, DEF_HYPER_SIZE );
			kPacket.Push( kBlockUserInfo.c_str(), kBlockUserInfo.length() );

			if( !SendToServer( c_iGravityGMServerNum, kPacket ) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV2, _T("SendToServer Failed.") );
				return ;
			}

			++userBlock_iter;
		}

		m_kContUserBlockInfo.clear();
	}
}

void PgGravityServer::Locked_OnDisconnectFromGravityGM( CEL::CSession_Base* pkSession )
{// nothing	
}

void PgGravityServer::Locked_OnRecvFromGravityGM( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	char acPacketType[4];
	ZeroMemory( acPacketType, sizeof( acPacketType) );
	pkPacket->PopMemory( acPacketType, sizeof( acPacketType) );

	const int iPacketType = atoi( acPacketType );
	switch( iPacketType )
	{
	case EGC_USER_BLOCK_PRE_ACK:
		{// nothing
		}break;
	case EGC_USER_BLOCK_ACK:
		{
			RemoveAddress( GRAVITY::c_iGravityGMServerNum ); // GM서버로 재 접속 시도 하지 않도록(Timer_5s_Gravity)
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Packet Type = ") << iPacketType );
		}break;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PgGravityServer::Locked_OnConnectFromGravityAuth( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnConnected(pkSession);
}

void PgGravityServer::Locked_OnDisconnectFromGravityAuth( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected(pkSession);
}

void PgGravityServer::Locked_OnRecvFromGravityAuth( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	char acPacketType[4];
	ZeroMemory( acPacketType, sizeof( acPacketType) );
	pkPacket->PopMemory( acPacketType, sizeof( acPacketType) );

	const int iPacketType = atoi( acPacketType );
	switch( iPacketType )
	{	
	case EGC_AUTH_ACK:
		{
			SGravityAuth_Result kGravityAuthResult;
			kGravityAuthResult.ReadFromPacket(*pkPacket);

			bool bIsPCRoom = false;
			if( 0 < atoi( kGravityAuthResult.acIsPCRoom ) )
			{
				bIsPCRoom = true;
			}

			SAuthRequest kAuthRecv;
			kAuthRecv.bIsPCCafe = bIsPCRoom;
			kAuthRecv.strID = UNI(kGravityAuthResult.acUserId);
			kAuthRecv.strPW = UNI(kGravityAuthResult.acUserId); // PW는 ID와 같다.
			kAuthRecv.iUID =  atoi( kGravityAuthResult.acAId );
			kAuthRecv.byGMLevel = static_cast<BYTE>( atoi( kGravityAuthResult.acRight ) );

			const int iResult = atoi( kGravityAuthResult.acResult );
			ETryLoginResult eResult = E_TLR_NONE;
			switch( iResult )
			{
			case EGAR_SUCCESS:			{ eResult = E_TLR_SUCCESS; }break;
			case EGAR_WRONG_PWD:		{ eResult = E_TLR_WRONG_PASS; }break;
			case EGAR_NOT_CONFIRM_EMAIL:{ eResult = E_TLR_NOT_CONFIRM_EMAIL; }break;
			case EGAR_PAYPAL_BLOCK:		{ eResult = E_TLR_PAYPAL_BLOCK; }break;
			case EGAR_WEB_GAME_BLOCK:	{ eResult = E_TLR_BLOCKED_USER; }break;
			case EGAR_RESTRICT_AGE:		{ eResult = E_TLR_RESTRICT_AGE; }break;
			case EGAR_NOT_AVALABLE:		{ eResult = E_TLR_NOT_AVALABLE; }break;
			case EGAR_NOT_DATA:			{ eResult = E_TLR_WRONG_ID; }break;
			case EGAR_SYSTEM_ERROR:		{ eResult = E_TLR_EXTERNAL_SERVER_ERROR; }break;
			default:					{ eResult = E_TLR_EXTERNAL_SERVER_ERROR; }break;
			}
			
			CONT_USER_BY_USERID::iterator find_iter = m_kContUserByUserId.find( kAuthRecv.strID );
			if( m_kContUserByUserId.end() != find_iter )
			{
				CONT_USER_BY_USERID::mapped_type pkUser = find_iter->second;

				pkUser->kAuthInfo.iUID = kAuthRecv.iUID;
				SendLoginResult( NULL, *pkUser, eResult ); // 로그인 인증 결과
			}
			else
			{// 여기로 오는 상황이 발생해서는 안됨.
				SendLoginResult( NULL, SUserInfo( 0, kAuthRecv ), E_TLR_MEMORY_ERROR );
			}
		}break;	
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Packet Type = ") << iPacketType );
		}break;
	}
}


void PgGravityServer::Locked_OnRecvFromGravityItem( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	char acPacketType[4];
	ZeroMemory( acPacketType, sizeof( acPacketType) );
	pkPacket->PopMemory( acPacketType, sizeof( acPacketType) );

	const int iPacketType = atoi( acPacketType );
	switch( iPacketType )
	{
	case EGC_REMAIN_CASH_ACK:
		{
			SGravityReqCash_Result kGravityReqCashResult;
			kGravityReqCashResult.ReadFromPacket(*pkPacket);

			const int iResult = atoi( kGravityReqCashResult.acStatus );
			ECashShopResult eResult = GetCashResult( static_cast<EGravityItemResult>( iResult ) );
			
			unsigned long TranId = atoi( kGravityReqCashResult.acOrderNum );

			CONT_CASH_TRAN_INFO::iterator tran_iter = m_kContCashTranInfo.find( TranId );
			if( m_kContCashTranInfo.end() != tran_iter )
			{
				SGravityCashTranInfo kTranInfo = tran_iter->second;

				BM::Stream kPacket( PT_X_N_ANS_CASH );
				dynamic_cast<SERVER_IDENTITY const&>( kTranInfo.kSI ).WriteToPacket( kPacket );
				kPacket.Push( kTranInfo.wSubType );

				kPacket.Push( eResult );
				kPacket.Push( static_cast<__int64>( atoi( kGravityReqCashResult.acCashBalance) ) );
				kPacket.Push( static_cast<__int64>( atoi( kGravityReqCashResult.acCouponBalace ) ));

				kPacket.Push( kTranInfo.OrgPacket ); // PgTranPointer
				SendToServer( kTranInfo.kSI.nSiteNo, kPacket );

				m_kContCashTranInfo.erase( tran_iter );
			}
		}break;
	case EGC_BUY_ITEM_ACK:
		{// 아이템 구매 결과
			SGravityBuyItem_Result kGravityBuyItemResult;
			kGravityBuyItemResult.ReadFromPacket(*pkPacket);

			const int iResult = atoi( kGravityBuyItemResult.acStatus );
			ECashShopResult eResult = GetCashResult( static_cast<EGravityItemResult>( iResult ) );
			
			unsigned long TranId = atoi( kGravityBuyItemResult.acOrderNum );

			CONT_CASH_TRAN_INFO::iterator tran_iter = m_kContCashTranInfo.find( TranId );
			if( m_kContCashTranInfo.end() != tran_iter )
			{
				SGravityCashTranInfo kTranInfo = tran_iter->second;

				BM::Stream kPacket( PT_X_N_ANS_CASH );
				dynamic_cast<SERVER_IDENTITY const&>( kTranInfo.kSI ).WriteToPacket( kPacket );
				kPacket.Push( kTranInfo.wSubType );

				kPacket.Push( eResult );
				kPacket.Push( static_cast<__int64>( atoi( kGravityBuyItemResult.acRemainCash) ) );
				kPacket.Push( static_cast<__int64>( atoi( kGravityBuyItemResult.acRemainCoupon ) ) ); // 그라비티의 쿠폰인데, 마일리지는 아니다.임시로 마일리지 값으로 사용.(실제 사용안함)

				kPacket.Push( kTranInfo.OrgPacket ); // PgTranPointer
				SendToServer( kTranInfo.kSI.nSiteNo, kPacket );

				m_kContCashTranInfo.erase( tran_iter );
			}
		}break;	
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unknown Packet Type = ") << iPacketType );
		}break;
	}
}

ECashShopResult PgGravityServer::GetCashResult( const EGravityItemResult eResult )
{
	ECashShopResult eReturn = CSR_SYSTEM_ERROR;

	switch( eResult )
	{
	case EGIR_SUCCESS:
		{
			eReturn = CSR_SUCCESS;
		}break;
	case EGIR_ITEM_INFO_ERROR:	
	case EGIR_IS_NOT_ITEM1:
	case EGIR_IS_NOT_ITEM2:		
		{ 
			eReturn = CSR_INVALID_ARTICLE; 
		}break;
	case EGIR_NOT_ENOUGH_POINT: 
		{ 
			eReturn = CSR_NOT_ENOUGH_CASH; 
		}break;			
	case EGIR_INVALID_ACCOUNT:
		{
			eReturn = CSR_INVALID_CHARACTER;
		}break;
	default:
		{
			eReturn = CSR_SYSTEM_ERROR;
		}break;
	}

	return eReturn;
}

void PgGravityServer::RemoveUserInfoAll()
{
	SUserInfo* pkUser = NULL;

	CONT_USER_BY_USERID::iterator loop_iter = m_kContUserByUserId.begin();
	while( m_kContUserByUserId.end() != loop_iter )
	{
		CONT_USER_BY_USERID::mapped_type pkUser = loop_iter->second;
		if( NULL != pkUser )
		{
			delete pkUser;
		}

		++loop_iter;
	}

	m_kContUserByUserId.clear();
}

void PgGravityServer::RemoveUserInfo( const std::wstring& rkUserId )
{
	SUserInfo* pkUser = NULL;

	CONT_USER_BY_USERID::iterator find_iter = m_kContUserByUserId.find( rkUserId );	
	if( m_kContUserByUserId.end() != find_iter )
	{
		CONT_USER_BY_USERID::mapped_type pkUser = find_iter->second;
		if( NULL != pkUser )
		{
			delete pkUser;
		}

		m_kContUserByUserId.erase( find_iter );
	}
}

void PgGravityServer::SendLoginResult( CEL::CSession_Base* pkSession, const SUserInfo& rkUser, ETryLoginResult const eResult )
{
	SAuthRequest kAuth = rkUser.kAuthInfo;
	BM::Stream kPacket( PT_CN_IM_ANS_LOGIN_AUTH_GRAVITY );
	kAuth.WriteToPacket( kPacket );
	kPacket.Push( eResult );
	
	if( NULL != pkSession )
	{
		pkSession->VSend( kPacket );
	}
	else
	{
		PgSessionMgr::SendPacket( rkUser.sSite, kPacket );
	}	
	
	if( E_TLR_SUCCESS != eResult )
	{// Send 이후에 지울 것.
		RemoveUserInfo( kAuth.strID );
	}
}

void PgGravityServer::Locked_OnConnectFromGravityItem( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnConnected(pkSession);
}

void PgGravityServer::Locked_OnDisconnectFromGravityItem( CEL::CSession_Base* pkSession )
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSessionMgr::OnDisconnected(pkSession);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PgGravityServer::RecvPT_IM_CN_NFY_BLOCKUSER_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{// Gravity GMServer와의 통신은 매번 Connect - Send - Recv - DisConnect 과정을 거쳐야 한다.

	std::wstring kBlockUserInfo;
	pkPacket->Pop( kBlockUserInfo );
	
	m_kContUserBlockInfo.push_back( MB( kBlockUserInfo ) ); // Connect 되면 저장시켜 둔 정보로 Send 한다.

	// Connector 등록
	CEL::INIT_CORE_DESC kGravityGMConnectorInit;
	kGravityGMConnectorInit.OnSessionOpen	= PgGravityServer::OnConnectFromGravityGM;
	kGravityGMConnectorInit.OnDisconnect	= PgGravityServer::OnDisconnectFromGravityGM;
	kGravityGMConnectorInit.OnRecv			= PgGravityServer::OnRecvFromGravityGM;
	kGravityGMConnectorInit.kOrderGuid.Generate();
	kGravityGMConnectorInit.IdentityValue( CEL::ST_EXTERNAL3 );
	kGravityGMConnectorInit.ServiceHandlerType( CEL::SHT_GRAVITY );
	g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kGravityGMConnectorInit );
}

void PgGravityServer::RecvPT_IM_CN_NFY_LOGOUT_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	std::wstring kUserId;
	pkPacket->Pop( kUserId );
	RemoveUserInfo( kUserId );
}

void PgGravityServer::RecvPT_IM_CN_REQ_LOGIN_AUTH_GRAVITY( CEL::CSession_Base* pkSession, BM::Stream* const pkPacket )
{
	SAuthRequest kAuthRequest;
	kAuthRequest.ReadFromPacket(*pkPacket);
	if( kAuthRequest.strID.length() <=0 || kAuthRequest.strPW.length() <= 0 )
	{
		INFO_LOG( BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size() );
		CAUTION_LOG( BM::LOG_LV4, __FL__ << _T("Cannot Pop LoginInfo from packet, PacketTotalSize=") << pkPacket->Size() );
		SendLoginResult( pkSession, SUserInfo( 0, kAuthRequest ), E_TLR_WRONG_ID );
		return ;
	}
	
	short sSite = 0;
	bool bFindSite = PgSessionMgr::GetSessionKey( pkSession, sSite );
	SessionInfo* const pkSessionInfo = PgSessionMgr::GetSessionInfo( pkSession );
	if( !bFindSite
	||	!pkSessionInfo )
	{
		INFO_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		CAUTION_LOG(BM::LOG_LV3, __FL__ << _T("Cannot Find Session"));
		SendLoginResult( pkSession, SUserInfo( 0, kAuthRequest ), E_TLR_WRONG_ID );
		return;
	}
	
	SUserInfo* pkNew = new SUserInfo( sSite, kAuthRequest );
	if( NULL == pkNew )
	{
		INFO_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		CAUTION_LOG(BM::LOG_LV2, __FL__ << _T("new SUserInfo failed"));
		SendLoginResult( pkSession, SUserInfo( 0, kAuthRequest ), E_TLR_MEMORY_ERROR );
		delete pkNew;
		return;
	}

	auto pairResult = m_kContUserByUserId.insert( std::make_pair( kAuthRequest.strID, pkNew ) );
	if( false == pairResult.second )
	{
		INFO_LOG(BM::LOG_LV7, __FL__ << L"m_kContUserByAId.insert failed");
		SendLoginResult( pkSession, SUserInfo( 0, kAuthRequest ), E_TLR_MEMORY_ERROR );
		delete pkNew;
		return ;
	}

	// 패킷타입
	char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
	sprintf( acPacketType, "%04d", EGC_AUTH_REQ );

	// MD5
	std::string strUserPW = MB(kAuthRequest.strPW);
	static int const iDigestSize = 16;
	md5_state_t md5state;
	md5_byte_t digest[iDigestSize];
	md5_init(&md5state);
	md5_append(&md5state, (md5_byte_t const *)(strUserPW.c_str()), static_cast<int>(strUserPW.length()));
	md5_finish(&md5state, digest);
	std::wstring wstrHexaPW = BM::vstring::ConvToHex(digest, sizeof(digest), 100, false);
	BM::vstring::ConvToLWR(wstrHexaPW);

	SGravityAuth kGravityAuth;
	strncpy( kGravityAuth.acUserId, MB(kAuthRequest.strID), DEF_ACCOUNTID_SIZE );
	strncpy( kGravityAuth.acUserPwd, MB(wstrHexaPW), DEF_ACCOUNTPWD_SIZE );
	strncpy( kGravityAuth.acGameType, acGameType, DEF_GAMETYPE_SIZE );
	strncpy( kGravityAuth.acUserType, acUserType, DEF_USERTYPE_SIZE );
	strncpy( kGravityAuth.acIP, MB(kAuthRequest.kAddress.IP()), DEF_IP_SIZE );

	BM::Stream kPacket;
	kPacket.Push( acPacketType, DEF_PACKET_SIZE );
	kGravityAuth.WriteToPacket( kPacket );
	
	if( !SendToServer( c_iGravityAuthServerNum, kPacket ) )
	{
		SendLoginResult( NULL, SUserInfo( 0, kAuthRequest ), E_TLR_EXTERNAL_SERVER_ERROR ); // 로그인 인증 결과
	}
}

void PgGravityServer::RecvPT_A_CN_REQ_QUERY_CASH( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket )
{
 	std::wstring kAccountID;
 	unsigned int uiUserId;
 	CEL::ADDR_INFO kAddress;
 
 	pkPacket->Pop(kAccountID);
 	pkPacket->Pop(uiUserId);
 	kAddress.ReadFromPacket(*pkPacket);

	unsigned long TranId = GetTranId();
	SGravityCashTranInfo kCashTranInfo( rkFrom, TranId, PT_CN_A_ANS_QUERY_CASH, pkPacket );
	m_kContCashTranInfo.insert( std::make_pair( TranId, kCashTranInfo ) );

	// 패킷타입
	char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
	sprintf( acPacketType, "%04d", EGC_REMAIN_CASH_REQ );

	char acOrderNum[DEF_ORDERNUM_SIZE+1] = { 0, };
	strncpy( acOrderNum, MB( BM::vstring( TranId ) ) , DEF_ORDERNUM_SIZE );

	char acAId[DEF_ACCOUNTNUM_SIZE+1] = { 0, };
	_itoa( uiUserId, acAId, 10 );

	BM::Stream kPacket;
	kPacket.Push( acPacketType, DEF_PACKET_SIZE );
	kPacket.Push( acGroupNum, DEF_GROUPNUM_SIZE );
	kPacket.Push( acGameNum, DEF_GROUPNUM_SIZE );
	kPacket.Push( acAId, DEF_ACCOUNTNUM_SIZE );
	kPacket.Push( acOrderNum, DEF_ORDERNUM_SIZE );

	if( !SendToServer( c_iGravityItemServerNum, kPacket ) )
	{
		BM::Stream kPacket( PT_X_N_ANS_CASH );
		dynamic_cast<SERVER_IDENTITY const&>( rkFrom ).WriteToPacket( kPacket );
		kPacket.Push( PT_CN_A_ANS_QUERY_CASH );
		kPacket.Push( CSR_SYSTEM_ERROR );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( *pkPacket ); // PgTranPointer
		SendToServer( rkFrom.nSiteNo, kPacket );
	}
}

void PgGravityServer::RecvPT_A_CN_REQ_BUYCASHITEM( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket )
{
	std::wstring kAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring kCharacterName;
	unsigned int uiUserId;
	CASH::SCashCost kCost;
	CONT_BUYITEM kBuyItem;
	
	pkPacket->Pop(kAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(kCharacterName);
	pkPacket->Pop(uiUserId);	
	kCost.ReadFromPacket(*pkPacket); // 아이템이 여러개일 경우 합산된 가격
	PU::TLoadArray_M(*pkPacket, kBuyItem);

	if( uiUserId <= 0 )
	{
		BM::Stream kPacket( PT_X_N_ANS_CASH );
		dynamic_cast<SERVER_IDENTITY const&>( rkFrom ).WriteToPacket( kPacket );
		kPacket.Push( PT_CN_A_ANS_BUYCASHITEM );
		kPacket.Push( CSR_INVALID_CHARACTER );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( *pkPacket ); // PgTranPointer
		SendToServer( rkFrom.nSiteNo, kPacket );
		return ;
	}

	unsigned long TranId = GetTranId();
	SGravityCashTranInfo kCashTranInfo( rkFrom, TranId, PT_CN_A_ANS_BUYCASHITEM, pkPacket );
	m_kContCashTranInfo.insert( std::make_pair( TranId, kCashTranInfo ) );
	
	char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
	char acUserId[DEF_ACCOUNTNUM_SIZE+1] = { 0, };
	char acOrderNum[DEF_ORDERNUM_SIZE+1] = { 0, };
	char acTotalCashAmount[DEF_POINT_SIZE+1] = { 0, };
	char acTotalCouponAmount[DEF_POINT_SIZE+1] = { 0, };
	char acRecvGiftUserId[DEF_ACCOUNTNUM_SIZE+1] = { 0, }; // 선물일 경우 받는 유저의 AId
	char acIP[DEF_IP_SIZE+1] = { 0, };
	char acTotalItemCount[DEF_ITEMCOUNT_SIZE+1] = { 0, };
	
	sprintf( acPacketType, "%04d", EGC_BUY_ITEM_REQ );	
	_itoa( uiUserId, acUserId, 10 );
	strncpy( acOrderNum, MB( BM::vstring( TranId ) ), DEF_ORDERNUM_SIZE );
	strncpy( acTotalCashAmount, MB( BM::vstring( abs( static_cast<int>(kCost.i64UsedCash) ) ) ), DEF_POINT_SIZE );
	strncpy( acTotalCouponAmount, MB( BM::vstring( abs( static_cast<int>(kCost.i64UsedMileage) ) ) ), DEF_POINT_SIZE );	
	strncpy( acIP, MB(kAddress.IP()), DEF_IP_SIZE );
	strncpy( acTotalItemCount, MB( BM::vstring( kBuyItem.size() ) ), DEF_POINT_SIZE );
	

	BM::Stream kPacket;
	kPacket.Push( acPacketType, DEF_PACKET_SIZE );
	kPacket.Push( acGroupNum, DEF_GROUPNUM_SIZE );
	kPacket.Push( acGameNum, DEF_GAMENUM_SIZE );
	kPacket.Push( acUserId, DEF_ACCOUNTNUM_SIZE );
	kPacket.Push( acOrderNum, DEF_ORDERNUM_SIZE );
	kPacket.Push( acTotalCashAmount, DEF_POINT_SIZE );
	kPacket.Push( acTotalCouponAmount, DEF_POINT_SIZE );
	kPacket.Push( acRecvGiftUserId, DEF_ACCOUNTNUM_SIZE ); // NULL
	kPacket.Push( acIP, DEF_IP_SIZE );
	kPacket.Push( acTotalItemCount, DEF_ITEMCOUNT_SIZE );

	const int iTotalItemCount = kBuyItem.size();
	for( int i = 0; i < iTotalItemCount; ++i )
	{
		char acItemNo[DEF_ITEMNO_SIZE+1] = { 0, };
		char acItemCount[DEF_ITEMCOUNT_SIZE+1] = { 0, };
		char acCash[DEF_POINT_SIZE+1] = { 0, };

		strncpy( acItemNo, MB( BM::vstring( kBuyItem.at(i).iItemNo ) ), DEF_ITEMNO_SIZE );
		strncpy( acItemCount, MB( BM::vstring( 1 ) ), DEF_ITEMCOUNT_SIZE ); // 개별 개수 1개 고정
		strncpy( acCash, MB( BM::vstring( abs( static_cast<int>(kBuyItem.at(i).i64Cash) ) ) ), DEF_POINT_SIZE );

		kPacket.Push( acItemNo, DEF_ITEMNO_SIZE );
		kPacket.Push( acItemCount, DEF_ITEMCOUNT_SIZE );
		kPacket.Push( acCash, DEF_POINT_SIZE );
	}
	
	if( !SendToServer( c_iGravityItemServerNum, kPacket ) )
	{
		BM::Stream kPacket( PT_X_N_ANS_CASH );
		dynamic_cast<SERVER_IDENTITY const&>( rkFrom ).WriteToPacket( kPacket );
		kPacket.Push( PT_CN_A_ANS_BUYCASHITEM );
		kPacket.Push( CSR_SYSTEM_ERROR );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( *pkPacket ); // PgTranPointer
		SendToServer( rkFrom.nSiteNo, kPacket );
	}
}

void PgGravityServer::RecvPT_A_CN_REQ_SENDGIFT( SERVER_IDENTITY_SITE const& rkFrom, BM::Stream* const pkPacket )
{
	unsigned int uiUserId;
	std::wstring strAccountID;
	CEL::ADDR_INFO kAddress;
	BM::GUID kCharacterGuid;
	std::wstring strCharacterName;
	std::wstring strReceiverName;
	unsigned int uiItemId;
	std::wstring strItemName;
	WORD wItemTime;
	CASH::SCashCost kCost;
	unsigned int uiReceiverUserId;

	pkPacket->Pop(uiUserId);
	pkPacket->Pop(strAccountID);
	kAddress.ReadFromPacket(*pkPacket);
	pkPacket->Pop(kCharacterGuid);
	pkPacket->Pop(strCharacterName);
	pkPacket->Pop(strReceiverName);
	pkPacket->Pop(uiItemId);
	pkPacket->Pop(strItemName);
	pkPacket->Pop(wItemTime);
	kCost.ReadFromPacket(*pkPacket);
	pkPacket->Pop(uiReceiverUserId);
	
	if( uiReceiverUserId <= 0 )
	{
		BM::Stream kPacket( PT_X_N_ANS_CASH );
		dynamic_cast<SERVER_IDENTITY const&>( rkFrom ).WriteToPacket( kPacket );
		kPacket.Push( PT_CN_A_ANS_SENDGIFT );
		kPacket.Push( CSR_INVALID_CHARACTER );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( *pkPacket ); // PgTranPointer
		SendToServer( rkFrom.nSiteNo, kPacket );
		return ;
	}

	unsigned long TranId = GetTranId();
	SGravityCashTranInfo kCashTranInfo( rkFrom, TranId, PT_CN_A_ANS_SENDGIFT, pkPacket );
	m_kContCashTranInfo.insert( std::make_pair( TranId, kCashTranInfo ) );

	char acOrderNum[DEF_ORDERNUM_SIZE+1] = { 0, };
	strncpy( acOrderNum, MB( BM::vstring( TranId ) ) , DEF_ORDERNUM_SIZE );	

	// 패킷타입
	char acPacketType[DEF_PACKET_SIZE+1] = { 0, };
	sprintf( acPacketType, "%04d", EGC_BUY_ITEM_REQ ); // 선물도 아이템 구매와 같다.

	char acReceiverAId[DEF_ACCOUNTNUM_SIZE+1] = { 0, };
	_itoa( uiReceiverUserId, acReceiverAId, 10 );

	SGravityBuyItem kGravityBuyItem;
	_itoa( uiUserId, kGravityBuyItem.acUserId, 10 );
	strncpy( kGravityBuyItem.acOrderNum, MB( BM::vstring( TranId ) ), DEF_ORDERNUM_SIZE );
	strncpy( kGravityBuyItem.acTotalCashAmount, MB( BM::vstring( abs( static_cast<int>(kCost.i64UsedCash) ) ) ), DEF_POINT_SIZE );
	strncpy( kGravityBuyItem.acTotalCouponAmount, MB( BM::vstring( abs( static_cast<int>(kCost.i64UsedMileage) ) ) ), DEF_POINT_SIZE );
	strncpy( kGravityBuyItem.acRecvGiftUserId, acReceiverAId, DEF_ACCOUNTNUM_SIZE );
	strncpy( kGravityBuyItem.acIP, MB( kAddress.IP() ), DEF_IP_SIZE );
	strncpy( kGravityBuyItem.acTotalItemCount, MB( BM::vstring( 1 ) ), DEF_ITEMCOUNT_SIZE ); // 선물 개수 1개 고정
	strncpy( kGravityBuyItem.acItemNo, MB( BM::vstring( uiItemId ) ), DEF_ITEMNO_SIZE );
	strncpy( kGravityBuyItem.acCash, MB( BM::vstring( abs( static_cast<int>( kCost.i64UsedCash ) ) ) ), DEF_POINT_SIZE );

	BM::Stream kPacket;
	kPacket.Push( acPacketType, DEF_PACKET_SIZE );
	kGravityBuyItem.WriteToPacket( kPacket );
	
	if( SendToServer( c_iGravityItemServerNum, kPacket ) )
	{
		BM::Stream kPacket( PT_X_N_ANS_CASH );
		dynamic_cast<SERVER_IDENTITY const&>( rkFrom ).WriteToPacket( kPacket );
		kPacket.Push( PT_CN_A_ANS_SENDGIFT );
		kPacket.Push( CSR_SYSTEM_ERROR );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( static_cast<__int64>(0) );
		kPacket.Push( *pkPacket ); // PgTranPointer
		SendToServer( rkFrom.nSiteNo, kPacket );
	}
}