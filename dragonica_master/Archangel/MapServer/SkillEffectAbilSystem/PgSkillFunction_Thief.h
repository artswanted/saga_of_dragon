#ifndef MAP_MAPSERVER_ACTION_SKILL_THIEF_PGSKILLFUNCTION_THIEF_H
#define MAP_MAPSERVER_ACTION_SKILL_THIEF_PGSKILLFUNCTION_THIEF_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 104300201 스트립 웨폰
class PgStripWeaponSkillFunction : public PgISkillFunction
{
public :
	PgStripWeaponSkillFunction() {}
	virtual ~PgStripWeaponSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgThiefDefaultHitSkillFunction : public PgISkillFunction
{
public :
	PgThiefDefaultHitSkillFunction() {}
	virtual ~PgThiefDefaultHitSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgThiefDefault24HitSkillFunction : public PgISkillFunction
{
public :
	PgThiefDefault24HitSkillFunction() {}
	virtual ~PgThiefDefault24HitSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 170000301 미혼향 a_Sleep Smell
class PgSleepSmellSkillFunction : public PgISkillFunction
{
public :
	PgSleepSmellSkillFunction() {}
	virtual ~PgSleepSmellSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 160000501 기합 a_Shout
class PgShoutSkillFunction : public PgISkillFunction
{
public :
	PgShoutSkillFunction() {}
	virtual ~PgShoutSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 104302101 덱스터리티 / 바이탈리티 / 인텔리젼스 / 하이퍼스트렝스
class PgDexteritySkillFunction : public PgISkillFunction
{
public :
	PgDexteritySkillFunction() {}
	virtual ~PgDexteritySkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};


class PgPhantomCloneSkillFunction : public PgISkillFunction
{
public :
	PgPhantomCloneSkillFunction() {}
	virtual ~PgPhantomCloneSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 2000820201 쉐도우블레이드 닌자 소환
class PgShadowBladeFunction : public PgISkillFunction
{
public:
	PgShadowBladeFunction() {}
	virtual ~PgShadowBladeFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_THIEF_PGSKILLFUNCTION_THIEF_H