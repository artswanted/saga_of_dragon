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

#ifndef NID3DXEFFECTTECHNIQUEDESC_H
#define NID3DXEFFECTTECHNIQUEDESC_H

#include "NiD3DXEffectShaderLibType.h"
#include <NiGeometryData.h>

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectTechniqueDesc : public NiMemObject
{
public:
    static NiD3DXEffectTechniqueDesc* GetDesc();

    const char* m_pcShaderName;
    const char* m_pcTechniqueName;
    const char* m_pcDesc;

    unsigned int m_uiImplementation;
    unsigned int m_uiPSVersion;
    unsigned int m_uiVSVersion;
    unsigned int m_uiUserVersion;

    unsigned int m_uiBonesPerPartition;
    NiGeometryData::DataFlags m_eNBTMethod;
    unsigned int m_uiNBTSourceUV;

    bool m_bUsesNiRenderState;
    bool m_bUsesNiLightState;
    bool m_bSoftwareVPRequired;
    bool m_bSoftwareVPAcceptible;
protected:
    static NiD3DXEffectTechniqueDesc ms_kTechniqueDesc;
};

#endif //NID3DXEFFECTTECHNIQUEDESC_H
