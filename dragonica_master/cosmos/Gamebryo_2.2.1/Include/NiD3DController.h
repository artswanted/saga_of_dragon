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

#ifndef NID3DCONTROLLER_H
#define NID3DCONTROLLER_H

#include "NiD3DDefines.h"
#include <NiTimeController.h>
#include <NiTPointerList.h>
#include <NiObjectNET.h>

NiSmartPointer(NiD3DController);

class NID3D_ENTRY NiD3DController : public NiTimeController
{
    NiDeclareRTTI;

public:
    NiD3DController();
    virtual ~NiD3DController();

    static void RemoveBeforeStreaming(NiObjectNET* pkRoot);
    static void AddAfterStreaming();
protected:
    static NiTPointerList<NiObjectNETPtr>* ms_pkObjectsList;
    static NiTPointerList<NiD3DControllerPtr>* ms_pkControllersList;
};

typedef NiPointer<NiD3DController> NiD3DControllerPtr;

#endif  // #ifndef NID3DCONTROLLER_H
