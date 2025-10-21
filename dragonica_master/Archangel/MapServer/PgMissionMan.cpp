#include "stdafx.h"
#include "variant/constant.h"
#include "Variant/PgMissionInfo.h"
#include "Variant/PgMission.h"
#include "constant.h"
#include "PgMissionMan.h"
#include "Global.h"
#include "Variant/def.h"
#include "Variant/MonsterDef.h"
#include "Variant/MonsterDefMgr.h"

typedef BM::TObjectPool< PgMissionInfo > PoolMission;
PoolMission g_kMissionPool(100, 100);

//---------------------------------------
// PgMissionManager 
//---------------------------------------

PgMissionManager::PgMissionManager()
{	
}

PgMissionManager::~PgMissionManager()
{
	Clear();
}

HRESULT PgMissionManager::CreateInfo()
{
	BM::CAutoMutex kLock(m_kMutex);

	VEC_WSTRING kVec;
	const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;
	g_kTblDataMgr.GetContDef(pContDefMission);

	CONT_DEF_MISSION_ROOT::const_iterator mission_itor = pContDefMission->begin();
	while(mission_itor != pContDefMission->end())
	{
		const TBL_DEF_MISSION_ROOT* pMission = &(*mission_itor).second;

		int iMission = pMission->iMissionNo;
		std::wstring wstrName;

		wchar_t szPath[200] = {0,};
		::swprintf_s(szPath, 200, L"Mission%08d.xml", iMission);
		wstrName = szPath;

		kVec.push_back(wstrName);

		auto ret = m_kMissionKeyMap.insert(std::make_pair(pMission->iKey, iMission));
		if( !ret.second )
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<L"Mission Add failure duplicated MissionKey["<<pMission->iKey<<L"] from MissionID["<<iMission<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		++mission_itor;
	}


	//Add Chapter
	VEC_WSTRING::iterator iter = kVec.begin();
	while(kVec.end() != iter)
	{
		std::wstring &rkTemp = (*iter);
//		INFO_LOG(BM::LOG_LV0, _T("[%s]-[%d] Create Mission From 'Script/Mission/%s'"), __FUNCTIONW__, __LINE__, rkTemp.c_str());
		if( !rkTemp.size() )
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<L"Error!!: file name length 0 at Mission");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALIDARG"));
			return E_INVALIDARG;
		}

		if( !Add(rkTemp.c_str()) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}

		++iter;
	}

	return S_OK;
}

bool PgMissionManager::Create()
{
	HRESULT hRet = S_OK;

	hRet = CreateInfo();
	if( FAILED(hRet) )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"failed to pre-loading MissionOptionChapter Return["<<hRet<<L"]");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CreateInfo Failed!"));
		return false;
	}
	return true;
}

void PgMissionManager::Reload()
{
}

void PgMissionManager::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);

	MissionContainer::iterator quest_iter = m_kMissionMap.begin();
	while(m_kMissionMap.end() != quest_iter)
	{
		MissionContainer::mapped_type pkElement = (*quest_iter).second;
		if( pkElement )
		{
			pkElement->Clear();
			g_kMissionPool.Delete( pkElement );
		}
		++quest_iter;
	}
	m_kMissionMap.clear();
	m_kMissionKeyMap.clear();
}

bool PgMissionManager::Add(TCHAR const* pkXmlPath)
{
	BM::CAutoMutex kLock(m_kMutex);

	PgMissionInfo* pkMission = g_kMissionPool.New();
	pkMission->Clear();

	char szPath[_MAX_PATH] = {0, };
	sprintf_s(szPath, _MAX_PATH, "XML/Mission/%s", MB(pkXmlPath));

	TiXmlDocument kDocu;
	if( !kDocu.LoadFile(szPath) )
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Failed parse xml ["<<UNI(szPath)<<L"]");
		g_kMissionPool.Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	TiXmlElement *pkChild = kDocu.FirstChildElement("MISSION");
	bool bRet = pkMission->ParseXml(pkChild);
	if(!bRet)
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Failed parse xml: Can't find <MISSION> element");
		g_kMissionPool.Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}


	auto ret = m_kMissionMap.insert(std::make_pair(pkMission->ID(), pkMission));
	if( !ret.second )
	{
		INFO_LOG(BM::LOG_LV6, __FL__<<L"Mission Add failure duplicated MissionID["<<pkMission->ID()<<L"] from XML["<<pkXmlPath<<L"]");
		g_kMissionPool.Delete(pkMission);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const CONT_DEF_MISSION_ROOT* pContDefMission = NULL;
	g_kTblDataMgr.GetContDef(pContDefMission);
	const CONT_DEF_MISSION_CANDIDATE* pContDefMissionCandi = NULL;
	g_kTblDataMgr.GetContDef(pContDefMissionCandi);

	CONT_DEF_MISSION_ROOT::const_iterator mission_itor = pContDefMission->find(pkMission->ID());
	if(mission_itor != pContDefMission->end())
	{
		for(int i=0; i!=MAX_MISSION_LEVEL; ++i)
		{
			CONT_DEF_MISSION_CANDIDATE::const_iterator candi_itr = pContDefMissionCandi->find(mission_itor->second.aiLevel[i]);
			if ( candi_itr!=pContDefMissionCandi->end() )
			{
				int iGroundNo = 0;
				for(int j=0; j!=MAX_MISSION_CANDIDATE; ++j)
				{
					iGroundNo = candi_itr->second.aiCandidate[j];
					if ( iGroundNo )
					{
						bool const bRet = pkMission->GetMissionChapterListCheck( iGroundNo );
						if( !bRet )
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" Ground XML Can't Setting failed to pre-loading MissionOptionChapter Return false, Ground : [") << iGroundNo << _T("]"));
							LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("bRet is false"));
						}
					}
				}
			}
		}
	}

	return true;//성공
}

bool PgMissionManager::GetMission(int const iMissionID, PgMissionInfo const *& pkOut)const
{
	//BM::CAutoMutex kLock(m_kMutex);
	MissionContainer::const_iterator iter = m_kMissionMap.find(iMissionID);
	if(m_kMissionMap.end() != iter)
	{
		pkOut = (*iter).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionManager::GetMissionKey(int const iMissionKey, PgMissionInfo const *& pkOut)const
{
	MissionKeyContainer::const_iterator iter = m_kMissionKeyMap.find(iMissionKey);
	if(m_kMissionKeyMap.end() != iter)
	{
		return GetMission( (*iter).second, pkOut );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMissionManager::GetMissionContainer(const MissionContainer*& pkOut) const
{
	//BM::CAutoMutex kLock(m_kMutex);
	pkOut = &m_kMissionMap;
	return true;
}
