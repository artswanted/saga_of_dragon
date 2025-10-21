#ifndef WEAPON_VARIANT_SKILL_EFFECT_PGSKILLFILTER_H
#define WEAPON_VARIANT_SKILL_EFFECT_PGSKILLFILTER_H

#include "constant.h"

typedef enum : unsigned short
{
	ESFilter_None = 0,			// Filter 없다
	ESFilter_Enable_All = 1,	// 모든 스킬 사용 가능
	ESFilter_Disable_All = 2,	// 모든 스킬 사용 금지
	ESFilter_Delete_Effect = 3, // 스킬이 사용 되면 삭제 되어야하는 이펙트
	ESFilter_Delete_Effect_From_Caster = 4, // 스킬이 사용 되면 삭제 되어야하는 이펙트가 캐스터가 건 이펙트일 경우만 삭제
	ESFilter_CannotAttack = 5,	// 공격금지 이펙트
	ESFilter_Ignore_Action_Effect = 6,	// 액션 이펙트 무시(리액션 무시)
	ESFilter_Disable_OnEffect = 7, // 특정 이펙트가 있으면 안됨
} ESkillFilterType;

typedef std::vector<int> VEC_EXCEPT_SKILL;

class PgSkillFilter
{
public:
	PgSkillFilter() {};
	virtual ~PgSkillFilter() {};

	virtual bool CheckFilter(int const iSkillNo) const= 0;
	virtual ESkillFilterType Type() = 0;
	void AddExcept(int const iSkillNo);
	void SetID(int const iEffectID);
	void Clear();
	VEC_EXCEPT_SKILL* GetExceptList();
	
protected:
	VEC_EXCEPT_SKILL m_kExcept;
	int m_iID;
};

class PgSkillFilter_EnableAll : public PgSkillFilter
{
public:
	PgSkillFilter_EnableAll() {};
	virtual ~PgSkillFilter_EnableAll() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Enable_All; }

};

class PgSkillFilter_DisableAll : public PgSkillFilter
{
public:
	PgSkillFilter_DisableAll() {};
	virtual ~PgSkillFilter_DisableAll() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Disable_All; }
};

class PgSkillFilter_DeleteEffect : public PgSkillFilter
{
public:
	PgSkillFilter_DeleteEffect() {};
	virtual ~PgSkillFilter_DeleteEffect() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Delete_Effect; }
};

class PgSkillFilter_DeleteEffect_From_Caster : public PgSkillFilter
{
public:
	PgSkillFilter_DeleteEffect_From_Caster() {};
	virtual ~PgSkillFilter_DeleteEffect_From_Caster() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Delete_Effect_From_Caster; }
};

class PgSkillFilter_CannotAttack : public PgSkillFilter
{
public:
	PgSkillFilter_CannotAttack() {};
	virtual ~PgSkillFilter_CannotAttack() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_CannotAttack; }
};

class PgSkillFilter_Ignore_Action_Effect : public PgSkillFilter
{
public:
	PgSkillFilter_Ignore_Action_Effect() {};
	virtual ~PgSkillFilter_Ignore_Action_Effect() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Ignore_Action_Effect; }
};

class PgSkillFilter_Disable_OnEffect : public PgSkillFilter
{
public:
	PgSkillFilter_Disable_OnEffect() {};
	virtual ~PgSkillFilter_Disable_OnEffect() {};

	virtual bool CheckFilter(int const iSkillNo) const;
	virtual ESkillFilterType Type() { return ESFilter_Disable_OnEffect; }
};

class PgSkillFilterMgr
{
public:
	PgSkillFilterMgr() {};
	~PgSkillFilterMgr() {};

	void Init();
	bool AdaptEffect(int const iEffectNo);
	void RemoveEffect(int const iEffectNo);
	bool CheckFilter(int const iSkillNo, SSFilter_Result* pkResult, ESkillFilterType eFilterType) const; // eFilterType 특정 필터만 체크 할 경우
	PgSkillFilter* GetSkillFilter(int const nSkillNo) const;

	void WriteToPacket(BM::Stream& rkPacket, EWRITETYPE const kWriteType) const;
	void ReadFromPacket(BM::Stream& rkPacket, EWRITETYPE const kWriteType);

protected:
	PgSkillFilter* CreateNew(int const iEffectNo, ESkillFilterType eType);
	void RemovePool(PgSkillFilter* pkFilter);
private:
	typedef std::map<int, PgSkillFilter*> CONT_SKILL_FILTER;
	CONT_SKILL_FILTER m_kFilterEffect;

	typedef std::list<int> CONT_RESERVE_FILTER;
	CONT_RESERVE_FILTER m_kReserveFilter;

	typedef BM::TObjectPool<PgSkillFilter_EnableAll> FilterEnableAllPool;
	static FilterEnableAllPool m_kEnableAllPool;
	typedef BM::TObjectPool<PgSkillFilter_DisableAll> FilterDisableAllPool;
	static FilterDisableAllPool m_kDisableAllPool;
	typedef BM::TObjectPool<PgSkillFilter_DeleteEffect> FilterDeleteEffectPool;
	static FilterDeleteEffectPool m_kDeleteEffectPool;
	typedef BM::TObjectPool<PgSkillFilter_DeleteEffect_From_Caster> FilterDeleteEffectFromCasterPool;
	static FilterDeleteEffectFromCasterPool m_kDeleteEffectFromCasterPool;
	typedef BM::TObjectPool<PgSkillFilter_CannotAttack> FilterCannotAttackPool;
	static FilterCannotAttackPool m_kCannotAttackPool;
	typedef BM::TObjectPool<PgSkillFilter_Ignore_Action_Effect> FilterIgnoreActionEffectPool;
	static FilterIgnoreActionEffectPool m_kIgnoreActionEffectPool;

	typedef BM::TObjectPool<PgSkillFilter_Disable_OnEffect> FilterDisableOnEffectPool;
	static FilterDisableOnEffectPool m_kDisableOnEffectPool;

};

#endif // WEAPON_VARIANT_SKILL_EFFECT_PGSKILLFILTER_H