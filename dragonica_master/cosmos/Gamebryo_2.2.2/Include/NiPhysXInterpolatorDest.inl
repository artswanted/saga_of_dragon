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
inline NiTransformInterpolator* NiPhysXInterpolatorDest::GetTarget() const
{
    return m_pkTarget;
}
//---------------------------------------------------------------------------
inline NiAVObject* NiPhysXInterpolatorDest::GetSceneGraphParent() const
{
    return m_pkSceneParent;
}
//---------------------------------------------------------------------------
inline void NiPhysXInterpolatorDest::SetTarget(
    NiTransformInterpolator* pkTarget, NiAVObject* pkParent)
{
    m_pkTarget = pkTarget;
    m_pkSceneParent = pkParent;
}
//---------------------------------------------------------------------------
inline bool NiPhysXInterpolatorDest::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
