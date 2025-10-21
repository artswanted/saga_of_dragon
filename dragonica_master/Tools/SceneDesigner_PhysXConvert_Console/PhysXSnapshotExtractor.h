#pragma once

#ifndef NIPHYSXSNAPSHOTEXTRACTOR_H
#define NIPHYSXSNAPSHOTEXTRACTOR_H

#include "NiPhysXSnapshotExtractorLibType.h"
#include <NiPhysXParticle.h>
#include <NxPhysics.h>


class NIPHYSXSNAPSHOTEXTRACTOR_ENTRY CPhysXSnapshotExtractor :
    public NiMemObject
{
public:
    NiPhysXSceneDesc*
        ExtractSnapshot(NiPhysXScene* pkScene, const NxMat34& kXform);
    
protected:
    bool CaptureScene(NiPhysXSceneDesc* pkSnapshot,
        NiPhysXScene* pkScene, const NxMat34& kXform,
        NiTMap<NxActor*, NiPhysXActorDescPtr>& kActorMap);
    NiPhysXActorDesc* CPhysXSnapshotExtractor::CaptureActor(
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

