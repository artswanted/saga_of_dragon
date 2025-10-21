#include "stdafx.h"
#include "ace/high_res_Timer.h"
#include "BM/Stream.h"
#include "BM/LocalMgr.h"
#include "BM/LogWorker.h"
#include "../Header/DBWorker_Dispatcher.h"
#include "../query.h"

using namespace CEL;

CDBWorker_Dispatcher::CDBWorker_Dispatcher(INIT_DB_DESC const& rInitInfo)
	:	CDBWorker_Base( rInitInfo )
{
	m_kIsStop = false;
	m_eDBType = DBWT_DBWORKER;
	msg_queue()->high_water_mark(E_WATER_MARK);
	
	if(m_kInitInfo.bUseConsoleLog)
	{
		g_kLogWorker.VRegistInstance( REG_LOG_INSTANCE(LWT_DB, BM::OUTPUT_FILE_AND_CONSOLE,	BM::LFC_WHITE, rInitInfo.m_kLogFolder, (std::wstring const)rInitInfo.kDBInfo.Name(), g_kLocal.GetLocale()));
	}
}

CDBWorker_Dispatcher::~CDBWorker_Dispatcher()
{
	CONT_DB_WORKER::iterator worker_itor = m_kContWorker.begin();
	for( ; worker_itor != m_kContWorker.end() ; ++worker_itor )
	{
		CONT_DB_WORKER::value_type pkWorker = *worker_itor;
		SAFE_DELETE(pkWorker);
	}
	m_kContWorker.clear();
}

bool CDBWorker_Dispatcher::RegistDBConn( DB_INFO const& rDBInfo)
{
	bool const bRet = CDBConnection::Init(rDBInfo, m_kInitInfo.bUseConsoleLog);
	if( bRet )
	{
		if(m_kInitInfo.bUseConsoleLog)
		{
			g_kLogWorker.PushLog(SDebugLogMessage(LWT_DB, BM::LOG_LV6, (BM::vstring)_T("[") << __FUNCTIONW__ << (BM::vstring)_T("] DBConn Reg.....DBIndex[") << rDBInfo.DBIndex() <<  _T("]")));
		}
		return true;
	}

	return false;
}

bool CDBWorker_Dispatcher::VInit()
{
	if(!m_kInitInfo.dwWorkerCount)
	{
		return false;
	}

	if(RegistDBConn(m_kInitInfo.kDBInfo) )//Init으로 OpenDB를 해서. 커넥션 풀을 사용할 준비가 됨.
	{
		DWORD dwCount = m_kInitInfo.dwWorkerCount;
		while(dwCount--)
		{
			CDBWorker *pkWorker = new CDBWorker(m_kInitInfo, this->ConnString(), g_kLogWorker, LWT_DB, dwCount);
			if(pkWorker->VInit())
			{
				m_kContWorker.push_back(pkWorker);
				pkWorker->VActivate();//각각의 워커는 1개의 스레드만.
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CDBWorker_Dispatcher::VActivate(int const iForceThreadCount)
{//디스패쳐는 무조건 스레드가 하나!
	if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 0, THREAD_BASE_PRIORITY_MAX))//THREAD_BASE_PRIORITY_MAX
	{
		__asm int 3;
		return false;
	}
	return true;
}

bool CDBWorker_Dispatcher::VDeactivate()
{
	m_kIsStop = true;
	msg_queue()->pulse();
	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
	if( 0 == iRet )
	{
		return true;
	}
	return false;
}

HRESULT CDBWorker_Dispatcher::VPush(DB_QUERY_TRAN const& rkQuery)
{
	DB_QUERY_TRAN *pQuery = new DB_QUERY_TRAN(rkQuery);
	if( pQuery )
	{
		DB_QUERY_TRAN** ppkMsg = (DB_QUERY_TRAN**)pQuery;

		ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
		if( -1 != msg_queue()->enqueue( ppkMsg, &tv ) )
		{
			return S_OK;
		}
		
		SAFE_DELETE(pQuery);
	}
	return E_FAIL;
}//! 셋팅된 쿼리 넣기

int CDBWorker_Dispatcher::svc (void)
{
	while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
	{
		DB_QUERY_TRAN **ppkMsg = NULL;

//		ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
		if( -1 != msg_queue()->dequeue( ppkMsg ) )
		{//실험!
			DB_QUERY_TRAN *pkMsg = (DB_QUERY_TRAN*)ppkMsg;
			
			VProcess(*pkMsg);
// 워커가 가져가서 지워라.			
//			SAFE_DELETE(pkMsg);
		}
	}

	CONT_DB_WORKER::iterator worker_itor = m_kContWorker.begin();
	for( ; worker_itor != m_kContWorker.end() ; ++worker_itor )
	{
		CONT_DB_WORKER::value_type pkWorker = (*worker_itor);
		pkWorker->VDeactivate();
	}

	return 1;
}

HRESULT CDBWorker_Dispatcher::VProcess(DB_QUERY_TRAN const& rkQuery)
{
	{	
		BM::CAutoMutex kLock(m_kRecordMutex);

		CONT_DB_QUERY::const_iterator q_itor = rkQuery.begin();

		while(rkQuery.end() != q_itor)
		{
			auto ret = m_kContQueryRecord.insert(std::make_pair((*q_itor).GetProc(), 1));
			if(!ret.second)
			{
				++(*ret.first).second;
			}
			++q_itor;
		}
	}

	while(true)
	{//1번에 A, 2번에 B 들어있을때 AB 들어오면. 이거 넣기 대기를 해야한다.
		int iAvailCount = 0;
		CONT_DB_WORKER::value_type pkAvailWorker = NULL;

		CONT_DB_WORKER::iterator worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			CONT_DB_WORKER::value_type pkWorker = (*worker_itor);

			if(pkWorker->IsInputAvailable(rkQuery))//워킹중인데. 타겟이 같아서 처리 가능 하겠는가.
			{
				++iAvailCount;
				pkAvailWorker = pkWorker;
			}
			++worker_itor;
		}

		if(iAvailCount == 1)
		{//맞는게 하나야.
			pkAvailWorker->VPush(rkQuery);
			return S_OK;
		}

		if(!iAvailCount)
		{//맞는게 없다.
			break;
		}

		Sleep(1);// while go. 한타임 쉬고.
	}//이걸 통과 하면 나에 맞는 큐가 없다.
	
	{
		CONT_DB_WORKER::iterator worker_itor = m_kContWorker.begin();
		
		CONT_DB_WORKER::value_type pkResultWorker = (*worker_itor);

		while(worker_itor != m_kContWorker.end())
		{
			CONT_DB_WORKER::value_type pkWorker = (*worker_itor);
			
			if(pkResultWorker->GetQueryCount() > pkWorker->GetQueryCount()	)
			{//쿼리 쌓인게 더 작다면. 그것이 타겟.
				pkResultWorker = pkWorker;
			}

			++worker_itor;
		}

		pkResultWorker->VPush(rkQuery);
		return S_OK;
	}
}

HRESULT CDBWorker_Dispatcher::ImmidiateProcess(DB_QUERY_TRAN const& rkQuery)//! 셋팅된 쿼리 넣기
{
	CONT_DB_WORKER::reverse_iterator worker_ritor = m_kContWorker.rbegin();
	while(worker_ritor != m_kContWorker.rend())
	{
		CONT_DB_WORKER::value_type pkWorker = (*worker_ritor);
		if(pkWorker->IsEmpty())
		{
			pkWorker->VProcess(rkQuery);
			return S_OK;
		}
		++worker_ritor;
	}

	worker_ritor = m_kContWorker.rbegin();
	CONT_DB_WORKER::value_type pkWorker = (*worker_ritor);
	return pkWorker->VProcess(rkQuery);
}

void CDBWorker_Dispatcher::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring)_T("===== CDBWorker_Dispatcher State Display ===== IsRun[") << !IsStop()<<  _T("]")));
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("ConnInfo [") << (wchar_t const *)m_kInitInfo.kDBInfo.DataToString()<<  _T("]")));

	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("------------ DB Dispatcher Queue Size = ") << GetQueryCount()));
	CONT_DB_WORKER::iterator worker_itor = m_kContWorker.begin();
	while(worker_itor != m_kContWorker.end())
	{
		CONT_DB_WORKER::value_type pkWorker = (*worker_itor);
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("------------ DB Worker Queue Size = ") << pkWorker->GetQueryCount()));
		++worker_itor;
	}

	size_t total_queued_acc = 0;
	size_t total_processed_acc = 0;
	worker_itor = m_kContWorker.begin();
	while(worker_itor != m_kContWorker.end())
	{
		CONT_DB_WORKER::value_type pkWorker = (*worker_itor);
		total_queued_acc += pkWorker->GetAccQueuedQueryCount();
		total_processed_acc += pkWorker->GetAccProcessedQueryCount();

		++worker_itor;
	}
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("------------ Queued Complete Query Count = [") << total_queued_acc <<  _T("]")));
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("------------ Processed Complete Query Count = [") << total_processed_acc <<  _T("]")));
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("------------ Queued - Processed = [") << total_queued_acc - total_processed_acc <<  _T("]")));
	
}

void CDBWorker_Dispatcher::VDisplayStateDBRecord(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring)_T("===== CDBWorker_Dispatcher Query Record Display ===== IsRun[") << !IsStop() <<  _T("]")));
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("ConnInfo [") << (wchar_t const *)m_kInitInfo.kDBInfo.DataToString() <<  _T("]")));

	BM::CAutoMutex kLock(m_kRecordMutex);

	CONT_QUERY_RECORD::const_iterator q_itor = m_kContQueryRecord.begin();

	while(q_itor != m_kContQueryRecord.end())
	{
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)((*q_itor).first.c_str())<< (BM::vstring)_T(" - ") << (BM::vstring)((*q_itor).second)));
		++q_itor;
	}
}

bool CDBWorker_Dispatcher::VReadyToStop(PgLogWorker_Base &rkLogWorker, DWORD const iLogType) const
{
	CONT_DB_WORKER::const_iterator worker_itor = m_kContWorker.begin();
	while(worker_itor != m_kContWorker.end())
	{
		CONT_DB_WORKER::value_type pkWorker = (*worker_itor);
		if (pkWorker->GetQueryCount() > 0)
		{
			rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV5, (BM::vstring)_T("DB_Worker QueryCount = ") << pkWorker->GetQueryCount()));
			return false;
		}
		++worker_itor;
	}
	return true;
}