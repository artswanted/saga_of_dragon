#include "stdafx.h"
#include "CSMail/SendMail.h"
#include "CEL/Cel_Log.h"
#include "Lohengrin/PgRealmManager.h"
#include "HellDart/PgIPChecker.h"
#include "Lohengrin/VariableContainer.h"
#include "FCS/AntiHack.h"
#include "variant/PgMCtrl.h"
#include "Variant/PgEventView.h"
#include "constant.h"
#include "PgRecvFromLog.h"
#include "PgRecvFromServer.h"
#include "PgKeyEvent.h"
#include "PgTimer.h"
#include "PgRecvFromMMC.h"
#include "PgWaitingLobby.h"
#include "ImmTask.h"
#include "PgSiteMgr.h"

extern HRESULT CALLBACK OnDB_EXECUTE(CEL::DB_RESULT &rkResult);
extern HRESULT CALLBACK OnDB_EXECUTE_TRAN(CEL::DB_RESULT_TRAN &rkResult);
extern bool LoadDB();

PgIPChecker g_kIPChecker;

#define MAIN_LOG2(a,b)	SDebugLogMessage kMsg(1, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);

int GetConnectionUserCount()
{
	return static_cast<int>(g_kWaitingLobby.Locked_GetLoginUserCount());
}

void GlobalInit()
{
	g_kTerminateFunc = OnTerminateServer;//Init MCtrl
	g_kGetNowUserFunc = GetConnectionUserCount;
	g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Immigration_constant.ini"));
}

void CALLBACK OnRegist( CEL::SRegistResult const &rkArg )
{
	INFO_LOG( BM::LOG_LV7, __FL__ << _T(" OnRegist Call Type[") << rkArg.eType << _T("] Ret[") << rkArg.iRet << _T("]") );

	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__<< rkArg.eType << _COMMA_ << rkArg.iRet );
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" OnRegist Failed Type[") << rkArg.eType << _T("] Ret[") << rkArg.iRet << _T("]") );
		return;
	}

	if( CEL::RT_CONNECTOR == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case CEL::ST_LOG:
			{
				g_kProcessCfg.LogConnector(rkArg.guidObj);
				g_kProcessCfg.Locked_SetLogServer();
				g_kProcessCfg.Locked_ConnectLog();
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ Connector!!!! ======================") );
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__<< rkArg.eType << _COMMA_ << rkArg.iRet );
				g_kConsoleCommander.StopSignal(true);
			}break;
		}
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		switch(rkArg.iIdentityValue)
		{
		case 1:
			{
				if(LoadDB())
				{
					CEL::INIT_FINAL_SIGNAL kInit;
					kInit.kIdentity = 2;
					g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInit);
				}
				else
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__<< rkArg.eType << _COMMA_ << rkArg.iRet );
					g_kConsoleCommander.StopSignal(true);
					return;
				}
			}break;
		case 2:
			{
				if(g_kCoreCenter.SvcStart())
				{
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );
					INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
					INFO_LOG( BM::LOG_LV6, _T("==================================================") );
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__<< rkArg.eType << _COMMA_ << rkArg.iRet );
					g_kConsoleCommander.StopSignal(true);
				}
			}break;
		}
	}
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

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout << "Load From \"Config/local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}
	
	//! 서버 세션 컨트롤 되도록 작성 해야함. (세션 매니저가 없다는 소리)

	std::wstring kLogFolder;
	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_IMM%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL);

#ifndef _DEBUG
		INFO_LOG_LEVEL(BM::LOG_LV7);
#endif
	}
	
	GlobalInit();

	TCHAR chProductVersion[80]={0,}, chFileVersion[80]={0,};
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	g_kProcessCfg.Locked_SetServerType(CEL::ST_IMMIGRATION);
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("File Version : ") << chFileVersion );	
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );
	
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

	g_kEventView.ReadConfigFile(NULL);

	CS_MAIL::CSendMail kSend;
	if(kSend.Init(g_kProcessCfg.ConfigDir() + L"MailConfig.ini"))
	{
		std::wstring wstrMailBody;

		TCHAR szMailMessage[1000] = {0,};
		wsprintf(szMailMessage, _T("==================== Server Initialize Start ====================\n"));	wstrMailBody+=szMailMessage;
		
		wsprintf(szMailMessage, _T("Realm[%d], Channel[%d] Type[%d] No[%d]\n"), 
			g_kProcessCfg.ServerIdentity().nRealm,
			g_kProcessCfg.ServerIdentity().nChannel,
			g_kProcessCfg.ServerIdentity().nServerType,
			g_kProcessCfg.ServerIdentity().nServerNo);
			wstrMailBody+=szMailMessage;

		wsprintf(szMailMessage, _T("Product Version : %s\n"), chProductVersion);							wstrMailBody+=szMailMessage;
		wsprintf(szMailMessage, _T("File Version : %s\n"), chFileVersion);									wstrMailBody+=szMailMessage;
		wsprintf(szMailMessage, _T("Packet Version : C-%s/S-%s\n"), PACKET_VERSION_C, PACKET_VERSION_S);	wstrMailBody+=szMailMessage;

		kSend.SendAuto(_T("ServerOpen"), wstrMailBody);

		INFO_LOG( BM::LOG_LV7, wstrMailBody.c_str() );
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("FileName NULL"));
	}

	CEL::INIT_CENTER_DESC kCenterInit;

	kCenterInit.eOT = BM::OUTPUT_ALL;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.bIsUseDBWorker = true;
	kCenterInit.dwProactorThreadCount = __min(kCenterInit.dwProactorThreadCount, 5);//컨텐츠 N + 로그인서버 N
	kCenterInit.m_kCelLogFolder = kLogFolder;
	// DB 서비스를 시작한다.
	g_kCoreCenter.Init(kCenterInit);
	
	CEL::REGIST_TIMER_DESC kTimerInit;
	kTimerInit.pFunc		= Timer_10s;
	kTimerInit.dwInterval	= 10000;//10sec

	CEL::REGIST_TIMER_DESC kTimerInit_RefreshSiteInfo;
	kTimerInit_RefreshSiteInfo.pFunc		= Timer20s;
	kTimerInit_RefreshSiteInfo.dwInterval	= 20000;//20s

	CONT_DB_INIT_DESC kContDBInit;

	if(g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_ReadIni)
	{
		if(!ReadDBConfig( EConfigType_Ini, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.ini\" Error!!") );
			LIVE_CHECK_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.ini\" Error!!") );
			system("pause");
			return 0;
		}
	}
	else
	{
		if(!ReadDBConfig( EConfigType_Inb, g_kProcessCfg.ConfigDir(), g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.inb\" Error!!") );
			LIVE_CHECK_LOG( BM::LOG_LV0, _T("Load From \"Config/DB_Config.inb\" Error!!") );
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

	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit_RefreshSiteInfo);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit);

	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 1;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	g_kImmTask.StartSvc(4);

	DWORD const dwServiceRegion = g_kLocal.ServiceRegion();
	switch( dwServiceRegion )
	{
	case LOCAL_MGR::NC_RUSSIA:
		{
			if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DEVELOP) )
			{
				g_kWaitingLobby.UseJoinSite(true);
			}
		}break;
	case LOCAL_MGR::NC_SINGAPORE:
	case LOCAL_MGR::NC_THAILAND:
	case LOCAL_MGR::NC_INDONESIA:
	case LOCAL_MGR::NC_VIETNAM:
	case LOCAL_MGR::NC_CHINA:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_FRANCE:
	case LOCAL_MGR::NC_GERMANY:
	case LOCAL_MGR::NC_PHILIPPINES:
	case LOCAL_MGR::NC_USA:
	case LOCAL_MGR::NC_TAIWAN:
		{
			// 대만은 ExtVar 사용 and now we use ExtVar for allow login beta user
			short nExtVar = 0;
			g_kVariableContainer.Get(EVar_Kind_Login, EVAR_Login_Check_HaveExtVarValue, nExtVar );//
			g_kWaitingLobby.UseExtVar(nExtVar != EXT_VAR_NONE);
			g_kWaitingLobby.HaveExtVar(nExtVar);
			// 중복로그인시에 유저를 끊어버릴라면 여기 주석을 풀어라
			g_kWaitingLobby.IsNewTryLoginDisconnectAll(true);
		}break;
	}

	if( !RegistKeyEvent() )
	{
		LIVE_CHECK_LOG( BM::LOG_LV0, _T("RegistKeyEvent() Error!!") );
		return 0; 
	}

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "-- Started --" << std::endl;
	g_kConsoleCommander.MainLoof();

	// 서비스 종료
	g_kImmTask.Close();
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();

	return 0;
}

