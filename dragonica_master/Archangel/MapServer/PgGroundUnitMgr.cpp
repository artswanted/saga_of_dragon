#include "stdafx.h"
#include "PgGroundUnitMgr.h"
#include "Variant/Global.h"

PgGroundUnitMgr::PgGroundUnitMgr()
{

}

PgGroundUnitMgr::~PgGroundUnitMgr()
{

}

bool PgGroundUnitMgr::Add(CUnit* pkUnit, int iGroundNo, T_GNDATTR const kGndAttr)
{
	if( ( ( kGndAttr & GATTR_FLAG_PVP ) || ( kGndAttr & GATTR_FLAG_MISSION ) || 
		( kGndAttr & GATTR_FLAG_BOSS ) || ( kGndAttr & GATTR_CHAOS_MISSION ) ) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( iGroundNo && pkUnit && pkUnit->UnitType() == UT_PLAYER )
	{
		BM::CAutoMutex kLock(m_kMutex);		

		auto bRet = ContUnitAll.insert(std::make_pair(pkUnit->GetID(), CONT_UNIT_MGR::mapped_type() ));
		auto bSunRet = (*bRet.first).second.insert(std::make_pair(iGroundNo, pkUnit));

		if( !bSunRet.second )
		{
			CONT_UNIT_MGR::iterator iter = ContUnitAll.find(pkUnit->GetID());
			if( iter != ContUnitAll.end() )
			{
				iter = ContUnitAll.erase(iter);

				auto bRet = ContUnitAll.insert(std::make_pair(pkUnit->GetID(), CONT_UNIT_MGR::mapped_type() ));
				auto bSunRet = (*bRet.first).second.insert(std::make_pair(iGroundNo, pkUnit));
				if( bSunRet.second )
				{
					return true;
				}
			}
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}			
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgGroundUnitMgr::Del(CUnit* pkUnit)
{
	if( pkUnit && pkUnit->UnitType() == UT_PLAYER )
	{
		BM::CAutoMutex kLock(m_kMutex);

		CONT_UNIT_MGR::iterator iter = ContUnitAll.find(pkUnit->GetID());
		if( iter != ContUnitAll.end() )
		{
			iter = ContUnitAll.erase(iter);

			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgGroundUnitMgr::Get(BM::GUID rkCharGuid, CONT_SEARCH_UNIT_INFO &kUnitArray)
{
	BM::CAutoMutex kLock(m_kMutex);

	int iCount = 0;
	SSearchPeopleInfo kInfo;

	CONT_UNIT_MGR::iterator iter = ContUnitAll.begin();
	while( iter != ContUnitAll.end() )
	{
		CONT_UNIT_MGR::mapped_type &kElements = (*iter).second;
		CONT_UNIT_MGR::mapped_type::iterator &kElement = kElements.begin();

		if( rkCharGuid == kElement->second->GetID() )
		{
			++iter;
			continue;
		}

		PgPlayer *pkUser = dynamic_cast<PgPlayer*>(kElement->second);
		if( pkUser->HaveParty() )
		{
			++iter;
			continue;
		}	

		kInfo.Clear();
		kInfo.kCharGuid = kElement->second->GetID();
		kInfo.iGndNo = kElement->first;
		kInfo.Name = kElement->second->Name();
		kInfo.iLevel = kElement->second->GetAbil(AT_LEVEL);

		kUnitArray.push_back(kInfo);

		iCount++;
		if( iCount >= PV_MAX_LIST_CNT )
		{
			break; 
		}

		++iter;
	}
}