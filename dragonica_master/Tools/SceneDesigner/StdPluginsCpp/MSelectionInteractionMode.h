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

#include "MViewInteractionMode.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
	public __gc class MSelectionInteractionMode : public MViewInteractionMode
    {
    public:
        MSelectionInteractionMode();

        void RegisterSettings();
        void OnSettingChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

    protected:
        static const float STANDARD_DISTANCE = 10.0f;

        NiPick::Record* m_pkPickRecord;
        float m_fDefaultDistance;
        NiNode* m_pkGizmo;
        void SetGizmoScale(NiCamera* pkCamera);

		// Rectangle Selection Variable
		bool m_bVisibleRectangleSelection;
		NiPoint2* m_pkMouseClickPos;
		NiPoint2* m_pkMouseMovePos;
		NiPoint2* m_pkMouseDownPos;
        NiLines* m_pkRectangle;
        NiMaterialProperty* m_pkRectangleMaterial;
        NiColor* m_pkHighlightColor;

		// HandOver, 사각 선택, 강정욱 2008.01.29
		// ReSelection Variable
		NiPoint2* m_pkPrevMousePos;
		float m_fPrevDistance;

		// Dragging Selection
		NiRect<float>* m_pkDragRect;
		NiFrustum* m_pkDragFrustum;
		NiVisibleArray* m_pkDragVA;
		NiCullingProcess* m_pkDragCuller;

    private:
        [UICommandHandlerAttribute("SelectInteractionMode")]
        void SetInteractionMode(Object* pmObject, EventArgs* mArgs);

        [UICommandValidatorAttribute("SelectInteractionMode")]
        void ValidateInteractionMode(Object* pmSender, UIState* pmState);

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IInteractionMode members.
    public:
        __property String* get_Name();
        bool Initialize();
        void RenderGizmo(MRenderingContext* pmRenderingContext);
        void MouseDown(MouseButtonType eType, int iX, int iY);
        void MouseUp(MouseButtonType eType, int iX, int iY);
        void MouseMove(int iX, int iY);
		
		// Rectangle Selection Function
		void CreateRectangleGeometry();
		void UpdateRectangleGeometry();
		void RegisterForHighlightColorSetting();

		// Connected Entities
		ArrayList* GetPileEntities(NiGeometry *pkGeometry, ArrayList *amExceptionEntities);
		ArrayList* GetSelectConnectedEntities(MEntity* pPickedEntity, ArrayList *amExceptionEntities);
    };
}}}}
