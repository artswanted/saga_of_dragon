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

#pragma once

#include "MTransition.h"
#include "NiAnimationEventSound.H"
#include "NiAnimationEventEffect.H"
#include "NiAnimationEventTextKey.H"
#include "NiAnimationEventFadeOutEffect.H"

namespace NiManagedToolInterface
{
    public __gc class MAnimation;
    public __gc class MSequence : public System::IComparable
    {
    public:
        int CompareTo(Object* pkObject);

        static const float INVALID_TIME = -NI_INFINITY;

		__gc class MTextKey
        {
		public:
			__gc class MAudioData
			{
			public:
				MAudioData(String *pkName, float fVol, float fMin, float fMax);
				MAudioData(NiAnimationEventSound *pkSound);
				~MAudioData();

				__property String *get_SoundName();
				__property float get_Volume();
				__property float get_MinDistance();
				__property float get_MaxDistance();
				
			protected:
				bool m_bManualDelete;
				NiAnimationEventSound *m_pkSound;
			};

			__gc class MEffectData
			{
			public:
				MEffectData(String *pkName, String *pkAttachPoint, float fScale, bool bAttach, bool bLoop);
				MEffectData(NiAnimationEventEffect *pkEffect);
				~MEffectData();

				__property String *get_EffectName();
				__property String *get_AttachPoint();
				__property float get_Scale();
				__property bool get_UseFadeIn();
				__property float get_FadeInTime();
				__property bool get_UseFadeOutWhenSequenceChanged();
				__property float get_FadeOutTimeWhenSequenceChanged();
				
			protected:
				bool m_bManualDelete;
				NiAnimationEventEffect *m_pkEffect;
			};

			__gc class MFadeOutEffectData
			{
			public:
				MFadeOutEffectData(String *pkEffectName, float fFadeOutTime);
				MFadeOutEffectData(NiAnimationEventFadeOutEffect *pkFadeOutEffect);
				~MFadeOutEffectData();

				__property String *get_EffectName();
				__property float get_FadeOutTime();
				
			protected:
				bool m_bManualDelete;
				NiAnimationEventFadeOutEffect *m_pkFadeOutEffect;
			};

			MTextKey(NiTextKey* pkKey);
			MTextKey(float fTime, String *pkKeyName);
			~MTextKey();

			__property float get_Time();
			__property void set_Time(float fTime);
            __property String* get_Text();
			__property Object *get_ExtraData();
			__property void set_ExtraData(Object *pkObject);
            
		protected:
			float m_fTime;
			String* m_strText;
			Object* m_pkExtraData;
		};

        __gc class MInvalidSequenceInfo
        {
        public:
            unsigned int SequenceID;
            String* Filename;
            String* OriginalFilename;
            int AnimIndex;
        };

        __gc class MInterpolatorInfo
        {
        public:
            __value enum KeyContent: unsigned char
            {
                FLOATKEY = NiAnimationKey::FLOATKEY,
                POSKEY = NiAnimationKey::POSKEY,
                ROTKEY = NiAnimationKey::ROTKEY,
                COLORKEY = NiAnimationKey::COLORKEY,
                TEXTKEY = NiAnimationKey::TEXTKEY,
                BOOLKEY = NiAnimationKey::BOOLKEY,
                NUMKEYCONTENTS  = NiAnimationKey::NUMKEYCONTENTS
            };

            __value enum KeyType : unsigned char
            {
                NOINTERP  = NiAnimationKey::NOINTERP,
                LINKEY  = NiAnimationKey::LINKEY,
                BEZKEY  = NiAnimationKey::BEZKEY,
                TCBKEY  = NiAnimationKey::TCBKEY,
                EULERKEY  = NiAnimationKey::EULERKEY,
                STEPKEY = NiAnimationKey::STEPKEY,
                NUMKEYTYPES = NiAnimationKey::NUMKEYTYPES
            };
            MInterpolatorInfo(NiInterpolator* pkInterp,
                NiControllerSequence::IDTag* pkTag);
            // Properties.
            __property String* get_RTTI();
            __property String* get_AVObjectName();
            __property String* get_PropertyType();
            __property String* get_ControllerType();
            __property String* get_ControllerID();
            __property String* get_InterpolatorID();
                        
            unsigned short GetKeyChannelCount();
            unsigned int GetKeyCount(unsigned short usChannel);
            KeyType GetKeyType(unsigned short usChannel);
            KeyContent GetKeyContent(unsigned short usChannel);
            bool GetChannelPosed(unsigned short usChannel);
            unsigned int GetAllocatedSize(unsigned short usChannel);
            bool IsBSplineInterpolator();
            unsigned int GetDimension(unsigned short usChannel);
            unsigned int GetDegree(unsigned short usChannel);
			
        protected:
            NiInterpolator* m_pkInterpolator;
            NiControllerSequence::IDTag* m_pkIDTag;
        };

        // Events.
        __value enum PropertyType : unsigned char
        {
            Prop_SequenceID,
            Prop_Name,
            Prop_Filename,
            Prop_AnimIndex,
			Prop_Activated
        };

		__value enum AnimState: unsigned char
		{
			INACTIVE = NiControllerSequence::INACTIVE,
			ANIMATING  = NiControllerSequence::ANIMATING,
            EASEIN = NiControllerSequence::EASEIN,
            EASEOUT = NiControllerSequence::EASEOUT,
            TRANSSOURCE = NiControllerSequence::TRANSSOURCE,
            TRANSDEST = NiControllerSequence::TRANSDEST,
			MORPHSOURCE  = NiControllerSequence::MORPHSOURCE
		};
        
        // Properties.
        __property unsigned int get_SequenceID();
        __property void set_SequenceID(unsigned int uiSequenceID);
        __property String* get_Name();
        __property String* get_Filename();
        __property void set_Filename(String* strFilename);
        __property int get_AnimIndex();
        __property void set_AnimIndex(int iAnimIndex);
        __property bool get_Loop();
        __property float get_Frequency();
        __property float get_BeginKeyTime();
        __property float get_EndKeyTime();
        __property float get_Length();
        __property float get_LastTime();
        __property float get_LastScaledTime();
        __property MTransition* get_Transitions()[];
        __property MInterpolatorInfo* get_InterpolatorInfo()[];
		__property MTextKey* get_TextKeys()[];

        __property float get_BeginKeyTimeDivFreq();
        __property float get_EndKeyTimeDivFreq();

        __property float get_DefaultPositionTime();
        __property void set_DefaultPositionTime(float fTime);

#ifdef NDL_DEBUG
        __property void set_Frequency(float fFrequency);
#endif
		AnimState GetAnimState();

        MSequence(MAnimation* pkAnimation, 
            NiKFMTool::Sequence* pkSequence,
            NiControllerSequence* pkControllerSequence);
        void DeleteContents();

        void RebuildTransitionsArray();
        MTransition* GetTransition(unsigned int uiDesID);

        float GetKeyTimeAt(String* strKeyText);
        float GetKeyTimeAtDivFreq(String* strKeyText);
        float TimeDivFreq(float fTime);
        float TimeMultFreq(float fTime);

		String*	SaveCopy(String *strNewName);

        // To support animation accumulation caching.
        void StoreAccum();
        void RestoreAccum();

        bool IsDeleted();
        virtual String* ToString();

        bool CanSyncTo(MSequence* pkSequence);
        float FindCorrespondingMorphFrame(MSequence* pkPartnerSequence,
            float fFrameTime);

        NiControllerSequence* GetControllerSequence();
		void RefreshTextKeys();

    protected:
        NiKFMTool::Sequence* m_pkSequence;
        NiControllerSequence* m_pkControllerSequence;

        MTransition* m_aTransitions[];

        NiQuatTransform* m_pkStoredAccum;
        MAnimation* m_pkAnimation;
        bool m_bDisposed;

        void FillArrays();
		void ClearArrays();
        MInterpolatorInfo* m_aInterpolatorInfo[];
		MTextKey* m_aTextKeys[];

        float m_fDefaultPositionTime;
    };
}
