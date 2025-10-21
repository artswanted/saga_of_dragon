
template< typename T >
bool PgXFStream_FMM::Push( T const & rhs )
{
	return this->Push( &rhs, sizeof(T) );
}

template<>
bool PgXFStream_FMM::Push( std::string const &str )
{
	return this->Push( reinterpret_cast<void const*>(str.c_str()), ( sizeof(char) * (str.length() + 1) ) );
}

template<>
bool PgXFStream_FMM::Push( std::wstring const &wstr )
{
	return this->Push( reinterpret_cast<void const*>(wstr.c_str()), ( sizeof(wchar_t) * (wstr.length() + 1) ) );
}

template<>
bool PgXFStream_FMM::Push( vstring const &vstr )
{
	return this->Push( vstr.operator std::wstring const&() );
}

template< typename T >
bool PgXFStream_FMM::Pop( T &rhs )
{
	return this->Pop( &rhs, sizeof(T) );
}
