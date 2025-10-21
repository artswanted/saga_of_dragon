#ifndef WEAPON_LOHENGRIN_DB_DBTABLES_H
#define WEAPON_LOHENGRIN_DB_DBTABLES_H

#include <map>

#include "bm/Guid.h"
#include "BM/Approximate.h"
#include "cel/query.h"
#include "DataPack/Packinfo.h"
#include "Lohengrin/PacketStruct.h"

#pragma pack (1)

#define TBL_KEY_INT			tagTBL_KEY<int>
#define TBL_KEY_SHORT		tagTBL_KEY<short>
#define TBL_KEY_UINT		tagTBL_KEY<unsigned int>

#define TBL_PAIR_KEY_INT		tagPairKey< int, int >
#define TBL_PAIR_KEY_INT64		tagPairKey< __int64, __int64 >
#define TBL_TRIPLE_KEY_INT		tagTripleKey< int, int, int >
#define TBL_QUAD_KEY_INT		tagQuadKey< int, int, int, int >

typedef struct tagTBL_KEY_GUID
	:public BM::GUID
{
	tagTBL_KEY_GUID(BM::GUID const &kInitvalue = BM::GUID::NullData())
	{
		BM::GUID::operator = (kInitvalue);
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_KEY_GUID;

int const MAX_CLASS_ABIL_LIST = 10;
typedef struct tagTblDefClass
{
	tagTblDefClass()
		:	iClass(0)
		,	sLevel(0)
		,	i64Experience(0i64)
		,	sMaxHP(0)
		,	sHPRecoveryInterval(0)
		,	sHPRecovery(0)
// 		,	sMaxDP(0)
// 		,	sDPRecoveryInterval(0)
// 		,	sDPRecovery(0)
		,	sMaxMP(0)
		,	sMPRecoveryInterval(0)
		,	sMPRecovery(0)
		,	iStr(0)
		,	iInt(0)
		,	iCon(0)
		,	iDex(0)
		,	sMoveSpeed(0)
		, sBonusStatus(0), sLimitStr(0), sLimitInt(0), sLimitCon(0), sLimitDex(0)

	{
		::memset(aiAbil, 0, sizeof(aiAbil));
	}

	int iClass;
	unsigned short  sLevel;
	__int64 i64Experience;
	unsigned short	sMaxHP;
	unsigned short  sHPRecoveryInterval;
	unsigned short  sHPRecovery;
// 	unsigned short	sMaxDP;
// 	unsigned short  sDPRecoveryInterval;
// 	unsigned short  sDPRecovery;
	unsigned short  sMaxMP;
	unsigned short  sMPRecoveryInterval;
	unsigned short  sMPRecovery;
	unsigned short  iStr;
	unsigned short  iInt;
	unsigned short  iCon;
	unsigned short  iDex;
	unsigned short  sMoveSpeed;
	int aiAbil[MAX_CLASS_ABIL_LIST];
	// TB_DefCalss_Bonus
	unsigned short sBonusStatus;
	unsigned short sLimitStr;
	unsigned short sLimitInt;
	unsigned short sLimitCon;
	unsigned short sLimitDex;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS;

int const MAX_CLASS_ABIL_ARRAY = 10;
typedef struct tagTBL_DEF_CLASS_ABIL
{
	tagTBL_DEF_CLASS_ABIL()
		:	iAbilNo(0)
	{
		memset(iType, 0, sizeof(iType));
		memset(iValue, 0, sizeof(iValue));
	}

	int iAbilNo;
	int iType[MAX_CLASS_ABIL_ARRAY];
	int iValue[MAX_CLASS_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_ABIL;

typedef struct tagTblDefClassPet
{
	tagTblDefClassPet()
		:	iClass(0)
		,	iClassName(0)
		,	byPetType(0)
		,	iLevelIndex(0)
		,	iSkillIndex(0)
		,	iItemOptionIndex(0)
		,	iDefaultHair(0)
		,	iDefaultFace(0)
		,	iDefaultBody(0)
	{}

	int			iClass;
	int			iClassName;
	BYTE		byPetType;
	int			iLevelIndex;
	int			iSkillIndex;
	int			iItemOptionIndex;
	int			iDefaultHair;
	int			iDefaultFace;
	int			iDefaultBody;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_PET;

typedef struct tagTblDefClassPetLevelBase
{
	tagTblDefClassPetLevelBase()
		:	sLevel(0)
		,	i64Experience(0i64)
		,	iTimeExperience(0)
		,	sMaxMP(0)
		,	sMPRecoveryInterval(0)
		,	sMPRecovery(0)
	{
	}

	short		sLevel;
	__int64		i64Experience;
	int			iTimeExperience;
	short		sMaxMP;
	short		sMPRecoveryInterval;
	short		sMPRecovery;

}TBL_DEF_CLASS_PET_LEVEL_BASE;

typedef struct tagTblDefClassPetLevel
:	public TBL_DEF_CLASS_PET_LEVEL_BASE
{
	tagTblDefClassPetLevel()
	{
		::memset(aiAbil, 0, sizeof(aiAbil));
	}

	int aiAbil[MAX_CLASS_ABIL_LIST];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_PET_LEVEL;

static size_t const MAX_PET_SKILLCOUNT = 18;
typedef struct tagTblDefClassPetSkill
{
	tagTblDefClassPetSkill()
		:	nLevel(0)
		,	iSkillUseTimeBit(0)
	{
		::memset( iSkillNo, 0, sizeof(iSkillNo) );
	}

	bool operator < ( tagTblDefClassPetSkill const & rhs )const{return (nLevel < rhs.nLevel);}
	bool operator > ( tagTblDefClassPetSkill const & rhs )const{return (rhs < *this);}
	bool operator <= ( tagTblDefClassPetSkill const & rhs )const{return ((*this < rhs) || (*this == rhs));}
	bool operator >= ( tagTblDefClassPetSkill const & rhs )const{return (rhs <= *this);}
	bool operator == ( tagTblDefClassPetSkill const & rhs )const{return (nLevel == rhs.nLevel);}

	short nLevel;
	int iSkillNo[MAX_PET_SKILLCOUNT];
	int iSkillUseTimeBit;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_PET_SKILL;

class PgDefClassPetSkillFinder
{
public:
	explicit PgDefClassPetSkillFinder( short const nLevel ):m_nLevel(nLevel){}
	~PgDefClassPetSkillFinder(){}

	bool operator > ( tagTblDefClassPetSkill const &rhs )const{return m_nLevel > rhs.nLevel;}
	bool operator < ( tagTblDefClassPetSkill const &rhs )const{return m_nLevel < rhs.nLevel;}

private:
	short const m_nLevel;
};

static size_t const MAX_PET_ITEMOPTION_COUNT = 4;
typedef struct tagTblDefClassPetItemOption
{
	tagTblDefClassPetItemOption()
	{
		::memset( iOptionType, 0, sizeof(iOptionType) );
		::memset( iOptionLevel, 0, sizeof(iOptionLevel) );
	}

	int	iOptionType[MAX_PET_ITEMOPTION_COUNT];
	int	iOptionLevel[MAX_PET_ITEMOPTION_COUNT];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_PET_ITEMOPTION;

typedef struct tagTBL_DEF_CLASS_PET_ABIL
{
	tagTBL_DEF_CLASS_PET_ABIL()
		:	iAbilNo(0)
	{
		memset(iType, 0, sizeof(iType));
		memset(iValue, 0, sizeof(iValue));
	}

	int iAbilNo;
	int iType[MAX_CLASS_ABIL_ARRAY];
	int iValue[MAX_CLASS_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CLASS_PET_ABIL;

typedef struct tagTblDefAIPattern
{
	tagTblDefAIPattern()
	{
		AINo = 0;
	}

	int AINo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_AIPATTERN;

typedef struct tagTBL_DEF_BASE_CHARACTER
{
	tagTBL_DEF_BASE_CHARACTER()
	{
		f_Class = 0;//key
		f_State = 0;
		f_GMLevel = 0;
		f_Race = 0;	
		f_Lv = 0;	
		f_Exp = 0;	
		f_Money = 0;
		f_HP = 0;
		f_MP = 0;
		f_CP = 0;
		f_ExtraSkillPoint = 0;
		memset(f_Skills, 0, sizeof(f_Skills));
		f_RecentMap = 0;	
		f_RecentLocX = 0;	
		f_RecentLocY = 0;
		f_RecentLocZ = 0;
		f_LastVillage = 0;
		f_LastVillageLocX = 0;
		f_LastVillageLocY = 0;
		f_LastVillageLocZ = 0;
		memset(f_QuickSlot, 0, sizeof(f_QuickSlot));
		memset(f_IngQuest, 0, sizeof(f_IngQuest));
		memset(f_EndQuest, 0, sizeof(f_EndQuest));
		memset(f_EndQuest2, 0, sizeof(f_EndQuest2));
		memset(f_EndQuestExt, 0, sizeof(f_EndQuestExt));
		memset(f_EndQuestExt2, 0, sizeof(f_EndQuestExt2));
	}

	BYTE f_Class;//key
	BYTE f_State;
	BYTE f_GMLevel;
	int f_Race;	
	short f_Lv;	
	__int64 f_Exp;	
	__int64 f_Money;
	int f_HP;
	int f_MP;
	int f_CP;
	short f_ExtraSkillPoint;
	BYTE f_Skills[MAX_DB_SKILL_SIZE];
	int	f_RecentMap;	
	double f_RecentLocX;	
	double f_RecentLocY;
	double f_RecentLocZ;
	int	f_LastVillage;
	double f_LastVillageLocX;
	double f_LastVillageLocY;
	double f_LastVillageLocZ;
	BYTE f_QuickSlot[MAX_DB_QUICKSLOT_SIZE];
	BYTE f_IngQuest[MAX_DB_INGQUEST_SIZE];
	BYTE f_EndQuest[MAX_DB_ENDQUEST_SIZE];
	BYTE f_EndQuest2[MAX_DB_ENDQUEST_SIZE];
	BYTE f_EndQuestExt[MAX_DB_ENDQUEST_EXT_SIZE];
	BYTE f_EndQuestExt2[MAX_DB_ENDQUEST_EXT2_SIZE];
	BYTE f_InvExtend[MAX_DB_INVEXTEND_SIZE];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_BASE_CHARACTER;

typedef struct tagTblDefAbilType
{
	tagTblDefAbilType()
	{
		AbilNo = 0;
		NameNo = 0;
	}

	int AbilNo;
	int NameNo;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ABILTYPE;

int const MAX_ITEM_ABIL_LIST = 10;
typedef struct tagTblDefItem
{
	tagTblDefItem()
	{
		iCostumeGrade = 0;
		iOrder1 = 0;
		iOrder2 = 0;
		iOrder3 = 0;
		ItemNo = 0;
		NameNo = 0;
		ResNo = 0;
		sType = 0;
		iPrice = 0;
		iSellPrice = 0;
		iAttribute = 0;
		byGender = 0;
		sLevel = 0;
		i64ClassLimit = 0;
		i64ClassLimitDisplayFilter = 0;
		i64DraClassLimit = 0;
		i64DraClassLimitDisplayFilter = 0;
		memset(aAbil, 0, sizeof(aAbil));// AbilNo
	}
	
	int ItemNo;
	int NameNo;
	int ResNo;
	short sType;
	int iPrice;
	int iSellPrice;
	int iAttribute;
	
	int iOrder1,
		iOrder2,
		iOrder3,
		iCostumeGrade;

	BYTE byGender;
	short int sLevel;
	__int64 i64ClassLimit;
	__int64 i64ClassLimitDisplayFilter;
	__int64 i64DraClassLimit;
	__int64 i64DraClassLimitDisplayFilter;
	int aAbil[MAX_ITEM_ABIL_LIST];// AbilNo

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM;

typedef struct tagTblDefItemResConvert
{
	tagTblDefItemResConvert()
	{
		ItemNo = 0;
		NationCode = 0;
		ResNo = 0;
	}
	
	int ItemNo;
	int NationCode;
	int ResNo;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_RES_CONVERT;

int const MAX_ITEM_ABIL_ARRAY = 10;
typedef struct tagTblDefItemAbil
{
	tagTblDefItemAbil()
	{
		ItemAbilNo = 0;
		memset(aType, 0, sizeof(aType));
		memset(aValue, 0, sizeof(aValue));
	}

	int ItemAbilNo;// AbilNo
	int aType[MAX_ITEM_ABIL_ARRAY];
	int aValue[MAX_ITEM_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMABIL;

typedef struct tagTblDefItemRare
{
	tagTblDefItemRare()
	{
		RareNo = 0;
		NameNo = 0;
	}

	int RareNo;
	int NameNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMRARE;

int const MAX_ITEM_RARE_KIND_ARRAY = 10;
typedef struct tagTblDefItemRareGroup
{
	tagTblDefItemRareGroup()
	{
		GroupNo = 0;
		NameNo = 0;
		memset(aRareNo, 0, sizeof(aRareNo));
	}
	int GroupNo;
	int NameNo;
	int aRareNo[MAX_ITEM_RARE_KIND_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMRAREGROUP;

int const MAX_ITEM_SET_ABIL = 2;
int const MAX_ITEM_SET_NEED_ITEM_ARRAY = 10;
typedef struct tagTblDefItemSet
{
	tagTblDefItemSet()
	{
		iSetNo = 0;
		iNameNo = 0;
		memset(aiAbilNo, 0, sizeof(aiAbilNo));
		memset(aiNeedItemNo, 0, sizeof(aiNeedItemNo));
	}
	int iSetNo;
	int iNameNo;
	int aiAbilNo[MAX_ITEM_SET_ABIL];
	int aiNeedItemNo[MAX_ITEM_SET_NEED_ITEM_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_SET;

typedef std::vector<int>	CONT_NEED_KEY_ITEM;
typedef std::vector<int>	CONT_MISSION_NO;
int const MAX_MAP_ABIL_COUNT = 5;
typedef struct tagTblDefMap
{
	tagTblDefMap()
		: byRecommend_Level(0)
		, byRecommend_Party(0)
		, iGroupTextNo(0)
		, byLevelLimit_Min(0)
		, byLevelLimit_Max(0)
		, iDescriptionTextNo(0)
		, byPartyBreakIn(0)
	{
		iMapNo = 0;
		iKey = 0;
		iAttr = 0;
		NameNo = 0;

		sZoneCX = 0;
		sZoneCY = 0;
		sZoneCZ = 0;
		sContinent = 0;
		sHometownNo = 0;
		sHiddenIndex = 0;
	}

	int iMapNo;
	int iKey;
	int iAttr;
	int NameNo;

	short int sZoneCX;
	short int sZoneCY;
	short int sZoneCZ;
	short int sContinent;
	short int sHometownNo;
	short int sHiddenIndex;
	std::wstring strXMLPath;
	int iAbil[MAX_MAP_ABIL_COUNT];

	BYTE byRecommend_Level;
	BYTE byRecommend_Party;
	int iGroupTextNo;
	BYTE byLevelLimit_Min;
	BYTE byLevelLimit_Max;
	int iDescriptionTextNo;
	BYTE byPartyBreakIn;
	
	CONT_NEED_KEY_ITEM ContItemNo;
	CONT_NEED_KEY_ITEM ContCashItemNo;

	CONT_MISSION_NO	kContMissionNo;
	CONT_MISSION_NO	ContDungeonNo;

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+

			sizeof(short int)+
			sizeof(short int)+
			sizeof(short int)+
			sizeof(short int)+
			sizeof(short int)+
			sizeof(short int)+
			
			sizeof(size_t)+//strXMLPath
			sizeof(iAbil)+
			sizeof(BYTE)+
			sizeof(BYTE)+
			sizeof(int)+
			sizeof(BYTE)+
			sizeof(BYTE)+
			sizeof(int)+
			sizeof(size_t)+//ContItemNo
			sizeof(size_t)+//ContCashItemNo
			sizeof(size_t)+//kContMissionNo
			sizeof(size_t);//ContDungeonNo
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(iMapNo);
		kPacket.Push(iKey);
		kPacket.Push(iAttr);
		kPacket.Push(NameNo);
		
		kPacket.Push(sZoneCX);
		kPacket.Push(sZoneCY);
		kPacket.Push(sZoneCZ);
		
		kPacket.Push(sContinent);
		kPacket.Push(sHometownNo);
		kPacket.Push(sHiddenIndex);

		kPacket.Push(strXMLPath);
		kPacket.Push(iAbil, sizeof(iAbil));
		kPacket.Push(byRecommend_Level);
		kPacket.Push(byRecommend_Party);
		kPacket.Push(iGroupTextNo);
		kPacket.Push(byLevelLimit_Min);
		kPacket.Push(byLevelLimit_Max);
		kPacket.Push(iDescriptionTextNo);
		kPacket.Push(byPartyBreakIn);
		kPacket.Push(ContItemNo);
		kPacket.Push(ContCashItemNo);
		kPacket.Push(kContMissionNo);
		kPacket.Push(ContDungeonNo);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iMapNo);
		kPacket.Pop(iKey);
		kPacket.Pop(iAttr);
		kPacket.Pop(NameNo);
		
		kPacket.Pop(sZoneCX);
		kPacket.Pop(sZoneCY);
		kPacket.Pop(sZoneCZ);
		
		kPacket.Pop(sContinent);
		kPacket.Pop(sHometownNo);
		kPacket.Pop(sHiddenIndex);

		kPacket.Pop(strXMLPath);
		kPacket.PopMemory(iAbil, sizeof(iAbil));
		kPacket.Pop(byRecommend_Level);
		kPacket.Pop(byRecommend_Party);
		kPacket.Pop(iGroupTextNo);
		kPacket.Pop(byLevelLimit_Min);
		kPacket.Pop(byLevelLimit_Max);
		kPacket.Pop(iDescriptionTextNo);
		kPacket.Pop(byPartyBreakIn);
		kPacket.Pop(ContItemNo);
		kPacket.Pop(ContCashItemNo);
		kPacket.Pop(kContMissionNo);
		kPacket.Pop(ContDungeonNo);
	}
}TBL_DEF_MAP;

int const MAX_MAP_ABIL = 10;
typedef struct tagTblDefMapAbil
{
	int iAbilNo;
	int iType[MAX_MAP_ABIL];
	int iValue[MAX_MAP_ABIL];
	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_MAP_ABIL;

typedef struct tagTBL_DEF_REVIVEPOS
{
	tagTBL_DEF_REVIVEPOS()
	{
		iFromMapNo = 0;
		iToMapNo = 0;
	}
	int iFromMapNo;
	int iToMapNo;
	POINT3 ptToPos;
	POINT3I sizePos;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_REVIVEPOS;

typedef struct tagTblDefMapNPC
{
	tagTblDefMapNPC()
	{
		Name = 0;
		NPCNo = 0;
		MapNo = 0;
	}
	BM::GUID	NPCID;
	int	Name;
	int	NPCNo;
	int	MapNo;
	POINT3 ptPos;
}TBL_DEF_MAPNPC;

typedef struct tagTblDefMapItem
{
	tagTblDefMapItem()
	{
		MapNo = 0;
		ItemNo = 0;
	}
	BM::GUID	PosID;
	int	MapNo;
	int	ItemNo;
	POINT3 ptPos;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MAPITEM;

//int const MAX_POINT_ITEM_BAG_COUNT = 3;
typedef struct tagTblMapRegenPoint
{
	tagTblMapRegenPoint()
	{
		Clear();
	}

	int iMapNo;
	BYTE cBagControlType;
	int iBagControlNo;
	BM::GUID kPosGuid;
	int iPointGroup;	// Regen Group (Mission�ʿ��� Section���ҿ����� ���)
	DWORD dwPeriod;
	POINT3 pt3Pos;
	int iMoveRange;
	BYTE cDirection;
	int iTunningNo;

	void Clear()
	{	
		iMapNo = 0;
		cBagControlType = 0;
		iBagControlNo = 0;
		kPosGuid.Clear();
		dwPeriod = 0;
		pt3Pos.Clear();
		iMoveRange = 0;
		cDirection = DEFAULT_MONSTER_INFO::REGEN_DIRECTION_NOSET;
		iTunningNo = 0;
	}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MAP_REGEN_POINT;

int const MAX_MONSTER_ABIL_LIST = 15;
typedef struct tagTblDefMonster
{
	tagTblDefMonster()
	{
		MonsterNo = 0;
		Name = 0;
		memset(aAbil, 0, sizeof(aAbil));
	}

	int	MonsterNo;
	int	Name;
	int	aAbil[MAX_MONSTER_ABIL_LIST];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTER;

int const MAX_MONSTER_ABIL_ARRAY = 10;
typedef struct tagTblDefMonsterAbil
{
	tagTblDefMonsterAbil()
	{
		MonsterAbilNo = 0;
		memset(	aType, 0, sizeof(aType));
		memset(	aValue, 0, sizeof(aValue));			
	}
	int MonsterAbilNo;
	int aType[MAX_MONSTER_ABIL_ARRAY];
	int aValue[MAX_MONSTER_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTERABIL;

typedef struct tagMonsterTunningKey
{
	tagMonsterTunningKey(int const inNo=0, int const inGrade=0, int const inLv=0)
		: iNo(inNo), iGrade(inGrade), iLevel(inLv)
	{
	}

	bool operator < (const tagMonsterTunningKey &rhs)const
	{
		if( iNo < rhs.iNo )	{return true;}
		if( iNo > rhs.iNo )	{return false;}

		if( iGrade < rhs.iGrade )	{return true;}
		if( iGrade > rhs.iGrade )	{return false;}

		if( iLevel < rhs.iLevel )	{return false;}
		if( iLevel > rhs.iLevel )	{return true;}

		return false;
	}

	int iNo;
	int iGrade;
	int iLevel;
	DEFAULT_TBL_PACKET_FUNC();
}SMonsterTunningKey;
typedef struct tagTblDefMonsterTunning
{
	tagTblDefMonsterTunning()
	{
		memset(	iAbil, 0, sizeof(iAbil));
	}
	SMonsterTunningKey kKey;
	int iAbil[MAX_MONSTER_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTERTUNNING;

int const MAX_NPC_ABIL_LIST = 10;
typedef struct tagTblDefNPC
{
	tagTblDefNPC()
	{
		NPCNo = 0;
		Name = 0;
		memset(aAbil, 0, sizeof(aAbil));
	}
	int	NPCNo;
	int	Name;
	int	aAbil[MAX_NPC_ABIL_LIST];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_NPC;

int const MAX_NPC_ABIL_ARRAY = 10;
typedef struct tagTblDefNPCAbil
{
	tagTblDefNPCAbil()
	{
		NPCAbilNo = 0;
		memset(aType, 0, sizeof(aType));
		memset(aValue, 0, sizeof(aValue));
	}
	int NPCAbilNo;
	int aType[MAX_NPC_ABIL_ARRAY];
	int aValue[MAX_NPC_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_NPCABIL;

int const MAX_DYNAMICABILRATE_LIST = 10;
typedef struct tagTblDefDynamicAbilRateBag
{
	tagTblDefDynamicAbilRateBag()
	{
		BagID = 0;
		memset(aAbil, 0, sizeof(aAbil));
	}
	int BagID;
	int aAbil[MAX_DYNAMICABILRATE_LIST];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_DYNAMICABILRATE_BAG;

int const MAX_DYNAMICABILRATE_ARRAY = 10;
typedef struct tagTblDefDynamicAbilRateAbil
{
	tagTblDefDynamicAbilRateAbil()
	{
		AbilRateNo = 0;
		memset(aType, 0, sizeof(aType));
		memset(aValue, 0, sizeof(aValue));
	}
	int AbilRateNo;
	int aType[MAX_DYNAMICABILRATE_ARRAY];
	int aValue[MAX_DYNAMICABILRATE_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_DYNAMICABILRATE;

typedef struct tagTblDefShop
{
	tagTblDefShop()
	{
		ShopNo = 0;
		ItemNo = 0;
	}
	int	ShopNo;
	int	ItemNo;
}TBL_DEF_SHOP;

int const MAX_SKILL_ABIL_LIST = 10;
typedef struct tagTblDefSkill
{
	tagTblDefSkill()
	{
		SkillNo = 0;
		Lv = 0;
		NameNo = 0;
		RscNameNo = 0;
		iActionCategoryNo = 0;
		memset(chActionName, 0, sizeof(chActionName));
		byType = 0;
		i64ClassLimit = 0;
		i64DraClassLimit = 0;
		sLevelLimit = 0;
		iWeaponLimit = 0;
		iStateLimit = 0;
		iParentSkill = 0;
		iTarget = 0;
		sRange = 0;
		sCastTime = 0;
		sCoolTime = 0;
		iAnimationTime = 0;
		sMP = 0;
		sHP = 0;
		iEffectID = 0;
		iCmdStringNo = 0;
		memset(aAbil, 0, sizeof(aAbil));
	}
	
	int	SkillNo;
	BYTE	Lv;
	int	NameNo;
	int	RscNameNo;
	int iActionCategoryNo;
	TCHAR chActionName[30];
	BYTE byType;
	__int64 i64ClassLimit;
	__int64 i64DraClassLimit;
	short int sLevelLimit;
	int iWeaponLimit;
	int iStateLimit;
	int iParentSkill;
	int iTarget;
	short int sRange;
	int sCastTime;
	int sCoolTime;
	int iAnimationTime;
	short int sMP;
	short int sHP;
	int iEffectID;
	int	iCmdStringNo;
	int	aAbil[MAX_SKILL_ABIL_LIST];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SKILL;

typedef struct tagSkillKey
{
	tagSkillKey()
	{
		SkillNo = 0;
		Lv = 0;
	}

	tagSkillKey(int const SkillNo_, BYTE const Lv_)
	{
		SkillNo = SkillNo_;
		Lv = Lv_;
	}
	
	int		SkillNo;
	BYTE	Lv;

	bool operator < (const tagSkillKey &rhs)const
	{
		if( SkillNo < rhs.SkillNo )	{return true;}
		if( SkillNo > rhs.SkillNo )	{return false;}

		if( Lv < rhs.Lv )	{return true;}
		if( Lv > rhs.Lv )	{return false;}

		return false;
	}

}SSKILL_KEY;

int const MAX_SKILL_ABIL_ARRAY = 10;
typedef struct tagTblDefSkillAbil
{
	tagTblDefSkillAbil()
	{
		SkillAbilNo = 0;

		memset(aType, 0, sizeof(aType));
		memset(aValue, 0, sizeof(aValue));
	}

	int	SkillAbilNo;
	int aType[MAX_SKILL_ABIL_ARRAY];
	int aValue[MAX_SKILL_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SKILLABIL;

typedef enum eSkillSetConditionType : BYTE
{
	SSCT_NONE	= 0,
	SSCT_CLASS	= 1,
	SSCT_ITEM	= 2,
	SSCT_LEVEL	= 3,//���� ����, ���� ���� ���Ŀ��� �������� ���ؼ� Ȱ��ȭ ����
};

typedef struct tagTblDefSkillSet
{
	tagTblDefSkillSet()
		: bySetNo(0)
		, byLimit(0)
		, byConditionType(0)
		, byConditionValue(0)
		, iResNo(0)
	{
	}

	bool operator < (const tagTblDefSkillSet &rhs)const
	{
		if( bySetNo < rhs.bySetNo )	{return true;}

		return false;
	}

	BYTE bySetNo;
	BYTE byLimit;
	BYTE byConditionType;	//eSkillSetConditionType
	BYTE byConditionValue;
	int	 iResNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SKILLSET;

typedef struct tagTblDefRes
{
	tagTblDefRes()
	{
		ResNo = 0;
		U = 0;
		V = 0;
		UVIndex = 0;

		AddResNo[0] = 0;
		AddResNo[1] = 0;
		AddResNo[2] = 0;
	}

	int	ResNo;
	std::wstring strIconPath;
	std::wstring strXmlPath;
	int U;
	int V;
	int UVIndex;

	int AddResNo[3];
	
	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(size_t)+//strIconPath
			sizeof(size_t)+//strXmlPath
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			(sizeof(int) * 3);
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(ResNo);
		
		kPacket.Push(strIconPath);
		kPacket.Push(strXmlPath);

		kPacket.Push(U);
		kPacket.Push(V);
		kPacket.Push(UVIndex);		

		kPacket.Push(AddResNo[0]);		
		kPacket.Push(AddResNo[1]);		
		kPacket.Push(AddResNo[2]);		
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(ResNo);
		
		kPacket.Pop(strIconPath);
		kPacket.Pop(strXmlPath);

		kPacket.Pop(U);
		kPacket.Pop(V);
		kPacket.Pop(UVIndex);		

		kPacket.Pop(AddResNo[0]);		
		kPacket.Pop(AddResNo[1]);		
		kPacket.Pop(AddResNo[2]);		
	}
}TBL_DEF_RES;

typedef struct tagTblDefString
{
	tagTblDefString()
	{
		StringNo =0;
	}
	int	StringNo;
	std::wstring strText;
	std::wstring strTextEng;
	
	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(size_t)+//strText
			sizeof(size_t);//strTextEng
	}
	
	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(StringNo);
		
		kPacket.Push(strText);
		kPacket.Push(strTextEng);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(StringNo);
		
		kPacket.Pop(strText);
		kPacket.Pop(strTextEng);
	}
}TBL_DEF_STRING;

typedef struct tagTBL_CHANNEL_EFFECT
{
	tagTBL_CHANNEL_EFFECT()
		:iEffectNo(0), sRealm(0), sChannel(0), sMinLv(0), sMaxLv(0)
	{		
	}

	int iEffectNo;
	short sRealm;
	short sChannel;
	short sMinLv;
	short sMaxLv;

	DEFAULT_TBL_PACKET_FUNC();

} TBL_CHANNEL_EFFECT;

typedef struct tagTBLN_DEF_ITEMGROUP
{
	tagTBLN_DEF_ITEMGROUP()
	{
		iItemNo=0;
		sProbability=0;
	}
	int iItemNo;
	short int sProbability;
}TBLN_DEF_ITEMGROUP;//�̸��� TBL_DEF_ITEMGROUP �� �ٲ��� ����.

typedef struct tagTblDefItemGroup
{
	tagTblDefItemGroup()
	{
		iGroupNo = 0;
		iProbabilitySum = 0;
	}
	int iGroupNo;
	int iProbabilitySum;
	std::vector<TBLN_DEF_ITEMGROUP> vkItem;
} TBL_DEF_ITEMGROUP;

int const MAX_ITEMMAKING_ARRAY = 10;
typedef struct tagTblDefItemMaking
{
	tagTblDefItemMaking()
	{
		iNo = 0;
		iTypeNo = 0;
		iContentStringNo = 0;
		iNeedMoney = 0;
		iNeedElementsNo = 0;
		iNeedCountControlNo = 0;
		iSuccesRateControlNo = 0;
		iResultControlNo = 0;
		iRarityControlNo = 0;

		for (int i=0 ; i<MAX_ITEMMAKING_ARRAY ; i++)
		{
			aiElements[i] = 0;
			aiCountControl[i] = 0;
		}
	}
	int iNo;
	int iTypeNo;
	int iContentStringNo;
	int iNeedMoney;
	int iNeedElementsNo;
	int iNeedCountControlNo;
	int iSuccesRateControlNo;
	int iResultControlNo;
	int iRarityControlNo;
	int aiElements[MAX_ITEMMAKING_ARRAY];
	int aiCountControl[MAX_ITEMMAKING_ARRAY];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMMAKING;


typedef struct tagTblDefCooking
{
	static int const MAX_NEEDITEM = 4;
	static int const MAX_RESULT = 3;

	struct SNeedItem {
		SNeedItem() : iItemNo(0), iCount(0) {}
		int iItemNo;
		int iCount;
	};

	struct SResult {
		SResult() : iItemNo(0), iCount(0), iRate(0), iOptionRate(0) {}
		int iItemNo;
		int iCount;
		int iRate;
		int iOptionRate;
	};

	tagTblDefCooking()
		: iNo(0)
		, iTypeNo(0)
		, iNeedMoney(0)
	{
	}

	int iNo;
	int iTypeNo;
	int iNeedMoney;
	SNeedItem kNeedItem[MAX_NEEDITEM];
	SNeedItem kOptionItem;
	SResult kResult[MAX_RESULT];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_COOKING;

int const MAX_RESULT_ARRAY = 10;
typedef struct tagTBL_DEF_RESULT_CONTROL
{
	tagTBL_DEF_RESULT_CONTROL()
	{
		iNo = 0;
		for (int i=0 ; i<MAX_RESULT_ARRAY ; i++)
		{
			aiResult[i] = 0;
		}
	}
	int iNo;
	int aiResult[MAX_RESULT_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_RESULT_CONTROL;

int const MAX_EFFECT_ABIL_LIST = 10;
typedef struct tagTBL_DEF_EFFECT
{
	tagTBL_DEF_EFFECT()
	{
		iEffectID = 0;
		iName = 0;
		
		sType = 0;
		sInterval = 0;
		iDurationTime = 0;
		byToggle = 0;
		memset(aiAbil, 0, sizeof(aiAbil));
	}

	int iEffectID;
	int iName;
	std::wstring strActionName;
	short int sType;
	short int sInterval;
	int iDurationTime;
	BYTE byToggle;
	int aiAbil[MAX_EFFECT_ABIL_LIST];

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t)+//strActionName
			sizeof(short int)+
			sizeof(short int)+
			sizeof(int)+
			sizeof(BYTE)+
			(sizeof(int)*MAX_EFFECT_ABIL_LIST);
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(iEffectID);
		kPacket.Push(iName);
		
		kPacket.Push(strActionName);

		kPacket.Push(sType);
		kPacket.Push(sInterval);
		
		kPacket.Push(iDurationTime);
		kPacket.Push(byToggle);

		kPacket.Push(aiAbil);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iEffectID);
		kPacket.Pop(iName);
		
		kPacket.Pop(strActionName);

		kPacket.Pop(sType);
		kPacket.Pop(sInterval);
		
		kPacket.Pop(iDurationTime);
		kPacket.Pop(byToggle);

		kPacket.Pop(aiAbil);
	}
}TBL_DEF_EFFECT;

int const MAX_EFFECT_ABIL_ARRAY = 10;
typedef struct tagTBL_DEF_EFFECTABIL
{
	tagTBL_DEF_EFFECTABIL()
	{
		iAbilNo = 0;
		memset(iType, 0, sizeof(iType));
		memset(iValue, 0, sizeof(iValue));
	}

	int iAbilNo;
	int iType[MAX_EFFECT_ABIL_ARRAY];
	int iValue[MAX_EFFECT_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_EFFECTABIL;

typedef struct tagTBL_DEF_EFFGRADECLASS
{
	tagTBL_DEF_EFFGRADECLASS()
	{
		iClass= 0;
		iParentClass= 0;
		byGrade= 0;
		sMinLevel= 0;
		sMaxLevel= 0;
		i64Bitmask= 0;
		byKind = 0;// Ŭ������ ����
	}
	
	int iClass;
	int iParentClass;
	BYTE byGrade;
	short int sMinLevel;
	short int sMaxLevel;
	__int64 i64Bitmask;
	BYTE byKind;// Ŭ������ ����

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_EFFGRADECLASS;

typedef struct tagItemEnchantKey
{
	static int const ms_iPetBaseType = 10000;// ���� Type���� ��ȣ...(SEnchantInfo�� Ư���� 127�����ۿ� ��� ���Ѵ�)

	tagItemEnchantKey()
	{
		Type = 0;
		Lv = 0;
	}

	tagItemEnchantKey(int const iInType, int const InLv)
	{
		Type = iInType;
		Lv = InLv;
	}

	bool operator < (const tagItemEnchantKey &rhs)const
	{
		if( Type < rhs.Type )	{return true;}
		if( Type > rhs.Type )	{return false;}

		if( Lv < rhs.Lv )	{return true;}
		if( Lv > rhs.Lv )	{return false;}

		return false;
	}

	int Type;
	BYTE Lv;
	
	DEFAULT_TBL_PACKET_FUNC();
}SItemEnchantKey;

int const MAX_ITEM_ENCHANT_ABIL_LIST = 10;
typedef struct tagTblDefItemEnchant
{	
	tagTblDefItemEnchant()
	{
		Type =0;
		Lv = 0;
		NameNo =0;
		memset(aAbil, 0, sizeof(aAbil));
	}
	int Type;
	BYTE Lv;
	int NameNo;
	int aAbil[MAX_ITEM_ENCHANT_ABIL_LIST];

	SItemEnchantKey Key()const
	{
		return SItemEnchantKey(Type, Lv);
	}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_ENCHANT;

typedef struct tagTblDefCharacterBaseWear
{
	tagTblDefCharacterBaseWear()
		:	iWearNo(0)
		,	iWearType(0)
		,	iSetNo(0)
		,	iClassNo(0)
	{
	}

	int iWearNo;	// �����۹�ȣ
	int iWearType;	// �������
	int iClassNo;
	int iSetNo;		// ��Ʈ��ȣ
	std::wstring strIconPath;

		size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t);
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(iWearNo);
		kPacket.Push(iWearType);
		kPacket.Push(iClassNo);
		kPacket.Push(iSetNo);

		kPacket.Push(strIconPath);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iWearNo);
		kPacket.Pop(iWearType);
		kPacket.Pop(iClassNo);
		kPacket.Pop(iSetNo);

		kPacket.Pop(strIconPath);
	}

}TBL_DEF_CHARACTER_BASEWEAR;

typedef tagTripleKey< bool, int, int >	SItemPlusUpgradeKey;// pri_key : IsPet?, sec_key : EquipPos, trd_key : PlusLevel

int const MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY = 3;
int const MAX_ITEM_PLUS_UPGRADE_MUST_NEED_ARRAY = 2;//����°���� ���������� ������ ��ħ.

typedef enum ePLUS_UPGRADE_NEED_TYPE
{
	PUNT_NEED_ITEM = 0,
	PUNT_BONUS_RATE_ITEM = 1,
	PUNT_INSURANCE_ITEM = 2,
}EPLUS_UPGRADE_NEED_TYPE;

typedef enum E_ENCHANT_SHIFT_RATE
{
	ESR_PLUS2,
	ESR_PLUS1,
	ESR_PLUS0,
	ESR_MINUS1,
	ESR_MINUS2,
	ESR_DELETE,

	ESR_NUM
}eEnchantShiftRate;

typedef struct tagTblDefItemPlusUpgrade
{
	tagTblDefItemPlusUpgrade()
	{
		SuccessRate = 0;
		SuccessRateBonus = 0;
		RareGroupNo = 0;
		RareGroupSuccessRate = 0;
		iNeedMoney = 0;
	}

	int SuccessRate;
	int SuccessRateBonus;
	int RareGroupNo;
	int RareGroupSuccessRate;
	int iNeedMoney;
	int aNeedItemNo[MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY];
	short aNeedItemCount[MAX_ITEM_PLUS_UPGRADE_NEED_ARRAY];
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMPLUSUPGRADE;

typedef struct tagTblDefItemEnchantShift
{
	tagTblDefItemEnchantShift()
		: EquipType(0)
		, Grade(0)
		, LevelLimit(0)
		, EnchantItemNo(0)
		, EnchantItemCount(0)
		, EnchantShiftCost(0)
		, EnchantShiftGemNo(0)
		, EnchantShiftGemCount(0)
		, InsuranceItemNo(0)
	{
		memset(EnchantShiftRate, 0, sizeof(EnchantShiftRate));
	}

	int EquipType;				// EEquipType
	int Grade;
	int LevelLimit;
	int EnchantItemNo;			// ����/�� ��ȭ ����
	int EnchantItemCount;		// ����/�� ��ȭ ���� ����
	int EnchantShiftCost;		// ��þƮ ���� ���
	int EnchantShiftGemNo;		// ��þƮ ���� ����
	int EnchantShiftGemCount;
	int InsuranceItemNo;		// ��þƮ ���� ����
	int	EnchantShiftRate[ESR_NUM];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEMENCHANTSHIFT;


#if 0

int const MAX_ITEM_RARITY_UPGRADE_NEED_ARRAY = 3;
int const MAX_ITEM_RARITY_UPGRADE_MUST_NEED_ARRAY = 2;//����°���� ���������� ������ ��ħ.
typedef struct tagTblDefItemRarityUpgrade
{
	tagTblDefItemRarityUpgrade()
	{
		iItemGrade = 0;
		iNeedMoney = 0;

		memset(anNeedItemCount, 0, sizeof(aiNeedItemNo));
		memset(anNeedItemCount, 0, sizeof(anNeedItemCount));
		nSuccessRate = 0;
	}
	
	int iItemGrade;
	int iNeedMoney;

	int aiNeedItemNo[MAX_ITEM_RARITY_UPGRADE_NEED_ARRAY];
	short anNeedItemCount[MAX_ITEM_RARITY_UPGRADE_NEED_ARRAY];
	short nSuccessRate;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_RARITY_UPGRADE;

#else

typedef struct tagTblDefItemRarityUpgrade
{
	tagTblDefItemRarityUpgrade()
	{
		iItemGrade = 0;
		iItemOptionLvMin = 0;
		iItemOptionLvMax = 0;
		iItemOptionCount = 0;
		iItemRarityMin = 0;
		iItemRarityMax = 0;
	}

	int iItemGrade;
	int iItemOptionLvMin;
	int iItemOptionLvMax;
	int iItemOptionCount;
	int iItemRarityMin;
	int iItemRarityMax;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_RARITY_UPGRADE;

#endif
typedef struct tagTblDefItemRarityControl
{
	tagTblDefItemRarityControl()
		:	iTypeNo(0)
		,	iSuccessRateControlNo(0)
	{}

	int iTypeNo;
	int iSuccessRateControlNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_RARITY_CONTROL;

typedef struct tagTblDefItemPlusUpControl
{
	tagTblDefItemPlusUpControl()
		:	iTypeNo(0)
		,	iSuccessRateControlNo_Low(0)
		,	iSuccessRateControlNo_High(0)
	{}

	int iTypeNo;
	int iSuccessRateControlNo_Low;
	int iSuccessRateControlNo_High;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_PLUSUP_CONTROL;

typedef struct tagTblDefItemDisassemble
{
	tagTblDefItemDisassemble()
	{
		iItemGrade = 0;
		iItemBagGrpNo = 0;
	}
	int iItemGrade;
	int iItemBagGrpNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_DISASSEMBLE;

int const MAX_SUCCESS_RATE_ARRAY = 10;
typedef struct tagTblDefSuccessRateControl
{
	tagTblDefSuccessRateControl()
	{
		iTotal = 0;
		memset(aRate, 0, sizeof(aRate));
	}
	int iTotal;//DB���� ���� ����. Load �ÿ� ���õ�.
	short aRate[MAX_SUCCESS_RATE_ARRAY];
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SUCCESS_RATE_CONTROL;

extern bool const RouletteRate(int const iRateTblNo, size_t &riOutResult, int const IN_MAX_INDEX,int const iAddRate = 0);//Ȯ�� �ε��� ������
extern bool const RouletteRate(int const iRateTblNo, int const iCount, std::vector<int> &rOutResult);//���� ��� ������
extern bool const RouletteRateEx( std::list<int> const &kListRateTblNo, int &riOutResult, int const IN_MAX_INDEX );// Ȯ�� �ε��� Ȯ�������

typedef struct tagTblDefCountControl
{
	tagTblDefCountControl()
	{
		iTotal = 0;
		memset(aCount, 0, sizeof(aCount));
	}
	int iTotal;//DB���� ���� ����. Load �ÿ� ���õ�.
	short aCount[MAX_SUCCESS_RATE_ARRAY];//SuccessRateControl�� ������ �����Ƿ� ���ڰ� ���Ҷ��� �Ѳ�����
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_COUNT_CONTROL;

typedef struct tagTblDefBagElements
{
	tagTblDefBagElements()
	{
		memset(aElement, 0, sizeof(aElement));
	}
	int aElement[MAX_SUCCESS_RATE_ARRAY];//SuccessRateControl�� ������ �����Ƿ� ���ڰ� ���Ҷ��� �Ѳ�����
}TBL_DEF_BAG_ELEMENTS;

typedef struct tagTblMonsterDefBagElements
	: public tagTblDefBagElements
{
	tagTblMonsterDefBagElements()
	{
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTER_BAG_ELEMENTS;

typedef struct tagTblItemDefBagElements
	: public tagTblDefBagElements
{
	tagTblItemDefBagElements()
		:	nTypeFlag(0)
	{}

	DEFAULT_TBL_PACKET_FUNC();

	WORD	nTypeFlag;
}TBL_DEF_ITEM_BAG_ELEMENTS;


int const MAX_MONSTERBAG_ELEMENT = 10;
typedef struct tagTblMonsterDefBagControl
{
	tagTblMonsterDefBagControl()
	{
		iParentBagNo = 0;
		memset(aBagElement, 0, sizeof(aBagElement));
	}
	
	int iParentBagNo;
	int aBagElement[MAX_MONSTERBAG_ELEMENT];
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTER_BAG_CONTROL;

typedef struct tagTblDefItemBag
{
	tagTblDefItemBag()
	{
		iElementsNo = 0;
		iRaseRateNo = 0;
		iCountControlNo = 0;
		iDropMoneyControlNo = 0;
	}
	int iElementsNo;
	int iRaseRateNo;
	int iCountControlNo;
	int iDropMoneyControlNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_BAG;

int const MAX_ITEM_CONTAINER_LIST = 10;
typedef struct tagTblDefItemContainer
{
	tagTblDefItemContainer()
	{
		iRewordItem = 0;
		iSuccessRateControlNo = 0;
		::memset(aiItemBagGrpNo, 0, sizeof(aiItemBagGrpNo));
	}

	int iRewordItem;
	int	iSuccessRateControlNo;
	int	aiItemBagGrpNo[MAX_ITEM_CONTAINER_LIST];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_CONTAINER;

typedef struct tagTblDefMonsterBag
{
	tagTblDefMonsterBag()
		: iTunningNo_Min(0)
		, iTunningNo_Max(0)
	{
		iBagNo = 0;
		iElementNo = 0;
		iRateNo = 0;
	}
	int iBagNo;
	int iElementNo;
	int iRateNo;
	int iTunningNo_Min;
	int iTunningNo_Max;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MONSTER_BAG;

int const MAX_OBJECT_ABIL_LIST = 10;
typedef struct tagTblDefObject
{
	tagTblDefObject()
	{
		iObjectNo = 0;
		fHeightValue = 0;
		memset(aAbil, 0, sizeof(aAbil));
	}
	int	iObjectNo;
	float fHeightValue;
	int	aAbil[MAX_OBJECT_ABIL_LIST];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_OBJECT;

int const MAX_OBJECT_ABIL_ARRAY = 10;
typedef struct tagTblDefObjectAbil
{
	tagTblDefObjectAbil()
	{
		iObjectAbilNo = 0;
		memset(aType, 0, sizeof(aType));
		memset(aValue, 0, sizeof(aValue));
	}

	int iObjectAbilNo;
	int aType[MAX_OBJECT_ABIL_ARRAY];
	int aValue[MAX_OBJECT_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_OBJECTABIL;

int const MAX_OBJECT_ELEMENT_NO = 10;
typedef struct tagTblDefObjectBag
{
	tagTblDefObjectBag()
	{
		iBagNo = 0;
		memset(iElementNo, 0 , sizeof(iElementNo));
	}
	int iBagNo;
	int iElementNo[MAX_OBJECT_ELEMENT_NO];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_OBJECT_BAG;

typedef struct tagTblDefObjectBagElement
{
	tagTblDefObjectBagElement()
	{
		iElementNo = 0;
		iObjectNo = 0;
	}

	int		iElementNo;
	int		iObjectNo;
	POINT3	pt3RelativePos;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_OBJECT_BAG_ELEMENT;

typedef struct tagTblDefItemByLevelElement
{
	tagTblDefItemByLevelElement()
		:	iLevel(0)
		,	iItemNo(0)
	{}

	tagTblDefItemByLevelElement( int const _iLevel, int const _iItemNo )
		:	iLevel(_iLevel)
		,	iItemNo(_iItemNo)
	{}

	bool operator < ( tagTblDefItemByLevelElement const &rhs )const{return iLevel < rhs.iLevel;}
	bool operator > ( tagTblDefItemByLevelElement const &rhs )const{return rhs < *this;}
	bool operator ==( tagTblDefItemByLevelElement const &rhs )const{return iLevel == rhs.iLevel;}

	int iLevel;
	int iItemNo;
}TBL_DEF_ITEM_BY_LEVEL_ELEMENT;

class TBL_DEF_ITEM_BY_LEVEL
	:	public BM::PgApproximateVector< TBL_DEF_ITEM_BY_LEVEL_ELEMENT >
{
public:
	TBL_DEF_ITEM_BY_LEVEL(){}
	~TBL_DEF_ITEM_BY_LEVEL(){}

	bool Get( int const iLevel, int &iItemNo )const
	{
		TBL_DEF_ITEM_BY_LEVEL_ELEMENT kFind( iLevel, 0 );
		TBL_DEF_ITEM_BY_LEVEL_ELEMENT kRet;
		if ( BM::PgApproximate::npos != find( kFind, kRet ) )
		{
			iItemNo = kRet.iItemNo;
			return true;
		}
		return false;
	}

	void WriteToPacket( BM::Stream &rkPacket )const
	{
		PU::TWriteArray_A( rkPacket, m_kContVar );
	}

	bool ReadFromPacket( BM::Stream &rkPacket )
	{
		m_kContVar.clear();
		return PU::TLoadArray_A( rkPacket, m_kContVar );
	}

	size_t min_size()const
	{
		return PU::type_size::min_size( &m_kContVar );
	}
};

typedef struct tagTBL_DEF_GMCMD
{
	tagTBL_DEF_GMCMD()
	{
		iCmdNo = 0;
		byLevel = 0;
	}

	int iCmdNo;
	BYTE byLevel;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_GMCMD;

int const MAX_MISSION_RANK_CLEAR_LV = 9;
typedef struct tagTBL_DEF_MISSION_RESULT
{
	tagTBL_DEF_MISSION_RESULT()
	{
		Clear();
	}
	void Clear()
	{
		iResultNo = 0;
		iRouletteBagGrpNo = 0;
		::memset(aiResultContainer, 0, sizeof(aiResultContainer));
		::memset(aiResultCountMin, 0, sizeof(aiResultCountMin));
		::memset(aiResultCountMax, 0, sizeof(aiResultCountMax));
	}
	
	int iResultNo;
	int iRouletteBagGrpNo; // �����ֱ� ����(���� �ִµ� ������� ����
	int aiResultContainer[MAX_MISSION_RANK_CLEAR_LV];//S A B C D E F 
	int aiResultCountMin[MAX_MISSION_RANK_CLEAR_LV];//S A B C D E F 
	int aiResultCountMax[MAX_MISSION_RANK_CLEAR_LV];//S A B C D E F 

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MISSION_RESULT;

int const MAX_MISSION_CANDIDATE = 8;
typedef struct tagTBL_DEF_MISSION_CANDIDATE
{
	tagTBL_DEF_MISSION_CANDIDATE()
	{
		iType = 0;
		iUseCount = 0;
		iCandidate_End = 0;
		iMissionTunningNo = 0;
		memset(aiCandidate, 0, sizeof(aiCandidate));
		memset(aiRankPoint, 0, sizeof(aiRankPoint));
	}

	int iType;
	int iUseCount;
	int iCandidate_End;
	int aiCandidate[MAX_MISSION_CANDIDATE];
	int aiRankPoint[MAX_MISSION_RANK_CLEAR_LV];
	int iMissionTunningNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MISSION_CANDIDATE;

int const MAX_MISSION_LEVEL = 6;
int const DEFENCE_MISSION_LEVEL = 7;
int const DEFENCE7_MISSION_LEVEL = 8;
int const DEFENCE8_MISSION_LEVEL = 9;

typedef struct tagTBL_DEF_MISSION_LEVELLIMIT
{
	tagTBL_DEF_MISSION_LEVELLIMIT()
	{
		::memset(aiLevel_Min,0,sizeof(aiLevel_Min));
		::memset(aiLevel_Max,100,sizeof(aiLevel_Max));

		::memset(aiLevel_AvgMin, 0, sizeof(aiLevel_AvgMin));
		::memset(aiLevel_AvgMax, 0, sizeof(aiLevel_AvgMax));
	}

	int aiLevel_Min[MAX_MISSION_LEVEL];
	int aiLevel_Max[MAX_MISSION_LEVEL];
	int aiLevel_AvgMin[MAX_MISSION_LEVEL];
	int aiLevel_AvgMax[MAX_MISSION_LEVEL];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MISSION_LEVELLIMIT;

typedef struct tagTBL_DEF_MISSION_ROOT
:	public tagTBL_DEF_MISSION_LEVELLIMIT
{
	tagTBL_DEF_MISSION_ROOT()
	{
		iMissionNo = 0;
		iKey = 0;
		memset(aiLevel, 0, sizeof(aiLevel));
		memset(aiLevel_AbilRateBagID, 0, sizeof(aiLevel_AbilRateBagID));
		memset(aiMissionResultNo, 0, sizeof(aiMissionResultNo));
		iDefence = 0;
		iDefence7 = 0;
		iDefence8 = 0;
	}

	int iMissionNo; 
	int iKey;
	int aiLevel[MAX_MISSION_LEVEL];
	int aiLevel_AbilRateBagID[MAX_MISSION_LEVEL];
	int aiMissionResultNo[MAX_MISSION_LEVEL];
	int iDefence;
	int iDefence7;
	int iDefence8;

	void Set(TBL_DEF_MISSION_LEVELLIMIT const& rhs)
	{
		::memcpy(aiLevel_Min, rhs.aiLevel_Min,sizeof(aiLevel_Min));
		::memcpy(aiLevel_Max, rhs.aiLevel_Max,sizeof(aiLevel_Max));
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MISSION_ROOT;

typedef struct tagTBL_MISSION_REPORT
{
	int		iClearTime_Top;
	int		iClearTime_Avg;
	__int64	i64ClearTime_Total;
	__int64 i64ClearCount;

	tagTBL_MISSION_REPORT()
	{
		Clear();
	}

	void Clear()
	{
		iClearTime_Top = 0;
		iClearTime_Avg = 0;
		i64ClearTime_Total = 0;
		i64ClearCount = 0;
	}

	// ���ٷ� �Ѵ�
	bool operator<(tagTBL_MISSION_REPORT const& rhs)const
	{
		return iClearTime_Top > rhs.iClearTime_Top;
	}
	bool operator>(tagTBL_MISSION_REPORT const& rhs)const
	{
		return iClearTime_Top < rhs.iClearTime_Top;
	}
	bool operator==(tagTBL_MISSION_REPORT const& rhs)const
	{
		return iClearTime_Top == rhs.iClearTime_Top;
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_MISSION_REPORT;

typedef struct tagTBL_MISSION_RANK
{
	BM::GUID				kCharGuid;
	int						iPoint;
	int						iPlayTime;		// �ʴ���
	std::wstring			wstrName;
	std::wstring			wstrMemo;
	int						iUserLevel;
	BYTE					kClass;
	BM::DBTIMESTAMP_EX		dtRegistTime;

	tagTBL_MISSION_RANK()
	{
		Clear();
	}

	void Clear()
	{
		kCharGuid.Clear();
		iPoint = 0;
		iPlayTime = 0;
		wstrMemo.clear();
		wstrName.clear();
		iUserLevel = 0;
		kClass = 0;
		dtRegistTime.Clear();
	}

	bool operator<(tagTBL_MISSION_RANK const& rhs)const
	{
		if ( iPoint == rhs.iPoint )
		{
			return iPlayTime < rhs.iPlayTime;
		}
		return iPoint > rhs.iPoint;	//���ٷ� �Ǿ� �ִ°� �´�
	}
	bool operator>(tagTBL_MISSION_RANK const& rhs)const
	{
		if ( iPoint == rhs.iPoint )
		{
			return iPlayTime > rhs.iPlayTime;
		}
		return iPoint < rhs.iPoint;	//���ٷ� �Ǿ� �ִ°� �´�
	}
	bool operator==(tagTBL_MISSION_RANK const& rhs)const
	{
		return (iPoint==rhs.iPoint) && (iPlayTime==rhs.iPlayTime);
	}

	size_t min_size()const
	{
		return 
			sizeof(kCharGuid)+
			sizeof(iPoint)+
			sizeof(iPlayTime)+
			sizeof(size_t)+//wstrName)+
			sizeof(size_t)+//wstrMemo)+
			sizeof(iUserLevel)+
			sizeof(kClass)+
			sizeof(dtRegistTime);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kCharGuid);
		kPacket.Push(iPoint);
		kPacket.Push(iPlayTime);
		kPacket.Push(wstrName);
		kPacket.Push(wstrMemo);
		kPacket.Push(iUserLevel);
		kPacket.Push(kClass);
		kPacket.Push(dtRegistTime);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kCharGuid);
		kPacket.Pop(iPoint);
		kPacket.Pop(iPlayTime);
		kPacket.Pop(wstrName);
		kPacket.Pop(wstrMemo);
		kPacket.Pop(iUserLevel);
		kPacket.Pop(kClass);
		kPacket.Pop(dtRegistTime);
	}
}TBL_MISSION_RANK;

typedef struct tagTBL_DEF_DEFENCE_ADD_MONSTER
{
	tagTBL_DEF_DEFENCE_ADD_MONSTER()
		: iAdd_StageTime(0)
		, iMonsterNo(0)
		, iEnchant_Probability(0)
		, iExpRate(0)
		, iItemNo(0)
		, iItemCount(1)
		, iDropRate(0)
	{
		wstrIconPath.clear();
	}

	int iAdd_StageTime; 
	int iMonsterNo;
	int iEnchant_Probability;
	int iExpRate;
	int iItemNo;
	int iItemCount;
	int iDropRate;
	std::wstring wstrIconPath;

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t);//wstrIconPath
	}

	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(iAdd_StageTime);

		kPacket.Push(iMonsterNo);
		kPacket.Push(iEnchant_Probability);

		kPacket.Push(iExpRate);
		
		kPacket.Push(iItemNo);
		kPacket.Push(iItemCount);
		kPacket.Push(iDropRate);

		kPacket.Push(wstrIconPath);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iAdd_StageTime);

		kPacket.Pop(iMonsterNo);
		kPacket.Pop(iEnchant_Probability);

		kPacket.Pop(iExpRate);
		
		kPacket.Pop(iItemNo);
		kPacket.Pop(iItemCount);
		kPacket.Pop(iDropRate);

		kPacket.Pop(wstrIconPath);
	}
}TBL_DEF_DEFENCE_ADD_MONSTER;

typedef struct tagTBL_SHOP_IN_GAME
{
	tagTBL_SHOP_IN_GAME()
	{
		iIDX = 0;
		kShopGuid.Clear();
		iCategory = 0;
		iItemNo = 0;
		iPrice = 0;
		iSellPrice = 0;
		iCP = 0;
		iCoin = 0;
		iUseTime = 0;
		bTimeType = 0;
		iState = 0;
	}
	
	int iIDX;
	BM::GUID kShopGuid;
	int iCategory;
	int iItemNo;
	int iPrice;
	int iSellPrice;
	int	iCP;
	int iCoin;
	BYTE bTimeType;
	int iUseTime;
	int iState;

	DEFAULT_TBL_PACKET_FUNC();

	bool operator < (const tagTBL_SHOP_IN_GAME &rhs)const
	{
		if( iIDX < rhs.iIDX )	{return true;}
		if( iIDX > rhs.iIDX )	{return false;}
		return false;
	}
	int Category2() const
	{
		return iCP; // CP ������ ä��(JobSkill)���� ī�װ��� 2�� ����Ѵ�
	}

	static tagTBL_SHOP_IN_GAME const & NullData()
	{
		static tagTBL_SHOP_IN_GAME sSelf;
		return sSelf;
	}
	bool IsNull()const
	{
//		(*this == NullData());
		if( 0 == iIDX
		&&	0 == iItemNo
		&&	BM::GUID::NullData() == kShopGuid)
		{
			return true;
		}
		return false;
	}

}TBL_SHOP_IN_GAME;

int const MAX_ITEM_OPTION_ABIL_NUM = 31;
typedef struct tagTBL_DEF_ITEM_OPTION
{
	tagTBL_DEF_ITEM_OPTION()
		:	iOptionType(0)
		,	iOutRate(0)
		,	iAbleEquipPos(0)
		,	iNameNo(0)
		,	iAbilType(0)
		,	iOptionGroup(0)
	{
		::memset( aiValue, 0, sizeof(aiValue) );
	}

	bool operator==(tagTBL_DEF_ITEM_OPTION const& rhs )const
	{
		return (( iOptionType == rhs.iOptionType) && (iOptionGroup == rhs.iOptionGroup) && (iAbleEquipPos == rhs.iAbleEquipPos));
	}

	int iOptionType;
	int iOutRate;
	int iAbleEquipPos;
	int	iOptionGroup;
	int iNameNo;
	int iAbilType;
	int aiValue[MAX_ITEM_OPTION_ABIL_NUM];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_OPTION;

int const MAX_OPTION_ABIL_ARRAY = 10;
typedef struct tagTBL_DEF_ITEM_OPTION_ABIL_PIECE
{
	typedef struct tagTBL_DEF_ITEM_OPTION_ABIL_PIECE_COMPARE_FUNCTOR
	{
		tagTBL_DEF_ITEM_OPTION_ABIL_PIECE_COMPARE_FUNCTOR(int iOrder, int iPieceCount) : m_iOrder(iOrder), m_iPieceCount(iPieceCount)
		{

		}
		bool operator() (tagTBL_DEF_ITEM_OPTION_ABIL_PIECE const& rhs)
		{
			return (rhs.iOrder == m_iOrder && rhs.iPieceCount == m_iPieceCount);
		}

		int m_iOrder;
		int m_iPieceCount;

	} TBL_DEF_ITEM_OPTION_ABIL_PIECE_COMPARE_FUNCTOR;

	tagTBL_DEF_ITEM_OPTION_ABIL_PIECE()
		: iPieceCount(0), iOrder(0)
	{
		memset(aiType, 0, sizeof(aiType));
		memset(aiValue, 0, sizeof(aiType));
	}
	bool operator <(tagTBL_DEF_ITEM_OPTION_ABIL_PIECE const& rhs) const
	{
		if(iPieceCount == rhs.iPieceCount) 
		{ 
			return iOrder < rhs.iOrder;
		}
		return iPieceCount < rhs.iPieceCount;
	}

	bool operator == (int Value) const
	{
		return iOrder == Value;
	}

	int iPieceCount;
	int iOrder;
	int aiType[MAX_OPTION_ABIL_ARRAY];
	int aiValue[MAX_OPTION_ABIL_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_OPTION_ABIL_PIECE;
typedef std::list< TBL_DEF_ITEM_OPTION_ABIL_PIECE > CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE;
typedef struct tagTBL_DEF_ITEM_OPTION_ABIL
{
	tagTBL_DEF_ITEM_OPTION_ABIL()
		:iOptionAbilNo(0), kContPieceAbil()
	{
	}
	tagTBL_DEF_ITEM_OPTION_ABIL(int const& riOptionAbilNo)
		:iOptionAbilNo(riOptionAbilNo), kContPieceAbil()
	{
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iOptionAbilNo);
		PU::TWriteArray_M(kPacket, kContPieceAbil);
	}
	bool ReadFromPacket(BM::Stream &kPacket)
	{
		bool bRet = kPacket.Pop(iOptionAbilNo);
		PU::TLoadArray_M(kPacket, kContPieceAbil);
		return bRet;
	}
	size_t min_size()const
	{
		return sizeof(iOptionAbilNo)+sizeof(size_t);
	}

	int iOptionAbilNo;
	CONT_TBL_DEF_ITEM_OPTION_ABIL_PIECE kContPieceAbil;
}TBL_DEF_ITEM_OPTION_ABIL;

typedef struct tagTBL_DEF_PVP_GROUND
{
	tagTBL_DEF_PVP_GROUND()
		: iGroundNo(0)
		, iName(0)
	{
		wstrPreviewImg.clear();
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iGroundNo);
		kPacket.Push(iName);
		kPacket.Push( wstrPreviewImg );
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iGroundNo);
		kPacket.Pop(iName);
		kPacket.Pop( wstrPreviewImg );
	}

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t);//wstrPreviewImg
	}
	
	int				iGroundNo;
	int				iName;
	std::wstring	wstrPreviewImg;
}TBL_DEF_PVP_GROUND;

typedef struct tagCONT_DEF_PVP_GROUND
	: public std::vector< TBL_DEF_PVP_GROUND >
{
	size_t min_size()const
	{
		return sizeof(size_t);
	}
	
	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_M(kPacket, *this);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_M(kPacket, *this);
	}
}CONT_DEF_PVP_GROUND;

typedef struct tagCONT_DEF_PVP_MAP
	: public std::map< TBL_KEY_INT, CONT_DEF_PVP_GROUND >
{
	size_t min_size()const
	{
		return sizeof(size_t);//std::map< TBL_KEY_INT, TBL_DEF_ACHIEVEMENTS >
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteTable_AM(kPacket, *this);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_AM(kPacket, *this);
	}
}CONT_DEF_PVP_MAP;

typedef struct tagTBL_DEF_PVP_GROUNDMODE
{
	tagTBL_DEF_PVP_GROUNDMODE()
	{
		iGroundNo = 0;
		iName = 0;
		iType = 0;
		wstrPreviewImg.clear();
	}

	int				iGroundNo;
	int				iName;
	int				iType;
	std::wstring	wstrPreviewImg;
	int				iUserCount;
	
	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t)+//wstrPreviewImg
			sizeof(int);
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push( iGroundNo );
		kPacket.Push( iName );
		kPacket.Push( iType );
		kPacket.Push( wstrPreviewImg );
		kPacket.Push( iUserCount );
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop( iGroundNo );
		kPacket.Pop( iName );
		kPacket.Pop( iType );
		kPacket.Pop( wstrPreviewImg );
		kPacket.Pop( iUserCount );
	}
}TBL_DEF_PVP_GROUNDMODE;

typedef struct tagDefPvPReward
{
	tagDefPvPReward()
		:	iPoint_Min(0)
		,	iPoint_Max(0)
		,	iCP(0)
	{
		iPoint_Min = 0;
		iPoint_Max = 0;
		iCP = 0;
	}

	int iPoint_Min;
	int iPoint_Max;
	int iCP;

	DEFAULT_TBL_PACKET_FUNC();

	bool operator<( tagDefPvPReward const &rhs )const
	{
		return iPoint_Max < rhs.iPoint_Min;
	}
	bool operator>( tagDefPvPReward const &rhs )const
	{
		return rhs < *this;
	}

	bool operator<( int const iPoint )const
	{
		return iPoint_Max < iPoint;
	}
	bool operator>( int const iPoint )const
	{
		return iPoint_Min > iPoint;
	}
	bool operator==( int const iPoint )const
	{
		return IsIn( iPoint );
	}

	bool Check()
	{
		return iPoint_Min <= iPoint_Max;
	}

	bool Check( tagDefPvPReward const &rhs )const
	{
		return ( *this < rhs ) != ( rhs < *this );
	}
	bool IsIn( int const iPoint )const
	{
		if ( (iPoint_Min <= iPoint) && (iPoint_Max) >= iPoint )
		{
			return true;
		}
		return false;
	}
	
}TBL_DEF_PVP_REWARD;

typedef struct tagDefPvPRewardItem
{
	tagDefPvPRewardItem()
		:	byWin_ItemCount(0)
		,	byDraw_ItemCount(0)
		,	byLose_ItemCount(0)
	{
		::memset( iItemBag, 0, sizeof(iItemBag) );
	}

	DEFAULT_TBL_PACKET_FUNC();

	BYTE byWin_ItemCount;
	BYTE byDraw_ItemCount;
	BYTE byLose_ItemCount;
	int iItemBag[3];

}TBL_DEF_PVP_REWARD_ITEM;

typedef struct tagDefPlayLimitInfo
{
	tagDefPlayLimitInfo(void)
		:	i64BeginTime(0i64)
		,	i64EndTime(0i64)
		,	nLevelLimit(0)
		,	iValue01(0)
		,	iValue02(0)
		,	iBalanceLevelMin(0)
		,	iBalanceLevelMax(0)
	{}

	__int64 i64BeginTime;
	__int64 i64EndTime;
	short	nLevelLimit;
	int		iValue01;
	int		iValue02;
	int		iBalanceLevelMin;
	int		iBalanceLevelMax;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PLAYLIMIT_INFO;

typedef struct tagTBL_DEF_DROP_MONEY_CONTROL
{
	tagTBL_DEF_DROP_MONEY_CONTROL()
	{
		iNo = 0;
		iRate = 0;
		iMin = 0;
		iMax = 0;
	}

	int iNo;
	int iRate;
	int iMin;
	int iMax;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_DROP_MONEY_CONTROL;

typedef struct tagTBL_DEF_QUEST_REWARD
{
	tagTBL_DEF_QUEST_REWARD()
	{
		kNationCodeStr.clear();
		iQuestID = 0;
		kXmlPath.clear();
		iGroupNo = 0;
		iDBQuestType = 0;
		iExp = 0;
		iGold = 0;
		iLevelMin = 0;
		iLevelMax = 0;
		iLimitMaxLevel = 0;
		iTacticsLevel = 0;
		iTacticsExp = 0;
		iGuildExp = 0;
		iDifficult = 0;
		iOrderIndex = 0;
		byIsCanRemoteComplete = 0;
		bIsCanShare = true;
		iTitleText = 0;
		iGroupName = 0;
		i64ClassFlag = 0i64;
		i64DraClassLimit = 0i64;
		kPreQuestAnd.clear();
		kPreQuestOr.clear();
		kNotQuest.clear();
		iMinParty = 0;
		iMaxParty = 0;
		iWantedClearCount = 0;
		iWantedItemNo = 0;
		iWantedItemCount = 0;
		iWantedCoolTime = 0;
		iDra_WantedItemNo = 0;
		iDra_WantedItemCount = 0;
		iRewardPerLevel_Exp = 0;
		iRewardPerLevel_Money = 0;
		bIsCanRemoteAccept = false;
	}

	std::wstring kNationCodeStr;
	int iQuestID;
	std::wstring kXmlPath;
	int iGroupNo;
	int iDBQuestType;
	int iExp;
	int iGold;
	int iLevelMin;
	int iLevelMax;
	int iLimitMaxLevel;
	int iTacticsLevel;
	int iTacticsExp;
	int iGuildExp;
	int iDifficult;
	int iOrderIndex;
	BYTE byIsCanRemoteComplete;
	bool bIsCanShare;
	int iTitleText;
	int iGroupName; // ����Ʈ �������� �ؽ�Ʈ ��ȣ
	__int64 i64ClassFlag;
	__int64	i64DraClassLimit;
	std::wstring kPreQuestAnd;
	std::wstring kPreQuestOr;
	std::wstring kNotQuest;
	int iMinParty;
	int iMaxParty;
	int iWantedClearCount;
	int iWantedItemNo;
	int iWantedItemCount;
	int iWantedCoolTime;
	int	iDra_WantedItemNo;
	int	iDra_WantedItemCount;
	int iRewardPerLevel_Exp;
	int iRewardPerLevel_Money;
	bool bIsCanRemoteAccept;

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kNationCodeStr);
		kPacket.Push(iQuestID);
		kPacket.Push(kXmlPath);
		kPacket.Push(iGroupNo);
		kPacket.Push(iDBQuestType);
		kPacket.Push(iExp);
		kPacket.Push(iGold);
		kPacket.Push(iLevelMin);
		kPacket.Push(iLevelMax);
		kPacket.Push(iLimitMaxLevel);
		kPacket.Push(iTacticsLevel);
		kPacket.Push(iTacticsExp);
		kPacket.Push(iGuildExp);
		kPacket.Push(iDifficult);
		kPacket.Push(iOrderIndex);
		kPacket.Push(byIsCanRemoteComplete);
		kPacket.Push(bIsCanShare);
		kPacket.Push(iTitleText);
		kPacket.Push(iGroupName);
		kPacket.Push(i64ClassFlag);
		kPacket.Push(i64DraClassLimit);
		kPacket.Push(kPreQuestAnd);
		kPacket.Push(kPreQuestOr);
		kPacket.Push(kNotQuest);
		kPacket.Push(iMinParty);
		kPacket.Push(iMaxParty);
		kPacket.Push(iWantedClearCount);
		kPacket.Push(iWantedItemNo);
		kPacket.Push(iWantedItemCount);
		kPacket.Push(iWantedCoolTime);
		kPacket.Push(iDra_WantedItemNo);
		kPacket.Push(iDra_WantedItemCount);
		kPacket.Push(iRewardPerLevel_Exp);
		kPacket.Push(iRewardPerLevel_Money);
		kPacket.Push(bIsCanRemoteAccept);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kNationCodeStr);
		kPacket.Pop(iQuestID);
		kPacket.Pop(kXmlPath);
		kPacket.Pop(iGroupNo);
		kPacket.Pop(iDBQuestType);
		kPacket.Pop(iExp);
		kPacket.Pop(iGold);
		kPacket.Pop(iLevelMin);
		kPacket.Pop(iLevelMax);
		kPacket.Pop(iLimitMaxLevel);
		kPacket.Pop(iTacticsLevel);
		kPacket.Pop(iTacticsExp);
		kPacket.Pop(iGuildExp);
		kPacket.Pop(iDifficult);
		kPacket.Pop(iOrderIndex);
		kPacket.Pop(byIsCanRemoteComplete);
		kPacket.Pop(bIsCanShare);
		kPacket.Pop(iTitleText);
		kPacket.Pop(iGroupName);
		kPacket.Pop(i64ClassFlag);
		kPacket.Pop(i64DraClassLimit);
		kPacket.Pop(kPreQuestAnd);
		kPacket.Pop(kPreQuestOr);
		kPacket.Pop(kNotQuest);
		kPacket.Pop(iMinParty);
		kPacket.Pop(iMaxParty);
		kPacket.Pop(iWantedClearCount);
		kPacket.Pop(iWantedItemNo);
		kPacket.Pop(iWantedItemCount);
		kPacket.Pop(iWantedCoolTime);
		kPacket.Pop(iDra_WantedItemNo);
		kPacket.Pop(iDra_WantedItemCount);
		kPacket.Pop(iRewardPerLevel_Exp);
		kPacket.Pop(iRewardPerLevel_Money);
		kPacket.Pop(bIsCanRemoteAccept);
	}
	size_t min_size()const
	{
		int const iIntCount = 21;
		size_t const iByteCount = 2;

		return	sizeof(int) * iIntCount
				+ sizeof(std::wstring::value_type) * kNationCodeStr.size()
				+ sizeof(std::wstring::value_type) * kXmlPath.size()
				+ sizeof(std::wstring::value_type) * kPreQuestAnd.size()
				+ sizeof(std::wstring::value_type) * kPreQuestOr.size()
				+ sizeof(std::wstring::value_type) * kNotQuest.size()
				+ ( sizeof(__int64) * 2 )
				+ iByteCount;
	}
} TBL_DEF_QUEST_REWARD;

typedef struct tagTBL_DEF_QUEST_RANDOM
{
	tagTBL_DEF_QUEST_RANDOM()
		: iType(0), iMinLevel(0), iMaxLevel(0), iGroupNo(0),
		iStartQuestID(0), iEndQuestID(0), iDisplayCount(0)
	{
	}
	tagTBL_DEF_QUEST_RANDOM(tagTBL_DEF_QUEST_RANDOM const& rhs)
		: iType(rhs.iType), iMinLevel(rhs.iMinLevel), iMaxLevel(rhs.iMaxLevel), iGroupNo(rhs.iGroupNo),
		iStartQuestID(rhs.iStartQuestID), iEndQuestID(rhs.iEndQuestID), iDisplayCount(rhs.iDisplayCount)
	{
	}

	int iType;
	int iMinLevel;
	int iMaxLevel;
	int iGroupNo;
	int iStartQuestID;
	int iEndQuestID;
	int iDisplayCount;

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_QUEST_RANDOM;

typedef struct tagTBL_DEF_QUEST_WANTED
{
	tagTBL_DEF_QUEST_WANTED()
		: iQuestID(0), iClearCount(0), iItemNo(0), iItemCount(0), iCoolTime(0), iDra_ItemNo(0), iDra_ItemCount(0)
	{
	}
	tagTBL_DEF_QUEST_WANTED( tagTBL_DEF_QUEST_WANTED const& rhs )
		: iQuestID(rhs.iQuestID), iClearCount(rhs.iClearCount),
		iItemNo(rhs.iItemNo), iItemCount(rhs.iItemCount), iCoolTime(rhs.iCoolTime),
		iDra_ItemNo(rhs.iDra_ItemNo), iDra_ItemCount(rhs.iDra_ItemCount)
	{
	}

	int iQuestID;
	int iClearCount;	
	int iItemNo;
	int iItemCount;
	int iCoolTime;
	int iDra_ItemNo;
	int iDra_ItemCount;

	DEFAULT_TBL_PACKET_FUNC();
} TBL_DEF_QUEST_WANTED;

typedef std::map<WORD, int> CLASSDEF_ABIL_CONT;
typedef struct
{
	TBL_DEF_CLASS kDef;
	CLASSDEF_ABIL_CONT kAbil;
}CLASS_DEF_BUILT;

int const MAX_ITEM_BAG_ELEMENT = 10;
typedef struct tagTBL_DQT_DEF_ITEM_BAG_GROUP
{
	tagTBL_DQT_DEF_ITEM_BAG_GROUP()
	{
		iBagGroupNo = 0;
		iSuccessRateNo = 0;
		memset(aiBagNo, 0, sizeof(aiBagNo));
	}

	int iBagGroupNo;
	int iSuccessRateNo;
	int aiBagNo[MAX_ITEM_BAG_ELEMENT];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DQT_DEF_ITEM_BAG_GROUP;

int const MAP_ITEM_BAG_GRP = 10;
typedef struct tagTBL_DQT_DEF_MAP_ITEM_BAG
{
	tagTBL_DQT_DEF_MAP_ITEM_BAG()
	{
		iMapNo = 0;
		iSuccessRateNo = 0;
		memset(aiBagGrpNo, 0, sizeof(aiBagGrpNo));
	}
	int iMapNo;
	int iSuccessRateNo;
	int aiBagGrpNo[MAP_ITEM_BAG_GRP];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DQT_DEF_MAP_ITEM_BAG;

typedef struct tagTBL_DEF_SPEND_MONEY
{
	tagTBL_DEF_SPEND_MONEY()
	{
		iID = 0;
		iType = 0;
		iLevelMin = 0;
		iLevelMax = 0;
		iSpendMoneyValue = 0;
	}

	int iID;
	int iType;
	int iLevelMin;
	int iLevelMax;
	int iSpendMoneyValue;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SPEND_MONEY;


typedef struct tagTBL_DEF_PROPERTY_KEY
{
	int	iProperty;		// �Ӽ�
	int	iPropertyLevel;	// �Ӽ� ����
	
	tagTBL_DEF_PROPERTY_KEY()
	{
		iProperty = 0;
		iPropertyLevel = 0;
	}

	tagTBL_DEF_PROPERTY_KEY(int const io, int const id)
		:	iProperty(io)
		,	iPropertyLevel(id)
	{
	}

	bool operator<(tagTBL_DEF_PROPERTY_KEY const& rhs )const
	{
		if ( iProperty == rhs.iProperty)
		{
			return iPropertyLevel < rhs.iPropertyLevel;
		}

		return iProperty < rhs.iProperty;
	}
	bool operator>(tagTBL_DEF_PROPERTY_KEY const& rhs )const
	{
		if ( iProperty == rhs.iProperty)
		{
			return iPropertyLevel > rhs.iPropertyLevel;
		}
		return iProperty > rhs.iProperty;
	}
	bool operator==(tagTBL_DEF_PROPERTY_KEY const& rhs )const
	{
		return (( iProperty == rhs.iProperty) && (iPropertyLevel > rhs.iPropertyLevel));
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PROPERTY_KEY;

typedef std::vector<int> CONT_DEF_PROPERTY_RATE;

typedef struct tagTBL_DEF_PROPERTY
{
	tagTBL_DEF_PROPERTY()
	{
	}

	TBL_DEF_PROPERTY_KEY	kKey;
	CONT_DEF_PROPERTY_RATE	kContRate;

	size_t min_size()const
	{
		return 
			sizeof(size_t)+//kKey
			sizeof(size_t);//kContRate
	}

	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(kKey);
		kPacket.Push(kContRate);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kKey);
		kPacket.Pop(kContRate);
	}
}TBL_DEF_PROPERTY;

int const MAX_MAP_EFFECT_ABIL_ARRAY = 10;
int const MAX_MAP_EFFECT_ABIL64_ARRAY = 1;

typedef struct tagTBL_DEF_MAP_EFFECT
{
	tagTBL_DEF_MAP_EFFECT()
	{
		iEffectID = 0;
		iGroundNo = 0;
		memset(aiType,  0, sizeof(aiType));
		memset(aiValue, 0, sizeof(aiValue));
	}

	int iEffectID;
	int iGroundNo;
	int aiType[MAX_MAP_EFFECT_ABIL_ARRAY];
	int aiValue[MAX_MAP_EFFECT_ABIL_ARRAY];

	int aiType64[MAX_MAP_EFFECT_ABIL64_ARRAY];
	__int64 aiValue64[MAX_MAP_EFFECT_ABIL64_ARRAY];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MAP_EFFECT;

typedef struct tagTBL_DEF_MAP_ENTITY
{
	tagTBL_DEF_MAP_ENTITY( int const _iGroundNo=0 )
		:	iGroundNo(_iGroundNo)
		,	iClass(0)
		,	nLevel(0)
		,	iEffect(0)
	{}

	int			iGroundNo;
	int			iClass;
	short		nLevel;
	int			iEffect;
	POINT3		pt3Min;
	POINT3		pt3Max;
	POINT3		pt3Pos;
	BM::GUID	kOwnerPointGuid;

	bool operator<( tagTBL_DEF_MAP_ENTITY const &rhs )const{return iGroundNo < rhs.iGroundNo;}
	bool operator>( tagTBL_DEF_MAP_ENTITY const &rhs )const{return rhs < *this;}
	bool operator==( tagTBL_DEF_MAP_ENTITY const &rhs )const{return iGroundNo == rhs.iGroundNo;}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MAP_ENTITY;

typedef struct tagTBL_DEF_FIVE_ELEMENT_INFO
{
	tagTBL_DEF_FIVE_ELEMENT_INFO()
	{
		iAttrType = 0;
		iNameNo = 0;
		iCrystalStoneNo = 0;
	}

	int iAttrType;
	int iNameNo;
	int iCrystalStoneNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_FIVE_ELEMENT_INFO;

typedef struct tagSMapStoneControl
{
	tagSMapStoneControl()
	{
		iStoneCount = 0;
		iTicketCount = 0;
	}

	int iStoneCount;
	int iTicketCount;
	DEFAULT_TBL_PACKET_FUNC();
}SMapStoneControl;

int const MAX_STONE_TICKET = 4;
typedef struct tagTBL_DEF_MAP_STONE_CONTROL
{
	tagTBL_DEF_MAP_STONE_CONTROL()
	{
		iMapNo = 0;
		iPeriod = 0;
	}

	int iMapNo;
	int iPeriod;
	SMapStoneControl aStoneCtrl[MAX_STONE_TICKET];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_MAP_STONE_CONTROL;

typedef struct tagTBL_DEF_ITEM_RARITY_UPGRADE_COST_RATE
{
	tagTBL_DEF_ITEM_RARITY_UPGRADE_COST_RATE()
	{
		iEquipPos = 0;
		iCostRate = 0;
	}
	int	iEquipPos;
	int iCostRate;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ITEM_RARITY_UPGRADE_COST_RATE;

typedef struct tagTBL_DEF_CASH_SHOP_ITEM
{
	tagTBL_DEF_CASH_SHOP_ITEM()
	{
		iItemNo = 0;
		iItemCount = 0;
	}
	int iItemNo,
		iItemCount;
	bool operator ==( tagTBL_DEF_CASH_SHOP_ITEM const &rhs )const
	{
		return( (iItemNo==rhs.iItemNo)
			&& (iItemCount==rhs.iItemCount) );
	}
}TBL_DEF_CASH_SHOP_ITEM;

typedef std::vector<TBL_DEF_CASH_SHOP_ITEM>			CONT_CASH_SHOP_ITEM;

typedef struct tagTBL_DEF_CASH_SHOP_ITEM_LIMITSELL
{
	tagTBL_DEF_CASH_SHOP_ITEM_LIMITSELL()
	{
		iItemTotalCount = 0;
		iItemLimitCount = 0;
		ItemBuyLimitCount = 0;
	}
	int iItemTotalCount,
		iItemLimitCount,
		ItemBuyLimitCount;
	bool operator ==( tagTBL_DEF_CASH_SHOP_ITEM_LIMITSELL const &rhs )const
	{
		return( (iItemTotalCount==rhs.iItemTotalCount)
			&& (iItemLimitCount==rhs.iItemLimitCount)
			&& (ItemBuyLimitCount==rhs.ItemBuyLimitCount) );
	}

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iItemTotalCount);
		kPacket.Push(iItemLimitCount);
		kPacket.Push(ItemBuyLimitCount);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iItemTotalCount);
		kPacket.Pop(iItemLimitCount);
		kPacket.Pop(ItemBuyLimitCount);
	}
}TBL_DEF_CASH_SHOP_ITEM_LIMITSELL;

typedef struct tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY
{
	BYTE bTimeType;
	int iUseTime;

	tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY()
	{
		bTimeType = 0;
		iUseTime = 0;
	}

	tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY(BYTE _bTimeType,int _iUseTime)
	{
		bTimeType = _bTimeType;
		iUseTime = _iUseTime;
	}

	bool const operator < (tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY const & kPrice) const
	{
		if(bTimeType < kPrice.bTimeType)
		{
			return true;
		}
		else if(bTimeType == kPrice.bTimeType)
		{
			if(iUseTime < kPrice.iUseTime)
			{
				return true;
			}
		}
		return false;
	}

	bool operator ==( tagTBL_DEF_CASH_SHOP_ITEM_PRICE_KEY const &rhs )const
	{
		return( (bTimeType==rhs.bTimeType)
			&& (iUseTime==rhs.iUseTime) );
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY;

typedef struct tagTBL_DEF_CASH_SHOP_ITEM_PRICE
{
	int iItemUseTime,
		iCash,
		iDiscountCash,
		iExtendCash,
		iDiscountExtendCash,
		iPriceIdx;

	bool IsDiscount,
		 IsExtendDiscount,
		 IsOnlyMileage;

	BYTE bTimeType;
	tagTBL_DEF_CASH_SHOP_ITEM_PRICE()
	{
		iItemUseTime = 0;
		iCash = 0;
		iDiscountCash = 0;
		IsDiscount = false;
		IsExtendDiscount = false;
		bTimeType = 0;
		iExtendCash = 0;
		iDiscountExtendCash = 0;
		IsOnlyMileage = false;
		iPriceIdx = 0;
	}

	bool operator ==( tagTBL_DEF_CASH_SHOP_ITEM_PRICE const &rhs )const
	{
		return( (iItemUseTime==rhs.iItemUseTime)
			&& (iCash==rhs.iCash)
			&& (iDiscountCash==rhs.iDiscountCash)
			&& (iExtendCash==rhs.iExtendCash)
			&& (iDiscountExtendCash==rhs.iDiscountExtendCash)
			&& (IsDiscount==rhs.IsDiscount)
			&& (IsExtendDiscount==rhs.IsExtendDiscount)
			&& (IsOnlyMileage==rhs.IsOnlyMileage) 
			&& (iPriceIdx == rhs.iPriceIdx) );
	}
}TBL_DEF_CASH_SHOP_ITEM_PRICE;

typedef std::map<TBL_DEF_CASH_SHOP_ITEM_PRICE_KEY,TBL_DEF_CASH_SHOP_ITEM_PRICE>	CONT_CASH_SHOP_ITEM_PRICE;

typedef struct tagTBL_DEF_CASH_SHOP_ARTICLE
{
	tagTBL_DEF_CASH_SHOP_ARTICLE()
	{
		iIdx = 0;
		iCategoryIdx = 0;

		bU = 0;
		bV = 0;
		bUVIndex = 0;

		bState = 0;
		bSaleType = 0;
		iMileage = 0;

		bDisplayTypeIdx = 0;
		wDisplayRank = 0;
	}

	int					iIdx;
	int					iCategoryIdx;

	BYTE				bU;
	BYTE				bV;
	BYTE				bUVIndex;

	std::wstring		wstrIconPath;
	std::wstring		wstrName;
	std::wstring		wstrDiscription;

	BYTE				bState;
	BYTE				bSaleType;
	
	BM::PgPackedTime	kSaleStartDate;
	BM::PgPackedTime	kSaleEndDate;

	int					iMileage;

	BYTE				bDisplayTypeIdx;
	WORD				wDisplayRank;

	CONT_CASH_SHOP_ITEM			kContCashItem;
	CONT_CASH_SHOP_ITEM_PRICE	kContCashItemPrice;

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+

			sizeof(BYTE)+
			sizeof(BYTE)+
			sizeof(BYTE)+

			sizeof(size_t)+
			sizeof(size_t)+
			sizeof(size_t)+

			sizeof(BYTE)+
			sizeof(BYTE)+
			
			sizeof(BM::PgPackedTime)+
			sizeof(BM::PgPackedTime)+

			sizeof(int)+

			sizeof(BYTE)+
			sizeof(WORD)+

			sizeof(size_t)+//CONT_CASH_SHOP_ITEM;
			sizeof(size_t);//CONT_CASH_SHOP_ITEM_PRICE;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iIdx);
		kPacket.Push(iCategoryIdx);
		kPacket.Push(bU);
		kPacket.Push(bV);
		kPacket.Push(bUVIndex);
		kPacket.Push(wstrIconPath);
		kPacket.Push(wstrName);
		kPacket.Push(wstrDiscription);
		kPacket.Push(bState);
		kPacket.Push(bSaleType);
		kPacket.Push(kSaleStartDate);
		kPacket.Push(kSaleEndDate);//10
		kPacket.Push(iMileage);
		kPacket.Push(bDisplayTypeIdx);
		kPacket.Push(wDisplayRank);
		kPacket.Push(kContCashItem);
		kPacket.Push(kContCashItemPrice);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iIdx);
		kPacket.Pop(iCategoryIdx);
		kPacket.Pop(bU);
		kPacket.Pop(bV);
		kPacket.Pop(bUVIndex);
		kPacket.Pop(wstrIconPath);
		kPacket.Pop(wstrName);
		kPacket.Pop(wstrDiscription);
		kPacket.Pop(bState);
		kPacket.Pop(bSaleType);
		kPacket.Pop(kSaleStartDate);
		kPacket.Pop(kSaleEndDate);//10
		kPacket.Pop(iMileage);
		kPacket.Pop(bDisplayTypeIdx);
		kPacket.Pop(wDisplayRank);
		kPacket.Pop(kContCashItem);
		kPacket.Pop(kContCashItemPrice);
	}

	bool operator ==( tagTBL_DEF_CASH_SHOP_ARTICLE const &rhs )const
	{
		return ( (iIdx==rhs.iIdx)
			&& (iCategoryIdx==rhs.iCategoryIdx)
			&& (bU==rhs.bU)
			&& (bV==rhs.bV)
			&& (bUVIndex==rhs.bUVIndex)
			&& (wstrIconPath==rhs.wstrIconPath)
			&& (wstrName==rhs.wstrName)
			&& (wstrDiscription==rhs.wstrDiscription)
			&& (bState==rhs.bState)
			&& (bSaleType==rhs.bSaleType)
			&& (kSaleStartDate==rhs.kSaleStartDate)
			&& (kSaleEndDate==rhs.kSaleEndDate)
			&& (iMileage==rhs.iMileage)
			&& (bDisplayTypeIdx==rhs.bDisplayTypeIdx)
			&& (wDisplayRank==rhs.wDisplayRank)
			&& (kContCashItem==rhs.kContCashItem)
			&& (kContCashItemPrice==rhs.kContCashItemPrice) );
	}

	bool IsComprise( tagTBL_DEF_CASH_SHOP_ARTICLE const &rhs )const
	{
		if( false == ((iIdx==rhs.iIdx)
			&& (iCategoryIdx==rhs.iCategoryIdx)
			&& (bU==rhs.bU)
			&& (bV==rhs.bV)
			&& (bUVIndex==rhs.bUVIndex)
			&& (wstrIconPath==rhs.wstrIconPath)
			&& (wstrName==rhs.wstrName)
			&& (wstrDiscription==rhs.wstrDiscription)
			&& (bState==rhs.bState)
			&& (bSaleType==rhs.bSaleType)
			&& (kSaleStartDate==rhs.kSaleStartDate)
			&& (kSaleEndDate==rhs.kSaleEndDate)
			&& (iMileage==rhs.iMileage)
			&& (bDisplayTypeIdx==rhs.bDisplayTypeIdx)
			&& (wDisplayRank==rhs.wDisplayRank)) )
		{
			return false;
		}

		if(kContCashItem.size()<rhs.kContCashItem.size())
		{
			return false;
		}

		if(kContCashItemPrice.size()<rhs.kContCashItemPrice.size())
		{
			return false;
		}

		for(CONT_CASH_SHOP_ITEM::const_iterator rhs_iter=rhs.kContCashItem.begin();rhs_iter!=rhs.kContCashItem.end();++rhs_iter)
		{
			bool bCheck = false;
			for(CONT_CASH_SHOP_ITEM::const_iterator iter=kContCashItem.begin();iter!=kContCashItem.end();++iter)
			{
				if(*rhs_iter == *iter)
				{
					bCheck = true;
					break;
				}
			}
			if(false==bCheck)
			{
				return false;
			}
		}

		for(CONT_CASH_SHOP_ITEM_PRICE::const_iterator rhs_iter=rhs.kContCashItemPrice.begin();rhs_iter!=rhs.kContCashItemPrice.end();++rhs_iter)
		{
			bool bCheck = false;
			for(CONT_CASH_SHOP_ITEM_PRICE::const_iterator iter=kContCashItemPrice.begin();iter!=kContCashItemPrice.end();++iter)
			{
				if(*rhs_iter == *iter)
				{
					bCheck = true;
					break;
				}
			}
			if(false==bCheck)
			{
				return false;
			}
		}

		return true;
	}
}TBL_DEF_CASH_SHOP_ARTICLE;

struct tagTBL_DEF_CASH_SHOP_CATEGORY;

typedef std::map<TBL_KEY_INT,TBL_DEF_CASH_SHOP_ARTICLE>	CONT_DEF_CASH_SHOP_ARTICLE;
typedef std::map<TBL_KEY_INT,tagTBL_DEF_CASH_SHOP_CATEGORY>	CONT_DEF_CASH_SHOP;
typedef std::map<TBL_KEY_INT,TBL_DEF_CASH_SHOP_ITEM_LIMITSELL>	CONT_CASH_SHOP_ITEM_LIMITSELL;


typedef struct tagTBL_DEF_CASH_SHOP_CATEGORY
{
	BYTE						bStep;
	BYTE						bRank;
	int							iRootIdx;
	int							iParentIdx;
	bool						bIsUsed;
	std::wstring				wstrName;
	CONT_DEF_CASH_SHOP_ARTICLE	kContArticle;
	CONT_DEF_CASH_SHOP			kSubCategory;

	size_t min_size()const
	{
		return 
			sizeof(bStep)+
			sizeof(bRank)+
			sizeof(iRootIdx)+
			sizeof(iParentIdx)+
			sizeof(bIsUsed)+
			sizeof(size_t);//wstrName);
	}

	tagTBL_DEF_CASH_SHOP_CATEGORY()
	{
		bStep = 0;
		bRank = 0;
		iRootIdx = 0;
		iParentIdx = 0;
		bIsUsed = false;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(bStep);
		kPacket.Push(bRank);
		kPacket.Push(iRootIdx);
		kPacket.Push(iParentIdx);
		kPacket.Push(bIsUsed);
		kPacket.Push(wstrName);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(bStep);
		kPacket.Pop(bRank);
		kPacket.Pop(iRootIdx);
		kPacket.Pop(iParentIdx);
		kPacket.Pop(bIsUsed);
		kPacket.Pop(wstrName);
		kContArticle.clear();
		kSubCategory.clear();
	}
}TBL_DEF_CASH_SHOP_CATEGORY;

typedef struct tagTBL_DEF_EMPORIA_FUNCTION
{
	tagTBL_DEF_EMPORIA_FUNCTION()
		:	nFuncNo(0)
		,	nPrice_ForWeek(0)
		,	iValue(0)
	{}

	short	nFuncNo;
	short	nPrice_ForWeek;
	int		iValue;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_EMPORIA_FUNCTION;
typedef std::map< short, TBL_DEF_EMPORIA_FUNCTION >	CONT_DEF_EMPORIA_FUNCTION;

int const MAX_EMPORIA_GRADE = 5; 

typedef struct tagTBL_DEF_EMPORIA_REWARD
{
	typedef enum eChallenge
	{
		ECG_QUARTER	= 0,
		ECG_SEMI,
		ECG_SECOND,
		ECG_WINNER,
		ECG_END,
	} EChallenge;

	typedef struct tagRank
	{
		tagRank():iItem(0),iMoney(0),iGradeTextNo(0) {}
		int iItem;
		int iMoney;
		int iGradeTextNo;
	} SRank;

	tagTBL_DEF_EMPORIA_REWARD()
		:	iFromNo(0)
		,	iTitleNo(0)
		,	iBodyNo(0)
		,	sLimitDay(0)
	{
	}

	int iFromNo;
	int iTitleNo;
	int iBodyNo;
	SRank kChallenge[ECG_END];
	SRank kGrade[MAX_EMPORIA_GRADE];
	short sLimitDay;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_EMPORIA_REWARD;

typedef struct tagTBL_DEF_EMPORIA_PACK
{
	tagTBL_DEF_EMPORIA_PACK()
		:	iBaseMapNo(0)
		,	iChallengeBattleGroundNo(0)
		,	iLimitCost(0)
	{
		::memset( iEmporiaGroundNo, 0, sizeof(iEmporiaGroundNo) );
		::memset( iBattleGroundNo, 0, sizeof(iBattleGroundNo) );
	}

	BM::GUID	guidEmporiaID;
	int			iBaseMapNo;
	int			iEmporiaGroundNo[MAX_EMPORIA_GRADE];
	int			iBattleGroundNo[MAX_EMPORIA_GRADE];
	int			iChallengeBattleGroundNo;
	int			iLimitCost;

	CONT_DEF_EMPORIA_FUNCTION	m_kContDefFunc[MAX_EMPORIA_GRADE];
	TBL_DEF_EMPORIA_REWARD		kReward;

	void WriteToPacket( BM::Stream &kPacket )const
	{
		kPacket.Push( guidEmporiaID );
		kPacket.Push( iBaseMapNo );
		kPacket.Push( iEmporiaGroundNo, sizeof(iEmporiaGroundNo) );
		kPacket.Push( iBattleGroundNo, sizeof(iBattleGroundNo) );
		kPacket.Push( iChallengeBattleGroundNo );
		kPacket.Push( iLimitCost );
		for ( int i = 0 ; i<MAX_EMPORIA_GRADE; ++i )
		{
			PU::TWriteTable_AM( kPacket, m_kContDefFunc[i] );
		}
		kPacket.Push( kReward );
	}

	bool ReadFromPacket( BM::Stream &kPacket )
	{
		if ( kPacket.RemainSize() < min_size() )
		{
			return false;
		}

		kPacket.Pop( guidEmporiaID );
		kPacket.Pop( iBaseMapNo );
		kPacket.PopMemory( iEmporiaGroundNo, sizeof(iEmporiaGroundNo) );
		kPacket.PopMemory( iBattleGroundNo, sizeof(iBattleGroundNo) );
		kPacket.Pop( iChallengeBattleGroundNo );
		kPacket.Pop( iLimitCost );
		for ( int i = 0 ; i<MAX_EMPORIA_GRADE; ++i )
		{
			m_kContDefFunc[i].clear();
			PU::TLoadTable_AM( kPacket, m_kContDefFunc[i] );
		}
		kPacket.Pop( kReward );
		return true;
	}

	size_t min_size()const
	{
		return	sizeof(guidEmporiaID)
			+	sizeof(iBaseMapNo)
			+	sizeof(iEmporiaGroundNo)
			+	sizeof(iBattleGroundNo)
			+	sizeof(iChallengeBattleGroundNo)
			+	sizeof(iLimitCost)
			+	(sizeof(size_t) * MAX_EMPORIA_GRADE)
			+	sizeof(kReward);
	}

}TBL_DEF_EMPORIA_PACK;

enum E_ACHIEVEMENT_CATEGORY_TYPE
{
	ACT_NONE = 0,			// ����
	ACT_GROWUP,				// ����
	ACT_ITEM,				// ������
	ACT_QUEST,				// ����Ʈ
	ACT_WOLRD,				// ����
	ACT_MISSION,			// �̼�
};

typedef struct tagTBL_DEF_ACHIEVEMENTS
{
	tagTBL_DEF_ACHIEVEMENTS():
	iIdx(0),
	iCategory(0),
	iTitleNo(0),
	iDiscriptionNo(0),
	iType(0),
	iValue(0),
	iItemNo(0),
	iSaveIdx(0),
	iRankPoint(0),
	iLevel(0),
	iUseTime(0),
	iGroupNo(0),
	i64CustomValue(0i64)
	{}

	int	iIdx;
	int	iCategory;		// Ÿ��
	int	iTitleNo;		// Ÿ��Ʋ ��Ʈ�� ���̺� ���̵�
	int	iDiscriptionNo;	// ���� ��Ʈ�� ���̺� ���̵�
	int	iType;
	
	int	iValue;
	int	iItemNo;
	int	iSaveIdx;
	int iRankPoint;		// ���� ��ŷ ����Ʈ
	int iLevel;
	int iUseTime;		// ��� �ð�

	int iGroupNo;
	__int64 i64CustomValue;


	BM::PgPackedTime kStartTime;
	BM::PgPackedTime kEndTime;

	bool CheckInTime(BM::PgPackedTime const & kCurTime) const
	{
		if( true == kStartTime.IsNull() || // �Ⱓ ������ ���� �༮�� 365�� ��� �޼� ����
			true == kEndTime.IsNull())
		{
			return true;
		}

		if(kStartTime < kCurTime && kCurTime < kEndTime)
		{
			return true;
		}

		return false;
	}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_ACHIEVEMENTS;

typedef struct tagCONT_DEF_ACHIEVEMENTS 
	: public std::map< TBL_KEY_INT, TBL_DEF_ACHIEVEMENTS >
{
	size_t min_size()const
	{
		return sizeof(size_t);//std::map< TBL_KEY_INT, TBL_DEF_ACHIEVEMENTS >
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteTable_MM(kPacket, *this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_MM(kPacket, *this);
	}
}CONT_DEF_ACHIEVEMENTS;

typedef struct tagCONT_DEF_ACHIEVEMENTS_SAVEIDX 
	: public std::map< TBL_KEY_INT, TBL_DEF_ACHIEVEMENTS >
{
	size_t min_size()const
	{
		return sizeof(size_t);//std::map< TBL_KEY_INT, TBL_DEF_ACHIEVEMENTS >
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteTable_MM(kPacket, *this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_MM(kPacket, *this);
	}
}CONT_DEF_ACHIEVEMENTS_SAVEIDX;

typedef struct tagTBL_DEF_ITEM2ACHIEVEMENT
{
	int	iItemNo,
		iSaveIdx;
	tagTBL_DEF_ITEM2ACHIEVEMENT():iItemNo(0),iSaveIdx(0)
	{
	}
	DEFAULT_TBL_PACKET_FUNC();
}STBL_DEF_ITEM2ACHIEVEMENT;

typedef std::vector<int> CONT_ITEMNO;
typedef struct tagRECOMMENDATION_ITEM
{
	enum{E_MAX_RECOMMENDATION_ITEM = 20};
	
	int			iIDX;
	int			iClass;
	int			iLvMin;
	int			iLvMax;
	CONT_ITEMNO kContItem;

	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(iIDX);
		kPacket.Push(iClass);
		kPacket.Push(iLvMin);
		kPacket.Push(iLvMax);
		kPacket.Push(kContItem);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iIDX);
		kPacket.Pop(iClass);
		kPacket.Pop(iLvMin);
		kPacket.Pop(iLvMax);
		kPacket.Pop(kContItem);
	}
}SRECOMMENDATION_ITEM;

typedef struct tagTBL_DEF_RARE_MONSTER_GEN
{
	int iMonNo;					// �߰��� ��� ���� ��ȣ
	int iGenGroupNo;			// �� �׷� ��ȣ
	int iDelayGenTime;			// �� �߰��� �� �ð����� ��ٸ� �� ���͸� ����
	int iRegenTime;				// ��Ȱ �ð�
	tagTBL_DEF_RARE_MONSTER_GEN()
	{
		iMonNo = 0;
		iGenGroupNo = 0;
		iDelayGenTime = 0;
		iRegenTime = 0;
	}
	DEFAULT_TBL_PACKET_FUNC();
}STBL_DEF_RARE_MONSTER_GEN;

typedef struct tagCONT_DEF_RARE_MONSTER_GEN : std::map< TBL_KEY_INT, STBL_DEF_RARE_MONSTER_GEN>
{
	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteTable_MM(kPacket,*this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_MM(kPacket,*this);
	}
}CONT_DEF_RARE_MONSTER_GEN;

typedef struct tagCONT_RARE_MONSTER_SPEECH : std::vector<int>
{
	enum{MAX_SPEECH_NUM = 5,};

	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_A(kPacket,*this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_A(kPacket,*this);
	}
}SCONT_RARE_MONSTER_SPEECH;

typedef struct tagOXEVENTNOTI
{
	std::wstring kMsg;
	int	iDelayTime;

	tagOXEVENTNOTI():iDelayTime(0){}

	size_t min_size()const
	{
		return 
			sizeof(size_t)+
			sizeof(iDelayTime);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(kMsg);
		kPacket.Push(iDelayTime);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(kMsg);
		kPacket.Pop(iDelayTime);
	}
}SOXEVENTNOTI;

int const MAX_OXEVENTNOTI_NUM = 10;

typedef std::vector<SOXEVENTNOTI> CONT_OXEVENTNOTI;

typedef struct tagOXQUIZEVENTINFO
{
	tagOXQUIZEVENTINFO():iEventID(0),iQuizNoMin(0),iQuizNoMax(0),iUserMin(0),iMaxStep(0){}
	
	int iEventID;
	int iQuizNoMin;
	int iQuizNoMax;
	int iUserMin;
	int iMaxStep;
	int iSaveMin;
	int iAnsTime;

	BM::PgPackedTime kStartDate;
	BM::PgPackedTime kEndDate;

	CONT_OXEVENTNOTI kContEventNoti;

	std::wstring kEventNotiClose;
	std::wstring kMailTitle;
	std::wstring kMailText;

	BYTE bState;
	BYTE bRepeat;

	void Clear()
	{
		iEventID = 0;
		iQuizNoMin = 0;
		iQuizNoMax = 0;
		iUserMin = 0;
		iSaveMin = 0;
		iMaxStep = 0;
		iAnsTime = 0;
		kContEventNoti.clear();
		kEventNotiClose.clear();
		kMailTitle.clear();
		kMailText.clear();
		bState = 0;
		bRepeat = 0;
	}

	size_t min_size()const
	{
		return 
		sizeof(iEventID)+
		sizeof(iQuizNoMin)+
		sizeof(iQuizNoMax)+
		sizeof(iUserMin)+
		sizeof(iSaveMin)+
		sizeof(iMaxStep)+
		sizeof(iAnsTime)+
		sizeof(size_t)+
		sizeof(kStartDate)+
		sizeof(kEndDate)+
		sizeof(size_t)+//kEventNotiClose)+
		sizeof(size_t)+//kMailTitle)+
		sizeof(size_t)+//kMailText)+
		sizeof(bState)+
		sizeof(bRepeat);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(iEventID);
		kPacket.Push(iQuizNoMin);
		kPacket.Push(iQuizNoMax);
		kPacket.Push(iUserMin);
		kPacket.Push(iSaveMin);
		kPacket.Push(iMaxStep);
		kPacket.Push(iAnsTime);
		PU::TWriteArray_M(kPacket,kContEventNoti);
		kPacket.Push(kStartDate);
		kPacket.Push(kEndDate);
		kPacket.Push(kEventNotiClose);
		kPacket.Push(bState);
		kPacket.Push(bRepeat);
		kPacket.Push(kMailTitle);
		kPacket.Push(kMailText);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iEventID);
		kPacket.Pop(iQuizNoMin);
		kPacket.Pop(iQuizNoMax);
		kPacket.Pop(iUserMin);
		kPacket.Pop(iSaveMin);
		kPacket.Pop(iMaxStep);
		kPacket.Pop(iAnsTime);
		PU::TLoadArray_M(kPacket,kContEventNoti,MAX_OXEVENTNOTI_NUM);
		kPacket.Pop(kStartDate);
		kPacket.Pop(kEndDate);
		kPacket.Pop(kEventNotiClose);
		kPacket.Pop(bState);
		kPacket.Pop(bRepeat);
		kPacket.Pop(kMailTitle);
		kPacket.Pop(kMailText);
	}
}SOXQUIZEVENTINFO;

typedef struct tagOXQUIZREWARDKEY
{
	tagOXQUIZREWARDKEY():iEventNo(0),iStep(0){}
	tagOXQUIZREWARDKEY(int const iE,int const iS):iEventNo(iE),iStep(iS){}
	int iEventNo,
		iStep;
	DEFAULT_TBL_PACKET_FUNC();
	bool const operator < (tagOXQUIZREWARDKEY const & rkKey) const
	{
		if(iEventNo < rkKey.iEventNo)
		{
			return true;
		}
		else if(iEventNo == rkKey.iEventNo)
		{
			if(iStep < rkKey.iStep)
			{
				return true;
			}
		}
		return false;
	}
}SOXQUIZREWARDKEY;

typedef struct tagOXQUIZREWARDITEM
{
	tagOXQUIZREWARDITEM():iItemNo(0),iItemCount(0){}
	int iItemNo;
	short iItemCount;
}SOXQUIZREWARDITEM;

typedef struct tagOXQUIZREWARD
{
	tagOXQUIZREWARD():iMoney(0){}
	enum{MAX_OXQUIZREWARDITEM_NUM = 5};
	int iMoney;
	SOXQUIZREWARDITEM	kItems[MAX_OXQUIZREWARDITEM_NUM];
	DEFAULT_TBL_PACKET_FUNC();
}SOXQUIZREWARD;

typedef struct tagOXQUIZINFO
{
	tagOXQUIZINFO():iQuizNo(0){}
	int iQuizNo;
	BYTE bResult;
	std::wstring kQuizText;
	std::wstring kQuizDiscription;
	size_t min_size()const
	{
		return 
		sizeof(iQuizNo)+
		sizeof(bResult)+
		sizeof(size_t)+//kQuizText
		sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(iQuizNo);
		kPacket.Push(bResult);
		kPacket.Push(kQuizText);
		kPacket.Push(kQuizDiscription);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iQuizNo);
		kPacket.Pop(bResult);
		kPacket.Pop(kQuizText);
		kPacket.Pop(kQuizDiscription);
	}
}SOXQUIZINFO;



typedef struct tagLUCKYSTAREVENTINFO
{
	tagLUCKYSTAREVENTINFO() :iEventID(0),iUserLimit(0),iCostMoney(0),
		iDelayTime(0),iLimitTime(0),iLuckyPoint(0),bState(0),bSendResultType(0),
		iLastRewardId(0),iEventCount(0),iLastResultStar(0),iFlag(0)
	{}
	
	int iEventID;
	int iUserLimit;
	int iCostMoney;
	BM::PgPackedTime kStartDate;
	BM::PgPackedTime kEndDate;
	std::wstring kEventNotiClose;
	std::wstring kMailTitle;
	std::wstring kMailText;
	int iDelayTime;
	int iLimitTime;
	int iLuckyPoint;

	BYTE bState;
	BYTE bSendResultType;
	int  iLastRewardId;
	int  iEventCount;
	int  iLastResultStar;
	int  iFlag;

	void Clear()
	{
		iEventID = 0;
		iUserLimit = 0;
		iCostMoney = 0;
		kStartDate.Clear();
		kEndDate.Clear();
		kEventNotiClose.clear();
		kMailTitle.clear();
		kMailText.clear();
		iDelayTime	  = 0;
		iLimitTime	  =	0;
		iLuckyPoint	  = 0;
		bState			= 0;
		bSendResultType = 0;
		iLastRewardId	= 0;
		iEventCount		= 0;
		iLastResultStar = 0;
		iFlag			= 0;
	}

	size_t min_size()const
	{
		return 
		sizeof(iEventID)+
		sizeof(iUserLimit)+
		sizeof(iCostMoney)+
		sizeof(kStartDate)+
		sizeof(kEndDate)+
		sizeof(size_t)+//kEventNotiClose)+
		sizeof(size_t)+//kMailTitle)+
		sizeof(size_t)+//kMailText)+
		sizeof(iDelayTime)+
		sizeof(iLimitTime)+
		sizeof(iLuckyPoint)+
		sizeof(bState)+
		sizeof(bSendResultType)+
		sizeof(iLastRewardId)+
		sizeof(iEventCount)+
		sizeof(iLastResultStar)+
		sizeof(iFlag);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(iEventID);
		kPacket.Push(iUserLimit);
		kPacket.Push(iCostMoney);
		kPacket.Push(kStartDate);
		kPacket.Push(kEndDate);
		kPacket.Push(kEventNotiClose);
		kPacket.Push(kMailTitle);
		kPacket.Push(kMailText);
		kPacket.Push(iDelayTime);	
		kPacket.Push(iLimitTime);	
		kPacket.Push(iLuckyPoint);	
		kPacket.Push(bState);
		kPacket.Push(bSendResultType);
		kPacket.Push(iLastRewardId);
		kPacket.Push(iEventCount);		
		kPacket.Push(iLastResultStar);
		kPacket.Push(iFlag);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iEventID);
		kPacket.Pop(iUserLimit);
		kPacket.Pop(iCostMoney);
		kPacket.Pop(kStartDate);
		kPacket.Pop(kEndDate);
		kPacket.Pop(kEventNotiClose);
		kPacket.Pop(kMailTitle);
		kPacket.Pop(kMailText);
		kPacket.Pop(iDelayTime);	
		kPacket.Pop(iLimitTime);	
		kPacket.Pop(iLuckyPoint);	
		kPacket.Pop(bState);
		kPacket.Pop(bSendResultType);
		kPacket.Pop(iLastRewardId);
		kPacket.Pop(iEventCount);		
		kPacket.Pop(iLastResultStar);
		kPacket.Pop(iFlag);
	}
}SLUCKYSTAREVENTINFO;


typedef struct tagLUCKYSTARJOINEDUSERINFO
{
	tagLUCKYSTARJOINEDUSERINFO():iEventID(0),iStar(0),iStep(0){}
	
	int iEventID;
	BM::GUID kCharGuid;
	int iStar;
	int iStep;

	void Clear()
	{
		iEventID = 0;
		kCharGuid.Clear();
		iStar = 0;
		iStep = 0;
	}

	size_t min_size()const
	{
		return 
		sizeof(iEventID)+
		sizeof(kCharGuid)+
		sizeof(iStar)+
		sizeof(iStep);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		kPacket.Push(iEventID);
		kPacket.Push(kCharGuid);
		kPacket.Push(iStar);
		kPacket.Push(iStep);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iEventID);
		kPacket.Pop(kCharGuid);
		kPacket.Pop(iStar);
		kPacket.Pop(iStep);
	}
}SLUCKYSTARJOINEDUSERINFO;

typedef struct tagLUCKYSTARREWARDKEY
{
	tagLUCKYSTARREWARDKEY():iRewardNo(0),iStep(0){}
	tagLUCKYSTARREWARDKEY(int const iE,int const iS):iRewardNo(iE),iStep(iS){}
	int iRewardNo,
		iStep;
	DEFAULT_TBL_PACKET_FUNC();
	bool const operator < (tagLUCKYSTARREWARDKEY const & rkKey) const
	{
		if(iRewardNo < rkKey.iRewardNo)
		{
			return true;
		}
		else if(iRewardNo == rkKey.iRewardNo)
		{
			if(iStep < rkKey.iStep)
			{
				return true;
			}
		}
		return false;
	}
}SLUCKYSTARREWARDKEY;

typedef struct tagLUCKYSTARREWARD
{
	tagLUCKYSTARREWARD():iMoney(0),iItemNo(0),iItemCount(0){}
	int iMoney;
	int iItemNo;
	short iItemCount;
	DEFAULT_TBL_PACKET_FUNC();
}SLUCKYSTARREWARD;

typedef struct tagCONT_LUCKYSTAR_REWARD : std::vector<SLUCKYSTARREWARD>
{
	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_A(kPacket,*this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_A(kPacket,*this);
	}
}SCONT_LUCKYSTAR_REWARD;

typedef struct tagCONT_LUCKYSTAR_REWARD_STEP : std::vector<int>
{
	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_A(kPacket,*this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_A(kPacket,*this);
	}
}SCONT_LUCKYSTAR_REWARD_STEP;

typedef struct tagCONT_LUCKYSTAR_EVENT_JOIN_REWARD : std::vector<int>
{
	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_A(kPacket,*this);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_A(kPacket,*this);
	}
}SCONT_LUCKYSTAR_EVENT_JOIN_REWARD;

typedef struct tagMACRO_CHECK_TABLE
{
	int iMacroCheckTime;			// ��ũ�� ���� �ð�
	int iMacroReleaseTime;			// ��ũ�� ���� �ð�
	int iMacroCheckCount;			// ��ũ�� ���� ī��Ʈ
	int iMacroPopupMax;				// ��ũ�� �˾�â ��� �ִ� ī��Ʈ BM::Rand_Range(iMacroPopupMax,iMacroCheckCount)
	int iMacroInputTime;			// ��ũ�� ���� ���� �ð�
	tagMACRO_CHECK_TABLE():iMacroCheckTime(0),iMacroReleaseTime(0),iMacroCheckCount(0),iMacroPopupMax(0),iMacroInputTime(0){}
	bool IsEnable(){return (iMacroCheckTime && iMacroReleaseTime && iMacroCheckCount && iMacroPopupMax && iMacroInputTime);}
	DEFAULT_TBL_PACKET_FUNC();
}SMACRO_CHECK_TABLE;

typedef struct tagCARD_ABIL_KEY
{
	BYTE kConstellation,
		 kHobby,
		 kBlood,
		 kStyle;

	tagCARD_ABIL_KEY():kConstellation(0),kHobby(0),kBlood(0),kStyle(0){}
	tagCARD_ABIL_KEY(	BYTE const bConstellation,
						BYTE const bHobby,
						BYTE const bBlood,
						BYTE const bStyle):
	kConstellation(bConstellation),kHobby(bHobby),kBlood(bBlood),kStyle(bStyle){}

	bool operator < (tagCARD_ABIL_KEY const & rhs) const
	{
		if(kConstellation < rhs.kConstellation)
		{
			return true;
		}
		else if(kConstellation == rhs.kConstellation)
		{
			if(kHobby < rhs.kHobby)
			{
				return true;
			}
			else if(kHobby == rhs.kHobby)
			{
				if(kBlood < rhs.kBlood)
				{
					return true;
				}
				else if(kBlood == rhs.kBlood)
				{
					if(kStyle < rhs.kStyle)
					{
						return true;
					}
				}
			}
		}

		return false;
	}
	DEFAULT_TBL_PACKET_FUNC();
}TBL_CARD_ABIL_KEY;

typedef struct tagCHARACTER_CARD_ABIL
{
	tagCHARACTER_CARD_ABIL():wAbilType(0),iValue(0){}
	tagCHARACTER_CARD_ABIL(WORD const kAbilType,int const kValue):wAbilType(kAbilType),iValue(kValue){}
	WORD wAbilType;
	int iValue;
	DEFAULT_TBL_PACKET_FUNC();
}SCHARACTER_CARD_ABIL;

typedef enum eCARD_ABIL_KEY_TYPE
{
	CAKT_NONE = 0,
	CAKT_BLOOD = 1,
	CAKT_CONSTELLATION = 2,
	CAKT_HOBBY = 3,
	CAKT_STYLE = 4,
}ECARD_ABIL_KEY_TYPE;

typedef struct tagDEF_CARD_KEY_STRING_KEY
{
	BYTE bKeyType;
	BYTE bKeyValue;

	tagDEF_CARD_KEY_STRING_KEY():bKeyType(0),bKeyValue(0){}
	tagDEF_CARD_KEY_STRING_KEY(BYTE const kType,BYTE const kValue):bKeyType(kType),bKeyValue(kValue){}

	bool operator < (tagDEF_CARD_KEY_STRING_KEY const & rhs) const
	{
		if(bKeyType < rhs.bKeyType)
		{
			return true;
		}
		else if(bKeyType == rhs.bKeyType)
		{
			if(bKeyValue < rhs.bKeyValue)
			{
				return true;
			}
		}
		return false;
	}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CARD_KEY_STRING_KEY;

typedef struct tagDEF_CARD_KEY_STRING
{
	int iStringNo;
	BYTE bCardKey;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CARD_KEY_STRING;

typedef struct tagCARD_LOCAL
{
	int iLocal;
	std::wstring kText;
	size_t min_size()const
	{
		return 
		sizeof(iLocal)+
		sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iLocal);
		kPacket.Push(kText);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iLocal);
		kPacket.Pop(kText);
	}
}SCARD_LOCAL;

int const MAX_GEMS_NUM = 5;

typedef std::map<int,short> CONT_GEMS;

typedef struct tagGEMSTORE_ARTICLE
{
	tagGEMSTORE_ARTICLE():iCP(0){}

	int			iCP;
	CONT_GEMS	kContGems;

	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iCP);
		PU::TWriteTable_AA(kPacket,kContGems);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iCP);
		PU::TLoadTable_AA(kPacket,kContGems);
	}
}SGEMSTORE_ARTICLE;

typedef std::map< TBL_KEY_INT, SGEMSTORE_ARTICLE> CONT_GEMSTORE_ARTICLE;

typedef struct tagGEMSTORE
{
	CONT_GEMSTORE_ARTICLE kContArticles;

	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteTable_MM(kPacket,kContArticles);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_MM(kPacket,kContArticles);
	}
}SGEMSTORE;

int const MAX_DEFGEMS_NUM = 5;

typedef struct tagDefGemsData
{
	tagDefGemsData() : iGemNo(0), sCount(0) {}
	int iGemNo;
	short sCount;

	bool operator == ( tagDefGemsData const& rhs )const
	{
		return (iGemNo == rhs.iGemNo) && (sCount == rhs.sCount);
	}

	DEFAULT_TBL_PACKET_FUNC();
}SDefGemsData;

typedef std::vector<SDefGemsData> CONT_DEFGEMS;

typedef struct tagDEFGEMSTORE_ARTICLE
{
	tagDEFGEMSTORE_ARTICLE():iMenu(0),iOrderIndex(0),iItemCount(0){}

	int				iMenu;
	int				iOrderIndex;
	int				iItemCount;
	CONT_DEFGEMS	kContGems;

	size_t min_size()const
	{
		return sizeof(int) + sizeof(int) + sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iMenu);
		kPacket.Push(iOrderIndex);
		kPacket.Push(iItemCount);
		PU::TWriteArray_M(kPacket,kContGems);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iMenu);
		kPacket.Pop(iOrderIndex);
		kPacket.Pop(iItemCount);
		PU::TLoadArray_M(kPacket,kContGems);
	}
}SDEFGEMSTORE_ARTICLE;

typedef struct tagDEFGEMSTORE_ARTICLE_KEY
{
	tagDEFGEMSTORE_ARTICLE_KEY():
	iItemNo(0),
	iMenu(0),
	iOrderIndex(0){}

	explicit tagDEFGEMSTORE_ARTICLE_KEY(int _ItemNo, int _iMenu, int _iOrderIndex):iItemNo(_ItemNo), iMenu(_iMenu), iOrderIndex(_iOrderIndex){}
	bool operator < (tagDEFGEMSTORE_ARTICLE_KEY const & rhs) const
	{
		if( iItemNo < rhs.iItemNo )	{return true;}
		if( iItemNo > rhs.iItemNo )	{return false;}

		if( iMenu < rhs.iMenu )	{return true;}
		if( iMenu > rhs.iMenu )	{return false;}

		if( iOrderIndex < rhs.iOrderIndex )	{return true;}
		if( iOrderIndex > rhs.iOrderIndex )	{return false;}

		return false;
	}

	size_t min_size()const
	{
		return sizeof(int) + sizeof(int) + sizeof(int);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iItemNo);
		kPacket.Push(iMenu);
		kPacket.Push(iOrderIndex);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iItemNo);
		kPacket.Pop(iMenu);
		kPacket.Pop(iOrderIndex);
	}

	int iItemNo;
	int iMenu;
	int iOrderIndex;
}SDEFGEMSTORE_ARTICLE_KEY;

typedef std::map< SDEFGEMSTORE_ARTICLE_KEY, SDEFGEMSTORE_ARTICLE> CONT_DEFGEMSTORE_ARTICLE;

typedef struct tagDEFGEMSTORE
{
	CONT_DEFGEMSTORE_ARTICLE kContArticles;

	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteTable_MM(kPacket,kContArticles);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_MM(kPacket,kContArticles);
	}
}SDEFGEMSTORE;

typedef std::map<int, std::pair<int,int> > CONT_DEF_ANTIQUE_ARTICLE;		//first: iIndex  |  second: iItemNo, iItemCount
typedef struct tagDEF_ANTIQUE
{
	tagDEF_ANTIQUE() : iNeedItem(0), iNeedCount(0) {}

	int iNeedItem;
	int iNeedCount;
	CONT_DEF_ANTIQUE_ARTICLE kContArticles;

	size_t min_size()const
	{
		return sizeof(int) + sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iNeedItem);
		kPacket.Push(iNeedCount);
		PU::TWriteTable_AA(kPacket,kContArticles);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iNeedItem);
		kPacket.Pop(iNeedCount);
		PU::TLoadTable_AA(kPacket,kContArticles);
	}
}SDEF_ANTIQUE;

typedef enum
{
	ET_EMOTICON,
	ET_EMOTION,
	ET_BALLOON,

	ET_MAX,
}EEMOTION_TYPE;

typedef struct tagEMOTIONKEY
{
	BYTE bType;
	int iEMotionNo;
	tagEMOTIONKEY():bType(0),iEMotionNo(0){}
	explicit tagEMOTIONKEY(BYTE const _type,int const _emotionno):bType(_type),iEMotionNo(_emotionno){}
	bool operator < (tagEMOTIONKEY const & rhs) const
	{
		if(bType < rhs.bType)
		{
			return true;
		}
		else if(bType == rhs.bType)
		{
			if(iEMotionNo < rhs.iEMotionNo)
			{
				return true;
			}
		}
		return false;
	}
}SEMOTIONKEY;

typedef struct tagEMOTIONGROUPKEY
{
	BYTE bType;
	int iGroupNo;
	tagEMOTIONGROUPKEY():bType(0),iGroupNo(0){}
	explicit tagEMOTIONGROUPKEY(BYTE const _type,int const _groupno):bType(_type),iGroupNo(_groupno){}
	bool operator < (tagEMOTIONGROUPKEY const & rhs) const
	{
		if(bType < rhs.bType)
		{
			return true;
		}
		else if(bType == rhs.bType)
		{
			if(iGroupNo < rhs.iGroupNo)
			{
				return true;
			}
		}
		return false;
	}
}SEMOTIONGROUPKEY;

typedef struct tagGAMBLE
{
	tagGAMBLE()
		:	iContainerNo(0)
		,	bTimeType(0)
		,	sUseTime(0)
		,	iRarityControl(0)
		,	iPlusUpControl(0)
	{}

	int		iContainerNo;
	BYTE	bTimeType;
	short	sUseTime;
	int		iRarityControl;
	int		iPlusUpControl;
}SGAMBLE;

typedef struct tagHIDDENREWORDBAG
{
	tagHIDDENREWORDBAG()
		:	iContainerNo(0)
		,	iRarityControl(0)
		,	iPlusUpControl(0)
	{}

	int		iContainerNo;
	int		iRarityControl;
	int		iPlusUpControl;
}SHIDDENREWORDBAG;

typedef struct tagTBL_LIMITED_ITEM
{
	tagTBL_LIMITED_ITEM()
	{
		iEventNo = 0;
		iBagNo = 0;
		iLimit_ResetPeriod = 0;
		iSafe_ResetPeriod = 0;
		iRefreshCount = 0;
	}

	int iEventNo;
	int iBagNo;
	BM::DBTIMESTAMP_EX kStartDate;
	BM::DBTIMESTAMP_EX kEndDate;
	int	iLimit_ResetPeriod;//�д���
	int	iSafe_ResetPeriod;//�д���
	int iRefreshCount;
}TBL_LIMITED_ITEM;


typedef struct tagLimitItemControlKey
{
	tagLimitItemControlKey(int const iInType = 0, int const InNo = 0)
	{
		iObjectType = iInType;
		iObjectNo = InNo;
	}

	bool operator < (const tagLimitItemControlKey &rhs)const
	{
		if( iObjectType < rhs.iObjectType )	{return true;}
		if( iObjectType > rhs.iObjectType )	{return false;}

		if( iObjectNo < rhs.iObjectNo )	{return true;}
		if( iObjectNo > rhs.iObjectNo )	{return false;}

		return false;
	}

	int iObjectType;
	int iObjectNo;
	
	DEFAULT_TBL_PACKET_FUNC();
}SLimitItemControlKey;


typedef struct tagTBL_LIMITED_ITEM_CONTROL
{
	tagTBL_LIMITED_ITEM_CONTROL()
	{
		iObjectType = 0;
		iObjectNo = 0;
		iEventNo = 0;//�ð�
		iRate = 0;
	}

	int iObjectType;
	int iObjectNo;
	int iEventNo;
	int iRate;
}TBL_LIMITED_ITEM_CONTROL;
typedef std::vector<TBL_LIMITED_ITEM_CONTROL> VEC_LIMITED_ITEM_CONTROL;

typedef struct tagDEF_CONTVERTITEM
{
	int		iSourceItemNo,
			iTargetItemNo;
	short	sSourceItemNum,
			sTargetItemNum;

	tagDEF_CONTVERTITEM():iSourceItemNo(0),iTargetItemNo(0),sSourceItemNum(0),sTargetItemNum(0){}
	explicit tagDEF_CONTVERTITEM(int const _sourceitemno,short const _sourceitemnum,int const _targetitemno,short const _targetitemnum):
	iSourceItemNo(_sourceitemno),iTargetItemNo(_targetitemno),sSourceItemNum(_sourceitemnum),sTargetItemNum(_targetitemnum){}
}TBL_DEF_CONVERTITEM;

typedef struct tagGAMBLE_SHOUT_KEY
{
	int iGambleNo,
		iItemNo;
	tagGAMBLE_SHOUT_KEY(){}
	tagGAMBLE_SHOUT_KEY(int iGambleNo_, int iItemNo_)
		: iGambleNo(iGambleNo_), iItemNo(iItemNo_)
	{
	}
	bool operator <(tagGAMBLE_SHOUT_KEY const & rhs) const
	{
		if(iGambleNo < rhs.iGambleNo)
		{
			return true;
		}
		else if(iGambleNo == rhs.iGambleNo)
		{
			if(iItemNo < rhs.iItemNo)
			{
				return true;
			}
		}
		return false;
	}
}SGAMBLE_SHOUT_KEY;

int const PET_HATCH_MAX_CLASS = 5;
int const PET_HATCH_MAX_PERIOD = 10;
int const PET_HATCH_MAX_BONUSSTATUS = 3;
typedef struct tagDEF_PET_HATCH
{
	tagDEF_PET_HATCH()
		:	iID(0)
	{
		::memset( iClass, 0, sizeof(iClass) );
		::memset( sLevel, 0, sizeof(sLevel) );
		::memset( iClassRate, 0, sizeof(iClassRate) );
		::memset( iBonusStatusRateID, 0, sizeof(iBonusStatusRateID) );

		::memset( sPeriod, 0, sizeof(sPeriod) );
		::memset( iPeriodRate, 0 , sizeof(iPeriodRate) );
	}

	int		iID;
	
	int		iClass[PET_HATCH_MAX_CLASS];
	short	sLevel[PET_HATCH_MAX_CLASS];
	int		iClassRate[PET_HATCH_MAX_CLASS];
	int		iBonusStatusRateID[PET_HATCH_MAX_CLASS][PET_HATCH_MAX_BONUSSTATUS];

	short	sPeriod[PET_HATCH_MAX_PERIOD];
	int		iPeriodRate[PET_HATCH_MAX_PERIOD];
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PET_HATCH;

int const MAX_PET_BONUSSTATUS_ITEMOPT2_NUM = 5;
typedef struct tagDEF_PET_BONUSSTATUS
{
	tagDEF_PET_BONUSSTATUS()
		:	iID(0)
		,	bySelectRate(0)
	{
		::memset( iItemOpt2_OptionType, 0, sizeof(iItemOpt2_OptionType) );
		::memset( iItemOpt2_Rate, 0, sizeof(iItemOpt2_Rate) );
		::memset( iItemOpt2_ValueLvRate, 0, sizeof(iItemOpt2_ValueLvRate) );
	}

	int	iID;

	BYTE	bySelectRate;

	int		iItemOpt2_OptionType[MAX_PET_BONUSSTATUS_ITEMOPT2_NUM];
	int		iItemOpt2_Rate[MAX_PET_BONUSSTATUS_ITEMOPT2_NUM];

	int		iItemOpt2_ValueLvRate[MAX_ITEM_OPTION_ABIL_NUM];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PET_BONUSSTATUS;

/*
int const PET_HATCH_MAX_BONUSSTATUS_MAX_VALUE_COUNT = 10;
typedef struct tagDEF_PET_BONUSSTATUSVALUE
{
	tagDEF_PET_BONUSSTATUSVALUE()
		:	iID(0)
	{
		::memset( iValue, 0, sizeof(iValue) );
		::memset( iValueRate, 0, sizeof(iValueRate) );
	}

	int	iID;
	int iValue[PET_HATCH_MAX_BONUSSTATUS_MAX_VALUE_COUNT];
	int iValueRate[PET_HATCH_MAX_BONUSSTATUS_MAX_VALUE_COUNT];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PET_BONUSSTATUSVALUE;
*/

typedef struct tagCREATE_CHARACTER_EVENT_REWARD
{
	tagCREATE_CHARACTER_EVENT_REWARD():
	iItemNo(0)
	,siCount(0)
	,bTimeType(0)
	,iUseTime(0)
	,iRarityControlNo(0){}

	int iItemNo;
	short siCount;
	BYTE bTimeType;
	int	iUseTime;
	int iRarityControlNo;
	DEFAULT_TBL_PACKET_FUNC();
}TBL_CCE_REWARD;

typedef std::vector<TBL_CCE_REWARD> CONT_CCE_REWARD;

typedef struct tagCREATE_CHARACTER_EVENT_REWARD_TABLE
{
	CONT_CCE_REWARD kRewards;
	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteArray_A(kPacket,kRewards);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadArray_A(kPacket,kRewards);
	}
	size_t min_size()const
	{
		return sizeof(size_t);
	}
}TBL_CCE_REWARD_TABLE;

typedef std::map<int,int> CONT_REALTY;

typedef struct tagREALTYDEALER
{
	tagREALTYDEALER(){}

	CONT_REALTY kTable;

	size_t min_size()const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket)const
	{
		PU::TWriteTable_AA(kPacket,kTable);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_AA(kPacket,kTable);
	}
}TBL_REALTYDEALER;

int const MAX_RAREOPT_MAGIC_ARRAY_NUM = 10;
typedef struct tagTBL_DEF_MAGICOPTION
{
	tagTBL_DEF_MAGICOPTION():iSuccessControlNo(0)
	{
		memset(iMagicNo,0,sizeof(iMagicNo));
	}
	int iSuccessControlNo;
	int	iMagicNo[MAX_RAREOPT_MAGIC_ARRAY_NUM];
}TBL_DEF_MAGICOPTION;

int const MAX_RAREOPT_SKILL_LEVEL = 5;
int const MAX_RAREOPT_SKILL_ARRAY_NUM = 10;
typedef struct tagTBL_DEF_SKILLOPTION
{
	tagTBL_DEF_SKILLOPTION():iSuccessControlNo(0),iSkillLvSuccessControlNo(0)
	{
		memset(iSkillIdx,0,sizeof(iSkillIdx));
	}
	int		iSuccessControlNo;
	int		iSkillIdx[MAX_RAREOPT_SKILL_ARRAY_NUM];
	int		iSkillLvSuccessControlNo;
}TBL_DEF_SKILLOPTION;

int const MAX_MIXUP_ITEM_ARRAY_NUM = 10;

typedef struct tagTBL_DEF_MIXUP_ITEM_KEY
{
	int iItemGrade,
		iGenderLimit,
		iEquipPos;

	tagTBL_DEF_MIXUP_ITEM_KEY():iItemGrade(0),iGenderLimit(0),iEquipPos(0){}
	explicit tagTBL_DEF_MIXUP_ITEM_KEY(int const _grade,int const _gender,int const _iequippos):iItemGrade(_grade),iGenderLimit(_gender),iEquipPos(_iequippos){}

	bool operator < (tagTBL_DEF_MIXUP_ITEM_KEY const & rhs)const
	{
		if(iItemGrade < rhs.iItemGrade)
		{
			return true;
		}
		else if(iItemGrade == rhs.iItemGrade)
		{
			if(iEquipPos < rhs.iEquipPos)
			{
				return true;
			}
			else if(iEquipPos == rhs.iEquipPos)
			{
				if(iGenderLimit < rhs.iGenderLimit)
				{
					return true;
				}
			}
		}
		return false;
	}
}TBL_DEF_MIXUP_ITEM_KEY;

typedef struct tagTBL_DEF_MIXUP_ITEM
{
	int	iSuccessControlNo,
		iRarityControlNo,
		iItemBagNo[MAX_MIXUP_ITEM_ARRAY_NUM];
	tagTBL_DEF_MIXUP_ITEM():iSuccessControlNo(0),iRarityControlNo(0){}
}TBL_DEF_MIXUP_ITEM;

typedef struct tagOPTION_CANDI_KEY
{
	int iEquipPos;
	int iOptGroup;

	tagOPTION_CANDI_KEY():iEquipPos(0),iOptGroup(0){}
	explicit tagOPTION_CANDI_KEY(int const _equippos,int const _optgroup):iEquipPos(_equippos),iOptGroup(_optgroup){}
	bool operator < (tagOPTION_CANDI_KEY const & rhs) const
	{
		if( iEquipPos < rhs.iEquipPos )	{return true;}
		if( iEquipPos > rhs.iEquipPos )	{return false;}

		if( iOptGroup < rhs.iOptGroup )	{return true;}
		if( iOptGroup > rhs.iOptGroup )	{return false;}
		
		return false;
	}
}OPTION_CANDI_KEY;

typedef struct tagTBL_DEF_MYHOME_DEFAULT_ITEM
{
	int iItemNo;
	tagTBL_DEF_MYHOME_DEFAULT_ITEM():iItemNo(0){}
	explicit tagTBL_DEF_MYHOME_DEFAULT_ITEM(int const __itemno):iItemNo(__itemno){}
	operator int()const{return iItemNo;}
}TBL_DEF_MYHOME_DEFAULT_ITEM;

typedef struct tagTBL_DEF_MYHOME_TEX
{
	int iTex;
	tagTBL_DEF_MYHOME_TEX():iTex(0){}
	explicit tagTBL_DEF_MYHOME_TEX(int const __tex):iTex(__tex){}
	operator int()const{return iTex;}
}TBL_DEF_MYHOME_TEX;

typedef struct tagTBL_DEF_HOMETOWNTOMAPCOST
{
	int iCost;
	tagTBL_DEF_HOMETOWNTOMAPCOST():iCost(0){}
	explicit tagTBL_DEF_HOMETOWNTOMAPCOST(int const __cost):iCost(__cost){}
	operator int()const{return iCost;}
}TBL_DEF_HOMETOWNTOMAPCOST;

typedef std::set< int >										CONT_SET_DATA;
typedef struct sMonsterControl
{
	sMonsterControl()
	{
	}

	CONT_SET_DATA kSetMonster[MAX_MONSTERBAG_ELEMENT];
} SMonsterControl;

typedef struct tagMISSION_CLASS_REWARD_DBCACHE_KEY
{
	tagMISSION_CLASS_REWARD_DBCACHE_KEY():
	iMissionNo(0){}

	explicit tagMISSION_CLASS_REWARD_DBCACHE_KEY(int _MissionNo):iMissionNo(_MissionNo){}
	bool operator < (tagMISSION_CLASS_REWARD_DBCACHE_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		return false;
	}

	int iMissionNo;
	std::wstring kLevelNo;
	std::wstring kClassNo;
}SMISSION_CLASS_REWARD_DBCACHE_KEY;

typedef struct tagMISSION_CLASS_REWARD_KEY
{
	tagMISSION_CLASS_REWARD_KEY() :
	iMissionNo(0), iLevelNo(0), iClassNo(0) {}

	explicit tagMISSION_CLASS_REWARD_KEY(int _MissionNo, int _iLevelNo, int _iClassNo):
	iMissionNo(_MissionNo), iLevelNo(_iLevelNo), iClassNo(_iClassNo) {}
	
	bool operator < (tagMISSION_CLASS_REWARD_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{ return true; }
		if( iMissionNo > rhs.iMissionNo )	{ return false; }

		if( iLevelNo < rhs.iLevelNo )		{ return true; }
		if( iLevelNo > rhs.iLevelNo )		{ return false; }

		if( iClassNo < rhs.iClassNo )		{ return true; }
		if( iClassNo > rhs.iClassNo )		{ return false; }

		return false;
	}

	int iMissionNo;
	int iLevelNo;
	int iClassNo;
}SMISSION_CLASS_REWARD_KEY;

typedef struct tagMISSION_CLASS_REWARD_ITEM
{
	tagMISSION_CLASS_REWARD_ITEM():
	iRate(0) {}
	
	int iItemBagNo;
	int	iRate;
}SMISSION_CLASS_REWARD_ITEM;

typedef std::vector< SMISSION_CLASS_REWARD_ITEM > CONT_MISSION_CLASS_REWARD_ITEM;

typedef struct tagMISSION_CLASS_REWARD_BAG
{
	tagMISSION_CLASS_REWARD_BAG():iTotalRate(0){}
	int iTotalRate;
	CONT_MISSION_CLASS_REWARD_ITEM	kCont;

	void Clear()
	{
		kCont.clear();
		iTotalRate = 0;
	}	

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_CLASS_REWARD_BAG;

typedef struct tagMISSION_RANK_REWARD_KEY
{
	tagMISSION_RANK_REWARD_KEY():
	iMissionNo(0),
	iLevelNo(0),
	iRankNo(0){}
	explicit tagMISSION_RANK_REWARD_KEY(int _MissionNo, int _LevelNo, int _Rank):iMissionNo(_MissionNo), iLevelNo(_LevelNo), iRankNo(_Rank){}
	bool operator < (tagMISSION_RANK_REWARD_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		if( iLevelNo < rhs.iLevelNo )	{return true;}
		if( iLevelNo > rhs.iLevelNo )	{return false;}
		
		if( iRankNo < rhs.iRankNo )	{return true;}
		if( iRankNo > rhs.iRankNo )	{return false;}

		return false;
	}

	int iMissionNo;
	int iLevelNo;
	int iRankNo;
}SMISSION_RANK_REWARD_KEY;

typedef struct tagMISSION_RANK_REWARD_ITEM
{
	tagMISSION_RANK_REWARD_ITEM():
	iItemNo(0),
	iRate(0),
	sCount(0){}

	int iItemNo;
	int iRate;
	short sCount;
}SMISSION_RANK_REWARD_ITEM;

typedef std::vector< SMISSION_RANK_REWARD_ITEM > CONT_MISSION_RANK_REWARD_ITEM;

typedef struct tagMISSION_RANK_REWARD_BAG
{
	tagMISSION_RANK_REWARD_BAG():iTotalRate(0){}
	int iTotalRate;
	CONT_MISSION_RANK_REWARD_ITEM	kCont;

	void Clear()
	{
		kCont.clear();
		iTotalRate = 0;
	}	

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_RANK_REWARD_BAG;

typedef struct tagMISSION_DEFENCE_STAGE_KEY
{
	tagMISSION_DEFENCE_STAGE_KEY():
	iMissionNo(0),
	iParty_Number(0),
	iStageNo(0){}
	explicit tagMISSION_DEFENCE_STAGE_KEY(int _MissionNo, int _Party_Number, int _StageNo):iMissionNo(_MissionNo), iParty_Number(_Party_Number), iStageNo(_StageNo){}
	bool operator < (tagMISSION_DEFENCE_STAGE_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		if( iParty_Number < rhs.iParty_Number )	{return true;}
		if( iParty_Number > rhs.iParty_Number )	{return false;}

		if( iStageNo < rhs.iStageNo )	{return true;}
		if( iStageNo > rhs.iStageNo )	{return false;}

		return false;
	}

	void Set(int _MissionNo, int _Party_Number, int _StageNo)
	{
		iMissionNo = _MissionNo;
		iParty_Number = _Party_Number;
		iStageNo = _StageNo;
	}

	int iMissionNo;
	int iParty_Number;
	int iStageNo;
}SMISSION_DEFENCE_STAGE_KEY;

typedef struct tagMISSION_DEFENCE_STAGE
{
	tagMISSION_DEFENCE_STAGE():	
	iStage_Time(0),
	iWave_GroupNo(0),
	iAddMonster_GroupNo(0),
	iTimeToExp_Rate(0),
	//iStage_Delay(0),
	iResultNo(0),
	iResultCount(0){}
	
	int iStage_Time;
	int iWave_GroupNo;
	int iAddMonster_GroupNo;
	int iTimeToExp_Rate;
	//int iStage_Delay;
	int iResultNo;
	int iResultCount;
	int iDirection_Item;
}SMISSION_DEFENCE_STAGE;

typedef std::vector< SMISSION_DEFENCE_STAGE > CONT_MISSION_DEFENCE_STAGE;

typedef struct tagMISSION_DEFENCE_STAGE_BAG
{
	tagMISSION_DEFENCE_STAGE_BAG(){}
	CONT_MISSION_DEFENCE_STAGE	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE_STAGE_BAG;

typedef struct tagMISSION_DEFENCE_WAVE_KEY
{
	tagMISSION_DEFENCE_WAVE_KEY():
	iWave_GroupNo(0),
	iWave_No(0){}
	explicit tagMISSION_DEFENCE_WAVE_KEY(int _Wave_GroupNo, int _Wave_No):iWave_GroupNo(_Wave_GroupNo), iWave_No(_Wave_No){}
	bool operator < (tagMISSION_DEFENCE_WAVE_KEY const & rhs) const
	{
		if( iWave_GroupNo < rhs.iWave_GroupNo )	{return true;}
		if( iWave_GroupNo > rhs.iWave_GroupNo )	{return false;}

		if( iWave_No < rhs.iWave_No )	{return true;}
		if( iWave_No > rhs.iWave_No )	{return false;}

		return false;
	}

	int iWave_GroupNo;
	int iWave_No;
}SMISSION_DEFENCE_WAVE_KEY;

int const MAX_WAVE_MONSTER_NUM = 10;

typedef struct tagMISSION_DEFENCE_WAVE
{
	tagMISSION_DEFENCE_WAVE():iWave_Delay(0), iAddMoveSpeedPercent(0), iTunningNo(0), iAddExpPercent(0), iAddHPPercent(0), iAddDamagePercent(0)
	{
		::memset(iMonster,0,sizeof(iMonster));
	}
	int iWave_Delay;
	int iAddMoveSpeedPercent;
	int iTunningNo;
	int iMonster[MAX_WAVE_MONSTER_NUM];
	int iAddExpPercent;
	int iAddHPPercent;
	int iAddDamagePercent;
}SMISSION_DEFENCE_WAVE;

typedef std::vector< SMISSION_DEFENCE_WAVE > CONT_MISSION_DEFENCE_WAVE;

typedef struct tagMISSION_DEFENCE_WAVE_BAG
{
	tagMISSION_DEFENCE_WAVE_BAG(){}
	CONT_MISSION_DEFENCE_WAVE	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE_WAVE_BAG;


typedef struct tagMISSION_DEFENCE7_MISSION_KEY
{
	tagMISSION_DEFENCE7_MISSION_KEY():
	iMissionNo(0),
	iMissionType(0){}
	explicit tagMISSION_DEFENCE7_MISSION_KEY(int _MissionNo, int _MissionType):iMissionNo(_MissionNo), iMissionType(_MissionType){}
	bool operator < (tagMISSION_DEFENCE7_MISSION_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		if( iMissionType < rhs.iMissionType )	{return true;}
		if( iMissionType > rhs.iMissionType )	{return false;}

		return false;
	}

	void Set(int _MissionNo, int _MissionType)
	{
		iMissionNo = _MissionNo;
		iMissionType = _MissionType;
	}

	int iMissionNo;
	int iMissionType;
}SMISSION_DEFENCE7_MISSION_KEY;

int const MAX_DEFENCE7_MISSION_SKILL = 8;

typedef struct tagMISSION_DEFENCE7_MISSION
{
	tagMISSION_DEFENCE7_MISSION():	
	iMin_Level(0),
	iMax_Level(0),
	iSlot_F1(0),
	iSlot_F2(0),
	iSlot_F3(0),
	iSlot_F4(0),
	iSlot_F5(0),
	iSlot_F6(0),
	iSlot_F7(0),
	iSlot_F8(0)
	{
		::memset(iSkill,0,sizeof(iSkill));
	}
	
	int iMin_Level;
	int iMax_Level;
	int iSlot_F1;
	int iSlot_F2;
	int iSlot_F3;
	int iSlot_F4;
	int iSlot_F5;
	int iSlot_F6;
	int iSlot_F7;
	int iSlot_F8;
	int iSkill[MAX_DEFENCE7_MISSION_SKILL];
}SMISSION_DEFENCE7_MISSION;

typedef std::vector< SMISSION_DEFENCE7_MISSION > CONT_MISSION_DEFENCE7_MISSION;

typedef struct tagMISSION_DEFENCE7_MISSION_BAG
{
	tagMISSION_DEFENCE7_MISSION_BAG(){}
	CONT_MISSION_DEFENCE7_MISSION	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE7_MISSION_BAG;

typedef struct tagMISSION_DEFENCE7_STAGE_KEY
{
	tagMISSION_DEFENCE7_STAGE_KEY():
	iMissionNo(0),
	iMissionType(0),
	iParty_Number(0),
	iStageNo(0){}
	explicit tagMISSION_DEFENCE7_STAGE_KEY(int _MissionNo, int _MissionType, int _Party_Number, int _StageNo):iMissionNo(_MissionNo), iMissionType(_MissionType), iParty_Number(_Party_Number), iStageNo(_StageNo){}
	bool operator < (tagMISSION_DEFENCE7_STAGE_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		if( iMissionType < rhs.iMissionType ) {return true;}
		if( iMissionType > rhs.iMissionType ) {return false;}

		if( iParty_Number < rhs.iParty_Number )	{return true;}
		if( iParty_Number > rhs.iParty_Number )	{return false;}

		if( iStageNo < rhs.iStageNo )	{return true;}
		if( iStageNo > rhs.iStageNo )	{return false;}

		return false;
	}

	void Set(int _MissionNo, int _MissionType, int _Party_Number, int _StageNo)
	{
		iMissionNo = _MissionNo;
		iMissionType = _MissionType;
		iParty_Number = _Party_Number;
		iStageNo = _StageNo;
	}

	int iMissionNo;
	int iMissionType;
	int iParty_Number;
	int iStageNo;
}SMISSION_DEFENCE7_STAGE_KEY;

typedef struct tagMISSION_DEFENCE7_STAGE
{
	tagMISSION_DEFENCE7_STAGE():	
	iResult_ItemNo(0),
	iResult_ItemCount(0),
	iWin_ItemNo(0),
	iWin_ItemCount(0),
	iClear_StategicPoint(0),
	iResult_No(0),
	iResult_Count(0),
	iUseCoin_StrategicPoint(0)
	{}
	
	int iResult_ItemNo;
	int iResult_ItemCount;
	int iWin_ItemNo;
	int iWin_ItemCount;
	int iClear_StategicPoint;
	int iResult_No;
	int iResult_Count;
	int iUseCoin_StrategicPoint;
}SMISSION_DEFENCE7_STAGE;

typedef std::vector< SMISSION_DEFENCE7_STAGE > CONT_MISSION_DEFENCE7_STAGE;

typedef struct tagMISSION_DEFENCE7_STAGE_BAG
{
	tagMISSION_DEFENCE7_STAGE_BAG(){}
	CONT_MISSION_DEFENCE7_STAGE	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE7_STAGE_BAG;

typedef struct tagMISSION_DEFENCE7_WAVE_KEY
{
	tagMISSION_DEFENCE7_WAVE_KEY():
	iMissionNo(0),
	iMissionType(0),
	iParty_Number(0),
	iStageNo(0),
	iWave_No(0){}
	explicit tagMISSION_DEFENCE7_WAVE_KEY(int _MissionNo, int _MissionType, int _Party_Number, int _StageNo, int _Wave_No):iMissionNo(_MissionNo), iMissionType(_MissionType), iParty_Number(_Party_Number), iStageNo(_StageNo), iWave_No(_Wave_No){}
	bool operator < (tagMISSION_DEFENCE7_WAVE_KEY const & rhs) const
	{
		if( iMissionNo < rhs.iMissionNo )	{return true;}
		if( iMissionNo > rhs.iMissionNo )	{return false;}

		if( iMissionType < rhs.iMissionType )	{return true;}
		if( iMissionType > rhs.iMissionType )	{return false;}

		if( iParty_Number < rhs.iParty_Number )	{return true;}
		if( iParty_Number > rhs.iParty_Number )	{return false;}

		if( iStageNo < rhs.iStageNo )	{return true;}
		if( iStageNo > rhs.iStageNo )	{return false;}

		if( iWave_No < rhs.iWave_No )	{return true;}
		if( iWave_No > rhs.iWave_No )	{return false;}

		return false;
	}

	int iMissionNo;
	int iMissionType;
	int iParty_Number;
	int iStageNo;
	int iWave_No;
}SMISSION_DEFENCE7_WAVE_KEY;

int const MAX_DEFENCE7_WAVE_MONSTER = 10;

typedef struct tagMISSION_DEFENCE7_WAVE
{
	tagMISSION_DEFENCE7_WAVE():iWave_Delay(0), iGive_StrategicPoint(0), iAddMoveSpeedPercent(0), iTunningNo(0), iAddExpPercent(0), iAddHPPercent(0), DamageDecRate(0)
	{
		::memset(iMonster,0,sizeof(iMonster));
	}
	int iWave_Delay;
	int iGive_StrategicPoint;
	int iMonster[MAX_DEFENCE7_WAVE_MONSTER];

	int iTunningNo;
	int iAddMoveSpeedPercent;	
	int iAddExpPercent;
	int iAddHPPercent;
	int DamageDecRate;
}SMISSION_DEFENCE7_WAVE;

typedef std::vector< SMISSION_DEFENCE7_WAVE > CONT_MISSION_DEFENCE7_WAVE;

typedef struct tagMISSION_DEFENCE7_WAVE_BAG
{
	tagMISSION_DEFENCE7_WAVE_BAG(){}
	CONT_MISSION_DEFENCE7_WAVE	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE7_WAVE_BAG;

int const MAX_DEFENCE7_GUARDIAN_UPGRADE = 2;

typedef struct tagMISSION_DEFENCE7_GUARDIAN
{
	tagMISSION_DEFENCE7_GUARDIAN():iMonsterNo(0), iGuardian_Abil01(0), iGuardian_Abil02(0), iGuardian_TunningNo(0), iNeed_StrategicPoint(0), iUpgrade_Type(0), iUpgrade_Step(0), Skill01_No(0), Skill01_StrategicPoint(0), Skill02_No(0), Skill02_StrategicPoint(0), Sell_StrategicPoint(0), iAutoSell_StrategicPoint(0)
	{
		::memset(iUpgrade,0,sizeof(iUpgrade));
	}

	int iMonsterNo;
	int iGuardian_Abil01;
	int iGuardian_Abil02;
	int iGuardian_TunningNo;
	int iNeed_StrategicPoint;
	int iUpgrade_Type;
	int iUpgrade_Step;
	int iUpgrade[MAX_DEFENCE7_GUARDIAN_UPGRADE];
	int Skill01_No;
	int Skill01_StrategicPoint;
	int Skill02_No;
	int Skill02_StrategicPoint;
	int Sell_StrategicPoint;
	int GuardianType;
	int iAutoSell_StrategicPoint;
}SMISSION_DEFENCE7_GUARDIAN;

typedef std::vector< SMISSION_DEFENCE7_GUARDIAN > CONT_MISSION_DEFENCE7_GUARDIAN;

typedef struct tagMISSION_DEFENCE7_GUARDIAN_BAG
{
	tagMISSION_DEFENCE7_GUARDIAN_BAG(){}
	CONT_MISSION_DEFENCE7_GUARDIAN	kCont;

	void Clear()
	{
		kCont.clear();
	}	

	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		Clear();
		PU::TLoadArray_A(kPacket,kCont);
	}
}SMISSION_DEFENCE7_GUARDIAN_BAG;

typedef struct tagMISSION_BONUSMAP_BAG
{
	tagMISSION_BONUSMAP_BAG()
	{
		iUse = 0;
		iTimeLimit = 0;
		iChanceRate = 0;
		iBonus1stMapRate = 0;
		iBonusMap1 = 0;
		iBonusMap2 = 0;
	}

	int iUse;
	int iTimeLimit;
	int iChanceRate;
	int iBonus1stMapRate;
	int iBonusMap1;
	int iBonusMap2;

	DEFAULT_TBL_PACKET_FUNC();
}SMISSION_BONUSMAP_BAG;

typedef struct tagEVENT_ITEM_REWARD_ELEMENT
{
	tagEVENT_ITEM_REWARD_ELEMENT():
	iItemNo(0),
	siCount(0),
	bTimeType(0),
	siUseTime(0),
	bRarity(0),
	bEnchantType(0),
	bEnchantLv(0),
	iRate(0),
	bBroadcast(false){}

	int		iItemNo;
	short	siCount;
	BYTE	bTimeType;
	short	siUseTime;
	BYTE	bRarity;
	BYTE	bEnchantType;
	BYTE	bEnchantLv;
	int		iRate;
	bool	bBroadcast;
}SEVENT_ITEM_REWARD_ELEMENT;

typedef std::vector<SEVENT_ITEM_REWARD_ELEMENT> CONT_EVENT_ITEM_REWARD_ELEMENT;

typedef struct tagTBL_EVENT_ITEM_REWARD
{
	tagTBL_EVENT_ITEM_REWARD():iTotalRate(0){}

	int iTotalRate;
	CONT_EVENT_ITEM_REWARD_ELEMENT	kContAll;
	CONT_EVENT_ITEM_REWARD_ELEMENT	kContRandom;

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t) + sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iTotalRate);
		PU::TWriteArray_A(kPacket,kContAll);
		PU::TWriteArray_A(kPacket,kContRandom);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iTotalRate);
		PU::TLoadArray_A(kPacket,kContAll);
		PU::TLoadArray_A(kPacket,kContRandom);
	}
}TBL_EVENT_ITEM_REWARD;


typedef struct tagTREASURE_CHEST_KEY_ELEMENT
{
	tagTREASURE_CHEST_KEY_ELEMENT() : iTotalRate(0), kContElem() {}
	int iTotalRate;
	CONT_EVENT_ITEM_REWARD_ELEMENT kContElem;

	size_t min_size() const
	{
		return sizeof(int) + sizeof(size_t);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iTotalRate);
		PU::TWriteArray_A(kPacket,kContElem);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iTotalRate);
		PU::TLoadArray_A(kPacket,kContElem);
	}
}STREASURE_CHEST_KEY_ELEMENT;
typedef std::map<int, STREASURE_CHEST_KEY_ELEMENT> CONT_TREASURE_CHEST_KEY_GROUP;

typedef struct tagTBL_TREASURE_CHEST
{
	CONT_TREASURE_CHEST_KEY_GROUP kCont;

	size_t min_size() const
	{
		return sizeof(size_t);
	}
	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteTable_AM<CONT_TREASURE_CHEST_KEY_GROUP>(kPacket, kCont);
	}
	void ReadFromPacket(BM::Stream & kPacket)
	{
		PU::TLoadTable_AM<CONT_TREASURE_CHEST_KEY_GROUP>(kPacket, kCont);
	}

	const STREASURE_CHEST_KEY_ELEMENT* FindKeyElem(int iChestIdx, int iKeyIdx) const
	{
		CONT_TREASURE_CHEST_KEY_GROUP::const_iterator iterChest = kCont.find(iChestIdx);
		if(iterChest == kCont.end()) return NULL;
		return &iterChest->second;
	}

}TBL_TREASURE_CHEST;

typedef struct tagTBL_DEFCHARCARDEFFECT : public std::vector<int>
{
	size_t min_size() const
	{
		return sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket,*this);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		clear();
		PU::TLoadArray_A(kPacket,*this);
	}
}TBL_DEFCHARCARDEFFECT;

typedef struct tagTBL_DEFSIDEJOBRATE
{
	int iCostRate;	// ��� ������
	int iSoulRate;	// �ҿ� ������
	int	iSuccessRate;// ���� Ȯ�� ����
	int iPayRate;	// �Ǹ� ������
}TBL_DEFSIDEJOBRATE;

typedef struct tagHiddenRewardItem
{
	int iMinLevel;
	int iMaxLevel;
	int iItemNo;

	bool CheckLevel(int const iLevel) const
	{
		return (iMinLevel <= iLevel) && (iLevel <= iMaxLevel);
	}

	DEFAULT_TBL_PACKET_FUNC();
} SHiddenRewardItem;

typedef struct tagMYHOMESIDEJOBTIME
{
	tagMYHOMESIDEJOBTIME():iTime(0){}
	tagMYHOMESIDEJOBTIME(int const & __time):iTime(__time){}
	operator int()const{return iTime;}
	int iTime;
}SMYHOMESIDEJOBTIME;

typedef struct tagMYHOMEBUILDING
{
	int iBuildingNo;
	int iGroundNo;
	int iGrade;
	int iMaxRoom;
	tagMYHOMEBUILDING():iBuildingNo(0),iGroundNo(0),iGrade(0),iMaxRoom(0){}
}SMYHOMEBUILDING;

typedef std::vector<SMYHOMEBUILDING> CONT_SET_BUILDINGS;
typedef struct tagGROUNDBUILDINGS
{
	CONT_SET_BUILDINGS kCont;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket,kCont);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kCont.clear();
		PU::TLoadArray_A(kPacket,kCont);
	}

	size_t min_size() const {return sizeof(size_t);}

}SGROUNDBUILDINGS;


typedef struct tagDefBasicOptionAmp
{
	int iEquipPos;
	int iLevelLimit;
	int iAmpLv;
	int iAmpMaxLv;
	int iAmpRate;
	int iSuccessRate;
	int iNeedCost;
	int iNeedSoulCount;
	int iAmpItemNo;
	int iAmpItemCount;
	int iInsuranceItemNo;

	tagDefBasicOptionAmp():iEquipPos(0),iLevelLimit(0),iAmpLv(0),iAmpMaxLv(0),iAmpRate(0),iSuccessRate(0),
		iNeedCost(0),iNeedSoulCount(0),iAmpItemNo(0),iAmpItemCount(0),iInsuranceItemNo(0){}
}SDefBasicOptionAmp;

typedef std::map<WORD, int> CONT_AMP_SPECIFIC;		//key: Abil_Type, value: AmplifyRate_Specific
typedef struct tagDefItemAmplify_Specific
{
	int iEquipType;
	CONT_AMP_SPECIFIC kContSpecific;

	int GetRate(WORD const wType)const
	{
		CONT_AMP_SPECIFIC::const_iterator c_it = kContSpecific.find(wType);
		if(c_it!=kContSpecific.end())
		{
			return (*c_it).second;
		}
		
		c_it = kContSpecific.find(0);
		if(c_it!=kContSpecific.end())
		{
			return (*c_it).second;
		}

		return 0;
	}

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iEquipType);
		kPacket.Push(kContSpecific);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iEquipType);
		kPacket.Pop(kContSpecific);
	}

	size_t min_size() const
	{
		return sizeof(int)+sizeof(size_t);
	}

	tagDefItemAmplify_Specific(int const _iEquipType=0):iEquipType(_iEquipType){}
}SDefItemAmplify_Specific;

typedef struct tagTBL_DEFDEATHPENALTY
{
	static const int MAX_DEATH_PENALTY_EFFECTSIZE = 2;

	tagTBL_DEFDEATHPENALTY() {};
	tagTBL_DEFDEATHPENALTY(tagTBL_DEFDEATHPENALTY const& rhs)
	{
		(*this) = rhs;
	}
	tagTBL_DEFDEATHPENALTY const& operator=(tagTBL_DEFDEATHPENALTY const& rhs)
	{
		iGroundAttr = rhs.iGroundAttr;
		iGroundNo = rhs.iGroundNo;
		sLevelMin = rhs.sLevelMin;
		sLevelMax = rhs.sLevelMax;
		bySuccessionalDeath = rhs.bySuccessionalDeath;
		memcpy(iEffect, rhs.iEffect, sizeof(int)*MAX_DEATH_PENALTY_EFFECTSIZE);
		return (*this);
	}

	int iGroundAttr;
	int iGroundNo;
	short sLevelMin;
	short sLevelMax;
	BYTE bySuccessionalDeath;
	int iEffect[MAX_DEATH_PENALTY_EFFECTSIZE];
} TBL_DEFDEATHPENALTY;

int const MAX_SKILLEXTENDITEM_NUM = 10;

typedef std::set<int> CONT_EXTEND_SKILLSET;

typedef struct tagTBL_DEFSKILLEXTENDITEM
{
	tagTBL_DEFSKILLEXTENDITEM(){}
	CONT_EXTEND_SKILLSET kCont;
	void WriteToPacket(BM::Stream & kPacket) const
	{
		PU::TWriteArray_A(kPacket, kCont);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		CONT_EXTEND_SKILLSET().swap(kCont);
		PU::TLoadArray_A(kPacket, kCont);
	}

	size_t min_size() const
	{
		return sizeof(size_t);
	}
}STBL_DEFSKILLEXTENDITEM;

typedef struct tagPvPLeague_Time
{
	tagPvPLeague_Time(void)
		:	iDayOfWeek(0)
		,	iLeagueState(0)
		,	iLeagueLevel(0)
		,	i64BeginTime(0i64)
		,	i64EndTime(0i64)
	{}

	int iDayOfWeek;
	int iLeagueState;
	int iLeagueLevel;
	__int64 i64BeginTime;
	__int64 i64EndTime;

	bool operator < ( tagPvPLeague_Time const & rhs )const{return (iLeagueLevel < rhs.iLeagueLevel);}
	bool operator > ( tagPvPLeague_Time const & rhs )const{return (rhs < *this);}
	bool operator <= ( tagPvPLeague_Time const & rhs )const{return ((*this < rhs) || (*this == rhs));}
	bool operator >= ( tagPvPLeague_Time const & rhs )const{return (rhs <= *this);}
	bool operator == ( tagPvPLeague_Time const & rhs )const{return ((iLeagueLevel == rhs.iLeagueLevel) && iDayOfWeek == rhs.iDayOfWeek);}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PVPLEAGUE_TIME;

typedef struct tagPvPLeague_Session
{
	tagPvPLeague_Session(void)
		:	iGameType(0)
		,	iGameMode(0)
		,	iGameTime(0)
	{}

	int iGameType;
	int iGameMode;
	int iGameTime;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PVPLEAGUE_SESSION;

typedef struct tagTBL_DEF_SPECIFIC_REWARD
{
	tagTBL_DEF_SPECIFIC_REWARD()
	{
		iType = 0;
		iSubType = 0;
		iRewardItemNo = 0;
		i64RewardCount = 0i64;
	}

	int iType;
	int iSubType;
	int iRewardItemNo;
	__int64 i64RewardCount;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_SPECIFIC_REWARD;

typedef struct tagTBL_DEF_SPECIFIC_REWARD_EVENT
{
	tagTBL_DEF_SPECIFIC_REWARD_EVENT()
	{
		EventNo = 0;
		RewardItemGroupNo = 0;
		LevelLimit = 0;
		DraLevelLimit = 0;
		RewardCount = 0;
		RewardPeriod = 0;
		StartTime.Clear();
		EndTime.Clear();
	}

	int EventNo;
	int RewardItemGroupNo;
	BM::DBTIMESTAMP_EX StartTime;
	BM::DBTIMESTAMP_EX EndTime;
	short LevelLimit;
	short DraLevelLimit;
	short RewardCount;
	short RewardPeriod;
}TBL_DEF_SPECIFIC_REWARD_EVENT;

typedef struct tagPvPLeague_Reward
{
	tagPvPLeague_Reward(void)
		:	iItemNo(0)
		,	iItemCount(0)
	{}

	int iItemNo;
	int iItemCount;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PVPLEAGUE_REWARD;

int const COUNT_JOBSKILL_RESULT_PROBABILITY_NO = 10;
typedef struct tagTBL_DEF_JOBSKILL_LOCATIONITEM
{
	tagTBL_DEF_JOBSKILL_LOCATIONITEM()
		:	iGatherType(0)
		,   iSpot_Probability(0)
		,	iSpot_TotalProbability(0)
		,	iSpot_Cycle_Min(0)
		,	iSpot_Cycle_Max(0)
		,	iBase_Expertness(0)
		,	i01Need_SaveIdx(0)
		,	i02Need_SaveIdx(0)
		,	i03Need_SaveIdx(0)
		,	i01Need_Skill_No(0)
		,	i01Need_Expertness(0)
		,	i02Need_Skill_No(0)
		,	i02Need_Expertness(0)
		,	i03Need_Skill_No(0)
		,	i03Need_Expertness(0)
		,	iProbability_UpRate(0)
	{
		::memset(iResultProbability_No, 0, sizeof(iResultProbability_No) );
	}

	int iGatherType;
	int iSpot_Probability;
	int iSpot_TotalProbability;
	int iSpot_Cycle_Min;
	int iSpot_Cycle_Max;
	int iSpot_Dration_Min;
	int iSpot_Dration_Max;
	int iResultProbability_No[COUNT_JOBSKILL_RESULT_PROBABILITY_NO];
	int iBase_Expertness;
	int i01Need_SaveIdx;
	int i02Need_SaveIdx;
	int i03Need_SaveIdx;
	int i01Need_Skill_No;
	int i01Need_Expertness;
	int i02Need_Skill_No;
	int i02Need_Expertness;
	int i03Need_Skill_No;
	int i03Need_Expertness;
	int iProbability_UpRate;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JOBSKILL_LOCATIONITEM;

typedef enum eJobSkillType
{
	JST_NONE		= 0,
	JST_1ST_MAIN	= 1, // ������ų(ä��) 1��, �� ��ų
	JST_1ST_SUB		= 2, // ������ų(ä��) 1��, ���� ��ų
	JST_2ND_MAIN	= 11, // ������ų(ä��) 2��, �� ��ų
	JST_2ND_SUB  	= 12, // ������ų(ä��) 2��, ���� ��ų
	// �ʿ��� ����� �߰�
	JST_3RD_MAIN	= 21, // ������ų(ä��) 3��, �� ��ų
} EJobSkillType;
typedef struct tagTBL_DEF_JOBSKILL_SKILL
{
	tagTBL_DEF_JOBSKILL_SKILL()
		:	iGatherType(0)
		,	eJobSkill_Type(JST_NONE)
		,	i01NeedParent_JobSkill_No(0)
		,	i01NeedParent_JobSkill_Expertness(0)
		,	i02NeedParent_JobSkill_No(0)
		,	i02NeedParent_JobSkill_Expertness(0)
		,	i03NeedParent_JobSkill_No(0)
		,	i03NeedParent_JobSkill_Expertness(0)
		
	{}

	int iGatherType;
	EJobSkillType eJobSkill_Type;
	int i01NeedParent_JobSkill_No;
	int i01NeedParent_JobSkill_Expertness;
	int i02NeedParent_JobSkill_No;
	int i02NeedParent_JobSkill_Expertness;
	int i03NeedParent_JobSkill_No;
	int i03NeedParent_JobSkill_Expertness;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JOBSKILL_SKILL;

typedef struct tagTBL_DEF_JOBSKILL_SKILLEXPERTNESS
{
	tagTBL_DEF_JOBSKILL_SKILLEXPERTNESS()
		:	iSkill_Expertness_Min(0)
		,	iSkill_Expertness_Max(0)
		,	iExpertness_Gain_Min(0)
		,	iExpertness_Gain_Max(0)
		,	iExpertness_Gain_Probability(0)
		,	iExpertness_Gain_TotalProbability(0)
		,	iUse_Duration(0)
		,	iUse_Duration_Probability(0)
		,	iUse_Duration_TotalProbability(0)
		,	iBasic_Turn_Time(0)
		,	iMax_Exhaustion(0)
		,	iUse_Exhaustion(0)
		,	iMax_Machine(0)
		,	iNeed_CharLevel(0)
		,	iSkill_Probability(0)
		
	{}

	bool operator <(tagTBL_DEF_JOBSKILL_SKILLEXPERTNESS const& rhs) const
	{
		if( iSkillNo == rhs.iSkillNo )
		{
			return iSkill_Expertness_Min < rhs.iSkill_Expertness_Min;
		}
		return iSkillNo < rhs.iSkillNo;
	}
	bool operator ==(tagTBL_DEF_JOBSKILL_SKILLEXPERTNESS const& rhs) const
	{
		return	iSkillNo == rhs.iSkillNo
			&&	iSkill_Expertness_Min == rhs.iSkill_Expertness_Min
			&&	iSkill_Expertness_Max == rhs.iSkill_Expertness_Max;
	}

	int iSkillNo;
	int iSkill_Expertness_Min;
	int iSkill_Expertness_Max;
	int iExpertness_Gain_Min;
	int iExpertness_Gain_Max;
	int iExpertness_Gain_Probability;
	int iExpertness_Gain_TotalProbability;
	int iUse_Duration;
	int iUse_Duration_Probability;
	int iUse_Duration_TotalProbability;
	int iBasic_Turn_Time;
	int iMax_Exhaustion;
	int iUse_Exhaustion;
	int iMax_Machine;
	int iNeed_CharLevel;
	int iSkill_Probability;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JOBSKILL_SKILLEXPERTNESS;

typedef struct tagTBL_DEF_JOBSKILL_TOOL
{
	tagTBL_DEF_JOBSKILL_TOOL()
		:	iToolType(0)
		,	iGatherType(0)
		,	i01Need_Skill_No(0)
		,	i01Need_Skill_Expertness(0)
		,	i02Need_Skill_No(0)
		,	i02Need_Skill_Expertness(0)
		,	i03Need_Skill_No(0)
		,	i03Need_Skill_Expertness(0)
		,	iGetCount(0)
		,	iGetCountMax(0)
		,	iOption_TurnTime(0)
		,	iOption_CoolTime(0)
		,	iOption_User_Exhaustion(0)
		,	iResult_No_Min(0)
		,	iResult_No_Max(0)
		
	{}

	int iToolType;
	int iGatherType;
	int i01Need_Skill_No;
	int i01Need_Skill_Expertness;
	int i02Need_Skill_No;
	int i02Need_Skill_Expertness;
	int i03Need_Skill_No;
	int i03Need_Skill_Expertness;
	int iGetCount;
	int iGetCountMax;
	int iOption_TurnTime;
	int iOption_CoolTime;
	int iOption_User_Exhaustion;
	int iResult_No_Min;
	int iResult_No_Max;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JOBSKILL_TOOL;

//
typedef struct tagProbability
{
	tagProbability();
	tagProbability(int const& riResultNo, int const& riCount, int const& riProbability);
	tagProbability(tagProbability const& rhs);
	~tagProbability();

	bool IsFail() const;
	bool IsRight() const;
	bool IsWrong() const;
	bool operator <(tagProbability const& rhs) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket) const;
	size_t min_size() const;

	int iResultNo;	// ��� ��ȣ(������, Ȯ�� ��ȣ)
	int iCount;		// �������� �� ����
	int iProbability;	// �߻��� Ȯ��
} SProbability;
typedef std::list< SProbability > CONT_PROBABILITY;

//
typedef enum eProbablityType
{
	PT_ITEM			= 1, // ����� ������
	PT_PROBABILITY	= 2, // ����� TB_DefJobSkill_Probability ��ȣ
} EProbablityType;

//
typedef struct tagProbabilityBag
{
	tagProbabilityBag();
	//tagProbabilityBag(int const& riNo, EProbablityType const& reType, int const& riTotalProbability, CONT_PROBABILITY const& rkContProbability);
	tagProbabilityBag(tagProbabilityBag const& rhs);
	~tagProbabilityBag();

	bool Add(SProbability const& rkNew);
	SProbability Get(int const iAddValue = 0) const;
	void ReadFromPacket(BM::Stream& rkPacket);
	void WriteToPacket(BM::Stream& rkPacket) const;
	size_t min_size() const;

	int iNo;
	EProbablityType eType;
	int iTotalProbability; // �Է°�
	CONT_PROBABILITY kContProbability;
	int iTotalSumProbability; // kContProbability�� Probability�� ���Ѱ�
	int i01NeedSaveIdx;
} SProbabilityBag;

//
typedef struct tagJobSkillItemUpgrade
{
	tagJobSkillItemUpgrade()
	{
		Clear();
	}
	int iItemNo;
	int iGrade;
	int iResourceGroupNo;
	int iUpgradeCount;
	int	iUpgradeTime; // ����(��)
	int iBasicExpertness;
	int iGatherType;
	int iMachine_UseDuration;
	int iResultProbabilityNo;
	int iResourceProbabilityUp;
	int iErrorStateTimeAbsolute;
	int iErrorStateMaxProbability;
	int iErrorStateProbability;
	int iNeedSaveIdx;
	int iResourceType;
	int iExpertnessUpVolume;

	tagJobSkillItemUpgrade& operator=( const tagJobSkillItemUpgrade& rhs )
	{
		iItemNo = rhs.iItemNo;
		iGrade = rhs.iGrade;
		iResourceGroupNo = rhs.iResourceGroupNo;
		iUpgradeCount = rhs.iUpgradeCount;
		iUpgradeTime = rhs.iUpgradeTime;
		iBasicExpertness = rhs.iBasicExpertness;
		iGatherType = rhs.iGatherType;
		iMachine_UseDuration = rhs.iMachine_UseDuration;
		iResultProbabilityNo = rhs.iResultProbabilityNo;
		iResourceProbabilityUp = rhs.iResourceProbabilityUp;
		iErrorStateTimeAbsolute = rhs.iErrorStateTimeAbsolute;
		iErrorStateMaxProbability = rhs.iErrorStateMaxProbability;
		iErrorStateProbability = rhs.iErrorStateProbability;
		iNeedSaveIdx = rhs.iNeedSaveIdx;
		iResourceType = rhs.iResourceType;
		iExpertnessUpVolume = rhs.iExpertnessUpVolume;
		return *this;
	}

	void Clear()
	{
		iItemNo = 0;
		iGrade = 0;
		iResourceGroupNo = 0;
		iUpgradeCount = 0;
		iUpgradeTime = 0;
		iBasicExpertness = 0;
		iGatherType = 0;
		iMachine_UseDuration = 0;
		iResultProbabilityNo = 0;
		iResourceProbabilityUp =0;
		iErrorStateTimeAbsolute = 0;
		iErrorStateMaxProbability = 0;
		iErrorStateProbability = 0;
		iNeedSaveIdx = 0;
		iResourceType = 0;
		iExpertnessUpVolume = 0;
	}

	DEFAULT_TBL_PACKET_FUNC();
} SJobSkillItemUpgrade;

//
typedef struct tagJobSkillSaveIdx
{
	int iSaveIdx;
	int iRecipeItemNo;
	int iResourceGroupNo;
	int iNeedSkillNo01;
	int iNeedSkillExpertness01;
	int iNeedSkillNo02;
	int iNeedSkillExpertness02;
	int iNeedSkillNo03;
	int iNeedSkillExpertness03;
	int iRecipePrice;
	int iBookItemNo;
	int iAlreadyLearn;
	int iShowUser;

	DEFAULT_TBL_PACKET_FUNC();
} SJobSkillSaveIdx;

//
int const MAX_JS3_NEEDJOBSKILL = 3;
int const MAX_JS3_RECIPE_RES = 4;
typedef struct tagJobSkillResource
{
	tagJobSkillResource():iGroupNo(0),iGrade(0),iCount(0){}
	void Clear()
	{
		iGroupNo = 0;
		iGrade = 0;
		iCount = 0;
	}
	int iGroupNo;		// ��� ������ ��ȣ
	int iGrade;			// �ּ� ���(�̻�)
	int iCount;
} SJobSkillRes;
typedef struct tagJobSkillProbability
{
	tagJobSkillProbability():iNo(0),iRate(0),iRateItem(0){}
	void Clear()
	{
		iNo = 0;
		iRate = 0;
		iRateItem = 0;
	}
	int iNo;			// ���� ���
	int iRate;			// ���õ��� ���� Ȯ����������
	int iRateItem;		// ��ῡ ���� Ȯ����������
} SJobSkillProb;

typedef struct tagJobSkillRecipe
{
	tagJobSkillRecipe():iItemNo(0),iOrderID(0),iNeedSkillNo(0),iNeedSkillExpertness(0),iNeedMoney(0),iNeedProductPoint(0),iExpertnessGain(0){}
	void Clear()
	{
		iItemNo = 0;
		iOrderID = 0;
		iNeedSkillNo = 0;
		iNeedSkillExpertness = 0;
		for(int i=0; i<MAX_JS3_RECIPE_RES; ++i)
		{
			kResource[i].Clear();
		}
		iNeedMoney = 0;
		kProbability.Clear();
		iNeedProductPoint = 0;
		iExpertnessGain = 0;
	}
	int iItemNo;
	int iOrderID;
	int iNeedSkillNo;				// �������
	int iNeedSkillExpertness;		// ������� ���õ�
	SJobSkillRes kResource[MAX_JS3_RECIPE_RES];
	int iNeedMoney;
	SJobSkillProb kProbability;
	int iNeedProductPoint;
	int iExpertnessGain;
	DEFAULT_TBL_PACKET_FUNC();
} SJobSkillRecipe;

int const MAX_PCCAFEABIL_COUNT = 4;
typedef struct tagPCCafeAbil
{
	tagPCCafeAbil()
		: byGrade(0)
	{
		ZeroMemory( arAbilNo, sizeof( arAbilNo ) );
	}

	BYTE byGrade;
	int arAbilNo[MAX_PCCAFEABIL_COUNT];

	DEFAULT_TBL_PACKET_FUNC();

} SPCCafeAbil;

typedef struct tagJobSkillSoulExtract
{
	tagJobSkillSoulExtract()
	:	iEquipPos(0), iLevelLimit(0), iSuccessRate(0), iExtractItemNo(0), iResultItemNo(0)
	{}
	void Clear()
	{
		iEquipPos = 0;;
		iLevelLimit = 0;
		iSuccessRate = 0;;
		iExtractItemNo = 0;
		iResultItemNo = 0;
	}
	bool IsNull()
	{
		return (0 == iEquipPos
			&&  0 == iLevelLimit
			&&  0 == iSuccessRate
			&&  0 == iExtractItemNo
			&&  0 == iResultItemNo );
	}
	bool operator <(tagJobSkillSoulExtract const& rhs) const
	{
		return iLevelLimit < rhs.iLevelLimit;
	}
	bool operator ==(tagJobSkillSoulExtract const& rhs) const
	{
		return	iEquipPos == rhs.iEquipPos
			&&	iLevelLimit == rhs.iLevelLimit
			&&	iExtractItemNo == rhs.iExtractItemNo
			&&	iResultItemNo == rhs.iResultItemNo;
	}
	int iEquipPos;
	int iLevelLimit;
	int iSuccessRate;
	int iExtractItemNo;
	int iResultItemNo;
	DEFAULT_TBL_PACKET_FUNC();
} SJobSkillSoulExtract;

typedef struct tagJobSkillSoulTransition
{
	tagJobSkillSoulTransition()
	:	iEquipType(0), iLevelLimit(0), iRairity_Grade(0), iSuccessRate(0), iSoulItemCount(0), iNeedMoney(0), 
	iInsuranceitemNo(0), iProbabilityUpItemNo(0), iProbabilityUpRate(0), iProbabilityUpItemCount(0)
	{}
	void Clear()
	{
		iEquipType = 0;
		iLevelLimit = 0;
		iRairity_Grade = 0;
		iSuccessRate = 0;
		iSoulItemCount = 0;
		iNeedMoney = 0;
		iInsuranceitemNo = 0;
		iProbabilityUpItemNo = 0;
		iProbabilityUpRate = 0;
		iProbabilityUpItemCount = 0;
	}
	bool IsNull()
	{
		return (0 == iEquipType
			&&  0 == iLevelLimit
			&&  0 == iRairity_Grade
			&&  0 == iSuccessRate );
	}
	bool operator <(tagJobSkillSoulTransition const& rhs) const
	{
		return iLevelLimit < rhs.iLevelLimit;
	}
	bool operator ==(tagJobSkillSoulTransition const& rhs) const
	{
		return	iEquipType == rhs.iEquipType
			&&	iLevelLimit == rhs.iLevelLimit
			&&	iRairity_Grade == rhs.iRairity_Grade;
	}
	int iEquipType;
	int iLevelLimit;
	int iRairity_Grade;
	int iSuccessRate;
	int iSoulItemCount;
	int iNeedMoney;
	int iInsuranceitemNo;
	int iProbabilityUpItemNo;
	int iProbabilityUpRate;
	int iProbabilityUpItemCount;
	DEFAULT_TBL_PACKET_FUNC();
} SJobSkillSoulTransition;

typedef struct tagSocketItem_Key
{
	tagSocketItem_Key():
iEquipPos(0),
iLevelLimit(0),
iSocket_Order(0){}
explicit tagSocketItem_Key(int _EquipPos, int _LevelLimit, int _Socket_Order):iEquipPos(_EquipPos), iLevelLimit(_LevelLimit), iSocket_Order(_Socket_Order){}
bool operator < (tagSocketItem_Key const & rhs) const
{
	if( iEquipPos < rhs.iEquipPos )	{return true;}
	if( iEquipPos > rhs.iEquipPos )	{return false;}

	if( iLevelLimit < rhs.iLevelLimit )	{return true;}
	if( iLevelLimit > rhs.iLevelLimit )	{return false;}

	if( iSocket_Order < rhs.iSocket_Order )	{return true;}
	if( iSocket_Order > rhs.iSocket_Order )	{return false;}

	return false;
}

void Set(int _EquipPos, int _LevelLimit, int _Socket_Order)
{
	iEquipPos = _EquipPos;
	iLevelLimit = _LevelLimit;
	iSocket_Order = _Socket_Order;
}

int iEquipPos;
int iLevelLimit;
int iSocket_Order;
}SocketItem_Key;

typedef struct tagSocketItem_Bag
{
	tagSocketItem_Bag()
	{
		iSuccessRate = 0;
		iSoulItemCount = 0;
		iNeedMoney = 0i64;
		iSocketItemNo = 0;
		iRestorationItemNo = 0;
		iResetItemNo = 0;
	}

	int iSuccessRate;
	int iSoulItemCount;
	__int64 iNeedMoney;
	int iSocketItemNo;
	int iRestorationItemNo;
	int iResetItemNo;

	DEFAULT_TBL_PACKET_FUNC();
}SocketItem_Bag;

typedef struct tagMonsterCard_Key
{
	tagMonsterCard_Key():
iOrderIndex(0),
iCardNo(0){}
explicit tagMonsterCard_Key(int _OrderIndex, int _CardNo):iOrderIndex(_OrderIndex), iCardNo(_CardNo){}
bool operator < (tagMonsterCard_Key const & rhs) const
{
	if( iOrderIndex < rhs.iOrderIndex )	{return true;}
	if( iOrderIndex > rhs.iOrderIndex )	{return false;}

	if( iCardNo < rhs.iCardNo )	{return true;}
	if( iCardNo > rhs.iCardNo )	{return false;}

	return false;
}

int iOrderIndex;
int iCardNo;
}MonsterCard_Key;

typedef struct tagExpeditionNpc
{
	BM::GUID	kNpcGuid;
	int iMapNo;
	int iMinPlayerCnt;
	int iMaxPlayerCnt;
	int iNeedItemNo;
	int iNeedItemCnt;
	int iDungeonStringNo;
	std::wstring kStrImgPath;
	int iDungeonInfoStringNo;

	tagExpeditionNpc():
	 iMapNo(0)
	,iMinPlayerCnt(0)
	,iMaxPlayerCnt(0)
	,iNeedItemNo(0)
	,iNeedItemCnt(0)
	,iDungeonStringNo(0)
	,iDungeonInfoStringNo(0)
	{
	}

	size_t min_size()const
	{
		return 
			sizeof(kNpcGuid)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(size_t)+//kImgPath
			sizeof(int);
	}
	
	void WriteToPacket(BM::Stream &kPacket)const 
	{
		kPacket.Push(kNpcGuid);
		kPacket.Push(iMapNo);
		kPacket.Push(iMinPlayerCnt);
		kPacket.Push(iMaxPlayerCnt);
		kPacket.Push(iNeedItemNo);
		kPacket.Push(iNeedItemCnt);
		kPacket.Push(iDungeonStringNo);
		
		kPacket.Push(kStrImgPath);
		kPacket.Push(iDungeonInfoStringNo);
	}

	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(kNpcGuid);
		kPacket.Pop(iMapNo);
		kPacket.Pop(iMinPlayerCnt);
		kPacket.Pop(iMaxPlayerCnt);
		kPacket.Pop(iNeedItemNo);
		kPacket.Pop(iNeedItemCnt);
		kPacket.Pop(iDungeonStringNo);
		
		kPacket.Pop(kStrImgPath);
		kPacket.Pop(iDungeonInfoStringNo);
	}
}SExpeditionNpc;

#define MAX_MUTATORABIL 5
typedef struct tagMissionMutator
{
	int iMutatorNo;
	int iMutatorNameNo;
	int iDescription;
	int iAddedExpRate;
	int iAddedMoneyRate;
	int iMutatorType;
	int iMutatorUnitType;
	int iAbil[5];

	tagMissionMutator()
	{
		iMutatorNo = 0;
		iMutatorNameNo = 0;
		iDescription = 0;
		iAddedExpRate = 0;
		iAddedMoneyRate = 0;
		iMutatorType = 0;
		iMutatorUnitType = 0;
		::memset(iAbil, 0, sizeof(iAbil));
	}

	DEFAULT_TBL_PACKET_FUNC()
}SMissionMutator;

#define MAX_MUTATUR_ABIL_TYPE 5
typedef struct tagMissionMutatorAbil
{
	int iAbilNo;
	int iType[MAX_MUTATUR_ABIL_TYPE];
	int iValue[MAX_MUTATUR_ABIL_TYPE];

	tagMissionMutatorAbil()
	{
		iAbilNo = 0;
		::memset(iType, 0, sizeof(iType));
		::memset(iValue, 0, sizeof(iValue));
	}

	DEFAULT_TBL_PACKET_FUNC()
}SMissionMutatorAbil;

typedef struct tagEventGroup
{
	int EventGroupNo;
	int MonsterGroupNo;
	BM::PgPackedTime StartTime;
	BM::PgPackedTime EndTime;
	int RegenPeriod;
	int DelayGenTime;
	std::wstring Message;

	tagEventGroup() :
	EventGroupNo(0),
	MonsterGroupNo(0),
	RegenPeriod(0),
	DelayGenTime(0)
	{
		StartTime.Clear();
		EndTime.Clear();
		Message.clear();
	}

	size_t min_size() const
	{
		return sizeof(int) +
			sizeof(int) +
			sizeof(StartTime) +
			sizeof(EndTime) +
			sizeof(int) +
			sizeof(int) +
			sizeof(size_t);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(EventGroupNo);
		Packet.Push(MonsterGroupNo);
		Packet.Push(StartTime);
		Packet.Push(EndTime);
		Packet.Push(RegenPeriod);
		Packet.Push(DelayGenTime);
		Packet.Push(Message);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(EventGroupNo);
		Packet.Pop(MonsterGroupNo);
		Packet.Pop(StartTime);
		Packet.Pop(EndTime);
		Packet.Pop(RegenPeriod);
		Packet.Pop(DelayGenTime);
		Packet.Pop(Message);
	}
} SEventGroup;

typedef struct tagEvnetMonsterInfo
{
	int MonsterNo;
	POINT3 GenPos;
	int RewardItemGroup;
	int RewardItemCount;
	int Order;
	int GenDelay;
	int MonsterLevel;
	int DifficultyHP;
	int DifficultyDamage;
	int DifficultyDefence;
	int DifficultyExp;
	int DifficultyAttackSpeed;
	int DifficultyEvade;
	int DifficultyHitrate;

	size_t min_size() const
	{
		return sizeof(int) +
			sizeof(GenPos) +
			sizeof(int) +
			sizeof(int);
	}

	tagEvnetMonsterInfo() :
	MonsterNo(0),
	GenPos(0, 0, 0),
	RewardItemGroup(0),
	RewardItemCount(0),
	Order(0), 
	GenDelay(0),
	MonsterLevel(0),
	DifficultyHP(0),
	DifficultyDamage(0),
	DifficultyDefence(0),
	DifficultyExp(0),
	DifficultyAttackSpeed(0),
	DifficultyEvade(0),
	DifficultyHitrate(0) { }
} SEvnetMonsterInfo;
typedef std::vector<SEvnetMonsterInfo> VEC_EVENTMONSTERINFO;

typedef struct tagEventMonsterGroup
{
	int MonsterGroupNo;
	int MapNo;
	VEC_EVENTMONSTERINFO MonsterInfo;

	tagEventMonsterGroup() :
	MonsterGroupNo(0),
	MapNo(0) { }

	size_t min_size() const
	{
		return sizeof(int) +
			sizeof(int) +
			sizeof(MonsterInfo);
	}

	void operator = (tagEventMonsterGroup & rhs)
	{
		MonsterGroupNo = rhs.MonsterGroupNo;
		MapNo = rhs.MapNo;
		MonsterInfo.resize( rhs.MonsterInfo.size() );
		std::copy( rhs.MonsterInfo.begin(), rhs.MonsterInfo.end(), MonsterInfo.begin() );
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(MonsterGroupNo);
		Packet.Push(MapNo);		
		PU::TWriteArray_A(Packet, MonsterInfo);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(MonsterGroupNo);
		Packet.Pop(MapNo);
		PU::TLoadArray_A(Packet, MonsterInfo);
	}
} SEventMonsterGroup;
typedef std::map<int, SEventMonsterGroup> CONT_EVENTMONSTERGROUP;	// first : MapNo.

typedef struct tagEventRewardItemGroup
{
	int RewardItemGroup;
	int ItemNo;
	int Rate;
	int Count;

	tagEventRewardItemGroup() :
	RewardItemGroup(0),
	ItemNo(0),
	Rate(0),
	Count(0) { }

	size_t min_size() const
	{
		return sizeof(int) +
			sizeof(int) +
			sizeof(int) +
			sizeof(int);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(RewardItemGroup);
		Packet.Push(ItemNo);
		Packet.Push(Rate);
		Packet.Push(Count);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(RewardItemGroup);
		Packet.Pop(ItemNo);
		Packet.Pop(Rate);
		Packet.Pop(Count);
	}
} SEventRewardItemGroup;
typedef std::vector<SEventRewardItemGroup> VEC_EVENTREWARDITEMGROUP;

typedef struct tagEventScheduleData
{
	char PartyMemeberMin;
	char ConsumeReqItem;
	char UnUsableReviveItem;
	char UnUsableConsumeItem;
	char PKOption;
	char EventCount;
	char NeedItemCount;
	char NeedQuestState;
	short UserCountMax;
	short LevelMin;
	short LevelMax;
	int NeedQuest;
	int NeedItemNo;
	int EventNo;
	int EventPeriod;
	int EventGround;
	__int64 ClassLimitHuman;
	__int64 ClassLimitDragon;
	BM::DBTIMESTAMP_EX StartDate;
	BM::DBTIMESTAMP_EX EndDate;
	BM::DBTIMESTAMP_EX StartTime;
	std::wstring MenuText;
	std::wstring EventType;
	std::vector< std::wstring > DayofWeek;

	tagEventScheduleData() :
		PartyMemeberMin(0),
		ConsumeReqItem(0),
		UnUsableReviveItem(0),
		UnUsableConsumeItem(0),
		PKOption(0),
		NeedItemCount(0),
		UserCountMax(0),
		NeedQuestState(0),
		EventCount(0),
		LevelMin(0),
		LevelMax(0),
		NeedQuest(0),
		NeedItemNo(0),
		EventNo(0),
		EventPeriod(0),
		EventGround(0),
		ClassLimitHuman(0i64),
		ClassLimitDragon(0i64)
	{
		StartDate.Clear();
		EndDate.Clear();
		StartTime.Clear();
		MenuText.clear();
		EventType.clear();
		DayofWeek.clear();
	}

	size_t min_size() const
	{
		return sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(char)
			+ sizeof(short)
			+ sizeof(short)
			+ sizeof(short)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(__int64)
			+ sizeof(__int64)
			+ sizeof(BM::DBTIMESTAMP_EX)
			+ sizeof(BM::DBTIMESTAMP_EX)
			+ sizeof(BM::DBTIMESTAMP_EX)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(PartyMemeberMin);
		Packet.Push(ConsumeReqItem);
		Packet.Push(UnUsableReviveItem);
		Packet.Push(UnUsableConsumeItem);
		Packet.Push(PKOption);
		Packet.Push(EventCount);
		Packet.Push(NeedItemCount);
		Packet.Push(UserCountMax);
		Packet.Push(NeedQuestState);
		Packet.Push(LevelMin);
		Packet.Push(LevelMax);
		Packet.Push(NeedQuest);
		Packet.Push(NeedItemNo);
		Packet.Push(EventNo);
		Packet.Push(EventPeriod);
		Packet.Push(EventGround);
		Packet.Push(ClassLimitHuman);
		Packet.Push(ClassLimitDragon);
		Packet.Push(StartDate);
		Packet.Push(EndDate);
		Packet.Push(StartTime);
		Packet.Push(MenuText);
		Packet.Push(EventType);
		PU::TWriteArray_A(Packet, DayofWeek);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(PartyMemeberMin);
		Packet.Pop(ConsumeReqItem);
		Packet.Pop(UnUsableReviveItem);
		Packet.Pop(UnUsableConsumeItem);
		Packet.Pop(PKOption);
		Packet.Pop(EventCount);
		Packet.Pop(NeedItemCount);
		Packet.Pop(UserCountMax);
		Packet.Pop(NeedQuestState);
		Packet.Pop(LevelMin);
		Packet.Pop(LevelMax);
		Packet.Pop(NeedQuest);
		Packet.Pop(NeedItemNo);
		Packet.Pop(EventNo);
		Packet.Pop(EventPeriod);
		Packet.Pop(EventGround);
		Packet.Pop(ClassLimitHuman);
		Packet.Pop(ClassLimitDragon);
		Packet.Pop(StartDate);
		Packet.Pop(EndDate);
		Packet.Pop(StartTime);
		Packet.Pop(MenuText);
		Packet.Pop(EventType);
		PU::TLoadArray_A(Packet, DayofWeek);
	}
} SEventScheduleData;

typedef struct tagEventBossBattleData
{
	char MonsterRegenType;
	int BossBattleEventNo;
	int StartEffect;
	int LimitTime;
	int MonsterGroupNo;
	int HarvestLumberEventItemNo;
	int HarvestGemEventItemNo;
	int HarvestHerbEventItemNo;
	int HarvestFishEventItemNo;
	int HarvestExpMin;

	tagEventBossBattleData() :
		MonsterRegenType(0),
		BossBattleEventNo(0),
		StartEffect(0),
		LimitTime(0),
		MonsterGroupNo(0),
		HarvestLumberEventItemNo(0),
		HarvestGemEventItemNo(0),
		HarvestHerbEventItemNo(0),
		HarvestFishEventItemNo(0),
		HarvestExpMin(0)
		{
		}

	size_t min_size() const
	{
		return sizeof(char)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int)
			+ sizeof(int);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(MonsterRegenType);
		Packet.Push(BossBattleEventNo);
		Packet.Push(StartEffect);
		Packet.Push(LimitTime);
		Packet.Push(MonsterGroupNo);
		Packet.Push(HarvestLumberEventItemNo);
		Packet.Push(HarvestGemEventItemNo);
		Packet.Push(HarvestHerbEventItemNo);
		Packet.Push(HarvestFishEventItemNo);
		Packet.Push(HarvestExpMin);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(MonsterRegenType);
		Packet.Pop(BossBattleEventNo);
		Packet.Pop(StartEffect);
		Packet.Pop(LimitTime);
		Packet.Pop(MonsterGroupNo);
		Packet.Pop(HarvestLumberEventItemNo);
		Packet.Pop(HarvestGemEventItemNo);
		Packet.Pop(HarvestHerbEventItemNo);
		Packet.Pop(HarvestFishEventItemNo);
		Packet.Pop(HarvestExpMin);
	}
} SEventBossBattleData;

typedef struct tagEventRaceData
{
	int EventNo;
	int StartEffect;
	short LimitTime;
	short MoveSpeed;
	bool UseSkill;
	BYTE RaceType;
	short TrackAround;
	short MpMax;
	DWORD RewardItemGroup[5]; //��� or �ð� ����
	//int RewardItemBagGroup[5]; //�׷캰 ���� �����۹�
	int RewardItemBagGroup[5][4]; //�׷캰 ���� �����۹�

	tagEventRaceData() :
		EventNo(0),
		StartEffect(0),
		LimitTime(0),
		MoveSpeed(0),
		UseSkill(false),
		RaceType(0),
		TrackAround(0),
		MpMax(0)
		{
			for(int i = 0; i < 5; i++)
			{
				RewardItemGroup[i] = 0;
				for(int j = 0; j < 4; j++)
				{
					RewardItemBagGroup[i][j] = 0;
				}
			}
		}

	size_t min_size() const
	{
		return sizeof(int)
			+ sizeof(int)
			+ sizeof(short)
			+ sizeof(short)
			+ sizeof(bool)
			+ sizeof(BYTE)
			+ sizeof(short)
			+ sizeof(short)
			+ (sizeof(DWORD) * 5)
			+ (sizeof(int) * 20);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(EventNo);
		Packet.Push(StartEffect);
		Packet.Push(LimitTime);
		Packet.Push(MoveSpeed);
		Packet.Push(UseSkill);
		Packet.Push(RaceType);
		Packet.Push(TrackAround);
		Packet.Push(MpMax);
		for(int i = 0; i < 5; i++)
		{
			Packet.Push(RewardItemGroup[i]);
			for(int j = 0; j < 4; j++)
			{
				Packet.Push(RewardItemBagGroup[i][j]);
			}
		}
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(EventNo);
		Packet.Pop(StartEffect);
		Packet.Pop(LimitTime);
		Packet.Pop(MoveSpeed);
		Packet.Pop(UseSkill);
		Packet.Pop(RaceType);
		Packet.Pop(TrackAround);
		Packet.Pop(MpMax);
		for(int i = 0; i < 5; i++)
		{
			Packet.Pop(RewardItemGroup[i]);
			for(int j = 0; j < 4; j++)
			{
				Packet.Pop(RewardItemBagGroup[i][j]);
			}
		}
	}
} SEventRaceData;

int const MAX_PREMIUM_ARTICLE = 30;
typedef struct tagTblDefPremiumService
{
	tagTblDefPremiumService()
		: iServiceNo(0)
		, byServiceType(0)
		, byGrade(0)
		, wUseDate(0)
	{
		::memset(iArticle, 0, sizeof(iArticle));
	}

	int iServiceNo;
	BYTE byServiceType;
	BYTE byGrade;
	WORD wUseDate;
	int iArticle[MAX_PREMIUM_ARTICLE];
	std::wstring kTitle;

	size_t min_size() const
	{
		return sizeof(int) +
			sizeof(BYTE) +
			sizeof(BYTE) +
			sizeof(WORD) +
			sizeof(iArticle) +
			sizeof(int);
	}

	void WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(iServiceNo);
		Packet.Push(byServiceType);
		Packet.Push(byGrade);
		Packet.Push(wUseDate);
		Packet.Push(iArticle, sizeof(iArticle));
		Packet.Push(kTitle);
	}

	void ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(iServiceNo);
		Packet.Pop(byServiceType);
		Packet.Pop(byGrade);
		Packet.Pop(wUseDate);
		Packet.PopMemory(iArticle, sizeof(iArticle));
		Packet.Pop(kTitle);
	}
}TBL_DEF_PREMIUM_SERVICE;

int const MAX_PREMIUM_ARTICLE_ABIL = 10;
typedef struct tagTblDefPremiumArticle
{
	tagTblDefPremiumArticle()
		: iServiceTypeNo(0)
		, wType(0)
		, wLv(0)
	{
		::memset(iAbil, 0, sizeof(iAbil));
		::memset(iValue, 0, sizeof(iValue));
	}

	int iServiceTypeNo;
	WORD wType;
	WORD wLv;
	int iAbil[MAX_PREMIUM_ARTICLE_ABIL];
	int iValue[MAX_PREMIUM_ARTICLE_ABIL];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_PREMIUM_ARTICLE;

typedef struct tagTblDefJumpingCharEvent
{
	tagTblDefJumpingCharEvent() : iEventNo(0), iLevelLimit(0), iMaxRewardCount(0), iRewardGroupNo(0)
	{
	}

	int iEventNo;
	BM::DBTIMESTAMP_EX StartTime;
	BM::DBTIMESTAMP_EX EndTime;
	int iLevelLimit;
	int iMaxRewardCount;
	int iRewardGroupNo;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JUMPINGCHAREVENT;

typedef struct tagTblDefJumpingCharEvent_Reward
{
	tagTblDefJumpingCharEvent_Reward()
	{
		memset(this, 0, sizeof(*this));
	}

	int iRewardGroupNo;
	int iClass;
	int iLevel;
	int iMapNo;
	BYTE f_IngQuest[MAX_DB_INGQUEST_SIZE];
	BYTE f_EndQuest[MAX_DB_ENDQUEST_SIZE];
	BYTE f_EndQuest2[MAX_DB_ENDQUEST_SIZE];
	BYTE f_EndQuestExt[MAX_DB_ENDQUEST_EXT_SIZE];
	BYTE f_EndQuestExt2[MAX_DB_ENDQUEST_EXT2_SIZE];
	int iItemGroupNo;

	bool operator < (const tagTblDefJumpingCharEvent_Reward &rhs)const
	{
		if( iRewardGroupNo < rhs.iRewardGroupNo )	{return true;}
		if( iRewardGroupNo > rhs.iRewardGroupNo )	{return false;}

		if( iClass < rhs.iClass )	{return true;}
		if( iClass > rhs.iClass )	{return false;}

		return false;
	}

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JUMPINGCHAREVENT_REWARD;

const int MAX_JUMPING_CHAR_EVENT_ITEM_ENCHANT = 4;
typedef struct tagTblDefJumpingCharEvent_Item
{
	tagTblDefJumpingCharEvent_Item()
	{
		memset(this, 0, sizeof(*this));
	}

	int iItemGroupNo;
	BYTE byInvType;
	BYTE byInvPos;
	int iItemNo;
	short sCount;
	__int64 i64Enchant[MAX_JUMPING_CHAR_EVENT_ITEM_ENCHANT];
	BYTE byState;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_JUMPINGCHAREVENT_ITEM;

typedef struct tagTblDefQuestLevelReward
{
	tagTblDefQuestLevelReward()
	{
		memset(this, 0, sizeof(*this));
	}
	int iLevel;
	int iExp;
	int iMoney;
	
	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_QUEST_LEVEL_REWARD;


typedef struct tagTblDefJobskill_Event_Location
{
	tagTblDefJobskill_Event_Location()
	{
		memset(this, 0, sizeof(*this));
	}
	void WriteToPacket(BM::Stream &kPacket)const
	{
		kPacket.Push(iJobGrade);
		kPacket.Push(iRewardItemGroup);
		kPacket.Push(iRate);
		kPacket.Push(kStartDate);
		kPacket.Push(kEndDate);
	}
	void ReadFromPacket(BM::Stream &kPacket)
	{
		kPacket.Pop(iJobGrade);
		kPacket.Pop(iRewardItemGroup);
		kPacket.Pop(iRate);
		kPacket.Pop(kStartDate);
		kPacket.Pop(kEndDate);
	}	
	size_t min_size()const
	{
		return 
			sizeof(int)+
			sizeof(int)+
			sizeof(int)+
			sizeof(BM::PgPackedTime)+
			sizeof(BM::PgPackedTime);
	}
	bool operator < (tagTblDefJobskill_Event_Location const rhs) const
	{
		if( (rhs.iJobGrade == iJobGrade ) )
		{
			return (iRewardItemGroup < rhs.iRewardItemGroup);
		}
		else
		{
			return (iJobGrade < rhs.iJobGrade);
		}
	}
	bool operator == (tagTblDefJobskill_Event_Location const rhs) const
	{
		return( (iJobGrade == rhs.iJobGrade) && (iRewardItemGroup == rhs.iRewardItemGroup));
	}
	operator std::wstring const& ()const
	{
		BM::vstring kStr(L"");
		kStr += L"<";
		kStr += iJobGrade;
		kStr += L",";
		kStr += iRewardItemGroup;
		kStr += L">";
		return kStr;
	}
	int iJobGrade;
	int iRewardItemGroup;
	int iRate;

	BM::PgPackedTime kStartDate;
	BM::PgPackedTime kEndDate;

}TBL_DEF_JOBSKILL_EVENT_LOCATION;

typedef struct tagTblDefCharacterCreateSet
{
	tagTblDefCharacterCreateSet()
	{
		memset(this, 0, sizeof(*this));
	}

	bool operator < (const tagTblDefCharacterCreateSet &rhs)const
	{
		if (iSetNo < rhs.iSetNo) { return true; }
		if (iSetNo > rhs.iSetNo) { return false; }
		
		if (byClassNo < rhs.byClassNo) { return true; }
		if (byClassNo > rhs.byClassNo) { return false; }
		
		if (byGender < rhs.byGender) { return true; }
		if (byGender > rhs.byGender) { return false; }
		
		return false;
	}

	
	int iSetNo;
	BYTE byClassNo;
	BYTE byGender;
	BYTE byInvType;
	BYTE byInvPos;
	int iItemNo;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CHARACTER_CREATE_SET;

const int MAX_DAILY_REWARD_ITEM = 7;
typedef struct tagTblDefDailyReward
{
	int iDayNo;
	int iItems[MAX_DAILY_REWARD_ITEM];

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_DAILY_REWARD;

typedef struct tagTblDefCartMissionMonster
{
	int iMonBagNo;

	DEFAULT_TBL_PACKET_FUNC();
}TBL_DEF_CART_MISSION_MONSTER;

typedef struct tagTblBattlePassQuest
{
	int iBattlePassIdx;
	int iGradeNo;
	int iBattlePassDay;
	int iBattlePassQuest;
	DEFAULT_TBL_PACKET_FUNC();
} TblBattlePassQuest;

#pragma pack ()

typedef std::map<TBL_TRIPLE_KEY_INT, TBL_DEF_CART_MISSION_MONSTER> CONT_DEF_CART_MISSION_MONSTER;
typedef std::map<TBL_KEY_INT, TBL_DEF_DAILY_REWARD>					CONT_DEF_DAILY_REWARD;
typedef std::map<int,STBL_DEFSKILLEXTENDITEM>				CONT_DEFSKILLEXTENDITEM;
typedef std::map<TBL_TRIPLE_KEY_INT,SDefBasicOptionAmp>		CONT_DEFBASICOPTIONAMP;
typedef std::map<TBL_KEY_INT,SDefItemAmplify_Specific>		CONT_DEFITEM_AMP_SPECIFIC;
typedef std::map<int,SGROUNDBUILDINGS>						CONT_DEFGROUNDBUILDINGS;
typedef std::map<int,SMYHOMEBUILDING>						CONT_DEFMYHOMEBUILDINGS;
typedef std::map<int,SMYHOMESIDEJOBTIME>					CONT_DEFMYHOMESIDEJOBTIME;				
typedef std::map<BYTE,short>								CONT_DEFREDICEOPTIONCOST;
typedef std::map<int,TBL_DEFSIDEJOBRATE>					CONT_DEFSIDEJOBRATE;
typedef std::map<int,TBL_DEFCHARCARDEFFECT>					CONT_DEFCHARCARDEFFECT;
typedef std::map<int,TBL_EVENT_ITEM_REWARD>					CONT_EVENT_ITEM_REWARD;
typedef std::map<int,TBL_TREASURE_CHEST>					CONT_TREASURE_CHEST;
typedef std::map<BYTE,int>									CONT_DEFITEMENCHANTABILWEIGHT;
typedef std::map<int,BM::GUID>								CONT_SHOPNPC_GUID;
typedef std::map<TBL_KEY_SHORT,TBL_DEF_HOMETOWNTOMAPCOST>	CONT_HOMETOWNTOMAPCOST;
typedef std::map<TBL_KEY_SHORT,TBL_DEF_MYHOME_TEX>			CONT_MYHOME_TEX;
typedef std::map<TBL_KEY_SHORT,TBL_DEF_MYHOME_DEFAULT_ITEM>	CONT_MYHOME_DEFAULTITEM;
typedef std::map<TBL_KEY_SHORT,int>							CONT_TOWN2GROUND;
typedef std::map<TBL_DEF_MIXUP_ITEM_KEY,TBL_DEF_MIXUP_ITEM>	CONT_MIXUPITEM;
typedef std::map<int,int>									CONT_IDX2SKILLNO;
typedef std::map<int,TBL_DEF_SKILLOPTION>					CONT_RAREOPT_SKILL;
typedef std::map<int,TBL_DEF_MAGICOPTION>					CONT_RAREOPT_MAGIC;

typedef std::map< BM::GUID,TBL_REALTYDEALER>				CONT_REALTYDEALER;
typedef std::map< int,TBL_CCE_REWARD_TABLE>					CONT_CCE_REWARD_TABLE;
typedef std::map< SGAMBLE_SHOUT_KEY,BYTE>					CONT_GAMBLE_SHOUT_ITEM;
typedef std::map< TBL_KEY_INT,	TBL_LIMITED_ITEM >			CONT_LIMITED_ITEM;
typedef std::map< tagLimitItemControlKey,	VEC_LIMITED_ITEM_CONTROL >	CONT_LIMITED_ITEM_CONTROL;
typedef std::map< int,TBL_DEF_CONVERTITEM>					CONT_DEF_CONVERTITEM;
typedef std::map< int, TBL_DEF_PET_HATCH >					CONT_DEF_PET_HATCH;
typedef std::map< int, TBL_DEF_PET_BONUSSTATUS >			CONT_DEF_PET_BONUSSTATUS;
typedef std::map< unsigned long,BM::GUID>					CONT_PCROOMIP;

typedef std::map< std::wstring, BYTE >						CONT_PCCAFE;
typedef std::map< BYTE, SPCCafeAbil >						CONT_PCCAFE_ABIL;

typedef std::map< int,SGAMBLE >								CONT_GAMBLE;
typedef std::set< WORD >									CONT_CASHITEMABILFILTER;
typedef std::map< SEMOTIONKEY, int>							CONT_EMOTION;
typedef std::map< SEMOTIONGROUPKEY, int>					CONT_EMOTION_GROUP;

typedef std::map< MonsterCard_Key, int>						CONT_MONSTERCARD;

typedef std::map< int,__int64>								CONT_MARRYTEXT;
typedef std::multimap< TBL_KEY_INT, SHiddenRewardItem >		CONT_HIDDENREWORDITEM;
typedef std::map< int, SHIDDENREWORDBAG >					CONT_HIDDENREWORDBAG;

typedef std::map< BM::GUID, SGEMSTORE >						CONT_GEMSTORE;

typedef std::map< TBL_KEY_INT, SCARD_LOCAL>					CONT_CARD_LOCAL;
typedef std::map< TBL_DEF_CARD_KEY_STRING_KEY, TBL_DEF_CARD_KEY_STRING>	CONT_DEF_CARD_KEY_STRING;

typedef std::map< TBL_CARD_ABIL_KEY, SCHARACTER_CARD_ABIL>	CONT_CARD_ABIL;
typedef std::map< TBL_KEY_INT, SOXQUIZINFO>					CONT_OXQUIZINFO;
typedef std::map< SOXQUIZREWARDKEY, SOXQUIZREWARD>			CONT_OXQUIZREWARD;
typedef std::map< TBL_KEY_INT, SOXQUIZEVENTINFO>			CONT_OXQUIZEVENTINFO;
typedef std::map< TBL_KEY_INT, SLUCKYSTAREVENTINFO>					CONT_LUCKYSTAREVENTINFO;
typedef std::map< SLUCKYSTARREWARDKEY, SCONT_LUCKYSTAR_REWARD>		CONT_LUCKYSTARREWARD;
typedef std::map< TBL_KEY_INT, SCONT_LUCKYSTAR_REWARD_STEP>			CONT_LUCKYSTARREWARDSTEP;
typedef std::map< TBL_KEY_INT, SCONT_LUCKYSTAR_EVENT_JOIN_REWARD>	CONT_LUCKYSTAREVENTJOINREWARD;
typedef std::map< BM::GUID, SLUCKYSTARJOINEDUSERINFO>				CONT_LUCKYSTARJOINEDUSERINFO;


typedef std::map< TBL_KEY_INT, SCONT_RARE_MONSTER_SPEECH>	CONT_DEF_RARE_MONSTER_SPEECH;

//�Ʒ� �����̳ʴ� second�� ��ġ�� �ȵ�.
typedef std::map< TBL_KEY_INT, CONT_DEF_RARE_MONSTER_GEN>	CONT_DEF_GROUND_RARE_MONSTER;
typedef std::map< TBL_KEY_INT, STBL_DEF_ITEM2ACHIEVEMENT>	CONT_DEF_ITEM2ACHIEVEMENT;
typedef std::map< TBL_KEY_INT, SRECOMMENDATION_ITEM>		CONT_DEF_RECOMMENDATION_ITEM;
typedef std::map< TBL_KEY_INT, CONT_DEF_ACHIEVEMENTS>		CONT_DEF_CONT_ACHIEVEMENTS;
typedef std::map< TBL_KEY_INT, TBL_DEF_BASE_CHARACTER >		CONT_DEF_BASE_CHARACTER;
typedef std::map< TBL_KEY_INT, TBL_DEF_ABILTYPE >			CONT_DEF_ABIL_TYPE;

//typedef std::map< TBL_KEY_INT, TBL_DEF_AIPATTERN >			CONT_DEFAIPATTERN;
typedef std::map< TBL_KEY_INT, TBL_DEF_CLASS >				CONT_DEFCLASS;
typedef std::map< TBL_KEY_INT, TBL_DEF_CLASS_ABIL >			CONT_DEFCLASS_ABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_CLASS_PET >			CONT_DEFCLASS_PET;
typedef std::map< SClassKey, TBL_DEF_CLASS_PET_LEVEL >		CONT_DEFCLASS_PET_LEVEL;
typedef BM::PgApproximateVector< TBL_DEF_CLASS_PET_SKILL >	CONT_DEFCLASS_PET_SKILL_ELEMENT;
typedef std::map< int, CONT_DEFCLASS_PET_SKILL_ELEMENT >	CONT_DEFCLASS_PET_SKILL;
typedef std::map< SClassKey, TBL_DEF_CLASS_PET_ITEMOPTION >	CONT_DEFCLASS_PET_ITEMOPTION;
typedef std::map< TBL_KEY_INT, TBL_DEF_CLASS_PET_ABIL >		CONT_DEFCLASS_PET_ABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM >				CONT_DEFITEM;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEMABIL >			CONT_DEFITEMABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_DROP_MONEY_CONTROL >	CONT_DEF_DROP_MONEY_CONTROL;

typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_RES_CONVERT >				CONT_DEF_ITEM_RES_CONVERT;


typedef std::map< TBL_KEY_INT, TBL_DEF_ITEMRARE >			CONT_DEFITEMRARE;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEMRAREGROUP >		CONT_DEFITEMRAREGROUP;
typedef std::map< TBL_KEY_INT, TBL_DEF_MAP >				CONT_DEFMAP;
typedef std::map< TBL_KEY_INT, TBL_DEF_MAP_ABIL>			CONT_DEFMAP_ABIL;
//typedef std::map< TBL_KEY_INT, TBL_DEF_REVIVEPOS >			CONT_DEF_REVIVEPOS;
typedef std::map< TBL_KEY_GUID, TBL_DEF_MAPITEM >		CONT_DEFMAPITEM;
typedef std::map< TBL_KEY_INT, TBL_DEF_MAP_REGEN_POINT >	CONT_DEF_MAP_REGEN_POINT;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER >			CONT_DEFMONSTER;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTERABIL >		CONT_DEFMONSTERABIL;
typedef std::map< SMonsterTunningKey, TBL_DEF_MONSTERTUNNING >		CONT_DEFMONSTERTUNNINGABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_NPC >				CONT_DEFNPC;
typedef std::map< TBL_KEY_INT, TBL_DEF_NPCABIL >			CONT_DEFNPCABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_DYNAMICABILRATE >	CONT_DEFDYNAMICABILRATE;
typedef std::map< TBL_KEY_INT, TBL_DEF_DYNAMICABILRATE_BAG>	CONT_DEFDYNAMICABILRATE_BAG;
typedef std::map< TBL_KEY_INT, TBL_DEF_SKILL >				CONT_DEFSKILL;
typedef std::map< TBL_KEY_INT, TBL_DEF_SKILLABIL >			CONT_DEFSKILLABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_SKILLSET >			CONT_DEFSKILLSET;
typedef std::vector< TBL_CHANNEL_EFFECT > CONT_DEF_CHANNEL_EFFECT;
typedef std::map< TBL_KEY_INT, TBL_DEF_STRING >				CONT_DEFSTRINGS;
typedef std::map< TBL_KEY_INT, TBL_DEF_RES >				CONT_DEFRES;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEMMAKING >			CONT_DEFITEMMAKING;
typedef std::map< TBL_KEY_INT, TBL_DEF_COOKING >			CONT_DEFCOOKING;
typedef std::map< TBL_KEY_INT, TBL_DEF_RESULT_CONTROL >		CONT_DEFRESULT_CONTROL;
typedef std::map< TBL_KEY_INT, TBL_DEF_EFFECT>				CONT_DEFEFFECT;
typedef std::map< TBL_KEY_INT, TBL_DEF_EFFECTABIL>			CONT_DEFEFFECTABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_EFFGRADECLASS>		CONT_DEFUPGRADECLASS;
typedef std::map< SItemEnchantKey, TBL_DEF_ITEM_ENCHANT >		CONT_DEFITEMENCHANT;
typedef std::map< TBL_KEY_INT, TBL_DEF_CHARACTER_BASEWEAR >	CONT_DEFCHARACTER_BASEWEAR;// first key : TBL_DEF_CHARACTER_BASEWEAR::iWearNo
typedef std::map< SItemPlusUpgradeKey, TBL_DEF_ITEMPLUSUPGRADE >	CONT_DEF_ITEM_PLUS_UPGRADE;// 
typedef std::map< TBL_TRIPLE_KEY_INT, TBL_DEF_ITEMENCHANTSHIFT >	CONT_DEF_ITEM_ENCHANT_SHIFT;// first key : EquipType,Grade,LevelLimit
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_RARITY_UPGRADE >	CONT_DEF_ITEM_RARITY_UPGRADE;// 
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_RARITY_CONTROL >	CONT_DEF_ITEM_RARITY_CONTROL;// 
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_PLUSUP_CONTROL >	CONT_DEF_ITEM_PLUSUP_CONTROL;

typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_DISASSEMBLE >	CONT_DEF_ITEM_DISASSEMBLE;// 
typedef std::map< TBL_KEY_INT, TBL_DEF_SUCCESS_RATE_CONTROL >	CONT_DEF_SUCCESS_RATE_CONTROL;// 
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER_BAG > CONT_DEF_MONSTER_BAG;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER_BAG_ELEMENTS > CONT_DEF_MONSTER_BAG_ELEMENTS;
typedef std::map< TBL_KEY_INT, TBL_DEF_MONSTER_BAG_CONTROL > CONT_DEF_MONSTER_BAG_CONTROL;
typedef std::map< TBL_KEY_INT, TBL_DEF_OBJECT >					CONT_DEF_OBJECT;
typedef std::map< TBL_KEY_INT, TBL_DEF_OBJECTABIL >				CONT_DEF_OBJECTABIL;
typedef std::map< TBL_KEY_INT, TBL_DEF_OBJECT_BAG >				CONT_DEF_OBJECT_BAG;
typedef std::map< TBL_KEY_INT, TBL_DEF_OBJECT_BAG_ELEMENT >		CONT_DEF_OBJECT_BAG_ELEMENTS;
typedef std::map< int, TBL_DEF_ITEM_BY_LEVEL >					CONT_DEF_ITEM_BY_LEVEL;
typedef std::map< TBL_KEY_INT, TBL_DEF_GMCMD > CONT_DEFGMCMD;
typedef std::map< std::pair< int, short >, TBL_DEF_ITEM_BAG > CONT_DEF_ITEM_BAG;// key : first=bag_no, second=level
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_CONTAINER> CONT_DEF_ITEM_CONTAINER;
typedef std::map< TBL_KEY_INT, TBL_DEF_COUNT_CONTROL > CONT_DEF_COUNT_CONTROL;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_BAG_ELEMENTS > CONT_DEF_ITEM_BAG_ELEMENTS;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_SET > CONT_DEF_ITEM_SET;

typedef std::map< TBL_KEY_INT, TBL_DEF_MISSION_RESULT > CONT_DEF_MISSION_RESULT;
typedef std::map< TBL_KEY_INT, TBL_DEF_MISSION_CANDIDATE > CONT_DEF_MISSION_CANDIDATE;
typedef std::map< TBL_KEY_INT, TBL_DEF_MISSION_ROOT > CONT_DEF_MISSION_ROOT;
typedef std::map< TBL_KEY_INT, TBL_DEF_MISSION_LEVELLIMIT> CONT_DEF_MISSION_LEVELLIMIT;
typedef std::map< SMissionKey, TBL_MISSION_REPORT> CONT_MISSION_REPORT;
typedef std::vector<TBL_MISSION_RANK>	CONT_MISSION_RANK;
typedef std::map< SDefenceAddMonsterKey, TBL_DEF_DEFENCE_ADD_MONSTER> CONT_DEF_DEFENCE_ADD_MONSTER;

typedef std::vector< TBL_DEF_ITEM_OPTION > CONT_TBL_DEF_ITEM_OPTION;
typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_OPTION_ABIL > CONT_TBL_DEF_ITEM_OPTION_ABIL;

typedef std::map< TBL_KEY_SHORT, TBL_SHOP_IN_GAME > CONT_TBL_SHOP_IN_EMPORIA;
typedef std::map< TBL_KEY_INT, TBL_SHOP_IN_GAME > CONT_TBL_SHOP_IN_GAME;
typedef std::map< TBL_KEY_UINT, TBL_SHOP_IN_GAME > CONT_TBL_SHOP_IN_STOCK;
typedef std::map< int, TBL_SHOP_IN_GAME > CONT_DEF_JOBSKILL_SHOP;

typedef std::map< TBL_KEY_INT, CONT_DEF_PVP_MAP > CONT_DEF_PVP_GROUNDGROUP;
typedef std::map< TBL_KEY_INT, TBL_DEF_PVP_GROUNDMODE > CONT_DEF_PVP_GROUNDMODE;
typedef std::vector< TBL_DEF_PVP_REWARD > CONT_DEF_PVP_REWARD;
typedef std::map< int, TBL_DEF_PVP_REWARD_ITEM > CONT_DEF_PVP_REWARD_ITEM;
typedef std::vector< TBL_DEF_PLAYLIMIT_INFO > CONT_DEF_PLAYLIMIT_INFO_LIST;
typedef std::map< int, CONT_DEF_PLAYLIMIT_INFO_LIST > CONT_DEF_PLAYLIMIT_INFO;
typedef std::map< TBL_KEY_INT, TBL_DEF_QUEST_REWARD > CONT_DEF_QUEST_REWARD;
typedef std::vector< TBL_DEF_QUEST_RANDOM > CONT_DEF_QUEST_RANDOM;
typedef std::map< TBL_KEY_INT, TBL_DEF_QUEST_WANTED > CONT_DEF_QUEST_WANTED;

typedef std::map< TBL_KEY_INT, TBL_DEF_SPEND_MONEY > CONT_DEF_SPEND_MONEY;
typedef std::map< TBL_KEY_GUID, SGuildMemberInfo > ContGuildMember;

typedef std::map< TBL_KEY_INT, TBL_DQT_DEF_ITEM_BAG_GROUP > CONT_DEF_ITEM_BAG_GROUP;
typedef std::map< TBL_KEY_INT, TBL_DQT_DEF_MAP_ITEM_BAG > CONT_DEF_MAP_ITEM_BAG;
typedef std::map< TBL_DEF_PROPERTY_KEY,TBL_DEF_PROPERTY> CONT_DEF_PROPERTY;
typedef std::vector<TBL_DEF_MAP_EFFECT> CONT_MAP_EFFECT;
typedef std::vector<TBL_DEF_MAP_ENTITY> CONT_MAP_ENTITY;

typedef std::map< TBL_KEY_INT, TBL_DEF_FIVE_ELEMENT_INFO > CONT_FIVE_ELEMENT_INFO;
typedef std::map< TBL_KEY_INT, TBL_DEF_MAP_STONE_CONTROL > CONT_MAP_STONE_CONTROL;

typedef std::map< TBL_KEY_INT, TBL_DEF_ITEM_RARITY_UPGRADE_COST_RATE > CONT_ITEM_RARITY_UPGRADE_COST_RATE;

typedef std::map< BM::GUID, TBL_DEF_EMPORIA_PACK > CONT_DEF_EMPORIA;

typedef std::map< SMISSION_CLASS_REWARD_KEY, SMISSION_CLASS_REWARD_BAG > CONT_MISSION_CLASS_REWARD_BAG;
typedef std::map< SMISSION_RANK_REWARD_KEY, SMISSION_RANK_REWARD_BAG > CONT_MISSION_RANK_REWARD_BAG;

typedef std::map< SMISSION_DEFENCE_STAGE_KEY, SMISSION_DEFENCE_STAGE_BAG > CONT_MISSION_DEFENCE_STAGE_BAG;
typedef std::map< SMISSION_DEFENCE_WAVE_KEY, SMISSION_DEFENCE_WAVE_BAG > CONT_MISSION_DEFENCE_WAVE_BAG;

typedef std::vector< TBL_DEFDEATHPENALTY > VEC_DEFDEATHPENALTY;
typedef std::map< int, VEC_DEFDEATHPENALTY > CONT_DEFDEATHPENALTY;	// <GroundNo, TBL_DEFDEATHPENALTY>

typedef std::map< BM::GUID, SDEFGEMSTORE >						CONT_DEFGEMSTORE;
typedef std::map< TBL_PAIR_KEY_INT, SDEF_ANTIQUE >				CONT_DEFANTIQUE;	//PriKey:Menu, SecKey:ItemNo


typedef std::map< SMISSION_DEFENCE7_MISSION_KEY, SMISSION_DEFENCE7_MISSION_BAG > CONT_MISSION_DEFENCE7_MISSION_BAG;
typedef std::map< SMISSION_DEFENCE7_STAGE_KEY, SMISSION_DEFENCE7_STAGE_BAG > CONT_MISSION_DEFENCE7_STAGE_BAG;
typedef std::map< SMISSION_DEFENCE7_WAVE_KEY, SMISSION_DEFENCE7_WAVE_BAG > CONT_MISSION_DEFENCE7_WAVE_BAG;
typedef std::map< TBL_KEY_INT, SMISSION_DEFENCE7_GUARDIAN_BAG > CONT_MISSION_DEFENCE7_GUARDIAN_BAG;

typedef std::map< TBL_KEY_INT, SMISSION_BONUSMAP_BAG > CONT_MISSION_BONUSMAP;

typedef std::vector< TBL_DEF_PVPLEAGUE_TIME > CONT_DEF_PVPLEAGUE_TIME;
typedef std::map< int, TBL_DEF_PVPLEAGUE_SESSION > CONT_DEF_PVPLEAGUE_SESSION;
typedef std::vector< TBL_DEF_PVPLEAGUE_REWARD > VEC_DEF_PVPLEAGUE_REWARD;
typedef std::map< int, VEC_DEF_PVPLEAGUE_REWARD > CONT_DEF_PVPLEAGUE_REWARD; // <LeagueLevel, TBL_DEF_PVPLEAGUE_REWARD>

typedef std::map< POINT2, TBL_DEF_SPECIFIC_REWARD > CONT_DEF_SPECIFIC_REWARD;
typedef std::map< int, TBL_DEF_SPECIFIC_REWARD_EVENT > CONT_DEF_SPECIFIC_REWARD_EVENT;


typedef std::map< int, SProbabilityBag >								CONT_DEF_JOBSKILL_PROBABILITY_BAG;
typedef std::map< int, SJobSkillItemUpgrade >							CONT_DEF_JOBSKILL_ITEM_UPGRADE;
typedef std::map< int, SJobSkillSaveIdx >								CONT_DEF_JOBSKILL_SAVEIDX;
typedef std::map< TBL_KEY_INT, TBL_DEF_JOBSKILL_LOCATIONITEM >			CONT_DEF_JOBSKILL_LOCATIONITEM;
typedef std::map< TBL_KEY_INT, TBL_DEF_JOBSKILL_SKILL >					CONT_DEF_JOBSKILL_SKILL;
typedef std::list< TBL_DEF_JOBSKILL_SKILLEXPERTNESS >					CONT_DEF_JOBSKILL_SKILLEXPERTNESS;
typedef std::map< TBL_KEY_INT, TBL_DEF_JOBSKILL_TOOL >					CONT_DEF_JOBSKILL_TOOL;
typedef std::map< int, SJobSkillRecipe >								CONT_DEF_JOBSKILL_RECIPE;

typedef std::list< SJobSkillSoulExtract >								CONT_DEF_JOBSKILL_SOUL_EXTRACT;
typedef std::list< SJobSkillSoulTransition >							CONT_DEF_JOBSKILL_SOUL_TRANSITION;

typedef std::map< SocketItem_Key, SocketItem_Bag >						CONT_DEF_SOCKET_ITEM;

typedef std::map< BM::GUID, SExpeditionNpc >					CONT_DEF_EXPEDITION_NPC;

typedef std::map< TBL_KEY_INT, CONT_SET_DATA >					CONT_DEF_BS_GEN_GROUND;

// Mission mutator
typedef std::map< TBL_KEY_INT, SMissionMutator >					CONT_DEF_MISSION_MUTATOR;
typedef std::map< TBL_KEY_INT, SMissionMutatorAbil >				CONT_DEF_MISSION_MUTATOR_ABIL;

typedef std::map< TBL_KEY_INT, SEventGroup >					CONT_DEF_EVENT_GROUP;
typedef std::map< TBL_KEY_INT, CONT_EVENTMONSTERGROUP>			CONT_DEF_EVENT_MONSTER_GROUP;
typedef std::map< TBL_KEY_INT, VEC_EVENTREWARDITEMGROUP>		CONT_DEF_EVENT_REWARD_ITEM_GROUP;
typedef std::map< TBL_KEY_INT, SEventScheduleData>				CONT_DEF_EVENT_SCHEDULE;
typedef std::map< TBL_KEY_INT, SEventBossBattleData>			CONT_DEF_EVENT_BOSSBATTLE;
typedef std::map< TBL_KEY_INT, SEventRaceData>					CONT_DEF_EVENT_RACE;

typedef std::map< TBL_KEY_INT, TBL_DEF_PREMIUM_SERVICE >				CONT_DEF_PREMIUM_SERVICE;
typedef std::map< TBL_KEY_INT, TBL_DEF_PREMIUM_ARTICLE >				CONT_DEF_PREMIUM_ARTICLE;

typedef std::map< TBL_KEY_INT, TBL_DEF_JUMPINGCHAREVENT >				CONT_DEF_JUMPINGCHAREVENT;
typedef std::set< TBL_DEF_JUMPINGCHAREVENT_REWARD >						CONT_SET_JUMPINGCHAREVENT_REWARD;
typedef std::map< TBL_KEY_INT, CONT_SET_JUMPINGCHAREVENT_REWARD >		CONT_DEF_JUMPINGCHAREVENT_REWARD;

typedef std::map< TBL_KEY_INT, TBL_DEF_QUEST_LEVEL_REWARD >			CONT_DEF_QUEST_LEVEL_REWARD;
typedef std::list< TBL_DEF_JOBSKILL_EVENT_LOCATION > CONT_DEF_JOBSKILL_EVENT_LOCATION;

typedef std::set<TBL_DEF_CHARACTER_CREATE_SET>	CONT_DEF_CHARACTER_CREATE_SET;
typedef std::map< TBL_TRIPLE_KEY_INT, TblBattlePassQuest >				CONT_DEF_BATTLE_PASS_QUEST;

#endif // WE