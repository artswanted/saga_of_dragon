#ifndef MAP_MAPSERVER_ACTION_SKILL_THIEF_PGEFFECTFUNCTION_THIEF_H
#define MAP_MAPSERVER_ACTION_SKILL_THIEF_PGEFFECTFUNCTION_THIEF_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//도둑 계열 이펙트만 모와 둔 파일

//EffectNo 104300201 스트립 웨폰
class PgStripWeaponEffectFunction : public PgIEffectFunction 
{
public :
	PgStripWeaponEffectFunction() {}
	virtual ~PgStripWeaponEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 104000601 버로우
class PgBurrowEffectFunction : public PgIEffectFunction 
{
public :
	PgBurrowEffectFunction() {}
	virtual ~PgBurrowEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 104000201 비상탈출
class PgExitEffectFunction : public PgIEffectFunction 
{
public :
	PgExitEffectFunction() {}
	virtual ~PgExitEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 104301301 쉐도우 워크
class PgShadowWalkEffectFunction : public PgIEffectFunction 
{
public :
	PgShadowWalkEffectFunction() {}
	virtual ~PgShadowWalkEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 170001101 닌자 변신
class PgChangeNinjaEffectFunction : public PgDefaultEffectFunction 
{
public :
	PgChangeNinjaEffectFunction() {}
	virtual ~PgChangeNinjaEffectFunction() {}

public :
	
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};
//EffectNo 2000802001 프로텍트엣지
class PgProtectEdgeEffectFunction : public PgIEffectFunction 
{
public :
	PgProtectEdgeEffectFunction() {}
	virtual ~PgProtectEdgeEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
	bool CalcParamValue(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg,int& riOutPercent, int& riMinDmg, int& riMaxDmg);
	bool GetParamFromEffect(CEffect const* pkEffect, int& riDmgType, int& riPercent);
	bool GetParamFromEffect(CEffectDef const* pkEffect, int& riDmgType, int& riPercent);
};

//EffectNo 
class PgHPMPRestoreEffectFunction : public PgIEffectFunction 
{
public :
	PgHPMPRestoreEffectFunction() {}
	virtual ~PgHPMPRestoreEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_THIEF_PGEFFECTFUNCTION_THIEF_H