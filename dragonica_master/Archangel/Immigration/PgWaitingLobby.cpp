#include "stdafx.h"
#include "BM/Hsel.h"
#include "Lohengrin/PgRealmManager.h"
#include "Lohengrin/PacketStruct_GFAP.h"
#include "Variant/PgEventView.h"
#include "variant/PgStringUtil.h"
#include "Collins/Log.h"
#include "PgWaitingLobby.h"
#include "ImmTask.h"


PgWaitingLobby::PgWaitingLobby(void)
:	m_bUseExtVar(false)
,	m_nHaveExtVar(0)
,	m_bNewTryLoginDisconnectAll(false)
,	m_kMgrGuid(BM::GUID::Create())
,	m_bBlockedLogin(false)
{
}

PgWaitingLobby::~PgWaitingLobby(void)
{
}

void PgWaitingLobby::DisconnectServer(SERVER_IDENTITY const &kRecvSI)
{

	short nInitChannel = 0;

{// 이거 풀면 DeadLock

	BM::CAutoMutex kLock(m_kWaitMutex);
	switch (kRecvSI.nServerType)
	{
	case CEL::ST_CENTER :
		{
			CONT_CENTER_PLAYER_BY_KEY::iterator key_itor = m_kContLoginedUserKey.begin();
			while(key_itor != m_kContLoginedUserKey.end())
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*key_itor).second;

				if(	pData->kSwitchServer.nChannel == kRecvSI.nChannel
				&&	pData->kSwitchServer.nRealm == kRecvSI.nRealm)
				{
					if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
					{
						switch( g_kLocal.ServiceRegion() )
						{
						case LOCAL_MGR::NC_KOREA:
							{
								NCLogOut(pData->gusID);
							}break;
						case LOCAL_MGR::NC_USA:
							{
								GravityLogOut(pData->strID);
							}break;
						default:
							{
							}break;
						}
					}
					
					ClearUserData(pData);
					m_kContLoginedUserKey.erase(key_itor++);
					continue;
				}
				++key_itor;
			}

			nInitChannel = kRecvSI.nChannel;
		}break;
	case CEL::ST_CONTENTS:
		{
			CONT_CENTER_PLAYER_BY_KEY::iterator key_itor = m_kContLoginedUserKey.begin();
			while(key_itor != m_kContLoginedUserKey.end())
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*key_itor).second;

				if(	pData->kSwitchServer.nRealm == kRecvSI.nRealm )
				{
					if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
					{
						switch( g_kLocal.ServiceRegion() )
						{
						case LOCAL_MGR::NC_KOREA:
							{
								NCLogOut(pData->gusID);
							}break;
						case LOCAL_MGR::NC_USA:
							{
								GravityLogOut(pData->strID);
							}break;
						default:
							{
							}break;
						}
					}

					ClearUserData(pData);
					m_kContLoginedUserKey.erase(key_itor++);
				}
				else
				{
					++key_itor;
				}
			}

			nInitChannel = -1;

			// Realm 이 죽었다..
			INFO_LOG( BM::LOG_LV5, _T("* Realm<") << kRecvSI.nRealm << _T("> Dead!!") );

			g_kRealmMgr.AliveChannel(kRecvSI.nRealm, -1, false);
			SERVER_IDENTITY kRealmSI;
			kRealmSI.Set(kRecvSI);
			kRealmSI.nChannel = -1;
			BM::Stream kDPacket(PT_N_A_NFY_CHANNEL_ALIVE);
			kRealmSI.WriteToPacket(kDPacket);
			kDPacket.Push(static_cast<bool>(false));
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_LOGIN, kDPacket);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CONTENTS, kDPacket);
		}break;
	case CEL::ST_SWITCH:
		{
			CONT_CENTER_PLAYER_BY_KEY::iterator key_itor = m_kContLoginedUserKey.begin();
			while(key_itor != m_kContLoginedUserKey.end())
			{
				CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*key_itor).second;

				if(	pData->kSwitchServer == kRecvSI )
				{
					{
						CEL::DB_QUERY kQuery(DT_MEMBER, DQT_CLEAR_CONNECTION_CHANNEL, _T("EXEC [dbo].[up_Clear_User_ConnectionChannel]"));
						kQuery.InsertQueryTarget(pData->guidMember);
						kQuery.QueryOwner(pData->guidMember);
						kQuery.PushStrParam(pData->guidMember);
						g_kCoreCenter.PushQuery(kQuery);
					}

					if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
					{
						switch( g_kLocal.ServiceRegion() )
						{
						case LOCAL_MGR::NC_KOREA:
							{
								NCLogOut(pData->gusID);
							}break;
						case LOCAL_MGR::NC_USA:
							{
								GravityLogOut(pData->strID);
							}break;
						default:
							{
							}break;
						}
					}

					ClearUserData(pData);
					m_kContLoginedUserKey.erase(key_itor++);
					continue;
				}
				++key_itor;
			}
		}break;
	case CEL::ST_LOGIN:
		{
			// Do nothing
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	case CEL::ST_CONSENT:
		{
// 			if ( true == UseJoinSite() )
// 			{
// 				SetBlockedLogin( true );
// 			}
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Consent Server Disconnected...") );
		}break;
	case CEL::ST_GALA:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("GALAServer Disconnected..."));
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Not implemented Disconnected ServerIdentity<") << C2L(kRecvSI) << _T(">") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}break;
	}
}// 이거 풀면 DeadLock

	if ( 0 != nInitChannel )
	{
		InitUserConnectionChannel( kRecvSI.nRealm, nInitChannel );
	}
}

void PgWaitingLobby::SetBlockedLogin( bool const bBlocked )
{
	BM::CAutoMutex kLock( m_kWaitMutex );
	if ( m_bBlockedLogin != bBlocked )
	{
		m_bBlockedLogin = bBlocked;
		if ( true == m_bBlockedLogin )
		{
			INFO_LOG( BM::LOG_LV5, _T("[Login] Block-----!!!") );
		}
		else
		{
			INFO_LOG( BM::LOG_LV6, _T("[Login] UnBlock") );
		}
	}
}

bool PgWaitingLobby::SendTryLoginResult(wchar_t const* szFunc, size_t const iLine, SAuthInfo const& rkAuthInfo, int const eRet, std::wstring const& strErrorMsg)const
{
	BM::E_LOG_LEVEL const eLogLv = ( E_TLR_SUCCESS == eRet ? BM::LOG_LV7 : BM::LOG_LV6 ); 
	INFO_LOG( eLogLv, __FL2__(szFunc, iLine) << _T(" Auth result sending to client ID<") << rkAuthInfo.ID() << _T("> Result<") << eRet << _T("> Msg<") <<strErrorMsg<<L">" );

	BM::Stream kPacket(PT_IM_L_TRY_LOGIN_RESULT);
	rkAuthInfo.WriteToPacket(kPacket);
	kPacket.Push(eRet);
	kPacket.Push(strErrorMsg);
	return SendToServer(rkAuthInfo.kLoginServer, kPacket );
}


HRESULT PgWaitingLobby::ProcessTryAuth( SAuthInfo& kAuthInfo, bool const bIsAutoLogin, std::wstring const &kStrAddonMsg )
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	ETryLoginResult eRet = E_TLR_NONE;
	CONT_WAIT_AUTH::const_iterator auth_check_itor;
	CONT_SWITCH_WAIT::const_iterator switch_check_itor;

	kAuthInfo.guidQuery	= BM::GUID::Create();
	kAuthInfo.ulExpireTime = BM::GetTime32() + WATINGUSER_MAX_DELAYTIME;
	kAuthInfo.MakeCorrect();

	INFO_LOG( BM::LOG_LV7, __FL__ << _T("Begin") );

	TBL_SERVERLIST kServerTable;

	if ( true == IsBlockedLogin() )
	{
		eRet = E_TLR_BLOCKED_SERVER_LOGIN;
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Blocked Server Login!!!") );
		goto __FAILED;
	}
	
	if( S_OK != g_kProcessCfg.Locked_GetServerInfo( kAuthInfo.kLoginServer, kServerTable ) )
	{
		CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Invalid Server No<") << kAuthInfo.kLoginServer.nServerNo << _T(">") );
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Invalid Server No<") << kAuthInfo.kLoginServer.nServerNo << _T(">"));
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		eRet = E_TLR_INCORRECT_LOGIN_SERVER;
		goto __FAILED;
	}

	// 이미그레이션에서는 PacthVersion을 서버 버젼을 검사한다.
	if(!CProcessConfig::IsCorrectVersion( kAuthInfo.szVersion2, false ))
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Incorrect Client Version : UserID<") << kAuthInfo.ID() << _T(">, Client_Version2=") << std::wstring(kAuthInfo.szVersion2)
			<< _T(", ServerVerion") << CProcessConfig::PatchVersionS() );
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		eRet = E_TLR_WRONG_VERSION;
		goto __FAILED;
	}

//	이 단계 에서는 스위치 접속 대기 유저를 알 수가 없다.
//	스위치 접속 대기 단계(TryLoin) 에서 한번더 걸러내도록 한다.
	if( IsConnectUser( kAuthInfo.ID() ) )//계정이 사용중이다.
	{//계정 사용중
		eRet = E_TLR_ALREADY_LOGINED;
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		goto __FAILED;
	}
	
{
	auth_check_itor = m_kContAuthWait.begin();
	while(auth_check_itor != m_kContAuthWait.end())
	{
		std::wstring const &strRegedID = (*auth_check_itor).second.ID();
		if(strRegedID == kAuthInfo.ID())
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Already Waiting Auth ID: UserID<") << kAuthInfo.ID() << _T(">") );
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Already Waiting Auth ID: UserID<") << kAuthInfo.ID() << _T(">") );
			eRet = E_TLR_ALREADY_LOGINED;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;	
		}
		++auth_check_itor;
	}

	switch_check_itor = m_kContSwitchWait.begin();
	while(switch_check_itor != m_kContSwitchWait.end())
	{
		std::wstring const &strRegedID = (*switch_check_itor).second.ID();
		if(strRegedID == kAuthInfo.ID())
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Already Waiting Switch UserID<") << kAuthInfo.ID() << _T(">") );
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("Already Waiting Switch UserID<") << kAuthInfo.ID() << _T(">") );
			eRet = E_TLR_ALREADY_LOGINED;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;	
		}
		++switch_check_itor;
	}
}

	if ( E_FAIL == InsertAuthWait( kAuthInfo.SessionKey(), kAuthInfo ) )
	{
		eRet = E_TLR_ALREADY_TRY;
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		goto __FAILED;
	}

	if(	(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_EU) || g_kLocal.IsServiceRegion(LOCAL_MGR::NC_VIETNAM)
	||	g_kLocal.IsServiceRegion(LOCAL_MGR::NC_RUSSIA)) && !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
	{
		// EUROPE 은 외부 서버에서 인증 처리 한다.
		// ConsentServer 에 Request 보내기
		BM::Stream kWPacket(PT_IM_CN_REQ_LOGIN_AUTH_GALA);
		SAuthRequest kReq(kAuthInfo.ID(), kAuthInfo.PW(), kAuthInfo.guidQuery, kAuthInfo.addrRemote);
		kReq.WriteToPacket(kWPacket);
		if ( !::SendToConsentServer(kWPacket) )
		{
			eRet = E_TLR_EXTERNAL_SERVER_DOWN;
			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			goto __FAILED;
		}
		return E_TLR_NONE;
	}

	if(	g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN)
	&&	!bIsAutoLogin
	&&	!g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
	{
//		일본 처리
		// ConsentServer 에 Request 보내기
		BM::Stream kWPacket(PT_IM_CN_REQ_LOGIN_AUTH_JAPAN);
		SAuthRequest kReq(kAuthInfo.ID(), kAuthInfo.PW(), kAuthInfo.guidQuery, kAuthInfo.addrRemote);
		kReq.WriteToPacket(kWPacket);
		kWPacket.Push(kStrAddonMsg);
		if ( !::SendToConsentServer(kWPacket) )
		{
			eRet = E_TLR_EXTERNAL_SERVER_DOWN;
			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			goto __FAILED;
		}
		return E_TLR_NONE;
	}

	if(	g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA)		
		&&	g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_KOREA_NC_SERVICE) 
		&&	!bIsAutoLogin )
	{// NC 연동
		INFO_LOG(BM::LOG_LV7, __FL__ << L"Begin");
		BM::Stream kWPacket(PT_IM_CN_REQ_LOGIN_AUTH_NC);
		SAuthRequest kReq(kAuthInfo.ID(), kAuthInfo.PW(), kAuthInfo.guidQuery, kAuthInfo.addrRemote);
		kReq.WriteToPacket(kWPacket);
		kWPacket.Push(kStrAddonMsg);//SessionKey

		if( !::SendToConsentServer(kWPacket) )
		{
			eRet = E_TLR_EXTERNAL_SERVER_DOWN;
			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			goto __FAILED;
		}
		INFO_LOG(BM::LOG_LV7, __FL__ << L"End");
		return E_TLR_NONE;
	}

	if( g_kLocal.IsServiceRegion(LOCAL_MGR::NC_USA) 
	&&	!g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP)
	&&	!bIsAutoLogin )
	{// 그라비티 연동
		INFO_LOG( BM::LOG_LV7, __FL__ << "Request to connect with Gravity's linkage server." << " ID: " << kAuthInfo.ID() );
		BM::Stream kPacket( PT_IM_CN_REQ_LOGIN_AUTH_GRAVITY );
		SAuthRequest kReq( kAuthInfo.ID(), kAuthInfo.PW(), kAuthInfo.guidQuery, kAuthInfo.addrRemote );
		kReq.WriteToPacket(kPacket);

		if( !::SendToConsentServer(kPacket) )
		{
			eRet = E_TLR_EXTERNAL_SERVER_DOWN;
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("SendToConsentServer Failed") );
			goto __FAILED;
		}

		return E_TLR_NONE;
	}

	if ( true == kAuthInfo.bNewLogin )
	{
		INFO_LOG(BM::LOG_LV8, __FL__ << ("<03> Login User<") << kAuthInfo.ID() << _T("> Push Waiting List. Req Center DB Query.") );

		CEL::DB_QUERY kQuery;
		if ( true == UseExtVar() )
		{
			kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH, L"EXEC [dbo].[UP_SelectMemberAuthData_ExtVar]");
		}
		else
		{
			kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH, L"EXEC [dbo].[UP_SelectMemberAuthData]" );	
		}

		kQuery.PushStrParam( std::wstring(kAuthInfo.ID()) );
		kQuery.PushStrParam( std::wstring(kAuthInfo.PW()) );
		kQuery.QueryGuid(kAuthInfo.guidQuery);

		if( E_FAIL == g_kCoreCenter.PushQuery(kQuery) )//쿼리 푸쉬
		{//쿼리 못넣었네.
			eRet = E_TLR_DB_ERROR;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __FAILED;
		}
	}
	else
	{
		if ( true == UseJoinSite() )
		{
			BM::Stream kWPacket( PT_IM_CN_REQ_LOGIN_AUTH );
			kAuthInfo.WriteToPacket(kWPacket);
			if ( !::SendToConsentServer(kWPacket) )
			{
				eRet = E_TLR_EXTERNAL_SERVER_DOWN;
				INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
				goto __FAILED;
			}
		}
		else
		{
			SendTryLoginResult(__FUNCTIONW__, __LINE__, kAuthInfo, E_TLR_SUCCESS_AUTH );// 결과전송
		}
	}

	return S_OK;

__FAILED:
	{
		SendTryLoginResult( __FUNCTIONW__, __LINE__, kAuthInfo, eRet );
	}

	if ( E_TLR_ALREADY_LOGINED == eRet )
	{
		if ( true == IsNewTryLoginDisconnectAll() )
		{
			// 이전에 접속한 유저의 접속을 끊어내야 한다.
			// 하지만 여기서는 아직 ID / Password가 맞는지 검사를 하지는 않았다.
			// IP / Password가 틀렸는데도 접속을 끊어내면 안되자나..

			CEL::DB_QUERY kQuery(DT_MEMBER, DQT_DISCONNECT_OLDLOGIN, L"EXEC [dbo].[Up_SelectMemberID]");
			kQuery.QueryOwner(kAuthInfo.guidQuery);
			kQuery.InsertQueryTarget(kAuthInfo.guidQuery);

			kQuery.PushStrParam(kAuthInfo.ID());

			kQuery.contUserData.Push( kAuthInfo.ID() );
			kQuery.contUserData.Push( kAuthInfo.PW() );
			g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	INFO_LOG(BM::LOG_LV7, __FL__ << L"End Err" << eRet << L"New TryDis:"<< IsNewTryLoginDisconnectAll());
	return E_FAIL;//쿼리에 들어갔으면 NONE
}

ETryLoginResult PgWaitingLobby::RecvPT_CN_IM_ANS_LOGIN_AUTH_NC(BM::Stream* const pkPacket)//로그인 시도자.
{	//CFunctionMonitor fm(__FUNCTIONW__, __LINE__);
//이 아래에서는 E_TLR_SUCCESS 를 셋팅해서는 안된다.
	BM::CAutoMutex kLock(m_kWaitMutex);

	INFO_LOG( BM::LOG_LV7, __FL__ << _T("Begin") );
	
	SAuthRequest kAuth;
	ETryLoginResult eResult = E_TLR_NONE;
	BYTE gusID[16] = {0, };

	kAuth.ReadFromPacket(*pkPacket);
	pkPacket->Pop(eResult);	
	pkPacket->PopMemory(gusID, sizeof(gusID));

	CONT_WAIT_AUTH::iterator itor_wait = m_kContAuthWait.begin();
	while (m_kContAuthWait.end() != itor_wait)
	{
		if ((*itor_wait).second.guidQuery == kAuth.kRequestGuid)
		{
			break;
		}
		++itor_wait;
	}
	if (m_kContAuthWait.end() == itor_wait)
	{
		// user 정보가 없다.
		return eResult;
	}
	SAuthInfo & rkAuthInfo = (*itor_wait).second;
	if(eResult != E_TLR_SUCCESS || 0 == gusID[0])
	{
		//실패...
		SendTryLoginResult( __FUNCTIONW__, __LINE__, rkAuthInfo, eResult, kAuth.strErrorMsg );
		return eResult;
	}
	// 정보 업데이트
	rkAuthInfo.byGMLevel = kAuth.byGMLevel;
	rkAuthInfo.iUID = kAuth.iUID;
	rkAuthInfo.ID(kAuth.strID);
	rkAuthInfo.PW(kAuth.strPW);
	rkAuthInfo.bIsPCCafe = kAuth.bIsPCCafe;
	rkAuthInfo.byPCCafeGrade = kAuth.byPCCafeGrade;
	::memcpy( rkAuthInfo.gusID, gusID, sizeof(rkAuthInfo.gusID) );

	/*TBL_SERVERLIST kServerTable;
	if( S_OK != g_kProcessCfg.Locked_GetServerInfo( rSTL.kLoginServer, kServerTable ) )
	{
		CAUTION_LOG(BM::LOG_LV0, __FL__ << _T("Invalid Login-Server No<") << rSTL.kLoginServer.nServerNo << _T(">") );
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Invalid Login-Server No<") << rSTL.kLoginServer.nServerNo << _T(">"));
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return E_TLR_INCORRECT_LOGIN_SERVER;
	}	*/
	
	//try Create 
	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_CREATE_NC_ACCOUNT, L"EXEC [dbo].[up_CreateMemberAccount_NC]" );//NC도 이거 한번 써보자.
	kQuery.InsertQueryTarget(m_kMgrGuid);
	kQuery.QueryOwner(rkAuthInfo.guidQuery);
	kQuery.PushStrParam(rkAuthInfo.iUID);
	kQuery.PushStrParam(kAuth.strID);
	kQuery.PushStrParam(kAuth.strPW);
	kQuery.PushStrParam((BYTE)0);
	kQuery.PushStrParam(rkAuthInfo.byGMLevel);
	rkAuthInfo.WriteToPacket(kQuery.contUserData);
	HRESULT const hQueryRet = g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
	INFO_LOG(BM::LOG_LV7, __FL__ << L"Push Query [up_CreateMemberAccount_NC] Ret:" << hQueryRet);

	return E_TLR_SUCCESS;

}

ETryLoginResult PgWaitingLobby::RecvPT_CN_IM_ANS_LOGIN_AUTH_GRAVITY(BM::Stream* const pkPacket)//로그인 시도자.
{	//CFunctionMonitor fm(__FUNCTIONW__, __LINE__);
//이 아래에서는 E_TLR_SUCCESS 를 셋팅해서는 안된다.
	BM::CAutoMutex kLock(m_kWaitMutex);

	INFO_LOG( BM::LOG_LV7, __FL__ << _T("Begin") );
	
	SAuthRequest kAuth;
	ETryLoginResult eResult = E_TLR_NONE;	

	kAuth.ReadFromPacket(*pkPacket);
	pkPacket->Pop(eResult);

	CONT_WAIT_AUTH::iterator itor_wait = m_kContAuthWait.begin();
	while (m_kContAuthWait.end() != itor_wait)
	{
		if ((*itor_wait).second.guidQuery == kAuth.kRequestGuid)
		{
			break;
		}
		++itor_wait;
	}
	if (m_kContAuthWait.end() == itor_wait)
	{
		// user 정보가 없다.
		return eResult;
	}
	SAuthInfo & rkAuthInfo = (*itor_wait).second;
	if(eResult != E_TLR_SUCCESS )
	{
		//실패...
		SendTryLoginResult( __FUNCTIONW__, __LINE__, rkAuthInfo, eResult, kAuth.strErrorMsg );
		return eResult;
	}
	// 정보 업데이트
	rkAuthInfo.byGMLevel = kAuth.byGMLevel;	
	rkAuthInfo.ID(kAuth.strID);
	rkAuthInfo.PW(kAuth.strPW);
	rkAuthInfo.iUID = kAuth.iUID;
	rkAuthInfo.bIsPCCafe = kAuth.bIsPCCafe;
	rkAuthInfo.byPCCafeGrade = kAuth.byPCCafeGrade;
	
	//try Create 
	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_CREATE_GRAVITY_ACCOUNT, L"EXEC [dbo].[up_CreateMemberAccount_Gravity]" );
	kQuery.InsertQueryTarget(m_kMgrGuid);
	kQuery.QueryOwner(rkAuthInfo.guidQuery);
	kQuery.PushStrParam(rkAuthInfo.iUID);
	kQuery.PushStrParam(kAuth.strID);
	kQuery.PushStrParam(kAuth.strPW);
	kQuery.PushStrParam((BYTE)0);
	kQuery.PushStrParam(rkAuthInfo.byGMLevel);
	rkAuthInfo.WriteToPacket(kQuery.contUserData);
	HRESULT const hQueryRet = g_kCoreCenter.PushQuery(kQuery);//쿼리 푸쉬
	INFO_LOG(BM::LOG_LV7, __FL__ << L"Push Query [up_CreateMemberAccount_Gravity] Ret:" << hQueryRet);

	return E_TLR_SUCCESS;
}

HRESULT PgWaitingLobby::Q_DQT_TRY_AUTH( CEL::DB_RESULT &rkResult)//대기자 숫자 체크, 스위치 로드밸런스 체크.
{	//CFunctionMonitor fm(__FUNCTIONW__, __LINE__);
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_WAIT_AUTH::iterator itr = m_kContAuthWait.begin();//멤버 ID 인증 대기줄
	for ( ; itr != m_kContAuthWait.end() ; ++itr )
	{
		if(itr->second.guidQuery == rkResult.QueryGuid() )
		{
			break;
		}
	}

	if ( itr == m_kContAuthWait.end() )
	{
		CAUTION_LOG(BM::LOG_LV6, __FL__ << _T("Cannot find User in WaitingList QueryGuid = ") << rkResult.QueryGuid() );
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Cannot find User in WaitingList QueryGuid = ") << rkResult.QueryGuid());
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return S_OK;
	}
	
	SAuthInfo &sAuthInfo = itr->second;

	ETryLoginResult eRet = E_TLR_WRONG_ID;
	if (rkResult.vecResultCount.size() > 0 && rkResult.vecResultCount[0] > 0	// Result 받은 것이 있는가 확인해야 한다.
		&& CEL::DR_SUCCESS == rkResult.eRet)
	{
		std::wstring kID;
		BM::DBTIMESTAMP_EX dtBlockStart;
		BM::DBTIMESTAMP_EX dtBlockEnd;
		unsigned short usChannelNo = 0;
		BYTE byPassword[MAX_PW_ENCRYPT_SIZE];

		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor)
		{
			(*itor).Pop(sAuthInfo.kMemberGuid);		++itor;
			(*itor).Pop(kID);						++itor;
			(*itor).Pop(byPassword);				++itor;
			(*itor).Pop(sAuthInfo.ucGender);		++itor;

			(*itor).Pop(sAuthInfo.dtUserBirth);		++itor;
			(*itor).Pop(sAuthInfo.i64TotalConnSec);	++itor;
			(*itor).Pop(sAuthInfo.iAccDisConnSec);	++itor;
			(*itor).Pop(sAuthInfo.iAccConnSec);		++itor;
			(*itor).Pop(sAuthInfo.dtLastLogin);		++itor;

			(*itor).Pop(sAuthInfo.dtLastLogout);	++itor;
			(*itor).Pop(sAuthInfo.dtNowDbTime);		++itor;
			(*itor).Pop(sAuthInfo.byGMLevel);		++itor;
			(*itor).Pop(sAuthInfo.byBlock);			++itor;
			(*itor).Pop(dtBlockStart);				++itor;

			(*itor).Pop(dtBlockEnd);				++itor;
			(*itor).Pop(usChannelNo);				++itor;

			(*itor).Pop(sAuthInfo.iEventRewardKey);	++itor;

			if ( true == UseExtVar() )
			{
				(*itor).Pop(sAuthInfo.nExtVar);		++itor;
			}
			else
			{
				sAuthInfo.nExtVar = 0;
			}

			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_JAPAN:
				{
					PgStringUtil::ConvFullToHalf(kID);
					LWR(kID);
				}break;
			default:
				{
					UPR(kID);
				}break;
			}
			
			__int64 const i64CurTime = g_kEventView.GetLocalSecTime(CGameTime::SECOND);
			__int64 i64LastLogoutSec = 0i64;
			CGameTime::DBTimeEx2SecTime( sAuthInfo.dtLastLogout, i64LastLogoutSec, CGameTime::SECOND );
			if( 0 < i64CurTime - i64LastLogoutSec )
			{//! 누적값이므로, 양수인 경우에만 더해준다. (음수인 경우는 서버 시간이 서로 다를경우)
				sAuthInfo.iAccDisConnSec += std::min(INT_MAX, static_cast<int>(i64CurTime - i64LastLogoutSec));
			}
			if( 0 > sAuthInfo.iAccDisConnSec )
			{
				sAuthInfo.iAccDisConnSec = INT_MAX;
			}

			std::wstring wstrBaseIP;
			std::wstring wstrSubNetMask;
			BYTE byGMLevelOfIP = 0;
			
			eRet = ((0 == sAuthInfo.byGMLevel) ? E_TLR_SUCCESS_AUTH : E_TLR_NOT_GMIP);
			if ( rkResult.vecArray.end() != itor )
			{
				while ( rkResult.vecArray.end() != itor )
				{
					itor->Pop( wstrBaseIP );		++itor;
					itor->Pop( wstrSubNetMask );	++itor;
					itor->Pop( byGMLevelOfIP );		++itor;
						
					if ( true == sAuthInfo.addrRemote.IsCorrectSubNetMask( wstrBaseIP, wstrSubNetMask ) )
					{
						eRet = E_TLR_SUCCESS_AUTH;
						sAuthInfo.byGMLevel &= byGMLevelOfIP;
						break;
					}
				}
			}
		}

		switch ( eRet )
		{
		case E_TLR_NOT_GMIP:
			{
				CAUTION_LOG( BM::LOG_LV5, _T("[HACKING GMACCOUNT] ID<") << kID << _T("> IP<") << sAuthInfo.addrRemote.IP() << _T(">") );
			};	// break 없음 ****** (이유:PW검사후에 GM_IP 에러메시지 출력해 주어야 한다.)
		case E_TLR_SUCCESS_AUTH:
			{
				// 디비 가져오는거 까지 성공, 남은 검증 작업
				if(	kID != sAuthInfo.ID() )
				{
					eRet = E_TLR_WRONG_ID;
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				}
				else
				{
					if ( m_nHaveExtVar != (m_nHaveExtVar & sAuthInfo.nExtVar) )
					{
						eRet = E_TLR_EXTVAR_ERROR;
					}
				}

				if (E_TLR_SUCCESS_AUTH == eRet)
				{
					// 아래 코드는, 해커가 DB의 PW를 변경하더라고, Encoding을 게임서버에서 처리하여, 초기화된 PW로 접속하지 못하도록 하기 위함
					//	SELECT CONVERT(binary(60), HashBytes('MD5', REVERSE(@ID)+@PW))
					BM::vstring vSQL;
					vSQL = vSQL << _T("SELECT CONVERT(binary(60), HashBytes('MD5', REVERSE(UPPER('") << sAuthInfo.ID() << _T("')) + N'") << sAuthInfo.PW() << _T("'))");
					CEL::DB_QUERY kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH_CHECKPW, vSQL.operator std::wstring const&());
					kQuery.QueryGuid(rkResult.QueryGuid());
					kQuery.QueryOwner(sAuthInfo.kMemberGuid);
					kQuery.contUserData.Push(eRet);
					kQuery.contUserData.Push((int)MAX_PW_ENCRYPT_SIZE);
					kQuery.contUserData.Push(byPassword, sizeof(byPassword));
					kQuery.contUserData.Push(dtBlockStart);
					kQuery.contUserData.Push(dtBlockEnd);
					kQuery.contUserData.Push(kID);
					HRESULT const hRet = g_kCoreCenter.PushQuery(kQuery, true);
					INFO_LOG(BM::LOG_LV7, __FL__ << L"Push Query [DQT_TRY_AUTH_CHECKPW] Ret:" << hRet);
					return S_OK;
				}

			}break;
		default:
			{
				// NOTHING TODO
			}break;
		}

	}

	SendTryLoginResult(__FUNCTIONW__, __LINE__, sAuthInfo, eRet);// 결과전송
	return S_OK;
}

HRESULT PgWaitingLobby::Q_DQT_TRY_AUTH_CHECKPW( CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_WAIT_AUTH::iterator itr = m_kContAuthWait.begin();//멤버 ID 인증 대기줄
	for ( ; itr != m_kContAuthWait.end() ; ++itr )
	{
		if(itr->second.guidQuery == rkResult.QueryGuid() )
		{
			break;
		}
	}

	if ( itr == m_kContAuthWait.end() )
	{
		CAUTION_LOG(BM::LOG_LV6, __FL__ << _T("Cannot find User in WaitingList QueryGuid = ") << rkResult.QueryGuid() );
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Cannot find User in WaitingList QueryGuid = ") << rkResult.QueryGuid());
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return S_OK;
	}

	SAuthInfo &sAuthInfo = itr->second;


	ETryLoginResult eRet = E_TLR_LOGIN_ERROR;

	if (CEL::DR_SUCCESS == rkResult.eRet)
	{
		BYTE byPassword1[MAX_PW_ENCRYPT_SIZE];
		BYTE byPassword2[MAX_PW_ENCRYPT_SIZE];
		int iPasswordLen = 0;
		BM::DBTIMESTAMP_EX dtBlockStart;
		BM::DBTIMESTAMP_EX dtBlockEnd;
		std::wstring kID;

		rkResult.contUserData.Pop(eRet);

		if (eRet == E_TLR_SUCCESS_AUTH)
		{
			eRet = E_TLR_LOGIN_ERROR;
			rkResult.contUserData.Pop(iPasswordLen);
			if (MAX_PW_ENCRYPT_SIZE != iPasswordLen)
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__ << _T("Password length mismatch..."));
				SendTryLoginResult(__FUNCTIONW__, __LINE__, sAuthInfo, E_TLR_LOGIN_ERROR);
				return E_FAIL;
			}
			rkResult.contUserData.PopMemory(byPassword1, sizeof(byPassword1));
			rkResult.contUserData.Pop(dtBlockStart);
			rkResult.contUserData.Pop(dtBlockEnd);
			rkResult.contUserData.Pop(kID);

			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor)
			{
				if (true == (*itor).PopMemory(byPassword2, sizeof(byPassword2)))
				{
					if(	!g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP)
					&& (g_kLocal.IsServiceRegion(LOCAL_MGR::NC_JAPAN)) )
					{
						// Consent 서버가 인증해주는 경우는 패스워드 체크하지 않음
						eRet = E_TLR_SUCCESS_AUTH;
					}
					else
					{
						if (0 == memcmp(byPassword1, byPassword2, sizeof(byPassword1)))
						{
							eRet = E_TLR_SUCCESS_AUTH;
						}
						else
						{
							eRet = E_TLR_WRONG_PASS;
						}
					}
				}
			}

			if ( (E_TLR_SUCCESS_AUTH == eRet)
				&& sAuthInfo.byBlock >= MEMBER_BLOCK_MARK_MIN )
			{
				if((dtBlockStart > dtBlockEnd) || (dtBlockEnd > sAuthInfo.dtNowDbTime))
				{
					INFO_LOG( BM::LOG_LV7, __FL__ << _T("<05> Blocked") );
					eRet = E_TLR_BLOCKED_USER;
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				} 
				else 
				{
					// Block 유효기간이 만료
					CEL::DB_QUERY kQuery(DT_MEMBER, DQT_GM_FREEZE_ACCOUNT, _T("EXEC [DBO].[up_UpdateMemberBlock_Id]"));
					kQuery.PushStrParam(0);
					kQuery.PushStrParam(kID);
					kQuery.PushStrParam((BYTE)0);

					dtBlockStart.Clear();
					dtBlockStart.year = 1900;
					dtBlockStart.month = 1;
					dtBlockStart.day = 1;

					kQuery.PushStrParam(dtBlockStart);
					kQuery.PushStrParam(dtBlockStart);
					g_kCoreCenter.PushQuery(kQuery);
				}
			}
		}

		if ( E_TLR_SUCCESS_AUTH == eRet )
		{
			if ( true == UseJoinSite() )
			{
				BM::Stream kWPacket( PT_IM_CN_REQ_LOGIN_AUTH );
				sAuthInfo.WriteToPacket(kWPacket);
				if ( true == ::SendToConsentServer(kWPacket) )
				{
					INFO_LOG(BM::LOG_LV7, __FL__ << L"true == UseJoinSite() SendToConsentServer( Packet(PT_IM_CN_REQ_LOGIN_AUTH) )");
					return S_OK;
				}

				eRet = E_TLR_EXTERNAL_SERVER_DOWN;
				INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			}
		}
	}

	SendTryLoginResult(__FUNCTIONW__, __LINE__, sAuthInfo, eRet);
	return S_OK;
}

void PgWaitingLobby::Q_DQT_DISCONNECT_OLDLOGIN( CEL::DB_RESULT &rkResult )
{
	std::wstring wstrReqID;
	std::wstring wstrReqPW;
	rkResult.contUserData.Pop( wstrReqID );
	rkResult.contUserData.Pop( wstrReqPW );

	bool bDisConnect = false;
	switch ( rkResult.eRet )
	{
	case CEL::DR_NO_RESULT:
	case CEL::DR_SUCCESS:
		{
			int iRet = 0;
			std::wstring kID;
			CEL::DB_BINARY vtPW;

			CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
			if( rkResult.vecArray.end() != itor )
			{	
				itor->Pop( iRet );	++itor;

				if ( rkResult.vecArray.end() != itor )
				{
					itor->Pop( kID );		++itor;
					itor->Pop( vtPW );		++itor;
				}
			}

			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_JAPAN:
				{
					LWR(kID);
				}break;
			default:
				{
					UPR(kID);
				}break;
			}

			if (wstrReqID == kID )
			{
				// 아래 코드는, 해커가 DB의 PW를 변경하더라고, Encoding을 게임서버에서 처리하여, 초기화된 PW로 접속하지 못하도록 하기 위함
				//	SELECT CONVERT(binary(60), HashBytes('MD5', REVERSE(@ID)+@PW))
				BM::vstring vSQL;
				std::wstring strQueryID, strQueryPW;
				CEL::DB_QUERY::StrConvForQuery(wstrReqID, strQueryID);
				CEL::DB_QUERY::StrConvForQuery(wstrReqPW, strQueryPW);
				vSQL = vSQL << _T("SELECT CONVERT(binary(60), HashBytes('MD5', REVERSE(UPPER('") << strQueryID << _T("')) + N'") << strQueryPW << _T("'))");
				CEL::DB_QUERY kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH_CHECKPW_OLDLOGIN, vSQL.operator std::wstring const&());
				kQuery.QueryGuid(rkResult.QueryGuid());
				kQuery.contUserData.Push(vtPW);
				kQuery.contUserData.Push(wstrReqID);
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("Error Ret = ") << rkResult.eRet );
		}break;
	}

	if ( true == bDisConnect )
	{
		// 끊어내야 한다.
		ProcessUserTerminate( wstrReqID, CDC_TryLoginFromOther );
	}
}

ETryLoginResult PgWaitingLobby::ProcessTryLogin( SAuthInfo const &rkAuthInfo, bool const bRecvConsent )	//로그인 시도자.
{//auth와 로그인은 아무 상관 없다.
	// 선택 랠름과 채널 받아서.
	// 그쪽 스위치로 이전 시켜주면. 그 센터가 알아서 플레이어 정보 로딩 해서 써먹어.
	// -> 인원체크는 해야겠고.
	// -> Center 로 스위치 할당요구 -> 
	// -> 센터에서 할당 결과 받음
	// -> 받은 결과 로그인으로 이전.

	// -> 유저는 할당받은 스위치로 접근.
 	INFO_LOG( BM::LOG_LV8, __FL__ << _T("<04> ID/PW Auth DB Result Start") );

	if ( !bRecvConsent )
	{
		if (	(EXT_VAR_MOBILELOCK & rkAuthInfo.nExtVar)
			&&	(true == rkAuthInfo.bNewLogin) )
		{
			BM::Stream kWPacket( PT_IM_CN_REQ_LOGIN_LOGIN);
			rkAuthInfo.WriteToPacket(kWPacket);
			if ( true == ::SendToConsentServer(kWPacket) )
			{
				return E_TLR_NONE;
			}

			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			SendTryLoginResult(__FUNCTIONW__, __LINE__, rkAuthInfo, E_TLR_EXTERNAL_SERVER_DOWN );
			return E_TLR_EXTERNAL_SERVER_DOWN;
		}

		static DWORD const dwServiceRegion = g_kLocal.ServiceRegion();
		if ( true == UseJoinSite() && (dwServiceRegion != LOCAL_MGR::NC_EU) && (dwServiceRegion != LOCAL_MGR::NC_RUSSIA)
			&& (dwServiceRegion != LOCAL_MGR::NC_VIETNAM))
		{
			BM::Stream kWPacket( PT_IM_CN_REQ_LOGIN_LOGIN );
			rkAuthInfo.WriteToPacket(kWPacket);
			if ( true == ::SendToConsentServer(kWPacket) )
			{
				return E_TLR_NONE;
			}

			INFO_LOG( BM::LOG_LV5, __FL__ << _T(" SendToConsentServer Failed") );
			SendTryLoginResult(__FUNCTIONW__, __LINE__, rkAuthInfo, E_TLR_EXTERNAL_SERVER_DOWN );
			return E_TLR_EXTERNAL_SERVER_DOWN;
		}
	}

	BM::CAutoMutex kLock(m_kWaitMutex);
 	//이 쏀터에는 로그인(Auth) 정보가 없을 수가 있어.

	ETryLoginResult eRet = E_TLR_NONE;
	PgChannel kChannel;

	if ( true == IsBlockedLogin() )
	{
		eRet = E_TLR_BLOCKED_SERVER_LOGIN;
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Blocked Server Login!!!") );
		goto __RETURN;
	}

	if(rkAuthInfo.kMemberGuid == BM::GUID::NullData())
	{
		CAUTION_LOG( BM::LOG_LV5, __FL__ << _T(" MemberGuid Is Null UserID = ") << rkAuthInfo.ID() );
		INFO_LOG( BM::LOG_LV5,	__FL__ << _T(" MemberGuid Is Null UserID = ") << rkAuthInfo.ID() );

		eRet = E_TLR_WRONG_ID;
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		goto __RETURN;
	}
	
	if( IsConnectUser(rkAuthInfo.kMemberGuid) )
	{//계정 사용중
		eRet = E_TLR_ALREADY_LOGINED;
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);

		if ( true == IsNewTryLoginDisconnectAll() )
		{
			// 끊어내야 한다.
			SERVER_IDENTITY kSwitchServer;
			if( true == GetConnectUserSwitch( rkAuthInfo.kMemberGuid, kSwitchServer ) )
			{
				::SendDisConnectUser( CDC_TryLoginFromOther, kSwitchServer, rkAuthInfo.kMemberGuid );
			}
		}
		goto __RETURN;
	}

	if(S_OK == g_kRealmMgr.GetChannel(rkAuthInfo.RealmNo(), rkAuthInfo.ChannelNo(), kChannel))
	{
		CONT_SERVER_HASH kContServer;
		if( S_OK == g_kProcessCfg.Locked_GetServerInfo(CEL::ST_CONTENTS, kContServer) )//센터의 IDENTITY 를 받아와야함.
		{
			SReqSwitchReserveMember kRSRM(rkAuthInfo);
			kRSRM.guidOrderKey.Generate();			//스위치에 전달한 해당 유저의 인증키
			kRSRM.ulExpireTime = BM::GetTime32() + WATINGUSER_MAX_DELAYTIME;

			kRSRM.dtUserBirth = rkAuthInfo.dtUserBirth;
			kRSRM.usAge = (short)(rkAuthInfo.dtNowDbTime.year - rkAuthInfo.dtUserBirth.year);
			if( kRSRM.usAge < 0 )
			{
				kRSRM.usAge = 0;
				INFO_LOG( BM::LOG_LV4, __FL__ << _T("User<") << kRSRM.ID() << _T("> BirthDay Year<") <<  rkAuthInfo.dtUserBirth.year << _T("> or Server Time Year<") << rkAuthInfo.dtNowDbTime.year << _T("> is Wrong Value") );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}

			if(g_kLocal.IsServiceRegion(LOCAL_MGR::NC_KOREA)
			&& 	g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_KOREA_NC_SERVICE) )
			{
				INFO_LOG(BM::LOG_LV7, __FL__ << L"Begin");
				++(kRSRM.usAge);
				INFO_LOG(BM::LOG_LV7, __FL__ << L"End");
			}

			CONT_SERVER_HASH::const_iterator server_itor = kContServer.begin();

			while(server_itor != kContServer.end())
			{		
				SERVER_IDENTITY const &kSI = (*server_itor).first;
				if(kSI.nRealm == rkAuthInfo.RealmNo())
				{
					kRSRM.kSwitchServer = kSI;
					auto ret = m_kContSwitchWait.insert( std::make_pair(kRSRM.guidMember, kRSRM) );//! 스위치 웨이팅에 등록
					if( ret.second )
					{//등록된 계정에 한해 해당 쎈터로 요청.
						BM::Stream kPacket(PT_IM_N_REQ_RESERVE_SWITCH_MEMBER);
						kRSRM.WriteToPacket(kPacket);
						if( SendToServer(kSI, kPacket) )
						{
							INFO_LOG( BM::LOG_LV8, __FL__ << _T("insert SwitchMember Success, Req to TargetChannel") );
							LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
							return E_TLR_SUCCESS;
						}
						
						eRet = E_TLR_CHANNEL_NOT_SERVICE_NOW;
						// 채널이 잘못된 경우는 스위치 정보에서 지워줘야 해...
						m_kContSwitchWait.erase( ret.first );
						goto __RETURN;
					}
					break;
				}
				++server_itor;
			}

			
			//벌써 사용중. //지워줘야되나?? 그렇다고?? -->이런상황이 생기지는 않는데.
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("<05-1> Success LOGIN(DB) but Already Waiting MemberGuid<") << rkAuthInfo.kMemberGuid << _T(">") );
			eRet = E_TLR_ALREADY_WAIT_FOR_SWITCH;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __RETURN;		
		}
		else
		{
			eRet = E_TLR_CHANNEL_INFO_INCORRECT;
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			goto __RETURN;
		}	
	}

__RETURN:
{
	SendTryLoginResult(__FUNCTIONW__, __LINE__, rkAuthInfo, eRet );
	return eRet;
}
}

HRESULT PgWaitingLobby::Q_DQT_CREATE_ACCOUNT(CEL::DB_RESULT &rkResult)
{
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("CraeteAccount eRet = ") << rkResult.eRet );

	return S_OK;
}

void PgWaitingLobby::Recv_PT_T_IM_REQ_SWITCH_USER_ADD(SSwitchPlayerData const &kSPD, CEL::CSession_Base *pkSession)
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	m_kContSwitchWait.erase(kSPD.guidMember);//스위치 웨이터는 지워줌.

	HRESULT const hResult = AddLoginedUser(kSPD);
	if (hResult != S_OK)
	{
		CAUTION_LOG( BM::LOG_LV0, _T("[ERROR] Can't Insert Userlist UserID = ") << kSPD.ID() );
		INFO_LOG( BM::LOG_LV0, _T("[ERROR] Can't Insert Userlist UserID = ") << kSPD.ID() );
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}

	BM::Stream kRetPacket(PT_IM_T_ANS_SWITCH_USER_ADD);
	kRetPacket.Push(hResult);
	kSPD.WriteToPacket(kRetPacket);
	pkSession->VSend(kRetPacket);//컨텐츠로 가는것임.
}

bool PgWaitingLobby::Recv_PT_T_IM_ANS_RESERVE_SWITCH_MEMBER_FAILED( BM::GUID const &kMemberGuid, ETryLoginResult const eRet )
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	CONT_SWITCH_WAIT::iterator wait_itr = m_kContSwitchWait.find( kMemberGuid );// 스위치 웨이팅에 있나 찾아서.
	if ( wait_itr != m_kContSwitchWait.end() )
	{
		SAuthInfo kAutoInfo(wait_itr->second);

		if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
		{
			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_KOREA:
				{
					NCLogOut(kAutoInfo.gusID);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					GravityLogOut( kAutoInfo.ID() );
				}break;
			default:
				{
				}break;
			}
		}

		if ( true == UseJoinSite() )
		{
			BM::Stream kWPacket( PT_IM_CN_ANS_LOGIN_LOGIN_FAILED, kAutoInfo.ID() );
			::SendToConsentServer( kWPacket );
		}

		SendTryLoginResult( __FUNCTIONW__, __LINE__, kAutoInfo, eRet );
		m_kContSwitchWait.erase(wait_itr);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

void PgWaitingLobby::ProcessClearAuthWaiting(CEL::SESSION_KEY const &kWaiterKey, BM::GUID const &kMemberGuid, ELoginState eLoginState)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_WAIT_AUTH::iterator Itr =  m_kContAuthWait.find(kWaiterKey);
	if(Itr != m_kContAuthWait.end())
	{
		CONT_WAIT_AUTH::mapped_type& kElement = Itr->second;

		if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP )
		&&	eLoginState == ELogin_None ) // 렐름선택 전에 끊김
		{
			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_KOREA:
				{
					NCLogOut(kElement.gusID);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					GravityLogOut(kElement.ID());
				}break;
			default:
				{
				}break;
			}
		}		

		if ( true == UseJoinSite() )
		{
			BM::Stream kWPacket( PT_IM_CN_NFY_LOGOUT_AUTH, kElement.ID() );
			::SendToConsentServer(kWPacket);
		}
		m_kContAuthWait.erase(Itr);
	}

	m_kContSwitchWait.erase( kMemberGuid );
}

short PgWaitingLobby::ProcessLogout( BM::GUID const &kMemberGuid, bool const bSendContents, bool const bAutoLogin)
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	INFO_LOG(BM::LOG_LV7, __FL__ << _T(" MemberGuid = ") << kMemberGuid );
	CONT_CENTER_PLAYER_BY_KEY::iterator key_itor = m_kContLoginedUserKey.find(kMemberGuid);

	if(key_itor != m_kContLoginedUserKey.end())
	{
		CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = (*key_itor).second;
		short nRealm = pData->kSwitchServer.nRealm;

		if ( bSendContents )
		{
			::SendDisConnectUser( CDC_Normal, pData->kSwitchServer, pData->guidMember );
		}

		if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP )
		&&	!bAutoLogin ) // 채널변경이나 캐릭터 변경시에는 연동서버로 로그아웃 처리 안함.
		{
			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_KOREA:
				{
					NCLogOut(pData->gusID);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					GravityLogOut(pData->strID);
				}break;
			default:
				{
				}break;
			}
		}

		ClearUserData(pData);
		m_kContLoginedUserKey.erase(key_itor);
		return nRealm;
	}	

	INFO_LOG( BM::LOG_LV1, __FL__ << _T(" LogOut Error MemberGuid = ") << kMemberGuid );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return 0;
}

void PgWaitingLobby::ProcessNotifyCash(BM::Stream* const pkPacket)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	std::wstring wstrID;
	unsigned short usPointId = 0;
	__int64 i64PointAmount = 0i64;
	__int64 i64PointDiff = 0i64;

	pkPacket->Pop(wstrID);
	pkPacket->Pop(usPointId);
	pkPacket->Pop(i64PointAmount);
	pkPacket->Pop(i64PointDiff);

	CONT_CENTER_PLAYER_BY_ID::const_iterator id_itr = m_kContLoginedUserID.find( wstrID );
	if( id_itr != m_kContLoginedUserID.end() )
	{	
		BM::Stream kPacket(PT_CN_A_NFY_CASH_NC);
		kPacket.Push(id_itr->second->guidMember);
		kPacket.Push(usPointId);
		kPacket.Push(i64PointAmount);
		kPacket.Push(i64PointDiff);

		::SendToContentsServer(id_itr->second->kSwitchServer.nRealm, kPacket );
	}
	else
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T("Not Found UserID<") << wstrID << _T(">") );
	}
}


bool PgWaitingLobby::ProcessUserTerminate( std::wstring const &wstrID, EClientDisconnectedCause const kCause, bool const bDisConnected, BYTE byReason )
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_ID::const_iterator id_itr = m_kContLoginedUserID.find( wstrID );
	if ( id_itr != m_kContLoginedUserID.end() )
	{
		if ( true == bDisConnected )
		{
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("UserID<") << wstrID << _T("> Cause<") << kCause << _T(">") );
			::SendDisConnectUser( kCause, id_itr->second->kSwitchServer, id_itr->second->guidMember, byReason );
		}
		
		return true;
	}

	CONT_WAIT_AUTH::const_iterator auth_check_itr = m_kContAuthWait.begin();
	for( ; auth_check_itr!=m_kContAuthWait.end(); ++auth_check_itr )
	{
		if( wstrID == auth_check_itr->second.ID() )
		{
			if( 0 == ( EXT_VAR_MOBILELOCK & auth_check_itr->second.nExtVar ) )
			{
				if( true == bDisConnected )
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("AuthWAit Disconnect UserID<") << wstrID << _T(">") << _T("> Cause<") << kCause << _T(">") );

					if( CDC_GM_GSMKick == kCause )
					{//GMS에 의한 Kick은 채널선택화면이라도 끊어낼 수 있어야 한다.
						BM::Stream kPacket(PT_IM_L_NFY_TERMINATE_USER);
						SAuthInfo kAuthinfo(auth_check_itr->second);
						kAuthinfo.WriteToPacket(kPacket);
						kPacket.Push(static_cast<int>(byReason));
						return SendToServer(auth_check_itr->second.kLoginServer, kPacket);
					}
					else
					{
						SendTryLoginResult( __FUNCTIONW__, __LINE__, auth_check_itr->second, E_TLR_OTHERLOGIN );
					}
				}

			}
			return true;
		}
	}

	CONT_SWITCH_WAIT::const_iterator switch_check_itr = m_kContSwitchWait.begin();
	for( ; switch_check_itr != m_kContSwitchWait.end() ; ++switch_check_itr )
	{
		if( wstrID == switch_check_itr->second.ID() )
		{
			if ( true == bDisConnected )
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("SwitchWait Disconnect UserID<") << wstrID << _T(">") );
				::SendDisConnectUser( kCause, switch_check_itr->second.kSwitchServer, switch_check_itr->first, byReason );
			}
			return true;
		}
	}

	INFO_LOG( BM::LOG_LV5, __FL__ << _T("NotFound UserID<") << wstrID << _T("> Cause<") << kCause << _T(">") );
	return false;
}

void PgWaitingLobby::NCLogOut( BYTE* pbyGusId )
{
	BYTE byGusId[16] = {0,};
	::memcpy( byGusId, pbyGusId, sizeof(byGusId) );
	BM::Stream kWPacket(PT_IM_CN_NFY_LOGOUT_NC);
	kWPacket.Push(byGusId, sizeof(byGusId));
	::SendToConsentServer(kWPacket);
}

void PgWaitingLobby::GravityLogOut( const std::wstring& kUserId )
{
	BM::Stream kPacket(PT_IM_CN_NFY_LOGOUT_GRAVITY);
	kPacket.Push( kUserId );
	::SendToConsentServer( kPacket );
}

void PgWaitingLobby::ClearUserData( CONT_CENTER_PLAYER_BY_KEY::mapped_type &pData ) 
{
	if ( true == UseJoinSite() )
	{
		// ConsentServer 에 보내기
		BM::Stream kWPacket(PT_IM_CN_NFY_LOGOUT_LOGIN, pData->strID);
		::SendToConsentServer(kWPacket);
	}

	m_kContLoginedUserID.erase(pData->strID);
	SAFE_DELETE(pData);
}

void PgWaitingLobby::CheckTimeOverUser()
{// 주기적으로 User List를 검사하여 오래된 유저는 삭제하도록 한다.
	BM::CAutoMutex kLock(m_kWaitMutex);

	unsigned long ulNow = BM::GetTime32();

	CONT_WAIT_AUTH::iterator auth_wait_itor = m_kContAuthWait.begin();//멤버 ID 인증 대기줄
	while( m_kContAuthWait.end() != auth_wait_itor )
	{
		if(	ulNow > (*auth_wait_itor).second.ulExpireTime )
		{
			INFO_LOG(BM::LOG_LV6, __FL__ << _T("AuthWaiting user TIMEOUT ID = ") << auth_wait_itor->second.ID() );
			
			if ( true == UseJoinSite() )
			{
				// Time Over라고 통보해야 한다.
				SendTryLoginResult( __FUNCTIONW__, __LINE__, auth_wait_itor->second, E_TLR_TIMEOVER );

				if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
				{
					switch( g_kLocal.ServiceRegion() )
					{
					case LOCAL_MGR::NC_KOREA:
						{
							NCLogOut(auth_wait_itor->second.gusID);
						}break;
					case LOCAL_MGR::NC_USA:
						{
							GravityLogOut(auth_wait_itor->second.ID());
						}break;
					default:
						{
						}break;
					}
				}

				BM::Stream kWPacket( PT_IM_CN_NFY_LOGOUT_AUTH );
				kWPacket.Push( auth_wait_itor->second.ID() );
				::SendToConsentServer(kWPacket);
			}

			m_kContAuthWait.erase(auth_wait_itor++);
			continue;
		}
		++auth_wait_itor;
	}
	//스위치 접근 대기자는 함부로 지우면 안된다.
	//센터의 NFY 가 있을때만 해야한다.

	CONT_SWITCH_WAIT::iterator	switch_wait_itor = m_kContSwitchWait.begin();//스위치 할당을 기다리는 줄
	while( m_kContSwitchWait.end() != switch_wait_itor )
	{
		if(	ulNow > switch_wait_itor->second.ulExpireTime )
		{
			if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
			{
				switch( g_kLocal.ServiceRegion() )
				{
				case LOCAL_MGR::NC_KOREA:
					{
						NCLogOut(switch_wait_itor->second.gusID);
					}break;
				case LOCAL_MGR::NC_USA:
					{
						GravityLogOut( switch_wait_itor->second.ID() );
					}break;
				default:
					{
					}break;
				}
			}

			if ( true == UseJoinSite() )
			{
				BM::Stream kWPacket( PT_IM_CN_ANS_LOGIN_LOGIN_FAILED, switch_wait_itor->second.ID() );
				::SendToConsentServer( kWPacket );
			}

			m_kContSwitchWait.erase(switch_wait_itor++);
			continue;
		}
		++switch_wait_itor;
	}
}

//bool PgWaitingLobby::GetAuthWaitWithQueryGuid(BM::GUID const& rkQueryGuid, SAuthInfo& rkInfo)
//{
//	BM::CAutoMutex kLock(m_kWaitMutex);
//
//	CONT_WAIT_AUTH::iterator	itor = m_kContAuthWait.begin();//멤버 ID 인증 대기줄
//	while (itor != m_kContAuthWait.end())
//	{
//		if(itor->second.guidQuery == rkQueryGuid)
//		{
//			rkInfo = itor->second;
//			return true;
//		}
//		++itor;
//	}
//	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
//	return false;
//}
//
//bool PgWaitingLobby::IsAuthWaitWithQueryGuid( BM::GUID const& rkQueryGuid )const
//{
//	BM::CAutoMutex kLock( m_kWaitMutex );
//	CONT_WAIT_AUTH::const_iterator itr = m_kContAuthWait.begin();
//	for ( ; itr != m_kContAuthWait.end() ; ++itr )
//	{
//		if ( itr->second.guidQuery == rkQueryGuid )
//		{
//			return true;
//		}
//	}
//	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
//	return false;
//}

void PgWaitingLobby::RecvPT_T_IM_NFY_USERLIST( SERVER_IDENTITY const &kRealmSI, VEC_SwitchPlayerData const &kPlayerVector )
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	INFO_LOG( BM::LOG_LV6, _T("* Realm<") << kRealmSI.nRealm << _T("> UserList Sync Start!") );

	size_t iFailedUserCount = 0;
	VEC_SwitchPlayerData::const_iterator itr = kPlayerVector.begin();
	for ( ; itr != kPlayerVector.end() ; ++itr )
	{
		SSwitchPlayerData const &kSPD = *itr;
		if ( S_OK != AddLoginedUser( kSPD, false) )
		{
			INFO_LOG( BM::LOG_LV5, __FL__ << _T("User Cannot Add UserID<") << kSPD.ID() << _T("> DisConnect!!") );

			// 로그인에 실패하면 이미 존재하는 놈과 새로 로그인한 놈 둘다 끊어버려
			short const nRealmNo = ProcessLogout( kSPD.guidMember, true );
			if ( nRealmNo != kSPD.kSwitchServer.nRealm )
			{
				::SendDisConnectUser( CDC_Normal, kSPD.kSwitchServer, kSPD.guidMember ); 
			}

			++iFailedUserCount;
		}
	}

	INFO_LOG( BM::LOG_LV6, _T("* Realm<") << kRealmSI.nRealm << _T("> UserList Sync Result : Total<") << kPlayerVector.size() << _T("> Failed<") << iFailedUserCount << _T(">") );
}

HRESULT PgWaitingLobby::AddLoginedUser(SSwitchPlayerData const &kSPD, bool bRealLogin)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_KEY::mapped_type pData = new SLoginedUser(kSPD);
	if(pData)
	{
		const auto ret = m_kContLoginedUserID.insert( std::make_pair( kSPD.ID(), pData) );
		if( ret.second )//넣기 성공
		{
			const auto ret2 = m_kContLoginedUserKey.insert( std::make_pair( kSPD.guidMember, pData) );
			if( ret2.second )//넣기 성공
			{
				if (bRealLogin)
				{
					if (kSPD.dtLastLogout.IsNull())	
					{
						// 1st Logined member
						CEL::DB_QUERY kQuery(DT_MEMBER, DQT_SAVE_MEMBER_1ST_LOGINED, _T("EXEC [dbo].[up_Save1stLogin]"));
						kQuery.QueryOwner(kSPD.guidMember);
						kQuery.PushStrParam(kSPD.guidMember);
						kQuery.PushStrParam(kSPD.addrRemote.ToString());
						kQuery.PushStrParam(kSPD.dtLastLogin);
						g_kCoreCenter.PushQuery(kQuery);
					}

					{
						CEL::DB_QUERY kQuery(DT_MEMBER, DQT_UPDATE_CONNECTION_CHANNEL, _T("EXEC [dbo].[up_Update_User_Connection_Channel]"));
						kQuery.QueryOwner(kSPD.guidMember);
						kQuery.PushStrParam(kSPD.guidMember);
						kQuery.PushStrParam(kSPD.kSwitchServer.nRealm);
						kQuery.PushStrParam(kSPD.kSwitchServer.nChannel);
						kQuery.PushStrParam(kSPD.iAccDisConnSec);
						kQuery.PushStrParam(kSPD.dtLastLogin);
						g_kCoreCenter.PushQuery(kQuery);
					}
				}
				return S_OK;	// 최종성공
			}
			else
			{
				m_kContLoginedUserID.erase(ret.first);
			}	
		}
	}

	if (pData != NULL)
	{
		// 실패했으므로 pData 삭제
		delete pData;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return E_FAIL;
}

bool PgWaitingLobby::IsConnectUser( BM::GUID const &kMemberID )const
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_KEY::const_iterator Itr =  m_kContLoginedUserKey.find( kMemberID );
	if ( Itr != m_kContLoginedUserKey.end() )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::IsConnectUser( std::wstring const &kID )const
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_ID::const_iterator Itr =  m_kContLoginedUserID.find(kID);
	if( Itr != m_kContLoginedUserID.end() )
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::GetConnectUserSwitch( BM::GUID const &kMemberGUID, SERVER_IDENTITY &kSwitchInfo )const
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	CONT_CENTER_PLAYER_BY_KEY::const_iterator Itr =  m_kContLoginedUserKey.find( kMemberGUID );
	if ( Itr != m_kContLoginedUserKey.end() )
	{
		kSwitchInfo = Itr->second->kSwitchServer;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::GetConnectUserSwitch( std::wstring const &kID, SERVER_IDENTITY &kSwitchInfo, BM::GUID &rkOutMemberGuid )const
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	CONT_CENTER_PLAYER_BY_ID::const_iterator Itr =  m_kContLoginedUserID.find(kID);
	if ( Itr != m_kContLoginedUserID.end() )
	{
		kSwitchInfo = Itr->second->kSwitchServer;
		rkOutMemberGuid = Itr->second->guidMember;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::GetMemberGuid( std::wstring const &kID, BM::GUID &kMemberGuid )const
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	CONT_CENTER_PLAYER_BY_ID::const_iterator Itr =  m_kContLoginedUserID.find(kID);
	if ( Itr != m_kContLoginedUserID.end() )
	{
		kMemberGuid = Itr->second->guidMember;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

/*
bool PgWaitingLobby::FindConnectUser(BM::GUID const &kMemberId, SLoginedUser &rkOut)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_KEY::iterator Itr =  m_kContLoginedUserKey.find(kMemberId);
	if(Itr == m_kContLoginedUserKey.end())
	{
		rkOut.Clear();
		return false;
	}

	CONT_CENTER_PLAYER_BY_KEY::mapped_type kElement = Itr->second;
	rkOut = *kElement;
	
	return true;
}

bool PgWaitingLobby::FindConnectUser(std::wstring const &kId, SLoginedUser &rkOut)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	CONT_CENTER_PLAYER_BY_ID::iterator Itr =  m_kContLoginedUserID.find(kId);
	if(Itr == m_kContLoginedUserID.end())
	{
		rkOut.Clear();
		return false;
	}

	CONT_CENTER_PLAYER_BY_ID::mapped_type kElement = Itr->second;
	rkOut = *kElement;
	
	return true;
}
*/

bool PgWaitingLobby::Q_DQT_UPDATE_MEMBER_LOGOUT(CEL::DB_RESULT &rkResult)
{
	if( CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet )
	{
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		int iErrorCode = 0;
		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iErrorCode);

			if(iErrorCode == 1)	//뭔가 문제 있다
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Failed MemberGuid = ") << rkResult.QueryOwner() );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return false;
			}	
			INFO_LOG( BM::LOG_LV7, __FL__ << _T("Sucess Update User Connection Channel!! MemberGuid = ") << rkResult.QueryOwner() );
		}
		return true;
	}
	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Failed Ret<") << rkResult.eRet << _T("> MemberGuid = ") << rkResult.QueryOwner() );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::Q_DQT_UPDATE_CONNECTION_CHANNEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if(rkResult.vecArray.end() != kIter)
		{
			int iErrorCode = 0;
			kIter->Pop(iErrorCode);

			if(iErrorCode == 1)	//뭔가 문제 있다
			{
				INFO_LOG(BM::LOG_LV5, __FL__ << _T("Failed MemberGuid = ") << rkResult.QueryOwner() );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return false;
			}	
			INFO_LOG( BM::LOG_LV7, __FL__ << _T("Sucess Update User Connection Channel!! MemberGuid = ") << rkResult.QueryOwner() );
		}
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::InitUserConnectionChannel(short RealmNo,  short ChannelNo, bool bDoNow)
{
	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_CLEAR_CONNECTION_CHANNEL, _T("EXEC [DBO].[UP_Clear_User_ConnectionChannel2]"));
	kQuery.PushStrParam(RealmNo);
	kQuery.PushStrParam(ChannelNo);
	return g_kCoreCenter.PushQuery(kQuery, bDoNow);
}

bool PgWaitingLobby::Q_DQT_CLEAR_CONNECTION_CHANNEL(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		int iErrorCode = 0;

		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iErrorCode);

			if(iErrorCode == 1)	//뭔가 문제 있다
			{
				INFO_LOG( BM::LOG_LV5, __FL__ << _T("Failed MemberGuid = ") << rkResult.QueryOwner() );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return false;
			}	
			INFO_LOG( BM::LOG_LV7, _T("Sucess Update User Connection Channel!! CharacterGuid = ") << rkResult.QueryOwner() );
		}
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}

bool PgWaitingLobby::Q_DQT_MEMBER_LOGINED(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{ 
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();

		int iErrorCode = 0;

		if(rkResult.vecArray.end() != kIter)
		{
			kIter->Pop(iErrorCode);

			if(iErrorCode == 2)	//뭔가 문제 있다
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Failed Clear") );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
				return false;
			}	
		}
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	return false;
}


void PgWaitingLobby::DisplayState()
{
	U_STATE_LOG(BM::LOG_LV1, _T("------------ Logined User List -----------"));

	BM::CAutoMutex kLock(m_kWaitMutex);

{
	CONT_WAIT_AUTH::const_iterator auth_wait_itor = m_kContAuthWait.begin();
	while(auth_wait_itor != m_kContAuthWait.end())
	{
		CONT_WAIT_AUTH::mapped_type const &kElement = (*auth_wait_itor).second;

		U_STATE_LOG(BM::LOG_LV0, L"A - ID["<<kElement.ID()<<L"]");
		++auth_wait_itor;
	}
}
{
	CONT_SWITCH_WAIT::const_iterator switch_wait_itor = m_kContSwitchWait.begin();
	while(switch_wait_itor != m_kContSwitchWait.end())
	{
		CONT_SWITCH_WAIT::mapped_type const &kElement = (*switch_wait_itor).second;

		U_STATE_LOG(BM::LOG_LV0, L"S - ID["<<kElement.ID()<<L"]");
		++switch_wait_itor;
	}
}	
{
	CONT_CENTER_PLAYER_BY_ID::const_iterator id_itor = m_kContLoginedUserID.begin();
	while(id_itor != m_kContLoginedUserID.end())
	{
		U_STATE_LOG(BM::LOG_LV0, L"L - ID["<<(*id_itor).first<<L"]");
		++id_itor;
	}
}
	U_STATE_LOG(BM::LOG_LV1, _T("--------------------- Cut Line ---------------------"));
}

void PgWaitingLobby::ProcessPlayTime(BM::GUID const &rkMemberGuid, BM::Stream * const pkPacket)
{
	__int64 i64TotalConnTime = 0;
	int iAccConnTime = 0;
	int iAccDisConnTime = 0;
	pkPacket->Pop( i64TotalConnTime );
	pkPacket->Pop( iAccConnTime );
	if ( pkPacket->Pop( iAccDisConnTime ) )
	{
		CEL::DB_QUERY kQuery( DT_MEMBER, DQT_UPDATE_LOGOUT, _T("EXEC [DBO].[up_UpDateMemberLogOut]"));
		kQuery.InsertQueryTarget(rkMemberGuid);
		kQuery.QueryOwner(rkMemberGuid);
		kQuery.PushStrParam(rkMemberGuid);
		kQuery.PushStrParam(i64TotalConnTime);		
		kQuery.PushStrParam(iAccDisConnTime);
		kQuery.PushStrParam(iAccConnTime);
		g_kCoreCenter.PushQuery(kQuery);
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
	}
}

void PgWaitingLobby::ProcessResetPlayTime()
{
	g_kDefPlayTime.LastApplyTime(BM::PgPackedTime::LocalTime());

	BM::Stream kPacket(PT_I_M_GMCMD_RESETPLAYERPLAYTIME);
	kPacket.Push(g_kDefPlayTime.LastApplyTime());
	SendToServerType(CEL::ST_CONTENTS, kPacket);

	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_UPDATE_RESETPLAYERPLAYTIME, L"EXEC [dbo].[up_UpdateResetPlayerPlayTime]");
	g_kCoreCenter.PushQuery(kQuery);
}

void PgWaitingLobby::ProcessSetPlayTime(std::wstring const & kID, int const iAccConSec, int const iAccDisSec)
{
	//Contents Server에게 Member 검색 & 수정요청
	BM::Stream kPacket(PT_I_M_GMCMD_SETPLAYERPLAYTIME);
	kPacket.Push(kID);
	kPacket.Push(iAccConSec);
	kPacket.Push(iAccDisSec);
	SendToServerType(CEL::ST_CONTENTS, kPacket);

	CEL::DB_QUERY kQuery(DT_MEMBER, DQT_UPDATE_SETPLAYERPLAYTIMEBYID, L"EXEC [dbo].[up_UpdateSetPlayerPlayTimeByID]");
	kQuery.PushStrParam(kID);
	kQuery.PushStrParam(iAccConSec);
	kQuery.PushStrParam(iAccDisSec);
	g_kCoreCenter.PushQuery(kQuery);

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("ProcessSetPlayTime Find: ") << kID << _T("(") << iAccConSec << _T(",") << iAccDisSec << _T(")") );
}


bool PgWaitingLobby::Q_DQT_SAVE_MEMBER_1ST_LOGINED(CEL::DB_RESULT& rkResult)
{
	if(CEL::DR_SUCCESS==rkResult.eRet || CEL::DR_NO_RESULT==rkResult.eRet)
	{
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("DBResult Return =") << rkResult.eRet);
	return false;
}

HRESULT PgWaitingLobby::InsertAuthWait( CONT_WAIT_AUTH::key_type const &kKey, CONT_WAIT_AUTH::mapped_type const &kElement )
{
	auto kPair = m_kContAuthWait.insert( std::make_pair( kKey, kElement ) );
	if ( true == kPair.second )
	{
		return S_OK;
	}

	CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Already Waiting Member UserID : ") << kElement.ID() );
	return E_FAIL;
}

void PgWaitingLobby::Locked_WriteToPacket_LoginUserInfoToConsent( BM::Stream &rkPacket )const
{
	if ( true == UseJoinSite() )
	{
		BM::CAutoMutex kLock( m_kWaitMutex );

		rkPacket.Push( m_kContLoginedUserKey.size() );

		SAuthSyncData kAuthSyncData;
		CONT_CENTER_PLAYER_BY_KEY::const_iterator itr = m_kContLoginedUserKey.begin();
		for ( ; itr!=m_kContLoginedUserKey.end() ; ++itr )
		{
			SLoginedUser const *pkLoginedUser = itr->second;
			kAuthSyncData.strID = pkLoginedUser->strID;
			kAuthSyncData.kRequestGuid = pkLoginedUser->guidMember;

			kAuthSyncData.WriteToPacket( rkPacket );
		}
	}
	else
	{
		rkPacket.Push( static_cast<size_t>(0) );
	}
}

void PgWaitingLobby::RecvPT_CN_IM_ANS_LOGIN_GALA(BM::Stream* const pkPacket)
{
	BM::CAutoMutex kLock(m_kWaitMutex);

	SAuthRequest kAuth;
	kAuth.ReadFromPacket(*pkPacket);
	ETryLoginResult eResult = E_TLR_NONE;
	pkPacket->Pop(eResult);

	CONT_WAIT_AUTH::iterator itor_wait = m_kContAuthWait.begin();
	while (m_kContAuthWait.end() != itor_wait)
	{
		if ((*itor_wait).second.guidQuery == kAuth.kRequestGuid)
		{
			break;
		}
		++itor_wait;
	}
	if (m_kContAuthWait.end() == itor_wait)
	{
		// user 정보가 없다.
		return;
	}
	SAuthInfo & rkAuthInfo = (*itor_wait).second;
	if (eResult != E_TLR_SUCCESS)
	{
		//실패...
		SendTryLoginResult( __FUNCTIONW__, __LINE__, rkAuthInfo, eResult, kAuth.strErrorMsg );
		return;
	}
	// 정보 업데이트
	rkAuthInfo.byGMLevel = kAuth.byGMLevel;
	rkAuthInfo.iUID = kAuth.iUID;

	std::wstring wstrCreateMemberAccountSQL;
	switch (g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_EU:
		{
			wstrCreateMemberAccountSQL = _T("EXEC [dbo].[UP_CreateMemberAccount_GALA]");
		}break;
	case LOCAL_MGR::NC_JAPAN:
		{
			wstrCreateMemberAccountSQL = _T("EXEC [dbo].[UP_CreateMemberAccount_JAPAN]");
			rkAuthInfo.ID(kAuth.strID);
			rkAuthInfo.PW( std::wstring() ); // 이후 처리에서 PW를 저장 하지 않음
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			wstrCreateMemberAccountSQL = _T("EXEC [dbo].[UP_CreateMemberAccount_Nival]");
		}break;
	case LOCAL_MGR::NC_VIETNAM:
		{
			wstrCreateMemberAccountSQL = _T("EXEC [dbo].[UP_CreateMemberAccount_FPT]");
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("not supported in this service region Region=") << g_kLocal.ServiceRegion());
			SendTryLoginResult( __FUNCTIONW__, __LINE__, rkAuthInfo, E_TLR_EXTERNAL_SERVER_ERROR, kAuth.strErrorMsg );
			return;
		}break;
	}

	CEL::DB_QUERY kQuery( DT_MEMBER, DQT_USER_CREATE_ACCOUNT_GALA, wstrCreateMemberAccountSQL);
	kQuery.InsertQueryTarget(m_kMgrGuid);
	kQuery.QueryOwner(rkAuthInfo.guidQuery);
	kQuery.PushStrParam(rkAuthInfo.iUID);		//UID
	kQuery.PushStrParam(kAuth.strID);
	kQuery.PushStrParam(kAuth.strPW);	// PW
	kQuery.PushStrParam(0);		//gender
	kQuery.PushStrParam(rkAuthInfo.byGMLevel);
	rkAuthInfo.WriteToPacket(kQuery.contUserData);
	g_kCoreCenter.PushQuery(kQuery);
}

bool PgWaitingLobby::Q_DQT_USER_CREATE_ACCOUNT_GALA( CEL::DB_RESULT &rkResult )
{
	BM::CAutoMutex kLock(m_kWaitMutex);
	
	// GALA 전용 함수
	int iResult = 99;
	BM::GUID kMemberGuid;
	std::wstring kID;
	BYTE byGender = 0;
	std::wstring kBrithday;
	int iUID = 0;

	CONT_WAIT_AUTH::iterator itor_wait;
	CEL::DB_QUERY kQuery;
	ETryLoginResult eResult = E_TLR_NONE;
	SAuthInfo kAuthInfo;
	kAuthInfo.ReadFromPacket(rkResult.contUserData);

	if (CEL::DR_NO_RESULT != rkResult.eRet 
	&&	CEL::DR_SUCCESS	!= rkResult.eRet)
	{
		eResult = E_TLR_DB_ERROR;
		goto __FAILED;
	}

	if(	CEL::DR_NO_RESULT == rkResult.eRet 
	||	CEL::DR_SUCCESS	== rkResult.eRet)	
	{
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if ( rkResult.vecArray.end() != kIter )
		{
			kIter->Pop(iResult);		++kIter;
			kIter->Pop(kMemberGuid);	++kIter;
			kIter->Pop(kID);			++kIter;
			kIter->Pop(byGender);		++kIter;
			kIter->Pop(kBrithday);		++kIter;
			kIter->Pop(iUID);			++kIter;
		}
	}

	if(iResult == 0)	// iResult == 1 일때는 그냥 정보 update 이다.
	{
		//계정 생성 로그
		PgLogCont kLogCont(ELogMain_System_Create_NewAccount);
		kLogCont.MemberKey(kMemberGuid);
		kLogCont.ID(kID);
		kLogCont.UID(iUID);
		PgLog kLog;
		kLog.Set(0, static_cast<int>(byGender));
		kLog.Set(2, iResult);
		kLog.Set(0, kBrithday);
		kLogCont.Add(kLog);
		kLogCont.Commit();
	}

	itor_wait = m_kContAuthWait.begin();
	while (m_kContAuthWait.end() != itor_wait)
	{
		if ((*itor_wait).second.guidQuery == rkResult.QueryOwner())
		{
			break;
		}
		++itor_wait;
	}
	if (m_kContAuthWait.end() == itor_wait)
	{
		// user 정보가 없다.
		eResult = E_TLR_LOGIN_ERROR;
		goto __FAILED;
	}	

	if(iResult == 1002)
	{
		eResult = E_TLR_INCORRECT_PERMISSION;
		goto __FAILED;
	}

	// DB Login try
	if ( true == UseExtVar() )
	{
		kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH, L"EXEC [dbo].[UP_SelectMemberAuthData_ExtVar]");
	}
	else
	{
		kQuery = CEL::DB_QUERY( DT_MEMBER, DQT_TRY_AUTH, L"EXEC [dbo].[UP_SelectMemberAuthData]" );	
	}
	kQuery.InsertQueryTarget(m_kMgrGuid);
	kQuery.PushStrParam( kAuthInfo.ID() );
	kQuery.PushStrParam( kAuthInfo.PW() );
	kQuery.QueryGuid(kAuthInfo.guidQuery);
	kAuthInfo.WriteToPacket(kQuery.contUserData);
	HRESULT const hRet = g_kCoreCenter.PushQuery(kQuery);
	if(E_FAIL == hRet)//쿼리 푸쉬
	{//쿼리 못넣었네.
		CAUTION_LOG( BM::LOG_LV7, __FL__ << _T(" Push Query failed[UP_SelectMemberAuthData] Ret:") << hRet << L" Ext:" << UseExtVar() );
		eResult = E_TLR_DB_ERROR;
		goto __FAILED;
	}
	return true;

__FAILED:
	{
		CAUTION_LOG( BM::LOG_LV4, __FL__ << _T(" Send Result[") << eResult << _T("] : UserID[") << kAuthInfo.ID() << _T("]") );
		SendTryLoginResult( __FUNCTIONW__, __LINE__, kAuthInfo, eResult );

		if( !g_kLocal.IsAbleServiceType( LOCAL_MGR::ST_DEVELOP ) )
		{
			switch( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_KOREA:
				{
					NCLogOut(kAuthInfo.gusID);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					GravityLogOut(kAuthInfo.ID());
				}break;
			default:
				{
				}break;
			}
		}
	}

	return false;
}

bool PgWaitingLobby::Q_DQT_UPDATE_MEMBER_PW( CEL::DB_RESULT &rkResult )
{
	return true;
}

bool PgWaitingLobby::Q_DQT_TRY_AUTH_CHECKPW_OLDLOGIN(CEL::DB_RESULT &rkResult)
{
	std::wstring kReqID;
	CEL::DB_BINARY vtDBPassword;
	rkResult.contUserData.Pop(vtDBPassword);
	rkResult.contUserData.Pop(kReqID);

	if (CEL::DR_SUCCESS	== rkResult.eRet)
	{
		CEL::DB_BINARY vtReqPW;
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();
		if ( rkResult.vecArray.end() != kIter )
		{
			(*kIter).Pop(vtReqPW);
			if ((vtReqPW.size() == vtDBPassword.size()) && (0 == memcmp(&vtDBPassword.at(0), &vtReqPW.at(0), vtReqPW.size())))
			{
				// 끊어내야 한다.
				ProcessUserTerminate( kReqID, CDC_TryLoginFromOther );
			}
		}
	}
	return true;
}

bool PgWaitingLobby::Q_DQT_LOAD_DEF_PLAYERPLAYTIME(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
		return false;
	}

	if(rkResult.vecResultCount.empty())
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" Empty ResultCount") << _T(", Query=") << rkResult.Command());
		return false;
	}

	CEL::DB_DATA_ARRAY::const_iterator iter = rkResult.vecArray.begin();

	CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();


	int const iDefCount = (*count_iter); ++count_iter;
	
	if(1 != iDefCount)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("Use PlayerPlayTime Count = ") << iDefCount );
		return false;
	}

	SPLAYERPLAYTIMEINFO kInfo;
	BM::DBTIMESTAMP_EX  kResetTime;
	BM::DBTIMESTAMP_EX  kLastTime;
	(*iter).Pop(kInfo.bUse);				++iter;
	if(!kInfo.bUse)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T(" PlayerPlayTime Not Use") );
		return false;
	}
	(*iter).Pop(kInfo.bResetType);			++iter;
	(*iter).Pop(kInfo.iResetValue1);		++iter;	
	(*iter).Pop(kResetTime);				++iter;
	kResetTime.year   = 0;
	kResetTime.month  = 0;
	kResetTime.day    = 0;
	kResetTime.second = 0;
	kInfo.kResetValue2 = static_cast<BM::PgPackedTime>(kResetTime);
	(*iter).Pop(kInfo.sApplyUser);			++iter;
	(*iter).Pop(kInfo.iPlayTime);			++iter;
	(*iter).Pop(kInfo.iFlag);				++iter;
	(*iter).Pop(kLastTime);					++iter;
	kInfo.kLastApply = static_cast<BM::PgPackedTime>(kLastTime);

	INFO_LOG(BM::LOG_LV5, __FL__ << _T("Use PlayerPlayTime ResetType[") << kInfo.bResetType << _T("]"));

	int const iSubCount = (*count_iter); ++count_iter;
	
	for(int i = 0;i < iSubCount;++i)
	{
		SPLAYERPLAYTIMEINFO_SUB kSub;
		
		(*iter).Pop(kSub.bType);		++iter;
		(*iter).Pop(kSub.bTypeSub);		++iter;
		(*iter).Pop(kSub.iValue1);		++iter;
		(*iter).Pop(kSub.iValue2);		++iter;
		(*iter).Pop(kSub.iValue3);		++iter;

		kInfo.kContSub.push_back(kSub);
	}

	//Immigration Server 업데이트
	//컨텐츠 > 센터 > 맵 서버 업데이트

	g_kDefPlayTime.SetDef(kInfo);
	g_kDefPlayTime.Build();
	return true;
}

bool PgWaitingLobby::Q_DQT_UPDATE_RESETPLAYERPLAYTIME(CEL::DB_RESULT &rkResult)
{
	if(	CEL::DR_SUCCESS == rkResult.eRet || CEL::DR_NO_RESULT == rkResult.eRet )
	{
		return true;
	}
	VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("PlayerPlayTime Reset Failed") );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}