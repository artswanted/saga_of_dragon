#ifndef MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLONFINDTARGET_H
#define MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLONFINDTARGET_H

#include <map>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

class BM::Stream;
class CSkillDef;
class PgActionResultVector;
class PgGround;

extern bool CheckTargetAngle(SActionInfo const &rkAction, ESkillArea const eArea, POINT3 const &rkTargetPos, float const fErrorDelta = 20.0f);

extern size_t DefaultOnFindTarget(BM::Stream * const pkPacket, const CSkillDef *pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, int const iSkillRange2, ESkillArea const eArea, PgGround* pkGround, SActionInfo const &rkAction, UNIT_PTR_ARRAY &rkArray, PgActionResultVector &rkActionResultVec);

extern int CalcRangeCheckMutiflier(int const iRange);

class PgISkillOnFindTarget
{
public:
	PgISkillOnFindTarget() {}
	virtual ~PgISkillOnFindTarget() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec) =  0;
};

class PgSkillOnFindTarget : public PgISkillOnFindTarget
{
public:
	PgSkillOnFindTarget() {}
	virtual ~PgSkillOnFindTarget() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
};

class PgSkillOnFindTargetPointBurst : public PgISkillOnFindTarget
{
public:
	PgSkillOnFindTargetPointBurst() {}
	virtual ~PgSkillOnFindTargetPointBurst() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
};

class PgSkillOnFindTargetStormBlade : public PgISkillOnFindTarget
{
public:
	PgSkillOnFindTargetStormBlade() {}
	virtual ~PgSkillOnFindTargetStormBlade() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
};

class PgSkillOnFindTargetChainLight : public PgISkillOnFindTarget
{
public:
	PgSkillOnFindTargetChainLight() {}
	virtual ~PgSkillOnFindTargetChainLight() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
};

class PgSkillOnFindTargetSummoned : public PgISkillOnFindTarget
{
public:
	PgSkillOnFindTargetSummoned() {}
	virtual ~PgSkillOnFindTargetSummoned() {}
	virtual bool OnFindTarget(BM::Stream * const pkPacket, CSkillDef const* pkDef, CUnit* pkUnit, BYTE const byTargetNum, int const iSkillRange, PgGround* pkGround, SActionInfo const& rkAction, UNIT_PTR_ARRAY& rkArray, PgActionResultVector& rkActionResultVec);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGSKILLONFINDTARGET_H