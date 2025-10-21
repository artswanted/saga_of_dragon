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

#ifndef NIREFOBJECT_H
#define NIREFOBJECT_H

#include "NiMainLibType.h"
#include <NiMemObject.h>

// NiRefObject provides the reference counting functionality needed for use
// of Gamebryo smart pointers. A class derived from NiRefObject can be used
// with the template class NiPointer.

class NIMAIN_ENTRY NiRefObject: public NiMemObject
{
public:
    NiRefObject();
    virtual ~NiRefObject();
    void IncRefCount();
    void DecRefCount();
    unsigned int GetRefCount() const;
    static unsigned int GetTotalObjectCount();
protected:
    virtual void DeleteThis();
private:
    unsigned int m_uiRefCount;
    static unsigned int ms_uiObjects;
};

#include "NiRefObject.inl"

#endif // NIREFOBJECT_H
