#ifndef MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGEFFECTFUNCTION_MAGICIAN_H
#define MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGEFFECTFUNCTION_MAGICIAN_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//EffectNo 102000201 마녀의저주
class PgTransformationEffectFunction : public PgIEffectFunction 
{
public :
	PgTransformationEffectFunction() {}
	virtual ~PgTransformationEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 102000401 바바리안
class PgBurberrianEffectFunction : public PgIEffectFunction 
{
public :
	PgBurberrianEffectFunction() {}
	virtual ~PgBurberrianEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 102000701 마나실드
class PgManaShieldEffectFunction : public PgIEffectFunction 
{
public :
	PgManaShieldEffectFunction() {}
	virtual ~PgManaShieldEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 102000801 정신일도하사불성
class PgConcentrationEffectFunction : public PgIEffectFunction 
{
public :
	PgConcentrationEffectFunction() {}
	virtual ~PgConcentrationEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 107000701 감염
class PgDiseaseEffectFunction : public PgIEffectFunction 
{
public :
	PgDiseaseEffectFunction() {}
	virtual ~PgDiseaseEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 109002001 미러 이미지
class PgMirrorImageEffectFunction : public PgIEffectFunction 
{
public :
	PgMirrorImageEffectFunction() {}
	virtual ~PgMirrorImageEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 109001401 썬더 브레이크
class PgThunderBreakEffectFunction : public PgIEffectFunction 
{
public :
	PgThunderBreakEffectFunction() {}
	virtual ~PgThunderBreakEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


//EffectNo 109001501 체인라이트닝
class PgChainLightingEffectFunction : public PgIEffectFunction 
{
public :
	PgChainLightingEffectFunction() {}
	virtual ~PgChainLightingEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 109001601 HP리스토어
class PgHPRestoreEffectFunction : public PgIEffectFunction 
{
public :
	PgHPRestoreEffectFunction() {}
	virtual ~PgHPRestoreEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 109001801 배리어
class PgBarrierEffectFunction : public PgIEffectFunction 
{
public :
	PgBarrierEffectFunction() {}
	virtual ~PgBarrierEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//
//EffectNo 1090028011 생츄어리 설치
class PgSantuaryEffectFunction : public PgIEffectFunction 
{
public :
	PgSantuaryEffectFunction() {}
	virtual ~PgSantuaryEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 107000501 큐어
class PgCureEffectFunction : public PgIEffectFunction 
{
public :
	PgCureEffectFunction() {}
	virtual ~PgCureEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGEFFECTFUNCTION_MAGICIAN_H