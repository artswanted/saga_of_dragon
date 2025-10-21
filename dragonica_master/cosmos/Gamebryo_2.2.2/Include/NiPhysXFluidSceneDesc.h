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


#ifndef NIPHYSXFLUIDSCENEDESC_H
#define NIPHYSXFLUIDSCENEDESC_H

#include "NiPhysXFluidLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include <NxPhysics.h>

#include "NiPhysXSceneDesc.h"
#include "NiPhysXFluidDesc.h"

class NIPHYSXFLUID_ENTRY NiPhysXFluidSceneDesc : public NiPhysXSceneDesc
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXFluidSceneDesc);

public:
    NiPhysXFluidSceneDesc(unsigned int uiMaterialHashSize = 521);
    virtual ~NiPhysXFluidSceneDesc();

    unsigned int GetNumFluids() const;
    NiPhysXFluidDescPtr GetFluidDesc(unsigned int uiIndex);
    void AddFluidDesc(NiPhysXFluidDesc* pkFluidDesc);
    NiPhysXFluidDescPtr RemoveFluidDesc(unsigned int uiIndex);
        
    static bool ProcessFluidMeshes(NxScene* pkScene,
        NxReal fRestParticlesPerMeter, NxReal fKernelRadiusMultiplier,
        NxReal fMotionLimitMultiplier, NxU32 uiPacketSizeMultiplier);

    // *** begin Emergent internal use ***
    
    // Creates a PhysX scene based on this descriptor. If pkExistingScene
    // is non-null, everything in this descriptor are added into the existing
    // scene.
    virtual NxScene* CreateScene(const NxMat34& kXform,
        const bool bKeepMeshes = false, NxScene* pkExistingScene = 0,
        const unsigned int uiIndex = 0);
    
    // Removes the fluid, in addition to anything in it's base class.
    virtual void RemoveScene(NxScene* pkScene);
    
    // There is no state caching for fluids, because the PhysX API does not
    // support the user removal of particles from a simulation.
    
    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    // *** end Emergent internal use ***
            
protected:
    // This class adds a fluid descriptor
    NiTObjectArray<NiPhysXFluidDescPtr> m_kFluids;
};

NiSmartPointer(NiPhysXFluidSceneDesc);

#include "NiPhysXFluidSceneDesc.inl"

#endif NIPHYSXFLUIDSCENEDESC_H
