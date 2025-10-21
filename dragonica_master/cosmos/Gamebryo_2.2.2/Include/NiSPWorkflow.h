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
#ifndef NISPWORKFLOW_H
#define NISPWORKFLOW_H
//---------------------------------------------------------------------------
#include <NiSystem.h>
#include <NiTPtrSet.h>
#include <NiTPool.h>

#include "NiSPKernel.h"
#include "NiSPTask.h"
#include "NiSPWorkflowImpl.h"
//---------------------------------------------------------------------------
// NiSPWorkflow provides a wrapper around an acyclic directed graph 
// of NiSPTasks. Each NiSPTask represents a node of this graph while
// each NiSPStream that is bound to 2 or more NiSPTasks represents
// the connections of the graph
class NiSPTask;
class NIFLOODGATE_ENTRY NiSPWorkflow : 
    public NiTPrimitivePtrSet<NiSPTask*>
{
    friend class NiStreamProcessor;
    friend class NiSPWorkflowImpl;
public:

    // Workflow status
    enum Status
    {
        IDLE = 0,
        PENDING,
        RUNNING,
        COMPLETED,
        ABORTED
    };

    // Initializes object pools
    static void InitializePools(NiUInt32 uiWorkflowPoolSize, 
        NiUInt32 uiTaskPoolSize);

    // Shuts down object pools
    static void ShutdownPools();

    // Constructor / Descructor
    NiSPWorkflow();
    virtual ~NiSPWorkflow();

    // Gets the id for a workflow.
    inline NiUInt32 GetId() const;

    // Gets the status of a workflow
    inline Status GetStatus() const;

    // Creates a new task and adds it to the workflow
    inline NiSPTask* AddNewTask();

    // Clears the internal state of the workflow
    void Clear(bool bIgnoreCaching = false);
   
    // Reset
    void Reset();

protected:
    
    // Sets the status of a workflow
    inline void SetStatus(Status eStatus);

    // Prepares the workflow for scheduling and execution
    // Returns true is scheduling was successful, false if
    // scheduling should be retried again later
    bool Prepare();
    
    // Runs the workflow
    inline void Execute();

    // Performs platform specific completion of a workflow
    inline void ExecutionComplete();

    // Class Members
    static NiTObjectPool<NiSPTask>*        ms_pkTaskPool; // Task pool
    static NiTObjectPool<NiSPWorkflowImpl>*ms_pkImplPool; // Impl Pool
   
    // Members
    NiSPWorkflowImpl*  m_pkWorkflowImpl;   // Private workflow implementation
    NiUInt32            m_uiId;             // Id of workflow instance
    Status              m_eStatus;          // Status of workflow
};

//---------------------------------------------------------------------------
#include "NiSPWorkflow.inl"
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
