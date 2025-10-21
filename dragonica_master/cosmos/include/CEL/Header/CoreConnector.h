#pragma once

#include "ACE/Task_ex_t.h"
#include "ACE/message_queue_t.h"
#include "BM/ObjectPool.h"
#include "BM/Stream.h"

#include "CEL/CoreConnector_Base.h"


namespace CEL
{
	class CCoreConnector
		:	public CCoreConnector_Base
		,	public ACE_Task_Ex< ACE_MT_SYNCH, SConnectInfo* >
	{
		friend class CCoreCenter;
	public:
		CCoreConnector()
		{
			m_kIsStop = false;
		}
		virtual ~CCoreConnector(){}

	public:
		virtual bool VActivate();
		virtual bool VDeactivate();
		
		virtual int svc (void);

		virtual bool Connect(ADDR_INFO const& rAddr, SESSION_KEY &kSessionKey, BM::GUID &kConnectObj);//! 등록된 결과 GUID를 반환 //바로 스레드가 시작. Init 에 스레드 값을 셋팅하고 SvcStart 를 하는게 정석.

	protected:
		CLASS_DECLARATION_S_NO_SET( bool, IsStop );
	};
}