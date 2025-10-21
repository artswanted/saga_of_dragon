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


#ifndef NIPHYSXMESHDESC_H
#define NIPHYSXMESHDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include <NxPhysics.h>

class NIPHYSX_ENTRY NiPhysXMeshDesc : public NiObject
{
    NiDeclareStream;
    NiDeclareRTTI;

public:
    NiPhysXMeshDesc();
    ~NiPhysXMeshDesc();
    
    bool GetIsConvex() const;
    void SetIsConvex(const bool bIsConvex);

    NxU32 GetFlags() const;
    void SetFlags(const NxU32 uiFlags);

    const NiFixedString& GetName() const;
    void SetName(const NiFixedString& kName);

    void GetData(size_t& kSize, unsigned char** ppucData);
    void SetData(const size_t uiSize, unsigned char* pucData);

    virtual void ToConvexDesc(NxConvexShapeDesc& kConvexDesc,
        const bool bKeepMesh);
    virtual void ToTriMeshDesc(NxTriangleMeshShapeDesc& kTriMeshDesc,
        const bool bKeepMesh);
 
    // *** begin Emergent internal use only ***

    virtual bool StreamCanSkip();

    // *** end Emergent internal use only ***

protected:
    bool m_bIsConvex;
    NxU32 m_uiMeshFlags;
    NiFixedString m_kMeshName;
    size_t m_uiMeshSize;
    unsigned char* m_pucMeshData;
};

NiSmartPointer(NiPhysXMeshDesc);

#include "NiPhysXMeshDesc.inl"

#endif // NIPHYSXMESHDESC_H

