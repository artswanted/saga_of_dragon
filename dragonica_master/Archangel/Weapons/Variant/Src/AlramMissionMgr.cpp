#include "stdafx.h"
#include "Variant/constant.h"
#include "variant/PgEventView.h"
#include "AlramMissionMgr.h"

PgAlramMissionMgr::PgAlramMissionMgr()
{}

PgAlramMissionMgr::~PgAlramMissionMgr()
{
}

ALRAM_MISSION::E_ALRAMTYPE PgAlramMissionMgr::GetAlramType( T_GNDATTR const kGndAttr )
{
	switch ( kGndAttr )
	{
	case GATTR_EMPORIABATTLE:
//	case GATTR_DEFAULT:// 테스트
		{
			return ALRAM_MISSION::AMT_GUILDWAR;
		}break;
	case GATTR_BATTLESQUARE:
		{
			return ALRAM_MISSION::AMT_BATTLESQUARE;
		}break;
	default:
		{
			// Do nothing
		}break;
	}
	return ALRAM_MISSION::AMT_NONE;
}

HRESULT PgAlramMissionMgr::Init( ALRAM_MISSION::E_ALRAMTYPE const kType, CONT_DEF_ALRAM_MISSION const &kContDefAlramMission )
{
	VEC_ALRAM_ID temp;
	VEC_ALRAM_ID tempDra;

	{
		// 깨끗이 비우고	
		m_kVecAlramID.swap(temp);

		VEC_ALRAM_MISSION vec2;
		m_kVecAlramMission.swap(vec2);

		m_kContAlramIDByClass.clear();
	}

	temp.clear();// Class체크용으로 사용한다.
	temp.resize(64);
	tempDra.clear();
	tempDra.resize(64);

	m_kAlramType = kType;

	std::set< int > kNextID;
	size_t iSize = 0;
	size_t iSize2 = 0;
	CONT_DEF_ALRAM_MISSION::const_iterator itr = kContDefAlramMission.begin();
	for ( ; itr != kContDefAlramMission.end() ; ++itr )
	{
		if ( itr->second.iType == m_kAlramType )
		{
			if ( 0i64 == itr->second.i64ClassLimit )
			{
				++iSize;
			}
			else
			{
				__int64 i64CheckBit = INT64_1;
				VEC_ALRAM_ID& rTemp = itr->second.nIsDraClass ? tempDra : temp;
				for ( int iClass = 0; iClass < 64; ++iClass )
				{
					if ( itr->second.i64ClassLimit & i64CheckBit )
					{
						++rTemp[iClass];
					}
					i64CheckBit <<= 1;
				}

				++iSize2;
			}

			if ( itr->second.iAlramID_Next )
			{
				size_t iCount = 0;

				CONT_DEF_ALRAM_MISSION::const_iterator next_itr = itr;
				while ( next_itr->second.iAlramID_Next )
				{
					next_itr = kContDefAlramMission.find( next_itr->second.iAlramID_Next );
					if ( next_itr != kContDefAlramMission.end() )
					{
						if ( next_itr->second.iType != m_kAlramType )
						{
							kNextID.insert( next_itr->first );
						}

						if ( ++iCount > 5 )
						{
							VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"AlramMission's NextMissionID Loop much!!!! AlramID<" << itr->first << L">" );
							return E_FAIL;
						}
					}
					else
					{
						VERIFY_INFO_LOG( false, BM::LOG_LV4, __FL__ << L"Not Found AlramMission NextMissionID<" << next_itr->second.iAlramID_Next << L"> AlramMissionID<" << next_itr->second.iAlramID << L">" );
						return E_FAIL;
					}
				}
			}
		}
	}// 용량낭비를 안해보게...

	{
		int iClass = 0;
		VEC_ALRAM_ID::const_iterator temp_itr = temp.begin();
		for ( ; temp_itr != temp.end() ; ++temp_itr )
		{
			if ( *temp_itr )
			{
				auto kPair = m_kContAlramIDByClass.insert( std::make_pair(iClass, VEC_ALRAM_ID()) );
				kPair.first->second.reserve( *temp_itr );
			}
			++iClass;
		}
		
		//용족
		iClass = static_cast<int>(UCLASS_SHAMAN) - 1;
		VEC_ALRAM_ID::const_iterator tempDra_itr = tempDra.begin();
		for ( ; tempDra_itr != tempDra.end() ; ++tempDra_itr )
		{
			if ( *tempDra_itr )
			{
				auto kPair = m_kContAlramIDByClass.insert( std::make_pair(iClass, VEC_ALRAM_ID()) );
				kPair.first->second.reserve( *tempDra_itr );
			}
			++iClass;
		}
	}

	if ( !iSize && m_kContAlramIDByClass.empty() )
	{
		return E_FAIL;
	}

	m_kVecAlramID.reserve( iSize );
	iSize += kNextID.size();
	iSize += iSize2;
	m_kVecAlramMission.resize( iSize );
	iSize = 0;

	itr = kContDefAlramMission.begin();
	for ( ; itr != kContDefAlramMission.end() ; ++itr )
	{
		if ( iSize > m_kContAlramMission.size() )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Critical Error!!!" );
			return E_FAIL;
		}

		TBL_DEF_ALRAM_MISSION * pkElement = NULL;
		if ( itr->second.iType == m_kAlramType )
		{
			if ( 0i64 == itr->second.i64ClassLimit )
			{
				m_kVecAlramID.push_back( itr->first );
			}
			else
			{
				__int64 i64CheckBit = INT64_1;
				for ( int iClass = 0; iClass < 64; ++iClass )
				{
					if ( itr->second.i64ClassLimit & i64CheckBit )
					{
						int add = 0;
						if(itr->second.nIsDraClass)
						{
							add = static_cast<int>(UCLASS_SHAMAN) - 1;//50부터 시작할 거니까
						}
						CONT_ALRAM_ID_BYCLASS::iterator id_itr = m_kContAlramIDByClass.find( iClass + add );
						if ( id_itr != m_kContAlramIDByClass.end() )
						{
							id_itr->second.push_back( itr->first );
						}
					}
					i64CheckBit <<= 1;
				}
			}

			pkElement = &(m_kVecAlramMission.at(iSize));
		}
		else
		{
			if ( kNextID.end() != kNextID.find( itr->first ) )
			{
				pkElement = &(m_kVecAlramMission.at(iSize));
			}
		}

		if ( pkElement )
		{
			*pkElement = itr->second;
			m_kContAlramMission.insert( std::make_pair(itr->first, pkElement) );
			++iSize;
		}
	}

	return S_OK;
}

HRESULT PgAlramMissionMgr::GetNewActionByID( int const iID, PgAlramMission &rkOutMission )const
{
	TBL_DEF_ALRAM_MISSION const *pkDef = GetDef( iID );
	if ( !pkDef )
	{
		return E_FAIL;
	}

	rkOutMission.Begin( pkDef, g_kEventView.GetServerElapsedTime(), false );
	return S_OK;
}

HRESULT PgAlramMissionMgr::GetNewAction( int const iClass, PgAlramMission &rkOutMission )const
{
	if ( PgAlramMission::STATE_PACKET == rkOutMission.GetState() )
	{ 
		// 이전맵에서 하던 Action을 살려준다!!!
		TBL_DEF_ALRAM_MISSION const *pkDef = GetDef( rkOutMission.GetExtValue() );
		if ( pkDef )
		{
			DWORD const dwBeginTime = g_kEventView.GetServerElapsedTime() - rkOutMission.GetBeginTime();
			rkOutMission.Begin( pkDef, dwBeginTime, true );
			return S_OK;
		}
		
		rkOutMission = PgAlramMission();// 초기화
		return E_FAIL;
	}

	CONT_ALRAM_ID_BYCLASS::const_iterator byclass_itr = m_kContAlramIDByClass.find( iClass );
	int const iMaxIndex = ( static_cast<int>(m_kVecAlramID.size()) + ( byclass_itr != m_kContAlramIDByClass.end() ? static_cast<int>(byclass_itr->second.size()) : 0 ) );

	HRESULT hRet = E_FAIL;
	if ( 0 < iMaxIndex )
	{
		size_t iIndex = static_cast<size_t>(BM::Rand_Index( iMaxIndex ));
		if ( iIndex < m_kVecAlramID.size() )
		{
			hRet = GetNewActionByID( m_kVecAlramID.at(iIndex), rkOutMission );
		}
		else
		{
			iIndex -= m_kVecAlramID.size();
			hRet = GetNewActionByID( byclass_itr->second.at(iIndex), rkOutMission );
		}

		if ( FAILED(hRet) )
		{
			VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found AlramID<" << m_kVecAlramID.at(iIndex) << L"> MgrType<" << m_kAlramType << L">" );
		}
		return hRet;
	}

	return hRet;
}

HRESULT PgAlramMissionMgr::GetNextAction( PgAlramMission &rkOutMission )const
{
	TBL_DEF_ALRAM_MISSION const *pkDef = GetDef( rkOutMission.GetNextID() );
	if ( !pkDef )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV0, __FL__ << L"Not Found AlramID<" << rkOutMission.GetNextID() << L"> MgrType<" << m_kAlramType << L">" );
		return E_FAIL;
	}

	rkOutMission.Begin( pkDef, g_kEventView.GetServerElapsedTime() );
	return S_OK;
}

TBL_DEF_ALRAM_MISSION const * PgAlramMissionMgr::GetDef( int const iID )const
{
	CONT_ALRAM_MISSION::const_iterator itr = m_kContAlramMission.find( iID );
	if ( itr != m_kContAlramMission.end() )
	{
		return itr->second;
	}
	return NULL;
}

PgAlramMissionMgr_Warpper::PgAlramMissionMgr_Warpper()
{}

PgAlramMissionMgr_Warpper::~PgAlramMissionMgr_Warpper()
{}

HRESULT PgAlramMissionMgr_Warpper::Init( ALRAM_MISSION::E_ALRAMTYPE const kType, CONT_DEF_ALRAM_MISSION const &kContDefAlramMission )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->Init( kType, kContDefAlramMission );
}

HRESULT PgAlramMissionMgr_Warpper::GetNewActionByID( int const iID, PgAlramMission &rkOutMission )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetNewActionByID( iID, rkOutMission );
}

HRESULT PgAlramMissionMgr_Warpper::GetNewAction( int const iClass, PgAlramMission &rkOutMission )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetNewAction( iClass, rkOutMission );
}

HRESULT PgAlramMissionMgr_Warpper::GetNextAction( PgAlramMission &rkOutMission )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->GetNextAction( rkOutMission );
}
