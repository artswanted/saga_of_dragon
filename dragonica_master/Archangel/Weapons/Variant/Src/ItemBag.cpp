#include "stdafx.h"
#include "ItemBag.h"
//#include "../ItemBagMgr.h"
#include "Lohengrin/LogGroup.h"
#include "Variant/tabledatamanager.h"
#include "PgControlDefMgr.h"

int PgItemBag::tagBagElement::GetItemNo( int const iLevel )const
{
	if ( true == bIsLevelBalance )
	{
		CONT_DEF_ITEM_BY_LEVEL const *pDefItemByLevel = NULL;
		g_kTblDataMgr.GetContDef( pDefItemByLevel );

		CONT_DEF_ITEM_BY_LEVEL::const_iterator itr = pDefItemByLevel->find( iItemNo );
		if ( itr != pDefItemByLevel->end() )
		{
			int iValue = 0;
			if ( true == itr->second.Get( iLevel, iValue ) )
			{
				return iValue;
			}

			VERIFY_INFO_LOG( false, BM::LOG_LV4, L"Not Found CONT_DEF_ITEM_BY_LEVEL ID<" << iItemNo << L">'s Level<" << iLevel << L">" );
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV4, L"Not Found CONT_DEF_ITEM_BY_LEVEL ID<" << iItemNo << L">" );
		}

		return 0;
	}

	return iItemNo;
}

PgItemBag::PgItemBag()
:	m_kTotalRaiseRate(0)
,	m_kMoneyMin(0)
,	m_kMoneyRand(0)
,	m_kMoneyRate(0)
{
}

PgItemBag::~PgItemBag()
{
}

// void PgItemBag::Clear()
// {
// 	TotalRaiseRate( 0 );
// 	MoneyMin(0);
// 	MoneyRand(0);
// 	MoneyRate(0);
// }

bool PgItemBag::Build(	CONT_DEF_ITEM_BAG_ELEMENTS::mapped_type const &rkItemBag, 
						CONT_DEF_SUCCESS_RATE_CONTROL::mapped_type const &rkRaise, 
						CONT_DEF_COUNT_CONTROL::mapped_type const &rkCount,
						CONT_DEF_DROP_MONEY_CONTROL::mapped_type const &rkMoneyDrop)
{	
	int i = 0;
	while(i != MAX_SUCCESS_RATE_ARRAY)
	{//	while - 한개의 아이템 백에 있는 모든 아이템정보를 검색
		BagElementCont::value_type Element( rkItemBag.aElement[i], rkCount.aCount[i], rkRaise.aRate[i], (rkItemBag.nTypeFlag & ( 1 << i ))  );
		
		if(!Element.IsEmpty())
		{
			m_kContBag.push_back(Element);
		}
		++i;
	}
	AccTotalRaiseValue();


	MoneyRate(rkMoneyDrop.iRate);
	MoneyMin(rkMoneyDrop.iMin);
	
	int iResultMoney = rkMoneyDrop.iMax - rkMoneyDrop.iMin;
	iResultMoney = __max(iResultMoney,0);//0보다 작을 수는 없고.

	MoneyRand(iResultMoney);

	return true;
}

HRESULT PgItemBag::PopItem( int const iLevel, int &rOutResultItemNo, int &rOutCount )
{
	if( !TotalRaiseRate() ){return E_FAIL;}

	int const iResult = BM::Rand_Index(TotalRaiseRate()); 
	
	int iAcc = 0;
	int iItemNo = 0;
	BagElementCont::iterator itor = m_kContBag.begin();
//Random 돌려서 나온 값을. 
	while( m_kContBag.end() != itor ) // first key : ItemNo
	{
		BagElementCont::value_type &rElement = (*itor);
		
		if( rElement.nCount > 0)//갯수제한 아이템이 아니거나
		{
			iAcc += rElement.nRate;
		}
	
		if( iResult <= iAcc )
		{
			rOutResultItemNo = rElement.GetItemNo( iLevel );
			rOutCount = rElement.nCount;
			break;
		}
		++itor;
	}

	AccTotalRaiseValue(); // 해당 ItemBag 의 RaiseRate 재계산
	if( rOutResultItemNo
	&&	rOutCount )
	{
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NO_ITEM"));
	return E_NO_ITEM;//아이템이 없음
}

HRESULT PgItemBag::PopItem( int const iLevel, int &rOutResultItemNo )
{
	if( !TotalRaiseRate() ){return E_FAIL;}

	int const iResult = BM::Rand_Index(TotalRaiseRate()); 
	
	int iAcc = 0;
	int iItemNo = 0;
	BagElementCont::iterator itor = m_kContBag.begin();
//Random 돌려서 나온 값을. 
	while( m_kContBag.end() != itor ) // first key : ItemNo
	{
		BagElementCont::value_type &rElement = (*itor);
		
		if( rElement.nCount > 0)//갯수제한 아이템이 아니거나
		{
			iAcc += rElement.nRate;
		}
	
		if( iResult <= iAcc )
		{
			if( rElement.nCount )
			{//갯수 아이템이면 갯수 까고.
				--rElement.nCount;
			}
			rOutResultItemNo = rElement.GetItemNo( iLevel );
			break;
		}
		++itor;
	}

	AccTotalRaiseValue(); // 해당 ItemBag 의 RaiseRate 재계산
	if(rOutResultItemNo)
	{
		return S_OK;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NO_ITEM"));
	return E_NO_ITEM;//아이템이 없음
}

HRESULT PgItemBag::PopItemToIndex( int const iLevel, int &rOutResultItemNo, size_t& rOutResultItemCount, int &iIndex )
{//! ItemBag에서 iIndex의 아이템을 뽑는다.
	if( !TotalRaiseRate() || 0 > iIndex || static_cast<int>(m_kContBag.size()) <= iIndex )
	{
		return E_FAIL;
	}

	int iAcc = 0;
	int iItemNo = 0;
	BagElementCont::value_type &rElement = m_kContBag[iIndex];

	if( rElement.nCount > 0)//갯수제한 아이템이 아니거나
	{
		iAcc += rElement.nRate;
	}

	if( iIndex <= iAcc )
	{
		rOutResultItemNo = rElement.GetItemNo(iLevel);
		rOutResultItemCount = rElement.nCount;
		return S_OK;
	}

	AccTotalRaiseValue(); // 해당 ItemBag 의 RaiseRate 재계산
	if(rOutResultItemNo)
	{
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NO_ITEM"));
	return E_NO_ITEM;//아이템이 없음
}

HRESULT PgItemBag::PopMoney(int &rOutResultMoney, int const iAddDropRate)
{
	int const iResult = (int) BM::Rand_Index(100); 

	int iDropRate = MoneyRate();
	if(iAddDropRate)
	{
		iDropRate = ((iDropRate*iAddDropRate)/100);//이벤트 보너스.
	}

	if(iDropRate > iResult)
	{
		int iResult = MoneyMin();
		if(MoneyRand())
		{
			iResult += BM::Rand_Range(MoneyRand());
		}
		
		if(iResult)
		{
			rOutResultMoney = iResult;
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgItemBag::AccTotalRaiseValue()
{
	BagElementCont::const_iterator itor = m_kContBag.begin();
	
	int iAcc = 0;
	while(m_kContBag.end() != itor )
	{//전체 확률 갱신을 위한 while문
		const BagElementCont::value_type &rElement = (*itor);

		if(rElement.nCount
		&& rElement.nRate)//갯수가 있거나 //리미티드거나?...
		{
			iAcc += rElement.nRate;
		}
		++itor;
	}

	TotalRaiseRate(iAcc);	// 하나의 ItemBag 에서 RaiseRate 의 총합

	if(TotalRaiseRate() > SHRT_MAX)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Overflow RaiseRate");
	}
}

void PgItemBag::AddDropRate(int const iAddDropRate)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);

	for(BagElementCont::iterator itor = m_kContBag.begin(); m_kContBag.end() != itor; ++itor)
	{
		BagElementCont::value_type & rElement = (*itor);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(rElement.iItemNo);
		if(NULL == pItemDef)
		{
			continue;
		}

		if((false == pItemDef->IsType(ITEM_TYPE_EQUIP)) && (false == pItemDef->IsType(ITEM_TYPE_ETC)))
		{
			continue;
		}

		rElement.nRate *= iAddDropRate/100;
	}

	AccTotalRaiseValue();
}

PgItemBagByLevel::PgItemBagByLevel()
:	m_nLevel(0)
{}

PgItemBagByLevel::PgItemBagByLevel( short const nLevel )
:	m_nLevel(nLevel)
{}

PgItemBagByLevel::~PgItemBagByLevel()
{}


bool PgItemBagByLevel::operator < ( PgItemBagByLevel const & rhs )const
{
	return ( m_nLevel < rhs.m_nLevel );
}

bool PgItemBagByLevel::operator > ( PgItemBagByLevel const & rhs )const
{
	return ( rhs < *this );
}

bool PgItemBagByLevel::operator <= ( PgItemBagByLevel const & rhs )const
{
	return ( (*this < rhs) || (*this == rhs) ); 
}

bool PgItemBagByLevel::operator >= ( PgItemBagByLevel const & rhs )const
{
	return ( (rhs < *this) || (*this == rhs) );
}

bool PgItemBagByLevel::operator == ( PgItemBagByLevel const & rhs )const
{
	return m_nLevel == rhs.m_nLevel;
}
