#ifndef MAP_MAPSERVER_MAP_GROUND_PGGROUND_H
#define MAP_MAPSERVER_MAP_GROUND_PGGROUND_H

#include "BM/Observer.h"
#include "Lohengrin/GMCommand.h"
#include "Variant/Global.h"
#include "variant/gm_const.h"
#include "Variant/PgAggroMeter.h"
#include "Variant/PgAggroMeterMgr.h"
#include "Variant/PgComboCounter.h"
#include "Variant/PgComboCounterMgr.h"
#include "Variant/PgEventQuestBase.h"
#include "Variant/AlramMissionChecker.h"
#include "Collins/Log.h"
#include "constant.h"
#include "AI/PgAIManager.h"
#include "PgSmallArea.h"
#include "PgGroundResource.h"
#include "PgReqMapMove.h"
#include "PgGenPointMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgLocalVendorMgr.h"
#include "PgUserQuestSessionMgr.h"
#include "PgMarryMgr.h"
#include "PgWorldEnvironmentStatus.h"
#include "Variant/PgHiddenRewordItemMgr.h"
#include "PgEventItemSet.h"
#include "PgDefencePartyMgr.h"
#include "PgJobSkillMgr.h"
#include "Variant/PgJobSkillLocationItem.h"
#include "PgDuelSystem.h"
#include "Variant/PgConstellation.h"
#include <tuple.h>

typedef enum eFAKE_REMOVE_TYPE
{
	FRT_NONE = 0,
	FRT_ONLY_HIDE,
	FRT_HIDE_AND_NO_MSG,
	FRT_HIDE_AND_NO_MSG_BUT_MAPMOVE,
}EFAKE_REMOVE_TYPE;

typedef enum eCREATE_HP_TYPE
{
	ECHT_NONE				= 0,
	ECHT_CALLER_HP_RATE		= 1,
	ECHT_ELGA_BONE_MON		= 2,
	ECHT_ELGA_EYE_MON		= 3,
} ECREATE_HP_TYPE;

typedef enum eINSTANCE_ITEM_GEN_GROUP
{
	EIGG_ANY		= 10000,
	EIGG_RED		= 10001,
	EIGG_BULE		= 10002,
	EIGG_END		= 19999,
} EINSTANCEITEM_GEN_GROUP;

namespace PgGroundUtil
{
	inline bool IsVillageGround(T_GNDATTR const& rAttr);
	bool IsCanTalkableRange(CUnit* pkPC, BM::GUID const& rkNpcGuid, CUnit* pkNPC, int const iCurGround, ENpcMenuType const eMenuType, TCHAR const* szFucn, size_t const iLine);
	bool IsCanTalkableRange(CUnit* pkPC, POINT3 const& rkPos, BM::GUID const& rkNpcGuid, CUnit* pkNPC, int const iCurGround, ENpcMenuType const eMenuType, TCHAR const* szFunc, size_t const iLine);

	bool IsNeedNpcTestChangeItemEvent(SItemPos const& rkPos);
	bool IsCallSafe(CUnit * pkUnit,SItemPos const & kSourcePos,SItemPos const & kTargetPos);
	bool IsCanTalkableRange(CUnit* pkPC, BM::GUID const& rkNpcGuid, CUnit* pkNPC, int const iCurGround, ENpcMenuType const eMenuType, TCHAR const* szFunc, size_t const iLine);
	bool IsMyHomeHasFuctionItem(CUnit * pkPlayerUnit,CUnit * pkNpcUnit,EUseItemCustomType const kType);

	enum ESpecialUnitStatusType
	{
		SUST_None					= 0,
		SUST_CashShop				= 1,
		SUST_OpenMarket				= 2,
		SUST_PvPModeSelectting		= 3,
		SUST_HardCoreDungeonVote	= 4,
		SUST_Vendor					= 5,
	};
	typedef std::map< BM::GUID, ESpecialUnitStatusType > ContUnitSpecialStatus;
	typedef std::map<int, size_t> ContAbilValue;

	
	class PgUnitSepcailStatusHandler // ���°��� �縳 ���Ѵ�. (�縳 �Ϸ��� �����̳ʷ� second�� ������ ���� �Ͻÿ�)
	{
	protected:
		PgUnitSepcailStatusHandler();
		virtual ~PgUnitSepcailStatusHandler();

		ESpecialUnitStatusType EnterSpecStatus(BM::GUID const& rkGuid, ESpecialUnitStatusType const eEnterType, bool const bChange = false );
		bool ChangeSpecStatus( BM::GUID const &rkGuid, ESpecialUnitStatusType const eCheckType, ESpecialUnitStatusType const eChangeType );
		bool LeaveSpecStatus(BM::GUID const& rkGuid, ESpecialUnitStatusType const eLeaveType);
		ESpecialUnitStatusType GetSpecStatus(BM::GUID const& rkGuid) const;
		void ClearSpecStatus(BM::GUID const& rkGuid);

	private:
		mutable Loki::Mutex m_kUnitSpecialStatusMutex;
		ContUnitSpecialStatus m_kContSpecStatus;
	};

	void DeleteInvenCoupleItem(CUnit* pkUnit, WORD const wItemHaveAbilType, CItemDefMgr const& rkItemDefMgr, SGroundKey const& rkGndKey);

	bool IsMyHomeHasFuctionItem(CUnit * pkPlayerUnit,CUnit * pkNpcUnit,EUseItemCustomType);
	bool IsMyHomeHasSideJob(CUnit * pkPlayerUnit,CUnit * pkNpcUnit,eMyHomeSideJob const kSideJob);
	bool IsSaveHPMP(EGndAttr const eGndAttr);

	template<typename T_CONT>
	void InitGenPointChild(T_CONT& rkCont)
	{
		T_CONT::iterator itor_cont = rkCont.begin();
		while (rkCont.end() != itor_cont)
		{
			T_CONT::mapped_type& rkGenInfo = (*itor_cont).second;
			rkGenInfo.RemoveAll();

			++itor_cont;
		}
	}

	SummonOptionType GetSummonOptionType(bool const bUniqueClass);

	void SafeImportAlwaysDropItemCont(ContAbilValue& rkOut, int iItemNo, size_t iItemCount = 1);
	void GetAlwaysDropItem(CUnit& rkMonster, ContAbilValue &rkOut);
	bool InsertAlwaysDropItem(CUnit * pkUnit, int const iItemNo, size_t iItemCount = 1);
	bool InsertEffectAbil(CUnit * pkUnit, int const iEffectNo);
	void DeleteEffect(CUnit* pkUnit, WORD const wEffetHaveAbilType);
	void DeleteInvenItem(CUnit* pkUnit, EInvType const eFromInvType, WORD const wItemHaveAbilType, CItemDefMgr const& rkItemDefMgr, SGroundKey const& rkGndKey);
	void DeleteInvenCoupleItem(CUnit* pkUnit, WORD const wItemHaveAbilType, CItemDefMgr const& rkItemDefMgr, SGroundKey const& rkGndKey);

	void SendAddSimpleUnit(CUnit * pkUnit, UNIT_PTR_ARRAY const& kUnitArray, DWORD const dwSendFlag);
	void SendPartyMgr_Refuse(BM::GUID const& rkCharGuid, EPartyRefuse const eType, bool const bRefuse);

	void SetActArgGround(SActArg & kArg, PgGround const* pkGround);
};

class PgPlayer;
class PgActionResultVector;
class PgLocalPartyMgr;
class PgLocalMarketMgr;

//#define MAX_SEND_BROADCASTING_DISTANCE 800
//#define MAX_SEND_BC_AREA_DISTANCE	MAX_SEND_BROADCASTING_DISTANCE * 1.5

int const MONSTER_AI_TICK_INTER = 300;	//���� AIƽ ����
int const MONSTER_AI_TICK_INTER_ERROR = 30;	//���� ƽ�� ������. ������ 1/10

int const MISSION_LEVEL5 = 4;
int const MISSION_LEVEL6 = 5;

#define AI_MONSTER_MAX_MOVETIME 10000	// Monster maximum move time
#define AI_MONSTER_MIN_MOVETIME	1500	// Monster minimum move time

enum eIndunState
{
	INDUN_STATE_NONE		= 0x00,
	INDUN_STATE_OPEN		= 0x01,	// �δ��� ���µǾ���.
	INDUN_STATE_WAIT		= 0x02,	// �δ��� ������ ��ٸ��� ���̴�.
	INDUN_STATE_READY		= 0x04,	// �δ��� �غ� �Ǿ���.
	INDUN_STATE_PLAY		= 0x08,	// �δ��� �������� ��� �ִ� ���̴�.
	INDUN_STATE_RESULT_WAIT	= 0x10,	// �δ��� �÷��� ����� �����־�� �Ѵ�.
	INDUN_STATE_RESULT		= 0x20,	//
	INDUN_STATE_CLOSE		= 0x40,	// �δ��� ������ �Ѵ�.
	INDUN_STATE_FAIL		= 0x80, // �δ� ���� ����. ���â�� �н��ϰ� PLAY -> FAIL -> CLOSE ������ ���¸� �����Ѵ�.
	INDUN_STATE_REGIST_RESULT_MGR = INDUN_STATE_OPEN | INDUN_STATE_WAIT | INDUN_STATE_READY | INDUN_STATE_PLAY,
};
typedef DWORD	EIndunState;

// �����׿���
typedef enum eOpeningState
{
	E_OPENING_NONE			= S_OK,
	E_OPENING_READY			= S_FALSE,//�δ� Ready���¿��� �������� �ؾ���
	E_OPENING_PLAY			= E_OPENING_READY+1,
	E_OPENING_ERROR			= E_FAIL,
}EOpeningState;

typedef enum
{
	E_SPAWN_DEFAULT			= 0,
	E_SPAWN_RED				= TEAM_RED,
	E_SPAWN_BLUE			= TEAM_BLUE,
	E_SPAWN_WIN				= 3,
	E_SPAWN_LOSE			= 4,
	E_SPAWN_BATTLEAREA_ATK	= 5,
	E_SPAWN_BATTLEAREA_DEF	= 6,
	E_SPAWN_DEFENCE7_RED	= 7,
	E_SPAWN_DEFENCE7_BLUE	= 8,
	E_SPAWN_EVENT_GROUND	= 9,
}ESpawnType;

// �̺�Ʈ �׶��� ����.
// ���¿� ���� �ٸ� ���� �����ϱ� ����( �δ�ó�� ).
typedef enum eEventGroundState
{
	EGS_NONE			= 0x00,
	EGS_IDLE			= 0x01,		// �̺�Ʈ ���� ���� �ƴ�( ������ ���� �̺�Ʈ�� ������ �� �ִ��� �ð��� �˻��ϴ� ����)
	EGS_START_WAIT		= 0x02,		// ���� ���� ���.
	EGS_READY			= 0x04,		// �̺�Ʈ ���� ��. �������� �̺�Ʈ ������ �̵��ϴ� ��.
	EGS_PLAY			= 0x08,		// �̺�Ʈ�� ���� ��. ���� ���� ������ ���� ���Ͱ� ���� �ȴ�.
	EGS_WAIT_SHORTTIME	= 0x10,		// ��� ����ϴ� ����.
	EGS_END_WAIT		= 0x20,		// �̺�Ʈ�� ����(�ð��� �ٵǰų� ������ ���� ���Ͱ� ���). ��� ��� �� ���� ���·� ����.
	EGS_END				= 0x40,		// �̺�Ʈ�� ����Ǿ� �ʿ� ���� �ִ� ��� ������ ���� ������ ��������.
}EEventGroundState;

// ���� �̺�Ʈ �ʿ� ������ ���� ���� �����ϴ� �����̳�.
// key : EventNo, value : user count
typedef std::map<int, int> CONT_CURRENT_EVENT_MAP_USER_COUNT_LIST;

// ���� �̺�Ʈ �ʿ� ������ �� �ִ��� �����ϴ� �����̳�.
// key : EventNo, value : �̺�Ʈ�ʿ� ������ �� �ִ���?
typedef std::map<int, bool> CONT_EVENT_MAP_ENTERABLE_ENTER_LIST;

typedef struct tagJoinPartyMember
{
	BM::GUID CharGuid;
	BM::GUID PartyGuid;
	bool bMaster;

	tagJoinPartyMember()
	{
		CharGuid.Clear();
		PartyGuid.Clear();
		bMaster = false;
	}

}SJoinPartyMember;
typedef std::map<BM::GUID, SJoinPartyMember> CONT_JOIN_PARTY_MEMBER_LIST;

#pragma pack(1)

/*
class CCheckSmallArea
	: public TVisitor< PgSmallArea* >
{
public:
	CCheckSmallArea(POINT3 const &rkPos)
		: m_kPoint(rkPos)
	{
	}

	virtual bool VIsCorrect(PgSmallArea* &pArea)
	{
		return pArea->IsAdjacentArea(m_kPoint);
	}
	POINT3 const m_kPoint;
};
*/

typedef struct tagSMonsterGen
{
	tagSMonsterGen()
	{
		iMonNo = 0;
		i64GenTime = 0;
	}

	tagSMonsterGen(int const _iMonNo,__int64 const i64CurTime)
	{
		iMonNo = _iMonNo;
		i64GenTime = i64CurTime;
	}

	tagSMonsterGen(tagSMonsterGen const& rkInfo)
	{
		iMonNo = rkInfo.iMonNo;
		i64GenTime = rkInfo.i64GenTime;
	}

	int		iMonNo;
	__int64	i64GenTime;
} SMonsterGen;

typedef struct tagEventMonsterGen
{
	tagEventMonsterGen()
	{
		MonNo = 0;
		GenTime = 0;
		DelTime = 0;
		DelayTime = 0;
		RegenPeriod = 0;
		GenPos = POINT3(0, 0, 0);
		RewardItemGroupNo = 0;
		RewardCount = 0;
		GenerateMessage.clear();
		IsInsertMonster = false;
	}

	tagEventMonsterGen(tagEventMonsterGen const & Info)
	{
		MonNo = Info.MonNo;
		GenTime = Info.GenTime;
		DelTime = Info.DelTime;
		DelayTime = Info.DelayTime;
		RegenPeriod = Info.RegenPeriod;
		GenPos = Info.GenPos;
		RewardItemGroupNo = Info.RewardItemGroupNo;
		RewardCount = Info.RewardCount;
		GenerateMessage = Info.GenerateMessage;
		IsInsertMonster = Info.IsInsertMonster;
	}

	int MonNo;
	__int64 GenTime;
	__int64 DelTime;
	int DelayTime;
	int RegenPeriod;
	POINT3 GenPos;
	int RewardItemGroupNo;
	int RewardCount;
	std::wstring GenerateMessage;
	bool IsInsertMonster;
} SEventMonsterGen;

typedef struct tagDeleteReserveInsItemInfo
{
	tagDeleteReserveInsItemInfo()
	{
		GenInfo.Clear();
		DelItemGuid.Clear();
	}

	tagDeleteReserveInsItemInfo( tagDeleteReserveInsItemInfo const & rhs )
	{
		GenInfo = rhs.GenInfo;
		DelItemGuid = rhs.DelItemGuid;
	}

	TBL_DEF_MAP_REGEN_POINT GenInfo;
	BM::GUID DelItemGuid;

} SDeleteReserveInsItemInfo;

typedef struct tagEventDropAnyMonster
{
	tagEventDropAnyMonster()
	{
		StartTime.Clear();
		EndTime.Clear();
		RewardItemGroupNo = 0;
		RewardCount = 0;
		bIsEventTime = false;
	}

	tagEventDropAnyMonster(tagEventDropAnyMonster const & param)
	{
		StartTime = param.StartTime;
		EndTime = param.EndTime;
		RewardItemGroupNo = param.RewardItemGroupNo;
		RewardCount = param.RewardCount;
		bIsEventTime = param.bIsEventTime;
	}

	BM::DBTIMESTAMP_EX StartTime;
	BM::DBTIMESTAMP_EX EndTime;
	int RewardItemGroupNo;
	int RewardCount;
	bool bIsEventTime;
}SEventDropAnyMonster;

#pragma pack()

typedef std::map<int,SMonsterGen>	CONT_MONSTER_GEN;
typedef std::set<int>		CONT_GENERATED_MONSTER;

typedef std::vector<SEventMonsterGen> VEC_EVENTMONSTERGEN;
typedef std::map<int, SEventMonsterGen> CONT_EVENT_MONSTER_GEN;

typedef std::vector<SEventDropAnyMonster> VEC_EVENTDROPANYMONSTER;

typedef std::map<BM::GUID, SDeleteReserveInsItemInfo> CONT_DELETERESERVERINSITEM;
typedef std::map<BM::GUID, BM::GUID> CONT_PARTY_MASTER;	//first:PartyGuid, second:MasterGuid

typedef std::tuple<int, int, int, int> SAddRange;

class PgGroundManager;
class PgUserQuestSessionMgr;
//Unit�� SmallArea �� ���� �𸥴�. ���� �׶��尡 �����ϱ� ���� �Ϸ��� �ϴ°��̴�. Objserver�ε� ��� ����
class PgGround
	:	public BM::CObserver< BM::Stream* >
	,	public PgGroundResource
	,	public PgAggroMeterMgr
	,	public PgComboCounterMgr
	,	protected PgObjectMgr2
	,	protected PgGroundUtil::PgUnitSepcailStatusHandler
{
	// �����
public:
	typedef std::vector<CUnit const*>	ConUnit;

	//typedef std::map< POINT3, PgSmallArea* > ContArea;
	typedef std::vector<PgSmallArea*> ContArea;

	typedef std::map< BM::GUID, SPartyUserInfo > ContPartyMember;
	typedef std::vector<SPartyUserInfo> VEC_UserInfo;
	typedef std::map< int, int > ContClassLevel;
	typedef std::map<int ,int> MutatorBoostAbil;

public :
	static AntiHackCheckActionCount ms_kAntiHackCheckActionCount;
	static AntiHackCheckVariable ms_kAntiHackCheckVariable;
	static void SetAntiHackVariable();

	static float		ms_fAutoHealMultiplier_NotInViliage;

	static int			ms_iEnableEntityCritialAttack;	// Entity ������ Player�� Critial Ȯ���� ���ݷ� ����
	static int			ms_iEnableEntityHitRate;		// Entity ������ Player�� ���߷� ����
	static int			ms_iEnableEntityAttackExtInfo;	// Entity ������ Player�� �߰� �������� ����

	static int			ms_iHiddenRewordItemNo;
	static T_GNDATTR	ms_kOffPet_GroundAttr;			// ���� ������ �ʴ� �׶��� �Ӽ�

	static float		ms_KnightAwakeReviserByKill;	//���͸� �׿����� ���� ����ġ ������
	static float		ms_WarriorAwakeReviserByKill;
	static float		ms_MageAwakeReviserByKill;
	static float		ms_BattleMageAwakeReviserByKill;
	static float		ms_HunterAwakeReviserByKill;
	static float		ms_RangerAwakeReviserByKill;
	static float		ms_ClownAwakeReviserByKill;
	static float		ms_AssassinAwakeReviserByKill;
	static float		ms_ShamanAwakeReviserByKill;
	static float		ms_DoubleFighterAwakeReviserByKill;

	static int			ms_JobSkillBaseExpDiff[4]; // 1~4

public:
	PgGround();
	virtual ~PgGround();

public: //AI ���� public 
	void AI_CallHelp(CUnit* pkUnit, float fCallRange);
	bool AI_SkillFire( CUnit* pkCaster, SActArg* pkActArg, const ESkillStatus eUnitState = ESS_FIRE);
	bool AI_SkillCasting( CUnit* pkCaster, SActArg* pkActArg, BM::Stream& rkTailPacket);
	bool AI_GetSkillTargetList(CUnit* pkCaster, int const iSkillNo, UNIT_PTR_ARRAY& rkTargetList, bool bDamageFire, SActArg const* pkActArg, POINT3 const* pkDir = NULL, SAddRange const& rkAddRange = SAddRange(0,0,0,0));
	bool FindEnemy(CUnit* pkFrom, int const iMaxTarget = 5);
	bool CanAttackTarget(CUnit* pkAttacker);
	void SendNfyAIAction(CUnit* pkUnit, EUnitState eState = US_IDLE, PgActionResultVector* pkActionResultVec=NULL, BM::Stream* pkTailPacket = NULL, int const iSkillNo = 0);
	bool IsTargetInRange(CUnit* pkUnit, int const iZLimit = AI_Z_LIMIT, bool const bCheckFromGround = false);
	int GetReserveAISkill(CUnit* pkUnit);
	POINT3 GetUnitPos( BM::GUID const & rkGuid, bool bCheckImpact = false, float const fRayRange = 300.0f );
	void SendNftChaseEnemy(CUnit* pkUnit);
	float GetAdditionalAttackProb(CUnit *pkUnit);	
	bool GetAroundVector(CUnit *pkUnit, BM::GUID const &rkTargetGuid, float const fRange, bool bNear, POINT3 &rkOut);
	POINT3BY GetPathNormal(BM::GUID const & rkGuid);
	bool GetTargetPos(CUnit* pkUnit, POINT3& rkTargetPos);

protected:

	void ProcessTimeOutedItem(PgPlayer * const pkPlayer);
	void ProcessTimeOutedMonsterCard(PgPlayer * const pkPlayer);
	void ProcessUnbindItem(PgPlayer * const pkPlayer);
	void ProcessCheckMacroUse(PgPlayer * const pkPlayer);
	void ProcessRecommendPointRefresh(PgPlayer * const pkPlayer);

//////////////////////////

	void OnReqItemAction( PgPlayer *pkPlayer, BM::Stream * pkNfy, PgPet * pkPet=NULL);

	void CheckPetAliveAchievement(PgPlayer * pkPlayer);

    virtual int GetMaxSummonUnitCount(CUnit * pkUnit)const;
	virtual void SendMonsterCount();


	bool	m_bUpdateMonsterCount;	//���� ī��Ʈ�� �����Ǿ����� ����

public:
	virtual EOpeningState Init( int const iMonsterControlID=0, bool const bMonsterGen=true );
	virtual void Clear();
	virtual bool Clone( PgGround* pkGround );

	// Ground Manager�� OnTimer���� ȣ��(���ϰ��� true�̸� �׶��尡 �����ȴ�.)
	virtual bool IsDeleteTime()const{return false;}
	virtual void OnTick100ms();
	virtual void OnTick1s();
	virtual void OnTick5s();
	virtual void OnTick30s();
	virtual void OnTick1m();
	inline bool CheckTickAvailable(ETickInterval const eInterval, DWORD const dwNow, DWORD& rdwElapsedTime, bool bUpdateLastTime = false);

	virtual EGroundKind GetKind()const{	return GKIND_DEFAULT;	}
	virtual EIndunState GetState()const{return INDUN_STATE_NONE;}
	virtual int GetGroundNo()const { return GroundKey().GroundNo();}; //���� �׶��� ��ȣ
	virtual int GetGroundItemRarityNo()const { return GIOT_FIELD; }
	virtual void CheckEnchantControl(PgBase_Item& rkItem)const {};

	virtual int GetMapItemBagGroundNo() const { return GetGroundNo(); };

	virtual void GetGenGroupKey(SGenGroupKey& rkkGenGrpKey)const;
	virtual SRecentInfo const& GetRecentInfo()const;

protected:
	DWORD GetTimeStamp();

	//	Zone
	HRESULT BuildZone();
#ifdef ZONE_OPTIMIZE_20090624
	HRESULT MakeZone(NiNode *pkRootNode);
#endif
	void ReleaseZone();

	bool RemoveGenGroupMonster(int const iGenGroupNo, bool const bKill);	//! gen group ���͸� �����Ѵ�. 
	bool RemoveGenGroupObject(int const iGenGroupNo);						//! �� �׷��� ������Ʈ�� �����Ѵ�. 

	HRESULT GetArea(POINT3 const &rkPos, POINT3 &rkOut, PgSmallArea* &pRetArea, bool const bIsMoveToSpawn = true, bool bCheckPos = true);
	PgSmallArea* GetArea( int const iAreaIndex )const;

	void ProcessMonsterGenNfy(int const iGenGroupNo,E_MONSTER_GEN_MODE const kMode);
	float GetAutoHealMultiplier()const;
	void OnActivateEventMonsterGroup();
	bool CheckPathNormal(CUnit* pkUnit, CSkillDef const* pkSkillDef, SActionInfo const& rkAction, float const fErrorDelta = 10.0f);//Ŭ�󿡼� ������ �н��븻�� ���ݹ��� �˻�. �����Ѱ� 10��
//	+-------------------------------------------------------+
//	|	Unit Control Method									|
//	+-------------------------------------------------------+
public:
	void OnRecvMonsterGenNfy(int const iGenGroupNo,E_MONSTER_GEN_MODE const kMode);
	size_t GetUnitCount( const eUnitType eType )const;
	bool UnitEventNfy(BM::GUID const &rkGuid, BM::Stream* pkPacket);

	CUnit* GetUnit( BM::GUID const &rkGuid )const;
	PgPlayer* GetUser( BM::GUID const &rkCharGuid ) const;
	PgPet* GetPet( PgPlayer * pkPlayer )const;
	PgPet* GetPet( BM::GUID const &kPetID )const;
	
	PgSubPlayer* GetSubPlayer( PgPlayer * pkPlayer )const;
	PgSubPlayer* GetSubPlayer( BM::GUID const &rkSubPlayerGuid )const;

	bool CheckUnit( BM::GUID const &rkGuid )const;//Unit�� �ִ°�?
	BM::GUID const & GetFollowingHead(CUnit* pktail, bool const bGotoEnd, short int sCallDepth = 0);

	int GetUnitAbil(BM::GUID const & rkGuid, WORD const wType);

	virtual HRESULT InsertMonster(TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, int const iMonNo, BM::GUID &rkOutGuid, CUnit* pkCaller=NULL, bool bDropAllItem = false, int iEnchantGradeNo = 0, ECREATE_HP_TYPE const eCreate_HP_Type = ECHT_NONE);

	void DeletePet( BM::GUID const &kPetID );
	PgPet* CreatePet( PgPlayer * pkCaller, BM::GUID const &kPetID, CONT_PET_MAPMOVE_DATA::mapped_type &kPetData );
	CUnit* CreateEntity(CUnit* pkCaller, SCreateEntity* pkCreateInfo, LPCTSTR lpszName, bool bSyncUnit = true);
	int GetEntity( BM::GUID const &kCaller, int const iClass, UNIT_PTR_ARRAY& rkUnitArray);
	CUnit* CreateGuardianEntity(CUnit* pkCaller, SCreateEntity* pkCreateInfo, LPCTSTR lpszName, bool bSyncUnit = true, int const iStartActionID=0);	
	CUnit* CreateSummoned(CUnit* pCaller, SCreateSummoned* pCreateInfo, LPCTSTR lpszName, SSummonedMapMoveData const& kMapMoveData, POINT3 kCreatePos = POINT3::NullData());
	CUnit* CreateCustomUnit(int iClassNo, POINT3 const& ptPos, Direction const& rkDirFront);
	void RefreshSummonEffect(PgPlayer* pkPlayer) const;
	PgSubPlayer* CreateSubPlayer( PgPlayer * pkCaller, BM::GUID const& rkSubPlayerID );
	void DeleteSubPlayer( BM::GUID const& rkSubPlayerID );

	int GetTotalSummonedSupply(CUnit* pkCaller)const;

	virtual bool ReleaseUnit( CUnit *pkUnit, bool bRecursiveCall=false, bool const bSendArea=true );
	void OnPT_T_C_NFY_MARRY(EMarryState const eCommandType, BM::Stream *const pkPacket);

protected:
	bool FakeRemoveUnit( CUnit * pkCaller, int const iHideMode );
	bool FakeAddUnit(CUnit * pkCaller);
	
	void SetUnitAbil( BM::GUID const & kCharGuid, WORD const Type, int const iValue, bool const blsSend = false, bool const bBroadcast = false );
	
	//void AddNPC(char const* pcName, char const* pcActor, char const* pcScript, char const* pcLocation, BM::GUID const &rkGuid, int iID);
	
	POINT3BY GetMoveDirection(BM::GUID const & rkGuid);

	void AutoHealAndCheckDieSkill(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier = 1.0f);

	//! Function used for hydra mutator
	void MakeHydraUnit(CUnit * pkUnit);

public:
	void LogOut( SERVER_IDENTITY const &kSI );
	virtual bool LogOut(BM::GUID const &rkCharGuid);

public:
	PgMonster* GetFirstMonster(int const iMonID);
	PgBoss* GetBossMonster(int const iMonID);

protected:
	CUnit* GetUnitControl(CUnit* pkUnit, CUnit* pkCalledUnit, unsigned short usPacketType, BM::Stream& rkPacket);

	virtual bool SaveUnit( CUnit *pkUnit, SReqSwitchReserveMember const *pRSRM = NULL, BM::Stream * const pPacket = NULL );
	bool AddUnit( CUnit *pkUnit, bool const bIsSendAreaData = true );	//��ӱ���

	virtual void PreReleaseUnit( CUnit const *pkUnit );
	void ReleaseUnit( UNIT_PTR_ARRAY& rkUnitArray, bool bRecursiveCall=false );
	void ReleaseAllUnit();
	bool RemoveAllMonster(bool const bKill=false, CUnit *pUnit = NULL, int iVoluntarilyDie = 0);
	bool RemoveAllObject();

	CUnit* GetTarget( CUnit *pkUnit )const;
	void GetTargetList(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iInNum);
	void GetSummonUnitArray( CUnit *pkUnit, UNIT_PTR_ARRAY& rkUnitArray );
	
//	void GetFirstUnit( EUnitType const eUnitType, CONT_OBJECT_MGR_UNIT::iterator& rkItor );
//	CUnit* GetNextUnit( EUnitType const eUnitType, CONT_OBJECT_MGR_UNIT::iterator& rkItor );
	
	CUnit* GetPlayer(char const *pcCharacterName);
	CUnit* GetPlayer( std::wstring const &wstrName );

	int GetTargetInArea(CUnit* pkCaster, CSkillDef const *pkSkillDef, EUnitType const eUnitType, UNIT_PTR_ARRAY& rkTargetList, bool const bDamageFire, POINT3 const* pkDIr = NULL, SAddRange const& rkAddRange = SAddRange(0,0,0,0));

	POINT3 GetUnitPos( CUnit* pkUnit, bool bCheckImpact = false, float const fRayRange = 300.0f );
	
	void SetUserLife( PgPlayer *pkUser, int const iHP, int const iMP, DWORD const dwFlag=E_SENDTYPE_SELF );
	
	HRESULT InsertObjectUnit( TBL_DEF_MAP_REGEN_POINT const &rkGenInfo, SObjUnitBaseInfo const &kInfo, BM::GUID &rkOutGuid, CUnit* pkCaller=NULL );

	void UpdateAbilGuild(PgPlayer const *pkPC, const WORD eAbil);
	void UpdatePartyUnitAbil(PgPlayer const *pkPC, BYTE const cAbil);//��Ƽ �ɹ��� �������� HP����
	void SendNfyMapLoaded( PgPlayer* pkUser );// ��ӱ���
	virtual void WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const{}

	bool IsInTriggerUnit( GTRIGGER_ID const& rkTriggerID, CUnit const* pkUnit);
	bool SetTriggerEnable(GTRIGGER_ID const& rkTriggerID, bool const bNewEnable);
	bool RecvTriggerAction( CUnit *pkUnit, BM::Stream * const pkPacket );
	bool RecvTriggerActionScript(std::string const& rkID);

public:
	void InsertItemBox(POINT3 const &kptPos, VEC_GUID const &kOwners, CUnit* pkCaller, PgBase_Item const &kItem, __int64 const iMoney, PgLogCont &kLogCont, ETeam Team = TEAM_NONE, BM::GUID & OutGuid = BM::GUID::Create(), WORD wCollectRemainTime = 0 );//�������� ��������� �������� �ʴ´�.
	void InsertItemBox(TBL_DEF_MAP_REGEN_POINT const & GenInfo, VEC_GUID const & Owners, CUnit * pCaller, PgBase_Item const & Item, __int64 const iMoney, PgLogCont & LogCont, BM::GUID & OutGuid = BM::GUID::Create(), WORD wCollectRemainTime = 0 );//�ν��Ͻ� ������ ����.

	//	+-------------------------------------------------------+
	//	|	Map Move / Spawn Method								|
	//	+-------------------------------------------------------+
	bool SendToSpawnLoc(CUnit *pkUnit, int iPotalNo, bool bRandomize = true, int iSpawnType=E_SPAWN_DEFAULT);// ������������ ������ ����
	bool SendToSpawnLoc(BM::GUID const &rkCharGuid, int iPotalNo, bool bRandomize = true, int iSpawnType=E_SPAWN_DEFAULT);	// ������������ ������ ����

	HRESULT FindTriggerLoc( char const* szName, POINT3 &rkTriggerLoc_out, bool const bFindSpawn=false )const;
	HRESULT FindSpawnLoc( int iPotalNo, POINT3 &rkSpawnLoc_out, bool bRandomize = true, int iSpawnType=E_SPAWN_DEFAULT) const;
	bool SendToPosLoc( CUnit *pkUnit, POINT3 const &pt3Pos, BYTE const byType=MMET_Normal );
	bool MissionItemOrderCheck(PgPlayer *pkPlayer, PgReqMapMove& kMapMove, int const iMissionNo, int const iCount,int const iMissionLevel = 0);
	bool MissionItemOrderCheck(PgPlayer *pkPlayer, CONT_PLAYER_MODIFY_ORDER & kOrder, int const iMissionNo, int const iCount, int const iMissionLevel = 0);
	bool MissionChaosItemOrderCheck(PgPlayer *pkPlayer, PgReqMapMove& kMapMove, int const iMissionNo, int const iCount);
	bool MissionChaosItemOrderCheck(PgPlayer *pkPlayer, CONT_PLAYER_MODIFY_ORDER & kOrder, int const iMissionNo, int const iCount);
	
	void ReqMapMoveCome( SReqMapMove_CM const &kRMMC, VEC_GUID const &kUserList );
	bool AddPartyMember( PgPlayer *pkMaster, PgReqMapMove &kMapMoveAction );
	bool AddExpeditionMember( PgPlayer * pMaster, PgReqMapMove & MapMoveAction );
	void GetStatisticsPlayTime(__int64 i64Now, SMapPlayTime& rkOut);
	void SetEventQuest(PgEventQuest const & rkNewEventQuest);
	bool AddAnyPartyMember( PgPlayer *pkMember, PgReqMapMove &kMapMoveAction );
	bool AddAnyExpeditionMember( PgPlayer *pMember, PgReqMapMove & MapMoveAction );
	bool AddPartyAllMember( PgPlayer *pkMaster, PgReqMapMove &kMapMoveAction );

	virtual void ProcessGateWayUnLock(HRESULT const hRet, CUnit * pkCaster, BM::Stream & rkAddonPacket);

protected:
	virtual bool RecvMapMove( UNIT_PTR_ARRAY &rkUnitArray, SReqMapMove_MT& rkRMM, CONT_PET_MAPMOVE_DATA &kContPetMapMoveData, CONT_UNIT_SUMMONED_MAPMOVE_DATA &kContUnitSummonedMapMoveData, CONT_PLAYER_MODIFY_ORDER const &kModifyOrder );
	bool RecvMapMoveCome( SReqMapMove_CM const &kRMMC, BM::GUID const &kComeCharGuid );
	virtual bool RecvMapMoveTarget( BM::GUID const &kTargetCharGuid, BM::GUID const &kReqCharGuid, bool const bGMCommand );
	virtual void RecvMapMoveComeFailed( BM::GUID const &kCharGuid );

	virtual bool IsAccess( PgPlayer * ){return true;}
	void ProcessMapMoveResult( UNIT_PTR_ARRAY& rkUnitArray, SAnsMapMove_MT const &rkAMM );
	virtual bool RecvRecentMapMove( PgPlayer *pkUser );	// ����Ʈ�� ��ġ�� ���� ������!
	virtual bool AdjustArea( CUnit *pkUnit, bool const bIsSendAreaData, bool const bIsCheckPos );
	void ChangeArea( CUnit *pkUnit, PgSmallArea *pkNewArea, PgSmallArea *pkLastArea, bool const bSendAreaData );
	virtual void SendMapLoadComplete( PgPlayer *pkUser );

	void PartyBreakIn_CousumeKeyItem(PgPlayer * pPlayer); // ��Ƽ���� �� Ű������ �Ҹ�
public:

	void DisplayAreaState();
	//NiPick* GetPhysxPick(){return m_pkPhysxPick;}
	//int GetWayPointGroupCount();
	//int GetWayPointIndexCount(int const iGroup);

//	+-------------------------------------------------------+
//	|	AI Method											|
//	+-------------------------------------------------------+
public:
	bool FindEnemy( CUnit* pkFrom, UNIT_PTR_ARRAY& rkUnitArray, int const iMaxTarget = 5, int const iUnitType=UT_PLAYER|UT_MONSTER, bool const bFindFromGround = false, bool const bCallTimeCheck = true);	//������ ���� ���� Ȯ��
	bool Locked_IsUnitInRange(ETeam const eTeam, POINT3 const& ptFind, int const iRange, EUnitType const eType);
	void GetTriggerInRange( GTRIGGER_ID const& rkTriggerID, EUnitType const eUnitType, UNIT_PTR_ARRAY & rkUnitArray)const;
	void GetUnitInDistance( POINT3 const& ptPos, int const iRange, EUnitType const eType, UNIT_PTR_ARRAY& rkUnitArray, int const iZLimit = 0)const;
	void GetUnitInRange( POINT3 const& ptPos, int iRange, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray, int iZLimit = 0, bool const bFindFromGround = false);
	void GetUnitInRange( EUnitType & eCheckUnitType, POINT3 const& FromPos, int const iRange, EUnitType const eUnitType, UNIT_PTR_ARRAY & rkUnitArray, int const iZLimit = 0, bool const bFindFromGround = false);
	void GetUnitInCube( POINT3 const &ptCubeMin, POINT3 const &ptCubeMax, POINT3 const &ptPos, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray );
	CUnit* GetUnitByClassNo( int const iClassNo, EUnitType eType );
	int GetUnitByClassNo( int const iClassNo, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray );
    CUnit* GetUnitByType( EUnitType const eType );
	/*
	 * Use this function only in specific situation, because it copy ALL unit of type
	*/
    int GetUnitByType( EUnitType const eType, UNIT_PTR_ARRAY& rkUnitArray );
	bool GetVisionDirection(CUnit* pkUnit, NxVec3& rkDirectionOut, bool bUseTargetPos = true);
	void GetUnitInWidthFromLine(POINT3 const& ptStart, POINT3 const& ptEnd, int iWidth, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray);
	void GetUnitIn2DRectangle(POINT3 const& kP1, POINT3 const& kP2, POINT3 const& kP3, POINT3 const& kP4, EUnitType eType, UNIT_PTR_ARRAY& rkUnitArray); // Only use x,y
	void DequeTargetToUNIT_PTR_ARRAY(CUnit::DequeTarget const &rkDeque, UNIT_PTR_ARRAY& rkArr);
	NxShape* RayCast(NxRay& rkWorldRay, NxRaycastHit& rkHit, NxReal maxDist = 200.0f, NxU32 hintFlags = NX_STATIC_SHAPES) const;

	//GetTargetList�� �Լ��� ��ġ�� �ʵ���
	//pkUnit �� ���� Ÿ���� ��� ����Ʈ�� �־��ش�.
	void GetUnitTargetList(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int iRange = 0, int const iZLimit = AI_Z_LIMIT);

	EOpeningState ActivateMonsterGenGroup( int const iGenGroup=0, bool const bReset=true, bool const bOnce=false, int const iCopy=0 );

	bool IsInReturnZone(CUnit * pkUnit)const;
	bool IsInWalkingZone(CUnit * pkUnit)const;
	bool IsInRunningZone(CUnit * pkUnit)const;
	bool IsInBattleZone(CUnit * pkUnit)const;

protected:
	void FindUnit_WhenJoinedTeam(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit, int const iTeam, bool const bIfNoneRangeThenAddUnit = false );
	void FindUnit_WhenNoneJoinedTeam(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit);
	void FindUnit_DuelTarget(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit);
	void FindUnit_DuelTarget(EUnitType const eUnitType, CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iTargetType, int const iRange, int const iZLimit);
	void RemoveNoneHiddenUnit(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iRange, int const iZLimit);
	void RemoveAliveUnit(CUnit* pkUnit, UNIT_PTR_ARRAY& rkUnitArray, int const iRange, int const iZLimit);

	bool FindNearestUnt(const EUnitType eUT, POINT3 const& rkPos, float const fRange, int const iFindType, bool const bOnlyAlive, float* pfDist,
		BM::GUID& pkFindUnit, NxVec3& kNormalVector);
	CUnit* FindRandomUnit(POINT3 const &rkPos,float const fRange,const EUnitType eUnitType);
	
	HRESULT CheckMonsterDie(PgMonster* pkUnit);
	HRESULT CheckObjectUnitDie(PgObjectUnit* pkUnit);
	virtual HRESULT SetUnitDropItem(CUnit *pkOwner, CUnit *pkDroper, PgLogCont &kLogCont );
	HRESULT SetUnitDropItemImp(CUnit *pkOwner, CUnit *pkDroper, POINT3 const& kDropPos, PgCreateSpreadPos& kCreatePosAction, int iItemDropRate, float fMoneyDropRate, VEC_GUID const& kOwnerVec, TBL_DQT_DEF_MAP_ITEM_BAG const& kDefMapItemBag, PgLogCont &kLogCont);
	size_t GetAddDropItemCount(CUnit* pkOwner);
	void GetItemOwner(CUnit const* pkUnit, VEC_GUID & rkOwnerVec)const;

	bool InitRareMonsterGen();
	bool RareMonsterGenerate();
	bool GetRareMonster(int const iMonBagNo,int & iMonNo) const;
	bool RemoveRareMonster(int const iMonNo,PgMonster * pkMonster);
	bool GetRandomMonsterGenPoint(int const iGenGroup,PgGenPoint & kGenPoint);	// ���� �� �׷��� �������� ������Ʈ �ϳ��� ���� �´�.

public:

	void GetEventMonsterGenInfo(int MonsterNo, SEventMonsterGen & OutInfo) const;
	void DropAnyMonsterCheck(CONT_MONSTER_DROP_ITEM & DropItems) const;

protected:

	bool InitEventMonsterGen();
	void InitEventMonsterGen_sub(SEventGroup const& rkElement, VEC_EVENTMONSTERINFO const& rkMonsterInfo, bool const bInsertMonster, BM::PgPackedTime const& rkNowTime, __int64 const& rkCurTime);
	bool EventMonsterGenerate();
	void RemoveEventMonster(int MonsterNo);
	bool DropAnyMonsterTimeCheck();
	
	EOpeningState MonsterGenerate( ContGenPoint_Monster& kContGenPoint, int const iCopy=0, CUnit* pkCaller=NULL);
	EOpeningState InstanceItemGenerate(ContGenPoint_InstanceItem & ContGenPoint, int const iCopy = 0, CUnit* pCaller = NULL);

	EOpeningState ActivateObjectUnitGenGroup( int const iGenGroup, bool const bReset, bool const bOnce);
	EOpeningState ActivateInstanceItemGenGroup( int const GenGroup, bool const bReset, bool const bOnce, int const Copy);

	bool ObjectUnitGenerateGroundCheck(PgGenPoint_Object& kGenPoint, BM::GUID& kGuid);	
	EOpeningState ObjectUnitGenerate(ContGenPoint_Object & kContGenPoint, bool const bReset = false);
	void ObjectUnitGenerate( bool const bReset=false, int const iGenGroup=0 );

	int TickAI( CUnit* pkUnit, DWORD dwkElapsed );
	virtual HRESULT PlayerTick(DWORD const dwElapsed);
	virtual HRESULT PetTick(DWORD const dwElapsed);

	HRESULT MonsterTick(DWORD const dwElapsed,VEC_UNIT& rkDelUnitVec);
	HRESULT ObjectUnitTick( DWORD const dwElapsed, UNIT_PTR_ARRAY& rkDelUnitArray );
	virtual void EntityTick(CUnit * pkUnit, DWORD const dwkElapsed, float const fAutoHealMultiplier);
	virtual bool IsMonsterTickOK() const {	return PgObjectMgr::GetUnitCount(UT_PLAYER) > 0;}
	int MonsterTickAI( CUnit* pkUnit, DWORD dwkElapsed );

	bool IsAttackable(EAbilType eType, CUnit* pkCaster, CUnit *pkTarget,int& iSkillNo);
	//bool SkillFire(int const iSkillNo, CUnit* pkCaster, CUnit* pkTarget, PgActionResultVector* pkResult, SActArg* pkAct, bool bSendPacket = false);

	//void PlayerQuestTick(CUnit* pkUnit);

private:
	EOpeningState MonsterGenerate(PgGenPoint& rkGenPoint, int& iMonNo, BM::GUID& kMonGuid, CUnit* pkCaller = NULL);
	EOpeningState InstanceItemGenerate(PgGenPoint & GenPoint, int & ItemNo, BM::GUID & ItemGuid, CUnit * pCaller = NULL);
	EOpeningState ObjectUnitGenerate(PgGenPoint_Object& rkGenPoint, PgGenPoint_Object::OwnGroupCont const& kCont);
	EOpeningState ObjectUnitGenerate(PgGenPoint_Object& rkGenPoint);

//	+-------------------------------------------------------+
//	|	Network Method										|
//	+-------------------------------------------------------+
public:
	virtual void VUpdate( BM::CSubject< BM::Stream* > * const pChangedSubject, BM::Stream* iNotifyCause );
//	void SendNoticeToAllUser( std::wstring wstrContent );

	//bool SendToUser_ByGuid( BM::GUID& rkGuid, BM::Stream &rkPacket, DWORD const dwSendType = E_SENDTYPE_SELF );
	//bool SendToUser_ByGuidVec( VEC_GUID const &rkVec, BM::Stream const &rkPacket, BM::GUID const &rkPartyGuid, DWORD const dwSendType = E_SENDTYPE_SELF);//GroundMng�̿ܿ��� ȣ�� ���� ������
	bool SendToUser_ByGuidVec( VEC_GUID const &rkVec, BM::Stream const &rkPacket, DWORD const dwSendType = E_SENDTYPE_SELF);
	void SendToParty(BM::GUID const& rkPartyGuid, BM::Stream & Packet);

	void RecvGndWrapped( BM::Stream* const pkPacket );
	void RecvChatInputState(CUnit *pkUnit, BM::Stream *pkPacket);
	void OrderSyncGameTime(BM::Stream* const pkSendPacket);

	void Broadcast( BM::Stream const& rkPacket, BM::GUID const &kCallerCharGuid, DWORD const dwSendFlag=E_SENDTYPE_NONE);
	void Broadcast(BM::Stream const& rkPacket, CUnit const *pkCaller=NULL, DWORD const dwSendFlag=E_SENDTYPE_NONE);

protected:
	void Broadcast( BM::Stream const& rkPacket, VEC_GUID& rkVecUnit, DWORD const dwSendFlag=E_SENDTYPE_NONE);

	virtual bool VUpdate( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );
	virtual bool RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket );
	virtual bool RecvGndWrapped_ItemPacket(unsigned short usType, BM::Stream* const pkPacket );
	virtual bool Recv_PT_C_M_REQ_PET_ACTION( PgPlayer *pkOwner, BM::Stream * pkNfy );
	void SendContentsToDirect( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy );			// �׳� ������ ������ �ٷ� ������ ��Ŷ
	
	void SendToArea(BM::Stream *pkPacket, int const iAreaIndex, BM::GUID const &rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag );
	void SendToArea(BM::Stream *pkPacket, PgSmallArea const &rkSmallArea, BM::GUID const &rkIgnoreGuid, BYTE const bySyncTypeCheck, DWORD const dwSendFlag );
	void SendAreaData( CUnit *pkUnit, PgSmallArea const* pkTo, PgSmallArea const* pkFrom,  BYTE const bySyncType=SYNC_TYPE_DEFAULT );
	void SendAddUnitAreaData( PgNetModule<> const &kNetModule, PgSmallArea const* pkTo, PgSmallArea const* pkFrom, BM::GUID const &kIgnoreCharGuid );
	void EffectTick(CUnit* pkUnit, DWORD dwElapsed);

	void ProcNotifyMsg( CUnit* pkUnit, WORD const wType, BM::Stream* pkNfy);

	bool GMCommand( PgPlayer *pkPlayer, EGMCmdType const iCmdType, BM::Stream * const pkNfy );
	bool GMCommandToContents( PgPlayer *pkPlayer, EGMCmdType const iCmdType, BM::Stream * const pkNfy );
	virtual void GMCommand_RecvGamePoint( PgPlayer *pkPlayer, int const iPoint ){}

	void RecvChat( PgPlayer *pkUnit, BM::Stream *pkPacket );

//	+-------------------------------------------------------+
//	|	 ��Ƽ / ����Ʈ ����									|
//	+-------------------------------------------------------+
public:
	PgPlayer const* GetPartyMaster(PgPlayer const* pkPlayer)const;
	bool GetPartyMember(BM::GUID const & rkPartyGuid, VEC_GUID& rkOut)const;
	bool GetPartyMaster(CONT_PARTY_MASTER & rkOut)const;
	bool GetPartyMasterGuid(BM::GUID const & rkPartyGuid, BM::GUID& rkOutGuid)const;
	bool PgGround::GetPartyMaxLv(BM::GUID const& rkPartyGuid, int& rkOutLevel) const;
	bool GetPartyMemberGround(BM::GUID const & rkPartyGuid, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const & rkIgnore = BM::GUID::NullData())const;
	bool GetPartyMemberGround(PgPlayer const* pkPlayer, SGroundKey const& rkGndKey, VEC_GUID& rkOutVec, BM::GUID const & rkIgnore = BM::GUID::NullData())const;
	size_t GetPartyMemberCount(BM::GUID const & rkPartyGuid)const;
	//void SendToGroundPartyPacket(SGroundKey const& rkGndKey, BM::GUID const & rkPartyGuid, VEC_GUID const& rkRecvGuidVec, BM::Stream const& rkPacket, DWORD const dwSendType = E_SENDTYPE_SELF) const;
	bool GetPartyOption(BM::GUID const & rkPartyGuid, SPartyOption& rkOut);
	bool GetPartyShareItem_NextOwner(BM::GUID const & rkPartyGuid, SGroundKey const& rkGndKey, BM::GUID& rkNextOwner);
	int GetPartyMemberFriend(BM::GUID const & rkPartyGuid, BM::GUID const & rkCharGuid);
	bool CheckEffectFromPartyMember( BM::GUID const & PartyGuid, int const EffectNo ); // ��Ƽ������ �ش� ����Ʈ�� ��� �ɷ��ִ��� �˻�
	void GetIndunPartyGuid(CONT_INDUN_PARTY & rkCont);
protected:
	bool SetPartyGuid( CUnit* pkUnit, BM::GUID const& rkNewGuid, EPartySystemType const kCause );
	void SetPartyGuid(BM::GUID const& rkCharGuid, BM::GUID const& rkNewGuid, EPartySystemType const kCause );
	void SetPartyGuid(VEC_GUID const& rkVecGuid, BM::GUID const& rkNewGuid, EPartySystemType const kCause );
    virtual bool CheckApplyUnitOwner(CUnit* pkOwner, BM::GUID& rkOutApplyOwner);
    virtual int GetGiveLevel(CUnit * pkMonster, CUnit * pkOwner);
	virtual void GiveExp(CUnit* pkMonster, CUnit* pkOwner);
	virtual void CheckMonsterDieQuest(CUnit* pkCaster, CUnit* pkMonster);
	bool CheckUnitOwner(CUnit* pkUnit, BM::GUID& rkOut, BM::GUID * pkOutApplyOwner=NULL);
	void PartyBuffAction(int& iMasterGroundNo, BM::GUID& rkPartyGuid, BM::GUID& kCharGuid, CUnit* pkUnit, bool bBuff, BM::Stream* pkPacket);

	void SendShowQuestDialog(CUnit* pkUnit, BM::GUID const &rkNpcGUID, EQuestShowDialogType const eQuestShowDialogType, int const iQuestID, int const iDialogID);
	void GiveAwakeValue(CUnit* pkMurderee, CUnit* pkMurderer);

//	+-------------------------------------------------------+
//	|	 ������ ����										|
//	+-------------------------------------------------------+
public:

	bool GetExpeditionMember(BM::GUID const & ExpeditionGuid, VEC_GUID & Out);
	bool GetExpeditionMasterGuid(BM::GUID const & rkPartyGuid, BM::GUID& rkOutGuid)const;
	bool GetExpeditionMemberGround(BM::GUID const & ExpeditionGuid, SGroundKey const& GndKey, VEC_GUID & OutVec, BM::GUID const & Ignore = BM::GUID::NullData()) const;
	bool AccumulatePlayerHeal(CUnit * pHealCaster, int HealValue);	// ���� ��� ���� ��(������ ���������� ���).
	bool CheckEffectFromExpeditionMember( BM::GUID const & ExpeditionGuid, int const EffectNo ); // ����������� �ش� ����Ʈ�� ��� �ɷ��ִ��� �˻�
protected:

	typedef std::vector<SExpeditionInviteUserInfo> ContExpeditionInviteUserInfo;

	bool SetExpeditionGuid( CUnit* pUnit, BM::GUID const & NewGuid, EPartySystemType const Cause );
	void SetExpeditionGuid(BM::GUID const & CharGuid, BM::GUID const & NewGuid, EPartySystemType const Cause);
	void SetExpeditionGuid(VEC_GUID const& VecGuid, BM::GUID const& NewGuid, EPartySystemType const Cause);

//	+-------------------------------------------------------+
//	|	 NetWork Method										|
//	+-------------------------------------------------------+
	void Recv_PT_C_M_REQ_JOIN_LOBBY( CUnit *pkUnit, BM::Stream &rkPacket );
	void Recv_PT_T_M_ANS_REGIST_HARDCORE_VOTE( BM::Stream &rkPacket );
	void Recv_PT_C_M_REQ_RET_HARDCORE_VOTE( CUnit *pkUnit, BM::Stream &rkPacket );
	void Recv_PT_T_M_ANS_RET_HARDCORE_VOTE_CANCEL( BM::Stream &rkPacket );
	void Recv_PT_T_M_REQ_JOIN_HARDCORE( BM::Stream &rkPacket );

//	+-------------------------------------------------------+
//	|	 �Ƿε� ����										|
//	+-------------------------------------------------------+

public:

	void Recv_PT_T_M_NFY_RECOVERY_STRATEGY_FATIGUABILITY(int RecoveryValue);

protected:

//	+-------------------------------------------------------+
//	|	 AI ����										    |
//	+-------------------------------------------------------+
    void StartAI() { m_bRunAI = true; }
    void StopAI() { m_bRunAI = false; }
    bool IsRunAI() const { return m_bRunAI; }

public:
	void SyncPlayerPlayTime(BM::GUID const kCharGuid, int const iAccConSec, int const iAccDisSec, __int64 const i64SelectSec);

//	+-------------------------------------------------------+
//	|	��ӹ��� Ŭ������ �ʿ信 ���� �߰��ϴ� �Լ�			|
//	+-------------------------------------------------------+
public:
//	PgPvPGround Method

public://���� �� AI Protected �� �Ұ�.
	//void SendNftMonsterGoal(CUnit* pkUnit,CUnit* pkTarget,POINT3& kPos);
	//void ProcessMonsterAttack(CUnit* pkMonster, CUnit* pkTarget, NxVec3& kNormal, int iSkill);
	//bool FindWayPos(CUnit* pkUnit);
	//bool FindBeforeWayPoint(int const& iGroup, int const& iIndex,POINT3& pt3NextPos);
	//bool FindNearWayPoint(POINT3 const& pt3Pos,int& iGroup , int& iIndex);
	//bool FindNextWayPoint(int const& iGroup, int const& iIndex,POINT3& pt3NextPos);
	//PgWayPoint2* GetWayPoint(int iGroup, int iIndex);

	bool GroundWeight(int const iWeight);
	void TunningLevel(int const iLevel);
	bool GetHaveAbilUnitArray(UNIT_PTR_ARRAY &rkUnitArray, WORD const kAbilType, EUnitType const eUnitType );
	int GroundTunningNo()const;

// ������
protected:
	bool ReqJoinHardCoreDungeon( PgPlayer * pkUser, SGroundKey const *pkDungeonGndKey );
	
	void AddUnitArray( UNIT_PTR_ARRAY &kUnitArray, EUnitType const kUnitType );

	void ReqProjectileAction(CUnit* pkSendUnit, BM::Stream * const pkPacket);
	void RecvReqAction2(CUnit* pkUnit, SActionInfo& rkAction, BM::Stream * const pkPacket);
	virtual bool VCheckUsableSkill(CUnit* pkUnit, const SActionInfo& rkAction) { return true; } //�׶��� ���� ��ų ��� ������ üũ�ϱ� ���� �Լ�

	EActionResultCode CheckAction2(CUnit* pkUnit, const SActionInfo& rkAction);
	bool CheckAction2_SubFunc_CheckPlayerAction(PgPlayer* pkPlayer, CSkillDef* pkSkillDef, SActionInfo const& rkAction, EActionResultCode& rkOut);

	bool FireSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec);
	bool FireToggleSkill(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec);
	void FireSkillFailed(CUnit* pkCaster, UNIT_PTR_ARRAY& rkTargetArray, const SActionInfo &rkAction, PgActionResultVector* pkActionResultVec);	
	void RecvReqTrigger(CUnit* pkUnit, int const iType, BM::Stream *pkNfy);
	void RecvReqRegQuickSlot(CUnit* pkUnit, size_t const slot_idx, SQuickInvInfo const& kQuickInvInfo);
	void RecvReqRegQuickSlotViewPage(CUnit* pkUnit, char const cViewPage);
	void RecvReqCastBegin(CUnit* pkUnit, int const iSkillNo, DWORD const dwCurrentTime);
	void ReqMonsterTrigger(CUnit* pkUnit, BM::GUID const & kObjectGuid, int iActionType);

	void CheckTargetList(UNIT_PTR_ARRAY& rkTargetArray); // ��� �´� Ÿ���� �ִ��� �˻��Ͽ� ����Ʈ�� �����Ѵ�.
	bool CheckHackingAction(CUnit* pkUnit, SActionInfo& rkAction); // �׼��� ��ŷ�� ������ üũ �Ѵ�.
	void GetClinetProjectileInfo(CUnit* const pkCaster, SActionInfo const & rkAction, CSkillDef const* pkSkillDef, SClientProjectile & kInfo) const;
	void CheckEffectUser(CUnit* pkUnit);
	void HiddenMapCheck(CUnit* pkUnit);
	void HiddenMapTimeLimit(CUnit* pkUnit);
	void GetHiddenMapTime(DWORD &m_dwTotalTimeLimit);

	//>>Quest
	void SendAllQuestInfo(PgPlayer* pkPlayer);
	void ReqIntroQuest(CUnit *pkUnit, int const iQuestID, BM::GUID const& rkObjectGuid);
	void ReqRemoteCompleteQuest(CUnit *pkUnit, int const iQuestID);
	//<<Quest
	void ReqNpcTrigger(CUnit* pkUnit, BM::GUID const& kObjectGuid, int iActionType);

	//>>WorldEvent
	virtual void DoWorldEventAction(int const iGroundNo, SWorldEventAction const& rkAction, PgWorldEvent const& rkWorldEvent);
	virtual void DoWorldEventCondition(int const iGroundNo, SWorldEventCondition const& rkCondition, PgWorldEvent const& rkWorldEvent, CUnit const* pkCaster = NULL);
	virtual void NfyWorldEventToGroundUser(PgWorldEventState const& rkState);
	//<<WorldEvent
	//>>
	virtual void NfyWEClientOjbectToGroundUser(PgWEClientObjectState const& rkState);
	//<<

	virtual void DynamicGndAttr(EDynamicGroundAttr const eNewDGAttr);
	void CheckTickMarry();
	void CheckTickHidden();
	void CheckTickCouple();

	virtual void RecvUnitDie(CUnit *pkUnit);
	virtual bool PushBSInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem)		{ return false; }

	virtual bool PushMissionDefenceInvenItem(CUnit* pkUnit, PgBase_Item const& rkItem)		{ return false; }

	virtual void Defence7ItemUse(CUnit* pkUnit, BM::Stream * const pkPacket);
    int GetGuardianTunningNo(int const iGuardian)const;
	void SetGuardianAbil(CUnit* pkUnit, SMISSION_DEFENCE7_GUARDIAN const& kValue)const;
	void SetGuardianAbilUpdate(CUnit* pkUnit, int const iMonAbilNo)const;
//---------------------------------------------------------------------
// Alram Mission
	virtual void OnTick_AlramMission( PgPlayer *pkPlayer, DWORD const dwCurTime );
	virtual bool IsAlramMission(void)const{return ( NULL != m_pkAlramMissionMgr );}

public:
	virtual void CallAlramReward( PgPlayer * pkPlayer );
// Alram Mission
//======================================================================
//---------------------------------------------------------------------
// SimpleMarket ����
public:
	bool RecvReqVendorCreate(CUnit* pkUnit, BM::GUID const& rkVendorGuid, std::wstring const& VendorTitle);
	bool RecvReqVendorRename(CUnit* pkUnit, BM::GUID const& rkVendorGuid, std::wstring const& VendorTitle);
	bool RecvReqVendorDelete(CUnit* pkUnit, BM::GUID const& rkVendorGuid);
	bool RecvReqReadToPacketVendorName(CUnit* pkUnit, BM::GUID const& rkVendorGuid, BM::Stream &rkPacket);
	void RecvVendorClose(CUnit* pkUnit);
//======================================================================
	CLASS_DECLARATION_S(DWORD, LastGenCheckTime);
	CLASS_DECLARATION_S_NO_SET(BM::GUID, GroundMute);
	//CLASS_DECLARATION_S(DWORD, LastTickTime);
#ifdef DEF_ESTIMATE_TICK_DELAY
	CLASS_DECLARATION_S(DWORD, Tick100msDelayAverage);
#endif
	CLASS_DECLARATION_S_NO_SET(EDynamicGroundAttr, DynamicGndAttr);
	CLASS_DECLARATION_S_NO_SET(PgEventQuest, EventQuest);
	CLASS_DECLARATION_S(SMapPlayTime, PlayTime);
	CLASS_DECLARATION_S(__int64, LastPlayTimeCalculateTime);	// CGameTime::GetLocalTimeSec()
	typedef std::vector< DWORD > LASTTICKTIME_VEC;
	LASTTICKTIME_VEC m_kContLastTickTime;
	CLASS_DECLARATION_S_NO_SET(PgWorldEnvironmentStatus, WorldEnvironmentStatus);
	CLASS_DECLARATION_S(DWORD, LastAddExpInVillageTickTime);
	CLASS_DECLARATION_S(DWORD, DefendModeTickTime);
	CLASS_DECLARATION_S(bool, HiddenMapOut);
	CLASS_DECLARATION_S(bool, IndunPartyDie);
	CLASS_DECLARATION_S(bool, Paused);

public:
	int GetMapLevel()const{return m_iMapLevel;}
	void SetMapLevel( int const iMapLevel );

	HRESULT SetEvent( TBL_EVENT const &kTblEvent );

	virtual bool IsBSItem(int const iItemNo) const				{ return false; }
	void UpdateWorldEnvironmentStatus(SWorldEnvironmentStatus const& rkNewEnvStatus);

	void SetRealmQuestStatus(SRealmQuestInfo const& rkRealmQuestInfo);

    virtual void SetTeam(BM::GUID const& kGuid, int const iTeam);
    virtual int GetTeam(BM::GUID const& kGuid)const;
	virtual bool IsDefenceItemList(int const iItemNo) const		{ return false; }
	bool GetDefenceModeMatchParty(BM::GUID const& kMyPartyGuid, BM::GUID& rkPartyGuid);
	int DefenceIsJoinParty(BM::GUID const& kPartyGuid);
	bool DefenceAddWaitParty(BM::GUID const& kPartyGuid, int eType);
	bool DefenceModifyWaitParty(BM::GUID const& kPartyGuid, int eType);
	bool DefenceDelWaitParty(BM::GUID const& kPartyGuid);
	void SendDefencePartyAllList(CUnit* pkUnit);

	PgJobSkillLocationMgr& JobSkillLocationMgr()		{ return m_kJobSkillLocationMgr; }
	void JobSkillLocationItemInit();
	void CheckTickJobSkillLocationItem();
    bool IsEventScriptPlayer()const { return !m_kContEventScriptPlayer.empty(); }

	void RemoveInstanceItem(TBL_DEF_MAP_REGEN_POINT const & GenInfo, BM::GUID const & ItemGuid);
	void RemoveReserveDeleteInsItem(BM::GUID const & CharGuid);
	void ReserveDeleteInsItem(BM::GUID const & CharGuid, TBL_DEF_MAP_REGEN_POINT const & GenInfo, BM::GUID const & ItemGuid);
	void UpdateDeleInsItemOwner(BM::GUID const & Looter, BM::GUID const & Droper);
	bool RemoveGenGroupInsItem(int const iGenGroupNo);

///////////////////////////////////////////////////////////////////////
///////////////////////// Ŀ�´�Ƽ �̺�Ʈ //////////////////////////////////
////////////////////////////////////////////////////////////////////////////
public:

	EErrorEventMapMove Recv_PT_C_M_REQ_MOVE_EVENT_GROUND(CUnit * pUnit, BM::Stream & Packet);
	void MoveToEventGround(int EventNo, PgPlayer * pPlayer, int GroundNo, bool bMaster = false, bool bSendCenter = false);
	void Recv_PT_T_M_NFY_COMMUNITY_EVENT_GROUND_STATE_CHANGE(int EventNo, bool EventMapState);
	void Recv_PT_T_M_NFY_EVENT_GROUND_USER_COUNT_MODIFY(int EventNo, int UserCount);

protected:

	virtual void AddJoinPartyMemberList(BM::Stream & Packet);
	void DelJoinPartyMemberList(BM::GUID & CharGuid);
	void CheckJoinPartyMemberList(CUnit * pUnit);

////////////////////////////////////////////////////////////////////////////

protected:
	
	int					m_iMapLevel;
	ContArea			m_kAreaCont;	//Small Area
	PgSmallArea*		m_pkBigArea;
	int					m_iGroundWeight;
	int					m_iTunningLevel;
	int					m_iGroundTunningNo;
    SET_GUID            m_kContEventScriptPlayer; //�������� EventScript UserGuid
	bool				m_bIsBossRoom;
	int					m_iMapMoveResultStartTimeSec;

	int m_iMonsterControlID;	//�̸� ���� ���� �����..
	DWORD m_dwTimeStamp;

	ContGenPoint_Monster		m_kContGenPoint_Monster;//���� ��������Ʈ
	ContGenPoint_Object			m_kContGenPoint_Object;// ObjectUnit ���� ����Ʈ
	ContGenPoint_InstanceItem	m_ContGenPoint_InstanceItem_AnyTeam;// InstanceItem ���� ����Ʈ
	ContGenPoint_InstanceItem	m_ContGenPoint_InstanceItem_RedTeam;// InstanceItem ���� ����Ʈ
	ContGenPoint_InstanceItem	m_ContGenPoint_InstanceItem_BlueTeam;// InstanceItem ���� ����Ʈ
	ContGenPoint_Object			m_kContGenPoint_SummonNPC;// SummonedNPC ���� ����Ʈ

	PgAIManager m_kAIMng;	// �ϴ� Ground�� �Ҽӵǵ��� �� ��������, Global�� �� ���� ���� �ϴ�.(Lock������ �ذ� �ȴٸ� ����)

	CONT_MONSTER_GEN		m_kContRareMonsterGen;// ��� ���� �� ���̺�
	CONT_GENERATED_MONSTER	m_kContGeneratedMonster;//�̹� ���� ���� ���̺� ���� ��� ���Ͱ� �ٽ� �� �ɼ� ����.

	CONT_EVENT_MONSTER_GEN	m_ContEventMonsterGen; // �̺�Ʈ ���� �� ���̺�.
	CONT_GENERATED_MONSTER	m_ContGeneratedEventMonster; //�̹� ���� ���� ���̺� ���� �̺�Ʈ ���Ͱ� �ٽ� �� �ɼ� ����.

	VEC_EVENTDROPANYMONSTER m_ContEventDropAnyMonster; // �̺�Ʈ �� Ư�� �̺�Ʈ�� ��� ���Ϳ��Լ� ������ �������� ����ǵ��� �Ѵ�.

	CONT_DELETERESERVERINSITEM m_ContDeleteReserveInsItem;	// ���� �ֱⰡ 0���� ������ �ν��Ͻ� �������� ���ϴ� ������ ������Ű�� ���ϴ� ������.

	CONT_EVENT_MAP_ENTERABLE_ENTER_LIST	m_EventMapEnterableList;

	CONT_CURRENT_EVENT_MAP_USER_COUNT_LIST m_CurrentEventMapUserCountList;

	// �̸� ������ ��Ƽ�� ������ ĳ���͵��� ����Ʈ. ���ӽÿ� ������ ��Ƽ�� �־��ش�.
	CONT_JOIN_PARTY_MEMBER_LIST m_JoinPartyMemberList;

	// �׶����� ������(����� �������Ƹ� ����Ѵ�. ���߿� MyHome������ ��� �� �� �ִ�.
	SGroundOwnerInfo		m_kGroundOwnerInfo;

	CONT_REALM_QUEST_INFO	m_kRealmQuestInfo;

public:
	bool PickUpGroundBox(CUnit* pkUnit, BM::GUID const& rkGuid, CUnit* pkLooter = NULL);
	void AddEffectToAllPlayer(int const iEffectNo);
	void HiddenMapRewordItem(CUnit* pkUnit);

public:
	// Ư���ɼ�
	virtual bool IsGroundEffect()const{return true;}// �׶���� ����Ʈ ��� ����
	virtual bool IsUseItem()const{return true;}// �Һ� �������� ��� ����
	virtual bool IsDecEquipDuration()const{return true;}// �׾����� ������ ������ ���
	virtual bool IsDeathPenalty()const{return true;}// �׾����� �г�Ƽ �ֱ�

protected:
	static void SendUnitPosChange( CUnit *pkUnit, BYTE const eType, DWORD const dwSendFlag );

	virtual bool IsMacroCheckGround() const;
	void SetChangeGroundOwnerInfo( SGroundOwnerInfo const &kInfo, bool const bBroadCast=true );

	// Observer Mode Packet
	HRESULT SetOBMode_Start( PgPlayer *pkCasterPlayer, CUnit *pkTarget );
	HRESULT SetOBMode_Start( PgNetModule<> const &kNetModule, BM::GUID const &kCharID, CUnit *pkTarget, bool const bOnlyModule );
	HRESULT SetOBMode_End( BM::GUID const &kCharID );

	bool ProcessObserverPacket( BM::GUID const &kCharGuid, PgNetModule<> const &kNetModule, PgPlayer *pkPlayer, BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream &rkPacket );
	int GetMapAbil(WORD const wType);	// public �Լ� �ƴ�..(Lock ������)

	mutable Loki::Mutex m_kRscMutex;

	DWORD m_dwOnTick1s_Check2s;	//2�ʸ��� ������ ���ǵ���ŷ ī��Ʈ�� �ʱ�ȭ ���� �ٷ���.
	typedef std::map<int, int> CONT_MAPABIL;
	CONT_MAPABIL	m_kContMapAbil;


protected:
	PgLocalPartyMgr		m_kLocalPartyMgr;
	PgUserQuestSessionMgr m_kQuestTalkSession;
	PgLocalVendorMgr	m_kLocalVendorMgr;

	//
	PgMarryMgr				m_kMarryMgr;
	PgHiddenRewordItemMgr	m_kHiddenRewordItemMgr;
	PgEventItemSetMgr		m_kEventItemSetMgr;
	PgDefencePartyMgr		m_kDefencePartyMgr;
	PgJobSkillLocationMgr	m_kJobSkillLocationMgr;
	PgDuelMgr				m_kDuelMgr;

private:

	bool Recv_PT_I_M_MYHOME_MOVE_TO_HOMETOWN(BM::Stream* const pkPacket );
    
    bool m_bRunAI;
	__int64 m_i64EventViewOldTime;

	CUnit* IsUnitInRangeImp(CUnit *pUnit, POINT3 const& ptFind, int const iRange, EUnitType const eType, int const iZLimit, bool const bFindFromGround);
	CUnit* GetReservedUnitByEffect(CUnit *pUnit);

public: 
	void SetEventScript_LockPartyMember(CUnit const *pkUnit, int const iEventScriptID);
	void SetEventScript_UnLockPartyMember(CUnit const *pkUnit);
	
	void Set_LockPlayerInGround();
private:
	void SetEventScript_PartyLock( CUnit const *pkUnit, int const iEventScriptID, BM::Stream::DEF_STREAM_TYPE const kPacketType );

/////////////////////////// ���ڸ� ���� ////////////////////////////////////
public:
	DWORD const Constellation_CheckCondition(PgPlayer *pPlayer, Constellation::ConstellationKey const& Key, bool const UseCashItem, int & NeedItemNo, int & NeedItemCount);
	Constellation::ECE_RESULT Constellation_MoveDungeon(PgPlayer *pPlayer, Constellation::ConstellationKey const& Key, int & NeedItemNo, int & NeedItemCount);
	Constellation::ECE_RESULT Constellation_MoveDungeon_Boss(PgPlayer *pPlayer, Constellation::SConstellationKey const& Key);
	void Constellation_NotifyResult(PgPlayer *pPlayer, DWORD const dwRet, int const worldNo, Constellation::ConstellationKey const& Key);

	void SetConstellationKey(Constellation::SConstellationKey const& constellationKey) { m_ConstellationKey = constellationKey; }
	Constellation::SConstellationKey const& GetConstellationKey()const { return m_ConstellationKey; }

	void Constellation_GetMapNo(VEC_INT & ContMapNo);
private:
	Constellation::SConstellationKey m_ConstellationKey;

	bool Constellation_CheckCondition_CashItem(PgPlayer *pPlayer, int & NeedItemNo, int & NeedItemCount);
	DWORD Constellation_CheckCondition_DayofWeek(Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo);
	DWORD Constellation_CheckCondition_Level(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo);
	DWORD Constellation_CheckCondition_Item(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo, int & NeedItemNo, int & NeedItemCount);
	DWORD Constellation_CheckCondition_Quest(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo);

	bool Constellation_Consume_Item(PgPlayer *pPlayer, Constellation::CONT_CONSTELLATION_ENTER::mapped_type const& MissionInfo
									, PgReqMapMove & MapMove, int const& NeedItemNo, int const& NeedItemCount); // ���� ������ �Һ�
////////////////////////////////////////////////////////////////////////////

private:
	std::set<int> m_kGroundMutator; // Used only for mission map
	MutatorBoostAbil m_kMutatorBoostAbil;
public:
	//! Get Ground Mutator
	std::set<int>& GroundMutator() { return this->m_kGroundMutator; }

	void MakeMutatorAbil();

	void SetMutatorAbil(const int iType, const int iValue);

	int GetMutatorAbil(const int iType) const;

	//Clear Mutator Abil, use for clear abil
	inline void ClearMutatorAbil() { this->m_kGroundMutator.clear(); this->m_kMutatorBoostAbil.clear(); }
};

#include "PgGround.inl"

float const COUPLE_PASSIVE_HPMP_INCREASE_EFFECT_ABLE_DIST = 600.f;	//��԰� �˸԰� ����Ʈ ���� �Ÿ�
float const DEFAULT_CLASS_AWAKE_VALUE_REVISER = 2.4f;
#endif // MAP_MAPSERVER_MAP_GROUND_PGGROUND_H