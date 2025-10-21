#include "stdafx.h"
#include "PgAction.h"
#include "PgWarGround.h"
#include "PgPvPType_DeathMatch.h"
#include "PgActionAchievement.h"
#include "PgEffectAbilTable.h"

PgPvPType_DeathMatch::PgPvPType_DeathMatch()
:	m_kMode(PVP_MODE_TEAM)
,	m_kOption(E_PVP_OPT_NONE)
,	m_ucNowRound(0)
,	m_ucMaxRound(0)
,	m_iRoomIndex(-1)
,	m_iLobbyID(PvP_Lobby_GroundNo_Exercise)
{

}

PgPvPType_DeathMatch::~PgPvPType_DeathMatch()
{
}

HRESULT PgPvPType_DeathMatch::Init( BM::Stream &kPacket, PgWarGround* pkGnd )
{
	PgWarMode::Init( kPacket, pkGnd );

	m_kContReady.clear();
	m_kContPlay.clear();

	kPacket.Pop(m_kOption);
	kPacket.Pop(m_iLobbyID);
	kPacket.Pop(m_iRoomIndex);
	kPacket.Pop(m_dwGamePlayTime);
	kPacket.Pop(m_kGamePoint);
	kPacket.Pop(m_kMode);
	kPacket.Pop(m_ucMaxRound);
	PU::TLoadTable_AM( kPacket, m_kContReady );

	InitTeamResult();

	if ( !m_ucMaxRound )
	{
		m_ucMaxRound = 1;
	}
	m_ucNowRound = 0;
	return S_OK;
}

void PgPvPType_DeathMatch::InitTeamResult()
{
	m_kContTeamResult.clear();

	if ( m_kMode == PVP_MODE_TEAM )
	{
		int iTotalLevel = 0;

		CONT_PVP_GAME_USER::const_iterator itr = m_kContReady.begin();
		for ( ; itr!=m_kContReady.end(); ++itr )
		{
			m_kContTeamResult.insert( std::make_pair( itr->second.kTeamSlot.GetTeam(), SPvPTeamResult()) );

			iTotalLevel += static_cast<int>(itr->second.sLevel);
		}

		if ( E_PVP_OPT_USEBATTLELEVEL & m_kOption )
		{
			int const iTotalPlayerCount = static_cast<int>(m_kContReady.size());
			if ( 0 < iTotalPlayerCount )
			{
				int iBalanceLevel = iTotalLevel / iTotalPlayerCount;
				int iAddLevel = 5 - (iBalanceLevel % 5);
				m_iBattleLevel = iBalanceLevel + iAddLevel;// 5의 배수로 끊어야 한다.
			}
		}
		else
		{
			m_iBattleLevel = 0;
		}
	}
}

bool PgPvPType_DeathMatch::GetReadyUnitList( VEC_GUID& kUnitList )const
{
	kUnitList.reserve( m_kContReady.size() );
	CONT_PVP_GAME_USER::const_iterator ready_itr = m_kContReady.begin();
	for ( ; ready_itr!=m_kContReady.end(); ++ready_itr )
	{
		kUnitList.push_back( ready_itr->first );
	}
	return !kUnitList.empty();
}
bool PgPvPType_DeathMatch::IsStart( DWORD const dwNowTime )
{
	return m_kContReady.empty();
}

bool PgPvPType_DeathMatch::IsJoin( PgPlayer *pkPlayer )
{
	CONT_PVP_GAME_USER::const_iterator itr = m_kContReady.find( pkPlayer->GetID() );
	return itr != m_kContReady.end();
}

HRESULT PgPvPType_DeathMatch::Join( PgPlayer *pkPlayer, bool const bInturde )
{
	if ( pkPlayer )
	{
		CONT_PVP_GAME_USER::iterator itr = m_kContReady.find( pkPlayer->GetID() );
		if ( itr != m_kContReady.end() )
		{
			CONT_PVP_GAME_USER::mapped_type &kElement = itr->second;
//			int const iTeam = kElement.kTeamSlot.GetTeam();

			if ( E_PVP_OPT_DISABLEDASHJUMP & m_kOption )
			{
				// 대쉬 점프 금지
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_PENALTY;
				kCreate.iEffectNum = EFFECTNO_PVP_NOTDASH;
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				pkPlayer->AddEffect( kCreate );
			}
			else
			{
				pkPlayer->DeleteEffect( EFFECTNO_PVP_NOTDASH );
			}

			if ( E_PVP_OPT_USEHANDYCAP & m_kOption )
			{
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_PENALTY;
				kCreate.iEffectNum = EFFECTNO_PVP_HANDYCAP;
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				pkPlayer->AddEffect( kCreate );
			}
			else
			{
				pkPlayer->DeleteEffect( EFFECTNO_PVP_HANDYCAP );
			}
							
			kElement.iMaxHP = pkPlayer->GetAbil(AT_C_MAX_HP);
			kElement.iMaxMP = pkPlayer->GetAbil(AT_C_MAX_MP);
			pkPlayer->SetAbil( AT_HPMP_BROADCAST, 1, false, false );

// 			if ( m_kMode == PVP_MODE_TEAM )
// 			{
// 				SPvPTeamResult *pkRet = GetTeamResult(iTeam);
// 				if ( pkRet )
// 				{
// 					PgAction_PartyGuid kAction( PARTY_SYS_PVP, GATTR_PVP, pkRet->kTeamGuid );
// 					kAction.DoAction( pkPlayer, NULL );
// 				}
// 			}
			
			m_kContPlay.insert( std::make_pair( itr->first, kElement ) );
			m_kContReady.erase(itr);// 레디상태에서는 지워준다.
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

void PgPvPType_DeathMatch::RefreshUserCount()
{
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::iterator team_itr = m_kContTeamResult.begin();
		for( ; team_itr!=m_kContTeamResult.end(); ++team_itr )
		{
			team_itr->second.iPlayUserCount = 0;
		}

		CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.begin();
		for ( ; itr!= m_kContPlay.end(); ++itr )
		{
			SPvPTeamResult *pkRet = GetTeamResult(itr->second.kTeamSlot.GetTeam());
			if ( pkRet )
			{
				++(pkRet->iPlayUserCount);
			}
		}
	}
}

bool PgPvPType_DeathMatch::Exit( BM::GUID const &kCharGuid )
{
	if ( m_kContPlay.erase( kCharGuid ) )
	{
		RefreshUserCount();
	}

	return 1 == m_kContReady.erase( kCharGuid );
}


bool PgPvPType_DeathMatch::Start( PgPlayer *pkPlayer, PgWarGround * pkGnd )
{
	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.find(pkPlayer->GetID());
	if ( itr != m_kContPlay.end() )
	{
		int const iTeam = itr->second.kTeamSlot.GetTeam();
		pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
		pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
		pkGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pkPlayer), 0, true, iTeam );

		pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPType_DeathMatch::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	// 이긴팀 계산을 해야 한다.
	int iRoundWinTeam = TEAM_NONE;
	m_bEndGame = CalcRoundWinTeam( iRoundWinTeam );

	BM::Stream kEndPacket( PT_M_C_NFY_GAME_END, bAutoChange );
	kEndPacket.Push( iRoundWinTeam );
	kEndPacket.Push( m_bEndGame );
	pkGnd->Broadcast( kEndPacket );

	return m_bEndGame;
}


void PgPvPType_DeathMatch::Modify( BM::Stream &kPacket )
{
	PU::TLoadTableAdd_AM( kPacket, m_kContReady );
}

bool PgPvPType_DeathMatch::CalcRoundWinTeam( int &iOutRoundWinTeam )
{
	bool bEndGame = false;
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::iterator red_itr = m_kContTeamResult.find(TEAM_RED);
		if ( red_itr!=m_kContTeamResult.end() )
		{
			CONT_PVP_TEAM_RESULT_INFO::iterator blue_itr = m_kContTeamResult.find(TEAM_BLUE);
			if ( blue_itr!=m_kContTeamResult.end() )
			{
				switch ( red_itr->second.IsWin( blue_itr->second ) )
				{
				case S_OK:
					{
						if ( ++(red_itr->second.ucWinCount) > (m_ucMaxRound/2) )
						{
							bEndGame = true;
						}
						iOutRoundWinTeam = TEAM_RED;
					}break;
				case E_FAIL:
					{
						if ( ++(blue_itr->second.ucWinCount) > (m_ucMaxRound/2) )
						{
							bEndGame = true;
						}
						iOutRoundWinTeam = TEAM_BLUE;
					}break;
				}
				blue_itr->second.Clear(true);
			}
			red_itr->second.Clear(true);
		}
	}
	else
	{
		iOutRoundWinTeam = TEAM_PERSONAL_CHK;
	}

	return bEndGame || (m_ucNowRound == m_ucMaxRound) || m_bNoEnemy;
}

int PgPvPType_DeathMatch::GetWinTeam( BYTE& kOutRedWin, BYTE& kOutBlueWin )const
{
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::const_iterator red_itr = m_kContTeamResult.find(TEAM_RED);
		if ( red_itr!=m_kContTeamResult.end() )
		{
			CONT_PVP_TEAM_RESULT_INFO::const_iterator blue_itr = m_kContTeamResult.find(TEAM_BLUE);
			if ( blue_itr!=m_kContTeamResult.end() )
			{
				kOutRedWin = red_itr->second.ucWinCount;
				kOutBlueWin = blue_itr->second.ucWinCount;

				if ( !blue_itr->second.iPlayUserCount )
				{
					return TEAM_RED;
				}

				if ( !red_itr->second.iPlayUserCount )
				{
					return TEAM_BLUE;
				}

				if ( kOutRedWin > kOutBlueWin )
				{
					return TEAM_RED;
				}

				if ( kOutBlueWin > kOutRedWin )
				{
					return TEAM_BLUE;
				}
			}
		}
		return TEAM_NONE;
	}
	return TEAM_PERSONAL_CHK;
}

HRESULT PgPvPType_DeathMatch::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
{
	if ( pkUnit )
	{
		BM::GUID const kKillerGuid = ( pkKiller ? pkKiller->GetID() : BM::GUID::NullData() );

		CONT_PVP_GAME_USER::iterator die_itr = m_kContPlay.find(pkUnit->GetID());
		if ( die_itr != m_kContPlay.end() )
		{
			int iDeadPoint = 0;
			int iKillPoint = 0;

			CONT_PVP_GAME_USER::iterator kill_itr = m_kContPlay.find(kKillerGuid);
			if ( kill_itr != m_kContPlay.end() )
			{
				// 순위를 구한다.
				int iDieRank = 1;
				int iBonusRank = 1;
				CONT_PVP_GAME_USER::const_iterator user_itr = m_kContPlay.begin();
				for ( ; user_itr!=m_kContPlay.end(); ++user_itr )
				{
					// 어차피 점수가 똑같으니까 내꺼인지 검사할 필요는 굳이 없다.if ( user_itr != die_itr )
					if ( user_itr->second.kResult > die_itr->second.kResult )
					{
						++iDieRank;
					}

					// 어차피 점수가 똑같으니까 내꺼인지 검사할 필요는 굳이 없다.if ( user_itr != kill_itr )
					if ( user_itr->second.kResult > kill_itr->second.kResult )
					{
						++iBonusRank;
					}
				}

				iBonusRank -= iDieRank;

				iDeadPoint = ms_kSetting.Get(this->Type(),PVP_ST_DEATH_POINT)+ (ms_kSetting.Get(this->Type(),PVP_ST_DEATH_POINT_WEIGHT) * iBonusRank);
				iKillPoint = ms_kSetting.Get(this->Type(),PVP_ST_KILL_POINT) + (ms_kSetting.Get(this->Type(),PVP_ST_KILL_POINT_WEIGHT) * iBonusRank);

				kill_itr->second.kResult.Kill( iKillPoint );
				SetEventTeam_Kill( kill_itr->second.kTeamSlot.GetTeam(), iKillPoint );
			}

			die_itr->second.kResult.Dead( iDeadPoint );
			SetEventTeam_Die( die_itr->second.kTeamSlot.GetTeam(), iDeadPoint );
				
			BM::Stream kPacket(PT_M_C_NFY_GAME_EVENT_KILL);
			kPacket.Push(pkUnit->GetID());
			kPacket.Push(iDeadPoint);
			kPacket.Push(kKillerGuid);
			kPacket.Push(iKillPoint);
			pkGnd->Broadcast( kPacket );
			return S_OK;
		}	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPType_DeathMatch::SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd )
{
	return S_OK;
}

HRESULT PgPvPType_DeathMatch::SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd )
{
	return S_OK;
}

HRESULT PgPvPType_DeathMatch::SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd )
{
	return S_OK;
}

HRESULT PgPvPType_DeathMatch::Update( DWORD const dwCurTime )
{
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::const_iterator team_itr;
		for ( team_itr=m_kContTeamResult.begin(); team_itr!=m_kContTeamResult.end(); ++team_itr )
		{
			if ( 0 == (team_itr->second.iPlayUserCount) )
			{
				m_bNoEnemy = true;
				LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
				return E_FAIL;
			}
		}
	}
	else
	{
		if ( 1 >= m_kContPlay.size() )
		{
			m_bNoEnemy = true;
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
			return E_FAIL;
		}
	}

	m_bNoEnemy = false;
	return S_OK;
}

void PgPvPType_DeathMatch::SetEventTeam_Kill( int const iTeam, int const iPoint )
{
	SPvPTeamResult *pkRet = GetTeamResult( iTeam );
	if ( pkRet )
	{
		pkRet->Kill(iPoint);
	}
}

void PgPvPType_DeathMatch::SetEventTeam_Die( int const iTeam, int const iPoint )
{
	SPvPTeamResult *pkRet = GetTeamResult( iTeam );
	if ( pkRet )
	{
		pkRet->Dead(iPoint);
	}
}

void PgPvPType_DeathMatch::StartGame( BM::Stream &kPacket, bool const bGame )
{
	RefreshUserCount();
	if ( bGame )
	{
		m_iWinTeam = TEAM_NONE;
		m_bEndGame = false;
		m_bNoEnemy = false;

		++m_ucNowRound;
	}
	kPacket.Push(m_ucNowRound);	
}

bool PgPvPType_DeathMatch::Send( BM::Stream& kPacket )const
{
	return SendToPvPRoom( m_iLobbyID, m_iRoomIndex, kPacket );
}

EResultEndType PgPvPType_DeathMatch::ResultEnd()
{
	if ( IsEndGame() )
	{
		BYTE kRedWin = 0;
		BYTE kBlueWin = 0;
		int const iWinTeam = GetWinTeam( kRedWin, kBlueWin );

		// 랭킹을 계산하고,
		// 점수를 계산하고,
		// 보상을 지급하고,
		// PvP Lobby로 결과를 알려준다.
		CONT_PVP_GAME_REWARD kReward;
		kReward.reserve(m_kContPlay.size());

		CONT_PVP_GAME_USER::const_iterator user_itr;
		for ( user_itr=m_kContPlay.begin(); user_itr!=m_kContPlay.end(); ++user_itr )
		{
			CONT_PVP_GAME_REWARD::value_type kElement( user_itr->first, user_itr->second.kResult );

			// Special Bonus Point Setting
			ms_kSetting.GetSpecial( this->Type(), kElement );
			kReward.push_back( kElement );
		}

		BM::Stream kNfyPacket( PT_A_G_NFY_PVP_REWARD, IsExercise() );
		kNfyPacket.Push( iWinTeam );
		PU::TWriteArray_M( kNfyPacket, kReward );
		kNfyPacket.Push( kRedWin );
		kNfyPacket.Push( kBlueWin );
		kNfyPacket.Push( Type() );
		kNfyPacket.Push( m_ucMaxRound );
		VNotify( &kNfyPacket );	
		return RESULT_END_CLOSE;
	}
	return RESULT_END_RESTART;
}

bool PgPvPType_DeathMatch::IsAbusing(void)const
{
	if ( !IsExercise() )
	{
		size_t const iAbusingKillCount = 15 * (m_kContPlay.size() / 2);

		BYTE byTemp1, byTemp2;
		int const iWinTeam = GetWinTeam( byTemp1, byTemp2 );

		size_t iChkKillCount[2] = {0,};

		CONT_PVP_GAME_USER::const_iterator user_itr = m_kContPlay.begin();
		for ( ; user_itr != m_kContPlay.end() ; ++user_itr )
		{
			if ( iWinTeam == user_itr->second.kTeamSlot.GetTeam() )
			{
				iChkKillCount[0] += static_cast<size_t>(user_itr->second.kResult.usKillCount);
			}
			else
			{
				iChkKillCount[1] += static_cast<size_t>(user_itr->second.kResult.usKillCount);
			}
		}

		if (	iAbusingKillCount <= iChkKillCount[0]
			||	iAbusingKillCount <= iChkKillCount[1]
			)
		{
			return true;
		}
	}

	return false;
}

int PgPvPType_DeathMatch::GetRewardID(void)const
{
	int const iAddID = ( IsExercise() ? 0 : 10000 );
	return iAddID + this->Type();
}
