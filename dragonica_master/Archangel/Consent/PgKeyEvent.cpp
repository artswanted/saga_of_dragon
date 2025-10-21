#include "stdafx.h"
#include "PgGFServer.h"
#include "GalaServer.h"
#include "MD5.h"
#include "PgNCServer.h"

bool CALLBACK OnTerminateServer(WORD const& wExitCode)
{
	INFO_LOG( BM::LOG_LV6, _T("===========================================================") );
	INFO_LOG( BM::LOG_LV6, _T("[CenterServer] will be shutdown ExitCode[") << wExitCode << _T("]") );
	INFO_LOG( BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING...") );
	INFO_LOG( BM::LOG_LV6, _T("===========================================================") );

	g_kProcessCfg.ExitCode( g_kProcessCfg.ExitCode() | wExitCode );
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("ExitCode<") << g_kProcessCfg.ExitCode() << _T(">") );

	g_kConsoleCommander.StopSignal(true);
	INFO_LOG( BM::LOG_LV6, _T("=== Shutdown END ====") );
	return true;
}

bool CALLBACK OnEscape( WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, __FUNCTIONW__ );
	std::cout << "Press [F11] key to quit" << std::endl;
	return true;
}

bool CALLBACK OnF11( WORD const &InputKey)
{
	return OnTerminateServer(E_Terminate_By_Console);
}

bool CALLBACK OnF1( WORD const &InputKey)
{
	if ( g_kLocal.IsServiceRegion( LOCAL_MGR::NC_TAIWAN ) )
	{
		g_kGFServer.Locked_LoadFromConfig();
	}
	
	return true;
}

bool CALLBACK OnF2( WORD const &InputKey)
{

	INFO_LOG( BM::LOG_LV0, _T("================================================"));
	INFO_LOG( BM::LOG_LV0, _T("================================================"));
	return true;
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Contents Server will be CRASHED.. are you sure??","DRAGONICA_Contents",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Contents Server will be CRASHED.. are you sure??","DRAGONICA_Contents",MB_OKCANCEL) )
		{
			INFO_LOG( BM::LOG_LV0, __FL__ << _T("Server crashed by INPUT") );
//			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
			int * p = NULL;
			*p = 1;
		}
	}
	return false;
}

bool CALLBACK OnOnOffNCMon(WORD const& rkInputKey)
{
	NCMonUtil::bUseMonitor = !NCMonUtil::bUseMonitor;
	INFO_LOG(BM::LOG_LV1, __FL__ << L"NC Server Mon new status["<<NCMonUtil::bUseMonitor<<L"]");
	return false;
}

void RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1, OnF1 );
	g_kConsoleCommander.Regist( VK_F2, OnF2 );
	g_kConsoleCommander.Regist( VK_F11,		OnF11 );
	g_kConsoleCommander.Regist( VK_END,			OnEnd );
	g_kConsoleCommander.Regist( VK_OEM_6,	OnOnOffNCMon );	// ]
}
