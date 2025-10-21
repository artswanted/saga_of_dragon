#include "stdafx.h"
#include "PgConstellationMgr.h"
#include "PgGroundMgr.h"

using namespace Constellation;

// PgConstellationPartyMgr
PgConstellationPartyMgr::PgConstellationPartyMgr()
{
}

void PgConstellationPartyMgr::Clear()
{
	m_PartyList.clear();
}

void PgConstellationPartyMgr::AddMission(SConstellationKey const& constellationKey
									   , SConstellationEnter const& constellationEnter
									   , CONT_MAP_MISSION & dungeonMission
									   , CONT_MAP_MISSION & normalMission
									   , CONT_MAP_MISSION & bossMission)
{
	if( false == constellationEnter.DungeonMission.empty() )
	{// 던전 미션
		dungeonMission.insert(dungeonMission.begin(), constellationEnter.DungeonMission.begin(), constellationEnter.DungeonMission.end());
	}

	Constellation::CONT_MAP::const_iterator map_iter = constellationEnter.MapGroup.ContMap.begin();
	while( constellationEnter.MapGroup.ContMap.end() != map_iter )
	{
		if( false == map_iter->ContMapMission.empty() )
		{
			if( (constellationKey.NormalGndNo == map_iter->ID) && (EMAP_TYPE::EMAP_NORMAL == map_iter->MapType) )
			{// 일반맵 미션
				normalMission.insert(normalMission.begin(), map_iter->ContMapMission.begin(), map_iter->ContMapMission.end());
			}
			else if( (constellationKey.BossGndNo == map_iter->ID) && (EMAP_TYPE::EMAP_BOSS == map_iter->MapType) )
			{// 보스방 미션
				bossMission.insert(bossMission.begin(), map_iter->ContMapMission.begin(), map_iter->ContMapMission.end());
			}
		}
		++map_iter;
	}
}

bool PgConstellationPartyMgr::AddParty(SConstellationKey const& constellationKey)
{
	if( BM::GUID::NullData() == constellationKey.PartyGuid )
	{
		return false;
	}

	BM::CAutoMutex kWLock(m_kMutex, true);
	CONT_CONSTELLATION_PARTY::iterator party_iter = m_PartyList.find(constellationKey.PartyGuid);
	if( m_PartyList.end() != party_iter )
	{
		return false;
	}

	CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(constellationKey.WorldGndNo);
	if( ContConstellation.end() == iter )
	{
		return false;
	}

	CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.find(constellationKey.Key);
	if( iter->second.end() == sub_iter )
	{
		return false;
	}

	CONT_MAP_MISSION dungeonMission, normalMission, bossMission;
	AddMission(constellationKey, sub_iter->second, dungeonMission, normalMission, bossMission);

	SConstellationMission constellationMission(dungeonMission, normalMission, bossMission, constellationKey);
	m_PartyList.insert( std::make_pair(constellationKey.PartyGuid, std::make_pair(1,constellationMission)) );
	return true;
}

bool PgConstellationPartyMgr::AddParty(SConstellationMission const& constellationMission)
{
	BM::CAutoMutex kWLock(m_kMutex, true);
	auto ret = m_PartyList.insert( std::make_pair(constellationMission.ConstellationKey.PartyGuid, std::make_pair(1,constellationMission)) );
	if( false == ret.second )
	{
		++(ret.first->second.first);
		ret.first->second.second = constellationMission;
	}

	return true;
}

bool PgConstellationPartyMgr::DelParty(BM::GUID const& PartyGuid)
{
	BM::CAutoMutex kWLock(m_kMutex, true);
	CONT_CONSTELLATION_PARTY::iterator iter = m_PartyList.find(PartyGuid);
	if( m_PartyList.end() == iter )
	{
		return false;
	}

	--(iter->second.first);
	if(0 == iter->second.first)
	{
		m_PartyList.erase(iter);
	}
	return true;
}

bool PgConstellationPartyMgr::ModifyMissionState(BM::GUID const& PartyGuid
											   , std::wstring const& MissionType
											   , int const MissionNo
											   , BYTE const MissionState)
{
	BM::CAutoMutex kWLock(m_kMutex, true);
	CONT_CONSTELLATION_PARTY::iterator party_iter = m_PartyList.find(PartyGuid);
	if( m_PartyList.end() == party_iter )
	{
		return false;
	}

	CONT_CONSTELLATION_PARTY::mapped_type::second_type & rkParty = party_iter->second.second;

	if( rkParty.ModifyMissionState(MissionType, MissionNo, MissionState) )
	{
		return true;
	}
	return false;
}

int PgConstellationPartyMgr::CheckMissionType(int const GroundNo
											, SConstellationKey const& constellationKey
											, EMISSION_TYPE const Type
											, int const Value
											, std::wstring & MissionType)const
{
	BM::CAutoMutex kRLock(m_kMutex, false);
	CONT_CONSTELLATION_PARTY::const_iterator party_iter = m_PartyList.find(constellationKey.PartyGuid);
	if( m_PartyList.end() == party_iter )
	{
		return false;
	}

	CONT_CONSTELLATION_PARTY::mapped_type::second_type const& rkParty = party_iter->second.second;

	int MissionNo = 0;
	if( GroundNo == constellationKey.NormalGndNo )
	{
		MissionNo = rkParty.CheckMissionType_NormalMap(Type, Value);
		if( MissionNo )
		{
			MissionType = MISSION_NORMAL;
			return MissionNo;
		}
	}
	else if( GroundNo == constellationKey.BossGndNo )
	{
		MissionNo = rkParty.CheckMissionType_BossMap(Type, Value);
		if( MissionNo )
		{
			MissionType = MISSION_BOSS;
			return MissionNo;
		}
	}

	MissionNo = rkParty.CheckMissionType_Dungeon(Type, Value);
	if( MissionNo )
	{
		MissionType = MISSION_DUNGEON;
	}
	return MissionNo;
}

bool PgConstellationPartyMgr::GetMissionInfo(BM::GUID const& PartyGuid, SConstellationMission & constellationMission)const
{
	BM::CAutoMutex kRLock(m_kMutex, false);
	CONT_CONSTELLATION_PARTY::const_iterator party_iter = m_PartyList.find(PartyGuid);
	if( m_PartyList.end() == party_iter )
	{
		return false;
	}

	CONT_CONSTELLATION_PARTY::mapped_type::second_type const& rkParty = party_iter->second.second;

	constellationMission = rkParty;
	return true;
}

bool PgConstellationPartyMgr::GetMissionInfo(BM::GUID const& PartyGuid, bool const bBossMap, BM::Stream & Packet)const
{
	BM::CAutoMutex kRLock(m_kMutex, false);
	CONT_CONSTELLATION_PARTY::const_iterator party_iter = m_PartyList.find(PartyGuid);
	if( m_PartyList.end() == party_iter )
	{
		return false;
	}

	CONT_CONSTELLATION_PARTY::mapped_type::second_type const& rkParty = party_iter->second.second;

	Packet.Push(bBossMap);
	PU::TWriteArray_A(Packet, rkParty.DungeonMission);
	PU::TWriteArray_A(Packet, rkParty.NormalMapMission);
	if( bBossMap )
	{
		PU::TWriteArray_A(Packet, rkParty.BossMapMission);
	}
	rkParty.ConstellationKey.WriteToPacket(Packet);
	return true;
}

bool PgConstellationPartyMgr::IsMissionInfo(BM::GUID const& PartyGuid)const
{
	BM::CAutoMutex kRLock(m_kMutex, false);
	CONT_CONSTELLATION_PARTY::const_iterator party_iter = m_PartyList.find(PartyGuid);
	return ( m_PartyList.end() != party_iter );
}

bool PgConstellationPartyMgr::CreateMissionInfo(SConstellationKey const& constellationKey
											  , SConstellationMission & constellationMission)
{
	if( BM::GUID::NullData() == constellationKey.PartyGuid )
	{
		return false;
	}

	if(IsMissionInfo(constellationKey.PartyGuid))
	{
		return false;
	}

	CONT_CONSTELLATION const& ContConstellation = g_kGndMgr.m_kGndRscMgr.GetConstellation();
	CONT_CONSTELLATION::const_iterator iter = ContConstellation.find(constellationKey.WorldGndNo);
	if( ContConstellation.end() == iter )
	{
		return false;
	}

	CONT_CONSTELLATION_ENTER::const_iterator sub_iter = iter->second.find(constellationKey.Key);
	if( iter->second.end() == sub_iter )
	{
		return false;
	}

	CONT_MAP_MISSION dungeonMission, normalMission, bossMission;
	AddMission(constellationKey, sub_iter->second, dungeonMission, normalMission, bossMission);

	SConstellationMission Mission(dungeonMission, normalMission, bossMission, constellationKey);
	constellationMission = Mission;
	return true;
}

HRESULT PgConstellationPartyMgr::RecvUnitDie(CUnit * pkUnit, PgIndun * pkGround)
{
	if( NULL == pkUnit )
	{
		return E_FAIL;
	}

	HRESULT hResult = S_OK;
	switch(pkUnit->UnitType())
	{
	case UT_PLAYER:
		{// 파티장이 죽으면 미션 실패
			hResult = RecvUnitDie_Player_Action(pkUnit, pkGround);
		}break;
	case UT_BOSSMONSTER:
	case UT_MONSTER:
		{// 몬스터가 죽으면 미션 성공
			hResult = RecvUnitDie_Monster_Action(pkUnit, pkGround);
		}break;
	case UT_SUMMONED:
		{// 소환체가 죽으면 미션 실패
			hResult = RecvUnitDie_Summoned_Action(pkUnit, pkGround);
		}break;
	default:
		{
		}break;
	}
	return hResult;
}

HRESULT PgConstellationPartyMgr::RecvUnitDie_Player_Action(CUnit * pUnit, PgIndun * pkGround)
{
	Constellation::SConstellationKey const& kConstellationKey = pkGround->GetConstellationKey();
	if( kConstellationKey.PartyGuid.IsNull() )
	{
		return E_NOT_JOINED_PARTY;
	}
	

	BM::GUID MasterGuid;
	if( false == pkGround->GetPartyMasterGuid( kConstellationKey.PartyGuid, MasterGuid ) )
	{
		return E_NOT_FIND_PARTY_MASTER;
	}
	
	if( pUnit->GetID() != MasterGuid )
	{// LEADER_ALIVE 타입 미션 체크
		return E_MIS_MATCH_DATA;
	}

	std::wstring MissionType;
	int MissionNo = CheckMissionType(pkGround->GetGroundNo(), kConstellationKey, EMIS_LEADER_ALIVE, 0, MissionType);
	if( 0 == MissionNo )
	{
		return E_NOT_FIND_MISSION;
	}

	if( ModifyMissionState(kConstellationKey.PartyGuid, MissionType, MissionNo, MS_FAIL) )
	{// 클라이언트에 미션 실패 알림
		SendToParty_MissionState(kConstellationKey.PartyGuid, pkGround, MissionType, MissionNo, MS_FAIL);
	}
	return E_INDUN_MISSION_FAIL;
}

HRESULT PgConstellationPartyMgr::RecvUnitDie_Monster_Action(CUnit * pUnit, PgIndun * pkGround)
{
	Constellation::SConstellationKey const& kConstellationKey = pkGround->GetConstellationKey();

	std::wstring MissionType;
	int MissionNo = CheckMissionType(pkGround->GetGroundNo(), kConstellationKey, EMIS_KILL_MONSTER, pUnit->GetAbil(AT_CLASS), MissionType);
	if( 0 == MissionNo )
	{
		return E_NOT_FIND_MISSION;
	}
	
	if( ModifyMissionState(kConstellationKey.PartyGuid, MissionType, MissionNo, MS_CLEAR) )
	{// 클라이언트에 미션 성공 알림
		SendToParty_MissionState(kConstellationKey.PartyGuid, pkGround, MissionType, MissionNo, MS_CLEAR);
	}
	return E_INDUN_MISSION_SUCCESS;
}

HRESULT PgConstellationPartyMgr::RecvUnitDie_Summoned_Action(CUnit * pUnit, PgIndun * pkGround)
{
	Constellation::SConstellationKey const& kConstellationKey = pkGround->GetConstellationKey();

	std::wstring MissionType;
	int MissionNo = CheckMissionType(pkGround->GetGroundNo(), kConstellationKey, EMIS_PROTECT_NPC, pUnit->GetAbil(AT_CLASS), MissionType);
	if( 0 == MissionNo )
	{
		return E_NOT_FIND_MISSION;
	}

	if( ModifyMissionState(kConstellationKey.PartyGuid, MissionType, MissionNo, MS_FAIL) )
	{// 클라이언트에 미션 실패 알림
		SendToParty_MissionState(kConstellationKey.PartyGuid, pkGround, MissionType, MissionNo, MS_FAIL);
	}
	return E_INDUN_MISSION_FAIL;
}

void PgConstellationPartyMgr::SendToParty_MissionState(BM::GUID const& PartyGuid, PgIndun * pkGround, std::wstring const& MissionType, int const MissionNo, Constellation::MissionState const State)
{
	BM::Stream Packet(PT_M_C_NFY_CONSTELLATION_MODIFY_MISSION);
	Packet.Push(MissionType);
	Packet.Push(MissionNo);
	Packet.Push(State);

	pkGround->SendToParty(PartyGuid, Packet);
}

bool GetConstellationMission(SConstellationKey const& constellationKey, SConstellationMission & constellationMission)
{
	if( BM::GUID::NullData() != constellationKey.PartyGuid )
	{
		if( false == g_ConstellationPartyMgr.GetMissionInfo(constellationKey.PartyGuid, constellationMission) )
		{
			if( false == g_ConstellationPartyMgr.CreateMissionInfo(constellationKey, constellationMission) )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}
	return true;
}