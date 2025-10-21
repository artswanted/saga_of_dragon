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
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
inline void NiSPKernelWin32<T_INS, T_OUTS>::SetTask(NiSPTask* pkTask)
{
    m_pkTask = pkTask;
    m_uiBlockCount = 0;

    // Iterate through inputs streams looking for 
    // first non fixed stream to determine block count
    NiUInt32 uiInputStreamCount = pkTask->GetInputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pStream = pkTask->GetInputAt(uiIndex);
        if (pStream->IsPartitioningEnabled())
        {
            m_uiBlockCount = pStream->GetBlockCount();
            break;
        }
    }

    // If block count couldn't be determined from input streams
    // then look through the output streams
    if (m_uiBlockCount == 0)
    {
        NiUInt32 uiOutputStreamCount = pkTask->GetOutputCount();
        for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
        {
            NiSPStream* pStream = pkTask->GetOutputAt(uiIndex);
            if (pStream->IsPartitioningEnabled())
            {
                m_uiBlockCount = pStream->GetBlockCount();
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
template <class T>
T* NiSPKernelWin32<T_INS, T_OUTS>::GetInput(NiUInt32 uiInputIndex)
{
    return reinterpret_cast<T*>(
        m_pkTask->GetInputAt(uiInputIndex)->GetData());
}
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
template <class T>
T* NiSPKernelWin32<T_INS, T_OUTS>::GetOutput(NiUInt32 uiOutputIndex)
{
    return reinterpret_cast<T*>(
        m_pkTask->GetOutputAt(uiOutputIndex)->GetData());
}
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
inline NiUInt32 NiSPKernelWin32<T_INS, T_OUTS>::GetBlockCount()
{
    return m_uiBlockCount;
}
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
inline NiUInt32 NiSPKernelWin32<T_INS, T_OUTS>::GetInputCount()
{
    return m_pkTask->GetInputCount();
}
//---------------------------------------------------------------------------
template <NiUInt32 T_INS, NiUInt32 T_OUTS>
inline NiUInt32 NiSPKernelWin32<T_INS, T_OUTS>::GetOutputCount()
{
    return m_pkTask->GetOutputCount();
}
