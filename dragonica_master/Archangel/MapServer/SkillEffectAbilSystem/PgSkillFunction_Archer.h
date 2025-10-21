#ifndef MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGSKILLFUNCTION_ARCHER_H
#define MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGSKILLFUNCTION_ARCHER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 103000301 멋진 언니 
class PgBeautifulGirlSkillFunction : public PgISkillFunction
{
public :
	PgBeautifulGirlSkillFunction() {}
	virtual ~PgBeautifulGirlSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

//SkillNo 110000401 모서리로 찍으세요
class PgBleedingSkillFunction : public PgISkillFunction
{
public :
	PgBleedingSkillFunction() {}
	virtual ~PgBleedingSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};


//SkillNo 1100028011 MP제로 트랩
class PgMPZeroTrapSkillFunction : public PgISkillFunction
{
public :
	PgMPZeroTrapSkillFunction() {}
	virtual ~PgMPZeroTrapSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 150000601 플래시뱅
class PgFlashBangSkillFunction : public PgISkillFunction
{
public :
	PgFlashBangSkillFunction() {}
	virtual ~PgFlashBangSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 150000901 아트로핀
class PgAtropineSkillFunction : public PgISkillFunction
{
public :
	PgAtropineSkillFunction() {}
	virtual ~PgAtropineSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 110001901 멀티샷
class PgMultiShotSkillFunction : public PgISkillFunction
{
public :
	PgMultiShotSkillFunction() {}
	virtual ~PgMultiShotSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 110001501 자동사격장치
class PgAutoShootSystemFunction : public PgDefaultOnlyAddeffectSkillFunction
{
public:
	PgAutoShootSystemFunction(){}
	virtual ~PgAutoShootSystemFunction(){}

public:
	virtual int	 SkillFire( CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult );
};

//SkillNo 150001101 융단폭격
class PgCarpetBombingFunction : public PgISkillFunction
{
	static const float ms_fDistance;
	static const float ms_fDistance2;
public:
	PgCarpetBombingFunction() {}
	virtual ~PgCarpetBombingFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 150001701 발칸사격
class PgValcan300Function :	public PgISkillFunction
{
public:
	PgValcan300Function() {}
	virtual ~PgValcan300Function() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
private:
	int MakeSummonUnit( CUnit *pkUnit, CSkillDef const* pkDefSkill, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_ARCHER_PGSKILLFUNCTION_ARCHER_H