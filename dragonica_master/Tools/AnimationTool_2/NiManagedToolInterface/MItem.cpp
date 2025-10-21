#include "stdafx.h"
#include "MItem.h"
#include "PgItem.h"
#include "PgItemMan.h"

using namespace NiManagedToolInterface;

MItem::MItem(PgItem *pkItem, PgItemMan *pkItemMan) :
	m_pkItem(pkItem),
	m_pkItemMan(pkItemMan),
	m_pkXmlPath(0)
{
}

MItem::MItem(String *pkXmlPath, PgItemMan *pkItemMan) : 
	m_pkXmlPath(pkXmlPath),
	m_pkItemMan(pkItemMan)
{
}

MItem::MItem(String *pkItemID, String *pkNIFPath) : 
	m_pkXmlPath(0)
{
	// Xml파일을 만들 때 쓰는 생성자. - 일단은 그냥 두자.
	const char *pcItemID = MStringToCharPointer(pkItemID);
	const char *pcNIFPath = MStringToCharPointer(pkNIFPath);

	m_pkItem = PgItem::CreateItem(pcItemID, pcNIFPath);

	MFreeCharPointer(pcItemID);
	MFreeCharPointer(pcNIFPath);
}

MItem::~MItem()
{
}

bool MItem::ParseItemXml(int iGender, int iClass, bool bForceParsing)
{
	if(!m_pkXmlPath)
	{
		return false;
	}
	
	const char *pcXmlPath = MStringToCharPointer(m_pkXmlPath);
	m_pkItem = m_pkItemMan->GetItem(pcXmlPath, iGender, iClass);
	m_pkItem = PgItem::CreateItemFromXmlFile(pcXmlPath, iGender, iClass, bForceParsing ? m_pkItem : 0);

	MFreeCharPointer(pcXmlPath);
	if(!m_pkItem)
	{
		return false;
	}

	// ItemMan에 추가/갱신한다.
	m_pkItemMan->AddItem(m_pkItem);
	return true;
}

String *MItem::get_Name()
{
	if(!m_pkItem)
	{
		return 0;
	}

	return m_pkItem->GetID();
}

void MItem::set_Name(String *kID)
{
	if(!m_pkItem)
	{
		return;
	}

	const char *pcID = MStringToCharPointer(kID);
	m_pkItem->SetID(pcID);
	MFreeCharPointer(pcID);
}

String *MItem::get_Path()
{
	if(!m_pkItem)
	{
		return m_pkXmlPath;
	}

	return m_pkItem->GetPath();
}

String *MItem::get_KFMPath()
{
	if(!m_pkItem)
	{
		return 0;
	}

	return m_pkItem->GetKFMPath();
}

String *MItem::get_NIFPath()
{
	if(!m_pkItem)
	{
		return 0;
	}

	return m_pkItem->GetMeshPath();
}

int MItem::get_ItemPos()
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->ItemPos();
}

bool MItem::get_Attached()
{
	if(!m_pkItem)
	{
		return 0;
	}
	
	return m_pkItem->IsAttached();
}

void MItem::set_Attached(bool bAttached)
{
	if(!m_pkItem)
	{
		return;
	}
	
	m_pkItem->SetAttached(bAttached);
}

PgItem *MItem::GetItem(int iGender, int iClass)
{
	const char *pcXmlPath = MStringToCharPointer(m_pkXmlPath);
	PgItem *pkItem = m_pkItemMan->GetItem(pcXmlPath, iGender, iClass);
	MFreeCharPointer(pcXmlPath);
	return pkItem;
}

void MItem::SetItem(PgItem *pkItem)
{
	m_pkItem = pkItem;
}

ArrayList *MItem::GetSrcTexture()
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->GetSrcTexture();
}

String *MItem::GetDestTexture(String *pkSrcTexture)
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->GetDestTexture(pkSrcTexture);
}

bool MItem::SetDestTexture(String *pkSrcTexture, String *pkDestTexture)
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->SetDestTexture(pkSrcTexture, pkDestTexture);
}

bool MItem::IsExistsTexture(String *pkDestTexture)
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->IsExistsTexture(pkDestTexture);
}

bool MItem::WriteToXml(String *pkXmlPath, bool bOverwrite)
{
	if(!m_pkItem)
	{
		return 0;
	}
	return m_pkItem->WriteToXml(pkXmlPath, bOverwrite);
}

bool MItem::ChangeMesh(String *pkNifPath)
{
	if(!m_pkItem)
	{
		return 0;
	}
	
	const char *pcNifPath = MStringToCharPointer(pkNifPath);
	bool bRet = m_pkItem->ChangeMesh(pcNifPath);
	MFreeCharPointer(pcNifPath);

	return bRet;
}

bool MItem::ChangeActorManager(String *pkKfmPath)
{
	if(!m_pkItem)
	{
		return 0;
	}
	
	const char *pcKfmPath = MStringToCharPointer(pkKfmPath);
	bool bRet = m_pkItem->ChangeActorManager(pcKfmPath);
	MFreeCharPointer(pcKfmPath);

	return bRet;
}