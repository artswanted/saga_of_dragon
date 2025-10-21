#include "stdafx.h"
#include "PgParty.h"
#include "PgEventView.h"
#include "PgHardCoreDungeonParty.h"

//---------------------------------------------------
//-- PgPartyContents_HardCoreDungeon
//---------------------------------------------------
PgPartyContents_HardCoreDungeon::PgPartyContents_HardCoreDungeon(void)
:	m_byState(E_HCT_VOTE)
,	m_iMode(0)
,	m_pkExtFunction(NULL)
{
}

PgPartyContents_HardCoreDungeon::~PgPartyContents_HardCoreDungeon(void)
{
	PgPartyContents_HardCoreDungeon::Release();
}

void PgPartyContents_HardCoreDungeon::Init( PgParty &rkParty )
{
	Init( rkParty.MasterCharGuid() );

	VEC_GUID kCharGuidList;
	rkParty.GetMemberCharGuidList( kCharGuidList, rkParty.MasterCharGuid() );

	VEC_GUID::const_iterator itr = kCharGuidList.begin();
	for ( ; itr != kCharGuidList.end() ; ++itr )
	{
		m_kContMemberState.insert( std::make_pair( *itr, SMemberState() ) );
	}
}

void PgPartyContents_HardCoreDungeon::Init( BM::GUID const &kMasterGuid )
{
	m_kContMemberState.insert( std::make_pair( kMasterGuid, SMemberState(E_HCT_V_OK) ) );
}

void PgPartyContents_HardCoreDungeon::Release()
{
	if ( m_pkExtFunction )
	{
		m_pkExtFunction->Release( m_byState, m_kContMemberState );
		delete m_pkExtFunction;
		m_pkExtFunction = NULL;
	}
}

bool PgPartyContents_HardCoreDungeon::Leave( BM::GUID const &kCharGuid, bool const bIsMaster )
{
	m_kContMemberState.erase( kCharGuid );
	return bIsMaster;
}

HRESULT PgPartyContents_HardCoreDungeon::SetWaitMove()
{
	if ( E_HCT_VOTE == m_byState )
	{
		m_byState = H_HCT_READY;

		CONT_MEMBER_STATE::iterator member_itr = m_kContMemberState.begin();
		for ( ; member_itr != m_kContMemberState.end() ; ++member_itr )
		{
			if ( E_HCT_V_OK == member_itr->second.byState )
			{
				member_itr->second.byState = E_HCT_V_NONE;
			}
		}

		m_kContPetMapMoveData.clear();
		m_kContUnitSummonedMapMoveData.clear();
		m_kModifyOrder.clear();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgPartyContents_HardCoreDungeon::SetMove()
{
	if ( H_HCT_READY == m_byState )
	{
		if ( true == IsAllSetState() )
		{
			m_byState = E_HCT_PLAY;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT PgPartyContents_HardCoreDungeon::SetMemberState( BM::GUID const &kCharGuid, BYTE const byState )
{
	CONT_MEMBER_STATE::iterator member_itr = m_kContMemberState.find( kCharGuid );
	if ( member_itr != m_kContMemberState.end() )
	{
		if ( E_HCT_V_NONE == member_itr->second.byState )
		{
			member_itr->second.byState = byState;
			return S_OK;
		}

		return E_ACCESSDENIED;
	}
	return E_FAIL;
}

BYTE PgPartyContents_HardCoreDungeon::GetMemberState( BM::GUID const &kCharGuid )const
{
	CONT_MEMBER_STATE::const_iterator member_itr = m_kContMemberState.find( kCharGuid );
	if ( member_itr != m_kContMemberState.end() )
	{
		return member_itr->second.byState;
	}
	return E_HCT_V_ERROR;
}

bool PgPartyContents_HardCoreDungeon::IsAllSetState(void)const
{
	CONT_MEMBER_STATE::const_iterator member_itr = m_kContMemberState.begin();
	for ( ; member_itr != m_kContMemberState.end() ; ++member_itr )
	{
		if ( E_HCT_V_NONE == member_itr->second.byState )
		{
			return false;
		}
	}
	return true;
}

HRESULT PgPartyContents_HardCoreDungeon::SetMemberMoveReady( BM::GUID const &kCharGuid, BM::GUID const &kVolatileID )
{
	CONT_MEMBER_STATE::iterator member_itr = m_kContMemberState.find( kCharGuid );
	if ( member_itr != m_kContMemberState.end() )
	{
		if ( E_HCT_V_NONE == member_itr->second.byState )
		{
			member_itr->second.kVolatileID = kVolatileID;
			member_itr->second.byState = E_HCT_V_OK;
			return S_OK;
		}

		return E_ACCESSDENIED;
	}
	return E_FAIL;
}

void PgPartyContents_HardCoreDungeon::WriteToPacket( BM::Stream &kPacket )const
{
	kPacket.Push( m_byState );
	kPacket.Push( m_iMode );
	m_kDungeonGndKey.WriteToPacket( kPacket );
	PU::TWriteTable_AA( kPacket, m_kContMemberState );
}

bool PgPartyContents_HardCoreDungeon::ReadFromPacket( BM::Stream &kPacket )
{
	m_kContMemberState.clear();

	kPacket.Pop( m_byState );
	kPacket.Pop( m_iMode );
	m_kDungeonGndKey.ReadFromPacket( kPacket );
	return PU::TLoadTable_AA( kPacket, m_kContMemberState );
}

void PgPartyContents_HardCoreDungeon::WriteToPacket_JoinDungeonOrder( BM::Stream &rkPacket )const
{
	size_t const iWrPos = rkPacket.WrPos();
	rkPacket.Push( m_kContMemberState.size() );

	size_t iPushSize = 0;
	CONT_MEMBER_STATE::const_iterator member_itr = m_kContMemberState.begin();
	for ( ; member_itr != m_kContMemberState.end() ; ++member_itr )
	{
		if ( E_HCT_V_OK == member_itr->second.byState )
		{
			rkPacket.Push( member_itr->first );
			rkPacket.Push( member_itr->second.kVolatileID );
			++iPushSize;
		}
	}

	if ( iPushSize != m_kContMemberState.size() )
	{
		rkPacket.ModifyData( iWrPos, &iPushSize, sizeof(iPushSize) );
	}

	PU::TWriteTable_AM( rkPacket, m_kContPetMapMoveData );
	PU::TWriteTable_AM( rkPacket, m_kContUnitSummonedMapMoveData );
	m_kModifyOrder.WriteToPacket( rkPacket );
}

void PgPartyContents_HardCoreDungeon::ReadFromPacket_MapMoveOrder( BM::Stream &rkPacket )
{
	PU::TLoadTable_AM( rkPacket, m_kContPetMapMoveData );
	PU::TLoadTable_AM( rkPacket, m_kContUnitSummonedMapMoveData );
	m_kModifyOrder.ReadFromPacket( rkPacket );
}

//---------------------------------------------------
//-- PgHardCoreDungeonParty
//---------------------------------------------------
PgHardCoreDungeonParty::PgHardCoreDungeonParty(void)
:	m_i64BeginTime(0i64)
{
}

PgHardCoreDungeonParty::~PgHardCoreDungeonParty(void)
{
}

HRESULT PgHardCoreDungeonParty::Init( BM::GUID const &kPartyID, BM::GUID const &kMasterID, VEC_GUID const &kMemberList )
{
	m_kPartyID = kPartyID;
	m_kMasterID = kMasterID;

	VEC_GUID::const_iterator guid_itr = kMemberList.begin();
	for ( ; guid_itr != kMemberList.end() ; ++guid_itr )
	{
		m_kContMemberState.insert( std::make_pair( *guid_itr, E_HCT_V_OK ) );
	}

	m_kContMemberState.insert( std::make_pair( m_kMasterID, E_HCT_V_OK ) );
	m_byState = H_HCT_READY;
	m_i64BeginTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
	return S_OK;
}
