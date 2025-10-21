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

//Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "TexColorNoAlphaShader.h"

//---------------------------------------------------------------------------
TexColorNoAlphaShader::TexColorNoAlphaShader() :
    m_pkStage(NULL)
{
    //  Create the local stages and passes we will use...
    CreateStagesAndPasses();
    SetName("TexColorNoAlphaShader");

    // This is the best (and only) implementation of this shader
    m_bIsBestImplementation = true;
}
//---------------------------------------------------------------------------
TexColorNoAlphaShader::~TexColorNoAlphaShader()
{
    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int TexColorNoAlphaShader::UpdatePipeline(NiGeometry* pkGeometry,
    const NiSkinInstance* pkSkin,
    NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    assert (pkState);
    NiTexturingProperty* pkTexProp = pkState->GetTexturing();
    assert (pkTexProp);
    NiTexture* pkTexture = pkTexProp->GetBaseTexture();
    assert (pkTexture);
    
    m_pkStage->SetTexture(pkTexture);

    return NiD3DShader::UpdatePipeline(pkGeometry, pkSkin, pkRendererData, 
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
bool TexColorNoAlphaShader::CreateStagesAndPasses()
{
    NiD3DPassPtr spPass = NiD3DPass::CreateNewPass();
    assert(spPass);

    spPass->SetRenderState(D3DRS_TEXTUREFACTOR, 0, true);
    spPass->SetRenderState(D3DRS_ZENABLE, 0, true);
    spPass->SetRenderState(D3DRS_ZWRITEENABLE, 0, true);
    spPass->SetRenderState(D3DRS_ALPHABLENDENABLE, 0, true);

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    assert(spStage);
    m_pkStage = spStage;

    m_pkStage->SetStage(0);
    m_pkStage->SetTexture(0);
    m_pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);

    m_pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
    m_pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 0);

    m_pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSU, 
        D3DTADDRESS_CLAMP);
    m_pkStage->SetSamplerState(NiD3DRenderState::NISAMP_ADDRESSV, 
        D3DTADDRESS_CLAMP);

    m_pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MAGFILTER, 
        D3DTEXF_POINT);
    m_pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MINFILTER, 
        D3DTEXF_POINT);
    m_pkStage->SetSamplerState(NiD3DRenderState::NISAMP_MIPFILTER, 
        D3DTEXF_NONE);

    m_pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

    spPass->AppendStage(m_pkStage);
    m_kPasses.SetAt(0, spPass);
    m_uiPassCount = 1;

    return true;
}
//---------------------------------------------------------------------------
