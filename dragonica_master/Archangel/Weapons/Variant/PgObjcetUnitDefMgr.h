#ifndef WEAPON_VARIANT_UNIT_PGOBJECTUNITDEFMGR_H
#define WEAPON_VARIANT_UNIT_PGOBJECTUNITDEFMGR_H

#include "BM/ObjectPool.h"
#include "Lohengrin/ProcessConfig.h"

#include "PgObjectUnitDef.h"
#include "TDefMgr.h"

class PgObjectUnitDefMgr
	:	public TDefMgr< TBL_DEF_OBJECT, TBL_DEF_OBJECTABIL, PgObjectUnitDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< PgObjectUnitDefMgr >;
public:

	PgObjectUnitDefMgr(void);
	virtual ~PgObjectUnitDefMgr(void);

public:
	virtual bool Build(CONT_BASE const &tblBase, CONT_ABIL const &tblAbil);
	virtual bool Build(CONT_BASE const &tblBase, CONT_ABIL const &tblAbil,CONT_DEFSTRINGS const &rkDefString);
	void swap(PgObjectUnitDefMgr &rkRight);

private:
	void Clear();
};

#endif // WEAPON_VARIANT_UNIT_PGOBJECTUNITDEFMGR_H