#include "stdafx.h"
#include "ObjectPool.h"
#include "vstring.h"
#include "Header/DebugLog.h"

//BM::CObjectObserver::CONT_OBSERVERS BM::CObjectObserver::m_kContObserver;
//Loki::Mutex BM::CObjectObserver::m_kMutex;
#ifndef _MDo_
CObjectObserver g_kObjObserver;

CObjectObserver::~CObjectObserver()
{
	CONT_OBSERVERS::iterator obs_itor = m_kContObserver.begin();
	while (obs_itor != m_kContObserver.end())
	{
		(*obs_itor)->Registerd(false);

		++obs_itor;
	}
}

void CObjectObserver::RegistObserver(CPoolObserver * const pObj)
{
	//BM::CAutoMutex kLock(m_kMutex);
	if (pObj == NULL)
	{
		return;
	}
	m_kContObserver.insert(pObj);
	pObj->Registerd(true);
}

void CObjectObserver::UnregistObserver(CPoolObserver * const pObj)
{
	// pObj 의 destructor 에서 호출됨.
	//	1. pObj 의 상위개체에 대해 접근 하려고 하지 말것~
	//	2. pObj 의 virtual 함수 호출 하지 말것.
	//BM::CAutoMutex kLock(m_kMutex);

	if (pObj == NULL)
	{
		return;
	}

	m_kContObserver.erase(pObj);
	pObj->Registerd(false);
	CONT_OBJECTINFO::iterator obj_itor = m_kContObjectInfo.find(pObj->GetName());

	ObjectInfo kInfo(pObj->GetUsedCount(), pObj->GetMaxUsedCount(), 0);
	if (m_kContObjectInfo.end() != obj_itor)
	{
		obj_itor->second.Update(kInfo);
	}
	else
	{
		m_kContObjectInfo.insert(std::make_pair(pObj->GetName(), kInfo));
	}
}
	
void CObjectObserver::DisplayState( BM::PgDebugLog &rkLog )const
{
	BM::CAutoMutex kLock(m_kMutex);

//	kLog.LogNoArg(BM::LOG_LV0, (BM::vstring)_T("=== Observer state ====="));
	CONT_OBSERVERS::const_iterator ob_itor = m_kContObserver.begin();
	while(ob_itor != m_kContObserver.end())
	{
		(*ob_itor)->VDisplayState(rkLog);
		++ob_itor;
	}

//	kLog.LogNoArg(BM::LOG_LV1, (BM::vstring)_T("=== DeletedObserver state ====="));
	CONT_OBJECTINFO::const_iterator dead_itor = m_kContObjectInfo.begin();
	while(dead_itor != m_kContObjectInfo.end())
	{
//		kLog.LogNoArg(BM::LOG_LV1, (BM::vstring)_T("Type Name = ") << UNI(dead_itor->first));
//		kLog.LogNoArg(BM::LOG_LV1, (BM::vstring)_T("UnReleasedCount=") << dead_itor->second.iUnReleasedCount <<(BM::vstring)_T(", MaxUsed=,")<< dead_itor->second.iMaxUsedCount<<(BM::vstring)_T(" MaxReserved=") << dead_itor->second.iMaxReservedCount);

		++dead_itor;
	}

}

/////////////////////////////////////////////////////////////////
//	CPoolBase

CPoolObserver::CPoolObserver(std::string const &kName)
{
	m_kName = kName;
	m_nUsedCount = 0;
	m_nMaxUsedCount = 0;
	m_bRegistered = false;
}

CPoolObserver::~CPoolObserver()
{
	if (m_bRegistered)
	{
		g_kObjObserver.UnregistObserver(this);
	}
}

#endif /* _MDo_ */