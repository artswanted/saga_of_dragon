#pragma once

class PgUtility
{
public:
	static bool ToAbsolutePath(const char *pcRelativePath, const char *pcBasePath, std::string &rkAbsolutePath);
	static bool ToSuitableRelativePath(const char *pcRelativePath, const char *pcBasePath, std::string &rkAbsolutePath);

};