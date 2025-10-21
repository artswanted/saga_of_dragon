#pragma once

#include "BM/LogWorker.h"

class CCelLog
{
public:
	CCelLog();
	~CCelLog();

	typedef void (*LPFN_LOGFUNCTION)(BM::E_LOG_LEVEL const LogLv, std::wstring const& kLogMsg);

	bool Init(DWORD const OutputMode, BM::E_LOG_FONT_COLOR const eFontColor, std::wstring const& wstrFolderName = _T("./Log"), std::wstring const& wstrFileName = _T("Log.txt"), std::string const& InLocale = "eng");
	void Init(DWORD const dwInfoLogType, DWORD const dwCautionLogType);

	DWORD InfoLogType()const{return m_dwInfoLogType;}
	DWORD CautionLogType()const{return m_dwCautionLogType;}
private:
	DWORD m_dwInfoLogType;
	DWORD m_dwCautionLogType;
};

#define g_kCelLog SINGLETON_STATIC(CCelLog)

#define CEL_INFO_LOG(Lv, Msg) {SDebugLogMessage kMsg(g_kCelLog.InfoLogType(), Lv, (BM::vstring() << Msg).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CEL_CAUTION_LOG(Lv, Msg) {SDebugLogMessage kMsg(g_kCelLog.CautionLogType(), Lv, (BM::vstring() << Msg).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}
#define CEL_LOG(Lv, Msg) CEL_INFO_LOG(Lv, Msg)