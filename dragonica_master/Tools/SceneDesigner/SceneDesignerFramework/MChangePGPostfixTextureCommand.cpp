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

 // Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MChangePGPostfixTextureCommand.h"
#include "MEventManager.h"
#include "MEntityFactory.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
MChangePGPostfixTextureCommand::MChangePGPostfixTextureCommand(
    NiChangePGPostfixTextureCommand* pkCommand) : m_pkCommand(pkCommand)
{
    MInitRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
void MChangePGPostfixTextureCommand::Do_Dispose(bool bDisposing)
{
    MDisposeRefObject(m_pkCommand);
}
//---------------------------------------------------------------------------
// ICommand members.
//---------------------------------------------------------------------------
String* MChangePGPostfixTextureCommand::get_Name()
{
    MVerifyValidInstance;

    return m_pkCommand->GetName();
}
//---------------------------------------------------------------------------
NiEntityCommandInterface*
    MChangePGPostfixTextureCommand::GetNiEntityCommandInterface()
{
    MVerifyValidInstance;

    return m_pkCommand;
}
//---------------------------------------------------------------------------
void MChangePGPostfixTextureCommand::DoCommand(bool bInBatch, bool bUndoable)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->DoCommand(spErrors, bUndoable);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityPgPostfixTextureChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetPGPostfixTexture());
}
//---------------------------------------------------------------------------
void MChangePGPostfixTextureCommand::UndoCommand(bool bInBatch)
{
    MVerifyValidInstance;

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();

    // Execute command.
    m_pkCommand->UndoCommand(spErrors);

    // Report errors.
    MUtility::AddErrorInterfaceMessages(MessageChannelType::Errors, spErrors);

    // Raise event.
    MEventManager::Instance->RaiseEntityPgPostfixTextureChanged(
        MEntityFactory::Instance->Get(m_pkCommand->GetEntity()),
        m_pkCommand->GetPGPostfixTexture());
}
//---------------------------------------------------------------------------
