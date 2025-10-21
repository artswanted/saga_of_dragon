#ifndef MAP_MAPSERVER_ACTION_SKILL_PGSKILLABILHANDLEMANAGER_H
#define MAP_MAPSERVER_ACTION_SKILL_PGSKILLABILHANDLEMANAGER_H

#include <map>
#include <vector>
#include <string>

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>

class CUnit;
class CSkill;
class UNIT_PTR_ARRAY;
class PgActionResultVector;
class SActArg;
class PgISkillFunction;
class CSkillDef;

class PgFunctionTypeXmlLoader
{
	typedef std::map<int, int const> CONT_FUNC_TYPE_TABLE; // FunctionType 0 : Default , 1 : Script, 100 ~ : 예외 클래스들
public :
	PgFunctionTypeXmlLoader();
	~PgFunctionTypeXmlLoader();

public :
	bool ParseXml(std::wstring const &strXmlPath);
	int  GetFuncType(int const iNo) const;

private :
	CONT_FUNC_TYPE_TABLE m_kConFuncType;

};

enum eSkillFuncitonType
{
	SFT_DEFAULT = 0,//! 기본 타입	
//	SFT_SCRIPT,		//! 스크립트 처리
	SFT_DEFAULT2_ONLY_ADDEFFECT	= 2, //! Target에 AddEffect만 하는 타입(버프류)
	SFT_DEFAULT3_ONLY_DAMAGE	= 3, //! Target에 Damage만 하는 타입(공격류)
	SFT_DEFAULT4_ONLY_DAMAGE_MON= 4,//!SFT_DEFAULT3_ONLY_DAMAGE의 몬스터용
	SFT_DEFAULT5_ONLY_ADDEFFECT_MON=5, //! Target에 AddEffect만 하는 타입(버프류) 몬스터용	
	SFT_DEFAULT6_DAMAGE_AND_ADDEFFECT	=6, //! Target에 Damage와 AddEffect 적용
	SFT_DEFAULT7_ONLY_ADDEFFECT = 7, //! Caster에 DefSkill의 EffectID를, Target에 Abil의 Effect를 적용
};

class PgSkillAbilHandleManager
{
	typedef std::unordered_map< int, PgISkillFunction* >	CONT_SKILL_FUNC;

public:
	PgSkillAbilHandleManager();
	~PgSkillAbilHandleManager();

public :
	void Init();
	void Release();
	bool Build();

	int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus);
	int	 SkillBegin		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, int const iStatus, BM::Stream* pkPacket);
	int	 SkillToggle	(CUnit* pkUnit, int const iSkillNo, SActArg* pArg, bool const bToggleOn, UNIT_PTR_ARRAY* pkUnitArray=NULL, PgActionResultVector* pkResult=NULL);
	int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	bool SkillFail		(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray, PgActionResultVector* pkResult);
	bool SkillCanReserve(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg, UNIT_PTR_ARRAY* pkUnitArray);
	
	// Passive	
	int	 SkillPassive	(CUnit* pkUnit, int const iSkillNo, SActArg const* pArg);
	int	 SkillPCheck	(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	void SkillBegin		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);
	void SkillEnd		(CUnit* pkUnit, CSkillDef const* pkSkill, SActArg const* pArg);

	int	 SkillFire		(CUnit* pkUnit, int const iSkillNo, int const iStatus, SActArg const* pArg);
	
private:
	PgISkillFunction* CreateSkillFunc(int const iFuncType)const;

	//mutable ACE_RW_Thread_Mutex m_kMutex;

	CONT_SKILL_FUNC	m_kConSkill;
	CONT_SKILL_FUNC	m_kConSkillPool;
};


#define g_kSkillAbilHandleMgr SINGLETON_STATIC(PgSkillAbilHandleManager)

#endif // MAP_MAPSERVER_ACTION_SKILL_PGSKILLABILHANDLEMANAGER_H