
// Tiny XML Library
#ifdef _DEBUG
#pragma comment(lib, "tinyxmld.lib")
#else
#pragma comment(lib, "tinyxml.lib")
#endif
#include "tinyxml/tinyxml.h"



#pragma once

#include "Variant/PgQuestInfo.h"

//Argument Structure
//typedef struct tagArgument
//{
//	std::string kID;			//OBJECTNO="" TYPE="" VALUE="">xxx</
//	std::string kObjectNo;		//OBJECTNO="xxx" TYPE="" VALUE=""></
//	std::string kStr1;			//OBJECTNO="" TYPE="xxx" VALUE=""></
//	std::string kStr2;			//OBJECTNO="" TYPE="" VALUE="xxx"></
//	bool bUsed;
//	tagArgument()
//	{
//		Clear();
//	}
//	tagArgument(const TCHAR* szID, const TCHAR* szObjectNo, const TCHAR* szStr1, const TCHAR* szStr2)
//	{
//		Set(szID, szObjectNo, szStr1, szStr2);
//	};
//	void Set(const TCHAR* szID, const TCHAR* szObjectNo, const TCHAR* szStr1, const TCHAR* szStr2)
//	{
//		Clear();
//		kID = (NULL!=szID)? szID: "";
//		kObjectNo = (NULL!=szObjectNo)? szObjectNo: "";
//		kStr1 = (NULL!=szStr1)? szStr1: "";
//		kStr2 = (NULL!=szStr2)? szStr2: "";
//	};
//	void Clear()
//	{
//		kID.clear();
//		kObjectNo.clear();
//		kStr1.clear();
//		kStr2.clear();
//		bUsed = false;
//	}
//} SArgument;

//typedef std::vector< std::pair< std::string, std::string > > ContRewardItem;	//Reward
//typedef std::vector< SArgument > ContMonsterPack;							//Monster
//typedef std::vector< SArgument > ContNpcPack;								//NPC
//typedef std::vector< SArgument > ContItemPack;							//Item;
//typedef std::vector< SArgument > ContLocationPack;							//Item;
//
typedef std::set< int > ContSetInt;

class PgXml2Lua : public PgQuestInfo
{
	typedef std::set< SItem > ContItemSet;

public:
	PgXml2Lua()
	{
		PgQuestInfo();
		//m_iItemCount1 = 0;
		//m_iItemCount2 = 0;
	}
	PgXml2Lua(const std::string &rkName)
	{
		PgQuestInfo();

		//m_iItemCount1 = 0;
		//m_iItemCount2 = 0;
		m_kInputName = rkName;
		BreakLastSep(rkName, m_kOutputName);
		m_kOutputName += ".lua";
	};
	~PgXml2Lua() {};

	////
	bool OpenXml(const TCHAR* szFileName);
	bool OpenXml(const std::string &rkName);
	bool OpenXml();

	////
	//bool ParseXml(TiXmlElement *pkNode);
	//bool Parseobjects(TiXmlElement *pkNode);
	//bool ParseDependents(TiXmlElement *pkNode);
	//bool ParseReward(TiXmlElement *pkNode);

	//
	//bool MakeDependElement(TiXmlElement *pkChild, SArgument &rkArgument);

	//
	void PrintWaring(TiXmlElement *pkNode);
	void Exit(int iVal);


	//
	bool PrintOutLua();
	bool PrintOutLua_Head(FILE *pFile);
	bool PrintOutLua_Init(FILE *pFile);
		bool Print_GiveReward(FILE* pFile);
	bool PrintOutLua_Monster(FILE *pFile);
	bool PrintOutLua_NPC(FILE *pFile);
	bool PrintOutLua_Item(FILE *pFile);
	bool PrintOutLua_Location(FILE *pFile);
	bool PrintOutLua_Mission(FILE* pFile);
	bool PrintOutLua_Pvp(FILE* pFile);

	bool PrintOutLua_OnComplete(FILE *pFile);

protected:
	void PrintOutLua_MonsterGiveItemDelete(FILE* pFile, ContItemSet& rkItemSet);
	void PrintOutLua_NPCGiveItemDelete(FILE* pFile, ContItemSet& rkItemSet);
	void PrintOutLua_DependItemDelete(FILE* pFile, ContItemSet& rkItemSet);

	virtual void CheckItem(const DWORD dwItemNo, TiXmlElement *pkNode);
	virtual void CheckMonster(const DWORD dwMonsterID, TiXmlElement *pkNode);

	virtual void ParseError(TiXmlNode* pkNode);
	
//private:
public:
	std::string			m_kInputName;		//Xml 파일명
	std::string			m_kOutputName;		//Lua 파일명
//
//public:
//	std::string			m_kQuestID;			//퀘스트 
//	std::string			m_kFileName;		//
//	std::string			m_kChangeClass;		//전직할 직업 번호
//	std::string			m_kPrevClass;		//이전 직업 번호
//	std::string			m_kMinLevel;		//최소 레벨(이상)
//	int					m_iItemCount1;		//1번 보상 아이템 열 지급 갯수
//	ContRewardItem		m_kItemVec1;		//벡터
//	int					m_iItemCount2;		//2번 보상 아이템 열 지급 갯수
//	ContRewardItem		m_kItemVec2;		//벡터
//
//	ContMonsterPack		m_kDependMonVec;	//의존 몬스터
//	ContNpcPack			m_kDependNpcVec;	//의존 NPC
//	ContItemPack		m_kDependItemVec;	//의존 Item목록
//	ContLocationPack	m_kDependLocation;	//의존 Location Trigger목록
//
//	int					m_iObject[5];		//퀘스트 목표 5개 항목

protected:
	
	bool NpcPreEvent(const ContQuestDenpendNpc::value_type& rkElement, FILE* pFile);
	bool NpcAfterEvent(const ContQuestDenpendNpc::value_type& rkElement, FILE* pFile);
};

#define ArrayFunc(Type, Array, FuncName)									\
	{																		\
		Type& rkVec = Array;												\
		Type::iterator iter = rkVec.begin();								\
		while(rkVec.end() != iter)											\
		{																	\
			const Type::value_type& rkElement = (*iter);					\
			const bool bRet = FuncName(rkElement, pFile);					\
			if( bRet )														\
			{																\
				iter = rkVec.erase(iter);									\
			}																\
			else															\
			{																\
				++iter;														\
			}																\
		}																	\
	}
//bool NpcPreEvent(const ContQuestDenpendNpc::value_type& rkElement, FILE* pFile);
