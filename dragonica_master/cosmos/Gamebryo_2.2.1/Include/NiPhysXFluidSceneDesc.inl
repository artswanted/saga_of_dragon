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
inline unsigned int NiPhysXFluidSceneDesc::GetNumFluids() const
{
    return m_kFluids.GetSize();
}
//---------------------------------------------------------------------------
inline NiPhysXFluidDescPtr NiPhysXFluidSceneDesc::GetFluidDesc(
    unsigned int uiIndex)
{
    return m_kFluids.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiPhysXFluidSceneDesc::AddFluidDesc(NiPhysXFluidDesc* pkFluidDesc)
{
    m_kFluids.AddFirstEmpty(pkFluidDesc);
}
//---------------------------------------------------------------------------
inline NiPhysXFluidDescPtr NiPhysXFluidSceneDesc::RemoveFluidDesc(
    unsigned int uiIndex)
{
    return m_kFluids.RemoveAtAndFill(uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiPhysXFluidSceneDesc::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
