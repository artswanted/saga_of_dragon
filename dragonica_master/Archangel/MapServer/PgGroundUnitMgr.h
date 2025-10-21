#ifndef MAP_MAPSERVER_MAP_PGGROUNDUNITMGR_H
#define MAP_MAPSERVER_MAP_PGGROUNDUNITMGR_H

#include <map>

#include "BM/GUID.h"
#include "BM/ObjectPool.h"
#include "BM/PgTask.h"
#include "PgTask_Mapserver.h"
#include "Lohengrin/packetstruct.h"

class PgGroundUnitMgr
{
	typedef std::map< BM::GUID, std::map< int, CUnit* > > CONT_UNIT_MGR;
public:
	PgGroundUnitMgr();
	~PgGroundUnitMgr();

	Loki::Mutex		m_kMutex;
	CONT_UNIT_MGR	ContUnitAll;

	bool Add(CUnit* pkUnit, int iGroundNo, T_GNDATTR const kGndAttr);
	bool Del(CUnit* pkUnit);
	void Get(BM::GUID rkCharGuid, CONT_SEARCH_UNIT_INFO &kUnitArray);
};
#define g_kGroundUnitMgr SINGLETON_STATIC(PgGroundUnitMgr)

#endif // MAP_MAPSERVER_MAP_PGGROUNDUNITMGR_H