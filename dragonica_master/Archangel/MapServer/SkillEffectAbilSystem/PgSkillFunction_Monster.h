#ifndef MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGSKILLFUNCTION_MONSTER_H
#define MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGSKILLFUNCTION_MONSTER_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgActionResultVector;
struct UNIT_PTR_ARRAY;

#include "PgSkillFunction.h"

int DefaultMonsterSkillFire(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult, bool bEffect = false);

//SkillNo 142 몬스터 타겟 링크
class PgTargetLinkSkillFunction : public PgISkillFunction
{
public :
	PgTargetLinkSkillFunction() {}
	virtual ~PgTargetLinkSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 143 몬스터 타겟 링크(내 타겟 있을땐 무시)
class PgTargetLink2SkillFunction : public PgISkillFunction
{
public :
	PgTargetLink2SkillFunction() {}
	virtual ~PgTargetLink2SkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6003101 데미지+이펙트
class PgDamageAndEffectSkillFunction : public PgISkillFunction
{
public :
	PgDamageAndEffectSkillFunction() {}
	virtual ~PgDamageAndEffectSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6004201 돌진 후 공격
class PgDashAfterAttackSkillFunction : public PgISkillFunction
{
public :
	PgDashAfterAttackSkillFunction() {}
	virtual ~PgDashAfterAttackSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

//SkillNo 6004904 일반 돌진 공격
class PgDashAttackSkillFunction : public PgISkillFunction
{
public :
	PgDashAttackSkillFunction() {}
	virtual ~PgDashAttackSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6009602 데스마스터 기모았다 발산
class PgEnergyExplosionSkillFunction : public PgISkillFunction
{
public :
	PgEnergyExplosionSkillFunction() {}
	virtual ~PgEnergyExplosionSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

//SkillNo 6009604 데스마스터 대지의 분노
class PgEarthQuakeSkillFunction : public PgISkillFunction
{
public :
	PgEarthQuakeSkillFunction() {}
	virtual ~PgEarthQuakeSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6013904 힐
class PgMonsterHealSkillFunction : public PgISkillFunction
{
public :
	PgMonsterHealSkillFunction() {}
	virtual ~PgMonsterHealSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6013905 단체힐
class PgMonsterMassiveHealSkillFunction : public PgISkillFunction
{
public :
	PgMonsterMassiveHealSkillFunction() {}
	virtual ~PgMonsterMassiveHealSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6018904 우마이 바닥치기
class PgSummonThornSkillFunction : public PgISkillFunction
{
public :
	PgSummonThornSkillFunction() {}
	virtual ~PgSummonThornSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6000932 엔티티 여러개 소환
class PgSummonThornsSkillFunction : public PgISkillFunction
{
public :
	PgSummonThornsSkillFunction() {}
	virtual ~PgSummonThornsSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6019004 몬스터가 바라보는 방향으로 SkillRange 위치에 엔티티 소환
class PgSummonEntitySkillFunction : public PgISkillFunction
{
public :
	PgSummonEntitySkillFunction() {}
	virtual ~PgSummonEntitySkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 3531110 엔티티 소환(클라에 위치정보 패킷 전달)
class PgSummonShotEntitySkillFunction : public PgISkillFunction
{
public :
	PgSummonShotEntitySkillFunction() {}
	virtual ~PgSummonShotEntitySkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6025904 벌룬 자코 소환
class PgSummonMonsterSkillFunction : public PgISkillFunction
{
public :
	PgSummonMonsterSkillFunction() {}
	virtual ~PgSummonMonsterSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 3542219 랜덤위치에 몬스터 소환
class PgRandomSummonMonsterSkillFunction : public PgISkillFunction
{
public :
	PgRandomSummonMonsterSkillFunction() {}
	virtual ~PgRandomSummonMonsterSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6053100 자폭공격스킬
class PgKamikazeSkillFunction : public PgISkillFunction
{
public :
	PgKamikazeSkillFunction() {}
	virtual ~PgKamikazeSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 60911031 엔터티 자폭공격스킬
class PgEntityKamikazeSkillFunction : public PgISkillFunction
{
public :
	PgEntityKamikazeSkillFunction() {}
	virtual ~PgEntityKamikazeSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6000903 라발론 화염폭풍
class PgLavalonBlazeFunction : public PgISkillFunction
{
public :
	PgLavalonBlazeFunction() {}
	virtual ~PgLavalonBlazeFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6033201 라발론 메테오
class PgLavalonMeteorFunction : public PgISkillFunction
{
public :
	PgLavalonMeteorFunction() {}
	virtual ~PgLavalonMeteorFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6033202 라발론 파란색 메테오
class PgLavalonMeteorBlueFunction : public PgISkillFunction
{
public :
	PgLavalonMeteorBlueFunction() {}
	virtual ~PgLavalonMeteorBlueFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6000900 브레스
class PgLavalonBreathSkillFunction : public PgISkillFunction
{
public :
	PgLavalonBreathSkillFunction() {}
	virtual ~PgLavalonBreathSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6031105 유도 원거리
class PgHomingSkillFunction : public PgISkillFunction
{
public :
	PgHomingSkillFunction() {}
	virtual ~PgHomingSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};
//SkillNo 6000944 멀티 유도 원거리
class PgMultiHomingSkillFunction : public PgISkillFunction
{
public :
	PgMultiHomingSkillFunction() {}
	virtual ~PgMultiHomingSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//스킬 강제로 발사
class PgForceFireSkillFunction : public PgISkillFunction	
{
public :
	PgForceFireSkillFunction() {}
	virtual ~PgForceFireSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//시퀀스 스킬 강제로 발사
class PgForceSequenseFireSkillFunction : public PgISkillFunction	
{
public :
	PgForceSequenseFireSkillFunction() {}
	virtual ~PgForceSequenseFireSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//타겟이 시퀀스 스킬 강제로 발동
class PgTargetForceSequenseFireSkillFunction : public PgISkillFunction	
{
public :
	PgTargetForceSequenseFireSkillFunction() {}
	virtual ~PgTargetForceSequenseFireSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//맵의 더미를 이용한 직선 공격
class PgTargetByDummySkillFunction : public PgISkillFunction
{
public :
	PgTargetByDummySkillFunction() {}
	virtual ~PgTargetByDummySkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//맵의 더미를 이용한 원형 공격
class PgTargetByDummySkill2Function : public PgISkillFunction
{
public :
	PgTargetByDummySkill2Function() {}
	virtual ~PgTargetByDummySkill2Function() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// 맵의 더미 있는 장소로 텔레포트
class PgTeleportToDummySkillFunction : public PgISkillFunction
{
public :
	PgTeleportToDummySkillFunction() {}
	virtual ~PgTeleportToDummySkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// 6074304 폭발 점멸(순간이동)
class PgExplosionBlinkFunction : public PgISkillFunction
{
public:
	PgExplosionBlinkFunction() {}
	virtual ~PgExplosionBlinkFunction() {}

public:
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// 6074305 블랙홀
class PgBlackholeFunction : public PgISkillFunction
{
public:
	PgBlackholeFunction() {}
	virtual ~PgBlackholeFunction() {}

public:
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// 6000955 자신의 이펙트를 모두 지운다.
class PgClearAllEffectFunction : public PgISkillFunction
{
public :
	PgClearAllEffectFunction() {}
	virtual ~PgClearAllEffectFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 운석낙하 여러군데에서
class PgMassiveProjectile2SkillFunction : public PgISkillFunction
{
public :
	PgMassiveProjectile2SkillFunction() {}
	virtual ~PgMassiveProjectile2SkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

// SkillNo 6000983 본드래곤 검은브레스
class PgDarkBreathFunction : public PgISkillFunction
{
public :
	PgDarkBreathFunction() {}
	virtual ~PgDarkBreathFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};


//SkillNo 뼈소환
class PgSummonBoneFunction : public PgISkillFunction
{
public:
	PgSummonBoneFunction() {}
	virtual ~PgSummonBoneFunction() {}

public:
	virtual int SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//SkillNo 6000988 뼈소환
class PgSummonBone2Function : public PgISkillFunction
{
public:
	PgSummonBone2Function() {}
	virtual ~PgSummonBone2Function() {}

public:
	virtual int SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 60009881 강제이동
class PgForceMoveFunction : public PgISkillFunction
{
public:
	PgForceMoveFunction() {}
	virtual ~PgForceMoveFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6089400 특정 몹의 위치까지 이동
class PgMoveToTargetDummyFunction : public PgISkillFunction
{
public:
	PgMoveToTargetDummyFunction() {}
	virtual ~PgMoveToTargetDummyFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

// SkillNo 6089401 특정 몹에게 HP 회복하기
class PgHealToTargetMonsterFunction : public PgISkillFunction
{
public:
	PgHealToTargetMonsterFunction() {}
	virtual ~PgHealToTargetMonsterFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

// 특정 몹에게 HP 회복하기
// TargetType가 자신인 경우 100%확률로 스킬이 발동된다고 가정을 하고 어빌을 통해서 대상을 다시 재검색함
class PgHealToTargetMonster_2_Function : public PgISkillFunction
{
public:
	PgHealToTargetMonster_2_Function() {}
	virtual ~PgHealToTargetMonster_2_Function() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

// SkillNo 6000982 원래 리젠 위치에 몬스터 소환
class PgSummonMonsterRegenPointFunction : public PgISkillFunction
{
public:
	PgSummonMonsterRegenPointFunction() {}
	virtual ~PgSummonMonsterRegenPointFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 3531030 원래 리젠 위치에 몬스터 소환(타입에 따라 선택)
class PgSummonMonsterRegenPointChooseFunction : public PgISkillFunction
{
public:
	PgSummonMonsterRegenPointChooseFunction() {}
	virtual ~PgSummonMonsterRegenPointChooseFunction() {}

public:
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);

private:
	typedef enum eBagControlChooseType
	{
		BCCT_NONE		= 0,
		BCCT_RANDOM		= 1,
	} EBagControlChooseType;

	typedef std::vector<std::pair<int,int> > CONT_BAGCONTROL;


	void GetChoose(EBagControlChooseType const eChooseType, CONT_BAGCONTROL & vecBagControl);
	int GetBagControl(CSkillDef const* pkSkillDef, CONT_BAGCONTROL & vecBagControl);
	int GetBagControl(CUnit const* pkUnit, CONT_BAGCONTROL & vecBagControl);
	void ClearBagControl(CUnit * pkUnit);
	void SetBagControl(CUnit * pkUnit, int const iIdx, int const iBag);
};

// SkillNo 6000982 리젠 위치에 몬스터 소환(스킬 발동 횟수에 따라 BagNo 호출)
class PgSummonMonsterRegenPoint_Loop_Function : public PgISkillFunction
{
public:
	PgSummonMonsterRegenPoint_Loop_Function() {}
	virtual ~PgSummonMonsterRegenPoint_Loop_Function() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//엔티티가 다른 몬스터에게 이펙트 시전. 타겟을 항상 자기자신으로 해야 함.
class PgAddEffectByEntitySkillFunction : public PgISkillFunction
{
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

// SkillNo 6089300 지정 몬스터에게 이펙트를 걸거나 해제한다.
class PgAddRemoveEffectTargetMonsterFunction : public PgISkillFunction
{
public:
	PgAddRemoveEffectTargetMonsterFunction() {}
	virtual ~PgAddRemoveEffectTargetMonsterFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 가장 먼 더미 위치로 순간이동한다.
class PgTeleportMostDistantFunction : public PgISkillFunction
{
public:
	PgTeleportMostDistantFunction() {}
	virtual ~PgTeleportMostDistantFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 발사체, 서버에서 타겟, 발사체 갯수 지정, 클라이언트 발사
class PgTargetLocProjectileFunction : public PgISkillFunction
{
public:
	PgTargetLocProjectileFunction() {}
	virtual ~PgTargetLocProjectileFunction() {}
public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 발사체의 갯수, 방향을 결정해서 직선으로 서버에서 발사
class PgFireProjectileServerFunction : public PgISkillFunction
{
public:
	PgFireProjectileServerFunction() {}
	virtual ~PgFireProjectileServerFunction() {}
public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6091302 몬스터를 중심으로 X 자 형태의 데미지를 준다.
class PgCrossAttackFunction : public PgISkillFunction
{
public:
	PgCrossAttackFunction() {}
	virtual ~PgCrossAttackFunction() {}

public:
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// 6091103 SkillNo 이동후 트랩을 설치한다(더미 위치에 순서대로)
class PgMoveAndTrapFunction : public PgISkillFunction
{
public:
	PgMoveAndTrapFunction() {}
	virtual ~PgMoveAndTrapFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6091304 안전영역을 만들고, 그곳을 제외한 부분을 공격한다.
class PgAttackWithoutSafetyZoneFunction : public PgISkillFunction
{
public:
	PgAttackWithoutSafetyZoneFunction() {}
	virtual ~PgAttackWithoutSafetyZoneFunction() {}

public:
	virtual int SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo  지정된 더미지역에 엔티티 소환, 어빌에 따라 랜덤하게 소환하지 않음
class PgDummyEntityZoneFunction : public PgISkillFunction
{
public:
	PgDummyEntityZoneFunction() {}
	virtual ~PgDummyEntityZoneFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int SkillBegin(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 60913041 유닛 주위에 유저가 있을시 타겟 몬스터를 공격한다.
class PgCheckUserAndAttackTargetMonster : public PgISkillFunction
{
public:
	PgCheckUserAndAttackTargetMonster() {}
	virtual ~PgCheckUserAndAttackTargetMonster() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

class PgDeleteEffectSkillFuntion : public PgISkillFunction
{
public:
	PgDeleteEffectSkillFuntion() {}
	virtual ~PgDeleteEffectSkillFuntion() {}

public:
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6091502 타겟들을 다른 더미위치로 랜덤하게 이동 시킨다.
class PgRandomTeleportTargettoDummyFunction : public PgISkillFunction
{
public:
	PgRandomTeleportTargettoDummyFunction() {}
	virtual ~PgRandomTeleportTargettoDummyFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6091505 타겟을 더미위치로 이동 시킨다.
class PgTeleportTargettoDummyFunction : public PgISkillFunction
{
public:
	PgTeleportTargettoDummyFunction() {}
	virtual ~PgTeleportTargettoDummyFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6000954 타겟유닛 위치로 순간이동 시킨다.
class PgTeleportTargetFunction : public PgISkillFunction
{
public:
	PgTeleportTargetFunction() {}
	virtual ~PgTeleportTargetFunction() {}

public:
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 3542108 타겟유닛 위치로 순간이동 시킨다.
class PgTeleportTargetDamageFunction : public PgISkillFunction
{
public:
	PgTeleportTargetDamageFunction() {}
	virtual ~PgTeleportTargetDamageFunction() {}

public:
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 6089101 공격당한 타겟의 HP를 특정 값으로 설정
class PgSetHpSkillFunction : public PgISkillFunction
{
public:
	PgSetHpSkillFunction() {}
	virtual ~PgSetHpSkillFunction() {}

public:
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

// SkillNo 특정 몹의 위치까지 이동
class PgMoveToFarhestTargetFunction : public PgISkillFunction
{
public:
	PgMoveToFarhestTargetFunction() {}
	virtual ~PgMoveToFarhestTargetFunction() {}

public:
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int SkillFire( CUnit *pkUnit, int const iSkillNo, SActArg const*  pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
};

//엔티티 여러개 소환
class PgSummonEntitiesSkillFunction : public PgISkillFunction
{
public :
	PgSummonEntitiesSkillFunction() {}
	virtual ~PgSummonEntitiesSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//유닛에게 지정된 강제스킬 발동
class PgUnitForceFileSkillFunction : public PgISkillFunction
{
public :
	PgUnitForceFileSkillFunction() {}
	virtual ~PgUnitForceFileSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//순차적으로 이동가능한 타겟팅 대상들의 위치를 구하고 스킬 발동
class PgRollingTargetingSkillFunction : public PgISkillFunction
{
public :
	PgRollingTargetingSkillFunction() {}
	virtual ~PgRollingTargetingSkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//엘가 눈동자 소환 전용 스킬
class PgElgaSummonEyeSkillFunction : public PgISkillFunction
{
public :
	PgElgaSummonEyeSkillFunction() {}
	virtual ~PgElgaSummonEyeSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//엘가3차 그로기 상태 스킬
class PgElga03_GroggySkillFunction : public PgISkillFunction
{
public :
	PgElga03_GroggySkillFunction() {}
	virtual ~PgElga03_GroggySkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//콤보 공격
class PgComboAttackSkillFunction : public PgISkillFunction
{
public :
	PgComboAttackSkillFunction() {}
	virtual ~PgComboAttackSkillFunction() {}

public :
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//협동 스킬
class PgTeamPlaySkillFunction : public PgISkillFunction
{
public :
	PgTeamPlaySkillFunction() {}
	virtual ~PgTeamPlaySkillFunction() {}

public :
	virtual bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

//히드라 대지의 분노
class PgHydraEarthWrathSkillFunction : public PgISkillFunction
{
public :
	PgHydraEarthWrathSkillFunction() {}
	virtual ~PgHydraEarthWrathSkillFunction() {}

public :
	virtual int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream *pkPacket);
	virtual int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_MONSTER_PGSKILLFUNCTION_MONSTER_H