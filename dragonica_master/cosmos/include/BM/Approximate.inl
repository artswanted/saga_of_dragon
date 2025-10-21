
template< typename T, class _Pr >
size_t PgTraitsApproximateFinder< T, _Pr >::operator()( std::vector< T > const &kVec, T const &kValue )
{
	if ( 0 < kVec.size() )
	{
		return find( kVec, kValue, 0, kVec.size() - 1 );
	}

	return PgApproximate::npos;
}

template< typename T, class _Pr >
size_t PgTraitsApproximateFinder< T, _Pr >::find( std::vector< T > const &kVec, T const &kValue, size_t const left_index, size_t const right_index )
{
	size_t find_index = (right_index - left_index);
	switch ( find_index )
	{
	case 0:
		{
			if ( !_Pr()( kVec.at( left_index ), kValue) )
			{
				return PgApproximate::npos;
			}

			return left_index;
		}break;
	case 1:
		{
			if ( !_Pr()( kVec.at( right_index ), kValue ) )
			{
				if ( !_Pr()( kVec.at( left_index ), kValue) )
				{
					return PgApproximate::npos;
				}
				return left_index;
			}
			return right_index;
		}break;
	default:
		{
		}break;
	}

	find_index /= 2;
	find_index += left_index;

	if ( !_Pr()( kVec.at(find_index), kValue ) )
	{
		if ( !_Pr()( kValue, kVec.at(find_index) ) )
		{
			return find_index;
		}

		return find( kVec, kValue, left_index, find_index );
	}

	return find( kVec, kValue, find_index, right_index );
}

template< typename T, class _FindPr >
size_t PgTraitsApproximateFinderEx< T, _FindPr >::operator()( std::vector< T > const &kVec, _FindPr const &kValue )
{
	if ( 0 < kVec.size() )
	{
		return find( kVec, kValue, 0, kVec.size() - 1 );
	}

	return PgApproximate::npos;
}

template< typename T, class _FindPr >
size_t PgTraitsApproximateFinderEx< T, _FindPr >::find( std::vector< T > const &kVec, _FindPr const &kValue, size_t const left_index, size_t const right_index )
{
	size_t find_index = (right_index - left_index);
	switch ( find_index )
	{
	case 0:
		{
			if ( kValue < kVec.at( left_index ) )
			{
				return PgApproximate::npos;
			}

			return left_index;
		}break;
	case 1:
		{
			if ( kValue < kVec.at( right_index ) )
			{
				if ( kValue < kVec.at( left_index ) )
				{
					return PgApproximate::npos;
				}
				return left_index;
			}
			return right_index;
		}break;
	default:
		{
		}break;
	}

	find_index /= 2;
	find_index += left_index;

	if ( !(kValue > kVec.at(find_index)) )
	{
		if ( !(kValue < kVec.at(find_index)) )
		{
			return find_index;
		}

		return find( kVec, kValue, left_index, find_index );
	}

	return find( kVec, kValue, find_index, right_index );
}

template< typename T, class _Pr >
PgApproximateVector< T, _Pr >::PgApproximateVector(void)
{
}

template< typename T, class _Pr >
PgApproximateVector< T, _Pr >::~PgApproximateVector(void)
{
}

template< typename T, class _Pr >
PgApproximateVector< T, _Pr >::PgApproximateVector( PgApproximateVector< T, _Pr > const &rhs )
:	m_kContVar(rhs.m_kContVar)
{
}

template< typename T, class _Pr >
PgApproximateVector< T, _Pr >& PgApproximateVector< T, _Pr >::operator = ( PgApproximateVector< T, _Pr > const &rhs )
{
	m_kContVar = rhs.m_kContVar;
	return *this;
}

template< typename T, class _Pr >
void PgApproximateVector< T, _Pr >::reserve( size_t const size )
{
	m_kContVar.reserve( size );
}

template< typename T, class _Pr >
bool PgApproximateVector< T, _Pr >::insert( T const & kValue )
{
	m_kContVar.push_back( kValue );
	std::sort( m_kContVar.begin(), m_kContVar.end(), _Pr() );
	return true;
}

template< typename T, class _Pr >
bool PgApproximateVector< T, _Pr >::insert( CONT_VAR const &kCont )
{
	size_t const iSize = m_kContVar.size() + kCont.size();
	reserve( iSize );
	
	CONT_VAR::const_iterator itr = kCont.begin();
	for ( ; itr != kCont.end() ; ++itr )
	{
		m_kContVar.push_back( *itr );
	}

	std::sort( m_kContVar.begin(), m_kContVar.end(), _Pr() );
	return true;
}

template< typename T, class _Pr >
size_t PgApproximateVector< T, _Pr >::find( T const & kValue, T & rkOutValue )const
{
	size_t const find_index = PgTraitsApproximateFinder< T, _Pr >()( m_kContVar, kValue );
	if ( find_index < m_kContVar.size() )
	{
		rkOutValue = m_kContVar.at(find_index);
	}

	return find_index;
}

template< typename T, class _Pr >
template< class _FindPr >
size_t PgApproximateVector< T, _Pr >::find( _FindPr const & kValue )const
{
	size_t const find_index = PgTraitsApproximateFinderEx< T, _FindPr >()( m_kContVar, kValue );
	if ( find_index < m_kContVar.size() )
	{
		return find_index;
	}

	return PgApproximate::npos;
}

template< typename T, class _Pr >
T const &PgApproximateVector< T, _Pr >::at( size_t const index )const
{
	return m_kContVar.at( index );
}
