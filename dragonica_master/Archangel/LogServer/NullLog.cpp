#include "stdafx.h"
#include "BM/LocalMgr.h"
#include "NullLog.h"

PgNullLog::PgNullLog( bool const bInterNalTest )
:	m_bInternalTest(bInterNalTest)
{
}

PgNullLog::~PgNullLog()
{
}

bool PgNullLog::LogInit(BM::E_OUPUT_TYPE const eOutType, std::wstring const& strFolder, std::wstring const &strFile)
{
	return g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LT_FILE_LOG_NC, eOutType, BM::LFC_WHITE, strFolder, strFile, g_kLocal.GetLocale()) );
}
