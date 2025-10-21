#pragma once
#include "ace/Asynch_Connector.h"
#include "BM/STLSupport.h"
#include "BM/Observer.h"
#include "CEL/Worker_Base.h"

namespace CEL
{
	template < typename T_SESSION_MGR >
	class CConnector
		:	public CWorker_Base
		,	public ACE_Asynch_Connector< typename T_SESSION_MGR::SERVICE_HANDLER >
	{
	public:
		typedef typename T_SESSION_MGR	SESSION_MGR;
		typedef typename T_SESSION_MGR::SERVICE_HANDLER SERVICE_HANDLER;
		
	public:
		CConnector(INIT_CORE_DESC const& rInitInfo);
		virtual ~CConnector(void);

	public:
		virtual bool VInit(BM::GUID const& rGuid, ADDR_INFO const& rBindAddr, ADDR_INFO const& rNATAddr, bool const bIsAllow = true);
		virtual bool VClose();

		virtual bool VSend(BM::GUID const& rSessionGuid,  BM::Stream const& rPacket);
		virtual bool VTerminate(BM::GUID const& rkSessionGuid);

		virtual void VConnect(ADDR_INFO const& rAddr, void *pAct = NULL);
		virtual bool VOpen(){return false;}//¹«µ¿ÀÛ

		virtual SERVICE_HANDLER* make_handler(void);
		virtual int validate_connection (const ACE_Asynch_Connect::Result& result, ACE_INET_Addr const& remote, ACE_INET_Addr const& local);

		int Connect(ACE_INET_Addr const& Addr, void const *pAct = NULL );
		int Connect(std::wstring const& wstrRemoteIP, WORD const wPort,  void const *pAct = NULL );

		virtual HRESULT Statistics(BM::GUID const& rSessionGuid,  SSessionStatistics &rkOutResult)const;

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
	public:
		bool IsAllow()const;
		void Allow(bool const bIsAllowConnect);

	protected:
		T_SESSION_MGR	m_kSessionMgr;
		volatile long	m_lIsAllow;
	};

	#include "Connector.inl"
//! #pragma pack ()
};