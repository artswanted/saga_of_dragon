#ifndef MAP_MAPSERVER_FRAMEWORK_CONSTANT_H
#define MAP_MAPSERVER_FRAMEWORK_CONSTANT_H

// Object Attribute type
typedef enum eQuestObjectAttr
{
	QOA_None = 0,
	QOA_Item = 1,		//가지고 있어야할 Item
	QOA_Item_Or,
	QOA_Quest,			//선행 퀘스트
	QOA_Level,			//권장 레벨
	QOA_Class,			//수락 가능 클래스
	QOA_Location,		//방문했어야할 위치
	QOA_Party,			//권장 파티원 수
	QOA_Count,			//이 퀘스트를 몇명이 받아 갈 수 있느냐?
}EQuestObjectAttr;

// Quest Limit PreQuest Type
#define QUEST_LIMIT_PREQUEST_CANWITHDROP		1

// Quest Client/Payer
typedef enum eQuestNpcType
{
	QNT_Client = 1,		//퀘스트를 주는 NPC
	QNT_Agent = 10,		//퀘스트 진행과 연관된 NPC
	QNT_Payer = 101,	//퀘스트를 완료 해주는 NPC
}EQuestNpcType;

//// Dialog Type
//#define DIALOG_TYPE_BALLOONTIP		1
//#define DIALOG_TYPE_QUEST_INTRO		2
//#define DIALOG_TYPE_QUEST			3

// ETC
#define SCRIPT_MAX_PATH		40

#define SAFE_DELETE_NI(p)	{ if(p) { NiDelete(p); (p)=NULL; }}

/////////////////////////////////////////////////////////////////
// Structure
////////////////////////////////////////////////////////////////

typedef enum ePVPAreaType
{
	PAT_NONE			= 0,
	PAT_KTH_HILL		= 1,
}EPVPAreaType;

/*
typedef struct tagPVPAreaPos
{
	tagPVPAreaPos()
		:	eType(PAT_NONE)
		,	x(0.0f)
		,	y(0.0f)
		,	z(0.0f)
		,	fRadius(0.0f)
	{
	}
	tagPVPAreaPos(const EPVPAreaType eInType, float const fx, float const fy, float const fz, float const fr)
	{
		eType = eInType;
		x = fx;
		y = fy;
		z = fz;
		fRadius = fr;
	}
	tagPVPAreaPos(POINT3 const& rhs)
	{
		*this = rhs;
	}
	void operator = (const tagPVPAreaPos& rhs)
	{	
		eType = rhs.eType;
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		fRadius = rhs.fRadius;
	}

	EPVPAreaType eType;
	float x;
	float y;
	float z;
	float fRadius;
}SPVPAreaPos;
*/

typedef enum eTickInterval
{
	// 값을 0부터 시작하여 빠짐없이 순서대로 증가시킬것
	// vector의 index 값으로 사용하고 있음.
	ETICK_INTERVAL_100MS	= 0,	
	ETICK_INTERVAL_30S		= 1,
	ETICK_INTERVAL_1S		= 2,
	ETICK_INTERVAL_2S		= 3,
	ETICK_INTERVAL_5S		= 4,
	ETICK_INTERVAL_1M		= 5,
	ETICK_INTERVAL_MAX		,
}ETickInterval;

//int const iCanAddExpLevelDiff = 15;

typedef enum eVar_Kind
{
	EVar_Kind_None = 0,
	EVar_Kind_Hunting = 1,
	EVar_Kind_Guild = 2,
	EVar_Kind_Player = 3,
	EVar_Kind_AI = 4,
	EVar_Kind_System = 5,
	EVar_Kind_Mission = 6,
	EVar_Kind_Socket = 7,
	EVar_Kind_Log = 8,
	EVar_Kind_ExpCard = 9,
	EVar_Kind_OpenMarket = 10,
	EVar_Kind_Pet = 11,
	EVar_Kind_GM_Emoticon = 12,
	EVar_Kind_MyHome = 13,
	EVar_Kind_EntityExtInfo = 14,
	EVar_Kind_Hidden = 15,
	EVar_Kind_EachClassAwakeValueReviser = 16,
	EVar_Kind_JobSkillPenalty = 17,
	EVar_Kind_DualKeyboardHack = 18,
	EVar_Kind_Premium_Service = 19,
	EVar_Kind_Enchant = 20,


} EVar_Kind;

typedef enum eVar_Socket
{
	EVar_Socket_Generate_SuccessRate = 1,
	EVar_Socket_Generate_NeedCost = 2,
	EVar_Socket_Generate_NeedSoul = 3,
}EVar_Socket;

typedef enum eVar_Hunting
{
	EVar_MonsterKill_SystemEnable = 1,		// MonsterKill Count 를 할것인가?
	EVar_MonsterKill_ResetCount = 2,		// 언제 카운트를 리셋 할거냐?
	EVar_MonsterKill_PerMsg = 3,			// 얼마마다 패킷을 보내줄 거냐?
	EVar_MonsterKill_CanMinDiffLevel = 4,	// 얼마에서 얼마 차이의 몬스터만 카운트 할거냐?
	EVar_MonsterKill_CanMaxDiffLevel = 5,	// ..
	EVar_MonsterExp_CanMinDiffLevel = 6,	// 얼마에서 얼마 차이의 몬스터만 경험치 줄거냐?
	EVar_MonsterExp_CanMaxDiffLevel = 7,
	EVar_DropBox_CanAnyPickUpItem = 8,		// 얼마나 지나면 누구나 먹을 수 있느냐?
	EVar_FranExp = 9,						// Fran 경험치 %
} EVar_Hunting;

typedef enum eVar_Guild
{
	EVar_Guild_CanCreateLevel = 1,
	EVar_Emporia_CanUseFunction = 2,
	EVar_Emporia_Battle_UserCount_MinInTeam = 3,	// 엠포리아전쟁 최소 유저 카운트 수
	EVar_Emporia_Battle_UserCount_MaxInTeam = 4,	// 엠포리아전쟁 최대 유저 카운트 수
	EVar_Emporia_Battle_ProtectedDragon_DefenceWinPoint = 5,//엠포리아전쟁 드래곤방어전 방어팀 승리점수
	EVar_Emporia_Battle_DestroyCore_Auth_TotalPoint = 6,
	EVar_Emporia_Battle_DestroyCore_Unit_KillPoint = 7,
} EVar_Guild;

typedef enum eVar_Player
{
	EVar_Player_AutoHeal_Multiplier_NotInVillage = 1,
} EVar_Player;

typedef enum eVar_AI
{
	UseDetailAI = 1,
	UseRayCache = 2,
} EVar_AI;

typedef enum eVar_System
{
	EVar_System_RunDebugMode = 1,
}EVar_System;

typedef enum eVar_Mission
{
	EVar_Mission_PartyMember1_AttackScore = 1,
	EVar_Mission_PartyMember2_AttackScore = 2,
	EVar_Mission_PartyMember3_AttackScore = 3,
	EVar_Mission_PartyMember4_AttackScore = 4,
	EVar_Mission_PartyMember1_ComboScore = 5,
	EVar_Mission_PartyMember2_ComboScore = 6,
	EVar_Mission_PartyMember3_ComboScore = 7,
	EVar_Mission_PartyMember4_ComboScore = 8,
	EVar_Mission_PartyMember1_DemageScore = 9,
	EVar_Mission_PartyMember2_DemageScore = 10,
	EVar_Mission_PartyMember3_DemageScore = 11,
	EVar_Mission_PartyMember4_DemageScore = 12,
	EVar_Mission_PartyMember1_TimeSec = 13,
	EVar_Mission_PartyMember2_TimeSec = 14,
	EVar_Mission_PartyMember3_TimeSec = 15,
	EVar_Mission_PartyMember4_TimeSec = 16,
	EVar_Mission_PartyMember1_TimeScore = 17,
	EVar_Mission_PartyMember2_TimeScore = 18,
	EVar_Mission_PartyMember3_TimeScore = 19,
	EVar_Mission_PartyMember4_TimeScore = 20,
	EVar_Mission_PartyMember1_DieScore = 21,
	EVar_Mission_PartyMember2_DieScore = 22,
	EVar_Mission_PartyMember3_DieScore = 23,
	EVar_Mission_PartyMember4_DieScore = 24,
	EVar_Mission_OverHitScore = 25,
	EVar_Mission_BackAttack = 26,
	EVar_Mission_Counter = 27,
	EVar_Mission_Style = 28,
	EVar_Mission_ThrowUpPenaltyCount = 29,
	EVar_Mission_ThrowUpPenaltyTime = 30,
	EVar_Mission_TimeLimitParty1 = 31,
	EVar_Mission_TimeLimitParty2 = 32,
	EVar_Mission_TimeLimitParty3 = 33,
	EVar_Mission_TimeLimitParty4 = 34,
	EVar_Mission_DefenceObjectAttackMonCount = 35,
	EVar_Mission_DefenceChangeTargetRate = 36,
	EVar_Mission_Defence7ObjectAttackMonCount = 37,
	EVar_Mission_Defence7ChangeTargetRate = 38,
	EVar_Mission_EscortMonSpawnDelay = 39,
	EVar_Mission_Max,
}EVar_Mission;

typedef enum eVar_Log
{
	EVar_LogOutputType	= 1,	//출력 타입(파일에 쓸건가, 화면에 남길건가 등 E_OUPUT_TYPE)
	EVar_LogLevelLimit	= 2,	//로그 레벨 제한
	EVar_EachMap_PlayTimeLog_Interval_Min = 4
}EVar_Log;

typedef enum eVar_ExpCard
{
	EVar_ExpCard_Enable_Use_Num = 1,// 하루에 사용할 수 있는 경험치 카드 수량
}EVar_ExpCard;

typedef enum eVar_OpenMarket
{
	EVar_OpenMarket_Enable_Reg_CashCost = 1,// 오픈마켓에서 Cash cost를 등록할 수 있는지 여부
	EVar_OpenMarket_NormalMarketOfflineTime = 2,// 일반상점으로 상점오픈시 오프라인 시간
}EVar_OpenMarket;

typedef enum eVar_Pet
{
	EVar_Pet_OffGroundAttr = 2,			// 펫이 사용되지 않는 그라운드 속성
	EVar_Pet_TakePCExpRate = 3,			//PC로 부터 받는 경험치의 만분율
}EVar_Pet;

typedef enum eVar_GM_Emoticon
{
	EVar_GM_Emoticon_Default = 1,
}EVar_GM_Emoticon;

typedef enum eVar_Premium_Service
{
	EVar_VIP_Emoticon = 1,
}EVar_Premium_Service;

typedef enum eVar_Enchant
{
	EVar_Downgrade_Rate = 1,
}EVar_Enchant;

typedef enum eVar_MyHome
{
	EVar_MyHome_AuctionLimitGold = 1,
	EVar_MyHome_Apt_Gold = 2,
	EVar_MyHome_Apt_LevelLimit = 3,
}EVar_MyHome;

typedef enum eVar_EntityExtInfo
{
	EVar_EntityExtInfo_EnableCriticalAttack	= 1,
	EVar_EntityExtInfo_EnableHitRate		= 2,
	EVar_EntityExtInfo_EnableAttackExtInfo	= 3,
}EVar_EntityExtInfo;

typedef enum eVar_Hidden
{
	EVar_Hidden_Open_Min	= 1,
	EVar_Hidden_Stone_Item	= 2,
}EVar_Hidden;

typedef enum eVar_EachClassAwakeValueReviser
{
	eVar_Knight_Reviser	= 1,
	eVar_Warrior_Reviser = 2,
	eVar_Mage_Reviser = 3,
	eVar_BattleMage_Reviser = 4,
	eVar_Hunter_Reviser = 5,
	eVar_Ranger_Reviser = 6,
	eVar_Clown_Reviser = 7,
	eVar_Assassin_Reviser = 8,
	eVar_Shaman_Reviser = 9,
	eVar_DoubleFighter_Reviser = 10,

}EVar_EachClassAwakeValueReviser;

extern int iCanAddExpMinLevel;
extern int iCanAddExpMaxLevel;
extern int g_iMyHomeAuctionLimitGold;

typedef enum eVar_JobSkillPenalty
{
	JobPenalty_BaseExp_Diff_1 = 30,
	JobPenalty_BaseExp_Diff_2 = 31,
	JobPenalty_BaseExp_Diff_3 = 32,
	JobPenalty_BaseExp_Diff_4 = 33,
}EVar_JobSkillPenalty;

typedef enum eVar_DualKeyboardHack
{
	eVar_DualKeyboardHack_TIME_MSEC		= 1,
	eVar_DualKeyboardHack_MATCH_RATE	= 2,
}EVar_DualKeyboardHack;


// AnitHack System 관련 정보 구조체

typedef struct tagAntiHackCheckActionCount
{
	bool	m_bUseAntiHackCheckActionCount;		// 액션카운트 시스템 사용 여부	

	int		m_iActionCount_CheckTime;			// 체크 시간
	int		m_iActionCount_CheckTime_Mutiflier;	// 체크 시간에 보정치
	int		m_iCheckCountSum;					// 체크할 카운트 수
	bool	m_bForcedResultView;				// 해킹에 상관없이 결과를 출력
	bool	m_bResultToPlayer;					// 결과를 Player에게 보낼지 여부(debug용)

	bool	m_bHackingIndex;					// 해킹 인덱스를 사용할 것인가?
	int		m_iAddHackingPoint;					// 증가할 해킹 점수

}AntiHackCheckActionCount;

typedef struct
{
	// EAHP_CheckGameTimeOver
	bool bUse_GameTimeOver;
	short sGameTimeOver_AddIndex;
	// EAHP_CheckGameTimeUnder
	bool bUse_GameTimeUnder;
	short sGameTimeUnder_AddIndex;
	// EAHP_MaxTarget
	bool bUse_MaxTarget;
	short sMaxTarget_AddIndex;
	// EAHP_SkillRange
	bool bUse_SkillRange;
	short sSkillRange_AddIndex;
	// EAHP_MoveSpeed
	bool bUse_MoveSpeed;
	short sMoveSpeed_AddIndex;
	float fMoveSpeed_BoundRate;
	short sMoveSpeed_HackingCount;
	// EAHP_Projectile
	bool bUse_Projectile;
	short sProjectile_AddIndex;
	// EAHP_OpenMarket
	bool bUse_OpenMarket;
	short sOpenMarket_AddIndex;
	// EAHP_CashShop
	bool bUse_CashShop;
	short sCashShop_AddIndex;
	// EAHP_NPCRange
	bool bUse_NPCRange;
	short sNPCRange_AddIndex;
	// EAHP_ClientNotify
	bool bUse_ClientNotify;
	short sClientNotify_AddIndex;
	// EAHP_AntiMacro
	bool bUse_AntiMacro;
	short sAntiMacro_AddIndex;
	// EAHP_HyperMove
	bool bUse_HyperMove;
	short sHyperMove_WrongGuid_AddIndex;
	short sHyperMove_WrongDistance_AddIndex;
	// EAHP_Blowup
	bool bUse_Blowup;
	short sBlowup_AddIndex;
	// EAHP_RandomSeedCallCounter
	bool bUse_RandomSeedCallCounter;
	short sRandomSeedCallCounter_AddHackIndex;
	int iRandomSeedCallCounter_CheckValue;


} AntiHackCheckVariable;

typedef struct tagMapPlayTime
{
	tagMapPlayTime()
	{
		Clear();
	}

	tagMapPlayTime(tagMapPlayTime const& rhs)
	{
		*this = rhs;
	}

	tagMapPlayTime const& operator=(tagMapPlayTime const& rhs)
	{
		iInCount = rhs.iInCount;
		iOutCount = rhs.iOutCount;
		iPlayTime = rhs.iPlayTime;
		return (*this);
	}

	void Clear()
	{
		iInCount = iOutCount = 0;
		iPlayTime = 0;
	}

	void operator+=(tagMapPlayTime const& rhs)
	{
		iInCount += rhs.iInCount;
		iOutCount += rhs.iOutCount;
		iPlayTime += rhs.iPlayTime;
	}

	bool IsEmpty() const
	{
		if (iInCount != 0 || iOutCount != 0 || iPlayTime != 0)
		{
			return false;
		}
		return true;
	}

	int iInCount;	// 들어온 유저
	int iOutCount;	// 나간 유저
	__int64 iPlayTime;	// playtime 합계
} SMapPlayTime;

#endif // MAP_MAPSERVER_FRAMEWORK_CONSTANT_H