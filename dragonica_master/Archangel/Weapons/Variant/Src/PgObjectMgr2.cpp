#include "stdafx.h"
#include "PgObjectMgr2.h"

PgObserverPlayer::PgObserverPlayer( PgPlayer * pkPlayer, BM::GUID const &kOwnerID )
:	m_kOwnerID(kOwnerID)
{
	SetPlayer( pkPlayer );
}

PgObserverPlayer::PgObserverPlayer( PgNetModule<> const &kNetModule, BM::GUID const &kCharID, BM::GUID const &kOwnerID )
:	m_kOwnerID(kOwnerID)
,	m_kNetModule(kNetModule)
,	m_kCharID(kCharID)
{
}

void PgObserverPlayer::SetPlayer( PgPlayer * pkPlayer )
{
	if ( pkPlayer )
	{
		m_kNetModule = pkPlayer->GetNetModule();
		m_kCharID = pkPlayer->GetID();
	}
	else
	{
		m_kNetModule = PgNetModule<>();
		m_kCharID.Clear();
	}
}


PgObjectMgr2::PgObjectMgr2(void)
{
}

PgObjectMgr2::~PgObjectMgr2(void)
{
	ReleaseObserver();
}

void PgObjectMgr2::InitObjectMgr()
{
	PgObjectMgr::InitObjectMgr();
	ReleaseObserver();
}

void PgObjectMgr2::ReleaseObserver()
{
// 	CONT_OBSERVER::const_iterator ob_itr = m_kContObserver.begin();
// 	for ( ; ob_itr != m_kContObserver.end() ; ++ob_itr )
// 	{
// 		if ( ob_itr->second.GetPlayer() )
// 		{
// 			g_kTotalObjMgr.ReleaseUnit( ob_itr->second.GetPlayer() );
// 		}
// 	}
	m_kContObserver.clear();
}

void PgObjectMgr2::UnRegistHasObserver( CUnit *pkUnit, CUnit* pkTarget, ContGuidSet* pkContHasObserver )
{
	if( pkUnit )
	{
		if ( true == pkUnit->IsHaveObserver() )
		{
			if ( pkTarget )
			{
				BM::Stream kChangePacket( PT_M_C_NFY_OBMODE_TARGET_CHANGE, pkTarget->GetID() );
				kChangePacket.Push( true );// 접속종료로...

				CONT_OBSERVER::iterator ob_itr = m_kContObserver.begin();
				for( ; ob_itr!=m_kContObserver.end() ; ++ob_itr )
				{
					if ( pkUnit->GetID() == ob_itr->second.GetOwnerID() )
					{
						if( pkContHasObserver )
						{
							pkContHasObserver->insert( (*ob_itr).first );
						}

						ob_itr->second.SetOwnerID( pkTarget->GetID() );
						pkTarget->AddObserver( ob_itr->second.GetNetModule() );
						ob_itr->second.Send( kChangePacket );
					}
				}
			}
			else
			{
				BM::Stream kEndPacket( PT_M_C_NFY_OBMODE_END );

				CONT_OBSERVER::iterator ob_itr = m_kContObserver.begin();
				while( ob_itr!=m_kContObserver.end() )
				{
					if ( pkUnit->GetID() == ob_itr->second.GetOwnerID() )
					{
						if( pkContHasObserver )
						{
							pkContHasObserver->insert( (*ob_itr).first );
						}

						ob_itr->second.Send( kEndPacket );
						ob_itr = m_kContObserver.erase( ob_itr );
					}
					else
					{
						++ob_itr;
					}
				}
			}
			pkUnit->m_kContObserver.clear();
		}
	}
}

bool PgObjectMgr2::UnRegistUnit( CUnit *pkUnit )
{
	if ( true == PgObjectMgr::UnRegistUnit( pkUnit ) )
	{
		CUnit *pkTarget = GetRandomUnit( pkUnit->UnitType(), pkUnit->GetID() );
		UnRegistHasObserver( pkUnit, pkTarget );
		return true;// return true
	}

	return false;
}

HRESULT PgObjectMgr2::ChangeObserver( PgNetModule<> const &kNetModule, BM::GUID const &kCharGuid, BM::GUID const &kTargetID, EUnitType const kTargetUnitType, BM::GUID &rkOutOldTargetID )
{
	CONT_OBSERVER::iterator ob_itr = m_kContObserver.find( kCharGuid );
	if ( ob_itr != m_kContObserver.end() )
	{
		if ( ob_itr->second.GetOwnerID() == kTargetID )
		{
			rkOutOldTargetID = ob_itr->second.GetOwnerID();
			return S_FALSE;
		}

		CUnit *pkTarget = PgObjectMgr::GetUnit( kTargetUnitType, kTargetID );
		if ( pkTarget )
		{
			if ( S_OK == UnRegistObserver( ob_itr->second ) )
			{
				rkOutOldTargetID = ob_itr->second.GetOwnerID();
				ob_itr->second.SetOwnerID( pkTarget->GetID() );

				pkTarget->AddObserver( kNetModule );
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT PgObjectMgr2::RegistObserver( PgNetModule<> const &kNetModule, BM::GUID const &kCharGuid, BM::GUID const &kTargetID, EUnitType const kTargetUnitType )
{
	CUnit *pkTarget = PgObjectMgr::GetUnit( kTargetUnitType, kTargetID );
	if ( pkTarget )
	{
		PgObserverPlayer kObserver( kNetModule, kCharGuid, kTargetID );
		PgPlayer *pkCaster = dynamic_cast<PgPlayer*>( PgObjectMgr::GetUnit( UT_PLAYER, kCharGuid ) );
		if ( pkCaster )
		{
			kObserver.SetPlayer( pkCaster );
		}

		auto kPair = m_kContObserver.insert( std::make_pair( kCharGuid, kObserver ) );
		if ( !kPair.second )
		{
			UnRegistObserver( kPair.first->second );
			kPair.first->second = kObserver;
		}

		pkTarget->AddObserver( kNetModule );
		return ( kPair.second ? S_OK : S_FALSE );
	}

	return E_FAIL;
}

HRESULT PgObjectMgr2::UnRegistObserver( BM::GUID const &kCharID, BM::GUID *pkOutOwnerID )
{
	CONT_OBSERVER::iterator ob_itr = m_kContObserver.find( kCharID );
	if ( ob_itr != m_kContObserver.end() )
	{
		UnRegistObserver( ob_itr->second );

		if ( pkOutOwnerID )
		{
			*pkOutOwnerID = ob_itr->second.GetOwnerID();
		}

		m_kContObserver.erase( ob_itr );
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgObjectMgr2::UnRegistObserver( PgObserverPlayer const &kObserver )
{
	CUnit *pkOwner = PgObjectMgr::GetUnit( kObserver.GetOwnerID() );
	if ( pkOwner )
	{
		pkOwner->RemoveObserver( kObserver.GetNetModule() );
		return S_OK;
	}
	return E_FAIL;
}
 
CUnit* PgObjectMgr2::GetRandomUnit( EUnitType const eType, BM::GUID const &kIgnoreID )
{
	CONT_OBJECT_MGR_UNIT *pkWorkingCont = NULL;
	if( true == GetUnitContainer( eType, pkWorkingCont) )
	{
		if ( pkWorkingCont->size() )
		{
			int iIndex = BM::Rand_Index( static_cast<int>(pkWorkingCont->size()) );

			CONT_OBJECT_MGR_UNIT::const_iterator unit_itr = pkWorkingCont->begin();
			while ( iIndex )
			{
				++unit_itr;
				--iIndex;
			}

			if ( kIgnoreID == unit_itr->second->GetID() )
			{
				if ( 1 == pkWorkingCont->size() )
				{
					return NULL;
				}

				++unit_itr;
				if ( pkWorkingCont->end() == unit_itr )
				{
					unit_itr = pkWorkingCont->begin();
				}
			}

			return unit_itr->second;
		}
	}
	return NULL;
}

HRESULT PgObjectMgr2::GetObserver( BM::GUID const &kCharGuid, PgNetModule<>& rkOutModule )const
{
	CONT_OBSERVER::const_iterator ob_itr = m_kContObserver.find( kCharGuid );
	if ( ob_itr != m_kContObserver.end() )
	{
		rkOutModule = ob_itr->second.GetNetModule();
		return S_OK;
	}
	return E_FAIL;
}
