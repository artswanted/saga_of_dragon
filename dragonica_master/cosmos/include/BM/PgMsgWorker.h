#pragma once
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "Loki/Threads.h"
#include "BM/Guid.h"
#include "BM/ClassSupport.h"
#include "BM/LogWorker_Base.h"

template< typename T_WORK_DATA > 
class PgMsgWorker
	:	public	ACE_Task_Ex< ACE_MT_SYNCH, T_WORK_DATA* >
{
	typedef enum eValue
	{
		E_WATER_MARK	= 500 * 1024 * 1024,//! 500MB 까지 버텨준다.
	}E_VALUE;

public:
	typedef typename T_WORK_DATA WORK_DATA;

	PgMsgWorker()
	{
		m_kIsStop = false;
		msg_queue()->high_water_mark(E_WATER_MARK);
		
		m_kAccProcessedQueryCount = 0;
		m_kAccQueuedQueryCount = 0;
	}

	virtual ~PgMsgWorker()
	{
		VDeactivate();
	}

public:

	virtual bool VInit()
	{
		return true;
	}

	virtual bool VActivate()
	{
		if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1, 0, THREAD_BASE_PRIORITY_MAX))//THREAD_BASE_PRIORITY_MAX
		{
			__asm int 3;
			return false;
		}
		return true;
	}
	
	virtual bool VDeactivate()
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

	virtual HRESULT VPush(T_WORK_DATA const *pkWorkerData)
	{//카피 하지 않기 위해 디스패쳐가 new 해온걸 받는다.
		if( pkWorkerData )
		{
			T_WORK_DATA** ppkWorkerData = (T_WORK_DATA**)pkWorkerData;

			OnBeginJob(*pkWorkerData);
			if( -1 != msg_queue()->enqueue(ppkWorkerData) )
			{
				++m_kAccQueuedQueryCount;
				return S_OK;
			}

			// 실패시
			OnCompleteJob(*pkWorkerData);

			T_WORK_DATA *pkCastData = const_cast<T_WORK_DATA*>(pkWorkerData);
			this->Free( pkCastData );//어쨌건 실패하면 지우고.
		}
		return E_FAIL;
	}

	virtual T_WORK_DATA* Alloc(){return new T_WORK_DATA;}
	virtual void Free( T_WORK_DATA *& p ){SAFE_DELETE(p);}

	virtual HRESULT VProcess(T_WORK_DATA *pkWorkData) = 0;//VProcess 는 상속받아 써야 합니다.

	virtual int svc (void)
	{
		while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
		{
			T_WORK_DATA **ppkMsg = NULL;
			if( -1 != msg_queue()->dequeue( ppkMsg ) )
			{//실험!
				T_WORK_DATA *pkMsg = (T_WORK_DATA*)ppkMsg;
				VProcess(pkMsg);
				++m_kAccProcessedQueryCount;
				OnCompleteJob(*pkMsg);
				this->Free(pkMsg);
			}
		}
		return 1;
	}

	virtual void VDisplayState( PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
	{
		rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV0, (BM::vstring) _T("===== PgMsgWorker State Display ===== IsRun[") << !IsStop() <<  _T("]") ));
	}

	virtual bool IsInputAvailable(T_WORK_DATA const& rkWorkData)const//처리 중일때. 추가로 넣을 수 있는지.
	{
		BM::CAutoMutex kLock(m_kWorkingtargetMutex, false);

		T_WORK_DATA::CONT_WORKDATA_TARGET::const_iterator target_itor = rkWorkData.m_kContTarget.begin();
		
		while(target_itor != rkWorkData.m_kContTarget.end())
		{
			CONT_WORKING_TARGET_INFO::const_iterator working_target_itor = m_kContWorkingInfo.find(*target_itor);
			if(working_target_itor != m_kContWorkingInfo.end())
			{
				return true;
			}
			++target_itor;
		}

		return false;
	}

	virtual size_t GetQueryCount()
	{
		return msg_queue()->message_count();
	}

	size_t GetAccQueuedQueryCount()const{return m_kAccQueuedQueryCount;}
	size_t GetAccProcessedQueryCount()const{return m_kAccProcessedQueryCount;}

protected:
	void OnBeginJob(T_WORK_DATA const& rkWorkData)
	{
		BM::CAutoMutex kLock(m_kWorkingtargetMutex, true);

		T_WORK_DATA::CONT_WORKDATA_TARGET::const_iterator target_itor = rkWorkData.m_kContTarget.begin();
		
		while(target_itor != rkWorkData.m_kContTarget.end())
		{
			auto ret = m_kContWorkingInfo.insert(std::make_pair((*target_itor), 1));
			if(!ret.second)
			{//넣는거 실패면 기존꺼 수량 증가.
				++((*ret.first).second);
			}
			++target_itor;
		}
	}
	
	void OnCompleteJob(T_WORK_DATA const& rkWorkData)
	{
		BM::CAutoMutex kLock(m_kWorkingtargetMutex, true);

		T_WORK_DATA::CONT_WORKDATA_TARGET::const_iterator target_itor = rkWorkData.m_kContTarget.begin();
		
		while(target_itor != rkWorkData.m_kContTarget.end())
		{
			CONT_WORKING_TARGET_INFO::iterator working_target_itor = m_kContWorkingInfo.find((*target_itor));
			if(working_target_itor != m_kContWorkingInfo.end())
			{
				--((*working_target_itor).second);
				
				if(!(*working_target_itor).second)
				{//0 카운트.
					m_kContWorkingInfo.erase(working_target_itor);
				}
			}
			else
			{
				__asm int 3;
			}

			++target_itor;
		}
	}

protected:
	CLASS_DECLARATION_S_NO_SET( bool, IsStop );

protected:
	mutable ACE_RW_Thread_Mutex			m_kWorkingtargetMutex;
	typedef std::map< typename T_WORK_DATA::CONT_WORKDATA_TARGET::value_type, size_t > CONT_WORKING_TARGET_INFO;//타켓 GUID, 워크 수.
	
	CONT_WORKING_TARGET_INFO m_kContWorkingInfo;

	size_t m_kAccQueuedQueryCount;
	size_t m_kAccProcessedQueryCount;
};
