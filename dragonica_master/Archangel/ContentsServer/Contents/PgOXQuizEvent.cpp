#include "stdafx.h"
#include "PgOXQuizEvent.h"
#include "PgServerSetMgr.h"
#include "Item/PgPostManager.h"
#include "JobWorker.h"
#include "jobdispatcher.h"
#include "pgGMProcessMgr.h"
#include "Global.h"

namespace PgOXQuizEventUtil
{
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

	bool IsInOnlyDate(BM::PgPackedTime kStart, BM::PgPackedTime kEnd, BM::PgPackedTime kCur)
	{
		ClearTime(kStart);
		ClearTime(kEnd);
		ClearTime(kCur);
		return IsInDateTime(kStart, kEnd, kCur);
	}

	bool IsInOnlyTime(BM::PgPackedTime kStart, BM::PgPackedTime kEnd, BM::PgPackedTime kCur)
	{
		ClearDate(kStart);
		ClearDate(kEnd);
		ClearDate(kCur);
		return IsInDateTime(kStart, kEnd, kCur);
	}

	void ResetDailyEvent(CONT_OXQUIZEVENTINFO &rkCopyEvent)
	{
		CONT_OXQUIZEVENTINFO::iterator iter = rkCopyEvent.begin();
		while( rkCopyEvent.end() != iter )
		{
			CONT_OXQUIZEVENTINFO::mapped_type& rkElement = (*iter).second;
			if( 0 != rkElement.bRepeat )
			{
				rkElement.bState = OXES_NONE; // 리로드시에 매일 반복 되는 이벤트는 강제로 초기화
			}
			++iter;
		}
	}
};

PgOXQuizEvent::PgOXQuizEvent()
{
	ClearEvent();
	m_kLastTickDateTime.Clear();
}

void PgOXQuizEvent::ClearEvent()
{
	m_kContParticipant.clear();
	m_kRewardMgr.clear();
	m_kContQuiz.clear();
	m_kCurEvent.Clear();
	m_kQuizState = OXQS_NONE;
	m_iQuizLevel = 0;
	m_kContParticipant.clear();
	m_kCurQuizResult = OXQR_NONE;
	m_i64StateTime = g_kEventView.GetLocalSecTime();
}

void PgOXQuizEvent::BroadCast(BM::Stream const & rkPacket)
{
	std::vector<BM::GUID> kVec;
	for(CONT_QUIZ_PARTICIPANT::const_iterator iter = m_kContParticipant.begin();iter != m_kContParticipant.end();++iter)
	{
		kVec.push_back((*iter).first);
	}
	g_kRealmUserMgr.Locked_SendToUser(kVec, rkPacket, false);
}

EOXQUIZ_EVENT_RESULT const PgOXQuizEvent::OnRecvPT_M_I_REQ_OXQUIZ_ENTER(BM::GUID const & kCharGuid,BM::Stream * const pkPacket)
{
	BM::GUID kEventGuid;
	pkPacket->Pop(kEventGuid);

	if(OXQS_ENTER_PARTICIPANT != m_kQuizState)
	{
		return OXQES_NOT_OPENED;
	}

	if(kEventGuid != m_kEventGuid)
	{
		return OXQES_NOT_FOUND_EVENT;
	}

	size_t const kMaxUser = (m_kCurEvent.iUserMin > 0 ? m_kCurEvent.iUserMin : std::numeric_limits<int>().max());

	if(kMaxUser <= m_kContParticipant.size())
	{
		return OXQES_PLAYER_FULL;
	}

	auto kPair = m_kContParticipant.insert(std::make_pair(kCharGuid,OXQR_NONE));
	if(!kPair.second)
	{
		return OXQES_ALREADY_JOINED;
	}

	SContentsUser rkOut;
	if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
	{
		PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_User_Enter,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
		PgLog kLog;
		kLog.Set(0,m_kCurEvent.iEventID);
		kLog.Set(2,m_kEventGuid.str());
		kContLog.Add(kLog);
		kContLog.Commit();
	}

	return OXQES_SUCCESS;
}

EOXQUIZ_EVENT_RESULT const PgOXQuizEvent::OnRecvPT_M_I_REQ_OXQUIZ_ANS_QUIZ(BM::GUID const & kCharGuid,BM::Stream * const pkPacket)
{
	BM::GUID kEventGuid;
	EOXQiuzResult kResult;
	pkPacket->Pop(kEventGuid);
	pkPacket->Pop(kResult);

	if(OXQS_QUIZ_RESULT != m_kQuizState)
	{
		return OXQES_NOT_QUIZ_TIME;
	}

	CONT_QUIZ_PARTICIPANT::iterator iter = m_kContParticipant.find(kCharGuid);
	if(iter == m_kContParticipant.end())
	{
		return OXQES_NOT_JOINED_EVENT;
	}

	(*iter).second = kResult;

	return OXQES_SUCCESS;
}

EOXQUIZ_EVENT_RESULT const PgOXQuizEvent::OnRecvPT_M_I_REQ_OXQUIZ_EXIT(BM::GUID const & kCharGuid)
{
	CONT_QUIZ_PARTICIPANT::iterator iter = m_kContParticipant.find(kCharGuid);
	if(iter == m_kContParticipant.end())
	{
		return OXQES_NOT_JOINED_EVENT;
	}

	m_kContParticipant.erase(iter);

	SContentsUser rkOut;
	if(S_OK == g_kRealmUserMgr.Locked_GetPlayerInfo(kCharGuid,false,rkOut))
	{
		PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_User_Exit,rkOut.kMemGuid,kCharGuid,rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
		PgLog kLog;
		kLog.Set(0,m_kCurEvent.iEventID);
		kLog.Set(1,m_iQuizLevel);
		kLog.Set(2,m_kEventGuid.str());
		kContLog.Add(kLog);
		kContLog.Commit();
	}

	return OXQES_SUCCESS;
}

bool PgOXQuizEvent::Locked_RecvMessage(BM::Stream::DEF_STREAM_TYPE const kPacketType,SERVER_IDENTITY const & kSI,SGroundKey const & kGndKey, BM::Stream * const pkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);

	switch(kPacketType)
	{
	case PT_ANS_GMCMD_OXQUIZ_RELOAD:
		{
			SGmOrder kOrderData;
			kOrderData.ReadFromPacket(pkPacket);
			g_kGMProcessMgr.SendOrderState(kOrderData.kCmdGuid, OS_DONE);
			m_kQuizState = OXQS_NONE;

			{
				TABLE_LOCK(CONT_OXQUIZEVENTINFO) kObjLock;
				g_kTblDataMgr.GetContDef(kObjLock);
				CONT_OXQUIZEVENTINFO const * pkCont = kObjLock.Get();
				if(!pkCont)
				{
					return false;
				}
				m_kCopyEvent = *pkCont;
			}
			PgOXQuizEventUtil::ResetDailyEvent(m_kCopyEvent);
		}break;
	case PT_REQ_GMCMD_OXQUIZ_RELOAD:
		{
			if(OXQS_NONE != m_kQuizState)	// 이벤트 실행중이면 종료 처리 해준다.
			{
				BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_CLOSE);
				kPacket.Push(m_kEventGuid);
				BroadCast(kPacket);
			}

			ClearEvent();

			m_kQuizState = OXQS_QUIZ_RELOAD;

			CEL::DB_QUERY kQuery( DT_LOCAL, DQT_OXQUIZ_EVENT, _T("EXEC [dbo].[up_LoadOXQuizEvent]"));
			kSI.WriteToPacket(kQuery.contUserData);
			kQuery.contUserData.Push(kGndKey);
			kQuery.contUserData.Push(*pkPacket);

			g_kCoreCenter.PushQuery(kQuery);
		}break;
	}

	if(OXQS_QUIZ_RELOAD == m_kQuizState)
	{
		return false;// 테이블 리로드중에는 아무것도 할 수 없다.
	}

	switch(kPacketType)
	{
	case PT_M_I_GMCMD_OXQUIZ_OPEN:
		{
			int iEventID = 0;
			pkPacket->Pop(iEventID);
			GetStartEvent(iEventID);
		}break;
	case PT_M_I_GMCMD_OXQUIZ_STEP:
		{
			int iEventStep = 0;
			int iEventLevel = 0;
			pkPacket->Pop(iEventStep);
			m_kQuizState = static_cast<EOXQuizState>(iEventStep);
		}break;
	case PT_M_I_REQ_OXQUIZ_ENTER:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);

			EOXQUIZ_EVENT_RESULT const kErr = OnRecvPT_M_I_REQ_OXQUIZ_ENTER(kCharGuid,pkPacket);
			BM::Stream kPacket(PT_M_C_ANS_OXQUIZ_ENTER);
			kPacket.Push(kErr);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid,kPacket,false);
		}break;
	case PT_M_I_REQ_OXQUIZ_EXIT:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);

			EOXQUIZ_EVENT_RESULT kErr = OnRecvPT_M_I_REQ_OXQUIZ_EXIT(kCharGuid);
			BM::Stream kPacket(PT_M_C_ANS_OXQUIZ_ENTER);
			kPacket.Push(kErr);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid,kPacket,false);
		}break;
	case PT_M_I_REQ_OXQUIZ_ANS_QUIZ:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop(kCharGuid);

			EOXQUIZ_EVENT_RESULT const kErr = OnRecvPT_M_I_REQ_OXQUIZ_ANS_QUIZ(kCharGuid,pkPacket);
			BM::Stream kPacket(PT_M_C_ANS_OXQUIZ_ANS_QUIZ);
			kPacket.Push(kErr);
			g_kRealmUserMgr.Locked_SendToUser(kCharGuid,kPacket,false);
		}break;
	default:
		{
			return false;
		}break;
	}

	return true;
}

bool PgOXQuizEvent::BuildEvent(CONT_OXQUIZEVENTINFO::mapped_type const & kEventInfo)
{
	TABLE_LOCK(CONT_OXQUIZINFO) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_OXQUIZINFO const * pkContQuiz = kObjLock.Get();
	if(!pkContQuiz)
	{
		return false;
	}

	ClearEvent();
	m_kQuizState = OXQS_QUIZ_NOTI_01;
	m_kEventGuid = BM::GUID::Create();
	m_kCurEvent = kEventInfo;

	int const iQuizMin = std::min(m_kCurEvent.iQuizNoMin,m_kCurEvent.iQuizNoMax);
	int const iQuizMax = std::max(m_kCurEvent.iQuizNoMin,m_kCurEvent.iQuizNoMax);

	CONT_OXQUIZINFO::const_iterator q_start = pkContQuiz->find(iQuizMin);
	CONT_OXQUIZINFO::const_iterator q_end = pkContQuiz->find(iQuizMax);

	if( (q_start == pkContQuiz->end())	||
		(q_end == pkContQuiz->end())	||
		(q_start == q_end))
	{
		return false;
	}

	++q_end;

	std::set<int> kContTable;
	int kCount = 0;
	for(CONT_OXQUIZINFO::const_iterator iter = q_start;iter != q_end;++iter)
	{
		++kCount;
	}

	m_kContQuiz.resize(kCount);

	for(CONT_OXQUIZINFO::const_iterator iter = q_start;iter != q_end;++iter)
	{
		while(true)
		{
			int kIndex = BM::Rand_Index(kCount);
			if(kContTable.insert(kIndex).second)
			{
				m_kContQuiz[kIndex] = (*iter).second;
				break;
			}
		}
	}

	return true;
}

bool PgOXQuizEvent::GetStartEvent(int const iEventID)
{
	if( true == m_kCopyEvent.empty() )
	{
		TABLE_LOCK(CONT_OXQUIZEVENTINFO) kObjLock;
		g_kTblDataMgr.GetContDef(kObjLock);
		CONT_OXQUIZEVENTINFO const * pkCont = kObjLock.Get();
		if(!pkCont)
		{
			return false;
		}

		m_kCopyEvent = *pkCont;
		PgOXQuizEventUtil::ResetDailyEvent(m_kCopyEvent);
	}

	if(0 != iEventID)
	{
		CONT_OXQUIZEVENTINFO::const_iterator iter = m_kCopyEvent.find(iEventID);
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

		if( m_kLastTickDateTime.Day() != kCurTime.Day() )
		{
			PgOXQuizEventUtil::ResetDailyEvent(m_kCopyEvent);
			m_kLastTickDateTime = kCurTime;
		}

		for(CONT_OXQUIZEVENTINFO::iterator iter = m_kCopyEvent.begin();iter != m_kCopyEvent.end();++iter)
		{
			CONT_OXQUIZEVENTINFO::mapped_type& kEvent = (*iter).second;

			bool bBuildEvent = false;
			if( 0 != kEvent.bRepeat )
			{
				// 정해진 기간만큼내에서, 정해진 시간에만 매일 반복
				// 취소 되었더라도 리로드 하면 시간내일 때 재시작
				if( OXES_NONE == kEvent.bState )
				{
					bBuildEvent = PgOXQuizEventUtil::IsInOnlyDate(kEvent.kStartDate, kEvent.kEndDate, kCurTime)
								&& PgOXQuizEventUtil::IsInOnlyTime(kEvent.kStartDate, kEvent.kEndDate, kCurTime);
					kEvent.bState = OXES_NONE;
				}
			}
			else
			{
				// 정확히 정해진 날자 및 시간 내에서 만 시작
				// 진행안함 상태가 아니면, 시작 가능 시간이더라도 시작 불가능
				bBuildEvent = PgOXQuizEventUtil::IsInDateTime(kEvent.kStartDate, kEvent.kEndDate, kCurTime);
			}

			if( bBuildEvent )
			{
				if( OXES_NONE != kEvent.bState )
				{
					continue;
				}
				return BuildEvent(kEvent);
			}
		}
	}
	return false;
}

void PgOXQuizEvent::Locked_Tick()
{
	BM::CAutoMutex kLock(m_kMutex);

	if(m_i64StateTime > g_kEventView.GetLocalSecTime())
	{
		return;
	}

	switch(m_kQuizState)
	{
	case OXQS_QUIZ_RELOAD:
		{
		}break;
	case OXQS_NONE:
		{
			GetStartEvent(0);
		}break;
	case OXQS_QUIZ_NOTI_01:
	case OXQS_QUIZ_NOTI_02:
	case OXQS_QUIZ_NOTI_03:
	case OXQS_QUIZ_NOTI_04:
	case OXQS_QUIZ_NOTI_05:
	case OXQS_QUIZ_NOTI_06:
	case OXQS_QUIZ_NOTI_07:
	case OXQS_QUIZ_NOTI_08:
	case OXQS_QUIZ_NOTI_09:
	case OXQS_QUIZ_NOTI_10:
		{
			int const iMsgIdx = m_kQuizState - 1;
			if(	(0 > iMsgIdx) 
			||	(m_kCurEvent.kContEventNoti.size() <= static_cast<size_t>(iMsgIdx)))
			{
				m_kQuizState = OXQS_QUIZ_CLOSE;
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("INVALID EVENT MSG ID [") << m_kCurEvent.iEventID << _T("]"));
				return;
			}

			CONT_OXEVENTNOTI::value_type const kNoti = m_kCurEvent.kContEventNoti[iMsgIdx];

			if(false == kNoti.kMsg.empty())
			{
				BM::Stream kBPacket(PT_I_M_NOTI_OXQUIZ_NOTI);
				kBPacket.Push(m_kEventGuid);
				kBPacket.Push(kNoti.kMsg);
				g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kBPacket);
			}

			m_kQuizState = static_cast<EOXQuizState>(m_kQuizState+1);
			m_i64StateTime = g_kEventView.GetLocalSecTime() + kNoti.iDelayTime;
			return;
		}break;
	case OXQS_QUIZ_READY:
		{
			{// 이벤트 시작 로그 기록
				PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_Open);
				PgLog kLog;
				kLog.Set(0,m_kCurEvent.iEventID);
				kLog.Set(1,0);
				kLog.Set(2,m_kCurEvent.iQuizNoMin);
				kLog.Set(3,m_kCurEvent.iQuizNoMax);
				kLog.Set(2,m_kEventGuid.str());
				kContLog.Add(kLog);
				kContLog.Commit();
			}

			CONT_OXQUIZEVENTINFO::iterator kFind = m_kCopyEvent.find(m_kCurEvent.iEventID);
			if(kFind == m_kCopyEvent.end())
			{
				m_kQuizState = OXQS_QUIZ_CLOSE;
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T("INVALID EVENT MSG ID [") << m_kCurEvent.iEventID << _T("]"));
				return;
			}

			(*kFind).second.bState = OXES_RUN;

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_OXQUIZ_UPDATE_STATE, _T("EXEC [DBO].[up_Update_OXQuizEventState]") );
			kQuery.InsertQueryTarget( m_kEventGuid );
			kQuery.QueryOwner( m_kEventGuid );
			kQuery.PushStrParam( m_kCurEvent.iEventID );
			kQuery.PushStrParam( (*kFind).second.bState );
			g_kCoreCenter.PushQuery(kQuery);

			// 여기서 모든 유저에게 이벤트 시작/참여 여부 패킷을 전송 하고 지정된 시간동안 기다린다.
			BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_OPEN);
			kPacket.Push(m_kEventGuid);
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kPacket);

			m_kQuizState = OXQS_ENTER_PARTICIPANT;
			m_i64StateTime = g_kEventView.GetLocalSecTime() + 30;
			return;
		}break;
	case OXQS_ENTER_PARTICIPANT:
		{
			m_kQuizState = OXQS_CLOSE_ROOM;
		}break;
	case OXQS_CLOSE_ROOM:
		{
			// 더이상 참가 하지 못하게 방을 닫음
			BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_LOCK);
			kPacket.Push(m_kEventGuid);
			kPacket.Push(m_kContParticipant.size());
			g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kPacket);

			if(true == m_kContParticipant.empty())	// 참가자 수 0 이면 공지 하고 이벤트 종료 한다.
			{
				m_kQuizState = OXQS_QUIZ_CLOSE;	// 이벤트를 종료한다.

				BM::Stream kBPacket(PT_I_M_NOTI_OXQUIZ_NOTI);
				kBPacket.Push(m_kEventGuid);
				kBPacket.Push(m_kCurEvent.kEventNotiClose);
				g_kProcessCfg.Locked_SendToServerType(CEL::ST_CENTER,kBPacket);
				return;
			}

			m_kQuizState = OXQS_QUIZ_START;	// 방을 닫았으면 문제를 내기 시작한다.
			m_i64StateTime = g_kEventView.GetLocalSecTime() + 5;
		}break;
	case OXQS_QUIZ_START:
		{
			// 참가자에게 문제 발송
			int iQuizNo = 0;

			CONT_LIST_QUIZ::value_type kQuiz;

			if(m_iQuizLevel < static_cast<int>(m_kContQuiz.size()))
			{
				m_kCurQuiz = m_kContQuiz[m_iQuizLevel];
				BM::Stream kPacket(PT_M_C_REQ_OXQUIZ_REQ_QUIZ);
				kPacket.Push(m_kEventGuid);
				kPacket.Push(m_kCurQuiz.kQuizText);
				kPacket.Push(m_kCurEvent.iAnsTime);
				BroadCast(kPacket);

				m_i64StateTime = g_kEventView.GetLocalSecTime() + m_kCurEvent.iAnsTime;
				m_kQuizState = OXQS_QUIZ_RESULT;
			}
			else
			{
				m_kQuizState = OXQS_QUIZ_CLOSE;
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << _T(" QUIZ LIST IS EMPTY EVENTID [") << m_kCurEvent.iEventID << _T("]"));
				return;
			}
		}break;
	case OXQS_QUIZ_RESULT:
		{
			// 퀴즈 결과 처리 틀린 사람 보내고 남은 사람 저장 하고 등등.....

			CONT_QUIZ_PARTICIPANT kTmpParticipant;
			CONT_VEC_GUID	kVecLive,
							kVecDead;

			for(CONT_QUIZ_PARTICIPANT::const_iterator iter = m_kContParticipant.begin();iter != m_kContParticipant.end();++iter)
			{
				if((*iter).second == m_kCurQuiz.bResult)
				{
					kVecLive.push_back((*iter).first);
					kTmpParticipant.insert(std::make_pair((*iter).first,OXQR_NONE));
				}
				else
				{
					kVecDead.push_back((*iter).first);
				}
			}

			m_kContParticipant = kTmpParticipant;

			if(!kVecLive.empty())	// 정답자가 있으면 정답자에게 정답 메시지 전송
			{
				ProcessRewardSet(kVecLive);

				BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_RESULT);
				kPacket.Push(true);
				kPacket.Push(kVecLive.size());
				kPacket.Push(kVecDead.size());
				kPacket.Push(m_kCurQuiz.kQuizDiscription);
				g_kRealmUserMgr.Locked_SendToUser(kVecLive, kPacket, false);

//				if(kVecLive.size() <= m_kCurEvent.iSaveMin)
				{
					// 여기서 살아 남은 플레이어 정보 디비로 기록 처리
					for(CONT_VEC_GUID::const_iterator iter = kVecLive.begin();iter != kVecLive.end();++iter)
					{
						SContentsUser rkOut;
						if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo((*iter),false,rkOut))
						{
							continue;
						}

						PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_User_Log,rkOut.kMemGuid,(*iter),rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
						PgLog kLog;
						kLog.Set(0,m_kCurEvent.iEventID);
						kLog.Set(1,m_iQuizLevel);
						kLog.Set(2,m_kEventGuid.str());
						kContLog.Add(kLog);
						kContLog.Commit();
					}
				}
			}

			if(!kVecDead.empty())	// 오답자가 있으면 오답자에게 오답 메시지 전송
			{
				BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_RESULT);
				kPacket.Push(false);
				kPacket.Push(kVecLive.size());
				kPacket.Push(kVecDead.size());
				kPacket.Push(m_kCurQuiz.kQuizDiscription);
				g_kRealmUserMgr.Locked_SendToUser(kVecDead, kPacket, false);

				for(CONT_VEC_GUID::const_iterator iter = kVecDead.begin();iter != kVecDead.end();++iter)
				{
					SContentsUser rkOut;
					if(S_OK != g_kRealmUserMgr.Locked_GetPlayerInfo((*iter),false,rkOut))
					{
						continue;
					}
					PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_User_Exit,rkOut.kMemGuid,(*iter),rkOut.kAccountID,rkOut.Name(),rkOut.iClass,rkOut.sLevel,rkOut.kGndKey.GroundNo());
					PgLog kLog;
					kLog.Set(0,m_kCurEvent.iEventID);
					kLog.Set(1,m_iQuizLevel);
					kLog.Set(2,m_kEventGuid.str());
					kContLog.Add(kLog);
					kContLog.Commit();
				}
			}

			{// 퀴즈 진행 단계별 유저 사황 기록
				PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_User_Num);
				PgLog kLog;
				kLog.Set(0,m_kCurEvent.iEventID);
				kLog.Set(1,m_iQuizLevel);
				kLog.Set(2,static_cast<int>(kVecLive.size()));
				kLog.Set(3,static_cast<int>(kVecDead.size()));
				kLog.Set(2,m_kEventGuid.str());
				kContLog.Add(kLog);
				kContLog.Commit();
			}

			size_t const kMaxStep = std::min(static_cast<size_t>(m_kCurEvent.iMaxStep),m_kContQuiz.size());

			m_i64StateTime = g_kEventView.GetLocalSecTime() + 5;

			if(	(m_iQuizLevel+1 < static_cast<int>(kMaxStep)) 
			&&	(kVecLive.size() > 0))	// 진행 스텝이 최대 스텝을 넘기면 이벤트를 종료 한다.
			{
				m_kQuizState = OXQS_QUIZ_START;
				++m_iQuizLevel;
				return;
			}

			m_kQuizState = OXQS_QUIZ_CLOSE;	// 여기서 이벤트 종료

		}break;
	case OXQS_QUIZ_CLOSE:
		{
			ProcessEndEvent();

			{// 이벤트 종료 로그 기록
				PgLogCont kContLog(ELogMain_OXQuiz,ELogSub_OxQuiz_Close);
				PgLog kLog;
				kLog.Set(0,m_kCurEvent.iEventID);
				kLog.Set(1,m_iQuizLevel);
				kLog.Set(2,m_kCurEvent.iQuizNoMin);
				kLog.Set(3,m_kCurEvent.iQuizNoMax);
				kLog.Set(2,m_kEventGuid.str());
				kContLog.Add(kLog);
				kContLog.Commit();
			}

			BM::Stream kPacket(PT_M_C_NOTI_OXQUIZ_CLOSE);
			kPacket.Push(m_kEventGuid);
			BroadCast(kPacket);

			m_kQuizState = OXQS_NONE;	// 여기서 이벤트 종료

			CONT_OXQUIZEVENTINFO::iterator kFind = m_kCopyEvent.find(m_kCurEvent.iEventID);
			if(kFind != m_kCopyEvent.end())
			{
				(*kFind).second.bState = OXES_END;
			}

			CEL::DB_QUERY kQuery( DT_PLAYER, DQT_OXQUIZ_UPDATE_STATE, _T("EXEC [DBO].[up_Update_OXQuizEventState]") );
			kQuery.InsertQueryTarget( m_kEventGuid );
			kQuery.QueryOwner( m_kEventGuid );
			kQuery.PushStrParam( m_kCurEvent.iEventID );
			kQuery.PushStrParam( (*kFind).second.bState );// 종료 기록
			g_kCoreCenter.PushQuery(kQuery);
		}break;
	default:
		{
		}break;
	}
}

void PgOXQuizEvent::ProcessRewardSet(CONT_VEC_GUID const & kVec)
{
	TABLE_LOCK(CONT_OXQUIZREWARD) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_OXQUIZREWARD const * pkCont = kObjLock.Get();
	if(!pkCont)
	{
		return;
	}

	int const iCurLevel = m_iQuizLevel+1;
	CONT_OXQUIZREWARD::key_type kKey(m_kCurEvent.iEventID,iCurLevel);

	CONT_OXQUIZREWARD::const_iterator finder = pkCont->find(kKey);
	if(finder == pkCont->end())
	{
		return;
	}

	for(CONT_VEC_GUID::const_iterator iter = kVec.begin();iter != kVec.end();++iter)
	{
		m_kRewardMgr[(*iter)] = kKey;
	}
}

void PgOXQuizEvent::ProcessEndEvent()
{
	TABLE_LOCK(CONT_OXQUIZREWARD) kObjLock;
	g_kTblDataMgr.GetContDef(kObjLock);
	CONT_OXQUIZREWARD const * pkCont = kObjLock.Get();
	if(!pkCont)
	{
		return;
	}

	std::wstring kFrom;
	GetDefString(OXQUIZ_RESULT_FROM, kFrom);

	for(CONT_REWARD_CHARACTER::const_iterator iter = m_kRewardMgr.begin();iter != m_kRewardMgr.end();++iter)
	{
		CONT_OXQUIZREWARD::const_iterator finder = pkCont->find((*iter).second);
		if(finder == pkCont->end())
		{
			continue;
		}

		CONT_OXQUIZREWARD::mapped_type const & kReward = (*finder).second;

		for(int i = 0;i < CONT_OXQUIZREWARD::mapped_type::MAX_OXQUIZREWARDITEM_NUM;++i)
		{
			SActionOrder* pkActionOrder = PgJobWorker::AllocJob();
			pkActionOrder->kCause = CNE_POST_SYSTEM_MAIL;
			pkActionOrder->kPacket2nd.Push((*iter).first);
			pkActionOrder->kPacket2nd.Push(std::wstring());
			pkActionOrder->kPacket2nd.Push(m_kCurEvent.kMailTitle);
			pkActionOrder->kPacket2nd.Push(m_kCurEvent.kMailText);
			pkActionOrder->kPacket2nd.Push(kReward.kItems[i].iItemNo);
			pkActionOrder->kPacket2nd.Push(kReward.kItems[i].iItemCount);
			pkActionOrder->kPacket2nd.Push(kReward.iMoney);
			pkActionOrder->kPacket2nd.Push(kFrom); // 차후 발송자 네임 GUID 사용 예약
			g_kJobDispatcher.VPush(pkActionOrder);
			// 현재는 아이템 한개만 첨부할 수 있으므로 메일 한통 발송 후 탈출...
			break;
		}
	}
}
