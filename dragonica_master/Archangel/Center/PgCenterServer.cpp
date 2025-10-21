#include "stdafx.h"
#include "CSMail/SendMail.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/PgDBCache.h"
#include "Variant/PgEventview.h"
#include "Variant/PgMCtrl.h"
#include "PgKeyEvent.h"
#include "PgRecvFromServer.h"
#include "PgRecvFromItem.h"
#include "PgRecvFromLog.h"
#include "PgLChannelMgr.h"
#include "PgTimer.h"
#include "PgEventDoc.h"
#include "PgRecvFromContents.h"
#include "PgRecvFromManagementServer.h"

void CALLBACK OnRegist( CEL::SRegistResult const &rkArg )
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("OnRegist Failed Type[") << rkArg.eType << _T("] Ret[") << rkArg.iRet << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Not Success Result"));
		return;
	}

	if( CEL::RT_CONNECTOR == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case CEL::ST_LOG:
			{
				g_kProcessCfg.LogConnector(rkArg.guidObj);
			}break;
		case CEL::ST_CONTENTS:
			{
				g_kProcessCfg.ContentsConnector(rkArg.guidObj);
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("============ Connector!!!! ======================") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			}break;
		}
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case 1:
			{
//				if(LoadDB())
				{
					CEL::REGIST_TIMER_DESC kInit1s;
					kInit1s.pFunc		= Timer1s;
					kInit1s.dwInterval	= 1000;

					CEL::REGIST_TIMER_DESC kInitCounter;	// БўјУАЪјц ГјЕ©
					kInitCounter.pFunc		= Timer30s;
					kInitCounter.dwInterval	= 1000 * 30;		// 30ГК
			
					CEL::REGIST_TIMER_DESC kInit1m;
					kInit1m.pFunc		= Timer1m;
					kInit1m.dwInterval	= 60000;

					CEL::REGIST_TIMER_DESC kInitShutDown;
					kInitShutDown.pFunc		= TimerCheckShutDown;
					kInitShutDown.dwInterval= 10000;

					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit1s);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit1m);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInitCounter);
					g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInitShutDown);

					if ( g_kProcessCfg.IsPublicChannel() )
					{
						CEL::REGIST_TIMER_DESC kInitPvP;
						kInitPvP.pFunc		= TimerPvP;
						kInitPvP.dwInterval	= 3000;
						g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInitPvP);
					}

					CEL::INIT_FINAL_SIGNAL kInitFinal;
					kInitFinal.kIdentity = 2;
					g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

					INFO_LOG( BM::LOG_LV6, __FL__ << _T("Ready to Service Start") );
				}
			}break;
		case 2:
			{
				if(g_kCoreCenter.SvcStart())
				{
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );
					INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );
					g_kProcessCfg.Locked_ConnectContents();
					g_kCenterTask.StartSvc();
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("============ CoreCenter Cannot Run!!!! ======================") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoreCenter Cannot Run"));
				}
			}break;
		}
	}
}

HRESULT ReadConfigFile(LPCTSTR lpszFileName)
{
	BM::vstring kOldCategory(L"CHANNEL_");
	kOldCategory += g_kProcessCfg.ChannelNo();
	BM::vstring kCategory(L"R");
	kCategory += g_kProcessCfg.RealmNo();
	kCategory += L"_";
	kCategory += kOldCategory;
	
	size_t iValue = (size_t)GetPrivateProfileInt( kOldCategory.operator const wchar_t *(), _T("MAX_USER_COUNT"), 0, lpszFileName);
	if(0 == iValue)
	{
		iValue = (size_t)GetPrivateProfileInt( kCategory.operator const wchar_t *(), _T("MAX_USER_COUNT"), 500, lpszFileName);
	}
	SetMaxUser(iValue);
	
	INFO_LOG( BM::LOG_LV7, __FL__ << _T("USER_COUNT_MAX_NUM_CHANNEL : [") << iValue << _T("]") );
	return S_OK;
}

void GlobalInit()
{
	g_kTerminateFunc = OnTerminateServer;//Init MCtrl
	g_kGetMaxUserFunc = GetMaxUser;
	g_kGetNowUserFunc = GetConnectionUser;
	g_kSetMaxUserFunc = SetMaxUser;

#ifdef _MEMORY_TRACKING
	g_pkMemoryTrack = new PgMemoryTrack();
	g_pkMemoryTrack->Init(BM::OUTPUT_FILE_AND_CONSOLE, _T("./MemoryTrack"), _T("MemoryTrack.txt"));
#endif

	if( !g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Center_constant.ini")) )
	{
		ASSERT_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't load Center_constant.ini file") );
	}
}

void GlobalRelease()
{
#ifdef _MEMORY_TRACKING
	PgMemoryTrack* pkTrack = g_pkMemoryTrack;
	g_pkMemoryTrack = NULL;
	delete pkTrack;
#endif
}

int CALLBACK ExceptionTerminate(void)
{
	g_kLogWorker.VDeactivate();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_kCoreCenter;//АОЅєЕПЅє »эјє.

#ifndef _DEBUG
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );
#endif

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout<< "local.ini" << std::endl;
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_CENTER);
	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_CENTER%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL);

#ifndef _DEBUG
		INFO_LOG_LEVEL(BM::LOG_LV7);
#endif
	}

	GlobalInit();

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG( BM::LOG_LV6, _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV6, _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV6, _T("File Version : ") << chFileVersion );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );

	SERVER_IDENTITY const &rkSI = g_kProcessCfg.ServerIdentity();
	
	CEL::INIT_CENTER_DESC kCenterInit;

	kCenterInit.eOT = BM::OUTPUT_ALL;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.dwProactorThreadCount = 10;//АЇАъ ј­єсЅє 
	kCenterInit.m_kCelLogFolder = kLogFolder;
	g_kCoreCenter.Init(kCenterInit);

	CEL::INIT_CORE_DESC kLogConnectorInit;
	kLogConnectorInit.OnSessionOpen	= OnConnectToLog;
	kLogConnectorInit.OnDisconnect	= OnDisconnectFromLog;
	kLogConnectorInit.OnRecv			= OnRecvFromLog;
	kLogConnectorInit.kOrderGuid.Generate();
	kLogConnectorInit.IdentityValue(CEL::ST_LOG);
	kLogConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

	CONT_DB_INIT_DESC kContDBInit;

	std::wstring kPatch = g_kProcessCfg.ConfigDir() + _T("Center_Config.ini");

	if(FAILED(ReadConfigFile(kPatch.c_str())))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	CEL::INIT_CORE_DESC kContentsConnectorInit;
	kContentsConnectorInit.OnSessionOpen	= OnConnectFromContents;
	kContentsConnectorInit.OnDisconnect	= OnDisconnectFromContents;
	kContentsConnectorInit.OnRecv		= OnRecvFromContents;
	kContentsConnectorInit.kOrderGuid.Generate();
	kContentsConnectorInit.IdentityValue(CEL::ST_CONTENTS);
	kContentsConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLogConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kContentsConnectorInit);

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 1;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	if( !RegistKeyEvent() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0; 
	}

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "-- Started --" << std::endl;
/*
	//IBGameInterface ГК±вИ­
	if(g_kLocal.IsAbleNation(LOCAL_MGR::NC_KOREA))
	{
		g_kIBGameMgr.InitIBinterface();
	}
*/
	g_kConsoleCommander.MainLoof();

	g_kCenterTask.Close();
	// ј­єсЅє Бѕ·б
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
	GlobalRelease();

	return 0;
}

