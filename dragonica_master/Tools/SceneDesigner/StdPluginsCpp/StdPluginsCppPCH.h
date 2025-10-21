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
 
#pragma unmanaged

#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>
#include <NiParticle.h>
#include <NiCollision.h>
#include <NiPortal.h>
#include <NiDX9Renderer.h>
#include <NiEntity.h>
#include <NiViewMath.h>

#pragma managed

#include "ManagedMacros.h"

#if (_MSC_VER == 1310) // VC71
    // VC71 has a bug that requires this to be a #using rather than a
    // project reference in order to build successfully from the command
    // line.  However, in VC80, the project reference is required to
    // create the real dependency between SceneDesignerFramework
    // and StdPluginsCpp to ensure the correct build order.  (Project
    // dependencies are not enough.)
    #using <SceneDesignerFramework.dll>
#endif

using namespace System;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
