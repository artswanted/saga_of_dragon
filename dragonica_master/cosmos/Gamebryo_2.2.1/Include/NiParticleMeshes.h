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

#ifndef NiPARTICLEMESHES_H
#define NiPARTICLEMESHES_H

#include "NiParticles.h"
#include "NiParticleMeshesData.h"
#include "NiTriShape.h"

class NIMAIN_ENTRY NiParticleMeshes : public NiParticles
{
    NiDeclareRTTI;
    NiDeclareClone(NiParticleMeshes);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // The constructed object is given ownership of the input arrays and
    // has the responsibility for deleting them when finished with them.
    NiParticleMeshes(unsigned short usVertices, NiPoint3* pkVertex,
        NiPoint3* pkNormal, NiColorA* pkColor, NiQuaternion* pkRots = NULL);

    // The constructed object shares the input data.
    NiParticleMeshes(NiParticleMeshesData* pkModelData);

    // particle attributes
    NiAVObject* GetMeshAt(unsigned int uiIndex) const;

    // *** begin Emergent internal use only ***

    // update
    virtual void UpdateDownwardPass(float fTime, bool bUpdateControllers);
    virtual void UpdateSelectedDownwardPass(float fTime);
    virtual void UpdateRigidDownwardPass(float fTime);
    virtual void UpdateWorldData();
    virtual void UpdatePropertiesDownward(NiPropertyState* pkParentState);
    virtual void UpdateEffectsDownward(NiDynamicEffectState* pkParentState);

    // Note: UpdateWorldBound used to be overridden such that the 
    // world bound of the particles container was used as the 
    // NiParticleMeshes world bound.  However, this interferes
    // with the NiParticleSystemController::m_bStaticBound flag and can
    // cause the bound to be invalid when static bounds are in use.
    // If static bounds are not being used, 
    // NiParticleSystemController::UpdateParticles dynamically computes 
    // the bound from the NiParticlesData vertices.  This may cause 
    // the bound radius to be too small and in rare cases visual 
    // artifacts may result.

    // culling (separate from display)
    virtual void OnVisible(NiCullingProcess& kCuller);

    // set selective update flags
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
        bool bSelectiveUpdateTransforms, bool& bRigid);

    // renderer data
    virtual void PurgeRendererData(NiRenderer* pkRenderer);

    // *** end Emergent internal use only ***

protected:
    // streaming support
    NiParticleMeshes();

    // for Update
    float m_fTime;
    bool m_bUpdateControllers;
};

NiSmartPointer(NiParticleMeshes);

#include "NiParticleMeshes.inl"

#endif // NiPARTICLEMESHES_H

