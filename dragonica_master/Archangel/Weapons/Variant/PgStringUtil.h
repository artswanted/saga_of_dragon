#ifndef WEAPON_VARIANT_UTIL_PGSTRINGUTIL_H
#define WEAPON_VARIANT_UTIL_PGSTRINGUTIL_H

typedef std::vector< std::wstring > VEC_WSTRING;
typedef std::vector< std::string > VEC_STRING;

namespace PgStringUtil
{
	int const SafeAtoi(char const* szText);
	int const SafeAtoi(std::string const &rkText);
	int const SafeAtoi(std::wstring const &rkText);
	bool const SafeAtob(char const* szText, bool bDefault = false);
	bool const SafeAtob(std::string const& rkText, bool bDefault = false);
	float const SafeAtof(char const* szText);
	float const SafeAtof(std::string const& rkText);
	std::wstring const SafeUni(char const* szText);
	int SafeStrcmp(char const* str1, char const* str2);
	void BreakSep(std::wstring const &rkText, VEC_WSTRING &rkOutVec, TCHAR const* szSep);
	void BreakSep(std::string const &rkText, VEC_STRING &rkOutVec, char const* szSep);
	template< typename T_STRING >
	size_t ReplaceStr(T_STRING const &rkIn, T_STRING const& kFromStr, T_STRING const& kToStr, T_STRING& rkOut)
	{
		rkOut = rkIn;
		if( rkIn.empty() || kFromStr.empty() )
		{
			return 0;
		}

		T_STRING::size_type kLastFind = 0;
		T_STRING::size_type kFindPos = rkOut.find(kFromStr);
		while(T_STRING::npos != kFindPos)
		{
			kLastFind = kFindPos + kToStr.size();
			rkOut.replace(kFindPos, kFromStr.size(), kToStr);
			kFindPos = rkOut.find(kFromStr, kLastFind);
		}
		return kLastFind;
	}

	

	template<typename T_STRING>
	void EraseStr(T_STRING const &rkIn, T_STRING const& rkTrgStr, T_STRING& rkOut)
	{
		T_STRING kEmptyString;
		ReplaceStr<T_STRING>(rkIn, rkTrgStr, kEmptyString, rkOut);
	}
	//void ReplaceStrW(std::wstring const &rkIn, std::wstring const kFromStr, std::wstring const kToStr, std::wstring& rkOut);
	//void EraseStrW(std::wstring const &rkIn, std::wstring const rkTrgStr, std::wstring& rkOut);
	
	template< typename T_STRING >
	void LTrim( T_STRING& rkStr, const T_STRING kTrimStr )
	{
		rkStr.erase( 0, rkStr.find_first_not_of( kTrimStr ) );
	}

	template< typename T_STRING >
	void RTrim( T_STRING& rkStr, const T_STRING kTrimStr )
	{
		rkStr.erase( rkStr.find_last_not_of( kTrimStr ) + 1 );
	}

	template< typename T_STRING >
	void Trim( T_STRING& rkStr, const T_STRING kTrimStr )
	{
		LTrim( rkStr, kTrimStr );
		RTrim( rkStr, kTrimStr );
	}

	template< typename T_STRING >
	void TrimAll( const T_STRING kIn, const T_STRING kTrimStr, T_STRING& rkOut )
	{
		EraseStr< T_STRING >( kIn, kTrimStr, rkOut );
	}

	void ConvFullToHalf(std::wstring& rkString);
}

#endif // WEAPON_VARIANT_UTIL_PGSTRINGUTIL_H