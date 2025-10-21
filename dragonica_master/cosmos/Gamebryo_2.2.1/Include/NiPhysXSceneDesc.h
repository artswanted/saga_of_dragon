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


#ifndef NIPHYSXSCENEDESC_H
#define NIPHYSXSCENEDESC_H

#include <NiMain.h>
#include <NiSystem.h>

#include <NxPhysics.h>

#include "NiPhysXActorDesc.h"
#include "NiPhysXBodyDesc.h"
#include "NiPhysXD6JointDesc.h"
#include "NiPhysXMaterialDesc.h"
#include "NiPhysXShapeDesc.h"

#include "NiPhysXScene.h"

class NIPHYSX_ENTRY NiPhysXSceneDesc : public NiObject
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXSceneDesc);

public:
    NiPhysXSceneDesc(unsigned int uiMaterialHashSize = 521);
    virtual ~NiPhysXSceneDesc();

    // Obtain all the data one can find in a PhysX scene descriptor.
    virtual NxSceneDesc& GetNxSceneDesc();
    virtual void SetNxSceneDesc(const NxSceneDesc& kSceneDesc);
    
    // Set things not in a NXSceneDesc
    void SetGroupCollisionFlag(const unsigned char ucGroup1,
        const unsigned char ucGroup2, const bool bVal);
    void SetFilterOps(NxFilterOp eOp0, NxFilterOp eOp1, NxFilterOp eOp2); 
    void SetFilterBool(bool bFlag);
    void SetFilterConstant0(const NxGroupsMask &kMask);
    void SetFilterConstant1(const NxGroupsMask &kMask);
    
    // Get functions for collision flags
    bool GetGroupCollisionFlag(const unsigned char ucGroup1,
        const unsigned char ucGroup2) const;
    void GetFilterOps(
        NxFilterOp& eOp0, NxFilterOp& eOp1, NxFilterOp& eOp2) const; 
    bool GetFilterBool() const;
    void GetFilterConstant0(NxGroupsMask &kMask) const;
    void GetFilterConstant1(NxGroupsMask &kMask) const;

    // Access to the actor list
    unsigned int GetActorCount() const;
    NiPhysXActorDescPtr GetActorAt(unsigned int uiIndex);
    unsigned int AddActor(NiPhysXActorDesc* pkActorDesc);
    NiPhysXActorDescPtr RemoveActorAt(unsigned int uiIndex);

    // Access to the joint list
    unsigned int GetJointCount() const;
    NiPhysXJointDescPtr GetJointAt(unsigned int uiIndex);
    unsigned int AddJoint(NiPhysXJointDesc* pkJointDesc);
    NiPhysXJointDescPtr RemoveJointAt(unsigned int uiIndex);

    // Access to the material map
    NxMaterialIndex AddMaterial(NxMaterialDesc& kMaterial,
        const NxMaterialIndex usBaseIndex = 0);
    NiPhysXMaterialDescPtr SetMaterialAt(NxMaterialDesc& kMaterial,
        const NxMaterialIndex usIndex);
    NiPhysXMaterialDescPtr GetMaterial(const NxMaterialIndex usIndex);
    NiTMapIterator GetFirstMaterial();
    void GetNextMaterial(NiTMapIterator& kPos, NxMaterialIndex& usIndex,
        NiPhysXMaterialDescPtr& spMaterial);
    bool RemoveMaterial(const NxMaterialIndex usIndex);
        
    // *** begin Emergent internal use ***
    
    // Creates a PhysX scene based on this descriptor. If pkExistingScene
    // is non-null, the actors, joints and materials in this descriptor are
    // added into the existing scene.
    virtual NxScene* CreateScene(const NxMat34& kXform,
        const bool bKeepMeshes = false, NxScene* pkExistingScene = 0,
        const unsigned int uiIndex = 0);
    
    // Removes any actors, joints and materials that this descriptor knows
    // about from the given PhysX scene.
    virtual void RemoveScene(NxScene* pkScene);

    void SetNumStates(const unsigned int uiNumStates);

    // Adds a new set of restore state from the scene, and returns the
    // index of the new state.
    virtual unsigned int AddState(NxScene* pkPhysXScene, 
        const NxMat34& kXform);

    // Updates the restore state from the given scene. The index tells which
    // of the stored states to update.
    virtual void UpdateState(NxScene* pkPhysXScene, const unsigned int uiIndex,
        const NxMat34& kXform);

    // Restores state to the given scene. The index tells which
    // of the stored states to restore from.
    virtual void RestoreState(NxScene* pkScene, const unsigned int uiIndex,
        const NxMat34& kXform);

    unsigned int GetNumStates() const;
    
    // Non-default streaming functions
    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    // *** end Emergent internal use ***
            
protected:
    //
    // For streaming of deprecated PhysX member variables.
    //
    enum NiSceneDescNxBroadPhaseType
	{
	    NX_BROADPHASE_QUADRATIC,
	    NX_BROADPHASE_FULL,
	    NX_BROADPHASE_COHERENT,

	    NX_BROADPHASE_FORCE_DWORD = 0x7fffffff
	};


    // Updates the actor pointers in NiPhysXSrc and NiPhysXDest objects.
    virtual void PushToSrcDest() const;

    // Most data is in a PhysX scene descriptor
    NxSceneDesc m_kNxSceneDesc;
    
    // These things are queryable from a scene, but not in a descriptor.
    bool m_abGroupCollisionFlags[32][32];
    NxFilterOp m_aeFilterOps[3];
    bool m_bFilterBool;
    NxGroupsMask m_auiFilterConstants[2];

    // Components of a scene
    NiTObjectArray<NiPhysXActorDescPtr> m_kActors;
    NiTObjectArray<NiPhysXJointDescPtr> m_kJoints;
    NiTMap<NxMaterialIndex, NiPhysXMaterialDescPtr> m_kMaterials;
    
    // The number of states we have for restoration
    unsigned int m_uiNumStates;

    // Actors that should not be stored. Particle system actors, typically.
    NiTMap<NxActor*, bool> kExcludedMap;
};

#include "NiPhysXSceneDesc.inl"

#endif // NIPHYSXSCENEDESC_H
