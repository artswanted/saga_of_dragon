#pragma once

#include "BM/TWrapper.h"
#include "BM/LogWorker_Base.h"

namespace BM
{
	class PgXLogStream;
	class PgDebugLog_Wrapper;
}

class PgLogWorker
{
public:
	typedef DWORD T_WORKER_TYPE;

public:
	PgLogWorker(void);
	virtual ~PgLogWorker(void);

public:
	virtual HRESULT VRegistInstance( REG_LOG_INSTANCE const &kRegInfo );
	virtual HRESULT VRegistInstanceStream( T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType, BM::PgXLogStream * pkLogStream );
	virtual HRESULT VUnRegistInstanceStream( T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType );
	virtual HRESULT PushLog(SDebugLogMessage &kLogMsg);
	
	HRESULT SetLogLimitLv( T_WORKER_TYPE const dwWorkerType, BM::E_LOG_LEVEL const eLv );
	HRESULT SetLogEnable( T_WORKER_TYPE const dwWorkerType, DWORD const dwOutPutMode );

	bool HasWorkerType( T_WORKER_TYPE const dwWorkerType )const;

protected:
	typedef std::map< T_WORKER_TYPE, BM::PgDebugLog_Wrapper* > CONT_LOG_INSTANCE;
	CONT_LOG_INSTANCE m_kContLogInst;
};

class PgLogWorker_Wrapper
	:	public PgLogWorker_Base
	,	protected TWrapper< PgLogWorker >
{
public:
	PgLogWorker_Wrapper(void);
	virtual ~PgLogWorker_Wrapper(void);

	virtual HRESULT VRegistInstance(REG_LOG_INSTANCE const &kRegInfo);
	virtual HRESULT VRegistInstanceStream( PgLogWorker::T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType, BM::PgXLogStream * pkLogStream );
	virtual HRESULT VUnRegistInstanceStream( PgLogWorker::T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType );
	virtual HRESULT PushLog(SDebugLogMessage &kLogMsg);
	
	HRESULT SetLogLimitLv( PgLogWorker::T_WORKER_TYPE const dwWorkerType, BM::E_LOG_LEVEL const eLv );
	HRESULT SetLogEnable( PgLogWorker::T_WORKER_TYPE const dwWorkerType, DWORD const dwOutPutMode );
};

class PgLogWorker_Wrapper_Task
	:	public PgLogWorker_Wrapper
	,	public PgTask< SDebugLogMessage >
{
public:
	PgLogWorker_Wrapper_Task(void);
	virtual ~PgLogWorker_Wrapper_Task(void);

	virtual HRESULT PushLog(SDebugLogMessage &kLogMsg);

	// high water mark°¡ ¾Æ´Ô(¸Þ¼¼Áö °¹¼öÀÓ high water mark´Â BYTE)
	virtual void SetMaxQueueSize( WORD const wSize );

protected:
	virtual void HandleMessage( MSG *pkMsg );
};

//#define g_kLogWorker	SINGLETON_STATIC(PgLogWorker_Wrapper)
#define g_kLogWorker	SINGLETON_STATIC(PgLogWorker_Wrapper_Task)
