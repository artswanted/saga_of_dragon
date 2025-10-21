#pragma once

#include "ACE/Task_ex_t.h"
#include "ACE/message_queue_t.h"

#include "BM/Stream.h"
#include "CEL/CoreRegister_Base.h"


namespace CEL
{
	class CCoreRegister
		:	public CCoreRegister_Base
		,	public ACE_Task_Ex< ACE_MT_SYNCH, SRegistInfo* >
	{
		friend class CCoreCenter;
		typedef enum eValue
		{
			E_WATER_MARK	= 500 * 1024,//! 500K 까지 버텨준다.
		}E_VALUE;
	public:
		CCoreRegister()
		{
			m_kIsStop = false;
			msg_queue()->high_water_mark(E_WATER_MARK);//대충;
		}
		virtual ~CCoreRegister(){}

	public:
		virtual bool VActivate();
		virtual bool VDeactivate();
		virtual int svc (void);

		virtual BM::GUID Regist(ERegistType const eRT, tagRegist_Base const *pInitInfo);//! 등록된 결과 GUID를 반환 //바로 스레드가 시작. Init 에 스레드 값을 셋팅하고 SvcStart 를 하는게 정석.

	protected:
		CLASS_DECLARATION_S_NO_SET( bool, IsStop );
	};
}