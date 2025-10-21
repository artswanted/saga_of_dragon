// SceneDesigner_PhysXConvert_Console.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "conio.h"
#include "PhysXSnapshotExtractor.h"

void MakePhysXData(std::wstring wstrFilePath, std::wstring wstrOutPath, std::wstring wstrFileName, int iGroup);

int _tmain(int argc, _TCHAR* argv[])
{
	// Parameters
	// 0: Excute File Path
	// 1: Target File
	// 2: Group
	if (argc != 3)
	{
		assert(0 && "arg isn't count 2");
		printf("arg isn't count 2");
		_getch();
		return 0;
	}
	int iGroup = _wtoi(argv[2]);

	printf("%s\n%s\n%d", argv[0], argv[1], iGroup);
	// 초기화
    NiInit();
	NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
	if (!pkManager->Initialize())
	{
		assert(0 && "PhysX Init Faild");
		printf("PhysX Init Faild");
		_getch();
		return false;
	}

	std::wstring wstrFolder;
	std::wstring wstrFile;
	BM::DivFolderAndFileName(argv[1], wstrFolder, wstrFile);

	MakePhysXData(argv[1], wstrFolder, wstrFile, iGroup);

	return 0;
}

void MakePhysXData(std::wstring wstrFilePath, std::wstring wstrOutPath, std::wstring wstrFileName, int iGroup)
{
	char szFilePath[256];
	size_t CharactersConverted = 0;
	wcstombs_s(&CharactersConverted, szFilePath, 
		wstrFilePath.size()+1, wstrFilePath.c_str(), 
			   _TRUNCATE);

	NiStream kStream;
	if (!kStream.Load(szFilePath))
	{
		std::wstring wstrText(L"Error : ");
		wstrText += wstrFileName;
		wstrText += _T(" was loaded fail.");
		printf("%s \n", wstrText);
		return;
	}

	// exist physx data
	unsigned int cnt = kStream.GetObjectCount();
	for (unsigned int ui = 0 ; ui < cnt ; ui++)
	{
		if(NiIsKindOf(NiPhysXScene, kStream.GetObjectAt(ui)))
		{
			// exist.
			std::wstring wstrText(L"Error : PhysX Data was exist in ");
			wstrText += wstrFileName;
			printf("%s \n", wstrText);
			return;
		}
	}

	NiPhysXManager *pkPhysXManager;
	pkPhysXManager = NiPhysXManager::GetPhysXManager();
	NiNode *pkRoot = NiDynamicCast(NiNode, kStream.GetObjectAt(0));

	// Create NxScene
	NxSceneDesc kSceneDesc;
	kSceneDesc.gravity = NxVec3(0.0f, 0.0f, -9.8f * 200.0f);
	kSceneDesc.simType = NX_SIMULATION_SW;
	NxScene* pkNxScene = pkPhysXManager->m_pkPhysXSDK->createScene(kSceneDesc);
	assert(pkNxScene);

	// Create NiPhysXScene
	NiPhysXScene *pkPhysXScene = NiNew NiPhysXScene();
	pkPhysXScene->SetPhysXScene(pkNxScene);	

	// Create Snapshot
	NxMat34 kMat;
	kMat.zero();
	kMat.M.id();
	CPhysXSnapshotExtractor kExtractor;
	NiPhysXSceneDesc *pkSceneDesc = kExtractor.ExtractSnapshot(pkPhysXScene, kMat);

	for (unsigned int i=0 ; i<pkRoot->GetChildCount() ; i++)
	{
		// Node가 아니면 다시 돌자
		if (!NiIsKindOf(NiNode, pkRoot->GetAt(i)))
		{
			continue;
		}

		NiNode *pkChildNode = NiDynamicCast(NiNode, pkRoot->GetAt(i));
		int iChildCount = pkChildNode->GetChildCount();

		for (int j=0 ; j<iChildCount ; j++)
		{
			// Path에 피직스가 들어가야 하지 않는다면 다시 돌자
			if (pkChildNode->GetName().Contains("paths_nonephysx_"))
			{
				continue;
			}

			NxVec3* pkPhysXMeshVerts = 0;
			int iVertexCount = 0;
			
			if (NiIsKindOf(NiTriStrips, pkChildNode->GetAt(j)))
			{
				NiTriStrips*pkTri = NiDynamicCast(NiTriStrips, pkChildNode->GetAt(j));

				iVertexCount = pkTri->GetVertexCount();
				NxVec3* pkPhysXMeshVerts = new NxVec3[iVertexCount];
				for (int k=0 ; k<iVertexCount ; k++)
				{
					NiPoint3 kPt = pkTri->GetVertices()[k];
					NiPhysXTypes::NiPoint3ToNxVec3(pkTri->GetVertices()[k],
						pkPhysXMeshVerts[k]);
				}

				// Create Data
				// The PhysX mesh data
				unsigned int uiPhysXNumVerts = iVertexCount;
				unsigned int uiPhysXNumFaces = pkTri->GetTriangleCount();
				NxU32* pkPhysXMeshFaces = new NxU32[uiPhysXNumFaces * 3];

				int v=0;
				for (unsigned int k=0 ; k<uiPhysXNumFaces ; k++)
				{
					unsigned short vi[3];
					pkTri->GetTriangleIndices(k, vi[0], vi[1], vi[2]);

					pkPhysXMeshFaces[v++] = vi[0];
					pkPhysXMeshFaces[v++] = vi[1];
					pkPhysXMeshFaces[v++] = vi[2];
				}
				NxTriangleMeshDesc kTriMeshDesc;
				kTriMeshDesc.numVertices = uiPhysXNumVerts;
				kTriMeshDesc.numTriangles = uiPhysXNumFaces;
				kTriMeshDesc.pointStrideBytes = sizeof(NxVec3);
				kTriMeshDesc.triangleStrideBytes = 3 * sizeof(NxU32);
				kTriMeshDesc.points = pkPhysXMeshVerts;
				kTriMeshDesc.triangles = pkPhysXMeshFaces;				
				kTriMeshDesc.flags = 0;
				kTriMeshDesc.heightFieldVerticalAxis = NX_Z;
				kTriMeshDesc.heightFieldVerticalExtent	= -10000.0f;

				// Create Triangle
				NxInitCooking();
				NiPhysXMemStream kMemStream;
				NIVERIFY(NxCookTriangleMesh(kTriMeshDesc, kMemStream));
				NxCloseCooking();
				kMemStream.Reset();
				NxTriangleMesh* pkPhysXMesh =
					pkPhysXManager->m_pkPhysXSDK->createTriangleMesh(kMemStream);

				// Create the terrain actor
				NxTriangleMeshShapeDesc kTriMeshShapeDesc;
				kTriMeshShapeDesc.name = pkTri->GetName();
				kTriMeshShapeDesc.group = iGroup; // int type
				kTriMeshShapeDesc.meshData = pkPhysXMesh;

				// Make mesh for Shapes
				NiPhysXMeshDesc *pkMeshDesc = NiNew NiPhysXMeshDesc;
				pkMeshDesc->SetIsConvex(false);
				pkMeshDesc->SetName(pkTri->GetName());
				pkMeshDesc->SetFlags(kTriMeshDesc.flags);
				pkMeshDesc->SetData(kMemStream.GetSize(), (unsigned char *)kMemStream.GetBuffer());
				pkMeshDesc->ToTriMeshDesc(kTriMeshShapeDesc, true);

				NxMat34 kNxMLocal;
				NxMat34 kNxMWorld;
				NiPhysXTypes::NiTransformToNxMat34(
					pkTri->GetLocalTransform().m_Rotate, pkTri->GetLocalTransform().m_Translate, kNxMLocal);
				NiPhysXTypes::NiTransformToNxMat34(
					pkTri->GetWorldTransform().m_Rotate, pkTri->GetWorldTransform().m_Translate, kNxMWorld);

				// Shapes for the NiPhysXActorDesc
				NiPhysXShapeDesc* pkShape = NiNew NiPhysXShapeDesc;
				pkShape->FromShapeDesc(kTriMeshShapeDesc);
				pkShape->SetMeshDesc(pkMeshDesc);
				pkShape->SetLocalPose(kNxMLocal);
				NiTObjectArray<NiPhysXShapeDescPtr> kShapes;
				kShapes.Add(pkShape);

				// The actor descriptor
				NxActorDesc kActorDesc;
				kActorDesc.name = pkTri->GetName();
				kActorDesc.shapes.pushBack(&kTriMeshShapeDesc);
				kActorDesc.body = 0;
				kActorDesc.density = 1;
				kActorDesc.group = 0; // int type
				kActorDesc.globalPose = kNxMWorld;

				// Set values in the NiPhysXActorDesc 
				NiPhysXActorDesc *pkNiActorDesc = NiNew NiPhysXActorDesc;
				pkNiActorDesc->SetConstants(kActorDesc.name, kActorDesc.density,
					kActorDesc.flags, kActorDesc.group, 0, kShapes);
				pkNiActorDesc->AddState(kActorDesc.globalPose);
				
				// Create the actor
				bool vaild = kActorDesc.isValid();
				NxActor *pkActor = pkNxScene->createActor(kActorDesc);
				pkActor->createShape(kTriMeshShapeDesc);
				pkNiActorDesc->SetActor(pkActor);
				pkSceneDesc->AddActor(pkNiActorDesc);
			}
		}
	}

	// Create SnapShot
	pkPhysXScene->SetSnapshot(pkSceneDesc);
	kStream.InsertObject((NiObject*)pkPhysXScene);

	// Backup Original file to ".OLD"
	std::wstring wstrOldFile = wstrFilePath + _T(".OLD");
	_wrename(wstrFilePath.c_str(), wstrOldFile.c_str());

	// Path Calculate
	std::wstring wstrLocation = wstrFilePath;
	wstrLocation.resize(wstrLocation.length() + 1);
	std::wstring wstrFolder;
	std::wstring wstrFile;
	//BM::DivFolderAndFileName(wstrLocation, wstrFolder, wstrFile);
	std::wstring wstrSavePath = wstrOutPath;
	BM::AddFolderMark(wstrSavePath);
	wstrSavePath += wstrFileName;
	char szSavePath[256];
	CharactersConverted = 0;
	wcstombs_s(&CharactersConverted, szSavePath, 
				wstrSavePath.length()+1, wstrSavePath.c_str(), 
			   _TRUNCATE);

	// Save nif file
	kStream.Save(szSavePath);

	// Complete
	std::wstring wstrText(L"Complete : ");
	wstrText += wstrFileName;
	printf("%s \n", wstrText);

	pkPhysXScene = 0;
}
