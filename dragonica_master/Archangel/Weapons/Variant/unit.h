//// Unit(Movable Objects) Class
// Dukguru
//

#ifndef WEAPON_VARIANT_UNIT_UNIT_H
#define WEAPON_VARIANT_UNIT_UNIT_H

#include <map>

#include "randomlib/Random.hpp"
#include "randomlib/Randomtype.hpp"
#include "BM/Observer.h"
#include "BM/GUID.h"
#include "def.h"
#include "IGObject.h"
#include "IDObject.h"
#include "defAbilType.h"
#include "Skill.h"
#include "Inventory.h"
#include "constant_ai.h"
#include "PgUnitEffectMgr.h"
#include "PgAggroMeter.h"
#include "PgNetModule.hpp"

class UNIT_PTR_ARRAY;
class CEffect;
class CUnit;

float const PG_CHARACTER_CAPSULE_HEIGHT = 40.0f;
float const PG_CHARACTER_CAPSULE_RADIUS	= 5.0f;
float const PG_CHARACTER_Z_ADJUST = PG_CHARACTER_CAPSULE_HEIGHT * 0.5f + PG_CHARACTER_CAPSULE_RADIUS;

int const MAX_AI_PARAM	= 5;
int const HP_VALUE_KILL_NOTADDED_UNIT  = -999;
int const HP_VALUE_MISSION_MAP_PENALTY = -998;

int const DEFAULT_DMG_CHECK_TIME = 500; //���� ������ �Դ� �ð� üũ

int const MAX_PROJECTILE_COUNT = 10; // �������� ����� �߻�ü �ִ� ����
int const MAX_SUMMONUNIT_COUNT = 8;

typedef enum eEffectQueueType
{
	EQT_NONE			= 0,
	EQT_ADD_EFFECT		= 1,
	EQT_DELETE_EFFECT	= 2,
} EffectQueueType;

typedef struct _tagEffectQueueData
{
	EffectQueueType m_kEffectProcessType;
	int				m_kEffectNo;
	int				m_kValue;
	SActArg			m_kArg;
	BM::GUID		m_kCasterGuid;
	BYTE			m_kEffectType;
	bool			m_bInGroup;

	_tagEffectQueueData() :
		m_kEffectProcessType(EQT_NONE), 
		m_kEffectNo(0), 
		m_kValue(0), 
		m_kEffectType(EFFECT_TYPE_NONE),
		m_bInGroup(false)
	{}

	_tagEffectQueueData(EffectQueueType const kQueueType, int const kEffectNo, int const kValue = 0, SActArg const* pkArg = NULL, BM::GUID const& kCasterGuid = BM::GUID::NullData(), BYTE const kEffectType = EFFECT_TYPE_NONE) :
		m_kEffectProcessType(kQueueType), 
		m_kEffectNo(kEffectNo), 
		m_kValue(kValue), 
		m_kCasterGuid(kCasterGuid),
		m_kEffectType(kEffectType),
		m_bInGroup(false)
	{
		if(pkArg)
		{
			m_kArg = *pkArg;
		}
	}

	bool operator == (_tagEffectQueueData const &rhs) const
	{
		return		(m_kEffectProcessType == rhs.m_kEffectProcessType)
				&&	(m_kEffectNo == rhs.m_kEffectNo)
				&&	(m_kValue == rhs.m_kValue)
				&&	(m_kCasterGuid == rhs.m_kCasterGuid)
				&&	(m_kEffectType == rhs.m_kEffectType)
				&&	(m_bInGroup == rhs.m_bInGroup);
		//SActArg�� ������ ����. ���� �����͸����ε� ����� ���� ����
	}

	bool operator != (_tagEffectQueueData const &rhs) const
	{
		return	!(*this==rhs);
	}

} EffectQueueData;
typedef std::list< EffectQueueData >  CONT_EFFECT_QUEUE;

typedef enum eUnitState
{
	// Bit mask valule
	US_BIT_CANT_MOVE			= 0x00010000,	// ������ �� ���� ���� (���������� ������ �� ���� ����)
	US_BIT_CANT_DAMAGED			= 0x00020000,	// Damage�� ���� �� ���� ����
	US_BIT_CANT_CHANGE			= 0x00040000,
	US_BIT_WAITDELAY_BYFORCE	= 0x00080000,
	US_BIT_CANT_ATTACK			= 0x00100000,	// ������ �� ���� ����
	US_BIT_CANT_DSKILL			= 0x00200000,	// �ڽſ��� �ɸ� Skill�� ���� �� �� ���� ����
	// GM Ability (0xF0000000 ���� bit�ڸ��� �������鼭 �Է��� ����)
	US_BIT_INVISIBLE			= 0xF0000000,	// ���Ͱ� �� �� ���� ����

	// Monster Status
	US_NONE					= 0,
	US_DEAD					= 1 | US_BIT_CANT_MOVE | US_BIT_CANT_DAMAGED,
	US_ATTACK				= 2,
	US_MOVE					= 3,
	US_IDLE					= 4,
	//US_GOD_MODE				= 5 | US_BIT_CANT_DAMAGED,
	//US_PLAY_SCENE			= 6 | US_BIT_WAITDELAY_BYFORCE,
	US_MOVEBYFORCE			= 7,		// Monster �ൿ�ݰ��� ��迡 ���� ��, Player �����ϰ� ���ư��� ���� �ൿ
	//US_WATCHPLAYER			= 8,		// ������ ���� ���������� �ü��� ������ �ִ�!
	//US_ATTACKTIME			= 9,
	//US_ATTACKDELAY			= 10,
	//US_BLOW_UP				= 11 | US_BIT_CANT_MOVE | US_BIT_CANT_ATTACK,		// �¾Ƽ� ���߿� �� ����. ��� AI ����.
	//US_INIT					= 12,
	US_PUSHED_BY_DAMAGE		= 13 | US_BIT_CANT_MOVE | US_BIT_CANT_ATTACK,
	US_DAMAGEDELAY			= 14 | US_BIT_CANT_MOVE | US_BIT_CANT_ATTACK,	// Damage ���� ���� (�ٸ� Skill�� ���� Damage ���·� ��ȯ ����)
	US_DAMAGEDELAY_NOCHANGE = 15 | US_BIT_CANT_MOVE | US_BIT_CANT_ATTACK | US_BIT_CANT_DSKILL, // Damage ���� ���� (�ٸ� Skill�� ���� Damage ���·� ��ȯ �Ұ�)
	US_OPENING			= 16 | US_BIT_CANT_MOVE | US_BIT_CANT_DAMAGED | US_BIT_CANT_CHANGE , // ���� �׼����̴�(���̵����̴�)
//	US_DEADREADY		= 17 | US_BIT_CANT_MOVE | US_BIT_CANT_DAMAGED,	// ���� �����̸�, ���̵� ������ ���� (US_DEAD -> Death Penalty ����(����ġ���̰�) -> US_DEADREADY)
	//US_FROZEN			= 18 | US_BIT_CANT_MOVE | US_BIT_CANT_ATTACK | US_BIT_CANT_DSKILL,	// ����� (��� �׼� ����, �Ѵ� ������ Ǯ��)
	US_SKILL_CAST		= 19,
	US_HIDE_IN			= 20,	// AI : Hide ���·� ����
	US_HIDE_OUT			= 21,	// AI : Hide ���¿��� ������
	US_CHASE_ENEMY		= 22,
	US_AROUND_ENEMY		= 23,
//	US_PORTAL			= 24,	// Portal�� ����!!!
	US_SKILL_FIRE		= 25,
	US_RETREAT			= 26,	// ���ڸ���.. 
	US_RESURRECTION		= 27,	// �׾����� ���������� ��Ȱ
	US_FIRE_WAITING		= 28,
	US_GHOST			= 29|US_BIT_CANT_DAMAGED|US_BIT_CANT_ATTACK,
	US_ACTION_LOCK		= 30|US_BIT_CANT_DAMAGED|US_BIT_CANT_CHANGE|US_BIT_CANT_ATTACK|US_BIT_CANT_DSKILL,// Action�� �� �� ���� ����
	US_LAST,
}EUnitState;

typedef enum eUnitType
{
	UT_NONETYPE			= 0x00000000,
	UT_PLAYER			= 0x00000001,
	UT_MONSTER			= 0x00000002,
	UT_GROUNDBOX		= 0x00000004,
	UT_PET				= 0x00000008,
	UT_NPC				= 0x00000010,
	UT_BOSSMONSTER		= 0x00000020| UT_MONSTER,
	UT_SUMMONED			= 0x00000040,

	UT_ENTITY			= 0x00000100,// ���°� ���� ��ü(�����ٹ�����..)
	UT_OBJECT			= 0x00000200,// Break Object
	UT_FURNITURE		= 0x00000400,// ����Ȩ ������
	UT_MYHOME			= 0x00000800,

	UT_UNIT_AI			= UT_BOSSMONSTER| UT_ENTITY| UT_SUMMONED,

	UT_SUB_PLAYER		= 0x00001000,// �ֵ��� ���� ĳ���� ���� Ÿ��
	UT_CUSTOM_UNIT		= 0x00002000,//Ư���� ����� ���� ����� ���� ������Ʈ. �����ý����� ����� ���� �߰��ߴ�

	UT_OBSERVER			= 0x01000000,
	UT_ALLUNIT			= 0xFFFFFFFF,
}EUnitType;

typedef enum eEntityType
{
	ENTITY_NONE			= 0,
	ENTITY_PARTICLE		= 1,
	ENTITY_GUARDIAN		= 2,
}EEntityType;

typedef enum eEntitySecondType
{
	ENTITY_SEC_NONE			= 0,
	ENTITY_SEC_STORM		= 1,
    ENTITY_SEC_BLAZE        = 2,
    ENTITY_SEC_GAIA         = 3,
    ENTITY_SEC_LIGHTNING    = 4,
    ENTITY_SEC_FREEZING     = 5,
	ENTITY_SEC_BLACK_HOLE	= 6,

	ENTITY_SEC_KTH_BASE_HILL	= 100, //Base ����
	ENTITY_SEC_KTH_HILL			= 101, //����
}EEntitySecondType;

typedef struct tagUnitCommonAbil
{
	tagUnitCommonAbil()
	{
		sLevel = 0;
		byGender = 0;
		iClass = 0;
		iRace = 0;
	}

	tagUnitCommonAbil& operator=( tagUnitCommonAbil const &rhs )
	{
		sLevel = rhs.sLevel;
		byGender = rhs.byGender;
		iClass = rhs.iClass;
		iRace = rhs.iRace;
		return *this;
	}

	short sLevel;
	BYTE byGender;
	int iClass;//���� ��ȣ�� �� ���� ������ int ������ �ؾ� �Ѵ�.
	int iRace;
}UNIT_COMMON_ABIL;

typedef enum eSyncType
{
	SYNC_TYPE_NONE		= 0x00,
	SYNC_TYPE_SEND_ADD	= 0x01,	// ������ AddUnit Packet�� ������.
	SYNC_TYPE_RECV_ADD	= 0x02,	// ������ ���� AddUnit Packet�� �޴´�.
	SYNC_TYPE_DEFAULT	= SYNC_TYPE_SEND_ADD|SYNC_TYPE_RECV_ADD,
}ESyncType;

typedef enum
{
	E_SENDTYPE_NONE =				0x0000,

	E_SENDTYPE_SELF =				0x0001,
	E_SENDTYPE_BROADCAST =			0x0002,
	E_SENDTYPE_BROADCAST_GROUND =	0x0004,	// Ground ��ü�� BroadCase 
	E_SENDTYPE_MUSTSEND =			0x0008,	// �ݵ�� ������ �Ѵ�.
	E_SENDTYPE_EFFECTABIL =			0x0010,	// Effect Abil ���� ������.
	E_SENDTYPE_SEND_BYFORCE =		0x0020,	// Unit�� ���¿� ������� ������ ������.
	
	E_SENDTYPE_BROADALL	= E_SENDTYPE_SELF|E_SENDTYPE_BROADCAST,
	E_SENDTYPE_BROADALL_EFFECTABIL = E_SENDTYPE_BROADALL | E_SENDTYPE_EFFECTABIL,
	E_SENDTYPE_BROADALL_EFFECTABIL_MUST = E_SENDTYPE_BROADALL | E_SENDTYPE_EFFECTABIL | E_SENDTYPE_MUSTSEND,

	E_SENDTYPE_AREATYPE_BITMASK	= E_SENDTYPE_SELF | E_SENDTYPE_BROADCAST | E_SENDTYPE_BROADCAST_GROUND,
	E_SENDTYPE_OPTION			= E_SENDTYPE_MUSTSEND | E_SENDTYPE_SEND_BYFORCE,
}E_SENDTYPE_TYPE;

typedef enum
{
	E_SENDABIL_TARGET_NONE = 0,
	E_SENDABIL_TARGET_UNIT = 1,
	E_SENDABIL_TARGET_EFFECT = 2,
} E_SENDABIL_TARGET;

typedef enum
{
	DIR_NONE		= 0x00,
	DIR_LEFT		= 0x01,
	DIR_RIGHT		= 0x02,
	DIR_UP			= 0x04,
	DIR_DOWN		= 0x08,
	
	DIR_LEFT_UP		= DIR_LEFT|DIR_UP,
	DIR_RIGHT_UP	= DIR_RIGHT|DIR_UP,
	DIR_LEFT_DOWN	= DIR_LEFT|DIR_DOWN,
	DIR_RIGHT_DOWN	= DIR_RIGHT|DIR_DOWN,

	DIR_HORIZONTAL	= DIR_LEFT|DIR_RIGHT,
	DIR_VERTICAL	= DIR_UP|DIR_DOWN,
	DIR_ALL			= 0x0F,
	DIR_LOOKING		= 0x10,
} Direction;

typedef enum eAIType
{
	AIT_OFFENSIVE		= 1,	// �����ϱ�
	AIT_CANNOTMOVE		= 2,	// ������ �� ����
	AIT_NONOFFENSIVE	= 3,	// �񼱰�
	AIT_RUNAWAY		    = 4,	// HP������ ���� ��������
	AIT_CALLHELP		= 5,	// HP������ ���� �������鼭 �����û�ϱ�
	AIT_HIDE_IDLE		= 6,	// IDLE �����϶� HIDE�ϱ�
}EAIType;

typedef struct tagClassKey SClassKey;

typedef struct tagSTargetInfo
{
	enum : BYTE {
		ETState_None = 0,
		ETState_Targeted = 1,	// ��� Ÿ���� �� ��
		ETState_Damaged = 2,	// Damage���� �� ��
	} eState;
	DWORD dwDmgTime;
	BM::GUID kGuid;

	tagSTargetInfo(BM::GUID const &rkGuid)
	{
		kGuid = rkGuid;
		eState = ETState_Targeted;
		dwDmgTime = 0;
	}

} STargetInfo;

typedef struct
{
	POINT3 ptCurrent;	// ���� ��ġ
	POINT3 ptTarget;	// ��ǥ ����
	int iMoveSpeed;		// �̵��ӵ�
	DWORD dwFlyTime;	// ���ư� �ð�(���ѽð� ���� �̵��ϴ°��� ���� ���� �ð� ��)

	void Begin(POINT3 const &ptBegin, POINT3 const &ptEnd, int const iSpeed)
	{
		ptCurrent = ptBegin;
		ptTarget = ptEnd;
		iMoveSpeed = iSpeed;
		dwFlyTime = 0;
	}
	bool Move(DWORD const dwElapsedTime)
	{
		dwFlyTime += dwElapsedTime;
		if (dwFlyTime > 10000)	// 10���̻� ���ư��°� ������?
		{
			return false;
		}
		return true;
	}
} SProjectileInfo;

typedef struct tagSClientProjectile
{
	tagSClientProjectile()
	{
		ptFirePos.Set(0, 0, 0);
		ptDirection.Set(0, 0, 0);
		iActionID = 0;
		dwExpireTime = 0;
		byMaxProjectile = byMaxTargetPerProjectile = 0;
		fDmgRangeQ = 0.0f;
	}

	tagSClientProjectile(tagSClientProjectile const & rhs)
	{
		ptFirePos = rhs.ptFirePos;
		ptDirection = rhs.ptDirection;
		iActionID = rhs.iActionID;
		dwExpireTime = rhs.dwExpireTime;
		byMaxProjectile = rhs.byMaxProjectile;
		kUsed.resize(byMaxProjectile, 0);	// kUsed�� �ʱ�ȭ ���� ����.
		byMaxTargetPerProjectile = rhs.byMaxTargetPerProjectile;
		fDmgRangeQ = rhs.fDmgRangeQ;
	}

	tagSClientProjectile const & operator=(tagSClientProjectile const & rhs)
	{
		ptFirePos = rhs.ptFirePos;
		ptDirection = rhs.ptDirection;
		iActionID = rhs.iActionID;
		dwExpireTime = rhs.dwExpireTime;
		byMaxProjectile = rhs.byMaxProjectile;
		kUsed.resize(byMaxProjectile, 0);	// kUsed�� �ʱ�ȭ ���� ����.
		byMaxTargetPerProjectile = rhs.byMaxTargetPerProjectile;
		fDmgRangeQ = rhs.fDmgRangeQ;
		return (*this);
	}

	POINT3 ptFirePos;
	POINT3BY ptDirection;
	float fDmgRangeQ;	// Range�� ������
	int iActionID;
	DWORD dwExpireTime;
	mutable std::vector<BYTE> kUsed;
	BYTE byMaxProjectile;
	BYTE byMaxTargetPerProjectile;
} SClientProjectile;

typedef struct tagSummonUnitInfo
{
	tagSummonUnitInfo(BM::GUID const &_kGuid, int const _iClassNo=0)
		:	kGuid(_kGuid)
		,	iClassNo(_iClassNo)
	{}

	bool operator<(const tagSummonUnitInfo& rhs)const
	{
		return kGuid < rhs.kGuid;
	}
	bool operator>(const tagSummonUnitInfo& rhs)const
	{
		return kGuid > rhs.kGuid;
	}
	bool operator==(const tagSummonUnitInfo& rhs)const
	{
		return kGuid == rhs.kGuid;
	}

	bool operator<(BM::GUID const &_kGuid)const
	{
		return kGuid < _kGuid;
	}
	bool operator>(BM::GUID const &_kGuid)const
	{
		return kGuid > _kGuid;
	}
	bool operator==(BM::GUID const &_kGuid)const
	{
		return kGuid == _kGuid;
	}

	bool operator==(int const _iClassNo)const
	{
		return iClassNo == _iClassNo;
	}

	BM::GUID	kGuid;
	int			iClassNo;
}SSummonUnitInfo;
typedef std::vector<SSummonUnitInfo>	VEC_SUMMONUNIT;
typedef std::set<SSummonUnitInfo>		SET_SUMMONUNIT;
typedef std::vector<std::pair<SSummonUnitInfo,DWORD> >	VEC_DIE_SUMMONUNIT;

typedef enum eSummonOptionType
{
	ESO_NONE				=	0,
	ESO_UNIQUE_CLASS		=	0x01,
	ESO_IGNORE_MAXCOUNT		=	0x02,
} SummonOptionType;

#define S_MANUAL_OPENING S_FALSE

typedef std::list< PgBase_Item > CONT_MONSTER_DROP_ITEM;
typedef std::list< std::pair< SERVER_IDENTITY, BM::GUID > > CONT_OBSERVER_SWITH_MEMBER_LIST;
typedef std::deque<POINT3> DEQUE_POS;

typedef enum : short
{
	EUnitTick_DoAll = 0xFFFF,
	EUnitTick_AutoHeal = 0x0001,
} EUnitTick_Op;

namespace EGetAbilType
{
	enum Enum
	{
		EGAT_NONE		= 0x00,
		EGAT_BASE		= 0x01,
		EGAT_EFFECT		= 0x02,	//���� ��� ����
		EGAT_INV		= 0x04,
		EGAT_UNIT		= (EGAT_BASE | EGAT_EFFECT),
		EGAT_ALL		= 0xFF,
	};
};

class CUnit 
	:	public IDObject
	,	public BM::CSubject< BM::Stream* >//������ ���� ���
{
protected:
	//static EffectPool m_kEffectPool;
	friend class PgObjectMgr2;

public:
	// constructor & destructor
	CUnit();
	virtual ~CUnit();

public:
	typedef std::deque<STargetInfo>	DequeTarget;
	typedef std::set<int> CONT_UNIQUE_INT;

	virtual HRESULT Create(void const* pkInfo) = 0;

	virtual EUnitType UnitType() const = 0;
	virtual void Send(BM::Stream const &Packet, DWORD const dwSendFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE&*/);
	virtual short GetChannel() const {return 0;}
	virtual bool SetState(EUnitState const eState) { m_State = eState; return true;}
	virtual bool IsMapLoading() { return false; }

	virtual EUnitState GetState()const{return m_State;}
	virtual void Invalidate();

	virtual bool IsCheckZoneTime(DWORD dwElapsed) = 0;
	virtual void SendAbil(EAbilType const eAT, DWORD const dwSendFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE&*/);
	virtual void SendAbil64(EAbilType const eAT, DWORD const dwSendFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE&*/);
	virtual int GetAbil(WORD const Type) const;
	virtual bool SetAbil(WORD const Type, int const iInValue, bool const bIsSend = false, bool const bBroadcast = false);

	virtual __int64 GetAbil64(WORD const eAT)const;
	virtual bool SetAbil64(WORD const Type, __int64 const iInValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual float GetAbilFloat(WORD const Type) const;
	virtual bool SetAbilFloat(WORD const Type, float const fInValue, bool const bIsSend = false, bool const bBroadcast = false);

	virtual int GetCountAbil( WORD const Type, int const iFlag )const;
	virtual void AddCountAbil( WORD const Type, int const iFlag, bool const bAdd, DWORD const dwSendFlag );
	virtual void RemoveCountAbil( WORD const Type, int const iFlag, DWORD const dwSendFlag );

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);

	virtual void SendAbiles(WORD const* pkAbils, BYTE const byNum, DWORD const dwFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE*/);
	virtual void SendAbiles64(WORD const* pkAbils, BYTE const byNum, DWORD const dwFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE*/);

	inline bool IsUnitType(const EUnitType eType) const {return eType == (UnitType() & eType);}//Equal than
	inline bool IsInUnitType(const EUnitType eInType) const {return 0 != (UnitType() & eInType);}//Contain than
	
	int GetDelay()const{return m_iDelay;}
	void SetDelay(int const iDelay);

	int GetAttackDelay() const {return m_iAttackDelay;}
	void SetAttackDelay(int const iAttackDelay) {m_iAttackDelay = iAttackDelay;}

	BM::GUID const &GetTarget();
	void SetTarget(BM::GUID const &guidTarget);
	void AddTarget(BM::GUID const &guidTarget, bool const bFront = false );
	DequeTarget& GetTargetList();
	void DeleteTarget(BM::GUID const &rkGuid);
	void SetTargetList(const UNIT_PTR_ARRAY& rkArray);
	void ClearTargetList();
	void OnTargetDamaged(BM::GUID const &guidTarget);

	BM::GUID const &GetReserveTarget();
	void SetReserveTarget(BM::GUID const &guidTarget);	//Ÿ�� ����
	void SetReserveTargetToDeque(bool const bClearReserve = true);

	virtual int Enter(SActArg *pActArg){return 0;}
	virtual int Leave(SActArg *pActArg){return 0;}

	virtual void Init();
	virtual void CalculateInitAbil();
	virtual void SendWarnMessage(int const iMessageNo, BYTE const byLevel=EL_Warning){};
	virtual void SendWarnMessage2(int const iMessageNo, int const iValue, BYTE const byLevel=EL_Warning ) {};
	virtual void SendWarnMessage3(int const iMessageNo, BM::Stream const& kValue, BYTE const byLevel=EL_Warning) {};
	virtual void SendWarnMessageStr(BM::vstring const& rkStr, BYTE const byLevel=EL_Warning){};

	virtual void SetPos(POINT3 const &ptNew);
	virtual bool OnDamage( int& iPower, int& iHP );
	
	virtual int UpdateStandUpTime( unsigned long ulElapsedTime );
	virtual bool IsAlive()const;
	virtual bool IsDead()const;
	virtual int AutoHeal( unsigned long ulElapsedTime, float const fMultiplier = 1.0f);

	virtual BM::GUID const& GetGuildGuid() const	{ return BM::GUID::NullData(); }	
	virtual BM::GUID const& GetCoupleGuid() const	{ return BM::GUID::NullData(); }
	virtual BM::GUID const& GetPartyGuid() const	{ return BM::GUID::NullData(); }
	virtual BM::GUID const& GetExpeditionGuid()  const { return BM::GUID::NullData(); }
	virtual BM::GUID const& GetExpeditionParty() const { return BM::GUID::NullData(); }

	int CalculateAbil(WORD const wAbil);
	CSkill* GetSkill(){ return &m_Skill; }
	bool CheckTickTime(DWORD const dwTickGab, DWORD& dwElapsedTime);//AI �� �� �ִ��� üũ

	PgInventory* GetInven()							{ return &m_kInv; }
	PgInventory const* GetInven() const				{ return &m_kInv; }
	void SwapInven( PgInventory & rkInven )			{ m_kInv.Swap( rkInven );}
	
	int GetActionInstanceID();
	void SetActionInstanceID(int const iActionID);
	
	bool IsSummonUnit()const						{return m_bSummoned;}
	bool IsSummonUnit(BM::GUID const &rkGuid, bool const bCheckDieUnit=false);

	BM::GUID GetBlowAttacker()						{return m_guidBlowAttacker;}
	void SetBlowAttacker(BM::GUID guidAttacker);
	bool IsState(int iCheckState)const;
	//bool IsEnemy(CUnit* pkTarget, bool bCheckTargetValid = true, BYTE byTargetType = ESTARGET_ENEMY);
	virtual bool IsTarget(CUnit* pkTarget, bool bCheckTargetValid = true, int iSkillTargetType = ESTARGET_ENEMY, CSkillDef const* pkSkillDef = NULL, int iDmgCheckTime = -1);
	SUnit_AI_Info const * GetAI()const				{ return &m_kAI; }
	SUnit_AI_Info * GetAI()							{ return &m_kAI; }
	void SetAICurrentAction(EAIActionType eNewAction);
	void SetSync(bool const bSync)					{ m_bSendSyncPacket = bSync; }
	bool GetSync();
	void NftChangedAbil(WORD const wAbil, DWORD  dwSendType=E_SENDTYPE_NONE);
	bool IsAIType(const EAIType eAIType)const;
	int GetSummonUnitCount();
	virtual int GetMaxSummonUnitCount()						{ return MAX_SUMMONUNIT_COUNT; }
	bool AddSummonUnit(BM::GUID const &rkSummonUnitGuid, int const iClassNo=0, SummonOptionType const eOption=ESO_NONE, int const iMaxSummonUnitCount=MAX_SUMMONUNIT_COUNT);
	bool DeleteSummonUnit(BM::GUID const &rkSummonUnitGuid);
	void ClearSummonUnit();
	BM::GUID const &GetSummonUnit(int const iIndex) const;
	VEC_SUMMONUNIT const &GetSummonUnit()const				{ return m_kSummonUnit; }
	bool CheckSkillFilter(int const iSkillNo, SSFilter_Result* pkResult = NULL, ESkillFilterType eFilterType = ESFilter_None);// Skill�� ����� �� �ִ� �������� �˻��Ѵ�.

	POINT3BY const &GetOrientedVector();
	void SetOrientedVector(POINT3BY const &rkOrientedVec);
	static bool IsTargetUnitType(int eCaster, int eTarget, int iSkillTargetType);

	void SetDeliverDamageUnit(BM::GUID const &rkGuid);
	BM::GUID const &GetDeliverDamageUnit() const;
	bool CheckPassive(int iSkillNo, SActArg const* pkAct);
	bool AddClientProjectile(int const iInstanceID, SClientProjectile const & kProjectile);
	HRESULT CheckClientProjectile(int const iActionID, int const iInstanceID, BYTE byIndex, POINT3 const & ptTarget, int const iUnitSizeXY,
		bool const bCheckUsed = true) const;
	void ClientProjectileTimeout(DWORD const dwNowTime);

	int GetDiffEnchantLevel(CUnit* const pkDefenceUnit) const;

	void SetDamageAction(bool const bAction);
	bool IsDamageAction()const;
	int AutoGroggy(unsigned long ulElapsedTime, float const fMultiplier=1.0f);
	void UpdateGroggyRelaxTime(DWORD const dwElapsedTime);
	int GetGroggyRecoveryValue(float const fMultiplier=1.0f)const;

	int GetDetectRange()const;
	void SetTargetGroundNo(int const iGroundNo);

	virtual int GetMyClass()const;

public : //Effect Function
	//! ����Ʈ �߰��� �� �Լ��� ����ؼ� �߰����־�� �Ѵ�. lua->C�� �Ű��� �Լ�
	CEffect* AddEffect(int const iEffectNo, int const, SActArg const* pkAct, CUnit const* pkCaster, BYTE btEffectType = EFFECT_TYPE_NONE);
	//CEffect* AddEffect(BYTE byType, int const iEffectNo, int iValue, SActArg* pkAct, unsigned long ulElapsedTime = 0, unsigned long ulInterval = 0);
	CEffect* AddEffect(SEffectCreateInfo& rkCreate);
	bool	 DeleteEffect(int iEffectKey, bool bInGroup = false);
	bool	 DeleteBuffEffect(int iEffectKey, bool bInGroup = false);
	void	 OnDamageDelEffect();
	PgUnitEffectMgr& GetEffectMgr()					{ return m_kEffect; }
	PgUnitEffectMgr const& GetEffectMgr() const		{ return m_kEffect; }
	
	void DoEffect();
	void DoEquipEffect();// ���� ������ Effect �� ������� �ش�.
	void DoEquipEffectMonsterCard(EffectQueueType eType, int const iOrderIndex, int const iCardIndex);
	void DoEquipEffectSetItem();
	void EffectNftChangeAbil();

	virtual bool CanNfyAddEffect()const{return true;}// �߿�

	CEffect const *FindEffect(int iEffectNo)const;
	CEffect* GetEffect(int iEffectNo, bool bInGroup = false)const;
	CEffect* GetItemTypeEffect(int iEffectNo, bool bInGroup = false);
	bool	 IsItemEffect(int iEffectNo, bool bInGroup = false);

	void ClearAllEffect(bool const bDirectDelete = false, bool const bLogout = false, bool const bPenalty = false );
	void ClearCurseEffect(bool const bDirectDelete = false);

	//Tick���� Add/Delete �Ǿ�� �ϴ� Effect���� Tick�ȿ��� ó������ �ʰ� Tick�� ���� �Ŀ� ó���ǵ��� 
	void AddEffectQueue(EffectQueueData &rkQueueData);
	void EffectQueueUpdate();
	void SwapEffectQueue(CONT_EFFECT_QUEUE& rkOut);

	void GoalPos(POINT3 const &ptPos);

	bool IsNextGoalPos();
	POINT3 NextGoalPos();
	void NextGoalPos_Add(POINT3 const &ptPos);
	void NextGoalPos_Clear();
	
	void Caller(BM::GUID const &kCallerGuid){m_kCaller=kCallerGuid;}
	BM::GUID const &Caller()const;
    bool IsHaveCaller()const;

	virtual void InvenRefreshAbil();

public : // Random
	DWORD	GetRandom();		// �������� ���� �� ȣ���ϴ� �Լ�
	virtual int GetBattleLevel(void)const{return this->GetAbil(AT_LEVEL);}// ��ų���� ���������� �����Ҷ� ������ �ʿ��� ��쿡�� �ݵ�� �̰� ��� �ؾ� �Ѵ�.
	
	void	SetRandomSeed(DWORD dwSeed);	// Random Seed �� �ٽ� ����
	DWORD	GetRandomSeed() const;				// Random Seed �� ��� ��

protected : // Random
	RandomLib::Random	m_kRandom;	// �������� ���ö� ���
	DWORD				m_dwRandomSeed; //������ ���� SeedValue
	CLASS_DECLARATION_S(bool, UseRandomSeedType); //GetRandom���� ���� Seed/Count�� �������� �ʵ��� ���ִ� �Լ�
	CLASS_DECLARATION_S(DWORD, RandomSeedCallCounter);

protected:
	void SetOnDamageDelEffect(CEffect const* pkEffect);
	void DoSetItemEffect(CONT_HAVE_ITEM_DATA const& rkItemList, CONT_UNIQUE_INT& rkAddEffect, CONT_UNIQUE_INT& rkDelEffect);

public:		
	bool IsSummonUnitClass(int const iClassNo)const;
protected: 
	virtual bool OnSetAbil(WORD const Type, int const iValue);		// SetAbil(..) ���Ŀ� ȣ��Ǵ� �Լ�
	virtual void VOnDie();
	virtual void VOnLvUp();
	virtual void VOnChangeClass();
	virtual void VOnRefreshAbil();
	virtual void VOnChangeTactics();

	virtual bool CheckSetAbil(WORD const Type, int& iValue);	// SetAbil(...) ���� ȣ��Ǵ� �Լ�
	virtual bool CanbeTarget(int const iTargetType);

	bool SendCaller( BM::Stream const &kPacket, DWORD const dwSendFlag );
	bool SendObserver( BM::Stream const &kPacket );


	bool FindTarget(BM::GUID const &rkGuid);
	void AdjustTargetList();

	int GetGroggyRecovery()const;

protected:
	mutable PgInventory m_kInv;	// Inven / Equips
	mutable CSkill m_Skill;		// Skill
	DequeTarget m_kTargetDeque;

	DWORD m_dwElapsedTimeCheckZone;	// ��ü�� Zone�� ���� ������ Ȯ���ϱ� ���� Time
	DWORD m_dwHPElapsedTime;
	DWORD m_dwMPElapsedTime;
	DWORD m_dwGroggyElapsedTime;
	UNIT_COMMON_ABIL m_kCommon;
	SPlayerBasicInfo m_kBasic;

	EUnitState m_State;
	int m_iDelay;
	int m_iAttackDelay;

	CLASS_DECLARATION_S(std::wstring, Name)	
	int m_iActionInstanceID;
	POINT3BY m_kOrientedVector;		// ���� �ִ� ���� ����(Normal�� ���� ���� �ƴϸ�, �������� ��)
	BM::GUID m_guidBlowAttacker;
	bool m_bDamageAction;
	//CLASS_DECLARATION_REF(SProjectileInfo, m_kProjectile, Projectile);	// � AI�� �߻�ü�� ���� �����ϱ⵵ �Ѵ�(�߻�ü�� ���� ����)
protected:
	SProjectileInfo m_kProjectile[MAX_PROJECTILE_COUNT];
public:
	SProjectileInfo& Projectile(int iNum) { if(0 > iNum || MAX_PROJECTILE_COUNT <= iNum ) return m_kProjectile[0]; return m_kProjectile[iNum]; }

	CLASS_DECLARATION_S(Direction, FrontDirection); //���� �ٶ󺸴� ����
	CLASS_DECLARATION_S(Direction, MoveDir); //���� �̵��ϴ� ����
	CLASS_DECLARATION_S(DWORD, LastTickTime);
	CLASS_DECLARATION_S(int, TickNum);
	CLASS_DECLARATION_S(DWORD, DeathTime);
	CLASS_DECLARATION_S(POINT3, StartPos);			// ���������� ���� ���� ��ġ(AI�� �ʿ��� �Ѹ� ����ϰ� �� ���̴�.)
	CLASS_DECLARATION_S_NO_SET(POINT3, GoalPos);	// ���������� ���� ���� ��ġ(AI�� �ʿ��� �Ѹ� ����ϰ� �� ���̴�.)
    CLASS_DECLARATION_S(POINT3, RoadPos);			// �����ؾ��� ���� ���� ��ġ(AI�� RoadPoint�� ����ϴ� �Ѹ� ����ϰ� �� ���̴�.)
	CLASS_DECLARATION_PTR(PgAggroMeter *, m_pkAggroMeter, AggroMeter);	//���� ��/�� ���
	bool GetTargetFromAggro(BM::GUID &rkGuid);
	CLASS_DECLARATION_S(int, StandUpTime);	//�Ѿ����ٰ� �Ͼ���� �ð�
	DEQUE_POS m_kContNextGoalPos;			//���������� �����ؾ� �� ��� �̿�
	
	// Last SmallArea's Index -> PgGround���� ����ؾ� �Ѵ�.
	CLASS_DECLARATION_S(int, LastAreaIndex );
	CLASS_DECLARATION_S(short, WeaponDecCount);//������ ������ ����ϴ� ī��Ʈ.
	CLASS_DECLARATION_S(short, ArmorDecCount);//������ ������ ����ϴ� ī��Ʈ.
	CLASS_DECLARATION_S(int ,ActionID);	// �ֱ��� ActionID
	CLASS_DECLARATION_S(POINT3, RelativeGoalPos);	// GoalPos�� ��� ��ǥ.
	CLASS_DECLARATION_S(POINT3BY, PathNormal);		// Path Normal
	CLASS_DECLARATION_S(POINT3, LastPos);			// ������ ��ǥ

	BYTE GetSyncType()const{return m_bySyncType;}
	virtual void SetSyncType( BYTE const bySyncType, bool const bOnlyChangeState=false );

	SUnit_AI_Info m_kAI;
	
	POINT3 GetDirectionVector(BYTE byDirection);

	static WORD GetBasicElementDmgRateAbil(WORD const wAbil);
	static WORD GetBasicElementAbilTotal(WORD const wAbil);
	static WORD GetBasicElementRateAbilTotal(WORD const wAbil);
	static WORD GetConvertElementRateAbilTotal(WORD const wAbil);
	static bool Is4ElementAbil(WORD const wAbil);
	int GetBasicElementDmg(WORD const wAbil, int const iAddValue, 
		EGetAbilType::Enum const getabiltype) const;
	int GetConvertElementalDmgRate(WORD const wAbil, int const iAddValue, 
		EGetAbilType::Enum const getabiltype) const;

public:
	void GetObserverMemberList( CONT_GUID_LIST &rkContGuidList )const;
	void GetObserverMemberList(CONT_OBSERVER_SWITH_MEMBER_LIST& rkContOut) const;

protected:
	void SwapObserverList( PgNetModule<>::CONT_NETMODULE &kContObserver ){m_kContObserver.swap(kContObserver);}
	void AddObserver( PgNetModule<> const &kNetModule ){m_kContObserver.insert(kNetModule);}
	void RemoveObserver( PgNetModule<> const &kNetModule ){m_kContObserver.erase(kNetModule);}
	bool IsHaveObserver()const{return 0 < m_kContObserver.size();}

protected:
	bool m_bSendSyncPacket;

	// ��ȯ
	bool m_bSummoned; // ��ȯ�� Unit�ΰ�?(���Ͱ� �θ��� ���ᳪ ����, ĳ���Ͱ� ��ȯ�ϴ� ��ȯ�����)
	VEC_SUMMONUNIT	m_kSummonUnit;	// Entity �� �ڽ��� ��ȯ�� Unit ����Ʈ
	VEC_DIE_SUMMONUNIT	m_kDieSummonUnit;	// ���� Entity �� �ڽ��� ��ȯ�� Unit ����Ʈ
	BM::GUID		m_kCaller;
	BM::GUID		m_kDeliverDamageUnit; // �������� ��� �Դ� ������ GUID	
	BM::GUID		m_kReserveTarget;

	PgUnitEffectMgr m_kEffect;
	CONT_EFFECT_QUEUE m_kContEffectQueue;
	VEC_INT			m_kContOnDamageDelEffect;

	BYTE			m_bySyncType;
	typedef std::map<int, SClientProjectile> CONT_CLIENT_PROJECTILE;	// <iActionInstanceID, SClientProjectile>
	CONT_CLIENT_PROJECTILE m_kClientProjectile;

	PgNetModule<>::CONT_NETMODULE	m_kContObserver;
public:
	virtual void SetMutatorAbil(SMissionMutatorAbil const& pkMutationAbil);
};

typedef std::vector<CUnit*>	VEC_UNIT;

extern void CheckSkillFilter_Delete_Effect(CUnit const* pkCaster, CUnit* pkTarget, int iSkillNo);

inline CONT_DEFMONSTERTUNNINGABIL::mapped_type const * GetContTunningAbilDef(int const iNo, int const iGrade, int const iLv);

#endif // WEAPON_VARIANT_UNIT_UNIT_H