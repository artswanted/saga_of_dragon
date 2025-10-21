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

#include "MFilenameEditor.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MNifFilenameEditor : public MFilenameEditor
    {
    protected:
        virtual String* GetDialogTitle();
        virtual String* GetDialogFilter();
    };
}}}}
