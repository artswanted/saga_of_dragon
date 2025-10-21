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

#ifndef NiD3DShaderProgramCreatorHLSL_H
#define NiD3DShaderProgramCreatorHLSL_H

#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderProgramFactory.h"

class NID3D_ENTRY NiD3DShaderProgramCreatorHLSL : 
    public NiD3DShaderProgramCreator
{
public:
    virtual ~NiD3DShaderProgramCreatorHLSL();

    // Shader assembly flags
    virtual unsigned int GetShaderCreationFlags() const;
    virtual void SetShaderCreationFlags(unsigned int uiFlags);

    // *** begin Emergent internal use only ***

    // These functions should only be called from NiD3DShaderProgramFactory

    // Vertex shader creation
    virtual NiD3DVertexShader* CreateVertexShaderFromFile(
        const char* pcFileName, const char* pcShaderName, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        NiD3DVertexDeclaration hDecl, unsigned int uiUsage,
        bool bSoftwareVP = false, bool bRecoverable = false);
    virtual NiD3DVertexShader* CreateVertexShaderFromBuffer(
        const void* pvBuffer, unsigned int uiBufferSize, 
        const char* pcShaderName, const char* pcEntryPoint, 
        const char* pcShaderTarget, NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage, bool bSoftwareVP = false, 
        bool bRecoverable = false);
    virtual NiD3DVertexShader* CreateVertexShaderFromD3DXBuffer(
        LPD3DXBUFFER pkCode, unsigned int uiBufferSize,
        const char* pcShaderName, const char* pcEntryPoint, 
        const char* pcShaderTarget, NiD3DVertexDeclaration hDecl, 
        unsigned int uiUsage, bool bSoftwareVP = false, 
        bool bRecoverable = false);

    // Pixel  shader creation
    virtual NiD3DPixelShader* CreatePixelShaderFromFile(
        const char* pcFileName, const char* pcShaderName, 
        const char* pcEntryPoint, const char* pcShaderTarget, 
        bool bRecoverable = false);
    virtual NiD3DPixelShader* CreatePixelShaderFromBuffer(
        const void* pvBuffer, unsigned int uiBufferSize, 
        const char* pcShaderName, const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);
    virtual NiD3DPixelShader* CreatePixelShaderFromD3DXBuffer(
        LPD3DXBUFFER pkCode, unsigned int uiBufferSize, 
        const char* pcShaderName, const char* pcEntryPoint, 
        const char* pcShaderTarget, 
        bool bRecoverable = false);

    // Shader recreation
    virtual bool RecreateVertexShader(NiD3DVertexShader* pkVertexShader);
    virtual bool RecreatePixelShader(NiD3DPixelShader* pkPixelShader);

    static void _SDMInit();
    static void _SDMShutdown();

    static NiD3DShaderProgramCreatorHLSL* GetInstance();
    static void Shutdown();
    // *** end Emergent internal use only ***

protected:
    NiD3DShaderProgramCreatorHLSL();

    bool LoadShaderCodeFromFile(const char* pcFileName, 
        const char* pcEntryPoint, const char* pcShaderTarget, void*& pvCode, 
        unsigned int& uiCodeSize, LPD3DXCONSTANTTABLE& pkConstantTable,
        bool bRecoverable);
    bool LoadShaderCodeFromBuffer(const void* pvBuffer, 
        unsigned int uiBufferSize, const char* pcEntryPoint, 
        const char* pcShaderTarget, void*& pvCode, unsigned int& uiCodeSize, 
        LPD3DXCONSTANTTABLE& pkConstantTable, bool bRecoverable);

    unsigned int m_uiShaderCreationFlags;

    static NiD3DShaderProgramCreatorHLSL* ms_pkCreator;
};

#include "NiD3DShaderProgramCreatorHLSL.inl"

#endif  //NiD3DShaderProgramCreatorHLSL_H
