#pragma once
#include "CEL/Common.h"
#include "CEL/Query.h"

namespace CEL
{
	typedef std::vector< char > DB_DATA_ORG;

	typedef struct tagDBData
	{
		tagDBData();
	
		DBTYPE DataType;
		DB_DATA_ORG m_vecData;
		void Push( void const *pData, size_t const in_size );
		
		bool PopMemory(void *pBuffer, size_t const out_size)const;
		
		template < typename T_ELEMENT >
		bool Pop(T_ELEMENT &rElement)const;

		template < typename T_ELEMENT >
		bool Pop(T_ELEMENT* &rElement)const;//강제선언. 에러 나라고 만들어둔 것.
		
		template< typename T_VALUE >
		bool Pop( std::vector< T_VALUE > &vecout )const;//강제선언. 에러 나라고 만들어둔 것.

		template <>
		bool Pop( std::vector< BYTE > &vecout )const;

		template <>
		bool Pop( std::vector< char > &vecout )const;

		bool Pop(std::string &rElement)const;
		bool Pop(std::wstring &rElement)const;

		void ReadFromPacket(BM::Stream & kStream);
		void WriteToPacket(BM::Stream & kStream) const;

	}DB_DATA;
	
	typedef std::list< DB_DATA >	DB_DATA_ARRAY;//결과용
	typedef BM::Stream					CONT_USER_DATA;//유저 데이타용

	typedef std::set< BM::GUID > CONT_QUERY_TARGET;
	typedef struct tagDBResult DB_RESULT; // Back forward declaration
	typedef struct tagDB_RESULT_TRAN DB_RESULT_TRAN; // Back forward declaration
	typedef HRESULT (CALLBACK *LP_CALLBACK_DBRESULT)(CEL::DB_RESULT &rResult);
	typedef HRESULT (CALLBACK *LP_CALLBACK_TRAN_DBRESULT)(CEL::DB_RESULT_TRAN &rResult);

	typedef struct tagDBQuery
	{
	public:

		tagDBQuery(tagDBQuery const&  rhs);

		tagDBQuery(int const InDBIndex = 0, int const InType= 0, std::wstring const& wstrInQuery = _T(""));

		void PushParam( void const *pData, size_t const in_size );

	public:

		int const GetDBIndex() const;

		void Set(int const InDBIndex, int const InType, std::wstring const& wstrInQuery);

		template< typename T_STRINPUT >
		void PushStrParam( T_STRINPUT const&  input);//강제 비선언

		template< >
		void PushStrParam( std::wstring const&  input);

		template< >
		void PushStrParam( bool const&  input);

		template< >
		void PushStrParam( char const&  input);
		
		template< >
		void PushStrParam( unsigned char const&  input);

		template< >
		void PushStrParam( short const&  input);

		template< >
		void PushStrParam( unsigned short const&  input);

		template< >
		void PushStrParam( int const&  input);
		
		template< >
		void PushStrParam( unsigned int const&  input);

		template< >
		void PushStrParam( unsigned long const&  input);

		template< >
		void PushStrParam( __int64 const&  input);
		
		template< >
		void PushStrParam( unsigned __int64 const&  input);

		template< >
		void PushStrParam( double const&  input);

		template< >
		void PushStrParam( float const&  input);

		template< >
		void PushStrParam( BM::DBTIMESTAMP_EX const&  input);

		template< >
		void PushStrParam( BM::GUID const&  input);

		template< typename T_VALUE >
		void PushStrParam( std::vector< T_VALUE > const& input );

		void PushStrParam( BYTE const *pData, size_t const in_size );

		void PushStrParam( std::wstring const&  input, bool const bIsNotQuat);//N'' 를 붙이지않음.

		size_t const GetStrParamCount()const {return m_kStrParamCount;}
		void Add(tagDBQuery const& rkQuery);

	public:
		CONT_USER_DATA contUserData;

		std::wstring const&  Command()const{return wstrQuery;}
		std::wstring const&  GetProc()const{return wstrProc;}
		
		void InsertQueryTarget(BM::GUID const& kTarget);

		void ReadFromPacket(BM::Stream & kStream);
		void WriteToPacket(BM::Stream & kStream) const;
		static void StrConvForQuery(std::wstring const& strOrg, std::wstring &strOut);
	
	protected:
		void PushStrParam_sub( std::wstring const&  input);

		CLASS_DECLARATION_S(DWORD, StartTime);

		CLASS_DECLARATION_S_NO_SET(int, DBIndex);//! 쿼리 날릴 대상 DB
		CLASS_DECLARATION_S_NO_SET(int, QueryType);//! 쿼리 타입
		
		CLASS_DECLARATION_S(BM::GUID, QueryGuid);//! 쿼리 자체의 GUID 
		CLASS_DECLARATION_S(BM::GUID, QueryOwner);//! 쿼리 날린자 혹은 기타에 사용되는 OwnerGuid
		CLASS_DECLARATION_S(LP_CALLBACK_DBRESULT, QueryResultCallback);
		
		CONT_QUERY_TARGET m_kContQueryTarget;

	protected:
		std::wstring wstrQuery;
		std::wstring wstrProc;
		
		bool		bHaveReturn;
		BYTE		m_kStrParamCount;

	}DB_QUERY;

	typedef std::vector<int> DB_RESULT_COUNT;

	typedef struct tagDBResult
		:	public DB_QUERY
	{
		tagDBResult(DB_QUERY const& rkOrder);
		~tagDBResult();

		void ReadFromPacket(BM::Stream & kStream);
		void WriteToPacket(BM::Stream & kStream) const;

		E_DB_RESULT eRet;
		DB_DATA_ARRAY vecArray;			// 쿼리 결과
		DB_RESULT_COUNT vecResultCount;	// 쿼리별 결과 카운트
	}DB_RESULT;

	typedef std::list< DB_RESULT >	CONT_DB_RESULT;
	typedef std::list< DB_QUERY >	CONT_DB_QUERY;

//#pragma pack(1)

	typedef struct tagDB_QUERY_TRAN
		: public CONT_DB_QUERY
	{
		tagDB_QUERY_TRAN()
		{
			m_kQueryType = 0;
			m_kDBIndex = 0;
		}
		~tagDB_QUERY_TRAN()
		{
		}

		CLASS_DECLARATION_S(BM::GUID, QueryOwner);//! 쿼리 날린자 혹은 기타에 사용되는 OwnerGuid
		CLASS_DECLARATION_S(int, QueryType)// 쿼리 타입
		CLASS_DECLARATION_S(int, DBIndex);//! 쿼리 날릴 대상 DB
		CONT_USER_DATA	contUserData;

		void BuildQueryTarget();//쿼리타겟 빌드.

		CONT_QUERY_TARGET m_kContQueryTarget;
	}DB_QUERY_TRAN;

	typedef struct tagDB_RESULT_TRAN 
		: public CONT_DB_RESULT
	{
		CLASS_DECLARATION_S(BM::GUID, QueryOwner);//! 쿼리 날린자 혹은 기타에 사용되는 OwnerGuid
		CLASS_DECLARATION_S(int, QueryType)// 쿼리 타입
		CLASS_DECLARATION_S(int, DBIndex);//! 쿼리 날릴 대상 DB
	
		tagDB_RESULT_TRAN(tagDB_RESULT_TRAN const&  rkResult);
		tagDB_RESULT_TRAN(tagDB_QUERY_TRAN const&  rkQuery);
		CONT_USER_DATA	contUserData;
	}DB_RESULT_TRAN;

	typedef struct tagInitDBDesc
		: public tagRegist_Base
	{
		typedef enum eValue
		{
			E_MAX_DB_INFO = 20,//! 20개
			E_DEFAULT_CONN_POOL_COUNT = 10,
		}E_VALUE;

		tagInitDBDesc();

		bool bIsImmidiate;
		bool bUseConsoleLog;
		
		BM::GUID kOrderGuid;

		LP_CALLBACK_DBRESULT		OnDBExecute;
		LP_CALLBACK_TRAN_DBRESULT	OnDBExecuteTran;
		DWORD						dwWorkerCount;
		DB_INFO						kDBInfo;
		int							iRunType;
		DWORD						dwConnectionPoolCount;//기본 10개.
		std::wstring				m_kLogFolder;

		void WriteToPacket(BM::Stream &kStream)const;
		void ReadFromPacket(BM::Stream &kStream);

		size_t min_size()const
		{
			return 
				sizeof(bIsImmidiate)+
				sizeof(bUseConsoleLog)+//strMachinIP
				sizeof(kOrderGuid)+//strName
				sizeof(OnDBExecute)+
				sizeof(OnDBExecuteTran)+
				sizeof(dwWorkerCount)+
				sizeof(iRunType)+
				sizeof(dwConnectionPoolCount);
		}
	}INIT_DB_DESC;
//#pragma pack()

	typedef enum
	{
		DBWT_NONE = 0,
		DBWT_BASE = 1,
		DBWT_DBWORKER = 2,
	}EDBWorkerType;

	class CDBWorker_Base
	{
		friend class CCoreCenter;
	public:
		CDBWorker_Base(INIT_DB_DESC const& rInitInfo)
			:	m_kInitInfo(rInitInfo)
		{	
			m_eDBType = DBWT_BASE;
		}
		virtual ~CDBWorker_Base(){};
	
	public:	
		virtual bool VInit() = 0;
		virtual HRESULT VPush(DB_QUERY_TRAN const& rkQuery) = 0;//! 셋팅된 쿼리 넣기
		virtual bool VActivate(int const iForceThreadCount = 0) = 0;
		virtual bool VDeactivate() = 0;

		EDBWorkerType GetDBWorkerType()const{ return m_eDBType; }

		INIT_DB_DESC const& InitInfo()const{return m_kInitInfo;}
	
		virtual std::wstring const&  ConnString()const = 0;

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) = 0;
		virtual void VDisplayStateDBRecord(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const{};
		virtual bool VReadyToStop(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) const { return true; }
		
	protected:
		virtual HRESULT VProcess(DB_QUERY_TRAN const& rkQuery) = 0;//! 셋팅된 쿼리 실행
		virtual size_t GetQueryCount() = 0;

	protected:
		INIT_DB_DESC const m_kInitInfo;
		EDBWorkerType m_eDBType;
	};
	
#include "DBWorker_Base.inl"
}
