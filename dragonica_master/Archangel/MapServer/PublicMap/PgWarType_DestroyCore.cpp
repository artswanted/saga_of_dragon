#include "stdafx.h"
#include "Lohengrin/VariableContainer.h"
#include "PgEffectAbilTable.h"
#include "PgWarGround.h"
#include "PgBattleForEmporia.h"
#include "PgWarType_DestroyCore.h"

PgWarType_DestroyCore::PgWarType_DestroyCore(void)
:	m_iLeaderTeam(TEAM_NONE), m_usCoreATKAuthPoint(0), m_usUseKillAuthPoint(0)
{}

HRESULT PgWarType_DestroyCore::Init( BM::Stream &kPacket, PgWarGround *pkGnd )
{
	if ( SUCCEEDED(PgBattleForEmporia::Init( kPacket, pkGnd ) ) )
	{
		m_kContObjectInfo.clear();
		m_iLeaderTeam = TEAM_NONE;
		m_kMainCoreGuid[0].Clear();
		m_kMainCoreGuid[1].Clear();
		if ( FAILED( g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_Battle_DestroyCore_Auth_TotalPoint, m_usCoreATKAuthPoint ) ) )
		{
			m_usCoreATKAuthPoint = 5000;// 기본은 5000점
		}

		if ( FAILED( g_kVariableContainer.Get( EVar_Kind_Guild, EVar_Emporia_Battle_DestroyCore_Unit_KillPoint, m_usUseKillAuthPoint ) ) )
		{
			m_usUseKillAuthPoint = 100;// 기본은 100점
		}

		return S_OK;
	}
	return E_FAIL;
}

BM::GUID const &PgWarType_DestroyCore::GetMainCoreID( int const iTeam )const
{
	switch( iTeam )
	{
	case TEAM_DEFENCER:
	case TEAM_ATTACKER:
		{
			return m_kMainCoreGuid[iTeam-1];
		}break;
	}
	return BM::GUID::NullData();
}

bool PgWarType_DestroyCore::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	if ( !m_bEndGame )
	{
		m_iWinTeam = TEAM_DEFENCER; // 기본은 방어 길드 승리

		// 코어 HP가 많은 팀이 이긴다.
		CUnit *pkAttackCore = pkGnd->GetUnit( GetMainCoreID(TEAM_ATTACKER) );
		CUnit *pkDefenceCore = pkGnd->GetUnit( GetMainCoreID(TEAM_DEFENCER) );
		if ( pkAttackCore && pkDefenceCore )
		{
			int const iAttackHP = pkAttackCore->GetAbil(AT_HP);
			int const iDefenceHP = pkDefenceCore->GetAbil(AT_HP);
			if ( iAttackHP == iDefenceHP )
			{
				// HP가 같으면 점수로 비교한다.
				if ( m_kAttackGuildInfo.GetPoint() > m_kDefenceGuildInfo.GetPoint() )
				{
					m_iWinTeam = TEAM_ATTACKER;
				}
			}
			else if ( iAttackHP > iDefenceHP )
			{
				m_iWinTeam = TEAM_ATTACKER;
			}
		}
		else
		{
			if ( pkAttackCore && !pkDefenceCore )
			{
				m_iWinTeam = TEAM_ATTACKER;
			}
		}
	}

	// 유저한테 줄 패킷을 만든다.
	return PgBattleForEmporia::End( bAutoChange, pkGnd );
}

HRESULT PgWarType_DestroyCore::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller, PgWarGround *pkGnd )
{
	if ( pkUnit )
	{// pkKiller가 없을수도 있다.

		BM::GUID const kKillerGuid = ( pkKiller ? pkKiller->GetID() : BM::GUID::NullData() );

		BYTE byLeaderTeam = 0;
		bool bDestroyCore = false;
		int iAddPoint = 0;

		switch (pkUnit->UnitType() )
		{
		case UT_PLAYER:
			{
				iAddPoint = m_usUseKillAuthPoint;

				// Killer를 찾을 수 없을때는 자살 10점
				CONT_EMPORIA_BATTLE_USER::iterator die_itr = m_kContUser.find( pkUnit->GetID() );
				if ( die_itr != m_kContUser.end() )
				{
					die_itr->second.kResult.Dead(0);

					CONT_EMPORIA_BATTLE_USER::iterator kill_itr = m_kContUser.find( kKillerGuid );
					if ( kill_itr != m_kContUser.end() )
					{
						kill_itr->second.kResult.Kill( iAddPoint );
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

				byLeaderTeam = this->AddTeamPoint( GetEnemyTeam(pkUnit->GetAbil(AT_TEAM)), static_cast<unsigned short>(iAddPoint), true, pkGnd );
			}break;
		case UT_OBJECT:
			{
				bDestroyCore = true;
				CONT_EMBATTLE_OBJECT::iterator core_itr = m_kContObjectInfo.find( pkUnit->GetID() );
				if( core_itr != m_kContObjectInfo.end() )
				{
					core_itr->second.iHP = 0;

					m_bEndGame = true;
					m_iWinTeam = GetEnemyTeam( pkUnit->GetAbil(AT_TEAM) );

					SendResultNotice(m_iWinTeam, ERNT_DESTORYCORE, pkKiller);
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

		BM::Stream kPacket(PT_M_C_NFY_GAME_EVENT_KILL, byLeaderTeam);
		kPacket.Push(m_kDefenceGuildInfo.nCharacterPoint);
		kPacket.Push(m_kAttackGuildInfo.nCharacterPoint);
		kPacket.Push(pkUnit->GetID());
		kPacket.Push(kKillerGuid);
		kPacket.Push(iAddPoint);
		kPacket.Push(bDestroyCore);
		pkGnd->Broadcast( kPacket );
		return S_OK;
	}

	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
	return E_FAIL;
}

BYTE PgWarType_DestroyCore::AddTeamPoint( int const iTeam, unsigned short nPoint, bool const bCharacter, PgWarGround *pkGnd )
{	
	PgBattleForEmporia::AddTeamPoint( iTeam, nPoint, bCharacter, pkGnd );

	int const iNewLeaderTeam = ( (m_kDefenceGuildInfo.GetPoint() > m_kAttackGuildInfo.GetPoint()) ? TEAM_DEFENCER : TEAM_ATTACKER );

	SGuildBattleInfo* pATKTeam = NULL;
	SGuildBattleInfo* pDEFTeam = NULL;

	switch( iNewLeaderTeam )
	{
	case TEAM_DEFENCER:
		{
			pATKTeam = &m_kDefenceGuildInfo;
			pDEFTeam = &m_kAttackGuildInfo;
		}break;
	case TEAM_ATTACKER:
		{
			pATKTeam = &m_kAttackGuildInfo;
			pDEFTeam = &m_kDefenceGuildInfo;
		}break;
	default:
		{
			return 0;
		}break;
	}

	if( pATKTeam->GetPoint() >= m_usCoreATKAuthPoint )
	{
		if ( iNewLeaderTeam != m_iLeaderTeam )
		{
			SetCoreAtkAuth(iNewLeaderTeam, pkGnd);
			m_iLeaderTeam = iNewLeaderTeam;
		}

		pATKTeam->SetPoint(0, true);
		pDEFTeam->SetPoint(static_cast<int>(m_usCoreATKAuthPoint/10), true);
		return static_cast<BYTE>(m_iLeaderTeam);
	}
	return 0;
}

void PgWarType_DestroyCore::SetCoreAtkAuth( int const iTeam, PgWarGround *pkGnd )
{
	if( !pkGnd )
	{
		return;
	}

	UNIT_PTR_ARRAY kUnitArray;
	pkGnd->GetHaveAbilUnitArray( kUnitArray, AT_TEAM, UT_OBJECT );

	UNIT_PTR_ARRAY::iterator unit_itr = kUnitArray.begin();
	for ( ; unit_itr != kUnitArray.end() ; ++unit_itr )
	{
		int const iUnitTeam = unit_itr->pkUnit->GetAbil(AT_TEAM);
		if ( TEAM_NONE == iTeam )
		{
			SEffectCreateInfo kCreate;
			kCreate.eType = EFFECT_TYPE_PENALTY;
			kCreate.iEffectNum = EFFECTNO_CANNOT_DAMAGE_CORE_BASE + iUnitTeam;
			kCreate.kActArg.Set( ACTARG_GROUND, pkGnd );
			kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
			unit_itr->pkUnit->AddEffect( kCreate );
		}
		else
		{
			if( iUnitTeam == iTeam )
			{//새로운 리더팀의 코어를 보호
				SEffectCreateInfo kCreate;
				kCreate.eType = EFFECT_TYPE_PENALTY;
				kCreate.iEffectNum = EFFECTNO_CANNOT_DAMAGE_CORE_BASE + iUnitTeam;
				kCreate.kActArg.Set( ACTARG_GROUND, pkGnd );
				kCreate.eOption = SEffectCreateInfo::ECreateOption_CallbyServer;
				unit_itr->pkUnit->AddEffect( kCreate );
			}
			else
			{//바뀐팀의 코어를 해제
				unit_itr->pkUnit->DeleteEffect( EFFECTNO_CANNOT_DAMAGE_CORE_BASE + iUnitTeam );
			}
		}
	}
}

bool PgWarType_DestroyCore::InitUnitInfo( UNIT_PTR_ARRAY const &kUnitArray, PgWarGround *pkGnd )
{
	CUnit *pkFirstAttackCore = NULL;
	CUnit *pkFirstDefenceCore = NULL;
	int iAttackAutoHealValue = 0;
	int iDefenceAutoHealValue = 0;
	int iAttackCoreClass = 0;
	int iDefenceCoreClass = 0;

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

			if ( UT_OBJECT == unit_itr->pkUnit->UnitType() )
			{
				//HP공유: 부모,자식 세팅
				if ( TEAM_ATTACKER == iTeam )
				{
					if( !pkFirstAttackCore )
					{
						m_kContObjectInfo.insert( std::make_pair( unit_itr->pkUnit->GetID(), kElement ) );// 부모만

						pkFirstAttackCore = unit_itr->pkUnit;
						m_kMainCoreGuid[TEAM_ATTACKER-1] = pkFirstAttackCore->GetID();
						pkFirstAttackCore->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_PARENT);
						pkFirstAttackCore->SetAbil( AT_HP_RECOVERY, iAttackAutoHealValue );
						pkFirstAttackCore->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_HP );
						iAttackCoreClass = pkFirstAttackCore->GetAbil(AT_CLASS);
					}
					else
					{
						pkFirstAttackCore->AddSummonUnit(unit_itr->pkUnit->GetID(), unit_itr->pkUnit->GetAbil(AT_CLASS));
						unit_itr->pkUnit->Caller(pkFirstAttackCore->GetID());
						unit_itr->pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_CHILDREN);
						unit_itr->pkUnit->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_NONE );

						if(iAttackCoreClass != unit_itr->pkUnit->GetAbil(AT_CLASS))
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Emporia DestroyCore Class Not Same, AttackCoreClass["<<iAttackCoreClass<<L"], Summon["<<unit_itr->pkUnit->GetAbil(AT_CLASS)<<L"]" );
						}
					}
				}
				else
				{
					if( !pkFirstDefenceCore )
					{
						m_kContObjectInfo.insert( std::make_pair( unit_itr->pkUnit->GetID(), kElement ) );// 부모만

						pkFirstDefenceCore = unit_itr->pkUnit;
						m_kMainCoreGuid[TEAM_DEFENCER-1] = pkFirstDefenceCore->GetID();
						pkFirstDefenceCore->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_PARENT);
						pkFirstDefenceCore->SetAbil( AT_HP_RECOVERY, iAttackAutoHealValue );
						pkFirstDefenceCore->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_HP );
						iDefenceCoreClass = pkFirstDefenceCore->GetAbil(AT_CLASS);
					}
					else
					{
						pkFirstDefenceCore->AddSummonUnit(unit_itr->pkUnit->GetID(), unit_itr->pkUnit->GetAbil(AT_CLASS));
						unit_itr->pkUnit->Caller(pkFirstDefenceCore->GetID());
						unit_itr->pkUnit->SetAbil(AT_SHARE_HP_RELATIONSHIP, ESHR_CHILDREN);
						unit_itr->pkUnit->SetAbil( AT_ENABLE_AUTOHEAL, AUTOHEAL_NONE );

						if(iDefenceCoreClass != unit_itr->pkUnit->GetAbil(AT_CLASS))
						{
							VERIFY_INFO_LOG(false, BM::LOG_LV0, __FL__<<L"Emporia DestroyCore Class Not Same, DefenceCoreClass["<<iDefenceCoreClass<<L"], Summon["<<unit_itr->pkUnit->GetAbil(AT_CLASS)<<L"]" );
						}
					}
				}

				unit_itr->pkUnit->SetAbil(AT_CANNOT_DAMAGE, 0, true, true);	//공격가능할 수 있도록 수정
			}
			else
			{
				if ( TEAM_ATTACKER == iTeam )
				{
					iAttackAutoHealValue += unit_itr->pkUnit->GetAbil(AT_C_HP_RECOVERY);
				}
				else
				{
					iDefenceAutoHealValue += unit_itr->pkUnit->GetAbil(AT_C_HP_RECOVERY);
				}
			}
		}
	}

	m_iLeaderTeam = TEAM_NONE;
	SetCoreAtkAuth(m_iLeaderTeam, pkGnd);
	return true;
}

void PgWarType_DestroyCore::WriteToStartAddOnPacket( BM::Stream &kPacket )const
{
	PgBattleForEmporia::WriteToStartAddOnPacket(kPacket);
	kPacket.Push( m_usCoreATKAuthPoint );
	kPacket.Push( static_cast<BYTE>(m_iLeaderTeam) );
}