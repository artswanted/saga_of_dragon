#ifndef CONTENTS_CONTENTSSERVER_MAINFRAME_CONSTANT_H
#define CONTENTS_CONTENTSSERVER_MAINFRAME_CONSTANT_H

#include "Doc/PgDoc_Player.h"

#pragma pack(1)

typedef struct tagCenterPlayerData//! 센터서버에 등록되는 유저 정보
{
	tagCenterPlayerData()
	{
		Clear();
	}

	void Clear()
	{
		guidMember.Clear();
		guidCharacter.Clear();

		ucGender = 0;
		kSwitchServer.Clear();
		
		kID = L"";
		addrRemote.Clear();
		kConnArea.Clear();

//		vtPlayer.clear();
		pkPlayer = NULL;
		pkBackupPlayer = NULL;

		i64TotalConnSec = 0;
		iAccConSec = 0;
		iAccDisSec = 0;
		sChannelNo = 0;
		usAge = 0;
		byGMLevel = 0;
		bIsPCCafe = false;
		byPCCafeGrade = 0;
		
		::memset( gusID, 0, sizeof(gusID) );
		iUID = 0;
		iEventRewardKey = 0;

		dtUserBirth.Clear();
	}

	tagCenterPlayerData( tagSwitchPlayerData const &rhs )
	{
		Clear();
		kID = rhs.ID();
		guidMember = rhs.guidMember;
		ucGender = rhs.ucGender;
		kSwitchServer = rhs.kSwitchServer;
		addrRemote = rhs.addrRemote;
		kConnArea = rhs.m_kConnArea;
		i64TotalConnSec = rhs.i64TotalConnSec;
		dtLastLogin = rhs.dtLastLogin;
		dtLastLogout = rhs.dtLastLogout;
		dtNowDbTime = rhs.dtNowDbTime;
		i64TotalConnSec = rhs.i64TotalConnSec;
		iAccConSec = rhs.iAccConnSec;
		iAccDisSec = rhs.iAccDisConnSec;
		sChannelNo = rhs.ChannelNo();
		usAge = rhs.usAge;
		byGMLevel = rhs.byGMLevel;
		bIsPCCafe = rhs.bIsPCCafe;
		byPCCafeGrade = rhs.byPCCafeGrade;
		dtUserBirth = rhs.dtUserBirth;

		::memcpy( gusID, rhs.gusID, sizeof(gusID) );
		iUID = rhs.iUID;
		iEventRewardKey = rhs.iEventRewardKey;
	}
	
	Loki::Mutex kMutex;

	BM::GUID guidMember;	//member의 guid
	unsigned char ucGender; //암수 구분
	SERVER_IDENTITY kSwitchServer;	//사용중인 switch의 번호

	std::wstring kID;			//시도ID
	CEL::ADDR_INFO addrRemote;	//유저의주소.
	SConnectionArea kConnArea;	//접속 정보

	BM::GUID guidCharacter;	//선택된 캐릭터의의 guid

	PgDoc_Player *pkPlayer;
	PgDoc_Player *pkBackupPlayer;
	short sChannelNo;

	unsigned short	usAge;
	__int64 i64TotalConnSec;
	int		iAccConSec;
	int		iAccDisSec;

	BM::DBTIMESTAMP_EX dtLastLogin;
	BM::DBTIMESTAMP_EX dtLastLogout;
	BM::DBTIMESTAMP_EX dtNowDbTime;
	BM::DBTIMESTAMP_EX dtUserBirth;

	BM::GUID kGetListQueryGuid;// 쿼리 진행중인지 구분.
	BM::GUID kSelectQueryGuid;// 퀴리QUID (SelectCharacter, CreateCharacter, DeleteCharacter)

	BYTE	byGMLevel;
	bool	bIsPCCafe; // 가맹 PC방 접속?
	BYTE byPCCafeGrade;

	// NC만 사용하는 것
	BYTE gusID[16];
	int iUID;
	
	int iEventRewardKey;

	DWORD dwLogoutTime;

	bool Copy(SContentsUser &rkOut) const
	{
		if( !pkPlayer )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
		rkOut.kMemGuid = guidMember;
		rkOut.kCharGuid = pkPlayer->GetID();
		rkOut.kName = pkPlayer->Name();
		rkOut.sLevel = pkPlayer->GetAbil(AT_LEVEL);
		rkOut.iClass = pkPlayer->GetAbil(AT_CLASS);
		rkOut.cGender = pkPlayer->GetAbil(AT_GENDER);
		rkOut.kGndKey = pkPlayer->GroundKey();
		rkOut.kGuildGuid = pkPlayer->GuildGuid();
		rkOut.kCoupleGuid = pkPlayer->CoupleGuid();
		rkOut.sChannel = sChannelNo;
		rkOut.kAccountID = pkPlayer->MemberID(); // 계정명
		rkOut.iUID = iUID;
		rkOut.kHomeAddr = pkPlayer->HomeAddr();
		pkPlayer->GetDateContents(DCT_GUILD_LEAVE_DATE, rkOut.kGuildLeaveDate);
		return true;
	}

	bool Copy(SSwitchPlayerData& rkOut) const
	{
		rkOut.Clear();
		// SClientTryLogin
		rkOut.szVersion1[0] = _T('\0');	// Unknown value
		rkOut.szVersion2[0] = _T('\0');	// Unknown value
		rkOut.ID(kID);
		rkOut.PW(L"");	// Unknown value
//		rkOut.szName[0] = _T('\0');	// Unknown value
		rkOut.RealmNo(0);	// Unknown value
		rkOut.ChannelNo(sChannelNo);	// Unknown value
		// SServerTryLogin
		//rkOut.SessionKey();	// Unknown value
		rkOut.addrRemote = addrRemote;
		rkOut.m_kConnArea = kConnArea;
		rkOut.kLoginServer.Clear();	// Unknown value
		rkOut.ulExpireTime = 0;	// Unknown value
		// SReqSwitchReserveMember
		rkOut.guidMember = guidMember;
		rkOut.guidOrderKey = BM::GUID::NullData();	// Unknown value
		rkOut.ucGender = ucGender;
		rkOut.usAge = usAge;	// Unknown value
		rkOut.i64TotalConnSec = i64TotalConnSec;
		rkOut.iAccDisConnSec = iAccDisSec;
		rkOut.iAccConnSec = iAccConSec;
		rkOut.dtLastLogin = dtLastLogin;
		rkOut.dtLastLogout = dtLastLogout;
		rkOut.dtNowDbTime = dtNowDbTime;
		rkOut.byBlock = 0;	// Unknown value
		rkOut.byGMLevel = byGMLevel;
		rkOut.bIsPCCafe = bIsPCCafe;
		rkOut.byPCCafeGrade = byPCCafeGrade;
		//SSwitchPlayerData
		rkOut.dwTimeLimit = 0;
		rkOut.dtUserBirth = dtUserBirth;

		::memcpy( rkOut.gusID, gusID, sizeof(rkOut.gusID) );
		rkOut.iUID = iUID;
		rkOut.iEventRewardKey = iEventRewardKey;
		return true;
	}

	bool Copy(SUserInfoExt& rkOut) const
	{
		rkOut.kAddress = addrRemote;
		rkOut.iPremiumNo = pkPlayer->GetPremium().GetServiceNo();
		return true;
	}

	short ChannelNo() const
	{
		return sChannelNo;
	}

	void SetChannel( short sChannelNo )
	{
		sChannelNo = sChannelNo;
	}

}SCenterPlayerData;

typedef struct tagSCenterPlayer_Lock
{
	tagSCenterPlayer_Lock()
	{
		pkCenterPlayerData = NULL;
	}

	~tagSCenterPlayer_Lock()
	{
		if (pkCenterPlayerData != NULL)
		{
			pkCenterPlayerData->kMutex.Unlock();
		}
	}

	tagSCenterPlayer_Lock(const tagSCenterPlayer_Lock& rhs)
	{
		pkCenterPlayerData = NULL;
		Set(rhs.pkCenterPlayerData);
	}

	void operator = (const tagSCenterPlayer_Lock& rhs)
	{
		Set(rhs.pkCenterPlayerData);
	}

	void Set(SCenterPlayerData* pkInfo)
	{
		if (pkCenterPlayerData != NULL)
		{
			pkCenterPlayerData->kMutex.Unlock();
		}
		pkCenterPlayerData = pkInfo;
		pkCenterPlayerData->kMutex.Lock();
	}

	void Set(SCenterPlayerData const * pkInfo)
	{
		if (pkCenterPlayerData != NULL)
		{
			pkCenterPlayerData->kMutex.Unlock();
		}
		pkCenterPlayerData = (SCenterPlayerData*) pkInfo;
		pkCenterPlayerData->kMutex.Lock();
	}

	SCenterPlayerData* pkCenterPlayerData;
} SCenterPlayer_Lock;

typedef enum
{
	ECEvent_None = 0,
	ECEvent_Kick_byGM = 2,
	ECEvent_PT_I_T_KICKUSER = 3,
	ECEvent_PT_T_N_REQ_MAP_MOVE = 5,
	ECEvent_SendToUser = 6,
} EContentsEvent;


typedef struct tagRealmGroundKey
{
	tagRealmGroundKey()
	{
		Clear();
	};

	tagRealmGroundKey(short const sChannel, SGroundKey const &rkGroundKey)
	{
		Channel(sChannel);
		GroundKey(rkGroundKey);
	};

	void Clear()
	{
		m_kChannel = 0;
		m_kGroundKey.Clear();
	}

	void Set(short const sChannel, SGroundKey const &rkGroundKey)
	{
		Channel(sChannel);
		GroundKey(rkGroundKey);
	}

	bool operator < (const tagRealmGroundKey& rhs) const
	{	
		if (Channel() < rhs.Channel()) { return true; }
		if (rhs.Channel() < Channel()) { return false; }

		if(GroundKey() < rhs.GroundKey()){ return true; }
		if(rhs.GroundKey() < GroundKey() ){ return false; }

		return false;
	}

	bool operator == (const tagRealmGroundKey& rhs) const
	{
		if (Channel() == rhs.Channel()
			&& GroundKey() == rhs.GroundKey())
		{
			return true;
		}
		return false;
	}

	bool IsEmpty()const
	{
		return GroundKey().GroundNo() == 0;
	}
/*
	operator size_t ()const
	{
		return Channel() ^ (size_t)GroundKey();;
	}
*/
	void ReadFromPacket(BM::Stream& rkPacket)
	{
		rkPacket.Pop(m_kChannel);
		m_kGroundKey.ReadFromPacket(rkPacket);
	}
	void WriteToPacket(BM::Stream& rkPacket) const
	{
		rkPacket.Push(Channel());
		GroundKey().WriteToPacket(rkPacket);
	}

	std::wstring str() const
	{
		TCHAR chTemp[200];
		_stprintf_s(chTemp, 200, _T("Channel:%d, GroundNo:%d, Guid:%s"), Channel(), GroundKey().GroundNo(), GroundKey().Guid().str().c_str());
		return std::wstring(chTemp);
	}

	CLASS_DECLARATION_S(short, Channel);
	CLASS_DECLARATION_S(SGroundKey, GroundKey);
} SRealmGroundKey;

SRealmGroundKey const Static_Default_RealmGroundKey(PUBLIC_CHANNEL_NUM, SGroundKey() );

typedef struct tagContentsActionEvent//	IMET_MODIFY_COUNT	=201
{//지정된 
	tagContentsActionEvent() {};
	tagContentsActionEvent(EContentsEvent eType)
	{
		Type(eType);
	}

	CLASS_DECLARATION_S(EContentsEvent, Type);//대상 아이템

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(Type());
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kType);
	}
}ContentsActionEvent;

typedef struct tagContentsActionEvent_SendPacket : public tagContentsActionEvent
{
	typedef enum : BYTE
	{
		E_SendUser_None = 0,
		E_SendUser_ToOneUser = 1,
	} E_SendUser_Type;

	tagContentsActionEvent_SendPacket() {};
	tagContentsActionEvent_SendPacket(EContentsEvent eType)
		: tagContentsActionEvent(eType)
	{
		SendType(E_SendUser_ToOneUser);
	}

	CLASS_DECLARATION_S(bool, MemberGuid);
	CLASS_DECLARATION_S(BM::GUID, Guid);
	CLASS_DECLARATION_S(E_SendUser_Type, SendType);
	mutable BM::Stream m_kPacket;

	BM::Stream const & Packet() { return m_kPacket; }

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(MemberGuid());
		kPacket.Push(Guid());
		kPacket.Push(SendType());
		m_kPacket.PosAdjust();
		kPacket.Push(m_kPacket.Data());
		tagContentsActionEvent::WriteToPacket(kPacket);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(m_kMemberGuid);
		kPacket.Pop(m_kGuid);
		kPacket.Pop(m_kSendType);
		m_kPacket.Clear();
		kPacket.Pop(m_kPacket.Data());
		m_kPacket.PosAdjust();
		tagContentsActionEvent::ReadFromPacket(kPacket);
	}
} ContentsActionEvent_SendPacket;

//
typedef enum eVar_Kind
{
	EVar_Kind_None = 0,
	EVar_Kind_DefaultMap = 1,
	EVar_Kind_HalfDelay = 2,
	EVar_Kind_DBManager = 3,
	EVar_Kind_Contents = 4,
	EVar_Kind_AntiHack = 5,
	EVar_Kind_Log = 6,
	EVar_Kind_Emporia = 7,
	EVar_Kind_Guild = 8,
	EVar_Kind_ExpBuff_Logoff = 9,
	EVar_Kind_OpenMarket = 10,
	Evar_Kind_CashShop = 11,
	EVar_Kind_Item = 12,
	EVar_Kind_PvPLeague = 13,
	EVar_Kind_SpecificReward = 14,
	EVar_Kind_StrategyFatigability = 15,
	EVar_Kind_LoginEventSpecific = 16,
	EVar_Kind_Community_Event_Time = 17,
} EVar_Kind;


//
typedef enum eVar_DefaultMap
{
	EVAR_TUTORIAL_FIGHTER		= 1,	// 전사 튜토리얼
	EVAR_TUTORIAL_MAGICIAN		= 2,	// 법사 튜토리얼
	EVAR_TUTORIAL_ARCHER		= 3,	// 궁수 튜토리얼
	EVAR_TUTORIAL_THIEF			= 4,	// 도적 튜토리얼	
	EVAR_STARTVILLAGE_FIGHTER	= 5,	// 전사 시작 마을
	EVAR_STARTVILLAGE_MAGICIAN	= 6,	// 법사 시작 마을
	EVAR_STARTVILLAGE_ARCHER	= 7,	// 궁수 시작 마을
	EVAR_STARTVILLAGE_THIEF		= 8,	// 도적 시작 마을
	EVAR_EMERGENCYMAP			= 9,	// 이동할 맵번호가 0일 때 이동할 맵
	EVAR_STARTPORTAL_FIGHTER	= 10,	// 전사 시작 마을 포털 번호
	EVAR_STARTPORTAL_MAGICIAN	= 11,	// 법사 시작 마을 포털 번호
	EVAR_STARTPORTAL_ARCHER		= 12,	// 궁수 시작 마을 포털 번호
	EVAR_STARTPORTAL_THIEF		= 13,	// 도적 시작 마을 포털 번호
	EVAR_STARTVILLAGE_SUMMONER	= 14,	// 소환사 시작 마을
	EVAR_STARTVILLAGE_TWINS		= 15,	// 쌍둥이 시작 마을
	EVAR_STARTPORTAL_SUMMONER	= 16,	// 소환사 시작 마을 포털 번호
	EVAR_STARTPORTAL_TWINS		= 17,	// 쌍둥이 시작 마을 포털 번호	
} EVar_DefaultMap;

typedef enum eVar_HalfDelay
{
	EVar_ExpSaveCycle = 1,					// 경험치 세이브 주기
	EVar_FranSaveCycle = 2,					// FRAN 세이브 주기
	EVar_MoneySaveCycle = 3,				// 돈 세이브 주기
} EVar_HalfDelay;

typedef enum eVar_DBManager
{
	EVar_DB_CleanDeleteCharacter_DelayDay = 1,
	EVar_DB_Eable_CreateCharacter = 2,
	EVar_DB_MinCharacterName = 3,
	EVar_DB_MaxCharacterName = 4,
	EVar_DB_ChangeDeletedCharacterName_DelayHour = 5,
	EVar_DB_MaxCreateCharacterNum = 6,			// 기본 최대 생성 가능한 캐릭터 개수
	EVar_DB_UseCash_100 = 7,
	EVar_DB_UseCash_1000 = 8,
	EVar_DB_Mileage_Cash_100 = 9,
	EVar_DB_Mileage_Cash_1000 = 10,
	EVar_DB_MaxExtendCreateCharacterNum = 20,	// 확장 가능한 캐릭터 생성 가능 개수
	EVar_DB_LogCCU_CycleMin	= 30,				// 동접로그를 남기는 주기(분)
} EVar_DBManager;

typedef enum eVar_Contents
{
	EVar_CoupleUse						= 1,	// 커플 사용유무 0/1
	EVar_Maximum_Level					= 3,	// player의 최대 level
	EVar_CoupleSweetHeartPanaltyDay		= 4,	// 연인 헤어지고 페널티 시간(날짜)

	EVar_UM_UseCash						= 5,	// 오픈마켓 캐시 사용 가능 여부
	EVar_CS_EanbleGiftLevel				= 6,	// 캐시샵에서 선물 가능한 캐릭터 레벨

	EVar_MyHome_Sidejob_Enter_Cost		= 7,	// 아르바이트 마이홈 진입 비용
	EVar_UseLevelRank					= 8,	// 제2케릭터생성유도 사용 가능 여부
	EVar_Item_NationCode				= 9,	// 아이템 NationCode 설정(개발용도)
	EVar_JobSkill_ExhaustionGap			= 10,	// 채집 피로도 회복 시간 (분)
	EVar_JobSkill_ExhaustionRestore		= 11,	// 채집 피로도 Gap당 회복양(Max / Setting)
	EVar_JobSkill_BlessRate				= 12,	// 마이홈 작업대 축복으로 인한 시간 가속율
	EVar_JobSkill_BlessRate_DurationSec	= 13,	// 마이홈 작업대 축복으로 지속 시간

	EVar_IndunParty_List_CasingTime		= 14,	// 인던 파티 리스트 리플레시 타임
	EVar_IndunParty_List_DelayTime		= 15,	// 인던 파티 리스트 센터서버로 부터 얼마나 기다릴지 정의
	EVar_Char_MinLvToCreateDrakan 		= 14,   // If player have gte char of this level he can create a drakan
	EVar_Char_CreateDrakanItemNo 		= 15,   // Create Drakan item no
} EVar_Contents;

typedef enum eVar_Emporia
{
	EVar_ChallengeTournament_Final_AfterBasicMinTime				= 1,
	EVar_ChallengeTournament_1stOfSemiFinal_AfterBasicMinTime		= 2,
	EVar_ChallengeTournament_2ndOfSemiFinal_AfterBasicMinTime		= 3,
	EVar_ChallengeTournament_1stOfQuarterFinal_AfterBasicMinTime	= 4,
	EVar_ChallengeTournament_2ndOfQuarterFinal_AfterBasicMinTime	= 5,
	EVar_ChallengeTournament_3rdOfQuarterFinal_AfterBasicMinTime	= 6,
	EVar_ChallengeTournament_4stOfQuarterFinal_AfterBasicMinTime	= 7,
	
	EVar_EmporiaTournament_1st_AfterBasicMinTime					= 11,
	EVar_EmporiaTournament_2nd_AfterBasicMinTime					= 12,
	EVar_EmporiaTournament_3rd_AfterBasicMinTime					= 13,
	EVar_EmporiaTournament_4st_AfterBasicMinTime					= 14,
	EVar_EmporiaTournament_5st_AfterBasicMinTime					= 15,

	EVar_EmporiaBattleReadyMinTime									= 30,//엠포리아 쟁탈전 준비 시간
//	EVar_BeginChallenge_BeforeBasicMinTime							= 30,
//	EVar_EndChallenge_BeforeBasicMinTime							= 31,

	EVar_EmporiaBattle_GameTime_DestroyCore = WAR_TYPE_DESTROYCORE,//엠포리아 전쟁 코어파괴전 기본  게임시간
	EVar_EmporiaBattle_GameTime_ProtectedDragon = WAR_TYPE_PROTECTDRAGON,//엠포리아 전쟁 드래곤방어전 기본 게임시간
} EVar_Emporia;

typedef enum eVar_Guild
{
	EVar_CAN_REJOIN_GUILD_HOUR			= 1,		// 길드 탈퇴 이후 다시 가입할 수 있는 시간 (예: 탈퇴 이후 몇시간(n시간) 지나야 재 가입 가능하다
	EVar_Send_LoginUser_ChatLine		= 2,		// 로그인 유저에게 보내는 길드채팅 남기는 라인수
} EVar_GUILD;

typedef enum eVar_Log
{
	EVar_LogOutputType	= 1,	//출력 타입(파일에 쓸건가, 화면에 남길건가 등 E_OUPUT_TYPE)
	EVar_LogLevelLimit	= 2,	//로그 레벨 제한
}EVar_Log;

typedef enum
{
	//얻을 수 있는 최대 휴식 경험치 값 (현재 레벨의 경험치에 대한 %값)
	EVar_ExpAdd_MaxExperienceRate = 1,
	// 휴식경험치에 의해 얻게 되는 사냥  경험치 증가 % (50% 증가이면 50 으로 입력)
	EVar_ExpAdd_AddedExp_Rate_Hunting = 2,
} EVAR_EXPADD_LOGOFF;

typedef enum eVar_OpenMarket
{
	EVar_OpenMarket_OpenMarketDealingCount = 1,// 오픈마켓에서 회수하지 않은 목록의 갯수 제한(제한된 개수가 넘으면 상점 등록 불가능)	
}EVar_OpenMarket;

typedef enum eVar_CashShop
{
	EVar_CashShop_Open = 1,
}EVar_CashShop;

typedef enum eVar_Item
{
	EVar_Item_CountLogMin = 1,
}EVar_Item;

typedef enum eVar_PvPLeague
{
	EVar_PvPLeague_MaxTeamCount = 1,
}EVar_PvPLeague;

typedef enum eVar_SpecificReward
{
	EVar_Revive_Feather_Login_Time	= 1, // 1일1회 보급상자 지급( 로그인 몇분 유지시 지급할 것인가? 0 이면 해당 기능 사용 안함 )
	EVar_Revive_Feather_LevelUp_Use = 2,
}EVar_SpecificReward;

typedef enum eVar_StrategyFatigability
{
	EVar_Max_StrategyFatigability	= 1, // 전략피로도 최대치.
	EVar_Init_Time_HOUR				= 2,
	EVar_Init_Time_MIN				= 3,
	EVar_Init_Time_SEC				= 4,
	EVar_Dec_Per_Stage_1			= 5,
	EVar_Dec_Per_Stage_2			= 6,
	EVar_Dec_Per_Stage_3			= 7,
	EVar_Dec_Per_Stage_4			= 8,
	EVar_Bonus_Exp_Rate				= 9,
	EVar_Fatigability_Div_Value		= 10,
} EVar_StrategyFatigability;

typedef enum eVar_SpecificRewardNotifyMessageInterval
{
	EVar_SpecificRewardNotifyMessageInterval = 1,
}EVar_SpecificRewardNotifyMessage;

typedef enum eVar_CommunityEventControlTime
{
	eVar_StartWaitTime	= 1,
	eVar_ReadyTime		= 2,
}EVar_CommunityEventControlTime;

namespace RealmUserManagerUtil
{
	struct SPassTutorialInfo
	{
		explicit SPassTutorialInfo()
			: iGroundNo(0), sPortalNo(0)
		{
		}
		explicit SPassTutorialInfo(int const iDefaultGroundNo)
			: iGroundNo(iDefaultGroundNo), sPortalNo(1) // 기본 포털은 1번
		{
		}

		int iGroundNo;
		short sPortalNo;
	};

	bool InitConstantValue();
	void ProcessKickUser( int const iCase, BM::GUID const &kReqGuid, SContentsUser const &kContUser );
	void ProcessAddWorldMap( BM::GUID const &kCharGuid, short const nChannelNo, int const iGroundNo, SGroundKey const &kCasterGndKey );
}

typedef enum
{
	// 삭제 정책
	ECHAR_DEL_NAME_CHANGED_TO_DUMMY = 0,	// Character Name 을 의미없는 값으로 변경
	ECHAR_DEL_REMOVE_IMMEDIATELY = 1,	// Database에서 바로 삭제 하기
	ECHAR_DEL_NAME_UNCHANGED = 2,	// Character Name 그대로 유지하기
	// 생성 정책
	ECHAR_CREATE_NOT_CHECK_DEL_CHAR = 0,	// 캐릭터 이름 중복 검사 : 삭제된 캐릭터 이름은 제외
	ECHAR_CREATE_CHECK_ALL_NAME = 2,	// 캐릭터 이름 중복 검사 : 삭제된 캐릭터이름도 검사
} ECHARACTER_DELETE_POLICY;

int const GOLD2BRONZE = 10000;

#pragma pack()

#endif // CONTENTS_CONTENTSSERVER_MAINFRAME_CONSTANT_H