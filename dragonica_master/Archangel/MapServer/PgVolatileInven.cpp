#include "stdafx.h"
#include "Variant/item.h"
#include "Variant/PgStringUtil.h"
#include "Variant/PgBattleSquare.h"
#include "Global.h"
#include "PgGenPointMgr.h"
#include "PgVolatileInven.h"

////
namespace VolatileInvenUtil
{
	BM::TObjectPool< PgVolatileInven > g_kVolatileInvenPool;

	PgRecycleAbleNum::PgRecycleAbleNum(size_t const iMax)
		: m_Next(0), m_Max(iMax), m_kContFreeNum()
	{
	}
	PgRecycleAbleNum::~PgRecycleAbleNum()
	{
	}
	void PgRecycleAbleNum::Clear()
	{
		m_kContFreeNum.clear();
		m_Next = 0;
	}
	size_t PgRecycleAbleNum::Pop()
	{
		if( !m_kContFreeNum.empty() )
		{
			size_t const NewNum = m_kContFreeNum.front();
			m_kContFreeNum.pop_front();
			return NewNum;
		}

		if( m_Max <= m_Next )
		{
			return 0;
		}

		return ++m_Next;
	}
	void PgRecycleAbleNum::Push(size_t FreeNum)
	{
		m_kContFreeNum.push_back(FreeNum);
		m_kContFreeNum.sort();
	}
};


////////////////////////////////////////////////////////////////
//		PgVolatileInven
////////////////////////////////////////////////////////////////
namespace VolatileInvenUtil
{
	void AddBuff(CONT_BS_BUFF_COUNT& rkCont, int const iBuff, CONT_BS_BUFF& rkAddOutBuff)
	{
		if( 0 != iBuff )
		{
			auto kRet = rkCont.insert( std::make_pair(iBuff, 1) );
			if( !kRet.second )
			{
				++(*kRet.first).second;
			}
			else
			{
				rkAddOutBuff.insert( iBuff );
			}
		}
	}
	void DelBuff(CONT_BS_BUFF_COUNT& rkCont, int const iBuff, CONT_BS_BUFF& rkDelOutBuff)
	{
		if( 0 != iBuff )
		{
			CONT_BS_BUFF_COUNT::iterator find_iter = rkCont.find( iBuff );
			if( rkCont.end() != find_iter )
			{
				--(*find_iter).second;
				if( 0 >= (*find_iter).second )
				{
					rkDelOutBuff.insert( iBuff );
					rkCont.erase( find_iter );
				}
			}
		}
	}
	void AddIcon(CONT_BS_MINIMAP_ICON& rkCont, int const iIcon)
	{
		if( 0 != iIcon )
		{
			auto kRet = rkCont.insert( std::make_pair(iIcon, 1) );
			if( !kRet.second )
			{
				++(*kRet.first).second;
			}
		}
	}
	void DelIcon(CONT_BS_MINIMAP_ICON& rkCont, int const iIcon)
	{
		if( 0 != iIcon )
		{
			CONT_BS_MINIMAP_ICON::iterator find_iter = rkCont.find( iIcon );
			if( rkCont.end() != find_iter )
			{
				--(*find_iter).second;
				if( 0 >= (*find_iter).second )
				{
					rkCont.erase( find_iter );
				}
			}
		}
	}
};
PgVolatileInven::PgVolatileInven()
	: m_kOwnerGuid(), m_kContVolatileItem(), m_kNum(MAX_VOLATILE_ITEM_COUNT)
	, m_kContBuffCount(), m_kContMinmapIcon()
{
}

PgVolatileInven::~PgVolatileInven()
{
}

void PgVolatileInven::Clear()
{
	m_kOwnerGuid.Clear();
	m_kContVolatileItem.clear();
	m_kNum.Clear();
	m_kContBuffCount.clear();
}

size_t PgVolatileInven::Push(CONT_BS_ITEM const& rkBSItem, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff)
{
	size_t const iNewInvenPos = m_kNum.Pop();
	if( 0 == iNewInvenPos )
	{
		return 0;
	}

	auto kRet = m_kContVolatileItem.insert( std::make_pair(iNewInvenPos, rkNewItem) );
	if( !kRet.second )
	{
		CAUTION_LOG(BM::LOG_LV2, __FL__<<L"Duplicate the new item, User["<<m_kOwnerGuid<<L"] ItemPos["<<iNewInvenPos<<L"]");
		return 0; // 중복 (?)
	}

	CONT_BS_ITEM::const_iterator find_iter = rkBSItem.find(rkNewItem.ItemNo());
	if( rkBSItem.end() != find_iter )
	{
		CONT_BS_ITEM::mapped_type const& rkItemPoint = (*find_iter).second;
		VolatileInvenUtil::AddBuff(m_kContBuffCount, rkItemPoint.iEffect1, kOutAddBuff);
		VolatileInvenUtil::AddBuff(m_kContBuffCount, rkItemPoint.iEffect2, kOutAddBuff);
		VolatileInvenUtil::AddBuff(m_kContBuffCount, rkItemPoint.iEffect3, kOutAddBuff);
		VolatileInvenUtil::AddIcon(m_kContMinmapIcon, rkItemPoint.iMinimapIcon);
	}
	return iNewInvenPos;
}
void PgVolatileInven::Swap(CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkContDelBuff)
{
	m_kNum.Clear();
	m_kContVolatileItem.swap(rkOut);
	CONT_BS_BUFF_COUNT::const_iterator iter = m_kContBuffCount.begin();
	while( m_kContBuffCount.end() != iter )
	{
		rkContDelBuff.insert( (*iter).first );
		++iter;
	}
	m_kContBuffCount.clear();
	m_kContMinmapIcon.clear();
}

int PgVolatileInven::IsHaveMinimapIcon() const
{
	if( m_kContMinmapIcon.empty() )
	{
		return 0;
	}
	return (*m_kContMinmapIcon.begin()).first;
}

int PgVolatileInven::MinimapIconCount() const
{
	return for_each(m_kContMinmapIcon.begin(), m_kContMinmapIcon.end(), SumSecondValue<CONT_BS_MINIMAP_ICON::const_iterator::value_type>());
}

////////////////////////////////////////////////////////////////
//		PgVolatileInvenMng
////////////////////////////////////////////////////////////////
PgVolatileInvenMng::PgVolatileInvenMng()
	: m_kContVolatileIneven()
{
}

PgVolatileInvenMng::~PgVolatileInvenMng()
{
}

void PgVolatileInvenMng::ClearInven()
{
	CONT_VOLATILE_INEVEN::iterator loop_iter = m_kContVolatileIneven.begin();
	while( m_kContVolatileIneven.end() != loop_iter )
	{
		VolatileInvenUtil::g_kVolatileInvenPool.Delete( (*loop_iter).second );
		++loop_iter;
	}
	m_kContVolatileIneven.clear();
}

bool PgVolatileInvenMng::AddInven(BM::GUID const& rkOwnerGuid)
{
	if( m_kContVolatileIneven.end() != m_kContVolatileIneven.find(rkOwnerGuid) )
	{
		return false;
	}

	PgVolatileInven* pkNewInven = VolatileInvenUtil::g_kVolatileInvenPool.New();
	if( !pkNewInven )
	{
		return false;
	}

	pkNewInven->Clear();
	pkNewInven->OwnerGuid(rkOwnerGuid);

	auto kRet = m_kContVolatileIneven.insert( std::make_pair(rkOwnerGuid, pkNewInven) );
	if( !kRet.second )
	{
		VolatileInvenUtil::g_kVolatileInvenPool.Delete(pkNewInven);
		return false;
	}
	return true;
}

bool PgVolatileInvenMng::DelInven(BM::GUID const& rkOwnerGuid)
{
	CONT_VOLATILE_INEVEN::iterator find_iter = m_kContVolatileIneven.find( rkOwnerGuid );
	if( m_kContVolatileIneven.end() == find_iter )
	{
		return false;
	}

	PgVolatileInven* pkInven = (*find_iter).second;

	m_kContVolatileIneven.erase( find_iter );

	if( pkInven )
	{
		pkInven->Clear();
		VolatileInvenUtil::g_kVolatileInvenPool.Delete(pkInven);
	}
	return true;
}

size_t PgVolatileInvenMng::PushItem(CONT_BS_ITEM const& rkBSItem, BM::GUID const& rkOwnerGuid, PgBase_Item const& rkNewItem, CONT_BS_BUFF& kOutAddBuff)
{
	PgVolatileInven* pkInven = Get(rkOwnerGuid);
	if( !pkInven )
	{
		return 0;
	}
	return pkInven->Push(rkBSItem, rkNewItem, kOutAddBuff);
}

bool PgVolatileInvenMng::SwapInven(BM::GUID const& rkOwnerGuid, CONT_VOLATILE_INVEN_ITEM& rkOut, CONT_BS_BUFF& rkContDelBuff)
{
	PgVolatileInven* pkInven = Get(rkOwnerGuid);
	if( !pkInven )
	{
		return false;
	}
	pkInven->Swap(rkOut, rkContDelBuff);
	return true;
}

PgVolatileInven* PgVolatileInvenMng::Get(BM::GUID const& rkOwnerGuid)
{
	CONT_VOLATILE_INEVEN::iterator find_iter = m_kContVolatileIneven.find(rkOwnerGuid);
	if( m_kContVolatileIneven.end() == find_iter )
	{
		return NULL;
	}
	return (*find_iter).second;
}

bool PgVolatileInvenMng::GetMinimapIconUser(CONT_BS_HAVE_MINIMAP_ICON& rkOut) const
{
	CONT_VOLATILE_INEVEN::const_iterator iter = m_kContVolatileIneven.begin();
	while( m_kContVolatileIneven.end() != iter )
	{
		CONT_VOLATILE_INEVEN::key_type const& rkKey = (*iter).first;
		CONT_VOLATILE_INEVEN::mapped_type const& rkInven = (*iter).second;

		int const iMinimapIcon = rkInven->IsHaveMinimapIcon();
		if( 0 != iMinimapIcon )
		{
			rkOut.insert( std::make_pair(rkKey, BSIconOwnInfo(rkKey, BST_NONE, iMinimapIcon, rkInven->MinimapIconCount())) );
		}
		++iter;
	}
	return true;
}

int PgVolatileInvenMng::GetMinimapIconCount(VEC_GUID const& rkVecGuid) const
{
	int iCount = 0;
	VEC_GUID::const_iterator itor_vec = rkVecGuid.begin();
	while (rkVecGuid.end() != itor_vec)
	{
		PgVolatileInven const* pkInven = Get(*itor_vec);
		if (pkInven != NULL)
		{
			iCount += pkInven->MinimapIconCount();
		}
		++itor_vec;
	}
	return iCount;
}


PgVolatileInven const* PgVolatileInvenMng::Get(BM::GUID const& rkOwnerGuid) const
{
	//return const_cast<PgVolatileInven*>(this)->Get(rkOwnerGuid);
	CONT_VOLATILE_INEVEN::const_iterator find_iter = m_kContVolatileIneven.find(rkOwnerGuid);
	if( m_kContVolatileIneven.end() == find_iter )
	{
		return NULL;
	}
	return (*find_iter).second;
}

int PgVolatileInvenMng::GetIconCount(BM::GUID const& kGuid) const
{
	PgVolatileInven const* pkInven = Get(kGuid);
	if (pkInven != NULL)
	{
		return pkInven->MinimapIconCount();
	}
	return 0;
}