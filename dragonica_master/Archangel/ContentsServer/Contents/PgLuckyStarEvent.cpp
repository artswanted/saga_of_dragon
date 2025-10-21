
/****************************************************************
구현 : 심헌섭
제목 : '행운의 별자리를 찾아서' 이벤트
특징 : 
		Local DB에 이벤트 정보, 보상 정보, 이벤트 & 보상 관계 테이블 이렇게 3개를 통해서 이벤트를 관리함
		User DB에 유저의 정보를 저장해 임시점검, 서버다운시 유저가 획득한 정보를 유지한다
		유저관리 키로는 MemberKey를 사용함
		이유는 한 유저가 여러 캐릭터로 중복해서 이벤트에 참여할 수 없게 하기 위해서임

		Update Event Sub Type 정보
		{
			EVENTSTATE		= 0,
			SENDRESULTTYPE	= 1,
			LASTREWARDID	= 2,
			EVENTCOUNT		= 3,
			LASTRESULTSTAR	= 4,
			EVENTFLAG		= 5, //이벤트 옵션
		}


		EVENTFLAG 설명 : OR연산을 통해 옵션을 확인할 수 있음
			- 1 : 이벤트 자동실행금지
			- 2 : 미정의
			- 4 : 미정의
			- 8 : 미정의
****************************************************************/


#include "stdafx.h"
#include "PgLuckyStarEvent.h"
#include "PgServerSetMgr.h"
#include "Item/PgPostManager.h"
#include "JobWorker.h"
#include "JobDispatcher.h"
#include "PgGMProcessMgr.h"
#include "Global.h"

int const LAST_EVENTID = 0;

namespace PgLuckyStarEventUtil
{
	int const ONE_HOUR   = 60 * 60 * 1000;
	int const TEN_MINUTE = 10 * 60 * 1000;	
	int const FIVE_MINUTE = 5 * 60 * 1000;	

	ELuckyStarResult ResultLuckyStar()
	{
		return (ELuckyStarResult)BM::Rand_Range(12, 1);
	}

	DWORD GetNextEventTime(int iDelayTime=0)
	{
		if(0 == iDelayTime)
		{
			SYSTEMTIME kSystemTime; 
			GetLocalTime(&kSystemTime); 

			DWORD iTmpTime = (kSystemTime.wMinute* 60 + kSystemTime.wSecond) * 1000 + kSystemTime.wMilliseconds;
			return ONE_HOUR - iTmpTime;
		}
		else
		{
			return iDelayTime * 1000;
		}
	}

	struct SRewardStepFind
	{
		SRewardStepFind( int iStep ) : m_iStep(iStep)
		{}

		bool operator()( PgLuckyStarEvent::SLSRM & kRewardSet )const
		{
			return kRewardSet.iStep <= m_iStep;
		}

	private:
		int const m_iStep;
	};

	struct SNextRewardIdFind
	{
		SNextRewardIdFind( int iID ) : m_iID(iID)
		{}

		bool operator()( int const & iNext )const
		{
			return iNext > m_iID;
		}

	private:
		int const m_iID;
	};

	bool IsLastEvent(BM::PgPackedTime kEndDate, int iDelayTime=0)
	{
		if(kEndDate.IsNull())
		{
			return false;
		}

		if(0==iDelayTime)
		{
			BM::DBTIMESTAMP_EX kSysTime;
			g_kEventView.GetLocalTime(kSysTime);
			BM::PgPackedTime const kCurTime(kSysTime);

			kEndDate.Min(0);
			kEndDate.Sec(0);

			if(kEndDate < kCurTime)
			{
				return true;
			}
		}
		else
		{
			time_t kTmEnd;
			time_t kTmCur;
			struct tm  kUserTime;
			double dwDiff;

			kUserTime.tm_year   = (2000+kEndDate.Year())   -1900; // 주의 :년도는 1900년부터 시작
			kUserTime.tm_mon    = kEndDate.Month()     -1; // 주의 :월은 0부터 시작
			kUserTime.tm_mday   = kEndDate.Day();
			kUserTime.tm_hour   = kEndDate.Hour();
			kUserTime.tm_min    = kEndDate.Min();
			kUserTime.tm_sec    = kEndDate.Sec();
			kUserTime.tm_isdst  = 0;						// 썸머 타임 사용 안함

			kTmEnd = mktime( &kUserTime );
			time( &kTmCur );

			dwDiff = difftime(kTmEnd, kTmCur);

			if(dwDiff-iDelayTime < 0)
			{
				return true;
			}
		}

		return false;
	}

	void ClearDate(BM::PgPackedTime& rkDateTime)
	{
		rkDateTime.Year(0);
		rkDateTime.Month(0);
		rkDateTime.Day(0);
	}

	void ClearTime(BM::PgPackedTime& rkDateTime)
	{
		rkDateTime.Hour(0);
		rkDateTime.Min(0);
		rkDateTime.Sec(0);
	}

	bool IsInDateTime(BM::PgPackedTime const& kStart, BM::PgPackedTime const& kEnd, BM::PgPackedTime const& kCur)
	{
		if( kStart < kCur
		&&	kCur < kEnd )
		{
			return true;
		}
		return false;
	}

	void ResetDailyEvent(CONT_LUCKYSTAREVENTINFO &rkCopyEvent)
	{
		CONT_LUCKYSTAREVENTINFO::iterator iter = rkCopyEvent.begin();
		while( rkCopyEvent.end() != iter )
		{
			CONT_LUCKYSTAREVENTINFO::mapped_type& rkElement = (*iter).second;
			rkElement.bState = LSS_NONE;
			++iter;
		}
	}
};

PgLuckyStarEvent::PgLuckyStarEvent()
{
	ClearEvent();
	m_kLuckyStarState    = LSS_RELOAD;
}

void PgLuckyStarEvent::ClearEvent()
{
	m_bLuckyStarEventRun = false;
	m_bLuckyStarJoinedUserDBRead = false;
	m_kLuckyStarState    = LSS_NONE;
	m_kSendResultType	 = LSSRT_NONE;
	m_kContParticipant.clear();
	m_kEventGuid.Clear();
	m_kCurEvent.Clear();
	m_dwNextEventTime = 0;
	m_dwOldEventTime  = 0;
	m_kRewardMgr.clear();
	m_kBeforResultStar = LSR_NONE;
	m_kNowResultStar = LSR_NONE;
	m_kContLuckyUser.clear();
	m_kContNomalUser.clear();
}

void PgLuckyStarEvent::UpdateJoinedUser(BM::GUID const & kMemberGuid, CONT_LUCKYSTAR_PARTICIPANT::mapped_type const & kMember, BYTE bState)
{
	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LUCKYSTAR_UPDATE_JOINEDUSER, _T("EXEC [DBO].[up_Update_LuckyStarJoinedUser]"));
	kQuery.InsertQueryTarget(m_kEventGuid);
	kQuery.QueryOwner(m_kEventGuid);
	kQuery.PushStrParam(m_kCurEvent.iEventID);
	kQuery.PushStrParam(kMemberGuid);
	kQuery.PushStrParam(kMember.kCharGuid);
	kQuery.PushStrParam(bState); //0:제거, 1:저장
	kQuery.PushStrParam((int)kMember.kSelectStar);
	kQuery.PushStrParam(kMember.iLuckyStep);
	g_kCoreCenter.PushQuery(kQuery);

	if(0 == bState)
	{
		SContentsUser rkOut;
		if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kMember.kCharGuid,false,rkOut))
		{
			PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_User_Exit,rkOut.kMemGuid,kMember.kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
			PgLog kLog;
			kLog.Set(0,m_kCurEvent.iEventID);
			kLog.Set(1,m_kCurEvent.iEventCount);
			kLog.Set(3,kMember.iLuckyStep);
			kLog.Set(2,m_kEventGuid.str());
			kContLog.Add(kLog);
			kContLog.Commit();
		}
	}
}

// 이벤트 서브 내용 바꾸기
void PgLuckyStarEvent::UpdateEventSub(int iType, int iValue)
{ 
	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LUCKYSTAR_UPDATE_EVENT_SUB, _T("EXEC [dbo].[UP_Update_LuckyStarEventSub]"));
	kQuery.PushStrParam(m_kCurEvent.iEventID);
	kQuery.PushStrParam(iType);
	kQuery.PushStrParam(iValue);
	g_kCoreCenter.PushQuery(kQuery);
}

void PgLuckyStarEvent::BroadCast(BM::Stream const & rkPacket)
{
	std::vector<BM::GUID> kVec;
	CONT_LUCKYSTAR_PARTICIPANT::const_iterator iter = m_kContParticipant.begin();
	while(m_kContParticipant.end() != iter)
	{
		if(LSUS_JOIN == (*iter).second.kState)
		{
			kVec.push_back((*iter).second.kCharGuid);
		}

		++iter;
	}
	g_kRealmUserMgr.Locked_SendToUser(kVec, rkPacket, false);
}

ELUCKYSTAR_EVENT_RESULT const PgLuckyStarEvent::OnRecvPT_M_I_REQ_LUCKYSTAR_ENTER(BM::GUID const & kCharGuid,BM::Stream * const pkPacket)
{
	BM::GUID kEventGuid;
	ELuckyStarResult kSelectStar = LSR_NONE;
	pkPacket->Pop(kEventGuid);
	pkPacket->Pop(kSelectStar);

	if(true != m_bLuckyStarEventRun)
	{
		return LSER_NOT_OPENED;
	}

	if(kEventGuid != m_kEventGuid)
	{
		return LSER_NOT_FOUND_EVENT;
	}

	if(LSR_AQUARIUS > kSelectStar || LSR_CAPRICORN < kSelectStar )
	{
		return LSER_NOT_SELECT_STAR;
	}

	size_t const kMaxUser = (m_kCurEvent.iUserLimit > 0 ? m_kCurEvent.iUserLimit : std::numeric_limits<int>().max());

	if(kMaxUser <= m_kContParticipant.size())
	{
		return LSER_PLAYER_FULL;
	}

	SContentsUser rkOut;
	if(S_FALSE == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
	{
		return LSER_NOT_FIND_USERINFO;
	}

	CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.find(rkOut.kMemGuid);
	if(m_kContParticipant.end() != member_iter)
	{
		CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;
		if(LSUS_JOIN == kMember.kState)
		{
			return LSER_ALREADY_JOINED;
		}
	}

	SLSP kIntoUser;
	kIntoUser.kCharGuid    = kCharGuid;
	kIntoUser.kState	   = LSUS_INTO;
	kIntoUser.kSelectStar  = kSelectStar;
	kIntoUser.iRewardCount = m_kCurEvent.iEventCount;
	auto kPair = m_kContParticipant.insert(std::make_pair(rkOut.kMemGuid,kIntoUser));
	if(!kPair.second)
	{
		kPair.first->second = kIntoUser;
	}

	CONT_PLAYER_MODIFY_ORDER kOrder;
	SPMOD_Add_Money kDelMoneyData(-m_kCurEvent.iCostMoney);	//이벤트 참여비용 빼기
	SPMO kIMO(IMET_ADD_MONEY, kCharGuid, kDelMoneyData);
	kOrder.push_back(kIMO);

	SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
	pkActionOrder->InsertTarget(kCharGuid);
	pkActionOrder->kGndKey.Set(rkOut.sChannel,rkOut.kGndKey);
	pkActionOrder->kCause = MCE_LuckyStar_CostMoney;
	pkActionOrder->kContOrder = kOrder;
	g_kJobDispatcher.VPush(pkActionOrder);

	return LSER_SUCCESS;
}

ELUCKYSTAR_EVENT_RESULT const PgLuckyStarEvent::OnRecvPT_M_I_REQ_LUCKYSTAR_CHANGE_STAR(BM::GUID const & kCharGuid,BM::Stream * const pkPacket,ELuckyStarResult & kSelectStar)
{
	BM::GUID kEventGuid;
	pkPacket->Pop(kEventGuid);
	pkPacket->Pop(kSelectStar);

	if(true != m_bLuckyStarEventRun)
	{
		return LSER_NOT_OPENED;
	}

	if(kEventGuid != m_kEventGuid)
	{
		return LSER_NOT_FOUND_EVENT;
	}

	if(m_kCurEvent.iLimitTime > static_cast<int>(m_dwNextEventTime))
	{
		return LSER_RESULT_MIN_AGO;
	}

	if(LSR_AQUARIUS > kSelectStar || LSR_CAPRICORN < kSelectStar )
	{
		return LSER_NOT_SELECT_STAR;
	}

	SContentsUser rkOut;
	if(S_FALSE == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
	{
		return LSER_NOT_FIND_USERINFO;
	}

	CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.find(rkOut.kMemGuid);
	if(m_kContParticipant.end() == member_iter)
	{
		return LSER_NOT_JOINED_EVENT;
	}

	CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;
	if(LSUS_JOIN != kMember.kState)
	{
		return LSER_NOT_JOINED_EVENT;
	}

	kMember.kSelectStar = kSelectStar;

	return LSER_SUCCESS;
}

bool PgLuckyStarEvent::Locked_RecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(kPacketType)
	{
	case PT_ANS_GMCMD_LUCKYSTAR_RELOAD:
		{
			bool bOrder = false;
			pkPacket->Pop(bOrder);
			if(true == bOrder)
			{
				SGmOrder kOrderData;
				kOrderData.ReadFromPacket(pkPacket);
				g_kGMProcessMgr.SendOrderState(kOrderData.kCmdGuid, OS_DONE);
			}
			
			m_kLuckyStarState = LSS_NONE;

			CONT_LUCKYSTAREVENTINFO const * pkCont = NULL;
			g_kTblDataMgr.GetContDef(pkCont);
			if(!pkCont)
			{
				return false;
			}

			m_kCopyEvent = *pkCont;
			PgLuckyStarEventUtil::ResetDailyEvent(m_kCopyEvent);
		}break;
	case PT_REQ_GMCMD_LUCKYSTAR_RELOAD:
		{
			if(LSS_NONE != m_kLuckyStarState)	// 이벤트 실행중이면 종료 처리 해준다.
			{
				ProcessEndEvent();
			}

			m_kLuckyStarState = LSS_RELOAD;

			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LUCKYSTAR_LOAD_EVENT, _T("EXEC [dbo].[up_LoadLuckyStarEvent]"));
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	case PT_ANS_LUCKYSTAR_LOAD_JOINEDUSER:
		{
			if(LSS_READY==m_kLuckyStarState)
			{
				m_bLuckyStarJoinedUserDBRead = true;
				CONT_LUCKYSTARJOINEDUSERINFO kContUser;
				pkPacket->Pop(kContUser);

				CONT_LUCKYSTARJOINEDUSERINFO::const_iterator joined_iter = kContUser.begin();
				while(kContUser.end()!=joined_iter)
				{
					SLSP kIntoUser;
					kIntoUser.kCharGuid    = (*joined_iter).second.kCharGuid;
					kIntoUser.kState	   = LSUS_LOADDB;
					kIntoUser.bCostMoney   = true;
					kIntoUser.kSelectStar  = (ELuckyStarResult)((*joined_iter).second.iStar);
					kIntoUser.iLuckyStep   = (*joined_iter).second.iStep;
					kIntoUser.kJoinTime.SetLocalTime();
					kIntoUser.iRewardCount = m_kCurEvent.iEventCount;
					auto kPair = m_kContParticipant.insert(std::make_pair((*joined_iter).first,kIntoUser));
					if(!kPair.second)
					{
						kPair.first->second = kIntoUser;
					}

					++joined_iter;
				}

				CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LUCKYSTAR_UPDATE_JOINEDUSER_READED, _T("EXEC [dbo].[up_Update_LuckyStarJoinedUserReaded]"));
				kQuery.PushStrParam(m_kCurEvent.iEventID);
				g_kCoreCenter.PushQuery(kQuery);
			}
		}break;
	case PT_REQ_GMCMD_LUCKYSTAR_UPDATE:
		{
			SGmOrder kOrderData;
			kOrderData.ReadFromPacket(pkPacket);
			g_kGMProcessMgr.SendOrderState(kOrderData.kCmdGuid, OS_DONE);

			int iSendResultType  = 0;
			int iFlag = 0;
			pkPacket->Pop(iSendResultType);
			pkPacket->Pop(iFlag);

			//결과 전송타입 변환
			if(-1 != iSendResultType)
			{
				if(1 == iSendResultType)
				{
					// 접속한 전 유저				
					m_kSendResultType = LSSRT_ALLUSER;
				}
				else
				{
					// 이벤트에 참여한 전 유저
					m_kSendResultType = LSSRT_PARTICIPATION;
				}

				UpdateEventSub(1, static_cast<int>(m_kSendResultType));
			}

			//이벤트 옵션
			if(-1 != iFlag)
			{
				m_kCurEvent.iFlag = m_kCurEvent.iFlag & iFlag;
			}
		}break;
	}

	if(LSS_RELOAD == m_kLuckyStarState)
	{
		return false;// 테이블 리로드중에는 아무것도 할 수 없다.
	}

	switch(kPacketType)
	{
	case PT_M_I_GMCMD_LUCKYSTAR_OPEN:
		{
			BM::GUID kGmCharGuid;
			int iEventID = 0;
			pkPacket->Pop(kGmCharGuid);
			pkPacket->Pop(iEventID);

			if(LSS_NONE != m_kLuckyStarState)
			{
				ProcessEndEvent();
			}

			if(true == GetStartEvent(iEventID))
			{
				m_kLuckyStarState = LSS_READY;

				g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 2338, EL_Normal, false );
			}
			else
			{
				g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 2339, EL_Warning, false );
			}
		}break;
	case PT_M_I_GMCMD_LUCKYSTAR_STEP:
		{
			BM::GUID kGmCharGuid;
			int iEventStep = 0;
			pkPacket->Pop(kGmCharGuid);
			pkPacket->Pop(iEventStep);
			if(0 == iEventStep)	//이벤트 종료
			{
				CONT_LUCKYSTAREVENTINFO::iterator iter = m_kCopyEvent.find(m_kCurEvent.iEventID);
				if(iter != m_kCopyEvent.end())
				{
					//이벤트 강제 종료시 자동실행 방지함
					(*iter).second.iFlag = m_kCurEvent.iFlag = m_kCurEvent.iFlag | 1;
				}	

				ProcessEndEvent();
			}
			else if(LSS_RESULT == static_cast<ELuckyStarState>(iEventStep)) //결과상태로 전환
			{
				if(LSS_OPEN == m_kLuckyStarState)
				{
					m_kLuckyStarState = LSS_RESULT;
					g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 2342, EL_Normal, false );
				}
			}
		}break;
	/*case PT_M_I_GMCMD_LUCKYSTAR_CLOSE:
		{
			ProcessEndEvent();

			BM::GUID kGmCharGuid;
			pkPacket->Pop(kGmCharGuid);
			g_kRealmUserMgr.Locked_SendWarnMessage( kGmCharGuid, 2340, EL_Normal, false );
		}break;*/
	case PT_NFY_LUCKYSTAR_LOGINUSER:
		{	//플레이어를 선택했을때 오는 메시지
			if(true == m_bLuckyStarEventRun)
			{
				BM::GUID kMemberGuid;
				BM::GUID kCharGuid;
				bool bJoinedUser = false;
				pkPacket->Pop(kMemberGuid);
				pkPacket->Pop(kCharGuid);
				pkPacket->Pop(bJoinedUser);

				bool bOldUser = false;

				CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.find(kMemberGuid);
				if(m_kContParticipant.end() != member_iter)
				{
					CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;
					
					//데이터가 있다는 것은
					//로그아웃을 하고 캐릭선택을 10분 초과하지 않은 유저 : 데이터 유지
					//로그아웃을 하고 캐릭선택창에서 캐릭을 바꾼 유저 : 초기화
					if(LSUS_LOGOUT == kMember.kState &&
						true == kMember.bCostMoney &&
						kMember.kCharGuid == kCharGuid)
					{
						const int iTempTime = BM::GetTime32() - kMember.dwLogoutTime;
						if(iTempTime < PgLuckyStarEventUtil::TEN_MINUTE)
						{
							bOldUser = true;
						}
					}
					else if(LSUS_LOADDB == kMember.kState)
					{
						if(kMember.kCharGuid == kCharGuid 
						&& (kMember.iRewardCount == m_kCurEvent.iEventCount || kMember.iRewardCount+1 == m_kCurEvent.iEventCount))
						{
							kMember.kState = LSUS_JOIN;
							bOldUser = true;
						}
					}

					if(false==bOldUser)
					{
						UpdateJoinedUser((*member_iter).first, kMember, 0);
						m_kContParticipant.erase(member_iter);
					}
				}

				if(false == bOldUser)
				{
					BM::Stream kPacket(PT_M_C_REQ_LUCKYSTAR_JOIN);
					kPacket.Push(m_kEventGuid);
					kPacket.Push(m_kCurEvent.iEventCount);
					kPacket.Push(m_kCurEvent.iDelayTime);
					kPacket.Push(m_kCurEvent.iLimitTime);
					kPacket.Push(m_kCurEvent.iCostMoney);
					kPacket.Push(m_kCurEvent.iLuckyPoint);
					kPacket.Push(bJoinedUser);
					g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);
				}
				else
				{
					CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;
					kMember.kState = LSUS_JOIN;
					kMember.bCostMoney = true;

					BM::Stream kPacket(PT_M_C_ANS_LUCKYSTAR_USERINFO);
					kPacket.Push(m_kEventGuid);
					kPacket.Push(m_kCurEvent.iEventCount);
					kPacket.Push(m_kCurEvent.iDelayTime);
					kPacket.Push(m_kCurEvent.iLimitTime);
					kPacket.Push(m_kCurEvent.iLuckyPoint);
					kPacket.Push(m_dwNextEventTime);
					kPacket.Push(kMember.kSelectStar);
					kPacket.Push(kMember.iLuckyStep);
					g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);

					//예를 들어 1:55분에 튕겨서 2:4분에 들어온 유저라면 보상 전송
					if(kMember.iRewardCount+1 == m_kCurEvent.iEventCount)
					{
						kMember.iRewardCount = m_kCurEvent.iEventCount;
						kMember.iLuckyStep = kMember.iLuckyStep + m_kCurEvent.iLuckyPoint;

						if(kMember.kSelectStar == m_kNowResultStar)
						{
							CONT_VEC_GUID	kContLuckyUser;
							kContLuckyUser.push_back((*member_iter).first);

							BM::Stream kPacket(PT_M_C_LUCKYSTAR_RESULT_CUSTOM);
							kPacket.Push(true);
							kPacket.Push(m_kCurEvent.iEventCount-1);
							kPacket.Push(kMember.iLuckyStep);
							g_kRealmUserMgr.Locked_SendToUser(kMember.kCharGuid, kPacket, false);

							ProcessReward(kContLuckyUser);
						}
						/*else
						{
							BM::Stream kPacket(PT_M_C_LUCKYSTAR_RESULT_CUSTOM);
							kPacket.Push(false);
							kPacket.Push(m_kCurEvent.iEventCount-1);
							kPacket.Push(kMember.iLuckyStep);

							g_kRealmUserMgr.Locked_SendToUser(kMember.kCharGuid, kPacket, false);
						}	*/				
					}	

					SContentsUser rkOut;
					if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
					{
						PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_User_ReEnter,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
						PgLog kLog;
						kLog.Set(0,m_kCurEvent.iEventID);
						kLog.Set(1,kMember.iRewardCount);
						kLog.Set(2,kMember.kSelectStar);
						kLog.Set(3,kMember.iLuckyStep);
						kLog.Set(2,m_kEventGuid.str());
						kContLog.Add(kLog);
						kContLog.Commit();
					}
				}
			}
		}break;
	case PT_NFY_LUCKYSTAR_LOGOUTUSER:
		{
			if(true == m_bLuckyStarEventRun)
			{
				BM::GUID kMemberGuid;
				BM::GUID kCharGuid;
				pkPacket->Pop(kMemberGuid);
				pkPacket->Pop(kCharGuid);

				CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.find(kMemberGuid);
				if(m_kContParticipant.end() != member_iter)
				{
                    CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;

					kMember.kState = LSUS_LOGOUT;
					kMember.dwLogoutTime = BM::GetTime32();
					kMember.kJoinTime.SetLocalTime();

                    UpdateJoinedUser((*member_iter).first, kMember, 1);
				}

				SContentsUser rkOut;
				if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
				{
					PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_User_Logout,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
					PgLog kLog;
					kLog.Set(0,m_kCurEvent.iEventID);
					kLog.Set(1,m_kCurEvent.iEventCount);
					kLog.Set(2,m_kEventGuid.str());
					kContLog.Add(kLog);
					kContLog.Commit();
				}
			}
		}break;
	case PT_M_I_REQ_LUCKYSTAR_ENTER:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);

			ELUCKYSTAR_EVENT_RESULT const kErr = OnRecvPT_M_I_REQ_LUCKYSTAR_ENTER(kCharGuid,pkPacket);

			if(LSER_SUCCESS != kErr)
			{
				BM::Stream kPacket(PT_M_C_ANS_LUCKYSTAR_ENTER);
				kPacket.Push(kErr);
				g_kRealmUserMgr.Locked_SendToUser(kCharGuid,kPacket,false);
			}
		}break;
	case PT_M_M_ANS_LUCKYSTAR_COSTMONEY:
		{
			BM::GUID kMemberGuid;
			BM::GUID kCharGuid;
			HRESULT kRet = 0;
			pkPacket->Pop(kMemberGuid);
			pkPacket->Pop(kCharGuid);
			pkPacket->Pop(kRet);

			CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.find(kMemberGuid);
			if(m_kContParticipant.end() != member_iter)
			{
				CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;

				if(S_OK == kRet)
				{
					kMember.kState = LSUS_JOIN;
					kMember.kJoinTime.SetLocalTime();
					kMember.bCostMoney = true;

					BM::Stream kPacket(PT_M_C_ANS_LUCKYSTAR_ENTER);
					kPacket.Push(LSER_SUCCESS);
					g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);

					BM::Stream kPacket2(PT_M_C_ANS_LUCKYSTAR_USERINFO);
					kPacket2.Push(m_kEventGuid);
					kPacket2.Push(m_kCurEvent.iEventCount);
					kPacket2.Push(m_kCurEvent.iDelayTime);
					kPacket2.Push(m_kCurEvent.iLimitTime);
					kPacket2.Push(m_kCurEvent.iLuckyPoint);
					kPacket2.Push(m_dwNextEventTime);
					kPacket2.Push(kMember.kSelectStar);
					kPacket2.Push(kMember.iLuckyStep);
					g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket2, false);

					UpdateJoinedUser((*member_iter).first, kMember, 1);

					SContentsUser rkOut;
					if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
					{
						PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_User_Enter,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
						PgLog kLog;
						kLog.Set(0,m_kCurEvent.iEventID);
						kLog.Set(1,m_kCurEvent.iCostMoney);
						kLog.Set(2,kMember.kSelectStar);
						kLog.Set(2,m_kEventGuid.str());
						kContLog.Add(kLog);
						kContLog.Commit();
					}
				}
				else
				{
					BM::Stream kPacket(PT_M_C_ANS_LUCKYSTAR_ENTER);
					kPacket.Push(LSER_HAVE_NOT_ENOUGH_MONEY);
					g_kRealmUserMgr.Locked_SendToUser(kCharGuid, kPacket, false);

					if(kMember.iLuckyStep < 1)
					{
						UpdateJoinedUser((*member_iter).first, kMember, 0);
						m_kContParticipant.erase(member_iter);
					}
				}
			}
			else
			{
				if(S_OK == kRet)
				{
					//유저 정보가 없는데 돈 소비가 이루어진다면
					//문제가 있는것임, 테스트할때만 막아놓기, 로그로 전환
					//__asm int 3;
				}
			}
		}break;
	case PT_M_I_REQ_LUCKYSTAR_CHANGE_STAR:
		{
			BM::GUID kCharGuid;
			ELuckyStarResult kSelectStar = LSR_NONE;
			pkPacket->Pop(kCharGuid);

			ELUCKYSTAR_EVENT_RESULT kErr = OnRecvPT_M_I_REQ_LUCKYSTAR_CHANGE_STAR(kCharGuid,pkPacket,kSelectStar);
			BM::Stream kPacket(PT_M_C_ANS_LUCKYSTAR_CHANGE_STAR);
			kPacket.Push(kErr);
			kPacket.Push(kSelectStar);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid,kPacket,false);
		}break;
	default:
		{
			return false;
		}break;
	}

	return true;
}

bool PgLuckyStarEvent::IsNewEvent()const
{
    CONT_LUCKYSTAREVENTINFO::const_iterator iter = m_kCopyEvent.find(LAST_EVENTID);
	if(iter == m_kCopyEvent.end())
	{
		return true;
	}

    SLUCKYSTAREVENTINFO const& kLastEvent = (*iter).second;
    if( (kLastEvent.iCostMoney == m_kCurEvent.iCostMoney)
        &&  (kLastEvent.iLuckyPoint == m_kCurEvent.iLuckyPoint)
        &&  (kLastEvent.kStartDate == m_kCurEvent.kStartDate)
        &&  (kLastEvent.kEndDate == m_kCurEvent.kEndDate) )
    {
        return false;
    }

    return true;
}

bool PgLuckyStarEvent::BuildEvent(CONT_LUCKYSTAREVENTINFO::mapped_type const & kEventInfo)
{
	ClearEvent();
	m_kEventGuid = BM::GUID::Create();
	m_kCurEvent = kEventInfo;
	m_kNowResultStar = static_cast<ELuckyStarResult>(m_kCurEvent.iLastResultStar);

	if(0 > m_kCurEvent.iDelayTime)
	{
		m_kCurEvent.iDelayTime = 0;
		m_kCurEvent.iLimitTime = PgLuckyStarEventUtil::ONE_HOUR - (m_kCurEvent.iLimitTime * 1000);
	}
	else
	{
		m_kCurEvent.iLimitTime = (m_kCurEvent.iDelayTime-m_kCurEvent.iLimitTime) * 1000;
	}

	if(0 > m_kCurEvent.iLimitTime)
	{
		m_kCurEvent.iLimitTime = 0;
	}

	int iNextRewardId = 0;
	{
		CONT_LUCKYSTAREVENTJOINREWARD const * pkContEventJoinReward = NULL;
		g_kTblDataMgr.GetContDef(pkContEventJoinReward);
		if(!pkContEventJoinReward)
		{
			return false;
		}

		CONT_LUCKYSTAREVENTJOINREWARD::const_iterator join_iter = (*pkContEventJoinReward).find(m_kCurEvent.iEventID);
		if((*pkContEventJoinReward).end() != join_iter)
		{
			CONT_LUCKYSTAREVENTJOINREWARD::mapped_type const & kContJoin = (*join_iter).second;
			
			CONT_LUCKYSTAREVENTJOINREWARD::mapped_type::const_iterator iter = 
				std::find_if(kContJoin.begin(), kContJoin.end(), PgLuckyStarEventUtil::SNextRewardIdFind(m_kCurEvent.iLastRewardId));

			if(kContJoin.end() != iter)
			{
				iNextRewardId = *iter;
			}
			else
			{
				iter = kContJoin.begin();
				if(kContJoin.end() != iter)
				{
					iNextRewardId = *iter;
				}
			}	
		}
	}

	m_kCurEvent.iLastRewardId = iNextRewardId;

	{
		CONT_LUCKYSTARREWARDSTEP const * pkContRewardStep = NULL;
		g_kTblDataMgr.GetContDef(pkContRewardStep);
		if(!pkContRewardStep)
		{
			return false;
		}

		CONT_LUCKYSTARREWARDSTEP::key_type kKey(iNextRewardId);
		CONT_LUCKYSTARREWARDSTEP::const_iterator step_iter = (*pkContRewardStep).find(kKey);
		if((*pkContRewardStep).end() != step_iter)
		{
			CONT_LUCKYSTARREWARD const * pkContReward = NULL;
			g_kTblDataMgr.GetContDef(pkContReward);
			if(!pkContReward)
			{
				return false;
			}

			CONT_LUCKYSTARREWARD const & kReward = (*pkContReward);

			CONT_LUCKYSTARREWARDSTEP::mapped_type const & kContStep = (*step_iter).second;
			SLSRM kStepReward;
			for(CONT_LUCKYSTARREWARDSTEP::mapped_type::const_iterator iter = kContStep.begin();
				kContStep.end() != iter; ++iter)
			{
				kStepReward.iStep = (*iter);

				CONT_LUCKYSTARREWARD::key_type kKey(iNextRewardId, kStepReward.iStep);
				CONT_LUCKYSTARREWARD::const_iterator reward_iter = kReward.find(kKey);
				if(kReward.end() == reward_iter)
				{
					continue;
				}

				kStepReward.kContReward = (*reward_iter).second;
				m_kRewardMgr.push_back(kStepReward);
			}
		}
	}

    if( IsNewEvent() )
	{
        //새로운이벤트인경우 기존 유저정보 받지 않음
        m_bLuckyStarJoinedUserDBRead = true;
        m_kCurEvent.iEventCount = 0;
	}
    else
    {
        // 저장된 유저 정보 읽기
		CEL::DB_QUERY kQuery( DT_PLAYER, DQT_LUCKYSTAR_LOAD_JOINEDUSER, _T("EXEC [dbo].[UP_Load_LuckyStarJoinedUser]"));
		kQuery.PushStrParam(m_kCurEvent.iEventID);
		g_kCoreCenter.PushQuery(kQuery);
	}

	if(m_kCurEvent.iEventCount < 1)
	{
		m_kCurEvent.iEventCount = 0;
		++m_kCurEvent.iEventCount;

		UpdateEventSub(3, m_kCurEvent.iEventCount);
	}
	
	UpdateEventSub(2, iNextRewardId);
    UpdateLastEvent();
	INFO_LOG( BM::LOG_LV6, __FL__ << _T("LuckyStarEvent BuildEvent[") << m_kCurEvent.iEventID << _T("],LastRewardId: ") << m_kCurEvent.iLastRewardId );
	return true;
}

void PgLuckyStarEvent::UpdateLastEvent()
{
    BM::CAutoMutex kLock(m_kMutex);
    CONT_LUCKYSTAREVENTINFO::const_iterator iter = m_kCopyEvent.find(m_kCurEvent.iEventID);
	if(iter == m_kCopyEvent.end())
	{
		return;
	}

    SLUCKYSTAREVENTINFO const& kLastEvent = (*iter).second;

    BM::DBTIMESTAMP_EX const dtStart(kLastEvent.kStartDate);
	BM::DBTIMESTAMP_EX const dtEnd(kLastEvent.kEndDate);

    CEL::DB_QUERY kQuery( DT_LOCAL, DQT_LUCKYSTAR_UPDATE_LASTEVENT, _T("EXEC [dbo].[up_UpdateLastLuckyStarEvent]"));
    kQuery.InsertQueryTarget(m_kEventGuid);
	kQuery.QueryOwner(m_kEventGuid);
	kQuery.PushStrParam(kLastEvent.iUserLimit);
    kQuery.PushStrParam(kLastEvent.iCostMoney);
    kQuery.PushStrParam(dtStart);
    kQuery.PushStrParam(dtEnd);
    kQuery.PushStrParam(kLastEvent.iDelayTime);
    kQuery.PushStrParam(kLastEvent.iLimitTime);
    kQuery.PushStrParam(kLastEvent.iLuckyPoint);
    g_kCoreCenter.PushQuery(kQuery);
}

bool PgLuckyStarEvent::GetStartEvent(int const iEventID)
{
	if( true == m_kCopyEvent.empty() )
	{
		CONT_LUCKYSTAREVENTINFO const * pkCont = NULL;
		g_kTblDataMgr.GetContDef(pkCont);
		if(!pkCont)
		{
			return false;
		}

		m_kCopyEvent = *pkCont;
		PgLuckyStarEventUtil::ResetDailyEvent(m_kCopyEvent);
	}

	if(0 != iEventID)
	{
		CONT_LUCKYSTAREVENTINFO::const_iterator iter = m_kCopyEvent.find(iEventID);
		if(iter == m_kCopyEvent.end())
		{
			return false;
		}
		return BuildEvent((*iter).second);
	}
	else
	{
		BM::DBTIMESTAMP_EX kSysTime;
		g_kEventView.GetLocalTime(kSysTime);
		BM::PgPackedTime const kCurTime(kSysTime);

		CONT_LUCKYSTAREVENTINFO::iterator iter = m_kCopyEvent.begin();
		while(iter != m_kCopyEvent.end())
		{
			CONT_LUCKYSTAREVENTINFO::mapped_type& kEvent = (*iter).second;

            if(LAST_EVENTID != kEvent.iEventID)
			if(false == (1&kEvent.iFlag))	//1: 자동으로 이벤트 실행 방지 기능
			{
				bool bBuildEvent = false;
				bBuildEvent = PgLuckyStarEventUtil::IsInDateTime(kEvent.kStartDate, kEvent.kEndDate, kCurTime);

				if( bBuildEvent )
				{
					if( LSS_NONE == kEvent.bState )
					{
						return BuildEvent(kEvent);
					}
				}
			}			

			++iter;
		}
	}
	return false;
}



void PgLuckyStarEvent::LogoutUserCheck(DWORD dwCurTime)
{
	CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.begin();
	while(member_iter != m_kContParticipant.end())
	{
		CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;

		bool bErase = false;
		if(LSUS_LOGOUT == kMember.kState)
		{
			const int iTempTime = dwCurTime - kMember.dwLogoutTime;
			if(iTempTime > PgLuckyStarEventUtil::ONE_HOUR)
			{
				UpdateJoinedUser((*member_iter).first, kMember, 0);
				bErase = true;
			}
		}

		if(true==bErase)
		{
			m_kContParticipant.erase(member_iter++);
		}
		else
		{
			++member_iter;
		}
	}
}

void PgLuckyStarEvent::SendResultCustomPacket(CONT_VEC_GUID const & kContUser, bool bResult) const
{
	for(CONT_VEC_GUID::const_iterator key_iter = kContUser.begin();
		key_iter != kContUser.end(); ++key_iter)
	{
		CONT_LUCKYSTAR_PARTICIPANT::const_iterator member_iter = m_kContParticipant.find(*key_iter);
		if(m_kContParticipant.end() == member_iter)
		{
			continue;
		}

		CONT_LUCKYSTAR_PARTICIPANT::mapped_type const & kMember = (*member_iter).second;

		BM::Stream kPacket(PT_M_C_LUCKYSTAR_RESULT_CUSTOM);
		kPacket.Push(bResult);
		kPacket.Push(m_kCurEvent.iEventCount);
		kPacket.Push(kMember.iLuckyStep);

		g_kRealmUserMgr.Locked_SendToUser(kMember.kCharGuid, kPacket, false);
	}
}

void PgLuckyStarEvent::Locked_Tick(const DWORD dwCurTime)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(m_kLuckyStarState)
	{
	case LSS_NONE:
		{
			if(true == GetStartEvent(0))
			{
				m_kLuckyStarState = LSS_READY;
			}
		}break;
	case LSS_READY:
		{
			if(true == m_bLuckyStarJoinedUserDBRead)
			{
				m_dwNextEventTime = PgLuckyStarEventUtil::GetNextEventTime(m_kCurEvent.iDelayTime);
				m_dwOldEventTime  = BM::GetTime32();

				m_kLuckyStarState    = LSS_OPEN;
				m_bLuckyStarEventRun = true;

				UpdateEventSub(0, static_cast<int>(m_kLuckyStarState));

				{// 이벤트 시작 로그 기록
					PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_Open);
					PgLog kLog;
					kLog.Set(0,m_kCurEvent.iEventID);
					kLog.Set(1,m_kCurEvent.iCostMoney);
					kLog.Set(2,m_kCurEvent.iLuckyPoint);
					kLog.Set(3,m_kCurEvent.iDelayTime / 1000);
					kLog.Set(4,(m_kCurEvent.iDelayTime - m_kCurEvent.iLimitTime) / 1000);
					kLog.Set(2,m_kEventGuid.str());
					kContLog.Add(kLog);
					kContLog.Commit();
				}

				//기존에 접속한 유저에게 이벤트 참여 정보 전송
				g_kRealmUserMgr.Locked_SendToLoginPlayer_LuckyStarOpen();
			}
		}break;
	case LSS_OPEN:
		{
			if(m_dwNextEventTime > 0)
			{
				int tmp = m_dwNextEventTime - (dwCurTime - m_dwOldEventTime);
				tmp > 0 ? m_dwNextEventTime = tmp : m_dwNextEventTime = 0;

				m_dwOldEventTime = dwCurTime;
			}

			if(0 >= m_dwNextEventTime)
			{
				m_dwNextEventTime = 0;
				m_kLuckyStarState = LSS_RESULT;
			}
		}break;
	case LSS_RESULT:
		{
			m_kNowResultStar = PgLuckyStarEventUtil::ResultLuckyStar();

			//이벤트시간 체크
			if(true == PgLuckyStarEventUtil::IsLastEvent(m_kCurEvent.kEndDate, m_kCurEvent.iDelayTime))
			{
				m_dwNextEventTime = 0;
			}
			else
			{
				//다음 이벤트 시작 설정
				m_dwNextEventTime = PgLuckyStarEventUtil::GetNextEventTime(m_kCurEvent.iDelayTime);
				m_dwOldEventTime  = BM::GetTime32();
			}

			LogoutUserCheck(dwCurTime);

			//m_kCurEvent.iEventCount를 여기서 수정하지 않는 이유는 현재카운트를 유지하다 LSS_UPDATE_EVENT에서 수정해야 하기때문
			int iEventCount = m_kCurEvent.iEventCount+1; 

			m_kContLuckyUser.clear();
			m_kContNomalUser.clear();

			CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.begin();
			while(member_iter != m_kContParticipant.end())
			{
				CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;

				if(LSUS_JOIN == kMember.kState)
				{
					if(kMember.kSelectStar == m_kNowResultStar)
					{
						m_kContLuckyUser.push_back((*member_iter).first);
					}
					else
					{
						m_kContNomalUser.push_back((*member_iter).first);
					}

					kMember.iRewardCount = iEventCount;	//보상받았다구 남기기
					kMember.iLuckyStep = kMember.iLuckyStep + m_kCurEvent.iLuckyPoint;	//참여중인 유저들의 행운성 지수 올리기
				}

				++member_iter;
			}

			int iParticipantSize = static_cast<int>(m_kContParticipant.size());
			int iLuckyUserSize = static_cast<int>(m_kContLuckyUser.size());

			BM::Stream kBPacket(PT_M_C_LUCKYSTAR_RESULT);
			kBPacket.Push(m_kCurEvent.iEventCount);
			kBPacket.Push(m_kBeforResultStar);
			kBPacket.Push(m_kNowResultStar);
			kBPacket.Push(iLuckyUserSize);
			kBPacket.Push(iParticipantSize);
			kBPacket.Push(m_dwNextEventTime);

			if(LSSRT_ALLUSER == m_kSendResultType)
			{
				g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kBPacket);
			}
			else
			{
				BroadCast(kBPacket);				
			}

			m_kLuckyStarState = LSS_RESULT_CUSTOM;
		}break;
	case LSS_RESULT_CUSTOM:
		{
			SendResultCustomPacket(m_kContLuckyUser, true);
			SendResultCustomPacket(m_kContNomalUser, false);

			ProcessReward(m_kContLuckyUser); // 행운을 가진 유저에게 보상 전송	

			m_kLuckyStarState = LSS_UPDATE_EVENT;
		}break;
	case LSS_UPDATE_EVENT:
		{
			m_kBeforResultStar = m_kNowResultStar;
			m_kCurEvent.iLastResultStar = static_cast<int>(m_kNowResultStar);
			++m_kCurEvent.iEventCount;

			UpdateEventSub(4, m_kCurEvent.iLastResultStar);
			UpdateEventSub(3, m_kCurEvent.iEventCount);

			CONT_LUCKYSTAR_PARTICIPANT::iterator member_iter = m_kContParticipant.begin();
			while(member_iter != m_kContParticipant.end())
			{
				CONT_LUCKYSTAR_PARTICIPANT::mapped_type & kMember = (*member_iter).second;

				if(LSUS_JOIN == kMember.kState)
				{
					UpdateJoinedUser((*member_iter).first, kMember, 1);
				}

				++member_iter;
			}

			//
			PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_Event_Log,m_kEventGuid);
			PgLog kLog;
			kLog.Set(0,m_kCurEvent.iEventID);
			kLog.Set(1,m_kNowResultStar);
			kLog.Set(2,static_cast<int>(m_kContLuckyUser.size()));
			kLog.Set(3,static_cast<int>(m_kContParticipant.size()));
			kLog.Set(1,m_kEventGuid.str());
			kContLog.Add(kLog);
			kContLog.Commit();


			if(0 == m_dwNextEventTime)
			{
				//마지막 이벤트이면 종료
				ProcessEndEvent();
			}
			else
			{
				//다음 이벤트 시작
				m_kLuckyStarState = LSS_OPEN;
			}
		}break;
	}
}

//행운지수에 따라서 아이템 차등 반환
CONT_LUCKYSTARREWARD::mapped_type PgLuckyStarEvent::GetCurrentReward(int const iStep)
{
	CONT_REWARD_CHARACTER::const_reverse_iterator riter =
		std::find_if(m_kRewardMgr.rbegin(), m_kRewardMgr.rend(), PgLuckyStarEventUtil::SRewardStepFind(iStep));
	if(m_kRewardMgr.rend() != riter)
	{
		return (*riter).kContReward;
	}

	static const CONT_LUCKYSTARREWARD::mapped_type kContEmpty;
	return kContEmpty;
}

void PgLuckyStarEvent::ProcessReward(CONT_VEC_GUID const & kVec)
{
	std::wstring kFrom;
	GetDefString(LUCK_STAR_RESULT_FROM,kFrom);

	for(CONT_VEC_GUID::const_iterator key_iter = kVec.begin();
		kVec.end() != key_iter; ++key_iter)
	{
		CONT_LUCKYSTAR_PARTICIPANT::const_iterator lucky_iter = m_kContParticipant.find(*key_iter);
		if(lucky_iter == m_kContParticipant.end())
		{
			continue;
		}

		CONT_LUCKYSTAR_PARTICIPANT::mapped_type const & kLucky = (*lucky_iter).second;
		CONT_LUCKYSTARREWARD::mapped_type const & kContReward =  GetCurrentReward(kLucky.iLuckyStep);	

		CONT_LUCKYSTARREWARD::mapped_type::const_iterator reward_iter = kContReward.begin();
		while(kContReward.end() != reward_iter)
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
			pkActionOrder->kPacket2nd.Push(kLucky.kCharGuid);
			pkActionOrder->kPacket2nd.Push(std::wstring());
			pkActionOrder->kPacket2nd.Push(m_kCurEvent.kMailTitle);
			pkActionOrder->kPacket2nd.Push(m_kCurEvent.kMailText);
			pkActionOrder->kPacket2nd.Push((*reward_iter).iItemNo);
			pkActionOrder->kPacket2nd.Push((*reward_iter).iItemCount);
			pkActionOrder->kPacket2nd.Push((*reward_iter).iMoney);
			pkActionOrder->kPacket2nd.Push(kFrom);

			g_kJobDispatcher.VPush(pkActionOrder);

			++reward_iter;
		}

		// 로그 남기기
		SContentsUser rkOut;
		if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kLucky.kCharGuid,false,rkOut))
		{
			PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_User_Log,rkOut.kMemGuid,kLucky.kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
			PgLog kLog;
			kLog.Set(0,m_kCurEvent.iEventID);
			kLog.Set(1,kLucky.kSelectStar);
			kLog.Set(2,m_kCurEvent.iLastRewardId);
			kLog.Set(3,kLucky.iLuckyStep);
			kLog.Set(1,m_kEventGuid.str());
			kContLog.Add(kLog);
			kContLog.Commit();
		}
	}
}

void PgLuckyStarEvent::ProcessEndEvent()
{
	BM::Stream kBPacket(PT_M_C_LUCKYSTAR_CLOSE);
	kBPacket.Push(m_kEventGuid);
	kBPacket.Push(m_kCurEvent.kEventNotiClose);
	g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kBPacket);
    //BroadCast(kPacket);
	/*CONT_LUCKYSTAREVENTINFO::iterator iter = m_kCopyEvent.find(m_kCurEvent.iEventID);
	if(iter != m_kCopyEvent.end())
	{
		(*iter).second = m_kCurEvent;
	}*/
	
	{// 이벤트 종료 로그 기록
		PgLogCont kContLog(ELogMain_LuckyStar,ELogSub_LuckyStar_Close);
		PgLog kLog;
		kLog.Set(0,m_kCurEvent.iEventID);
		kLog.Set(2,m_kEventGuid.str());
		kContLog.Add(kLog);
		kContLog.Commit();
	}

	// 이벤트 상태 바꾸기
	m_kLuckyStarState    = LSS_NONE;
	UpdateEventSub(0, static_cast<int>(m_kLuckyStarState));

	ClearEvent();
}
