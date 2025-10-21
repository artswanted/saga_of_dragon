#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "MPgElevator.h"

using namespace System::Text;
using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgElevator::MPgElevator()
{
	m_pmElevatorList = new ArrayList;
	m_pmTranslateList = new ArrayList;
	m_pmDeleteList = new ArrayList;
	m_strFileName = "";
	//m_pkDocument = new XmlDocument();
	m_bSync = false;
	m_bAdd = false;
	m_bDelete = false;
	m_bTranslate = false;
}
MPgElevator::~MPgElevator()
{
	m_pmElevatorList->Clear();
	m_pmTranslateList->Clear();
	m_pmDeleteList->Clear();
}

void MPgElevator::Clear()
{
	if(m_pmElevatorList)
	{
		for(int ii=0; ii<m_pmElevatorList->Count; ++ii)
		{
			SElevatorList* pElevatorList = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
			if(pElevatorList && pElevatorList->m_pmPointList)
			{
				pElevatorList->m_pmPointList->Clear();
			}
		}

		m_pmElevatorList->Clear();
	}

	if(m_pkDocument)
	{
		m_pkDocument->RemoveAll();
	}
}

//bool MPgElevator::LoadXML()
//{
//	Clear();
//	//XmlDocument* m_pkDocument = new XmlDocument();
//	m_pkDocument->Load(m_strFileName);
//	m_pkDocument->PreserveWhitespace = true;
//
//	XmlElement* pRoot = m_pkDocument->DocumentElement;
//	if(!pRoot)
//	{
//		return false;
//	}
//
//	XmlNodeList* nodeList = m_pkDocument->GetElementsByTagName("ELEVATOR");
//	if(!nodeList)
//	{
//		return false;
//	}
//
//
//	String* strTemp = "";
//	float* pfTemp = new float;
//	if(!pfTemp)
//		return false;
//	int* piTemp = new int;
//	if(!piTemp)
//		return false;
//
//	for(int ii=0; ii<nodeList->Count; ++ii)
//	{
//		SElevatorList* pElevaterList = new SElevatorList;
//		if(!pElevaterList)
//			continue;
//
//		XmlNode* pkNode = nodeList->get_ItemOf(ii);
//		if(pkNode)
//		{
//			pElevaterList->m_strClassNo = pkNode->Attributes->GetNamedItem("ID")->InnerText;
//
//			strTemp = pkNode->Attributes->GetNamedItem("SPEED")->InnerText;
//			Single::TryParse(strTemp, pfTemp);
//			pElevaterList->m_fSpeed = *pfTemp;
//
//			if(pkNode->ChildNodes)
//			{
//				XmlNodeList* pkChildList = pkNode->ChildNodes;
//				for(int jj=0; jj<pkChildList->Count; ++jj)
//				{
//					SPointList* pPointList = new SPointList;
//					if(!pPointList)
//						continue;
//
//					XmlNode* pkChild = pkChildList->get_ItemOf(jj);
//					if(pkChild)
//					{
//						strTemp = pkChild->Attributes->GetNamedItem("POSX")->InnerText;
//						Single::TryParse(strTemp, pfTemp);
//						pPointList->m_fPosX = *pfTemp;
//
//						strTemp = pkChild->Attributes->GetNamedItem("POSY")->InnerText;
//						Single::TryParse(strTemp, pfTemp);
//						pPointList->m_fPosY = *pfTemp;
//
//						strTemp = pkChild->Attributes->GetNamedItem("POSZ")->InnerText;
//						Single::TryParse(strTemp, pfTemp);
//						pPointList->m_fPosZ = *pfTemp;
//
//						strTemp = pkChild->Attributes->GetNamedItem("TIME")->InnerText;
//						Single::TryParse(strTemp, pfTemp);
//						pPointList->m_fTime = *pfTemp;
//					}
//
//					NiPoint3 pkPoint;
//					pkPoint.x = pPointList->m_fPosX;
//					pkPoint.y = pPointList->m_fPosY;
//					pkPoint.z = pPointList->m_fPosZ;
//					MEntity* pkEntity = AddElevatorEntity(pkPoint, pName);
//					pPointList->m_pmEntity = pkEntity;
//
//					pElevaterList->m_pmPointList->Add(pPointList);
//				}
//			}
//		}
//	}
//
//	delete pfTemp;
//	delete piTemp;
//
//	return true;
//}
//bool MPgElevator::SaveXML()
//{
//	if(m_pmElevatorList->Count == 0)
//		return false;
//
//	//XmlDocument* m_pkDocument = new XmlDocument();
//	if(!m_pkDocument)
//		return false;
//
//	m_pkDocument->PreserveWhitespace = true;
//
//	//// Version
//	//XmlDeclaration* newDec = m_pkDocument->CreateXmlDeclaration("1.0", "euc-kr", 0);
//	//if(!newDec)
//	//	return false;
//	//m_pkDocument->AppendChild(newDec);
//
//	// 기존 노드 지우기
//	XmlNodeList* nodeList = m_pkDocument->GetElementsByTagName("ELEVATOR");
//	if(nodeList)
//	{
//		for(int ii=0; ii<nodeList->Count; ++ii)
//		{
//			XmlNode* pkNode = nodeList->get_ItemOf(ii);
//			if(pkNode)
//			{
//				pkNode->RemoveAll();
//			}
//		}
//	}
//
//	for(int ii=0; ii<m_pmElevatorList->Count; ++ii)
//	{
//		SElevatorList* pElevatorList = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
//		if(!pElevatorList || !pElevatorList->m_pmPointList)
//			continue;
//
//		XmlElement* pElement = m_pkDocument->CreateElement("ELEVATOR");
//		if(!pElement)
//			continue;
//
//		pElement->InnerText = "";
//		pElement->SetAttribute("ID", pElevatorList->m_strClassNo);
//
//		char szTmp[100]={0,};
//		sprintf(szTmp, "%f", pElevatorList->m_fSpeed);
//		pElement->SetAttribute("SPEED", szTmp);
//
//		for(int jj=0; jj<pElevatorList->m_pmPointList->Count; ++jj)
//		{
//			SPointList* pPointList = dynamic_cast<SPointList*>(pElevatorList->m_pmPointList->get_Item(jj));
//			if(!pPointList)
//				continue;
//
//			XmlElement* pChild = m_pkDocument->CreateElement("POINT");
//			if(!pChild)
//				continue;
//			// number
//			itoa(pPointList->m_iNum, szTmp, 10);
//			pChild->SetAttribute("NUMBER", szTmp);
//			// PosX
//			sprintf(szTmp, "%f", pPointList->m_fPosX);
//			pChild->SetAttribute("POSX", szTmp);
//			// PosY
//			sprintf(szTmp, "%f", pPointList->m_fPosY);
//			pChild->SetAttribute("POSY", szTmp);
//			// PosZ
//			sprintf(szTmp, "%f", pPointList->m_fPosZ);
//			pChild->SetAttribute("POSZ", szTmp);
//			// Time
//			sprintf(szTmp, "%f", pPointList->m_fTime);
//			pChild->SetAttribute("TIME", szTmp);
//
//			pElement->AppendChild(pChild);
//		}
//	}
//
//	XmlTextWriter* tr = new XmlTextWriter(m_strFileName, Encoding::GetEncoding(949));
//	tr->Formatting = Formatting::Indented;
//	m_pkDocument->WriteContentTo(tr);
//	tr->Close();
//
//	return true;
//}

MEntity* MPgElevator::AddElevatorEntity(MEntity* pkEntity)
{
	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(m_iSelectElevatorList));
	if(pElevator)
	{
		SPointList* pPoint = new SPointList;
		NiPoint3 kEntityPos;
		pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
		pPoint->m_fPosX = kEntityPos.x;
		pPoint->m_fPosY = kEntityPos.y;
		pPoint->m_fPosZ = kEntityPos.z;
		pPoint->m_pmEntity = pkEntity;
		pPoint->m_fTime = m_fPointTime;
		pElevator->m_pmPointList->Add(pPoint);

		//MFramework::Instance->Scene->AddEntity(pkEntity, false);

		return pkEntity;
	}

	return NULL;
}


void MPgElevator::RemoveElevatorEntity(String* strTemplateID)
{
	for(int ii = 0; m_pmElevatorList->Count; ++ii)
	{
		SElevatorList* pkElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
		if(pkElevator && pkElevator->m_pmPointList)
		{
			for(int jj=0; pkElevator->m_pmPointList->Count; ++jj)
			{
				SPointList* pkPoint = dynamic_cast<SPointList*>(pkElevator->m_pmPointList->get_Item(jj));
				if(pkPoint)
				{
					if(pkPoint->m_pmEntity->TemplateID.Equals(strTemplateID))
					{
						RemoveElevatorEntity(pkPoint->m_pmEntity);
						ii =  m_pmElevatorList->Count;
						break;
					}
				}
			}
		}
	}
}

void MPgElevator::RemoveElevatorEntity(MEntity* pkEntity)
{
	MFramework::Instance->Scene->RemoveEntity(pkEntity, false);
}
void MPgElevator::RemoveElevatorPoint(MEntity* pkEntity)
{
	for(int ii = 0; m_pmElevatorList->Count; ++ii)
	{
		SElevatorList* pkElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
		if(pkElevator && pkElevator->m_pmPointList)
		{
			for(int jj=0; pkElevator->m_pmPointList->Count; ++jj)
			{
				SPointList* pkPoint = dynamic_cast<SPointList*>(pkElevator->m_pmPointList->get_Item(jj));
				if(pkPoint)
				{
					if(pkPoint->m_pmEntity == pkEntity)
					{
						RemoveElevatorEntity(pkPoint->m_pmEntity);
						pkElevator->m_pmPointList->RemoveAt(jj);
						ii = m_pmElevatorList->Count;
						break;
					}
				}
			}
		}
	}	
}
Guid MPgElevator::CreateElevator(String* strClassNo, String* strRidable, float fSpeed, Guid kGuid)
{
	if(strClassNo == NULL)
		return Guid::Empty;

	SElevatorList* pkElevator = new SElevatorList;
	if(!pkElevator)
		return Guid::Empty;

	if(kGuid == Guid::Empty)
	{
		pkElevator->m_kGuid = Guid::NewGuid();
	}
	else
	{
		pkElevator->m_kGuid = kGuid;
	}
	pkElevator->m_strClassNo = strClassNo;
	pkElevator->m_strRidable = strRidable;
	pkElevator->m_fSpeed = fSpeed;

	m_pmElevatorList->Add(pkElevator);

	return pkElevator->m_kGuid;
}

void MPgElevator::DeleteElevator(int iIndex)
{
	if(m_pmElevatorList->Count <= iIndex)
	{
		return;
	}

	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iIndex));
	if(!pElevator)
		return;

	if(pElevator->m_pmPointList->Count > 0)
	{
		for(int ii=0; ii<pElevator->m_pmPointList->Count; ++ii)
		{
			SPointList* pPoint = dynamic_cast<SPointList*>(pElevator->m_pmPointList->get_Item(ii));
			if(pPoint && pPoint->m_pmEntity)
			{
				MFramework::Instance->Scene->RemoveEntity(pPoint->m_pmEntity, false);
			}
		}
	}

	if(pElevator->m_pmPointList)
	{
		pElevator->m_pmPointList->Clear();
	}

	m_pmElevatorList->RemoveAt(iIndex);
}

void MPgElevator::ModifyElevator(int iIndex, String* strClassNo, String* strRidable, float fSpeed)
{
	if(m_pmElevatorList->Count <= iIndex)
	{
		return;
	}

	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iIndex));
	if(!pElevator)
		return;

	pElevator->m_strClassNo = strClassNo;
	pElevator->m_strRidable = strRidable;
	pElevator->m_fSpeed = fSpeed;
}
void MPgElevator::DeletePoint(int iElevatorIndex, int iPointIndex)
{
	if(m_pmElevatorList->Count <= iElevatorIndex)
	{
		return;
	}

	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iElevatorIndex));
	if(!pElevator)
		return;

	if(pElevator->m_pmPointList->Count <= iPointIndex)
	{
		return;
	}

	SPointList* pPoint = dynamic_cast<SPointList*>(pElevator->m_pmPointList->get_Item(iPointIndex));
	if(pPoint)
	{
		MFramework::Instance->Scene->RemoveEntity(pPoint->m_pmEntity, false);
	}
	pElevator->m_pmPointList->RemoveAt(iPointIndex);
}

void MPgElevator::ModifyPoint(int iElevatorIndex, int iPointIndex, float fTime)
{
	if(m_pmElevatorList->Count <= iElevatorIndex)
	{
		return;
	}

	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iElevatorIndex));
	if(!pElevator)
		return;	

	if(pElevator->m_pmPointList->Count <= iPointIndex)
	{
		return;
	}

	SPointList* pPoint = dynamic_cast<SPointList*>(pElevator->m_pmPointList->get_Item(iPointIndex));
	if(!pPoint)
	{
		return;
	}

	pPoint->m_fTime = fTime;
}
void MPgElevator::CreatePoint(int iElevatorIndex, int iPointNum, float fTime, float fPosX, float fPosY, float fPosZ, MEntity* pEntity)
{
	if(m_pmElevatorList->Count <= iElevatorIndex)
	{
		return;
	}

	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iElevatorIndex));
	if(!pElevator)
		return;	

	SPointList* pPoint = new SPointList;
	if(!pPoint)
	{
		return;
	}

	if(!pEntity)
	{
		pEntity = AddpointEntity(iElevatorIndex, iPointNum, fPosX, fPosY, fPosZ);
	}
	pPoint->Set(fTime, fPosX, fPosY, fPosZ, pEntity);
	pElevator->m_pmPointList->Add(pPoint);
}
MEntity* MPgElevator::AddpointEntity(int iElevatorIndex, int iPointNum, float fX, float fY, float fZ)
{
	// 팔레트를 로드한다.
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return NULL;
	}
	String* strEntityName = "[General]Target.elevator_target";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Target.elevator_target 가 없습니다.", 0, 0);
		return NULL;
	}

	// 엔터티 생성
	char szTmp[20]={0,};
	sprintf(szTmp, "Elevator_%d_%d", iElevatorIndex, iPointNum);
	String* strNewName = MFramework::Instance->Scene->GetUniqueEntityName(szTmp);
	MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
	if(!pkNewEntity)
		return NULL;

	NiPoint3 kPos;
	kPos.x = fX;
	kPos.y = fY;
	kPos.z = fZ;
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
		return NULL;

	pkSceneNode->UpdateProperties();
	pkSceneNode->Update(0.0f);

	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

	return pkNewEntity;	
}
void MPgElevator::DeletePointEntity(MEntity* pkEntity)
{
	for(int ii=0; ii<m_pmElevatorList->Count; ++ii)
	{
		SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
		if(pElevator)
		{
			for(int jj=0; jj<pElevator->m_pmPointList->Count; ++jj)
			{
				SPointList* pPoint = dynamic_cast<SPointList*>(pElevator->m_pmPointList->get_Item(jj));
				if(pPoint)
				{
					if(pPoint->m_pmEntity == pkEntity)
					{
						m_bDelete = true;
						m_iDeleteElevatorIndex = ii;
						m_iDeletePointIndex = jj;

						//SDeleteList* pkDeletedObject = new SDeleteList;
						//pkDeletedObject->Set(ii, jj);
						//m_pmDeleteList->Add(pkDeletedObject);
						//m_pkDocument->a

						XmlNodeList *nodeList = m_pkDocument->GetElementsByTagName("OBJECT");
						XmlNode *kNode = nodeList->Item(ii);
						XmlNode *kChild = kNode->ChildNodes->Item(jj);
						kNode->RemoveChild(kChild);


						pElevator->m_pmPointList->Remove(pPoint);
						MFramework::Instance->Scene->RemoveEntity(pkEntity, false);
						break;
					}
				}
			}
		}
	}
}

void MPgElevator::RearrangeElevatorList()
{//포인트를 가지지 않은(빈) 엘리베이터를 지운다. 
	for(int iDeleteIndex=0;m_pmElevatorList->Count > iDeleteIndex;++iDeleteIndex)
	{
		SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(iDeleteIndex));
		if(0==pElevator->m_pmPointList->Count)
		{
			m_pmElevatorList->Remove(pElevator);
			if(0<iDeleteIndex) 
			{
				--iDeleteIndex;
			}
		}
	}
}

MEntity* MPgElevator::AddPointEntity(MEntity* pkEntity)
{
	SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(m_iSelectElevatorList));
	if(pElevator)
	{
		SPointList* pPoint = new SPointList;
		NiPoint3 kEntityPos;
		pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
		pPoint->m_fPosX = kEntityPos.x;
		pPoint->m_fPosY = kEntityPos.y;
		pPoint->m_fPosZ = kEntityPos.z;
		pPoint->m_pmEntity = pkEntity;
		pPoint->m_fTime = m_fPointTime;
		pElevator->m_pmPointList->Add(pPoint);

		//MFramework::Instance->Scene->AddEntity(pkEntity, false);

		m_bAdd = true;
		m_iAddElevatorIndex = m_iSelectElevatorList;
		m_iAddPointIndex = pElevator->m_pmPointList->Count-1;

		return pkEntity;
	}

	return NULL;
}

MEntity* MPgElevator::TranslatePointEntity(MEntity* pkEntity, NiPoint3 kPos)
{
	for(int ii=0; ii<m_pmElevatorList->Count; ++ii)
	{
		SElevatorList* pElevator = dynamic_cast<SElevatorList*>(m_pmElevatorList->get_Item(ii));
		if(pElevator)
		{
			for(int jj=0; jj<pElevator->m_pmPointList->Count; ++jj)
			{
				SPointList* pPoint = dynamic_cast<SPointList*>(pElevator->m_pmPointList->get_Item(jj));
				if(pPoint)
				{
					if(pPoint->m_pmEntity == pkEntity)
					{
						pPoint->m_fPosX = kPos.x;
						pPoint->m_fPosY = kPos.y;
						pPoint->m_fPosZ = kPos.z;

						m_bTranslate = true;
						STranslateData* pkData = new STranslateData;
						pkData->Set(ii, jj);
						m_pmTranslateList->Add(pkData);

						return pkEntity;
					}
				}
			}
		}
	}

	return NULL;
}
void MPgElevator::SetSyncData(bool bSync) 
{ 
	m_bSync = bSync; 
}
bool MPgElevator::GetSyncData() 
{
	return m_bSync; 
}