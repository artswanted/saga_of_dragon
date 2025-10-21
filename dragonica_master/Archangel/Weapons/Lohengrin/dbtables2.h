#ifndef WEAPON_LOHENGRIN_DB_DBTABLES2_H
#define WEAPON_LOHENGRIN_DB_DBTABLES2_H

#include <map>

#include "bm/Guid.h"
#include "cel/query.h"

#include "LockUtil.h"
#include "DbTables.h"
#include "PacketStruct.h"

#pragma pack (1)

typedef enum : WORD
{
	EVERY_SUNDAY	= 0,	// 매주 일요일
	EVERY_MONDAY	= 1,	// 매주 월요일
	EVERY_TUEDAY	= 2,	// 매주 화요일
	EVERY_WEDDAY	= 3,	// 매주 수요일
	EVERY_THUDAY	= 4,	// 매주 목요일
	EVERY_FRIDAY	= 5,	// 매주 금요일
	EVERY_SATDAY	= 6,	// 매주 토요일
	EVERY_WEEKCHECK	= 7,	// 체크용도로만 사용

}ESIEGEWARTIME_TYPE;

enum eWarDBUpdateType : BYTE
{
	E_WAR_UPDATE_ERROR = 0,
	E_WAR_UPDATE_BEGIN = 1,
	E_WAR_UPDATE_END   = 2,
};
typedef BYTE EWARDBUPDATETYPE;

int const  MAX_GUILD_LEVEL_ITEM_COUNT	= (5);
typedef struct tagTBL_DEF_GUILDLEVEL
{
	tagTBL_DEF_GUILDLEVEL()
	{
		sGuildLv = 0;
		iMaxMemberCount = 0;
		iSkillPoint = 0;
		iExp = 0;
		iGold = 0;
		ZeroMemory(iItemNo, sizeof(iItemNo));
		ZeroMemory(iCount, sizeof(iItemNo));
		iIntroText = 0;
		iResultText = 0;
		iAutoChangeOwner1 = 0;
		iAutoChangeOwner2 = 0;
		iAutoChangeOwnerRun = 0;
	}

	short sGuildLv;
	int iMaxMemberCount;
	int iSkillPoint;
	__int64 iExp;
	__int64 iGold;
	int iItemNo[MAX_GUILD_LEVEL_ITEM_COUNT];
	int iCount[MAX_GUILD_LEVEL_ITEM_COUNT];
	int iIntroText;
	int iResultText;
	int iAutoChangeOwner1;
	int iAutoChangeOwner2;
	int iAutoChangeOwnerRun;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_GUILDLEVEL;

int const MAX_GUILD_SKILL_ITEM_COUNT = (2);
typedef struct tagTBL_DEF_GUILDSKILL
{
	tagTBL_DEF_GUILDSKILL()
	{
		sGuildLv = 0;
		iSkillNo = 0;
		iSkillPoint = 0;
		iGold = 0;
		iGuildExp = 0;
		ZeroMemory(iItemNo, sizeof(iItemNo));
		ZeroMemory(iCount, sizeof(iCount));
	}

	short sGuildLv;
	int iSkillNo;
	int iSkillPoint;
	__int64 iGold;
	__int64 iGuildExp;
	int iItemNo[MAX_GUILD_SKILL_ITEM_COUNT];
	int iCount[MAX_GUILD_SKILL_ITEM_COUNT];

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_GUILDSKILL;

typedef struct tagTBL_DEF_TACTICS_LEVEL
{
	tagTBL_DEF_TACTICS_LEVEL()
	{
		sTacticsLevel = 0;
		iTacticsExp = 0;
		iTacticsLevelTextID = 0;
	}

	unsigned short sTacticsLevel;
	__int64 iTacticsExp;
	int iTacticsLevelTextID;

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_TACTICS_LEVEL;

typedef std::vector< int > ContPseudoVec;
typedef struct tagTBL_DEF_TACTICS_QUEST_PSEUDO
{
	int iTacticsLevel;
	ContPseudoVec kPseudoVec;

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(size_t);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iTacticsLevel);
		kPacket.Push(kPseudoVec);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iTacticsLevel);
		kPacket.Pop(kPseudoVec);
	}
} TBL_DEF_TACTICS_QUEST_PSEUDO;

typedef struct tagMonKillRewardItem
{
	int iMinLevel;
	int iMaxLevel;
	int iItemNo;
	int iCount;

	bool CheckLevel(int const iLevel) const
	{
		return iMinLevel <= iLevel && iLevel <= iMaxLevel;
	}

	DEFAULT_TBL_PACKET_FUNC();
} SMonKillRewardItem;

typedef struct tagQuestResetSchedule
{
	tagQuestResetSchedule()
		: iQuestID(0), kResetBaseDate(), iResetLoopDay(0)
	{
	}
	~tagQuestResetSchedule()
	{
	}

	int iQuestID;
	BM::PgPackedTime kResetBaseDate;
	int iResetLoopDay;

	DEFAULT_TBL_PACKET_FUNC();
} SQuestResetSchedule;

typedef struct tagFilterUnicde
{
	tagFilterUnicde()
		: iFuncCode(0), cStart(0), cEnd(0), bFilterType(false)
	{
	}
	~tagFilterUnicde()
	{
	}

	int iFuncCode;
	wchar_t cStart;
	wchar_t cEnd;
	bool bFilterType;

	DEFAULT_TBL_PACKET_FUNC();
} SFilterUnicde;

int const MAX_QUEST_RANDOM_EXP_COUNT = 10;
typedef struct tagTBL_DEF_QUEST_RANDOM_EXP
{
	tagTBL_DEF_QUEST_RANDOM_EXP()
		: iLevel(0)
	{
		::ZeroMemory(aiExp, sizeof(aiExp));
	}

	int iLevel;
	int aiExp[MAX_QUEST_RANDOM_EXP_COUNT];

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_QUEST_RANDOM_EXP;
typedef struct tagTBL_DEF_QUEST_RANDOM_TACTICS_EXP
{
	tagTBL_DEF_QUEST_RANDOM_TACTICS_EXP()
		: iLevel(0)
	{
		::ZeroMemory(aiExp, sizeof(aiExp));
		::ZeroMemory(aiGuildExp, sizeof(aiGuildExp));
	}

	int iLevel;
	int aiExp[MAX_QUEST_RANDOM_EXP_COUNT];
	int aiGuildExp[MAX_QUEST_RANDOM_EXP_COUNT];

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_QUEST_RANDOM_TACTICS_EXP;

int const MAX_EVENT_ITEM_SET_REWARD = 3;
typedef struct tagTBL_DEF_EVENT_ITEM_SET
{
	tagTBL_DEF_EVENT_ITEM_SET()
		: iEventItemSetID(0), bIsUse(false), bIsAbsoluteCount(false), iEquipItemNo(0)
	{
		::ZeroMemory(aiRewardEffectID, sizeof(aiRewardEffectID));
	}

	int iEventItemSetID;
	bool bIsUse;
	bool bIsAbsoluteCount;
	int iEquipItemNo;
	int iItemCount;
	int aiRewardEffectID[MAX_EVENT_ITEM_SET_REWARD];

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_EVENT_ITEM_SET;

//
int const MAX_MONSTER_ENCHANT_ADD_ABIL_COUNT = 7;
int const MAX_MONSTER_ENCHANT_SET_ABIL_COUNT = 3;
int const MAX_MONSTER_ENCHANT_EFFECT_COUNT = 3;
typedef struct tagTBL_DEF_MONSTER_ENCHANT_GRADE
{
	int iEnchantLevel;
	int iPrefixNameNo;
	std::string akEffectName[MAX_MONSTER_ENCHANT_EFFECT_COUNT];
	std::string akEffectRoot[MAX_MONSTER_ENCHANT_EFFECT_COUNT];
	int aiAddAbilNo[MAX_MONSTER_ENCHANT_ADD_ABIL_COUNT];
	int aiSetAbilNo[MAX_MONSTER_ENCHANT_SET_ABIL_COUNT];

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iEnchantLevel);
		kPacket.Push(iPrefixNameNo);
		kPacket.Push(akEffectName[0]);
		kPacket.Push(akEffectName[1]);
		kPacket.Push(akEffectName[2]);
		kPacket.Push(akEffectRoot[0]);
		kPacket.Push(akEffectRoot[1]);
		kPacket.Push(akEffectRoot[2]);
		kPacket.Push(aiAddAbilNo);
		kPacket.Push(aiSetAbilNo);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		return kPacket.Pop(iEnchantLevel)
			&& kPacket.Pop(iPrefixNameNo)
			&& kPacket.Pop(akEffectName[0])
			&& kPacket.Pop(akEffectName[1])
			&& kPacket.Pop(akEffectName[2])
			&& kPacket.Pop(akEffectRoot[0])
			&& kPacket.Pop(akEffectRoot[1])
			&& kPacket.Pop(akEffectRoot[2])
			&& kPacket.Pop(aiAddAbilNo)
			&& kPacket.Pop(aiSetAbilNo);
	}
	size_t min_size()const
	{
		return sizeof(iEnchantLevel)+sizeof(iPrefixNameNo)+sizeof(aiAddAbilNo)+sizeof(aiSetAbilNo)+4*2;
	}
} TBL_DEF_MONSTER_ENCHANT_GRADE;

//
typedef struct tagDefMonsterGradeProbabilty
{
	int iProbability;
	int iMonsterEnchantGrade;

	DEFAULT_TBL_PACKET_FUNC();
} SDefMonsterEnchantGradeProbabilty;
typedef std::list< SDefMonsterEnchantGradeProbabilty > CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY;
typedef struct tagTBL_DEF_MONSTER_ENCHANT_GRADE_PROBABILTY_GROUP
{
	tagTBL_DEF_MONSTER_ENCHANT_GRADE_PROBABILTY_GROUP()
		: iTotalProbability(0), kContProbability()
	{
	}

	int iTotalProbability;
	CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY kContProbability;

	void Add(CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY::value_type const& rkNew)
	{
		kContProbability.push_back(rkNew);
		iTotalProbability += rkNew.iProbability;
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iTotalProbability);
		PU::TWriteArray_M(kPacket, kContProbability);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		return kPacket.Pop(iTotalProbability)
			&& PU::TLoadArray_M(kPacket, kContProbability);
	}
	size_t min_size()const
	{
		return sizeof(*this);
	}
} TBL_DEF_MONSTER_ENCHANT_GRADE_PROBABILTY_GROUP;

//
int const MAX_SUPER_GROUND_MODE_NO = 3;
int const MAX_SUPER_GROUND_COUNT = 60;
typedef struct tagSuperGround
{
	int iGroundNo;
	int iOrderNo;
	int aiMonsterEnchantGroupNo[MAX_SUPER_GROUND_MODE_NO];
	int aiItemRarityNo[MAX_SUPER_GROUND_MODE_NO];
	int aiItemEnchantControlNo[MAX_SUPER_GROUND_MODE_NO];
	int aiBossItemContainerNo[MAX_SUPER_GROUND_MODE_NO];

	tagSuperGround()
		: iGroundNo(0), iOrderNo(0)
	{
		//::ZeroMemory(aiMonsterEnchantGroupNo, sizeof(aiMonsterEnchantGroupNo));
		//::ZeroMemory(aiItemRarityNo, sizeof(aiItemRarityNo));
		//::ZeroMemory(aiItemEnchantControlNo, sizeof(aiItemEnchantControlNo));
		//::ZeroMemory(aiBossItemContainerNo, sizeof(aiBossItemContainerNo));
	}
	tagSuperGround(tagSuperGround const& rhs)
	{
		BM::Stream kTemp;
		rhs.WriteToPacket(kTemp);
		ReadFromPacket(kTemp);
	}
	~tagSuperGround()
	{
	}

	bool operator <(tagSuperGround const& rhs) const
	{
		return iGroundNo < rhs.iGroundNo;
	}
	DEFAULT_TBL_PACKET_FUNC();
} SSuperGround;
typedef std::vector< SSuperGround > CONT_SUPER_GROUND;
typedef struct tagTBL_DEF_SUPER_GROUND_GROUP
{
	tagTBL_DEF_SUPER_GROUND_GROUP()
		: kContSuperGround()
	{
	}

	CONT_SUPER_GROUND kContSuperGround;
	
	void WriteToPacket(BM::Stream &kPacket)const
	{
		PU::TWriteArray_M(kPacket, kContSuperGround);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		return PU::TLoadArray_M(kPacket, kContSuperGround);
	}
	size_t min_size()const
	{
		return sizeof(*this);
	}
} TBL_DEF_SUPER_GROUND_GROUP;

typedef struct tagTBL_DEF_NPC_TALK_MAP_MOVE
{
	tagTBL_DEF_NPC_TALK_MAP_MOVE()
		: kNpcGuid(), iTargetGroundNo(0), nTargetPortalNo(0)
	{
	}
	tagTBL_DEF_NPC_TALK_MAP_MOVE(BM::GUID const& rkNpcGuid, int const iTrgGroundNo, short nTrgPortalNo)
		: kNpcGuid(rkNpcGuid), iTargetGroundNo(iTrgGroundNo), nTargetPortalNo(nTrgPortalNo)
	{
	}
	tagTBL_DEF_NPC_TALK_MAP_MOVE(tagTBL_DEF_NPC_TALK_MAP_MOVE const& rhs)
		: kNpcGuid(rhs.kNpcGuid), iTargetGroundNo(rhs.iTargetGroundNo), nTargetPortalNo(rhs.nTargetPortalNo)
	{
	}
	~tagTBL_DEF_NPC_TALK_MAP_MOVE()
	{
	}

	BM::GUID kNpcGuid;
	int iTargetGroundNo;
	short nTargetPortalNo;

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_NPC_TALK_MAP_MOVE;
typedef std::list< TBL_DEF_NPC_TALK_MAP_MOVE > CONT_NPC_TALK_MAP_MOVE;

#pragma pack ()

typedef std::map< TBL_KEY_SHORT, TBL_DEF_GUILDLEVEL >			CONT_DEF_GUILD_LEVEL;
typedef std::map< TBL_KEY_INT, TBL_DEF_GUILDSKILL >				CONT_DEF_GUILD_SKILL;
typedef std::map< TBL_KEY_SHORT, TBL_DEF_TACTICS_LEVEL >		CONT_DEF_TACTICS_LEVEL;
typedef std::map< TBL_KEY_INT, TBL_DEF_TACTICS_QUEST_PSEUDO >	CONT_DEF_TACTICS_QUEST_PSEUDO;
typedef std::multimap< TBL_KEY_INT, SMonKillRewardItem >		CONT_DEF_MONSTER_KILL_COUNT_REWARD;
typedef std::list< SQuestResetSchedule >						CONT_DEF_QUEST_RESET_SCHEDULE;
typedef std::list< SFilterUnicde >								CONT_DEF_FILTER_UNICODE;
typedef std::map< TBL_KEY_INT, TBL_DEF_QUEST_RANDOM_EXP >		CONT_DEF_QUEST_RANDOM_EXP;
typedef std::map< TBL_KEY_INT, TBL_DEF_QUEST_RANDOM_TACTICS_EXP >		CONT_DEF_QUEST_RANDOM_TACTICS_EXP;
typedef std::list< TBL_DEF_EVENT_ITEM_SET >						CONT_DEF_EVENT_ITEM_SET;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER_ENCHANT_GRADE >		CONT_DEF_MONSTER_ENCHANT_GRADE;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER_ENCHANT_GRADE_PROBABILTY_GROUP >	CONT_DEF_MONSTER_ENCHANT_GRADE_PROBABILITY_GROUP;
typedef std::map< TBL_KEY_INT, TBL_DEF_SUPER_GROUND_GROUP >				CONT_DEF_SUPER_GROUND_GROUP;
typedef std::map< int, CONT_NPC_TALK_MAP_MOVE >					CONT_TBL_DEF_NPC_TALK_MAP_MOVE;

#pragma pack (1)
typedef struct tagReloadDef
{
	tagReloadDef()
	{
		pkContBaseCharacter = NULL;
		pkContDefRes = NULL;
		pContDefMonster = NULL;
		pContDefMonsterAbil = NULL;
		pContDefMonsterTunningAbil = NULL;
		pContDefSkill = NULL;
		pContDefSkillAbil = NULL;
		pContDefSkillSet = NULL;

		pContDefItem = NULL;
		pContDefItemAbil = NULL;
		pContDefRare = NULL;
		pContDefItemEnchant = NULL;

		pkContDefUpgradeClass = NULL;
		pContDefMonsterBag = NULL;
		pContDefMonsterBagControl = NULL;
		pkMonsterBagElements = NULL;

		pkDefClass = NULL;
		pkDefClassAbil = NULL;
		pkDefClassPet = NULL;
		pkDefClassPetLevel = NULL;
		pkDefClassPetSkill = NULL;
		pkDefClassPetAbil = NULL;

		pkItemBagElements = NULL;
		pkSuccessRateControl = NULL;
		pkCountControl = NULL;
		pkMoneyControl = NULL;
		pkItemBag = NULL;
		pkItemContainer = NULL;

		pkContMap = NULL;
		pkContMissionLevelRoot = NULL;
		pkContMissionCandidate = NULL;
		pkContDefenceAddMonster = NULL;
		
    	pkShopInEmporia = NULL;
		pkShopInGame = NULL;
		pkShopInStock = NULL;
		pkJobSkillShop = NULL;
		pkShopNpcGuid = NULL;

		pkDefChannelEffect = NULL;
		pkDefStrings = NULL;
		pkItemOption = NULL;
		pkItemOptionAbil = NULL;

		pkPvPGroundMode = NULL;
		pkPvPGroundGroup = NULL;
		pkPvPReward = NULL;
		pkPvPRewardItem = NULL;
		pkMissionResult = NULL;
		pkMissionCandi = NULL;
		pkMissionRoot = NULL;
		pkSpendMoney = NULL;

		pkRegenPPoint = NULL;
		pkItemSet = NULL;
		pkDynamicAbilRate = NULL;
		pkDynamicAbilRateBag = NULL;
		pkQuestReward = NULL;
		pkContDefEffect = NULL;
		pkContDefEffectAbil = NULL;
		pkDefItemMaking = NULL;
		pkDefResultControl = NULL;

		pkDefObject = NULL;
		pkDefObjectAbil = NULL;
		pkDefObjectBag = NULL;
		pkDefObjectBagElements = NULL;
		pkItemBagGroup = NULL;

		pkDefProperty = NULL;
		pkDefMapEffect = NULL;

		pkDefRecommendationItem = NULL;
		pkDefGroundRareMonster = NULL;
		pkDefRareMonsterSpeech = NULL;

		pkCardLocal = NULL;
		pkDefCardKeyString = NULL;
		pkCardAbil = NULL;
		pkGemStore = NULL;
		pkMonsterCard = NULL;
		pkMarryText = NULL;
		pkHiddenRewordItem = NULL;
		pkHiddenRewordBag = NULL;
		pkMissionClassReward = NULL;
		pkMissionRankReward = NULL;
		pkMissionDefenceStage = NULL;
		pkMissionDefenceWave = NULL;

		pkMissionDefence7Mission = NULL;
		pkMissionDefence7Stage = NULL;
		pkMissionDefence7Wave = NULL;
		pkMissionDefence7guardian = NULL;

		pkEmotion = NULL;
		pkEmotionGroup = NULL;
		pkCashItemAbilFilter = NULL;
		pkGamble = NULL;
		pkPCRoom = NULL;
		pkPCCafe = NULL;
		pkPCCafeAbil = NULL;
		pkDefConvertItem = NULL;
		pkDefGambleShoutItem = NULL;
		pkDefPetHatch = NULL;
		pkDefPetBonusStatus = NULL;
		pkCCERewardTable = NULL;
		pkRealtyDealer = NULL;
		pkRareOptSkill = NULL;
		pkRareOptMagic = NULL;
		pkIdx2SkillNo = NULL;
		pkMixupItem = NULL;
		pkTown2Ground = NULL;
		pkMyHomeDefaultItem = NULL;
		pkMyHomeTex = NULL;
		pkHometownToMapCost = NULL;

		pkDefItemEnchantAbilWeight = NULL;

		pkContMapItemBag = NULL;
		pkDefMap = NULL;
		pkDefMapAbil = NULL;
		pkSuperGroundGrp = NULL;
		pkMissionBonusMap = NULL;
		pkDefItemResConvert = NULL;
		pkDefGemStore = NULL;

		pkDefPvPLeagueTime = NULL;
		pkDefPvPLeagueSession = NULL;
		pkDefPvPLeagueReward = NULL;

		pkDefPremium = NULL;
		pkDefPremiumType = NULL;
	}

	// ; 뒤에 주석은 초기화 코드의 일괄 치환을 위해서 추가
	CONT_DEF_BASE_CHARACTER					const* pkContBaseCharacter;// = NULL;
	CONT_DEFRES								const* pkContDefRes;// = NULL;
	CONT_DEFMONSTER							const* pContDefMonster;// = NULL;
	CONT_DEFMONSTERABIL						const* pContDefMonsterAbil;// = NULL;
	CONT_DEFMONSTERTUNNINGABIL				const* pContDefMonsterTunningAbil;// = NULL;
	CONT_DEFSKILL							const* pContDefSkill;// = NULL;
	CONT_DEFSKILLABIL						const* pContDefSkillAbil;// = NULL;
	CONT_DEFSKILLSET						const* pContDefSkillSet;// = NULL;

	CONT_DEFITEM							const* pContDefItem;// = NULL;
	CONT_DEFITEMABIL						const* pContDefItemAbil;// = NULL;
	CONT_DEFITEMRARE						const* pContDefRare;// = NULL;
	CONT_DEFITEMENCHANT						const* pContDefItemEnchant;// = NULL;

	CONT_DEFUPGRADECLASS					const* pkContDefUpgradeClass;// = NULL;
	CONT_DEF_MONSTER_BAG					const* pContDefMonsterBag;// = NULL;
	CONT_DEF_MONSTER_BAG_CONTROL			const* pContDefMonsterBagControl;// = NULL;
	CONT_DEF_MONSTER_BAG_ELEMENTS			const* pkMonsterBagElements;// = NULL;

	CONT_DEFCLASS							const* pkDefClass;// = NULL;
	CONT_DEFCLASS_ABIL						const* pkDefClassAbil;// = NULL;
	CONT_DEFCLASS_PET						const* pkDefClassPet;// = NULL;
	CONT_DEFCLASS_PET_LEVEL					const* pkDefClassPetLevel;// = NULL;
	CONT_DEFCLASS_PET_SKILL					const* pkDefClassPetSkill;// = NULL;
	CONT_DEFCLASS_PET_ABIL					const* pkDefClassPetAbil;// = NULL;

	CONT_DEF_ITEM_BAG_ELEMENTS				const* pkItemBagElements;// = NULL;
	CONT_DEF_SUCCESS_RATE_CONTROL			const* pkSuccessRateControl;// = NULL;
	CONT_DEF_COUNT_CONTROL					const* pkCountControl;// = NULL;
	CONT_DEF_DROP_MONEY_CONTROL				const* pkMoneyControl;// = NULL;
	CONT_DEF_ITEM_BAG						const* pkItemBag;// = NULL;
	CONT_DEF_ITEM_CONTAINER					const* pkItemContainer;// = NULL;

	CONT_DEFMAP								const* pkContMap;// = NULL;
	CONT_DEF_MISSION_ROOT					const* pkContMissionLevelRoot;// = NULL;
	CONT_DEF_MISSION_CANDIDATE				const* pkContMissionCandidate;// = NULL;
	CONT_DEF_DEFENCE_ADD_MONSTER			const* pkContDefenceAddMonster;// = NULL;
	
    CONT_TBL_SHOP_IN_EMPORIA				const* pkShopInEmporia;// = NULL;
	CONT_TBL_SHOP_IN_GAME					const* pkShopInGame;// = NULL;
	CONT_TBL_SHOP_IN_STOCK					const* pkShopInStock;// = NULL;
	CONT_DEF_JOBSKILL_SHOP					const* pkJobSkillShop;// = NULL;
	CONT_SHOPNPC_GUID						const* pkShopNpcGuid;// = NULL;

	CONT_DEF_CHANNEL_EFFECT					const* pkDefChannelEffect;// = NULL;
	CONT_DEFSTRINGS							const* pkDefStrings;// = NULL;
	CONT_TBL_DEF_ITEM_OPTION				const* pkItemOption;// = NULL;
	CONT_TBL_DEF_ITEM_OPTION_ABIL			const* pkItemOptionAbil;// = NULL;

	CONT_DEF_PVP_GROUNDMODE					const* pkPvPGroundMode;// = NULL;
	CONT_DEF_PVP_GROUNDGROUP				const* pkPvPGroundGroup;// = NULL;
	CONT_DEF_PVP_REWARD						const* pkPvPReward;// = NULL;
	CONT_DEF_PVP_REWARD_ITEM				const* pkPvPRewardItem;// = NULL;
	CONT_DEF_MISSION_RESULT					const* pkMissionResult;// = NULL;
	CONT_DEF_MISSION_CANDIDATE				const* pkMissionCandi;// = NULL;
	CONT_DEF_MISSION_ROOT					const* pkMissionRoot;// = NULL;
	CONT_DEF_SPEND_MONEY					const* pkSpendMoney;// = NULL;

	CONT_DEF_MAP_REGEN_POINT				const* pkRegenPPoint;// = NULL;
	CONT_DEF_ITEM_SET						const* pkItemSet;// = NULL;
	CONT_DEFDYNAMICABILRATE					const* pkDynamicAbilRate;// = NULL;
	CONT_DEFDYNAMICABILRATE_BAG				const* pkDynamicAbilRateBag;// = NULL;
	CONT_DEF_QUEST_REWARD					const* pkQuestReward;// = NULL;
	CONT_DEFEFFECT							const* pkContDefEffect;// = NULL;;// = NULL;
	CONT_DEFEFFECTABIL						const* pkContDefEffectAbil;// = NULL;
	CONT_DEFITEMMAKING						const* pkDefItemMaking;// = NULL;
	CONT_DEFRESULT_CONTROL					const* pkDefResultControl;// = NULL;

	CONT_DEF_OBJECT							const* pkDefObject;// = NULL;
	CONT_DEF_OBJECTABIL						const* pkDefObjectAbil;// = NULL;
	CONT_DEF_OBJECT_BAG						const* pkDefObjectBag;// = NULL;
	CONT_DEF_OBJECT_BAG_ELEMENTS			const* pkDefObjectBagElements;// = NULL;
	CONT_DEF_ITEM_BAG_GROUP					const* pkItemBagGroup;// = NULL;

	CONT_DEF_PROPERTY						const* pkDefProperty;// = NULL;
	CONT_MAP_EFFECT							const* pkDefMapEffect;// = NULL;

	CONT_DEF_RECOMMENDATION_ITEM			const* pkDefRecommendationItem;// = NULL;
	CONT_DEF_GROUND_RARE_MONSTER			const* pkDefGroundRareMonster;// = NULL;
	CONT_DEF_RARE_MONSTER_SPEECH			const* pkDefRareMonsterSpeech;// = NULL;

	CONT_CARD_LOCAL							const* pkCardLocal;// = NULL;
	CONT_DEF_CARD_KEY_STRING				const* pkDefCardKeyString;// = NULL;
	CONT_CARD_ABIL							const* pkCardAbil;// = NULL;
	CONT_GEMSTORE							const* pkGemStore;// = NULL;
	CONT_MONSTERCARD						const* pkMonsterCard;// = NULL;
	CONT_MARRYTEXT							const* pkMarryText;// = NULL;
	CONT_HIDDENREWORDITEM					const* pkHiddenRewordItem;// = NULL;
	CONT_HIDDENREWORDBAG					const* pkHiddenRewordBag;// = NULL;
	CONT_MISSION_CLASS_REWARD_BAG			const* pkMissionClassReward;// = NULL;
	CONT_MISSION_RANK_REWARD_BAG			const* pkMissionRankReward;// = NULL;
	CONT_MISSION_DEFENCE_STAGE_BAG			const* pkMissionDefenceStage;// = NULL;
	CONT_MISSION_DEFENCE_WAVE_BAG			const* pkMissionDefenceWave;// = NULL;

	CONT_MISSION_DEFENCE7_MISSION_BAG		const* pkMissionDefence7Mission;// = NULL;
	CONT_MISSION_DEFENCE7_STAGE_BAG			const* pkMissionDefence7Stage;// = NULL;
	CONT_MISSION_DEFENCE7_WAVE_BAG			const* pkMissionDefence7Wave;// = NULL;
	CONT_MISSION_DEFENCE7_GUARDIAN_BAG		const* pkMissionDefence7guardian;// = NULL;

	CONT_EMOTION							const* pkEmotion;// = NULL;
	CONT_EMOTION_GROUP						const* pkEmotionGroup;// = NULL;
	CONT_CASHITEMABILFILTER					const* pkCashItemAbilFilter;// = NULL;
	CONT_GAMBLE								const* pkGamble;// = NULL;
	CONT_PCROOMIP							const* pkPCRoom;// = NULL;
	CONT_PCCAFE								const* pkPCCafe;// = NULL;
	CONT_PCCAFE_ABIL						const* pkPCCafeAbil;// = NULL;
	CONT_DEF_CONVERTITEM					const* pkDefConvertItem;// = NULL;
	CONT_GAMBLE_SHOUT_ITEM					const* pkDefGambleShoutItem;// = NULL;
	CONT_DEF_PET_HATCH						const* pkDefPetHatch;// = NULL;
	CONT_DEF_PET_BONUSSTATUS				const* pkDefPetBonusStatus;// = NULL;
	CONT_CCE_REWARD_TABLE					const* pkCCERewardTable;// = NULL;
	CONT_REALTYDEALER						const* pkRealtyDealer;// = NULL;
	CONT_RAREOPT_SKILL						const* pkRareOptSkill;// = NULL;
	CONT_RAREOPT_MAGIC						const* pkRareOptMagic;// = NULL;
	CONT_IDX2SKILLNO						const* pkIdx2SkillNo;// = NULL;
	CONT_MIXUPITEM							const* pkMixupItem;// = NULL;
	CONT_TOWN2GROUND						const* pkTown2Ground;// = NULL;
	CONT_MYHOME_DEFAULTITEM					const* pkMyHomeDefaultItem;// = NULL;
	CONT_MYHOME_TEX							const* pkMyHomeTex;// = NULL;
	CONT_HOMETOWNTOMAPCOST					const* pkHometownToMapCost;// = NULL;

	CONT_DEFITEMENCHANTABILWEIGHT			const* pkDefItemEnchantAbilWeight;// = NULL;

	CONT_DEF_MAP_ITEM_BAG					const* pkContMapItemBag;// = NULL;
	CONT_DEFMAP								const* pkDefMap;// = NULL;
	CONT_DEFMAP_ABIL						const* pkDefMapAbil;// = NULL;
	CONT_DEF_SUPER_GROUND_GROUP				const* pkSuperGroundGrp;// = NULL;
	CONT_MISSION_BONUSMAP					const* pkMissionBonusMap;// = NULL;
	CONT_DEF_ITEM_RES_CONVERT				const* pkDefItemResConvert;// = NULL;
	CONT_DEFGEMSTORE						const* pkDefGemStore;// = NULL;

	CONT_DEF_PVPLEAGUE_TIME					const* pkDefPvPLeagueTime;// = NULL;
	CONT_DEF_PVPLEAGUE_SESSION				const* pkDefPvPLeagueSession;// = NULL;
	CONT_DEF_PVPLEAGUE_REWARD				const* pkDefPvPLeagueReward;// = NULL;

	CONT_DEF_PREMIUM_SERVICE				const* pkDefPremium;// = NULL;
	CONT_DEF_PREMIUM_ARTICLE				const* pkDefPremiumType;// = NULL;

	GroundArr kGroundArray;

	CObjLockWrapper kLockWrapper;
}SReloadDef;

#pragma pack ()

#endif // WEAPON_LOHENGRIN_DB_DBTABLES2_H