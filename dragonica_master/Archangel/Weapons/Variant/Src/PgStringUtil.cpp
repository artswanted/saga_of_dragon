#include "stdafx.h"
#include "PgStringUtil.h"

namespace PgStringUtil
{
	int const SafeAtoi(char const* szText)
	{
		if( szText )
		{
			return atoi(szText);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}


	int const SafeAtoi(std::string const &rkText)
	{
		return SafeAtoi(rkText.c_str());
	}

	
	int const SafeAtoi(std::wstring const &rkText)
	{
		if( !rkText.empty() )
		{
			return _wtoi(rkText.c_str());
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return 0;
	}


	bool const SafeAtob(char const* szText, bool bDefault)
	{
		if( szText )
		{
			return SafeAtob( std::string(szText) );
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0"));
		return bDefault;
	}


	bool const SafeAtob(std::string const& rkText, bool bDefault)
	{
		if( rkText.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return bDefault;
		}
		if( std::string("0") != rkText )
		{
			return true;
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
		return false;
	}


	float const SafeAtof(char const* szText)
	{
		if( szText )
		{
			return static_cast<float>(atof(szText));
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0.f"));
		return 0.f;
	}


	float const SafeAtof(std::string const& rkText)
	{
		if( rkText.empty() )
		{
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return 0.f"));
			return 0.f;
		}
		return SafeAtof(rkText.c_str());
	}


	std::wstring const SafeUni(char const* szText)
	{
		if( szText )
		{
			return UNI(szText);
		}
		LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return NULL String"));
		return _T("");
	}

	int SafeStrcmp(char const* str1, char const* str2)
	{
		if(!str1){ return -1; }
		if(!str2){ return  1; }
		
		return strcmp(str1, str2);
	}

	void BreakSep(std::wstring const &rkText, VEC_WSTRING &rkOutVec, TCHAR const* szSep)
	{
		std::wstring kTemp = rkText;
		TCHAR* pToken = NULL;
		TCHAR* pNextToken = NULL;
		pToken = _tcstok_s(&kTemp[0], szSep, &pNextToken);
		while(pToken)
		{
			rkOutVec.push_back(pToken);
			pToken = _tcstok_s(NULL, szSep, &pNextToken);
		}

		if( rkOutVec.empty() )
		{
			rkOutVec.push_back(rkText);
			return;
		}
	}


	void BreakSep(std::string const &rkText, VEC_STRING &rkOutVec, char const* szSep)
	{
		std::string kTemp = rkText;
		char* pToken = NULL;
		char* pNextToken = NULL;
		pToken = strtok_s(&kTemp[0], szSep, &pNextToken);
		while(pToken)
		{
			rkOutVec.push_back(pToken);
			pToken = strtok_s(NULL, szSep, &pNextToken);
		}

		if( rkOutVec.empty() )
		{
			rkOutVec.push_back(rkText);
			return;
		}
	}

	void ConvHalfToFullChar(wchar_t& cHalf)
	{
		if( cHalf > 0x0021 && cHalf <= 0x007e )
		{
			cHalf += 0xfee0;
		}
		else if( cHalf == 0x0020 )
		{
			cHalf = 0x3000;
		}
	}
	void ConvFullToHalfChar(wchar_t& cFull)
	{
		if( cFull > 0xff00 && cFull <= 0xff5e )
		{
			cFull -= 0xfee0;
		}
		else if( cFull == 0x3000 )
		{
			cFull = 0x0020;
		}
	}

	void ConvFullToHalf(std::wstring& rkString)
	{
		std::for_each(rkString.begin(), rkString.end(), ConvFullToHalfChar);
	}
}