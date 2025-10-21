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

#include "MDisposable.h"
#include "MScene.h"
#include "ColorPicker.h"
#include "MViewport.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MPickUtility : public MDisposable
    {
    public:
        MPickUtility();

        bool PerformPick(MScene* pmScene, const NiPoint3& kOrigin,
            const NiPoint3& kDir, bool bIncludeFrozenEntities);
        const NiPick* GetNiPick();

        MEntity* GetEntityFromPickedObject(NiAVObject* pkPickedObject);

        MEntity* GetEntityFromViewCoordinates(MScene* pmScene, 
            MViewport* pmViewport, int iX, int iY);

        MEntity* PerformColorPick(MScene* pmScene, MEntity* pmCamera,
            unsigned int uiPickX, unsigned int uiPickY,
            bool bIncludeFrozenEntities);

	public:
		bool GetObserveAppCullFlag();
		void SetObserveAppCullFlag(bool bFlag);

    private:
        bool PickOnEntityRecursive(MEntity* pmEntity, const NiPoint3& kOrigin,
            const NiPoint3& kDir, MEntity* pmMainEntity);
        void ColorPickOnEntityRecursive(MEntity* pmEntity,
            unsigned int uiColor);

        NiPick* m_pkPick;
        NiFixedString* m_pkSceneRootPointerName;
        Hashtable* m_pmPickedObjectToEntity;

        ColorPicker* m_pkColorPicker;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
