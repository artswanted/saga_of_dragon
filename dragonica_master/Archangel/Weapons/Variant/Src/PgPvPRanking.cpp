#include "stdafx.h"
#include "PgPvPRanking.h"

// PgPvPRanking
PgPvPRanking::PgPvPRanking(void)
:	m_i64LastUpdateTime(0i64)
{

}

PgPvPRanking::~PgPvPRanking(void)
{

}

void PgPvPRanking::Init( VEC_PVP_RANKING &kVecPvPRanking )
{
	m_kContRank.clear();

	std::sort( kVecPvPRanking.begin(), kVecPvPRanking.end(), std::greater<VEC_PVP_RANKING::value_type>() );

	size_t iIndex = 0;
	VEC_PVP_RANKING::const_iterator itr = kVecPvPRanking.begin();
	for ( ; itr != kVecPvPRanking.end() ; ++itr )
	{
		auto kPair = m_kContRank.insert( std::make_pair( itr->kCharacterGuid, m_kRank + iIndex ) );
		if ( kPair.second )
		{
			m_kRank[iIndex] = *itr; 
			if ( ++iIndex >= ms_iMaxRank )
			{
				break;
			}
		}
		else
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Overapping CharacterGuid<" << itr->kCharacterGuid << L">" );
		}
	}

	m_i64LastUpdateTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
}

bool PgPvPRanking::Update( SPvPRanking const &kRankingInfo )
{
	CONT_RANK::iterator itr = m_kContRank.find( kRankingInfo.kCharacterGuid );
	if ( itr != m_kContRank.end() )
	{
		SPvPRanking * pkRank = itr->second;
		if ( pkRank )
		{
			if ( *pkRank == kRankingInfo )
			{
				// 같으면 업데이트 할 필요 없다.
				return false;
			}

			size_t iIndex = GetRank( pkRank );
			if ( 1 < iIndex )
			{
				size_t iPrevIndex = --iIndex;
				iIndex = iPrevIndex - 1;

				do
				{
					if ( m_kRank[iIndex] < kRankingInfo )
					{
						CONT_RANK::iterator other_itr = m_kContRank.find( m_kRank[iIndex].kCharacterGuid );
						if ( other_itr != m_kContRank.end() )
						{
							other_itr->second = m_kRank + iPrevIndex;
						}
						m_kRank[iPrevIndex] = m_kRank[iIndex];
					}
					else
					{
						break;
					}

					iPrevIndex = iIndex;
				}while( iIndex-- );

				itr->second = m_kRank + iPrevIndex;
				pkRank = itr->second;
			}

			*pkRank = kRankingInfo;
		}
	}
	else
	{
		// 못찾으면 마지막 놈과 일단 비교
		if ( m_kRank[ms_iMaxRank-1] < kRankingInfo )
		{
			size_t iPrevIndex = ms_iMaxRank-1;

			m_kContRank.erase( m_kRank[iPrevIndex].kCharacterGuid );// 한놈이 100위밖으로 밀려났다.
			size_t iIndex = iPrevIndex - 1;
			do
			{
				if ( m_kRank[iIndex] < kRankingInfo )
				{
					CONT_RANK::iterator itr = m_kContRank.find( m_kRank[iIndex].kCharacterGuid );
					if ( itr != m_kContRank.end() )
					{
						itr->second = m_kRank + iPrevIndex;
					}
					m_kRank[iPrevIndex] = m_kRank[iIndex];
				}
				else
				{
					break;
				}

				iPrevIndex = iIndex;
			}while( --iIndex );

			m_kContRank.insert( std::make_pair( kRankingInfo.kCharacterGuid, m_kRank + iPrevIndex ));
			m_kRank[iPrevIndex] = kRankingInfo;
		}
		else
		{
			return false;// 이놈은 원래 랭킹밖이라 갱신조차 안됬다.
		}
	}

	m_i64LastUpdateTime = g_kEventView.GetLocalSecTime( CGameTime::DEFAULT );
	return true;
}

size_t PgPvPRanking::GetRank( SPvPRanking *pkRank )const
{
	size_t iRet = 0;
	if ( m_kRank <= pkRank )
	{
		unsigned __int64 const iBeginAddress = reinterpret_cast<unsigned __int64>(m_kRank);
		unsigned __int64 const iAddress = reinterpret_cast<unsigned __int64>(pkRank);
		iRet = static_cast<size_t>(iAddress - iBeginAddress);
		iRet /= sizeof(SPvPRanking);
		if ( iRet < ms_iMaxRank )
		{
			++iRet;
		}
		else
		{
			iRet = 0;
		}
	}
	return iRet;
}

bool PgPvPRanking::GetRanking( size_t iRank, SPvPRanking &rkOut )const
{
	if ( 0 < iRank && iRank <= ms_iMaxRank )
	{
		rkOut = m_kRank[iRank-1];
		return true;
	}
	return false;
}

size_t PgPvPRanking::GetRank( BM::GUID const &kCharGuid, SPvPRanking *pOutRanking )const
{
	CONT_RANK::const_iterator itr = m_kContRank.find( kCharGuid );
	if ( itr != m_kContRank.end() )
	{
		if ( pOutRanking )
		{
			*pOutRanking = *(itr->second);
		}
		return GetRank( itr->second );
	}
	return 0;
}

void PgPvPRanking::WriteToPacket( BM::Stream &rkPacket )const
{
	rkPacket.Push( m_i64LastUpdateTime );
	for ( size_t i = 0; i < ms_iMaxRank ; ++i )
	{
		m_kRank[i].WriteToPacket( rkPacket );
	}
	
}

bool PgPvPRanking::ReadFromPacket( BM::Stream &rkPacket )
{
	m_kContRank.clear();

	rkPacket.Pop( m_i64LastUpdateTime );
	for ( size_t i = 0; i < ms_iMaxRank ; ++i )
	{
		if ( true == m_kRank[i].ReadFromPacket( rkPacket ) )
		{
			m_kContRank.insert( std::make_pair( m_kRank[i].kCharacterGuid, m_kRank + i ) );
		}
		else
		{
			return false;
		}
	}
	return true;
}

// PgPvPRankingMgr
PgPvPRankingMgr::PgPvPRankingMgr(void)
:	m_bInit(false)
{
}

PgPvPRankingMgr::~PgPvPRankingMgr(void)
{

}

void PgPvPRankingMgr::Init( VEC_PVP_RANKING &kVecPvPRanking )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	Instance()->Init( kVecPvPRanking );
	m_bInit = true;
}

bool PgPvPRankingMgr::Update( SPvPRanking const &kRankingInfo )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	if ( true == m_bInit )
	{
		return Instance()->Update( kRankingInfo );
	}
	return false;
}

bool PgPvPRankingMgr::WriteToPacket( BM::Stream &rkPacket, __int64 const i64LastUpdateTime )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	value_type const *p = Instance();
	if ( i64LastUpdateTime != p->LastUpdateTime() )
	{
		p->WriteToPacket( rkPacket );
		return true;
	}
	return false;
}

void PgPvPRankingMgr::WriteToPacket( BM::Stream &rkPacket )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	Instance()->WriteToPacket( rkPacket );
}

bool PgPvPRankingMgr::ReadFromPacket( BM::Stream &rkPacket )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	m_bInit = Instance()->ReadFromPacket( rkPacket );
	return m_bInit;
}

bool PgPvPRankingMgr::GetRanking( size_t iRank, SPvPRanking &rkOut )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetRanking( iRank, rkOut );
}

size_t PgPvPRankingMgr::GetRank( BM::GUID const &kCharGuid, SPvPRanking *pOutRanking )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetRank( kCharGuid, pOutRanking );
}

__int64 PgPvPRankingMgr::GetLastUpdateTime(void)const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->LastUpdateTime();
}
