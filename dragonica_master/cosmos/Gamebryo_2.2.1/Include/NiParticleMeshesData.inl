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
inline NiAVObject* NiParticleMeshesData::GetMeshAt(unsigned int uiIndex)
    const
{
    return m_spParticleMeshes->GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiNode* NiParticleMeshesData::GetContainerNode() const
{
    return m_spParticleMeshes;
}
//---------------------------------------------------------------------------
inline bool NiParticleMeshesData::GetNeedPropertyEffectUpdate() const
{
    return m_bNeedsPropertyEffectUpdate;
}
//---------------------------------------------------------------------------
inline void NiParticleMeshesData::SetNeedPropertyEffectUpdate(bool bUpdate)
{
    m_bNeedsPropertyEffectUpdate = bUpdate;
}
//---------------------------------------------------------------------------
