#pragma once

namespace BM
{
	//======================================================
	// Trait_ReplaceStr
	//======================================================
	template< typename T_STRING, typename T_REPLACE > struct _Trait_ReplaceStr;

	template< > 
	struct _Trait_ReplaceStr< std::wstring, std::wstring >
	{
		static void Replace( std::wstring &kOrgStr, std::wstring const &kFindStr, std::wstring const &kReplaceStr )
		{
			std::wstring::size_type offset = kOrgStr.find( kFindStr, 0 );
			while ( std::wstring::npos != offset )
			{
				kOrgStr.replace( offset, kFindStr.length(), kReplaceStr );
				offset = kOrgStr.find( kFindStr, offset );
			}
		}
	};

	template< > 
	struct _Trait_ReplaceStr< std::wstring, std::string >
	{
		static void Replace( std::wstring &kOrgStr, std::wstring const &kFindStr, std::string const &kReplaceStr )
		{
			_Trait_ReplaceStr< std::wstring, std::wstring >::Replace( kOrgStr, kFindStr, UNI(kReplaceStr) );
		}
	};

	template< > 
	struct _Trait_ReplaceStr< std::wstring, wchar_t* >
	{
		static void Replace( std::wstring &kOrgStr, std::wstring const &kFindStr, wchar_t * const pkReplaceStr )
		{
			_Trait_ReplaceStr< std::wstring, std::wstring >::Replace( kOrgStr, kFindStr, std::wstring(pkReplaceStr) );
		}
	};

	template< > 
	struct _Trait_ReplaceStr< std::wstring, char* >
	{
		static void Replace( std::wstring &kOrgStr, std::wstring const &kFindStr, char * const pkReplaceStr )
		{
			_Trait_ReplaceStr< std::wstring, std::wstring >::Replace( kOrgStr, kFindStr, UNI(pkReplaceStr) );
		}
	};

	template< > 
	struct _Trait_ReplaceStr< std::wstring, vstring >
	{
		static void Replace( std::wstring &kOrgStr, std::wstring const &kFindStr, vstring const &kReplaceStr )
		{
			_Trait_ReplaceStr< std::wstring, std::wstring >::Replace( kOrgStr, kFindStr, static_cast<std::wstring>(kReplaceStr) );
		}
	};

	//<-- Trait_ReplaceStr

	//======================================================
	// _Trait_CreateFormatStr
	//======================================================
	template< typename T > struct _Trait_CheckFormatStr;

	template< > 
	struct _Trait_CheckFormatStr< int >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			if ( pwszFormat )
			{
				if ( L'%' == *pwszFormat )
				{
					++pwszFormat;

					size_t iLength = 0;
					while ( (iLength < 10 )
						&&	(L'\0' != *pwszFormat)
						)
					{
						if ( !::iswdigit( *pwszFormat ) )
						{
							switch( *pwszFormat )
							{
							case L'd':case L'u':
							case L'c':
							case L'x':case L'X':
							case L'o':
								{
									++pwszFormat;
									return (L'\0' == *pwszFormat);
								}break;
							default:
								{
									return false;
								}break;
							}
						}

						++iLength;
						++pwszFormat;
					}
				}
			}
			return false;
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< __int64 >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			if ( pwszFormat )
			{
				if ( L'%' == *pwszFormat )
				{
					++pwszFormat;

					int iChk64Step = 0;

					size_t iLength = 0;
					while ( (iLength < 10 )
						&&	(L'\0' != *pwszFormat)
						)
					{
						switch( iChk64Step )
						{
						case 1:
							{
								if ( L'6' != *pwszFormat )
								{
									return false;
								}

								++iChk64Step;
							}break;
						case 2:
							{
								if ( L'4' != *pwszFormat )
								{
									return false;
								}
								
								++iChk64Step;
							}break;
						default:
							{
								if ( !::iswdigit( *pwszFormat ) )
								{
									switch( *pwszFormat )
									{
									case L'd':case L'u':
									case L'c':
									case L'x':case L'X':
									case L'o':
										{
											++pwszFormat;
											return (L'\0' == *pwszFormat);
										}break;
									case L'I':
										{
											if ( iChk64Step++ )
											{
												return false;
											}
										}break;
									default:
										{
											return false;
										}break;
									}
								}
							}break;
						}

						++iLength;
						++pwszFormat;
					}
				}
			}
			return false;
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< double >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			if ( pwszFormat )
			{
				if ( L'%' == *pwszFormat )
				{
					++pwszFormat;

					bool bFindPoint = false;

					size_t iLength = 0;
					while ( (iLength < 10 )
						&&	(L'\0' != *pwszFormat)
						)
					{
						if ( !::iswdigit( *pwszFormat ) )
						{
							switch( *pwszFormat )
							{
							case L'f':
								{
									++pwszFormat;
									return (L'\0' == *pwszFormat);
								}break;
							case L'.':
								{
									if ( true == bFindPoint )
									{
										return false;
									}
									
									bFindPoint = true;
								}break;
							default:
								{
									return false;
								}break;
							}
						}

						++iLength;
						++pwszFormat;
					}
				}
			}
			return false;
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< unsigned int >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< char >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< unsigned char >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<char>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< short >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< unsigned short >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<unsigned int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< long >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< unsigned long >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<unsigned int>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< unsigned __int64 >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<__int64>()(pwszFormat);
		}
	};

	template< > 
	struct _Trait_CheckFormatStr< float >
	{
		bool operator()( wchar_t const *pwszFormat )
		{
			return _Trait_CheckFormatStr<double>()(pwszFormat);
		}
	};

};//<-- namespace BM
