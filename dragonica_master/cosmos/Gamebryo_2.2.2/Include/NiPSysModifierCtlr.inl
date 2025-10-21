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
inline const NiFixedString& NiPSysModifierCtlr::GetModifierName() const
{
    return m_kModifierName;
}
//---------------------------------------------------------------------------
inline NiPSysModifier* NiPSysModifierCtlr::GetModifierPointer() const
{
    return m_pkModifier;
}
//---------------------------------------------------------------------------
inline void NiPSysModifierCtlr::GetModifierPointerFromName()
{
    assert(m_pkTarget && m_kModifierName.Exists());
    m_pkModifier = ((NiParticleSystem*) m_pkTarget)->GetModifierByName(
        m_kModifierName);
    assert(m_pkModifier);
}
//---------------------------------------------------------------------------
inline void NiPSysModifierCtlr::SetModifierName(const NiFixedString& kName)
{
    assert(kName.Exists());
    m_kModifierName = kName;
}
//---------------------------------------------------------------------------
