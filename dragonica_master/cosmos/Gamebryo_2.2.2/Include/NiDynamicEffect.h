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

#ifndef NIDYNAMICEFFECT_H
#define NIDYNAMICEFFECT_H

#include "NiAVObject.h"

class NiNode;

// note that these are stars, not smart pointers
typedef NiTPointerList<NiNode*> NiNodeList;

class NIMAIN_ENTRY NiDynamicEffect : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiDynamicEffect);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    // Note that the values for lights in this enumeration have been made to 
    // match those in NiShaderAttributeDesc for fast type checking. Do not move
    // them out of synch. Additionally, this value is stored as a char below to
    // avoid virtualization of the GetEffectType method. It is unlikely that 
    // this enum will exceed 255, but in that case m_ucEffectType should be 
    // shifted to a short.
    enum EffectType
    {
        AMBIENT_LIGHT = 0,
        POINT_LIGHT = 2,
        DIR_LIGHT = 3,
        SPOT_LIGHT = 4,
        TEXTURE_EFFECT = 5,
        MAX_TYPES = TEXTURE_EFFECT + 1,

        // Note - all vertex-based lights types _must_ come before
        // the texture effects.  This allows the enum below to be
        // used for fast type checking in the low-level renderers, i.e.
        // (pE->GetEffectType() <= MAX_LIGHT_TYPE) <---> IsKindOf(NiLight, pS)

        MAX_LIGHT_TYPE = SPOT_LIGHT
    };


    // construction and destruction
    NiDynamicEffect();
    virtual ~NiDynamicEffect();

    // attributes
    EffectType GetEffectType() const;
    
    void SetSwitch(bool bOn);
    bool GetSwitch() const;

    // affected objects
    const NiNodeList& GetAffectedNodeList();
    void AttachAffectedNode(NiNode* pkAffectedNode);
    void DetachAffectedNode(NiNode* pkAffectedNode);
    void DetachAllAffectedNodes();

    // Unaffected objects
    const NiNodeList& GetUnaffectedNodeList();
    void AttachUnaffectedNode(NiNode* pkUnaffectedNode);
    void DetachUnaffectedNode(NiNode* pkUnaffectedNode);
    void DetachAllUnaffectedNodes();
    bool IsUnaffectedNode(NiNode* pkPossibleNode);

    // *** begin Emergent internal use only ***

    // bookkeeping
    int GetIndex() const;
    void IncRevisionID();
    unsigned int GetRevisionID() const;
    void SetRevisionID(unsigned int uiRevID);

    void IncPushCount();
    void DecPushCount();
    int GetPushCount() const;

    // *** end Emergent internal use only ***

protected:
    bool m_bOn;
    unsigned char m_ucEffectType;

    // bookkeeping
    int m_iIndex;
    unsigned int m_uiPushCount;
    unsigned int m_uiRevID;
    static int ms_iNextIndex;
    
    // illuminated objects
    NiNodeList m_kAffectedNodeList;

    // unaffected objects
    NiNodeList m_kUnaffectedNodeList;

#if !defined(WIN32) && !defined(_XENON)
    static NiCriticalSection ms_kCritSec;
#endif
};

typedef NiPointer<NiDynamicEffect> NiDynamicEffectPtr;

// note that these are standard pointers, not smart pointers
typedef NiTPointerList<NiDynamicEffect*> NiDynamicEffectList;

#include "NiDynamicEffect.inl"

#endif // NIDYNAMICEFFECT_H

