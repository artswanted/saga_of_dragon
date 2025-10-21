//!  stdafx.h : 자주 사용하지만 자주 변경되지는 않는
//!  표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
//!  들어 있는 포함 파일입니다.
//! 

#ifndef LOG_LOGSERVER_STDAFX_H
#define LOG_LOGSERVER_STDAFX_H

// 무시하는 경고 (이 외의 경고는 모두 오류로 처리한다!)
#pragma warning(disable : 4819)		// 코드 페이지 워닝
#pragma warning(disable : 4267)		// usngined int --> size_t 워닝
#pragma warning(disable : 4099) 

#define WIN32_LEAN_AND_MEAN		//!  거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define NOMINMAX				// PhysX의 min/max을 사용합니다.

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

#include "BM/bm.h"
#include "CEL/CEL.h"

#include "Lohengrin/Lohengrin.h"
#include "Variant/Variant.h"
#include "Collins/Collins.h"

#endif // LOG_LOGSERVER_STDAFX_H