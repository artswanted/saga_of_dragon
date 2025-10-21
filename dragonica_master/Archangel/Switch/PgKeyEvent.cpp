#include "stdafx.h"
#include "Constant.h"

bool CALLBACK OnEscape(unsigned short const &rusInputKey)
{
	std::cout<< __FUNCTIONW__ << std::endl;
	std::cout << "Press [F11] key to quit" << std::endl;
	return true;
}

bool CALLBACK OnF1(unsigned short const &rusInputKey)
{
	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO );
	g_kProcessCfg.Locked_DisplayState();
	g_kHub.Locked_DisplayState();
	return false;
}


bool CALLBACK OnF3(unsigned short const &rusInputKey)
{
//	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER, BM::Stream(64000));
	return false;
}

bool CALLBACK OnF4( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	SHORT sState = GetKeyState(VK_SHIFT);
	if (HIBYTE(sState) == 0)
	{
		S_STATE_LOG(BM::LOG_LV0, _T("---Memory Observe---"));
#ifdef _MEMORY_TRACKING
		g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
		if (g_pkMemoryTrack)
		{
			g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE);
		}
#endif
	}
	else
	{
#ifdef _MEMORY_TRACKING
		sState = GetKeyState(VK_CONTROL);
		if (HIBYTE(sState) == 0)
		{
			return false;
		}
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING] MapServer will display MemoryAlloced, It takes some time OK??","DRAGONICA_Map",MB_OKCANCEL) )
		{
			g_kObjObserver.DisplayState(g_kLogWorker, LT_S_STATE);
			if (g_pkMemoryTrack)
			{
				g_pkMemoryTrack->DisplayState(g_kLogWorker, LT_S_STATE, true);
			}
		}
#endif
	}	return false;
}


bool CALLBACK OnF7(unsigned short const &rusInputKey)
{
	return false;
//	BM::GUID kMemberGuid = "18EC1C7C-7071-4B0F-9696-DB10AA3794BB";

//	BM::Stream kPacket(PT_S_T_REQ_CHARACTER_LIST, kMemberGuid);
//	g_kProcessCfg.SendToCenter(kPacket);

	return false;
}

bool CALLBACK OnF8(unsigned short const &rusInputKey)
{
	INFO_LOG(BM::LOG_LV7, "Reload lua...");
	lua_tinker::call<void>("SelfReload");
	lua_tinker::call<void>("Reload");
	INFO_LOG(BM::LOG_LV5, "Reload success!");
	return true;
}

bool CALLBACK OnF9(unsigned short const &rusInputKey)
{
	if( g_kVariableContainer.LoadIni(g_kProcessCfg.ConfigDir() + _T("Switch_constant.ini")) )
	{
		int iValue = 0;

		//임시로 해야 모든 상황에서 나온다
		SetLogLeveLimit(BM::LOG_LV9);

		if (S_OK == g_kVariableContainer.Get( EVar_UserManage, EVAR_USER_MANAGE_DISCONNECT_HACKUSER, iValue) )
		{
			PgHub::ms_bDisconnectHackUser = (iValue == 1) ? true : false;
		}
		else
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("Cannot Read EVar_UserManage_Disconnect_HackUser Variable"));
			PgHub::ms_bDisconnectHackUser = false;
		}
		INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload EVar_UserManage_Disconnect_HackUser = ") << (PgHub::ms_bDisconnectHackUser?"true":"false"));

		if( !g_kLocal.IsAbleServiceType(LOCAL_MGR::ST_DR_UNABLE_ANTIHACK) )
		{
			if (S_OK == g_kVariableContainer.Get( EVar_UserManage, EVAR_USERMANAGE_USEANTIHACKSERVERBIND, iValue) )
			{
				PgHub::ms_bUseAntiHackServerBind = ((0 != iValue) ? true : false);
			}
			if( S_OK == g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Min, iValue) )
			{
				PgHub::ms_iAntiHackCheck_Min = iValue;
			}
			if( S_OK == g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Max, iValue) )
			{
				PgHub::ms_iAntiHackCheck_Max = iValue;
			}
			if( S_OK == g_kVariableContainer.Get( EVar_Kind_TimeCheck, EVar_TimeCheck_AntiHack_Wait, iValue) )
			{
				PgHub::ms_iAntiHackCheck_Wait = iValue;
			}
			if( S_OK == g_kVariableContainer.Get( EVar_UserManage, EVAR_USERMANAGE_USEANTIHACK_DISCONNECT_2ND, iValue) )
			{
				PgHub::ms_bUseAntiHackDisconnect2nd = iValue;
			}

			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload [EVar_TimeCheck_AntiHack_Min] = ") << PgHub::ms_iAntiHackCheck_Min);
			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload [EVar_TimeCheck_AntiHack_Max] = ") << PgHub::ms_iAntiHackCheck_Max);
			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload [EVar_TimeCheck_AntiHack_Wait] = ") << PgHub::ms_iAntiHackCheck_Wait);
			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload [EVAR_USERMANAGE_USEANTIHACKSERVERBIND] = ") << PgHub::ms_bUseAntiHackServerBind);
			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Reload [EVAR_USERMANAGE_USEANTIHACK_DISCONNECT_2ND] = ") << PgHub::ms_bUseAntiHackDisconnect2nd);
		}
		else
		{
			INFO_LOG(BM::LOG_LV4, __FL__ << _T("Can't Reload [EVAR_USERMANAGE_USEANTIHACKSERVERBIND], Unable AntiHack is Setted"));
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
	}
	return false;
}

bool CALLBACK OnF10(unsigned short const &rusInputKey)
{
	return false;
}


bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG( BM::LOG_LV6, _T("[Switch] will be shutdown"));
	INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));

	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
	return false;
}

bool CALLBACK OnF12(unsigned short const &rusInputKey)
{
	static bool bIsCheck = true;
	
	if(bIsCheck)
	{
// 		bIsCheck = !bIsCheck;
// 		g_kHub.IsGameGuardCSCheck(bIsCheck);

		INFO_LOG( BM::LOG_LV6, __FL__ << _T(" GameGuard CSAuth Toggle [") << bIsCheck << _T("]") );
	}
	return false;
}

bool CALLBACK OnEnd(WORD const& rkInputKey)
{
	// Ctrl + Shift + End
	std::cout << __FUNCTION__ << " / Key:" << rkInputKey << std::endl;
	SHORT sState = GetKeyState(VK_SHIFT);
	if (HIBYTE(sState) == 0)
	{
		return false;
	}
	sState = GetKeyState(VK_CONTROL);
	if (HIBYTE(sState) == 0)
	{
		return false;
	}
	std::cout << "SUCCESS " << __FUNCTION__ << " / Key:" << rkInputKey << std::endl;

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Switch Server will be CRASHED.. are you sure??","DRAGONICA_Switch",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Switch Server will be CRASHED.. are you sure??","DRAGONICA_Switch",MB_OKCANCEL) )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T(" Server crashed by INPUT") );
//			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
			int * p = NULL;
			*p = 1;
		}
	}
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist(VK_ESCAPE,	OnEscape);

	g_kConsoleCommander.Regist(VK_F1,  OnF1);
	g_kConsoleCommander.Regist(VK_F3,  OnF3);
	g_kConsoleCommander.Regist(VK_F4,  OnF4);
	g_kConsoleCommander.Regist(VK_F7,  OnF7);
	g_kConsoleCommander.Regist(VK_F8,  OnF8);
	g_kConsoleCommander.Regist(VK_F9,	OnF9);
	g_kConsoleCommander.Regist(VK_F10, OnF10);
	g_kConsoleCommander.Regist(VK_F11, OnTerminateServer); 
	g_kConsoleCommander.Regist(VK_F12, OnF12); 
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
