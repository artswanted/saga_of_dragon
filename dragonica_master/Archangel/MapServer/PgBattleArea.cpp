#include "StdAfx.h"
#include "PgBattleArea.h"

PgBattleArea::PgBattleArea()
:	m_iAttackerCount(0)
,	m_iDefencerCount(0)
{
}

PgBattleArea::~PgBattleArea()
{}

PgBattleArea::PgBattleArea( PgGTrigger_BattleArea const &rhs )
:	PgGTrigger_BattleArea( rhs )
,	m_iAttackerCount(0)
,	m_iDefencerCount(0)
{
}

PgBattleArea::PgBattleArea( PgBattleArea const &rhs )
:	PgGTrigger_BattleArea( rhs )
,	m_iAttackerCount(rhs.m_iAttackerCount)
,	m_iDefencerCount(rhs.m_iDefencerCount)
{

}

PgBattleArea& PgBattleArea::operator=( PgBattleArea const &rhs )
{
	PgGTrigger_BattleArea::operator = ( rhs );
	m_iAttackerCount = rhs.m_iAttackerCount;
	m_iDefencerCount = rhs.m_iDefencerCount;
	return *this;
}

HRESULT PgBattleArea::AddUser( BM::GUID const &kID, int const iTeam )
{
	auto kPair = m_kContUserList.insert( std::make_pair( kID, iTeam ) );
	if ( true == kPair.second )
	{
		if ( TEAM_ATTACKER == iTeam )
		{
			++m_iAttackerCount;
		}
		else if ( TEAM_DEFENCER == iTeam )
		{
			++m_iDefencerCount;
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgBattleArea::RemoveUser( BM::GUID const &kID )
{
	CONT_USER_LIST::iterator itr = m_kContUserList.find( kID );
	if ( itr != m_kContUserList.end() )
	{
		if ( TEAM_ATTACKER == itr->second )
		{
			if ( m_iAttackerCount > 0 ){--m_iAttackerCount;}
		}
		else if ( TEAM_DEFENCER == itr->second )
		{
			if ( m_iDefencerCount > 0 ){--m_iDefencerCount;}
		}
		m_kContUserList.erase( itr );
		return S_OK;
	}
	return E_FAIL;
}

size_t PgBattleArea::GetUserCount( int const iTeam )const
{
	switch ( iTeam )
	{
	case TEAM_NONE:{return m_iAttackerCount+m_iDefencerCount;}
	case TEAM_ATTACKER:{return m_iAttackerCount;}
	case TEAM_DEFENCER:{return m_iDefencerCount;}
	}
	return 0;
}

void PgBattleArea::WriteToPacket( BM::Stream &kPacket )const
{
	kPacket.Push( GetParam() );
	PU::TWriteTable_AA( kPacket, m_kContUserList );
}
