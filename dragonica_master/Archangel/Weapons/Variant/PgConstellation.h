#ifndef WEAPON_VARIANT_CONSTELLATION_PGCONSTELLATION_H
#define WEAPON_VARIANT_CONSTELLATION_PGCONSTELLATION_H

#include "PgStringUtil.h"
#include "BM/Stream.h"
#include "Lohengrin/dbtables.h"

namespace Constellation
{
	std::wstring const MISSION_DUNGEON = L"DUNGEON_MISSION";
	std::wstring const MISSION_NORMAL = L"NORMAL_MISSION";
	std::wstring const MISSION_BOSS = L"BOSS_MISSION";

	typedef enum eCONSTELLATION_ENTER_RESULT : DWORD
	{
		ECE_OK						= 0x0000,
		ECE_ERROR_LV_MIN			= 0x0001, //최소 레벨 제한에 걸렸을 때
		ECE_ERROR_LV_MAX			= 0x0002, //최대 레벨 제한에 걸렸을 때
		ECE_ERROR_PARTYMASTER		= 0x0004, //파티장이 아닐 때
		ECE_ERROR_DAY				= 0x0008, //요일에 걸렸을 때
		ECE_ERROR_QUEST				= 0x0010, //퀘스트 필요 조건에 걸렸을 때
		ECE_ERROR_ITEM				= 0x0020, //아이템 필요 조건에 걸렸을 때
		ECE_ERROR_NO_PARTY			= 0x0040, //파티가 없을 때
		ECE_ERROR_MAPMOVE			= 0x0080, //맵이동을 실패했을 떄
		ECE_ERROR_MGR_ADD_USER		= 0x0100, //매니져에 유저등록을 실패했을 때
		ECE_ERROR_MGR_ADD_PARTY		= 0x0200, //매니져에 파티등록을 실패했을 때
		ECE_ERROR_CASH_ITEM			= 0x0400, //캐시아이템을 사용하려고 했는데 없을 때
		ECE_ERROR_SYSTEM			= 0x8000, //시스템 내부적 에러
	}ECE_RESULT;

	typedef enum eMapType
	{
		EMAP_NONE = 0,
		EMAP_NORMAL,
		EMAP_BOSS,
	}EMAP_TYPE;

	typedef enum eMissionType
	{
		EMIS_NONE = 0,
		EMIS_PROTECT_NPC,
		EMIS_LEADER_ALIVE,
		EMIS_KILL_MONSTER,
		EMIS_GET_ITEM,
	}EMISSION_TYPE;

	struct SCheck_Bonus_Cash_item
	{
		SCheck_Bonus_Cash_item();
		SCheck_Bonus_Cash_item(SCheck_Bonus_Cash_item const& rhs);

		void Clear();

		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem);

		int WorldMapNo;
	
		VEC_INT VecItemNo;
		VEC_INT VecItemCount;
	};
	
	struct SCheck_Entrance_Cash_item
	{
		SCheck_Entrance_Cash_item();
		SCheck_Entrance_Cash_item(SCheck_Entrance_Cash_item const& rhs);

		void Clear();

		bool ParseXml(TiXmlElement const* pNode);

		int WorldMapNo;
		VEC_INT ItemNo;
	};

	struct SCondition
	{
		SCondition();
		SCondition(SCondition const& rhs);

		void Clear();

		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEF_QUEST_REWARD const* pDefQuestReward);

		int ItemNo;
		int ItemCount;
		int QuestNo;
		int MinLevel;
		int MaxLevel;
		VEC_WSTRING DayofWeek;
	};

	struct SReward
	{
		SReward();
		SReward(SReward const& rhs);

		void Clear();

		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem);

		int ItemNo;
		int ItemCount;
		int BagGroupNo;
	};

	struct SMapMission
	{
		SMapMission();
		SMapMission(SMapMission const& rhs);
		
		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster);

		int MissionNo;
		int MissionTextNo;
		EMISSION_TYPE Type;
		int Value01;
	};

	typedef std::vector<SMapMission> CONT_MAP_MISSION;
	struct SMap
	{
		SMap();
		SMap(SMap const& rhs);

		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster);

		int ID;
		EMAP_TYPE MapType;
		CONT_MAP_MISSION ContMapMission;
	};

	typedef std::vector<SMap> CONT_MAP;
	struct SMapGroup
	{
		SMapGroup();
		SMapGroup(SMapGroup const& rhs);

		void Clear();

		bool ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster);
		
		CONT_MAP ContMap;
	};

	struct SConstellationEnter
	{
		SConstellationEnter();
		SConstellationEnter(SConstellationEnter const& rhs);

		bool ParseXml(TiXmlElement const* pNode
					, CONT_DEFITEM const* pDefItem
					, CONT_DEFMONSTER const* pDefMonster
					, CONT_DEF_QUEST_REWARD const* pDefQuestReward);

		int Position;
		int Difficulty;
		int Name;
		int Description;
		VEC_INT MonsterNo;
		bool Enable;
		SCondition Condition;
		SReward	Reward;
		SMapGroup MapGroup;
		CONT_MAP_MISSION DungeonMission;
	};

	typedef tagPairKey<int, int> ConstellationKey;
	typedef std::map< ConstellationKey, SConstellationEnter > CONT_CONSTELLATION_ENTER;	// first : Position, Difficulty
	typedef std::map< int, CONT_CONSTELLATION_ENTER > CONT_CONSTELLATION;					// first : MapNo
	typedef std::map<int,SCheck_Bonus_Cash_item> CONT_CONSTELLATION_BONUS_ITEM;

	typedef std::vector<VEC_WSTRING> CONT_VEC_BOUNS_CASH_ITEM_NO;

	typedef std::map<int, SCheck_Entrance_Cash_item> CONT_CONSTELLATION_ENTRANCE_CASH_ITEM;
////// 던전임무 매니저 관련 ///////////////////////////////////////////////////////////////////////////
	enum MissionState : BYTE
	{
		MS_NONE = 0,
		MS_FAIL = 1,
		MS_CLEAR = 2,
		MS_MAX
	};

	struct SMapMission_Extend
	{
		SMapMission_Extend();
		SMapMission_Extend(BYTE const byState, SMapMission const& mission);
		SMapMission_Extend(SMapMission_Extend const& rhs);

		bool SetState(BYTE const MissionState);

		BYTE State;				// 현재 임무 상태
		SMapMission Mission;	// 맵 임무 내용
	};

	typedef std::vector<SMapMission_Extend> CONT_MAP_MISSION_EX;

	struct SConstellationKey
	{
		SConstellationKey();
		SConstellationKey(SConstellationKey const& rhs);
		SConstellationKey(BM::GUID const& partyGuid, ConstellationKey const& key
						, int const worldGndNo, int const normalGndNo, int const bossGndNo);
		SConstellationKey& operator = (SConstellationKey const & rhs);

		void Clear();

		void ReadFromPacket(BM::Stream & rPacket);
		void WriteToPacket(BM::Stream & rPacket) const;

		BM::GUID PartyGuid;
		int WorldGndNo;
		int NormalGndNo;
		int BossGndNo;
		ConstellationKey Key;
	};

	struct SConstellationMission
	{
		SConstellationMission();
		SConstellationMission(SConstellationMission const& rhs);
		SConstellationMission(CONT_MAP_MISSION const& dungeonMission, CONT_MAP_MISSION const& normalMission
			, CONT_MAP_MISSION const& bossMission, SConstellationKey const& constellationKey);
		SConstellationMission& operator = (SConstellationMission const & rhs);

		void Clear();
		bool ModifyMissionState(std::wstring const& MissionType, int const MissionNo, BYTE const MissionState);	// 미션 상태 변경

		bool SetDungeonMissionState(int const MissionNo, BYTE const MissionState);	// 던전 임무 상태 변경
		bool SetNormalMissionState(int const MissionNo, BYTE const MissionState);	// 일반맵 임무 상태 변경
		bool SetBossMissionState(int const MissionNo, BYTE const MissionState);		// 보스맵 임무 상태 변경

		int CheckMissionType_Dungeon(EMISSION_TYPE const MissionType, int const Value)const;
		int CheckMissionType_NormalMap(EMISSION_TYPE const MissionType, int const Value)const;
		int CheckMissionType_BossMap(EMISSION_TYPE const MissionType, int const Value)const;

		CONT_MAP_MISSION_EX		DungeonMission;		// 던전 임무
		CONT_MAP_MISSION_EX		NormalMapMission;	// 일반맵 임무
		CONT_MAP_MISSION_EX		BossMapMission;		// 보스맵 임무

		SConstellationKey ConstellationKey;			// 임무 정보에 접근하기 위한 키값들

		void ReadFromPacket(BM::Stream & rPacket);
		void WriteToPacket(BM::Stream & rPacket) const;
	};

	typedef std::map<BM::GUID, std::pair<int,SConstellationMission> > CONT_CONSTELLATION_PARTY;	// first : PartyGuid
///////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // WEAPON_VARIANT_CONSTELLATION_PGCONSTELLATION_H