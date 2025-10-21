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

#ifndef NIDX9SHADERCONSTANTMANAGER_H
#define NIDX9SHADERCONSTANTMANAGER_H

#include "NiD3DShaderConstantManager.h"

class NID3D_ENTRY NiDX9ShaderConstantManager : 
    public NiD3DShaderConstantManager
{
public:
    virtual ~NiDX9ShaderConstantManager();

    // *** begin Emergent internal use only ***

    virtual void CommitChanges();

    static NiDX9ShaderConstantManager* Create(NiD3DRenderer* pkRenderer, 
        const D3DCAPS9& kD3DCaps9);

    // *** end Emergent internal use only ***

protected:
    NiDX9ShaderConstantManager(NiD3DRenderer* pkD3DRenderer, 
        const D3DCAPS9& kD3DCaps9);

    bool m_bSoftwareOnlyVP;
};

typedef NiPointer<NiDX9ShaderConstantManager> NiDX9ShaderConstantManagerPtr;

//#include "NiDX9ShaderConstantManager.inl"

#endif  //#ifndef NIDX9SHADERCONSTANTMANAGER_H
