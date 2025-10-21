#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_WINNERSTEAM_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_WINNERSTEAM_H

#include "PgPvPType_DeathMatch.h"

class PgPvPType_WinnersTeam 
	:	public PgPvPType_DeathMatch
{
	typedef std::list< SPvPGameUser >				CONT_PVP_TEAM_USER;
	typedef std::map< BYTE, CONT_PVP_TEAM_USER >	CONT_USER_ENTRY;
	typedef std::map< BYTE, SPvPGameUser >			CONT_PVP_RESULT;
public:
	PgPvPType_WinnersTeam ();
	virtual ~PgPvPType_WinnersTeam ();
	
	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );
	virtual EPVPTYPE Type(void)const{return PVP_TYPE_WINNERS;}
	
	virtual HRESULT IsObModePlayer( PgPlayer *pkPlayer, BM::GUID& rkOutTargetGuid );
	virtual bool IsStart( DWORD const dwNowTime );
	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd );
	
	virtual HRESULT Join( PgPlayer *pkPlayer, bool const bInturde=false );
	
	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );
	virtual HRESULT Update( DWORD const dwCurTime );
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );
	virtual EResultEndType ResultEnd();
	virtual bool IsAbusing(void)const;
	virtual void StartGame( BM::Stream &kPacket, bool const bGame );
	virtual bool InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd );
	virtual bool IsRevive()const{return false;}
protected:
	virtual void InitEntry();
	virtual void InitTeamResult();
	virtual bool CalcRoundWinTeam( int &iOutRoundWinTeam );// 리턴값은 게임 종료여부
	void SetWinTeam(int const iWinTeam);
	void SetLoseTeam(int const iLoseTeam);
	
public:
	static bool SortEntry(SPvPGameUser const &lhs, SPvPGameUser const &rhs);
	
	BM::GUID GetTopEntryUser( int const iTeam );
	BM::GUID GetSecondEntryUser( int const iTeam );
	SPvPGameUser PopTopEntryUser( int const iTeam );
	
protected:
	bool EntryChange(int const iTeam, BM::GUID const& kCharGuid);
	bool IsDieTeamUser(int const iTeam, BM::GUID const& kCharGuid, BM::GUID& kOutTeammateGuid);
	
protected:
	CONT_USER_ENTRY					m_kContUserEntry;
	CONT_PVP_RESULT					m_kContPvPResult;
	CONT_PVP_GAME_USER			m_kContWait;
	CONT_PVP_GAME_USER			m_kContBattle;

	bool							m_bEndRound;
	int								m_iRoundWinTeam;
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_WINNERSTEAM_H