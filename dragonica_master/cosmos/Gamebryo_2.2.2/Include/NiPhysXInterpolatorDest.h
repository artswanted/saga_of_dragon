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

#ifndef NIPHYSXINTERPOLATORDEST_H
#define NIPHYSXINTERPOLATORDEST_H

#include "NiPhysXLibType.h"

#include <NiTransformInterpolator.h>

#include "NiPhysXRigidBodyDest.h"

class NIPHYSX_ENTRY NiPhysXInterpolatorDest : public NiPhysXRigidBodyDest
{
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXInterpolatorDest);

public:
    NiPhysXInterpolatorDest(NiTransformInterpolator* pkTarget,
        NiAVObject* pkSceneParent, NxActor *pkActor, NxActor* pkActorParent);
    ~NiPhysXInterpolatorDest();

    NiTransformInterpolator* GetTarget() const;
    void SetTarget(NiTransformInterpolator* pkTarget, NiAVObject* pkParent);
    
    // Update functions
    virtual void UpdateSceneGraph(const float fT);

    // *** begin Emergent internal use only ***

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    // *** end Emergent internal use only ***
    
protected:
    NiPhysXInterpolatorDest();
    
    virtual NiAVObject* GetSceneGraphParent() const;
    
    NiTransformInterpolator* m_pkTarget;
    NiAVObject* m_pkSceneParent;
};

NiSmartPointer(NiPhysXInterpolatorDest);

#include "NiPhysXInterpolatorDest.inl"

#endif  // #ifndef NIPHYSXINTERPOLATORDEST_H
