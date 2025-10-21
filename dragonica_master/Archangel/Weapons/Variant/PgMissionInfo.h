#ifndef WEAPON_VARIANT_USERINFO_PGMISSIONINFO_H
#define WEAPON_VARIANT_USERINFO_PGMISSIONINFO_H

#include "PgStringUtil.h"

typedef std::vector< int > VEC_INT;

#define MISSION_PARAMNUM		5
#define MISSION_ITEM_TIME		4
#define MISSION_LEVEL_MAX		6

typedef enum eMissionMode
{
	MO_ITEM = 0,
	MO_SCENARIO,
	MO_DEFENCE,
	MO_DEFENCE7,
	MO_DEFENCE8
};

typedef enum eMissionOptionKind
{
	MO_CHAPTER = 1,
	MO_MISSION,
	MO_SCENARIO_TIMEATTACK
};

typedef enum eChapterType
{
	MC_NONE = 1,
	MC_MON_ALL_KILL,
	MC_MON_TARGET_KILL,
	MC_ITEM_GET,
	MC_OBJECT_KICK,
	MC_TIME_ATTACK,
	MC_EFFECT,
	MC_OBJECT_TARGET_KILL,
	MC_TYPE_MAX
} EChapterType;

typedef enum eMissionType
{
	MM_NONE = 1,
	MM_MON_TARGET_KILL,
	MM_OBJECT_KICK,
	MM_TIME_ATTACK,
	MM_MON_BOSS_KILL,
	MM_TYPE_MAX
} EMissionType;

typedef struct tagMissionBasicDepend
{
	tagMissionBasicDepend()
	{
		iType = 0;
		iObjectTextNo = 0;
	}

	int iType;
	int iObjectTextNo;
} SMissionBasicDepend;

typedef struct tagMissionBasicInfo
{
	int iMissionID;		//Mission IDX
	int iGroupNo;		//Group Mission No
	int iTimeMin;
	int iTimeSec;
	int iScript;
	int iErrorTTW;
	int iGadaCoinCount;
	int iItemTimeMin[MISSION_ITEM_TIME];
	int iItemTimeSec[MISSION_ITEM_TIME];
	int iEventMission;

	tagMissionBasicInfo();
	//tagMissionBasicInfo(int const iID, int const iGroup, int const iMin, int const iSec, int const iSt, int const iItemMin, int const iItemSec, int const iTTW)
	tagMissionBasicInfo(int const iID, int const iGroup, int const iMin, int const iSec, int const iSt, int const iTTW, bool EventMission = false);
	void Clear();
	void SetTime(VEC_WSTRING kVec);
	void SetItemTime(VEC_WSTRING kVec, int iLevel);
} SMissionBasicInfo;

typedef struct tagMissionOptionChapter 
	: public SMissionBasicDepend
{
	int iKind;
	int iCount;
	VEC_INT	m_kLimit_LevelValue;
	VEC_INT	m_kLimit_EffectValue;
	VEC_INT m_kLimit_KindValue;

	tagMissionOptionChapter();
	tagMissionOptionChapter(int const ObjectTextNo);
	bool Build(std::wstring const &rkType, int const &Count, std::wstring const &rkLevel, std::wstring const &rkEffect, std::wstring const &rkValue);
	void GetEffectNo(VEC_INT &rkEffect) const;
	void GetKindNo(VEC_INT &rkValue) const;

} SMissionOptionChapter;
//typedef std::multimap< unsigned int, SMissionOptionChapter > ContMissionOptionChapter;
typedef std::vector< SMissionOptionChapter > ContMissionOptionChapter;
typedef std::map< unsigned int, ContMissionOptionChapter > ContMissionOptionChapterList;
typedef std::map< unsigned int, ContMissionOptionChapter > ContMissionOptionChapterLevelList;


typedef struct tagMissionOptionMission 
	: public SMissionBasicDepend
{
	int iCount;
	VEC_INT	m_kLimit_LevelValue;
	VEC_INT m_kLimit_KindValue;

	tagMissionOptionMission();
	tagMissionOptionMission(int const ObjectTextNo);
	bool Build(std::wstring const &rkType, int const &Count, std::wstring const &rkLevel, std::wstring const &rkValue);
	void GetKindNo(VEC_INT &rkValue) const;
} SMissionOptionMission;
typedef std::vector< SMissionOptionMission > ContMissionOptionMissionList;
typedef std::map< unsigned int, ContMissionOptionMissionList > ContMissionOptionMissionLevelList;

typedef struct tagMissionOptionMissionOpen
{
	int iLevel;
	int iNeedItem;
	VEC_INT m_kLimit_NeedItem;
	VEC_INT m_kLimit_IngQuestValue;
	VEC_INT m_kLimit_ClearQuestValue;
	VEC_INT	m_kLimit_PreLevelValue;
	VEC_INT	m_kLimit_EffectValue;
	
	tagMissionOptionMissionOpen();
	tagMissionOptionMissionOpen(int const Level,int const NeedItem, std::wstring const &rkLevel, std::wstring const &rkIngQuest, std::wstring const &rkClearQuest, std::wstring const &rkEffect,std::wstring const &rkNeedItem);
	bool PreLevelCheck(int const iPreLevel) const;
	bool PreLevelCheck(bool bClearLevel[]) const;
	void GetIngQuestIDVec(VEC_INT &rkIngQuest) const;
	void GetClearQuestIDVec(VEC_INT &rkIngQuest) const;
	void GetEffectNo(VEC_INT &rkEffect) const;
	void GetNeedItem(VEC_INT &rkEffect) const;
} SMissionOptionMissionOpen;
typedef std::vector< SMissionOptionMissionOpen > ContMissionOptionMissionOpenList;

typedef struct tagMissionObject
{
	int iObjectCount[MISSION_PARAMNUM];
	int iObjectText[MISSION_PARAMNUM];
	
	tagMissionObject();
	void Clear();
	int GetEndCount(int const iObjectNo) const;
	void SetObject(int iObjectIndex, int iText, int iCount);
	int GetTextNumber(int const iObjectNo) const;
} SMissionObject;

typedef enum eMissionItemLevelType
{
	MILT_LEVLE1		    = 0x0001,	// 레벨1(1)
	MILT_LEVLE2	        = 0x0002,	// 레벨2(2)
	MILT_LEVLE3	        = 0x0004,	// 레벨3(4)
	MILT_LEVLE4	        = 0x0008,	// 레벨4(8)
	MILT_LEVLE5		    = 0x0010,	// 시나리오(16)
	MILT_LEVLE6		    = 0x0020,	// 시나리오 결과(32)
	MILT_LEVLE7			= 0x0040,	// Defence 6(64)
    MILT_LEVLE8			= 0x0080,	// Defence 7(128)
	MILT_LEVLE9			= 0x0100,	// Defence 8(256)
    MILT_MAX            = 0x0200,
    
    MILT_NORMAL         = MILT_LEVLE1|MILT_LEVLE2|MILT_LEVLE3|MILT_LEVLE4|MILT_LEVLE5|MILT_LEVLE6,//(63)
	MILT_DEFENCE_ALL    = MILT_LEVLE7|MILT_LEVLE8|MILT_LEVLE9,//(448)
    MILT_CUSTOM1        = MILT_NORMAL|MILT_LEVLE7,//(127)
} EMissionItemLevelType;

typedef enum eDefence7ItemType
{
	DI_ADD_POINT		= 1,	// 포인트 추가
	DI_EFFECT_DAMAGE	= 2,	// 상대방 PC 데미지 감소
	DI_EFFECT_MOVESPEED	= 3,	// 상대방 PC 이속 감소
	DI_EFFECT_MOVESTOP	= 4,	// 상대방 PC 멈춤
	DI_EFFECT_BOMB		= 5,	// 지뢰
	DI_ADD_MONSTER		= 6,	// 특공대 투입
	DI_ATTACK			= 7,	// 미사일 발사
	DI_GUARDIAN			= 8,	// 가디언 정지
	DI_ADD_GUARDIAN		= 9,	// 슈퍼 가디언 소환
	DI_D7_RELAY_STAGE	= 10,	// 디펜스7 이어하기 아이템
	DI_D7_POINT_COPY	= 11,	// 디펜스7 포인트 복제 아이템.
	DI_TYPE_MAX
} EDefence7ItemType;

typedef enum eDefence7SkillType
{
	DS_ADD_POINT				= 1,	// 포인트 추가
	DS_ADD_MOVE					= 2,	// 이동속도 추가
	DS_ADD_PC_ATTACK			= 3,	// 파티원 공격력 상승
	DS_ADD_ENTITY_SKILL_01		= 4,	// 가디언 스킬1
	DS_ADD_ENTITY_SKILL_02		= 5,	// 가디언 스킬2
	DS_TYPE_MAX
} EDefence7SkillType;

typedef enum eDefenceGuardianType
{
	GT_CHANGE			= 1,
	GT_UPGRADE			= 2,
} EDefenceGuardianType;

typedef enum eDefencePointType : BYTE
{
    DP_NONE             = 0,    //일반 Point
    DP_STAGEEND         = 1,    //Stage End Point
} EDefencePointType;

class PgMissionInfo
{
public:
	PgMissionInfo();
	~PgMissionInfo();

	void Clear();

	void Build();
	void Clone(PgMissionInfo& rkOut) const;

	int ID()const {return m_kBasic.iMissionID;}
	int GroupNo()const {return m_kBasic.iGroupNo;}

	int GetTimeMin() const {return m_kBasic.iTimeMin;}
	int GetTimeSec() const {return m_kBasic.iTimeSec;}
	int GetScript() const {return m_kBasic.iScript;}
	int GetErrorText() const {return m_kBasic.iErrorTTW;}
	int GetGadaCoinCount() const {return m_kBasic.iGadaCoinCount;}
	int GetItemTimeMin(int iLevel) const {return m_kBasic.iItemTimeMin[iLevel];}
	int GetItemTimeSec(int iLevel) const {return m_kBasic.iItemTimeSec[iLevel];}
	bool GetHaveArcadeMode() const { return m_bIsHaveArcade; }

	bool ParseXml(TiXmlElement* pkNode);


	int GetParamEndCondition(eMissionOptionKind eKind, int iParamNo)const;

	bool GetMissionChapterList(int const iGround, ContMissionOptionChapter const *& pkOut) const;
//	bool SetMissionOptionChapter(int iGround);
	bool SetMissionOptionMission();
	bool GetMissionOpen(int const iLevel, SMissionOptionMissionOpen const*& pkOut) const;
	bool GetMissionOptionChapter( int const iGroundNo, int const iLevel, ContMissionOptionChapter& rkOut) const;
	bool GetMissionOptionMission(int const iLevel, ContMissionOptionMissionList const*& pkOut) const;
	bool GetMissionChapterListCheck(int const iGround) const;

protected:
	bool ParseXml_Chapter(TiXmlElement* pkNode);
	bool ParseXml_Mission(TiXmlElement* pkNode);
	bool ParseXml_MissionOpen(TiXmlElement* pkNode);
	bool ParseXml_MissionType(TiXmlElement* pkNode);
	bool ParseXml_Options(TiXmlElement* pkNode);

	virtual void ParseError(TiXmlNode* pkNode);
public:
	SMissionBasicInfo	m_kBasic;

	SMissionObject		m_kObjectChapter;
	SMissionObject		m_kObjectMission;	
	
	ContMissionOptionChapterList		m_kDepend_MissionOptionChapterList;
	ContMissionOptionMissionList		m_kDepend_MissionOptionMissionList;
	ContMissionOptionMissionOpenList	m_kDepend_MissionOptionMissionOpenList;

	ContMissionOptionChapterLevelList	m_kDepend_MissionOptionChapterLevelList;
	ContMissionOptionMissionLevelList	m_kDepend_MissionOptionMissionLevelList;

	bool				m_bIsHaveArcade;	// 아케이드 모드가 있는 미션이면 true, 없으면 false. 디폴트는 true
};


typedef struct tagGuardian_Install
{
	tagGuardian_Install() 
		: iGuardianNo(0)
	{}

	size_t min_size()const
	{
		return 
			sizeof(iGuardianNo)+
			sizeof(kGuid);
	}

	int iGuardianNo;
	BM::GUID kGuid;

	void WriteToPacket(BM::Stream & kPacket) const
	{
		kPacket.Push(iGuardianNo);
		kPacket.Push(kGuid);
	}

	void ReadFromPacket(BM::Stream & kPacket)
	{
		kPacket.Pop(iGuardianNo);
		kPacket.Pop(kGuid);
	}	
}SGuardian_Install;

typedef struct tagSGuardian_Skill
{
	tagSGuardian_Skill(int const _iGuardianNo=0, int const _iSkillNo=0)
		: iGuardianNo(_iGuardianNo)
		, iSkillNo(_iSkillNo)
	{}
	int iGuardianNo;
	int iSkillNo;
	BM::GUID kGuardianGuid;
}SGuardian_Skill;

typedef std::map<int, CONT_SET_DATA> CONT_DEFENCE_SKILL;
typedef std::map<int, int> CONT_TEAM_POINT;
typedef std::map<int, int> CONT_DEFENCE_PARTY_LIST;

class PgDefenceMissionSkill
{
public:
	PgDefenceMissionSkill();
	~PgDefenceMissionSkill();

	void Clear();
	void Set(CONT_DEFENCE_SKILL & kCont);
	void Get(CONT_DEFENCE_SKILL& kCont)const;

	bool IsLearn(int const iTeam, int const iSkillNo)const;
	int GetNextSkillNo(int const iTeam, int const iSkillNo)const;
	//bool HasNextSkill(int const iSkillNo, int & iNextNo)const;
	static int GetGuardianSkillPoint(int const iGuardianNo, int const iSkillNo);
	static bool IsGuardianSkill(int const iSkillNo);

	bool SetSkillInsert(int const iTeam, int const iSkillNo);

private:
	static int _GetGuardianSkillPoint(int const iSkillNo);
	static int _GetGuardianSkillPoint(int const iGuardianNo, int const iSkillNo);

	CONT_DEFENCE_SKILL m_kContDefenceSkill;
};

#endif // WEAPON_VARIANT_USERINFO_PGMISSIONINFO_H