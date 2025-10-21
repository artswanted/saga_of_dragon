#include "stdafx.h"
#include "LogWorker_Base.h"
#include "TImeCheck.h"
#include "BM/vstring.h"

PgTimeCheck::PgTimeCheck( PgLogWorker_Base *pkLogWorker, DWORD const iLogType, std::wstring const& kFunction, int const iLineNo, DWORD const dwOverTime, std::wstring const& kAddon)
	:	m_pkLogWorker(pkLogWorker), m_kLogType(iLogType)
{
	StartTime(BM::GetTime32());
	FunctionName(kFunction);
	LineNo(iLineNo);
	AddonStr(kAddon);
	OverTime(dwOverTime);
}

PgTimeCheck::~PgTimeCheck()
{
	if ( m_pkLogWorker )
	{
		DWORD const dwEndTime = BM::GetTime32();

		DWORD const dwGab = dwEndTime - StartTime();

		if(m_kOverTime <= dwGab )
		{
			BM::vstring kStr;
			kStr += FunctionName();
			kStr += _T("-");
			kStr += LineNo();

			kStr += _T(" Time:");
			kStr += (int)dwGab;

			if(AddonStr().size())
			{
				kStr += _T("\n");
				kStr += AddonStr();
			}

			m_pkLogWorker->PushLog(SDebugLogMessage(m_kLogType, BM::LOG_LV0, kStr));
		}
	}
}
