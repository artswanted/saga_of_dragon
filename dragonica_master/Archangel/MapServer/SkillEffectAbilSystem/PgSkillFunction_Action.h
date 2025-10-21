#ifndef MAP_MAPSERVER_ACTION_SKILL_ACTION_PGSKILLFUNCTION_ACTION_H
#define MAP_MAPSERVER_ACTION_SKILL_ACTION_PGSKILLFUNCTION_ACTION_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//Action_Fire 11101 a_Resurrection_01
class PgResurrection01SkillFunction : public PgISkillFunction
{
public :
	PgResurrection01SkillFunction() {}
	virtual ~PgResurrection01SkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg);
};

//Action_Fire 100005426 a_run
class PgRunSkillFunction : public PgISkillFunction
{
public :
	PgRunSkillFunction() {}
	virtual ~PgRunSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg);
};

//점령전 봄버맨
class PgKOH_area_bomb : public PgISkillFunction
{
public :
	PgKOH_area_bomb() {}
	virtual ~PgKOH_area_bomb() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_ACTION_PGSKILLFUNCTION_ACTION_H