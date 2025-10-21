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

#include "MFlatWireframeRenderingMode.h"
#include "MSettingsHelper.h"
#include <NiD3DShaderFactory.h>

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;
//---------------------------------------------------------------------------
MFlatWireframeRenderingMode::MFlatWireframeRenderingMode() :
    m_pkMaterial(NULL), m_pkErrors(NULL)
{
    m_pkErrors = NiNew NiDefaultErrorHandler();
    m_pkColor = NiNew NiColor(1.0f, 1.0f, 1.0f);
    m_pkPropState = NiNew NiPropertyState();
    m_pkPropState->IncRefCount();

    NiWireframeProperty* pkWireProp = NiNew NiWireframeProperty();
    pkWireProp->SetWireframe(true);
    NiMaterialProperty* pkMaterialProp = NiNew NiMaterialProperty();
    m_pkPropState->SetProperty(pkWireProp);
    m_pkPropState->SetProperty(pkMaterialProp);

    MInitInterfaceReference(m_pkErrors);
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::Do_Dispose(bool bDisposing)
{
    MDisposeRefObject(m_pkMaterial);
    MDisposeRefObject(m_pkPropState);
    NiDelete m_pkColor;
    MDisposeInterfaceReference(m_pkErrors);
}
//---------------------------------------------------------------------------
String* MFlatWireframeRenderingMode::get_Name()
{
    MVerifyValidInstance;

    return "Wireframe (Unshaded)";
}
//---------------------------------------------------------------------------
bool MFlatWireframeRenderingMode::get_DisplayToUser()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::Update(float fTime)
{
    MVerifyValidInstance;
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::Begin(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    if (!m_pkMaterial)
    {
        m_pkMaterial = NiMaterialLibrary::CreateMaterial(
            "NiFlatWireframeMaterial");
        MAssert(m_pkMaterial != NULL, "NiFlatWireframeMaterial is not found!");
        if (m_pkMaterial)
        {
            MInitRefObject(m_pkMaterial);
        }
    }

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    // Clear out error handler.
    m_pkErrors->ClearErrors();

    // Set up the renderer's camera data.
    pkRenderingContext->m_pkRenderer->SetCameraData(
        pkRenderingContext->m_pkCamera);

    // Clear out visible array.
    pkRenderingContext->m_pkCullingProcess->GetVisibleSet()->RemoveAll();
    
    NiColorA kWireframeColor(m_pkColor->r, m_pkColor->g, m_pkColor->b, 1.0f);
	bool bUpdateWire = NiD3DShaderFactory::UpdateGlobalShaderConstant("WireframeColor",
        sizeof(NiColorA), &kWireframeColor);
    if( false == bUpdateWire )
	{
		bool bRegisterWire = NiD3DShaderFactory::RegisterGlobalShaderConstant("WireframeColor", 
			NiShaderAttributeDesc::ATTRIB_TYPE_COLOR, sizeof(NiColorA), &kWireframeColor);
		bUpdateWire = NiD3DShaderFactory::UpdateGlobalShaderConstant("WireframeColor",
        sizeof(NiColorA), &kWireframeColor);
	}

    m_pkPropState->GetMaterial()->SetEmittance(*(m_pkColor));
}
void MFlatWireframeRenderingMode::BeginUnselected(MRenderingContext* pmRenderingContext)
{
	Begin(pmRenderingContext);
}

void MFlatWireframeRenderingMode::BeginSelected(MRenderingContext* pmRenderingContext)
{
	Begin(pmRenderingContext);
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::Render(MEntity* pmEntity,
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    MEntity* amEntities[] = {pmEntity};
    Render(amEntities, pmRenderingContext);
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::Render(MEntity* amEntities[],
    MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");
    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();

    NiTObjectArray<NiPropertyPtr> kOldProperties(0);
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        MAssert(pmEntity != NULL, "Null entity in array!");

        pmEntity->GetNiEntityInterface()->BuildVisibleSet(pkRenderingContext,
           m_pkErrors);
    }
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::End(MRenderingContext* pmRenderingContext)
{
    MVerifyValidInstance;

    MAssert(pmRenderingContext != NULL, "Null rendering context provided to "
        "function!");

    NiEntityRenderingContext* pkRenderingContext =
        pmRenderingContext->GetRenderingContext();
    NiVisibleArray* pkVisibleSet = pkRenderingContext->m_pkCullingProcess
        ->GetVisibleSet();
    assert(pkVisibleSet);

    if (m_pkMaterial)
    {
        const unsigned int uiQuantity = pkVisibleSet->GetCount();
        unsigned int ui;
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();

        for (ui = 0; ui < uiQuantity; ui++)
        {
            const NiMaterial* pkPrevMaterial = NULL;
            NiDynamicEffectStatePtr spPrevEffects = NULL;
            NiPropertyStatePtr spPrevProp = NULL;

            if (NiIsKindOf(NiTriBasedGeom, &pkVisibleSet->GetAt(ui)))
            {
                if (!pkVisibleSet->GetAt(ui).IsMaterialApplied(m_pkMaterial))
                {
                    pkVisibleSet->GetAt(ui).ApplyMaterial(m_pkMaterial);
                }
                
                pkPrevMaterial = pkVisibleSet->GetAt(ui).GetActiveMaterial();
                pkVisibleSet->GetAt(ui).SetActiveMaterial(m_pkMaterial);
            }
            else if (NiIsKindOf(NiParticles, &pkVisibleSet->GetAt(ui)))
            {
                spPrevProp = pkVisibleSet->GetAt(ui).GetPropertyState();
                spPrevEffects = pkVisibleSet->GetAt(ui).GetEffectState();

                pkVisibleSet->GetAt(ui).SetPropertyState(m_pkPropState);
                pkVisibleSet->GetAt(ui).SetEffectState(NULL);

            }

            pkVisibleSet->GetAt(ui).RenderImmediate(pkRenderer);

            pkVisibleSet->GetAt(ui).SetActiveMaterial(pkPrevMaterial);

            if (spPrevEffects)
                pkVisibleSet->GetAt(ui).SetEffectState(spPrevEffects);
            if (spPrevProp)
                pkVisibleSet->GetAt(ui).SetPropertyState(spPrevProp);
        }
    }


    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors,
        m_pkErrors);
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::RegisterSettings()
{
    SettingChangedHandler* pmHandler = new SettingChangedHandler(this,
        &MFlatWireframeRenderingMode::OnSettingChanged);

    
    MSettingsHelper::RegisterSetting("Unshaded Wireframe Color", 
        "Determines the wireframe color for any object rendered.", 
        "Viewport Settings.Unshaded Wireframe Color", *(m_pkColor), 
        SettingsCategory::PerUser, true, pmHandler);
}
//---------------------------------------------------------------------------
void MFlatWireframeRenderingMode::OnSettingChanged(Object* pmSender,
    SettingChangedEventArgs* pmEventArgs)
{
    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    ISettingsService* pmSettingsService = MGetService(
        ISettingsService);

    Object* pmObj = pmSettingsService->GetSettingsObject(
            strSetting, eCategory);
    __box Color* pmValue;
    pmValue = dynamic_cast<__box Color*>(pmObj);
    if (pmValue != NULL)
    {
        m_pkColor->r = MUtility::RGBToFloat((*pmValue).R);
        m_pkColor->g = MUtility::RGBToFloat((*pmValue).G);
        m_pkColor->b = MUtility::RGBToFloat((*pmValue).B);
    }
}
//---------------------------------------------------------------------------
