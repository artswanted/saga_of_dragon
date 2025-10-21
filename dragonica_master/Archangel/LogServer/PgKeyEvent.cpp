#include "stdafx.h"
#include "CEL/ConsoleCommander.h"

bool CALLBACK OnEscape( unsigned short const &rkInputKey)
{
	std::cout<< __FUNCTIONW__ << std::endl;
	g_kConsoleCommander.StopSignal(true);
	return true;
}


bool CALLBACK OnF1( unsigned short const &rkInputKey)
{
	std::cout<< __FUNCTIONW__ << std::endl;

	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO );
	g_kProcessCfg.Locked_DisplayState();

	return true;
}

bool CALLBACK OnF2( unsigned short const &rkInputKey)
{
	std::cout<< __FUNCTIONW__ << std::endl;
	
	INFO_LOG( BM::LOG_LV7, __FL__ << _T(" Reserve Inv") );

	return true;
}

bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
	INFO_LOG( BM::LOG_LV6, _T("[LogServer] will be shutdown") );
	INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING...") );
	INFO_LOG( BM::LOG_LV6, _T("===========================================================") );

	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] LogServer will be CRASHED.. are you sure??","DRAGONICA_Log",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] LogServer will be CRASHED.. are you sure??","DRAGONICA_Log",MB_OKCANCEL) )
		{
			INFO_LOG( BM::LOG_LV0, _T(" Server crashed by INPUT") );
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
	g_kConsoleCommander.Regist( VK_F1,	OnF1 );
	g_kConsoleCommander.Regist( VK_F2,	OnF2 );
	g_kConsoleCommander.Regist( VK_F11, OnTerminateServer );
	
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
