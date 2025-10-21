#ifndef MAP_MAPSERVER_ACTION_SKILL_PET_PGEFFECTFUNCTION_PET_H
#define MAP_MAPSERVER_ACTION_SKILL_PET_PGEFFECTFUNCTION_PET_H

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include "PgEffectFunction.h"

class PgAutoLootByPetFunction : public PgIEffectFunction 
{
public :
	PgAutoLootByPetFunction() {}
	virtual ~PgAutoLootByPetFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgNurseByPetFunction : public PgIEffectFunction 
{
public :
	PgNurseByPetFunction() {}
	virtual ~PgNurseByPetFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgMetaAblityToPetFunction : public PgIEffectFunction
{
public :
	PgMetaAblityToPetFunction() {}
	virtual ~PgMetaAblityToPetFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgActivateSkillPetFunction : public PgIEffectFunction
{
public:
	PgActivateSkillPetFunction() {}
	virtual ~PgActivateSkillPetFunction () {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_PET_PGEFFECTFUNCTION_PET_H