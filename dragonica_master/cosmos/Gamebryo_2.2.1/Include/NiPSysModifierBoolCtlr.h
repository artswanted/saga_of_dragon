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

#ifndef NIPSYSMODIFIERBOOLCTLR_H
#define NIPSYSMODIFIERBOOLCTLR_H

#include "NiPSysModifierCtlr.h"

class NIPARTICLE_ENTRY NiPSysModifierBoolCtlr : public NiPSysModifierCtlr
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSysModifierBoolCtlr);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:

    virtual void Update(float fTime);

    // *** begin Emergent internal use only ***
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        bool bAccumulateAnimations = false, float fWeightThreshold = 0.0f, 
        unsigned char ucArraySize = 2) const;
    // *** end Emergent internal use only ***
protected:
    NiPSysModifierBoolCtlr(const char* pcModifierName);

    // For cloning and streaming only.
    NiPSysModifierBoolCtlr();

    // Virtual function overrides from base classes.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;

    virtual void GetTargetBoolValue(bool& bValue) = 0;
    virtual void SetTargetBoolValue(bool bValue) = 0;
};

NiSmartPointer(NiPSysModifierBoolCtlr);

#include "NiPSysModifierBoolCtlr.inl"

#endif  // #ifndef NIPSYSMODIFIERBOOLCTLR_H
