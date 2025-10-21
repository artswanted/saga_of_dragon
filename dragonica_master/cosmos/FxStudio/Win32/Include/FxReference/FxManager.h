/*****************************************************************

    MODULE    : FxManager.h

    PURPOSE   : A manager for creating FxObjects.

    CREATED   : 10/15/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXMANAGER_H_
#define FXMANAGER_H_

#include "FxStudioReferenceLibType.h"

#include "FxStudioRT.h"

#include <NiCamera.h>
#include <NiEntity.h>

#include "FxAssetManager.h"
#include "FxSoundManager.h"

#include <NiScreenElements.H>

class FXSTUDIOREFERENCE_ENTRY FxManager
{

public :

	 void InitializeStatics();
	 void TerminateStatics();
	 void SetCamera(NiCamera* pkCamera);
	 void SetFxManager(FxStudio::Manager* pkFxManager);

	// In order for rumble to work, these values must be set each frame.
	 void GetRumbleValues(float& fPrimaryMotor, float& fSecondaryMotor);

	// The ScreenEffect uses this to render its screen-elements.

	 void SetScreenElementsRenderArray(NiScreenElementsArray* pkScreenElementsRenderArray);
	 NiScreenElementsArray* GetScreenElementsRenderArray();

	// The asset manager is used to load the dependent assets.
	 void SetAssetManager(FxAssetManager* pkAssetManager);
	 FxAssetManager* GetAssetManager();

	// The sound component uses this to play its sounds.
	 void SetSoundManager(FxSoundManager* pkSoundManager);
	 FxSoundManager* GetSoundManager();

private:

	NiScreenElementsArray		*m_spScreenElementsRenderArray;	
	FxAssetManagerPtr			m_spAssetManager;
	FxSoundManagerPtr			m_spSoundManager;

};


#endif // FXMANAGER_H_