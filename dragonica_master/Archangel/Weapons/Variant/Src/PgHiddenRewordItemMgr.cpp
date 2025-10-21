#include "stdafx.h"
#include "PgHiddenRewordItemMgr.h"
#include "tabledatamanager.h"

void PgHiddenRewordItemMgrImpl::Clear()
{
}

void PgHiddenRewordItemMgrImpl::Init()
{
}

int const PgHiddenRewordItemMgrImpl::GetMyItem_ViewCount(int const iLevel, int const iItemCount)
{
	CONT_HIDDENREWORDITEM const *kContHiddenRewordItem = NULL;
	g_kTblDataMgr.GetContDef(kContHiddenRewordItem);
	if( kContHiddenRewordItem )
	{
		CONT_HIDDENREWORDITEM::key_type iMaxItemCount = 0;

		CONT_HIDDENREWORDITEM::const_iterator iter = kContHiddenRewordItem->begin();
		while( kContHiddenRewordItem->end() != iter )
		{
			CONT_HIDDENREWORDITEM::key_type rkItemCount = (*iter).first;
			CONT_HIDDENREWORDITEM::mapped_type rkItemValue = (*iter).second;

			if( iMaxItemCount < rkItemCount )
			{
				iMaxItemCount = rkItemCount;
			}			

			if( 0 < rkItemCount )
			{
				if( iItemCount < rkItemCount )
				{
					if( true == rkItemValue.CheckLevel(iLevel) )
					{
						return static_cast<int>(rkItemCount);
					}
				}
			}
			++iter;
		}
		return static_cast<int>(iMaxItemCount);	// 해당되는 데이터가 없으면 마지막 데이터를 준다
	}
	return 0;
}

int const PgHiddenRewordItemMgrImpl::GetMyItem_ViewItemNo(int const iLevel, int const iItemCount, bool bItemCount)
{
	int iFirstItem = 0;
	int iViewItemCount = 0;
	int iViewItemNo = 0;
	CONT_HIDDENREWORDITEM const *kContHiddenRewordItem = NULL;
	g_kTblDataMgr.GetContDef(kContHiddenRewordItem);
	if( kContHiddenRewordItem )
	{
		CONT_HIDDENREWORDITEM::const_iterator iter = kContHiddenRewordItem->begin();
		while( kContHiddenRewordItem->end() != iter )
		{
			CONT_HIDDENREWORDITEM::key_type rkItemCount = (*iter).first;
			CONT_HIDDENREWORDITEM::mapped_type rkItemValue = (*iter).second;

			if( true == rkItemValue.CheckLevel(iLevel) )
			{
				if( 0 == iFirstItem )
				{
					iFirstItem = rkItemValue.iItemNo;
					iViewItemCount = rkItemCount;
					iViewItemNo = rkItemValue.iItemNo;
				}

				if( rkItemCount <= iItemCount )
				{
					iViewItemCount = rkItemCount;
					iViewItemNo = rkItemValue.iItemNo;
				}
				else
				{
					if( true == bItemCount )
					{
						return static_cast<int>(iViewItemCount);
					}
					else
					{
						return static_cast<int>(iViewItemNo);
					}
				}
			}
			++iter;
		}
	}
	if( true == bItemCount )
	{
		return static_cast<int>(iViewItemCount);
	}
	else
	{
		return static_cast<int>(iViewItemNo);	// 해당되는 데이터가 없으면 최근의 데이터를 대신 준다
	}
}

bool PgHiddenRewordItemMgrImpl::GiveHiddenRewordItem(CONT_PLAYER_MODIFY_ORDER& kOrder, RESULT_SITEM_BAG_LIST_CONT& kContItem, CUnit* pkUnit, int const iItemNo, int const iItemNeedCount, int iNeedItemNo)
{
	if( !pkUnit )
	{
		return false;
	}

	if( 0 >= iItemNo )
	{
		return false;
	}

	if( 0 >= iItemNeedCount )
	{
		return false;
	}

	int const iCount  = 1;
	PgBase_Item kItem;
	if( S_OK != CreateSItem(iItemNo, iCount, 0, kItem) )
	{
		return false;
	}

	kContItem.clear();

	kContItem.push_back(kItem);

	size_t const kStoneCount = pkUnit->GetInven()->GetTotalCount(iNeedItemNo);

	if( iItemNeedCount <=  kStoneCount )
	{	
		SItemPos kItemPos;
		if(S_OK == pkUnit->GetInven()->GetFirstItem(IT_ETC, iNeedItemNo, kItemPos))
		{
			PgBase_Item kItem;
			if( S_OK == pkUnit->GetInven()->GetItem(kItemPos, kItem) )
			{
				SPMOD_Modify_Count kAddItem(kItem, kItemPos, -iItemNeedCount);
				SPMO kIMO(IMET_MODIFY_COUNT, pkUnit->GetID(), kAddItem);
				kOrder.push_back(kIMO);

				if( kContItem.size() )
				{
					return true;
				}
			}
		}
	}
	else
	{
		pkUnit->SendWarnMessage(400998);
	}
	return false;
}

//===============================================================================================================================================================================================

void PgHiddenRewordItemMgr::Clear()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
	Instance()->Clear();
}

void PgHiddenRewordItemMgr::Init()
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);
	Instance()->Init();
}

int const PgHiddenRewordItemMgr::GetMyItem_ViewCount(int const iLevel, int const iItemCount)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMyItem_ViewCount(iLevel, iItemCount);
}

int const PgHiddenRewordItemMgr::GetMyItem_ViewItemNo(int const iLevel, int const iItemCount, bool bItemCount)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_);
	return Instance()->GetMyItem_ViewItemNo(iLevel, iItemCount, bItemCount);
}

bool PgHiddenRewordItemMgr::GiveHiddenRewordItem(CONT_PLAYER_MODIFY_ORDER& kOrder, RESULT_SITEM_BAG_LIST_CONT& kContItem, CUnit* pkUnit, int const iItemNo, int const iItemNeedCount, int iNeedItemNo)
{
	BM::CAutoMutex lock(m_kMutex_Wrapper_, true);

	return Instance()->GiveHiddenRewordItem(kOrder, kContItem, pkUnit, iItemNo, iItemNeedCount, iNeedItemNo);
}