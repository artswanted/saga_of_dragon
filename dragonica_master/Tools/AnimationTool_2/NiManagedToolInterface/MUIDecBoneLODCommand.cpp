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

#include "stdafx.h"
#include "MUIDecBoneLODCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIDecBoneLODCommand::MUIDecBoneLODCommand()
{
}
//---------------------------------------------------------------------------
String* MUIDecBoneLODCommand::GetName()
{
    return "Decrement Bone LOD";
}
//---------------------------------------------------------------------------
bool MUIDecBoneLODCommand::Execute(MUIState* pkState)
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    if (!pkActorManager)
    {
        pkSharedData->Unlock();
        return false;
    }

    NiBoneLODController* pkBoneLOD = pkActorManager->GetBoneLODController();
    if (pkBoneLOD)
    {
        int iBoneLOD = pkBoneLOD->GetBoneLOD();
        if (iBoneLOD - 1 >= 0)
            pkBoneLOD->SetBoneLOD(--iBoneLOD);
    }

    pkSharedData->Unlock();    
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIDecBoneLODCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIDecBoneLODCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIDecBoneLODCommand::GetEnabled()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiActorManager* pkActorManager = pkSharedData->GetActorManager();
    if (!pkActorManager)
    {
        pkSharedData->Unlock();
        return false;
    }

    NiBoneLODController* pkBoneLOD = pkActorManager->GetBoneLODController();
    if (pkBoneLOD)
    {
        int iBoneLOD = pkBoneLOD->GetBoneLOD();
        if (iBoneLOD - 1 < 0)
        {
            pkSharedData->Unlock();
            return false;
        }
    }
    else
    {
        pkSharedData->Unlock();
        return false;
    }

    
    pkSharedData->Unlock();
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIDecBoneLODCommand::GetCommandType()
{
    return MUICommand::BONELOD_DECREMENT;
}
//---------------------------------------------------------------------------
