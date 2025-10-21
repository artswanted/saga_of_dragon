#include "stdafx.h"

#include "BM/bm.h"
#include "BM/FileSupport.h"

bool InbMake(std::wstring strName)
{
	if(strName.empty())
	{
		return false;
	}

	std::vector< char > vecData;

	if(!BM::FileToMem(strName, vecData))
	{
		return false;
	}

	std::wstring strOut = strName.substr(0, strName.size()-4);
	strOut  = strOut + _T(".inb");

	if(!BM::EncSave(strOut, vecData))
	{
		return false;
	}

	return true;
}
