#include "stdafx.h"
#include "PgJobChecker.h"

volatile LONG PgJobChecker::ms_lAddJobCount = 0;
volatile LONG PgJobChecker::ms_lCompleteJobCount = 0;
DWORD PgJobChecker::ms_dwJobStartTime = 0;

void PgJobChecker::AddJob()
{
	if ( 1 == ::InterlockedIncrement( &ms_lAddJobCount ) )
	{
		ms_dwJobStartTime = BM::GetTime32();
	}
}

void PgJobChecker::CompleteJob()
{
	::InterlockedIncrement( &ms_lCompleteJobCount );
}

void PgJobChecker::DisplayJobState()
{
	DWORD const dwTime = (BM::GetTime32() - ms_dwJobStartTime) / 1000;
	INFO_LOG( BM::LOG_RED, __FL__ << _T("Job Count Add[") << ms_lAddJobCount << _T("] Complete[") << ms_lCompleteJobCount << _T("] / Speed ") << (dwTime ? ms_lCompleteJobCount/dwTime : 0) << _T(" for Second") );
}

void PgJobChecker::ResetJobState()
{
	ms_lAddJobCount -= ms_lCompleteJobCount;
	ms_lCompleteJobCount = 0;
	ms_dwJobStartTime = BM::GetTime32();
	INFO_LOG( BM::LOG_RED, __FL__ );
}

PgJobChecker::PgJobChecker()
:	m_iTotalCount(0)
,	m_dwStartTime( BM::GetTime32() )
{
}

PgJobChecker::~PgJobChecker()
{

}

PgJobChecker::PgJobChecker( PgJobChecker const &rhs )
{
	*this = rhs;
}

PgJobChecker& PgJobChecker::operator=( PgJobChecker const &rhs )
{
	m_kContJobChecker = rhs.m_kContJobChecker;
	m_iTotalCount = rhs.m_iTotalCount;
	m_dwStartTime = rhs.m_dwStartTime;
	return *this;
}

void PgJobChecker::Add( __int64 const iType )
{
	++m_iTotalCount;

	CONT_COUNT::iterator itr = m_kContJobChecker.find( iType );
	if ( itr != m_kContJobChecker.end() )
	{
		++(itr->second);
		return;
	}

	m_kContJobChecker.insert( std::make_pair( iType, 1 ) );
}

void PgJobChecker::Display()const
{
	DWORD const dwTime = (BM::GetTime32() - m_dwStartTime) / 1000;

	CONT_COUNT::const_iterator itr = m_kContJobChecker.begin();
	for ( ; itr!=m_kContJobChecker.end() ; ++itr )
	{
		INFO_LOG( BM::LOG_YELLOW, __FL__ << _T("* TYPE[") << itr->first << _T("] Count : ") << itr->second );
	}


	INFO_LOG( BM::LOG_YELLOW, __FL__ << _T("* TotalCount[") << m_iTotalCount << _T("], TotalTime[") << dwTime << _T(" Sec]") );
}