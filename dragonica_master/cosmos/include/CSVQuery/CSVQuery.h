#pragma once

// Must be override in app
extern const wchar_t* GetCsvPathByDBType(const int iType);
extern bool CsvQueryIsDBSupport(int iBDIndex);
HRESULT PushCSVQuery(CEL::DB_QUERY & kQuery, bool bIsImmidiate);