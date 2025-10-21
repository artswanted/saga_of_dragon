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

#include "MScene.h"
#include "MEntityFactory.h"
#include "MSelectionSetFactory.h"
#include "MEventManager.h"
#include "ServiceProvider.h"
#include "MUtility.h"
#include "MAddRemoveEntityCommand.h"
#include "MAddRemoveSelectionSetCommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MScene::MScene(NiScene* pkScene) : m_pkScene(pkScene)
{
    ms_pmScenes->Add(this);

    MInitRefObject(m_pkScene);

	m_pkErrors = NiNew NiDefaultErrorHandler();
	MInitRefObject(m_pkErrors);
    
    __hook(&MEventManager::EntityComponentAdded, MEventManager::Instance,
        &MScene::OnEntityComponentAdded);
    __hook(&MEventManager::EntityComponentRemoved, MEventManager::Instance,
        &MScene::OnEntityComponentRemoved);
    __hook(&MEventManager::EntityRemovedFromScene, MEventManager::Instance,
        &MScene::OnEntityRemovedFromScene);
}
//---------------------------------------------------------------------------
void MScene::Do_Dispose(bool bDisposing)
{
	ms_pmScenes->Remove(this);

    MDisposeRefObject(m_pkErrors);
    MDisposeRefObject(m_pkScene);

    if (bDisposing)
    {
        __unhook(&MEventManager::EntityComponentAdded,
            MEventManager::Instance, &MScene::OnEntityComponentAdded);
        __unhook(&MEventManager::EntityComponentRemoved,
            MEventManager::Instance, &MScene::OnEntityComponentRemoved);
        __unhook(&MEventManager::EntityRemovedFromScene,
            MEventManager::Instance, &MScene::OnEntityRemovedFromScene);
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityComponentAdded(MEntity* pmEntity, MComponent* pmComponent)
{
    MVerifyValidInstance;

    unsigned int uiEntityCount = m_pkScene->GetEntityCount();
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        ResolveAddedComponentDependencies(pmEntityInScene, pmEntity,
            pmComponent);
    }
}

//---------------------------------------------------------------------------
void MScene::ResolveAddedComponentDependencies(MEntity* pmEntity,
    MEntity* pmMasterEntity, MComponent* pmAddedComponent)
{
    MVerifyValidInstance;

    if (pmEntity->MasterEntity == pmMasterEntity)
    {
        pmEntity->AddComponent(pmAddedComponent->Clone(true), false, true);
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    unsigned int uiEntityCount = m_pkScene->GetEntityCount();

    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        ResolveRemovedComponentDependencies(pmEntityInScene, pmEntity,
            pmComponent);
    }
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedComponentDependencies(MEntity* pmEntity,
    MEntity* pmMasterEntity, MComponent* pmRemovedComponent)
{
    MVerifyValidInstance;

    if (pmEntity->MasterEntity == pmMasterEntity)
    {
        //Find the same component on the affected entity
        MComponent* pmComponentToRemove = 
            pmEntity->GetComponentByTemplateID(
            pmRemovedComponent->TemplateID);
        if (pmComponentToRemove != NULL)
        {
            pmEntity->RemoveComponent(pmComponentToRemove, false, true);
        }
    }
}
//---------------------------------------------------------------------------
void MScene::OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (!CommandService->BeginUndoFrame(String::Format("Remove all references "
        "to \"{0}\" entity from \"{1}\" scene", pmEntity->Name, this->Name)))
    {
        return;
    }

    for (unsigned int ui = 0; ui < m_pkScene->GetEntityCount(); ui++)
    {
        MEntity* pmEntityInScene = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
        ResolveRemovedEntityDependencies(pmEntityInScene, pmEntity);
    }

    for (unsigned int ui = 0; ui < m_pkScene->GetSelectionSetCount(); ui++)
    {
        MSelectionSet* pmSelectionSet = MSelectionSetFactory::Instance->Get(
            m_pkScene->GetSelectionSetAt(ui));
        ResolveRemovedEntityDependencies(pmSelectionSet, pmEntity);
    }

    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedEntityDependencies(MEntity* pmEntity,
    MEntity* pmRemovedEntity)
{
    MVerifyValidInstance;

    String* astrPropertyNames[] = pmEntity->GetPropertyNames();
    for (int i = 0; i < astrPropertyNames->Length; i++)
    {
        String* strPropertyName = astrPropertyNames[i];
        PropertyType* pmPropertyType = pmEntity->GetPropertyType(
            strPropertyName);
        if (pmPropertyType != NULL && pmPropertyType->PrimitiveType->Equals(
            NiEntityPropertyInterface::PT_ENTITYPOINTER))
        {
            for (unsigned int ui = 0;
                ui < pmEntity->GetElementCount(strPropertyName); ui++)
            {
                MEntity* pmEntityProperty = dynamic_cast<MEntity*>(
                    pmEntity->GetPropertyData(strPropertyName, ui));
                if (pmEntityProperty != NULL)
                {
                    if (pmEntityProperty == pmRemovedEntity)
                    {
                        pmEntity->SetPropertyData(strPropertyName, NULL, ui,
                            true);
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MScene::ResolveRemovedEntityDependencies(MSelectionSet* pmSelectionSet,
    MEntity* pmRemovedEntity)
{
    MVerifyValidInstance;

    MEntity* amEntities[] = pmSelectionSet->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        if (pmEntity == pmRemovedEntity)
        {
            pmSelectionSet->RemoveEntity(pmEntity);
            break;
        }
    }

    if (pmSelectionSet->EntityCount == 0)
    {
        RemoveSelectionSet(pmSelectionSet);
    }
}
//---------------------------------------------------------------------------
NiScene* MScene::GetNiScene()
{
    MVerifyValidInstance;

    return m_pkScene;
}
//---------------------------------------------------------------------------
String* MScene::get_Name()
{
    MVerifyValidInstance;

    return m_pkScene->GetName();
}
//---------------------------------------------------------------------------
void MScene::set_Name(String* strName)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    const char* pcName = MStringToCharPointer(strName);
    m_pkScene->SetName(pcName);
    MFreeCharPointer(pcName);
}
//---------------------------------------------------------------------------
unsigned int MScene::get_EntityCount()
{
    MVerifyValidInstance;

    return m_pkScene->GetEntityCount();
}
//---------------------------------------------------------------------------
bool MScene::get_Dirty()
{
    MVerifyValidInstance;

    bool bDirty = m_bDirtyBit;
    MEntity* pmAllEntities[] = GetEntities();
    int size = pmAllEntities->Count;
    int index = 0;
    while (!bDirty && index < size)
    {
        MEntity* pmEntity = pmAllEntities[index];
        bDirty = bDirty || pmEntity->Dirty;
        index++;
    }

    return bDirty;
}
//---------------------------------------------------------------------------
void MScene::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    m_bDirtyBit = bDirty;
}
//---------------------------------------------------------------------------
MEntity* MScene::GetEntities()[]
{
    MVerifyValidInstance;

    unsigned int uiEntityCount = m_pkScene->GetEntityCount();

    MEntity* amEntities[] = new MEntity*[uiEntityCount];
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        amEntities[ui] = MEntityFactory::Instance->Get(
            m_pkScene->GetEntityAt(ui));
    }

    return amEntities;
}

//---------------------------------------------------------------------------
MEntity* MScene::GetEntityByName(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    NiEntityInterface* pkEntity = m_pkScene->GetEntityByName(pcName);
    MFreeCharPointer(pcName);

    if (pkEntity != NULL)
    {
        return MEntityFactory::Instance->Get(pkEntity);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MScene::AddEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    if (!IsEntityInScene(pmEntity))
    {
        CommandService->ExecuteCommand(new MAddRemoveEntityCommand(
            NiNew NiAddRemoveEntityCommand(m_pkScene,
            pmEntity->GetNiEntityInterface(), true)), bUndoable);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MScene::RemoveEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->ExecuteCommand(new MAddRemoveEntityCommand(
        NiNew NiAddRemoveEntityCommand(m_pkScene,
        pmEntity->GetNiEntityInterface(), false)), bUndoable);
}
//---------------------------------------------------------------------------
void MScene::RemoveAllEntities(bool bUndoable)
{
    MVerifyValidInstance;

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        RemoveEntity(amEntities[i], bUndoable);
    }
}
//---------------------------------------------------------------------------
bool MScene::IsEntityInScene(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");
    return NIBOOL_IS_TRUE(
        m_pkScene->IsEntityInScene(pmEntity->GetNiEntityInterface()));
}
//---------------------------------------------------------------------------
MEntity* MScene::GetDependentEntities(MEntity* pmEntity)[]
{
    MEntity* allEntities[] = GetEntities();
    ArrayList* retVal = new ArrayList();
    for (int i = 0; i < allEntities->Count; i++)
    {
        MEntity* pmCurrentEntity = allEntities[i];//->get_Item(i);
        if (pmEntity == pmCurrentEntity->MasterEntity)
        {
            retVal->Add(pmCurrentEntity);
        }
    }
    return dynamic_cast<MEntity*[]>(retVal->ToArray(__typeof(MEntity)));
}
//---------------------------------------------------------------------------
unsigned int MScene::get_SelectionSetCount()
{
    MVerifyValidInstance;

    return m_pkScene->GetSelectionSetCount();
}
//---------------------------------------------------------------------------
MSelectionSet* MScene::GetSelectionSets()[]
{
    MVerifyValidInstance;

    unsigned int uiSelectionSetCount = m_pkScene->GetSelectionSetCount();

    MSelectionSet* amSelectionSets[] = new MSelectionSet*[
        uiSelectionSetCount];
        for (unsigned int ui = 0; ui < uiSelectionSetCount; ui++)
        {
            amSelectionSets[ui] = MSelectionSetFactory::Instance->Get(
                m_pkScene->GetSelectionSetAt(ui));
        }

        return amSelectionSets;
}
//---------------------------------------------------------------------------
MSelectionSet* MScene::GetSelectionSetByName(String* strName)
{
    MVerifyValidInstance;

    const char* pcName = MStringToCharPointer(strName);
    NiEntitySelectionSet* pkSelectionSet = m_pkScene->GetSelectionSetByName(
        pcName);
    MFreeCharPointer(pcName);

    if (pkSelectionSet != NULL)
    {
        return MSelectionSetFactory::Instance->Get(pkSelectionSet);
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool MScene::AddSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    MAssert(pmSelectionSet != NULL, "Null selection set passed to function!");

    if (GetSelectionSetByName(pmSelectionSet->Name) == NULL)
    {
        CommandService->ExecuteCommand(new MAddRemoveSelectionSetCommand(
            NiNew NiAddRemoveSelectionSetCommand(m_pkScene,
            pmSelectionSet->GetNiEntitySelectionSet(), true)), true);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void MScene::RemoveSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    m_bDirtyBit = true;

    MAssert(pmSelectionSet != NULL, "Null selection set passed to function!");

    if (GetSelectionSetByName(pmSelectionSet->Name) != NULL)
    {
        CommandService->ExecuteCommand(new MAddRemoveSelectionSetCommand(
            NiNew NiAddRemoveSelectionSetCommand(m_pkScene,
            pmSelectionSet->GetNiEntitySelectionSet(), false)), true);
    }
}
//---------------------------------------------------------------------------
void MScene::RemoveAllSelectionSets()
{
    MVerifyValidInstance;

    MSelectionSet* amSelectionSets[] = GetSelectionSets();
    for (int i = 0; i < amSelectionSets->Length; i++)
    {
        RemoveSelectionSet(amSelectionSets[i]);
    }
}
//---------------------------------------------------------------------------
bool MScene::IsSelectionSetInScene(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MAssert(pmSelectionSet != NULL, "Null selection set provided to "
        "function!");

    for (unsigned int ui = 0; ui < m_pkScene->GetSelectionSetCount(); ui++)
    {
        if (pmSelectionSet->GetNiEntitySelectionSet() ==
            m_pkScene->GetSelectionSetAt(ui))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void MScene::Update(float fTime, NiExternalAssetManager* pkAssetManager)
{
    MVerifyValidInstance;

    // Clear out errors in handler.
	m_pkErrors->ClearErrors();
    pkAssetManager->SetErrorHandler(m_pkErrors);

    // Update the scene.
    m_pkScene->Update(fTime, m_pkErrors, pkAssetManager);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
void MScene::UpdateEffects()
{
    MVerifyValidInstance;

    MEntity* amEntities[] = GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        UpdateEffects(amEntities[i]);
    }
}
//---------------------------------------------------------------------------
void MScene::UpdateEffects(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    for (unsigned int ui = 0; ui < pmEntity->GetSceneRootPointerCount();
        ui++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
        if (pkSceneRoot)
        {
            pkSceneRoot->UpdateEffects();
        }
    }
}
//---------------------------------------------------------------------------
String* MScene::GetUniqueEntityName(String* strProposedName)
{
    MVerifyValidInstance;

    if (GetEntityByName(strProposedName) == NULL)
    {
        return strProposedName;
    }

    String* strBaseName = strProposedName;

    // Search for last space character in proposed name.
    int iIndex = strProposedName->LastIndexOf(' ');
    if (iIndex == strProposedName->Length - 1)
    {
        iIndex = strProposedName->LastIndexOf(' ', iIndex - 1);
    }
    if (iIndex > -1)
    {
        String* strSuffix = strProposedName->Substring(iIndex + 1);
        try
        {
            int iSuffix = Int32::Parse(strSuffix);
            strBaseName = strProposedName->Substring(0, iIndex);
        }
        catch (FormatException*)
        {
            // The suffix is not an integer; set the base name to the
            // proposed name.
            strBaseName = strProposedName;
        }

    }

    int iSuffix = 1;
    String* strUniqueName;
    do
    {
        strUniqueName = String::Format("{0} {1:D02}", strBaseName,
            __box(iSuffix++));
    }
    while (GetEntityByName(strUniqueName) != NULL);

    return strUniqueName;
}
//---------------------------------------------------------------------------
String* MScene::GetUniqueSelectionSetName(String* strProposedName)
{
    MVerifyValidInstance;

    if (GetSelectionSetByName(strProposedName) == NULL)
    {
        return strProposedName;
    }

    String* strBaseName = strProposedName;

    // Search for last space character in proposed name.
    int iIndex = strProposedName->LastIndexOf(' ');
    if (iIndex == strProposedName->Length - 1)
    {
        iIndex = strProposedName->LastIndexOf(' ', iIndex - 1);
    }
    if (iIndex > -1)
    {
        String* strSuffix = strProposedName->Substring(iIndex + 1);
        try
        {
            int iSuffix = Int32::Parse(strSuffix);
            strBaseName = strProposedName->Substring(0, iIndex);
        }
        catch (FormatException*)
        {
            // The suffix is not an integer; set the base name to the
            // proposed name.
            strBaseName = strProposedName;
        }

    }

    int iSuffix = 1;
    String* strUniqueName;
    do
    {
        strUniqueName = String::Format("{0} {1:D02}", strBaseName,
            __box(iSuffix++));
    }
    while (GetSelectionSetByName(strUniqueName) != NULL);

    return strUniqueName;
}
//---------------------------------------------------------------------------
void MScene::GetBound(NiBound* pkBound)
{
    MVerifyValidInstance;

    m_pkScene->GetBound(*pkBound);
}
//---------------------------------------------------------------------------
bool MScene::GetPGAvailEntity(MEntity* pkEntity)
{
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("BaseObject"))
	{
		return false;
	}
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("PhysX"))
	{
		return true;
	}
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("CharacterSpawn"))
	{
		return true;
	}
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("Trigger"))
	{
		return true;
	}
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("Path"))
	{
		return true;
	}
	if (pkEntity->GetNiEntityInterface()->GetPGUsePhysX())
	{
		return true;
	}
	if (pkEntity->GetNiEntityInterface()->GetHidden())
	{
		if(pkEntity->GetNiEntityInterface()->GetName().ContainsNoCase("telejump_zone_"))
		{
			return true;
		}
		else if(pkEntity->GetNiEntityInterface()->GetName().ContainsNoCase("teleport_zone_"))
		{
			return true;
		}
	}
	if(pkEntity->GetNiEntityInterface()->GetPGProperty().Equals("Telejump"))
	{
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void MScene::GetPGWolrdBound(NiPoint3* pkMinPos, NiPoint3* pkMaxPos)
{
	MVerifyValidInstance;

	if(NULL==pkMinPos || NULL==pkMaxPos)
	{
		return;
	}

	MEntity* amEntities[] = GetEntities();
	NiPoint3 kEntityPos;
	int const iEntityCount = amEntities->Count;
	for(int i=0;iEntityCount>i;++i)
	{
		if(false != GetPGAvailEntity(amEntities[i]))
		{
			amEntities[i]->GetNiEntityInterface()->GetPropertyData("Translation", kEntityPos);
			pkMinPos->x = System::Math::Min(kEntityPos.x, pkMinPos->x);
			pkMinPos->y = System::Math::Min(kEntityPos.y, pkMinPos->y);
			pkMinPos->z = System::Math::Min(kEntityPos.z, pkMinPos->z);

			pkMaxPos->x = System::Math::Max(kEntityPos.x, pkMaxPos->x);
			pkMaxPos->y = System::Math::Max(kEntityPos.y, pkMaxPos->y);
			pkMaxPos->z = System::Math::Max(kEntityPos.z, pkMaxPos->z);
		}
	}
}
//---------------------------------------------------------------------------
IMessageService* MScene::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
ICommandService* MScene::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
MScene* MScene::FindSceneContainingEntity(MEntity* pmEntity)
{
    for (int i = 0; i < ms_pmScenes->Count; i++)
    {
        MScene* pmScene = dynamic_cast<MScene*>(ms_pmScenes->Item[i]);
        if (pmScene->IsEntityInScene(pmEntity))
        {
            return pmScene;
        }
    }

    return NULL;
}
////---------------------------------------------------------------------------
//String* MScene::get_Path()
//{
//    MVerifyValidInstance;
//
//	return m_strPath;
//}
////---------------------------------------------------------------------------
//void MScene::set_Path(String* strPath)
//{
//    MVerifyValidInstance;
//
//    m_bDirtyBit = true;
//
//    const char* pcPath = MStringToCharPointer(strPath);
//    m_strPath = pcPath;
//    MFreeCharPointer(pcPath);
//}
////---------------------------------------------------------------------------
