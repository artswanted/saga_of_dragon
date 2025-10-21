#include "stdafx.h"
#include "PgHardCoreDungeonSystem.h"

PgHardCoreDungeonSystem::PgHardCoreDungeonSystem( int const iMode )
:	m_iMode(iMode)
,	m_i64BeginTime(0i64)
,	m_i64EndTime(0i64)
,	m_iLimitLevel(0)
,	m_iBossGroundNo(0)
{

}

PgHardCoreDungeonSystem::~PgHardCoreDungeonSystem(void)
{
}

int PgHardCoreDungeonSystem::IsCanJoin( int const iLevel, SGroundKey &rkOutGndKey )const
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( true == IsOpen() )
	{
		if ( m_iLimitLevel > iLevel )
		{
			return 72024;
		}
		rkOutGndKey = m_kGroundKey;
		return 0;
	}
	return 200153;
}

HRESULT PgHardCoreDungeonSystem::ReqOpen( __int64 i64BeginTime, __int64 i64EndTime, int const iLimitLevel, int const iGroundNo, int const iBossGroundNo )
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( 0 == m_kGroundKey.GroundNo() )
	{
		m_i64BeginTime = i64BeginTime;
		m_i64EndTime = i64EndTime;
		m_iLimitLevel = iLimitLevel;
		m_iBossGroundNo = iBossGroundNo;

		SGroundMakeOrder kOrder;
		kOrder.kKey.GroundNo( iGroundNo );
		kOrder.kKey.Guid( BM::GUID::Create() );

		BM::Stream kCreatePacket( PT_N_T_REQ_CREATE_PUBLICMAP, PMET_HARDCORE_DUNGEON );
		kOrder.WriteToPacket( kCreatePacket );
		kCreatePacket.Push( m_iMode );
		kCreatePacket.Push( m_i64EndTime );
		kCreatePacket.Push( m_iBossGroundNo );
		if ( true == ::SendToCenter( CProcessConfig::GetPublicChannel(), kCreatePacket ) )
		{
			m_kGroundKey.GroundNo( iGroundNo );
			return S_OK;
		}

		CAUTION_LOG( BM::LOG_LV2, L"[HardCoreDungeonSystem] Mode<" << m_iMode << L"> Open Dungeon Failed(Not Connect Public Center) : " << iGroundNo );
	}
	return E_FAIL;
}

HRESULT PgHardCoreDungeonSystem::RecvOpenResult( HRESULT const hRet, SGroundKey const &kGndKey )
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( kGndKey.GroundNo() == m_kGroundKey.GroundNo() )
	{
		if ( SUCCEEDED(hRet) )
		{
			m_kGroundKey.Guid( kGndKey.Guid() );
			INFO_LOG( BM::LOG_LV5, L"[HardCoreDungeonSystem] Mode<" << m_iMode << L"> Open Dungeon : " << kGndKey.GroundNo() ); 	
		}
		else
		{
			CAUTION_LOG( BM::LOG_LV5, L"[HardCoreDungeonSystem] Mode<" << m_iMode << L"> Open Dungeon Failed : " << kGndKey.GroundNo() );
			m_kGroundKey.Clear();
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgHardCoreDungeonSystem::RecvClose( SGroundKey const &kGndKey )
{
	BM::CAutoMutex kLock( m_kMutex );
	if ( kGndKey == m_kGroundKey )
	{
		SetClose();
		return S_OK;
	}
	return E_FAIL;
}

void PgHardCoreDungeonSystem::SetClose()
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( true == IsOpen() )
	{
		INFO_LOG( BM::LOG_LV5, L"[HardCoreDungeonSystem] Mode<" << m_iMode << L"> Close Dungeon : " << m_kGroundKey.GroundNo() );
		m_kGroundKey.Clear();
	}
}
