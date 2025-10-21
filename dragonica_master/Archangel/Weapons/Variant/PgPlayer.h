#ifndef WEAPON_VARIANT_UNIT_PGPLAYER_H
#define WEAPON_VARIANT_UNIT_PGPLAYER_H

#include "Lohengrin/PacketStruct.h"
#include "Lohengrin/PacketStruct2.h"
#include "Variant/AlramMission.h"
#include "FCS/AntiHack.h"
#include "PgControlUnit.h"
#include "PgParty.h"
#include "PgExpedition.h"
#include "PgMyQuest.h"
#include "PgQuickInventory.h"
#include "PgUserMapInfo.h"
#include "PgPlayer_MissionReport.h"
#include "PgPlayTime.h"
#include "PgAchievements.h"
#include "PgCharacterCard.h"
#include "PgHiddenOpen.h"
#include "PgJobSkillExpertness.h"
#include "PgJobSKillSaveIdx.h"
#include "PgPremiumMgr.h"
#include "PgDailyReward.h"
#include "Variant/PgBattlePassMgr.h"

#pragma pack(1)

typedef struct tagPLAYER_ABIL
{
	tagPLAYER_ABIL()
	{
		sSP = 0;	// Skill Point
		sJumpHeight = 0;	// AT_JUMP_HEIGHT
		sCJumpHeight = 0;	// AT_C_JUMP_HEIGHT
		sAddMoneyPer = 0;	// AT_ADD_MONEY_PER

		iHairColor = 0;
		iHairStyle = 0;
		iFace = 0;
		iJacket = 0;
		iPants = 0;
		iShoes = 0;
		iGloves = 0;
		byFiveElementBody = 0;
	}

	short int sSP;	// Skill Point
	short int sJumpHeight;	// AT_JUMP_HEIGHT
	short int sCJumpHeight;	// AT_C_JUMP_HEIGHT
	short int sAddMoneyPer;	// AT_ADD_MONEY_PER

	int iHairColor;
	int iHairStyle;
	int iFace;
	int iJacket;
	int iPants;
	int iShoes;
	int iGloves;
	BYTE byFiveElementBody;
}PLAYER_ABIL;

typedef struct tagPlayerLevelupInfo
{
	tagPlayerLevelupInfo()
	{
		sAddSP = 0;
	}

	SPlayerBasicInfo kBasic;
	short int sAddSP;	// Added SkillPoint
}PLAYER_LEVELUP_INFO;

const HRESULT S_LEVEL_UP = 100;
const HRESULT S_CHANGE_CLASS = 101;

typedef struct tagPlayerActionInfo
{
	tagPlayerActionInfo()
	{
		Init();
	}
	void Init()
	{
		dwLastTime = 0;
		dwCalculateTime = 0;
		iActionID = 0;
		iOldActionID = 0;
		iMoveSpeed = 0;
		bDashJump = false;
		iGroundNo = 0;
	}

	void SetPos(DWORD const dwTime, POINT3 const &ptNew, bool const bSimulate = false)
	{
		if (!bSimulate)
		{
			dwLastTime = dwTime;
		}
		dwCalculateTime = dwTime;
		ptPos = ptNew;
	}

	DWORD dwLastTime;	// 마지막으로 Pos(클라로 부터 받은값) 설정된 시간
	DWORD dwCalculateTime;	// 최근 위치 Simulate 된 시간
	int iActionID;
	int iOldActionID;
	int iMoveSpeed;
	int iGroundNo;
	POINT3 ptPos;
	bool bDashJump;
}PLAYER_ACTION_INFO;

typedef struct tagPlayerActionTimeCount
{
	typedef struct tagActionInfo
	{
		DWORD	m_dwActionTime;			// 액션을 한 시간
		DWORD	m_dwServerTime;			// 액션 패킷을 받은 서버 시간
		int		m_iAnimationTime;		// 액션의 애니 시간
		int		m_iActionID ;			// 액션의 ID
		BYTE	m_byActionParam;		// 액션의 상태

		tagActionInfo() :
			m_dwActionTime(0),
			m_dwServerTime(0),
			m_iAnimationTime(0),
			m_iActionID(0),
			m_byActionParam(0)
		{}

		void Init()
		{
			m_dwActionTime = 0;
			m_dwServerTime = 0;
			m_iAnimationTime = 0;
			m_iActionID = 0;
			m_byActionParam = 0;
		}
	}ACTION_INFO;

	DWORD m_dwCheckFirstCleintTime;		// Client Time
	DWORD m_dwCheckFirstServerTime;		// Server Time
	int m_iRecvPacketCount;
	int m_iTotalActionAniTime;
	ACTION_INFO m_kLastActionInfo;
	
	tagPlayerActionTimeCount() :
		m_dwCheckFirstCleintTime(0),
		m_dwCheckFirstServerTime(0),
		m_iTotalActionAniTime(0),
		m_iRecvPacketCount(0)		
	{}
}PLAYER_ACTION_TIME_COUNT;

typedef struct tagPlayerActionTimeCountResult
{
	bool m_bHackingWarning;
	int m_iClientElapsedTime;
	int m_iServerElapsedTime;
	int m_iRecvPacketCount;
	int m_iTotalActionAniTime;

	tagPlayerActionTimeCountResult() :
		m_bHackingWarning(false),
		m_iClientElapsedTime(0),
		m_iServerElapsedTime(0),
		m_iRecvPacketCount(0),
		m_iTotalActionAniTime(0)	
	{}

}PLAYER_ACTION_TIME_COUNT_RESULT;

typedef enum // int형으로 32가지 bool형 옵션을 저장할수 있도록 한다.
{
	OPTION_TYPE_SHOW_HELMET	= 0, // 투구 보이기 옵션
	OPTION_TYPE_MAX			= (1 << 0),

}CLIENT_OPTION_SAVE_TYPE;
typedef enum
{
	TELE_NONE = 0,
	TELE_JUMP = 1,
	TELE_MOVE = 2,
	TELE_PARTY_MOVE = 3,		// 신청자 본인
	TELE_PARTY_MOVE_OTHER = 4,	// 타 파티원 (트리거에 위치 하지 않음)
	
	TELE_PARTY_MOVE_ROCKET = 6,		// 신청자 본인
	TELE_PARTY_MOVE_ROCKET_OTHER = 7,	// 타 파티원 (트리거에 위치 하지 않음)
	
	TELE_MOVE_DIRECT_OTHER_REQ = 8,	// 요청 - 파티원 애니없이 바로 이동
	TELE_MOVE_DIRECT_OTHER_ACT = 9,	// 액션 - 파티원 애니없이 바로 이동
} HYPERMOVE_TYPE;

typedef struct tagHyperMoveInfo
{
	HYPERMOVE_TYPE	kType;			//텔레점프인지 텔레무브인지
	BM::GUID		kAuthGuid;		//서버에서 발급된 임시 GUID
	DWORD			dwArriveTime;	//도착예정 시간. 클라 알 필요 없음
	POINT3			kArrivePos;		//도착 예정 위치. 클라 알 필요 없음
//	int				iAddAbilCount;	//AddAbil 된 횟수
	
	tagHyperMoveInfo()/* : iAddAbilCount(0)*/
	{
		Init();
	}

	bool IsValidType() const	{ return kType!=TELE_NONE; }

	void Init(HYPERMOVE_TYPE kHyperType = TELE_NONE)
	{
		kType = kHyperType;
		kAuthGuid.Clear();
		dwArriveTime = 0;
		kArrivePos = POINT3I(0,0,0);
	} 

	bool IsInPos(POINT3 const& rkPos, float const fDist = 80.0f) const
	{
		return fDist >= POINT3::Distance(kArrivePos, rkPos);
	}

} PLAYER_HYPERMOVE_INFO;

size_t const PLAYER_POS_SIMULATE_MIN_INTERVAL = 400;

typedef struct tagUSER_PORTAL
{
	tagUSER_PORTAL(){}
	explicit tagUSER_PORTAL(std::wstring const & kCmt,int const iGndNo,POINT3 const & kP3):
	kComment(kCmt),iGroundNo(iGndNo),kPos(kP3){}
	std::wstring kComment;
	int iGroundNo;
	POINT3 kPos;

	size_t min_size()const
	{
		return 	sizeof(size_t)+
				sizeof(iGroundNo)+
				sizeof(kPos);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kComment);
		kPacket.Push(iGroundNo);
		kPacket.Push(kPos);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kComment);
		kPacket.Pop(iGroundNo);
		kPacket.Pop(kPos);
	}
}SUSER_PORTAL;

#pragma pack()

typedef std::map<BM::GUID,SUSER_PORTAL> CONT_USER_PORTAL;

typedef enum eAliveType
{
	EALIVE_NONE,
	EALIVE_PVP,//PVP에서 살아났다.
	EALIVE_INSURANCE,//보험으로 살아났다
	EALIVE_MONEY,//돈으로 살아났다
	
	EALIVE_ITEM,//아이템으로 살아났다
	EALIVE_SKILL,//스킬로 살아났다
}EALIVETYPE;

typedef enum
{
	EDBSAVE_DEFAULT =		0x01,
	EDBSAVE_PVP		=		0x02,
	EDBSAVE_MISSION	=		0x04,
	EDBSAVE_EXTERN	=		EDBSAVE_PVP|EDBSAVE_MISSION,
}EDBSaveType;

typedef enum
{
	E_RANKPOINT_NONE = 0,
	E_RANKPOINT_MAXCOMBO,

	// 여기서 부터는 DB에만 있고 메모리에 가지고 있지는 않는다.
	E_RANKPOINT_FOODMAKE,
	E_RANKPOINT_ENCHANTSUCCEED,
	E_RANKPOINT_ENCHANTFAILED,
	E_RANKPOINT_RECVMAIL_COUNT,
	E_RANKPOINT_USEPHOENIXFEATHER_COUNT,
}ERankPointType;

typedef enum
{
	E_MISSION_EVENT_NONE	= 0,
	E_MISSION_EVENT_SET		= 1,
	E_MISSION_EVENT_RESET	= 2,
}EMissionEventPointType;

typedef enum
{
	E_MISSION_ABIL_AWAKE_STATE = 0x100,
	E_MISSION_ABIL_AWAKE_VALUE,
}EMissionAbilType;

typedef enum
{
	GMABIL_NONE				= 0x00,
	GMABIL_HIDDEN			= 0x01,// GM 숨기 기능
	GMABIL_CANNOT_DAMAGE	= 0x02,// GM 공격안받기
}EGMAbil;
typedef BYTE GMABIL_TYPE;

typedef std::set<WORD> CONT_ACHIEVEMENT_ABIL;

typedef std::set<T_PT3<int> >	CONT_MACRO_AREA;
typedef std::map<int,int>		CONT_MACRO_SKILL;
typedef std::map<int,int>		CONT_MACRO_ITEM;
typedef std::map<int,int>		CONT_MACRO_MONSTER_KILL;
typedef std::vector<SCHARACTER_CARD_ABIL> VEC_CARD_ABIL;
typedef std::deque<DWORD>		CONT_HISTORYJOBITEM;

template<int NUM>
class PgBitArray
{
public:
	
	PgBitArray(){Clear();}

	explicit PgBitArray(PgBitArray const & rhs)
	{
		(*this) = rhs;
	}

	explicit PgBitArray(BYTE const abyData[NUM])
	{
		Set(abyData);
	}

	~PgBitArray(){}

public:

	void Clear()
	{
		::memset(m_bBuff,0,sizeof(m_bBuff));
	}

	void Set(BYTE const abyData[NUM])
	{
		::memcpy(m_bBuff,abyData,NUM);
	}

	bool Set(int const idx,bool const bVal)
	{
		int const iBytePos = idx/8;
		if(NUM <= iBytePos)
		{
			return false;
		}
		int const iBitPos = idx%8;

		if(true == bVal)
		{
			m_bBuff[iBytePos] = m_bBuff[iBytePos] | static_cast<BYTE>(0x01 << iBitPos); 
		}
		else
		{
			m_bBuff[iBytePos] = m_bBuff[iBytePos] & ~static_cast<BYTE>(0x01 << iBitPos); 
		}

		return true;
	}

	void Get(BYTE abyData[NUM]) const
	{
		::memcpy(abyData,m_bBuff,NUM);
	}

	bool Get(int const idx) const
	{
		int const iBytePos = idx/8;
		if(NUM <= iBytePos)
		{
			return false;
		}
		int const iBitPos = idx%8;
		return (m_bBuff[iBytePos] & static_cast<BYTE>(1 << iBitPos)) > 0;
	}

	PgBitArray & operator = (PgBitArray const & rhs)
	{
		::memcpy(m_bBuff,rhs.m_bBuff,sizeof(m_bBuff));
		return (*this);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(m_bBuff);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(m_bBuff);
	}

private:
	BYTE m_bBuff[NUM];
};

typedef struct tagENCHANT_BACKUP
{
	BM::GUID kItemGuid;
	SEnchantInfo kEnchant;
	tagENCHANT_BACKUP(){}
	explicit tagENCHANT_BACKUP(BM::GUID const & _guid,SEnchantInfo const & _enchant):kItemGuid(_guid),kEnchant(_enchant){}
}SENCHANT_BACKUP;

typedef std::map<int, int> CONT_GUARDIAN_INSTALL_DISCOUNT;

class PgPlayer
	: public PgControlUnit
	, public PgPlayerPlayTime
	, public SPvPReport
{
public:
	PgPlayer(void);
	virtual ~PgPlayer(void);

public:
	virtual void Init();
	HRESULT Create( BM::GUID const &guidMember, SPlayerDBData const &kDBInfo, SPlayerBasicInfo const &kBasicInfo, SPlayerBinaryData const &kBinaryData);

	ERACE_TYPE UnitRace(void) const;
	virtual EUnitType UnitType()const{ return UT_PLAYER; }
	virtual void CalculateInitAbil();

	bool SetAbilDirect(WORD const Type, int const iValue);
	virtual int GetAbil(WORD const Type) const;
	virtual bool SetAbil(WORD const Type, int const iInValue, bool const bIsSend = false, bool const bBroadcast = false);
	virtual __int64 GetAbil64(WORD const eAT)const;
	virtual bool SetAbil64(WORD const Type, __int64 const iInValue, bool const bIsSend = false, bool const bBroadcast = false);

	virtual bool AddAbil(WORD const Type, int const iValue);
	virtual bool AddAbil64(WORD const Type, __int64 const iValue);

	virtual void WriteToPacket(BM::Stream &rkPacket, EWRITETYPE const kWriteType=WT_DEFAULT)const;
	virtual EWRITETYPE ReadFromPacket(BM::Stream &rkPacket);
	
	virtual void Send(BM::Stream const &Packet, DWORD const dwSendFlag = E_SENDTYPE_SELF/*E_SENDTYPE_TYPE&*/);
	virtual void Invalidate();
	
	virtual bool SetState(EUnitState const eState);
	virtual void MapLoading()	{ m_State = US_OPENING;	}
	virtual bool IsMapLoading();
	// bRegen이 true면 해당그라운드에서 다시 살아나는 것이다.
	// iNewHP, iNewMP = -1 이면 최대값으로 바꾸어 준다.
	virtual void Alive( EALIVETYPE const bAliveType=EALIVE_NONE, DWORD const dwFlag=E_SENDTYPE_SELF, int const iNewHP = -1, int const iNewMP = -1 );
		
	void SendAllAbil();
	void RecvAllAbil(BM::Stream &rkPacket);

	HRESULT SetTacticsExp(__int64 const iVal);
	HRESULT AddTacticsExp(__int64 const iAddVal);

	virtual void SetPos(POINT3 const &ptNew);
	virtual void SendWarnMessage( int const iMessageNo, BYTE const byLevel=EL_Warning );
	virtual void SendWarnMessage2( int const iMessageNo, int const iValue, BYTE const byLevel=EL_Warning );
	virtual void SendWarnMessage3( int const iMessageNo, BM::Stream const& kValue, BYTE const byLevel=EL_Warning );
	virtual void SendWarnMessageStr(BM::vstring const& rkStr, BYTE const byLevel=EL_Warning, bool bForce = false);
	virtual void SendWarnMessageStrDebug(BM::vstring const& rkStr, BYTE const byLevel=EL_Warning);

	SERVER_IDENTITY const& GetSwitchServer()const {return m_kNetModule.GetNet(); }	
	BM::GUID const &GetMemberGUID() const { return m_kNetModule.GetID(); }
	short GetChannel()const{return m_kNetModule.GetNet().nChannel;}

	PgNetModule<> const& GetNetModule(void)const{return m_kNetModule;}

//	bool SetInfo(SPlayerDBSimpleData const &rkSimple, SPlayerBasicInfo* pkBasic);

	virtual bool OnDamage( int& iPower,int& iHP );
	void RecvLatency(DWORD const kLatencyTime, POINT3 const & kPos);
	bool HaveParty()const{return ( PartyGuid() != BM::GUID::NullData() );}
	bool HaveExpedition()const{return ( ExpeditionGuid() != BM::GUID::NullData() );}
	bool HaveExpeditionParty()const{return ( ExpeditionPartyGuid() != BM::GUID::NullData() );}

	void SetActionID(unsigned int const iActionID, BYTE const btDirection = DIR_NONE);
	int GetActionID()const;

	bool GmAbilCheck( GMABIL_TYPE const kCheck )const;
	GMABIL_TYPE GmAbilAdd( GMABIL_TYPE const kGmAbil, bool const bAdd);
	PLAYER_ACTION_INFO* GetPlayerActionInfo() { return &m_kActionInfo; }
	PLAYER_ACTION_INFO* GetOldPlayerActionInfo() { return &m_kOldActionInfo; }

	void GetRecentGround(SGroundKey& rOutkKey,T_GNDATTR const kGndAttr)const;
	int	GetRecentMapNo(T_GNDATTR const kGndAttr)const;
	POINT3 const &GetRecentPos(T_GNDATTR const kGndAttr)const;
	bool UpdateRecent( SGroundKey const &kGndKey, T_GNDATTR const kGndAttr );

	bool IsReadyNetwork(void)const{return m_kNetModule.IsReady();}
	void ReadyNetwork(bool const bReady){m_kNetModule.SetReady( bReady );}
	bool DoSimulate()const{ return m_bDoSimulate; }
	void DoSimulate(bool const bDoSimulate) { m_bDoSimulate = bDoSimulate; }

	PgPlayer_MissionData *GetMissionData( unsigned int iMissionKey ){return m_kMissionReport.Get(iMissionKey);}
	bool UpdateMission( SMissionKey const& kMissionKey, BYTE const kStageFlag ){return m_kMissionReport.Update(kMissionKey,kStageFlag);}

	void GetPlayerAbil(PLAYER_ABIL &rkOut)const;

	bool SetDefaultItem(int const iEquipPos,int const iItemNo);
	
	void CopyTo(SContentsUser &rkOut)const;
	void CopyTo(PgPlayer& rkPlayer, EWRITETYPE eType) const;

	BYTE GMLevel()const{return m_byGMLevel;}
	bool IsPCCafe()const{ return m_bIsPCCafe; }
	BYTE PCCafeGrade()const{ return m_byPCCafeGrade; }
	BM::PgPackedTime const& BirthDate()const{return m_kInfo.kBirthDate;}

	void ClearInstantAbils(T_GNDATTR const attr);

	BYTE GetDBPlayerState()const		{ return m_kInfo.byState; }
	void ClearDBPlayerState()			{ m_kInfo.byState = 0; }
	void ClearIngQuest();

	void AccumulateDamage(int iDamage);	// 원정대 던전에서 데미지 누적.
	void AccumulateHeal(int iHeal);		// 원정대 던전에서 힐량 누적.
	void ClearAccmulateValue();			// 원정대 던전 누적량 초기화.

	void SetRecentNormalMap(SRecentPlayerNormalMapData const & MapData);
	void GetRecentNormalMap(SRecentPlayerNormalMapData & OutMapData);

	void SetRecentMapInfo(int const GroundNo, POINT3 const & Pos);
	void SetRecentMapInfo(SRecentInfo const& kRecentInfo);
	SRecentInfo const& GetRecentMapInfo()const;
	
	virtual int GetMaxSummonUnitCount()						{ return 20; }
public:
	bool UpdateWorldMap( int const iGroundNo );
	HRESULT IsOpenWorldMap( int const iGroundNo )const;

	void GuildGuid(BM::GUID const & kGuildGuid){m_kInfo.kGuildGuid = kGuildGuid;}
	void CoupleGuid(BM::GUID const & kCoupleGuid);
	void PartyGuid(BM::GUID const & kGuildGuid){m_kPartyGuid = kGuildGuid;}
	void ExpeditionGuid(BM::GUID const & kExpeditionGuid){m_kExpeditionGuid = kExpeditionGuid;}
	void ExpeditionPartyGuid(BM::GUID const & kExpeditionPartyGuid){m_kExpeditionPartyGuid = kExpeditionPartyGuid;}

	BM::GUID const & GuildGuid()const{return m_kInfo.kGuildGuid;}
	BM::GUID const & CoupleGuid()const{return m_kInfo.kCoupleGuid;}
	BM::GUID const & PartyGuid()const{return m_kPartyGuid;}
	BM::GUID const & ExpeditionGuid()const{return m_kExpeditionGuid;}
	BM::GUID const & ExpeditionPartyGuid()const{return m_kExpeditionPartyGuid;}

	virtual BM::GUID const& GetGuildGuid() const { return GuildGuid(); }	
	virtual BM::GUID const& GetCoupleGuid() const { return CoupleGuid(); }
	virtual BM::GUID const& GetPartyGuid() const { return PartyGuid(); }
	virtual BM::GUID const& GetExpeditionGuid() const { return ExpeditionGuid();}
	virtual BM::GUID const& GetExpeditionParty() const { return ExpeditionPartyGuid();}

public:
	PgQuickInventory* GetQInven() { return &m_kQInv; }
	PgMyQuest const * GetMyQuest()const{ return &m_kQuest; }
	SUserQuestState const* GetQuestState(short int const sQuestID) const	{ return m_kQuest.Get(sQuestID); }
	void UpdateQuestResult(DB_ITEM_STATE_CHANGE_ARRAY::value_type const &rkResult);
	void UpdateQuestResult(SUserQuestState* pkInfo, int iQuestValue, int const iQuestType);
	int GetRandomQuestCompleteCount() const;
	int GetRandomQuestPlayerLevel() const;
	int GetRandomTacticsQuestCompleteCount() const;
	int GetRandomTacticsQuestPlayerLevel() const;
	void EndQuestMigration(bool const bIsNewbiePlayer);
	PgAchievements * GetAchievements(){return &m_kAchievements;}
	PgHiddenOpen * GetHiddenOpen(){return &m_kHiddenOpen;}

	// 현상 수배 퀘스트 Wrapper
	void InitWantedQuestClearCount();
	void SetWantedQuestClearCount(ContQuestClearCount const &rkContQuestClearCount);
	void AddWantedQuestClearCount(int const iQuestID);
	int  GetWantedQuestClearCount(int const iQuestID) const;
	void SetExistCanBuildWantedQuest( const bool bCanBuild );

	void SendHyperMoveInfoToClient();
	bool SetHyperMoveInfo(HYPERMOVE_TYPE const kHyperType, DWORD const dwCurrentTime = 0, POINT3 const& rkPos = POINT3(0.0f,0.0f,0.0f));//서버용
	void SetHyperMoveInfo(BM::GUID const& rkGuid) { m_kHyperMoveInfo.kAuthGuid = rkGuid; }	//이건 클라용
	HRESULT EndHyperMove(POINT3 const& rkEndPos, BM::GUID const& rkGuidFromClient, bool const bRealyJump, float& fErrorDist);
	BM::GUID const& GetHyperJumpGuid() const { return m_kHyperMoveInfo.kAuthGuid; }
	void Update(unsigned long const ulElapsedTime);
	void ClearHyperJumpInfo() { m_kHyperMoveInfo.Init(); }

	void ForceSetCoupleColorGuid(BM::GUID const & kCoupleColorGuid);
	BM::GUID const& CoupleColorGuid() const { return m_kInfo.kCoupleColorGuid; }

	void SetCoupleStatus(BYTE const & kStatus);
	BYTE const& GetCoupleStatus() const { return m_kInfo.kCoupleStatus; }

	virtual void InvenRefreshAbil();

	int GetGuardianDiscountRate(int GuardianType);			// 가디언 설치비 할인률을 얻어옴.
	void SetGuardianDiscountRate(int GuardianType, int RateValue);	// 가디언 설치비 할인률 설정.
	void SetGuardianDicountContainer(CONT_GUARDIAN_INSTALL_DISCOUNT & ContDiscount);	// 할인률 컨테이너 설정.

protected:
	virtual void VOnDie();
	virtual bool OnSetAbil(WORD const Type, int const iValue);

	void CopyAbilFromClassDef();

	virtual bool DoLevelup( SClassKey const& rkNewLevelKey );
	virtual bool DoBattleLevelUp( short const nBattleLv );

	void SetUsedAchievementAbilList(WORD const wAbilType);	

public:
	HRESULT CheckActionPacketCount(SActionInfo const& rkActionInfo);
	bool ResultActionPacketCount(bool bForceResult = false, PLAYER_ACTION_TIME_COUNT_RESULT* pkResult = NULL, DWORD  dwInitTime = 30000, int iActionCountSum = 100, int iCheckTimeMutiflier = 0);
	PLAYER_ACTION_TIME_COUNT const& GetActionPacketCountInfo() const { return m_kPacketCounter; }
	bool SuspectHacking(short const sHackType, short const sAddIndex);	// true = Hacking이 확실함.

	BM::GUID const SelectedPetID(void);
	
	typedef std::vector<short> VECTOR_MAX_HACKINDEX;
	static VECTOR_MAX_HACKINDEX m_sMaxHackIndex;
	
protected:
	PgNetModule<>			m_kNetModule;

	SPlayerDBData			m_kInfo;

	PgUserMapInfo< DWORD >	m_kWorldMapFlag;
	BM::GUID m_kPartyGuid;
	BM::GUID m_kExpeditionGuid;				// 원정대 파티
	BM::GUID m_kExpeditionPartyGuid;		// 원정대에서 자신이 속한 파티

	SRecentPlayerNormalMapData m_RecentNormalMap; // 원정대 채널로 이동할 때 마지막 맵번호와 위치를 저장.
	
	BYTE m_byGMLevel;
	bool m_bIsPCCafe;
	BYTE m_byPCCafeGrade;
	GMABIL_TYPE m_kGmAbil;// EGMAbil사용(bit)

	bool m_bDoSimulate;

	PLAYER_ACTION_INFO		m_kActionInfo;	// Client와 Player의 위치 동기화를 위해서 필요하다.
	PLAYER_ACTION_INFO		m_kOldActionInfo; //해킹 체크를 위한 이전 액션 정보를 저장한다.

public:
	PgAlramMission& GetAlramMission(void){return m_kAlramMission;}
	PgAlramMission const& GetAlramMission(void)const{return m_kAlramMission;}

	CLASS_DECLARATION_S(std::wstring, MemberID);

	// To Do : VolatileID 
	// 셋팅은 오직 Contents Server의 ProcessSelectCharacter에서만 한다.
	// 다른데서는 절대 건드리지 마라!!! 죽음이다..
	CLASS_DECLARATION_S(BM::GUID, VolatileID);
	
	CLASS_DECLARATION_S(DWORD, NotifyTime);	// SpeedHack 을 막기위해 클라이언트가 자신의 GameTime 값을 계속 보낸다.
	CLASS_DECLARATION_S(DWORD, Latency);
	CLASS_DECLARATION_S(POINT3, OldPos);	
	CLASS_DECLARATION_S(POINT3BY, MoveDirection);
	CLASS_DECLARATION_S(BM::GUID, FollowingHeadGuid);// 따라가기 주체자(Head) GUID
	CLASS_DECLARATION_S(SConnectionArea, ConnArea);
	CLASS_DECLARATION_S(SGroundKey, GroundKey);
	CLASS_DECLARATION_S(int, Cash);
	CLASS_DECLARATION_S(POINT3, PosAtGround);	//바닥에 닿았을때의 좌표를 저장
	CLASS_DECLARATION_S(DWORD, PosAtGroundTime);	//바닥에 닿았을때의 좌표를 계산한 마지막 시간을 저장
	CLASS_DECLARATION_S(CEL::ADDR_INFO,addrRemote);	// 유저 ip
	CLASS_DECLARATION( int, m_iUID, UID );// NC에서 사용하는 UID
	
	void SetCardAbil(VEC_CARD_ABIL const& rkContCardAbil);		// 캐릭터 카드 어빌 셋팅
	void ResetCardAbil();	// 캐릭터 카드 어빌 삭제

	CLASS_DECLARATION_S(PgBitArray<MAX_DB_EMOTICON_SIZE>,Emoticon);
	CLASS_DECLARATION_S(PgBitArray<MAX_DB_EMOTICON_SIZE>,Emotion);
	CLASS_DECLARATION_S(PgBitArray<MAX_DB_EMOTICON_SIZE>,Balloon);

	bool GetContEmotion(BYTE const _type,PgBitArray<MAX_DB_EMOTICON_SIZE> & kCont);
	bool SetContEmotion(BYTE const _type,PgBitArray<MAX_DB_EMOTICON_SIZE> const & kCont);

	CLASS_DECLARATION_S( SRecentInfo, TransTowerSaveRct )// TransTower에서 저장한 위치

//	일회성 변수들
	CLASS_DECLARATION_S(BM::GUID,CashShopGuid); //캐시샵 진입시 마다 지급되는 구매용 guid

//	Map에서만 통하는 변수
	CLASS_DECLARATION_S(BM::GUID, CreatePetID);

	CLASS_DECLARATION_S(BYTE, ExtendCharacterNum);

	CLASS_DECLARATION_S(BM::GUID, SubPlayerID);	// 쌍둥이 Class의 SubPlayer

	CLASS_DECLARATION_NO_SET(int, m_iAccruedDamage, GetAccruedDamage);	// 원정대 던전에서 데미지 누적용 변수.
	CLASS_DECLARATION_NO_SET(int, m_iAccruedHeal, GetAccruedHeal);		// 원정대 던전에서 힐량 누적용 변수.
	CLASS_DECLARATION_S(T_GNDATTR, Gattr);								// 원정대 던전에서 데미지, 힐량 누적해야하는데,, 던전안에서만 해야 되서
																		// 플레이어가 있는 맵속성을 가지고 있게..

	CLASS_DECLARATION_S(float, DefenceMssionExpBonusRate);	// 전략 디펜스 모드 추가 경험치 비율.

	CLASS_DECLARATION_S(bool, BearEffect);	// 현재 곰 디버프가 걸린 상태인가
protected:
	PLAYER_ACTION_TIME_COUNT	m_kPacketCounter; //액션 패킷이 얼마나 오는지 체크
	PLAYER_HYPERMOVE_INFO		m_kHyperMoveInfo;

	short int m_ksJumpHeight;	// AT_JUMP_HEIGHT
	short int m_ksCJumpHeight;	// AT_C_JUMP_HEIGHT
	short int m_ksAddMoneyPer;	// AT_ADD_MONEY_PER

	PgPlayer_MissionReport	m_kMissionReport;
	mutable PgQuickInventory m_kQInv;
	PgMyQuest m_kQuest;

	PgAchievements m_kAchievements;
	PgHiddenOpen m_kHiddenOpen;

	CONT_ACHIEVEMENT_ABIL m_kContAchievementAbil;		
	typedef std::map<short, short> CONT_HACKINDEX;
	CONT_HACKINDEX m_kHackIndex;

	PgAlramMission	m_kAlramMission;// Alram Mission

	CONT_GUARDIAN_INSTALL_DISCOUNT m_GuardianDiscount;	// 가디언 설치비 할인 컨테이너(first : guardian type, second : discount rate).

public:
/*
	메크로 체크 관련 맴버들.....
*/

	CLASS_DECLARATION_S(bool,MacroWaitAns);			// 메크로 응답 대기 여부
	CLASS_DECLARATION_S(bool,MacroMode);			// 메크로 모드 false : 정상 true : 메크로 사용
	CLASS_DECLARATION_S(bool,MacroRecvExp);			// 메크로 경험치 획득 여부
	CLASS_DECLARATION_S(__int64,MacroModeStartTime);	// 메크로 모드 타임 아웃
	CLASS_DECLARATION_S(__int64,MacroInputTimeOut);	// 메크로 입력 타임 아웃
	CLASS_DECLARATION_S(__int64,MacroLastRecvExpTime);	// 메크로 마지막 경험치 획득 시간
	CLASS_DECLARATION_S(int,MacroKeyCount);				// 메크로 키 개수
	CLASS_DECLARATION_S(short,MacroInputFailCount);	// 메크로 응답 실패 카운트
	CLASS_DECLARATION_S(short,MacroCheckCount);		// 메크로 사용 누적 카운트
	CLASS_DECLARATION_S(short,MacroPopupCount);		// 메크로 팝업 카운트

public:
	void MacroClear();
	void MakeMacroCheckPassword(wchar_t & kCharKey,int & iCount);
	bool MacroUseDetected();
	bool ProcessMacroInputTimeOut();
	bool ProcessMacroCheckPassword(int const iCount);
	void MacroResetInputTime();
	bool IsOpenStrategySkill(ESkillTabType const eTabType)const;
	bool GetSwapStrategySkillTabNo(ESkillTabType & eTabType)const;

	BM::PgPackedTime const & RefreshDate(){return m_kInfo.kRefreshDate;}
	void RefreshDate(BM::PgPackedTime const & kDate){m_kInfo.kRefreshDate = kDate;}
	int const RecommendPoint(){return m_kInfo.iRecommendPoint;}
	void RecommendPoint(int const iVal){m_kInfo.iRecommendPoint = iVal;}
	CLASS_DECLARATION_S(CONT_USER_PORTAL,ContPortal);
	CLASS_DECLARATION_S(bool,IsCreateCard);
	void SetDateContents(EDateContentsType const eType, BM::DBTIMESTAMP_EX const& rkDateTime);
	bool GetDateContents(EDateContentsType const eType, BM::DBTIMESTAMP_EX& rkOutDateTime) const;

	bool InsertPortal(CONT_USER_PORTAL::key_type const & kKey,CONT_USER_PORTAL::mapped_type const & kPortal);
	bool RemovePortal(CONT_USER_PORTAL::key_type const & kKey);
	bool GetPortal(CONT_USER_PORTAL::key_type const & kKey,CONT_USER_PORTAL::mapped_type & kPortal);

	PgJobSkillExpertness& JobSkillExpertness()				{ return m_kJobSkillExpertness; }
	PgJobSkillExpertness const& JobSkillExpertness() const	{ return m_kJobSkillExpertness; }
	CONT_HISTORYJOBITEM const& ContJobSkillHistoryItem() const	{ return m_kContJobSkillHistoryItem; }
	bool AddJobSkillHistoryItem(DWORD const iSaveIdx);

	bool EffectEscapeKeyDown(int const iEffectID);

	//
	void SetChatBlockList(CONT_BLOCKCHAT_LIST & ChatBlockList) { m_ChatBlockList.swap(ChatBlockList); }
	void GetChatBlockList(CONT_BLOCKCHAT_LIST & ChatBlockList);
	bool AddChatBlockList(std::wstring & CharName, BYTE BlockType);
	void RemoveChatBlockList(std::wstring & CharName);
	bool CheckChatBlockList(std::wstring & CharName);
	void ModifyChatBlockCharacter(std::wstring & CharName, BYTE BlockType);

	// 프리미엄서비스
	PgPremiumMgr & GetPremium(){ return m_kPremium; }
	inline PgDailyReward & GetDailyReward() { return m_kDailyReward; }
	inline PgBattlePassInfo & GetBattlePassInfo() { return m_kBattlePassInfo; }

public:
//	마이홈
	CLASS_DECLARATION_S(SHOMEADDR,HomeAddr);

//	업적 저장 시간
	CLASS_DECLARATION_S(DWORD,AchievementLastSaveTime);
protected:
//노점
	CLASS_DECLARATION_S(bool, OpenVendor);
	CLASS_DECLARATION_S(std::wstring, VendorTitle);
	CLASS_DECLARATION_S(BM::GUID, VendorGuid);

protected:
	CAbilObject m_kCardAbilObj;		// 캐릭터 카드

	PgJobSkillExpertness	m_kJobSkillExpertness;	// 채집스킬 숙련도
	CONT_HISTORYJOBITEM		m_kContJobSkillHistoryItem;	// 채집스킬 최근에 제작한 아이템

	PgPremiumMgr m_kPremium;
	PgDailyReward m_kDailyReward;
	PgBattlePassInfo m_kBattlePassInfo;
	CONT_BLOCKCHAT_LIST m_ChatBlockList;

	CLASS_DECLARATION_S(bool, VisibleMyMinimapIcon);	// 미니맵에 자신의 아이콘이 보여지는 상태

private:
	bool m_bInitComplete;
public:
	void SetCompleteInit(void) { m_bInitComplete = true; }
	bool IsCompleteInit(void) { return m_bInitComplete; }
};

namespace PgPlayerUtil
{
	bool IsNewbiePlayer(SPlayerDBData const& rkDBData);
	bool IsNewbiePlayer(PgPlayer const* pkPlayer);
};

inline BM::GUID const PgPlayer::SelectedPetID(void)
{
	BM::GUID kPetID;
	m_kInv.GetSelectedPetID( kPetID );
	return kPetID;
}

#endif // WEAPON_VARIANT_UNIT_PGPLAYER_H