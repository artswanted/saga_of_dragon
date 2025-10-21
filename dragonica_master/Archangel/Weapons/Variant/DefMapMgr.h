#ifndef WEAPON_VARIANT_BASIC_GROUND_DEFMAPMGR_H
#define WEAPON_VARIANT_BASIC_GROUND_DEFMAPMGR_H

#include "Lohengrin/dbtables.h"
#include "idobject.h"
#include "TDefMgr.h"

class PgMapDef
	: public CAbilObject
{
public:
	PgMapDef() {}
	virtual ~PgMapDef() {}

protected:
	CLASS_DECLARATION_S(int, MapNo);
};


class PgDefMapMgr
	: public TDefMgr< TBL_DEF_MAP, TBL_DEF_MAP_ABIL, PgMapDef, TBL_KEY_INT, TBL_KEY_INT>
{
public:
	PgDefMapMgr(){}
	virtual ~PgDefMapMgr(){}

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	int GetAbil(int const iMapNo, WORD const wType) const;
};

#endif // WEAPON_VARIANT_BASIC_GROUND_DEFMAPMGR_H