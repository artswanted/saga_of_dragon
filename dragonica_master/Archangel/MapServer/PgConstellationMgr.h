#ifndef MAP_MAPSERVER_MAP_PGCONSTELLATIONMGR_H
#define MAP_MAPSERVER_MAP_PGCONSTELLATIONMGR_H

#include "Variant/PgConstellation.h"

class PgIndun;

class PgConstellationPartyMgr
{
public:
	PgConstellationPartyMgr();

	void Clear();

	void AddMission(Constellation::SConstellationKey const& constellationKey
				  , Constellation::SConstellationEnter const& constellationEnter
				  , Constellation::CONT_MAP_MISSION & dungeonMission
				  , Constellation::CONT_MAP_MISSION & normalMission
				  , Constellation::CONT_MAP_MISSION & bossMission);

	bool AddParty(Constellation::SConstellationKey const& constellationKey);
	bool AddParty(Constellation::SConstellationMission const& constellationMission);
	bool DelParty(BM::GUID const& PartyGuid);

	bool ModifyMissionState(BM::GUID const& PartyGuid
						  , std::wstring const& MissionType
						  , int const MissionNo
						  , BYTE const MissionState);

	int CheckMissionType(int const GroundNo
					   , Constellation::SConstellationKey const& constellationKey
					   , Constellation::EMISSION_TYPE const Type, int const Value
					   , std::wstring & MissionType)const;

	bool GetMissionInfo(BM::GUID const& PartyGuid, Constellation::SConstellationMission & constellationMission)const;
	bool GetMissionInfo(BM::GUID const& PartyGuid, bool const bBossMap, BM::Stream & Packet)const;
	bool IsMissionInfo(BM::GUID const& PartyGuid)const;

	bool CreateMissionInfo(Constellation::SConstellationKey const& constellationKey
						 , Constellation::SConstellationMission & constellationMission);

	HRESULT RecvUnitDie(CUnit * pUnit, PgIndun * pkGround);

private:
	HRESULT RecvUnitDie_Player_Action(CUnit * pUnit, PgIndun * pkGround);
	HRESULT RecvUnitDie_Monster_Action(CUnit * pUnit, PgIndun * pkGround);
	HRESULT RecvUnitDie_Summoned_Action(CUnit * pUnit, PgIndun * pkGround);
	void SendToParty_MissionState(BM::GUID const& PartyGuid, PgIndun * pkGround, std::wstring const& MissionType, int const MissionNo, Constellation::MissionState const State);

private:
	mutable BM::ACE_RW_Thread_Mutex_Ext m_kMutex;
	Constellation::CONT_CONSTELLATION_PARTY m_PartyList;
};

bool GetConstellationMission(Constellation::SConstellationKey const& constellationKey, Constellation::SConstellationMission & constellationMission);

#define g_ConstellationPartyMgr SINGLETON_STATIC(PgConstellationPartyMgr)

#endif