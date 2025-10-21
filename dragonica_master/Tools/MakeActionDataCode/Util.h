#pragma once
#include "stdafx.h"
typedef std::map<std::string, std::string> CONT_ACTION_PATH; // ActionID, Path
typedef std::vector<std::string> CONT_STR;

void ConvToUPR(std::string &strSrc);
bool IsActionType(char const* pcStr);
bool ParsePathXML(std::string const kPath, CONT_ACTION_PATH& rkOutCont);

void WriteToFile(FILE* pFile, std::string const& kStr);

std::string  UTF16ToMBCS(std::wstring const& kUTF16);
std::wstring MBCSToUTF16(std::string const& kMBCS);

bool ReplaceStringOneAtaTime(std::string const& kOrigStr, std::string const& kReplaceStr, std::string const& kBeginStr, std::string const& kEndStr, std::string& rkOutStr);
void ReplaceString(std::string const& kOrigStr, std::string const& kReplaceStr, std::string const& kBeginStr, std::string const& kEndStr, std::string& rkOutStr);
void GetBetweenStringOneAtaTime(std::string const& kOrigStr, std::string const& kBeginStr, std::string const& kEndStr, CONT_STR& rkContOut);