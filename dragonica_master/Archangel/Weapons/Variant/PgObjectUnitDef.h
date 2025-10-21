#ifndef WEAPON_VARIANT_UNIT_PGOBJECTUNITDEF_H
#define WEAPON_VARIANT_UNIT_PGOBJECTUNITDEF_H

#include <Math.h>

#include "Unit.h"
#include "Lohengrin/dbtables.h"

class PgObjectUnitDef
	:	public CAbilObject
{
public:
	PgObjectUnitDef():	m_fHeight(0.0f){};
	virtual ~PgObjectUnitDef(){};
public:

	virtual int GetAbil(WORD const wAbilType) const;
	virtual void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(m_iNo);
		kPacket.Push(m_fHeight);
		kPacket.Push(m_mapAbils);
	}

	virtual void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_iNo);
		kPacket.Pop(m_fHeight);
		m_mapAbils.clear();
		kPacket.Pop(m_mapAbils);
	}

protected:
	CLASS_DECLARATION(int, m_iNo, No);
//	CLASS_DECLARATION(int, m_iName, NameNo);
	CLASS_DECLARATION(float, m_fHeight, Height);
};

#endif // WEAPON_VARIANT_UNIT_PGOBJECTUNITDEF_H