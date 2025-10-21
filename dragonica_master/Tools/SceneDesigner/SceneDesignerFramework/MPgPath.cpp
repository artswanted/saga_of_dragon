//
// HandOver, 강정욱 2008.01.29
//
// 패스 Object를 관리 하거나.. 그 Object로 패스벽, PhysX벽 등을 만든다. (NIF를 만들어 냄)
// PhysX벽을 만들때에는 'SceneDesigner_PhysXConvert_Console.exe' 라는 프로그램을 이용해서 만든다.
// 따로 프로젝트가 있음.
// ./Tools/SceneDesigner_PhysXConvert_Console/
//
#include "SceneDesignerFrameworkPCH.h"
#include <vector>
#include "MFramework.h"
#include "IEntityPathService.h"
#include "MpgPath.h"

using namespace std; 
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgPath::MPgPath()
{
    m_pkTargetList = new ArrayList();
	m_pkListForUndo = new ArrayList();
	m_pkTargetListUp = new ArrayList();
	m_fHeight = 500.0f;

	m_pkWorldTransform = NiNew NiTransform();
	m_pkWorldTransform->MakeIdentity();
	m_bIsMakeLeft = true;
	m_bIsTypePath = true;
	m_bIsTypePhysX = false;
	m_iMeshGroup = 1;
	m_bMakeUp = false;
	m_bMakeSide = false;
}

MPgPath::~MPgPath()
{
	if(NULL != m_pkWorldTransform)
	{
		NiDelete m_pkWorldTransform;
		m_pkWorldTransform = NULL;
	}
	MDisposeRefObject(m_pkSideLineMarker);
}

ISelectionService* MPgPath::get_SelectionService()
{
    if (NULL == ms_pmSelectionService)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}

ICommandService* MPgPath::get_CommandService()
{
	if (NULL == ms_pmCommandService)
	{
		ms_pmCommandService = MGetService(ICommandService);
		MAssert(NULL != ms_pmCommandService, "Command Service not found");
	}
	return ms_pmCommandService;
}

void MPgPath::Initialize()
{
	m_fPathColorR = 0.5f;
	m_fPathColorG = 0.5f;
	m_fPathColorB = 1.0f;

	NiBool* pbMinorConnections;
	pbMinorConnections = NiAlloc(NiBool, 2);
	pbMinorConnections[0] = true;
	pbMinorConnections[1] = true;
	m_pkSideLineVerts = NiNew NiPoint3[2];
	m_pkSideLineMarker = NiNew NiLines(2, m_pkSideLineVerts, 0, 0, 0, NiGeometryData::NBT_METHOD_NONE, pbMinorConnections);
    MInitRefObject(m_pkSideLineMarker);
	m_pkSideLineMarker->UpdateProperties();
	m_pkSideLineMarker->UpdateEffects();
	m_pkSideLineMarker->Update(0.0f);

	m_pkMaterial = NiNew NiMaterialProperty();
	m_pkMaterial->SetAmbientColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
	m_pkMaterial->SetDiffuseColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
	m_pkMaterial->SetSpecularColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
	m_pkMaterial->SetEmittance(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
	m_pkMaterial->SetShineness(0.0f);
	m_pkMaterial->SetAlpha(1.0f);
	m_pkSideLineMarker->AttachProperty(m_pkMaterial);
	m_pkSideLineMarker->Update(0.0f);
	m_pkSideLineMarker->UpdateProperties();
	m_pkSideLineMarker->UpdateEffects();
	m_pkSideLineMarker->UpdateNodeBound();

	CreatePathDataConsole();
}
//---------------------------------------------------------------------------
void MPgPath::CreatePathDataConsole()
{
    if (!m_pkPathDataConsole)
    {
		if (MFramework::Instance)
		{
			m_pkPathDataConsole = NiNew NiScreenConsole();
			MInitRefObject(m_pkPathDataConsole);

			const char* pcPath = MStringToCharPointer(String::Concat(
				MFramework::Instance->AppStartupPath, "Data\\"));
			m_pkPathDataConsole->SetDefaultFontPath(pcPath);
			MFreeCharPointer(pcPath);
			m_pkPathDataConsole->SetFont(m_pkPathDataConsole->CreateConsoleFont());
			m_pkPathDataConsole->Enable(true);
		}
    }
}
//---------------------------------------------------------------------------
void MPgPath::Render(MRenderingContext* pmRenderingContext)
{
    NiEntityRenderingContext* pkContext = 
        pmRenderingContext->GetRenderingContext();
    NiCamera* pkCam = pkContext->m_pkCamera;
    //clear the z-buffer
    pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

	RenderPathDataConsole(pmRenderingContext);
}
//---------------------------------------------------------------------------
void MPgPath::RenderPathDataConsole(MRenderingContext* pmRenderingContext)
{
	CreatePathDataConsole();

	// Render screen console.
    if (NULL != m_pkPathDataConsole)
    {
		NiPoint3 kCenter(0,0,0);
		std::vector<NiPoint3>::iterator itr;
		// color setting
		{
			m_pkSideLineMarker->DetachProperty(m_pkMaterial);
			m_pkMaterial->SetAmbientColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
			m_pkMaterial->SetDiffuseColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
			m_pkMaterial->SetSpecularColor(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
			m_pkMaterial->SetEmittance(NiColor::NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));
			m_pkMaterial->SetShineness(0.0f);
			m_pkMaterial->SetAlpha(1.0f);
			m_pkSideLineMarker->AttachProperty(m_pkMaterial);
			m_pkSideLineMarker->UpdateProperties();
			m_pkSideLineMarker->UpdateEffects();
			m_pkSideLineMarker->Update(0.0f);
		}

		NiEntityRenderingContext* pkContext = 
			pmRenderingContext->GetRenderingContext();
		NiCamera* pkCam = pkContext->m_pkCamera;
		for (int i=0 ; i<m_pkTargetList->Count-1 ; ++i)
		{
			MEntity *pkData =
				dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));
			MEntity *pkNextData =
				dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i+1));

			// Translate
			NiPoint3 kPoint;
			pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			NiPoint3 kPoint_next;
			pkNextData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint_next);

			kCenter = (kPoint + kPoint_next) * 0.5f;
			m_pkSideLineMarker->SetTranslate(kCenter);
			m_pkSideLineVerts[0] = kPoint - kCenter;
			m_pkSideLineVerts[1] = kPoint_next - kCenter;
			m_pkSideLineMarker->Update(0.0f);
			NiDrawScene(pkCam, m_pkSideLineMarker, *pkContext->m_pkCullingProcess);

			// start point
			if (i==0)
			{
				m_pkSideLineMarker->SetTranslate(kPoint);
				m_pkSideLineVerts[0] = NiPoint3(100,0,0);
				m_pkSideLineVerts[1] = NiPoint3(-100,0,0);
				m_pkSideLineMarker->Update(0.0f);
				NiDrawScene(pkCam, m_pkSideLineMarker, *pkContext->m_pkCullingProcess);

				m_pkSideLineMarker->SetTranslate(kPoint);
				m_pkSideLineVerts[0] = NiPoint3(0,100,0);
				m_pkSideLineVerts[1] = NiPoint3(0,-100,0);
				m_pkSideLineMarker->Update(0.0f);
				NiDrawScene(pkCam, m_pkSideLineMarker, *pkContext->m_pkCullingProcess);
			}
		}

		for (int i=0 ; i<m_pkTargetListUp->Count-1 ; ++i)
		{
			MEntity *pkData =
				dynamic_cast<MEntity *>(m_pkTargetListUp->get_Item(i));
			MEntity *pkNextData =
				dynamic_cast<MEntity *>(m_pkTargetListUp->get_Item(i+1));

			// Translate
			NiPoint3 kPoint;
			pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			NiPoint3 kPoint_next;
			pkNextData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint_next);

			kCenter = (kPoint + kPoint_next) * 0.5f;
			m_pkSideLineMarker->SetTranslate(kCenter);
			m_pkSideLineVerts[0] = kPoint - kCenter;
			m_pkSideLineVerts[1] = kPoint_next - kCenter;
			m_pkSideLineMarker->Update(0.0f);
			NiDrawScene(pkCam, m_pkSideLineMarker, *pkContext->m_pkCullingProcess);
		}
		for (int i=0 ; i<m_pkTargetListUp->Count ; ++i)
		{
			MEntity *pkData =
				dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));
			MEntity *pkNextData =
				dynamic_cast<MEntity *>(m_pkTargetListUp->get_Item(i));

			// Translate
			NiPoint3 kPoint;
			pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			NiPoint3 kPoint_next;
			pkNextData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint_next);

			kCenter = (kPoint + kPoint_next) * 0.5f;
			m_pkSideLineMarker->SetTranslate(kCenter);
			m_pkSideLineVerts[0] = kPoint - kCenter;
			m_pkSideLineVerts[1] = kPoint_next - kCenter;
			m_pkSideLineMarker->Update(0.0f);
			NiDrawScene(pkCam, m_pkSideLineMarker, *pkContext->m_pkCullingProcess);
		}
	}
}

	
void MPgPath::InsertPath(MEntity *pkEntity)
{
	PathTarget* pkData = new PathTarget(pkEntity->GetNiEntityInterface());
	m_pkTargetList->Add(pkData);
}

void MPgPath::DeletePath(MEntity *pkEntity)
{
	PathTarget*		pkData = NULL;
	for (int iIndex=0 ; iIndex<m_pkTargetList->Count ; ++iIndex)
	{
		pkData =
			reinterpret_cast<PathTarget *>(m_pkTargetList->get_Item(iIndex));

		if (pkData->GetNiEntityInterface() == pkEntity->GetNiEntityInterface())
		{
			m_pkTargetList->Remove(pkData);
			//MFramework::Instance->Scene->RemoveEntity(pkData, false);

			if( (true==m_bMakeUp) || (true==m_bMakeSide) )
			{
				if(iIndex<m_pkTargetListUp->Count)
				{
					pkData = reinterpret_cast<PathTarget *>(m_pkTargetListUp->get_Item(iIndex));
					if(NULL != pkData)
					{
						m_pkTargetListUp->Remove(pkData);
						//MFramework::Instance->Scene->RemoveEntity(pkData, false);
					}
				}
			}
			return;
		}
	}

	if( (true==m_bMakeUp) || (true==m_bMakeSide) )
	{
		for (int iIndex=0 ; iIndex<m_pkTargetListUp->Count ; ++iIndex)
		{
			pkData =
				dynamic_cast<PathTarget *>(m_pkTargetListUp->get_Item(iIndex));

			if (pkData->GetNiEntityInterface() == pkEntity->GetNiEntityInterface())
			{
				m_pkTargetListUp->Remove(pkData);
				//MFramework::Instance->Scene->RemoveEntity(pkData, false);

				if(iIndex<m_pkTargetList->Count)
				{
					pkData = dynamic_cast<PathTarget *>(m_pkTargetList->get_Item(iIndex));
					if(NULL != pkData)
					{
						m_pkTargetList->Remove(pkData);
						//MFramework::Instance->Scene->RemoveEntity(pkData, false);
					}
				}				
				return;
			}
		}
	}
}

void MPgPath::MakeUp()
{
	m_bMakeUp = true;
	m_bMakeSide = false;
	
	if (NULL != m_pkTargetListUp)
	{
		for (int i=0 ; i<m_pkTargetListUp->Count ; i++)
		{
			PathTarget *pkData =
				dynamic_cast<PathTarget *>(m_pkTargetListUp->get_Item(i));

			if (pkData)
			{
				m_pkTargetListUp->Remove(pkData);
				MFramework::Instance->Scene->RemoveEntity(pkData, false);
				i -= 1;
			}
		}

		m_pkTargetListUp->Clear();
	}

	MScene* pkScene = MFramework::Instance->Scene;

	for (int i=0 ; i<m_pkTargetList->Count ; ++i)
	{
		MEntity *pkData =
			dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));

		// Translate
		NiPoint3 kPoint;
		pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);

		// Clone
		String* strCloneName = pkScene->GetUniqueEntityName(
			String::Concat(pkData->Name, " Up01"));
		MEntity* pkClone = pkData->Clone(strCloneName, true);

		// Set data
		kPoint.z += m_fHeight;
		pkClone->GetNiEntityInterface()->SetPropertyData("Translation", kPoint);
		MFramework::Instance->Scene->AddEntity(pkClone, false);
		PathTarget* pkDataClone = new PathTarget(pkClone->GetNiEntityInterface());
		m_pkTargetListUp->Add(pkDataClone);
	}
}

void MPgPath::MakeSide()
{
	m_bMakeSide = true;
	m_bMakeUp = false;
	if (NULL != m_pkTargetListUp)
	{
		for (int i=0 ; i<m_pkTargetListUp->Count ; i++)
		{
			PathTarget *pkData =
				dynamic_cast<PathTarget *>(m_pkTargetListUp->get_Item(i));

			if (pkData)
			{
				m_pkTargetListUp->Remove(pkData);
				MFramework::Instance->Scene->RemoveEntity(pkData, false);
				i -= 1;
			}
		}
		m_pkTargetListUp->Clear();
	}

	MScene* pkScene = MFramework::Instance->Scene;
	MEntity *pkData = 0;
	NiPoint3 kPoint;

	pkData = dynamic_cast<MEntity *>(m_pkTargetList->get_Item(0));
	pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
	NiPoint3 kStartPt = kPoint;
	pkData = dynamic_cast<MEntity *>(m_pkTargetList->get_Item(m_pkTargetList->Count-1));
	pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
	NiPoint3 kEndPt = kPoint;
	NiPoint3 kNormTarget = kEndPt - kStartPt;
	kNormTarget.Unitize();

	NiPoint3 kNormPt;
	if (true == m_bIsMakeLeft)
	{
		kNormPt = NiPoint3(0,0,1).UnitCross(kNormTarget);
	}
	else
	{
		kNormPt = kNormTarget.UnitCross(NiPoint3(0,0,1));
	}
	NiPoint3 kResultPt = kNormPt * m_fHeight;


	for (int i=0 ; i<m_pkTargetList->Count ; i++)
	{
		pkData = dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));

		// Translate
		pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);

		// Clone
		String* strCloneName = pkScene->GetUniqueEntityName(
			String::Concat(pkData->Name, " Side01"));
		MEntity* pkClone = pkData->Clone(strCloneName, true);

		// Set data
		kPoint += kResultPt;
		pkClone->GetNiEntityInterface()->SetPropertyData("Translation", kPoint);
		MFramework::Instance->Scene->AddEntity(pkClone, false);
		PathTarget* pkDataClone = new PathTarget(pkClone->GetNiEntityInterface());
		m_pkTargetListUp->Add(pkDataClone);
	}
}

//선택된 두개의 패스앵커에 대해 그 사이에 있는 패스 앵커들을 일직선으로 정렬 해줌.
//패스앵커가 두 개만 선택되어야 하고, 선택된 두 개의 패스가 연속이 아니어야 함.
void MPgPath::MakeStraight()
{
	if(true == IsMakeUp())
	{//올리기가 실행되어있으면 취소
		::MessageBoxA(0, "올리기가 실행되어있습니다.","오류",0);
		return;
	}

	if(0<m_pkListForUndo->Count)
	{//Undolist가 있으면 삭제한다.
		m_pkListForUndo->Clear();
		m_uiStartIndexForUndo = 0;
		m_uiCountForUndo = 0;
	}

	if(2 == SelectionService->GetSelectedEntities()->Count) //선택된 엔트리가 두개일때만 동작
	{
		int			iTwoPathAnchorSelected = 0;//최소한 두개의 앵커가 선택되어 있는가?
		int			iIndexOfPathlist=0, iIndexOfSelected=0;
		int			iFirstIndex=0, iSecondIndex=0;
		NiPoint3	kFirstPos(0.0f, 0.0f, 0.0f), kSecondPos(0.0f, 0.0f, 0.0f);
		MEntity		*pkFirstAnchor=NULL, *pkSecondAnchor=NULL;
		PathTarget*	pkTarget=NULL;
		
		pkFirstAnchor = SelectionService->GetSelectedEntities()[0];
		pkFirstAnchor->GetNiEntityInterface()->GetPropertyData("Translation", kFirstPos);
		pkSecondAnchor = SelectionService->GetSelectedEntities()[1];
		pkSecondAnchor->GetNiEntityInterface()->GetPropertyData("Translation", kSecondPos);
		
		for(iIndexOfPathlist=0;iIndexOfPathlist<m_pkTargetList->Count;++iIndexOfPathlist)
		{//패스앵커리스트를 이용해 선택된 두개의 패스앵커 위치, 인덱스, 앵커엔티티를 찾음
			NiPoint3 kPathPos(0.0f, 0.0f, 0.0f);				

			pkTarget = dynamic_cast<PathTarget*>(m_pkTargetList->get_Item(iIndexOfPathlist));
			pkTarget->GetNiEntityInterface()->GetPropertyData("Translation", kPathPos);

			if(kPathPos==kFirstPos)
			{
				++iTwoPathAnchorSelected;
				iFirstIndex = iIndexOfPathlist;
				//pkFirstAnchor = pkEntity1;
			}
			else if(kPathPos==kSecondPos)
			{
				++iTwoPathAnchorSelected;
				iSecondIndex = iIndexOfPathlist;
				//pkSecondAnchor = pkEntity2;
			}
			if(2==iTwoPathAnchorSelected)
			{
				break;
			}
		}
		
		bool bSelectFirstOrLast=false;
		bSelectFirstOrLast |= (System::Math::Min(iFirstIndex, iSecondIndex)==0);
		bSelectFirstOrLast |= (System::Math::Max(iFirstIndex+1, iSecondIndex+1)==m_pkTargetList->Count);

		/*
		String* strFirstIndex = System::Math::Min(iFirstIndex+1, iSecondIndex+1).ToString();
		strFirstIndex = String::Concat("첫번째 패스 : ",strFirstIndex);
		String* strSecondIndex = System::Math::Max(iFirstIndex+1, iSecondIndex+1).ToString();
		strSecondIndex = String::Concat(", 두번째 패스: ", strSecondIndex);
		String* strNumTargetAlignedPath = System::Math::Abs(iFirstIndex-iSecondIndex).ToString();
		strNumTargetAlignedPath = String::Concat("\n 정렬될 대상 패스 : ", strNumTargetAlignedPath);
		String* strStraightConfrimMessage = String::Concat(strFirstIndex, strSecondIndex, strNumTargetAlignedPath, "\n 실행하시겠습니까?");
*/
		//char const* pcMessage=MStringToCharPointer(strStraightConfrimMessage);
		if(true == bSelectFirstOrLast)
		{
			if (IDCANCEL == ::MessageBox(NULL, "첫번째 패스 또는 마지막 패스가 선택되어 있습니다.\n 정렬하시겠습니까?", "확인", MB_OKCANCEL|MB_ICONWARNING) )
			{
				//MFreeCharPointer(pcMessage);
				return;
			}
		}

		//직선화 Undo를 위한 인덱스와 카운트
		m_uiStartIndexForUndo = System::Math::Min(iFirstIndex, iSecondIndex);
		m_uiCountForUndo = System::Math::Abs(iFirstIndex - iSecondIndex);

		//MFreeCharPointer(pcMessage);
		////////////////////
		if((1<(System::Math::Abs(iSecondIndex-iFirstIndex))) && (2==iTwoPathAnchorSelected)) 
		{//선택된 두개의 패스가 연속되지 않고, 패스앵커가 2개 이상 선택되어야 함.
			int const	iLoopCount=System::Math::Abs(iSecondIndex-iFirstIndex);
			float const	fCalibRatio=1.0f/static_cast<float>(System::Math::Abs(iSecondIndex-iFirstIndex));
			CommandService->BeginUndoFrame("Cancel Straigt");
			for(int i=1;i<iLoopCount;++i)
			{//사이에 있는 패스앵커들의 위치를 재조정
				NiPoint3 kCalibPos(0.0f, 0.0f, 0.0f);

				//선택된 두개의 패스앵커위치 중 나중것에서 처음것을 빼야 함. 
				//벡터 뺄셈의 교환법칙은 성립하지 않음.
				if(iSecondIndex>iFirstIndex)
				{
					kCalibPos = (kSecondPos-kFirstPos) * (fCalibRatio*static_cast<float>(i));
					kCalibPos += kFirstPos;
				}
				else
				{
					kCalibPos = (kFirstPos-kSecondPos) * (fCalibRatio*static_cast<float>(i));
					kCalibPos += kSecondPos;
				}
				pkTarget = dynamic_cast<PathTarget*>(m_pkTargetList->get_Item(System::Math::Min(iFirstIndex,iSecondIndex)+i));
				//위치 조정하기전에 이전위치 백업
				NiPoint3 kOrgPos;
				pkTarget->GetNiEntityInterface()->GetPropertyData("Translation", kOrgPos);
				//m_pkListForUndo
				MPoint3 *kTargetPos = new MPoint3(kOrgPos);
				m_pkListForUndo->Add(kTargetPos);
				//새 위치로 조정
				pkTarget->GetNiEntityInterface()->SetPropertyData("Translation", kCalibPos);
			}
			CommandService->EndUndoFrame(true);
		}
	}
}

void MPgPath::CancelMakeStraight()
{
	if( (0<m_uiCountForUndo) && 
		(m_pkTargetList->Count >= m_uiStartIndexForUndo+m_uiCountForUndo) && 
		(m_uiCountForUndo-1 == m_pkListForUndo->Count) )
	{//만약 Undo할 갯수가 0보다 크고 타겟리스트의 수량을 벗어나지 않고 Undo리스트 갯수가 m_uiCountForUndo-1과 같으면
		PathTarget* pkTarget;
		MPoint3* pkCurPos;
		NiPoint3 kOrgPos;
		for(int i=1;i<m_uiCountForUndo;++i)
		{//타겟리스트의 모든 엔티티에 대해
			//이전에 백업해 둔 위치로 되돌림.
			pkCurPos = dynamic_cast<MPoint3*>(m_pkListForUndo->Item[i-1]);
			pkTarget = dynamic_cast<PathTarget*>(m_pkTargetList->Item[m_uiStartIndexForUndo + i]);
			kOrgPos.x = pkCurPos->X;
			kOrgPos.y = pkCurPos->Y;
			kOrgPos.z = pkCurPos->Z;
			pkTarget->GetNiEntityInterface()->SetPropertyData("Translation", kOrgPos);
		}
	}
	m_uiStartIndexForUndo = 0;
	m_uiCountForUndo = 0;
	m_pkListForUndo->Clear();
}

void MPgPath::CancelMakeUpSide()
{
	m_bMakeUp = false;
	m_bMakeSide = false;

	for (int i=0 ; i<m_pkTargetListUp->Count ; i++)
	{
		PathTarget *pkData =
			dynamic_cast<PathTarget *>(m_pkTargetListUp->get_Item(i));

		if (NULL != pkData)
		{
			if (pkData->m_pkLinkEntity &&
				m_pkTargetList->Contains(pkData->m_pkLinkEntity))
			{
				m_pkTargetList->Remove(pkData->m_pkLinkEntity);
				MFramework::Instance->Scene->RemoveEntity(pkData->m_pkLinkEntity, false);
			}

			m_pkTargetListUp->Remove(pkData);
			MFramework::Instance->Scene->RemoveEntity(pkData, false);
			i -= 1;
		}
	}
}

void MPgPath::ExtractDot()
{
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if (NULL == pkPalette)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", 0, 0);
		return;
	}
	String* strEntityName = "[General]Target.path_target";
	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);
	if (NULL == pkTemplate)
	{
		::MessageBox(0, "[General]Target.path_target 가 없습니다.", 0, 0);
		return;
	}
	MScene* pkScene = MFramework::Instance->Scene;

	String* strCloneName = NULL;
	IEntityPathService* pkPathService = MGetService(IEntityPathService);

	Clear();

	// 선택된 놈이 하나일경우에만 점을 추출하자.
	if (SelectionService->GetSelectedEntities()->Count == 1)
	{
		MEntity* pkEntity = SelectionService->GetSelectedEntities()[0];
		NiPoint3 kEntityPos(0.0f, 0.0f, 0.0f);
		pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
		if (pkEntity->get_PGProperty() == MEntity::ePGProperty::Path)
		{
			NiObjectList kGeometries;
			pkEntity->GetAllGeometries(kGeometries);
			if(false == kGeometries.IsEmpty())
			{
				NiTriStrips* kPath = NiDynamicCast(NiTriStrips, kGeometries.GetHead());
				if (false == kPath)
				{
					return;
				}

				if (kPath->GetVertexCount() < 3)
				{
					return;
				}
				// 첫 시작점의 위 아래를 정하자.
				int iUp = 0;
				if (kEntityPos.z + kPath->GetVertices()[1].z < kEntityPos.z + kPath->GetVertices()[0].z)
				{
					iUp = 1;
				}

				// 처리
				for (int i=0 ; i<kPath->GetVertexCount() ; i++)
				{
					if(i%2 == iUp)
					{
						strCloneName = pkScene->GetUniqueEntityName(
							String::Concat(pkPathService->GetSimpleName(pkTemplate->Name),
							" 01"));
						MEntity* pkNewEntity = pkTemplate->Clone(strCloneName, false);
						pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
							MFramework::Instance->ExternalAssetManager);
						pkNewEntity->MasterEntity = pkTemplate;
						pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos + kPath->GetVertices()[i]);

						MFramework::Instance->Scene->AddEntity(pkNewEntity, false);

						PathTarget* pkPathData = new PathTarget(pkNewEntity->GetNiEntityInterface());
						m_pkTargetList->Add(pkPathData);
					}
				}
				//for (int i=0 ; i<kPath->GetVertexCount() ; i++)
				//{
				//	strCloneName = pkScene->GetUniqueEntityName(
				//		String::Concat(pkPathService->GetSimpleName(pkTemplate->Name),
				//		" 01"));
				//	MEntity* pmNewEntity = pkTemplate->Clone(strCloneName, false);
				//	pmNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
				//		MFramework::Instance->ExternalAssetManager);
				//	pmNewEntity->MasterEntity = pkTemplate;
				//	pmNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kEntityPos + kPath->GetVertices()[i]);

				//    MFramework::Instance->Scene->AddEntity(pmNewEntity, false);

				//	PathTarget* pkPathData = new PathTarget(pmNewEntity->GetNiEntityInterface());
				//	if (i%2==iUp)
				//	{
				//		m_pkTargetList->Add(pkPathData);
				//	}
				//	else
				//	{
				//		m_pkTargetListUp->Add(pkPathData);
				//	}
				//}
			}
		}
	}
}

//선택한 패스 중간에 패스 삽입
void MPgPath::ModifyPath()
{
	MPalette *pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("General");
	if(NULL == pkPalette)
	{
		::MessageBox(0, "General 팔레트가 없습니다.", "오류", 0);
	}

	String* strEntityName = "[General]Target.path_target";
	MEntity* pkPathEntity = pkPalette->GetEntityByName(strEntityName);
	if(NULL == pkPathEntity)
	{
		::MessageBox(0, "[General]Target.path_target 가 없습니다.", 0, 0);
		return;
	}

	MScene* pkScene = MFramework::Instance->Scene;
	IEntityPathService* pkPathService = MGetService(IEntityPathService);

	//Clear();

	//선택된 엔트리가 한개일 경우만 동작
	if(SelectionService->GetSelectedEntities()->Count == 1)
	{
		MEntity* pkEntity = SelectionService->GetSelectedEntities()[0];
		NiPoint3 kEntityPos(0.0f, 0.0f, 0.0f), kMatchedPathPos(0.0f, 0.0f, 0.0f);
		pkEntity->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos); //선택한 오브젝트의 위치를 구해옴.

		unsigned int const	iPathlistLength = m_pkTargetList->Count;
		unsigned int		iIndexOfMatched = 0;
		
		
		for(iIndexOfMatched=0 ; iIndexOfMatched<iPathlistLength ; ++iIndexOfMatched)//Off by one error 검증 완료
		{
			PathTarget* pkPathtarget = dynamic_cast<PathTarget*>(m_pkTargetList->get_Item(iIndexOfMatched));
			pkPathtarget->GetNiEntityInterface()->GetPropertyData("Translation", kMatchedPathPos);

			if( (kEntityPos==kMatchedPathPos) && (iIndexOfMatched < iPathlistLength-1) ) //매치된 위치를 찾았고 그 위치가 마지막이 아니면.
			{ //찾은 위치 다음에 패스와 패스앵커 추가
				//패스 추가할 지점 계산(찾은위치와 다음위치 선형보간)
				NiPoint3	kMatchedPathPosNext(0.0f, 0.0f, 0.0f), kNewPathPos(0.0f, 0.0f, 0.0f);
				float		fOrgScale;
				PathTarget*	pkPathNexttarget = dynamic_cast<PathTarget*>(m_pkTargetList->get_Item(iIndexOfMatched+1));
				pkPathNexttarget->GetNiEntityInterface()->GetPropertyData("Translation", kMatchedPathPosNext);
				pkPathNexttarget->GetNiEntityInterface()->GetPropertyData("Scale", fOrgScale);
				kNewPathPos = (kMatchedPathPos + kMatchedPathPosNext) * 0.5f;
				
				//먼저 패스앵커 생성
				String* pstrNewAnchoreName = pkScene->GetUniqueEntityName(
					String::Concat(pkPathService->GetSimpleName(pkPathEntity->Name), " 01"));
				MEntity* pkNewAnchorEntity = pkPathEntity->Clone(pstrNewAnchoreName, false);
				pkNewAnchorEntity->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
				pkNewAnchorEntity->MasterEntity = pkPathEntity;
				pkNewAnchorEntity->GetNiEntityInterface()->SetPropertyData("Translation", kNewPathPos);
				pkNewAnchorEntity->GetNiEntityInterface()->SetPropertyData("Scale", fOrgScale);
				MFramework::Instance->Scene->AddEntity(pkNewAnchorEntity, false);

				PathTarget* pkNewTarget = new PathTarget(pkNewAnchorEntity->GetNiEntityInterface());
				m_pkTargetList->Insert(iIndexOfMatched+1, pkNewTarget);//iset_Item(iIndexOfMatched,pkNewTarget);

				if( (true==m_bMakeUp) || (true==m_bMakeSide) )
				{
					//'올리기'나 '옆으로'가 수행된 경우 윗 패스 리스트에 추가하기전 추가할 지점을 새로 구해야 함.
					//원본패스에 추가할 때 처럼 두 지점 구해서 선형보간
					NiPoint3 kNewPathPosUp(0.0f, 0.0f, 0.0f), kCurrentUp(0.0f, 0.0f, 0.0f), kCurrentNextUp(0.0f, 0.0f, 0.0f);
					PathTarget* pkCurrentUp = dynamic_cast<PathTarget*>(m_pkTargetListUp->get_Item(iIndexOfMatched));
					pkCurrentUp->GetNiEntityInterface()->GetPropertyData("Translation", kCurrentUp);
					PathTarget* pkCurrentNextUp = dynamic_cast<PathTarget*>(m_pkTargetListUp->get_Item(iIndexOfMatched+1));
					pkCurrentNextUp->GetNiEntityInterface()->GetPropertyData("Translation", kCurrentNextUp);
					kNewPathPosUp = (kCurrentUp + kCurrentNextUp)*0.5f;					

					//윗 패스에 추가
					String* pstrNewAnchoreNameUp = pkScene->GetUniqueEntityName(
						String::Concat(pkPathService->GetSimpleName(pkPathEntity->Name), " Up01"));
					MEntity* pkNewAnchorEntityUp = pkPathEntity->Clone(pstrNewAnchoreNameUp, false);
					pkNewAnchorEntityUp->Update(MFramework::Instance->TimeManager->CurrentTime, MFramework::Instance->ExternalAssetManager);
					pkNewAnchorEntityUp->MasterEntity = pkPathEntity;
					pkNewAnchorEntityUp->GetNiEntityInterface()->SetPropertyData("Translation", kNewPathPosUp);
					pkNewAnchorEntityUp->GetNiEntityInterface()->SetPropertyData("Scale", fOrgScale);
					MFramework::Instance->Scene->AddEntity(pkNewAnchorEntityUp, false);

					PathTarget* pkNewTargetUp = new PathTarget(pkNewAnchorEntityUp->GetNiEntityInterface());
					m_pkTargetListUp->Insert(iIndexOfMatched+1, pkNewTargetUp);
				}

				break;
			}
		}
	}
}

void MPgPath::Clear()
{
	m_bMakeUp = false;
	m_bMakeSide = false;

	for (int i=0 ; i<m_pkTargetList->Count ; i++)
	{
		PathTarget *pkData =
			dynamic_cast<PathTarget *>(m_pkTargetList->get_Item(i));

		if (NULL != pkData)
		{
			if (pkData->m_pkLinkEntity &&
				m_pkTargetListUp->Contains(pkData->m_pkLinkEntity))
			{
				m_pkTargetListUp->Remove(pkData->m_pkLinkEntity);
				MFramework::Instance->Scene->RemoveEntity(pkData->m_pkLinkEntity, false);
			}

			m_pkTargetList->Remove(pkData);
			MFramework::Instance->Scene->RemoveEntity(pkData, false);
			i -= 1;
		}
	}

	for (int i=0 ; i<m_pkTargetListUp->Count ; i++)
	{
		PathTarget *pkData =
			dynamic_cast<PathTarget *>(m_pkTargetListUp->get_Item(i));


		if (NULL != pkData)
		{
			if (pkData->m_pkLinkEntity &&
				m_pkTargetList->Contains(pkData->m_pkLinkEntity))
			{
				m_pkTargetList->Remove(pkData->m_pkLinkEntity);
				MFramework::Instance->Scene->RemoveEntity(pkData->m_pkLinkEntity, false);
			}

			m_pkTargetListUp->Remove(pkData);
			MFramework::Instance->Scene->RemoveEntity(pkData, false);
			i -= 1;
		}
	}
}

void MPgPath::MakeWall(String *pkFilePath, String *pkSelectPaletteName)
{
    if (m_pkTargetList->Count < 2 ||
		m_pkTargetListUp->Count < 2)
	{
		return;
	}

	if (true == m_bMakeSide)
	{
		m_bIsMakeLeft = !m_bIsMakeLeft;
	}

	unsigned short usTrianglecount=0;
	usTrianglecount = (m_pkTargetList->Count-1) * 2;
	
	unsigned short *pusStripLengths = NiAlloc(unsigned short, 1);
	*pusStripLengths = m_pkTargetList->Count + m_pkTargetListUp->Count;
	unsigned short *pusStripLists = NiAlloc(unsigned short, *pusStripLengths);

	// Strip Data 만들기.
	NiNode *pkNode = NiNew NiNode();
	pkNode->SetName("Scene Root");
	int iLastSlash = pkFilePath->LastIndexOf("\\");
	int iLastDot = pkFilePath->LastIndexOf(".");
	String *pkFilename = pkFilePath->Substring(iLastSlash+1, iLastDot-(iLastSlash+1));
	
	NiNode *pkPathNode = NiNew NiNode();
	String *pkNodeName = "paths_";
	String *pkResultName = "";
	pkResultName = pkNodeName->Concat(pkNodeName, pkFilename);
    const char* pcPathName = MStringToCharPointer(pkResultName);
	pkPathNode->SetName(pcPathName);

	pkNode->AttachChild(pkPathNode);

	Guid kGuid = Guid::NewGuid();
	String *pkGuid = kGuid.ToString();

	NiMaterialProperty* pkPathMaterial;
	pkPathMaterial = NiNew NiMaterialProperty();
	pkPathMaterial->SetEmittance(NiColor(m_fPathColorR, m_fPathColorG, m_fPathColorB));

	int iStripIncCount = 0;
	int iStripIdx = 0;
	{
		iStripIncCount = 0;
		NiPoint3 kCenterPt = NiPoint3(0,0,0);
		NiPoint3 kTotalPt = NiPoint3(0,0,0);

		// Calculate Center
		for (int i=0 ; i<m_pkTargetList->Count ; i++)
		{
			MEntity *pkData = dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));
			MEntity *pkNextData = dynamic_cast<MEntity *>(m_pkTargetListUp->get_Item(i));
			NiPoint3 kPoint;
			pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			NiPoint3 kPoint_next;
			pkNextData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint_next);

			kTotalPt += (kPoint + kPoint_next);
		}
		kCenterPt = kTotalPt / (m_pkTargetList->Count * 2.0f);
		m_pkWorldTransform->m_Translate = kCenterPt;

		// Get Points
		NiPoint3 *pkPoint = NiNew NiPoint3[*pusStripLengths];
		for (int i=0 ; i<m_pkTargetListUp->Count ; i++)
		{
			MEntity *pkData =
				dynamic_cast<MEntity *>(m_pkTargetList->get_Item(i));
			MEntity *pkNextData =
				dynamic_cast<MEntity *>(m_pkTargetListUp->get_Item(i));

			NiPoint3 kPoint;
			pkData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint);
			NiPoint3 kPoint_next;
			pkNextData->GetNiEntityInterface()->GetPropertyData("Translation", kPoint_next);

			if (m_bIsMakeLeft)
			{
				pkPoint[iStripIncCount] = kPoint - kCenterPt;
				pusStripLists[iStripIncCount++] = iStripIncCount;
				pkPoint[iStripIncCount] = kPoint_next - kCenterPt;
				pusStripLists[iStripIncCount++] = iStripIncCount;
			}
			else
			{
				pkPoint[iStripIncCount] = kPoint_next - kCenterPt;
				pusStripLists[iStripIncCount++] = iStripIncCount;
				pkPoint[iStripIncCount] = kPoint - kCenterPt;
				pusStripLists[iStripIncCount++] = iStripIncCount;
			}
		}

		// Save
		NiTriStrips *pkTri = NiNew NiTriStrips(
			*pusStripLengths, pkPoint, NULL, NULL,
			NULL, 0, NiGeometryData::NBT_METHOD_NONE,
			usTrianglecount, 1, pusStripLengths, pusStripLists);
		String *pkPartName = "Path_";
		pkPartName = pkPartName->Concat(pkPartName, pkGuid);
		const char* pcPartName = MStringToCharPointer(pkPartName);
		pkTri->SetName(pcPartName);
		pkTri->AttachProperty(pkPathMaterial);
		pkTri->UpdateProperties();
		pkPathNode->AttachChild(pkTri);

		pkNode->Update(0.0f);
	}

	NiStream kStream;
	kStream.InsertObject(pkNode);

	const char* szFileName = MStringToCharPointer(pkFilePath->ToString());
	kStream.Save(szFileName);

	// PhysX가 체크되어 있다면 외부 어플리 실행후 인자로 파일패스 넘겨줘서
	// 그 프로그램에서 방금 만든 NIF파일에 PhysX를 넣어준다.
	if (m_bIsTypePhysX)
	{
		char path[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);
		if (strlen(path)==0)
		{
			return;
		}
		strrchr( path, '\\' )[1] = 0;

		String* strParameter = pkFilePath->ToString();
		int iGroup = m_iMeshGroup;
		char szGroup[256];
		_itoa_s(iGroup, szGroup, 10);
		strParameter = strParameter->Concat(strParameter, " ");
		strParameter = strParameter->Concat(strParameter, szGroup);
		const char* szParameter = MStringToCharPointer(strParameter);

		TCHAR Buffer[256];
		GetCurrentDirectory(256, Buffer);
		HINSTANCE kIns = ::ShellExecute(NULL, "open", "SceneDesigner_PhysXConvert_Console.exe", szParameter, path, SW_SHOW);

		if (!kIns)
		{
			::MessageBox(0, "SceneDesigner_PhysXConvert_Console.exe wasn't have exist", "Warning", 0);
		}
	}

	LoadEntityToPalette(szFileName, pkSelectPaletteName);

	if (m_bMakeSide)
	{
		m_bIsMakeLeft = !m_bIsMakeLeft;
	}
}

String* AppendPaletteNameAndCategory(String *entityName, MPalette *palette, String *category)
{
    String *retVal = entityName;
	if (category != 0)
    {
		retVal = String::Format("{0}.{1}", category, retVal);
    }
	retVal = String::Format("[{0}]{1}", palette->Name, retVal);

	return retVal;
}

void MPgPath::LoadEntityToPalette(String *pkFilename, String *pkSelectPaletteName)
{
    MEntity *pmNewTemplate = 0;
	pmNewTemplate = MPalette::CreateTemplateFromFile(pkFilename);
	if (!pmNewTemplate)
	{
		return;
	}
	if (m_bIsTypePath)
	{
		pmNewTemplate->SetPGProperty(MEntity::ePGProperty::Path);
	}

	// 팔레트에 추가.
	if (pkSelectPaletteName != 0 && !pkSelectPaletteName->Equals("_None_"))
	{
		MPalette *pmPalette = MFramework::Instance->PaletteManager->GetPaletteByName(pkSelectPaletteName);
		if (pmPalette)
		{
			String *fullTemplateName = AppendPaletteNameAndCategory(pmNewTemplate->Name, pmPalette, 0);
			pmNewTemplate->Name = fullTemplateName;
			pmPalette->AddEntity(pmNewTemplate, String::Empty, true);
		}
	}

	// Scene에 생성
	// 이름 유일하게 만듬.
	String* strPathName = "Path";
	MScene* pkScene = MFramework::Instance->Scene;
	IEntityPathService* pkPathService = MGetService(IEntityPathService);
	strPathName = pkScene->GetUniqueEntityName(String::Concat(pkPathService->GetSimpleName(strPathName), " 01"));
	MEntity *pkNewEntity = pmNewTemplate->Clone(strPathName, true);
	if (m_bIsTypePath)
	{
		pkNewEntity->SetPGProperty(MEntity::Path);
	}
	else if (m_bIsTypePhysX)
	{
		pkNewEntity->SetPGProperty(MEntity::PhysX);
	}
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", m_pkWorldTransform->m_Translate);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Rotation", m_pkWorldTransform->m_Rotate);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Scale", m_pkWorldTransform->m_fScale);
	MFramework::Instance->Scene->AddEntity(pkNewEntity, false);
}

void MPgPath::SetAnchorScale(float fScale)
{
	for(int iIndex=0; m_pkTargetList->Count>iIndex; ++iIndex)
	{
		MEntity* pkEntity = dynamic_cast<MEntity*>(m_pkTargetList->Item[iIndex]);
		pkEntity->GetNiEntityInterface()->SetPropertyData("Scale", fScale);
	}
}