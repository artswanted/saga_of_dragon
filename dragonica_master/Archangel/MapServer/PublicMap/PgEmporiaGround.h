#ifndef MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGEMPORIAGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGEMPORIAGROUND_H

#include "Variant/PgEmporia.h"
#include "PgGround.h"

class PgEmporiaGround
	:	public PgGround
{
public:
	PgEmporiaGround();
	virtual ~PgEmporiaGround();

	virtual void Clear();
	bool Clone( PgEmporiaGround *pkGround );
	virtual bool Clone( PgGround *pkGround );

	virtual EGroundKind GetKind()const{return GKIND_EMPORIA;}

	void ReadFromPacket_BaseInfo( BM::Stream &kPacket );

protected:
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	

protected:
	virtual bool IsAccess( PgPlayer *pkPlayer );
	virtual void SendMapLoadComplete( PgPlayer *pkUser );

protected:
	// 전용함수
	int CheckEmporiaFunctionAccess( CUnit *pkUnit, CONT_EMPORIA_FUNC::key_type const nFuncNo )const;
	void ProcessEmporiaFunction( CUnit *pkUnit, CONT_EMPORIA_FUNC::key_type const nFuncNo, BM::Stream * const pkPacket, BYTE const byGuildGrade );

protected:
	
	PgEmporia		m_kEmporiaInfo;

};

#endif // MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGEMPORIAGROUND_H