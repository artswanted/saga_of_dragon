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

#ifndef NIPHYSXALLOCATOR_H
#define NIPHYSXALLOCATOR_H

#include "NiPhysXLibType.h"

#include <NiSystem.h>
#include <NxUserAllocator.h>

// The interface between PhysX's memory manager and Gamebryo's memory manager.
class NIPHYSX_ENTRY NiPhysXAllocator :
    public NxUserAllocator, public NiMemObject
{

public:
    virtual void* malloc(NxU32 size);
    virtual void* mallocDEBUG(NxU32 size, const char* fileName, int line);    
    virtual void* realloc(void* memory, NxU32 size);   
    virtual void free(void* memory);
};

#include "NiPhysXAllocator.inl"

#endif // #ifndef NIPHYSXALLOCATOR_H
