#include "stdafx.h"
#include "Global.h"
#include "PgAggroMeter.h"
#include "PgAggroMeterMgr.h"

BM::TObjectPool< PgAggroMeter > g_kAggroMeterPool(500, 100);

PgAggroMeterMgr::PgAggroMeterMgr()
{
}

PgAggroMeterMgr::~PgAggroMeterMgr()
{
	Clear();
}

void PgAggroMeterMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMeterMutex);
	ContAggroMeter::iterator meter_iter = m_kMap.begin();
	while(m_kMap.end() != meter_iter)
	{
		ContAggroMeter::mapped_type pkElement = (*meter_iter).second;
		g_kAggroMeterPool.Delete(pkElement);
		++meter_iter;
	}
	m_kMap.clear();
}

bool PgAggroMeterMgr::AddMeter(BM::GUID const &rkGuid, int const iHP)
{
	BM::CAutoMutex kLock(m_kMeterMutex);

	PgAggroMeter* pkMeter = NULL;
	bool const bFindMeter = GetMeter(rkGuid, pkMeter);
	if( !bFindMeter )
	{
		pkMeter = g_kAggroMeterPool.New();
		if( pkMeter )
		{
			pkMeter->Clear();
			pkMeter->Set(rkGuid, iHP);

			auto kRet = m_kMap.insert( std::make_pair(rkGuid, pkMeter) );
			if( kRet.second )
			{
				return true;
			}
			g_kAggroMeterPool.Delete(pkMeter);
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAggroMeterMgr::GetMeter(BM::GUID const &rkGuid, PgAggroMeter*& pkAggroMeter)
{
	BM::CAutoMutex kLock(m_kMeterMutex);
	ContAggroMeter::iterator meter_iter = m_kMap.find(rkGuid);
	if( m_kMap.end() != meter_iter )
	{
		pkAggroMeter = (*meter_iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgAggroMeterMgr::DelMeter(BM::GUID const &rkGuid)
{
	BM::CAutoMutex kLock(m_kMeterMutex);
	PgAggroMeter* pkMeter = NULL;
	ContAggroMeter::iterator meter_iter = m_kMap.find(rkGuid);
	if( m_kMap.end() != meter_iter )
	{
		pkMeter = (*meter_iter).second;
		pkMeter->Clear();

		m_kMap.erase(meter_iter);
		g_kAggroMeterPool.Delete(pkMeter);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}