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

#pragma once

using namespace System::Collections;

#include "MDisposable.h"
#include "MComponent.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MComponentFactory : public MDisposable
    {
    public:
        MComponent* Get(NiEntityComponentInterface* pkUnmanaged);
        void Remove(NiEntityComponentInterface* pkUnmanaged);
        void Remove(MComponent* pmManaged);
        void Clear();

    private:
        Hashtable* m_pmUnmanagedToManaged;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MComponentFactory* get_Instance();
    private:
        static MComponentFactory* ms_pmThis = NULL;
        MComponentFactory();
    };
}}}}
