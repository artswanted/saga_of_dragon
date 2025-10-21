#include "stdafx.h"
#include "Variant/PgPartyMgr.h"
#include "PgLocalPartyMgr.h"
#include "PgGround.h"

HRESULT PgGround::SetOBMode_Start( PgPlayer *pkCasterPlayer, CUnit *pkTarget )
{
	if ( S_OK == SetOBMode_Start( pkCasterPlayer->GetNetModule(), pkCasterPlayer->GetID(), pkTarget, false ) )
	{
		pkCasterPlayer->SetSyncType( SYNC_TYPE_NONE, true );

		// Target과 Area정보를 동일하게 맞추어 주어야 한다.
		if ( pkCasterPlayer->LastAreaIndex() != pkTarget->LastAreaIndex() )
		{
			PgSmallArea *pkCasterArea = GetArea( pkCasterPlayer->LastAreaIndex() );
			PgSmallArea *pkTargetArea = GetArea( pkTarget->LastAreaIndex() );
			SendAreaData( pkCasterPlayer, pkTargetArea, pkCasterArea, SYNC_TYPE_RECV_ADD );// AddUnit 받기만 해야 한다.
		}
		
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgGround::SetOBMode_Start( PgNetModule<> const &kNetModule, BM::GUID const &kCharID, CUnit *pkTarget, bool const bOnlyModule )
{
	if ( SUCCEEDED( PgObjectMgr2::RegistObserver( kNetModule, kCharID, pkTarget->GetID(), pkTarget->UnitType() ) ) )
	{
		BM::Stream kPacket( PT_M_C_NFY_OBMODE_START, pkTarget->GetID() );
		kNetModule.Send( kPacket, true, true );

		if ( true == bOnlyModule )
		{
			// 일단 Target과 동일하게 AddUnit을 받은 상태이어야 한다.
			bool const bRecvAddUnit = ( SYNC_TYPE_RECV_ADD & pkTarget->GetSyncType() );
			if ( true == bRecvAddUnit )
			{
				PgSmallArea *pkTargetArea = GetArea( pkTarget->LastAreaIndex() );
				if ( pkTargetArea )
				{
					SendAddUnitAreaData( kNetModule, pkTargetArea, NULL, kCharID );
				}
			}
		}
		return S_OK;
	}

	return E_FAIL;
}

HRESULT PgGround::SetOBMode_End( BM::GUID const &kCharID )
{
	BM::GUID kOwnerID;
	if ( SUCCEEDED( PgObjectMgr2::UnRegistObserver( kCharID, &kOwnerID ) ) )
	{
// 		int iAreaIndex = PgSmallArea::NONE_AREA_INDEX;
// 		CUnit *pkOwnerUnit = PgObjectMgr::GetUnit( kOwnerID );
// 		if ( pkOwnerUnit )
// 		{
// 			iAreaIndex = pkOwnerUnit->LastAreaIndex();
// 		}

		PgPlayer *pkMyPlayer = dynamic_cast<PgPlayer*>(PgObjectMgr::GetUnit( UT_PLAYER, kCharID ));
		if ( pkMyPlayer )
		{
			BM::Stream kEndPacket( PT_M_C_NFY_OBMODE_END );
			pkMyPlayer->Send( kEndPacket, E_SENDTYPE_SELF|E_SENDTYPE_SEND_BYFORCE);

			// 원래 Area정보로 복구 시켜 주어야 한다~
			pkMyPlayer->SetSyncType( SYNC_TYPE_DEFAULT, false );

// 			if ( pkMyPlayer->LastAreaIndex() != iAreaIndex )
// 			{
// 				PgSmallArea *pkCasterArea = GetArea( pkMyPlayer->LastAreaIndex() );
// 				PgSmallArea *pkTargetArea = GetArea( iAreaIndex );
// 				SendAreaData( pkMyPlayer, pkCasterArea, pkTargetArea, SYNC_TYPE_RECV_ADD );// AddUnit 받기만 해야 한다.
// 			}
		}
		return S_OK;
	}
	return E_FAIL;
}

bool PgGround::ProcessObserverPacket( BM::GUID const &kCharGuid, PgNetModule<> const &kNetModule, PgPlayer *pkPlayer, BM::Stream::DEF_STREAM_TYPE const kType, BM::Stream &rkPacket )
{
	switch ( kType )
	{
	case PT_C_M_REQ_OBMODE_TARGET_CHANGE:
		{
			BM::GUID kTargetID;
			rkPacket.Pop(kTargetID);

			CUnit *pkTarget = PgObjectMgr::GetUnit( kTargetID );
			if ( pkTarget )
			{
				BM::GUID kOldTargetID;
				if ( SUCCEEDED(PgObjectMgr2::ChangeObserver( kNetModule, kCharGuid, kTargetID, pkTarget->UnitType(), kOldTargetID )) )
				{
					CUnit *pkOldTarget = PgObjectMgr::GetUnit( kOldTargetID );

					PgSmallArea *pkTargetArea = GetArea( pkTarget->LastAreaIndex() );

					if ( pkOldTarget )
					{
						PgSmallArea *pkCastArea = GetArea(pkOldTarget->LastAreaIndex());
						if (	!(SYNC_TYPE_RECV_ADD & pkOldTarget->GetSyncType()) 
							||	pkTargetArea != pkCastArea )
						{
							SendAddUnitAreaData( kNetModule, pkTargetArea, pkCastArea, kCharGuid );
						}
					}
					else
					{
						SendAddUnitAreaData( kNetModule, pkTargetArea, NULL, kCharGuid );
					}

					BM::Stream kAnsPacket( PT_M_C_NFY_OBMODE_TARGET_CHANGE, kTargetID );
					kAnsPacket.Push( false );
					kNetModule.Send( kAnsPacket, false, true );
					return true;// return true
				}
			}

			BM::Stream kFailedPacket( PT_M_C_ANS_OBMODE_TARGET_FAILED, kTargetID );
			kNetModule.Send( kFailedPacket, false, true );
		}break;
	case PT_C_S_NFY_UNIT_POS:
		{// 핑은 처리해야 하는데.
			if ( pkPlayer )
			{
				DWORD dwLatancy = 0;
				POINT3 pt3Pos;

				rkPacket.Pop(dwLatancy);
				rkPacket.Pop(pt3Pos);

				pkPlayer->RecvLatency(dwLatancy, pt3Pos);
			}
		}break;
	default:
		{
			CAUTION_LOG( BM::LOG_LV5, __FL__ << _T("Unknown Packet Type<") << kType << _T("> CharGuid<") << kCharGuid << _T(">") );
			return false;
		}break;
	}
	return true;
}

HRESULT PgGround::SetEvent( TBL_EVENT const &kTblEvent )
{
	BM::CAutoMutex kLock( m_kRscMutex );

	PgEventAbil * pkEventAbil = dynamic_cast< PgEventAbil * >(&g_kEventView);
	if ( m_pkEventAbil == pkEventAbil )
	{
		m_pkEventAbil = new PgEventAbil;
	}

	if ( m_pkEventAbil )
	{
		switch( kTblEvent.iEventType )
		{
		case ET_EXP_BONUS_RATE:
			{
				m_pkEventAbil->AddAbil( AT_ADD_EXP_PER, kTblEvent.aIntData[0], true );
			}break;
		case ET_MONEY_DROP_RATE:
			{
				m_pkEventAbil->AddAbil( AT_ADD_MONEY_RATE, kTblEvent.aIntData[0], true );
			}break;
		case ET_MONEY_GIVE_RATE:
			{
				m_pkEventAbil->AddAbil( AT_ADD_MONEY_PER, kTblEvent.aIntData[0], true );
			}break;
		default:
			{
				return S_FALSE;
			}break;
		}

		if ( true == m_pkEventAbil->IsEmpty() )
		{
			if ( m_pkEventAbil != pkEventAbil )
			{
				SAFE_DELETE( m_pkEventAbil );
				m_pkEventAbil = pkEventAbil;
			}
		}

		return S_OK;
	}
	else
	{
		m_pkEventAbil = pkEventAbil;
	}

	return E_FAIL;
}