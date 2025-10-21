#ifndef MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_H
#define MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_H

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class PgAbilTypeTable;

void OnAddAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnSetAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnSetAbil2(CUnit* pkUnit, WORD const wType, int const iValue);
void OnSetNullAbil(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnSetNullAbil2(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnAddAbil_MaxHP_MaxMP(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnAddAbil_Attack(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );
void OnExecAbil_TTW_MSG(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_SELF );
void OnHPMPDec_MaxHP_MaxMP_Rate(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL ); // HP/MP를 MaxHp/MaxMp의 % 만큼 감소 시키는 어빌 처리
void OnAddEffect(CUnit* pkUnit, WORD const wType, int const iValue, DWORD dwSendType = E_SENDTYPE_BROADALL_EFFECTABIL_MUST );

int GetTick_AT_HP_Value(CEffect* pkEffect, SActArg const* pkArg, CUnit* pkUnit=NULL); //Effect에서 Tick에서 사용하는 AT_HP 값을 구해온다. 카오스맵에 있을 경우 데미지인 경우 값이 달라져야한다. / 회복은 정상적으로 동작

namespace PgEffectFunctionUtil
{
	void DeleteAllDebuff(CEffect* pkEffect, CUnit* pkUnit=NULL);
}

//Interface Class
class PgIEffectFunction
{
protected :
	typedef std::list< SAbil >	CONT_ABIL_TYPE_VAULE;
	typedef std::map< WORD, CONT_ABIL_TYPE_VAULE* >	CONT_ABIL_TYPE_VAULE_TABLE;

	typedef void (*pfnOnExecFunc)(CUnit*, WORD const, int const, DWORD);

public :
	PgIEffectFunction();
	virtual ~PgIEffectFunction();

public :
	virtual void Init();
	virtual void Release();
	virtual void Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef);

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
	virtual int EffectReset(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);

protected :

	CONT_ABIL_TYPE_VAULE_TABLE	m_kTable;
};

//디폴트
class PgDefaultEffectFunction : public PgIEffectFunction
{
public :
	PgDefaultEffectFunction();
	virtual ~PgDefaultEffectFunction();

public :
	virtual void Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef);

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

//No 1 HP, MP 최대치 증가 및 최대치까지 회복
class PgMaxHpAndMPEffectFunction : public PgIEffectFunction 
{
public :
	PgMaxHpAndMPEffectFunction() {}
	virtual ~PgMaxHpAndMPEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
};

//No 14001 PvPHandyCap
class PgHandyCapFunction : public PgIEffectFunction 
{
public :
	PgHandyCapFunction() {}
	virtual ~PgHandyCapFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

//No 20001 Mute Chat
class PgMuteChat : public PgIEffectFunction
{
public:
	PgMuteChat() {}
	virtual ~PgMuteChat() {}

public:
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

// 마할카 변신
class PgChangeMahalkaEffectFunction : public PgDefaultEffectFunction
{
public :
	PgChangeMahalkaEffectFunction() {}
	virtual ~PgChangeMahalkaEffectFunction() {}

public :
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);

};

// 각성기 시스템
class PgAwakeSystemEffectFunction : public PgIEffectFunction 
{
public :
	PgAwakeSystemEffectFunction() {}
	virtual ~PgAwakeSystemEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};


// 각성 노말 상태
class PgAwakeNormalStateEffectFunction : public PgIEffectFunction 
{
public :
	PgAwakeNormalStateEffectFunction() {}
	virtual ~PgAwakeNormalStateEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgAwakeMaxStateEffectFunction : public PgIEffectFunction 
{
public :
	PgAwakeMaxStateEffectFunction() {}
	virtual ~PgAwakeMaxStateEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgAwakePenaltyStateEffectFunction : public PgIEffectFunction 
{
public :
	PgAwakePenaltyStateEffectFunction() {}
	virtual ~PgAwakePenaltyStateEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

// 안전거품(펫에게 특정 능력치 전이)
class PgSafeBubbleExtFunction : public PgDefaultEffectFunction
{
public :
	PgSafeBubbleExtFunction() {}
	virtual ~PgSafeBubbleExtFunction() {}

public :
	virtual void Build(PgAbilTypeTable const* pkAbilTable, CEffectDef const* pkDef);

	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
};

class PgSkillDmgByPhaseEffectFunction : public PgIEffectFunction 
{// 단계별 스킬 데미지 감소 이펙트
public :
	PgSkillDmgByPhaseEffectFunction() {}
	virtual ~PgSkillDmgByPhaseEffectFunction() {}

public :
	virtual void Build(PgAbilTypeTable const *, CEffectDef const *) {}

	virtual void EffectBegin(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect *pkEffect, SActArg const *pkArg, DWORD const dwElapsed);
};

class PgAwakeChargeEffectFunction : public PgIEffectFunction 
{// 각성기 챠지 이펙트 
public :
	PgAwakeChargeEffectFunction() {}
	virtual ~PgAwakeChargeEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
};

class PgHealFromHitDamageEffectFunction : public PgIEffectFunction 
{// 때린 대미지를 기준으로 HP를 회복시키는 이펙트
public :
	PgHealFromHitDamageEffectFunction() {}
	virtual ~PgHealFromHitDamageEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

class PgDeleteBuffEffectFunction : public PgIEffectFunction 
{// 버프 1개를 지워버리는 버프
public :
	PgDeleteBuffEffectFunction() {}
	virtual ~PgDeleteBuffEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

class PgJobSkillRecoveryFunction : public PgIEffectFunction 
{// 채집 회복 이펙트 버프
public :
	PgJobSkillRecoveryFunction() {}
	virtual ~PgJobSkillRecoveryFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
};

class PgSkillLinkageEffectFunction : public PgIEffectFunction 
{// 연계효과 유지 이펙트(연계효과의 정보를 가지고있는 이펙트)
public :
	PgSkillLinkageEffectFunction() {}
	virtual ~PgSkillLinkageEffectFunction() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
private:
};

class PgDrainEffect : public PgIEffectFunction 
{// HP, MP등을 흡수 하는 이펙트
public:
	enum eDrainType
	{// 어떤것을 흡수하는가
		DT_NONE = 0,
		DT_HP = 1,
		DT_MP = 2,
	};

	enum eDrainedValApplyTarget
	{// 흡수한 값을 적용할 대상
		DVAT_NONE	= 0,
		DVAT_CASTER = 1,
	};
public :
	PgDrainEffect() {}
	virtual ~PgDrainEffect() {}

public :
	virtual void EffectBegin(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual void EffectEnd(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg);
	virtual int EffectTick(CUnit* pkUnit, CEffect* pkEffect, SActArg const* pkArg, DWORD const dwElapsed);
private:
	bool ApplyDrainHPMP(CUnit* pkUnit, CEffect* pkEffect, int const iType, int const iDrainedValue);
};

#endif // MAP_MAPSERVER_ACTION_SKILL_DEFAULT_PGEFFECTFUNCTION_H