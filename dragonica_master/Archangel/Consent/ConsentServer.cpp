// APServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Lohengrin/Loggroup.h"
#include "Lohengrin/VariableContainer.h"
#include "Variant/PgMCtrl.h"
#include "PgTimer.h"
#include "GALAServer.h"
#include "PgGFServer.h"
#include "NivalServer.h"
#include "PgBillingAPServer.h"
#include "PgMsgWorker.h"
#include "Nexon.h"
#include "FPTServer.h"
#include "PgNcServer.h"
#include "PgNcGSM.h"
#include "PgGravityServer.h"
#include "NC/AdminGateway.h"

extern void RegistKeyEvent();
extern bool CALLBACK OnTerminateServer(WORD const& wExitCode);

int GetConnectionUserCount()
{
	size_t iCount = 0;
	switch ( g_kLocal.ServiceRegion() )
	{
	case LOCAL_MGR::NC_TAIWAN:
		{
			iCount = g_kGFServer.Locked_GetLoginUserCount();
		}break;
	case LOCAL_MGR::NC_EU:
		{
			iCount = g_kGala.Locked_GetLoginUserCount(); 
		}break;
	case LOCAL_MGR::NC_RUSSIA:
		{
			iCount = g_kNival.Locked_GetLoginUserCount();
		}break;
	case LOCAL_MGR::NC_VIETNAM:
		{
			iCount = g_kFPT.Locked_GetLoginUserCount();
		}break;
	case LOCAL_MGR::NC_KOREA:
		{
			iCount = g_kNc.Locked_GetLoginUserCount();
		}break;
	case LOCAL_MGR::NC_USA:
		{
			iCount = g_kGravity.Locked_GetLoginUserCount();
		}break;
	}
	
	return static_cast<int>(iCount);
}

void GlobalInit()
{
	g_kTerminateFunc = OnTerminateServer;//Init MCtrl
	g_kGetNowUserFunc = GetConnectionUserCount;
}

void CALLBACK OnRegist(CEL::SRegistResult const &rkArg)
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("OnRegist Failed") );
	}

	switch ( rkArg.eType )
	{
	case CEL::RT_CONNECTOR:
		{
			switch ( g_kLocal.ServiceRegion() )
			{
			case LOCAL_MGR::NC_TAIWAN:
				{
					g_kGFServer.Locked_OnRegistConnector(rkArg);
				}break;
			case LOCAL_MGR::NC_EU:
				{
					g_kGala.Locked_OnRegistConnector(rkArg); 
				}break;
			case LOCAL_MGR::NC_RUSSIA:
				{
					g_kNival.Locked_OnRegistConnector(rkArg);
				}break;
			case LOCAL_MGR::NC_JAPAN:
				{
					g_kJapanDaemon.OnRegistConnector(rkArg);
				}break;
			case LOCAL_MGR::NC_VIETNAM:
				{
					g_kFPT.Locked_OnRegistConnector(rkArg);
				}break;
			case LOCAL_MGR::NC_KOREA:
				{
					g_kNc.Locked_OnRegistConnector(rkArg);
				}break;
			case LOCAL_MGR::NC_USA:
				{
					g_kGravity.Locked_OnRegistConnector(rkArg);
				}break;
			default:
				{
					__asm int 3;
				}break;
			}
		}break;
	case CEL::RT_TIMER_FUNC:
		{
		}break;
	case CEL::RT_ACCEPTOR:
		{
		}break;
	case CEL::RT_FINAL_SIGNAL:
		{//추가 등록 완료
			if( true == g_kCoreCenter.SvcStart() )
			{
				INFO_LOG( BM::LOG_LV6, _T("==================================================") );
				INFO_LOG( BM::LOG_LV6, _T("============ CoreCenter Run ======================") );
				INFO_LOG( BM::LOG_LV6, _T("==================================================") );
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================"));
			}
		}break;
	default:
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unknown Type : ") << rkArg.eType );
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

	g_kProcessCfg.Locked_SetConfigDirectory();
	if(!g_kLocal.LoadFromINI(g_kProcessCfg.ConfigDir() + L"Local.ini"))
	{
		std::cout << "Load From \"Config/local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}

	g_kProcessCfg.Locked_SetServerType(CEL::ST_CONSENT);
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
		return 0;
	}

	DWORD const dwServiceRegion = g_kLocal.ServiceRegion();
	switch (dwServiceRegion)
	{
	case LOCAL_MGR::NC_EU:
	case LOCAL_MGR::NC_TAIWAN:
	case LOCAL_MGR::NC_RUSSIA:
	case LOCAL_MGR::NC_JAPAN:
	case LOCAL_MGR::NC_VIETNAM:
	case LOCAL_MGR::NC_KOREA:
	case LOCAL_MGR::NC_USA:
		{
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Cannot run ConsentServer SERVICE_REGION = ") << dwServiceRegion);
			system("pause");
			return 0;
		}break;
	}

	if( !g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Consent_constant.ini")) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Can't load Contents_constant.ini file") );
		return 0;
	}

	std::wstring kLogFolder;

	{
		TCHAR chLog[MAX_PATH] = {0,};
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_Consent%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL);

#ifndef _DEBUG
		if ( !(CProcessConfig::E_RunMode_Debug & g_kProcessCfg.RunMode()) )
		{
			INFO_LOG_LEVEL(BM::LOG_LV7);
		}
#endif
	}

	GlobalInit();

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion( 80, chFileVersion, 80, chProductVersion);
	INFO_LOG(BM::LOG_LV6, _T("==================== Server Initialize Start ===================="));
	INFO_LOG(BM::LOG_LV6, _T("Product Version : ") << chProductVersion );
	INFO_LOG(BM::LOG_LV6, _T("File Version : ") << chFileVersion );
	INFO_LOG(BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S);

	{
		CEL::INIT_CENTER_DESC kCenterInit;
		kCenterInit.eOT = BM::OUTPUT_ALL;
		kCenterInit.pOnRegist = OnRegist;
		kCenterInit.dwProactorThreadCount = 3;
		kCenterInit.m_kCelLogFolder = kLogFolder;
		g_kCoreCenter.Init(kCenterInit);
	}

	{
		switch ( g_kLocal.ServiceRegion() )
		{
		case LOCAL_MGR::NC_TAIWAN:
			{
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= PgGFServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgGFServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgGFServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit);

				CEL::INIT_CORE_DESC kMoblieLockConnectorInit;
				kMoblieLockConnectorInit.OnSessionOpen	= PgGFServer::OnConnectFromMobileLock;
				kMoblieLockConnectorInit.OnDisconnect	= PgGFServer::OnDisConnectFromMobileLock;
				kMoblieLockConnectorInit.OnRecv			= PgGFServer::OnRecvFromMobileLock;
				kMoblieLockConnectorInit.kOrderGuid.Generate();
				kMoblieLockConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				kMoblieLockConnectorInit.ServiceHandlerType( CEL::SHT_GF );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kMoblieLockConnectorInit);

				{
					std::wstring const wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_AP_Config.ini") );

					TCHAR wszIP[MAX_PATH] = {0,};
					::GetPrivateProfileString( _T("BILLINGAP"), _T("ACCEPT_IP"), _T("0.0.0.0"), wszIP, MAX_PATH, wstrFilename.c_str() );
					WORD const nPort = static_cast<WORD>( ::GetPrivateProfileInt( _T("BILLINGAP"), _T("ACCEPT_PORT"), 0, wstrFilename.c_str() ) );
					
					CEL::INIT_CORE_DESC kBillingAPAcceptorInit;
					kBillingAPAcceptorInit.kBindAddr.Set( wszIP, nPort );

					if ( true == kBillingAPAcceptorInit.kBindAddr.IsCorrect() )
					{
						kBillingAPAcceptorInit.kNATAddr = kBillingAPAcceptorInit.kBindAddr;

						kBillingAPAcceptorInit.OnSessionOpen		= PgBillingAPServer::OnAcceptFromBillingAP;
						kBillingAPAcceptorInit.OnDisconnect			= PgBillingAPServer::OnDisConnectFromBillingAP;
						kBillingAPAcceptorInit.OnRecv				= PgBillingAPServer::OnRecvFromBillingAP;
						kBillingAPAcceptorInit.bIsImmidiateActivate = true;
						kBillingAPAcceptorInit.IdentityValue( CEL::ST_AP );
						kBillingAPAcceptorInit.ServiceHandlerType( CEL::SHT_GF );
						g_kCoreCenter.Regist( CEL::RT_ACCEPTOR, &kBillingAPAcceptorInit);
					}
					else
					{
						INFO_LOG( BM::LOG_LV5, L"[Billing] Accept IP " << kBillingAPAcceptorInit.kBindAddr.ToString() << L" Error!!! " );
					}
				}
				
				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_GF;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);

				CEL::REGIST_TIMER_DESC kInitTimer2;
				kInitTimer2.pFunc = Timer_30s_GF;
				kInitTimer2.dwInterval = 30000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer2);

				CEL::REGIST_TIMER_DESC kInitTimer3;
				kInitTimer3.pFunc = Timer_Ping_GF;
				kInitTimer3.dwInterval = 1000 * 180;// 3 minute
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer3);

				g_kMsgDisPatcher.VInit(5);
				g_kMsgDisPatcher.VActivate();
			}break;
		case LOCAL_MGR::NC_JAPAN:
			{
				InitNexonPassport();

//				SAFE_DELETE(g_pkGalaServer);
//				g_pkGalaServer = new PgNexon;

				//Immigration 접근
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= OnConnectFromImmigrationForJapan;
				kImmConnectorInit.OnDisconnect	= OnDisconnectFromImmigrationForJapan;
				kImmConnectorInit.OnRecv		= OnRecvFromImmigrationForJapan;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit);

				//넥슨 캐쉬 접근
				CEL::INIT_CORE_DESC kJpnCashConnectorInit;
				kJpnCashConnectorInit.OnSessionOpen		= OnConnectFromNexonCash;
				kJpnCashConnectorInit.OnDisconnect		= OnDisconnectFromNexonCash;
				kJpnCashConnectorInit.OnRecv			= OnRecvFromNexonCash;
				kJpnCashConnectorInit.IdentityValue(CEL::ST_EXTERNAL1);
				kJpnCashConnectorInit.ServiceHandlerType(CEL::SHT_JAPAN);

				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kJpnCashConnectorInit);

				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_JAPAN;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);

				g_kMsgDisPatcher.VInit(5);
				g_kMsgDisPatcher.VActivate();
			}break;
		case LOCAL_MGR::NC_EU:
			{
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= PgGalaServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgGalaServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgGalaServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit) ;

				CEL::INIT_CORE_DESC kGTXDConnectorInit;
				kGTXDConnectorInit.OnSessionOpen	= PgGalaServer::OnConnectFromGTXD;
				kGTXDConnectorInit.OnDisconnect	= PgGalaServer::OnDisconnectFromGTXD;
				kGTXDConnectorInit.OnRecv		= PgGalaServer::OnRecvFromGTXD;
				kGTXDConnectorInit.kOrderGuid.Generate();
				kGTXDConnectorInit.IdentityValue( CEL::ST_GALA );
				kGTXDConnectorInit.ServiceHandlerType( CEL::SHT_NC );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kGTXDConnectorInit) ;

				CEL::INIT_CORE_DESC kHttpConnectorInit;
				kHttpConnectorInit.OnSessionOpen	= PgGalaServer::OnConnectFromHttp;
				kHttpConnectorInit.OnDisconnect	= PgGalaServer::OnDisconnectFromHttp;
				kHttpConnectorInit.OnRecv		= PgGalaServer::OnRecvFromHttp;
				kHttpConnectorInit.kOrderGuid.Generate();
				kHttpConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				kHttpConnectorInit.ServiceHandlerType( CEL::SHT_NOHEADER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kHttpConnectorInit);

				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_GALA;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);
			}break;
		case LOCAL_MGR::NC_RUSSIA:
			{
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= PgNivalServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgNivalServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgNivalServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit) ;

				CEL::INIT_CORE_DESC kAuthHttpConnectorInit;
				kAuthHttpConnectorInit.OnSessionOpen	= PgNivalServer::OnConnectFromAuthHttp;
				kAuthHttpConnectorInit.OnDisconnect	= PgNivalServer::OnDisconnectFromAuthHttp;
				kAuthHttpConnectorInit.OnRecv		= PgNivalServer::OnRecvFromAuthHttp;
				kAuthHttpConnectorInit.kOrderGuid.Generate();
				kAuthHttpConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				kAuthHttpConnectorInit.ServiceHandlerType( CEL::SHT_NOHEADER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kAuthHttpConnectorInit);

				CEL::INIT_CORE_DESC kCashHttpConnectorInit;
				kCashHttpConnectorInit.OnSessionOpen	= PgNivalServer::OnConnectFromCashHttp;
				kCashHttpConnectorInit.OnDisconnect	= PgNivalServer::OnDisconnectFromCashHttp;
				kCashHttpConnectorInit.OnRecv		= PgNivalServer::OnRecvFromCashHttp;
				kCashHttpConnectorInit.kOrderGuid.Generate();
				kCashHttpConnectorInit.IdentityValue( CEL::ST_EXTERNAL2 );
				kCashHttpConnectorInit.ServiceHandlerType( CEL::SHT_NOHEADER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kCashHttpConnectorInit);

				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_Nival;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);
			}break;
		case LOCAL_MGR::NC_VIETNAM:
			{
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= PgFPTServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgFPTServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgFPTServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit) ;

				CEL::INIT_CORE_DESC kAuthHttpConnectorInit;
				kAuthHttpConnectorInit.OnSessionOpen	= PgFPTServer::OnConnectFromAuthHttp;
				kAuthHttpConnectorInit.OnDisconnect	= PgFPTServer::OnDisconnectFromAuthHttp;
				kAuthHttpConnectorInit.OnRecv		= PgFPTServer::OnRecvFromAuthHttp;
				kAuthHttpConnectorInit.kOrderGuid.Generate();
				kAuthHttpConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				kAuthHttpConnectorInit.ServiceHandlerType( CEL::SHT_NOHEADER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kAuthHttpConnectorInit);

				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_FPT;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);
			}break;
		case LOCAL_MGR::NC_KOREA:
			{
				// Imm
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen	= PgNcServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgNcServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgNcServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit);

				{// GSM
					g_kGSM; // 인스턴스 생성

					CEL::INIT_CORE_DESC kNcGsmInit;
					std::wstring const wstrFilename( g_kProcessCfg.ConfigDir() + _T("Consent_NC_Config.ini") );

					TCHAR wszIP[MAX_PATH] = {0,};
					::GetPrivateProfileString( _T("GSM"), _T("ACCEPT_IP"), _T("0.0.0.0"), wszIP, MAX_PATH, wstrFilename.c_str() );
					WORD const nPort = static_cast<WORD>( ::GetPrivateProfileInt( _T("GSM"), _T("ACCEPT_PORT"), 0, wstrFilename.c_str() ) );

					kNcGsmInit.kBindAddr.Set(wszIP, nPort);
					kNcGsmInit.kNATAddr.Set(wszIP, nPort);
					kNcGsmInit.OnSessionOpen	= PgNcGSM::OnAcceptFromGsm;
					kNcGsmInit.OnDisconnect		= PgNcGSM::OnDisconnectFromGsm;
					kNcGsmInit.OnRecv			= PgNcGSM::OnRecvFromGsm;
					kNcGsmInit.ServiceHandlerType(CEL::SHT_NC);
					kNcGsmInit.bIsImmidiateActivate	= true;
					g_kCoreCenter.Regist(CEL::RT_ACCEPTOR, &kNcGsmInit);
				}
				
				//// SA
				//CEL::INIT_CORE_DESC kSAConnectorInit;
				//kSAConnectorInit.OnSessionOpen	= PgNcServer::OnConnectFromSA;
				//kSAConnectorInit.OnDisconnect	= PgNcServer::OnDisconnectFromSA;
				//kSAConnectorInit.OnRecv			= PgNcServer::OnRecvFromSA;
				//kSAConnectorInit.kOrderGuid.Generate();
				//kSAConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				//kSAConnectorInit.ServiceHandlerType( CEL::SHT_NC );
				//g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kSAConnectorInit );
				
				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc = Timer_5s_NC;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer);
				
				if( !PgNcServer::InitAdminGateWay() )
				{
					std::cout<<"Initialized fail."<<std::endl;
				}
			}break;
		case LOCAL_MGR::NC_USA:
			{
				// Immgration 연결
				CEL::INIT_CORE_DESC kImmConnectorInit;
				kImmConnectorInit.OnSessionOpen = PgGravityServer::OnConnectFromImmigration;
				kImmConnectorInit.OnDisconnect	= PgGravityServer::OnDisconnectFromImmigration;
				kImmConnectorInit.OnRecv		= PgGravityServer::OnRecvFromImmigration;
				kImmConnectorInit.kOrderGuid.Generate();
				kImmConnectorInit.IdentityValue( CEL::ST_IMMIGRATION );
				kImmConnectorInit.ServiceHandlerType( CEL::SHT_SERVER );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kImmConnectorInit );
				
				// Auth
				CEL::INIT_CORE_DESC kGravityAuthConnectorInit;
				kGravityAuthConnectorInit.OnSessionOpen = PgGravityServer::OnConnectFromGravityAuth;
				kGravityAuthConnectorInit.OnDisconnect	= PgGravityServer::OnDisconnectFromGravityAuth;
				kGravityAuthConnectorInit.OnRecv		= PgGravityServer::OnRecvFromGravityAuth;
				kGravityAuthConnectorInit.kOrderGuid.Generate();
				kGravityAuthConnectorInit.IdentityValue( CEL::ST_EXTERNAL1 );
				kGravityAuthConnectorInit.ServiceHandlerType( CEL::SHT_GRAVITY );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kGravityAuthConnectorInit );

				// Item
				CEL::INIT_CORE_DESC kGravityItemConnectorInit;
				kGravityItemConnectorInit.OnSessionOpen = PgGravityServer::OnConnectFromGravityItem;
				kGravityItemConnectorInit.OnDisconnect	= PgGravityServer::OnDisconnectFromGravityItem;
				kGravityItemConnectorInit.OnRecv		= PgGravityServer::OnRecvFromGravityItem;
				kGravityItemConnectorInit.kOrderGuid.Generate();
				kGravityItemConnectorInit.IdentityValue( CEL::ST_EXTERNAL2 );
				kGravityItemConnectorInit.ServiceHandlerType( CEL::SHT_GRAVITY );
				g_kCoreCenter.Regist( CEL::RT_CONNECTOR, &kGravityItemConnectorInit );
				
				CEL::REGIST_TIMER_DESC kInitTimer;
				kInitTimer.pFunc =  Timer_5s_Gravity;
				kInitTimer.dwInterval = 5000;
				g_kCoreCenter.Regist( CEL::RT_TIMER_FUNC, &kInitTimer );
			}break;
		default:
			{
			}break;
		}
	}

	{
		CEL::INIT_FINAL_SIGNAL kFinalInit;
		g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kFinalInit);
	}

	RegistKeyEvent();

	g_kProcessCfg.Locked_SetConsoleTitle();

	std::cout<< "-- Started --" << std::endl;

	g_kConsoleCommander.MainLoof();

	switch ( g_kLocal.ServiceRegion() )
	{
	case LOCAL_MGR::NC_TAIWAN:
		{
			g_kMsgDisPatcher.VDeactivate();
		}break;
	case LOCAL_MGR::NC_KOREA:
		{
			PgNcServer::UnloadAdminGateWay();
		}break;
	case LOCAL_MGR::NC_JAPAN:
		{
			g_kMsgDisPatcher.VDeactivate();
			g_kNxLoginDis.VDeactivate();
		}break;
	default:
		{
		}break;
	}
	// 서비스 종료
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();

	return 0;
}

