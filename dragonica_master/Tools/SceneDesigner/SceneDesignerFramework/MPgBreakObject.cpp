#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgBreakObject.h"

#using <System.Xml.dll>
using namespace System::Text;
using namespace System::Xml;
using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

const int ELEMENT_COUNT_IN_BAG = 10;

ObjectData::ObjectData()
{
}
ObjectData::~ObjectData()
{
}
GroupData::GroupData()
{
	m_strGroupName = "";
	m_pmObjectData = new ArrayList;
}
GroupData::~GroupData()
{
}

//////////////////////////////////////////////////////////////////////////
MPgBreakObject::MPgBreakObject(void) : 
	m_bAddGroup(false), m_bDeleteGroup(false)
{	
}
MPgBreakObject::~MPgBreakObject(void)
{
	if(m_pMakingData != NULL)
		delete m_pMakingData;
}

void MPgBreakObject::Initalize()
{
	m_pmGroupList = new ArrayList;
	//m_pMakingData = new GroupData;
	m_pMakingData = new MakingData;

	m_pmSettedBreakObjectList = new ArrayList;
	//m_pmDeletedBreakObjectList = new ArrayList;

	m_pmSettedElementList = new ArrayList;
	//m_pmNewElementList = new ArrayList;

	m_pmAllBagList = new ArrayList;
	//m_pmNewBagList = new ArrayList;

	// test
	m_strXmlPath = "D:\\Dragon_Pilot\\SFreedom_Dev\\XML";
}
void MPgBreakObject::Clear()
{
	if(m_pmGroupList)
	{
		for(int nn=0; nn<m_pmGroupList->Count; ++nn)
		{
			GroupData* pObjectList = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
			if(pObjectList->m_pmObjectData)
			{
				pObjectList->m_pmObjectData->Clear();
			}
		}

		m_pmGroupList->Clear();
	}

	m_pmSettedBreakObjectList->Clear();
	//m_pmDeletedBreakObjectList->Clear();

	m_pmSettedElementList->Clear();
	//m_pmNewElementList->Clear();

	m_pmAllBagList->Clear();
	//m_pmNewBagList->Clear();
}

bool MPgBreakObject::AddObjectData(int nObjectNumber, float fPosX, float fPosY, float fPosZ, String* strPath)
{
	//ObjectData* pData = new ObjectData;
	//pData->m_nIndex = m_pMakingData->m_pmObjectData->Count;
	//pData->m_nObjectNumber = nObjectNumber;
	//pData->m_fPosX = fPosX;
	//pData->m_fPosY = fPosY;
	//pData->m_fPosZ = fPosZ;
	//pData->m_strPath = strPath->ToString();

	//m_pMakingData->m_pmObjectData->Add(pData);
	return true;
}
bool MPgBreakObject::ModifyObjectData(int nIndex, int nObjectNumber, float fPosX, float fPosY, float fPosZ)
{
	//if(nIndex < 0)
	//	return false;
	// ObjectData* pData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nIndex));
	// if(!pData)
	//	 return false;
	// pData->m_nObjectNumber = nObjectNumber;
	// pData->m_fPosX = fPosX;
	// pData->m_fPosY = fPosY;
	// pData->m_fPosZ = fPosZ;

	 return true;
}
bool MPgBreakObject::RemoveObjectData(int nIndex)
{
	if(nIndex < 0)
		return false;

	if(!m_pMakingData->m_pmElementList)
		return false;

	m_pMakingData->m_pmElementList->RemoveAt(nIndex);
	//if(!m_pMakingData->m_pmObjectData)
	//	return false;

	//bool bRemove = false;
	//for(int nn=0; nn<m_pMakingData->m_pmObjectData->Count; ++nn)
	//{
	//	ObjectData* pData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nn));
	//	if(!pData)
	//		return false;

	//	if(pData->m_nIndex == nIndex)
	//	{
	//		m_pMakingData->m_pmObjectData->Remove(pData);
	//		bRemove = true;
	//		break;
	//	}
	//}

	//if(bRemove)
	//{// 인덱스 재 정렬
	//	for(int nn=0; nn<m_pMakingData->m_pmObjectData->Count; ++nn)
	//	{
	//		ObjectData* pData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nn));
	//		if(pData)
	//		{
	//			pData->m_nIndex = nn;
	//		}
	//	}
	//}
	return true;
}
void MPgBreakObject::ClearObjectData()
{
	//m_pMakingData->m_pmObjectData->Clear();
}
int MPgBreakObject::GetObjectDataObjectNumber(int nIndex)
{
	return 0;
	//if (nIndex == -1)
	//	return 0;
	//ObjectData* pData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nIndex));

	//if(!pData)
	//	return 0;

	//return pData->m_nObjectNumber;
}
float MPgBreakObject::GetObjectDataHeight(int nIndex)
{
	return 0;
	//if (nIndex == -1)
	//	return 0;

	//ObjectData* pData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nIndex));

	//if(!pData)
	//	return 0;

	//return pData->m_fPosZ;
}

bool MPgBreakObject::AddGroupData()
{
	//if(m_pMakingData->m_pmObjectData == NULL || m_pMakingData->m_strGroupName == NULL || m_pMakingData->m_strGroupName->Length == 0)
	//	return false;

	//for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	//{
	//	GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
	//	if(pData)
	//	{
	//		if(pData->m_strGroupName->Equals(m_pMakingData->m_strGroupName))
	//		{
	//			return false;
	//		}
	//	}
	//}

	//GroupData* pNewData = new GroupData;
	//pNewData->m_strGroupName = dynamic_cast<String*>(m_pMakingData->m_strGroupName->Clone());
	//pNewData->m_pmObjectData = dynamic_cast<ArrayList*>(m_pMakingData->m_pmObjectData->Clone());
	//pNewData->m_pkEntity = m_pMakingData->m_pkEntity;
	//m_pmGroupList->Add(pNewData);

	return true;
}
bool MPgBreakObject::AddGroupDataEntity(MEntity* pOrgEntity)
{
	// 팔레트를 로드한다.
	//MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	//if (pkPalette == NULL)
	//{
	//	::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
	//	return false;
	//}
	//String* strEntityName = "[General]Object.break_object";
	//MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	//if (pkTemplate == NULL)
	//{
	//	::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
	//	return false;
	//}

	//if(!m_pMakingData->m_strGroupName || m_pMakingData->m_strGroupName->Length == 0)
	//{
	//	::MessageBox(0, "GroupName 을 넣어주세요", 0, 0);
	//	return false;
	//}

	//for(int nCount=0; nCount<m_pmGroupList->Count; ++nCount)
	//{
	//	GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nCount));
	//	if(pData)
	//	{
	//		if(pData->m_strGroupName->Equals(m_pMakingData->m_strGroupName))
	//		{
	//			::MessageBox(0, "GroupName 이 중복되었습니다.", 0, 0);
	//			return false;
	//		}
	//	}
	//}

	//// 엔터티 생성
	//String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", m_pMakingData->m_strGroupName));
	//MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
	//if(!pkNewEntity)
	//	return false;

	//NiPoint3 kEntityPos;
	//pOrgEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
	//pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos);
	//pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
	//pkNewEntity->MasterEntity = pkTemplate;
	//m_pMakingData->m_pkEntity = pkNewEntity;

	//NiBool bBool;
	//NiObject* pkSceneRoot = NULL;
	//bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
	//if(!bBool)
	//{
	//	delete pkNewEntity;
	//	return false;
	//}

	//NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
	//if(!pkSceneNode)
	//	return false;
	//
	//int nCount = m_pMakingData->m_pmObjectData->Count;
	//for(int nn=0; nn<nCount; ++nn)
	//{
	//	ObjectData* pObjectData = dynamic_cast<ObjectData*>(m_pMakingData->m_pmObjectData->get_Item(nn));
	//	if(pObjectData)
	//	{
	//		String* strPath = pObjectData->m_strPath->ToString();
	//		if(strPath && m_strXmlPath)
	//		{
	//			String* strFilePath = String::Concat(m_strXmlPath->ToString(), strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
	//			const char* pcFilePath = MStringToCharPointer(strFilePath);
	//			NiStream kStream;
	//			kStream.Load(pcFilePath);
	//			MFreeCharPointer(pcFilePath);
	//			NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
	//			if(pkNode)
	//			{
	//				pkSceneNode->AttachChild(pkNode);
	//				pkNode->SetTranslate(pObjectData->m_fPosX, pObjectData->m_fPosY, pObjectData->m_fPosZ);
	//			}
	//		}
	//	}
	//}

	//pkSceneNode->UpdateProperties();
	//pkSceneNode->Update(0.0f);

	//MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

	return true;
}
MEntity* MPgBreakObject::AddGroupDataEntityFromLoad(NiPoint3 _kPos, String* _strName, ArrayList* pArray)
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return NULL;
	}
	String* strEntityName = "[General]Object.break_object";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
		return NULL;
	}

	// 엔터티 생성
	String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", _strName));
	MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
	if(!pkNewEntity)
		return NULL;

	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", _kPos);
	pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
	pkNewEntity->MasterEntity = pkTemplate;

	NiBool bBool;
	NiObject* pkSceneRoot = NULL;
	bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
	if(!bBool)
	{
		delete pkNewEntity;
		return NULL;
	}

	NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
	if(!pkSceneNode)
		return NULL;

	int nCount = pArray->Count;
	for(int nn=0; nn<nCount; ++nn)
	{
		ObjectData* pObjectData = dynamic_cast<ObjectData*>(pArray->get_Item(nn));
		if(pObjectData)
		{
			String* strPath = pObjectData->m_strPath->ToString();
			if(strPath && m_strXmlPath)
			{
				String* strFilePath = String::Concat(m_strXmlPath->ToString(), strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
				const char* pcFilePath = MStringToCharPointer(strFilePath);
				NiStream kStream;
				kStream.Load(pcFilePath);
				MFreeCharPointer(pcFilePath);
				NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
				if(pkNode)
				{
					pkSceneNode->AttachChild(pkNode);
					pkNode->SetTranslate(pObjectData->m_fPosX, pObjectData->m_fPosY, pObjectData->m_fPosZ);
				}
			}
		}
	}

	pkSceneNode->UpdateProperties();
	pkSceneNode->Update(0.0f);

	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

	return pkNewEntity;
}
void MPgBreakObject::RemoveGroupData(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_pmGroupList->Count)
		return;

	GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nIndex));
	if(!pData || !pData->m_pkEntity)
		return;

	MFramework::Instance->Scene->RemoveEntity(pData->m_pkEntity, false);
	m_pmGroupList->RemoveAt(nIndex);
	//if(strGroupName == NULL)
	//	return false;

	//for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	//{
	//	GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
	//	if(!pData)
	//		return false;

	//	if(pData->m_strGroupName->Equals(strGroupName))
	//	{
	//		m_pmGroupList->Remove(pData);
	//		return true;
	//	}
	//}
	//return false;
}
void MPgBreakObject::RemoveGroupData(MEntity* pkEntity)
{
	if(!pkEntity)
		return;

	for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	{
		GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
		if(!pData)
			return;

		if(pData->m_pkEntity == pkEntity)
		{
			m_pmGroupList->Remove(pData);
			return;
		}
	}
}
void MPgBreakObject::GroupListSelect(int nIndex)
{
	if (nIndex == -1)
		return;

	int nPos = 0;
	SettedObjectData* pData = NULL;;
	for(int i=0; i<m_pmSettedBreakObjectList->Count; ++i)
	{
		pData = dynamic_cast<SettedObjectData*>(m_pmSettedBreakObjectList->get_Item(i));
		if(!pData)
		{
			++nPos;
			continue;
		}

		if(pData->m_nChangeFlag == DCV_DELETE)
			continue;

		if(nPos == nIndex)
			break;

		++nPos;
	}

	if(!pData)
		return;
	
	ObjectBagData* pBag = GetObjectBagData(pData->m_iMonParentBagNo);
	if(!pBag)
		return;

	int nElement[ELEMENT_COUNT_IN_BAG]={0,};
	nElement[0] = pBag->m_nObjElement1;
	nElement[1] = pBag->m_nObjElement2;
	nElement[2] = pBag->m_nObjElement3;
	nElement[3] = pBag->m_nObjElement4;
	nElement[4] = pBag->m_nObjElement5;
	nElement[5] = pBag->m_nObjElement6;
	nElement[6] = pBag->m_nObjElement7;
	nElement[7] = pBag->m_nObjElement8;
	nElement[8] = pBag->m_nObjElement9;
	nElement[9] = pBag->m_nObjElement10;
	m_pMakingData->m_pmElementList->Clear();
	for(int nn=0; nn<ELEMENT_COUNT_IN_BAG; ++nn)
	{
		if(nElement[nn] == 0)
			break;

		ObjectElementData* pElement = GetObjectElementData(nElement[nn]);
		if(pElement)
		{
			m_pMakingData->m_pmElementList->Add(pElement);
		}
	}

	m_pMakingData->m_nBagNo = pData->m_iMonParentBagNo;
	m_pMakingData->m_nRegenPeriod = pData->m_iRegenPeriod;
	m_pMakingData->m_nPointGroup = pData->m_iPointGroup;
	m_pMakingData->m_pkEntity = pData->m_pkEntity;
}
bool MPgBreakObject::get_AddGroup()
{
	return m_bAddGroup;
}

void MPgBreakObject::set_AddGroup(bool bAdd)
{
	m_bAddGroup = bAdd;
}
bool MPgBreakObject::get_DeleteGroup()
{
	return m_bDeleteGroup;
}
void MPgBreakObject::set_DeleteGroup(bool bDelete)
{
	m_bDeleteGroup = bDelete;
}
void MPgBreakObject::TranslationGroupData(MEntity* pkEntity, NiPoint3 kDelta)
{
	//if(pkEntity == NULL)
	//	return;

	//for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	//{
	//	GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
	//	if(pData)
	//	{
	//		if(pData->m_pkEntity == pkEntity)
	//		{
	//		}
	//	}
	//}
}
bool MPgBreakObject::ModifyGroupData(String* strGroupName)
{
	if(strGroupName == NULL || strGroupName->Length == 0)
		return false;

	for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	{
		GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
		if(pData)
		{
			if(pData->m_strGroupName->Equals(strGroupName))
			{
				//MEntity* pkEntity = pData->m_pkEntity;
				//pData->m_strGroupName = dynamic_cast<String*>(m_pMakingData->m_strGroupName->Clone());
				//pData->m_pmObjectData = dynamic_cast<ArrayList*>(m_pMakingData->m_pmObjectData->Clone());
			}
		}
	}

	return true;
}
String* MPgBreakObject::GetGroupDataGroupName(int nIndex)
{
	if(nIndex >= 0)
	{
		GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nIndex));
		if(pData)
		{
			return pData->m_strGroupName;
		}
	}

	return NULL;
}
bool MPgBreakObject::SaveXml()
{
	XmlDocument* pkDocument = new XmlDocument();
	if(!pkDocument)
		return false;

	pkDocument->PreserveWhitespace = true;

	// Version
    XmlDeclaration* newDec = pkDocument->CreateXmlDeclaration("1.0", "euc-kr", 0);
	if(!newDec)
		return false;
    pkDocument->AppendChild(newDec);

	// Root
	XmlElement* newRoot = pkDocument->CreateElement("BREAKOBJECT");
    newRoot->SetAttribute("ID", "breakobject");// test
    pkDocument->AppendChild(newRoot);

	// Entity 수
	for(int nn=0; nn<m_pmGroupList->Count; ++nn)
	{
		GroupData* pData = dynamic_cast<GroupData*>(m_pmGroupList->get_Item(nn));
		if(!pData || !pData->m_pmObjectData)
			continue;

		XmlElement* pElement = pkDocument->CreateElement("GROUPNAME");
		if(!pElement)
			continue;

		pElement->InnerText = "";
		pElement->SetAttribute("NAME", pData->m_strGroupName);
		NiPoint3 kTrans;
		pData->m_pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kTrans);
		char szTmp[100]={0,};
		sprintf(szTmp, "%f", kTrans.x);
		pElement->SetAttribute("POSX", szTmp);
		newRoot->AppendChild(pElement);
		sprintf(szTmp, "%f", kTrans.y);
		pElement->SetAttribute("POSY", szTmp);
		newRoot->AppendChild(pElement);
		sprintf(szTmp, "%f", kTrans.z);
		pElement->SetAttribute("POSZ", szTmp);
		newRoot->AppendChild(pElement);

		for(int n1=0; n1<pData->m_pmObjectData->Count; ++n1)
		{
			ObjectData* pObjectData = dynamic_cast<ObjectData*>(pData->m_pmObjectData->get_Item(n1));
			if(!pObjectData || !pObjectData->m_strPath || pObjectData->m_strPath->Length == 0)
				continue;

			XmlElement* pChild = pkDocument->CreateElement("OBJECT");
			if(!pChild)
				continue;

			char szNum[20]={0,};
			// number
			_itoa_s(pObjectData->m_nObjectNumber, szNum, 10);
			pChild->SetAttribute("NUMBER", szNum);
			// index
			_itoa_s(pObjectData->m_nIndex, szNum, 10);
			pChild->SetAttribute("INDEX", szNum);
			// PosX
			sprintf(szNum, "%f", pObjectData->m_fPosX);
			pChild->SetAttribute("POSX", szNum);
			// PosY
			sprintf(szNum, "%f", pObjectData->m_fPosY);
			pChild->SetAttribute("POSY", szNum);
			// PosZ
			sprintf(szNum, "%f", pObjectData->m_fPosZ);
			pChild->SetAttribute("POSZ", szNum);
			// path
			pChild->SetAttribute("PATH", pObjectData->m_strPath);
			pElement->AppendChild(pChild);
		}
	}

	// Save
	XmlTextWriter* tr = new XmlTextWriter(m_strFileName, Encoding::GetEncoding(949));
	tr->Formatting = Formatting::Indented;
	pkDocument->WriteContentTo(tr);
	tr->Close();

	return true;
}
void MPgBreakObject::LoadXml()
{
	Clear();

	XmlDocument* pkDocument = new XmlDocument();
	pkDocument->Load(m_strFileName);
	pkDocument->PreserveWhitespace = true;

	// 내용 채워넣자.
	
	XmlElement* pRoot = pkDocument->DocumentElement;
	XmlNodeList* nodeList = pkDocument->GetElementsByTagName("GROUPNAME");
	if(!nodeList)
		return;

	for(int nn=0; nn<nodeList->Count; ++nn)
	{
		GroupData* pGroupData = new GroupData;
		if(!pGroupData)
			return;

		XmlNode* pkNode = nodeList->get_ItemOf(nn);
		if(pkNode)
		{
			pGroupData->m_strGroupName = pkNode->Attributes->GetNamedItem("NAME")->InnerText;
			String* strTemp = "";
			NiPoint3 kPos;
			float* pfTemp = new float;
			if(!pfTemp)
				continue;
			strTemp = pkNode->Attributes->GetNamedItem("POSX")->InnerText;
			Single::TryParse(strTemp, pfTemp);
			kPos.x = *pfTemp;
			strTemp = pkNode->Attributes->GetNamedItem("POSY")->InnerText;
			Single::TryParse(strTemp, pfTemp);
			kPos.y = *pfTemp;
			strTemp = pkNode->Attributes->GetNamedItem("POSZ")->InnerText;
			Single::TryParse(strTemp, pfTemp);
			kPos.z = *pfTemp;
			for(int mm=0; mm<pkNode->ChildNodes->Count; ++mm)
			{
				ObjectData* pObjectData = new ObjectData;
				if(!pObjectData)
					continue;

				XmlNode* pkChild = pkNode->ChildNodes->get_ItemOf(mm);
				if(pkChild)
				{
					int* pnTemp = new int;
					strTemp = pkChild->Attributes->GetNamedItem("NUMBER")->InnerText;
					Int32::TryParse(strTemp, pnTemp);
					pObjectData->m_nObjectNumber = *pnTemp;

					strTemp = pkChild->Attributes->GetNamedItem("INDEX")->InnerText;
					Int32::TryParse(strTemp, pnTemp);
					pObjectData->m_nIndex = *pnTemp;

					strTemp = pkChild->Attributes->GetNamedItem("POSX")->InnerText;
					Single::TryParse(strTemp, pfTemp);
					pObjectData->m_fPosX = *pfTemp;

					strTemp = pkChild->Attributes->GetNamedItem("POSY")->InnerText;
					Single::TryParse(strTemp, pfTemp);
					pObjectData->m_fPosY = *pfTemp;

					strTemp = pkChild->Attributes->GetNamedItem("POSZ")->InnerText;
					Single::TryParse(strTemp, pfTemp);
					pObjectData->m_fPosZ = *pfTemp;

					strTemp = pkChild->Attributes->GetNamedItem("PATH")->InnerText;
					pObjectData->m_strPath = strTemp->ToString();
					delete pnTemp;
				}
				pGroupData->m_pmObjectData->Add(pObjectData);
			}
			AddGroupDataEntityFromLoad(kPos, pGroupData->m_strGroupName, pGroupData->m_pmObjectData);
			m_pmGroupList->Add(pGroupData);

			delete pfTemp;
		}
	}
}
MEntity* MPgBreakObject::AddSettedBreakObjectData(String* strBagName, Guid kGuid, String *strMemo, int iMapNo, int iMonParentBagNo, int iPointGroup, 
							  int iRegenPeriod, float fPosX, float fPosY, float fPosZ, int iRotAxZ, int iMoveRange, String* strGroupNum)
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return NULL;
	}
	String* strEntityName = "[General]Object.break_object";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
		return NULL;
	}

	// 엔터티 생성
	String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", strBagName));
	MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
	if(!pkNewEntity)
		return NULL;
	pkNewEntity->TemplateID = kGuid;
	NiPoint3 kPos(fPosX, fPosY, fPosZ);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPos);
	pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
	pkNewEntity->MasterEntity = pkTemplate;

	NiBool bBool;
	NiObject* pkSceneRoot = NULL;
	bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
	if(!bBool)
	{
		delete pkNewEntity;
		return NULL;
	}

	NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
	if(!pkSceneNode)
	{
		delete pkNewEntity;
		return NULL;
	}

	ObjectBagData* pData = GetObjectBagData(iMonParentBagNo);
	if(!pData)
	{
		delete pkNewEntity;
		return NULL;
	}

	int nElement[ELEMENT_COUNT_IN_BAG]={0,};
	nElement[0] = pData->m_nObjElement1;
	nElement[1] = pData->m_nObjElement2;
	nElement[2] = pData->m_nObjElement3;
	nElement[3] = pData->m_nObjElement4;
	nElement[4] = pData->m_nObjElement5;
	nElement[5] = pData->m_nObjElement6;
	nElement[6] = pData->m_nObjElement7;
	nElement[7] = pData->m_nObjElement8;
	nElement[8] = pData->m_nObjElement9;
	nElement[9] = pData->m_nObjElement10;
	for(int nEl=0; nEl<ELEMENT_COUNT_IN_BAG; ++nEl)
	{
		if(nElement[nEl] == 0)
			break;

		ObjectElementData* pElementData = GetObjectElementData(nElement[nEl]);
		if(!pElementData)
			continue;

		String* strPath = pElementData->m_strPath->ToString();
		if(strPath && m_strXmlPath)
		{
			String* strFilePath = String::Concat(m_strXmlPath->ToString(), "\\", strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
			const char* pcFilePath = MStringToCharPointer(strFilePath);
			//String* strFilePath = String::Concat(m_strXmlPath->ToString(), strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
			//const char* pcFilePath = MStringToCharPointer(strFilePath);
			NiStream kStream;
			bool bRtn = kStream.Load(pcFilePath);
			if(!bRtn)
			{
				::MessageBox(0, pcFilePath, "Nif file load fail", 0);
				MFreeCharPointer(pcFilePath);
				return NULL;
			}
			MFreeCharPointer(pcFilePath);
			NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
			if(pkNode)
			{
				pkSceneNode->AttachChild(pkNode);
				pkNode->SetTranslate(pElementData->m_fRelativeX, pElementData->m_fRelativeY, pElementData->m_fRelativeZ);
			}
		}
	}

	pkSceneNode->UpdateProperties();
	pkSceneNode->Update(0.0f);

	if(iNotRotated!=iRotAxZ)
	{
		float fRotResult = static_cast<float>(iRotAxZ) * (static_cast<float>(Math::PI)/180.0f) * 2.0f;//회전값(호도법)
		NiQuaternion kQuat;
		NiMatrix3 kRot;
		kRot.FromEulerAnglesXYZ(0.0f, 0.0f, fRotResult);
		//kQuat.FromAngleAxis(fRotResult, kPoint);
		//kQuat.ToRotation(kRot);
		pkNewEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRot);
	}

	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

	SettedObjectData *pkData = new SettedObjectData;
	pkData->Initialize(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, 
		iRegenPeriod, fPosX, fPosY, fPosZ, iRotAxZ, iMoveRange, strGroupNum, DCV_NORMAL);
	pkData->m_pkEntity = pkNewEntity;

	m_pmSettedBreakObjectList->Add(pkData);

	return pkNewEntity;
}
int MPgBreakObject::DeleteSettedBreakObjectData(Guid kGuid)
{
	int nRtn = 0;
	for (int i=0 ; i<m_pmSettedBreakObjectList->Count ; i++)
	{
		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(i));

		if (pkData && pkData->m_kGuid == kGuid)
		{
			if(pkData->m_nChangeFlag == DCV_INSERT)
			{
				m_pmSettedBreakObjectList->RemoveAt(i);
			}
			else
			{
				pkData->m_nChangeFlag = DCV_DELETE;
			}
			nRtn = pkData->m_iMonParentBagNo;
			break;
		}
	}

	return nRtn;
}
void MPgBreakObject::ModifySettedBreakObjectPos(MEntity* pkEntity, NiPoint3 kPosition)
{
	if(!pkEntity)
		return;

	Guid kGuid = pkEntity->TemplateID;
	for (int i=0 ; i<m_pmSettedBreakObjectList->Count ; i++)
	{
		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(i));

		if (pkData && pkData->m_kGuid == kGuid)
		{
			if(pkData->m_nChangeFlag != DCV_INSERT)
				pkData->m_nChangeFlag = DCV_UPDATE;

			pkData->m_fPosX = kPosition.x;
			pkData->m_fPosY = kPosition.y;
			pkData->m_fPosZ = kPosition.z;
			break;
		}
	}
}
//Guid MPgBreakObject::GetDeletedBreakObjectGuidData(int iDeletedMonsterIdx)
//{
//	SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmDeletedBreakObjectList->get_Item(iDeletedMonsterIdx));
//
//	return pkData->m_kGuid;
//}
//void MPgBreakObject::DeleteToDeletedBreakObjectGuidData(Guid kGuid)
//{
//	for (int i=0 ; i<m_pmDeletedBreakObjectList->Count ; i++)
//	{
//		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmDeletedBreakObjectList->get_Item(i));
//		if (pkData->m_kGuid == kGuid)
//		{
//			m_pmDeletedBreakObjectList->Remove(pkData);
//			break;
//		}
//	}
//}
MPoint3* MPgBreakObject::GetBreakObjectPos(MEntity* pkEntity)
{
	MPoint3 *pkPoint = new MPoint3(0,0,0);
	NiPoint3 kPoint;

	for (int i=0 ; i<m_pmSettedBreakObjectList->Count ; i++)
	{
		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			pkData->m_pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			pkPoint->X = kPoint.x;
			pkPoint->Y = kPoint.y;
			pkPoint->Z = kPoint.z;

			return pkPoint;
		}
	}

	return pkPoint;
}
MPoint3* MPgBreakObject::GetBreakObjectPos(int nBagNo)
{
	MPoint3 *pkPoint = new MPoint3(0,0,0);
	NiPoint3 kPoint;

	for (int i=0 ; i<m_pmSettedBreakObjectList->Count ; i++)
	{
		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(i));

		if (pkData->m_iMonParentBagNo == nBagNo)
		{
			pkData->m_pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			pkPoint->X = kPoint.x;
			pkPoint->Y = kPoint.y;
			pkPoint->Z = kPoint.z;

			return pkPoint;
		}
	}

	return pkPoint;
}

int	MPgBreakObject::GetBreakObjectRot(MEntity* pkEntity)
{
	if(NULL!=pkEntity)
	{
		NiMatrix3 kRot;
		NiPoint3 kPoint;
		float fRotResult=0.0f;
		pkEntity->GetNiEntityInterface()->GetPropertyData("Rotation", kRot);
		kRot.ExtractAngleAndAxis(fRotResult, kPoint.x, kPoint.y, kPoint.z);
		fRotResult = fRotResult * (180.0f/static_cast<float>(Math::PI));//라디안 -> 호도법
		
		float const fFloatEp = 0.001f;
		if( !MPgUtil::IsEqualF(kPoint.z, 1.0f, fFloatEp) && !MPgUtil::IsEqualF(kPoint.z, -1.0f, fFloatEp) )
		{//Z축에 대해 변환된 적이 없으면, 즉 사용자가 툴에서 몬스터의 쳐다보는 방향을 변경한 적이 없으면...
			return iNotRotated;
		}
		else
		{
			kRot.ToEulerAnglesXYZ(kPoint.x, kPoint.y, kPoint.z);
			kPoint.z = kPoint.z * (180.0f/static_cast<float>(Math::PI));
			if(0.0f > kPoint.z)
			{
				kPoint.z = 360.0f + kPoint.z; 
			}
			return (static_cast<int>(kPoint.z) / 2);
		}
	}
	return iNotRotated;
}
void MPgBreakObject::ClearSettedObjectData()
{
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];

		if (IsBreakObjctData(pkEntity->TemplateID.ToString()))
		{
			MFramework::Instance->Scene->RemoveEntity(pkEntity,false);
			uiEntityCount -= 1;
			i -= 1;
		}
	}
	m_pmSettedBreakObjectList->RemoveRange(0, m_pmSettedBreakObjectList->Count);
	//m_iEntityCount = 0;
}
bool MPgBreakObject::IsBreakObjctData(String *strName)
{
	if (strName->Equals("[General]Object.break_object"))
		return true;

	for (int i=0 ; i<m_pmSettedBreakObjectList->Count ; i++)
	{
		SettedObjectData *pkData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(i));

		if (pkData->m_kGuid.ToString()->Equals(strName))
		{
			return true;
		}
	}

	return false;
}
bool MPgBreakObject::AddElementData(int nElementNo, int nObjectNo, float fPosX, float fPosY, float fPosZ, String* strPath)
{
	for(int nn=0; nn<m_pmSettedElementList->Count; ++nn)
	{
		ObjectElementData* pElement = dynamic_cast<ObjectElementData*>(m_pmSettedElementList->get_Item(nn));
		if(pElement && pElement->m_nElementNo == nElementNo)
		{
			if(pElement->m_nObjectNo == nObjectNo && pElement->m_fRelativeX == fPosX
				&& pElement->m_fRelativeY == fPosY && pElement->m_fRelativeZ == fPosZ)
			{
				m_pMakingData->m_pmElementList->Add(pElement);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	ObjectElementData* pData = new ObjectElementData;
	if(!pData)
		return false;

	pData->Initialize(nElementNo, nObjectNo,  fPosX,fPosY, fPosZ, strPath, DCV_INSERT);
	m_pMakingData->m_pmElementList->Add(pData);
	return true;
}
void MPgBreakObject::AddSettedElementList(int nElementNo, int nObjectNo, float fRelativeX, float fRelativeY, float fRelativeZ, String* strPath)
{
	ObjectElementData* pData = new ObjectElementData;
	if(!pData)
		return;
	pData->Initialize(nElementNo, nObjectNo, fRelativeX, fRelativeY, fRelativeZ, strPath, DCV_NORMAL);
	m_pmSettedElementList->Add(pData);
}
bool MPgBreakObject::AddNewElemntList(int nElementNo, int nObjectNo, float fRelativeX, float fRelativeY, float fRelativeZ, String* strPath)
{
	ObjectElementData* pData = GetObjectElementData(nElementNo);
	if(pData)
	{
		if(pData->m_nChangeFlag == DCV_INSERT)
			pData->Initialize(nElementNo, nObjectNo, fRelativeX, fRelativeY, fRelativeZ, strPath, DCV_INSERT);
		else
			pData->Initialize(nElementNo, nObjectNo, fRelativeX, fRelativeY, fRelativeZ, strPath, DCV_UPDATE);
	}
	else
	{
		pData = new ObjectElementData;
		if(!pData)
			return false;
		pData->Initialize(nElementNo, nObjectNo, fRelativeX, fRelativeY, fRelativeZ, strPath, DCV_INSERT);
		m_pmSettedElementList->Add(pData);
	}

	return true;
}

ObjectBagData* MPgBreakObject::AddAllBagList(int nBagNo, int nObjElement1, int nObjElement2, int nObjElement3, int nObjElement4
					  , int nObjElement5, int nObjElement6, int nObjElement7, int nObjElement8, int nObjElement9, int nObjElement10)
{
	ObjectBagData* pData = new ObjectBagData;
	if(!pData)
		return NULL;
	pData->Initialize(nBagNo, nObjElement1, nObjElement2, nObjElement3, nObjElement4
		, nObjElement5, nObjElement6, nObjElement7, nObjElement8, nObjElement9, nObjElement10, DCV_NORMAL);
	m_pmAllBagList->Add(pData);

	return pData;
}
ObjectBagData* MPgBreakObject::AddNewBagList(int nBagNo, int nObjElement1, int nObjElement2, int nObjElement3, int nObjElement4
				   , int nObjElement5, int nObjElement6, int nObjElement7, int nObjElement8, int nObjElement9, int nObjElement10)
{
	ObjectBagData* pSetted;
	for(int nn=0; nn<m_pmAllBagList->Count; ++nn)
	{
		pSetted = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(nn));
		if(!pSetted)
			continue;

		if(pSetted->m_nBagNo == nBagNo)
		{
			if(pSetted->m_nChangeFlag == DCV_DELETE)
			{
				pSetted->m_nChangeFlag = DCV_UPDATE;
				pSetted->m_nObjElement1 = nObjElement1;
				pSetted->m_nObjElement2 = nObjElement2;
				pSetted->m_nObjElement3 = nObjElement3;
				pSetted->m_nObjElement4 = nObjElement4;
				pSetted->m_nObjElement5 = nObjElement5;
				pSetted->m_nObjElement6 = nObjElement6;
				pSetted->m_nObjElement7 = nObjElement7;
				pSetted->m_nObjElement8 = nObjElement8;
				pSetted->m_nObjElement9 = nObjElement9;
				pSetted->m_nObjElement10 = nObjElement10;

				return pSetted;
			}
			else if(pSetted->m_nChangeFlag == DCV_INSERT)
			{
				pSetted->m_nChangeFlag = DCV_INSERT;
				pSetted->m_nObjElement1 = nObjElement1;
				pSetted->m_nObjElement2 = nObjElement2;
				pSetted->m_nObjElement3 = nObjElement3;
				pSetted->m_nObjElement4 = nObjElement4;
				pSetted->m_nObjElement5 = nObjElement5;
				pSetted->m_nObjElement6 = nObjElement6;
				pSetted->m_nObjElement7 = nObjElement7;
				pSetted->m_nObjElement8 = nObjElement8;
				pSetted->m_nObjElement9 = nObjElement9;
				pSetted->m_nObjElement10 = nObjElement10;

				return pSetted;
			}
			else
			{
				return NULL;
			}
		}
	}

	ObjectBagData* pData = new ObjectBagData;
	if(!pData)
		return NULL;
	pData->Initialize(nBagNo, nObjElement1, nObjElement2, nObjElement3, nObjElement4
		, nObjElement5, nObjElement6, nObjElement7, nObjElement8, nObjElement9, nObjElement10, DCV_INSERT);
	m_pmAllBagList->Add(pData);
	//m_pmNewBagList->Add(pData);

	return pData;
}

MEntity* MPgBreakObject::AddBagListEntity(MEntity* pOrgEntity)
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return NULL;
	}
	String* strEntityName = "[General]Object.break_object";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
		return NULL;
	}

	if(m_pMakingData->m_nBagNo == 0)
	{
		::MessageBox(0, "BagNo 를 넣어주세요", 0, 0);
		return NULL;
	}

	SettedObjectData *pSetted = NULL;
	ObjectBagData* pBag = GetObjectBagData(m_pMakingData->m_nBagNo);
	if(pBag && pBag->m_nChangeFlag != DCV_DELETE)
	{
		pSetted = GetSettedBreakObjectFromBagNo(pBag->m_nBagNo);
		if(pSetted && pSetted->m_nChangeFlag != DCV_DELETE)
		{
			::MessageBox(0, "BagNo 가 중복되었습니다.", 0, 0);
			return NULL;
		}
	}

	// 엔터티 생성
	char szTemp[20]={0,};
	_itoa_s(m_pMakingData->m_nBagNo, szTemp, 10);
	String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", szTemp));
	MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
	if(!pkNewEntity)
		return NULL;

	Guid kGuid;
	pSetted = GetSettedBreakObjectFromBagNo(m_pMakingData->m_nBagNo);
	if(pSetted && pSetted->m_nChangeFlag == DCV_DELETE)
	{
		kGuid = pSetted->m_kGuid;
	}
	else
	{
		kGuid = Guid::NewGuid();
	}
	pkNewEntity->TemplateID = kGuid;
	NiPoint3 kEntityPos;
	NiMatrix3 kRotMat;
	pOrgEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
	pOrgEntity->GetNiEntityInterface()->GetPropertyData("Rotation", kRotMat);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Rotation", kRotMat);
	pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
	pkNewEntity->MasterEntity = pkTemplate;

	m_pMakingData->m_pkEntity = pkNewEntity;

	NiBool bBool;
	NiObject* pkSceneRoot = NULL;
	bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
	if(!bBool)
	{
		delete pkNewEntity;
		return NULL;
	}

	NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
	if(!pkSceneNode)
	{
		delete pkNewEntity;
		return NULL;
	}

	int nCount = m_pMakingData->m_pmElementList->Count;
	if(nCount == 0)
	{
		delete pkNewEntity;
		return NULL;
	}
	for(int nn=0; nn<nCount; ++nn)
	{
		ObjectElementData* pObjectData = dynamic_cast<ObjectElementData*>(m_pMakingData->m_pmElementList->get_Item(nn));
		if(pObjectData)
		{
			String* strPath = pObjectData->m_strPath->ToString();
			if(strPath && m_strXmlPath)
			{
				String* strFilePath = String::Concat(m_strXmlPath->ToString(), "\\", strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
				const char* pcFilePath = MStringToCharPointer(strFilePath);
				NiStream kStream;
				bool bRtn = kStream.Load(pcFilePath);
				if(!bRtn)
				{
					::MessageBox(0, pcFilePath, "Nif file load fail", 0);
					return NULL;
				}
				MFreeCharPointer(pcFilePath);
				NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
				if(pkNode)
				{
					pkSceneNode->AttachChild(pkNode);
					pkNode->SetTranslate(pObjectData->m_fRelativeX, pObjectData->m_fRelativeY, pObjectData->m_fRelativeZ);
				}
			}
		}
	}

	pkSceneNode->UpdateProperties();
	pkSceneNode->Update(0.0f);

	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);


	InsertSettedBreakObject(kGuid, "", m_nMapNum, m_pMakingData->m_nBagNo, m_pMakingData->m_nPointGroup, m_pMakingData->m_nRegenPeriod, kEntityPos.x, 
		kEntityPos.y, kEntityPos.z, GetBreakObjectRot(pkNewEntity), 0, 0, pkNewEntity);

	return pkNewEntity;
}
bool MPgBreakObject::RemoveElementData(int nIndex)
{
	if(!m_pMakingData->m_pmElementList)
		return false;

	if(nIndex < 0 || nIndex >= m_pMakingData->m_pmElementList->Count)
		return false;

	m_pMakingData->m_pmElementList->RemoveAt(nIndex);

	return true;
}
bool MPgBreakObject::AddBagListFromMakingData()
{
	if(m_pMakingData->m_pmElementList == NULL)
		return false;

	int nElement[ELEMENT_COUNT_IN_BAG]={0,};
	
	for(int mm=0; mm<m_pMakingData->m_pmElementList->Count; ++mm)
	{
		ObjectElementData* pData = dynamic_cast<ObjectElementData*>(m_pMakingData->m_pmElementList->get_Item(mm));
		if(pData)
		{
			nElement[mm] = pData->m_nElementNo;
		}
	}

 	ObjectBagData* pObjectBagData = AddNewBagList(m_pMakingData->m_nBagNo, nElement[0], nElement[1]
	, nElement[2], nElement[3], nElement[4], nElement[5], nElement[6], nElement[7], nElement[8], nElement[9]);

	if(!pObjectBagData)
		return false;

	pObjectBagData->m_pkEntity = m_pMakingData->m_pkEntity;

	AddElementListFromMakingData();

	return true;
}
bool MPgBreakObject::AddElementListFromMakingData()
{
	if(!m_pMakingData->m_pmElementList)
		return false;

	int nCount = m_pMakingData->m_pmElementList->Count;

	for(int nn=0; nn<nCount; ++nn)
	{
		ObjectElementData* pNew = dynamic_cast<ObjectElementData*>(m_pMakingData->m_pmElementList->get_Item(nn));
		if(!pNew)
			continue;

		AddNewElemntList(pNew->m_nElementNo, pNew->m_nObjectNo, pNew->m_fRelativeX, pNew->m_fRelativeY, pNew->m_fRelativeZ, pNew->m_strPath);
	}

	return true;
}
void MPgBreakObject::AddBagListEntityFromLoad()
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return;
	}
	String* strEntityName = "[General]Object.break_object";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
		return;
	}

	int nTotalCount = m_pmSettedBreakObjectList->Count;
	for(int nCount=0; nCount<nTotalCount; ++nCount)
	{
		SettedObjectData *pSettedMonData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(nCount));
		if(!pSettedMonData)
			continue;
		ObjectBagData* pData = GetObjectBagData(pSettedMonData->m_iMonParentBagNo);
		if(!pData)
			continue;
		// 엔터티 생성
		char szTemp[20]={0,};
		_itoa_s(m_pMakingData->m_nBagNo, szTemp, 10);
		String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", szTemp));
		MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
		if(!pkNewEntity)
			continue;

		MPoint3* pkPos = GetBreakObjectPos(pData->m_nBagNo);
		NiPoint3 kEntityPos;
		kEntityPos.x = pkPos->get_X();
		kEntityPos.y = pkPos->get_Y();
		kEntityPos.z = pkPos->get_Z();
		pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos);
		pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
		pkNewEntity->MasterEntity = pkTemplate;

		NiBool bBool;
		NiObject* pkSceneRoot = NULL;
		bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
		if(!bBool)
		{
			delete pkNewEntity;
			continue;
		}

		NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
		if(!pkSceneNode)
		{
			delete pkNewEntity;
			continue;
		}

		int nElement[ELEMENT_COUNT_IN_BAG]={0,};
		nElement[0] = pData->m_nObjElement1;
		nElement[1] = pData->m_nObjElement2;
		nElement[2] = pData->m_nObjElement3;
		nElement[3] = pData->m_nObjElement4;
		nElement[4] = pData->m_nObjElement5;
		nElement[5] = pData->m_nObjElement6;
		nElement[6] = pData->m_nObjElement7;
		nElement[7] = pData->m_nObjElement8;
		nElement[8] = pData->m_nObjElement9;
		nElement[9] = pData->m_nObjElement10;
		for(int nEl=0; nEl<ELEMENT_COUNT_IN_BAG; ++nEl)
		{
			if(nElement[nEl] == 0)
				break;

			ObjectElementData* pElementData = GetObjectElementData(nElement[nEl]);
			if(!pElementData)
				continue;

			String* strPath = pElementData->m_strPath->ToString();
			if(strPath && m_strXmlPath)
			{
				String* strFilePath = String::Concat(m_strXmlPath->ToString(), strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
				const char* pcFilePath = MStringToCharPointer(strFilePath);
				NiStream kStream;
				bool bRtn = kStream.Load(pcFilePath);
				MFreeCharPointer(pcFilePath);
				if(!bRtn)
				{
					::MessageBox(0, pcFilePath, "Nif file load fail", 0);
					return;
				}
				NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
				if(pkNode)
				{
					pkSceneNode->AttachChild(pkNode);
					pkNode->SetTranslate(pElementData->m_fRelativeX, pElementData->m_fRelativeY, pElementData->m_fRelativeZ);
				}
			}
		}

		pkSceneNode->UpdateProperties();
		pkSceneNode->Update(0.0f);

		MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
	}
}
ObjectElementData* MPgBreakObject::GetObjectElementData(int nElementNo)
{
	int nCount = m_pmSettedElementList->Count;
	ObjectElementData* pData = NULL;
	ObjectElementData* pRtn = NULL;
	for(int nn=0; nn<nCount; ++nn)
	{
		pData = dynamic_cast<ObjectElementData*>(m_pmSettedElementList->get_Item(nn));
		if(!pData)
			continue;

		if(pData->m_nElementNo == nElementNo)
		{
			pRtn = pData;
			break;
		}
	}

	return pRtn;
}
int MPgBreakObject::GetSettedMonsterBagNo(int nIndex)
{
	if(nIndex >= 0)
	{
		int nPos = 0;
		SettedObjectData *pSettedMonData = NULL;
		for(int nn=0; nn<m_pmSettedBreakObjectList->Count; ++nn)
		{	
			pSettedMonData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(nn));
			if(!pSettedMonData)
			{
				++nPos;
				continue;
			}

			if(pSettedMonData->m_nChangeFlag == DCV_DELETE)
				continue;

			if(nPos == nIndex)
				break;

			++nPos;

		}

		if(pSettedMonData)
		{
			return pSettedMonData->m_iMonParentBagNo;
		}
	}

	return 0;
}
void MPgBreakObject::RemoveObjectBagData(int nBagNo)
{
	if(nBagNo == 0)
		return;

	for(int mm=0; mm<m_pmAllBagList->Count; ++mm)
	{
		ObjectBagData* pData = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(mm));
		if(!pData)
			return;

		if(pData->m_nBagNo == nBagNo)
		{
			if(pData->m_nChangeFlag == DCV_INSERT)
			{
				m_pmAllBagList->Remove(pData);
			}
			else
			{
				pData->m_nChangeFlag = DCV_DELETE;
			}
			return;
		}
	}

	//for(int ll=0; ll<m_pmNewBagList->Count; ++ll)
	//{
	//	ObjectBagData* pData = dynamic_cast<ObjectBagData*>(m_pmNewBagList->get_Item(ll));
	//	if(!pData)
	//		return;

	//	if(pData->m_pkEntity == pkEntity)
	//	{
	//		m_pmNewBagList->Remove(pData);
	//		return;
	//	}
	//}
}
void MPgBreakObject::RemoveSettedBreakObject(int nIndex)
{
	int nPos=0;
	SettedObjectData *pSettedMonData = NULL;
	for(int nn=0; nn<m_pmSettedBreakObjectList->Count; ++nn)
	{
		pSettedMonData = dynamic_cast<SettedObjectData *>(m_pmSettedBreakObjectList->get_Item(nn));
		if(!pSettedMonData)
		{
			++nPos;
			continue;
		}

		if(pSettedMonData->m_nChangeFlag == DCV_DELETE)
		{
			continue;
		}

		if(nPos == nIndex)
			break;

		++nPos;
	}

	if(!pSettedMonData)
		return;
	
	DeleteSettedBreakObjectData(pSettedMonData->m_kGuid);
	MFramework::Instance->Scene->RemoveEntity(pSettedMonData->m_pkEntity, false);

	ObjectBagData* pData = GetObjectBagData(pSettedMonData->m_iMonParentBagNo);
	if(!pData)
		return;

	if(pData->m_nChangeFlag == DCV_INSERT)
	{
		m_pmAllBagList->Remove(pData);
	}
	else
	{
		pData->m_nChangeFlag = DCV_DELETE;
	}
}
ObjectBagData* MPgBreakObject::GetObjectBagData(int nBagNo)
{
	ObjectBagData* pRtn = NULL;
	for(int nn=0; nn<m_pmAllBagList->Count; ++nn)
	{
		ObjectBagData* pData = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(nn));
		if(pData && pData->m_nBagNo == nBagNo)
		{
			pRtn = pData;
			break;
		}
	}

	return pRtn;
}
void MPgBreakObject::ObjectBagFlagSettingAfterDBUpdate()
{
	int nCount = m_pmAllBagList->Count; 
	for(int nn=0; nn<nCount; ++nn)
	{
		ObjectBagData* pData = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(nn));
		if(pData)
		{
			if(pData->m_nChangeFlag == DCV_DELETE)
			{
				m_pmAllBagList->RemoveAt(nn);
				--nn;
				nCount = m_pmAllBagList->Count;
			}
			else
			{
				pData->m_nChangeFlag = DCV_NORMAL;
			}
		}
	}
}
void MPgBreakObject::ObjectElementFlagSettingAfterDBUpdate()
{
	int nCount = m_pmSettedElementList->Count;
	for(int nn=0; nn<nCount; ++nn)
	{
		ObjectElementData* pData = dynamic_cast<ObjectElementData*>(m_pmSettedElementList->get_Item(nn));
		if(pData)
		{
			if(pData->m_nChangeFlag == DCV_DELETE)
			{
				m_pmSettedElementList->RemoveAt(nn);
				--nn;
				nCount = m_pmSettedElementList->Count;
			}
			else
			{
				pData->m_nChangeFlag = DCV_NORMAL;
			}
		}
	}
}
void MPgBreakObject::SettedObjectFlagSettingAfterDBUpdate()
{
	int nCount = m_pmSettedBreakObjectList->Count;
	for(int nn=0; nn<nCount; ++nn)
	{
		SettedObjectData* pData = dynamic_cast<SettedObjectData*>(m_pmSettedBreakObjectList->get_Item(nn));
		if(pData)
		{
			if(pData->m_nChangeFlag == DCV_DELETE)
			{
				m_pmSettedBreakObjectList->RemoveAt(nn);
				--nn;
				nCount = m_pmSettedBreakObjectList->Count;
			}
			else
			{
				pData->m_nChangeFlag = DCV_NORMAL;
			}
		}
	}
}
bool MPgBreakObject::ModifyElement(int nElement, int nObject, float fX, float fY, float fZ)
{
	for(int nn=0; nn<m_pmSettedElementList->Count; ++nn)
	{
		ObjectElementData* pData = dynamic_cast<ObjectElementData*>(m_pmSettedElementList->get_Item(nn));
		if(pData && pData->m_nElementNo == nElement)
		{
			if(pData->m_nChangeFlag == DCV_DELETE)
				return false;

			if(pData->m_nChangeFlag != DCV_INSERT)
				pData->m_nChangeFlag = DCV_UPDATE;

			pData->m_nObjectNo = nObject;
			pData->m_fRelativeX = fX;
			pData->m_fRelativeY = fY;
			pData->m_fRelativeZ = fZ;
			return true;
		}
	}

	return false;
}

bool MPgBreakObject::RemoveElement(int nIndex)
{
	int nPos = 0;
	for(int nn=0; nn<m_pmSettedElementList->Count; ++nn)
	{
		ObjectElementData* pData = dynamic_cast<ObjectElementData*>(m_pmSettedElementList->get_Item(nn));
		if(!pData)
		{
			++nPos;
			continue;
		}
		if(pData->m_nChangeFlag == DCV_DELETE)
			continue;

		if(nIndex == nPos)
		{	
			if(pData->m_nChangeFlag == DCV_INSERT)
			{
				m_pmSettedElementList->Remove(pData);
			}
			else
			{
				pData->m_nChangeFlag = DCV_DELETE;
			}
			return true;
		}

		++nPos;
	}

	return false;
}

void MPgBreakObject::RefreshScreenBreakObject()
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return;
	}
	String* strEntityName = "[General]Object.break_object";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Object.break_object 가 없습니다.", 0, 0);
		return;
	}

	{
		unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
		for (unsigned int i = 0 ; i < uiEntityCount ; i++)
		{
			MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];

			if (IsBreakObjctData(pkEntity->TemplateID.ToString()))
			{
				MFramework::Instance->Scene->RemoveEntity(pkEntity,false);
				uiEntityCount -= 1;
				i -= 1;
			}
		}
	}

	for(int i=0; i<m_pmSettedBreakObjectList->Count; ++i)
	{
		SettedObjectData* pData = dynamic_cast<SettedObjectData*>(m_pmSettedBreakObjectList->get_Item(i));
		if(!pData)
			continue;

		if(pData->m_nChangeFlag == DCV_DELETE)
			continue;

		// 엔터티 생성
		char szTemp[20]={0,};
		_itoa_s(pData->m_iMonParentBagNo, szTemp, 10);
		String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(String::Concat("break_object_", szTemp));
		MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
		if(!pkNewEntity)
			continue;

		pkNewEntity->TemplateID = pData->m_kGuid;
		NiPoint3 kEntityPos;
		kEntityPos.x = pData->m_fPosX;
		kEntityPos.y = pData->m_fPosY;
		kEntityPos.z = pData->m_fPosZ;
		pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos);
		pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
		pkNewEntity->MasterEntity = pkTemplate;

		NiBool bBool;
		NiObject* pkSceneRoot = NULL;
		bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
		if(!bBool)
		{
			delete pkNewEntity;
			return;
		}

		NiNode* pkSceneNode = NiDynamicCast(NiNode, pkSceneRoot);
		if(!pkSceneNode)
		{
			delete pkNewEntity;
			return;
		}

		int nCount = m_pMakingData->m_pmElementList->Count;
		if(nCount == 0)
		{
			delete pkNewEntity;
			return;
		}

		ObjectBagData *pBag = GetObjectBagData(pData->m_iMonParentBagNo);
		int nElement[ELEMENT_COUNT_IN_BAG]={0,};
		nElement[0] = pBag->m_nObjElement1;
		nElement[1] = pBag->m_nObjElement2;
		nElement[2] = pBag->m_nObjElement3;
		nElement[3] = pBag->m_nObjElement4;
		nElement[4] = pBag->m_nObjElement5;
		nElement[5] = pBag->m_nObjElement6;
		nElement[6] = pBag->m_nObjElement7;
		nElement[7] = pBag->m_nObjElement8;
		nElement[8] = pBag->m_nObjElement9;
		nElement[9] = pBag->m_nObjElement10;
		for(int nn=0; nn<ELEMENT_COUNT_IN_BAG; ++nn)
		{
			if(nElement[nn] == 0)
				break;

			ObjectElementData* pObjectData = GetObjectElementData(nElement[nn]);
			if(pObjectData)
			{
				String* strPath = pObjectData->m_strPath->ToString();
				if(strPath && m_strXmlPath)
				{
					String* strFilePath = String::Concat(m_strXmlPath->ToString(), "\\", strPath->Substring(0, strPath->LastIndexOf("kfm")), "nif");
					const char* pcFilePath = MStringToCharPointer(strFilePath);
					NiStream kStream;
					kStream.Load(pcFilePath);
					MFreeCharPointer(pcFilePath);
					NiNode* pkNode = (NiNode*)kStream.GetObjectAt(0);
					if(pkNode)
					{
						pkSceneNode->AttachChild(pkNode);
						pkNode->SetTranslate(pObjectData->m_fRelativeX, pObjectData->m_fRelativeY, pObjectData->m_fRelativeZ);
					}
				}
			}
		}

		pkSceneNode->UpdateProperties();
		pkSceneNode->Update(0.0f);

		MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
	}
}
SettedObjectData* MPgBreakObject::GetSettedBreakObjectFromBagNo(int nBagNo)
{
	for(int nCount=0; nCount<m_pmSettedBreakObjectList->Count; ++nCount)
	{
		SettedObjectData* pData = dynamic_cast<SettedObjectData*>(m_pmSettedBreakObjectList->get_Item(nCount));
		if(pData)
		{
			if(pData->m_iMonParentBagNo == nBagNo)
			{	
				return pData;
			}
		}
	}

	return NULL;
}

bool MPgBreakObject::RemoveObjectBag(int nIndex)
{
	int nPos = 0;
	for(int nn=0; nn<m_pmAllBagList->Count; ++nn)
	{
		ObjectBagData* pData = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(nn));
		if(!pData)
		{
			++nPos;
			continue;
		}
		if(pData->m_nChangeFlag == DCV_DELETE)
			continue;

		if(nIndex == nPos)
		{	
			if(pData->m_nChangeFlag == DCV_INSERT)
			{
				m_pmAllBagList->Remove(pData);
			}
			else
			{
				pData->m_nChangeFlag = DCV_DELETE;
			}
			return true;
		}

		++nPos;
	}

	return false;
}

void MPgBreakObject::ObjectBagSelect(int nIndex)
{
	if (nIndex == -1)
		return;

	int nPos = 0;
	ObjectBagData* pData = NULL;;
	for(int i=0; i<m_pmAllBagList->Count; ++i)
	{
		pData = dynamic_cast<ObjectBagData*>(m_pmAllBagList->get_Item(i));
		if(!pData)
		{
			++nPos;
			continue;
		}

		if(pData->m_nChangeFlag == DCV_DELETE)
			continue;

		if(nPos == nIndex)
			break;

		++nPos;
	}

	if(!pData)
		return;

	int nElement[ELEMENT_COUNT_IN_BAG]={0,};
	nElement[0] = pData->m_nObjElement1;
	nElement[1] = pData->m_nObjElement2;
	nElement[2] = pData->m_nObjElement3;
	nElement[3] = pData->m_nObjElement4;
	nElement[4] = pData->m_nObjElement5;
	nElement[5] = pData->m_nObjElement6;
	nElement[6] = pData->m_nObjElement7;
	nElement[7] = pData->m_nObjElement8;
	nElement[8] = pData->m_nObjElement9;
	nElement[9] = pData->m_nObjElement10;
	m_pMakingData->m_pmElementList->Clear();
	for(int nn=0; nn<ELEMENT_COUNT_IN_BAG; ++nn)
	{
		if(nElement[nn] == 0)
			break;

		ObjectElementData* pElement = GetObjectElementData(nElement[nn]);
		if(pElement)
		{
			m_pMakingData->m_pmElementList->Add(pElement);
		}
	}

	m_pMakingData->m_nBagNo = pData->m_nBagNo;
	SettedObjectData* pSetted = GetSettedBreakObjectFromBagNo(pData->m_nBagNo);
	if(pSetted)
	{
		m_pMakingData->m_nRegenPeriod = pSetted->m_iRegenPeriod;
		m_pMakingData->m_nPointGroup = pSetted->m_iPointGroup;
		m_pMakingData->m_pkEntity = pSetted->m_pkEntity;
	}
	else
	{
		m_pMakingData->m_nRegenPeriod = 0;
	}
}

void MPgBreakObject::InsertSettedBreakObject(Guid kGuid, String *strMemo, int iMapNo,
							 int iMonParentBagNo, int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, 
							 float fPosZ,int iRotAxZ, int iMoveRange, String* strGroup, MEntity* pkEntity)
{

	SettedObjectData *pkData = GetSettedBreakObjectFromBagNo(iMonParentBagNo);
	if(pkData)
	{
		if(pkData->m_nChangeFlag == DCV_INSERT)
		{
			pkData->Initialize(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, 
				fPosY, fPosZ, iRotAxZ, iMoveRange, strGroup, DCV_INSERT);
		}
		else
		{
			pkData->Initialize(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, 
				fPosY, fPosZ, iRotAxZ, iMoveRange, strGroup, DCV_UPDATE);
		}

		pkData->m_pkEntity = pkEntity;
	}
	else
	{
		pkData = new SettedObjectData;
		pkData->Initialize(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, 
			fPosY, fPosZ, iRotAxZ, iMoveRange, strGroup, DCV_INSERT);
		pkData->m_pkEntity = pkEntity;

		m_pmSettedBreakObjectList->Add(pkData);
	}
}