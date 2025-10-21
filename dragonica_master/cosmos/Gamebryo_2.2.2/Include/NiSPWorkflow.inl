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
inline NiSPTask* NiSPWorkflow::AddNewTask()
{
    NiSPTask* pkTask = ms_pkTaskPool->GetFreeObject();
    Add(pkTask);
    return pkTask;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiSPWorkflow::GetId() const
{
    return m_uiId;
}
//---------------------------------------------------------------------------
inline NiSPWorkflow::Status NiSPWorkflow::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::SetStatus(NiSPWorkflow::Status eStatus)
{
    m_eStatus = eStatus;
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::Execute()
{
    assert(m_pkWorkflowImpl && "m_pkWorkflowImpl was NULL!");
    m_pkWorkflowImpl->Execute(this);
}
//---------------------------------------------------------------------------
inline void NiSPWorkflow::ExecutionComplete()
{
    assert(m_pkWorkflowImpl && "m_pkWorkflowImpl was NULL!");
    m_pkWorkflowImpl->Terminate();
}
