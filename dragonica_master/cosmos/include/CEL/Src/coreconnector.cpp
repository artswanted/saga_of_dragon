#include "stdafx.h"
#include "ace/high_res_Timer.h"
#include "../header/CoreConnector.h"
#include "../CoreCenter.h"
#include "Cel_Log.h"

using namespace CEL;

bool CCoreConnector::VActivate()
{
	if(-1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1))//THREAD_BASE_PRIORITY_MAX
	{
		__asm int 3;
		return false;
	}
	return true;
}

bool CCoreConnector::VDeactivate()
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

int CCoreConnector::svc (void)
{
	while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
	{
		SConnectInfo** ppkObj = NULL;

		if( -1 != msg_queue()->dequeue( ppkObj) )
		{
			SConnectInfo* pkObj = (SConnectInfo*)ppkObj;
			g_kCoreCenter.ConnectSub(*pkObj);
			SAFE_DELETE(pkObj);
		}
	}

	::CoUninitialize();
	return 1;
}

bool CCoreConnector::Connect(ADDR_INFO const& rAddr, SESSION_KEY &kSessionKey, BM::GUID &kConnectObj) //소유자와 child 의 키
{//	queSendInfo.high_water_mark( 반드시 필요함.
	SConnectInfo *pObj = new SConnectInfo;//m_poolConnectInfo.New();
	if(pObj)
	{
		pObj->addr = rAddr;
		pObj->guidConnector = kSessionKey.WorkerGuid();
		pObj->guidOrder.Generate();
		kConnectObj = pObj->guidOrder;

		pObj->guidSession.Generate();
		kSessionKey.SessionGuid(pObj->guidSession);

		ACE_Time_Value tv(1, 100);	tv += ACE_OS::gettimeofday();

		SConnectInfo ** ppkObj = (SConnectInfo **)pObj;

		if( -1 != msg_queue()->enqueue(ppkObj, &tv) )
		{
			return true;
		}
		else
		{
			SAFE_DELETE(pObj);
		}
	}
	
	CEL_LOG(BM::LOG_LV0, _T("CCoreConnector::Connect Failed"));
	return false;
}