#include "stdafx.h"
#include "Lohengrin/packetstruct.h"
#include "PgActionT2M.h"


PgUtilAction_Base::PgUtilAction_Base()
{
}

PgUtilAction_Base::~PgUtilAction_Base()
{
}


//
PgActionA2M_CreateItemBase::PgActionA2M_CreateItemBase(EItemModifyParentEventType const kType, CONT_ITEM_CREATE_ORDER const& rkContItem)
: m_kType(kType), m_kContItem(rkContItem)
{
}

PgActionA2M_CreateItemBase::~PgActionA2M_CreateItemBase()
{
}

bool PgActionA2M_CreateItemBase::DoAction(BM::GUID const &rkCharGuid,SGroundKey const &rkGndKey)
{
	BM::Stream kMPacket(PT_N_M_REQ_CREATE_ITEM, rkCharGuid);
	kMPacket.Push(m_kType);
	PU::TWriteArray_M(kMPacket, m_kContItem);
	return Send(rkCharGuid, rkGndKey, kMPacket);
}

//
PgActionT2M_CreateItem::PgActionT2M_CreateItem(const EItemModifyParentEventType kType, CONT_ITEM_CREATE_ORDER const& rkConItem)
	: PgActionA2M_CreateItemBase(kType, rkConItem)
{
}

PgActionT2M_CreateItem::~PgActionT2M_CreateItem()
{
}

bool PgActionT2M_CreateItem::Send(BM::GUID const& rkCharGuid, SGroundKey const& rkGndKey, BM::Stream const& rkPacket)
{
	return g_kServerSetMgr.Locked_SendToGround( rkGndKey, rkPacket );
}
