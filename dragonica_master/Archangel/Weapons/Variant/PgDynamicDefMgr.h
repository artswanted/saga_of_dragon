#ifndef WEAPON_VARIANT_ABIL_PGDYNAMICDEFMGR_H
#define WEAPON_VARIANT_ABIL_PGDYNAMICDEFMGR_H

#include "BM/ObjectPool.h"
#include "Lohengrin/dbtables.h"
#include "PgAddAbilRateDef.h"
#include "TDefMgr.h"


class PgDynamicDefMgr
	:	protected TDefMgr< TBL_DEF_DYNAMICABILRATE_BAG, TBL_DEF_DYNAMICABILRATE, PgAddAbilRateDef , TBL_KEY_INT, TBL_KEY_INT >
{
	template<typename T_DEF,typename T_UNIT,typename T_TUNNINGDEF>
	friend class PgDefControl;

public:

	PgDynamicDefMgr();
	virtual ~PgDynamicDefMgr();

	bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	virtual bool Build(const CONT_BASE *ptblBase, const CONT_ABIL *ptblAbil);
	void swap(PgDynamicDefMgr& rkRight);
};

//#define g_kDynamicDefMgr SINGLETON_STATIC(PgDynamicDefMgr)

#endif // WEAPON_VARIANT_ABIL_PGDYNAMICDEFMGR_H