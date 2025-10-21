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

#include "MPickUtility.h"
#include "MEntity.h"
#include "MRenderer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPickUtility::MPickUtility() : m_pkPick(NULL), m_pkSceneRootPointerName(NULL),
    m_pkColorPicker(NULL)
{
    m_pkPick = NiNew NiPick();
    m_pkPick->SetReturnNormal(true);
    //m_pkPick->SetObserveAppCullFlag(true);
	m_pkPick->SetPickType(NiPick::FIND_ALL);
    m_pkSceneRootPointerName = NiNew NiFixedString("Scene Root Pointer");
    m_pmPickedObjectToEntity = new Hashtable();
}
//---------------------------------------------------------------------------
void MPickUtility::Do_Dispose(bool bDisposing)
{
    NiDelete m_pkColorPicker;

    NiDelete m_pkSceneRootPointerName;
    NiDelete m_pkPick;
}
//---------------------------------------------------------------------------
bool MPickUtility::PerformPick(MScene* pmScene, const NiPoint3& kOrigin,
    const NiPoint3& kDir, bool bIncludeFrozenEntities)
{
    MVerifyValidInstance;

    MAssert(pmScene != NULL, "Null scene provided to function!");

    m_pmPickedObjectToEntity->Clear();
    m_pkPick->ClearResultsArray();

    bool bFoundIntersection = false;
    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];

        // Do not pick on hidden entities. Only pick on frozen entities if
        // requested.
        if (!pmEntity->Hidden && (bIncludeFrozenEntities || !pmEntity->Frozen))
        {
            bFoundIntersection = PickOnEntityRecursive(pmEntity, kOrigin, kDir, pmEntity) || bFoundIntersection;
        }
    }

    return bFoundIntersection;
}
//---------------------------------------------------------------------------
bool MPickUtility::PickOnEntityRecursive(MEntity* pmEntity,
    const NiPoint3& kOrigin, const NiPoint3& kDir, MEntity* pmMainEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    bool bFoundIntersection = false;

    unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
    for (unsigned int uiSceneRoot = 0; uiSceneRoot < uiSceneRootCount; uiSceneRoot++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(uiSceneRoot);
        if (pkSceneRoot)
        {
            m_pkPick->SetTarget(pkSceneRoot);
            bFoundIntersection = m_pkPick->PickObjects(kOrigin, kDir, true) || bFoundIntersection;

            const NiPick::Results& kResults = m_pkPick->GetResults();
            for (unsigned int ui = 0; ui < kResults.GetSize(); ui++)
            {
                NiPick::Record* pkRecord = kResults.GetAt(ui);
                if (pkRecord)
                {
                    Object* pmObj = __box((unsigned int)
                        pkRecord->GetAVObject());
                    if (!m_pmPickedObjectToEntity->Contains(pmObj))
                    {
                        m_pmPickedObjectToEntity->Item[pmObj] = pmMainEntity;
                    }
                }
            }
        }
    }

    return bFoundIntersection;
}
//---------------------------------------------------------------------------
const NiPick* MPickUtility::GetNiPick()
{
    MVerifyValidInstance;

    return m_pkPick;
}
//---------------------------------------------------------------------------
MEntity* MPickUtility::GetEntityFromPickedObject(
    NiAVObject* pkPickedObject)
{
    MVerifyValidInstance;

    return dynamic_cast<MEntity*>(m_pmPickedObjectToEntity->Item[
        __box((unsigned int) pkPickedObject)]);
}
//---------------------------------------------------------------------------
MEntity* MPickUtility::GetEntityFromViewCoordinates(MScene* pmScene,
    MViewport* pmViewport, int iX, int iY)
{
        NiPoint3 kOrigin, kDir;
        NiViewMath::MouseToRay((float) iX, (float) iY, 
            pmViewport->Width, pmViewport->Height,
            pmViewport->GetNiCamera(), kOrigin, kDir);

        PerformPick(pmScene, kOrigin, kDir, false);

        const NiPick* pkPick = GetNiPick();

        const NiPick::Results& kPickResults = pkPick->GetResults();

        if (kPickResults.GetSize() > 0)
        {        
            NiPick::Record* pkPickRecord = kPickResults.GetAt(0);
            if (pkPickRecord)
            {
                NiAVObject* pkPickedObject = pkPickRecord->GetAVObject();
                return GetEntityFromPickedObject(pkPickedObject);
            }
        }

        return NULL;

}
//---------------------------------------------------------------------------
MEntity* MPickUtility::PerformColorPick(MScene* pmScene, MEntity* pmCamera,
    unsigned int uiPickX, unsigned int uiPickY, bool bIncludeFrozenEntities)
{
    MVerifyValidInstance;

    MAssert(pmScene != NULL, "Null scene provided to function!");
    MAssert(pmCamera != NULL, "Null camera provided to function!");

    NiAVObject* pkSceneRoot = pmCamera->GetSceneRootPointer(0);
    NiCamera* pkCamera = NiDynamicCast(NiCamera, pkSceneRoot);
    if (!pkCamera)
    {
        return NULL;
    }

    // The color picker is created here because the renderer is accessed in
    // its constructor and must exist. We are guaranteed that the renderer
    // exists when this function is called.
    if (!m_pkColorPicker)
    {
        m_pkColorPicker = NiNew ColorPicker();
    }

    m_pkColorPicker->StartPick(pkCamera, uiPickX, uiPickY,
        MRenderer::Instance->Width, MRenderer::Instance->Height);

    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];

        // Do not pick on hidden entities. Only pick on frozen entities if
        // requested.
        if (!pmEntity->Hidden &&
            (bIncludeFrozenEntities || !pmEntity->Frozen))
        {
            ColorPickOnEntityRecursive(pmEntity, i);
        }
    }

    unsigned int uiPicked = m_pkColorPicker->EndPick();
    if (uiPicked < (unsigned int) amEntities->Length)
    {
        return amEntities[uiPicked];
    }

    return NULL;
}
//---------------------------------------------------------------------------
void MPickUtility::ColorPickOnEntityRecursive(MEntity* pmEntity,
    unsigned int uiColor)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    for (unsigned int ui = 0; ui < pmEntity->GetSceneRootPointerCount();
        ui++)
    {
        NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
        if (pkSceneRoot)
        {
            m_pkColorPicker->PickRender(uiColor, pkSceneRoot);
        }
    }
}
//---------------------------------------------------------------------------
bool MPickUtility::GetObserveAppCullFlag()
{
	return m_pkPick->GetObserveAppCullFlag();
}
//---------------------------------------------------------------------------
void MPickUtility::SetObserveAppCullFlag(bool bFlag)
{
	m_pkPick->SetObserveAppCullFlag(bFlag);
}
//---------------------------------------------------------------------------
