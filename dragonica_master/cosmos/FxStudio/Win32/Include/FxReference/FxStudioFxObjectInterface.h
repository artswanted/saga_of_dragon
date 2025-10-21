/*****************************************************************

    MODULE    : FxStudioFxObjectInterface.h

    PURPOSE   : Implements an Emergent component which wraps an FxStudio Fx for game code.

    CREATED   : 9/24/2008 3:46:22 PM

    COPYRIGHT : (C) 2008 Aristen, Inc. 

*****************************************************************/

#ifndef FXSTUDIOFXOBJECTINTERFACE_H
#define FXSTUDIOFXOBJECTINTERFACE_H


#include "FxStudioFxObjectInterfaceBase.h"
#include "FxStudioRT.h"
#include "FxStudioReferenceLibType.h"

#include <NiNode.h>
#include <NiCamera.h>
#include <NiSmartPointer.h>

class FxObject;
class FxManager;
class FXSTUDIOREFERENCE_ENTRY FxStudioFxObjectInterface : public FxStudioFxObjectInterfaceBase
{

public:

	FxStudioFxObjectInterface();
	virtual ~FxStudioFxObjectInterface() {}

	static void SetCamera(NiCamera* pkCamera);
	static void SetFxStudioManager(FxStudio::Manager* pkFxManager);
	static void	SetFxManager(FxManager *pkFxManager)	{	ms_pkFxManager = pkFxManager;	}

	virtual NiObject* GetRootScenePointer() const { return m_spNode; }

	const FxObject*	GetFxObject() const { return m_pkFxObject; }
	FxObject*		GetFxObject()		{ return m_pkFxObject; }

public:
    // NiEntityComponentInterface overrides.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);

    virtual void Update(NiEntityPropertyInterface* pkParentEntity, float fTime, NiEntityErrorInterface* pkErrors, NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext, NiEntityErrorInterface* pkErrors);

protected:

	// Construction from a master component should only be done internally.
    FxStudioFxObjectInterface(FxStudioFxObjectInterfaceBase* pkMasterComponent);

	static NiCameraPtr			ms_spCamera;
	static FxStudio::Manager*	ms_pkFxStudioManager;
	static FxManager*	ms_pkFxManager;

	bool					m_bSpawnedFx;
	NiNodePtr				m_spNode;

	FxObject* 				m_pkFxObject;
	float					m_fRestartTime;

};

NiSmartPointer(FxStudioFxObjectInterface);

#endif  // #ifndef FXSTUDIOFXOBJECTINTERFACE_H
