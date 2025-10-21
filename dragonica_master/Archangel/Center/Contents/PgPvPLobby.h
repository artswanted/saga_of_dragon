#ifndef CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_H
#define CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_H

#include "Variant/PgPvPUtil.h"
#include "PgPvPRoom.h"
#include "PgPvPLobby_Base.h"

// PgPvPLobby를 분산하기 위한 Class
class PgPvPCell
{

public:
	typedef std::map< BM::GUID, PgPlayer const * >		CONT_USERINFO;

public:
	struct SJoinProcess
	{
		bool operator()( PgPvPCell &rhs )const;
		BM::GUID		m_kMemberGuid;
		SERVER_IDENTITY m_kSwitchServer;
	};
	
public:
	PgPvPCell(void);
	~PgPvPCell(void);

	void Locked_SendJoinPacket( BM::GUID const &kMemberGuid, SERVER_IDENTITY const &kSwitchServer )const;
	HRESULT Locked_Join( CONT_USERINFO::mapped_type const &kElement );
	HRESULT Locked_Exit( CONT_USERINFO::mapped_type const &kElement );
	HRESULT Locked_JoinRoom( CONT_USERINFO::mapped_type const &kElement );
	HRESULT Locked_ExitRoom( CONT_USERINFO::mapped_type const &kElement );

	void Locked_BroadCast( BM::Stream const &kPacket, bool const bOnlyLobby )const;
	size_t Locked_SetUserEvent( BM::Stream &kPacket );

	void SetIndex( int const nIndex ){m_iIndex=nIndex;}

private:
	void CreateJoinPacket(void);
	void BroadCast( BM::Stream const &kPacket, bool const bOnlyLobby )const;

private:
	mutable Loki::Mutex			m_kMutex;

	int			 				m_iIndex;

	CONT_USERINFO				m_kContUserInfo;

	BM::Stream				m_kJoinPacket;	// 조인시에 사용할 패킷

private:
	PgPvPCell( PgPvPCell const & );
	PgPvPCell& operator=( PgPvPCell const & );
};

class PgPvPLobby
	:	public PgPvPLobby_Base
{
public:
	static int const MAX_CELL_SIZE = 5;

public:
	typedef std::vector< PgPvPRoom* >				CONT_ALL_ROOM;
	typedef std::queue< PgPvPRoom* >				CONT_CLOSE_ROOM;
	typedef std::set< CONT_PVPROOM_LIST::key_type, std::greater<CONT_PVPROOM_LIST::key_type> >	CONT_UPDATE_ROOM_LIST;

public:
	explicit PgPvPLobby( int const iLobbyID );
	virtual ~PgPvPLobby(void);

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
	bool Locked_JoinRoom( BM::GUID const &kCharGuid, BM::Stream * const pkPacket );
	bool Locked_ReadFromUserInfo( BM::GUID const &kCharGuid, BM::Stream * const pkPacket, bool const bKickRoom );
	bool Locked_ReadFromItemChangeArray( BM::GUID const &kCharGuid, DB_ITEM_STATE_CHANGE_ARRAY &kArray );

	void Locked_WriteRoomList( BM::Stream &kPacket )const;
	bool Locked_Recv_PT_C_T_CREATE_ROOM( BM::GUID const &kCharGuid, BM::Stream * const pkPacket );
	bool Locked_PT_C_T_REQ_CHANGE_LOBBY( BM::GUID const &kCharGuid, int const iTargetLobbyID );
	void Locked_Recv_PT_C_T_REQ_WITH_PVP( BM::GUID const &kCharGuid, BM::Stream * const pkPacket );
	void Locked_Recv_PT_C_T_REQ_INVITE_PVP( BM::GUID const &kCharGuid, BM::Stream * const pkPacket )const;
	bool Locked_Recv_PT_C_M_REQ_CHAT(  BM::GUID const &kCharGuid, BM::Stream * const pkPacket )const;

	bool Locked_SendToUser( BM::GUID const &kCharGuid, BM::Stream const &kPacket )const;
	void BroadCast( BM::Stream const &kPacket, bool const bOnlyLobby )const;

	void Locket_PT_C_T_REQ_INVATE_FAIL( BM::GUID const &kCharGuid, BM::Stream * const pkPacket );
protected:
	void Join( PgPlayer *pkPlayer, bool const bKick )const;
	void Exit( PgPlayer *pkPlayer, bool const bDisConnected );

	bool SetOpen( bool const bIsOpen );

	PgPvPRoom *Locked_GetRoom( BM::GUID const &kCharGuid )const;
	bool Locked_ModifyRoom( CONT_PVPROOM_LIST::key_type const &kKey, CONT_PVPROOM_LIST::mapped_type const &kElement );
	void Locked_RemoveRoom( int const iIndex );

	bool SendToUser( BM::GUID const &kCharGuid, BM::Stream const &kPacket )const;

	PgPvPRoom *GetRoom( int const iIndex )const;
	PgPvPCell *GetCell( PgPlayer const *pkPlayer );

protected:

//	주 : m_kUserMutex -> m_kRoomMutex 만 허용
	mutable ACE_RW_Thread_Mutex	m_kUserMutex;

//	주2 : m_kContActiveRoom, m_kContUpdateRoom m_kContCloseRoom 에만 사용
	mutable Loki::Mutex			m_kRoomMutex;
	
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

//	Room한테 뿌려줄 이벤트 정보들
private:
	void SetEvent( PgPlayer const *pkPlayer, BYTE const byType );
};

inline PgPvPRoom* PgPvPLobby::GetRoom( int const iIndex )const
{
	if ( IsIndex(iIndex) )
	{
		return m_kContRoom.at(iIndex);
	}
	return NULL;
}

inline PgPvPCell* PgPvPLobby::GetCell( PgPlayer const * pkPlayer )
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

#endif // CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_H