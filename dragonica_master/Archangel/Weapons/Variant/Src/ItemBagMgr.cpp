#include "stdafx.h"
#include "itembagmgr.h"
#include "Lohengrin/LogGroup.h"


// CItemBagMgr
CItemBagMgr::CItemBagMgr(void)
//	:m_mapItemBagPool(50)
{
}

CItemBagMgr::~CItemBagMgr(void)
{
	Clear();
}

void CItemBagMgr::Clear()
{
	m_kItemBag.clear();
	m_kContDefItemBag.clear();
	m_kContDefBagElements.clear();
	m_kContDefCountControl.clear();
	m_kContDefRaiseControl.clear();
	m_kContDefItemBagGroup.clear();
	m_kMoneyControl.clear();
	m_kContDefItemContainer.clear();
}

void CItemBagMgr::swap(CItemBagMgr &rhs)
{
	m_kItemBag.swap(rhs.m_kItemBag);
	m_kContDefItemBag.swap( rhs.m_kContDefItemBag );
	m_kContDefBagElements.swap( rhs.m_kContDefBagElements );
	m_kContDefRaiseControl.swap( rhs.m_kContDefRaiseControl );
	m_kContDefCountControl.swap( rhs.m_kContDefCountControl );
	m_kMoneyControl.swap( rhs.m_kMoneyControl );
	m_kContDefItemBagGroup.swap(rhs.m_kContDefItemBagGroup);
	m_kContDefItemContainer.swap(rhs.m_kContDefItemContainer);
}

bool CItemBagMgr::Build(
		CONT_DEF_ITEM_BAG_GROUP const &rkItemBagGroup,
		CONT_DEF_ITEM_BAG const &rkDefItemBag,
		CONT_DEF_ITEM_BAG_ELEMENTS const &rkBagElements,
		CONT_DEF_SUCCESS_RATE_CONTROL const &rkRaiseControl,
		CONT_DEF_COUNT_CONTROL const &rkCountControl,
		CONT_DEF_DROP_MONEY_CONTROL const &rkMoneyControl,
		CONT_DEF_ITEM_CONTAINER const &rkContItemContainer
		)
{
	bool bReturn = true;
	Clear();

	m_kContDefItemBagGroup = rkItemBagGroup;
	m_kContDefItemBag = rkDefItemBag;
	m_kContDefBagElements = rkBagElements;
	m_kContDefRaiseControl = rkRaiseControl;
	m_kContDefCountControl = rkCountControl;
	m_kContDefItemContainer = rkContItemContainer;

	if( rkContItemContainer.empty() )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ItemContianer is Empty");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	CONT_DEF_ITEM_CONTAINER::const_iterator container_iter = rkContItemContainer.begin();
	while(container_iter != rkContItemContainer.end())
	{
		CONT_DEF_ITEM_CONTAINER::mapped_type const& rkElement = (*container_iter).second;
		int const iSuccess_Rate = rkElement.iSuccessRateControlNo;

		CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator success_rate_iter = rkRaiseControl.find(iSuccess_Rate);
		if(success_rate_iter == rkRaiseControl.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"SuccessRateControlNo is Not Find. RaseRateNo["<<iSuccess_Rate<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int i = 0;
		while(i<MAX_ITEM_CONTAINER_LIST)
		{
			int const iBagGroupNo = rkElement.aiItemBagGrpNo[i];
			if( 0 != iBagGroupNo
				&&	rkItemBagGroup.end() == rkItemBagGroup.find(iBagGroupNo) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ItemBagGroupNo is Not Find. BagGroupNo["<<iBagGroupNo<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
			++i;
		}

		++container_iter;
	}
	
	CONT_DEF_ITEM_BAG::iterator bag_itor = m_kContDefItemBag.begin();
	while(bag_itor != m_kContDefItemBag.end())
	{
		int const iElementsNo = (*bag_itor).second.iElementsNo;
		int const iRaseRateNo = (*bag_itor).second.iRaseRateNo;
		int const iCountControlNo = (*bag_itor).second.iCountControlNo;
		int const iMoneyControlNo = (*bag_itor).second.iDropMoneyControlNo;
		
		CONT_DEF_ITEM_BAG_ELEMENTS::const_iterator bag_element_itor = rkBagElements.find(iElementsNo);
		CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator success_rate_itor = rkRaiseControl.find(iRaseRateNo);
		CONT_DEF_COUNT_CONTROL::const_iterator count_itor = rkCountControl.find(iCountControlNo);
		CONT_DEF_DROP_MONEY_CONTROL::const_iterator money_itor = rkMoneyControl.find(iMoneyControlNo);
		
		if(bag_element_itor == rkBagElements.end() 
		|| success_rate_itor == rkRaiseControl.end() 
		|| count_itor == rkCountControl.end() 
		||	money_itor == rkMoneyControl.end())
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ItemBag Build Failed ElementsNo["<<iElementsNo<<L"], RaseRateNo["<<iRaseRateNo<<L"], CountControlNo["<<iCountControlNo<<L"] MoneyControlNo["<<iMoneyControlNo<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		PgItemBagByLevel kItemBag( bag_itor->first.second );
		if(!kItemBag.Build( bag_element_itor->second, success_rate_itor->second, count_itor->second, money_itor->second))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"ItemBag Build Failed");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int const iBagNo = bag_itor->first.first;
		CONT_ITEM_BAG::iterator item_bag_itr = m_kItemBag.find( iBagNo );
		if ( item_bag_itr == m_kItemBag.end() )
		{
			auto kPair = m_kItemBag.insert( std::make_pair( iBagNo, CONT_ITEM_BAG::mapped_type()) );
			item_bag_itr = kPair.first;
		}

		item_bag_itr->second.insert( kItemBag );
		++bag_itor;
	}
	return bReturn;
}

HRESULT CItemBagMgr::GetItemBagByGrp(int const iBagGroupNo, short nLevel, PgItemBag &rkItemBag)const
{
	CONT_DEF_ITEM_BAG_GROUP::const_iterator itor = m_kContDefItemBagGroup.find( iBagGroupNo );
	if(itor != m_kContDefItemBagGroup.end())
	{
		int const iSuccessRateNo = (*itor).second.iSuccessRateNo;

		size_t iIDX = 0;
		if(RouletteRate(iSuccessRateNo, iIDX, MAX_ITEM_BAG_ELEMENT))
		{
			int const iBagNo = (*itor).second.aiBagNo[iIDX];
			return GetItemBag(iBagNo, nLevel, rkItemBag);
		}	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::GetItemBagNoByGrp(int const iBagGroupNo, short nLevel, int& riOutItemBagNo)const
{
	CONT_DEF_ITEM_BAG_GROUP::const_iterator itor = m_kContDefItemBagGroup.find( iBagGroupNo );
	if(itor != m_kContDefItemBagGroup.end())
	{
		int const iSuccessRateNo = (*itor).second.iSuccessRateNo;

		size_t iIDX = 0;
		if(RouletteRate(iSuccessRateNo, iIDX, MAX_ITEM_BAG_ELEMENT))
		{
			CONT_DEF_ITEM_BAG_GROUP::mapped_type const& rkDefItemBagGroup = (*itor).second;
			if(iIDX <= MAX_ITEM_BAG_ELEMENT)
			{
				riOutItemBagNo = rkDefItemBagGroup.aiBagNo[iIDX];
				return S_OK;
			}
		}	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::GetItemBag( int const iBagNo, short nLevel, PgItemBag &rkItemBag )const
{
	CONT_ITEM_BAG::const_iterator itor = m_kItemBag.find( iBagNo );
	if(itor != m_kItemBag.end())
	{
		T_BAG_ELEMENT const &kElement = itor->second;
		size_t const index = kElement.find( PgBagByLevelFinder(nLevel) );
		if ( BM::PgApproximate::npos != index )
		{
			rkItemBag = kElement.at( index );
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::GetItemBagElements(int const iBagNo, TBL_DEF_ITEM_BAG_ELEMENTS &rkItemBagElements)const
{
	CONT_DEF_ITEM_BAG_ELEMENTS::const_iterator itor = m_kContDefBagElements.find( iBagNo );
	if(itor != m_kContDefBagElements.end())
	{
		rkItemBagElements = (*itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::GetSuccesRateControl(int const iNo, TBL_DEF_SUCCESS_RATE_CONTROL &rkSuccesRateControl)const
{
	CONT_DEF_SUCCESS_RATE_CONTROL::const_iterator itor = m_kContDefRaiseControl.find( iNo );
	if(itor != m_kContDefRaiseControl.end())
	{
		rkSuccesRateControl = (*itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::GetCountControl(int const iNo, TBL_DEF_COUNT_CONTROL &rkCountControl)const
{
	CONT_DEF_COUNT_CONTROL::const_iterator itor = m_kContDefCountControl.find( iNo );
	if(itor != m_kContDefCountControl.end())
	{
		rkCountControl = (*itor).second;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT CItemBagMgr::EnumerateItemBag(int const iBagNo,CONT_SET_DATA & kCont)const
{
	CONT_ITEM_BAG::const_iterator itr = m_kItemBag.find( iBagNo );
	if( itr != m_kItemBag.end() )
	{
		T_BAG_ELEMENT const &kBagElement = itr->second;
		T_BAG_ELEMENT::const_iterator itr = kBagElement.begin();
		for ( ; itr < kBagElement.end() ; ++itr )
		{
			PgItemBag::BagElementCont const & kElement = itr->GetElements();
			for( PgItemBag::BagElementCont::const_iterator iter = kElement.begin();iter != kElement.end();++iter )
			{
				int const & iItemNo = (*iter).iItemNo;
				if( iItemNo )
				{
					kCont.insert(iItemNo);
				}
			}
		}

		return S_OK;
	}
		
	return E_FAIL;
}

HRESULT CItemBagMgr::EnumerateItemBagGroup(int const iBagGroupNo,CONT_SET_DATA & kCont)const
{
	HRESULT bRet = S_OK;
	CONT_DEF_ITEM_BAG_GROUP::const_iterator iter = m_kContDefItemBagGroup.find( iBagGroupNo );
	if(iter == m_kContDefItemBagGroup.end())
	{
		return E_FAIL;
	}

	for(int i = 0;i < MAX_ITEM_BAG_ELEMENT;++i)
	{
		int const iBagNo = (*iter).second.aiBagNo[i];

		if(0 == iBagNo)
		{
			continue;
		}

		if(S_OK != EnumerateItemBag(iBagNo,kCont))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't Find BagNo["<<iBagNo<<L"] from ItemBagGruop["<<iBagGroupNo<<"]" );
			bRet = E_FAIL;
			continue;
		}
	}

	return bRet;
}

HRESULT CItemBagMgr::EnumerateItemContainer(int const iContainerNo,CONT_SET_DATA & kCont)const
{
	HRESULT bRet = S_OK;

	CONT_DEF_ITEM_CONTAINER::const_iterator iter = m_kContDefItemContainer.find( iContainerNo );
	if(iter == m_kContDefItemContainer.end())
	{
		return E_FAIL;
	}

	for(int i = 0;i < MAX_ITEM_CONTAINER_LIST;++i)
	{
		int const iBagGroupNo = (*iter).second.aiItemBagGrpNo[i];

		if(0 == iBagGroupNo)
		{
			continue;
		}

		if(S_OK != EnumerateItemBagGroup(iBagGroupNo,kCont))
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Can't EnumerateItemBagGroup BagGroupNo["<<iBagGroupNo<<L"] from ItemContainer["<<iContainerNo<<"]" );
			bRet = E_FAIL;
			continue;
		}
	}

	return bRet;
}