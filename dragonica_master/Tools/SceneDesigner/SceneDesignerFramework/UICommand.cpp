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

#include "UICommand.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

//---------------------------------------------------------------------------
UICommand::UICommand()
{
}
//---------------------------------------------------------------------------
UICommand::UICommand(String* strName) : m_strName(strName)
{
}
//---------------------------------------------------------------------------
UICommand::UICommand(String* strName, Object* pmData) : m_strName(strName),
    m_pmData(pmData)
{
}
//---------------------------------------------------------------------------
String* UICommand::get_Name()
{
    return m_strName;
}
//---------------------------------------------------------------------------
Object* UICommand::get_Data()
{
    return m_pmData;
}
//---------------------------------------------------------------------------
void UICommand::set_Data(Object* pmData)
{
    m_pmData = pmData;
}
//---------------------------------------------------------------------------
void UICommand::DoClick(Object* pmSender, EventArgs* pmEventArgs)
{
    if (Click != NULL)
    {
        Click(this, pmEventArgs);
    }
}
//---------------------------------------------------------------------------
void UICommand::add_Validate(ValidateCommandHandler* pmHandler)
{
    if (m_pmValidateDelegate == NULL)
    {
        m_pmValidateDelegate = pmHandler;
    }
    else
    {
        throw new InvalidOperationException(
            "Command Validator has already been assigned");
    }
}
//---------------------------------------------------------------------------
void UICommand::remove_Validate(ValidateCommandHandler* pmHandler)
{
    m_pmValidateDelegate -= pmHandler;
}
//---------------------------------------------------------------------------
void UICommand::ValidateCommand(UIState* pmState)
{
    if (m_pmValidateDelegate != NULL)
    {
        m_pmValidateDelegate(this, pmState);
    }
    else
    {
        // The default enabled state of command depends on the 
        // command being bound.
        pmState->Enabled = (Click != NULL);
    }
}
//---------------------------------------------------------------------------
void UICommand::UnregisterAllByTarget(Object* pmObject)
{
    Click = static_cast<EventHandler*>(RemoveByTarget(Click,
        pmObject));
    m_pmValidateDelegate = static_cast<ValidateCommandHandler*>(
        RemoveByTarget(m_pmValidateDelegate, pmObject));
}
//---------------------------------------------------------------------------
Delegate* UICommand::RemoveByTarget(Delegate* pmDelegate, Object* pmTarget)
{
    Delegate* pmReturnValue = pmDelegate;
    if (pmDelegate != NULL)
    {
        Delegate* amHandlers[] = pmDelegate->GetInvocationList();
        ArrayList* pmRemovalList = new ArrayList();
        for (int i = 0; i < amHandlers->Length; i++)
        {
            Delegate* pmHandler = amHandlers[i];
            if (pmHandler->Target == pmTarget)
            {
                pmRemovalList->Add(pmHandler);
            }
        }
        for (int i = 0; i < pmRemovalList->Count; i++)
        {
            Delegate* pmHandler = dynamic_cast<Delegate*>(
                pmRemovalList->Item[i]);
            pmReturnValue = Delegate::Remove(pmReturnValue,
                pmHandler);
        }
    }

    return pmReturnValue;
}
//---------------------------------------------------------------------------
Delegate* UICommand::GetHandlerList()[]
{
    Delegate* clickDelegate = Click;
    Delegate* pmInvokationList[] = NULL;
    
    if (clickDelegate != NULL)
    {
        
        pmInvokationList = clickDelegate->GetInvocationList();
    }
    return pmInvokationList;
}
//---------------------------------------------------------------------------
Delegate* UICommand::GetValidator()
{
    //Delegate* pmInvokationList[] = NULL;
    //if (m_pmValidateDelegate != NULL)
    //{
    //    pmInvokationList = m_pmValidateDelegate->GetInvocationList();
    //}
    //if (pmInvokationList != NULL && pmInvokationList->GetLength(0) > 0)
    //{
    //    return dynamic_cast<Delegate*>(pmInvokationList->get_Item(0));
    //}
    return m_pmValidateDelegate;
}
//---------------------------------------------------------------------------

