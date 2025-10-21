//!  stdafx.h : АЪБЦ »зїлЗПБцёё АЪБЦ єЇ°жµЗБцґВ ѕКґВ
//!  ЗҐБШ ЅГЅєЕЫ ЖчЗФ ЖДАП №Ч ЗБ·ОБ§Ж® °ь·Г ЖчЗФ ЖДАПАМ
//!  µйѕо АЦґВ ЖчЗФ ЖДАПАФґПґЩ.
//! 

#ifndef CENTER_CENTERSERVER_MAINFRAME_STDAFX_H
#define CENTER_CENTERSERVER_MAINFRAME_STDAFX_H

#define WIN32_LEAN_AND_MEAN		//!  °ЕАЗ »зїлµЗБц ѕКґВ і»їлАє Windows ЗмґхїЎј­ Б¦їЬЗХґПґЩ.
#include <stdio.h>
#include <tchar.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <ostream>

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "BM/BM.h"
#include "BM/Guid.h"
#include "BM/Objectpool.h"
#include "BM/ExceptionFilter.h"
#include "BM/vstring.h"
#include "BM/FileSupport.h"
#include "BM/Stream.h"
#include "BM/LocalMgr.h"
#include "BM/PgFilterString.h"

#include "CEL/CEL.h"
#include "CEL/CoreCenter.h"
#include "CEL/ConsoleCommander.h"

#include "Lohengrin/Lohengrin.h"
//#define _MEMORY_TRACKING
#include "Lohengrin/MemoryTrack.h"

#include "FCS/FCS.h"

#include "Collins/Collins.h"

#include "Variant/Variant.h"
#include "Variant/gm_const.h"
#include "Variant/PgDBCache.h"
#include "variant/PgTotalObjectMgr.h"

//!  TODO: ЗБ·О±Ч·ҐїЎ ЗКїдЗС ГЯ°Ў ЗмґхґВ ї©±вїЎј­ ВьБ¶ЗХґПґЩ.
#include "PgServerSetMgr.h"
#include "PgSwitchAssignMgr.h"
#include "PgSendWrapper.h"
#include "PgUserWrapper.h"

#include "Contents/PgTask_Contents.h"
/*
#include "NC/IBGameDefine.h"
#include "NC/IBGameInterface.h"

#pragma comment(lib, "NC/IBGameInterface.lib")
*/
//LOCAL_MGR::CLocal g_kLocal;

//#define _DEBUGGING

#endif // CENTER_CENTERSERVER_MAINFRAME_STDAFX_H