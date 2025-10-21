#include "stdafx.h"
#include "PgSmallAreaInfo.h"

PgSmallAreaInfo::PgSmallAreaInfo()
{

}

PgSmallAreaInfo::~PgSmallAreaInfo()
{

}

HRESULT PgSmallAreaInfo::Init( int const iMapNo, CONT_DEFMAP const &rkContDefMap, POINT3 const &pt3Min, POINT3 const &pt3Max )
{
	if( pt3Max < pt3Min )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV3, __FL__ << _T("'pt3Max < pt3Min' error") );
		LIVE_CHECK_LOG( BM::LOG_LV1, __FL__ << _T("Return E_FAIL") );
		return E_FAIL;
	}

	CONT_DEFMAP::const_iterator itr = rkContDefMap.find(iMapNo);
	if ( itr == rkContDefMap.end() )
	{
		INFO_LOG(BM::LOG_LV0, __FL__ << _T("Cannot find DefMap MapNo<") << iMapNo << _T(">") );
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return E_FAIL"));
		return E_FAIL;
	}

	TBL_DEF_MAP const &rkDefMap = itr->second;

	m_pt3Min.Set( static_cast<int>( pt3Min.x ), static_cast<int>( pt3Min.y ), static_cast<int>( pt3Min.z ) );
	m_pt3Max.Set( static_cast<int>( pt3Max.x + 1.0f), static_cast<int>( pt3Max.y + 1.0f), static_cast<int>( pt3Max.z + 1.0f) );
	m_pt3AreaSize.Set( rkDefMap.sZoneCX, rkDefMap.sZoneCY, rkDefMap.sZoneCZ );

	POINT3 ptTemp = ( pt3Max - pt3Min );
	m_pt3Count.x = std::max(1, static_cast<int>( ceil( ptTemp.x / static_cast<float>(m_pt3AreaSize.x) ) ));
	m_pt3Count.y = std::max(1, static_cast<int>( ceil( ptTemp.y / static_cast<float>(m_pt3AreaSize.y) ) ));
	m_pt3Count.z = std::max(1, static_cast<int>( ceil( ptTemp.z / static_cast<float>(m_pt3AreaSize.z) ) ));

	if ( GetTotalSize() <= 0 )
	{
		VERIFY_INFO_LOG( false, BM::LOG_LV2, __FL__ << _T("Size Error!!! MapNo<") << iMapNo << _T(">") );
		return E_FAIL;
	}
	if ( GetTotalSize() > 50000 )
	{
		// Too many smallarea
		CAUTION_LOG(BM::LOG_LV5, __FL__ << __T("Too many SmallArea GroundNo=") << iMapNo << _T(", Count=") << GetTotalSize() << _T(", MinPos=") << pt3Min << _T(", MaxPos=") << pt3Max);
	}
	return S_OK;
}

void PgSmallAreaInfo::WriteToPacket( BM::Stream &rkPacket )const
{
	rkPacket.Push( m_pt3Min );
	rkPacket.Push( m_pt3Max );
	rkPacket.Push( m_pt3AreaSize );
	rkPacket.Push( m_pt3Count );
}

void PgSmallAreaInfo::ReadFromPacket( BM::Stream &rkPacket )
{
	rkPacket.Pop( m_pt3Min );
	rkPacket.Pop( m_pt3Max );
	rkPacket.Pop( m_pt3AreaSize );
	rkPacket.Pop( m_pt3Count );
}

bool PgSmallAreaInfo::GetAreaIndex3( int const iIndex, POINT3I &rkOutIndex )const
{
	if ( ( ERROR_INDEX < iIndex ) && ( iIndex < GetTotalSize() ) )
	{
		int const iYZ = m_pt3Count.y * m_pt3Count.z;
		rkOutIndex.x = iIndex / iYZ;
		rkOutIndex.y = (iIndex % iYZ) / m_pt3Count.z;
		rkOutIndex.z = iIndex % m_pt3Count.z;
		return true;
	}
	return false;
}

bool PgSmallAreaInfo::GetAreaMinPos( int const iIndex, POINT3 &rkOutPos )const
{
	POINT3I pt3Index;
	if ( true == GetAreaIndex3( iIndex, pt3Index ) )
	{
		pt3Index *= m_pt3AreaSize;
		rkOutPos.x = m_pt3Min.x + static_cast<float>(pt3Index.x);
		rkOutPos.y = m_pt3Min.y + static_cast<float>(pt3Index.y);
		rkOutPos.z = m_pt3Min.z + static_cast<float>(pt3Index.z);
		return true;
	}
	return false;
}

bool PgSmallAreaInfo::GetAreaMaxPos( int const iIndex, POINT3 &rkOutPos )const
{
	if ( true == GetAreaMinPos( iIndex, rkOutPos ) )
	{
		rkOutPos.x += static_cast<float>(m_pt3AreaSize.x);
		rkOutPos.y += static_cast<float>(m_pt3AreaSize.y);
		rkOutPos.z += static_cast<float>(m_pt3AreaSize.z);
		return true;
	}
	return false;
}

int PgSmallAreaInfo::GetRelativeIndexFromIndex( int const iIndex, ESmallArea_Direction const eDir )const
{
	POINT3I pt3Index;
	if ( true == GetAreaIndex3( iIndex, pt3Index ) )
	{
		return GetRelativeIndexFromIndex3( pt3Index, eDir );
	}
	return ERROR_INDEX;
}

int PgSmallAreaInfo::GetRelativeIndexFromIndex3( POINT3I const &pt3Index, ESmallArea_Direction const eDir )const
{
	POINT3I pt3RetIndex;
	GetRelativeIndex3FromIndex3( pt3Index, eDir, pt3RetIndex );
	return GetAreaIndexFromIndex3( pt3RetIndex );
}
