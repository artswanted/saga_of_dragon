#pragma once

//!#define ATL_NO_LEAN_AND_MEAN 
//!#define _ATL_NO_DEFAULT_LIBS
#include <atlbase.h>
#include <atlconv.h>
#include <atldbcli.h>
#include <atldbsch.h>

#include <OLEDB.h>
#include <tchar.h>
#include <comutil.h>

#include "BM/STLSupport.H"
#include "BM/vstring.h"
#include "BM/GUID.h"
#include "BM/threadobject.h"

#include "Loki/Threads.h"
#include "CEL/common.h"
//!#pragma pack(push, 1)

namespace CEL
{
	typedef enum eDBResult
	{
		DR_NONE	 = 0,
		DR_SUCCESS,
		DR_NO_RESULT,
		DR_ERR_BEGIN		= 100,
		DR_ERR_PUSH			= 101,//Push 되지 않았음.//Queue가 꽉 차거나함.
		DR_ERR_QUERY		= 102,
		DR_ERR_PARAM		= 103,
		DR_ERR_PARAM_COUNT	= 104,
		DR_ERR_PARAM_SIZE	= 105,
		DR_ERR_PARAM_SET_DEFAULT	= 106,
		DR_ERR_PARAM_SET_TIMESTAMP	= 107,
		DR_ERR_PARAM_SET_STRING	= 108,
		DR_ERR_PARAM_SET_WSTRING= 109,
		DR_ERR_PARAM_SET_INT8	= 110,
		DR_ERR_PARAM_SET_INT16	= 111,
		DR_ERR_PARAM_SET_INT32	= 112,
		DR_ERR_PARAM_SET_INT64	= 113,
		DR_ERR_PARAM_SET_FLOAT	= 114,
		DR_ERR_PARAM_SET_DOUBLE	= 115,
		DR_ERR_PARAM_SET_GUID	= 116,
		DR_ERR_PARAM_SET_BINARY	= 117,
		DR_ERR_PARAM_SET_BOOL	= 118,
		DR_ERR_EXECUTE			= 119,
		DR_ERR_PARTIAL			= 120,		// partial process error in Tera DB
		DR_ERR_TIMEOUT			= 121,
		DR_ERR_CREATE_CONN		= 122,
		DR_ERR_PREPARE			= 123,
		DR_ERR_BIND_PARAM		= 124,
		DR_ERR_CALL_BINDINFO	= 125,
		DR_ERR_BIND_COLUMN		= 126,
		DR_ERR_CREATE_SESSION	= 127,
		DB_ERR_CREATE_ACCESSOR	= 128,
		DR_ERR_BEGIN_TRAN		= 129,
	}E_DB_RESULT;

	typedef std::vector< BYTE > DB_BINARY;

#pragma pack(1)
	class DB_INFO
	{
	public:
		typedef int	DB_INDEX;
	public:
		DB_INFO()
		{
			SetInfo(0, BM::vstring(_T("")), BM::vstring(_T("")), BM::vstring(_T("")), BM::vstring(_T("")));
		}
		DB_INFO( const DB_INDEX in_index, BM::vstring const& in_strDBName, BM::vstring const& in_strDBAddr, BM::vstring const& in_strDBID, BM::vstring const& in_strDBPW)
		{
			SetInfo(in_index, in_strDBName, in_strDBAddr, in_strDBID, in_strDBPW);
		}

		virtual ~DB_INFO(){}
	public:

		void SetInfo(const DB_INDEX in_index, BM::vstring const& in_strDBName, BM::vstring const& in_strDBAddr, BM::vstring const& in_strDBID, BM::vstring const& in_strDBPW)
		{
			DBIndex(in_index);
			_tcscpy_s(m_kDBName,30, (wchar_t const*)in_strDBName);
			_tcscpy_s(m_kDBAddr,30, (wchar_t const*)in_strDBAddr);
			_tcscpy_s(m_kDBID,30, (wchar_t const*)in_strDBID);
			_tcscpy_s(m_kDBPW,30, (wchar_t const*)in_strDBPW);
		}

		void DBIndex(const DB_INDEX &in_index) {m_index = in_index;}
		DB_INDEX DBIndex() const {return m_index;}
		
		BM::vstring const Addr() const { return BM::vstring(m_kDBAddr);}
		BM::vstring const Name() const { return BM::vstring(m_kDBName);}
		BM::vstring const ID() const { return BM::vstring(m_kDBID);}
		BM::vstring const PW() const { return BM::vstring(m_kDBPW);}

		BM::vstring DataToString() const;

	protected:
		DB_INDEX m_index;	// DB Type
	
		//등록때 카피 되므로 STL을 써서는 안된다.
		TCHAR m_kDBAddr[30];
		TCHAR m_kDBName[30];
		TCHAR m_kDBID[30];
		TCHAR m_kDBPW[30];
	};

	size_t const MAX_BIND_DATA_SIZE = 20480;

	typedef union 
	{
/*		BYTE		byte;
		short		int2;
		int			int8;
		__int64		int64;
		char		strText[1];
		wchar_t		wstrText[1];*/
		BYTE		Data[MAX_BIND_DATA_SIZE];
	}BindData;

	typedef struct tagDBBINDDATA
	{
		tagDBBINDDATA()
		{
			memset(kBindData.Data, 0, MAX_BIND_DATA_SIZE);
			dwState = 0;
			dwLength = 0;
		}

		BindData kBindData;
		DWORD dwState;
		DWORD dwLength;
	}DBBINDDATA;

#pragma pack()

	template< typename T_ACCESSOR >
	class CommonQuery
		:	public CCommand< T_ACCESSOR, CRowset, CMultipleResults >
	{
	public:
		CommonQuery(){}
		virtual ~CommonQuery()
		{
			Clear();
		}

	public:
		void Clear()
		{
			if(this->GetInterface())
			{
				this->ReleaseRows();
			}
			this->Close();

			this->ReleaseCommand();
		}

		CEL::E_DB_RESULT SetQuery(CSession const&  rkSession,std::wstring const& wstrQuery, bool const bParam) throw(...)
		{
			m_wstrQuery = wstrQuery;

			HRESULT hr = Create(rkSession, wstrQuery.c_str());
			if(FAILED(hr))
			{
				std::wstring strErr = GetErrorMsg();
				CEL_INFOLOG((BM::LOG_LV0, _T("[%s][%d] HRESULT = 0x%08x, ErrorMsg = %s"), __FUNCTIONW__, __LINE__, hr, strErr.c_str()));
				return DR_ERR_CREATE_CONN;
			}

			hr = Prepare();
			if(FAILED(hr))
			{
				std::wstring strErr = GetErrorMsg();
				CEL_INFOLOG((BM::LOG_LV0, _T("[%s][%d] HRESULT = 0x%08x, ErrorMsg = %s"), __FUNCTIONW__, __LINE__, hr, strErr.c_str()));
				return DR_ERR_PREPARE;
			}

			void* pDummy = NULL;
			if (bParam)
			{
				hr = BindParameters( &m_hParameterAccessor, m_spCommand, &pDummy);//! , true, true);//!뒤에 두 부분을 true로 해 놓으면 길이와 상태를 받아 올 수 있다.
				if(FAILED(hr))
				{
					std::wstring strErr = GetErrorMsg();
					CEL_INFOLOG((BM::LOG_LV0, _T("[%s][%d] HRESULT = 0x%08x, ErrorMsg = %s"), __FUNCTIONW__, __LINE__, hr, strErr.c_str()));
					return DR_ERR_BIND_PARAM;
				}
			}
			return DR_SUCCESS;
		}

		std::wstring GetErrorMsg()
		{
			USES_CONVERSION; 

			std::wstring wstrDesc, wstrSource, wstrHelpFile;
			int iErrorReserve = 0;
			IErrorInfo * pErrorInfo = NULL;

			while(S_OK == ::GetErrorInfo(iErrorReserve,&pErrorInfo))
			{
				BSTR bstrDesc = NULL;
				pErrorInfo->GetDescription(&bstrDesc);
				if(bstrDesc)
				{
					wstrDesc += _bstr_t(bstrDesc);
					SysFreeString(bstrDesc);
				}

				BSTR bstrSource = NULL;
				pErrorInfo->GetSource(&bstrSource);
				if(bstrSource)
				{
					wstrDesc += _bstr_t(bstrSource);
					SysFreeString(bstrSource);
				}

				BSTR bstrHelpFile = NULL;
				pErrorInfo->GetHelpFile(&bstrHelpFile);
				if(bstrHelpFile)
				{
					wstrDesc += _bstr_t(bstrHelpFile);
					SysFreeString(bstrHelpFile);
				}
				++iErrorReserve;
			}

			return wstrDesc + wstrSource + wstrHelpFile;
		}

		bool Execute(bool const bIsHaveResult) throw(...)
		{// 캐싱이 안됨.
			DBROWCOUNT RowCount = 0;
			
			HRESULT const hr = Open( NULL, &RowCount, bIsHaveResult, 0);//!커맨드 Open. //!여기서 Bind를 한다.
			if(FAILED(hr))
			{
				std::wstring strErr = GetErrorMsg();
				CEL_INFOLOG((BM::LOG_LV0, _T("[%s][%d] HRESULT = 0x%08x, ErrorMsg = %s"), __FUNCTIONW__, __LINE__, hr, strErr.c_str()));
				goto RETURN_FAILED;
			}

			return true;
		RETURN_FAILED:
			{
				return false;
			}
		}

		bool First()
		{
			if( GetInterface() )
			{
				if( S_OK == MoveFirst() )
				{	
					return true;
				}
			}
			else
			{
			}
			return false;
		}

		bool Next()
		{
			if( GetInterface() )
			{
				if( S_OK == MoveNext() )
				{
					return true;
				}
			}
			else
			{
			}
			return false;
		}

		bool NextResult()
		{
			LONG count = 0;
			
			if( S_OK == GetNextResult(&count) )
			{
				return true;
			}
			return false;
		}

		bool LastResult()
		{
			LONG count = 0;
			
			if( S_OK == GetNextResult(&count) )
			{
				while( !GetInterface() )
				{
					if( S_OK == GetNextResult(&count) )
					{
						continue;
					}
					return false;
				}
				return true;
			}
			return false;
		}

		template <typename T1>
		bool GetResult(IN const ULONG lIndex, OUT T1 &rOutData)
		{
			bool const bIsSuccess = GetValue(lIndex, &rOutData);
			
			if(!bIsSuccess)
			{
				assert(bIsSuccess && "Can't GetData");
			}
			return bIsSuccess;
		}

		template <>//!명시적으로 string 에관해서 부분적 특화.
		bool GetResult(IN const ULONG lIndex, OUT std::string &rOutData)
		{
			bool bIsSuccess = false;//! GetValue(lIndex, *Data);

			void *p = GetValue(lIndex);
			if(p)
			{
				rOutData.swap( std::string( (char const*)p) ); 
				bIsSuccess = true;
			}

			if(!bIsSuccess)
			{
				assert(bIsSuccess && "Can't GetData");
			}
			return bIsSuccess;
		}

		template <>//!명시적으로 BM::vstring 에관해서 부분적 특화.
		bool GetResult(IN const ULONG lIndex, OUT BM::vstring &rOutData)
		{
			bool bIsSuccess = false;//! GetValue(lIndex, *Data);

			void *p = GetValue(lIndex);
			if(p)
			{
				rOutData =  std::string( (char const*)p); 
				bIsSuccess = true;
			}

			if(!bIsSuccess)
			{
				assert(bIsSuccess && "Can't GetData");
			}
			return bIsSuccess;
		}

		template <>//!명시적으로 BLOB 데이터 받는곳 특화 
		bool GetResult(IN const ULONG lIndex, OUT DB_BINARY &rvecOutData)
		{//!vector값의 길이를 제대로 넣어야함.
			bool bIsSuccess = false;//! GetValue(lIndex, *Data);

			void *p = GetValue(lIndex);
			if(p)
			{//!바이너리값이 
				DBLENGTH uiLen = 0;
				if( GetLength(lIndex, &uiLen) )
				{
					if(0 < uiLen )
					{
						rvecOutData.resize(uiLen);
						if(rvecOutData.size() == uiLen)
						{
							::memcpy( &rvecOutData.at(0), p, uiLen);
							bIsSuccess = true;
						}					
					} //! if(0 < uiLen )
					else if( 0 == uiLen)
					{
						if(rvecOutData.size())
						{
							::memset( &rvecOutData.at(0), 0, rvecOutData.size() );
						}
						bIsSuccess = true;
					}
					//!나머지 경우는 에러
				}
			}

			if(!bIsSuccess)
			{
				assert(bIsSuccess && "Can't GetData");
			}
			return bIsSuccess;
		}

		template <>//!명시적으로 boolean 에관해서 부분적 특화.
		bool GetResult(IN const ULONG lIndex, OUT bool &rOutData)
		{//!BYTE 포인터가 제대로 와야 한다.
			bool bIsSuccess = false;//! GetValue(lIndex, *Data);

			void *p = GetValue(lIndex);
			if(p)
			{
				if(! *(char*)p)
				{
					rOutData = false;
				}
				else
				{
					rOutData = true;
				}
				bIsSuccess = true;
			}

			if(!bIsSuccess)
			{
				assert(bIsSuccess && "Can't GetData");
			}
			return bIsSuccess;
		}
		virtual ULONG GetParameterCount()const =0;
	protected:
		std::wstring m_wstrQuery;
	};

	class CManualQuery : public CommonQuery<CManualAccessor>
	{
	public:

		CManualQuery(){}
		virtual ~CManualQuery(){}

	public:
		virtual		ULONG GetParameterCount()const;

		static void 	SetDataSourceType(int const nDataType,DBPARAMBINDINFO & kDBBindInfo);
		static DWORD const GetBindSize(int const nDataType);
		static void 	SetBindInfo(CProcedureParameterInfo & kParam,DBPARAMBINDINFO & kDBBindInfo);
	};

	typedef std::vector< CProcedureParameterInfo > CONT_PROCEDUREPARAM;
	typedef std::vector< DBPARAMINFO > CONT_DBPARAMINFO;

	class CDBSession 
		: public CSession
	{
	public:

		CDBSession(){}
		virtual ~CDBSession(){}

	public:
		char* m_pkBufResult;
		void ReserveDBBind(size_t const kReserveCount);
		DBBINDDATA* BindData(){return (DBBINDDATA*)m_pkBufResult;}
	};

	class CDBConnection
	{
	public:
		CDBConnection(void);
		CDBConnection(std::wstring const& rkConnString);

		virtual ~CDBConnection(void);

	public:
		bool Init(	BM::vstring const& vstrDBSource,	BM::vstring const& vstrDBCatalog, 
					BM::vstring const& vstrUserID,		BM::vstring const& vstrUserPW,	
					bool const bUseConsoleLog = false);

		bool Init( DB_INFO const& rInfo, bool const bUseConsoleLog = false);
		
		bool OpenDB();//커넥션 풀만 콜 하시오. 그외에는 절대 콜 금지

		void CloseDB();

		BM::vstring const& DBSource()const;
		BM::vstring const& DBCatalog()const;
		BM::vstring const& UserID()const;
		BM::vstring const& UserPW()const;
		BM::vstring const& NickName()const;

		void DBSource(BM::vstring const& vstrDBSource);
		void DBCatalog(BM::vstring const& vstrDBCatalog);
		void UserID(BM::vstring const& vstrUserID);
		void UserPW(BM::vstring const& vstrUserPW);

	private:
		BM::vstring	m_vstrDBSource;
		BM::vstring	m_vstrDBCatalog;
		BM::vstring m_vstrUserID;
		BM::vstring m_vstrUserPW;

		CLASS_DECLARATION_S(std::wstring, ConnString);

		CDBSession * AllocSession();
	public:
		CDataSource & DataSource(){return m_kDataSource;}
		CDBSession & Session(){return m_kSession;}
		
		mutable ACE_RW_Thread_Mutex m_kMutex;

	protected:
		CDataSource						m_kDataSource;
		CDBSession						m_kSession;

		mutable Loki::Mutex				m_kSessionMutex;
	};
#include "Loki/Singleton.h"
}