#ifndef WEAPON_VARIANT_QUEST_PGQUESTINFO_H
#define WEAPON_VARIANT_QUEST_PGQUESTINFO_H

//
size_t const MAX_QUEST_REWARD_ITEM	= 5;

class PgMyQuest;
class PgPlayer;
namespace RandomQuest
{
	int const iRandomQuestMinLimitLevel = 24;
	int const iRandomTacticsQuestMinLimitLevel = 20;
	bool IsCanRebuild(PgMyQuest const* pkMyQuest, int const iQuestType);
	bool GetRebuildItem(PgPlayer* pkPlayer, int const iQuestType, int const iUICType, PgBase_Item& rkOutItem, SItemPos& rkOutPos);
	bool IsHasRebuildItem(PgPlayer* pkPlayer, int const iQuestType, int const iUICType);
}

// Quest Begin Limit type (순서 변경 하지 말것)
typedef enum eQuestBeginLimit
{
	QBL_None			= 0,
	QBL_Item			= 1,	//시작 아이템 제한
	QBL_Quest			= 2,	//이전 완료 퀘스트 제한
	QBL_Level			= 3,	//레벨 제한
	QBL_Class			= 4,	//직업 제한
	QBL_Time			= 5,	//수락 가능 시간 제한
	QBL_Location		= 6,	//이전 가봤던 장소 제한
	QBL_Party			= 7,	//파티원 수 제한
	QBL_Count			= 8,	//최대 수행 가능한 퀘스트 제한
	QBL_Group			= 10,	//그룹 제한
	QBL_Anyway			= 11,	//뭐지...
	QBL_Error			= 12,	//에러
	QBL_Begined			= 13,	//이미 시작한 퀘스트
	QBL_Ended			= 14,	//이미 완료한 퀘스트
	QBL_Level_One		= 15,	//레벨 제한이나, 1레벨 업 하면 수행 가능
	QBL_Inven			= 16,
	QBL_Couple			= 17,
	QBL_PetLevel		= 18,	// 펫제한에 걸렸어
	QBL_Guild			= 19,	// 길드 가입 해야되
	QBL_MyHome			= 20,	// 마이홈 소유 해야되
	QBL_PetType			= 21,	// 펫타입 제한에 걸렸어 (EPET_TYPE_1, ...)
	QBL_PetClassGrade	= 22,	// 펫전직 제한 (전직 레벨)에 걸렸어
	QBL_SkillHave		= 23,	// 스킬 보유 제한
	QBL_PetCashType		= 24,	// 펫캐쉬 타입 제한

	// 예외 용
	QBL_Max				= 101,	//퀘스트 공간이 없다
	QBL_NowTalking		= 102,	//퀘스트 대화 중이다
	QBL_NowEventScript	= 103,	//이벤트 스크립트이다
	QBL_Reject			= 104,	//거절했다
	QBL_State			= 105,	//상태가 안되
	QBL_HaveCoupleQuest	= 106,	//이미 커플 퀘스트 다른걸 가지고 있음
}EQuestBeginLimit;

//
enum EQuestItemLimitOperator
{
	QILO_NONE			= 0,
	QILO_GREATEQUAL		= 1,
	QILO_EQUAL			= 2,
	QILO_LESSEQUAL		= 3,
};

typedef enum eQuestRemoteCompleteMode
{
	QRCM_NOT_USE		= 0,
	QRCM_USE			= 1,
	QRCM_FORCE_USE		= 2,
} EQuestRemoteCompleteMode;

//
class PgMyQuest;
class PgQuestInfo;
class PgPlayer;

//
typedef std::vector< int > ContLocationID;

namespace EventTaiwanEffectQuest
{
	extern int const iNotHaveQuestMsg;
	extern int const iCantDupRewardEffectMsg;
	extern int const iCantDupExpPotionMsg;
	extern int const iEventEffectNo;
	int GetEnableCount(PgMyQuest const* pkMyQuest);
	int GetEnableQuestID(PgMyQuest const* pkMyQuest);
};

//
class PgMyQuest;
struct tagQuestDependAbil;
namespace PgQuestInfoUtil
{
	int GetHaveCoupleQuestID(PgMyQuest const* pkMyQuest);
	int GetQuestDefType(int const iQuestID);
	int GetQuestDefType(int const iQuestID, CONT_DEF_QUEST_REWARD const* pkDefReward);
	bool IsDailyUIQuest(PgQuestInfo const* pkQuestInfo);
	bool IsDailySystemQuest(PgQuestInfo const* pkQuestInfo);
	bool IsWeeklySystemQuest(PgQuestInfo const* pkQuestInfo);
	int GetQuestRewardExp(PgQuestInfo const* pkQuestInfo, PgMyQuest const* pkMyQuest, int const iAddCompleteCount = 0);
	int GetQuestRewardGuildExp(PgQuestInfo const* pkQuestInfo, PgMyQuest const* pkMyQuest, int const iAddCompleteCount = 0);

	EQuestBeginLimit CheckBeginQuest(PgPlayer const* pkPlayer, int const iQuestID, PgQuestInfo const* pkQuestInfo);

	void MakeDayLoopQuestTime(BM::PgPackedTime &rkFailPrevTime, BM::PgPackedTime &rkNextTime);
	void MakeWeekLoopQuestTime(BM::PgPackedTime &rkFailPrevTime, BM::PgPackedTime &rkNextTime);
	int GetDaysinMonth(int const Year, int const Month);
	bool NeedRefreshDayQuest(PgMyQuest const& rkMyQuest, BM::PgPackedTime const &rkFailPrevTime, BM::PgPackedTime const &rkNextTime);

	bool VerifyIndex(int const iObjectNo); // 
	bool CheckIndex(int const iObjectNo); //

	bool IsLoadXmlType(int const iQuestType);
	bool IsCompleteQuestOrder(int const iOrderCause);

	class PgTextReplacer
	{
	private:
		typedef std::vector< std::wstring > ContStr;

	public:
		// USEAGE
		//static std::wstring Parse(std::wstring const& rkOrgString)
		//{
		//	PgTextReplacer kReplacer(rkOrgString);
		//	return kReplacer.m_kResult;
		//}

	protected:
		PgTextReplacer(std::wstring const& rkOrgText);
		virtual ~PgTextReplacer();

		void Parse();
	private:
		virtual void AddParsedError(std::wstring const& , std::wstring const& )
		{
		}

		virtual void ConvertText(int const , std::wstring &) // TextTableNo -> Text
		{
		}

		bool ParseElement(std::wstring const& rkInput, std::wstring &kResult);

		CLASS_DECLARATION_S_NO_SET(std::wstring, Result);
	private:

		CONT_DEFSTRINGS const* m_pkDefString;
		CONT_DEFMONSTER const* m_pkDefMonster;
		CONT_DEFITEM const* m_pkDefItem;
		CONT_DEFMAP const* m_pkDefMap;
		CONT_DEF_MISSION_ROOT const* m_pkDefMissionRoot;
	};

	bool CheckQuestItemLimitOperator(EQuestItemLimitOperator const eType, size_t const iInfoValue, size_t const iCurCount);

	// for Realm Quest
	typedef struct tagVerifyRealmQuestInfo
	{
		tagVerifyRealmQuestInfo();
		tagVerifyRealmQuestInfo(std::wstring const& rkRealmQuestID, int const& riQuestID);
		tagVerifyRealmQuestInfo(tagVerifyRealmQuestInfo const& rhs);
		~tagVerifyRealmQuestInfo();
		bool operator ==(std::wstring const& rhs) const;

		std::wstring kRealmQuestID;
		int iQuestID;
	} SVerifyRealmQuestInfo;
	typedef std::list< SVerifyRealmQuestInfo > CONT_REALM_QUEST_ID;
	void GetVerifyRealmQuestID(CONT_REALM_QUEST_ID& rkOut);
	void AddVerifyRealmQuestID(int const iQuestID, std::wstring const& rkRealmQuestID);
	bool IsQuestDependAbilType(int const iAbilType);
	bool IsNfyQuestDependAbilType(int const iAbilType);
}


//
namespace PgQuestInfoVerifyUtil
{
	typedef enum eErrorType
	{
		ET_None				= 0,
		ET_ParseError		= 1,
		ET_DataError		= 2,
		ET_LogicalError		= 3,
	} EErrorType;

	std::wstring GetText(EErrorType const eType);
	
	struct SQuestInfoError
	{
		explicit SQuestInfoError(PgQuestInfo const* pkQuestInfo);
		explicit SQuestInfoError(SQuestInfoError const& rhs);
		explicit SQuestInfoError(PgQuestInfo const* pkQuestInfo, EErrorType const Type , TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg);
		explicit SQuestInfoError(EErrorType const Type ,std::string const FilePathName, int const QuestID, TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg);

		void operator = (SQuestInfoError const& rhs); // don't use operator =

		void Set(EErrorType const Type ,TCHAR const* Func, size_t const Line, BM::vstring const ErrorMsg);

		EErrorType eType;
		std::string const kFilePathName;
		int const iQuestID;
		TCHAR const* szFunc;
		size_t iLine;
		BM::vstring kErrorMsg;
	};

	typedef std::list< SQuestInfoError > ContErrorMsg;
	typedef void (CALLBACK* LPOUTPUTERRORFUNC)(SQuestInfoError const& rkNewError);
	typedef std::map< int, PgQuestInfo* > ContQuestInfo;

	extern LPOUTPUTERRORFUNC g_lpAddErrorFunc; // used at PARSING and Verify()
	extern LPOUTPUTERRORFUNC g_lpOutputErrorFunc; // used at DisplayResult()

	void CALLBACK AddError(SQuestInfoError const& rkNewError);
	void CALLBACK OutputError(SQuestInfoError const& rkError);

	// not seted g_lpAddErrorFunc, use default queued internal g_kErrorMsg
	// not seted g_lpOutputErrorFunc, default display consol by CAUTION_LOG
	void VerifyXmlToDef(ContQuestInfo const& rkInfoMap);
	void Verify(PgQuestInfo const* pkQuestInfo, ContQuestInfo const& rkXmlInfoMap, int const iMaxKillCount);
	bool DisplayResult();
}


//
typedef enum eQuestDefaultEnum
{
	QDE_QuestLimitMaxLevel = 999,
	QDE_QuestBeginShowLimitLevel = 10,
} EQuestDefaultEnum;


//
typedef enum eQuestTalkState
{
	QTS_None = 0,
	QTS_NowTalking = 1, // 0이 아니면 대화중이다(현재 대화중인 QuestID 가 저장)
} EQuestTalkState;


// Quest State
typedef enum eQuestState//이미 있는 값은 수정 하지 말자(GM Tool DB에서 사용 중)
{
	QS_None				= 0x00,//							(only system)
	QS_Begin			= 0x01,// 시작 가능한 퀘스트		(only system)
	QS_Started			= 0x02,// 사용안함					(only system)
	QS_Ing				= 0x03,// 수행중인 퀘스				(DB Save, in system)
	QS_Failed			= 0x04,// 실패 퀘스트				(DB Save, in system)
	QS_End				= 0xf0,// 종료가능한 퀘스트			(DB Save, in system)
	QS_Finished			= 0xff,// 완료한 퀘스트				(only system)
	
	//이하는 시스템 사용 전용
	QS_Begin_NYet		= 0x11,// 1렙만 더 업하면 사용 가능
	QS_Begin_Loop		= 0x21,// 반복 퀘스트 전용
	QS_End_Loop			= 0x22,// 반복 퀘스트 전용
	QS_Begin_Story		= 0x41,// Story 퀘스트 전용
	QS_End_Story		= 0x42,// Story 퀘스트 전용
	QS_Begin_Tactics	= 0x44,// 용병 퀘스트 전용
	QS_End_Tactics		= 0x48,// 용병 퀘스트 전용
	QS_Begin_Weekly		= 0x81,// 주간 퀘스트 전용(Quest_ResetSchedule 테이블에 존재 하는 퀘스트)
	QS_End_Weekly		= 0x82,// 주간 퀘스트 전용
	QS_Begin_Couple		= 0x84,// 커플 퀘스트 전용
	QS_End_Couple		= 0x88,// 커플 퀘스트 전용

	QS_Begin_Event		= 0x91,// 이벤트 퀘스트 전용
	QS_End_Event		= 0x92,// 이벤트 퀘스트 전용
}EQuestState;


//
typedef enum eQuestType
{
	QT_None				= 0,
	QT_Normal			= 1,	// 보통 (NPC 얼굴이 나오는 기본 퀘스트, 현상수배 퀘스트도 이 타입)
	QT_Scenario			= 2,	// 시나리오 퀘
	QT_ClassChange		= 3,	// 전직 퀘 (Reward에 전직이 존재하면 자동으로 이 퀘스트가 된다)
	QT_Loop				= 4,	// 반복 퀘 (퍼렁색 마크, 별도의 리스트 UI가 존재)
	QT_Day				= 5,	// 1일 퀘스트 (1일 마다 리셋이 되는 퀘스트)
	QT_MissionQuest		= 6,	// 미션 퀘스트 (사용하지 않음)
	QT_GuildTactics		= 7,	// 길드 용병 퀘스트 (1일 마다 리셋이 되는 길드 경험치 퀘스트, 녹색 마크, 별도의 리스트 UI가 존재)
	QT_Couple			= 8,	// 커플 들이 하는 퀘스트
	QT_SweetHeart		= 9,	// 부부 들이 하는 퀘스트
	QT_Random			= 10,	// 랜덤 퀘스트
	QT_RandomTactics	= 11,	// 랜덤 용병 퀘스트
	QT_Soul				= 12,	// Soul Quest
	QT_Wanted			= 13,	// 현상수배 퀘스트( 하루마다 초기화 )	
	QT_Scroll			= 14,
	QT_Week				= 15,	// 주간퀘스트
	QT_BattlePass		= 16,	// Battle pass quest

	QT_Event_Normal		= 21,	//기존 Type 1과 동일한 역할을 한다. QT_Normal
	QT_Event_Loop		= 24,	//기존 Type 4와 동일한 역할을 한다. QT_Loop
	QT_Event_Normal_Day	= 25,	//기존 Type 101과 동일한 역할을 한다. QT_Normal_Day

	QT_Normal_Day		= 101,	// 일반 퀘스트(보통 퀘스트 UI에, 1일 마다 반복)

	QT_Dummy			= 99,	// 더미 퀘스트들(퀘스트 분기 지점을 만들기 위한 데이타)
	QT_Deleted			= -1,	// 삭제 예정 퀘스트
}EQuestType;

typedef enum eBuildLoopQuestFlag
{
	BLQF_NONE					= 0,
	BLQF_DAY_LOOP_QUEST			= 0x01,
	BLQF_RANDOM_QUEST			= 0x02,
	BLQF_TACTICS_RANDOM_QUEST 	= 0x04,
	BLQF_WANTED_QUEST			= 0x08,
	BLQF_SCHEDULE_QUEST			= 0x10,
	BLQF_WEEK_LOOP_QUEST		= 0x20,

	BLQF_ALL					= 0xFFFF,
}EBuildLoopQuestFlag;

//
typedef enum eGuildTacticsQuestLevel	// QT_GuildTactics 타입만 적용된다.
{
	GTQL_None = 0,
	GTQL_LvF = 1,
	GTQL_LvE = 2,
	GTQL_LvD = 3,
	GTQL_LvC = 4,
	GTQL_LvB = 5,
	GTQL_LvA = 6,
	GTQL_LvS = 7,
	GTQL_LvSS = 8,
	GTQL_LvSSS = 9,
} EGuildTacticsQuestLevel;


//
typedef enum eQuestEventType
{
	QET_None = 0,

	QET_MONSTER_IncParam = 1,		// 몬스터 사냥으로 경험치 획득시 단순 n증가
	QET_MONSTER_IncParam100,

	QET_MONSTER_InstDrop,			// 몬스터 사냥으로 경험치 획득시 인벤으로 지정 아이템 지급
	QET_MONSTER_InstDrop100,

	QET_MONSTER_IncParam_In,		// 몬스터 사냥으로 경험치 획득시(그라운드 내에서 만) 단순 n증가
	QET_MONSTER_IncParam100_In,

	QET_MONSTER_InstDrop_In,		// 몬스터 사냥으로 경험치 획득시(그라운드 내에서 만) 인벤으로 지정 아이템 지급
	QET_MONSTER_InstDrop100_In,

	QET_MONSTER_IncParam_M,			// 몬스터를 죽이면 Parameter 증가. 단, 특정 미션의 난이도에서
	QET_MONSTER_IncParam100_M,		// 그룹화된 몬스터를 죽이면 Parameter 증가. 단, 특정 미션의 난이도에서

	QET_MONSTER_InstDrop_M,			// 몬스터를 죽이면 아이템 드롭. 단, 특정 미션의 난이도에서
	QET_MONSTER_InstDrop100_M,		// 그룹화된 몬스터를 죽이면 아이템 드롭. 단, 특정 미션의 난이도에서

	QET_MONSTER_IncParam_MN,		//  몬스터를 죽이면 Parameter 증가. 단, 특정 미션의 특정 난이도를 특정 맵장수로 입장했을 때
	QET_MONSTER_IncParam100_MN,		// 그룹화된 몬스터를 죽이면 Parameter 증가. 단, 특정 미션의 특정 난이도를 특정 맵장수로 입장했을 때

	QET_MONSTER_InstDrop_MN,		// 몬스터를 죽이면 아이템 드롭. 단, 특정 미션의 특정 난이도를 특정 맵장수로 입장했을 때
	QET_MONSTER_InstDrop100_MN,		// 그룹화된 몬스터를 죽이면 Parameter 증가. 단, 특정 미션의 특정 난이도를 특정 맵장수로 입장했을 때

	QET_MONSTER_ComboCheck,			// 몬스터 사냥으로 경험치 획득시 지정한 콤보 이상 달성하면 단순 n 증가
	QET_GROUND_ComboCheck,			// 그라운드내에 모든 몬스터 사냥시 단순 n증가
	QET_ITEM_ChangeCount,			// 지정 아이템 소유 수량에 따라 n값 결정
	QET_ITEM_NotChangeCount,		// 퀘스트 완료 시 아이템을 소모하지 않음
	QET_ITEM_Durability_Check,		// 지정한 아이템 수리시에 단순 n증가
	QET_ITEM_Plus_Check,			// 지정한 아이템 인챈트시에 단순 n증가
	QET_ITEM_Check_Pet,				// 펫의 레벨과 번호를 체크한 뒤 반환
	QET_LOCATION_LocationEnter,		// 지정한 그라운드내에 트리거 충돌시 단순 n증가
	QET_NPC_Client,					// 퀘스트를 지급해 주는 npc 이벤트 설정
	QET_NPC_Agent,					// 퀘스트 진행과 관계된 npc 이벤트 설정
	QET_NPC_Payer,					// 퀘스트 보상 주는 npc 이벤트 설정
	QET_NPC_IncParam,				// npc 대화시 단순 n증가 이벤트
	QET_NPC_CheckGold,				// npc 대화시 골드량에 따른 대화 분기 이벤트
	QET_NPC_GiveItem,				// npc 대화시 지정한 아이템 지급 이벤트
	QET_NPC_Ing_Dlg,				// 진행중인 npc 대화 이벤트
	QET_NPC_CheckItem,				// npc 대화시 지정한 아이템 수량에 따른 분기 이벤트
	QET_NPC_RemoveItem,				// npc 대화시 지정한 아이템 삭제 이벤트
	QET_NPC_CompleteQuest,			// npc 대화시 별도로 지정한 퀘스트 강제 완료 이벤트 (더미 퀘스트를 완료 시켜야 한다)
	QET_NPC_AddGold,				// npc 대화시 지정한 골드를 지급하는 이벤트
	QET_NPC_CheckEffect,			// npc 대화시 지정한 버프를 체크하는 대화 분기 이벤트
	QET_NPC_GiveEffect,				// npc 대화시 지정한 버프를 지급하는 이벤트
	QET_MISSION_Rank,				// 지정한 미션에서 지정한 랭크를 달성시 단순 n증가
	QET_MISSION_Percent,			// 지정한 미션에 지정한 달성율 이상이면 단순 n증가
	QET_MISSION_Clear,				// 지정한 미션을 클리어 했거나(한다면) 단순 n증가
	QET_MISSION_DEFENCE_CLEAR,		// 지정한 디펜스 미션을 클리어
	QET_MISSION_STRATEGY_DEFENCE_CLEAR,	// 지정한 전략 디펜스 미션을 클리어
	QET_MISSION_RankN,				// 지정한 미션의 지정된 맵수량에서 지정한 랭크를 달성시 단순 n 증가
	QET_MISSION_ClearN,				// 지정한 미션의 지정된 맵수량을 클리어 했거나(한다면) 단순 n 증가
	QET_PVP_Win,					// PVP 에서 승리를 한다면 단순 n증가
	QET_PVP_Lose,					// PVP 에서 패배를 한다면 단순 n증가
	QET_PVP_Draw,					// PVP 에서 비긴다면 단순 n증가
	QET_CLASS_Change,				// 전직을 한다면 단순 n증가
	QET_KillCount,					// 킬카운트 수량이 지정한 수 이상이면 단순 n증가
	QET_MYHOME_INVITEUSERCOUNT,		// 마이홈을 가지고 있고 방문자 수가 지정한 수 이상이면 단순 n증가
	QET_ABIL_GREATER_EQUAL,			// 유닛에 해당 어빌을 일정 이상 달성 했는가	
	QET_GROUND_KillMonster,			// 그라운드내에서 어떠한 몬스터라도 죽이면 단순 n 증가
	QET_GROUND_MonsterDrop,			// 그라운드내에서 어떠한 몬스터라도 죽이면 아이템 n개를 확률로 Drop	
	QET_GLOBAL_Combo_IncParam,		// 지정한 콤보 이상 달성하면 Param 증가
	QET_GROUND_IndunResult,			// Complete indu mission, see: INDUN_STATE_RESULT
} EQuestEventType;


//
typedef enum eQuestSelectType
{
	QST_None = 0,
	QST_Normal = 1,			//기본 선택지 버튼
	QST_Accept,				//수락 버튼
	QST_Reject,				//거부 버튼
	QST_Complete,			//완료 버튼
} EQuestSelectType;


//
typedef enum eQuestDialogType
{
	QDT_None = 0,
	QDT_Normal = 1,			//기본 퀘스트 창
	QDT_Prolog,				//수락/거부 형태
	QDT_Complete,			//완료 형태
	QDT_Info,				//퀘스트 종합 정보
	QDT_Talk_Menu,			//퀘스트 대화 선택지 메뉴

	//여기서 부턴 다른곳에서 사용하는 용도 (!! 번호 변경하지 마시오 !!)
	QDT_Wanted					= 100, //현상금 퀘스트 정보창
	QDT_Wanted_Complete			= 101, //현상금 퀘스트 완료 정보창
	// 아래부터 클라이언트 측 전용
	QDT_NpcTalk					= 102, //NPC 대화창
	QDT_Guild_Menu				= 103,
	QDT_Guild_Destroy			= 104,
	QDT_Guild_PreCreate			= 105,
	QDT_Guild_Create			= 106,
	QDT_Guild_LvUP				= 107,
	QDT_Guild_BuySkill			= 108,
	QDT_Guild_Mark				= 110,
	QDT_EventScriptTalk			= 111,
	QDT_VillageBoard			= 112, // 마을 게시판 퀘스트 정보창
	//QDT_VillageBoard_Complete	= 113, // 마을 게시판 퀘스트 완료 정보창
	QDT_Guild_Inventory_Create	= 114,
	
	QDT_Movie					= 115, //퀘스트에 포함되는 동영상

	QDT_GuildEmporia_Administrator	= 200,
	QDT_GuildEmporia_Admin_Building = 201,
	QDT_GuildEmporia_Admin_Removal	= 202,


	QDT_Function_1		= -1,		// Call CP UI
	QDT_Function_2		= -2,		// Call Fran UI
	QDT_Function_3		= -3,		// Call Soul Craft UI

	QDT_TextDialog		= 10000,	// 기획자가 입력할수 있는 TextDialogs.xml에 예약 (10000 ~ INT_MAX)
} EQuestDialogType;

// ECashType: 펫이 캐쉬펫이냐 아니냐의 제한을 두기 위해서 추가함
typedef enum tagECashType
{
	CT_All			= 0,		//체크하지 않음
	CT_Ingame,					//캐쉬가 아닌 펫
	CT_Cash,					//캐쉬펫
} ECashType;

//
typedef struct tagQuestBasicInfo
{
	EQuestType eType;
	int iQuestID;		//Quest IDX
	int iGroupNo;		//Group Quest No
	int iDifficult;		//Quest Difficult(use only daily quest)
	int iOrderIndex;	//Order by Index(use only daily quest)

	tagQuestBasicInfo();
	tagQuestBasicInfo(int const iID, int const iGroup);

	void Clear();
} SQuestBasicInfo;


//
typedef struct tagQuestLimitTime
{
	SYSTEMTIME kTimeStart;
	SYSTEMTIME kTimeEnd;

	explicit tagQuestLimitTime(char const* szStart, char const* szEnd, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	explicit tagQuestLimitTime(SYSTEMTIME const& rkStartTime, SYSTEMTIME const& rkEndTime);

	void Clear();
	bool IsCan(SYSTEMTIME const& rkCurTime) const;
	static void StrToTime(char const* szTimeString, SYSTEMTIME& rkOutTime);//"200x-xx-xx xx:xx:xx"
	static std::wstring Str(SYSTEMTIME const& rkTime);
	//아래 이후는 좌항이 항상 현재 시간값이다
	static bool Less(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight);//Left < Right(config)
	static bool Greater(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight);//Left > Right(Config)
	static bool Equal(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight);//Left == Right
	static bool LessEqual(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight);//Left <= Right
	static bool GreaterEqual(SYSTEMTIME const& rkNow, SYSTEMTIME const& rkRight);//Left >= Right
} SQuestLimitTime;
typedef std::vector< SQuestLimitTime > ContQuestTimeLimit;


//
typedef std::set< int > ContDayOfWeek;
typedef std::vector<BYTE> ContPetType;
typedef std::vector<BYTE> ContPetClassGrade;
typedef std::vector<int> ContHaveSkill;
typedef struct tagQuestLimitInfo
{
	__int64 iClassFlag;		//수행 가능한 직업
	int iMinLevel;			//최소 레벨
	int iMaxLevel;			//최대 레벨(시작 마크를 표시 하느냐 마느냐 만)
	int iLimitMaxLevel;		//최대 레벨(실제 적용 되는 최대 레벨)
	int iMinParty;			//최소 파티
	int iMaxParty;			//최대 파티
	ContQuestTimeLimit kLimitTime; //진행 가능 시간
	ContDayOfWeek kDayOfWeek; // 진행 가능 요일 (0 == 일, 1~6 == 월~토)
	ContPetType kPetType;	//펫 종류 (캐쉬펫, 전투펫, 라이딩펫)
	ContPetClassGrade kPetClassGrade; //펫 전직 등급 (1, 2, 3)
	int iTacticsLevel;		//용병 레벨
	int iPetLevelMin;
	int iPetLevelMax;
	ECashType ePetCashType;
	ContHaveSkill kSkillAndLimit;
	ContHaveSkill kSkillOrLimit;

	tagQuestLimitInfo();

	void Clear();
	void AddClass(int iClassNo);
	bool CheckLevel(int const iLevel) const;
	bool CheckPetLevel(int const iPetLevel) const;
	bool CheckClass(int const iClassNo) const;
	bool CheckPetType(int const iPetType) const;
	bool CheckPetClassGrade(BYTE const byPetClassLevel) const;
	bool CheckPetCashType(PgBase_Item const &kItem) const;
	void AddTime(const SQuestLimitTime& rkNewLimit);
	bool CheckTime() const;
	bool CheckTime(SYSTEMTIME const& rkCurrentTime) const;
} SQuestLimitInfo;


//
typedef struct tagQuestItem
{
	int iItemNo;
	size_t iCount;
	int iRarityType;
	bool bSeal;
	int iTimeType;
	int iTimeValue;

	tagQuestItem();
	explicit tagQuestItem(int const ItemNo, int const Count);
	explicit tagQuestItem(int const ItemNo, int const Count, int const RarityType, bool const Seal, int const TimeType, int const TimeValue);

	void Clear();
	bool operator == (tagQuestItem const& rhs) const;
	bool operator == (int const rhs) const;
} SQuestItem;
typedef std::vector< SQuestItem > ContQuestItem;


//
typedef struct tagQuestLimitItem : public tagQuestItem
{
	explicit tagQuestLimitItem(int const ItemNo, int const Count, EQuestItemLimitOperator const OperatorType);

	void Clear();
	bool operator == (tagQuestLimitItem const& rhs) const;

	EQuestItemLimitOperator eOperatorType;
} SQuestLimitItem;
typedef std::vector< SQuestLimitItem > ContQuestLimitItem;


//
typedef struct tagChangeClassInfo
{
	int iChangeClassNo;
	int iPrevClassNo;
	int iMinLevel;

	tagChangeClassInfo();
	explicit tagChangeClassInfo(int const ChangeClassNo, int const PrevClassNo, int const MinLevel);

	void Set(int const ChangeClassNo, int const PrevClassNo, int const MinLevel);
	void Clear();
} SChangeClassInfo;


//
typedef struct tagQuestRewardSet
{
	typedef std::vector< int > ContClassNo;

	ContClassNo kCanClass;
	ContQuestItem kItemList;
	bool bSelective;
	int iGender;

	tagQuestRewardSet();

	void Clear();
	bool AddRewardItem(const SQuestItem& rkRewardItem);
	bool AddClass(int const iClassNo);
	bool IsCanClass(int const Gender, int const iClassNo) const;
} SQuestRewardSet;
typedef std::vector< SQuestRewardSet > ContRewardVec;
typedef std::vector< int > ContRewardEffectVec;

//
typedef struct tagQuestReward
{
	int iExp;
	int iMoney;
	int iTacticsExp;
	int iGuildExp;
	int iSkillNo;
	int iChangeLevel;

	int iLevelPer_Exp_1000;			//천분률
	int iLevelPer_Money_1000;		//천분률

	ContRewardVec kClassRewardSet1;
	ContRewardVec kClassRewardSet2;
	SChangeClassInfo kChangeClass;
	ContRewardEffectVec kEffect;
	ContQuestID kAddEndQuest;		// 추가 완료 설정할 퀘스트
	std::wstring kRealmQuetID;		// 렐름 협동 퀘스트 ID
	bool kUnlockCharacters;			// Unlock all character in account with state CIDBS_NeedRestore

	tagQuestReward();
	explicit tagQuestReward(int const Exp, int const Money, int const TacticsExp, int const GuildExp, std::wstring const& rkRealmQuestID, int iLevelPer_Exp_1000, int iLevelPer_Money_1000 );

	bool AddRewardSet(int const iSetNo, SQuestRewardSet const &rkRewardSet, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	void Clear();
	ContRewardVec::value_type const* GetRewardSet1(int const iGender, int const iClassNo) const;
	ContRewardVec::value_type const* GetRewardSet2(int const iGender, int const iClassNo) const;
	size_t SizeRewardSet1()const		{ return kClassRewardSet1.size(); }
	size_t SizeRewardSet2()const		{ return kClassRewardSet2.size(); }

protected:
	ContRewardVec::value_type const* GetRewardSet(ContRewardVec const &rkVec, int const iGender, int const iClassNo) const;
} SQuestReward;


//
typedef struct tagQuestObject
{
	int iObjectCount[QUEST_PARAMNUM];
	
	tagQuestObject();

	void Clear();
	int GetEndCount(int const iObjectNo) const;
} SQuestObject;


//
typedef struct tagQuestNpc
{
	BM::GUID kNpcGuid;
	int iEventNo;
	EQuestState eMark;

	tagQuestNpc();
	//tagQuestNpc(BM::GUID const &rkGuid, int const EventNo, const EQuestState Mark);
	explicit tagQuestNpc(BM::GUID const &rkGuid, int const EventNo, char const* szMark, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
} SQuestNpc;
typedef std::vector< SQuestNpc > ContQuestNpc;


//
typedef struct tagQuestBasicDepend
{
	int iType;
	int iObjectNo;

	explicit tagQuestBasicDepend(int const ObjectNo);
} SQuestBasicDepend;


//
typedef struct tagQuestGround : public SQuestBasicDepend
{
	int iGroundNo;
	//						"COMBOCHECK"	"KILLMONSTER"	"MONSTERDROP"
	unsigned int iVal1;	//	TargetCombo		 IncValue		 Max Rate
	int iVal2; //			IncValue						 Success rate
	int iVal3; //											 Max Count
	int iVal4; //											 Min Count
	int iItemNo;//											 Item No

	explicit tagQuestGround(int const GroundNo, int const ObjectNo);

	bool Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestGround;
typedef std::multimap< int, SQuestGround > ContQuestGround;


//
typedef struct tagQuestMonster : public SQuestBasicDepend
{
	int iClassNo;
	//						"INCPARAM"	"INSTDROP"	"INCPARAM_IN"	"INSTDROP_IN"	"COMBOCHECK"   "INSTDROP_M"   "INCPARAM_M"		"INCPARAM_MN"			"INSTDROP_MN"
	int iTargetGroundNo;//							TargetGndNo		TargetGndNo					
	unsigned int iVal1;//	IncValue	Max Rate	IncValue		Max Rate		TargetCombo		Max Rate		IncValue		IncValue				Max Rate
	int iVal2;//						Success Rate				Success Rate	Inc Value		Success Rate											Success Rate
	int iVal3;//						Max Count					Max Count						Max Count												Max Count
	int iVal4;//						Min Count					Min Count						Min Count												Min Count
	int iItemNo;//						Item No						Item No							Item No													Item No
	int iMissionKey;//																				MissionKey		MissionKey		MissionKey				MissionKey
	int iMissionLevel;//																			MissionLevel	MissionLevel	MissionLevel			MissionLevel
	int iStageCount;//																												Mission Stage Count		Mission Stage Count
	
	explicit tagQuestMonster(int const ClassNo, int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestMonster;
typedef std::multimap< unsigned int, SQuestMonster > ContQuestMonster;//Monster Class No, Event


//
typedef struct tagQuestDependItem : public SQuestBasicDepend
{
	int iItemNo;
	int iVal1;

	tagQuestDependItem();
	explicit tagQuestDependItem(int const ItemNo, int const ObjectNo, int const Val1 = 0);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
}SQuestDependItem;
typedef std::multimap< unsigned int, SQuestDependItem > ContQuestDependItem;//ItemID, Event


//
typedef struct tagQuestLocation : public SQuestBasicDepend
{
	int iLocationNo;
	int iGroundNo;

	tagQuestLocation();
	explicit tagQuestLocation(int const LocationNo, int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestLocation;
typedef std::vector< SQuestLocation > ContQuestLocation;


//
typedef struct tagQuestDependNpc : public SQuestBasicDepend
{
	int iEventNo;
	int iDialogID;
	int iVal1;
	std::wstring kStr;
	__int64 iGold;
	SQuestItem kItem;

	tagQuestDependNpc();
	explicit tagQuestDependNpc(int const EventNo, int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestDependNpc;
typedef std::multimap< int, SQuestDependNpc > ContQuestDependNpc;//EventNo, Event


//
typedef struct tagQuestDependMission : public SQuestBasicDepend
{
						//	QET_MISSION_Rank		QET_MISSION_Percent		QET_MISSION_Clear	QET_MISSION_DEFENCE_CLEAR	QET_MISSION_RankN	QET_MISSION_ClearN
	int iMissionKey;	//	
	int iVal1;			//	Mission Level			Percent					Mission Level		Clear Stage(<=)				Mission Level		Mission Level
	int iVal2;			//	Mission Rank																					Mission Rank		
	int iStageCount;	//																									Mission Stage Count	Mission Stage Count

	tagQuestDependMission();
	explicit tagQuestDependMission(int const MissionKey, int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestDependMission;
typedef std::vector< SQuestDependMission > ContQuestDependMission;


//
typedef struct tagQuestDependPvp : public SQuestBasicDepend
{
	tagQuestDependPvp();
	explicit tagQuestDependPvp(int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
	bool Empty() const;
} SQuestDependPvp;
typedef std::vector< SQuestDependPvp > ContQuestDependPvp;

//
typedef struct tagQuestDependCouple : public SQuestBasicDepend
{
	__int64		iTime;
	tagQuestDependCouple();
	explicit tagQuestDependCouple(int const ObjectNo);

	//bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
	bool Empty() const;
} SQuestDependCouple;

//
typedef struct tagQuestDependClass : public SQuestBasicDepend
{
	int iClassNo;

	tagQuestDependClass();
	explicit tagQuestDependClass(int const ClassNo, int const ObjectNo);

	bool Build(std::wstring const &rkType, std::wstring const &rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
}SQuestDependClass;
typedef std::vector< SQuestDependClass > ContQuestDependClass;

//
typedef struct tagQuestDependKillCount : public SQuestBasicDepend
{
	int iKillCount;

	tagQuestDependKillCount();
	explicit tagQuestDependKillCount(int const KillCount, int const ObjectNo);

	bool Build(PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestDependKillCount;

//
typedef struct tagQuestDependMyHome : public SQuestBasicDepend
{
	int iCount;

	tagQuestDependMyHome();
	explicit tagQuestDependMyHome(int const Count, int const ObjectNo);

	bool Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestDependMyHome;

//
typedef struct tagQuestDependAbil : public SQuestBasicDepend
{
	int iAbilType;
	int iAbilValue;
	tagQuestDependAbil();
	explicit tagQuestDependAbil(int const iAbilType, int const ObjectNo);

	bool Build(std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName);
} SQuestDependAbil;
typedef std::vector< SQuestDependAbil > ContQuestDependAbil;

typedef struct tagQuestDependGlobal : public SQuestBasicDepend
{
				// "COMBOCHECK"
	int iVal1;	// Combo Count
	int iVal2;	// Inc Value

	tagQuestDependGlobal();
	explicit tagQuestDependGlobal( int const iObjectNo );

	bool Build( std::wstring const& rkType, std::wstring const& rkValue, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, char const* szElementName );
} SQuestDependGlobal;
typedef std::vector< SQuestDependGlobal > ContQuestDependGlobal;

///////////////////////////////////////////////////////////////


//
typedef struct tagQuestSelect
{
	EQuestSelectType eType;
	int iSelectID;
	int iSelectTextNo;

	tagQuestSelect();
	explicit tagQuestSelect(std::wstring const &rkType, int const iID, int const iTTW, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	explicit tagQuestSelect(const EQuestSelectType Type, int const iID, int const iTTW);

	void Set(std::wstring const &rkType, int const iID, int const iTTW, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	void Set(const EQuestSelectType Type, int const iID, int const iTTW);
	void Clear();
} SQuestSelect;
typedef std::vector< SQuestSelect > ContQuestSelect;


//
typedef struct tagQuestDialogText
{
	int iTextNo;					//From QuestTextTable.xls(xml)
	std::wstring kFaceID;			//From NPC_Face.xml
	int iFaceAniID;					//Only 3D Model(actor Ani ID No)
	std::wstring kModelID;			//From Actor ID
	int iTextNameID;				//From TextTable.xls(xml)
	int iStringNameID;				//From TB_DefStrings
	std::wstring kStitchImageID;	//From StitchImage.xml
	POINT3 kCamPos;
	int iMovieNo;					//From QuestMovie.xml

	tagQuestDialogText();
	explicit tagQuestDialogText(int const TextNo);

	void Clear();
} SQuestDialogText;
typedef std::vector< SQuestDialogText > ContQuestDialogText;



//
typedef enum eQuestReservedDialogID
{
	QRDID_None					= 0,
	QRDID_Close					= 0,
	QRDID_StartDIalogID			= 100,	// 퀘스트 
	QRDID_Begin					= 101,	// 퀘스트를 시작 할 수 있는 경우 수락/거부 선택 다이얼로그
	QRDID_AcceptReject			= 201,	// 수락 거부 했을 때
	QRDID_Accepted				= 301,	// 수락 했을 때 
	QRDID_Rejected				= 401,	// 포기했을 때(사용하지 않음)
	QRDID_Ing_Start				= 501,	// 진행중에 사용될 퀘스트 다이얼로그 대역
	QRDID_Ing_End				= 599,	// 
	QRDID_CanComplete			= 601,	// 퀘스트를 완료한다음 대화 했을때 나올 다이얼로그
	QRDID_Completed				= 701,	// 완료를 수락 했을 때 기본으로 이동할 다이얼로그
	QRDID_RejectComplete		= 801,	// 퀘스트를 완료 거부 했을 때 기본으로 이동할 다이얼로그
	QRDID_Info					= 901,	// 퀘스트 정보창 "L" 단축 키 창에서 보여짐
	QRDID_FullInven				= 902,	// 인벤이 꽉 차 있어 보상아이템/지급 아이템을 받을 수 없을 때 출력
	QRDID_MaxQuestSlot			= 903,	// 퀘스트 시작시 빈 퀘스트 슬롯이 부족 할 때 출력
	QRDID_LessItemCount			= 904,	// 완료시 아이템이 부족 할 때 출력
	QRDID_DailyQuestClosedInfo	= 905,	// 일일 퀘스트 진행하지 못할때 출력
	QRDID_TiredLimitCantReward	= 910,	// 피로도에 의한 보상을 받지 못 할 때 출력
	QRDID_TimeLimited			= 950,	// 시간제한 넘어서 완료 하려 할 때 출력
	QRDID_PreDayQuestLimited	= 951,	// 
	QRDID_FailedComplete		= 952,	// 실패한 퀘스트를 완료 하려 할때 출력
	QRDID_OnlyOneCoupleQuest	= 953,	// 커플 퀘스트는 하나만 진행 가능하다
	QRDID_MaxDialogID			= 1000,
	QRDID_ACCEPT_Start			= 10000,
	QRDID_ACCEPT_End			= 19999,
	QRDID_REJECT_Start			= 20000,
	QRDID_REJECT_End			= 29999,
	QRDID_COMPLETE_Start		= 30000,
	QRDID_COMPLETE_End			= 39999,
	QRDID_REJECTCOMPLETE_Start	= 40000,
	QRDID_REJECTCOMPLETE_End	= 49999,
} EQuestReservedDialogID;



//
typedef struct tagQuestDialog
{
	EQuestDialogType	eType;
	int					iDialogID;
	ContQuestDialogText	kDialogText;
	ContQuestSelect		kSelectList;

	tagQuestDialog();
	explicit tagQuestDialog(std::wstring const &rkType, int const iID, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	explicit tagQuestDialog(const EQuestDialogType Type, int const iID);

	bool Parse(TiXmlElement const* pkDialogNode, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	bool ParseSub(TiXmlElement const* pkChildNode, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo, bool const bIsNullPass = false);
	void Set(std::wstring const &rkType,  int const iID, PgQuestInfoVerifyUtil::SQuestInfoError& rkErrorInfo);
	void Set(const EQuestDialogType iType, int const iID);
	void Clear();

private:
	void AddSelect(const SQuestSelect& rkSelect);
	void AddText(const SQuestDialogText& rkQuestText);
} SQuestDialog;
typedef std::map< int, SQuestDialog > ContQuestDialog;


//
typedef struct tagQuestDelItem
{
	tagQuestDelItem(int const ItemNo, int const Count, int const ObjectNo, int PetLv = 0);
	~tagQuestDelItem();
	bool operator == (int const rhs) const;

	int iItemNo;
	int iCount;
	int iObjectNo;
	int iPetLv;
} SQuestDelItem;
typedef std::list< SQuestDelItem > CONT_DEL_QUEST_ITEM;

typedef enum eQuestObjectsType
{
	QUEST_OBJ_NOSTEP				= 0,
	QUEST_OBJ_STEPBYSTEP			= 1,
} EQuestObjectsType;

///////////////////////////////////////////////////////////////
class PgQuestInfo
{
public:
	PgQuestInfo();
	virtual ~PgQuestInfo();

	void Clear();

	void Build();
	void BuildFromDef();
	void BuildFromDef(TBL_DEF_QUEST_REWARD const &rkQuestReward);

	int ID()const				{ return m_kBasic.iQuestID; }
	int GroupNo()const			{ return m_kBasic.iGroupNo; }
	EQuestType Type()const		{ return m_kBasic.eType; }
	int	Difficult()const		{ return m_kBasic.iDifficult; }
	int OrderIndex() const		{ return m_kBasic.iOrderIndex; }

	bool ParseXml(TiXmlElement* pkNode, int iQuestId);

	int GetParamEndCondition(int iParamNo)const;
	int GetObjectText(int iParamNo)const;

	bool GetDialog(int const iDialogID, SQuestDialog const *& pkOut) const;//Const를 해제 하지 말자
	bool FindDialog(int const iDialogID) const;
	bool GetInfoDialog(const SQuestDialog*& pkOut) const;

	int FindNpcEventNo(BM::GUID const& rkGuid, EQuestState const eState) const;
	int GetNpcDialog(int const iEventNo, const EQuestState eState) const;

	bool IsDependItemQuest() const		{ return 0 != m_kDepend_Item.size(); }
	bool GetFirstClientNpc(BM::GUID& rkOut) const;
	bool GetFirstPayerNpc(BM::GUID& rkOut) const;
	bool IsCanRemoteComplete() const	{ return 0 != m_byIsCanRemoteComplete; }
	bool IsCanRemoteAccept() const	{ return m_bIsCanRemoteAccept; }
	int GroupTextNo()const	{return m_iGroupTextNo; }

protected:
	bool ParseXml_Sub(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Npc(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Limit(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Event(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Object(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Reward(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_Reward_Set(TiXmlElement const* pkChild, SQuestRewardSet& rkRewardSet, bool const bIsNullPass = false);
	bool ParseXml_Dialog(TiXmlElement const* pkChild, bool const bIsNullPass = false);
	bool ParseXml_LimitIngQuest(TiXmlElement const* pkChild, bool const bIsNullPass = false);

	virtual void ParseError(TiXmlNode const* pkNode, TCHAR const* szFucn, size_t const iLine);

	void AddNpcEvent(ContQuestNpc& rkDest, TiXmlElement const* pkChild, char const* szDefaultMark);

public:
	//------------- Uni side -------------
	//Basic Info
	SQuestBasicInfo			m_kBasic;
	SQuestReward			m_kReward;

	EQuestObjectsType		m_ObjectType;
	SQuestObject			m_kObject;		//목표

	//Limit Info
	SQuestLimitInfo			m_kLimit;
	ContQuestID				m_kLimit_PreQuestAnd;	//먼저 완료 해야만 하는 퀘스트 들
	ContQuestID				m_kLimit_PreQuestOr;	//먼저 완료 해야 하는 퀘스트
	ContQuestID				m_kLimit_IngQuest;	//진행 중이어야만 하는 퀘스트
	ContQuestID				m_kLimit_NotQuest;	//진행 중이거나, 완료하면 안되는 퀘스트
	ContQuestLimitItem		m_kLimit_ItemHaveAnd; //AND로 엮인 아이템
	ContQuestLimitItem		m_kLimit_ItemHaveOr;	//OR로 엮인 아이템
	ContQuestLimitItem		m_kLimit_SkillHave;	//스킬 보유 여부
	ContLocationID			m_kLimit_Location;

	//
	//NPC Info
	ContQuestNpc			m_kNpc_Client;
	ContQuestNpc			m_kNpc_Agent;
	ContQuestNpc			m_kNpc_Payer;
	//Depend Object Info
	ContQuestDependNpc		m_kDepend_Npc;
	ContQuestGround			m_kDepend_Ground;
	ContQuestMonster		m_kDepend_Monster;
	ContQuestDependItem		m_kDepend_Item;
	ContQuestLocation		m_kDepend_Location;
	ContQuestDependMission	m_kDepend_Mission;
	ContQuestDependPvp		m_kDepend_Pvp;
	SQuestDependCouple		m_kDepend_Couple;
	ContQuestDependClass	m_kDepend_Class;
	__int64					m_kDepend_Time;
	SQuestDependKillCount	m_kDepend_KillCount;
	SQuestDependMyHome		m_kDepend_MyHome;
	ContQuestDependAbil		m_kDepend_Abil;
	ContQuestDependGlobal	m_kDepend_Global;

	//------------- Client -------------
	int					m_iTitleTextNo;
	int					m_iGroupTextNo;
	//Object Description
	int					m_iObjectTextNo1;
	int					m_iObjectTextNo2;
	int					m_iObjectTextNo3;
	int					m_iObjectTextNo4;
	int					m_iObjectTextNo5;
	//Dialog Info
	ContQuestDialog		m_kDialog;	//다이얼로그들의 목록


	//------------- Only system -------------
	// 삭제 아이템 정의
	//
	CONT_DEL_QUEST_ITEM		m_kDropDeleteItem;		//Drop, 완료시에 삭제
	CONT_DEL_QUEST_ITEM		m_kCompleteDeleteItem;	//완료시만 삭제

	CLASS_DECLARATION_S_NO_SET(int, AcceptEventScriptID);
	CLASS_DECLARATION_S_NO_SET(int, CompleteEventScriptID);
	CLASS_DECLARATION_S_NO_SET(int, CompleteNextQuestID);
	CLASS_DECLARATION_S(std::string, XmlFileName);
	CLASS_DECLARATION_S_NO_SET(bool, IsCanShare);
	CLASS_DECLARATION_S(bool, IsPartyQuest);
	BYTE m_byIsCanRemoteComplete;
	bool m_bIsCanRemoteAccept;
};

#endif // WEAPON_VARIANT_QUEST_PGQUESTINFO_H