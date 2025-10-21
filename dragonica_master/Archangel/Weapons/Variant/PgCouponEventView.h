#ifndef	WEAPON_VARIANT_PGCOUPONEVENTVIEW_H
#define WEAPON_VARIANT_PGCOUPONEVENTVIEW_H

#include "ace/high_res_Timer.h"
//#include "BM\Packet.h"
#include "Lohengrin\GameTime.h"
#include "Variant/idobject.h"
#include "Lohengrin/DBTables.h"

class PgCouponEventView
{
public:
	explicit PgCouponEventView(){}
	~PgCouponEventView(){}

public:
	void ProcessMsg(BM::Stream *pkMsg);
	void WriteToPacket(BM::Stream & kPacket) const;
protected:
	void ReadFromPacket(BM::Stream & kPacket);

	void GetActivatedCouponEvent(CONT_COUPON_EVENT_INFO & kContEvent);
	void GetEventReward(int const iKey,CONT_COUPON_EVENT_REWARD & kContReward);

protected:

	CONT_COUPON_EVENT_INFO m_kContCouponEvent;
	CONT_COUPON_EVENT_REWARD m_kContCouponEventReward;
	mutable Loki::Mutex m_kMutex;
};

//#define g_kCouponEventView SINGLETON_STATIC(PgCouponEventView)

#endif // WEAPON_VARIANT_PGCOUPONEVENTVIEW_H