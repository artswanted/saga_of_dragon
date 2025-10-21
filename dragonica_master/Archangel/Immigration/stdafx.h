//!  stdafx.h : 자주 사용하지만 자주 변경되지는 않는
//!  표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
//!  들어 있는 포함 파일입니다.
//! 

#ifndef IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_STDAFX_H
#define IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_STDAFX_H

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
#include "BM/ExceptionFilter.h"
#include "BM/vstring.h"
#include "BM/FileSupport.h"
#include "BM/LocalMgr.h"
#include "BM/PgFilterString.h"
#include "BM/Stream.h"

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "CEL/ConsoleCommander.h"

#include "Lohengrin/Lohengrin.h"
#include "Lohengrin/PacketStruct.h"
#include "Collins/Collins.h"
#include "FCS/FCS.h"
#include "Variant/Variant.h"
#include "Variant/PgDBCache.h"
#include "variant/PgTotalObjectMgr.h"
#include "HellDart/Helldart.h"

#include "PgSendWrapper.h"

#endif // IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_STDAFX_H