#ifndef WEAPON_VARIANT_SKILL_EFFECT_GROUNDEFFECT_H
#define WEAPON_VARIANT_SKILL_EFFECT_GROUNDEFFECT_H

#include "IDObject.h"
#include "Lohengrin/dbtables.h"
#include "ItemDefMgr.h"


class PgGroundEffect
	:	public CAbilObject
{
public:
	PgGroundEffect(){}
	~PgGroundEffect(){}

public:
	CLASS_DECLARATION_S(int, EffectNo);
	CLASS_DECLARATION_S(int, GroundNo);
};

class PgGroundEffectMgr
{
public:
	PgGroundEffectMgr();
	~PgGroundEffectMgr();

public:
	typedef std::vector<PgGroundEffect*> CONT_GROUND_EFFECT_ELEMENT;	
	typedef std::map<int, CONT_GROUND_EFFECT_ELEMENT> CONT_GROUND_EFFECT;
	bool Build( CONT_MAP_EFFECT const &tblMapEffect, CONT_DEFMAP const &tblDefMap );
	void Clear();
	PgGroundEffect const * GetGroundEffect(int const iIndex, int const iGroundNo) const;

protected:
	CONT_GROUND_EFFECT m_kGroundEffect;
	BM::TObjectPool<PgGroundEffect> m_kGroundEffectPool;
};

#endif // WEAPON_VARIANT_SKILL_EFFECT_GROUNDEFFECT_H