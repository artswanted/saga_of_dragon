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

#include "UICommandHandlerAttribute.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
UICommandHandlerAttribute::UICommandHandlerAttribute(String* strName) :
    m_strName(strName)
{
}
//---------------------------------------------------------------------------
String* UICommandHandlerAttribute::get_Name()
{
    return m_strName;
}
//---------------------------------------------------------------------------
void UICommandHandlerAttribute::set_Name(String* strName)
{
    m_strName = strName;
}
//---------------------------------------------------------------------------
