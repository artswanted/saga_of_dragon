#include "stdafx.h"
#include "PacketType.h"
#include "LogGroup.h"
#include "ProcessConfig.h"

std::wstring const CProcessConfig::ms_wstrPatchVersionC = PACKET_VERSION_C;
std::wstring const CProcessConfig::ms_wstrPatchVersionS = PACKET_VERSION_S;

void ClearSession(CEL::SESSION_KEY const& kSessionKey, CProcessConfig::CONT_CONNECTION::mapped_type & rkElement)
{
	if(rkElement.kSessionKey == kSessionKey)
	{
		if(rkElement.kConnectObj != BM::GUID::NullData())
		{//접속 실패시에도 들어오기 때문.
			rkElement.kConnectObj = BM::GUID::NullData();//접속 종료 되었으므로 커넥션 오브젝트 리셋.
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"None Connect Obj");
		}
	}
}

void ClearSession(CEL::SESSION_KEY const& kSessionKey, CProcessConfig::CONT_CONNECTION & kContSession)
{
	for(CProcessConfig::CONT_CONNECTION::iterator switch_itr = kContSession.begin(); switch_itr != kContSession.end(); ++switch_itr)
	{
		CProcessConfig::CONT_CONNECTION::mapped_type &kElement = switch_itr->second;
		ClearSession(kSessionKey, kElement);
	}
}

CProcessConfig::CProcessConfig(void)
{
	LuaDebugPort(0);
	RunMode(E_RunMode_Normal);
}

CProcessConfig::~CProcessConfig(void)
{
}

bool CProcessConfig::Locked_ParseArg(int const argc, TCHAR* argv[])
{
	BM::CAutoMutex kLock(m_kMutex, true);
//!  /SERVER_TYPE=MAP /SERVER_INDEX=12 /CENTER_ADDR=125.131.115.94:3000
//!  서버 인덱스로 맵서버의 경우는 맵 번호및 기타 정보를 센터서버가 알 수 있다.

//! ServerType	CenterServerAddr
//! 머신 컨트롤러가 쎈터로 가면.
//! 쎈터는 해당 머신에 떠야하는 서버 정보를 왕창 보내고(ADDR까지)
//! 쎈터가 CreateProcess 를 실행!.
	if( 1 > argc )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int i = 1;
	while(i != argc)
	{
		std::wstring const wstrArg( argv[i] );
		if( !ParseArg_Sub( wstrArg ) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		++i;
	}

	return CheckArg();
}

void CProcessConfig::Locked_SetConfigDirectory()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	strPath = strPath.substr(0, strPath.rfind(_T('\\')));
	strPath = strPath + L"\\Config\\";
	ConfigDir(strPath);
//	SetCurrentDirectory(strPath.c_str());
}

int CProcessConfig::ParseArg_Sub( std::wstring const &wstrText )
{//protected:
	if(!wstrText.size()) //해석할 Argument의 사이즈가 0이다
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Wrong Argurment (Argument size == 0)");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

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
	
	if( _T("SERVER_NO") == wstrHead )
	{
		m_kServerIdentity.nServerNo = (int)BM::vstring(wstrBody);
	}
	else if( _T("CENTER_ADDR") == wstrHead )
	{
		std::wstring::size_type offset = wstrBody.find(_T(":"));
		std::wstring wstrIP = wstrBody.substr( 0, offset );
		std::wstring wstrPort = wstrBody.substr( offset+1 );

		CenterServerAddr( CEL::ADDR_INFO( wstrIP, (int)BM::vstring(wstrPort) ) );
	}
	else if( _T("MMC_ADDR") == wstrHead )
	{
		std::wstring::size_type offset = wstrBody.find(_T(":"));
		std::wstring wstrIP = wstrBody.substr( 0, offset );
		std::wstring wstrPort = wstrBody.substr( offset+1 );

		MMCServerAddr( CEL::ADDR_INFO( wstrIP, (int)BM::vstring(wstrPort)  ) );
	}
	else if( _T("IMM_ADDR") == wstrHead )
	{
		std::wstring::size_type offset = wstrBody.find(_T(":"));
		std::wstring wstrIP = wstrBody.substr( 0, offset );
		std::wstring wstrPort = wstrBody.substr( offset+1 );

		ImmigrationServerAddr( CEL::ADDR_INFO( wstrIP, (int)BM::vstring(wstrPort)  ) );
	}
	else if( _T("CONTENTS_ADDR") == wstrHead )
	{
		std::wstring::size_type offset = wstrBody.find(_T(":"));
		std::wstring wstrIP = wstrBody.substr( 0, offset );
		std::wstring wstrPort = wstrBody.substr( offset+1 );

		ContentsServerAddr( CEL::ADDR_INFO( wstrIP, (int)BM::vstring(wstrPort)  ) );
	}
	else if ( _T("CHANNEL") == wstrHead )
	{
		m_kServerIdentity.nChannel = (int)BM::vstring(wstrBody);
	}
	else if ( _T("REALM") == wstrHead )
	{
		m_kServerIdentity.nRealm = (int)BM::vstring(wstrBody);
	}
	else if ( _T("LUADEBUG") == wstrHead )
	{
		int iPort = 0;
		swscanf_s(wstrBody.c_str(),L"%d",&iPort);
		LuaDebugPort(iPort);
	}
	else if ( _T("DEBUG") == wstrHead )
	{
		RunMode(RunMode()|E_RunMode_Debug);
	}
	else if ( _T("RUN_MODE") == wstrHead)
	{
		if( wstrBody == _T("INI") )
		{
			RunMode(RunMode()|E_RunMode_ReadIni);
		}
		else if( wstrBody == _T("INB") )
		{
			RunMode(RunMode()|E_RunMode_ReadInb);			
		}
	}
	return 1;
}

bool CProcessConfig::CheckArg()
{//protected:
	if(!ServerNo())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Server No Zero");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	switch(ServerType())
	{
	case CEL::ST_SUB_MACHINE_CONTROL:
	case CEL::ST_MACHINE_CONTROL:
	case CEL::ST_LOGIN:
	case CEL::ST_GMSERVER:
	case CEL::ST_LOG:
	case CEL::ST_AP:
	case CEL::ST_IMMIGRATION:
	case CEL::ST_CONSENT:
		{
			return true;
		}break;
	case CEL::ST_MAP:
	case CEL::ST_SWITCH:
		{
			if(!CenterServerAddr().IsCorrect())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Incorrect CenterServerAddr["<<C2L(CenterServerAddr())<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}//break; 없음
	case CEL::ST_CENTER:
	case CEL::ST_ITEM:
		{
			if(!m_kServerIdentity.nRealm)//렐름번호 없음.
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Incorrect Server RealmNo["<<RealmNo()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case CEL::ST_CONTENTS://채널 번호도 없어야함.
		{
			if(!m_kServerIdentity.nRealm)//렐름번호 없음.
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Incorrect Server RealmNo["<<RealmNo()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			if(m_kServerIdentity.nChannel)
			{//공용이라 채널 번호 있으면 안됨
				VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Incorrect Server ChannelNo["<<RealmNo()<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}break;
	case CEL::ST_NONE:
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__<<L"Incorrect Server Type["<<static_cast<int>(ServerType())<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}break;
	}
	return true;
}

void CProcessConfig::SendIdentity(CEL::CSession_Base *pSession, SERVER_IDENTITY const *pkForceSI)//! 내 정보를 날려주자. 내정보의 셋팅은 ParseArg 나 DB에서 읽어 놔야 한다.
{
	BM::Stream kPacket(PT_A_S_REQ_GREETING);
	if(!pkForceSI)
	{
		pkForceSI = &m_kServerIdentity;
	}

	pkForceSI->WriteToPacket(kPacket);
	kPacket.Push(ms_wstrPatchVersionS);
	pSession->VSend(kPacket);
}

HRESULT CProcessConfig::Locked_Read_ServerList(BM::Stream &rPacket)
{//딱 한번만 받아야함.(재접속시 다시 받을 수 있자나?)
	BM::CAutoMutex kLock(m_kMutex, true);
	{
		if( m_kServerHash.size() )
		{
			CONT_SERVER_HASH kTemp1;
			CONT_MAP_CONFIG kTemp2;
			CONT_MAP_CONFIG kTemp3;
			PU::TLoadTable_MM( rPacket, kTemp1 );
			PU::TLoadKey_A( rPacket, kTemp2 );
			PU::TLoadKey_A( rPacket, kTemp3 );

			INFO_LOG(BM::LOG_LV5, __FL__<<L"Server information received : Yet");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return S_FALSE"));
			return S_FALSE;
		}
		else
		{//처음 받을때만
			PU::TLoadTable_MM( rPacket, m_kServerHash );
			PU::TLoadKey_A( rPacket, m_kContMapCfg_Static );
			PU::TLoadKey_A( rPacket, m_kContMapCfg_Mission );
		}
		
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Server information received : server_num["<<m_kServerHash.size()<<L"]");

		if ( m_kServerHash.empty() )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5,	__FL__<<L"Server information received : but server_num is ZERO");
			CAUTION_LOG(BM::LOG_LV5, __FL__<<L"Server information received : but server_num is ZERO");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	CONT_SERVER_HASH kContServerHash;

	{	
		kContServerHash.clear();
		GetServerInfo(CEL::ST_CENTER, kContServerHash);
		CONT_SERVER_HASH::const_iterator itor = kContServerHash.begin();
		for( ; itor != kContServerHash.end(); ++itor )
		{// Public Center 셋팅
			if (	itor->first.nRealm == RealmNo() 
				&&	IsPublicChannel( itor->first.nChannel ) )
			{
				PublicCenterServerAddr( itor->second.addrServerNat );
				break;
			}
		}
	}
	

	kContServerHash.clear();

	{
		kContServerHash.clear();
		GetServerInfo(CEL::ST_SWITCH, kContServerHash);
		CONT_SERVER_HASH::const_iterator itor = kContServerHash.begin();
		while(itor != kContServerHash.end() )
		{//스위치 서버 접속 정보 셋팅.
			if (	itor->first.nRealm == RealmNo()
				&&	(	IsPublicChannel()	||	itor->first.nChannel == ChannelNo() )
				)
			{// 내 서버가 공유서버(채널0)이면 모든 스위치 서버와 연결
				CONT_SERVER_HASH::mapped_type const &kElement = (*itor).second;

				CONT_CONNECTION::mapped_type kConnectionObj; 
				kConnectionObj.kSessionKey.WorkerGuid(SwitchConnector());

				m_kContSwitchSession.insert(std::make_pair(kElement.addrServerNat, kConnectionObj));
			}
			++itor;
		}
	}

	{
		kContServerHash.clear();
		GetServerInfo(CEL::ST_LOGIN, kContServerHash);

		CONT_SERVER_HASH::iterator Login_Itr = kContServerHash.begin();
		while(Login_Itr != kContServerHash.end())
		{
			CONT_SERVER_HASH::mapped_type const &kElement = Login_Itr->second;
			CONT_CONNECTION::mapped_type kConnectionObj;
			kConnectionObj.kSessionKey.WorkerGuid(LoginConnector());
			m_kContLoginSession.insert(std::make_pair(kElement.addrServerNat, kConnectionObj));
			++Login_Itr;
		}
	}

	SetLogServer();

	return S_OK;
}

bool CProcessConfig::Locked_Write_ServerList( BM::Stream &rPacket, short const nRealm )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( !nRealm )
	{
		PU::TWriteTable_MM(rPacket, m_kServerHash);
		PU::TWriteKey_A(rPacket,m_kContMapCfg_Static);
		PU::TWriteKey_A(rPacket,m_kContMapCfg_Mission);
		return true;
	}

	// 다른 Realm의 정보는 이미그레이션에서 걸러서 보내주자.

	// m_kServerHash
	size_t iSize = 0;
	size_t iWRPos = rPacket.WrPos();
	rPacket.Push(m_kServerHash.size());

	CONT_SERVER_HASH::const_iterator server_itr = m_kServerHash.begin();
	for ( ; server_itr!=m_kServerHash.end(); ++server_itr )
	{
		if (	!(server_itr->first.nRealm) 
			||	server_itr->first.nRealm == nRealm 
			)
		{
			server_itr->first.WriteToPacket( rPacket );
			server_itr->second.WriteToPacket( rPacket );
			++iSize;
		}
	}
	rPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );

	// m_kContMapCfg_Static
	iSize = 0;
	iWRPos = rPacket.WrPos();
	rPacket.Push(m_kContMapCfg_Static.size());

	CONT_MAP_CONFIG::const_iterator mapcfg_itr = m_kContMapCfg_Static.begin();
	for ( ; mapcfg_itr!=m_kContMapCfg_Static.end(); ++mapcfg_itr )
	{
		if (	!(mapcfg_itr->nRealmNo)
			||	mapcfg_itr->nRealmNo == nRealm 
			)
		{
			mapcfg_itr->WriteToPacket( rPacket );
			++iSize;
		}
	}
	rPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );

	// m_kContMapCfg_Mission
	iSize = 0;
	iWRPos = rPacket.WrPos();
	rPacket.Push(m_kContMapCfg_Mission.size());

	mapcfg_itr = m_kContMapCfg_Mission.begin();
	for ( ; mapcfg_itr!=m_kContMapCfg_Mission.end(); ++mapcfg_itr )
	{
		if (	!(mapcfg_itr->nRealmNo)
			||	mapcfg_itr->nRealmNo == nRealm 
			)
		{
			mapcfg_itr->WriteToPacket( rPacket );
			++iSize;
		}
	}
	rPacket.ModifyData( iWRPos, &iSize, sizeof(size_t) );

	return true;
}

bool CProcessConfig::Locked_ConnectLog()
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if(!m_kContLogSession.size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_CONNECTION::iterator Itr = m_kContLogSession.begin();
	while(Itr != m_kContLogSession.end())
	{
		if(m_kContLogSession.size() == 1)
		{
			CONT_CONNECTION::mapped_type &kElement = Itr->second;

			if(kElement.kConnectObj == BM::GUID::NullData())
			{
				g_kCoreCenter.Connect(Itr->first, kElement.kSessionKey, kElement.kConnectObj);
			}
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Incorrect Log Server Count["<<m_kContLogSession.size()<<L"]");
		}
		++Itr;
	}
	return true;
}

bool CProcessConfig::Locked_ConnectImmigration()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if ( ImmigrationServerAddr().IsCorrect() )
	{
		if( m_kImmigrationSession.kConnectObj == BM::GUID::NullData() )
		{
			return g_kCoreCenter.Connect( ImmigrationServerAddr(), m_kImmigrationSession.kSessionKey, m_kImmigrationSession.kConnectObj );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Address["<<C2L(ImmigrationServerAddr())<<L"] is invalid" );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::Locked_ConnectContents()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if ( ContentsServerAddr().IsCorrect() )
	{
		if( m_kContentsSession.kConnectObj == BM::GUID::NullData() )
		{
			return g_kCoreCenter.Connect( ContentsServerAddr(), m_kContentsSession.kSessionKey, m_kContentsSession.kConnectObj );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Address["<<C2L(ContentsServerAddr())<<L"] is invalid" );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::Locked_ConnectCenter()
{
	BM::CAutoMutex kLock(m_kMutex, true);

	if ( CenterServerAddr().IsCorrect() )
	{
		if( m_kCenterSession.kConnectObj == BM::GUID::NullData() )
		{
			return g_kCoreCenter.Connect( CenterServerAddr(), m_kCenterSession.kSessionKey, m_kCenterSession.kConnectObj );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__<<L"Address["<<C2L(CenterServerAddr())<<L"] is invalid" );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::Locked_ConnectPublicCenter()
{
	BM::CAutoMutex kLock(m_kMutex, true);
	if ( !IsPublicChannel() )
	{
		if ( PublicCenterServerAddr().IsCorrect() )
		{
			if( m_kPublicCenterSession.kConnectObj == BM::GUID::NullData() )
			{
				return g_kCoreCenter.Connect( PublicCenterServerAddr(), m_kPublicCenterSession.kSessionKey, m_kPublicCenterSession.kConnectObj );
			}
		}
		else
		{
			INFO_LOG( BM::LOG_LV4, __FL__<<L"Address["<<C2L(PublicCenterServerAddr())<<L"] is invalid" );
		}
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Error Call Public Channel" );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CProcessConfig::Locked_ConnectSwitch()
{//	스위치는 여러개임.. 중복해서 접속 시도 하지 않도록. 
	BM::CAutoMutex kLock(m_kMutex, true);

	if(!m_kContSwitchSession.size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_CONNECTION::iterator switch_itor = m_kContSwitchSession.begin();

	while(switch_itor != m_kContSwitchSession.end())
	{
		CONT_CONNECTION::mapped_type &kElement = (*switch_itor).second;

		if(kElement.kConnectObj == BM::GUID::NullData())
		{
			g_kCoreCenter.Connect((*switch_itor).first, kElement.kSessionKey, kElement.kConnectObj);
		}
		++switch_itor;
	}

	return true;
}

bool CProcessConfig::Locked_ConnectLogin()
{//	로그인은 여러개임.. 중복해서 접속 시도 하지 않도록. 
	BM::CAutoMutex kLock(m_kMutex, true);

	if(!m_kContLoginSession.size())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_CONNECTION::iterator Itr = m_kContLoginSession.begin();

	while(Itr != m_kContLoginSession.end())
	{
		CONT_CONNECTION::mapped_type &kElement = Itr->second;

		if(kElement.kConnectObj == BM::GUID::NullData())
		{
			g_kCoreCenter.Connect(Itr->first, kElement.kSessionKey, kElement.kConnectObj);
		}
		++Itr;
	}

	return true;
}
/*
bool CProcessConfig::Locked_ConnectAllServers()
{
	BM::CAutoMutex kLock(m_kMutex);

	if( !m_kAllServerSession.size() )
	{
		return false;
	}

	CONT_CONNECTION::iterator Itr = m_kAllServerSession.begin();
	while(Itr != m_kAllServerSession.end())
	{
		CONT_CONNECTION::mapped_type &kElement = Itr->second;
		if(kElement.kConnectObj == BM::GUID::NullData())
		{
			g_kCoreCenter.Connect((*Itr).first, kElement.kSessionKey, kElement.kConnectObj);			
		}
		++Itr;		
	}


	return true;	
}
*/
void CProcessConfig::Locked_SetLogServer()
{
	BM::CAutoMutex kLock(m_kMutex, true);
	SetLogServer();
}

void CProcessConfig::SetLogServer()
{//쪼끔 이상한 함수네..??
	//LogServer 접속 정보 셋팅
	CONT_SERVER_HASH kContServerHash;
	GetServerInfo(CEL::ST_LOG, kContServerHash);

	SERVER_IDENTITY const &kMySI = ServerIdentity();

	CONT_SERVER_HASH::iterator Log_Itr = kContServerHash.begin();
	while(Log_Itr != kContServerHash.end())
	{
		CONT_SERVER_HASH::key_type const &kKey = Log_Itr->first;
		CONT_SERVER_HASH::mapped_type const &kElement = Log_Itr->second;
		
		//LogServer는 렐름에만 종속적이다.
		if(kKey.nRealm == kMySI.nRealm)
		{
			if( kKey.nChannel )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Log server Set No Channel");
			}

			if(!kKey.nChannel)//로그서버는 채널이 없어야한다/
			{
				CEL::ADDR_INFO const kAddr(kElement.addrServerBind.IP(), kElement.addrServerBind.wPort);
				CONT_CONNECTION::mapped_type kConnectionObj;
				kConnectionObj.kSessionKey.WorkerGuid(LogConnector());
				m_kContLogSession.insert(std::make_pair(kAddr, kConnectionObj));
			}
		}
		++Log_Itr;
	}

	if(1 < m_kContLogSession.size())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Log server Must Single at Realm");
	}
}


void CProcessConfig::Locked_SetConsoleTitle() const
{
	BM::CAutoMutex kLock(m_kMutex);

	std::wstring kTitle;

	switch( ServerType() )
	{
	case CEL::ST_SUB_MACHINE_CONTROL:
		{
			_GetConsoleTitle( kTitle, ServerType(), RealmNo(), ChannelNo(), ServerNo() );
		}break;
	case CEL::ST_MACHINE_CONTROL:
		{
			_GetConsoleTitle( kTitle, ServerType(), RealmNo(), ChannelNo(), ServerNo() );
		}break;
	case CEL::ST_IMMIGRATION:
		{
			_GetConsoleTitle( kTitle, ServerType(), RealmNo(), ChannelNo(), ServerNo() );
		}break;
	default:
		{
			_GetConsoleTitle( kTitle, ServerType(), RealmNo(), ChannelNo(), ServerNo() );
		}break;
	}

	::SetConsoleTitle( kTitle.c_str() );
}

void CProcessConfig::_GetConsoleTitle( std::wstring &rkTitleOut, int const iServerType, short const nRealmNo, short const nChannelNo, short const nServerNo )const
{
	BM::vstring vstrTitle;

	switch(iServerType)
	{
	case CEL::ST_CENTER:{vstrTitle +=_T("[CENTER] ");}break;
	case CEL::ST_MACHINE_CONTROL:{vstrTitle +=_T("[MMC] ");}break;
	case CEL::ST_SUB_MACHINE_CONTROL:{vstrTitle +=_T("[SMC] ");}break;
	case CEL::ST_LOGIN:{vstrTitle += _T("[LOGIN] ");}	break;
	case CEL::ST_SWITCH:{vstrTitle += _T("[SWITCH] ");}break;
	case CEL::ST_MAP:{vstrTitle += _T("[MAP] ");}break;
	case CEL::ST_LOG:{vstrTitle += _T("[LOG] ");}break;
	case CEL::ST_ITEM:{vstrTitle += _T("[ITEM] ");}break;
	case CEL::ST_CONTENTS:{vstrTitle += _T("[CONTENTS] ");}break;
	case CEL::ST_GMSERVER:{vstrTitle += _T("[GMServer] ");}break;
	case CEL::ST_IMMIGRATION:{vstrTitle += _T("[IMMIGRATION] ");}break;
	case CEL::ST_AP:{vstrTitle += _T("[AP] ");}break;
	case CEL::ST_CONSENT:{vstrTitle += _T("[CONSENT] ");}break;
	}

	if ( nRealmNo )
	{
		vstrTitle += _T(" Realm-");
		vstrTitle += nRealmNo;
	}

	if ( nChannelNo )
	{
		vstrTitle += _T(" Channel-");
		vstrTitle += nChannelNo;
	}
	else if ( iServerType == CEL::ST_CENTER || iServerType == CEL::ST_MAP )
	{
		vstrTitle += _T(" Channel-Public");
	}

	vstrTitle += _T(" ServerNo-");
	vstrTitle += nServerNo;
	
#ifdef _DEBUG
	vstrTitle += _T("  ");
	vstrTitle += ms_wstrPatchVersionS;
#endif

	rkTitleOut = (std::wstring const)vstrTitle;
}

bool CProcessConfig::Locked_AddServerInfo(TBL_SERVERLIST const &rkInfo)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	auto kRet = m_kServerHash.insert(std::make_pair((tagServerIdentity)rkInfo, rkInfo));
	return kRet.second;
}

HRESULT CProcessConfig::Locked_GetServerInfo(short const nServerType, CONT_SERVER_HASH &rkCont)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetServerInfo(nServerType, rkCont);
}

HRESULT CProcessConfig::GetServerInfo(short const nServerType, CONT_SERVER_HASH &rkCont)const
{
	rkCont.clear();

	if(nServerType == CEL::ST_NONE)
	{
		rkCont = m_kServerHash;
		return S_OK;
	}

	CONT_SERVER_HASH::const_iterator itor = m_kServerHash.begin();
	while(itor != m_kServerHash.end())
	{
		if((*itor).second.nServerType == nServerType)
		{
			auto kRet = rkCont.insert(std::make_pair((*itor).first, (*itor).second));
			
			if(!kRet.second)
			{
				rkCont.clear();
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
		++itor;
	}

	if((0 != rkCont.size()))
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::Locked_GetServerInfo(SERVER_IDENTITY const &kSI, TBL_SERVERLIST &rkOut)const
{
	BM::CAutoMutex kLock(m_kMutex);
	return GetServerInfo(kSI, rkOut);
}

HRESULT CProcessConfig::GetServerInfo(SERVER_IDENTITY const &kSI, TBL_SERVERLIST &rkOut)const
{
	CONT_SERVER_HASH::const_iterator itor = m_kServerHash.find(kSI);
	if(itor != m_kServerHash.end())
	{
		rkOut = (*itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::Locked_GetServerIdentity(short const sRealm, short const sChannel, short const nServerType, ContServerID &rkCont) const
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SERVER_HASH::const_iterator itr = m_kServerHash.begin();
	for ( ; itr != m_kServerHash.end() ; ++itr )
	{
		if ( sRealm == itr->first.nRealm )
		{
			if (	(ALL_CHANNEL_NUM == sChannel)
				||	(sChannel == itr->first.nChannel) 
				)
			{
				if ( nServerType == itr->first.nServerType )
				{
					rkCont.push_back( itr->first );
				}
			}
		}
	}

	if( rkCont.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProcessConfig::Locked_GetMapServerCfg(CONT_MAP_CONFIG* pkMapCfgStatic,CONT_MAP_CONFIG* pkMapCfgMission)const
{
	BM::CAutoMutex kLock(m_kMutex);
	if ( pkMapCfgStatic )
	{
		*pkMapCfgStatic = m_kContMapCfg_Static;
	}

	if ( pkMapCfgMission )
	{
		*pkMapCfgMission = m_kContMapCfg_Mission;
	}
	return S_OK;
}

HRESULT CProcessConfig::Locked_AddMapCfg(TBL_MAP_CONFIG const &rkKey)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	auto ret = m_kContMapCfg_Static.insert(rkKey);
	if(ret.second)
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::Locked_AddMapCfg_Mission(TBL_MAP_CONFIG const &rkKey)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	auto ret = m_kContMapCfg_Mission.insert(rkKey);
	if(ret.second)
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

// Getting FileVersion information
// [PARAMETER]
//	lpszFile : filename to get
//	strVersion : [OUT] FileVersion String
// [RETURN]
//	Success ?
BOOL CProcessConfig::GetFileVersionString(LPCTSTR lpszFile, int iFileVersionSize, LPTSTR lpszFileVersion, int iProductVersionSize, LPTSTR lpszProductVersion)const
{
	if (lpszFileVersion == NULL && lpszProductVersion == NULL)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	DWORD dwSize;
	DWORD dwSetZero = 0;
	UINT uSize;

	// Get New File version
	dwSize = GetFileVersionInfoSize(lpszFile, &dwSetZero);
	if (dwSize == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FALSE"));
		return FALSE;
	}
	LPBYTE lpBuffer = new_tr BYTE[dwSize];
	TCHAR chBlock[100];
	//LPTSTR lpszVersion;
	if (!GetFileVersionInfo(lpszFile, 0, dwSize, lpBuffer))
	{
		delete[] lpBuffer;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FALSE"));
		return FALSE;
	}

	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	// Read the list of languages and code pages.
	if (!VerQueryValue(lpBuffer, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &uSize))
	{
		delete[] lpBuffer;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FALSE"));
		return FALSE;
	}
	LPTSTR lpszTemp;
	if (lpszFileVersion != NULL)
	{
		_stprintf_s(chBlock, 100, TEXT("\\StringFileInfo\\%04x%04x\\FileVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
		if (!VerQueryValue(lpBuffer, chBlock, (LPVOID*)&lpszTemp, &uSize))
		{
			delete[] lpBuffer;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FALSE"));
			return FALSE;
		}
		_tcscpy_s(lpszFileVersion, iFileVersionSize, lpszTemp);
	}

	if (lpszProductVersion != NULL)
	{
		_stprintf_s(chBlock, 100, TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"), lpTranslate[0].wLanguage, lpTranslate[0].wCodePage);
		if (!VerQueryValue(lpBuffer, chBlock, (LPVOID*)&lpszTemp, &uSize))
		{
			delete[] lpBuffer;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return FALSE"));
			return FALSE;
		}
		_tcscpy_s(lpszProductVersion, iProductVersionSize, lpszTemp);
	}

	delete[] lpBuffer;
	return TRUE;
}

void CProcessConfig::Locked_GetVersion(int const iFileVersionSize, const LPTSTR lpszFileVersion, int const iProductVersionSize, const LPTSTR lpszProductVersion)const
{
	BM::CAutoMutex kLock(m_kMutex);

	if (lpszFileVersion != NULL)
	{
		lpszFileVersion[0] = _T('\0');
	}
	if (lpszProductVersion != NULL)
	{
		lpszProductVersion[0] = _T('\0');
	}

	TCHAR chFile[MAX_PATH];
	if (::GetModuleFileName(NULL, chFile, MAX_PATH) == 0)
	{
		return;
	}
	GetFileVersionString(chFile, iFileVersionSize, lpszFileVersion, iProductVersionSize, lpszProductVersion);
}

void CProcessConfig::Locked_SetServerType(const CEL::E_SESSION_TYPE eInType)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_kServerIdentity.nServerType = eInType;
}

void CProcessConfig::SetServerNo(short const nServerNo)//서비스중에는 서버 번호 바뀌지 않게해라
{
	m_kServerIdentity.nServerNo = nServerNo;
}

HRESULT CProcessConfig::Locked_OnConnectServer(CEL::CSession_Base * const pkSession)
{	
	BM::CAutoMutex kLock(m_kMutex, true);

	CONT_CONNECTION::iterator switch_itor = m_kContSwitchSession.find(pkSession->Addr());

	if(switch_itor != m_kContSwitchSession.end())
	{
		(*switch_itor).second.kSessionKey = pkSession->SessionKey();
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::Locked_OnDisconnectServer(CEL::CSession_Base * const pkSession, SERVER_IDENTITY *pkOutDisconnectSI)
{//accept 받은 세션의 ADDR의 Port 는 예측 되지 않기 때문에 find를 할 수 없음.
	BM::CAutoMutex kLock(m_kMutex, true);
	CEL::SESSION_KEY const &kSessionKey = pkSession->SessionKey();

	ClearSession(kSessionKey, m_kImmigrationSession);
	ClearSession(kSessionKey, m_kContentsSession);
	ClearSession(kSessionKey, m_kCenterSession);
	ClearSession(kSessionKey, m_kPublicCenterSession);

	if ( CEL::ST_CONSENT == ServerType() )
	{
		// ConsentServer 는 m_kServerHash 가 비어있다.
		CONT_BASE_SESSION* pkOut = NULL;
		if( S_OK == GetSessionCont( CEL::ST_IMMIGRATION, pkOut) )
		{
			CONT_BASE_SESSION::iterator session_itor = pkOut->begin();
			while(session_itor != pkOut->end())
			{
				if((*session_itor).pkSession == pkSession)
				{
					pkOut->erase(session_itor);
					break;
				}
				++session_itor;
			}
		}

		return S_OK;
	}

	//아래는 서버 테이블 기반.
	CONT_SERVER_HASH::iterator itor = m_kServerHash.begin();
	while(itor != m_kServerHash.end())
	{
		TBL_SERVERLIST &kTblServer = (*itor).second;

		if(kTblServer.pkSession == pkSession)
		{
			kTblServer.pkSession = NULL;
			kTblServer.nPlayerCount = 0;

			if ( pkOutDisconnectSI )
			{
				*pkOutDisconnectSI = static_cast<SERVER_IDENTITY>(kTblServer);
			}

			CONT_BASE_SESSION* pkOut = NULL;
			if(S_OK == GetSessionCont((CEL::E_SESSION_TYPE)kTblServer.nServerType, pkOut))
			{
				CONT_BASE_SESSION::iterator session_itor = pkOut->begin();
				while(session_itor != pkOut->end())
				{
					if((*session_itor).pkSession == pkSession)
					{
						pkOut->erase(session_itor);
						break;
					}
					++session_itor;
				}
			}

			INFO_LOG( BM::LOG_LV6, __FL__<<L"ServerSession Clear Realm["<<kTblServer.nRealm<<L"] Channel["<<kTblServer.nChannel<<L"] ServerNo["<<kTblServer.nServerNo<<L"] ServerType["<<kTblServer.nServerType<<L"]" );
		}
		++itor;
	}

	ClearSession(kSessionKey, m_kContSwitchSession);
	ClearSession(kSessionKey, m_kContLoginSession);
	ClearSession(kSessionKey, m_kContLogSession);

/*
	CONT_CONNECTION::iterator server_itr = m_kAllServerSession.begin();
	for ( ; server_itr!=m_kAllServerSession.end(); ++server_itr )
	{
		CONT_CONNECTION::mapped_type &kElement = server_itr->second;
		if(kElement.kSessionKey == kSessionKey)
		{
			if(kElement.kConnectObj != BM::GUID::NullData())
			{//접속 실패시에도 들어오기 때문.
				kElement.kConnectObj = BM::GUID::NullData();//접속 종료 되었으므로 커넥션 오브젝트 리셋.
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, _T("[%s]-[%d] None Connect Obj"), __FUNCTIONW__, __LINE__);
			}
		}
	}
*/
	//서버 세션이 없다는게 말이 안되기 때문에. (물론 Connect 하자마자 ServerIdentity 를 못보내면 그럴 수도 있긴 하다)
//	INFO_LOG( BM::LOG_LV0,		_T("[%s] ServerSession Clear Failed"), __FUNCTIONW__);
//	CAUTION_LOG( BM::LOG_LV0,	_T("[%s] ServerSession Clear Failed"), __FUNCTIONW__);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CProcessConfig::Locked_OnGreetingServer(SERVER_IDENTITY const& rkSI, CEL::CSession_Base * const pkSession)
{
	if(!pkSession)
	{
		INFO_LOG( BM::LOG_LV0,		__FL__<<L"\tWARNING.....Session is NULL");
		CAUTION_LOG( BM::LOG_LV0,	__FL__<<L"\tWARNING.....Session is NULL");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	BM::CAutoMutex kLock(m_kMutex, true);

	CEL::ADDR_INFO const & rkAddr = pkSession->Addr();

	CONT_SERVER_HASH::iterator itor = m_kServerHash.find(rkSI);
	if(itor != m_kServerHash.end())
	{
		TBL_SERVERLIST &kTblServer = (*itor).second;

		if(kTblServer.pkSession)
		{
			INFO_LOG( BM::LOG_LV0,		__FL__<<L"\tWARNING..... Already Connected Server No["<<kTblServer.nServerNo<<L"] Type["<<kTblServer.nServerType<<L"] ADDR["<<C2L(rkAddr)<<L"]");
			CAUTION_LOG( BM::LOG_LV0,	__FL__<<L"\tWARNING..... Already Connected Server No["<<kTblServer.nServerNo<<L"] Type["<<kTblServer.nServerType<<L"] ADDR["<<C2L(rkAddr)<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		kTblServer.pkSession = pkSession;
	}
	else
	{
		if (ServerType() != CEL::ST_CONSENT)
		{
			// ConsentServer 는 m_kServerHash 가 비어있다.
			INFO_LOG( BM::LOG_LV0,		__FL__<<L"\tWARNING..... Incorrect Server Identity No["<<rkSI.nServerNo<<L"] Type["<<rkSI.nServerType<<L"] ADDR["<<C2L(rkAddr)<<L"]");
			CAUTION_LOG( BM::LOG_LV0,	__FL__<<L"\tWARNING..... Incorrect Server Identity No["<<rkSI.nServerNo<<L"] Type["<<rkSI.nServerType<<L"] ADDR["<<C2L(rkAddr)<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	CONT_BASE_SESSION* pkOut = NULL;
	if(S_OK == GetSessionCont((CEL::E_SESSION_TYPE)rkSI.nServerType, pkOut))
	{
		pkOut->push_back(SSession_Base_Info(rkSI, pkSession));
	}
	return S_OK;
}

bool CProcessConfig::IsCorrectVersion( std::wstring const &wstrVersion, bool const bClient )
{
	if ( true == bClient )
	{
		return ms_wstrPatchVersionC == wstrVersion;
	}

	return ms_wstrPatchVersionS == wstrVersion;
}

HRESULT CProcessConfig::Locked_SetPlayerCount(SERVER_IDENTITY const &kSI, size_t const user_count)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	CONT_SERVER_HASH::iterator itor = m_kServerHash.find(kSI);
	if(itor != m_kServerHash.end())
	{
		CONT_SERVER_HASH::mapped_type &element = (*itor).second;
		element.nPlayerCount = user_count;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void CProcessConfig::Locked_DisplayState()const
{//서버 상태나 기타 등등 보여줘라.
	BM::CAutoMutex kLock(m_kMutex);

	S_STATE_LOG(BM::LOG_LV1, _T("============== Session State =============="));

	CEL::SSessionStatistics kStattistics;

	CONT_SERVER_HASH::const_iterator server_itor = m_kServerHash.begin();
	while( m_kServerHash.end() !=  server_itor )
	{
		CONT_SERVER_HASH::mapped_type const &kElement = (*server_itor).second;
		CONT_SERVER_HASH::key_type const &kKey= (*server_itor).first;
		
//		TCHAR const* szAccount = pkElement->szID;
//		TCHAR const* szName = pkElement->szName;
//		int const iLoginNo = pkElement->kLoginServer.nServerNo;
//		SERVER_IDENTITY const &kMapServer = pkElement->kSI;
//		SERVER_IDENTITY const& kSwitchServer = pkElement->kSwitchServer;
//		int const iGroundNo = pkElement->kGroundKey.GroundNo();

		if(kElement.pkSession)
		{
			if(S_OK == g_kCoreCenter.Statistics(kElement.pkSession->SessionKey(), kStattistics))
			{
				S_STATE_LOG(BM::LOG_LV1, L"ServerNo["<<kKey.nServerNo<<L"], Bytes["<<kStattistics.RemainMsgBytes()<<L"], Length["<<kStattistics.RemainMsgLength()<<L"], Count["<<kStattistics.RemainMsgCount()<<L"]");
			}
		}

		++server_itor;
	}

	S_STATE_LOG(BM::LOG_LV1, _T("======================================="));
}

HRESULT CProcessConfig::Recv_PT_A_ENCRYPT_KEY(CEL::CSession_Base *pkSession, BM::Stream * const pkPacket)
{
	int iError = 1;//Pop 실패.
	std::vector<char> kKey;
	if(pkPacket->Pop(kKey))
	{	
		if(S_OK != pkSession->VSetEncodeKey(kKey))
		{	iError = 2;//SetEncode 실패.
			pkSession->VTerminate();
		}
		else
		{
			g_kProcessCfg.SendIdentity(pkSession);//! 내 정보를 날려주자.
			return S_OK;
		}
	}
	
	VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Incoreect EncodeKey Session Terminate Error["<<iError<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

/*
void CProcessConfig::Locked_OnConnectConsentServer(CEL::CSession_Base * const pkSession)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	m_pkConsentSession = pkSession;
}
*/

