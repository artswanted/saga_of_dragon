#ifndef MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGEFFECTFUNCTION_MONSTER_H
#define MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGEFFECTFUNCTION_MONSTER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

#include "PgEffectFunction.h"
#include "Variant/PgActionResult.h"

class PgGround;

//No 2501 Venom
class PgVenomFunction : public PgIEffectFunction 
{
public :
	PgVenomFunction() {}
	virtual ~PgVenomFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 2601 IncreaseSight 시야 증가
class PgIncreaseSightFunction : public PgIEffectFunction 
{
public :
	PgIncreaseSightFunction() {}
	virtual ~PgIncreaseSightFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 6009401 데스마스터 블럭킹
class PgBlockingFunction : public PgIEffectFunction 
{
public :
	PgBlockingFunction() {}
	virtual ~PgBlockingFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
}; 

//No 6013401 넷바인더
class PgNetBinderFunction : public PgIEffectFunction 
{
public :
	PgNetBinderFunction() {}
	virtual ~PgNetBinderFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 6000900 라발론 브레스
class PgLavalonBreathFunction : public PgIEffectFunction 
{
public :
	PgLavalonBreathFunction() {}
	virtual ~PgLavalonBreathFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg) { return ; }
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 6044603 벨라 자기 자신에게 독 버프 걸기
class PgAutoTickEffectFunction : public PgIEffectFunction 
{
public :
	PgAutoTickEffectFunction() {}
	virtual ~PgAutoTickEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 6000914 마력포 리젠시 가지고 나오는 무적 어빌(특수 어빌 값이 다되면 자동으로 삭제 된다./삭제시 특수 어빌값은 빼주지 않는다.)
class PgDependAbilTickEffectFunction : public PgDefaultEffectFunction 
{
public :
	PgDependAbilTickEffectFunction();
	virtual ~PgDependAbilTickEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef);

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);

private:
	WORD m_sDependAbilType;
	int m_iTargetValue;
};

class PgMetaMorphosisEffectFunction : public PgDefaultEffectFunction
{
public:
	PgMetaMorphosisEffectFunction() {}
	virtual ~PgMetaMorphosisEffectFunction() {}
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

class PgHardDungeonEffectFunction : public PgDefaultEffectFunction
{
public:
	PgHardDungeonEffectFunction() {}
	virtual ~PgHardDungeonEffectFunction() {}
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	//EffectEnd는 PgDefaultEffectFunction 이 클래스에서 다 처리 되므로 별도의 처리가 필요 없음.

};

class PgEffectHealToTargetFunction : public PgIEffectFunction 
{
public :
	PgEffectHealToTargetFunction() {}
	virtual ~PgEffectHealToTargetFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);

private:
	void DoAction_Target(EPlayContentsType const eType, PgGround* pkGround, CEffect* pkEffect, CUnit* pkTarget, CUnit* pkCaller, int const iOriginalHP);
	void DoAction_Caller(EPlayContentsType const eType, PgGround* pkGround, CEffect* pkEffect, CUnit* pkCaller);
};

class PgElgaDealingTimeEffectFunction : public PgIEffectFunction 
{
public :
	PgElgaDealingTimeEffectFunction() {}
	virtual ~PgElgaDealingTimeEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgTargetToDeleteEffectFunction : public PgIEffectFunction 
{
public :
	PgTargetToDeleteEffectFunction() {}
	virtual ~PgTargetToDeleteEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgRangeBySkillFireEffectFunction : public PgIEffectFunction 
{
public :
	PgRangeBySkillFireEffectFunction() {}
	virtual ~PgRangeBySkillFireEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgDistanceWallFunction : public PgIEffectFunction 
{
public :
	PgDistanceWallFunction() {}
	virtual ~PgDistanceWallFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgRandomAttachEffectFunction : public PgIEffectFunction 
{
public :
	PgRandomAttachEffectFunction() {}
	virtual ~PgRandomAttachEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

class PgSummonEntityEffectFunction : public PgIEffectFunction 
{
public :
	PgSummonEntityEffectFunction() {}
	virtual ~PgSummonEntityEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

class PgChangeAIEffectFunction : public PgIEffectFunction 
{
public :
	PgChangeAIEffectFunction() {}
	virtual ~PgChangeAIEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGEFFECTFUNCTION_MONSTER_H