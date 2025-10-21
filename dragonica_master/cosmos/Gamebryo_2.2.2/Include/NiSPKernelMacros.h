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

#ifndef NISPKERNELMACROS_H
#define NISPKERNELMACROS_H
//---------------------------------------------------------------------------
#include "NiSPKernelWin32.h"
//---------------------------------------------------------------------------
// Macro to declare an SP Kernel on Win32
//---------------------------------------------------------------------------
typedef NiSPKernelWin32<16,8> NiSPKernelBase;
#define NiSPDeclareKernel(KernelName) \
class KernelName : public NiSPKernelBase { \
public: \
    KernelName() : NiSPKernelBase(#KernelName){} \
    void SetWorkflow(NiSPWorkflow* pkWorkflow); \
    virtual void Execute(); \
}; 
#define NiSPDeclareKernelLib(KernelName, entrytag) \
class entrytag KernelName : public NiSPKernelBase { \
public: \
    KernelName() : NiSPKernelBase(#KernelName){} \
    void SetWorkflow(NiSPWorkflow* pkWorkflow); \
    virtual void Execute(); \
}; 

//---------------------------------------------------------------------------
// Macro to implement the execute method on Win32
//---------------------------------------------------------------------------
#define NiSPBeginKernelImpl(KernelName) void KernelName::Execute()

//---------------------------------------------------------------------------
// Macro to finish the implementation and launch the Execute method
//---------------------------------------------------------------------------
#define NiSPEndKernelImpl(KernelName)

//---------------------------------------------------------------------------
// Macro to delegate printf
//---------------------------------------------------------------------------
#define KernelDebugOutput NILOG
//---------------------------------------------------------------------------

#endif // NISPKERNELMACROS_H
