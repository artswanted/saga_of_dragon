#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DEATHMATCH_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DEATHMATCH_H

#include "PgWarMode.h"

typedef struct tagPvPTeamResult
:	public SPvPResult
{
	tagPvPTeamResult()
		:	iPlayUserCount(0)
		,	ucWinCount(0)
	{
		kTeamGuid.Generate();
	}

	// 리턴값이 S_FALSE이면 비긴거다.
	// One Round에만 적용됨
	HRESULT IsWin( tagPvPTeamResult const &rhs )const
	{
		if ( !iPlayUserCount )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
		if ( !rhs.iPlayUserCount ){return S_OK;}
		if ( *this > rhs ){return S_OK;}
		if ( *this == rhs ){return S_FALSE;}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	void Clear( bool bOnlyResult=false )
	{
		SPvPResult::Clear();
		if ( !bOnlyResult )
		{
			iPlayUserCount = 0;
			ucWinCount = 0;
		}
	}

	size_t		iPlayUserCount;
	BYTE		ucWinCount;
	BM::GUID	kTeamGuid;//PartyGUID때문에 필요

}SPvPTeamResult;

struct SPvPRewardFind
{
	SPvPRewardFind( SPvPReward const &kUserReward )
		:	iPoint((int)kUserReward.GetTotalPoint())
	{}

	bool operator()( CONT_DEF_PVP_REWARD::value_type const &kDefPvPReward )const
	{
		return kDefPvPReward.IsIn( iPoint );
	}

	int const iPoint;
};

class PgPvPType_DeathMatch
	:	public PgWarMode
{
public:
	typedef std::map<TBL_KEY_INT,SPvPTeamResult>		CONT_PVP_TEAM_RESULT_INFO;

public:

	PgPvPType_DeathMatch();
	virtual ~PgPvPType_DeathMatch();

	virtual EPVPTYPE Type(void)const{return PVP_TYPE_DM;}
	virtual int GetPvPLobbyID(void)const{return m_iLobbyID;}

	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );

	virtual bool IsJoin( PgPlayer *pkPlayer );
	virtual HRESULT Join( PgPlayer *pkPlayer, bool const bInturde=false );
	virtual bool Exit( BM::GUID const &kCharGuid );
	virtual bool GetReadyUnitList( VEC_GUID& kUnitList )const;

	virtual void ReadyGame( PgWarGround *pkGnd=NULL ){}
	virtual bool IsStart( DWORD const dwNowTime );
	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd );
	virtual void StartGame( BM::Stream &kPacket, bool const bGame );
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );
	virtual EResultEndType ResultEnd();

	virtual HRESULT Update( DWORD const dwCurTime );

	virtual void Modify( BM::Stream &kPacket );

	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );
	virtual HRESULT SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd );
	virtual HRESULT SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd );
	virtual HRESULT SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd );

	virtual bool Send( BM::Stream& kPacket )const;

	virtual bool IsUseItem()const{return E_PVP_OPT_USEITEM & m_kOption;}
	virtual bool IsExercise(void)const{return m_iLobbyID == PvP_Lobby_GroundNo_Exercise;}

	virtual bool IsAbusing(void)const;
	virtual int GetRewardID(void)const;
	virtual bool InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd ){return false;}
	virtual bool CheckEntryWait(DWORD const dwElapsedTime) { return true; }
	virtual bool SelectEntry(int const iTeam, BM::GUID const &rkCharGuid) { return false; }

	virtual bool IsRevive()const{return true;}

protected:
	virtual void InitTeamResult();
	virtual void RefreshUserCount();
	virtual bool CalcRoundWinTeam( int &iOutRoundWinTeam );// 리턴값은 게임 종료여부

	virtual void SetEventTeam_Kill( int const iTeam, int const iPoint );
	virtual void SetEventTeam_Die( int const iTeam, int const iPoint );

	virtual int GetWinTeam( BYTE& kOutRedWin, BYTE& kOutBlueWin )const;
	SPvPTeamResult* GetTeamResult( int const iTeam );

public:
	// 전용함수
	EPVPMODE GetMode()const{return m_kMode;}
	int GetRoomIndex()const{return m_iRoomIndex;}
	BYTE GetNowRound()const{return m_ucNowRound;}
	BYTE GetMaxRound()const{return m_ucMaxRound;}

protected:
	int							m_iLobbyID;

	CONT_PVP_GAME_USER			m_kContPlay;
	CONT_PVP_GAME_USER			m_kContReady;

	CONT_PVP_TEAM_RESULT_INFO	m_kContTeamResult;

	// 정리해보자.
	BYTE						m_kOption;
	EPVPMODE					m_kMode;
	int							m_iRoomIndex;
	BYTE						m_ucMaxRound;
	BYTE						m_ucNowRound;
};

inline SPvPTeamResult* PgPvPType_DeathMatch::GetTeamResult( int const iTeam )
{
	CONT_PVP_TEAM_RESULT_INFO::iterator itr = m_kContTeamResult.find(iTeam);
	if ( itr != m_kContTeamResult.end() )
	{
		return &(itr->second);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DEATHMATCH_H