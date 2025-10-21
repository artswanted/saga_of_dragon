#include "stdafx.h"
#include "PgUtility.h"

bool PgUtility::ToAbsolutePath(const char *pcRelativePath, const char *pcBasePath, std::string &rkAbsolutePath)
{
	// 경로를 절대 경로로 바꾼다.
	std::string kAbsolutePath(pcRelativePath);
	if(kAbsolutePath.find(":") != std::string::npos)
	{
		rkAbsolutePath = pcRelativePath;
		return 0;
	}

	std::string::size_type kPos = kAbsolutePath.find_first_of("/");
	if(kPos == std::string::npos)
	{
		kPos = kAbsolutePath.find_first_of("\\");
	}

	if(kPos != std::string::npos)
	{
		kAbsolutePath = kAbsolutePath.substr(kPos);
		kAbsolutePath.insert(0, pcBasePath);
	}

	rkAbsolutePath = kAbsolutePath;

	return true;
}


bool PgUtility::ToSuitableRelativePath(const char *pcRelativePath, const char *pcBasePath, std::string &rkRelativePath)
{
	std::string kSuitablePath(pcRelativePath);
	std::string::size_type kPos = kSuitablePath.find_first_of("/");
	if(kPos == std::string::npos)
	{
		kPos = kSuitablePath.find_first_of("\\");
	}

	if(kPos != std::string::npos)
	{
		kSuitablePath = kSuitablePath.substr(kPos);
		kSuitablePath.insert(0, pcBasePath);
	}

	rkRelativePath = kSuitablePath;

	return true;
}