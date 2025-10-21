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

#include "MTransition.h"

namespace NiManagedToolInterface
{
    public __gc class MChainCompletionInfo
    {
    public:
        __property float get_SeqStart();
        __property float get_TransStart();
        __property float get_TransEnd();
        __property float get_InSeqBeginFrame();
        __property String* get_Name();
        __property String* get_NextName();
        __property MTransition::TransitionType get_TransitionType();

        MChainCompletionInfo(NiActorManager::ChainCompletionInfo*
            pkChainCompletionInfo);

    private:
        void SetChainCompletionInfo(NiActorManager::ChainCompletionInfo*
            pkChainCompletionInfo);

        NiActorManager::ChainCompletionInfo* m_pkChainCompletionInfo;
    };

    public __gc class MCompletionInfo
    {
    public:
        __property float get_FrameInDestWhenTransitionCompletes();
        __property float get_TimeToCompleteTransition();
        __property float get_FrameTransitionOccursInSrc();
        __property float get_TimeForChainToComplete();
        __property MTransition::MBlendPair* get_BlendPair();
        __property MChainCompletionInfo* get_ChainCompletionInfo()[];

        MCompletionInfo(NiActorManager::CompletionInfo* pkCompletionInfo);

    private:
        void SetCompletionInfo(
            NiActorManager::CompletionInfo* pkCompletionInfo);
        void BuildChainCompletionInfoArray();

        NiActorManager::CompletionInfo* m_pkCompletionInfo;
        MChainCompletionInfo* m_aChainCompletionInfo[];
    };
}
