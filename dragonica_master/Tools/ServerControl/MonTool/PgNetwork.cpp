#include "StdAfx.h"
#include "PgRecvFromManagementServer.h"
#include "PgServerStateDoc.h"
#include "PgNetwork.h"
#include "PgTimer.h"
#include "PgMCTTask.h"

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg);

PgNetwork::PgNetwork(void)
{
	IsConnect(false);
	IsServiceStart(false);
	Initialize();//켜자마자 초기화.
}

PgNetwork::~PgNetwork(void)
{
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
}

int CALLBACK ExceptionTerminate(void)
{
	g_kLogWorker.VDeactivate();
	return 0;
}

bool PgNetwork::Initialize()
{
	//인스턴스 생성
	g_kCoreCenter;
	g_kTask;

	//미니덤프 셋팅
	PgExceptionFilter::Install( PgExceptionFilter::DUMP_LEVEL_HEAVY, PgExceptionFilter::DUMP_OP_Exit_Program|PgExceptionFilter::DUMP_OP_UseTimeInDumpFile, ExceptionTerminate );

	//config를 읽어온다
	ReadConfigFile();

	//로그 셋팅
	std::wstring kLogFolder;
	InitLog(kLogFolder, L"ServerToolLog", LogOutputType(), LogLevelLimit(), std::wstring(L"LogFiles/") );

	g_kProcessCfg.Locked_SetServerType(CEL::ST_MACHINE_CONTROL);
	
	CEL::INIT_CENTER_DESC kManagementServerInit;
	kManagementServerInit.eOT = LogOutputType();
	kManagementServerInit.pOnRegist = OnRegist;

	g_kCoreCenter.Init(kManagementServerInit);

	//커넥터.
	CEL::INIT_CORE_DESC kInit;
	kInit.kBindAddr.Set(g_kGameServerMgr.MMC_IP(), g_kGameServerMgr.MMC_PORT());
	kInit.OnSessionOpen	= OnConnectToMMC;
	kInit.OnDisconnect	= OnDisconnectFromMMC;
	kInit.OnRecv		= OnRecvFromMMC;
//	kInit.dwKeepAliveTime	= 600000;
	kInit.ServiceHandlerType(CEL::SHT_SERVER);
	ServerRegistGuid(kInit.kOrderGuid);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kInit);

	//타이머
 	CEL::REGIST_TIMER_DESC kTimer;
 	kTimer.pFunc = Timer;
 	kTimer.dwInterval = 1000;
 	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimer);


	CEL::INIT_FINAL_SIGNAL kFinalInit;
	kFinalInit.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kFinalInit);
	return true;
}

bool PgNetwork::Terminate()
{
	DisConnectServer();
	return true;
}

bool PgNetwork::Send(const BM::CPacket& rkPacket)
{
	return g_kCoreCenter.Send(SessionKey(), rkPacket);
}

bool PgNetwork::TryConnectServer()
{
	if ( IsServiceStart() )
	{
		if( IsConnect() )
		{
			INFO_LOG( BM::LOG_LV8, _T("Re login error") );
			return false;
		}
		else
		{
			CEL::ADDR_INFO kAddr;
			kAddr.Set(g_kGameServerMgr.MMC_IP(), g_kGameServerMgr.MMC_PORT());
			return g_kCoreCenter.Connect( kAddr, m_kSessionKey);
		}
	}
	
	return false;
}

bool PgNetwork::DisConnectServer()
{
	// 서비스 종료
	g_kCoreCenter.Terminate(SessionKey());
	return true;
}

void CALLBACK OnRegist(const CEL::SRegistResult &rkArg)//쓰레드가 정상 등록되면 CallBack
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("OnRegist Failed") );
	}

	if( CEL::RT_CONNECTOR == rkArg.eType )
	{
		INFO_LOG(BM::LOG_LV8, __FL__ << _T("RT_CONNECTOR") );
		CEL::SESSION_KEY kSessionKey(rkArg.guidObj, BM::GUID::NullData());
		g_kNetwork.SessionKey(kSessionKey);
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{
		if(g_kCoreCenter.SvcStart())
		{
			g_kTask.StartSvc(1);//! 다운로드를 위한 Task

			g_kNetwork.IsServiceStart(true);
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
			INFO_LOG(BM::LOG_LV6, _T("============ CoreCenter Run ======================"));
			INFO_LOG(BM::LOG_LV6, _T("=================================================="));
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================"));
		}
	}
}

bool PgNetwork::ReadConfigFile()
{
	std::wstring const wstrTerminal = _T("MMC");
	std::wstring const wstrRecord = _T("RECORD");
	std::wstring const wstrLog = _T("LOG");

	TCHAR chFile[MAX_PATH] = {0,};
	_tcscat_s(chFile, MAX_PATH, _T("./servermon_config.ini"));

	if( !g_kLocal.LoadFromINI( chFile ) )
	{
		MessageBox( NULL, _T("Can't Open servermon_config.ini"), _T("ERROR"), MB_OK );
		exit(0);
		return false;
	}


	TCHAR chValue[100];
	if (GetPrivateProfileString(wstrTerminal.c_str(), NULL, NULL, chValue, 100, chFile) > 0)
	{
		if (GetPrivateProfileString(wstrTerminal.c_str(), _T("MMC_IP"), NULL, chValue, 100, chFile) > 0)
		{
			g_kGameServerMgr.MMC_IP( chValue );
		}
		g_kGameServerMgr.MMC_PORT( static_cast<WORD>(::GetPrivateProfileInt(wstrTerminal.c_str(), _T("MMC_PORT"), 0, chFile)) );
	}

	if ( GetPrivateProfileString(wstrRecord.c_str(), NULL, NULL, chValue, 100, chFile) > 0 )
	{
		if ( GetPrivateProfileString(wstrRecord.c_str(), _T("PATH"), NULL, chValue, 100, chFile) > 0 )
		{
			g_kGameServerMgr.RecordPath( chValue );
		}
		
		WORD const wCCUTime = __min( static_cast<WORD>( ::GetPrivateProfileInt(wstrRecord.c_str(), _T("CCU_TIME"), 0, chFile) ), 60 );
		g_kGameServerMgr.RecordCCUTimeMin( wCCUTime );
	}

	if ( GetPrivateProfileString(wstrLog.c_str(), NULL, NULL, chValue, 100, chFile) > 0 )
	{
		int iValue = GetPrivateProfileInt(wstrLog.c_str(), _T("LOG_OUTPUT_TYPE"), BM::OUTPUT_FILE_AND_TRACE, chFile);
		LogOutputType(static_cast<BM::E_OUPUT_TYPE>(iValue));

		iValue = GetPrivateProfileInt(wstrLog.c_str(), _T("LOG_LEVEL_LIMIT"), BM::LOG_LV9, chFile);
		LogLevelLimit(static_cast<BM::E_LOG_LEVEL>(iValue));
	}


	return true;
}