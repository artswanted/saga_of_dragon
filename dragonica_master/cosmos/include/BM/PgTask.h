#pragma once
#include "ace/high_res_Timer.h"
#include "ace/Task_ex_t.h"
#include "ace/Message_Queue_t.h"
#include "BM/Stream.h"

#pragma pack(1)

typedef struct tagEventMessage
	:	public BM::Stream
{
	tagEventMessage(int const iInPriType=0, int const iInSecType=0)
	{
		Set(iInPriType,iInSecType);
	}

	void operator = (tagEventMessage const& rhs)
	{
		BM::Stream::operator = (rhs);
		PriType(rhs.PriType());
		SecType(rhs.SecType());
	}

	void Set(int const iInPriType, int const iInSecType)
	{
		PriType(iInPriType);
		SecType(iInSecType);
	}

	void WriteToPacket(BM::Stream &kStream)const
	{
		kStream.Push(PriType());
		kStream.Push(SecType());
		
		kStream.Push(Size());
		kStream.Push((BM::Stream)*this);
		// *this 로 임시개체 만들어 Push 하는 것이 맞음.
		// 이유 : WriteToPacket 했을때, wrpos 값이 변경되지 않기를 원하기 때문.
	}
	
	void ReadFromPacket(BM::Stream &kStream)
	{
		Reset();

		kStream.Pop(m_kPriType);
		kStream.Pop(m_kSecType);

		size_t iSize;
		kStream.Pop(iSize);
		if(iSize)
		{
			this->Data().resize(iSize);
			kStream.PopMemory(&this->Data().at(0), iSize);
			PosAdjust();
		}
	}

	CLASS_DECLARATION_S(int, PriType);
	CLASS_DECLARATION_S(int, SecType);
}SEventMessage;

#pragma pack()

template< typename T_MSG = SEventMessage >
class PgTask
	:	public ACE_Task_Ex< ACE_MT_SYNCH , T_MSG* >
{
protected:
	typedef typename T_MSG MSG;

	typedef enum eValue
	{
		E_WATER_MARK = 500 * 1024 * 1024,//! 500MB 까지 버텨준다.
	}E_VALUE;

public:
	explicit PgTask( bool const bIsDeactivateFlushMsg=false, size_t const iHighWaterMark=E_WATER_MARK )
		:	m_bIsDeactivateFlushMsg(bIsDeactivateFlushMsg)
		,	m_kIsStop(false)
	{
		SetHighWaterMark(iHighWaterMark);
	}

	virtual ~PgTask()
	{
		//	Terminate();//여기서 호출하면 안된다. virtual 함수를 호출하게 되므로
	}

	virtual bool PutMsg(T_MSG &rkMsg, unsigned long ulPriority = 0)
	{
		T_MSG *pkMsg = new T_MSG;
		
		if(pkMsg)
		{
			*pkMsg = rkMsg;

			T_MSG** ppkMsg = (T_MSG**)pkMsg;

			ACE_Time_Value tv(1, 100);	tv += ACE_OS::gettimeofday();
			//if(-1 == msg_queue()->enqueue(ppkMsg, &tv))
			if (-1 == msg_queue()->enqueue_prio(ppkMsg, &tv, ulPriority))
			{//실패
				SAFE_DELETE(pkMsg);
				return false;
			}
			return true;
		}
		return false;
	}

	virtual int svc(void)
	{
		while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
		{
			DequeueMessage( NULL );
		}

		Terminate();
		return 0;
	}

	virtual void HandleMessage(MSG *pkMsg) = 0;

	virtual int StartSvc(int const iThreadCount = 3)
	{
		if(iThreadCount)
		{
			return this->activate( THR_NEW_LWP| THR_JOINABLE | THR_INHERIT_SCHED, iThreadCount, 0, THREAD_BASE_PRIORITY_MAX);//THREAD_BASE_PRIORITY_MAX
		}
		return 0;
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

protected:
	void Terminate()
	{
		SetHighWaterMark(0);// 더이상 Msg를 못받게 
		if ( true == m_bIsDeactivateFlushMsg )
		{
			while( !msg_queue()->is_empty() )
			{
				ACE_Time_Value tv(1, 0);	tv += ACE_OS::gettimeofday();
				DequeueMessage( &tv );
			}
		}
	}

	void DequeueMessage( ACE_Time_Value *timeout )
	{
		T_MSG **ppkMsg = NULL;

		if( -1 != msg_queue()->dequeue( ppkMsg, timeout ) )
		{
			T_MSG *pkMsg = (T_MSG*)ppkMsg;
			this->HandleMessage(pkMsg);
			SAFE_DELETE(pkMsg);
		}
	}

	void SetHighWaterMark( size_t const iHwm )
	{
		msg_queue()->high_water_mark(iHwm);
	}

protected:
	bool	m_bIsDeactivateFlushMsg;
	
	CLASS_DECLARATION_S_NO_SET( bool, IsStop );
	
};
