#pragma once

#include "MDisposable.h"
#include "ICommand.h"
#include "MPropertyContainer.h"
#include "NiChangePGOptimizationCommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MChangePGOptimizationCommand : public MDisposable,
        public ICommand
    {
    public:
        MChangePGOptimizationCommand(NiChangePGOptimizationCommand* pkCommand);

    private:
        NiChangePGOptimizationCommand* m_pkCommand;

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
