#include "stdafx.h"
#include "PgPvPLobbyMgr.h"
#include "PgPvPLobby.h"
#include "PgPvPLobby_League.h"

PgPvPLobbyMgr::PgPvPLobbyMgr(void)
{

}

PgPvPLobbyMgr::~PgPvPLobbyMgr(void)
{

}

bool PgPvPLobbyMgr::ProcessMsg( SEventMessage *pkMsg )
{
	BM::CAutoMutex lock( m_kMutex_Wrapper_, false );
	return Instance()->ProcessMsg( pkMsg );
}

void PgPvPLobbyMgr::OnTick(void)
{
	BM::CAutoMutex lock( m_kMutex_Wrapper_, false );
	Instance()->OnTick();
}

void PgPvPLobbyMgr::OnTick_Event(void)
{
	BM::CAutoMutex lock( m_kMutex_Wrapper_, false );
	Instance()->OnTick_Event();
}

void PgPvPLobbyMgr::OnTick_Log(void)
{
	BM::CAutoMutex lock( m_kMutex_Wrapper_, false );
	Instance()->OnTick_Log();
}

bool PgPvPLobbyMgr::AddLobby( int const iLobbyID, CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kTimeInfo )
{
	switch ( iLobbyID )
	{
	case PvP_Lobby_GroundNo_Exercise:
	case PvP_Lobby_GroundNo_Ranking:
		{
			BM::CAutoMutex lock( m_kMutex_Wrapper_, true );
			return Instance()->AddLobby<PgPvPLobby>( iLobbyID, kTimeInfo );
		}break;
	case PvP_Lobby_GroundNo_League:
		{
			BM::CAutoMutex lock( m_kMutex_Wrapper_, true );
			return Instance()->AddLobby<PgPvPLobby_League>( iLobbyID, kTimeInfo );
		}break;
	default:
		{

		}break;
	}
	return false;
}
