/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2007 IntelleSoft.
 * All rights reserved.
 *
 * Description: Precomplied header file.
 * Author: Maksim Pyatkovskiy.
 *
 * This source code is only intended as a supplement to the
 * BugTrap package reference and related electronic documentation
 * provided with the product. See these sources for detailed
 * information regarding the BugTrap package.
 */

#pragma once

#ifdef _MANAGED
 #pragma unmanaged              // Compile all code as unmanaged by default
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <winsock2.h>
#include <shellapi.h>
#include <psapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdlib.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vdmdbg.h>
#include <dbghelp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <mapi.h>
#include <lmcons.h>
#include <wininet.h>
#include <process.h>
#include <zmouse.h>
#include <limits.h>
#include "minizip/zip.h"
#include <stdio.h>

struct _IMAGELIST { }; // unresolved typeref token

//#define DEBUG_WITH_BOUNDSCHECKER

#ifdef DEBUG_WITH_BOUNDSCHECKER
 #include <crtdbg.h>
 #pragma warning (disable : 4702) // unreachable code
 #define DEBUG_NEW new
#else
 #include "LeakWatcher.h"
#endif

#include <malloc.h>

#define countof(array) (sizeof(array) / sizeof((array)[0]))

#ifdef _MT_
	#pragma comment(lib, "zlib_MT.lib")	
	#pragma comment(lib, "minizip_MT.lib")
#endif

#ifdef _MTd_
	#pragma comment(lib, "zlib_MTd.lib")	
	#pragma comment(lib, "minizip_MTd.lib")
#endif

#ifdef _MTo_
	#pragma comment(lib, "zlib_MTo.lib")	
	#pragma comment(lib, "minizip_MTo.lib")
#endif

#ifdef _MD_
	#pragma comment(lib, "zlib_MD.lib")	
	#pragma comment(lib, "minizip_MD.lib")
#endif

#ifdef _MDd_
	#pragma comment(lib, "zlib_MDd.lib")	
	#pragma comment(lib, "minizip_MDd.lib")
#endif

#ifdef _MDo_
	#pragma comment(lib, "zlib_MDo.lib")	
	#pragma comment(lib, "minizip_MDo.lib")
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "version.lib") 
#pragma comment(lib, "wininet.lib")