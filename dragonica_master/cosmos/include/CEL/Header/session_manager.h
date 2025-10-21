#pragma once

#include "ace/Guard_T.h"
#include "ace/Lock.h"
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Inet_Addr.h"

#include "BM/PgTask.h"
#include "BM/observer.h"
#include "BM/STLSupport.h"
#include "BM/Guid.h"
#include "BM/Functor.h"
#include "BM/ObjectPool.h"
#include "BM/ObjectPool.h"

#include "CEL/Common.h"

namespace CEL
{
	template < typename T_SESSION >
	class CSession_Manager
		:	public BM::CObserver< eNOTIFY_CAUSE >
//		,	public BM::PgTask<
	{
	public:
		typedef typename std::map< BM::GUID, T_SESSION * const >	CONT_SESSION;//key = SessionGuid 
		typedef typename T_SESSION							SERVICE_HANDLER;
		typedef typename SERVICE_HANDLER::HEADER_TYPE		HEADER_TYPE;

	public:
		CSession_Manager(INIT_CORE_DESC const& rInfo);
		virtual ~CSession_Manager(void);

	public:
		virtual bool VSend( typename CONT_SESSION::key_type const& rHashKey,  BM::Stream const& rPacket);
		virtual bool Terminate( typename CONT_SESSION::key_type const& rHashKey );
		virtual bool TerminateAll();

		void OnConnectFail( ACE_INET_Addr const& remote, void const *pAct );//! 이건 옵저버 패턴에서 쓰는것이 아님.

		SERVICE_HANDLER* AcquireHandler();//! Onclose 에서 알아서 돌려줌.

		virtual void CheckKeepAlive(DWORD const dwSec);
		virtual HRESULT Statistics(typename CONT_SESSION::key_type const& rHashKey,  SSessionStatistics &rkOutResult)const;

		virtual void VDisplayState(PgLogWorker_Base &rkLogWorker, DWORD const iLogType)const;
	protected:
		virtual void VUpdate( BM::CSubject<eNOTIFY_CAUSE> * const pChangedSubject, eNOTIFY_CAUSE rkNfy);
		void OnOpen(SERVICE_HANDLER * const pServiceHandler);
		void OnClose(SERVICE_HANDLER * const pServiceHandler);

	protected:
		CONT_SESSION			m_mapSession;

	protected:
		//! On 시리즈 등록물 여기서 컨트롤
		INIT_CORE_DESC const& m_rInitInfo;
		CLASS_DECLARATION(BM::GUID, m_Guid, WorkerGuid );

		mutable ACE_RW_Thread_Mutex m_kMutex;
	};

	#include "Session_manager.inl"
};

