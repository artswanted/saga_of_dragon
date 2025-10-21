 // Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MChangePGOptimizationCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MChangePGOptimizationCommand::MChangePGOptimizationCommand(
    NiChangePGOptimizationCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MChangePGOptimizationCommand::Do_Dispose(bool bDisposing)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MChangePGOptimizationCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MChangePGOptimizationCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MChangePGOptimizationCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityPgOptimizationChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetPGOptimization());
}
//---------------------------------------------------------------------------
void MChangePGOptimizationCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityPgOptimizationChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetPGOptimization());
}
//---------------------------------------------------------------------------
