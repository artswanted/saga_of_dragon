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
inline NiPhysXScenePtr NiPhysXParticleSystem::GetScene() const
{
    return ((NiPhysXPSysData*)GetModelData())->GetScene();
}
//---------------------------------------------------------------------------
inline bool NiPhysXParticleSystem::GetPhysXSpace() const
{
    return m_bPhysXSpace;
}
//---------------------------------------------------------------------------
inline void NiPhysXParticleSystem::SetPhysXSpace(const bool bPhysXSpace)
{
    m_bPhysXSpace = bPhysXSpace;
}
//---------------------------------------------------------------------------
