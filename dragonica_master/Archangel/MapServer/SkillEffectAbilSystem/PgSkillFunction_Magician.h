#ifndef MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGSKILLFUNCTION_MAGICIAN_H
#define MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGSKILLFUNCTION_MAGICIAN_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

//SkillNo 102000201 마녀의 저주 / 바바리안
class PgTransformationSkillFunction : public PgISkillFunction
{
public :
	PgTransformationSkillFunction() {}
	virtual ~PgTransformationSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 102000301 딕텍션
class PgDetectionSkillFunction : public PgISkillFunction
{
public :
	PgDetectionSkillFunction() {}
	virtual ~PgDetectionSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 102000501 힐은 셀프!
class PgHealisSelfSkillFunction : public PgISkillFunction
{
public :
	PgHealisSelfSkillFunction() {}
	virtual ~PgHealisSelfSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 107000301 와이드힐
class PgWideHealSkillFunction : public PgISkillFunction
{
public :
	PgWideHealSkillFunction() {}
	virtual ~PgWideHealSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 107000501 큐어
class PgCureSkillFunction : public PgISkillFunction
{
public :
	PgCureSkillFunction() {}
	virtual ~PgCureSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 109000401 콰그마이어
class PgQuagmireSkillFunction : public PgISkillFunction
{
public :
	PgQuagmireSkillFunction() {}
	virtual ~PgQuagmireSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 1090004011 콰그마이어 설치
class PgQuagmireSetSkillFunction : public PgISkillFunction
{
public :
	PgQuagmireSetSkillFunction() {}
	virtual ~PgQuagmireSetSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};



//SkillNo 109001001 부활
class PgResurrectionSkillFunction : public PgISkillFunction
{
public :
	PgResurrectionSkillFunction() {}
	virtual ~PgResurrectionSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 108000101 블리자드
class PgBlizzardSkillFunction : public PgISkillFunction
{
public :
	PgBlizzardSkillFunction() {}
	virtual ~PgBlizzardSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 109001501 체인라이트닝
class PgChainLightingSkillFunction : public PgISkillFunction
{
public :
	PgChainLightingSkillFunction() {}
	virtual ~PgChainLightingSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 109001601 HP리스토어
class PgHPRestoreSkillFunction : public PgISkillFunction
{
public :
	PgHPRestoreSkillFunction() {}
	virtual ~PgHPRestoreSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 109001701 MP전이
class PgMPTransitionSkillFunction : public PgISkillFunction
{
public :
	PgMPTransitionSkillFunction() {}
	virtual ~PgMPTransitionSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 109002101 리소스 컨버터
class PgResourceConverterSkillFunction : public PgISkillFunction
{
public :
	PgResourceConverterSkillFunction() {}
	virtual ~PgResourceConverterSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);	
};

//SkillNo 102002401 슬로우 힐
class PgSlowHealSkillFunction : public PgISkillFunction
{
public :
	PgSlowHealSkillFunction() {}
	virtual ~PgSlowHealSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

int CalcSlowHealValue(int const iHealPercent, CUnit const* pkUnit);

//SkillNo 109001801 배리어
class PgBarrierSkillFunction : public PgISkillFunction
{
public :
	PgBarrierSkillFunction() {}
	virtual ~PgBarrierSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 109002301 Snow Blues
class PgSnowBluesFunction : public PgISkillFunction
{
public:
	PgSnowBluesFunction() {}
	virtual ~PgSnowBluesFunction() {}

public:
	virtual int	 SkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 2000410101 마그마피스톤
class PgMagmaPistonFunction : public PgISkillFunction
{
public:
	PgMagmaPistonFunction() {}
	virtual ~PgMagmaPistonFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 2000302001 힐링웨이브
class PgHealingWaveFunction: public PgISkillFunction
{
public :
	PgHealingWaveFunction() {}
	virtual ~PgHealingWaveFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_MAGICIAN_PGSKILLFUNCTION_MAGICIAN_H