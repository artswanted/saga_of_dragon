#ifndef CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_LEAGUE_H
#define CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_LEAGUE_H

#include "Variant/PgPvPUtil.h"
#include "PgPvPRoom.h"
#include "PgPvPLobby_Base.h"
#include "PgPvPLobby.h"

class PgPvPLobby_League
	:	public PgPvPLobby_Base
{
public:
	static int const MAX_CELL_SIZE = 5;
public:
	typedef std::vector< PgPvPRoom* >				CONT_ALL_ROOM;
	typedef std::map< CONT_PVPROOM_LIST::key_type, PgPvPRoom* >				CONT_CLOSE_ROOM;
	typedef std::set< CONT_PVPROOM_LIST::key_type, std::greater<CONT_PVPROOM_LIST::key_type> >	CONT_UPDATE_ROOM_LIST;

public:
	explicit PgPvPLobby_League( int const iLobbyID );
	virtual ~PgPvPLobby_League(void);

	bool IsIndex( int const iIndex )const{return iIndex>=0 && iIndex<(int)m_kContRoom.size();}
	
	virtual void Locked_OnTick();
	virtual void Locked_OnTickEvent(void);
	virtual void Locked_OnTickLog(void);

	virtual bool ProcessMsg( BM::Stream::DEF_STREAM_TYPE const kType, SEventMessage * pkMsg );
	virtual bool Locked_SetOpen( bool const bIsOpen );
	virtual bool Locked_SetPlayLimitInfo( CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kPlayLimitInfo );

	void RecvFromUser( BM::GUID const &kCharGuid, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket );

	virtual bool Locked_Join( PgPlayer *pkPlayer );
	bool Locked_Exit(BM::GUID const &kCharGuid, bool const bDisConnected );

	bool Locked_ReJoin( BM::GUID const &kCharGuid, bool const bKick );
	bool Locked_JoinRoom( int const iRoomIndex, int const iTeam, BM::GUID const &rkCharGuid );
	bool Locked_ReadFromUserInfo( BM::GUID const &kCharGuid, BM::Stream * const pkPacket, bool const bKickRoom );
	bool Locked_ReadFromItemChangeArray( BM::GUID const &kCharGuid, DB_ITEM_STATE_CHANGE_ARRAY &kArray );

	void Locked_ReadLeagueInfo( BM::Stream &kPacket );
	void Locked_WriteLeagueInfo( BM::Stream &kPacket )const;

	void Locked_WriteRoomList( BM::Stream &kPacket )const;
	bool Locked_Recv_PT_N_T_REQ_CREATE_PVPLEAGUE_ROOM( BM::Stream * const pkPacket );
	bool Locked_Recv_PT_C_T_REQ_CHANGE_LOBBY( BM::GUID const &kCharGuid, int const iTargetLobbyID );
	bool Locked_Recv_PT_C_M_REQ_CHAT(  BM::GUID const &kCharGuid, BM::Stream * const pkPacket )const;

	bool Locked_SendToUser( BM::GUID const &kCharGuid, BM::Stream const &kPacket )const;
	void BroadCast( BM::Stream const &kPacket, bool const bOnlyLobby )const;

	void Locked_Recv_PT_N_T_REQ_START_PVPLEAGUE_GAME();
	bool Locked_Recv_PT_N_T_REQ_END_PVPLEAGUE_GAME();
	bool Locked_Recv_PT_N_C_NFY_CLOSE_PVPLEAGUE_GAME(BM::Stream * const pkPacket );
protected:
	void Join( PgPlayer *pkPlayer, bool const bKick )const;
	void Exit( PgPlayer *pkPlayer, bool const bDisConnected );

	bool SetOpen( bool const bIsOpen );

	PgPvPRoom *Locked_GetRoom( BM::GUID const &kCharGuid )const;
	bool Locked_ModifyRoom( CONT_PVPROOM_LIST::key_type const &kKey, CONT_PVPROOM_LIST::mapped_type const &kElement );
	void Locked_RemoveRoom( int const iIndex );

	bool SendToUser( BM::GUID const &kCharGuid, BM::Stream const &kPacket )const;

	PgPvPRoom *GetCloseRoom( int const iIndex )const;
	PgPvPRoom *GetRoom( int const iIndex )const;
	PgPvPCell *GetCell( PgPlayer const *pkPlayer );

	bool Locked_GetBattleRoomInfo( BM::GUID const & rkTeamGuid, int &iOutRoomIndex, int &iOutTeam ) const;
protected:

//	БЦ : m_kUserMutex -> m_kRoomMutex ёё Згїл m_kContUser
	mutable ACE_RW_Thread_Mutex	m_kUserMutex;

//	БЦ2 : m_kContActiveRoom, m_kContUpdateRoom m_kContCloseRoom їЎёё »зїл
	mutable Loki::Mutex			m_kRoomMutex;

//	БЦ3 : m_kContBattle, m_kContTeamToBattle їЎёё »зїл
	mutable Loki::Mutex			m_kBattleMutex;

//	БЦ4 : m_kPacketLeagueInfo, m_dwOldCheckTime їЎёё »зїл
	mutable Loki::Mutex			m_kLeagueInfoMutex;
	
	CONT_ALL_ROOM				m_kContRoom;
	CONT_PVPROOM_LIST			m_kContActiveRoom;
	CONT_UPDATE_ROOM_LIST		m_kContUpdateRoom;
	CONT_CLOSE_ROOM				m_kContCloseRoom;

//	CellMgr
	PgPvPCell					m_kCellArray[MAX_CELL_SIZE];
	int							m_iBalanceCellIndex;//
	size_t						m_iBalanceCellUserSize;//
	PVPUTIL::PgUserEvent		m_kUserEvent;
	DWORD						m_dwLastEventTime;

	CONT_DEF_PLAYLIMIT_INFO::mapped_type::value_type m_kPlayLimitInfo;

	CONT_DEF_PVPLEAGUE_BATTLE m_kContBattle;
	CONT_PVPLEAGUE_TEAM_TO_BATTLE m_kContTeamToBattle;

	BM::Stream m_kPacketLeagueInfo;

	//Tick 10s Check
	DWORD m_dwOldCheckTime;
//	RoomЗСЕЧ »С·ББЩ АМєҐЖ® Б¤єёµй
private:
	void SetEvent( PgPlayer const *pkPlayer, BYTE const byType );
};

inline PgPvPRoom* PgPvPLobby_League::GetRoom( int const iIndex )const
{
	if ( IsIndex(iIndex) )
	{
		return m_kContRoom.at(iIndex);
	}
	return NULL;
}

inline PgPvPCell* PgPvPLobby_League::GetCell( PgPlayer const * pkPlayer )
{
	int const iCellIndex = pkPlayer->GetAbil( AT_PVP_CELL_INDEX );
	if ( iCellIndex < MAX_CELL_SIZE )
	{
		return m_kCellArray + iCellIndex;
	}
	else
	{
		CAUTION_LOG( BM::LOG_LV1, __FL__ << L"Player<" << pkPlayer->GetID() << L"> CellIndex<" << iCellIndex << L"> Error!!!" )
	}
	return NULL;
}

#endif // CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_LEAGUE_H