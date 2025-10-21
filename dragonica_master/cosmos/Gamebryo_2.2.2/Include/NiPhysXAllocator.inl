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

// NiPhysXAllocator inline functions

//---------------------------------------------------------------------------
inline void* NiPhysXAllocator::malloc(NxU32 size)
{
    return NiMalloc(size); 
}
//---------------------------------------------------------------------------
inline void* NiPhysXAllocator::mallocDEBUG(NxU32 size,
    const char* fileName, int line)
{
#ifdef NI_MEMORY_DEBUGGER
    return _NiMalloc(size, fileName, line, __FUNCTION__);
#else
    return NiMalloc(size);
#endif // NI_MEMORY_DEBUGGER 
}    
//---------------------------------------------------------------------------
inline void* NiPhysXAllocator::realloc(void* memory, NxU32 size)
{
    return NiRealloc(memory, size);
}   
//---------------------------------------------------------------------------
inline void NiPhysXAllocator::free(void* memory)
{
    NIMEMASSERT(NULL != memory);
    NiFree(memory);
}
//---------------------------------------------------------------------------
