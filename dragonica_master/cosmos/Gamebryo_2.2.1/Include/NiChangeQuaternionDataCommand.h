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

#ifndef NICHANGEQUATERNIONDATACOMMAND_H
#define NICHANGEQUATERNIONDATACOMMAND_H

#include "NiChangePropertyCommand.h"
#include <NiQuaternion.h>

class NIENTITY_ENTRY NiChangeQuaternionDataCommand :
    public NiChangePropertyCommand
{
public:
    NiChangeQuaternionDataCommand(
        NiEntityPropertyInterface* pkEntityPropertyInterface,
        const NiFixedString& kPropertyName, const NiQuaternion& kData,
        unsigned int uiPropertyIndex = 0);

private:
    NiQuaternion m_kNewData;
    NiQuaternion m_kOldData;

protected:
    // NiChangePropertyCommand overrides.
    virtual NiBool StoreOldData();
    virtual NiBool SetNewData();
    virtual NiBool SetOldData();
};

NiSmartPointer(NiChangeQuaternionDataCommand);

#endif // NICHANGEQUATERNIONDATACOMMAND_H
