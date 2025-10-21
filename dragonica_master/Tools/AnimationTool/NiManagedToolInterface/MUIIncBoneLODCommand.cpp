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
#include "MUIIncBoneLODCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIIncBoneLODCommand::MUIIncBoneLODCommand()
{
}
//---------------------------------------------------------------------------
String* MUIIncBoneLODCommand::GetName()
{
    return "Increment Bone LOD";
}
//---------------------------------------------------------------------------
bool MUIIncBoneLODCommand::Execute(MUIState* pkState)
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
        if (iBoneLOD + 1 < (int) pkBoneLOD->GetNumberOfBoneLODs())
            pkBoneLOD->SetBoneLOD(++iBoneLOD);
    }

    pkSharedData->Unlock();
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIIncBoneLODCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIIncBoneLODCommand::RefreshData()
{
    DeleteContents();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIIncBoneLODCommand::GetEnabled()
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
        if (iBoneLOD == -1 || 
           (iBoneLOD + 1 == (int) pkBoneLOD->GetNumberOfBoneLODs()))
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
MUICommand::CommandType MUIIncBoneLODCommand::GetCommandType()
{
    return MUICommand::BONELOD_INCREMENT;
}
//---------------------------------------------------------------------------
