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

#ifndef NIKEYFRAMEMANAGER_H
#define NIKEYFRAMEMANAGER_H

#include <NiTimeController.h>
#include "NiAnimationLibType.h"
#include "NiSequence.h"
#include <NiTStringMap.h>

NiSmartPointer(NiKeyframeManager);

// NOTICE: This class is deprecated. You should use NiControllerManager
//         instead.

class NIANIMATION_ENTRY NiKeyframeManager : public NiTimeController
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    NiKeyframeManager();

    virtual bool TargetIsRequiredType() const;

    bool Add(const char* pcSequenceFile, const char* pcSequenceName,
        unsigned int uiIndex = 0);
    bool Add(NiSequence* pkSequence);

    virtual void Update(float fTime){}

    NiTStringPointerMap<NiSequencePtr> m_kMap;

    // This LinkRecord class is for backwards compatibility only.
    // It can be removed when loading NIF GetFileVersion < 4.1.0.3 is
    // no longer supported.
    class NIANIMATION_ENTRY LinkRecord : public NiMemObject
    {
    public:
        NiTPrimitiveArray<unsigned int> m_kTextKeyReferences;
        NiTPrimitiveArray<char*> m_kSequenceNames;
        NiTPrimitiveArray<unsigned int> m_kNumSequenceKeys;
        NiTPrimitiveArray<char*> m_kObjectName;
    };
    static LinkRecord* ms_pkLinkRecord;
};

#endif
