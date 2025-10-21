#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_PROTECTDRGON_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_PROTECTDRGON_H

class PgWarType_ProtectDragon
	:	public PgBattleForEmporia
{
public:
	PgWarType_ProtectDragon(void);
	virtual ~PgWarType_ProtectDragon(void){}

	virtual EPVPTYPE Type()const{return WAR_TYPE_PROTECTDRAGON;}
	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround *pkGnd );

	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );

	virtual bool InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd );
	virtual bool IsUseObjectUnit()const{return false;}
	virtual bool IsUseMonster()const{return true;}

	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );

	virtual void WriteToStartAddOnPacket( BM::Stream &kPacket )const;

protected:
	WORD				m_usDefenceWinPoint;
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGWARTYPE_PROTECTDRGON_H