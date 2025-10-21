#include "stdafx.h"
#include "PgSMCMgr.h"

bool CALLBACK OnEscape( WORD const &InputKey)
{
#ifdef _DEBUG
	g_kConsoleCommander.StopSignal(true);
#else
	std::cout<< "Can't exit Machine Controller service." << std::endl;
#endif
	return false;
}


bool CALLBACK OnTerminateServer( WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG( BM::LOG_LV6, _T("[SubMachineControl] will be shutdown"));
	INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));
	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] SMC Server will be CRASHED.. are you sure??","DRAGONICA_SMC",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] SMC Server will be CRASHED.. are you sure??","DRAGONICA_SMC",MB_OKCANCEL) )
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
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	//ConsoleCommander.Regist( VK_F1,		OnF1);
	g_kConsoleCommander.Regist( VK_F11,	OnTerminateServer );
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
