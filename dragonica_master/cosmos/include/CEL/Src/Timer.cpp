#include "stdafx.h"

#include "ace/high_res_Timer.h"
#include "BM/STLSupport.h"
#include "BM/ThreadObject.h"
#include "../Common.h"
#include "../Header/timer.h"

using namespace CEL;

CTimer::CTimer_Handler::CTimer_Handler(const LPTIMERCALLBACKFUNC lpTimerCallbackFunc, DWORD dwUserData)
	:	m_lpTimerCallBackFunc(lpTimerCallbackFunc)
//	,	m_r_parent_mutex(parent_mutex)
	,	m_dwUserData(dwUserData)
{
	assert(lpTimerCallbackFunc && "Timer Callback Function is NULL");
	TimerID(-1);
}

CTimer::CTimer_Handler::~CTimer_Handler()
{
}

int CTimer::CTimer_Handler::handle_timeout(const ACE_Time_Value &current_time, void const *act)
{
	(*m_lpTimerCallBackFunc)(m_dwUserData);
	return true;
}

void CTimer::CTimer_Handler::TimerID(long const lTimerID)
{
	::InterlockedExchange( &m_lTimerID, lTimerID);
}

long CTimer::CTimer_Handler::TimerID() const
{
	return m_lTimerID; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTimer::CTimer(DWORD const dwThreadCount)
{
	BM::CAutoMutex kLock(m_kMutex);

	m_mapTimer.clear();
	m_bIsSvcStart = false;
	m_dwThreadCount = dwThreadCount;
}

CTimer::~CTimer(void)
{
	Clear();
}

long CTimer::Regist(LPTIMERCALLBACKFUNC Func, DWORD const dwUserData, DWORD const dwMsecInterval, DWORD const dwMsecFirstInterval)
{
	BM::CAutoMutex kLock(m_kMutex);

	if(!dwMsecInterval)
	{
		assert(NULL && "Incorrect Interval Time");
		return 0;
	}
//! ACE_Time_Value tv(0, dwMsecInterval*1000); --> 이렇게 안쓰는 이유는 *1000 했을때의 오버 플로우가 무서워서;
	
	CTimer_Handler *pTimer = new CTimer_Handler(Func, dwUserData);
	if(!pTimer)
	{
		assert(NULL && "Not Enough Memory");
		return 0;
	}

	ACE_Time_Value interval			= Interval(dwMsecInterval);
	ACE_Time_Value first_interval	= Interval(dwMsecFirstInterval);

	long const lTimerID = schedule(pTimer, 0, ACE_OS::gettimeofday()+first_interval, interval);//! 시작하자마자 실행

	pTimer->TimerID(lTimerID);

	auto kRet = m_mapTimer.insert( TIMER_HASH::value_type(lTimerID, pTimer) );
	if(kRet.second)
	{
		return lTimerID;
	}

	SAFE_DELETE(pTimer);
	assert(NULL && "CTimer::Regist");
	return 0;
}

void CTimer::Remove(long const iTimerID)
{
	BM::CAutoMutex kLock(m_kMutex);

	timer_queue()->cancel(iTimerID);

	TIMER_HASH::iterator Itor = m_mapTimer.find(iTimerID);

	if(Itor != m_mapTimer.end())
	{
		SAFE_DELETE( (*Itor).second );
		m_mapTimer.erase(iTimerID);
		return;
	}
	assert(NULL && "CTimer::Remove");
}

void CTimer::ChangeInterval(long const iTimerID, DWORD const dwNewMsecInterval)
{
	BM::CAutoMutex kLock(m_kMutex);
	timer_queue()->reset_interval(iTimerID, Interval(dwNewMsecInterval));
}

ACE_Time_Value CTimer::Interval(DWORD const dwMSec)//! 밀리세컨 단위를 ACE_Time_Value로 변경
{
	BM::CAutoMutex kLock(m_kMutex);
	long const lSec = dwMSec/1000;//! 초단위
	long const USec = (dwMSec%1000)*1000;//! 마이크로 세컨드 단위
	ACE_Time_Value tv(lSec, USec);
	return tv;
}

void CTimer::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	deactivate();

	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );

	TIMER_HASH::iterator itor = m_mapTimer.begin();
	
	while(m_mapTimer.end() != itor)
	{
		cancel( (*itor).second->TimerID() );
		SAFE_DELETE( (*itor).second );
		m_mapTimer.erase(itor++);		
	}
}

bool CTimer::SvcStart()
{
	BM::CAutoMutex kLock(m_kMutex);
	if(!m_bIsSvcStart && m_dwThreadCount)
	{
		this->open();
		if(-1 == this->activate( THR_NEW_LWP| THR_JOINABLE | THR_INHERIT_SCHED, m_dwThreadCount, 0, THREAD_BASE_PRIORITY_MAX ))//THREAD_BASE_PRIORITY_MAX
		{
			__asm int 3;
			return false;
		}
		m_bIsSvcStart = true;
		return true;
	}
	assert(NULL && "CTimer::SvcStart()");
	return false;
}

bool CTimer::VDeactivate()
{
	Clear();
	return true;
}

void CTimer::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("===== Timer State Display ===== IsRun[") << m_bIsSvcStart <<  _T("]")));

	BM::CAutoMutex kLock(m_kMutex);
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV2, (BM::vstring)_T("TimerCount [") << m_mapTimer.size() << (BM::vstring)_T("] Thread Count [") << m_dwThreadCount <<  _T("]")));
}