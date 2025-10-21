#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_ANNIHILATION_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_ANNIHILATION_H

#include "PgPvPType_DeathMatch.h"

class PgPvPType_Annihilation
	:	public PgPvPType_DeathMatch
{

public:
	PgPvPType_Annihilation();
	virtual ~PgPvPType_Annihilation();

	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );

	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd );
	virtual EPVPTYPE Type()const{return PVP_TYPE_ANNIHILATION;}
	virtual bool End( bool const bAutoChange, PgWarGround *pkGnd );

	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );

	virtual bool IsRevive()const{return false;}// 섬멸전은 부활 불가

	virtual bool IsAbusing(void)const;

protected:
	DWORD	m_dwBeginTime;
	DWORD	m_dwTotalGameTime;
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_ANNIHILATION_H