#include "stdafx.h"
#include "PgWarGround.h"
#include "PgPvPMode_KingOfHill.h"

PgPvPHillStatus::PgPvPHillStatus( SPvPHillNode const &rkNode )
:	m_kContNode(rkNode)
,	m_dwLastHoldTime(0)
,	m_bHold(false)
,	m_iHoldTeam(TEAM_NONE)
{
}

PgPvPHillStatus::~PgPvPHillStatus()
{

}

bool PgPvPHillStatus::Refresh()
{
	int const iHoldTeam = m_iHoldTeam;
	m_iHoldTeam = TEAM_NONE;

	if ( !m_kContHoldUnitInfo.empty() )
	{
		CONT_PVPHILL_ITEM::const_iterator hill_itr = m_kContHoldUnitInfo.begin();
		m_iHoldTeam = hill_itr->iTeam;
		while ( ++hill_itr != m_kContHoldUnitInfo.end() )
		{
			if ( m_iHoldTeam != hill_itr->iTeam )
			{
				m_iHoldTeam = TEAM_NONE;
			}
		}
	}

	return m_iHoldTeam != iHoldTeam;
}

void PgPvPHillStatus::Hold( bool bHold )
{
	if ( m_bHold != bHold )
	{
		m_bHold = bHold;
		if ( m_bHold )
		{
			m_dwLastHoldTime = BM::GetTime32();
		}
	}
}

bool PgPvPHillStatus::GetHoldRet( DWORD const dwCurTime, SHoldRet &kRet )
{
	if ( m_kContHoldUnitInfo.empty() || (m_iHoldTeam == TEAM_NONE) )
	{
		return false;
	}

	kRet.kCharGuid = m_kContHoldUnitInfo.begin()->kCharGuid;
	kRet.dwHoldTime = DifftimeGetTime( m_dwLastHoldTime, dwCurTime );
	m_dwLastHoldTime = dwCurTime;
	return true;
}

HRESULT PgPvPHillStatus::OnHill( CUnit *pkUnit, SHoldRet &kOldRet )
{
	if ( !m_kContNode.InPos(pkUnit->GetPos() ) )
	{
		return E_FAIL;
	}

	CONT_PVPHILL_ITEM::iterator hill_itr = std::find( m_kContHoldUnitInfo.begin(), m_kContHoldUnitInfo.end(), pkUnit );
	if ( hill_itr == m_kContHoldUnitInfo.end() )
	{
		m_kContHoldUnitInfo.push_back( SPvPHillItem(pkUnit) );
		if ( Refresh() && m_bHold )
		{
			DWORD const dwCurTime = BM::GetTime32();
			if ( m_kContHoldUnitInfo.size() > 1 )
			{
				kOldRet.kCharGuid = m_kContHoldUnitInfo.begin()->kCharGuid;
				kOldRet.dwHoldTime = DifftimeGetTime( m_dwLastHoldTime, dwCurTime );
			}
			m_dwLastHoldTime = dwCurTime;
			return S_OK;
		}
	}
	return S_FALSE;
}

HRESULT PgPvPHillStatus::OutHill( CUnit *pkUnit, SHoldRet &kOldRet )
{
	CONT_PVPHILL_ITEM::iterator hill_itr = std::find( m_kContHoldUnitInfo.begin(), m_kContHoldUnitInfo.end(), pkUnit );
	if ( hill_itr != m_kContHoldUnitInfo.end() )
	{
		DWORD const dwCurTime = BM::GetTime32();
		if ( m_bHold && (m_iHoldTeam != TEAM_NONE) )
		{
			if ( hill_itr == m_kContHoldUnitInfo.begin() )
			{// 이전에 점령하고 있던 놈이라면
				kOldRet.kCharGuid = hill_itr->kCharGuid;
				kOldRet.dwHoldTime = DifftimeGetTime( m_dwLastHoldTime, dwCurTime );
				m_dwLastHoldTime = dwCurTime;
			}
		}

		m_kContHoldUnitInfo.erase( hill_itr );
		if ( Refresh() )
		{
			m_dwLastHoldTime = dwCurTime;
			return S_OK;
		}
		return S_FALSE;
	}
	return E_FAIL;
}

// 
PgPvPMode_KingOfHill::PgPvPMode_KingOfHill()
:	m_dwNextChangeTime(0)
,	m_iRedPoint(0)
,	m_iBluePoint(0)
,	m_pkGnd(NULL)
,	m_dwVictoryPointStartTime(0)
,	m_dwChangeEntityTime(0)
{

}

PgPvPMode_KingOfHill::~PgPvPMode_KingOfHill()
{

}

DWORD PgPvPMode_KingOfHill::GetTime( EPvPTime const kType )const
{
	switch ( kType )
	{
	case PVP_TIME_READY:
		{
			return PgWarMode::ms_kSetting.Get( Type(), PVP_ST_TIME_READY );
		}break;
	}

	return PgWarMode::GetTime( kType );
}

PgGTrigger_KingOfHill const* PgPvPMode_KingOfHill::GetTrigger(std::string const& kTriggerID)const
{
	CONT_KOH_TRIGGER::const_iterator tri_it = m_kContTrigger.find(kTriggerID);
	if(tri_it!=m_kContTrigger.end())
	{
		return (*tri_it).second;
	}
	return NULL;
}

PgGTrigger_KingOfHill const* PgPvPMode_KingOfHill::GetTrigger(BM::GUID const& kUnitGuid)const
{
	CONT_KOH_TRIGGER::const_iterator tri_it = m_kContTrigger.begin();
	while(tri_it!=m_kContTrigger.end())
	{
		if((*tri_it).second && (*tri_it).second->GetUnit() && (*tri_it).second->GetUnit()->GetID() == kUnitGuid)
		{
			return (*tri_it).second;
		}
		++tri_it;
	}
	return NULL;
}

void PgPvPMode_KingOfHill::InsertEntity(std::string const& kTriggerID, int const iEntityNo, int const iLevel, ETeam const eTeam)
{
	if( 0==iEntityNo || NULL==m_pkGnd)
	{
		return;
	}

	SCreateEntity kCreateInfo;
	kCreateInfo.kClassKey.iClass = iEntityNo;
	kCreateInfo.kClassKey.nLv = iLevel;
	kCreateInfo.bUniqueClass = false;
	kCreateInfo.bEternalLife = false;
	kCreateInfo.kGuid.Generate();

	if( S_OK != m_pkGnd->FindTriggerLoc(kTriggerID.c_str(), kCreateInfo.ptPos) )
	{
		return;
	}

	NxRay kRay(NxVec3(kCreateInfo.ptPos.x, kCreateInfo.ptPos.y, kCreateInfo.ptPos.z+50), NxVec3(0, 0, -1.0f));
	NxRaycastHit kHit;
	if(NxShape * pkHitShape = m_pkGnd->RayCast(kRay, kHit))
	{
		kCreateInfo.ptPos.z = kHit.worldImpact.z;
	}

	PgEntity* pkEntity = dynamic_cast<PgEntity*>(m_pkGnd->CreateGuardianEntity(NULL, &kCreateInfo, L"PvP_KOH_Entity", true));
	if( pkEntity )
	{
		pkEntity->SetAbil(AT_TEAM, eTeam);
		if(eTeam==TEAM_NONE)
		{
			pkEntity->SetAbil(AT_CANNOT_ATTACK, 1);
		}
		else
		{
			if(0 == iLevel)
			{
				int const iTeamEffectNo = GetIngEffectNo(kTriggerID, eTeam);
				EffectQueueData kData(EQT_ADD_EFFECT, iTeamEffectNo);
				pkEntity->AddEffectQueue(kData);
			}
		}

		if(int const iEffectNo = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_DEFAULT_ENTITY_EFFECT ))
		{
			EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo);
			pkEntity->AddEffectQueue(kData);
		}

		int const iExpPoint = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV0+iLevel );
		pkEntity->SetAbil(AT_MIN_OCCUPY_POINT, eTeam==TEAM_RED ? iExpPoint : eTeam==TEAM_BLUE ? -iExpPoint : 0);
		pkEntity->SetAbil(AT_MAX_OCCUPY_POINT, PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV4));
		if(ENTITY_SEC_KTH_BASE_HILL==pkEntity->GetAbil(AT_ENTITY_SECOND_TYPE))
		{
			pkEntity->SetAbil(AT_OCCUPY_POINT, pkEntity->GetAbil(AT_MIN_OCCUPY_POINT), true, true);
			pkEntity->SetAbil(AT_USE_IS_ALLY_SKILL, 0);
		}

		CONT_KOH_TRIGGER::iterator tri_it = m_kContTrigger.find(kTriggerID);
		if(tri_it!=m_kContTrigger.end() && (*tri_it).second)
		{
			pkEntity->SetAbil(AT_DISTANCE, static_cast<int>((*tri_it).second->GetRadius())-10);
			pkEntity->VisualFieldMin((*tri_it).second->Min());
			pkEntity->VisualFieldMax((*tri_it).second->Max());
			pkEntity->SetAbil(AT_ENTITY_SECOND_TYPE, 0!=(*tri_it).second->GetInitEntityTeam() ? ENTITY_SEC_KTH_BASE_HILL  : ENTITY_SEC_KTH_HILL, true, true);
			
			if( CUnit * pkTriggerUnit = (*tri_it).second->GetUnit() )
			{
				int const iPoint = pkTriggerUnit->GetAbil(AT_OCCUPY_POINT);
				pkEntity->SetAbil(AT_OCCUPY_POINT, iPoint, true, true);
				::OnDamaged(NULL, pkTriggerUnit, 0, pkTriggerUnit->GetAbil(AT_HP), m_pkGnd, g_kEventView.GetServerElapsedTime());

				//중립가디언 색깔 넣기
				if(0 == iLevel)
				{
					int iEffectNo = 0;
					if(iPoint > 0)
					{
						iEffectNo = GetIngEffectNo(kTriggerID, TEAM_RED);
					}
					else if(iPoint < 0)
					{
						iEffectNo = GetIngEffectNo(kTriggerID, TEAM_BLUE);
					}

					if(iEffectNo)
					{
						EffectQueueData kData(EQT_ADD_EFFECT, iEffectNo);
						pkEntity->AddEffectQueue(kData);
					}
				}
			}
			(*tri_it).second->SetUnit(pkEntity);
		}

		BM::Stream kPacket(PT_M_C_NFY_KOH_INSERT_GUARDIAN);
		kPacket.Push(kTriggerID);
		kPacket.Push(pkEntity->GetID());
		kPacket.Push(static_cast<int>(eTeam));
		m_pkGnd->Broadcast(kPacket);
		return;
	}
}

void PgPvPMode_KingOfHill::ClearContTrigger()
{
	if(NULL==m_pkGnd)
	{
		return;
	}

	CUnit * pkUnit = NULL;
	CONT_KOH_TRIGGER::iterator trigger_it = m_kContTrigger.begin();
	while(trigger_it != m_kContTrigger.end())
	{
		if( (*trigger_it).second )
		if( pkUnit = (*trigger_it).second->GetUnit() )
		{
			::OnDamaged(NULL, pkUnit, 0, pkUnit->GetAbil(AT_HP), m_pkGnd, g_kEventView.GetServerElapsedTime());
			(*trigger_it).second->SetUnit(NULL);
		}
		++trigger_it;
	}
}

void PgPvPMode_KingOfHill::InstallContTriggerEntity()
{
	CONT_KOH_TRIGGER::const_iterator tri_it = m_kContTrigger.begin();
	while(tri_it!=m_kContTrigger.end())
	{
		KOH_ENTITY_KEY const kEntityNo = (*tri_it).second->GetEntityNo(TEAM_NONE, 0);
		InsertEntity((*tri_it).first, kEntityNo.first, kEntityNo.second, (*tri_it).second->GetInitEntityTeam());
		++tri_it;
	}
}

void PgPvPMode_KingOfHill::SetTriggerUnitPoint(std::string const& kTriggerID, int const iPoint)
{
	CONT_KOH_TRIGGER::iterator it = m_kContTrigger.find(kTriggerID);
	if(it != m_kContTrigger.end() && it->second)
	{
		SetTriggerUnitPoint(it->second->GetUnit(), iPoint);
	}
}

void PgPvPMode_KingOfHill::SetTriggerUnitPoint(CUnit * pkUnit, int const iPoint)
{
	if(!pkUnit)
	{
		return;
	}

	int const iOldPoint = pkUnit->GetAbil(AT_OCCUPY_POINT);
	int iApplyPoint = iPoint;
	if(ENTITY_SEC_KTH_BASE_HILL==pkUnit->GetAbil(AT_ENTITY_SECOND_TYPE))
	{
		int const iMinPoint = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV1 );
		int const iTeam = pkUnit->GetAbil(AT_TEAM);
		if(TEAM_RED==iTeam && iApplyPoint < iMinPoint)
		{
			iApplyPoint = iMinPoint;
		}
		else if(TEAM_BLUE==iTeam && iApplyPoint > -iMinPoint)
		{
			iApplyPoint = -iMinPoint;
		}

		pkUnit->SetAbil(AT_LEVEL, 1==pkUnit->GetAbil(AT_LEVEL) ? 1 : 0);	//레벨1이면 유지, 1이 아니면 Lv1로 새로 만듬
		pkUnit->SetAbil(AT_MIN_OCCUPY_POINT, iApplyPoint);
	}

	pkUnit->SetAbil(AT_OCCUPY_POINT, iApplyPoint, true, true);
}

void PgPvPMode_KingOfHill::ReadyGame(PgWarGround *pkGnd)
{
	m_iRedPoint = m_kGamePoint;
	m_iBluePoint = m_kGamePoint;

	int const iCount = pkGnd->GetUnitCount(UT_PLAYER);
	ClearContTrigger();
	InstallContTriggerEntity();
	m_dwVictoryPointStartTime = BM::GetTime32();
	m_dwChangeEntityTime = m_dwVictoryPointStartTime;

	BM::Stream kPacket(PT_M_C_NFY_OCCUPY_POINT_SECTION_INFO);
	kPacket.Push( PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV1 ) );
	kPacket.Push( PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV2 ) );
	kPacket.Push( PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV3 ) );
	kPacket.Push( PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV4 ) );
	pkGnd->Broadcast(kPacket);

	//각 지역으로 스폰시키기
	PgPlayer * pkPlayer = NULL;
	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.begin();
	while ( itr != m_kContPlay.end() )
	{
		if(pkPlayer = dynamic_cast<PgPlayer*>(pkGnd->GetUnit( (*itr).first )))
		{
			int const iTeam = itr->second.kTeamSlot.GetTeam();
			pkGnd->SendToSpawnLoc( pkPlayer, 0, true, iTeam );
			pkPlayer->Alive(EALIVE_PVP,E_SENDTYPE_BROADALL);
			pkPlayer->CUnit::SetState(US_IDLE);//ACTION_LOCK 때문에. 강제로 풀어줌.
			pkPlayer->SetAbil( AT_TEAM, iTeam, true, true );
		}
		++itr;
	}
}

bool PgPvPMode_KingOfHill::Start( PgPlayer *pkPlayer, PgWarGround *pkGnd )
{
	if(!pkPlayer || !pkGnd)
	{
		return false;
	}

	CONT_PVP_GAME_USER::const_iterator itr = m_kContPlay.find(pkPlayer->GetID());
	if ( itr != m_kContPlay.end() )
	{
		pkPlayer->SetAbil( AT_BATTLE_LEVEL, m_iBattleLevel );
	}

	int const iOccupyPoint = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_OCCUPY_POINT );
	pkPlayer->SetAbil(AT_OCCUPY_POINT, iOccupyPoint);

	m_bEndRedLimt = true;
	m_bEndBlueLimit = true;
	return true;
}

bool PgPvPMode_KingOfHill::End( bool const bAutoChange, PgWarGround *pkGnd )
{
	return PgPvPType_DeathMatch::End(bAutoChange, pkGnd);
}

EResultEndType PgPvPMode_KingOfHill::ResultEnd()
{
	ClearContTrigger();
	return PgPvPType_DeathMatch::ResultEnd();
}

bool PgPvPMode_KingOfHill::CalcRoundWinTeam( int &iOutRoundWinTeam )
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
				if(m_iRedPoint > 0)
				{
					iOutRoundWinTeam = TEAM_RED;
				}
				else if(m_iBluePoint > 0)
				{
					iOutRoundWinTeam = TEAM_BLUE;
				}

				if(TEAM_NONE==iOutRoundWinTeam)
				{
					//Kill수에 따라서..
				}

				if(TEAM_RED==iOutRoundWinTeam)
				{
					if ( ++(red_itr->second.ucWinCount) > (m_ucMaxRound/2) )
					{
						bEndGame = true;
					}
				}
				else if(TEAM_BLUE==iOutRoundWinTeam)
				{
					if ( ++(blue_itr->second.ucWinCount) > (m_ucMaxRound/2) )
					{
						bEndGame = true;
					}
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

bool PgPvPMode_KingOfHill::Exit( BM::GUID const& kGuid )
{
	CONT_PVP_GAME_USER::iterator user_itr = m_kContPlay.find(kGuid);
	if ( user_itr != m_kContPlay.end() )
	{
		/*if ( user_itr->second.iHillNo )
		{
			SetEvent_HillOut( pkPlayer );
		}*/
		m_kContPlay.erase(user_itr);
		RefreshUserCount();
		return true;
	}
	m_kContReady.erase( kGuid );

	CUnit const* pkUnit = NULL;
	CONT_KOH_TRIGGER::iterator it = m_kContTrigger.begin();
	while(it != m_kContTrigger.end())
	{
		if( (*it).second )
		if( pkUnit = (*it).second->GetUnit() )
		{
			if(kGuid==pkUnit->GetID())
			{
				(*it).second->SetUnit(NULL);
				break;
			}
		}
		++it;
	}
	return true;
}

HRESULT PgPvPMode_KingOfHill::Init( BM::Stream &kPacket, PgWarGround* pkGnd )
{
	if(NULL==pkGnd)
	{
		return E_FAIL;
	}

	if ( SUCCEEDED( PgPvPType_DeathMatch::Init(kPacket, pkGnd) ) )
	{
		m_pkGnd = pkGnd;
		m_iRedPoint = m_kGamePoint;
		m_iBluePoint = m_kGamePoint;

		m_kContTrigger.clear();
		CONT_GTRIGGER const& kContTrigger = m_pkGnd->GetContTrigger();
		for (CONT_GTRIGGER::const_iterator trigger_itr=kContTrigger.begin(); trigger_itr!=kContTrigger.end(); ++trigger_itr)
		{
			if ( GTRIGGER_TYPE_KING_OF_HILL == trigger_itr->second->GetType() )
			{
				if ( PgGTrigger_KingOfHill *pkTrigger = dynamic_cast<PgGTrigger_KingOfHill*>(trigger_itr->second) )
				{
					m_kContTrigger.insert(std::make_pair(trigger_itr->first, pkTrigger));
				}
			}
		}
		return S_OK;
	}

	VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << _T("Error") );
	return E_FAIL;
}

int PgPvPMode_KingOfHill::GetLinkPoint(std::string const& kTriggerID, int const iTeam)const
{
	if(0==iTeam)
	{
		return 0;
	}

	int iLinkPoint = 0;
	PgGTrigger_KingOfHill const* pkLink = NULL;
	PgGTrigger_KingOfHill const* pkTrigger = GetTrigger(kTriggerID);
	if(pkTrigger)
	{
		CONT_HILL_LINK const& kContLink = pkTrigger->GetContLink();
		CONT_HILL_LINK::const_iterator link_it = kContLink.begin();
		while(link_it != kContLink.end())
		{
			if( pkLink = GetTrigger(*link_it) )
			{
				if(pkLink->GetUnit() && iTeam == pkLink->GetUnit()->GetAbil(AT_TEAM))
				{
					++iLinkPoint;
				}
			}

			++link_it;
		}
	}
	return iLinkPoint;
}

KOH_ENTITY_KEY PgPvPMode_KingOfHill::GetEntityNo(std::string const& kTriggerID, int const iLevel, ETeam const eTeam)const
{
	PgGTrigger_KingOfHill const* pkTrigger = GetTrigger(kTriggerID);
	if(pkTrigger)
	{
		return pkTrigger->GetEntityNo(eTeam, iLevel);
	}
	static const KOH_ENTITY_KEY kNullData(0,0);
	return kNullData;
}

int PgPvPMode_KingOfHill::GetIngEffectNo(std::string const& kTriggerID, ETeam const eTeam)const
{
	PgGTrigger_KingOfHill const* pkTrigger = GetTrigger(kTriggerID);
	if(pkTrigger)
	{
		return pkTrigger->GetIngEffectNo(eTeam);
	}
	return 0;
}

HRESULT PgPvPMode_KingOfHill::Update(DWORD const dwCurTime)
{
	if ( SUCCEEDED( PgPvPType_DeathMatch::Update(dwCurTime) ) )
	{
		CUnit * pkUnit = NULL;

		//가디언의 변신
		if( BM::TimeCheck(m_dwChangeEntityTime, 300) )
		{
			CONT_KOH_TRIGGER::iterator unit_it = m_kContTrigger.begin();
			while(unit_it != m_kContTrigger.end())
			{
				if( (*unit_it).second )
				if( pkUnit = (*unit_it).second->GetUnit() )
				{
					int const iExpLv1 = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV1 );
					int const iExpLv2 = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV2 );
					int const iExpLv3 = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_NEXT_EXP_ENTITY_LV3 );
					int const iPoint = pkUnit->GetAbil(AT_OCCUPY_POINT);
					int const iLevel = pkUnit->GetAbil(AT_LEVEL);
					ETeam const eTeam = static_cast<ETeam>(pkUnit->GetAbil(AT_TEAM));
					ETeam const eCheckTeam = iPoint>0 ? TEAM_RED : (iPoint<0 ? TEAM_BLUE : TEAM_NONE);
					if(iPoint <= -iExpLv3 || iPoint >= iExpLv3)
					{
						//cout << "Lv3" << endl;
						if(3 != iLevel)
						{
							KOH_ENTITY_KEY const kNo = GetEntityNo((*unit_it).first, 3, eCheckTeam);
							InsertEntity((*unit_it).first, kNo.first, kNo.second, eCheckTeam);
						}
					}
					else if(iPoint <= -iExpLv2 || iPoint >= iExpLv2)
					{
						//cout << "Lv2" << endl;
						if(2 != iLevel)
						{
							KOH_ENTITY_KEY const kNo = GetEntityNo((*unit_it).first, 2, eCheckTeam);
							InsertEntity((*unit_it).first, kNo.first, kNo.second, eCheckTeam);
						}
					}
					else if(iPoint <= -iExpLv1 || iPoint >= iExpLv1)
					{
						//cout << "Lv1" << endl;
						if(1 != iLevel)
						{
							KOH_ENTITY_KEY const kNo = GetEntityNo((*unit_it).first, 1, eCheckTeam);
							InsertEntity((*unit_it).first, kNo.first, kNo.second, eCheckTeam);
						}
					}
					else
					{
						int const iSecondType = pkUnit->GetAbil(AT_ENTITY_SECOND_TYPE);
						ETeam eApplyTeam = TEAM_NONE;
						if(ENTITY_SEC_KTH_BASE_HILL==iSecondType)
						{
							eApplyTeam = eTeam;
						}
						if(0 != iLevel)
						{
							KOH_ENTITY_KEY const kNo = GetEntityNo((*unit_it).first, 0, eApplyTeam);
							InsertEntity((*unit_it).first, kNo.first, kNo.second, eApplyTeam);
						}
					}
					
					if(0==iLevel && (iPoint > -iExpLv1 && iPoint < iExpLv1))
					{
						//cout << "Ing" << endl;
						int const iRedEffectNo = GetIngEffectNo((*unit_it).first, TEAM_RED);
						int const iBlueEffectNo = GetIngEffectNo((*unit_it).first, TEAM_BLUE);
						CEffect const* pkRedEffect = pkUnit->GetEffect(iRedEffectNo);
						CEffect const* pkBlueEffect = pkUnit->GetEffect(iBlueEffectNo);

						//DelEffect
						if(0>=iPoint || eCheckTeam==TEAM_BLUE)
						{
							if(pkRedEffect)
							{
								pkUnit->DeleteEffect(iRedEffectNo);
							}
						}
						if(0<=iPoint || eCheckTeam==TEAM_RED)
						{
							if(pkBlueEffect)
							{
								pkUnit->DeleteEffect(iBlueEffectNo);
							}
						}

						//AddEffect
						if(0!=iPoint)
						{
							if(eCheckTeam==TEAM_RED)
							{
								if(NULL==pkRedEffect)
								{
									SActArg kArg;
									PgGroundUtil::SetActArgGround(kArg, m_pkGnd);
									pkUnit->AddEffect(iRedEffectNo, 0, &kArg, pkUnit);
								}
							}
							else if(eCheckTeam==TEAM_BLUE)
							{
								if(NULL==pkBlueEffect)
								{
									SActArg kArg;
									PgGroundUtil::SetActArgGround(kArg, m_pkGnd);
									pkUnit->AddEffect(iBlueEffectNo, 0, &kArg, pkUnit);
								}
							}
						}
					}
				}
				++unit_it;
			}
		}

		//가디언의 포인터 검사
		int const iInterval = PgWarMode::ms_kSetting.Get( Type(), PVP_ST_VICTORY_INTERVAL_TIME );
		if( BM::TimeCheck(m_dwVictoryPointStartTime, iInterval) )
		{
			int iPointRed = 0;
			int iLinkRed = 0;
			int iPointBlue = 0;
			int iLinkBlue = 0;
			CONT_KOH_TRIGGER::const_iterator c_it = m_kContTrigger.begin();
			while(c_it != m_kContTrigger.end())
			{
				if( (*c_it).second )
				if( pkUnit = (*c_it).second->GetUnit() )
				{
					int const iTeam = pkUnit->GetAbil(AT_TEAM);
					int const iLevel = pkUnit->GetAbil(AT_LEVEL);
					int const iLinkPoint = GetLinkPoint((*c_it).first, iTeam);
					if(TEAM_RED==iTeam)
					{
						iPointRed += iLevel;
						iLinkRed += iLinkPoint;
					}
					else if(TEAM_BLUE==iTeam)
					{
						iPointBlue += iLevel;
						iLinkBlue += iLinkPoint;
					}
				}
				++c_it;
			}
			iLinkRed /= 2;
			iLinkBlue /= 2;

			int const iPoint = (iPointRed+iLinkRed)-(iPointBlue+iLinkBlue);
			if(iPoint > 0)
			{
				SetBluePoint(iPoint, true);
			}
			else if(iPoint < 0)
			{
				SetRedPoint(-iPoint, true);
			}

			int EndLimit = m_kGamePoint * 0.2f;
			if( (EndLimit >= m_iRedPoint) && m_bEndRedLimt )
			{
				BM::Stream Packet(PT_M_C_NFY_KTH_MESSAGE);
				Packet.Push(74105);
				Packet.Push(true);

				m_pkGnd->Broadcast( Packet );
				m_bEndRedLimt = false;
			}

			if( (EndLimit >= m_iBluePoint) && m_bEndBlueLimit )
			{
				BM::Stream Packet(PT_M_C_NFY_KTH_MESSAGE);
				Packet.Push(74106);
				Packet.Push(true);

				m_pkGnd->Broadcast( Packet );
				m_bEndBlueLimit = false;
			}
		}

		if(m_iRedPoint<=0 || m_iBluePoint<=0)
		{
			return E_FAIL;
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgPvPMode_KingOfHill::SetEvent_Kill( CUnit *pkUnit, CUnit *pkKiller )
{
	static WORD usPoint = ms_kSetting.Get(Type(), PVP_ST_KILL_POINT);
	if ( pkUnit )
	{
		//pkUnit->SetState(US_DEADREADY);
		pkUnit->DeathTime(BM::GetTime32());

		CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pkUnit->GetID());
		if ( itr != m_kContPlay.end() )
		{
			//itr->second.kResult.SetDie();
			//SetEventTeam_Die( itr->second.kTeamSlot.GetTeam(), 0 );

			/*if ( itr->second.iHillNo )
			{
				SetEvent_HillOut( pkUnit );
			}*/
		}

		if ( pkKiller )
		{
			CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pkKiller->GetID());
			if ( itr != m_kContPlay.end() )
			{
				//itr->second.kResult.SetKill(usPoint);
				SetEventTeam_Kill( itr->second.kTeamSlot.GetTeam(), usPoint );
			}

			BM::Stream kPacket(PT_M_C_NFY_GAME_EVENT_KILL);
			kPacket.Push(pkUnit->GetID());
			kPacket.Push(pkKiller->GetID());
			kPacket.Push(usPoint);
			VNotify(&kPacket);
			return S_OK;
		}
	}
	return E_FAIL;
}

void PgPvPMode_KingOfHill::SetRedPoint(int const iAddPoint, bool const bSend)
{
	int const iOldPoint = m_iRedPoint;
	m_iRedPoint = std::min(m_kGamePoint, m_iRedPoint-iAddPoint);
	m_iRedPoint = std::max(m_iRedPoint, 0);

	if(bSend && iOldPoint!=m_iRedPoint)
	{
		BM::Stream kNfyPacket( PT_M_C_NFY_ADD_POINT_USER );
		kNfyPacket.Push( TEAM_RED );
		kNfyPacket.Push( m_iRedPoint );
		m_pkGnd->Broadcast( kNfyPacket );
	}
}

void PgPvPMode_KingOfHill::SetBluePoint(int const iAddPoint, bool const bSend)
{
	int const iOldPoint = m_iBluePoint;
	m_iBluePoint = std::min(m_kGamePoint, m_iBluePoint-iAddPoint);
	m_iBluePoint = std::max(m_iBluePoint, 0);

	if(bSend && iOldPoint!=m_iRedPoint)
	{
		BM::Stream kNfyPacket( PT_M_C_NFY_ADD_POINT_USER );
		kNfyPacket.Push( TEAM_BLUE );
		kNfyPacket.Push( m_iBluePoint );
		m_pkGnd->Broadcast( kNfyPacket );
	}
}

HRESULT PgPvPMode_KingOfHill::SetEvent_BonusPoint( CUnit *pkUnit, short const nAddPoint, PgWarGround *pkGnd )
{
	if ( !pkUnit )
	{
		return E_FAIL;
	}
	
	ETeam const eTeam = static_cast<ETeam>(pkUnit->GetAbil(AT_TEAM));
	if(TEAM_RED==eTeam)
	{
		SetRedPoint(nAddPoint, true);
	}
	else if(TEAM_BLUE==eTeam)
	{
		SetBluePoint(nAddPoint, true);
	}

	/*BM::Stream kNfyPacket( PT_M_C_NFY_ADD_POINT_USER, pkUnit->GetID() );
	kNfyPacket.Push( static_cast<WORD>(nAddPoint) );
	kNfyPacket.Push( byLeaderTeam );
	kNfyPacket.Push( m_kDefenceGuildInfo.nCharacterPoint );
	kNfyPacket.Push( m_kAttackGuildInfo.nCharacterPoint );
	pkGnd->Broadcast( kNfyPacket );*/
	return S_OK;
}

HRESULT PgPvPMode_KingOfHill::SetEvent_GetInstanceItem( CUnit * pUnit, PgWarGround * pGnd )
{
	if( !pUnit )
	{
		return E_FAIL;
	}

	CONT_PVP_GAME_USER::iterator user_iter = m_kContPlay.find(pUnit->GetID());
	if( user_iter == m_kContPlay.end() )
	{
		return E_FAIL;
	}

	static int InsItemRemovePoint = ms_kSetting.Get(PVP_TYPE_KTH, PVP_ST_INSITEM_POINT);

	user_iter->second.kResult.AddPoint(InsItemRemovePoint);

	BM::Stream Packet(PT_M_C_NFY_GAME_EVENT_CHANGEPOINT);

	CONT_SEND_PVP_POINT SendContainer;
	SendContainer.insert( std::make_pair(user_iter->first, user_iter->second.kResult) );

	PU::TWriteTable_AM(Packet, SendContainer);
	m_pkGnd->Broadcast(Packet);

	return S_OK;
}

HRESULT PgPvPMode_KingOfHill::SetEvent_CapturePoint( CUnit * pUnit, float const AddPoint, PgWarGround * pGnd )
{
	if( !pUnit )
	{
		return E_FAIL;
	}

	CONT_PVP_GAME_USER::iterator user_iter = m_kContPlay.find(pUnit->GetID());
	if( user_iter == m_kContPlay.end() )
	{
		return E_FAIL;
	}

	user_iter->second.kResult.fCaptureAccm += AddPoint;
	if( (user_iter->second.kResult.fCaptureAccm - user_iter->second.kResult.iPoint) >= 1 )
	{
		user_iter->second.kResult.iCapturePoint += 1;
		user_iter->second.kResult.iPoint += 1;
	}

	return S_OK;
}

bool PgPvPMode_KingOfHill::UpdateHoldResult( SHoldRet const &kRet )
{
	CONT_PVP_GAME_USER::iterator oldret_itr = m_kContPlay.find( kRet.kCharGuid );
	if ( oldret_itr != m_kContPlay.end() )
	{
		WORD usAddPoint = (WORD)(kRet.dwHoldTime/200);//1초당 5점
		//oldret_itr->second.kResult.usPoint += usAddPoint;
		
		SPvPTeamResult *pkRet = GetTeamResult( oldret_itr->second.kTeamSlot.GetTeam() );
		if ( pkRet )
		{
			//pkRet->usPoint += usAddPoint;
		}

		/*BM::Stream kSyncPacket( PT_PM_C_NFY_ADD_POINT_USER, kRet.kCharGuid );
		kSyncPacket.Push( usAddPoint );
		VNotify( &kSyncPacket );*/
		return true;
	}
	return false;
}

HRESULT PgPvPMode_KingOfHill::SetEvent_HillUp( CUnit *pkUnit, size_t const iHillNo )
{
	if ( pkUnit )
	{
		CONT_PVPHILL_STATUS::iterator hill_itr = m_kContHill.find( iHillNo );
		if ( hill_itr != m_kContHill.end() )
		{
			CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pkUnit->GetID());
			if ( itr != m_kContPlay.end() )
			{
				SHoldRet kOldRet;
				switch( hill_itr->second.OnHill( pkUnit, kOldRet ) )
				{
				case S_OK:
					{
						/*BM::Stream kPacket( PT_PM_C_NFY_HILL_TEAM, m_itrNowHill->second.GetHoldTeam() );
						VNotify( &kPacket );*/

						UpdateHoldResult( kOldRet );
					} // No Break;
				case S_FALSE:
					{
						//if ( itr->second.iHillNo )
						//{// HillNo가 있으면 이건 머냐...
						//	if ( itr->second.iHillNo != iHillNo )
						//	{
						//		SetEvent_HillOut( pkUnit );
						//	}
						//}
						//itr->second.iHillNo = iHillNo;
					}break;
				case E_FAIL:
					{
						//pkUnit->Send( BM::Stream(PT_PM_C_ANS_HILL_UP_ERROR, iHillNo), E_SENDTYPE_SELF );
						return E_ACCESSDENIED;
					}break;
				}
			}
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT PgPvPMode_KingOfHill::SetEvent_HillOut( CUnit *pkUnit )
{
	if ( pkUnit )
	{
		CONT_PVP_GAME_USER::iterator itr = m_kContPlay.find(pkUnit->GetID());
		if ( itr != m_kContPlay.end() )
		{
			//CONT_PVPHILL_STATUS::iterator hill_itr = m_kContHill.find( itr->second.iHillNo );
			//if ( hill_itr != m_kContHill.end() )
			//{
			//	SHoldRet kOldRet;
			//	switch( hill_itr->second.OutHill( pkUnit, kOldRet ) )
			//	{
			//	case S_OK:
			//		{
			//			BM::Stream kPacket( PT_PM_C_NFY_HILL_TEAM, m_itrNowHill->second.GetHoldTeam() );
			//			VNotify( &kPacket );

			//			UpdateHoldResult( kOldRet );
			//		} // No Break;
			//	case S_FALSE:
			//		{
			//			itr->second.iHillNo = 0;
			//			if ( kOldRet.kCharGuid == pkUnit->GetID() )
			//			{

			//			}
			//		}break;
			//	case E_FAIL:
			//		{
			//			return E_FAIL;
			//		}break;
			//	}
			//}
			return S_OK;
		}
	}
	return E_FAIL;
}

void PgPvPMode_KingOfHill::SendGamePoint()
{
	BM::Stream Packet(PT_M_C_NFY_GAME_EVENT_CHANGEPOINT);

	CONT_SEND_PVP_POINT SendContainer;
	CONT_PVP_GAME_USER::iterator user_iter = m_kContPlay.begin();
	for( ; user_iter != m_kContPlay.end() ; ++user_iter )
	{
		auto Ret = SendContainer.insert( std::make_pair(user_iter->first, user_iter->second.kResult) );
		if( !Ret.second )
		{
			Ret.first->second = user_iter->second.kResult;
		}
	}

	PU::TWriteTable_AM(Packet, SendContainer);

	m_pkGnd->Broadcast(Packet);
}