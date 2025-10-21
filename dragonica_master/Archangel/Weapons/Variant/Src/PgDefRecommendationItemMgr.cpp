#include "stdafx.h"
#include "Lohengrin/dbtables.h"
#include "PgDefRecommendationItemMgr.h"

bool PgDefRecommendationItemMgr::GetRecommendationItemByItemNo(int const charclass,int const charlevel,int const iItemNo,CONT_ITEMNO & rkRecommendationItem) const
{
	CONT_CLASS_RECOMMENDATION_ITEM::const_iterator iter = m_kContRecommendatinItem.find(charclass);
	if(iter == m_kContRecommendatinItem.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	std::vector<CONT_DEF_RECOMMENDATION_ITEM::const_iterator>::const_iterator citer = (*iter).second.begin();

	for(;citer != (*iter).second.end();++citer)
	{
		CONT_DEF_RECOMMENDATION_ITEM::const_iterator itemiter = (*citer);

		//if(charlevel < (*itemiter).second.iLvMin || (*itemiter).second.iLvMax < charlevel)
		//{
		//	continue;
		//}

		CONT_ITEMNO const& rkContItemNo = (*(*citer)).second.kContItem;
		if( rkContItemNo.end() != std::find(rkContItemNo.begin(), rkContItemNo.end(), iItemNo) )
		{
			rkRecommendationItem = (*(*citer)).second.kContItem;
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDefRecommendationItemMgr::GetRecommendationItem(int const charclass,int const charlevel,CONT_ITEMNO & rkRecommendationItem) const
{
	CONT_CLASS_RECOMMENDATION_ITEM::const_iterator iter = m_kContRecommendatinItem.find(charclass);
	if(iter == m_kContRecommendatinItem.end())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	
	std::vector<CONT_DEF_RECOMMENDATION_ITEM::const_iterator>::const_iterator citer = (*iter).second.begin();

	for(;citer != (*iter).second.end();++citer)
	{
		CONT_DEF_RECOMMENDATION_ITEM::const_iterator itemiter = (*citer);

		if(charlevel < (*itemiter).second.iLvMin || (*itemiter).second.iLvMax < charlevel)
		{
			continue;
		}

		rkRecommendationItem = (*(*citer)).second.kContItem;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgDefRecommendationItemMgr::Build(CONT_DEF_RECOMMENDATION_ITEM const & rkContRecommendationItem)
{
	for(CONT_DEF_RECOMMENDATION_ITEM::const_iterator iter = rkContRecommendationItem.begin();iter != rkContRecommendationItem.end();++iter)
	{
		m_kContRecommendatinItem[(*iter).second.iClass].push_back(iter);
	}
	return true;
}
