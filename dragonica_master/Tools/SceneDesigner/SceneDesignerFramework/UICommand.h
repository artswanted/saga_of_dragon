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

#include "UIState.h"

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    /// <summary>
    /// Summary description for UICommand.
    /// </summary>
    public __gc class UICommand
    {
    public:
        UICommand();
        UICommand(String* strName);
        UICommand(String* strName, Object* pmData);

        __property String* get_Name();

        /// <summary>
        /// Allows a command to store arbitrary data that may be used
        /// by the handlers
        /// </summary>
        __property Object* get_Data();
        __property void set_Data(Object* pmData);

        __event EventHandler* Click;
        virtual void DoClick(Object* pmSender, EventArgs* pmEventArgs);

        // Event wrapper for Single case delegate
        __delegate void ValidateCommandHandler(Object* pmSender, 
            UIState* pmState);
        __event void add_Validate(ValidateCommandHandler* pmHandler);
        __event void remove_Validate(ValidateCommandHandler* pmHandler);
        virtual void ValidateCommand(UIState* pmState);

        /// <summary>
        /// This method will unhook all events for this 
        /// command object that have the passed in object as the
        /// event target
        /// </summary>
        /// <param name="obj">Any events that target this object will be 
        /// removed</param>
        void UnregisterAllByTarget(Object* pmObject);

        // These functions are for internal use only.
        Delegate* GetHandlerList()[];
        Delegate* GetValidator();

    protected:
        static Delegate* RemoveByTarget(Delegate* pmDelegate,
            Object* pmTarget);

    private:
        ValidateCommandHandler* m_pmValidateDelegate;
        String* m_strName;
        Object* m_pmData;
    };
}}}}
