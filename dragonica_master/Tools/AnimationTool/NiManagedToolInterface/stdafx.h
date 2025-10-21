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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once
// Disable warning for generating native code, since AnimationTool is
// a Win32-specific application.
#pragma warning( disable : 4793 )

#pragma comment(lib, "mss32.lib")

// Include Gamebryo header files.
#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>
#include <NiCollision.h>
#include <NiPortal.h>
#include <NiParticle.h>
#include <NiDX9Renderer.h>
#include <NiAudio.h>
#include <NiMilesAudio.h>

using namespace System;

// managed
#include <vcclr.h>

// STL
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>


#include "NiVirtualBoolBugWrapper.h"
#include "ManagedMacros.h"
#include "MLogger.h"
#include "MUtility.h"
#include "PgUtility.h"

// Loki
#include "Loki/Singleton.h"

#if defined(NIEXTERNALRELEASE)
	#define RELATIVE_PATH "../../SFreedom_Dev/"
#elif defined(NIDEBUG) || defined(NIRELEASE)
	#define RELATIVE_PATH "../../../../../../Dragonica_Exe/SFreedom_Dev/"
#endif