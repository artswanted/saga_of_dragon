#include "stdafx.h"
#include "ace/high_res_Timer.h"
#include "../header/CoreRegister.h"
#include "../CoreCenter.h"
#include "Cel_Log.h"

using namespace CEL;

bool CCoreRegister::VActivate()
{
	if( -1 == this->activate( THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1 ))//레지스터는 무조건 1개.
	{
		__asm int 3;
		return false;
	}
	return false;
}

bool CCoreRegister::VDeactivate()
{
#ifdef _DEBUG
	std::cout << "CCoreRegister::VDeactivate" << std::endl;
#endif
	m_kIsStop = true;
	int const iRet = ACE_Thread_Manager::instance()->wait_task( this );
	if( 0 == iRet )
	{
		return true;
	}
	return false;
}

int CCoreRegister::svc (void)
{
	::CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);

#ifdef _DEBUG
	std::cout << "CCoreRegister::svr [[[ENTER]]]" << std::endl;
#endif
	while( !IsStop() )//! stop 시그날 걸어서 꺼줄것.
	{
		SRegistInfo** ppkObj = NULL;

		ACE_Time_Value tv(1, 1000);	tv += ACE_OS::gettimeofday();
		if( -1 != msg_queue()->dequeue( ppkObj, &tv ) )
		{
			SRegistInfo* pkObj = (SRegistInfo*)ppkObj;

			g_kCoreCenter.RegistSub(pkObj);
			SAFE_DELETE(pkObj);
		}
	}
#ifdef _DEBUG
	std::cout << "CCoreRegister::svr [[[LEAVE]]]" << std::endl;
#endif
	::CoUninitialize();
	return 1;
}

BM::GUID CCoreRegister::Regist(ERegistType const eRT, tagRegist_Base const *pInitInfo)//소유자와 child 의 키
{//	queSendInfo.high_water_mark( 반드시 필요함.
	SRegistInfo *pObj= new SRegistInfo;
	if(pObj)
	{
//		pObj->vtInfo.resize(size);
//		void* pDest = &pObj->vtInfo.at(0);
		pObj->eType = eRT;

		switch( eRT )
		{
		case RT_ACCEPTOR:
		case RT_CONNECTOR:
			{
				INIT_CORE_DESC const* pkInitDesc = dynamic_cast<INIT_CORE_DESC const*>(pInitInfo);
				if(pInitInfo)
				{
					pObj->guidOrder = pkInitDesc->kOrderGuid;
					pkInitDesc->WriteToPacket(pObj->kPacket);
				}
			}break;
		case RT_DB_WORKER:
			{
				INIT_DB_DESC const* pkInitDesc = dynamic_cast<INIT_DB_DESC const*>(pInitInfo);

				if(pInitInfo)
				{
					pObj->guidOrder = pkInitDesc->kOrderGuid;
					pkInitDesc->WriteToPacket(pObj->kPacket);
				}
			}break;
		case RT_TIMER_FUNC:
			{
				REGIST_TIMER_DESC const* pkInitDesc = dynamic_cast<REGIST_TIMER_DESC const*>(pInitInfo);
				if(pInitInfo)
				{				
					pObj->guidOrder = pkInitDesc->kOrderGuid;
					pkInitDesc->WriteToPacket(pObj->kPacket);
				}
			}break;
		case RT_FINAL_SIGNAL:
			{
				const INIT_FINAL_SIGNAL* pkInitDesc = dynamic_cast<const INIT_FINAL_SIGNAL*>(pInitInfo);
				if(pInitInfo)
				{
					pObj->guidOrder = pkInitDesc->kOrderGuid;
					pkInitDesc->WriteToPacket(pObj->kPacket);
				}
			}break;
		default:
			{
				return BM::GUID::NullData();
			}break;
		}

		if(pObj->guidOrder != BM::GUID::NullData())
		{
			ACE_Time_Value tv(0, 1000);	tv += ACE_OS::gettimeofday();
			
			SRegistInfo** ppkObj = (SRegistInfo**)pObj;

			if( -1 != msg_queue()->enqueue(ppkObj, &tv) )
			{
				return pObj->guidOrder;
			}
		}
		
		SAFE_DELETE(pObj);
	}
	
	CEL_LOG(BM::LOG_LV0, _T("CCoreRegister::Regist Failed"));
	return BM::GUID::NullData();
}