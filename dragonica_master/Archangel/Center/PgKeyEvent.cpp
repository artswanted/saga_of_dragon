#include "stdafx.h"
#include "PgTimer.h"

bool CALLBACK OnEscape( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	std::cout << "Press [F11] key to quit" << std::endl;

	return false;
}

bool CALLBACK OnF1( WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	
	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO );
	g_kProcessCfg.Locked_DisplayState();
	g_kServerSetMgr.Locked_DisplayState();

	return false;
}

bool CALLBACK OnF2(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	MAIN_LOG( BM::LOG_LV0, _T("=========================================="));
	//ј­№ц »уЕВ.
	g_kServerSetMgr.Locked_DisplayState();

	return false;
}

bool CALLBACK OnF3(WORD const &InputKey)
{
	/*
	std::vector<int> kValue(100, 0);
	kValue.resize(100);
	int i = 0;
	PgPlayer kUnit;
	kUnit.SetID(BM::GUID::Create());
	INFO_LOG(BM::LOG_LV7, _T("Guid=") << kUnit.GetID());
	DWORD dwBegin = BM::GetTime32();
	while (i++ < 10000)
	{
		int iValue = kUnit.GetRandom(dwBegin) % 100;
		kValue[iValue] = kValue[iValue] + 1;
	}
	INFO_LOG(BM::LOG_LV7, _T("New GetRandom Time=") << BM::GetTime32()-dwBegin);
	
	for (int j= 0; j<100; j++)
	{
		INFO_LOG(BM::LOG_LV7, j << _T(" = ") << kValue[j]);
	}

	INFO_LOG(BM::LOG_LV7, _T("=========================="));
	std::vector<int> kValue2(100, 0);
	kValue2.resize(100);
	i=0;
	dwBegin = BM::GetTime32();
	while (i++ < 10000)
	{
		int iValue = kUnit.GetRandom(dwBegin) % 100;
		kValue2[iValue] = kValue2[iValue] + 1;
	}
	INFO_LOG(BM::LOG_LV7, _T("Old GetRandom Time=") << BM::GetTime32()-dwBegin);

	for (int j= 0; j<100; j++)
	{
		INFO_LOG(BM::LOG_LV7, j << _T(" = ") << kValue2[j]);
	}
	*/
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

bool CALLBACK OnF5(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	return false;
}

bool CALLBACK OnF8(WORD const &InputKey)
{
	return false;
}

bool CALLBACK OnPlus(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;

	size_t const ret = __min(g_kSwitchAssignMgr.MaxPlayerCount()+100, 1500 );
	g_kSwitchAssignMgr.MaxPlayerCount( ret );
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("UserCount ") << g_kSwitchAssignMgr.MaxPlayerCount() );
	return true;
}

bool CALLBACK OnMinus(WORD const &InputKey)
{
	std::cout<< __FUNCTION__ << std::endl;
	
	size_t const ret = __max((int)g_kSwitchAssignMgr.MaxPlayerCount()-100, 0 );
	g_kSwitchAssignMgr.MaxPlayerCount( ret );
	INFO_LOG( BM::LOG_LV0, __FL__ << _T("UserCount ") << g_kSwitchAssignMgr.MaxPlayerCount() );
	return true;
}

bool CALLBACK OnTerminateServer(WORD const& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("Trying to [CenterServer shutdown]"));

	g_kServerSetMgr.Locked_ShutDown();
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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Center Server will be CRASHED.. are you sure??","DRAGONICA_Center",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Center Server will be CRASHED.. are you sure??","DRAGONICA_Center",MB_OKCANCEL) )
		{
			INFO_LOG( BM::LOG_LV6, __FL__ << _T("Server crashed by INPUT") );
			::RaiseException(1,  EXCEPTION_NONCONTINUABLE_EXCEPTION, 0, NULL);
		}
	}
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1,		OnF1 );
	g_kConsoleCommander.Regist( VK_F2,		OnF2 );
	g_kConsoleCommander.Regist( VK_F3,		OnF3 );
	g_kConsoleCommander.Regist( VK_F4,		OnF4 );
	g_kConsoleCommander.Regist( VK_F5,		OnF5 );
	g_kConsoleCommander.Regist( VK_F8,		OnF8 );
	g_kConsoleCommander.Regist( VK_F11,	OnTerminateServer );

	g_kConsoleCommander.Regist( VK_ADD,		OnPlus );//јэАЪЖЗ
	g_kConsoleCommander.Regist( VK_SUBTRACT,	OnMinus );//јэАЪЖЗ
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}
