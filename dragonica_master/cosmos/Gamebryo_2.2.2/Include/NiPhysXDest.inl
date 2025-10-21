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
inline bool NiPhysXDest::GetActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline void NiPhysXDest::SetActive(const bool bActive)
{
    m_bActive = bActive;
}
//---------------------------------------------------------------------------
inline bool NiPhysXDest::GetInterpolate() const
{
    return m_bInterp;
}
//---------------------------------------------------------------------------
inline void NiPhysXDest::SetInterpolate(const bool bActive)
{
    m_bInterp = bActive;
}
//---------------------------------------------------------------------------
