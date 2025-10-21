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

#ifndef NIPHYSXSCENE_H
#define NIPHYSXSCENE_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXDest.h"
#include "NiPhysXSrc.h"

#include <NxPhysics.h>

class NiPhysXManager;

NiSmartPointer(NiPhysXSceneDesc);
NiSmartPointer(NiPhysXScene);

class NIPHYSX_ENTRY NiPhysXScene : public NiObjectNET
{
    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXScene);

public:
    NiPhysXScene();
    ~NiPhysXScene();

    // Get and set the PhysX scene.
    NxScene* GetPhysXScene() const;
    void SetPhysXScene(NxScene* pkScene);
    void ReleaseScene();
    
    // If slaved, another scene controls the PhysX NxScene.
    bool GetSlaved() const;
    NxMat34 GetSlavedXform() const;
    void SetSlaved(NiPhysXScene* pkMaster, const NxMat34& kXform);
    
    // Get and set the global tranform for the PhysX scene we are part of.
    const NiTransform& GetSceneXform() const;
    void SetSceneXform(const NiTransform& kXform);
    
    // Get and set the scale. World size is scale * physics size
    float GetScaleFactor() const;
    void SetScaleFactor(const float fScale);

    // Update control
    bool GetUpdateSrc() const;
    void SetUpdateSrc(const bool bActive);
    bool GetUpdateDest() const;
    void SetUpdateDest(const bool bActive);

    // Time step control
    bool GetDoFixedStep() const;
    void SetDoFixedStep(const bool bActive);
    float GetTimestep() const;
    void SetTimestep(const float fStep);

    // Interpolation control
    void SetAllSrcInterp(const bool bActive);
    void SetAllDestInterp(const bool bActive);
    
    // Debug visualization
    bool GetDebugRender() const;
    void SetDebugRender(const bool bActive, NiNode* pkDebugParent = 0);
    void ClearDebugGeometry();
    
    // Simulation and results fetching.
    bool GetInSimFetch() const;
    bool Simulate(const float fTargetTime, const bool bForceSim = false);
    bool FetchResults(const float fTargetTime, const bool bBlock = false,
        NxU32* puiErrorState = 0);
    void SetFetchTime(const float fCurrentTime);
    float GetPrevFetchTime() const;
    float GetNextFetchTime() const; // Only valid if simulating
    
    // Update functions. Times are Gamebryo application times
    void UpdateSources(const float fTime, bool bForce = false);
    void UpdateDestinations(const float fTime, bool bForce = false);
        
    // Access to the arrays of sources and destinations
    unsigned int GetSourcesCount() const;
    unsigned int AddSource(NiPhysXSrc* pkSrc);
    NiPhysXSrc* GetSourceAt(const unsigned int uiIndex);
    bool DeleteSource(NiPhysXSrc* pkTarget);
    void DeleteSourceAt(const unsigned int uiIndex);    
    unsigned int GetDestinationsCount() const;
    unsigned int AddDestination(NiPhysXDest* pkDest);
    NiPhysXDest* GetDestinationAt(const unsigned int uiIndex);
    bool DeleteDestination(NiPhysXDest* pkTarget);
    void DeleteDestinationAt(const unsigned int uiIndex);
    
    // Snapshots and reset
    NiPhysXSceneDesc* GetSnapshot();
    void SetSnapshot(NiPhysXSceneDesc* pkSnapshot);
    void CreateSceneFromSnapshot(const unsigned int uiIndex = 0);
    void ClearSceneFromSnapshot();
    void ReleaseSnapshot();
    unsigned int AddSnapshotState();
    void UpdateSnapshotState(const unsigned int uiIndex);
    void RestoreSnapshotState(const unsigned int uiIndex);

    // Whether or not snapshots keep their mesh data around. You would keep
    // meshes if you planned on removing meshes from the NiPhysXManager and
    // recreating them later, because otherwise mesh data is shared and once
    // you have one copy, you never need another.
    bool GetKeepMeshes() const;
    void SetKeepMeshes(const bool bVal);
        
    // *** begin Emergent internal use only ***

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    // *** end Emergent internal use only ***
            
protected:
    // Slaves
    bool IsSlave(const NiPhysXScene* pkScene) const;
    void AddSlave(NiPhysXScene* pkScene);
    void RemoveSlave(const NiPhysXScene* pkScene);
    
    // For masters to set times on slaves
    void SetPrevFetchTime(const float fTime);
    void SetNextFetchTime(const float fTime);

    // Update
    void UpdateSourcesToActors(const float fTBegin, const float fTEnd);

    // Debug geometry
    void SetDebugGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugTriangleGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugLineGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugPointGeometry(const NxDebugRenderable* pkRenderState);

    // A local pointer to the manager
    NiPhysXManager* m_pkManager;

    // The transform from Gamebryo world coordinates to the PhysX world
    // coordinates. Only valid if not slaved, otherwise the master's
    // transform is used, with the m_kSlavedXform used when caching and
    // restoring state.
    NiTransform m_kSceneXform;
    
    // Only one of m_pkPhysXScene and m_pkMasterScene should be non-0
    NxScene* m_pkPhysXScene;
    NiPhysXScene* m_pkMasterScene;
    
    // The transform that takes this scene's PhysX coordinate system to the
    // master scene's coordinate system. Used in snapshots.
    NxMat34 m_kSlavedXform;
    
    // Scale factor
    float m_fScalePToW;

    // Lists of objects to update
    NiTObjectArray<NiPhysXSrcPtr> m_kSources;
    NiTObjectArray<NiPhysXDestPtr> m_kDestinations;
   
    // Management of time for PhysX simulation
    float m_fTimestep; // Only used if fixed timestep
    float m_fPrevFetchTime; // The time of the most recent FetchResults.
    float m_fNextFetchTime; // The time of any upcoming FecthResults.
    
    // Protect the simulate/fetchResult pair.
    bool m_bInSimFetch;

    // Debug rendering
    NiNodePtr m_spDebugRoot;
    NiTPrimitiveArray<unsigned short> m_kDebugGeomSizes;
    
    // Snapshots
    bool m_bKeepMeshes;
    NiPhysXSceneDescPtr m_spSnapshot;
    
    // Slaves
    NiTObjectArray<NiPhysXScenePtr> m_kSlaves;
    
    // Flags
    enum {
        // Control of simulation and update
        UPDATE_SRC_MASK         = 0x0002,
        UPDATE_DEST_MASK        = 0x0004,
        
        // Time step control
        FIXED_TIMESTEP_MASK     = 0x0010,
        
        // Debug rendering enabled
        DEBUG_RENDER_MASK       = 0x0400
    };
    
    NiDeclareFlags(unsigned short);
    
    // Prime numbers
    static const int NUM_PRIMES;
    static const unsigned int PRIMES[10];
    static unsigned int NextPrime(const unsigned int target);
};

#include "NiPhysXScene.inl"

#endif  // #ifndef NIPHYSXSCENE_H

