#include "stdafx.h"
#include "PgConstellation.h"

#include <string.h>
#include "Tinyxml/Tinyxml.h"
#include "BM/LocalMgr.h"
#include "Variant/tabledatamanager.h"

namespace Constellation
{
	SCheck_Bonus_Cash_item::SCheck_Bonus_Cash_item()
		:WorldMapNo(0)
	{
		VecItemNo.clear();
		VecItemCount.clear();
	}
	SCheck_Bonus_Cash_item::SCheck_Bonus_Cash_item(SCheck_Bonus_Cash_item const& rhs)
		:WorldMapNo(rhs.WorldMapNo)
	{
		VecItemNo.clear();
		VEC_INT::const_iterator iter_ItemNo = rhs.VecItemNo.begin();
		while( rhs.VecItemNo.end() != iter_ItemNo )
		{
			VecItemNo.push_back(*iter_ItemNo);
			++iter_ItemNo;
		}

		VecItemCount.clear();
		VEC_INT::const_iterator iter = rhs.VecItemCount.begin();
		while( rhs.VecItemCount.end() != iter )
		{
			VecItemCount.push_back(*iter);
			++iter;
		}
	}

	void SCheck_Bonus_Cash_item::Clear()
	{
		WorldMapNo = 0;
		VecItemNo.clear();
		VecItemCount.clear();
	}

	bool SCheck_Bonus_Cash_item::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem)
	{//tls
		if( NULL == pNode || NULL == pDefItem)
		{
			return false;
		}
		bool bRet = true;

		WorldMapNo = PgStringUtil::SafeAtoi(pNode->Attribute("MAPNO"));

		std::wstring ItemStr = PgStringUtil::SafeUni(pNode->Attribute("ITEM_NO"));
		if( !ItemStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(ItemStr, wStrVec, _T("/"));
			VEC_WSTRING::const_iterator iter_ItemNo = wStrVec.begin();
			while( wStrVec.end() != iter_ItemNo )
			{
				int ItemNo = PgStringUtil::SafeAtoi(*iter_ItemNo);
				if( ItemNo > 0 )
				{
					VecItemNo.push_back(ItemNo);
				}
				++iter_ItemNo;
			}
		}

		std::wstring ItemCountStr = PgStringUtil::SafeUni(pNode->Attribute("ITEM_COUNT"));
		if( !ItemCountStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(ItemCountStr, wStrVec, _T("/"));
			VEC_WSTRING::const_iterator iter = wStrVec.begin();
			while( wStrVec.end() != iter )
			{
				int ItemNo = PgStringUtil::SafeAtoi(*iter);
				if( ItemNo > 0 )
				{
					VecItemCount.push_back(ItemNo);
				}
				++iter;
			}
		}
		return bRet;
	}

	// SCheck_Entrance_Cash_item
	SCheck_Entrance_Cash_item::SCheck_Entrance_Cash_item()
		: WorldMapNo(0)
	{
		ItemNo.clear();
	}

	SCheck_Entrance_Cash_item::SCheck_Entrance_Cash_item(SCheck_Entrance_Cash_item const& rhs)
		: WorldMapNo(rhs.WorldMapNo)
	{
		ItemNo.clear();
		ItemNo.resize((int)(rhs.ItemNo.size()));
		std::copy( rhs.ItemNo.begin(), rhs.ItemNo.end(), ItemNo.begin() );
	}

	void SCheck_Entrance_Cash_item::Clear()
	{
		WorldMapNo = 0;
		ItemNo.clear();
	}

	bool SCheck_Entrance_Cash_item::ParseXml(TiXmlElement const* pNode)
	{
		if( NULL == pNode )
		{
			return false;
		}
		bool bRet = true;

		Clear();

		WorldMapNo = PgStringUtil::SafeAtoi(pNode->Attribute("MAPNO"));

		std::wstring ItemStr = PgStringUtil::SafeUni(pNode->Attribute("ITEM_NO"));
		if( !ItemStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(ItemStr, wStrVec, _T("/"));
			VEC_WSTRING::const_iterator iter = wStrVec.begin();
			while( wStrVec.end() != iter )
			{
				int CashItemNo = PgStringUtil::SafeAtoi(*iter);
				if( CashItemNo > 0 )
				{
					ItemNo.push_back(CashItemNo);
				}
				++iter;
			}
		}
		return bRet;
	}

	// SCondition
	SCondition::SCondition()
		: ItemNo(0), ItemCount(0), QuestNo(0), MinLevel(0), MaxLevel(0)
	{
		DayofWeek.clear();
	}

	SCondition::SCondition(SCondition const& rhs)
		: ItemNo(rhs.ItemNo), ItemCount(rhs.ItemCount), QuestNo(rhs.QuestNo)
		, MinLevel(rhs.MinLevel), MaxLevel(rhs.MaxLevel)
	{
		DayofWeek.clear();
		VEC_WSTRING::const_iterator iter = rhs.DayofWeek.begin();
		while( rhs.DayofWeek.end() != iter )
		{
			DayofWeek.push_back(*iter);
			++iter;
		}
	}

	void SCondition::Clear()
	{
		ItemNo = 0;
		ItemCount = 0;
		QuestNo = 0;
		MinLevel = 0;
		MaxLevel = 0;
		DayofWeek.clear();
	}

	bool SCondition::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEF_QUEST_REWARD const* pDefQuestReward)
	{
		if( NULL == pNode || NULL == pDefItem || NULL == pDefQuestReward )
		{
			return false;
		}
		bool bRet = true;

		std::wstring ItemStr = PgStringUtil::SafeUni(pNode->Attribute("ITEM"));
		if( !ItemStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(ItemStr, wStrVec, _T("/"));
			if( 2 == wStrVec.size() )
			{
				int const itemNo = PgStringUtil::SafeAtoi(*wStrVec.begin());
				int const itemCount = PgStringUtil::SafeAtoi(*wStrVec.rbegin());

				if( itemNo )
				{
					bool bSubRet = false;
					CONT_DEFITEM::const_iterator item_iter = pDefItem->find(itemNo);
					if( pDefItem->end() != item_iter )
					{
						ItemNo = itemNo;
						ItemCount = itemCount;
						bSubRet = true;
					}
					bRet |= bSubRet;
				}
			}
		}

		QuestNo = PgStringUtil::SafeAtoi(pNode->Attribute("QUEST"));
		if( QuestNo )
		{
			CONT_DEF_QUEST_REWARD::const_iterator quest_iter = pDefQuestReward->find(QuestNo);
			if( pDefQuestReward->end() == quest_iter )
			{
				QuestNo = 0;
				bRet = false;
			}
		}

		std::wstring LevelStr = PgStringUtil::SafeUni(pNode->Attribute("LEVEL"));
		if( !LevelStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(LevelStr, wStrVec, _T("/"));
			if( 2 == wStrVec.size() )
			{
				MinLevel = PgStringUtil::SafeAtoi(*wStrVec.begin());
				MaxLevel = PgStringUtil::SafeAtoi(*wStrVec.rbegin());
			}
			else
			{
				bRet = false;
			}
		}

		std::wstring DayStr = PgStringUtil::SafeUni(pNode->Attribute("DAYOFWEEK"));
		if( !DayStr.empty() )
		{
			PgStringUtil::BreakSep(DayStr, DayofWeek, _T("/"));
		}
		else
		{
			bRet = false;
		}

		return bRet;
	}

	// SReward
	SReward::SReward()
		: ItemNo(0), ItemCount(0), BagGroupNo(0)
	{
	}

	SReward::SReward(SReward const& rhs)
		: ItemNo(rhs.ItemNo), ItemCount(rhs.ItemCount), BagGroupNo(rhs.BagGroupNo)
	{
	}

	void SReward::Clear()
	{
		ItemNo = 0;
		ItemCount = 0;
		BagGroupNo = 0;
	}

	bool SReward::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem)
	{
		if( NULL == pNode || NULL == pDefItem )
		{
			return false;
		}
		bool bRet = true;

		std::wstring ItemStr = PgStringUtil::SafeUni(pNode->Attribute("ITEM"));
		if( !ItemStr.empty() )
		{
			VEC_WSTRING wStrVec;
			PgStringUtil::BreakSep(ItemStr, wStrVec, _T("/"));
			if( 2 == wStrVec.size() )
			{
				int const itemNo = PgStringUtil::SafeAtoi(*wStrVec.begin());
				int const itemCount = PgStringUtil::SafeAtoi(*wStrVec.rbegin());

				if( itemNo )
				{
					bool bSubRet = false;
					CONT_DEFITEM::const_iterator item_iter = pDefItem->find(itemNo);
					if( pDefItem->end() != item_iter )
					{
						ItemNo = itemNo;
						ItemCount = itemCount;
						bSubRet = true;
					}
					bRet |= bSubRet;
				}
			}
		}

		BagGroupNo = PgStringUtil::SafeAtoi(pNode->Attribute("BAGGROUP"));

		return bRet;
	}

	// SMapMission
	SMapMission::SMapMission()
		: MissionNo(0), MissionTextNo(0), Type(EMIS_NONE), Value01(0)
	{
	}

	SMapMission::SMapMission(SMapMission const& rhs)
		: MissionNo(rhs.MissionNo), MissionTextNo(rhs.MissionTextNo), Type(rhs.Type), Value01(rhs.Value01)
	{
	}

	bool SMapMission::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster)
	{
		if( NULL == pNode || NULL == pDefItem || NULL == pDefMonster )
		{
			return false;
		}
		bool bRet = true;

		MissionNo = PgStringUtil::SafeAtoi(pNode->Attribute("NO"));
		MissionTextNo = PgStringUtil::SafeAtoi(pNode->Attribute("MISSION_TEXT"));
		
		std::wstring const wType = PgStringUtil::SafeUni(pNode->Attribute("TYPE"));
		if( 0 == BM::vstring("PROTECT_NPC").icmp(wType) )
		{
			Type = EMIS_PROTECT_NPC;
		}
		else if( 0 == BM::vstring("LEADER_ALIVE").icmp(wType) )
		{
			Type = EMIS_LEADER_ALIVE;
		}
		else if( 0 == BM::vstring("KILL_MONSTER").icmp(wType) )
		{
			Type = EMIS_KILL_MONSTER;
		}
		else if( 0 == BM::vstring("GET_ITEM").icmp(wType) )
		{
			Type = EMIS_GET_ITEM;
		}
		else
		{
			bRet = false;
		}

		int const value01 = PgStringUtil::SafeAtoi(pNode->Attribute("VALUE01"));

		switch( Type )
		{
		case EMIS_PROTECT_NPC:
			{
				Value01 = value01;
			}break;
		case EMIS_KILL_MONSTER:
			{
				CONT_DEFMONSTER::const_iterator mon_iter = pDefMonster->find(value01);
				if( pDefMonster->end() != mon_iter )
				{
					Value01 = value01;
				}
				else
				{
					bRet = false;
				}
			}break;
		case EMIS_GET_ITEM:
			{
				CONT_DEFITEM::const_iterator item_iter = pDefItem->find(value01);
				if( pDefItem->end() != item_iter )
				{
					Value01 = value01;
				}
				else
				{
					bRet = false;
				}
			}break;
		default:
			{
				Value01 = value01;
			}break;
		}

		return bRet;
	}

	// SMap
	SMap::SMap()
		: ID(0), MapType(EMAP_NONE)
	{
		ContMapMission.clear();
	}
	
	SMap::SMap(SMap const& rhs)
		: ID(rhs.ID), MapType(rhs.MapType)
	{
		ContMapMission.clear();
		CONT_MAP_MISSION::const_iterator mis_iter = rhs.ContMapMission.begin();
		while( rhs.ContMapMission.end() != mis_iter )
		{
			ContMapMission.push_back(*mis_iter);
			++mis_iter;
		}
	}

	bool SMap::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster)
	{
		if( NULL == pNode || NULL == pDefItem || NULL == pDefMonster )
		{
			return false;
		}
		bool bRet = true;

		ID = PgStringUtil::SafeAtoi(pNode->Attribute("ID"));
		
		std::wstring const wMapType = PgStringUtil::SafeUni(pNode->Attribute("TYPE"));
		if( 0 == BM::vstring("NORMAL").icmp(wMapType) )
		{
			MapType = EMAP_NORMAL;
		}
		else if( 0 == BM::vstring("BOSS").icmp(wMapType) )
		{
			MapType = EMAP_BOSS;
		}
		else
		{
			bRet = false;
		}

		TiXmlElement const* pChild = pNode->FirstChildElement();
		while( pChild )
		{
			char const* szName = pChild->Value();
			if( 0 == _stricmp("MAP_MISSION", szName) )
			{
				CONT_MAP_MISSION::value_type NewElement;
				bRet = NewElement.ParseXml(pChild, pDefItem, pDefMonster) && bRet;
				ContMapMission.push_back(NewElement);
			}
			else
			{
				bRet = false;
			}
			pChild = pChild->NextSiblingElement();
		}
		return bRet;
	}

	// SMapGroup
	SMapGroup::SMapGroup()
	{
		ContMap.clear();
	}
	
	SMapGroup::SMapGroup(SMapGroup const& rhs)
	{
		ContMap.clear();
		CONT_MAP::const_iterator iter = rhs.ContMap.begin();
		while( rhs.ContMap.end() != iter )
		{
			ContMap.push_back(*iter);
			++iter;
		}
	}

	void SMapGroup::Clear()
	{
		ContMap.clear();
	}

	bool SMapGroup::ParseXml(TiXmlElement const* pNode, CONT_DEFITEM const* pDefItem, CONT_DEFMONSTER const* pDefMonster)
	{
		if( NULL == pNode || NULL == pDefItem || NULL == pDefMonster )
		{
			return false;
		}
		bool bRet = true;

		TiXmlElement const* pChild = pNode->FirstChildElement();
		while( pChild )
		{
			char const* szName = pChild->Value();
			if( 0 == _stricmp("MAP", szName) )
			{
				CONT_MAP::value_type NewElement;
				bRet = NewElement.ParseXml(pChild, pDefItem, pDefMonster) && bRet;

				bool bDuplication = false;
				CONT_MAP::const_iterator iter = ContMap.begin();
				while( ContMap.end() != iter )
				{
					if( iter->ID == NewElement.ID )
					{
						bDuplication = true;
						break;
					}
					++iter;
				}
				if( false == bDuplication )
				{
					ContMap.push_back(NewElement);
				}
			}
			else
			{
				bRet = false;
			}
			pChild = pChild->NextSiblingElement();
		}
		return bRet;
	}

	// SConstellationEnter
	SConstellationEnter::SConstellationEnter()
		: Position(0), Difficulty(0), Name(0), Description(0), Enable(false)
	{
		Condition.Clear();
		Reward.Clear();
		MapGroup.Clear();

		MonsterNo.clear();
		DungeonMission.clear();
	}

	SConstellationEnter::SConstellationEnter(SConstellationEnter const& rhs)
		: Position(rhs.Position), Difficulty(rhs.Difficulty), Name(rhs.Name), Description(rhs.Description)
		, Enable(rhs.Enable), Condition(rhs.Condition), Reward(rhs.Reward), MapGroup(rhs.MapGroup)
	{
		MonsterNo.clear();
		VEC_INT::const_iterator mon_iter = rhs.MonsterNo.begin();
		while( rhs.MonsterNo.end() != mon_iter )
		{
			MonsterNo.push_back(*mon_iter);
			++mon_iter;
		}
		
		DungeonMission.clear();
		CONT_MAP_MISSION::const_iterator dun_iter = rhs.DungeonMission.begin();
		while( rhs.DungeonMission.end() != dun_iter )
		{
			DungeonMission.push_back(*dun_iter);
			++dun_iter;
		}
	}

	bool SConstellationEnter::ParseXml(TiXmlElement const* pNode
									 , CONT_DEFITEM const* pDefItem
									 , CONT_DEFMONSTER const* pDefMonster
									 , CONT_DEF_QUEST_REWARD const* pDefQuestReward)
	{
		if( NULL == pNode || NULL == pDefItem || NULL == pDefMonster || NULL == pDefQuestReward )
		{
			return false;
		}
		bool bRet = true;

		Position = PgStringUtil::SafeAtoi(pNode->Attribute("POSITION"));
		Difficulty = PgStringUtil::SafeAtoi(pNode->Attribute("DIFFICULTY"));
		Name = PgStringUtil::SafeAtoi(pNode->Attribute("NAME"));
		Description = PgStringUtil::SafeAtoi(pNode->Attribute("DESCRIPTION"));
		std::wstring Monster = PgStringUtil::SafeUni(pNode->Attribute("MONSTER"));
		Enable = (PgStringUtil::SafeAtoi(pNode->Attribute("ENABLE")) != 0) ? true : false;

		VEC_WSTRING MonsterVec;
		PgStringUtil::BreakSep(Monster, MonsterVec, _T("/"));

		VEC_WSTRING::const_iterator iter = MonsterVec.begin();
		while( MonsterVec.end() != iter )
		{
			int const monsterNo = PgStringUtil::SafeAtoi(*iter);
			CONT_DEFMONSTER::const_iterator mon_iter = pDefMonster->find(monsterNo);
			if( pDefMonster->end() != mon_iter )
			{
				MonsterNo.push_back( monsterNo );
			}
			++iter;
		}

		TiXmlElement const* pChild = pNode->FirstChildElement();
		while( pChild )
		{
			char const* szName = pChild->Value();
			if( 0 == _stricmp("CONDITION", szName) )
			{
				Condition.ParseXml(pChild, pDefItem, pDefQuestReward);
			}
			else if( 0 == _stricmp("REWARD", szName) )
			{
				Reward.ParseXml(pChild, pDefItem) && bRet;
			}
			else if( 0 == _stricmp("MAPGROUP", szName) )
			{
				MapGroup.ParseXml(pChild, pDefItem, pDefMonster);
			}
			else if( 0 == _stricmp("DUNGEON_MISSION", szName) )
			{
				CONT_MAP_MISSION::value_type NewElement;
				NewElement.ParseXml(pChild, pDefItem, pDefMonster);
				DungeonMission.push_back(NewElement);
			}
			else
			{
				bRet = false;
			}
			pChild = pChild->NextSiblingElement();
		}
		return bRet;
	}

////// 던전임무 매니저 관련 ///////////////////////////////////////////////////////////////////////////
	// SMapMission_Extend
	SMapMission_Extend::SMapMission_Extend()
		: State(0), Mission()
	{
	}

	SMapMission_Extend::SMapMission_Extend(BYTE const byState, SMapMission const& mission)
		: State(byState), Mission(mission)
	{
	}

	SMapMission_Extend::SMapMission_Extend(SMapMission_Extend const& rhs)
		: State(rhs.State), Mission(rhs.Mission)
	{
	}

	bool SMapMission_Extend::SetState(BYTE const MissionState)
	{
		if( (MissionState < MS_NONE) || (MissionState >= MS_MAX) )
		{
			return false;
		}

		State = MissionState;
		return true;
	}

	// SConstellationUserInfo
	SConstellationKey::SConstellationKey()
		: WorldGndNo(0), NormalGndNo(0), BossGndNo(0)
	{
		Key.kPriKey = 0;
		Key.kSecKey = 0;
	}

	SConstellationKey::SConstellationKey(SConstellationKey const& rhs)
		: PartyGuid(rhs.PartyGuid), WorldGndNo(rhs.WorldGndNo), NormalGndNo(rhs.NormalGndNo), BossGndNo(rhs.BossGndNo)
	{
		Key.kPriKey = rhs.Key.kPriKey;
		Key.kSecKey = rhs.Key.kSecKey;
	}

	SConstellationKey::SConstellationKey(BM::GUID const& partyGuid, ConstellationKey const& key, int const worldGndNo, int const normalGndNo, int const bossGndNo)
		: PartyGuid(partyGuid), WorldGndNo(worldGndNo), NormalGndNo(normalGndNo), BossGndNo(bossGndNo)
	{
		Key.kPriKey = key.kPriKey;
		Key.kSecKey = key.kSecKey;
	}

	SConstellationKey& SConstellationKey::operator = (SConstellationKey const & rhs)
	{
		PartyGuid = rhs.PartyGuid;
		WorldGndNo = rhs.WorldGndNo;
		NormalGndNo = rhs.NormalGndNo;
		BossGndNo = rhs.BossGndNo;
		Key.kPriKey = rhs.Key.kPriKey;
		Key.kSecKey = rhs.Key.kSecKey;
		return *this;
	}

	void SConstellationKey::Clear()
	{
		PartyGuid.Clear();
		WorldGndNo = 0;
		NormalGndNo = 0;
		BossGndNo = 0;
		Key.kPriKey = 0;
		Key.kSecKey = 0;
	}

	void SConstellationKey::ReadFromPacket(BM::Stream & Packet)
	{
		Packet.Pop(PartyGuid);
		Packet.Pop(WorldGndNo);
		Packet.Pop(NormalGndNo);
		Packet.Pop(BossGndNo);
		Packet.Pop(Key.kPriKey);
		Packet.Pop(Key.kSecKey);
	}

	void SConstellationKey::WriteToPacket(BM::Stream & Packet) const
	{
		Packet.Push(PartyGuid);
		Packet.Push(WorldGndNo);
		Packet.Push(NormalGndNo);
		Packet.Push(BossGndNo);
		Packet.Push(Key.kPriKey);
		Packet.Push(Key.kSecKey);
	}

	// SConstellationMission
	SConstellationMission::SConstellationMission()
	{
	}
	
	SConstellationMission::SConstellationMission(SConstellationMission const& rhs)
		: ConstellationKey(rhs.ConstellationKey)
	{
		DungeonMission.insert(DungeonMission.begin(), rhs.DungeonMission.begin(), rhs.DungeonMission.end());
		NormalMapMission.insert(NormalMapMission.begin(), rhs.NormalMapMission.begin(), rhs.NormalMapMission.end());
		BossMapMission.insert(BossMapMission.begin(), rhs.BossMapMission.begin(), rhs.BossMapMission.end());
	}

	SConstellationMission::SConstellationMission(CONT_MAP_MISSION const& dungeonMission
		, CONT_MAP_MISSION const& normalMission, CONT_MAP_MISSION const& bossMission, SConstellationKey const& constellationKey)
		: ConstellationKey(constellationKey)
	{
		CONT_MAP_MISSION::const_iterator iter = dungeonMission.begin();
		while( dungeonMission.end() != iter )
		{
			SMapMission_Extend NewElement(0, *iter);
			DungeonMission.push_back(NewElement);
			++iter;
		}

		iter = normalMission.begin();
		while( normalMission.end() != iter )
		{
			SMapMission_Extend NewElement(0, *iter);
			NormalMapMission.push_back(NewElement);
			++iter;
		}

		iter = bossMission.begin();
		while( bossMission.end() != iter )
		{
			SMapMission_Extend NewElement(0, *iter);
			BossMapMission.push_back(NewElement);
			++iter;
		}
	}

	SConstellationMission& SConstellationMission::operator = (SConstellationMission const & rhs)
	{
		Clear();
		DungeonMission.insert(DungeonMission.begin(), rhs.DungeonMission.begin(), rhs.DungeonMission.end());
		NormalMapMission.insert(NormalMapMission.begin(), rhs.NormalMapMission.begin(), rhs.NormalMapMission.end());
		BossMapMission.insert(BossMapMission.begin(), rhs.BossMapMission.begin(), rhs.BossMapMission.end());
		ConstellationKey = rhs.ConstellationKey;
		return *this;
	}

	void SConstellationMission::Clear()
	{
		DungeonMission.clear();
		NormalMapMission.clear();
		BossMapMission.clear();
		ConstellationKey.Clear();
	}

	bool SConstellationMission::ModifyMissionState(std::wstring const& MissionType, int const MissionNo, BYTE const MissionState)
	{
		if( MissionType == MISSION_DUNGEON )
		{
			if( SetDungeonMissionState(MissionNo, MissionState) )
			{
				return true;
			}
		}
		else if( MissionType == MISSION_NORMAL )
		{
			if( SetNormalMissionState(MissionNo, MissionState) )
			{
				return true;
			}
		}
		else if( MissionType == MISSION_BOSS )
		{
			if( SetBossMissionState(MissionNo, MissionState) )
			{
				return true;
			}
		}
		return false;
	}

	bool SConstellationMission::SetDungeonMissionState(int const MissionNo, BYTE const MissionState)
	{
		CONT_MAP_MISSION_EX::iterator iter = DungeonMission.begin();
		while( DungeonMission.end() != iter )
		{
			if( MissionNo == iter->Mission.MissionNo )
			{
				if( iter->SetState(MissionState) )
				{
					return true;
				}
			}
			++iter;
		}
		return false;
	}

	bool SConstellationMission::SetNormalMissionState(int const MissionNo, BYTE const MissionState)
	{
		CONT_MAP_MISSION_EX::iterator iter = NormalMapMission.begin();
		while( NormalMapMission.end() != iter )
		{
			if( MissionNo == iter->Mission.MissionNo )
			{
				if( iter->SetState(MissionState) )
				{
					return true;
				}
			}
			++iter;
		}
		return false;
	}

	bool SConstellationMission::SetBossMissionState(int const MissionNo, BYTE const MissionState)
	{
		CONT_MAP_MISSION_EX::iterator iter = BossMapMission.begin();
		while( BossMapMission.end() != iter )
		{
			if( MissionNo == iter->Mission.MissionNo )
			{
				if( iter->SetState(MissionState) )
				{
					return true;
				}
			}
			++iter;
		}
		return false;
	}

	int SConstellationMission::CheckMissionType_Dungeon(EMISSION_TYPE const MissionType, int const Value)const
	{
		CONT_MAP_MISSION_EX::const_iterator iter = DungeonMission.begin();
		while( DungeonMission.end() != iter )
		{
			if( iter->Mission.Type == MissionType
				&& iter->State == MS_NONE )
			{
				switch(MissionType)
				{
				case EMIS_LEADER_ALIVE:
					{
						return iter->Mission.MissionNo;
					}break;
				case EMIS_PROTECT_NPC:
				case EMIS_KILL_MONSTER:
				case EMIS_GET_ITEM:
					{
						if( Value == iter->Mission.Value01 )
						{
							return iter->Mission.MissionNo;
						}
					}break;
				}
			}
			++iter;
		}
		return 0;
	}

	int SConstellationMission::CheckMissionType_NormalMap(EMISSION_TYPE const MissionType, int const Value)const
	{
		CONT_MAP_MISSION_EX::const_iterator iter = NormalMapMission.begin();
		while( NormalMapMission.end() != iter )
		{
			if( iter->Mission.Type == MissionType
				&& iter->State == MS_NONE )
			{
				switch(MissionType)
				{
				case EMIS_LEADER_ALIVE:
					{
						return iter->Mission.MissionNo;
					}break;
				case EMIS_PROTECT_NPC:
				case EMIS_KILL_MONSTER:
				case EMIS_GET_ITEM:
					{
						if( Value == iter->Mission.Value01 )
						{
							return iter->Mission.MissionNo;
						}
					}break;
				}
			}
			++iter;
		}
		return 0;
	}
	
	int SConstellationMission::CheckMissionType_BossMap(EMISSION_TYPE const MissionType, int const Value)const
	{
		CONT_MAP_MISSION_EX::const_iterator iter = BossMapMission.begin();
		while( BossMapMission.end() != iter )
		{
			if( iter->Mission.Type == MissionType
				&& iter->State == MS_NONE )
			{
				switch(MissionType)
				{
				case EMIS_LEADER_ALIVE:
					{
						return iter->Mission.MissionNo;
					}break;
				case EMIS_PROTECT_NPC:
				case EMIS_KILL_MONSTER:
				case EMIS_GET_ITEM:
					{
						if( Value == iter->Mission.Value01 )
						{
							return iter->Mission.MissionNo;
						}
					}break;
				}
			}
			++iter;
		}
		return 0;
	}

	void SConstellationMission::ReadFromPacket(BM::Stream & rPacket)
	{
		bool bIsConstellationMission = false;
		if(rPacket.Pop(bIsConstellationMission) && bIsConstellationMission)
		{
			PU::TLoadArray_A(rPacket, DungeonMission);
			PU::TLoadArray_A(rPacket, NormalMapMission);
			PU::TLoadArray_A(rPacket, BossMapMission);

			ConstellationKey.ReadFromPacket(rPacket);
		}
	}

	void SConstellationMission::WriteToPacket(BM::Stream & rPacket) const
	{
		if(ConstellationKey.PartyGuid.IsNotNull())
		{
			rPacket.Push(true);
			PU::TWriteArray_A(rPacket, DungeonMission);
			PU::TWriteArray_A(rPacket, NormalMapMission);
			PU::TWriteArray_A(rPacket, BossMapMission);

			ConstellationKey.WriteToPacket(rPacket);
		}
		else
		{
			rPacket.Push(false);
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
}