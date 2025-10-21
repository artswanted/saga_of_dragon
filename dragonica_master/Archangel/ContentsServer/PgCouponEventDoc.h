#ifndef CONTENTS_CONTENTSSERVER_ETC_PGCOUPONEVENTDOC_H
#define CONTENTS_CONTENTSSERVER_ETC_PGCOUPONEVENTDOC_H

#include "ace/time_value.h"
#include "BM/PgTask.h"
#include "Variant/idobject.h"
#include "Variant/PgCouponEventView.h"

typedef struct tagCOUPONEVENTACTION
{
	__int64 i64LastActionTime;
	BYTE	bActionCount;

	tagCOUPONEVENTACTION()
	{
		i64LastActionTime = 0;
		bActionCount = 0;
	}
}SCOUPONEVENTACTION;

BYTE const MAX_COUPON_EVENT_ACTION_FAIL_COUNT = 10;

typedef std::map<BM::GUID,SCOUPONEVENTACTION> CONT_COUPONEVENTACTION;

class PgCouponEventDoc
{
public:

	explicit PgCouponEventDoc(){}
	~PgCouponEventDoc(){}

public:
	
	bool ProcessMsg(SEventMessage *pkMsg);
	void Build(CONT_COUPON_EVENT_INFO const & kContEventInfo,CONT_COUPON_EVENT_REWARD const & kContEventReward);
	void WriteToPacket(BM::Stream & kPacket);

	void UpdateEventActionFailPlayer(BM::GUID const & kOnwerGuid);
	bool CheckEnableEventActionPlayer(BM::GUID const & kOnwerGuid);

protected:

	CONT_COUPON_EVENT_INFO m_kContCouponEvent;
	CONT_COUPON_EVENT_REWARD m_kContCouponEventReward;
	mutable Loki::Mutex m_kMutex;

	CONT_COUPONEVENTACTION m_kContEventAction;
	mutable Loki::Mutex m_kEventAction;
};

//#define g_kCouponEventDoc SINGLETON_STATIC(PgCouponEventDoc)

#endif // CONTENTS_CONTENTSSERVER_ETC_PGCOUPONEVENTDOC_H