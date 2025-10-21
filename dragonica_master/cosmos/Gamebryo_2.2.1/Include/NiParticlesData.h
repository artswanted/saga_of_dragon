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

#ifndef NIPARTICLESDATA_H
#define NIPARTICLESDATA_H

#include "NiGeometryData.h"
#include "NiQuaternion.h"

class NIMAIN_ENTRY NiParticlesData : public NiGeometryData
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiParticlesData(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiQuaternion* pkRots,
        float* pfRotationAngles, NiPoint3* pkRotationAxes);

    NiParticlesData();

    virtual ~NiParticlesData();

    // particle attributes
    float* GetRadii();
    const float* GetRadii() const;
    float* GetSizes();
    const float* GetSizes() const;

    // These two functions should always return NULL for NiParticle particle
    // systems. This array is only used by NiOldParticle particle systems.
    NiQuaternion* GetRotations();
    const NiQuaternion* GetRotations() const;

    float* GetRotationAngles();
    const float* GetRotationAngles() const;
    NiPoint3* GetRotationAxes();
    const NiPoint3* GetRotationAxes() const;

    virtual void SetActiveVertexCount(unsigned short usActive);
    virtual unsigned short GetActiveVertexCount() const;

    // reallocation support
    void ReplaceRadii(float* pfRadii);
    void ReplaceSizes(float* pfSizes);
    void ReplaceRotations(NiQuaternion* pkRotations);
    void ReplaceRotationAngles(float* pfRotationAngles);
    void ReplaceRotationAxes(NiPoint3* pkRotationAxes);

    virtual void RemoveParticle(unsigned short usParticle);

#ifdef _PS3
    virtual bool ContainsVertexData(NiShaderDeclaration::ShaderParameter 
        eParameter) const;
#endif

    // *** begin Emergent internal use only ***

    // For temporarily using data only.  This call is like Replace(), 
    // but does not delete any pointers.  Be careful to call this only 
    // on objects created by the default (empty) constructor to prevent 
    // memory leaks.  These pointers will be owned (and deleted upon 
    // destruction) by this object so use this call carefully.
    void SetRadiiData(float* pfRadii);
    void SetSizesData(float* pfSizes);
    void SetRotationsData(NiQuaternion* pkRotations);
    void SetRotationAnglesData(float* pfRotationAngles);
    void SetRotationAxesData(NiPoint3* pkRotationAxes);

    // updates
    virtual void CalculateNormals();

#ifndef T3D_NO_NIF_CONVERSION
    // We now create an NiParticlesData object when in the old system, we
    // would have created an NiRotatingParticlesData object
    // Because the NiRotatingParticlesData added an optional array of 
    // rotations, we need to flag the new NiParticlesData object inside a
    // new creation function, so that inside of NiParticlesData::LoadBinary,
    // we can look for the optional array of quaternions.
    static NiObject* CreateOldRotatingParticlesObject();
    bool m_bIsOldRotatingParticlesObject;
#endif

    // *** end Emergent internal use only ***

protected:
    float* m_pfRadii;
    unsigned short m_usActive;
    float* m_pfSizes;

    // This member should always be NULL for NiParticle particle systems. It
    // is only used by NiOldParticle particle systems.
    NiQuaternion* m_pkRotations;

    float* m_pfRotationAngles;
    NiPoint3* m_pkRotationAxes;
};

typedef NiPointer<NiParticlesData> NiParticlesDataPtr;

#include "NiParticlesData.inl"

#endif // NIPARTICLESDATA_H
