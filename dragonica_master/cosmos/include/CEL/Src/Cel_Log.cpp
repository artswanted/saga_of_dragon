#include "stdafx.h"
#include "Cel_Log.h"

CCelLog::CCelLog()
{
	m_dwInfoLogType = LWT_CEL_IN;
	m_dwCautionLogType = LWT_CEL_IN;// LWT_CEL_CA 이 아니다.
}

CCelLog::~CCelLog()
{
}

bool CCelLog::Init(DWORD const OutputMode, BM::E_LOG_FONT_COLOR const eFontColor, std::wstring const& wstrFolderName, std::wstring const& wstrFileName,
				   std::string const& InLocale)
{
	return SUCCEEDED(g_kLogWorker.VRegistInstance(REG_LOG_INSTANCE(LWT_CEL_IN, OutputMode, eFontColor, wstrFolderName, wstrFileName, InLocale)));
}

void CCelLog::Init(DWORD const dwInfoLogType, DWORD const dwCautionLogType)
{	// Lock 개체가 없다, 반드시 CEL초기화 호출해 주어야 한다.
	m_dwInfoLogType = dwInfoLogType;
	m_dwCautionLogType = dwCautionLogType;
}
