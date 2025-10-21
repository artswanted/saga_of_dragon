#include "SceneDesignerFrameworkPCH.h"
#include "MPgBrightBloom.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPgBrightBloom::MPgBrightBloom() : 
    m_fBlurWidth(1.0f), m_fBrightness(1.0f), m_fSceneIntensity(1.0f),
    m_fGlowIntensity(1.0f), m_fHighLightIntensity(1.0f)
{
}
//---------------------------------------------------------------------------
MPgBrightBloom::~MPgBrightBloom(void)
{
}

void MPgBrightBloom::set_IsUpdated(bool bIsUpdated)
{
	m_bIsUpdated = bIsUpdated;
}
bool MPgBrightBloom::get_IsUpdated()
{
	return m_bIsUpdated;
}
void MPgBrightBloom::set_IsActive(bool bIsActive)
{
	m_bIsActive = bIsActive;
}
bool MPgBrightBloom::get_IsActive()
{
	return m_bIsActive;
}
__inline float MPgBrightBloom::GetBlur() 
{
	return m_fBlurWidth;
}
__inline float MPgBrightBloom::GetBrightness() 
{
	return m_fBrightness;
}
__inline float MPgBrightBloom::GetSceneIntensity() 
{
	return m_fSceneIntensity;
}
__inline float MPgBrightBloom::GetGlowIntensity() 
{
	return m_fGlowIntensity;
}
__inline float MPgBrightBloom::GetHighLightIntensity() 
{
	return m_fHighLightIntensity;
}
void MPgBrightBloom::set_Blur(float fBlur)
{
	m_fBlurWidth = fBlur;
}
float MPgBrightBloom::get_Blur()
{
	return m_fBlurWidth;
}

void MPgBrightBloom::set_Brightness(float fBrightness)
{
	m_fBrightness = fBrightness;
}
float MPgBrightBloom::get_Brightness()
{
	return m_fBrightness;
}

void MPgBrightBloom::set_SceneIntensity(float fIntensity)
{
	m_fSceneIntensity = fIntensity;
}
float MPgBrightBloom::get_SceneIntensity()
{
	return m_fSceneIntensity;
}

void MPgBrightBloom::set_GlowIntensity(float fIntensity)
{
	m_fGlowIntensity = fIntensity;
}
float MPgBrightBloom::get_GlowIntensity()
{
	return m_fGlowIntensity;
}

void MPgBrightBloom::set_HighLightIntensity(float fIntensity)
{
	m_fHighLightIntensity = fIntensity;
}
float MPgBrightBloom::get_HighLightIntensity()
{
	return m_fHighLightIntensity;
}

//---------------------------------------------------------------------------
float MPgBrightBloom::get_CurrentTime()
{
    return 1.0f;
}
//---------------------------------------------------------------------------
void MPgBrightBloom::set_CurrentTime(float fTime)
{
}