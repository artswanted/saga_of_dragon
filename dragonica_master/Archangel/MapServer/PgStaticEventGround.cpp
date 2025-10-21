#include "stdafx.h"
#include "Variant/PgLogUtil.h"
#include "PgStaticEventGround.h"
#include "Variant/PgPlayer.h"
#include "Variant/Unit.h"
#include "PgEffectFunction.h"
#include "Lohengrin/ActArg.h"
#include "PgPVPEffectSelector.h"
int const CHANGE_APPERANCE_EFFECT_NO = 60501;

//PVP 팀 셋팅 매니저-------------------------------------------------------------
PgGroundPVPTeamManager::PgGroundPVPTeamManager() :
	m_iTeamCount(0),
	m_kContPartyTeam()
{
}

PgGroundPVPTeamManager::~PgGroundPVPTeamManager()
{
}


void PgGroundPVPTeamManager::SetTeam(PgGround* pkGround, CUnit *pkUnit)
{
	switch(pkUnit->UnitType())
	{
	case UT_PLAYER:
		{
			BM::CAutoMutex kLock(m_kMutex);

			PgPlayer* pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if(!pkPlayer) { break; }

			BM::GUID kTeamGuid = BM::GUID::NullData();
			if(pkPlayer->GetExpeditionGuid().IsNotNull()) //우선적으로 원정대에 소속되었는지 검사
			{
				kTeamGuid = pkPlayer->GetExpeditionGuid();
			}
			else if(pkPlayer->GetPartyGuid().IsNotNull()) //원정대에 소속되지 않았다면 파티에 소속되었는지 검사
			{
				kTeamGuid = pkPlayer->GetPartyGuid();
			}

			if(kTeamGuid.IsNotNull()) //이 플레이어의 파티가 있고
			{
				CONT_PARTY_TEAM::const_iterator iter = m_kContPartyTeam.find(kTeamGuid);
				if(iter != m_kContPartyTeam.end()) //컨테이너에 등록된 파티의 팀번호가 있다면
				{
					pkPlayer->SetAbil(AT_TEAM, (*iter).second, true, true);
				}
				else //등록된 파티가 없다면 등록해야 한다.
				{
					auto ib = m_kContPartyTeam.insert(std::make_pair(kTeamGuid, m_iTeamCount + 1));
					if(ib.second) //파티 등록이 성공할 경우 카운트 증가 후 팀 그룹 셋팅
					{
						m_iTeamCount++;
						pkPlayer->SetAbil(AT_TEAM, m_iTeamCount, true, true);
					}
				}
			}
			else //파티가 없이 개인 참가일 경우
			{
				m_iTeamCount++;
				pkPlayer->SetAbil(AT_TEAM, m_iTeamCount, true, true);
			}

		}
		break;
	//case UT_PET: //얘는 어빌도 주인 따라 가는거니까 따로 SetAbil을 해주지 않아도 될듯
	//case UT_SUB_PLAYER: //얘는 어빌도 주인 따라 가는거니까 따로 SetAbil을 해주지 않아도 될듯
	case UT_SUMMONED:
		{
			if(!pkUnit->IsHaveCaller())
			{
				break;
			}
			CUnit* pkCallerUnit = pkGround->GetUnit(pkUnit->Caller());
			if(!pkCallerUnit || !pkCallerUnit->IsUnitType(UT_PLAYER))
			{
				break;
			}

			PgPlayer* pkCaller = dynamic_cast<PgPlayer*>(pkCallerUnit);
			if(!pkCaller)
			{
				break;
			}

			pkUnit->SetAbil(AT_TEAM, pkCaller->GetAbil(AT_TEAM), true, true);
		}
		break;
	default:
		{
		}
		break;
	}
}

void PgGroundPVPTeamManager::Cleanup(void)
{
	BM::CAutoMutex kLock(m_kMutex);
	m_iTeamCount = 0;
	m_kContPartyTeam.clear();
}
//-----------------------------------------------------------------------------------------




PgStaticEventGround::PgStaticEventGround()
{
}

PgStaticEventGround::~PgStaticEventGround()
{
}

EOpeningState PgStaticEventGround::Init(const int iMonsterControlID /*= 0*/, const bool bMonsterGen /*= true*/)
{
	m_EventSchedule.clear();
	m_nCurrentUserCount = 0;
	m_nAlivePlayerCount = 0;
	Clear();

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
	m_nCurrentSpawnLocNum = 0;

	return PgGround::Init(iMonsterControlID, bMonsterGen);
}

bool PgStaticEventGround::Clone(PgGround *pGround)
{
	return true;
}

void PgStaticEventGround::Clear()
{
	m_RegenEventMonsterInfo.clear();
	m_PartySpawnLocList.clear();
	m_kTeamMgr.Cleanup();
	m_JoinPartyMemberList.clear();
	m_nRegenMaxIdx = 0;
	m_nRegenCurrentIdx = 0;
	m_nRegenMonsterCount = 0;
	m_nCurrentSpawnLocNum = 0;
	m_EventLimitTime = 0;
	m_dwNextMonsterGenDelayTime = 0;
	m_bBossRegenOnce = false;
	m_bSuccessEvent = false;

	m_kCurrentEventNo = 0;
	m_kIsPK = false;
	m_kUnusableReviveItem = false;
	m_kUnusableConsumeItem = false;

	m_bSendMonsterRegenWarnning = false;
}

void PgStaticEventGround::OnTick1s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	
	DWORD dwElapsed = 0;
	DWORD const dwNow = BM::GetTime32();

	bool bTimeUpdate = true;
	if( EGS_PLAY == m_eState )
	{
		bTimeUpdate = false;
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
			if( m_nRegenMonsterCount <= 0 )
			{
				if( (m_dwNextMonsterGenDelayTime == 0) || (m_dwNextMonsterGenDelayTime <= dwElapsed) )
				{
					GenerateNextMonster();
					m_dwNextMonsterGenDelayTime = 0;
				}

				if( 0 < m_dwNextMonsterGenDelayTime )
				{					
					if( 10000 < m_dwNextMonsterGenDelayTime && m_dwNextMonsterGenDelayTime <= 11000 && m_bSendMonsterRegenWarnning )
					{	// 10초 남았다고 알려줌.
						VEC_REGEN_EVENT_MONSTER_INFO::iterator first_iter = m_RegenEventMonsterInfo[m_nRegenCurrentIdx].begin();
						if( first_iter != m_RegenEventMonsterInfo[m_nRegenCurrentIdx].end() )
						{
							BM::Stream ToMapPacket(PT_M_C_NFY_REMAIN_TIME_MONSTER_GENERATE);
							ToMapPacket.Push( 10 );
							ToMapPacket.Push( (*first_iter).MonsterNo );
							Broadcast(ToMapPacket);

							m_bSendMonsterRegenWarnning = false;
						}
					}
					else
					{
						m_dwNextMonsterGenDelayTime -= dwElapsed;
					}
				}
			}

			if( 0 < m_EventLimitTime )
			{
				if( m_EventLimitTime <= dwElapsed )
				{
					if( m_bBossRegenOnce )
					{
						m_bSuccessEvent = false;
					}
					else
					{
						m_bSuccessEvent = true;
					}
					SetState(EGS_END_WAIT);
				}
				else
				{
					m_EventLimitTime -= dwElapsed;
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

	// 5분 간격으로 남기는 로그.
	// 현재 진행 중인 이벤트 번호와, 시간.
	static DWORD s_CommunityEvent_LogTime = 0;
	if( BM::TimeCheck(s_CommunityEvent_LogTime, 300000) )
	{
		SYSTEMTIME time;
		g_kEventView.GetLocalTime(&time);

		PgLogCont LogCont(ELogMain_Community_Event, ELogSub_Community_Event_Tick);
		LogCont.GroundNo(GroundKey().GroundNo());			// 그라운드 번호.
		LogCont.ChannelNo( g_kProcessCfg.ChannelNo() );		// 채널 번호.

		PgLog Log(ELOrderMain_CommunityEvent);
		Log.Set(0, static_cast<int>(CurrentEventNo()));		// 현재 진행 중인 이벤트 번호.

		wchar_t wszTemp[200] = {0,};
		swprintf_s(wszTemp, 200, _T("%d-%d-%d %d:%d:%d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

		std::wstring LogMsg(wszTemp);

		Log.Set(0, LogMsg);		// 현재 시간.

		LogCont.Add(Log);
		LogCont.Commit();
	}
}

void PgStaticEventGround::SetState(EEventGroundState const State, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex kLock(m_kRscMutex);

	if( m_eState == State )
	{	// 현재 상태와 같다면 상태를 변경할 필요가 없다.
		return;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = State;

	{	// 상태가 바뀔 때 마다 남기는 로그.
		SYSTEMTIME time;
		g_kEventView.GetLocalTime(&time);

		PgLogCont LogCont(ELogMain_Community_Event, ELogSub_Community_Event_State);
		LogCont.GroundNo(GroundKey().GroundNo());				// 그라운드 번호.
		LogCont.ChannelNo( g_kProcessCfg.ChannelNo() );			// 채널 번호.

		PgLog Log(ELOrderMain_CommunityEvent);
		Log.Set(0, static_cast<int>(m_eState));		// 현재 상태.
		
		wchar_t wszTemp[200] = {0,};
		swprintf_s(wszTemp, 200, _T("%d-%d-%d %d:%d:%d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

		std::wstring LogMsg(wszTemp);

		Log.Set(0, LogMsg);		// 현재 시간.
		
		LogCont.Add(Log);
		LogCont.Commit();
	}

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
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(true);
			SendToContents(ToContentsPacket);

			ToContentsPacket.Clear();
			ToContentsPacket.Push(PT_M_N_NFY_EVENT_GROUND_USER_COUNT_MODIFY);
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(m_nCurrentUserCount);

			SendToContents(ToContentsPacket);

			{	// 이벤트가 시작될 때 남기는 록.
				SYSTEMTIME time;
				g_kEventView.GetLocalTime(&time);

				PgLogCont LogCont(ELogMain_Community_Event, ELogSub_Community_Event_NotifyStart);
				LogCont.GroundNo(GroundKey().GroundNo());			// 그라운드 번호.
				LogCont.ChannelNo( g_kProcessCfg.ChannelNo() );		// 채널 번호.

				PgLog Log(ELOrderMain_CommunityEvent, ELOrderSub_Start);
				Log.Set(0, static_cast<int>(CurrentEventNo()));		// 시작하는 이벤트 번호.

				wchar_t wszTemp[200] = {0,};
				swprintf_s(wszTemp, 200, _T("%d-%d-%d %d:%d:%d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

				std::wstring LogMsg(wszTemp);

				Log.Set(0, LogMsg);		// 현재 시간.

				LogCont.Add(Log);
				LogCont.Commit();
			}

			SetAutoNextState(g_kEventView.VariableCont().CommunityEventWaitTime * 1000 + 2000);
		}break;
	case EGS_READY:
		{	// 60초 카운트하고 이벤트 시작.
			BM::Stream SendPacket(PT_M_C_NFY_COMMUNITY_EVENT_READY);
			SendPacket.Push(CurrentEventNo());
			Broadcast(SendPacket);

			BM::Stream ToContentsPacket(PT_M_N_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE);
			ToContentsPacket.Push(m_kCurrentEventNo);
			ToContentsPacket.Push(false);
			SendToContents(ToContentsPacket);

			SetAutoNextState(g_kEventView.VariableCont().CommunityEventReadyTime * 1000 + 2000);
		}break;
	case EGS_PLAY:
		{	
			if( m_nCurrentUserCount == 0 )
			{
				SetState(EGS_WAIT_SHORTTIME);
				break;
			}
			// 팀세팅.
			if( IsPK() )
			{
				SetPVPTeam();
			}

			BM::Stream SendPacket(PT_M_C_NFY_COMMUNITY_EVENT_START);
			SendPacket.Push(CurrentEventNo());
			Broadcast(SendPacket);

			// 시작할 때 살아있는 유저 수 = 접속 중인 유저 수.
			m_nAlivePlayerCount = m_nCurrentUserCount;

			// 스타트 이펙트 걸어줘야 함.
			if( m_EventStartEffectNo > 0 )
			{
				CUnit * pUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator Itor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
				while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
				{
					SEffectCreateInfo Create;
					Create.eType = EFFECT_TYPE_NORMAL;
					Create.iEffectNum = m_EventStartEffectNo;
					Create.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
					pUnit->AddEffect(Create);					
				}				
			}

			if( m_RegenEventMonsterInfo.size() >= m_nRegenCurrentIdx + 1 )
			{
				VEC_REGEN_EVENT_MONSTER_INFO::iterator first_iter = m_RegenEventMonsterInfo[m_nRegenCurrentIdx].begin();
				if( first_iter != m_RegenEventMonsterInfo[m_nRegenCurrentIdx].end() )
				{
					m_dwNextMonsterGenDelayTime = (*first_iter).DelayTime * 1000 + 2000;

					BM::Stream ToMapPacket(PT_M_C_NFY_REMAIN_TIME_MONSTER_GENERATE);
					ToMapPacket.Push( (*first_iter).DelayTime );
					ToMapPacket.Push( (*first_iter).MonsterNo );
					Broadcast(ToMapPacket);

					m_bSendMonsterRegenWarnning = true;
				}
			}
		}break;
	case EGS_WAIT_SHORTTIME:
		{
			SetAutoNextState(2000);
		}break;
	case EGS_END_WAIT:
		{
			BM::Stream SendPacket(PT_M_C_NFY_COMMUNITY_EVENT_END);
			SendPacket.Push(CurrentEventNo());
			SendPacket.Push(m_bSuccessEvent);
			Broadcast(SendPacket);

			RestorePVPTeam();
			
			RemoveAllMonster(true, NULL, 1);
			
			SetAutoNextState(60000);
		}break;
	case EGS_END:
		{
			m_RegenEventMonsterInfo.clear();
			KickUserFromGround();
		}break;
	default:
		{
		}break;
	}
}

void PgStaticEventGround::UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime)
{
	if( 0 == m_dwAutoStateRemainTime )
	{
		return;
	}

	if( dwElapsedTime >= m_dwAutoStateRemainTime )
	{
		EEventGroundState eState = (EEventGroundState)(m_eState << 1);

		SetState( eState, true );
	}
	else
	{
		m_dwAutoStateRemainTime -= dwElapsedTime;
	}
}

bool PgStaticEventGround::Clone(PgStaticEventGround *pGround)
{
	return true;
}

void PgStaticEventGround::AddJoinPartyMemberList(BM::Stream & Packet)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	BM::GUID CharGuid;
	BM::GUID PartyGuid;
	bool bMaster;

	Packet.Pop(CharGuid);
	Packet.Pop(PartyGuid);
	Packet.Pop(bMaster);

	SJoinPartyMember JoinPartyMember;
	JoinPartyMember.CharGuid = CharGuid;
	JoinPartyMember.PartyGuid = PartyGuid;
	JoinPartyMember.bMaster = bMaster;

	auto Ret = m_JoinPartyMemberList.insert( std::make_pair(CharGuid, JoinPartyMember) );
	if( !Ret.second )
	{
		// 중복?
	}

	// 파티 스폰 위치 등록.
	CONT_PARTY_SPAWN_LOC_LIST::iterator iter = m_PartySpawnLocList.find( PartyGuid );
	if( iter == m_PartySpawnLocList.end() )
	{
		m_PartySpawnLocList.insert( std::make_pair( PartyGuid, m_nCurrentSpawnLocNum ) );
		if( ++m_nCurrentSpawnLocNum == MaxSpawnLocationCount() )
		{
			m_nCurrentSpawnLocNum = 0;
		}
	}
}

void PgStaticEventGround::SendMapLoadComplete(PgPlayer * pUser)
{
	PgGround::SendMapLoadComplete(pUser);

	if( IsPK() )
	{
		SActArg Arg;
		PgGroundUtil::SetActArgGround(Arg, this);
		g_kPVPEffectSlector.AddEffect(dynamic_cast<CUnit *>(pUser), &Arg);

		pUser->SetAbil(AT_HP, pUser->GetAbil(AT_C_MAX_HP), true, true);
		pUser->SetAbil(AT_MP, pUser->GetAbil(AT_C_MAX_MP), true, true);
	}

	CONT_JOIN_PARTY_MEMBER_LIST::iterator find_iter = m_JoinPartyMemberList.find(pUser->GetID());
	if( find_iter != m_JoinPartyMemberList.end() )
	{
		CONT_PARTY_SPAWN_LOC_LIST::iterator spawn_iter = m_PartySpawnLocList.find( find_iter->second.PartyGuid );
		if( spawn_iter != m_PartySpawnLocList.end() )
		{
			int SpawnLoc = spawn_iter->second;

			int nPortalNum = 0;
			int nSpawnCountPerLocation = SpawnCountPerLocation();
			if( nSpawnCountPerLocation > 0 )
			{
				nPortalNum = MaxSpawnLocationCount() * (rand() % nSpawnCountPerLocation) + SpawnLoc;
			}

			SendToSpawnLoc(dynamic_cast<CUnit*>(pUser), nPortalNum, true, E_SPAWN_EVENT_GROUND);
			
			DelJoinPartyMemberList(BM::GUID(pUser->GetID()));
		}
	}
	else
	{
		int SpawnLoc = m_nCurrentSpawnLocNum++;
		if( m_nCurrentSpawnLocNum == MaxSpawnLocationCount() )
		{
			m_nCurrentSpawnLocNum = 0;
		}

		int nPortalNum = 0;
		int nSpawnCountPerLocation = SpawnCountPerLocation();
		if( nSpawnCountPerLocation > 0)
		{
			nPortalNum = MaxSpawnLocationCount() * (rand() % nSpawnCountPerLocation) + SpawnLoc;
		}

		SendToSpawnLoc(dynamic_cast<CUnit*>(pUser), nPortalNum, true, E_SPAWN_EVENT_GROUND);
	}
	if(pUser)
	{// 아군, 그외 복장 변경 이펙트
		SActArg kArg;
		PgGroundUtil::SetActArgGround(kArg, this);
		pUser->AddEffect(CHANGE_APPERANCE_EFFECT_NO, 0, &kArg, NULL);
	}
	
	BM::Stream NfyPacket(PT_M_C_NFY_EVENT_GROUND_MOVE_COMPLETE);
	NfyPacket.Push(CurrentEventNo());
	NfyPacket.Push(m_dwAutoStateRemainTime);
	pUser->Send(NfyPacket);
}

bool PgStaticEventGround::VUpdate(CUnit * pUnit, WORD const wType, BM::Stream * pNfy)
{
	switch(wType)
	{
	case PT_C_M_REQ_DEFAULT_MAP_MOVE:
		{
			PgPlayer * pPlayer = dynamic_cast< PgPlayer* >(pUnit);
			if( pPlayer )
			{ 
				pPlayer->ClearAllEffect(true);

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
			m_nCurrentUserCount += 1;
			BM::Stream Packet(PT_M_C_NFY_EVENT_GROUND_USER_COUNT_MODIFY, m_nCurrentUserCount);
			Broadcast(Packet);			
		} // break; 없음.
	default:
		{
			return PgGround::VUpdate(pUnit, wType, pNfy);
		}break;
	}
	return true;
}

bool PgStaticEventGround::GenerateGenInfo(int EventNo)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( 0 == EventNo )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Wrong EventNo"));
		return false;
	}

	CONT_DEF_EVENT_BOSSBATTLE const * pEventBossBattle = NULL;
	g_kTblDataMgr.GetContDef(pEventBossBattle);

	if( NULL == pEventBossBattle )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event_BossBattle Table is NULL!"));
		return false;
	}

	CONT_DEF_EVENT_MONSTER_GROUP const * pEventMonsterGroup = NULL;
	g_kTblDataMgr.GetContDef(pEventMonsterGroup);

	if( NULL == pEventMonsterGroup )
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("Event Monster Group Table is NULL!"));
		return false;
	}

	CONT_DEF_EVENT_BOSSBATTLE::const_iterator boss_iter = pEventBossBattle->find(EventNo);
	if( boss_iter != pEventBossBattle->end() )
	{
		CONT_DEF_EVENT_BOSSBATTLE::mapped_type const & Element = boss_iter->second;

		// 이벤트 시작 이펙트.
		m_EventStartEffectNo = Element.StartEffect;

		// 이벤트 제한 시간.
		m_EventLimitTime = Element.LimitTime * 1000;

		int MonsterGroupNo = Element.MonsterGroupNo;
		while( MonsterGroupNo )
		{
			CONT_DEF_EVENT_MONSTER_GROUP::const_iterator monster_iter = pEventMonsterGroup->find(MonsterGroupNo);
			if( monster_iter != pEventMonsterGroup->end() )
			{
				CONT_EVENTMONSTERGROUP::const_iterator monsterinfo_iter = monster_iter->second.find(this->GetGroundNo());

				if( monsterinfo_iter !=  monster_iter->second.end() )
				{

					VEC_REGEN_EVENT_MONSTER_INFO vec_info;
					VEC_EVENTMONSTERINFO::const_iterator vec_iter = monsterinfo_iter->second.MonsterInfo.begin();
					for( ; vec_iter != monsterinfo_iter->second.MonsterInfo.end() ; ++vec_iter )
					{
						SEventMonsterInfo MonsterInfo;
						MonsterInfo.MonsterNo			= vec_iter->MonsterNo;
						MonsterInfo.GenPos				= vec_iter->GenPos;
						MonsterInfo.DelayTime			= vec_iter->GenDelay;
						MonsterInfo.AdjustLevel			= vec_iter->MonsterLevel;
						MonsterInfo.AdjustHP_Rate		= vec_iter->DifficultyHP;
						MonsterInfo.AdjustDamage_Rate	= vec_iter->DifficultyDamage;
						MonsterInfo.AdjustDefence_Rate	= vec_iter->DifficultyDefence;
						MonsterInfo.AdjustExp_Rate		= vec_iter->DifficultyExp;
						MonsterInfo.AdjustAttackSpeed	= vec_iter->DifficultyAttackSpeed;
						MonsterInfo.AdjustHitRate		= vec_iter->DifficultyHitrate;
						MonsterInfo.AdjustEvade			= vec_iter->DifficultyEvade;
						MonsterInfo.RewardItemGroupNo	= vec_iter->RewardItemGroup;
						MonsterInfo.RewardItemCount		= vec_iter->RewardItemCount;

						vec_info.push_back(MonsterInfo);
					}

					m_RegenEventMonsterInfo.push_back(vec_info);
					vec_info.clear();

					VEC_EVENTMONSTERINFO::const_iterator first_iter = monsterinfo_iter->second.MonsterInfo.begin();
					if( first_iter != monsterinfo_iter->second.MonsterInfo.end() )
					{
						int NextOrderNo = first_iter->Order;
						if( (MonsterGroupNo == NextOrderNo) || (0 == NextOrderNo) )
						{
							MonsterGroupNo = 0;
							break;
						}
						else
						{
							MonsterGroupNo = NextOrderNo;
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV2, __FL__ << _T("EventMonsterGroup Table is Empty"));
					return false;
				}
			}
			else
			{
				MonsterGroupNo = 0;
				break;
			}
		}

		if( m_RegenEventMonsterInfo.empty() )
		{
			return false;
		}

		m_nRegenMaxIdx = m_RegenEventMonsterInfo.size();

		if( ( boss_iter->second.MonsterRegenType == EBRT_RANDOM_REGEN_IN_LIMITTIME )
			|| ( boss_iter->second.MonsterRegenType == EBRT_RANDOM_REGEN_ONCE ) )
		{
			std::random_shuffle(m_RegenEventMonsterInfo.begin(), m_RegenEventMonsterInfo.end());
		}

		if( (boss_iter->second.MonsterRegenType == EBRT_RANDOM_REGEN_ONCE)
			|| (boss_iter->second.MonsterRegenType == EBRT_SEQUENTIAL_REGEN_ONCE) )
		{
			m_bBossRegenOnce = true;
		}
	}

	return true;
}

void PgStaticEventGround::CheckEventStartable(void)
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
			kEndTime.byHour += ((dwEventSec >= 3600) ? dwEventSec / 3600 : 0);
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
					IsPK( Element.PKOption );
					UnusableReviveItem( Element.UnUsableReviveItem);
					UnusableConsumeItem( Element.UnUsableConsumeItem);

					if( GenerateGenInfo(Element.EventNo) )
					{
						SetState(EGS_START_WAIT);
						break;
					}
				} //if(iElapsedCurEventTimeBySec < 600)
			} //if(iStartTimeBySec <= iCurTimeBySec && iCurTimeBySec <= iEndTimeBySec)
		}
	} //for(CONT_EVENT_SCHEDULE::const_iterator....
}

void PgStaticEventGround::RecvUnitDie(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgGround::RecvUnitDie(pUnit);

	if( NULL == pUnit )
	{
		return;
	}

	EUnitType UnitType = pUnit->UnitType();
	switch(UnitType)
	{
	case UT_PLAYER:
		{
			if( GetEventGroundState() == EGS_PLAY )
			{
				m_nAlivePlayerCount -= 1;
				if( m_nAlivePlayerCount <= 0 && UnusableReviveItem())
				{
					m_bSuccessEvent = false;
					SetState(EGS_WAIT_SHORTTIME);
				}
			}
		}break;
	case UT_BOSSMONSTER:
	case UT_MONSTER:
		{
			PgMonster* pkMonster = dynamic_cast<PgMonster*>(pUnit);
			if(pkMonster && pkMonster->GetAbil(AT_GRADE) >= EMGRADE_ELITE)
			{ //정예 몬스터 이상을 처치하면 시스템 메시지를 띄워줌
				BM::GUID const kCasterGuid = pUnit->GetTarget();
				CUnit * pkCaster = GetUnit(kCasterGuid);
				//BM::GUID kKillerGuid(kCasterGuid);
				if(pkCaster && 0==pkMonster->GetAbil(AT_VOLUNTARILY_DIE))
				{
					switch(pkCaster->UnitType())
					{
					case UT_PET:
					case UT_SUMMONED:
					case UT_SUB_PLAYER:
					case UT_ENTITY:
						{
							pkCaster = GetUnit(pkCaster->Caller());
						}break;
					//default:
					//	{
					//		kKillerGuid = GlobalHelper::FindOwnerGuid(pkCaster);
					//	}break;
					}
					BM::Stream kPacket(PT_M_C_NFY_CHAT);
					kPacket.Push(static_cast<BYTE>(CT_RAREMONSTERDEAD));
					kPacket.Push(pkMonster->GetID());
					kPacket.Push(pkMonster->GetAbil(AT_CLASS));
					kPacket.Push(pkCaster->Name());
					Broadcast(kPacket,NULL,E_SENDTYPE_SELF);
				}
			}


			if( pUnit->GetAbil(AT_EVENT_MONSTER_DROP) && (GetEventGroundState() == EGS_PLAY) )
			{
				--m_nRegenMonsterCount;
				if( 0 >= m_nRegenMonsterCount )
				{
					RemoveAllMonster(true);

					if( m_bBossRegenOnce && (m_nRegenMaxIdx == m_nRegenCurrentIdx) )
					{
						m_bSuccessEvent = true;
						SetState(EGS_WAIT_SHORTTIME);
					}
					else
					{
						if( m_nRegenMaxIdx == m_nRegenCurrentIdx )
						{
							m_nRegenCurrentIdx = 0;
						}

						if( m_RegenEventMonsterInfo.size() >= m_nRegenCurrentIdx + 1 )
						{
							VEC_REGEN_EVENT_MONSTER_INFO::iterator first_iter = m_RegenEventMonsterInfo[m_nRegenCurrentIdx].begin();
							if( first_iter != m_RegenEventMonsterInfo[m_nRegenCurrentIdx].end() )
							{
								m_dwNextMonsterGenDelayTime = (*first_iter).DelayTime * 1000 + 2000;

								BM::Stream ToMapPacket(PT_M_C_NFY_REMAIN_TIME_MONSTER_GENERATE);
								ToMapPacket.Push( (*first_iter).DelayTime );
								ToMapPacket.Push( (*first_iter).MonsterNo );
								Broadcast(ToMapPacket);

								m_bSendMonsterRegenWarnning = true;
							}
						}
					}
				}
			}
		}break;
	default:
		{
		}break;
	}
}

void PgStaticEventGround::GenerateNextMonster(void)
{
	BM::CAutoMutex Lock(m_kRscMutex);


	if( m_RegenEventMonsterInfo.size() >= m_nRegenCurrentIdx + 1 )
	{
		VEC_REGEN_EVENT_MONSTER_INFO::iterator gen_iter = m_RegenEventMonsterInfo[m_nRegenCurrentIdx].begin();

		for( ; gen_iter != m_RegenEventMonsterInfo[m_nRegenCurrentIdx].end() ; ++gen_iter )
		{
			if( gen_iter->AdjustLevel > 0 )
			{	// 몬스터 튜닝 설정.
				m_iTunningLevel = gen_iter->AdjustLevel;
				m_iGroundTunningNo = 10;
			}
			else
			{
				m_iTunningLevel = 0;
				m_iGroundTunningNo = 0;
			}

			BM::GUID InsertedMonsterGuid;

			if( gen_iter->GenPos == POINT3::NullData() )
			{
				PgGenPoint GenPoint;
				GetRandomMonsterGenPoint(12, GenPoint);
				InsertMonster(GenPoint.Info(), gen_iter->MonsterNo, InsertedMonsterGuid);
			}
			else
			{
				TBL_DEF_MAP_REGEN_POINT GenPoint;
				GenPoint.iMapNo = GetGroundNo();
				GenPoint.pt3Pos = gen_iter->GenPos;
				InsertMonster(GenPoint, gen_iter->MonsterNo, InsertedMonsterGuid);
			}

			if( BM::GUID::IsNotNull(InsertedMonsterGuid) )
			{	// 이벤트 몬스터 설정.
				CUnit * pUnit = GetUnit(InsertedMonsterGuid);
				if( pUnit )
				{
					pUnit->SetAbil(AT_EVENT_MONSTER_DROP, 1);
					pUnit->SetAbil(AT_EVENT_MONSTER_DROP_GROUP_NO, gen_iter->RewardItemGroupNo);
					pUnit->SetAbil(AT_EVENT_MONSTER_DROP_ITEM_COUNT, gen_iter->RewardItemCount);
					pUnit->SetAbil(AT_EVENT_MONSTER_NO_HAVE_GENINFO, 1);

					if( gen_iter->AdjustHP_Rate > 0 )
					{
						OnAddAbil(pUnit, AT_R_MAX_HP, gen_iter->AdjustHP_Rate);
						pUnit->CalculateAbil(AT_C_MAX_HP);
						OnSetAbil(pUnit, AT_HP, pUnit->GetAbil(AT_C_MAX_HP));
					}

					if( gen_iter->AdjustDamage_Rate > 0 )
					{
						OnAddAbil(pUnit, AT_R_PHY_ATTACK_MAX, gen_iter->AdjustDamage_Rate);
						OnAddAbil(pUnit, AT_R_PHY_ATTACK_MIN, gen_iter->AdjustDamage_Rate);
						OnAddAbil(pUnit, AT_R_MAGIC_ATTACK_MAX, gen_iter->AdjustDamage_Rate);
						OnAddAbil(pUnit, AT_R_MAGIC_ATTACK_MIN, gen_iter->AdjustDamage_Rate);

						pUnit->CalculateAbil(AT_C_PHY_ATTACK_MAX);
						pUnit->CalculateAbil(AT_C_PHY_ATTACK_MIN);
						pUnit->CalculateAbil(AT_C_MAGIC_ATTACK_MAX);
						pUnit->CalculateAbil(AT_C_MAGIC_ATTACK_MIN);
					}

					if( gen_iter->AdjustDefence_Rate > 0 )
					{
						OnAddAbil(pUnit, AT_R_PHY_DEFENCE, gen_iter->AdjustDefence_Rate);
						OnAddAbil(pUnit, AT_R_MAGIC_DEFENCE, gen_iter->AdjustDefence_Rate);

						pUnit->CalculateAbil(AT_C_PHY_DEFENCE);
						pUnit->CalculateAbil(AT_C_MAGIC_DEFENCE);
					}

					if( gen_iter->AdjustExp_Rate > 0 )
					{
						__int64 Exp = pUnit->GetAbil64(AT_DIE_EXP);
						pUnit->SetAbil64(AT_DIE_EXP, Exp + (__int64)((float)Exp * ((float)gen_iter->AdjustExp_Rate / 10000)));
					}

					if( gen_iter->AdjustAttackSpeed > 0 )
					{
						OnAddAbil(pUnit, AT_ATTACK_SPEED, gen_iter->AdjustAttackSpeed);
					}

					if( gen_iter->AdjustEvade > 0 )
					{
						pUnit->SetAbil(AT_DODGE_SUCCESS_VALUE, pUnit->GetAbil(AT_FINAL_DODGE_SUCCESS_RATE) + gen_iter->AdjustEvade);
					}

					if( gen_iter->AdjustHitRate > 0 )
					{
						pUnit->SetAbil(AT_HIT_SUCCESS_VALUE, pUnit->GetAbil(AT_FINAL_HIT_SUCCESS_RATE) + gen_iter->AdjustEvade);
					}
				}
			}

			++m_nRegenMonsterCount;
		}

		++m_nRegenCurrentIdx;
	}
}

void PgStaticEventGround::SetPVPTeam(void)
{
	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		m_kTeamMgr.SetTeam(this, pUnit);
	}

	PgObjectMgr::GetFirstUnit(UT_SUMMONED, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_SUMMONED, Itor)) != NULL )
	{
		m_kTeamMgr.SetTeam(this, pUnit);
	}

	//UT_PET, UT_SUB_PLAYER는 어빌이 주인 따라가므로 해줄필요 없다.
}

void PgStaticEventGround::RestorePVPTeam(void)
{
	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		pUnit->SetAbil(AT_TEAM, 0, true, true);
	}

	PgObjectMgr::GetFirstUnit(UT_SUMMONED, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_SUMMONED, Itor)) != NULL )
	{
		pUnit->SetAbil(AT_TEAM, 0, true, true);
	}

	//UT_PET, UT_SUB_PLAYER는 어빌이 주인 따라가므로 해줄필요 없다.

	m_kTeamMgr.Cleanup();
}

void PgStaticEventGround::KickUserFromGround(void)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	// 루프를 두번 도는데 한 번으로 처리할 수 있는 방법이 있는지 찾아서 수정해야 함.
	CUnit * pUnit = NULL;
	CONT_OBJECT_MGR_UNIT::iterator Itor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		PgPlayer * pPlayer = dynamic_cast< PgPlayer* >(pUnit);
		if( pPlayer )
		{
			SRecentPlayerNormalMapData NormalMap;
			pPlayer->GetRecentNormalMap(NormalMap);

			SChannelMapMove MoveInfo;
			MoveInfo.iChannelNo = NormalMap.ChannelNo;
			MoveInfo.iGroundNo = NormalMap.GroundNo;

			if( BM::GUID::IsNotNull(pPlayer->PartyGuid()) )
			{
				bool bMaster = m_kLocalPartyMgr.IsMaster(pPlayer->PartyGuid(), pPlayer->GetID());

				if( bMaster )
				{
					BM::Stream ToCenter(PT_M_N_PRE_MAKE_PARTY_EVENT_GROUND);
					ToCenter.Push(static_cast<int>(NormalMap.ChannelNo));
					ToCenter.Push(pPlayer->PartyGuid());
					SendToContents(ToCenter);
				}
			}
		}
	}

	PgObjectMgr::GetFirstUnit(UT_PLAYER, Itor);
	while( (pUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, Itor)) != NULL )
	{
		PgPlayer * pPlayer = dynamic_cast< PgPlayer* >(pUnit);
		if( pPlayer )
		{
			SRecentPlayerNormalMapData NormalMap;
			pPlayer->GetRecentNormalMap(NormalMap);

			SChannelMapMove MoveInfo;
			MoveInfo.iChannelNo = NormalMap.ChannelNo;
			MoveInfo.iGroundNo = NormalMap.GroundNo;

			if( BM::GUID::IsNotNull(pPlayer->PartyGuid()) )
			{
				bool bMaster = m_kLocalPartyMgr.IsMaster(pPlayer->PartyGuid(), pPlayer->GetID());

				BM::Stream ToNormalMap(PT_M_N_REQ_JOIN_ANOTHER_CHANNEL_PARTY);
				ToNormalMap.Push(static_cast<int>(NormalMap.ChannelNo));
				ToNormalMap.Push(NormalMap.GroundNo);
				ToNormalMap.Push(pPlayer->GetID());
				ToNormalMap.Push(pPlayer->PartyGuid());
				ToNormalMap.Push(bMaster);

				SendToContents(ToNormalMap);
			}

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
	}

	SetState(EGS_IDLE);
}

bool PgStaticEventGround::ReleaseUnit(CUnit * pUnit, bool bRecursiveCall/* = false*/, bool const bSendArea/* = true*/)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( pUnit && pUnit->UnitType() == UT_PLAYER )
	{
		m_nCurrentUserCount -= 1;
		BM::Stream Packet(PT_M_C_NFY_EVENT_GROUND_USER_COUNT_MODIFY, m_nCurrentUserCount);
		Broadcast(Packet);

		BM::Stream ToContentsPacket(PT_M_N_NFY_EVENT_GROUND_USER_COUNT_MODIFY);
		ToContentsPacket.Push(m_kCurrentEventNo);
		ToContentsPacket.Push(m_nCurrentUserCount);
		SendToContents(ToContentsPacket);

		pUnit->ClearAllEffect(true);

		if( (GetEventGroundState() == EGS_PLAY) )
		{
			m_nAlivePlayerCount -= 1;
			if( m_nCurrentUserCount <= 0 )
			{
				m_bSuccessEvent = false;
				SetState(EGS_WAIT_SHORTTIME);
			}
		}
	}

	return PgGround::ReleaseUnit(pUnit, bRecursiveCall, bSendArea);
}

void PgStaticEventGround::RevivePlayer(void)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( GetEventGroundState() == EGS_PLAY )
	{
		m_nAlivePlayerCount += 1;
	}
}

void PgStaticEventGround::GMCommandStartEvent(int EventNo)
{	// 지정된 이벤트를 바로 시작한다.
	if( GetEventGroundState() != EGS_IDLE )
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
		IsPK( Element.PKOption );
		UnusableReviveItem( Element.UnUsableReviveItem);
		UnusableConsumeItem( Element.UnUsableConsumeItem);

		if( GenerateGenInfo(Element.EventNo) )
		{
			SetState(EGS_START_WAIT);
		}
	}
}

void PgStaticEventGround::GMCommandEndEvent()
{	// 진행하고 있는 이벤트 강제로 끝냄.
	if( GetEventGroundState() == EGS_PLAY )
	{
		m_bSuccessEvent = true;
		SetState(EGS_WAIT_SHORTTIME);
	}
}