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

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MToolbarListener : public MDisposable
    {
    public:
        // constructor
        MToolbarListener();

        // settings changed callback
        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        // command handlers
		[UICommandHandlerAttribute("DisablePhysXPick")]
        void DisablePhysXPickSnapHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("TranslateSnapToggle")]
        void TranslateSnapHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("RotateSnapToggle")]
        void RotateSnapHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("ScaleSnapToggle")]
        void ScaleSnapHandler(Object* pmObject, EventArgs* mArgs);

        [UICommandHandlerAttribute("TranslatePrecisionToggle")]
        void TranslatePrecisionHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("ScalePrecisionToggle")]
        void ScalePrecisionHandler(Object* pmObject, EventArgs* mArgs);

        [UICommandHandlerAttribute("SnapToSurfaceToggle")]
        void SnapToSurfaceHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("SnapToPointToggle")]
        void SnapToPointHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("AlignToSurfaceToggle")]
        void AlignToSurfaceHandler(Object* pmObject, EventArgs* mArgs);

        [UICommandHandlerAttribute("LookAtSelection")]
        void LookAtSelectionHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("MoveToSelection")]
        void MoveToSelectionHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("ZoomExtentsAll")]
        void ZoomExtentsAllHandler(Object* pmObject, EventArgs* mArgs);

        [UICommandHandlerAttribute("OptimizeSelectedLight")]
        void OptimizeLightHandler(Object* pmObject, EventArgs* mArgs);
		[UICommandHandlerAttribute("CopyLightPropertyByPick")]
        void CopyLightPropertyByPickHandler(Object* pmObject, EventArgs* mArgs);
        [UICommandHandlerAttribute("SelectLightsAffectingEntity")]
        void SelectAffectingLightsHandler(Object* pmObject, EventArgs* mArgs);

		[UICommandHandlerAttribute("CameraChange")]
		void CameraChangeHandler(Object* pmObject, EventArgs* mArgs);

        // tool button validators
		[UICommandValidatorAttribute("DisablePhysXPick")]
        void DisablePhysXPickSnapValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("TranslateSnapToggle")]
        void TranslateSnapValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("RotateSnapToggle")]
        void RotateSnapValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("ScaleSnapToggle")]
        void ScaleSnapValidator(Object* pmSender, UIState* pmState);

        [UICommandValidatorAttribute("TranslatePrecisionToggle")]
        void TranslatePrecisionValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("ScalePrecisionToggle")]
        void ScalePrecisionValidator(Object* pmSender, UIState* pmState);

        [UICommandValidatorAttribute("SnapToSurfaceToggle")]
        void SnapToSurfaceValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("SnapToPointToggle")]
        void SnapToPointValidator(Object* pmSender, UIState* pmState);
        [UICommandValidatorAttribute("AlignToSurfaceToggle")]
        void AlignToSurfaceValidator(Object* pmSender, UIState* pmState);

        [UICommandValidatorAttribute("LookAtSelection")]
        void LookAtSelectionValidator(Object* pmSender, UIState* pmState);

        [UICommandValidatorAttribute("OptimizeSelectedLight")]
        void OptimizeLightValidator(Object* pmObject, UIState* pmState);
		[UICommandValidatorAttribute("CopyLightPropertyByPick")]
		void CopyLightPropertyByPickValidator(Object* pmobject, UIState* pmState);
        [UICommandValidatorAttribute("SelectLightsAffectingEntity")]
        void SelectAffectingLightValidator(Object* pmObject, UIState* pmState);

		[UICommandValidatorAttribute("CameraChange")]
		void CameraChangeValidator(Object* pmSender, UIState* pmState);

    protected:
        __property static IInteractionModeService*
            get_InteractionModeService();
        static IInteractionModeService* ms_pmInteractionModeService;

        __property static ISelectionService* get_SelectionService();
        static ISelectionService* ms_pmSelectionService;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        __property static IEntityPathService* get_EntityPathService();
        static IEntityPathService* ms_psEntityPathService;

        __property static IRenderingModeService* get_RenderingModeService();
        static IRenderingModeService* ms_pmRenderingModeService;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        void ZoomExtentsHelper(MViewport* pmViewport, NiBound* pkBound, 
            bool bTransition);
        void AddLightsToArray(MEntity* pmEntity, ArrayList* pmLights);

        NiFixedString* m_pkTranslationName;
        NiFixedString* m_pkRotationName;
        NiFixedString* m_pkStaticName;
        NiFixedString* m_pkOrthoWidthName;
        NiFixedString* m_pkFOVName;
        NiFixedString* m_pkAspectName;
        NiFixedString* m_pkOrthoName;
        NiFixedString* m_pkNearClipName;
        NiFixedString* m_pkFarClipName;
        NiFixedString* m_pkLightTypeName;
        NiFixedString* m_pkAttenuationConstantName;
        NiFixedString* m_pkAttenuationLinearName;
        NiFixedString* m_pkAttenuationQuadraticName;
        NiFixedString* m_pkOuterSpotAngleName;
        NiFixedString* m_pkInnerSpotAngleName;
        NiFixedString* m_pkSpotExponentName;
        NiFixedString* m_pkLightDirectionName;
        NiFixedString* m_pkAffectedEntitiesName;
        NiFixedString* m_pkSceneRootName;
        NiPoint3* m_pkUpAxis;
		//MEntity* m_pkSelection[];

		bool m_bDisablePhysXPickEnabled;
        bool m_bTranslateSnapEnabled;
        bool m_bRotateSnapEnabled;
        bool m_bScaleSnapEnabled;

        bool m_bAlignToSurfaceEnabled;
        bool m_bSnapToSurfaceEnabled;
        bool m_bSnapToPointEnabled;
		bool m_bPickLightEnabled;

        bool m_bTranslatePrecisionEnabled;
        bool m_bScalePrecisionEnabled;

        float m_fDefaultOrbitDistance;
        float m_fLightOptimizeThreshold;

        static String* ms_strLightThresholdName = "Light Optimization "
            "Threshold";
        static String* ms_strLightThresholdOption = "Light Settings.Light "
            "Optimization Threshold";
        static String* ms_strLightThresholdDescription = "Entities who are "
            "influenced by a light less than this number on a range of 0 to "
            "1, then those entities will be excluded from the light's "
            "affected list";

        bool m_bViewUndoable;

    // inherited from MDisposable
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
