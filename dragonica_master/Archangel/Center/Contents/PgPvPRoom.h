#ifndef CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPROOM_H
#define CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPROOM_H

#include "Variant/PgPvPUtil.h"

// јіБ¤ << 1024ён / 256ЖА(°­) / 128°ж±в 
int const MAX_PVP_ROOM_COUNT	= 130;

// tagPvPGameUser¶ы АПёЖ»уЕл(°°АМ єЇ°жЗШѕЯ ЗСґЩ.
// WriteToPacket, ReadFromPacket
typedef struct tagPvPRoomUser
{
	tagPvPRoomUser( PgPlayer const * _pkPlayer, BYTE const _byStatus=PS_EMPTY )
		:	pkPlayer(_pkPlayer)
//		,	iHandyCap(100)
		,	byStatus(_byStatus)
		,	byEntryNo(0)
	{}

	tagPvPRoomUser( tagPvPRoomUser const &rhs )
		:	pkPlayer(rhs.pkPlayer)
		,	kTeamSlot(rhs.kTeamSlot)
//		,	kResult(rhs.kResult)
//		,	iHandyCap(rhs.iHandyCap)
		,	byStatus(rhs.byStatus)
		,	byEntryNo(rhs.byEntryNo)
	{}

	PgPlayer const * pkPlayer;

	SPvPTeamSlot		kTeamSlot;
//	SPvPResult			kResult;
//	int					iHandyCap;// ЗЪµрДё °Є
	BYTE				byStatus;
	BYTE				byEntryNo;

	void WriteToPacket( BM::Stream& kPacket )const
	{
		PVPUTIL::WriteToPacket_PlayerToLobbyUser( pkPlayer, kPacket );
		kPacket.Push(pkPlayer->GetAbil(AT_C_MAX_HP));
		kPacket.Push(pkPlayer->GetAbil(AT_C_MAX_MP));
		kPacket.Push(pkPlayer->GetAbil(AT_HP));
		kPacket.Push(pkPlayer->GetAbil(AT_MP));
		kPacket.Push(pkPlayer->CoupleGuid());
		kPacket.Push(pkPlayer->GMLevel());
		kTeamSlot.WriteToPacket(kPacket);
//		kPacket.Push(iHandyCap);
		kPacket.Push(byStatus);
		kPacket.Push(byEntryNo);
	}

}SPvPRoomUser;

class PgPvPRoom
{
public:
	static BYTE const ms_ucMaxRoomUserCount = 10;

	typedef std::map< BM::GUID, SPvPRoomUser >		ContUserSlot;

public:
	explicit PgPvPRoom( int const iIndex, int const &iLobbyID );
	~PgPvPRoom();

	void Clear();

	int const GetIndex()const{return m_iIndex;}
	CONT_PVPROOM_LIST::key_type const GetRoomNo()const{return static_cast<CONT_PVPROOM_LIST::key_type>(m_iIndex) + 1;}
//	unsigned int GetCreateNo()const{return m_iCreateNo;}

	EPVPTYPE Mode()const{return m_kBasicInfo.m_kType;}
	bool IsOpen()const{return m_kBasicInfo.m_kStatus != ROOM_STATUS_CLOSE;}
	bool IsPlaying()const{return m_kBasicInfo.m_kStatus >= ROOM_STATUS_READY;}

	HRESULT Create( BM::GUID const &kBattleGuid, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo);//BattleID·О №«АО »эјє
	HRESULT Create( PgPlayer const * pkPlayer, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo /*unsigned int const iCreateNo*/ );//АЇАъ°Ў »эјє
	void Close();// °­Б¦ ґЭ±в
	HRESULT Join( PgPlayer const * pkPlayer, int const iTeam);
	HRESULT Join( PgPlayer const * pkPlayer, std::string const &strPassWord, bool const bJoin=true );
	EPvPRoomJoinRet SetJoin( PgPlayer const *pkPlayer, std::string const &strPassWord );
	HRESULT Exit( BM::GUID const &kCharGuid, bool const bDisConnected=false );
	HRESULT Modify( BM::GUID const &kCharGuid, BM::Stream * const pkPacket );
	HRESULT TeamChange( BM::GUID const &kCharGuid, int const iTeam );
	HRESULT KickUser( BM::GUID const &kReqCharGuid, BM::GUID const &kKickCharGuid );
//	HRESULT ChangeHandyCap( BM::GUID const &kReqCharGuid, BM::Stream * const pkPacket );
	HRESULT SetSlotStatus( BM::GUID const &kCharGuid, BM::Stream *pkPacket );
	HRESULT EntryChange( BM::GUID const &kCharGuid, BM::Stream *pkPacket );
	HRESULT EntrustMaster( BM::GUID const &kMasterCharGuid, BM::GUID const &kTargetCharGuid );

	HRESULT AutoReady();
	HRESULT Ready( BM::GUID const &kCharGuid, BM::Stream * const pkPacket  );
	void ReadyFailed( SGroundKey const &kGroundKey );
	HRESULT Start( SGroundKey const &kGndKey, bool bSuccess );
	HRESULT End( SEventMessage* const pkMsg );
	HRESULT ReloadRoom( bool const bNoEnemy );

	void ProcessMsg( SEventMessage* const pkMsg );
	void RecvPacket( BM::GUID const &kCharGuid, BM::Stream::DEF_STREAM_TYPE const usType, BM::Stream * const pkPacket );
	void BroadCast( BM::Stream const &kPacket, int const iTeam=TEAM_NONE, BYTE const kFilterStatus=PS_EMPTY, BM::GUID const &kRemoveCharGuid=BM::GUID::NullData() );
	void SetUserEvent( BM::Stream const &kPacket, PVPUTIL::PgUserEvent const &kMergeEvent );

	static void SendWarnMessage( PgPlayer const *pkPlayer, int const iMessageNo, BYTE const byLevel );

	bool IsExercise(void)const{return m_iLobbyID == PvP_Lobby_GroundNo_Exercise;}
	bool IsRanking(void)const{return m_iLobbyID == PvP_Lobby_GroundNo_Ranking;}
	bool IsLeague(void)const{return m_iLobbyID == PvP_Lobby_GroundNo_League;}
	bool IsBalance(void)const{return IsRanking() && (PVP_MODE_TEAM == m_kBasicInfo.m_kMode);}
	bool IsModifyRoom(void)const{return IsExercise();}
	bool GetBattleGuid(BM::GUID& rkOut);
	int GetAutoWinTeam();
protected:
	void SendModifyed()const;

	HRESULT Check( SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo );
	HRESULT Check( PgPlayer const * pkPlayer, SPvPRoomBasicInfo &kBasicInfo, SPvPRoomExtInfo &kExtInfo );
	HRESULT Check( EPVPTYPE const kType, EPVPMODE& kMode, BYTE& ucRound )const;

	HRESULT SetMaster( BM::GUID const &kCharGuid );
	bool IsMaster( BM::GUID const &kCharGuid )const{return m_kMasterGuid == kCharGuid;}
	HRESULT RefreshSlot();
	HRESULT RefreshEntry();

	bool IsCheckBalanceJoin( int const iClass, int const iTeam )const;
	EPvPRoomJoinRet JoinEmptySlot( PgPlayer const * pkPlayer, int iTeam=TEAM_NONE, bool bSendUnit=false );
	bool SetJoinEmptySlot( PgPlayer const *pkPlayer, int const iTeam, bool const bSendUnit );

	HRESULT ClearEntry( BM::GUID const &kCharGuid );
	int	GetTeamUserCount(int& iRedTeamCount, int& iBlueTeamCount)const;
private:
	bool SetStatus( EPvPRoomStatus const kStatus );
	void ReqCreateGround( bool const bReCall=false );

protected:
	mutable Loki::Mutex		m_kMutex;

	int	const				m_iIndex;
	int const				m_iLobbyID;
	BM::GUID const			m_kRoomGuid;

	SPvPRoomBasicInfo		m_kBasicInfo;
	SPvPRoomExtInfo			m_kExtInfo;

	ContUserSlot			m_kContSlot_User;
	CONT_PVP_SLOT		m_kContSlot_Empty;
	CONT_PVP_ENTRY	m_kContEntry_Empty;
	BM::GUID				m_kMasterGuid;

	SGroundKey				m_kGameGround;

	PVPUTIL::PgUserEvent	m_kUserEvent;
	bool					m_bOrderClose;

	BM::GUID		m_kBattleGuid;
private:
	PgPvPRoom();
	PgPvPRoom( PgPvPRoom const & );
	PgPvPRoom& operator=( PgPvPRoom const & );
};

inline void PgPvPRoom::SendModifyed()const
{
	BM::Stream kPacket( PT_ROOM_TO_LOBBY_MODIFYED, GetRoomNo() );
	m_kBasicInfo.WriteToPacket(kPacket);
	SendToPvPLobby( kPacket, m_iLobbyID );
}

#endif // CENTER_CENTERSERVER_CONTENTS_PVP_PGPVPROOM_H