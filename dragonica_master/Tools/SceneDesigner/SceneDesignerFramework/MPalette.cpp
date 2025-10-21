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

#include "MPalette.h"
#include "MSceneFactory.h"
#include "MEntityFactory.h"
#include "MEventManager.h"
#include "ServiceProvider.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Text::RegularExpressions;

//---------------------------------------------------------------------------
MPalette::MPalette(MScene* pmScene)
{
    m_pmScene = pmScene;
}
//---------------------------------------------------------------------------
MPalette::MPalette(String* strName, unsigned int uiInitialNumEntities)
{
    const char* pcName = MStringToCharPointer(strName);
    NiScene* pkScene = NiNew NiScene(pcName, uiInitialNumEntities);
    MFreeCharPointer(pcName);
    m_pmScene = MSceneFactory::Instance->Get(pkScene);
	m_pPath = NULL;
}
//---------------------------------------------------------------------------
void MPalette::Do_Dispose(bool bDisposing)
{
}
//---------------------------------------------------------------------------
ICommandService* MPalette::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
String* MPalette::get_Name()
{
    MVerifyValidInstance;

    return m_pmScene->Name;
}
//---------------------------------------------------------------------------
void MPalette::set_Name(String* strName)
{
    MVerifyValidInstance;

    m_pmScene->Name = strName;
}
//---------------------------------------------------------------------------
MScene* MPalette::get_Scene()
{
    MVerifyValidInstance;

    return m_pmScene;
}
//---------------------------------------------------------------------------
unsigned int MPalette::get_EntityCount()
{
    MVerifyValidInstance;

    return m_pmScene->EntityCount;
}
//---------------------------------------------------------------------------
bool MPalette::get_DontSave()
{
    return m_bDontSave;
}
//---------------------------------------------------------------------------
void MPalette::set_DontSave(bool bDontSave)
{
    m_bDontSave = bDontSave;
}
//---------------------------------------------------------------------------
String* MPalette::get_Path()
{
	return m_pPath;
}
//---------------------------------------------------------------------------
void MPalette::set_Path(String* pPath)
{
	m_pPath = pPath;
}
//---------------------------------------------------------------------------
MEntity* MPalette::GetEntities()[]
{
    MVerifyValidInstance;

    return m_pmScene->GetEntities();
}
//---------------------------------------------------------------------------
MEntity* MPalette::GetEntityByName(String* strName)
{
    MVerifyValidInstance;

    return m_pmScene->GetEntityByName(strName);
}
//---------------------------------------------------------------------------
bool MPalette::AddEntity(MEntity* pmEntity, String* strCategory,
    bool bUndoable)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    String* strFullName;
    if (strCategory->Equals(String::Empty))
    {
        strFullName = pmEntity->Name;
    }
    else
    {
        strFullName = String::Format("{0}.{1}", strCategory, pmEntity->Name);
    }

    //strFullName = m_pmScene->GetUniqueEntityName(strFullName);
        
    if (GetEntityByName(strFullName) == NULL)
    {
        CommandService->BeginUndoFrame(String::Format("Add \"{0}\" entity to "
            "\"{1}\" palette", pmEntity->Name, this->Name));

        pmEntity->Name = strFullName;
        bool bSuccess = m_pmScene->AddEntity(pmEntity, true);

        CommandService->EndUndoFrame(bUndoable);
        return bSuccess;
    }

    return false;
}
//---------------------------------------------------------------------------
void MPalette::RemoveEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->BeginUndoFrame(String::Format("Remove \"{0}\" entity "
        "from \"{1}\" palette", pmEntity->Name, this->Name));

    m_pmScene->RemoveEntity(pmEntity, true);
    CommandService->EndUndoFrame(bUndoable);
}
//---------------------------------------------------------------------------
void MPalette::RemoveAllEntities(bool bUndoable)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame(String::Format("Remove all entities from "
        "\"{0}\" palette", this->Name));

    m_pmScene->RemoveAllEntities(bUndoable);
    CommandService->EndUndoFrame(bUndoable);
}
//---------------------------------------------------------------------------
void MPalette::CopyComponentProperty(MComponent* pmSource, 
   String* strPropertyName, MComponent* pmDestination)
{
    PropertyType* pmPropertyType = pmSource->GetPropertyType(
        strPropertyName);
    String* strDisplayName = pmSource->GetPropertyDisplayName(
        strPropertyName);
    String* strPrimitiveType = pmPropertyType->PrimitiveType;
    String* strSemanticType = pmPropertyType->Name;
    bool bCollection = pmSource->IsCollection(strPropertyName);
    String* strDescription = pmSource->GetDescription(
        strPropertyName);
    pmDestination->AddProperty(strPropertyName, strDisplayName,
        strPrimitiveType, strSemanticType, bCollection, strDescription, true);
    if (pmDestination->CanResetProperty(strPropertyName))
    {
        pmDestination->ResetProperty(strPropertyName);
    }
}
//---------------------------------------------------------------------------
ArrayList* MPalette::GetAffectedEntities(MEntity* pmTemplateEntity, 
    MScene* pmScenes[])
{
    ArrayList* pmEntityList = new ArrayList();
    unsigned int uiSceneCount = pmScenes->Count;
    for (unsigned int ui = 0; ui < uiSceneCount; ui++)
    {
        MScene* pmScene = pmScenes[ui];
        MEntity* pmDependentEntities[] = 
            pmScene->GetDependentEntities(pmTemplateEntity);
        pmEntityList->AddRange(static_cast<Array*>(pmDependentEntities));
    }
    return pmEntityList;
}
//---------------------------------------------------------------------------
void MPalette::AddComponentPropertyToEntities(MComponent* pmComponent,
    String* strPropertyName, MScene* pmAffectedScenes[])
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    unsigned int uiEntityCount = pmEntities->Count;
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = pmEntities[ui];
        MComponent* pmEntityComponent = pmEntity->GetComponentByTemplateID(
            pmComponent->TemplateID);
        if (pmEntityComponent != NULL)
        {
            CopyComponentProperty(pmComponent, strPropertyName, 
                pmEntityComponent);
            pmEntity->Dirty = true;
            ArrayList* pmAffectedEntities = GetAffectedEntities(pmEntity,
                pmAffectedScenes);
            
            unsigned int pmAffectedEntityCount = pmAffectedEntities->Count;

            for ( unsigned int uiIndex = 0; uiIndex < pmAffectedEntityCount;
                uiIndex++)
            {
                MEntity* pmAffectedEntity = 
                    dynamic_cast<MEntity*>
                    (pmAffectedEntities->get_Item(uiIndex));
                MComponent* pmDependentEntityComponent =
                    pmAffectedEntity->GetComponentByTemplateID(
                    pmComponent->TemplateID);
                if (pmDependentEntityComponent != NULL)
                {
                    CopyComponentProperty(pmComponent, strPropertyName, 
                        pmDependentEntityComponent);
                    pmAffectedEntity->Dirty = true;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void MPalette::RemoveComponentPropertyFromEntities(MComponent* pmComponent,
    String* strPropertyName, MScene* pmAffectedScenes[])
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    unsigned int uiEntityCount = pmEntities->Count;
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = pmEntities[ui];
        MComponent* pmEntityComponent = pmEntity->GetComponentByTemplateID(
            pmComponent->TemplateID);
        if (pmEntityComponent != NULL &&
            pmEntityComponent->HasProperty(strPropertyName))
        {
            pmEntity->Dirty = true;
            pmEntityComponent->RemoveProperty(strPropertyName, true);
            ArrayList* pmAffectedEntities = GetAffectedEntities(pmEntity,
                pmAffectedScenes);
            
            unsigned int pmAffectedEntityCount = pmAffectedEntities->Count;

            for ( unsigned int uiIndex = 0; uiIndex < pmAffectedEntityCount;
                uiIndex++)
            {
                MEntity* pmAffectedEntity = 
                    dynamic_cast<MEntity*>
                    (pmAffectedEntities->get_Item(uiIndex));
                MComponent* pmDependentEntityComponent =
                    pmAffectedEntity->GetComponentByTemplateID(
                    pmComponent->TemplateID);
                if (pmDependentEntityComponent != NULL &&
                    pmDependentEntityComponent->HasProperty(strPropertyName))
                {
                    pmDependentEntityComponent->RemoveProperty(
                        strPropertyName, true);
                    pmAffectedEntity->Dirty = true;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
String* MPalette::GetEntityCategory(MEntity* pmEntity)
{
    MVerifyValidInstance;


    String* strFullName = StripPaletteName(pmEntity->Name);
    int iLastDotIndex = strFullName->LastIndexOf(".");
    if (iLastDotIndex == -1)
    {
        return String::Empty;
    }
    String* strCategoryName = strFullName->Substring(0, 
        iLastDotIndex);
    return strCategoryName;
}
//---------------------------------------------------------------------------
void MPalette::ResetEntityNames()
{
    MVerifyValidInstance;

    MEntity* pmFlatEntityArray[] = Scene->GetEntities();

    int iSize = pmFlatEntityArray->Count;
    String* strIdentifierString = String::Format("[{0}]",
        Name);

    CommandService->BeginUndoFrame("Reset template names");

    for (int i = 0; i < iSize; i++)
    {
        MEntity* pmEntity = pmFlatEntityArray[i];
        if (pmEntity->Name->StartsWith(strIdentifierString))
            continue;
        String* strNonPaletteName = MPalette::StripPaletteName(pmEntity->Name);
        String* newName = String::Format("{0}{1}", strIdentifierString,
            strNonPaletteName);
        pmEntity->Name = newName;
    }
    CommandService->EndUndoFrame(false);

}
//---------------------------------------------------------------------------
MEntity* MPalette::get_ActiveEntity()
{
    MVerifyValidInstance;

    return m_pmActiveEntity;
}
//---------------------------------------------------------------------------
void MPalette::set_ActiveEntity(MEntity* pmActiveEntity)
{
    MVerifyValidInstance;

    if (m_pmActiveEntity != pmActiveEntity)
    {
        MEntity* pmOldActiveEntity = m_pmActiveEntity;
        if (
            (pmActiveEntity != NULL) && 
            m_pmScene->IsEntityInScene(pmActiveEntity)            
            )
        {
            m_pmActiveEntity = pmActiveEntity;
            MEventManager::Instance->RaisePaletteActiveEntityChanged(this,
                pmOldActiveEntity);
        }
    }
}
//---------------------------------------------------------------------------
MEntity* MPalette::CreateTemplateFromFile(String* strFileName)
{
    MAssert(strFileName != NULL && strFileName != String::Empty, "Null or "
        "empty filename provided to function!");

    // Get filename.
    const char* pcFileName = MStringToCharPointer(strFileName);

    // Get base name.
    const char* pcBaseName = strrchr(pcFileName, '\\') + 1;

    // Copy base name to a new buffer.
    size_t stBufferSize = strlen(pcBaseName) + 1;
    char* pcEntityName = NiAlloc(char, stBufferSize);
    NiStrcpy(pcEntityName, stBufferSize, pcBaseName);

    // Clear off extension from base name.
    char* pcPtr = strrchr(pcEntityName, '.');
    *pcPtr = '\0';

    // Create new entity with name constructed above.
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterfaceIPtr spEntity = NiNew NiGeneralEntity(pcEntityName,
        kTemplateID, 2);

    // Free entity name buffer.
    NiFree(pcEntityName);

    // Add transformation component.
    spEntity->AddComponent(NiNew NiTransformationComponent());

    // Check file extension and add additional components.
    const char* pcExtension = strrchr(pcBaseName, '.') + 1;
    bool bAddedComponent = false;
    if (_stricmp(pcExtension, "nif") == 0)
    {
        NiSceneGraphComponent* pkSceneGraphComponent = NiNew
            NiSceneGraphComponent();
        pkSceneGraphComponent->SetNifFilePath(pcFileName);
        spEntity->AddComponent(pkSceneGraphComponent);
        bAddedComponent = true;
    }
    else if (_stricmp(pcExtension, "kfm") == 0)
    {
        NiActorComponent* pkActorComponent = NiNew NiActorComponent();
        pkActorComponent->SetKfmFilePath(pcFileName);
        spEntity->AddComponent(pkActorComponent);
        bAddedComponent = true;
    }

    // Free the filename pointer.
    MFreeCharPointer(pcFileName);

    // If no component was added, the file format is unknown. The entity
    // cannot be created.
    if (!bAddedComponent)
    {
        return NULL;
    }

    // Return the managed entity.
    return MEntityFactory::Instance->Get(spEntity);
}
//---------------------------------------------------------------------------
String* MPalette::StripPaletteName(String* strName)
{
    Regex* regex = new Regex(S"[\\[][^\\]]+[\\]]");
	Match* match = regex->Match(strName);
    if (match->Success)
    {
        return strName->Substring(match->Length);
    }
    else
    {
        return strName;
    }
}

//---------------------------------------------------------------------------
