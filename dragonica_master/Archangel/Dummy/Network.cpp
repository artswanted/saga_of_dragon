#include "stdafx.h"
#include "constant.h"
#include "Global.h"
#include "ai_Action.h"
#include "Network.h"
// #include "PgClientMng.h"
#include "DummyClientMgr.h"
#include "DummyMgr2.h"

bool CClientNetwork::RegistLoginConnector()
{
	CEL::INIT_CORE_DESC kInit;
	kInit.OnSessionOpen	= CClientNetwork::OnConnectFromLogin;
	kInit.OnDisconnect	= CClientNetwork::OnDisconnectFromLogin;
	kInit.OnRecv			= CClientNetwork::OnRecvFromLogin;
	// kInit.dwThreadCount = 2;
	// kInit.bIsImmidiateActivate = true;
	kInit.IdentityValue(CEL::ST_LOGIN);

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kInit);
	return true;
}


bool CClientNetwork::RegistSwitchConnector()
{
	CEL::INIT_CORE_DESC kInit;
	kInit.OnSessionOpen	= CClientNetwork::OnConnectFromSwitch;
	kInit.OnDisconnect	= CClientNetwork::OnDisconnectFromSwitch;
	kInit.OnRecv			= CClientNetwork::OnRecvFromSwitch;
	// kInit.dwThreadCount = 2;
	// kInit.bIsImmidiateActivate = true;
	kInit.IdentityValue(CEL::ST_SWITCH);

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kInit);
	return true;
}

bool CClientNetwork::RegistTimer()
{
	for(BYTE byI = 0; byI < m_kTestInfo.byTimer50ms; byI++)
	{
		CEL::REGIST_TIMER_DESC kInit;
		kInit.pFunc = TimerTick50msec;
		kInit.dwInterval = 50;
		kInit.dwUserData = byI;
		g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit);
	}

	CEL::REGIST_TIMER_DESC kInit;
	kInit.pFunc = TimerTick1m;
	kInit.dwInterval = 60000;
	kInit.dwUserData = 0;
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit);

	return true;
}

void CALLBACK CClientNetwork::OnRegist(const CEL::SRegistResult &rArg)
{
	BM::CAutoMutex kLock(g_kNetwork.m_kMutex);

	if( rArg.iRet != CEL::CRV_SUCCESS)
	{
		assert( NULL );
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Regist Failed");
	}

	if(rArg.eType == CEL::RT_CONNECTOR)
	{
		switch(rArg.iIdentityValue)
		{
		case CEL::ST_LOGIN:
			{
				g_kNetwork.LoginConnector(rArg.guidObj);
			}break;
		case CEL::ST_SWITCH:
			{
				g_kNetwork.SwitchConnector(rArg.guidObj);
			}break;
		}
	}

	if(rArg.eType == CEL::RT_FINAL_SIGNAL)
	{
		if(g_kCoreCenter.SvcStart())
		{
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
			INFO_LOG(BM::LOG_LV6, _T("============ CoreCenter Run ======================"));
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
		}
	}
}

bool CClientNetwork::Init()
{
	BM::CAutoMutex kLock(m_kMutex);

	CEL::INIT_CENTER_DESC CenterInit;

	CenterInit.eOT = BM::OUTPUT_FILE_AND_CONSOLE;
	CenterInit.pOnRegist = CClientNetwork::OnRegist;

	g_kCoreCenter.Init(CenterInit);

	RegistLoginConnector();
	RegistSwitchConnector();
	RegistTimer();

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

//	RegistAcceptor();
	
	return true;
}

/*
bool CClientNetwork::ConnectLoginServer(const CEL::ADDR_INFO &addr)
{
	return g_kCoreCenter.Connect( m_guidLoginConnector, addr, m_guidLoginSession );
}
*/

void CALLBACK CClientNetwork::OnConnectFromLogin(CEL::CSession_Base *pSession)
{
	if (pSession->IsAlive())
	{
		_DETAIL_LOG INFO_LOG( BM::LOG_LV7, __FL__<<L"로그인으로 접속 성공! ["<<C2L(pSession->Addr())<<L"]");
		g_kDummyMgr2.OnConnectFromLogin(pSession->SessionKey(), pSession->IsAlive());
	}
	else
	{
		_DETAIL_LOG INFO_LOG( BM::LOG_LV5, __FL__<<L"Failed to connect Loginserver! ["<<C2L(pSession->Addr())<<L"]");
	}

//	g_ClientMng.ConnectedLogin(pSession, ::timeGetTime());
}

void CALLBACK CClientNetwork::OnDisconnectFromLogin( CEL::CSession_Base *pSession )
{//!  세션락
	_DETAIL_LOG INFO_LOG( BM::LOG_LV5, __FL__<<L"로그인서버로 부터 접속 종료! ["<<C2L(pSession->Addr())<<L"]");
//	g_ClientMng.DisconnectedLogin(pSession);
	g_kDummyMgr2.OnDisconnectFromLogin(pSession);
}

//유저 세션은 pSession->pData 에다가 현재 정보를 셋팅 해 줄것이다. OnDisconnect 에서 delete 해주면 된다.
void CALLBACK CClientNetwork::OnRecvFromLogin( CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
	g_kDummyMgr2.OnRecvFromLogin(pSession, pPacket);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Switch
void CALLBACK CClientNetwork::OnConnectFromSwitch( CEL::CSession_Base *pSession )
{	//!  세션락
	if (pSession->IsAlive())
	{
		_DETAIL_LOG INFO_LOG( BM::LOG_LV7, __FL__<<L"스위치로 접속 성공! ["<<C2L(pSession->Addr())<<L"]");
		g_kDummyMgr2.OnConnectFromSwitch(pSession->SessionKey(), pSession->IsAlive());
	}
	else
	{
		_DETAIL_LOG INFO_LOG( BM::LOG_LV5, __FL__<<L"Failed to connect SwitchServer ["<<C2L(pSession->Addr())<<L"]");
	}
}

void CALLBACK CClientNetwork::OnDisconnectFromSwitch( CEL::CSession_Base *pSession )
{//!  세션락
	_DETAIL_LOG INFO_LOG( BM::LOG_LV5, __FL__<<L"스위치서버로 부터 접속 종료! ["<<C2L(pSession->Addr())<<L"]");
	g_kDummyMgr2.OnDisconnectFromSwitch(pSession);
}

//유저 세션은 pSession->pData 에다가 현재 정보를 셋팅 해 줄것이다. OnDisconnect 에서 delete 해주면 된다.
void CALLBACK CClientNetwork::OnRecvFromSwitch( CEL::CSession_Base *pSession, BM::Stream * const pPacket )
{
	g_kDummyMgr2.OnRecvFromSwitch(pSession, pPacket);
/*
	WORD wType = 0;
	pPacket->Pop(wType);

	std::cout<< "패킷 받음 T:[" << wType <<"] Size["<< pPacket->Size() << "]"<< std::endl;
	switch( wType )
	{
	case PT_S_C_TRY_ACCESS_SWITCH_RESULT:
		{
			ETryLoginResult eRet;
			pPacket->Pop(eRet);
			g_ClientMng.ReqCharacterList(pSession, eRet);
		}
		break;
	case PT_S_C_ANS_CHARACTER_LIST:
		{
			int iSize;
			pPacket->Pop(iSize);
			SPlayerDBSimpleData2 kSimpleData;
			for (int i=0; i<iSize; i++)
			{
				pPacket->Pop(kSimpleData);
			}
			g_ClientMng.AnsCharacterList(pSession, &kSimpleData);
		}
		break;
	case PT_S_C_ANS_SELECT_CHARACTER:
		{
			// Packet Pop 생략
			g_ClientMng.AnsSelectCharacter(pSession);
		}
		break;
	case PT_T_C_ANS_RESERVE_PLAYER:
		{
			SAnsReservePlayer kARP;
			pPacket->Pop(kARP);
			g_ClientMng.AnsReservePlayer(pSession, &kARP);
		}
		break;
	case PT_M_C_NFY_MAPLOADED:
		{
			g_ClientMng.NfyMapLoaded(pSession);
		} break;
	default:
		{
			//pSession->VTerminate();
		}break;
	}
*/
}


bool CClientNetwork::ParseArg(const int argc, _TCHAR* argv[])
{
	if( 1 > argc ){return false;}

	int i = 1;
	while(i != argc)
	{
		const std::wstring wstrArg( argv[i] );
		if( !ParseArg_Sub( wstrArg ) )
		{
			return false;
		}
		++i;
	}
	return true;
}

bool CClientNetwork::ParseArg_Sub( const std::wstring &wstrText )
{
	std::wstring::size_type offset = 0;
	std::wstring::size_type pos = std::wstring::npos;//! ! 

	std::wstring::size_type start= std::wstring::npos;//! ! 시작점
	std::wstring::size_type end= std::wstring::npos;//! ! 끝점.

	pos = wstrText.find(_T('/'), offset );
	if( std::wstring::npos == pos){return 0;}
	offset = pos+1;		start = offset;

	pos = wstrText.find(_T('='), offset );
	if( std::wstring::npos == pos){return 0;}
	offset = pos+1;		end = pos-1;
	
	std::wstring wstrHead = wstrText.substr(start, end);
	std::wstring wstrBody = wstrText.substr(offset);

	if ( _T("CONFIG") == wstrHead )
	{
		// g_kDummyMgr.IniFile(wstrBody);
		return ReadConfigFile(wstrBody.c_str());
	}
	return false;
}

bool CClientNetwork::ReadConfigFile(LPCTSTR lpszFile)
{
	TCHAR chOut[100];

	// SERVER
	::GetPrivateProfileString(_T("SERVER"), _T("IP"), NULL, chOut,  100, lpszFile);
	int iPort = ::GetPrivateProfileInt(_T("SERVER"), _T("PORT"), 0, lpszFile);

	m_kTestInfo.sLoginServer.Set(chOut, iPort);
	
	// LOGIN
	::GetPrivateProfileString(_T("LOGIN"), _T("ID"), NULL, m_kTestInfo.chID, 20, lpszFile);
	m_kTestInfo.sBeginIndex = ::GetPrivateProfileInt(_T("LOGIN"), _T("FROM_IDX"), 0, lpszFile);
	m_kTestInfo.sEndIndex = ::GetPrivateProfileInt(_T("LOGIN"), _T("TO_IDX"), 0, lpszFile);
	::GetPrivateProfileString(_T("LOGIN"), _T("PASSWORD"), NULL, m_kTestInfo.chPassword, 20, lpszFile);
	m_kTestInfo.sRealm = ::GetPrivateProfileInt(_T("LOGIN"), _T("Realm"), 0, lpszFile);
	m_kTestInfo.sChannel = ::GetPrivateProfileInt(_T("LOGIN"), _T("Channel"), 0, lpszFile);

	m_kTestInfo.byTimer50ms = ::GetPrivateProfileInt(_T("SERVER"), _T("TIMER_50MS_THREAD"), 0, lpszFile);

	m_kVersionInfo.Version.i16Major = ::GetPrivateProfileInt(_T("PACK_VERSION"), _T("MAJOR_VERSION"), 0, lpszFile);
	m_kVersionInfo.Version.i16Minor = ::GetPrivateProfileInt(_T("PACK_VERSION"), _T("MINOR_VERSION"), 0, lpszFile);
	m_kVersionInfo.Version.i32Tiny = ::GetPrivateProfileInt(_T("PACK_VERSION"), _T("PATCH_VERSION"), 0, lpszFile);

	return true;
}

bool CClientNetwork::DoConnectLogin(CEL::SESSION_KEY &rkOutSessionGuid)
{
	return g_kCoreCenter.Connect(m_kTestInfo.sLoginServer, rkOutSessionGuid);
}

bool CClientNetwork::DoConnectSwitch(const CEL::ADDR_INFO &rkSwitchAddr, CEL::SESSION_KEY &rkOutSessionGuid)
{
	return g_kCoreCenter.Connect(rkSwitchAddr, rkOutSessionGuid);
}

bool CClientNetwork::SendToSwitch(const CEL::SESSION_KEY &rkSessionKey, const BM::Stream &rkPacket)
{
	const bool ret = g_kCoreCenter.Send(rkSessionKey, rkPacket);
	return ret;
}

bool CClientNetwork::SendToLogin(const CEL::SESSION_KEY &rkSessionKey, const BM::Stream &rkPacket)
{
	const bool ret = g_kCoreCenter.Send(rkSessionKey, rkPacket);
	return ret;
}















int CClientNetwork::m_iTotalSessionCount;

void CClientNetwork::RegistAcceptor()
{
/*	CEL::INIT_CORE_DESC kInit;
	kInit.kBindAddr.Set( _T("127.0.0.1") , 11002 );

	kInit.OnSessionOpen	= CClientNetwork::OnConnectFromX;
	kInit.OnDisconnect	= CClientNetwork::OnDisconnectFromX;
	kInit.OnRecv			= CClientNetwork::OnRecvFromX;
	
	BM::GUID kGuid = g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);
	AddRegist( kGuid );
	m_iTotalSessionCount =0;
*/
}


void CALLBACK CClientNetwork::OnConnectFromX(CEL::CSession_Base *pSession)
{
	++m_iTotalSessionCount;
//	INFO_LOG( BM::LOG_LV5, _T("로그인으로 접속 성공! [%s:%d]"), pSession->Addr().wstrIP.c_str(), pSession->Addr().wPort);
//	Sleep(1000);
//	pSession->VTerminate();

//	g_kDummyMgr.OnConnectFromLogin(pSession->SessionKey(), pSession->IsAlive());

//	g_ClientMng.ConnectedLogin(pSession, ::timeGetTime());
}

void CALLBACK CClientNetwork::OnDisconnectFromX( CEL::CSession_Base *pSession )
{//!  세션락
	--m_iTotalSessionCount;
//	INFO_LOG( BM::LOG_LV5, _T("로그인서버로 부터 접속 종료! [%s:%d]"), pSession->Addr().wstrIP.c_str(), pSession->Addr().wPort);
//	g_ClientMng.DisconnectedLogin(pSession);
//	g_kDummyMgr.OnDisconnectFromLogin(pSession);
}

//유저 세션은 pSession->pData 에다가 현재 정보를 셋팅 해 줄것이다. OnDisconnect 에서 delete 해주면 된다.
void CALLBACK CClientNetwork::OnRecvFromX( CEL::CSession_Base *pSession, BM::Stream * const pPacket)
{
//	g_kDummyMgr.OnRecvFromLogin(pSession, pPacket);
}
