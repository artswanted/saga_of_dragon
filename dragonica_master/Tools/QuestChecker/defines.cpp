#include "stdafx.h"
#include "defines.h"

std::wstring const WSTR_QUESTLISTCOLUMN	= L"@ID|50@TXTID|55@Name|230@AREAID|55@Area|120@MinLevel|70@MaxLevel|70";
std::wstring const WSTR_QUESTLISTITEM	= L"%d@%d@%s@%d@%s@%d@%d";
std::wstring const WSTR_TEXTLISTCOLUMN	= L"@Dlg ID|70@TxtID|70@CONTANT|580";
std::wstring const WSTR_TEXTLISTITEM	= L"@%d@%d@%s";
std::wstring const WSTR_LOGLISTCOLUMN	= L"@LOG|500";
std::wstring const WSTR_LOGERRLISTCOLUMN= L"@TYPE|120@ID|50@MSG|500";
std::wstring const WSTR_LOGERRLISTITEM	= L"@%s@%d@%s";
std::wstring const WSTR_QUESTEMPTY		= L"@Q-ID|100";
			 
std::wstring const WSTR_QUEST_XML	= L"./XML/QUEST/";
std::wstring const WSTR_QUEST_QTT	= L"./XML/QuestTextTable.XML";
std::wstring const WSTR_QUEST_QTT2	= L"./XML/QuestTextTable_New.XML";
std::wstring const WSTR_QUEST_QTT_NEW2	= L"./XML/QuestTextTable_New2.XML";
std::wstring const WSTR_QUEST_QTT_NEW3	= L"./XML/QuestTextTable_New3.XML";
std::wstring const WSTR_QUEST_QTT_WANTED	= L"./XML/QuestTextTable_Wanted.XML";
std::wstring const WSTR_TT			= L"./XML/TextTable.XML";
std::wstring const WSTR_TABLE		= L"./TABLE/";
std::wstring const WSTR_ALL_NPC		= L"./XML/WORLD/All_Npc.xml";
std::wstring const WSTR_NULLSTRING	= L"";

std::wstring const WSTR_CONFIG_FILE = L"./Checker.ini";

bool g_bDBCon = true;
PgQuestInfoVerifyUtil::LPOUTPUTERRORFUNC g_lpAddErrorFunc;
ContErrorMsg g_kErrorMsg;
Loki::Mutex g_kError;



//	Set Work Directory
void SetLocation()
{
	setlocale(LC_ALL, "");
	char path[MAX_PATH];

	GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);

	if (strlen(path)==0)
	{
		return;
	}

	strrchr( path, '\\' )[1] = 0;
	SetCurrentDirectoryA(path);
}

namespace DevelopeFunction
{
	int const iUseDebugDevelop = 
#ifdef NO_DEBUG_DEV
		0
#else
		1
#endif
	; //
}