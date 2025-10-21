#pragma once
#include "NiFogProperty.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgFog
	{
	public:
		__value typedef enum eFOG_FUNC
		{
			FF_ZLINEAR=0,
			FF_RANGESQ,
		}FOG_FUNC;

		MPgFog();
		~MPgFog();

		void SetFogColor(float const fR, float const fG, float const fB);
		NiColor GetFogColor();

		void SetFogPlane(float const fNear, float const fFar);
		float GetFogNearPlane();
		float GetFogFarPlane();
		
		void SetFogDensity(float const fDensity);
		float GetFogDensity();

		void SetFogEnable(bool const bEnable);
		bool IsFogEnable();

		void SetFogType(FOG_FUNC kFogFunc);
		FOG_FUNC GetFogType();

		float GetBound();

		void UpdateFog();

	private:
		NiColor*		m_pkFogColor;
		float			m_fNearPlane, m_fFarPlane;
		float			m_fDensity;
		bool			m_bFogEnable;
		NiFogProperty*	m_pkFogProperty;
		NiFogProperty::FogFunction m_eFogType;

		//Set*()함수내부의 마지막에 호출한다.
		void SetFog();
	};
}}}}