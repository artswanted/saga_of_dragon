#include "StdAfx.h"
#include "constant.h"
#include "PgDefPropertyMgr.h"
#include "DefAbilType.h"
#include "PgControlDefMgr.h"

bool PgDefPropertyMgr::Build(const CONT_DEF_PROPERTY & kContDefProperty)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bReturn = true;
	Clear();

	static int const aiAttr[E_PPTY_BASIC_MAX] = 
		{	E_PPTY_NONE,
			E_PPTY_FIRE,
			E_PPTY_WATER,
			E_PPTY_WOOD,
			E_PPTY_METAL,
			E_PPTY_EARTH
		};
	
	for(int i = 0;i < E_PPTY_BASIC_MAX;i++)
	{
		for(int iLevel = 1;iLevel <= PROPERTY_LEVEL_LIMIT;iLevel++)
		{
			CONT_DEF_PROPERTY::const_iterator itor = kContDefProperty.find(TBL_DEF_PROPERTY_KEY(aiAttr[i],iLevel));
			if (itor == kContDefProperty.end())
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find Base Property["<<aiAttr[i]<<L","<<iLevel<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return false;
			}
			m_kContProperty.insert(std::make_pair(itor->first, itor->second));
		}
	}

	return bReturn;
}

void PgDefPropertyMgr::Clear()
{
	m_kContProperty.clear();
}

void PgDefPropertyMgr::swap(PgDefPropertyMgr &rRight)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kContProperty.swap(rRight.m_kContProperty);
}

int PgDefPropertyMgr::GetRate(int iOffense, int iOffenseLv, int iDefence, int iDefenceLv) const
{
	BM::CAutoMutex lock(m_kMutex);
	
	if(!iOffense)
	{
		iOffenseLv = 1;
	}

	if(!iDefence)
	{
		iDefenceLv = 1;
	}

	CONT_DEF_PROPERTY::const_iterator itor = m_kContProperty.find( TBL_DEF_PROPERTY_KEY(iOffense, iOffenseLv) );
	if(itor == m_kContProperty.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	int const iRatePos = CALC_PROPERTY_RATE_POS(iDefence,iDefenceLv);
	if(int((*itor).second.kContRate.size()) <= iRatePos)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	return (*itor).second.kContRate.at(iRatePos);
}
