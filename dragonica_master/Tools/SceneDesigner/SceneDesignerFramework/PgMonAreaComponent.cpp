//
// HandOver, 강정욱 2008.01.29
//
// MonsterWay의 다음 버젼으로 만들려던 MonsterArea 입니다.
// Component를 상속받고 컴퍼넌트로써 쓰일려고 했는데 쓰이지 않아서 주석 처리 했습니다.
//
#include "SceneDesignerFrameworkPCH.h"
//#include "MFramework.h"
//#include "IEntityPathService.h"
//#include "PgMonAreaComponent.h"
//
//PgMonAreaComponent::PgMonAreaComponent(void)
//{
//	m_pkTriStrips = 0;
//	m_pkTriStrips2 = 0;
//}
//
//PgMonAreaComponent::~PgMonAreaComponent(void)
//{
//	m_pkTriStrips = 0;
//	m_pkTriStrips2 = 0;
//	m_pkSceneRoot = 0;
//	m_spSceneRoot = 0;
//}
//
//PgMonAreaComponent::PgMonAreaComponent(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity, bool bIsRectangle)
//{
//	m_pkStartEntity = 0;
//	m_pkEndEntity = 0;
//	m_pkTriStrips = 0;
//	m_pkTriStrips2 = 0;
//	m_pkCircle = 0;
//	m_fRadius = 0;
//	m_pkSceneRoot = NiNew NiNode();
//	m_bRevertPoint = false;
//
//	if (bIsRectangle)
//	{
//		CreateTriStrips(pkStartEntity, pkEndEntity);
//		m_eMonAreaType = Rectangle;
//	}
//	else
//	{
//		CreateCircle(pkStartEntity, pkEndEntity);
//		m_eMonAreaType = Circle;
//	}
//}
//
//PgMonAreaComponent::PgMonAreaComponent(NiPoint3 kPoint1, NiPoint3 kPoint2, bool bIsRectangle)
//{
//	m_pkStartEntity = 0;
//	m_pkEndEntity = 0;
//	m_pkTriStrips = 0;
//	m_pkTriStrips2 = 0;
//	m_pkCircle = 0;
//	m_fRadius = 0;
//	m_pkSceneRoot = NiNew NiNode();
//	m_bRevertPoint = false;
//
//	if (bIsRectangle)
//	{
//		CreateTriStrips(kPoint1, kPoint2);
//		m_eMonAreaType = Rectangle;
//	}
//	else
//	{
//		CreateCircle(kPoint1, kPoint2);
//		m_eMonAreaType = Circle;
//	}
//}
//
//void PgMonAreaComponent::CreateTriStrips(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity)
//{
//	NiPoint3 akPoint[2];
//	if (!pkStartEntity->GetPropertyData(ms_kTranslationName, akPoint[0]) ||
//		!pkEndEntity->GetPropertyData(ms_kTranslationName, akPoint[1]) )
//	{
//		// assert
//	}
//	m_pkStartEntity = pkStartEntity;
//	m_pkEndEntity = pkEndEntity;
//
//	CreateTriStrips(akPoint[0], akPoint[1]);
//}
//
//void PgMonAreaComponent::CreateTriStrips(NiPoint3 kPoint1, NiPoint3 kPoint2)
//{
//	NiPoint3 akPoint[2];
//	akPoint[0] = kPoint1;
//	akPoint[1] = kPoint2;
//
//	if (akPoint[0] != m_akOldPoint[0] ||
//		akPoint[1] != m_akOldPoint[1])
//	{
//		m_akOldPoint[0] = akPoint[0];
//		m_akOldPoint[1] = akPoint[1];
//	}
//
//	// Tri 만들기
//	NiPoint3 akPos[4];
//	akPos[0] = m_akOldPoint[0];
//	akPos[1] = NiPoint3(m_akOldPoint[1].x, m_akOldPoint[0].y, (m_bRevertPoint ? m_akOldPoint[1].z : m_akOldPoint[0].z));
//	akPos[2] = NiPoint3(m_akOldPoint[0].x, m_akOldPoint[1].y, (m_bRevertPoint ? m_akOldPoint[0].z : m_akOldPoint[1].z));
//	akPos[3] = m_akOldPoint[1];	
//
//	unsigned short usLeftTrianglecount=0;
//	usLeftTrianglecount = 2;
//	unsigned short *pusLeftStripLengths = NiAlloc(unsigned short, 1);
//	unsigned short *pusRightStripLengths = NiAlloc(unsigned short, 1);
//	*pusLeftStripLengths = 4;
//	*pusRightStripLengths = 4;
//	unsigned short *pusLeftStripLists =
//		NiAlloc(unsigned short, *pusLeftStripLengths);
//	unsigned short *pusRightStripLists =
//		NiAlloc(unsigned short, *pusRightStripLengths);
//
//	Guid kGuid = Guid::NewGuid();
//	String *pkGuid = kGuid.ToString();
//
//	float fColorR = 0.5f;
//	float fColorG = 0.5f;
//	float fColorB = 1.0f;
//	NiMaterialProperty* pkMaterial;
//	pkMaterial = NiNew NiMaterialProperty();
//	pkMaterial->SetEmittance(NiColor(fColorR, fColorG, fColorB));
//    pkMaterial->SetAmbientColor(NiColor::BLACK);
//    pkMaterial->SetDiffuseColor(NiColor::BLACK);
//    pkMaterial->SetSpecularColor(NiColor::BLACK);
//    pkMaterial->SetShineness(0.0f);
//    pkMaterial->SetAlpha(1.0f);
//
//	{
//		NiPoint3 kCenterPt = NiPoint3(0,0,0);
//		NiPoint3 kTotalPt = NiPoint3(0,0,0);
//
//		// Calculate Center
//		for (int i=0 ; i<2 ; i++)
//		{
//			NiPoint3 kPoint = akPos[i];
//			NiPoint3 kPoint_next = akPos[i+2];
//
//			kTotalPt += (kPoint + kPoint_next);
//		}
//		kCenterPt = kTotalPt / (2 * 2.0f);
//
//		// Get Points
//		int iStripIncCount = 0;
//		int iStripIncCount2 = 0;
//		NiPoint3 *pkLeftPoint = NiNew NiPoint3[*pusLeftStripLengths];
//		NiPoint3 *pkRightPoint = NiNew NiPoint3[*pusRightStripLengths];
//		for (int i=0 ; i<2 ; i++)
//		{
//			NiPoint3 kPoint = akPos[i];
//			NiPoint3 kPoint_next = akPos[i+2];
//			{
//				pkLeftPoint[iStripIncCount] = kPoint - kCenterPt;
//				pusLeftStripLists[iStripIncCount++] = iStripIncCount;
//				pkLeftPoint[iStripIncCount] = kPoint_next - kCenterPt;
//				pusLeftStripLists[iStripIncCount++] = iStripIncCount;
//			}
//			{
//				pkRightPoint[iStripIncCount2] = kPoint_next - kCenterPt;
//				pusRightStripLists[iStripIncCount2++] = iStripIncCount2;
//				pkRightPoint[iStripIncCount2] = kPoint - kCenterPt;
//				pusRightStripLists[iStripIncCount2++] = iStripIncCount2;
//			}
//		}
//
//		if (m_pkTriStrips)
//		{
//			m_pkSceneRoot->DetachChild(m_pkTriStrips);
//			//NiDelete m_pkTriStrips;
//		}
//		if (m_pkTriStrips2)
//		{
//			m_pkSceneRoot->DetachChild(m_pkTriStrips2);
//			//NiDelete m_pkTriStrips2;
//		}
//		m_pkTriStrips = 0;
//		m_pkTriStrips2 = 0;
//		// Create
//		m_pkTriStrips = NiNew NiTriStrips(
//			*pusLeftStripLengths, pkLeftPoint, NULL, NULL,
//			NULL, 0, NiGeometryData::NBT_METHOD_NONE,
//			usLeftTrianglecount, 1, pusLeftStripLengths, pusLeftStripLists);
//		m_pkTriStrips2 = NiNew NiTriStrips(
//			*pusRightStripLengths, pkRightPoint, NULL, NULL,
//			NULL, 0, NiGeometryData::NBT_METHOD_NONE,
//			usLeftTrianglecount, 1, pusRightStripLengths, pusRightStripLists);
//
//		const char* pcPartName = MStringToCharPointer(pkGuid);
//		m_pkTriStrips->SetName(pcPartName);
//		MFreeCharPointer(pcPartName);
//		m_pkTriStrips->AttachProperty(pkMaterial);
//		m_pkTriStrips->SetTranslate(kCenterPt);
//		m_pkTriStrips->Update(0);
//		m_pkTriStrips->UpdateNodeBound();
//		m_pkTriStrips->UpdateProperties();
//
//		pcPartName = MStringToCharPointer(pkGuid);
//		m_pkTriStrips2->SetName(pcPartName);
//		MFreeCharPointer(pcPartName);
//		m_pkTriStrips2->AttachProperty(pkMaterial);
//		m_pkTriStrips2->SetTranslate(kCenterPt);
//		m_pkTriStrips2->Update(0);
//		m_pkTriStrips2->UpdateNodeBound();
//		m_pkTriStrips2->UpdateProperties();
//
//		m_pkSceneRoot->AttachChild(m_pkTriStrips);
//		m_pkSceneRoot->AttachChild(m_pkTriStrips2);
//		m_pkSceneRoot->Update(0);
//		m_pkSceneRoot->UpdateProperties();
//
//		m_kInitPoint = kCenterPt;
//		m_bInitUpdated = false;
//	}
//}
//
//void PgMonAreaComponent::CreateCircle(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity)
//{
//	NiPoint3 akPoint[2];
//	if (!pkStartEntity->GetPropertyData(ms_kTranslationName, akPoint[0]) ||
//		!pkEndEntity->GetPropertyData(ms_kTranslationName, akPoint[1]) )
//	{
//		// assert
//	}
//	m_pkStartEntity = pkStartEntity;
//	m_pkEndEntity = pkEndEntity;
//
//	CreateCircle(akPoint[0], akPoint[1]);
//}
//
//void PgMonAreaComponent::CreateCircle(NiPoint3 kPoint1, NiPoint3 kPoint2)
//{
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
//	NiPoint3 akPoint[2];
//	akPoint[0] = kPoint1;
//	akPoint[1] = kPoint2;
//	m_fRadius = abs((akPoint[0] - akPoint[1]).Length());
//
//	MScene* pmScene = MFramework::Instance->Scene;
//
//	String* strCloneName;
//	IEntityPathService* pmPathService = MGetService(IEntityPathService);
//	strCloneName = pmScene->GetUniqueEntityName(
//		String::Concat(pmPathService->GetSimpleName(pkTemplate->Name),
//		" 01"));
//	if (m_pkCircle)
//	{
//		m_pkSceneRoot->DetachChild(m_pkCircle);
//		m_pkCircle = 0;
//	}
//	
//	MEntity* pmNewCircle = pkTemplate->Clone(strCloneName, false);
//	pmNewCircle->Update(MFramework::Instance->TimeManager->CurrentTime,
//        MFramework::Instance->ExternalAssetManager);
//	pmNewCircle->MasterEntity = pkTemplate;
//	//pmNewCircle->GetNiEntityInterface()->SetPropertyData("Translation", akPoint[0]);
//	//pmNewCircle->GetNiEntityInterface()->SetPropertyData("Scale", m_fRadius, false);
//
//	NiObjectList kGeometries;
//	pmNewCircle->GetAllGeometries(kGeometries);
//
//	if(!kGeometries.IsEmpty())
//	{
//		m_pkCircle = NiDynamicCast(NiTriStrips, kGeometries.GetHead()->Clone());
//		if (m_pkCircle)
//		{
//			m_pkCircle->SetTranslate(akPoint[0]);
//			m_pkCircle->SetScale(m_fRadius / 100.0f);
//
//			// 내적 외적 지지고 볶고.
//			float PI = 3.141592653589793238462643f;
//
//			// 우선 0번 버텍스를 x축에 맞춘다,
//			{
//				for (int i=0 ; i<m_pkCircle->GetVertexCount() ; i++)
//				{
//					NiPoint3 kVer = m_pkCircle->GetVertices()[i];
//
//					int asdfff = 0;
//				}
//				NiMatrix3 kMat;
//				kMat.IDENTITY;
//				m_pkCircle->SetRotate(kMat);
//				m_pkCircle->Update(0);
//				for (int i=0 ; i<m_pkCircle->GetVertexCount() ; i++)
//				{
//					NiPoint3 kVer = m_pkCircle->GetVertices()[i];
//
//					int asdfff = 0;
//				}
//
//				NiPoint3 kCirclePt1 = m_pkCircle->GetWorldTransform() * m_pkCircle->GetVertices()[0];
//				//kCirclePt1.z = 0;
//				NiPoint3 kCirclePt2 = akPoint[1];
//				kCirclePt2.z = akPoint[0].z;
//
//				NiPoint3 kNormPt = kCirclePt1 - akPoint[0];
//				kNormPt.Unitize();
//				NiPoint3 kNormPt2 = kCirclePt2 - akPoint[0];
//				kNormPt2.Unitize();
//
//				// Dot
//				float fDotValue = kNormPt.Dot(kNormPt2);
//				if (fDotValue < -1.0f) fDotValue = -1.0f;
//				else if (fDotValue > 1.0f) fDotValue = 1.0f;
//				float fRadian = acos(fDotValue);
//				float fAngle = fRadian * 180.0f / PI;
//
//				// 외적
//				NiPoint3 ptAxisVec;
//				ptAxisVec = (kNormPt).UnitCross(kNormPt2);
//
//				// Quaternion
//				NiQuaternion kQuat;
//				kQuat = NiQuaternion(fRadian, ptAxisVec);
//				kQuat.Normalize();
//				m_pkCircle->SetRotate(kQuat);
//				m_pkCircle->Update(0);
//				for (int i=0 ; i<m_pkCircle->GetVertexCount() ; i++)
//				{
//					NiPoint3 kVer = m_pkCircle->GetVertices()[i];
//
//					int asdfff = 0;
//				}
//				// Normal;
//			}
//
//			NiPoint3 kGroundPt = m_pkCircle->GetWorldTransform() * m_pkCircle->GetVertices()[0];
//			NiPoint3 kNormPt = akPoint[1] - akPoint[0];
//			kNormPt.Unitize();
//			NiPoint3 kNormPt2 = kGroundPt - akPoint[0];
//			kNormPt2.Unitize();
//			// 내적값
//			float fDotValue = kNormPt2.Dot(kNormPt);
//			if (fDotValue < -1.0f) fDotValue = -1.0f;
//			else if (fDotValue > 1.0f) fDotValue = 1.0f;
//			float fRadian = acos(fDotValue);
//			float fAngle = fRadian * 180.0f / PI;
//
//			// 외적
//			NiPoint3 ptAxisVec;
//			ptAxisVec = (kNormPt2).UnitCross(kNormPt);
//
//			//  사원수
//			NiQuaternion kQuat;
//			NiMatrix3 kMat;
//			kQuat = NiQuaternion(fRadian, ptAxisVec);
//			kQuat.Normalize();
//			//kQuat.ToRotation(kMat);
//			m_pkCircle->SetRotate(kQuat);
//
//			// 조작 끝
//			for (int i=0 ; i<m_pkCircle->GetVertexCount() ; i++)
//			{
//				NiPoint3 kVer = m_pkCircle->GetVertices()[i];
//
//				int asdfff = 0;
//			}
//			NiMaterialProperty* pkMaterial;
//			pkMaterial = NiNew NiMaterialProperty();
//			float fColorR = 1.0f;
//			float fColorG = 0.5f;
//			float fColorB = 0.0f;
//			pkMaterial->SetEmittance(NiColor(fColorR, fColorG, fColorB));
//			pkMaterial->SetAmbientColor(NiColor::BLACK);
//			pkMaterial->SetDiffuseColor(NiColor::BLACK);
//			pkMaterial->SetSpecularColor(NiColor::BLACK);
//			pkMaterial->SetShineness(0.0f);
//			pkMaterial->SetAlpha(1.0f);
//			m_pkCircle->DetachAllProperties();
//			m_pkCircle->AttachProperty(pkMaterial);
//
//			m_pkSceneRoot->AttachChild(m_pkCircle);
//			m_pkSceneRoot->Update(0);
//			m_pkSceneRoot->UpdateProperties();
//
//			m_kInitPoint = akPoint[0];
//			m_bInitUpdated = false;
//		}
//	}
//}
//
//void PgMonAreaComponent::RevertPoint()
//{
//	if (m_eMonAreaType == Rectangle && m_pkTriStrips)
//	{
//		m_bRevertPoint = !m_bRevertPoint;
//		NiPoint3 kStartPoint = m_pkTriStrips->GetVertices()[0] + m_pkTriStrips->GetTranslate();
//		NiPoint3 kEndPoint = m_pkTriStrips->GetVertices()[3] + m_pkTriStrips->GetTranslate();
//		CreateTriStrips(kStartPoint, kEndPoint);
//	}
//}
//
//MEntity* PgMonAreaComponent::AddNewTarget(NiPoint3 kPoint, NiPoint3 kTranslate)
//{
//	return AddNewTarget(kPoint.x, kPoint.y, kPoint.z, kTranslate);
//}
//
//MEntity* PgMonAreaComponent::AddNewTarget(float fX, float fY, float fZ, NiPoint3 kTranslate)
//{
//	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
//	if (pkPalette == NULL)
//	{
//		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
//		return 0;
//	}
//	String* strEntityName = "[General]Target.monarea_target";
//	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
//	if (pkTemplate == NULL)
//	{
//		::MessageBox(0, "[General]Target.monarea_target 가 없습니다.", 0, 0);
//		return 0;
//	}
//	MScene* pmScene = MFramework::Instance->Scene;
//
//	String* strCloneName;
//	IEntityPathService* pmPathService = MGetService(IEntityPathService);
//	strCloneName = pmScene->GetUniqueEntityName(
//		String::Concat(pmPathService->GetSimpleName(pkTemplate->Name),
//		" 01"));
//	MEntity* pkNewEntity = pkTemplate->Clone(strCloneName, false);
//	pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
//		MFramework::Instance->ExternalAssetManager);
//	pkNewEntity->MasterEntity = pkTemplate;
//
//	NiPoint3 kPoint = NiPoint3(fX, fY, fZ) + kTranslate;
//	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPoint);
//
//	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
//
//	return pkNewEntity;
//}
//
//void PgMonAreaComponent::ExtractPoint(MEntity*& pkOutEntity1, MEntity*& pkOutEntity2)
//{
//	MEntity* pkStartEntity = 0;
//	MEntity* pkEndEntity = 0;
//	NiPoint3 kStartPoint;
//	NiPoint3 kEndPoint;
//	if (m_eMonAreaType == Rectangle)
//	{
//		if (!m_pkTriStrips)
//		{
//			return;
//		}
//		kStartPoint = m_pkTriStrips->GetVertices()[0];
//		kEndPoint = m_pkTriStrips->GetVertices()[3];
//		pkStartEntity = AddNewTarget(kStartPoint, m_pkTriStrips->GetTranslate());
//		pkEndEntity = AddNewTarget(kEndPoint, m_pkTriStrips->GetTranslate());
//	}
//	else if (m_eMonAreaType == Circle)
//	{
//		if (!m_pkCircle)
//		{
//			return;
//		}
//
//		NiPoint3 kPoint;
//		kPoint = m_pkCircle->GetTranslate();
//		kStartPoint = kPoint;
//		kEndPoint = m_pkCircle->GetWorldTransform() * m_pkCircle->GetVertices()[0];
//		pkStartEntity = AddNewTarget(kStartPoint, NiPoint3(0,0,0));
//		pkEndEntity = AddNewTarget(kEndPoint, NiPoint3(0,0,0));
//	}
//
//	if (pkStartEntity && pkEndEntity)
//	{
//		m_pkStartEntity = pkStartEntity->PropertyInterface;
//		m_pkEndEntity = pkEndEntity->PropertyInterface;
//		pkOutEntity1 = pkStartEntity;
//		pkOutEntity2 = pkEndEntity;
//	}
//}
//
//NiPoint3 PgMonAreaComponent::GetNormalVec()
//{
//	if (m_eMonAreaType == Rectangle)
//	{
//		NiPoint3 kVec1 = m_pkTriStrips->GetVertices()[3] - m_pkTriStrips->GetVertices()[0];
//		NiPoint3 kVec2 = m_pkTriStrips->GetVertices()[1] - m_pkTriStrips->GetVertices()[0];
//
//		NiPoint3 kNor1 = kVec1.UnitCross(kVec2);
//		NiPoint3 kNor2 = kVec2.UnitCross(kVec1);
//		if (kNor1.z > 0)
//		{
//			return kNor1;
//		}
//		else
//		{
//			return kNor2;
//		}
//	}
//	else if (m_eMonAreaType == Circle)
//	{
//		NiPoint3 kVec1 = (m_pkCircle->GetWorldTransform() * m_pkCircle->GetVertices()[0]) - m_pkCircle->GetTranslate();
//		NiPoint3 kVec2 = (m_pkCircle->GetWorldTransform() * m_pkCircle->GetVertices()[2]) - m_pkCircle->GetTranslate();
//
//		NiPoint3 kNor1 = kVec1.UnitCross(kVec2);
//		NiPoint3 kNor2 = kVec2.UnitCross(kVec1);
//		if (kNor1.z > 0)
//		{
//			return kNor1;
//		}
//		else
//		{
//			return kNor2;
//		}
//	}
//
//	return NiPoint3(0,0,0);
//}
//
//void PgMonAreaComponent::Update(NiEntityPropertyInterface* pkParentEntity,
//    float fTime, NiEntityErrorInterface* pkErrors,
//	NiExternalAssetManager* pkAssetManager)
//{
//	bool bUpdatedTransforms = false;
//
//	if (!m_bInitUpdated)
//	{
//		m_bInitUpdated = true;
//		pkParentEntity->SetPropertyData(ms_kTranslationName, m_kInitPoint);
//	}
//
//	m_spSceneRoot = m_pkSceneRoot;
//
//	// case Rectangle
//	NiAVObject* pkTriStrips = NiDynamicCast(NiAVObject, m_pkTriStrips);
//    NiAVObject* pkTriStrips2 = NiDynamicCast(NiAVObject, m_pkTriStrips2);
//	if (pkTriStrips && pkTriStrips2)
//	{
//		NiBool bDependentPropertiesFound = true;
//
//		// Find dependent properties.
//		NiPoint3 kTranslation;
//		if (!pkParentEntity->GetPropertyData(ms_kTranslationName,
//			kTranslation))
//		{
//			bDependentPropertiesFound = false;
//			pkErrors->ReportError(ERR_TRANSLATION_NOT_FOUND, NULL,
//				pkParentEntity->GetName(), ms_kTranslationName);
//		}
//
//		//// Use dependent properties to update transform of scene root.
//		//bool bUpdatedTransforms = false;
//		if (bDependentPropertiesFound)
//		{
//			if (pkTriStrips->GetTranslate() != kTranslation &&
//				pkTriStrips2->GetTranslate() != kTranslation)
//			{
//				pkTriStrips->SetTranslate(kTranslation);
//				pkTriStrips2->SetTranslate(kTranslation);
//				bUpdatedTransforms = true;
//			}
//		}
//
//		// Update scene root with the provided time.
//		if (bUpdatedTransforms)
//		{
//			pkTriStrips->Update(fTime);
//			pkTriStrips2->Update(fTime);
//		}
//		else if (GetShouldUpdateSceneRoot())
//		{
//			pkTriStrips->UpdateSelected(fTime);
//			pkTriStrips2->UpdateSelected(fTime);
//		}
//	}
//
//	// Case Circle
//	NiAVObject* pkCircle = NiDynamicCast(NiAVObject, m_pkCircle);
//	if (pkCircle)
//	{
//		NiBool bDependentPropertiesFound = true;
//
//		// Find dependent properties.
//		NiPoint3 kTranslation;
//		if (!pkParentEntity->GetPropertyData(ms_kTranslationName,
//			kTranslation))
//		{
//			bDependentPropertiesFound = false;
//			pkErrors->ReportError(ERR_TRANSLATION_NOT_FOUND, NULL,
//				pkParentEntity->GetName(), ms_kTranslationName);
//		}
//
//		//// Use dependent properties to update transform of scene root.
//		//bool bUpdatedTransforms = false;
//		if (bDependentPropertiesFound)
//		{
//			if (pkCircle->GetTranslate() != kTranslation)
//			{
//				pkCircle->SetTranslate(kTranslation);
//				bUpdatedTransforms = true;
//			}
//		}
//
//		// Update scene root with the provided time.
//		if (bUpdatedTransforms)
//		{
//			pkCircle->Update(fTime);
//		}
//		else if (GetShouldUpdateSceneRoot())
//		{
//			pkCircle->UpdateSelected(fTime);
//		}
//	}
//}
//
//void PgMonAreaComponent::BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext, NiEntityErrorInterface* pkErrors)
//{
//	NiPoint3 akPoint[2];
//	if (m_pkStartEntity && m_pkEndEntity)
//	{
//		if (m_pkStartEntity->GetPropertyData(ms_kTranslationName, akPoint[0]) &&
//			m_pkEndEntity->GetPropertyData(ms_kTranslationName, akPoint[1]) )
//		{
//			if (akPoint[0] != m_akOldPoint[0] ||
//				akPoint[1] != m_akOldPoint[1])
//			{
//				m_akOldPoint[0] = akPoint[0];
//				m_akOldPoint[1] = akPoint[1];
//
//				if (m_eMonAreaType == Rectangle)
//				{
//					CreateTriStrips(m_pkStartEntity, m_pkEndEntity);
//				}
//				else if (m_eMonAreaType == Circle)
//				{
//					CreateCircle(m_pkStartEntity, m_pkEndEntity);
//				}
//			}
//		}
//	}
//
//	NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
//        ->GetVisibleSet();
//    assert(pkVisibleSet);
//	if (m_eMonAreaType == Rectangle)
//	{
//		pkVisibleSet->Add(*m_pkTriStrips);
//		pkVisibleSet->Add(*m_pkTriStrips2);
//	}
//	else if (m_eMonAreaType == Circle)
//	{
//		pkVisibleSet->Add(*m_pkCircle);
//	}
//}
//
//NiFixedString PgMonAreaComponent::GetClassName() const
//{
//	return "PgMonAreaComponent";
//}
//
//NiFixedString PgMonAreaComponent::GetName() const
//{
//	return "PgMonArea";
//}
