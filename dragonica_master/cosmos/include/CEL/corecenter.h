#pragma once

#include "ace/Task.h"

#include "CEL/Common.h"
#include "CEL/DBWorker_Base.h"
#include "CEL/Worker_Base.h"
#include "CEL/Session_Base.h"
#include "CEL/Timer_Base.h"
#include "CEL/CoreWorker_Base.h"
#include "CEL/CoreRegister_Base.h"
#include "CEL/CoreConnector_Base.h"

namespace CEL
{
	typedef enum eCoreReturnValue
	{
		CRV_NONE					= 0,
		CRV_SUCCESS					= 1,
	
		CRV_REGIST_FAILED			= 2,
		CRV_ALLOC_FAILED			= 3,
		CRV_CREATE_GUID_FAILED		= 4,

		CRV_ACCEPTOR_INIT_FAILED	= 5,
		CRV_REGIST_TIMER_FAILED		= 6,

		CRV_INCORRECT_INIT_INFO		= 7,

		CRV_NO_DB_EXECUTE_FUNC		= 8,
		CRV_ALREADY_USE_DB_INDEX	= 9,

		CRV_NULL_DB_WORKER_FAILED	= 10,

		CRV_NO_DB_EXECUTE_TRAN_FUNC = 11,
		CRV_INCORRECT_DB_INDEX		= 12,//디비 인덱스는 n > 0 이어야함. NULL_DB_INDEX = 0, 크리에이터가 등록 인덱스 * -1

		CRV_NO_DB_WORKER_COUNT		= 13,
	}E_CORE_RETURN_VALUE;

	class CCoreCenter//외부에서 쓰는 함수( OnRecv 등의 그런 함수에서 써지는 CoreCenter 외부 함수는 public 으로 선언
		:	public ACE_Task_Base
	{//! cpp 내부 에서 singleton 으로 클래스를 정의하고 쓴다.
		friend struct ::Loki::CreateStatic< CCoreCenter >;
		friend class CCoreWorker;
		friend class CCoreRegister;
		friend class CCoreConnector;
		friend class CDBWorker_Base;
		friend class CDBWorker;

		typedef std::map< BM::GUID, CWorker_Base* >		CONT_WORKER;
		typedef std::map< BM::GUID, CTimer_Base* >		CONT_TIMER;
		typedef std::map< int, CDBWorker_Base* >		CONT_DB_DISPATCHER_WORKER;//DBIndex 로 찾음.

	private:
		CCoreCenter();
		virtual ~CCoreCenter();
	
	public://Init 그룹
		bool Init(INIT_CENTER_DESC const& kInit);
		bool SvcStart();
		void Close();
		void SvcStartProactor();

	public:
		bool Send(SESSION_KEY const& kSessionKey,  BM::Stream const& rPacket);//소유자와 child 의 키.
		BM::GUID Regist(ERegistType const eRT, tagRegist_Base const *pInitInfo);
		bool Connect(ADDR_INFO const& rAddr, SESSION_KEY &kSessionKey, BM::GUID &kConnectObj = BM::GUID());//WorkerGuid만 셋팅해서 넣으시오 //리턴 -> Connect 시도 개체의 GUID
		bool Terminate(SESSION_KEY const& kSessionKey);//즉시 종료. 다 보내고 끊으려면 Stream에 SetStop 해서 Send를 하시오

		HRESULT Statistics(SESSION_KEY const& kSessionKey,  SSessionStatistics &rkOutResult);//소유자와 child 의 키.
		HRESULT GetDBInitInfo(std::list< INIT_DB_DESC > &kContOut)const;

		bool IsExistAcceptor(ADDR_INFO const& rUseAddr)const;
		bool IsExistDBWorker(int const iDBIndex)const;

		void DisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
		void DisplayStateDBRecord(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
		bool VReadyToStop(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) const;

	public://DB 사용그룹
		size_t GetQueryCount(int const iDBIndex)const;
		HRESULT PushQuery( DB_QUERY &rkQuery, bool const bIsImmidiate = false);
		HRESULT PushQuery( DB_QUERY_TRAN &rkContQuery, bool const bIsImmidiate = false);
		HRESULT ClearQueryResult( DB_RESULT &rQuery );
		HRESULT ExecDBResult( DB_RESULT &rQuery ); //! used for execute external provide, for example CSV Query

	protected://등록 그룹
		E_CORE_RETURN_VALUE RegistAccpetor(INIT_CORE_DESC const& rInit, BM::GUID &rOutGuid);//! 등록된 결과 GUID를 반환 //바로 스레드가 시작. Init 에 스레드 값을 셋팅하고 SvcStart 를 하는게 정석.
		E_CORE_RETURN_VALUE RegistConnector(INIT_CORE_DESC const& rInit, BM::GUID &rOutGuid);//! 등록된 결과 GUID를 반환 //바로 스레드가 시작.
		E_CORE_RETURN_VALUE RegistDBWorker(INIT_DB_DESC const& rInit, BM::GUID &rOutGuid);//! 등록된 결과 GUID를 반환
		E_CORE_RETURN_VALUE RegistTimerFunc(REGIST_TIMER_DESC const& rInit, BM::GUID &rOutGuid);

	protected://외부서는 건들지마라.
		bool ConnectSub(SConnectInfo const& rkObj);
		bool RegistSub(SRegistInfo * const pObj);
		void CheckKeepAliveSub();

	protected:
		void InitEnv();
		void ClearEnv();

	protected://상속되어 내부에서 쓰는것을 캡슐화 하기위해 포인터로 선언됨.
		mutable ACE_RW_Thread_Mutex m_core_mutex;
		mutable ACE_RW_Thread_Mutex m_db_mutex;//for m_kContDBWorker

		CCoreRegister_Base *m_pCoreRegister;
		CCoreConnector_Base *m_pCoreConnector;

		CONT_WORKER		m_kContWorker;
		CONT_TIMER		m_kContTimer;
		CONT_DB_DISPATCHER_WORKER	m_kContDBWorker;

		INIT_CENTER_DESC m_kInit;
		
		bool m_bIsClose;

		virtual int svc(void);
	};
}
