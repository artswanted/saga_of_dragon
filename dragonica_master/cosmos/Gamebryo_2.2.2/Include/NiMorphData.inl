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
inline NiPoint3* NiMorphData::MorphTarget::GetTargetVerts() const
{
    return m_aTargetVerts;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiMorphData::MorphTarget::GetTargetVert(unsigned int uiIndex)
    const
{
    // uiIndex must be less than NiMorphData::m_uiNumVertsPerTarget
    return m_aTargetVerts[uiIndex];
}
//---------------------------------------------------------------------------
inline unsigned int NiMorphData::GetNumTargets() const
{
    return m_uiNumTargets;
}
//---------------------------------------------------------------------------
inline unsigned int NiMorphData::GetNumVertsPerTarget() const
{
    return m_uiNumVertsPerTarget;
}
//---------------------------------------------------------------------------
inline NiMorphData::MorphTarget* NiMorphData::GetTargets() const
{
    return m_aMorphTargets;
}
//---------------------------------------------------------------------------
inline NiMorphData::MorphTarget* NiMorphData::GetTarget(unsigned int uiIndex)
    const
{
    if (uiIndex < m_uiNumTargets)
        return &(m_aMorphTargets[uiIndex]);
    else
        return 0;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiMorphData::GetTargetVerts(unsigned int uiIndex) const
{
    return m_aMorphTargets[uiIndex].GetTargetVerts();
}
//---------------------------------------------------------------------------
inline bool NiMorphData::GetRelativeTargets() const
{
    return m_bRelativeTargets;
}
//---------------------------------------------------------------------------
inline void NiMorphData::SetRelativeTargets(bool bRelativeTargets)
{
    m_bRelativeTargets = bRelativeTargets;
}
//---------------------------------------------------------------------------
inline float NiMorphData::MorphTarget::GetLegacyWeight() const
{
    return m_fLegacyWeight;
}
//---------------------------------------------------------------------------
