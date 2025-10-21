#include "stdafx.h"
#include "Variant/constant.h"
#include "Global.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgIndun.h"
#include "Variant/PgParty.h"
#include "Variant/PgPartyMgr.h"
#include "variant/PgEventview.h"
#include "BM/PgTask.h"
#include "PgPartyItemRule.h"
#include "PgLocalPartyMgr.h"
#include "PgStoneMgr.h"
#include "PgAction.h"
#include "PgConstellationMgr.h"
#include "Variant/PgQuestInfo.h"
#include "PgQuest.h"
#include "PgAction.h"
#include "PgActionQuest.h"

PgIndun::PgIndun()
:	m_dwStartTime(0)
,	m_eState(INDUN_STATE_NONE)
,	m_dwAutoStateRemainTime(0)
,	m_eOpening(E_OPENING_NONE)

// 특정 옵션
,	m_bUseGroundEffect(true)
,	m_bUseItem(true)
,	m_iResultMoveMapNum(0)
,	m_FailTime(0)
{
}

PgIndun::~PgIndun()
{

}

EOpeningState PgIndun::Init( int const iMonsterControlID, bool const bMonsterGen )
{
	m_kConstellationResult.OnInit(this);

	// 특정 옵션
	m_bUseGroundEffect = true;
	m_bUseItem = true;
	m_eOpening = PgGround::Init( iMonsterControlID, bMonsterGen );
	m_eState = INDUN_STATE_NONE;
	SetState( INDUN_STATE_OPEN );
	m_FailTime = 0;
	return m_eOpening;
}

void PgIndun::Clear()
{
	if ( !m_kOwnerGndInfo.kOwnerGndKey.IsEmpty() )
	{
		BM::Stream kNfyPacket( PT_M_M_NFY_HARDCORE_BOSS_ENDTIME );
		GroundKey().WriteToPacket( kNfyPacket );
		kNfyPacket.Push( 0i64 );
		::SendToGround( m_kOwnerGndInfo.kOwnerGndKey, kNfyPacket );
	}

	PgGround::Clear();
	StartTime( 0 );
	m_dwAutoStateRemainTime = 0;
	m_eState = INDUN_STATE_NONE;
	m_eOpening = E_OPENING_NONE;

	m_bUseGroundEffect = true;
	m_bUseItem = true;
	m_kOwnerGndInfo = SOwnerGroundInfo();
}

bool PgIndun::Clone( PgGround* pkGround )
{
	return this->Clone( (dynamic_cast<PgIndun*>(pkGround)) );
}

bool PgIndun::Clone( PgIndun* pkIndun )
{
	if ( pkIndun )
	{
		StartTime( pkIndun->StartTime() );
		m_dwAutoStateRemainTime = pkIndun->m_dwAutoStateRemainTime;
		m_eState = pkIndun->m_eState;
		m_eOpening = pkIndun->m_eOpening;
		m_bUseGroundEffect = pkIndun->m_bUseGroundEffect;
		m_bUseItem = pkIndun->m_bUseItem;
		m_kOwnerGndInfo = pkIndun->m_kOwnerGndInfo;
		return PgGround::Clone( dynamic_cast<PgGround*>(pkIndun) );
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgIndun::IsDeleteTime()const
{
	BM::CAutoMutex kLock(m_kRscMutex);
	switch ( GetState() )
	{
	case INDUN_STATE_OPEN:
		{

		}break;
	case INDUN_STATE_WAIT:
		{
			if ( m_kWaitUserList.empty() )
			{
				return ( 0 == PgObjectMgr::GetUnitCount(UT_PLAYER) );
			}
		}break;
	default:
		{
			return ( 0 == PgObjectMgr::GetUnitCount(UT_PLAYER) );
		}break;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

void PgIndun::UpdateAutoNextState(DWORD const dwNow, DWORD const dwElapsedTime)
{
	if ( 0 == m_dwAutoStateRemainTime )
	{
		return;
	}

	if ( dwElapsedTime >= m_dwAutoStateRemainTime )
	{
		EIndunState eState = (EIndunState)(m_eState << 1);
		switch( eState )
		{
		case INDUN_STATE_WAIT:
			{
				eState = INDUN_STATE_CLOSE;
			}break;
		case INDUN_STATE_READY:
			{
				for( ConWaitUser::const_iterator itr=m_kWaitUserList.begin(); itr!=m_kWaitUserList.end(); ++itr )
				{
					INFO_LOG(BM::LOG_LV5, __FL__<<L"[USER MAPLOADING]Wait OverTime["<<(ms_dwMaxWaitUserWaitngTime/1000)<<L" sec] : User["<<itr->kCharGuid<<L"]");
				}
			}break;
		}
		SetState( eState, true );
	}
	else
	{
		m_dwAutoStateRemainTime -= dwElapsedTime;
	}
}

bool PgIndun::RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder )
{
	if ( !PgGround::RecvMapMove( rkUnitArray, rkRMM, kContPetMapMoveData, kContUnitSummonedMapMoveData, kModifyOrder ) )
	{
		return false;
	}

	if ( INDUN_STATE_REGIST_RESULT_MGR & m_eState )
	{
		VEC_GUID kWaitList;
		VEC_JOININDUN kJoinIndunList;
		UNIT_PTR_ARRAY::iterator unit_itr;
		for ( unit_itr=rkUnitArray.begin(); unit_itr!=rkUnitArray.end(); ++unit_itr )
		{
			PgPlayer *pkUser = dynamic_cast<PgPlayer*>(unit_itr->pkUnit);
			if ( pkUser )
			{
				kWaitList.push_back(pkUser->GetID());
				kJoinIndunList.push_back( SNfyJoinIndun(pkUser->GetID(),pkUser->GetMemberGUID(),pkUser->GetAbil64(AT_EXPERIENCE)) );
				if(m_kOwnerGuid.IsNull())
				{
					m_kOwnerGuid = pkUser->GetID();
				}
			}
		}

		// Contents Server에게 통보
		switch( this->GetAttr() )
		{
		case GATTR_PVP:
			{
				VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__<<L"State Error!!");
				kWaitList.clear();
			}break;
		case GATTR_HARDCORE_DUNGEON_BOSS:
			{
				BM::Stream kMPacket( PT_M_M_REQ_READY_HARDCORE_BOSS );
				GroundKey().WriteToPacket( kMPacket );
				::SendToGround( rkRMM.kCasterKey, kMPacket );

				m_kOwnerGndInfo.kOwnerGndKey = rkRMM.kCasterKey;
			}// break을 사용하지 않고 다음에는 반드시 case GATTR_BOSS:
		case GATTR_BOSS:
		case GATTR_SUPER_GROUND_BOSS:
		case GATTR_ELEMENT_GROUND_BOSS:
			{
				BM::Stream kNPacket( PT_M_N_NFY_JOIN_INDUN_PLAYER, GroundKey() );
				kNPacket.Push( kJoinIndunList );
				::SendToResultMgr(kNPacket);
			}break;
		case GATTR_CHAOS_MISSION:
		case GATTR_MISSION:
			{
			}break;
		}

		this->InitWaitUser( kWaitList );
	}

	return true;
}

bool PgIndun::ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall, bool const bSendArea )
{
	BM::CAutoMutex Lock(m_kRscMutex);

	if ( UT_PLAYER == pkUnit->UnitType() )
	{
		m_kConstellationResult.OnReleaseUnit(pkUnit);

		if ( INDUN_STATE_WAIT == GetState() ) 
		{
			ReleaseWaitUser( dynamic_cast<PgPlayer*>(pkUnit) );
		}
		if( GetAttr() == GATTR_BOSS
		||	GetAttr() == GATTR_SUPER_GROUND_BOSS 
		||	GetAttr() == GATTR_ELEMENT_GROUND_BOSS )
		{
			BM::Stream kNPacket( PT_M_N_NFY_LEAVE_INDUN_PLAYER, GroundKey() );
			kNPacket.Push( pkUnit->GetID() );
			SendToResultMgr(kNPacket);
		}

		ChangeOwner(pkUnit);
	}

	return PgGround::ReleaseUnit( pkUnit, bRecursiveCall, bSendArea );
}

void PgIndun::RecvUnitDie(CUnit * pUnit)
{
	BM::CAutoMutex Lock(m_kRscMutex);

	PgGround::RecvUnitDie(pUnit);
}

bool PgIndun::ChangeOwner(CUnit * pkOwner)
{
	if(NULL == pkOwner)
	{
		return false;
	}

	BM::CAutoMutex Lock(m_kRscMutex);
	if( m_kOwnerGuid != pkOwner->GetID() )
	{
		return false;
	}

	CUnit * pkUser = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
	{
		if ( pkUser->GetID() != m_kOwnerGuid )
		{
			m_kOwnerGuid = pkUser->GetID();

			//Summon NPC
			PgSummoned * pkSummoned = NULL;
			VEC_SUMMONUNIT const kCont = pkOwner->GetSummonUnit();
			VEC_SUMMONUNIT::const_iterator npc_it = kCont.begin();
			while(npc_it != kCont.end())
			{
				pkSummoned = dynamic_cast<PgSummoned*>(GetUnit(npc_it->kGuid));
				if(pkSummoned && pkSummoned->IsNPC())
				{
					pkSummoned->Caller(pkUser->GetID());
					pkUser->AddSummonUnit(pkSummoned->GetID());
					pkOwner->DeleteSummonUnit(pkSummoned->GetID());
				}

				++npc_it;
			}
			return true;
		}
	}
	return false;
}

void PgIndun::MapLoadComplete()
{
	BM::CAutoMutex Lock( m_kRscMutex );

	CUnit* pkUser = NULL;
	CONT_OBJECT_MGR_UNIT::iterator unit_itr;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
	while(( pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
	{		
		this->SendMapLoadComplete( dynamic_cast<PgPlayer*>(pkUser) );
	}
}

void PgIndun::SendMapLoadComplete( PgPlayer *pkUser )
{
	BM::CAutoMutex Lock( m_kRscMutex );

	m_kConstellationResult.OnSendMapLoadComplete(pkUser);

	switch( m_eState )
	{
	case INDUN_STATE_WAIT:
		{
			ReleaseWaitUser( pkUser );
			if( pkUser->PartyGuid().IsNull() || m_kLocalPartyMgr.IsMaster(pkUser->PartyGuid(), pkUser->GetID()) )
			{
				m_kRecentInfo = pkUser->GetRecentMapInfo();
			}
		}break;
	case INDUN_STATE_READY:
		{
			PgGround::SendMapLoadComplete( pkUser );
			SendMonsterCount();
		}break;
	case INDUN_STATE_PLAY:
		{
			PgGround::SendMapLoadComplete( pkUser );
			SendMonsterCount();
			PgGround::PartyBreakIn_CousumeKeyItem( pkUser );
		}break;
	default:
		{
			// xxxxxx 다시 돌려 보내야 한다.
		}
	}
}

/*
void PgIndun::SendMapLoadState( const BM::GUID kCharGuid, unsigned char const ucPer )
{
	SNfyMapLoad kMapLoad( kCharGuid, ucPer );
	BM::Stream kBroadCast( PT_C_M_NFY_MAPLOAD_STATE, kMapLoad );
	Broadcast( kBroadCast, kCharGuid );
}
*/

void PgIndun::SendToParty_ConstellationMission()
{
	bool bBossMap = false;
	if( this->GetAttr() & (GATTR_FLAG_BOSS|GATTR_FLAG_MISSION) )
	{
		bBossMap = true;
	}

	BM::Stream Packet(PT_M_C_NFY_CONSTELLATION_MISSION);
	if( g_ConstellationPartyMgr.GetMissionInfo(GetConstellationKey().PartyGuid, bBossMap, Packet) )
	{
		SendToParty(GetConstellationKey().PartyGuid, Packet);
	}
}

void PgIndun::AllPlayerRegen(DWORD const dwSendFlag)
{
	PgPlayer *pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while( (pkPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		if ( pkPlayer )
		{
			pkPlayer->Alive(EALIVE_PVP,dwSendFlag);

			if( pkPlayer->HaveExpedition() )	// 원정대에 가입되어 있다면, 자신의 상태 변화를 알려야 함.
			{
				BM::Stream Packet(PT_M_N_NFY_EXPEDITION_MODIFY_MEMBER_STATE);
				Packet.Push(pkPlayer->ExpeditionGuid());
				Packet.Push(pkPlayer->GetID());
				Packet.Push(pkPlayer->IsAlive());
				::SendToGlobalPartyMgr(Packet);
			}
		}
	}
}

CUnit * PgIndun::GetOwnerUser()const
{
	return GetUser(m_kOwnerGuid);
}

CUnit * PgIndun::GetPartyMasterUser()const
{
	PgPlayer * pkUser = NULL;
	CONT_OBJECT_MGR_UNIT::const_iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkUser = dynamic_cast<PgPlayer*>(PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		if(m_kLocalPartyMgr.IsMaster( pkUser->PartyGuid(), pkUser->GetID() ))
		{
			return pkUser;
		}
		++kItor;
	}

	return NULL;
}

void PgIndun::SetState( EIndunState const eState, bool bAutoChange, bool bChangeOnlyState )
{
	BM::CAutoMutex kLock(m_kRscMutex);
	if ( eState == m_eState)
	{
		return;
	}

	m_dwAutoStateRemainTime = 0;
	m_eState = eState;

	if( bChangeOnlyState )
	{
		return;
	}

	PgWorldEventMgr::DoIndunState(GroundKey().GroundNo(), m_eState, GetOwnerUser());

	switch( m_eState )
	{
	case INDUN_STATE_OPEN:
		{
			SetAutoNextState(md_dwMaxOpenWaitingTime);//중요
		}break;
	case INDUN_STATE_WAIT:
		{
			SetAutoNextState(ms_dwMaxWaitUserWaitngTime);
		}break;
	case INDUN_STATE_READY:
		{
			GroundWeight( std::max<int>( static_cast<int>(PgObjectMgr::GetUnitCount(UT_PLAYER)) - 1, 0 ) );
			ActivateMonsterGenGroup( -1, false, false, m_iGroundWeight );
			ActivateMonsterGenGroup( m_iGroundWeight+1, false, false, 0 );
			PgGround::OnActivateEventMonsterGroup();

			MapLoadComplete();

			if ( !OpeningMovie() )
			{
				SetState( INDUN_STATE_PLAY );
			}

			if ( m_FailTime )
			{
				SetState( INDUN_STATE_CLOSE );
			}
		}break;
	case INDUN_STATE_PLAY:
		{
			StartTime( BM::GetTime32() );

			switch( GetAttr() )
			{
			case GATTR_HARDCORE_DUNGEON_BOSS:
				{
					__int64 const i64NowTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
					__int64 const i64RemainTime = m_kOwnerGndInfo.i64EndTime - i64NowTime;
					if ( 0i64 < i64RemainTime )
					{
						__int64 i64PlayingTime = 30i64 * CGameTime::MINUTE;
						if ( i64RemainTime < i64PlayingTime )
						{
							i64PlayingTime = i64RemainTime;
						}

						DWORD const dwEndRemainTime = static_cast<DWORD>(i64PlayingTime / CGameTime::MILLISECOND);
						__int64 const i64EndTime = i64NowTime + i64PlayingTime;

						BM::Stream kNfyPacket( PT_M_M_NFY_HARDCORE_BOSS_ENDTIME );
						GroundKey().WriteToPacket( kNfyPacket );
						kNfyPacket.Push( i64EndTime );
						::SendToGround( m_kOwnerGndInfo.kOwnerGndKey, kNfyPacket );

						BM::Stream kCPacket( PT_M_C_NFY_HARDCORE_BOSS_ENDTIME_IN, i64EndTime );
						Broadcast( kCPacket );

						SetAutoNextState( dwEndRemainTime );
					}
					else
					{
						// 종료되어야 한다.
						return SetState( INDUN_STATE_RESULT_WAIT );
					}
				}break;
			}

			SendMonsterCount();
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = EFFECT_TYPE_PENALTY;
			kCreate.iEffectNum = EFFECTNO_CANNOT_DAMAGE;
			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;

			CUnit* pkUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			while ((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
			{
				if( false == pkUser->IsDead() )
				{
					pkUser->SetAbil( AT_HP, pkUser->GetAbil(AT_C_MAX_HP), true );
					pkUser->SetAbil( AT_MP, pkUser->GetAbil(AT_C_MAX_MP), true );
					pkUser->AddEffect( kCreate);//무적 이펙트를 걸어줌
				}
			}

			SetAutoNextState(GetResultWaitTime());
		}break;
	case INDUN_STATE_RESULT:
		{
			StartTime( BM::GetTime32() );
			{
				CUnit* pkUser = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
				{
					PgAction_GroundQuestCheck kGroundQuestAction(GetGroundNo(), GroundKey(), 0, (unsigned int)m_eState);
					kGroundQuestAction.DoAction(pkUser, pkUser/*Target self*/);
				}
			}
			switch( GetAttr() )
			{
			case GATTR_HARDCORE_DUNGEON_BOSS:
				{
					if ( !m_kOwnerGndInfo.kOwnerGndKey.IsEmpty() )
					{
						BM::Stream kNfyPacket( PT_M_M_NFY_HARDCORE_BOSS_ENDTIME );
						GroundKey().WriteToPacket( kNfyPacket );
						kNfyPacket.Push( 0i64 );
						::SendToGround( m_kOwnerGndInfo.kOwnerGndKey, kNfyPacket );

						m_kOwnerGndInfo = SOwnerGroundInfo();// 초기화
					}
				} // braek 사용안함 다음에 반드시 case GATTR_BOSS:
			case GATTR_BOSS:
			case GATTR_SUPER_GROUND_BOSS:
			case GATTR_ELEMENT_GROUND_BOSS:
				{
					size_t iPlayUserCount = 0;
					VEC_RESULTINDUN kResultList;
					CUnit* pkUser = NULL;
					CONT_OBJECT_MGR_UNIT::iterator kItor;
					PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
					while ((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, kItor)) != NULL)
					{
						if( false == pkUser->IsDead() )
						{
							kResultList.push_back( SNfyResultIndun(pkUser->GetID(), pkUser->GetAbil64(AT_EXPERIENCE), pkUser->GetAbil(AT_LEVEL)) );
						}
						
						// 통계
						++iPlayUserCount;
					}

					BM::Stream kNPacket( PT_M_N_NFY_RESULT_INDUN, GroundKey() );
					kNPacket.Push( kResultList );
					SendToResultMgr(kNPacket);

					BM::Stream kWebPacket(PT_A_N_NFY_GROUND_PLAYER_COUNT_INFO);
					kWebPacket.Push( GetGroundNo() );
					kWebPacket.Push( iPlayUserCount );
					::SendToRealmContents(PMET_WEB_HELPER, kWebPacket);
				}
			default:
				{
					RemoveAllMonster();
				}break;
			}

		}break;
	case INDUN_STATE_FAIL:
		{
			if( GetConstellationKey().PartyGuid.IsNotNull() )
			{
				BM::Stream Packet(PT_M_C_NFY_CONSTELLATION_MISSION_FAIL);
				SendToParty(GetConstellationKey().PartyGuid, Packet);

				Set_LockPlayerInGround();
				StopAI();
			}
		}break;
	}
}

void PgIndun::OnTick1s()
{
	BM::CAutoMutex Lock( m_kRscMutex );
	DWORD const dwNow = BM::GetTime32();
	DWORD dwElapsed = 0;
	bool bTimeUpdate = true;
	if( INDUN_STATE_PLAY == m_eState )
	{
		bTimeUpdate = false;
	}
	CheckTickAvailable(ETICK_INTERVAL_1S, dwNow, dwElapsed, bTimeUpdate);

	switch( m_eState )
	{
	case INDUN_STATE_WAIT:
		{
			if ( GATTR_HARDCORE_DUNGEON_BOSS == this->GetAttr() )
			{
				if ( 0i64 == m_kOwnerGndInfo.i64EndTime )
				{
					break;
				}
			}

			if ( IsAllUserSameStep(m_eOpening) )
			{
				if ( E_OPENING_READY == m_eOpening )
				{	
					m_eOpening = E_OPENING_PLAY;

					// Opening할 놈들을 찾아야 한다.
					UNIT_PTR_ARRAY kUnitArray;				
					if ( !GetHaveAbilUnitArray( kUnitArray, AT_MANUAL_OPENING, UT_MONSTER ) )
					{
						LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("GetHaveAbilUnitArray Failed!"));
						INFO_LOG(BM::LOG_LV4, __FL__<<L"Not Found Opening Unit / GroundNo["<<GetGroundNo()<<L"]" );
						m_eOpening = E_OPENING_NONE;
						SetState( INDUN_STATE_READY );
					}
					else
					{
 						CUnit* pkUser = NULL;
 						CONT_OBJECT_MGR_UNIT::iterator unit_itr;
						PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
 						while(( pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
 						{
							PgGroundUtil::SendAddSimpleUnit(pkUser, kUnitArray, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);
							ReleaseWaitUser( dynamic_cast<PgPlayer*>(pkUser) );
						}

//						SetUnitAbil(kUnitArray, AT_HPMP_BROADCAST, 1);
					}

					ClearManualOpeningMonster(kUnitArray);
				}
				else
				{
					SetState( INDUN_STATE_READY );
				}
			}
		}break;
	case INDUN_STATE_READY:
		{
			if ( !OpeningMovie() )
			{
				SetState( INDUN_STATE_PLAY );
			}
		}break;
	case INDUN_STATE_PLAY:
		{
			PgGround::OnTick1s();
		}break;
	case INDUN_STATE_RESULT_WAIT:
		{

		}break;
	case INDUN_STATE_RESULT:
		{
			PgPlayer* pkPlayer = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
			{
				if (pkPlayer->IsDead())
				{
					if ( DifftimeGetTime( pkPlayer->DeathTime(), dwNow ) >= this->GetDeathDelayTime() )
					{
						BM::Stream kNfyPacket(PT_C_M_NFY_RECENT_MAP_MOVE);
						pkPlayer->VNotify(&kNfyPacket);
					}
				}
			}
		}break;
	case INDUN_STATE_CLOSE:
		{
			if( 0 != GetResultMoveMapNum())
			{
				SReqMapMove_MT kRMM(MMET_None);
				kRMM.kTargetKey.GroundNo(GetResultMoveMapNum());
				kRMM.nTargetPortal = 1;
				PgReqMapMove kMapMove( this, kRMM, NULL );

				bool bRun = false;
				PgPlayer* pkPlayer = NULL;
				CONT_OBJECT_MGR_UNIT::iterator kItor;
				PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
				while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
				{
					if ( kMapMove.Add( pkPlayer ) )
					{
						bRun = true;
					}
				}

				if( bRun )
				{
					kMapMove.DoAction();
				}
			}
			else
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

	UpdateAutoNextState( dwNow, dwElapsed );
}

void PgIndun::OnTick5s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	DWORD const dwNow = BM::GetTime32();
	DWORD dwkElapsed = 0;
	CheckTickAvailable(ETICK_INTERVAL_5S, dwNow, dwkElapsed, true);

	if( !(m_eState & (INDUN_STATE_RESULT|INDUN_STATE_RESULT_WAIT)) && BM::TimeCheck(m_kLastGenCheckTime, 6000))
	{
		RareMonsterGenerate();
		MonsterGenerate(m_kContGenPoint_Monster);
		ObjectUnitGenerate(m_kContGenPoint_Object);
	}

	DWORD const dwServerElapsedTime = g_kEventView.GetServerElapsedTime();

	PgPlayer* pkPlayer = NULL;
	CONT_OBJECT_MGR_UNIT::iterator kItor;
	PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
	while ((pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor))) != NULL)
	{
		pkPlayer->Update(dwNow);

		if ( !pkPlayer->IsMapLoading() )
		{
			OnTick_AlramMission( pkPlayer, dwServerElapsedTime );
		}
	}
}

HRESULT PgIndun::SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont )
{
	if ( INDUN_STATE_PLAY != m_eState )
	{// 그냥 S_OK로 리턴~
		return S_OK;
	}

	//Item & Gold
	switch( pkDroper->GetAbil(AT_GRADE) )
	{
	case EMGRADE_BOSS:
		{
			SNfyResultItemList kResultItemList( pkDroper->GetAbil(AT_MAX_DROP_ITEM_COUNT) );

			CONT_DEF_MAP_ITEM_BAG const *pkContMapItemBag = NULL;
			g_kTblDataMgr.GetContDef(pkContMapItemBag);

			CONT_DEF_MAP_ITEM_BAG::const_iterator map_item_bag_Itr = pkContMapItemBag->find( GetMapItemBagGroundNo() );
			
			int iMax_IndunParticipationRate = 0;
			CUnit *pkUser = NULL;
			CONT_OBJECT_MGR_UNIT::iterator unit_itr;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, unit_itr);
			while((pkUser = PgObjectMgr::GetNextUnit(UT_PLAYER, unit_itr)) != NULL)
			{
				SNfyResultItem kUserItem( pkUser->GetID() );

				if( map_item_bag_Itr != pkContMapItemBag->end() )
				{
					PgPlayer *pkPlayer = dynamic_cast<PgPlayer*>(pkUser);
					if( pkPlayer && true == pkPlayer->IsTakeUpItem() )
					{
						kUserItem.fParticipationRate = pkPlayer->GetAbil(AT_INDUN_PARTICIPATION_RATE);
						iMax_IndunParticipationRate = std::max(static_cast<int>(kUserItem.fParticipationRate), iMax_IndunParticipationRate);
						PgAction_DropItemBox kDropItem( this, map_item_bag_Itr->second );
						if ( true == kDropItem.DoAction( pkDroper, pkUser ) )
						{
							CONT_MONSTER_DROP_ITEM::const_iterator bag_itr = kDropItem.m_kContDropItem.begin();
							for( ; bag_itr != kDropItem.m_kContDropItem.end() ; ++bag_itr )
							{
								if( bag_itr->ItemNo() )
								{
									kUserItem.kItemList.push_back( *bag_itr );
								}
							}
						}
					}
					CommitIndunEndLog(pkPlayer);
				}

				kResultItemList.push_back( kUserItem );
			}

			kResultItemList.CalcParticipationRate(iMax_IndunParticipationRate);
			SendResultItem( kResultItemList );
			SetState(INDUN_STATE_RESULT_WAIT);
		}break;
	case EMGRADE_ELITE:
		{
			int const iMoveMapNum = pkDroper->GetAbil(AT_MON_RESULT_MOVE_MAP_NUM);
			if( 0 != iMoveMapNum )
			{
				SetResultMoveMapNum(iMoveMapNum);
				SetState(INDUN_STATE_RESULT, false, true);
				int iMoveMapTime = pkDroper->GetAbil(AT_MON_RESULT_MOVE_MAP_TIME);
				if( iMoveMapTime <= 0 )// 시간이 없으면 안된다, 디폴트 1초로.
				{
					iMoveMapTime = 1000;
				}
				SetAutoNextState(iMoveMapTime);
			}

			HRESULT kRet = PgGround::SetUnitDropItem(pkOwner, pkDroper, kLogCont );// 아이템 드랍은 해야지.
			int const iKill = pkDroper->GetAbil(AT_MON_RESULT_KILL_ALL);
			if((GetAttr()==GATTR_BOSS && iMoveMapNum) || iKill)
			{
				return S_FALSE;//가고일 같은 경우 미션맵은 아니지만 몹들을 다 죽여줘야 한다
			}
			return kRet;

		}break;
	default:
		{
			return PgGround::SetUnitDropItem(pkOwner, pkDroper, kLogCont );
		}break;
	}
	return S_OK;
}

void PgIndun::SendResultItem( SNfyResultItemList& rkResultItem )
{
	BM::Stream kNPacket( PT_M_N_NFY_RESULTITEM_INDUN, GroundKey() );
	rkResultItem.WriteFromPacket( kNPacket );
	SendToResultMgr( kNPacket );
}


void PgIndun::ClearManualOpeningMonster(UNIT_PTR_ARRAY& rkUnitArray)
{
	UNIT_PTR_ARRAY::iterator unit_itr;
	for( unit_itr=rkUnitArray.begin(); unit_itr!=rkUnitArray.end(); ++unit_itr )
	{
		if ( unit_itr->pkUnit )
		{
			unit_itr->pkUnit->ActionID(0);
		}
	}
}

void PgIndun::SetUnitAbil(UNIT_PTR_ARRAY &rkUnitArray, WORD const wType, int const iValue)
{
	UNIT_PTR_ARRAY::iterator unit_itr;
	for( unit_itr=rkUnitArray.begin(); unit_itr!=rkUnitArray.end(); ++unit_itr )
	{
		if ( unit_itr->pkUnit )
		{
			unit_itr->pkUnit->SetAbil(wType, iValue);
		}
	}
}

DWORD PgIndun::GetResultWaitTime()
{	
	int const iTime = GetMapAbil(AT_RESULT_WAIT_TIME); 
	return 0<iTime ? iTime : 5000;
}

bool PgIndun::RecvRecentMapMove( PgPlayer *pkUser )
{
	if(NULL == pkUser)
	{
		return false;
	}

	BM::CAutoMutex kLock(m_kRscMutex);
	UpdateIndunRecentMap(pkUser);
	
	switch( m_eState )
	{
	case INDUN_STATE_READY:
	case INDUN_STATE_PLAY:
		{
			BM::GUID const& kPartyGuid = pkUser->GetPartyGuid();
			BM::GUID const& kCharGuid = pkUser->GetID();
			if( BM::GUID::IsNotNull(kPartyGuid) )
			{
				size_t const iMinSize = 1;
				VEC_GUID kContGuid;
				m_kLocalPartyMgr.GetPartyMemberGround(kPartyGuid, GroundKey(), kContGuid, kCharGuid);
				if( m_kLocalPartyMgr.IsMaster(kPartyGuid, kCharGuid)
				&&	iMinSize <= kContGuid.size() )
				{
					// 오너를 교체해야 한다.
					BM::Stream kNPacket(PT_C_N_REQ_PARTY_CHANGE_MASTER, kCharGuid);
					kNPacket.Push( (*kContGuid.begin()) );
					::SendToGlobalPartyMgr(kNPacket);
				}
			}
		}break;
	case INDUN_STATE_RESULT:
		{
			BM::GUID const& PartyGuid = pkUser->GetPartyGuid();
			BM::GUID const& CharGuid = pkUser->GetID();
			if( BM::GUID::IsNotNull(PartyGuid) && m_kLocalPartyMgr.IsMaster(PartyGuid, CharGuid) )
			{// 결산 페이즈에서 인던 밖으로 맵이동을 하려는 유저가 파티장이면
				VEC_GUID MemberGuid;
				BM::GUID NewMasterGuid;
				m_kLocalPartyMgr.GetPartyMemberGround(PartyGuid, GroundKey(), MemberGuid, CharGuid);
				
				VEC_GUID::const_iterator iter = MemberGuid.begin();
				while( MemberGuid.end() != iter )
				{// 현재 그라운드에 살아있는 유저에게 파티장을 넘긴다.
					CUnit * pUnit = GetUnit(*iter);
					if( pUnit && pUnit->IsAlive() )
					{
						NewMasterGuid = *iter;
						break;
					}
					++iter;
				}
				if( NewMasterGuid.IsNotNull() )
				{// 오너를 교체해야 한다.
					BM::Stream NPacket(PT_C_N_REQ_PARTY_CHANGE_MASTER, CharGuid);
					NPacket.Push( NewMasterGuid );
					::SendToGlobalPartyMgr(NPacket);
				}
			}
		}break;
	}

	return PgGround::RecvRecentMapMove(pkUser);
}

bool PgIndun::UpdateIndunRecentMap(PgPlayer * pkUser)
{
	if(NULL==pkUser)
	{
		return false;
	}
	
	if(GetRecentInfo().IsEmpty())
	{
		return false;
	}

	if(GetRecentInfo().iMapNo == pkUser->GetRecentMapNo(GATTR_DEFAULT))
	{
		return false;
	}

	switch( GetState() )
	{
	case INDUN_STATE_READY:
	case INDUN_STATE_PLAY:
		{
			if(pkUser->IsAlive())
			{
				return false;
			}
		}break;
	}

	pkUser->SetRecentMapInfo(GetRecentInfo());
	return true;
}

void PgIndun::CommitIndunEndLog(PgPlayer const *player) const
{
	if(NULL==player)
	{
		return;
	}

	PgLogCont kLogCont(ELogMain_Contents_MIssion, ELogSub_Mission_End);				
	kLogCont.MemberKey(player->GetMemberGUID());
	kLogCont.CharacterKey(player->GetID());
	kLogCont.ID(player->MemberID());
	kLogCont.UID(player->UID());
	kLogCont.Name(player->Name());
	kLogCont.ChannelNo( player->GetChannel() );
	kLogCont.Class(static_cast<short>(player->GetAbil(AT_CLASS)));
	kLogCont.Level(static_cast<short>(player->GetAbil(AT_LEVEL)));
	kLogCont.GroundNo( GetGroundNo() );
	//kLogCont.ChannelType( player->ChannelType() );

	PgLog kLog(ELOrderMain_Mission, ELOrderSub_End);
	kLog.Set( 0, static_cast<int>(1));// 종료사유 정상
	kLog.Set( 1, static_cast<int>(m_kLocalPartyMgr.GetMemberCount(player->PartyGuid())) );
	kLog.Set( 2, static_cast<int>(0));//랭크포인트 대신
	kLog.Set( 3, static_cast<int>(0) );//인던은 레벨이 없다
	kLog.Set( 0, player->GetAbil64(AT_EXPERIENCE) );// i64Value1
	kLog.Set( 2, player->PartyGuid().str() );	// guidValue 1
	kLog.Set( 3, this->GroundKey().Guid().str() );// guidValue 2
	kLog.Set( 0, std::wstring(L"0") );
	kLogCont.Add(kLog);
	kLogCont.Commit();
}

EOpeningState PgIndun::SummonNPC_Generate(PgGenPoint_Object& rkGenPoint, int const iLevel)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	// CanGen() 함수는 미리 검사 해야 한다 (아니면 강제 리젠이거나)

	PgGenPoint_Object::OwnGroupCont kCont;
	rkGenPoint.GetBaseInfo( kCont );
	CUnit * pkCaller = GetPartyMasterUser();
	if(NULL == pkCaller || pkCaller->IsDead())
	{
		pkCaller = GetOwnerUser();
	}
	return SummonNPC_Generate(rkGenPoint, kCont, iLevel, pkCaller);
}

EOpeningState PgIndun::SummonNPC_Generate(PgGenPoint_Object& rkGenPoint, PgGenPoint_Object::OwnGroupCont const& kCont, int const iLevel, CUnit * pkCaller)
{
	BM::CAutoMutex kLock(m_kRscMutex);
	// CanGen() 함수는 미리 검사 해야 한다 (아니면 강제 리젠이거나)

	GET_DEF(PgClassDefMgr, kClassDefMgr);

	EOpeningState eState = E_OPENING_NONE;
	for(PgGenPoint_Object::OwnGroupCont::const_iterator info_itr = kCont.begin(); info_itr != kCont.end(); ++info_itr)
	{
		SClassKey const kKey((*info_itr).iID, iLevel);

		SCreateSummoned kCreateInfo;
		kCreateInfo.kClassKey.iClass = kKey.iClass;
		kCreateInfo.kClassKey.nLv = kKey.nLv;
		kCreateInfo.bNPC = true;
		//kCreateInfo.bUniqueClass = kClassDefMgr.GetAbil(kKey, AT_CREATE_UNIQUE_SUMMONED)>0;
		//kCreateInfo.sOverlapClass = static_cast<short>(kClassDefMgr.GetAbil(kKey, AT_CREATE_SUMMONED_MAX_COUNT));
		//kCreateInfo.sNeedSupply = static_cast<short>(kClassDefMgr.GetAbil(kKey, AT_CREATE_SUMMONED_SUPPLY));
		
		kCreateInfo.kGuid.Generate();			// GUID 생성
		

		CUnit * pkSummonNpc = CreateSummoned(pkCaller, &kCreateInfo, L"SummonedNPC", SSummonedMapMoveData::NullData(), (*info_itr).pt3Pos);
		if ( pkSummonNpc )
		{
			rkGenPoint.AddChild( pkSummonNpc->GetID() );
			//PgWorldEventMgr::IncObjectGenPointCount(rkGenPoint.Info().kPosGuid);
			//PgWorldEventMgr::IncObjectGenPointGroupCount(rkGenPoint.Info().iPointGroup);

			//if( E_OPENING_NONE != eState )
			//{
			//	PgWorldEventMgr::DoObjectRegenPoint(GetGroundNo(), rkGenPoint.Info().kPosGuid, rkGenPoint.Info().iPointGroup); // 월드 이벤트 검사
			//}
		}
	}
	return eState;
}