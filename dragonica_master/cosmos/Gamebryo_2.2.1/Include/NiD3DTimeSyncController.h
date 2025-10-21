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

#ifndef NID3DTIMESYNCCONTROLLER_H
#define NID3DTIMESYNCCONTROLLER_H

#include "NiD3DController.h"
#include <NiFloatExtraData.h>

class NID3D_ENTRY NiD3DTimeSyncController :
    public NiD3DController
{
    NiDeclareRTTI;
    NiDeclareClone(NiD3DTimeSyncController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiD3DTimeSyncController();
    virtual ~NiD3DTimeSyncController();

    virtual void Update(float fTime);

    // *** begin Emergent internal use only ***
    virtual void GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime);
    
    // Overridden from NiTimeController so that m_spExtraData can be set
    // appropriately.
    virtual void SetTarget(NiObjectNET* pkTarget);

    virtual bool CanBeExtracted() const;
    virtual bool TargetIsRequiredType() const;
    // *** end Emergent internal use only ***

    virtual bool IsStreamable() const;

protected:

    NiFloatExtraDataPtr m_spExtraData;

};

NiSmartPointer(NiD3DTimeSyncController);

#include "NiD3DTimeSyncController.inl"

#endif  // #ifndef NID3DTIMESYNCCONTROLLER_H
