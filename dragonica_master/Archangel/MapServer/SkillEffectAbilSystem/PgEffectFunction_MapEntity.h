#ifndef MAP_MAPSERVER_ACTION_SKILL_MAPENTITY_PGEFFECTFUNCTION_MAPENTITY_H
#define MAP_MAPSERVER_ACTION_SKILL_MAPENTITY_PGEFFECTFUNCTION_MAPENTITY_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

#include "PgEffectFunction.h"

class PgAbilTypeTable;

//맵에 설치되는 엔티티가 사용하는 이펙트

//No 9030500 PgDontJumpFunction 점프 못하게
class PgDontJumpFunction : public PgIEffectFunction 
{
public :
	PgDontJumpFunction() {}
	virtual ~PgDontJumpFunction() {}

public :
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

//! 파렐경 맵에 엔티티가 사용하는 독거는 이펙트
class PgParelPoisonAura : public PgIEffectFunction 
{
public :
	PgParelPoisonAura() {}
	virtual ~PgParelPoisonAura() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

class PgParelPoison : public PgIEffectFunction
{
public :
	PgParelPoison() {}
	virtual ~PgParelPoison() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

class PgChaosMapEffect : public  PgDefaultEffectFunction
{
public :
	PgChaosMapEffect() {}
	virtual ~PgChaosMapEffect() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

};

// 카오스 맵에 있는 유저들을 찾아서 해독 버프가 없으면 Chaos 버프를 걸어준다.
class PgChaosMapAuraEffect : public  PgIEffectFunction
{
public :
	PgChaosMapAuraEffect() {}
	virtual ~PgChaosMapAuraEffect() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

};

// 엘가 심장의 방 엔티티가 사용하는 이펙트
class PgElga_room_of_heart : public  PgDefaultEffectFunction
{
public :
	PgElga_room_of_heart() {}
	virtual ~PgElga_room_of_heart() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

};

// 플레이어 더미 위치로 강제 이동
class PgTeleportToDummyEffect : public  PgDefaultEffectFunction
{
public :
	PgTeleportToDummyEffect() {}
	virtual ~PgTeleportToDummyEffect() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

};

// 점령전 가디언의 범위안 플레이어 점수 계산되는 이펙트
class PgKingOfHill_Guardian : public  PgDefaultEffectFunction
{
public :
	PgKingOfHill_Guardian() {}
	virtual ~PgKingOfHill_Guardian() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

};

//! 지역 적용 이펙트
class PgEffectApplyArea : public PgIEffectFunction 
{
public :
	PgEffectApplyArea() {}
	virtual ~PgEffectApplyArea() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_MAPENTITY_PGEFFECTFUNCTION_MAPENTITY_H