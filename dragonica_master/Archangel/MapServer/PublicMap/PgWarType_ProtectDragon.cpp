#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "PgEffectAbilTable.h"
#include "PgWarGround.h"
#include "PgBattleForEmporia.h"
#include "PgWarType_ProtectDragon.h"

PgWarType_ProtectDragon::PgWarType_ProtectDragon()
:	m_usDefenceWinPoint(600)
{

}

HRESULT PgWarType_ProtectDragon::Init( BM::Stream &kPacket, PgWarGround *pkGnd )
{
	if ( SUCCEEDED(PgBattleForEmporia::Init( kPacket, pkGnd ) ) )
	{
		if ( FAILED( g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_Battle_ProtectedDragon_DefenceWinPoint, m_usDefenceWinPoint ) ) )
		{
			m_usDefenceWinPoint = 600;// 기본은 600점
		}

		return S_OK;
	}
	return E_FAIL;
}

bool PgWarType_ProtectDragon::InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd )
{
	CUnit *pkFirstAttackDragon = NULL;
	CUnit *pkFirstDefenceDragon = NULL;
	int iAttackDragonClass = 0;
	int iDefenceDragonClass = 0;

	int iGrade = 0;
	int iTeam = TEAM_NONE;
	UNIT_PTR_ARRAY::const_iterator unit_itr = kUnitArray.begin();
	for ( ; unit_itr!=kUnitArray.end(); ++unit_itr )
	{
		iTeam = unit_itr->pkUnit->GetAbil(AT_TEAM);
		if ( (TEAM_ATTACKER == iTeam) || (TEAM_DEFENCER == iTeam) )
		{
			iGrade = unit_itr->pkUnit->GetAbil(AT_GRADE);

			CONT_EMBATTLE_OBJECT::mapped_type kElement( static_cast<WORD>(unit_itr->pkUnit->UnitType()) );
			kElement.iHP = unit_itr->pkUnit->GetAbil( AT_HP );
			kElement.iTeam = iTeam;
			kElement.byGrade = static_cast<BYTE>(iGrade);

			CONT_BATTLEAREA::iterator area_itr = m_kContBattleArea.begin();
			for ( ; area_itr != m_kContBattleArea.end() ; ++area_itr )
			{
				if ( true == area_itr->IsInPos( unit_itr->pkUnit->GetPos() ) )
				{
					kElement.iBattleAreaIndex = area_itr->GetParam();
					unit_itr->pkUnit->SetAbil( AT_BATTLEAREA_INDEX, kElement.iBattleAreaIndex, false, false );
					break;
				}
			}

			//HP공유: 부모,자식 세팅
			if(1==kElement.iBattleAreaIndex)
			{
				m_kContObjectInfo.insert( std::make_pair( unit_itr->pkUnit->GetID(), kElement ) );// 부모만
			}

			if ( TEAM_ATTACKER == iTeam )
			{
				if( !pkFirstAttackDragon )
				{
					pkFirstAttackDragon = unit_itr->pkUnit;
					pkFirstAttackDragon->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_PARENT);
					pkFirstAttackDragon->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_HP );
					iAttackDragonClass = pkFirstAttackDragon->GetAbil(AT_CLASS);
				}
				else
				{
					pkFirstAttackDragon->AddSummonUnit(unit_itr->pkUnit->GetID(), unit_itr->pkUnit->GetAbil(AT_CLASS));
					unit_itr->pkUnit->Caller(pkFirstAttackDragon->GetID());
					unit_itr->pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_CHILDREN);
					unit_itr->pkUnit->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_NONE );

					if(iAttackDragonClass != unit_itr->pkUnit->GetAbil(AT_CLASS))
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Emporia ProtectDragon Class Not Same, AttackDragonClass["<<iAttackDragonClass<<L"], Summon["<<unit_itr->pkUnit->GetAbil(AT_CLASS)<<L"]" );
					}
				}
			}
			else
			{
				if( !pkFirstDefenceDragon )
				{
					pkFirstDefenceDragon = unit_itr->pkUnit;
					pkFirstDefenceDragon->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_PARENT);
					pkFirstDefenceDragon->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_HP );
					iDefenceDragonClass = pkFirstDefenceDragon->GetAbil(AT_CLASS);
				}
				else
				{
					pkFirstDefenceDragon->AddSummonUnit(unit_itr->pkUnit->GetID(), unit_itr->pkUnit->GetAbil(AT_CLASS));
					unit_itr->pkUnit->Caller(pkFirstDefenceDragon->GetID());
					unit_itr->pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_CHILDREN);
					unit_itr->pkUnit->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_NONE );

					if(iDefenceDragonClass != unit_itr->pkUnit->GetAbil(AT_CLASS))
					{
						VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Emporia ProtectDragon Class Not Same, DefenceDragonClass["<<iDefenceDragonClass<<L"], Summon["<<unit_itr->pkUnit->GetAbil(AT_CLASS)<<L"]" );
					}
				}
			}
		}
	}

	if(m_kContObjectInfo.empty())
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Not Parent ProtectDragon, kUnitArray size["<<kUnitArray.size()<<L"]" );
	}
	return true;
}

bool PgWarType_ProtectDragon::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	if ( !m_bEndGame )
	{
		m_iWinTeam = TEAM_DEFENCER;
	}

	return PgBattleForEmporia::End( bAutoChange, pkGnd );
}

HRESULT PgWarType_ProtectDragon::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
{
	BM::GUID const &kKillerGuid = ( pkKiller ? pkKiller->GetID() : BM::GUID::NullData() );
	int iAddPoint = 0;

	bool bKillDragon = false;
	switch ( pkUnit->UnitType() )
	{
	case UT_PLAYER:
		{		
			iAddPoint = 5;// 기본점수는 5점이다.
			if ( pkKiller )
			{
				// 레벨차이를 체크한다.
				int const iLevel = ( pkUnit->GetAbil( AT_LEVEL ) - pkKiller->GetAbil( AT_LEVEL ) );
				if ( iLevel > 4 )
				{
					// 레벨 차이가 5이상이면(4보다 크면) 7점이다.
					iAddPoint = 7;
				}
				else if ( iLevel < -4 )
				{
					// 레벨 차이가 -5이상(-4보다 작으면) 3점이다.
					iAddPoint = 3;
				}
			}
			else
			{
				// 킬러가 없으면 자살 10점이다.
				iAddPoint = 10;
			}

			CONT_EMPORIA_BATTLE_USER::iterator itr = m_kContUser.find( pkUnit->GetID() );
			if ( itr != m_kContUser.end() )
			{
				itr->second.kResult.Dead(0);

				itr = m_kContUser.find( kKillerGuid );
				if ( itr != m_kContUser.end() )
				{
					itr->second.kResult.Kill( iAddPoint );
				}
			}

			int const iLastBattleAreaIndex = pkUnit->GetAbil( AT_BATTLEAREA_INDEX );
			if ( iLastBattleAreaIndex > 0 )
			{
				size_t const iIndex = iLastBattleAreaIndex - 1;
				if ( iIndex < m_kContBattleArea.size() )
				{
					if ( S_OK == m_kContBattleArea.at(iIndex).RemoveUser( pkUnit->GetID() ) )
					{
						BM::Stream kBroadPacket( PT_M_C_NFY_BATTLE_AREA_USER, iLastBattleAreaIndex );
						kBroadPacket.Push( 0 );
						kBroadPacket.Push( pkUnit->GetID() );
						kBroadPacket.Push( pkUnit->GetAbil(AT_TEAM) );
						pkGnd->Broadcast( kBroadPacket );
					}
				}

				pkUnit->DeleteEffect( EFFECTNO_DRAGON_SELECT_DELAYTIME );
				pkUnit->SetAbil( AT_BATTLEAREA_INDEX, 0, false, false );
			}

			int const iAddPointTeam = GetEnemyTeam(pkUnit->GetAbil(AT_TEAM));
			AddTeamPoint( iAddPointTeam, static_cast<unsigned short>(iAddPoint), true, pkGnd );

			if ( TEAM_DEFENCER == iAddPointTeam )
			{
				if ( m_kDefenceGuildInfo.GetPoint() >= m_usDefenceWinPoint )
				{
					m_bEndGame = true;
					m_iWinTeam = TEAM_DEFENCER;

					SendResultNotice(m_iWinTeam, ERNT_HAVEPOINT);
				}
			}
		}break;
	case UT_MONSTER:
	case UT_BOSSMONSTER:
		{
			bKillDragon = true;
			CONT_EMBATTLE_OBJECT::iterator dragon_itr = m_kContObjectInfo.find( pkUnit->GetID() );
			if( dragon_itr != m_kContObjectInfo.end() )
			{
				dragon_itr->second.iHP = 0;

				m_bEndGame = true;
				m_iWinTeam = GetEnemyTeam( pkUnit->GetAbil(AT_TEAM) );

				SendResultNotice(m_iWinTeam, ERNT_KILLDRAGON, pkKiller);
			}
			else
			{
				return E_FAIL;
			}
		}break;
	default:
		{
			return E_FAIL;
		}break;
	}

	BM::Stream kPacket( PT_M_C_NFY_GAME_EVENT_KILL, static_cast<BYTE>(0) );
	kPacket.Push(m_kDefenceGuildInfo.nCharacterPoint);
	kPacket.Push(m_kAttackGuildInfo.nCharacterPoint);
	kPacket.Push(pkUnit->GetID());
	kPacket.Push(kKillerGuid);
	kPacket.Push(iAddPoint);
	kPacket.Push(bKillDragon);
	pkGnd->Broadcast( kPacket );
	return S_OK;
}

void PgWarType_ProtectDragon::WriteToStartAddOnPacket( BM::Stream &kPacket )const
{
	PgBattleForEmporia::WriteToStartAddOnPacket(kPacket);
	kPacket.Push( m_usDefenceWinPoint );
}