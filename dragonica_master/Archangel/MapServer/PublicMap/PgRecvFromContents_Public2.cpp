#include "stdafx.h"
#include "Variant/PgQuestInfo.h"
#include "constant.h"
#include "PgQuest.h"
#include "PgAction.h"
#include "PgActionQuest.h"
#include "PgPacketManager.h"
#include "PgWarGround.h"
#include "PgBattleForEmporia.h"
#include "PgPvPType_DeathMatch.h"
#include "PgWarType_ProtectDragon.h"
#include "PgActionAchievement.h"
#include "PgPvPType_WinnersMatch.h"
#include "PgPvPType_WinnersTeam.h"
#include "PgPvPType_AceMatch.h"
#include "PgPvPType_Love.h"

void PgWarGround::VUpdate( BM::CSubject< BM::Stream* > *const pChangedSubject, BM::Stream* pkNfy )
{
	BM::CAutoMutex Lock(m_kRscMutex); 
	
	size_t const iRdPos = pkNfy->RdPos();

	BM::Stream::DEF_STREAM_TYPE wType = 0;
	if ( pkNfy->Pop(wType) )
	{
		switch( wType )
		{
		// default에 안걸리면 PvPMode한테 받은 것임.
		case PT_M_C_NFY_GAME_INTRUDER:
			{
				BM::GUID kIgnoreGuid;
				pkNfy->Pop( kIgnoreGuid );

				pkNfy->PosAdjust();
				Broadcast( *pkNfy, kIgnoreGuid );
			}break;
		case PT_M_C_NFY_GAME_EXIT:
		case PT_M_C_NFY_GAME_EVENT_KILL:
		case PT_M_C_NFY_ADD_POINT_USER:
		case PT_M_C_NFY_GAME_RESULT_END:
			{
				pkNfy->PosAdjust();
				Broadcast( *pkNfy );
			}break;
		case PT_M_C_NFY_ABIL_DEC:
			{
				BM::GUID kUnitGuid;
				pkNfy->Pop( kUnitGuid );
				
				CUnit *pkUnit = GetUnit( kUnitGuid );
				if ( pkUnit )
				{
					WORD wType = 0;
					int iValue = 0;
					pkNfy->Pop( wType );
					pkNfy->Pop( iValue );
					iValue = __max( pkUnit->GetAbil(wType) - iValue, 0 );
					pkUnit->SetAbil( wType, iValue );
					pkUnit->NftChangedAbil( wType );
				}
			}break;
		case PT_A_G_NFY_DESTROY_OBJECT_REGEN:
			{
				ActivateMonsterGenGroup( 11, true, true, 0 );
				ActivateObjectUnitGenGroup( 11, true, true);
//				ObjectUnitGenerate( true, 11 );
				PgGround::OnActivateEventMonsterGroup();
			}break;
		case PT_A_G_NFY_PVP_REWARD:
			{
				RecvPvPReward( *pkNfy );
			}break;
		case PT_A_G_NFY_RESPAWN:
			{
				BM::GUID kUnitGuid;
				pkNfy->Pop( kUnitGuid );

				CUnit *pkUnit = GetUnit( kUnitGuid );
				if ( pkUnit )
				{
					int iPotalNo = 0;
					int iSpawnType = 0;
					pkNfy->Pop( iPotalNo );
					pkNfy->Pop( iSpawnType );
					SendToSpawnLoc( pkUnit, iPotalNo, true, iSpawnType );
				}
			}break;
		case PT_A_G_NFY_ADD_STATETIME:
			{
				if ( m_dwAutoStateRemainTime > 0 )
				{
					int iTime = 0;
					bool bBroadCast = false;
					pkNfy->Pop( iTime );
					pkNfy->Pop( bBroadCast );

					if ( iTime > 0 )
					{
						m_dwAutoStateRemainTime += static_cast<DWORD>(iTime);
					}
					else
					{
						int iTemp = -iTime;
						DWORD dwMinusTime = static_cast<DWORD>(iTemp);
						if ( m_dwAutoStateRemainTime > dwMinusTime )
						{
							m_dwAutoStateRemainTime -= dwMinusTime;
						}
						else
						{
							m_dwAutoStateRemainTime = 0;
						}
					}

					if ( true == bBroadCast )
					{
						BM::Stream kPacket( PT_M_C_NFY_SYNC_GAMTTIME, m_dwAutoStateRemainTime );
						Broadcast( kPacket );
					}
				}
			}break;
		case PT_M_M_NFY_BREAK_LOVE_FENCE:
			{
				RecvBreakLoveFence(*pkNfy);				
			}break;
		default:
			{
				CUnit* pkUnit = dynamic_cast<CUnit*>(pChangedSubject);
				if( pkUnit )
				{
					pkUnit = PgObjectMgr::GetUnit(pkUnit->GetID());	//이 그라운드에 있는가.
					if( pkUnit )
					{
						if ( !this->VUpdate( pkUnit, wType, pkNfy ) )
						{
							this->ProcNotifyMsg(pkUnit, wType, pkNfy);
						}
					}
					else
					{
						//컨버팅 안됨.
						INFO_LOG(BM::LOG_LV1, __FL__<<L"There None Unit");
					}
				}
			}break;
		}
	}
	pkNfy->RdPos(iRdPos);
}

bool PgWarGround::VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy )
{
	switch(wType)
	{
	case PT_C_N_REQ_JOIN_PARTY:
	case PT_C_N_REQ_JOIN_PARTYFIND:
	case PT_C_N_ANS_JOIN_PARTY:
	case PT_C_N_ANS_JOIN_PARTYFIND:
	case PT_C_N_REQ_LEAVE_PARTY:
	case PT_C_N_REQ_KICKOUT_PARTY_USER:
	case PT_C_N_REQ_PARTY_CHANGE_MASTER:
	case PT_C_N_REQ_PARTY_CHANGE_OPTION:
		{
			pkUnit->SendWarnMessage(700034);//파티명령어를 사용할 수 없는 지역입니다.
			CAUTION_LOG( BM::LOG_LV5, __FL__<<L"PgWarGround : Impossible Party!!! PacketType["<<wType<<L"], Hacking User["<<pkUnit->Name()<<L":"<<pkUnit->GetID()<<L"]" );
		}break;
	case PT_S_A_REQ_AUTOLOGIN:
		{
			// PvP에서는 사용 할 수 없음
			pkUnit->SendWarnMessage(98);
		}break;
	case PT_C_M_NFY_RECENT_MAP_MOVE:
	case PT_C_M_REQ_JOIN_EMPORIA:
		{
			if ( true == IsModeType( PVP_TYPE_ALL ) )
			{// PvP에서는 막아야 한다.
				CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Bad Packet Type=") << wType << _T("User=") << pkUnit->Name() << _T("/") << pkUnit->GetID() );
			}
			else
			{
				return PgGround::VUpdate( pkUnit, wType, pkNfy );
			}
		}break;

	case PT_C_T_REQ_EXIT_ROOM:
		{
			if ( IsModeType( PVP_TYPE_ALL ) )
			{
				if ( INDUN_STATE_CLOSE == m_eState )
				{
					SReqMapMove_MT kRMM(MMET_BackToPvP);
					kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );

					PgReqMapMove kMapMove( this, kRMM, NULL );
					if ( kMapMove.Add( dynamic_cast<PgPlayer*>(pkUnit) ) )
					{
						kMapMove.DoAction();
					}
				}
				else
				{
//					INFO_LOG( BM::LOG_LV5, __FL__<<L"[PT_C_PM_REQ_EXIT_ROOM] Not State["<<m_eState<<L"] User["<<pkUnit->Name()<<L"]["<<pkUnit->GetID()<<L"]" );
				}
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, __FL__<<L"[PT_C_PM_REQ_EXIT_ROOM] 'Bad Mode' User["<<pkUnit->Name()<<L"]["<<pkUnit->GetID()<<L"]" );
			}		
		}break;
	case PT_C_M_REQ_CHAT:
		{
			if (	true == IsModeType( PVP_TYPE_ALL ) 
				&&	INDUN_STATE_CLOSE == m_eState 
				)
			{
				BM::Stream kLobbyPakcet( wType, pkUnit->GetID() );
				kLobbyPakcet.Push(*pkNfy);
				SendToPvPLobby( kLobbyPakcet, m_pkMode->GetPvPLobbyID() );
			}
			else
			{
				PgGround::VUpdate( pkUnit, wType, pkNfy );
			}
		}break;
	case PT_C_T_REQ_TEAM_CHANGE:
	case PT_C_T_REQ_GAME_READY:
	case PT_C_T_REQ_KICK_ROOM_USER:
	case PT_C_T_REQ_MODIFY_ROOM:
	case PT_C_T_REQ_SLOTSTATUS_CHANGE:
	case PT_C_T_REQ_ENTRUST_MASTER:
	case PT_C_T_REQ_ENTRY_CHANGE:
		{
			if ( true == IsModeType( PVP_TYPE_ALL ) )
			{
				if ( INDUN_STATE_CLOSE == m_eState )
				{
					BM::Stream kRoomPacket( wType, pkUnit->GetID() );
					kRoomPacket.Push(*pkNfy);
					m_pkMode->Send( kRoomPacket );
				}
				else
				{
//					INFO_LOG( BM::LOG_LV5, __FL__<<L"[PT_C_PM_REQ_EXIT_ROOM] Not State["<<m_eState<<L"] User["<<pkUnit->Name()<<L"]["<<pkUnit->GetID()<<L"]" );
				}	
			}
		}break;
	case PT_C_T_REQ_INVITE_PVP:
		{
			if (	true == IsModeType( PVP_TYPE_ALL ) 
				&&	INDUN_STATE_CLOSE == m_eState 
				)
			{
				BM::Stream kLobbyPakcet( wType, pkUnit->GetID() );
				kLobbyPakcet.Push(*pkNfy);
				SendToPvPLobby( kLobbyPakcet, m_pkMode->GetPvPLobbyID() );
			}
		}break;
	case PT_C_M_REQ_SELECT_BATTLE_AREA:
		{
			//if ( true == IsModeType(WAR_TYPE_PROTECTDRAGON) )
			//{
				PgBattleForEmporia *pkType = dynamic_cast<PgBattleForEmporia*>(m_pkMode);
				if ( pkType )
				{
					if ( true == pkUnit->IsAlive() )
					{// 살아있을 때만 사용 가능
						int iAreaIndex = 0;
						if ( true == pkNfy->Pop( iAreaIndex ) )
						{
							pkType->SetSelectArea( pkUnit, iAreaIndex, this );
						}
					}
				}
				else
				{
					VERIFY_INFO_LOG( false, BM::LOG_LV0, _T("[PT_C_M_REQ_SELECT_BATTLE_AREA] Critical Error!! Point is NULL") );
				}
			//}
		}break;
	case PT_U_G_NFY_SHARE_HP_SET:
		{
			int iHP = 0;
			pkNfy->Pop( iHP );
			int const iMaxUnit = pkUnit->GetSummonUnitCount();
			for(int i=0; i<iMaxUnit;++i)
			{
				BM::GUID const & kChildID = pkUnit->GetSummonUnit(i);
				CUnit * pkChild = GetUnit(kChildID);
				if(pkChild)
				{
					pkChild->SetAbil(AT_HP, iHP);
				}
			}
		}break;
	case PT_C_M_ANS_SELECT_ENTRY:
		{
			if ( !IsModeType( PVP_TYPE_ACE ) )
			{//에이스 매치에서만 쓰는 패킷
				if ( INDUN_STATE_RESULT == m_eState )
				{//결과 대기 중일때만 패킷이 올 수 있음.
					int iTeam = 0;
					BM::GUID kCharGuid;
					pkNfy->Pop( iTeam );
					pkNfy->Pop( kCharGuid );

					m_pkMode->SelectEntry(iTeam, kCharGuid);
				}
			}
		}break;
	default:
		{
			return PgIndun::VUpdate( pkUnit, wType, pkNfy );
		}break;
	}
	return true;
}

bool PgWarGround::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch(usType)
	{
	case PT_T_M_NFY_CANCEL_GROUND:
		{
			Cancel();
		}break;
 	case PT_T_M_REQ_READY_GROUND:
	case PT_N_M_REQ_READY_EMPORIA_BATTLE:
 		{
 			Ready( *pkPacket );
 		}break;
	case PT_T_M_NFY_EXIT_ROOM:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop( kCharGuid );
			if ( m_pkMode )
			{
				m_pkMode->Exit( kCharGuid );
			}
		}break;
	case PT_T_M_REQ_KICK_ROOM_USER:
		{
			BM::GUID kCharGuid;
			pkPacket->Pop( kCharGuid );
			if ( m_pkMode )
			{
				m_pkMode->Exit( kCharGuid );
			}

			PgPlayer *pkPlayer = GetUser( kCharGuid );
			if ( pkPlayer )
			{
				SReqMapMove_MT kRMM(MMET_KickToPvP);
				kRMM.kTargetKey.GroundNo( m_pkMode->GetPvPLobbyID() );

				PgReqMapMove kMapMove( this, kRMM, NULL );
				if ( kMapMove.Add( pkPlayer ) )
				{
					kMapMove.DoAction();
				}
			}
			else
			{
				// 못찾을 수도있다. PvPLobby로 알려주자
				BM::Stream kFailedPacket( PT_M_T_ANS_KICK_ROOM_USER_NOTFOUND, kCharGuid );
				kFailedPacket.Push( this->GroundKey() );
				SendToCenter( kFailedPacket );
			}
		}break;
	default:
		{
			return PgIndun::RecvGndWrapped( usType, pkPacket );
		}
	}
	return true;
}

CUnit * GetOwner(CUnit * pkUnit, PgGround * pkGround)
{
	CUnit* pkOwner = pkUnit;
	if( pkOwner )
	{
		switch( pkOwner->UnitType() )
		{
		case UT_PET:
		case UT_SUMMONED:
			{
				pkOwner = pkGround->GetUnit(pkOwner->Caller());
			}break;
		case UT_ENTITY:
			{
				CUnit* pkCaller = pkGround->GetUnit(pkOwner->Caller());
				while( pkCaller && pkCaller->IsUnitType(UT_ENTITY) )
				{
					if( !pkCaller->IsHaveCaller() )
					{
						break;
					}
					pkCaller = pkGround->GetUnit(pkCaller->Caller());
				}

				pkOwner = pkCaller;
			}break;
		}
	}
	return pkOwner;
}

void PgWarGround::RecvUnitDie( CUnit *pkUnit )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if( !pkUnit )
	{
		return;
	}

	if( m_pkMode 
	&& (m_eState == INDUN_STATE_PLAY) )
	{
		CUnit *pkTarget = GetOwner( GetTarget(pkUnit), this );
		m_pkMode->SetEvent_Kill( pkUnit, pkTarget, this );

		if ( UT_PLAYER == pkUnit->UnitType() )
		{
			PgPlayer *pkDeathPlayer = dynamic_cast<PgPlayer*>(pkUnit);
			if ( pkDeathPlayer )
			{
				if ( 0 == pkDeathPlayer->DeathTime() )
				{
					if ( !pkTarget )
					{
						pkTarget = PgObjectMgr2::GetRandomUnit( UT_PLAYER, pkDeathPlayer->GetID() );
					}

					if ( pkTarget )
					{
						SetOBMode_Start( pkDeathPlayer, pkTarget );
					}
				}

				ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_DIE>()( pkDeathPlayer, 1, this, (pkTarget ? pkTarget : pkUnit) );
			}

			if (	pkTarget
				&&	UT_PLAYER == pkTarget->UnitType() )
			{
				PgPlayer * pkCasterPlayer = dynamic_cast<PgPlayer*>(pkTarget);
				if ( pkCasterPlayer )
				{
					ALRAM_MISSION::PgClassAlramMissionEvent<ALRAM_MISSION::EVENT_KILL_ANYBODY>()( pkCasterPlayer, 1, this, pkUnit );
				}
			}
		}
	}

	if( pkUnit->UnitType() != UT_PLAYER )
	{
		PgGround::RecvUnitDie(pkUnit);
	}
}

void PgWarGround::RecvPvPReward( BM::Stream& kPacket )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	if ( !m_pkMode )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("Critical Error!") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("m_pkMode is NULL"));
		return;
	}

	int iRoomIndex = 0;
	PgPvPType_DeathMatch *pkPvPType = dynamic_cast<PgPvPType_DeathMatch*>(m_pkMode);
	if ( pkPvPType )
	{
		iRoomIndex = pkPvPType->GetRoomIndex() + 1;	
	}
	else
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << _T("What Type?? ") << m_pkMode->Type() );
		CAUTION_LOG( BM::LOG_LV0, __FL__ << _T("What Type?? ") << m_pkMode->Type() );
	}

	EPVPTYPE const kType = m_pkMode->Type();

	WORD const kWinRoundPoint = (WORD)PgWarMode::ms_kSetting.Get( kType, PVP_ST_WIN_ROUND_POINT );
	WORD const kWinGamePoint = (WORD)PgWarMode::ms_kSetting.Get( kType, PVP_ST_WIN_GAME_POINT );

	bool bIsExercise = false;
	int iWinTeam = TEAM_NONE;
	CONT_PVP_GAME_REWARD kReward;
	BYTE kRedWin = 0;
	BYTE kBlueWin = 0;
	BYTE ucMaxRound = 0;
	EPVPTYPE ePvPType = PVP_TYPE_NONE;
	kPacket.Pop( bIsExercise );
	kPacket.Pop( iWinTeam );
	PU::TLoadArray_M( kPacket, kReward );
	kPacket.Pop( kRedWin );
	kPacket.Pop( kBlueWin );
	kPacket.Pop( ePvPType );
	kPacket.Pop( ucMaxRound );

	// 정렬을 하고.
	std::sort( kReward.begin(), kReward.end(), std::greater<CONT_PVP_GAME_REWARD::value_type>() );

	CONT_DEF_PVP_REWARD const *pkPvPReward = NULL;
	g_kTblDataMgr.GetContDef( pkPvPReward );

	// 지급 아이템 정보를 준비
	TBL_DEF_PVP_REWARD_ITEM kPvPRewardItem;
	if ( !m_pkMode->IsAbusing() )
	{
		CONT_DEF_PVP_REWARD_ITEM const *pkPvPRewardItem = NULL;
		g_kTblDataMgr.GetContDef( pkPvPRewardItem );

		CONT_DEF_PVP_REWARD_ITEM::const_iterator item_itr = pkPvPRewardItem->find( m_pkMode->GetRewardID() );
		if ( item_itr != pkPvPRewardItem->end() )
		{
			kPvPRewardItem = item_itr->second;
		}
	}

	// 순위를 매긴다.
	BYTE kRankWin = 1;
	BYTE kRankLose = 1;

	BYTE kRank = 1;
	BYTE kChkRank = kRank;
	BYTE const kRecordsBase = ( bIsExercise ? GAME_DRAW_EXERCISE : GAME_DRAW );

	CONT_PVP_GAME_REWARD::iterator chk_reward_itr = kReward.begin();
	CONT_PVP_GAME_REWARD::iterator reward_itr = kReward.begin();
	for ( ; reward_itr!=kReward.end(); ++reward_itr )
	{
		// Rank Setting
		if ( *reward_itr < *chk_reward_itr )
		{
			chk_reward_itr = reward_itr;
			kChkRank = kRank;
		}
		reward_itr->kRank = kChkRank;

		// Rank Bonus Point Setting
		reward_itr->kRankPoint = (WORD)PgWarMode::ms_kSetting.Get( m_pkMode->Type(), PVP_ST_RANK_POINT_CHK+(reward_itr->kRank) );

		PgPlayer *pkPlayer = GetUser( reward_itr->kCharGuid );
		if ( pkPlayer )
		{
			switch(kType)
			{
			case PVP_TYPE_DM:
				{
					PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_ENTRY_DM, 1, pkPvPType->GndKey() );
					kMA.DoAction(pkPlayer,NULL);
				}break;
			case PVP_TYPE_KTH:
				{
				}break;
			case PVP_TYPE_ANNIHILATION:
				{
					PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_ENTRY_ANNIHILATION, 1, pkPvPType->GndKey() );
					kMA.DoAction( pkPlayer, NULL );
				}break;
			case PVP_TYPE_DESTROY:
				{
					PgAddAchievementValue kMA(AT_ACHIEVEMENT_PVP_ENTRY_DESTROY,1, pkPvPType->GndKey());
					kMA.DoAction(pkPlayer,NULL);
				}break;
			case PVP_TYPE_LOVE:
				{
					
				}break;
			case PVP_TYPE_ALL:
				{
				}break;
			}

			// 로그를 남겨
			PgLogCont kLogCont(ELogMain_Contents_PVP, ELogSub_PVP_Result );
			kLogCont.MemberKey( pkPlayer->GetMemberGUID() );
			kLogCont.CharacterKey( pkPlayer->GetID() );
			kLogCont.ID( pkPlayer->MemberID() );
			kLogCont.UID( pkPlayer->UID());
			kLogCont.Name( pkPlayer->Name() );
			kLogCont.Class( pkPlayer->GetAbil(AT_CLASS) );
			kLogCont.Level( pkPlayer->GetAbil(AT_LEVEL) );
			kLogCont.GroundNo( GetGroundNo() );
			kLogCont.ChannelNo( g_kProcessCfg.ChannelNo() );

			PgLog kLog( ELOrderMain_Result_One );
			kLog.Set( 0, iRoomIndex );					// iValue1 RoomNo
			kLog.Set( 2, GroundKey().Guid().str() );			// guidValue1 그라운드 GUID

			// Win Bonus Point Setting For Team
			int const iMyTeam = pkPlayer->GetAbil(AT_TEAM);
			switch ( iWinTeam )
			{
			case TEAM_NONE:
				{
					reward_itr->kWinLose = kRecordsBase + GAME_DRAW;
				}break;
			case TEAM_RED:
			case TEAM_BLUE:
				{
					reward_itr->kWinLose = (( iMyTeam == iWinTeam ) ? GAME_WIN : GAME_LOSE);
					reward_itr->kWinLose += kRecordsBase;

					if ( TEAM_RED == iMyTeam )
					{
						reward_itr->kWinPoint = kWinRoundPoint * (WORD)kRedWin;
					}
					else if ( TEAM_BLUE == iMyTeam )
					{
						reward_itr->kWinPoint = kWinRoundPoint * (WORD)kBlueWin;
					}
				}break;
			case TEAM_PERSONAL_CHK:
				{
					if (	(1 == reward_itr->kRank)
						&&	(0 < reward_itr->kResult.usKillCount) )
					{
						// 개인전에서는 킬카운트가 1명 이상 있어야 승리다.
						reward_itr->kWinPoint = kWinRoundPoint;
						reward_itr->kWinLose = GAME_WIN;
					}
					else
					{
						reward_itr->kWinLose = GAME_LOSE;
					}

					reward_itr->kWinLose += kRecordsBase;
				}break;
			}

			BYTE byRewardItemCount = 0;

			kLog.Set( 1, static_cast<int>(reward_itr->kWinLose) );	// iValue2 승무패여부
			switch( reward_itr->kWinLose )
			{
			case GAME_DRAW:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN] );		// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE] );		// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW]+1 );	// iValue5 변화된 총무승부

					byRewardItemCount = kPvPRewardItem.byDraw_ItemCount;
				}break;
			case GAME_WIN:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN]+1 );	// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE] );		// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW] );		// iValue5 변화된 총무승부

					reward_itr->kWinPoint += kWinGamePoint;

					byRewardItemCount = kPvPRewardItem.byWin_ItemCount;

					{
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_RESULT_WIN, 1, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}

					{// 연승 정보 지면 리셋됨
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_60MILLIONMEN, 1, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}

				}break;
			case GAME_LOSE:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN] );		// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE]+1 );	// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW] );		// iValue5 변화된 총무승부

					byRewardItemCount = kPvPRewardItem.byLose_ItemCount;

					{
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_RESULT_LOSE, 1, GroundKey());
						kMA.DoAction( pkPlayer,  NULL );
					}

					{// 한번이라도 지면 그동안 누적된 연승정보는 모두 리셋된다.
						PgSetAchievementValue kMA( AT_ACHIEVEMENT_60MILLIONMEN, 0, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}
				}break;
			case GAME_DRAW_EXERCISE:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN_EXERCISE] );		// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE_EXERCISE] );	// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW_EXERCISE]+1 );	// iValue5 변화된 총무승부

					byRewardItemCount = kPvPRewardItem.byDraw_ItemCount;
				}break;
			case GAME_WIN_EXERCISE:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN_EXERCISE]+1 );	// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE_EXERCISE] );	// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW_EXERCISE] );	// iValue5 변화된 총무승부

					reward_itr->kWinPoint += kWinGamePoint;

					byRewardItemCount = kPvPRewardItem.byWin_ItemCount;

					{
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_RESULT_WIN, 1, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}

					{// 연승 정보 지면 리셋됨
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_60MILLIONMEN, 1, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}

				}break;
			case GAME_LOSE_EXERCISE:
				{
					kLog.Set( 2, pkPlayer->SPvPReport::m_iRecords[GAME_WIN_EXERCISE] );		// iValue3 변화된 총승
					kLog.Set( 3, pkPlayer->SPvPReport::m_iRecords[GAME_LOSE_EXERCISE]+1 );	// iValue4 변화된 총패
					kLog.Set( 4, pkPlayer->SPvPReport::m_iRecords[GAME_DRAW_EXERCISE] );	// iValue5 변화된 총무승부

					byRewardItemCount = kPvPRewardItem.byLose_ItemCount;

					{
						PgAddAchievementValue kMA( AT_ACHIEVEMENT_PVP_RESULT_LOSE, 1, GroundKey());
						kMA.DoAction( pkPlayer,  NULL );
					}

					{// 한번이라도 지면 그동안 누적된 연승정보는 모두 리셋된다.
						PgSetAchievementValue kMA( AT_ACHIEVEMENT_60MILLIONMEN, 0, GroundKey());
						kMA.DoAction( pkPlayer, NULL );
					}

				}break;
			}

			// PvP Quest
			PgAction_QuestPvP kQuestAction( GroundKey(), reward_itr->kWinLose );
			kQuestAction.DoAction( pkPlayer, NULL );

			// 일단 win_spawn_1로 모이게 한다.
			if( g_kLocal.ServiceRegion() != LOCAL_MGR::NC_USA )
			{
				if(NULL==m_pkMode || PVP_TYPE_KTH!=m_pkMode->Type())
				{
					SendToSpawnLoc( pkPlayer, 1, false, E_SPAWN_WIN );
				}
			}

			if ( !bIsExercise && pkPvPReward && pkPvPReward->size() )
			{// CP 보상
				CONT_DEF_PVP_REWARD::const_iterator def_rwd_itr = std::find_if( pkPvPReward->begin(), pkPvPReward->end(), SPvPRewardFind(*reward_itr) );
				if ( def_rwd_itr == pkPvPReward->end() )
				{
					def_rwd_itr = pkPvPReward->end() - 1;
				}

				// CP
				reward_itr->iCP = def_rwd_itr->iCP;		

				// 명예 포인트 공식
				int iRankCheck = 1;

				if( GAME_WIN == reward_itr->kWinLose )
				{
					iRankCheck = kRankWin;
					++kRankWin;
				}
				else
				{
					iRankCheck = kRankLose;
					++kRankLose;
				}
				
				int iAddCP = 0;
				if( 0 != iRankCheck )
				{
					iAddCP = kReward.size() * 100 / iRankCheck;

					if( GAME_LOSE == reward_itr->kWinLose )
					{
						iAddCP = __max( iAddCP - PVP_LOSE_SCORE, 0 );
					}					
				}

				switch( kType )
				{
				case PVP_TYPE_DM:
					{
						iAddCP += (GAME_WIN == reward_itr->kWinLose) ? PVP_CP_DM_WIN_SCORE : PVP_CP_DM_LOSE_SCORE;
					}break;
				case PVP_TYPE_ANNIHILATION:
					{
						iAddCP += (GAME_WIN == reward_itr->kWinLose) ? PVP_CP_ANNIHILATION_WIN_SCORE : PVP_CP_ANNIHILATION_LOSE_SCORE;
					}break;
				default:
					{
					}break;
				}

				if( PVP_TYPE_ANNIHILATION == ePvPType )
				{ // 설정한 라운드 수에 따라 CP 보상 변경(섬멸전의 경우에만).
					switch( ucMaxRound )
					{
					case 3:	// 3라운드 설정.
						{
							iAddCP *= 0.4f;
						}break;
					case 5: // 5라운드 설정.
						{
							iAddCP *= 0.6f;
						}break;
					case 7: // 7라운드 설정.
						{
							iAddCP *= 0.8f;
						}break;
					case 9: // 9라운드 설정.
						{
							iAddCP *= 1.0f;
						}break;
					default:
						{
						}break;
					}
				}

				iAddCP *= 0.1f;

				reward_itr->iCP = __max( iAddCP, 0 );
			}

			if ( 0 < byRewardItemCount )
			{// 아이템 보상
				int const iLevel = pkPlayer->GetAbil( AT_LEVEL );
				PgItemBag kItemBag;

				GET_DEF(CItemBagMgr, kItemBagMgr);
				if ( S_OK == kItemBagMgr.GetItemBag( kPvPRewardItem.iItemBag[reward_itr->kWinLose % 3], static_cast<short>(iLevel), kItemBag ) )
				{
					while ( byRewardItemCount-- )
					{
						int iItemNo = 0;
						int iItemCount = 0;
						if ( SUCCEEDED(kItemBag.PopItem( iLevel, iItemNo, iItemCount )) )
						{
							reward_itr->kContItem.push_back( REWARD_ITEM_LIST::value_type(iItemNo, iItemCount) );
						}
					}

					if ( !reward_itr->kContItem.empty() )
					{
						PgAction_RewardItem kAction( CIE_PVP, GroundKey(), reward_itr->kContItem );
						kAction.DoAction( pkPlayer, NULL );
					}
				}
				else
				{
					CAUTION_LOG( BM::LOG_LV4, __FL__ << L"Not Found ItemBag<" << kPvPRewardItem.iItemBag << L"> RewardID<" << m_pkMode->GetRewardID() << L">" );
				}
			}	

			CONT_PLAYER_MODIFY_ORDER kOrder;
			/////////////////////////////////////////////////////////////////////
			// 실제 기록.
			SPMOD_SAbil kAddRecord( reward_itr->kWinLose, 1i64 );
			kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, reward_itr->kCharGuid, kAddRecord));
			//오더 보내야지. AddCP 를 보낼때 같이.

			int iAddPoint = 0;
			if ( !bIsExercise )
			{
				iAddPoint = reward_itr->GetTotalPoint();
				if ( iAddPoint )
				{
					SPMOD_SAbil kAddPoint( GAME_POINT, static_cast<__int64>(iAddPoint) );
					kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, reward_itr->kCharGuid, kAddPoint));
				}

				SPMOD_SAbil kAddKill( GAME_KILL, reward_itr->kResult.usKillCount );
				kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, reward_itr->kCharGuid, kAddKill));

				SPMOD_SAbil kAddDeath( GAME_DEATH, reward_itr->kResult.usDieCount );
				kOrder.push_back(SPMO(IMET_ADD_PVP_RANK_RECORD, reward_itr->kCharGuid, kAddDeath));
			}

			if ( reward_itr->iCP )
			{
				SPMOD_Add_CP kAddCP(reward_itr->iCP);
				kOrder.push_back(SPMO(IMET_ADD_CP, reward_itr->kCharGuid, kAddCP));
			}

			PgAction_ReqModifyItem kItemModifyAction(IMEPT_PVP, GroundKey(), kOrder, BM::Stream(), true);//랭킹 기록은 사망여부 상관없이.
			kItemModifyAction.DoAction( pkPlayer, NULL );

			kLog.Set( 0, static_cast<__int64>(reward_itr->kRank) );	// i64Value1 랭킹
			kLog.Set( 1, static_cast<__int64>(iAddPoint) );			// i64Value2 획득포인트
			kLog.Set( 2, static_cast<__int64>(reward_itr->iCP) );	// i64Value3 획득CP
			kLog.Set( 4, static_cast<__int64>(kType) );				// i64Value5 게임타입
			
			kLogCont.Add( kLog );
			kLogCont.Commit();

			++kRank;// 중요
		}
	}

	// Client한테 보내줄 패킷
	BM::Stream kCPacket( PT_M_C_NFY_GAME_RESULT, iWinTeam );
	PU::TWriteArray_M( kCPacket, kReward );
	Broadcast( kCPacket );

	BM::Stream kLobbyPacket( PT_M_T_NFY_GAME_RESULT, iWinTeam );
	m_pkMode->Send( kLobbyPacket );
}

void PgWarGround::RecvBreakLoveFence(BM::Stream & Packet)
{
	BM::CAutoMutex Lock( m_kRscMutex );

	int rdPos = Packet.RdPos();

	int ObjectNo = 0;
	Packet.Pop(ObjectNo);

	Packet.RdPos(rdPos);

	CONT_GTRIGGER::const_iterator iter = m_kContTrigger.begin();
	for( ; iter != m_kContTrigger.end() ; ++iter )
	{
		CONT_GTRIGGER::mapped_type const & Element = (*iter).second;

		if( GARIGGER_TYPE_LOVEFENCE == Element->GetType() )
		{
			PgGTrigger_LoveFence * pLoveFence = dynamic_cast<PgGTrigger_LoveFence *>(Element);
			if( pLoveFence )
			{
				if( ObjectNo == pLoveFence->GetFenceObjectNo() )
				{
					pLoveFence->Event(NULL, this, &Packet);
				}
			}
		}
	}
}