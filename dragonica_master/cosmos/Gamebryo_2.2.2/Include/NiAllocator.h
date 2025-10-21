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

#ifndef NIALLOCATOR_H
#define NIALLOCATOR_H

#include "NiSystemLibType.h"
#include "NiMemoryDefines.h"

class NISYSTEM_ENTRY NiAllocator
{
public:
    virtual ~NiAllocator(){};
    // Encapsulate all memory management through a 
    // single set of API calls.

    // Note that the size and alignment are passed-by-reference. 
    // This allows the allocator to adjust the values
    // internally and pass the results back to the caller.
    virtual void* Allocate(
        size_t& stSizeInBytes, 
        size_t& stAlignment,
        NiMemEventType eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        const char* pcFile, 
        int iLine,
        const char* pcFunction) = 0;

    virtual void Deallocate(
        void* pvMemory, 
        NiMemEventType eEventType, 
        size_t stSizeInBytes) = 0;

    virtual void* Reallocate(
        void* pvMemory, 
        size_t& stSizeInBytes,
        size_t& stAlignment, 
        NiMemEventType eEventType, 
        bool bProvideAccurateSizeOnDeallocate,
        size_t stSizeCurrent,
        const char* pcFile, int iLine,
        const char* pcFunction) = 0;

    // Called by the memory manager during NiInit()
    virtual void Initialize() = 0;
    // Called by the memory manager during NiShutdown()
    virtual void Shutdown() = 0;

    // Called by the memory manager to verify a memory address
    virtual bool VerifyAddress(const void* pvMemory) = 0;
};

#endif
