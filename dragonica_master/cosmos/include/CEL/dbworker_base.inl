	
	template < typename T_ELEMENT >
	bool tagDBData::Pop(T_ELEMENT &rElement)const
	{
		size_t const element_size = sizeof(T_ELEMENT);
		return PopMemory( &rElement, element_size );
	}

	template <>
	bool tagDBData::Pop( std::vector< BYTE > &vecout )const
	{
		vecout.clear();
		size_t const arr_size = m_vecData.size();
		if( arr_size )
		{
			vecout.resize( arr_size );
			return PopMemory( &vecout.at(0), arr_size * sizeof( std::vector< char >::value_type ) );
		}
		return true;
	}
	
	template <>
	bool tagDBData::Pop( std::vector< char > &vecout )const
	{
		vecout.clear();
		size_t const arr_size = m_vecData.size();
		if( arr_size )
		{
			vecout.resize( arr_size );
			return PopMemory( &vecout.at(0), arr_size * sizeof( std::vector< char >::value_type ) );
		}
		return true;
	}

	template< >
	void tagDBQuery::PushStrParam( std::wstring const&  input)
	{
		std::wstring strRet;
		StrConvForQuery(input, strRet);

		std::wstring kStr = _T("N'");
		kStr += strRet;
		kStr += _T("'");

		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( bool const&  input)
	{
		PushStrParam(static_cast<int>(input ? 1 : 0));
	}

	template< >
	void tagDBQuery::PushStrParam( char const&  input)
	{
		PushStrParam(static_cast<int>(input));
	}

	template< >
	void tagDBQuery::PushStrParam( unsigned char const&  input)
	{
		PushStrParam(static_cast<unsigned int>(input));
	}

	template< >
	void tagDBQuery::PushStrParam( short const&  input)
	{
		PushStrParam(static_cast<int>(input));
	}

	template< >
	void tagDBQuery::PushStrParam( unsigned short const& input)
	{
		PushStrParam(static_cast<unsigned int>(input));
	}

	template< >
	void tagDBQuery::PushStrParam( int const&  input)
	{
		std::wstring kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( unsigned int const&  input)
	{
		std::wstring kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( unsigned long const&  input)
	{
		PushStrParam(static_cast<unsigned int>(input));
	}

	template< >
	void tagDBQuery::PushStrParam( __int64 const&  input)
	{
		std::wstring const kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( unsigned __int64 const&  input)
	{
		std::wstring const kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( double const&  input)
	{
		std::wstring const kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( float const&  input)
	{
		std::wstring const kStr = BM::vstring(input);
		PushStrParam_sub(kStr);
	}

	template< >
	void tagDBQuery::PushStrParam( BM::DBTIMESTAMP_EX const&  input)
	{
		if(input.IsNull())
		{//NULL 이면 따옴표를 안붙임.
			std::wstring kStr = static_cast<std::wstring const&>(BM::vstring(input));
			PushStrParam_sub(kStr);
		}
		else
		{
			std::wstring kStr = _T("'");
			kStr += static_cast<std::wstring const&>(BM::vstring(input));
			kStr += _T("'");
			PushStrParam_sub(kStr);
		}
	}

	template< >
	void tagDBQuery::PushStrParam( BM::GUID const&  input)
	{//GUID 는 NULL을 넣어본적이 없어서 널 쿼리를 안만든다.
		std::wstring kStr = _T("'");
		kStr += input.str();
		kStr += _T("'");
		PushStrParam_sub(kStr);
	}

	template< typename T_VALUE >
	void tagDBQuery::PushStrParam( std::vector< T_VALUE > const& input )
	{
		size_t const in_size = input.size();
		if( in_size )
		{
			BYTE *pData = (BYTE*)(&(input.at(0)));
			PushStrParam( pData, in_size * sizeof(T_VALUE) );
		}
	}

