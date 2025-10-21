#ifndef PGSKILLFUNTION_SUMMONER_H
#define PGSKILLFUNTION_SUMMONER_H

#include "PgSkillFunction.h"

class PgGround;

//SkillNo 150001701 발칸사격
class PgSummonFunction :	public PgISkillFunction
{
public:
	PgSummonFunction() {}
	virtual ~PgSummonFunction() {}

public:
	virtual int SkillFire( CUnit *pUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pUnitArray, PgActionResultVector* pResult);
};


//소환체 전체를 없애고 남은 체력을 마나로 환원해줌
class PgReGainSummonedSkillFunction : public PgISkillFunction
{
public :
	PgReGainSummonedSkillFunction() {}
	virtual ~PgReGainSummonedSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

private:
	bool DoAction(CUnit * pkSummoned, int const iDieSkillNo, int & iAddHp, int & iAddMp);
};

//소환체를 유닛 주변으로 순간이동
class PgReCallSummonedSkillFunction : public PgISkillFunction
{
public :
	PgReCallSummonedSkillFunction() {}
	virtual ~PgReCallSummonedSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

private:
	bool DoAction(PgGround const * pkGround, CSkillDef const* pkSkillDef, CUnit const * pkCaller, CUnit* pkSummoned, SActArg const* pArg);
};

//소환체에게 이펙트 적용
class PgEffectTargetSummonedSkillFunction : public PgISkillFunction
{
public :
	PgEffectTargetSummonedSkillFunction() {}
	virtual ~PgEffectTargetSummonedSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

private:
	bool DoAction(SActArg const* pArg, CSkillDef const* pkSkillDef, CUnit * pkCaller, CUnit * pkSummoned);
};

//소환체에게 특정 몹을 집중 공격
class PgAttackSummonedSkillFunction : public PgISkillFunction
{
public :
	PgAttackSummonedSkillFunction() {}
	virtual ~PgAttackSummonedSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

private:
	bool DoAction(CUnit * pkCaller, CUnit * pkSummoned, CUnit * pkMonster, SActArg const* pArg, CSkillDef const* pkSkillDef);
};
#endif