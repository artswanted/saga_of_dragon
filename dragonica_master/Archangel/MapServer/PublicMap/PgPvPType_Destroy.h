#ifndef MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DESTORY_H
#define MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DESTORY_H

#include "PgPvPType_DeathMatch.h"

class PgPvPType_Destroy
	:	public PgPvPType_DeathMatch
{
public:
	PgPvPType_Destroy();
	virtual ~PgPvPType_Destroy();

	virtual bool Start( PgPlayer *pkPlayer, PgWarGround * pkGnd );

	virtual EPVPTYPE Type()const{return PVP_TYPE_DESTROY;}
	virtual HRESULT Init( BM::Stream &kPacket, PgWarGround* pkGnd );

	virtual HRESULT SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd );

	virtual HRESULT Update( DWORD const dwCurTime );
	virtual void ReadyGame( PgWarGround *pkGnd=NULL ){}
	virtual void StartGame( BM::Stream &kPacket, bool const bGame );

protected:
	virtual bool CalcRoundWinTeam( int &iOutRoundWinTeam );// 리턴값은 게임 종료여부
};

#endif // MAP_MAPSERVER_PUBLICMAP_PVPWAR_MODE_PGPVPTYPE_DESTORY_H