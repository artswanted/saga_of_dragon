#pragma once

#include "BM/STLSupport.h"
#include "BM/ClassSupport.h"


class PgLogWorker_Base;

class PgTimeCheck
{
public:
	explicit PgTimeCheck( PgLogWorker_Base *pkLogWorker, DWORD const iLogType, std::wstring const& kFunction, int const iLineNo, DWORD const dwOverTime = 0, std::wstring const& kAddon = _T(""));
	~PgTimeCheck();

protected:
	CLASS_DECLARATION_S(DWORD, StartTime);
	CLASS_DECLARATION_S(std::wstring, FunctionName);
	CLASS_DECLARATION_S(int, LineNo);
	CLASS_DECLARATION_S(std::wstring, AddonStr);
	CLASS_DECLARATION_S(DWORD, OverTime);

protected:
	PgLogWorker_Base * const m_pkLogWorker;
	DWORD const m_kLogType;
};
