//// Monsters Defining Class
// Dukguru
//

#ifndef WEAPON_VARIANT_UNIT_MONSTERDEF_H
#define WEAPON_VARIANT_UNIT_MONSTERDEF_H

#include <Math.h>
#include "Unit.h"
#include "Lohengrin/dbtables.h"

class CMonsterDef
	:	public CAbilObject
{
public:
	CMonsterDef(){};
	virtual ~CMonsterDef(){};
public:

	virtual int GetAbil(WORD const wAbilType) const;
	virtual __int64 GetAbil64(WORD const wAbilType) const;
	virtual void WriteToPacket(BM::Stream &Packet)const
	{
		Packet.Push(m_mapAbils);
	}
protected:
	CLASS_DECLARATION(int, m_iNo, No);
	CLASS_DECLARATION(int, m_iName, NameNo);
};

class CMonsterTunningDefMgr
{
public:
	CMonsterTunningDefMgr(TBL_DEF_MONSTERTUNNING const * pkDef);
	~CMonsterTunningDefMgr();
	bool FindAbil(int const iAbil, int & iValue) const;

private:
	TBL_DEF_MONSTERTUNNING const *m_pkDef;
	CONT_DEFMONSTERABIL const * m_pkContAbil;
};

#endif // WEAPON_VARIANT_UNIT_MONSTERDEF_H