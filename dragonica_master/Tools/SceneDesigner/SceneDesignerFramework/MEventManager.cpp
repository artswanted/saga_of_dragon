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

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MEventManager.h"
#include "ServiceProvider.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
void MEventManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MEventManager();
    }
}
//---------------------------------------------------------------------------
void MEventManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MEventManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MEventManager* MEventManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MEventManager::MEventManager()
{
}
//---------------------------------------------------------------------------
void MEventManager::Do_Dispose(bool bDisposing)
{
}
//---------------------------------------------------------------------------
IMessageService* MEventManager::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyAdded(MEntity* pmEntity, 
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyAdded(pmEntity, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyRemoved(MEntity* pmEntity, 
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyRemoved(pmEntity, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyChanging(MEntity* pmEntity, 
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyChanging(pmEntity, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPropertyChanged(MEntity* pmEntity,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    EntityPropertyChanged(pmEntity, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyAdded(MComponent* pmComponent,
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyAdded(pmComponent, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyRemoved(MComponent* pmComponent,
    String* strPropertyName, bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyRemoved(pmComponent, strPropertyName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyChanging(MComponent* pmComponent,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyChanging(pmComponent, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentPropertyChanged(MComponent* pmComponent,
    String* strPropertyName, unsigned int uiPropertyIndex,
    bool bInBatch)
{
    MVerifyValidInstance;

    ComponentPropertyChanged(pmComponent, strPropertyName, uiPropertyIndex,
        bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityComponentAdded(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    EntityComponentAdded(pmEntity, pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityComponentRemoved(MEntity* pmEntity,
    MComponent* pmComponent)
{
    MVerifyValidInstance;

    EntityComponentRemoved(pmEntity, pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectedEntitiesChanged()
{
    MVerifyValidInstance;

    SelectedEntitiesChanged();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseNewSceneLoaded(MScene* pmScene)
{
    MVerifyValidInstance;

    NewSceneLoaded(pmScene);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityAddedToScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    EntityAddedToScene(pmScene, pmEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityRemovedFromScene(MScene* pmScene,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    EntityRemovedFromScene(pmScene, pmEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetAddedToScene(MScene* pmScene,
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    SelectionSetAddedToScene(pmScene, pmSelectionSet);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetRemovedFromScene(MScene* pmScene,
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    SelectionSetRemovedFromScene(pmScene, pmSelectionSet);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityNameChanging(MEntity* pmEntity,
    String* strNewName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityNameChanging(pmEntity, strNewName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityNameChanged(MEntity* pmEntity,
    String* strOldName, bool bInBatch)
{
    MVerifyValidInstance;

    EntityNameChanged(pmEntity, strOldName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteAdded(MPalette* pmPalette)
{
    MVerifyValidInstance;

    PaletteAdded(pmPalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteRemoved(MPalette* pmPalette)
{
    MVerifyValidInstance;

    PaletteRemoved(pmPalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaisePaletteActiveEntityChanged(MPalette* pmPalette,
    MEntity* pmOldActiveEntity)
{
    MVerifyValidInstance;

    PaletteActiveEntityChanged(pmPalette, pmOldActiveEntity);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseActivePaletteChanged(MPalette* pmActivePalette,
    MPalette* pmOldActivePalette)
{
    MVerifyValidInstance;

    ActivePaletteChanged(pmActivePalette, pmOldActivePalette);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetNameChanging(
    MSelectionSet* pmSelectionSet, String* strNewName, bool bInBatch)
{
    MVerifyValidInstance;

    SelectionSetNameChanging(pmSelectionSet, strNewName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseSelectionSetNameChanged(
    MSelectionSet* pmSelectionSet, String* strOldName, bool bInBatch)
{
    MVerifyValidInstance;

    SelectionSetNameChanged(pmSelectionSet, strOldName, bInBatch);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseViewportCameraChanged(MViewport* pmViewport,
    MEntity* pmCamera)
{
    MVerifyValidInstance;

    ViewportCameraChanged(pmViewport, pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseViewportRenderingModeChanged(MViewport* pmViewport,
    IRenderingMode* pmRenderingMode)
{
    MVerifyValidInstance;

    ViewportRenderingModeChanged(pmViewport, pmRenderingMode);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseActiveViewportChanged(MViewport* pmOldViewport,
    MViewport* pmNewViewport)
{
    MVerifyValidInstance;

    ActiveViewportChanged(pmOldViewport, pmNewViewport);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseExclusiveViewportChanged(MViewport* pmOldViewport,
    MViewport* pmNewViewport)
{
    MVerifyValidInstance;

    ExclusiveViewportChanged(pmOldViewport, pmNewViewport);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseCameraAddedToManager(MEntity* pmCamera)
{
    MVerifyValidInstance;

    CameraAddedToManager(pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseCameraRemovedFromManager(MEntity* pmCamera)
{
    MVerifyValidInstance;

    CameraRemovedFromManager(pmCamera);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityHiddenStateChanged(MEntity* pmEntity,
    bool bHidden)
{
    MVerifyValidInstance;

    EntityHiddenStateChanged(pmEntity, bHidden);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgPropertyStateChanged(MEntity* pmEntity, String *strProperty)
{
    MVerifyValidInstance;

	// MProxyManager 에 있는 OnEntityHiddenStateChanged 와 동일한
	// 기능을 하는 것을 만들어 주지 않아도 돌아감.
    EntityPgPropertyStateChanged(pmEntity, strProperty);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgUsePhysXChanged(MEntity* pmEntity, bool bUsePhysX)
{
    MVerifyValidInstance;
    EntityPgUsePhysXChanged(pmEntity, bUsePhysX);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgPostfixTextureChanged(MEntity* pmEntity, String *strPostfixTexture)
{
    MVerifyValidInstance;
    EntityPgPostfixTextureChanged(pmEntity, strPostfixTexture);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgAlphaGroupChanged(MEntity* pmEntity, String *strAlphaGroup)
{
    MVerifyValidInstance;
    EntityPgAlphaGroupChanged(pmEntity, strAlphaGroup);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgOptimizationChanged(MEntity* pmEntity, String *strOptimization)
{
    MVerifyValidInstance;
    EntityPgOptimizationChanged(pmEntity, strOptimization);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityPgRandomAniChanged(MEntity* pmEntity, bool bRandomAni)
{
    MVerifyValidInstance;
    EntityPgRandomAniChanged(pmEntity, bRandomAni);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseEntityFrozenStateChanged(MEntity* pmEntity,
    bool bFrozen)
{
    MVerifyValidInstance;

    EntityFrozenStateChanged(pmEntity, bFrozen);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseComponentServiceChanged(MComponent* pmComponent)
{
    MVerifyValidInstance;

    ComponentServiceChanged(pmComponent);
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLongOperationStarted()
{
    MVerifyValidInstance;

    LongOperationStarted();
}
//---------------------------------------------------------------------------
void MEventManager::RaiseLongOperationCompleted()
{
    MVerifyValidInstance;

    LongOperationCompleted();
}
//---------------------------------------------------------------------------
