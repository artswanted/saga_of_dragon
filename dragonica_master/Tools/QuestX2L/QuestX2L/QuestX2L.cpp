// QuestX2L.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
#include <crtdbg.h>
#include "PgXml2Lua.h"
#include "bm/vstring.h"

#include <set>

#define SCRIPT "./SCRIPT"
#define XML "./XML"
#define SCRIPT_QUEST "./SCRIPT/Quest/"
#define XML_QUEST "./XML/Quest/"

#define _MKDIR(NAME) \
	_mkdir(NAME);

//PgXml2Lua::m_bDisableCheck = false;

extern bool g_bDisableCheck;

int _tmain(int argc, TCHAR* argv[])
{
	printf_s("이것은 XML파일을 파싱해서 LUA파일을 만들어주는 유틸리티 입니다\n");
	printf_s("Simple is Best\n\n\n\n\n");

	std::vector<PgXml2Lua> kXmlVec;

	printf_s("Table.bin 로드 시작\n");
	try
	{
		const bool bLoadTB = LoadTBData(_T("./Table/"));
		if( bLoadTB )
		{
			g_bDisableCheck = false;
			printf_s("Table.bin 로드 완료\n\n\n\n\n");
		}
	}
	catch(...)
	{
		g_bDisableCheck = true;
		printf_s("Table.bin 로드 실패\n\n\n\n\n");

		return 0;
	}

	//
	//
	unsigned int i=0;

	//Environment
	_MKDIR("SCRIPT");
	_MKDIR("XML");
	_MKDIR("SCRIPT/Quest");
	_MKDIR("XML/Quest");

	//----------------------------------------------------------------------------------------
	// Parsing from xml
	// and build quest data
	//----------------------------------------------------------------------------------------
	const std::string kCurDir = _getcwd(NULL, 0);
	std::string kFindTarget = kCurDir.c_str();
	kFindTarget += "\\*";
	//printf_s("%s\n", kFindTarget.c_str());

	HANDLE hFind = NULL;
	WIN32_FIND_DATA kFFData;
	hFind = FindFirstFile(UNI(kFindTarget.c_str()), &kFFData);
	if(INVALID_HANDLE_VALUE == hFind)
	{
		printf_s("FindFirstFile Error\n");
	}
	else
	{
		while(0 != FindNextFile(hFind, &kFFData))
		{
			std::string kName = MB(kFFData.cFileName);
			std::string kExt;

			BreakExtern(kName, kExt);
			if(3 != kName.size() && 0 == strcmp("XML", kExt.c_str()))
			{
				PgXml2Lua kXml(kName);

				//std::vector<PgXml2Lua>::iterator kIter = kXmlVec.begin();
				bool bRet = kXml.OpenXml();//Parse Immediate Now
				if(bRet)
				{
					printf_s("Parse Success: %s\n", kName.c_str());
					kXmlVec.push_back(kXml);//Push Back
				}
				else
				{
					printf_s("/aParse Failed: %s\n", kName.c_str());
					exit(0);
				}
			}
		}//End while
	}
	FindClose(hFind);


	//----------------------------------------------------------------------------------------
	// pre check
	//----------------------------------------------------------------------------------------
	std::set<int> kSet;
	for(i=0; i<kXmlVec.size(); ++i)
	{
		const int iQuestID = kXmlVec[i].ID();
		std::set<int>auto eRet = kSet.insert(iQuestID);
		if( !eRet.second )
		{
			printf_s("중복된 Quest ID [%d] 입니다. [%s]\n", iQuestID, kXmlVec[i].m_kInputName.c_str());
			for(unsigned int j=0;kXmlVec.size()>j; ++j)
			{
				if( kXmlVec[j].ID() == iQuestID )
				{
					printf_s("중복된 다른 Qeust [%d] 입니다. [%s]\n", kXmlVec[j].ID(), kXmlVec[j].m_kInputName.c_str());
					break;
				}
			}
			assert(0); exit(0); return false;
		}
	}


	//----------------------------------------------------------------------------------------
	// build lua files
	//----------------------------------------------------------------------------------------
	for(i=0; i<kXmlVec.size(); ++i)
	{
		//printf_s("%s : %s\n", kXmlVec[i].m_kInputName.c_str(), kXmlVec[i].m_kOutputName.c_str());
		kXmlVec[i].PrintOutLua();
	}

	//Quest_List.lua 와 Quest.ini 파일 생성
	FILE *pFileIni = NULL;
	FILE *pFileList = NULL;
	errno_t eRet = 0;
	eRet = fopen_s(&pFileIni, "./SCRIPT/Quest/Quest.ini", "w");
	if(0 != eRet || NULL == pFileIni)
	{
		printf_s("Can't open Quest.ini\n");
		return 0;
	}
	eRet = fopen_s(&pFileList, "./SCRIPT/Quest/Quest_List.lua", "w");
	if(0 != eRet || NULL == pFileList)
	{
		printf_s("Can't open Quest_List.lua\n");
		return 0;
	}
	
	//
	for(i=0; i<kXmlVec.size(); ++i)
	{
		char szXmlName[_MAX_PATH] = {0, };
		char szLuaName[_MAX_PATH] = {0, };

		const int iQuestID = kXmlVec[i].ID();
		sprintf_s(szXmlName, _MAX_PATH, "Quest%08d.xml", iQuestID);
		sprintf_s(szLuaName, _MAX_PATH, "Quest%08d.lua", iQuestID);

		fprintf_s(pFileIni, "%s\n", szXmlName);
		fprintf_s(pFileList, "dofile(\"Script/Quest/%s\")\n", szLuaName);
	}
	fclose(pFileIni);
	fclose(pFileList);

	//완료
	printf_s("\n\n\n\n\n");
	printf_s("모든작업이 완료 되었습니다.\n");
	printf_s("\n\n\n\n\n");
	
	return 0;
}