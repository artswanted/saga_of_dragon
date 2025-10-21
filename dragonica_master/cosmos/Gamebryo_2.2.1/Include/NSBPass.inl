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
// NSBPass inline functions
//---------------------------------------------------------------------------
inline const char* NSBPass::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetPixelShaderProgramFile() const
{
#if defined(_XENON)
    return m_pcPixelShaderProgramFileXenon;
#elif defined(_DX9)
    return m_pcPixelShaderProgramFileDX9;
#elif defined(_PS3)
    return m_pcPixelShaderProgramFilePS3;
#endif  //#if defined(_XENON)
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetVertexShaderProgramFile() const
{
#if defined(_XENON)
    return m_pcVertexShaderProgramFileXenon;
#elif defined(_DX9)
    return m_pcVertexShaderProgramFileDX9;
#elif defined(_PS3)
    return m_pcVertexShaderProgramFilePS3;
#endif  //#if defined(_XENON)
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetPixelShaderProgramEntryPoint() const
{
    return m_pcVSProgramEntryPoint;
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetPixelShaderProgramShaderTarget() const
{
    return m_pcVSProgramTarget;
}
//---------------------------------------------------------------------------
inline bool NSBPass::GetPixelShaderPresent() const
{
    return (m_pcPixelShaderProgramFileXenon != NULL ||
        m_pcPixelShaderProgramFileDX9 != NULL);
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetVertexShaderProgramEntryPoint() const
{
    return m_pcVSProgramEntryPoint;
}
//---------------------------------------------------------------------------
inline const char* NSBPass::GetVertexShaderProgramShaderTarget() const
{
    return m_pcVSProgramTarget;
}
//---------------------------------------------------------------------------
inline bool NSBPass::GetVertexShaderPresent() const
{
    return (m_pcVertexShaderProgramFileXenon != NULL ||
        m_pcVertexShaderProgramFileDX9 != NULL);
}
//---------------------------------------------------------------------------
inline bool NSBPass::GetSoftwareVertexProcessing() const
{
    return m_bSoftwareVP;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetSoftwareVertexProcessing(bool bSoftwareVP)
{
    m_bSoftwareVP = bSoftwareVP;
}
//---------------------------------------------------------------------------
inline NSBUserDefinedDataSet* NSBPass::GetUserDefinedDataSet()
{
    return m_spUserDefinedDataSet;
}
//---------------------------------------------------------------------------
inline void NSBPass::SetUserDefinedDataSet(NSBUserDefinedDataSet* pkUDDSet)
{
    m_spUserDefinedDataSet = pkUDDSet;
}
//---------------------------------------------------------------------------
