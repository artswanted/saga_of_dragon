#include "stdafx.h"
#include "PgPvPRankingSceduler.h"

PgPvPRankingSceduler::PgPvPRankingSceduler(void)
:	m_iResult(E_UPDATE_ERROR)
{}

PgPvPRankingSceduler::~PgPvPRankingSceduler(void)
{}

void PgPvPRankingSceduler::SetScedule( SPvPRankingSeasonScedule const &kScedule )
{
	m_kScedule=kScedule;
}

bool PgPvPRankingSceduler::CheckUpdate(void)
{
	if ( true == m_kScedule.kBeginDate.IsNull() )
	{
		return true;
	}
	
	INFO_LOG( BM::LOG_LV6, L"==============================================================");
	INFO_LOG( BM::LOG_LV6, L"[PvP_Ranking_Schedule] Check Update Start");

	CEL::DB_QUERY kQuery( DT_PLAYER, DQT_PVP_RANK_UPDATE, _T("EXEC [dbo].[up_Check_PvPRankingSeason]"));
	kQuery.PushStrParam( m_kScedule.kBeginDate );
	kQuery.PushStrParam( m_kScedule.iSeason_InitPoint );
	kQuery.PushStrParam( m_kScedule.kCycle_Pass );
	kQuery.PushStrParam( m_kScedule.iCycleDay );
	kQuery.PushStrParam( m_kScedule.fCycle_PointRate );
	kQuery.PushStrParam( m_kScedule.iCycle_GivePoint );

	if( S_OK != g_kCoreCenter.PushQuery(kQuery, true) )
	{
		INFO_LOG( BM::LOG_LV1, L"Query Fail Data Recovery... Cause = " << kQuery.Command() );
		return false;
	}

	INFO_LOG( BM::LOG_LV6, L"[PvP_Ranking_Schedule] Check Update Finish");
	INFO_LOG( BM::LOG_LV6, L"---------------------------------------------------------------");
	return !IsError();
}

void PgPvPRankingSceduler::SetResult( int const iRet )
{
	switch( iRet )
	{
	case E_UPDATE_ERROR:
		{
			CAUTION_LOG( BM::LOG_LV1, L"[PvP_Ranking_Schedule] Update Error!!!");
			VERIFY_INFO_LOG( false, BM::LOG_LV1, L"[PvP_Ranking_Schedule] Update Error!!!" );
		}break;
	case E_UPDATE_NONE:
		{
		}break;
	case E_UPDATE_NEW_SEASON:
		{
			INFO_LOG( BM::LOG_LV5, L"[PvP_Ranking_Schedule] Begin New Season!!");
		}break;
	case E_UPDATE_NEW_CYCLE:
		{
			INFO_LOG( BM::LOG_LV5, L"[PvP_Ranking_Schedule] Begin New Cycle!!");
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV1, L"[PvP_Ranking_Schedule] Unknown Result = " << iRet );
			VERIFY_INFO_LOG( false, BM::LOG_LV1, L"[PvP_Ranking_Schedule] Unknown Result = " << iRet );
			m_iResult = E_UPDATE_ERROR;
			return;
		}break;
	}

	m_iResult = iRet;
}
