#include "stdafx.h"
#include "PgXml2Lua.h"
#include "PgXml2Lua_2.h"
#include "Variant/ItemDefMgr.h"

bool g_bDisableCheck = false;

//>>
bool PgXml2Lua::NpcPreEvent(const ContQuestDenpendNpc::value_type& rkElement, FILE* pFile)
{
	//ONDIALOG Pre EVENT
	ContWStrVec kVec;
	BreakSep(rkElement.kValue, kVec, _T("/"));
	const TCHAR* szType = rkElement.kType.c_str();

	if( TYPE_CHECKPARAM == szType )
	{
		if(4 != kVec.size())
		{
			printf_s("인자 수 오류!! 인자는 4개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"현재 다이얼로그ID/다음 다이얼로그ID/확인할 Parameter번호/실패시 이동할 다이얼로그ID\" \n");
			assert(0); Exit(0); return false;
		}
		//Cur Dialog ID, Next Dialog ID, Check Param No, Failed Dialog ID
		const TCHAR* szCurDialogID = kVec[0].c_str();
		const TCHAR* szNextDialogID = kVec[1].c_str();
		const TCHAR* szCheckParamNo = kVec[2].c_str();
		const TCHAR* szFailedDialogID = kVec[3].c_str();
		_ftprintf_s(pFile, ONDIALOG_CHECKPARAM, szCurDialogID, szNextDialogID, szCheckParamNo, szFailedDialogID);
		return true;
	}
	else if( TYPE_CHECKITEM == szType )
	{
		if( 5 == kVec.size() )
		{
			//Cur Dialog ID, Next Dialog ID, Item No, Minimum Item Count, Failed Dialog ID
			const TCHAR* szCurDialogID = kVec[0].c_str();
			const TCHAR* szNextDialogID = kVec[1].c_str();
			const TCHAR* szItemNo = kVec[2].c_str();
			const TCHAR* szItemCount = kVec[3].c_str();
			const TCHAR* szFailedDialogID = kVec[4].c_str();
			CheckItem(_ttoi(szItemNo), NULL);
			_ftprintf_s(pFile, ONDIALOG_CHECKITEM_5, szCurDialogID, szNextDialogID, szItemNo, szItemCount, szFailedDialogID);
		}
		else if( 4 == kVec.size() )
		{
			//Cur Dialog ID, Item No, Minimum Item Count, Failed Dialog ID
			const TCHAR* szCurDialogID = kVec[0].c_str();
			const TCHAR* szItemNo = kVec[1].c_str();
			const TCHAR* szItemCount = kVec[2].c_str();
			const TCHAR* szFailedDialogID = kVec[3].c_str();
			CheckItem(_ttoi(szItemNo), NULL);
			_ftprintf_s(pFile, ONDIALOG_CHECKITEM_4, szCurDialogID, szItemNo, szItemCount, szFailedDialogID);
		}
		else
		{
			printf_s("인자 수 오류!! ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"현재 다이얼로그ID/아이템 번호/몇몇 '이상' 수량/실패시 이동할 다이얼로그ID\" \n");
			printf_s("VALUE=\"현재 다이얼로그ID/다음 다이얼로그ID/아이템 번호/몇몇 '이상' 수량/실패시 이동할 다이얼로그ID\" \n");
			assert(0); Exit(0); return false;
		}
		return true;
	}
	else if( TYPE_CHECKGOLD == szType )
	{
		if( 6 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 5개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"현재 다이얼로그ID/부호(<,>,==,!=,<=,>=)/금/은/동/실패시 이동할 다이얼로그ID\" \n");
			assert(0); Exit(0); return false;
		}
		//Cur Dialog DI, Sign, Gold, Silver, Bronze, Failed Dialog ID
		const TCHAR* szCurDialogID = kVec[0].c_str();
		const TCHAR* szSign = kVec[1].c_str();
		const TCHAR* szGold = kVec[2].c_str();
		const TCHAR* szSilver = kVec[3].c_str();
		const TCHAR* szBronze = kVec[4].c_str();
		const TCHAR* szFailedDialogID = kVec[5].c_str();
		_ftprintf_s(pFile, ONDIALOG_CHECKGOLD, szCurDialogID, szSign, szGold, szSilver, szBronze, szFailedDialogID);
		return true;
	}
	else if( TYPE_CLIENT == szType ){}
	else if( TYPE_PAYER ==  szType ){}
	else if( TYPE_ING_DLG == szType ){}
	else if( TYPE_GIVEITEM == szType ){}
	else if( TYPE_REMOVEITEM == szType ){}
	else if( TYPE_INCPARAM == szType ){}
	else if( TYPE_COMPLETEQUEST == szType ){}
	else if( TYPE_ADDGOLD == szType ){}
	else
	{
		printf_s("잘못된 이름 TYPE=\"%s\" \n", MB(szType));
		assert(0); Exit(0); return false;
	}
	return false;
}

bool PgXml2Lua::NpcAfterEvent(const ContQuestDenpendNpc::value_type& rkElement, FILE* pFile)
{
	//ONDIALOG After EVENT
	ContWStrVec kVec;
	BreakSep(rkElement.kValue, kVec, _T("/"));
	const TCHAR* szType = rkElement.kType.c_str();
	
	if( TYPE_REMOVEITEM == szType)
	{
		if( 3 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 3개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"진행할 다이얼로그ID/아이템번호/제거할 수량\" \n");
			assert(0); Exit(0); return false;
		}
		//Result Next Dialog ID, ItemNo, ItemCount
		const TCHAR* szResultNextID = kVec[0].c_str();
		const TCHAR* szItemNo = kVec[1].c_str();
		const TCHAR* szItemCount = kVec[2].c_str();
		CheckItem(_ttoi(szItemNo), NULL);
		_ftprintf_s(pFile, ONDIALOG_EVENT_REMOVEITEM, szResultNextID, szItemNo, szItemCount);
		return true;
	}
	else if( TYPE_GIVEITEM == szType)
	{
		if( 3 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 3개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"진행할 다이얼로그ID/아이템번호/지급할 수량\" \n");
			assert(0); Exit(0); return false;
		}
		//Result next Dialog ID, ItemNo, ItemCount
		const TCHAR* szResultNextID = kVec[0].c_str();
		const TCHAR* szItemNo = kVec[1].c_str();
		const TCHAR* szItemCount = kVec[2].c_str();
		CheckItem(_ttoi(szItemNo), NULL);
		_ftprintf_s(pFile, ONDIALOG_EVENT_GIVEITEM, szResultNextID, szItemNo, szItemCount);
		return true;
	}
	else if( TYPE_INCPARAM == szType)
	{
		if( 2 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 2개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"진행할 다이얼로그ID/Parameter가 증가할 수\" \n");
			assert(0); Exit(0); return false;
		}

		if( 0 > rkElement.iObjectNo || 4 < rkElement.iObjectNo )
		{
			printf_s("NPC 오브젝트 번호 오류 ... TYPE=\"%s\" OBJECTNO=\"%d\" \n", MB(szType), rkElement.iObjectNo+1);
			printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"진행할 다이얼로그ID/Parameter가 증가할 수\" \n");
			assert(0); Exit(0); return false;
		}
		//Result Next Dialog ID, ParamNo, Inc Val
		const TCHAR* szResultNextID = kVec[0].c_str();
		const TCHAR* szIncVal = kVec[1].c_str();
		_ftprintf_s(pFile, ONDIALOG_EVENT_INCPARAM, szResultNextID, rkElement.iObjectNo, szIncVal);
		return true;
	}
	else if( TYPE_COMPLETEQUEST == szType)
	{
		if( 2 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 2개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"진행할 다이얼로그ID/완료시킬 퀘스트ID\" \n");
			assert(0); Exit(0); return false;
		}
		printf_s("!!주의!! 이 Type으로 자기자신을 완료 시킬 경우 유저가 보상을 못 받을 수 있습니다\n");
		//Result Next Dialog ID, QuestID
		const TCHAR* szResultNextID = kVec[0].c_str();
		const TCHAR* szQuestID = kVec[1].c_str();
		_ftprintf_s(pFile, ONDIALOG_EVENT_COMPLETEQUEST, szResultNextID, szQuestID);
		return true;
	}
	//else if(TYPE_DROPQUEST == szType)
	//{
	//	if( 2 != kVec.size() )
	//	{
	//		printf_s("인자 수 오류!! 인자는 2개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
	//		printf_s("VALUE=\"진행할 다이얼로그ID/포기 할 퀘스트ID\" \n");
	//		assert(0); Exit(0); return false;
	//	}
	//	//Result Next Dialog ID, QuestID
	//	const TCHAR* szResultNextID = kVec[0].c_str();
	//	const TCHAR* szQuestID = kVec[1].c_str();
	//	return true;
	//}
	else if( TYPE_ADDGOLD == szType )
	{
		if( 5 != kVec.size() )
		{
			printf_s("인자 수 오류!! 인자는 2개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
			printf_s("VALUE=\"진행할 다이얼로그ID/부호(+,-)/금/은/동\" \n");
			assert(0); Exit(0); return false;
		}
		//Result Next Dialog ID, Sign, Gold, Silver, Bronze
		const TCHAR* szResultNextID = kVec[0].c_str();
		const TCHAR* szSign = kVec[1].c_str();
		const TCHAR* szGold = kVec[2].c_str();
		const TCHAR* szSilver = kVec[3].c_str();
		const TCHAR* szBronze = kVec[4].c_str();
		_ftprintf_s(pFile, ONDIALOG_EVENT_ADDGOLD, szResultNextID, szSign, szGold, szSilver, szBronze);
		return true;
	}
	else if( TYPE_CLIENT == szType ){}
	else if( TYPE_PAYER == szType ){}
	else if( TYPE_ING_DLG == szType ){}
	//else if( TYPE_GIVEITEM == szType ){}
	//else if( TYPE_REMOVEITEM == szType ){}
	//else if( TYPE_INCPARAM == szType ){}
	else
	{
		printf_s("잘못된 이름 TYPE=\"%s\" \n", MB(szType));
		assert(0); Exit(0); return false;
	}
	return false;
}
//<<

/////////////////////////////////////////////////////////////////////////////////////////////////
bool PgXml2Lua::OpenXml()
{
	//Parse Xml
	TiXmlDocument kDoc;

	const bool bLoadRet = kDoc.LoadFile(m_kInputName.c_str());
	if( !bLoadRet )
	{
		printf_s("\tLoad XML Failed: %s - (Check XML Format)\n", m_kInputName.c_str());
		return false;
	}

	TiXmlElement *pkChild = kDoc.FirstChildElement("QUEST");

	bool bRet = false;
	if(NULL != pkChild)
	{
		bRet =  ParseXml(pkChild);
	}
	return bRet;
}
//
//bool PgXml2Lua::ParseXml(TiXmlElement *pkNode)
//{
//	//First Child Element
//	TiXmlElement *pkChild = pkNode->FirstChildElement();
//	while(pkChild)
//	{
//		const TCHAR* szName = pkChild->Value();
//		const TCHAR* szValue = pkChild->GetText();
//
//		bool bRet = true;
//		if(0 == strcmp(szName, "ID"))
//		{
//			m_kQuestID = szValue;
//		}
//		else if(0 == strcmp(szName, "GROUPNO"))
//		{
//		}
//		else if(0 == strcmp(szName, "TITLE"))
//		{
//		}
//		else if(0 == strcmp(szName, "GROUPNAME"))
//		{
//		}
//		else if(0 == strcmp(szName, "LIMITS"))
//		{
//		}
//		else if(0 == strcmp(szName, "CLIENTS"))//
//		{
//		}
//		else if(0 == strcmp(szName, "AGENTS"))
//		{
//		}
//		else if(0 == strcmp(szName, "PAYERS"))
//		{
//		}
//		else if(0 == strcmp(szName, "EVENTS"))
//		{
//			bRet = ParseDependents(pkChild);
//		}
//		else if(0 == strcmp(szName, "OBJECTS"))
//		{
//			bRet = Parseobjects(pkChild);
//		}
//		else if(0 == strcmp(szName, "REWARD"))
//		{
//			bRet = ParseReward(pkChild);
//		}
//		else if(0 == strcmp(szName, "DIALOGS"))
//		{
//		}
//		else if(0 == strcmp(szName, "TEXTS"))
//		{
//		}
//		else
//		{
//			PrintWaring(pkChild);
//			assert(0);
//			return false;
//		}
//
//		if(!bRet)
//		{
//			return false;
//		}
//
//		pkChild = pkChild->NextSiblingElement();
//	}
//	return true;
//}

//bool PgXml2Lua::ParseReward(TiXmlElement *pkNode)
//{
//	TiXmlElement *pkChild = pkNode->FirstChildElement();
//	while(pkChild)
//	{
//		const TCHAR* szName = pkChild->Value();
//		const TCHAR* szValue = pkChild->GetText();
//		if(0 == strcmp(szName, "EXP"))
//		{
//		}
//		else if(0 == strcmp(szName, "GOLD"))
//		{
//		}
//		else if(0 == strcmp(szName, "CHANGECLASS"))
//		{
//			const TCHAR* szPrevClass = pkChild->Attribute("PREVCLASS");
//			const TCHAR* szMinLevel = pkChild->Attribute("MINLEVEL");
//			if(NULL == szPrevClass || NULL == szMinLevel)
//			{
//				printf_s("전직 퀘스트 인 ID:%s 퀘스트의 이전 직업번호, 또는 전직가능 레벨이 없습니다.\n", m_kQuestID.c_str());
//				printf_s("도움!! <%s PREVCLASS=\"이전직업번호\" MINLEVEL=\"전직가능레벨\"></%s>\n", szName, szName);
//				assert(0); Exit(0); return false;
//			}
//
//			m_kChangeClass = pkChild->GetText();//전직할 번호
//			m_kMinLevel = szMinLevel;
//			m_kPrevClass = szPrevClass;
//		}
//		else if(0 == strcmp(szName, "SELECT1"))
//		{
//			if( pkChild->Attribute("COUNT") )
//			{
//				m_iItemCount1 = _ttoi(pkChild->Attribute("COUNT"));
//			}
//
//			TiXmlElement *pkEle = pkChild->FirstChildElement("ITEM");
//			while(pkEle)
//			{
//				const TCHAR* szCount = pkEle->Attribute("COUNT");
//				const TCHAR* szItemNo = pkEle->GetText();
//
//				if(NULL != szCount && NULL != szItemNo)
//				{
//					CheckItem(_ttoi(szItemNo), pkEle);
//					m_kItemVec1.push_back(std::make_pair(szItemNo, szCount));
//				}
//				else
//				{
//					PrintWaring(pkEle);
//				}
//				pkEle = pkEle->NextSiblingElement("ITEM");
//			}
//		}
//		else if(0 == strcmp(szName, "SELECT2"))
//		{
//			if( pkChild->Attribute("COUNT") )
//			{
//				m_iItemCount2 = _ttoi(pkChild->Attribute("COUNT"));
//			}
//
//			TiXmlElement *pkEle = pkChild->FirstChildElement("ITEM");
//			while(pkEle)
//			{
//				const TCHAR* szCount = pkEle->Attribute("COUNT");
//				const TCHAR* szItemNo = pkEle->GetText();
//
//				if(NULL != szCount && NULL != szItemNo)
//				{
//					CheckItem(_ttoi(szItemNo), pkEle);
//					m_kItemVec2.push_back(std::make_pair(szItemNo, szCount));
//				}
//				else
//				{
//					PrintWaring(pkEle);
//				}
//				pkEle = pkEle->NextSiblingElement("ITEM");
//			}
//		}
//		else
//		{
//			PrintWaring(pkChild);
//		}
//		pkChild = pkChild->NextSiblingElement();
//	}
//	return true;
//}

//>>local function
//bool PgXml2Lua::MakeDependElement(TiXmlElement *pkChild, SArgument &rkArgument)
//{
//	bool bRet = true;
//	const TCHAR* szName = pkChild->Value();
//	const TCHAR* szValue = pkChild->GetText();
//
//	const TCHAR* szObjectNo = pkChild->Attribute("OBJECTNO");
//	const TCHAR* szType = pkChild->Attribute("TYPE");
//	const TCHAR* szVal = pkChild->Attribute("VALUE");
//
//	char szTemp[255] = {0, };
//	if(NULL != szObjectNo)
//	{
//		int iVal = _ttoi(szObjectNo);
//		//if(0 > iVal) iVal = 0;
//		if(0 > iVal && 4 < iVal) bRet = false;
//		_itoa_s(iVal, szTemp, 255, 10);
//	}
//	if(NULL == szType) bRet = false;
//
//	if(bRet)
//	{
//		rkArgument.Set(szValue, szObjectNo, szType, szVal);
//	}
//	else
//	{
//		PrintWaring(pkChild);
//	}
//	return bRet;
//}
//<<local function

//bool PgXml2Lua::ParseDependents(TiXmlElement *pkNode)
//{
//	TiXmlElement *pkChild = pkNode->FirstChildElement();
//	while(pkChild)
//	{
//		SArgument kArgument;
//		const TCHAR* szName = pkChild->Value();
//		const TCHAR* szValue = pkChild->GetText();
//		if( 0 == strcmp(szName, "NPC") )
//		{
//			const bool bRet = MakeDependElement(pkChild, kArgument);
//			if( bRet )
//			{
//				m_kDependNpcVec.push_back(kArgument);
//				printf_s("NPC -> %s:\t%s\n", kArgument.kStr1.c_str(), pkChild->Attribute("MEMO"));
//			}
//		}
//		else if( 0 == strcmp(szName, "MONSTER") )
//		{
//			const bool bRet = MakeDependElement(pkChild, kArgument)
//			if( bRet )
//			{
//				CheckMonster(_ttoi(kArgument.kID.c_str()), pkChild);
//
//				m_kDependMonVec.push_back(kArgument);
//				printf_s("MONSTER -> %s:\t%s\n", kArgument.kStr1.c_str(), pkChild->Attribute("MEMO"));
//			}
//		}
//		else if( 0 == strcmp(szName, "ITEM") )
//		{
//			const bool bRet = MakeDependElement(pkChild, kArgument);
//			if( bRet )
//			{
//				CheckItem(_ttoi(kArgument.kID.c_str()), pkChild);
//
//				//중복검사
//				for(size_t i=0; m_kDependItemVec.size() > i; ++i)
//				{
//					if(m_kDependItemVec[i].kID == kArgument.kID)
//					{
//						printf_s("XML: %s, Row:%d, Col:%d, 중복되는 ItemNo[%s]\n", m_kInputName.c_str(), pkChild->Row(), pkChild->Column(), kArgument.kID.c_str());
//						assert(0); Exit(0); return false;
//					}
//				}
//				m_kDependItemVec.push_back(kArgument);
//				printf_s("NPC -> %s:\t%s\n", kArgument.kStr1.c_str(), pkChild->Attribute("MEMO"));
//			}
//		}
//		else if( 0 == strcmp(szName, "LOCATION") )
//		{
//			const bool bRet = MakeDependElement(pkChild, kArgument)
//			if( bRet )
//			{
//				m_kDependLocation.push_back(kArgument);
//				printf_s("LOCATION -> %s:\t%s\n", kArgument.kStr2.c_str(), pkChild->Attribute("MEMO"));
//			}
//		}
//		else if( 0 == strcmp(szName, "MISSION") )
//		{
//			const bool bRet = MakeDependElement(pkChild, kArgument)
//			if( bRet )
//			{
//				
//			}
//		}
//		else
//		{
//			printf_s("XML: %s, Row:%d, Col:%d, [%s] 잘못된 이름\n", m_kInputName.c_str(), pkChild->Row(), pkChild->Column(), szName);
//			assert(0); Exit(0); return false;
//		}
//		pkChild = pkChild->NextSiblingElement();
//	}
//	return true;
//}

//bool PgXml2Lua::Parseobjects(TiXmlElement *pkNode)
//{
//	TiXmlElement *pkChild = pkNode->FirstChildElement();
//	while(pkChild)
//	{
//		SArgument kArgument;
//		const TCHAR* szName = pkChild->Value();
//		const TCHAR* szValue = pkChild->GetText();
//		const TCHAR* szCount = pkChild->Attribute("COUNT");
//		if(szCount == NULL)
//		{
//			printf_s("Object Count가 없습니다. <OBJECTn COUNT=''></OBJECTn>");
//			PrintWaring(pkChild);
//			assert(0); Exit(0); return false;
//		}
//
//		if(0 == strcmp(szName, "OBJECT1"))
//		{
//			m_iObject[0] = _ttoi(szCount);
//		}
//		else if(0 == strcmp(szName, "OBJECT2"))
//		{
//			m_iObject[1] = _ttoi(szCount);
//		}
//		else if(0 == strcmp(szName, "OBJECT3"))
//		{
//			m_iObject[2] = _ttoi(szCount);
//		}
//		else if(0 == strcmp(szName, "OBJECT4"))
//		{
//			m_iObject[3] = _ttoi(szCount);
//		}
//		else if(0 == strcmp(szName, "OBJECT5"))
//		{
//			m_iObject[4] = _ttoi(szCount);
//		}
//		else
//		{
//			printf_s("XML: %s, Row:%d, Col:%d, [%s] 잘못된 이름\n", m_kInputName.c_str(), pkChild->Row(), pkChild->Column(), szName);
//			assert(0); Exit(0); return false;
//		}
//		pkChild = pkChild->NextSiblingElement();
//	}
//	return true;
//}

bool PgXml2Lua::OpenXml(const std::string &rkName)
{
	OpenXml();
	return true;
}
bool PgXml2Lua::OpenXml(const TCHAR* szFileName)
{
	OpenXml();
	return true;
}

bool PgXml2Lua::PrintOutLua()
{
	//Quest00000000.xml
	//Quest00000000.lua
	//퀘스트 ID로 xml 복사본과 lua 파일을 만든다.
	char szXmlName[_MAX_PATH] = {0, };
	char szLuaName[_MAX_PATH] = {0, };

	const int iQuestID = ID();
	sprintf_s(szXmlName, _MAX_PATH, "./XML/Quest/Quest%08d.xml", iQuestID);
	sprintf_s(szLuaName, _MAX_PATH, "./SCRIPT/Quest/Quest%08d.lua", iQuestID);
	
	//Xml 복사
	{
		printf_s("Copy %s to %s ... \n", m_kInputName.c_str(), szXmlName);
		TiXmlDocument kDocu(m_kInputName.c_str());
		kDocu.LoadFile();
		kDocu.SaveFile(szXmlName);
	}

	//Lua
	FILE *pFile = NULL;
	errno_t eRet = 0;
	//eRet = fopen_s(&pFile, m_kOutputName.c_str(), "w");
	eRet = fopen_s(&pFile, szLuaName, "w");
	if(0 != eRet || NULL == pFile)
	{
		return false;
	}

	//LUA Make Start
	bool bRet = true;
	bRet = PrintOutLua_Head(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Init(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Monster(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_NPC(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Item(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Location(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Mission(pFile);
	if(!bRet) return false;

	bRet = PrintOutLua_Pvp(pFile);
	if(!bRet) return false;

	fclose(pFile);

	return true;
}

bool PgXml2Lua::PrintOutLua_Head(FILE *pFile)
{
	FILETIME kLastWriteTime, kLocalFTime;
	HANDLE hFile = CreateFileA(m_kInputName.c_str(), FILE_READ_EA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileTime(hFile, NULL, NULL, &kLastWriteTime);
	CloseHandle(hFile);

	SYSTEMTIME kTime;
	//GetLocalTime(&kTime);
	FileTimeToLocalFileTime(&kLastWriteTime, &kLocalFTime);
	FileTimeToSystemTime(&kLocalFTime, &kTime);
	_ftprintf_s(pFile, _T("-----------------------------------------------------------------------------\n"));
	_ftprintf_s(pFile, _T("-- Original						: ProjectG/Tools/QuestX2L\n"));
	_ftprintf_s(pFile, _T("-- Xml2Lua Tool Made	: 2007.05.24\n"));
	_ftprintf_s(pFile, _T("-- Last Edited				: 2008.02.01\n"));
	_ftprintf_s(pFile, _T("-- From XML File			: %s\n"), UNI(m_kInputName.c_str()));
	_ftprintf_s(pFile, _T("-- XML File Time Stamp	: %04d.%02d.%02d %02d:%02d:%02d\n"), kTime.wYear, kTime.wMonth, kTime.wDay, kTime.wHour, kTime.wMinute, kTime.wSecond);
	_ftprintf_s(pFile, _T("-----------------------------------------------------------------------------\n"));


	_ftprintf_s(pFile, LUA_INIT, ID());
	_ftprintf_s(pFile, LUA_STARTER);

	return true;
}

bool PgXml2Lua::Print_GiveReward(FILE* pFile)
{
	//Reward
	_ftprintf_s(pFile, _T("\t\t\t\t--Give Reward\n"));
	_ftprintf_s(pFile, _T("\t\t\t\tkGround:GiveReward(__TEMP__, who, id2, id3, iNextDialog)\n"));
	_ftprintf_s(pFile, _T("\t\t\t\tiNextDialog = 0--Process cut\n"));//잠시 프로세스를 끊는다.

	//if( QT_ClassChange == m_kBasic.eType )//Can Transform Class
	//{//Transform!!
	//	const SChangeClassInfo& kChangeClss = m_kReward.kChangeClass;
	//	_ftprintf_s(pFile, LUA_INDIALOG_BODY_REWARD_2, kChangeClss.iMinLevel, kChangeClss.iPrevClassNo, kChangeClss.iChangeClassNo);//전직할 ClassNo
	//}

	_ftprintf_s(pFile, LUA_INDIALOG_BODY_REWARD_END);

	return true;
}

void PgXml2Lua::PrintOutLua_MonsterGiveItemDelete(FILE* pFile, ContItemSet& rkItemSet)
{
	ContQuestMonster::iterator monster_iter = m_kDepend_Monster.begin();
	while(m_kDepend_Monster.end() != monster_iter)
	{
		const ContQuestMonster::value_type &rkElement = (*monster_iter);
		if(TYPE_INSTDROP == rkElement.kType)
		{
			ContWStrVec kVec;
			BreakSep(rkElement.kValue, kVec, _T("/"));//여기까지 왔으면 갯수에 문제가 없다.

			const int iItemNo = _ttoi(kVec[4].c_str());			
			const int iCount = m_kObject.GetEndCount(rkElement.iObjectNo);

			SItem kNewItem;
			kNewItem.ItemNo(iItemNo);
			kNewItem.Count(iCount);

			ContItemSet::iterator set_iter = rkItemSet.find(kNewItem);
			if( rkItemSet.end() == set_iter )//중복 방지
			{
				_ftprintf_s(pFile, _T("\t\t") _T("kGround:DeletePlayerItem(who, %d, %d)\n"), iItemNo, iCount);

				rkItemSet.insert( kNewItem );
			}
		}
		++monster_iter;
	}
}

void PgXml2Lua::PrintOutLua_NPCGiveItemDelete(FILE* pFile, ContItemSet& rkItemSet)
{
	ContQuestDenpendNpc::iterator event_iter = m_kDepend_Npc.begin();
	while(m_kDepend_Npc.end() != event_iter)
	{
		const ContQuestDenpendNpc::value_type& rkEvent = (*event_iter);
		if( TYPE_GIVEITEM == rkEvent.kType )
		{
			ContWStrVec kVec;
			BreakSep(rkEvent.kValue, kVec, _T("/"));
			
			const TCHAR* szDialog = kVec[0].c_str();
			const TCHAR* szItemNo = kVec[1].c_str();
			const TCHAR* szCount = kVec[2].c_str();

			SItem kNewItem;
			kNewItem.ItemNo(_ttoi(szItemNo));
			kNewItem.Count(_ttoi(szCount));

			ContItemSet::iterator set_iter = rkItemSet.find(kNewItem);
			if( rkItemSet.end() == set_iter )//중복 방지
			{
				_ftprintf_s(pFile, L"		kGround:DeletePlayerItem(who, %s, %s)\n", szItemNo, szCount);
				rkItemSet.insert( kNewItem );
			}
		}
		++event_iter;
	}
}

void PgXml2Lua::PrintOutLua_DependItemDelete(FILE* pFile, ContItemSet& rkItemSet)
{
	ContQuestDependItem::iterator item_iter = m_kDepend_Item.begin();
	while(m_kDepend_Item.end() != item_iter)
	{
		const ContQuestDependItem::value_type& rkItem = (*item_iter);
		if( TYPE_CHANGE_COUNT == rkItem.kType )
		{
			const int iCount = GetParamEndCondtion(rkItem.iObjectNo);

			SItem kNewItem;
			kNewItem.ItemNo(rkItem.iItemNo);
			kNewItem.Count(iCount);

			ContItemSet::iterator set_iter = rkItemSet.find(kNewItem);
			if( rkItemSet.end() == set_iter )//중복 방지
			{
				_ftprintf_s(pFile, L"		kGround:DeletePlayerItem(who, %d, %d)\n", rkItem.iItemNo, iCount);
				rkItemSet.insert( kNewItem );
			}
		}
		++item_iter;
	}
}

bool PgXml2Lua::PrintOutLua_Init(FILE *pFile)
{
	//size_t i = 0, j = 0;
	//Default Function Start
	_ftprintf_s(pFile, LUA_HEADER);
	{
		_ftprintf_s(pFile, LUA_ADD_STATE_STRING, _T("LuaFile"), UNI(m_kOutputName.c_str()));

		_ftprintf_s(pFile, LUA_ACCEPT_HEADER);
		_ftprintf_s(pFile, LUA_ACCEPT_FOOTER);
		_ftprintf_s(pFile, LUA_DROP_HEADER);
		ContItemSet kDeleteItemSet;
		PrintOutLua_NPCGiveItemDelete(pFile, kDeleteItemSet);
		_ftprintf_s(pFile, LUA_DROP_FOOTER);

		PrintOutLua_OnComplete(pFile);

		_ftprintf_s(pFile, LUA_ONDIALOG_HEADER);
			_ftprintf_s(pFile, LUA_ONDIALOG_HEAD_DEFAULT);

			ArrayFunc(ContQuestDenpendNpc, m_kDepend_Npc, NpcPreEvent);//Pre

			_ftprintf_s(pFile, LUA_ONDIALOG_BODY_DEFAULT_1);
			if(!Print_GiveReward(pFile))	return false;
			_ftprintf_s(pFile, LUA_ONDIALOG_BODY_DEFAULT_2);

			//End Pre Show Dialog
			_ftprintf_s(pFile, LUA_ONDIALOG_FOOT_DEFAULT); //Show Quest Dialog
			//Start OnShowDialog
		_ftprintf_s(pFile, LUA_ONDIALOG_FOOTER);

		_ftprintf_s(pFile, LUA_ONDIALOG_AFTER_HEADER);

			ArrayFunc(ContQuestDenpendNpc, m_kDepend_Npc, NpcAfterEvent);//After

		_ftprintf_s(pFile, LUA_ONDIALOG_AFTER_FOOTER);
	}
	_ftprintf_s(pFile, LUA_FOOTER);
	return true;
}

bool MonsterSort(const ContQuestMonster::value_type& rkLeft, const ContQuestMonster::value_type& rkRight)
{
	//<
	return rkLeft.iClassNo < rkRight.iClassNo;
}

bool PgXml2Lua::PrintOutLua_Monster(FILE *pFile)
{
	if( m_kDepend_Monster.empty() )
	{
		return true;	
	}

	//Monster
	_ftprintf_s(pFile, LUA_MONSTER_STARTER);

	typedef std::list< ContQuestMonster::value_type > ContMonsterList;

	ContMonsterList kMonsterList(m_kDepend_Monster.begin(), m_kDepend_Monster.end());
	kMonsterList.sort(MonsterSort);

	int iCurClassNo = 0;

	ContMonsterList::iterator iter = kMonsterList.begin();
	for(; kMonsterList.end() != iter; ++iter)
	{
		const ContQuestMonster::value_type& rkElement = (*iter);

		if( kMonsterList.begin() != iter
		&&	rkElement.iClassNo != iCurClassNo )
		{
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_END);
			_ftprintf_s(pFile, LUA_MONSTER_FOOTER);
		}
		if( kMonsterList.begin() == iter
		||	rkElement.iClassNo != iCurClassNo )
		{
			_ftprintf_s(pFile, LUA_MONSTER_HEADER, rkElement.iClassNo);
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_START);

			iCurClassNo = rkElement.iClassNo;
		}

		const TCHAR* szType = rkElement.kType.c_str();
		ContWStrVec kVec;
		BreakSep(rkElement.kValue, kVec, _T("/"));
		if(TYPE_INCPARAM == szType)
		{
			if( 0 > rkElement.iObjectNo || 4 < rkElement.iObjectNo )
			{
				printf_s("MONSTER 오브젝트 번호 오류 ... TYPE=\"%s\" OBJECTNO=\"%d\" \n", MB(szType), rkElement.iObjectNo+1);
				printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"Parameter가 증가할 수\" \n");
				assert(0); Exit(0); return false;
			}
			//Object No, Inv Value
			const TCHAR* szIncVal = kVec[0].c_str();
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_INCMON, rkElement.iObjectNo, szIncVal);
		}
		else if( TYPE_INCPARAM_IN == szType )
		{
			if( 2 != kVec.size() )
			{
				printf_s("VALUE 오류 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
				printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"진행할 그라운드 번호/Parameter가 증가할 수\" \n");
				assert(0); Exit(0); return false;
			}
			//Object No, GroundNo, Inc Value
			const TCHAR* szGroundNo = kVec[0].c_str();
			const TCHAR* szIncVal = kVec[1].c_str();
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_INCMON_IN, rkElement.iObjectNo, szGroundNo, szIncVal);
		}
		else if( TYPE_INSTDROP == szType )
		{
			if( 5 != kVec.size() )
			{
				printf_s("인자 수 오류!! 인자는 4개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
				printf_s("VALUE=\"최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				assert(0); Exit(0); return false;
			}
			const int iMaxPercent = _ttoi(kVec[0].c_str());
			const int iCutLine = _ttoi(kVec[1].c_str());
			const int iItemMax = _ttoi(kVec[2].c_str());
			const int iItemMin = _ttoi(kVec[3].c_str());
			const int iItemNo = _ttoi(kVec[4].c_str());
			if( 0 > rkElement.iObjectNo || 4 < rkElement.iObjectNo )
			{
				printf_s("오브젝트 번호 오류 ... TYPE=\"%s\" OBJECTNO=\"%d\" \n", MB(szType), rkElement.iObjectNo+1);
				printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				assert(0); Exit(0); return false;
			}
			if( 0 > iMaxPercent || iMaxPercent < iCutLine || iItemMax < iItemMin )
			{
				printf_s("VALUE=\"최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				printf_s("VALUE=\"%d/%d/%d/%d/%d\"\n", iMaxPercent, iCutLine, iItemMax, iItemMin, iItemNo);
				assert(0); Exit(0); return false;
			}
			//Param No, Max Percent, Cut Line, Item Max, Item Min, ItemNo
			const TCHAR* szMaxPercent = kVec[0].c_str();
			const TCHAR* szCutLine = kVec[1].c_str();
			const TCHAR* szMaxItem = kVec[2].c_str();
			const TCHAR* szMinItem = kVec[3].c_str();
			const TCHAR* szItemNo = kVec[4].c_str();
			CheckItem(iItemNo, NULL);
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_INSTDROP, rkElement.iObjectNo, szMaxPercent, szCutLine, szMaxItem, szMinItem, szItemNo);
		}
		else if( TYPE_INSTDROP_IN == szType )
		{
			if( 6 != kVec.size() )
			{
				printf_s("인자 수 오류!! 인자는 4개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), (rkElement.kValue.c_str()));
				printf_s("VALUE=\"그라운드번호/최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				assert(0); Exit(0); return false;
			}
			const int iGroundNo = _ttoi(kVec[0].c_str());
			const int iMaxPercent = _ttoi(kVec[1].c_str());
			const int iCutLine = _ttoi(kVec[2].c_str());
			const int iItemMax = _ttoi(kVec[3].c_str());
			const int iItemMin = _ttoi(kVec[4].c_str());
			const int iItemNo = _ttoi(kVec[5].c_str());
			if( 0 > rkElement.iObjectNo || 4 < rkElement.iObjectNo )
			{
				printf_s("오브젝트 번호 오류 ... TYPE=\"%s\" OBJECTNO=\"%d\" \n", MB(szType), rkElement.iObjectNo+1);
				printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"그라운드번호/최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				assert(0); Exit(0); return false;
			}
			if( 0 > iMaxPercent || iMaxPercent < iCutLine || iItemMax < iItemMin )
			{
				printf_s("VALUE=\"그라운드번호/최대확율/커트라인/최대아이템수량/최소아이템수량/지급할 아이템번호\" \n");
				printf_s("VALUE=\"%d/%d/%d/%d/%d\"\n", iMaxPercent, iCutLine, iItemMax, iItemMin, iItemNo);
				assert(0); Exit(0); return false;
			}
			//Param No, GroundNo, Max Percent, Cut Line, Item Max, Item Min, ItemNo
			const TCHAR* szGroundNo = kVec[0].c_str();
			const TCHAR* szMaxPercent = kVec[1].c_str();
			const TCHAR* szCutLine = kVec[2].c_str();
			const TCHAR* szMaxItem = kVec[3].c_str();
			const TCHAR* szMinItem = kVec[4].c_str();
			const TCHAR* szItemNo = kVec[5].c_str();
			CheckItem(iItemNo, NULL);
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_INSTDROP_IN, rkElement.iObjectNo, szGroundNo, szMaxPercent, szCutLine, szMaxItem, szMinItem, szItemNo);
		}
		else if(TYPE_COMBOCHECK == szType )
		{
			if( 2 != kVec.size() )
			{
				printf_s("인자 수 오류!! 인자는 2개 ... TYPE=\"%s\" VALUE=\"%s\" \n", MB(szType), MB(rkElement.kValue.c_str()));
				printf_s("VALUE=\"목표콤보/증가값\" \n");
				assert(0); Exit(0); return false;
			}
			//const int iObjectNo = _ttoi(m_kDependMonVec[i].kObjectNo);
			const int iTrgComboCount = _ttoi(kVec[0].c_str());
			const int iIncVal = _ttoi(kVec[1].c_str());
			if( 0 > rkElement.iObjectNo || 4 < rkElement.iObjectNo )
			{
				printf_s("오브젝트 번호 오류 ... TYPE=\"%s\" OBJECTNO=\"%d\" \n", MB(szType), rkElement.iObjectNo+1);
				printf_s("OBJECTNO=\"0이 아닌 1~5\" VALUE=\"목표콤보/증가값\" \n");
				assert(0); Exit(0); return false;
			}
			//CheckCombo Count, ObjectNo, Inc Value
			//const TCHAR* szObjectNo = m_kDependMonVec[i].kObjectNo.c_str();
			const TCHAR* szTrgComboCount = kVec[0].c_str();
			const TCHAR* szIncValue = kVec[1].c_str();
			_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_COMBOCHECK, szTrgComboCount, rkElement.iObjectNo, szIncValue);
		}
		else
		{
			printf_s("잘못된 이름 TYPE=\"%s\" \n", MB(szType));
			assert(0); Exit(0); return false;
		}
	}
	_ftprintf_s(pFile, LUA_MONSTER_QUEST_ONDIE_END);
	_ftprintf_s(pFile, LUA_MONSTER_FOOTER);
	return true;
}

bool SortDependNpc(const ContQuestDenpendNpc::value_type& rkLeft, const ContQuestDenpendNpc::value_type& rkRight)
{
	return rkLeft.iEventNo < rkRight.iEventNo;
}

bool PgXml2Lua::PrintOutLua_NPC(FILE *pFile)
{
	if( m_kDepend_Npc.empty() )
	{
		return true;
	}

	//size_t i = 0, j = 0;

	//NPC
	_ftprintf_s(pFile, LUA_NPC_STATER);

	typedef std::list< ContQuestDenpendNpc::value_type > ContDependNpcList;

	ContDependNpcList kList(m_kDepend_Npc.begin(), m_kDepend_Npc.end());
	kList.sort(SortDependNpc);

	int iCurEventNo = 0;
	ContDependNpcList::iterator iter = kList.begin();
	for(; kList.end() != iter; ++iter)
	{
		const ContDependNpcList::value_type& rkElement = (*iter);

		if( kList.begin() != iter
		&&	iCurEventNo != rkElement.iEventNo )
		{
			_ftprintf_s(pFile, LUA_NPC_ONTALK_END);
			_ftprintf_s(pFile, LUA_NPC_FOOTER);
		}
		if( kList.begin() == iter
		||	rkElement.iEventNo != iCurEventNo )
		{
			_ftprintf_s(pFile, LUA_NPC_HEADER, rkElement.iEventNo);
			_ftprintf_s(pFile, LUA_NPC_ONTALK_START);
			iCurEventNo = rkElement.iEventNo;
		}
		
		const TCHAR* szType = rkElement.kType.c_str();
		ContWStrVec kVec;
		BreakSep(rkElement.kValue, kVec, _T("/"));

		if(TYPE_CLIENT == szType)
		{
			_ftprintf_s(pFile, LUA_NPC_QUEST_CLIENT, kVec[0].c_str());
		}
		else if(TYPE_PAYER == szType)
		{
			_ftprintf_s(pFile, LUA_NPC_QUEST_PAYER, kVec[0].c_str());
		}
		else if(TYPE_ING_DLG == szType || TYPE_ING_CHAT == szType)
		{
			int iType = 0;
			if(TYPE_ING_DLG == szType)	iType = 1;//DLG
			if(TYPE_ING_CHAT == szType)	iType = 2;//CHAT

			const int iLimitObjectNo = 0;
			_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_START);
			if( iLimitObjectNo < rkElement.iObjectNo+1 )
			{//해당 파라메터를 종료하지 않았을 때 뜬다
				_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_PARAM_START, rkElement.iObjectNo);
			}
			//else//항상 뜨는 Dialog

			_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_HEAD);
			if(1 == kVec.size())
			{
				//if(1==iType)	fprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_SINGLE, kVec[0].c_str());
				//if(2==iType)	fprintf_s(pFile, LUA_NPC_QUEST_ING_CHAT_SINGLE, kVec[0].c_str());
				const TCHAR* szNextDialogID = kVec[0].c_str();
				_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_SINGLE, szNextDialogID);
			}
			else
			{
				_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_RND_HEAD);
				for(size_t iA = 0; kVec.size() > iA; ++iA)
				{
					const TCHAR* szRandomNextID = kVec[iA].c_str();
					_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_RND_ITEM, iA+1, szRandomNextID);
				}
				_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_RND_RAND, kVec.size()+1);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			{//다이얼로그가 뜨기 전에 처리 할 부분
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////

			if( 1 == iType )	_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_RND);
			if( 2 == iType )	_ftprintf_s(pFile, LUA_NPC_QUEST_ING_CHAT_RND);

			_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_PARAM_END_1);
			if( iLimitObjectNo < rkElement.iObjectNo+1 )
			{//해당 파라메터를 종료하지 않았을 때 뜬다
				_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_PARAM_END_2);
			}
			_ftprintf_s(pFile, LUA_NPC_QUEST_ING_DLG_END);
		}
		else
		{
			printf_s("잘못된 NPC TYPE=\"%s\" KID=\"%d\"\n", MB(szType), rkElement.iEventNo);
			assert(0); Exit(0); return false;
		}
	}
	_ftprintf_s(pFile, LUA_NPC_ONTALK_END);
	_ftprintf_s(pFile, LUA_NPC_FOOTER);
	return true;
}

bool PgXml2Lua::PrintOutLua_Item(FILE *pFile)
{
	ContQuestDependItem::iterator iter = m_kDepend_Item.begin();
	while(m_kDepend_Item.end() != iter)
	{
		const ContQuestDependItem::value_type& rkElement = (*iter);
		const TCHAR* szType = rkElement.kType.c_str();
		if( TYPE_CHANGE_COUNT == szType )//단순 오류 체크
		{
		}
		else if( TYPE_DURABILITY_CHECK == szType )
		{
		}
		else if( TYPE_PLUS_CHECK == szType )
		{
		}
		else
		{
			printf_s("잘못된 ITEM TYPE=\"%s\" ItemNo=\"%d\"\n", MB(szType), rkElement.iItemNo);
			assert(0); Exit(0); return false;
		}

		++iter;
	}

	//size_t i = 0, j = 0;
	////ITEM
	//fprintf_s(pFile, LUA_ITEM_TOPARAM_HEADER);
	//for(i=0; m_kDependItemVec.size() > i; ++i)
	//{
	//	SArgument &rkCur = m_kDependItemVec[i];

	//	if(m_kDependItemVec[i].kObjectNo == "0")
	//	{
	//		printf_s("ITEM Object No는 0이 될 수 없습니다. ObjectNo=\"%s\" ItemNo=\"%s\"\n", rkCur.kObjectNo.c_str(), m_kDependNpcVec[j].kID.c_str());
	//		assert(0); Exit(0); return false;
	//	}

	//	fprintf_s(pFile, LUA_ITEM_TOPARAM, rkCur.kID.c_str(), rkCur.kObjectNo.c_str());
	//}

	////ITEM에 대해서 TYPE_INCPARAM 만
	//fprintf_s(pFile, LUA_ITEM_STATER);
	//for(i=0; m_kDependItemVec.size() > i; ++i)
	//{
	//	SArgument &rkCur = m_kDependItemVec[i];
	//	if(rkCur.bUsed) continue;
	//	if(TYPE_CHANGE_COUNT != rkCur.kStr1) continue;

	//	//SArgument &rkSrc = m_kDependItemVec[i];
	//	std::string kSrcObjectNo = rkCur.kObjectNo;

	//	fprintf_s(pFile, LUA_ITEM_HEADER, kSrcObjectNo.c_str());
	//	fprintf_s(pFile, LUA_ITEM_ONCHANGE_START);
	//	for(j=0; m_kDependItemVec.size() > j; ++j)
	//	{
	//		SArgument &rkArgument = m_kDependItemVec[j];
	//		if(rkArgument.bUsed) continue;

	//		if(kSrcObjectNo == rkArgument.kObjectNo)
	//		{
	//			fprintf_s(pFile, LUA_ITEM_ONCHANGE_BODY, rkArgument.kID.c_str());
	//			rkArgument.bUsed = true;
	//		}
	//	}
	//	fprintf_s(pFile, LUA_ITEM_ONCHANGE_END, kSrcObjectNo.c_str());
	//	fprintf_s(pFile, LUA_ITEM_FOOTER, kSrcObjectNo.c_str());
	//}
	///*for(i=0; i<m_kDependNpcVec.size(); ++i)
	//{
	//	if(m_kDependNpcVec[i].bUsed) continue;
	//	fprintf_s(pFile, LUA_NPC_ONTALK_END);
	//	fprintf_s(pFile, LUA_NPC_FOOTER);
	//}*/
	return true;
}

bool SortLocation(const ContQuestLocation::value_type& rkLeft, const ContQuestLocation::value_type& rkRight)
{
	return rkLeft.iLocationNo < rkRight.iLocationNo;
}

bool PgXml2Lua::PrintOutLua_Location(FILE *pFile)
{
	if( m_kDepend_Location.empty() )
	{
		return true;
	}

	//LOCATION
	_ftprintf_s(pFile, LUA_LOCATION_STARTER);

	typedef std::list< ContQuestLocation::value_type > ContLocationList;
	ContLocationList kList(m_kDepend_Location.begin(), m_kDepend_Location.end());
	kList.sort(SortLocation);

	int iCurGroundNo = 0;
	ContLocationList::iterator iter = kList.begin();
	for(; kList.end() != iter; ++iter)
	{
		const ContLocationList::value_type& rkElement = (*iter);

		const int iGroundNo = _ttoi(rkElement.kValue.c_str());

		if( kList.begin() != iter
		&&	iGroundNo != iCurGroundNo )
		{
			_ftprintf_s(pFile, LUA_LOCATION_ENTER_FOOT);
			_ftprintf_s(pFile, LUA_LOCATION_FOOTER);
		}
		if( kList.begin() == iter
		||	iGroundNo != iCurGroundNo )
		{
			_ftprintf_s(pFile, LUA_LOCATION_HEADER, iGroundNo);//한 그라운드당
			_ftprintf_s(pFile, LUA_LOCATION_ENTER_HEAD);
		}

		const TCHAR* szType = rkElement.kType.c_str();
		if(TYPE_LOCATION_ENTER == szType)
		{
			_ftprintf_s(pFile, LUA_LOCATION_ENTER_BODY, rkElement.iLocationNo, rkElement.iObjectNo);
		}
		else
		{
			printf_s("잘못된 LOCATION TYPE=\"%s\" \n", MB(szType));
			assert(0); Exit(0); return false;
		}
	}

	_ftprintf_s(pFile, LUA_LOCATION_ENTER_FOOT);
	_ftprintf_s(pFile, LUA_LOCATION_FOOTER);
	return true;
}

bool SortMission(const ContQuestDependMission::value_type& rkLeft, const ContQuestDependMission::value_type& rkRight)
{
	return rkLeft.iMissionKey < rkRight.iMissionKey;
}

bool PgXml2Lua::PrintOutLua_Mission(FILE* pFile)
{
	if( m_kDepend_Mission.empty() )
	{
		return true;
	}

	//MISSION
	_ftprintf_s(pFile, LUA_MISSION_STARTER);

	typedef std::list< ContQuestDependMission::value_type > ContDependMissionList;

	ContDependMissionList kList(m_kDepend_Mission.begin(), m_kDepend_Mission.end());
	kList.sort(SortMission);

	//분류
	ContQuestDependMission kRankReserveVec;
	ContQuestDependMission kPercentReserveVec;
	ContDependMissionList::iterator iter = kList.begin();
	while(kList.end() != iter)
	{
		const ContDependMissionList::value_type& rkElement = (*iter);

		ContWStrVec kVec;
		BreakSep(rkElement.kValue, kVec, _T("/"));

		const TCHAR* szType = rkElement.kType.c_str();
		if( TYPE_PERCENT == rkElement.kType )
		{
			if( 1 != kVec.size() )
			{
				assert(0); Exit(0); return false;
			}

			kPercentReserveVec.push_back(rkElement);
		}
		else if( TYPE_RANK == rkElement.kType )
		{
			if( 2 != kVec.size() )
			{
				assert(0); Exit(0); return false;
			}

			kRankReserveVec.push_back(rkElement);//임시로 저장
		}
		else
		{
			printf_s("잘못된 MISSION TYPE=\"%s\" \n", MB(szType));
			assert(0); Exit(0); return false;
		}

		++iter;
	}

	/////////////////////////
	int iCurMissoinKey = 0;
	ContQuestDependMission::iterator percent_iter = kPercentReserveVec.begin();
	while(kPercentReserveVec.end() != percent_iter)
	{
		const ContQuestDependMission::value_type& rkElement = (*percent_iter);

		ContWStrVec kVec;
		BreakSep(rkElement.kValue, kVec, _T("/"));

		if( kPercentReserveVec.begin() != percent_iter
		&&	rkElement.iMissionKey != iCurMissoinKey )
		{
			_ftprintf_s(pFile, LUA_MISSION_FOOTER);
		}
		if( kPercentReserveVec.begin() == percent_iter
		||	rkElement.iMissionKey != iCurMissoinKey )
		{
			_ftprintf_s(pFile, LUA_MISSION_HEADER, rkElement.iMissionKey);
			iCurMissoinKey = rkElement.iMissionKey;
		}

		const TCHAR* szTargetPercent = kVec[0].c_str();
		_ftprintf_s(pFile, LUA_MISSION_PERCENT, rkElement.iObjectNo, szTargetPercent);

		++percent_iter;
	}
	if( kPercentReserveVec.size() )
	{
		_ftprintf_s(pFile, LUA_MISSION_FOOTER);
	}

	/////////////////////////
	iCurMissoinKey = 0;
	int iCurMissionLevel = 0;
	ContQuestDependMission::iterator temp_iter = kRankReserveVec.begin();
	while(kRankReserveVec.end() != temp_iter)
	{
		const ContQuestDependMission::value_type& rkRankEvent = (*temp_iter);

		ContWStrVec kVec;
		BreakSep(rkRankEvent.kValue, kVec, _T("/"));
		const TCHAR* szMissionLevel = kVec[0].c_str();
		const TCHAR* szTargetRank = kVec[1].c_str();

		const int iMissionLevel = _ttoi(szMissionLevel);

		if( kRankReserveVec.begin() != temp_iter
		&&	iMissionLevel != iCurMissionLevel )
		{
			_ftprintf_s(pFile, LUA_MISSION_RANK_FOOTER);
		}

		if( kRankReserveVec.begin() == temp_iter
		||	rkRankEvent.iMissionKey != iCurMissoinKey )
		{
			iCurMissoinKey = rkRankEvent.iMissionKey;
			_ftprintf_s(pFile, LUA_MISSION_RANK, iCurMissoinKey);
			_ftprintf_s(pFile, LUA_MISSION_RANK_STATER, iCurMissoinKey);
		}

		if( kRankReserveVec.begin() == temp_iter
		||	iMissionLevel != iCurMissionLevel )
		{
			iCurMissionLevel = iMissionLevel;
			_ftprintf_s(pFile, LUA_MISSION_RANK_HEADER, iCurMissoinKey, iCurMissionLevel);
		}

		_ftprintf_s(pFile, LUA_MISSION_RANK_BODY, szTargetRank, rkRankEvent.iObjectNo);


		++temp_iter;
	}
	if( kRankReserveVec.size() )
	{
		_ftprintf_s(pFile, LUA_MISSION_RANK_FOOTER);
	}
	kRankReserveVec.clear();
	/////////////////////////

	return true;
}

bool PgXml2Lua::PrintOutLua_Pvp(FILE* pFile)
{
	if( m_kDepend_Pvp.Empty() )
	{
		return true;
	}

	_ftprintf_s(pFile, LUA_PVP_STARTER);
	_ftprintf_s(pFile, LUA_PVP_WIN, m_kDepend_Pvp.iObjectNo);
	_ftprintf_s(pFile, LUA_PVP_FOOTER);

	return true;
}

bool PgXml2Lua::PrintOutLua_OnComplete(FILE *pFile)
{
	_ftprintf_s(pFile, LUA_COMPLETE_HEADER);
	//퀘스트 아이템을 여기서 회수 한다.
	ContItemSet kDeleteItemSet;
	PrintOutLua_MonsterGiveItemDelete(pFile, kDeleteItemSet);
	PrintOutLua_NPCGiveItemDelete(pFile, kDeleteItemSet);
	PrintOutLua_DependItemDelete(pFile, kDeleteItemSet);
	_ftprintf_s(pFile, LUA_COMPLATE_FOOTER);
	return true;
}

void PgXml2Lua::PrintWaring(TiXmlElement *pkNode)
{
	printf_s("경고!! [%s - R:%d, C:%d]: 잘못된 XML Element <%s>\n", m_kInputName.c_str(), pkNode->Row(), pkNode->Column(), pkNode->Value());
}

void PgXml2Lua::ParseError(TiXmlNode* pkNode)
{
	const char* szName = pkNode->Value();
	const int iRow = pkNode->Row();
	const int iColumn = pkNode->Column();
	const int iQuestID = m_kBasic.iQuestID;
	printf_s("[%s]-[%d] Quest ID[%d] Unknown Element Name:[%s] Row:[%d] Column:[%d]", __FUNCTION__, __LINE__, iQuestID, szName, iRow, iColumn);
	exit(0);
}

void PgXml2Lua::Exit(int iVal)
{
	printf_s("경고!! [%s] 에서 오류\n", m_kInputName.c_str());
	exit(iVal);
}

void PgXml2Lua::CheckItem(const DWORD dwItemNo, TiXmlElement *pkNode)
{
	if( g_bDisableCheck ) {return;}

	const CItemDef *pkItemDef = g_kItemDefMgr.GetDef(dwItemNo);
	if( !pkItemDef )//없으면 에러
	{
		printf_s("\a\n\n");
		printf_s("잘못된 아이템 번호 [%d]\n", dwItemNo);
		if( pkNode )
		{
			printf_s("위치 Line %d, Column %d\n", pkNode->Row(), pkNode->Column());
		}
		Exit(0);
	}
}

void PgXml2Lua::CheckMonster(const DWORD dwMonsterID, TiXmlElement *pkNode)
{
	if( g_bDisableCheck ) {return;}

	const CMonsterDef* pkMonsterDef = g_MonsterDef.GetDef(dwMonsterID);
	if( !pkMonsterDef )//없으면 에러
	{
		printf_s("\a\n\n");
		printf_s("잘못된 몬스터 번호 [%d]\n", dwMonsterID);
		if( pkNode )
		{
			printf_s("위치 Line %d, Column %d\n", pkNode->Row(), pkNode->Column());
		}
		Exit(0);
	}
}