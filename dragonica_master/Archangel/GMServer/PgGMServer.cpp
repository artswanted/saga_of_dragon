#include "stdafx.h"
#include "Variant/GM_const.h"
#include "Variant/PgMCtrl.h"
#include "Variant/PgEventView.h"
#include "PgKeyEvent.h"
#include "PgRecvFromImmigration.h"
#include "PgRecvFromLogin.h"
#include "PgRecvFromGMTool.h"
//#include "PgRecvFromCenter.h"
#include "PgDBProcess.h"

//#include "PgRecvFromUser.h"
//#include "PgGMUserMgr.h"
#include "PgGMTask.h"
#include "PgPetitionDataMgr.h"
#include "PgPetitionDataMgr.h"
#include "PgNoticeMgr.h"


std::wstring g_GMTool_IP;	//GMTool서버의 IP
WORD g_GMTool_Port = 0;		//GMTool서버가 접속할 GMServer가 오픈해주는 포트

void CALLBACK Timer1s(DWORD dwUserData)
{
	g_kNoticeMgr.CheckTime();
}


void CALLBACK PetitionTimer(DWORD dwUserData)
{
	g_kPetitionDataMgr.ReflashAllPetitionData();
}

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg)
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T(" OnRegist Failed") );
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
		case CEL::ST_LOGIN:
			{
				g_kProcessCfg.LoginConnector(rkArg.guidObj);
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ Connector!!!! ======================") );
//				g_kRealmMgr.Connector(rkArg.guidOrder);
//				g_kRealmMgr.ConnectAllCenter(rkArg.guidObj);
			}break;
		}
	} 

	if( CEL::RT_ACCEPTOR == rkArg.eType )
	{
/*		if(g_kGMUserMgr.ConnectorRegistOrder() == rkArg.guidOrder)
		{
			g_kGMUserMgr.Connector(rkArg.guidObj);
		}*/
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{//추가 등록 완료
		g_kGMTask.VInit(3);
		g_kGMTask.VActivate();
		if(g_kCoreCenter.SvcStart())
		{
			INFO_LOG( BM::LOG_LV6, _T("==================================================") );
			INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
			INFO_LOG( BM::LOG_LV6, _T("==================================================") );
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
		}
	}
}

void GlobalInit()
{
	g_kTerminateFunc = OnTerminateServer;//Init MCtrl
}


bool ReadConfigFile()
{
	std::wstring const kPatch = g_kProcessCfg.ConfigDir() + L"GM_Config.ini";

	if(!PathFileExists(kPatch.c_str()) )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Gm_Config.ini isn't exist") );
		return false;
	}

	//OrderDB부터 셋팅
	std::wstring kMain = L"ORDER_DB";
	g_kGMTask.iReflashTime(GetPrivateProfileInt(kMain.c_str(), _T("Reflash_Time"), 0, kPatch.c_str()) * 1000);

	//Petition 셋팅
	kMain = L"PETITION";
	g_kPetitionDataMgr.iReflashTime(GetPrivateProfileInt(kMain.c_str(), _T("Reflash_Time"), 0, kPatch.c_str()) * 1000);
	g_kPetitionDataMgr.iMaxLoadData(GetPrivateProfileInt(kMain.c_str(), _T("Max_Data"), 0, kPatch.c_str()));
	g_kPetitionDataMgr.bCheckReceip(GetPrivateProfileInt(kMain.c_str(), _T("Check_Receip"), 0, kPatch.c_str()));

	//GM Tool 셋팅
	TCHAR wszIP[MAX_PATH] = {0,};
	::GetPrivateProfileString( _T("GM_TOOL"), _T("TOOL_IP"), _T("0.0.0.0"), wszIP, MAX_PATH, kPatch.c_str() );
	g_GMTool_IP = wszIP;
	g_GMTool_Port = static_cast<WORD>( ::GetPrivateProfileInt( _T("GM_TOOL"), _T("ACCEPT_PORT"), 0, kPatch.c_str() ) );

		//LoadServer Time
	g_kEventView.ReadConfigFile(kPatch.c_str());

	return true;
}

int CALLBACK ExceptionTerminate(void)
{
	g_kLogWorker.VDeactivate();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	g_kCoreCenter;//인스턴스 생성.

#ifndef _DEBUG
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );
#endif

	GlobalInit();

	::CoInitialize(NULL);

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout << "Load From \"Config/local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}

	//Ini Load
	if(!ReadConfigFile())
	{
		return 0;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_GMSERVER);
	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		return 0;
	}
	
	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_GM%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL);

#ifndef _DEBUG
		if ( !(CProcessConfig::E_RunMode_Debug & g_kProcessCfg.RunMode()) )
		{
			INFO_LOG_LEVEL(BM::LOG_LV7);
		}
#endif
	}

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG( BM::LOG_LV7, _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV7, _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV7, _T("File Version : ") << chFileVersion );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );

	CEL::INIT_CENTER_DESC kGmSeverInit;
	kGmSeverInit.eOT = BM::OUTPUT_ALL;
	kGmSeverInit.pOnRegist = OnRegist;
	kGmSeverInit.bIsUseDBWorker = true;
	g_kCoreCenter.Init(kGmSeverInit);


	//DB Init
	CONT_DB_INIT_DESC kContDBInit;

	if(g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_ReadIni)
	{
		if(!ReadDBConfig( EConfigType_Ini, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.ini\" Error!!") );
			system("pause");
			return 0;
		}
	}
	else
	{
		if(!ReadDBConfig( EConfigType_Inb, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.ini\" Error!!") );
			system("pause");
			return 0;
		}
	}
	
	CONT_DB_INIT_DESC::iterator dbinit_itor = kContDBInit.begin();
	while(dbinit_itor != kContDBInit.end())
	{
		CEL::INIT_DB_DESC &kDBInit = (*dbinit_itor);
		
		kDBInit.bUseConsoleLog = true;
		kDBInit.OnDBExecute = OnDB_EXECUTE;
		kDBInit.OnDBExecuteTran = OnDB_EXECUTE_TRAN;
		kDBInit.dwWorkerCount = 1;
		kDBInit.m_kLogFolder = kLogFolder;

		g_kCoreCenter.Regist(CEL::RT_DB_WORKER, &kDBInit);
		++dbinit_itor;
	}

	//Connect from ImmigrationServer
	CEL::INIT_CORE_DESC kImmConnectorInit;
	kImmConnectorInit.OnSessionOpen		= OnConnectToImmigration;
	kImmConnectorInit.OnDisconnect		= OnDisConnectToImmigration;
	kImmConnectorInit.OnRecv			= OnRecvFromImmigration;
	kImmConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);
	kImmConnectorInit.IdentityValue(CEL::ST_IMMIGRATION);

	//Connect from Login
	CEL::INIT_CORE_DESC kLoginConnectorInit;
	kLoginConnectorInit.OnSessionOpen	= OnConnectToLogin;
	kLoginConnectorInit.OnDisconnect	= OnDisConnectToLogin;
	kLoginConnectorInit.OnRecv			= OnRecvFromLogin;
	kLoginConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);
	kLoginConnectorInit.IdentityValue(CEL::ST_LOGIN);

	//
	CEL::INIT_CORE_DESC kGMToolInit;
	kGMToolInit.kBindAddr.Set(L"", g_GMTool_Port);
	kGMToolInit.kNATAddr = kGMToolInit.kBindAddr;
	kGMToolInit.OnSessionOpen	= OnAcceptFromGMTool;
	kGMToolInit.OnDisconnect	= OnDisConnectToGMTool;
	kGMToolInit.OnRecv			= OnRecvFromGMTool;
	kGMToolInit.bIsImmidiateActivate	= true;
	kGMToolInit.ServiceHandlerType(CEL::SHT_SERVER);
	kGMToolInit.IdentityValue(CEL::ST_NONE);

	CEL::REGIST_TIMER_DESC kTimerInit_1s;
	kTimerInit_1s.pFunc		= Timer1s;
	kTimerInit_1s.dwInterval	= 1000;
	kTimerInit_1s.kOrderGuid.Generate();
	
	CEL::REGIST_TIMER_DESC kTimerInit_Petition;
	kTimerInit_Petition.pFunc		= PetitionTimer;
	kTimerInit_Petition.dwInterval	= g_kPetitionDataMgr.iReflashTime();
	kTimerInit_Petition.kOrderGuid.Generate();
	
	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 0;

	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit_1s);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit_Petition);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kImmConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLoginConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kGMToolInit);
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	if(!RegistKeyEvent()){ return 0; }

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "== Start Server Process ==" << std::endl;
	g_kConsoleCommander.MainLoof();

	g_kGMTask.VDeactivate();
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
	::CoUninitialize();

	return 0;
}
