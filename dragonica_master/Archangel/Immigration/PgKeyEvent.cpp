#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/PgRealmManager.h"
#include "PgWaitingLobby.h"
#include "PgKeyEvent.h"
#include "PgSiteMgr.h"
#include "constant.h"

//extern void ReloadAntiHackMgr();

bool CALLBACK OnEscape( WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, __FL__ );

	std::cout << "Press [F11] key to quit" << std::endl;
	return false;
}

bool CALLBACK OnF1( WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("[") << __FL__ << _T("]") );
	
	g_kCoreCenter.DisplayState( g_kLogWorker, LT_INFO);
	g_kProcessCfg.Locked_DisplayState();
	g_kWaitingLobby.DisplayState();
	return false;
}

bool CALLBACK OnF2(WORD const &InputKey)
{
	CONT_REALM_CANDIDATE kContRealmCandi;
	g_kSiteMgr.Get(kContRealmCandi);
	CONT_REALM_CANDIDATE::const_iterator itor_realmcandi = kContRealmCandi.begin();
	while (itor_realmcandi != kContRealmCandi.end())
	{
		if ((*itor_realmcandi).first > 0)
		{
			CEL::DB_QUERY kQuery( DT_SITE_CONFIG, DQT_LOAD_CHANNEL_NOTICE, _T("EXEC [dbo].[up_LoadChannelNotice]"));
			kQuery.PushStrParam((*itor_realmcandi).second.Notice_TblName());
			kQuery.PushStrParam((*itor_realmcandi).first);
			g_kCoreCenter.PushQuery(kQuery, true);
		}

		++itor_realmcandi;
	}

	BM::Stream kCPacket(PT_IM_A_NFY_CHANNEL_NOTICE);
	g_kRealmMgr.WriteToPacket(kCPacket, 0, ERealm_SendType_Sync_Notice);
	SendToServerType(CEL::ST_LOGIN, kCPacket);
	SendToServerType(CEL::ST_CONTENTS, kCPacket);

	return false;
}

bool CALLBACK OnF3(WORD const &InputKey)
{
	/*
	int const iHour = 26;
	BM::DBTIMESTAMP_EX kLocalTime;
	CGameTime::GetLocalTime(kLocalTime);
	BM::PgPackedTime kPackedTime = kLocalTime;
	kPackedTime.Hour(iHour % 24);
	if (iHour > 24 && kLocalTime.hour > kPackedTime.Hour())
	{
		CGameTime::AddTime(kPackedTime, OneDay);
	}
	BM::DBTIMESTAMP_EX kLimitTime(kPackedTime);
	__int64 const iDiffSec = CGameTime::GetElapsedTime(kLocalTime, kLimitTime, CGameTime::SECOND);
	*/
	return false;
}

bool CALLBACK OnF5(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("[") << __FL__ << " Reload Config " << _T("]") );
	short nExtVar = 0;
	g_kVariableContainer.Get(EVar_Kind_Login, EVAR_Login_Check_HaveExtVarValue, nExtVar );//
	g_kWaitingLobby.UseExtVar(nExtVar != EXT_VAR_NONE);
	g_kWaitingLobby.HaveExtVar(nExtVar);
	/*
	INFO_LOG(BM::LOG_LV6, _T("[%s]"), __FUNCTIONW__);
	std::cout << "Start GSM Test" << std::endl;

	int iUID = 0;
	int iBirth = 0;
	PortalFlag kPotalFlag = {0,};
	GameFlag kGameFlag = {0,};
	char* pWebSID = 0;

	__int64 iMacAddr;
	BM::MacAddress(iMacAddr);
	BYTE abyOutID[16] = {0,};

	int iIPAddr;
	BM::IPAddress(iIPAddr);


	std::wstring kId = L"angel988";
	std::wstring kPW = L"aaaa1111";

	int iRet = LoginWithIDPWD((wchar_t*)kId.c_str(), (wchar_t*)kPW.c_str(), iMacAddr, iIPAddr, abyOutID, &iUID, &kPotalFlag, &kGameFlag, &iBirth);

	if(iRet > 0)
	{
		wchar_t szErrBuf[1024] = {0,};
		int MsgLen = 0;
		GetErrorMsg(iRet, sizeof(szErrBuf), szErrBuf, MsgLen);
		INFO_LOG(BM::LOG_BLUE, _T("%s"), szErrBuf);
	}
	*/

	return true;
}

bool CALLBACK OnF7(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("[") << __FL__ << _T("]") );

	/*
	int iCount = 0;
	DWORD const dwBegin = BM::GetTime32();
	wchar_t ret_buf[300];
	while (++iCount < 9999999)
	{
		wchar_t wszTime[20] = {0,};
		::_wstrtime_s(wszTime, 20 );
		::swprintf_s(ret_buf, 300, L"%s", wszTime);
	}
	INFO_LOG(BM::LOG_LV7, __FL__ << _T("_wstrtime_s ElapsedTime=") << BM::GetTime32() - dwBegin);

	DWORD dwBegin2 = BM::GetTime32();
	iCount = 0;
	while (++iCount < 9999999)
	{
		SYSTEMTIME kSystem;
		::GetLocalTime(&kSystem);
	}
	INFO_LOG(BM::LOG_LV7, __FL__ << _T("GetLocalTime ElapsedTime=") << BM::GetTime32() - dwBegin2);


	dwBegin2 = BM::GetTime32();
	iCount = 0;
	while (++iCount < 9999999)
	{
		time_t ltime;
		tm newtime;
		time(&ltime);
		localtime_s(&newtime, &ltime);
	}
	INFO_LOG(BM::LOG_LV7, __FL__ << _T("localtime_s() ElapsedTime=") << BM::GetTime32() - dwBegin2);
	*/

	return false;
}

bool CALLBACK OnF8(WORD const &InputKey)
{
	/*
	int iCount = 0;
	DWORD const dwBegin = BM::GetTime32();
	wchar_t ret_buf[300];
	while (++iCount < 9999999)
	{
		//ACE_Time_Value tv(0, 100);	tv += ACE_OS::gettimeofday();
		//struct _timeb timebuffer;timebuffer.
		//_ftime64_s( &timebuffer );
		//wchar_t wTime[40];
		//_wctime64_s( wTime, 40, & ( timebuffer.time ) );
		//swprintf_s(ret_buf, 300, L"%s.%hu", wTime);

		//SYSTEMTIME kSystemTime;
		//GetLocalTime(&kSystemTime);
		//_stprintf_s(ret_buf, 300, _T("%02d/%02d %02d:%02d:%02d.%04d"), kSystemTime.wMonth, kSystemTime.wDay, kSystemTime.wHour, 
		//	kSystemTime.wMinute, kSystemTime.wSecond, kSystemTime.wMilliseconds);
	}
	INFO_LOG(BM::LOG_LV7, __FL__ << _T("_wstrtime_s ElapsedTime=") << BM::GetTime32() - dwBegin);
	*/

	return false;
}

bool CALLBACK OnPlus(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("[") << __FL__ << _T("]") );
	return true;
}

bool CALLBACK OnMinus(WORD const &InputKey)
{
	INFO_LOG( BM::LOG_LV6, _T("[") << __FL__ << _T("]") );

	return true;
}

bool CALLBACK OnF11(WORD const &InputKey)
{
	return OnTerminateServer(E_Terminate_By_Console);
}

bool CALLBACK OnTerminateServer(WORD const& wExitCode)
{
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("===========================================================") );
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("[ImmigrationServer] will be shutdown ExitCode[") << wExitCode << _T("]") );
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("\tIt takes some times depens on system....WAITING...") );
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("===========================================================") );

	OnCleanupService(wExitCode);

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

	if ( IDOK == ::MessageBoxA(NULL,"[WARNING] Immigration Server will be CRASHED.. are you sure??","DRAGONICA_Immigration",MB_OKCANCEL) )
	{
		if ( IDOK == ::MessageBoxA(NULL,"[WARNING][WARNING] Immigration Server will be CRASHED.. are you sure??","DRAGONICA_Immigration",MB_OKCANCEL) )
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
	g_kConsoleCommander.Regist( VK_F1,		OnF1 );
	g_kConsoleCommander.Regist( VK_F2,		OnF2 );
	g_kConsoleCommander.Regist( VK_F3,		OnF3 );
	g_kConsoleCommander.Regist( VK_F5,		OnF5 );
	g_kConsoleCommander.Regist( VK_F7,		OnF7 );
	g_kConsoleCommander.Regist( VK_F8,		OnF8 );
	g_kConsoleCommander.Regist( VK_F11,		OnF11 );

	g_kConsoleCommander.Regist( VK_ADD,		OnPlus );//숫자판
	g_kConsoleCommander.Regist( VK_SUBTRACT,	OnMinus );//숫자판
	g_kConsoleCommander.Regist( VK_END,	OnEnd );
	return true;
}

// Server를 내리기 전에 정리하는 작업하기
bool CALLBACK OnCleanupService(WORD const& wExitCode)
{
	g_kProcessCfg.ExitCode(g_kProcessCfg.ExitCode() | wExitCode);
	INFO_LOG( BM::LOG_LV0, __FL__ << _T(" ExitCode[") << g_kProcessCfg.ExitCode() << _T("]") );

	// Do Cleanup .............
	return true;
}

