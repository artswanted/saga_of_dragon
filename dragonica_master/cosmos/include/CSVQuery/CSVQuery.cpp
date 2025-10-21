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

#define DB_LOG(a,b)		{SDebugLogMessage kMsg(2, a, (BM::vstring() << b).operator std::wstring const&()); g_kLogWorker.PushLog(kMsg);}

typedef struct tagCsvQueryColumn
{
    tagCsvQueryColumn() :sName(""), eType(DBTYPE_EMPTY) { }
    std::string sName;
    DBTYPEENUM eType;
} CsvQueryColumn;

typedef struct tagCsvQuery
{
    std::string sTableName;
    std::vector<tagCsvQueryColumn> vColumn;
} CsvQuery;

static HRESULT ExecSingleQuery(CsvQuery const &kQuery, CEL::DB_RESULT & rkResult)
{
    BM::Stream::STREAM_DATA buff;
    CsvParser csv;
    if (!csvdb::load(buff, csv, BM::vstring(kQuery.sTableName)))
    {
        DB_LOG(BM::LOG_LV2, "Failed open csv file '" << kQuery.sTableName << "'");
        rkResult.eRet = CEL::DR_ERR_CREATE_CONN;
        return E_FAIL;
    }

    const size_t iSize = kQuery.vColumn.size();
    std::vector<int> kColIndex(iSize);
    for( size_t i = 0; i < iSize; ++i )
    {
        if ( (kColIndex[i] = csv.col_idx(kQuery.vColumn[i].sName.c_str())) < 0)
        {
            DB_LOG(BM::LOG_LV2, "Invalid column '" << kQuery.vColumn[i].sName << "'");
            rkResult.eRet = CEL::DR_ERR_BIND_COLUMN;
            return E_FAIL;
        }
    }

    int iQueryResultCount = 0;
    while (csv.next_row())
    {
        for ( size_t i = 0; i < iSize; ++i )
        {
            CEL::DB_DATA kDBData;
			kDBData.DataType = kQuery.vColumn[i].eType;
            int iDataSize = CEL::CManualQuery::GetBindSize(kDBData.DataType);
            switch (kDBData.DataType)
            {
            case DBTYPE_R4:  { float fValue         = (float)atof(csv.col(kColIndex[i]));kDBData.Push(&fValue, iDataSize); } break;
            case DBTYPE_I4:  { int iValue           = atoi(csv.col(kColIndex[i]));      kDBData.Push(&iValue, iDataSize); } break;
            case DBTYPE_UI1: { BYTE iValue          = atoi(csv.col(kColIndex[i]));      kDBData.Push(&iValue, iDataSize); } break;
            case DBTYPE_I2:  { short iValue         = atoi(csv.col(kColIndex[i]));      kDBData.Push(&iValue, iDataSize); } break;
            case DBTYPE_I8:  { __int64 iValue       = _atoi64(csv.col(kColIndex[i]));   kDBData.Push(&iValue, iDataSize); } break;
            case DBTYPE_STR: { const char* sValue   = csv.col(kColIndex[i]);            kDBData.Push(sValue, ::strlen(sValue)); } break;
            case DBTYPE_WSTR:{ std::wstring sValue  = UNI(csv.col(kColIndex[i]));       kDBData.Push(sValue.c_str(), sValue.length() * sizeof(std::wstring::value_type)); } break;
            case DBTYPE_GUID:{ BM::GUID sValue      = BM::GUID(csv.col(kColIndex[i]));  kDBData.Push(sValue, iDataSize); } break;
            case DBTYPE_DBTIMESTAMP:
                {
                    BM::DBTIMESTAMP_EX kValue;
                    kValue.DateFromString("%d-%d-%d %d:%d:%d", csv.col(kColIndex[i]));
                    kDBData.Push(&kValue, iDataSize);
                } break;
            default:
                {
                    DB_LOG(BM::LOG_LV2, "Invalid column type '" << kDBData.DataType << "'");
                    rkResult.eRet = CEL::DR_ERR_PARTIAL;
                    return E_FAIL;
                }
            }
            rkResult.vecArray.push_back( kDBData );
        }
        ++iQueryResultCount;
    }

    rkResult.vecResultCount.push_back(iQueryResultCount);
    return S_OK;
}

static HRESULT ExecQuery(std::vector<CsvQuery> const &kQueryArr, CEL::DB_QUERY & kCelQuery)
{
    CEL::DB_RESULT rkResult(kCelQuery);
    rkResult.eRet = CEL::DR_SUCCESS;

    std::vector<CsvQuery>::const_iterator it = kQueryArr.begin();
    for(; it != kQueryArr.end(); ++it)
    {
        HRESULT hRet = ExecSingleQuery(*it, rkResult);
        if (hRet != S_OK)
        {
            return hRet;
        }
    }

    rkResult.eRet = ((rkResult.vecArray.empty())?CEL::DR_NO_RESULT:rkResult.eRet);
    if (kCelQuery.QueryResultCallback())
        return kCelQuery.QueryResultCallback()(rkResult);
    return g_kCoreCenter.ExecDBResult(rkResult);
}

static HRESULT ParseSignleQuery(TiXmlElement *kQueryElement, CsvQuery &kQuery)
{
    if (!kQueryElement || strcmp(kQueryElement->Value(), "QUERY") != 0)
    {
        DB_LOG(BM::LOG_LV2, "Query file isn't correct! Can't find section 'QUERY'");
        return E_FAIL;
    }

    const char* pkTableName = kQueryElement->Attribute("TABLE");
    if (!pkTableName)
    {
        DB_LOG(BM::LOG_LV2, "Can't find 'TABLE' attribute for 'QUERY' section");
        return E_FAIL;
    }

    kQuery.sTableName = pkTableName;

    TiXmlElement *kColumnElement = kQueryElement->FirstChildElement();
    while (kColumnElement != NULL)
    {
        if (!strcmp(kColumnElement->Value(), "COLUMN"))
        {
            CsvQueryColumn kCol;
            TiXmlAttribute *pkAttribute = kColumnElement->FirstAttribute();
            while (pkAttribute != NULL)
            {
                const char* kName = pkAttribute->Name();
                const char* kValue = pkAttribute->Value();
                if ( !::strcmp( kName, "NAME") )
                {
                    kCol.sName = kValue;
                }
                else if ( !::strcmp( kName, "TYPE") )
                {
                    if      ( !::_stricmp(kValue, "INT") )        { kCol.eType = DBTYPE_I4; }
                    else if ( !::_stricmp(kValue, "STRING") )     { kCol.eType = DBTYPE_STR; }
                    else if ( !::_stricmp(kValue, "WSTRING") )    { kCol.eType = DBTYPE_WSTR; }
                    else if ( !::_stricmp(kValue, "SMALLINT") )   { kCol.eType = DBTYPE_I2; }
                    else if ( !::_stricmp(kValue, "BIGINT") )     { kCol.eType = DBTYPE_I8; }
                    else if ( !::_stricmp(kValue, "TINYINT") )    { kCol.eType = DBTYPE_UI1; }
                    else if ( !::_stricmp(kValue, "REAL") )       { kCol.eType = DBTYPE_R4; }
                    else if ( !::_stricmp(kValue, "DATETIME") )   { kCol.eType = DBTYPE_DBTIMESTAMP; }
                    else if ( !::_stricmp(kValue, "UNIQUEIDENTIFIER") )   { kCol.eType = DBTYPE_GUID; }
                    else
                    {
                        DB_LOG(BM::LOG_LV2, "Undefined column type '" << kValue << "'");
                        return E_FAIL;
                    }
                }
                else
                {
                    DB_LOG(BM::LOG_LV2, "Undefined attribute '" << kName << "'");
                    return E_FAIL;
                }
                pkAttribute = pkAttribute->Next();
            }

            if (kCol.eType != DBTYPE_EMPTY)
            {
                kQuery.vColumn.push_back(kCol);
            }
        }
        else
        {
            DB_LOG(BM::LOG_LV2, "Undefined element type '" << kColumnElement->Value() << "'");
            return E_FAIL;
        }
        kColumnElement = kColumnElement->NextSiblingElement();
    }

    if (kQuery.vColumn.size() == 0)
    {
        DB_LOG(BM::LOG_LV2, "Can't execute empty query!");
        return E_FAIL;
    }

    return S_OK;
}

static HRESULT ParseAndExecQueryFile(BM::vstring const& kFilePath, CEL::DB_QUERY & kCelQuery)
{
    TiXmlDocument kXmlDoc(MB(kFilePath));
    if (!kXmlDoc.LoadFile())
    {
        DB_LOG(BM::LOG_LV6, "Failed load xml file: '" << kFilePath << "' " << kXmlDoc.ErrorDesc());
        return ERROR_FILE_NOT_FOUND;
    }

    std::vector<CsvQuery> kQueryArray;

    TiXmlElement *kQueryElement = kXmlDoc.FirstChildElement();
    while (kQueryElement)
    {
        CsvQuery kQuery;
        HRESULT hRet = ParseSignleQuery(kQueryElement, kQuery);
        if (hRet != S_OK)
        {
            return hRet;
        }

        kQueryArray.push_back(kQuery);
        kQueryElement = kQueryElement->NextSiblingElement();
    }

    return ExecQuery(kQueryArray, kCelQuery);
}

HRESULT PushCSVQuery(CEL::DB_QUERY & kQuery, bool bIsImmidiate)
{
    if (bIsImmidiate == false)
    {
        DB_LOG(BM::LOG_LV5, _T("CSV Query now support only Immidiate type"));
        return E_FAIL;
    }

    if (!CsvQueryIsDBSupport(kQuery.DBIndex()))
    {
        return g_kCoreCenter.PushQuery(kQuery, bIsImmidiate);
    }

    BM::vstring vstrQueryPath;
    vstrQueryPath += GetCsvPathByDBType((kQuery.DBIndex()));
    vstrQueryPath += kQuery.GetProc();
    vstrQueryPath += _T(".xml");
    HRESULT hRet = ParseAndExecQueryFile(vstrQueryPath, kQuery);
    if (hRet == ERROR_FILE_NOT_FOUND)
    {
        DB_LOG(BM::LOG_LV5, _T("Query '") << kQuery.GetProc() << "' is database. Rewrite it to DBQuery");
        return g_kCoreCenter.PushQuery(kQuery, bIsImmidiate);
    }

    if (hRet != S_OK)
    {
        DB_LOG(BM::LOG_LV5, _T("Failed execute csv query '") << vstrQueryPath << "' see log below");
    }
    return hRet;
}
