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

#ifndef NIINITOPTIONS_H
#define NIINITOPTIONS_H

#include "NiSystemLibType.h"
#include "NiMemManager.h"

class NISYSTEM_ENTRY NiInitOptions
{
public:
    NiInitOptions();
    NiInitOptions(NiAllocator* pkAllocator);

    ~NiInitOptions();

    NiAllocator* GetAllocator() const;
private:
    NiAllocator* m_pkAllocator;
    bool m_bAllocatedInternally;
};

#endif // #ifndef NIINITOPTIONS_H
