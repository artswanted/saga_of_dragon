
#include "stdafx.h"
#include <algorithm>
#include "PgStaticEventGround.h"
#include "Variant/PgPlayer.h"
#include "Variant/Unit.h"
#include "PgEffectFunction.h"
#include "Lohengrin/ActArg.h"
#include "PgStaticRaceGround.h"
#include "PgGroundTrigger.h"
#include "PgAction.h"
#include "PgPVPEffectSelector.h"

PgStaticRaceGround::PgStaticRaceGround() : 
m_pkRaceProgress(NULL)
{
}


PgStaticRaceGround::~PgStaticRaceGround()
{
	if(NULL != m_pkRaceProgress)
	{
		delete m_pkRaceProgress;
		m_pkRaceProgress = NULL;
	}
}


EOpeningState PgStaticRaceGround::Init(int const iMonsterControlID, bool const bMonsterGen)
{
	m_EventSchedule.clear();
	Clear();
	m_pkRaceProgress = new_tr PgRaceBilateralWayProgress(1, 1);

	CONT_DEF_EVENT_SCHEDULE const * pEventSchedule = NULL;
	g_kTblDataMgr.GetContDef(pEventSchedule);

	if( NULL == pEventSchedule )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event Schedule Table is NULL!") );
	}

	CONT_DEF_EVENT_SCHEDULE::const_iterator schedule_iter = pEventSchedule->begin();
	if( schedule_iter != pEventSchedule->end() )
	{
		for( ; schedule_iter != pEventSchedule->end() ; ++schedule_iter )
		{	// 이벤트 스케쥴 테이블에서 현재 그라운드에서 진행할 수 있는 이벤트를 찾아서 리스트로 만든다.
			CONT_DEF_EVENT_SCHEDULE::mapped_type const & Element = schedule_iter->second;

			if( Element.EventGround == this->GetGroundNo() )
			{
				BM::DBTIMESTAMP_EX NowTime;
				NowTime.SetLocalTime();	// 현재 시간.

				if( Element.EndDate > NowTime )
				{	// 이벤트 종료 시간이 지나지 않은 이벤트만 걸러낸다.
					m_EventSchedule.push_back(Element);
				}
			}
		}
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event Schedule Table is empty!") );
	}

	SetState(EGS_IDLE);
	CurrentUserCount(0);

	return PgGround::Init(iMonsterControlID, bMonsterGen);
}

bool PgStaticRaceGround::Clone(PgGround * pGround)
{
	return true;
}

void PgStaticRaceGround::Clear()
{
}

bool PgStaticRaceGround::ReleaseUnit(CUnit * pUnit, bool bRecursiveCall, bool const bSendArea)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( pUnit && pUnit->UnitType() == UT_PLAYER )
	{
		CurrentUserCount(CurrentUserCount() - 1);
		BM::Stream Packet(PT_M_C_NFY_EVENT_GROUND_USER_COUNT_MODIFY, CurrentUserCount());
		Broadcast(Packet);

		BM::Stream ToContentsPacket(PT_M_N_NFY_EVENT_GROUND_USER_COUNT_MODIFY);
		ToContentsPacket.Push(CurrentEventNo());
		ToContentsPacket.Push(CurrentUserCount());
		SendToContents(ToContentsPacket);

		pUnit->ClearAllEffect(true);
		g_kPVPEffectSlector.DelEffect(pUnit);

		if( (m_eState == EGS_PLAY) )
		{
			if( CurrentUserCount() <= 0 )
			{
				SetState(EGS_WAIT_SHORTTIME);
			}
		}

		PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if(pkPlayer)
		{
			m_kRankingSystem.RemovePlayer(pkPlayer);
			if(m_pkRaceProgress) m_pkRaceProgress->RemovePlayer(pkPlayer);
		}
	}
	//아래 주석 처리된 코드는 이후 복원될 소지가 있으므로(기획 문제) 삭제하지 않고 남겨둠
//	if(m_eState == EGS_PLAY)
//	{ //플레이 중 유저가 나가면 갱신된 순위 리스트를 다시 보내준다.
//		BroadcastRanking();
//	}


	return PgGround::ReleaseUnit(pUnit, bRecursiveCall, bSendArea);
}





void PgStaticRaceGround::SetState(EEventGroundState const State, bool bChangeOnlyState)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( m_eState == State )
	{	// 현재 상태와 같다면 상태를 변경할 필요가 없다.
		return;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = State;

	PgWorldEventMgr::DoEventGroundState(GetGroundNo(), m_eState);

	if( bChangeOnlyState )
	{	// 상태만 변경.
		return;
	}

	switch( m_eState )
	{
	case EGS_IDLE:
		{
			Clear();
		}break;
	case EGS_START_WAIT:
		{
			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(CurrentEventNo());
			ToContentsPacket.Push(true);
			SendToContents(ToContentsPacket);

			SetAutoNextState(g_kEventView.VariableCont().CommunityEventWaitTime * 1000);
		}break;
	case EGS_READY:
		{	// 60초 카운트하고 이벤트 시작.
			BM::Stream SendPacket(PT_M_C_NFY_RACE_EVENT_READY);
			SendPacket.Push(CurrentEventNo());
			Broadcast(SendPacket);

			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(false);
			SendToContents(ToContentsPacket);

			SetAutoNextState(g_kEventView.VariableCont().CommunityEventReadyTime * 1000);
		}break;
	case EGS_PLAY:
		{	
			m_dwStartTime = BM::GetTime32();

			BM::Stream SendPacket(PT_M_C_NFY_RACE_EVENT_START);
			Broadcast(SendPacket);

			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(false);
			SendToContents(ToContentsPacket);

			// 스타트 이펙트 걸어줘야 함.
			if( m_iEventStartEffectNo > 0 )
			{
				CUnit * pUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator Itor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
				while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
				{
					SEffectCreateInfo Create;
					Create.eType = EFFECT_TYPE_NORMAL;
					Create.iEffectNum = m_iEventStartEffectNo;
					Create.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pUnit->AddEffect(Create);					
				}				
			}

		}break;
	case EGS_WAIT_SHORTTIME:
		{
			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(false);
			SendToContents(ToContentsPacket);

			SetAutoNextState(EndWaitMomentTime());
		}break;
	case EGS_END_WAIT:
		{
			MergeFinalRankList();
			BroadcastFinalResult(false); //전체 유저에게 결과 패킷을 브로드캐스트
			SetAutoNextState(EndWaitTime());
		}break;
	case EGS_END:
		{
			KickUserFromGround();
			m_eState = EGS_IDLE;

			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(CurrentEventNo());
			ToContentsPacket.Push(false);
			SendToContents(ToContentsPacket);
		}break;
	default:
		{
		}break;
	}
}

void PgStaticRaceGround::UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime)
{
	if( 0 == m_dwAutoStateRemainTime )
	{
		return;
	}

	if( dwElapsedTime >= m_dwAutoStateRemainTime )
	{
		EEventGroundState eState = static_cast<EEventGroundState>(m_eState << 1);

		SetState( eState );
	}
	else
	{
		m_dwAutoStateRemainTime -= dwElapsedTime;
	}
}

void PgStaticRaceGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	
	DWORD dwElapsed = 0;
	DWORD const dwNow = BM::GetTime32();

	bool bTimeUpdate = true;
	if( EGS_IDLE != m_eState )
	{
		bTimeUpdate = false;
	}
	if(EGS_PLAY <= m_eState && CurrentUserCount() <= 0)
	{ //게임 시작 후 유저가 한명도 없으면 이벤트 종료 
		SetState(EGS_END);
		return;
	}

	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsed, bTimeUpdate);

	switch( m_eState )
	{
	case EGS_IDLE:
		{
			CheckEventStartable();
		}break;
	case EGS_START_WAIT:
		{
		}break;
	case EGS_READY:
		{
		}break;
	case EGS_PLAY:
		{
			if( 0 < LimitTime() )
			{
				if( LimitTime() <= dwElapsed )
				{
					SetState(EGS_WAIT_SHORTTIME);
				}
				else
				{
					LimitTime(LimitTime() - dwElapsed);
				}
			}
			if(0 < MaxLimitPlayTime()) //이벤트 
			{
				if( MaxLimitPlayTime() <= dwElapsed )
				{
					SetState(EGS_WAIT_SHORTTIME);
				}
				else
				{
					MaxLimitPlayTime(MaxLimitPlayTime() - dwElapsed);
				}
			}
		}break;
	case EGS_WAIT_SHORTTIME:
		{
		}break;
	case EGS_END_WAIT:
		{
		}break;
	case EGS_END:
		{
		}break;
	default:
		{
		}break;
	}

	UpdateAutoNextState( dwNow, dwElapsed );
	PgGround::OnTick1s();

}


void PgStaticRaceGround::CheckEventStartable(void)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( m_EventSchedule.empty() )
	{	// 비었으면 할 필요 없다.
		return;
	}


	BM::DBTIMESTAMP_EX NowTime;
	NowTime.SetLocalTime();	// 현재 시간.
	for(CONT_EVENT_SCHEDULE::const_iterator itSchedule = m_EventSchedule.begin(); itSchedule != m_EventSchedule.end(); ++itSchedule)
	{
		CONT_DEF_EVENT_SCHEDULE::mapped_type const & Element = *itSchedule;
		if( GroundKey().GroundNo() == Element.EventGround )
		{
			BM::DBTIMESTAMP_EX kStartDay(Element.StartDate);
			BM::DBTIMESTAMP_EX kEndDay(Element.EndDate);
			BM::DBTIMESTAMP_EX kCurTime(NowTime);
			if(!kStartDay.IsNull())
			{ //이벤트 시작 날짜 제한이 걸려있다면..
				kStartDay.hour = 0;
				kStartDay.minute = 0;
				kStartDay.second = 0;
				if(kStartDay > kCurTime)
				{
					continue;
				}
			}
			if(!kEndDay.IsNull())
			{ //이벤트 종료 날짜 제한이 걸려있다면..
				kEndDay.hour = 23;
				kEndDay.minute = 59;
				kEndDay.second = 59;
				if(kEndDay < kCurTime)
				{
					continue;
				}
			}

			SYSTEMTIME SysTime;
			GetLocalTime(&SysTime);

			//요일/시간/분 주기 생성 & 체크
			SDAYOFWEEK kDayOfWeek(EDOW_NONE);
			kDayOfWeek.SetDayofWeek(Element.DayofWeek);

			//이벤트 주기 개수 만큼 시간대를 만들어 준다
			DWORD dwEventSec = Element.EventCount * (Element.EventPeriod);
			SSIMPLETIME kStartTime(Element.StartTime);
			SSIMPLETIME kEndTime(Element.StartTime);

			//Zero Divide 방지
			kEndTime.byHour += ((dwEventSec > 3600) ? dwEventSec / 3600 : 0);
			kEndTime.byMin += (dwEventSec > 60) ? ((dwEventSec % 3600) / 60) : dwEventSec;
			if(kEndTime.byMin >= 60) //분이 60을 넘어가면 그만큼 시간을 플러스 해준다
			{
				kEndTime.byHour += static_cast<int>(kEndTime.byMin / 60);
				kEndTime.byMin = kEndTime.byMin % 60;
			}
			if(kEndTime.byHour >= 24) //시가 24를 넘어갈 경우
			{ //넘어가는 시간은 그냥 24시까지로 잘라주자.. 만약 요일 추가가 되어야 한다면 이곳을 수정
				kEndTime.byHour = 23;
				kEndTime.byMin = 59;
			}

			SSIMPLETIMELIMIT kTimeLimit(kStartTime, kEndTime, kDayOfWeek);
			if(!kTimeLimit.CheckTimeIsInDuration(SysTime))
			{
				continue;
			}

			int iCurTimeBySec = NowTime.hour * 3600 + NowTime.minute * 60 + NowTime.second;
			int iStartTimeBySec = Element.StartTime.hour * 3600 + Element.StartTime.minute * 60 + Element.StartTime.second;
			int iEndTimeBySec = iStartTimeBySec + (Element.EventPeriod * Element.EventCount);
			if( (iStartTimeBySec <= iCurTimeBySec)
			&& (iCurTimeBySec <= iEndTimeBySec)
			&& (Element.EventPeriod > 0) )
			{ //현재 시간이 이벤트 시간 이내일때만
				int iElapsedTimeBySec = iCurTimeBySec - iStartTimeBySec;

				int iCurEventNo = (iElapsedTimeBySec >= Element.EventPeriod) ?
					(iElapsedTimeBySec / Element.EventPeriod) : 0;
				int iElapsedCurEventTimeBySec = (iElapsedTimeBySec > Element.EventPeriod) ?
					(iElapsedTimeBySec % Element.EventPeriod) : iElapsedTimeBySec;
				if(iElapsedCurEventTimeBySec <= g_kEventView.VariableCont().CommunityEventWaitTime) //이벤트 시작 후 현재 경과시간이 10분 이내인가(기준이 정해지면 이 조건문을 수정)
				{
					CurrentEventNo( Element.EventNo );
					SetGame(CurrentEventNo(), Element.EventPeriod * 1000, Element.UnUsableConsumeItem);
				} //if(iElapsedCurEventTimeBySec < 600)
			} //if(iStartTimeBySec <= iCurTimeBySec && iCurTimeBySec <= iEndTimeBySec)
		}
	} //for(CONT_EVENT_SCHEDULE::const_iterator....
}


void PgStaticRaceGround::SetGame(int iEventNo, DWORD dwEventDuration, bool bUnusableItem)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( 0 >= iEventNo )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Wrong EventNo: ") << iEventNo);
		return;
	}

	CONT_DEF_EVENT_RACE const * pEventRace = NULL;
	g_kTblDataMgr.GetContDef(pEventRace);
	if( NULL == pEventRace )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event Race Table is NULL!"));
		return;
	}

	CONT_DEF_EVENT_RACE::const_iterator itFind = pEventRace->find(iEventNo);
	if(pEventRace->end() == itFind)
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Cannot Find Race Event: ") << iEventNo);
		return;
	}

	EndWaitTime(10000);
	EndWaitMomentTime(2000);
	DWORD dwDummyTime = 30000;

	CONT_DEF_EVENT_RACE::mapped_type kElem = (*itFind).second;
	LimitTime(0);
	if(dwEventDuration > 0) //게임 지속시간이 끝나면 게임이 강제 종료되도록 설정한다.
	{
		DWORD dwTemp = dwEventDuration -
			( (g_kEventView.VariableCont().CommunityEventWaitTime * 1000) +
				(g_kEventView.VariableCont().CommunityEventReadyTime * 1000 + 10000) +
				EndWaitTime() + EndWaitMomentTime() + dwDummyTime
				);
		if(dwTemp > 0)
		{
			MaxLimitPlayTime(dwTemp);
		}
	}
	else
	{
		MaxLimitPlayTime(0);
	}

	m_dwLimitTimeValue = kElem.LimitTime * 1000; //밀리세컨드로 사용하기 위해 x1000
	UsableSkill(kElem.UseSkill);
	RaceType(static_cast<ERACERULETYPE>(kElem.RaceType));
	TrackAround(1); // 현재는 1바퀴로 강제 설정
	RaceType(ERRT_BYRANK); // 현재는 등수별 룰만 가능
	MpMax(kElem.MpMax);
	RunningSpeed(kElem.MoveSpeed);
	FirstGoalIn(false);
	UnusableConsumeItem(bUnusableItem);
	m_iEventStartEffectNo = kElem.StartEffect;

	for(int index = 0; index < 5; index++)
	{
		SetRewardItemGroup(index, kElem.RewardItemGroup[index]);
		for(int iColumn = 0; iColumn < 4; iColumn++)
		{
			SetRewardItemBagGroup(index, iColumn, kElem.RewardItemBagGroup[index][iColumn]);
		}
	}

	CurrentUserCount(0);
	m_kRankingSystem.Init();
	m_dwStartTime = 0;
	if(m_pkRaceProgress)
	{
		m_pkRaceProgress->SetTrack(TrackAround(), m_kContCheckPointOrder.size());
		m_pkRaceProgress->InitProgress();
	}

	if(m_kContRaceResult.empty() == false)
	{
		CONT_RACE_RESULT kTemp;
		m_kContRaceResult.swap(kTemp); //최종결과 리스트 컨테이너 초기화
	}

	SetState(EGS_START_WAIT);

	BM::Stream ToContentsPacket(PT_M_N_NFY_EVENT_GROUND_USER_COUNT_MODIFY);
	ToContentsPacket.Push(CurrentEventNo());
	ToContentsPacket.Push(CurrentUserCount());
	SendToContents(ToContentsPacket);

}

DWORD PgStaticRaceGround::GetCurrentRecordTime(void)
{
	return BM::GetTime32() - m_dwStartTime;
}

void PgStaticRaceGround::SendMapLoadComplete(PgPlayer * pUser)
{
	if(NULL == pUser) { return; }

	PgGround::SendMapLoadComplete(pUser);


	if(RunningSpeed() > 0)
	{
		pUser->SetAbil(AT_C_MOVESPEED, RunningSpeed(), true, true);
	}
	if(MpMax() > 0)
	{
		pUser->SetAbil(AT_MEMORIZED_MP, pUser->GetAbil(AT_MP)); //처음 MP를 저장
		pUser->SetAbil(AT_MEMORIZED_MAX_MP, pUser->GetAbil(AT_C_MAX_MP)); //처음 MP를 저장
		pUser->SetAbil(AT_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
		pUser->SetAbil(AT_C_MAX_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
		pUser->SetAbil(AT_R_MAX_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
		pUser->SetAbil(AT_MAX_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
	}

	bool bLockInput = (m_eState == EGS_START_WAIT) || (m_eState == EGS_READY);
	BM::Stream NfyPacket(PT_M_C_NFY_RACE_EVENT_GROUND_MOVE_COMPLETE);
	NfyPacket.Push(CurrentEventNo());
	NfyPacket.Push(m_dwAutoStateRemainTime); //EGS_READY => EGS_PLAY까지 남은 시간
	pUser->Send(NfyPacket);
}


bool PgStaticRaceGround::VCheckUsableSkill(CUnit* pkUnit, const SActionInfo& rkAction)
{
	if(NULL == pkUnit) { return false; }
	if(true == UsableSkill()) { return true; } //설정에서 스킬 사용을 허용했다면, 무조건 true를 리턴

	GET_DEF(CSkillDefMgr, kSkillDefMgr);
	CSkillDef const* pkSkillDef = kSkillDefMgr.GetDef(rkAction.iActionID);
	if(NULL == pkSkillDef) { return false; }

	if(EST_GENERAL != pkSkillDef->GetType() && EST_NONE != pkSkillDef->GetType()) { return false; } //설정에서 스킬 사용을 금지했다면, 이동 액션만 허용해준다.


	return true;
}


bool PgStaticRaceGround::VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy)
{
	if(NULL == pUnit || NULL == pNfy) { return false; }
	switch(wType)
	{
	case PT_C_M_NFY_RACE_CHECK_POINT:
		{
			PgPlayer * pPlayer = dynamic_cast<PgPlayer *>(pUnit);
			if( NULL == pPlayer ) { return false; }

			GTRIGGER_ID kTriggerID;
			pNfy->Pop( kTriggerID, MAX_PATH );


			CONT_GTRIGGER::const_iterator iter = m_kContTrigger.find( kTriggerID );
			if( m_kContTrigger.end() == iter )
			{
				BM::vstring kLogMsg;
				kLogMsg << _T("[RaceEvent] Cannot Find Trigger: ") << kTriggerID;
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << kLogMsg );
				return false;
			}

			CONT_GTRIGGER::mapped_type pkTrigger = (*iter).second;
			if( NULL == pkTrigger )
			{
				BM::vstring kLogMsg;
				kLogMsg << _T("[RaceEvent] Cannot Find Trigger: ") << kTriggerID;
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << kLogMsg );
				return false;
			}
			if(EGS_PLAY == m_eState)
			{
				EnterCheckPoint(kTriggerID, pPlayer);
			}
		}break;
	case PT_C_M_REQ_DEFAULT_MAP_MOVE:
		{
			PgPlayer * pPlayer = dynamic_cast< PgPlayer* >(pUnit);
			if( pPlayer )
			{
				pPlayer->ClearAllEffect(true);
				pPlayer->SetAbil(AT_C_MAX_MP, pPlayer->GetAbil(AT_MEMORIZED_MAX_MP), true, false); //입장전 MP로 복원
				pPlayer->SetAbil(AT_MP, pPlayer->GetAbil(AT_MEMORIZED_MP), true, false); //입장전 MP로 복원
				pPlayer->SetAbil(AT_MEMORIZED_MP, 0);
				pPlayer->SetAbil(AT_MEMORIZED_MAX_MP, 0);


				SRecentPlayerNormalMapData NormalMap;
				pPlayer->GetRecentNormalMap(NormalMap);

				SChannelMapMove MoveInfo;
				MoveInfo.iChannelNo = NormalMap.ChannelNo;
				MoveInfo.iGroundNo = NormalMap.GroundNo;

				// 스위치로 보냄.
				BM::Stream Packet(PT_M_L_TRY_LOGIN_CHANNELMAPMOVE);
				Packet.Push(pPlayer->GetID());
				Packet.Push(MoveInfo);
				SendToServer(pPlayer->GetSwitchServer(), Packet);

				// 클라로 보냄.
				BM::Stream UserPacket(PT_M_C_TRY_LOGIN_CHANNELMAPMOVE);
				UserPacket.Push(MoveInfo);
				pPlayer->Send(UserPacket);
			}
		}break;
	case PT_C_M_NFY_MAPLOADED:
		{	// default 바로 위에 있어야함. 이 case와 default사이에 다른 case가 들어오면 안됨.
			PgPlayer *pkUser = dynamic_cast<PgPlayer*>(pUnit);
			if(NULL != pkUser )
			{
				CurrentUserCount(CurrentUserCount() + 1);
				BM::Stream Packet(PT_M_C_NFY_EVENT_GROUND_USER_COUNT_MODIFY, CurrentUserCount());
				Broadcast(Packet);
			}
		} // break; 없음.
	default:
		{
			return PgGround::VUpdate(pUnit, wType, pNfy);
		}break;
	}
	return true;
}


void PgStaticRaceGround::KickUserFromGround(void)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pUnit);
		if(pkPlayer)
		{
			pkPlayer->SetAbil(AT_C_MAX_MP, pkPlayer->GetAbil(AT_MEMORIZED_MAX_MP), true, true); //입장전 MP로 복원
			pkPlayer->SetAbil(AT_MP, pkPlayer->GetAbil(AT_MEMORIZED_MP), true, true); //입장전 MP로 복원
			pkPlayer->SetAbil(AT_MEMORIZED_MP, 0);
			pkPlayer->SetAbil(AT_MEMORIZED_MAX_MP, 0);

			SRecentPlayerNormalMapData NormalMap;
			pkPlayer->GetRecentNormalMap(NormalMap);

			SChannelMapMove MoveInfo;
			MoveInfo.iChannelNo = NormalMap.ChannelNo;
			MoveInfo.iGroundNo = NormalMap.GroundNo;

			// 스위치로 보냄.
			BM::Stream Packet(PT_M_L_TRY_LOGIN_CHANNELMAPMOVE);
			Packet.Push(pkPlayer->GetID());
			Packet.Push(MoveInfo);
			SendToServer(pkPlayer->GetSwitchServer(), Packet);

			// 클라로 보냄.
			BM::Stream UserPacket(PT_M_C_TRY_LOGIN_CHANNELMAPMOVE);
			UserPacket.Push(MoveInfo);
			pkPlayer->Send(UserPacket);
		}
	}
}

int PgStaticRaceGround::GetTriggerIndex(GTRIGGER_ID const& kTrigID)
{
	int iIndex = 1;
	CONT_ORDER_CHECKPOINT::const_iterator iter = m_kContCheckPointOrder.begin();
	for(; iter != m_kContCheckPointOrder.end(); ++iter)
	{
		if(*iter == kTrigID)
		{
			return iIndex;
		}

		iIndex++;
	}

	return 0;
}

bool PgStaticRaceGround::EnterCheckPoint(GTRIGGER_ID const& kTrigID, PgPlayer* pkPlayer)
{
	if(NULL == pkPlayer) { return false; }

	BM::CAutoMutex kLock(m_kRscMutex);

	CONT_GTRIGGER::const_iterator itTrig = m_kContTrigger.find(kTrigID);
	if(itTrig == m_kContTrigger.end()) { return false; }
	PgGTrigger_CheckPoint* pkCheckPoint = dynamic_cast<PgGTrigger_CheckPoint*>(itTrig->second);
	if(pkCheckPoint == NULL) { return false; }

	int iRecentIndex = GetTriggerIndex(kTrigID);
	BYTE byProgressState = ERPS_INCORRECT;
	if(NULL != m_pkRaceProgress)
	{
		byProgressState = m_pkRaceProgress->IsCorrectNextPos(pkPlayer, m_kRankingSystem.GetPlayerPos(pkPlayer), iRecentIndex);
	}


	if(!(ERPS_CORRECT & byProgressState) || false == m_kRankingSystem.SetRecord(pkPlayer, iRecentIndex, GetCurrentRecordTime())) { return false; }
	m_kRankingSystem.SetPlayerProgressPos(pkPlayer, pkCheckPoint->GetProgressNo());

	if(byProgressState & ERPS_FINISH)
	{
		CONT_RACE_RESULT::size_type iNumArrived = m_kContRaceResult.size();
		RANKER_INFO kInfo;
		if(NULL != pkPlayer && true == GetPlayerInfo(pkPlayer, kInfo))
		{
			auto kRet = m_kContRaceResult.insert(std::make_pair(iNumArrived + 1, kInfo)); //최종 결과리스트에 순서대로 정보를 저장 (저장된 유저가 던전을 나가더라도 정보는 남아야한다)
			if(false == kRet.second)
			{ //insert 실패하면
				VERIFY_INFO_LOG(false, BM::LOG_LV6, __FL__ << _T("PgStaticRaceGround::EnterCheckPoint() function, critical error!") );
				return false;
			}
			int iRank = kRet.first->first;
			RANKER_INFO& rkRankInfo = kRet.first->second;
			if(false == RewardItem(iRank, rkRankInfo))
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Reward Item Failed!") );
			}

			BroadcastFinalResult(true); //결승 통과한 유저들에게만 전송
		}
	}

	BroadcastRanking(ERPS_FINISH & byProgressState);

	return true;
}

void PgStaticRaceGround::BroadcastRanking(bool bFirstGoalIn)
{
	//기록을 패킷화해서 브로드캐스트
	CONT_RANK_LIST kRankList;
	m_kRankingSystem.GetRankList(kRankList);
	CONT_RANK_LIST::const_iterator itTopRanker = kRankList.begin();
	CONT_RANK_LIST::iterator itTopRankerBound = kRankList.begin();
	CONT_RANK_LIST::iterator itRankListBegin = kRankList.begin();
	for(int i = 0; i < 3; i++) //1~3위까지의 경계
	{
		if(itTopRankerBound != kRankList.end())
		{
			++itTopRankerBound;
		}
		else
		{
			break;
		}
	}
	CONT_RANK_LIST::const_iterator itRankList = kRankList.begin();
	int iPlayerNum = kRankList.size();
	RANKER_INFO kInfo;
	int iRank = 0;
	PgPlayer const* pkPlayer = NULL;

	if(FirstGoalIn() == false && bFirstGoalIn == true)
	{
		FirstGoalIn(true);
	}
	else
	{
		bFirstGoalIn = false;
	}

	PgPlayer* pkTargetPlayer = NULL;
	BM::Stream kSourcePacket(PT_M_C_NFY_RACE_RANK_INFO);
	kSourcePacket.Push(iPlayerNum <= 3 ? iPlayerNum : 4);
	for(itTopRanker = kRankList.begin(); itTopRanker != kRankList.end(), itTopRanker != itTopRankerBound; ++itTopRanker)
	{
		iRank = itTopRanker->first;
		pkPlayer = itTopRanker->second;
		if(pkPlayer == NULL) { return; }
		if(false == GetPlayerInfo(pkPlayer, kInfo)) { return; }

		kSourcePacket.Push(iRank); //순위
		kSourcePacket.Push(kInfo.kName);
		kSourcePacket.Push(kInfo.bGoalIn);
		kSourcePacket.Push(kInfo.kCurProgressPos != 0 ? kInfo.kCurProgressPos : kInfo.kCurPos);
		kSourcePacket.Push(kInfo.dwRecord);
	}


	CUnit* pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		if(pUnit == NULL || pUnit->IsUnitType(UT_PLAYER) == false) { continue; }
		pkTargetPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if(pkTargetPlayer == NULL) { continue; }

		BM::Stream kPacket(kSourcePacket); //공통으로 포함되는 패킷 데이터를 사본에 복사

		CONT_RANK_LIST::iterator itFindInTopRanker =
			std::find_if(itRankListBegin, itTopRankerBound, std::bind2nd(map_data_compare<CONT_RANK_LIST>(), pkTargetPlayer) );
		if(itFindInTopRanker == kRankList.end()) //패킷 받을 대상이 탑랭커가 아닐 때
		{
			if(true == GetPlayerInfo(pkTargetPlayer, kInfo))
			{ //내 순위 정보를 추가해서 보내자
				iRank = itRankList->first;
				kPacket.Push(iRank); //순위
				kPacket.Push(kInfo.kName);
				kPacket.Push(kInfo.bGoalIn);
				kPacket.Push(kInfo.kCurProgressPos != 0 ? kInfo.kCurProgressPos : kInfo.kCurPos);
				kPacket.Push(kInfo.dwRecord);
			}
		}

		kPacket.Push(bFirstGoalIn);
		if(true == bFirstGoalIn)
		{
			LimitTime(m_dwLimitTimeValue);
			kPacket.Push(LimitTime());
		}
		pkTargetPlayer->Send(kPacket);
	}

}

bool PgStaticRaceGround::GetPlayerInfo(PgPlayer const* pkPlayer, RANKER_INFO& rkOutInfo)
{
	if(NULL == pkPlayer) { return false; }
	rkOutInfo.kName = pkPlayer->Name();
	rkOutInfo.kCurPos = m_kRankingSystem.GetPlayerPos(pkPlayer);
	rkOutInfo.kCurProgressPos = m_kRankingSystem.GetPlayerProgressPos(pkPlayer);
	rkOutInfo.dwRecord = m_kRankingSystem.GetCurrentPlayerRecord(pkPlayer);
	rkOutInfo.pkPlayer = pkPlayer;
	if(NULL == m_pkRaceProgress) { return false; }
	rkOutInfo.bGoalIn = m_pkRaceProgress->IsCompleteRace(pkPlayer);

	if(rkOutInfo.kCurPos == 0) { return false; }

	return true;
}


//게임 종료시 결승 통과 못한 유저들도 최종 랭킹에 포함하여 등수를 매긴다. 보상은 없겠지만
void PgStaticRaceGround::MergeFinalRankList(void)
{
	if(NULL == m_pkRaceProgress) { return; }
	CONT_RANK_LIST kRankList;
	m_kRankingSystem.GetRankList(kRankList);

	PgPlayer const* pkPlayer = NULL;
	RANKER_INFO kInfo;

	CONT_RANK_LIST::iterator itRankList = kRankList.begin();
	for(; itRankList != kRankList.end(); ++itRankList)
	{
		pkPlayer = itRankList->second;
		if(pkPlayer != NULL && true == GetPlayerInfo(pkPlayer, kInfo) && false == kInfo.bGoalIn) //골인하지 않은 애들을 순서별로 순위를 매긴다.
		{
			m_kContRaceResult.insert(std::make_pair(m_kContRaceResult.size() + 1, kInfo));
		}
	}

}


bool PgStaticRaceGround::RewardItem(int iRank, RANKER_INFO& rkRankerInfo)
{
	if(NULL == rkRankerInfo.pkPlayer) { return false; }
	int iRewardGroupNo = GetRewardGroupNo(iRank);
	if(iRewardGroupNo < 0) { return false; }
	PgPlayer* pkTargetPlayer = dynamic_cast<PgPlayer*>(GetUnit(rkRankerInfo.pkPlayer->GetID() ));
	if(NULL == pkTargetPlayer) { return false; }

	const int MAX_COLUMN = 4;
	int iRewardItemGroupNo = 0;
	for(int iColumn = 0; iColumn < MAX_COLUMN; iColumn++)
	{
		iRewardItemGroupNo = GetRewardItemBagGroup(iRewardGroupNo, iColumn);
		if(iRewardItemGroupNo == 0) { continue; } //테이블에 보상 아이템 번호가 비어있을 경우 건너뜀

		PgAction_RaceEventRewardItem kAction(GroundKey(), iRewardItemGroupNo);
		if(true == kAction.DoAction(pkTargetPlayer, pkTargetPlayer))
		{
			rkRankerInfo.kContRewardItemList.push_back(kAction.GetRewardItemNo());
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << _T("Reward Item Failed! RewardGroupNo: ") << iRewardGroupNo );
		}
	}
	if(true == rkRankerInfo.kContRewardItemList.empty()) { return false; }

	return true;
}

void PgStaticRaceGround::BroadcastFinalResult(bool bSendToCompletePlayer) //bSendToCompletePlayer: 골인한 플레이어에게만 결과 패킷 보낼 것인가 (EGS_PLAY 일때)
{ //레이스 결과를 클라이언트에 알림
	//결과 데이터를 패킷에 미리 담아 놓자
	BM::Stream kSendPacket(PT_M_C_NFY_RACE_EVENT_END);
	kSendPacket.Push(m_kContRaceResult.size());

	CONT_RACE_RESULT::const_iterator itRankList = m_kContRaceResult.begin();
	int iRank = 0;
	RANKER_INFO const* pkRankerInfo = NULL;
	for(; itRankList != m_kContRaceResult.end(); ++itRankList)
	{
		iRank = itRankList->first;
		pkRankerInfo = &itRankList->second;
		if(pkRankerInfo == NULL) { continue; }

		kSendPacket.Push(iRank); //등수
		kSendPacket.Push(pkRankerInfo->kName); //선수 이름
		kSendPacket.Push(pkRankerInfo->dwRecord); //시간 기록
		kSendPacket.Push(pkRankerInfo->bGoalIn);
		kSendPacket.Push(pkRankerInfo->kContRewardItemList.size());
		if(true == pkRankerInfo->bGoalIn && false == pkRankerInfo->kContRewardItemList.empty())
		{ //보상 받은 아이템 번호들도 보냄
			RESULT_ITEM_BAG_LIST_CONT::const_iterator itRewardItem = pkRankerInfo->kContRewardItemList.begin();
			for(; itRewardItem != pkRankerInfo->kContRewardItemList.end(); ++itRewardItem)
			{
				kSendPacket.Push(static_cast<int>(*itRewardItem));
			}
		}
	}
	bool bFinishGame = !bSendToCompletePlayer;
	kSendPacket.Push(bFinishGame); //잠시 후 게임을 끝낼 것인가?



	if(m_pkRaceProgress == NULL) { return; }

	CUnit* pUnit = NULL;
	PgPlayer* pkTargetPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		if(pUnit == NULL || pUnit->IsUnitType(UT_PLAYER) == false) { continue; }
		pkTargetPlayer = dynamic_cast<PgPlayer*>(pUnit);
		if(pkTargetPlayer == NULL) { continue; }

		if(true == bSendToCompletePlayer && false == m_pkRaceProgress->IsCompleteRace(pkTargetPlayer)) { continue; }

		pkTargetPlayer->Send(kSendPacket);

	}

}

int PgStaticRaceGround::GetRewardGroupNo(int iRank)
{ //순위별 보상일 때..
	for(int n = 4; n >= 0; n--)
	{
		if(n > 0)
		{
			if(iRank <= m_ArrRewardItemGroup[n] && iRank > m_ArrRewardItemGroup[n - 1])
			{
				return n;
			}
		}
		else if(n == 0 && iRank == m_ArrRewardItemGroup[n])
		{ //최고 보상
			return n;
		}
	}

	return -1;
}


void PgStaticRaceGround::GMCommandStartEvent(int EventNo)
{
	if( m_eState != EGS_IDLE )
	{
		return;
	}

	CONT_DEF_EVENT_SCHEDULE const * pEventSchedule = NULL;
	g_kTblDataMgr.GetContDef(pEventSchedule);

	if( NULL == pEventSchedule )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event Schedule Table is NULL!") );
		return;
	}

	CONT_DEF_EVENT_SCHEDULE::const_iterator schedule_iter = pEventSchedule->find(EventNo);

	if( schedule_iter != pEventSchedule->end() )
	{
		CONT_DEF_EVENT_SCHEDULE::mapped_type const & Element = schedule_iter->second;

		CurrentEventNo( Element.EventNo );
		SetGame(CurrentEventNo(), 0, schedule_iter->second.UnUsableConsumeItem);
	}

	BM::CAutoMutex Lock(m_kRscMutex);

	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		PgPlayer* pkPlayer = dynamic_cast< PgPlayer* >(pUnit);
		if(pkPlayer)
		{
			if(RunningSpeed() > 0)
			{
				pkPlayer->SetAbil(AT_C_MOVESPEED, RunningSpeed(), true, true);
			}
			if(MpMax() > 0)
			{
				pkPlayer->SetAbil(AT_MEMORIZED_MP, pkPlayer->GetAbil(AT_MP)); //처음 MP를 저장
				pkPlayer->SetAbil(AT_MEMORIZED_MAX_MP, pkPlayer->GetAbil(AT_C_MAX_MP)); //처음 MP를 저장
				pkPlayer->SetAbil(AT_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
				pkPlayer->SetAbil(AT_C_MAX_MP, MpMax(), true, false); //패널티 능력치로 강제 설정
			}

			BM::Stream NfyPacket(PT_M_C_NFY_RACE_EVENT_GROUND_MOVE_COMPLETE);
			NfyPacket.Push(CurrentEventNo());
			NfyPacket.Push(m_dwAutoStateRemainTime); //EGS_READY => EGS_PLAY까지 남은 시간
			pkPlayer->Send(NfyPacket);
		}
	}
}


void PgStaticRaceGround::GMCommandEndEvent()
{	// 진행하고 있는 이벤트 강제로 끝냄.
	if( m_eState == EGS_PLAY )
	{
		SetState(EGS_WAIT_SHORTTIME);
	}
}


PgRankingSystem::PgRankingSystem() : 
m_iTopPosition(0),
m_kRankTree(),
m_kContPlayerPos(),
m_kContPlayerProgressPos(),
m_kContPlayerLapTime()
{
}


void PgRankingSystem::Init(void)
{
	m_iTopPosition = 0;

	m_kRankTree.clear();
	m_kContPlayerPos.clear();
	m_kContPlayerProgressPos.clear();
	m_kContPlayerLapTime.clear();
}


EVENTMAP_POS PgRankingSystem::GetPlayerPos(PgPlayer const* pkPlayer)
{
	if(NULL == pkPlayer) { return 0; }

	CONT_PLAYERPOS::iterator itPlayerPos = m_kContPlayerPos.find(pkPlayer);
	if(itPlayerPos == m_kContPlayerPos.end()) { return 0; } //에러처리: 없는 플레이어

	return itPlayerPos->second;
}


bool PgRankingSystem::SetPlayerPos(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos)
{
	if(NULL == pkPlayer) { return false; }

	CONT_PLAYERPOS::iterator itPlayerPos = m_kContPlayerPos.find(pkPlayer);
	if(itPlayerPos == m_kContPlayerPos.end())
	{
		return m_kContPlayerPos.insert(std::make_pair(pkPlayer, kRecentPos)).second;
	}
	//if(itPlayerPos->second + 1 < kRecentPos) { return false; } //에러처리: 순차적인 진행이 아님!=>이 체크는 외부에 맡긴다.
	itPlayerPos->second = kRecentPos;

	return true;
}

EVENTMAP_POS PgRankingSystem::GetPlayerProgressPos(PgPlayer const* pkPlayer)
{
	if(NULL == pkPlayer) { return 0; }

	CONT_PLAYERPOS::iterator itPlayerPos = m_kContPlayerProgressPos.find(pkPlayer);
	if(itPlayerPos == m_kContPlayerProgressPos.end()) { return 0; } //에러처리: 없는 플레이어

	return itPlayerPos->second;
}


bool PgRankingSystem::SetPlayerProgressPos(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos)
{
	if(NULL == pkPlayer) { return false; }

	CONT_PLAYERPOS::iterator itPlayerPos = m_kContPlayerProgressPos.find(pkPlayer);
	if(itPlayerPos == m_kContPlayerProgressPos.end())
	{
		return m_kContPlayerProgressPos.insert(std::make_pair(pkPlayer, kRecentPos)).second;
	}
	//if(itPlayerPos->second + 1 < kRecentPos) { return false; } //에러처리: 순차적인 진행이 아님!=>이 체크는 외부에 맡긴다.
	itPlayerPos->second = kRecentPos;

	return true;
}


bool PgRankingSystem::SetRankRecord(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos, DWORD dwRec, EVENTMAP_POS kCurPos)
{
	if(NULL == pkPlayer) { return false; }

	//선수의 현재 기록을 뒤져보고 있다면 지우자
	CONT_RANK_TREE::iterator itCurRankGroup = m_kRankTree.find(kCurPos);
	if(itCurRankGroup != m_kRankTree.end())
	{
		CONT_SORTED_RECORD& kCurRecord = itCurRankGroup->second;
		CONT_SORTED_RECORD::iterator itPlayerRecord = kCurRecord.begin();
		for(; itPlayerRecord != kCurRecord.end(); ++itPlayerRecord)
		{
			if(itPlayerRecord->second == pkPlayer)
			{
				kCurRecord.erase(itPlayerRecord);
				if(true == kCurRecord.empty())
				{
					m_kRankTree.erase(itCurRankGroup);
				}
				break;
			}
		}
	}

	//선수의 새 기록을 저장해준다. 그룹이 없다면 생성
	CONT_RANK_TREE::iterator itRecentRankGroup = m_kRankTree.find(kRecentPos);
	if(itRecentRankGroup == m_kRankTree.end())
	{
		CONT_SORTED_RECORD kNewRecord;
		auto kRet = m_kRankTree.insert(std::make_pair(kRecentPos, kNewRecord));
		if(false == kRet.second) { return false; }
		itRecentRankGroup = kRet.first;
	}
	CONT_SORTED_RECORD& kContPlayerRecord = itRecentRankGroup->second;
	kContPlayerRecord.insert(std::make_pair(dwRec, pkPlayer));

	return true;

}


bool PgRankingSystem::SetRecord(PgPlayer const* pkPlayer, EVENTMAP_POS kRecentPos, DWORD dwRec)
{
	if(NULL == pkPlayer) { return false; }
	EVENTMAP_POS kCurPos = GetPlayerPos(pkPlayer);
	if(false == SetPlayerPos(pkPlayer, kRecentPos)) { return false; }

	//선수 구간별 기록 저장
	CONT_PLAYER_LAPTIME::iterator itPlayerLapTime = m_kContPlayerLapTime.find(pkPlayer);
	if(itPlayerLapTime == m_kContPlayerLapTime.end())
	{
		auto kRet = m_kContPlayerLapTime.insert(std::make_pair(pkPlayer, CONT_LAPTIME()));
		if(false == kRet.second) { return true; }
		itPlayerLapTime = kRet.first;
	}
	CONT_LAPTIME& kContLapTime = itPlayerLapTime->second;
	CONT_LAPTIME::iterator iterPlayerLaptimeElem = kContLapTime.find(kRecentPos);
	if(iterPlayerLaptimeElem != kContLapTime.end())
	{
		iterPlayerLaptimeElem->second = dwRec;
	}
	else
	{
		auto kResult = kContLapTime.insert(std::make_pair(kRecentPos, dwRec) );
		if(kResult.second == false) { return false; }
	}

	SetRankRecord(pkPlayer, kRecentPos, dwRec, kCurPos);

	if(kRecentPos >= m_iTopPosition)
	{
		m_iTopPosition = kRecentPos;
	}

	return true;
}

bool PgRankingSystem::GetRankList(CONT_RANK_LIST& kOutList)
{
	if(false == kOutList.empty())
	{
		kOutList.clear();
	}

	DWORD dwCurRec = 0;
	EVENTMAP_POS kCurPos = 0;
	CONT_RANK_TREE::iterator itRankTree = m_kRankTree.begin();
	for(; itRankTree != m_kRankTree.end(); ++itRankTree)
	{
		EVENTMAP_POS kPos = itRankTree->first;
		if(kPos == 0) { continue; }

		CONT_SORTED_RECORD& kContRankGroup = itRankTree->second;
		CONT_SORTED_RECORD::const_iterator itRankGroup = kContRankGroup.begin();
		for(; itRankGroup != kContRankGroup.end(); ++itRankGroup)
		{
			PgPlayer const* pkPlayer = itRankGroup->second;
			if(kCurPos != kPos || dwCurRec != itRankGroup->first)
			{
				dwCurRec = itRankGroup->first;
				kCurPos = kPos;
			}
			int iSizeCont = kOutList.size();
			auto kRet = kOutList.insert(std::make_pair(iSizeCont + 1, pkPlayer));
			if(false == kRet.second) { return false; }

		}
	}

	return true;
}

DWORD PgRankingSystem::GetCurrentPlayerRecord(PgPlayer const* pkPlayer) const
{
	if(NULL==pkPlayer) { return 0; }
	CONT_PLAYERPOS::const_iterator itPlayerPos = m_kContPlayerPos.find(pkPlayer);
	if(itPlayerPos == m_kContPlayerPos.end()) { return 0; } 
	EVENTMAP_POS kPos = itPlayerPos->second; //플레이어의 최종위치는 어디인가? (체크포인트 트리거 인덱스)

	CONT_PLAYER_LAPTIME::const_iterator itPlayerLaptime = m_kContPlayerLapTime.find(pkPlayer);
	if(itPlayerLaptime == m_kContPlayerLapTime.end()) { return 0; }
	CONT_LAPTIME const& kLaptimeList = itPlayerLaptime->second;
	CONT_LAPTIME::const_iterator itCurLaptime = kLaptimeList.find(kPos);
	if(itCurLaptime == kLaptimeList.end()) { return 0; }

	return itCurLaptime->second; //최종 위치의 기록을 리턴
}

void PgRankingSystem::RemovePlayer(PgPlayer const* pkPlayer)
{
	m_kContPlayerPos.erase(pkPlayer);
	m_kContPlayerLapTime.erase(pkPlayer);

	CONT_RANK_TREE::iterator itRankTree = m_kRankTree.begin();
	for(; itRankTree != m_kRankTree.end(); ++itRankTree)
	{
		CONT_SORTED_RECORD* pkRec = &itRankTree->second;
		CONT_SORTED_RECORD::iterator itRec = pkRec->begin();
		for(; itRec != pkRec->end(); ++itRec)
		{
			if(itRec->second == pkPlayer)
			{
				pkRec->erase(itRec);
				break;
			}
		}
	}
}


void PgRaceProgress::RemovePlayer(PgPlayer const* pkPlayer)
{
	m_kContTrackPos.erase(pkPlayer);
}


void PgRaceProgress::SetTrack(RACE_TRACK_COUNT kTrackCount, EVENTMAP_POS kMaxPos)
{
	m_kTrackCount = kTrackCount;
	m_kMaxPos = kMaxPos;
}

void PgRaceProgress::InitProgress(void)
{
	CONT_RACE_TRACK_POS kEmptyCont;
	m_kContTrackPos.swap(kEmptyCont);
}

bool PgRaceProgress::IsCompleteRace(PgPlayer const* pkPlayer)
{
	if(pkPlayer == NULL) { return false; }
	CONT_RACE_TRACK_POS::iterator itFind = m_kContTrackPos.find(pkPlayer);
	if(itFind != m_kContTrackPos.end())
	{
		return (itFind->second >= m_kTrackCount);
	}

	return false;
}

BYTE PgRaceBilateralWayProgress::IsCorrectNextPos(PgPlayer const* pkPlayer, EVENTMAP_POS kCurPos, EVENTMAP_POS kNextPos)
{
	if(NULL == pkPlayer) { return 0; }
	BYTE kRet = ERPS_INCORRECT;
	if(m_kTrackCount == 0 || m_kMaxPos == 0) { return kRet; }

	CONT_RACE_TRACK_POS::iterator itFind = m_kContTrackPos.find(pkPlayer);
	if(itFind == m_kContTrackPos.end())
	{
		auto kRetFind = m_kContTrackPos.insert(std::make_pair(pkPlayer, 0));
		if(false == kRetFind.second) { return kRet; }
		itFind = kRetFind.first;
	}

	RACE_TRACK_COUNT* pkPlayerTrackCount = &itFind->second;
	if(NULL == pkPlayerTrackCount) { return kRet; }

	if((*pkPlayerTrackCount) == 0 || (*pkPlayerTrackCount) % 2 == 0) //짝수면 순방향 진행
	{
		if(kCurPos + 1 == kNextPos) { kRet |= ERPS_CORRECT; }
		if(kRet & ERPS_CORRECT && m_kMaxPos == kNextPos) //정주행시 마지막 위치에 도달하면
		{
			(*pkPlayerTrackCount)++;
			if((*pkPlayerTrackCount) >= m_kTrackCount) { kRet |= ERPS_FINISH; }
		}
	}
	else //홀수면 역방향 진행
	{
		if(kCurPos - 1 == kNextPos) { kRet |= ERPS_CORRECT; }
		if(kRet & ERPS_CORRECT && 1 == kNextPos) //역주행시 시작 위치에 도달하면
		{
			(*pkPlayerTrackCount)++;
			if((*pkPlayerTrackCount) >= m_kTrackCount) { kRet |= ERPS_FINISH; }
		}
	}


	return kRet;
}
