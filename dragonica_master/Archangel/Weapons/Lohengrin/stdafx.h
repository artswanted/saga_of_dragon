// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#ifndef WEAPON_LOHENGRIN_MAINFRAME_STDAFX_H
#define WEAPON_LOHENGRIN_MAINFRAME_STDAFX_H

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#include <windows.h>
#include <tchar.h>

//#include "BM/BM.h"
#include "BM/Common.h"
//#include "CEL/CEL.h"
#include "CEL/Common.h"

//#define _MEMORY_TRACKING
#include "MemoryTrack.h"

#pragma comment(lib, "dbghelp")
#pragma warning(error : 4715) 

#endif // WEAPON_LOHENGRIN_MAINFRAME_STDAFX_H