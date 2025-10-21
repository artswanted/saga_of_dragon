// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif			

#include <stdio.h>

//#include <Windows.h>
//MobileSuie
//#include "BM/Guid.h"
#include "ServerLib.h"

#include <tchar.h>

#include <vector>
#include <set>
#include <utility>

#define TYPE(NAME, TEXT) const std::wstring TYPE_##NAME = _T(TEXT);

typedef std::vector< std::string > ContStrVec;
typedef std::vector< std::wstring > ContWStrVec;

// TODO: reference additional headers your program requires here
void BreakSep(const std::string &rkText, ContStrVec &rkVec, std::string kSep=" ");
void BreakSep(const std::wstring &rkText, ContWStrVec &rkVec, std::wstring kSep=_T(" "));
void BreakLastSep(const std::string &rkText, std::string &rkOut);
void ToUpper(std::string &rkText);
void BreakExtern(const std::string &rkFile, std::string &rkExt);

bool operator != (const std::string& rkLeft, const std::string& rkRight);
bool operator == (const std::string& rkLeft, const std::string& rkRight);
bool operator == (const std::string& rkLeft, const char* szRight);
bool operator == (const char* szLeft, const std::string& rkRight);
int atoi(const std::string &rkStr);
bool operator < (const SItem& rkLeft, const SItem& rkRight);