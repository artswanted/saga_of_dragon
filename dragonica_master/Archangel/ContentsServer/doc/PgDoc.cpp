#include "stdafx.h"
#include "PgDoc.h"

PgDoc::PgDoc(void)
:	m_kCanDeleteTime(ACE_Time_Value::zero)
{
}

PgDoc::~PgDoc(void)
{
}

PgDoc::PgDoc( PgDoc const & rhs )
:	m_kCanDeleteTime(rhs.m_kCanDeleteTime)
{
}

PgDoc& PgDoc::operator = ( PgDoc const & rhs )
{
	m_kCanDeleteTime = rhs.m_kCanDeleteTime;
	return *this;
}

bool PgDoc::CanDelete(ACE_Time_Value const &kNowTime)const
{
// 	if(m_kContWorkQuery.size())
// 	{
// 		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
// 		return false;
// 	}

	if(m_kCanDeleteTime < kNowTime)
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgDoc::SetDeletable()
{
	m_kCanDeleteTime = ACE_OS::gettimeofday() + ACE_Time_Value(30*60);//30분 유예기간.
}
