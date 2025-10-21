#ifndef MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGINDUNHOME_H
#define MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGINDUNHOME_H

#include "PgIndun.h"

class PgIndunHome
	:	public PgIndun
{
public:
	PgIndunHome();
	virtual ~PgIndunHome();
	virtual EGroundKind GetKind()const{return GKIND_HOME;}
	virtual bool IsDeleteTime()const;
	virtual void OnTick1s(){}
protected:
	virtual void SendMapLoadComplete( PgPlayer *pkUser );
private:

	__int64 m_i64DeleteTime;
};

#endif // MAP_MAPSERVER_MAP_GROUND_PUBLIC_PGINDUNHOME_H