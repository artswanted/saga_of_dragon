

template< typename T_KEY >
template< typename T >
HRESULT PgVariables< T_KEY >::Set( T_KEY const &kKey, T const &kElement )
{
	value_type vstr( kElement );
	return Set( kKey, vstr );
}

template< typename T_KEY >
template< typename T >
HRESULT PgVariables< T_KEY >::Get( T_KEY const &kKey, T &rkOut )const
{
	value_type vstr;
	HRESULT const hRet = Get( kKey, vstr );
	if ( SUCCEEDED( hRet ) )
	{
		rkOut = static_cast<T>(vstr);
	}
	return hRet;
}

template< typename T_KEY >
void PgVariables< T_KEY >::Swap( PgVariables< T_KEY > &rhs )
{
	m_kContVariable.swap( rhs.m_kContVariable ); 
}

template< typename T_KEY >
void PgVariables< T_KEY >::Add( PgVariables< T_KEY > const &rhs )
{
	CONT_VARIABLES::const_iterator itr = rhs.m_kContVariable.begin();
	for ( ; itr != rhs.m_kContVariable.end() ; ++itr )
	{
		auto kPair = m_kContVariable.insert( std::make_pair( itr->first, itr->second ) );
		if ( !kPair.second )
		{
			kPair.first->second = itr->second;
		}
	}
}

template< typename T_KEY >
void PgVariables< T_KEY >::WriteToPacket( Stream &kStream )const
{
	_Traits_MarshallingPacket< CONT_VARIABLE >::WriteToPacket( kStream, m_kContVariable );
}

template< typename T_KEY >
bool PgVariables< T_KEY >::ReadFromPacket( Stream &kStream )
{
	m_kContVariable.clear();
	return _Traits_MarshallingPacket< CONT_VARIABLE >::ReadFromPacket( kStream, m_kContVariable );
}


//
template< typename T_GROUPKEY, typename T_KEY >
PgVariableMgr< T_GROUPKEY, T_KEY >::PgVariableMgr( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs )
:	m_kContVariables(rhs.m_kContVariables)
{
}

template< typename T_GROUPKEY, typename T_KEY >
PgVariableMgr< T_GROUPKEY, T_KEY >& PgVariableMgr< T_GROUPKEY, T_KEY >::operator=( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs )
{
	m_kContVariables = rhs.m_kContVariables;
	return *this;
}

template< typename T_GROUPKEY, typename T_KEY >
template< typename T >
HRESULT PgVariableMgr< T_GROUPKEY, T_KEY >::Set( T_GROUPKEY const &kGroupKey, T_KEY const &kKey, T const &kElement )
{
	CONT_VARIABLES::iterator cont_itr = m_kContVariables.find( kGroupKey );
	if ( cont_itr == m_kContVariables.end() )
	{
		auto kPair = m_kContVariables.insert( std::make_pair( kGroupKey, PgVariables<T_KEY>() ) );
		cont_itr = kPair.first;
	}

	if ( cont_itr != m_kContVariables.end() )
	{
		return cont_itr->second.Set<T>( kKey, kElement );
	}
	return E_FAIL;
}

template< typename T_GROUPKEY, typename T_KEY >
template< typename T >
HRESULT PgVariableMgr< T_GROUPKEY, T_KEY >::Get( T_GROUPKEY const &kGroupKey, T_KEY const &kKey, T &rkOut )const
{
	CONT_VARIABLES::const_iterator cont_itr = m_kContVariables.find( kGroupKey );
	if ( cont_itr != m_kContVariables.end() )
	{
		return cont_itr->second.Get<T>( kKey, rkOut );
	}
	return E_FAIL;
}

template< typename T_GROUPKEY, typename T_KEY >
void PgVariableMgr< T_GROUPKEY, T_KEY >::Swap( PgVariableMgr< T_GROUPKEY, T_KEY > &rhs )
{
	m_kContVariables.swap( rhs.m_kContVariables );
}

template< typename T_GROUPKEY, typename T_KEY >
void PgVariableMgr< T_GROUPKEY, T_KEY >::Add( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs )
{
	CONT_VARIABLES::const_iterator itr = rhs.m_kContVariables.begin();
	for ( ; itr != rhs.m_kContVariables.end() ; ++itr )
	{
		auto kPair = m_kContVariables.insert( std::make_pair( itr->first, itr->second ) );
		if ( !kPair.second )
		{
			kPair.first->second.Add( itr->second );
		}
	}
}

template< typename T_GROUPKEY, typename T_KEY >
HRESULT PgVariableMgr< T_GROUPKEY, T_KEY >::Add( PgVariableMgr< T_GROUPKEY, T_KEY > const &rhs, T_GROUPKEY const &kGroupKey )
{
	CONT_VARIABLES::const_iterator itr = rhs.m_kContVariables.find( kGroupKey );
	if ( itr != rhs.m_kContVariables.end() )
	{
		auto kPair = m_kContVariables.insert( std::make_pair( itr->first, itr->second ) );
		if ( !kPair.second )
		{
			kPair.first->second.Add( itr->second );
		}
		return S_OK;
	}
	return E_FAIL;
}


template< typename T_GROUPKEY, typename T_KEY >
void PgVariableMgr< T_GROUPKEY, T_KEY >::WriteToPacket( Stream &kStream )const
{
	_Traits_MarshallingPacket< CONT_VARIABLES >::WriteToPacket( kStream, m_kContVariables );
}

template< typename T_GROUPKEY, typename T_KEY >
bool PgVariableMgr< T_GROUPKEY, T_KEY >::ReadFromPacket( Stream &kStream )
{
	m_kContVariables.clear();
	return _Traits_MarshallingPacket< CONT_VARIABLES >::ReadFromPacket( kStream, m_kContVariables );
}
