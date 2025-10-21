#include "stdafx.h"
#include "LimitClass.h"

PgLimitClass::PgLimitClass()
:	m_i64ClassLimit(0i64)
,	m_nLevelLimit(0)
{
}

PgLimitClass::PgLimitClass( int const iClass, short const nLevelLimit )
:	m_i64ClassLimit(GET_CLASS_LIMIT(iClass))
,	m_nLevelLimit(nLevelLimit)
{
}

PgLimitClass::PgLimitClass( __int64 const i64ClassLimit, short const nLevelLimit )
:	m_i64ClassLimit(i64ClassLimit)
,	m_nLevelLimit(nLevelLimit)
{
}

PgLimitClass::~PgLimitClass()
{
}

PgLimitClass PgLimitClass::operator + ( PgLimitClass const &rhs )const
{
	PgLimitClass temp( *this );
	temp += rhs;
	return temp;
}

PgLimitClass PgLimitClass::operator - ( PgLimitClass const &rhs )const
{
	PgLimitClass temp( *this );
	temp -= rhs;
	return temp;
}

PgLimitClass& PgLimitClass::operator +=( PgLimitClass const &rhs )
{
	m_i64ClassLimit |= rhs.m_i64ClassLimit;

	if ( 0==m_nLevelLimit || m_nLevelLimit > rhs.m_nLevelLimit )
	{
		m_nLevelLimit = rhs.m_nLevelLimit;
	}
	return *this;
}

PgLimitClass& PgLimitClass::operator -=( PgLimitClass const &rhs )
{
	m_i64ClassLimit &= (~(rhs.m_i64ClassLimit));
	return *this;
}

bool PgLimitClass::IsAccess( PgLimitClass const &kLimit )const
{
	return	IsAccessClassLimit( kLimit.m_i64ClassLimit )
		&&	IsAccessLevel( kLimit.m_nLevelLimit );
}