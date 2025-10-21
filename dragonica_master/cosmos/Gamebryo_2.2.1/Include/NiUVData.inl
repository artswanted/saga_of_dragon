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
inline unsigned int NiUVData::GetNumUOffsetKeys() const
{
    return m_uiNumUOffsetKeys;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVData::GetNumVOffsetKeys() const
{
    return m_uiNumVOffsetKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVData::GetUOffsetAnim(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumUOffsetKeys;
    eType = m_eUOffsetType;
    ucSize = m_ucUOffsetSize;
    return m_pkUOffsetKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVData::GetVOffsetAnim(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumVOffsetKeys;
    eType = m_eVOffsetType;
    ucSize = m_ucVOffsetSize;
    return m_pkVOffsetKeys;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVData::GetNumUTilingKeys() const
{
    return m_uiNumUTilingKeys;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVData::GetNumVTilingKeys() const
{
    return m_uiNumVTilingKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVData::GetUTilingAnim(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumUTilingKeys;
    eType = m_eUTilingType;
    ucSize = m_ucUTilingSize;
    return m_pkUTilingKeys;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVData::GetVTilingAnim(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    uiNumKeys = m_uiNumVTilingKeys;
    eType = m_eVTilingType;
    ucSize = m_ucVTilingSize;
    return m_pkVTilingKeys;
}
//---------------------------------------------------------------------------
inline float NiUVData::GetLastUOffset() const
{
    return m_fLastUOffset;
}
//---------------------------------------------------------------------------
inline float NiUVData::GetLastVOffset() const
{
    return m_fLastVOffset;
}
//---------------------------------------------------------------------------
inline float NiUVData::GetLastUTiling() const
{
    return m_fLastUTiling;
}
//---------------------------------------------------------------------------
inline float NiUVData::GetLastVTiling() const
{
    return m_fLastVTiling;
}
//---------------------------------------------------------------------------
inline void NiUVData::SetLastUVOffsetTiling(float fUOffset, float fVOffset, 
    float fUTiling, float fVTiling)
{
    m_fLastUOffset = fUOffset;
    m_fLastVOffset = fVOffset;
    m_fLastUTiling = fUTiling;
    m_fLastVTiling = fVTiling;
}
//---------------------------------------------------------------------------
