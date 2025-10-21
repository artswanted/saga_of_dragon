#pragma once
#include "ace/Asynch_Acceptor.h"
#include "CEL/Worker_Base.h"
#include "Cel_Log.h"

namespace CEL
{
	template < typename T_SESSION_MGR >
	class CAcceptor
		:	public CWorker_Base
		,	public ACE_Asynch_Acceptor< typename T_SESSION_MGR::SERVICE_HANDLER >
	{//! Acceptor와 Connector 는 Session 을 사용치 않는다..
	public:
		typedef typename T_SESSION_MGR	SESSION_MGR;
		typedef typename T_SESSION_MGR::SERVICE_HANDLER SERVICE_HANDLER;

	public:
		CAcceptor( INIT_CORE_DESC const& rInitInfo, size_t const page_pool_size = 0 );
		virtual ~CAcceptor(void);

	public:
		virtual bool VInit(BM::GUID const& rGuid, ADDR_INFO const& rBindAddr, ADDR_INFO const& rNATAddr, bool const bIsAllow = true);
		virtual bool VClose();

		virtual bool VSend(BM::GUID const& rSessionGuid,  BM::Stream const& rPacket);
		virtual bool VTerminate(BM::GUID const& rkSessionGuid);

		virtual void VConnect(ADDR_INFO const& rAddr, void *pAct){}//무동작.
		virtual bool VOpen();

		virtual SERVICE_HANDLER* make_handler(void);
		virtual int validate_connection (const ACE_Asynch_Accept::Result& result, ACE_INET_Addr const& remote, ACE_INET_Addr const& local);

		virtual HRESULT Statistics(BM::GUID const& rSessionGuid,  SSessionStatistics &rkOutResult)const;
		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
	public:
		bool IsAllow()const;
		void Allow(bool const bIsAllowAccept);
		
		std::string IP( bool const bIsBindIP = false ) const;
		WORD const Port(bool const bIsBindPort = false ) const;

	protected:
		T_SESSION_MGR		m_kSessionMgr;
		volatile long		m_lIsAllow;

		size_t const m_page_pool_size;

		ACE_INET_Addr m_BindAddr;
		ACE_INET_Addr m_NAT_Addr;
	};
	#include "Acceptor.inl"
};