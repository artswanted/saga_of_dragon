#include "StdAfx.h"
#include "Global.h"
#include "Inventory.h"
#include "ItemSetDefMgr.h"
#include "PgPlayer.h"
#include "TableDataManager.h"
#include "PgControlDefMgr.h"
#include "PgEventView.h"
#include "PgSystemInvManager.h"
#include "InventoryUtil.h"

PgInventory::PgInventory(void)
	:	m_kEquipInv(m_kOwnerGuid)
	,	m_kConsumeInv(m_kOwnerGuid)
	,	m_kEtcInv(m_kOwnerGuid)
	,	m_kCashInv(m_kOwnerGuid)
	,	m_kSafeInv(m_kOwnerGuid)

	,	m_kCashSafeInv(m_kOwnerGuid)
	,	m_kHomeInv(m_kOwnerGuid)
	,	m_kFit(m_kOwnerGuid)
	,	m_kPostInv(m_kOwnerGuid)
	,	m_kUserMarketInv(m_kOwnerGuid)
	,	m_kFitCash(m_kOwnerGuid)

	,	m_kRentalSafe1(m_kOwnerGuid)
	,	m_kRentalSafe2(m_kOwnerGuid)
	,	m_kRentalSafe3(m_kOwnerGuid)

	,	m_kPetItem(m_kOwnerGuid)

	,	m_kShareRentalSafe1(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe2(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe3(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe4(m_kMemberGuid,m_kOwnerGuid)
	,	m_kSafeAddInv(m_kOwnerGuid)
	,	m_kFitCostume(m_kOwnerGuid)
{
	Money(0);
	CP(0);
	ExpCardItem(BM::GUID::NullData());
}

PgInventory::PgInventory(PgInventory const &rhs)
	:	m_kEquipInv(m_kOwnerGuid)
	,	m_kConsumeInv(m_kOwnerGuid)
	,	m_kEtcInv(m_kOwnerGuid)
	,	m_kCashInv(m_kOwnerGuid)
	,	m_kSafeInv(m_kOwnerGuid)

	,	m_kCashSafeInv(m_kOwnerGuid)
	,	m_kHomeInv(m_kOwnerGuid)
	,	m_kFit(m_kOwnerGuid)
	,	m_kPostInv(m_kOwnerGuid)
	,	m_kUserMarketInv(m_kOwnerGuid)
	,	m_kFitCash(m_kOwnerGuid)

	,	m_kRentalSafe1(m_kOwnerGuid)
	,	m_kRentalSafe2(m_kOwnerGuid)
	,	m_kRentalSafe3(m_kOwnerGuid)

	,	m_kPetItem(m_kOwnerGuid)
	,	m_kShareRentalSafe1(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe2(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe3(m_kMemberGuid,m_kOwnerGuid)
	,	m_kShareRentalSafe4(m_kMemberGuid,m_kOwnerGuid)
	,	m_kSafeAddInv(m_kOwnerGuid)
	,	m_kFitCostume(m_kOwnerGuid)
{
	m_kEquipInv	= rhs.m_kEquipInv;
	m_kConsumeInv = rhs.m_kConsumeInv;
	m_kEtcInv = rhs.m_kEtcInv;
	m_kCashInv = rhs.m_kCashInv;
	m_kHomeInv = rhs.m_kHomeInv;
	
	m_kSafeInv = rhs.m_kSafeInv;
	m_kCashSafeInv = rhs.m_kCashSafeInv;
	m_kFit = rhs.m_kFit;
	m_kPostInv = rhs.m_kPostInv;
	m_kUserMarketInv = rhs.m_kUserMarketInv;
	m_kFitCash = rhs.m_kFitCash;

	m_kRentalSafe1 = rhs.m_kRentalSafe1;
	m_kRentalSafe2 = rhs.m_kRentalSafe2;
	m_kRentalSafe3 = rhs.m_kRentalSafe3;

	m_kPetItem = rhs.m_kPetItem;

	m_kShareRentalSafe1 = rhs.m_kShareRentalSafe1;
	m_kShareRentalSafe2 = rhs.m_kShareRentalSafe2;
	m_kShareRentalSafe3 = rhs.m_kShareRentalSafe3;
	m_kShareRentalSafe4 = rhs.m_kShareRentalSafe4;

	m_kContUnbindTime = rhs.m_kContUnbindTime;

	m_kSafeAddInv = rhs.m_kSafeAddInv;
	m_kFitCostume = rhs.m_kFitCostume;

	Money(rhs.Money());
	CP(rhs.CP());
	OwnerGuid(rhs.OwnerGuid());
	OwnerName(rhs.OwnerName());
	MemberGuid(rhs.MemberGuid());
	ExpCardItem(rhs.ExpCardItem());

	m_kItemCoolTime = rhs.m_kItemCoolTime;
	m_kGroupCoolTime = rhs.m_kGroupCoolTime;
	m_kFullSetItem = rhs.m_kFullSetItem;
	m_kOverSkill = rhs.m_kOverSkill;

	CAbilObject::operator = ( rhs );
}

void PgInventory::Swap( PgInventory &kInv, bool const bSwapOwner )
{
	m_kEquipInv.Swap( kInv.m_kEquipInv );
	m_kConsumeInv.Swap( kInv.m_kConsumeInv );
	m_kEtcInv.Swap( kInv.m_kEtcInv );
	m_kCashInv.Swap( kInv.m_kCashInv );
	m_kHomeInv.Swap( kInv.m_kHomeInv );

	m_kSafeInv.Swap( kInv.m_kSafeInv );
	m_kCashSafeInv.Swap( kInv.m_kCashSafeInv );
	m_kFit.Swap( kInv.m_kFit );
	m_kPostInv.Swap( kInv.m_kPostInv );
	m_kUserMarketInv.Swap( kInv.m_kUserMarketInv );
	m_kFitCash.Swap( kInv.m_kFitCash );

	m_kRentalSafe1.Swap( kInv.m_kRentalSafe1 );
	m_kRentalSafe2.Swap( kInv.m_kRentalSafe2 );
	m_kRentalSafe3.Swap( kInv.m_kRentalSafe3 );

	m_kPetItem.swap( kInv.m_kPetItem );

	m_kShareRentalSafe1.swap( kInv.m_kShareRentalSafe1 );
	m_kShareRentalSafe2.swap( kInv.m_kShareRentalSafe2 );
	m_kShareRentalSafe3.swap( kInv.m_kShareRentalSafe3 );
	m_kShareRentalSafe4.swap( kInv.m_kShareRentalSafe4 );

	m_kContUnbindTime.swap( kInv.m_kContUnbindTime );

	m_kSafeAddInv.Swap( kInv.m_kSafeAddInv );
	m_kFitCostume.Swap(kInv.m_kFitCostume);

	if ( true == bSwapOwner )
	{// 주의
		std::swap( m_kOwnerGuid, kInv.m_kOwnerGuid );
		std::swap( m_kOwnerName, kInv.m_kOwnerName );
	}

	std::swap( m_kMoney, kInv.m_kMoney );
	std::swap( m_kCP, kInv.m_kCP );
	std::swap( m_kExpCardItem, kInv.m_kExpCardItem );

	m_kItemCoolTime.swap( kInv.m_kItemCoolTime );
	m_kGroupCoolTime.swap( kInv.m_kGroupCoolTime );
	m_kFullSetItem.swap( kInv.m_kFullSetItem );
	m_kOverSkill.swap( kInv.m_kOverSkill );

	CAbilObject::Swap( kInv );
}

PgInventory::~PgInventory(void)
{
}

void PgInventory::WriteToPacket(BM::Stream &kPacket, EWRITETYPE const kWriteType)
{
	if ((kWriteType & WT_OP_No_Contents) == 0)
	{
		WriteToPacket(kPacket, &m_kEquipInv);
		WriteToPacket(kPacket, &m_kConsumeInv);
		WriteToPacket(kPacket, &m_kEtcInv);
		WriteToPacket(kPacket, &m_kCashInv);
		WriteToPacket(kPacket, &m_kSafeInv);
		WriteToPacket(kPacket, &m_kCashSafeInv);
		WriteToPacket(kPacket, &m_kFit);
		WriteToPacket(kPacket, &m_kPostInv);
		WriteToPacket(kPacket, &m_kHomeInv);
		WriteToPacket(kPacket, &m_kUserMarketInv);
		WriteToPacket(kPacket, &m_kFitCash);

		WriteToPacket(kPacket, &m_kRentalSafe1);
		WriteToPacket(kPacket, &m_kRentalSafe2);
		WriteToPacket(kPacket, &m_kRentalSafe3);

		WriteToPacket(kPacket, &m_kPetItem);

		WriteToPacket(kPacket, &m_kShareRentalSafe1);
		WriteToPacket(kPacket, &m_kShareRentalSafe2);
		WriteToPacket(kPacket, &m_kShareRentalSafe3);
		WriteToPacket(kPacket, &m_kShareRentalSafe4);

		WriteToPacket(kPacket, &m_kSafeAddInv);
		WriteToPacket(kPacket, &m_kFitCostume);

		kPacket.Push(m_kMoney);
		kPacket.Push(m_kCP);

		PU::TWriteTable_AA(kPacket,m_kContUnbindTime);
	}
	
	kPacket.Push(m_kItemCoolTime);
	kPacket.Push(m_kGroupCoolTime);
	kPacket.Push(m_kExpCardItem);
}

bool PgInventory::WriteToPacket(EInvType const eInvType, BM::Stream &kPacket)
{
	ItemContainer* pCont = NULL;
	if(S_OK == GetContainer(eInvType, pCont))
	{
		WriteToPacket(kPacket, pCont);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgInventory::ReadFromPacket(EInvType const eInvType, BM::Stream &kPacket)
{
	ItemContainer *pCont = NULL;
	if(S_OK == GetContainer(eInvType, pCont))
	{
		ReadFromPacket(kPacket, pCont);
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgInventory::WriteToPacket(BM::Stream &kPacket, ItemContainer *pkCont)
{
	pkCont->WriteToPacket(kPacket);
}

void PgInventory::Clear()
{
	m_kEquipInv.Clear();
	m_kConsumeInv.Clear();
	m_kEtcInv.Clear();
	m_kCashInv.Clear();
	m_kSafeInv.Clear();
	m_kCashSafeInv.Clear();
	m_kFit.Clear();
	m_kPostInv.Clear();
	m_kHomeInv.Clear();
	m_kUserMarketInv.Clear();
	m_kFitCash.Clear();

	m_kRentalSafe1.Clear();
	m_kRentalSafe2.Clear();
	m_kRentalSafe3.Clear();

	m_kPetItem.Clear();

	m_kShareRentalSafe1.Clear();
	m_kShareRentalSafe2.Clear();
	m_kShareRentalSafe3.Clear();
	m_kShareRentalSafe4.Clear();

	m_kContUnbindTime.clear();

	m_kSafeAddInv.Clear();
	m_kFitCostume.Clear();

	m_kMoney = 0i64;
	m_kCP = 0;

	CAbilObject::Clear();
}

void PgInventory::Clear(EInvType const eInvType)
{
	ItemContainer* pkCont = NULL;
	switch(eInvType)
	{
	case IT_EQUIP:				{ pkCont = &m_kEquipInv; }break;
	case IT_CONSUME:			{ pkCont = &m_kConsumeInv;}break;
	case IT_ETC:				{ pkCont = &m_kEtcInv;	}break;
	case IT_CASH:				{ pkCont = &m_kCashInv;	}break;
	case IT_SAFE:				{ pkCont = &m_kSafeInv;	}break;
	case IT_CASH_SAFE:			{ pkCont = &m_kCashSafeInv;	}break;
	case IT_FIT:				{ pkCont = &m_kFit;	}break;
	case IT_POST:				{ pkCont = &m_kPostInv;	}break;
	case IT_HOME:				{ pkCont = &m_kHomeInv;	}break;
	case IT_USER_MARKET:		{ pkCont = &m_kUserMarketInv;}break;
	case IT_FIT_CASH:			{ pkCont = &m_kFitCash;}break;
	case IT_RENTAL_SAFE1:		{ pkCont = &m_kRentalSafe1;}break;
	case IT_RENTAL_SAFE2:		{ pkCont = &m_kRentalSafe2;}break;
	case IT_RENTAL_SAFE3:		{ pkCont = &m_kRentalSafe3;}break;
	case IT_PET:				{ pkCont = &m_kPetItem;}break;
	case IT_SHARE_RENTAL_SAFE1:	{ pkCont = &m_kShareRentalSafe1;}break;
	case IT_SHARE_RENTAL_SAFE2:	{ pkCont = &m_kShareRentalSafe2;}break;
	case IT_SHARE_RENTAL_SAFE3:	{ pkCont = &m_kShareRentalSafe3;}break;
	case IT_SHARE_RENTAL_SAFE4:	{ pkCont = &m_kShareRentalSafe4;}break;
	case IT_SAFE_ADDITION:		{ pkCont = &m_kSafeAddInv;}break;
	case IT_FIT_COSTUME:		{ pkCont = &m_kFitCostume;}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return ;
		}break;
	}
	pkCont->Clear();
}

void PgInventory::ReadFromPacket(BM::Stream &kPacket, EWRITETYPE const kWriteType)
{//아이템서버의 UnitInventory 와도 맞아야 한다.
	if ((kWriteType & WT_OP_No_Contents) == 0)
	{
		Clear();
		ReadFromPacket(kPacket, &m_kEquipInv);
		ReadFromPacket(kPacket, &m_kConsumeInv);
		ReadFromPacket(kPacket, &m_kEtcInv);
		ReadFromPacket(kPacket, &m_kCashInv);
		ReadFromPacket(kPacket, &m_kSafeInv);
		ReadFromPacket(kPacket, &m_kCashSafeInv);
		ReadFromPacket(kPacket, &m_kFit);
		ReadFromPacket(kPacket, &m_kPostInv);
		ReadFromPacket(kPacket, &m_kHomeInv);
		ReadFromPacket(kPacket, &m_kUserMarketInv);
		ReadFromPacket(kPacket, &m_kFitCash);

		ReadFromPacket(kPacket, &m_kRentalSafe1);
		ReadFromPacket(kPacket, &m_kRentalSafe2);
		ReadFromPacket(kPacket, &m_kRentalSafe3);

		ReadFromPacket(kPacket, &m_kPetItem);

		ReadFromPacket(kPacket, &m_kShareRentalSafe1);
		ReadFromPacket(kPacket, &m_kShareRentalSafe2);
		ReadFromPacket(kPacket, &m_kShareRentalSafe3);
		ReadFromPacket(kPacket, &m_kShareRentalSafe4);

		ReadFromPacket(kPacket, &m_kSafeAddInv);
		ReadFromPacket(kPacket, &m_kFitCostume);

		kPacket.Pop(m_kMoney);
		kPacket.Pop(m_kCP);

		PU::TLoadTable_AA(kPacket,m_kContUnbindTime);
	}

	kPacket.Pop(m_kItemCoolTime);
	kPacket.Pop(m_kGroupCoolTime);
	kPacket.Pop(m_kExpCardItem);
}

void PgInventory::ReadFromPacket(BM::Stream &kPacket, ItemContainer *pkCont)
{
	pkCont->ReadFromPacket(kPacket);
}

HRESULT PgInventory::GetContainer(EInvType const eInvType, ItemContainer *&pkCont)
{
	switch(eInvType)
	{
	case IT_EQUIP:		{ pkCont = &m_kEquipInv; }break;
	case IT_CONSUME:	{ pkCont = &m_kConsumeInv;}break;
	case IT_ETC:		{ pkCont = &m_kEtcInv;	}break;
	case IT_CASH:		{ pkCont = &m_kCashInv;	}break;
	case IT_SAFE:		{ pkCont = &m_kSafeInv;	}break;
	case IT_CASH_SAFE:	{ pkCont = &m_kCashSafeInv;	}break;
	case IT_FIT:		{ pkCont = &m_kFit;	}break;
	case IT_POST:		{ pkCont = &m_kPostInv;	}break;
	case IT_HOME:		{ pkCont = &m_kHomeInv;	}break;
	case IT_USER_MARKET:{ pkCont = &m_kUserMarketInv;}break;
	case IT_FIT_CASH:	{ pkCont = &m_kFitCash;}break;
	case IT_FIT_COSTUME: { pkCont = &m_kFitCostume;}break;
	case IT_RENTAL_SAFE1:	{ pkCont = &m_kRentalSafe1;}break;
	case IT_RENTAL_SAFE2:	{ pkCont = &m_kRentalSafe2;}break;
	case IT_RENTAL_SAFE3:	{ pkCont = &m_kRentalSafe3;}break;
	case IT_PET:		{ pkCont = &m_kPetItem;}break;
	case IT_SHARE_RENTAL_SAFE1:{ pkCont = &m_kShareRentalSafe1;}break;
	case IT_SHARE_RENTAL_SAFE2:{ pkCont = &m_kShareRentalSafe2;}break;
	case IT_SHARE_RENTAL_SAFE3:{ pkCont = &m_kShareRentalSafe3;}break;
	case IT_SHARE_RENTAL_SAFE4:{ pkCont = &m_kShareRentalSafe4;}break;
	case IT_SAFE_ADDITION:	{ pkCont = &m_kSafeAddInv;}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Warning!! Invalid CaseType"));
			return E_FAIL;
		}break;
	}
	return S_OK;
}

HRESULT PgInventory::SwapItem( SItemPos const &rPos, PgBase_Item &kItem, bool const bSameID )
{
	ItemContainer* pCont = NULL;
	if(S_OK == GetContainer( static_cast<EInvType>(rPos.x), pCont ) )
	{
		return pCont->SwapItem( rPos, kItem, bSameID );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItemID( SItemPos const &rPos, BM::GUID &kOutItemID )
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer((EInvType)rPos.x, pCont) )
	{
		return pCont->GetItemID(rPos, kOutItemID);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItem(SItemPos const &rPos, PgBase_Item &kOutItem)
{	
	ItemContainer* pCont = NULL;
	if(S_OK == GetContainer((EInvType)rPos.x, pCont))
	{
		return pCont->GetItem(rPos, kOutItem);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItem(EInvType const eInvType, BYTE const kPos, PgBase_Item &kOutItem)
{
	return GetItem(SItemPos(eInvType, kPos), kOutItem);
}

HRESULT PgInventory::GetItem(BM::GUID const &rGuid, PgBase_Item &kOutItem, bool const bIsForQuickSlot)
{
	SItemPos kPos;
	return GetItem(rGuid,kOutItem,kPos,bIsForQuickSlot);
}

HRESULT PgInventory::GetItem(BM::GUID const &rGuid, PgBase_Item &kOutItem, SItemPos & kOutPos, bool const bIsForQuickSlot)
{
	if (rGuid == BM::GUID::NullData())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	if (S_OK == GetItem(rGuid, kOutPos, &m_kEquipInv))			{ goto __SUCCESS; }
	if (S_OK == GetItem(rGuid, kOutPos, &m_kConsumeInv))		{ goto __SUCCESS; }
	if (S_OK == GetItem(rGuid, kOutPos, &m_kEtcInv))			{ goto __SUCCESS; }
	if (S_OK == GetItem(rGuid, kOutPos, &m_kCashInv))			{ goto __SUCCESS; }
	if (S_OK == GetItem(rGuid, kOutPos, &m_kHomeInv))			{ goto __SUCCESS; }
	if (S_OK == GetItem(rGuid, kOutPos, &m_kPetItem))			{ goto __SUCCESS; }
	
	if (!bIsForQuickSlot)
	{
		if (S_OK == GetItem(rGuid, kOutPos, &m_kSafeInv))		{ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kCashSafeInv))	{ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kRentalSafe1))	{ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kRentalSafe2))	{ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kRentalSafe3))	{ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kShareRentalSafe1)){ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kShareRentalSafe2)){ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kShareRentalSafe3)){ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kShareRentalSafe4)){ goto __SUCCESS; }
		if (S_OK == GetItem(rGuid, kOutPos, &m_kSafeAddInv))	{ goto __SUCCESS; }	
	}
	if ( S_OK == GetItem(rGuid, kOutPos, &m_kFit))				{ goto __SUCCESS; }
	if ( S_OK == GetItem(rGuid, kOutPos, &m_kFitCash))			{ goto __SUCCESS; }
	if ( S_OK == GetItem(rGuid, kOutPos, &m_kFitCostume))			{ goto __SUCCESS; }

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;

__SUCCESS:

	return GetItem(kOutPos, kOutItem);
}

HRESULT PgInventory::GetItems(EInvType const eInvType, CONT_ITEM_CREATE_ORDER &rOut, bool const bChkTimeOut)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer(eInvType, pCont) )
	{
		ItemContainer::iterator itor = pCont->begin();
		while(itor != pCont->end())
		{
			if(false == bChkTimeOut || false == (*itor).second.IsUseTimeOut())
			{
				rOut.push_back((*itor).second);
			}
			++itor;
		}

		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, CONT_HAVE_ITEM_DATA &rOut, bool const bChkTimeOut)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer(eInvType, pCont) )
	{
		ItemContainer::iterator itor = pCont->begin();
		while(itor != pCont->end())
		{
			if(false == bChkTimeOut || false == (*itor).second.IsUseTimeOut())
			{
				rOut.insert(std::make_pair((*itor).second.ItemNo(), (*itor).second));
			}
			++itor;
		}

		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, CONT_HAVE_ITEM_DATA_ALL &rOut, bool const bChkTimeOut)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer(eInvType, pCont) )
	{
		ItemContainer::iterator itor = pCont->begin();
		while(itor != pCont->end())
		{
			if(false == bChkTimeOut || false == (*itor).second.IsUseTimeOut())
			{
				rOut.push_back((*itor).second);
			}
			++itor;
		}

		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, CONT_HAVE_ITEM_NO &rOut, bool const bChkTimeOut)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer(eInvType, pCont) )
	{
		ItemContainer::iterator itor = pCont->begin();
		while(itor != pCont->end())
		{
			if(false == bChkTimeOut || false == (*itor).second.IsUseTimeOut())
			{
				rOut.insert((*itor).second.ItemNo());
			}
			++itor;
		}

		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, ContHaveItemNoCount &rkOut, bool const bChkTimeOut)
{
	ItemContainer* pkCont = NULL;
	if( S_OK != GetContainer(eInvType, pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ItemContainer::const_iterator item_iter = pkCont->begin();
	while(pkCont->end() != item_iter)
	{
		ItemContainer::mapped_type const &rkElement = (*item_iter).second;

		if(false == bChkTimeOut || false == rkElement.IsUseTimeOut())
		{
			auto kRet = rkOut.insert( std::make_pair(rkElement.ItemNo(), rkElement.Count()) );
			if( !kRet.second )
			{
				(*kRet.first).second += rkElement.Count();
			}
		}
		++item_iter;
	}

	return S_OK;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, CONT_HAVE_ITEM_POS &rOut, bool const bChkTimeOut)
{
	ItemContainer* pkCont = NULL;
	if( S_OK != GetContainer(eInvType, pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ItemContainer::const_iterator item_iter = pkCont->begin();
	while(pkCont->end() != item_iter)
	{
		ItemContainer::mapped_type const &rkElement = (*item_iter).second;

		if(false == bChkTimeOut || false == rkElement.IsUseTimeOut())
		{
			auto kRet = rOut.insert( std::make_pair(rkElement.ItemNo(), CONT_HAVE_ITEM_POS::mapped_type()) );
			(*kRet.first).second.iCount += (*item_iter).second.Count();
			(*kRet.first).second.kContItemPos.push_back( (*item_iter).first );
		}
		++item_iter;
	}

	return S_OK;
}

HRESULT PgInventory::GetItems(EUseItemCustomType const eCustomType, ContHaveItemNoCount& rkOut, bool const bChkTimeOut )
{
	for(int i = IT_EQUIP; i < IT_SAFE; ++i)
	{
		GetItems(static_cast<EInvType>(i), eCustomType, rkOut, bChkTimeOut);
	}
	return rkOut.empty() ? E_FAIL : S_OK;
}

HRESULT PgInventory::GetItems(EInvType const kInvType,EUseItemCustomType const eCustomType, ContHaveItemNoCount& rkOut, bool const bChkTimeOut )
{
	ItemContainer* pkCont = NULL;
	if( S_OK != GetContainer( kInvType, pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	ItemContainer::const_iterator item_iter = pkCont->begin();
	while( item_iter != pkCont->end() )
	{
		ItemContainer::mapped_type const& rkElement = item_iter->second;

		if ( !bChkTimeOut || !rkElement.IsUseTimeOut() )
		{
			CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkElement.ItemNo());
			if( pkItemDef )
			{
				int const UICT_Type = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if( eCustomType == UICT_Type )
				{
					auto kRet = rkOut.insert(std::make_pair(rkElement.ItemNo(), rkElement.Count()));
					if( !kRet.second )
					{
						(*kRet.first).second += rkElement.Count();
					}
				}
			}
		}
		++item_iter;
	}

	return rkOut.empty() ? E_FAIL : S_OK;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, EUseItemCustomType const eCustomType, CONT_HAVE_ITEM_POS &rOut, bool const bChkTimeOut )
{
	ItemContainer* pkCont = NULL;
	if( S_OK != GetContainer( eInvType, pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	ItemContainer::const_iterator item_iter = pkCont->begin();
	while( item_iter != pkCont->end() )
	{
		ItemContainer::mapped_type const& rkElement = item_iter->second;

		if ( !bChkTimeOut || !rkElement.IsUseTimeOut() )
		{
			CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkElement.ItemNo());
			if( pkItemDef )
			{
				int const UICT_Type = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if( eCustomType == UICT_Type )
				{
					CONT_HAVE_ITEM_POS::iterator pos_iter = rOut.find(rkElement.ItemNo());
					
					if( rOut.end() != pos_iter )
					{//같은 아이템인데 다른 위치에 있음.
						pos_iter->second.iCount += rkElement.Count();//수량 추가
						pos_iter->second.kContItemPos.push_back( (*item_iter).first );//위치 추가
					}
					else
					{//새로운 아이템
						CONT_HAVE_ITEM_POS::mapped_type kPosElement;
						kPosElement.iCount += rkElement.Count();
						kPosElement.kContItemPos.push_back( (*item_iter).first );//위치 추가
						rOut.insert(std::make_pair( rkElement.ItemNo(), kPosElement));
					}
				}
			}
		}
		++item_iter;
	}

	return rOut.empty() ? E_FAIL : S_OK;
}

HRESULT PgInventory::GetItems(EInvType const eInvType, EUseItemCustomType const eCustomType, CONT_HAVE_ITEM_DATA &rOut, bool const bChkTimeOut)
{
	ItemContainer* pkCont = NULL;
	if( S_OK != GetContainer( eInvType, pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);

	ItemContainer::const_iterator item_iter = pkCont->begin();
	while( item_iter != pkCont->end() )
	{
		ItemContainer::mapped_type const& rkElement = item_iter->second;

		if ( !bChkTimeOut || !rkElement.IsUseTimeOut() )
		{
			CItemDef const *pkItemDef = kItemDefMgr.GetDef(rkElement.ItemNo());
			if( pkItemDef )
			{
				int const UICT_Type = pkItemDef->GetAbil(AT_USE_ITEM_CUSTOM_TYPE);
				if( eCustomType == UICT_Type )
				{
					rOut.insert(std::make_pair((*item_iter).second.ItemNo(), (*item_iter).second));
				}
			}
		}
		++item_iter;
	}

	return rOut.empty() ? E_FAIL : S_OK;
}

bool PgInventory::Modify(SItemPos const &kPos, PgBase_Item const &kItem)
{
	ItemContainer *pCont = NULL;
	
	if(S_OK == GetContainer((EInvType)kPos.x, pCont))
	{
		ItemContainer::iterator itor = pCont->find(kPos);
		if(itor != pCont->end())
		{
			if(!PgBase_Item::IsEmpty(&kItem))
			{
				itor->second.SetItem(kItem);
			}
			else
			{
				pCont->erase(itor);
			}
			return true;
		}
		else
		{
			if(kItem.ItemNo())
			{
				auto ret = pCont->insert(std::make_pair(kPos, PgItemWrapper(kItem, kPos)));
				if( !ret.second )
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				}
				return ret.second;
			}
			return true;
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgInventory::RefreshAbil(CUnit const * pkUnit)
{
	if ( !pkUnit )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Unit is NULL!!");
		return E_FAIL;
	}

	CAbilObject::Clear();//싹 밀고.

	m_kFullSetItem.clear();

	ItemContainer* pkCont = NULL,* pkContCash = NULL;
	CAbilObject* pkAbilObj = dynamic_cast<CAbilObject*>(this); 

	m_kOverSkill.clear();
	m_kItemActionEffect.clear();
	m_kAmplyOptionValue.Clear();

	if( S_OK == GetContainer(IT_FIT, pkCont) )
	{
		HRESULT const hRet1 = PgInventoryUtil::ApplyAbil(pkAbilObj, pkCont, m_kFullSetItem, m_kOverSkill, m_kItemActionEffect, pkUnit, &m_kAmplyOptionValue);
	}

	if( S_OK == GetContainer(IT_FIT_CASH, pkContCash) )
	{
		HRESULT const hRet2 = PgInventoryUtil::ApplyAbil(pkAbilObj, pkContCash, m_kFullSetItem, m_kOverSkill, m_kItemActionEffect, pkUnit, &m_kAmplyOptionValue);
	}
	return S_OK;
}


size_t PgInventory::GetTotalCount(int const iItemNo, bool const bIsForQuickSlot)const
{
	if(!iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	size_t ret_count = 0;
	ret_count += GetTotalCount(iItemNo, &m_kEquipInv);
	ret_count += GetTotalCount(iItemNo, &m_kConsumeInv);
	ret_count += GetTotalCount(iItemNo, &m_kEtcInv);
	ret_count += GetTotalCount(iItemNo, &m_kCashInv);
	ret_count += GetTotalCount(iItemNo, &m_kHomeInv);
	ret_count += GetTotalCount(iItemNo, &m_kPetItem);
	
	if(!bIsForQuickSlot)
	{
		ret_count += GetTotalCount(iItemNo, &m_kSafeInv);
		ret_count += GetTotalCount(iItemNo, &m_kCashSafeInv);
		ret_count += GetTotalCount(iItemNo, &m_kRentalSafe1);
		ret_count += GetTotalCount(iItemNo, &m_kRentalSafe2);
		ret_count += GetTotalCount(iItemNo, &m_kRentalSafe3);
		ret_count += GetTotalCount(iItemNo, &m_kShareRentalSafe1);
		ret_count += GetTotalCount(iItemNo, &m_kShareRentalSafe2);
		ret_count += GetTotalCount(iItemNo, &m_kShareRentalSafe3);
		ret_count += GetTotalCount(iItemNo, &m_kShareRentalSafe4);
		ret_count += GetTotalCount(iItemNo, &m_kSafeAddInv);
	}
	ret_count += GetTotalCount(iItemNo, &m_kFit);
	ret_count += GetTotalCount(iItemNo, &m_kFitCash);
	ret_count += GetTotalCount(iItemNo, &m_kFitCostume);
	return ret_count;
}

size_t PgInventory::GetInvTotalCount(int const iItemNo)const
{
	if(!iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	size_t ret_count = 0;
	ret_count += GetTotalCount(iItemNo, &m_kEquipInv);
	ret_count += GetTotalCount(iItemNo, &m_kConsumeInv);
	ret_count += GetTotalCount(iItemNo, &m_kEtcInv);
	ret_count += GetTotalCount(iItemNo, &m_kCashInv);
	ret_count += GetTotalCount(iItemNo, &m_kHomeInv);
	ret_count += GetTotalCount(iItemNo, &m_kPetItem);

	return ret_count;
}

size_t PgInventory::GetTotalCount(int const iItemNo, ItemContainer const *pkCont)const
{
	if(!iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	const CItemDef* pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( !pkItemDef )
	{
		return 0;
	}

	size_t ret_count = 0;
	ItemContainer::const_iterator item_itor = pkCont->begin();
	while(item_itor != pkCont->end())
	{
		if((*item_itor).second.ItemNo() == iItemNo)
		{
			if( pkItemDef->IsAmountItem() )
			{// 겹치는 아이템
				ret_count += (*item_itor).second.Count();
			}
			else
			{
				++ret_count;
			}	
		}

		++item_itor;
	}

	return ret_count;
}

HRESULT PgInventory::GetFirstItem(int const iItemNo, SItemPos &rkRetPos, bool const bIsForQuickSlot, bool const bChkTimeOut )
{
	if(!iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kEquipInv, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kConsumeInv, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kEtcInv, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kCashInv, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kHomeInv, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kPetItem, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kSafeAddInv, bChkTimeOut)){return S_OK;}
	if(!bIsForQuickSlot)
	{
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kSafeInv, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kCashSafeInv, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kRentalSafe1, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kRentalSafe2, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kRentalSafe3, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kShareRentalSafe1, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kShareRentalSafe2, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kShareRentalSafe3, bChkTimeOut)){return S_OK;}
		if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kShareRentalSafe4, bChkTimeOut)){return S_OK;}
	}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kFit, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kFitCash, bChkTimeOut)){return S_OK;}
	if(S_OK == GetFirstItem(iItemNo, rkRetPos, &m_kFitCostume, bChkTimeOut)) { return S_OK; };

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetNextItem(int const iItemNo, SItemPos &rkRetPos)
{
	BYTE byMaxSize = GetMaxIDX((EInvType)rkRetPos.x);
	++rkRetPos.y;
	while (byMaxSize > rkRetPos.y)
	{
		PgBase_Item kItem;
		if(S_OK == GetItem(rkRetPos, kItem))
		{
			if(kItem.ItemNo() == iItemNo)
			{
				return S_OK;
			}
		}
		++rkRetPos.y;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItemModifyOrder( EInvType const eInvType, int const iItemNo, size_t iSize, CONT_PLAYER_MODIFY_ORDER *pkOrder )
{
	if ( !iItemNo || !iSize )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ItemContainer *pkCont = NULL;
	if ( E_FAIL == GetContainer(eInvType,pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	size_t iPushCount = 0;
	ItemContainer::const_iterator item_itor = pkCont->begin();
	while( item_itor != pkCont->end() && iSize )
	{
		if( (*item_itor).second.ItemNo() == iItemNo )
		{
			int iModifyCount = 0;
			if( iSize <= (size_t)(item_itor->second.Count()) )
			{
				iModifyCount = (int)iSize;
				iSize = 0;
			}
			else
			{
				iModifyCount = (int)(item_itor->second.Count());
				iSize -= (size_t)iModifyCount;
			}

			if ( pkOrder )
			{
				SPMOD_Modify_Count kModifyData( item_itor->second, item_itor->first, -iModifyCount );
				SPMO kIMO( IMET_MODIFY_COUNT, OwnerGuid(), kModifyData );
				pkOrder->push_back(kIMO);
				++iPushCount;
			}
		}
		++item_itor;
	}

	if ( iSize )
	{
		if ( pkOrder )
		{
			while ( iPushCount-- )
			{
				pkOrder->pop_back();
			}
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT PgInventory::GetFirstItem(EInvType const eInvType, int const iItemNo, SItemPos& rkRetPos, bool const bChkTimeOut )
{
	ItemContainer *pkCont = NULL;
	if ( E_FAIL == GetContainer(eInvType,pkCont) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	return GetFirstItem(iItemNo, rkRetPos, pkCont, bChkTimeOut);
}

HRESULT PgInventory::GetFirstItem(int const iItemNo, SItemPos &rkRetPos, ItemContainer const *pkCont, bool const bChkTimeOut )
{
	if(!iItemNo)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	size_t ret_count = 0;
	ItemContainer::const_iterator item_itor = pkCont->begin();
	while(item_itor != pkCont->end())
	{
		if((*item_itor).second.ItemNo() == iItemNo)
		{
			if ( !bChkTimeOut || !item_itor->second.IsUseTimeOut() )
			{
				rkRetPos = (*item_itor).first;
				return S_OK;
			}
		}
		++item_itor;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::GetItem(BM::GUID const &rGuid, SItemPos &rkRetPos, ItemContainer const *pkCont)
{
	if (rGuid == BM::GUID::NullData())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	ItemContainer::const_iterator item_itor = pkCont->begin();
	while(item_itor != pkCont->end())
	{
		PgBase_Item const &rkItem = (*item_itor).second;
		if (rGuid == rkItem.Guid())
		{
			rkRetPos = (*item_itor).first;
			return S_OK;
		}
		
		++item_itor;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgInventory::GetFirstEmptyPos(EInvType eInvType, SItemPos& rkOutPos)
{
	ItemContainer *pCont = NULL;
	if(S_OK != GetContainer(eInvType, pCont))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	int iMaxIdx = (int) GetMaxIDX(eInvType);
	for (int i=0; i<iMaxIdx; ++i)
	{
		SItemPos kFind((BYTE)eInvType, (BYTE)i);
		ItemContainer::iterator itor = pCont->find(kFind);
		if (itor == pCont->end())
		{
			rkOutPos = kFind;
			return true;
		}
		else if (PgBase_Item::IsEmpty(&(itor->second)))
		{
			rkOutPos = kFind;
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgInventory::GetNextEmptyPos(EInvType eInvType, SItemPos& rkOutPos)
{
	ItemContainer* pCont = NULL;
	if(S_OK != GetContainer(eInvType, pCont))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	int iMaxIdx = (int) GetMaxIDX(eInvType);
	for (int i=rkOutPos.y; i<iMaxIdx; ++i)
	{
		SItemPos kFind((BYTE)eInvType, (BYTE)i);
		ItemContainer::iterator itor = pCont->find(kFind);
		if (itor == pCont->end())
		{
			rkOutPos = kFind;
			return true;
		}
		else if (PgBase_Item::IsEmpty(&(itor->second)))
		{
			rkOutPos = kFind;
			return true;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

int const PgInventory::GetMaxIDX(EInvType eInvType)
{
	PgBaseItemContainer * pkCont;
	if(S_OK != GetContainer(eInvType,pkCont))
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Unknown Inven Type request InvenType[") << eInvType << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return MAX_NONE_INVEN_IDX"));
		return MAX_NONE_INVEN_IDX;
	}
	return pkCont->GetMaxIDX();
}

// 비어있는 슬롯의 개수 얻기
int const PgInventory::GetEmptyPosCount(EInvType eInvType)
{
	PgBaseItemContainer * pkCont;
	if(S_OK != GetContainer(eInvType,pkCont))
	{
		INFO_LOG( BM::LOG_LV5, __FL__ << _T(" Unknown Inven Type request InvenType[") << eInvType << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return MAX_NONE_INVEN_IDX"));
		return MAX_NONE_INVEN_IDX;
	}

	int iResultCount = 0;

	int iMaxIdx = (int)GetMaxIDX(eInvType);
	for( int i = 0; i < iMaxIdx; ++i )
	{
		SItemPos kFind((BYTE)eInvType, (BYTE)i);
		ItemContainer::iterator iter = pkCont->find(kFind);
		if( iter == pkCont->end() )
		{
			++iResultCount; // 비어있는 슬롯 개수
		}
	}

	return iResultCount;
}

HRESULT PgInventory::CanInsert(PgBase_Item const &kItem)
{
	int const iItemNo = kItem.ItemNo();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	
	if(pkItemDef)
	{
		int const iPriInvType = pkItemDef->PrimaryInvType();
		
		ItemContainer *pCont = NULL;
		if(S_OK == GetContainer((EInvType)iPriInvType, pCont))
		{
			SItemPos kItemPos;
			if(GetFirstEmptyPos((EInvType)iPriInvType, kItemPos))
			{
				return S_OK;
			}

			if(pkItemDef->IsAmountItem())
			{
				size_t const iMaxAmount = pkItemDef->MaxAmount();
				int iInAmount = kItem.Count();
		
				ItemContainer::iterator itor = pCont->begin();
				while(itor != pCont->end())
				{
					if((*itor).second.ItemNo() == kItem.ItemNo())
					{
						size_t const iExtraCount = iMaxAmount - (*itor).second.Count();
						
						if(iExtraCount)//빈공간 있음.
						{
							if(kItem.Count() <= iExtraCount)
							{//빈공간이 더 많구만. 
								return S_OK;
							}
							else
							{//빈공간 모자람.
								iInAmount -= (int)iExtraCount;
							}
						}
					}
					++itor;
				}
			
				if(!iInAmount)
				{
					return S_OK;
				}
			}			
		}
		
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgInventory::CheckCoolTime(int const iItemNo, EItemCoolTimeType eType, DWORD const dwCurrentTime) const
{
	if (iItemNo == 0)
	{
		return true;
	}

	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( !pkItemDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot get ItemDef[") << iItemNo << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ItemDef Failed!"));
		return false;
	}

	CONT_ITEM_COOLTIME const * pkContCoolTime = NULL;
	int iCoolTimeKey = 0;

	int const iItemGroup = pkItemDef->GetAbil(AT_USE_ITEM_GROUP);
	if(0 == iItemGroup)
	{
		iCoolTimeKey = iItemNo;
		pkContCoolTime = &m_kItemCoolTime;
	}
	else
	{
		iCoolTimeKey = iItemGroup;
		pkContCoolTime = &m_kGroupCoolTime;
	}

	CONT_ITEM_COOLTIME::const_iterator itor = pkContCoolTime->find(iCoolTimeKey);
	if (itor != pkContCoolTime->end() && ((eType & itor->second.byCoolTimeType) != 0))
	{
		if (dwCurrentTime < itor->second.dwEndCoolTime)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	return true;
}

bool PgInventory::GetCoolTime(int const iItemNo,PgInventory::SItemCoolTimeInfo & kCoolTime) const
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if((iItemNo == 0) || !pkItemDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot get ItemDef[") << iItemNo << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ItemDef Failed!"));
		return false;
	}

	CONT_ITEM_COOLTIME const * pkContCoolTime = NULL;
	int iCoolTimeKey = 0;

	int const iItemGroup = pkItemDef->GetAbil(AT_USE_ITEM_GROUP);
	if(0 == iItemGroup)
	{
		iCoolTimeKey = iItemNo;
		pkContCoolTime = &m_kItemCoolTime;
	}
	else
	{
		iCoolTimeKey = iItemGroup;
		pkContCoolTime = &m_kGroupCoolTime;
	}

	CONT_ITEM_COOLTIME::const_iterator itor = pkContCoolTime->find(iCoolTimeKey);
	if (itor != pkContCoolTime->end())
	{
		kCoolTime = (*itor).second;
		return true;
	}

	return false;
}

void PgInventory::AddCoolTime(int const iItemNo, EItemCoolTimeType const eType, DWORD const dwCurrentTime,int const iCoolTimeRate)
{
	if (iItemNo == 0)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("iItemNo is 0"));
		return;
	}
	//INFO_LOG(BM::LOG_LV9, _T("[%s] CoolTimeType[%d], ItemNo[%d]"), __FUNCTIONW__, eType, iItemNo);
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pkItemDef = kItemDefMgr.GetDef(iItemNo);
	if( !pkItemDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV5, __FL__ << _T(" Cannot get ItemDef[") << iItemNo << _T("]") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Get ItemDef Failed!"));
		return;
	}

	int const iCoolTime = pkItemDef->GetAbil(ATS_COOL_TIME);	// 아이템에 쿨타임이 없으면 쓸데 없이 코드 낭비 하지 말자...
	if(0 >= iCoolTime)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("CoolTime is 0"));
		return;
	}
	
	CONT_ITEM_COOLTIME * pkContCoolTime = NULL;
	int iCoolTimeKey = 0;

	int const iItemGroup = pkItemDef->GetAbil(AT_USE_ITEM_GROUP);
	if(0 == iItemGroup)
	{
		iCoolTimeKey = iItemNo;
		pkContCoolTime = &m_kItemCoolTime;
	}
	else
	{
		iCoolTimeKey = iItemGroup;
		pkContCoolTime = &m_kGroupCoolTime;
	}

	int iCoolTimeType = pkItemDef->GetAbil(AT_COOLTIME_TYPE);
	if ((iCoolTimeType & eType) != 0)
	{
		int const iDiffCoolTime = static_cast<int>(pkItemDef->GetAbil(ATS_COOL_TIME) * (static_cast<double>(iCoolTimeRate)/static_cast<double>(ABILITY_RATE_VALUE)));
		int const iCoolTime = std::max(0,pkItemDef->GetAbil(ATS_COOL_TIME) - iDiffCoolTime);// 0 이하는 되면 안된다.
		int const iSaveDB = pkItemDef->GetAbil(AT_SAVE_DB);

		// CoolTime 적용 확정
		CONT_ITEM_COOLTIME::iterator itor = pkContCoolTime->find(iCoolTimeKey);

		if (itor == pkContCoolTime->end())
		{
			SItemCoolTimeInfo sCoolTimeInfo;
			sCoolTimeInfo.byCoolTimeType = (BYTE) iCoolTimeType;
			sCoolTimeInfo.dwEndCoolTime = dwCurrentTime + iCoolTime;
			sCoolTimeInfo.dwTotalCoolTime = iCoolTime;
			sCoolTimeInfo.bSaveDB = (0 != iSaveDB && 0 != iItemGroup); // 그룹만 세이브 가능
			pkContCoolTime->insert(std::make_pair(iCoolTimeKey, sCoolTimeInfo));
		}
		else
		{
			itor->second.dwEndCoolTime = dwCurrentTime + iCoolTime;
		}
	}
}

void PgInventory::ClearCoolTime()
{
	m_kItemCoolTime.clear();
	m_kGroupCoolTime.clear();
}

void PgInventory::ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate)
{
	ReCalcCoolTime(dwCurrentTime,iCoolTimeRate,&m_kItemCoolTime);
	ReCalcCoolTime(dwCurrentTime,iCoolTimeRate,&m_kGroupCoolTime);
}

void PgInventory::ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate,CONT_ITEM_COOLTIME * pkCont)
{
	for(CONT_ITEM_COOLTIME::iterator iter = pkCont->begin();iter != pkCont->end();++iter)
	{
		if(dwCurrentTime < (*iter).second.dwEndCoolTime)
		{
			int const iCoolTime = (*iter).second.dwEndCoolTime - dwCurrentTime;
			int const iDiffCoolTime = static_cast<int>(iCoolTime * (static_cast<double>(iCoolTimeRate)/static_cast<double>(ABILITY_RATE_VALUE)));
			(*iter).second.dwEndCoolTime = dwCurrentTime + std::max(0,(iCoolTime - iDiffCoolTime));
		}
	}
}

EPropertyType PgInventory::ItemAttr(EEquipPos const &ePos) const//장착된 부위에서 속성을 뽑아냄.
{
	SItemPos const kPos(IT_FIT, ePos);

	ItemContainer *pCont = NULL;
	if(S_OK == ((PgInventory*)this)->GetContainer((EInvType)kPos.x, pCont))
	{
		ItemContainer::iterator itor = pCont->find(kPos);
		if(itor != pCont->end())
		{
			return static_cast<EPropertyType>((*itor).second.EnchantInfo().Attr());
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_PPTY_NONE"));
	return E_PPTY_NONE;
}

int PgInventory::ItemAttrLevel(EEquipPos const &ePos) const//장착된 부위에서 속성을 뽑아냄.
{
	
	SItemPos const kPos(IT_FIT, ePos);

	ItemContainer *pCont = NULL;
	if(S_OK == ((PgInventory*)this)->GetContainer((EInvType)kPos.x, pCont))
	{
		ItemContainer::iterator itor = pCont->find(kPos);
		if(itor != pCont->end())
		{
			return static_cast<int>((*itor).second.EnchantInfo().AttrLv());
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
	return 0;
}

int PgInventory::GetAbil(WORD const wAbilType) const
{
	int iValue = 0;
	switch(wAbilType)
	{
	case AT_OFFENSE_PROPERTY:
		{
			EPropertyType eAttr = ItemAttr(EQUIP_POS_WEAPON);
			if (eAttr >= E_PPTY_BASIC_MAX)
			{
				iValue = GetAbil(AT_OFFENSE_PROPERTY_EXPAND);
			}
			else
			{
				iValue = eAttr;
			}
		}break;
	case AT_OFFENSE_PROPERTY_LEVEL:
		{
			iValue = ItemAttrLevel(EQUIP_POS_WEAPON);
		}break;
	case AT_DEFENCE_PROPERTY:
		{
			EPropertyType eAttr = ItemAttr(EQUIP_POS_SHIRTS);
			if (eAttr >= E_PPTY_BASIC_MAX)
			{
				iValue = GetAbil(AT_DEFENCE_PROPERTY_EXPAND);
			}
			else
			{
				iValue = eAttr;
			}
		}break;
	case AT_DEFENCE_PROPERTY_LEVEL:
		{
			iValue = ItemAttrLevel(EQUIP_POS_SHIRTS);
		}break;
	default:
		{
			iValue = CAbilObject::GetAbil(wAbilType);
		}break;
	}
	return iValue;
}
int PgInventory::GetRemovedAmplyAbil(WORD const wAbilType) const
{// 옵션 증폭이 제거된 값을 얻으려면
	int iValue = GetAbil(wAbilType);					// 이미계산된 증폭 값을 얻어와
	if( true == PgInventoryUtil::FilterPercentAbil( wAbilType ) )
	{	// FilterPercentAbil에서 true를 반환하는 AbilType은 미리 Rate값이 계산되어 있는 상태이기 때문에.
		// 다른 AbilType과는 계산하는 방법이 다름. - 자세한 내용은 commit log 참조. 2011.10.12
		// 옵션 증폭 적용 전 값.
		switch( wAbilType )
		{
		case AT_R_PHY_DEFENCE: 
			{
				iValue = GetAbil( ::GetAmpliAbil(AT_I_PHY_DEFENCE_ADD_RATE) );
			} break;
		case AT_R_MAGIC_DEFENCE:
			{
				iValue = GetAbil( ::GetAmpliAbil(AT_I_MAGIC_DEFENCE_ADD_RATE) );
			} break;
		default:
			{
				iValue = GetAbil( ::GetAmpliAbil(wAbilType) );
			} break;
		}
	}
	else
	{
		iValue -= m_kAmplyOptionValue.GetAbil(wAbilType);	// 증폭되었을때 저장해둔 값을 제거하고
	}
	return iValue;										// 반환한다
}

HRESULT PgInventory::MergeItem(PgItemWrapper &kCasterWrapper, PgItemWrapper &kTargetWrapper, __int64 & iCause)
{//실제 머징이 일어났을때만 S_OK 리턴
	if(	kCasterWrapper.IsEmpty()
	&&	kTargetWrapper.IsEmpty())
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	int const iCasterItemNo = kCasterWrapper.ItemNo();
	int const iTargetItemNo = kTargetWrapper.ItemNo();
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(iCasterItemNo);

	HRESULT hReturn = E_INCORRECT_ITEM;
	if(	pItemDef
	&&	iCasterItemNo == iTargetItemNo
	&&	pItemDef->IsAmountItem()
	&&	kCasterWrapper.EnchantInfo() == kTargetWrapper.EnchantInfo() )//속성이 같아야 함.
	{
		int const iMaxAmount = (int)pItemDef->MaxAmount();

		PgBase_Item kCasterItem = kCasterWrapper;
		PgBase_Item kTargetItem = kTargetWrapper;

		int const iRemain = iMaxAmount - kCasterItem.Count();//

		if(iRemain == 0)
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
			return E_INCORRECT_ITEM;
		}
		else if(iRemain > 0)
		{//남은 갯수.
			int const iTargetCount = kTargetItem.Count();
			int const iCasterCount = kCasterItem.Count();
			if(iTargetCount > iRemain)
			{//캐스터가 많으면
				kTargetItem.Count(iTargetCount-iRemain);
				kCasterItem.Count(iCasterCount+iRemain);
			}
			else
			{//캐스터가 지워져야지.
				kTargetItem.Count(0);
				kCasterItem.Count(iCasterCount+iTargetCount);//캐스터 카운트 만큼 추가.
			}

			kTargetWrapper.SetItem(kTargetItem);
			kCasterWrapper.SetItem(kCasterItem);

			// Merge되었으면, iCause에 표시해 주면
			// OverWriteItem 내부에서 없어진 아이템을 실제로 없애 준다.
			iCause |= IMD_MERGED_SUCCESS;
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
	return hReturn;
}


HRESULT PgInventory::AddItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(kData.ItemNo());
	if(!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	ItemContainer *pkCont = NULL;
	if(S_OK != GetContainer( (EInvType)pItemDef->PrimaryInvType(), pkCont))//Primary 인벤에 안있었다면 아이템 수정이 안되는것이다/
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTORY"));
		return E_INVALID_INVENTORY;
	}

	int iModifyCount = 0;
	CONT_MODIFY_COUNT kContModifyCount;

	if(!pkCont->EmulateModifyCount(iCause, kData.ItemNo(), kData.AddCount(),kContModifyCount,iModifyCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	for(CONT_MODIFY_COUNT::iterator modify_iter = kContModifyCount.begin();modify_iter != kContModifyCount.end();++modify_iter)
	{
		pkCont->ModifyCount(iCause, (*modify_iter), kChangeArray,kContLogMgr);
	}

	if(iModifyCount == kData.AddCount())	// 겹치는 루틴에서 모든 아이템이 다 입력 되었다면 여기서 리턴 하자 
	{
		return S_OK;
	}

	// 여기 부터는 남아 있는 아이템을 인벤토리에 넣는 작업을 하는것.....

	int iGenItemCount = kData.AddCount() - iModifyCount;
	int iNeedInvCount = 0;
	int iInsertItemCount = 0;
	
	if(pItemDef->IsAmountItem())
	{
		iNeedInvCount = (iGenItemCount + ((int)pItemDef->MaxAmount() - 1))/(int)pItemDef->MaxAmount();
		iInsertItemCount = (int)pItemDef->MaxAmount();
	}
	else
	{
		iNeedInvCount = iGenItemCount;
		iInsertItemCount = 1;
	}

	CONT_EMPTY_ITEM_POS kContEmptyItemPos;
	if(!pkCont->EnumEmptyInvPos(kContEmptyItemPos,iNeedInvCount))
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_ENOUGH_INV_SPACE"));
		return E_NOT_ENOUGH_INV_SPACE;
	}

	for(CONT_EMPTY_ITEM_POS::iterator insert_iter = kContEmptyItemPos.begin();insert_iter != kContEmptyItemPos.end();++insert_iter)
	{
		if(pItemDef->IsAmountItem())
		{
			int iTmpInsertCount = __min(iGenItemCount,(int)pItemDef->MaxAmount());

			PgBase_Item kItem;
			if(S_OK == CreateSItem(kData.ItemNo(),iTmpInsertCount,GIOT_NONE, kItem))
			{
				PgItemWrapper kCasterWrapper(kItem, SItemPos());
				if(S_OK != pkCont->InsertItem(iCause, kCasterWrapper, true, kChangeArray,kContLogMgr))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
					return E_INCORRECT_ITEM;
				}
			}

			iGenItemCount -= iTmpInsertCount;
		}
		else
		{
			PgBase_Item kItem;
			if(S_OK == CreateSItem(kData.ItemNo(),pItemDef->GetAbil(AT_DEFAULT_DURATION),GIOT_NONE, kItem))
			{
				PgItemWrapper kCasterWrapper(kItem, SItemPos());
				if(S_OK != pkCont->InsertItem(iCause, kCasterWrapper, true, kChangeArray,kContLogMgr))
				{
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
					return E_INCORRECT_ITEM;
				}
			}

			--iGenItemCount;
		}
	}

	return S_OK;
}

HRESULT PgInventory::RemoveItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(kData.ItemNo());
	if(!pItemDef)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INCORRECT_ITEM"));
		return E_INCORRECT_ITEM;
	}

	int iModifyCount = 0;
	int iRemainCount = kData.AddCount();
	int iTotalModifyCount = 0;
	CONT_MODIFY_COUNT kContModifyCount;
	ItemContainer *pkCont = NULL;

	EInvType eFindInvType[] = {IT_EQUIP, IT_CONSUME, IT_ETC, IT_CASH, IT_SAFE, IT_CASH_SAFE, IT_FIT, IT_FIT_CASH, IT_FIT_COSTUME, IT_SAFE_ADDITION,
		IT_RENTAL_SAFE1, IT_RENTAL_SAFE2, IT_RENTAL_SAFE3, IT_NONE};
	int index = 0;
	while (eFindInvType[index] != 0)
	{
		if(S_OK == GetContainer(eFindInvType[index], pkCont))
		{
			if(pkCont->EmulateModifyCount(iCause, kData.ItemNo(), iRemainCount,kContModifyCount,iModifyCount,false))
			{
				iRemainCount -= iModifyCount;
				iTotalModifyCount += iModifyCount;
				if(iRemainCount == 0)
				{
					break;
				}
			}
		}
		++index;
	}

	if(iTotalModifyCount != kData.AddCount())	// 겹치는 루틴에서 모든 아이템이 다 입력 되었다면 여기서 리턴 하자 
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_ENOUGH_ITEM"));
		return E_NOT_ENOUGH_ITEM;
	}

	for(CONT_MODIFY_COUNT::iterator modify_iter = kContModifyCount.begin();modify_iter != kContModifyCount.end();++modify_iter)
	{
		if(S_OK == GetContainer(static_cast<EInvType>((*modify_iter).Pos().x), pkCont))
		{
			pkCont->ModifyCount(iCause, (*modify_iter), kChangeArray,kContLogMgr);
		}
	}

	return S_OK;
}

HRESULT PgInventory::ModifyItemAny(__int64 const iCause, SPMOD_Add_Any const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	if(kData.AddCount() > 0)	// 추가 동작은 해당 아이템이 들어갈수 있는 인벤에만 추가된다.
	{
		return AddItemAny(iCause,kData,kChangeArray,kContLogMgr);
	}
	
	return RemoveItemAny(iCause, kData, kChangeArray,kContLogMgr);
}

HRESULT PgInventory::ModifyCount(__int64 const iCause, SPMOD_Modify_Count const &kData, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{
	HRESULT hReturn = E_INCORRECT_ITEM;
	GET_DEF(CItemDefMgr, kItemDefMgr);
	CItemDef const *pItemDef = kItemDefMgr.GetDef(kData.Item().ItemNo());
	if(pItemDef)
	{
		ItemContainer *pkCont = NULL;
		//if(S_OK == GetContainer( (EInvType)pItemDef->PrimaryInvType(), pkCont))//Primary 인벤에 안있었다면 아이템 수정이 안되는것이다/
		if(S_OK == GetContainer( (EInvType)kData.Pos().x, pkCont))
		{
			return pkCont->ModifyCount(iCause, kData, kChangeArray,kContLogMgr);
		}
		hReturn = E_INVALID_INVENTYPE;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return ") << hReturn);
	return hReturn;
}

HRESULT PgInventory::SetItem(PgBase_Item const & kItem, SItemPos const & kPos)
{
	int BaseContainerPos = kPos.x;
	ItemContainer *pkCont = NULL;
	if(S_OK == GetContainer( (EInvType)BaseContainerPos, pkCont))
	{
		return pkCont->SetItem(kItem,kPos);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTYPE"));
	return E_INVALID_INVENTYPE;
}

HRESULT PgInventory::InsertItem(PgBase_Item const & kItem, SItemPos const & kPos)
{
	int BaseContainerPos = kPos.x;
	ItemContainer *pkCont = NULL;
	if(S_OK == GetContainer( (EInvType)BaseContainerPos, pkCont))
	{
		HRESULT const hRet = pkCont->InsertItem( kItem, kPos );
		return hRet;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTYPE"));
	return E_INVALID_INVENTYPE;
}

HRESULT PgInventory::AddMoney(const  __int64 kValue, bool const bIsTest)
{
	if(0 == kValue)
	{
		return S_OK;
	}

	if(0 > kValue)
	{//빼기
		if(0 <= m_kMoney + kValue)
		{//더해봐도 0 넘어.
			if(!bIsTest)
			{
				m_kMoney += kValue;
			}
			return S_OK;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_ENOUGH_MONEY"));
		return E_NOT_ENOUGH_MONEY;
	}
	else
	{
		if(!bIsTest)
		{
			m_kMoney += kValue;
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::AddCP(const int kValue, bool const bIsTest)
{
	if(0 == kValue)
	{
		return S_OK;
	}

	if(0 > kValue)
	{//빼기
		if(0 <= m_kCP + kValue)
		{//더해봐도 0 넘어.
			if(!bIsTest)
			{
				m_kCP += kValue;
			}
			return S_OK;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_NOT_ENOUGH_CP"));
		return E_NOT_ENOUGH_CP;
	}
	else
	{
		if(!bIsTest)
		{
			m_kCP += kValue;
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgInventory::OverWriteItem(__int64 const iCause, PgItemWrapper const &kItemWrapper, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//자리만 맞으면 Null 인데 Pop이 성공했다고 하기도 한다.
	PgBaseItemContainer *pkCont = NULL;
	if(S_OK == GetContainer((EInvType)kItemWrapper.Pos().x, pkCont))
	{
		return pkCont->OverWriteItem(iCause, kItemWrapper, kChangeArray,kContLogMgr);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTYPE"));
	return E_INVALID_INVENTYPE;
}

HRESULT PgInventory::PopItem(__int64 const iCause, SItemPos const &kPos, PgItemWrapper &rkOutItem, bool const bIsPop, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)
{//자리만 맞으면 Null 인데 Pop이 성공했다고 하기도 한다.
	PgBaseItemContainer *pkCont = NULL;
	if(S_OK == GetContainer((EInvType)kPos.x, pkCont))
	{
		return pkCont->PopItem(iCause, kPos, rkOutItem, bIsPop, kChangeArray,kContLogMgr);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_ITEMPOS"));
	return E_INVALID_ITEMPOS;
}

HRESULT PgInventory::InsertItem(__int64 const iCause, PgItemWrapper &kItemWrapper, bool const bToEmptyPos, bool const bIsTest, DB_ITEM_STATE_CHANGE_ARRAY &kChangeArray,PgContLogMgr & kContLogMgr)//Wrapped가 있는상태
{
	int BaseContainerPos = kItemWrapper.Pos().x;
	if(bToEmptyPos)
	{
		GET_DEF(CItemDefMgr, kItemDefMgr);
		CItemDef const *pItemDef = kItemDefMgr.GetDef(kItemWrapper.ItemNo());
		
		if( pItemDef )
		{
			BaseContainerPos = pItemDef->PrimaryInvType();
		}
	}

	PgBaseItemContainer *pkCont = NULL;
	if(S_OK == GetContainer( (EInvType)BaseContainerPos, pkCont))
	{
		return pkCont->InsertItem(iCause, kItemWrapper, bToEmptyPos, kChangeArray, kContLogMgr);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_INVALID_INVENTYPE"));
	return E_INVALID_INVENTYPE;
}

void CastSDBItemDataToSItem(SDBItemData const & kDBItemData, PgBase_Item&  kItem, SItemPos & kItemPos)
{
	SEnchantInfo kEnchantInfo;
	kEnchantInfo.Field_1(kDBItemData.Enchant_01);
	kEnchantInfo.Field_2(kDBItemData.Enchant_02);
	kEnchantInfo.Field_3(kDBItemData.Enchant_03);
	kEnchantInfo.Field_4(kDBItemData.Enchant_04);

	kItemPos.x = kDBItemData.InvType;
	kItemPos.y = kDBItemData.InvPos;

	kItem.Guid( kDBItemData.ItemGuid );
	kItem.ItemNo( kDBItemData.ItemNo );
	kItem.Count( kDBItemData.Count );
	kItem.EnchantInfo(kEnchantInfo);
	kItem.State( kDBItemData.State );

	SStatTrackInfo kStatTrack;
	kStatTrack.Clear();
	for (int i = 0; i < MAX_STAT_TRACK_ABIL; ++i)
	{
		switch (kDBItemData.StatTrackType[i])
		{
			case 0:
				{
					continue;
				}break;
			case AT_STAT_TRACK:
				{
					kStatTrack.HasStatTrack(true);
				}break;
			case AT_STAT_TRACK_KILL_COUNT_MON:
				{
					kStatTrack.MonsterKillCount(kDBItemData.StatTrackValue[i]);
				}break;
			case AT_STAT_TRACK_KILL_COUNT_PLAYER:
				{
					kStatTrack.PlayerKillCount(kDBItemData.StatTrackValue[i]);
				}break;
			default:
				{
					CAUTION_LOG(BM::LOG_LV5, "Undefined abil TYPE[" << kDBItemData.StatTrackType[i] << "] " << "VALUE[" << kDBItemData.StatTrackValue[i] << "]");
				}break;
		}
	}
	kItem.StatTrackInfo(kStatTrack);
	kItem.CreateDate(static_cast<BM::PgPackedTime>(kDBItemData.CreateDate));
}

void PgInventory::Init( BYTE const * pkInvExtern, BYTE const * pkExternIdx )
{
	if ( pkInvExtern )
	{
		for(int i = 0;i < MAX_DB_INVEXTEND_SIZE;i+=2)
		{
			ItemContainer* pCont = NULL;
			if(S_OK != GetContainer(static_cast<EInvType>(pkInvExtern[i]), pCont))
			{
				continue;
			}
			pCont->Reduction(pkInvExtern[i+1]);
		}
	}

	if ( pkExternIdx )
	{
		for(int i = 0;i < MAX_DB_INVEXTEND_SIZE;i+=2)
		{
			ItemContainer* pCont = NULL;
			if(S_OK != GetContainer(static_cast<EInvType>(pkExternIdx[i]), pCont))
			{
				continue;
			}
			pCont->ExtendMaxIdx(pkExternIdx[i+1]);
		}
	}

	// 돈 초기화는 시키지 말것
}

void PgInventory::InsertItemList( CONT_SELECTED_CHARACTER_ITEM_LIST const &kItemList )
{
	// 돈 초기화는 시키지 말것

	DB_ITEM_STATE_CHANGE_ARRAY kChangeArray;//보내줄 용도 아님.
	PgContLogMgr kContLogMgr;				//로그 서버에 기록되지 않음

	CONT_SELECTED_CHARACTER_ITEM_LIST::const_iterator item_itr = kItemList.begin();
	for( ; item_itr != kItemList.end() ; ++item_itr )
	{
		PgItemWrapper rElement = item_itr->second;
		HRESULT const hInsertItemRet = InsertItem( IMET_INSERT_FIXED|IMC_DB_INIT, rElement, false, false, kChangeArray,kContLogMgr );
		if( S_OK != hInsertItemRet )
		{
			CAUTION_LOG( BM::LOG_LV0, __FL__ << _T(" InsertItem Failed Ret["<<hInsertItemRet<<L"] - Pos[") << rElement.Pos().x << _T(",") << rElement.Pos().y << _T("] ItemGuid[")
				<< rElement.Guid() << _T("] OwnerID[") << OwnerGuid() << L"] ItemNo[" << rElement.ItemNo() << _T("]") );
		}
		/*=====================================================================================
			아이템 언바인드 타임이 유실된 아이템은 시간 정보를 다시 만들어 준다.
			창고에 옮겨진 아이템의 아이템 언바인드 버그로 생성된 아이템들이 해당된다.
		=====================================================================================*/
		if(0 < rElement.EnchantInfo().IsUnbindReq() && false == HasUnbindDate(rElement.Guid()))
		{
			BM::DBTIMESTAMP_EX kUnbindDate;
			kUnbindDate.SetLocalTime();
			AddUnbindDate( rElement.Guid(), kUnbindDate );
		}
	}
}

void PgInventory::ProcessTimeOutedItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, PgPlayer const * pkPlayer )
{
	if(	(true == m_kFit.OnTimeOutedItem(rkContModifyOrder))			|| 
		(true == m_kFitCash.OnTimeOutedItem(rkContModifyOrder))		||
		(true == m_kFitCostume.OnTimeOutedItem(rkContModifyOrder))  )
	{
		RefreshAbil(pkPlayer);	// 장착중인 아이템 사용 기간이 만료 되었을때...
	}

	if( m_kHomeInv.OnTimeOutedItem(rkContModifyOrder) && pkPlayer )
	{
		SHOMEADDR const &kHomeAddr = pkPlayer->HomeAddr();
		rkContModifyOrder.push_back(SPMO(IMET_MYHOME_SET_EQUIP_COUNT, OwnerGuid(), SMOD_MyHome_Sync_EquipItemCount(kHomeAddr.StreetNo(), kHomeAddr.HouseNo(), GetHomeEquipItemCount())));
	}
}

void PgInventory::ProcessTimeOutedMonsterCard(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder, CUnit const * pkUnit )
{
	if(	(true == m_kFit.OnTimeOutedMonsterCard(rkContModifyOrder))		||
		(true == m_kFitCash.OnTimeOutedMonsterCard(rkContModifyOrder))	||
		(true == m_kFitCostume.OnTimeOutedMonsterCard(rkContModifyOrder))		)
	{
		RefreshAbil(pkUnit);	// 장착중인 아이템 사용 기간이 만료 되었을때...
	}
}

void PgInventory::ProcessUnbindTimeItem(CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder)
{
	SYSTEMTIME kSysTime;
	if(false == g_kEventView.GetLocalTime(&kSysTime))
	{
		return;
	}

	BM::PgPackedTime const kCurTime = static_cast<BM::PgPackedTime>(kSysTime);

	for(CONT_UNBINDTIME::const_iterator iter = m_kContUnbindTime.begin();iter != m_kContUnbindTime.end();++iter)
	{
		if(kCurTime < (*iter).second)
		{
			continue;
		}

		PgBase_Item kItem;
		SItemPos kItemPos;
		if(S_OK == GetItem((*iter).first,kItem,kItemPos,false)) // 모든 인벤토리를 다 검사해야한다. 창고도 포함
		{
			SEnchantInfo kNewEnchant = kItem.EnchantInfo();
			kNewEnchant.IsBinding(0);
			kNewEnchant.IsUnbindReq(0);
			rkContModifyOrder.push_back(SPMO(IMET_MODIFY_ENCHANT,OwnerGuid(),SPMOD_Enchant(kItem, kItemPos, kNewEnchant)));
		}

		rkContModifyOrder.push_back(SPMO(IMET_DEL_UNBIND_DATE,OwnerGuid(),SMOD_DelUnbindDate((*iter).first)));
	}
}

void PgInventory::Money(__int64 const iNewMoney)
{
	if (iNewMoney > MAX_INV_MONEY)
	{
		m_kMoney = MAX_INV_MONEY;
	}
	else
	{
		m_kMoney = iNewMoney;
	}
}

HRESULT PgInventory::SetRentalSafeEnableTime(EInvType const kInvType,BM::DBTIMESTAMP_EX const & kDate)
{
	__int64 i64RentalTime = 0;

	CGameTime::DBTimeEx2SecTime(kDate,i64RentalTime);

	return SetRentalSafeEnableTime(kInvType,i64RentalTime);
}

PgRentalSafeItemContainer * PgInventory::GetRentalSafe(EInvType const kInvType)
{
	PgBaseItemContainer * pkCont;
	if(S_OK != GetContainer(kInvType,pkCont))
	{
		return NULL;
	}
	return dynamic_cast<PgRentalSafeItemContainer *>(pkCont);
}

HRESULT PgInventory::SetRentalSafeEnableTime(EInvType const kInvType,__int64 const i64RentalTime)
{
	PgRentalSafeItemContainer * pkSafeInv = GetRentalSafe(kInvType);
	if(NULL == pkSafeInv)
	{
		return E_INVALID_INVENTORY;
	}
	pkSafeInv->SetEnableUseTime(i64RentalTime);
	return S_OK;
}

HRESULT PgInventory::CheckEnableUseRentalSafe(EInvType const kInvType)
{
	PgRentalSafeItemContainer * pkSafeInv = GetRentalSafe(kInvType);
	if(NULL == pkSafeInv)
	{
		return E_INVALID_INVENTORY;
	}

	if(true == pkSafeInv->CheckEnableUse())
	{
		return S_OK;
	}

	return E_ENABLE_TIMEOUT;
}

__int64 const PgInventory::GetRentalSafeUseTime(EInvType const kInvType)
{
	PgRentalSafeItemContainer * pkSafeInv = GetRentalSafe(kInvType);
	if(NULL == pkSafeInv)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}
	return pkSafeInv->GetEanbleUseTime() - g_kEventView.GetLocalSecTime();
}

bool PgInventory::HasUnbindDate(BM::GUID const & kItemGuid)
{
	CONT_UNBINDTIME::const_iterator iter = m_kContUnbindTime.find(kItemGuid);
	return (iter != m_kContUnbindTime.end());
}

bool PgInventory::AddUnbindDate(BM::GUID const & kItemGuid,BM::DBTIMESTAMP_EX const & kUnbindDate)
{
	return m_kContUnbindTime.insert(std::make_pair(kItemGuid,kUnbindDate)).second;
}

bool PgInventory::DelUnbindDate(BM::GUID const & kItemGuid)
{
	CONT_UNBINDTIME::iterator iter = m_kContUnbindTime.find(kItemGuid);
	if(iter == m_kContUnbindTime.end())
	{
		return false;
	}

	m_kContUnbindTime.erase(iter);
	return true;
}

HRESULT PgInventory::GetActivateExpCardItem(PgBase_Item & kItem,SItemPos & kItemPos)
{
	return GetItem(ExpCardItem(), kItem, kItemPos);
}

void PgInventory::GetReductionInfo2Bin(ItemContainer const * pkCont,CONT_REDUCTION & kCont) const
{
	kCont.push_back(static_cast<BYTE>(pkCont->InvType()));
	kCont.push_back(static_cast<BYTE>(pkCont->Reduction()));
}

bool const PgInventory::GetReductionInfo2Bin(BYTE * const pkReductionInfo) const
{
	memset(pkReductionInfo,0,MAX_DB_INVEXTEND_SIZE);

	CONT_REDUCTION kCont;

	GetReductionInfo2Bin(&m_kEquipInv,kCont);
	GetReductionInfo2Bin(&m_kConsumeInv,kCont);
	GetReductionInfo2Bin(&m_kEtcInv,kCont);
	GetReductionInfo2Bin(&m_kCashInv,kCont);
	GetReductionInfo2Bin(&m_kSafeInv,kCont);
	GetReductionInfo2Bin(&m_kCashSafeInv,kCont);
	GetReductionInfo2Bin(&m_kFit,kCont);
	GetReductionInfo2Bin(&m_kPostInv,kCont);
	GetReductionInfo2Bin(&m_kHomeInv,kCont);
	GetReductionInfo2Bin(&m_kUserMarketInv,kCont);
	GetReductionInfo2Bin(&m_kFitCash,kCont);
	GetReductionInfo2Bin(&m_kRentalSafe1,kCont);
	GetReductionInfo2Bin(&m_kRentalSafe2,kCont);
	GetReductionInfo2Bin(&m_kRentalSafe3,kCont);
	GetReductionInfo2Bin(&m_kPetItem,kCont);
	GetReductionInfo2Bin(&m_kShareRentalSafe1,kCont);
	GetReductionInfo2Bin(&m_kShareRentalSafe2,kCont);
	GetReductionInfo2Bin(&m_kShareRentalSafe3,kCont);
	GetReductionInfo2Bin(&m_kShareRentalSafe4,kCont);
	GetReductionInfo2Bin(&m_kFitCostume, kCont);

	if(kCont.size() > MAX_DB_INVEXTEND_SIZE)
	{
		return false;
	}

	if(kCont.size())
	{
		memcpy(pkReductionInfo,&kCont.at(0),kCont.size());
	}

	return true;
}

void PgInventory::GetExtendMaxIdx2Bin(ItemContainer const * pkCont,CONT_REDUCTION & kCont) const
{
	kCont.push_back(static_cast<BYTE>(pkCont->InvType()));
	kCont.push_back(static_cast<BYTE>(pkCont->ExtendMaxIdx()));
}

bool const PgInventory::GetExtendIdx2Bin(BYTE * const pkReductionInfo) const
{
	memset(pkReductionInfo,0,MAX_DB_INVEXTEND_SIZE);

	CONT_REDUCTION kCont;

	GetExtendMaxIdx2Bin(&m_kEquipInv,kCont);
	GetExtendMaxIdx2Bin(&m_kConsumeInv,kCont);
	GetExtendMaxIdx2Bin(&m_kEtcInv,kCont);
	GetExtendMaxIdx2Bin(&m_kCashInv,kCont);
	GetExtendMaxIdx2Bin(&m_kSafeInv,kCont);
	GetExtendMaxIdx2Bin(&m_kCashSafeInv,kCont);
	GetExtendMaxIdx2Bin(&m_kFit,kCont);
	GetExtendMaxIdx2Bin(&m_kPostInv,kCont);
	GetExtendMaxIdx2Bin(&m_kHomeInv,kCont);
	GetExtendMaxIdx2Bin(&m_kUserMarketInv,kCont);
	GetExtendMaxIdx2Bin(&m_kFitCash,kCont);
	GetExtendMaxIdx2Bin(&m_kRentalSafe1,kCont);
	GetExtendMaxIdx2Bin(&m_kRentalSafe2,kCont);
	GetExtendMaxIdx2Bin(&m_kRentalSafe3,kCont);
	GetExtendMaxIdx2Bin(&m_kPetItem,kCont);
	GetExtendMaxIdx2Bin(&m_kShareRentalSafe1,kCont);
	GetExtendMaxIdx2Bin(&m_kShareRentalSafe2,kCont);
	GetExtendMaxIdx2Bin(&m_kShareRentalSafe3,kCont);
	GetExtendMaxIdx2Bin(&m_kShareRentalSafe4,kCont);
	GetExtendMaxIdx2Bin(&m_kFitCostume, kCont);

	if(kCont.size() > MAX_DB_INVEXTEND_SIZE)
	{
		return false;
	}

	if(kCont.size())
	{
		memcpy(pkReductionInfo,&kCont.at(0),kCont.size());
	}

	return true;
}

BYTE const PgInventory::GetInvExtendSize(EInvType const kInvType)
{
	ItemContainer* pCont = NULL;
	if(S_OK != GetContainer(kInvType, pCont))
	{
		return 0;
	}

	return pCont->Reduction();
}

bool const PgInventory::InvExtend(EInvType const kInvType,BYTE const bReductionNum)
{
	ItemContainer* pCont = NULL;
	if(S_OK != GetContainer(kInvType, pCont))
	{
		return false;
	}

	if(pCont->Reduction() < bReductionNum)
	{
		return false;
	}

	BYTE const bNewVal = pCont->Reduction() - bReductionNum;
	pCont->Reduction(bNewVal);
	return true;
}

BYTE const PgInventory::GetEnableExtendMaxIdx(EInvType const kInvType)
{
	ItemContainer* pCont = NULL;
	if(S_OK != GetContainer(kInvType, pCont))
	{
		return 0;
	}
	return pCont->GetMaxIDXExtend() - pCont->GetMaxIDX();
}

bool const PgInventory::ExtendMaxIdx(EInvType const kInvType,BYTE const bIncIdxNum)
{
	ItemContainer* pCont = NULL;
	if(S_OK != GetContainer(kInvType, pCont))
	{
		return false;
	}

	if(pCont->GetMaxIDXExtend() < pCont->GetMaxIDX() + bIncIdxNum)
	{
		return false;
	}

	BYTE const bNewVal = pCont->ExtendMaxIdx() + bIncIdxNum;
	pCont->ExtendMaxIdx(bNewVal);
	return true;
}

HRESULT PgInventory::GetSelectedPetID( BM::GUID &rkOutID )
{
	return GetItemID( PgItem_PetInfo::ms_kPetItemEquipPos, rkOutID );
}

HRESULT PgInventory::GetSelectedPetItem( PgBase_Item &rkPetItem )
{
	return GetItem( PgItem_PetInfo::ms_kPetItemEquipPos, rkPetItem );
}

size_t PgInventory::GetPetItemCount(void)
{
	size_t iCount = 0;

	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer( IT_CASH, pCont ) )
	{
		iCount += pCont->GetItemCount( UIT_STATE_PET );
	}

	if( S_OK == GetContainer( IT_CASH_SAFE, pCont ) )
	{
		iCount += pCont->GetItemCount( UIT_STATE_PET );
	}
	
	BM::GUID kPetID;
	if ( S_OK == GetSelectedPetID( kPetID ) )
	{
		++iCount;
	}
	return iCount;
}

void PgInventory::AddOverSkillOption(int const iSkillNo, int const iAddLevel)
{
	PgInventoryUtil::GetOverSkill(m_kOverSkill, iSkillNo, iAddLevel);
}

void PgInventory::GetOverSkillOption(CONT_SKILL_OPT& kSkillOpt)const
{
	kSkillOpt = m_kOverSkill;
}

size_t PgInventory::GetHomeEquipItemCount()
{
	CONT_HAVE_ITEM_NO kCont;
	GetItems(IT_HOME, kCont, true);
	return kCont.size();
}

HRESULT PgInventory::MakeSortOrder(EInvType const kInvType, CUnit const * pkUnit, CONT_QUEST_ITEM const & kContQuestItem, CONT_PLAYER_MODIFY_ORDER & rkContModifyOrder)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer( kInvType, pCont ) )
	{
		return pCont->MakeSortOrder( pkUnit, kContQuestItem, rkContModifyOrder);
	}
	return E_FAIL;
}
int PgInventory::GetItemEnchantLevel(SItemPos const& rkPos)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer( static_cast< EInvType >(rkPos.x), pCont) )
	{
		return pCont->GetItemEnchantLevel(rkPos);
	}
	return 0;
}

size_t PgInventory::GetItemCount(EInvType const eInvType)
{
	ItemContainer* pCont = NULL;
	if( S_OK == GetContainer(eInvType, pCont) )
	{
		return pCont->size();
	}
	return 0;
}

void PgInventory::SaveGroupCoolTime(DWORD const dwCurTime, BYTE (&abyItemGroupCoolTime)[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE]) const
{
	// 대충 20개 정도 저장 가능하다
	BM::Stream kSaveData;
	DWORD const dwMinimumTime = 1000 * 60 * 5; // 5분 이상만
	CONT_ITEM_COOLTIME::const_iterator iter = m_kGroupCoolTime.begin();
	while( m_kGroupCoolTime.end() != iter )
	{
		CONT_ITEM_COOLTIME::key_type const& rkKey = (*iter).first;
		CONT_ITEM_COOLTIME::mapped_type const& rkCoolTimeInfo = (*iter).second;
		DWORD const dwDiffTime = rkCoolTimeInfo.dwEndCoolTime - dwCurTime;
		if( static_cast< size_t >(MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE) > kSaveData.Size()
		&&	rkCoolTimeInfo.bSaveDB
		&&	dwMinimumTime < dwDiffTime )
		{
			kSaveData.Push( rkKey );
			kSaveData.Push( rkCoolTimeInfo.byCoolTimeType );
			kSaveData.Push( rkCoolTimeInfo.dwTotalCoolTime );
			kSaveData.Push( dwDiffTime );
		}
		++iter;
	}
	if( 0 < kSaveData.Size() )
	{
		memcpy_s(abyItemGroupCoolTime, MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE, &kSaveData.Data().at(0), kSaveData.Size());
	}
}
void PgInventory::LoadGroupCoolTime(DWORD const dwLogInGapTime, BYTE const (&abyItemGroupCoolTime)[MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE])
{
	BM::Stream kSaveData;
	kSaveData.Push(abyItemGroupCoolTime, MAX_DB_ITEM_GROUP_COOLTIMEP_SIZE);
	kSaveData.PosAdjust();

	CONT_ITEM_COOLTIME::key_type kKey = 0;
	CONT_ITEM_COOLTIME::mapped_type kNewCoolTimeInfo;
	kNewCoolTimeInfo.bSaveDB = true; // 공통 정보

	while(1)
	{
		if( kSaveData.Pop(kKey)
		&&	kSaveData.Pop(kNewCoolTimeInfo.byCoolTimeType)
		&&	kSaveData.Pop(kNewCoolTimeInfo.dwTotalCoolTime)
		&&	kSaveData.Pop(kNewCoolTimeInfo.dwEndCoolTime)
		)
		{
			if( 0 < kKey )
			{
				if( dwLogInGapTime < kNewCoolTimeInfo.dwTotalCoolTime
				&& kNewCoolTimeInfo.dwEndCoolTime < kNewCoolTimeInfo.dwTotalCoolTime)
				{//로그아웃한 사이의 시간이랑 남은 쿨타임 쿨타임 총량보다 작을 때만
					kNewCoolTimeInfo.dwEndCoolTime = g_kEventView.GetServerElapsedTime() + kNewCoolTimeInfo.dwEndCoolTime - dwLogInGapTime;
					m_kGroupCoolTime.insert( std::make_pair(kKey, kNewCoolTimeInfo) );
				}
			}
			else
			{
				return ;
			}
		}
		else
		{
			return; // 중단
		}
	}
}

void SPMOD_ADD_StrategySkillTabChange::WriteToPacket(BM::Stream & rkPacket)const
{
	rkPacket.Push(iValue);
	kQInv.WriteToPacket(rkPacket);
}

void SPMOD_ADD_StrategySkillTabChange::ReadFromPacket(BM::Stream & rkPacket)
{
	rkPacket.Pop(iValue);
	kQInv.ReadFromPacket(rkPacket);
}