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
#include "SceneDesignerFrameworkPCH.h"
//#include "BM/vstring.h"
//#include "BM/BM.h"

#include "MEntity.h"
#include "MPgPropertyDef.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MComponentFactory.h"
#include "MUtility.h"
#include "MAddRemoveComponentCommand.h"
#include "MRenameEntityCommand.h"
#include "MChangeHiddenStateCommand.h"
#include "MChangePGPropertyStateCommand.h"
#include "MChangePGUsePhysXCommand.h"
#include "MChangePGPostfixTextureCommand.h"
#include "MChangePGAlphaGroupCommand.h"
#include "MChangePGOptimizationCommand.h"
#include "MChangePGRandomAniCommand.h"
#include "MChangeFrozenStateCommand.h"
#include "ServiceProvider.h"
#include "MEntityPropertyDescriptor.h"
#include "MExpandablePropertyConverter.h"
#include "MFramework.h"
#include "IEntityPathService.h"
#include "MEntityPostFixTextureUtil.H"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Drawing;
using namespace System::IO;
static const char* gs_pcSceneRootPointerName = "Scene Root Pointer";

//---------------------------------------------------------------------------
MEntity::MEntity(NiEntityInterface* pkEntity) : m_pkEntity(pkEntity)
{
    MInitInterfaceReference(m_pkEntity);
	m_ePGProperty = ePGProperty::Object;
	m_bUsePhysX = false;
	m_kPostfixTexture = "";
	m_bRandomAni = false;
	//m_eAlphaGroup = ePGAlphaGroup::Group0;
	//m_eOptimization = ePGOptimization::High_0;
}
//---------------------------------------------------------------------------
void MEntity::_SDMInit()
{
    m_pkPropertyNames = NiNew NiTObjectSet<NiFixedString>(64);
}
//---------------------------------------------------------------------------
void MEntity::_SDMShutdown()
{
    NiDelete m_pkPropertyNames;
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* MEntity::get_PropertyInterface()
{
    return m_pkEntity;
}
//---------------------------------------------------------------------------
void MEntity::Do_Dispose(bool bDisposing)
{
    MDisposeInterfaceReference(m_pkEntity);
}
//---------------------------------------------------------------------------
String* MEntity::ToString()
{
    MVerifyValidInstance;

    return this->Name;
}
//---------------------------------------------------------------------------
String* MEntity::get_Name()
{
    MVerifyValidInstance;

    return m_pkEntity->GetName();
}
//---------------------------------------------------------------------------
void MEntity::set_Name(String* strName)
{
    MVerifyValidInstance;
    m_bDirtyBit = true;

	String* strOldName = this->Name;

    if (!strName->Equals(strOldName))
    {
        // Check for unique names here.
        MScene* pmScene = MScene::FindSceneContainingEntity(this);
        if (pmScene != NULL && pmScene->GetEntityByName(strName) != NULL)
        {
            throw new ArgumentException(String::Format("An entity "
                "already exists in the \"{0}\" scene with the name "
                "\"{1}\"; new name cannot be set.", pmScene->Name, strName));

			return;
        }

		// if this entity is Npc then Add Npc NameChange
		if (MFramework::Instance->Npc->IsNpcData(this))
		{
			MFramework::Instance->Npc->InsertModifiedNpcData(strOldName, strName);
		}

        const char* pcName = MStringToCharPointer(strName);
        CommandService->ExecuteCommand(new MRenameEntityCommand(
            NiNew NiRenameEntityCommand(m_pkEntity, pcName)), true);

		// Update Name
		if (pmScene && pmScene->GetNiScene())
		{
			NiFixedString kNewName = pcName;
			NiFixedString kOldName = MStringToCharPointer(strOldName);
			//bool bRet = pmScene->GetNiScene()->UpdateName(kOldName, kNewName);
			//assert(bRet);
		}
		MFreeCharPointer(pcName);
    }
}
//---------------------------------------------------------------------------
Guid MEntity::get_TemplateID()
{
    MVerifyValidInstance;

    return MUtility::IDToGuid(m_pkEntity->GetTemplateID());
}
//---------------------------------------------------------------------------
void MEntity::set_TemplateID(Guid mGuid)
{
    MVerifyValidInstance;
    NiUniqueID kUniqueID;
    MUtility::GuidToID(mGuid, kUniqueID);
    m_pkEntity->SetTemplateID(kUniqueID);
}
//---------------------------------------------------------------------------
MEntity* MEntity::CreateGeneralEntity(String* strName)
{
    const char* pcName = MStringToCharPointer(strName);
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiGeneralEntity* pkEntity = NiNew NiGeneralEntity(pcName, kTemplateID);
    MFreeCharPointer(pcName);
    return MEntityFactory::Instance->Get(pkEntity);
}
//---------------------------------------------------------------------------
MEntity* MEntity::Clone(String* pstrNewName, bool bInheritProperties)
{
    MVerifyValidInstance;

    const char* pcNewName = MStringToCharPointer(pstrNewName);
    MEntity* pmClone = MEntityFactory::Instance->Get(m_pkEntity->Clone(
        pcNewName, bInheritProperties));
    MFreeCharPointer(pcNewName);
    pmClone->Update(0, MFramework::Instance->ExternalAssetManager);
	// Clone ProjectG Property(PG)
	pmClone->SetPGPostfixTexture(m_kPostfixTexture, false);
	//알파그룹을 지정할 때 해당 클론의 MasterEntity->name을 얻어와서 알파그룹판단용으로 사용
	//마스터엔티티가 없을 경우 알파그룹은 디폴트 Group5로 설정
	//있지만 general, object, block, effect가 아닐 경우 1로 설정
	m_eAlphaGroup = Group1;
	if(NULL != pmClone->MasterEntity)
	{
		//오브젝트를 복사하는 경우 팔레트가 활성화 되지 않을 수가 있으므로 그에 대한 처리.
		//인스턴스나, 팔레트매니저가 활성화되지 않은 경우는 
		//프로그램이 정상적으로 동작하지 않는다고 봐야하므로 처리 생략
		if(false == bInheritProperties) //복사일 경우 부모 엔티티의 알파그룹 그대로 승계
		{
			ePGAlphaGroup const s_eaAlphaGroup[] = //매니지드라 static 불가
			{
				Group_5, Group_4, Group_3, Group_2, Group_1, 
				Group0,	Group1,	Group2,	Group3,	Group4,	Group5
			};
			
			int const iCalibration = 5;	//Entity->GetAlphaGroup()으로 가져올 경우
										//값이 -5~5사이값이 나오므로 보정을 하기 위한 수치
			char const * pcAlphaGroup = m_pkEntity->GetPGAlphaGroup();
			System::String *strAlphaGroup(pcAlphaGroup);
			System::IFormatProvider* kIFP;
			unsigned int const uiIndexOfAlphaArray = strAlphaGroup->ToInt32(kIFP)+iCalibration;
			m_eAlphaGroup = s_eaAlphaGroup[uiIndexOfAlphaArray];
			
		}
		//복사가 아닌 경우는 팔렛에 따라 알파그룹 설정
		else if(NULL != MFramework::Instance->PaletteManager->ActivePalette)
		{
			System::String *pstrNameOfMasterEntity = MFramework::Instance->PaletteManager->ActivePalette->Name;

			if(System::String::Compare(pstrNameOfMasterEntity, "General") == 0)
			{
				m_eAlphaGroup = Group5;
			}
			else if(System::String::Compare(pstrNameOfMasterEntity, "Object") == 0)
			{
				m_eAlphaGroup = Group1;
			}
			else if(System::String::Compare(pstrNameOfMasterEntity, "Block") == 0)
			{
				m_eAlphaGroup = Group3;
			}
			else if(System::String::Compare(pstrNameOfMasterEntity, "Effect") == 0)
			{
				m_eAlphaGroup = Group1;
			}
			else
			{
				m_eAlphaGroup = Group5;
			}
		}
	}
	pmClone->SetPGAlphaGroup(m_eAlphaGroup, false);
	pmClone->SetPGOptimization(m_eOptimization, false);
	pmClone->SetPGProperty(m_ePGProperty);
	pmClone->SetPGUsePhysX(m_bUsePhysX, false);
	pmClone->SetPGRandomAni(m_bRandomAni, false);
	// PG End

    return pmClone;
}
//---------------------------------------------------------------------------
MEntity* MEntity::get_MasterEntity()
{
    MVerifyValidInstance;

    NiEntityInterface* pkMasterEntity = m_pkEntity->GetMasterEntity();
    if (pkMasterEntity)
    {
        return MEntityFactory::Instance->Get(pkMasterEntity);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MEntity::set_MasterEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    
    MAssert(pmEntity != this, "Master Entity set to itself");

    if (pmEntity)
    {
        m_pkEntity->ReplaceMasterEntity(pmEntity->m_pkEntity);
    }
    else
    {
        m_pkEntity->ReplaceMasterEntity(NULL);
    }
}
//---------------------------------------------------------------------------
void MEntity::MakeEntityUnique()
{
    MVerifyValidInstance;
    m_bDirtyBit = true;

    String* astrPropertyNames[] = GetPropertyNames();

    CommandService->BeginUndoFrame(String::Format("Make \"{0}\" entity "
        "unique", this->Name));
    for (int i = 0; i < astrPropertyNames->Length; i++)
    {
        MakePropertyUnique(astrPropertyNames[i]);
    }
    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
bool MEntity::get_SupportsComponents()
{
    MVerifyValidInstance;

    return NIBOOL_IS_TRUE(m_pkEntity->SupportsComponents());
}
//---------------------------------------------------------------------------
unsigned int MEntity::get_ComponentCount()
{
    MVerifyValidInstance;

    return m_pkEntity->GetComponentCount();
}
//---------------------------------------------------------------------------
MComponent* MEntity::GetComponents()[]
{
    MVerifyValidInstance;

    unsigned int uiComponentCount = m_pkEntity->GetComponentCount();
    MComponent* amComponents[] = new MComponent*[uiComponentCount];
    for (unsigned int ui = 0; ui < uiComponentCount; ui++)
    {
        amComponents[ui] = MComponentFactory::Instance->Get(
            m_pkEntity->GetComponentAt(ui));
    }

    return amComponents;
}
//---------------------------------------------------------------------------
MComponent* MEntity::GetComponentByTemplateID(Guid mTemplateID)
{
    MVerifyValidInstance;

    NiUniqueID kTemplateID;
    MUtility::GuidToID(mTemplateID, kTemplateID);
    return MComponentFactory::Instance->Get(
        m_pkEntity->GetComponentByTemplateID(kTemplateID));
}
//---------------------------------------------------------------------------
bool MEntity::CanAddComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    return NIBOOL_IS_TRUE(pmComponent->GetNiEntityComponentInterface()
        ->CanAttachToEntity(m_pkEntity));
}
//---------------------------------------------------------------------------
bool MEntity::CanRemoveComponent(MComponent* pmComponent)
{
    MVerifyValidInstance;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    return NIBOOL_IS_TRUE(pmComponent->GetNiEntityComponentInterface()
        ->CanDetachFromEntity(m_pkEntity));

}
//---------------------------------------------------------------------------
void MEntity::AddComponent(MComponent* pmComponent,
    bool bPerformErrorChecking, bool bUndoable)
{
    MVerifyValidInstance;
    m_bDirtyBit = true;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveComponentCommand(
        NiNew NiAddRemoveComponentCommand(m_pkEntity, 
        pmComponent->GetNiEntityComponentInterface(), true,
        bPerformErrorChecking)), bUndoable);

}
//---------------------------------------------------------------------------
void MEntity::RemoveComponent(MComponent* pmComponent,
    bool bPerformErrorChecking, bool bUndoable)
{
    MVerifyValidInstance;
    m_bDirtyBit = true;

    MAssert(pmComponent != NULL, "Null component provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveComponentCommand(
        NiNew NiAddRemoveComponentCommand(m_pkEntity, 
        pmComponent->GetNiEntityComponentInterface(), false,
        bPerformErrorChecking)), bUndoable);

}
//---------------------------------------------------------------------------
String* MEntity::GetPropertyNames()[]
{
    MVerifyValidInstance;

    assert(m_pkPropertyNames->GetSize() == 0);
    m_pkEntity->GetPropertyNames(*m_pkPropertyNames);

    String* astrPropertyNames[] = new String*[m_pkPropertyNames->GetSize()];
    for (unsigned int ui = 0; ui < m_pkPropertyNames->GetSize(); ui++)
    {
        astrPropertyNames[ui] = m_pkPropertyNames->GetAt(ui);
    }

    m_pkPropertyNames->RemoveAll();

    return astrPropertyNames;
}
//---------------------------------------------------------------------------
NiAVObject* MEntity::GetSceneRootPointer(unsigned int uiIndex)
{
    MVerifyValidInstance;

    if (uiIndex >= GetSceneRootPointerCount())
    {
        return NULL;
    }

    NiObject* pkSceneRootPointer = NULL;
    NiAVObject* pkSceneRoot = NULL;
    if (m_pkEntity->GetPropertyData(gs_pcSceneRootPointerName,
        pkSceneRootPointer, uiIndex))
    {
        pkSceneRoot = NiDynamicCast(NiAVObject, pkSceneRootPointer);
    }

    return pkSceneRoot;
}
//---------------------------------------------------------------------------
unsigned int MEntity::GetSceneRootPointerCount()
{
    MVerifyValidInstance;

    unsigned int uiCount;
    if (!m_pkEntity->GetElementCount(gs_pcSceneRootPointerName, uiCount))
    {
        uiCount = 0;
    }

    return uiCount;
}
//---------------------------------------------------------------------------
bool MEntity::get_Hidden()
{
    MVerifyValidInstance;

    return NIBOOL_IS_TRUE(m_pkEntity->GetHidden());
}
//---------------------------------------------------------------------------
void MEntity::set_Hidden(bool bHidden)
{
    MVerifyValidInstance;

    SetHidden(bHidden, true);
}
//---------------------------------------------------------------------------
void MEntity::SetHidden(bool bHidden, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    if (this->Hidden != bHidden)
    {
        CommandService->ExecuteCommand(new MChangeHiddenStateCommand(
            NiNew NiChangeHiddenStateCommand(m_pkEntity, bHidden)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_Frozen()
{
    MVerifyValidInstance;

    return m_bFrozen;
}
//---------------------------------------------------------------------------
void MEntity::set_Frozen(bool bFrozen)
{
    MVerifyValidInstance;


    SetFrozen(bFrozen, true);
}
//---------------------------------------------------------------------------
void MEntity::SetFrozen(bool bFrozen, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    if (m_bFrozen != bFrozen)
    {
        CommandService->ExecuteCommand(new MChangeFrozenStateCommand(this,
            bFrozen), bUndoable);
    }
}
//---------------------------------------------------------------------------
void MEntity::SetPropertyDataInMEntity(String* strPropertyName, Object* pmData, bool bUndoable)
{
    MVerifyValidInstance;

    const char* pcPropertyName = MStringToCharPointer(strPropertyName);
    NiFixedString kPropertyName = pcPropertyName;
    MFreeCharPointer(pcPropertyName);
    NiFixedString kPrimitiveType;
    bool bSuccess = NIBOOL_IS_TRUE(PropertyInterface->GetPrimitiveType(
        kPropertyName, kPrimitiveType));
    MAssert(bSuccess, "MEntity Error: Property name not found!");

	if (kPrimitiveType == NiEntityPropertyInterface::PT_FLOAT)
	{
        __box float* pfData = dynamic_cast<__box float*>(pmData);
        if (pfData == NULL)
        {
            if (pmData == NULL)
            {
                pfData = __box((float)0);
            }
        }
        MAssert(pfData != NULL, "Object type does not match primitive type; "
            "cannot set value.");

		//MFramework::Instance->ExtEntity->ScaleExtEntity(this, *pfData);
	}
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_POINT3)
    {
        MPoint3* pmPoint3 = dynamic_cast<MPoint3*>(pmData);
        if (pmPoint3 == NULL)
        {
            if (pmData == NULL)
            {
                pmPoint3 = new MPoint3();
            }
        }
        MAssert(pmPoint3 != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiPoint3 kData;
        pmPoint3->ToNiPoint3(kData);

		MFramework::Instance->ExtEntity->TranslateExtEntity(this, kData);
    }
    else if (kPrimitiveType == NiEntityPropertyInterface::PT_MATRIX3)
    {
        MMatrix3* pmMatrix3 = dynamic_cast<MMatrix3*>(pmData);
        if (pmMatrix3 == NULL)
        {
            if (pmData == NULL)
            {
                pmMatrix3 = new MMatrix3();
            }
        }
        MAssert(pmMatrix3 != NULL, "Object type does not match primitive "
            "type; cannot set value.");
        NiMatrix3 kData;
        pmMatrix3->ToNiMatrix3(kData);

		MFramework::Instance->ExtEntity->RotateExtEntity(this, kData);
    }
}
//---------------------------------------------------------------------------
void MEntity::SetPropertyData(String* strPropertyName, Object* pmData, bool bUndoable)
{
	SetPropertyDataInMEntity(strPropertyName, pmData, bUndoable);
	MPropertyContainer::SetPropertyData(strPropertyName, pmData, bUndoable);
}
//---------------------------------------------------------------------------
void MEntity::SetPropertyData(String* strPropertyName, Object* pmData, unsigned int uiIndex, bool bUndoable)
{
	SetPropertyDataInMEntity(strPropertyName, pmData, bUndoable);
	MPropertyContainer::SetPropertyData(strPropertyName, pmData, uiIndex, bUndoable);
}
//---------------------------------------------------------------------------
MEntity::ePGProperty MEntity::get_PGProperty()
{
    MVerifyValidInstance;
	return m_ePGProperty;
}
//---------------------------------------------------------------------------
void MEntity::set_PGProperty(ePGProperty eType)
{
    MVerifyValidInstance;
	SetPGProperty(eType);
}
//---------------------------------------------------------------------------
void MEntity::SetPGProperty(ePGProperty eType)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

	bool bUndoable = false;

	if (m_ePGProperty != eType)
    {
		m_ePGProperty = eType;
		int iType = (int)eType;

		NiFixedString strPGProperty = gs_pcPGProperty[iType];
        CommandService->ExecuteCommand(new MChangePGPropertyStateCommand(
            NiNew NiChangePGPropertyStateCommand(m_pkEntity, strPGProperty)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_PGUsePhysX()
{
    MVerifyValidInstance;
	return m_bUsePhysX;
}
//---------------------------------------------------------------------------
void MEntity::set_PGUsePhysX(bool bUsePhysX)
{
    MVerifyValidInstance;
	SetPGUsePhysX(bUsePhysX, true);
}
//---------------------------------------------------------------------------
void MEntity::SetPGUsePhysX(bool bUsePhysX, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

	if (m_bUsePhysX != bUsePhysX)
    {
		m_bUsePhysX = bUsePhysX;
        CommandService->ExecuteCommand(new MChangePGUsePhysXCommand(
            NiNew NiChangePGUsePhysXCommand(m_pkEntity, bUsePhysX)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
String* MEntity::get_PGPostfixTexture()
{
    MVerifyValidInstance;
	return m_kPostfixTexture;
}
//---------------------------------------------------------------------------
void MEntity::set_PGPostfixTexture(String* kPostfixTexture)
{
    MVerifyValidInstance;
	SetPGPostfixTexture(kPostfixTexture, true);
}
//---------------------------------------------------------------------------
// Geometry를 찾는 Stack 재귀 함수, 깊이 탐색
void MEntity::GetAllGeometries(NiObjectList &kGeometries)
{
	unsigned int iTot = GetSceneRootPointerCount();
	for (unsigned int i=0 ; i<iTot ; i++)
	{
		NiNode *pkRoot = NiDynamicCast(NiNode, GetSceneRootPointer(i));
		GetAllGeometries(pkRoot, kGeometries);
	}
}

void MEntity::GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeometries)
{
	for(unsigned int i = 0;
		i < pkNode->GetChildCount();
		i++)
	{	
		NiAVObject *pkChild = pkNode->GetAt(i);

		if(NiIsKindOf(NiNode, pkChild))
		{
			GetAllGeometries(NiDynamicCast(NiNode, pkChild), kGeometries);
			//kQueue.AddHead(pkChild);
		}
		else if(NiIsKindOf(NiGeometry, pkChild))
		{
			kGeometries.AddTail(pkChild);
			NiPoint3 *pkPoint = NiDynamicCast(NiGeometry, pkChild)->GetVertices();
			NiGeometryData *pkData = NiDynamicCast(NiGeometry, pkChild)->GetModelData();
		}
	}
}

// HandOver, Set Postfix Tex, 강정욱 2008.01.29
// Hash로 바꾸어야할 텍스쳐들을 얻은 다음에
// Set을 합니다.
void MEntity::SetPGPostfixTexture(String* kPostfixTexture, bool bUndoable)
{
	String* kFinalPostfix = MEntityPostFixTextureUtil::ApplyPostFixTextureToEntity(this,kPostfixTexture);

	// 프로퍼티값 변경.
	m_kPostfixTexture = kFinalPostfix;
	const char* pcPostfixTexture = MStringToCharPointer(m_kPostfixTexture);
	CommandService->ExecuteCommand(new MChangePGPostfixTextureCommand(
		NiNew NiChangePGPostfixTextureCommand(m_pkEntity, pcPostfixTexture)),
		bUndoable);
	MFreeCharPointer(pcPostfixTexture);
}
//---------------------------------------------------------------------------
MEntity::ePGAlphaGroup MEntity::get_PGAlphaGroup()
{
    MVerifyValidInstance;
	return m_eAlphaGroup;
}
//---------------------------------------------------------------------------
void MEntity::set_PGAlphaGroup(ePGAlphaGroup eAlphaGroup)
{
    MVerifyValidInstance;
	SetPGAlphaGroup(eAlphaGroup, false);
}
//---------------------------------------------------------------------------
void MEntity::SetPGAlphaGroup(ePGAlphaGroup eAlphaGroup, bool bUndoable)
{
    MVerifyValidInstance;
    m_bDirtyBit = true;
	if (m_eAlphaGroup != eAlphaGroup)
    {
		m_eAlphaGroup = eAlphaGroup;
		int iType = (int)eAlphaGroup;

		NiFixedString strAlphaGroup = gs_pcPGAlphaGroup[iType];
        CommandService->ExecuteCommand(new MChangePGAlphaGroupCommand(
            NiNew NiChangePGAlphaGroupCommand(m_pkEntity, strAlphaGroup)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_PGRandomAni()
{
    MVerifyValidInstance;
	return m_bRandomAni;
}
//---------------------------------------------------------------------------
void MEntity::set_PGRandomAni(bool bRandomAni)
{
    MVerifyValidInstance;
	SetPGRandomAni(bRandomAni, true);
}
//---------------------------------------------------------------------------
void MEntity::SetPGRandomAni(bool bRandomAni, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

	if (m_bRandomAni != bRandomAni)
    {
		m_bRandomAni = bRandomAni;
        CommandService->ExecuteCommand(new MChangePGRandomAniCommand(
            NiNew NiChangePGRandomAniCommand(m_pkEntity, bRandomAni)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
MEntity::ePGOptimization MEntity::get_PGOptimization()
{
    MVerifyValidInstance;
	return m_eOptimization;
}
//---------------------------------------------------------------------------
void MEntity::set_PGOptimization(ePGOptimization eOptimization)
{
    MVerifyValidInstance;
	SetPGOptimization(eOptimization, false);
}
//---------------------------------------------------------------------------
void MEntity::SetPGOptimization(ePGOptimization eOptimization, bool bUndoable)
{
    MVerifyValidInstance;
    m_bDirtyBit = true;
	if (m_eOptimization != eOptimization)
    {
		m_eOptimization = eOptimization;
		int iType = (int)eOptimization;

		NiFixedString strPGOptimization = gs_pcPGOptimization[iType];
        CommandService->ExecuteCommand(new MChangePGOptimizationCommand(
            NiNew NiChangePGOptimizationCommand(m_pkEntity, strPGOptimization)),
            bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MEntity::get_Dirty()
{
    MVerifyValidInstance;

    bool bDirty = __super::get_Dirty();
    //This may not be needed...
    if (!bDirty && MasterEntity != NULL)
    {
        bDirty = bDirty || MasterEntity->Dirty;
    }

    return bDirty;
}
//---------------------------------------------------------------------------
void MEntity::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    m_bDirtyBit = bDirty;
}
//---------------------------------------------------------------------------
NiEntityInterface* MEntity::GetNiEntityInterface()
{
    MVerifyValidInstance;

    return m_pkEntity;
}
//---------------------------------------------------------------------------
void MEntity::Update(float fTime, NiExternalAssetManager* pkAssetManager)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkAssetManager->SetErrorHandler(spErrors);

    // Update the entity.
    m_pkEntity->Update(NULL, fTime, spErrors, pkAssetManager);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);
}
//---------------------------------------------------------------------------
IMessageService* MEntity::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEntity::GetProperties()
{
    MVerifyValidInstance;

    return GetProperties(new Attribute*[0]);
}
//---------------------------------------------------------------------------
PropertyDescriptorCollection* MEntity::GetProperties(
    Attribute* amAttributes[])
{
    MVerifyValidInstance;

    ArrayList* pmPropertyDescs = new ArrayList();

    String* strGeneralCategory = "General";
    String* strProejctG = "ProejctG";

    // Name.
    Attribute* amEntityAttributes[] = new Attribute*[3];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute(
        "The name of the entity.");
    amEntityAttributes[2] = new MergablePropertyAttribute(false);
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Name, "Name", amEntityAttributes));

    // ProjectG Property.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
        "Game specific property for Entity.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGProperty, "ProjectG Property", amEntityAttributes));

    // ProjectG UsePhysX.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
        "Set object PhysX");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGUsePhysX, "Use PhysX", amEntityAttributes));

    // ProjectG PostfixTexutre.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
        "텍스쳐를 로드할때 'Postfix Texture'에 입력한 것을 네이밍에 더해서 텍스쳐를 로드 합니다.\n"
		"예) 'texture.dds'를 로드 하는 물체에 'Postfix Texture'속성에 '_1' 이라고 입력하면 클라이언트에서는 'texture_1.dds'를 로드합니다.\n"
		"없는 텍스쳐를 로드 했을때에는 맵툴에서는 이상하게 보이지만 클라이언트 상에선 Default를 로드합니다.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGPostfixTexture, "Postfix Texture", amEntityAttributes));

    // ProjectG AlphaGroup.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
		"Alpha Group을 설정합니다. '_5' 라는것은 '-5'를 가리킵니다.\n"
		"Alpha Group에 설정한 값이 적은 Alpha를 먼저 그립니다." );
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGAlphaGroup, "Alpha Group", amEntityAttributes));

    // ProjectG Optimization.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
		"최적화를 설정합니다. 적힌 값 사양 대로 보입니다. \n"
		"'High_Mid_Low' 는 모든 곳에서 다 보이고 'High_Mid' 는 High와 Mid에서만 보이고 'High'는 High에서만 보입니다." );
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGOptimization, "Optimization", amEntityAttributes));

	/*
    // ProjectG RandomAni.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strProejctG);
    amEntityAttributes[1] = new DescriptionAttribute(
        "애니메이션이 들어간 물체가. 클라이언트에서 재생시.. 재생 시작 위치가 랜덤으로 재생 하는지.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::ProjectGRandomAni, "Random Ani", amEntityAttributes));
		*/

    // Entity template.
    amEntityAttributes = new Attribute*[2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("The entity template "
        "from which this entity inherits its properties.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::MasterEntity, "Entity Template",
        amEntityAttributes));

    // Hidden.
    amEntityAttributes = new Attribute* [2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("Indicates whether or "
        "not the entity is hidden. Hidden entities are not displayed in the "
        "tool.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Hidden, "Hidden", amEntityAttributes));

    // Frozen.
    amEntityAttributes = new Attribute* [2];
    amEntityAttributes[0] = new CategoryAttribute(strGeneralCategory);
    amEntityAttributes[1] = new DescriptionAttribute("Indicates whether or "
        "not the entity is frozen. Frozen entities cannot be edited in the "
        "tool.");
    pmPropertyDescs->Add(new EntityDescriptor(this,
        EntityDescriptor::ValueType::Frozen, "Frozen", amEntityAttributes));

    PropertyDescriptorCollection* pmBaseProperties =
        __super::GetProperties(amAttributes);

    pmPropertyDescs->AddRange(pmBaseProperties);

    PropertyDescriptor* amPropertyDescArray[] =
        dynamic_cast<PropertyDescriptor*[]>(pmPropertyDescs->ToArray(
        __typeof(PropertyDescriptor)));
    return new PropertyDescriptorCollection(amPropertyDescArray);
}
//---------------------------------------------------------------------------
// EntityDescriptor implementation.
//---------------------------------------------------------------------------
MEntity::EntityDescriptor::EntityDescriptor(MEntity* pmEntity,
    ValueType eValueType, String* strName, Attribute* amAttributes[]) :
    PropertyDescriptor(strName, amAttributes), m_pmEntity(pmEntity),
    m_eValueType(eValueType)
{
    MAssert(m_pmEntity != NULL, "MEntity::EntityDescriptor Error: "
        "Null entity provided to constructor.");
}
//---------------------------------------------------------------------------
Type* MEntity::EntityDescriptor::get_ComponentType()
{
    return m_pmEntity->GetType();
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::get_IsReadOnly()
{
    switch (m_eValueType)
    {
        case ValueType::Name:
        case ValueType::Hidden:
        case ValueType::Frozen:
        case ValueType::ProjectGProperty:
        case ValueType::ProjectGUsePhysX:
        case ValueType::ProjectGPostfixTexture:
        case ValueType::ProjectGAlphaGroup:
        case ValueType::ProjectGOptimization:
        case ValueType::ProjectGRandomAni:
            if (MScene::FindSceneContainingEntity(m_pmEntity) ==
                MFramework::Instance->Scene)
            {
                return false;
            }
            else
            {
                return true;
            }
        case ValueType::MasterEntity:
            return true;
        default:
            return true;
    }
}
//---------------------------------------------------------------------------
Type* MEntity::EntityDescriptor::get_PropertyType()
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            return m_pmEntity->Name->GetType();
        case ValueType::MasterEntity:
            return __typeof(String);
        case ValueType::Hidden:
            return __box(m_pmEntity->Hidden)->GetType();
        case ValueType::Frozen:
            return __box(m_pmEntity->Frozen)->GetType();
        case ValueType::ProjectGProperty:
			return __box(m_pmEntity->PGProperty)->GetType();
        case ValueType::ProjectGUsePhysX:
			return __box(m_pmEntity->PGUsePhysX)->GetType();
        case ValueType::ProjectGPostfixTexture:
            return m_pmEntity->PGPostfixTexture->GetType();
        case ValueType::ProjectGAlphaGroup:
			return __box(m_pmEntity->PGAlphaGroup)->GetType();
        case ValueType::ProjectGOptimization:
			return __box(m_pmEntity->PGOptimization)->GetType();
        case ValueType::ProjectGRandomAni:
			return __box(m_pmEntity->PGRandomAni)->GetType();
        default:
            return NULL;
    }
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::CanResetValue(Object* pmComponent)
{
    return false;
}
//---------------------------------------------------------------------------
Object* MEntity::EntityDescriptor::GetValue(Object* pmComponent)
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            return m_pmEntity->Name;
        case ValueType::MasterEntity:
            if (m_pmEntity->MasterEntity != NULL)
            {
                IEntityPathService* pmPathService = 
                    MGetService(IEntityPathService);
                String* strFullPath = 
                    pmPathService->FindFullPath(m_pmEntity->MasterEntity);

                return strFullPath;
            }
            else
            {
                return NULL;
            }
        case ValueType::Hidden:
            return __box(m_pmEntity->Hidden);
        case ValueType::Frozen:
            return __box(m_pmEntity->Frozen);
        case ValueType::ProjectGProperty:
			return __box(m_pmEntity->PGProperty);
        case ValueType::ProjectGUsePhysX:
			return __box(m_pmEntity->PGUsePhysX);
        case ValueType::ProjectGPostfixTexture:
            return m_pmEntity->PGPostfixTexture;
        case ValueType::ProjectGAlphaGroup:
			return __box(m_pmEntity->PGAlphaGroup);
        case ValueType::ProjectGOptimization:
			return __box(m_pmEntity->PGOptimization);
        case ValueType::ProjectGRandomAni:
			return __box(m_pmEntity->PGRandomAni);
		default:
            return NULL;
    }
}
//---------------------------------------------------------------------------
void MEntity::EntityDescriptor::ResetValue(Object* pmComponent)
{
}
//---------------------------------------------------------------------------
void MEntity::EntityDescriptor::SetValue(Object* pmComponent, Object* pmValue)
{
    switch (m_eValueType)
    {
        case ValueType::Name:
            m_pmEntity->Name = dynamic_cast<String*>(pmValue);
            break;
        case ValueType::Hidden:
            m_pmEntity->Hidden = *dynamic_cast<__box bool*>(pmValue);
            break;
        case ValueType::Frozen:
            m_pmEntity->Frozen = *dynamic_cast<__box bool*>(pmValue);
            break;
        case ValueType::ProjectGProperty:
            m_pmEntity->PGProperty = *dynamic_cast<__box ePGProperty*>(pmValue);
            break;
        case ValueType::ProjectGUsePhysX:
            m_pmEntity->PGUsePhysX = *dynamic_cast<__box bool*>(pmValue);
            break;
        case ValueType::ProjectGPostfixTexture:
            m_pmEntity->PGPostfixTexture = dynamic_cast<String*>(pmValue);
            break;
        case ValueType::ProjectGAlphaGroup:
            m_pmEntity->PGAlphaGroup = *dynamic_cast<__box ePGAlphaGroup*>(pmValue);
			break;
        case ValueType::ProjectGOptimization:
            m_pmEntity->PGOptimization = *dynamic_cast<__box ePGOptimization*>(pmValue);
            break;
        case ValueType::ProjectGRandomAni:
            m_pmEntity->PGRandomAni = *dynamic_cast<__box bool*>(pmValue);
            break;
        default:
            break;
    }

	//Filter
	switch(m_eValueType)
	{
	case ValueType::ProjectGProperty:
		{
			if( MFramework::Instance->LightManager->IsEntityFilteringProperty(m_pmEntity->PGProperty) )
			{
				MFramework::Instance->LightManager->RemoveEntityFromLights(m_pmEntity);
			}
		}break;
	}
}
//---------------------------------------------------------------------------
bool MEntity::EntityDescriptor::ShouldSerializeValue(Object* pmComponent)
{
    return true;
}
//---------------------------------------------------------------------------
