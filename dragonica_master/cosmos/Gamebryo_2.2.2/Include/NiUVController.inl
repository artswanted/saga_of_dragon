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
inline NiUVData* NiUVController::GetUVData()
{
    return m_spUVData;
}
//---------------------------------------------------------------------------
inline void NiUVController::SetUVData(NiUVData* pData)
{
    m_spUVData = pData;
    m_uiLastUOffsetIdx = 0;
    m_uiLastVOffsetIdx = 0;
    m_uiLastUTilingIdx = 0;
    m_uiLastVTilingIdx = 0;

    ResetKeyTimeExtrema();
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVController::GetUOffsetData(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spUVData)
        return m_spUVData->GetUOffsetAnim(uiNumKeys,eType,ucSize);
    
    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;;
    ucSize = 0;
    return 0;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVController::GetVOffsetData(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spUVData)
        return m_spUVData->GetVOffsetAnim(uiNumKeys,eType,ucSize);
    
    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;
    ucSize = 0;
    return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVController::GetNumUOffsetKeys() const
{
    return m_spUVData ? m_spUVData->GetNumUOffsetKeys() : 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVController::GetNumVOffsetKeys() const
{
    return m_spUVData ? m_spUVData->GetNumVOffsetKeys() : 0;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVController::GetUTilingData(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spUVData)
        return m_spUVData->GetUTilingAnim(uiNumKeys,eType,ucSize);

    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;;
    ucSize = 0;
    return 0;
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiUVController::GetVTilingData(unsigned int& uiNumKeys,
    NiAnimationKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spUVData)
        return m_spUVData->GetVTilingAnim(uiNumKeys,eType,ucSize);

    uiNumKeys = 0;
    eType = NiAnimationKey::NOINTERP;;
    ucSize = 0;
    return 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVController::GetNumUTilingKeys() const
{
    return m_spUVData ? m_spUVData->GetNumUTilingKeys() : 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiUVController::GetNumVTilingKeys() const
{
    return m_spUVData ? m_spUVData->GetNumVTilingKeys() : 0;
}
//---------------------------------------------------------------------------
inline unsigned short NiUVController::GetTextureSet() const
{
    return m_usTextureSet;
}
//---------------------------------------------------------------------------
inline void NiUVController::SetTextureSet(unsigned short usTextureSet)
{
    m_usTextureSet = usTextureSet;
}
//---------------------------------------------------------------------------
inline void NiUVController::SetTarget(NiObjectNET* pkTarget)
{
    m_spUVData->SetLastUVOffsetTiling(0.0f, 0.0f, 1.0f, 1.0f);

    NiTimeController::SetTarget(pkTarget);
}
//---------------------------------------------------------------------------
inline bool NiUVController::IsVertexController() const
{
    return true;
}
//---------------------------------------------------------------------------
inline bool NiUVController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiGeometry, m_pkTarget);
}
//---------------------------------------------------------------------------
