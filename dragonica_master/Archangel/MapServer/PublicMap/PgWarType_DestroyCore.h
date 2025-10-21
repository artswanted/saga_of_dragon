#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_DESTROYCORE_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_DESTROYCORE_H

class PgWarType_DestroyCore
	:	public PgBattleForEmporia
{
public:
	PgWarType_DestroyCore(void);
	virtual ~PgWarType_DestroyCore(void){}

	virtual EPVPTYPE Type()const{return WAR_TYPE_DESTROYCORE;}
	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround *pkGnd );

	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );

	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );

	virtual bool InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd );
	virtual bool IsUseObjectUnit()const{return true;}
	virtual void WriteToStartAddOnPacket( BM::Stream &kPacket )const;

protected:
	BM::GUID const &GetMainCoreID( int const iTeam )const;
	virtual BYTE AddTeamPoint( int const iTeam, unsigned short nPoint, bool const bCharacter, PgWarGround *pkGnd );
	void SetCoreAtkAuth( int const iTeam, PgWarGround *pkGnd );

protected:
	int				m_iLeaderTeam;
	WORD			m_usCoreATKAuthPoint;
	WORD			m_usUseKillAuthPoint;
	BM::GUID		m_kMainCoreGuid[2];
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_DESTROYCORE_H