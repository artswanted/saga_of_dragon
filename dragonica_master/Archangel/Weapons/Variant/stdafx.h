// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef WEAPON_VARIANT_BASIC_STDAFX_H
#define WEAPON_VARIANT_BASIC_STDAFX_H

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <windows.h>
#include <tchar.h>

#include "BM/ObjectPool.h"
#include "BM/Common.h"
#include "CEL/Common.h"
#include "Lohengrin/Common.h"



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
/*
extern "C"
{
	#include "lua/lua.h"
}
*/
#include <stack>

#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")
#include "tinyxml/tinyxml.h"

//#ifndef BATTLE_FORMULA_081120_ICE
#define BATTLE_FORMULA_081120_ICE
//#define _MEMORY_TRACKING
#include "Lohengrin/MemoryTrack.h"
//#endif

#include <limits>
#pragma warning(error : 4715) 

#endif // WEAPON_VARIANT_BASIC_STDAFX_H