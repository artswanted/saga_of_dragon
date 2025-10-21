#include "StdAfx.h"
#include <set>
#include "PgMonsterBag.h"

PgMonsterBag::PgMonsterBag(void)
{
}

PgMonsterBag::~PgMonsterBag(void)
{
}

bool PgMonsterBag::Build(	
	const CONT_DEF_MAP_REGEN_POINT *pkMapRegenPoint,
	const CONT_DEF_MONSTER_BAG_CONTROL *pkMonsterBagControl,
	const CONT_DEF_MONSTER_BAG *pkMonsterBag,
	const CONT_DEF_MONSTER_BAG_ELEMENTS *pkMonsterBagElements )
{
	bool bReturn = true;
	m_kMapRegenPoint = *pkMapRegenPoint;
	m_kMonsterBagControl = *pkMonsterBagControl;
	m_kMonsterBag = *pkMonsterBag;
	m_kMonsterBagElements = *pkMonsterBagElements;

	if(!m_kMapRegenPoint.size()
	&& !m_kMonsterBagControl.size()
	&& !m_kMonsterBag.size()
	&& !m_kMonsterBagElements.size())
	{
		assert(NULL);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// DefMapMonsterRegen에는 중복된 MapNo가 많다. 그것을 하나의 Hash로 모아주자.
	CONT_HASH_MON_SINGLE_DATA kTopRegenData;
	CONT_DEF_MAP_REGEN_POINT::iterator regen_itr = m_kMapRegenPoint.begin();
	while(regen_itr != m_kMapRegenPoint.end())
	{
		TBL_DEF_MAP_REGEN_POINT& rkRegen = (*regen_itr).second;

		CONT_HASH_MON_SINGLE_DATA::iterator regen_data_itr = kTopRegenData.find(rkRegen.iMapNo);
		if (regen_data_itr != kTopRegenData.end())
		{
			CONT_SET_DATA& rkSetData = (*regen_data_itr).second;
			rkSetData.insert(rkRegen.iBagControlNo);
		}
		else
		{
			CONT_SET_DATA kSetData;
			kSetData.insert(rkRegen.iBagControlNo);
			kTopRegenData.insert(std::make_pair(rkRegen.iMapNo, kSetData));
		}

		++regen_itr;
	}


	// 맵과 대칭 되는 몬스터 번호를 찾아오자.
	CONT_HASH_MON_SINGLE_DATA::const_iterator regen_data_itr = kTopRegenData.begin();
	while(regen_data_itr != kTopRegenData.end())
	{
		const CONT_SET_DATA& rkRegenData = (*regen_data_itr).second;
		SMonsterControl kMonControl;

		// 정리된 Regen의 Control들.
		CONT_SET_DATA::const_iterator set_itr = rkRegenData.begin();
		while(set_itr != rkRegenData.end())
		{
			int const &riControlNo = (*set_itr);

			// 해당 Control이 가진 Bag.
			CONT_DEF_MONSTER_BAG_CONTROL::iterator control_itr = m_kMonsterBagControl.find(riControlNo);			
			if (control_itr != m_kMonsterBagControl.end())
			{
				TBL_DEF_MONSTER_BAG_CONTROL& rkControl = (*control_itr).second;

				// 해당 Control이 가진 Bag 들을 체크 하기 위해 루프.
				for (int i=0 ; i<MAX_MONSTERBAG_ELEMENT ; i++)
				{
					// Control이 가진 Bag.
					// rkControl.aBagElement[i] 의 중복 체크. 해야하나?
					CONT_DEF_MONSTER_BAG::iterator bag_itr = m_kMonsterBag.find(rkControl.aBagElement[i]);
					if (bag_itr != m_kMonsterBag.end())
					{
						TBL_DEF_MONSTER_BAG& rkBag = (*bag_itr).second;

						// Bag이 가진 Element
						CONT_DEF_MONSTER_BAG_ELEMENTS::iterator element_itr = m_kMonsterBagElements.find(rkBag.iElementNo);
						if (element_itr != m_kMonsterBagElements.end())
						{
							TBL_DEF_MONSTER_BAG_ELEMENTS& rkElement = (*element_itr).second;

							// Element가 가진 몬스터 번호들.
							for (int j=0 ; j<MAX_SUCCESS_RATE_ARRAY ; j++)
							{
								int iMonNo = rkElement.aElement[j];	// 몬스터 번호
								if(0 == iMonNo)
								{
									continue;
								}
								kMonControl.kSetMonster[i].insert(iMonNo);
							}
						}
					}
				}
			}
			++set_itr;
		}

		int const &riMapNo = (*regen_data_itr).first;
		m_kRealMonData.insert(std::make_pair(riMapNo, kMonControl));

		++regen_data_itr;
	}

	return bReturn;
}

bool PgMonsterBag::GetMonster(int iMapNo, SMonsterControl& rkMonsterData) const
{
	CONT_HASH_MON_MULTI_DATA::const_iterator find_itr = m_kRealMonData.find(iMapNo);
	if (find_itr != m_kRealMonData.end())
	{
		rkMonsterData = (*find_itr).second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgMonsterBag::GetMonsterBag(int const iMonBagNo,int & iMonNo) const
{
	CONT_DEF_MONSTER_BAG::const_iterator iter = m_kMonsterBag.find(iMonBagNo);

	if(iter == m_kMonsterBag.end())
	{
		return false;
	}

	CONT_DEF_MONSTER_BAG_ELEMENTS::const_iterator moniter = m_kMonsterBagElements.find((*iter).second.iElementNo);

	if(moniter == m_kMonsterBagElements.begin())
	{
		return false;
	}

	size_t kIdx = 0;
	if(false == RouletteRate((*iter).second.iRateNo,kIdx,MAX_SUCCESS_RATE_ARRAY))
	{
		return false;
	}

	iMonNo = (*moniter).second.aElement[kIdx];

	return true;
}
