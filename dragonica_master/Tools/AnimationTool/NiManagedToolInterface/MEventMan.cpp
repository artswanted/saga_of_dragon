#include "stdafx.h"
#include "MEventMan.h"
#include "MSharedData.h"
#using <System.Xml.dll>

using namespace System::Xml;
using namespace System::Collections;
using namespace NiManagedToolInterface;

MEventMan::MEventMan()
{
	m_pkPathXmlContainer = NiExternalNew PathXmlContainer();
	m_pkPathContainer = NiExternalNew PathContainer();
}

MEventMan::~MEventMan()
{
}

const char *MEventMan::GetEventPath(const char *pcEventName)
{
	if(!m_pkPathContainer)
	{
		return 0;
	}

	PathContainer::iterator itr = m_pkPathContainer->find(pcEventName);
	if(itr == m_pkPathContainer->end())
	{
		return 0;
	}

	return itr->second.c_str();
}

ArrayList *MEventMan::GetEventPathList()
{
	if(!m_pkPathContainer)
	{
		return 0;
	}

	ArrayList *pkPathList = NiExternalNew ArrayList();
	PathContainer::iterator itr = m_pkPathContainer->begin();
	while(itr != m_pkPathContainer->end())
	{
		String *pkPath = itr->second.c_str();
		String *pkEventName = itr->first.c_str();
		DictionaryEntry kDic(pkEventName, pkPath);
		pkPathList->Add(__box(kDic));
		++itr;
	}

	return pkPathList;
}

void MEventMan::DeleteContents()
{
	m_pkPathContainer->clear();
	m_pkPathXmlContainer->clear();
}

String *MEventMan::GetLastEventPath()
{
	return m_pkLastAddedPath;
}

bool MEventMan::AddEventItems(String *pkPath, String *pkReqTag)
{
	if(!m_pkPathContainer || !m_pkPathXmlContainer)
	{
		return 0;
	}

	const char *pcPath = MStringToCharPointer(pkPath);
	std::string kPath(pcPath);
	MFreeCharPointer(pcPath);
	m_pkLastAddedPath = pkPath;

	DateTime kLastAccessTime = System::IO::File::GetLastAccessTime(pkPath);
	__int64 i64LastAccessTime = kLastAccessTime.ToFileTime();

	PathXmlContainer::iterator itr = m_pkPathXmlContainer->find(pcPath);
	if(itr != m_pkPathXmlContainer->end())
	{
		__int64 i64LoggedLastAccessTime = itr->second;
		if(i64LastAccessTime <= i64LoggedLastAccessTime)
		{
			return true;
		}

		(*m_pkPathXmlContainer)[kPath] = i64LastAccessTime;
	}
	else
	{
		m_pkPathXmlContainer->insert(std::make_pair(kPath, i64LastAccessTime));
	}

	XmlTextReader *pkReader = new XmlTextReader(pkPath);
	
	if(!pkReader)
	{
		assert(!"Can't load Xml File");
		return false;
	}

	pkReader->WhitespaceHandling = WhitespaceHandling::None;
	// 최상위 노드를 찾는다.
	while(pkReader->Read())
	{
		if(pkReader->NodeType == XmlNodeType::Element)
		{
			// 최상위 노드가 원하는 Tag가 아니면 종료
			if(pkReader->Name->CompareTo(pkReqTag->ToUpper()) != 0)
			{
				pkReader->Close();
				assert(!"Top Level Tag is invalid!!");
				return false;		
			}
			else
			{
				break;
			}
		}
	}

	while(pkReader->Read())
	{
		if(pkReader->NodeType == XmlNodeType::Element)
		{
			String *pkTagName = pkReader->Name;
			if(pkTagName->CompareTo(S"ITEM") == 0)
			{
				if(pkReader->MoveToAttribute(S"ID"))
				{
					const char *pcID = MStringToCharPointer(pkReader->Value->ToLower());
					std::string kID(pcID);
					std::string kPath;

					MFreeCharPointer(pcID);
					if(pkReader->Read() && pkReader->NodeType == XmlNodeType::Text)
					{
						const char *pcPath = MStringToCharPointer(pkReader->Value->ToLower());
						PgUtility::ToSuitableRelativePath(pcPath, RELATIVE_PATH, kPath);
						MFreeCharPointer(pcPath);

						if(!kID.empty() && !kPath.empty())
						{
							m_pkPathContainer->insert(std::make_pair(kID, kPath));
						}
					}
				}
			}
			else
			{
#ifdef _DEBUG
				char szMsg[512];
				const char *pcTagName = MStringToCharPointer(pkTagName);
				sprintf(szMsg, "Unknown Tag : %s", pkTagName);
				MFreeCharPointer(pcTagName);
//				NiMessageBox(szMsg, "Error");
#endif
			}
		}
	}

	pkReader->Close();
	
	return true;
}