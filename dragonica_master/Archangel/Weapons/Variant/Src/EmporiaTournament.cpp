#include "stdafx.h"
#include "Variant/PgEmporia.h"
#include "EmporiaTournament.h"

PgEmporiaTournamentElement::PgEmporiaTournamentElement()
:	m_iIndex(0)
,	m_i64BattleTime(0i64)
,	m_pkAttackGuilDInfo(NULL)
,	m_pkDefenceGuilDInfo(NULL)
,	m_pkParent(NULL)
,	m_byState(STATE_NONE)
{
}

PgEmporiaTournamentElement::~PgEmporiaTournamentElement()
{
}

HRESULT PgEmporiaTournamentElement::SetWinner( BM::GUID const &kGuildGuid )
{
	tagEmporiaGuildInfoEx * pkWinGuildInfo = NULL;

	if (	m_pkDefenceGuilDInfo 
	&&		m_pkDefenceGuilDInfo->kGuildID == kGuildGuid )
	{
		m_byState = PgEmporiaTournamentElement::FLAG_WIN_DEFENCE;
		pkWinGuildInfo = m_pkDefenceGuilDInfo;
	}
	else if (	m_pkAttackGuilDInfo 
			&&	m_pkAttackGuilDInfo->kGuildID == kGuildGuid )
	{
		m_byState = PgEmporiaTournamentElement::FLAG_WIN_ATTACK;
		pkWinGuildInfo = m_pkAttackGuilDInfo;
	}
	else
	{
		return E_FAIL;
	}

	if ( m_pkParent )
	{
		if ( true == IsParentLinkAttack() )
		{
			m_pkParent->m_pkAttackGuilDInfo = pkWinGuildInfo;
		}
		else
		{
			m_pkParent->m_pkDefenceGuilDInfo = pkWinGuildInfo;
		}
		return S_OK;
	}
	return S_FALSE;
}

HRESULT PgEmporiaTournamentElement::GetWinnerInfo( tagEmporiaGuildInfo &kOutGuildInfo )const
{
	if ( PgEmporiaTournamentElement::FLAG_WIN_DEFENCE & m_byState )
	{
		if ( m_pkDefenceGuilDInfo )
		{
			if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID != m_pkDefenceGuilDInfo->kGuildID )
			{
				kOutGuildInfo = *m_pkDefenceGuilDInfo;
				return S_OK;
			}
		}
	}
	else if ( PgEmporiaTournamentElement::FLAG_WIN_ATTACK & m_byState )
	{
		if ( m_pkAttackGuilDInfo )
		{
			if ( PgEmporiaTournament::ms_kEmptyGuildInfo.kGuildID != m_pkAttackGuilDInfo->kGuildID )
			{
				kOutGuildInfo = *m_pkAttackGuilDInfo;
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT PgEmporiaTournamentElement::GetParentIndex( size_t const iIndex, size_t &iOutParentIndex )
{
	if ( iIndex > 0 )
	{
		iOutParentIndex = ((iIndex-1) / 2);
		return S_OK;
	}
	return E_FAIL;
}

void PgEmporiaTournamentElement::GetChildIndex( size_t const iIndex, size_t &iOutChildLeft, size_t &iOutChildRight )
{
	iOutChildLeft = iIndex * 2 + 1;
	iOutChildRight = iOutChildLeft+1;
}

HRESULT PgEmporiaTournamentElement::GetGuildInfo( bool const bAttacker, tagEmporiaGuildInfo &kOutGuildInfo )const
{
	if ( true == bAttacker )
	{
		if ( m_pkAttackGuilDInfo )
		{
			kOutGuildInfo = *m_pkAttackGuilDInfo;
			return S_OK;
		}
	}
	else
	{
		if ( m_pkDefenceGuilDInfo )
		{
			kOutGuildInfo = *m_pkDefenceGuilDInfo;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT PgEmporiaTournamentElement::GetGuildInfoEx( bool const bAttacker, tagEmporiaGuildInfoEx &kOutGuildInfo )const
{
	if ( true == bAttacker )
	{
		if ( m_pkAttackGuilDInfo )
		{
			kOutGuildInfo = *m_pkAttackGuilDInfo;
			return S_OK;
		}
	}
	else
	{
		if ( m_pkDefenceGuilDInfo )
		{
			kOutGuildInfo = *m_pkDefenceGuilDInfo;
			return S_OK;
		}
	}
	return E_FAIL;
}

BM::GUID PgEmporiaTournamentElement::GetGuildGuid( bool const bAttacker )const
{
	if ( true == bAttacker )
	{
		if ( m_pkAttackGuilDInfo )
		{
			return m_pkAttackGuilDInfo->kGuildID;
		}
	}
	else
	{
		if ( m_pkDefenceGuilDInfo )
		{
			return m_pkDefenceGuilDInfo->kGuildID;
		}
	}
	return BM::GUID::NullData();
}

PgLimitClass const PgEmporiaTournamentElement::GetMercenaryLimit()
{
	PgLimitClass kLimit;
	if( m_pkAttackGuilDInfo )
	{
		if(m_pkAttackGuilDInfo->m_kLimitMercenary.IsUsing())
		{
			kLimit += m_pkAttackGuilDInfo->m_kLimitMercenary;
		}
	}
	if ( m_pkDefenceGuilDInfo )
	{
		if(m_pkDefenceGuilDInfo->m_kLimitMercenary.IsUsing())
		{
			kLimit += m_pkDefenceGuilDInfo->m_kLimitMercenary;
		}
	}
	return kLimit;
}

HRESULT PgEmporiaTournamentElement::SaveMercenary( bool const bAttacker, VEC_GUID const & rkMember )
{
	tagEmporiaGuildInfoEx* pkInfo = (bAttacker ? m_pkAttackGuilDInfo : m_pkDefenceGuilDInfo);
	if( !pkInfo ){ return E_FAIL; }

	pkInfo->kContMercenary = rkMember;
	return S_OK;
}

bool PgEmporiaTournamentElement::IsGuild( bool const bAttacker, BM::GUID const &kGuildGuid )const
{
	return kGuildGuid == GetGuildGuid(bAttacker);
}

bool PgEmporiaTournamentElement::IsAccess( bool const bAttacker, PgLimitClass const &kLimitClass )const
{
	if ( true == bAttacker )
	{
		if ( m_pkAttackGuilDInfo )
		{
			return m_pkAttackGuilDInfo->m_kLimitMercenary.IsAccess( kLimitClass );
		}
	}
	else
	{
		if ( m_pkDefenceGuilDInfo )
		{
			return m_pkDefenceGuilDInfo->m_kLimitMercenary.IsAccess( kLimitClass );
		}
	}
	return false;
}

HRESULT PgEmporiaTournamentElement::IsBattle()const
{
	if ( m_pkAttackGuilDInfo && m_pkDefenceGuilDInfo )
	{
		if ( FLAG_CHECK_END & m_byState )
		{
			return E_FAIL;
		}

		return S_OK;
	}
	return E_ACCESSDENIED;
}


void PgEmporiaTournamentElement::GetMecenaryList(VEC_GUID & rkContList)
{
	if(m_pkAttackGuilDInfo)
	{
		rkContList.insert(rkContList.end(), m_pkAttackGuilDInfo->kContMercenary.begin(),m_pkAttackGuilDInfo->kContMercenary.end());
	}
	if(m_pkDefenceGuilDInfo)
	{
		rkContList.insert(rkContList.end(), m_pkDefenceGuilDInfo->kContMercenary.begin(),m_pkDefenceGuilDInfo->kContMercenary.end());
	}
}

bool PgEmporiaTournamentElement::IsBeforJoinMercenary(bool const bIsAttack, BM::GUID const & kCharGuid)const
{
	tagEmporiaGuildInfoEx* m_pkInfo = (bIsAttack ? m_pkAttackGuilDInfo : m_pkDefenceGuilDInfo);
	if( !m_pkInfo ){ return false; }

	CONT_EMPORIA_MERCENARY const & kContUser = m_pkInfo->kContMercenary;
	CONT_EMPORIA_MERCENARY::const_iterator c_iter = std::find(kContUser.begin(), kContUser.end(), kCharGuid);
	if( c_iter!=kContUser.end() )
	{ 
		return true; 
	}

	return false;
}

PgEmporiaTournamentElement::T_WINNER_INFO PgEmporiaTournamentElement::GetWinner()const
{
	T_WINNER_INFO kInfo;
	kInfo.first = STATE_NONE;

	if ( m_pkParent )
	{
		if ( true == IsParentLinkAttack() )
		{
			kInfo.second = m_pkParent->m_pkAttackGuilDInfo;
		}
		else
		{
			kInfo.second = m_pkParent->m_pkDefenceGuilDInfo;
		}

		kInfo.first = ( FLAG_CHECK_END & m_byState );  
	}

	return kInfo;
}

SEmporiaGuildInfo const PgEmporiaTournament::ms_kEmptyGuildInfo( BM::GUID( std::wstring(L"CFC1E5BA-D16B-41AA-BE02-10B2538EEFB7") ) );

PgEmporiaTournament::PgEmporiaTournament()
:	m_kEmptyGuildInfo(ms_kEmptyGuildInfo)
{

}

PgEmporiaTournament::~PgEmporiaTournament()
{
}

HRESULT PgEmporiaTournament::Init( CONT_INIT_TIME const &kTournamentTime, CONT_INIT_TIME const &kTournamentTime2 )
{
	Release();

	if ( kTournamentTime.size() )
	{
		//	Tournament의 갯수는 1+2+4+8+16+32...(1,3,7,15,31,63...)가 되어야 한다.
		//	맞는지 검사를 하자.
		size_t iTemp = 1;
		while ( kTournamentTime.size() > iTemp )
		{
			iTemp *= 2;
		}

		if ( kTournamentTime.size() != (iTemp-1) )
		{
			return E_FAIL;
		}

		iTemp /= 2;
		--iTemp;// 요걸 가지고 계산할께 하나 더 있어

		if ( kTournamentTime2.size() > static_cast<size_t>(MAX_EMPORIA_GRADE) )
		{
			return E_FAIL;
		}

		try
		{
			m_kContTournament.resize( kTournamentTime.size() );
			m_kContTournament2.resize( kTournamentTime2.size() );

			size_t iIndex = 0;
			size_t iParentIndex = 0;

			CONT_INIT_TIME::const_iterator time_itr = kTournamentTime.begin();
			for ( ; time_itr != kTournamentTime.end() ; ++time_itr )
			{
				PgEmporiaTournamentElement &kElement = m_kContTournament.at(iIndex);
				kElement.m_iIndex = iIndex;
				kElement.m_i64BattleTime = *time_itr;

				if ( iTemp <= iIndex )
				{
					kElement.m_pkAttackGuilDInfo = &m_kEmptyGuildInfo;
					kElement.m_pkDefenceGuilDInfo = &m_kEmptyGuildInfo;
				}

				if ( S_OK == PgEmporiaTournamentElement::GetParentIndex( iIndex, iParentIndex ) )
				{
					kElement.m_pkParent = &(m_kContTournament.at(iParentIndex));
					if ( kElement.m_pkParent->GetBattleTime() < kElement.GetBattleTime() )
					{
						throw E_FAIL;
					}
				}
				else if ( 0 == iIndex )
				{
                    if( false==m_kContTournament2.empty() )
                    {
					    kElement.m_pkParent = &(m_kContTournament2.at(0));
                    }
				}

				++iIndex;
			}

			if ( m_kContTournament2.size() )
			{
				m_kContTournament2.back().m_pkDefenceGuilDInfo = &m_kEmptyGuildInfo;
			}

			iIndex = kTournamentTime2.size() - 1;
			CONT_INIT_TIME::const_reverse_iterator r_time_itr = kTournamentTime2.rbegin();
			for ( ; r_time_itr != kTournamentTime2.rend() ; ++r_time_itr )
			{
				PgEmporiaTournamentElement &kElement = m_kContTournament2.at(iIndex);
				kElement.m_iIndex = EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX + iIndex;
				kElement.m_i64BattleTime = *r_time_itr;

				iParentIndex = iIndex + 1;
				if ( iParentIndex < kTournamentTime2.size() )
				{
					kElement.m_pkParent = &(m_kContTournament2.at(iParentIndex));
					if ( kElement.m_pkParent->GetBattleTime() < kElement.GetBattleTime() )
					{
						if ( kElement.m_pkParent->m_pkParent )
						{
							throw E_FAIL;
						}
					}
				}

				--iIndex;
			}
		}
		catch ( HRESULT hRet )
		{
			m_kContGuildInfo.clear();
			m_kContTournament.clear();
			m_kContTournament2.clear();
			return hRet;
		}
	}

	return S_OK;
}

HRESULT PgEmporiaTournament::InitGuild( CONT_INIT_GUILD const &kInitGuild )
{
	{
		CONT_GUILDINFO temp;
		m_kContGuildInfo.swap(temp);//완전 제거
	}

	m_kContGuildInfo.resize( kInitGuild.size() );

	try
	{
		size_t iIndex = 0;
		size_t iParentIndex;

		CONT_INIT_GUILD::const_iterator guild_itr = kInitGuild.begin();
		for ( ; guild_itr != kInitGuild.end() ; ++guild_itr )
		{
			m_kContGuildInfo.at(iIndex) = guild_itr->second;

			if ( EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX > guild_itr->first )
			{
				if ( S_OK == PgEmporiaTournamentElement::GetParentIndex( guild_itr->first, iParentIndex ) )
				{
					if ( iParentIndex >= m_kContTournament.size() )
					{
						throw E_FAIL;
					}

					PgEmporiaTournamentElement &kElement = m_kContTournament.at(iParentIndex);
					if ( guild_itr->first % 2 )
					{
						kElement.m_pkAttackGuilDInfo = &(m_kContGuildInfo.at(iIndex));
					}
					else
					{
						kElement.m_pkDefenceGuilDInfo = &(m_kContGuildInfo.at(iIndex));
					}
				}
			}
			else
			{
				PgEmporiaTournamentElement * pkElement = GetElement( guild_itr->first );
				if ( !pkElement )
				{
					throw E_FAIL;
				}

				pkElement->m_pkDefenceGuilDInfo = &(m_kContGuildInfo.at(iIndex));
			}

			++iIndex;
		}
	}
	catch ( HRESULT hRet )
	{
		m_kContGuildInfo.clear();
		return hRet;
	}

	return S_OK;
}

HRESULT PgEmporiaTournament::InitMercenary( CONT_TBL_EM_MERCENARY_INDEX const &rkContData )
{
	for(CONT_TBL_EM_MERCENARY_INDEX::const_iterator c_itr=rkContData.begin();c_itr!=rkContData.end();++c_itr)
	{
		PgEmporiaTournamentElement * pkElement = GetElement(c_itr->nIndex);
		if(!pkElement || !pkElement->IsBattle())
		{
			continue;
		}

		for(CONT_TBL_EM_MERCENARY_JOINGUILD::const_iterator it_3=c_itr->kContJoinGuild.begin();it_3!=c_itr->kContJoinGuild.end();++it_3)
		{
			tagEmporiaGuildInfoEx* pkAttack = pkElement->m_pkAttackGuilDInfo;
			if(pkAttack && (pkAttack->kGuildID==it_3->kJoinGuildID))
			{
				pkAttack->kContMercenary = it_3->kContCharID;
				continue;
			}

			tagEmporiaGuildInfoEx* pkDefence = pkElement->m_pkDefenceGuilDInfo;
			if(pkDefence && (pkDefence->kGuildID==it_3->kJoinGuildID))
			{
				pkDefence->kContMercenary = it_3->kContCharID;
				continue;
			}
		}
	}

	return S_OK;
}

void PgEmporiaTournament::Release()
{
	CONT_GUILDINFO	kContGuildInfo;
	CONT_TOURNAMENT	kContTournament;
	CONT_TOURNAMENT	kContTournament2;
	m_kContGuildInfo.swap(kContGuildInfo);
	m_kContTournament.swap(kContTournament);
	m_kContTournament2.swap(kContTournament2);
	m_kEmptyGuildInfo = ms_kEmptyGuildInfo;
}

PgEmporiaTournamentElement* PgEmporiaTournament::GetElement( size_t const iIndex )
{
	if ( EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX > iIndex )
	{
		if ( m_kContTournament.size() > iIndex )
		{
			return &(m_kContTournament.at(iIndex));
		}
	}
	else
	{
		size_t const iNewIndex = (iIndex - EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX);
		if ( m_kContTournament2.size() > iNewIndex )
		{
			return &(m_kContTournament2.at(iNewIndex));
		}
	}
	return NULL;
}

PgEmporiaTournamentElement const * PgEmporiaTournament::GetElement( size_t const iIndex )const
{
	if ( EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX > iIndex )
	{
		if ( m_kContTournament.size() > iIndex )
		{
			return &(m_kContTournament.at(iIndex));
		}
	}
	else
	{
		size_t const iNewIndex = (iIndex - EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX);
		if ( m_kContTournament2.size() > iNewIndex )
		{
			return &(m_kContTournament2.at(iNewIndex));
		}
	}
	return NULL;
}

HRESULT	PgEmporiaTournament::SetWinner( size_t const iIndex, BM::GUID const &kWinGuildID )
{
	PgEmporiaTournamentElement* pkElement = GetElement( iIndex );
	if ( pkElement )
	{
		return pkElement->SetWinner( kWinGuildID );
	}

	return E_FAIL;
}

void PgEmporiaTournament::WriteToPacket( BM::Stream &kPacket, PgEmporiaTournamentElement const &kElement )const
{
	kPacket.Push( kElement.m_iIndex );
	kPacket.Push( kElement.m_byState );
	kPacket.Push( kElement.m_i64BattleTime );

	if ( kElement.m_pkAttackGuilDInfo )
	{
		kPacket.Push( kElement.m_pkAttackGuilDInfo->kGuildID );
	}
	else
	{
		kPacket.Push( BM::GUID::NullData() );
	}

	if ( kElement.m_pkDefenceGuilDInfo )
	{
		kPacket.Push( kElement.m_pkDefenceGuilDInfo->kGuildID );
	}
	else
	{
		kPacket.Push( BM::GUID::NullData() );
	}
}

void PgEmporiaTournament::WriteToPacket( BM::Stream &kPacket )const
{
	PU::TWriteArray_M( kPacket, m_kContGuildInfo );

	kPacket.Push( m_kContTournament2.size() );
	CONT_TOURNAMENT::const_iterator tour_itr = m_kContTournament2.begin();
	for ( ; tour_itr != m_kContTournament2.end() ; ++tour_itr )
	{
		WriteToPacket( kPacket, *tour_itr );
	}

	kPacket.Push( m_kContTournament.size() );
	tour_itr = m_kContTournament.begin();
	for ( ; tour_itr != m_kContTournament.end() ; ++tour_itr )
	{
		WriteToPacket( kPacket, *tour_itr );
	}
}

void PgEmporiaTournament::ReadFromPacket( BM::Stream &kPacket, PgEmporiaTournamentElement &kElement, bool const bIsSecond )
{
	kPacket.Pop( kElement.m_iIndex );
	kPacket.Pop( kElement.m_byState );
	kPacket.Pop( kElement.m_i64BattleTime );

	CONT_GUILDINFO::value_type kGuildInfo;
	if (	true == kPacket.Pop( kGuildInfo.kGuildID ) 
		&&	BM::GUID::IsNotNull( kGuildInfo.kGuildID ) )
	{
		if ( ms_kEmptyGuildInfo.kGuildID == kGuildInfo.kGuildID )
		{
			kElement.m_pkAttackGuilDInfo = &m_kEmptyGuildInfo;
		}
		else
		{
			CONT_GUILDINFO::iterator guild_itr = std::find( m_kContGuildInfo.begin(), m_kContGuildInfo.end(), kGuildInfo );
			if ( guild_itr != m_kContGuildInfo.end() )
			{
				kElement.m_pkAttackGuilDInfo = &(*guild_itr);
			}
		}
	}

	if (	true == kPacket.Pop( kGuildInfo.kGuildID ) 
		&&	BM::GUID::IsNotNull( kGuildInfo.kGuildID ) )
	{
		if ( ms_kEmptyGuildInfo.kGuildID == kGuildInfo.kGuildID )
		{
			kElement.m_pkDefenceGuilDInfo = &m_kEmptyGuildInfo;
		}
		else
		{
			CONT_GUILDINFO::iterator guild_itr = std::find( m_kContGuildInfo.begin(), m_kContGuildInfo.end(), kGuildInfo );
			if ( guild_itr != m_kContGuildInfo.end() )
			{
				kElement.m_pkDefenceGuilDInfo = &(*guild_itr);
			}
		}
	}

	if ( true == bIsSecond )
	{
		size_t const iParentIndex = kElement.m_iIndex - EMPORIA_BATTLE_TOURNAMENT_OWNERSHIP_STARTINDEX + 1;
		if ( m_kContTournament2.size() > iParentIndex )
		{
			kElement.m_pkParent = &(m_kContTournament2.at(iParentIndex));
		}
	}
	else
	{
		size_t iParentIndex = 0;
		if ( S_OK == PgEmporiaTournamentElement::GetParentIndex( static_cast<size_t>(kElement.m_iIndex), iParentIndex ) )
		{
			if ( m_kContTournament.size() > iParentIndex )
			{
				kElement.m_pkParent = &(m_kContTournament.at(iParentIndex));
			}
		}
		else if ( 0 == kElement.m_iIndex )
		{
			if ( m_kContTournament2.size() )
			{
				kElement.m_pkParent = &(m_kContTournament2.at(0));
			}
		}
	}
}

bool PgEmporiaTournament::ReadFromPacket( BM::Stream &kPacket )
{
	Release();

	if ( true == PU::TLoadArray_M( kPacket, m_kContGuildInfo ) )
	{
		size_t iSize = 0;

		kPacket.Pop( iSize );
		m_kContTournament2.resize(iSize);

		for ( size_t i = 0 ; i < iSize ; ++i )
		{
			PgEmporiaTournamentElement &kElement = m_kContTournament2.at(i);
			ReadFromPacket( kPacket, kElement, true );
		}

		kPacket.Pop( iSize );
		m_kContTournament.resize(iSize);

		for ( size_t i = 0 ; i < iSize ; ++i )
		{
			PgEmporiaTournamentElement &kElement = m_kContTournament.at(i);
			ReadFromPacket( kPacket, kElement, false );
		}

		return true;
	}

	return false;
}

bool PgEmporiaTournament::IsHaveGuild( BM::GUID const &kGuildID )const
{
	CONT_GUILDINFO::value_type kGuildInfo;
	kGuildInfo.kGuildID = kGuildID;

	CONT_GUILDINFO::const_iterator guild_itr = std::find( m_kContGuildInfo.begin(), m_kContGuildInfo.end(), kGuildInfo );
	if ( guild_itr != m_kContGuildInfo.end() )
	{
		return true;
	}
	return false;
}

inline bool CheckNowJoinMercenary(bool const bIsAttack, PgEmporiaTournamentElement const & rkElement, PgLimitClass const & rkLimitClass)
{
	SEmporiaGuildInfoEx kGuildInfo;
	rkElement.GetGuildInfoEx( bIsAttack, kGuildInfo );

	if ( true == kGuildInfo.m_kLimitMercenary.IsAccess( rkLimitClass ) )
	{
		PgEmporiaTournamentElement const * pkTempElement = &rkElement;
		while ( pkTempElement )
		{
			if ( PgEmporiaTournamentElement::FLAG_CHECK_END & pkTempElement->GetState() )
			{
				pkTempElement = pkTempElement->GetParent();
				continue;
			}

			if ( PgEmporiaTournamentElement::STATE_CREATED & pkTempElement->GetState() )
			{
				if( (kGuildInfo.kGuildID==pkTempElement->GetGuildGuid(true))
				|| (kGuildInfo.kGuildID==pkTempElement->GetGuildGuid(false)) )
				{
					return true;
				}
			}

			break;
		}
	}

	return false;
}

bool PgEmporiaTournament::IsNowJoinMercenary(PgLimitClass const & rkLimitClass)const
{
	for(CONT_TOURNAMENT::const_iterator c_iter=m_kContTournament.begin();c_iter!=m_kContTournament.end();++c_iter)
	{
		CONT_TOURNAMENT::value_type const & rkElement = *c_iter;

		if( CheckNowJoinMercenary(true,rkElement,rkLimitClass) )
		{
			return true;
		}

		if( CheckNowJoinMercenary(false,rkElement,rkLimitClass) )
		{
			return true;
		}
	}

	for(CONT_TOURNAMENT::const_iterator c_iter=m_kContTournament2.begin();c_iter!=m_kContTournament2.end();++c_iter)
	{
		CONT_TOURNAMENT::value_type const & rkElement = *c_iter;

		if( CheckNowJoinMercenary(true,rkElement,rkLimitClass) )
		{
			return true;
		}

		if( CheckNowJoinMercenary(false,rkElement,rkLimitClass) )
		{
			return true;
		}
	}

	return false;
}

inline bool CheckJoinMercenary(PgEmporiaTournament::CONT_TOURNAMENT const & kContTournament, BM::GUID const & kMercenaryCharID, PgLimitClass const & rkLimitClass, float & fRemainTime)
{
	fRemainTime = -1.f;

	for(PgEmporiaTournament::CONT_TOURNAMENT::const_iterator c_iter=kContTournament.begin();c_iter!=kContTournament.end();++c_iter)
	{
		PgEmporiaTournament::CONT_TOURNAMENT::value_type const & rkElement = *c_iter;

		PgEmporiaTournamentElement const * pkTempElement = &rkElement;
		while ( pkTempElement )
		{
			BYTE const byState = pkTempElement->GetState();
			
			if( PgEmporiaTournamentElement::FLAG_CHECK_END & byState )
			{
				break;
			}

			bool bLastParent = false;
			if( !pkTempElement->GetParent()
			||	(PgEmporiaTournamentElement::STATE_NONE == pkTempElement->GetParent()->GetState()) )
			{
				bLastParent = true;
			}

			if( bLastParent 
			&& ((PgEmporiaTournamentElement::STATE_RECV_START&byState) || (PgEmporiaTournamentElement::STATE_RECV_CREATE&byState)) )
			{
				if( !pkTempElement->IsAccess(true,rkLimitClass)
				&& !pkTempElement->IsAccess(false,rkLimitClass) )
				{
					break;
				}

				fRemainTime = 0.f;

				if( !(PgEmporiaTournamentElement::STATE_RECV_START&byState)
				&& !(pkTempElement->IsBeforJoinMercenary(true, kMercenaryCharID))
				&& !(pkTempElement->IsBeforJoinMercenary(false, kMercenaryCharID)) )
				{
					__int64 const i64NowTime = g_kEventView.GetLocalSecTime(CGameTime::DEFAULT);
					fRemainTime = std::max<float>(0.f, (pkTempElement->GetBattleTime()-i64NowTime) / CGameTime::SECOND * 1.f);
				}
				break;
			}

			pkTempElement = pkTempElement->GetParent();
		}

		if(fRemainTime >= 0.f)
		{
			return true;
		}
	}

	fRemainTime = 0.f;
	return false;
}

bool PgEmporiaTournament::ProcessJoinMercenary(BM::GUID const & kMercenaryCharID, BM::GUID const & kGuildID, PgLimitClass const & rkLimitClass, float & fRemainTime)const
{
	if( IsHaveGuild(kGuildID) )
	{ 
		return false; 
	}
	
	if( CheckJoinMercenary(m_kContTournament, kMercenaryCharID, rkLimitClass, fRemainTime) )
	{
		return true;
	}

	if( CheckJoinMercenary(m_kContTournament2, kMercenaryCharID, rkLimitClass, fRemainTime) )
	{
		return true;
	}

	return false;
}

HRESULT PgEmporiaTournament::SetGuildMercenary( BM::GUID const &kGuildID, PgLimitClass const &kLimitClass )
{
	CONT_GUILDINFO::iterator guild_itr = m_kContGuildInfo.begin();
	for ( ; guild_itr != m_kContGuildInfo.end() ; ++guild_itr )
	{
		if ( kGuildID == guild_itr->kGuildID )
		{
			guild_itr->m_kLimitMercenary = kLimitClass;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT PgEmporiaTournament::GetBattleTime( size_t const iIndex, __int64 &i64OutTime )const
{
	PgEmporiaTournamentElement const * pkElement = GetElement( iIndex );
	if ( pkElement )
	{
		i64OutTime = pkElement->GetBattleTime();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT PgEmporiaTournament::IsBattle( size_t const iIndex )const
{
	PgEmporiaTournamentElement const * pkElement = GetElement( iIndex );
	if ( pkElement )
	{
		return pkElement->IsBattle();
	}
	return E_FAIL;
}

void PgEmporiaTournament::SetBattleTimeAsSoon( __int64 const i64Time )
{
	__int64 i64ChkTime = _I64_MAX;
	bool bSet = false;
	CONT_TOURNAMENT::reverse_iterator r_itr = m_kContTournament.rbegin();
	for ( ; r_itr != m_kContTournament.rend() ; ++r_itr )
	{
		if ( PgEmporiaTournamentElement::STATE_NONE == r_itr->GetState() )
		{
			if ( i64ChkTime >= r_itr->GetBattleTime() )
			{
				i64ChkTime = r_itr->GetBattleTime();
				r_itr->m_i64BattleTime = i64Time;
				bSet = true;
			}
			else
			{
				return;
			}
		}
	}

	if ( !bSet )
	{
		CONT_TOURNAMENT::iterator itr = m_kContTournament2.begin();
		for ( ; itr != m_kContTournament2.end() ; ++itr )
		{
			if ( PgEmporiaTournamentElement::STATE_NONE == itr->GetState() )
			{
				if ( i64ChkTime >= itr->GetBattleTime() )
				{
					i64ChkTime = itr->GetBattleTime();
					itr->m_i64BattleTime = i64Time;
					return;
				}
			}
		}
	}
}

HRESULT PgEmporiaTournament::GetGuildInfo( size_t const iIndex, bool const bAttacker, tagEmporiaGuildInfo &kOutGuildInfo )const
{
	PgEmporiaTournamentElement const * pkElement = GetElement( iIndex );
	if ( pkElement )
	{
		return pkElement->GetGuildInfo( bAttacker, kOutGuildInfo );
	}
	return E_FAIL;
}

HRESULT PgEmporiaTournament::GetGuildInfoEx( size_t const iIndex, bool const bAttacker, tagEmporiaGuildInfoEx &kOutGuildInfo )const
{
	PgEmporiaTournamentElement const * pkElement = GetElement( iIndex );
	if ( pkElement )
	{
		return pkElement->GetGuildInfoEx( bAttacker, kOutGuildInfo );
	}
	return E_FAIL;
}

void PgEmporiaTournament::GetGuildListGuid(VEC_GUID & kContGuild)
{
	for(CONT_GUILDINFO::iterator guild_itr = m_kContGuildInfo.begin();guild_itr!=m_kContGuildInfo.end();++guild_itr)
	{
		kContGuild.push_back(guild_itr->kGuildID);
	}
}