// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MSelectionInteractionMode.h"
#include "NiVirtualBoolBugWrapper.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MSelectionInteractionMode::MSelectionInteractionMode() : m_pkGizmo(NULL),
	m_pkMouseClickPos(NULL), m_pkMouseMovePos(NULL),
	m_pkRectangle(NULL), m_pkRectangleMaterial(NULL), m_pkHighlightColor(NULL),
	m_pkDragRect(NULL), m_pkDragFrustum(NULL), m_pkMouseDownPos(NULL),
	m_pkDragVA(NULL), m_pkDragCuller(NULL),
	m_pkPrevMousePos(NULL)
{
    m_pkPickRecord = NiNew NiPick::Record(NULL);
    m_pkPickRecord->SetDistance(NI_INFINITY);
    m_pkPickRecord->SetIntersection(NiPoint3::ZERO);
    m_pkPickRecord->SetNormal(NiPoint3::ZERO);

	m_pkPrevMousePos = NiNew NiPoint2(-999999, -999999);
	m_fPrevDistance = 0;

	m_bVisibleRectangleSelection = false;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::RegisterSettings()
{
    // register the settings of all the classes we inherit from 
    __super::RegisterSettings();

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MSelectionInteractionMode::OnSettingChanged);

    // convert this settings from a distance to a ratio
    float fScale = STANDARD_DISTANCE / m_fDefaultDistance;
    MSettingsHelper::GetStandardSetting(MSettingsHelper::GIZMO_SCALE, fScale,
        pmHandler);
    m_fDefaultDistance = STANDARD_DISTANCE / fScale;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    // if a setting we care about was changed, re-cache it
    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::GIZMO_SCALE))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(
            strSetting, eCategory);
        __box float* pfVal = dynamic_cast<__box float*>(pmObj);
        if (pfVal != NULL)
        {
            m_fDefaultDistance = STANDARD_DISTANCE / *pfVal;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::SetInteractionMode(Object* pmObject, 
    EventArgs* mArgs)
{
    MVerifyValidInstance;

    InteractionModeService->ActiveMode = this;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::ValidateInteractionMode(Object* pmSender,
    UIState* pmState)
{
    MVerifyValidInstance;

    pmState->Checked = (InteractionModeService->ActiveMode == this);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::SetGizmoScale(NiCamera* pkCamera)
{
    // figure out the gizmo's location
    NiPoint3 kCenter;
    SelectionService->SelectionCenter->ToNiPoint3(kCenter);
    m_pkGizmo->SetTranslate(kCenter);
    // figure out how large it needs to be
    if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        NiFrustum pkFrustum = pkCamera->GetViewFrustum();
        m_pkGizmo->SetScale((pkFrustum.m_fRight * 2.0f) / 
            m_fDefaultDistance);
    }
    else
    {
        float fCamDistance = (kCenter - 
            pkCamera->GetWorldTranslate()).Length();
        if ((fCamDistance / m_fDefaultDistance) > 0.0f)
        {
            m_pkGizmo->SetScale((fCamDistance / m_fDefaultDistance) * 
                pkCamera->GetViewFrustum().m_fRight * 2.0f);
        }
        else
        {
            m_pkGizmo->SetScale(1.0f);
        }
    }
    m_pkGizmo->Update(0.0f);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::Do_Dispose(bool bDisposing)
{
	NiDelete m_pkHighlightColor;
	
	NiDelete m_pkDragRect;
	NiDelete m_pkDragCuller;
	NiDelete m_pkDragVA;
	NiDelete m_pkDragFrustum;

	NiDelete m_pkMouseClickPos;
	NiDelete m_pkMouseMovePos;
	NiDelete m_pkMouseDownPos;
	NiDelete m_pkPrevMousePos;

    MDisposeRefObject(m_pkRectangle);

    NiDelete m_pkPickRecord;
    MDisposeRefObject(m_pkGizmo);
    MViewInteractionMode::Do_Dispose(bDisposing);
}
//---------------------------------------------------------------------------
String* MSelectionInteractionMode::get_Name()
{
    return "GamebryoSelection";
}
//---------------------------------------------------------------------------
bool MSelectionInteractionMode::Initialize()
{
    MVerifyValidInstance;

    m_fDefaultDistance = STANDARD_DISTANCE;

    String* pmPath = String::Concat(MFramework::Instance->AppStartupPath,
        "Data\\select.nif");
    NiStream kStream;
    const char* pcPath = MStringToCharPointer(pmPath);
    int iSuccess = NiVirtualBoolBugWrapper::NiStream_Load(kStream, pcPath);
    MFreeCharPointer(pcPath);

	CreateRectangleGeometry();

    if (iSuccess != 0)
    {
        m_pkGizmo = (NiNode*)kStream.GetObjectAt(0);
        MInitRefObject(m_pkGizmo);
        m_pkGizmo->UpdateProperties();
        m_pkGizmo->UpdateEffects();
        m_pkGizmo->Update(0.0f);
    }
    else
    {
        MessageService->AddMessage(MessageChannelType::Errors, 
            "Failed to load scale gizmo file 'select.nif'");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::CreateRectangleGeometry()
{
    MVerifyValidInstance;

	m_pkMouseClickPos = NiNew NiPoint2();
	m_pkMouseMovePos = NiNew NiPoint2();
	m_pkMouseDownPos = NiNew NiPoint2();

    // Create NiLines geometry.
    unsigned short usNumVertices = 4;
    NiPoint3* pkVertices = NiNew NiPoint3[usNumVertices];
    NiBool* pbFlags = NiAlloc(NiBool, usNumVertices);
    for (unsigned int ui = 0; ui < usNumVertices; ui++)
    {
        pbFlags[ui] = true;
    }
    m_pkRectangle = NiNew NiLines(usNumVertices, pkVertices, NULL, NULL, 0,
        NiGeometryData::NBT_METHOD_NONE, pbFlags);
    MInitRefObject(m_pkRectangle);
    m_pkRectangle->SetConsistency(NiGeometryData::MUTABLE);

    // Attach material property.
    m_pkRectangleMaterial = NiNew NiMaterialProperty();
    m_pkRectangleMaterial->SetEmittance(NiColor::BLACK);
    m_pkRectangle->AttachProperty(m_pkRectangleMaterial);

    // Attach vertex color property.
    NiVertexColorProperty* pkVertexColorProperty = NiNew
        NiVertexColorProperty();
    pkVertexColorProperty->SetSourceMode(
        NiVertexColorProperty::SOURCE_IGNORE);
    pkVertexColorProperty->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
    m_pkRectangle->AttachProperty(pkVertexColorProperty);

    // Attach z-buffer property.
    NiZBufferProperty* pkZBufferProperty = NiNew NiZBufferProperty();
    pkZBufferProperty->SetZBufferTest(false);
    pkZBufferProperty->SetZBufferWrite(false);
    m_pkRectangle->AttachProperty(pkZBufferProperty);
	
    // Perform initial update.
    m_pkRectangle->Update(0.0f);
    m_pkRectangle->UpdateProperties();
    m_pkRectangle->UpdateEffects();
    m_pkRectangle->UpdateNodeBound();

	// Init. Logical Dragging Camera and ETC.
	m_pkDragFrustum = NiNew NiFrustum();
	*m_pkDragFrustum = NiFrustum(0, 0, 0, 0, 0, 0);
	m_pkDragRect = NiNew NiRect<float>;
	m_pkDragVA = NiNew NiVisibleArray;
	m_pkDragCuller = NiNew NiCullingProcess(m_pkDragVA);

	// Ready
	UpdateRectangleGeometry();
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::UpdateRectangleGeometry()
{
    MVerifyValidInstance;

    if (!m_pkRectangle ||
		!m_pkMouseClickPos ||
		!m_pkMouseMovePos ||
		!m_pkMouseDownPos)
    {
        return;
    }

	int iCurrentViewport = -1;
	unsigned int uiCount = MFramework::Instance->ViewportManager->ViewportCount;
	float fRatio = 1.0f;	// 전체화면시 배율은 줄어들지 않는다.
	for (unsigned int ui = 0 ; ui < uiCount ; ui++)
	{
		if ( MFramework::Instance->ViewportManager->GetViewport(ui) ==
			MFramework::Instance->ViewportManager->get_ExclusiveViewport() )
		{
			iCurrentViewport = ui;
			fRatio = 1.0f;
			break;
		}
		if ( MFramework::Instance->ViewportManager->GetViewport(ui) ==
			MFramework::Instance->ViewportManager->get_ActiveViewport() )
		{
			iCurrentViewport = ui;
			fRatio = 0.5f;
			break;
		}
	}
	if (iCurrentViewport == -1)
		return;

	// Rectangle Selection 출력위치
	MViewport *pkViewport = MFramework::Instance->ViewportManager->GetViewport(iCurrentViewport);
	m_pkDragRect->m_left = pkViewport->get_Left() + m_pkMouseClickPos->x * fRatio;
	m_pkDragRect->m_top = 1.0f - pkViewport->get_Top() + m_pkMouseClickPos->y * fRatio;
	m_pkDragRect->m_right = pkViewport->get_Left() + m_pkMouseMovePos->x * fRatio;
	m_pkDragRect->m_bottom = 1.0f - pkViewport->get_Top() + m_pkMouseMovePos->y * fRatio;

	// 화면 밖으로 나가면 컬링
	m_pkDragRect->m_right = NiMin(m_pkDragRect->m_right, pkViewport->get_Right());
	m_pkDragRect->m_right = NiMax(m_pkDragRect->m_right, pkViewport->get_Left());
	m_pkDragRect->m_bottom = NiMin(m_pkDragRect->m_bottom, 1.0f - pkViewport->get_Bottom());
	m_pkDragRect->m_bottom = NiMax(m_pkDragRect->m_bottom, 1.0f - pkViewport->get_Top());

    // Adjust left and top locations to ensure that the lines are visible.
    m_pkDragRect->m_left = (m_pkDragRect->m_left == 0.0f ? 0.05f : m_pkDragRect->m_left);
    m_pkDragRect->m_top = (m_pkDragRect->m_top == 1.0f ? 0.999f : m_pkDragRect->m_top);

    NiPoint3* pkVertices = m_pkRectangle->GetVertices();
    pkVertices[0] = NiPoint3(m_pkDragRect->m_left, m_pkDragRect->m_top, 0.0f);
    pkVertices[1] = NiPoint3(m_pkDragRect->m_left, m_pkDragRect->m_bottom, 0.0f);
    pkVertices[2] = NiPoint3(m_pkDragRect->m_right, m_pkDragRect->m_bottom, 0.0f);
    pkVertices[3] = NiPoint3(m_pkDragRect->m_right, m_pkDragRect->m_top, 0.0f);
    m_pkRectangle->GetModelData()->MarkAsChanged(NiGeometryData::VERTEX_MASK);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::RegisterForHighlightColorSetting()
{
    MVerifyValidInstance;

    m_pkHighlightColor = NiNew NiColor(1, 1, 1);
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::RenderGizmo(
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    if ((m_pkGizmo) && (SelectionService->NumSelectedEntities > 0))
    {
        NiEntityRenderingContext* pkContext = 
            pmRenderingContext->GetRenderingContext();
        NiCamera* pkCam = pkContext->m_pkCamera;

        //clear the z-buffer
        pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

        SetGizmoScale(pkCam);
        NiDrawScene(pkCam, m_pkGizmo, *pkContext->m_pkCullingProcess);
    }


	// Render rectangle selection.
	if (m_bVisibleRectangleSelection)
	{
		UpdateRectangleGeometry();
		if (!m_pkHighlightColor)
		{
			RegisterForHighlightColorSetting();
		}
		m_pkRectangleMaterial->SetEmittance(*m_pkHighlightColor);

		NiRenderer* pkRenderer = pmRenderingContext
			->GetRenderingContext()->m_pkRenderer;
		pkRenderer->SetScreenSpaceCameraData();
		m_pkRectangle->RenderImmediate(pkRenderer);
	}

}

// Geometry를 찾는 Stack 재귀 함수, 깊이 탐색
void GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeomerties)
{
	for(unsigned int i = 0;
		i < pkNode->GetChildCount();
		i++)
	{	
		NiAVObject *pkChild = pkNode->GetAt(i);

		if(NiIsKindOf(NiNode, pkChild))
		{
			GetAllGeometries(NiDynamicCast(NiNode, pkChild), kGeomerties);
			//kQueue.AddHead(pkChild);
		}
		else if(NiIsKindOf(NiGeometry, pkChild))
		{
			kGeomerties.AddTail(pkChild);
			NiPoint3 *pkPoint = NiDynamicCast(NiGeometry, pkChild)->GetVertices();
			NiGeometryData *pkData = NiDynamicCast(NiGeometry, pkChild)->GetModelData();
		}
	}
}

//---------------------------------------------------------------------------
ArrayList* MSelectionInteractionMode::GetPileEntities(NiGeometry *pkGeometry, ArrayList *amExceptionEntities)
{
	const NiBound &kBound = pkGeometry->GetWorldBound();
	
	ArrayList *amPileEntities = new ArrayList();

	unsigned int uiPileIdx=0;
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];
		
		// 이미 등록 되어 있는지 체크를 한다.
		bool bContinue = false;
		for (int j = 0 ; j < amExceptionEntities->Count ; j++)
		{
			if(pkEntity == amExceptionEntities->get_Item(j))
			{
				bContinue = true;
				break;
			}
		}
		if (bContinue)
			continue;

		// Entity의 Geometry들을 얻는다.
		NiObjectList kGeometries;
		for(unsigned int i = 0;
			i < pkEntity->GetSceneRootPointerCount();
			i++)
		{
			NiNode *pkRoot = NiDynamicCast(NiNode, pkEntity->GetSceneRootPointer(i));
			if(pkRoot)
			{
				GetAllGeometries(pkRoot, kGeometries);
			}
		}

		bool bBreak = false;
		// 지오 메트리 모두 검사한다.
		while(!kGeometries.IsEmpty() && !bBreak)
		{
			NiGeometry *pkGeo = NiDynamicCast(NiGeometry, kGeometries.GetTail());
			kGeometries.RemoveTail();
	
			const NiBound &kDestBound = pkGeo->GetWorldBound();
			NiPoint3 kPoint = NiPoint3(0,0,0);

			// 우선 근처에 있는 물체인지 체크.
			if (NiBound::TestIntersect(0.0f, kBound, kPoint, kDestBound, kPoint))
			{
				// 
				for (unsigned int iVertCount=0 ;
					iVertCount < pkGeometry->GetModelData()->GetVertexCount() ;
					iVertCount ++)
				{
					NiPoint3 kSrcPoint = pkGeometry->GetModelData()->GetVertices()[iVertCount];
					kSrcPoint = pkGeometry->GetWorldTransform() * kSrcPoint;

					// 
					for (unsigned int iVertCount2=0 ;
						iVertCount2 < pkGeo->GetModelData()->GetVertexCount() ;
						iVertCount2 ++) 
					{
						NiPoint3 kDestPoint = pkGeo->GetModelData()->GetVertices()[iVertCount2];
						kDestPoint = pkGeo->GetWorldTransform() * kDestPoint;

						if (abs(kSrcPoint.x - kDestPoint.x) < 0.05f && 
							abs(kSrcPoint.y - kDestPoint.y) < 0.05f && 
    							abs(kSrcPoint.z - kDestPoint.z) < 0.05f)
						{
							amPileEntities->Add(pkEntity);

							// be ended geometries loop
							iVertCount = pkGeometry->GetModelData()->GetVertexCount();
							iVertCount2 = pkGeo->GetModelData()->GetVertexCount();
							bBreak = true;
						}
					} // End dest loop
				} // End src loop
			} // End intersect
		} // End geometries
	} // End main loop

	return amPileEntities;
}


//---------------------------------------------------------------------------
ArrayList* MSelectionInteractionMode::GetSelectConnectedEntities(MEntity* pkPickedEntity, ArrayList *amExceptionEntities)
{
	if (!pkPickedEntity)
		return 0;

	ArrayList* amConnectedEntities = new ArrayList();

	// Entity의 Geometry들을 얻는다.
	NiObjectList kGeometries;
	for(unsigned int i = 0;
		i < pkPickedEntity->GetSceneRootPointerCount();
		i++)
	{
		NiNode *pkRoot = NiDynamicCast(NiNode, pkPickedEntity->GetSceneRootPointer(i));
		if(pkRoot)
		{
			GetAllGeometries(pkRoot, kGeometries);
		}
	}

	// Geometry들을 검색하면서 겹치는 Entity를 찾아 재귀 호출 한다.
	while(!kGeometries.IsEmpty())
	{
		NiGeometry *pkGeo = NiDynamicCast(NiGeometry, kGeometries.GetTail());
		kGeometries.RemoveTail();

		ArrayList* amPiledEntities = 
			GetPileEntities(pkGeo, amExceptionEntities);

		for (int i=0 ; i<amPiledEntities->Count ; i++)
		{
			amExceptionEntities->Add(amPiledEntities->get_Item(i));
			amConnectedEntities->Add(amPiledEntities->get_Item(i));
		}
		for (int i=0 ; i<amPiledEntities->Count ; i++)
		{
			ArrayList *amConEntities =
				GetSelectConnectedEntities(
				dynamic_cast<MEntity*>(amPiledEntities->get_Item(i)),
				amExceptionEntities);

			if (amConEntities)
			{
				for (int j=0 ; j<amConEntities->Count ; j++)
				{
					amExceptionEntities->Add(amConEntities->get_Item(j));
					amConnectedEntities->Add(amConEntities->get_Item(j));
				}
			}
		}
	}
	
	return amConnectedEntities;
}

//---------------------------------------------------------------------------
void MSelectionInteractionMode::MouseDown(MouseButtonType eType, int iX,
    int iY)
{
    MVerifyValidInstance;

    MViewInteractionMode::MouseDown(eType, iX, iY);

	if (eType == MouseButtonType::LeftButton)
	{
		NiPoint3 kOrigin, kDir;
		NiViewMath::MouseToRay((float) iX, (float) iY, 
			MFramework::Instance->ViewportManager->ActiveViewport->Width,
			MFramework::Instance->ViewportManager->ActiveViewport->Height,
			MFramework::Instance->ViewportManager->ActiveViewport->
			GetNiCamera(), kOrigin, kDir);

		MEntity* pmPickedEntity = 0;
		float fPickDistance = NI_INFINITY;

		if (MFramework::Instance->PickUtility->PerformPick(
			MFramework::Instance->Scene, kOrigin, kDir, false))
		{
			const NiPick* pkPick =
				MFramework::Instance->PickUtility->GetNiPick();
			const NiPick::Results& kPickResults = pkPick->GetResults();
			NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
			if (pkPickRecord)
			{
				if (pkPickRecord->GetDistance() < fPickDistance)
				{
					m_pkPickRecord->SetDistance(pkPickRecord->GetDistance());
					m_pkPickRecord->SetIntersection(
						pkPickRecord->GetIntersection());
					m_pkPickRecord->SetNormal(pkPickRecord->GetNormal());
					fPickDistance = pkPickRecord->GetDistance();
					NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
					pmPickedEntity = MFramework::Instance->PickUtility
						->GetEntityFromPickedObject(pkPickedObject);
				}
			}
		}

		MFramework::Instance->ProxyManager->UpdateProxyScales(
			MFramework::Instance->ViewportManager->ActiveViewport);

		if (MFramework::Instance->PickUtility->PerformPick(
			MFramework::Instance->ProxyManager->ProxyScene, kOrigin, kDir,
			false))
		{
			const NiPick* pkPick =
				MFramework::Instance->PickUtility->GetNiPick();
			const NiPick::Results& kPickResults = pkPick->GetResults();
			NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
			if (pkPickRecord)
			{
				if (pkPickRecord->GetDistance() < fPickDistance)
				{
					m_pkPickRecord->SetDistance(pkPickRecord->GetDistance());
					m_pkPickRecord->SetIntersection(
						pkPickRecord->GetIntersection());
					m_pkPickRecord->SetNormal(pkPickRecord->GetNormal());
					fPickDistance = pkPickRecord->GetDistance();
					NiAVObject* pkPickedProxy = pkPickRecord->GetAVObject();
					MEntity* pmPickedProxy = MFramework::Instance->PickUtility
						->GetEntityFromPickedObject(pkPickedProxy);
					if (pmPickedProxy != NULL &&
						pmPickedProxy->HasProperty("Source Entity"))
					{
						MEntity* pmEntity = dynamic_cast<MEntity*>(
							pmPickedProxy->GetPropertyData("Source Entity"));
						if (pmEntity != NULL)
						{
							if (MFramework::Instance->Scene->IsEntityInScene(
								pmEntity))
							{
								pmPickedEntity = pmEntity;
							}
						}
					}
				}
			}
		}

		// 키보드와 마우스 클릭에 대한 반응
		if (pmPickedEntity != NULL)
		{
			if((GetAsyncKeyState(VK_CONTROL) & 0x8000)
				&& (GetAsyncKeyState(VK_MENU) & 0x8000))
			{
				if (pmPickedEntity)
				{
					SelectionService->ClearSelectedEntities();
					SelectionService->AddEntityToSelection(pmPickedEntity);

					ArrayList *amException = new ArrayList();
					amException->Add(pmPickedEntity);
					ArrayList* amSelection = 
						GetSelectConnectedEntities(pmPickedEntity, amException);

					if(amSelection->Count)
					{
						MEntity *pkEntites[] = NiNew MEntity*[amSelection->Count];
						for(int i = 0; i < amSelection->Count; i++)
						{
							MEntity *pkEntity = dynamic_cast<MEntity *>(amSelection->get_Item(i));
							if(pkEntity)
							{
								pkEntites[i] = pkEntity;
							}
						}

						SelectionService->AddEntitiesToSelection(pkEntites);
						NiDelete[] pkEntites;
					}
				}				
			}
			else if((GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000))//!/ 같은 오브젝트를 쓰는 엔터티를 선택
			{
				if(pmPickedEntity->MasterEntity)
				{
					if(pmPickedEntity->HasProperty("NIF File Path"))
					{
						Object* pPickObject = pmPickedEntity->GetPropertyData("NIF File Path");
						if(pPickObject)
						{
							String* pPickString = dynamic_cast<String*>(pPickObject);
							if(pPickString)
							{
								SelectionService->RemoveEntityFromSelection(pmPickedEntity);

								MEntity *pkEntities[] = MFramework::Instance->Scene->GetEntities();
								for (int nn=0; nn<pkEntities->Count ; ++nn)
								{
									MEntity* pkEntity = pkEntities[nn];
									if(!pkEntity && !pkEntity->MasterEntity)
									{
										continue;
									}
									if(!pkEntity->HasProperty("NIF File Path"))
									{
										continue;
									}
									Object* pObject = pkEntity->GetPropertyData("NIF File Path");
									if(!pObject)
									{
										continue;
									}
									String* pString = dynamic_cast<String*>(pObject);
									if(!pString)
									{
										continue;
									}

									if(pPickString->Equals(pString))
									{
										SelectionService->AddEntityToSelection(pkEntity);
									}
								}
							}
						}
					}
				}
			}
			else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				// first, check if the picked entity is already selected
				bool bAddSelection = true;
				MEntity* amSelection[] = 
					SelectionService->GetSelectedEntities();
				for (int i = 0; i < amSelection->Count; i++)
				{
					if (amSelection[i] == pmPickedEntity)
					{
						bAddSelection = false;
						break;
					}
				}
				if (bAddSelection)
				{
					SelectionService->AddEntityToSelection(pmPickedEntity);
				}
			}
			else if (GetAsyncKeyState(VK_MENU) & 0x8000)
			{
				// first check that the entity is in the selection
				bool bRemove = false;
				MEntity* amSelection[] = 
					SelectionService->GetSelectedEntities();
				for (int i = 0; i < amSelection->Count; i++)
				{
					if (amSelection[i] == pmPickedEntity)
					{
						bRemove = true;
						break;
					}
				}
				if (bRemove)
				{
					SelectionService->RemoveEntityFromSelection(
						pmPickedEntity);
				}
			}
			else
			{
				// HandOver, 순서대로 선택, 강정욱 2008.01.29
				// 이전과 똑같은 마우스 좌표면.. 겹쳐진 것들중 다음 물체를 선택하자.
				if (m_pkPrevMousePos->x == iX && m_pkPrevMousePos->y == iY)
				{
					MEntity* pmNextEntity = 0;
					float fFarDistance = NI_INFINITY;

					// 
					if (MFramework::Instance->PickUtility->PerformPick(
						MFramework::Instance->Scene, kOrigin, kDir, false))
					{
						const NiPick* pkPick = MFramework::Instance->PickUtility->GetNiPick();
						const NiPick::Results& kPickResults = pkPick->GetResults();
						MEntity* pmLastestEntity = SelectionService->SelectedEntity;

						// 선택되어진 것의 다음 거리에 있는 물체를 선택한다.
						for (int i=0 ; i<kPickResults.GetSize() ; i++)
						{
							NiPick::Record* pkPickRecord = kPickResults.GetAt(i);
							if (!pkPickRecord) continue;

							float fDistance = pkPickRecord->GetDistance();
							NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
							MEntity* pmEntity = MFramework::Instance->PickUtility->GetEntityFromPickedObject(pkPickedObject);
							if (pmLastestEntity == pmEntity) continue;

							if (fDistance < fFarDistance && fDistance > m_fPrevDistance)
							{
								pmNextEntity = pmEntity;
								fFarDistance = fDistance;
							}
						}

						// 다음은 없고 사이즈는 1개 초과 일때는.. 마지막 것을 선택 한것이므로..
						// 처음것을 선택해준다.
						if (pmNextEntity)
						{
							SelectionService->SelectedEntity = pmNextEntity;
							m_fPrevDistance = fFarDistance;
						}
						if (!pmNextEntity && kPickResults.GetSize() > 1)
						{
							SelectionService->SelectedEntity = pmPickedEntity;
							m_fPrevDistance = 0;
						}
					}
				}
				else
				{
					// HandOver, 강정욱 2008.01.29
					// 사각 선택이나 그냥 선택을 할 것이다.
					m_pkPrevMousePos->x = iX;
					m_pkPrevMousePos->y = iY;
					m_fPrevDistance = 0;

					// check that the selection doesn't match the entity
					if ((SelectionService->NumSelectedEntities != 1) || 
						(SelectionService->SelectedEntity != pmPickedEntity))
					{
						SelectionService->SelectedEntity = pmPickedEntity;
					}
				}
			}
		}
		else
		{
			if (m_pkMouseDownPos)
			{
				m_pkMouseDownPos->x = (float)iX;
				m_pkMouseDownPos->y = (float)iY;
			}
			SelectionService->SelectedEntity = NULL;
			m_bVisibleRectangleSelection = true;

			if (m_pkMouseClickPos && m_pkMouseMovePos)
			{
				if (MFramework::Instance->ViewportManager->ActiveViewport)
				{
					m_pkMouseClickPos->x =
						(float)iX / 
						(float)MFramework::Instance->ViewportManager->ActiveViewport->Width *
						1.0f;
					m_pkMouseClickPos->y =
						(float)iY / 
						(float)MFramework::Instance->ViewportManager->ActiveViewport->Height *
						1.0f;
					m_pkMouseMovePos->x =
						(float)iX / 
						(float)MFramework::Instance->ViewportManager->ActiveViewport->Width *
						1.0f;
					m_pkMouseMovePos->y =
						(float)iY / 
						(float)MFramework::Instance->ViewportManager->ActiveViewport->Height *
						1.0f;
				}
				else if (MFramework::Instance->ViewportManager->ExclusiveViewport)
				{
					m_pkMouseClickPos->x =
						(float)iX / 
						(float)MFramework::Instance->ViewportManager->ExclusiveViewport->Width *
						1.0f;
					m_pkMouseClickPos->y =
						(float)iY / 
						(float)MFramework::Instance->ViewportManager->ExclusiveViewport->Height *
						1.0f;
					m_pkMouseMovePos->x =
						(float)iX / 
						(float)MFramework::Instance->ViewportManager->ExclusiveViewport->Width *
						1.0f;
					m_pkMouseMovePos->y =
						(float)iY / 
						(float)MFramework::Instance->ViewportManager->ExclusiveViewport->Height *
						1.0f;
				}
				UpdateRectangleGeometry();
			}
		}
	}
}
//---------------------------------------------------------------------------
void MSelectionInteractionMode::MouseUp(MouseButtonType eType, int iX, int iY)
{
	// HandOver, 사각 선택, 강정욱 2008.01.29
	// 사각 선택 모드
	if (m_bVisibleRectangleSelection &&
		m_pkMouseClickPos &&
		m_pkMouseMovePos &&
		((int)m_pkMouseDownPos->x != iX || (int)m_pkMouseDownPos->y != iY))
	{
		UpdateRectangleGeometry();
	
		MViewport *pViewport;
		float fRatio = 1.0f;
		bool bDivision = false;
		if (MFramework::Instance->ViewportManager->get_ExclusiveViewport())
		{
			pViewport = MFramework::Instance->ViewportManager->get_ExclusiveViewport();
			fRatio = 1.0f;
		}
		else if (MFramework::Instance->ViewportManager->get_ActiveViewport())
		{
			pViewport = MFramework::Instance->ViewportManager->get_ActiveViewport();
			fRatio = 0.5f;
			bDivision = true;
		}
		else
			return;

		// 현재 뷰포트의 데이터를 모두 받아놓는다.
		NiCamera *pkCamera = pViewport->GetNiCamera();
		NiFrustum kOldFrustum = pkCamera->GetViewFrustum();

		float fLeft = kOldFrustum.m_fLeft;
		float fRight = kOldFrustum.m_fRight;
		float fTop = kOldFrustum.m_fTop;
		float fBottom = kOldFrustum.m_fBottom;
		float fNear = kOldFrustum.m_fNear;
		float fFar = kOldFrustum.m_fFar;

		float fHorizontalSmall = NiMin(m_pkDragRect->m_left, m_pkDragRect->m_right);
		float fHorizontalLarge = NiMax(m_pkDragRect->m_left, m_pkDragRect->m_right);
		float fVerticalSmall = NiMin(m_pkDragRect->m_top, m_pkDragRect->m_bottom);
		float fVerticalLarge = NiMax(m_pkDragRect->m_top, m_pkDragRect->m_bottom);

		float fCameraX = fHorizontalSmall + (fHorizontalLarge - fHorizontalSmall) * 0.5f;
		float fCameraY = fVerticalSmall + (fVerticalLarge - fVerticalSmall) * 0.5f;

		// 분활 되어있을때는 계산식을 그 뷰포트에 맞게 해준다.
		if (bDivision)
		{
			fHorizontalSmall = (fHorizontalSmall - pViewport->get_Left()) *
				1.0f / (pViewport->get_Right() - pViewport->get_Left());
			fHorizontalLarge = (fHorizontalLarge - pViewport->get_Left()) *
				1.0f / (pViewport->get_Right() - pViewport->get_Left());
			fVerticalSmall = (fVerticalSmall - (1.0f - pViewport->get_Top())) *
				1.0f / ((1.0f - pViewport->get_Top()) + pViewport->get_Bottom());
			fVerticalLarge = (fVerticalLarge - (1.0f - pViewport->get_Top())) *
				1.0f / ((1.0f - pViewport->get_Top()) + pViewport->get_Bottom());
		}

		m_pkDragFrustum->m_fNear = kOldFrustum.m_fNear;
		m_pkDragFrustum->m_fFar = kOldFrustum.m_fFar;
		m_pkDragFrustum->m_bOrtho = kOldFrustum.m_bOrtho;
		m_pkDragFrustum->m_fLeft = fHorizontalSmall * (fRight * 2) - fRight + 0.05f;
		m_pkDragFrustum->m_fRight = fHorizontalLarge * (fRight * 2) - fRight - 0.05f;
		m_pkDragFrustum->m_fTop = (fVerticalSmall * (fTop * 2) - fTop) * -1 - 0.05f;
		m_pkDragFrustum->m_fBottom = (fVerticalLarge * (fTop * 2) - fTop) * -1 + 0.05f;
		
		pkCamera->SetViewFrustum(*m_pkDragFrustum);
		pkCamera->Update(0.0f);

		// 플러스텀과 물체가 겹치는 것을 모두 찾아준다.
		unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
		ArrayList *pmEntities = new ArrayList;
		for (unsigned int i = 0 ; i < uiEntityCount ; i++)
		{
			MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];
			
			for(unsigned int j = 0; j < pkEntity->GetSceneRootPointerCount(); j++)
			{
				NiCullScene(pkCamera, pkEntity->GetSceneRootPointer(j), *m_pkDragCuller, *m_pkDragVA, true);
				if(m_pkDragVA->GetCount())
				{
					pmEntities->Add(pkEntity);
					break;
				}
			}			
		}

		if(pmEntities->Count)
		{
			MEntity *pkEntites[] = NiNew MEntity*[pmEntities->Count];
			for(int i = 0; i < pmEntities->Count; i++)
			{
				MEntity *pkEntity = dynamic_cast<MEntity *>(pmEntities->get_Item(i));
				if(pkEntity)
				{
					pkEntites[i] = pkEntity;
				}
			}

			SelectionService->AddEntitiesToSelection(pkEntites);
			NiDelete[] pkEntites;
		}

		pkCamera->SetViewFrustum(kOldFrustum);
		pkCamera->Update(0.0f);
	}
	// 물체 선택이 하나 일때의 처리
	else if (SelectionService->GetSelectedEntities()->Count == 1)
	{
		MEntity *pkEntity = SelectionService->GetSelectedEntities()[0];

		// Select ExtEntity
		if (MFramework::Instance->ExtEntity->SelectExtEntity(pkEntity))
		{
		}
	}

	m_bVisibleRectangleSelection = false;

	__super::MouseUp(eType, iX, iY);
}

void MSelectionInteractionMode::MouseMove(int iX, int iY)
{
	if (m_bVisibleRectangleSelection &&
		m_pkMouseClickPos &&
		m_pkMouseMovePos)
	{
		m_pkMouseMovePos->x =
			(float)iX / 
			(float)MFramework::Instance->ViewportManager->ActiveViewport->Width *
			1.0f;
		m_pkMouseMovePos->y =
			(float)iY / 
			(float)MFramework::Instance->ViewportManager->ActiveViewport->Height *
			1.0f;
	}
	else
	{
		__super::MouseMove(iX, iY);
	}
}
