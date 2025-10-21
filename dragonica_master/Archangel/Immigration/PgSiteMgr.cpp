#include "StdAfx.h"
#include "PgSiteMgr.h"
#include "ImmTask.h"
#include "PgDBProcess.h"

extern std::wstring g_kLogFolder;

namespace ForceServerIP
{
	std::wstring kForceServerIP;
	void Init()
	{
		FILE *pFile = NULL;
		std::wstring const kFileName( g_kProcessCfg.ConfigDir() + L"ForceIP.txt" );
		errno_t const eError = ::_wfopen_s(&pFile, kFileName.c_str(), L"r");
		if( 0 == eError
		&&	NULL != pFile )
		{
			wchar_t szTemp[MAX_PATH] = {0, };
			int const iReadCount = ::fwscanf_s(pFile, L"%s", szTemp);
			if( 0 < iReadCount )
			{
				kForceServerIP.swap( std::wstring(szTemp) );
			}
			::fclose(pFile);
		}
	}
	void Check(std::wstring& rkIP)
	{
		if( !rkIP.empty()
		&&	!kForceServerIP.empty() )
		{
			rkIP = kForceServerIP;
		}
	}
};

PgSiteMgr::PgSiteMgr(void)
{
}

PgSiteMgr::~PgSiteMgr(void)
{
}

void PgSiteMgr::LoadFromDB()
{
	std::wstring const kFileName = g_kProcessCfg.ConfigDir() + L"Immigration_DB_Config.ini";

	CONT_REALM_CANDIDATE kOldRealmCandi = m_kRealmCandi;// Reload를 위해서

	{// Site Config
		TCHAR szTemp[100] = _T("TB_SiteConfig");
		::GetPrivateProfileString( _T("ETC"), _T("SERVER_TBNAME"), szTemp, szTemp, 100, kFileName.c_str() );

		CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_SITE_CONFIG, L"EXEC [dbo].[UP_LoadSiteConfig]" );
		kQuery.PushStrParam( std::wstring(szTemp) );
		g_kCoreCenter.PushQuery( kQuery, true );
	}

	CONT_REALM_CANDIDATE kNewRealmCandi;

	{// Realm Config
		CONT_REALM_CANDIDATE::const_iterator realm_candi_itr = m_kRealmCandi.begin();
		for( ; m_kRealmCandi.end() != realm_candi_itr ; ++realm_candi_itr )
		{
			CONT_REALM_CANDIDATE::const_iterator old_realm_candi_itr = kOldRealmCandi.find( realm_candi_itr->first );
			if ( old_realm_candi_itr == kOldRealmCandi.end() )
			{
				kNewRealmCandi.insert( std::make_pair( realm_candi_itr->first, realm_candi_itr->second ) );

				CEL::DB_QUERY kQuery(DT_SITE_CONFIG, DQT_LOAD_REALM_CONFIG, _T("EXEC [dbo].UP_LoadRealmConfig"));
				kQuery.PushStrParam((*realm_candi_itr).second.RealmTblName());
				kQuery.PushStrParam((*realm_candi_itr).second.RealmNo());
				kQuery.PushStrParam((*realm_candi_itr).second.RealmName());
				g_kCoreCenter.PushQuery(kQuery, true);
			}
		}
	}


	if ( kOldRealmCandi.size() )
	{
		CONT_REALM_CANDIDATE::const_iterator new_realm_candi_itr = kNewRealmCandi.begin();
		for( ; kNewRealmCandi.end() != new_realm_candi_itr ; ++new_realm_candi_itr )
		{
			{// Map Config From Static Ground
				TCHAR szTemp[100] = _T("TB_MapCfg_Static");
				::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_STATIC"), szTemp, szTemp, 100, kFileName.c_str());
				CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_STATIC, _T("EXEC [dbo].[UP_Load_MapCfg_Static]"));
				kQuery.PushStrParam( std::wstring(szTemp ));
				kQuery.PushStrParam( new_realm_candi_itr->first );
				g_kCoreCenter.PushQuery(kQuery, true);
			}

			{// Map Config From Mission
				TCHAR szTemp[100] = _T("TB_MapCfg_Mission");
				::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_MISSION"), szTemp, szTemp, 100, kFileName.c_str());
				CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_MISSION, _T("EXEC [dbo].[UP_Load_MapCfg_Mission]"));
				kQuery.PushStrParam( std::wstring(szTemp) );
				kQuery.PushStrParam( new_realm_candi_itr->first );
				g_kCoreCenter.PushQuery( kQuery, true );
			}


			{// Map Config From Public Channel's Ground
				TCHAR szTemp[100] = _T("TB_MapCfg_Public");
				::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_PUBLIC"), szTemp, szTemp, 100, kFileName.c_str());
				CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_PUBLIC, _T("EXEC [dbo].[UP_Load_MapCfg_Public]"));
				kQuery.PushStrParam( std::wstring(szTemp) );
				kQuery.PushStrParam( new_realm_candi_itr->first );
				g_kCoreCenter.PushQuery(kQuery, true);
			}
		}
	}
	else
	{
		{// Map Config From Static Ground
			TCHAR szTemp[100] = _T("TB_MapCfg_Static");
			::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_STATIC"), szTemp, szTemp, 100, kFileName.c_str());
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_STATIC, _T("EXEC [dbo].[UP_Load_MapCfg_Static]"));
			kQuery.PushStrParam( std::wstring(szTemp ));
			g_kCoreCenter.PushQuery(kQuery, true);
		}

		{// Map Config From Mission
			TCHAR szTemp[100] = _T("TB_MapCfg_Mission");
			::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_MISSION"), szTemp, szTemp, 100, kFileName.c_str());
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_MISSION, _T("EXEC [dbo].[UP_Load_MapCfg_Mission]"));
			kQuery.PushStrParam( std::wstring(szTemp) );
			g_kCoreCenter.PushQuery( kQuery, true );
		}


		{// Map Config From Public Channel's Ground
			TCHAR szTemp[100] = _T("TB_MapCfg_Public");
			::GetPrivateProfileString( _T("ETC"), _T("MAPCFG_TBNAME_PUBLIC"), szTemp, szTemp, 100, kFileName.c_str());
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_MAP_CONFIG_PUBLIC, _T("EXEC [dbo].[UP_Load_MapCfg_Public]"));
			kQuery.PushStrParam( std::wstring(szTemp) );
			g_kCoreCenter.PushQuery(kQuery, true);
		}
	}
}

HRESULT PgSiteMgr::Q_DQT_LOAD_SITE_CONFIG(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(CEL::DR_SUCCESS == rkResult.eRet 
		|| CEL::DR_NO_RESULT == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator kIter = rkResult.vecArray.begin();		
		while(rkResult.vecArray.end() != kIter)
		{
			short nRealmNo;
			std::wstring  strRealmName;
			std::wstring  strRealmTblName;
			std::wstring  strNotice_TblName;

			kIter->Pop(nRealmNo); ++kIter;
			kIter->Pop(strRealmName); ++kIter;
			kIter->Pop(strRealmTblName); ++kIter;
			kIter->Pop(strNotice_TblName); ++kIter;

			m_kRealmCandi.insert(std::make_pair(nRealmNo, tagRealmElement(nRealmNo, strRealmName, strRealmTblName, strNotice_TblName)));
		}

		return S_OK;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("None Data") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
	return E_FAIL;
}

HRESULT PgSiteMgr::Q_DQT_LOAD_REALM_CONFIG(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	ForceServerIP::Init();

	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		short nRealm = 0;
		std::wstring kRealmName = _T("");

		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor )
		{
			(*itor).Pop( nRealm );	++itor;
			(*itor).Pop( kRealmName );	++itor;
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
		}

		if( rkResult.vecArray.end() == itor )
		{//렐름 테이블 명을 잘못 기입.
			VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" TblNameWrong") );
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("TblNameWrong"));
			return E_FAIL;
		}
		else
		{
			while( rkResult.vecArray.end() != itor )
			{
				CONT_SERVER_HASH::mapped_type element;

				std::wstring kName = _T("");
				std::wstring kMachineIP = _T("");
				std::wstring kBindIP = _T("");
				std::wstring kNATIP = _T("");
				unsigned short usBindPort = 0;
				unsigned short usNatPort = 0;

				element.nRealm = nRealm;
				(*itor).Pop( element.nChannel );	++itor;
				(*itor).Pop( element.nServerNo );	++itor;
				(*itor).Pop( element.nServerType );	++itor;
				(*itor).Pop( kMachineIP );			++itor;
				(*itor).Pop( kName );				++itor;

				(*itor).Pop( kBindIP );				++itor;		ForceServerIP::Check(kBindIP);
				(*itor).Pop( usBindPort );			++itor;
				(*itor).Pop( kNATIP );				++itor;		ForceServerIP::Check(kNATIP);
				(*itor).Pop( usNatPort );			++itor;

				element.strMachinIP = kMachineIP;
				element.addrServerBind.Set(kBindIP, usBindPort);
				element.addrServerNat.Set(kNATIP, usNatPort);

				kBindIP = _T("");
				kNATIP = _T("");
				usBindPort = 0;
				usNatPort = 0;

				(*itor).Pop( kBindIP );				++itor;		ForceServerIP::Check(kBindIP);
				(*itor).Pop( usBindPort );			++itor;
				(*itor).Pop( kNATIP );				++itor;		ForceServerIP::Check(kNATIP);
				(*itor).Pop( usNatPort );			++itor;

				element.addrUserBind.Set(kBindIP, usBindPort);
				element.addrUserNat.Set(kNATIP, usNatPort);
				element.strName = kName;
				element.Build();

				bool bIsSuccess = g_kProcessCfg.Locked_AddServerInfo(element);
				if(!bIsSuccess)
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Already Use ServerIdentity Realm<") << element.nRealm << _T("> Channel<") << element.nChannel << _T("> ServerNo<") << element.nServerNo << _T("> ServerType<") << element.nServerType << _T(">") );
					LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("Already Use ServerIdentity"));
				}
			}
			return S_OK;
		}
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" TABLE [Tbl_ServerList] is row count 0") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
	return E_FAIL;
}

HRESULT PgSiteMgr::Q_DQT_LOAD_MAP_CONFIG_STATIC( CEL::DB_RESULT &rkResult )
{
	BM::CAutoMutex kLock(m_kMutex);

	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			CONT_MAP_CONFIG::key_type element;

			(*itor).Pop( element.nRealmNo );	++itor;
			(*itor).Pop( element.nChannelNo );	++itor;
			(*itor).Pop( element.nServerNo );	++itor;
			(*itor).Pop( element.iGroundNo );	++itor;

			if( (0==element.nChannelNo) || FAILED(g_kProcessCfg.Locked_AddMapCfg(element)) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Insert Failed Realm[") << element.nRealmNo << _T("] Channel[") << element.nChannelNo << _T("] Server[") << element.nServerNo << _T("] Ground[") << element.iGroundNo << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << element.nRealmNo<< _COMMA_ << element.nChannelNo<< _COMMA_ << element.nServerNo << _COMMA_ << element.iGroundNo);
			}
		}
		return S_OK;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" TABLE row count 0") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));

	return E_FAIL;
}

HRESULT PgSiteMgr::Q_DQT_LOAD_MAP_CONFIG_MISSION( CEL::DB_RESULT &rkResult )
{
	BM::CAutoMutex kLock(m_kMutex);

	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			CONT_MAP_CONFIG::key_type element;

			(*itor).Pop( element.nRealmNo );	++itor;
			(*itor).Pop( element.nChannelNo );	++itor;
			(*itor).Pop( element.nServerNo );	++itor;
			(*itor).Pop( element.iGroundNo );	++itor;

			if( (0==element.nChannelNo) || FAILED(g_kProcessCfg.Locked_AddMapCfg_Mission(element)) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Insert Failed Realm[") << element.nRealmNo << _T("] Channel[") << element.nChannelNo << _T("] Server[") << element.nServerNo << _T("] MissionNo[") << element.iGroundNo << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << element.nRealmNo << _COMMA_ << element.nChannelNo << _COMMA_ << element.nServerNo << _COMMA_ << element.iGroundNo);
			}
		}
		return S_OK;
	}

	INFO_LOG( BM::LOG_LV5, __FL__ << _T(" TABLE row count 0") );
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
	return E_FAIL;
}

HRESULT PgSiteMgr::Q_DQT_LOAD_MAP_CONFIG_PUBLIC(CEL::DB_RESULT &rkResult)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		while( rkResult.vecArray.end() != itor )
		{
			CONT_MAP_CONFIG::key_type element;

			(*itor).Pop( element.nRealmNo );	++itor;
			//			(*itor).Pop( element.nChannelNo );	++itor;
			element.nChannelNo = 0;// 채널번호는 0으로 강제.
			(*itor).Pop( element.nServerNo );	++itor;
			(*itor).Pop( element.iGroundNo );	++itor;

			if( FAILED(g_kProcessCfg.Locked_AddMapCfg(element) ) )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" Insert Failed Realm[") << element.nRealmNo << _T("] Channel[") << element.nChannelNo << _T("] Server[") << element.nServerNo << _T("] Ground[") << element.iGroundNo << _T("]") );
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << element.nRealmNo << _COMMA_ << element.nChannelNo << _COMMA_ << element.nServerNo << _COMMA_ << element.iGroundNo);
			}
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
	return E_FAIL;
}

HRESULT PgSiteMgr::Q_DQT_PATCH_VERSION_EDIT(CEL::DB_RESULT &rkResult)
{
	if(CEL::DR_SUCCESS == rkResult.eRet)
	{
		CEL::DB_DATA_ARRAY::const_iterator itor = rkResult.vecArray.begin();
		if( rkResult.vecArray.end() != itor )
		{
			BM::Stream kPacket(PT_IMM_LOGIN_PATCH_VERSION_EDIT);

			int iValue = 0;
			(*itor).Pop(iValue); ++itor;
			kPacket.Push(iValue);
			(*itor).Pop(iValue); ++itor;
			kPacket.Push(iValue);
			(*itor).Pop(iValue); ++itor;
			kPacket.Push(iValue);

			SendToServerType(CEL::ST_LOGIN,kPacket);

			SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_ORDERSTATE);
			kMsg.Push(rkResult.QueryOwner());
			kMsg.Push(OS_DONE);
			g_kImmTask.PutMsg(kMsg);
		}
		return S_OK;
	}
	SEventMessage kMsg(EIMM_OBJ_GM, PgGMProcessMgr::EGM_2ND_ORDERSTATE);
	kMsg.Push(rkResult.QueryOwner());
	kMsg.Push(OS_RESERVE);
	g_kImmTask.PutMsg(kMsg);

	LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << _T("None Data"));
	return E_FAIL;
}
