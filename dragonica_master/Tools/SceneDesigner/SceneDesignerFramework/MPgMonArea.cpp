//
// HandOver, 강정욱 2008.01.29
//
// 쓰지 않는 코드.
// 몬스터 WayPoint를 다른 구조로 변경 할 때 만들었지만.
// 기존 WayPoint를 쓰기 했으므로 코드는 쓰지 않음..
//
// UI : MonsterArea.cs
//
#include "SceneDesignerFrameworkPCH.h"
//#include "MFramework.h"
//#include "IEntityPathService.h"
//#include "MUtility.h"
//#include "PgMonAreaComponent.h"
//#include "MPgMonArea.h"
//#using <System.Xml.dll>
//
//using namespace Emergent::Gamebryo::SceneDesigner::Framework;
//using namespace System::Text;
//using namespace System::Xml;
//
//MPgMonArea::MPgMonArea(void)
//{
//	m_iCurrentAreaID = 0;
//	m_pkConsole = 0;
//	m_pkTargetList = new ArrayList;
//	m_pkAreaList = new ArrayList;
//}
//
//MPgMonArea::~MPgMonArea(void)
//{
//    MDisposeRefObject(m_pkConsole);
//}
//
//ISelectionService* MPgMonArea::get_SelectionService()
//{
//    if (ms_pmSelectionService == NULL)
//    {
//        ms_pmSelectionService = MGetService(ISelectionService);
//        MAssert(ms_pmSelectionService != NULL, "Selection service not "
//            "found!");
//    }
//    return ms_pmSelectionService;
//}
//
//bool MPgMonArea::Initialize()
//{
//	CreateConsole();
//
//	return true;
//}
//
//void MPgMonArea::Clear()
//{
//    m_pkTargetList->Clear();
//    m_pkAreaList->Clear();
//	m_pkMonAreaID = 0;;
//	m_iCurrentAreaID = 0;
//}
//
//bool MPgMonArea::AddMonAreaTarget(MEntity *pkEntity)
//{
//	if (m_pkTargetList->Count > 2)
//	{
//		return false;
//	}
//	m_pkTargetList->Add(pkEntity);
//
//	return true;
//}
//
//void MPgMonArea::DeleteMonAreaTarget(MEntity *pkEntity, bool bRemoveInScene)
//{
//	for (int i=0 ; i<m_pkTargetList->Count ; i++)
//	{
//		MEntity* pkData =
//			dynamic_cast<MEntity*>(m_pkTargetList->get_Item(i));
//
//		if (pkData->GetNiEntityInterface() == pkEntity->GetNiEntityInterface())
//		{
//			m_pkTargetList->Remove(pkData);
//			if (bRemoveInScene)
//			{
//				MFramework::Instance->Scene->RemoveEntity(pkEntity, false);
//			}
//			return;
//		}
//	}
//}
//
//void MPgMonArea::MakeMonArea()
//{
//	if (m_pkTargetList->Count < 2)
//	{
//		return ;
//	}
//	IEntityPathService* pmPathService = MGetService(IEntityPathService);
//
//	// 1
//	MEntity* pkEntity1 = dynamic_cast<MEntity*>(m_pkTargetList->get_Item(0));
//
//	// 2
//	MEntity* pkEntity2 = dynamic_cast<MEntity*>(m_pkTargetList->get_Item(1));
//
//	if (pkEntity1 && pkEntity2)
//	{
//		MEntity* pkNewArea = CreateMonArea(pkEntity1->GetNiEntityInterface(), pkEntity2->GetNiEntityInterface(), true);
//
//		MonAreaEntity* pkData = new MonAreaEntity();
//		pkData->m_pkEntity = pkNewArea;
//		pkData->m_iAreaID = m_iCurrentAreaID;
//		m_pkAreaList->Add(pkData);
//	}
//}
//
//void MPgMonArea::MakeMonAreaCircle()
//{
//	if (m_pkTargetList->Count < 2)
//	{
//		return ;
//	}
//	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
//    if (pkPalette == NULL)
//	{
//		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
//	    return;
//	}
//	String* strEntityName = "[General]Target.waypoint_circle";
//	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
//    if (pkTemplate == NULL)
//	{
//		::MessageBox(0, "[General]Target.waypoint_circle 가 없습니다.", 0, 0);
//	    return;
//	}
//
//	MEntity* pkEntity1 = dynamic_cast<MEntity*>(m_pkTargetList->get_Item(0));
//	MEntity* pkEntity2 = dynamic_cast<MEntity*>(m_pkTargetList->get_Item(1));
//	if (pkEntity1 && pkEntity2)
//	{
//		MEntity* pkNewArea = CreateMonArea(pkEntity1->GetNiEntityInterface(), pkEntity2->GetNiEntityInterface(), false);
//
//		MonAreaEntity* pkData = new MonAreaEntity();
//		pkData->m_pkEntity = pkNewArea;
//		pkData->m_iAreaID = m_iCurrentAreaID;
//		m_pkAreaList->Add(pkData);
//	}
//}
//
//void MPgMonArea::DeleteMonArea(MEntity *pkMonArea, bool bRemoveInScene)
//{
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//
//		if (pkData->m_pkEntity->TemplateID == pkMonArea->TemplateID)
//		{
//			m_pkAreaList->Remove(pkData);
//			if (bRemoveInScene)
//			{
//				MFramework::Instance->Scene->RemoveEntity(pkMonArea, false);
//			}
//			break;
//		}
//	}
//}
//
//void MPgMonArea::RevertPoint()
//{
//	MEntity *pkEntities[] = SelectionService->GetSelectedEntities();
//	for (int i=0 ; i<pkEntities->Count ; i++)
//	{
//		MEntity* pkEntity = pkEntities[i];
//		NiEntityInterface* pkInter = (NiEntityInterface*)pkEntity->PropertyInterface;
//
//		for (int j=0 ; j<pkInter->GetComponentCount() ; j++)
//		{
//			if (pkInter->GetComponentAt(j)->GetClassName().Equals("PgMonAreaComponent"))
//			{
//				// PgMonAreaComponent 가 있다!
//				// 뒤집어 주자.
//				PgMonAreaComponent* pkCompo = (PgMonAreaComponent*)pkInter->GetComponentAt(j);
//				pkCompo->RevertPoint();
//			}
//		}
//	}
//}
//
//void MPgMonArea::ExtractPoint()
//{
//	ErasePoint();
//
//	if (SelectionService->GetSelectedEntities()->Count > 0)
//	{
//		MEntity* pkEntity = SelectionService->GetSelectedEntities()[0];
//		NiEntityInterface* pkInter = (NiEntityInterface*)pkEntity->PropertyInterface;
//
//		for (int j=0 ; j<pkInter->GetComponentCount() ; j++)
//		{
//			if (pkInter->GetComponentAt(j)->GetClassName().Equals("PgMonAreaComponent"))
//			{
//				// PgMonAreaComponent 가 있다!
//				// 점 추출!
//				PgMonAreaComponent* pkCompo = (PgMonAreaComponent*)pkInter->GetComponentAt(j);
//				MEntity* outEntity1;
//				MEntity* outEntity2;
//				pkCompo->ExtractPoint(outEntity1, outEntity2);
//
//				m_pkTargetList->Add(outEntity1);
//				m_pkTargetList->Add(outEntity2);
//			}
//		}
//	}
//}
//
//void MPgMonArea::ErasePoint()
//{
//	for (int i=0 ; i<m_pkTargetList->Count ; i++)
//	{
//		MEntity* pkData =
//			dynamic_cast<MEntity*>(m_pkTargetList->get_Item(i));
//		MFramework::Instance->Scene->RemoveEntity(pkData, false);
//	}
//
//	m_pkTargetList->Clear();
//}
//
//void MPgMonArea::SetMonAreaID(String* pkMonAreaID)
//{
//	m_pkMonAreaID = pkMonAreaID;
//}
//
//String* MPgMonArea::GetMonAreaID()
//{
//	return m_pkMonAreaID;
//}
//
//void MPgMonArea::SaveToXML(String* pkFilename)
//{
//	XmlDocument* pkDocument = new XmlDocument();
//	pkDocument->PreserveWhitespace = true;
//
//	// 내용 채워넣자.
//	// Version
//    XmlDeclaration* newDec = pkDocument->CreateXmlDeclaration("1.0", "euc-kr", 0);
//    pkDocument->AppendChild(newDec);
//
//	// Root
//	XmlElement* newRoot = pkDocument->CreateElement("MONAREA");
//    newRoot->SetAttribute("ID", m_pkMonAreaID);
//    pkDocument->AppendChild(newRoot);
//
//	// Entity 수
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		PgMonAreaComponent* pkCompo = GetMonAreaComponent(pkData->m_pkEntity);
//		if (!pkCompo)
//		{
//			continue;
//		}
//
//		// Sub - Area
//		XmlElement* pElement = pkDocument->CreateElement("AREA");
//		pElement->InnerText = "";
//		pElement->SetAttribute("ID", pkData->m_iAreaID.ToString());
//		int iAreaType = (int)pkCompo->GetMonAreaType() + 1;
//		pElement->SetAttribute("AREA_TYPE", iAreaType.ToString());
//		newRoot->AppendChild(pElement);
//
//		// Element in Area
//		if (pkCompo->GetMonAreaType() == PgMonAreaComponent::Rectangle)
//		{
//			// 4 고정.
//			XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Comment, "", 0);
//			pNode->InnerText = "AREA_TYPE1(Plane)";
//			pElement->AppendChild(pNode);
//
//			pNode = pkDocument->CreateElement("IS_REVERT");
//			pNode->InnerText = pkCompo->IsRevert() ? "1" : "0";
//			pElement->AppendChild(pNode);
//
//			for (int j=0 ; j<4 ; j++)
//			{
//				NiPoint3 kPt = pkCompo->m_pkTriStrips->GetVertices()[j] + pkCompo->m_pkTriStrips->GetTranslate();
//			
//				String* pkPosition = "POSITION";
//				pkPosition = pkPosition->Concat(pkPosition, (j+1).ToString());
//				pNode = pkDocument->CreateElement(pkPosition);
//				String* pkText = "";
//				pkText = pkText->Concat(pkText, kPt.x.ToString());
//				pkText = pkText->Concat(pkText, ",");
//				pkText = pkText->Concat(pkText, kPt.y.ToString());
//				pkText = pkText->Concat(pkText, ",");
//				pkText = pkText->Concat(pkText, kPt.z.ToString());
//				pNode->InnerText = pkText;
//				pElement->AppendChild(pNode);
//			}
//			NiPoint3 kPt = pkCompo->GetNormalVec();
//			pNode = pkDocument->CreateElement("NORMAL_VEC");
//			String* pkText = "";
//			pkText = pkText->Concat(pkText, kPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//		}
//		else if (pkCompo->GetMonAreaType() == PgMonAreaComponent::Circle)
//		{
//			XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Comment, "", 0);
//			pNode->InnerText = "AREA_TYPE2(Circle)";
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kCenterPt = pkCompo->m_pkCircle->GetTranslate();
//			pNode = pkDocument->CreateElement("CENTER");
//			String* pkText = "";
//			pkText = pkText->Concat(pkText, kCenterPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kCenterPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kCenterPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//
//			pNode = pkDocument->CreateElement("RADIUS");
//			pNode->InnerText = pkCompo->m_fRadius.ToString();
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kDestPt = pkCompo->m_pkCircle->GetWorldTransform() * pkCompo->m_pkCircle->GetVertices()[0];
//			pNode = pkDocument->CreateElement("DEST_POS");
//			pkText = "";
//			pkText = pkText->Concat(pkText, kDestPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kDestPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kDestPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kNormPt = pkCompo->GetNormalVec();
//			pNode = pkDocument->CreateElement("NORMAL_VEC");
//			pkText = "";
//			pkText = pkText->Concat(pkText, kNormPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kNormPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kNormPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//		}
//
//		// Save Link Area
//		for (int j=0 ; j<pkData->m_aiLinkArea->get_Count() ; j++)
//		{
//			String* strID = dynamic_cast<String*>(pkData->m_aiLinkArea->get_Item(j));
//			if (strID)
//			{
//				XmlNode* pNode = pkDocument->CreateElement("LINK_AREA");
//				pNode->InnerText = strID;
//				pElement->AppendChild(pNode);
//			}
//		}
//	}
//
//	//XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Element, "GSAPATH", 0);
// //   pNode->InnerText = "";
// //   newRoot->AppendChild(pNode);
//
//	// Save
//	XmlTextWriter* tr = new XmlTextWriter(pkFilename, Encoding::GetEncoding(949));
//	tr->Formatting = Formatting::Indented;
//	pkDocument->WriteContentTo(tr);
//	tr->Close();
//}
//
//NiPoint3 MPgMonArea::XMLPtToNiPoint(String* strPoint)
//{
//	NiPoint3 kRet;
//	String* kX = "";
//	String* kY = "";
//	String* kZ = "";
//
//	kX = strPoint->Substring(0, strPoint->IndexOf(","));
//	kY = strPoint->Substring(strPoint->IndexOf(",")+1, strPoint->LastIndexOf(",") - (strPoint->IndexOf(",")+1));
//	kZ = strPoint->Substring(strPoint->LastIndexOf(",")+1, strPoint->Length - (strPoint->LastIndexOf(",")+1));
//
//	float __gc *fResult = new float;
//	if (!Single::TryParse(kX, fResult))
//	{
//		MAssert(0);
//	}
//	kRet.x = *fResult;
//	if (!Single::TryParse(kY, fResult))
//	{
//		MAssert(0);
//	}
//	kRet.y = *fResult;
//	if (!Single::TryParse(kZ, fResult))
//	{
//		MAssert(0);
//	}
//	kRet.z = *fResult;
//
//	return kRet;
//}
//
//void MPgMonArea::LoadFromXML(String* pkFilename)
//{
//	for (int i=0 ; i<m_pkTargetList->Count ; i++)
//	{
//		MEntity* pkData = dynamic_cast<MEntity*>(m_pkTargetList->get_Item(i));
//		DeleteMonAreaTarget(pkData, true);
//	}
//	m_pkTargetList->Clear();
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		DeleteMonArea(pkData->m_pkEntity, true);
//	}
//	m_pkAreaList->Clear();
//
//	XmlDocument* pkDocument = new XmlDocument();
//	pkDocument->Load(pkFilename);
//	pkDocument->PreserveWhitespace = true;
//
//	// 내용 채워넣자.
//	int __gc *iResult = new int;
//	XmlElement* pRoot = pkDocument->DocumentElement;
//	m_pkMonAreaID = pRoot->Attributes->GetNamedItem("ID")->InnerText;
//	
//	XmlNodeList* nodeList = pkDocument->GetElementsByTagName("AREA");
//	for (int i=0 ; i<nodeList->Count ; i++)
//	{
//		MonAreaEntity* pkData = new MonAreaEntity();
//		//m_pkAreaList = new ArrayList;
//		XmlNode* pkNode = nodeList->get_ItemOf(i);
//
//		int __gc *iResult = new int;
//		if (Int32::TryParse(pkNode->Attributes->GetNamedItem("ID")->InnerText, iResult))
//		{
//			pkData->m_iAreaID = *iResult;
//		}
//		else
//		{
//			continue;
//		}
//
//		int iAreaType = -1;
//		if (Int32::TryParse(pkNode->Attributes->GetNamedItem("AREA_TYPE")->InnerText, iResult))
//		{
//			iAreaType = *iResult;
//		}
//		else
//		{
//			continue;
//		}
//
//		ArrayList* pkLinkArea = new ArrayList;
//		PgMonAreaComponent::EPGMonAreaType eType = (PgMonAreaComponent::EPGMonAreaType)(iAreaType - 1);
//		if (eType == PgMonAreaComponent::Rectangle)
//		{
//			NiPoint3 akPoint[4];
//			NiPoint3 kNormPoint;
//			bool bRevert;
//			for (int j=0 ; j<pkNode->ChildNodes->Count ; j++)
//			{
//				XmlNode* pkElement = pkNode->ChildNodes->get_ItemOf(j);
//				if (pkElement->Name->Equals("POSITION1"))
//					akPoint[0] = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("POSITION2"))
//					akPoint[1] = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("POSITION3"))
//					akPoint[2] = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("POSITION4"))
//					akPoint[3] = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("NORMAL_VEC"))
//					kNormPoint = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("LINK_AREA"))
//					pkLinkArea->Add(pkElement->InnerText);
//				else if (pkElement->Name->Equals("IS_REVERT"))
//					bRevert = pkElement->InnerText->Equals("1") ? true : false;
//			}
//
//			int ti=0, tj=3;
//			if (akPoint[0].x != akPoint[3].x &&
//				akPoint[0].y != akPoint[3].y &&
//				akPoint[0].z != akPoint[3].z)
//			{
//				ti = 0;
//				tj = 3;
//			}
//			else
//			{
//				for (int pi=0 ; pi<4 ; ++pi)
//				{
//					for (int pj=pi ; pj<4 ; ++pj)
//					{
//						if (akPoint[pi].x != akPoint[pj].x &&
//							akPoint[pi].y != akPoint[pj].y &&
//							akPoint[pi].z != akPoint[pj].z)
//						{
//							ti = pi;
//							tj = pj;
//							pi = 4;
//							pj = 4;
//						}
//					}
//				}
//			}
//
//			MEntity* pkEntity = CreateMonArea(akPoint[ti], akPoint[tj], true);
//			pkData->m_pkEntity = pkEntity;
//			if (bRevert)
//			{
//				PgMonAreaComponent* pkCompo = (PgMonAreaComponent*)GetMonAreaComponent(pkEntity);
//				pkCompo->RevertPoint();
//			}
//		}
//		else if (eType == PgMonAreaComponent::Circle)
//		{
//			String* strRadius;
//			NiPoint3 kCenterPoint;
//			NiPoint3 kDestPoint;
//			NiPoint3 kNormPoint;
//			for (int j=0 ; j<pkNode->ChildNodes->Count ; j++)
//			{
//				XmlNode* pkElement = pkNode->ChildNodes->get_ItemOf(j);
//				if (pkElement->Name->Equals("CENTER"))
//					kCenterPoint = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("RADIUS"))
//				{
//					strRadius = pkElement->InnerText;
// 					float __gc *fResult = new float;
//					if (!Single::TryParse(strRadius, fResult))
//					{
//						MAssert(0);
//					}
//					float fRadius = *fResult;
//				}
//				else if (pkElement->Name->Equals("DEST_POS"))
//					kDestPoint = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("NORMAL_VEC"))
//					kNormPoint = XMLPtToNiPoint(pkElement->InnerText);
//				else if (pkElement->Name->Equals("LINK_AREA"))
//					pkLinkArea->Add(pkElement->InnerText);
//			}
//
//			MEntity* pkEntity = CreateMonArea(kCenterPoint, kDestPoint, false);
//			pkData->m_pkEntity = pkEntity;
//		}
//
//		// Add Link
//		// Save Link Area
//		for (int j=0 ; j<pkLinkArea->get_Count() ; j++)
//		{
//			pkData->m_aiLinkArea->Add(pkLinkArea->get_Item(j));
//		}
//
//		m_pkAreaList->Add(pkData);
//	}
//
//	/*
//    // Npc 수만큼 루프
//    foreach (XmlNode node in nodeList)
//    {
//        string strAdd = null;
//
//        for (int i = 0; i < node.Attributes.Count; i++)
//        {
//            XmlNode childnode = node.Attributes.Item(i);
//
//            strAdd = strAdd + " [" +
//                childnode.InnerText + "]";
//        }
//
//        m_lbNpcSets.Items.Add(strAdd);
//
//        // Create NPC Entity
//        MEntity pkNewEntity;
//        pkNewEntity = pkTemplate.Clone(node.Attributes.GetNamedItem("NAME").InnerText, true);
//
//        XML_LOCATION kLoc = StringsToLocation(node.Attributes.GetNamedItem("LOCATION").InnerText);
//        MPoint3 translation = new MPoint3(kLoc.fLocX, kLoc.fLocY, kLoc.fLocZ);
//        pkNewEntity.SetPropertyData("Translation", translation, false);
//        MFramework.Instance.Scene.AddEntity(pkNewEntity, false);
//    }
//	*/
//
//
//
//	//CreateMonArea(,,);
//
//
//
//	/*
//	// Root
//	XmlElement* newRoot = pkDocument->CreateElement("MONAREA");
//    newRoot->SetAttribute("ID", m_pkMonAreaID);
//    pkDocument->AppendChild(newRoot);
//
//	// Entity 수
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		PgMonAreaComponent* pkCompo = GetMonAreaComponent(pkData->m_pkEntity);
//		if (!pkCompo)
//		{
//			continue;
//		}
//
//		// Sub - Area
//		XmlElement* pElement = pkDocument->CreateElement("AREA");
//		pElement->InnerText = "";
//		pElement->SetAttribute("ID", pkData->m_iAreaID.ToString());
//		int iAreaType = (int)pkCompo->GetMonAreaType() + 1;
//		pElement->SetAttribute("AREA_TYPE", iAreaType.ToString());
//		newRoot->AppendChild(pElement);
//
//		// Element in Area
//		if (pkCompo->GetMonAreaType() == PgMonAreaComponent::Rectangle)
//		{
//			// 4 고정.
//			XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Comment, "", 0);
//			pNode->InnerText = "AREA_TYPE1(Plane)";
//			pElement->AppendChild(pNode);
//			for (int j=0 ; j<4 ; j++)
//			{
//				NiPoint3 kPt = pkCompo->m_pkTriStrips->GetVertices()[j] + pkCompo->m_pkTriStrips->GetTranslate();
//			
//				String* pkPosition = "POSITION";
//				pkPosition = pkPosition->Concat(pkPosition, (j+1).ToString());
//				pNode = pkDocument->CreateElement(pkPosition);
//				String* pkText = "";
//				pkText = pkText->Concat(pkText, kPt.x.ToString());
//				pkText = pkText->Concat(pkText, ",");
//				pkText = pkText->Concat(pkText, kPt.y.ToString());
//				pkText = pkText->Concat(pkText, ",");
//				pkText = pkText->Concat(pkText, kPt.z.ToString());
//				pNode->InnerText = pkText;
//				pElement->AppendChild(pNode);
//			}
//			NiPoint3 kPt = pkCompo->GetNormalVec();
//			pNode = pkDocument->CreateElement("NORMAL_VEC");
//			String* pkText;
//			pkText = pkText->Concat(pkText, kPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//		}
//		else if (pkCompo->GetMonAreaType() == PgMonAreaComponent::Circle)
//		{
//			XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Comment, "", 0);
//			pNode->InnerText = "AREA_TYPE2(Circle)";
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kCenterPt = pkCompo->m_pkCircle->GetTranslate();
//			pNode = pkDocument->CreateElement("CENTER");
//			String* pkText = "";
//			pkText = pkText->Concat(pkText, kCenterPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kCenterPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kCenterPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//
//			pNode = pkDocument->CreateElement("RADIUS");
//			pNode->InnerText = pkCompo->m_fRadius.ToString();
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kDestPt = pkCompo->m_pkCircle->GetVertices()[0];
//			pNode = pkDocument->CreateElement("DEST_POS");
//			pkText = "";
//			pkText = pkText->Concat(pkText, kDestPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kDestPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kDestPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//
//			NiPoint3 kNormPt = pkCompo->GetNormalVec();
//			pNode = pkDocument->CreateElement("NORMAL_VEC");
//			pkText = "";
//			pkText = pkText->Concat(pkText, kNormPt.x.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kNormPt.y.ToString());
//			pkText = pkText->Concat(pkText, ",");
//			pkText = pkText->Concat(pkText, kNormPt.z.ToString());
//			pNode->InnerText = pkText;
//			pElement->AppendChild(pNode);
//		}
//
//		// Save Link Area
//		for (int j=0 ; j<pkData->m_aiLinkArea->get_Count() ; j++)
//		{
//			String* strID = dynamic_cast<String*>(pkData->m_aiLinkArea->get_Item(j));
//			if (strID)
//			{
//				XmlNode* pNode = pkDocument->CreateElement("LINK_AREA");
//				pNode->InnerText = strID;
//				pElement->AppendChild(pNode);
//			}
//		}
//	}
//
//	//XmlNode* pNode = pkDocument->CreateNode(XmlNodeType::Element, "GSAPATH", 0);
// //   pNode->InnerText = "";
// //   newRoot->AppendChild(pNode);
//
//	// Save
//	XmlTextWriter* tr = new XmlTextWriter(pkFilename, Encoding::GetEncoding(949));
//	tr->Formatting = Formatting::Indented;
//	pkDocument->WriteContentTo(tr);
//	tr->Close();
//	*/
//}
//
//int MPgMonArea::GetSelectedAreaID(MEntity* pkEntity)
//{
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		if (pkData->m_pkEntity->TemplateID == pkEntity->TemplateID)
//		{
//			return pkData->m_iAreaID;
//		}
//	}
//
//	return -1;
//}
//
//ArrayList* MPgMonArea::GetLinkAreaList(MEntity* pkEntity)
//{
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		if (pkData->m_pkEntity->TemplateID == pkEntity->TemplateID)
//		{
//			return pkData->m_aiLinkArea;
//		}
//	}
//
//	return 0;
//}
//
//void MPgMonArea::SetAreaID(int iAreaID)
//{
//	m_iCurrentAreaID = iAreaID;
//
//	MEntity *pkEntities[] = SelectionService->GetSelectedEntities();
//	if (pkEntities->Count == 1 && GetMonAreaComponent(pkEntities[0]) )
//	{
//		for (int i=0 ; i<m_pkAreaList->Count ; i++)
//		{
//			MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//			if (pkData->m_pkEntity->TemplateID == pkEntities[0]->TemplateID)
//			{
//				// Set Current
//				pkData->m_iAreaID = iAreaID;
//				return ;
//			}
//		}
//	}
//}
//
//bool MPgMonArea::ModifyLinkAreaList(MEntity* pkEntity, int iOrg, int iAfter)
//{
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		if (pkData->m_pkEntity->TemplateID == pkEntity->TemplateID)
//		{
//			for (int j=0 ; j<pkData->m_aiLinkArea->get_Count() ; j++)
//			{
//				String* strID = dynamic_cast<String*>(pkData->m_aiLinkArea->get_Item(j));
//				if (strID &&
//					strID->Equals(iOrg.ToString()))
//				{
//					// modify
//					strID = iAfter.ToString();
//					pkData->m_aiLinkArea->set_Item(j, strID);
//					return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
//bool MPgMonArea::RemoveLinkAreaList(MEntity* pkEntity, int iOrg)
//{
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		if (pkData->m_pkEntity->TemplateID == pkEntity->TemplateID)
//		{
//			for (int j=0 ; j<pkData->m_aiLinkArea->get_Count() ; j++)
//			{
//				String* strID = dynamic_cast<String*>(pkData->m_aiLinkArea->get_Item(j));
//				if (strID &&
//					strID->Equals(iOrg.ToString()))
//				{
//					// Remove
//					pkData->m_aiLinkArea->Remove(strID);
//					return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
//// 사각형, 원 둘다 만들어 준다. bIsRectangle 이 false면 원.
//MEntity* MPgMonArea::CreateMonArea(NiEntityInterface* pkEntity1, NiEntityInterface* pkEntity2, bool bIsRectangle)
//{
//    IEntityPathService* pmPathService = MGetService(IEntityPathService);
//
//	String* strCloneName;
//	strCloneName = MFramework::Instance->Scene->GetUniqueEntityName(
//		String::Concat(pmPathService->GetSimpleName("MonArea"),
//		" 01"));
//
//    NiUniqueID kTemplateID;
//    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
//    const char* pcEntityName = MStringToCharPointer(strCloneName);
//	NiEntityInterface* pkEntity = NiNew NiGeneralEntity(pcEntityName, kTemplateID, 2);
//    MFreeCharPointer(pcEntityName);
//	pkEntity->AddComponent(NiNew NiTransformationComponent());
//	PgMonAreaComponent* pkComponent = NiNew PgMonAreaComponent(pkEntity1, pkEntity2, bIsRectangle);
//	pkEntity->AddComponent(pkComponent);
//	MEntity* pmEntity = new MEntity(pkEntity);
//
//	MFramework::Instance->Scene->AddEntity(pmEntity, false);
//
//	return pmEntity;
//}
//
//MEntity* MPgMonArea::CreateMonArea(NiPoint3 kPoint1, NiPoint3 kPoint2, bool bIsRectangle)
//{
//    IEntityPathService* pmPathService = MGetService(IEntityPathService);
//
//	String* strCloneName;
//	strCloneName = MFramework::Instance->Scene->GetUniqueEntityName(
//		String::Concat(pmPathService->GetSimpleName("MonArea"),
//		" 01"));
//
//    NiUniqueID kTemplateID;
//    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
//    const char* pcEntityName = MStringToCharPointer(strCloneName);
//	NiEntityInterface* pkEntity = NiNew NiGeneralEntity(pcEntityName, kTemplateID, 2);
//    MFreeCharPointer(pcEntityName);
//	pkEntity->AddComponent(NiNew NiTransformationComponent());
//	PgMonAreaComponent* pkComponent = NiNew PgMonAreaComponent(kPoint1, kPoint2, bIsRectangle);
//	pkEntity->AddComponent(pkComponent);
//	MEntity* pmEntity = new MEntity(pkEntity);
//
//	MFramework::Instance->Scene->AddEntity(pmEntity, false);
//
//	return pmEntity;
//}
//
//PgMonAreaComponent* MPgMonArea::GetMonAreaComponent(MEntity* pkEntity)
//{
//	return GetMonAreaComponent(pkEntity->GetNiEntityInterface());
//}
//
//PgMonAreaComponent* MPgMonArea::GetMonAreaComponent(NiEntityInterface* pkInterface)
//{
//	PgMonAreaComponent* pkCompo = 0;
//	for (int j=0 ; j<pkInterface->GetComponentCount() ; j++)
//	{
//		if (pkInterface->GetComponentAt(j)->GetClassName().Equals("PgMonAreaComponent"))
//		{
//			pkCompo = (PgMonAreaComponent*)pkInterface->GetComponentAt(j);
//			break;
//		}
//	}
//
//	return pkCompo;
//}
//
////---------------------------------------------------------------------------
//void MPgMonArea::CreateConsole()
//{
//    if (!m_pkConsole)
//    {
//		if (MFramework::Instance)
//		{
//			m_pkConsole = NiNew NiScreenConsole();
//			MInitRefObject(m_pkConsole);
//
//			const char* pcPath = MStringToCharPointer(String::Concat(
//				MFramework::Instance->AppStartupPath, "Data\\"));
//			m_pkConsole->SetDefaultFontPath(pcPath);
//			MFreeCharPointer(pcPath);
//
//			m_pkConsole->SetFont(m_pkConsole->CreateConsoleFont());
//			m_pkConsole->Enable(true);
//		}
//    }
//}
////---------------------------------------------------------------------------
//void MPgMonArea::Render(MRenderingContext* pmRenderingContext)
//{
//    NiEntityRenderingContext* pkContext = 
//        pmRenderingContext->GetRenderingContext();
//    NiCamera* pkCam = pkContext->m_pkCamera;
//    //clear the z-buffer
//    pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);
//
//	for (int i=0 ; i<m_pkAreaList->Count ; i++)
//	{
//		MonAreaEntity* pkData = dynamic_cast<MonAreaEntity*>(m_pkAreaList->get_Item(i));
//		RenderConsole(pmRenderingContext, pkData);
//	}
//}
////---------------------------------------------------------------------------
//void MPgMonArea::RenderConsole(MRenderingContext* pmRenderingContext, MonAreaEntity* pkData)
//{
//    NiRenderer* pkRenderer = pmRenderingContext
//        ->GetRenderingContext()->m_pkRenderer;
//    //pkRenderer->SetScreenSpaceCameraData();
//
//	CreateConsole();
//	// Render screen console.
//    if (m_pkConsole)
//    {
//		MEntity *pkEntity = 0;
//		pkEntity = pkData->m_pkEntity;
//
//		float fWidth=0, fHeight=0;
//		MViewport *pkViewport = 0;
//		if (MFramework::Instance->ViewportManager->get_ExclusiveViewport())
//		{
//			pkViewport = MFramework::Instance->ViewportManager->get_ExclusiveViewport();
//		}
//		else if (MFramework::Instance->ViewportManager->get_ActiveViewport())
//		{
//			pkViewport = MFramework::Instance->ViewportManager->get_ActiveViewport();
//		}
//		if (!pkViewport)
//			return;
//
//		fWidth = (float)pkViewport->Width;
//		fHeight = (float)pkViewport->Height;
//		m_pkConsole->SetDimensions(NiPoint2(fWidth, fHeight));
//
//		MEntity* pmCameraEntity = pkViewport->get_CameraEntity();
//	    NiAVObject* pkCameraObject = pmCameraEntity->GetSceneRootPointer(0);
//		NiCamera* pkCamera = NiDynamicCast(NiCamera, pkCameraObject);
//		if (!pkCamera)
//			return;
//
//		NiEntityPropertyInterface* pkEntityProp = 
//			pkEntity->GetNiEntityInterface();
//		NiPoint3 kPoint;
//		pkEntityProp->GetPropertyData("Translation", kPoint);
//
//		kPoint.z = kPoint.z;
//
//		float fX, fY;
//		kPoint.z = kPoint.z + 100;
//		pkCamera->WorldPtToScreenPt(kPoint, fX, fY);
//		float fCenterX = (fX) * fWidth;
//		float fCenterY = (1-fY) * fHeight;
//		if (fCenterX > fWidth - fWidth*0.05f || fCenterX < fWidth*0.05f ||
//			fCenterY > fHeight - fHeight*0.03f || fCenterY < fHeight*0.03f)
//			return;
//
//		// Render Text
//		String *strConsole = pkData->m_iAreaID.ToString();
//		strConsole = strConsole->Insert(strConsole->Length, "_");
//		if (GetMonAreaComponent(pkData->m_pkEntity)->GetMonAreaType() == PgMonAreaComponent::Rectangle)
//		{
//			strConsole = strConsole->Concat(strConsole, "Rectangle");
//		}
//		else if (GetMonAreaComponent(pkData->m_pkEntity)->GetMonAreaType() == PgMonAreaComponent::Circle)
//		{
//			strConsole = strConsole->Concat(strConsole, "Circle");
//		}
//		float fDx = (float)strConsole->Length * (float)m_pkConsole->GetFont()->m_uiCharWidth * 0.5f;
//		float fDy = (float)m_pkConsole->GetFont()->m_uiCharHeight * 0.5f;
//		float fRenderX = fCenterX - fDx;
//		float fRenderY = fCenterY + fDy;
//
//		m_pkConsole->SetOrigin(NiPoint2(fRenderX, fRenderY));
//		m_pkConsole->SetCamera(pkCamera);
//		const char* pcText = MStringToCharPointer(strConsole);
//		m_pkConsole->SetLine(pcText, 0);
//		MFreeCharPointer(pcText);
//		m_pkConsole->RecreateText();
//
//
//		//////////////////////////////////////////////////////////////////////////////
//        NiScreenTexture* pkConsoleTexture = m_pkConsole
//            ->GetActiveScreenTexture();
//        if (pkConsoleTexture)
//        {
//            pkConsoleTexture->Draw(pkRenderer);
//        }
//    }
//}