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

#include "IService.h"
#include "Message.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    public __value enum MessageChannelType
    {
        Errors,
        General,
        Conflicts
    };

    public __delegate void MessageAddedHandler(MessageChannelType eChannel,
        int iIndex);
    public __delegate void ChannelMessagesClearedHandler(
        MessageChannelType eChannel);
    public __delegate void AllMessagesClearedHandler();

    public __gc __interface IMessageService : public IService
    {
        __event MessageAddedHandler* MessageAdded;
        __event ChannelMessagesClearedHandler* ChannelMessagesCleared;
        __event AllMessagesClearedHandler* AllMessagesCleared;

        String* GetChannelName(MessageChannelType eChannel);

        void AddMessage(MessageChannelType eChannel, String* strMessage);
        void AddMessage(MessageChannelType eChannel, Message* pmMessage);
        Message* GetMessage(MessageChannelType eChannel, int iIndex);
        Message* GetLatestMessage(MessageChannelType eChannel);
        Message* GetMessages(MessageChannelType eChannel)[];

        void ClearMessages(MessageChannelType eChannel);
        void ClearAllMessages();
    };
}}}}}
