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

#include "ObjectPickShader.h"

//---------------------------------------------------------------------------
ObjectPickShader::ObjectPickShader()
{
    //  Create the local stages and passes we will use...
    CreateStagesAndPasses();

    m_pkRenderStateGroup = NiD3DRenderStateGroup::GetFreeRenderStateGroup();
#if !defined(_XENON)
    m_pkRenderStateGroup->SetRenderState(D3DRS_LIGHTING, FALSE, true);
#endif
    m_pkRenderStateGroup->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE, true);
    m_pkRenderStateGroup->SetRenderState(D3DRS_ZWRITEENABLE, TRUE, true);
    m_pkRenderStateGroup->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE, true);
}
//---------------------------------------------------------------------------
ObjectPickShader::~ObjectPickShader()
{
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int ObjectPickShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiD3DShader::PreProcessPipeline(pkGeometry, pkSkin, pkBuffData, pkState,
        pkEffects, kWorld, kWorldBound);

    m_uiGeometryIndex = 0;

    return 0;
}
//---------------------------------------------------------------------------
unsigned int ObjectPickShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
    const NiBound& kWorldBound)
{
    NiD3DShader::SetupShaderPrograms(pkGeometry, pkSkin, pkPartition, 
        pkBuffData, pkState, pkEffects, kWorld, kWorldBound);

    unsigned int uiColor = m_kColorArray.GetAt(m_uiGeometryIndex);
    m_uiGeometryIndex++;

    // This is to eliminate issues with the alpha value read from the
    // back buffer. On some cards, it will be 0, on others 0xff...
    uiColor |= 0xff000000;

#if !defined(_XENON)
    m_pkD3DRenderState->SetRenderState(D3DRS_TEXTUREFACTOR, uiColor);
#endif

    return 0;
}
//---------------------------------------------------------------------------
unsigned int ObjectPickShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin, pkBuffData, 
        pkState, pkEffects, kWorld, kWorldBound);

    return 0;
}
//---------------------------------------------------------------------------
void ObjectPickShader::ResetGeometryIndex()
{
    m_uiGeometryIndex = 0;
}
//---------------------------------------------------------------------------
void ObjectPickShader::RegisterColor(unsigned int uiColor)
{
    m_kColorArray.SetAtGrow(m_uiGeometryIndex++, uiColor);
}
//---------------------------------------------------------------------------
bool ObjectPickShader::CreateStagesAndPasses()
{
    NiD3DPassPtr spPass = NiD3DPass::CreateNewPass();
    assert(spPass);

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    assert(spStage);

    //  Preset the stages
    PresetStages(spStage);

    spPass->AppendStage(spStage);
    m_kPasses.SetAt(0, spPass);
    m_uiPassCount = 1;

    return true;
}
//---------------------------------------------------------------------------
bool ObjectPickShader::PresetStages(NiD3DTextureStage* pkStage)
{
    pkStage->SetStage(0);
    pkStage->SetTexture(0);
    pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TFACTOR);
    pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
    pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 0);
    
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSU, 
        D3DTADDRESS_WRAP);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSV, 
        D3DTADDRESS_WRAP);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSW, 
        D3DTADDRESS_WRAP);
    
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MAGFILTER, 
        D3DTEXF_LINEAR);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MINFILTER, 
        D3DTEXF_LINEAR);
    pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MIPFILTER, 
        D3DTEXF_NONE);

    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    return true;
}
//---------------------------------------------------------------------------
void ObjectPickShader::ResetColorArray()
{
    m_kColorArray.RemoveAll();
}
//---------------------------------------------------------------------------
