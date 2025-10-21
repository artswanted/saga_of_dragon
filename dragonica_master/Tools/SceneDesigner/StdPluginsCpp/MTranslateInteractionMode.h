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

#include "MSelectionInteractionMode.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MTranslateInteractionMode : 
        public MSelectionInteractionMode
    {
    public:
        MTranslateInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        __value enum TranslateAxis
        {
            AXIS_X = 0,
            AXIS_Y,
            AXIS_Z,
            PLANE_XY,
            PLANE_XZ,
            PLANE_YZ
        };

        [UICommandHandlerAttribute("TranslateInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("TranslateInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    protected:
        // setting for allowing us to invert default distance to a ratio
        static const float STANDARD_DISTANCE = 10.0f;

        // fixed strings for finding objects in the gizmo
        NiFixedString* m_pkXAxisName;
        NiFixedString* m_pkYAxisName;
        NiFixedString* m_pkZAxisName;
        NiFixedString* m_pkXYPlaneName;
        NiFixedString* m_pkXZPlaneName;
        NiFixedString* m_pkYZPlaneName;
        NiFixedString* m_pkXLineName;
        NiFixedString* m_pkYLineName;
        NiFixedString* m_pkZLineName;
        NiFixedString* m_pkXYLineName;
        NiFixedString* m_pkXZLineName;
        NiFixedString* m_pkYXLineName;
        NiFixedString* m_pkYZLineName;
        NiFixedString* m_pkZXLineName;
        NiFixedString* m_pkZYLineName;

        // values that need to be stored between frames
        TranslateAxis m_eAxis;
        TranslateAxis m_eCurrentAxis;
        bool m_bOnGizmo;
        bool m_bAlreadyTranslating;
        float m_fStartScale;
        NiPoint3* m_pkStartPoint;		// 마우스 찍었을때의 점의 좌표
        NiPoint3* m_pkStartPick;		// 마우스 찍었을때의 그 폴리곤이랑 겹치는 좌표
        int m_iMouseX;
        int m_iMouseY;

        // per scene settings
        float m_fSnapSpacing;
        bool m_bSnapEnabled;
        NiTObjectSet<NiPoint3>* m_pkInitialTranslation;
        NiTObjectSet<NiMatrix3>* m_pkInitialRotation;
        float m_fPrecision;
        bool m_bPrecisionEnabled;
        bool m_bSnapToPickEnabled;
        bool m_bSnapToPointPickEnabled;
        bool m_bRotateToPickEnabled;
        unsigned short m_usAlignFacingAxis;
        unsigned short m_usAlignUpAxis;

        // members for cloning
        ArrayList* m_pmPreviousSelection;
        bool m_bCloning;
        ArrayList* m_pmCloneArray;
        NiPoint3* m_pkCloneCenter;

        NiColor* m_pkHighLightColor;
        NiPick* m_pkPick;
		NiPick::Record* m_pkFinalSelectRecord;
		
		// 포인트 스냅
		bool m_bFoundSnapSrcPoint;			// 점 출력을 위한 스냅 포인트를 찾았는지
        NiPoint3 *m_pkSnapSrcPoint;			// 점 출력을 위한 점 위치
		NiPoint3 *m_pkSnapSrcMarkerVerts;	// 점 출력을 위한 점에서 4개의 점위치
		NiLines *m_pkSnapSrcMarker;			// 점 출력을 위한 라인
        NiPoint3 *m_pkSnapDestPoint;		// 선택한 물체가 아닌 붙일곳의 물체와 마우스와 급접한 점
		NiPick::Record* m_pkPickSrcRecord;	// 항상 바뀌어서 이용안함..
		NiPick::Record* m_pkPickDestRecord;	// 목표 물체
		NiGeometry* m_pkSrcGeometry;		// 선택 물체의 지오메트리
		bool m_bFoundSnapLineSrcPoint;			// 점 출력을 위한 스냅 포인트를 찾았는지
		NiPoint3 *m_pkSnapSrcLineVerts;			// 선택한 라인 (할당 2개)
		NiLines *m_pkSnapSrcLineMarker;		// 점 출력을 위한 라인

		NiPoint3 *m_pkSrcNearestPoint;
		NiPoint3 *m_pkSrcNearPoint;
		NiPoint3 *m_pkDestNearestPoint;
		NiPoint3 *m_pkDestNearPoint;
		NiMatrix3 *m_pkSrcRotation;			// 선택물체의 스냅전 로테이션
		NiPoint3 *m_pkSrcTranslate;			// 선택물체의 스냅전 트렌슬.
		NiMatrix3 *m_pkSrcMultiRotation;		// 단체 선택 물체의 스냅전 로테이션

		MEntity* m_pCloneSource;

        bool CanTransform();
        void TranslateHelper(const NiPoint3* pkOrigin, const NiPoint3* pkDir);
        TranslateAxis GetBestAxis(const TranslateAxis eAxis);
        void HighLightAxis(const TranslateAxis eAxis);
        void SetGizmoScale(NiCamera* pkCamera);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IInteractionMode members.
    public:
        __property String* get_Name();
        bool Initialize();
        void Update(float fTime);
        void RenderGizmo(MRenderingContext* pmRenderingContext);
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);

    };
}}}}
