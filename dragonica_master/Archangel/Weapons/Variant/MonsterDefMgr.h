#ifndef WEAPON_VARIANT_UNIT_MONSTERDEFMGR_H
#define WEAPON_VARIANT_UNIT_MONSTERDEFMGR_H

#include "BM/ObjectPool.h"
#include "Lohengrin/ProcessConfig.h"

#include "MonsterDef.h"
#include "TDefMgr.h"

typedef std::map< int, CMonsterDef* > CONT_MONSTER;


class CMonsterDefMgr
	:	public TDefMgr< TBL_DEF_MONSTER, TBL_DEF_MONSTERABIL, CMonsterDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< CMonsterDefMgr >;
public:

	CMonsterDefMgr(void);
	virtual ~CMonsterDefMgr(void);

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil,const CONT_DEFSTRINGS& rkDefString);
	void swap(CMonsterDefMgr &rkRight);

private:
	void Clear();
	CONT_DEF_MONSTER_BAG	m_kContDefMonsterBag;

};

//#define g_MonsterDef SINGLETON_STATIC(CMonsterDefMgr)

#endif // WEAPON_VARIANT_UNIT_MONSTERDEFMGR_H