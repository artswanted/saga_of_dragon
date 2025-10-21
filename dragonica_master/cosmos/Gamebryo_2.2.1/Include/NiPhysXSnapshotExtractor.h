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

#ifndef NIPHYSXSNAPSHOTEXTRACTOR_H
#define NIPHYSXSNAPSHOTEXTRACTOR_H

#include "NiPhysXSnapshotExtractorLibType.h"

#include <NiSystem.h>
#include <NiMain.h>
#include <NiPhysX.h>
#include <NiPhysXFluid.h>
#include <NiPhysXParticle.h>

#include <NxPhysics.h>


class NIPHYSXSNAPSHOTEXTRACTOR_ENTRY NiPhysXSnapshotExtractor :
    public NiMemObject
{
public:
    NiPhysXSceneDesc*
        ExtractSnapshot(NiPhysXScene* pkScene, const NxMat34& kXform);
    //NiPhysXFluidSceneDesc*
    //    ExtractFluidSnapshot(NiPhysXScene* pkScene, const NxMat34& kXform);
    
protected:
    bool CaptureScene(NiPhysXSceneDesc* pkSnapshot,
        NiPhysXScene* pkScene, const NxMat34& kXform,
        NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap);
    NiPhysXActorDesc* NiPhysXSnapshotExtractor::CaptureActor(
        NxActor* pkActor, const NxMat34& kXform);
    NiPhysXShapeDesc* CaptureConvex(const NxConvexShapeDesc& kConvexDesc);
    NiPhysXShapeDesc* CaptureTriMesh(
        const NxTriangleMeshShapeDesc& kTriMeshDesc);

    void MapExcludedActors(NiPhysXScene* pkScene,
        NiTMap<NxActor*, bool>& kExcludedMap);
    
    // Prime numbers
    static const int NUM_PRIMES;
    static const unsigned int PRIMES[10];
    static unsigned int NextPrime(const unsigned int target);
};

#endif  // #ifndef NIPHYSXSNAPSHOTEXTRACTOR_H

