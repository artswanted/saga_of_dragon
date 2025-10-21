#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include <NiPhysx.h>
#include "MPgDrawPhysX.h"

void MPgDrawPhysX::Render(MRenderingContext* pmRenderingContext)
{
}

void MPgDrawPhysX::CreatePhysX()
{
}

void MPgDrawPhysX::Do_Dispose(bool bDisposing)
{
}

void MPgDrawPhysX::Init()
{/*
	ms_pmThis = new MPgDrawPhysX();

	ms_pkPhysXManager = NiPhysXManager::GetPhysXManager();
	if(!ms_pkPhysXManager->Initialize())
	{
		::MessageBox(NULL, "PhysX manager initialize failed", "Error", MB_OK);
		return;
	}

	ms_pkPhysXManager->m_pkPhysXSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	ms_pkPhysXManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
	ms_pkPhysXManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 15.0f);
	ms_pkPhysXManager->m_pkPhysXSDK->setParameter(NX_VISUALIZE_BODY_MASS_AXES, 1.0f);	*/
}

void MPgDrawPhysX::Shutdown()
{
	if(NULL != ms_pmThis)
	{
		delete ms_pmThis;
	}
}

bool MPgDrawPhysX::InstanceIsValid()
{
	return (NULL != ms_pmThis);
}

MPgDrawPhysX* MPgDrawPhysX::get_Instance()
{
	return ms_pmThis;
}