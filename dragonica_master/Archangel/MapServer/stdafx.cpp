// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// MapServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include "PgSmallArea.h"
// TODO: 필요한 추가 헤더는


bool GetDefString(int const iTextNo, const wchar_t *&pString)
{
	const CONT_DEFSTRINGS* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);

	CONT_DEFSTRINGS::const_iterator str_itor = pContDefStrings->find(iTextNo);
	if(str_itor != pContDefStrings->end())
	{
		pString = (*str_itor).second.strText.c_str();
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool GetDefString(int const iTextNo, std::wstring &kOutString )
{
	const CONT_DEFSTRINGS* pContDefStrings = NULL;
	g_kTblDataMgr.GetContDef(pContDefStrings);

	CONT_DEFSTRINGS::const_iterator str_itor = pContDefStrings->find(iTextNo);
	if(str_itor != pContDefStrings->end())
	{
		kOutString = (*str_itor).second.strText;
		return true;
	}
	LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
	return false;
}

bool lwIsRandSuccess(int iValue)
{
	int const iRet = BM::Rand_Index(ABILITY_RATE_VALUE);
	
	if(iRet < iValue)
	{
		return true;
	}
	return false;
}

bool lwIsRandSuccess(int const iValue, DWORD const dwRate)
{
	int const iRet = BM::Rand_Index(dwRate);
	
	if(iRet < iValue)
	{
		return true;
	}
	return false;
}
