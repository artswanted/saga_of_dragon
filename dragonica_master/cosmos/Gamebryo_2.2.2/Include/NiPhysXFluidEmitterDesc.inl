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
inline NxFluidEmitterDesc NiPhysXFluidEmitterDesc::GetNxFluidEmitterDesc()
    const
{
    return m_kDesc;
}
//---------------------------------------------------------------------------
inline NiFixedString NiPhysXFluidEmitterDesc::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidEmitterDesc::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
inline NxFluidEmitter* NiPhysXFluidEmitterDesc::GetEmitter() const
{
    return m_pkEmitter;
}
//---------------------------------------------------------------------------
inline NiPhysXActorDescPtr NiPhysXFluidEmitterDesc::GetFrameActor() const
{
    return m_spFrameActor;
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidEmitterDesc::SetFrameActor(
    NiPhysXActorDesc* pkActorDesc)
{
    m_spFrameActor = pkActorDesc;
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidEmitterDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
