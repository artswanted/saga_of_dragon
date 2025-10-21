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
inline NxSceneDesc& NiPhysXSceneDesc::GetNxSceneDesc()
{
    return m_kNxSceneDesc;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetGroupCollisionFlag(
    const unsigned char ucGroup1, const unsigned char ucGroup2, 
    const bool bVal)
{
    m_abGroupCollisionFlags[ucGroup1][ucGroup2] = bVal;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::GetGroupCollisionFlag(
    const unsigned char ucGroup1, const unsigned char ucGroup2) const
{
    return m_abGroupCollisionFlags[ucGroup1][ucGroup2];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterOps(NxFilterOp bOp0, NxFilterOp bOp1,
    NxFilterOp bOp2)
{
    m_aeFilterOps[0] = bOp0;
    m_aeFilterOps[1] = bOp1;
    m_aeFilterOps[2] = bOp2;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterOps(NxFilterOp& bOp0, NxFilterOp& bOp1,
    NxFilterOp& bOp2) const
{
    bOp0 = m_aeFilterOps[0];
    bOp1 = m_aeFilterOps[1];
    bOp2 = m_aeFilterOps[2];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterBool(bool bFlag)
{
    m_bFilterBool = bFlag;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::GetFilterBool() const
{
    return m_bFilterBool;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterConstant0(const NxGroupsMask &kMask)
{
    m_auiFilterConstants[0] = kMask;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterConstant0(NxGroupsMask &kMask) const
{
    kMask = m_auiFilterConstants[0];
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetFilterConstant1(const NxGroupsMask &kMask)
{
    m_auiFilterConstants[1] = kMask;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetFilterConstant1(NxGroupsMask &kMask) const
{
    kMask = m_auiFilterConstants[1];
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXSceneDesc::GetActorCount() const
{
    return m_kActors.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXSceneDesc::GetActorAt(unsigned int uiIndex)
{
    return m_kActors.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXSceneDesc::AddActor(NiPhysXActorDesc* pkActorDesc)
{
    return m_kActors.AddFirstEmpty(pkActorDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXSceneDesc::RemoveActorAt(
    unsigned int uiIndex)
{
    return m_kActors.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXSceneDesc::GetJointCount() const
{
    return m_kJoints.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXJointDescPtr NiPhysXSceneDesc::GetJointAt(unsigned int uiIndex)
{
    return m_kJoints.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXSceneDesc::AddJoint(NiPhysXJointDesc* pkJointDesc)
{
    return m_kJoints.AddFirstEmpty(pkJointDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXJointDescPtr NiPhysXSceneDesc::RemoveJointAt(
    unsigned int uiIndex)
{
    return m_kJoints.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline NiPhysXMaterialDescPtr NiPhysXSceneDesc::GetMaterial(
    const NxMaterialIndex usIndex)
{
    NiPhysXMaterialDescPtr pkMaterial;
    
    if (m_kMaterials.GetAt(usIndex, pkMaterial))
        return pkMaterial;

    return 0;
}
//---------------------------------------------------------------------------
inline NiTMapIterator NiPhysXSceneDesc::GetFirstMaterial()
{
    return m_kMaterials.GetFirstPos();
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::GetNextMaterial(NiTMapIterator& kPos,
    NxMaterialIndex& usIndex, NiPhysXMaterialDescPtr& spMaterial)
{
    m_kMaterials.GetNext(kPos, usIndex, spMaterial);
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::RemoveMaterial(const NxMaterialIndex usIndex)
{
    return m_kMaterials.RemoveAt(usIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiPhysXSceneDesc::GetNumStates() const
{
    return m_uiNumStates;
}
//---------------------------------------------------------------------------
inline void NiPhysXSceneDesc::SetNumStates(const unsigned int uiNumStates)
{
    m_uiNumStates = uiNumStates;
}
//---------------------------------------------------------------------------
inline bool NiPhysXSceneDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
