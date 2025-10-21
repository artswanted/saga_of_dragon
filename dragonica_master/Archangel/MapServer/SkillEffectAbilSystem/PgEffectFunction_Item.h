#ifndef MAP_MAPSERVER_ACTION_SKILL_ITEMEFFECT_PGEFFECTFUNCTION_ITEM_H
#define MAP_MAPSERVER_ACTION_SKILL_ITEMEFFECT_PGEFFECTFUNCTION_ITEM_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//맵에 설치되는 엔티티가 사용하는 이펙트

//! 파렐경 맵 독을 막아주는 방독면 이펙트
class PgGasMaskEffectFunction : public PgIEffectFunction 
{
public :
	PgGasMaskEffectFunction() {}
	virtual ~PgGasMaskEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


//! Tick당 회복시켜주는 포션 아이템
class PgPotionEffectFunction : public PgIEffectFunction 
{
public :
	PgPotionEffectFunction() {}
	virtual ~PgPotionEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


// 틱당 목표 게이지 그룹의 Value를 낮춘다
class PgGaugeDecreaseValEffectFunction : public PgIEffectFunction 
{
public :
	PgGaugeDecreaseValEffectFunction() {}
	virtual ~PgGaugeDecreaseValEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

private:
	//POINT3 m_kPreCasterPos;
};

//! Tick당 회복시켜주는 포션 아이템
class PgRestEffectFunction : public PgIEffectFunction 
{
public :
	PgRestEffectFunction() {}
	virtual ~PgRestEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

//! 사랑의 열기구 이펙트를 실제로 걸어주는 이펙트
class PgLoveBalloonEffectFunction : public PgIEffectFunction 
{
public :
	PgLoveBalloonEffectFunction() {}
	virtual ~PgLoveBalloonEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

class PgCoupleRingEffectFunction : public PgIEffectFunction 
{
public :
	PgCoupleRingEffectFunction() {}
	virtual ~PgCoupleRingEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//! Tick당 회복시켜주는 포션 아이템
class PgRestExpEffectFunction : public PgIEffectFunction 
{
public :
	PgRestExpEffectFunction() {}
	virtual ~PgRestExpEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

//! 점령전 봄버맨
class PgKingOfHill_Bomberman : public PgIEffectFunction 
{
public :
	PgKingOfHill_Bomberman() {}
	virtual ~PgKingOfHill_Bomberman() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

//! 지속적인 디버프 제거기능
class PgFilterExceptEffectFunction : public PgDefaultEffectFunction
{
public :
	PgFilterExceptEffectFunction() {}
	virtual ~PgFilterExceptEffectFunction() {}

public :
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);

};

#endif  // MAP_MAPSERVER_ACTION_SKILL_ITEMEFFECT_PGEFFECTFUNCTION_ITEM_H