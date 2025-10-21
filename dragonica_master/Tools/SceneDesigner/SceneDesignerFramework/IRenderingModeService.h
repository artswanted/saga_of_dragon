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
#include "IRenderingMode.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    public __gc __interface IRenderingModeService : public IService
    {
        void AddRenderingMode(IRenderingMode* pmMode);
        IRenderingMode* GetRenderingModeByName(String* strName);
        IRenderingMode* GetRenderingModes()[];
    };
}}}}}
