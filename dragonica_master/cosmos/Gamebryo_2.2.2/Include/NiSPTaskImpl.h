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

#ifndef NISPTASKIMPL_H
#define NISPTASKIMPL_H

#include "NiFloodgateLibType.h"
//---------------------------------------------------------------------------
class NiSPTask;
class NIFLOODGATE_ENTRY NiSPTaskImpl : public NiMemObject
{
public: 

    // Initialize pools
    static void InitializePools(NiUInt32 uiPartitionPoolSize);
    
    // Shutdown pool
    static void ShutdownPools();

    // Constructor
    NiSPTaskImpl();
    ~NiSPTaskImpl();

    // Clear
    void Clear();

    virtual void Initialize(NiSPTask* pkTask);
protected:
    
};
#include "NiSPTaskImpl.inl"
#endif
