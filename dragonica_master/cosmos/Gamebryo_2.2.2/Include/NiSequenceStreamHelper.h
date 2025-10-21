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

#ifndef NISEQUENCESTREAMHELPER_H
#define NISEQUENCESTREAMHELPER_H

#include "NiAnimationLibType.h"
#include <NiObjectNET.h>

// NOTICE: This class is deprecated and is included only to support streaming
//         of KF files from NetImmerse 4.0 or earlier.

class NIANIMATION_ENTRY NiSequenceStreamHelper : public NiObjectNET
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    NiSequenceStreamHelper() { /**/ };
};

#endif // NISEQUENCESTREAMHELPER_H
