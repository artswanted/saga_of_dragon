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
inline NiSPTask::Status NiSPTask::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetStatus(NiSPTask::Status eStatus)
{
    m_eStatus = eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPTask::AddInput(NiSPStream* pkInputStream)
{
    assert(m_kInputStreams.GetSize() < MAX_INPUTS && "Max inputs exceeded");
    m_kInputStreams.Add(pkInputStream);
    pkInputStream->AddInput(this);
}
//---------------------------------------------------------------------------
inline void NiSPTask::AddOutput(NiSPStream* pkOutputStream)
{
    assert(m_kOutputStreams.GetSize() < MAX_OUTPUTS && "Max outputs exceeded");
    m_kOutputStreams.Add(pkOutputStream);
    pkOutputStream->AddOutput(this);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetKernel(NiSPKernel* pkKernel)
{
    m_pkKernel = pkKernel;
}
//---------------------------------------------------------------------------
inline NiSPKernel* NiSPTask::GetKernel() const
{
    return m_pkKernel;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetInputCount() const
{
    return m_kInputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetOutputCount() const
{
    return m_kOutputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetTotalCount() const
{
    return m_kInputStreams.GetSize() + m_kOutputStreams.GetSize();
}
//---------------------------------------------------------------------------
inline NiSPStream* NiSPTask::GetInputAt(NiUInt32 uiIndex) const
{
    assert(uiIndex < m_kInputStreams.GetSize());
    return m_kInputStreams.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline NiSPStream* NiSPTask::GetOutputAt(NiUInt32 uiIndex) const
{
    assert(uiIndex < m_kOutputStreams.GetSize());
    return m_kOutputStreams.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetSyncData(NiUInt64 uiSyncData)
{
    m_uiSyncData = uiSyncData;
}
//---------------------------------------------------------------------------
inline NiUInt64 NiSPTask::GetSyncData() const
{
    return m_uiSyncData;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsMarked() const
{
    return m_bIsMarked;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsMarked(bool bIsMarked)
{
    m_bIsMarked = bIsMarked;
}
//---------------------------------------------------------------------------
inline unsigned short NiSPTask::GetStage() const
{
    return m_usStage;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetStage(unsigned short usStage)
{
    m_usStage = usStage;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsRoot() const
{
    return m_bIsRoot;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsLeaf() const
{
    return m_bIsLeaf;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsSync() const
{
    return m_bIsSync;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsAligned() const
{
    return m_bIsAligned;
}
//---------------------------------------------------------------------------
inline bool NiSPTask::IsCacheable() const
{
    return m_bIsCacheable;
}
//---------------------------------------------------------------------------
inline void NiSPTask::SetIsCacheable(bool bIsCacheable)
{
    m_bIsCacheable = bIsCacheable;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPTask::GetId() const
{
    return m_uiTaskId;
}
//---------------------------------------------------------------------------
inline NiSPTaskImpl& NiSPTask::GetImpl()
{
    return m_kImpl;
}
//---------------------------------------------------------------------------
inline bool operator<(const NiSPTask& pkLhs, const NiSPTask& pkRhs)
{
    return pkLhs.GetStage() < pkRhs.GetStage();
}
//---------------------------------------------------------------------------
inline void NiSPTask::Reset()
{
    m_uiSliceSize = 0;
    m_eStatus = IDLE;
    m_bIsRoot = false;
    m_bIsLeaf = false;
    m_bIsAligned = false;
    m_kImpl.Clear();
}
//---------------------------------------------------------------------------
inline void NiSPTask::Clear(bool bIgnoreCachine)
{
    Reset();

    // If the task can not be reused then clear streams
    if (!m_bIsCacheable || bIgnoreCachine)
    {
        m_usStage = 0;
        m_bIsMarked = false;
        m_kInputStreams.RemoveAll();
        m_kOutputStreams.RemoveAll();
    }
}
//---------------------------------------------------------------------------
