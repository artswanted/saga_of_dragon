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

#ifndef NICHANGEENTITYPOINTERDATACOMMAND_H
#define NICHANGEENTITYPOINTERDATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include "NiEntityInterface.h"
#include <NiObject.h>

class NIENTITY_ENTRY NiChangeEntityPointerDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeEntityPointerDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, NiEntityInterface* pkData,
        unsigned int uiPropertyIndex = 0);

private:
    NiEntityInterfaceIPtr m_spNewData;
    NiEntityInterfaceIPtr m_spOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeEntityPointerDataCommand);

#endif // NICHANGEENTITYPOINTERDATACOMMAND_H
