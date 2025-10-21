#include "stdafx.h"
#include "BM/TimeCheck.h"
#include "DataPack/Common.h"
#include "Variant/Global.h"
#include "PgMMCTask.h"
#include "PgSiteControl.h"
#include "PgMMCManager.h"
#include "PgRecvFromSMC.h"
#include "PgRecvFromMonTool.h"
#include "PgFileCopyMgr.h"
#include "Variant/PgNoticeAction.h"

int const SITE_NO = 1;

PgMMCManager::PgMMCManager(void)
:	m_dwIntervalTime(5)
,	m_bUseSMCAutoPatch(true)
,	m_bUseSMCDataSync(true)
,	m_bLoadPatchFileList(true)
,	m_iMaxSyncCount(3)
,	m_iKeyValue(0)
{
	m_pkCmdReserveSession = NULL;
	m_kVersion.Version.i32Tiny = -1;
}

PgMMCManager::~PgMMCManager(void)
{
	Terminate();
}

void PgMMCManager::Terminate()
{
	BM::CAutoMutex kLock(m_kMutex);
	CONT_SITE::iterator itr = m_kContSite.begin();
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}

	m_kContPatchFileList.clear();
}

int PgMMCManager::GetSiteNo( CEL::CSession_Base * const pkSession )
{
	if ( pkSession )
	{
		pkSession->m_kSessionData.PosAdjust();

		SERVER_IDENTITY_SITE kSI;
		kSI.ReadFromPacket( pkSession->m_kSessionData );

		pkSession->m_kSessionData.PosAdjust();

		return static_cast<int>(kSI.nSiteNo);
	}
	return 0;
}

bool PgMMCManager::RegistAcceptor(TBL_SERVERLIST const &rkServerInfo)
{
	{
		CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrUserBind;
		CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrUserNat;

		if(rkBindAddr.wPort)
		{
			if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
			{
				return false;//Already In Use
			}

			CEL::INIT_CORE_DESC kInit;
			kInit.kBindAddr = rkBindAddr;
			kInit.kNATAddr = rkNATAddr;

			kInit.OnSessionOpen		= OnAcceptFromMonTool;
			kInit.OnDisconnect		= OnDisConnectToMonTool;
			kInit.OnRecv			= OnRecvFromMonTool;
			kInit.bIsImmidiateActivate	= true;
			kInit.IdentityValue(CEL::ST_NONE);
			kInit.ServiceHandlerType(CEL::SHT_SERVER);

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("Try Regist Acceptor [") << rkBindAddr.ToString() << _T("]") );

			g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);
		}
		else
		{
			ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("Incorrect Acceptor Addr [") << rkBindAddr.ToString() << _T("]") );
		}
	}
	{
		CEL::ADDR_INFO const &rkBindAddr= rkServerInfo.addrServerBind;
		CEL::ADDR_INFO const &rkNATAddr = rkServerInfo.addrServerNat;

		if(rkBindAddr.wPort)
		{
			if(g_kCoreCenter.IsExistAcceptor(rkBindAddr))
			{
				return false;//Already In Use
			}

			CEL::INIT_CORE_DESC kInit;
			kInit.kBindAddr = rkBindAddr;
			kInit.kNATAddr = rkNATAddr;

			//SMS Acceptor
			kInit.OnSessionOpen		= OnAcceptFromSMC;
			kInit.OnDisconnect		= OnDisConnectToSMC;
			kInit.OnRecv			= OnRecvFromSMC;
			kInit.bIsImmidiateActivate	= true;
			kInit.IdentityValue(CEL::ST_SUB_MACHINE_CONTROL);
			kInit.ServiceHandlerType(CEL::SHT_FILE_SERVER);//MMC는 압축 때문에 

			INFO_LOG(BM::LOG_LV6, __FL__ << _T("Try Regist Acceptor [") << rkBindAddr.ToString() << _T("]") );
			g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kInit);
			return true;
		}
		else
		{
			ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("Incorrect Acceptor Addr [") << rkBindAddr.ToString() << _T("]") );
		}
	}
	return false;
}

bool PgMMCManager::Locked_ReadFromConfig( LPCWSTR lpFileName)
{
	if ( TRUE == ::PathFileExists(lpFileName) )
	{
		BM::CAutoMutex kLock(m_kMutex);
		return true;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << lpFileName << L" isn't exist" );
	return false;
}

bool PgMMCManager::Locked_ReadFromConfig( LPCWSTR lpFileName, bool const bOnlyAuth )
{
	if ( TRUE == ::PathFileExists(lpFileName) )
	{
		BM::CAutoMutex kLock(m_kMutex);

		TCHAR chValue[MAX_PATH] = {0,};

		BM::Stream kStopPacket;
		kStopPacket.SetStopSignal(true);
		BroadCast_MonTool( kStopPacket );

		if ( !bOnlyAuth )
		{
			m_kSyncPath.clear();

			DWORD const dwInterval = __max( static_cast<DWORD>(::GetPrivateProfileInt( _T("MMC"), _T("INTERVAL_TIME "), 0, lpFileName )), 5 );
			m_iMaxSyncCount = std::max( static_cast<size_t>(::GetPrivateProfileInt( _T("MMC"), _T("SYNC_MACHINE_NUM"), 0, lpFileName )), static_cast<size_t>(3) );
			IntervalTime( dwInterval * 1000 );
			UseSMCAutoPatch( 0 < ::GetPrivateProfileInt( _T("MMC"), _T("SMC_AUTO_PATCH "), 0, lpFileName ) );
			UseSMCDataSync( 0 < ::GetPrivateProfileInt( _T("MMC"), _T("SMC_DATA_SYNC "), 1, lpFileName ) );
			if( ::GetPrivateProfileString( _T("MMC"), _T("FORCE_DATA_PATH"), NULL, chValue, MAX_PATH, lpFileName) > 0 )
			{
				m_kForceDataPath = chValue;
			}

			ReadIni_Path(CEL::ST_CENTER, _T("CENTER"), lpFileName);
			ReadIni_Path(CEL::ST_CONTENTS, _T("CONTENTS"), lpFileName);
			ReadIni_Path(CEL::ST_LOGIN, _T("LOGIN"), lpFileName);
			ReadIni_Path(CEL::ST_MAP, _T("MAP"), lpFileName);
			ReadIni_Path(CEL::ST_SWITCH, _T("SWITCH"), lpFileName);
			ReadIni_Path(CEL::ST_IMMIGRATION, _T("IMMIGRATION"), lpFileName);
			ReadIni_Path(CEL::ST_LOG, _T("LOG"), lpFileName);
			ReadIni_Path(CEL::ST_GMSERVER, _T("GM"), lpFileName);
			ReadIni_Path(CEL::ST_NONE, _T("CONFIG"), lpFileName);
			ReadIni_Path(CEL::ST_SUB_MACHINE_CONTROL, _T("SMC"), lpFileName);
			ReadIni_Path(CEL::ST_CONSENT, _T("CONSENT"), lpFileName);

			BM::Stream kReloadPacket(PT_MMC_SMC_NFY_CMD, MCT_REFRESH_CFG );
			PU::TWriteTable_MM( kReloadPacket, m_kSyncPath );
			BroadCast_SMC( kReloadPacket );
		}
		
		// User 인증 정보
		if( ::GetPrivateProfileString( _T("USER_INFO"), _T("MAX_USER"), NULL, chValue, MAX_PATH, lpFileName ) > 0 )
		{
			m_kUserAuthInfo.clear();

			int const iUserCount = _ttoi(chValue);
			for(int i = 0; i < iUserCount; ++i )
			{
				TCHAR chUserInfo[50] = {0,};
				::wsprintf( chUserInfo, _T("USER_INFO_%d"), i+1 );

				STerminalAuthInfo kAuthInfo;
				if( ::GetPrivateProfileString( chUserInfo, L"ID", NULL, chValue, MAX_PATH, lpFileName) > 0 )
				{
					kAuthInfo.kID = chValue;
				}

				if( ::GetPrivateProfileString( chUserInfo, L"PW", NULL, chValue, MAX_PATH, lpFileName) > 0 )
				{
					kAuthInfo.kPW = chValue;
				}

				if( ::GetPrivateProfileString( chUserInfo, L"IP", NULL, chValue, MAX_PATH, lpFileName) > 0 )
				{
					kAuthInfo.kAuthIP = chValue;
				}

				kAuthInfo.cGrant = static_cast<BYTE>(::GetPrivateProfileInt(chUserInfo, _T("GRADE"), 0, lpFileName) );

				auto kPair = m_kUserAuthInfo.insert( std::make_pair(kAuthInfo.kID, SMonToolInfo(kAuthInfo)) );
			}

			return true;
		}

		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Not Found [USER_INFO] in ") << lpFileName );
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << lpFileName << L" isn't exist" );
	return false;
}

bool PgMMCManager::Locked_LoadDB( int const iDBIndex )
{
	//! DB에 최신 패치 버전을 물어본다.
	CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_PATCH_VERSION_EDIT, L"EXEC [dbo].[up_GetLastPatchVersion2]");
	g_kCoreCenter.PushQuery(kQuery);

	PgSiteControl *pkControl = new PgSiteControl( iDBIndex );
	if ( !pkControl )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Memory Error!!" );
		return false;
	}

	{
		BM::CAutoMutex kLock(m_kMutex);
		auto kPair = m_kContSite.insert( std::make_pair( iDBIndex, pkControl ) );
		if ( !kPair.second )
		{
			SAFE_DELETE( pkControl );
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Overlapping ID = " << iDBIndex );
			return false;
		}
	}

	{
		TCHAR szAppName[MAX_PATH] = {0,};
		_stprintf_s( szAppName, MAX_PATH, _T("R0C0_DB_INFO_%d"), iDBIndex );

		TCHAR szTemp[MAX_PATH] = _T("TB_SiteConfig");
		::GetPrivateProfileString( _T("ETC"), _T("SERVER_TBNAME"), szTemp, szTemp, 100, _T("./DB_Config.ini") );

		CEL::DB_QUERY kQuery( iDBIndex, DQT_LOAD_SITE_CONFIG, _T("EXEC [dbo].[UP_LoadSiteConfig]"));
		kQuery.PushStrParam( std::wstring(szTemp) );
		g_kCoreCenter.PushQuery(kQuery, true);
	}

	CONT_REALM_CANDIDATE kRealmCandi;

	{
		BM::CAutoMutex kLock(m_kMutex);
		pkControl->GetRealmCandi( kRealmCandi );
	}
	
	CONT_REALM_CANDIDATE::const_iterator realm_candi_itr =  kRealmCandi.begin();
	for( ; kRealmCandi.end() != realm_candi_itr ; ++realm_candi_itr )
	{
		CEL::DB_QUERY kQuery( iDBIndex, DQT_LOAD_REALM_CONFIG, _T("EXEC [dbo].[UP_LoadRealmConfig]"));

		kQuery.PushStrParam((*realm_candi_itr).second.RealmTblName());
		kQuery.PushStrParam((*realm_candi_itr).second.RealmNo());
		kQuery.PushStrParam((*realm_candi_itr).second.RealmName());
		g_kCoreCenter.PushQuery(kQuery, true);
	}

	BM::CAutoMutex kLock(m_kMutex);
	if ( !pkControl->InitRealmMgr() )
	{
		m_kContSite.erase( iDBIndex );
		SAFE_DELETE( pkControl );
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Error ID = " << iDBIndex );
		return false;
	}

	return true;
}

void PgMMCManager::Locked_ConnectImmigration(void)
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( true == m_bLoadPatchFileList )
	{
		return;
	}

	if ( BM::GUID::IsNotNull( ImmigrationConnector() ) )
	{
		CONT_SITE::iterator itr = m_kContSite.begin();
		for ( ; itr != m_kContSite.end() ; ++itr )
		{
			itr->second->Connect( ImmigrationConnector() );
		}
	}
}

void PgMMCManager::Locked_CheckHeartBeat()
{
	BM::CAutoMutex kLock(m_kMutex);

	BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO, MCT_REFRESH_STATE);
	size_t const iWrPos = kPacket.WrPos();
	size_t iSize = 0;
	kPacket.Push( iSize );

	CONT_SITE::iterator itr = m_kContSite.begin();
	CONT_SERVER_ID kContServerId;
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		if ( true == itr->second->CheckHeartBeat( BM::GetTime32(), IntervalTime() * 3, kContServerId ) )
		{
			++iSize;
			kPacket.Push( itr->first );
			itr->second->WriteToPacket_MonTool( kPacket );
		}
	}
	if ( iSize )
	{
		if( kContServerId.size() )
		{
			BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO);
			kPacket.Push(MCT_SERVER_OFF);
			PU::TWriteArray_M(kPacket, kContServerId);
			BroadCast_MonTool(kPacket);
		}

		kPacket.ModifyData( iWrPos, &iSize, sizeof(iSize) );
		BroadCast_MonTool( kPacket );
	}

	switch(g_kLocal.ServiceRegion())
	{
	case LOCAL_MGR::NC_DEVELOP:
	case LOCAL_MGR::NC_KOREA:
		{
			PgSiteControl *pkControl = GetSiteControl( SITE_NO );
			if ( pkControl )
			{
				// 렐름별로 데이터 생성
				CONT_REALM_CANDIDATE kContRealmCandi;
				pkControl->GetRealmCandi( kContRealmCandi );

				CONT_REALM_CANDIDATE::iterator realm_iter = kContRealmCandi.begin();
				while( kContRealmCandi.end() != realm_iter )
				{
					CONT_REALM_CANDIDATE::key_type iRealmNo = realm_iter->first;

					if( 0 < iRealmNo )
					{
						BM::Stream kPacket( PT_MMC_CONSENT_NFY_INFO );
						pkControl->WriteToPacket_AdminGateway( kPacket, iRealmNo );
						pkControl->SendToImmigration( kPacket );
					}

					++realm_iter;
				}
			}
		}break;
	}
}

bool PgMMCManager::Locked_SetConnectImmigration( CEL::CSession_Base* pkSession, bool const bConnect )
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_SITE::iterator itr = m_kContSite.begin();
	SERVER_IDENTITY kServerIdentity;
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		if ( true == itr->second->SetConnect( pkSession, bConnect, kServerIdentity ) )
		{
			if( !bConnect )
			{
				BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO);
				kPacket.Push(MCT_SERVER_OFF);
				CONT_SERVER_ID kContServerId;
				kContServerId.push_back(kServerIdentity);
				PU::TWriteArray_M(kPacket, kContServerId);
				BroadCast_MonTool(kPacket);
			}
			return true;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Session Error : ") << pkSession->Addr().ToString() );
	return false;
}

bool PgMMCManager::Locked_SetSycnEndSMC( CEL::CSession_Base* pkSession )
{
	int iSiteNo = GetSiteNo( pkSession );
	bool bRet = false;

	BM::CAutoMutex kLock(m_kMutex);

	PgSiteControl *pkControl = GetSiteControl( iSiteNo );
	if ( !pkControl )
	{
		return false;
	}

	if( pkControl->SetSyncEndSMC( pkSession ) )
	{
		m_kContSyncSession.erase(pkSession);	
	}
	
	return true;
}

bool PgMMCManager::Locked_SetConnectSMC( CEL::CSession_Base* pkSession, SERVER_IDENTITY_SITE &kRecvSI, bool const bConnect )
{
	BM::CAutoMutex kLock(m_kMutex);

	if ( !bConnect )
	{
		m_kContSyncSession.erase(pkSession);
	}

	CONT_SITE::iterator itr = m_kContSite.begin();
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		if ( true == itr->second->SetConnectSMC( pkSession, kRecvSI, bConnect ) )
		{
			kRecvSI.nSiteNo = static_cast<short>(itr->first);
			return true;
		}
	}

	return false;
}

bool PgMMCManager::ReadIni_Path( CEL::E_SESSION_TYPE const &eST, std::wstring const &strCategory, std::wstring const &strFileName )
{
	TCHAR szSrcFolder[MAX_PATH] = {0, };
	TCHAR szExe[MAX_PATH] = {0, };

	std::wstring strExeElement = _T("EXE_") + strCategory;
	std::wstring strSrcElement = _T("SRC_") + strCategory;

	if(		0 < ::GetPrivateProfileString( L"SYNC", strSrcElement.c_str(), _T(""), szSrcFolder, MAX_PATH, strFileName.c_str()) 
	&&	0 < ::GetPrivateProfileString( L"SYNC", strExeElement.c_str(), _T(""), szExe, MAX_PATH, strFileName.c_str())
	)
	{
		auto ret = m_kSyncPath.insert( std::make_pair(eST, SGameServerPath(szSrcFolder, szExe)) );
		return ret.second;
	}
	else if( (CEL::ST_NONE == eST) && (L"CONFIG" == strCategory) )
	{
		auto ret = m_kSyncPath.insert( std::make_pair(eST, SGameServerPath(szSrcFolder, szExe)) );
		return ret.second;
	} 
	return false;
}

HRESULT CALLBACK PgMMCManager::OnDB_EXECUTE( CEL::DB_RESULT &rkResult )
{
	switch(rkResult.QueryType())
	{
	case DQT_LOAD_SITE_CONFIG:
		{
			g_kMMCMgr.Locked_DQT_LOAD_SITE_CONFIG( rkResult );
		}break;
	case DQT_LOAD_REALM_CONFIG:
		{
			g_kMMCMgr.Locked_DQT_LOAD_REALM_CONFIG( rkResult );
		}break;
	case DQT_PATCH_VERSION_EDIT:
		{
			g_kMMCMgr.Locked_DQT_PATCH_VERSION_EDIT(rkResult);
		}break;
	}
	return S_OK;
}

HRESULT CALLBACK PgMMCManager::OnDB_EXECUTE_TRAN( CEL::DB_RESULT_TRAN &rkResult )
{
	return S_OK;
}

HRESULT PgMMCManager::Locked_DQT_LOAD_SITE_CONFIG( CEL::DB_RESULT &rkResult )
{
	if(		CEL::DR_SUCCESS == rkResult.eRet 
		||	CEL::DR_NO_RESULT == rkResult.eRet
		)
	{
		BM::CAutoMutex kLock(m_kMutex);

		PgSiteControl *pkControl = GetSiteControl( rkResult.DBIndex() );
		if ( pkControl )
		{
			CONT_REALM_CANDIDATE kRealmCandi;

			CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();		
			while(rkResult.vecArray.end() != itr)
			{
				short nRealmNo = 0;
				std::wstring  strRealmName;
				std::wstring  strRealmTblName;
				std::wstring  strNotice_TblName;

				itr->Pop(nRealmNo);			++itr;
				itr->Pop(strRealmName);		++itr;
				itr->Pop(strRealmTblName);	++itr;
				itr->Pop(strNotice_TblName); ++itr;

				auto kPair = kRealmCandi.insert( std::make_pair( nRealmNo,  SRealmElement(nRealmNo, strRealmName, strRealmTblName, strNotice_TblName) ) );
				if ( !kPair.second )
				{
					CAUTION_LOG( BM::LOG_LV4, __FL__ << L"AddRealm Error RealmNo<" << nRealmNo << L"> RealmName<" << strRealmName << L"> TblName<" << strRealmTblName << L">" );
				}
			}

			pkControl->SetRealmCandi( kRealmCandi );
			return S_OK;
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found Index = " << rkResult.DBIndex() );
		}

	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"None Data" );
	}
	
	LIVE_CHECK_LOG( BM::LOG_LV0, __FL__ << L"return E_FAIL")
	return E_FAIL;
}

HRESULT PgMMCManager::Locked_DQT_LOAD_REALM_CONFIG( CEL::DB_RESULT &rkResult )
{
	if(		CEL::DR_SUCCESS == rkResult.eRet 
		||	CEL::DR_NO_RESULT == rkResult.eRet
		)
	{
		short nRealm = 0;
		std::wstring kRealmName;

		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itr )
		{
			itr->Pop( nRealm );		++itr;
			itr->Pop( kRealmName );	++itr;
		}

		if( rkResult.vecArray.end() != itr )
		{
			BM::CAutoMutex kLock(m_kMutex);

			PgSiteControl *pkControl = GetSiteControl( rkResult.DBIndex() );
			if ( pkControl )
			{
				while( rkResult.vecArray.end() != itr )
				{
					CONT_SERVER_HASH::mapped_type element;

					std::wstring kName;
					std::wstring kMachineIP;
					std::wstring kBindIP;
					std::wstring kNATIP;
					unsigned short usBindPort = 0;
					unsigned short usNatPort = 0;

					element.nRealm = nRealm;
					itr->Pop( element.nChannel );		++itr;
					itr->Pop( element.nServerNo );		++itr;
					itr->Pop( element.nServerType );	++itr;
					itr->Pop( kMachineIP );				++itr;
					itr->Pop( kName );					++itr;

					itr->Pop( kBindIP );				++itr;
					itr->Pop( usBindPort );				++itr;
					itr->Pop( kNATIP );					++itr;
					itr->Pop( usNatPort );				++itr;

					element.strMachinIP = kMachineIP;
					element.addrServerBind.Set(kBindIP, usBindPort);
					element.addrServerNat.Set(kNATIP, usNatPort);

					kBindIP = L"";
					kNATIP = L"";
					usBindPort = 0;
					usNatPort = 0;

					itr->Pop( kBindIP );				++itr;
					itr->Pop( usBindPort );				++itr;
					itr->Pop( kNATIP );					++itr;
					itr->Pop( usNatPort );				++itr;

					element.addrUserBind.Set(kBindIP, usBindPort);
					element.addrUserNat.Set(kNATIP, usNatPort);
					element.strName = kName;
					element.Build();

					if ( !pkControl->AddServerInfo( element ) )
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"Already Use ServerIdentity DBIndex<" << rkResult.DBIndex() << L"> Realm<" << element.nRealm << L"> Channel<" << element.nChannel << L"> ServerNo<" << element.nServerNo << L"> ServerType<" << element.nServerType << L">" );
					}

					if ( 1 == m_kContSite.size() )
					{
						if ( (tagServerIdentity)element == g_kProcessCfg.ServerIdentity() )
						{
							RegistAcceptor( element );
						}
					}
				}

				
				return S_OK;
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found Index = " << rkResult.DBIndex() );
			}
			
		}
		else
		{//렐름 테이블 명을 잘못 기입.
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << L"DBIndex<" << rkResult.DBIndex() << L"> TblName<" << kRealmName << L"> in Realm<" << nRealm << L">" );
		}
	}

	LIVE_CHECK_LOG( BM::LOG_LV0, __FL__ << L"return E_FAIL")
	return E_FAIL;
}

HRESULT PgMMCManager::Locked_DQT_PATCH_VERSION_EDIT( CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor )
		{
			int iMajor = 0, iMinor = 0, iPatch = 0;
			(*itor).Pop(iMajor); ++itor;
			(*itor).Pop(iMinor); ++itor;
			(*itor).Pop(iPatch); ++itor;

			SetPatchVersion(iMajor, iMinor, iPatch);
		}
		return S_OK;
	}

	return E_FAIL;
}

void PgMMCManager::Locked_GetPatchVersion(std::wstring &wstrVersion)const
{
	BM::CAutoMutex kLock( m_kMutex );
	wchar_t chVersion[80];
	_stprintf_s(chVersion, _countof(chVersion), _T("%I64d, %I64d, %I64d"), m_kVersion.Version.i16Major, 
		m_kVersion.Version.i16Minor, m_kVersion.Version.i32Tiny);

	wstrVersion = chVersion;
}

PgSiteControl* PgMMCManager::GetSiteControl( int const iDBIndex )const
{
	CONT_SITE::const_iterator itor = m_kContSite.find( iDBIndex );
	if ( itor != m_kContSite.end() )
	{
		return itor->second;
	}
	return NULL;
}

PgSiteControl* PgMMCManager::GetSiteControl( CEL::CSession_Base* const pkSession )const
{
	int const iDBIndex = GetSiteNo(pkSession);
	CONT_SITE::const_iterator itor = m_kContSite.find( iDBIndex );
	if ( itor != m_kContSite.end() )
	{
		return itor->second;
	}
	return NULL;
}

bool PgMMCManager::Locked_GetServerHash( int const iDBIndex, CONT_SERVER_HASH &rkServerHash )const
{
	BM::CAutoMutex kLock(m_kMutex);
	PgSiteControl *pkControl = GetSiteControl( iDBIndex );
	if ( pkControl )
	{
		pkControl->GetServerHash( rkServerHash );
		return true;
	}
	return false;
}

void PgMMCManager::Locked_LoadPatchFileList( bool const bInit )
{
	CON_SYNC_PATH kContSyncPath;
	CONT_PATCH_FILELIST kPatchFileList;

	{
		BM::CAutoMutex kLock(m_kMutex);
		if ( !m_bLoadPatchFileList || (true == bInit) )
		{
			kContSyncPath = m_kSyncPath;
			m_bLoadPatchFileList = true;
		}
		else
		{
			return;
		}
	}
	
	LoadPatchFileList( kContSyncPath, kPatchFileList );

	BM::CAutoMutex kLock(m_kMutex);

	m_bLoadPatchFileList = false;
	m_kContPatchFileList.swap( kPatchFileList );

	size_t iSyncingCount = 0;

	CONT_SYNC_SESSION::iterator itor = m_kContSyncSession.begin();
	for ( ; itor != m_kContSyncSession.end() ; ++itor )
	{
		itor->second.byState = SSyncSessionInfo::SYNC_WAIT;
		itor->second.dwLastGetFileTime = BM::GetTime32();
		itor->second.wstrFileName.clear();

		if ( m_iMaxSyncCount > iSyncingCount++ )
		{
			Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO( itor->first, true );
			itor->second.byState = SSyncSessionInfo::SYNC_STEP1;
		}
	}
	
	CONT_SITE::const_iterator itor_site = m_kContSite.begin();
	for ( ; itor_site != m_kContSite.end() ; ++itor_site )
	{
		itor_site->second->Clear_SMC();
	}
}

void PgMMCManager::LoadPatchFileList( CON_SYNC_PATH const &kSyncPath, CONT_PATCH_FILELIST &rkOutPatchFileList )
{
	PgTimeCheck kTime( dynamic_cast<PgLogWorker_Base*>(&g_kLogWorker), LT_MAIN, __FUNCTIONW__, __LINE__ );

	HANDLE hProcess = ::GetCurrentProcess();
	::SetPriorityClass( hProcess, REALTIME_PRIORITY_CLASS );

	INFO_LOG( BM::LOG_LV6, L"[LoadPatchFileList] ---- Begin" );

	rkOutPatchFileList.clear();
	CON_SYNC_PATH::const_iterator path_Itr = kSyncPath.begin();
	for ( ; path_Itr != kSyncPath.end() ; ++path_Itr )
	{
		auto kPair = rkOutPatchFileList.insert( std::make_pair( path_Itr->second.kSrcPath, CONT_PATCH_FILELIST::mapped_type() ) );
		if ( true == kPair.second )
		{
			INFO_LOG( BM::LOG_LV7, L"* GetFolder : " << kPair.first->first );
			if ( true == BM::PgDataPackManager::GetFileList( kPair.first->first, kPair.first->second ) )
			{
				INFO_LOG( BM::LOG_LV7, L"   -----> OK" );
			}
			else
			{
				INFO_LOG( BM::LOG_LV7, L"   -----> Failed" );
			}
		}
	}

	INFO_LOG( BM::LOG_LV6, L"[LoadPatchFileList] ---- End");

	::SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
	::CloseHandle(hProcess);
}

void PgMMCManager::Locked_CheckSync( void )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( true == m_bLoadPatchFileList )
	{
		return;
	}

	DWORD const dwCurTime = BM::GetTime32();

	size_t iSyncingCount = 0;

	CONT_SYNC_SESSION::iterator itor = m_kContSyncSession.begin();
	while ( m_kContSyncSession.end() != itor )
	{
		if ( SSyncSessionInfo::SYNC_WAIT2 < itor->second.byState )
		{
			if ( itor->second.wstrFileName.size() )
			{//문제가 있어서 못보낸 파일들 다시 태스크로 보냄
				CEL::CSession_Base* const pkSession = itor->first;

				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
				kEventMsg.SecType(PT_SMC_MMC_REQ_GET_FILE);

				kEventMsg.Push( pkSession->SessionKey() );
				kEventMsg.Push( itor->second.wstrFileName );
				g_kTask.PutMsg(kEventMsg);

				itor->second.wstrFileName.clear();
				itor->second.dwLastGetFileTime = dwCurTime;
			}
			else
			{
				DWORD const dwTime = ::DifftimeGetTime( itor->second.dwLastGetFileTime, dwCurTime );
				if ( dwTime >= ms_dwSyncTimeOut )
				{
					// 맨뒤로 보내야 겠네
					CONT_SYNC_SESSION::value_type kElement = *itor;
					itor = m_kContSyncSession.erase( itor );

					if ( dwTime < ms_dwSyncEndTime )
					{
						m_kContSyncSession.insert( kElement );
					}
					else
					{
						CAUTION_LOG( BM::LOG_LV5, __FL__ << L"Time Over Session " << kElement.first->Addr().ToString() );
					}	
					continue;
				}	
			}
		}
		else
		{
			// 싱크 시작!
			Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO( itor->first, itor->second.byState == SSyncSessionInfo::SYNC_WAIT );
			itor->second.byState += SSyncSessionInfo::SYNC_ADD_STEP;
			itor->second.dwLastGetFileTime = dwCurTime;	
		}

		if ( m_iMaxSyncCount <= ++iSyncingCount )
		{
			break;
		}

		++itor;
	}
}

void PgMMCManager::SendFile( CEL::SESSION_KEY const &kSessionKey, std::wstring const &wstrAddr, std::wstring const &wstrFileName )
{
	if ( false == CheckMemory(wstrFileName) )
	{
		INFO_LOG(BM::LOG_LV2, _T("Not Enought Memory!") );

		SEventMessage kEventMsg;
		kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
		kEventMsg.SecType(PT_SMC_MMC_REQ_GET_FILE);

		kEventMsg.Push( kSessionKey );
		kEventMsg.Push( wstrFileName );
		g_kTask.PutMsg(kEventMsg);
		return;
	}

	// 파일을 주면 된다.
	INFO_LOG(BM::LOG_LV2, _T("Recv[") << wstrAddr.c_str() << _T("] GetFile Start[") << wstrFileName.c_str() << _T("]") );

	std::vector< char > kFileData;
	BM::FileToMem( wstrFileName, kFileData);

	BM::Stream kPacket( PT_MMC_SMC_ANS_GET_FILE, wstrFileName );
	kPacket.Push( kFileData );

	if( g_kCoreCenter.Send(kSessionKey, kPacket) )
	{
		INFO_LOG(BM::LOG_LV3, _T("Recv[") << wstrAddr.c_str() << _T("] GetFile End[") << wstrFileName.c_str() << _T("]") );
	}
	else
	{//! 보내려는 시점에서 세션이 끊어질 수도 있다.
		INFO_LOG(BM::LOG_LV5, _T("Send Failed! Addr : [") << wstrAddr.c_str() << _T("] FileName : [") << wstrFileName.c_str() << _T("]") );
	}
}

void PgMMCManager::Locked_GetFile( CEL::SESSION_KEY const &kSessionKey, BM::Stream* const pkPacket )
{
	std::wstring wstrAddr, wstrFileName;
	pkPacket->Pop( wstrFileName );

	bool bRet = false;
	{
		BM::CAutoMutex kLock( m_kMutex );

		size_t iCurSyncCount = m_kContSyncSession.size();
		CONT_SYNC_SESSION::iterator itor = m_kContSyncSession.begin();
		while( m_kContSyncSession.end() != itor )
		{
			if( itor->first->SessionKey() == kSessionKey )
			{
				itor->second.dwLastGetFileTime = BM::GetTime32();
				if( m_iMaxSyncCount > iCurSyncCount )
				{
					wstrAddr = itor->first->Addr().IP();
					bRet= true;
					break;
				}
				else
				{//! 싱크 갯수 초과 이거나 메모리가 부족하면 나중에 처리
					itor->second.wstrFileName = wstrFileName;
					return;
				}
			}
			++itor;
		}
	}

	if( !bRet )
	{ 
		CAUTION_LOG( BM::LOG_LV5, __FL__ << L"Not Fount SessionID : " << kSessionKey.SessionGuid() );
		BM::Stream kPacket( PT_MMC_SMC_NFY_CMD, MCT_RE_SYNC );
		kPacket.Push( CEL::SESSION_KEY() );
		g_kCoreCenter.Send( kSessionKey, kPacket );
		return;
	}

	SendFile(kSessionKey, wstrAddr, wstrFileName); 
}

void PgMMCManager::Locked_Recv_PT_SMC_MMC_REQ_DATA_SYNC_INFO( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	bool IsSMC_SyncStart = false;
	pkPacket->Pop(IsSMC_SyncStart);	
	BYTE const byState = (( true == IsSMC_SyncStart ) ? SSyncSessionInfo::SYNC_WAIT : SSyncSessionInfo::SYNC_WAIT2 );

	BM::CAutoMutex kLock(m_kMutex);

	CONT_SYNC_SESSION::iterator itor = m_kContSyncSession.find(pkSession);
	if( m_kContSyncSession.end() != itor)
	{
		itor->second.byState = byState;
		itor->second.dwLastGetFileTime = BM::GetTime32();
		itor->second.wstrFileName.clear();
	}
	else
	{
		m_kContSyncSession.insert( std::make_pair(pkSession, SSyncSessionInfo(byState)) );
	}
}

void PgMMCManager::Send_PT_SMC_MMC_REQ_DATA_SYNC_INFO( CEL::CSession_Base *pkSession, bool const bIsSMC_SyncStart )
{
	BM::Stream kSendPacket( PT_MMC_SMC_ANS_DATA_SYNC_INFO );
	kSendPacket.Push( bIsSMC_SyncStart );

	if( true == bIsSMC_SyncStart )
	{
		CON_SYNC_PATH::const_iterator Itr = m_kSyncPath.find( CEL::ST_SUB_MACHINE_CONTROL );
		if( Itr == m_kSyncPath.end() )
		{
			INFO_LOG(BM::LOG_LV0, _T("Find not SMC Patch Info in MMC.ini File"));
		}

		CON_SYNC_PATH kSendPath;
		kSendPath.insert( std::make_pair(Itr->first, Itr->second) );
		PU::TWriteTable_MM( kSendPacket, kSendPath );

		CONT_PATCH_FILELIST::const_iterator file_itr = m_kContPatchFileList.find( Itr->second.kSrcPath );
		if ( file_itr != m_kContPatchFileList.end() )
		{
			PU::TWriteTable_AM( kSendPacket, file_itr->second );
		}
		else
		{
			const BM::FolderHash kMMCFolders;
			PU::TWriteTable_AM( kSendPacket, kMMCFolders );
		}	
	}
	else
	{
		CON_SYNC_PATH kSendPath;
		ContServerID kSyncList;

		//default로 Config폴더는 항상 들어가야 한다.
		CON_SYNC_PATH::const_iterator config_Itr = m_kSyncPath.find(CEL::ST_NONE);
		if ( config_Itr != m_kSyncPath.end() )
		{
			kSendPath.insert( std::make_pair(config_Itr->first, config_Itr->second) );
		}
		
		PgSiteControl *pkControl = GetSiteControl( GetSiteNo(pkSession ) );
		if ( pkControl )
		{
			pkControl->GetPatchServerList( pkSession, kSyncList );

			ContServerID::iterator SI_Itr = kSyncList.begin();
			for( ; SI_Itr != kSyncList.end() ; ++SI_Itr )
			{
				CON_SYNC_PATH::const_iterator Itr = m_kSyncPath.find((CEL::E_SESSION_TYPE)SI_Itr->nServerType);
				if( Itr != m_kSyncPath.end() )
				{
					kSendPath.insert(std::make_pair(Itr->first, Itr->second));
				}
			}

		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error" );
		}

		PU::TWriteTable_MM(kSendPacket, kSendPath);

		CON_SYNC_PATH::const_iterator path_itr = kSendPath.begin();
		for( ; kSendPath.end() != path_itr ; ++path_itr )
		{
			CONT_PATCH_FILELIST::const_iterator file_itr = m_kContPatchFileList.find( path_itr->second.kSrcPath );
			if ( file_itr != m_kContPatchFileList.end() )
			{
				PU::TWriteTable_AM( kSendPacket, file_itr->second );
			}
			else
			{
				const BM::FolderHash kMMCFolders;
				PU::TWriteTable_AM( kSendPacket, kMMCFolders );
			}	
		}
	}

	pkSession->VSend(kSendPacket);
}

BYTE PgMMCManager::Locked_LoginMonTool(CEL::CSession_Base *pkSession, std::wstring const &kID, std::wstring const &kPW )
{
	if( pkSession )
	{
		BM::CAutoMutex kLock(m_kMutex);
		CONT_MON_TOOL_SESSION::iterator Itr = m_kUserAuthInfo.find(kID);
		if(Itr != m_kUserAuthInfo.end())
		{
			SMonToolInfo &kElement = Itr->second;
			if ( kElement.kSessionKey == CEL::SESSION_KEY() )
			{
				if ( true == kElement.IsEqual( kID, kPW, pkSession->Addr().IP() ) )
				{
					kElement.kSessionKey = pkSession->SessionKey();
					pkSession->m_kSessionData.Push( kElement.cGrant );
					pkSession->m_kSessionData.Push( kID );
					return kElement.cGrant;
				}
				else
				{
					INFO_LOG( BM::LOG_LV5, __FL__ << _T("Wrong Auth Info[") 	<< kElement.kID << L"]["
						<< kElement.kAuthIP << L"] != [" << kID 	
						<< L"][" << pkSession->Addr().IP() << L"]" );
				}
			}
		}
	}

	return 0;
}

bool PgMMCManager::Locked_LogOutMonTool( CEL::CSession_Base *pkSession )
{
	if( pkSession )
	{
		pkSession->m_kSessionData.PosAdjust();

		BYTE byGrant = 0;
		std::wstring wstrID;
		pkSession->m_kSessionData.Pop( byGrant );
		pkSession->m_kSessionData.Pop( wstrID );

		BM::CAutoMutex kLock(m_kMutex);
		CONT_MON_TOOL_SESSION::iterator Itr = m_kUserAuthInfo.find( wstrID );
		if(Itr != m_kUserAuthInfo.end())
		{
			if ( Itr->second.kSessionKey == pkSession->SessionKey() )
			{
				Itr->second.kSessionKey = CEL::SESSION_KEY();
				return true;
			}
		}
	}
	return false;
}

bool PgMMCManager::Locked_RecvMonToolCmd( CEL::CSession_Base *pkSession, EMMC_CMD_TYPE const eCmdType, BM::Stream* const pkPacket )
{
	if ( pkSession->m_kSessionData.IsEmpty() )
	{
		BM::Stream kPacket(PT_ANS_MMC_TOOL_CMD_RESULT);
		int iErrorCode = 2;
		kPacket.Push(iErrorCode);
		pkSession->VSend(kPacket);
		pkSession->VTerminate();
		return false;
	}

	BYTE byGrant = 0;
	std::wstring wstrID;
	pkSession->m_kSessionData.Pop( byGrant );
	pkSession->m_kSessionData.Pop( wstrID );
	pkSession->m_kSessionData.PosAdjust();

	switch(eCmdType)
	{
	case MCT_REQ_NOTICE:
		{
			int iSiteNo = 1;
			short nRealm=0, nChannel=0;
			std::wstring wstrContent;
			pkPacket->Pop(iSiteNo);
			pkPacket->Pop(nRealm);
			pkPacket->Pop(nChannel);
			pkPacket->Pop(wstrContent);

			PgSiteControl *pkControl = GetSiteControl( iSiteNo );
			if( pkControl ) 
			{
				BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_NOTICE);
				kPacket.Push(nRealm);
				kPacket.Push(nChannel);
				kPacket.Push(wstrContent);
				pkControl->SendToImmigration(kPacket);
			}
		}break;
	case MCT_REFRESH_STATE:
	case MCT_REFRESH_LIST:
		{
			BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO, eCmdType);
			kPacket.Push( m_kContSite.size() );

			BM::CAutoMutex kLock(m_kMutex);

			CONT_SITE::const_iterator site_itr = m_kContSite.begin();
			for ( ; site_itr != m_kContSite.end() ; ++site_itr )
			{
				kPacket.Push( site_itr->first );
				site_itr->second->WriteToPacket_MonTool( kPacket );
			}
			pkSession->VSend(kPacket);
		}break;
	case MCT_SERVER_ON:
	case MCT_SERVER_SHUTDOWN:
	case MCT_SERVER_OFF:
		{
			if ( USE_COMMAND_LEVEL != byGrant )
			{
				CAUTION_LOG( BM::LOG_LV5, L"User" << wstrID << L" Command<" << static_cast<int>(eCmdType) << L"> Error!" );
				return false;
			}

			if( true == m_bLoadPatchFileList )
			{
				BM::Stream kPacket(PT_ANS_MMC_TOOL_CMD_RESULT);
				int iErrorCode = 1;
				kPacket.Push(iErrorCode);
				pkSession->VSend(kPacket);
				return true;
			}

			int iSiteNo = 0;
 			ContServerID kCommandVec;
			pkPacket->Pop( iSiteNo );
 			PU::TLoadArray_M(*pkPacket, kCommandVec);

			BM::CAutoMutex kLock(m_kMutex);

			PgSiteControl *pkControl = GetSiteControl( iSiteNo );
			if ( pkControl )
			{
				pkControl->ServerControl( pkSession->SessionKey(), kCommandVec, eCmdType );
			}
		}break;
	case MCT_RE_SYNC:
		{
			if ( USE_COMMAND_LEVEL != byGrant )
			{
				CAUTION_LOG( BM::LOG_LV5, L"User" << wstrID << L" Command<" << static_cast<int>(eCmdType) << L"> Error!" );
				return false;
			}

			if( true == m_bLoadPatchFileList )
			{
				BM::Stream kPacket(PT_ANS_MMC_TOOL_CMD_RESULT);
				int iErrorCode = 1;
				kPacket.Push(iErrorCode);
				pkSession->VSend(kPacket);
				return true;
			}

			BM::CAutoMutex kLock(m_kMutex);
			//! SMC 접속을 모두 끊는다. (SMC는 자동으로 재접속시도 후 리싱크 시작한다)
			Disconnect_SMC();

			SEventMessage kEventMsg;
			kEventMsg.PriType(PMET_MMC_MANAGER);
			kEventMsg.SecType(PT_MMC_LOADPATCHFILE);
			kEventMsg.Push( false );
			g_kTask.PutMsg(kEventMsg);
		}break;
	case MCT_CHANGE_MAXUSER:
		{
			if ( USE_COMMAND_LEVEL != byGrant )
			{
				CAUTION_LOG( BM::LOG_LV5, L"User" << wstrID << L" Command<" << static_cast<int>(eCmdType) << L"> Error!" );
				return false;
			}

			int iSiteNo = 0;
			pkPacket->Pop( iSiteNo );

			BM::CAutoMutex kLock(m_kMutex);

			PgSiteControl *pkControl = GetSiteControl( iSiteNo );
			if ( pkControl )
			{
 				BM::Stream kPacket(PT_MCTRL_MMC_A_NFY_SERVER_COMMAND, MCC_CHANGE_MAXUSER);
 				kPacket.Push(*pkPacket);
				pkControl->SendToImmigration( kPacket );

				//! MMC/Patch/Config/ 폴더의 ini도 바꿔줘야, 다음 서버 기동시에도 적용됨.
				SERVER_IDENTITY kSI;
				int iMaxUser = 0;
				BM::Stream kTmpPacket(*pkPacket);
				kSI.ReadFromPacket(kTmpPacket);
				kTmpPacket.Pop(iMaxUser);
				ChangeMaxUser(kSI, iMaxUser);
			}
		}break;
	case MCT_GET_LOG_FILE_INFO:
		{
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] REQ FILE LIST"));

			int iKeyValue = 0;
			pkPacket->Pop(iKeyValue);
			m_iKeyValue = iKeyValue;

			if( FT_CATION_HACK == iKeyValue )
			{
				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
				kEventMsg.SecType(PT_MCT_REQ_GET_FILE_INFO);
				kEventMsg.Push( pkSession->SessionKey() );
				kEventMsg.Push( *pkPacket );
				g_kTask.PutMsg(kEventMsg);
			}
			else
			{
				BM::Stream kPacket(PT_MMC_SMC_NFY_CMD, MCT_GET_LOG_FILE_INFO);
				CEL::SESSION_KEY kSessionKey = pkSession->SessionKey();
				kPacket.Push( kSessionKey );
				kPacket.Push( *pkPacket );

				BM::CAutoMutex kLock(m_kMutex);
				g_kMMCMgr.BroadCast_SMC( kPacket );
			}
		}break;
	case MCT_REFRESH_LOG:
		{//! [ 로그 정보 갱신 ] -- 요건 task로 안가고 바로 SMC로 broadcasting 해주자.
			INFO_LOG( BM::LOG_LV2, _T("[ServerTool] REQ REFRESH FILE"));
			if(m_pkCmdReserveSession) 
			{// 다른 MCT의 명령을 처리중 이므로 실패. 
				//break;
			}

			m_pkCmdReserveSession = pkSession;

			int iKeyValue = NULL;
			pkPacket->Pop(iKeyValue);
			if(iKeyValue != FT_CATION_HACK)
			{//! 0~1 Realm or Dump
				BM::Stream kPacket(PT_MMC_SMC_NFY_CMD, MCT_REFRESH_LOG);
				kPacket.Push(iKeyValue);

				BM::CAutoMutex kLock(m_kMutex);
				BroadCast_SMC( kPacket );
			}
			else
			{//! Caution/Hack
				pkPacket->Push(iKeyValue);
				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
				kEventMsg.SecType(PT_MCT_REFRESH_LOG);
				kEventMsg.Push( pkSession->SessionKey() );
				kEventMsg.Push( pkPacket );
				g_kTask.PutMsg(kEventMsg);
			}
		}break;
	case MCT_REQ_GET_FILE:
		{
			int iKeyValue = NULL;
			pkPacket->Pop(iKeyValue);

			if(iKeyValue == FT_CATION_HACK)
			{//! Caution/Hack 로그는 MMC에서 처리.
				SEventMessage kEventMsg;
				kEventMsg.PriType(PMET_FILE_COPY_MANAGER);
				kEventMsg.SecType(PT_MCT_REQ_GET_FILE);
				kEventMsg.Push( pkSession->SessionKey() );
				kEventMsg.Push( pkPacket );
				g_kTask.PutMsg(kEventMsg);
			}
			else
			{//! 기타등등은 내꺼 아니다. SMC로 보내자.
				BM::Stream kPacket(PT_MMC_SMC_NFY_CMD, MCT_REQ_GET_FILE);
				CEL::SESSION_KEY kSessionKey = pkSession->SessionKey();
				kPacket.Push(kSessionKey);
				kPacket.Push(iKeyValue);
				kPacket.Push(*pkPacket);

				BM::CAutoMutex kLock(m_kMutex);
				g_kMMCMgr.BroadCast_SMC( kPacket );
			}
			
		}break;
	default:
 		{
		}break;
	}
	return true;
}

void PgMMCManager::Locked_Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND( CEL::CSession_Base *pkSession, E_IMM_MCC_CMD_TYPE const eCmdType, BM::Stream * const pkPacket )
{
	pkSession->m_kSessionData.PosAdjust();

	int iSiteNo = 0;
	pkSession->m_kSessionData.Pop( iSiteNo );

	BM::CAutoMutex kLock( m_kMutex );

	PgSiteControl *pkControl = GetSiteControl( iSiteNo );
	if ( pkControl )
	{
		CONT_SERVER_ID kContServerId;
		pkControl->Recv_PT_MCTRL_A_MMC_ANS_SERVER_COMMAND( pkSession, eCmdType, pkPacket, kContServerId );
		if( pkControl->IsRefresh() )
		{
			BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO);
			kPacket.Push(eCmdType);
			PU::TWriteArray_M(kPacket, kContServerId);
			BroadCast_MonTool(kPacket);
			INFO_LOG(BM::LOG_LV6, _T("PT_MMC_TOOL_NFY_INFO [CMD TYPE : ") << static_cast<short>(eCmdType) << _T("]"));
		}
	}
}

void PgMMCManager::Locked_Recv_PT_SMC_MMC_ANS_CMD( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	int const iSiteNo = GetSiteNo(pkSession);
	EMMC_CMD_TYPE eCmdType = MCT_ERR_CMD;
	CEL::SESSION_KEY kSessionKey;
	SERVER_IDENTITY kServerIdentity;
	bool bRet = false;
	BM::CAutoMutex kLock( m_kMutex );

	pkPacket->Pop( eCmdType );
	switch(eCmdType)
	{
	case MCT_REFRESH_LOG:
		{
			//Locked_SendToReservedSession(pkSession, pkPacket);
			BM::CAutoMutex kLock( m_kMutex );
			if(!m_pkCmdReserveSession)
			{// 세션이 비어있음...-_-
				return;
			}

			int iKeyValue = 0; 
			pkPacket->Pop(iKeyValue);

			BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO, MCT_REFRESH_LOG);
			kPacket.Push(iKeyValue);
			if(iKeyValue == FT_DUMP)
			{
				kPacket.Push(pkSession->Addr().IP());
			}

			kPacket.Push(*pkPacket);

			//! 문제 없다. 패킷 전송.
			m_pkCmdReserveSession->VSend(kPacket);

			//! 작업 끝났으므로 포인터를 NULL로 만든다.
			m_pkCmdReserveSession = NULL;
		}
		break;
	case MCT_SMC_ANS_LOG_FILE_INFO:
		{
			CEL::SESSION_KEY kSessionKey;
			pkPacket->Pop(kSessionKey);

			BM::Stream kPacket( PT_MMC_TOOL_NFY_INFO, MCT_SMC_ANS_LOG_FILE_INFO);
			kPacket.Push(*pkPacket);
			g_kCoreCenter.Send( kSessionKey, kPacket );
		}break;
	case MCT_SMC_ANS_GET_FILE:
		{
			CEL::SESSION_KEY kSessionKey;
			pkPacket->Pop(kSessionKey);

			BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO);
			kPacket.Push(MCT_SMC_ANS_GET_FILE);
			kPacket.Push(*pkPacket);

			g_kCoreCenter.Send( kSessionKey, kPacket );
		}break;
	case MCT_SERVER_ON:
	case MCT_SERVER_OFF:
	case MCT_SERVER_SHUTDOWN:
		{
			pkPacket->Pop( kSessionKey );
			BM::Stream kPacket( PT_MMC_TOOL_NFY_INFO, eCmdType);
			kPacket.Push(*pkPacket);
			g_kCoreCenter.Send( kSessionKey, kPacket );
		}break;
	default:
		{
			pkPacket->Pop( kSessionKey );
			pkPacket->Pop( kServerIdentity );
			pkPacket->Pop( bRet );

			PgSiteControl *pkControl = GetSiteControl( iSiteNo );
			if ( pkControl )
			{

			}
		}break;
	}
}

void PgMMCManager::Locked_Recv_PT_SMC_MMC_REFRESH_STATE( CEL::CSession_Base *pkSession, BM::Stream * const pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	PgSiteControl *pkControl = GetSiteControl( pkSession );
	if ( pkControl )
	{
		pkControl->RefreshState( pkPacket );
	}
}

void PgMMCManager::BroadCast_MonTool( BM::Stream const &kPacket )const
{
	CONT_MON_TOOL_SESSION::const_iterator itr =	m_kUserAuthInfo.begin();
	for ( ; itr != m_kUserAuthInfo.end() ; ++itr )
	{
		if ( itr->second.kSessionKey != CEL::SESSION_KEY() )
		{
			g_kCoreCenter.Send( itr->second.kSessionKey, kPacket );
		}
	}
}

void PgMMCManager::BroadCast_SMC( BM::Stream const &kPacket )const
{
	CONT_SITE::const_iterator itr =	m_kContSite.begin();
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		itr->second->BroadCast_SMC( kPacket );
	}
}

void PgMMCManager::Disconnect_SMC()const
{
	CONT_SITE::const_iterator itr =	m_kContSite.begin();
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		itr->second->Disconnect_SMC();
	}
}

void PgMMCManager::Locked_GetServerList( short const nServerType, ContServerSiteID &rkOut )const
{
	BM::CAutoMutex kLock( m_kMutex );
	CONT_SITE::const_iterator itr =	m_kContSite.begin();
	for ( ; itr != m_kContSite.end() ; ++itr )
	{
		itr->second->GetServerList( nServerType, rkOut );
	}
}

bool PgMMCManager::ChangeMaxUser(SERVER_IDENTITY const & rkSI, int const iMaxUser)
{
	CONT_SERVER_ID kContServerId;
	if( (CEL::ST_CONTENTS == rkSI.nServerType && 0 == rkSI.nRealm)
		|| (CEL::ST_CENTER == rkSI.nServerType && -1 == rkSI.nChannel) )
	{
		CONT_SERVER_HASH kContServerHash;
		g_kProcessCfg.Locked_GetServerInfo( rkSI.nServerType, kContServerHash );
		
		CONT_SERVER_HASH::const_iterator server_itr = kContServerHash.begin();
		for ( ; server_itr != kContServerHash.end() ; ++server_itr )
		{
			SERVER_IDENTITY const &kSI = server_itr->first;
			kContServerId.push_back(kSI);
		}
	}
	else
	{
		kContServerId.push_back(rkSI);
	}

	ChangeMaxUser(kContServerId, iMaxUser);
	return true;
}

void PgMMCManager::ChangeMaxUser(CONT_SERVER_ID const & rkContServerId, int const iMaxUser)
{
	CONT_SERVER_ID::const_iterator itor = rkContServerId.begin();
	while( rkContServerId.end() != itor )
	{
		std::wstring kTargetPath;
		BM::vstring kCategory;
		switch( (*itor).nServerType )
		{
		case CEL::ST_CONTENTS :
			kTargetPath = L"Contents_Config.ini";
			kCategory = L"REALM_";
			kCategory += (*itor).nRealm;
			break;
		case CEL::ST_CENTER :
			kTargetPath = L"Center_Config.ini";
			kCategory = L"CHANNEL_";
			kCategory += (*itor).nChannel;
			break;
		default :
			return;
		}
		std::wstring kPatch = ConfigDir() + kTargetPath;

		//Ini파일에 저장해야 한다.
		wchar_t wcString[100] = {0,};
		_itow_s((int)iMaxUser, wcString, 100, 10);
		WritePrivateProfileStringW(kCategory.operator const wchar_t *(), L"MAX_USER_COUNT", wcString, kPatch.c_str());	

		++itor;
	}
}

bool PgMMCManager::CheckMemory(DWORD dwSize)
{
	MEMORYSTATUS kMemoryStatus;
	::GlobalMemoryStatus(&kMemoryStatus);
	if( kMemoryStatus.dwAvailPhys > dwSize )
	{
		return true;
	}
	
	return false;
}

bool PgMMCManager::CheckMemory(std::wstring wstrFileName)
{
	DWORD dwFileSize = BM::GetFileSize(wstrFileName);
	return CheckMemory(dwFileSize);
}
void PgMMCManager::Locked_SetConfigDirectory()
{
	BM::CAutoMutex kLock( m_kMutex );

	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	strPath = strPath.substr(0, strPath.rfind(_T('\\')));
	strPath = strPath + L"\\MMC\\Patch\\Config\\";
	ConfigDir(strPath);
}