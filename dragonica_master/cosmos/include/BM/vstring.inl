#pragma once

#include "vstring_traits.h"

namespace BM
{
	template<typename T_STRING>
	void vstring::CutTextByKey(T_STRING const& kOrg, T_STRING const& kKey, std::list< T_STRING > &rOut, bool bEmptyLineUse)
	{
		T_STRING::size_type start_pos = 0;
		while(start_pos != T_STRING::npos)
		{
			T_STRING::size_type const find_pos = kOrg.find( kKey.c_str(), start_pos);

			T_STRING strCut;
			if(find_pos != T_STRING::npos)
			{
				strCut = kOrg.substr(start_pos, find_pos - start_pos);
				start_pos = find_pos+kKey.length();
			}
			else
			{
				strCut = kOrg.substr(start_pos);
				start_pos = T_STRING::npos;
			}

			if(bEmptyLineUse || !strCut.empty())
			{
				rOut.push_back(strCut);
			}
		}
	}

	template<typename T_RETURN>
	HRESULT vstring::GetNumber(std::wstring const& kFormat, T_RETURN& kReturn)
	{
		if (size() <= 0)
		{
			return E_FAIL;
		}
		if (swscanf_s(this->operator wchar_t const*(), kFormat.c_str(), &kReturn) > 0)
		{
			return S_OK;
		}
		return E_FAIL;
	}

	template<typename T_STRING>
	void vstring::ConvToUPR(T_STRING &wstrSrc)
	{
		std::transform( wstrSrc.begin(), wstrSrc.end(), wstrSrc.begin(), towupper );
	}

	template<typename T_STRING>
	void vstring::ConvToLWR(T_STRING &wstrSrc)
	{
		std::transform( wstrSrc.begin(), wstrSrc.end(), wstrSrc.begin(), towlower );
	}

	template<typename T>
	HRESULT vstring::Set( T const &rhs, wchar_t *pkFormat )
	{
		if ( true == _Trait_CheckFormatStr<T>()( pkFormat ) )
		{
			wchar_t pkTemp[MAX_PATH] = {0,};
			::swprintf_s( pkTemp, MAX_PATH, pkFormat, rhs );
			m_wstr = pkTemp;
			return S_OK;
		}
		return E_FAIL;
	}

	template< typename T_REPLACE >
	void vstring::Replace( std::wstring const &kFindStr, T_REPLACE const &kReplaceData )
	{
		vstring kReplaceStr( kReplaceData );
		_Trait_ReplaceStr< std::wstring, vstring >::Replace( m_wstr, kFindStr, kReplaceStr );
	}

	template< >
	void vstring::Replace( std::wstring const &kFindStr, std::wstring const &kReplaceData )
	{
		_Trait_ReplaceStr< std::wstring, std::wstring >::Replace( m_wstr, kFindStr, kReplaceData );
	}

	template< >
	void vstring::Replace( std::wstring const &kFindStr, std::string const &kReplaceData )
	{
		_Trait_ReplaceStr< std::wstring, std::string >::Replace( m_wstr, kFindStr, kReplaceData );
	}

	template< >
	void vstring::Replace( std::wstring const &kFindStr, vstring const &kReplaceData )
	{
		_Trait_ReplaceStr< std::wstring, vstring >::Replace( m_wstr, kFindStr, kReplaceData );
	}
};
