#include "stdafx.h"
#include "PgWarGround.h"
#include "PgPvPType_Love.h"

PgPvPType_Love::PgPvPType_Love()
{
}

PgPvPType_Love::~PgPvPType_Love()
{
}

void PgPvPType_Love::ReadyGame(PgWarGround * pGnd)
{
	m_kOnRedBear = true;
	m_kOnBlueBear = true;

	m_kOnRedBearRegen = false;
	m_kOnBlueBearRegen = false;

	m_kRedBearDropAccumTime = 0;
	m_kBlueBearDropAccumTime = 0;
}

bool PgPvPType_Love::Start(PgPlayer * pPlayer, PgWarGround * pGnd)
{
	if( !pPlayer || !pGnd )
	{
		return false;
	}

	m_kOnRedBear = true;
	m_kOnBlueBear = true;

	m_kOnRedBearRegen = false;
	m_kOnBlueBearRegen = false;

	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.find(pPlayer->GetID());
	if ( itr != m_kContPlay.end() )
	{
		int const iTeam = itr->second.kTeamSlot.GetTeam();
		pPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
		pPlayer->SetAbil( AT_TEAM, iTeam, true, true );
		pGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pPlayer), 0, true, iTeam );

		pPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
	}

	m_kRedBearDropAccumTime = 0;
	m_kBlueBearDropAccumTime = 0;

	return true;
}

bool PgPvPType_Love::End(bool const bAutoChange, PgWarGround * pGnd)
{
	m_bEndGame = m_bEndGame || m_bNoEnemy;
	return m_bEndGame;
}

bool PgPvPType_Love::Exit(BM::GUID const & Guid)
{
	CONT_PVP_GAME_USER::iterator user_itr = m_kContPlay.find(Guid);
	if( user_itr != m_kContPlay.end() )
	{		
		m_kContPlay.erase(user_itr);
		RefreshUserCount();
		return true;
	}

	m_kContReady.erase(Guid);

	return true;
}

EResultEndType PgPvPType_Love::ResultEnd()
{
	return PgPvPType_DeathMatch::ResultEnd();
}

HRESULT PgPvPType_Love::Init(BM::Stream & Packet, PgWarGround * pGnd)
{
	m_kRedBearNo = 0;
	m_kBlueBearNo = 0;

	m_kOnRedBear = true;
	m_kOnBlueBear = true;

	m_kRedBearDropAccumTime = 0;
	m_kBlueBearDropAccumTime = 0;

	m_kPrevRoundWinTeam = TEAM_NONE;

	m_pRedScoreTrigger = NULL;
	m_pBlueScoreTrigger = NULL;

	if( NULL == pGnd )
	{
		return E_FAIL;
	}

	if( SUCCEEDED(PgPvPType_DeathMatch::Init(Packet, pGnd)) )
	{
		m_pGnd = pGnd;

		return S_OK;
	}

	VERIFY_INFO_LOG(false, BM::LOG_LV4, __FL__ << _T("Error"));
	return E_FAIL;
}

HRESULT PgPvPType_Love::Update(DWORD const dwCurTime)
{
	if( SUCCEEDED( PgPvPType_DeathMatch::Update(dwCurTime)) )
	{
		return S_OK;
	}

	return E_FAIL;
}

DWORD PgPvPType_Love::GetTime(EPvPTime const Type) const
{
	switch ( Type )
	{
	case PVP_TIME_READY:
		{
			return 5000;
		}break;
	}

	return PgWarMode::GetTime( Type );
}

HRESULT PgPvPType_Love::SetEvent_Kill(CUnit * pUnit, CUnit * pKiller, PgWarGround * pGnd)
{
	if( pUnit && pKiller && pGnd )
	{	// 죽을 때 곰 디버프가 있으면 곰 아이템 생성 시킴.

		if( UT_OBJECT == pUnit->UnitType() )
		{// 죽은게 파괴 오브젝트(곰 우리 입구)라면, 해당 트리거로 알림.
			PgObjectUnit * pObjectUnit = dynamic_cast<PgObjectUnit *>(pUnit);
			if( pObjectUnit )
			{
				int ObjectNo = pObjectUnit->GetAbil(AT_CLASS);
				BM::Stream Packet(PT_M_M_NFY_BREAK_LOVE_FENCE);
				Packet.Push(ObjectNo);
				Packet.Push(false);
				VNotify( &Packet );

				if( pKiller )
				{
					BM::Stream Packet(PT_M_C_NFY_LOVEMODE_MESSAGE);
					Packet.Push(74107);
					Packet.Push(false);
					Packet.Push(pKiller->GetAbil(AT_TEAM));
					Packet.Push(pKiller->Name());
					m_pGnd->Broadcast(Packet);
					SendLoveGamePoint(pKiller->GetID(), PVP_ST_DESTROYFENCE);

					return S_OK;
				}
			}
		}

		if( UT_PLAYER == pUnit->UnitType() )
		{
			pUnit->DeathTime(BM::GetTime32());

			int KillPoint = 0;
			PgUnitEffectMgr const & EffectMgr = pUnit->GetEffectMgr();
			if( EffectMgr.GetAbil(AT_BEAR_EFFECT_RED) || EffectMgr.GetAbil(AT_BEAR_EFFECT_BLUE) )
			{
				CheckPlayerHaveBear(pUnit);

				static WORD BearKillPoint = ms_kSetting.Get(Type(), PVP_ST_BEARUSERKILL);
				KillPoint = BearKillPoint;

				CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pKiller->GetID());
				if ( itr != m_kContPlay.end() )
				{
					itr->second.kResult.Kill(KillPoint);
				}
			}
			else 
			{
				static WORD usPoint = ms_kSetting.Get(Type(), PVP_ST_KILL_POINT);
				KillPoint = usPoint;

				CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pKiller->GetID());
				if ( itr != m_kContPlay.end() )
				{
					itr->second.kResult.Kill(KillPoint);
				}
			}

			BM::Stream Packet(PT_M_C_NFY_GAME_EVENT_KILL);
			Packet.Push(pUnit->GetID());
			Packet.Push(0);
			Packet.Push(pKiller->GetID());
			Packet.Push(KillPoint);
			pGnd->Broadcast( Packet );
		}
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPType_Love::SetRoundWinTeam(int WinTeam, PgWarGround * pGnd)
{
	m_kPrevRoundWinTeam = WinTeam;
	CONT_PVP_TEAM_RESULT_INFO::iterator win_iter = m_kContTeamResult.find(WinTeam);
	if( win_iter != m_kContTeamResult.end() )
	{
		if( ++(win_iter->second.ucWinCount) > (m_ucMaxRound / 2) )
		{
			m_bEndGame = true;
		}

		BM::Stream EndPacket( PT_M_C_NFY_GAME_END, false );
		EndPacket.Push( WinTeam );
		EndPacket.Push( m_bEndGame );
		pGnd->Broadcast( EndPacket );
	}
}

void PgPvPType_Love::BearTouchDown(CUnit * pUnit, PgWarGround * pGnd)
{	
	if( !pUnit || !pGnd )
	{
		return;
	}

	// 곰 이펙트가 있는지 검사.
	PgUnitEffectMgr const & EffectMgr = pUnit->GetEffectMgr();
	if( !EffectMgr.GetAbil(AT_BEAR_EFFECT_RED) && !EffectMgr.GetAbil(AT_BEAR_EFFECT_BLUE) )
	{
		return ;
	}

	int WinTeam = pUnit->GetAbil(AT_TEAM);
	m_kPrevRoundWinTeam = WinTeam;
	CONT_PVP_TEAM_RESULT_INFO::iterator win_iter = m_kContTeamResult.find(WinTeam);
	if( win_iter != m_kContTeamResult.end() )
	{
		SendLoveGamePoint(pUnit->GetID(), PVP_ST_GETROUNDSCORE);

		// 해당 플레이어의 팀 승리 카운트를 하나 증가시키고.
		if( ++(win_iter->second.ucWinCount) > (m_ucMaxRound / 2) )
		{
			m_bEndGame = true;
		}

		// 해당 유닛에 걸려있는 모든 이펙트 제거.
		pUnit->ClearAllEffect();

		BM::Stream EndPacket( PT_M_C_NFY_GAME_END, false );
		EndPacket.Push( WinTeam );
		EndPacket.Push( m_bEndGame );
		pGnd->Broadcast( EndPacket );

		// 스코어를 획득한 캐릭터에게 카메라 맞춤.
		BM::Stream CameraPacket(PT_M_C_NFY_FOCUS_TOUCH_DOWN_USER, true);
		CameraPacket.Push( pUnit->GetID() );
		pGnd->Broadcast( CameraPacket );
	}
}

void PgPvPType_Love::BearFenceReset(PgObjectUnit * pObjUnit)
{
	if( !pObjUnit )
	{
		return;
	}

	// 라운드가 종료되어 곰 우리 입구 오브젝트가 재생성 됐음을 트리거로 알림.
	int ObjectNo = pObjUnit->GetAbil(AT_CLASS);
	BM::Stream Packet(PT_M_M_NFY_BREAK_LOVE_FENCE);
	Packet.Push(ObjectNo);
	Packet.Push(true);
	VNotify( &Packet );
}

void PgPvPType_Love::SetRedScoreTrigger(PgGTrigger_Score * pTrigger)
{
	m_pRedScoreTrigger = pTrigger;
}

void PgPvPType_Love::SetBlueScoreTrigger(PgGTrigger_Score * pTrigger)
{
	m_pBlueScoreTrigger = pTrigger;
}

PgGTrigger_Score * const PgPvPType_Love::GetRedScoreTrigger() const
{
	return m_pRedScoreTrigger;
}

PgGTrigger_Score * const PgPvPType_Love::GetBlueScoreTrigger() const
{
	return m_pBlueScoreTrigger;
}

void PgPvPType_Love::SendLoveGamePoint(BM::GUID const & CharGuid, EPvPSetting SettingType)
{
	BM::Stream Packet(PT_M_C_NFY_GAME_EVENT_CHANGEPOINT);

	CONT_SEND_PVP_POINT SendContainer;
	CONT_PVP_GAME_USER::iterator user_iter = m_kContPlay.find(CharGuid);
	if( user_iter == m_kContPlay.end() )
	{
		return;
	}

	user_iter->second.kResult.AddPoint( ms_kSetting.Get(Type(), SettingType) );

	SendContainer.insert( std::make_pair(user_iter->first, user_iter->second.kResult) );

	PU::TWriteTable_AM(Packet, SendContainer);
	m_pGnd->Broadcast(Packet);
}

void PgPvPType_Love::StartGame(BM::Stream & Packet, bool const bGame)
{
	PgPvPType_DeathMatch::StartGame(Packet, bGame);

	CONT_PVP_TEAM_RESULT_INFO::iterator iter = m_kContTeamResult.find(m_kPrevRoundWinTeam);
	if( iter != m_kContTeamResult.end() )
	{
		if( iter->second.ucWinCount == (m_ucMaxRound / 2) )
		{
			BM::Stream NfyPacket(PT_M_C_NFY_LOVEMODE_MESSAGE);
			NfyPacket.Push(74116);
			NfyPacket.Push(true);
			NfyPacket.Push(m_kPrevRoundWinTeam);

			m_pGnd->Broadcast(NfyPacket);
		}
	}

	if( m_ucNowRound == m_ucMaxRound )
	{
		BM::Stream NfyPacket(PT_M_C_NFY_LOVEMODE_MESSAGE);
		NfyPacket.Push(74115);
		NfyPacket.Push(true);
		NfyPacket.Push(0);

		m_pGnd->Broadcast(NfyPacket);
	}
}

void PgPvPType_Love::CheckPlayerHaveBear(CUnit * pUnit)
{
	VEC_GUID Onwers;
	int bearNo = 0;
	ETeam CheckPlayerTeam = static_cast<ETeam>(pUnit->GetAbil(AT_TEAM));
	if( TEAM_RED == CheckPlayerTeam )
	{
		bearNo = BlueBearNo();
	}
	else
	{
		bearNo = RedBearNo();
	}

	PgBase_Item BearItem;
	if( SUCCEEDED(::CreateSItem(bearNo, 1, GIOT_FIELD, BearItem)) )
	{
		WORD RemainTime = 0;
		if( TEAM_RED == CheckPlayerTeam )
		{
			RemainTime = DROPBEAR_ITEM_DELTIME - m_kRedBearDropAccumTime;
		}
		else if( TEAM_BLUE == CheckPlayerTeam )
		{
			RemainTime = DROPBEAR_ITEM_DELTIME - m_kBlueBearDropAccumTime;
		}

		PgLogCont LogCont;
		BM::GUID DropBoxGuid;
		m_pGnd->InsertItemBox(pUnit->GetPos(), Onwers, pUnit, BearItem, 0i64, LogCont, CheckPlayerTeam, DropBoxGuid, RemainTime);
		m_pGnd->AddInsItemDropInfo(pUnit->GetID(), CheckPlayerTeam);

		BM::Stream Packet(PT_M_C_NFY_DISPLAY_DROPBEAR_TIMER);
		Packet.Push(static_cast<int>(1));	// 드롭된건지 본진으로 돌아간건지 누가 먹은건지 0=본진생성 1=드랍 2=누가먹음
		Packet.Push(CheckPlayerTeam);		// 곰의 팀
		Packet.Push(RemainTime);			// 남은 시간
		m_pGnd->Broadcast(Packet);
	}

	BM::Stream Packet(PT_M_C_NFY_LOVEMODE_MESSAGE);
	Packet.Push(74111);
	Packet.Push(false);
	Packet.Push(CheckPlayerTeam);
	m_pGnd->Broadcast(Packet);
}