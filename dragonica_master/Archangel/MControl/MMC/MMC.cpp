// MMC.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DataPack/Common.h"
#include "PgMMCManager.h"
#include "PgRecvFromImmigration.h"
#include "PgTimer.h"
#include "PgKeyEvent.h"
#include "PgMMCTask.h"
#include "PgFileCopyMgr.h"

void CALLBACK OnRegist(CEL::SRegistResult const &rkArg)//쓰레드가 정상 등록되면 CallBack
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		ASSERT_LOG(false, BM::LOG_LV1, __FL__ << _T("OnRegist Failed Type = ") << rkArg.eType );
		return;
	}

	switch ( rkArg.eType )
	{
	case CEL::RT_CONNECTOR:
		{
			switch(rkArg.iIdentityValue)
			{
			case CEL::ST_IMMIGRATION:
				{
					g_kMMCMgr.ImmigrationConnector( rkArg.guidObj );
				}break;
			default:
				{
					ASSERT_LOG( false, BM::LOG_LV0, L"============ Incorrect Connector!!!! ===================<" << rkArg.iIdentityValue << L">" );
				}break;
			}
		}break;
	case CEL::RT_ACCEPTOR:
		{
			switch(rkArg.iIdentityValue)
			{
			case CEL::ST_NONE://For Tool 
			case CEL::ST_SUB_MACHINE_CONTROL:// SUb Machine
				{
				}break;
			default:
				{
					ASSERT_LOG( false, BM::LOG_LV0, _T("============ Incorrect Acceptor!!!! ===================<") << rkArg.iIdentityValue << L">" );
				}break;
			}
		}break;
	case CEL::RT_DB_WORKER:
		{
			g_kMMCMgr.Locked_LoadDB( rkArg.iIdentityValue );
		}break;
	case CEL::RT_FINAL_SIGNAL:
		{
			if( true == g_kCoreCenter.SvcStart() )
			{
				g_kTask.StartSvc(3);

				ContServerSiteID kContServerSiteID;
				g_kMMCMgr.Locked_GetServerList( CEL::ST_SUB_MACHINE_CONTROL, kContServerSiteID );
				g_kLogCopyMgr.Locked_Init( kContServerSiteID );

				INFO_LOG(BM::LOG_LV6, _T("==================================================") );
				INFO_LOG(BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
				INFO_LOG(BM::LOG_LV6, _T("==================================================") );

				if( g_kMMCMgr.UseSMCDataSync() )
				{
					SEventMessage kEventMsg;
					kEventMsg.PriType(PMET_MMC_MANAGER);
					kEventMsg.SecType(PT_MMC_LOADPATCHFILE);
					kEventMsg.Push( true );
					g_kTask.PutMsg(kEventMsg);
				}
				else
				{//싱크 안하게 강제 설정 했을 경우
					INFO_LOG( BM::LOG_LV1, _T("[ Pass Data Sync ]") );
					g_kMMCMgr.Locked_SetLoadPatchFileList(false);
				}
			}
			else
			{
				ASSERT_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
			}
		}break;
	default:
		{
			
		}break;
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

	g_kMMCMgr.Locked_SetConfigDirectory();
	if( !g_kLocal.LoadFromINI() )
	{
		std::cout << "Can't Load Local.ini" << std::endl;
		system("pause");
		return 0;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_MACHINE_CONTROL);
	if( !g_kProcessCfg.Locked_ParseArg(argc, argv) )
	{
		std::cout << "Error Arg" << std::endl;
		system("pause");
		return 0;
	}

	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_MMC%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL, BM::LOG_LV9, std::wstring(L"../LogFiles/") );
	}

	TCHAR chProductVersion[80] = {0, }, chFileVersion[80] = {0, };
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);

	INFO_LOG(BM::LOG_LV7, _T("==================== Server Initialize Start ====================") );
	INFO_LOG(BM::LOG_LV7, _T("Product Version : ") << chProductVersion );
	INFO_LOG(BM::LOG_LV7, _T("File Version : ") << chFileVersion );
	INFO_LOG(BM::LOG_LV7, _T("Protocol Version : ") << PACKET_VERSION_S);

	BM::CPackInfo kPackInfo;
	kPackInfo.Init();
	if(!BM::PgDataPackManager::Init(kPackInfo))
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("BM::PgDataPackManager Init Failed") );
		system("pause");
		return 0;
	}

	{
		if ( !g_kMMCMgr.Locked_ReadFromConfig( L"./MMC.ini", false ) )
		{
			std::cout << "Can't Load MMC.ini" << std::endl;
			system("pause");
			return 0;
		}

		g_kLogCopyMgr.Locked_ReadFromConfig( L"./MMC.ini" );
	}

	//{
	//	if ( !g_kMMCMgr.Locked_ReadFromConfig( L"./AG_PacketTypeName.ini", false ) )
	//	{
	//		std::cout << "Can't Load AG_PacketTypeName.ini" << std::endl;
	//	}
	//}
	
	CEL::INIT_CENTER_DESC kManagementServerInit;
	kManagementServerInit.eOT = BM::OUTPUT_ALL;
	kManagementServerInit.pOnRegist = OnRegist;
	kManagementServerInit.bIsUseDBWorker = true;
	kManagementServerInit.m_kCelLogFolder = kLogFolder;
	g_kCoreCenter.Init(kManagementServerInit);

	CONT_DB_INIT_DESC kContDBInit;
	if( g_kProcessCfg.RunMode() == CProcessConfig::E_RunMode_ReadIni )
	{
		if( !ReadDBConfig( EConfigType_Ini, L"", g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"DB_Config.ini\" Error!!") );
			system("pause");
			return 0;
		}
	}
	else
	{
		if( !ReadDBConfig( EConfigType_Inb, L"", g_kProcessCfg.ServerIdentity(), kContDBInit ) )
		{
			CAUTION_LOG( BM::LOG_LV0, _T("Load From \"DB_Config.inb\" Error!!") );
			system("pause");
			return 0;
		}
	}

	CONT_DB_INIT_DESC::iterator dbinit_itor = kContDBInit.begin();
	for( ; dbinit_itor != kContDBInit.end() ; ++dbinit_itor )
	{
		CEL::INIT_DB_DESC &kDBInit = (*dbinit_itor);
		kDBInit.bUseConsoleLog = false;
		kDBInit.OnDBExecute = PgMMCManager::OnDB_EXECUTE;
		kDBInit.OnDBExecuteTran = PgMMCManager::OnDB_EXECUTE_TRAN;
		kDBInit.dwWorkerCount = 1;
		kDBInit.m_kLogFolder = kLogFolder;

		g_kCoreCenter.Regist( CEL::RT_DB_WORKER, &kDBInit );
	}

	CEL::INIT_CORE_DESC kImmConnectorInit;
	kImmConnectorInit.OnSessionOpen	= OnConnectToImmigration;
	kImmConnectorInit.OnDisconnect	= OnDisConnectToImmigration;
	kImmConnectorInit.OnRecv		= OnRecvFromImmigration;
	kImmConnectorInit.IdentityValue(CEL::ST_IMMIGRATION);
	kImmConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kImmConnectorInit);

	{
		CEL::REGIST_TIMER_DESC kTimerInit;
		kTimerInit.pFunc = Timer_Connector;
		kTimerInit.dwInterval = 5000;
		g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit);
	}
	
	{
		CEL::REGIST_TIMER_DESC kTimerInit;
		kTimerInit.pFunc = Timer_ServerState;
		kTimerInit.dwInterval = 3000 + g_kMMCMgr.IntervalTime();
		g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit);
	}

	CEL::INIT_FINAL_SIGNAL kFinalInit;
	kFinalInit.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kFinalInit);

	RegistKeyEvent();

	g_kProcessCfg.Locked_SetConsoleTitle();

	g_kConsoleCommander.MainLoof();

	// 서비스 종료
	g_kTask.Close();
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();

	g_kMMCMgr.Terminate();
	BM::PgDataPackManager::Clear();

	return 0;
}

