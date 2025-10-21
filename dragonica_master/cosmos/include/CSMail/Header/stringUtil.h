#pragma once

#include "BM/STLSupport.h"


class StringUtil
{
public:
	void TrimLeft(std::wstring& s);
	void TrimRight(std::wstring& s);
	std::wstring Left(std::wstring& s, int i);
	std::wstring Right(std::wstring& s, int i);
	std::wstring Mid(std::wstring& s, int first, int n);

};


