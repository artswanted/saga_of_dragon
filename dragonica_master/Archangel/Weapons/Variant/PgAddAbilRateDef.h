#ifndef WEAPON_VARIANT_ABIL_PGADDABILRATEDEF_H
#define WEAPON_VARIANT_ABIL_PGADDABILRATEDEF_H

#include "idobject.h"

class PgAddAbilRateDef
	:	public CAbilObject
{
public:
	PgAddAbilRateDef(){};
	virtual ~PgAddAbilRateDef(){};
public:

	int GetAbil(WORD const wAbilType, BYTE const kGrade) const
	{
		WORD wType = wAbilType / 10 * 10;
		int iResult = 0;
		iResult = CAbilObject::GetAbil(wType);
		iResult += CAbilObject::GetAbil(wType+kGrade);
		return iResult;
	}

protected:

};

#endif // WEAPON_VARIANT_ABIL_PGADDABILRATEDEF_H