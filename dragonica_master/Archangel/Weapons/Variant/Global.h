#ifndef WEAPON_VARIANT_BASIC_GLOBAL_H
#define WEAPON_VARIANT_BASIC_GLOBAL_H

#include "loki/typemanip.h"
#include "BM/Point.h"
#include "idobject.h"
#include "unit.h"

#include "Global.inl"

class PgActionResult;
class PgActionResultVector;
class UNIT_PTR_ARRAY;
class CSkillDef;

// 액티브 스킬 사용시 영향을 받는 자식 스킬의 최대치
int const CHILD_SKILL_MAX = 10;
int const EFFECTNUM_MAX = 10;
int const ON_DIE_SKILL_HP = 1;

extern int GetGroggyPoint(CSkillDef const* pkSkillDef);
extern bool CheckBalanceWStringLen( std::wstring const &wstr, size_t const iLen );// 영문의경우 iLen사이즈의 2배를 적용한다.
extern int CS_GetReflectDamage(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, bool bPhysicSkill);
extern int CS_GetReflectDamage_FromItem(CUnit* pkCaster, CUnit* pkTarget, int const iDamage, bool bPhysicSkill);
extern __int64 CS_CheckDamage(CUnit* pkCaster, CUnit* pkTarget, __int64 i64Damage, bool bPhysical, PgActionResult* pkResult, int const iSkillRate, bool const bProjectileDmg = false, CSkillDef const* pkSkillDef = NULL);
extern int CS_DoReflectDamage(CUnit const* pkCaster, CUnit const* pkTarget, int const iSkillNo, int const iReflected);
extern bool CS_GetDmgResult(int const iSkillNo, CUnit* pkCaster, CUnit* pkTarget, int const iDmgRate, int const iDmg, PgActionResult* pkResult);
extern bool CS_GetSkillResultDefault(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector, CUnit * pkCaller=NULL);
extern WORD GetBasicAbil(WORD const wAbil);
extern WORD GetCalculateAbil(WORD const wAbil);
extern WORD GetAmpliAbil(WORD const wAbil);
extern bool IsCalculateAbilRange(WORD const wAbil);
extern bool IsRateAbil(WORD const wAbil);
extern bool IsCalculatedAbil(WORD const wAbil);
extern bool IsMonsterCardAbil(WORD const wAbil);
extern bool IsHitAbilFromEquipItem(WORD const wAbil);	// 장비에서 세팅해주는 '타격시 걸리는 이펙트'에 사용되는 어빌인가?
extern bool IsCountAbil( WORD const wAbil );
extern bool IsItemActionAbil(WORD const wAbil);
extern int GetCountAbil( CAbilObject const *pkAbilObj, WORD const wType, int const iFlag );
extern bool AddCountAbil( CAbilObject *pkAbilObj, WORD const wType, int const iFlag, bool const bAdd );
extern bool RemoveCountAbil( CAbilObject *pkAbilObj, WORD const wType, int const iFlag );
extern int GetCountAbilCheckFlag( int const iFlag );
extern void POINT3_2_POINT3BY(POINT3 const &rkPos, POINT3BY& rkOut);
extern WORD GetRateAbil(WORD const wAbil);
extern DWORD DifftimeGetTime(DWORD const& dwStart, DWORD const& dwEnd);
extern int SetDamageDelay(CSkillDef const *pkSkillDef, CUnit const* pkCaster, CUnit* pkTarget);
extern int CalcDecHitRate(CUnit const* pkCaster, CUnit const* pkTarget, int const iHitRate, int const iDecDodgeAbs = 0, int const iDecDodgeRate = 0);
extern void AddDamageEffectAndChangeBlockValue(CUnit* pkCaster, CUnit* pkTarget, CSkillDef const* pkSkillDef, PgActionResult* pkResult);
extern __int64 CS_CheckDmgPer(CUnit* pkTarget, __int64 const i64Damage, bool bPhysicSkill);
extern __int64 CS_CheckItemDamage( CUnit *pkCaster, CUnit *pkTarget, bool const bIsCritical, __int64 i64Damage );
extern bool IsHaveSkillDamage(CSkillDef const* pkSkillDef);
extern bool IsDamageAction(CUnit const * const pkUnit, CSkillDef const * const pkSkillDef);

//타겟 리스트르 참고하여 데미지를 대신 입는 유닛이 있을 경우 ActionResultVec를 갱신한다.
extern void CheckActionResultVec(UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector& kActionResultVec);

//도둑 기본 공격 2/4타 
extern bool CS_GetSkillResult103201201(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector); 
//투사 룰렛
extern bool CS_GetSkillResult106000101(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector); 
//전사 조인트 브레이크 (데미지없이 AT_DAM_EFFECT_S 만 추가 하고 싶을 경우 사용 조인트/플래시뱅 등)
extern bool CS_GetSkillResult105300501(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector); 
//트랩퍼 MP제로 폭발
extern bool CS_GetSkillResult1100028011(int const iSkillNo, CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, PgActionResultVector* pkResultVector); 

bool GetItemName(int const iItemNo, std::wstring & rkOut);

extern bool IsElementAbil(WORD const wAbil);
extern bool IsBasicElementDmgAbilRange(WORD const wAbil);
extern bool IsConvertElementalDmgRateAbilRange(WORD const wAbil);
extern float CalcTotalElementalResistRate(CUnit const* pAttacker, CUnit const* pDefender, int const iAbil);	//%값이 아님. 0. 이하의 소숫점 값이 리턴 됨
extern float CalcTotalElementalResistRateSimple(int const iAttackerLevel, int const iDefenderResiste);

namespace PgNpcTalkUtil
{
	extern float const fNPC_TALK_ENABLE_RANGE;
}

namespace PgArrayUtil
{
	template< typename _TYPE, size_t _ARRAY_SIZE >
	size_t GetArrayCount(_TYPE (&abyArray)[_ARRAY_SIZE])
	{
		return _ARRAY_SIZE;
	}

	template< typename _TYPE, size_t _ARRAY_SIZE >
	_TYPE* GetEndArray(_TYPE (&abyArray)[_ARRAY_SIZE]) //주의 Last Element가 아닌, End Point다
	{
		return abyArray + _ARRAY_SIZE;
	}

	template< typename _TYPE, size_t _ARRAY_SIZE >
	bool IsInArray(_TYPE* pCur, _TYPE (&abyArray)[_ARRAY_SIZE])
	{
		return abyArray <= pCur && &abyArray[_ARRAY_SIZE] > pCur;
	}
}

namespace PgHometownPortalUtil
{
	__int64 UsePortalCost(int const iLevel);
}

template< typename T > struct SCalcValueRate;

template< >
struct SCalcValueRate<int>{typedef __int64 TypeCalcValueRate;};

template< >
struct SCalcValueRate<__int64>{typedef __int64 TypeCalcValueRate;};

struct SRateControl
{
	typedef enum
	{
		E_RATE_VALUE,
		E_RATE_RATE,
	};

	static bool IsActivate( WORD const wType, CUnit *pkCaster )
	{
		int const iRate = pkCaster->GetAbil( wType );
		if ( 0 < iRate )
		{
			int const iRand = static_cast<int>(pkCaster->GetRandom() % static_cast<DWORD>(ABILITY_RATE_VALUE));
			return (iRand <= iRate);
		}

		return false;
	}

	template< int iType >
	static int Get( WORD const wType, CUnit *pkCaster, int const iValue )
	{
		if ( true == IsActivate( wType, pkCaster ) )
		{
			return GetResult( Loki::Int2Type<iType>(), wType+1, pkCaster, iValue );
		}
		return 0;
	}

	template< int iType >
	static int GetResult( WORD const wType, CUnit *pkCaster, int const iValue )
	{	
		return GetResult( Loki::Int2Type<iType>(), wType, pkCaster, iValue );
	}

	static int GetResult( Loki::Int2Type<E_RATE_VALUE>, WORD const wType, CUnit *pkCaster, int const iValue )
	{
		return pkCaster->GetAbil( wType );
	}

	static int GetResult( Loki::Int2Type<E_RATE_RATE>, WORD const wType, CUnit *pkCaster, int const iValue )
	{
		int const iAddRate = pkCaster->GetAbil( wType );
		return GetValueRate( iValue, iAddRate );
	}

	template< typename T >
	static T GetValueRate( T const iBasicValue, T const iAddRate )
	{
		typedef typename SCalcValueRate<T>::TypeCalcValueRate TYPE_CAST;

		TYPE_CAST const iCastValue = static_cast<TYPE_CAST>(iBasicValue) * static_cast<TYPE_CAST>(iAddRate);
		return static_cast<T>( iCastValue / static_cast<TYPE_CAST>(ABILITY_RATE_VALUE64) );
	}
};

namespace PgMyHomeFuncRate
{
	int		GetHomeFuncItemValue(eMyHomeSideJob const kSideJob, eMyHomeSideJobRateType const kJobRateType, CUnit * pkUnit);
	double	GetCostRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit);
	double	GetSoulRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit);
	int		GetSuccessRate(eMyHomeSideJob const kSideJob, CUnit * pkUnit);
}

namespace LOCAL_MGR
{
	class CLocal; // 클래스 전방 선언
}

namespace PgXmlLocalUtil
{
	typedef std::pair<TiXmlNode const*, TiXmlNode const*> RESULT_NODE;	//first: FindNode, second; DefultNode
	extern char const* LOCAL_ELEMENT_NAME;
	TiXmlNode const* FindInLocal(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlNode const* pkLocalNode, char const* szContents = "NULL", bool const bNationCode = false);
	RESULT_NODE FindInLocalNode(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlNode const* pkLocalNode, char const* szContents = "NULL", bool const bNationCode = false);
	TiXmlElement const* FindInLocalResult(LOCAL_MGR::CLocal& rkLocalMgr, TiXmlElement const* pkLocalNode, bool const bNationCode = false);
}

class PgSkillHelpFunc
{
public:
	enum eAbilityType
	{// 수치 -> 확률 계산이 사용되는 종류들
		EAT_NONE = 0,
		EAT_HIT,
		EAT_DODGE,
		EAT_BLOCK,
		EAT_CRITICAL,		
		EAT_MAX
	};

	static float const MAX_DODGE_SUCCESS_RATE;	// 최대 회피 성공률
	static float const MAX_BLOCK_SUCCESS_RATE;	// 최대 블록 성공률
	static float const MAX_CRITICAL_RATE;		// 최대 크리티컬 성공률
	static float const MAX_CRITICAL_POWER;		// 최대 크리티컬 데미지
	static float const MAX_BLOCK_DAMAGE_RATE;	// 최대 블록데미지 비율	
	
	static float const MAX_CRITICAL_POWER_DEC;				// 치명타 방어력
	static float const MAX_FINAL_CRITICAL_SUCCESS_RATE_DEC;	// 치명타저항율
	static float const MAX_DEFENCE_IGNORE_RATE;				// 상대방의 방어력(물방,마방)을 무시하고 공격 할 확률 ( 만분률 )
	static float const MAX_BLOCK_SUCCESS_RATE_DEC;			// 블록관통률
	static float const MAX_RESIST_DEBUFF;					// 저주저항율	
	static float const MAX_SUCCESS_DEBUFF;					// 저주성공률	
	static float const MAX_DEBUFF_DURATION_DEC_RATE;	// 디버프 해독력(?)
	static float const MAX_DEBUFF_DURATION_ADD_RATE;	// 디버프 중독력(?)
	static int iDexStatic;
	static int iEvadeValueValueStatic;
	static int iHitValueStatic;
	static float iCriticalStatic;

public:
	// 반사 데미지 값 얻어오는 함수
	static int CalcReflectDamage(CUnit* pkCaster, CUnit* pkTarget,__int64 const i64Damage, PgActionResult* pkActionResult, CSkillDef const* pkSkillDef, bool const bPhysicDmg, bool const bOffRandomSeed, bool const bCompelCalc=false);
	// 회피율과, 명중률을 계산해서 최종 명중률을 넘겨 주는 함수(만분률)
	static int GetCalcHitSuccessRate(CUnit* pkCaster,  CUnit* pkTarget, CSkillDef const* pkCasterSkillDef, int const iDecDodgeAbs, int const iDecDodgeRate);

	//블록 
	static bool IsAbleToBlock(CUnit* pkTarget, CSkillDef const* pkCasterSkillDef);		// 블록이 가능한 상태인가? (AT_100PERECNT_BLOCK_RATE에선 체크하지 않음)
	static int CalcDamageAfterBlock(__int64 iOriginalDamage, int iBlockDamageRate);	// 블록 후, 받을 데미지
	static bool CanChangeToBlockActionEffect(int const iActionEffect);				// 블록가능데미지액션(액션 이펙트) 인가?
	
	// 수치 -> 확률 계산
	static int CalcSuccessValueToRate(eAbilityType const eType, CUnit* pkCaster, CUnit* pkTarget, CSkillDef const* pkSkillDef, int const iAddExceptionalFinalSuccessRate);
	static int CalcSuccessValueToRate(int const iSuccessValueType, int iAbilValue, int iFinalSuccessRateType,int iAddFinalSuccessRate, int const iCasterLv, int const iTargetLv, CUnit* pkCaster=NULL);
	
	// 최대 수치
	static float AdjustMaxValue(int const iAbilType, float fValue, int const iCasterLv, int const iTargetLv, CUnit* pkCaster=NULL);

	static float GetRateValue(float &fMaxElement,int const &iAbilType, CUnit* pkCaster);
		
	// 특정 어빌의 최종율 값을 증가시키는 어빌이 존재한다면 그 어빌을 리턴함
	static int GetFinalRateAbil(int const iAbilType);
	
	// 크리티컬로 인한 데미지 증가 값 계산
	static __int64 CalcCriticalDamage(__int64 const i64OrigDamage, CUnit* pkCaster, CUnit* pkTarget);
	static __int64 CalcCriticalDamage(__int64 i64OrigDamage, int const iCriticalPowerRate, int const iDecCriticalPowerRate, int const iCasterLv, int const iTargetLv,CUnit* pkCaster=NULL);

	// Stat을 각 실 값으로 계산
	static void CalcStatToPhyAttack(int const iStr, int const iCon, int& riOutPhyMin, int& riOutPhyMax);	// 물공
	static void CalcStatToAddPhyAttack(int const iLevel, int const iStr, int& riOutAddPhy);					// 물리 후공격력(?)
	static void CalcStatToMagicAttack(int const iInt, int& riOutPhyMin, int& riOutPhyMax);					// 마공
	static void CalcStatToPhyDefence(int const iStr, int const iCon, int const iDex, int& riOutPhyDefence);	// 물방
	static void CalcStatToMagicDefence(int const iLevel, int const iInt, int& riOutPhyDefence);				// 마방
	static void CalcStatToHitSuccess(PgPlayer * pkPlayer,int const iDex, int& riOutHitSuccessValue);							// 명중수치
	static void CalcStatToDodgeSuccess(PgPlayer * pkPlayer,int const iDex, int& riOutDodgeSuccessValue);						// 회치 수치
	static void CalcStatToCriticalSuccess(PgPlayer * pkPlayer, int const iDex, int& riOutDodgeSuccessValue);						// 치명타 성공수치
	static void CalcStatToMaxHP(int const iLevel, int const iCon, int& riOutMaxHP);							// 최대 HP
	static void CalcStatToMaxMP(int const iLevel, int const iInt, int& riOutMaxMP);							// 최대 MP

	static void SkillOnDie(CUnit * pkUnit, int const iSkillNo, bool const bVoluntarily);

	// 이펙트류 종류 확인
	static bool IsSystemEffect(CEffect const* pkEffect);
	static bool IsSystemEffect(CEffectDef const* pkEffectDef);
	static bool IsSystemEffect(short int const sEffectType, bool const bDeleteAble);
private:
	static float ConvertAbilValueToRate(int const iAbilType, float fValue, int const iCasterLv, int const iTargetLv, CUnit* pkCaster);

	PgSkillHelpFunc(){}
	~PgSkillHelpFunc(){}
};
class PgAdjustSkillFunc
{
public:
	// 스킬 조정 데미지
	static __int64 CalcAdjustSkillValue(EAdjustSkillCalcType const eCalcType, int const iSkillNo, CUnit* pkUnit, __int64 const i64OriginValue);
	static int GetAttackRange(CUnit * pkUnit, CSkillDef const* pkDef);
	static int CalcAttackRange(CUnit * pkUnit, int const iSkillRange);
private:
	static __int64 GetAddtionalAdjustSkillValue(CUnit* pkUnit, int const iAbilType, CEffect* pkEffect, __int64 i64Value);	// CalcAdjustSkillValue 함수에서 어빌값에 따라 실값을 계산해주는 함수
	static bool IsApplySkill(int const iSkillNo, CEffect* pkEffect);	// CalcAdjustSkillValue 함수에서 iSkillNo을 pkEffect에서 제어하는지 확인해주는 함수

	PgAdjustSkillFunc(){}
	~PgAdjustSkillFunc(){}
};

// SubPlayer(쌍둥이와 같은 보조 캐릭터)를 가지고 있는 class인가
bool IsClass_OwnSubPlayer(int iClass);
#endif // WEAPON_VARIANT_BASIC_GLOBAL_H