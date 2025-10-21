#include "StdAfx.h"
#include "JobDispatcher.h"

PgJobDispatcher::PgJobDispatcher()
{
}

PgJobDispatcher::~PgJobDispatcher()
{
}

HRESULT PgJobDispatcher::VPush( SActionOrder const * pkJob, unsigned long priority )
{
	CONT_PLAYER_MODIFY_ORDER::const_iterator order_itor = pkJob->kContOrder.begin();
	while (order_itor != pkJob->kContOrder.end())
	{
		if (order_itor->OwnerGuid() != BM::GUID::NullData())
		{
			((SActionOrder*)pkJob)->InsertTarget(order_itor->OwnerGuid());
		}
		++order_itor;
	}

	return PgMsgDispatcher<SActionOrder, PgJobWorker>::VPush( pkJob, priority );
}

