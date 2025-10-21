#include "stdafx.h"
#include "Lohengrin/GameTime.h"
#include "PgCouponEventDoc.h"
#include "PgServerSetMgr.h"

void PgCouponEventDoc::Build(CONT_COUPON_EVENT_INFO const & kContEventInfo,CONT_COUPON_EVENT_REWARD const & kContEventReward)
{
	BM::CAutoMutex kLockFree(m_kMutex);

	m_kContCouponEvent.clear();
	m_kContCouponEventReward.clear();
	m_kContCouponEvent = kContEventInfo;
	m_kContCouponEventReward = kContEventReward;

	BM::Stream kPacket;
	WriteToPacket(kPacket);

	{
		BM::Stream kViewPacket;
		kViewPacket.Push(E_CouponEvent_ReadFromPacket);
		kViewPacket.Push(kPacket);
		SendToCouponEventView(kViewPacket);
	}
	kPacket.PosAdjust();

	BM::Stream kSyncPacket(PT_T_A_COUPON_EVENT_SYNC);
	kSyncPacket.Push(kPacket);

	SendToServerType(CEL::ST_CENTER,kSyncPacket);
}

void PgCouponEventDoc::WriteToPacket(BM::Stream & kPacket)
{
	PU::TWriteArray_M(kPacket,m_kContCouponEvent);
	PU::TWriteArray_M(kPacket,m_kContCouponEventReward);
}

void PgCouponEventDoc::UpdateEventActionFailPlayer(BM::GUID const & kOnwerGuid)
{
	SCOUPONEVENTACTION & kEventAction = m_kContEventAction[kOnwerGuid];

	++kEventAction.bActionCount;

	kEventAction.bActionCount = __min(kEventAction.bActionCount,MAX_COUPON_EVENT_ACTION_FAIL_COUNT);

	kEventAction.i64LastActionTime = g_kEventView.GetLocalSecTime();
}

const __int64 ONE_HOUR_TO_SEC = 3600;

bool PgCouponEventDoc::CheckEnableEventActionPlayer(BM::GUID const & kOnwerGuid)
{

	CONT_COUPONEVENTACTION::iterator iter = m_kContEventAction.find(kOnwerGuid);
	if(iter == m_kContEventAction.end())
	{
		return true;
	}

	SCOUPONEVENTACTION & kEventAction = (*iter).second;

	if(kEventAction.bActionCount < MAX_COUPON_EVENT_ACTION_FAIL_COUNT)
	{
		return true;
	}

	__int64 const i64CurTime = g_kEventView.GetLocalSecTime();
	if((i64CurTime - kEventAction.i64LastActionTime) < ONE_HOUR_TO_SEC)
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	kEventAction.bActionCount = 0;

	return true;
}

bool PgCouponEventDoc::ProcessMsg(SEventMessage *pkMsg)
{
	BM::CAutoMutex kLockFree(m_kEventAction);

	ECouponEvent_MsgType eType = E_CouponEvnet_None;
	pkMsg->Pop(eType);
	switch (eType)
	{
	case E_CouponEvent_TakeCoupon:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharacterGuid;
			std::wstring kCouponKey;

			pkMsg->Pop(kMemberGuid);
			pkMsg->Pop(kCharacterGuid);
			pkMsg->Pop(kCouponKey);

			if(!CheckEnableEventActionPlayer(kCharacterGuid))
			{
				BM::Stream kPacket(PT_N_M_ANS_TAKE_COUPON,kCharacterGuid);
				kPacket.Push(CRT_TOO_MANY_FAILED);
				g_kRealmUserMgr.Locked_SendToUserGround( kCharacterGuid, kPacket, false, true);
				return true;
			}

			if(false == kCouponKey.empty())
			{//같은 결과를 리턴 하도록 셋팅(DQT_TAKE_COUPON_REWARD)
//				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_TAKE_COUPON_REWARD, _T("{? = call [dbo].[up_Take_Coupon_Event2](?,?,?)}"));
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_TAKE_COUPON_REWARD, _T("EXEC [dbo].[up_Take_Coupon_Event]"));
				kQuery.InsertQueryTarget(kCharacterGuid);
				kQuery.QueryOwner(kCharacterGuid);

				kQuery.PushStrParam(kMemberGuid);
				kQuery.PushStrParam(kCharacterGuid);
				kQuery.PushStrParam(kCouponKey);
				kQuery.PushStrParam(g_kProcessCfg.SiteNo());
				kQuery.PushStrParam(g_kProcessCfg.RealmNo());

				g_kCoreCenter.PushQuery(kQuery);
			}
			else
			{//같은 결과를 리턴 하도록 셋팅(DQT_TAKE_COUPON_REWARD) => 자기 memberID 로 확인하는.
//				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_TAKE_COUPON_REWARD, _T("{? = call [dbo].[up_Take_Coupon_Auth_Event](?,?)}"));
				CEL::DB_QUERY kQuery( DT_MEMBER, DQT_TAKE_COUPON_REWARD, _T("EXEC [dbo].[up_Take_Coupon_Auth_Event]"));
				kQuery.InsertQueryTarget(kCharacterGuid);
				kQuery.QueryOwner(kCharacterGuid);

				kQuery.PushStrParam(kMemberGuid);
				kQuery.PushStrParam(kCharacterGuid);
				kQuery.PushStrParam(g_kProcessCfg.SiteNo());
				kQuery.PushStrParam(g_kProcessCfg.RealmNo());

				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case E_CouponEvent_UpdateFailed:
		{
			BM::GUID kGuid;
			pkMsg->Pop(kGuid);
			UpdateEventActionFailPlayer(kGuid);
		}break;
	default:
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T("undefined msg type=") << static_cast<int>(eType));
			return false;
		}break;
	}
	return true;
}