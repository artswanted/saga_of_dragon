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
//  NiDX9RenderState inline functions
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetVertexShader(
    NiD3DVertexShaderHandle hShader, bool bSave)
{
    if (bSave)
        m_hPreviousVertexShader = m_hCurrentVertexShader;
    if (m_hCurrentVertexShader != hShader)
    {
        m_hCurrentVertexShader = hShader;
        m_pkD3DDevice->SetVertexShader(m_hCurrentVertexShader);
		NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_SHADER_CHANGES, 1);
    }
}
//---------------------------------------------------------------------------
inline NiD3DVertexShaderHandle NiDX9RenderState::GetVertexShader() const
{
    return m_hCurrentVertexShader;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreVertexShader()
{
    SetVertexShader(m_hPreviousVertexShader);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearVertexShader(
    NiD3DVertexShaderHandle hShader)
{
    if (m_hCurrentVertexShader == hShader)
    {
        // We need to clear this!
        m_hCurrentVertexShader = 0;
        m_pkD3DDevice->SetVertexShader(0);
		NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_SHADER_CHANGES, 1);
    }

    if (m_hPreviousVertexShader == hShader)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // if happening
        m_hPreviousVertexShader = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetFVF(unsigned int uiFVF, bool bSave)
{
    if (uiFVF != 0 && (m_bDeclaration || m_uiCurrentFVF != uiFVF))
    {
        m_bDeclaration = false;
        if (bSave)
            m_uiPreviousFVF = m_uiCurrentFVF;
        m_uiCurrentFVF = uiFVF;
        m_pkD3DDevice->SetFVF(m_uiCurrentFVF);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetFVF() const
{
    if (m_bDeclaration)
        return 0;
    else
        return m_uiCurrentFVF;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreFVF()
{
    SetFVF(m_uiPreviousFVF);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearFVF(unsigned int uiFVF)
{
    if (uiFVF == 0 || m_bDeclaration)
        return;

    if (m_uiCurrentFVF == uiFVF)
    {
        // We need to clear this!
        m_uiCurrentFVF = D3DFVF_XYZ;
        m_pkD3DDevice->SetFVF(D3DFVF_XYZ);
    }

    if (m_uiPreviousFVF == uiFVF)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // was happening
        m_uiPreviousFVF = D3DFVF_XYZ;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetDeclaration(
    NiD3DVertexDeclaration hDeclaration, bool bSave)
{
    if (hDeclaration != NULL && 
        (!m_bDeclaration || m_hCurrentVertexDeclaration != hDeclaration))
    {
        m_bDeclaration = true;
        if (bSave)
            m_hPreviousVertexDeclaration = m_hCurrentVertexDeclaration;
        m_hCurrentVertexDeclaration = hDeclaration;
        m_pkD3DDevice->SetVertexDeclaration(m_hCurrentVertexDeclaration);
    }
}
//---------------------------------------------------------------------------
inline NiD3DVertexDeclaration NiDX9RenderState::GetDeclaration() const
{
    if (m_bDeclaration)
        return m_hCurrentVertexDeclaration;
    else
        return NULL;
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreDeclaration()
{
    SetDeclaration(m_hPreviousVertexDeclaration);
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::ClearDeclaration(
    NiD3DVertexDeclaration hDeclaration)
{
    if (hDeclaration == NULL || !m_bDeclaration)
        return;

    if (m_hCurrentVertexDeclaration == hDeclaration)
    {
        // We need to clear this!
        m_hCurrentVertexDeclaration = 0;
        m_pkD3DDevice->SetVertexDeclaration(0);
    }

    if (m_hPreviousVertexDeclaration == hDeclaration)
    {
        // This is likely a bad thing to have happening
        // If someone calls restore on this, they will have lost what
        // if happening
        m_hPreviousVertexDeclaration = 0;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState, unsigned int uiValue, bool bSave)
{
    if (m_akSamplerStateSettings[uiStage][eState].GetValue() != uiValue)
    {   
        m_akSamplerStateSettings[uiStage][eState].SetValue(uiValue, bSave);
        D3DSAMPLERSTATETYPE eD3DState = 
            (D3DSAMPLERSTATETYPE)ms_auiSamplesStateMappings[eState];
        m_pkD3DDevice->SetSamplerState(uiStage, eD3DState, uiValue);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState)
{
    return m_akSamplerStateSettings[uiStage][eState].GetValue();
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreSamplerState(unsigned int uiStage, 
    NiD3DSamplerState eState)
{
    SetSamplerState(uiStage, eState, 
        m_akSamplerStateSettings[uiStage][eState].GetPrevValue());
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::SetSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState, unsigned int uiValue, bool bSave)
{
    NiD3DSamplerState eNiState = 
        (NiD3DSamplerState)ms_auiSamplesStatesRevMappings[eState];
    if (m_akSamplerStateSettings[uiStage][eNiState].GetValue() != uiValue)
    {   
        m_akSamplerStateSettings[uiStage][eNiState].SetValue(uiValue, bSave);
        m_pkD3DDevice->SetSamplerState(uiStage, eState, uiValue);
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9RenderState::GetSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState)
{
    NiD3DSamplerState eNiState = 
        (NiD3DSamplerState)ms_auiSamplesStatesRevMappings[eState];
    return m_akSamplerStateSettings[uiStage][eNiState].GetValue();
}
//---------------------------------------------------------------------------
inline void NiDX9RenderState::RestoreSamplerState(unsigned int uiStage, 
    D3DSAMPLERSTATETYPE eState)
{
    NiD3DSamplerState eNiState = 
        (NiD3DSamplerState)ms_auiSamplesStatesRevMappings[eState];
    SetSamplerState(uiStage, eNiState, 
        m_akSamplerStateSettings[uiStage][eNiState].GetPrevValue());
}
//---------------------------------------------------------------------------
