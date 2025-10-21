#include "StdAfx.h"
#include "TableDataManager.h"
#include "Lohengrin/LogGroup.h"
//#include "ItemBagMgr.h"
#include "ItemMakingDefMgr.h"
#include "PgControlDefMgr.h"

CItemMakingDefMgr::CItemMakingDefMgr(void)
{
}

CItemMakingDefMgr::~CItemMakingDefMgr(void)
{
}

bool CItemMakingDefMgr::Build( CONT_DEFITEMMAKING  const* pkDefItemMaking, CONT_DEFRESULT_CONTROL  const* pkDefResultControl )
{
	m_kDefItemMaking = *pkDefItemMaking;
	m_kDefResultControl = *pkDefResultControl;

	if ( !m_kDefItemMaking.size() &&
		 !m_kDefResultControl.size() )
	{
		assert(NULL);
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

//bool CItemMakingDefMgr::GetMakingInfo(TBL_DEF_ITEMMAKING& rkOutInfo, int iNo)const
//{
//	CONT_DEFITEMMAKING::const_iterator find_itr = m_kDefItemMaking.find(iNo);
//	if (find_itr != m_kDefItemMaking.end())
//	{
//		rkOutInfo = (*find_itr).second;
//		return true;
//	}
//	return false;
//}
//
bool CItemMakingDefMgr::GetResultInfo(TBL_DEF_RESULT_CONTROL& rkOutInfo, int iNo)const
{
	const CONT_DEFRESULT_CONTROL* pCont = &m_kDefResultControl;
	if (!m_kDefResultControl.size())
	{
		g_kTblDataMgr.GetContDef(pCont);
	}
	if (!pCont)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFRESULT_CONTROL::const_iterator find_itr = pCont->find(iNo);
	if (find_itr != pCont->end())
	{
		rkOutInfo = (*find_itr).second;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CItemMakingDefMgr::GetDefItemMaking(CONT_DEFITEMMAKING& rkDefItemMaking)const
{
	rkDefItemMaking = m_kDefItemMaking;
	return true;
}

bool CItemMakingDefMgr::GetDefResultControl(CONT_DEFRESULT_CONTROL& rkDefResultControl)const
{
	rkDefResultControl = m_kDefResultControl;
	return true;
}

bool CItemMakingDefMgr::GetDefItemMakingInfo(TBL_DEF_ITEMMAKING& rkDefItemMaking, int const iMakingNo)const
{
	const CONT_DEFITEMMAKING* pCont = NULL;
	g_kTblDataMgr.GetContDef(pCont);

	CONT_DEFITEMMAKING::const_iterator itr = pCont->find(iMakingNo);
	if(itr != pCont->end())
	{
		rkDefItemMaking = (*itr).second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool CItemMakingDefMgr::GetDefItemMakingInfo(SDefItemMakingData& rkOutData, int const iMakingNo)const
{	// Def Item Making
	TBL_DEF_ITEMMAKING kDefMaking;
	bool const bRet = GetDefItemMakingInfo(kDefMaking, iMakingNo);
	if (!bRet)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	rkOutData.iNo = kDefMaking.iNo;
	rkOutData.iNeedMoney = kDefMaking.iNeedMoney;
	rkOutData.iTypeNo = kDefMaking.iTypeNo;
	rkOutData.iContentStringNo = kDefMaking.iContentStringNo;
	rkOutData.iNeedElementsNo = kDefMaking.iNeedElementsNo;
	rkOutData.iNeedCountControlNo = kDefMaking.iNeedCountControlNo;
	rkOutData.iSuccesRateControlNo = kDefMaking.iSuccesRateControlNo;
	rkOutData.iResultControlNo = kDefMaking.iResultControlNo;
	rkOutData.iRarityControlNo = kDefMaking.iRarityControlNo;

	// Need Data
	GET_DEF(CItemBagMgr, kItemBagMgr);
	if (kItemBagMgr.GetItemBagElements(kDefMaking.iNeedElementsNo, rkOutData.kNeedElements) != S_OK)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if (kItemBagMgr.GetCountControl(kDefMaking.iNeedCountControlNo, rkOutData.kNeedCount) != S_OK)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	// Output Data
	for (int i=0 ; i<MAX_ITEMMAKING_ARRAY; ++i)
	{
		rkOutData.aiElements[i] = kDefMaking.aiElements[i];
		rkOutData.aiCountControl[i] = kDefMaking.aiCountControl[i];
		if (kItemBagMgr.GetItemBagElements(rkOutData.aiElements[i], rkOutData.akResultItemElements[i]) != S_OK)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		if (kItemBagMgr.GetCountControl(rkOutData.aiCountControl[i], rkOutData.akResultItemCount[i]) != S_OK)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	return true;
}

bool CItemMakingDefMgr::GetContainer(const EMakingType eType, ItemMakingDef_Cont &rkCont) const
{
	rkCont.clear();
	if (EMAKING_TYPE_NONE==eType)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	const CONT_DEFITEMMAKING* pCont = &m_kDefItemMaking;
	if (!m_kDefItemMaking.size())
	{
		g_kTblDataMgr.GetContDef(pCont);
	}
	if (!pCont)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	CONT_DEFITEMMAKING::const_iterator def_itr = pCont->begin();
	while (pCont->end() != def_itr)
	{
		const TBL_DEF_ITEMMAKING& rkDef = (*def_itr).second;
		if( 0 != (rkDef.iTypeNo & eType) )
		{
			SDefItemMakingData kTempData;
			if(GetDefItemMakingInfo(kTempData, rkDef.iNo))
			{
				rkCont.insert(std::make_pair(rkDef.iNo, kTempData));
			}
		}
		++def_itr;
	}
	
	return true;
}