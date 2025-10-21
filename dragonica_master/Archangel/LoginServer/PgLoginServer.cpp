#include "stdafx.h"
#include "Variant/PgMCtrl.h"
#include "PgKeyEvent.h"
#include "PgRecvFromImmigration.h"
#include "PgRecvFromUser.h"
#include "PgLoginUserMgr.h"
#include "PgRecvFromLog.h"
#include "PgPatchVersionManager.h"
#include "Variant/PgCheckMacAddress.h"

void CALLBACK Timer2s(DWORD dwUserData)
{
}

void CALLBACK Timer_10s(DWORD dwUserData)
{
	g_kLoginUserMgr.Tick();
}

void CALLBACK Timer_30s(DWORD dwUserData)
{
}

void CALLBACK OnRegist( CEL::SRegistResult const &rkArg )
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << L"OnRegist Failed Type[" << rkArg.eType << L"] Ret[" << rkArg.iRet << L"]");
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
		case CEL::ST_IMMIGRATION:
			{
				g_kProcessCfg.ImmigrationConnector(rkArg.guidObj);
				g_kProcessCfg.Locked_ConnectImmigration();
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("============ Connector!!!! ======================"));
				LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
			}break;
		}
	} 

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{//추가 등록 완료
		if(g_kCoreCenter.SvcStart())
		{
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
			INFO_LOG(BM::LOG_LV6, _T("============ CoreCenter Run ======================"));
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================"));
			LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		}
	}
}

int GetTryLoginUserCount()
{
	return static_cast<int>(g_kLoginUserMgr.GetTryLoginUserCount());
}

HRESULT ReadConfigFile(LPCTSTR lpszFileName)
{
	if(NULL == lpszFileName)
	{
		return S_FALSE;
	}

	int const iValue = static_cast<int>(GetPrivateProfileInt(_T("LOGIN"), _T("VersionCheckType"), 0, lpszFileName));
	g_kPatchVersionMgr.SetCheckVersion(iValue);
	std::string kType;
	if(1==iValue)
	{
		kType = "Server <= Client(small and equal), Join OK";
	}
	else
	{
		kType = "Server == Client(equal), Join OK";
	}

	std::cout<< "VersionCheckType : " << kType << std::endl;
	//INFO_LOG(BM::LOG_LV6, _T("VersionCheckType : ") << kType);	
	return S_OK;
}

void GlobalInit(bool bReload)
{
	if( !bReload )
	{
		g_kTerminateFunc = OnTerminateServer;//Init MCtrl
		g_kGetNowUserFunc = GetTryLoginUserCount;
	}



	std::wstring kPatch = g_kProcessCfg.ConfigDir() + _T("Login_Config.ini");
	ReadConfigFile(kPatch.c_str());
}

int CALLBACK ExceptionTerminate(void)
{
	g_kLogWorker.VDeactivate();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_kCoreCenter;//인스턴스 생성.

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout << "Load From \"Config/local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}

#ifndef _DEBUG
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );
#endif

	GlobalInit(false);
	g_kProcessCfg.Locked_SetServerType(CEL::ST_LOGIN);
	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		BM::vstring arg;
		for ( int i = 1; i< argc ; ++i )
		{
			arg += argv[i];
			arg += _T(" ");
		}
		CAUTION_LOG( BM::LOG_LV0, _T("Argment Error! ") << arg );
		system("pause");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_LOGIN%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog( kLogFolder, chLog, BM::OUTPUT_ALL );

#ifndef _DEBUG
		INFO_LOG_LEVEL(BM::LOG_LV6);
#endif
	}

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG(BM::LOG_LV6, _T("==================== Server Initialize Start ===================="));
	INFO_LOG(BM::LOG_LV6, _T("Product Version : ") << std::wstring(chProductVersion));
	INFO_LOG(BM::LOG_LV6, _T("File Version : ") << std::wstring(chFileVersion));
	INFO_LOG( BM::LOG_LV6, _T("Packet Version C : ") << PACKET_VERSION_C );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );
{
	CEL::INIT_CENTER_DESC kCenterInit;
	kCenterInit.eOT = BM::OUTPUT_ALL;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.dwProactorThreadCount = __min(kCenterInit.dwProactorThreadCount, 5);//유저 서비스 
	kCenterInit.m_kCelLogFolder = kLogFolder;
	g_kCoreCenter.Init(kCenterInit);
}

	CEL::REGIST_TIMER_DESC kInit;
	kInit.pFunc				= Timer_10s;
	kInit.dwInterval		= 10000;//10sec

	CEL::INIT_CORE_DESC kLogConnectorInit;
	kLogConnectorInit.OnSessionOpen	= OnConnectToLog;
	kLogConnectorInit.OnDisconnect	= OnDisConnectToLog;
	kLogConnectorInit.OnRecv			= OnRecvFromLog;
	kLogConnectorInit.IdentityValue(CEL::ST_LOG);
	kLogConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

	CEL::INIT_CORE_DESC kImmConnectorInit;
	kImmConnectorInit.OnSessionOpen	= OnConnectToImmigration;
	kImmConnectorInit.OnDisconnect	= OnDisConnectToImmigration;
	kImmConnectorInit.OnRecv			= OnRecvFromImmigration;
	kImmConnectorInit.IdentityValue(CEL::ST_IMMIGRATION);
	kImmConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);

//	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kLogTimerInit);
//	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kLogTimerInit2);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLogConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kImmConnectorInit);

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	if(!RegistKeyEvent())
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__);
		return 0; 
	}

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "== Start Server Process ==" << std::endl;
	g_kConsoleCommander.MainLoof();

	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
	return 0;
}
