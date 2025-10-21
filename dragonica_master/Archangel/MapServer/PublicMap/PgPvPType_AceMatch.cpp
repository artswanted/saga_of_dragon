#include "stdafx.h"
#include "PgWarGround.h"
#include "PgAction.h"
#include "PgPvPType_AceMatch.h"
#include "PgActionAchievement.h"

bool PgPvPType_AceMatch::SortEntry(SPvPGameUser const &lhs, SPvPGameUser const &rhs)
{
	if( lhs.byEntryNo > rhs.byEntryNo )
	{
		return false;
	}
	else
	{
		return true;
	}
}

PgPvPType_AceMatch::PgPvPType_AceMatch()
{
	m_iWinTeam = TEAM_NONE;
	m_kContUserEntry.clear();
	m_kContPvPResult.clear();
	m_bEndRound = false;
	m_dwWaitBeginTime = 0;
}

PgPvPType_AceMatch::~PgPvPType_AceMatch()
{
	m_iWinTeam = TEAM_NONE;
	m_kContUserEntry.clear();
	m_kContPvPResult.clear();
	m_bEndRound = false;
	m_dwWaitBeginTime = 0;
}


HRESULT PgPvPType_AceMatch::Init( BM::Stream &kPacket, PgWarGround* pkGnd )
{
	PgWarMode::Init( kPacket, pkGnd );

	m_kContReady.clear();
	m_kContPlay.clear();
	m_kContPvPResult.clear();
	m_kContUserEntry.clear();
	m_kContWait.clear();

	kPacket.Pop(m_kOption);
	kPacket.Pop(m_iLobbyID);
	kPacket.Pop(m_iRoomIndex);
	kPacket.Pop(m_dwGamePlayTime);
	kPacket.Pop(m_kMode);
	kPacket.Pop(m_ucMaxRound);
	PU::TLoadTable_AM( kPacket, m_kContReady );

	InitTeamResult();
	InitEntry();

	if ( !m_ucMaxRound )
	{
		m_ucMaxRound = 1;
	}
	m_ucNowRound = 0;
	m_bEndRound = false;
	return S_OK;
}

void PgPvPType_AceMatch::InitEntry()
{
	CONT_PVP_TEAM_USER kContTeamRed, kContTeamBlue;
	CONT_PVP_GAME_USER::const_iterator itr = m_kContReady.begin();
	for ( ; itr!=m_kContReady.end(); ++itr )
	{
		if( TEAM_RED == itr->second.kTeamSlot.GetTeam() )
		{
			kContTeamRed.push_back(itr->second);
		}
		else
		{
			kContTeamBlue.push_back(itr->second);
		}
	}

	kContTeamRed.sort(PgPvPType_AceMatch::SortEntry);
	kContTeamBlue.sort(PgPvPType_AceMatch::SortEntry);

	m_kContUserEntry.insert( std::make_pair(TEAM_RED, kContTeamRed) );
	m_kContUserEntry.insert( std::make_pair(TEAM_BLUE, kContTeamBlue) );
}

void PgPvPType_AceMatch::InitTeamResult()
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
				m_iBattleLevel = iBalanceLevel + iAddLevel;
			}
		}
		else
		{
			m_iBattleLevel = 0;
		}
	}
}

bool PgPvPType_AceMatch::IsStart( DWORD const dwNowTime )
{
	return m_kContReady.empty();
}

HRESULT PgPvPType_AceMatch::Join( PgPlayer *pkPlayer, bool const bInturde )
{
	if ( pkPlayer )
	{
		CONT_PVP_GAME_USER::iterator itr = m_kContReady.find( pkPlayer->GetID() );
		if ( itr != m_kContReady.end() )
		{
			CONT_PVP_GAME_USER::mapped_type &kElement = itr->second;
			if ( E_PVP_OPT_DISABLEDASHJUMP & m_kOption )
			{
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

			m_kContPlay.insert( std::make_pair( itr->first, kElement ) );
			m_kContReady.erase(itr);
			return S_OK;
		}
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

BM::GUID PgPvPType_AceMatch::GetTopEntryUser( int const iTeam )
{
	CONT_USER_ENTRY::const_iterator itr = m_kContUserEntry.find(iTeam);
	if( m_kContUserEntry.end() != itr )
	{
		CONT_USER_ENTRY::mapped_type const &kElement = itr->second;
		if( !kElement.empty() )
		{
			return (kElement.begin())->kCharacterGuid;
		}
	}
	return BM::GUID::NullData();
}

SPvPGameUser PgPvPType_AceMatch::PopTopEntryUser( int const iTeam )
{
	SPvPGameUser kUser;
	CONT_USER_ENTRY::iterator entry_itr = m_kContUserEntry.find(iTeam);
	if( m_kContUserEntry.end() != entry_itr )
	{
		CONT_USER_ENTRY::mapped_type& kElement = entry_itr->second;
		if( !kElement.empty() )
		{
			CONT_PVP_TEAM_USER::iterator user_itr = kElement.begin();
			kUser = *(user_itr);
			kElement.erase( user_itr );
		}
	}

	return kUser;
}

bool PgPvPType_AceMatch::Start( PgPlayer *pkPlayer, PgWarGround * pkGnd )
{
	CONT_PVP_GAME_USER::const_iterator PlayUser_itr = m_kContPlay.find(pkPlayer->GetID());
	if ( m_kContPlay.end() != PlayUser_itr )
	{
		pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
		pkPlayer->SetAbil(AT_HP, pkPlayer->GetAbil(AT_C_MAX_HP));
		pkPlayer->SetAbil(AT_MP, pkPlayer->GetAbil(AT_C_MAX_MP));
		pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);

		CONT_PVP_GAME_USER::const_iterator WaitUser_itr = m_kContWait.find(pkPlayer->GetID());
		if( m_kContWait.end() != WaitUser_itr )
		{
			pkPlayer->SetAbil( AT_TEAM, 0 );
			pkPlayer->SetAbil( AT_HIDE, 1, true, true );
		}
		else
		{
			int const iTeam = PlayUser_itr->second.kTeamSlot.GetTeam();
			pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
			pkPlayer->SetAbil( AT_HIDE, 0, true, true );
			pkGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pkPlayer), 0, true, iTeam );

			CONT_PVP_TEAM_RESULT_INFO::iterator team_itr = m_kContTeamResult.find(iTeam);
			if ( m_kContTeamResult.end() != team_itr )
			{
				if( team_itr->second.iPlayUserCount )
				{
					VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__<<L"Play User Count is Wrong!! Count : " << team_itr->second.iPlayUserCount );
					LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("User Count is to many"));
				}
				else
				{
					team_itr->second.iPlayUserCount = 1;
				}
			}
		}
		return true;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

HRESULT PgPvPType_AceMatch::IsObModePlayer( PgPlayer *pkPlayer, BM::GUID& rkOutTargetGuid )
{//죽을때 엔트리 컨테이너에서 삭제하고, 여기선 확인후 유저 컨테이너 삭제/추가 작업만 함.
	int iTeam = TEAM_NONE;
	CONT_PVP_GAME_USER::const_iterator wait_itr = m_kContWait.find( pkPlayer->GetID() );
	if ( m_kContWait.end() != wait_itr )
	{
		iTeam = wait_itr->second.kTeamSlot.GetTeam();
		BM::GUID kTargetGuid = GetTopEntryUser(iTeam);
		if( kTargetGuid.IsNotNull() )
		{//타겟 찾고
			rkOutTargetGuid = kTargetGuid;
		}
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT PgPvPType_AceMatch::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
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
				int iDieRank = 1;
				int iBonusRank = 1;
				CONT_PVP_GAME_USER::const_iterator user_itr = m_kContPlay.begin();
				for ( ; user_itr!=m_kContPlay.end(); ++user_itr )
				{
					if ( user_itr->second.kResult > die_itr->second.kResult )
					{
						++iDieRank;
					}

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

				//이긴 팀 처리
				SetWinTeam(kill_itr->second.kTeamSlot.GetTeam());
			}

			die_itr->second.kResult.Dead( iDeadPoint );
			SetEventTeam_Die( die_itr->second.kTeamSlot.GetTeam(), iDeadPoint );

			//진 팀 처리
			SetLoseTeam(die_itr->second.kTeamSlot.GetTeam());
			
			if ( m_ucMaxRound != m_ucNowRound+1 )
			{//에이스 매치는 승패와 상관없이 양팀 모두 엔트리 변경 해줌.
				//진팀
				CONT_PVP_GAME_USER::key_type const &kDieUser_Key = die_itr->first;
				CONT_PVP_GAME_USER::mapped_type const &kkDieUser_Element= die_itr->second;
				EntryChange( kkDieUser_Element.kTeamSlot.GetTeam(), kDieUser_Key );

				//이긴팀
				CONT_PVP_GAME_USER::key_type const &kKillUser_Key = kill_itr->first;
				CONT_PVP_GAME_USER::mapped_type const &kKillUser_Element = kill_itr->second;
				EntryChange( kKillUser_Element.kTeamSlot.GetTeam(), kKillUser_Key );
			}
			else
			{//다음 라운드가 마지막 라운드일 경우...
			}
			//m_kContPvPResult.insert( std::make_pair(m_ucNowRound, *(kill_itr->second) );
				
			BM::Stream kPacket(PT_M_C_NFY_GAME_EVENT_KILL);
			kPacket.Push(pkUnit->GetID());
			kPacket.Push(iDeadPoint);
			kPacket.Push(kKillerGuid);
			kPacket.Push(iKillPoint);
			pkGnd->Broadcast( kPacket );

			m_bEndRound = true;
			return S_OK;
		}	
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

HRESULT PgPvPType_AceMatch::Update( DWORD const dwCurTime )
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

bool PgPvPType_AceMatch::EntryChange(int const iTeam, BM::GUID const& kCharGuid)
{
	CONT_PVP_GAME_USER::iterator PlayUser_itr = m_kContPlay.find( kCharGuid );
	if( m_kContPlay.end() != PlayUser_itr )
	{
		CONT_USER_ENTRY::iterator entry_itr = m_kContUserEntry.find(iTeam);
		if( m_kContUserEntry.end() != entry_itr )
		{//엔트리 컨테이너 꺼내
			m_kContWait.insert( std::make_pair( PlayUser_itr->first, PlayUser_itr->second ) );//대기열에 추가
			(entry_itr->second).pop_front();//엔트리에서 삭제한다
		}
		
		BM::GUID kNextEntryGuid = GetTopEntryUser(iTeam);//첫번째 넘을 꺼내자
		CONT_PVP_GAME_USER::iterator NextPlayUser_itr = m_kContWait.find( kNextEntryGuid );
		if( m_kContWait.end() != NextPlayUser_itr )
		{//다음 타자는 대기중인넘이어야 함.
			m_kContWait.erase(NextPlayUser_itr);//대기열에서 삭제
			return true;
		}
	}

	return false;
}

void PgPvPType_AceMatch::SetWinTeam(int const iWinTeam)
{
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::iterator team_itr = m_kContTeamResult.find(iWinTeam);
		if ( m_kContTeamResult.end() != team_itr )
		{
			++(team_itr->second.ucWinCount);
			m_iRoundWinTeam = iWinTeam;
		}
	}
}

void PgPvPType_AceMatch::SetLoseTeam(int const iLoseTeam)
{
	if ( m_kMode == PVP_MODE_TEAM )
	{
		CONT_PVP_TEAM_RESULT_INFO::iterator team_itr = m_kContTeamResult.find(iLoseTeam);
		if ( m_kContTeamResult.end() != team_itr )
		{
			team_itr->second.iPlayUserCount = 0;
		}
	}
}

bool PgPvPType_AceMatch::CalcRoundWinTeam( int &iOutRoundWinTeam )
{
	bool bEndGame = false;
	if ( m_kMode == PVP_MODE_TEAM )
	{
		BYTE kRedWin = 0;
		BYTE kBlueWin = 0;
		PgPvPType_DeathMatch::GetWinTeam( kRedWin, kBlueWin );
		iOutRoundWinTeam = m_iRoundWinTeam;
		if( m_ucMaxRound <= kRedWin+kBlueWin )
		{
			bEndGame = true;
		}
		CONT_USER_ENTRY::iterator red_itr = m_kContUserEntry.find(TEAM_RED);
		CONT_USER_ENTRY::iterator blue_itr = m_kContUserEntry.find(TEAM_BLUE);
		if( m_kContUserEntry.end() != red_itr && m_kContUserEntry.end() != blue_itr )
		{
			if( 0 == red_itr->second.size() 
				|| 0 == blue_itr->second.size() )
			{
				bEndGame = true;
			}
		}
	}
	else
	{
		iOutRoundWinTeam = TEAM_PERSONAL_CHK;
	}

	return bEndGame || (m_ucNowRound == m_ucMaxRound) || m_bNoEnemy;
}

bool PgPvPType_AceMatch::InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd )
{
	m_kContWait.clear();
	VEC_GUID kContPlayGuid, kContHideGuid;
	CONT_PVP_GAME_USER::const_iterator play_itr = m_kContPlay.begin();
	while( m_kContPlay.end() != play_itr )
	{
		if( GetTopEntryUser(TEAM_RED) != play_itr->first && GetTopEntryUser(TEAM_BLUE) != play_itr->first )
		{//구경할놈
			m_kContWait.insert( std::make_pair( play_itr->first, play_itr->second ) );
			kContHideGuid.push_back( play_itr->first );
		}
		else
		{
			kContPlayGuid.push_back( play_itr->first );
		}
		
		++play_itr;
	}

	BM::Stream kCPacket(PT_M_C_NFY_PVP_HIDEUNIT);
	PU::TWriteArray_A( kCPacket, kContHideGuid );
	PU::TWriteArray_A( kCPacket, kContPlayGuid );
	pkGnd->Broadcast(kCPacket);
	return false;
}

bool PgPvPType_AceMatch::IsAbusing(void)const
{
	if ( !PgPvPType_DeathMatch::IsExercise() )
	{
		return false;

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

void PgPvPType_AceMatch::StartGame( BM::Stream &kPacket, bool const bGame )
{
	PgPvPType_DeathMatch::StartGame( kPacket, bGame );
}


bool PgPvPType_AceMatch::End( bool const bAutoChange, PgWarGround *pkGnd )
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

EResultEndType PgPvPType_AceMatch::ResultEnd()
{
	if ( IsEndGame() )
	{
		CONT_PVP_GAME_USER::const_iterator ReadyUser_itr = m_kContReady.begin();
		if( m_kContReady.end() != ReadyUser_itr )
		{
			m_kContPlay.insert(std::make_pair(ReadyUser_itr->first, ReadyUser_itr->second));
		}

		BYTE kRedWin = 0;
		BYTE kBlueWin = 0;
		int const iWinTeam = PgPvPType_DeathMatch::GetWinTeam( kRedWin, kBlueWin );

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
	else if ( m_ucMaxRound != m_ucNowRound+1 )
	{//다음 라운드가 마지막 라운드일 경우...
		ClearEntryNo();
		m_dwWaitBeginTime = BM::GetTime32();
		return RESULT_END_WAIT_ENTRY;
	}
	return RESULT_END_RESTART;
}

bool PgPvPType_AceMatch::CheckEntryWait(DWORD const dwElapsedTime)
{
	DWORD dwNowTime = BM::GetTime32();
	if( m_dwWaitBeginTime - dwNowTime > dwElapsedTime )
	{
		return true;
	}

	return false;
}

void PgPvPType_AceMatch::ClearEntryNo()
{
	CONT_USER_ENTRY::iterator entry_itr = m_kContUserEntry.begin();
	while( m_kContUserEntry.end() != entry_itr )
	{
		CONT_PVP_TEAM_USER& kContElement = entry_itr->second;
		if( !kContElement.empty() )
		{
			CONT_PVP_TEAM_USER::iterator user_itr = kContElement.begin();
			while( kContElement.end() != user_itr)
			{
				user_itr->byEntryNo = 0;
				++user_itr;
			}
		}
		++entry_itr;
	}
}

bool PgPvPType_AceMatch::SelectEntry(int const iTeam, BM::GUID const &rkCharGuid) 
{ 
	if( TEAM_NONE == iTeam )
	{
		return false;
	}

	CONT_USER_ENTRY::iterator entry_itr = m_kContUserEntry.find(iTeam);
	if( m_kContUserEntry.end() != entry_itr )
	{
		CONT_PVP_TEAM_USER& kContElement = entry_itr->second;
		if( !kContElement.empty() )
		{
			CONT_PVP_TEAM_USER::iterator user_itr = kContElement.begin();
			while( kContElement.end() != user_itr)
			{
				if( rkCharGuid == user_itr->kCharacterGuid )
				{//존재 하는놈이라면 엔트리 증가
					++(user_itr->byEntryNo);
					return true;
				}
				++user_itr;
			}

			CONT_PVP_GAME_USER::const_iterator ready_itr = m_kContReady.find(rkCharGuid);
			if( m_kContReady.end() != ready_itr )
			{//처음 뽑힌 놈이면 엔트리 번호 0으로 추가
				CONT_PVP_GAME_USER::mapped_type kElement = ready_itr->second;
				kElement.byEntryNo = 0;

				kContElement.push_back(kElement);
			}
		}
	}
	else
	{//팀도 없네
		CONT_PVP_GAME_USER::const_iterator user_itr = m_kContReady.find(rkCharGuid);
		if( m_kContReady.end() != user_itr )
		{//팀 만들고 엔트리 번호 0으로 해서 추가해줌
			CONT_PVP_GAME_USER::mapped_type kElement = user_itr->second;
			kElement.byEntryNo = 0;

			CONT_PVP_TEAM_USER kContTeamUser;
			kContTeamUser.push_back(kElement);

			m_kContUserEntry.insert( std::make_pair(iTeam, kContTeamUser) );
		}
		
	}
	return false; 
}

void PgPvPType_AceMatch::ReadyGame( PgWarGround *pkGnd )
{
	if ( m_ucMaxRound != m_ucNowRound+1 )
	{//다음 라운드가 마지막 라운드일 경우...
		CONT_USER_ENTRY::iterator entry_itr = m_kContUserEntry.begin();
		while( m_kContUserEntry.end() != entry_itr )
		{//엔트리 정렬
			CONT_PVP_TEAM_USER& kContElement = entry_itr->second;
			kContElement.sort(PgPvPType_AceMatch::SortEntry);
			++entry_itr;
		}
	}
}