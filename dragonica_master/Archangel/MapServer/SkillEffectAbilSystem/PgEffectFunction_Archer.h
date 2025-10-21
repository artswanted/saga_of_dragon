#ifndef MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGEFFECTFUNCTION_ARCHER_H
#define MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGEFFECTFUNCTION_ARCHER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//궁수 계열 이펙트만 모와 둔 파일

//EffectNo 103000301 멋진 언니
class PgBeautifulGirlFunction : public PgIEffectFunction 
{
public :
	PgBeautifulGirlFunction() {}
	virtual ~PgBeautifulGirlFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 103000401 버드 워칭
class PgIncreasedStatusFunction : public PgIEffectFunction 
{
public :
	PgIncreasedStatusFunction() {}
	virtual ~PgIncreasedStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 103300101 보우마스터리
class PgBowMasteryFunction : public PgIEffectFunction 
{
public :
	PgBowMasteryFunction() {}
	virtual ~PgBowMasteryFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 110000401 모서리로 찍으세요
class PgBleedingFunction : public PgIEffectFunction 
{
public :
	PgBleedingFunction() {}
	virtual ~PgBleedingFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 150000901 아트로핀 발동
class PgAtropineFireFunction : public PgIEffectFunction 
{
public :
	PgAtropineFireFunction() {}
	virtual ~PgAtropineFireFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

//EffectNo 150000901 아트로핀 효과
class PgAtropineEffectFunction : public PgIEffectFunction 
{
public :
	PgAtropineEffectFunction() {}
	virtual ~PgAtropineEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

//EffectNo 110002501 건들지마라
class PgDonTouchMeFunction : public PgIEffectFunction 
{
public :
	PgDonTouchMeFunction() {}
	virtual ~PgDonTouchMeFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

//EffectNo 110001701 카모플라쥬
class PgCamouflageEffectFunction : public PgIEffectFunction 
{
public :
	PgCamouflageEffectFunction() {}
	virtual ~PgCamouflageEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 1500008011 WP그레네이드 투척 a_WP Granade Fire
class PgWPGranadeFireEffectFunction : public PgIEffectFunction 
{
public :
	PgWPGranadeFireEffectFunction() {}
	virtual ~PgWPGranadeFireEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 1100029011 스모크 그레네이드 발동
class PgSmokeGranadeActiveEffectFunction : public PgIEffectFunction
{
public :
	PgSmokeGranadeActiveEffectFunction() {}
	virtual ~PgSmokeGranadeActiveEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 110001001 애시드 애로우 a_Acid Arrow
class PgAcidArrowEffectFunction : public PgIEffectFunction
{
public :
	PgAcidArrowEffectFunction() {}
	virtual ~PgAcidArrowEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


#endif // MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGEFFECTFUNCTION_ARCHER_H