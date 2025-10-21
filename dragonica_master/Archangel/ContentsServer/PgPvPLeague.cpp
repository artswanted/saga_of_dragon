#include "StdAfx.h"
#include "Variant/PgDbCache.h"
#include "Global.h"
#include "Lohengrin/VariableContainer.h"
#include "Lohengrin/GameTime.h"
#include "PgServerSetMgr.h"
#include "PgPvPLeague.h"
#include "Item/PgPostManager.h"

PgPvPGroup::PgPvPGroup(BM::GUID const & rkTournamentGuid, int const iGroupIndex)
{
	Clear();
	m_kTournamentGuid = rkTournamentGuid;
	m_iGroupIndex = iGroupIndex;
}

PgPvPGroup::~PgPvPGroup()
{
	Clear();
}

void PgPvPGroup::Clear()
{
	m_iGroupIndex = 0;
	m_kTournamentGuid.Clear();
	m_kContBattle.clear();
}

bool PgPvPGroup::CreateBattle(int const iTournamentIndex)
{
	if( 1 < m_kContBattle.size() )
	{
		return false;
	}

	BM::GUID kBattleGuid;
	kBattleGuid.Generate();

	CONT_DEF_PVPLEAGUE_BATTLE::mapped_type kElement;
	kElement.kTournamentGuid = m_kTournamentGuid;
	kElement.iGroupIndex = m_iGroupIndex;
	kElement.iTournamentIndex = iTournamentIndex;
	kElement.kWinTeamGuid.Clear();

	if( m_kContBattle.end() != m_kContBattle.find(kBattleGuid) )
	{
		return false;
	}
	
	m_kContBattle.insert( std::make_pair(kBattleGuid, kElement) );

	return true;
}

bool PgPvPGroup::AddBattle(int const iTournamentIndex, TBL_DEF_PVPLEAGUE_BATTLE &rkBattleInfo)
{
	if( 1 < m_kContBattle.size() )
	{
		return false;
	}

	BM::GUID kBattleGuid;
	kBattleGuid.Generate();

	rkBattleInfo.kTournamentGuid = m_kTournamentGuid;
	rkBattleInfo.iGroupIndex = m_iGroupIndex;
	rkBattleInfo.iTournamentIndex = iTournamentIndex;
	rkBattleInfo.kWinTeamGuid.Clear();

	if( m_kContBattle.end() != m_kContBattle.find(kBattleGuid) )
	{
		return false;
	}
	
	m_kContBattle.insert( std::make_pair(kBattleGuid, rkBattleInfo) );
	return true;
}

bool PgPvPGroup::AddBattle(BM::GUID const rkBattleGuid, TBL_DEF_PVPLEAGUE_BATTLE const & rkBattleInfo)
{
	if( m_kContBattle.end() != m_kContBattle.find(rkBattleGuid) )
	{
		return false;
	}
	m_kContBattle.insert( std::make_pair(rkBattleGuid, rkBattleInfo) );
	return true;
}

bool PgPvPGroup::GetNextBattle(TBL_DEF_PVPLEAGUE_BATTLE &rkOutBattleInfo) const
{
	if( 2 != m_kContBattle.size() )
	{
		return false;
	}

	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = m_kContBattle.begin();
	rkOutBattleInfo.kTeamGuid1 = battle_itr->second.kWinTeamGuid; ++battle_itr;
	rkOutBattleInfo.kTeamGuid2 = battle_itr->second.kWinTeamGuid;
	return true;
}

bool PgPvPGroup::IsAllAutoWin() const
{
	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kElement = battle_itr->second;
		if( kElement.kTeamGuid1.IsNotNull() && kElement.kTeamGuid2.IsNotNull() )
		{
			return false;
		}
		++battle_itr;
	}

	return true;
}

bool PgPvPGroup::IsAutoWin(BM::GUID const &rkTeamGuid) const
{
	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kElement = battle_itr->second;
		if( (kElement.kTeamGuid1 == rkTeamGuid && kElement.kTeamGuid2.IsNull())
			|| (kElement.kTeamGuid2 == rkTeamGuid && kElement.kTeamGuid1.IsNull()) )
		{
			return true;
		}
		++battle_itr;
	}

	return false;
}

bool PgPvPGroup::IsEmpty() const
{
	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kElement = battle_itr->second;
		if( kElement.kTeamGuid1.IsNotNull() || kElement.kTeamGuid2.IsNotNull() )
		{
			return false;
		}
		++battle_itr;
	}

	return true;
}

bool PgPvPGroup::Insert(BM::GUID const &rkTeamGuid)
{
	CONT_DEF_PVPLEAGUE_BATTLE::iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type& kElement = battle_itr->second;
		if( kElement.kTeamGuid1.IsNull() )
		{
			kElement.kTeamGuid1 = rkTeamGuid;
			return true;
		}
		else if( kElement.kTeamGuid2.IsNull() )
		{
			kElement.kTeamGuid2 = rkTeamGuid;
			return true;
		}

		++battle_itr;
	}

	return false;
}

void PgPvPGroup::SaveBattle(BM::GUID const & rkLeagueGuid, int const iLeagueLevel)
{
	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kElement = battle_itr->second;
		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_INSERT_PVPLEAGUE_BATTLE, _T("EXEC [dbo].[up_Insert_PvPLeague_Battle]"));
		kQuery.PushStrParam(m_kTournamentGuid);
		kQuery.PushStrParam(battle_itr->first);
		kQuery.PushStrParam(kElement.iTournamentIndex);
		kQuery.PushStrParam(kElement.kTeamGuid1);
		kQuery.PushStrParam(kElement.kTeamGuid2);
		kQuery.PushStrParam(kElement.iGroupIndex);
		kQuery.PushStrParam(rkLeagueGuid);
		kQuery.PushStrParam(iLeagueLevel);
		g_kCoreCenter.PushQuery(kQuery, true);
		++battle_itr;
	}
}

bool PgPvPGroup::Find(BM::GUID const rkBattleGuid)
{
	return (m_kContBattle.end() != m_kContBattle.find(rkBattleGuid));
}

bool PgPvPGroup::SetBattleResult(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, BM::GUID const rkBattleGuid, int const iWinTeam, int const iNextLeagueLevel)
{
	CONT_DEF_PVPLEAGUE_BATTLE::iterator battle_itr = m_kContBattle.find(rkBattleGuid);
	if(m_kContBattle.end() != battle_itr)
	{
		TBL_DEF_PVPLEAGUE_BATTLE &kBattleInfo = battle_itr->second;
		BM::GUID kWinTeamGuid;
		switch( iWinTeam )
		{
		case TEAM_RED:
			{
				kWinTeamGuid = kBattleInfo.kTeamGuid1;
			}break;
		case TEAM_BLUE:
			{
				kWinTeamGuid = kBattleInfo.kTeamGuid2;
			}break;
		default:
			{
				return false;
			}break;
		}
		
		CONT_DEF_PVPLEAGUE_TEAM::iterator team_itr = rkContTeam.find(kWinTeamGuid);
		if( rkContTeam.end() != team_itr )
		{
			team_itr->second.iLeagueLevel = iNextLeagueLevel;
			kBattleInfo.kWinTeamGuid = kWinTeamGuid;

			{//Update Battle Result
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_BATTLE, _T("EXEC [dbo].[up_Set_PvPLeague_Battle]"));
				kQuery.PushStrParam(battle_itr->first);
				kQuery.PushStrParam(iWinTeam);
				g_kCoreCenter.PushQuery(kQuery, true);
			}

			{//Update WinTeam LeagueLevel
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_TEAM, _T("EXEC [dbo].[up_Set_PvPLeague_Team]"));
				kQuery.PushStrParam(team_itr->first);
				kQuery.PushStrParam(iNextLeagueLevel);
				g_kCoreCenter.PushQuery(kQuery, true);
			}
		}
		return true;
	}
	return false;
}

void PgPvPGroup::WriteToBattleInfo(CONT_DEF_PVPLEAGUE_BATTLE & rkContBattle) const
{
	rkContBattle.insert(m_kContBattle.begin(), m_kContBattle.end());
}

void PgPvPGroup::AutoWin(int const iNextLeagueLevel, CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, bool bIsSaveDB)
{
	CONT_DEF_PVPLEAGUE_BATTLE::iterator battle_itr = m_kContBattle.begin();
	while(m_kContBattle.end() != battle_itr)
	{
		TBL_DEF_PVPLEAGUE_BATTLE &kBattleInfo = battle_itr->second;
		int iWinTeam = TEAM_NONE;
		BM::GUID kWinTeamGuid;
		if( kBattleInfo.kTeamGuid1.IsNotNull() && kBattleInfo.kTeamGuid2.IsNull() )
		{
			iWinTeam = TEAM_RED;
			kWinTeamGuid = kBattleInfo.kTeamGuid1;
		}
		else if( kBattleInfo.kTeamGuid2.IsNotNull() && kBattleInfo.kTeamGuid1.IsNull() )
		{
			iWinTeam = TEAM_BLUE;
			kWinTeamGuid = kBattleInfo.kTeamGuid2;
		}

		if( TEAM_NONE != iWinTeam )
		{
			CONT_DEF_PVPLEAGUE_TEAM::iterator team_itr = rkContTeam.find(kWinTeamGuid);
			if( rkContTeam.end() != team_itr )
			{
				team_itr->second.iLeagueLevel = iNextLeagueLevel;
				kBattleInfo.kWinTeamGuid = kWinTeamGuid;

				if( bIsSaveDB )
				{
					{//Update Battle Result
						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_BATTLE, _T("EXEC [dbo].[up_Set_PvPLeague_Battle]"));
						kQuery.PushStrParam(battle_itr->first);
						kQuery.PushStrParam(iWinTeam);
						g_kCoreCenter.PushQuery(kQuery, true);
					}

					{//Update WinTeam LeagueLevel
						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_TEAM, _T("EXEC [dbo].[up_Set_PvPLeague_Team]"));
						kQuery.PushStrParam(team_itr->first);
						kQuery.PushStrParam(iNextLeagueLevel);
						g_kCoreCenter.PushQuery(kQuery, true);
					}
				}
			}
		}
		++battle_itr;
	}
}

PgPvPTournament::PgPvPTournament(BM::GUID const & rkTournamentGuid)
{
	Clear();
	m_kGuid = rkTournamentGuid;
}

PgPvPTournament::PgPvPTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement)
{
	Clear();
	m_kGuid = rkTournamentGuid;
	AddBattle(rkBattleGuid, rkBattleElement);
}

PgPvPTournament::~PgPvPTournament()
{
	Clear();
}

void PgPvPTournament::Clear()
{
	m_kGuid.Clear();
	m_kBasicInfo.Clear();
	m_kContGroup.clear();
}

bool PgPvPTournament::CreateGroup(int const iMaxGroupCount)
{
	int iTournamentIndex = 1;
	for(int iGroupIndex=1; iGroupIndex<=iMaxGroupCount; ++iGroupIndex)
	{
		PgPvPGroup kGroup(m_kGuid, iGroupIndex);
		kGroup.CreateBattle(iTournamentIndex);
		++iTournamentIndex;
		kGroup.CreateBattle(iTournamentIndex);
		++iTournamentIndex;
		m_kContGroup.insert( std::make_pair(iGroupIndex, kGroup) );
	}

	return true;
}

bool PgPvPTournament::RegistTeam(CONT_DEF_PVPLEAGUE_TEAM const &rkContTeam)
{
	CONT_DEF_PVPLEAGUE_TEAM::const_iterator team_itr =	rkContTeam.begin();
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.begin();
	while( rkContTeam.end() != team_itr )
	{
		BM::GUID const & kTeamGuid = team_itr->first;
		PgPvPGroup &kGroup = group_itr->second;
		if( !kGroup.Insert(kTeamGuid) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		++team_itr;
		++group_itr;
		if( m_kContGroup.end() == group_itr )
		{
			group_itr = m_kContGroup.begin();
		}
	}

	return true;
}

void PgPvPTournament::SaveBattle(BM::GUID const & rkLeagueGuid)
{
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		group_itr->second.SaveBattle(rkLeagueGuid, m_kBasicInfo.iLeagueLevel);
		++group_itr;
	}
}

bool PgPvPTournament::CreateTournament(BM::GUID const & rkLeagueGuid, int const iMaxGroupCount, int const iMaxTeamCount, CONT_DEF_PVPLEAGUE_TEAM const &rkContTeam)
{
	if( rkContTeam.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	//int const iMaxGroupCount = iMaxTeamCount/4;
	if( !CreateGroup(iMaxGroupCount) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	if( !RegistTeam(rkContTeam) )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	if( !IsAllAutoWin() )
	{
		SaveBattle(rkLeagueGuid);
	}
	return true;
}

bool PgPvPTournament::IsAllEmptyBattle() const
{
	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		if( !group_itr->second.IsEmpty() )
		{
			return false;
		}
		++group_itr;
	}

	return true;
}

bool PgPvPTournament::IsAllAutoWin() const
{
	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		if( !group_itr->second.IsAllAutoWin() )
		{
			return false;
		}
		++group_itr;
	}

	return true;
}

bool PgPvPTournament::IsAutoWin(BM::GUID const &rkTeamGuid) const
{
	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		if( group_itr->second.IsAutoWin(rkTeamGuid) )
		{
			return true;
		}
		++group_itr;
	}

	return false;
}

bool PgPvPTournament::AddBattle(int const iGroupIndex, int const iTournamentIndex, TBL_DEF_PVPLEAGUE_BATTLE & rkBattleInfo)
{
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.find(iGroupIndex);
	if( m_kContGroup.end() != group_itr )
	{
		return group_itr->second.AddBattle(iTournamentIndex, rkBattleInfo);
	}
	else
	{
		PgPvPGroup kGroup(m_kGuid, iGroupIndex);
		if( kGroup.AddBattle(iTournamentIndex, rkBattleInfo) )
		{
			m_kContGroup.insert( std::make_pair(iGroupIndex, kGroup) );
			return true;
		}
	}

	return false;
}

bool PgPvPTournament::CreateNextTournament(BM::GUID const & rkLeagueGuid, PgPvPTournament& rkTournament, TBL_DEF_PVPLEAGUE_TOURNAMENT const & rkBasicInfo)
{
	if( rkBasicInfo.iLeagueLevel != m_kBasicInfo.iLeagueLevel+1 || m_kContGroup.empty() )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	int iTournamentIndex = 1;
	int iGroupIndex = 1;
	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		TBL_DEF_PVPLEAGUE_BATTLE kBattleInfo1, kBattleInfo2;
		
		if( group_itr->second.GetNextBattle(kBattleInfo1) )
		{
			rkTournament.AddBattle(iGroupIndex, iTournamentIndex, kBattleInfo1);
			++iTournamentIndex;
		}

		++group_itr;
		if( m_kContGroup.end() != group_itr )
		{
			if( group_itr->second.GetNextBattle(kBattleInfo2) )
			{
				rkTournament.AddBattle(iGroupIndex, iTournamentIndex, kBattleInfo2);
				++iTournamentIndex;
			}
			++group_itr;
			++iGroupIndex;
		}
		else if( 1 == m_kContGroup.size() )
		{//준결승->결승 예외
			break;
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	if( !rkTournament.IsAllAutoWin() )
	{
		rkTournament.SaveBattle(rkLeagueGuid);
	}
	return true;
}

bool PgPvPTournament::AddBattle(BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkElement)
{
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.find(rkElement.iGroupIndex);
	if( m_kContGroup.end() != group_itr )
	{
		return group_itr->second.AddBattle(rkBattleGuid, rkElement);
	}
	else
	{
		PgPvPGroup kGroup(m_kGuid, rkElement.iGroupIndex);
		if( kGroup.AddBattle(rkBattleGuid, rkElement) )
		{
			m_kContGroup.insert( std::make_pair(rkElement.iGroupIndex, kGroup) );
			return true;
		}
	}

	return true;
}

bool PgPvPTournament::WriteToBattleInfo(BM::Stream& rkPacket) const
{
	CONT_DEF_PVPLEAGUE_BATTLE kContBattle;

	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		PgPvPGroup const &kGroup = group_itr->second;
		kGroup.WriteToBattleInfo(kContBattle);
		++group_itr;
	}

	PU::TWriteTable_AM(rkPacket, kContBattle);
	return true;
}

bool PgPvPTournament::WriteToBattleInfo(CONT_DEF_PVPLEAGUE_BATTLE& rkContBattle) const
{
	CONT_DEF_PVPLEAGUE_GROUP::const_iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		PgPvPGroup const &kGroup = group_itr->second;
		kGroup.WriteToBattleInfo(rkContBattle);
		++group_itr;
	}

	return true;
}

bool PgPvPTournament::SetBattleResult(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, BM::GUID const & rkBattleGuid, int const iWinTeam)
{
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		PgPvPGroup &kGroup = group_itr->second;
		if( kGroup.Find(rkBattleGuid) )
		{
			return kGroup.SetBattleResult(rkContTeam, rkBattleGuid, iWinTeam, m_kBasicInfo.iLeagueLevel+1);
		}
		++group_itr;
	}
	
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}
bool PgPvPTournament::WriteTournamentInfo(BM::Stream& rkPacket) const
{
	m_kBasicInfo.WriteToPacket(rkPacket);
	return true;
}

void PgPvPTournament::AutoWin(CONT_DEF_PVPLEAGUE_TEAM &rkContTeam, bool bIsSaveDB)
{
	CONT_DEF_PVPLEAGUE_GROUP::iterator group_itr = m_kContGroup.begin();
	while( m_kContGroup.end() != group_itr )
	{
		group_itr->second.AutoWin(m_kBasicInfo.iLeagueLevel+1, rkContTeam, bIsSaveDB);
		++group_itr;
	}
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//
//-- class :: PgPvPLeague
//
//--------------------------------------------------------------
//--------------------------------------------------------------

PgPvPLeague::PgPvPLeague(BM::GUID const & rkLeagueGuid, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement)
{
	Clear();
	m_kGuid = rkLeagueGuid;
	m_kBasicInfo = rkLeagueElement;
}

PgPvPLeague::~PgPvPLeague()
{
	Clear();
}

void PgPvPLeague::Init()
{
	m_kBasicInfo.iLeagueState = PVPLS_WAIT;
	int iMaxTeamCount = 256;
	//int iMaxTeamCount = 0;
	//if( S_OK != g_kVariableContainer.Get(EVar_Kind_PvPLeague, EVar_PvPLeague_MaxTeamCount, iMaxTeamCount) )
	//{
	//	VERIFY_INFO_LOG( false, BM::LOG_LV1, __FL__ << _T("Can't Find 'EVar_PvPLeague_MaxTeamCount'") );
	//}
	//m_iMaxTeamCount = iMaxTeamCount;
}

void PgPvPLeague::Clear()
{
	m_iMaxTeamCount = 256;
	m_kGuid.Clear();
	m_kBasicInfo.Clear();
	m_kContTeam.clear();
	m_kContCharToTeam.clear();
	m_kContNameToTeam.clear();
	m_kContTournament.clear();
}

bool PgPvPLeague::LoadDB(BM::Stream& rkPacket)
{
	PU::TLoadTable_AM(rkPacket, m_kContTeam);
	CONT_DEF_PVPLEAGUE_TEAM::const_iterator team_itr = m_kContTeam.begin();
	while( m_kContTeam.end() != team_itr )
	{
		CONT_DEF_PVPLEAGUE_TEAM::key_type const & kKey = team_itr->first;
		CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & kElement = team_itr->second;

		CONT_LEAGUE_USER::const_iterator charid_itr = kElement.kContUserGuid.begin();
		while( kElement.kContUserGuid.end() != charid_itr )
		{
			if( (charid_itr->kGuid).IsNotNull()
				&& m_kContCharToTeam.end() == m_kContCharToTeam.find(charid_itr->kGuid) )
			{
				m_kContCharToTeam.insert( std::make_pair(charid_itr->kGuid, kKey) );
			}
			++charid_itr;
		}

		if( m_kContNameToTeam.end() == m_kContNameToTeam.find( kElement.wstrTeamName ) )
		{
			m_kContNameToTeam.insert( std::make_pair(kElement.wstrTeamName, kKey) );
		}
		++team_itr;
	}
	
	CONT_DEF_PVPLEAGUE_BATTLE kContBattle;
	CONT_DEF_PVPLEAGUE_TOURNAMENT kContTournament;
	PU::TLoadTable_AM(rkPacket, kContBattle);
	PU::TLoadTable_AM(rkPacket, kContTournament);

	//Initialize Tournament
	CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = kContBattle.begin();
	while( kContBattle.end() != battle_itr )
	{
		CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kElement = battle_itr->second;
		CONT_DEF_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = kContTournament.find(kElement.kTournamentGuid);
		if( kContTournament.end() != tour_itr )
		{
			if( !InitTournament(tour_itr->first, battle_itr->first, kElement) )
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Tournament Initialize Failed!! GUID [" << kElement.kTournamentGuid << L"]");
				return false;
			}
		}
		else
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Cannot Find Tournament GUID [" << kElement.kTournamentGuid << L"] Load Failed!!");
			return false;
		}
		++battle_itr;
	}
	
	//Update Tournament Info
	CONT_DEF_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = kContTournament.begin();
	while( kContTournament.end() != tour_itr )
	{
		if( !SetTournamentInfo(tour_itr->first, tour_itr->second) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"Cannot Find Tournament GUID [" << tour_itr->first << L"] Load Failed!!");
			return false;
		}
		++tour_itr;
	}

	return true;
}

bool PgPvPLeague::InitTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement)
{
	CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(rkTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		return tour_itr->second.AddBattle(rkBattleGuid, rkBattleElement);
	}
	else
	{
		return AddTournament(rkTournamentGuid, rkBattleGuid, rkBattleElement);
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::AddTournament(BM::GUID const & rkTournamentGuid, BM::GUID const & rkBattleGuid, CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & rkBattleElement)
{
	PgPvPTournament kTournament(rkTournamentGuid, rkBattleGuid, rkBattleElement);
	m_kContTournament.insert( std::make_pair(rkTournamentGuid, kTournament) );
	return true;
}

bool PgPvPLeague::SetTournamentInfo(BM::GUID const & rkTournamentGuid, CONT_DEF_PVPLEAGUE_TOURNAMENT::mapped_type const & rkTournamentElement)
{
	CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(rkTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		tour_itr->second.SetBasicInfo(rkTournamentElement);
		return true;
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
}

bool PgPvPLeague::IsAllAutoWin() const
{
	CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		return tour_itr->second.IsAllAutoWin();
	}
	return false;
}

bool PgPvPLeague::IsAllEmptyBattle() const
{
	CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		return tour_itr->second.IsAllEmptyBattle();
	}
	return false;
}

bool PgPvPLeague::CreateTournament(TBL_DEF_PVPLEAGUE_TIME const & rkBattleTimeInfo, SYSTEMTIME const & rkNowDate)
{
	BM::GUID kTournamentGuid;
	kTournamentGuid.Generate();

	const CONT_DEF_PVPLEAGUE_SESSION* pkContDefPvPLeagueSession = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueSession);
	if( !pkContDefPvPLeagueSession )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	
	CONT_DEF_PVPLEAGUE_SESSION::const_iterator session_itr = pkContDefPvPLeagueSession->find(m_kBasicInfo.iSession);
	if( pkContDefPvPLeagueSession->end() == session_itr )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}
	TBL_DEF_PVPLEAGUE_SESSION const &kSessionInfo = session_itr->second;


	TBL_DEF_PVPLEAGUE_TOURNAMENT kTournamentInfo;
	kTournamentInfo.iLeagueLevel = rkBattleTimeInfo.iLeagueLevel;
	kTournamentInfo.iGameType = kSessionInfo.iGameType;
	kTournamentInfo.iGameMode = kSessionInfo.iGameMode;
	kTournamentInfo.iGameTime = kSessionInfo.iGameTime;
	::CGameTime::SecTime2DBTimeEx(rkBattleTimeInfo.i64BeginTime, kTournamentInfo.kBeginTime, CGameTime::DEFAULT);
	kTournamentInfo.kBeginTime.year = rkNowDate.wYear;
	kTournamentInfo.kBeginTime.month = rkNowDate.wMonth;
	kTournamentInfo.kBeginTime.day = rkNowDate.wDay+(rkBattleTimeInfo.iDayOfWeek - rkNowDate.wDayOfWeek);

	PgPvPTournament kTournament(kTournamentGuid);
	kTournament.SetBasicInfo(kTournamentInfo);

	if( m_kBasicInfo.kTournamentGuid.IsNull() )
	{
		int iMaxTeamCount = m_kContTeam.size(), iMaxGroupCount=0, iLeagueLevel=0;
		GetGroupLevel(iMaxTeamCount, iMaxGroupCount, iLeagueLevel);

		kTournamentInfo.iLeagueLevel = iLeagueLevel;
		kTournament.SetBasicInfo(kTournamentInfo);

		if( !kTournament.CreateTournament(m_kGuid, iMaxGroupCount, iMaxTeamCount, m_kContTeam) )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		if( kTournament.IsAllAutoWin() )
		{
			kTournament.AutoWin(m_kContTeam, false);
		}
	}
	else
	{
		CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
		if( m_kContTournament.end() != tour_itr )
		{
			if( !(tour_itr->second.CreateNextTournament(m_kGuid, kTournament, kTournamentInfo)) 
				|| kTournament.IsAllEmptyBattle() )
			{//리그를 종료시킨다
				m_kBasicInfo.iLeagueState = PVPLS_END;
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_STATE, _T("EXEC [dbo].[up_Set_PvPLeague_State]"));
				kQuery.PushStrParam(m_kGuid);
				kQuery.PushStrParam(m_kBasicInfo.iLeagueState);
				g_kCoreCenter.PushQuery(kQuery, true);
				return false;
			}

			if( kTournament.IsAllAutoWin() )
			{//모두 부전승
				kTournament.AutoWin(m_kContTeam, false);
			}
		}
		else
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	m_kBasicInfo.kTournamentGuid = kTournamentGuid;
	m_kContTournament.insert( std::make_pair(kTournamentGuid, kTournament) );

	CEL::DB_QUERY kQuery(DT_PLAYER, DQT_INSERT_PVPLEAGUE_TOURNAMENT, _T("EXEC [dbo].[up_Insert_PvPLeague_Tournament]"));
	kQuery.PushStrParam(m_kGuid);
	kQuery.PushStrParam(kTournamentGuid);
	kQuery.PushStrParam(kTournamentInfo.iLeagueLevel);
	kQuery.PushStrParam(kTournamentInfo.iGameType);
	kQuery.PushStrParam(kTournamentInfo.iGameMode);
	kQuery.PushStrParam(kTournamentInfo.iGameTime);
	g_kCoreCenter.PushQuery(kQuery, true);
	return true;
}

void PgPvPLeague::InitTeamInfo()
{
	int iIndex = 1;
	CONT_DEF_PVPLEAGUE_TEAM::iterator team_itr = m_kContTeam.begin();
	while( m_kContTeam.end() != team_itr )
	{
		CONT_DEF_PVPLEAGUE_TEAM::mapped_type &kElement = team_itr->second;
		kElement.iTournamentIndex = iIndex++;
		kElement.iLeagueLevel = GetLeagueLevel();

		CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_TEAM_INDEX, _T("EXEC [dbo].[up_Set_PvPLeague_TeamIndex]"));
		kQuery.PushStrParam(team_itr->first);
		kQuery.PushStrParam(kElement.iTournamentIndex);
		g_kCoreCenter.PushQuery(kQuery, true);

		++team_itr;
	}
}

HRESULT PgPvPLeague::Check_RegistTeam(TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo) const
{
	switch( m_kBasicInfo.iLeagueState )
	{
	case PVPLS_JOIN:
		{
			if( m_kContNameToTeam.end() != m_kContNameToTeam.find(rkTeamInfo.wstrTeamName) )
			{//팀이름 중뷁
				return E_PVPLEAGUE_REGIST_TEAM_NAME_DUPLICATE;
			}
			
			if( m_iMaxTeamCount <= m_kContTeam.size() )
			{//팀 최대 갯수 초과함 ㅇㅇ
				return E_PVPLEAGUE_REGIST_TEAM_LIMIT_COUNT;
			}

			CONT_LEAGUE_USER::const_iterator user_itr = rkTeamInfo.kContUserGuid.begin();
			while( rkTeamInfo.kContUserGuid.end() != user_itr )
			{
				if( m_kContCharToTeam.end() != m_kContCharToTeam.find( user_itr->kGuid ) )
				{
					return E_PVPLEAGUE_REGIST_TEAM_MEMBER_DUPLICATE;
				}
				++user_itr;
			}
		}break;
	default:
		{//팀 등록할 수 없는 상태임.
			return E_PVPLEAGUE_REGIST_TEAM_WRONG_STATE;
		}break;
	}

	return E_PVPLEAGUE_REGIST_TEAM_SUCCESS;
}

HRESULT PgPvPLeague::RegistTeam(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo)
{
	HRESULT hResult = Check_RegistTeam(rkTeamInfo);
	if( E_PVPLEAGUE_REGIST_TEAM_SUCCESS == hResult )
	{
		m_kContTeam.insert( std::make_pair(rkTeamGuid, rkTeamInfo) );
		m_kContNameToTeam.insert( std::make_pair(rkTeamInfo.wstrTeamName, rkTeamGuid) );
		CONT_LEAGUE_USER::const_iterator charid_itr = rkTeamInfo.kContUserGuid.begin();
		while( rkTeamInfo.kContUserGuid.end() != charid_itr )
		{
			m_kContCharToTeam.insert( std::make_pair(charid_itr->kGuid, rkTeamGuid) );
			++charid_itr;
		}
	}

	return hResult;
}

bool PgPvPLeague::Check_ReleaseTeam(BM::GUID const & rkTeamGuid)
{
	switch( m_kBasicInfo.iLeagueState )
	{
	case PVPLS_JOIN:
		{
			CONT_DEF_PVPLEAGUE_TEAM::const_iterator team_itr = m_kContTeam.find(rkTeamGuid);
			if( m_kContTeam.end() != team_itr )
			{//찾았네
				return true;
			}
		}break;
	default:
		{//팀 등록할 수 없는 상태임.
			return false;
		}break;
	}
	
	return false;
}

bool PgPvPLeague::ReleaseTeam(BM::GUID const & rkTeamGuid)
{
	if( Check_ReleaseTeam(rkTeamGuid) )
	{
		CONT_DEF_PVPLEAGUE_TEAM::iterator team_itr = m_kContTeam.find(rkTeamGuid);
		if( m_kContTeam.end() != team_itr )
		{
			CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & rkElement = team_itr->second;
			CONT_LEAGUE_USER::const_iterator charid_itr = rkElement.kContUserGuid.begin();
			while( rkElement.kContUserGuid.end() != charid_itr )
			{
				CONT_PVPLEAGUE_CHAR_TO_TEAM::iterator char_itr = m_kContCharToTeam.find(charid_itr->kGuid);
				if( m_kContCharToTeam.end() != char_itr)
				{
					m_kContCharToTeam.erase(charid_itr->kGuid);
				}
				++charid_itr;
			}

			CONT_PVPLEAGUE_NAME_TO_TEAM::iterator name_itr = m_kContNameToTeam.find(rkElement.wstrTeamName);
			if( m_kContNameToTeam.end() != name_itr )
			{
				m_kContNameToTeam.erase(name_itr);
			}

			m_kContTeam.erase(team_itr);
			return true;
		}
	}
	return false;
}

bool PgPvPLeague::GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM& rkOutInfo) const
{
	CONT_DEF_PVPLEAGUE_TEAM::const_iterator team_itr = m_kContTeam.find(rkTeamGuid);
	if( m_kContTeam.end() != team_itr )
	{
		rkOutInfo = team_itr->second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::GetTeamID(BM::GUID const & rkCharGuid, BM::GUID& rkOutTeamGuid) const
{
	CONT_PVPLEAGUE_CHAR_TO_TEAM::const_iterator team_itr = m_kContCharToTeam.find(rkCharGuid);
	if( m_kContCharToTeam.end() != team_itr )
	{
		rkOutTeamGuid = team_itr->second;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::IsRegistTeam(BM::GUID const & rkCharGuid) const
{
	CONT_PVPLEAGUE_CHAR_TO_TEAM::const_iterator team_itr = m_kContCharToTeam.find(rkCharGuid);
	if( m_kContCharToTeam.end() != team_itr )
	{
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::SetJoinStart()
{
	if( PVPLS_WAIT == m_kBasicInfo.iLeagueState )
	{
		m_kBasicInfo.iLeagueState = PVPLS_JOIN;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::SetJoinEnd()
{
	if( PVPLS_JOIN == m_kBasicInfo.iLeagueState )
	{
		m_kBasicInfo.iLeagueState = PVPLS_WAIT;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::SetGameReady()
{
	if( PVPLS_WAIT == m_kBasicInfo.iLeagueState )
	{
		m_kBasicInfo.iLeagueState = PVPLS_READY;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::SetGameStart()
{
	if( PVPLS_READY == m_kBasicInfo.iLeagueState)
	{
		m_kBasicInfo.iLeagueState = PVPLS_GAME;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::SetGameEnd()
{
	if( PVPLS_GAME == m_kBasicInfo.iLeagueState)
	{
		m_kBasicInfo.iLeagueState = PVPLS_WAIT;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::NfyLobby_ChangeState() const
{
	BM::Stream kPacket( PT_N_T_NFY_CHANGE_STATE_LEAGUE_LOBBY );

	const CONT_DEF_PVPLEAGUE_TIME* pkContDefPvPLeagueTime = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueTime);
	PU::TWriteArray_A(kPacket, *pkContDefPvPLeagueTime);

	if( WriteLeagueInfo(kPacket) )
	{
		::SendToChannelContents( CProcessConfig::GetPublicChannel(), PMET_PVP_LEAGUE, kPacket, PvP_Lobby_GroundNo_League  );
	}
	return true;
}


bool PgPvPLeague::ReqLobby_CreateRoom()
{
	CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		TBL_DEF_PVPLEAGUE_TOURNAMENT const & kTournamentInfo = tour_itr->second.GetBasicInfo();
		
		SPvPRoomBasicInfo kRoomBasicInfo;
		//kRoomBasicInfo.m_wstrName = L"";//::GetDefString(iDefaultRoomTitleStringNo + BM::Rand_Index(4));
		kRoomBasicInfo.m_bPwd = false;
		kRoomBasicInfo.m_kStatus = ROOM_STATUS_LOBBY;
		kRoomBasicInfo.m_kType = kTournamentInfo.iGameType;
		kRoomBasicInfo.m_kMode = kTournamentInfo.iGameMode;
		kRoomBasicInfo.m_iGameTime = kTournamentInfo.iGameTime;
		kRoomBasicInfo.m_sLevelLimit_Max = 100;
		kRoomBasicInfo.m_sLevelLimit_Min = 1;
		kRoomBasicInfo.m_ucMaxUser = 8;

		SPvPRoomExtInfo kRoomExtInfo;
		kRoomExtInfo.ucRound = 1;
		kRoomExtInfo.kOption = (E_PVP_OPT_USEITEM | E_PVP_OPT_USEBATTLELEVEL); //10

		BM::Stream kPacket( PT_N_T_REQ_CREATE_PVPLEAGUE_ROOM );

		//룸 정보 셋팅
		kRoomBasicInfo.WriteToPacket(kPacket);
		kRoomExtInfo.WriteToPacket(kPacket);

		//부전승 처리
		tour_itr->second.AutoWin(m_kContTeam);

		//배틀 정보 
		tour_itr->second.WriteToBattleInfo(kPacket);

		::SendToChannelContents( CProcessConfig::GetPublicChannel(), PMET_PVP_LEAGUE, kPacket, PvP_Lobby_GroundNo_League  );
		return true;
	}
	return false;
}

bool PgPvPLeague::NfyMessage(EPvPLeagueMsg const eMessageType) const
{
	CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() == tour_itr )
	{
		return false;
	}
	PgPvPTournament const &rkTournament = tour_itr->second;
	int iLeagueLevel = GetLeagueLevel();

	VEC_GUID kContGuid;
	CONT_DEF_PVPLEAGUE_TEAM::const_iterator team_itr = m_kContTeam.begin();
	while(m_kContTeam.end() != team_itr)
	{
		CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & kElement = team_itr->second;
		if( GetLeagueLevel() == kElement.iLeagueLevel )
		{
			switch( eMessageType )
			{
			case PVPLM_INVITE:
				{//참가 대상 유저에게 초대 메시지
					if( !rkTournament.IsAutoWin(team_itr->first) )
					{
						CONT_LEAGUE_USER::const_iterator userid_itr = kElement.kContUserGuid.begin();
						while(kElement.kContUserGuid.end() != userid_itr)
						{
							kContGuid.push_back(userid_itr->kGuid);
							++userid_itr;
						}
					}
				}break;
			case PVPLM_AUTOWIN:
				{//부전승 유저에게 메시지
					if( rkTournament.IsAutoWin(team_itr->first) )
					{
						CONT_LEAGUE_USER::const_iterator userid_itr = kElement.kContUserGuid.begin();
						while(kElement.kContUserGuid.end() != userid_itr)
						{
							kContGuid.push_back(userid_itr->kGuid);
							++userid_itr;
						}
						++iLeagueLevel;
					}
				}break;
			case PVPLM_READY:
			default: 
				{//모든 유저에게 메시지 보냄
					CONT_LEAGUE_USER::const_iterator userid_itr = kElement.kContUserGuid.begin();
					while(kElement.kContUserGuid.end() != userid_itr)
					{
						kContGuid.push_back(userid_itr->kGuid);
						++userid_itr;
					}
				}break;
			}
		}
		++team_itr;
	}

	BM::Stream kPacket(PT_N_C_NFY_PVPLEAGUE_NOTICE_MSG, eMessageType);
	kPacket.Push( iLeagueLevel );
	g_kRealmUserMgr.Locked_SendToUser(kContGuid, kPacket, false);
	return true;
}

bool PgPvPLeague::SetBattleResult(BM::GUID const & rkBattleGuid, int const iWinTeam)
{
	CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		return tour_itr->second.SetBattleResult(m_kContTeam, rkBattleGuid, iWinTeam);
	}

	return false;
}

bool PgPvPLeague::SendBattleReward()
{
	CONT_PVPLEAGUE_TOURNAMENT::iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		CONT_DEF_PVPLEAGUE_BATTLE kContBattle;
		tour_itr->second.WriteToBattleInfo(kContBattle);

		CONT_DEF_PVPLEAGUE_BATTLE::const_iterator battle_itr = kContBattle.begin();
		while( kContBattle.end() != battle_itr )
		{
			CONT_DEF_PVPLEAGUE_BATTLE::mapped_type const & kBattleElement = battle_itr->second;
			TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
			if( GetTeamInfo(kBattleElement.kWinTeamGuid, kTeamInfo) )
			{
				if( !SendBattleReward(kTeamInfo) )
				{//이긴넘한테 보상을 주자
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
					return false;
				}
			}
			++battle_itr;
		}
	}
	else
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	return true;
}

bool PgPvPLeague::SendBattleReward(CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & rkElement)
{
	//업적 보상
	const int iPvpLeagueLevel = GetLeagueLevel();
	switch( iPvpLeagueLevel )
	{
	case EPLL_16:
	case EPLL_8:
	case EPLL_4:
	case EPLL_2:
	case EPLL_VICTORY:
		{
			const CONT_DEF_ACHIEVEMENTS* pkContDef = NULL;
			g_kTblDataMgr.GetContDef(pkContDef);
			if( !pkContDef )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}

			CONT_DEF_ACHIEVEMENTS::const_iterator c_Achievement_iter = pkContDef->begin();
			while( pkContDef->end() != c_Achievement_iter )
			{
				CONT_DEF_ACHIEVEMENTS::mapped_type kTblAchievements = c_Achievement_iter->second;
				if( AT_ACHIEVEMENT_PVP_LEAGUE_REWARD == kTblAchievements.iType
				&&	iPvpLeagueLevel == kTblAchievements.iValue )
				{
					PostSystemMail(rkElement, iPvpLeagueLevel, kTblAchievements.iItemNo);
					break;
				}

				++c_Achievement_iter;
			}
		}break;
	default:
		{
		}break;
	}

	//기타 아이템 보상
	const CONT_DEF_PVPLEAGUE_REWARD* pkContDefPvPLeagueReward = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueReward);
	if( pkContDefPvPLeagueReward )
	{
		CONT_DEF_PVPLEAGUE_REWARD::const_iterator main_itr = pkContDefPvPLeagueReward->find( iPvpLeagueLevel );
		if( pkContDefPvPLeagueReward->end() != main_itr )
		{
			CONT_DEF_PVPLEAGUE_REWARD::mapped_type const kVecReward= main_itr->second;
			VEC_DEF_PVPLEAGUE_REWARD::const_iterator reward_itr = kVecReward.begin();
			while( kVecReward.end() != reward_itr )
			{
				if( reward_itr->iItemNo )
				{
					PostSystemMail(rkElement, iPvpLeagueLevel, reward_itr->iItemNo, reward_itr->iItemCount, EPLS_MAIL_CONTENTS2);
				}
				++reward_itr;
			}
		}
	}

	return true;
}

void PgPvPLeague::PostSystemMail(CONT_DEF_PVPLEAGUE_TEAM::mapped_type const & rkElement, int const iLeagueLevel, int const iItemNo, int const iItemCount, EPVP_LEAGUE_STRING const eMailContentsStringNo)
{
	std::wstring kSenderString;
	std::wstring kTitleString;
	std::wstring kContentsString;
	std::wstring kLeagueLevelString;
	std::wstring kItemName;

	::GetDefString( static_cast<int>(EPLS_MAIL_TITLE), kSenderString  );
	::GetDefString( static_cast<int>(EPLS_MAIL_SENDER), kTitleString  );
	::GetDefString( static_cast<int>(eMailContentsStringNo), kContentsString );
	::GetDefString( static_cast<int>( iItemNo ), kItemName );

	switch( iLeagueLevel )
	{
	case EPLL_16:		{ ::GetDefString( static_cast<int>(EPLS_16), kLeagueLevelString ); }break;
	case EPLL_8:			{ ::GetDefString( static_cast<int>(EPLS_8), kLeagueLevelString ); }break;
	case EPLL_4:			{ ::GetDefString( static_cast<int>(EPLS_4), kLeagueLevelString ); }break;
	case EPLL_2:			{ ::GetDefString( static_cast<int>(EPLS_2), kLeagueLevelString ); }break;
	case EPLL_VICTORY:	{ ::GetDefString( static_cast<int>(EPLS_VICTORY), kLeagueLevelString ); }break;
	default:{}break;
	}

	BM::vstring vStrContents(kContentsString);
	vStrContents.Replace(L"#GRADE#", kLeagueLevelString);
	vStrContents.Replace(L"#ITEMNAME#", kItemName);

	CONT_LEAGUE_USER::const_iterator c_TeamUser_iter = rkElement.kContUserGuid.begin();
	while( rkElement.kContUserGuid.end() != c_TeamUser_iter )
	{
		CONT_LEAGUE_USER::value_type kTeamUser = (*c_TeamUser_iter);
		g_kPostMgr.PostSystemMail( kTeamUser.kGuid, kSenderString, kTeamUser.wstrName, kTitleString, vStrContents, iItemNo, iItemCount, 0i64 );
		++c_TeamUser_iter;
	}
}
int PgPvPLeague::GetLeagueLevel() const
{
	CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		return tour_itr->second.GetLeagueLevel();
	}

	return 0;
}

bool PgPvPLeague::WriteLeagueInfo(BM::Stream& rkPacket) const
{
	CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
	if( m_kContTournament.end() != tour_itr )
	{
		m_kBasicInfo.WriteToPacket(rkPacket);
		tour_itr->second.WriteTournamentInfo(rkPacket);
		PU::TWriteTable_AM(rkPacket, m_kContTeam);
		tour_itr->second.WriteToBattleInfo(rkPacket);
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeague::CheckTournament(SPvPLeague_Event eEvent)
{
	if( PVPLE_NFY_GMAE_READY == eEvent.eEvent )
	{
		CONT_PVPLEAGUE_TOURNAMENT::const_iterator tour_itr = m_kContTournament.find(m_kBasicInfo.kTournamentGuid);
		if( m_kContTournament.end() != tour_itr )
		{
			return (eEvent.iLevel == tour_itr->second.GetLeagueLevel());
		}
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return true;
}

void PgPvPLeague::SaveWinTeam()
{
}

void PgPvPLeague::SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid)
{
	CONT_DEF_PVPLEAGUE_TEAM::iterator team_itr = m_kContTeam.find(rkTeamGuid);
	if( m_kContTeam.end() != team_itr )
	{
		team_itr->second.iLeagueLevel = iLeagueLevel;
	}
}

void PgPvPLeague::GetGroupLevel(int const iTeamCount, int& iGroupCount, int& iLeagueLevel)
{
	int iResultLevel = 0;
	iLeagueLevel = 1;
	do
	{
		iResultLevel = ::pow(2.0f, iLeagueLevel);
		if( iResultLevel >= iTeamCount )
		{
			if( 1 < iLeagueLevel )
			{
				--iLeagueLevel;
			}
			break;
		}
		else
		{
			iGroupCount = iResultLevel/2.0f;
			++iLeagueLevel;
		}
	}while(m_iMaxTeamCount > iResultLevel);
}