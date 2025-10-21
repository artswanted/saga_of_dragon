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
#include "IMessageService.h"
#include "MEntity.h"
#include "MScene.h"
#include "MSelectionSet.h"
#include "MBatchCommand.h"
#include "MPalette.h"
#include "MViewport.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MEventManager : public MDisposable
    {
    public:
        // Events definitions.
        __event void EntityPropertyAdded(MEntity* pmEntity,
            String* strPropertyName, bool bInBatch);
        __event void EntityPropertyRemoved(MEntity* pmEntity,
            String* strPropertyName, bool bInBatch);
        __event void EntityPropertyChanging(MEntity* pmEntity, 
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void EntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void ComponentPropertyAdded(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        __event void ComponentPropertyRemoved(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        __event void ComponentPropertyChanging(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void ComponentPropertyChanged(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        __event void EntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        __event void EntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);
        __event void SelectedEntitiesChanged();
        __event void NewSceneLoaded(MScene* pmScene);
        __event void EntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        __event void EntityRemovedFromScene(MScene* pmScene,
            MEntity* pmEntity);
        __event void SelectionSetAddedToScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        __event void SelectionSetRemovedFromScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        __event void EntityNameChanging(MEntity* pmEntity, String* strNewName,
            bool bInBatch);
        __event void EntityNameChanged(MEntity* pmEntity, String* strOldName,
            bool bInBatch);
        __event void PaletteAdded(MPalette* pmPalette);
        __event void PaletteRemoved(MPalette* pmPalette);
        __event void PaletteActiveEntityChanged(MPalette* pmPalette,
            MEntity* pmOldActiveEntity);
        __event void ActivePaletteChanged(MPalette* pmActivePalette,
            MPalette* pmOldActivePalette);
        __event void SelectionSetNameChanging(MSelectionSet* pmSelectionSet,
            String* strNewName, bool bInBatch);
        __event void SelectionSetNameChanged(MSelectionSet* pmSelectionSet,
            String* strOldName, bool bInBatch);
        __event void ViewportCameraChanged(MViewport* pmViewport, 
            MEntity* pmCamera);
        __event void ViewportRenderingModeChanged(MViewport* pmViewport,
            IRenderingMode* pmRenderingMode);
        __event void ActiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        __event void ExclusiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        __event void CameraAddedToManager(MEntity* pmCamera);
        __event void CameraRemovedFromManager(MEntity* pmCamera);
        __event void EntityHiddenStateChanged(MEntity* pmEntity, bool bHidden);
        __event void EntityPgPropertyStateChanged(MEntity* pmEntity, String *strProperty);
        __event void EntityPgUsePhysXChanged(MEntity* pmEntity, bool bUsePhysX);
        __event void EntityPgPostfixTextureChanged(MEntity* pmEntity,  String *strPostfixTexture);
        __event void EntityPgAlphaGroupChanged(MEntity* pmEntity, String *strAlphaGroup);
        __event void EntityPgRandomAniChanged(MEntity* pmEntity, bool bRandomAni);
        __event void EntityPgOptimizationChanged(MEntity* pmEntity, String *strOptimization);
        __event void EntityFrozenStateChanged(MEntity* pmEntity,
            bool bFrozen);

		__event void ComponentServiceChanged(MComponent* pmComponent);
        __event void LongOperationStarted();
        __event void LongOperationCompleted();

        // Raising functions.
        void RaiseEntityPropertyAdded(MEntity* pmEntity, 
            String* strPropertyName, bool bInBatch);
        void RaiseEntityPropertyRemoved(MEntity* pmEntity, 
            String* strPropertyName, bool bInBatch);
        void RaiseEntityPropertyChanging(MEntity* pmEntity, 
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseEntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseComponentPropertyAdded(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        void RaiseComponentPropertyRemoved(MComponent* pmComponent,
            String* strPropertyName, bool bInBatch);
        void RaiseComponentPropertyChanging(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseComponentPropertyChanged(MComponent* pmComponent,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void RaiseEntityComponentAdded(MEntity* pmEntity,
            MComponent* pmComponent);
        void RaiseEntityComponentRemoved(MEntity* pmEntity,
            MComponent* pmComponent);
        void RaiseSelectedEntitiesChanged();
        void RaiseNewSceneLoaded(MScene* pmScene);
        void RaiseEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void RaiseEntityRemovedFromScene(MScene* pmScene,
            MEntity* pmEntity);
        void RaiseSelectionSetAddedToScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        void RaiseSelectionSetRemovedFromScene(MScene* pmScene,
            MSelectionSet* pmSelectionSet);
        void RaiseEntityNameChanging(MEntity* pmEntity, String* strNewName,
            bool bInBatch);
        void RaiseEntityNameChanged(MEntity* pmEntity, String* strOldName,
            bool bInBatch);
        void RaisePaletteAdded(MPalette* pmPalette);
        void RaisePaletteRemoved(MPalette* pmPalette);
        void RaisePaletteActiveEntityChanged(MPalette* pmPalette,
            MEntity* pmOldActiveEntity);
        void RaiseActivePaletteChanged(MPalette* pmActivePalette,
            MPalette* pmOldActivePalette);       
        void RaiseSelectionSetNameChanging(MSelectionSet* pmSelectionSet,
            String* strNewName, bool bInBatch);
        void RaiseSelectionSetNameChanged(MSelectionSet* pmSelectionSet,
            String* strOldName, bool bInBatch);
        void RaiseViewportCameraChanged(MViewport* pmViewport,
            MEntity* pmCamera);
        void RaiseViewportRenderingModeChanged(MViewport* pmViewport,
            IRenderingMode* pmRenderingMode);
        void RaiseActiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        void RaiseExclusiveViewportChanged(MViewport* pmOldViewport,
            MViewport* pmNewViewport);
        void RaiseCameraAddedToManager(MEntity* pmCamera);
        void RaiseCameraRemovedFromManager(MEntity* pmCamera);
        void RaiseEntityHiddenStateChanged(MEntity* pmEntity, bool bHidden);
		// PG
        void RaiseEntityPgPropertyStateChanged(MEntity* pmEntity, String *strProperty);
        void RaiseEntityPgUsePhysXChanged(MEntity* pmEntity, bool bUsePhysX);
        void RaiseEntityPgPostfixTextureChanged(MEntity* pmEntity, String *strPostfixTexture);
        void RaiseEntityPgAlphaGroupChanged(MEntity* pmEntity, String *strAlphaGroup);
        void RaiseEntityPgOptimizationChanged(MEntity* pmEntity, String *strOptimization);
        void RaiseEntityPgRandomAniChanged(MEntity* pmEntity, bool bRandomAni);
        void RaiseEntityFrozenStateChanged(MEntity* pmEntity, bool bFrozen);
        void RaiseComponentServiceChanged(MComponent* pmComponent);
        void RaiseLongOperationStarted();
        void RaiseLongOperationCompleted();

    private:
        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MEventManager* get_Instance();
    private:
        static MEventManager* ms_pmThis = NULL;
        MEventManager();
    };
}}}}
