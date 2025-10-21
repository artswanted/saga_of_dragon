#pragma once
#include "BM/PgTask.h"
#include "CEL/CoreWorker_Base.h"

/*
namespace CEL
{
#pragma pack(1)
	typedef struct tagCoreWorkerMsg
	{
		tagCoreWorkerMsg()
		{
			Clear();
		}

		void Clear()
		{
			MsgType(0);
			m_kPacket.Clear();
			m_kSessionKey.Clear();
		}

		CLASS_DECLARATION_S(int, MsgType);
		CLASS_DECLARATION_S(SESSION_KEY, SessionKey);
		CLASS_DECLARATION_S(BM::Stream, Packet);
	}SCoreWorkerMsg;
#pragma pack()

	class CCoreWorker
		:	public PgTask<SCoreWorkerMsg>
		,	public CCoreWorker_Base
	{
		typedef SCoreWorkerMsg T_MSG;

		friend class CCoreCenter;
		typedef enum eValue
		{
			E_WATER_MARK	= 500 * 1024,//! 500K 까지 버텨준다.
		}E_VALUE;

	public:
		CCoreWorker()
		{
			msg_queue()->high_water_mark( E_WATER_MARK );
			msg_queue()->low_water_mark(0);//! -> 이거 나중에 어떻게 좀 해보자.
			m_kIsStop = false;
		}

		virtual ~CCoreWorker(){}

	public://Worker_Base
		virtual bool VActivate(int const iForceThreadCount);
		virtual bool VDeactivate();
	
		virtual bool Send(SESSION_KEY const& kSessionKey,  BM::Stream const& rPacket);//소유자와 child 의 키

	public://PgTask
		virtual void HandleMessage(T_MSG *pkMsg);
	};
}
*/