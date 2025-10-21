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
#include "StdPluginsCppPCH.h"

#include "MSelectionSetService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MSelectionSetService::MSelectionSetService()
{
}
//---------------------------------------------------------------------------
void MSelectionSetService::Do_Dispose(bool bDisposing)
{
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MSelectionSetService::get_Name()
{
    MVerifyValidInstance;

    return "Selection Set Service";
}
//---------------------------------------------------------------------------
bool MSelectionSetService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MSelectionSetService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// ISelectionSetService members.
//---------------------------------------------------------------------------
unsigned int MSelectionSetService::get_SelectionSetCount()
{
    MVerifyValidInstance;

    return MFramework::Instance->Scene->SelectionSetCount;
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSetService::GetSelectionSets()[]
{
    MVerifyValidInstance;

    return MFramework::Instance->Scene->GetSelectionSets();
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSetService::GetSelectionSetByName(String* strName)
{
    MVerifyValidInstance;

    return MFramework::Instance->Scene->GetSelectionSetByName(strName);
}
//---------------------------------------------------------------------------
bool MSelectionSetService::AddSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    return MFramework::Instance->Scene->AddSelectionSet(pmSelectionSet);
}
//---------------------------------------------------------------------------
void MSelectionSetService::RemoveSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MFramework::Instance->Scene->RemoveSelectionSet(pmSelectionSet);
}
//---------------------------------------------------------------------------
void MSelectionSetService::RemoveAllSelectionSets()
{
    MVerifyValidInstance;

    MFramework::Instance->Scene->RemoveAllSelectionSets();
}
//---------------------------------------------------------------------------
