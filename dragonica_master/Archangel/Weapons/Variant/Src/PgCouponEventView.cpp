#include "stdafx.h"
#include "Lohengrin/PacketStruct2.h"
#include "PgEventView.h"
#include "PgCouponEventView.h"

void PgCouponEventView::ReadFromPacket(BM::Stream & kPacket)
{
	BM::CAutoMutex kLockFree(m_kMutex);

	m_kContCouponEvent.clear();
	m_kContCouponEventReward.clear();

	PU::TLoadArray_M(kPacket,m_kContCouponEvent);
	PU::TLoadArray_M(kPacket,m_kContCouponEventReward);

	CONT_COUPON_EVENT_INFO kContEvent;
	GetActivatedCouponEvent(kContEvent);

	CONT_COUPON_EVENT_REWARD kContReward;

	for(CONT_COUPON_EVENT_INFO::iterator iter = kContEvent.begin();iter != kContEvent.end();++iter)
	{
		for(CONT_COUPON_EVENT_REWARD::iterator rwiter = m_kContCouponEventReward.begin();rwiter != m_kContCouponEventReward.end();++rwiter)
		{
			if((*rwiter).iEventKey != (*iter).iEventKey)
			{
				continue;
			}
			kContReward.push_back((*rwiter));
		}
	}

	m_kContCouponEvent = kContEvent;
	m_kContCouponEventReward = kContReward;
}

void PgCouponEventView::WriteToPacket(BM::Stream & kPacket) const
{
	BM::CAutoMutex kLockFree(m_kMutex);

	PU::TWriteArray_M(kPacket,m_kContCouponEvent);
	PU::TWriteArray_M(kPacket,m_kContCouponEventReward);
}

void PgCouponEventView::GetActivatedCouponEvent(CONT_COUPON_EVENT_INFO & kContEvent)
{
	BM::CAutoMutex kLockFree(m_kMutex);

	kContEvent.clear();

	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();
	__int64 i64StartTime = 0,
			i64EndTime = 0;

	for(CONT_COUPON_EVENT_INFO::iterator iter = m_kContCouponEvent.begin();iter != m_kContCouponEvent.end();++iter)
	{
		SCOUPON_EVENT_INFO const & kEventInfo = (*iter);
		CGameTime::DBTimeEx2SecTime(BM::DBTIMESTAMP_EX(kEventInfo.kStartTime),i64StartTime);
		CGameTime::DBTimeEx2SecTime(BM::DBTIMESTAMP_EX(kEventInfo.kEndTime),i64EndTime);

		if((i64StartTime > i64CurTime) || (i64CurTime > i64EndTime))
		{
			continue;
		}

		kContEvent.push_back(kEventInfo);
	}
}

void PgCouponEventView::GetEventReward(int const iKey,CONT_COUPON_EVENT_REWARD & kContReward)
{
	BM::CAutoMutex kLockFree(m_kMutex);

	kContReward.clear();

	for(CONT_COUPON_EVENT_REWARD::iterator iter = m_kContCouponEventReward.begin();iter != m_kContCouponEventReward.end();++iter)
	{
		if((*iter).iEventKey != iKey)
		{
			continue;
		}
		kContReward.push_back((*iter));
	}
}

void PgCouponEventView::ProcessMsg(BM::Stream *pkMsg)
{
	BM::CAutoMutex kLockFree(m_kMutex);

	ECouponEvent_MsgType eType;
	pkMsg->Pop(eType);
	switch( eType )
	{
	case E_CouponEvent_ReadFromPacket:
		{
			ReadFromPacket(*(dynamic_cast<BM::Stream*>(pkMsg)));
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("unhandled msg type=") << static_cast<int>(eType));
		}break;
	}
}