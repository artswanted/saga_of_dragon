//
// HandOver, 강정욱 2008.01.29
//
// 몬스터 관련을 관리, 표시(이름 등)를 해준다.
// DB로 쿼리 날리는 것은 UI쪽에서 한다.
// 이 클래스는 단순히 Entity 관련을 관리.
// 몬스터 백은 관리 하지 않기 때문에 ifdef로 막아 두었음.
//
// UI : MonsterPanel.cs(Panel), MonsterPanelSetting.cs(Config)
//
#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "IEntityPathService.h"
#include "MPgMonster.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

MPgMonster::MPgMonster(void)
{
	m_pkMonsterDataConsole = NULL;
	m_pkWaitData = NULL;
	m_bIsActiveText = true;
	m_kGroupNum = MonsterBagControl::NONE;
}

MPgMonster::~MPgMonster(void)
{
	NiDelete m_pkHighlightColor;
    MDisposeRefObject(m_pkMonsterDataConsole);
}
//---------------------------------------------------------------------------
void MPgMonster::Initialize()
{
	m_iEntityCount = 0;
	m_bAddedMonsterData = false;

#ifdef PG_USE_MONSTERBAG
	m_pmMonsterBagList = new ArrayList;
#endif
	m_pmSettedMonsterList = new ArrayList;
	m_pmDeletedMonsterList = new ArrayList;
	m_pmMonsterBagControl = NULL;

	CreateMonsterDataConsole();
}
//---------------------------------------------------------------------------
void MPgMonster::Clear()
{
	for(int iIndex=0;m_pmSettedMonsterList->Count>iIndex; ++iIndex)
	{
		MEntity* pkEntity = dynamic_cast<SettedMonsterData*>(m_pmSettedMonsterList->Item[iIndex])->m_pkEntity;
		if(NULL != pkEntity)
		{
			MFramework::Instance->Scene->RemoveEntity(pkEntity, false);
		}
	}
	m_pmSettedMonsterList->Clear();
	m_pmDeletedMonsterList->Clear();
}
//---------------------------------------------------------------------------
void MPgMonster::AddMonsterTemplate(int iMonsterBag, String* pkTemplateName)
{
	if(NULL == GetMonsterTemplate(iMonsterBag))
	{
		MonsterTemplate* pkData = new MonsterTemplate;

		pkData->MonsterBagNo = iMonsterBag;
		pkData->MonsterTemplateName = pkTemplateName;

		m_pmMonsterTemplate->Add(pkData);
	}
}
//---------------------------------------------------------------------------
String* MPgMonster::GetMonsterTemplate(int iMonsterBag)
{
	for(int iIndex=0; m_pmMonsterTemplate->Count > iIndex; ++iIndex)
	{
		MonsterTemplate* pkData = dynamic_cast<MonsterTemplate*>(m_pmMonsterTemplate->Item[iIndex]);
		if(NULL != pkData)
		{
			if((pkData->MonsterBagNo==iIndex))
			{
				return pkData->MonsterTemplateName;
			}
		}
	}

	return NULL;
}
//---------------------------------------------------------------------------
void MPgMonster::CreateMonsterDataConsole()
{
    if (!m_pkMonsterDataConsole)
    {
		if (MFramework::Instance)
		{
			m_pkMonsterDataConsole = NiNew NiScreenConsole();
			MInitRefObject(m_pkMonsterDataConsole);

			const char* pcPath = MStringToCharPointer(String::Concat(
				MFramework::Instance->AppStartupPath, "Data\\"));
			m_pkMonsterDataConsole->SetDefaultFontPath(pcPath);
			MFreeCharPointer(pcPath);

			m_pkMonsterDataConsole->SetFont(m_pkMonsterDataConsole->CreateConsoleFont());
			m_pkMonsterDataConsole->Enable(true);
		}
    }
}
bool MPgMonster::IsHidden(const int iParentBagNo)
{
	if (!m_pmMonsterBagControl)
	{
		return true;
	}

	Object* pmKey = __box(iParentBagNo);
	if (!m_pmMonsterBagControl->ContainsKey(pmKey))
	{
		return true;
	}

	MonsterBagControl *pkBagControlData = dynamic_cast<MonsterBagControl *>(m_pmMonsterBagControl->get_Item(pmKey));

	bool bHidden = (MonsterBagControl::NONE != m_kGroupNum);
	if(MonsterBagControl::DEFAULT == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iDefaultBagNo>0) ? false : true);
	}
	if(MonsterBagControl::EASY == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iEasyBagNo>0) ? false : true);
	}
	if(MonsterBagControl::NORMAL == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iNormalBagNo>0) ? false : true);
	}
	if(MonsterBagControl::HARD == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iHardBagNo>0) ? false : true);
	}
	if(MonsterBagControl::ULTRA == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iUltraBagNo>0) ? false : true);
	}
	if(MonsterBagControl::LEVEL05 == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iLevel05Bag>0) ? false : true);
	}
	if(MonsterBagControl::LEVEL06 == m_kGroupNum)
	{
		bHidden = ((pkBagControlData->m_iLevel06Bag>0) ? false : true);
	}

	return bHidden;
}
//---------------------------------------------------------------------------
void MPgMonster::Render(MRenderingContext* pmRenderingContext)
{
    NiEntityRenderingContext* pkContext = 
        pmRenderingContext->GetRenderingContext();
    NiCamera* pkCam = pkContext->m_pkCamera;
    //clear the z-buffer
    pkContext->m_pkRenderer->ClearBuffer(NULL, NiRenderer::CLEAR_ZBUFFER);

	NiMatrix3 kRot;
	if(MFramework::Instance->Scene->EntityCount > 0)
	{
		(MFramework::Instance->Scene->GetEntities()[0])->GetNiEntityInterface()->GetPropertyData("Rotation", kRot);
		float fAngle=0.0f, fx, fy, fz;
		kRot.ExtractAngleAndAxis(fAngle, fx, fy, fz);
	}

	if (m_bIsActiveText)
	{
		for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
		{
			SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));
			if (!IsHidden(pkData->m_iMonParentBagNo))
			{
				RenderMonsterDataConsole(pmRenderingContext, pkData);
			}
		}
	}
}
//---------------------------------------------------------------------------
void MPgMonster::RenderMonsterDataConsole(MRenderingContext* pmRenderingContext, SettedMonsterData *pkData)
{
    NiRenderer* pkRenderer = pmRenderingContext
        ->GetRenderingContext()->m_pkRenderer;

	CreateMonsterDataConsole();
	// Render screen console.
    if (m_pkMonsterDataConsole)
    {
		MEntity *pkEntity = 0;
		if (!pkData->m_pkEntity)
		{
			unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
			for (unsigned int i = 0 ; i < uiEntityCount ; i++)
			{
				MEntity *pkTemp = MFramework::Instance->Scene->GetEntities()[i];
				if (pkTemp->TemplateID == pkData->m_kGuid)
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
		if (!pkViewport)
			return;

		fWidth = (float)pkViewport->Width;
		fHeight = (float)pkViewport->Height;
		m_pkMonsterDataConsole->SetDimensions(NiPoint2(fWidth, fHeight));
		//m_pkMonsterDataConsole->setp

		MEntity* pmCameraEntity = pkViewport->get_CameraEntity();
	    NiAVObject* pkCameraObject = pmCameraEntity->GetSceneRootPointer(0);
		NiCamera* pkCamera = NiDynamicCast(NiCamera, pkCameraObject);
		if (!pkCamera)
			return;

		NiEntityPropertyInterface* pkEntityProp = 
			pkEntity->GetNiEntityInterface();
		NiPoint3 kPoint;
		pkEntityProp->GetPropertyData("Translation", kPoint);

		//kPoint.z = kPoint.z;

		float fX, fY;
		//kPoint.z = kPoint.z + 100;
		pkCamera->WorldPtToScreenPt(kPoint, fX, fY);
		float fCenterX = (fX) * fWidth;
		float fCenterY = (1.0f-fY) * fHeight;
		if (fCenterX > fWidth - fWidth*0.05f || fCenterX < fWidth*0.05f ||
			fCenterY > fHeight - fHeight*0.03f || fCenterY < fHeight*0.03f)
			return;

		String *strConsole = pkData->m_iMonParentBagNo.ToString();
		strConsole = strConsole->Insert(strConsole->Length, "_G(");
		strConsole = strConsole->Insert(strConsole->Length, pkData->m_iPointGroup.ToString());
		strConsole = strConsole->Insert(strConsole->Length, ")_");
		strConsole = strConsole->Insert(strConsole->Length, pkData->m_iRegenPeriod.ToString());
		float fDx = (float)strConsole->Length * (float)m_pkMonsterDataConsole->GetFont()->m_uiCharWidth * 0.5f;
		float fDy = (float)m_pkMonsterDataConsole->GetFont()->m_uiCharHeight * 0.5f;
		float fRenderX = fCenterX - fDx;
		float fRenderY = fCenterY + fDy;

		m_pkMonsterDataConsole->SetOrigin(NiPoint2(fRenderX, fRenderY));
		m_pkMonsterDataConsole->SetCamera(pkCamera);
		const char* pcMonsterData = MStringToCharPointer(strConsole);
		m_pkMonsterDataConsole->SetLine(pcMonsterData, 0);
		MFreeCharPointer(pcMonsterData);
		m_pkMonsterDataConsole->RecreateText();


		//////////////////////////////////////////////////////////////////////////////
        NiScreenTexture* pkConsoleTexture = m_pkMonsterDataConsole
            ->GetActiveScreenTexture();
        if (pkConsoleTexture)
        {
            pkConsoleTexture->Draw(pkRenderer);
        }
    }
}


#ifdef PG_USE_MONSTERBAG
// Monster Bag Data
//---------------------------------------------------------------------------
void MPgMonster::AddMonsterBagData(int iMonsterBagNo, String *strMonsterBagMemo, int aiMonsterNo __gc[], int aiMonsterRate __gc[])
{
	int iBagNo = iMonsterBagNo;
	if (iMonsterBagNo == -1)
	{
		for (int i=0 ; i<m_pmMonsterBagList->Count ; i++)
		{
			MonsterBagData *pkData = dynamic_cast<MonsterBagData *>(m_pmMonsterBagList->get_Item(i));

			if (pkData->m_iMonsterBagNo > iBagNo)
			{
				iBagNo = pkData->m_iMonsterBagNo;
			}
		}
		iBagNo += 1;
	}

	MonsterBagData *pkData = new MonsterBagData;
	pkData->Initialize(iBagNo, strMonsterBagMemo, aiMonsterNo, aiMonsterRate);
	m_pmMonsterBagList->Add(pkData);
}
//---------------------------------------------------------------------------
void MPgMonster::DeleteMonsterBagData(int iMonsterBagNo)
{
	for (int i=0 ; i<m_pmMonsterBagList->Count ; i++)
	{
		MonsterBagData *pkData = dynamic_cast<MonsterBagData *>(m_pmMonsterBagList->get_Item(i));

		if (pkData->m_iMonsterBagNo == iMonsterBagNo)
		{
			m_pmMonsterBagList->RemoveAt(i);
			break;
		}
	}
}
void MPgMonster::ClearMonsterBagData()
{
	m_pmMonsterBagList->Clear();
}
//---------------------------------------------------------------------------
void MPgMonster::ModifyMonsterBagData(int iArrayIdx, int iMonsterBagNo, String *strMonsterBagMemo, int aiMonsterNo __gc[], int aiMonsterRate __gc[])
{
	MonsterBagData *pkData = dynamic_cast<MonsterBagData *>(m_pmMonsterBagList->get_Item(iArrayIdx));

	pkData->m_iMonsterBagNo = iMonsterBagNo;
	pkData->m_pkMonsterBagMemo = strMonsterBagMemo;
	pkData->m_aiMonsterNo = aiMonsterNo;
	pkData->m_aiMonsterRate = aiMonsterRate;
}
//---------------------------------------------------------------------------
MonsterBagData* MPgMonster::get_GetMonsterBagData(int iMonsterBagNo)
{
	for (int i=0 ; i<m_pmMonsterBagList->Count ; i++)
	{
		MonsterBagData *pkData = dynamic_cast<MonsterBagData *>(m_pmMonsterBagList->get_Item(i));

		if (pkData->m_iMonsterBagNo == iMonsterBagNo)
		{
			return pkData;
		}
	}

	return 0;
}
#endif 

// Setted Monster Data
//---------------------------------------------------------------------------
MEntity* MPgMonster::AddSettedMonsterData(Guid kGuid, String *strMemo, int iMapNo, int iMonParentBagNo, int iPointGroup,
									  int iRegenPeriod, float fPosX, float fPosY, float fPosZ, int iRotAxZ, int iMoveRange, String* strTemplateName)
{
	MPalette *pkPalette = NULL;
	String* strEntityName = NULL;
	if( MFramework::Instance->PaletteManager->IsAlteredMonsterShape() )
	{
		pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("Default");

		strEntityName = "[Default]monster_target";
	}
	else
	{
		pkPalette = MFramework::Instance->PaletteManager->GetPaletteByName("Monster");

		strEntityName = "[Monster]";
		strEntityName = String::Concat(strEntityName, strTemplateName);
	}

	if (pkPalette == NULL)
	{
		::MessageBox(0, "Default or Monster 팔레트가 없습니다.", 0, 0);
		return 0;
	}

	MEntity* pkTemplate = pkPalette->GetEntityByName(strEntityName);

    if (pkTemplate == NULL)
	{
		String* strError = strEntityName;
		String* strErrorPostfix = "가 없습니다";
		strError = String::Concat(strError, strErrorPostfix);
		const char* pcError = MStringToCharPointer(strError);
		::MessageBox(0, pcError, 0, 0);
		MFreeCharPointer(pcError);
	    return 0;
	}
    MScene* pmScene = MFramework::Instance->Scene;

	IEntityPathService* pmPathService = MGetService(IEntityPathService);
	String* strMonName = "[Monster]";
	strMonName = strMonName->Concat(strMonName, strMemo);
	String* strCloneName = pmScene->GetUniqueEntityName(String::Concat(strMonName, " 01"));

	MEntity* pkNewEntity = pkTemplate->Clone(strCloneName, false);
	pkNewEntity->TemplateID = kGuid;
    pkNewEntity->Update(MFramework::Instance->TimeManager->CurrentTime,
        MFramework::Instance->ExternalAssetManager);
	pkNewEntity->MasterEntity = pkTemplate;

	//변환 적용: 위치/회전각
	NiPoint3 kPoint = NiPoint3(fPosX, fPosY, fPosZ);
	pkNewEntity->GetNiEntityInterface()->SetPropertyData("Translation", kPoint);	

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
	
	////////////////////////////////////////////////////////////////////////////////
	SettedMonsterData *pkData = new SettedMonsterData;
	pkData->Initialize(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, fPosY, fPosZ, static_cast<float>(iRotAxZ), iMoveRange);
	pkData->m_pkEntity = pkNewEntity;

	m_pmSettedMonsterList->Add(pkData);

	m_iEntityCount += 1;

	return pkNewEntity;
}
//---------------------------------------------------------------------------
void MPgMonster::DeleteSettedMonsterData(Guid kGuid)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_kGuid == kGuid)
		{
			m_pmDeletedMonsterList->Add(pkData);
			m_pmSettedMonsterList->RemoveAt(i);
			break;
		}
	}
}
//---------------------------------------------------------------------------
void MPgMonster::ClearSettedMonsterData()
{
	unsigned int uiEntityCount = MFramework::Instance->Scene->get_EntityCount();
	for (unsigned int i = 0 ; i < uiEntityCount ; i++)
	{
		MEntity *pkEntity = MFramework::Instance->Scene->GetEntities()[i];

		if (IsMonsterData(pkEntity->TemplateID.ToString()))
		{
			MFramework::Instance->Scene->RemoveEntity(pkEntity,false);
			uiEntityCount -= 1;
			i -= 1;
		}
	}
	m_pmSettedMonsterList->RemoveRange(0, m_pmSettedMonsterList->Count);
	m_iEntityCount = 0;
}
//---------------------------------------------------------------------------
void MPgMonster::StackSettedMonsterData(String *strMemo, int iMapNo, int iMonParentBagNo, int iPointGroup, int iRegenPeriod, int iMoveRange)
{
	if (m_pkWaitData)
	{
		delete m_pkWaitData;
		m_pkWaitData = NULL;
	}

	m_pkWaitData = new SettedMonsterData;
	m_pkWaitData->m_strMemo = strMemo;
	m_pkWaitData->m_iMapNo = iMapNo;
	m_pkWaitData->m_iMonParentBagNo = iMonParentBagNo;
	m_pkWaitData->m_iPointGroup = iPointGroup;
	m_pkWaitData->m_iRegenPeriod = iRegenPeriod;
	m_pkWaitData->m_iMoveRange = iMoveRange;
}
//---------------------------------------------------------------------------
void MPgMonster::StackSettedMonsterData_RegenPeriod(int iRegenPeriod)
{
	if (!m_pkWaitData)
		return;

	m_pkWaitData->m_iRegenPeriod = iRegenPeriod;
}
//---------------------------------------------------------------------------
void MPgMonster::CompleateStackedMonsterData(Guid kGuid, float fPosX, float fPosY, float fPosZ, MEntity* pkEntity)
{
	m_pkWaitData->m_kGuid = kGuid;
	m_pkWaitData->m_fPosX = fPosX;
	m_pkWaitData->m_fPosY = fPosY;
	m_pkWaitData->m_fPosZ = fPosZ;
	m_pkWaitData->m_pkEntity = pkEntity;

	m_pmSettedMonsterList->Add(m_pkWaitData);
	m_iEntityCount += 1;

	m_bAddedMonsterData = true;
	if (m_pkAddedMonsterData)
	{
		delete m_pkAddedMonsterData;
		m_pkAddedMonsterData = 0;
	}
	m_pkAddedMonsterData = new SettedMonsterData;
	m_pkAddedMonsterData->m_kGuid = m_pkWaitData->m_kGuid;
	m_pkAddedMonsterData->m_fPosX = m_pkWaitData->m_fPosX;
	m_pkAddedMonsterData->m_fPosY = m_pkWaitData->m_fPosY;
	m_pkAddedMonsterData->m_fPosZ = m_pkWaitData->m_fPosZ;
	m_pkAddedMonsterData->m_strMemo = m_pkWaitData->m_strMemo;
	m_pkAddedMonsterData->m_iMapNo = m_pkWaitData->m_iMapNo;
	m_pkAddedMonsterData->m_iMonParentBagNo = m_pkWaitData->m_iMonParentBagNo;
	m_pkAddedMonsterData->m_iPointGroup = m_pkWaitData->m_iPointGroup;
	m_pkAddedMonsterData->m_iRegenPeriod = m_pkWaitData->m_iRegenPeriod;
	m_pkAddedMonsterData->m_iMoveRange = m_pkWaitData->m_iMoveRange;
	m_pkAddedMonsterData->m_pkEntity = m_pkWaitData->m_pkEntity;


	SettedMonsterData *pkTemp = new SettedMonsterData;
	pkTemp->m_strMemo = m_pkWaitData->m_strMemo;
	pkTemp->m_iMapNo = m_pkWaitData->m_iMapNo;
	pkTemp->m_iMonParentBagNo = m_pkWaitData->m_iMonParentBagNo;
	pkTemp->m_iPointGroup = m_pkWaitData->m_iPointGroup;
	pkTemp->m_iRegenPeriod = m_pkWaitData->m_iRegenPeriod;
	pkTemp->m_iMoveRange = m_pkWaitData->m_iMoveRange;
	delete m_pkWaitData;
	m_pkWaitData = pkTemp;
}
//---------------------------------------------------------------------------
bool MPgMonster::IsMonsterData(String *strName)
{
	if (strName->Equals("[General]Target.monster_target"))
		return true;

	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_kGuid.ToString()->Equals(strName))
		{
			return true;
		}
	}
	
	return false;
}
//---------------------------------------------------------------------------
Guid MPgMonster::GetDeletedMonsterGuidData(int iDeletedMonsterIdx)
{
	SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmDeletedMonsterList->get_Item(iDeletedMonsterIdx));
		
	return pkData->m_kGuid;
}
//---------------------------------------------------------------------------
void MPgMonster::DeleteToDeletedMonsterGuidData(Guid kGuid)
{
	for (int i=0 ; i<m_pmDeletedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmDeletedMonsterList->get_Item(i));
		if (pkData->m_kGuid == kGuid)
		{
			m_pmDeletedMonsterList->Remove(pkData);
			break;
		}
	}
}
//---------------------------------------------------------------------------
bool MPgMonster::get_AddedMonsterData()
{
	return m_bAddedMonsterData;
}
//---------------------------------------------------------------------------
void MPgMonster::set_AddedMonsterData(bool bAdded)
{
	m_bAddedMonsterData = bAdded;
}
//---------------------------------------------------------------------------
bool MPgMonster::get_DeletedMonsterData()
{
	return m_bDeletedMonsterData;
}
//---------------------------------------------------------------------------
void MPgMonster::set_DeletedMonsterData(bool bDeleted)
{
	m_bDeletedMonsterData = bDeleted;
}
//---------------------------------------------------------------------------
void MPgMonster::ModifyPointGroup(MEntity* pkEntity, int iPointGroup)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			pkData->m_iPointGroup = iPointGroup;
			break;
		}
	}
}
//---------------------------------------------------------------------------
void MPgMonster::ModifyRegenPeriod(MEntity* pkEntity, int iPeriod)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			pkData->m_iRegenPeriod = iPeriod;
			break;
		}
	}
}
//---------------------------------------------------------------------------
void MPgMonster::ModifyParentBagNo(MEntity* pkEntity, int iBagNo)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			pkData->m_iMonParentBagNo = iBagNo;
			break;
		}
	}
}
//---------------------------------------------------------------------------
void MPgMonster::ModifyMemo(MEntity* pkEntity, String *strMemo)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			pkData->m_strMemo = strMemo;

			IEntityPathService* pmPathService = MGetService(IEntityPathService);
			String* strMonName = "Mon_";
			strMonName = strMonName->Concat(strMonName, strMemo);
			String* strCloneName =
				MFramework::Instance->Scene->GetUniqueEntityName(String::Concat(pmPathService->GetSimpleName(strMonName), " 01"));
			pkData->m_pkEntity->Name = strCloneName;
			break;
		}
	}
}
//---------------------------------------------------------------------------
bool MPgMonster::GetMonsterData(MEntity* pkEntity)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			return true;
		}
	}

	return false;
}
int MPgMonster::GetMonsterData_PointGroup(MEntity* pkEntity)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			return pkData->m_iPointGroup;
		}
	}
	return -1;
}
int MPgMonster::GetMonsterData_RegenPeriod(MEntity* pkEntity)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			return pkData->m_iRegenPeriod;
		}
	}
	return -1;
}
int MPgMonster::GetMonsterData_ParentBagNo(MEntity* pkEntity)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			return pkData->m_iMonParentBagNo;
		}
	}
	return -1;
}
String* MPgMonster::GetMonsterData_Memo(MEntity* pkEntity)
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

		if (pkData->m_pkEntity == pkEntity)
		{
			return pkData->m_strMemo;
		}
	}
	return 0;
}
MPoint3* MPgMonster::GetMonsterPos(MEntity* pkEntity)
{
	MPoint3 *pkPoint = new MPoint3(0,0,0);
	NiPoint3 kPoint;

	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));

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

int MPgMonster::GetMonsterRotAxZ(MEntity* pkEntity)
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
void MPgMonster::ChangeHideText()
{
	m_bIsActiveText = !m_bIsActiveText;
}

void MPgMonster::SetSettedMonsterListGroupVisible()
{
	for (int i=0 ; i<m_pmSettedMonsterList->Count ; i++)
	{
		SettedMonsterData *pkData = dynamic_cast<SettedMonsterData *>(m_pmSettedMonsterList->get_Item(i));
		if (!pkData)
		{
			continue;
		}

		bool bHidden = IsHidden(pkData->m_iMonParentBagNo);
		pkData->m_pkEntity->SetHidden(bHidden, false);
	}
}

void MPgMonster::SetMonsterBagControl(Hashtable * pkMonsterBagControl)
{
	m_pmMonsterBagControl = pkMonsterBagControl;
}