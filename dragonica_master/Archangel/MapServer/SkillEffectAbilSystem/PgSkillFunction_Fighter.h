#ifndef MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGSKILLFUNCTION_FIGHTER_H
#define MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGSKILLFUNCTION_FIGHTER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 101000101 Hammer Crush 
class PgHammerCrushSkillFunction : public PgISkillFunction
{
public :
	PgHammerCrushSkillFunction() {}
	virtual ~PgHammerCrushSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 101000701 Armor Break
class PgArmorBreakSkillFunction : public PgISkillFunction
{
public :
	PgArmorBreakSkillFunction() {}
	virtual ~PgArmorBreakSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 101000801 Storm blade
class PgStormbladeSkillFunction : public PgISkillFunction
{
public :
	PgStormbladeSkillFunction() {}
	virtual ~PgStormbladeSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 101300101 a_Sword Mastery
class PgSwordMasterySkillFunction : public PgISkillFunction
{
public :
	PgSwordMasterySkillFunction() {}
	virtual ~PgSwordMasterySkillFunction() {}

public :
	// Passive	
	virtual int	 SkillPassive	(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg);
	virtual int	 SkillPCheck	(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillBegin		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	virtual void SkillEnd		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
};

//Skill 106000101 a_Roulette
class PgRouletteSkillFunction : public PgISkillFunction
{
public :
	PgRouletteSkillFunction() {}
	virtual ~PgRouletteSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//Skill 106300401 a_Stumblebum
class PgStumblebumSkillFunction : public PgISkillFunction
{
public :
	PgStumblebumSkillFunction() {}
	virtual ~PgStumblebumSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//Skill 105300501 a_Joint Break
class PgJointBreakSkillFunction : public PgISkillFunction
{
public :
	PgJointBreakSkillFunction() {}
	virtual ~PgJointBreakSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//Skill 105501601 a_Bugs Life
class PgBugsLifeSkillFunction : public PgISkillFunction
{
public :
	PgBugsLifeSkillFunction() {}
	virtual ~PgBugsLifeSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_FIGHTER_PGSKILLFUNCTION_FIGHTER_H