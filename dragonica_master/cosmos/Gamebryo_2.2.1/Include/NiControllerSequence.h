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

#ifndef NICONTROLLERSEQUENCE_H
#define NICONTROLLERSEQUENCE_H

#include <NiObject.h>
#include "NiAnimationLibType.h"
#include "NiInterpController.h"
#include "NiTextKeyExtraData.h"
#include "NiBlendInterpolator.h"
#include "NiStringPalette.h"
#include <NiMatrix3.h>
#include <NiPoint3.h>

NiSmartPointer(NiControllerSequence);
typedef NiTObjectArray<NiControllerSequencePtr>     
    NiControllerSequencePointerArray;
typedef NiTObjectSet<NiControllerSequencePtr> NiControllerSequencePointerSet;

class NiNode;
class NiControllerManager;
class NiSequence;
class NiObjectNET;
class NiAVObjectPalette;
class NiTextKeyMatch;

class NIANIMATION_ENTRY NiControllerSequence : public NiObject
{
public:
    NiDeclareRTTI;
    NiDeclareClone(NiControllerSequence);
    NiDeclareStream;

    enum AnimState
    {
        INACTIVE,
        ANIMATING,
        EASEIN,
        EASEOUT,
        TRANSSOURCE,
        TRANSDEST,
        MORPHSOURCE
    };

    static const float INVALID_TIME;
    static const unsigned int INVALID_INDEX;

    // Constructor and destructor.
    NiControllerSequence(const NiFixedString& kName,
        unsigned int uiArraySize = 12,
        unsigned int uiArrayGrowBy = 12);
    ~NiControllerSequence();

    // Static functions for creating sequences from KF files.
    static NiControllerSequencePtr CreateSequenceFromFile(
        const char* pcFilename, unsigned int uiIndex);
    static NiControllerSequencePtr CreateSequenceFromFile(
        const NiStream& kStream, unsigned int uiIndex);
    static bool CreateAllSequencesFromFile(const char* pcFilename,
        NiControllerSequencePointerArray& kSequences);
    static bool CreateAllSequencesFromFile(const NiStream& kStream,
        NiControllerSequencePointerArray& kSequences);
    static NiControllerSequencePtr ConvertOldSequence(
        NiSequence* pkOldSequence);

    // Functions for getting and setting the sequence name.
    const NiFixedString& GetName() const;
    void SetName(const NiFixedString& kName);

    // Functions for getting and setting the text keys.
    NiTextKeyExtraData* GetTextKeys() const;
    void SetTextKeys(NiTextKeyExtraData* pkKeys);
	void AddTextKeys(NiTextKey *pkKeys, unsigned int uiNumTextKey);
	void RemoveTextKey(NiTextKey *pkTextKey);
	void ModifyTextKey(NiTextKey *pkTextKey, float fNewTime);

    // Function to reset the sequence to the beginning.
    void ResetSequence();

    // Function that returns the animation state for this sequence.
    AnimState GetState() const;

    // Functions for getting and setting the offset for this sequence.
    float GetOffset() const;
    void SetOffset(float fOffset);

    // Functions for adding and removing interpolators.
    unsigned int AddInterpolator(NiInterpolator* pkInterpolator,
        const NiFixedString& kAVObjectName, const NiFixedString& kPropertyType,
        const NiFixedString& kCtlrType, const NiFixedString& kCtlrID,
        const NiFixedString& kInterpolatorID);
    NiInterpolatorPtr RemoveInterpolator(unsigned int uiIndex);

    // Functions for retrieving interpolators and other information.
    unsigned int GetArraySize() const;
    void SetArraySize(unsigned int uiNewArraySize);
    unsigned int GetArrayGrowBy() const;
    void SetArrayGrowBy(unsigned int uiArrayGrowBy);
    NiInterpolator* GetInterpolatorAt(unsigned int uiIndex) const;
    void GetInterpInfoAt(unsigned int uiIndex, const char*& pcAVObjectName,
        const char*& pcPropertyType, const char*& pcCtlrType,
        const char*& pcCtlrID, const char*& pcInterpolatorID);
    NiInterpController* GetControllerAt(unsigned int uiIndex) const;
    NiObjectNET* GetTargetAt(unsigned int uiIndex) const;
    NiBlendInterpolator* GetBlendInterpolatorAt(unsigned int uiIndex) const;
    unsigned char GetBlendIdxAt(unsigned int uiIndex);

    // Functions for getting and setting the sequence weight.
    float GetSequenceWeight() const;
    void SetSequenceWeight(float fWeight);

    // Function for looking up the time of a text key.
    float GetTimeAt(const NiFixedString& kTextKey, float fCurrentTime = 0.0f);
	float GetTimeAt(NiTextKeyMatch * pkMatchObject, float fCurrentTime = 0.0f);

    // Function for looking up the key time for a particular text key.
    float GetKeyTimeAt(const NiFixedString& kTextKey) const;
	float GetKeyTimeAt(NiTextKeyMatch * pkMatchObject) const;

    // Adjusted functions adjust by frequency of sequence.
    float GetKeyTimeAtDivFreq(const NiFixedString& kTextKey) const;
    float GetBeginKeyTimeDivFreq() const;
    float GetEndKeyTimeDivFreq() const;
    float TimeDivFreq(float fTime) const;
    float TimeMultFreq(float fTime) const;

    // Functions for getting and setting timing variables.
    NiTimeController::CycleType GetCycleType() const;
    bool SetCycleType(NiTimeController::CycleType eType);
    float GetFrequency() const;
    void SetFrequency(float fFrequency);
    float GetBeginKeyTime() const;
    void SetBeginKeyTime(float fBeginKeyTime);
    float GetEndKeyTime() const;
    void SetEndKeyTime(float fEndKeyTime);
    float GetLength();

    // Functions to provide easy access to the last stored time values.
    float GetLastTime() const;
    float GetLastScaledTime() const;

    // *** Begin deprecated functions ***
    // Deprecated function for getting the object name at an index. It
    // concatenates the AVObjectName and PropertyType from the IDTag at the
    // specified index and returns a newly allocated string in pcObjectName.
    // NOTE: pcObjectName must be deleted by the calling function to avoid
    //       a memory leak.
    void GetObjectNameAt(unsigned int uiIndex, char*& pcObjectName) const;

    // Deprecated functions for adding and removing name/controller pairs.
    // New code should use AddInterpolator and RemoveInterpolator instead.
    bool AddNameControllerPair(const char* pcObjectName,
        NiTimeController* pkController);
    void RemoveNameControllerPair(unsigned int uiIndex);
    // *** End deprecated functions ***

    // *** begin Emergent internal use only ***
    class NIANIMATION_ENTRY IDTag : public NiMemObject
    {
    public:
        IDTag(const NiFixedString& kAVObjectName = NULL,
            const NiFixedString& kPropertyType = NULL,
            const NiFixedString& kCtlrType = NULL,
            const NiFixedString& kCtlrID = NULL,
            const NiFixedString& kInterpolatorID = NULL);
        IDTag(const IDTag& kOther);

        const NiFixedString& GetAVObjectName() const;
        void SetAVObjectName(const NiFixedString& kAVObjectName);

        const NiFixedString& GetPropertyType() const;
        void SetPropertyType(const NiFixedString& kPropertyType);

        const NiFixedString& GetCtlrType() const;
        void SetCtlrType(const NiFixedString& kCtlrType);

        const NiFixedString& GetCtlrID() const;
        void SetCtlrID(const NiFixedString& kCtlrID);

        const NiFixedString& GetInterpolatorID() const;
        void SetInterpolatorID(const NiFixedString& kInterpolatorID);

        IDTag& operator=(const IDTag& kTag);

        void ClearValues();

        // *** begin Emergent internal use only ***

        // Streaming functions.
        void RegisterStreamables(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void LoadBinary(NiStream& kStream,
             void*& pvDeprecatedExtraData);
        void HandleDeprecatedExtraData(NiStringPalette* pkPalette,
            NiStream& kStream);
        bool IsEqual(const IDTag& kDest);
        // *** begin Emergent internal use only ***

    protected:
        NiFixedString m_kAVObjectName;
        NiFixedString m_kPropertyType;
        NiFixedString m_kCtlrType;
        NiFixedString m_kCtlrID;
        NiFixedString m_kInterpolatorID;
    };
    unsigned int AddInterpolator(NiInterpolator* pkInterpolator,
        const IDTag& kIDTag);
    unsigned int GetIDTagCount();
    IDTag* GetIDTag(unsigned int uiTagIndex);

    void Update(float fTime, bool bUpdateInterpolators = true);

    NiControllerManager* GetOwner() const;
    bool SetOwner(NiControllerManager* pkOwner);

    bool StoreTargets(NiAVObject* pkRoot);
    void ClearTargets();

    bool Activate(char cPriority, bool bStartOver, float fWeight,
        float fEaseInTime, NiControllerSequence* pkTimeSyncSeq,
        bool bTransition);
    bool Deactivate(float fEaseOutTime, bool bTransition);

    bool StartBlend(NiControllerSequence* pkDestSequence, float fDuration,
        const char* pcDestTextKey, int iPriority, float fSourceWeight,
        float fDestWeight, NiControllerSequence* pkTimeSyncSeq);
    bool StartBlend(NiControllerSequence* pkDestSequence, float fDuration,
        float fDestFrame, int iPriority, float fSourceWeight,
        float fDestWeight, NiControllerSequence* pkTimeSyncSeq);

    bool StartMorph(NiControllerSequence* pkDestSequence, float fDuration,
        int iPriority, float fSourceWeight, float fDestWeight);

    void PopulatePoseSequence(NiControllerSequence* pkPoseSequence,
        unsigned int uiArraySize,
        NiControllerSequence* pkSequenceToSynchronize);

    void SetAccumRootName(const NiFixedString& kAccumName);
    const NiFixedString& GetAccumRootName() const;
    NiAVObject* GetAccumRoot() const;

    // This function is only called by NiControllerExtractor to support
    // storing controllers in NiControllerSequence objects for compatibility
    // reasons.
    void SetControllerAt(NiInterpController* pkInterpCtlr,
        unsigned int uiIndex);

    // This function is only called by the tools to post-process interpolators
    // once they've been attached to a sequence
    void ReplaceInterpolatorAt(NiInterpolator* pkInterp, 
        unsigned int uiIndex);

    bool CanSyncTo(NiControllerSequence* pkTargetSequence);

    float FindCorrespondingMorphFrame(NiControllerSequence* pkSourceSequence,
        float fSourceSequenceTime);
    float ComputeScaledTime(float fTime, bool bStoreLastTime = true);

    float GetEaseEndTime() const;

    // Cloning support.
    virtual void ProcessClone(NiCloningProcess& kCloning);
    virtual void PostLinkObject(NiStream& kStream);
    // *** end Emergent internal use only ***

protected:
    // For use with cloning and streaming only.
    NiControllerSequence();

    void SortByBlendInterpolator();

    // Internal storage structures.
    class InterpArrayItem : public NiMemObject
    {
    public:
        InterpArrayItem();

        void ClearValues();

        // Streaming functions.
        void RegisterStreamables(NiStream& kStream);
        void SaveBinary(NiStream& kStream);
        void LoadBinary(NiStream& kStream);
        bool IsEqual(const InterpArrayItem& kDest);

        NiInterpolatorPtr m_spInterpolator;
        NiInterpControllerPtr m_spInterpCtlr;
        NiBlendInterpolator* m_pkBlendInterp;
        unsigned char m_ucBlendIdx;
    };

    void AttachInterpolators(char cPriority = 0);
    void DetachInterpolators();

    bool ConvertOldNameControllerPair(const char* pcObjectName,
        NiInterpController* pkController);

    void SetInterpsWeightAndTime(float fWeight, float fEaseSpinner,
        float fTime);

    bool VerifyDependencies(NiControllerSequence* pkSequence);
    bool VerifyMatchingMorphKeys(NiControllerSequence* pkTimeSyncSeq);

    bool ReallocateArrays();
    bool ResolveTransformInterpolators(NiAVObject* pkRoot,
        NiAVObjectPalette* pkObjectPalette);

    // The name of this sequence.
    NiFixedString m_kName;

    // Storage arrays for interpolators and ID tags.
    unsigned int m_uiArraySize;
    unsigned int m_uiArrayGrowBy;
    InterpArrayItem* m_pkInterpArray;
    IDTag* m_pkIDTagArray;

    // The weight of the sequence.
    float m_fSeqWeight;

    // Text keys which point to times in the animation
    NiTextKeyExtraDataPtr m_spTextKeys;

    // Timing variables.
    NiTimeController::CycleType m_eCycleType;
    float m_fFrequency;
    float m_fBeginKeyTime;
    float m_fEndKeyTime;
    float m_fLastTime;
    float m_fWeightedLastTime;
    float m_fLastScaledTime;

    // The NiControllerManager that owns this sequence.
    NiControllerManager* m_pkOwner;

    // The current state of the sequence.
    AnimState m_eState;

    // Variables used for time measurement.
    float m_fOffset;
    float m_fStartTime;
    float m_fEndTime;
    float m_fDestFrame;

    // Variables used for time synchronization.
    NiControllerSequence* m_pkPartnerSequence;

    // Variables used for animation accumulation.
    NiFixedString m_kAccumRootName;
    NiAVObject* m_pkAccumRoot;

    // NiStringPalettes were used to allocate strings
    // for prior versions of the engine. This member
    // is here purely for successful streaming of old sequences
    // and will only be non-NULL if in streaming code.
    // Version begin use: 10, 1, 0, 113
    // Version end use:   20, 1, 0, 1
    NiStringPalettePtr m_spDeprecatedStringPalette;
};

#include "NiControllerSequence.inl"

#endif  // #ifndef NICONTROLLERSEQUENCE_H
