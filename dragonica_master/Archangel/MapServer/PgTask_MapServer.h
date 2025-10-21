#ifndef MAP_MAPSERVER_PGTSK_MAPSERVER_H
#define MAP_MAPSERVER_PGTSK_MAPSERVER_H

#include "Variant/PgCouponEventView.h"

typedef enum ePrimaryEventMessageType
{
	PMET_NONE_MAP = 0,
	PMET_PARTY_MAP = 1,
	PMET_GROUND_MGR = 2,
	PMET_COUPON_EVENT_VIEW_MAP = 3,
	PMET_HACK_CHECK_DUALKEYBOARD = 4,
}EPrimaryEventMessageType;

namespace TaskUtil
{
	bool Init();
};

class PgTask_MapServer
	:public PgTask<>
{
public:
	PgTask_MapServer(){};
	virtual ~PgTask_MapServer(){};

public:
	virtual void HandleMessage(SEventMessage *pkMsg);
	void Close();

	bool WriteToPacket(EPrimaryEventMessageType eType, BM::Stream & rkPacket);

private:
	PgCouponEventView m_kCouponEventView;
};

#define g_kTask SINGLETON_STATIC(PgTask_MapServer)

#endif // MAP_MAPSERVER_PGTSK_MAPSERVER_H