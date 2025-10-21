// MCtrlServer.cpp : Defines the entry point for the console application.
//
//GameServerMgr	(중앙이 전체 게임 서버 정보 관리용도)
//	게임 서버 정보
//	서브 들의 정보로 부터 -> On/Off 라인 판독 및 Tool에 정보 제공
//
//MCtrlServerMgr	(서브/중앙 용도의 게임서버 컨트롤러)
//	서버 프로세스 생성
//	관리
//	종료
//	긴급 종료 처리

#include "stdafx.h"

#include "PgKeyEvent.h"
#include "PgSMCMgr.h"
#include "PgRecvFromMMC.h"
#include "Tlhelp32.h"
#include "PgLogCopyer.h"
#include <curl/curl.h>
#include <telegram/telegram.h>

void CALLBACK Timer(DWORD dwUSerData)
{
	g_kSMCMgr.RefreshProcessState();
	g_kSMCMgr.NfyProcessStateToMMC();
	g_kSMCMgr.ConnectToMMC();
}

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg)//쓰레드가 정상 등록되면 CallBack
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T(" OnRegist Failed") );
	}

	if( CEL::RT_CONNECTOR == rkArg.eType )
	{
		INFO_LOG( BM::LOG_LV8, __FL__ << _T(" RT_CONNECTOR") );
		switch(rkArg.iIdentityValue)
		{
		case CEL::ST_MACHINE_CONTROL:
			{
				g_kSMCMgr.m_kMMCSessionKey.WorkerGuid(rkArg.guidObj);
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ Connector!!!! ======================") );
			}break;
		}
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		if(g_kCoreCenter.SvcStart())
		{
			INFO_LOG( BM::LOG_LV6, _T("==================================================") );
			INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
			INFO_LOG( BM::LOG_LV6, _T("==================================================") );			
			g_kSMCMgr.ConnectToMMC();//접속 시도
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
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
	curl_global_init(CURL_GLOBAL_ALL); // Init curl for telegram

#ifndef _DEBUG
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );
#endif

	if(!g_kLocal.LoadFromINI())
	{
		std::cout << "Can't Load Local.ini" << std::endl;
		system("pause");
		return 0;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_SUB_MACHINE_CONTROL);
	if( !g_kSMCMgr.CheckRunParam(argc, argv))
	{
		return 0;
	}

	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		return 0;
	}

	if ( !g_kSMCMgr.ReadFromConfig( L"./SMC.ini" ) )
	{
		std::cout << "Can't Load SMC.ini" << std::endl;
		system("pause");
		return 0;
	}

	telegram::init(g_kSMCMgr.TelegramToken().c_str());

	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_SMC%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL, BM::LOG_LV9, std::wstring(L"../LogFiles/") );
	}
	
	if(g_kSMCMgr.IsDeleteLog())
	{//! ini 셋팅으로 삭제 유무 결정
		g_kLogCopyer.Init( g_kSMCMgr.PathLog(), g_kSMCMgr.PathLogBak(), g_kSMCMgr.DeleteLogInterval() );
	}

	
	if( g_kSMCMgr.AutoPatch() )
	{
		bool bProcessStart = true;
		while(bProcessStart)
		{
			INFO_LOG( BM::LOG_LV1, _T("Wait For ProcessShutDown ... [") << g_kSMCMgr.SMCFileName() << _T("]") );
			INFO_LOG( BM::LOG_LV1, _T("Wait For ProcessShutDown ... [SMC_Release.exe]") );
			bProcessStart = g_kSMCMgr.CheckProcessShutDown(_T("SMC_Release.exe"));
			Sleep(1000);
		}
		g_kSMCMgr.StartSelfPatch();
	} 
	else
	{
		bool bProcessStart = true;
		while(bProcessStart)
		{
			INFO_LOG( BM::LOG_LV1, _T("Wait For ProcessShutDown ... [") << g_kSMCMgr.SMCFileName() << _T("]") );
			INFO_LOG( BM::LOG_LV1, _T("Wait For ProcessShutDown ... [SMC_Temp.exe]") );
			bProcessStart = g_kSMCMgr.CheckProcessShutDown(L"SMC_Temp.exe");
			Sleep(1000);
		}
	}
	
	TCHAR chProductVersion[80] = {0, }, chFileVersion[80] = {0, };
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG( BM::LOG_LV7, _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV7, _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV7, _T("File Version : ") << chFileVersion );
	INFO_LOG( BM::LOG_LV7, _T("Protocol Version : ") << PACKET_VERSION_S );

	CEL::INIT_CENTER_DESC kManagementServerInit;
	kManagementServerInit.eOT = BM::OUTPUT_ALL;
	kManagementServerInit.pOnRegist = OnRegist;
	kManagementServerInit.m_kCelLogFolder = kLogFolder;

	g_kCoreCenter.Init(kManagementServerInit);

	CEL::INIT_CORE_DESC kMMCInit;//MMC 붙는 Connector
	kMMCInit.OnSessionOpen	= OnConnectToMMC;
	kMMCInit.OnDisconnect	= OnDisConnectToMMC;
	kMMCInit.OnRecv			= OnRecvFromMMC;
	kMMCInit.IdentityValue(CEL::ST_MACHINE_CONTROL);//
	kMMCInit.ServiceHandlerType(CEL::SHT_FILE_SERVER);//SMC 는 파일 서버 커넥션을 사용

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kMMCInit);

	CEL::REGIST_TIMER_DESC kTimerInit;
	kTimerInit.pFunc		= Timer;
	kTimerInit.dwInterval	= 15000;//
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit);

	CEL::INIT_FINAL_SIGNAL kFinalInit;
	kFinalInit.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kFinalInit);

	if( !RegistKeyEvent() ){ return 0; }

	g_kProcessCfg.Locked_SetConsoleTitle();

	g_kConsoleCommander.MainLoof();

	// 서비스 종료
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();

	BM::PgDataPackManager::Clear();
	telegram::term();
	curl_global_cleanup();

	return 0;
}
