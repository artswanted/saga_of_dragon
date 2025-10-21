#include "stdafx.h"
#include "GameTime.h"
#include "PgPlayLimiter.h"

PgPlayLimit_Finder::PgPlayLimit_Finder( CONT_DEF_PLAYLIMIT_INFO_LIST const &kLimitInfoList )
:	m_rkLimitInfoList(kLimitInfoList)
{
}

PgPlayLimit_Finder::~PgPlayLimit_Finder()
{
}

HRESULT PgPlayLimit_Finder::Find( __int64 const i64Time, CONT_DEF_PLAYLIMIT_INFO_LIST::value_type &rkOut )const
{
	if ( 0 < m_rkLimitInfoList.size() )
	{
		return Find( i64Time, 0, m_rkLimitInfoList.size(), rkOut );
	}
	return E_FAIL;
}

HRESULT PgPlayLimit_Finder::Find( __int64 const i64Time, size_t left_index, size_t right_index, CONT_DEF_PLAYLIMIT_INFO_LIST::value_type &rkOut )const
{
	size_t const calc_index = right_index - left_index;
	size_t find_index = calc_index / 2;
	find_index += left_index;

	CONT_DEF_PLAYLIMIT_INFO_LIST::value_type const &rkElement = m_rkLimitInfoList.at(find_index);
	if ( true == CGameTime::IntoInDayTime( rkElement.i64BeginTime, rkElement.i64EndTime, i64Time ) )
	{
		rkOut = rkElement;
		return S_OK;
	}

	if ( 2 > calc_index )
	{
		if ( rkElement.i64BeginTime > i64Time )
		{
			rkOut = rkElement;
		}
		else
		{
			if ( ++find_index >= m_rkLimitInfoList.size() )
			{
				rkOut = m_rkLimitInfoList.at(0);
			}
			else
			{
				rkOut = m_rkLimitInfoList.at(find_index);
			}
		}

		return S_FALSE;
	}

	if ( rkElement.i64BeginTime > i64Time )
	{
		return Find( i64Time, left_index, find_index, rkOut );
	}
	return Find( i64Time, find_index, right_index, rkOut );
}
