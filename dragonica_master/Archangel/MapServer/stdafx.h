//!  stdafx.h : 자주 사용하지만 자주 변경되지는 않는
//!  표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
//!  들어 있는 포함 파일입니다.
//! 

#ifndef MAP_MAPSERVER_FRAMEWORK_STDAFX_H
#define MAP_MAPSERVER_FRAMEWORK_STDAFX_H

// 무시하는 경고 (이 외의 경고는 모두 오류로 처리한다!)
#pragma warning(disable : 4819)		// 코드 페이지 워닝
#pragma warning(disable : 4267)		// usngined int --> size_t 워닝
#pragma warning(disable : 4099) 

#define WIN32_LEAN_AND_MEAN		//!  거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#define NOMINMAX				// PhysX의 min/max을 사용합니다.

#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <ostream>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <map>
#include <stack>

#include "Loki/Threads.h"
#include "RandomLib/common.h"

#include "BM/bm.h"
#include "BM/Guid.h"
#include "BM/Objectpool.h"
#include "BM/ExceptionFilter.h"
#include "BM/vstring.h"
#include "BM/FileSupport.h"
#include "BM/LocalMgr.h"
#include "BM/Stream.h"

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "CEL/ConsoleCommander.h"

#include "Lohengrin/Lohengrin.h"
#include "FCS/FCS.h"
#include "Collins/Collins.h"
#include "Variant/Variant.h"

/*
extern "C" 
{
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}
#include "LuaDev/LuaDebuggerLIB.h"
#include "lua_tinker/common.h"
*/
#include <lua_tinker/lua_tinker.h>
#include <lua_tinker/lua_wrapper.h>
#include "tinyxml/tinyxml.h"


//// Gamebryo
//
#define _DX9
#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>
#include <NiParticle.h>
#include <NiEntity.h>

#pragma comment(lib, "NiSystem.lib")
#pragma comment(lib, "NiMain.lib")
#pragma comment(lib, "NiAnimation.lib")
#pragma comment(lib, "NiParticle.lib")
#pragma comment(lib, "NiEntity.lib")

//// PhysX
//
#pragma comment(lib, "NiPhysX.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma comment(lib, "NxCharacter.lib")
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NiCollision.lib")

#include <ControllerManager.h> 
#include <NxCapsuleController.h> 
#include <NxCooking.h>
#include <NiPhysX.h>

//// globals
//
//#define _MEMORY_TRACKING
#include "Lohengrin/MemoryTrack.h"
#include "Variant/DefAbilType.h"
#include "Variant/Unit.h"
#include "Variant/PgTotalObjectMgr.h"
#include "BM/LocalMgr.h"
#include "PgSendWrapper.h"
#include "zlib/zlib.h"

//// Lua Scripting
//
#include <Onibal/Onibal.h>
#include <Onibal/lwOnibal.h>

//#define AI_DEBUG
#define ZONE_OPTIMIZE_20090624

#define DEF_ESTIMATE_TICK_DELAY	// Ground의 100ms의 Elapsed Time Average를 계산할 것인가?
#define DEF_RETURN_TICK_IF_NOPLAYER	// Player 가 없다면 100ms Tick을 그냥 return 할 것인가?

extern bool GetDefString(int const iTextNo, const wchar_t *&pString);
extern bool GetDefString(int const iTextNo, std::wstring &kOutString );

extern bool lwIsRandSuccess(int iValue);
extern bool lwIsRandSuccess(int const iValue, DWORD const dwRate);

#endif // MAP_MAPSERVER_FRAMEWORK_STDAFX_H