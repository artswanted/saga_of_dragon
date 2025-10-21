#include <WinSock2.h>
#include <Windows.h>
#include "CEL/CEL.h"
#include "CEL/common.h"
#include "cel/corecenter.h"
#include "tinyxml/tinyxml.h"
#include "CEL/DBWorker_Base.h"
#include "libcsv/csv.h"
#include "csvdb/load.h"
#include "CSVQuery.h"
#include "BM/logworker_base.h"
#include "BM/LogWorker.h"

bool CsvQueryIsDBSupport(int iBDIndex)
{
    return true;
}

extern HRESULT InitLog(std::wstring &kRetFolder, std::wstring const &strSub, DWORD const dwMode = BM::OUTPUT_ALL, BM::E_LOG_LEVEL const kLogLimitLv = BM::LOG_LV9, std::wstring const &strRoot = _T("./LogFiles/") );
int _tmain(int argc, _TCHAR* argv[])
{
    g_kCoreCenter;
    g_kLogWorker.StartSvc(1);

	TCHAR chLog[MAX_PATH] = {0,};
	_stprintf_s(chLog, _countof(chLog), _T("DUMMY%04d"), 0);

	std::wstring kLogFolder;
	InitLog(kLogFolder, chLog);

    CEL::DB_QUERY kQuery(0, 0, _T("EXEC [dbo].[UP_LoadDefTacticsLevel]"));
    PushCSVQuery(kQuery, true);
    // ParseQueryFile("Z:/games/projectd/dragonica_master/example/csv_query_simple/UP_LoadDefTacticsLevel.xml");
	g_kLogWorker.VDeactivate();
	g_kCoreCenter.Close();
    return 0;
}


// Impl later

const wchar_t* GetCsvPathByDBType(const int iType)
{
    return _T("");
}