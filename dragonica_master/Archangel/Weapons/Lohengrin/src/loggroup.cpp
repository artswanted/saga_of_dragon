#include "stdafx.h"
#include <tchar.h>
#include "BM/vstring.h"
#include "BM/Localmgr.h"
#include "BM/Point.h"
#include "LogGroup.h"

HRESULT InitLog( std::wstring &kRetFolder, std::wstring const &strSub, DWORD const dwMode, BM::E_LOG_LEVEL const kLogLimitLv, std::wstring const &strRoot )
{
	g_kLogWorker.StartSvc(1);

	std::string strLocale = g_kLocal.GetLocale();
	kRetFolder = strRoot + strSub;

	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_MAIN,			dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"Main_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_CORE,			dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"Core_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_INFO,			dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"Info_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_S_STATE,		dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"SState_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_USER_STATE,	dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"UState_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_CURR_USER,	dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"Count_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_LIVE_CHECK,	dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"LiveCheck_") + strSub,	strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_HEARTBEAT,	dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"HeartBeat_") + strSub,	strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_CHATTING,		dwMode,	BM::LFC_WHITE, kRetFolder,				std::wstring(L"Chat_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_CAUTION,		dwMode,	BM::LFC_WHITE, strRoot + L"CautionLog", std::wstring(L"Caution_") + strSub,		strLocale,	kLogLimitLv) );
	g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_HACKING,		dwMode,	BM::LFC_WHITE, strRoot + L"HackLog",	std::wstring(L"Hack_") + strSub,		strLocale,	kLogLimitLv) );
	return S_OK;
}

void SetLogOutPutType(BM::E_OUPUT_TYPE const kOutPutType)
{
	g_kLogWorker.SetLogEnable( LT_MAIN,			static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_CORE,			static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_INFO,			static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_S_STATE,		static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_USER_STATE,	static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_CURR_USER,	static_cast<DWORD>(kOutPutType) );
//	g_kLogWorker.SetLogEnable( LT_LIVE_CHECK,	static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_HEARTBEAT,	static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_CHATTING,		static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_CAUTION,		static_cast<DWORD>(kOutPutType) );
	g_kLogWorker.SetLogEnable( LT_HACKING,		static_cast<DWORD>(kOutPutType) );
}

void SetLogLeveLimit(BM::E_LOG_LEVEL const eLevel)
{
	g_kLogWorker.SetLogLimitLv( LT_MAIN,		eLevel );
	g_kLogWorker.SetLogLimitLv( LT_CORE,		eLevel );
	g_kLogWorker.SetLogLimitLv( LT_INFO,		eLevel );
	g_kLogWorker.SetLogLimitLv( LT_S_STATE,		eLevel );
	g_kLogWorker.SetLogLimitLv( LT_USER_STATE,	eLevel );
	g_kLogWorker.SetLogLimitLv( LT_CURR_USER,	eLevel );
//	g_kLogWorker.SetLogLimitLv( LT_LIVE_CHECK,	eLevel );
	g_kLogWorker.SetLogLimitLv( LT_HEARTBEAT,	eLevel );
	g_kLogWorker.SetLogLimitLv( LT_CHATTING,	eLevel );
	g_kLogWorker.SetLogLimitLv( LT_CAUTION,		eLevel );
	g_kLogWorker.SetLogLimitLv( LT_HACKING,		eLevel );
}


//bool _ASSERT_INFO_LOG(bool bExp, BM::E_LOG_LEVEL eLevel, BM::vstring const& rkLogMsg)
//{
//#ifndef _DEBUG
//	if ( !bExp )
//	{
//		INFO_LOG( eLevel, _T("-=-=-=-=-=-=-=  ASSERT_INFO_LOG  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
//		INFO_LOG( eLevel, rkLogMsg);
//		INFO_LOG( eLevel, _T("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-="));
//		return false;
//	}
//#endif
//	assert(bExp);
//	return bExp;
//}
//
//bool _VERIFY_INFO_LOG(bool bExp, BM::E_LOG_LEVEL eLevel, BM::vstring const& rkLogMsg) throw(...)
//{
//	VERIFY_INFO_LOG(bExp, eLevel, rkLogMsg);
//	assert(bExp);
//	return bExp;
//}
//
//bool _CHECK_INFO_LOG(bool bExp, BM::E_LOG_LEVEL eLevel, BM::vstring const& rkLogMsg)
//{
//	if( false == bExp )
//	{
//		INFO_LOG(eLevel, rkLogMsg);
//	}
//	return bExp;
//}

void _Call_ASM_INT3()
{
	__asm int 3;
}


CFunctionMonitor::CFunctionMonitor(char const* pFunc, int const Line, int LockCount )
	:m_Func( UNI(pFunc) ), m_Line(Line), m_LockCount(LockCount)
{
	BM::vstring strLog(_T("L "));
	while(LockCount)
	{
		strLog += _T("	");
		--LockCount;
	}
	strLog += _T("[%s]:[%d]");

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL2__(m_Func, m_Line)<<strLog);
}

CFunctionMonitor::~CFunctionMonitor()
{
	int LockCount = m_LockCount;
	BM::vstring strLog(_T("U "));
	while(LockCount)
	{
		strLog += _T("	");
		--LockCount;
	}
	strLog += _T("[%s]:[%d]");

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL2__(m_Func, m_Line)<<strLog);
}

void Callback_InfoLog(BM::E_LOG_LEVEL const LogLv, std::wstring const& kLogMsg)
{
	INFO_LOG(LogLv, kLogMsg);
}

void Callback_CautionLog(BM::E_LOG_LEVEL const LogLv, std::wstring const& kLogMsg)
{
	CAUTION_LOG(LogLv, kLogMsg);
}