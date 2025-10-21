/*****************************************************************

    MODULE    : FxPreview.h

    PURPOSE   : Reference implementation for a FxStudio preview system.

    CREATED   : 8/18/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXPREVIEW_H_
#define FXPREVIEW_H_

#include "FxStudioReferenceLibType.h"

#include "FxStudioRT.h"

#include "FxObject.h"

#include <NiNode.h>
#include <NiCamera.h>

class FXSTUDIOREFERENCE_ENTRY FxPreview : public FxStudio::Preview
{
public :

	FxPreview()
		: m_bWasConnected(false)
		, m_fAccumTime(0.0f) 
		, m_bDebugDraw(false)
		, m_bHasPreviewTransform(false)
		// m_kPreviewTransform
		// m_spFxObject
		// m_spScene
		// m_spCamera
		, m_pWindowHandle(NULL),
	m_pkFxManager(NULL)
	{}

	void Initialize(NiNode* pScene, NiCamera* pCamera,FxManager *pkFxManager);

	virtual FxStudio::FxInstance SpawnFx(FxStudio::Manager& manager, const unsigned char* pByteStream, const char* szFxName);

	void Update(float fDeltaTime, float fAccumTime);

	void SetWindowHandle(const void* pWindowHandle);
	virtual const void* GetWindowHandle() const;

	void SetDebugDraw(bool bDebugDraw);
	bool GetDebugDraw() const { return m_bDebugDraw; }


	void SetPreviewTransform(const NiTransform& kPreviewTransform);
	void ClearPreviewTransform() { m_bHasPreviewTransform = false; }

	void Terminate();

private :

	bool		m_bWasConnected;
	float		m_fAccumTime;
	
	bool		m_bDebugDraw;
	
	bool		m_bHasPreviewTransform;
	NiTransform m_kPreviewTransform;

	FxObjectPtr m_spFxObject;
    NiNodePtr	m_spScene;
    NiCameraPtr m_spCamera;
	const void* m_pWindowHandle;

	FxManager*	m_pkFxManager;
};


#endif // FXPREVIEW_H_