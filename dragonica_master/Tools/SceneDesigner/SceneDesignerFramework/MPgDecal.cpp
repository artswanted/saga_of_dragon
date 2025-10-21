//* "Portions Copyright (C) Eric Lengyel, 2001"
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "IEntityPathService.h"
#include "MPgDecal.h"
#include "MUtility.h"

using namespace System::IO;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgDecal::MPgDecal(void) :
	m_spShapeGeo(0)
{

}
MPgDecal::~MPgDecal(void)
{
	m_spShapeGeo = 0;
}

MPgDecal::MPgDecal(const NiPoint3& _center, const NiPoint3& normal, const NiPoint3& tangent, float width, float height, float depth)
{
	decalCenter = _center;
	decalNormal = normal;

	NiPoint3 center = _center;
	//center.Unitize();
	NiPoint3 binormal = CrossProduct(normal, tangent);

	// Calculate boundary planes
	float d = DotProduct(center, tangent);
	leftPlane = vector4(tangent.x, tangent.y, tangent.z, width * 0.5F - d);
	rightPlane = vector4(-tangent.x, -tangent.y, -tangent.z, width * 0.5F + d);

	d = DotProduct(center, binormal);
	bottomPlane = vector4(binormal.x, binormal.y, binormal.z, height * 0.5F - d);
	topPlane = vector4(-binormal.x, -binormal.y, -binormal.z, height * 0.5F + d);

	d = DotProduct(center, normal);
	frontPlane = vector4(-normal.x, -normal.y, -normal.z, depth + d);
	backPlane = vector4(normal.x, normal.y, normal.z, depth - d);
	//float d = DotProduct(center, tangent);
	//leftPlane = vector4(tangent.x, tangent.y, tangent.z, d + (width * 0.5F));
	//rightPlane = vector4(-tangent.x, -tangent.y, -tangent.z, d - (width * 0.5F));

	//d = DotProduct(center, binormal);
	//bottomPlane = vector4(binormal.x, binormal.y, binormal.z, d + (height * 0.5F));
	//topPlane = vector4(-binormal.x, -binormal.y, -binormal.z, d - (height * 0.5F));

	//d = DotProduct(center, normal);
	//frontPlane = vector4(-normal.x, -normal.y, -normal.z, d - depth);
	//backPlane = vector4(normal.x, normal.y, normal.z, d + depth);
	// Begin with empty mesh
	decalVertexCount = 0;
	decalTriangleCount = 0;

	// Add this point, determine which surfaces may be affected by this decal
	// and call ClipMesh() for each one.

	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for(int nn=0; nn<pkEntities->Count; ++nn)
	{
		MEntity *pkEntity = pkEntities[nn];
		if(pkEntity)
		{
		  	String* strName = pkEntity->get_Name();
			if(strName->Contains("decal"))
				continue;

			NiObjectList kGeometries;
			for(unsigned int gg=0; gg<pkEntity->GetSceneRootPointerCount(); ++gg)
			{
				NiNode* pkNode = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(gg));
				if(pkNode)
				{
					pkEntity->GetAllGeometries(pkNode, kGeometries);
				}
			}
			while(!kGeometries.IsEmpty())
			{
				NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, kGeometries.GetTail());
				if(pkGeometry)
				{
					ClipMesh(pkGeometry);
				}

				kGeometries.RemoveTail();
			}
		}
	}

	// Assign texture mapping coordinates
	float one_over_w = 1.0F / width;
	float one_over_h = 1.0F / height;
	for (long a = 0; a < decalVertexCount; a++)
	{
		NiPoint3 v = vertexArray[a] - decalCenter;
		float s = DotProduct(v, tangent) * one_over_w + 0.5F;
		float t = DotProduct(v, binormal) * one_over_h + 0.5F;
		texcoordArray[a] = NiPoint2(s, t);
	}

	//int	iTotalIndex = decalTriangleCount*3;
	//unsigned	short	*pkIndex = NiAlloc(unsigned short,iTotalIndex);
	//for(int j=0;j<iTotalIndex;j++)
	//{
	//	*(pkIndex+j) = j;
	//}

	NiTriShapeData* pkModelData = NiNew NiTriShapeData(decalVertexCount,vertexArray,NULL,NULL,texcoordArray,1,NiGeometryData::NBT_METHOD_NONE,decalTriangleCount,(unsigned short*)&(triangleArray->index[0]));
	m_spShapeGeo = NiNew NiTriShape(pkModelData);

	NiMaterialProperty	*pkMat =NiNew NiMaterialProperty();
	pkMat->SetDiffuseColor(NiColor(0,0,0));
	pkMat->SetAmbientColor(NiColor::BLACK);
	pkMat->SetEmittance(NiColor::BLACK);
	pkMat->SetAlpha(1.0f);

	m_spShapeGeo->AttachProperty(pkMat);
	m_spShapeGeo->UpdateProperties();
	m_spShapeGeo->Update(0);
}

bool MPgDecal::AddPolygon(long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal)
{
	long count = decalVertexCount;
	if (count + vertexCount >= maxDecalVertices) return (false);

	// Add polygon as a triangle fan
	Triangle *triangle = triangleArray + decalTriangleCount;
	decalTriangleCount += vertexCount - 2;
	for (long a = 2; a < vertexCount; a++)
	{
		triangle->index[0] = (unsigned short) count;
		triangle->index[1] = (unsigned short) (count + a - 1);
		triangle->index[2] = (unsigned short) (count + a);
		triangle++;
	}

	// Assign vertex colors
	float f = 1.0F / (1.0F - decalEpsilon);
	for (long b = 0; b < vertexCount; b++)
	{
		vertexArray[count] = vertex[b];
		const NiPoint3& n = normal[b];
		float alpha = (DotProduct(decalNormal, n) / n.Length() - decalEpsilon) * f;
		colorArray[count] = ColorRGBA(1.0F, 1.0F, 1.0F, (alpha > 0.0F) ? alpha : 0.0F);
		count++;
	}

	decalVertexCount = count;
	return (true);
}

void MPgDecal::ClipMesh(long triangleCount, const Triangle *triangle, const NiPoint3 *vertex, const NiPoint3 *normal)
{
	NiPoint3		newVertex[9];
	NiPoint3		newNormal[9];

	// Clip one triangle at a time
	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const NiPoint3& v1 = vertex[i1];
		const NiPoint3& v2 = vertex[i2];
		const NiPoint3& v3 = vertex[i3];

		NiPoint3 cross = CrossProduct(v2 - v1, v3 - v1);
		if (DotProduct(decalNormal, cross) > decalEpsilon * cross.Length())
		{
			newVertex[0] = v1;
			newVertex[1] = v2;
			newVertex[2] = v3;

			newNormal[0] = normal[i1];
			newNormal[1] = normal[i2];
			newNormal[2] = normal[i3];

			long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
			if ((count != 0) && (!AddPolygon(count, newVertex, newNormal))) break;
		}

		triangle++;
	}
}
void MPgDecal::ClipMesh(NiGeometry* pkGeometry)
{
	NiPoint3		newVertex[9];
	NiPoint3		newNormal[9];

	if(!pkGeometry)
		return;

	NiTriBasedGeom* pkTri = NiDynamicCast(NiTriBasedGeom, pkGeometry);
	if(pkTri)
	{
		long triangleCount = pkTri->GetTriangleCount();
		const NiPoint3* vertex = pkTri->GetVertices();
		const NiPoint3* normal = pkTri->GetNormals();
		if(!(vertex && normal))
			return;
		// Clip one triangle at a time
		for (long a = 0; a < triangleCount; a++)
		{
			unsigned short i1;
			unsigned short i2;
			unsigned short i3;

			pkTri->GetTriangleIndices(a, i1, i2, i3);

			const NiPoint3& v1 = vertex[i1];
			const NiPoint3& v2 = vertex[i2];
			const NiPoint3& v3 = vertex[i3];

			NiPoint3 cross = CrossProduct(v2 - v1, v3 - v1);
			if (DotProduct(decalNormal, cross) > decalEpsilon * cross.Length())
			{
				newVertex[0] = v1;
				newVertex[1] = v2;
				newVertex[2] = v3;

				newNormal[0] = normal[i1];
				newNormal[1] = normal[i2];
				newNormal[2] = normal[i3];

				long count = ClipPolygon(3, newVertex, newNormal, newVertex, newNormal);
				if ((count != 0) && (!AddPolygon(count, newVertex, newNormal))) break;
			}
		}
	}
}

long MPgDecal::ClipPolygon(long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal) const
{
	return 3;
	NiPoint3		tempVertex[9];
	NiPoint3		tempNormal[9];

	// Clip against all six planes
	long count = ClipPolygonAgainstPlane(leftPlane, vertexCount, vertex, normal, tempVertex, tempNormal);
	if (count != 0)
	{
		count = ClipPolygonAgainstPlane(rightPlane, count, tempVertex, tempNormal, newVertex, newNormal);
		if (count != 0)
		{
			count = ClipPolygonAgainstPlane(bottomPlane, count, newVertex, newNormal, tempVertex, tempNormal);
			if (count != 0)
			{
				count = ClipPolygonAgainstPlane(topPlane, count, tempVertex, tempNormal, newVertex, newNormal);
				if (count != 0)
				{
					count = ClipPolygonAgainstPlane(backPlane, count, newVertex, newNormal, tempVertex, tempNormal);
					if (count != 0)
					{
						count = ClipPolygonAgainstPlane(frontPlane, count, tempVertex, tempNormal, newVertex, newNormal);
					}
				}
			}
		}
	}

	return (count);
}

long MPgDecal::ClipPolygonAgainstPlane(const vector4& plane, long vertexCount, const NiPoint3 *vertex, const NiPoint3 *normal, NiPoint3 *newVertex, NiPoint3 *newNormal)
{
	bool	negative[10];

	// Classify vertices
	long negativeCount = 0;
	for (long a = 0; a < vertexCount; a++)
	{
		bool neg = (DotProduct(plane, vertex[a]) < 0.0F);
		negative[a] = neg;
		negativeCount += neg;
	}

	// Discard this polygon if it's completely culled
	if (negativeCount == vertexCount) return (0);

	long count = 0;
	for (long b = 0; b < vertexCount; b++)
	{
		// c is the index of the previous vertex
		long c = (b != 0) ? b - 1 : vertexCount - 1;

		if (negative[b])
		{
			if (!negative[c])
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				const NiPoint3& v1 = vertex[c];
				const NiPoint3& v2 = vertex[b];
				float t = DotProduct(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				const NiPoint3& n1 = normal[c];
				const NiPoint3& n2 = normal[b];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}
		}
		else
		{
			if (negative[c])
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				const NiPoint3& v1 = vertex[b];
				const NiPoint3& v2 = vertex[c];
				float t = DotProduct(plane, v1) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				newVertex[count] = v1 * (1.0F - t) + v2 * t;

				const NiPoint3& n1 = normal[b];
				const NiPoint3& n2 = normal[c];
				newNormal[count] = n1 * (1.0F - t) + n2 * t;
				count++;
			}

			// Include current vertex
			newVertex[count] = vertex[b];
			newNormal[count] = normal[b];
			count++;
		}
	}

	// Return number of vertices in clipped polygon
	return (count);
}

NiTriShape* MPgDecal::GetShapeGeo()
{
	return m_spShapeGeo;
}

//////////////////////////////////////////////////////////////////////////
MPgDecalManager::MPgDecalManager(void) :
m_spDecalRoot(0)
{
	m_pmNameList = new ArrayList();
	//m_spDecalRoot = NiNew NiNode;
	//if(m_spDecalRoot)
	//{
	//	m_spDecalRoot->SetName("decal_data");
	//	m_spDecalRoot->SetAppCulled(false);
	//}
}
MPgDecalManager::~MPgDecalManager(void)
{
	if(m_spDecalRoot)
		NiDelete m_spDecalRoot;
	m_spDecalRoot = NULL;

	m_pmNameList->Clear();
}

ISelectionService* MPgDecalManager::get_SelectionService()
{
	if (ms_pmSelectionService == NULL)
	{
		ms_pmSelectionService = MGetService(ISelectionService);
		MAssert(ms_pmSelectionService != NULL, "Selection service not "
			"found!");
	}
	return ms_pmSelectionService;
}

NiNode* MPgDecalManager::GetDecalRoot()
{
	return m_spDecalRoot;
}

void MPgDecalManager::LoadDecal()
{
	// Nif 파일을 로드한다.
	String* strPath = MFramework::Instance->GetScenePathName();
	FileInfo* pmFileInfo = new FileInfo(strPath);
	String* strFileName = pmFileInfo->Name;
	strFileName = strFileName->Substring(0, strFileName->LastIndexOf("."));
	String* strDecal = "_decal.nif";
	strFileName =  String::Concat(strFileName, strDecal);
	String* strFilePathName = pmFileInfo->DirectoryName;
	strFilePathName = String::Concat(strFilePathName, "\\", strFileName);
	const char* pcFilePathName = MStringToCharPointer(strFilePathName);
	NiStream kStream;
	if(!kStream.Load(pcFilePathName))
	{
		// 파일을 만든다.
		m_spDecalRoot = NiNew NiNode;
	}
	else
	{
		m_spDecalRoot = (NiNode*)kStream.GetObjectAt(0);
	}
	MFreeCharPointer(pcFilePathName);
	if(!m_spDecalRoot)
		return;

	if(m_spDecalRoot->GetChildCount() == 0)
		return;

	// 차일드 카운트만큼 돌리며 엔터티에서 데칼을 찾아서 연결시켜준다.
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];
		if (!pkEntity)
		{
			continue;
		}
		//NiEntityInterface* pkEntityInterface = pkEntity->GetNiEntityInterface();
		//if (!pkEntityInterface)
		//{
		//	continue;
		//}

		String* strName = pkEntity->get_Name();
		if(strName->Contains("decal_data"))
		{
			const char* pcName = MStringToCharPointer(strName);
			NiNode* pkNode = NiDynamicCast(NiNode, m_spDecalRoot->GetObjectByName(pcName));
			MFreeCharPointer(pcName);
			if(pkNode)
			{
				 NiNode* pkObjectNode = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(0));
				 if(pkObjectNode)
				 {
					 pkObjectNode->AttachChild(pkNode);
					 pkObjectNode->UpdateProperties();
					 pkObjectNode->Update(0.0f);

					 m_pmNameList->Add(strName);
				 }
			}
		}
	}



	////NiStream kStream;
	////if (!kStream.Load("D:/Dragon_Pilot/SFreedom_Dev/Data/3_World/00_Object/Box/Art_Drum1001_01.nif"))
	////{
	////	return;
	////}
	////NiNode* pkTest = (NiNode*) kStream.GetObjectAt(0);
	////if(pkTest)
	////{
	////	m_spDecalRoot->AttachChild(pkTest);
	////}
	//


	//MEntity* pkNewEntity = pkTemplate->Clone("decal_data", false);
	//if(!pkNewEntity)
	//	return;

	//pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
	//pkNewEntity->MasterEntity = pkTemplate;
	//MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

	//NiBool bBool = 0;

	//NiObject* pkSceneRoot = NULL;
	//bBool = pkNewEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
	//if(!bBool)
	//{
	//	delete pkNewEntity;
	//	return;
	//}


	//m_spDecalRoot = NiDynamicCast(NiNode, pkSceneRoot);
	//m_spDecalRoot->SetName("decal_data");

	////NiStream kStream;
	////if (!kStream.Load("D:/Dragon_Pilot/SFreedom_Dev/Data/3_World/00_Object/Box/Art_Drum1001_01.nif"))
	////{
	////	return;
	////}
	////NiNode* pkTest = (NiNode*) kStream.GetObjectAt(0);
	////if(pkTest)
	////{
	////	m_spDecalRoot->AttachChild(pkTest);
	////}

	//m_spDecalRoot->UpdateProperties();
	//m_spDecalRoot->Update(0.0f);


}
//void MPgDecalManager::LoadDecal()
//{
//	//NiFixedString kDefaultName = "decal_data";
//
//	//NiUniqueID kTemplateID;
//	//MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
//	//NiEntityInterface* pkDecalManager = NiNew NiGeneralEntity(
//	//	kDefaultName, kTemplateID, 2);
//	//pkDecalManager->AddComponent(NiNew NiTransformationComponent());
//
//	//NiMatrix3 kRotMat;
//	//kRotMat.MakeIdentity();
//	//kRotMat.MakeXRotation(0);
//	//kRotMat.MakeYRotation(0);
//	//kRotMat.MakeZRotation(0);
//	//pkDecalManager->SetPropertyData("Rotation", kRotMat);
//
//
//	//MEntity* pkEntity = new MEntity(pkDecalManager);
//	MEntity* pkEntity = MEntity::CreateGeneralEntity("decal_data");
//
//	m_spDecalRoot->SetName("decal_data");
//
//	NiSceneGraphComponent* pkSceneGraphComponent = NiNew NiSceneGraphComponent();
//	pkSceneGraphComponent->SetNifFilePath("D:/Dragon_Pilot/SFreedom_Dev/Data/3_World/00_Object/Box/Art_Drum1001_01.nif");
//	//pkSceneGraphComponent->SetNifFilePath(pcFileName);
//	NiBool bBool = pkEntity->GetNiEntityInterface()->AddComponent(pkSceneGraphComponent);
//	pkEntity->GetNiEntityInterface()->AddComponent(NiNew NiTransformationComponent());
//
//
//	pkEntity->GetNiEntityInterface()->SetPropertyData("Scene Root Pointer", (NiObject*)m_spDecalRoot, 0);
//	pkEntity->GetNiEntityInterface()->SetPropertyData("NIF File Path", "D:/Dragon_Pilot/SFreedom_Dev/Data/3_World/00_Object/Box/Art_Drum1001_01.nif");
//
//	NiObject* pkSceneRoot = NULL;
//	bBool = pkEntity->GetNiEntityInterface()->GetPropertyData("Scene Root Pointer", pkSceneRoot, 0);
//	if(bBool)
//		pkSceneRoot = (NiObject*)m_spDecalRoot;
//
//	NiStream kStream;
//	if (!kStream.Load("D:/Dragon_Pilot/SFreedom_Dev/Data/3_World/00_Object/Box/Art_Drum1001_01.nif"))
//	{
//		return;
//	}
//	NiNode* pkTest = (NiNode*) kStream.GetObjectAt(0);
//	if(pkTest)
//	{
//		m_spDecalRoot->AttachChild(pkTest);
//	}
//
//	m_spDecalRoot->UpdateProperties();
//	m_spDecalRoot->Update(0.0f);
//
//	//MEntity* pmNewProxy;
//	//if (pmNewProxy != NULL)
//	//{
//	//	MFramework::Instance->EntityFactory->Remove(pmNewProxy->GetNiEntityInterface());
//	//	pmNewProxy = NULL;
//	//}
//	//pmNewProxy = MFramework::Instance->ProxyManager->CreateProxy(pkEntity);
//	MFramework::Instance->Scene->AddEntity(pkEntity, false);
//}

//void MPgDecalManager::MakeDecal(NiPoint3 pkCenter, NiPoint3 pkTangent, float fWidth, float fHeight, float fDepth)
//{
//	MPgDecal* pDecal = new MPgDecal(pkCenter, NiPoint3(0,0,1), pkTangent, fWidth, fHeight, fDepth);
//	if(pDecal)
//	{
//		NiTriShape* pkTriShape = pDecal->GetShapeGeo();
//		if(pkTriShape)
//		{
//			NiNode* pkNode = NiNew NiNode;
//			if(!pkNode)
//				return;
//			int nChildCount = m_spDecalRoot->GetChildCount();
//			char szTmp[20]={0,};
//			NiSprintf(szTmp, 20, "decal_%d", nChildCount+1);
//			NiAVObject* pkObject = m_spDecalRoot->GetObjectByName(szTmp);
//
//			if(pkObject)
//			{
//				for(int nn=1; nn<nChildCount+1; ++nn)
//				{
//					NiSprintf(szTmp, 20, "decal_%d", nn);
//					NiAVObject* pkObject = m_spDecalRoot->GetObjectByName(szTmp);
//					if(!pkObject)
//					{
//						break;
//					}
//				}
//			}
//
//			pkNode->SetName(szTmp);
//			pkNode->AttachChild(pkTriShape);
//			pkNode->SetTranslate(pkCenter);
//			pkNode->UpdateProperties();
//			pkNode->Update(0.0f);
//			m_spDecalRoot->AttachChild(pkNode);
//
//			m_spDecalRoot->UpdateProperties();
//			m_spDecalRoot->Update(0.0f);
//		}
//		delete pDecal;
//	}
//}

void MPgDecalManager::MakeDecal(NiPoint3 pkCenter, NiPoint3 pkTangent, float fWidth, float fHeight, float fDepth)
{
	MPgDecal* pDecal = new MPgDecal(pkCenter, NiPoint3(0,0,1), pkTangent, fWidth, fHeight, fDepth);
	if(pDecal)
	{
		NiTriShape* pkTriShape = pDecal->GetShapeGeo();
		if(pkTriShape)
		{
			NiNode* pkNode = NiNew NiNode;
			if(!pkNode)
				return;
			// 새로운 번호 결정
			int nNewNum = 0;
			for(int nn=0; nn<m_pmNameList->Count; ++nn)
			{
				String* strName = dynamic_cast<String*>(m_pmNameList->get_Item(nn));
				int nLastIndex = strName->LastIndexOf("_");
				int nLength = strName->Length;
				String* strNum = strName->Substring(nLastIndex+1);
				const char* pcNum = MStringToCharPointer(strNum);
				int nNum = atoi(pcNum);
				if(nNum > nNewNum)
					nNewNum	= nNum;
				MFreeCharPointer(pcNum);
			}
			++nNewNum;

			String* strNewName = "decal_data_";
			strNewName = String::Concat(strNewName, nNewNum.ToString());
			char szTmp[20]={0,};
			NiSprintf(szTmp, 20, "decal_data_%d", nNewNum);


			// 팔레트를 로드한다.
			MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
			if (pkPalette == NULL)
			{
				::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
				return;
			}
			String* strEntityName = "[General]Object.decal_base";
			MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
			if (pkTemplate == NULL)
			{
				::MessageBox(0, "[General]Object.decal_base 가 없습니다.", 0, 0);
				return;
			}

			// 엔터티 생성
			MEntity* pkNewEntity = pkTemplate->Clone(strNewName, false);
			if(!pkNewEntity)
				return;

			pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", pkCenter);
			pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
			pkNewEntity->MasterEntity = pkTemplate;
			//MEntityFactory::Instance->Get(pkNewEntity->GetNiEntityInterface());

			// 엔터티에 생성된 데칼 연결
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
				return;

			pkNode->SetName(szTmp);
			pkNode->AttachChild(pkTriShape);
			pkSceneNode->AttachChild(pkNode);
			pkSceneNode->UpdateProperties();
			pkSceneNode->Update(0.0f);

			MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
			m_pmNameList->Add(strNewName);
		}
		delete pDecal;
	}
}

void MPgDecalManager::MakeDecal()
{
	if (SelectionService->GetSelectedEntities()->Count == 1)
	{
		MEntity* pkEntity = SelectionService->GetSelectedEntities()[0];
		if(pkEntity)
		{
			NiPoint3 kEntityPos;
			pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
			MakeDecal(kEntityPos, NiPoint3(0,1,0), m_fWidth, m_fHeight, m_fDepth);
		}
	}
}
void MPgDecalManager::SetWidth(float fWidth)
{
	m_fWidth = fWidth;
}
void MPgDecalManager::SetHeight(float fHeight)
{
	m_fHeight = fHeight;
}
void MPgDecalManager::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
}
void MPgDecalManager::SaveDecal()
{
	typedef std::map<NiNode*, NiNode*> Restore;
	Restore smRestore;

	NiNode* pkRoot = NiNew NiNode;
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];
		if (!pkEntity)
			continue;

		String* strName = pkEntity->get_Name();
		if(!strName->Contains("decal_data"))
			continue;

		NiNode* pkScene = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(0));
		if(!pkScene)
			continue;

		NiNode* pkChild = NiDynamicCast(NiNode, pkScene->GetAt(0));
		if(!pkChild)
			continue;

		smRestore.insert(std::make_pair(pkScene, pkChild));

		pkRoot->AttachChild(pkScene->DetachChild(pkChild));
		//pkRoot->AttachChild(pkChild);
		pkRoot->UpdateProperties();
		pkRoot->Update(0.0f);
	}

	// Nif 파일로 저정한다.
	String* strPath = MFramework::Instance->GetScenePathName();
	FileInfo* pmFileInfo = new FileInfo(strPath);
	String* strFileName = pmFileInfo->Name;
	strFileName = strFileName->Substring(0, strFileName->LastIndexOf("."));
	String* strDecal = "_decal.nif";
	strFileName =  String::Concat(strFileName, strDecal);
	String* strFilePathName = pmFileInfo->DirectoryName;
	strFilePathName = String::Concat(strFilePathName, "\\", strFileName);
	const char* pcFilePathName = MStringToCharPointer(strFilePathName);
	NiStream kStream;
	kStream.InsertObject(pkRoot);
	kStream.Save(pcFilePathName);
	MFreeCharPointer(pcFilePathName);
	
	Restore::iterator itr = smRestore.begin();
	for(; itr != smRestore.end(); ++itr)
	{
		NiNode* pkChild = itr->second;
		NiNode* pkNode = itr->first;
		if(pkChild && pkNode)
		{
			pkNode->AttachChild(pkRoot->DetachChild(pkChild));
		}
	}
}