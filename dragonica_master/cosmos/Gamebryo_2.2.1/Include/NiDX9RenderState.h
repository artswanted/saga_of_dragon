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
//---------------------------------------------------------------------------
#ifndef NIDX9RENDERSTATE_H
#define NIDX9RENDERSTATE_H

#include "NiD3DRenderState.h"
#include "NiDX9Headers.h"
#include "NiD3DPass.h"
#include "NiDX9RendererLibType.h"

class NID3D_ENTRY NiDX9RenderState : public NiD3DRenderState
{
protected:
    NiDX9RenderState(NiD3DRenderer* pkRenderer, const D3DCAPS9& kD3DCaps9);

public:
    virtual ~NiDX9RenderState();

    static NiDX9RenderState* Create(NiD3DRenderer* pkRenderer, 
        const D3DCAPS9& kD3DCaps9, bool bZBuffer);

    virtual void PurgeMaterial(NiMaterialProperty* pkMaterial);

    // Shader program management
    virtual void InitShaderState();
    virtual void SaveShaderState();
    virtual void RestoreShaderState();

    // Vertex declarations
    virtual void SetVertexShader(NiD3DVertexShaderHandle hShader, 
        bool bSave = false);
    virtual NiD3DVertexShaderHandle GetVertexShader() const;
    virtual void RestoreVertexShader();
    virtual void ClearVertexShader(NiD3DVertexShaderHandle hShader);

    virtual void SetFVF(unsigned int uiFVF, bool bSave = false);
    virtual unsigned int GetFVF() const;
    virtual void RestoreFVF();
    virtual void ClearFVF(unsigned int uiFVF);

    virtual void SetDeclaration(NiD3DVertexDeclaration hDeclaration, 
        bool bSave = false);
    virtual NiD3DVertexDeclaration GetDeclaration() const;
    virtual void RestoreDeclaration();
    virtual void ClearDeclaration(NiD3DVertexDeclaration hDeclaration);

    // Texture stage and sampler states
    virtual void SetSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState, unsigned int uiValue, 
        bool bSave = false);
    virtual unsigned int GetSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState);
    virtual void RestoreSamplerState(unsigned int uiStage, 
        NiD3DSamplerState eState);

    void SetSamplerState(unsigned int uiStage, D3DSAMPLERSTATETYPE eState, 
        unsigned int uiValue, bool bSave = false);
    unsigned int GetSamplerState(unsigned int uiStage, 
        D3DSAMPLERSTATETYPE eState);
    void RestoreSamplerState(unsigned int uiStage, D3DSAMPLERSTATETYPE eState);

    virtual void SetSoftwareVertexProcessing(bool bSoftwareVP);
    virtual bool GetSoftwareVertexProcessing() const;

    // *** begin Emergent internal use only ***
    virtual void InitRenderModes();
    // *** end Emergent internal use only ***

protected:
    virtual void ReadRenderStateCaps();

    // Vertex formats
    bool m_bDeclaration;
    unsigned int m_uiCurrentFVF;
    unsigned int m_uiPreviousFVF;
    NiD3DVertexDeclaration m_hCurrentVertexDeclaration;
    NiD3DVertexDeclaration m_hPreviousVertexDeclaration;

    // Software vertex processing
    bool m_bUsingSoftwareVP;

    D3DCAPS9 m_kD3DCaps9;

    static unsigned int ms_auiSamplesStatesRevMappings[14];
};

typedef NiPointer<NiDX9RenderState> NiDX9RenderStatePtr;

#include "NiDX9RenderState.inl"

#endif  //#ifndef NIDX9RENDERSTATE_H
