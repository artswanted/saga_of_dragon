#ifndef CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_BASE_H
#define CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_BASE_H

#include "Variant/PgObjMgr.h"

class PgPvPLobby_Base
{
public:
	static __int64 const ms_i64ChangeLobbyTime = CGameTime::SECOND * 10i64;

public:
	typedef std::map< BM::GUID, PgPlayer* >	CONT_USER;// first : CharacterKey

public:
	PgPvPLobby_Base( int const iLobbyID ):m_iLobbyID(iLobbyID),m_bIsOpen(true){}
	virtual ~PgPvPLobby_Base(void){}

	static bool IsLobbyUser( PgPlayer const * const pkPlayer )
	{
		if ( pkPlayer )
		{
			return 0 > pkPlayer->GetAbil( AT_PVP_ROOM_INDEX );
		}
		return false;
	}

	int GetID(void)const{return m_iLobbyID;}

	virtual bool ProcessMsg( BM::Stream::DEF_STREAM_TYPE const kType, SEventMessage * pkMsg ) = 0;


	virtual bool Locked_SetOpen( bool const bIsOpen ) = 0;
	virtual bool Locked_SetPlayLimitInfo( CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kPlayLimitInfo ) = 0;
	virtual void Locked_OnTick(){}
	virtual void Locked_OnTickEvent(void){}
	virtual void Locked_OnTickLog(void){}

	virtual bool Locked_Join( PgPlayer *pkPlayer ) = 0;

protected:
	int const				m_iLobbyID;
	bool					m_bIsOpen;

	CONT_USER				m_kContUser;

	CONT_DEF_PLAYLIMIT_INFO::mapped_type	m_kContPlayLimitInfo;

private://사용금지
	PgPvPLobby_Base(void);
	PgPvPLobby_Base( PgPvPLobby_Base const & );
	PgPvPLobby_Base& operator = ( PgPvPLobby_Base const & );
};

class PgPvPLobby_AnterRoom
{
	struct SMsgBroadCast
	{
		SMsgBroadCast( SEventMessage * pkMsg )
			:	m_pkMsg(pkMsg)
			,	m_kType(0)
		{
			m_pkMsg->Pop( m_kType );
			m_iRDPos = m_pkMsg->RdPos();
		}

		void operator()(PgPvPLobby_Base * p )
		{
			m_pkMsg->RdPos( m_iRDPos );
			p->ProcessMsg( m_kType, m_pkMsg );
		}

		SEventMessage						*m_pkMsg;
		BM::Stream::DEF_STREAM_TYPE		m_kType;
		size_t 								m_iRDPos;
	};

	struct STick{void operator()( PgPvPLobby_Base * p ){p->Locked_OnTick();}};
	struct STickEvent {void operator()( PgPvPLobby_Base * p ){p->Locked_OnTickEvent();}};
	struct STickLog {void operator()( PgPvPLobby_Base * p ){p->Locked_OnTickLog();}};

public:
	PgPvPLobby_AnterRoom(void);
	~PgPvPLobby_AnterRoom(void);

	bool ProcessMsg( SEventMessage *pkMsg );

	template< typename T_LOBBY >
	bool AddLobby( int const iLobbyID, CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kTimeInfo );

	void OnTick(void);
	void OnTick_Event(void);
	void OnTick_Log(void);

protected:
	bool Exit( BM::GUID const &kCharGuid );
	bool RecvFromUser( BM::GUID const &kCharGuid, BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream * const pkPacket );

private:
	mutable	Loki::Mutex					m_kUserMutex;// User에만 Lock사용
	PgPvPLobby_Base::CONT_USER			m_kContUser;

	PgObjMgr< int, PgPvPLobby_Base >	m_kLobbys;
};

template< typename T_LOBBY >
inline bool PgPvPLobby_AnterRoom::AddLobby( int const iLobbyID, CONT_DEF_PLAYLIMIT_INFO::mapped_type const &kLimitInfo )
{
	if ( PvP_Lobby_GroundNo_AnterRoom < iLobbyID )
	{
		typename T_LOBBY *pkLobby = NULL;
		if ( true == m_kLobbys.New<T_LOBBY>( iLobbyID, &pkLobby ) )
		{
			pkLobby->Locked_SetPlayLimitInfo( kLimitInfo );
			return true;
		}
		else
		{
			PgPvPLobby_Base *pkLobby = m_kLobbys.Get( iLobbyID );
			if ( pkLobby )
			{
				pkLobby->Locked_SetPlayLimitInfo( kLimitInfo );
				return true;
			}

			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!!! LobbyID<" << iLobbyID << L">" );		
		}
	}
	return false;
}

#endif // CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPLOBBY_BASE_H