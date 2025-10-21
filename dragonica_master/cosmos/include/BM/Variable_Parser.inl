
template< typename T_GROUPKEY, typename T_KEY >
HRESULT PgVariable_ParserINI< T_GROUPKEY, T_KEY >::Load( std::wstring const &kPath )
{
	std::fstream kFile;

	try
	{
		kFile.open( MB(kPath), std::ios_base::in | std::ios_base::binary );
		if ( !kFile.is_open() )
		{
			throw static_cast<HRESULT>(E_FAIL);
		}

		BM::vstring vstrGroupName;
		std::string strTemp;
		bool bComments = false;
		std::string::size_type find_pos = 0;

		while ( std::getline( kFile, strTemp ) )
		{
			boost::algorithm::trim( strTemp );

			if ( strTemp.size() )
			{
				if ( true == bComments )
				{
					find_pos = strTemp.find( "*/", 0 );
					if ( std::string::npos == find_pos )
					{
						continue;
					}

					strTemp.erase( 0, find_pos + 2 );
					bComments = false;
				}

				if ( '[' == strTemp.at(0) )
				{
					find_pos = strTemp.find( ']', 1 );
					if ( std::string::npos != find_pos )
					{
						vstrGroupName = BM::vstring( std::string( strTemp, 1, find_pos-1 ) );
						continue;
					}
				}

				{
					find_pos = strTemp.find( "/*", 0 );
					if ( std::string::npos != find_pos )
					{// 주석의 시작
						bComments = true;

						std::string::size_type find_pos2 = find_pos + 2;
						if ( find_pos2 < strTemp.size() )
						{
							find_pos2 = strTemp.find( "*/", find_pos2 );
							if ( std::string::npos != find_pos2 )
							{
								strTemp.erase( find_pos, find_pos2 + 2 );
								bComments = false;
							}
							else
							{
								continue;
							}
						}
						else
						{
							continue;
						}
					}

					find_pos = strTemp.find( "//", 0 );
					if ( std::string::npos != find_pos )
					{// 주석이다.
						strTemp.erase( find_pos );
					}

					find_pos = strTemp.find( ";", 0 );
					if ( std::string::npos != find_pos )
					{// 주석이다.
						strTemp.erase( find_pos );
					}

					find_pos = strTemp.find( '=', 0 );
					if ( std::string::npos != find_pos )
					{
						std::string strKeyName = std::string( strTemp, 0, find_pos - 1 );
						boost::algorithm::trim( strKeyName );

						std::string kVariantValue = std::string( strTemp, find_pos+1, strTemp.length() );
						boost::algorithm::trim( kVariantValue );

						if ( strKeyName.size() && kVariantValue.size() )
						{
							this->Set( static_cast<T_GROUPKEY>(vstrGroupName), static_cast<T_KEY>( BM::vstring( strKeyName ) ), kVariantValue );
						}
					}
				}
			}
		}

		kFile.close();
	}
	catch ( HRESULT hRet )
	{
		return hRet;
	}

	return S_OK;
}
