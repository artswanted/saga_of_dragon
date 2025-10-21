#include "stdafx.h"
#include "PgHardCoreDungeon.h"

PgHardCoreDungeon::PgHardCoreDungeon(void)
:	m_i64EndTime(_I64_MAX)
,	m_iBossGroundNo(0)
,	m_bIsDelete(false)
,	m_iModeID(0)
{

}

PgHardCoreDungeon::~PgHardCoreDungeon(void)
{

}

void PgHardCoreDungeon::Clear()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	PgGround::Clear();
	m_i64EndTime = _I64_MAX;
	m_iBossGroundNo = 0;
	m_bIsDelete = false;
	m_iModeID = 0;
	m_kHardBossGndInfo = SHardBossGndInfo();
}

bool PgHardCoreDungeon::Clone( PgHardCoreDungeon *pkGround )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	m_i64EndTime = pkGround->m_i64EndTime;
	m_iBossGroundNo = pkGround->m_iBossGroundNo;
	m_bIsDelete = pkGround->m_bIsDelete;
	m_iModeID = pkGround->m_iModeID;
	return PgGround::Clone( dynamic_cast<PgGround*>(pkGround) );
}

bool PgHardCoreDungeon::Clone( PgGround *pkGround )
{
	BM::CAutoMutex kLock( m_kRscMutex );
	PgHardCoreDungeon *pkHDC = dynamic_cast<PgHardCoreDungeon*>(pkGround);
	if ( pkHDC )
	{
		return Clone( pkHDC );
	}

	PgGround::Clone( pkGround );
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

EOpeningState PgHardCoreDungeon::Init( int const iID, __int64 const i64EndTime, int const iBossGroundNo, int const iMonsterControlID, bool const bMonsterGen )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	m_iModeID = iID;
	m_i64EndTime = i64EndTime;
	m_iBossGroundNo = iBossGroundNo;
	return PgGround::Init( iMonsterControlID, bMonsterGen );
}

bool PgHardCoreDungeon::IsDeleteTime()const
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if ( true == m_bIsDelete )
	{
		return ( 0 == PgObjectMgr::GetUnitCount(UT_PLAYER) );
	}
	return false;
}

void PgHardCoreDungeon::OnTick5s()
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if ( true == m_bIsDelete )
	{
		// 유저를 쫒아내야 한다.
		size_t iUserCount = 10;// 최대 10명씩 쫒아내자

		while( iUserCount-- )
		{
			PgPlayer* pkPlayer = NULL;
			CONT_OBJECT_MGR_UNIT::iterator kItor;
			PgObjectMgr::GetFirstUnit(UT_PLAYER, kItor);
			pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor));
			if ( !pkPlayer )
			{
				break;
			}

			while ( pkPlayer )
			{
				if ( true == this->RecvRecentMapMove(pkPlayer) )
				{
					break;
				}
				pkPlayer = dynamic_cast<PgPlayer*> (PgObjectMgr::GetNextUnit(UT_PLAYER, kItor));
			}
		}
	}
	else
	{
		__int64 const i64CurTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
		if ( i64CurTime >= m_i64EndTime )
		{
			m_bIsDelete = true;
		}
		else
		{
			PgGround::OnTick5s();
		}
	}
}

void PgHardCoreDungeon::WriteToPacket_AddMapLoadComplete( BM::Stream &rkPacket )const
{
	rkPacket.Push( m_i64EndTime );
	rkPacket.Push( m_kHardBossGndInfo.kPartyGuid );
	rkPacket.Push( m_kHardBossGndInfo.i64EndTime );
}

bool PgHardCoreDungeon::ReqJoinBossGround( PgPlayer *pkReqPlayer, SPMO const &kModifyOrder )
{
	BM::CAutoMutex Lock(m_kRscMutex);
	if ( !pkReqPlayer )
	{
		LIVE_CHECK_LOG(BM::LOG_LV0, __FL__ << L"Player is NULL");
		return false;
	}

	BM::GUID const kPartyGuid = pkReqPlayer->PartyGuid();
	if (	BM::GUID::IsNotNull(kPartyGuid) 
		&&	!m_kLocalPartyMgr.IsMaster( kPartyGuid, pkReqPlayer->GetID() )
		)
	{
		pkReqPlayer->SendWarnMessage( 402010 );// 개인 또는 파티장이 사용해야 합니다.
		return false;
	}

	if ( m_kHardBossGndInfo.kGndKey.IsEmpty() )
	{// 만들어야 한다.

		if ( m_iBossGroundNo )
		{
			SReqMapMove_MT kRMM(MMET_None);
			kRMM.kTargetKey.Set( m_iBossGroundNo, BM::GUID::Create() );

			PgReqMapMove kMapMove( this, kRMM, NULL );
			if ( kMapMove.Add( pkReqPlayer ) )
			{
				if ( pkReqPlayer->HaveParty() )
				{
					if ( !AddPartyMember( pkReqPlayer, kMapMove ) )
					{
						return false;
					}
				}

				kMapMove.AddModifyOrder( kModifyOrder );
				if ( true == kMapMove.DoAction() )
				{
					m_kHardBossGndInfo.kGndKey = kRMM.kTargetKey;
					m_kHardBossGndInfo.kPartyGuid = kPartyGuid;
				}
			}
		}
	}

	if ( m_kHardBossGndInfo.i64EndTime )
	{
		pkReqPlayer->SendWarnMessage( 402011 );// 다른파티가 보스전 진행중입니다.
	}
	else
	{
		pkReqPlayer->SendWarnMessage( 402014 );// 다른 파티의 보스전 요청을 처리중입니다.
	}
	
	return false;
}

bool PgHardCoreDungeon::RecvGndWrapped( unsigned short usType, BM::Stream* const pkPacket )
{
	switch ( usType )
	{
	case PT_M_M_REQ_READY_HARDCORE_BOSS:
		{
			SGroundKey kGndKey;
			kGndKey.ReadFromPacket( *pkPacket );

			if (	m_kHardBossGndInfo.kGndKey == kGndKey
				&&	!m_bIsDelete )
			{
				BM::Stream kPacket( PT_M_M_ANS_READY_HARDCORE_BOSS );
				GroundKey().WriteToPacket( kPacket );
				kPacket.Push( true );
				kPacket.Push( m_i64EndTime );
				::SendToGround( kGndKey, kPacket );
			}
			else
			{	
				BM::Stream kPacket( PT_M_M_ANS_READY_HARDCORE_BOSS );
				GroundKey().WriteToPacket( kPacket );
				kPacket.Push( false );
				::SendToGround( kGndKey, kPacket );
			}
		}break;
	case PT_M_M_NFY_HARDCORE_BOSS_ENDTIME:
		{
			SGroundKey kGndKey;
			kGndKey.ReadFromPacket( *pkPacket );

			if ( m_kHardBossGndInfo.kGndKey == kGndKey )
			{
				pkPacket->Pop( m_kHardBossGndInfo.i64EndTime );
				if ( m_kHardBossGndInfo.i64EndTime )
				{
					m_kHardBossGndInfo.i64BeginTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
				}
				else
				{
					m_kHardBossGndInfo = SHardBossGndInfo();
				}
				
				BM::Stream kCPacket( PT_M_C_NFY_HARDCORE_BOSS_ENDTIME, m_kHardBossGndInfo.kPartyGuid );
				kCPacket.Push( m_kHardBossGndInfo.i64EndTime );
				Broadcast( kCPacket );
			}
		}break;
	default:
		{
			return PgGround::RecvGndWrapped( usType, pkPacket );
		}break;
	}
	return true;
}