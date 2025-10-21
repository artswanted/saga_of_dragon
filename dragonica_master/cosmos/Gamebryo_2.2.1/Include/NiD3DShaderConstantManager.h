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

#ifndef NID3DSHADERCONSTANTMANAGER_H
#define NID3DSHADERCONSTANTMANAGER_H

#include "NiD3DDefines.h"
#include <NiRefObject.h>
#include <NiSmartPointer.h>

class NID3D_ENTRY NiD3DShaderConstantManager : public NiRefObject
{
public:
    virtual ~NiD3DShaderConstantManager();

    bool SetVertexShaderConstantB(unsigned int uiStartRegister,
        const BOOL* pbConstantData, unsigned int uiBoolCount, 
        bool bSave = false);
    bool GetVertexShaderConstantB(unsigned int uiStartRegister,
        BOOL* pbConstantData, unsigned int uiBoolCount);
    bool RestoreVertexShaderConstantB(unsigned int uiStartRegister, 
        unsigned int uiBoolCount);
    bool SetVertexShaderConstantF(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetVertexShaderConstantF(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestoreVertexShaderConstantF(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);
    bool SetVertexShaderConstantI(unsigned int uiStartRegister,
        const int* piConstantData, unsigned int uiInt4Count, 
        bool bSave = false);
    bool GetVertexShaderConstantI(unsigned int uiStartRegister,
        int* piConstantData, unsigned int uiInt4Count);
    bool RestoreVertexShaderConstantI(unsigned int uiStartRegister, 
        unsigned int uiInt4Count);

    bool SetPixelShaderConstantB(unsigned int uiStartRegister,
        const BOOL* pbConstantData, unsigned int uiBoolCount, 
        bool bSave = false);
    bool GetPixelShaderConstantB(unsigned int uiStartRegister,
        BOOL* pbConstantData, unsigned int uiBoolCount);
    bool RestorePixelShaderConstantB(unsigned int uiStartRegister, 
        unsigned int uiBoolCount);
    bool SetPixelShaderConstantF(unsigned int uiStartRegister,
        const float* pfConstantData, unsigned int uiFloat4Count, 
        bool bSave = false);
    bool GetPixelShaderConstantF(unsigned int uiStartRegister,
        float* pfConstantData, unsigned int uiFloat4Count);
    bool RestorePixelShaderConstantF(unsigned int uiStartRegister, 
        unsigned int uiFloat4Count);
    bool SetPixelShaderConstantI(unsigned int uiStartRegister,
        const int* piConstantData, unsigned int uiInt4Count, 
        bool bSave = false);
    bool GetPixelShaderConstantI(unsigned int uiStartRegister,
        int* piConstantData, unsigned int uiInt4Count);
    bool RestorePixelShaderConstantI(unsigned int uiStartRegister, 
        unsigned int uiInt4Count);

    // *** begin Emergent internal use only ***

    virtual void CommitChanges() = 0;

    // *** end Emergent internal use only ***

protected:
    NiD3DShaderConstantManager(NiD3DRenderer* pkD3DRenderer);

    // Floating-point registers
    float* m_pfFloatVSConstantData;
    float* m_pfSavedFloatVSConstantData;
    float* m_pfFloatPSConstantData;
    float* m_pfSavedFloatPSConstantData;

    unsigned int m_uiFirstDirtyFloatVSReg;
    unsigned int m_uiFirstCleanFloatVSReg; // One past last dirty
    unsigned int m_uiFirstDirtyFloatPSReg;
    unsigned int m_uiFirstCleanFloatPSReg; // One past last dirty

    unsigned int m_uiNumFloatVSConstants;
    unsigned int m_uiNumFloatPSConstants;

    // Integer registers
    int* m_piIntVSConstantData;
    int* m_piSavedIntVSConstantData;
    int* m_piIntPSConstantData;
    int* m_piSavedIntPSConstantData;

    unsigned int m_uiFirstDirtyIntVSReg;
    unsigned int m_uiFirstCleanIntVSReg; // One past last dirty
    unsigned int m_uiFirstDirtyIntPSReg;
    unsigned int m_uiFirstCleanIntPSReg; // One past last dirty

    unsigned int m_uiNumIntVSConstants;
    unsigned int m_uiNumIntPSConstants;

    // Booleger registers
    BOOL* m_pbBoolVSConstantData;
    BOOL* m_pbSavedBoolVSConstantData;
    BOOL* m_pbBoolPSConstantData;
    BOOL* m_pbSavedBoolPSConstantData;

    unsigned int m_uiFirstDirtyBoolVSReg;
    unsigned int m_uiFirstCleanBoolVSReg; // One past last dirty
    unsigned int m_uiFirstDirtyBoolPSReg;
    unsigned int m_uiFirstCleanBoolPSReg; // One past last dirty

    unsigned int m_uiNumBoolVSConstants;
    unsigned int m_uiNumBoolPSConstants;

    D3DDevicePtr m_pkD3DDevice;
    NiD3DRenderer* m_pkD3DRenderer;
};

typedef NiPointer<NiD3DShaderConstantManager> NiD3DShaderConstantManagerPtr;

//#include "NiD3DShaderConstantManager.inl"

#endif  //#ifndef NID3DSHADERCONSTANTMANAGER_H
