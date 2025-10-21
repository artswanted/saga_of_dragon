#ifndef MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_STATUSEFFEC_H
#define MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_STATUSEFFEC_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include "PgEffectFunction.h"

class PgAbilTypeTable;

//상태이상 이펙트만 모와 둔 파일

//EffectNo 101020101 기절
class PgStunStatusFunction : public PgIEffectFunction 
{
public :
	PgStunStatusFunction() {}
	virtual ~PgStunStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//엘가 기절
class PgElgaStunStatusFunction : public PgStunStatusFunction 
{
public :
	PgElgaStunStatusFunction() {}
	virtual ~PgElgaStunStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010001 출혈 / 과다 출혈
class PgBleedStatusFunction : public PgIEffectFunction 
{
public :
	PgBleedStatusFunction() {}
	virtual ~PgBleedStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010201 저주
class PgCurseStatusFunction : public PgIEffectFunction 
{
public :
	PgCurseStatusFunction() {}
	virtual ~PgCurseStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010401 결빙 / 동빙
class PgColdStatusFunction : public PgIEffectFunction 
{
public :
	PgColdStatusFunction() {}
	virtual ~PgColdStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010501 침묵
class PgSilenceStatusFunction : public PgIEffectFunction 
{
public :
	PgSilenceStatusFunction() {}
	virtual ~PgSilenceStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010601 중독 / 심각한 중독
class PgPoisonStatusFunction : public PgIEffectFunction 
{
public :
	PgPoisonStatusFunction() {}
	virtual ~PgPoisonStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100010801 수면
class PgSleepStatusFunction : public PgIEffectFunction 
{
public :
	PgSleepStatusFunction() {}
	virtual ~PgSleepStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


//EffectNo 100010901 석화
class PgFleshtoStoneStatusFunction : public PgIEffectFunction 
{
public :
	PgFleshtoStoneStatusFunction() {}
	virtual ~PgFleshtoStoneStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};


//EffectNo 100011001 화상 / 심각한 화상
class PgBurnStatusFunction : public PgIEffectFunction 
{
public :
	PgBurnStatusFunction() {}
	virtual ~PgBurnStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100011201 실명
class PgBlindStatusFunction : public PgIEffectFunction 
{
public :
	PgBlindStatusFunction() {}
	virtual ~PgBlindStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//EffectNo 100011301 슬로우
class PgSlowStatusFunction : public PgIEffectFunction 
{
public :
	PgSlowStatusFunction() {}
	virtual ~PgSlowStatusFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 11001 미션 페널티
class PgMissionPenaltyFunction : public PgIEffectFunction 
{
public :
	PgMissionPenaltyFunction() {}
	virtual ~PgMissionPenaltyFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 11201 부활 무적
class PgCannotDamageFunction : public PgIEffectFunction 
{
public :
	PgCannotDamageFunction() {}
	virtual ~PgCannotDamageFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 12001 아이템 이펙트
class PgItemEffectFunction : public PgIEffectFunction 
{
public :
	PgItemEffectFunction() {}
	virtual ~PgItemEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 12101 병아리 안전모
class PgSafetyCapFunction : public PgIEffectFunction 
{
public :
	PgSafetyCapFunction() {}
	virtual ~PgSafetyCapFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 13001 PVP 방어력 보정
class PgDefenceCorrectionFunction : public PgIEffectFunction 
{
public :
	PgDefenceCorrectionFunction() {}
	virtual ~PgDefenceCorrectionFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 90000001 사랑의 힘으로
class PgLovePowerFunction : public PgIEffectFunction 
{
public :
	PgLovePowerFunction() {}
	virtual ~PgLovePowerFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *pkAbilTable, CEffectDef const *pkDef) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 531301 소생
class PgReviveEffectFunction : public PgIEffectFunction 
{
public :
	PgReviveEffectFunction() {}
	virtual ~PgReviveEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//No 531401 이펙트 유지시간 증감
class PgAddEffectDurationTimeFunction : public PgIEffectFunction 
{
public :
	PgAddEffectDurationTimeFunction() {}
	virtual ~PgAddEffectDurationTimeFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};
#endif // MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_STATUSEFFEC_H