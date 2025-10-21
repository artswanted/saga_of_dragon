#include "stdafx.h"
#include "PgMMCManager.h"
#include "PgKeyEvent.h"

bool CALLBACK OnEscape( WORD const &InputKey)
{
	std::cout<< "Can't exit Machine Controller service." << std::endl;
	return true;
}

bool CALLBACK OnTerminateServer( WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG( BM::LOG_LV6, _T("[MachineControl] will be shutdown"));
	INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG( BM::LOG_LV6, _T("==========================================================="));
	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
	return false;
}

bool CALLBACK OnF1(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("Reload Auth Info") );
	g_kMMCMgr.Locked_ReadFromConfig( L"./MMC.ini", true );
	return true;
}

bool CALLBACK OnF5(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("RefreshState Start") );

	g_kMMCMgr.Locked_ReadFromConfig( L"./MMC.ini", false );
	g_kMMCMgr.Locked_LoadPatchFileList();

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("RefreshState Complete") );
	return true;
}

bool CALLBACK OnF8(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("ReSync Order Start") );

	g_kMMCMgr.Locked_LoadPatchFileList();

	INFO_LOG( BM::LOG_LV6, __FL__ << _T("ReSync Order Complete") );
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] MMC Server will be CRASHED.. are you sure??","DRAGONICA_MMC",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] MMC Server will be CRASHED.. are you sure??","DRAGONICA_MMC",MB_OKCANCEL) )
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
	g_kConsoleCommander.Regist( VK_F1,		OnF1);
	g_kConsoleCommander.Regist( VK_F5,		OnF5);
	g_kConsoleCommander.Regist( VK_F8,		OnF8);
	g_kConsoleCommander.Regist( VK_F11,	OnTerminateServer );
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
