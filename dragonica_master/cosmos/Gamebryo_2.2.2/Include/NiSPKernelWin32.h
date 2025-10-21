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

#ifndef NISPKERNLWIN23_H
#define NISPKERNLWIN23_H

#include <NiSystem.h>
#include <NiSPKernel.h>
#include <NiSPTask.h>

template <NiUInt32 T_INS, NiUInt32 T_OUTS> 
class NiSPKernelWin32 : public NiSPKernel
{
public:

    NiSPKernelWin32(const char* pcName) : NiSPKernel(pcName){}
    virtual ~NiSPKernelWin32(){}

    // Sets the task
    void SetTask(NiSPTask* pkTask);

    // Gets an input stream by index
    template <class T> T* GetInput(NiUInt32 uiInputStreamIndex);

    // Gets an output stream by index
    template <class T> T* GetOutput(NiUInt32 uiOutputStreamIndex);

    // Gets the number of chunks to process
    inline NiUInt32 GetBlockCount();

    // Gets the number of inputs
    inline NiUInt32 GetInputCount();

    // Gets the number of output streams
    inline NiUInt32 GetOutputCount();

    // Executes the job
    virtual void Execute() = 0;

protected:

    NiSPTask*  m_pkTask;       // Task that contains streams to be processed
    NiUInt32    m_uiBlockCount; // Number of blocks to process
};


#include "NiSPKernelWin32.inl"
#endif

