#ifndef WEAPON_VARIANT_UNIT_PGGROUNDITEMBOXMGR_H
#define WEAPON_VARIANT_UNIT_PGGROUNDITEMBOXMGR_H

#include "BM/ObjectPool.h"

#include "PgGroundItemBox.h"

class PgGroundItemBoxMgr
{
public:
	PgGroundItemBoxMgr(void);
	~PgGroundItemBoxMgr(void);

	PgGroundItemBox* Create();
protected:
	BM::TObjectPool< PgGroundItemBox > m_kBoxPool;
};

#define g_kGndItemBoxMgr SINGLETON_STATIC(PgGroundItemBoxMgr)

#endif // WEAPON_VARIANT_UNIT_PGGROUNDITEMBOXMGR_H