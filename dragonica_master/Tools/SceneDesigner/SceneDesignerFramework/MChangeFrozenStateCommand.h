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

#include "MDisposable.h"
#include "ICommand.h"
#include "MEntity.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MChangeFrozenStateCommand : public MDisposable,
        public ICommand
    {
    public:
        MChangeFrozenStateCommand(MEntity* pmEntity, bool bNewFrozenState);

    private:
        MEntity* m_pmEntity;
        bool m_bOldFrozenState;
        bool m_bNewFrozenState;

        bool m_bOldDataValid;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // ICommand members.
    public:
        __property String* get_Name();
        NiEntityCommandInterface* GetNiEntityCommandInterface();
        void DoCommand(bool bInBatch, bool bUndoable);
        void UndoCommand(bool bInBatch);
    };
}}}}
