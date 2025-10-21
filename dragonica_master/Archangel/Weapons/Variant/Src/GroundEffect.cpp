#include "stdafx.h"
#include "GroundEffect.h"

PgGroundEffectMgr::PgGroundEffectMgr()
{
}


PgGroundEffectMgr::~PgGroundEffectMgr()
{
	Clear();
}


bool PgGroundEffectMgr::Build( CONT_MAP_EFFECT const &tblMapEffect,  CONT_DEFMAP const &tblDefMap )
{
	bool bReturn = true;
	Clear();

	CONT_DEFMAP::const_iterator map_itr;
	for ( size_t i=0; i<tblMapEffect.size(); ++i )
	{
		CONT_MAP_EFFECT::value_type const &kEffect = tblMapEffect.at(i);

		map_itr = tblDefMap.find( kEffect.iGroundNo );
		if ( map_itr != tblDefMap.end() )
		{
			PgGroundEffect* pkNew = m_kGroundEffectPool.New();
			pkNew->EffectNo(kEffect.iEffectID);
			pkNew->GroundNo(kEffect.iGroundNo);
			for (int j=0; j<MAX_MAP_EFFECT_ABIL_ARRAY; j++)
			{
				if (kEffect.aiValue[j] != 0)
				{
					pkNew->SetAbil(kEffect.aiType[j], kEffect.aiValue[j]);
				}
			}

			for (int k=0; k<MAX_MAP_EFFECT_ABIL64_ARRAY; k++)
			{
				if (kEffect.aiValue[k] != 0)
				{
					pkNew->SetAbil(kEffect.aiType64[k], (int)(kEffect.aiValue64[k]) );
				}
			}

			CONT_GROUND_EFFECT::iterator itor = m_kGroundEffect.find(kEffect.iGroundNo);
			if (itor == m_kGroundEffect.end())
			{
				auto kRet = m_kGroundEffect.insert(std::make_pair(kEffect.iGroundNo, CONT_GROUND_EFFECT_ELEMENT()));
				if( !kRet.second )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"Cannot insert GroundNo["<<kEffect.iGroundNo<<L"]");
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
					bReturn = false;
					m_kGroundEffectPool.Delete( pkNew );
					continue;
				}
				itor = kRet.first;
			}
			CONT_GROUND_EFFECT_ELEMENT& rkElement = itor->second;
			rkElement.push_back(pkNew);
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("[TBL_DEF_MAP_EFFECT] Bad GroundNo<") << kEffect.iGroundNo << _T("> EffectNo<") << kEffect.iEffectID << _T(">") );
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Not Find Data"));
			bReturn = false;
		}
	}

	return bReturn;
}

void PgGroundEffectMgr::Clear()
{
	CONT_GROUND_EFFECT::iterator itor = m_kGroundEffect.begin();
	while (itor != m_kGroundEffect.end())
	{
		CONT_GROUND_EFFECT_ELEMENT& rkElement = itor->second;
		size_t iSize = rkElement.size();
		for (size_t i=0; i<iSize; i++)
		{
			m_kGroundEffectPool.Delete(rkElement.at(i));
		}
		rkElement.clear();

		++itor;
	}
	m_kGroundEffect.clear();
}

PgGroundEffect const * PgGroundEffectMgr::GetGroundEffect(int const iIndex, int const iGroundNo) const
{
	CONT_GROUND_EFFECT::const_iterator itor = m_kGroundEffect.find(iGroundNo);
	if (itor == m_kGroundEffect.end())
	{
		return NULL;
	}
	CONT_GROUND_EFFECT_ELEMENT const &rkElement = itor->second;
	size_t iSize = rkElement.size();
	if (iIndex < 0 || (int)iSize <= iIndex)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
		return NULL;
	}
	return rkElement.at(iIndex);
}