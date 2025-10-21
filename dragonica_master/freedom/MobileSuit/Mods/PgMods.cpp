#include "stdafx.h"
#include "PgMods.h"
#include "Windows.h"
#include "drem/sys/os/filesystem.h"
#include "Variant/tabledatamanager.h"

static void LoadFolder(const char* kRoot, const char* kFindSelector, void(*callback)(const char* kRoot, const WIN32_FIND_DATAA& kFileData))
{
    WIN32_FIND_DATAA kFindData;
	HANDLE hFind = FindFirstFileA(kFindSelector, &kFindData);
    if(INVALID_HANDLE_VALUE == hFind)
    {
        FindClose(hFind);
        return;
    }

	do
	{
		callback(kRoot, kFindData);
	} while(FindNextFileA(hFind, &kFindData));
    FindClose(hFind);
}

static void LoadDefString(const char* kRoot, const WIN32_FIND_DATAA& kFileData)
{
    char fileName[MAX_PATH];
	sprintf_s(fileName, MAX_PATH, "%s/%s", kRoot, kFileData.cFileName);
    TiXmlDocument kDoc;
    if(kDoc.LoadFile(fileName))
    {
        g_kTblDataMgr.ParseDefStringXML(kDoc.FirstChild());
    }
}

static void LoadTextTable(const char* kRoot, const WIN32_FIND_DATAA& kFileData)
{

}

static void LoadSingleMod(const char* kRoot, const WIN32_FIND_DATAA& kFileData)
{
    if ((kFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0 &&
        (::strcmp(kFileData.cFileName, ".") && ::strcmp(kFileData.cFileName, "..")))
    {
        char fileName[MAX_PATH];
        char searchKey[MAX_PATH];
	    sprintf_s(fileName, MAX_PATH, "%s/%s/DefStrings/", kRoot, kFileData.cFileName);
	    sprintf_s(searchKey, MAX_PATH, "%s/*.xml", fileName);
        LoadFolder(fileName, searchKey, LoadDefString);

	    sprintf_s(fileName, MAX_PATH, "%s/%s/TextTable/", kRoot, kFileData.cFileName);
	    sprintf_s(searchKey, MAX_PATH, "%s/*.xml", fileName);
        LoadFolder(fileName, searchKey, LoadDefString);
    }
}

void PgMods::Load(const char* kFolder)
{
    if (kFolder)
    {
	    char fileName[MAX_PATH];
	    sprintf_s(fileName, MAX_PATH, "%s/*.*", kFolder);
        LoadFolder(kFolder, fileName, LoadSingleMod);
    }
}