
template< typename T >
HRESULT PgVariableMgr_Wrapper::Set( T_VARIANT_KEY const &kGroupKey, T_VARIANT_KEY const &kKey, T const &kElement )
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, true );
	return Instance()->Set<T>( kGroupKey, kKey, kElement );
}

template< typename T >
HRESULT PgVariableMgr_Wrapper::Get( T_VARIANT_KEY const &kGroupKey, T_VARIANT_KEY const &kKey, T &rkOut )const
{
	BM::CAutoMutex kLock( m_kMutex_Wrapper_, false );
	return Instance()->Get<T>( kGroupKey, kKey, rkOut );
}
