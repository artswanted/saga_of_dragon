#include "$(ProjectName)PCH.h"
#include "SceneDesignerFrameworkPCH.h"
#include "MPgBrightBloom.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MPgBrightBloom::MPgBrightBloom() : m_bIsChecked(false),
    m_fBlurWidth(1.0f), m_fBrightness(1.0f), m_fSceneIntensity(1.0f),
    m_fGlowIntensity(1.0f), m_fHighLightIntensity(1.0f)
{
}
//---------------------------------------------------------------------------
MPgBrightBloom::~MPgBrightBloom(void)
{
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
bool MPgBrightBloom::get_Check()
{
	return m_bIsChecked;
}
void MPgBrightBloom::set_Check(bool bCheck)
{
	m_bIsChecked = bCheck;
}

void MPgBrightBloom::set_Blur(float fBlur)
{
	m_fBlurWidth = fBlur;
}

void MPgBrightBloom::set_Brightness(float fBrightness)
{
	m_fBrightness = fBrightness;
}

void MPgBrightBloom::set_SceneIntensity(float fIntensity)
{
	m_fSceneIntensity = fIntensity;
}

void MPgBrightBloom::set_GlowIntensity(float fIntensity)
{
	m_fGlowIntensity = fIntensity;
}

void MPgBrightBloom::set_HighLightIntensity(float fIntensity)
{
	m_fHighLightIntensity = fIntensity;
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