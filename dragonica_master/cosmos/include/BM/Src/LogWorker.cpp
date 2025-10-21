#include "stdafx.h"
#include "ThreadObject.h"
#include "Header/DebugLog.h"
#include "XLogStream.h"
#include "LogWorker.h"

PgLogWorker::PgLogWorker(void)
{
}

PgLogWorker::~PgLogWorker(void)
{
	CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.begin();
	for( ; inst_itr != m_kContLogInst.end() ; ++inst_itr )
	{
		SAFE_DELETE( inst_itr->second );
	}
	m_kContLogInst.clear();
}

HRESULT PgLogWorker::VRegistInstance( REG_LOG_INSTANCE const &kRegInfo )
{	//로그 개체 등록.
	BM::PgDebugLog_Wrapper *pkInstance = new BM::PgDebugLog_Wrapper;
	if(pkInstance)
	{
		pkInstance->SetLogLimitLv( kRegInfo.kLogLimitLv );
		pkInstance->SetUseHeader( !(BM::OUTPUT_IGNORE_LOG_HEAD & kRegInfo.dwOutputMode) );

		if ( BM::OUTPUT_JUST_FILE & kRegInfo.dwOutputMode )
		{
			if ( !(BM::OUTPUT_JUST_FILE_MEMMAPPING & kRegInfo.dwOutputMode) )
			{
				BM::PgXLogStream_File *pkFileStream = new BM::PgXLogStream_File;
				if ( pkFileStream )
				{
					if ( SUCCEEDED(pkFileStream->Init( kRegInfo.wstrFileName, kRegInfo.wstrFolderName, kRegInfo.strLocale )) )
					{
						if ( FAILED(pkInstance->Regist( BM::OUTPUT_JUST_FILE, pkFileStream )) )
						{
							SAFE_DELETE(pkFileStream);
						}
					}
				}
			}
		}

		if ( BM::OUTPUT_JUST_TRACE & kRegInfo.dwOutputMode )
		{
			BM::PgXLogStream_Trace *pkTraceStream = new BM::PgXLogStream_Trace;
			if ( pkTraceStream )
			{
				if ( FAILED(pkInstance->Regist( BM::OUTPUT_JUST_TRACE, pkTraceStream )) )
				{
					SAFE_DELETE(pkTraceStream);
				}
			}
		}

		if ( BM::OUTPUT_JUST_CONSOLE & kRegInfo.dwOutputMode )
		{
			BM::PgXLogStream_Console *pkConsoleStream = new BM::PgXLogStream_Console;
			if ( pkConsoleStream )
			{
				BM::PgXLogStream_Console::CONT_LEVEL_BG kContLevelBG;
				kContLevelBG.reserve(10);

				kContLevelBG.push_back(BM::LC_RED);//LV0
				kContLevelBG.push_back(BM::LC_YELLOW);//LV1
				kContLevelBG.push_back(BM::LC_VIOLET);//LV2
				kContLevelBG.push_back(BM::LC_JADE);//LV3
				kContLevelBG.push_back(BM::LC_BLUE);//LV4
				kContLevelBG.push_back(BM::LC_GREEN);//LV5
				kContLevelBG.push_back(BM::LC_BLACK_I);//LV6
				kContLevelBG.push_back(BM::LC_BLACK);//LV7
				kContLevelBG.push_back(BM::LC_BLACK);//LV8
				kContLevelBG.push_back(BM::LC_BLACK);//LV9

				if ( SUCCEEDED(pkConsoleStream->Init( kRegInfo.eFontColor, kContLevelBG )) )
				{
					if ( FAILED(pkInstance->Regist( BM::OUTPUT_JUST_CONSOLE, pkConsoleStream )) )
					{
						SAFE_DELETE( pkConsoleStream );
					}
				}
			}
		}

		if ( BM::OUTPUT_JUST_FILE_MEMMAPPING & kRegInfo.dwOutputMode )
		{
			BM::PgXLogStream_FileMM *pkFileMapStream = new BM::PgXLogStream_FileMM;
			if ( pkFileMapStream )
			{
				if ( SUCCEEDED(pkFileMapStream->Init( kRegInfo.wstrFileName, kRegInfo.wstrFolderName )) )
				{
					if ( FAILED(pkInstance->Regist( BM::OUTPUT_JUST_FILE, pkFileMapStream )) )
					{
						SAFE_DELETE(pkFileMapStream);
					}
				}
			}
		}

		auto ret = m_kContLogInst.insert(std::make_pair(kRegInfo.dwLogWorkerType, pkInstance));
		if( true == ret.second )
		{
			return S_OK;
		}
	}

	SAFE_DELETE(pkInstance);
	return E_FAIL;
}

HRESULT PgLogWorker::VRegistInstanceStream( T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType, BM::PgXLogStream * pkLogStream )
{
	if ( pkLogStream )
	{
		CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.find( dwLogWorketType );
		if ( inst_itr != m_kContLogInst.end() )
		{
			return inst_itr->second->Regist( kType, pkLogStream );
		}
	}
	return E_FAIL;
}

HRESULT PgLogWorker::VUnRegistInstanceStream( T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType )
{
	CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.find( dwLogWorketType );
	if ( inst_itr != m_kContLogInst.end() )
	{
		return inst_itr->second->UnRegist( kType );
	}
	return E_FAIL;
}

HRESULT PgLogWorker::PushLog( SDebugLogMessage &kLogMsg )//
{//로그 기록
	CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.find( kLogMsg.LogWorkerType() );
	if( inst_itr != m_kContLogInst.end() )
	{
		inst_itr->second->LogNoArg( static_cast<int>(kLogMsg.LogLv()), kLogMsg.LogMsg() );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgLogWorker::SetLogLimitLv( T_WORKER_TYPE const dwWorkerType, BM::E_LOG_LEVEL const eLv )
{
	CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.find(dwWorkerType);
	if( inst_itr != m_kContLogInst.end() )
	{
		inst_itr->second->SetLogLimitLv( static_cast<int>(eLv) );
		return S_OK;
	}

	return E_FAIL;
}

HRESULT PgLogWorker::SetLogEnable( T_WORKER_TYPE const dwWorkerType, DWORD const dwOutPutMode )
{
	CONT_LOG_INSTANCE::iterator inst_itr = m_kContLogInst.find(dwWorkerType);
	if( inst_itr != m_kContLogInst.end() )
	{
		BM::PgDebugLog_Wrapper* pkDebugWrapper = inst_itr->second;

		pkDebugWrapper->SetEnable( BM::OUTPUT_JUST_FILE, BM::OUTPUT_JUST_FILE & dwOutPutMode );
		pkDebugWrapper->SetEnable( BM::OUTPUT_JUST_CONSOLE, BM::OUTPUT_JUST_CONSOLE & dwOutPutMode );
		pkDebugWrapper->SetEnable( BM::OUTPUT_JUST_TRACE, BM::OUTPUT_JUST_TRACE & dwOutPutMode );
		return S_OK;
	}
	return E_FAIL;
}

bool PgLogWorker::HasWorkerType( T_WORKER_TYPE const dwWorkerType )const
{
	CONT_LOG_INSTANCE::const_iterator inst_itr = m_kContLogInst.find(dwWorkerType);
	return inst_itr != m_kContLogInst.end();
}


//
PgLogWorker_Wrapper::PgLogWorker_Wrapper(void)
{

}

PgLogWorker_Wrapper::~PgLogWorker_Wrapper(void)
{

}

HRESULT PgLogWorker_Wrapper::VRegistInstance( REG_LOG_INSTANCE const &kRegInfo )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->VRegistInstance( kRegInfo );
}

HRESULT PgLogWorker_Wrapper::VRegistInstanceStream( PgLogWorker::T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType, BM::PgXLogStream * pkLogStream )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	return Instance()->VRegistInstanceStream( dwLogWorketType, kType, pkLogStream );
}

HRESULT PgLogWorker_Wrapper::VUnRegistInstanceStream( PgLogWorker::T_WORKER_TYPE const dwLogWorketType, BM::E_OUPUT_TYPE const kType )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	return Instance()->VUnRegistInstanceStream( dwLogWorketType, kType );
}

HRESULT PgLogWorker_Wrapper::PushLog(SDebugLogMessage &kLogMsg)
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	return Instance()->PushLog( kLogMsg );
}

HRESULT PgLogWorker_Wrapper::SetLogLimitLv( PgLogWorker::T_WORKER_TYPE const dwWorkerType, BM::E_LOG_LEVEL const eLv )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	return Instance()->SetLogLimitLv( dwWorkerType, eLv );
}

HRESULT PgLogWorker_Wrapper::SetLogEnable( PgLogWorker::T_WORKER_TYPE const dwWorkerType, DWORD const dwOutPutMode )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	return Instance()->SetLogEnable( dwWorkerType, dwOutPutMode );
}


//
PgLogWorker_Wrapper_Task::PgLogWorker_Wrapper_Task(void)
:	PgTask< SDebugLogMessage >( true )
{
}

PgLogWorker_Wrapper_Task::~PgLogWorker_Wrapper_Task(void)
{
	PgTask< SDebugLogMessage >::Terminate();
}

HRESULT PgLogWorker_Wrapper_Task::PushLog(SDebugLogMessage &kLogMsg)
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	if ( true == Instance()->HasWorkerType( kLogMsg.LogWorkerType() ) )
	{
		if ( true == this->PutMsg( kLogMsg ) )
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

void PgLogWorker_Wrapper_Task::SetMaxQueueSize( WORD const wSize )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );

	size_t const iHwm = static_cast<size_t>(wSize) * sizeof(MSG*);
	SetHighWaterMark( iHwm );
}

void PgLogWorker_Wrapper_Task::HandleMessage( MSG *pkMsg )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_ );
	Instance()->PushLog( *pkMsg );
}
