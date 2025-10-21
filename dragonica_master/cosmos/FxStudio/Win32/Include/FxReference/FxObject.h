/*****************************************************************

    MODULE    : FxObject.h

    PURPOSE   : Reference implementation for a FxStudio Fx object.

    CREATED   : 5/22/2008 

    COPYRIGHT : (C) 2008 Aristen, Inc.

*****************************************************************/

#ifndef FXOBJECT_H_
#define FXOBJECT_H_

#include "FxStudioReferenceLibType.h"

#include "FxStudioRT.h"

#include <NiNode.h>
#include <NiCamera.h>

class	FxManager;

class FXSTUDIOREFERENCE_ENTRY FxObject : public NiNode, public FxStudio::FxListener
{
    NiDeclareRTTI;

public :

	FxObject(FxManager &kFxManager,FxStudio::Manager& rManager, const char* szFxName, NiCamera* kpCamera);
	FxObject(FxManager &kFxManager,FxStudio::Manager& rManager, const char* szBankName, const char* szFxName, NiCamera* kpCamera);
	FxObject(FxManager &kFxManager,FxStudio::Manager& rManager, const unsigned char* pByteStream, const char* szFxName, NiCamera* kpCamera);
	FxObject(FxManager &kFxManager,FxStudio::Manager& rManager, const char* szBankName, unsigned int uiFxId, NiCamera* kpCamera);

	virtual ~FxObject();

	FxStudio::FxInstance&       GetFxInstance()       { return m_FxInstance; }
	const FxStudio::FxInstance& GetFxInstance() const { return m_FxInstance; }

	// We need to receive updates in order to keep our LOD distance correct.
	virtual void UpdateDownwardPass(float fTime, bool bUpdateControllers);
	virtual void UpdateSelectedDownwardPass(float fTime);
	virtual void UpdateRigidDownwardPass(float fTime);
	
    // Cloning Methods....    
    virtual NiObject* CreateClone(NiCloningProcess& kCloning);
    virtual void ProcessClone(NiCloningProcess &kCloningProcess);
    void CopyMembers(FxObject *pDest, NiCloningProcess &kCloning);

	// For FxListener
	virtual void OnFxDestroyed();
	
	// Accessors....
	
	void	SetCamera(NiCamera *pkCamera)	{	m_spCamera = pkCamera;	}
	NiCameraPtr GetCamera() { return m_spCamera; }
	

	bool	GetDebugDraw() const			{ return m_bDebugDraw; }
	void	SetDebugDraw(bool bDebugDraw)	{ m_bDebugDraw = bDebugDraw; }

	void	SetDestroyed()	{	m_bDestroyed	=	true;	}
	bool	GetDestroyed()	const	{	return	m_bDestroyed;	}

	void	SetDetachFromParentWhenDestroyed(bool bDetach)	{	m_bDetachFromParentWhenDestroyed = bDetach;	}
	bool	GetDetachFromParentWhenDestroyed()	const	{	return	m_bDetachFromParentWhenDestroyed;	}


	FxManager*	GetFxManager()	const	{	return	m_pkFxManager;	}

private :

	void SetupInputs();

	void UpdateLOD();
	void UpdateInputs();

	FxStudio::FxInstance m_FxInstance;
	NiCameraPtr			 m_spCamera;
	char				 m_CameraDistString[16];
	bool				 m_bDebugDraw;

	const char**	m_pCameraDistanceString;
	float*			m_pCameraDistance;
	NiPoint3*		m_pViewDirection;
	NiPoint3*		m_pInverseViewDirection;

	bool	m_bDestroyed;
	bool	m_bDetachFromParentWhenDestroyed;

	FxManager*	m_pkFxManager;
};

typedef NiPointer<FxObject> FxObjectPtr;

#endif // FXOBJECT_H_