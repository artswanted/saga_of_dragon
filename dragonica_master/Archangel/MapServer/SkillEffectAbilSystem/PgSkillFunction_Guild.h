#ifndef MAP_MAPSERVER_ACTION_SKILL_GUILD_PGSKILLFUNCTION_GUILD_H
#define MAP_MAPSERVER_ACTION_SKILL_GUILD_PGSKILLFUNCTION_GUILD_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 90000001 CoupleLover 
class PgCoupleLoverSkillFunction : public PgISkillFunction
{
public :
	PgCoupleLoverSkillFunction() {}
	virtual ~PgCoupleLoverSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_GUILD_PGSKILLFUNCTION_GUILD_H