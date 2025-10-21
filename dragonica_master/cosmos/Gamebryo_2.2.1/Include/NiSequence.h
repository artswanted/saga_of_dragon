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

#ifndef NISEQUENCE_H
#define NISEQUENCE_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiTransformController.h"
#include "NiTextKeyExtraData.h"

class NiKeyframeManager;

NiSmartPointer(NiSequence);

// NOTICE: This class is deprecated. You should use NiControllerSequence
//         instead.

class NIANIMATION_ENTRY NiSequence : public NiObject
{
    friend class NiKeyframeManager;

    NiDeclareRTTI;
    NiDeclareStream;

public:
    ~NiSequence();

    static NiSequencePtr CreateFromKFFile(const char* pcSequenceFile,
        unsigned int uiIndex, const char* pcSequenceName = NULL);
    static NiSequencePtr CreateFromKFFile(const NiStream& kStream,
        unsigned int uiIndex, const char* pcSequenceName = NULL);

    const char* GetName() const;
    unsigned int GetCount() const;
    const char* GetObjectName(unsigned int uiIndex);
    NiTransformController* GetController(unsigned int uiIndex);
    NiTextKeyExtraData* GetTextKeys() const;

protected:
    // Constructor is protected to eliminate instantiation of this class
    // ouside of stream loading.
    NiSequence(const char* pcName, unsigned int uiInitSize,
        unsigned int uiGrowBy);

    static NiSequence* ConvertSequence(const NiStream& kStream,
        unsigned int uiIndex, const char* pcSequenceName = NULL);

    unsigned int AddNameControllerPair(const char* pcName, 
        NiTransformController* pkController);
    void SetName(const char* pcName);
    void SetTextKeyReference(unsigned int uiReference);
    void SetTextKeys(NiTextKeyExtraData* pkData);

    // Sequence name
    char* m_pcName;

    // Animation data
    NiTPrimitiveArray<char*> m_kObjectName;
    NiTObjectArray<NiTransformControllerPtr> m_kCtrl;

    // Text keys which point to times in the animation
    NiTextKeyExtraDataPtr m_spTextKeys;

    // Index of an NiKeyframeController with timing values the text keys
    //   refer to (eg CycleType, start/end times, etc)
    unsigned int m_uiTextKeyReference;
};

#include "NiSequence.inl"

#endif  // #ifndef NISEQUENCE_H
