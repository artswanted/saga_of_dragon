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

#ifndef NIOBJECTGROUP_H
#define NIOBJECTGROUP_H

#include "NiMainLibType.h"
#include "NiMemManager.h"
#include <NiMemObject.h>

class NIMAIN_ENTRY NiObjectGroup : public NiMemObject
{
public:
    NiObjectGroup(unsigned int uiBytes = 0);
    ~NiObjectGroup();

    void* Allocate(unsigned int uiBytes);
    void DecRefCount();

    void SetSize(unsigned int uiSize);
    unsigned int GetSize() const;

private:
    unsigned int m_uiSize;
    void* m_pvBuffer;
    void* m_pvFree;
    unsigned int m_uiRefCount;
};

#include "NiObjectGroup.inl"

#endif // NIOBJECTGROUP_H
