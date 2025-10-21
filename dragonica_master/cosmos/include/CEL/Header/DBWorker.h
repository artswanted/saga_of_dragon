#pragma once

#include "Loki/Threads.h"
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "CEL/DBWorker_Base.h"

//!!! 디비 워커는 파라메터 쿼리를 지원 하면 안됨.(성능이 너무 느림)
namespace CEL
{
	class CDBWorker
		:	public	CDBWorker_Base
//		,	public	CDBConnection
		,	public	ACE_Task_Ex< ACE_MT_SYNCH, DB_QUERY_TRAN* >
	{
		//!!! 디비 워커는 파라메터 쿼리를 지원 하면 안됨.(성능이 너무 느림)

		typedef enum eValue
		{
			E_WATER_MARK	= 500 * 1024 * 1024,//! 500MB 까지 버텨준다.
		}E_VALUE;
	public:
		CDBWorker(INIT_DB_DESC const& rInitInfo, std::wstring const &kConnStr, PgLogWorker_Base &rkLogWorker, DWORD const iLogType,DWORD const dwWorkerIndex);
		virtual ~CDBWorker();
	
	public:
		virtual bool VInit();
		virtual bool VActivate(int const iForceThreadCount = 0);
		virtual bool VDeactivate();

		virtual HRESULT VPush(DB_QUERY_TRAN const& rkQuery);//! 셋팅된 쿼리 넣기
		virtual HRESULT VProcess(DB_QUERY_TRAN const& rkQuery);//! 셋팅된 쿼리 넣기

		virtual int svc (void);

//		bool RegistDBConn( DB_INFO const& rDBInfo);
		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType);

		virtual std::wstring const&  ConnString()const{return m_kConnString;};
		
		bool IsEmpty()const;
		bool IsInputAvailable(DB_QUERY_TRAN const& rkQuery)const;//처리 중일때. 추가로 넣을 수 있는지.

		virtual size_t GetQueryCount()
		{
			return msg_queue()->message_count();
		}

		size_t GetAccQueuedQueryCount()const{return m_kAccQueuedQueryCount;}
		size_t GetAccProcessedQueryCount()const{return m_kAccProcessedQueryCount;}
	
	protected:
		void OnBeginQuery(DB_QUERY_TRAN const& rkQuery);
		void OnCompleteQuery(DB_QUERY_TRAN const& rkQuery);

		virtual HRESULT CallProcedureNoParam(CDBSession * pkSession,DB_QUERY const& rkQuery, CEL::DB_RESULT& rkRet);

	protected:
		PgLogWorker_Base &m_kLogWorker;
		DWORD const m_kLogType;

		CLASS_DECLARATION_S_NO_SET( bool, IsStop );
		DWORD const m_dwWorkerIndex;
	protected:
		mutable ACE_RW_Thread_Mutex			m_kWorkingtargetMutex;
		mutable Loki::Mutex					m_kProcessMutex;

		typedef std::map< BM::GUID, size_t > CONT_WORKING_TARGET_INFO;//타켓 GUID, 워크 수.
		
		CONT_WORKING_TARGET_INFO m_kContWorkingInfo;

		size_t m_kAccQueuedQueryCount;
		size_t m_kAccProcessedQueryCount;

		char m_kBindBuf[5000000];
		char m_kOrdiBuf[1000000];
		char m_kResultBuf[5000000];
		std::wstring const m_kConnString;
	};

	#include "DBWorker.inl"
}
