#pragma once
#include "CEL/Common.h"

namespace CEL
{
	class CWorker_Base
	{
		friend class CCoreCenter;
	protected:
		CWorker_Base(INIT_CORE_DESC const& rInitInfo)
			:	m_kInitInfo(rInitInfo)
		{	}
		virtual ~CWorker_Base()	{	}

	public:
		virtual bool VInit(BM::GUID const& rGuid, ADDR_INFO const& rBindAddr = ADDR_INFO(), ADDR_INFO const& rNATAddr = ADDR_INFO(), bool const bIsAllow = true) = 0;
		virtual bool VClose()	= 0;

		virtual bool VSend(BM::GUID const& rSessionGuid,  BM::Stream const& rPacket) = 0;
		virtual bool VTerminate(BM::GUID const& rkSessionGuid) = 0;

		virtual void VConnect(ADDR_INFO const& rAddr, void *pAct = NULL) = 0;//For Connector
		virtual bool VOpen()	= 0;//ForAcceptor

		virtual HRESULT Statistics(BM::GUID const& rSessionGuid,  SSessionStatistics &rkOutResult)const=0;

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const = 0;

	protected:
		INIT_CORE_DESC const m_kInitInfo;
		CLASS_DECLARATION_S(BM::GUID, WorkerGuid);
	};
}