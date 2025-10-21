// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef MACHINE_MMC_STDAFX_H
#define MACHINE_MMC_STDAFX_H

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						
#define WIN32_LEAN_AND_MEAN		//!  거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <ostream>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

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

#include "Collins/Collins.h"
#include "Lohengrin/Lohengrin.h"
#include "Lohengrin/PgRealmManager.h"
#include "Variant/Variant.h"

#define USE_COMMAND_LEVEL 10
// TODO: reference additional headers your program requires here

#endif // MACHINE_MMC_STDAFX_H