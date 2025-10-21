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
#include "MStandardRenderingMode.h"
//#include "..\\SceneDesignerFramework\\mframework.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MStandardRenderingMode::MStandardRenderingMode() : m_pkAlphaAccumulator(NULL),
    m_pkErrors(NULL)
{
    m_pkAlphaAccumulator = NiNew NiAlphaAccumulator();
    MInitRefObject(m_pkAlphaAccumulator);

    m_pkErrors = NiNew NiDefaultErrorHandler();
    MInitInterfaceReference(m_pkErrors);
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Do_Dispose(bool bDisposing)
{
	MDisposeInterfaceReference(m_pkErrors);
    MDisposeRefObject(m_pkAlphaAccumulator);
}
//---------------------------------------------------------------------------
String* MStandardRenderingMode::get_Name()
{
    MVerifyValidInstance;

    return "Standard";
}
//---------------------------------------------------------------------------
bool MStandardRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Update(float fTime)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Begin(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Clear out error handler.
    m_pkErrors->ClearErrors();

	// Set up the renderer's camera data.
    pkRenderingContext->m_pkRenderer->SetCameraData(
        pkRenderingContext->m_pkCamera);

    // Clear out visible array.
    pkRenderingContext->m_pkCullingProcess->GetVisibleSet()->RemoveAll();

	//	OnRenderFrameStart
}

//---------------------------------------------------------------------------
void MStandardRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::Render(MEntity* amEntities[],
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");
    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        MAssert(pmEntity != NULL, "Null entity in array!");

        pmEntity->GetNiEntityInterface()->BuildVisibleSet(pkRenderingContext,
            m_pkErrors);
    }
}
//---------------------------------------------------------------------------
void MStandardRenderingMode::End(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Set accumulator.
    NiAccumulatorPtr spOldAccumulator = pkRenderingContext->m_pkRenderer
        ->GetSorter();
    pkRenderingContext->m_pkRenderer->SetSorter(m_pkAlphaAccumulator);

    // Draw objects in the visible array.
    NiDrawVisibleArray(pkRenderingContext->m_pkCamera,
        *pkRenderingContext->m_pkCullingProcess->GetVisibleSet());

    // Restore accumulator.
    pkRenderingContext->m_pkRenderer->SetSorter(spOldAccumulator);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------

