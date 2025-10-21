#include "stdafx.h"
#include "PgWarGround.h"
#include "PgAction.h"
#include "PgPvPType_Annihilation.h"
#include "PgActionAchievement.h"

PgPvPType_Annihilation::PgPvPType_Annihilation()
:	m_dwTotalGameTime(0)
,	m_dwBeginTime(0)
{

}

PgPvPType_Annihilation::~PgPvPType_Annihilation()
{

}

HRESULT PgPvPType_Annihilation::Init( BM::Stream &kPacket, PgWarGround* pkGnd )
{
	m_dwTotalGameTime = 0;
	m_dwBeginTime = 0;
	return PgPvPType_DeathMatch::Init( kPacket, pkGnd );
}

HRESULT PgPvPType_Annihilation::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
{
	if ( pkUnit )
	{
		pkUnit->DeathTime(0);// 섬멸전에서는 다시 살아 나지 못하게

		if ( pkKiller )
		{
			CONT_PVP_GAME_USER::iterator die_itr = m_kContPlay.find(pkUnit->GetID());
			if ( die_itr != m_kContPlay.end() )
			{
				CONT_PVP_GAME_USER::iterator kill_itr = m_kContPlay.find(pkKiller->GetID());
				if ( kill_itr != m_kContPlay.end() )
				{
					// 순위를 구한다.
					int iDieRank = 1;
					int iBonusRank = 1;
					CONT_PVP_GAME_USER::const_iterator user_itr;
					for ( user_itr=m_kContPlay.begin(); user_itr!=m_kContPlay.end(); ++user_itr )
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

					int iDeadPoint = (ms_kSetting.Get(this->Type(),PVP_ST_DEATH_POINT)+ (ms_kSetting.Get(this->Type(),PVP_ST_DEATH_POINT_WEIGHT) * iBonusRank));
					int iKillPoint = (ms_kSetting.Get(this->Type(),PVP_ST_KILL_POINT) + (ms_kSetting.Get(this->Type(),PVP_ST_KILL_POINT_WEIGHT) * iBonusRank));

					die_itr->second.kResult.Dead(iDeadPoint);
					SetEventTeam_Die( die_itr->second.kTeamSlot.GetTeam(), iDeadPoint );

					kill_itr->second.kResult.Kill(iKillPoint);
					SetEventTeam_Kill( kill_itr->second.kTeamSlot.GetTeam(), iKillPoint );

					// 다시 살아나지 않는다.
					m_kContReady.insert(std::make_pair(die_itr->first, die_itr->second));
					m_kContPlay.erase(die_itr);
					RefreshUserCount();

					BM::Stream kPacket(PT_M_C_NFY_GAME_EVENT_KILL);
					kPacket.Push(pkUnit->GetID());
					kPacket.Push(iDeadPoint);
					kPacket.Push(pkKiller->GetID());
					kPacket.Push(iKillPoint);
					pkGnd->Broadcast( kPacket );
					return S_OK;
				}
			}
		}	
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

bool PgPvPType_Annihilation::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	// 죽은 놈은 Ready컨테이너에 저장해놨으니까 가져와야 한다.
	CONT_PVP_GAME_USER::const_iterator user_itr;
	for ( user_itr=m_kContReady.begin(); user_itr!=m_kContReady.end(); ++user_itr )
	{
		m_kContPlay.insert(std::make_pair(user_itr->first,user_itr->second));
	}
	m_kContReady.clear();

	RefreshUserCount();

	DWORD const dwCurTime = BM::GetTime32();
	PgPvPType_DeathMatch::Update( dwCurTime );//
	m_dwTotalGameTime += ( dwCurTime - m_dwBeginTime );
	
	return PgPvPType_DeathMatch::End( bAutoChange, pkGnd );
}

bool PgPvPType_Annihilation::Start( PgPlayer *pkPlayer, PgWarGround * pkGnd )
{
	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.find(pkPlayer->GetID());
	if ( itr != m_kContPlay.end() )
	{
		int const iTeam = itr->second.kTeamSlot.GetTeam();
		pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
		pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
		pkGnd->SendToSpawnLoc( dynamic_cast<CUnit*>(pkPlayer), 0, true, iTeam );

		pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );

		m_dwBeginTime = BM::GetTime32();
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool PgPvPType_Annihilation::IsAbusing(void)const
{
	if ( !IsExercise() )
	{
		// 섬멸전은 시간으로 어뷰징을 판단...
		DWORD dwAbuingTime = 1000 * 60 * 3;
		if ( 4 <= m_kContPlay.size() )
		{
			DWORD iCount = static_cast<DWORD>( m_kContPlay.size() - 4 ) / 2;
			dwAbuingTime += (30000 * iCount);

			return m_dwTotalGameTime <= dwAbuingTime;
		}
	}

	return false;
}
