#include "stdafx.h"
#include "PgSystemInvManager.h"

HRESULT PgSystemInvManager::AddSysItem(BM::GUID const & kCharGuid, PgBase_Item const & kItem)
{
	BM::CAutoMutex kLock(m_kMutex);

	auto kRet = m_kContContSysItem[kCharGuid].insert(std::make_pair(kItem.Guid(),SSYSSimpleItemInfo(kItem)));
	if(!kRet.second)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	return S_OK;
}

PgBase_Item PgSystemInvManager::GetSysItem(BM::GUID const & kCharGuid,BM::GUID const & kGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CONT_SYS_ITEM::iterator sysiter = m_kContContSysItem.find(kCharGuid);
	if(sysiter == m_kContContSysItem.end())
	{
		return PgBase_Item::NullData();
	}

	CONT_SYS_ITEM & kCont = (*sysiter).second;

	CONT_SYS_ITEM::const_iterator iter = kCont.find(kGuid);
	if(iter == kCont.end())
	{
		return PgBase_Item::NullData();
	}
	return (*iter).second.kSysItem;
}

bool PgSystemInvManager::RemoveSysItem(BM::GUID const & kCharGuid,BM::GUID const & kGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	CONT_CONT_SYS_ITEM::iterator sysiter = m_kContContSysItem.find(kCharGuid);
	if(sysiter == m_kContContSysItem.end())
	{
		return false;
	}

	CONT_SYS_ITEM & kCont = (*sysiter).second;

	CONT_SYS_ITEM::iterator iter = kCont.find(kGuid);
	if(iter == kCont.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	kCont.erase(iter);

	if(true == kCont.empty())
	{
		m_kContContSysItem.erase(sysiter);
	}
	return true;
}

void PgSystemInvManager::ClearTimeOutSysItem(BM::GUID const & kCharGuid)
{
	BM::CAutoMutex kLock(m_kMutex);

	DWORD dwCurTime = BM::GetTime32();

	CONT_CONT_SYS_ITEM::iterator sysiter = m_kContContSysItem.find(kCharGuid);
	if(sysiter == m_kContContSysItem.end())
	{
		return;
	}

	CONT_SYS_ITEM & kCont = (*sysiter).second;

	CONT_SYS_ITEM::iterator iter = kCont.begin();
	while(iter != kCont.end())
	{
		if((*iter).second.dwDeleteTime < dwCurTime)
		{
			iter = kCont.erase(iter);
			continue;
		}
		++iter;
	}
}
