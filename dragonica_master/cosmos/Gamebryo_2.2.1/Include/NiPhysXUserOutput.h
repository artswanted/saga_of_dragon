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

#ifndef NIPHYSXUSEROUTPUT_H
#define NIPHYSXUSEROUTPUT_H

#include "NiPhysxLibType.h"

#include <NiSystem.h>
#include <NxPhysics.h>

class NIPHYSX_ENTRY NiPhysXUserOutput :
    public NxUserOutputStream, public NiMemObject
{
public:
    virtual void reportError(NxErrorCode code, const char *message,
        const char *file, int line); 
    virtual NxAssertResponse reportAssertViolation (const char *message,
        const char *file, int line);
    virtual void print (const char *message);    
};

#endif  // #ifndef NIPHYSXUSEROUTPUT_H

