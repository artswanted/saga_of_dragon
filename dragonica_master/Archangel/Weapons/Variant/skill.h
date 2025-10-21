//// Skill Object Class
// Dukguru
//
#ifndef WEAPON_VARIANT_SKILL_EFFECT_SKILL_H
#define WEAPON_VARIANT_SKILL_EFFECT_SKILL_H

#include <stack>

#include "TDefMgr.h"
#include "IGObject.h"
#include "IDObject.h"
#include "Lohengrin/dbtables.h"
#include "DefAbilType.h"
#include "PgSkillFilter.h"

//#define MAKE_SKILL_NO(no, lv) ((DWORD)no | ((DWORD)lv << 16))

// Learn new skill
// [PARAMETER]
//	iSkillID : skill ID to learn
// [RETURN]
//	return code
//		1 : success
//		101 : Cannot find skill
//		102 : learn lower level skill (Parent skill)
//		103 : SkillPoint limit
//		104 : Level limit
//		105 : Class limit
//		106 : NeedSkill limit
//		107 : Main skill limit
//		108 : Only Player can learn skill
//		108 : Max Level limit
//		200 : system error
typedef enum : int
{
	LS_RET_SUCCEEDED				= 1,
	LS_RET_ERROR_DEF				= 101,
	LS_RET_LOWER_PARANTSKILL_LEVEL	= 102,
	LS_RET_POINT_LIMIT				= 103,
	LS_RET_LEVEL_LIMIT				= 104,
	LS_RET_CLASS_LIMIT				= 105,
	LS_RET_NEEDSKILL_LIMIT			= 106,
	LS_RET_MAIN_SKILL_LIMIT			= 107,
	LS_RET_ERROR_UNITTYPE			= 108,
	LS_RET_LEVEL_LIMIT_MAX			= 109, //Max Skill Level 을 넘었음
	LS_RET_ERROR_SYSTEM				= 200,
	
}ELearnSkillRet;

typedef enum
{
	EST_NONE = 0,
	
	EST_MIN = 1,
	//디비 저장되는값.
	EST_GENERAL = 1,
	EST_ACTIVE = 2,
	EST_PASSIVE = 3,
	EST_TOGGLE = 4,
	
	EST_MAX_STATIC,

	//동적 컨테이너 이므로 저장 하거나 하지 않음.
	EST_TOGGLE_ON = 5,
	EST_GROUP = 6,	// 실제 Skill이 아니라, 다른스킬들의 묶음(Group)
	
	EST_MAX,
}ESkillType;

typedef enum
{
	EMST_NONE = 0,
	EMST_GENERAL = 1,
	EMST_THROW1 = 2,
	EMST_THROW2 = 3,
	EMST_INSTANCE_MAGIC = 4,
	EMST_CAST_MAGIC = 5,
}EMonsterSkillType;

typedef enum
{
	ESS_NONE = 0,
	ESS_CASTTIME = 1,
	ESS_FIRE = 2,
	ESS_MONITOR = 3,	// 클라이언트에서 다른 클라이언트로 브로드캐스팅
	//ESS_MAINTENANCETIME,
	ESS_REQ_CAST = 4,
	ESS_TARGETLISTMODIFY=5,	//	동일한 액션에 타겟리스트만 바뀌었을 때 이것을 브로드캐스팅
	ESS_TOGGLE_ON=6,
	ESS_TOGGLE_OFF=7,
	ESS_TIMEWAITING=8,	// 일정시간을 기다린다.(폭발형스킬같을때, 폭탄이 폭발할때 까지 기다리는 시간)
	ESS_STOP = 9,
} ESkillStatus;

typedef enum
{
	ESK_NONE = 0,
	ESK_TRAINED	= 1,	// 숙련
	ESK_ATTACK	= 2,	// 공격
	ESK_DEFENCE	= 3,	// 방어
	ESK_CONTROL	= 4,	// 통제
	ESK_BATTLE	= 5,	// 전투
	ESK_ATTRIBUTE	= 6,	// 속성
	ESK_RECOVERY	= 7,	// 회복
	ESK_ARROW	= 8,	// 화살
	ESK_TRAP	= 9,	// 덫
	ESK_HIDE	= 10,	// 은신
	ESK_POISON	= 11,	// 독
} ESkillKind;

class CSkillDef;

int const MAX_SKILL_STACK_SIZE = 20;
int const MAX_SKILL_TARGETNUM = 20;
int const SKILL_NO_GLOBAL_COOLTIME = 0;
int const MAX_SKILL_LEVEL = 5; 
int const MAX_PET_SKILL_LEVEL = 5;

typedef enum {
	ECaster_State_OnGround	= 0x0001,	// 땅위에 있는 상태
	ECaster_State_Jumping = 0x0002,		// 점프한 상태
	ECaster_State_Dead	= 0x0004,		// 죽은 상태
} ECaster_State;	// Abil(AT_CASTER_STATE) 값으로 사용

typedef enum {
	EFireType_Normal				= 0x0000,	// (0)		: 단발성 보통 공격
	EFireType_Dur_GoalPos			= 0x0001,	// (1)		: GoalPos 갈때까지 Fire 상태 유지 하는 스킬
	EFireType_Dur_Time				= 0x0002,	// (2)		: 일정시간 Fire 상태 유지 하는 스킬 (유지시간은 SkillDef::GetAbil(AT_MAINTENANCE_TIME))
	EFireType_Moving				= 0x0004,	// (4)		: Fire 동안 이동 가능(AI Unit 자신이 이동)(이동속도는 SkillDef::GetAbil(AT_R_MOVESPEED))
	EFireType_Projectile			= 0x0008,	// (8)		: 발사체 발사하는 것
	EFireType_TimeDelay				= 0x0010,	// (16)		: Fire 이후 얼마후에 Damage주는 스킬(폭발형같은 경우)(유지시간은 SkillDef::GetAbil(AT_MAINTENANCE_TIME))
	EFireType_JumpGoalPos			= 0x0020,	// (32)		: Casting이후, Fire 이전에 GoalPos로 순간이동해야 한다.(점프해서바닥찍기 공격같은 경우 순간이동 필요)
	EFireType_AttackToGoalPos		= 0x0040,	// (64)		: GoalPos 위치를 중심으로 공격범위가 설정된다.
	EFireType_KeepVisionVector		= 0x0080,	// (128)	: Fire할 때 보는 방향을 유지한다.

	// Fire 상태일때는 Damage 계산 하면 안되는 FireType
	EFireType_DontDmgWhenFire = EFireType_Dur_GoalPos | EFireType_Dur_Time | EFireType_Moving | EFireType_Projectile | EFireType_TimeDelay,
} ESkillFireType;	// Abil(AT_FIRE_TYPE) bit flag


typedef enum {
	EFSF_NONE					= 0,
	EFSF_NOT_RUN_BLOWUP			= 0x01, //Blowup상태에서는 강제스킬을 사용하지 않음
	EFSF_NO_DMGACT				= 0x02, //스킬 사용시 데미지 액션하지 않음
	EFSF_CLEAR_RESERVE			= 0x04, //기존에 등록된 강제스킬을 클리어시킴
	EFSF_NOW_FIRESKILL_CANCLE	= 0x08, //현재 수행중인 스킬 강제 해제
	EFSF_NO_SET_TARGETLIST		= 0x10, //타겟을 설정하지 않는다. 기존에 존재하면 그대로 사용
	EFSF_MAX					= 0xFF
} EForceSetFlag;

class CSkill 
	: public IGObject
{
public :
	typedef struct tagSCoolTimeInfo
	{
		tagSCoolTimeInfo()
			:	iSkillNo(0)
			,	dwCoolTime(0)
		{}

		int iSkillNo;
		DWORD dwCoolTime;
	} SCoolTimeInfo;

	typedef struct tagForceSetInfo
	{
		tagForceSetInfo(int const iSkillNo, EForceSetFlag const eType, BM::Stream const& param = BM::Stream())
			: iForceSkillNo(iSkillNo)
			, eForceSetFlag(eType)
			, kParam(param)
		{}

		void operator = (const tagForceSetInfo& rhs)
		{
			iForceSkillNo = rhs.iForceSkillNo;
			eForceSetFlag = rhs.eForceSetFlag;
			kParam = rhs.kParam;
		}

		int iForceSkillNo;				//강제로 발동할 스킬번호
		EForceSetFlag eForceSetFlag;	//강제스킬로 발동할 스킬옵션
		BM::Stream kParam;
	} SForceSetInfo;

	typedef std::list<int> SKILL_STACK;
	typedef std::map<int, DWORD> MAP_COOLTIME;	// <SkillNo, GameElapsedTime(ms)>
	typedef std::deque<SForceSetInfo> CONT_FORCESETINFO;

public:
	CSkillDef const* GetSkillDef() const;
	int GetEffectNo() const;
	int GetSkillNo() const ;

	EActionResultCode Casting(int const iSkillNo, DWORD const dwCurrentTime);
	EActionResultCode Fire(int const iSkillNo, DWORD const dwCurentTime,int const iCoolTimeRate);
	EActionResultCode TargetModified(int const iSkillNo);
	EActionResultCode ToggleOnOff(int const iSkillNo, ESkillStatus const eStatus, DWORD const dwCurrentTime,int const iCoolTimeRate, bool const bFired = true);
	EActionResultCode ReqCasting(int const iSkillNo, DWORD const dwCurrentTime,int const iCastTimeRate);
	EActionResultCode CheckSkillAction(int const iSkillNo, ESkillStatus const eStatus, DWORD const dwCurrentTime,int const iCoolTimeRate,int const iCastTimeRate);

	void Init();
	void InitCoolTime( MAP_COOLTIME const &kCoolTimeMap );

	bool CheckCoolTime(int const iSkillNo, DWORD const dwCurrentTime);
	bool CheckFilter(int const iSkillNo, SSFilter_Result* pkResult, ESkillFilterType eFilterType);
	PgSkillFilter* GetSkillFilter(int const iSkillNo) const;
	
	int		GetAbil(WORD const Type) const;
	__int64 GetAbil64(WORD const Type) const;
	
	bool Reserve(int const iSkillNo, bool const bForceSet = false, DWORD const dwCurrentTime = 0);
	bool ForceReserve(int const iSkillNo, EForceSetFlag const eForceSetFlag=EFSF_NONE, BM::Stream const& kParam=BM::Stream());	
	void ClearForceReserve();

	// about IGObject
	virtual int CallAction(WORD wSkillActCode, SActArg *pSkillActArg);

	virtual int Enter(SActArg *pActArg);
	virtual int Leave(SActArg *pActArg);

	void EffectFilter(bool const bAdd, int const iEffectID);

	void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType) const;
	void WriteToPacket_CoolTimeMap( BM::Stream &rkPacket )const;
	void ReadFromPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType);
	void ClearCoolTime();
	unsigned long GetCheckTime() const;
	void SetCheckTime(unsigned long const ulTime);
	void ReCalcCoolTime(DWORD const dwCurrentTime,int const iCoolTimeRate);

	SKILL_STACK::value_type PopSkillStack();
	void PushSkillStack(SKILL_STACK::value_type const& rkValue);
	void SwapSkillStack(SKILL_STACK& rkNewStack);

	bool IsForceSkill()const;
	bool IsForceSetFlag(int const iFlag)const;
	void DoForceSkill();
	int GetForceSkillNo()const;
	BM::Stream GetForceSkillParam()const;

public:
	typedef MAP_COOLTIME::const_iterator CONST_COOLTIME_INDEX;

	void GetFirstCoolTime(CONST_COOLTIME_INDEX& kIndex) const;
	bool GetNextCoolTime(CONST_COOLTIME_INDEX& kIndex, SCoolTimeInfo& rkInfo) const;

	// 스킬의 추가적인 쿨 타임 / 캐스팅 타임
	void SetSkillCoolTime(int const iAddCoolTime);
	void SetSkillCoolTimeRate(int const iAddCoolTime);
	void SetSkillCastingTime(int const iAddCastingTime);

	int	 GetSkillCoolTime() const { return m_iSkillCoolTime; }
	float GetSkillCoolTimeRate() const { return m_fSkillCoolTimeRate; }
	int  GetSkillCastingTime() const { return m_iSkillCastingTime; }	

	void AddCoolTime(int const iSkill, DWORD const dwCoolTime,int const iCoolTimeRate);
	DWORD GetCoolTime(int const iSkillNo) const;

	void GetCoolTimeMap( MAP_COOLTIME &kOutCont )const{kOutCont = m_kCoolTimeMap;}

	void SaveToDBCoolTimeMap( MAP_COOLTIME &kOutCont );
	void LoadFromDBCoolTimeMap( MAP_COOLTIME const &kOutCont );

	BM::Stream* CustomData(){ return &m_kCustomData; }
	
protected:
	mutable CSkillDef *m_pSkillDef;

	SKILL_STACK m_kSkillStack;
	int m_iSkillNo;

	CONT_FORCESETINFO m_kContForceSetInfo;
	
	unsigned long m_ulCheckTime; // Reserved Skill이 얼마나 오래 머물러 있는지 체크하기 위한 변수.. 좋은 이름 없을까;
	unsigned long m_ulTimeLimit;
	ESkillStatus m_eStatus;
	MAP_COOLTIME m_kCoolTimeMap;
	PgSkillFilterMgr m_kFilterMgr;

	int m_iSkillCoolTime;		//스킬의 추가적인 쿨 타임
	float m_fSkillCoolTimeRate;	//스킬의 추가적인 쿨 타임(상대값)
	int m_iSkillCastingTime;	//스킬의 추가적인 캐스팅 타임
	BM::Stream m_kCustomData;
};

typedef enum
{
	E_SCAST_INSTANT	= 1,
	//E_SCAST_MAINTENANCE = 2,
	E_SCAST_CASTSHOT = 4,
} ESkillCastType;

typedef enum
{
	ESTARGET_NONE		= 0,
	ESTARGET_SELF		= (1<<0),	// 1 나 자신
	ESTARGET_ENEMY		= (1<<1),	// 2 적
	ESTARGET_PARTY		= (1<<2),	// 4 Party member
	ESTARGET_LOCATION	= (1<<3),	// 8 Location
	ESTARGET_HIDDEN		= (1<<4),	// 16 Hidden unit
	ESTARGET_DEAD		= (1<<5),	// 32 Dead unit
	ESTARGET_ALLIES		= (1<<6),	// 64 아군
	ESTARGET_COUPLE		= (1<<7),	// 128 커플
	ESTARGET_GUILD		= (1<<8),	// 256 길드	
	ESTARGET_FRIEND		= (1<<9),	// 512 친구
	ESTARGET_CASTER		= (1<<10),	// 1024 캐스터(펫의 주인, 소환물의 캐스터)
	ESTARGET_SUMMONED	= (1<<11),	// 2048 자신이 소환한 소환체

} ESkillTargetType;

class CSkillDef
	:public CAbilObject
{
public:
	BYTE GetType()const { return m_byType; }
	virtual int GetAbil(WORD const wAbilType) const;
	virtual __int64 GetAbil64(WORD const wAbilType) const;
	//int GetNeedSkill()const { return m_iNeedSkill; }
	int GetParentSkill() const { return m_iParentSkill; }
	TCHAR const* GetActionName() const {return m_chActionName;}
	int GetEffectNo() const { return m_iEffectID; }
	bool IsSkillAtt(ESkillAttribute eType) const;
	int GetTargetType() const { return m_iTarget; }
	BYTE GetLevel() const { return m_byLv; }

protected:	
	CLASS_DECLARATION(int, m_iNo, No);
	CLASS_DECLARATION_V(int, m_NameNo, NameNo);
	CLASS_DECLARATION_V(int, m_RscNameNo, RscNameNo);

	BYTE m_byLv;
	BYTE m_byType;
	int m_iTarget;
	
	TCHAR m_chActionName[30];

	__int64 m_i64ClassLimit;
	short int m_sLevelLimit;

	short int m_sRange;

	short int m_sMP;
	short int m_sHP;

	int m_iWeaponLimit;
	int m_iStateLimit;
	int m_iParentSkill;
	int m_sCastTime;
	int m_sCoolTime;
	int m_iAnimationTime;
	int m_iEffectID;	
};

class CSkillDefMgr
	:	public TDefMgr< TBL_DEF_SKILL, TBL_DEF_SKILLABIL, CSkillDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< CSkillDefMgr >;
public:
	CSkillDefMgr();
	~CSkillDefMgr();

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	int GetSkillNum(std::wstring const &rkSkillName) const;
	int GetAbil(int const iDefNo, WORD const wType) const;
	__int64 GetAbil64(int const iDefNo, WORD const wType) const;
	int GetSkillNoFromActionName(std::wstring const &rkActionName, bool const bIgnoreNameCase = false) const;
	std::wstring const GetActionName(int const iDefNo) const;
	int GetLevelupSkill(int const iID, int const iLevel) const;
	int GetCallSkillNum(int const iID) const;
	void swap(CSkillDefMgr& rkRight);
	int GetNeedSkillDepth(int const iSkillNo, int& iCallDepth) const;

private:

	void Clear();
	typedef std::map<std::wstring, CSkillDef*> CONT_SKILLDEF_NAME;
	CONT_SKILLDEF_NAME m_kSkillDef_ActionName;
	CONT_SKILLDEF_NAME m_kSkillDef_ActionName_Upper;
};

//#define g_SkillDefMgr SINGLETON_STATIC(CSkillDefMgr)

#endif // WEAPON_VARIANT_SKILL_EFFECT_SKILL_H