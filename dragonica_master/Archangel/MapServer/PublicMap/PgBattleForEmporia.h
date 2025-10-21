#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEFOREMPORIA_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEFOREMPORIA_H

#include "PgBattleArea.h"
#include "PgWarMode.h"

class PgBattleForEmporia
	:	public PgWarMode
{
public:
	PgBattleForEmporia();
	virtual ~PgBattleForEmporia(){}

	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround *pkGnd );
	virtual bool IsJoin( PgPlayer *pkPlayer );
	virtual HRESULT Join( PgPlayer *pkPlayer, bool const bInturde=false );
	virtual bool Exit( BM::GUID const &kCharGuid );

	virtual void ReadyGame( PgWarGround *pkGnd=NULL );
	virtual bool IsStart( DWORD const dwNowTime );
	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd );
	virtual void StartGame( BM::Stream &kPacket, bool const bGame );
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );
	virtual EResultEndType ResultEnd();

	virtual HRESULT Update( DWORD const dwCurTime );
	virtual bool IsRevive()const{return m_bIsRevive;}

	virtual HRESULT SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd );
	virtual HRESULT SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd );
	virtual HRESULT SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd );
	virtual void SetEvent_AdjustArea( CUnit *pkUnit );

	virtual void WriteToStartAddOnPacket( BM::Stream &kPacket )const;
	virtual HRESULT SetSelectArea( CUnit *pkUnit, int const iAreaIndex, PgGround *pkGnd );

	virtual bool Send( BM::Stream& kPacket )const;
	
	virtual DWORD GetTime( EPvPTime const kType )const;

protected:
	void SendResultNotice(int const iWinTeam, EEmporiaResultNoticeType const eType, CUnit const * pkKiller=NULL)const;

	HRESULT InitBattleArea(PgWarGround *pkGnd);
	int GetTeam( PgPlayer *pkPlayer )const;
	virtual BYTE AddTeamPoint( int const iTeam, unsigned short nPoint, bool const bCharacter, PgWarGround *pkGnd );
	void AddUserCount( int const iTeam, bool const bAdd );
	bool IsMercenary(int const iTeam, BM::GUID const rkGuildGuid);
	void GetMemberData(CONT_EM_RESULT_USER & kContWinUser, CONT_EM_RESULT_USER & kContLoseUser);

public:
	BM::GUID const &GetEmporiaGUID()const{return m_kEmporiaID;}

protected:
	BM::GUID					m_kEmporiaID;
	BM::GUID					m_kBattleID;
	int							m_iBattleIndex;

	SGuildBattleInfo			m_kDefenceGuildInfo;
	SGuildBattleInfo			m_kAttackGuildInfo;
	CONT_EMPORIA_BATTLE_USER	m_kContUser;
	CONT_EMPORIA_BATTLE_USER	m_kContOutUser;//한번 들어왔다가 나간놈

	CONT_EMBATTLE_OBJECT		m_kContObjectInfo;
	CONT_BATTLEAREA				m_kContBattleArea;

	__int64						m_i64ReadyTime;//Init을 한 시간

	DWORD						m_dwLastReBirthTime;
	bool						m_bIsRevive;
    mutable EEmporiaResultNoticeType    m_eResultType;

	// 입장인원 제한
	size_t						m_iUserCount_MinInTeam;	// 한팀당 최소 유저수
	size_t						m_iUserCount_MaxInTeam;	// 한팀당 허용 유저수
	size_t						m_iUserCount_AttackTeam;
	size_t						m_iUserCount_DefenceTeam;
};

inline bool PgBattleForEmporia::Send( BM::Stream& kPacket )const
{
	return SendToRealmContents( PMET_EMPORIA, kPacket );
}

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGBATTLEFOREMPORIA_H