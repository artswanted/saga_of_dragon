#pragma	once

#define WINVER 0x500
#define _WIN32_WINNT 0x500

#include	<winsock2.h>
#include	<windows.h>

#include	<io.h>
#include	<string>
#include	<shlobj.h>
#include	<commdlg.h>
#include	<process.h>
#include	<wininet.h>
#include	<atlbase.h>
#include	<atlhost.h>
#include	<exdispid.h>		

//BM
#include	"BM/BM.h"
#include	"BM/HSEL.h"
#include	"BM/Guid.h"
#include	"BM/vstring.h"
#include	"BM/localmgr.h"
#include	"BM/FileSupport.h"
#include	"BM/ThreadObject.h"
#include	"BM/PgFileMapping.h"
#include	"SimpEnc/SimpEnc.h"
#include	"tinyxml/tinyxml.h"
#include	"DataPack/PgDataPackManager.h"

#include	"BuildNumber.h"
#include	"Window.h"
#include	"FTPMgr.h"
#include	"resource.h"
#include	"PgAXPlay.h"
#include	"ConfigDef.h"
#include	"ConfigXml.h"
#include	"PatchCommon.h"
#include	"PatchProcess.h"
#include	"HBitmap.h"
#include	"TEdit.h"
#include	"CTButton.h"
#include	"SiteControl.h"
#include	"OSApi.h"

#pragma	comment(lib, "wininet.lib")
#pragma	comment(lib, "comsuppw.lib")
#pragma	comment(lib, "msimg32.lib") // ���� Blt

const BYTE PG_PATCHER_CRC_SIZE = 100;
const int WINSIZEX = 514;
const int WINSIZEY = 395;

#ifndef NO_FTP_LOCAL_TEST
extern class CFTPMgr		g_FTPMgr;
#else
extern class CDummyFTPMgr	g_FTPMgr;
#endif //NO_FTP_LOCAL_TEST
extern class Window*		g_WinApp;
extern class CPatchProcess	g_PProcess;

wchar_t const* const WSTR_OPEN = L"open";
std::wstring const WSTR_KOR = L"KOR";
std::wstring const WSTR_ENG = L"ENG";
std::wstring const WSTR_DEU = L"DEU";
std::wstring const WSTR_FRA = L"FRA";
std::wstring const WSTR_RUS = L"RUS";
std::wstring const WSTR_BRA = L"BRA";
std::wstring const WSTR_JPN = L"JPN";
std::wstring const WSTR_CHT = L"CHT";
std::wstring const WSTR_CHS = L"CHS";

std::wstring const WSTR_PATCHERCONFIGFILE = L".\\PatcherConfig.ini";
std::wstring const WSTR_LOCALCONFIGFILE = L".\\local.ini";
std::wstring const WSTR_MSG_INIFILE = L"./Message.ini";
std::wstring const WSTR_SUB_ADRESS = L"SUB_ADDRESS";
std::wstring const DIR_MARK(L".\\");
std::wstring const CUR_DIR_MARK(L"./");

std::string const STR_BIG5("BIG5");
std::string const STR_EUCTW("EUC-TW");

extern std::wstring WSTR_GAME_NAME;
extern std::wstring WSTR_PATCHER_NAME;
extern std::wstring WSTR_VERSION;

DWORD const NONE_USE_COLORKEY= RGB(255,255,254);

int const MAX_MSG_CHAR = 256;

#define WM_RUN_RECOVERYMODE		WM_USER + 1

#ifdef INFO_LOG
#undef INFO_LOG
#endif

#ifdef PATCHER_DEBUG_LOG
#include "BM/TimeCheck.h"
#include "DebugLog.h"
extern BM::PgDebugLog g_kLog;
#define INFO_LOG	g_kLog.Log
#else
#define INFO_LOG	/##/
#endif

#ifdef PATHCER_DEBUG
#define DEBUG_BREAK	__asm int 3;
#else
#define DEBUG_BREAK
#endif