#include "stdafx.h"

bool CALLBACK OnEscape( unsigned short const &rkInputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	std::cout << "Press [F11] key to quit" << std::endl;
	return true;
}

bool CALLBACK OnF1(WORD const& rkInputKey)
{
	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO);
	g_kProcessCfg.Locked_DisplayState();

	return false;
}

bool CALLBACK OnF3(WORD const& rkInputKey)
{
	return false;
}

extern void GlobalInit(bool bReload);
bool CALLBACK OnF9( WORD const &rkInputKey)
{
	GlobalInit(true);
	return false;
}

bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG(BM::LOG_LV6, _T("[LoginServer] will be shutdown"));
	INFO_LOG(BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	g_kConsoleCommander.StopSignal(true);
	INFO_LOG(BM::LOG_LV6, _T("=== Shutdown END ===="));
	return false;
}

bool CALLBACK OnEnd(WORD const& rkInputKey)
{
	// Ctrl + Shift + END
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] LoginServer will be CRASHED.. are you sure??","DRAGONICA_Login",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] LoginServer will be CRASHED.. are you sure??","DRAGONICA_Login",MB_OKCANCEL) )
		{
			INFO_LOG(BM::LOG_LV0, __FL__ << _T("Server crashed by INPUT"));
//			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
			int * p = NULL;
			*p = 1;
		}
	}
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1, OnF1 );
	g_kConsoleCommander.Regist( VK_F3, OnF3 );
	g_kConsoleCommander.Regist( VK_F9, OnF9 );
	
	g_kConsoleCommander.Regist( VK_F11, OnTerminateServer );
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}

