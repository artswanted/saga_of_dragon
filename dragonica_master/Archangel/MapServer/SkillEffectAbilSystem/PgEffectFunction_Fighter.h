#ifndef MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGEFFECTFUNCTION_FIGHTER_H
#define MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGEFFECTFUNCTION_FIGHTER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//전사 계열 이펙트만 모와 둔 파일
//EffectNo 105500901 / 105501001 / 105501101 / 105501201 성기사 오라류(리커버리/타임/오펜시브/디펜시브 오라)
class PgPaladinAura : public PgIEffectFunction 
{
public :
	PgPaladinAura() {}
	virtual ~PgPaladinAura() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 105300601 Revenge
class PgRevengeEffectFunction : public PgIEffectFunction 
{
public :
	PgRevengeEffectFunction() {}
	virtual ~PgRevengeEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 106300301 Annihilation
class PgAnnihilationEffectFunction : public PgIEffectFunction 
{
public :
	PgAnnihilationEffectFunction() {}
	virtual ~PgAnnihilationEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 105501301 HolyArmor
class PgHolyArmorEffectFunction : public PgIEffectFunction 
{
public :
	PgHolyArmorEffectFunction() {}
	virtual ~PgHolyArmorEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 1055013011 AuraTarget
class PgAuraTargetEffectFunction : public PgDefaultEffectFunction 
{
public :
	PgAuraTargetEffectFunction() {}
	virtual ~PgAuraTargetEffectFunction() {}

public :
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 106501201 a_Bloody Lore
class PgBloodyLoreEffectFunction : public PgIEffectFunction 
{
public :
	PgBloodyLoreEffectFunction() {}
	virtual ~PgBloodyLoreEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 105501801 도발
class PgTauntEffectFunction : public PgIEffectFunction 
{
public :
	PgTauntEffectFunction() {}
	virtual ~PgTauntEffectFunction() {}

public :
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGEFFECTFUNCTION_FIGHTER_H