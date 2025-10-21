#pragma once
/*
#include "BM/Observer.h"
#include "BM/Functor.h"

#include "Loki/Typelist.h"
#include "Loki/TypelistMacros.h"

namespace CEL
{
//! #pragma pack(1)
	template< typename T_SESSION >
	class CPacketHandler_Arg
	{
		typedef typename T_SESSION::REAL_PACKET REAL_PACKET;
	public:
		CPacketHandler_Arg(T_SESSION &rSession, REAL_PACKET &rRP)
			:	m_rSession(rSession)
			,	m_rRealPacket(rRP)
		{
		}

		~CPacketHandler_Arg()
		{
		}
	public:
		T_SESSION&			Session(){return m_rSession;}
		REAL_PACKET&		Packet(){return m_rRealPacket;}

	protected:
		T_SESSION		&m_rSession;	
		REAL_PACKET		&m_rRealPacket;
	};
	
	template< typename T_SESSION, typename T_PACKET_TYPE, typename T_PACKET >//! T_SESSION_MGR -> 패킷 처리시 세션에 락을 걸기위함.
	class CPacket_Handler
//! 	:	public BM::CFunctor< T_PACKET_TYPE, CPacketHandler_Arg< typename T_SESSION_MGR::SESSION > >
//! 	,	public BM::CObserver
	{

//! 	typedef ACE_Mutex PACKET_HANDLE_MUTEX;

//! 	PACKET_HANDLE_MUTEX m_packethandle_mutex;

//! 	typedef typename T_SESSION_MGR::SESSION SESSION;
//! 	typedef typename SESSION::SERVICE_HANDLER SERVICE_HANDLER;
	public:
//! 	typedef typename CPacketHandler_Arg< SESSION > HANDLER_ARG;
	public:
//! 	CPacket_Handler(T_SESSION_MGR &rSessionMgr)
//! 		:	m_rSessionMgr(rSessionMgr)	{}
//! 	virtual ~CPacket_Handler(void){}

//! 	T_SESSION_MGR &m_rSessionMgr;

	public:
		virtual int svc(void)
		{
			return 0;
		}

		virtual void VUpdate( BM::CSubject<eNOTIFY_CAUSE> *pChangedSubject, __int32 const iNotifyCause)
		{

		}
	};
//! #pragma pack()
};
*/