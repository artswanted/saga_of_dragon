#include "stdafx.h"
#include "PgComboCounter.h"
#include "PgComboCounterMgr.h"

BM::TObjectPool< PgComboCounter > g_kPoolComboCounter(200, 50);

PgComboCounterMgr::PgComboCounterMgr()
{
}

PgComboCounterMgr::~PgComboCounterMgr()
{
	Clear();
}

void PgComboCounterMgr::Clear()
{
	BM::CAutoMutex kLock(m_kCounterMutex);
	ContCombo::iterator itr = m_kMap.begin();
	for ( ; itr!=m_kMap.end() ; ++itr )
	{
		SAFE_DELETE( itr->second );
	}
	m_kMap.clear();
}

bool PgComboCounterMgr::AddComboCounter(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock(m_kCounterMutex);

	PgComboCounter* pkCounter = NULL;
	bool const bFind = GetComboCounter(rkGuid, pkCounter);
	if( bFind )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//중복
	}

	pkCounter = g_kPoolComboCounter.New();
	if( !pkCounter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	pkCounter->Clear();
	pkCounter->OwnerGuid(rkGuid);

	auto kRet = m_kMap.insert( std::make_pair(rkGuid, pkCounter) );
	if( !kRet.second )
	{
		g_kPoolComboCounter.Delete(pkCounter);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	return true;
}

bool PgComboCounterMgr::DelComboCounter(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock(m_kCounterMutex);
	ContCombo::iterator iter = m_kMap.find(rkGuid);
	if( m_kMap.end() == iter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;//찾을 수 없다
	}

	PgComboCounter* pkCounter = (*iter).second;
	if( pkCounter )
	{
		g_kPoolComboCounter.Delete(pkCounter);
	}

	m_kMap.erase(iter);
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return true"));
	return true;
}

bool PgComboCounterMgr::GetComboCounter(BM::GUID const &rkGuid, PgComboCounter*& pkOut) const
{
	BM::CAutoMutex kLock(m_kCounterMutex);
	ContCombo::const_iterator iter = m_kMap.find(rkGuid);
	if( m_kMap.end() != iter )
	{
		pkOut = (*iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

COMBO_TYPE PgComboCounterMgr::GetCurrentCombo(BM::GUID const &rkGuid, const ECOMBO_STYLE kStyle) const
{
	BM::CAutoMutex kLock(m_kCounterMutex);
	
	PgComboCounter* pkCounter = NULL;
	bool const bFindCounter = GetComboCounter(rkGuid, pkCounter);
	if( !bFindCounter )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	return pkCounter->GetComboCount(kStyle);
}