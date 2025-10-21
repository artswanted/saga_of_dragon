//
// HandOver, 강정욱 2008.01.29
//
// 몬스터의 움직임, 이동을 정하는 WayPoint Object를 관리 해줍니다.
// WayPoint는 txt파일로 저장이 됩니다.
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include <string>
#include "MPgWayPoint.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

WayPointGroup::WayPointGroup()
{
	m_pmSettedWayPointList = new ArrayList;
}

WayPointGroup::~WayPointGroup()
{
}


MPgWayPoint::MPgWayPoint(void)
{
	m_pkWayPointConsole = NULL;
}

MPgWayPoint::~MPgWayPoint(void)
{
	NiDelete m_pkHighlightColor;
    MDisposeRefObject(m_pkWayPointConsole);
}
//---------------------------------------------------------------------------
void MPgWayPoint::Initialize()
{
	m_iEntityCount = 0;

	m_pkCurrentGroup = 0;
	m_pmWayPointGroup = new ArrayList;
	m_bIsLoaded = false;

	CreateWayPointConsole();
}
//---------------------------------------------------------------------------
void MPgWayPoint::Clear()
{
	m_iEntityCount = 0;
	m_pkCurrentGroup = 0;
	m_bIsLoaded = false;

	m_pmWayPointGroup->Clear();
}
//---------------------------------------------------------------------------
void MPgWayPoint::CreateWayPointConsole()
{
    if (!m_pkWayPointConsole)
    {
		if (MFramework::Instance)
		{
			m_pkWayPointConsole = NiNew NiScreenConsole();
			MInitRefObject(m_pkWayPointConsole);

			const char* pcPath = MStringToCharPointer(String::Concat(
				MFramework::Instance->AppStartupPath, "Data\\"));
			m_pkWayPointConsole->SetDefaultFontPath(pcPath);
			MFreeCharPointer(pcPath);

			m_pkWayPointConsole->SetFont(m_pkWayPointConsole->CreateConsoleFont());
			m_pkWayPointConsole->Enable(true);
		}
    }
}
//---------------------------------------------------------------------------
void MPgWayPoint::Render(MRenderingContext* pmRenderingContext)
{
    NiEntityRenderingContext* pkContext = 
        pmRenderingContext->GetRenderingContext();
    NiCamera* pkCam = pkContext->m_pkCamera;
    //clear the z-buffer
    pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

	if (m_pkCurrentGroup)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pkCurrentGroup);
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
			RenderWayPointConsole(pmRenderingContext, pkData);
		}
	}
}
//---------------------------------------------------------------------------
void MPgWayPoint::RenderWayPointConsole(MRenderingContext* pmRenderingContext, WayPointData *pkData)
{
    NiRenderer* pkRenderer = pmRenderingContext
        ->GetRenderingContext()->m_pkRenderer;

	CreateWayPointConsole();
	// Render screen console.
    if (m_pkWayPointConsole)
    {
		MEntity *pkEntity = 0;
		if (!pkData->m_pkEntity)
		{
			unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
			for (unsigned int i = 0 ; i < uiEntityCount ; i++)
			{
				MEntity *pkTemp = MFramework::Instance->Scene->GetEntities()[i];
				if (pkTemp->Name->Equals(pkData->m_strName))
				{
					pkEntity = pkTemp;
					break;
				}
			}
			if (!pkEntity)
				return ;

			pkData->m_pkEntity = pkEntity;
		}
		pkEntity = pkData->m_pkEntity;

		float fWidth=0, fHeight=0;
		MViewport *pkViewport = 0;
		if (MFramework::Instance->ViewportManager->get_ExclusiveViewport())
		{
			pkViewport = MFramework::Instance->ViewportManager->get_ExclusiveViewport();
		}
		else if (MFramework::Instance->ViewportManager->get_ActiveViewport())
		{
			pkViewport = MFramework::Instance->ViewportManager->get_ActiveViewport();
		}
		if (!pkViewport)
			return;

		fWidth = (float)pkViewport->Width;
		fHeight = (float)pkViewport->Height;
		m_pkWayPointConsole->SetDimensions(NiPoint2(fWidth, fHeight));

		MEntity* pmCameraEntity = pkViewport->get_CameraEntity();
	    NiAVObject* pkCameraObject = pmCameraEntity->GetSceneRootPointer(0);
		NiCamera* pkCamera = NiDynamicCast(NiCamera, pkCameraObject);
		if (!pkCamera)
			return;

		NiEntityPropertyInterface* pkEntityProp = 
			pkEntity->GetNiEntityInterface();
		NiPoint3 kPoint;
		pkEntityProp->GetPropertyData("Translation", kPoint);

		kPoint.z = kPoint.z;

		float fX, fY;
		kPoint.z = kPoint.z + 100;
		pkCamera->WorldPtToScreenPt(kPoint, fX, fY);
		float fCenterX = (fX) * fWidth;
		float fCenterY = (1-fY) * fHeight;
		if (fCenterX > fWidth - fWidth*0.05f || fCenterX < fWidth*0.05f ||
			fCenterY > fHeight - fHeight*0.03f || fCenterY < fHeight*0.03f)
			return;

		String *strConsole = pkData->m_iIndex.ToString();
		strConsole = strConsole->Insert(strConsole->Length, "_");
		strConsole = strConsole->Insert(strConsole->Length, pkData->m_fRadius.ToString());
		float fDx = (float)strConsole->Length * (float)m_pkWayPointConsole->GetFont()->m_uiCharWidth * 0.5f;
		float fDy = (float)m_pkWayPointConsole->GetFont()->m_uiCharHeight * 0.5f;
		float fRenderX = fCenterX - fDx;
		float fRenderY = fCenterY + fDy;

		m_pkWayPointConsole->SetOrigin(NiPoint2(fRenderX, fRenderY));
		m_pkWayPointConsole->SetCamera(pkCamera);
		const char* pcWayPoint = MStringToCharPointer(strConsole);
		m_pkWayPointConsole->SetLine(pcWayPoint, 0);
		MFreeCharPointer(pcWayPoint);
		m_pkWayPointConsole->RecreateText();


		//////////////////////////////////////////////////////////////////////////////
        NiScreenTexture* pkConsoleTexture = m_pkWayPointConsole
            ->GetActiveScreenTexture();
        if (pkConsoleTexture)
        {
            pkConsoleTexture->Draw(pkRenderer);
        }
    }
}

void MPgWayPoint::MakeFile(String *strFilePath)
{
	std::wstring wstrFilePath;
	wstrFilePath.resize(strFilePath->Length);
	for (int i=0 ; i<strFilePath->Length ; i++)
	{
		wstrFilePath.at(i) = strFilePath->get_Chars(i);
	}

	FILE *fp;
	_wfopen_s(&fp, wstrFilePath.c_str(), L"w");
	if (fp)
	{
		fwprintf(fp, L"Total %d\n", m_pmWayPointGroup->Count);
		for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
		{
			WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));

			std::wstring wstrName;
			wstrName.resize(pkGroup->m_strName->Length);
			for (int j=0 ; j<pkGroup->m_strName->Length ; j++)
			{
				wstrName.at(j) = pkGroup->m_strName->get_Chars(j);
			}

			if(wstrName.empty())
			{
				::MessageBox(0, "그룹이름이 없습니다.", 0, 0);
				continue;
			}
			if(pkGroup->m_pmSettedWayPointList->Count == 0)
			{
				::MessageBox(0, "WapPointList 가 없습니다.", 0, 0);
				continue;
			}

			fwprintf(fp, L"Group %d %s %d\n{\n", i+1, wstrName.c_str(), pkGroup->m_pmSettedWayPointList->Count);
			for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
			{
				WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

				fwprintf(fp, L"\t%d %f %f %f %f\n",
					pkData->m_iIndex,
					pkData->m_fRadius,
					pkData->m_fPosX,
					pkData->m_fPosY,
					pkData->m_fPosZ );
			}
			fwprintf(fp, L"}\n");
		}
	}
	fclose(fp);
}

void MPgWayPoint::LoadFile(String *strFilePath)
{
	ClearWayPointData_All();
	DeleteCircle();

	std::wstring wstrFilePath;
	wstrFilePath.resize(strFilePath->Length);
	for (int i=0 ; i<strFilePath->Length ; i++)
	{
		wstrFilePath.at(i) = strFilePath->get_Chars(i);
	}

	std::list<std::wstring> slGroupName;

	FILE *fp;
	_wfopen_s(&fp, wstrFilePath.c_str(), L"r");
	if (fp)
	{
		wchar_t szTemp[256];
		wchar_t szName[256];
		int iTemp;
		int iTotalCount = 0;
		fwscanf(fp, L"%s %d", szTemp, &iTotalCount);
		for (int ui=0 ; ui<iTotalCount ; ui++)
		{
			//WayPointGroup *pkGroup = new WayPointGroup;
			int iCount;
			fwscanf(fp, L"%s %d %s %d %s",
				szTemp,
				&iTemp,
				szName,
				&iCount,
				szTemp);
			slGroupName.push_back(szName);
			AddGroup(szName);
			//pkGroup->m_strName = new String(szName);

			for (int i=0 ; i<iCount ; i++)
			{
				int iIndex;
				float fRadius;
				float fPosX;
				float fPosY;
				float fPosZ;

				fwscanf(fp, L"\t%d %f %f %f %f\n",
					&iIndex,
					&fRadius,
					&fPosX,
					&fPosY,
					&fPosZ );

				Guid kGuid = Guid::NewGuid();
				
				WayPointData *pkData = AddWayPoint(szName, kGuid.ToString(), iIndex,
					fRadius, fPosX, fPosY, fPosZ);

				if (pkData && pkData->m_pkEntity)
				{
					pkData->m_pkEntity->set_Hidden(true);
				}
			}

			fwscanf(fp, L"%s", szTemp);
		}
	}
	m_pkCurrentGroup = 0;
	m_strCurrentGroupName = 0;
	fclose(fp);

	//!/
	slGroupName.sort();
	std::list<std::wstring>::iterator itr = slGroupName.begin();
	for(; itr != slGroupName.end(); ++itr)
	{
		for(int nn=0; nn<m_pmWayPointGroup->Count; ++nn)
		{
			WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(nn));
			if(pkGroup->m_strName->Equals((*itr).c_str()))
			{
				m_pmWayPointGroup->Remove(pkGroup);
				m_pmWayPointGroup->Add(pkGroup);
				break;
			}
		}
	}

	m_bIsLoaded = true;
}

NiAVObject* GetNodeByName(const NiFixedString &kString)
{
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
	for (int i=0 ; i<pkEntities->Length ; i++)
	{
		MEntity* pkEntity = pkEntities[i];
        unsigned int uiSceneRootCount = pkEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
		{
			NiNode *pkRoot = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(0));
			
			if(pkRoot)
			{
				NiAVObject *pkObject = pkRoot->GetObjectByName(kString);
				if (pkObject)
				{
					return pkObject;
				}
			}
		}
	}

	return 0;
}

NiAVObject* GetRootNode(NiNode *pkRootNode)
{
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();

	for (int i=0 ; i<pkEntities->Length ; i++)
	{
		MEntity* pkEntity = pkEntities[i];
        unsigned int uiSceneRootCount = pkEntity->GetSceneRootPointerCount();

		for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
		{
			NiNode *pkRoot = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(0));
			pkRootNode->AttachChild(pkRoot);
		}
	}

	return pkRootNode;
}

//
bool PickNearTriangle(NiPoint3 *pkOutPts,
					  NiPoint3 kPoint,
					  NiPoint3 kRotPoint,
					  float *pfDistance,
					  NiAVObject *pkTarget,
					  bool bFrontOnly)
{
	bool bFind = false;
	NiTriStrips *pkTri = 0;

	NiPick* pkPick;
    pkPick = NiNew NiPick();
	pkPick->SetCoordinateType(NiPick::WORLD_COORDINATES);
	pkPick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	pkPick->SetFrontOnly(bFrontOnly);
	pkPick->SetTarget(pkTarget);

	// 픽 된 물체가 있다!
	if (pkPick->PickObjects(kPoint, kRotPoint,  false))
	{
		// 픽된 물체중에 가까운것을 찾자.
		const NiPick::Results& kPickResults = pkPick->GetResults();
		for (unsigned int j=0 ; j<kPickResults.GetSize() ; j++)
		{
			NiPick::Record* pkPickRecord = kPickResults.GetAt(j);
			if (!pkPickRecord)
				continue;

			NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
			
			// tristips 만 찾고, 가까운것을 찾자.
			if (NiIsKindOf(NiTriStrips, pkPickedObject))
			{
				pkTri = NiDynamicCast(NiTriStrips, pkPickedObject);
				unsigned short vi[3];
				pkPickRecord->GetVertexIndices(vi[0], vi[1], vi[2]);
				NiPoint3 kFindPoint[3];
				kFindPoint[0] = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[0]];
				kFindPoint[1] = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[1]];
				kFindPoint[2] = pkTri->GetWorldTransform() * pkTri->GetVertices()[vi[2]];
				NiPoint3 kCenter = kFindPoint[0] + kFindPoint[1] + kFindPoint[2];
				kCenter = pkPickRecord->GetIntersection();
				float fFindDistance = abs((kCenter - kPoint).Length());

				if( fFindDistance < *pfDistance )
				{
					*pfDistance = fFindDistance;
					pkOutPts[0] = kFindPoint[0];
					pkOutPts[1] = kFindPoint[1];
					pkOutPts[2] = kFindPoint[2];

					bFind = true;
				}
			}
		}
	}

	return bFind;
}


bool GetNearTriangle(NiPoint3 *pkOutPts, ArrayList *pkPathEntities, NiPoint3 kOrgPoint, float fDistance, NiPoint3 kLimitNormal)
{
	bool bFind = false;
	NiTriStrips *pkTri = 0;

	NiPoint3 ptAxisVec(0, 0, 1);

	NiPoint3 kPoint = kOrgPoint + NiPoint3(0, 0, 50);

	// 여러 각을 조사
	for (int i=0 ; i<32 ; i++)
	{
		float fAngle = ((360.0f / 32.0f) * i);
		NiPoint3 kAccPt(0, fDistance, 0);

		NiQuaternion kQuat;
		NiMatrix3 kMat;
		NiPoint3 kRotPoint;
		kMat.MakeIdentity();
		float fRadian = (fAngle * static_cast<float>(Math::PI) / 180.0f);
		kQuat = NiQuaternion(fRadian, ptAxisVec);
		kQuat.Normalize();
		kQuat.ToRotation(kMat);
		//kRotPoint = (kMat * kAccPt) + kPoint;
		kRotPoint = (kMat * kAccPt);
		NiPoint3 kURotPoint = kRotPoint;
		kURotPoint.Unitize();

		//제한 노멀을 벗어났다면
		if(kLimitNormal != NiPoint3::ZERO && kLimitNormal.Dot(kURotPoint) <= 0.9f)
		{
			continue;
		}

		for (int j=0 ; j<pkPathEntities->Count ; j++)
		{
            MEntity* pkEntity = dynamic_cast<MEntity*>(
                pkPathEntities->Item[j]);

			for (unsigned int k=0 ; k<pkEntity->GetSceneRootPointerCount() ; k++)
			{
				NiAVObject *pkAVObject = pkEntity->GetSceneRootPointer(k);
				if (PickNearTriangle(pkOutPts, kPoint, kRotPoint, &fDistance, pkAVObject, (kLimitNormal == NiPoint3::ZERO)))
				{
					bFind = true;
				}
			}
		}
	}

	return bFind;
}
bool GetNearTriangle_Old(NiPoint3 *pkOutPts, NiAVObject *pkNode, NiPoint3 kOrgPoint, float fDistance, NiPoint3 kLimitNormal)
{
	bool bFind = false;
	NiTriStrips *pkTri = 0;

	NiPoint3 ptAxisVec(0, 0, 1);

	NiPoint3 kPoint = kOrgPoint + NiPoint3(0, 0, 50);

	// 여러 각을 조사
	for (int i=0 ; i<32 ; i++)
	{
		float fAngle = ((360.0f / 32.0f) * i);
		NiPoint3 kAccPt(0, fDistance, 0);

		NiQuaternion kQuat;
		NiMatrix3 kMat;
		NiPoint3 kRotPoint;
		kMat.MakeIdentity();
		float fRadian = (fAngle * static_cast<float>(Math::PI) / 180.0f);
		kQuat = NiQuaternion(fRadian, ptAxisVec);
		kQuat.Normalize();
		kQuat.ToRotation(kMat);
		//kRotPoint = (kMat * kAccPt) + kPoint;
		kRotPoint = (kMat * kAccPt);
		NiPoint3 kURotPoint = kRotPoint;
		kURotPoint.Unitize();

		//제한 노멀을 벗어났다면
		if(kLimitNormal != NiPoint3::ZERO && kLimitNormal.Dot(kURotPoint) <= 0.9f)
		{
			continue;
		}

		if (pkNode)
		{
			if (PickNearTriangle(pkOutPts, kPoint, kRotPoint, &fDistance, pkNode, (kLimitNormal == NiPoint3::ZERO)))
			{
				bFind = true;
			}
		}
		else
		{
			NiAVObject *pkTarget = GetNodeByName("paths");
			NiNode *pkRoot = NiDynamicCast(NiNode, pkTarget);
			if (PickNearTriangle(pkOutPts, kPoint, kRotPoint, &fDistance, pkRoot, (kLimitNormal == NiPoint3::ZERO)))
			{
				bFind = true;
			}

			pkTarget = GetNodeByName("physx");
			pkRoot = NiDynamicCast(NiNode, pkTarget);
			if (PickNearTriangle(pkOutPts, kPoint, kRotPoint, &fDistance, pkRoot, (kLimitNormal == NiPoint3::ZERO)))
			{
				bFind = true;
			}
		}
	}

	return bFind;
}

void MPgWayPoint::AddGroup(String *strGroupName)
{
	DeleteCircle();

	bool bIsExist = false;
	
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		
		if (pkGroup->m_strName->Equals(strGroupName))
		{
			bIsExist = true;
			m_pkCurrentGroup = pkGroup;
		}
	}

	if (!bIsExist)
	{
		WayPointGroup *pkGroup = new WayPointGroup;
		pkGroup->m_strName = strGroupName;

		m_pmWayPointGroup->Add(pkGroup);
		m_pkCurrentGroup = pkGroup;
	}
}

void MPgWayPoint::DelGroup(String *strGroupName)
{
	DeleteCircle();

	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		
		if (pkGroup->m_strName->Equals(strGroupName))
		{
			for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
			{
				WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

				if (pkData)
				{
					DeleteWayPoint(pkData->m_pkEntity);
					MFramework::Instance->Scene->RemoveEntity(pkData->m_pkEntity, false);
					pkGroup->m_pmSettedWayPointList->Remove(pkData);
					j -= 1;
				}
				if (pkGroup->m_pmSettedWayPointList->Count <= 0)
				{
					break;
				}
			}

			m_pmWayPointGroup->Remove(pkGroup);
			break;
		}
	}
}

void MPgWayPoint::ChangeGroup(String *strGroupName)
{
	// 투명화

	SetCurrentGroupName(strGroupName);

	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));

		if (pkGroup && pkGroup->m_strName->Equals(strGroupName))
		{
			m_pkCurrentGroup = pkGroup;
		}
	}

	String *strPastGroupName = m_strCurrentGroupName;

	//for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	//{
	//	WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));

	//	bool bHideen = true;
	//	bool bLight = false;
	//	if (pkGroup->m_strName->Equals(strGroupName))
	//	{
	//		bHideen = false;
	//		m_pkCurrentGroup = pkGroup;
	//	}
	//	if (pkGroup->m_strName->Equals(strPastGroupName))
	//	{
	//		bHideen = false;
	//		bLight = true;
	//	}

	//	for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
	//	{
	//		WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
	//		pkData->m_pkEntity->set_Hidden(bHideen);
	//		pkData->m_pkEntity->set_Frozen(false);

	//		if (bLight)
	//		{
	//			MFramework::Instance->LightManager->AddEntityToDefaultLights(pkData->m_pkEntity);
	//			pkData->m_pkEntity->set_Frozen(true);
	//		}
	//	}
	//}
}

void MPgWayPoint::ChangeGroupName(String *strOrgGroupName, String *strNewGroupName)
{
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		if (pkGroup->m_strName->Equals(strOrgGroupName))
		{
			pkGroup->m_strName = strNewGroupName;

			break;
		}
	}
}

void MPgWayPoint::ChangeIndex(MEntity *pkEntity, int iIndex)
{
	if (m_pkCurrentGroup)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pkCurrentGroup);
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

			if (pkData->m_pkEntity == pkEntity)
			{
				pkData->m_iIndex = iIndex;
				break;
			}
		}
	}
}

void MPgWayPoint::ChangeRadius(MEntity *pkEntity, float fRadius)
{
	if (m_pkCurrentGroup)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pkCurrentGroup);
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

			if (pkData->m_pkEntity == pkEntity)
			{
				pkData->m_fRadius = fRadius;
				break;
			}
		}
	}
}

void MPgWayPoint::ApplyPosition()
{
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));

		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

			if (pkData->m_pkEntity)
			{
				NiPoint3 kPoint;
				pkData->m_pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
				pkData->m_fPosX = kPoint.x;
				pkData->m_fPosY = kPoint.y;
				pkData->m_fPosZ = kPoint.z;
			}
		}
	}

}

void MPgWayPoint::ShowCircle()
{
	DeleteCircle();
	ApplyPosition();

	if (m_pkCurrentGroup)
	{
        MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
        if (!pkPalette)
		{
			::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
            return;
		}
        MEntity *pkTemplate = pkPalette->GetEntityByName("[General]Target.waypoint_circle");
        if (!pkTemplate)
		{
			::MessageBox(0, "[General]Target.waypoint_circle 가 없습니다.", 0, 0);
            return;
		}

		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pkCurrentGroup);
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
			if (pkData->m_fRadius <= 0.0f)
			{
				continue;
			}

            // Create WayPoint Circle Entity
			String *strCloneName =
				MFramework::Instance->Scene->GetUniqueEntityName(
				String::Concat(pkTemplate->Name,
				" 01"));
            MEntity *pkNewEntity;
			pkNewEntity = pkTemplate->Clone(strCloneName, true);

			NiPoint3 kPoint = NiPoint3(pkData->m_fPosX, pkData->m_fPosY, pkData->m_fPosZ);
            pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPoint, false);
			pkNewEntity->GetNiEntityInterface()->SetPropertyData("Scale", pkData->m_fRadius / 100, false);

			MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
		}
	}
}

void MPgWayPoint::DeleteCircle()
{
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();

	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];
		if (MFramework::Instance->ExtEntity->IsExtEntityType(pkEntity) ==
			MPgExtEntity::ExtEntityType_WayPoint_Circle)
		{
			MFramework::Instance->Scene->RemoveEntity(pkEntity, false);
		}
	}
}

int MPgWayPoint::GetGroupCount()
{
	return m_pmWayPointGroup->Count;
}

String* MPgWayPoint::GetGroupName(int iIndex)
{
	WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(iIndex));
	if (!pkGroup)
	{
		return 0;
	}

	return pkGroup->m_strName;
}

#define PG_USE_FIND_PATH_NEW_VERSION
#define PG_USE_FIND_PATH_OLD_VERSION
void MPgWayPoint::Array(String *strGroupName)
{
	DeleteCircle();

#ifdef PG_USE_FIND_PATH_NEW_VERSION
	MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
    ArrayList* pkPathEntities = new ArrayList();

	for (int i=0 ; i<pkEntities->Count ; i++)
	{
		MEntity *pkEntity = pkEntities[i];

		if (pkEntity->get_PGProperty() == MEntity::ePGProperty::Path ||
			pkEntity->get_PGProperty() == MEntity::ePGProperty::PhysX )
		{
			pkPathEntities->Add(pkEntity);
		}
	}
#endif

	NiPoint3 *pkTriangle = NiNew NiPoint3[3];
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));

		// check current;
		if ((strGroupName->Length <= 0 && pkGroup == m_pkCurrentGroup) ||
			pkGroup->m_strName->Equals(strGroupName))
		{
			for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
			{
				WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));

				NiPoint3 kPoint;
				pkData->m_pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);

				bool bFind = false;
#ifdef PG_USE_FIND_PATH_NEW_VERSION
				if (!bFind)
				{
					bFind = GetNearTriangle(pkTriangle, pkPathEntities, kPoint, 1000, NiPoint3::ZERO);
				}
#endif
#ifdef PG_USE_FIND_PATH_OLD_VERSION
				if (!bFind)
				{
					// found paths
					NiAVObject *pkNode = GetNodeByName("paths");
					assert(pkNode || "path node is not avail");
					bFind = GetNearTriangle_Old(pkTriangle, pkNode, kPoint, 1000, NiPoint3::ZERO);
				}
#endif
				if (!bFind)
				{
					continue;
				}
				bFind = false;

				NiPlane kPlane(pkTriangle[0], pkTriangle[1], pkTriangle[2]);
				float fDis = kPlane.Distance(kPoint);
				NiPoint3 kNormalInv = kPlane.GetNormal() * -1;
				// intersection
				NiPoint3 kPathInter;
				kPathInter = kPoint + (kNormalInv * fDis);
#ifdef PG_USE_FIND_PATH_NEW_VERSION
				if (!bFind)
				{
					bFind = GetNearTriangle(pkTriangle, pkPathEntities, kPoint, 1000, kPlane.GetNormal());
				}
#endif
#ifdef PG_USE_FIND_PATH_OLD_VERSION
				if (!bFind)
				{
					// found physx
					//pkNode = GetNodeByName("physx");
					//NiNode* pkSecondNode = NiNew NiNode();
					//GetRootNode(pkSecondNode);
					bFind = GetNearTriangle_Old(pkTriangle, 0, kPoint, 1000, kPlane.GetNormal());
				}
#endif
				if (!bFind)
				{
					continue;
				}
				kPlane = NiPlane(pkTriangle[0], pkTriangle[1], pkTriangle[2]);
				fDis = kPlane.Distance(kPoint);
				kNormalInv = kPlane.GetNormal() * -1;
				// intersection
				NiPoint3 kPhysxInter;
				kPhysxInter = kPoint + (kNormalInv * fDis);

				// Modify
				NiPoint3 kCenter = (kPathInter + kPhysxInter) * 0.5f;
				pkData->m_fRadius = (kPathInter - kCenter).Length();
				pkData->SetPos(kCenter);
				pkData->m_pkEntity->GetNiEntityInterface()->SetPropertyData("Translation", kCenter);

				if (!bFind)
				{
				}
			}

			break;
		}
	}

}

//---------------------------------------------------------------------------
// Entity가 없는 경우에는 새로 Create 해준다.
WayPointData* MPgWayPoint::AddWayPoint(String *strGroupName, String *strName, int iIndex, float fRadius, float fPosX, float fPosY, float fPosZ)
{
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
    if (pkPalette == NULL)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
	    return 0;
	}
	String* strEntityName = "[General]Target.waypoint_target";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
    if (pkTemplate == NULL)
	{
		::MessageBox(0, "[General]Target.waypoint_target 가 없습니다.", 0, 0);
	    return 0;
	}

    MScene* pmScene = MFramework::Instance->Scene;

	MEntity* pkNewEntity = pkTemplate->Clone(strName, false);
    pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
        MFramework::Instance->ExternalAssetManager);
	pkNewEntity->MasterEntity = pkTemplate;

	NiPoint3 kPoint = NiPoint3(fPosX, fPosY, fPosZ);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPoint);

    MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
	
	////////////////////////////////////////////////////////////////////////////////
	WayPointData *pkData = new WayPointData;
	pkData->Initialize(pkNewEntity, strName, iIndex, fRadius, fPosX, fPosY, fPosZ);
	pkData->m_pkEntity = pkNewEntity;

	bool bAdded = false;
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		if (pkGroup->m_strName->Equals(strGroupName))
		{
			pkGroup->m_pmSettedWayPointList->Add(pkData);

			bAdded = true;
			break;
		}
	}

	if (!bAdded)
	{
		WayPointGroup *pkGroup = new WayPointGroup;
		pkGroup->m_strName = strGroupName;
		pkGroup->m_pmSettedWayPointList->Add(pkData);

		m_pmWayPointGroup->Add(pkGroup);
	}

	m_iEntityCount += 1;

	return pkData;
}
//---------------------------------------------------------------------------
// Entity 가 있는 경우 포인터만 넘겨준다.
void MPgWayPoint::AddWayPoint(MEntity *pkEntity, String *strName, int iIndex, float fRadius, float fPosX, float fPosY, float fPosZ)
{
	if (m_strCurrentGroupName->Length <= 0)
		return;

	String *strGroupName = m_strCurrentGroupName;
	WayPointData *pkData;
	pkData = new WayPointData;

	if (fRadius <= 0)
	{
		fRadius = 60;
	}

	pkData->Initialize(pkEntity, strName, iIndex, fRadius, fPosX, fPosY, fPosZ);
	pkData->m_pkEntity = pkEntity;

	bool bAdded = false;
	WayPointGroup *pkGroup;
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		if (pkGroup->m_strName->Equals(strGroupName))
		{
			pkGroup->m_pmSettedWayPointList->Add(pkData);

			bAdded = true;
			break;
		}
	}

	if (!bAdded)
	{
		pkGroup = new WayPointGroup;
		pkGroup->m_strName = strGroupName;
		pkGroup->m_pmSettedWayPointList->Add(pkData);

		m_pmWayPointGroup->Add(pkGroup);
	}

	// insert index
	// index와 radius를 만들어야 하네.
	if (iIndex == -1)
	{
		int iLastIdx = 0;
		for (int i=0 ; i<pkGroup->m_pmSettedWayPointList->Count ; i++)
		{
			WayPointData *pkRcvData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(i));
			
			if (iLastIdx < pkRcvData->m_iIndex)
			{
				iLastIdx = pkRcvData->m_iIndex;
			}
		}
		pkData->m_iIndex = iLastIdx + 1;
	}

	m_iEntityCount += 1;
}
//---------------------------------------------------------------------------
void MPgWayPoint::DeleteWayPoint(MEntity *pkEntity)
{
	if (m_pkCurrentGroup)
	{
		for (int i=0 ; i<m_pkCurrentGroup->m_pmSettedWayPointList->Count ; i++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(m_pkCurrentGroup->m_pmSettedWayPointList->get_Item(i));
			if (pkData->m_pkEntity == pkEntity)
			{
				m_pkCurrentGroup->m_pmSettedWayPointList->RemoveAt(i);
				m_pkCurrentGroup = 0;
				break;
			}
		}
	}
}
//---------------------------------------------------------------------------
void MPgWayPoint::ClearWayPointData_CurrentGroup()
{
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];
		if (IsWayPoint(pkEntity->Name))
		{
			for (int j=0 ; j<m_pkCurrentGroup->m_pmSettedWayPointList->Count ; j++)
			{
				WayPointData *pkData = dynamic_cast<WayPointData *>(m_pkCurrentGroup->m_pmSettedWayPointList->get_Item(j));
				
				if (pkData->m_pkEntity == pkEntity)
				{
					m_pkCurrentGroup->m_pmSettedWayPointList->Remove(pkData);

					MFramework::Instance->Scene->RemoveEntity(pkEntity,false);
					uiEntityCount -= 1;
					i -= 1;
					m_iEntityCount -= 1;
				}
			}
		}
	}

	m_pkCurrentGroup = 0;
}

void MPgWayPoint::ClearWayPointData_All()
{
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];
		if (IsWayPoint(pkEntity->Name))
		{
			for (int j=0 ; j<m_pmWayPointGroup->Count ; j++)
			{
				WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(j));
				for (int k=0 ; k<pkGroup->m_pmSettedWayPointList->Count ; k++)
				{
					WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(k));

					if (pkData->m_pkEntity == pkEntity)
					{
						MFramework::Instance->Scene->RemoveEntity(pkEntity,false);
						uiEntityCount -= 1;
						i -= 1;
						m_iEntityCount -= 1;
					}
				}
			}
		}
	}

	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		pkGroup->m_pmSettedWayPointList->Clear();
	}
	m_pmWayPointGroup->Clear();

	m_pkCurrentGroup = 0;
}

//---------------------------------------------------------------------------
bool MPgWayPoint::IsWayPoint(String *strName)
{
	if (strName->Equals("[General]Target.waypoint_target"))
		return true;

	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
			if (pkData->m_strName->Equals(strName))
			{
				return true;
			}

		}
	}

	return false;
}

bool MPgWayPoint::IsWayPoint(MEntity *pkEntity)
{
	if (pkEntity->Name->Equals("[General]Target.waypoint_target") ||
		pkEntity->MasterEntity->get_Name()->Equals("[General]Target.waypoint_target"))
		return true;

	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
			if (pkData->m_pkEntity == pkEntity ||
				pkData->m_strName->Equals(pkEntity->Name))
			{
				return true;
			}

		}
	}

	return false;
}
void MPgWayPoint::SetEntityHide(bool bHide)
{
	for (int i=0 ; i<m_pmWayPointGroup->Count ; i++)
	{
		WayPointGroup *pkGroup = dynamic_cast<WayPointGroup *>(m_pmWayPointGroup->get_Item(i));
		if(!pkGroup)
			continue;

		for (int j=0 ; j<pkGroup->m_pmSettedWayPointList->Count ; j++)
		{
			WayPointData *pkData = dynamic_cast<WayPointData *>(pkGroup->m_pmSettedWayPointList->get_Item(j));
			pkData->m_pkEntity->set_Hidden(bHide);
		}
	}
}