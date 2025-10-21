#include "stdafx.h"
#include "ai_Action.h"
#include "Network.h"
#include "DummyClientMgr.h"
#include "DummyMgr2.h"

bool CALLBACK OnEscape( const WORD &InputKey)
{
	std::cout<< "END" << std::endl;
	g_kConsoleCommander.StopSignal(true);
	return false;
}

//! extern CDBConnection g_Conn;

bool CALLBACK OnF1( const WORD &InputKey)
{
	for(int i = 0; i < 100; ++i)
	{
		g_kDummyMgr2.AddUser(L"jjjlost", L"jjjlost");
		g_kDummyMgr2.AddUser(L"yoga3001", L"2Yt32j3jGMFMkXx");
		Sleep(3000);
	}
	// TEST_INFO const * pkInfo = g_kNetwork.GetTestInfo();
	// g_kDummyMgr.Begin(pkInfo->chID, pkInfo->chPassword);
	/*
	TEST_INFO const * pkInfo = g_kNetwork.GetTestInfo();
//	kID += 1 + (rand()%50);
	static int index = pkInfo->sBeginIndex;
	
	int count = (pkInfo->sEndIndex - pkInfo->sBeginIndex);
	while(0 < count-- )
	{
		BM::vstring kID = pkInfo->chID;
		kID += index++;
		g_kDummyMgr.AddNewClient( kID, L"1234");
		Sleep(100);//초당 2개
	}
	*/

	std::cout<< __FUNCTION__ << " end" << std::endl;
	return false;
}

bool CALLBACK OnF6( const WORD &InputKey)
{
	// g_kDummyMgr.InfoLog_Statistic();
	/*
	const TEST_INFO *pkInfo = g_kNetwork.GetTestInfo();

	BM::vstring kID = pkInfo->chID;

	static int i = 1;
	kID += i++;

	g_kDummyMgr.AddNewClient(kID, L"1234", E_AI_TYPE_PVP);
	INFO_LOG( BM::LOG_LV6, _T("[%s] Add PvP Client"), pkInfo->chID );
	*/
	return false;
}

bool CALLBACK OnF4( const WORD &InputKey)
{
	/*
	const TEST_INFO *pkInfo = g_kNetwork.GetTestInfo();

	for(int i = 1; 500 > i;i++)
	{
		BM::vstring kID = pkInfo->chID;
		kID += i;
		g_kDummyMgr.AddNewClient(kID, L"1234");
	}
	*/
	return false;
}


bool CALLBACK OnF2(const WORD &InputKey)
{
	// g_kDummyMgr.CloseAll(true);
	return false;
}

bool CALLBACK OnF3(const WORD &InputKey)
{
	std::cout<< "OnF3" << std::endl;

	printf("Total Session = %d\n", g_kNetwork.m_iTotalSessionCount);

//	g_kCoreCenter.vdis
	return false;
}

bool CALLBACK OnPlus(const WORD &InputKey)
{
	return false;
}

bool CALLBACK OnMinus(const WORD &InputKey)
{
	return false;
}

bool CALLBACK OnTerminateServer(const WORD& rkInputKey)
{
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));
	INFO_LOG(BM::LOG_LV6, _T("[Dummy] will be shutdown"));
	INFO_LOG(BM::LOG_LV6, _T("\tIt takes some times depens on system....WAITING..."));
	INFO_LOG(BM::LOG_LV6, _T("==========================================================="));

	g_kConsoleCommander.StopSignal(true);
	INFO_LOG(BM::LOG_LV6, _T("=== Shutdonw END ===="));
	return false;
}

bool RegistKeyEvent()
{
	g_kConsoleCommander.Regist( VK_ESCAPE,	OnEscape );
	g_kConsoleCommander.Regist( VK_F1,		OnF1 );
	g_kConsoleCommander.Regist( VK_F2,		OnF2 );
	g_kConsoleCommander.Regist( VK_F3,		OnF3 );
	g_kConsoleCommander.Regist( VK_F4,		OnF4 );
	g_kConsoleCommander.Regist( VK_F6,		OnF6 );
	g_kConsoleCommander.Regist( VK_F11,	OnTerminateServer );

	g_kConsoleCommander.Regist( VK_ADD,		OnPlus );//숫자판
	g_kConsoleCommander.Regist( VK_SUBTRACT,	OnMinus );//숫자판
	return true;
}
