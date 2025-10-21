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

#ifndef NSBPASS_H
#define NSBPASS_H

#include "NiBinaryShaderLibLibType.h"
#include "NSBRenderStates.h"
#include "NSBConstantMap.h"
#include "NSBUserDefinedDataSet.h"

#include <NiTArray.h>

class NSBTextureStage;
class NSBStateGroup;
class NiD3DPass;

class NIBINARYSHADERLIB_ENTRY NSBPass : public NiMemObject
{
public:
    NSBPass();
    ~NSBPass();

    // Name
    const char* GetName() const;
    void SetName(const char* pcName);

    // RenderStateGroup
    NSBStateGroup* GetRenderStateGroup();

    // Shader Programs
    const char* GetPixelShaderProgramFile() const;
    void SetPixelShaderProgramFileXenon(const char* pcProgramFileName);
    void SetPixelShaderProgramFileDX9(const char* pcProgramFileName);
    void SetPixelShaderProgramFilePS3(const char* pcProgramFileName);
    const char* GetPixelShaderProgramEntryPoint() const;
    void SetPixelShaderProgramEntryPoint(const char* pcEntryPoint);
    const char* GetPixelShaderProgramShaderTarget() const;
    void SetPixelShaderProgramShaderTarget(const char* pcShaderTarget);
    bool GetPixelShaderPresent() const;

    const char* GetVertexShaderProgramFile() const;
    void SetVertexShaderProgramFileXenon(const char* pcProgramFileName);
    void SetVertexShaderProgramFileDX9(const char* pcProgramFileName);
    void SetVertexShaderProgramFilePS3(const char* pcProgramFileName);
    const char* GetVertexShaderProgramEntryPoint() const;
    void SetVertexShaderProgramEntryPoint(const char* pcEntryPoint);
    const char* GetVertexShaderProgramShaderTarget() const;
    void SetVertexShaderProgramShaderTarget(const char* pcShaderTarget);
    bool GetVertexShaderPresent() const;
    bool GetSoftwareVertexProcessing() const;
    void SetSoftwareVertexProcessing(bool bSWVertexProcessing);

    // Constant Maps
    NSBConstantMap* GetPixelConstantMap();
    NSBConstantMap* GetVertexConstantMap();

    // Stages
    unsigned int GetStageCount();
    NSBTextureStage* GetStage(unsigned int uiIndex, bool bCreate = true);

    // User defined data set
    NSBUserDefinedDataSet* GetUserDefinedDataSet();
    void SetUserDefinedDataSet(NSBUserDefinedDataSet* pkUDDSet);

    // *** begin Emergent internal use only
    bool SetupNiD3DPass(NiD3DPass& kPass, NiShaderDesc* pkShaderDesc);

    bool SaveBinary(NiBinaryStream& kStream);
    bool LoadBinary(NiBinaryStream& kStream);

#if defined(_DEBUG)
    void Dump(FILE* pf);
#endif  //#if defined(_DEBUG)
    // *** end Emergent internal use only

protected:
    bool SaveBinaryPixelShaderProgram(NiBinaryStream& kStream);
    bool SaveBinaryVertexShaderProgram(NiBinaryStream& kStream);
    bool SaveBinaryStages(NiBinaryStream& kStream);

    bool LoadBinaryPixelShaderProgram(NiBinaryStream& kStream);
    bool LoadBinaryVertexShaderProgram(NiBinaryStream& kStream);
    bool LoadBinaryStages(NiBinaryStream& kStream);

    char* m_pcName;

    // User defined data set
    NSBUserDefinedDataSetPtr m_spUserDefinedDataSet;

    // 'Local' render state settings for the shader
    NSBStateGroup* m_pkRenderStateGroup;
    // 'Local' pixel shader mapping for the shader
    NSBConstantMap* m_pkPixelConstantMap;
    // 'Local' vertex shader mapping for the shader
    NSBConstantMap* m_pkVertexConstantMap;

    // Shader Programs
    char* m_pcPixelShaderProgramFileXenon;
    char* m_pcPixelShaderProgramFileDX9;
    char* m_pcPixelShaderProgramFilePS3;
    char* m_pcPSProgramEntryPoint;
    char* m_pcPSProgramTarget;

    char* m_pcVertexShaderProgramFileXenon;
    char* m_pcVertexShaderProgramFileDX9;
    char* m_pcVertexShaderProgramFilePS3;
    char* m_pcVSProgramEntryPoint;
    char* m_pcVSProgramTarget;

    bool m_bSoftwareVP;

    NiTPrimitiveArray<NSBTextureStage*> m_akStages;
};

#include "NSBPass.inl"

#endif  //NSBPASS_H
