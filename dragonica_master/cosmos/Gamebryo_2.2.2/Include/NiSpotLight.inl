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
//  NiSpotLight inline functions

//---------------------------------------------------------------------------
inline const NiPoint3& NiSpotLight::GetWorldDirection() const
{
    return m_kWorldDir;
}
//---------------------------------------------------------------------------
inline void NiSpotLight::SetSpotAngle(float fSpotAngle)
{
    m_fOuterSpotAngle = fSpotAngle;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiSpotLight::GetSpotAngle() const
{
    return m_fOuterSpotAngle;
}
//---------------------------------------------------------------------------
inline void NiSpotLight::SetInnerSpotAngle(float fSpotAngle)
{
    m_fInnerSpotAngle = fSpotAngle;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiSpotLight::GetInnerSpotAngle() const
{
    return m_fInnerSpotAngle;
}
//---------------------------------------------------------------------------
inline void NiSpotLight::SetSpotExponent(float fSpotExponent)
{
    m_fSpotExponent = fSpotExponent;
    IncRevisionID();
}
//---------------------------------------------------------------------------
inline float NiSpotLight::GetSpotExponent() const
{
    return m_fSpotExponent;
}
//---------------------------------------------------------------------------
