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
#include "MFramework.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
void MLogger::LogDebug(System::String* pMessage)
{
    LogDebug(pMessage,true);
}
//---------------------------------------------------------------------------
void MLogger::LogGeneral(System::String* pMessage)
{
    LogGeneral(pMessage,true);
}
//---------------------------------------------------------------------------
void MLogger::LogWarning(System::String* pMessage)
{
    LogWarning(pMessage,true);
}
//---------------------------------------------------------------------------
void MLogger::LogEvent(System::String* pMessage)
{
    LogEvent(pMessage,true);
}
//---------------------------------------------------------------------------
void MLogger::LogDebug(System::String* pMessage, bool bUpdateScroll)
{
#ifdef _DEBUG
    MMessageChannel* pkChannel = MFramework::Instance->Output->Channel[0];
    if (pkChannel)
    {
        pkChannel->Add(pMessage, bUpdateScroll);
    }
#endif
}
//---------------------------------------------------------------------------
void MLogger::LogGeneral(System::String* pMessage, bool bUpdateScroll)
{
    MMessageChannel* pkChannel = MFramework::Instance->Output->Channel[1];
    if (pkChannel)
    {
        pkChannel->Add(pMessage, bUpdateScroll);
    }
}
//---------------------------------------------------------------------------
void MLogger::LogWarning(System::String* pMessage, bool bUpdateScroll)
{
    MMessageChannel* pkChannel = MFramework::Instance->Output->Channel[2];
    if (pkChannel)
    {
        pkChannel->Add(pMessage, bUpdateScroll);
    }
}
//---------------------------------------------------------------------------
void MLogger::LogEvent(System::String* pMessage, bool bUpdateScroll)
{
#ifdef _DEBUG
    MMessageChannel* pkChannel = MFramework::Instance->Output->Channel[3];
    if (pkChannel)
    {
        pkChannel->Add(pMessage, bUpdateScroll);
    }
#endif
}
//---------------------------------------------------------------------------
void MLogger::Update()
{
    raise_OnUpdateScrollEvent();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
void MLogger::add_OnUpdateScrollEvent(UpdateScrollEvent* pkEvent)
{
    m_pkUpdateScrollDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MLogger::remove_OnUpdateScrollEvent(UpdateScrollEvent* pkEvent)
{
    m_pkUpdateScrollDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MLogger::raise_OnUpdateScrollEvent( )
{
    if (m_pkUpdateScrollDelegate)
    {
        m_pkUpdateScrollDelegate();
    }
}
//---------------------------------------------------------------------------

