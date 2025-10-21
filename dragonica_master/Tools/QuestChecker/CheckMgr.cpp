#include "stdafx.h"
#include "variant/PgStringUtil.h"
#include "Variant/PgMission.h"
#include "Variant/Global.h"
#include "BM/LocalMgr.h"
#include "../../Archangel/MapServer/constant.h"
#include "CheckMgr.h"
#include "LogMgr.h"

namespace CheckerUtil
{
	CObserverMgr kObserverMgr;
}

namespace TextHelper
{
	bool bLoading = false;
	typedef	std::map< int, std::wstring > CONT_TEXT_TABLE;
	Loki::Mutex m_kTextMutex;
	CONT_TEXT_TABLE	kContText;
	void Clear()
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		kContText.clear();
	}
	void LoadFinish()
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		bLoading = false;
	}
	void Load()
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		if( !bLoading )
		{
			bLoading = true;
			CheckerUtil::CObserver kObserver(LoadFinish);
			kObserver << LT_LOAD_TEXT_TABLE << LT_LOAD_QUEST_TEXT_TABLE << LT_LOAD_QUEST_TEXT_TABLE;
			CheckerUtil::kObserverMgr.Add(kObserver);

			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_QUEST_TEXT_TABLE, WSTR_QUEST_QTT) );
			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_QUEST_TEXT_TABLE, WSTR_QUEST_QTT2) );
			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_QUEST_TEXT_TABLE, WSTR_QUEST_QTT_NEW2) );
			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_QUEST_TEXT_TABLE, WSTR_QUEST_QTT_NEW3) );
			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_QUEST_TEXT_TABLE, WSTR_QUEST_QTT_WANTED) );			
			g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LOAD_TEXT_TABLE, WSTR_TT) );
		}
	}
	bool AddText(int const iTextID, std::wstring const& rkText)
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		auto kRet = kContText.insert( std::make_pair(iTextID, rkText) );
		return kRet.second;
	}
	std::wstring GetText(int const iTextID)
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		CONT_TEXT_TABLE::const_iterator find_iter = kContText.find(iTextID);
		if( kContText.end() != find_iter )
		{
			return (*find_iter).second;
		}
		return std::wstring();
	}
	bool IsText(int const iTextID)
	{
		BM::CAutoMutex kLock(m_kTextMutex);
		return kContText.end() == kContText.find(iTextID);
	}
};

namespace ResultHelper
{
	typedef	struct	tagPgQuest
	{
		PgQuestInfo		Info;
		std::wstring	Path;			
	}PgQuest;
	typedef std::map< int, PgQuest >				ContQuestList;

	Loki::Mutex m_kResultMutex;
	ContQuestList		m_kSuccessQuestList;
	ContQuestList		m_kQFailedList;

	void Clear()
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		m_kSuccessQuestList.clear();
		m_kQFailedList.clear();
	}
	void AddSuccessQuest(const PgQuest& QuestInfo)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		ContQuestList::iterator iter = m_kSuccessQuestList.find(QuestInfo.Info.ID());
		if( iter == m_kSuccessQuestList.end() )
		{
			m_kSuccessQuestList.insert(std::make_pair(QuestInfo.Info.ID(), QuestInfo));
		}	
	}
	void AddQFailed(const PgQuest& QuestInfo)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		ContQuestList::iterator	iter = m_kQFailedList.find(QuestInfo.Info.ID());
		if( iter == m_kQFailedList.end() )
		{
			m_kQFailedList.insert(std::make_pair(QuestInfo.Info.ID(), QuestInfo));		
		}	
	}
	void OutputResult(const PgQuest& Info, EOUTTYPE OutType )
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		wchar_t		szTemp[MAX_PATH] = {0,};
		std::wstring	Name	= TextHelper::GetText(Info.Info.m_iTitleTextNo);//	퀘스트 이름
		std::wstring	Area	= TextHelper::GetText(Info.Info.m_iGroupTextNo);//	퀘스트 지역
		swprintf(szTemp, MAX_PATH, WSTR_QUESTLISTITEM.c_str(), 
			Info.Info.ID(), Info.Info.m_iTitleTextNo, Name.c_str(), Info.Info.m_iGroupTextNo, Area.c_str(), Info.Info.m_kLimit.iMinLevel, Info.Info.m_kLimit.iMaxLevel );
		g_Core.AddListItem(szTemp, OutType, CHK_SUCCESS);
	}
	bool Verify(QuestContainer& rkContQuestInfo)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		// g_kErrorMsg 안에 있는 놈들은 성공 리스트에서 지우고, 실패리스트에 추가.
		ContErrorMsg::iterator iterErr = g_kErrorMsg.begin();
		while( g_kErrorMsg.end() != iterErr )
		{
			ContErrorMsg::mapped_type const& rkContMsg = (*iterErr).second;
			ContErrorMsg::mapped_type::const_iterator msg_iter = rkContMsg.begin();
			while( rkContMsg.end() != msg_iter )
			{
				ResultHelper::PgQuest	kQuest;

				QuestContainer::iterator iter = rkContQuestInfo.find( (*msg_iter).iQuestID );
				if( iter != rkContQuestInfo.end() )
				{
					kQuest.Info = *(iter->second);
				}//여기에 없으면 퀘스트와 관련 없음

				m_kQFailedList.insert( std::make_pair( (*msg_iter).iQuestID, kQuest ) );
				m_kSuccessQuestList.erase( (*msg_iter).iQuestID );

				g_kLogMgr.PutMsg( SLOGMESSAGE( (*msg_iter).iQuestID, LT_ERROR, L"", SErrorData( (*msg_iter).eType, (*msg_iter).iQuestID, (*msg_iter).kErrorMsg ) ) );

				++msg_iter;
			}
			++iterErr;
		}

		// 성공리스트 출력
		ContQuestList::iterator iterSuccess = m_kSuccessQuestList.begin();
		while( m_kSuccessQuestList.end() != iterSuccess )
		{
			OutputResult( iterSuccess->second, OT_SUCCESS );
			++iterSuccess;
		}

		// 실패 리스트 출력
		ContQuestList::iterator iterFail = m_kQFailedList.begin();
		while( m_kQFailedList.end() != iterFail )
		{	
			OutputResult( iterFail->second, OT_FAIL );
			//OutputResult( iterFail->second, OT_DB_NOT_EXIST );
			++iterFail;
		}
		return g_kErrorMsg.empty();
	}
	void GetQuestText(std::wstring const& wstrPath)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		ContQuestDialog*	m_kSQTextInfo = NULL;
		const int	nID = _wtoi(wstrPath.c_str());
		wchar_t		szError[MAX_PATH] = {0,};
		std::wstring	wstrError = L"";

		ContQuestList::iterator	iter = m_kSuccessQuestList.find(nID);
		if(iter != m_kSuccessQuestList.end())
		{
			m_kSQTextInfo	= &(iter->second.Info.m_kDialog);
			
			ContQuestDialog::iterator	Dlg_iter = m_kSQTextInfo->begin();

			if(m_kSQTextInfo->size())
			{
				while(Dlg_iter != m_kSQTextInfo->end())
				{
					const int	DlgID	= Dlg_iter->first;

					if(Dlg_iter->second.kDialogText.size())
					{
						ContQuestDialogText::iterator	TxtID_iter = Dlg_iter->second.kDialogText.begin();
						while(TxtID_iter != Dlg_iter->second.kDialogText.end())
						{
							if( TextHelper::IsText(TxtID_iter->iTextNo) )
							{
								swprintf(szError, MAX_PATH, L"%d번 텍스트가 없습니다.", TxtID_iter->iTextNo);
								wstrError = szError;
								g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, wstrError));
							}
							else
							{
								const int			TxtID		= TxtID_iter->iTextNo;
								const std::wstring	wstrText	= TextHelper::GetText(TxtID_iter->iTextNo);

								wchar_t	szResult[2048] = {0,};
								swprintf(szResult, MAX_PATH, WSTR_TEXTLISTITEM.c_str(), DlgID, TxtID, wstrText.c_str());
								g_Core.AddListItem(szResult, OT_FAIL, CHK_QTVIEWER);
							}

							++TxtID_iter;
						}
					}
					else
					{
						swprintf(szError, MAX_PATH, L"%d번 다이얼로그에 할당된 텍스트가 없습니다.", DlgID);
						wstrError = szError;
						g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, wstrError));
					}

					++Dlg_iter;
				}
			}
			else
			{
				g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"출력 다이얼로그가 없습니다."));
			}
		}
		else
		{
			swprintf(szError, MAX_PATH, L"%d번 퀘스트의 텍스트가 존재하지 않습니다.", nID);
			wstrError = szError;
			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, wstrError));
		}

		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"텍스트 출력 완료."));
	}
	void CheckExistIDToFailList(CONT_NOT_EXIST_ID& List)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		if( !m_kQFailedList.size() )
		{
			return;
		}

		ContQuestList::iterator Fail_iter = m_kQFailedList.begin();
		while( Fail_iter != m_kQFailedList.end() )
		{
			auto Rst = List.insert(Fail_iter->first);
			if( !Rst.second )
			{
				//error
			}
			++Fail_iter;
		}
	}

	void CheckExistIDToSuccessList(CONT_NOT_EXIST_ID& List)
	{
		BM::CAutoMutex kLock(m_kResultMutex);
		if( !m_kSuccessQuestList.size() )
		{
			return;
		}

		ContQuestList::iterator Suc_iter = m_kSuccessQuestList.begin();
		while( Suc_iter != m_kSuccessQuestList.end() )
		{
			auto Rst = List.insert(Suc_iter->first);
			if( !Rst.second )
			{
				//error
			}
			++Suc_iter;
		}
	}
};

namespace DefStringHelper
{
	std::wstring GetText(int const iDefStringNo)
	{
		CONT_DEFSTRINGS const* pkDefStrings = NULL;
		g_kTblDataMgr.GetContDef(pkDefStrings);
		CONT_DEFSTRINGS::const_iterator find_iter = pkDefStrings->find(iDefStringNo);
		if( pkDefStrings->end() != find_iter )
		{
			return (*find_iter).second.strText;
		}
		return std::wstring();
	}
	std::wstring GetMapName(int const iMapNo)
	{
		CONT_DEFMAP const* pkDefMap = NULL;
		g_kTblDataMgr.GetContDef(pkDefMap);
		CONT_DEFMAP::const_iterator find_iter = pkDefMap->find(iMapNo);
		if( pkDefMap->end() != find_iter )
		{
			return GetText((*find_iter).second.NameNo);
		}
		return std::wstring();
	}
	std::wstring GetEffectName(int const iEffectNo)
	{
		CONT_DEFEFFECT const* pkDefEffect = NULL;
		g_kTblDataMgr.GetContDef(pkDefEffect);
		CONT_DEFEFFECT::const_iterator iter = pkDefEffect->find(iEffectNo);
		if( pkDefEffect->end() != iter )
		{
			return GetText((*iter).second.iName);
		}
		return std::wstring();
	}
	std::wstring GetSkillName(int const iSkillNo)
	{
		CONT_DEFSKILL const* pkDefSkill = NULL;
		g_kTblDataMgr.GetContDef(pkDefSkill);
		CONT_DEFSKILL::const_iterator iter = pkDefSkill->find(iSkillNo);
		if( pkDefSkill->end() != iter )
		{
			return GetText((*iter).second.RscNameNo);
		}
		return std::wstring();
	}
	std::wstring GetItemName(int const iItemNo)
	{
		CONT_DEFITEM const* pkDefItem = NULL;
		g_kTblDataMgr.GetContDef(pkDefItem);
		CONT_DEFITEM::const_iterator iter = pkDefItem->find(iItemNo);
		if( pkDefItem->end() != iter )
		{
			return GetText((*iter).second.NameNo);
		}
		return std::wstring();
	}
	std::wstring GetMonsterName(int const iMonsterNo)
	{
		CONT_DEFMONSTER const* pkDefMonster = NULL;
		g_kTblDataMgr.GetContDef(pkDefMonster);
		CONT_DEFMONSTER::const_iterator iter = pkDefMonster->find(iMonsterNo);
		if( pkDefMonster->end() != iter )
		{
			return GetText((*iter).second.Name);
		}
		return std::wstring();
	}
}

namespace WorldHelper
{
	struct SNpcInfo
	{
		SNpcInfo(BM::GUID const& rkGuid, int const iMapNo, std::wstring const& rkName)
			: kNpcGuid(rkGuid), iGroundNo(iMapNo), kNpcName(rkName)
		{
		}
		SNpcInfo(SNpcInfo const& rhs)
			: kNpcGuid(rhs.kNpcGuid), iGroundNo(rhs.iGroundNo), kNpcName(rhs.kNpcName)
		{
		}

		BM::GUID kNpcGuid;
		int const iGroundNo;
		std::wstring kNpcName;
	};
	typedef std::list< SNpcInfo > CONT_NPC_INFO;
	typedef std::map< BM::GUID, CONT_NPC_INFO > CONT_NPC_GUID_INFO;

	Loki::Mutex m_kNpcMutex;
	CONT_NPC_GUID_INFO kNpcInfo;

	void Clear()
	{
		BM::CAutoMutex kLock(m_kNpcMutex);
		kNpcInfo.clear();
	}
	TiXmlElement const* AddNpc(TiXmlElement const* pkNode, int const iMapNo)
	{
		if(NULL==pkNode)	{return NULL;}

		BM::GUID const kNpcGuid( PgStringUtil::SafeUni(pkNode->Attribute("GUID")) );
		std::wstring kNpcName( DefStringHelper::GetText(PgStringUtil::SafeAtoi(pkNode->Attribute("NAME"))) );
		PgStringUtil::ReplaceStr(kNpcName, std::wstring(L"<"), std::wstring(L"&lt;"), kNpcName);
		PgStringUtil::ReplaceStr(kNpcName, std::wstring(L">"), std::wstring(L"&gt;"), kNpcName);
		{
			BM::CAutoMutex kLock(m_kNpcMutex);
			SNpcInfo const kTempNpcInfo(kNpcGuid, iMapNo, kNpcName);

			auto kRet = kNpcInfo.insert( std::make_pair(kNpcGuid, CONT_NPC_GUID_INFO::mapped_type()) );
			(*kRet.first).second.push_back(kTempNpcInfo);
		}
		return pkNode->NextSiblingElement("NPC");
	}
	bool ParseXml(TiXmlElement const* pkTopNode, int const iMapNo)
	{
		if( !pkTopNode )
		{
			return false;
		}

		TiXmlElement const* pkNode = pkTopNode->FirstChildElement("NPC");
		while( pkNode )
		{
			/*BM::GUID const kNpcGuid( PgStringUtil::SafeUni(pkNode->Attribute("GUID")) );
			std::wstring kNpcName( DefStringHelper::GetText(PgStringUtil::SafeAtoi(pkNode->Attribute("NAME"))) );
			PgStringUtil::ReplaceStr(kNpcName, std::wstring(L"<"), std::wstring(L"&lt;"), kNpcName);
			PgStringUtil::ReplaceStr(kNpcName, std::wstring(L">"), std::wstring(L"&gt;"), kNpcName);
			{
				BM::CAutoMutex kLock(m_kNpcMutex);
				SNpcInfo const kTempNpcInfo(kNpcGuid, iMapNo, kNpcName);
				
				auto kRet = kNpcInfo.insert( std::make_pair(kNpcGuid, CONT_NPC_GUID_INFO::mapped_type()) );
				(*kRet.first).second.push_back(kTempNpcInfo);
			}
			pkNode = pkNode->NextSiblingElement("NPC");*/
			pkNode = AddNpc(pkNode, iMapNo);
		}
		return true;
	}
	std::wstring GetNpcText(SNpcInfo const& rkNpc)
	{
		return rkNpc.kNpcName + L"(" + DefStringHelper::GetMapName(rkNpc.iGroundNo) + L":" + std::wstring(BM::vstring(rkNpc.iGroundNo)) + L")";
	}
	std::wstring GetNpcText(BM::GUID const& rkNpcGuid, bool const bFull)
	{
		BM::CAutoMutex kLock(m_kNpcMutex);
		std::wstring kTempString;
		CONT_NPC_GUID_INFO::const_iterator find_iter = kNpcInfo.find(rkNpcGuid);
		if( kNpcInfo.end() != find_iter )
		{
			CONT_NPC_INFO const& rkNpcCont = (*find_iter).second;
			if( bFull )
			{
				CONT_NPC_INFO::const_iterator iter = rkNpcCont.begin();
				while( rkNpcCont.end() != iter )
				{
					if( rkNpcCont.begin() != iter )
					{
						kTempString += std::wstring(L"<BR>");
					}
					kTempString += GetNpcText(*iter);
					++iter;
				}
			}
			else
			{
				kTempString += GetNpcText(rkNpcCont.front());
				if( 1 < rkNpcCont.size() )
				{
					kTempString += std::wstring(L" ...");
				}
			}
		}
		return kTempString;
	}

	TiXmlNode const* ParseAllNpcXml( TiXmlNode const *pkNode, int& rikOutGroundNo )
	{
		int const iType = pkNode->Type();

		switch(iType)
		{
		case TiXmlNode::ELEMENT:
			{
				TiXmlElement *pkElement = (TiXmlElement *)pkNode;

				std::string kTagName(pkElement->Value());

				if( kTagName == PgXmlLocalUtil::LOCAL_ELEMENT_NAME)
				{
					TiXmlNode const* pkFindLocalNode = PgXmlLocalUtil::FindInLocal(g_kLocal, pkElement);
					if( pkFindLocalNode )
					{
						TiXmlElement const* pkResultNode = pkFindLocalNode->FirstChildElement();
						if( pkResultNode )
						{
							ParseAllNpcXml( pkResultNode, rikOutGroundNo );
						}
					}
				}
				else if( kTagName == "WORLD" )
				{
					TiXmlAttribute* pkAttr = pkElement->FirstAttribute();
					while( pkAttr )
					{
						char const* pcAttrName = pkAttr->Name();
						char const* pcAttrValue = pkAttr->Value();

						if( strcmp( pcAttrName, "MAPNO" ) == 0 )
						{
							rikOutGroundNo = atoi(pcAttrValue);
						}

						pkAttr = pkAttr->Next();
					}

					const TiXmlNode* pkChildNode = pkNode->FirstChild();
					if( pkChildNode != 0 )
					{
						while( ( pkChildNode = ParseAllNpcXml( pkChildNode, rikOutGroundNo ) ) );
					}
				}
				else if( kTagName == "NPC" )
				{
					AddNpc( pkElement, rikOutGroundNo );
				}
				else
				{

				}
			}break;
		default:
			{
			}break;
		}

		// 같은 층의 다음 노드를 재귀적으로 파싱한다.
		const TiXmlNode* pkNextNode = pkNode->NextSibling();
		if(pkNextNode)
		{
			return pkNextNode;
		}

		return NULL;
	}

	bool LoadAllNpc(std::wstring const& wstrPath)
	{
		TiXmlDocument kXmlDoc;
		if( kXmlDoc.LoadFile(MB(wstrPath) ))
		{
			TiXmlNode* pkRootNode = kXmlDoc.FirstChild("TOTALNPC");
			if( pkRootNode )
			{
				TiXmlNode* pkNextNode = pkRootNode->FirstChild();
				while( pkNextNode )
				{
					int iOutGroundNo = 0;
					ParseAllNpcXml(pkNextNode, iOutGroundNo);
					pkNextNode = pkNextNode->NextSibling();
				}

				return true;
			}
		}

		return false;
	}
};

namespace SortUtil
{
	// ID
	bool SortByID(PgQuestInfo const* lhs, PgQuestInfo const* rhs)
	{
		return lhs->ID() < rhs->ID();
	}
	bool SortByIDReverse(PgQuestInfo const* lhs, PgQuestInfo const* rhs)
	{
		return lhs->ID() > rhs->ID();
	}
};

namespace HtmlUtil
{
#define QUEST_KEY_NAME_FORMAT L"Quest%08d"
	enum EQuestLinkType
	{
		QLT_Title = 1,
		QLT_PreQuest = 2,
		QLT_NoAnchor = 3
	};
	struct STitleLink
	{
		void Clear()
		{
			kQuestID.clear();
			kTitle.clear();
			kType.clear();
			kGroup.clear();
			kMinLevel.clear();
			kMaxLevel.clear();
			kLimitMaxLevel.clear();
		}

		std::wstring kQuestID;
		std::wstring kTitle;
		std::wstring kType;
		std::wstring kGroup;
		std::wstring kMinLevel;
		std::wstring kMaxLevel;
		std::wstring kLimitMaxLevel;
	};
	std::wstring GetReverseString(bool const bReverse)
	{
		if( bReverse )
		{
			return std::wstring(L"Reverse");
		}
		return std::wstring();
	}
	std::wstring GetReverseTitle(bool const bReverse)
	{
		if( bReverse )
		{
			return std::wstring(L"(▲)");
		}
		return std::wstring(L"(▼)");
	}
	enum EQuestListSortType
	{
		QLST_BY_ID	= 1,
		QLST_BY_TITLE,
		QLST_BY_TYPE,
		QLST_BY_GROUP,
		QLST_BY_MIN_LEVEL,
		QLST_BY_MAX_LEVEL,
		QLST_BY_LIMIT_MAX_LEVEL,
	};
	void MakeTitleLink(STitleLink& rkOutLink, STitleLink& rkOutAddTitle, EQuestListSortType const eType, bool const bReverse)
	{
		std::wstring const kTail(L".html");
		STitleLink kTempLink, kTempTitle;
		kTempLink.kQuestID			= std::wstring(L"ListByID")				+ GetReverseString(eType == QLST_BY_ID && bReverse)		+ kTail;
		kTempLink.kTitle			= std::wstring(L"ListByTitle")			+ GetReverseString(eType == QLST_BY_TITLE && bReverse)	+ kTail;
		kTempLink.kType				= std::wstring(L"ListByType")			+ GetReverseString(eType == QLST_BY_TYPE && bReverse)	+ kTail;
		kTempLink.kGroup			= std::wstring(L"ListByGroup")			+ GetReverseString(eType == QLST_BY_GROUP && bReverse)	+ kTail;
		kTempLink.kMinLevel			= std::wstring(L"ListByMinLevel")		+ GetReverseString(eType == QLST_BY_MIN_LEVEL && bReverse)	+ kTail;
		kTempLink.kMaxLevel			= std::wstring(L"ListByMaxLevel")		+ GetReverseString(eType == QLST_BY_MAX_LEVEL && bReverse)	+ kTail;
		kTempLink.kLimitMaxLevel	= std::wstring(L"ListByLimitMaxLevel")	+ GetReverseString(eType == QLST_BY_LIMIT_MAX_LEVEL && bReverse)	+ kTail;
		rkOutLink = kTempLink;
		switch( eType )
		{
		case QLST_BY_ID:				{ kTempTitle.kQuestID = GetReverseTitle(bReverse); }break;
		case QLST_BY_TITLE:				{ kTempTitle.kTitle = GetReverseTitle(bReverse); }break;
		case QLST_BY_TYPE:				{ kTempTitle.kType = GetReverseTitle(bReverse); kTempTitle.kMinLevel = GetReverseTitle(bReverse); kTempTitle.kMaxLevel = GetReverseTitle(bReverse); }break;
		case QLST_BY_GROUP:				{ kTempTitle.kGroup = GetReverseTitle(bReverse); kTempTitle.kMinLevel = GetReverseTitle(bReverse); kTempTitle.kMaxLevel = GetReverseTitle(bReverse); }break;
		case QLST_BY_MIN_LEVEL:			{ kTempTitle.kMinLevel = GetReverseTitle(bReverse); kTempTitle.kMaxLevel = GetReverseTitle(bReverse); }break;
		case QLST_BY_MAX_LEVEL:			{ kTempTitle.kMaxLevel = GetReverseTitle(bReverse); }break;
		case QLST_BY_LIMIT_MAX_LEVEL:	{ kTempTitle.kLimitMaxLevel = GetReverseTitle(bReverse); }break;
		}
		rkOutAddTitle = kTempTitle;
	}
	typedef std::list< PgQuestInfo const* > CONT_QUEST;

	std::wstring MakeQuestLink(EQuestLinkType const eType, int const iQuestID, std::wstring kLinkName)
	{
		wchar_t szTemp[MAX_PATH] = {0, };
		switch( eType )
		{
		case QLT_Title:
			{
				swprintf_s(szTemp, L"<A HREF='" QUEST_KEY_NAME_FORMAT L".html' NAME='" QUEST_KEY_NAME_FORMAT L"'> %s </A>", iQuestID, iQuestID, kLinkName.c_str());
			}break;
		case QLT_PreQuest:
			{
				swprintf_s(szTemp, L"<A HREF='" QUEST_KEY_NAME_FORMAT L".html'> [%d#] %s</A> (<A HREF='#" QUEST_KEY_NAME_FORMAT L"'>##</A>)", iQuestID, iQuestID, kLinkName.c_str(), iQuestID);
			}break;
		case QLT_NoAnchor:
			{
				swprintf_s(szTemp, L"<A HREF='" QUEST_KEY_NAME_FORMAT L".html'> [%d#] %s </A>", iQuestID, iQuestID, kLinkName.c_str());
			}break;
		default:
			{
			}break;
		}
		return std::wstring(szTemp);
	}
	std::wstring MakeQuestLink(EQuestLinkType const eType, int const iQuestID)
	{
		return MakeQuestLink(eType, iQuestID, g_kCheckMgr.GetQuestTitleText(iQuestID));
	}
	std::wstring MakeQuestLink(EQuestLinkType const eType, PgQuestInfo const* pkQuestInfo)
	{
		return MakeQuestLink(eType, pkQuestInfo->ID(), TextHelper::GetText(pkQuestInfo->m_iTitleTextNo));
	}
	std::wstring MakeQuestName(EQuestLinkType const eType, ContQuestID const& rkContID)
	{
		std::wstring kTemp;
		ContQuestID::const_iterator iter = rkContID.begin();
		while( rkContID.end() != iter )
		{
			if( rkContID.begin() != iter )
			{
				kTemp += L", ";
			}
			kTemp += MakeQuestLink(eType, *iter);
			++iter;
		}
		return kTemp;
	}
	std::wstring MakeLimitQuestName(EQuestLinkType const eType, PgQuestInfo const* pkQuestInfo)
	{
		if( !pkQuestInfo )
		{
			return std::wstring();
		}

		std::wstring kTemp;

		int iHaveCount = 0;
		if( !pkQuestInfo->m_kLimit_PreQuestAnd.empty() )	{ ++iHaveCount; }
		if( !pkQuestInfo->m_kLimit_PreQuestOr.empty() )		{ ++iHaveCount; }
		if( !pkQuestInfo->m_kLimit_IngQuest.empty() )		{ ++iHaveCount; }
		if( !pkQuestInfo->m_kLimit_NotQuest.empty() )		{ ++iHaveCount; }

		if( 0 == iHaveCount )
		{
			kTemp += TextHelper::GetText(700552); // 없음
		}

		if( !pkQuestInfo->m_kLimit_PreQuestAnd.empty() )
		{
			kTemp += TextHelper::GetText(700553) + L": "; // 선행 퀘스트(모두)
			kTemp += MakeQuestName(eType, pkQuestInfo->m_kLimit_PreQuestAnd);
		}
		if( !pkQuestInfo->m_kLimit_PreQuestOr.empty() )
		{
			if( !kTemp.empty() ) { kTemp += L", "; }
			kTemp += TextHelper::GetText(700554) + L": "; // 선행 퀘스트(이중 하나)
			kTemp += MakeQuestName(eType, pkQuestInfo->m_kLimit_PreQuestOr);
		}
		if( !pkQuestInfo->m_kLimit_IngQuest.empty() )
		{
			if( !kTemp.empty() ) { kTemp += L", "; }
			kTemp += TextHelper::GetText(700564) + L": "; // 선행 퀘스트(진행 중이어야)
			kTemp += MakeQuestName(eType, pkQuestInfo->m_kLimit_IngQuest);
		}
		if( !pkQuestInfo->m_kLimit_NotQuest.empty() )
		{
			if( !kTemp.empty() ) { kTemp += L", "; }
			kTemp += TextHelper::GetText(700565) + L": "; // 선행 퀘스트(진행 중이지 않거나, 완료하지 않아야)
			kTemp += MakeQuestName(eType, pkQuestInfo->m_kLimit_NotQuest);
		}
		return kTemp;
	}
	std::wstring MakeNpcName(ContQuestNpc const& rkContNpc, bool const bFull = false)
	{
		std::wstring kTemp;
		if( rkContNpc.empty() )
		{
			return TextHelper::GetText(700552); // 없음
		}
		if( bFull )
		{
			ContQuestNpc::const_iterator iter = rkContNpc.begin();
			while( rkContNpc.end() != iter )
			{
				if( rkContNpc.begin() != iter )
				{
					kTemp += L"<BR>";
				}
				kTemp += WorldHelper::GetNpcText((*iter).kNpcGuid, true);
				++iter;
			}
		}
		else
		{
			kTemp += WorldHelper::GetNpcText(rkContNpc.front().kNpcGuid, false);
			if( 1 < rkContNpc.size() )
			{
				kTemp += L" ...";
			}
		}
		return kTemp;
	}
	std::wstring GetClassName(int const iClassNo)
	{
		int const iBaseClassTextID = 30000;
		return TextHelper::GetText(iBaseClassTextID + iClassNo);
	}
	std::wstring MakeClassName(__int64 const iClassFlag)
	{
		if( 0 == iClassFlag )
		{
			return TextHelper::GetText(700552); // 없음
		}
		BM::vstring kTemp;
		__int64 const iBits = 8;
		__int64 const iMaxClassNo = sizeof(__int64) * iBits;
		__int64 const iBase = 1;
		__int64 const iStartNo = 1;
		for( __int64 iCur = iStartNo; iMaxClassNo > iCur; ++iCur )
		{
			if( 0 != (iClassFlag & (iBase<<iCur)) )
			{
				if( 0 != kTemp.size() )
				{
					kTemp << L", ";
				}
				kTemp << GetClassName(static_cast< int >(iCur)) << L"(" << BM::vstring(iCur) << L")";
			}
		}
		return std::wstring(kTemp);
	}
	std::wstring MakeChangeClassName(SChangeClassInfo const& rkClass)
	{
		std::wstring kTemp;
		if( 0 == rkClass.iChangeClassNo )
		{
			return TextHelper::GetText(700552); // 없음
		}
		int const iBaseChangeClassTextID = 30101;
		kTemp += TextHelper::GetText(rkClass.iChangeClassNo + iBaseChangeClassTextID - 1);
		//rkClass.iMinLevel
		//rkClass.iPrevClassNo
		return kTemp;
	}
	std::wstring MakeEffectName(ContRewardEffectVec const& rkCont)
	{
		std::wstring kTemp;
		if( rkCont.empty() )
		{
			return TextHelper::GetText(700552); // 없음
		}
		CONT_DEFEFFECT const* pkDefEffect = NULL;
		g_kTblDataMgr.GetContDef(pkDefEffect);
		ContRewardEffectVec::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			if( rkCont.begin() != iter )
			{
				kTemp += L"<BR>";
			}
			int const iEffectNo = (*iter);
			kTemp += DefStringHelper::GetEffectName(iEffectNo) + L" (" + std::wstring(BM::vstring(iEffectNo)) + L")";
			++iter;
		}
		return std::wstring(kTemp);
	}
	std::wstring MakeTimeText(SYSTEMTIME const& rkTime)
	{
		BM::vstring kTemp;
		kTemp << rkTime.wYear << L"-";
		kTemp << rkTime.wMonth << L"-";
		kTemp << rkTime.wDay << L" ";
		kTemp << rkTime.wHour << L":";
		kTemp << rkTime.wMinute << L":";
		kTemp << rkTime.wSecond;
		return std::wstring(kTemp);
	}
	std::wstring MakeTimeLimitText(ContQuestTimeLimit const& rkCont)
	{
		std::wstring kTemp;
		if( rkCont.empty() )
		{
			return TextHelper::GetText(700552); // 없음
		}
		ContQuestTimeLimit::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			if( rkCont.begin() != iter )
			{
				kTemp += L"<BR>";
			}
			ContQuestTimeLimit::value_type const& rkLimitTime = (*iter);
			kTemp += TextHelper::GetText(700569) + L":" + MakeTimeText(rkLimitTime.kTimeStart) + L" ~ " + TextHelper::GetText(700570) + L":" + MakeTimeText(rkLimitTime.kTimeEnd);
			++iter;
		}
		return kTemp;
	}
	std::wstring MakeWeekLimitText(ContDayOfWeek const& rkCont)
	{
		std::wstring kTemp;
		if( rkCont.empty() )
		{
			return TextHelper::GetText(700552); // 없음
		}
		int const iBaseWeekTextID = 700571;
		ContDayOfWeek::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			if( rkCont.begin() != iter )
			{
				kTemp += L"<BR>";
			}
			kTemp += TextHelper::GetText(iBaseWeekTextID + (*iter));
			++iter;
		}
		return kTemp;
	}
	std::wstring MakeCompleteTargetString(PgQuestInfo const* pkQuestInfo, bool const bFull = false)
	{
		BM::vstring kTemp;
		size_t iPrintCount = 0;
		for( int iCur = 0; QUEST_PARAMNUM > iCur; ++iCur )
		{
			int iTTW = 0;
			int const iTargetCount = pkQuestInfo->GetParamEndCondition(iCur);
			switch( iCur )
			{
			case 0:		{ iTTW = pkQuestInfo->m_iObjectTextNo1; }break;
			case 1:		{ iTTW = pkQuestInfo->m_iObjectTextNo2; }break;
			case 2:		{ iTTW = pkQuestInfo->m_iObjectTextNo3; }break;
			case 3:		{ iTTW = pkQuestInfo->m_iObjectTextNo4; }break;
			case 4:		{ iTTW = pkQuestInfo->m_iObjectTextNo5; }break;
			default:	{ }break;
			}
			if( iTargetCount )
			{
				if( 0 != iPrintCount )
				{
					if( bFull )
					{
						kTemp += L"<BR>";
					}
					else
					{
						kTemp += L", ";
					}
				}
				if( 1 < iTargetCount )
				{
					kTemp += BM::vstring(iCur+1) + L". " + TextHelper::GetText(iTTW) + L" (" + TextHelper::GetText(700555) + L":" + BM::vstring(iTargetCount) + L")";
				}
				else
				{
					kTemp += BM::vstring(iCur+1) + L". " + TextHelper::GetText(iTTW) + L"";
				}
				++iPrintCount;
			}
		}
		if( 0 == iPrintCount)
		{
			kTemp += L"&nbsp;";
		}
		return kTemp;
	}
	std::wstring MakeRewardText(SQuestRewardSet const& rkReward)
	{
		BM::vstring kTemp;
		kTemp += TextHelper::GetText(700582) + L":" + ((rkReward.bSelective)? TextHelper::GetText(700583): TextHelper::GetText(700584)) + L", ";
		//
		kTemp += TextHelper::GetText(700581) + L":";
		if( 0 == rkReward.iGender )
		{
			kTemp += TextHelper::GetText(700552);
		}
		else
		{
			kTemp += (1 == rkReward.iGender)? TextHelper::GetText(700585): TextHelper::GetText(700586);
		}
		kTemp += L", ";
		//
		kTemp += TextHelper::GetText(700580) + L":";
		if( rkReward.kCanClass.empty() )
		{
			kTemp += TextHelper::GetText(700552);
		}
		else
		{
			tagQuestRewardSet::ContClassNo::const_iterator iter = rkReward.kCanClass.begin();
			while( rkReward.kCanClass.end() != iter )
			{
				if( rkReward.kCanClass.begin() != iter )
				{
					kTemp += L", ";
				}
				kTemp += GetClassName((*iter));
				++iter;
			}
		}
		kTemp += L"<BR>";
		//
		size_t iCount = 0;
		ContQuestItem::const_iterator iter = rkReward.kItemList.begin();
		while( rkReward.kItemList.end() != iter )
		{
			++iCount;
			ContQuestItem::value_type const& rkItem = (*iter);
			if( rkReward.kItemList.begin() != iter )
			{
				kTemp += L"<BR>";
			}
			kTemp << iCount << L". " << DefStringHelper::GetItemName(rkItem.iItemNo) << L"(" << rkItem.iItemNo << L")";
			if( 1 < rkItem.iCount )
			{
				kTemp << L"(" << BM::vstring(rkItem.iCount) << L")";
			}
			if( rkItem.bSeal )
			{
				kTemp << L"(" << TextHelper::GetText(700593) << L")";
			}
			if( 0 != rkItem.iRarityType )
			{
				kTemp << L"(" << TextHelper::GetText(700592) << L":" << rkItem.iRarityType << L")";
			}
			if( 0 != rkItem.iTimeType
			&&	0 != rkItem.iTimeValue )
			{
				switch( rkItem.iTimeType )
				{
				case UIT_MIN:
					{
						kTemp << L"(" << TextHelper::GetText(700588)<< L"(" << TextHelper::GetText(700589) << L"):" << rkItem.iTimeValue << L")";
					}break;
				case UIT_HOUR:
					{
						kTemp << L"(" << TextHelper::GetText(700588)<< L"(" << TextHelper::GetText(700590) << L"):" << rkItem.iTimeValue << L")";
					}break;
				case UIT_DAY:
					{
						kTemp << L"(" << TextHelper::GetText(700588)<< L"(" << TextHelper::GetText(700591) << L"):" << rkItem.iTimeValue << L")";
					}break;
				}
			}
			++iter;
		}
		kTemp += L"<BR>";
		return std::wstring(kTemp);
	}
	std::wstring MakeRewardText(ContRewardVec const& rkCont)
	{
		std::wstring kTemp;
		if( rkCont.empty() )
		{
			return TextHelper::GetText(700552); // 없음
		}
		ContRewardVec::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			if( rkCont.begin() != iter )
			{
				kTemp += L"<BR>";
			}
			kTemp += MakeRewardText((*iter));
			++iter;
		}
		return kTemp;
	}
	std::wstring MakeRankText(int const iRankNo)
	{
		switch( iRankNo )
		{
		case MRANK_SSS:		{ return TextHelper::GetText(700618); }break;
		case MRANK_SS:		{ return TextHelper::GetText(700619); }break;
		case MRANK_S:		{ return TextHelper::GetText(700620); }break;
		case MRANK_A:		{ return TextHelper::GetText(700621); }break;
		case MRANK_B:		{ return TextHelper::GetText(700622); }break;
		case MRANK_C:		{ return TextHelper::GetText(700623); }break;
		case MRANK_D:		{ return TextHelper::GetText(700624); }break;
		case MRANK_E:		{ return TextHelper::GetText(700625); }break;
		case MRANK_F:		{ return TextHelper::GetText(700626); }break;
		default:
			{
			}break;
		}
		return std::wstring();
	}
	std::wstring MakeList(CONT_QUEST const& rkCont, STitleLink const& rkLink, STitleLink const& rkAddTitle)
	{
		std::wstring kTemp;
		kTemp += std::wstring(L"<HTML><link href='style.css' rel='stylesheet' type='text/css'/><HEAD></HEAD><BODY>\n");
		kTemp += std::wstring(L"<table border='1'>\n");
		kTemp += std::wstring(L"<TR>\n");
		if( 1 )
		{ // No link
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700529) + L"</TH>\n"; // 퀘스트 ID
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700530) + L"</TH>\n"; // 제목
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700531) + L"</TH>\n"; // 분류
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700532) + L"</TH>\n"; // 그룹명
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700533) + L"</TH>\n"; // 시작 레벨
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700534) + L"</TH>\n"; // 제한 레벨
			kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700535) + L"</TH>\n"; // 제한 레벨
		}
		else
		{ // link
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kQuestID		+ L"'>" + TextHelper::GetText(700529) + rkAddTitle.kQuestID			+ L"</A></TH>\n"; // 퀘스트 ID
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kTitle			+ L"'>" + TextHelper::GetText(700530) + rkAddTitle.kTitle			+ L"</A></TH>\n"; // 제목
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kType			+ L"'>" + TextHelper::GetText(700531) + rkAddTitle.kType			+ L"</A></TH>\n"; // 분류
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kGroup			+ L"'>" + TextHelper::GetText(700532) + rkAddTitle.kGroup			+ L"</A></TH>\n"; // 그룹명
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kMinLevel		+ L"'>" + TextHelper::GetText(700533) + rkAddTitle.kMinLevel		+ L"</A></TH>\n"; // 시작 레벨
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kMaxLevel		+ L"'>" + TextHelper::GetText(700534) + rkAddTitle.kMaxLevel		+ L"</A></TH>\n"; // 제한 레벨
			kTemp += std::wstring(L"<TH><A HREF='") + rkLink.kLimitMaxLevel	+ L"'>" + TextHelper::GetText(700535) + rkAddTitle.kLimitMaxLevel	+ L"</A></TH>\n"; // 제한 레벨
		}
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700579) + L"</TH>\n"; // 보상 경험치
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700578) + L"</TH>\n"; // 보상 골드
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700536) + L"</TH>\n"; // 선행 퀘스트
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700540) + L"</TH>\n"; // 시작 NPC
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700541) + L"</TH>\n"; // 진행 NPC
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700542) + L"</TH>\n"; // 완료 NPC
		kTemp += std::wstring(L"<TH>") + TextHelper::GetText(700556) + L"</TH>\n"; // 완료 조건(설명문)
		kTemp += std::wstring(L"</TR>\n");
		CONT_QUEST::const_iterator iter = rkCont.begin();
		while( rkCont.end() != iter )
		{
			BM::vstring kTempQuestInfo;
			CONT_QUEST::value_type const pkQuestInfo = (*iter);
			kTempQuestInfo << std::wstring(L"<TR>\n");
			kTempQuestInfo << std::wstring(L"<TD>#")<<BM::vstring(pkQuestInfo->ID())<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<MakeQuestLink(QLT_Title, pkQuestInfo)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<g_kCheckMgr.GetQuestTypeText(pkQuestInfo->Type())<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<TextHelper::GetText(pkQuestInfo->m_iGroupTextNo)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<BM::vstring(pkQuestInfo->m_kLimit.iMinLevel)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<BM::vstring(pkQuestInfo->m_kLimit.iMaxLevel)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<BM::vstring(pkQuestInfo->m_kLimit.iLimitMaxLevel)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<BM::vstring(pkQuestInfo->m_kReward.iExp)<<L"</TD>";
			kTempQuestInfo << std::wstring(L"<TD>")<<BM::vstring(pkQuestInfo->m_kReward.iMoney)<<L"</TD>";
			kTempQuestInfo += L"<TD>" + MakeLimitQuestName(QLT_PreQuest, pkQuestInfo) + L"</TD>";
			kTempQuestInfo += L"<TD>"; kTempQuestInfo += MakeNpcName(pkQuestInfo->m_kNpc_Client); kTempQuestInfo += L"</TD>\n"; // 시작 NPC
			kTempQuestInfo += L"<TD>"; kTempQuestInfo += MakeNpcName(pkQuestInfo->m_kNpc_Agent); kTempQuestInfo += L"</TD>\n"; // 시작 NPC
			kTempQuestInfo += L"<TD>"; kTempQuestInfo += MakeNpcName(pkQuestInfo->m_kNpc_Payer); kTempQuestInfo += L"</TD>\n"; // 시작 NPC
			kTempQuestInfo += L"<TD>"; kTempQuestInfo += MakeCompleteTargetString(pkQuestInfo); kTempQuestInfo += L"</TD>";  //완료 조건
			kTempQuestInfo += L"</TR>\n";

			kTemp += std::wstring(kTempQuestInfo);
			++iter;
		}
		kTemp += L"</table>\n";
		kTemp += L"</BODY><HTML>\n";
		return kTemp;
	}
	bool MakeFile(std::wstring const& rkFileName, std::wstring const& rkText)
	{
		if( rkText.empty() )
		{
			return false;
		}
		FILE* pFile = NULL;
		errno_t eRet = ::_wfopen_s(&pFile, rkFileName.c_str(), L"w,ccs=UNICODE");
		if( 0 != eRet
		||	pFile == NULL )
		{
			return false;
		}
		fwprintf_s(pFile, L"%s", rkText.c_str());
		::fclose(pFile);
		return true;
	}
	void MakeList(CONT_QUEST const& rkCont, std::wstring const& kFileName, STitleLink const& rkLink, STitleLink const& rkAddTitle)
	{
		MakeFile(kFileName, MakeList(rkCont, rkLink, rkAddTitle));
	}
	void MakeCSS(std::wstring const& rkPath)
	{
		std::wstring kFileName = rkPath + L"style.css";
		std::wstring kCSS =	L"@charset 'Unicode';\n"
							L"body {margin:4px; padding:0; color:#000000; font-size:12px; font-family:dotumche, sans-serif; background:#FFF;}\n"
							L"a:link, a:visited {color:#ff; text-decoration: none;}\n"
							L"a:hover, a:active {color: #444; text-decoration: underline;}\n"
							L"h1, h2, h3, h4, h5, h6 {margin:0; padding:0; font-size:12px;}\n"
							L"hr {display: none;}\n"
							L"th,td{border:1px solid #999;padding:5px;}\n"
							L"ol, ul {list-style-type: none;}\n"
							L"p, dl, dt, dd, ol, ul, li, form, fieldset, legend {margin: 0; padding: 0;}\n"
							L"img, fieldset, legend, input.input_img {border: 0; }\n"
							L"input { vertical-align:middle}\n"
							L"table {border-collapse: collapse;border-style:solid; border-spacing :5;border-width:1; border-color:777777; background-color:ffffff;}\n"
							L"th,td {empty-cells:show; font-size:12px; font-family:dotumche, sans-serif;}\n"
							L"img { vertical-align:middle}\n"
							L"input { vertical-align:middle;}\n"
							L"select { vertical-align:middle}\n";
		MakeFile(kFileName, kCSS);
	}
	void MakeInfo(PgQuestInfo const* pkQuestInfo, std::wstring const& rkPath)
	{
		if( !pkQuestInfo )
		{
			return;
		}
		wchar_t szFileName[MAX_PATH] = {0, };
		swprintf_s(szFileName, QUEST_KEY_NAME_FORMAT L".html", pkQuestInfo->ID());
		std::wstring const kTempFileName(szFileName);

		BM::vstring kTemp;
		{
			kTemp << L"<HTML><link href='style.css' rel='stylesheet' type='text/css'/><HEAD></HEAD><BODY>\n";
			kTemp << L"<TABLE BORDER='1' WIDTH='100%'>\n";
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700557) << L"</TH>\n"; // ------ 기본 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700529) << L"</TH><TD>" << BM::vstring(pkQuestInfo->ID()) << L"</TD></TR>\n"; // 퀘스트 ID
			kTemp << L"<TR><TH>" << TextHelper::GetText(700530) << L"</TH><TD>" << TextHelper::GetText(pkQuestInfo->m_iTitleTextNo) << L"</TD></TR>\n"; // 제목
			kTemp << L"<TR><TH>" << TextHelper::GetText(700531) << L"</TH><TD>" << g_kCheckMgr.GetQuestTypeText(pkQuestInfo->Type()) << L"</TD></TR>\n"; // 분류
			kTemp << L"<TR><TH>" << TextHelper::GetText(700532) << L"</TH><TD>" << TextHelper::GetText(pkQuestInfo->m_iGroupTextNo) << L"</TD></TR>\n"; // 그룹명
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700558) << L"</TH>\n"; // ------ 수락 제한 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700533) << L"</TH><TD>" << BM::vstring(pkQuestInfo->m_kLimit.iMinLevel) << L"</TD></TR>\n"; // 시작 레벨
			kTemp << L"<TR><TH>" << TextHelper::GetText(700534) << L"</TH><TD>" << BM::vstring(pkQuestInfo->m_kLimit.iMaxLevel) << L"</TD></TR>\n"; // 제한 레벨
			kTemp << L"<TR><TH>" << TextHelper::GetText(700535) << L"</TH><TD>" << (pkQuestInfo->m_kLimit.iLimitMaxLevel? TextHelper::GetText(700552): BM::vstring(pkQuestInfo->m_kLimit.iLimitMaxLevel)) << L"</TD></TR>\n"; // 제한 레벨(수락 불가능)
			kTemp << L"<TR><TH>" << TextHelper::GetText(700536) << L"</TH><TD>" << MakeLimitQuestName(QLT_NoAnchor, pkQuestInfo) << L"</TD></TR>\n"; // 선행 퀘스트
			kTemp << L"<TR><TH>" << TextHelper::GetText(700537) << L"</TH><TD>" << MakeClassName(pkQuestInfo->m_kLimit.iClassFlag) << L"</TD></TR>\n"; // 수락 가능 직업
			kTemp << L"<TR><TH>" << TextHelper::GetText(700538) << L"</TH><TD>" << MakeTimeLimitText(pkQuestInfo->m_kLimit.kLimitTime) << L"</TD></TR>\n"; // 수락 가능 날자
			kTemp << L"<TR><TH>" << TextHelper::GetText(700539) << L"</TH><TD>" << MakeWeekLimitText(pkQuestInfo->m_kLimit.kDayOfWeek) << L"</TD></TR>\n"; // 수락 가능 요일
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700559) << L"</TH>\n"; // ------ 달성 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700556) << L"</TH><TD>" << MakeCompleteTargetString(pkQuestInfo, true) << L"</TD></TR>\n"; // 완료 조건(설명문)
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700562) << L"</TH>\n"; // ------ NPC 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700540) << L"</TH><TD>" << MakeNpcName(pkQuestInfo->m_kNpc_Client, true) << L"</TD></TR>\n"; // 시작 NPC
			kTemp << L"<TR><TH>" << TextHelper::GetText(700541) << L"</TH><TD>" << MakeNpcName(pkQuestInfo->m_kNpc_Agent, true) << L"</TD></TR>\n"; // 진행 NPC
			kTemp << L"<TR><TH>" << TextHelper::GetText(700542) << L"</TH><TD>" << MakeNpcName(pkQuestInfo->m_kNpc_Payer, true) << L"</TD></TR>\n"; // 완료 NPC
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700561) << L"</TH>\n"; // ------ 보상 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700578) << L"</TH><TD>" << BM::vstring(pkQuestInfo->m_kReward.iMoney) << L"</TD></TR>\n"; // 보상 골드
			kTemp << L"<TR><TH>" << TextHelper::GetText(700579) << L"</TH><TD>" << BM::vstring(pkQuestInfo->m_kReward.iExp) << L"</TD></TR>\n"; // 보상 경험치
			//kTemp << L"<TR><TH>" << TextHelper::GetText(700544) << L"</TH><TD>" << BM::vstring(pkQuestInfo->m_kReward.iGuildExp) << L"</TD></TR>\n"; // 보상 길드 경험치
			kTemp << L"<TR><TH>" << TextHelper::GetText(700545) << L"</TH><TD>" << ((0 == pkQuestInfo->m_kReward.iSkillNo)? TextHelper::GetText(700552): DefStringHelper::GetSkillName(pkQuestInfo->m_kReward.iSkillNo)) << L"</TD></TR>\n"; // 보상 스킬
			kTemp << L"<TR><TH>" << TextHelper::GetText(700546) << L"</TH><TD>" << MakeRewardText(pkQuestInfo->m_kReward.kClassRewardSet1) << L"</TD></TR>\n"; // 보상 아이템1
			kTemp << L"<TR><TH>" << TextHelper::GetText(700547) << L"</TH><TD>" << MakeRewardText(pkQuestInfo->m_kReward.kClassRewardSet2) << L"</TD></TR>\n"; // 보상 아이템2
			kTemp << L"<TR><TH>" << TextHelper::GetText(700548) << L"</TH><TD>" << MakeChangeClassName(pkQuestInfo->m_kReward.kChangeClass) << L"</TD></TR>\n"; // 보상 전직
			kTemp << L"<TR><TH>" << TextHelper::GetText(700549) << L"</TH><TD>" << MakeEffectName(pkQuestInfo->m_kReward.kEffect) << L"</TD></TR>\n"; // 보상 이펙트
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700560) << L"</TH>\n"; // ------ 이벤트 정보
			{
				kTemp << L"<TR><TH>" << TextHelper::GetText(700543) << L"</TH><TD>";
				{
					if( !pkQuestInfo->m_kDepend_Npc.empty() )
					{
						ContQuestDependNpc::const_iterator iter = pkQuestInfo->m_kDepend_Npc.begin();
						while( pkQuestInfo->m_kDepend_Npc.end() != iter )
						{
							ContQuestDependNpc::mapped_type const& rkEvent = (*iter).second;
							switch( rkEvent.iType )
							{
							case QET_NPC_Client:
							case QET_NPC_Agent:
							case QET_NPC_Payer:
							case QET_NPC_Ing_Dlg:
								{
								}break;
							case QET_NPC_IncParam:
								{
									kTemp << L"- " << TextHelper::GetText(700609) << L"<BR>";
								}break;
							case QET_NPC_CheckGold:
								{
									kTemp << L"- " << TextHelper::GetText(700610) << L", " << rkEvent.iGold << L"<BR>";
								}break;
							case QET_NPC_GiveItem:
								{
									kTemp << L"- " << TextHelper::GetText(700611) << L", " << DefStringHelper::GetItemName(rkEvent.kItem.iItemNo) << L"(" << rkEvent.kItem.iItemNo << L")(*" << rkEvent.kItem.iCount << L")" << L"<BR>";
								}break;
							case QET_NPC_CheckItem:
								{
									kTemp << L"- " << TextHelper::GetText(700612) << L", " << DefStringHelper::GetItemName(rkEvent.kItem.iItemNo) << L"(" << rkEvent.kItem.iItemNo << L")(*" << rkEvent.kItem.iCount << L")" << L"<BR>";
								}break;
							case QET_NPC_RemoveItem:
								{
									kTemp << L"- " << TextHelper::GetText(700613) << L", " << DefStringHelper::GetItemName(rkEvent.kItem.iItemNo) << L"(" << rkEvent.kItem.iItemNo << L")(*" << rkEvent.kItem.iCount << L")" << L"<BR>";
								}break;
							case QET_NPC_CompleteQuest:
								{
									kTemp << L"- " << TextHelper::GetText(700614) << L", " << g_kCheckMgr.GetQuestTitleText(rkEvent.iVal1) << L"(" << rkEvent.iVal1 << L")" << L"<BR>";
								}break;
							case QET_NPC_AddGold:
								{
									kTemp << L"- " << TextHelper::GetText(700615) << L", " << rkEvent.iGold << L"<BR>";
								}break;
							case QET_NPC_CheckEffect:
								{
									kTemp << L"- " << TextHelper::GetText(700616) << L", " << DefStringHelper::GetEffectName(rkEvent.kItem.iItemNo) << L"(" << rkEvent.kItem.iItemNo << L")" << L"<BR>";
								}break;
							case QET_NPC_GiveEffect:
								{
									kTemp << L"- " << TextHelper::GetText(700617) << L", " << DefStringHelper::GetEffectName(rkEvent.kItem.iItemNo) << L"(" << rkEvent.kItem.iItemNo << L")" << L"<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Ground.empty() )
					{
						ContQuestGround::const_iterator iter = pkQuestInfo->m_kDepend_Ground.begin();
						while( pkQuestInfo->m_kDepend_Ground.end() != iter )
						{
							ContQuestGround::mapped_type const& rkEvent = (*iter).second;
							switch( rkEvent.iType )
							{
							case QET_GROUND_ComboCheck:
								{
									kTemp << L"- " << TextHelper::GetText(700594) << L", " << DefStringHelper::GetMapName(rkEvent.iGroundNo) << L"(" << rkEvent.iGroundNo << L") " << rkEvent.iVal1 << L"<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Monster.empty() )
					{
						ContQuestMonster::const_iterator iter = pkQuestInfo->m_kDepend_Monster.begin();
						while( pkQuestInfo->m_kDepend_Monster.end() != iter )
						{
							ContQuestMonster::mapped_type const& rkEvent = (*iter).second;
							switch( rkEvent.iType )
							{
							case QET_MONSTER_IncParam:
								{
									kTemp << L"- " << TextHelper::GetText(700595) << L", " << DefStringHelper::GetMonsterName(rkEvent.iClassNo) << L"(" << rkEvent.iClassNo << L")" << L"<BR>";
								}break;
							case QET_MONSTER_IncParam_In:
								{
									kTemp << L"- " << TextHelper::GetText(700595) << L", " << DefStringHelper::GetMonsterName(rkEvent.iClassNo) << L"(" << rkEvent.iClassNo << L"), " << DefStringHelper::GetMapName(rkEvent.iTargetGroundNo) << L"(" << rkEvent.iTargetGroundNo << L")<BR>";
								}break;
							case QET_MONSTER_InstDrop:
								{
									kTemp << L"- " << TextHelper::GetText(700596) << L", " << DefStringHelper::GetMonsterName(rkEvent.iClassNo) << L"(" << rkEvent.iClassNo << L"), " << DefStringHelper::GetItemName(rkEvent.iItemNo) << L"(" << rkEvent.iItemNo << L")(" << rkEvent.iVal2 << L"/" << rkEvent.iVal1 << L")(" << rkEvent.iVal4 << L"~" << rkEvent.iVal3 << L")<BR>";
								}break;
							case QET_MONSTER_InstDrop_In:
								{
									kTemp << L"- " << TextHelper::GetText(700596) << L", " << DefStringHelper::GetMonsterName(rkEvent.iClassNo) << L"(" << rkEvent.iClassNo << L"), " << DefStringHelper::GetItemName(rkEvent.iItemNo) << L"(" << rkEvent.iItemNo << L")(" << rkEvent.iVal2 << L"/" << rkEvent.iVal1 << L")(" << rkEvent.iVal4 << L"~" << rkEvent.iVal3 << L"), " << DefStringHelper::GetMapName(rkEvent.iTargetGroundNo) << L"(" << rkEvent.iTargetGroundNo << L")<BR>";
								}break;
							case QET_MONSTER_ComboCheck:
								{
									kTemp << L"- " << TextHelper::GetText(700594) << L", " << rkEvent.iVal1 << L"<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Item.empty() )
					{
						ContQuestDependItem::const_iterator iter = pkQuestInfo->m_kDepend_Item.begin();
						while( pkQuestInfo->m_kDepend_Item.end() != iter )
						{
							ContQuestDependItem::mapped_type const& rkEvent = (*iter).second;
							switch( rkEvent.iType )
							{
							case QET_ITEM_ChangeCount:
								{
									kTemp << L"- " << TextHelper::GetText(700597) << L", " << DefStringHelper::GetItemName(rkEvent.iItemNo) << L"(" << rkEvent.iItemNo << L")<BR>";
								}break;
							case QET_ITEM_Durability_Check:
								{
									kTemp << L"- " << TextHelper::GetText(700598) << L", " << DefStringHelper::GetItemName(rkEvent.iItemNo) << L"(" << rkEvent.iItemNo << L")<BR>";
								}break;
							case QET_ITEM_Plus_Check:
								{
									kTemp << L"- " << TextHelper::GetText(700599) << L", " << DefStringHelper::GetItemName(rkEvent.iItemNo) << L"(" << rkEvent.iItemNo << L")<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Location.empty() )
					{
						ContQuestLocation::const_iterator iter = pkQuestInfo->m_kDepend_Location.begin();
						while( pkQuestInfo->m_kDepend_Location.end() != iter )
						{
							ContQuestLocation::value_type const& rkEvent = (*iter);
							switch( rkEvent.iType )
							{
							case QET_LOCATION_LocationEnter:
								{
									kTemp << L"- " << TextHelper::GetText(700600) << L", " << DefStringHelper::GetMapName(rkEvent.iGroundNo) << L"(" << rkEvent.iGroundNo << L"), " << rkEvent.iLocationNo << L"<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Mission.empty() )
					{
						ContQuestDependMission::const_iterator iter = pkQuestInfo->m_kDepend_Mission.begin();
						while( pkQuestInfo->m_kDepend_Mission.end() != iter )
						{
							ContQuestDependMission::value_type const& rkEvent = (*iter);
							switch( rkEvent.iType )
							{
							case QET_MISSION_Rank:
								{
									kTemp << L"- " << TextHelper::GetText(700601) << L", (" << rkEvent.iMissionKey << L", " << rkEvent.iVal1 << L"), " << MakeRankText(rkEvent.iVal2) << L"<BR>";
								}break;
							case QET_MISSION_Percent:
								{
									kTemp << L"- " << TextHelper::GetText(700602) << L", (" << rkEvent.iMissionKey << L"), " << rkEvent.iVal1 << L"% <BR>";
								}break;
							case QET_MISSION_Clear:
								{
									kTemp << L"- " << TextHelper::GetText(700603) << L", (" << rkEvent.iMissionKey << L", " << rkEvent.iVal1 << L")<BR>";
								}break;
							case QET_MISSION_DEFENCE_CLEAR:
								{
									kTemp << L"- " << TextHelper::GetText(700646) << L", (" << rkEvent.iMissionKey << L"), Stage(" << rkEvent.iVal1 << L")<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Abil.empty() )
					{
						ContQuestDependAbil::const_iterator iter = pkQuestInfo->m_kDepend_Abil.begin();
						while( pkQuestInfo->m_kDepend_Abil.end() != iter )
						{
							ContQuestDependAbil::value_type const& rkEvent = (*iter);
							switch( rkEvent.iType )
							{
							case QET_ABIL_GREATER_EQUAL:
								{
									kTemp << L"- " << TextHelper::GetText(700647) << L", (AbilType:" << rkEvent.iAbilType << L", Value:" << rkEvent.iAbilValue << L")<BR>";
								}break;
							default:
								{
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Pvp.empty() )
					{
						ContQuestDependPvp::const_iterator iter = pkQuestInfo->m_kDepend_Pvp.begin();
						while( pkQuestInfo->m_kDepend_Pvp.end() != iter )
						{
							ContQuestDependPvp::value_type const& rkEvent = (*iter);
							switch( rkEvent.iType )
							{
							case QET_PVP_Win:
								{
									kTemp << L"- " << TextHelper::GetText(700604) << L"<BR>";
								}break;
							case QET_PVP_Lose:
								{
									kTemp << L"- " << TextHelper::GetText(700605) << L"<BR>";
								}break;
							case QET_PVP_Draw:
								{
									kTemp << L"- " << TextHelper::GetText(700606) << L"<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( !pkQuestInfo->m_kDepend_Class.empty() )
					{
						ContQuestDependClass::const_iterator iter = pkQuestInfo->m_kDepend_Class.begin();
						while( pkQuestInfo->m_kDepend_Class.end() != iter )
						{
							ContQuestDependClass::value_type const& rkEvent = (*iter);
							switch( rkEvent.iType )
							{
							case QET_CLASS_Change:
								{
									kTemp << L"- " << TextHelper::GetText(700607) << L", " << GetClassName(rkEvent.iClassNo) << L"(" << rkEvent.iClassNo << L")<BR>";
								}break;
							}
							++iter;
						}
						kTemp << L"<BR>";
					}
					if( 0 != pkQuestInfo->m_kDepend_Couple.iTime )
					{
						kTemp << L"- " << TextHelper::GetText(700628) << L", " << pkQuestInfo->m_kDepend_Couple.iTime << L"<BR>";
					}
					if( 0 != pkQuestInfo->m_kDepend_Time )
					{
						kTemp << L"- " << TextHelper::GetText(700629) << L", " << pkQuestInfo->m_kDepend_Time << L"<BR>";
					}
					if( 0 != pkQuestInfo->m_kDepend_KillCount.iKillCount )
					{
						kTemp << L"- " << TextHelper::GetText(700627) << L", " << pkQuestInfo->m_kDepend_KillCount.iKillCount << L"<BR>";
					}
					if( 0 != pkQuestInfo->m_kDepend_MyHome.iCount )
					{
						kTemp << L"- " << TextHelper::GetText(700608) << L", " << pkQuestInfo->m_kDepend_MyHome.iCount << L"<BR>";
					}
				}
				kTemp << L"</TD></TR>\n"; // 퀘스트 이벤트
			}
			kTemp << L"<TR><TH COLSPAN='2'>" << TextHelper::GetText(700563) << L"</TH>\n"; // ------ 기타 정보
			kTemp << L"<TR><TH>" << TextHelper::GetText(700550) << L"</TH><TD>" << ((pkQuestInfo->m_kReward.kAddEndQuest.empty())? TextHelper::GetText(700552): MakeQuestName(QLT_NoAnchor, pkQuestInfo->m_kReward.kAddEndQuest)) << L"</TD></TR>\n"; // 추가 완료 퀘스트
			kTemp << L"<TR><TH>" << TextHelper::GetText(700551) << L"</TH><TD>" << ((pkQuestInfo->m_kReward.kRealmQuetID.empty())? TextHelper::GetText(700552) : pkQuestInfo->m_kReward.kRealmQuetID) << L"</TD></TR>\n"; // 렐름 협동 퀘스트 설정
			kTemp << L"</TABLE>\n";
			kTemp << L"</BODY></HTML>\n";
		}

		MakeFile(rkPath + kTempFileName, std::wstring(kTemp));
	}
};

//-------------------------------------------------------------------------------------
// Global
//-------------------------------------------------------------------------------------
typedef BM::TObjectPool< PgQuestInfo > PoolQuest;
PoolQuest g_kQuestPool(100, 100);

CCheckMgr::CCheckMgr()
	: m_iQueryQuestParseXml(0), m_iCompleteQuestParseXml(0), m_iQueryMapParseXml(0), m_iCompleteMapParseXml(0), m_bReqVerify(false)
{
}

CCheckMgr::~CCheckMgr()
{
}

void CCheckMgr::Start(int const iLoadQuestNationCode, int const iMaxKillCountValue)
{
	BM::CAutoMutex kLock(m_kMutex);
	PgDBCache::m_bIsForTool = true;
	m_iMaxKillCountValue = iMaxKillCountValue; // 
	g_kCheckMgr.PutMsg( SCHECKMESSAGE(LT_LAOD_DB, WSTR_TABLE) ); // 작업의 시작은 DB로드 부터
}

void CCheckMgr::Clear()
{		
	BM::CAutoMutex kLock(m_kMutex);
	ResultHelper::Clear();
	WorldHelper::Clear();
	TextHelper::Clear();
	m_kContQuestInfo.clear();
	m_iQueryQuestParseXml = 0;
	m_iCompleteQuestParseXml = 0;
	m_iQueryMapParseXml = 0;
	m_iCompleteMapParseXml = 0;
	m_bReqVerify = false;
}


//
void CCheckMgr::HandleMessage(MSG *rkMsg)
{
	if(rkMsg)
	{
		switch(rkMsg->eType)
		{
		case LT_INIT:
			{				
				//	팩 옵션을 읽고
				BM::CPackInfo	kPackInfo;
				if(!kPackInfo.Init())
				{
					g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"PackInfo Init Failed."));
				}
				else
				{
					g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"PackInfo Init Success."));
				}

				//	매니져롤 초기화
				if(!BM::PgDataPackManager::Init(kPackInfo))
				{
					g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"PackManager Init Failed"));
				}
				else
				{
					g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"PackManager Init Success."));
				}
			}break;
 		case LT_LAOD_DB:
 			{
				if( LoadDump(g_bDBCon, rkMsg->wstrPath) )
				{
					TextHelper::Load();
					PutMsg( SCHECKMESSAGE(LT_QUEST_LIST_LOAD, g_Core.GetTargetFolder()) );
				}
				else
				{
					g_Core.SetWork(false);
				}
 			}break;
		case LT_LOAD_TEXT_TABLE:
		case LT_LOAD_QUEST_TEXT_TABLE:
			{
				LoadTextTable(rkMsg->wstrPath);
			}break;
		case LT_QUEST_LIST_LOAD:
			{
				LoadAllNpc();
				LoadQuestList(rkMsg->wstrPath);
				LoadMapList(rkMsg->wstrPath);
			}break;
		case LT_QUEST_XML_PARSE:
			{
				QuestParse(rkMsg->wstrPath);
			}break;
		case LT_MAP_XML_PARSE:
			{
				MapParse(rkMsg->wstrPath, rkMsg->iExternVal);
			}break;
		case LT_QUEST_XML_VERIFY:
			{
				Verify();
			}break;
		case LT_QTTV:
			{
				ResultHelper::GetQuestText(rkMsg->wstrPath);
			}break;
		}
		CheckerUtil::kObserverMgr.Process(rkMsg->eType);
	}
}

//-----------------------------------------------------------------------------------------------------
//	Load TB.BIN, TB2.BIN
//-----------------------------------------------------------------------------------------------------
bool CCheckMgr::LoadDump(bool const bFromDB, std::wstring const& wstrPath)
{
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"start DB Load......."));
	if( !bFromDB )
	{
		//기존 Dump Load
		if( g_kTblDataMgr.LoadDump(wstrPath) )
		{
			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"load DB Dump success"));
			return true;
		}
	}
	else
	{
		if( PgDBCache::TableDataQuery()
		&&	g_kTblDataMgr.Dump(wstrPath) )
		{
			g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"load DB and create Dump success"));
			return true;
		}
	}
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"load DB fail..."));
	return false;
}

//-----------------------------------------------------------------------------------------------------
//	텍스트 테이블 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadTextTable(std::wstring const& wstrPath)
{
	//	텍스트 테이블 로드
	if(wstrPath.size())
	{
		TiXmlDocument	TiDoc;
		if(TiDoc.LoadFile(MB(wstrPath)))
		{
			TiXmlElement*	TiElem = TiDoc.FirstChildElement();
			if(TiElem != NULL)
			{
				const TiXmlNode*	TiNode = (TiXmlNode *)TiElem;
				assert(TiNode);
				if(TextTableParse(TiNode))
				{
					g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, wstrPath + L" Text Table load completed."));
					return;
				}
			}
		}
	}
	//	여기까지 오면 에러
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Quest Text Table load failed."));
}

bool CCheckMgr::TextTableParse(const TiXmlNode* pkNode)
{
	const int iType	= pkNode->Type();

	while(pkNode)
	{
		switch(iType)
		{
		case TiXmlNode::ELEMENT:
			{
				TiXmlElement *pkElement = (TiXmlElement *)pkNode;
				assert(pkElement);

				const char *pcTagName = pkElement->Value();

				if( 0 == strcmp(pcTagName, "QUEST_TEXT_TABLE")
				||	0 == strcmp(pcTagName, "TEXT_TABLE") )
				{
					const TiXmlNode * pkChildNode = pkNode->FirstChild();
					if(pkChildNode != NULL)
					{
						if(!TextTableParse(pkChildNode))
						{
							return false;
						}
					}
				}
				else if(strcmp(pcTagName, "TEXT") == 0)
				{
					const TiXmlAttribute *pkAttr = pkElement->FirstAttribute();
					unsigned long	ulTextID	= 0;
					const char*		strText		= 0;
					while(pkAttr)//
					{
						const char *pcAttrName	= pkAttr->Name();
						const char *pcAttrValue	= pkAttr->Value();

						if(strcmp(pcAttrName, "ID") == 0)
						{
							ulTextID = (unsigned long)atol(pcAttrValue);
						}
						else if(strcmp(pcAttrName, "Text") == 0)
						{
							strText = pcAttrValue;
						}
						else
						{
							assert(!"속성값이 없습니다.");
						}

						pkAttr = pkAttr->Next();
					}				

					//	같은 아이디로 이미 텍스트가 존재하는지 체크한다.
					std::wstring kText = UNI(strText);
					if( ulTextID &&	!kText.empty() )
					{
						
						if( !TextHelper::AddText(ulTextID, kText) )//중복 되었으면
						{
							//	같은 아이디가 존재하면 안되는데, 존재한다 -,-;
							wchar_t szStr[100] = {0, };
							swprintf(szStr, 100, L"Text(ID:[%u]) is exist.", ulTextID);
							g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, szStr));
						}
					}
				}
			}break;
		}

		const TiXmlNode* pkNextNode = pkNode->NextSibling();
		pkNode = pkNextNode;
	}

	return	true;
}

void CCheckMgr::ChangeEnterMark(std::wstring& kText)
{
	while(true)
	{
		std::basic_string<wchar_t>::size_type	TxtIndex = kText.find(L"%%");
		if(TxtIndex == std::wstring::npos)
		{
			return;
		}

		kText.erase(TxtIndex, 2);
		kText.insert(TxtIndex, L"\r\n", 1);
	}
}


//-----------------------------------------------------------------------------------------------------
//	퀘스트 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadQuestList(std::wstring const& wstrPath)
{
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"XML Parsing.... This working for a minute or so."));

	CONT_DEF_QUEST_REWARD const* pkQuestReward = NULL;
	g_kTblDataMgr.GetContDef(pkQuestReward);
	CONT_DEF_QUEST_REWARD::const_iterator quest_iter = pkQuestReward->begin();
	while( pkQuestReward->end() != quest_iter )
	{
		CONT_DEF_QUEST_REWARD::mapped_type const& rkDefQuest = (*quest_iter).second;
		if( PgQuestInfoUtil::IsLoadXmlType(rkDefQuest.iDBQuestType) )
		{
			BM::CAutoMutex kLock(m_kMutex);
			++m_iQueryQuestParseXml;
			PutMsg(SCHECKMESSAGE(LT_QUEST_XML_PARSE, wstrPath + L"/XML/" + rkDefQuest.kXmlPath));
		}
		++quest_iter;
	}
}
void CCheckMgr::LoadMapList(std::wstring const& wstrPath)
{
	CONT_DEFMAP const* pkDefMap = NULL;
	g_kTblDataMgr.GetContDef(pkDefMap);
	CONT_DEFMAP::const_iterator iter = pkDefMap->begin();
	while( pkDefMap->end() != iter )
	{
		CONT_DEFMAP::mapped_type const& rkDefMap = (*iter).second;
		PutMsg(SCHECKMESSAGE(LT_MAP_XML_PARSE, wstrPath + L"/XML/" + rkDefMap.strXMLPath, rkDefMap.iMapNo));
		{
			BM::CAutoMutex kLock(m_kMutex);
			++m_iQueryMapParseXml;
		}
		++iter;
	}
}

//	폴더검사 리스트 생성
//bool CCheckMgr::GetQuestList(BM::FolderHash& Hash)
//{
//	BM::FolderHash	FileHash;
//	if(!BM::PgDataPackManager::GetFileList(WSTR_QUEST_XML, FileHash))
//	{
//		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"퀘스트 파일을 불러올 수 없습니다."));
//		return	false;
//	}
//
//	if(!FileHash.size())
//	{
//		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"퀘스트 파일이 존재하지 않습니다."));
//		return	false;
//	}
//
//	BM::FolderHash::iterator	iter = FileHash.begin();
//	while(iter != FileHash.end())
//	{
//		if(iter->second->kFileHash.size())
//		{
//			BM::PgFolderInfoPtr	pFolderInfo = NULL;
//			pFolderInfo = BM::PgFolderInfo::New();
//
//			BM::FileHash::iterator	File_iter = iter->second->kFileHash.begin();
//			while(File_iter != iter->second->kFileHash.end())
//			{
//				std::wstring	FileType = File_iter->first.substr(File_iter->first.size() - 3,  File_iter->first.size());
//				UPR( FileType );
//				if(wcscmp(FileType.c_str(), L"XML") == 0)
//				{
//					pFolderInfo->kFileHash.insert(std::make_pair(File_iter->first,File_iter->second));
//				}
//				
//				++File_iter;
//			}
//
//			if(pFolderInfo->kFileHash.size())
//			{
//				Hash.insert(std::make_pair(iter->first, pFolderInfo));
//			}
//		}
//
//		++iter;
//	}
//
//	if(!FileHash.size())
//	{
//		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"퀘스트 파일이 존재하지 않습니다."));
//		return	false;
//	}
//	return	true;
//}

void CCheckMgr::MapParse(std::wstring const& FileName, int const iMapNo)
{
	std::string const kXmlPath( MB(FileName) );
	TiXmlDocument kDocu(kXmlPath.c_str());
	if( kDocu.LoadFile() )
	{
		TiXmlElement *pkChild = kDocu.FirstChildElement("WORLD");
		if( pkChild )
		{
			if( !WorldHelper::ParseXml(pkChild, iMapNo) )
			{
				g_kLogMgr.PutMsg(SLOGMESSAGE( 0, LT_COMPLATE, FileName + L" Parse error.") );
			}
		}
		else
		{
			g_kLogMgr.PutMsg(SLOGMESSAGE( 0, LT_COMPLATE, FileName + L" wrong file.") );
		}
	}
	else
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE( 0, LT_COMPLATE, FileName + L" File dose not exist or wrong file.") );
	}

	{
		BM::CAutoMutex kLock(m_kMutex);
		++m_iCompleteMapParseXml;
		if( m_iQueryMapParseXml == m_iCompleteMapParseXml
		&&	m_iCompleteQuestParseXml == m_iQueryQuestParseXml
		&&	false == m_bReqVerify )
		{
			PutMsg(SCHECKMESSAGE(LT_QUEST_XML_VERIFY)); // 모든걸 다 로드 했을 때 검사 요청
			m_bReqVerify = true;
		}
	}
}

//	퀘스트 파싱
void CCheckMgr::QuestParse(std::wstring const& FileName)
{
	PgQuestInfo* pkQuest = g_kQuestPool.New();
	pkQuest->Clear();

	bool bSuccess = false;
	std::string const kXmlPath( MB(FileName) );
	TiXmlDocument kDocu(kXmlPath.c_str());
	if( kDocu.LoadFile() )
	{
		pkQuest->XmlFileName( kXmlPath );
		TiXmlElement *pkChild = kDocu.FirstChildElement("QUEST");
		bSuccess = pkQuest->ParseXml(pkChild);
		pkQuest->Build();

		ResultHelper::PgQuest	kQuest;
		kQuest.Path	= FileName;
		kQuest.Info	= *pkQuest;
		if( bSuccess ) // 파싱 성공
		{
			// m_kContQuestInfo는 Verify()의 인자
			auto ret;
			{
				BM::CAutoMutex kLock(m_kMutex);
				ret = m_kContQuestInfo.insert(std::make_pair(pkQuest->ID(), pkQuest));
			}
			bSuccess = ret.second;
			if( bSuccess )
			{
				AddSuccessQuest( kQuest );
			}
			else
			{
				g_kLogMgr.PutMsg( SLOGMESSAGE(pkQuest->ID(), LT_ERROR, BM::vstring(L"Duplicate quest id ")<<pkQuest->ID()<<L"in File["<<FileName<<L"]") );
			}
		}
		
		if( !bSuccess )
		{ // 파싱 실패
			AddQFailed( kQuest );
			ContErrorMsg::iterator iter = g_kErrorMsg.find( pkQuest->ID() );
			if( g_kErrorMsg.end() != iter )
			{
				ContErrorMsg::mapped_type const& rkContMsg = (*iter).second;
				ContErrorMsg::mapped_type::const_iterator msg_iter = rkContMsg.begin();
				while( rkContMsg.end() != msg_iter )
				{
					g_kLogMgr.PutMsg( SLOGMESSAGE(pkQuest->ID(), LT_ERROR, L"", SErrorData((*msg_iter).eType, (*msg_iter).iQuestID, (*msg_iter).kErrorMsg)) );
					++msg_iter;
				}
			}
		}
	}
	else// 파일이 없음
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE( 0, LT_COMPLATE, FileName + L"File dose not exist or wrong file.") );
	}

	{
		BM::CAutoMutex kLock(m_kMutex);
		++m_iCompleteQuestParseXml;
		if( !bSuccess )
		{
			g_kQuestPool.Delete( pkQuest ); // 실패시 여기로
		}

		if( m_iCompleteQuestParseXml == m_iQueryQuestParseXml
		&&	m_iQueryMapParseXml == m_iCompleteMapParseXml
		&&	false == m_bReqVerify)
		{
			PutMsg(SCHECKMESSAGE(LT_QUEST_XML_VERIFY)); // 모든걸 다 로드 했을 때 검사 요청
		}
	}
}

void CCheckMgr::MakeHelp()
{
	std::wstring const kFolder(L".\\QuestDocument\\");
	::_wmkdir(kFolder.c_str());

	HtmlUtil::MakeCSS(kFolder);

	HtmlUtil::CONT_QUEST kContSorted;
	{
		BM::CAutoMutex kLock(m_kMutex);
		QuestContainer::const_iterator iter = m_kContQuestInfo.begin();
		while( m_kContQuestInfo.end() != iter )
		{
			kContSorted.push_back( (*iter).second );
			
			HtmlUtil::MakeInfo((*iter).second, kFolder); // 개별 퀘스트 정보

			++iter;
		}
	}
	// 리스트 제작
	{
		HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
		HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_ID, false);
		HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_ID, true);
		kContSorted.sort(SortUtil::SortByID);
		HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kQuestID), kRLink, kTitle);
		//kContSorted.sort(SortUtil::SortByIDReverse);
		//HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kQuestID), kLink, kRTitle);
	}
	//{
	//	HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
	//	HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_TITLE, false);
	//	HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_TITLE, true);
	//	kContSorted.sort(SortUtil::SortByTitle);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kTitle), kRLink, kTitle);
	//	kContSorted.sort(SortUtil::SortByTitleReverse);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kTitle), kLink, kRTitle);
	//}
	//{
	//	HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
	//	HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_TYPE, false);
	//	HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_TYPE, true);
	//	kContSorted.sort(SortUtil::SortByType);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kType), kRLink, kTitle);
	//	kContSorted.sort(SortUtil::SortByTypeReverse);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kType), kLink, kRTitle);
	//}
	//{
	//	HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
	//	HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_GROUP, false);
	//	HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_GROUP, true);
	//	kContSorted.sort(SortUtil::SortByGroup);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kGroup), kRLink, kTitle);
	//	kContSorted.sort(SortUtil::SortByGroupReverse);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kGroup), kLink, kRTitle);
	//}
	//{
	//	HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
	//	HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_MIN_LEVEL, false);
	//	HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_MIN_LEVEL, true);
	//	kContSorted.sort(SortUtil::SortByMinLevel);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kMinLevel), kRLink, kTitle);
	//	kContSorted.sort(SortUtil::SortByMinLevelReverse);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kMinLevel), kLink, kRTitle);
	//}
	//{
	//	HtmlUtil::STitleLink kLink, kTitle, kRLink, kRTitle;
	//	HtmlUtil::MakeTitleLink(kLink, kTitle, HtmlUtil::QLST_BY_MAX_LEVEL, false);
	//	HtmlUtil::MakeTitleLink(kRLink, kRTitle, HtmlUtil::QLST_BY_MAX_LEVEL, true);
	//	kContSorted.sort(SortUtil::SortByMaxLevel);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kLink.kMaxLevel), kRLink, kTitle);
	//	kContSorted.sort(SortUtil::SortByMaxLevelReverse);
	//	HtmlUtil::MakeList(kContSorted, std::wstring(kFolder + kRLink.kMaxLevel), kLink, kRTitle);
	//}
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"export HTML complete."));
}

// 검증(데이터 체크)
void CCheckMgr::Verify()
{
	BM::CAutoMutex kLock(m_kMutex);
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Verifying.... This working for a minute or so."));

	// 파싱 성공한 퀘스트 검사
	QuestContainer::iterator iter = m_kContQuestInfo.begin();
	while( m_kContQuestInfo.end() != iter )
	{
		PgQuestInfoVerifyUtil::Verify( (*iter).second, m_kContQuestInfo, m_iMaxKillCountValue );
		++iter;
	}
	if( ResultHelper::Verify(m_kContQuestInfo) )
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Verify complete."));
		MakeHelp(); // 성공하면 HTML 생성
	}
	else
	{
		g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"Verify complete. but failed"));
		if( DevelopeFunction::iUseDebugDevelop )
		{
			MakeHelp(); // 개발팀은 실패해도 HTML 생성
		}
	}
	g_Core.SetWork(false);
}

std::wstring CCheckMgr::GetTT(int const iTextID) const
{
	return TextHelper::GetText(iTextID);
}
std::wstring CCheckMgr::GetQuestTitleText(int const iQuestID) const
{
	BM::CAutoMutex kLock(m_kMutex);
	QuestContainer::const_iterator find_iter = m_kContQuestInfo.find(iQuestID);
	if( m_kContQuestInfo.end() != find_iter )
	{
		return GetTT((*find_iter).second->m_iTitleTextNo);
	}
	return std::wstring();
}
std::wstring CCheckMgr::GetQuestTypeText(EQuestType const eType) const
{
	switch( eType )
	{
	case QT_Normal:			{ return GetTT(700513); }break;	//모험
	case QT_Scenario:		{ return GetTT(700514); }break;	//영웅
	case QT_ClassChange:	{ return GetTT(700515); }break;	//전직
	case QT_Loop:			{ return GetTT(700516); }break;	//반복
	case QT_Day:			{ return GetTT(700517); }break;	//보통 1일(5)
	case QT_GuildTactics:	{ return GetTT(700518); }break;	//길드 용병
	case QT_Couple:			{ return GetTT(700519); }break;	//커플
	case QT_SweetHeart:		{ return GetTT(700520); }break;	//부부
	case QT_Random:			{ return GetTT(700521); }break;	//헌터G 1일
	case QT_RandomTactics:	{ return GetTT(700522); }break;	//길드 1일
	case QT_Soul:			{ return GetTT(700523); }break;	//Soul
	case QT_Wanted:			{ return GetTT(700524); }break;	//현상수배
	case QT_Normal_Day:		{ return GetTT(700525); }break;	//더미
	case QT_Dummy:			{ return GetTT(700526); }break;	//싱글 1일(101)
	case QT_Deleted:		{ return GetTT(700527); }break;	//삭제
	default:
		{
		}break;
	}
	return GetTT(700528);	//새로운 타입(개발자에게 문의 요망)
}

//void	CCheckMgr::OutPut()
//{
//	BM::CAutoMutex kLock(m_kMutex);
//	HANDLE	hFile_s = CreateFile(L"QuestList_S.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	HANDLE	hFile_c = CreateFile(L"QuestList_C.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if( hFile_s == INVALID_HANDLE_VALUE || hFile_c == INVALID_HANDLE_VALUE )
//	{
//		return;
//	}
//
//	DWORD	dwRead;
//	std::string	TextType_one = "Quest%08d.xml\r\n";
//	std::string	TextType_two = "<QUEST ID=\"Quest%08d\">Quest/Quest%08d.xml</Quest>\r\n";
//
//	kQuestList::iterator	iter = m_kSuccessQuestList.begin();
//	while( iter != m_kSuccessQuestList.end() )
//	{
//		char	szText_s[MAX_PATH] = {0,};
//		char	szText_c[MAX_PATH] = {0,};
//		sprintf_s(szText_s, MAX_PATH, TextType_one.c_str(), iter->first);
//		sprintf_s(szText_c, MAX_PATH, TextType_two.c_str(), iter->first, iter->first);
//		WriteFile(hFile_s, szText_s, (DWORD)strlen(szText_s), &dwRead, NULL);
//		WriteFile(hFile_c, szText_c, (DWORD)strlen(szText_c), &dwRead, NULL);
//		++iter;
//	}
//
//	CloseHandle(hFile_s);
//	CloseHandle(hFile_c);
//
//	::MessageBox(g_hDlgWnd, L"퀘스트 리스트 목록을 저장하였습니다.\n위치 : 지정폴더\n파일 : QuestList_S.txt, QouestList_C.txt", L"알림", MB_OK);
//}

void	CCheckMgr::NotExistID()
{
	CONT_NOT_EXIST_ID	kList;

	ResultHelper::CheckExistIDToFailList(kList);
	ResultHelper::CheckExistIDToSuccessList(kList);

	if( !kList.size() )
	{
		return;
	}

	int const MAX_QUEST_ID = (*kList.rbegin());

	for(int i = 0; i < MAX_QUEST_ID; ++i)
	{
		CONT_NOT_EXIST_ID::iterator	iter = kList.find(i);
		if( iter == kList.end() )
		{
			wchar_t	szTemp[MAX_PATH] = {0,};
			_itow_s(i, szTemp, MAX_PATH, 10);
			g_Core.AddListItem(szTemp, OT_FAIL, CHK_EMPTYID);
		}
	}
}

void CCheckMgr::LoadAllNpc()
{
	g_kLogMgr.PutMsg(SLOGMESSAGE(0, LT_COMPLATE, L"All_Npc XML Parsing.... This working for a minute or so."));
	WorldHelper::LoadAllNpc(WSTR_ALL_NPC);
}