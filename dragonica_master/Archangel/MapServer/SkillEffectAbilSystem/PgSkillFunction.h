#ifndef MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLFUNCTION_H
#define MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLFUNCTION_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

//Interface Class
class PgISkillFunction
{
public :
	PgISkillFunction();
	virtual ~PgISkillFunction();

public :
	virtual void Init();
	virtual void Release();

	virtual void Build();

	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillToggle	(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillFail		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	
	// Passive용
	virtual int	 SkillPassive	(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg);
	virtual int	 SkillPCheck	(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillBegin		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillEnd		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);

	// Action_Fire용
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg);

	void		 CheckTagetVaild(CUnit* pkUnit, UNIT_PTR_ARRAY* pkUnitArray, CSkillDef const* pkSkill = NULL, int const iDmgCheckTIme = -1); //Target이 유요한지 검사
};

//DefaultSkilll - DoFinalDamage / AddEffect
class PgDefaultSkillFunction : public PgISkillFunction
{
public :
	PgDefaultSkillFunction();
	virtual ~PgDefaultSkillFunction();

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillToggle	(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	
	// Passive	
	virtual int	 SkillPassive	(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg);
	virtual int	 SkillPCheck	(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillBegin		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillEnd		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
};

// Target Only AddEffect
class PgDefaultOnlyAddeffectSkillFunction : public PgISkillFunction
{
public :
	PgDefaultOnlyAddeffectSkillFunction() {}
	virtual ~PgDefaultOnlyAddeffectSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Target Only Damage
class PgDefaultOnlyDamageSkillFunction : public PgISkillFunction
{
public :
	PgDefaultOnlyDamageSkillFunction() {}
	virtual ~PgDefaultOnlyDamageSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillToggle	(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Target Only Damage Monster
class PgDefaultOnlyDamageMonSkillFunction : public PgISkillFunction
{
public :
	PgDefaultOnlyDamageMonSkillFunction() {}
	virtual ~PgDefaultOnlyDamageMonSkillFunction() {}

public :
	virtual void Build() {}

	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Target Only AddEffect
class PgDefaultOnlyAddeffectMonSkillFunction : public PgISkillFunction
{
public :
	PgDefaultOnlyAddeffectMonSkillFunction() {}
	virtual ~PgDefaultOnlyAddeffectMonSkillFunction() {}

public :
	virtual void Build() {}

	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Target Damage and Effect Monster
class PgDefaultDamageAndEffectSkillFunction : public PgISkillFunction
{
public :
	PgDefaultDamageAndEffectSkillFunction() {}
	virtual ~PgDefaultDamageAndEffectSkillFunction() {}

public :
	virtual void Build() {}

	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

	int DoAction(CUnit * pkUnit, CSkillDef const* pkSkill, SActArg const* pArg, UNIT_PTR_ARRAY * pkUnitArray, PgActionResultVector * pkResult);
};

// Begin때 Caster에 DefSkill의 EffectID를, Fire때 Target에 데미지와 Abil의 Effect를 적용
class PgDefaultDamageAndEffectSkillFunction2 : public PgDefaultDamageAndEffectSkillFunction
{
public :
	PgDefaultDamageAndEffectSkillFunction2() {}
	virtual ~PgDefaultDamageAndEffectSkillFunction2() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Fire때 Target이 없어도 시전자에게 이펙트 적용
class PgDefaultDamageAndEffectSkillFunction3 : public PgDefaultDamageAndEffectSkillFunction
{
public :
	PgDefaultDamageAndEffectSkillFunction3() {}
	virtual ~PgDefaultDamageAndEffectSkillFunction3() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// Caster에 DefSkill의 EffectID를, Target에 Abil의 Effect를 적용
class PgDefaultOnly_Add_EffectSkillFunction : public PgISkillFunction
{
public :
	PgDefaultOnly_Add_EffectSkillFunction() {}
	virtual ~PgDefaultOnly_Add_EffectSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgRePositionFunction : public PgISkillFunction
{
public:
	static Direction const ms_eDir[8];

public:
	PgRePositionFunction() {}
	virtual ~PgRePositionFunction() {}

public:
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgCoupleVicinityEffectFunction : public PgDefaultSkillFunction
{// 패시브
public :
	PgCoupleVicinityEffectFunction() {}
	virtual ~PgCoupleVicinityEffectFunction() {}

	virtual void	 SkillBegin(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLFUNCTION_H