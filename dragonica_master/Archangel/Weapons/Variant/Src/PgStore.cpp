#include "StdAfx.h"
#include "PgStore.h"
#include "PgControlDefMgr.h"

PgStore::PgStore(BYTE const byType)
	:m_byType(byType)
{
}

PgStore::~PgStore(void)
{
}

void PgStore::Init(BM::GUID const &kShopGuid)
{
	m_kGoods.clear();
	m_kShopGuid = kShopGuid;
}

void PgStore::WriteToPacket(BM::Stream &kPacket)const
{
	BM::CAutoMutex kLock(m_kMutex);
	
	kPacket.Push(m_byType);
	kPacket.Push(m_kShopGuid);
	kPacket.Push(m_kGoods);
}

void PgStore::ReadFromPacket(BM::Stream &kPacket)
{//?? 읽을 일이 있나?
	BM::CAutoMutex kLock(m_kMutex);
	
	m_kGoods.clear();

	kPacket.Pop(m_byType);
	kPacket.Pop(m_kShopGuid);
	kPacket.Pop(m_kGoods);
}
	
PgBase_Item const* PgStore::GetViewItem(int const iIDX)const 
{
	if(m_kGoodsView.size() > (unsigned int)iIDX)
	{
		return &m_kGoodsView.at(iIDX);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

HRESULT PgStore::GetGoods(int const iItemNo, CONT_GODDS::value_type &rkOutGoods)const 
{
	CONT_GODDS::const_iterator goods_itor = m_kGoods.begin();
	while(goods_itor != m_kGoods.end())
	{
		if((*goods_itor).iItemNo == iItemNo)
		{
			rkOutGoods = (*goods_itor);
			return S_OK;
		}
		++goods_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgStore::GetGoodsToIdx(int const Idx, CONT_GODDS::value_type  &rkOutGoods)const
{
	if( m_kGoods.size() > static_cast<size_t>(Idx) )
	{
		CONT_GODDS::const_iterator goods_itor = m_kGoods.begin();
		for(int i = 0; i < Idx; ++i, ++goods_itor)
		{
			if( m_kGoods.end() == goods_itor )
			{
				return E_FAIL;
			}
		}
		rkOutGoods = (*goods_itor);
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgStore::AddGoods(const TBL_SHOP_IN_GAME &rkTbl)//테이블 데이터 넣는거.
{//같은 품목이 있는건지 검사 같은건 해야 되는데.
	BM::CAutoMutex kLock(m_kMutex);
	m_kGoods.push_back(rkTbl);

	return S_OK;
}

void PgStore::BuildByView(int const iView, __int64 const iOrderBy)
{
	m_kGoodsView.clear();

	CONT_GODDS::iterator goods_itor = m_kGoods.begin();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	while(goods_itor != m_kGoods.end())
	{
		CItemDef const *pkItemDef = kItemDefMgr.GetDef((*goods_itor).iItemNo);
		if(pkItemDef)
		{
			if(	(*goods_itor).iCategory == iView
			)// &&	!(*goods_itor).iState)//판매 상태이면.
			{
				__int64 const i64ClassLimit = pkItemDef->GetAbil64(AT_CLASSLIMIT);

				if(	!iOrderBy//착용제한값이 없거나.
				//||	0 != (i64ClassLimit & (__int64)pow((double)2, iOrderBy)))//착용가능 클래스라면.
				|| IS_CLASS_LIMIT2(i64ClassLimit, iOrderBy))
				{
					PgBase_Item kItem;
					kItem.ItemNo((*goods_itor).iItemNo);
					kItem.Count((WORD)pkItemDef->MaxAmount());
					m_kGoodsView.push_back(kItem);
				}
			}
		}
		++goods_itor;
	}
}
void PgStore::BuildByJobSkill(int const iMain, int const iSub)
{
	m_kGoodsView.clear();

	CONT_GODDS::iterator goods_itor = m_kGoods.begin();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	while(goods_itor != m_kGoods.end())
	{
		CItemDef const *pkItemDef = kItemDefMgr.GetDef((*goods_itor).iItemNo);
		if(pkItemDef)
		{
			if( iMain == 0
				|| (*goods_itor).iCategory == iMain )
			{
				__int64 const i64ClassLimit = pkItemDef->GetAbil64(AT_CLASSLIMIT);

				if(	!iSub || (*goods_itor).iCP == iSub )
				{
					PgBase_Item kItem;
					kItem.ItemNo((*goods_itor).iItemNo);
					kItem.Count((WORD)pkItemDef->MaxAmount());
					m_kGoodsView.push_back(kItem);
				}
			}
		}
		++goods_itor;
	}
}
void PgStore::BuildAll()
{
	m_kGoodsView.clear();

	CONT_GODDS::iterator goods_itor = m_kGoods.begin();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	while(goods_itor != m_kGoods.end())
	{
		CItemDef const *pkItemDef = kItemDefMgr.GetDef((*goods_itor).iItemNo);
		if(pkItemDef)
		{//아이템이기만 하면 무조건 넣는다.
			PgBase_Item kItem;
			kItem.ItemNo((*goods_itor).iItemNo);
			kItem.Count((WORD)pkItemDef->MaxAmount());
			m_kGoodsView.push_back(kItem);
		}
		++goods_itor;
	}
}

int PgStore::GetGoodsViewSize() const
{
	return static_cast<int>(m_kGoodsView.size());
}
void PgStore::swap(PgStore &rRight)
{
	m_kGoods.swap(rRight.m_kGoods);
	m_kGoodsView.swap(rRight.m_kGoodsView);
}

int PgStore::GetGoodsCountByView(int const iView) const
{
	CONT_GODDS::const_iterator goods_itor = m_kGoods.begin();
	int iCount = 0;
	while(goods_itor != m_kGoods.end())
	{
		if((*goods_itor).iCategory == iView)
		{
			++iCount;
		}
		++goods_itor;
	}

	return iCount;
}

bool PgStore::IsHaveCPItem()	//CP로 판매되는 아이템을 들고 있냐?
{
	CONT_GODDS::const_iterator goods_itor = m_kGoods.begin();
	while(goods_itor != m_kGoods.end())
	{
		GOODS const & rkGoods = (*goods_itor);
		if (0<rkGoods.iCP)
		{
			return true;
		}
		++goods_itor;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgStore::IsHaveCoinItem()
{
	CONT_GODDS::const_iterator goods_itor = m_kGoods.begin();
	while(goods_itor != m_kGoods.end())
	{
		GOODS const & rkGoods = (*goods_itor);
		if (0<rkGoods.iCoin)
		{
			return true;
		}
		++goods_itor;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

PgStore& PgStore::operator = (PgStore const &rhs)
{
	m_kGoods = rhs.m_kGoods;
	m_kGoodsView = rhs.m_kGoodsView;
	m_kShopGuid = rhs.m_kShopGuid;
	m_byType = rhs.m_byType;
	return *this;
}
