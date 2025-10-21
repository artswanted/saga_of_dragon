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

#include "NiProxyComponent.h"

NiFixedString NiProxyComponent::ms_kProxyComponentName;
NiFixedString NiProxyComponent::ms_kProxyComponentClassName;
float NiProxyComponent::ms_fDefaultDistance;

//---------------------------------------------------------------------------
void NiProxyComponent::_SDMInit()
{
    ms_kProxyComponentName = "Proxy Component";
    ms_kProxyComponentClassName = "NiProxyComponent";
    ms_fDefaultDistance = 30.0f;
}
//---------------------------------------------------------------------------
void NiProxyComponent::_SDMShutdown()
{
    ms_kProxyComponentName = NULL;
    ms_kProxyComponentClassName = NULL;
}
//---------------------------------------------------------------------------
NiProxyComponent::NiProxyComponent() : NiInheritedTransformationComponent(),
    m_pkParentEntity(NULL)
{
    SetInheritTranslation(true);
    SetInheritRotation(true);
    SetInheritScale(false);
}
//---------------------------------------------------------------------------
NiProxyComponent::NiProxyComponent(NiEntityInterface* pkMasterEntity,
    const NiFixedString& kAttachmentPointName) :
    NiInheritedTransformationComponent(pkMasterEntity, kAttachmentPointName,
    true, true, false), m_pkParentEntity(NULL)
{
}
//---------------------------------------------------------------------------
void NiProxyComponent::UpdateScale(NiCamera* pkCamera)
{
    assert(pkCamera);

    // Set the scale based off of camera distance to the proxy.
    float fScale;
    if (pkCamera->GetViewFrustum().m_bOrtho)
    {
        fScale = (pkCamera->GetViewFrustum().m_fRight * 2.0f) /
            ms_fDefaultDistance;
    }
    else
    {
        NiPoint3 kTranslation;
        NiBool bSuccess = GetPropertyData(ms_kTranslationName, kTranslation,
            0);
        assert(bSuccess);

        float fCamDistance = (kTranslation -
            pkCamera->GetWorldTranslate()).Length();
        if ((fCamDistance / ms_fDefaultDistance) > 0.0f)
        {
            fScale = (fCamDistance / ms_fDefaultDistance) *
                pkCamera->GetViewFrustum().m_fRight * 2.0f;
        }
        else
        {
            fScale = 0.0f;
        }
    }
    NiBool bSuccess = SetPropertyData(ms_kScaleName, fScale, 0);
    assert(bSuccess);

    if (m_pkParentEntity)
    {
        NiObject* pkObject;
        m_pkParentEntity->GetPropertyData(ms_kSceneRootPointerName,
            pkObject);
        NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject, pkObject);
        if (pkSceneRoot)
        {
            pkSceneRoot->SetScale(fScale);
            pkSceneRoot->Update(0.0f, false);
        }
    }
}
//---------------------------------------------------------------------------
NiFixedString NiProxyComponent::ClassName()
{
    return ms_kProxyComponentClassName;
}
//---------------------------------------------------------------------------
// NiInheritedTransformationComponent overrides
//---------------------------------------------------------------------------
NiBool NiProxyComponent::SetTemplateID(const NiUniqueID& kTemplateID)
{
    // This component does not support setting the template ID.
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID NiProxyComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID(0x4D, 0x85, 0xF9, 0xAC, 0xE8, 0x28,
        0x51, 0x4D, 0x93, 0xE0, 0xD, 0xB4, 0x2A, 0xB6, 0x6, 0x73);
    return kUniqueID;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiProxyComponent::Clone(
    bool bInheritProperties)
{
    return NiNew NiProxyComponent(m_pkSourceEntity, m_kAttachmentPointName);
}
//---------------------------------------------------------------------------
NiFixedString NiProxyComponent::GetName() const
{
    return ms_kProxyComponentName;
}
//---------------------------------------------------------------------------
NiFixedString NiProxyComponent::GetClassName() const
{
    return ms_kProxyComponentClassName;
}
//---------------------------------------------------------------------------
void NiProxyComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    NiInheritedTransformationComponent::Update(pkParentEntity, fTime,
        pkErrors, pkAssetManager);
    m_pkParentEntity = pkParentEntity;
}
//---------------------------------------------------------------------------
void NiProxyComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface* pkErrors)
{
    UpdateScale(pkRenderingContext->m_pkCamera);
}
//---------------------------------------------------------------------------
