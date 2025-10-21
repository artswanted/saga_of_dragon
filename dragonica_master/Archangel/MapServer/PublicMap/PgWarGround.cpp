#include "stdafx.h"
#include "BM/PgFilterString.h"
#include "Collins/Log.h"
#include "Variant/PgEventView.h"
#include "Variant/PgQuestInfo.h"
#include "Variant/PgMission.h"
#include "Variant/PgLogUtil.h"
#include "constant.h"
#include "Global.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgPvPType_Annihilation.h"
#include "PgPvPType_Destroy.h"
#include "PgBattleForEmporia.h"
#include "PgWarType_DestroyCore.h"
#include "PgWarType_ProtectDragon.h"
#include "PgWarGround.h"
#include "PgWarMode.h"
#include "PgPvPType_WinnersMatch.h"
#include "PgPvPType_WinnersTeam.h"
#include "PgPvPType_AceMatch.h"
#include "PgPvPMode_KingOfHill.h"
#include "PgPvPType_Love.h"

inline bool PgWarGround::IsModeType( EPVPTYPE const kChkType )const
{
	return m_pkMode && (m_pkMode->Type() & kChkType);
}

PgWarGround::PgWarGround()
:	m_pkMode(NULL)
{
}

PgWarGround::~PgWarGround()
{
	SAFE_DELETE(m_pkMode);
}

EOpeningState PgWarGround::Init( int const iMonsterControlID, bool const bMonsterGen )
{
	EOpeningState eRet = PgIndun::Init( iMonsterControlID, false );

	m_ContInsItemDropInfo.clear();
	m_dwLoveModeObjectDestroyWaitTime = 0;
	m_bCheckLoveModeObjectDestroyWaitTime = false;

	return eRet;
}

void PgWarGround::Clear()
{
	PgIndun::Clear();
}

bool PgWarGround::Clone( PgGround* pkGround )
{
	return this->Clone( (dynamic_cast<PgWarGround*>(pkGround)) );
}

bool PgWarGround::Clone( PgWarGround* pkIndun )
{
	return PgIndun::Clone( dynamic_cast<PgIndun*>(pkIndun) );
}

bool PgWarGround::IsDeleteTime()const
{
	BM::CAutoMutex kLock(m_kRscMutex);
	switch ( GetState() )
	{
	case INDUN_STATE_OPEN:
	case INDUN_STATE_WAIT:
		{
		}break;
	case INDUN_STATE_READY:
	case INDUN_STATE_PLAY:
		{
			if ( true == IsModeType( WAR_TYPE_ALL ) )
			{
				// 엠포리아 쟁탈전은 무조건 삭제되면 안된다!!
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}// No Break
	default:
		{
			return ( 0 == PgObjectMgr::GetUnitCount(UT_PLAYER) );
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgWarGround::Cancel()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( m_eState == INDUN_STATE_WAIT )
	{
		SetState( INDUN_STATE_CLOSE );
	}
	else
	{
		INFO_LOG( BM::LOG_LV0, __FL__ << _T("Error State<") << (DWORD)m_eState << _T("> GroundNo<") << GroundKey().GroundNo() << _T("/") << GroundKey().Guid() << _T(">") );
	}
}

HRESULT PgWarGround::Ready( BM::Stream& kPacket )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	EPVPTYPE kType = PVP_TYPE_NONE;
	kPacket.Pop(kType);

//	size_t const iRdPos = kPacket.RdPos();

	if( m_pkMode )
	{
		if ( kType != m_pkMode->Type() )
		{
			delete m_pkMode;
			m_pkMode = NULL;
		}
	}

	if( !m_pkMode )
	{
		switch( kType )
		{
		case PVP_TYPE_DM:
			{
				m_pkMode = new_tr PgPvPType_DeathMatch;
			}break;
		case PVP_TYPE_ANNIHILATION:
			{
				m_pkMode = new_tr PgPvPType_Annihilation;
			}break;
		case PVP_TYPE_DESTROY:
			{
				m_pkMode = new_tr PgPvPType_Destroy;
			}break;
		case PVP_TYPE_WINNERS:
			{
				m_pkMode = new_tr PgPvPType_WinnersMatch;
			}break;
		case PVP_TYPE_WINNERS_TEAM:
			{
				m_pkMode = new_tr PgPvPType_WinnersTeam;
			}break;
		case PVP_TYPE_ACE:
			{
				m_pkMode = new_tr PgPvPType_AceMatch;
			}break;
		case WAR_TYPE_DESTROYCORE:
			{
				m_pkMode = new_tr PgWarType_DestroyCore;
			}break;
		case WAR_TYPE_PROTECTDRAGON:
			{
				m_pkMode = new_tr PgWarType_ProtectDragon;
				
				// 드래곤 방어전에서는 돌발미션을 사용하지 않는다.
				m_pkAlramMissionMgr = NULL;
			}break;
		case PVP_TYPE_KTH:
			{
				m_pkMode = new_tr PgPvPMode_KingOfHill;
			}break;
		case PVP_TYPE_LOVE:
			{
				m_pkMode = new_tr PgPvPType_Love;
			}break;
		default:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Unknown Type<") << kType << _T(">") );
			}break;
		}
	}

	if ( m_pkMode )
	{
		if ( SUCCEEDED(m_pkMode->Init( kPacket, this )) )
		{
			m_bUseItem = m_pkMode->IsUseItem();
			SetState(INDUN_STATE_WAIT);

			// 현재 있는 유저는 Join으로 처리
			SReqMapMove_MT kRMM(MMET_KickToPvP);
			kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );

			PgReqMapMove kOutMapMove( this, kRMM, NULL );

			CUnit* pkUnit = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_itr;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
			while(( pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
			{	
				PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
				if ( FAILED(m_pkMode->Join( pkPlayer, false ) ) )
				{
					// GM일수도 있다.
					BYTE const byGMLevel = pkPlayer->GMLevel();
					if ( byGMLevel )
					{
						CONT_DEFGMCMD const *pkGmCmd = NULL;
						g_kTblDataMgr.GetContDef(pkGmCmd);
						if ( pkGmCmd )
						{
							PgGMLevelChecker kGMLevelChecker( *pkGmCmd );
							if ( true == kGMLevelChecker.IsAccess( GMCMD_MAPMOVE_TARGET, byGMLevel ) )
							{
								continue;
							}
						}
					}
					// 여기 있는 놈은 쫒아내야 한다.
					// Lobby로 돌려보내야 한다. OK?
					// 근데 로직상 여기에 누군가 있다면 문제가 있는건데.
					kOutMapMove.Add( pkPlayer );
					VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"Why This Ground["<<GetGroundNo()<<L"-"<<GroundKey().Guid()<<L"] Remain["<<pkUnit->Name()<<L"-"<<pkUnit->GetID()<<L"]");
				}
			}

			VEC_GUID kUnitList;
			if ( m_pkMode->GetReadyUnitList( kUnitList ) )
			{
				SReqMapMove_CM kRMMC( MMET_Normal );
				kRMMC.kGndKey = GroundKey();
				kRMMC.nPortalNo = 1;

				ReqMapMoveCome( kRMMC, kUnitList );
			}

			// Init하고 나서 해야 할 일이 있다.
			switch( kType )
			{
			case PVP_TYPE_DM:
			case PVP_TYPE_ANNIHILATION:
			case PVP_TYPE_DESTROY:
				{
					kOutMapMove.DoAction();
				}break;
			case WAR_TYPE_DESTROYCORE:
			case WAR_TYPE_PROTECTDRAGON:
				{
				}break;
			}
			return true;
		}
	}
	else
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Mode["<<kType<<L"] Memory Error!!" );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
	}

	INFO_LOG(BM::LOG_LV2, __FL__<<L"Init Failed Mode["<<kType<<L"] ["<<GetGroundNo()<<L"]["<<GroundKey().Guid()<<L"]");
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgWarGround::OnTick1s()
{
	BM::CAutoMutex Lock( m_kRscMutex );

	DWORD const dwNow = BM::GetTime32();
	DWORD dwElapsedTime = 0;
	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsedTime, true);

	switch( m_eState )
	{
	case INDUN_STATE_OPEN:
		{	
		}break;
	case INDUN_STATE_WAIT:
		{
			if ( m_pkMode && m_pkMode->IsStart( dwNow ) )
			{
				SetState( INDUN_STATE_READY );
			}
		}break;
	case INDUN_STATE_READY:
		{
		}break;
	case INDUN_STATE_PLAY:
		{		
			// Player AutoHealing.......
			float const fAutoHealMultiplier = GetAutoHealMultiplier();
			bool const bSpeedHackCountClear = BM::TimeCheck(m_dwOnTick1s_Check2s, 2000);	// 2s 마다 한번씩

			DWORD dwDeathTime = 0;
			UNIT_PTR_ARRAY kDeleteUnit;
			PgPlayer* pkPlayer = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
			{
				pkPlayer->AutoHeal( dwElapsedTime, fAutoHealMultiplier );
				if(bSpeedHackCountClear)
				{
					pkPlayer->SetAbil(AT_SPEED_HACK_COUNT, 0);
				}
				if( pkPlayer->IsDead() )
				{
					if ( m_pkMode && ( true == m_pkMode->IsRevive() ) )
					{
						dwDeathTime = pkPlayer->DeathTime();
						if (	dwDeathTime
							&&	(::DifftimeGetTime( dwDeathTime, dwNow ) >= m_pkMode->GetTime(PVP_TIME_REVIVE))	)
						{
							pkPlayer->Alive( EALIVE_PVP, E_SENDTYPE_BROADALL );

							PgDeathPenalty kAction(GroundKey(), LURT_WarGround, this->IsDeathPenalty() );//공성전에서 죽으면. 패널티를 준다.
							kAction.DoAction(pkPlayer, NULL);

							int iTeam = pkPlayer->GetAbil(AT_TEAM);
							int const iPortalNo = GetRandomSpawn(iTeam);
							SendToSpawnLoc( pkPlayer, iPortalNo, true, iTeam );
						}
					}
				}
			}

			// Entity Tick
			/*PgEntity *pkEntity = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor_Entity;
			PgObjectMgr::GetFirstUnit(UT_ENTITY, kItor_Entity);
			while ((pkEntity = dynamic_cast<PgEntity*> (PgObjectMgr::GetNextUnit(UT_ENTITY, kItor_Entity))) != NULL)
			{
				pkEntity->AutoHeal( dwElapsedTime );

				if ( pkEntity->GetState() == US_DEAD )
				{
					kDeleteUnit.Add( pkEntity );
				}
			}*/

			// Pet Tick
			PgPet *pkPet = NULL;
			CONT_OBJECT_MGR_UNIT::iterator itr_pet;
			PgObjectMgr::GetFirstUnit(UT_PET, itr_pet);
			while ((pkPet = dynamic_cast<PgPet*> (PgObjectMgr::GetNextUnit(UT_PET, itr_pet))) != NULL)
			{
				pkPet->AutoHeal( dwElapsedTime );
			}

			// ItemBox Deleting......
			PgGroundItemBox* pkBox = NULL;
			PgObjectMgr::GetFirstUnit(UT_GROUNDBOX, kItor);
			while( (pkBox = dynamic_cast<PgGroundItemBox*>(PgObjectMgr::GetNextUnit(UT_GROUNDBOX, kItor))) != NULL)
			{
				if( !pkBox->IsInstanceItem() )
				{
					if (dwNow - pkBox->CreateDate() > PgGroundItemBox::ms_GROUNDITEMBOX_DURATION_TIME)
					{
						kDeleteUnit.Add(pkBox);				
					}
				}
			}

			ObjectUnitTick( dwElapsedTime, kDeleteUnit );

			UNIT_PTR_ARRAY::const_iterator itor = kDeleteUnit.begin();
			while (itor != kDeleteUnit.end())
			{
				ReleaseUnit(itor->pkUnit);
				++itor;
			}

			if ( FAILED( m_pkMode->Update( dwNow ) ) )
			{
				SetState( INDUN_STATE_RESULT_WAIT );
			}

			if( IsModeType(PVP_TYPE_LOVE) )
			{
				CheckGenBear();
				CheckDropBearDeleteTime();
			}

			if( IsModeType(PVP_TYPE_KTH) )
			{
				static DWORD SendGamePointTime = 0;
				if( BM::TimeCheck(SendGamePointTime, 10000) )
				{
					PgPvPMode_KingOfHill * pKTH = dynamic_cast<PgPvPMode_KingOfHill *>(m_pkMode);
					if( pKTH )
					{
						pKTH->SendGamePoint();
					}
				}
			}
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
			if( IsModeType(PVP_TYPE_LOVE) )
			{
				if( IsModeType(PVP_TYPE_LOVE) )
				{
					if( 0 == m_dwLoveModeObjectDestroyWaitTime )
					{
						break;
					}

					if( dwElapsedTime >= m_dwLoveModeObjectDestroyWaitTime )
					{
						RemoveAllObject();
						ActivateObjectUnitGenGroup(0, true, false);
						ResetBreakObject();

						m_bCheckLoveModeObjectDestroyWaitTime = false;
					}
					else
					{
						m_dwLoveModeObjectDestroyWaitTime -= dwElapsedTime;					
					}
				}
			}
		}break;
	case INDUN_STATE_RESULT:
		{
			if( PVP_TYPE_ACE == m_pkMode->Type() )
			{//에이스 매치에서만 사용
				DWORD const dwElapsedTime = m_pkMode->GetTime(PVP_TIME_ENTRYSELECT);
				if( m_pkMode->CheckEntryWait(dwElapsedTime) )
				{//시간 초과 되었거나, 엔트리가 정해졌음, 마지막 라운드 시작
					SetState( INDUN_STATE_READY );
				}
			}
		}break;
	case INDUN_STATE_CLOSE:
		{
			if ( !IsModeType(PVP_TYPE_ALL) )
			{
				// 한명씩 쫒아 낸다.
				PgPlayer* pkPlayer = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
				{
					if ( this->RecvRecentMapMove(pkPlayer) )
					{
						break;
					}
				}
			}
		}break;
	}
	UpdateAutoNextState(dwNow, dwElapsedTime);
}

void PgWarGround::SetState( EIndunState const eState, bool bAutoChange, bool bChangeOnlyState)
{
	BM::CAutoMutex Lock( m_kRscMutex );
	if ( eState == m_eState)
	{
		return;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = eState;

	PgWorldEventMgr::DoIndunState(GroundKey().GroundNo(), m_eState, NULL);

	switch( m_eState )
	{
	case INDUN_STATE_OPEN:
		{
			SetAutoNextState(md_dwMaxOpenWaitingTime);//중요
		}break;
	case INDUN_STATE_WAIT:
		{
			if ( true == IsModeType( WAR_TYPE_ALL ) )
			{
				SetAutoNextState(3600000);// 1시간을 기다려?
			}
			else
			{
				SetAutoNextState(ms_dwMaxWaitUserWaitngTime);
			}

			RemoveGenGroupInsItem(EIGG_ANY);
			RemoveGenGroupInsItem(EIGG_RED);
			RemoveGenGroupInsItem(EIGG_BULE);

			if( true == IsModeType( PVP_TYPE_LOVE ) )
			{
				RemoveGenGroupObject(0);
				ActivateObjectUnitGenGroup(0, true, false);
				if( m_pkMode )
				{	// 곰 아이템 번호 저장.
					PgPvPType_Love * pLove = NULL;
					pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
					if( pLove )
					{
						int itemNo = 0;
						
						itemNo = m_pkMode->PgWarMode::ms_kSetting.Get(m_pkMode->Type(), PVP_ST_LOVE_MODE_DROP_RED_BEAR);
						pLove->RedBearNo(itemNo);

						itemNo = m_pkMode->PgWarMode::ms_kSetting.Get(m_pkMode->Type(), PVP_ST_LOVE_MODE_DROP_BLUE_BEAR);
						pLove->BlueBearNo(itemNo);

						CONT_GTRIGGER::const_iterator iter = m_kContTrigger.begin();
						for( ; iter != m_kContTrigger.end() ; ++iter )
						{
							CONT_GTRIGGER::mapped_type const & Element = (*iter).second;

							if( GTRIGGER_TYPE_SCORE == Element->GetType() )
							{
								PgGTrigger_Score * pScore = dynamic_cast<PgGTrigger_Score *>(Element);
								if( pScore )
								{
									if( pScore->GetTriggerTeamNo() == TEAM_RED )
									{
										pLove->SetRedScoreTrigger(pScore);
									}
									else if( pScore->GetTriggerTeamNo() == TEAM_BLUE )
									{
										pLove->SetBlueScoreTrigger(pScore);
									}
								}
							}
						}
					}
				}
			}
		}break;
	case INDUN_STATE_READY:
		{
			if ( m_pkMode )
			{
				SetAutoNextState( m_pkMode->GetTime(PVP_TIME_READY) );

				m_pkMode->ReadyGame( this );				

				BM::Stream kPacket( PT_M_C_NFY_GAME_READY, m_pkMode->Type() );
				kPacket.Push( m_dwAutoStateRemainTime );
				Broadcast( kPacket );
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"INDUN_STATE_READY : Mode Is Null!!!");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
			}
		}break;
	case INDUN_STATE_PLAY:
		{
			if ( m_pkMode )
			{
				UNIT_PTR_ARRAY kUnitArray;

				if ( true == m_pkMode->IsUseMonster() )
				{
					ActivateMonsterGenGroup( 1, true, true, 0 );
					GetHaveAbilUnitArray( kUnitArray, AT_TEAM, UT_MONSTER );
				}

				if ( true == m_pkMode->IsUseObjectUnit() )
				{
					ActivateObjectUnitGenGroup( 1, false, true );
//					ObjectUnitGenerate( false, 1 );
					GetHaveAbilUnitArray( kUnitArray, AT_TEAM, UT_OBJECT );
				}

				m_pkMode->InitUnitInfo( kUnitArray, this );

				CUnit *pkUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if ( pkPlayer ) 
					{
						pkUnit->CUnit::SetState(US_IDLE);//ACTION_LOCK 때문에. 강제로 풀어줌.
						switch( m_pkMode->Type() )
						{
						case PVP_TYPE_WINNERS:
						case PVP_TYPE_WINNERS_TEAM:
						case PVP_TYPE_ACE:
							{//승자전이라면 구경할놈은 셋팅해줌
								BM::GUID kTargetGuid;
								if( SUCCEEDED(m_pkMode->IsObModePlayer(pkPlayer, kTargetGuid)) )
								{
									CUnit* pkTarget = PgObjectMgr::GetUnit(UT_PLAYER, kTargetGuid);
									if( pkTarget )
									{//옵저버 ~
										SetOBMode_Start(pkPlayer, pkTarget);
									}
								}
							}break;
						default: { }break;
						}

						// 스타트~
						m_pkMode->Start( pkPlayer, this );

						// 삭제 예약된 인스턴스 아이템이 있다면 삭제.
						RemoveReserveDeleteInsItem(pkPlayer->GetID());
					}
				}
				
				// 남아 있는 인스턴스 아이템을 전부 삭제하고.
				CUnit * pUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator Itor;
				PgObjectMgr::GetFirstUnit(UT_GROUNDBOX, Itor);
				while( (pUnit = PgObjectMgr::GetNextUnit(UT_GROUNDBOX, Itor)) != NULL )
				{
					BM::Stream Packet( PT_M_C_ANS_PICKUPGBOX, BM::GUID::NullData() );
					Packet.Push( pUnit->GetID() );
					Broadcast(Packet);

					this->ReleaseUnit(pUnit, false, false);
				}

				// 다시 리젠.
				ActivateInstanceItemGenGroup(EIGG_ANY, true, false, 0);
				ActivateInstanceItemGenGroup(EIGG_RED, true, false, 0);
				ActivateInstanceItemGenGroup(EIGG_BULE, true, false, 0);

				if( IsModeType(PVP_TYPE_LOVE) )
				{
					PgPvPType_Love * pLove = NULL;
					pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
					if( pLove )
					{
						pLove->OnBlueBear(true);
						pLove->OnRedBear(true);						
					}
				}

				DWORD const dwRemainTime = m_pkMode->GameTime();

				// 유저에게 시작을 알리는 패킷
				BM::Stream kPacket( PT_M_C_NFY_GAME_START, m_pkMode->Type() );
				kPacket.Push( dwRemainTime );

				m_pkMode->StartGame( kPacket, true );	

				Broadcast( kPacket );// 유저에게 전송
				SetAutoNextState( dwRemainTime );

				EPVPTYPE const kType = m_pkMode->Type();
				if ( kType & PVP_TYPE_ALL )
				{
					PgPvPType_DeathMatch *pkPvPType = dynamic_cast<PgPvPType_DeathMatch*>(m_pkMode);
					if ( pkPvPType )
					{
						__int64 const i64RemainGameTime = static_cast<__int64>(pkPvPType->GameTime()) / 1000i64;

						// 로그를 남겨
						PgLogCont kLogCont(ELogMain_Contents_PVP, ELogSub_PVP_Game);
						kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
						kLogCont.GroundNo( GetGroundNo() );
						kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

						PgLog kLog( ELOrderMain_Round, ELOrderSub_Start );
						kLog.Set( 0, pkPvPType->GetRoomIndex() + 1 );				// iValue1 방번호
						kLog.Set( 1, static_cast<int>(pkPvPType->GetNowRound()) );	// iValue2 라운드 번호
						kLog.Set( 2, static_cast<int>(pkPvPType->GetMode()) );		// iValue3 게임 모드
						kLog.Set( 3, static_cast<int>(kType) );						// iValue4 게임 타입
						kLog.Set( 0, static_cast<__int64>(PgObjectMgr::GetUnitCount(UT_PLAYER)) );			// i64Value1 플레이어숫자
						kLog.Set( 1, i64RemainGameTime );							// i64Value2 게임시간

						kLogCont.Add( kLog );
						kLogCont.Commit();
					}
					else
					{
						//VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!!") );
						//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("pkPvPType is NULL"));
					}
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"INDUN_STATE_PLAY : Mode Is Null!!!");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
			}
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
			if ( m_pkMode )
			{	
				EPVPTYPE const kType = m_pkMode->Type();
				if ( kType & PVP_TYPE_ALL )
				{
					PgPvPType_DeathMatch *pkPvPType = dynamic_cast<PgPvPType_DeathMatch*>(m_pkMode);
					if ( pkPvPType )
					{
						__int64 const i64RemainGameTime = static_cast<__int64>(m_dwAutoStateRemainTime) / 1000i64;

						// 로그를 남겨
						PgLogCont kLogCont(ELogMain_Contents_PVP, ELogSub_PVP_Game);
						kLogCont.MemberKey( GroundKey().Guid() );// GroundGuid
						kLogCont.GroundNo( GetGroundNo() );
						kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

						PgLog kLog( ELOrderMain_Round, ELOrderSub_End );
						kLog.Set( 0, pkPvPType->GetRoomIndex() + 1 );				// iValue1 방번호
						kLog.Set( 1, static_cast<int>(pkPvPType->GetNowRound()) );	// iValue2 라운드 번호
						kLog.Set( 2, static_cast<int>(pkPvPType->GetMode()) );		// iValue3 게임 모드
						kLog.Set( 3, static_cast<int>(kType) );						// iValue4 게임 타입
						kLog.Set( 0, static_cast<__int64>(PgObjectMgr::GetUnitCount(UT_PLAYER)) );			// i64Value1 플레이어숫자
						kLog.Set( 1, i64RemainGameTime );							// i64Value2 게임시간

						kLogCont.Add( kLog );
						kLogCont.Commit();
					}
					else
					{
						//VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!!!") );
						//LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkPvPType is NULL"));
					}
				}
				UNIT_PTR_ARRAY kUnitArray;
				if ( true == m_pkMode->IsUseObjectUnit() )
				{
					ActivateObjectUnitGenGroup( 1, false, true );
//					ObjectUnitGenerate( false, 1 );
					GetHaveAbilUnitArray( kUnitArray, AT_TEAM, UT_OBJECT );
				}
				m_pkMode->InitUnitInfo( kUnitArray, this );

				CUnit *pkUnit = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUnit);
					if ( pkPlayer ) 
					{
						pkPlayer->ClearAllEffect();
					}
				}
				m_pkMode->End( bAutoChange, this );
				if( IsModeType( PVP_TYPE_LOVE ) )
				{	// 러브러브 모드는 연출씬이 있어서 연출씬 만큼 기다려야함.
					SetAutoNextState( m_pkMode->GetTime(PVP_TIME_EVENT_SCENE_RESULT) );
					m_dwLoveModeObjectDestroyWaitTime = m_pkMode->GetTime(PVP_TIME_EVENT_SCENE_RESULT) / 2;
					m_bCheckLoveModeObjectDestroyWaitTime = true;
				}
				else
				{
					SetAutoNextState( m_pkMode->GetTime(PVP_TIME_ROUNDTERM) );
				}
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"INDUN_STATE_RESULT_WAIT : Mode Is Null!!!");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
			}
		}break;
	case INDUN_STATE_RESULT:
		{
			if( !IsModeType(PVP_TYPE_LOVE) )
			{	// 러브러브 모드에서는 제거 안함(기획팀 요청).
				RemoveAllMonster();
				RemoveAllObject();
			}

			// 결과도 종료 되었음.
			{
				UNIT_PTR_ARRAY kDelEntityArray;

				PgPlayer *pkPlayer = NULL;
				CONT_OBJECT_MGR_UNIT::iterator unit_itr;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
				while( ( pkPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) ) != NULL)
				{	
					switch ( pkPlayer->GetState() )
					{
					case US_DEAD:
					case US_GHOST:
						{// 죽은 놈은 다  살려준다.
							switch( m_pkMode->Type() )
							{
							case PVP_TYPE_WINNERS:
							case PVP_TYPE_WINNERS_TEAM:
							case PVP_TYPE_ACE:
								{//부활 안함.
								}break;
							default:
								{
									pkPlayer->Alive( EALIVE_PVP, E_SENDTYPE_BROADALL );
								}break;
							}

							pkPlayer->SetAbil(AT_SUCCESSIONAL_DEATH_COUNT, 0);

							PgDeathPenalty kAction(GroundKey(), LURT_WarGround, this->IsDeathPenalty() );//단, 패널티가 있다.
							kAction.DoAction(pkPlayer, NULL);
						}break;
					}

					// Action을 못하게 한다.
					pkPlayer->SetState( US_ACTION_LOCK );

					// 소환물은 다 삭제해야 한다.
					GetSummonUnitArray( pkPlayer, kDelEntityArray );
				}

				CONT_OBSERVER::const_iterator ob_itr = m_kContObserver.begin();
				for ( ; ob_itr != m_kContObserver.end() ; ++ob_itr )
				{
					if ( S_OK == PgObjectMgr2::UnRegistObserver( ob_itr->second ) )
					{
// 						int iAreaIndex = PgSmallArea::NONE_AREA_INDEX;
// 						CUnit *pkOwnerUnit = PgObjectMgr::GetUnit( ob_itr->second.GetOwnerID() );
// 						if ( pkOwnerUnit )
// 						{
// 							iAreaIndex = pkOwnerUnit->LastAreaIndex();
// 						}

						PgPlayer *pkMyPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetUnit( UT_PLAYER, ob_itr->second.GetCharID() ));
						if ( pkMyPlayer )
						{
							BM::Stream kEndPacket( PT_M_C_NFY_OBMODE_END );
							pkMyPlayer->Send( kEndPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);

							// 원래 Area정보로 복구 시켜 주어야 한다~
							pkMyPlayer->SetSyncType( SYNC_TYPE_DEFAULT, false );

// 							if ( pkMyPlayer->LastAreaIndex() != iAreaIndex )
// 							{
// 								PgSmallArea *pkCasterArea = GetArea( pkMyPlayer->LastAreaIndex() );
// 								PgSmallArea *pkTargetArea = GetArea( iAreaIndex );
// 								SendAreaData( pkMyPlayer, pkCasterArea, pkTargetArea, SYNC_TYPE_RECV_ADD );// AddUnit 받기만 해야 한다.
// 							}
						}
					}
				}
				m_kContObserver.clear();
				
				UNIT_PTR_ARRAY::iterator del_itr = kDelEntityArray.begin();
				while ( del_itr!=kDelEntityArray.end() )
				{
					if(del_itr->pkUnit)
					{
						del_itr->pkUnit->SetAbil(AT_HP,0);
					}
					++del_itr;
				}
				PgGround::ReleaseUnit( kDelEntityArray, true );
			}

			if ( m_pkMode )
			{
				EResultEndType const eNextStatus = m_pkMode->ResultEnd();
				switch( eNextStatus )
				{
				case RESULT_END_RESTART:
					{
						SetState( INDUN_STATE_READY );
					}break;
				case RESULT_END_CLOSE:
					{
						// 10초후에 닫게 만듬
						SetAutoNextState( m_pkMode->GetTime(PVP_TIME_RESULT) );
					}break;
				case RESULT_END_WAIT_ENTRY:
					{//엔트리 재선택해라. 
						if( PVP_TYPE_ACE == m_pkMode->Type() )
						{//에이스 매치에서만 됨.
							BM::Stream kPacket(PT_M_C_NFY_SELECT_ENTRY);
							Broadcast( kPacket );
						}
					}break;
				default:
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
						// 아무것도 안함
					}break;
				}

				PgPvPType_Love * pLove = NULL;
				pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
				if( pLove )
				{	// 카메라 되돌림.					
					BM::Stream CameraPacket(PT_M_C_NFY_FOCUS_TOUCH_DOWN_USER, false);
					Broadcast( CameraPacket );
				}
			}
			else
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV1, "INDUN_STATE_RESULT : Mode Is Null!!!");
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
			}
		}break;
	case INDUN_STATE_CLOSE:
		{
			if ( true == IsModeType(PVP_TYPE_ALL) )
			{
				BM::Stream kPacket( PT_M_T_NFY_GAME_RESULT_END, m_pkMode->NoEnemy() );
				m_pkMode->Send( kPacket );
			}
		}break;
	default:
		{

		}break;
	}
}

void PgWarGround::SendMapLoadComplete( PgPlayer *pkUser )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	PgGround::SendMapLoadComplete( pkUser );

	switch( m_eState )
	{
	case INDUN_STATE_WAIT:
		{
			if ( m_pkMode )
			{
				if ( FAILED(m_pkMode->Join( pkUser, false )) )
				{
					SReqMapMove_MT kRMM(MMET_BackToChannel);

					if ( IsModeType( PVP_TYPE_ALL) )
					{
						kRMM.cType = MMET_KickToPvP;
						kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );
					}
					
					PgReqMapMove kMapMove( this, kRMM, NULL );
					kMapMove.Add( pkUser );
					kMapMove.DoAction();
				}
			}
		}break;
	case INDUN_STATE_READY:
		{
			if ( m_pkMode )
			{
				if ( FAILED(m_pkMode->Join( pkUser, false )) )
				{
					SReqMapMove_MT kRMM(MMET_BackToChannel);

					if ( IsModeType( PVP_TYPE_ALL) )
					{
						kRMM.cType = MMET_KickToPvP;
						kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );
					}

					PgReqMapMove kMapMove( this, kRMM, NULL );
					kMapMove.Add( pkUser );
					kMapMove.DoAction();
				}
				else
				{
					BM::Stream kPacket( PT_M_C_NFY_GAME_READY, m_pkMode->Type() );
					kPacket.Push( m_dwAutoStateRemainTime );
					pkUser->Send( kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
				}
			}
		}break;
	case INDUN_STATE_PLAY:
		{
			if ( m_pkMode )
			{
				if ( FAILED(m_pkMode->Join( pkUser, true )) )
				{
					SReqMapMove_MT kRMM(MMET_BackToChannel);

					if ( IsModeType( PVP_TYPE_ALL) )
					{
						kRMM.cType = MMET_KickToPvP;
						kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );
					}

					PgReqMapMove kMapMove( this, kRMM, NULL );
					kMapMove.Add( pkUser );
					kMapMove.DoAction();
				}
				else
				{
					m_pkMode->Start( pkUser, this );

					BM::Stream kPacket( PT_M_C_NFY_GAME_START, m_pkMode->Type() );
					kPacket.Push( m_dwAutoStateRemainTime );
					m_pkMode->StartGame( kPacket, false );
					pkUser->Send( kPacket, E_SENDTYPE_SELF|E_SENDTYPE_MUSTSEND );
				}
			}	
		}break;
	case INDUN_STATE_RESULT_WAIT:
	case INDUN_STATE_RESULT:
		{
		}break;
	default:
		{
			// xxxxxx 다시 돌려 보내야 한다.
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
		}break;
	}
}

bool PgWarGround::IsAccess( PgPlayer *pkPlayer )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	
	if ( m_pkMode )
	{
		return m_pkMode->IsJoin( pkPlayer );
	}

	BYTE const byGMLevel = pkPlayer->GMLevel();
	if ( byGMLevel )
	{// GM Level이 있으면
		CONT_DEFGMCMD const *pkGmCmd = NULL;
		g_kTblDataMgr.GetContDef(pkGmCmd);
		if ( pkGmCmd )
		{
			PgGMLevelChecker kGMLevelChecker( *pkGmCmd );
			if ( true == kGMLevelChecker.IsAccess( GMCMD_MAPMOVE_TARGET, byGMLevel ) )
			{
				return true;
			}
		}
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgWarGround::SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont )
{
	int const iGrade = pkDroper->GetAbil(AT_GRADE);
	switch( iGrade )
	{
	case EMGRADE_BOSS:
		{
			// 보스가 죽으면 게임 종료
			SetState(INDUN_STATE_RESULT_WAIT);
		}break;
	default:
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Invalid CaseType!"));
		}break;
	}
	return S_OK;
}

bool PgWarGround::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		if ( m_pkMode )
		{
			m_pkMode->Exit( pkUnit->GetID() );
		}
	}
	else if(pkUnit->IsUnitType(UT_ENTITY) && ENTITY_GUARDIAN==pkUnit->GetAbil(AT_ENTITY_TYPE))
	{
		if ( m_pkMode )
		{
			m_pkMode->Exit( pkUnit->GetID() );
		}
	}
	return PgGround::ReleaseUnit( pkUnit, bRecursiveCall, bSendArea );
}

void PgWarGround::RecvMapMoveComeFailed( BM::GUID const &kCharGuid )
{
	if ( m_pkMode )
	{
		m_pkMode->Exit( kCharGuid );
	}
}

bool PgWarGround::SaveUnit( CUnit *pkUnit, SReqSwitchReserveMember const *pRSRM, BM::Stream * const pPacket )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	// WarGround에서는 Save를 하지 않는다.
	// 하지만 PvP Game중에 나간거라면 패배로 기록해야 한다.
	if ( INDUN_STATE_PLAY == GetState() )
	{
		if ( true == IsModeType( PVP_TYPE_ALL ) )
		{
			if( UT_PLAYER == pkUnit->UnitType() )
			{
				CONT_PLAYER_MODIFY_ORDER kOrder;

				// 패배로 기록
				SPMOD_SAbil kAddRecord( (m_pkMode->IsExercise() ? GAME_LOSE_EXERCISE : GAME_LOSE), 1 );
				kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, pkUnit->GetID(), kAddRecord));

				PgAction_ReqModifyItem kItemModifyAction( IMEPT_PVP, GroundKey(), kOrder, BM::Stream(), true );//랭킹 기록은 사망여부 상관없이.
				kItemModifyAction.DoAction( pkUnit, NULL );
			}
		}

		if( true == IsModeType( PVP_TYPE_LOVE ) )
		{	// 러브러브 모드에서 캐릭터가 접속이 끊어지게 될 경우, 해당 캐릭터가 곰 아이템을 가지고 있다면 곰을 생성시켜야 한다.
			PgPvPType_Love * pLove = NULL;
			pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
			if( pLove )
			{
				PgUnitEffectMgr const & EffectMgr = pkUnit->GetEffectMgr();
				if( EffectMgr.GetAbil(AT_BEAR_EFFECT_RED) || EffectMgr.GetAbil(AT_BEAR_EFFECT_BLUE) )
				{
					pLove->CheckPlayerHaveBear(pkUnit);
				}
			}
		}
	}

	return PgGround::SaveUnit( pkUnit, pRSRM, pPacket);;
}

bool PgWarGround::AdjustArea( CUnit *pkUnit, bool const bIsSendAreaData, bool const bIsCheckPos )// Area 에 유닛 넣어주는것 까지 다함.
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if ( true == PgGround::AdjustArea( pkUnit, bIsSendAreaData, bIsCheckPos ) )
	{
		if ( true == bIsCheckPos )
		{
			if ( INDUN_STATE_PLAY == m_eState )
			{
				if ( m_pkMode )
				{
					m_pkMode->SetEvent_AdjustArea( pkUnit );
				}
			}
		}

		return true;
	}
	return false;
}

void PgWarGround::CallAlramReward( PgPlayer * pkPlayer )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	PgAlramMission &rkAlramMission = pkPlayer->GetAlramMission();

	TBL_DEF_ALRAM_MISSION const * const pkDef = rkAlramMission.GetDef();
	if ( pkDef )
	{
		if ( pkDef->nPoint )
		{
			if ( m_pkMode )
			{
				m_pkMode->SetEvent_BonusPoint( pkPlayer, pkDef->nPoint, this );
			}
		}

		PgGround::CallAlramReward( pkPlayer );
	}
}

void PgWarGround::GMCommand_RecvGamePoint( PgPlayer *pkPlayer, int const iPoint )
{
	if ( INDUN_STATE_PLAY == GetState() )
	{
		if ( m_pkMode )
		{
			m_pkMode->SetEvent_BonusPoint( pkPlayer, static_cast<short>(iPoint), this );
		}
	}
}

int PgWarGround::Get_KOH_OccupyPoint_NotIdleRate()const
{
	if(m_pkMode)
	{
		return m_pkMode->PgWarMode::ms_kSetting.Get( m_pkMode->Type(), PVP_ST_OCCUPY_POINT_NOT_IDLE_RATE );
	}
	return ABILITY_RATE_VALUE64;
}

PgGTrigger_KingOfHill const* PgWarGround::Get_KOH_Trigger(BM::GUID const& rkUnitGuid)const
{
	if ( m_pkMode && PVP_TYPE_KTH==m_pkMode->Type())
	{
		if( PgPvPMode_KingOfHill * pkMode = dynamic_cast<PgPvPMode_KingOfHill*>(m_pkMode) )
		{
			return pkMode->GetTrigger( rkUnitGuid );
		}
	}
	return NULL;
}

void PgWarGround::KOH_SetTriggerUnitPoint(std::string const& rkTriggerID, int const iPoint)
{
	if ( m_pkMode && PVP_TYPE_KTH==m_pkMode->Type())
	{
		if( PgPvPMode_KingOfHill * pkMode = dynamic_cast<PgPvPMode_KingOfHill*>(m_pkMode) )
		{
			pkMode->SetTriggerUnitPoint( rkTriggerID, iPoint );
		}
	}
}

void PgWarGround::RemoveEnemeyInstanceItem(CUnit* pUnit)
{
	m_pkMode->SetEvent_GetInstanceItem(pUnit, this);
}

void PgWarGround::SendNfyMessage(CUnit * pUnit, int MessageNo, bool bPlaySound, bool bBroadCast)
{
	PgPlayer * pPlayer = dynamic_cast<PgPlayer*>(pUnit);
	if( !pPlayer)
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << " pUnit is NULL");
		return;
	}

	BM::Stream Packet(PT_M_C_NFY_KTH_MESSAGE);
	Packet.Push(MessageNo);
	Packet.Push(bPlaySound);
	Packet.Push(pPlayer->Name());

	if( bBroadCast )
	{
		Broadcast(Packet);
	}
	else
	{
		pPlayer->Send(Packet);
	}
}

int PgWarGround::GetKOH_EntityLevelExp(EPvPSetting Setting)
{
	return m_pkMode->PgWarMode::ms_kSetting.Get(m_pkMode->Type(), Setting);
}

int PgWarGround::GetKOH_CaptureDecValue(EPvPSetting Setting)
{
	return m_pkMode->PgWarMode::ms_kSetting.Get(m_pkMode->Type(), Setting);
}

void PgWarGround::KOH_CpatureStrongHold(CUnit * pUnit, float const Point)
{
	m_pkMode->SetEvent_CapturePoint(pUnit, Point, this);
}

bool PgWarGround::IsMonsterTickOK()const
{
	if(m_pkMode && PVP_TYPE_KTH==m_pkMode->Type())
	{
		return m_eState & (INDUN_STATE_READY|INDUN_STATE_PLAY);
	}
	return PgObjectMgr::GetUnitCount(UT_PLAYER) > 0;
}

void PgWarGround::EntityTick(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier)
{
	if( true==IsMonsterTickOK() )
	{
		TickAI(pkUnit, dwkElapsed);
		AutoHealAndCheckDieSkill(pkUnit, dwkElapsed, fAutoHealMultiplier);

		if( m_eState & INDUN_STATE_PLAY )
		{
			EffectTick(pkUnit, dwkElapsed);
		}
	}
}

void PgWarGround::RoundWin(int WinTeam)
{
	m_pkMode->SetRoundWinTeam(WinTeam, this);
	SetAutoNextState( m_pkMode->GetTime(PVP_TIME_ROUNDTERM) );
}

// 러브러브 모드.
void PgWarGround::BearTouchDown(CUnit * pUnit)
{
	if( !m_pkMode || PVP_TYPE_LOVE != m_pkMode->Type() )
	{
		return;
	}

	BM::CAutoMutex Lock( m_kRscMutex );

	if( m_pkMode->IsEndGame() )
	{
		return;
	}

	PgPvPType_Love * pLoveMode = NULL;
	pLoveMode = dynamic_cast<PgPvPType_Love *>(m_pkMode);
	if( !pLoveMode )
	{
		return;
	}

	// 우리편 곰이 제라리에 있을 때만 라운드를 승리할 수 있다.
	ETeam TouchDownTeam = static_cast<ETeam>(pUnit->GetAbil(AT_TEAM));
	if( TouchDownTeam == TEAM_RED )
	{
		if( !pLoveMode->OnRedBear() )
		{
			return;
		}
	}
	else if( TouchDownTeam == TEAM_BLUE )
	{
		if( !pLoveMode->OnBlueBear() )
		{
			return;
		}
	}

	pLoveMode->BearTouchDown(pUnit, this);
	SetAutoNextState( m_pkMode->GetTime(PVP_TIME_ROUNDTERM) );
}

void PgWarGround::CheckGenBear()
{
	BM::CAutoMutex Lock( m_kRscMutex );

	PgPvPType_Love * pLove = NULL;
	pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
	if( !pLove )
	{
		return;
	}


	bool bSend = false;
	ETeam RegenTeam = TEAM_NONE;
	if( pLove->OnRedBearRegen() )
	{	// 빨간 곰을 정해진 울타리 안에 리젠 시켜야 한다.
		ActivateInstanceItemGenGroup(EIGG_RED, true, false, 0);
		pLove->OnRedBearRegen(false);
		RegenTeam = TEAM_RED;
		bSend = true;
	}

	if( pLove->OnBlueBearRegen() )
	{	// 파란 곰을 정해진 울타리 안에 리젠 시켜야 한다.
		ActivateInstanceItemGenGroup(EIGG_BULE, true, false, 0);
		pLove->OnBlueBearRegen(false);
		RegenTeam = TEAM_BLUE;
		bSend = true;
	}

	if( bSend )
	{
		BM::Stream Packet(PT_M_C_NFY_DISPLAY_DROPBEAR_TIMER);
		Packet.Push(static_cast<int>(0));				// 드롭된건지 본진으로 돌아간건지 누가 먹은건지 WORD 0=본진생성 1=드랍 2=누가먹음.
		Packet.Push(RegenTeam);						// 곰의 팀.
		Packet.Push(0);									// 남은 시간.

		Broadcast(Packet);
	}
}

void PgWarGround::CheckDropBearDeleteTime()
{	// 곰 운송 도중 캐릭터가 사망해서 바닥에 떨어진 곰이 원래 자리로 되돌아갈 시간이 됐는지 검사한다.
	BM::CAutoMutex Lock( m_kRscMutex );

	if( m_ContInsItemDropInfo.empty() )
	{	// 드랍 정보 컨테이너가 비어 있으면 더이상 진행 하지 않아도 됨.
		return ;
	}

	CONT_INSITEMDROPINFO::iterator iter = m_ContInsItemDropInfo.begin();
	for( ; iter != m_ContInsItemDropInfo.end() ; ++iter )
	{
		CONT_INSITEMDROPINFO::mapped_type & Element = iter->second;

		PgPvPType_Love * pLove = NULL;
		pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
		if( !pLove )
		{
			return;
		}

		DWORD AddTime = 0;
		if( Element.Team == TEAM_RED )
		{	// 빨간 곰 드랍시간 누적.
			AddTime = pLove->RedBearDropAccumTime();
			pLove->RedBearDropAccumTime(AddTime + 1000);
		}
		else if( Element.Team == TEAM_BLUE )
		{	// 파란 곰 드랍시간 누적.
			AddTime = pLove->BlueBearDropAccumTime();
			pLove->BlueBearDropAccumTime(AddTime + 1000);
		}

		if( AddTime >= DROPBEAR_ITEM_DELTIME )
		{	// 누적 시간이 넘었으면.

			Element.bDelete = true;

			CUnit* pUnit = NULL;
			CONT_OBJECT_MGR_UNIT::iterator iter;
			PgObjectMgr::GetFirstUnit(UT_GROUNDBOX, iter);
			while( (pUnit = PgObjectMgr::GetNextUnit(UT_GROUNDBOX, iter)) != NULL )
			{
				PgGroundItemBox * pBox = dynamic_cast<PgGroundItemBox*>(pUnit);
				if( pBox )
				{
					if( Element.DroperGuid == pBox->Caller() )
					{
						BM::Stream Packet( PT_M_C_ANS_PICKUPGBOX, BM::GUID::NullData() );
						Packet.Push( pBox->GetID() );
						Broadcast(Packet);					

						this->ReleaseUnit(pUnit, false, false);

						EINSTANCEITEM_GEN_GROUP GenGroup = EIGG_ANY;
						PgGTrigger_Score * pScoreTrigger = NULL;
						switch( Element.Team )
						{
						case TEAM_RED:
							{
								pLove->OnBlueBear(true);
								pLove->OnRedBearRegen(true);
								pLove->RedBearDropAccumTime(0);
								pScoreTrigger = pLove->GetBlueScoreTrigger();
								GenGroup = EIGG_RED;
							}break;
						case TEAM_BLUE:
							{
								pLove->OnRedBear(true);
								pLove->OnBlueBearRegen(true);
								pLove->BlueBearDropAccumTime(0);
								pScoreTrigger = pLove->GetRedScoreTrigger();
								GenGroup = EIGG_BULE;
							}break;
						default:
							{
								pLove->OnRedBear(true);
								pLove->OnBlueBear(true);
								GenGroup = EIGG_ANY;
							}break;
						}
				
						if( pScoreTrigger )
						{
							VEC_UNIT TeamUnit;
							CUnit * pPlayerUnit = NULL;
							CONT_OBJECT_MGR_UNIT::iterator unit_iter;
							PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_iter);
							while( (pPlayerUnit = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_iter)) != NULL )
							{
								if( pPlayerUnit->GetAbil(AT_TEAM) == pScoreTrigger->GetTriggerTeamNo() )
								{
									TeamUnit.push_back(pPlayerUnit);
								}
							}

							if( !TeamUnit.empty() )
							{
								pScoreTrigger->CheckBearOnTrigger(this, TeamUnit);
							}
						}
					}
				}
			}
			
			RemoveReserveDeleteInsItem( Element.DroperGuid );
		}
	}

	CleanInsItemDropInfo();
}

void PgWarGround::PickupBear(CUnit * pUnit)
{
	if( !pUnit || !m_pkMode )
	{
		return;
	}

	BM::CAutoMutex Lock( m_kRscMutex );

	PgPvPType_Love * pLove = NULL;
	pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);

	if( pLove )
	{
		WORD RemainTime = 0;
		ETeam LooterTeam = static_cast<ETeam>(pUnit->GetAbil(AT_TEAM));
		switch( LooterTeam )
		{
		case TEAM_RED:
			{				
				pLove->OnBlueBear(false);
				RemainTime = DROPBEAR_ITEM_DELTIME - pLove->RedBearDropAccumTime();
			}break;
		case TEAM_BLUE:
			{
				pLove->OnRedBear(false);
				RemainTime = DROPBEAR_ITEM_DELTIME - pLove->BlueBearDropAccumTime();
			}break;
		default:
			{
				pLove->OnRedBear(true);
				pLove->OnBlueBear(true);
			}break;
		}

		if( pUnit->GetAbil(AT_BEAR_EFFECT_RED) || pUnit->GetAbil(AT_BEAR_EFFECT_BLUE) )
		{	// 곰 이펙트를 얻었다면.
			{
				BM::Stream Packet(PT_M_C_NFY_LOVEMODE_MESSAGE);
				Packet.Push(74109);
				Packet.Push(false);
				Packet.Push(LooterTeam);
				Packet.Push(pUnit->Name());
				Broadcast(Packet);

				BM::Stream WarnPacket(PT_M_C_NFY_LOVEMODE_MESSAGE);
				WarnPacket.Push(74113);
				WarnPacket.Push(false);
				WarnPacket.Push(0);
				pUnit->Send(WarnPacket);
			}

			pLove->SendLoveGamePoint(pUnit->GetID(), PVP_ST_PICKUPBEAR);

			VEC_SUMMONUNIT const & ContSummonUnit = pUnit->GetSummonUnit();
			if( !ContSummonUnit.empty() )
			{	// 소환체를 가지고 있으면.
				PgSummoned * pSommonUnit = NULL;
				VEC_SUMMONUNIT::const_iterator sommon_iter = ContSummonUnit.begin();
				for( ; sommon_iter != ContSummonUnit.end() ; ++sommon_iter )
				{
					pSommonUnit = dynamic_cast<PgSummoned*>(GetUnit(sommon_iter->kGuid));
					if( pSommonUnit && (false == pSommonUnit->IsNPC()) )
					{	// 전부다 제거 한다.
						::OnDamaged(NULL, pSommonUnit, 0, pSommonUnit->GetAbil(AT_HP), this, g_kEventView.GetServerElapsedTime());
					}
				}
			}

			BM::Stream Packet(PT_M_C_NFY_DISPLAY_DROPBEAR_TIMER);
			Packet.Push(static_cast<int>(2));	// 드롭된건지 본진으로 돌아간건지 누가 먹은건지 0=본진생성 1=드랍 2=누가먹음
			Packet.Push(LooterTeam);			// 곰의 팀
			Packet.Push(RemainTime);			// 남은 시간

			Broadcast(Packet);
		}
	}
}

void PgWarGround::ResetBreakObject()
{
	BM::CAutoMutex Lock( m_kRscMutex );

	if( IsModeType(PVP_TYPE_LOVE) )
	{
		PgPvPType_Love * pLove = NULL;
		pLove = dynamic_cast<PgPvPType_Love *>(m_pkMode);
		if( pLove )
		{
			CUnit * pUnit = NULL;
			CONT_OBJECT_MGR_UNIT::iterator Itor;
			PgObjectMgr::GetFirstUnit(UT_OBJECT, Itor);
			while( (pUnit = PgObjectMgr::GetNextUnit(UT_OBJECT, Itor)) != NULL )
			{
				PgObjectUnit * pObjUnit = dynamic_cast<PgObjectUnit *>(pUnit);
				if( pObjUnit )
				{
					pLove->BearFenceReset(pObjUnit);
				}
			}
		}
	}
}

void PgWarGround::AddInsItemDropInfo(BM::GUID const & Droper, ETeam Team)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	SInsItemDropInfo DropInfo;

	DropInfo.DroperGuid = Droper;
	DropInfo.DropTime = BM::GetTime32();
	DropInfo.Team = Team;
	DropInfo.bDelete = false;

	m_ContInsItemDropInfo.insert( std::make_pair(Droper, DropInfo) );
}

void PgWarGround::RemoveInsItemDropInfo(BM::GUID const & CharGuid)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	m_ContInsItemDropInfo.erase(CharGuid);
}

void PgWarGround::CleanInsItemDropInfo()
{
	BM::CAutoMutex Lock(m_kRscMutex);

	CONT_INSITEMDROPINFO::iterator iter = m_ContInsItemDropInfo.begin();
	while( iter != m_ContInsItemDropInfo.end() )
	{
		CONT_INSITEMDROPINFO::mapped_type & Element = (*iter).second;
		if( Element.bDelete )
		{
			iter = m_ContInsItemDropInfo.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}