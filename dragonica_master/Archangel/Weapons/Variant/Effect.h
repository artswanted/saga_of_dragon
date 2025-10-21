//// Effect Object Class
// Dukguru
//

#ifndef WEAPON_VARIANT_SKILL_EFFECT_EFFECT_H
#define WEAPON_VARIANT_SKILL_EFFECT_EFFECT_H

#include <map>

#include "TDefMgr.h"
#include "IGObject.h"
#include "IDObject.h"
#include "Lohengrin/dbtables.h"
#include "ItemDefMgr.h"

class CEffectDef;

typedef enum
{
	ECT_NONE = 0,
	ECT_DOTICK = 2,
	ECT_MUSTDELETE = 4,
	ECT_TICKBREAK = 8,
	ECT_RESET = 16,
} ECT_CheckTick;

typedef	enum
{
	EVS_VISIBLE=0,
	EVS_HIDE
}	EffectVisualState;

typedef enum
{
	EFFECT_TYPE_NONE			= 0,
	EFFECT_TYPE_BLESSED			= 1,	// 버프타입의 Effect
	EFFECT_TYPE_CURSED			= 2,	// 디버프 타입 effect
	EFFECT_TYPE_PASSIVE			= 3,	// Passive skill로 생성된 effect
	EFFECT_TYPE_NORMAL			= 4,	// EffectDef값에서 Type 읽어 오도록 하고 싶을때 설정하는 값)
	EFFECT_TYPE_PENALTY			= 5,	// 패널티
	EFFECT_TYPE_ITEM			= 6,
	EFFECT_TYPE_GROUND			= 7,	// GroundEffect
	EFFECT_TYPE_ABIL			= 8,
	EFFECT_TYPE_ACTIVATE_SKILL	= 9,	// 스킬을 활성화 시키는 이펙트
	EFFECT_TYPE_SEND_GROUND		= 10,	// SendGroundEffect
	EFFECT_TYPE_SYSTEM			= 11,	// 시스템 타입(유저가 삭제할 수 없음)
	EFFECT_TYPE_TEMP			= 16,
} EEffectType;

typedef enum
{
	EFTMT_NONE		= 0,
	EFTMT_MODE7		= 1,		// 전략디펜스7에서 사용
} EEffectMsgType;

typedef enum eEffectNoDef
{
	EFFECTNO_DEFAULT					= 0,//! 기본 타입
	EFFECTNO_MAX_HP_AND_MP				= 1, //HP, MP 최대치 증가 및 최대치까지 회복
	EFFECTNO_RESURRECTION01				= 11101, // 부활 이펙트 번호
	EFFECTNO_CANNOT_DAMAGE				= 11201,//부활 무적
	EFFECTNO_PVP_HANDYCAP				= 14001,
	EFFECTNO_DRAGON_SELECT_DELAYTIME	= 18001,//드래곤 방어전 지역 선택 딜레이 이펙트
	EFFECTNO_MUTECHAT					= 20001,//채팅금지 이펙트
	EFFECTNO_CANNOT_DAMAGE_CORE_BASE	= 406700,//무한 무적
	EFFECTNO_PVP_NOTDASH				= 406601,//PvP 대쉬 금지
	EFFECTNO_DRESS_WAR_RED				= 60201,
	EFFECTNO_DRESS_WAR_BLUE				= 60301,
	EFFECTNO_TRANSTOWER_FREE			= 650001,//전송타워 자유이용권
	EFFECTNO_AWAKE_NORMAL_STATE			= 2000000101, //각성기 시스템 노말 상태(각성치 증가 하는 상태)
	EFFECTNO_AWAKE_MAX_STATE			= 2000000201, //각성기 시스템 맥스 유지 상태(각성치 감소 하지 않는 상태)
	EFFECTNO_AWAKE_PENALTY				= 2000000301, //각성기 시스템 페널티 상태(Max 에서 스킬 사용 후 틱당 감소하는 상태)
	EFFECTNO_AWAKE_SKILL_ADD_EFFECT_NO	= 91000000, // 각성기 사용시 걸어주는 슈퍼 아머 이펙트 번호
	EFFECTNO_MASTER_SKILL_ADD_EFFECT_NO = 91000001, // 궁극기 사용시 걸어주는 무적 이펙트 번호
	EFFECTNO_PETSKILL_ACTIVATE_BASE		= 80020001,	// 펫 스킬 활성화

	EFFECTNO_CHARACTER_BONUS_EFFECT_BASENO	= 412901,// 케릭터 생성 유도 시스템의 보너스 이펙트 번호	
}EEffectNoDef;

typedef enum
{
	EDET_NONE			= 0,
	EDET_EFFECT_NO		= 0x01,	//이펙트 번호를 통해서 제거
	EDET_ABIL_01		= 0x02,	//AT_EFFECTNUM1 번호를 통해서 제거
} EDelEffectType;

typedef struct tagSEffectCreateInfo
{
	EEffectType eType;
	int iEffectNum;
	//int iItemNum;
	int iValue;
	SActArg kActArg;
	DWORD dwElapsedTime;
	DWORD dwInterval;
	DWORD dwStartTime;
	DWORD dwEndTime;
	BM::DBTIMESTAMP_EX kWorldExpireTime;

	typedef enum
	{
		ECreateOption_None = 0x0000,
		ECreateOption_CallbyServer = 0x0001,
		ECreateOption_CallbyClient = 0x0002,
	} ECreateOption;
	ECreateOption eOption;

	tagSEffectCreateInfo()
	{
		eType = EFFECT_TYPE_NONE;
		iEffectNum = 0;
		//iItemNum = 0;
		iValue = 0;
		dwElapsedTime = dwInterval = 0;
		dwStartTime = dwEndTime = 0;
		eOption = ECreateOption_None;
	}

	tagSEffectCreateInfo const& operator=(tagSEffectCreateInfo const& rhs)
	{
		eType = rhs.eType;
		iEffectNum = rhs.iEffectNum;
		//iItemNum = rhs.iItemNum;
		iValue = rhs.iValue;
		kActArg = rhs.kActArg;
		dwElapsedTime = rhs.dwElapsedTime;
		dwInterval = rhs.dwInterval;
		dwStartTime = rhs.dwStartTime;
		dwEndTime = rhs.dwEndTime;
		kWorldExpireTime = rhs.kWorldExpireTime;
		return (*this);
	}

	bool IsNull()const
	{
		return 0==iEffectNum;
	}

} SEffectCreateInfo;

enum EEffectDeleteCause
{
	E_EDC_NONE = 0,
	E_EDC_DELETE_BY_HIGH_LEVEL = 1,
};

#define EFFECT_DBTYPE_NORMAL	0x01
#define EFFECT_DBTYPE_ACTION	0x02

#define EFFECT_ALL_MYEFFECT_NUM	0xFFFFFFFF

class CEffect : public IGObject
{
public:
	CEffect();
	virtual ~CEffect();
	CEffect( CEffect const &rhs );

	CEffect& operator=( CEffect const &rhs );
public:
	static int GetLevel(int const iEffectNo);

	int const GetEffectNo() const;	

	int const GetValue() const;
	void	SetValue(int const iValue);
	int		AddValue(int const iValue /* negativable */);

	unsigned long	GetTime() const;

	void			SetInterval(unsigned long ulInterval);
	unsigned long	GetInterval() const;

	virtual void Init();
	int	 GetDurationTime() const;
	
	int GetAddDurationTime() const;
	void SetAddDurationTime(int const iAddTime);

	// about IGObject
	//virtual TCHAR const* GetName();
	virtual int  CallAction(WORD wEffectActCode, SActArg *pEffectActArg);
	virtual int  Enter(SActArg *pActArg){return 0;}
	virtual int  Leave(SActArg *pActArg){return 0;}
	//virtual int Register();
	virtual int	 Level() const;
	virtual int	 GetKey() const;
	virtual bool const IsSame(int const iEffectKey, bool const bInGroup) const;

	//virtual void Begin(BYTE const byType, int const iEffectNo, int const iValue, SActArg const* pkAct, unsigned long const ulElapsedTime, unsigned long const ulInterval);
	virtual void Begin( SEffectCreateInfo const& rkCreate, bool const bRestart );
	//void		 Restart(int const iValue, SActArg const* pkArg, unsigned long const ulElapsedTime);
	
	ECT_CheckTick	CheckTick( unsigned long const ulElapsed, bool const bDamaged );

	typedef enum : BYTE
	{
		ESAVE_DB_TYPE_NOSAVE = 0,			// Save하지 않는다.
		ESAVE_DB_TYPE_PLAYTIME_ELAPSED_5S = 1,	// Play time 으로 expire 되는 시점을 저장한다.(5초간격)
		ESAVE_DB_TYPE_WORLDTIME_EXPIRE = 2,	// World 시간으로 Expire 되는 시점을 저장한다.
	} ESAVE_DB_TYPE;
	virtual ESAVE_DB_TYPE GetDBSaveType() const;
	
	virtual int	GetAbil(WORD const Type) const;
	
	void		SetCaster(BM::GUID const &rkGuid) { m_kCasterGuid = rkGuid; }
	BM::GUID const&	GetCaster() const { return m_kCasterGuid; }

	//동일한 Effect를 거는 caster GUID
	void		AddCasterEx(BM::GUID const &rkGuid);
	bool		IsCasterEx(BM::GUID const &rkGuid);
	void		DeleteCasterEx(BM::GUID const &rkGuid);
	void		DeleteCasterExFirst();
	int			GetCasterExSize() { return (int)m_kCasterExList.size(); }

	CEffectDef const* GetEffectDef() const;

	void		SetInstanceID(int const iInstanceID) { m_iInstanceID = iInstanceID; }
	int			GetInstanceID() const { return m_iInstanceID; } 

	void		SetType(BYTE byType) { m_byType = byType; }
	BYTE const	GetType() const { return m_byType; }

	void		SetActArg(int const iType, void const* pValue);
	void		SetActArg(int const iType, int const iValue);
	HRESULT		GetActArg(int const iType, int &kOutValue) const;
	SActArg*	GetActArg() { return &m_kActArg; }
	
	void				SetVisualState(EffectVisualState kState) {	m_EffectVisualState = kState;	}
	EffectVisualState	GetVisualState() const {	return	m_EffectVisualState;	}

	void			SetStartTime(unsigned long ulStartTime)	{	m_ulStartTime	=	ulStartTime;	}
	unsigned long	GetStartTime() const	{	return	m_ulStartTime;	}

	bool 	IsSendingTypeDuringMapMove() const;

	//int GetActionInstanceID() { return m_iActionInstanceID; }
	
	//Target GUID List
	bool		AddTarget(BM::GUID const &rkGuid);
	bool		DeleteTarget(BM::GUID const &rkGuid);
	bool		IsTarget(BM::GUID const &rkGuid);
	BM::GUID	GetTarget(int const nIndex) const;	
	int			GetTargetListSize() const { return (int)m_kTargetList.size(); }
	VEC_GUID&	GetTargetList() { return m_kTargetList; }

	unsigned long GetEndTime() const { return m_ulEndTime; }
	void SetEndTime(unsigned long const ulTime)	{ m_ulEndTime = ulTime; }

	// 서브 이펙트가 몇개가 등록되어있는지 얻어온다.
	int			GetSubEffectSize() { return static_cast<int>(m_kSubEffectNo.size()); }
	// 해당 인덱스에 있는 서브이펙트번호를 얻어온다.
	int			GetSubEffectNoByIndex(int const iIndex) const;
	// 이펙트 번호가 등록되어있는지 얻어온다. 없으면 0을 리턴
	int			GetSubEffectNo(int const iSubEffectNo, bool const bInGroup = false) const;
	// 이펙트 번호를 추가
	void		AddSubEffectNo(int const iSubEffectNo);
	void		DeleteSubEffectNo(int const iSubEffectNo);

	// 서브 이펙트의 컨테이너를 외부의 컨테이너와 스왑시킨다.
	void		swap_SubEffectCont(std::vector<int>& kSubEffectCont); 
	
	void		SetDeleteCause(int const iCause) { m_iDeleteCause = iCause; }
	int			GetDeleteCause() const { return m_iDeleteCause;	}

	// PT_U_G_NFY_DELETE_EFFECT가 호출이 되어도 GetEffect()에서 얻을수 있기 때문에 지워지고 있는 중인지 판단하는 용도
	bool		IsDelete() const { return m_bDelete; }
	void		SetDelete() { m_bDelete = true; }
protected:
	int GroupNo() const;
	static int GroupNo(int const iEffectNo);

protected:
	CLASS_DECLARATION_S(BM::DBTIMESTAMP_EX, ExpireTime);
	
private:
	BYTE m_byType;	// EFFECT_TYPE_XXXX
	int m_iEffectNo;
	int m_iValue;
	unsigned long m_ulTime;
	unsigned long m_ulInterval;
	unsigned long m_ulEndTime;

	mutable CEffectDef* m_pEffectDef;

	SActArg m_kActArg;
	BM::GUID m_kCasterGuid;
	int m_iInstanceID;

	EffectVisualState	m_EffectVisualState;
	unsigned	long	m_ulStartTime;

	VEC_GUID m_kTargetList;
	VEC_GUID m_kCasterExList;

	typedef std::vector<int> CONT_INT;
	// AT_CHILD_EFFECT_NUM_01 ~ AT_CHILD_EFFECT_NUM_10 어빌값이 있을 경우 어빌 처리하면서 세팅
	// 세팅 후 리셋하지 않으며 이펙트가 지워질때 얻어와서 해당 처리를 해준다.
	//이펙트가 걸릴때 같이 걸어주는 이펙트
	CONT_INT	m_kSubEffectNo;
	int m_iDeleteCause;
	bool m_bDelete;

	int m_iAddDurationTime;
};

class CItemEffect : public CEffect
{
public:
	CItemEffect();
	virtual ~CItemEffect();
	CItemEffect( CItemEffect const &rhs );

	CItemEffect& operator=( CItemEffect const &rhs );

	virtual void Init();
	virtual int Level() const;
	virtual int GetKey() const;
	//virtual void Begin(BYTE const byType, int const iEffectNo, int const iValue, SActArg const* pkAct, unsigned long const ulElapsedTime, unsigned long const ulInterval);
	virtual void Begin(SEffectCreateInfo const& rkCreate, bool const bRestart );
	virtual int GetAbil(WORD const Type) const;
	virtual bool const IsSame(int const iEffectKey, bool const bInGroup) const;

	static int GetLevel(int const iItemNo);
	static int const GetAbil(int const iItemNo, WORD const wType);
protected:
	int GroupNo() const;
	CItemDef const* GetItemDef() const;

private:
	CLASS_DECLARATION_S(int , ItemNo);
	CLASS_DECLARATION_S(int , ParentItemNo);

	mutable CItemDef* m_pItemDef;
};

class CEffectDef
	:public CAbilObject
{
public:
	virtual int GetAbil(WORD const wAbilType) const;

	short int GetInterval() const {return m_sInterval;}
	int GetDurationTime() const{return m_iDurationTime;}
	BYTE GetToggle() const{return m_byToggle;}
	short int GetType() const{ return m_sType; }
	std::wstring const GetActionName()const{return m_strActionName;}
	bool IsUseMapNo(int const iMapNo)const;
	void SetContUseMapNo(int const iValue);

protected:	
	CLASS_DECLARATION(int, m_iEffectID, EffectID);
	int m_iName;
	std::wstring m_strActionName;
	short int m_sType;
	short int m_sInterval;
	int m_iDurationTime;
	BYTE m_byToggle;
	SET_INT m_kContUseMapNo;
};

class CEffectDefMgr
	:	public TDefMgr< TBL_DEF_EFFECT, TBL_DEF_EFFECTABIL, CEffectDef, TBL_KEY_INT, TBL_KEY_INT  >
{
	friend struct ::Loki::CreateStatic< CEffectDefMgr >;
public:
	CEffectDefMgr();
	~CEffectDefMgr();

public:
	virtual bool Build(const CONT_BASE &tblBase, const CONT_ABIL &tblAbil);
	int GetAbil(int const iDefNo, WORD const wType) const;
	void swap(CEffectDefMgr& rkRight);
	int GetCallEffectNum(int const iEffectNo) const;
};

//#define g_kEffectDefMgr SINGLETON_STATIC(CEffectDefMgr)

#endif // WEAPON_VARIANT_SKILL_EFFECT_EFFECT_H