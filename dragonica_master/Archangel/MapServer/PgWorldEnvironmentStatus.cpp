#include "stdafx.h"
#include "PgWorldEnvironmentStatus.h"

PgWorldEnvironmentStatus::PgWorldEnvironmentStatus()
	: m_kContEnv(), m_kResult()
{
}
PgWorldEnvironmentStatus::PgWorldEnvironmentStatus(PgWorldEnvironmentStatus const& rhs)
	: m_kContEnv(rhs.m_kContEnv), m_kResult(rhs.m_kResult)
{
}
PgWorldEnvironmentStatus::~PgWorldEnvironmentStatus()
{
}

void PgWorldEnvironmentStatus::operator =(PgWorldEnvironmentStatus const& rhs)
{
}
bool PgWorldEnvironmentStatus::Update(SWorldEnvironmentStatus const& rkNewEnvStatus)
{
	//BM::CAutoMutex kLock(m_kMutex);
	DWORD const dwOld = m_kResult.dwEnvStatus;
	auto kRet = m_kContEnv.insert( std::make_pair(rkNewEnvStatus.eType, rkNewEnvStatus) );
	if( !kRet.second )
	{
		(*kRet.first).second = rkNewEnvStatus;
	}
	m_kResult.Update(m_kContEnv);
	return dwOld != m_kResult.dwEnvStatus;
}
bool PgWorldEnvironmentStatus::Del(EWorldEnvironmentStatusType const& reType)
{
	//BM::CAutoMutex kLock(m_kMutex);
	DWORD const dwOld = m_kResult.dwEnvStatus;
	m_kContEnv.erase( reType );
	m_kResult.Update(m_kContEnv);
	return dwOld != m_kResult.dwEnvStatus;
}
bool PgWorldEnvironmentStatus::Tick()
{
	//BM::CAutoMutex kLock(m_kMutex);
	__int64 const iCurGameTime = g_kEventView.GetGameSecTime();
	DWORD const dwOld = m_kResult.dwEnvStatus;
	CONT_WORLD_ENVIRONMENT_STATUS::iterator iter = m_kContEnv.begin();
	while( m_kContEnv.end() != iter )
	{
		if( !(*iter).second.IsCanTime(iCurGameTime) )
		{
			iter = m_kContEnv.erase( iter );
		}
		else
		{
			++iter;
		}
	}
	m_kResult.Update(m_kContEnv);
	return dwOld != m_kResult.dwEnvStatus;
}
DWORD PgWorldEnvironmentStatus::GetFlag() const
{
	//BM::CAutoMutex kLock(m_kMutex);
	return m_kResult.dwEnvStatus;
}