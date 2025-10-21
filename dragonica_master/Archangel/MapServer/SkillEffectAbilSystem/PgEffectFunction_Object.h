#ifndef MAP_MAPSERVER_ACTION_SKILL_OBJECTUNIT_PGEFFECTFUNCTION_OBJECT_H
#define MAP_MAPSERVER_ACTION_SKILL_OBJECTUNIT_PGEFFECTFUNCTION_OBJECT_H

#include "PgEffectFunction.h"

// 회복우물 : 일정틱마다 주위 플레이어의 HP/MP를 회복
class PgRecoveryPoolEffect : public PgIEffectFunction
{
public :
	PgRecoveryPoolEffect() {}
	virtual ~PgRecoveryPoolEffect() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
	virtual int EffectReset(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_OBJECTUNIT_PGEFFECTFUNCTION_OBJECT_H