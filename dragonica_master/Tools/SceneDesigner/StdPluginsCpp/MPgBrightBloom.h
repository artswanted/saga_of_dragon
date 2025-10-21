#pragma once

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgBrightBloom
	{
	private:
		bool m_bIsChecked;
		float	m_fBlurWidth;
		float	m_fBrightness;
		float	m_fSceneIntensity;
		float	m_fGlowIntensity;
		float	m_fHighLightIntensity;

	public:
		MPgBrightBloom(void);
		~MPgBrightBloom(void);

	public:
		__inline float GetBlur() ;
		__inline float GetBrightness() ;
		__inline float GetSceneIntensity() ;
		__inline float GetGlowIntensity() ;
		__inline float GetHighLightIntensity() ;

	public:
		__property bool get_Check();
		__property void set_Check(bool bCheck);
		__property void set_Blur(float fBlur);
		__property void set_Brightness(float fBrightness);
		__property void set_SceneIntensity(float fIntensity);
		__property void set_GlowIntensity(float fIntensity);
		__property void set_HighLightIntensity(float fIntensity);

        __property float get_CurrentTime();
        __property void set_CurrentTime(float fTime);

	};
}}}}
