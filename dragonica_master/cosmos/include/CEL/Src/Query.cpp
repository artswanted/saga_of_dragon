#include "stdafx.h"
#include "../query.h"
#include "BM/vstring.h"
#include "BM/ThreadObject.h"	// BM::CAutoMutex
#include "CoreCenter.h"
#include "Cel_Log.h"

using namespace CEL;

CDBConnection::CDBConnection(void)
{
}

CDBConnection::CDBConnection(std::wstring const& rkConnString)
{
	m_kConnString = rkConnString;
}

CDBConnection::~CDBConnection(void)
{
/*
	CONT_SESSION::iterator session_itor = m_kContSession.begin();
	while(m_kContSession.end() != session_itor)
	{
		(*session_itor)->Close();
		SAFE_DELETE((*session_itor))
		++session_itor;
	}
*/
//	m_kSession.Close();
	m_kDataSource.Close();
}

CDBSession * CDBConnection::AllocSession()
{
	CDBSession * pSession =  new CDBSession;
	if(	pSession
	&&	S_OK == pSession->Open(m_kDataSource))
	{
		return pSession;
	}
	return NULL;
}

bool CDBConnection::Init( DB_INFO const& rInfo, bool const bUseConsoleLog)
{
	if (Init( rInfo.Addr(), rInfo.Name(), rInfo.ID(), rInfo.PW(), bUseConsoleLog))
	{
		return true;
	}
	return false;
}

bool CDBConnection::Init(	BM::vstring const& vstrDBSource,	BM::vstring const& vstrDBCatalog, 
							BM::vstring const& vstrUserID,		BM::vstring const& vstrUserPW,	
							bool const bUseConsoleLog) 
{
	DBSource(vstrDBSource);
	DBCatalog(vstrDBCatalog);
	UserID(vstrUserID);
	UserPW(vstrUserPW);

	BM::vstring strLogPath(_T("."));
	strLogPath += vstrDBSource;
	strLogPath += L" ";
	strLogPath += vstrDBCatalog;

	BM::vstring strConn;
	strConn += L"Provider = MSOLEDBSQL";
	strConn += L"; Server="	;	 
	strConn += DBSource();
	strConn += L"; Database=";
	strConn += DBCatalog();
	strConn += L"; UID=";
	strConn += UserID();
	strConn += L"; PWD=";
	strConn += UserPW();
	strConn += L"; Packet Size = 16384";
	strConn += L"; Use Procedure for Prepare = 1";
	strConn += L"; Auto Translate = True";

//! Parameter Description //! 기타 옵션 있음.
//! Trusted_Connection
//! Current Language 
//! Network Address 
//! Network Library 
//! Use Procedure for Prepare Determines 
//! Auto Translate 
//! Packet Size 
//! Application Name 
//! Workstation ID 

	ConnString((std::wstring)strConn);

	return OpenDB();
}

bool CDBConnection::OpenDB()
{//! connect to the database
	HRESULT const hr = m_kDataSource.OpenFromInitializationString(ConnString().c_str());
	if(SUCCEEDED(hr))
	{
		if(S_OK == m_kSession.Open(m_kDataSource))
		{
			return true;
		}
		CEL_LOG(BM::LOG_LV0, __FL__ << _T("[Open] Open=") << ConnString() << _T(" Failed=")<< HEX(hr));
		return false;
	}
	else
	{
		CEL_LOG(BM::LOG_LV0, __FL__ << _T("[OpenFromInitializationString] Open=") << ConnString() << _T(" Failed=")<< HEX(hr));
		assert(NULL);
		return false;
	}
	return false;
}

void CDBConnection::CloseDB()
{
	//m_DataConnection.CloseDataSource();
	m_kDataSource.Close();
}

BM::vstring const& CDBConnection::DBSource()const 
{
	return m_vstrDBSource;
}

BM::vstring const& CDBConnection::DBCatalog()const 
{
	return m_vstrDBCatalog;
}

BM::vstring const& CDBConnection::UserID()const 
{
	return m_vstrUserID;
}

BM::vstring const& CDBConnection::UserPW()const 
{
	return m_vstrUserPW;
}

void CDBConnection::DBSource(BM::vstring const& vstrDBSource)
{
	m_vstrDBSource = vstrDBSource;
}

void CDBConnection::DBCatalog(BM::vstring const& vstrDBCatalog)
{
	m_vstrDBCatalog = vstrDBCatalog;
}

void CDBConnection::UserID(BM::vstring const& vstrUserID)
{
	m_vstrUserID = vstrUserID;
}

void CDBConnection::UserPW(BM::vstring const& vstrUserPW)
{
	m_vstrUserPW = vstrUserPW;
}

BM::vstring DB_INFO::DataToString() const 
{
	BM::vstring strLog;
	strLog += L"[Source = ";
	strLog +=	Addr();
	strLog += L" Catalog = ";
	strLog +=	Name();
	strLog += L" ID = ";
	strLog +=	ID();
	strLog += L"]";
	return strLog;
}

//=======================================================================================================================================

ULONG CManualQuery::GetParameterCount()const
{
	return 0;
}
void CManualQuery::SetDataSourceType(int const nDataType,DBPARAMBINDINFO & kDBBindInfo)
{
	switch(nDataType)
	{	
	case DBTYPE_I2:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_I2"));
		}break;
	case DBTYPE_I4:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_I4"));
		}break;
	case DBTYPE_R4:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_R4"));
		}break;
	case DBTYPE_R8:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_R8"));
		}break;
	case DBTYPE_CY:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_CY"));
		}break;
	case DBTYPE_DATE:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_DATE"));
		}break;
	case DBTYPE_BSTR:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_BSTR"));
		}break;
	case DBTYPE_BOOL:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_BOOL"));
		}break;
	case DBTYPE_VARIANT:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_VARIANT"));
		}break;
	case DBTYPE_DECIMAL:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_DECIMAL"));
		}break;
	case DBTYPE_UI1:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_UI1"));
		}break;
	case DBTYPE_I1:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_I1"));
		}break;
	case DBTYPE_UI2:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_UI2"));
		}break;
	case DBTYPE_UI4:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_UI4"));
		}break;
	case DBTYPE_I8:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_I8"));
		}break;
	case DBTYPE_UI8:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_UI8"));
		}break;
	case DBTYPE_GUID:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_GUID"));
		}break;
	case DBTYPE_BYTES:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_BINARY"));
		}break;
	case DBTYPE_STR:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_CHAR"));
		}break;
	case DBTYPE_WSTR:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_WCHAR"));
		}break;
	case DBTYPE_DBDATE:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_DBDATE"));
		}break;
	case DBTYPE_DBTIME:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_DBTIME"));
		}break;
	case DBTYPE_DBTIMESTAMP:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_DBTIMESTAMP"));
		}break;
	case DBTYPE_EMPTY:
	case DBTYPE_NULL:
	case DBTYPE_IDISPATCH:
	case DBTYPE_ERROR:
	case DBTYPE_NUMERIC:
	case DBTYPE_UDT:
	case DBTYPE_VECTOR:
	case DBTYPE_RESERVED:
	case DBTYPE_IUNKNOWN:
	case DBTYPE_ARRAY:
	case DBTYPE_BYREF:
		{
			kDBBindInfo.pwszDataSourceType = T2OLE(_T("DBTYPE_IUNKNOWN"));
		}break;
	}
}

DWORD const CManualQuery::GetBindSize(int const nDataType)
{
	switch(nDataType)
	{	
	case DBTYPE_I2:{return sizeof(short);}break;
	case DBTYPE_I4:{return sizeof(int);}break;
	case DBTYPE_R4:{return sizeof(float);}break;
	case DBTYPE_R8:{return sizeof(double);}break;
	case DBTYPE_CY:{return sizeof(__int64);}break;
	case DBTYPE_DATE:{return sizeof(DBTIMESTAMP);}break;
	case DBTYPE_BSTR:{return MAX_BIND_DATA_SIZE;}break;
	case DBTYPE_BOOL:{return sizeof(bool);}break;
	case DBTYPE_VARIANT:{return sizeof(VARIANT);}break;
	case DBTYPE_DECIMAL:{return sizeof(DECIMAL);}break;
	case DBTYPE_UI1:{return sizeof(BYTE);}break;
	case DBTYPE_I1:{return sizeof(signed char);}break;
	case DBTYPE_UI2:{return sizeof(USHORT);}break;
	case DBTYPE_UI4:{return sizeof(ULONG);}break;
	case DBTYPE_I8:{return sizeof(__int64);}break;
	case DBTYPE_UI8:{return sizeof(unsigned __int64);}break;
	case DBTYPE_GUID:{return sizeof(GUID);}break;
	case DBTYPE_BYTES:{return MAX_BIND_DATA_SIZE;}break;
	case DBTYPE_STR:{return MAX_BIND_DATA_SIZE;}break;
	case DBTYPE_WSTR:{return MAX_BIND_DATA_SIZE;}break;
	case DBTYPE_DBDATE:{return sizeof(DBDATE);}break;
	case DBTYPE_DBTIME:{return sizeof(DBTIME);}break;
	case DBTYPE_DBTIMESTAMP:{return sizeof(DBTIMESTAMP);}break;
	case DBTYPE_EMPTY:
	case DBTYPE_NULL:
	case DBTYPE_IDISPATCH:
	case DBTYPE_ERROR:
	case DBTYPE_NUMERIC:
	case DBTYPE_UDT:
	case DBTYPE_VECTOR:
	case DBTYPE_RESERVED:
	case DBTYPE_IUNKNOWN:
	case DBTYPE_ARRAY:
	case DBTYPE_BYREF:{return 0;}break;
	}
	return 0;
}

void CManualQuery::SetBindInfo(CProcedureParameterInfo & kParam,DBPARAMBINDINFO & kDBBindInfo)
{
	kDBBindInfo.pwszName = NULL;
	kDBBindInfo.bPrecision = static_cast<BYTE>(kParam.m_nPrecision);
	kDBBindInfo.bScale = static_cast<BYTE>(kParam.m_nScale);
	kDBBindInfo.dwFlags = 0;
	SetDataSourceType(kParam.m_nDataType,kDBBindInfo);
	kDBBindInfo.ulParamSize = GetBindSize(kParam.m_nDataType);
}

void CDBSession::ReserveDBBind(size_t const kReserveCount)
{
	memset(m_pkBufResult, 0, sizeof(DBBINDDATA)*kReserveCount);
}	
