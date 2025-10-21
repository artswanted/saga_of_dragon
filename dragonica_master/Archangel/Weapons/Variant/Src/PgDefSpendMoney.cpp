#include "stdafx.h"
#include "PgDefSpendMoney.h"

PgDefSpendMoneyMgr::PgDefSpendMoneyMgr()
{

}

PgDefSpendMoneyMgr::~PgDefSpendMoneyMgr()
{

}

bool PgDefSpendMoneyMgr::Build(const CONT_DEF_SPEND_MONEY* pkDef)
{
	BM::CAutoMutex kLock(m_kMutex);

	bool bReturn = true;
	if( NULL == pkDef )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV3, __FL__<<L"CONT_DEF_SPEND_MONEY Is Null");
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	Clear();

	CONT_DEF_SPEND_MONEY::const_iterator def_itr;
	for( def_itr=pkDef->begin(); def_itr!=pkDef->end(); ++def_itr )
	{
		const CONT_DEF_SPEND_MONEY::mapped_type& kElement = def_itr->second;
		ContSpendMoney::key_type kKey((ESpendMoneyType)kElement.iType,kElement.iLevelMin);
		while( kKey.iUserLevel <= kElement.iLevelMax )
		{
			auto kPair = m_kContSpendMoney.insert(std::make_pair(kKey,kElement.iSpendMoneyValue));
			if( !kPair.second )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__<<L"Overapping Data Type["<<kKey.eType<<L"],Level["<<kKey.iUserLevel<<L"],MoneyValue1["<<kPair.first->second<<L"],MoneyValue2["<<kElement.iSpendMoneyValue<<L"]");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Insert Failed Data"));
				bReturn = false;
			}
			++kKey.iUserLevel;
		}
	}
	return bReturn;
}

void PgDefSpendMoneyMgr::Swap(PgDefSpendMoneyMgr& rkRight)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kContSpendMoney.swap(rkRight.m_kContSpendMoney);
}

void PgDefSpendMoneyMgr::Clear()
{
	BM::CAutoMutex kLock(m_kMutex);
	m_kContSpendMoney.clear();
}

int PgDefSpendMoneyMgr::GetSpendMoney(const ESpendMoneyType _Type, int const _iUserLevel) const
{
	BM::CAutoMutex kLock(m_kMutex);

	ContSpendMoney::key_type kKey(_Type,_iUserLevel);
	ContSpendMoney::const_iterator itr = m_kContSpendMoney.find(kKey);
	if ( itr!=m_kContSpendMoney.end() )
	{
		return itr->second;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}