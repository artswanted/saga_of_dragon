#pragma once
#include "Loki/Threads.h"
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "CEL/DBWorker_Base.h"
#include "./DBWorker.h"

//관계도.
//디스패쳐
//	디비워커(참조: 세션크리에이터, 델리터)
namespace CEL
{
	class CDBWorker_Dispatcher//디스패쳐 
		:	public	CDBWorker_Base
		,	public	CDBConnection
		,	public	ACE_Task_Ex< ACE_MT_SYNCH, DB_QUERY_TRAN* >
	{
		typedef enum eValue
		{
			E_WATER_MARK	= 500 * 1024 * 1024,//! 500MB 까지 버텨준다.
		}E_VALUE;
	public:
		CDBWorker_Dispatcher(INIT_DB_DESC const& rInitInfo);
		virtual ~CDBWorker_Dispatcher();
	
	public:
		virtual bool VInit();
		virtual bool VActivate(int const iForceThreadCount = 0);
		virtual bool VDeactivate();

		virtual HRESULT VPush(DB_QUERY_TRAN const& rkQuery);//! 셋팅된 쿼리 넣기
		virtual int svc (void);

		bool RegistDBConn( DB_INFO const& rDBInfo);
		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType);
		virtual void VDisplayStateDBRecord(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
		virtual bool VReadyToStop(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) const;

//		virtual std::wstring const&  ConnString()const{static const std::wstring kStr; return kStr;}
		virtual std::wstring const&  ConnString()const{return CDBConnection::ConnString();};

		HRESULT ImmidiateProcess(DB_QUERY_TRAN const& rkQuery);//! 셋팅된 쿼리 넣기
	protected:
		virtual HRESULT VProcess(DB_QUERY_TRAN const& rkQuery);//! 셋팅된 쿼리 넣기
		virtual size_t GetQueryCount()
		{
			return msg_queue()->message_count();
		}
		
	protected:
		CLASS_DECLARATION_S_NO_SET( bool, IsStop );

	protected:
		typedef std::list< CDBWorker* > CONT_DB_WORKER;
		CONT_DB_WORKER m_kContWorker;

		typedef std::map< std::wstring, __int64 > CONT_QUERY_RECORD;//쿼리 기록.
		CONT_QUERY_RECORD m_kContQueryRecord;

		mutable Loki::Mutex m_kRecordMutex;
	};

}
