#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "Variant/PgMCtrl.h"
#include "HellDart/PgProtocolFilter.h"
#include "Constant.h"
#include "PgKeyEvent.h"
#include "PgRecvFromCenter.h"
#include "PgRecvFromUser.h"
#include "PgHub.h"
#include "PgRecvFromLog.h"

//스위치에는 PlayerDBData 를 가지고 있지 않는다.

HRESULT InitAntiHack(void)
{
	if ( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DR_UNABLE_ANTIHACK) )
	{
		switch( g_kLocal.ServiceRegion() )
		{
		case LOCAL_MGR::NC_DEVELOP:
			{
				g_kHub.InitAntiHack(ANTIHACK_DUMMY);
			}break;
		case LOCAL_MGR::NC_TAIWAN:
		case LOCAL_MGR::NC_EU:
		case LOCAL_MGR::NC_RUSSIA:
		case LOCAL_MGR::NC_CHINA:
		case LOCAL_MGR::NC_SINGAPORE:
			{
				g_kHub.InitAntiHack(ANTIHACK_GAMEGUARD);
			}break;
		case LOCAL_MGR::NC_THAILAND:
		case LOCAL_MGR::NC_INDONESIA:
		//case LOCAL_MGR::NC_USA:
		case LOCAL_MGR::NC_PHILIPPINES:
		//case LOCAL_MGR::NC_JAPAN:
		case LOCAL_MGR::NC_KOREA:
			{
				g_kHub.InitAntiHack(ANTIHACK_AHN_HACKSHIELD);
			}break;
		default:
			{
				INFO_LOG(BM::LOG_LV2, __FL__ << _T("====================== Not Use AnitHack ======================") );
				// none use
			}break;
		}
	}
	return S_OK;
}

void CALLBACK Timer_CheckPing(DWORD dwUserData)
{//핑체크는 빠르게.
	//if ( g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug )
	//{
	//	return; // 디버그에서는 핑체크를 하지 않는다.
	//}
	g_kHub.Locked_OnTimer_CheckPing();
}

void CALLBACK Timer_ClearReserveMember(DWORD dwUserData)
{
	g_kHub.Locked_OnTimer_ClearTimeOverMember();
}

void CALLBACK Timer_HeartBeat(DWORD dwUserData)
{
	//static DWORD dwChecker1 = BM::GetTime32();
	static DWORD dwChecker2 = BM::GetTime32();
	
	//if( BM::TimeCheck(dwChecker1, 30000) )
	//{
	//	g_kHub.Locked_CheckGameGuard();
	//}

	if ( ANTIHACK_GAMEGUARD == g_kHub.AntiHackType() )
	{
		if( BM::TimeCheck(dwChecker2, static_cast<DWORD>(PgHub::ms_iAntiHackCheck_Max)) )
		{
			GGAuthUpdateTimer();
		}	
	}

	static Loki::Mutex s_UserCounterLogMutex;
	static DWORD s_dwUserCounterLogTimer = BM::GetTime32();
	{
		// Timer Thread 가 혹시 두개 이상 될까봐 Thread safe...
		BM::CAutoMutex kLock(s_UserCounterLogMutex);
		if ( BM::TimeCheck( s_dwUserCounterLogTimer, 900000 ) )	// 15분마다 한번씩..
		{
#ifdef _MEMORY_TRACKING
			S_STATE_LOG(BM::LOG_LV0, _T("---Memory Observe---"));
			g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
			if (g_pkMemoryTrack)
			{
				g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE);
			}
#endif
		}
	}
}

void CALLBACK OnRegist( CEL::SRegistResult const &rkArg )
{
	if( rkArg.iRet != CEL::CRV_SUCCESS)
	{
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
			}break;
		case CEL::ST_CENTER:
			{
				g_kProcessCfg.CenterConnector(rkArg.guidObj);
				g_kProcessCfg.Locked_ConnectCenter();
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ Connector!!!! ======================") );
			}break;
		}
	}

	if( CEL::RT_FINAL_SIGNAL == rkArg.eType )
	{//추가 등록 완료
		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" RegistAllComplete") );

		if(g_kCoreCenter.SvcStart())
		{
			if( S_OK != InitAntiHack() )
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ AnitHack Init Fail !!!! ======================") );
			}

			INFO_LOG( BM::LOG_LV6, __FL__ << _T("==================================================") );
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("============ CoreCenter Run ======================") );
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("==================================================") );
			g_kProcessCfg.Locked_ConnectLog();
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("============ CoreCenter Cannot Run!!!! ======================") );
		}
	}
}

extern void RegProtocolFunc();
bool LoadXML()
{
	RegProtocolFunc();

	// 안티핵 연동 패킷 등록
	g_kProtocolFilter.Regist(PT_C_S_TRY_ACCESS_SWITCH_SECOND, Check_PT_C_S_TRY_ACCESS_SWITCH_SECOND);
	g_kProtocolFilter.Regist(PT_C_S_ANS_GAME_GUARD_CHECK, Check_PT_C_S_ANS_GAME_GUARD_CHECK);
	return true;
}

bool GlobalInit()
{
	std::wstring const kConstantFile(L"Switch_constant.ini");
	if( !g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + kConstantFile) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Open ["<<kConstantFile<<L"] constant file"));
		return false;
	}
	if( S_OK != g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_SpeedHackGap, PgReserveMemberData::ms_iSpeedCheckGap ) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read ["<<kConstantFile<<L"][EVar_Kind_TimeCheck][EVar_TimeCheck_SpeedHackGap] Variable"));
		return false;
	}
	if( S_OK != g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Min, PgHub::ms_iAntiHackCheck_Min) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read ["<<kConstantFile<<L"][EVar_Kind_TimeCheck][EVar_TimeCheck_AntiHack_Min] Variable"));
		return false;
	}
	if( S_OK != g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Max, PgHub::ms_iAntiHackCheck_Max) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read ["<<kConstantFile<<L"][EVar_Kind_TimeCheck][EVar_TimeCheck_AntiHack_Max] Variable"));
		return false;
	}
	if( S_OK != g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Wait, PgHub::ms_iAntiHackCheck_Wait) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read ["<<kConstantFile<<L"][EVar_Kind_TimeCheck][EVar_TimeCheck_AntiHack_Wait] Variable"));
		return false;
	}

	if ( PgHub::ms_iAntiHackCheck_Min > PgHub::ms_iAntiHackCheck_Max )
	{
		std::swap( PgHub::ms_iAntiHackCheck_Min, PgHub::ms_iAntiHackCheck_Max );
	}
	if ( PgHub::ms_iAntiHackCheck_Min < PgHub::ms_iAntiHackCheck_Wait )
	{
		PgHub::ms_iAntiHackCheck_Min = static_cast<int>(PgHub::ms_iAntiHackCheck_Wait);
	}

	int iValue = 0;
	if( S_OK != g_kVariableContainer.Get( EVar_UserManage, EVAR_USER_MANAGE_DISCONNECT_HACKUSER, iValue) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read [EVar_UserManage][EVAR_USER_MANAGE_DISCONNECT_HACKUSER] Variable"));
		return false;
	}
	PgHub::ms_bDisconnectHackUser = (0 != iValue)? true: false;
	INFO_LOG(BM::LOG_LV6, __FL__ << _T("EVar_UserManage_Disconnect_HackUser = ") << PgHub::ms_bDisconnectHackUser);


	if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DR_UNABLE_ANTIHACK) )
	{
		if( S_OK != g_kVariableContainer.Get( EVar_UserManage, EVAR_USERMANAGE_USEANTIHACKSERVERBIND, iValue) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("Cannot Read [EVar_UserManage][EVAR_USERMANAGE_USEANTIHACKSERVERBIND] Variable"));
			return false;
		}
		PgHub::ms_bUseAntiHackServerBind = (0 != iValue)? true: false;

		if( S_OK != g_kVariableContainer.Get( EVar_UserManage, EVAR_USERMANAGE_USEANTIHACK_DISCONNECT_2ND, iValue) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Cannot Read [EVar_UserManage][EVAR_USERMANAGE_USEANTIHACK_DISCONNECT_2ND] Variable");
			return false;
		}
		PgHub::ms_bUseAntiHackDisconnect2nd = (0 != iValue)? true: false;

		INFO_LOG(BM::LOG_LV6, __FL__ << _T("[EVAR_USERMANAGE_USEANTIHACKSERVERBIND] = ") << PgHub::ms_bUseAntiHackServerBind);
		INFO_LOG(BM::LOG_LV6, __FL__ << _T("Evar_UserManage_UseAntiHack_Disconnect_2nd = ") << PgHub::ms_bUseAntiHackDisconnect2nd);
	}
	else
	{
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Can't load [EVAR_USERMANAGE_USEANTIHACKSERVERBIND], Unable AntiHack is True"));
		PgHub::ms_bUseAntiHackServerBind = false;
		PgHub::ms_bUseAntiHackDisconnect2nd = false;
	}

	{// Log
		int iValue_OutPut = BM::OUTPUT_ALL;
		int iValue_Level = static_cast<int>(BM::LOG_LV9);

		g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogOutputType, iValue_OutPut);
		g_kVariableContainer.Get(EVar_Kind_Log, EVar_LogLevelLimit, iValue_Level);

		iValue_OutPut &= BM::OUTPUT_ALL;

		SetLogOutPutType(static_cast<BM::E_OUPUT_TYPE>(iValue_OutPut));
		SetLogLeveLimit(static_cast<BM::E_LOG_LEVEL>(iValue_Level));

		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_LogOutputType = ") << iValue_OutPut);
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Load EVar_LogLevelLimit = ") << iValue_Level);
	}

	g_kTerminateFunc = OnTerminateServer;//Init MCtrl
	g_kGetNowUserFunc = GetConnectionUserCount;

#ifdef _MEMORY_TRACKING
	g_pkMemoryTrack = new PgMemoryTrack();
	g_pkMemoryTrack->Init(BM::OUTPUT_FILE_AND_CONSOLE, _T("./MemoryTrack"), _T("MemoryTrack.txt"));
#endif
	return true;
}

void GlobalRelease()
{
#ifdef _MEMORY_TRACKING
	PgMemoryTrack* pkTrack = g_pkMemoryTrack;
	g_pkMemoryTrack = NULL;
	delete pkTrack;
#endif
}

#define REGISTER_LUA_MODULE(name) \
	extern void name ##_RegisterWrapper(lua_State *pkState); \
	static lwOnibal::AddLibAuto g_kAutoLib(name ##_RegisterWrapper); \

void RegisterLuaModules()
{
	REGISTER_LUA_MODULE(net);
}

#undef REGISTER_LUA_MODULE

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
		std::cout << "Load From \"local.ini\" Error!!" << std::endl;
		system("pause");
		return 0;
	}

	if( !GlobalInit() )
	{
		return 0;
	}
	
	g_kProcessCfg.Locked_SetServerType(CEL::ST_SWITCH);
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
		_stprintf_s(chLog, _countof(chLog), _T("R%dC%d_SWITCH%04d"), g_kProcessCfg.RealmNo(), g_kProcessCfg.ChannelNo(), g_kProcessCfg.ServerNo());
		InitLog(kLogFolder, chLog, BM::OUTPUT_ALL );

#ifndef _DEBUG
		if ( !(g_kProcessCfg.RunMode() & CProcessConfig::E_RunMode_Debug) )
		{
			INFO_LOG_LEVEL(BM::LOG_LV9);
		}
#endif
	}


	LoadXML();

	RegisterLuaModules();
	if (!lwOnibal::InitOnibal())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"failed to initializing Onibal Scripting");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	if (!lwOnibal::InitOnibalServerSide())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"failed to initializing Onibal Server Scripting");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	{
		lua_wrapper_user kLua(g_kLuaTinker);
		lua_tinker::dofile(*kLua, "./Script/init.lua");
	}

	TCHAR chProductVersion[80], chFileVersion[80];
	g_kProcessCfg.Locked_GetVersion(80, chFileVersion, 80, chProductVersion);
	INFO_LOG( BM::LOG_LV6, _T("==================== Server Initialize Start ====================") );
	INFO_LOG( BM::LOG_LV6, _T("Product Version : ") << chProductVersion );
	INFO_LOG( BM::LOG_LV6, _T("File Version : ") << chFileVersion );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version C : ") << PACKET_VERSION_C );
	INFO_LOG( BM::LOG_LV6, _T("Packet Version S : ") << PACKET_VERSION_S );
	//! 들어온 인자를 파싱해서.
	//! Connector와 Acceptor를 만든다.
	//! Conenctor로 Center로 접근.
	//! 본인의 서버타입을 보내고.
	//! 필요한 맵 혹은 기타 데이터를 요청한다.
	//! 이후 서비스 시작.
{
	CEL::INIT_CENTER_DESC kCenterInit;
	kCenterInit.eOT = BM::OUTPUT_ALL;
	kCenterInit.pOnRegist = OnRegist;
	kCenterInit.dwProactorThreadCount = 25;//유저 + 맵서버들.
	kCenterInit.m_kCelLogFolder = kLogFolder;
	g_kCoreCenter.Init(kCenterInit);
}
	CEL::REGIST_TIMER_DESC kTimerInit;
	kTimerInit.pFunc			= Timer_ClearReserveMember;
	kTimerInit.dwInterval	= 5000;


	CEL::REGIST_TIMER_DESC kTimerInit2;
	kTimerInit2.pFunc			= Timer_CheckPing;
	kTimerInit2.dwInterval	= 1000;

	CEL::REGIST_TIMER_DESC kLogTimerInit;
	kLogTimerInit.pFunc			= Timer_HeartBeat;
	kLogTimerInit.dwInterval	= 10000;//10sec

	CEL::INIT_CORE_DESC kLogConnectorInit;
	kLogConnectorInit.OnSessionOpen	= OnConnectToLog;
	kLogConnectorInit.OnDisconnect	= OnDisconnectFromLog;
	kLogConnectorInit.OnRecv			= OnRecvFromLog;
	kLogConnectorInit.IdentityValue(CEL::ST_LOG);
	kLogConnectorInit.ServiceHandlerType(CEL::SHT_SERVER);


	CEL::INIT_CORE_DESC kConnInit;
	kConnInit.OnSessionOpen	= OnConnectFromCenter;
	kConnInit.OnDisconnect	= OnDisconnectFromCenter;
	kConnInit.OnRecv			= OnRecvFromCenter;
	kConnInit.IdentityValue(CEL::ST_CENTER);
	kConnInit.ServiceHandlerType(CEL::SHT_SERVER);

	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kLogConnectorInit);
	g_kCoreCenter.Regist(CEL::RT_CONNECTOR, &kConnInit);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kLogTimerInit);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit);
	g_kCoreCenter.Regist(CEL::RT_TIMER_FUNC, &kTimerInit2);
	
	CEL::INIT_FINAL_SIGNAL kInitFinal;
	kInitFinal.kIdentity = 0;
	g_kCoreCenter.Regist(CEL::RT_FINAL_SIGNAL, &kInitFinal);

	if( !RegistKeyEvent() ){ return 0; }
	
	g_kProcessCfg.Locked_SetConsoleTitle();

	g_kConsoleCommander.MainLoof();

	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
	GlobalRelease();
	CleanupGameguardAuth();
	return 0;
}

GGAUTHS_API void NpLog(int mode, char* msg)
{
	//각 게임사의 로그 작성 정책에 맞게 로그를 남깁니다. 
	if( mode & (NPLOG_DEBUG | NPLOG_ERROR) //Select log mode. 
	&&	msg )
	{
		std::string const kStrTemp(msg);
		INFO_LOG(BM::LOG_LV2, kStrTemp);
	}
};


GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	INFO_LOG(BM::LOG_LV2, L"GGAuth version update ["<<((report->nType==1)?L"GameGuard Ver":L"Protocol Num")<<L"] : ["<<HEX(report->dwBefore)<<L"] -> ["<<HEX(report->dwNext)<<L"]");
};
