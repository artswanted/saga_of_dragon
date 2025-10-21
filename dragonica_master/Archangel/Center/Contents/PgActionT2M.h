#ifndef CENTER_CENTERSERVER_CONTENTS_PGACTIONT2M_H
#define CENTER_CENTERSERVER_CONTENTS_PGACTIONT2M_H

#include "Lohengrin/packetstruct.h"

class PgUtilAction_Base
{
public:
	PgUtilAction_Base();
	virtual ~PgUtilAction_Base();

	virtual bool DoAction(BM::GUID const &rkCharGuid, SGroundKey const &rkGndKey) = 0;

protected:
	virtual bool Send(BM::GUID const& rkCharGuid, SGroundKey const& rkGndKey, BM::Stream const& rkPacket) = 0;
};


class PgActionA2M_CreateItemBase : public PgUtilAction_Base
{
public:
	PgActionA2M_CreateItemBase(EItemModifyParentEventType const kType, CONT_ITEM_CREATE_ORDER const& rkContItem);
	virtual ~PgActionA2M_CreateItemBase();

public:
	bool DoAction(BM::GUID const &rkCharGuid, SGroundKey const &rkGndKey);

protected:
	virtual bool Send(BM::GUID const& rkCharGuid, SGroundKey const& rkGndKey, BM::Stream const& rkPacket) = 0;

private:
	EItemModifyParentEventType const m_kType;
	CONT_ITEM_CREATE_ORDER const& m_kContItem;
};

class PgActionT2M_CreateItem : public PgActionA2M_CreateItemBase
{
public:
	PgActionT2M_CreateItem(const EItemModifyParentEventType kType, CONT_ITEM_CREATE_ORDER const& rkConItem);
	virtual ~PgActionT2M_CreateItem();

protected:
	bool Send(BM::GUID const& rkCharGuid, SGroundKey const& rkGndKey, BM::Stream const& rkPacket);
};

#endif // CENTER_CENTERSERVER_CONTENTS_PGACTIONT2M_H