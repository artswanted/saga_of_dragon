// stdafx.cpp : source file that includes just the standard includes
// QuestX2L.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <ctype.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void BreakSep(const std::wstring &rkText, ContWStrVec &rkVec, std::wstring kSep)
{
	std::wstring kTemp = rkText;
	TCHAR* pToken = NULL;
	TCHAR* pNextToken = NULL;
	pToken = _tcstok_s(&kTemp[0], kSep.c_str(), &pNextToken);

	if( !pToken )
	{
		rkVec.push_back(rkText);
		return;
	}
	rkVec.push_back(pToken);

	while(1)
	{
		pToken = _tcstok_s(NULL, kSep.c_str(), &pNextToken);
		if( pToken != NULL )
		{
			rkVec.push_back( pToken );
		}
		else
		{
			break;
		}
	}
}

void BreakSep(const std::string &rkText, ContStrVec &rkVec, std::string kSep)
{
	std::string kTemp = rkText;
	char* pToken = NULL;
	char* pNextToken = NULL;
	pToken = strtok_s(&kTemp[0], kSep.c_str(), &pNextToken);

	if( !pToken )
	{
		rkVec.push_back(rkText);
		return;
	}
	rkVec.push_back(pToken);

	while(1)
	{
		pToken = strtok_s(NULL, kSep.c_str(), &pNextToken);
		if( pToken != NULL )
		{
			rkVec.push_back( pToken );
		}
		else
		{
			break;
		}
	}
}

void BreakLastSep(const std::string &rkText, std::string &rkOut)
{
	std::vector<std::string> kVec;
	BreakSep(rkText, kVec, ".");
	rkOut = kVec[0];
	for(unsigned int i=1; i<kVec.size()-1; ++i)
	{
		rkOut += ".";
		rkOut += kVec[i];
	}
}

void ToUpper(std::string &rkText)
{
	std::string::iterator kIter = rkText.begin();
	while(rkText.end() != kIter)
	{
		*kIter = toupper(*kIter);
		++kIter;
	}
}

void BreakExtern(const std::string &rkFile, std::string &rkExt)
{
	//std::string kName = kFFData.cFileName;
	//printf_s("%s\n", kName.c_str());
	std::vector<std::string> kVec;
	BreakSep(rkFile, kVec, ".");

	rkExt = kVec[kVec.size()-1];
	ToUpper(rkExt);
}

bool operator != (const std::string& rkLeft, const std::string& rkRight)
{
	return !(rkLeft == rkRight);
}

bool operator == (const std::string& rkLeft, const std::string& rkRight)
{
	return 0 == strcmp(rkLeft.c_str(), rkRight.c_str());
}

bool operator == (const std::string& rkLeft, const char* szRight)
{
	return 0 == strcmp(rkLeft.c_str(), szRight);
}

bool operator == (const char* szLeft, const std::string& rkRight)
{
	return 0 == strcmp(szLeft, rkRight.c_str());
}

int atoi(const std::string& rkStr)
{
	return atoi(rkStr.c_str());
}

bool operator < (const SItem& rkLeft, const SItem& rkRight)
{
	const bool bItemNo = rkLeft.ItemNo() < rkRight.ItemNo();
	const bool bCount = rkLeft.Count() < rkRight.Count();
	if( bItemNo && bCount )
	{
		return true;
	}
	else if( bItemNo && !bCount )
	{
		return true;
	}
	else if( !bItemNo && bCount )
	{
		return false;
	}
	return false;
}