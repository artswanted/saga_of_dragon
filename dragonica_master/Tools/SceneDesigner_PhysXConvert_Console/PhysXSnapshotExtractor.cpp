#include "StdAfx.h"
#include "PhysXSnapshotExtractor.h"

#include <NxCooking.h>

const int CPhysXSnapshotExtractor::NUM_PRIMES = 10;
const unsigned int CPhysXSnapshotExtractor::PRIMES[10] =
    { 17, 37, 67, 127, 257, 521, 1031, 2053, 4127, 8237 };

//---------------------------------------------------------------------------
NiPhysXSceneDesc* CPhysXSnapshotExtractor::ExtractSnapshot(
    NiPhysXScene* pkScene, const NxMat34& kXform)
{
    unsigned int uiNumMaterials = pkScene->GetPhysXScene()->getNbMaterials();
    NiTMap<NxActor*, NiPhysXActorDescPtr> kActorMap;
    NiPhysXSceneDesc* pkSnapshot =
        NiNew NiPhysXSceneDesc(NextPrime(uiNumMaterials));

    if (!CaptureScene(pkSnapshot, pkScene, kXform, kActorMap))
    {
        NiDelete pkSnapshot;
        return NULL;
    }
        
    return pkSnapshot;
}
//---------------------------------------------------------------------------
bool CPhysXSnapshotExtractor::CaptureScene(NiPhysXSceneDesc* pkSnapshot,
    NiPhysXScene* pkScene, const NxMat34& kXform, NiTMap<NxActor*,
    NiPhysXActorDescPtr>& kActorMap)
{
    NxScene* pkPhysXScene = pkScene->GetPhysXScene();
    
    // First extract all the info in the scene descriptor.
    NxSceneDesc kSceneDesc;
    pkPhysXScene->saveToDesc(kSceneDesc);
    pkSnapshot->SetNxSceneDesc(kSceneDesc);
    
    // Now extract the other global scene information
    for (unsigned int i = 0; i < 32; i++)
    {
        for (unsigned int j = 0; j < 32; j++)
        {
            pkSnapshot->SetGroupCollisionFlag(i, j,
                pkPhysXScene->getGroupCollisionFlag(i, j));
        }
    }

    NiTMap<NxActor*, bool> kExcludedMap;
    MapExcludedActors(pkScene, kExcludedMap);

    // Extract the actors
    NxInitCooking(NxGetPhysicsSDKAllocator(), NiPhysXManager::GetPhysXManager()
        ->m_pkPhysXSDK->getFoundationSDK().getErrorStream());
    unsigned int uiNumActors = pkPhysXScene->getNbActors();
    NxActor** pkActorArray = pkPhysXScene->getActors();
    unsigned int uiNum = 0;
    for (unsigned int i = 0; i < uiNumActors; i++)
    {
        bool bDummy;
        if (kExcludedMap.GetAt(pkActorArray[i], bDummy))
            continue;
        NiPhysXActorDescPtr spActorDesc =
            CaptureActor(pkActorArray[i], kXform);
        if (spActorDesc)
        {
            pkSnapshot->AddActor(spActorDesc);
            kActorMap.SetAt(pkActorArray[i], spActorDesc);
        }
    }
    NxCloseCooking();

    // Set all the source and destination pointers
    for (unsigned int i = 0; i < pkScene->GetSourcesCount(); i++ )
    {
        NiPhysXSrc* pkSrc = pkScene->GetSourceAt(i);
        if (NiIsKindOf(NiPhysXRigidBodySrc, pkSrc))
        {
            NiPhysXRigidBodySrc* pkRBSrc = (NiPhysXRigidBodySrc*)pkSrc;
            NxActor* pkActor = pkRBSrc->GetTarget();
            NiPhysXActorDescPtr pkActorDesc;
            if (kActorMap.GetAt(pkActor, pkActorDesc))
                pkActorDesc->SetSource(pkRBSrc);
        }
    }
    for (unsigned int i = 0; i < pkScene->GetDestinationsCount(); i++ )
    {
        NiPhysXDest* pkDest = pkScene->GetDestinationAt(i);
        if (NiIsKindOf(NiPhysXRigidBodyDest, pkDest))
        {
            NiPhysXRigidBodyDest* pkRBDest = (NiPhysXRigidBodyDest*)pkDest;
            NxActor* pkActor = pkRBDest->GetActor();
            NiPhysXActorDescPtr pkActorDesc;
            if (kActorMap.GetAt(pkActor, pkActorDesc))
                pkActorDesc->SetDest(pkRBDest);
            pkActor = pkRBDest->GetActorParent();
            if (pkActor && kActorMap.GetAt(pkActor, pkActorDesc))
                pkActorDesc->SetActorParent(pkActorDesc);
        }
    }

    // Extract the joints
    unsigned int uiNumJoints = pkPhysXScene->getNbJoints();
    pkPhysXScene->resetJointIterator();
    NxJoint* pkJoint = pkPhysXScene->getNextJoint();
    while (pkJoint)
    {
        NiPhysXJointDescPtr spJointDesc =
            NiPhysXJointDesc::CreateFromJoint(pkJoint, kActorMap);
        pkSnapshot->AddJoint(spJointDesc);
        pkJoint = pkPhysXScene->getNextJoint();
    }
    
    // Extract materials
    unsigned int uiNumMaterials = pkPhysXScene->getNbMaterials();
    NxMaterial** pkMaterialArray = NiAlloc(NxMaterial*, uiNumMaterials);
    unsigned int uiMaterialIter = 0;
    pkPhysXScene->getMaterialArray(pkMaterialArray, uiNumMaterials,
        uiMaterialIter);
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        NxMaterialDesc kMatDesc;
        pkMaterialArray[i]->saveToDesc(kMatDesc);
        NiPhysXMaterialDescPtr spExisting = pkSnapshot->SetMaterialAt(
            kMatDesc, pkMaterialArray[i]->getMaterialIndex());
        spExisting = 0;
    }
    NiFree(pkMaterialArray);

    // Can't get the collision filter info out of the scene. Users have to set
    // them in the snapshot themselves via calls on the NiPhysXSceneDesc.
    
    pkSnapshot->SetNumStates(1);
    
    return true;
}
//---------------------------------------------------------------------------
NiPhysXActorDesc* CPhysXSnapshotExtractor::CaptureActor(
    NxActor* pkActor, const NxMat34& kXform)
{
    NiPhysXActorDesc* pkActorDesc = NiNew NiPhysXActorDesc;

    NxActorDesc kActorDesc;
    pkActor->saveToDesc(kActorDesc);
    
    NiFixedString kActorName;
    if (pkActor->getName())
        kActorName = NiFixedString(pkActor->getName());
    else
        kActorName = NiFixedString(NULL);
    
    NxBodyDesc kBodyDesc;
    NiPhysXBodyDesc* pkBodyDesc;
    bool bDynamic = pkActor->saveBodyToDesc(kBodyDesc);
    if (bDynamic)
    {
        pkBodyDesc = NiNew NiPhysXBodyDesc;
        pkBodyDesc->SetFromBodyDesc(kBodyDesc, kXform.M);
    }
    else
    {
        pkBodyDesc = 0;
    }

    NiTObjectArray<NiPhysXShapeDescPtr> kActorShapes;
      
    unsigned int uiNumShapes = pkActor->getNbShapes();
    NxShape*const* pkActorShapes = pkActor->getShapes();
    for (unsigned int i = 0; i < uiNumShapes; i++, pkActorShapes++)
    {
        NxShape* pkShape = *pkActorShapes;
        switch (pkShape->getType())
        {
            case NX_SHAPE_PLANE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxPlaneShapeDesc kPlaneShapeDesc;
                ((NxPlaneShape*)pkShape)->saveToDesc(kPlaneShapeDesc);
                pkShapeDesc->FromPlaneDesc(kPlaneShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;

            case NX_SHAPE_SPHERE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxSphereShapeDesc kSphereShapeDesc;
                ((NxSphereShape*)pkShape)->saveToDesc(kSphereShapeDesc);
                pkShapeDesc->FromSphereDesc(kSphereShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;

            case NX_SHAPE_CAPSULE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxCapsuleShapeDesc kCapsuleShapeDesc;
                ((NxCapsuleShape*)pkShape)->saveToDesc(kCapsuleShapeDesc);
                pkShapeDesc->FromCapsuleDesc(kCapsuleShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;

            case NX_SHAPE_BOX: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxBoxShapeDesc kBoxShapeDesc;
                ((NxBoxShape*)pkShape)->saveToDesc(kBoxShapeDesc);
                pkShapeDesc->FromBoxDesc(kBoxShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;
            
            case NX_SHAPE_CONVEX: {
                NxConvexShapeDesc kConvexShapeDesc;
                ((NxConvexShape*)pkShape)->saveToDesc(kConvexShapeDesc);
                
                NiPhysXShapeDesc* pkShapeDesc =
                    CaptureConvex(kConvexShapeDesc);
                if (pkShapeDesc)
                    kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;
                
            case NX_SHAPE_MESH: {
                NxTriangleMeshShapeDesc kTriangleMeshShapeDesc;
                ((NxTriangleMeshShape*)pkShape)->saveToDesc(
                    kTriangleMeshShapeDesc);
                    
                NiPhysXShapeDesc* pkShapeDesc =
                    CaptureTriMesh(kTriangleMeshShapeDesc);
                if (pkShapeDesc)
                    kActorShapes.AddFirstEmpty(pkShapeDesc);
                } break;
                
            default:;
        }
    }

    pkActorDesc->SetConstants(kActorName, kActorDesc.density,
        kActorDesc.flags, kActorDesc.group, pkBodyDesc, kActorShapes);
         
    // Store one set of state.
    NxMat34 kPose = kXform * pkActor->getGlobalPose();
    pkActorDesc->SetPose(kPose, 0);

    pkActorDesc->SetActor(pkActor);

    return pkActorDesc;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc* CPhysXSnapshotExtractor::CaptureConvex(
    const NxConvexShapeDesc& kConvexDesc)
{
    NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
    
    pkShapeDesc->FromShapeDesc(kConvexDesc);

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    NiPhysXConvexMeshPtr spMesh =
        pkManager->GetConvexMesh(kConvexDesc.meshData);
    
    NiPhysXMeshDesc* pkMeshDesc;
    bool bStreamInline = true;
    if (spMesh)
    {
        pkMeshDesc = spMesh->GetMeshDesc();
        if (pkMeshDesc)
        {
            pkShapeDesc->SetMeshDesc(pkMeshDesc);
            return pkShapeDesc;
        }
        else
        {
            pkMeshDesc = NiNew NiPhysXMeshDesc;
            pkMeshDesc->SetName(spMesh->GetName());
            bStreamInline = spMesh->GetStreamInline();
        }
    }
    else
    {
        pkMeshDesc = NiNew NiPhysXMeshDesc;  
        pkMeshDesc->SetName(kConvexDesc.name);
    }

    pkShapeDesc->SetMeshDesc(pkMeshDesc);
    pkMeshDesc->SetIsConvex(true);
    pkMeshDesc->SetFlags(kConvexDesc.meshFlags);
    
    NxConvexMesh* pkMesh = kConvexDesc.meshData;
    if (!pkMesh)
    {
        NiDelete pkShapeDesc;
        return 0;
    }
    
    
    if (!bStreamInline)
    {
        pkMeshDesc->SetData(0, 0);
        return pkShapeDesc;
    }
    
    NxConvexMeshDesc pkConvexMeshDesc;
    pkMesh->saveToDesc(pkConvexMeshDesc);
    NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
    bool bCookRes = NxCookConvexMesh(pkConvexMeshDesc, *kCookStream);
    if (!bCookRes)
    {
        NiDelete kCookStream;
        NiDelete pkShapeDesc;
        return false;
    }
    pkMeshDesc->SetData(kCookStream->GetSize(),
        (unsigned char*)kCookStream->GetBuffer());
    
    NiDelete kCookStream;
    
    return pkShapeDesc;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc* CPhysXSnapshotExtractor::CaptureTriMesh(
    const NxTriangleMeshShapeDesc& kTriMeshDesc)
{
    NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
    
    pkShapeDesc->FromShapeDesc(kTriMeshDesc);

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    NiPhysXTriangleMeshPtr spMesh =
        pkManager->GetTriangleMesh(kTriMeshDesc.meshData);
    
    NiPhysXMeshDesc* pkMeshDesc;
    bool bStreamInline = true;
    if (spMesh)
    {
        pkMeshDesc = spMesh->GetMeshDesc();
        if (pkMeshDesc)
        {
            pkShapeDesc->SetMeshDesc(pkMeshDesc);
            return pkShapeDesc;
        }
        else
        {
            pkMeshDesc = NiNew NiPhysXMeshDesc;
            pkMeshDesc->SetName(spMesh->GetName());
            bStreamInline = spMesh->GetStreamInline();
        }
    }
    else
    {
        pkMeshDesc = NiNew NiPhysXMeshDesc;  
        pkMeshDesc->SetName(kTriMeshDesc.name);
    }
    
    pkShapeDesc->SetMeshDesc(pkMeshDesc);
    pkMeshDesc->SetIsConvex(false);
    pkMeshDesc->SetFlags(kTriMeshDesc.meshFlags);
    
    NxTriangleMesh* pkMesh = kTriMeshDesc.meshData;
    if (!pkMesh)
    {
        NiDelete pkShapeDesc;
        return 0;
    }
    
    if (!bStreamInline)
    {
        pkMeshDesc->SetData(0, 0);
        return pkShapeDesc;
    }
    
    NxTriangleMeshDesc pkTriangleMeshDesc;
    pkMesh->saveToDesc(pkTriangleMeshDesc);
    NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
    bool bCookRes = NxCookTriangleMesh(pkTriangleMeshDesc, *kCookStream);
    if (!bCookRes)
    {
        NiDelete kCookStream;
        NiDelete pkShapeDesc;
        return false;
    }
    pkMeshDesc->SetData(kCookStream->GetSize(),
        (unsigned char*)kCookStream->GetBuffer());
    
    NiDelete kCookStream;
    
    return pkShapeDesc;

}
//---------------------------------------------------------------------------
void CPhysXSnapshotExtractor::MapExcludedActors(NiPhysXScene* pkScene,
    NiTMap<NxActor*, bool>& kExcludedMap)
{
    // Go through the descriptors looking for NiPhysXPSysSrcs
    // If we find one (or more), then we exclude all it's actors.
    for (unsigned int ui = 0; ui < pkScene->GetSourcesCount(); ui++)
    {
        NiPhysXSrc* pkSrc = pkScene->GetSourceAt(ui);
        if (NiIsKindOf(NiPhysXMeshPSysSrc, pkSrc))
        {
            NiPhysXMeshPSysSrc* pkPSysSrc = (NiPhysXMeshPSysSrc*)pkSrc;
            NiPhysXMeshParticleSystem* pkPSys =
                (NiPhysXMeshParticleSystem*)pkPSysSrc->GetSource();
            NiPhysXMeshPSysData* pkData =
                (NiPhysXMeshPSysData*)pkPSys->GetModelData();    
            NiPhysXParticleInfo* pkPhysXInfo = pkData->GetPhysXParticleInfo(); 

            for (unsigned int uj = 0; uj < pkData->GetActiveVertexCount();
                uj++)
            {
                NxActor* pkActor = pkPhysXInfo[uj].m_pkActor;
                if (pkActor)
                    kExcludedMap.SetAt(pkActor, true);
            }
            
            pkData->MapActorPools(kExcludedMap);
        }
        else if (NiIsKindOf(NiPhysXPSysSrc, pkSrc))
        {
            NiPhysXPSysSrc* pkPSysSrc = (NiPhysXPSysSrc*)pkSrc;
            NiPhysXParticleSystem* pkPSys =
                (NiPhysXParticleSystem*)pkPSysSrc->GetSource();
            NiPhysXPSysData* pkData = (NiPhysXPSysData*)pkPSys->GetModelData();
            NiPhysXParticleInfo* pkPhysXInfo = pkData->GetPhysXParticleInfo(); 

            for (unsigned int uj = 0; uj < pkData->GetActiveVertexCount();
                uj++)
            {
                NxActor* pkActor = pkPhysXInfo[uj].m_pkActor;
                if (pkActor)
                    kExcludedMap.SetAt(pkActor, true);
            }
            
            pkData->MapActorPool(kExcludedMap);
        }
    }
}
//---------------------------------------------------------------------------
unsigned int CPhysXSnapshotExtractor::NextPrime(const unsigned int target)
{
    int i;
    for (i = 0; i < NUM_PRIMES && PRIMES[i] < target; i++);
    return PRIMES[i];
}
//---------------------------------------------------------------------------
