#include "stdafx.h"
#include "PgAction.h"
#include "PgWarGround.h"
#include "PgPvPType_Destroy.h"
#include "PgActionAchievement.h"

PgPvPType_Destroy::PgPvPType_Destroy()
{

}

PgPvPType_Destroy::~PgPvPType_Destroy()
{

}

HRESULT PgPvPType_Destroy::Init( BM::Stream &kPacket, PgWarGround* pkGnd )
{
	return PgPvPType_DeathMatch::Init( kPacket, pkGnd );
}

HRESULT PgPvPType_Destroy::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
{
	if ( pkUnit && pkKiller )
	{
		switch ( pkUnit->UnitType() )
		{
		case UT_PLAYER:
			{
				PgPvPType_DeathMatch::SetEvent_Kill( pkUnit, pkKiller, pkGnd );
			}break;
		case UT_OBJECT:
			{// Object Unit이면 게임이 끝난다.
				int const iTeam = pkUnit->GetAbil(AT_TEAM);
				switch ( iTeam )
				{
				case TEAM_RED:{m_iWinTeam=TEAM_BLUE;}break;
				case TEAM_BLUE:{m_iWinTeam=TEAM_RED;}break;
				}
			}break;
		}
		return S_OK;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgPvPType_Destroy::CalcRoundWinTeam( int &iOutRoundWinTeam )
{
	if ( m_iWinTeam )
	{
		CONT_PVP_TEAM_RESULT_INFO::iterator team_itr = m_kContTeamResult.find(m_iWinTeam);
		if ( team_itr != m_kContTeamResult.end() )
		{
			++(team_itr->second.ucWinCount);
			return m_iWinTeam;
		}
		VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__<<L"Not Found WinTeam["<<m_iWinTeam<<L"]" );
	}
	return PgPvPType_DeathMatch::CalcRoundWinTeam( iOutRoundWinTeam );
}

void PgPvPType_Destroy::StartGame( BM::Stream &kPacket, bool const bGame )
{
	PgPvPType_DeathMatch::StartGame( kPacket, bGame );
	if ( bGame )
	{
		// 몬스터와 Object를 Generate 해야 한다.
		BM::Stream kPacket( PT_A_G_NFY_DESTROY_OBJECT_REGEN );
		VNotify( &kPacket );
	}
}

HRESULT PgPvPType_Destroy::Update( DWORD const dwCurTime )
{
	if ( m_iWinTeam )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}
	return PgPvPType_DeathMatch::Update( dwCurTime );
}

bool PgPvPType_Destroy::Start( PgPlayer *pkPlayer, PgWarGround * pkGnd )
{
	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.find(pkPlayer->GetID());
	if ( itr != m_kContPlay.end() )
	{
		int const iTeam = itr->second.kTeamSlot.GetTeam();
		pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
		pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
		pkGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pkPlayer), 0, true, iTeam );

		pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
		return true;

	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

