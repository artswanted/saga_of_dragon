#ifndef MAP_MAPSERVER_MAP_GROUND_PGPORTAL_EMPORIA_H
#define MAP_MAPSERVER_MAP_GROUND_PGPORTAL_EMPORIA_H

#include "Variant/PgEmporia.h"
#include "PgGroundTrigger.h"

class PgPortal_Emporia
	:	public PgEmporia
{
public:
	explicit PgPortal_Emporia( PgGTrigger_PortalEmporia const * pGTrigger_PortalEmporia );
	PgPortal_Emporia(void);
	virtual ~PgPortal_Emporia(void);

	virtual EGTriggerType const GetType(void)const{return GTRIGGER_TYPE_PORTAL_EMPORIA;}

	void WriteToPacket_ToClient( BM::Stream &kPacket )const;

protected:
	PgGTrigger_PortalEmporia const * m_pGTrigger_PortalEmporia;

private:// 사용금지
};

typedef std::map< SEmporiaKey, PgPortal_Emporia >		CONT_EMPORIA_PORTAL;

#endif // MAP_MAPSERVER_MAP_GROUND_PGPORTAL_EMPORIA_H