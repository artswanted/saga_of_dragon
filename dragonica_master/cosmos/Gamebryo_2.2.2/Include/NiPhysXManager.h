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

#ifndef NIPHYSXMANAGER_H
#define NIPHYSXMANAGER_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXAllocator.h"
#include "NiPhysXConvexMesh.h"
#include "NiPhysXTriangleMesh.h"
#include "NiPhysXUserOutput.h"

#include <NxPhysics.h>

// class NiPhysXScene;

class NIPHYSX_ENTRY NiPhysXManager : public NiMemObject
{
public:
    // Access to the unique physics system
    static NiPhysXManager* GetPhysXManager();
    
    // Initialize and shutdown. This cannot be done automatically because
    // users must be free to set their own allocators and output stream.
    // Shutdown must not be called until all smart pointers into PhysX related
    // things have been freed. This is because de-allocation for some objects
    // goes into the PhysX SDK.
    bool Initialize(NxUserAllocator* pkAllocator = NULL,
        NxUserOutputStream* pkOutputStream = NULL);
    void Shutdown();
    
    // Semaphores for locking access to the PhysX SDK.
    // The wait call is blocking if the caller does not already hold the lock.
    void WaitSDKLock();
    void ReleaseSDKLock();

    // For getting/setting the directory to look for PhysX meshes.
    // Modelled on the NiImageConverter class.
    static const char* GetPlatformSpecificSubdirectory();
    static void SetPlatformSpecificSubdirectory(const char* pcSubDir);

    // For converting mesh filenames to an appropriate name.
    static char* ConvertFilenameToPlatformSpecific(const char* pcPath);
    static void ConvertFilenameToPlatformSpecific(const NiFixedString& kPath,
        NiFixedString& kPlatformSpecificPath);
    
    // Mesh access functions. SHould be locked in in a multi-threaded
    // environment.
    NiPhysXConvexMesh* GetConvexMesh(const char* pcPath);
    NiPhysXConvexMesh* GetConvexMesh(const NxConvexMesh* pkMesh);
    void SetConvexMesh(NiPhysXConvexMesh* pkMesh);
    NiPhysXTriangleMesh* GetTriangleMesh(const char* pcPath);
    NiPhysXTriangleMesh* GetTriangleMesh(const NxTriangleMesh* pkMesh);
    void SetTriangleMesh(NiPhysXTriangleMesh* pkMesh);

    // The SDK object that is created. Not available until Initialize has been
    // called, nor after Shutdown has been called.
    NxPhysicsSDK* m_pkPhysXSDK;

protected:
    
    // Cannot create one of these directly - only through SDM
    NiPhysXManager();
    
    // And only destructed through the SDM
    ~NiPhysXManager();
    
    // Locking functions for the mesh tables.
    void WaitMeshLock();
    void ReleaseMeshLock();

    // Some platforms require specific initialization. This function is
    // implemented in platforms specific files.
    bool PlatformSpecificPostSDKInit();

    // Only non-NULL if the user did not provide their own versions to
    // Initialize().
    NiPhysXUserOutput* m_pkOutputStream;
    NiPhysXAllocator* m_pkAllocator;

    // The one and only instance of this class
    static NiPhysXManager* ms_pPhysXManager;
    
    // Mesh directory.
    static char* ms_pcPlatformSubDir;
    
    // Whether or not we own the SDK (ie whether we created it.)
    bool m_bOwnSDK;
    
    // For managing the locks
    NiCriticalSection m_kSDKCritSect;
    NiCriticalSection m_kMeshCritSect;
    
    // The list of scenes.
    // NiTObjectArray<NiPhysXScene*> m_kScenes;
    
    // Meshes
    NiTStringMap<NiPhysXConvexMeshPtr> m_kConvexMeshes;
    NiTStringMap<NiPhysXTriangleMeshPtr> m_kTriangleMeshes;

    friend class NiPhysXSDM;
};

#include "NiPhysXManager.inl"

#endif  // #ifndef NIPHYSXMANAGER_H

