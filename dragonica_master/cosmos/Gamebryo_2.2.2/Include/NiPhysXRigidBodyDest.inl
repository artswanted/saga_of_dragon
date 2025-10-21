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
inline NxActor* NiPhysXRigidBodyDest::GetActor() const
{
    return m_pkActor;
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodyDest::SetActor(NxActor* pkActor)
{
    m_pkActor = pkActor;
}
//---------------------------------------------------------------------------
inline NxActor* NiPhysXRigidBodyDest::GetActorParent() const
{
    return m_pkActorParent;
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodyDest::SetActorParent(NxActor* pkParent)
{
    m_pkActorParent = pkParent;
}
//---------------------------------------------------------------------------
inline bool NiPhysXRigidBodyDest::GetOptimizeSleep() const
{
    return m_bOptimizeSleep;
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodyDest::SetOptimizeSleep(const bool bOptimize)
{
    m_bOptimizeSleep = bOptimize;
}
//---------------------------------------------------------------------------
inline float NiPhysXRigidBodyDest::GetTime(unsigned int uiIndex) const
{
    return m_afTimes[m_aucIndices[uiIndex]];
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPhysXRigidBodyDest::GetTranslate(
    unsigned int uiIndex) const
{
    return m_akTranslates[m_aucIndices[uiIndex]];
}
//---------------------------------------------------------------------------
inline const NiQuaternion& NiPhysXRigidBodyDest::GetRotate(
    unsigned int uiIndex) const
{
    return m_akRotates[m_aucIndices[uiIndex]];
}
//---------------------------------------------------------------------------
inline bool NiPhysXRigidBodyDest::GetSleeping() const
{
    return m_bSleeping;
}
//---------------------------------------------------------------------------
inline void NiPhysXRigidBodyDest::Interpolate(const float fT,
    NiQuaternion& kRotation, NiPoint3& kTranslate)
{
    assert(fT >= m_afTimes[m_aucIndices[0]]
        && fT <= m_afTimes[m_aucIndices[1]]);
    assert(m_afTimes[m_aucIndices[1]] != m_afTimes[m_aucIndices[0]]);
    
    float fU = (fT - m_afTimes[m_aucIndices[0]])
        / (m_afTimes[m_aucIndices[1]] - m_afTimes[m_aucIndices[0]]);

    if ( NiQuaternion::Dot(m_akRotates[m_aucIndices[0]],
        m_akRotates[m_aucIndices[1]]) < 0.0f )
    {
        NiQuaternion kNegQuat = -m_akRotates[m_aucIndices[0]];
        kRotation = NiQuaternion::Slerp(fU, kNegQuat,
            m_akRotates[m_aucIndices[1]]);
    }
    else
    {
        kRotation = NiQuaternion::Slerp(fU, m_akRotates[m_aucIndices[0]],
            m_akRotates[m_aucIndices[1]]);
    }
    kTranslate = m_akTranslates[m_aucIndices[0]] + fU
        * (m_akTranslates[m_aucIndices[1]] - m_akTranslates[m_aucIndices[0]]);
}
//---------------------------------------------------------------------------
