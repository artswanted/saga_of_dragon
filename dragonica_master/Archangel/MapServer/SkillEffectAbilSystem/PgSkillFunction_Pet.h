#ifndef MAP_MAPSERVER_ACTION_SKILL_PET_PGSKILLFUNCTION_PET_H
#define MAP_MAPSERVER_ACTION_SKILL_PET_PGSKILLFUNCTION_PET_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 80000701 골드 러쉬
class PgGoldRushSkillFunction : public PgISkillFunction
{
public :
	PgGoldRushSkillFunction() {}
	virtual ~PgGoldRushSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgGround;
class CSkillDef;
class PgPetProduceItem : public PgDefaultOnlyAddeffectSkillFunction
{
public:
	PgPetProduceItem() {}
	virtual ~PgPetProduceItem() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

protected:
	bool ProduceItem(CUnit* pkCaster, CUnit* pkTargetUnit, CSkillDef const* pkSkillDef, PgGround* pkGround);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_PET_PGSKILLFUNCTION_PET_H