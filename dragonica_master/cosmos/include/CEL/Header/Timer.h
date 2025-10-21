#pragma once

#include <map>
#include "Loki/Threads.h"
#include "ace/Timer_heap.h"
#include "ace/Timer_Queue_Adapters.h"

#include "CEL/Timer_Base.h"

namespace CEL
{
	class CTimer
		:	public CTimer_Base
		,	public ACE_Thread_Timer_Queue_Adapter< ACE_Timer_Heap >
	{
		friend class CCoreCenter;
	private:
		class CTimer_Handler
			:	public ACE_Event_Handler
		{
			friend class CTimer;
		protected://! 외부에서 생성 금지.
			CTimer_Handler(const LPTIMERCALLBACKFUNC lpTimerCallbackFunc, DWORD dwUserData);
			virtual ~CTimer_Handler(void);

		protected:
			virtual int handle_timeout (const ACE_Time_Value &current_time, void const *act = 0);
			void TimerID(long const lTimerID);
			long TimerID() const;

		private:
			const LPTIMERCALLBACKFUNC m_lpTimerCallBackFunc;
			volatile long m_lTimerID;
			DWORD m_dwUserData;
		};

	private:
		CTimer(DWORD const dwThreadCount);
		virtual ~CTimer(void);

	public:
		virtual bool VDeactivate();
		virtual bool SvcStart();

		virtual long Regist(const LPTIMERCALLBACKFUNC Func, DWORD const dwUserData, DWORD const dwMsecInterval, DWORD const dwMsecFirstInterval = 0);//! 단위:밀리세컨드, 콜백자체가 인터벌 보다 시간이 오래걸리면 인터벌이 제대로 안 먹을 수도 있습니다.(스케쥴이 밀린다는 이야기)
		void Remove(long const iTimerID);
		void ChangeInterval(long const iTimerID, DWORD const dwNewMsecInterval);
		void Clear();
		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
	
	private:
		typedef std::map< long, CTimer_Handler* > TIMER_HASH;

		ACE_Time_Value Interval(DWORD const dwMSec);//! 밀리세컨 단위를 ACE_Time_Value로 변경
		TIMER_HASH m_mapTimer;
		
		mutable Loki::Mutex m_kMutex;
		
		bool m_bIsSvcStart;
		DWORD m_dwThreadCount;
	};
};