#ifndef MAP_MAPSERVER_ACTION_AI_TARGETTING_PGTARGETTINGMANAGER_H
#define MAP_MAPSERVER_ACTION_AI_TARGETTING_PGTARGETTINGMANAGER_H

#include <map>

class CUnit;
class CSkillDef;
class UNIT_PTR_ARRAY;
class PgGround;

class PgIBaseTargetting
{
public:
    PgIBaseTargetting(){}
	virtual ~PgIBaseTargetting() {}
	CUnit* SetArray(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType=0) const;
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const = 0;	//계산 방법
};

class PgTargettingNone : public PgIBaseTargetting	//아무런 타겟팅도 하지 않는다
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const;	//계산 방법
};

class PgTargettingHP : public PgIBaseTargetting	//HP기준으로 타겟팅
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	static bool SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
	static bool SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
};

class PgTargettingLevel : public PgIBaseTargetting	//레벨기준으로 타겟팅
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	static bool SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
	static bool SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
};

class PgTargettingBaseClass : public PgIBaseTargetting	//특정 베이스클래스 기준으로 타겟팅
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const;	//계산 방법
private:
	CUnit* FindEqual(int const iClass, CONT_DEFUPGRADECLASS const* pkUpClass, UNIT_PTR_ARRAY* pkArr) const;
	CUnit* FindDiff(int const iClass, CONT_DEFUPGRADECLASS const* pkUpClass, UNIT_PTR_ARRAY* pkArr) const;
};

class PgTargettingAggro : public PgIBaseTargetting	//어그로 기준으로 타겟팅
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	CUnit* FindGreatest(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround*) const;
	CUnit* FindLeast(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround*) const;
};

class PgTargettingDistance : public PgIBaseTargetting	//거리 기준으로 타겟팅
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	CUnit* FindGreatest(CUnit* pkUnit, UNIT_PTR_ARRAY* pkArr, PgGround*) const;
};

class PgTargettingMoveSpeed : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	static bool SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
	static bool SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
};

class PgTargettingStrategicPoint : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	static bool SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
	static bool SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
};

class PgTargettingCallerTarget : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
};

class PgTargettingAggroPoint : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0 ) const;	//계산 방법
private:
	static bool SortGreater(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
	static bool SortLesser(UNIT_PTR_HELPER const& lhs, UNIT_PTR_HELPER const& rhs);
};

//클래스 기준으로 타겟팅
class PgTargettingClassNo : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const;
};

//그라운드에 있는 파티마스터를 기준으로 타겟팅
class PgTargettingPartyMaster : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const;
};

//별자리 던전 보스방 타게팅 방식(1차 NPC 클래스 번호, 2차 파티장)
class PgTargettingConstellation : public PgIBaseTargetting
{
public:
	virtual CUnit* CalcWeight(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, int const iSecondType = 0) const;
};

class PgTargettingManager
{
public:

	typedef enum eTargettingType
	{
		ETT_NONE        = 0,
		ETT_DAMAGE      = 1,
		ETT_BASE_CALSS  = 2,
		ETT_LEVEL       = 3,
		ETT_HP          = 4,
		ETT_DISTANCE    = 5,
        ETT_MOVESPEED   = 6,
        ETT_STRATEGICPOINT	= 7,
		ETT_CALLERTARGET	= 8, //Caller를 타겟팅한 적을 우선
		ETT_AGGROPOINT	= 9,
		ETT_CLASS_NO		= 10,
		ETT_PARTY_MASTER	= 11,
		ETT_CONSTELLATION	= 12, //별자리 던전 보스방 타게팅 방식
		ETT_MAX,
	} E_TARGETTING_TYPE;
	typedef std::multimap<int, CUnit*> UNIT_PTR_ARRAY_WEIGHT;
	typedef std::map<int, PgIBaseTargetting*> Targetting_Map;
	PgTargettingManager() { Init(); }
	~PgTargettingManager();

	CUnit* FindTarget(CUnit*, CSkillDef const*, UNIT_PTR_ARRAY*, PgGround* pkGround = NULL, E_TARGETTING_TYPE eType=ETT_NONE, int const iSecType=0) const;
	CUnit* FindTarget(CUnit*, CSkillDef const*, CUnit::DequeTarget&, PgGround* pkGround = NULL, E_TARGETTING_TYPE const eType=ETT_NONE, int const iSecType=0) const;
	PgIBaseTargetting const* GetPtr(E_TARGETTING_TYPE const eType)const;
private:
	void Init();
	Targetting_Map m_kTargetting_Map;
};

#define g_kTargettingManager SINGLETON_STATIC(PgTargettingManager)

#endif // MAP_MAPSERVER_ACTION_AI_TARGETTING_PGTARGETTINGMANAGER_H