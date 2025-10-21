#pragma once
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "Loki/Threads.h"
#include "BM/PgMsgWorker.h"

template< typename T_WORK_DATA , typedef typename T_MSG_WORKER > 
class PgMsgDispatcher//디스패쳐 
	:	public	ACE_Task_Ex< ACE_MT_SYNCH, T_WORK_DATA* >//등록을 멀티스레드에서 하는것이 아니라면, Lock 으로 보호할 이유는 없음. msg_queue 가 스레드 세이프 하기 때문.
{
	typedef enum eValue
	{
		E_WATER_MARK	= 500 * 1024 * 1024,//! 500MB 까지 버텨준다.
	}E_VALUE;

public:
	typedef typename T_MSG_WORKER MSG_WORKER;
	typedef typename T_WORK_DATA WORK_DATA;

	PgMsgDispatcher()
	{
		m_kIsStop= false;
		msg_queue()->high_water_mark(E_WATER_MARK);
	}
	virtual ~PgMsgDispatcher()
	{
		CONT_WORKER::iterator worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			ReleaseMsgWorker(*worker_itor);
			++worker_itor;
		}
	}

public:
	virtual MSG_WORKER* CreateMsgWorker()
	{
		return new T_MSG_WORKER;
	}

	virtual void ReleaseMsgWorker(MSG_WORKER* &pkWorker)const
	{
		SAFE_DELETE(pkWorker);
	}

	virtual bool VInit(DWORD const dwThreadCount)
	{
		if(!dwThreadCount)
		{
			__asm int 3;//스레드 없으면 동작 하지 않으므로 강제 크래쉬를 냅니다.
			return false;
		}

		DWORD dwCount = dwThreadCount;
		while(dwCount--)
		{
			MSG_WORKER *pkWorker = CreateMsgWorker();

			if(pkWorker->VInit())
			{
				m_kContWorker.push_back(pkWorker);
				pkWorker->VActivate();//각각의 워커는 1개의 스레드만.
			}
			else
			{
				__asm int 3;//동작 실패시 크래쉬.
				return false;
			}
		}

		return true;
	}

	virtual bool VActivate()
	{//디스패쳐는 무조건 스레드가 하나!
		if(-1 == activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 0, THREAD_BASE_PRIORITY_MAX))//THREAD_BASE_PRIORITY_MAX
		{
			__asm int 3;
			return false;
		}
		return true;
	}
	
	virtual bool VDeactivate()
	{
		CONT_WORKER::iterator worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			CONT_WORKER::value_type pkWorker = (*worker_itor);
			pkWorker->VDeactivate();
			SAFE_DELETE(pkWorker);
			++worker_itor;
		}
		m_kContWorker.clear();

		m_kIsStop = true;
		msg_queue()->pulse();
		int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
		if( 0 == iRet )
		{
			return true;
		}

		__asm int 3;// 스레드를 종료시키지 못했으므로 크래쉬
		return false;
	}

	virtual HRESULT VPush( WORK_DATA const* pkjob, unsigned long priority = 0 )
	{
		if( pkjob )
		{
			WORK_DATA** ppkMsg = (WORK_DATA**)pkjob;
			
			ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
			if( -1 != msg_queue()->enqueue_prio( ppkMsg, &tv, priority ) )
			{
				return S_OK;
			}
			
		}

		__asm int 3;//메세지를 넣기 실패..
		
		return E_FAIL;
	}
	
	virtual int svc (void)
	{
		while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
		{
			WORK_DATA **ppkMsg = NULL;

			if( -1 != msg_queue()->dequeue( ppkMsg ) )
			{
				WORK_DATA *pkMsg = (WORK_DATA*)ppkMsg;
				VProcess(pkMsg);//메세지는 워커가 가져가서 지우므로 여기서 지우지 않음.
			}
		}
		return 1;
	}

	void VDisplayState( PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
	{
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring) _T("===== PgMsgDispatcher State Display ===== IsRun[") << !IsStop() << _T("]") ));
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ Dispatcher Queue Size = ") << GetQueryCount()));
		
		CONT_WORKER::const_iterator worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			CONT_WORKER::value_type pkWorker = (*worker_itor);
			rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ MsgWorker Queue Size = ") << pkWorker->GetQueryCount() << L", Processed = " << pkWorker->GetAccProcessedQueryCount()));
			rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ MsgWorker Processed = ") << pkWorker->GetAccProcessedQueryCount() << _T(", Queue Size = ") << pkWorker->GetQueryCount() ));
			++worker_itor;
		}

		size_t total_queued_acc = 0;
		size_t total_processed_acc = 0;
		worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			CONT_WORKER::value_type pkWorker = (*worker_itor);
			total_queued_acc += pkWorker->GetAccQueuedQueryCount();
			total_processed_acc += pkWorker->GetAccProcessedQueryCount();

			++worker_itor;
		}
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ Queued Complete Query Count = [") << total_queued_acc << _T("]") ));
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ Processed Complete Query Count = [") << total_processed_acc << _T("]") ));
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring) _T("------------ Queued - Processed = [") << total_queued_acc - total_processed_acc << _T("]") ));
	}

	bool VReadyToStop( PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
	{
		if (GetQueryCount() > 0)
		{

			rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV6, (BM::vstring) _T("[%s] Queued Query Count = ") << GetQueryCount() ));
			return false;
		}
		
		CONT_WORKER::const_iterator worker_itor = m_kContWorker.begin();
		while(worker_itor != m_kContWorker.end())
		{
			CONT_WORKER::value_type pkWorker = (*worker_itor);
			if (pkWorker->GetQueryCount() > 0)
			{
				rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV6,  (BM::vstring) _T("------------ MsgWorker Queue Size = ") << pkWorker->GetQueryCount()));
				return false;
			}
			++worker_itor;
		}
		return true;
	}

protected:
	virtual HRESULT VProcess(WORK_DATA const* pkjob)
	{
		while(true)
		{//타겟이 없을 경우가 있구만.....
			int iAvailCount = 0;
			
			CONT_WORKER::value_type pkAvailWorker = NULL;

			CONT_WORKER::iterator worker_itor = m_kContWorker.begin();
			while(worker_itor != m_kContWorker.end())
			{
				CONT_WORKER::value_type pkWorker = (*worker_itor);

				if(pkWorker->IsInputAvailable(*pkjob))//워킹중인데. 타겟이 같아서 처리 가능 하겠는가.
				{
					++iAvailCount;
					pkAvailWorker = pkWorker;
				}
				++worker_itor;
			}
		
			if(iAvailCount == 1)
			{//맞는게 하나야.
				pkAvailWorker->VPush(pkjob);
					return S_OK;
				}

			if(!iAvailCount)
			{//맞는게 없다.
				break;
			}

			Sleep(1);// while go. 한타임 쉬고.
		}//이걸 통과 하면 나에 맞는 큐가 없다.
		
		{
			CONT_WORKER::iterator worker_itor = m_kContWorker.begin();
			
			CONT_WORKER::value_type pkResultWorker = (*worker_itor);

			while(worker_itor != m_kContWorker.end())
			{
				CONT_WORKER::value_type pkWorker = (*worker_itor);
				
				if(pkResultWorker->GetQueryCount() > pkWorker->GetQueryCount()	)
				{//쿼리 쌓인게 더 작다면. 그것이 타겟.
					pkResultWorker = pkWorker;
				}

				++worker_itor;
			}

			pkResultWorker->VPush(pkjob);
			return S_OK;
		}
	}	
	
	virtual size_t GetQueryCount()
	{
		return msg_queue()->message_count();
	}

protected:
	CLASS_DECLARATION_S_NO_SET( bool, IsStop );

protected:
	typedef std::list< MSG_WORKER* > CONT_WORKER;
	CONT_WORKER m_kContWorker;
};