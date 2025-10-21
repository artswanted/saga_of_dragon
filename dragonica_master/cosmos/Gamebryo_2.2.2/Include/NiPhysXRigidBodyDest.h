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

#ifndef NIPHYSXRIGIDBODYDEST_H
#define NIPHYSXRIGIDBODYDEST_H

#include "NiPhysXLibType.h"

#include <NxPhysics.h>
#include <NiAVObject.h>
#include <NiPoint3.h>
#include <NiQuaternion.h>

#include "NiPhysXDest.h"

class NIPHYSX_ENTRY NiPhysXRigidBodyDest : public NiPhysXDest
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXRigidBodyDest);

public:
    NiPhysXRigidBodyDest(NxActor* pkActor, NxActor* pkActorParent);
    ~NiPhysXRigidBodyDest();

    NxActor* GetActor() const;
    void SetActor(NxActor* pkActor);
    NxActor* GetActorParent() const;
    void SetActorParent(NxActor* pkParent);
    
    // Optimizing for sleeping
    bool GetOptimizeSleep() const;
    void SetOptimizeSleep(const bool bOptimize);
    
    // For printf debugging access
    float GetTime(unsigned int uiIndex) const;
    const NiPoint3& GetTranslate(unsigned int uiIndex) const;
    const NiQuaternion& GetRotate(unsigned int uiIndex) const;
    bool GetSleeping() const;
    
    // Update functions
    virtual void UpdateFromActors(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);
        
protected:
    NiPhysXRigidBodyDest();
    
    // We need the parent of the controlled scene graph node to
    // compute the local transformation, but what that is depends on
    // the type of thing we are driving. So it's virtual.
    virtual NiAVObject* GetSceneGraphParent() const = 0;
    
    // Provide interpolation functionality for use by sub-classes
    void Interpolate(const float fT, NiQuaternion& kRotation,
        NiPoint3& kTranslate);
    
    NxActor* m_pkActor;    
    NxActor* m_pkActorParent;
    
    bool m_bOptimizeSleep;
    bool m_bSleeping;
    
    // A rotating buffer of two transforms. These are the incremental
    // transforms that go directly into the scene graph.
    unsigned char m_aucIndices[2];
    float m_afTimes[2];
    NiPoint3 m_akTranslates[2];
    NiQuaternion m_akRotates[2];
};

NiSmartPointer(NiPhysXRigidBodyDest);

#include "NiPhysXRigidBodyDest.inl"

#endif  // #ifndef NIPHYSXRIGIDBODYDEST_H
