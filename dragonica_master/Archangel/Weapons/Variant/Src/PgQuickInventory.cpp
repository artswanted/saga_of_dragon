#include "StdAfx.h"
#include "Inventory.h"
#include "ItemDefMgr.h"
#include "Unit.h"
#include "PgQuickInventory.h"
#include "TableDataManager.h"


tagQuickInvInfo::tagQuickInvInfo()
{
	Clear();
}

void tagQuickInvInfo::Clear()
{
	Set(0,0);
}

void tagQuickInvInfo::Set(BYTE const byInGrp, int const iInID)
{
	Grp(byInGrp);
	ID(iInID);
}

bool tagQuickInvInfo::IsEmpty()const 
{
	if( Grp() == 0 
	||	ID() == 0)
	{
		return true;
	}
	return false;
}
	
//////////////////////////////////////////////////////////////////////////////////////
//		PgQuickInventory
//////////////////////////////////////////////////////////////////////////////////////
PgQuickInventory::PgQuickInventory()
{
	m_kBuffer.reserve(Q_INV_MAX_COUNT);
	m_kBuffer.resize(Q_INV_MAX_COUNT);
	m_kViewPage = 0;
}

PgQuickInventory::~PgQuickInventory()
{
}

PgQuickInventory::PgQuickInventory( PgQuickInventory const &rhs )
{
	*this = rhs;
}

PgQuickInventory& PgQuickInventory::operator=( PgQuickInventory const &rhs )
{
	m_kBuffer = rhs.m_kBuffer;
	m_kViewPage = rhs.m_kViewPage;
	return *this;
}

void PgQuickInventory::Swap(PgQuickInventory &rhs)
{
	m_kBuffer.swap(rhs.m_kBuffer);
	std::swap(m_kViewPage, rhs.m_kViewPage);
}

bool PgQuickInventory::IsCorrectIDX(size_t const slot_idx)const
{
	if(m_kBuffer.size() > slot_idx)
	{
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgQuickInventory::Clear()
{
	m_kBuffer.clear();
	m_kBuffer.resize(Q_INV_MAX_COUNT);
}

HRESULT PgQuickInventory::Init(BYTE const abyQuickSlot[MAX_DB_QUICKSLOT_SIZE])
{
	Clear();

	size_t const slot_mem_size = m_kBuffer.size() * sizeof(SLOT_CONT::value_type);
	if(MAX_DB_QUICKSLOT_SIZE == slot_mem_size + sizeof(char))
	{
		memcpy(&m_kBuffer.at(0), &abyQuickSlot[0], slot_mem_size );
		memcpy(&m_kViewPage, &abyQuickSlot[slot_mem_size], sizeof(char) );
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::GetItem(size_t const slot_idx, SQuickInvInfo &rkOutItem)const
{
	size_t const ret_idx = slot_idx;// + (m_kViewPage()*8);//

	if(IsCorrectIDX(ret_idx))
	{
		rkOutItem = m_kBuffer.at(ret_idx);
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::FindEmptySlot(size_t &rkResult)const
{
	SLOT_CONT::const_iterator slot_itor = m_kBuffer.begin();

	rkResult = 0;
	while(slot_itor != m_kBuffer.end())
	{
		if((*slot_itor).IsEmpty())
		{
			return S_OK;
		}
		++rkResult;
		++slot_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::Modify(size_t const slot_idx, SQuickInvInfo &rkInfo)
{
	size_t const ret_idx = slot_idx;// + (m_kViewPage()*8);//

	if(IsCorrectIDX(ret_idx))
	{
		if(rkInfo.IsEmpty())
		{
			rkInfo.Clear();
		}

		m_kBuffer.at(ret_idx) = rkInfo;
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::Modify(char const cViewPage)//서버에서만.
{
	m_kViewPage = cViewPage;
	AdjustViewPage();
	return S_OK;
}

HRESULT PgQuickInventory::DeleteItem(size_t const slot_idx)
{
	size_t const ret_idx = slot_idx;// + (m_kViewPage()*8);//

	if(IsCorrectIDX(ret_idx))
	{
		m_kBuffer.at(ret_idx).Clear();
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::IsExist(int const iItemNo)
{
	if(0>=iItemNo) {return E_FAIL;}

	SLOT_CONT::const_iterator slot_itor = m_kBuffer.begin();
	while(slot_itor != m_kBuffer.end())
	{
		if(!(*slot_itor).IsEmpty())
		{
			if((*slot_itor).ID() == iItemNo)
			{
				return S_OK;
			}
		}
		++slot_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgQuickInventory::Save(BYTE *abyDBData)const
{
	size_t const slot_mem_size = m_kBuffer.size() * sizeof(SLOT_CONT::value_type);

	if(MAX_DB_QUICKSLOT_SIZE == slot_mem_size + sizeof(m_kViewPage))
	{
		memcpy(abyDBData, &m_kBuffer.at(0), slot_mem_size);
		memcpy(&abyDBData[slot_mem_size], &m_kViewPage, sizeof(m_kViewPage));
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgQuickInventory::WriteToPacket(BM::Stream &rkPacket) const
{
	rkPacket.Push(m_kBuffer);
	rkPacket.Push(m_kViewPage);
}

void PgQuickInventory::ReadFromPacket(BM::Stream &rkPacket)
{
	rkPacket.Pop(m_kBuffer);
	rkPacket.Pop(m_kViewPage);
}

int PgQuickInventory::ViewPage()const
{
	return m_kViewPage;
}

int PgQuickInventory::IncViewPage()
{
	++m_kViewPage;
	if(MAX_PAGE == m_kViewPage)
	{
		m_kViewPage = 0;
	}
	return AdjustViewPage();
}

int PgQuickInventory::DecViewPage()
{
	--m_kViewPage;
	if(MIN_PAGE > m_kViewPage)
	{
		m_kViewPage = MAX_PAGE-1;//선형으로 돌도록.
	}
	return AdjustViewPage();
}

int PgQuickInventory::AdjustViewPage()
{
	m_kViewPage = __max(MIN_PAGE, m_kViewPage);
	m_kViewPage = __min(MAX_PAGE-1, m_kViewPage);
	return m_kViewPage;
}

size_t PgQuickInventory::AdjustSlotIDX(size_t const slot_idx)const
{
	if(slot_idx >= Q_INV_COUNT)
	{
		return (slot_idx % Q_INV_COUNT) + (MAX_PAGE * Q_INV_COUNT);
	}
	return slot_idx + (m_kViewPage * Q_INV_COUNT);
}