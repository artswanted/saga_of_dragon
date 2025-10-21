#include "SceneDesignerFrameworkPCH.h"
#include "MFramework.h"
#include "IEntityPathService.h"

MPgFog::MPgFog() : m_pkFogColor(NULL), m_pkFogProperty(NULL),
m_fNearPlane(0.0f), m_fFarPlane(0.0f), m_fDensity(0.0f), m_bFogEnable(false)
{
	m_eFogType = NiFogProperty::FOG_Z_LINEAR;
	m_pkFogColor = NiNew NiColor(0.0f, 0.0f, 0.0f);
	m_pkFogProperty = NiNew NiFogProperty;
}
MPgFog::~MPgFog()
{
	if(NULL != m_pkFogColor)
	{
		NiDelete m_pkFogColor;
	}
	if(NULL != m_pkFogProperty)
	{
		NiDelete m_pkFogProperty;
	}
}

void MPgFog::SetFogColor(float const fR, float const fG, float const fB)
{
	m_pkFogColor->r = fR;
	m_pkFogColor->g = fG;
	m_pkFogColor->b = fB;
	SetFog();
}

NiColor MPgFog::GetFogColor()
{
	return *m_pkFogColor;
}

void MPgFog::SetFogPlane(float const fNear, float const fFar)
{
	m_fNearPlane = fNear;
	m_fFarPlane = fFar;
	SetFog();
}

float MPgFog::GetFogNearPlane() 
{
	return m_fNearPlane;
}

float MPgFog::GetFogFarPlane()
{
	return m_fFarPlane;
}

void MPgFog::SetFogDensity(float const fDensity)
{
	m_fDensity = fDensity;
	SetFog();
}

float MPgFog::GetFogDensity()
{
	return m_fDensity;
}

void MPgFog::SetFogEnable(bool const bEnable)
{
	m_bFogEnable = bEnable;
	SetFog();
}

bool MPgFog::IsFogEnable()
{
	return m_bFogEnable;
}

float MPgFog::GetBound()
{
	NiBound* pkBound = NiNew NiBound;
	MFramework::Instance->Scene->GetBound(pkBound);
	float fRadius = pkBound->GetRadius();
	NiDelete pkBound;
	return fRadius;
}

void MPgFog::SetFogType(FOG_FUNC kFogFunc)
{
	switch(kFogFunc)
	{
	case FF_ZLINEAR:
		{
			m_eFogType = NiFogProperty::FOG_Z_LINEAR;
		}
		break;
	case FF_RANGESQ:
		{
			m_eFogType = NiFogProperty::FOG_RANGE_SQ;
		}
		break;
	default:
		{
			m_eFogType = NiFogProperty::FOG_Z_LINEAR;
		}
		break;
	}

	SetFog();
}

MPgFog::FOG_FUNC MPgFog::GetFogType()
{
	switch(m_eFogType)
	{
	case NiFogProperty::FOG_Z_LINEAR:
		{
			return FF_ZLINEAR;
		}
		break;
	case NiFogProperty::FOG_RANGE_SQ:
		{
			return FF_RANGESQ;
		}
		break;
	default:
		{
			return FF_ZLINEAR;
		}
		break;
	}

	return FF_ZLINEAR;
}

void MPgFog::SetFog()
{
	if(true==m_bFogEnable)
	{
		m_pkFogProperty->SetFog(m_bFogEnable);
		m_pkFogProperty->SetFogColor(*m_pkFogColor);
		m_pkFogProperty->SetFogStart(m_fNearPlane);
		m_pkFogProperty->SetFogEnd(m_fFarPlane);
		//m_pkFogProperty->SetFogDensity(m_fDensity);
		m_pkFogProperty->SetDepth(m_fDensity);
		m_pkFogProperty->SetFogFunction(m_eFogType);
	}
	else
	{
		m_pkFogProperty->SetFog(m_bFogEnable);
	}
}

void MPgFog::UpdateFog()
{
	int const iEntityCount = MFramework::Instance->Scene->GetEntities()->Count;

	for(int i=0;i<iEntityCount;++i)
	{//for(i <- 0~iEntityCount : 각각의 엔티티에 대해)
		//pEntity <- i번째 엔티티
		MEntity* pkEntity = MFramework::Instance->Scene->GetEntities()[i];
		//pEntity.속성추가(포그속성)
		if(NULL!=pkEntity)
		{
			//for(int i=0; i<pkEntity->GetSceneRootPointerCount(); ++i)
			{
				NiAVObject* pkRoot = pkEntity->GetSceneRootPointer(0);
				if(NULL==pkRoot->GetProperty(m_pkFogProperty->GetType()))
				{
					pkRoot->AttachProperty(m_pkFogProperty);
				}
				pkRoot->UpdateProperties();
				//pkRoot->Update(0.0f);
			}
		}
	}
}
