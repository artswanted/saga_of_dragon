
template< typename T_ID, typename T_OBJ >
PgObjMgr< T_ID, T_OBJ >::PgObjMgr(void)
{
}

template< typename T_ID, typename T_OBJ >
PgObjMgr< T_ID, T_OBJ >::~PgObjMgr(void)
{
	CONT_OBJ::iterator obj_itr = m_kContObj.begin();
	for ( ; obj_itr != m_kContObj.end() ; ++obj_itr )
	{
		SAFE_DELETE( obj_itr->second );
	}
	m_kContObj.clear();
}

template< typename T_ID, typename T_OBJ >
template< typename T >
bool PgObjMgr< T_ID, T_OBJ >::New( T_ID const &kID, T ** pOutObj )
{
	T * p = new_tr T( kID );
	if ( p )
	{
		T_OBJ *pkObj = dynamic_cast<T_OBJ*>(p);
		if ( pkObj )
		{
			auto kPair = m_kContObj.insert( std::make_pair( kID, pkObj ) );
			if ( true == kPair.second )
			{
				if ( pOutObj )
				{
					*pOutObj = p;
				}
				return true;
			}
		}
	}

	SAFE_DELETE(p);
	return false;
}

template< typename T_ID, typename T_OBJ >
bool PgObjMgr< T_ID, T_OBJ >::Delete( T_ID const &kID )
{
	CONT_OBJ::iterator obj_itr = m_kContObj.find( kID );
	if ( obj_itr != m_kContObj.end() )
	{
		SAFE_DELETE( obj_itr->second );
		m_kContObj.erase( obj_itr );
		return true;
	}
	return false;
}

template< typename T_ID, typename T_OBJ >
T_OBJ* PgObjMgr< T_ID, T_OBJ >::Get( T_ID const &kID )const
{
	CONT_OBJ::const_iterator obj_itr = m_kContObj.find( kID );
	if ( obj_itr != m_kContObj.end() )
	{
		return obj_itr->second;
	}
	return NULL;
}

template< typename T_ID, typename T_OBJ >
template< typename FN >
void PgObjMgr< T_ID, T_OBJ >::ForEach( FN &kFunction )
{
	CONT_OBJ::iterator obj_itr = m_kContObj.begin();
	for ( ; obj_itr != m_kContObj.end() ; ++obj_itr )
	{
		kFunction( obj_itr->second );
	}
}
