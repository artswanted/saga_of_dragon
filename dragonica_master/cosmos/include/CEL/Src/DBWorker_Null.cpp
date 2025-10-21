#include "stdafx.h"

#include "ace/high_res_Timer.h"

#include "../Header/DBWorker_Null.h"
#include "../query.h"

using namespace CEL;

CDBWorker_Eraser::CDBWorker_Eraser()
	:	CDBWorker_Base( INIT_DB_DESC() )
//	,	m_QueryPool(100, 100)
{
	IsStop(false);
	m_eDBType = DBWT_DBWORKER;
	msg_queue()->high_water_mark(E_WATER_MARK);
	
//	if(m_kInitInfo.bUseConsoleLog)
//	{
//		m_kDBLog.Init(BM::OUTPUT_FILE_AND_CONSOLE, BM::LFC_WHITE, _T("./Log"), (std::wstring const)rInitInfo.kDBInfo.Name()+_T(".txt"));
//	}
}

CDBWorker_Eraser::~CDBWorker_Eraser()
{
}

bool CDBWorker_Eraser::VInit()
{
	return true;
}

bool CDBWorker_Eraser::VActivate(int const iForceThreadCount)
{
	if(iForceThreadCount)
	{
		if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, iForceThreadCount, 0, 0 ))//THREAD_BASE_PRIORITY_MAX
		{
			__asm int 3;
			return false;
		}
		return true;
	}
	else
	{
//		if(m_kInitInfo.dwThreadCount)
//		{
//			activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, (int)m_kInitInfo.dwThreadCount, 0, 0 );//THREAD_BASE_PRIORITY_MAX
//			return true;
//		}
	}
	return true;
}

bool CDBWorker_Eraser::VDeactivate()
{
	IsStop(true);
	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
	if( 0 == iRet )
	{
		return true;
	}
	return false;
}

HRESULT CDBWorker_Eraser::VPush(DB_RESULT &rQuery)
{
	DB_RESULT *pResult = new DB_RESULT(DB_QUERY());//m_QueryPool.New();
	if( !pResult ){return false;}
	
	pResult->vecArray.swap(rQuery.vecArray);

	DB_RESULT** ppkMsg = (DB_RESULT**)pResult;

	if( -1 != msg_queue()->enqueue( ppkMsg ) )
	{
		return S_OK;
	}
	
	SAFE_DELETE(pResult);
	return E_FAIL;
}//! 셋팅된 쿼리 넣기

int CDBWorker_Eraser::svc (void)
{
	while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
	{
		DB_RESULT **ppkData = NULL;

		ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
		if( -1 != msg_queue()->dequeue( ppkData, &tv ) )
		{
			DB_RESULT *pData = (DB_RESULT *)ppkData;
			
			SAFE_DELETE(pData);//클리어기 때문에.
		}

	}
	return 1;
}

void CDBWorker_Eraser::VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)
{
	rkLogWorker.PushLog(SDebugLogMessage(iLogType, BM::LOG_LV1, (BM::vstring)_T("===== CDBWorker_Eraser State Display ===== IsRun[") << !IsStop() <<  _T("]")));
//	kLogger.Log(BM::LOG_LV1, _T("Remain Msg Count [%d], Thread Count [%d]"), msg_queue()->message_count(), iForceThreadCount);
}