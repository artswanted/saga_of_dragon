#include "StdAfx.h"
#include "Lohengrin/PgPlayLimiter.h"
#include "PgServerSetMgr.h"
#include "PgPvPLeagueMgr.h"
#include "Lohengrin/GMCommand.h"

int const MAX_SESSION_COUNT = 4;
//--------------------------------------------------------------
//-- namespace :: PgPvPLeagueUtil
//--------------------------------------------------------------
namespace PgPvPLeagueUtil
{
	bool Q_DQT_LOAD_DEF_PVPLEAGUE_TIME(CEL::DB_RESULT& rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
			return false;
		}

		CONT_DEF_PVPLEAGUE_TIME kContPvPLeagueTime;
		CEL::DB_DATA_ARRAY::iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			CONT_DEF_PVPLEAGUE_TIME::value_type kElement;
			itr->Pop( kElement.iDayOfWeek );	++itr;
			itr->Pop( kElement.iLeagueState);	++itr;
			itr->Pop( kElement.iLeagueLevel);	++itr;

			char cHour = 0;
			char cMin = 0;

			itr->Pop( cHour );					++itr;
			itr->Pop( cMin );						++itr;
			kElement.i64BeginTime = (static_cast<__int64>(cHour) * CGameTime::HOUR) + (static_cast<__int64>(cMin) * CGameTime::MINUTE);

			itr->Pop( cHour );					++itr;
			itr->Pop( cMin );						++itr;
			kElement.i64EndTime = (static_cast<__int64>(cHour) * CGameTime::HOUR) + (static_cast<__int64>(cMin) * CGameTime::MINUTE) + (CGameTime::MINUTE - 1i64);//초단위는 Max초로 강제;

			kContPvPLeagueTime.push_back( kElement );	
		}

		std::sort( kContPvPLeagueTime.begin(), kContPvPLeagueTime.end() );

		g_kTblDataMgr.SetContDef(kContPvPLeagueTime);
		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}

	bool Q_DQT_LOAD_DEF_PVPLEAGUE_SESSION(CEL::DB_RESULT& rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
			return false;
		}

		CONT_DEF_PVPLEAGUE_SESSION kContPvPLeagueSession;
		CEL::DB_DATA_ARRAY::iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			CONT_DEF_PVPLEAGUE_SESSION::key_type kKey;
			CONT_DEF_PVPLEAGUE_SESSION::mapped_type kElement;
			itr->Pop( kKey );	++itr;
			itr->Pop( kElement.iGameMode);	++itr;
			itr->Pop( kElement.iGameType );	++itr;
			itr->Pop( kElement.iGameTime);	++itr;

			kContPvPLeagueSession.insert( std::make_pair(kKey, kElement) );
		}

		g_kTblDataMgr.SetContDef(kContPvPLeagueSession);
		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}

	bool Q_DQT_LOAD_DEF_PVPLEAGUE_REWARD(CEL::DB_RESULT& rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if(CEL::DR_SUCCESS != rkResult.eRet && CEL::DR_NO_RESULT != rkResult.eRet)
		{
			INFO_LOG(BM::LOG_LV5, __FL__ << _T(" DB Query failed ErrorCode=") << rkResult.eRet << _T(", Query=") << rkResult.Command());
			return false;
		}

		CONT_DEF_PVPLEAGUE_REWARD kContDefPvPLeagueReward;
		CEL::DB_DATA_ARRAY::iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			CONT_DEF_PVPLEAGUE_REWARD::key_type kKey;
			CONT_DEF_PVPLEAGUE_REWARD::mapped_type kElement;

			itr->Pop( kKey );	++itr;
			for( int iIndex=0; iIndex<5; ++iIndex)
			{
				TBL_DEF_PVPLEAGUE_REWARD kReward;
				itr->Pop( kReward.iItemNo);	++itr;
				itr->Pop( kReward.iItemCount);	++itr;

				kElement.push_back(kReward);
			}

			kContDefPvPLeagueReward.insert( std::make_pair(kKey, kElement) );
		}

		g_kTblDataMgr.SetContDef(kContDefPvPLeagueReward);
		g_kCoreCenter.ClearQueryResult(rkResult);
		return true;
	}

	bool Q_DQT_LOAD_PVPLEAGUE(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		
		CONT_DEF_PVPLEAGUE kPvPLeague;

 		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();
		//Load TB_PvPLeague
		{
			int const iResultCount = (*count_iter);	++count_iter;
			for(int i = 0;i < iResultCount;++i)
			{
				CONT_DEF_PVPLEAGUE::key_type kKey;
				CONT_DEF_PVPLEAGUE::mapped_type kElement;

				itr->Pop( kKey );		++itr;
				itr->Pop( kElement.kTournamentGuid );	++itr;
				itr->Pop( kElement.iLeagueState );		++itr;
				itr->Pop( kElement.iSession );				++itr;
				itr->Pop( kElement.kRegistDate );			++itr;

				kPvPLeague.insert( std::make_pair(kKey, kElement) );
			}
		}

		BM::GUID kCurLeagueID;
		int iCurSession;
		//Load TB_PvPLeague
		{
			int const iResultCount = (*count_iter);	++count_iter;
			for(int i = 0;i < iResultCount;++i)
			{
				itr->Pop( kCurLeagueID );		++itr;
				itr->Pop( iCurSession );			++itr;
			}
		}

		BM::Stream kPacket(PT_N_N_NFY_RELOAD_PVPLEAGUE);
		PU::TWriteTable_AM(kPacket, kPvPLeague);
		kPacket.Push(kCurLeagueID);
		kPacket.Push(iCurSession);
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}

	bool Q_DQT_LOAD_PVPLEAGUE_SUB(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		CEL::DB_RESULT_COUNT::const_iterator count_iter = rkResult.vecResultCount.begin();

		//Load TB_PvPLeague_Team
		CONT_DEF_PVPLEAGUE_TEAM kPvPLeagueTeam;
		{
			int const iResultCount = (*count_iter);	++count_iter;
			for(int i = 0;i < iResultCount;++i)
			{
				CONT_DEF_PVPLEAGUE_TEAM::key_type kKey;
				CONT_DEF_PVPLEAGUE_TEAM::mapped_type kElement;

				itr->Pop( kKey );			++itr;
				itr->Pop( kElement.wstrTeamName);	++itr;
				for( int i=0; i<4; ++i )
				{
					SPvPLeague_User kUser;
					itr->Pop( kUser.kGuid );		++itr;
					itr->Pop( kUser.wstrName );	++itr;
					kElement.kContUserGuid.push_back( kUser );
				}
				itr->Pop( kElement.kRegistDate);			++itr;
				itr->Pop( kElement.iTournamentIndex);	++itr;
				itr->Pop( kElement.iLeagueLevel);			++itr;
				itr->Pop( kElement.iPoint);					++itr;

				kPvPLeagueTeam.insert( std::make_pair(kKey, kElement) );
			}
		}

		//Load TB_PvPLeague_Battle
		CONT_DEF_PVPLEAGUE_BATTLE kContBattle;
		{
			int const iResultCount = (*count_iter);	++count_iter;
			for(int i = 0;i < iResultCount;++i)
			{
				CONT_DEF_PVPLEAGUE_BATTLE::key_type kKey;
				CONT_DEF_PVPLEAGUE_BATTLE::mapped_type kElement;

				itr->Pop( kKey );								++itr;
				itr->Pop( kElement.kTournamentGuid );	++itr;
				itr->Pop( kElement.iTournamentIndex );	++itr;
				itr->Pop( kElement.kTeamGuid1 );		++itr;
				itr->Pop( kElement.kTeamGuid2 );		++itr;
				itr->Pop( kElement.kWinTeamGuid );		++itr;
				itr->Pop( kElement.iGroupIndex );		++itr;

				kContBattle.insert( std::make_pair(kKey, kElement) );
			}
		}

		//Load TB_PvPLeague_Tournament
		CONT_DEF_PVPLEAGUE_TOURNAMENT kContTournament;
		{
			int const iResultCount = (*count_iter);	++count_iter;
			for(int i = 0;i < iResultCount;++i)
			{
				CONT_DEF_PVPLEAGUE_TOURNAMENT::key_type kKey;
				CONT_DEF_PVPLEAGUE_TOURNAMENT::mapped_type kElement;

				itr->Pop( kKey );							++itr;
				itr->Pop( kElement.iLeagueLevel );	++itr;
				itr->Pop( kElement.iGameType );		++itr;
				itr->Pop( kElement.iGameMode );		++itr;
				itr->Pop( kElement.iGameTime );		++itr;
				itr->Pop( kElement.kBeginTime );		++itr;

				kContTournament.insert( std::make_pair(kKey, kElement) );
			}
		}

		BM::GUID kCurLeagueID;
		rkResult.contUserData.Pop(kCurLeagueID);

		BM::Stream kPacket(PT_N_N_NFY_UPDATE_PVPLEAGUE, kCurLeagueID);
		PU::TWriteTable_AM(kPacket, kPvPLeagueTeam);
		PU::TWriteTable_AM(kPacket, kContBattle);
		PU::TWriteTable_AM(kPacket, kContTournament);
		kPacket.Push(kCurLeagueID);
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	
	bool Q_DQT_INSERT_PVPLEAGUE(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		BM::GUID kLeagueGuid;
		TBL_DEF_PVPLEAGUE kLeagueInfo;

		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			itr->Pop( kLeagueGuid ); ++itr;
			itr->Pop( kLeagueInfo.kRegistDate ); ++itr;
		}
		
		rkResult.contUserData.Pop(kLeagueInfo.iSession);

		BM::Stream kPacket(PT_N_N_NFY_INIT_PVPLEAGUE);
		kPacket.Push(kLeagueGuid);
		kPacket.Push(kLeagueInfo);
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	
	bool Q_DQT_INSERT_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		//Load TB_PvPLeague_Team
		CONT_DEF_PVPLEAGUE_TEAM::key_type kTeamGuid;
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			itr->Pop( kTeamGuid );			++itr;
		}

		BM::GUID kReqUserGuid;
		rkResult.contUserData.Pop(kReqUserGuid);

		BM::Stream kPacket(PT_N_N_NFY_REGIST_PVPLEAGUE_TEAM);
		kPacket.Push( kTeamGuid );
		kPacket.Push( kReqUserGuid );
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}

	bool Q_DQT_DELETE_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		BM::GUID kReqUserGuid;
		TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
		rkResult.contUserData.Pop(kReqUserGuid);
		kTeamInfo.ReadFromPacket(rkResult.contUserData);

		BM::Stream kPacket(PT_N_N_NFY_GIVEUP_PVPLEAGUE_TEAM);
		kPacket.Push(kReqUserGuid);
		kTeamInfo.WriteToPacket(kPacket);
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}

	bool Q_DQT_INSERT_PVPLEAGUE_TOURNAMENT(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_INSERT_PVPLEAGUE_BATTLE(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		int iLeagueLevel = 0;
		BM::GUID kTeamGuid1, kTeamGuid2;
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			itr->Pop( iLeagueLevel );			++itr;
			itr->Pop( kTeamGuid1 );			++itr;
			itr->Pop( kTeamGuid2 );			++itr;
		}

		BM::Stream kPacket(PT_N_N_NFY_PVPLEAGUE_TEAMLEVEL, iLeagueLevel);
		kPacket.Push(kTeamGuid1);
		kPacket.Push(kTeamGuid2);
		::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	bool Q_DQT_SET_PVPLEAGUE_BATTLE(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		BM::GUID kBattleGuid, kWinTeamGuid;
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			itr->Pop( kBattleGuid );			++itr;
			itr->Pop( kWinTeamGuid );	++itr;
		}

		if( kBattleGuid.IsNull() || kWinTeamGuid.IsNull() )
		{
			INFO_LOG(BM::LOG_LV6, __FL__<<L"Cannot Find, BattleGuid <" << kBattleGuid <<L"> TeamGuid <" << kWinTeamGuid <<L">");
			return true;
		}

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}

	bool Q_DQT_SET_PVPLEAGUE_STATE(CEL::DB_RESULT &rkResult)
	{
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		//TB_PvPLeague
		int iLeagueState = 0;
		CEL::DB_DATA_ARRAY::const_iterator itr = rkResult.vecArray.begin();
		while(itr != rkResult.vecArray.end())
		{
			itr->Pop( iLeagueState );			++itr;
		}

		if( PVPLS_END == iLeagueState )
		{//ReLoad
			BM::Stream kPacket(PT_A_N_REQ_LOAD_PVPLEAGUE);
			::SendToRealmContents(PMET_PVP_LEAGUE, kPacket);
		}

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}

	bool Q_DQT_SET_PVPLEAGUE_TEAM(CEL::DB_RESULT &rkResult)
	{
		INFO_LOG(BM::LOG_LV5, __FL__ << _T("START"));
		if( CEL::DR_SUCCESS != rkResult.eRet
		&&	CEL::DR_NO_RESULT != rkResult.eRet )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Load Fail!!"));
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}

		g_kCoreCenter.ClearQueryResult( rkResult );
		return true;
	}
	
}

//--------------------------------------------------------------
//-- class Impl :: PgPvPLeagueUtil
//--------------------------------------------------------------
PgPvPLeagueImpl::PgPvPLeagueImpl(void)
{
	Clear();
}

PgPvPLeagueImpl::~PgPvPLeagueImpl(void)
{
	Clear();
}

void PgPvPLeagueImpl::Clear()
{
	{
		decltype(m_kContEvent) tmpQueue;
		std::swap(m_kContEvent, tmpQueue);
	}
	m_kContLeague.clear();
	m_kCurLeagueID.Clear();
	::GetLocalTime(&m_kDateTime);
}

bool PgPvPLeagueImpl::GetNextBattleTime(int const iLeagueLevel, TBL_DEF_PVPLEAGUE_TIME& rkOutInfo) const
{
	const CONT_DEF_PVPLEAGUE_TIME* pkContDefPvPLeagueTime = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueTime);
	if( pkContDefPvPLeagueTime )
	{
		CONT_DEF_PVPLEAGUE_TIME::const_iterator time_itr = pkContDefPvPLeagueTime->begin();
		while(pkContDefPvPLeagueTime->end() != time_itr)
		{
			if( iLeagueLevel < time_itr->iLeagueLevel
				&& time_itr->iLeagueState == PVPLS_GAME )
			{
				rkOutInfo = *time_itr;
				return true;
			}
			++time_itr;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPLeagueImpl::IsJoinEndDay(WORD const wDayOfWeek) const
{
	const CONT_DEF_PVPLEAGUE_TIME* pkContDefPvPLeagueTime = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueTime);
	if( pkContDefPvPLeagueTime )
	{
		CONT_DEF_PVPLEAGUE_TIME::const_iterator time_itr = pkContDefPvPLeagueTime->begin();
		while(pkContDefPvPLeagueTime->end() != time_itr)
		{
			if( 0 == time_itr->iLeagueLevel
				&& PVPLS_JOIN == time_itr->iLeagueState
				&& wDayOfWeek < time_itr->iDayOfWeek )
			{
				return false;
			}
			++time_itr;
		}
	}

	return true;
}

void PgPvPLeagueImpl::OnTick()
{
	SYSTEMTIME kNow;
	::GetLocalTime( &kNow );
	__int64 const i64NowTimeInDay = g_kEventView.GetLocalSecTimeInDay( CGameTime::DEFAULT );

	if( kNow.wDay != m_kDateTime.wDay )
	{
		BuildTime();
		m_kDateTime = kNow;
	}
	
	if( m_kContEvent.empty() )
	{
		return;
	}

	SPvPLeague_Event kEvent = m_kContEvent.front();
	if( i64NowTimeInDay >= kEvent.i64Time )
	{
		PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
		if( pkLeague )
		{
			m_kContEvent.pop();
			ProcessEvent(kEvent.eEvent);
		}
	}
}

bool PgPvPLeagueImpl::AddLeague(CONT_DEF_PVPLEAGUE& rkContLeague)
{
	m_kContLeague.clear();

	CONT_DEF_PVPLEAGUE::const_iterator league_itr = rkContLeague.begin();
	while( rkContLeague.end() != league_itr )
	{
		if( !AddLeague(league_itr->first, league_itr->second) )
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV5, __FL__ << L"League Initialize Failed!! GUID [" << league_itr->first <<L"] ");
			return false;
		}
		++league_itr;
	}
	return true;
}

bool PgPvPLeagueImpl::AddLeague(BM::GUID const & rkLeagueID, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement)
{
	CONT_LEAGUE::iterator league_itor = m_kContLeague.find(rkLeagueID);
	if( m_kContLeague.end() != league_itor )
	{
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}

	PgPvPLeague kLeague(rkLeagueID, rkLeagueElement);
	m_kContLeague.insert( std::make_pair(rkLeagueID, kLeague) );
	return true;
}

void PgPvPLeagueImpl::SendToUser(CONT_LEAGUE_USER const &rkContUser, BM::Stream const &rkPacket) const
{
	CONT_LEAGUE_USER::const_iterator user_itr = rkContUser.begin();
	while( rkContUser.end() != user_itr )
	{
		g_kRealmUserMgr.Locked_SendToUser(user_itr->kGuid, rkPacket, false);
		++user_itr;
	}
}

void PgPvPLeagueImpl::SendToTeam(BM::GUID const & rkTeamGuid, BM::Stream const &rkPacket) const
{
	PgPvPLeague const* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
		if( pkLeague->GetTeamInfo(rkTeamGuid, kTeamInfo) )
		{
			SendToUser(kTeamInfo.kContUserGuid, rkPacket);
		}
	}
}

bool PgPvPLeagueImpl::InitLeague(BM::GUID const & rkLeagueID)
{
	CONT_LEAGUE::iterator league_itr = m_kContLeague.find(rkLeagueID);
	if( m_kContLeague.end() != league_itr )
	{
		CONT_LEAGUE::mapped_type& kLeague = league_itr->second;
		BuildTime();
		if( !m_kContEvent.empty() )
		{
			SPvPLeague_Event kEvent = m_kContEvent.front();
			if( !kLeague.CheckTournament(kEvent) )
			{// 이전 토너먼트가 정상적으로 진행되지 않은 경우다
				VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__ << _T("Diff LeagueLevel"));
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Diff LeagueLevel"));
				//return false;
			}
		}

		kLeague.Init();
		kLeague.NfyLobby_ChangeState();
		m_kCurLeagueID = rkLeagueID;
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Init Failed, League GUID : ") << rkLeagueID );
	return false;
}

bool PgPvPLeagueImpl::LoadDB(BM::GUID const & rkLeagueGuid, BM::Stream& rkPacket)
{
	PgPvPLeague* pkLeague = GetLeague(rkLeagueGuid);
	if( pkLeague && pkLeague->LoadDB(rkPacket) )
	{
		return true;
	}

	return false;
}

bool PgPvPLeagueImpl::CheckNewLeage(CONT_DEF_PVPLEAGUE const & rkContLeagueInfo, BM::GUID const & rkLeagueGuid) const
{
	if( !rkContLeagueInfo.empty() && rkLeagueGuid.IsNotNull() )
	{
		CONT_DEF_PVPLEAGUE::const_iterator league_itr = rkContLeagueInfo.find(rkLeagueGuid);
		if( rkContLeagueInfo.end() != league_itr )
		{
			if( PVPLS_END != league_itr->second.iLeagueState )
			{
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
				return false;
			}
		}
	}

	return true;
}

bool PgPvPLeagueImpl::BuildTime()
{
	SYSTEMTIME kNowDateTime;
	::GetLocalTime( &kNowDateTime );
	__int64 const i64NowTimeInDay = g_kEventView.GetLocalSecTimeInDay( CGameTime::DEFAULT );

	{
		decltype(m_kContEvent) tmpQueue;
		std::swap(m_kContEvent, tmpQueue);
	}

	const CONT_DEF_PVPLEAGUE_TIME* pkContDefPvPLeagueTime = NULL;
	g_kTblDataMgr.GetContDef(pkContDefPvPLeagueTime);
	CONT_DEF_PVPLEAGUE_TIME::const_iterator time_itr = pkContDefPvPLeagueTime->begin();
	while(pkContDefPvPLeagueTime->end() != time_itr)
	{
		if( 0 != time_itr->i64EndTime && 0 != time_itr->i64BeginTime
			&& time_itr->iDayOfWeek == kNowDateTime.wDayOfWeek 
			&& i64NowTimeInDay < time_itr->i64BeginTime )
		{
			SPvPLeague_Event kEvent;
			switch( time_itr->iLeagueState )
			{
			case PVPLS_JOIN:
				{
					kEvent.i64Time = time_itr->i64BeginTime;
					kEvent.eEvent = PVPLE_JOIN_START;
					m_kContEvent.push(kEvent);

					kEvent.i64Time = time_itr->i64EndTime;
					kEvent.eEvent = PVPLE_JOIN_END;
					m_kContEvent.push(kEvent);
				}break;
			case PVPLS_GAME:
				{					
					kEvent.i64Time = time_itr->i64BeginTime - (CGameTime::MINUTE * 30);
					kEvent.eEvent = PVPLE_NFY_GMAE_READY;
					kEvent.iLevel = time_itr->iLeagueLevel;
					m_kContEvent.push(kEvent);

					kEvent.i64Time = time_itr->i64BeginTime - (CGameTime::MINUTE * 10);
					kEvent.eEvent = PVPLE_GAME_READY;
					kEvent.iLevel = time_itr->iLeagueLevel;
					m_kContEvent.push(kEvent);

					kEvent.i64Time = time_itr->i64BeginTime;
					kEvent.eEvent = PVPLE_GAME_START;
					kEvent.iLevel = time_itr->iLeagueLevel;
					m_kContEvent.push(kEvent);

					kEvent.i64Time = time_itr->i64EndTime;
					kEvent.eEvent = PVPLE_GAME_END;
					kEvent.iLevel = time_itr->iLeagueLevel;
					m_kContEvent.push(kEvent);
				}break;
			default : //PVPLL_CLOSE
				{
				}break;
			}
		}
		++time_itr;
	}

	return true;
}

PgPvPLeague* PgPvPLeagueImpl::GetLeague(BM::GUID const & rkLeagueGuid)
{
	if( !m_kContLeague.empty() )
	{
		CONT_LEAGUE::iterator league_itr = m_kContLeague.find(rkLeagueGuid);
		if( m_kContLeague.end() != league_itr )
		{
			return &(league_itr->second);
		}
	} 

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

PgPvPLeague const* PgPvPLeagueImpl::GetLeague(BM::GUID const & rkLeagueGuid) const
{
	if( !m_kContLeague.empty() )
	{
		CONT_LEAGUE::const_iterator league_itr = m_kContLeague.find(rkLeagueGuid);
		if( m_kContLeague.end() != league_itr )
		{
			return &(league_itr->second);
		}
	} 

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL"));
	return NULL;
}

void PgPvPLeagueImpl::ProcessEvent(ePvPLeagueEvent eEvent)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( !pkLeague )
	{
		return;
	}

	switch( eEvent )
	{
	case PVPLE_JOIN_START:
		{
			pkLeague->SetJoinStart();
		}break;
	case PVPLE_JOIN_END:
		{
			SYSTEMTIME kNowTime;
			::GetLocalTime( &kNowTime );
			if( pkLeague->SetJoinEnd() && IsJoinEndDay(kNowTime.wDayOfWeek) )
			{
				int iLeagueLevel = pkLeague->GetLeagueLevel();
				do
				{
					TBL_DEF_PVPLEAGUE_TIME kBattleTimeInfo;
					if( GetNextBattleTime(iLeagueLevel, kBattleTimeInfo) )
					{
						if( pkLeague->CreateTournament(kBattleTimeInfo, kNowTime) )
						{
							if( pkLeague->IsAllAutoWin() )
							{//recreate tournament
								++iLeagueLevel;
								continue;
							}
							else
							{//create success
								pkLeague->NfyLobby_ChangeState();
								pkLeague->InitTeamInfo();
								pkLeague->NfyMessage(PVPLM_CREATE_TOURNAMENT);
								INFO_LOG(BM::LOG_LV5, __FL__ << _T("Create Tournament Success! LeagueLevel : ") << pkLeague->GetLeagueLevel() );
							}
						}
					}
					break;
				}while( iLeagueLevel );
			}
		}break;
	case PVPLE_NFY_GMAE_READY:
		{
			if( pkLeague->SetGameReady() )
			{
				pkLeague->ReqLobby_CreateRoom();
			}
		}break;
	case PVPLE_GAME_READY:
		{
			pkLeague->NfyLobby_ChangeState();
			pkLeague->NfyMessage(PVPLM_INVITE);
		}break;
	case PVPLE_GAME_START:
		{
			if( pkLeague->SetGameStart() )
			{
				BM::Stream kPacket(PT_N_T_REQ_START_PVPLEAGUE_GAME);
				::SendToChannelContents( CProcessConfig::GetPublicChannel(), PMET_PVP_LEAGUE, kPacket, PvP_Lobby_GroundNo_League );
			}
		}break;
	case PVPLE_GAME_END:
		{
			if( pkLeague->SetGameEnd() )
			{
				BM::Stream kPacket(PT_N_T_REQ_END_PVPLEAGUE_GAME);
				::SendToChannelContents( CProcessConfig::GetPublicChannel(), PMET_PVP_LEAGUE, kPacket, PvP_Lobby_GroundNo_League );

				pkLeague->NfyMessage(PVPLM_GAME_END);
			}
		}break;
	}
}

bool PgPvPLeagueImpl::GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM &rkOutTeamInfo)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague && pkLeague->GetTeamInfo(rkTeamGuid, rkOutTeamInfo) )
	{
		return true;
	}

	return false;
}

HRESULT PgPvPLeagueImpl::RegistTeam(BM::GUID const &rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		return pkLeague->RegistTeam(rkTeamGuid, rkTeamInfo);
	}

	return E_FAIL;
}

bool PgPvPLeagueImpl::GetTeamID(BM::GUID const & rkUserGuid, BM::GUID &rkOutTeamGuid) const
{
	PgPvPLeague const *pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		return pkLeague->GetTeamID(rkUserGuid, rkOutTeamGuid);
	}

	return false;
}

bool PgPvPLeagueImpl::ReleaseTeam(BM::GUID const &rkTeamGuid)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		return pkLeague->ReleaseTeam(rkTeamGuid);
	}

	return false;
}

bool PgPvPLeagueImpl::SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague && iLeagueLevel )
	{
		pkLeague->SetTeamLevel(iLeagueLevel, rkTeamGuid);
		return true;
	}

	return false;
}

bool PgPvPLeagueImpl::NfyMessage(EPvPLeagueMsg const eMessageType) const
{
	PgPvPLeague const *pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		pkLeague->NfyMessage(eMessageType);
	}

	return false;
}

bool PgPvPLeagueImpl::NfyLobby_ChangeState() const
{
	PgPvPLeague const *pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		pkLeague->NfyLobby_ChangeState();
	}

	return false;
}

bool PgPvPLeagueImpl::SetBattleResult(BM::GUID const rkBattleGuid, int const iWinTeam)
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{//결과 처리
		return pkLeague->SetBattleResult(rkBattleGuid, iWinTeam);
	}

	return false;
}

void PgPvPLeagueImpl::EndBattle()
{
	PgPvPLeague* pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		//보상을 주자
		pkLeague->SendBattleReward();

		//결승전인지 검사
		const CONT_DEF_PVPLEAGUE_TIME* pkContDefPvPLeagueTime = NULL;
		g_kTblDataMgr.GetContDef(pkContDefPvPLeagueTime);
		if( pkContDefPvPLeagueTime && !pkContDefPvPLeagueTime->empty() )
		{
			CONT_DEF_PVPLEAGUE_TIME::const_reverse_iterator time_itr = pkContDefPvPLeagueTime->rbegin();
			if( pkLeague->GetLeagueLevel() == time_itr->iLeagueLevel )
			{//리그 종료
				pkLeague->SetState(PVPLS_END);
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_SET_PVPLEAGUE_STATE, _T("EXEC [dbo].[up_Set_PvPLeague_State]"));
				kQuery.PushStrParam(pkLeague->GetID());
				kQuery.PushStrParam(pkLeague->GetState());
				g_kCoreCenter.PushQuery(kQuery, true);
				return;
			}
		}

		TBL_DEF_PVPLEAGUE_TIME kBattleTimeInfo;
		if( GetNextBattleTime(pkLeague->GetLeagueLevel(), kBattleTimeInfo) )
		{//다음 토너먼트 생성
			SYSTEMTIME kNow;
			::GetLocalTime( &kNow );
			pkLeague->CreateTournament(kBattleTimeInfo, kNow);
		}

		//로비에 알림
		pkLeague->NfyLobby_ChangeState();
	}
}

void PgPvPLeagueImpl::WriteLeagueInfo(BM::Stream &rkPacket) const
{
	PgPvPLeague const *pkLeague = GetLeague(m_kCurLeagueID);
	if( pkLeague )
	{
		pkLeague->WriteLeagueInfo(rkPacket);
	}
}

bool PgPvPLeagueImpl::GetCurLeagueID(BM::GUID &rkLeagueGuid) const
{
	if( m_kCurLeagueID.IsNotNull() )
	{
		rkLeagueGuid = m_kCurLeagueID;
		return true;
	}

	return false;
}

//--------------------------------------------------------------
//-- TWrapper :: PgPvPLeagueMgr
//--------------------------------------------------------------
PgPvPLeagueMgr::PgPvPLeagueMgr(void)
{
}

PgPvPLeagueMgr::~PgPvPLeagueMgr(void)
{
}

//--------------------------------------------------------------
// League

bool PgPvPLeagueMgr::CheckNewLeage(CONT_DEF_PVPLEAGUE const & rkContLeagueInfo, BM::GUID const & rkLeagueGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->CheckNewLeage(rkContLeagueInfo, rkLeagueGuid);
}

bool PgPvPLeagueMgr::AddLeague(CONT_DEF_PVPLEAGUE& rkContLeague)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->AddLeague(rkContLeague);
}

bool PgPvPLeagueMgr::AddLeague(BM::GUID const & rkLeagueID, CONT_DEF_PVPLEAGUE::mapped_type const & rkLeagueElement)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->AddLeague(rkLeagueID, rkLeagueElement);
}

bool PgPvPLeagueMgr::InitLeague(BM::GUID const & rkLeagueID)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->InitLeague(rkLeagueID);
}

void PgPvPLeagueMgr::WriteLeagueInfo(BM::Stream &rkPacket) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->WriteLeagueInfo(rkPacket);
}

bool PgPvPLeagueMgr::GetCurLeagueID(BM::GUID &rkLeagueGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->GetCurLeagueID(rkLeagueGuid);
}

//--------------------------------------------------------------
// Team

bool PgPvPLeagueMgr::GetTeamID(BM::GUID const & rkUserGuid, BM::GUID &rkOutTeamGuid) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->GetTeamID(rkUserGuid, rkOutTeamGuid);
}

bool PgPvPLeagueMgr::GetTeamInfo(BM::GUID const & rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM &rkOutTeamInfo)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->GetTeamInfo(rkTeamGuid, rkOutTeamInfo);
}

HRESULT PgPvPLeagueMgr::RegistTeam(BM::GUID const &rkTeamGuid, TBL_DEF_PVPLEAGUE_TEAM const & rkTeamInfo)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->RegistTeam(rkTeamGuid, rkTeamInfo);
}

bool PgPvPLeagueMgr::ReleaseTeam(BM::GUID const &rkTeamGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->ReleaseTeam(rkTeamGuid);
}

bool PgPvPLeagueMgr::SetTeamLevel(int const iLeagueLevel, BM::GUID const &rkTeamGuid)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->SetTeamLevel(iLeagueLevel, rkTeamGuid);
}

//--------------------------------------------------------------
// Battle

bool PgPvPLeagueMgr::SetBattleResult(BM::GUID const rkBattleGuid, int const iWinTeam)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->SetBattleResult(rkBattleGuid, iWinTeam);
}

void PgPvPLeagueMgr::EndBattle()
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->EndBattle();
}

//--------------------------------------------------------------
// Database

bool PgPvPLeagueMgr::LoadDB(BM::GUID const & rkLeagueGuid, BM::Stream& rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->LoadDB(rkLeagueGuid, rkPacket);
}


//--------------------------------------------------------------
// Broadcast

void PgPvPLeagueMgr::SendToUser(CONT_LEAGUE_USER const &rkContUser, BM::Stream const &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->SendToUser(rkContUser, rkPacket);
}
void PgPvPLeagueMgr::SendToTeam(BM::GUID const & rkTeamGuid, BM::Stream const &rkPacket)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->SendToTeam(rkTeamGuid, rkPacket);
}

bool PgPvPLeagueMgr::NfyMessage(EPvPLeagueMsg const eMessageType) const
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->NfyMessage(eMessageType);
}

void PgPvPLeagueMgr::NfyLobby_ChangeState() const
{
	BM::CAutoMutex kLock(m_kMutex);
	Instance()->NfyLobby_ChangeState();
}

//--------------------------------------------------------------
// Event Process

void PgPvPLeagueMgr::ProcessEvent(ePvPLeagueEvent eEvent)
{
	BM::CAutoMutex kLock(m_kMutex);
	return Instance()->ProcessEvent(eEvent);
}


//--------------------------------------------------------------
// GM Command

void PgPvPLeagueMgr::ProcessGMCommand(BM::Stream& rkPacket)
{
	EGMCmdType eGodCommandType;
	BM::GUID kCommandOwnerGuid;
	if( rkPacket.Pop(eGodCommandType) && rkPacket.Pop(kCommandOwnerGuid) )
	{
		switch( eGodCommandType )
		{
		case GMCMD_PVPLEAGUE_SETEVENT:
			{
				ePvPLeagueEvent eEvent = PVPLE_NONE;
				if( rkPacket.Pop(eEvent) )
				{
					ProcessEvent(eEvent);
				}
			}break;
		default:
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Wrong GodCommandType: "<<eGodCommandType);
				return;
			}break;
		}
	}
}

//--------------------------------------------------------------
// Packet Process

bool PgPvPLeagueMgr::ProcessMsg(BM::Stream& rkPacket)
{
	BM::Stream::DEF_STREAM_TYPE usType = 0;
	rkPacket.Pop( usType );

	switch( usType )
	{
	case PT_A_N_REQ_LOAD_PVPLEAGUE:
		{
			CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LOAD_PVPLEAGUE, _T("EXEC [dbo].[up_Load_PvPLeague]"));
			g_kCoreCenter.PushQuery(kQuery, true);
		}break;
	case PT_N_N_NFY_RELOAD_PVPLEAGUE:
		{
			CONT_DEF_PVPLEAGUE kContPvPLeague;
			PU::TLoadTable_AM(rkPacket, kContPvPLeague);
			BM::GUID kCurLeagueID;
			int iSession;
			rkPacket.Pop(kCurLeagueID);
			rkPacket.Pop(iSession);

			if( CheckNewLeage(kContPvPLeague, kCurLeagueID) )
			{//insert new league
				++iSession;
				if( MAX_SESSION_COUNT < iSession )
				{
					iSession = 1;
				}
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_INSERT_PVPLEAGUE, _T("EXEC [dbo].[up_Insert_PvPLeague]"));
				kQuery.PushStrParam(iSession);
				kQuery.contUserData.Push(iSession);
				g_kCoreCenter.PushQuery(kQuery, true);
			}
			else
			{//load league data
				CONT_DEF_PVPLEAGUE::const_iterator itr = kContPvPLeague.find(kCurLeagueID);
				if( kContPvPLeague.end() != itr )
				{
					if( AddLeague(kContPvPLeague) )
					{
						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_LOAD_PVPLEAGUE_SUB, _T("EXEC [dbo].[up_Load_PvPLeague_sub]"));
						kQuery.PushStrParam(kCurLeagueID);
						kQuery.contUserData.Push(kCurLeagueID);
						g_kCoreCenter.PushQuery(kQuery, true);
					}
				}
			}
		}break;
	case PT_N_N_NFY_INIT_PVPLEAGUE:
		{
			BM::GUID kLeagueGuid;
			TBL_DEF_PVPLEAGUE kLeagueInfo;
			rkPacket.Pop(kLeagueGuid);
			rkPacket.Pop(kLeagueInfo);

			if( AddLeague(kLeagueGuid, kLeagueInfo) )
			{
				InitLeague(kLeagueGuid);
			}
		}break;
	case PT_N_N_NFY_UPDATE_PVPLEAGUE:
		{
			BM::GUID kLeagueGuid;
			rkPacket.Pop(kLeagueGuid);

			if( LoadDB(kLeagueGuid, rkPacket) )
			{
				InitLeague(kLeagueGuid);
			}
			else
			{
				VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Data Load Failed!! League GUID : " << kLeagueGuid );
				return false;
			}
		}break;
	case PT_C_N_REQ_REGIST_PVPLEAGUE_TEAM:
		{//클라가 팀 등록을 요청했다.
			BM::GUID kReqUserGuid;
			rkPacket.Pop( kReqUserGuid );

			TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
			kTeamInfo.ReadFromPacket(rkPacket);

			BM::GUID kTeamGuid;
			kTeamGuid.Generate();
			HRESULT hResult = RegistTeam(kTeamGuid, kTeamInfo);

			BM::GUID kLeagueGuid;
			if( E_PVPLEAGUE_REGIST_TEAM_SUCCESS == hResult && GetCurLeagueID(kLeagueGuid) )
			{//성공이다 저장하자
				CEL::DB_QUERY kQuery(DT_PLAYER, DQT_INSERT_PVPLEAGUE_TEAM, _T("EXEC [dbo].[up_Insert_PvPLeague_Team]"));
				kQuery.PushStrParam(kLeagueGuid);
				kQuery.PushStrParam(kTeamGuid);
				kQuery.PushStrParam(kTeamInfo.wstrTeamName);
				kQuery.PushStrParam(kTeamInfo.iTournamentIndex);

				CONT_LEAGUE_USER::const_iterator user_itr = kTeamInfo.kContUserGuid.begin();
				while( kTeamInfo.kContUserGuid.end() != user_itr )
				{
					kQuery.PushStrParam(user_itr->kGuid);
					++user_itr;
				}
				size_t const iCount = 4 - kTeamInfo.kContUserGuid.size();
				for(size_t i=0; i<iCount; ++i)
				{
					kQuery.PushStrParam(BM::GUID::NullData());
				}
				kQuery.contUserData.Push(kReqUserGuid);
				g_kCoreCenter.PushQuery(kQuery, true);
				return true;
			}
			else
			{
				//실패했소.
				BM::Stream kAnsPacket(PT_N_C_ANS_REGIST_PVPLEAGUE_TEAM, hResult);
				SendToUser(kTeamInfo.kContUserGuid, kAnsPacket);
			}
		}break;
	case PT_N_N_NFY_REGIST_PVPLEAGUE_TEAM:
		{//팀 등록 쿼리 결과
			BM::GUID kTeamGuid;
			BM::GUID kReqUserGuid;
			rkPacket.Pop(kTeamGuid);
			rkPacket.Pop(kReqUserGuid);

			BM::Stream kAnsPacket(PT_N_C_ANS_REGIST_PVPLEAGUE_TEAM);
			HRESULT hResult = E_PVPLEAGUE_REGIST_TEAM_ERROR_SYSTEM;
			if( kTeamGuid.IsNotNull() )
			{
				TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
				if( GetTeamInfo(kTeamGuid, kTeamInfo) )
				{
					hResult = E_PVPLEAGUE_REGIST_TEAM_SUCCESS;
					kAnsPacket.Push(hResult);
					kAnsPacket.Push(kReqUserGuid);
					kAnsPacket.Push(kTeamInfo.wstrTeamName);
					SendToTeam(kTeamGuid, kAnsPacket);
					return true;
				}
			}
			hResult = E_PVPLEAGUE_REGIST_TEAM_ERROR_SYSTEM;
			kAnsPacket.Push(hResult);
			kAnsPacket.Push(kReqUserGuid);
			SendToTeam(kTeamGuid, kAnsPacket);
		}break;
	case PT_C_N_REQ_GIVEUP_PVPLEAGUE_TEAM:
		{// 팀 포기 요청
			BM::GUID kReqUserGuid;
			rkPacket.Pop(kReqUserGuid);

			BM::GUID kTeamGuid;
			if( GetTeamID(kReqUserGuid, kTeamGuid) )
			{
				TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
				if( GetTeamInfo(kTeamGuid, kTeamInfo) )
				{
					if( ReleaseTeam(kTeamGuid) )
					{
						CEL::DB_QUERY kQuery(DT_PLAYER, DQT_DELETE_PVPLEAGUE_TEAM, _T("EXEC [dbo].[up_Delete_PvPLeague_Team]"));
						kQuery.PushStrParam(kTeamGuid);
						kQuery.contUserData.Push(kReqUserGuid);
						kTeamInfo.WriteToPacket(kQuery.contUserData);
						g_kCoreCenter.PushQuery(kQuery, true);
					}
				}
			}
		}break;
	case PT_N_N_NFY_GIVEUP_PVPLEAGUE_TEAM:
		{//팀 삭제 쿼리 결과
			BM::GUID kReqUserGuid;
			TBL_DEF_PVPLEAGUE_TEAM kTeamInfo;
			rkPacket.Pop(kReqUserGuid);
			kTeamInfo.ReadFromPacket(rkPacket);

			BM::Stream kAnsPacket(PT_N_C_ANS_GIVEUP_PVPLEAGUE_TEAM);
			kAnsPacket.Push( kReqUserGuid );
			kAnsPacket.Push( true );
			SendToUser(kTeamInfo.kContUserGuid, kAnsPacket);
		}break;
	case PT_C_N_REQ_QUERY_PVPLEAGUE_TEAM:
		{//팀 등록 여부 질의
			BM::GUID kReqUserGuid;
			rkPacket.Pop(kReqUserGuid);

			BM::GUID kTeamGuid;
			bool bIsRegist = GetTeamID(kReqUserGuid, kTeamGuid);
				
			BM::Stream kAnsPacket(PT_N_C_ANS_QUERY_PVPLEAGUE_TEAM);
			kAnsPacket.Push( bIsRegist );
			g_kRealmUserMgr.Locked_SendToUser(kReqUserGuid, kAnsPacket, false);
		}break;
	case PT_T_N_ANS_CREATE_PVPLEAGUE_ROOM:
		{//로비의 방 생성 요청 응답
			bool bIsSuccess;
			rkPacket.Pop(bIsSuccess);
			if( bIsSuccess )
			{//로비의 준비가 끝났다. 유저들에게 초대 준비 메시지를 보내자
				NfyMessage(PVPLM_READY);
				NfyMessage(PVPLM_AUTOWIN);
			}
		}break;
	case PT_T_N_ANS_START_PVPLEAGUE_GAME:
		{
			bool bIsEmpty = false;
			VEC_GUID kContBattleID;
			rkPacket.Pop(bIsEmpty);
			if( !bIsEmpty )
			{
				rkPacket.Pop(kContBattleID);
				if( !kContBattleID.empty() )
				{//겜 시작 실패한 넘들 처리
					VEC_GUID::const_iterator guid_itr = kContBattleID.begin();
					while( kContBattleID.end() != guid_itr )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Cannot Start Game, BattleGuid: "<< (*guid_itr) );
						++guid_itr;
					}
				}
			}
		}break;
	case PT_T_N_NFY_RESULT_PVPLEAGUE_GAME:
		{//로비가 게임 결과를 보내줬다.
			BM::GUID kBattleGuid;
			int iWinTeam=0, iRoomIndex=0;
			rkPacket.Pop(kBattleGuid);
			rkPacket.Pop(iWinTeam);
			rkPacket.Pop(iRoomIndex);

			SetBattleResult(kBattleGuid, iWinTeam);

			BM::Stream kPacket(PT_N_C_NFY_CLOSE_PVPLEAGUE_GAME);
			kPacket.Push(kBattleGuid);
			kPacket.Push(iRoomIndex);
			::SendToChannelContents( CProcessConfig::GetPublicChannel(), PMET_PVP_LEAGUE, kPacket, PvP_Lobby_GroundNo_League );
		}break;
	case PT_T_N_ANS_END_PVPLEAGUE_GAME:
		{//경기가 모두 끝났다.
			bool bIsEmpty = false;
			VEC_GUID kContBattleID;
			rkPacket.Pop(bIsEmpty);
			if( !bIsEmpty )
			{
				rkPacket.Pop(kContBattleID);
				if( !kContBattleID.empty() )
				{//진행되지 못한 경기들
					VEC_GUID::const_iterator guid_itr = kContBattleID.begin();
					while( kContBattleID.end() != guid_itr )
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Cannot Play PvP_League Game, BattleGuid: "<< (*guid_itr) );
						++guid_itr;
					}
				}
			}

			EndBattle();
		}break;
	case PT_N_N_NFY_PVPLEAGUE_TEAMLEVEL:
		{
			int iLeagueLevel = 0;
			BM::GUID kTeamGuid;
			rkPacket.Pop(iLeagueLevel);
			
			rkPacket.Pop(kTeamGuid);
			SetTeamLevel(iLeagueLevel, kTeamGuid);

			rkPacket.Pop(kTeamGuid);
			SetTeamLevel(iLeagueLevel, kTeamGuid);
		}break;
	case PT_T_N_REQ_CHANGE_STATE_LEAGUE_LOBBY:
		{
			NfyLobby_ChangeState();
		}break;
	case PT_A_N_REQ_PVPLEAGUE_GM_COMMAND:
		{
			ProcessGMCommand(rkPacket);
		}break;
	default:
		{
			VERIFY_INFO_LOG(false, BM::LOG_LV1, L"Wrong PgPvPLeague packet type:"<<usType);
			return false;
		}break;
	}
	return true;
}

void PgPvPLeagueMgr::OnTick()
{
	BM::CAutoMutex kLock(m_kMutex);
	Instance()->OnTick();
}