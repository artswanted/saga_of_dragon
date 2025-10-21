#include "stdafx.h"
#include "VariableContainer.h"


PgVariableContainer::PgVariableContainer()
{
}

PgVariableContainer::~PgVariableContainer()
{
}

bool PgVariableContainer::LoadIni(std::wstring const &rkfilename)
{
	BM::CAutoMutex kLock(m_kMutex, true);
	typedef std::map<std::wstring, int> CONT_TEMP_TYPE;

	DWORD dwBufferSize = 400;
	LPTSTR lpszBuffer = NULL;
	DWORD dwSize = dwBufferSize-2;
	while (dwSize == dwBufferSize-2 && dwBufferSize < 60000)
	{
		// 모두 읽을 수 있는 버퍼 크기가 될때까지 반복~
		if (lpszBuffer != NULL)
		{
			delete[] lpszBuffer;
			lpszBuffer = NULL;
		}
		dwBufferSize *= 2;
		lpszBuffer = new_tr TCHAR[dwBufferSize];
		dwSize = ::GetPrivateProfileString(NULL, NULL, NULL, lpszBuffer, dwBufferSize, rkfilename.c_str());
		if (dwSize <= 0)
		{
			if (lpszBuffer != NULL)
			{
				delete[] lpszBuffer;
			}
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot read ini section_name ["<<rkfilename<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	LPTSTR lpszBegin = lpszBuffer;
	LPTSTR lpszEnd = NULL;
	while (lpszBegin != NULL)
	{
		// Reading [Section_name] ......
		lpszEnd = _tcschr(LPTSTR(lpszBegin+1), _T('\0'));
		if (lpszEnd == NULL && (lpszEnd-lpszBegin) <= 1)
		{
			// section_name is too short
			break;
		}
		ReadIni(lpszBegin, rkfilename.c_str());
		if ((lpszEnd+1) - lpszBuffer >= (int)dwSize)
		{
			// reached to end of buffer string
			break;
		}
		lpszBegin = lpszEnd+1;
	}
	delete[] lpszBuffer;
	return true;
}

// INI 파일 읽어 오기
// [PARAMETER]
//	lpszSectionName == NULL : 전체 [Section_name]을 읽고 있는 중
//	lpszSectionName == "sec_name" : [sec_name] 의 keyname을 읽는 중
// [NOTICE]
//	외부에서는 lpszFilename 값만 넣어주고 호출하면 재귀호출되어 값이 들어감
//	예) ReadIni(NULL, _T("test.ini"));
bool PgVariableContainer::ReadIni(LPCTSTR lpszSectionName, LPCTSTR lpszFilename)
{
	int iSectionNum = 0;
	if (lpszSectionName != NULL)
	{
		TCHAR chSection[100];
		int iReturn = _stscanf_s(lpszSectionName, _T("%d %s"), &iSectionNum, chSection, 100);
		if (iSectionNum <= 0)
		{
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot read section num SectionName["<<lpszSectionName<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}
	
	DWORD dwBufferSize = 400;
	LPTSTR lpszBuffer = NULL;
	DWORD dwSize = dwBufferSize-2;
	while (dwSize == dwBufferSize-2 && dwBufferSize < 60000)
	{
		// 모두 읽을 수 있는 버퍼 크기가 될때까지 반복~
		if (lpszBuffer != NULL)
		{
			delete[] lpszBuffer;
			lpszBuffer = NULL;
		}
		dwBufferSize *= 2;
		lpszBuffer = new_tr TCHAR[dwBufferSize];
		dwSize = ::GetPrivateProfileString(lpszSectionName, NULL, NULL, lpszBuffer, dwBufferSize, lpszFilename);
		if (dwSize <= 0)
		{
			if (lpszBuffer != NULL)
			{
				delete[] lpszBuffer;
			}
			INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot read ini section_name ["<<lpszSectionName<<L"]");
			LIVE_CHECK_LOG(BM::LOG_LV1, __FL__ << _T("Return false"));
			return false;
		}
	}

	// lpszBuffer = _T("section_1\nsection_2\nsection_3\n\n")
	// 이런식으로 Buffer에 저장되어 있음. (StringList)
	LPTSTR lpszBegin = lpszBuffer;
	LPTSTR lpszEnd = NULL;
	while (lpszBegin != NULL)
	{
		// StringList에서 한개씩 읽어 오기
		lpszEnd = _tcschr(LPTSTR(lpszBegin+1), _T('\0'));
		if (lpszEnd == NULL && (lpszEnd-lpszBegin) <= 1)
		{
			// section_name is too short
			break;
		}
		if (lpszBegin[0] != _T('/'))
		{
			if (lpszSectionName == NULL)
			{
				// [Section_name] 을 읽은 것이다.
				ReadIni(lpszBegin, lpszFilename);
			}
			else
			{
				// [Section] 안의 KeyName 을 읽은 것이다.
				ParsingValue(iSectionNum, lpszFilename, lpszSectionName, lpszBegin);
			}
			if ((lpszEnd+1) - lpszBuffer >= (int)dwSize)
			{
				// reached to end of buffer string
				break;
			}
		}
		lpszBegin = lpszEnd+1;
	}
	delete[] lpszBuffer;

	return true;


}

bool PgVariableContainer::ParsingValue(int const iKind, LPCTSTR lpszFilename, LPCTSTR lpszSectionName, LPCTSTR lpszKeyname)
{
	TCHAR chLine[200];
	::GetPrivateProfileString(lpszSectionName, lpszKeyname, NULL, chLine, 200, lpszFilename);
	int iType = 0;
	TCHAR chValue[200];
	_stscanf_s(chLine, _T("%d %s"), &iType, chValue, 200);
	if (iType <= 0)
	{
		INFO_LOG(BM::LOG_LV5, __FL__<<L"Cannot parse get value type ["<<chLine<<L"]");
		return false;
	}

	if (chValue[0] == _T('\"'))	// string type
	{
		std::wstring strValue(&chValue[1], _tcslen(chValue)-2);
		Set(iKind, iType, strValue, true);
	}
	else
	{
		if (_tcschr(chValue, _T('.')) != NULL)
		{
			float fValue = (float) _tstof(chValue);
			Set(iKind, iType, fValue, true);
		}
		else
		{
			int iValue = (int) _ttoi(chValue);
			Set(iKind, iType, iValue, true);
		}
	}
	return true;
}

/*
bool PgVariableContainer::GetActArg(int const iKind, SActArg*& pkOutActArg, bool const bCreate)
{
	CONT_VARIABLE::iterator itor = m_kContVariable.find(iKind);
	if (itor == m_kContVariable.end())
	{
		if (!bCreate)
		{
			pkOutArcArg = NULL;
			return false;
		}
		auto ibRet = m_kContVariable.insert(std::make_pair(iKind, SActArg());
		itor = itor.first;
	}
	pkOutActArg = &(itor->second);
	return true;
}
*/