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

#ifndef NIPHYSXSHAREDDATA_H
#define NIPHYSXSHAREDDATA_H

#include "NiPhysXSharedDataLibType.h"

#include <NiSharedData.h>
#include <NiPhysX.h>

/// A shared data object to contain a Renderer pointer.
class NIPHYSXSHAREDDATA_ENTRY NiPhysXSharedData : public NiSharedData
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiPhysXSharedData();

    /// Virtual destructor.
    virtual ~NiPhysXSharedData();

    // Get and set the PhysX scene
    NiPhysXScene* GetScene();
    void SetScene(NiPhysXScene* pkScene);

protected:
    // A smart pointer to the scene.
    NiPhysXScenePtr m_spScene;
};

NiSmartPointer(NiPhysXSharedData);

#endif  // #ifndef NIPHYSXSHAREDDATA_H
