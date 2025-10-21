// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net
//---------------------------------------------------------------------------
//  NiFogProperty inline functions

//---------------------------------------------------------------------------
inline NiFogProperty::NiFogProperty()
{
    m_uFlags = 0;
    SetFog(false);
    m_fDepth = 1.0f;
	m_fFogDensity = 0.05f;
	m_fFogStart = 10;
	m_fFogEnd = 10000;
	m_fFogAlpha = 1.0f;
    SetFogFunction(FOG_Z_LINEAR); 
    m_kColor = NiColor::BLACK;
}
//---------------------------------------------------------------------------
inline void NiFogProperty::SetFog(bool bFog)
{
    SetBit(bFog, FOG_MASK);
}
//---------------------------------------------------------------------------
inline bool NiFogProperty::GetFog() const
{
    return GetBit(FOG_MASK);
}
//---------------------------------------------------------------------------
inline void NiFogProperty::SetFogFunction(FogFunction eFunc)
{
    SetField(eFunc, FOG_FUNC_MASK, FOG_FUNC_POS);
}
//---------------------------------------------------------------------------
inline NiFogProperty::FogFunction NiFogProperty::GetFogFunction() const
{
    return (FogFunction)GetField(FOG_FUNC_MASK, FOG_FUNC_POS);
}
//---------------------------------------------------------------------------
inline void NiFogProperty::SetFogColor(const NiColor& kColor)
{
    m_kColor = kColor;
}
//---------------------------------------------------------------------------
inline const NiColor& NiFogProperty::GetFogColor() const
{
    return m_kColor;
}
//---------------------------------------------------------------------------
inline void NiFogProperty::SetDepth(float fDepth)
{
    m_fDepth = (fDepth >= 0.0f ? fDepth : 0.0f);
}
//---------------------------------------------------------------------------
inline float NiFogProperty::GetDepth() const
{
    return m_fDepth;
}

inline void NiFogProperty::SetFogStart(float fFogStart)
{
	m_fFogStart = (fFogStart >= 0.0f ? fFogStart : 0.0f);
}
//---------------------------------------------------------------------------
inline float NiFogProperty::GetFogStart() const
{
	return m_fFogStart;
}

inline void NiFogProperty::SetFogEnd(float fFogEnd)
{
	m_fFogEnd = (fFogEnd >= 0.0f ? fFogEnd : 0.0f);
}
//---------------------------------------------------------------------------
inline float NiFogProperty::GetFogEnd() const
{
	return m_fFogEnd;
}

inline void NiFogProperty::SetFogDensity(float fFogDensity)
{
	m_fFogDensity = (fFogDensity >= 0.0f ? fFogDensity : 0.0f);
}
//---------------------------------------------------------------------------
inline float NiFogProperty::GetFogDensity() const
{
	return m_fFogDensity;
}

//---------------------------------------------------------------------------
inline bool NiFogProperty::IsEqualFast(const NiFogProperty& kProp) const
{
    return &kProp == this || (!kProp.GetFog() && !GetBit(FOG_MASK));
}
//---------------------------------------------------------------------------
inline int NiFogProperty::Type() const
{
    return NiProperty::FOG;
}
//---------------------------------------------------------------------------
inline int NiFogProperty::GetType()
{ 
    return NiProperty::FOG;
}
//---------------------------------------------------------------------------
inline NiFogProperty* NiFogProperty::GetDefault()
{
    return ms_spDefault;
}
//---------------------------------------------------------------------------
