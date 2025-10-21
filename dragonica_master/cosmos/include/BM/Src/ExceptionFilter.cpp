#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "ExceptionFilter.h"

#	pragma pack(push, exception)
#	pragma pack(8)
#	include <imagehlp.h>
#	pragma pack(pop, exception)

#	pragma comment(lib, "imagehlp.lib")

////////////////////////////////////////////////////////////////////////////////
///
/// 덤프 파일 옵션... from MSDN
///
/// - MiniDumpNormal
///     Include just the information necessary to capture stack traces for 
///     all existing threads in a process. 
///
/// - MiniDumpWithDataSegs 
///     Include the data sections from all loaded modules. This results in 
///     the inclusion of global variables, which can make the minidump file 
///     significantly larger. 
///
/// - MiniDumpWithFullMemory 
///     Include all accessible memory in the process. The raw memory data is 
///     included at the end, so that the Initial structures can be mapped 
///     directly without the raw memory information. This option can result 
///     in a very large file. 
///
/// - MiniDumpWithHandleData
///     Include high-level information about the operating system handles 
///     that are active when the minidump is made. 
///     \n Windows Me/98/95: This value is not supported.
///
/// - MiniDumpFilterMemory
///     Stack and backing store memory written to the minidump file should be 
///     filtered to remove all but the pointer values necessary to reconstruct 
///     a stack trace. Typically, this removes any private information. 
///
/// - MiniDumpScanMemory 
///     Stack and backing store memory should be scanned for pointer 
///     references to modules in the module list. If a module is referenced by 
///     stack or backing store memory, the ModuleWriteFlags member of the 
///     MINIDUMP_CALLBACK_OUTPUT structure is set to ModuleReferencedByMemory. 
///
/// - MiniDumpWithUnloadedModules
///     Include information from the list of modules that were recently 
///     unloaded, if this information is maintained by the operating system.
///     \n DbgHelp 5.1 and earlier: This value is not supported.
///
/// - MiniDumpWithIndirectlyReferencedMemory 
///     Include pages with data referenced by locals or other stack memory. 
///     This option can increase the size of the minidump file significantly.
///     \n DbgHelp 5.1 and earlier:  This value is not supported.
///
/// - MiniDumpFilterModulePaths
///     Filter module paths for information such as user names or important 
///     directories. This option may prevent the system from locating the 
///     image file and should be used only in special situations.
///     \n DbgHelp 5.1 and earlier:  This value is not supported.
///
/// - MiniDumpWithProcessThreadData
///     Include complete per-process and per-thread information from the 
///     operating system.
///     \n DbgHelp 5.1 and earlier:  This value is not supported.
///
/// - MiniDumpWithPrivateReadWriteMemory 
///     Scan the virtual address space for other types of memory to be 
///     included.
///     \n DbgHelp 5.1 and earlier:  This value is not supported.
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
namespace
{
	/// \brief 예외의 원인에 대한 문자열을 반환한다.
	extern LPCTSTR GetFaultReason(PEXCEPTION_POINTERS exPtrs);

	/// \brief 사용자 정보를 반환한다.
	extern LPCTSTR GetUserInfo();

	/// \brief 윈도우즈 버전을 반환한다.
	extern LPCTSTR GetOSInfo();

	/// \brief CPU 정보를 반환한다.
	extern LPCTSTR GetCpuInfo();

	/// \brief 메모리 정보를 반환한다.
	extern LPCTSTR GetMemoryInfo();

	/// \brief 윈도우즈 버전을 알아낸다.
	extern bool GetWinVersion(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild);

	/// \brief 예외의 원인에 대한 문자열을 반환한다.
	/// \param exPtrs 예외 구조체 포인터
	/// \return LPCTSTR 원인 문자열
	LPCTSTR GetFaultReason(PEXCEPTION_POINTERS exPtrs)
	{
		if (::IsBadReadPtr(exPtrs, sizeof(EXCEPTION_POINTERS))) 
			return _T("bad exception pointers");

		// 간단한 에러 코드라면 그냥 변환할 수 있다.
		switch(exPtrs->ExceptionRecord->ExceptionCode)
		{
		case EXCEPTION_ACCESS_VIOLATION:         return _T("EXCEPTION_ACCESS_VIOLATION");
		case EXCEPTION_DATATYPE_MISALIGNMENT:    return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
		case EXCEPTION_BREAKPOINT:               return _T("EXCEPTION_BREAKPOINT");
		case EXCEPTION_SINGLE_STEP:              return _T("EXCEPTION_SINGLE_STEP");
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		case EXCEPTION_FLT_DENORMAL_OPERAND:     return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
		case EXCEPTION_FLT_INEXACT_RESULT:       return _T("EXCEPTION_FLT_INEXACT_RESULT");
		case EXCEPTION_FLT_INVALID_OPERATION:    return _T("EXCEPTION_FLT_INVALID_OPERATION");
		case EXCEPTION_FLT_OVERFLOW:             return _T("EXCEPTION_FLT_OVERFLOW");
		case EXCEPTION_FLT_STACK_CHECK:          return _T("EXCEPTION_FLT_STACK_CHECK");
		case EXCEPTION_FLT_UNDERFLOW:            return _T("EXCEPTION_FLT_UNDERFLOW");
		case EXCEPTION_INT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
		case EXCEPTION_INT_OVERFLOW:             return _T("EXCEPTION_INT_OVERFLOW");
		case EXCEPTION_PRIV_INSTRUCTION:         return _T("EXCEPTION_PRIV_INSTRUCTION");
		case EXCEPTION_IN_PAGE_ERROR:            return _T("EXCEPTION_IN_PAGE_ERROR");
		case EXCEPTION_ILLEGAL_INSTRUCTION:      return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
		case EXCEPTION_STACK_OVERFLOW:           return _T("EXCEPTION_STACK_OVERFLOW");
		case EXCEPTION_INVALID_DISPOSITION:      return _T("EXCEPTION_INVALID_DISPOSITION");
		case EXCEPTION_GUARD_PAGE:               return _T("EXCEPTION_GUARD_PAGE");
		case EXCEPTION_INVALID_HANDLE:           return _T("EXCEPTION_INVALID_HANDLE");
			//case EXCEPTION_POSSIBLE_DEADLOCK:        return _T("EXCEPTION_POSSIBLE_DEADLOCK");
		case CONTROL_C_EXIT:                     return _T("CONTROL_C_EXIT");
		case 0xE06D7363:                         return _T("Microsoft C++ Exception");
		default:
			break;
		}

		// 뭔가 좀 더 복잡한 에러라면...
		static TCHAR szFaultReason[2048];
		::lstrcpy(szFaultReason, _T("Unknown")); 
		::FormatMessage(
			FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
			::GetModuleHandle(_T("ntdll.dll")),
			exPtrs->ExceptionRecord->ExceptionCode, 
			0,
			szFaultReason,
			0,
			NULL);

		return szFaultReason;
	}

	/// \brief 사용자 정보를 반환한다.
	/// \return LPCTSTR 사용자 이름
	LPCTSTR GetUserInfo()
	{
		static TCHAR szUserName[200] = {0,};

		ZeroMemory(szUserName, sizeof(szUserName));
		DWORD UserNameSize = _ARRAYSIZE(szUserName) - 1;

		if (!::GetUserName(szUserName, &UserNameSize))
			::lstrcpy(szUserName, _T("Unknown"));

		return szUserName;
	}

	/// \brief 윈도우즈 버전을 반환한다.
	/// \return LPCTSTR 윈도우즈 버전 문자열
	LPCTSTR GetOSInfo()
	{
		TCHAR szWinVer[50] = {0,};
		TCHAR szMajorMinorBuild[50] = {0,};
		int nWinVer = 0;
		GetWinVersion(szWinVer, &nWinVer, szMajorMinorBuild);

		static TCHAR szOSInfo[512] = {0,};
#ifdef UNICODE
		swprintf(szOSInfo, _ARRAYSIZE(szOSInfo)-1, L"%s (%s)",
			szWinVer, szMajorMinorBuild);
#else
		_snprintf_s(szOSInfo, _ARRAYSIZE(szOSInfo)-1, _TRUNCATE, "%s (%s)",
			szWinVer, szMajorMinorBuild);
#endif
		szOSInfo[_ARRAYSIZE(szOSInfo)-1] = 0;
		return szOSInfo;
	}

	/// \brief CPU 정보를 반환한다.
	/// \return LPCTSTR CPU 정보 문자열
	LPCTSTR GetCpuInfo()
	{
		// CPU 정보 기록
		SYSTEM_INFO    SystemInfo;
		GetSystemInfo(&SystemInfo);

		static TCHAR szCpuInfo[512] = {0,};
#ifdef UNICODE
		swprintf(szCpuInfo, _ARRAYSIZE(szCpuInfo)-1, 
			L"%d processor(s), type %d",
			SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);
#else
		_snprintf_s(szCpuInfo, _ARRAYSIZE(szCpuInfo)-1, _TRUNCATE, 
			"%d processor(s), type %d",
			SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);
#endif
		return szCpuInfo;
	}

	/// \brief 메모리 정보를 반환한다.
	/// \return LPCTSTR 메모리 정보 문자열
	LPCTSTR GetMemoryInfo()
	{
		static int const ONE_K = 1024;
		static int const ONE_M = ONE_K * ONE_K;
		static int const ONE_G = ONE_K * ONE_K * ONE_K;

		MEMORYSTATUS MemInfo;
		MemInfo.dwLength = sizeof(MemInfo);
		GlobalMemoryStatus(&MemInfo);

		static TCHAR szMemoryInfo[2048] = {0,};
#ifdef UNICODE
		swprintf(szMemoryInfo, _ARRAYSIZE(szMemoryInfo)-1, 
			L"%d%% of memory in use.\n"
			L"%d MB physical memory.\n"
			L"%d MB physical memory free.\n"
			L"%d MB paging file.\n"
			L"%d MB paging file free.\n"
			L"%d MB user address space.\n"
			L"%d MB user address space free.",
			MemInfo.dwMemoryLoad, 
			(MemInfo.dwTotalPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalVirtual + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailVirtual + ONE_M - 1) / ONE_M);
#else
		_snprintf_s(szMemoryInfo, _ARRAYSIZE(szMemoryInfo)-1, _TRUNCATE,
			"%d%% of memory in use.\n"
			"%d MB physical memory.\n"
			"%d MB physical memory free.\n"
			"%d MB paging file.\n"
			"%d MB paging file free.\n"
			"%d MB user address space.\n"
			"%d MB user address space free.",
			MemInfo.dwMemoryLoad, 
			(MemInfo.dwTotalPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalVirtual + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailVirtual + ONE_M - 1) / ONE_M);
#endif

		return szMemoryInfo;
	}

	/// \brief 윈도우즈 버전을 알아낸다.
	///
	/// This table has been assembled from Usenet postings, personal observations, 
	/// and reading other people's code.  Please feel free to add to it or correct 
	/// it.
	///
	/// <pre>
	/// dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
	/// 95            1               4                 0            950
	/// 95 SP1        1               4                 0            >950 && <=1080
	/// 95 OSR2       1               4               <10            >1080
	/// 98            1               4                10            1998
	/// 98 SP1        1               4                10            >1998 && <2183
	/// 98 SE         1               4                10            >=2183
	/// ME            1               4                90            3000
	///
	/// NT 3.51       2               3                51
	/// NT 4          2               4                 0            1381
	/// 2000          2               5                 0            2195
	/// XP            2               5                 1            2600
	/// 2003 Server   2               5                 2            3790
	///
	/// CE            3
	/// </pre>
	///
	/// \param pszVersion 버전 문자열을 집어넣을 포인터
	/// \param nVersion 버전 숫자값을 집어넣을 포인터
	/// \param pszMajorMinorBuild 빌드 문자열을 집어넣을 포인터
	/// \return bool 무사히 실행한 경우에는 true, 뭔가 에러가 생긴 경우에는 false
	bool GetWinVersion(LPTSTR pszVersion, int *nVersion, LPTSTR pszMajorMinorBuild)
	{
		// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s 0
#endif

#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT 2
#endif

#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE 3
#endif

		static LPCTSTR WUNKNOWNSTR     = _T("Unknown Windows Version");
		static LPCTSTR W95STR          = _T("Windows 95");
		static LPCTSTR W95SP1STR       = _T("Windows 95 SP1");
		static LPCTSTR W95OSR2STR      = _T("Windows 95 OSR2");
		static LPCTSTR W98STR          = _T("Windows 98");
		static LPCTSTR W98SP1STR       = _T("Windows 98 SP1");
		static LPCTSTR W98SESTR        = _T("Windows 98 SE");
		static LPCTSTR WMESTR          = _T("Windows ME");
		static LPCTSTR WNT351STR       = _T("Windows NT 3.51");
		static LPCTSTR WNT4STR         = _T("Windows NT 4");
		static LPCTSTR W2KSTR          = _T("Windows 2000");
		static LPCTSTR WXPSTR          = _T("Windows XP");
		static LPCTSTR W2003SERVERSTR  = _T("Windows 2003 Server");
		static LPCTSTR WCESTR          = _T("Windows CE");

		static int const WUNKNOWN      = 0;
		static int const W9XFIRST      = 1;
		static int const W95           = 1;
		static int const W95SP1        = 2;
		static int const W95OSR2       = 3;
		static int const W98           = 4;
		static int const W98SP1        = 5;
		static int const W98SE         = 6;
		static int const WME           = 7;
		static int const W9XLAST       = 99;
		static int const WNTFIRST      = 101;
		static int const WNT351        = 101;
		static int const WNT4          = 102;
		static int const W2K           = 103;
		static int const WXP           = 104;
		static int const W2003SERVER   = 105;
		static int const WNTLAST       = 199;
		static int const WCEFIRST      = 201;
		static int const WCE           = 201;
		static int const WCELAST       = 299;

		if (!pszVersion || !nVersion || !pszMajorMinorBuild) return false;

		::lstrcpy(pszVersion, WUNKNOWNSTR);
		*nVersion = WUNKNOWN;

		OSVERSIONINFO osinfo;
		osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if (!GetVersionEx(&osinfo)) return false;

		DWORD dwPlatformId   = osinfo.dwPlatformId;
		DWORD dwMinorVersion = osinfo.dwMinorVersion;
		DWORD dwMajorVersion = osinfo.dwMajorVersion;
		DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF; // Win 95 needs this

		TCHAR buf[50] = {0, };
#ifdef UNICODE
		swprintf(buf, _ARRAYSIZE(buf), L"%u.%u.%u", 
			dwMajorVersion, dwMinorVersion, dwBuildNumber);
#else
		_snprintf_s(buf, _ARRAYSIZE(buf), _TRUNCATE, "%u.%u.%u", 
			dwMajorVersion, dwMinorVersion, dwBuildNumber);
#endif
		::lstrcpy(pszMajorMinorBuild, buf);

		if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
		{
			if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
			{
				::lstrcpy(pszVersion, W95STR);
				*nVersion = W95;
			}
			else if ((dwMinorVersion < 10) && 
				((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
			{
				::lstrcpy(pszVersion, W95SP1STR);
				*nVersion = W95SP1;
			}
			else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
			{
				::lstrcpy(pszVersion, W95OSR2STR);
				*nVersion = W95OSR2;
			}
			else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
			{
				::lstrcpy(pszVersion, W98STR);
				*nVersion = W98;
			}
			else if ((dwMinorVersion == 10) && 
				((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
			{
				::lstrcpy(pszVersion, W98SP1STR);
				*nVersion = W98SP1;
			}
			else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
			{
				::lstrcpy(pszVersion, W98SESTR);
				*nVersion = W98SE;
			}
			else if (dwMinorVersion == 90)
			{
				::lstrcpy(pszVersion, WMESTR);
				*nVersion = WME;
			}
		}
		else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
			{
				::lstrcpy(pszVersion, WNT351STR);
				*nVersion = WNT351;
			}
			else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
			{
				::lstrcpy(pszVersion, WNT4STR);
				*nVersion = WNT4;
			}
			else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
			{
				::lstrcpy(pszVersion, W2KSTR);
				*nVersion = W2K;
			}
			else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
			{
				::lstrcpy(pszVersion, WXPSTR);
				*nVersion = WXP;
			}
			else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
			{
				::lstrcpy(pszVersion, W2003SERVERSTR);
				*nVersion = W2003SERVER;
			}
		}
		else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
		{
			::lstrcpy(pszVersion, WCESTR);
			*nVersion = WCE;
		}

		return true;

#undef VER_PLATFORM_WIN32s
#undef VER_PLATFORM_WIN32_WINDOWS
#undef VER_PLATFORM_WIN32_NT
#undef VER_PLATFORM_WIN32_CE
	}
};

PgExceptionFilter::E_DUMP_LEVEL			PgExceptionFilter::ms_kDumpLevel = PgExceptionFilter::DUME_LEVEL_NONE;
char									PgExceptionFilter::ms_szAppName[MAX_PATH] = {0,};
TCHAR									PgExceptionFilter::ms_CallStack[8192]    = {0,};
TCHAR									PgExceptionFilter::ms_Modules[8192]      = {0,};
LPCTSTR									PgExceptionFilter::ms_DialogTemplate     = NULL;
DLGPROC									PgExceptionFilter::ms_DialogProc         = NULL;
LPTOP_LEVEL_EXCEPTION_FILTER			PgExceptionFilter::ms_pfnPreviousHandler = NULL;
_se_translator_function					PgExceptionFilter::ms_pfnPrevTranslatorFunc = NULL;
_purecall_handler						PgExceptionFilter::ms_pfnPrevPureHandler = NULL;	
_PNH									PgExceptionFilter::ms_pfnPrevNewHandler = NULL;
_invalid_parameter_handler				PgExceptionFilter::ms_pfnPrevInvalidParameterHandler = NULL;
terminate_handler						PgExceptionFilter::ms_pfnPrevTerminateHandler = NULL;;
unexpected_handler						PgExceptionFilter::ms_pfnPrevUnexpectedHandler = NULL;
PgExceptionFilter::PTerminateFunc		PgExceptionFilter::ms_pfnTerminateFunc = NULL;
LONG volatile							PgExceptionFilter::ms_iExceptedCount = 0;
BYTE									PgExceptionFilter::ms_kDumpOperation = PgExceptionFilter::DUMP_OP_None;

void PgExceptionFilter::Install( E_DUMP_LEVEL kDumpLevel, BYTE kOp, PTerminateFunc pfnTerminateFunc )
{
	assert(ms_szAppName[0] == 0);
	assert(kDumpLevel >= DUME_LEVEL_NONE);
	assert(kDumpLevel <= DUMP_LEVEL_HEAVY);

	ms_kDumpLevel = kDumpLevel;
	ms_kDumpOperation = kOp;

	// 모듈 경로를 알아낸다.
	char szFileName[MAX_PATH] = {0,};
	::GetModuleFileNameA( NULL, szFileName, MAX_PATH );

	// 확장자를 제거한 모듈 경로를 준비해둔다.
	char* dot = ::strrchr(ms_szAppName, '.');
	::lstrcpynA(ms_szAppName, szFileName, (int)(dot - szFileName + 1));

	// 예외 처리 핸들러를 설정한다.
	ms_pfnPreviousHandler = ::SetUnhandledExceptionFilter(ExceptionFilter);
	ms_pfnPrevTranslatorFunc = _set_se_translator(trans_func);
	ms_pfnPrevPureHandler = ::_set_purecall_handler(purecall_handler);
	ms_pfnPrevNewHandler= ::_set_new_handler(_new_handler);
	ms_pfnPrevInvalidParameterHandler= ::_set_invalid_parameter_handler(invalid_parameter_handler);
	ms_pfnPrevTerminateHandler = ::set_terminate(terminate_handler_func);
	ms_pfnPrevUnexpectedHandler = ::set_unexpected(unexpected_handler_func);
	ms_pfnTerminateFunc = pfnTerminateFunc;
}

void PgExceptionFilter::SetMessageBoxInfo( LPCTSTR dialogTemplate, DLGPROC dialogProc )
{
	ms_DialogTemplate = dialogTemplate;
	ms_DialogProc = dialogProc;
}

LONG WINAPI PgExceptionFilter::ExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo ) 
{
	if ( NULL == pExceptionInfo )
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	HANDLE const hCurrentProcess = ::GetCurrentProcess();
	HANDLE const hCurrentThread	= ::GetCurrentThread();

	// Initialize symbols..
	::SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
	BOOL const bInit = ::SymInitialize( hCurrentProcess, NULL, TRUE );
	if( bInit ) 
	{
		// Create StackDump File
		char szFileName[MAX_PATH] = {0,};
		{
			SYSTEMTIME t;
			::GetLocalTime(&t);

			::sprintf_s(szFileName, MAX_PATH, "StackDump_%04d%02d%02d_%02d%02d%02d.txt", 
				t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );
		}

		FILE* pFile = NULL;
		errno_t const err = ::fopen_s( &pFile, szFileName, "at+");
		if( !err && pFile ) 
		{
			char szDate[MAX_PATH] = {0,};
			char szTime[MAX_PATH] = {0,};
			::_strdate_s(szDate, sizeof(szDate) );
			::_strtime_s(szTime, sizeof(szTime) );

			::fprintf(pFile, "*============================================================*\n");
			::fprintf(pFile, "\tDate / Time : [%s %s]\n", szDate, szTime );
			::fprintf(pFile, "\tProcessID / ThreadID : [0x%08X] / [0x%08X]\n", GetCurrentProcessId(), GetCurrentThreadId());
			::fprintf(pFile, "\tExceptionCode : [0x%08X]\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
			::fprintf(pFile, "\tCommandLine : %s\n", GetCommandLineA());
			::fprintf(pFile, "*============================================================*\n");

			::printf("*============================================================*\n");
			::printf("\tUnhandled excetpion triggerd!\n");
			::printf("\tDate / Time : [%s %s]\n", szDate, szTime );
			::printf("\tProcessID / ThreadID : [0x%08X] / [0x%08X]\n", GetCurrentProcessId(), GetCurrentThreadId());
			::printf("\tExceptionCode : [0x%08X]\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
			::printf("\tCommandLine : %s\n", GetCommandLineA());

			CONTEXT& context		= *pExceptionInfo->ContextRecord;
			STACKFRAME stackFrame = {0,};
			stackFrame.AddrPC.Offset	= context.Eip;
			stackFrame.AddrPC.Mode		= AddrModeFlat;
			stackFrame.AddrStack.Offset	= context.Esp;
			stackFrame.AddrStack.Mode	= AddrModeFlat;
			stackFrame.AddrFrame.Offset	= context.Ebp;
			stackFrame.AddrFrame.Mode	= AddrModeFlat;

			// stackwalk!!
			for( int i = 0; i < 512; ++i ) 
			{
				if( stackFrame.AddrPC.Offset == 0 )
				{
					break;
				}

				BOOL const bSuccess = ::StackWalk( IMAGE_FILE_MACHINE_I386, hCurrentProcess, hCurrentThread, 
					&stackFrame, &context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL);
				if( bSuccess ) 
				{
					if(stackFrame.AddrFrame.Offset == 0) {break;}

					DWORD dwDisplacement = 0;
					char chSymbol[sizeof(IMAGEHLP_SYMBOL) + 512 ] = {0,};
					PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)chSymbol;

					pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
					pSymbol->MaxNameLength = 512;

					if( ::SymGetSymFromAddr( hCurrentProcess, stackFrame.AddrPC.Offset, &dwDisplacement, pSymbol) )
					{
						::fprintf(pFile, "0x%08x - %s() + %xh\n", 
							stackFrame.AddrPC.Offset, pSymbol->Name, dwDisplacement/*stackFrame.AddrPC.Offset-pSymbol->Address*/);
					}
					else
					{
						::fprintf(pFile, "0x%08x - [Unknown Symbol:Error %u]\n", 
							stackFrame.AddrPC.Offset, GetLastError());
					}

					IMAGEHLP_MODULE module = {sizeof(IMAGEHLP_MODULE), 0,};
					if( ::SymGetModuleInfo( hCurrentProcess, stackFrame.AddrPC.Offset, &module) ) 
					{
						::fprintf(pFile, "\tImageName: %s\n", module.ImageName);
						::fprintf(pFile, "\tLoadedImageName: %s\n", module.LoadedImageName);
					}

					IMAGEHLP_LINE line = {sizeof(IMAGEHLP_LINE), 0,};
					for(int i = 0; i < 512; ++i) 
					{
						if( ::SymGetLineFromAddr( hCurrentProcess, stackFrame.AddrPC.Offset - i, &dwDisplacement, &line) ) 
						{
							::fprintf(pFile, "\tFile: %s, %u Line\r\n", line.FileName, line.LineNumber);
							break;
						}
					}
				} 
				else 
				{
					break;
				}
			}

			::fprintf(pFile, "\n");
		}

		if( pFile )
		{
			fclose(pFile);
		}

		::SymCleanup(hCurrentProcess);
	}

	if ( ms_kDumpLevel )
	{
		::printf("\tWriting Dump....\n");
		::printf("*============================================================*\n");
		return WriteDump( pExceptionInfo );
	}

	::printf("*============================================================*\n");
	return EXCEPTION_EXECUTE_HANDLER;
}

LONG WINAPI PgExceptionFilter::WriteDump( PEXCEPTION_POINTERS pExceptionInfo )
{
	if ( NULL == pExceptionInfo )
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	::InterlockedIncrement( &ms_iExceptedCount );

	// 대화창이 설정되어 있다면 보여준다.
	if (ms_DialogTemplate != NULL && ms_DialogProc != NULL)
	{
		if ( DialogBox( NULL, ms_DialogTemplate, HWND_DESKTOP, ms_DialogProc) != IDOK )
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	HANDLE const hCurrentProcess = ::GetCurrentProcess();
	HANDLE const hCurrentThread	= ::GetCurrentThread();

	// 덤프 파일 이름 += 시간 문자열
	char szDumpPath[MAX_PATH*2] = {0,};
	char szLogPath[MAX_PATH*2] = {0,};
	::strcat_s( szDumpPath, MAX_PATH*2, ms_szAppName );
	::strcat_s( szLogPath, MAX_PATH*2, ms_szAppName );
	if ( DUMP_OP_UseTimeInDumpFile & ms_kDumpOperation )
	{
		SYSTEMTIME t;
		::GetLocalTime(&t);

		char szTail[MAX_PATH] = {0,};

		sprintf_s(	szTail, MAX_PATH
				,	"_%04d%02d%02d_%02d%02d%02d"
				,	t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond
				);

		::strcat_s(szDumpPath, MAX_PATH*2, szTail);
		::strcat_s(szLogPath, MAX_PATH*2, szTail);
	}

	::strcat_s(szDumpPath, MAX_PATH*2, ".dmp");
	::strcat_s(szLogPath,  MAX_PATH*2, ".log");

	// 먼저 실행 파일이 있는 디렉토리에서 DBGHELP.DLL을 로드해 본다.
	// Windows 2000 의 System32 디렉토리에 있는 DBGHELP.DLL 파일은 버전이 
	// 오래된 것일 수 있기 때문이다. (최소 5.1.2600.0 이상이어야 한다.)
	HMODULE hDLL = NULL;
	char szDbgHelpPath[MAX_PATH] = {0, };
	if (::GetModuleFileNameA(NULL, szDbgHelpPath, MAX_PATH))
	{
		if (char *slash = ::strrchr(szDbgHelpPath, '\\'))
		{
			::lstrcpyA( slash + 1, "DBGHELP.DLL" );
			hDLL = ::LoadLibraryA(szDbgHelpPath);
		}
	}

	// 현재 디렉토리에 없다면, 아무 버전이나 로드한다.
	if ( NULL == hDLL ) 
	{
		hDLL = ::LoadLibrary(_T("dbghelp.dll"));
	}

	// DBGHELP.DLL을 찾을 수 없다면 더 이상 진행할 수 없다.
	if ( NULL == hDLL )
	{
//		LogToFile(szLogPath, _T("dbghelp.dll not found"));
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// DLL 내부에서 MiniDumpWriteDump API를 찾는다.
	// based on dbghelp.h
	typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
		HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
		);

	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
		(MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
//		LogToFile(szLogPath, _T("dbghelp.dll too old"));
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// 파일을 생성한다.
	HANDLE hFile = ::CreateFileA(
		szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
	//	LogToFile(szLogPath, _T("failed to Create dump file '%s' (error %d)"), 
	//		szDumpPath, ::GetLastError());
		return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION ExParam;
	ExParam.ThreadId = ::GetCurrentThreadId();
	ExParam.ExceptionPointers = pExceptionInfo;
	ExParam.ClientPointers = FALSE;

	MINIDUMP_TYPE dumptype = MiniDumpNormal;
	switch(ms_kDumpLevel)
	{
	case DUMP_LEVEL_LIGHT:{dumptype = MiniDumpNormal;}break;
	case DUMP_LEVEL_MEDIUM:{dumptype = MiniDumpWithDataSegs;}break;
	case DUMP_LEVEL_HEAVY:{dumptype = MiniDumpWithFullMemory;}break;
	}

	// 덤프 파일 생성 결과를 로그 파일에다 기록한다.
	if (pfnMiniDumpWriteDump(
		hCurrentProcess, ::GetCurrentProcessId(), 
		hFile, dumptype, &ExParam, NULL, NULL))
	{
	}
	else
	{
//		LogToFile(szLogPath, _T("failed to save dump file to '%s' (error %d)"), 
//			szDumpPath, ::GetLastError());
	}

	::CloseHandle(hFile);

	// 이전에 등록된 ExceptionHandler가 있다면 그것을 먼저 호출해 준다.
	if (ms_pfnPreviousHandler != NULL)
	{
		(ms_pfnPreviousHandler)(pExceptionInfo);
	}

	if ( DUMP_OP_ShowMessageBox & ms_kDumpOperation )
	{
		TCHAR chMessage[500];
		_stprintf_s(chMessage, 500, _T("[CommandLine] %s\nExceptionReason [%s][0x%08x] Address[0x%08p]"), 
			GetCommandLine(),
			GetFaultReason(ExParam.ExceptionPointers),
			ExParam.ExceptionPointers->ExceptionRecord->ExceptionCode,
			ExParam.ExceptionPointers->ExceptionRecord->ExceptionAddress);
		::MessageBox(NULL, chMessage, _T("Exception"), MB_OK);
	}

	//abort();
	//ExitProcess(0);
	if ( DUMP_OP_Exit_Program & ms_kDumpOperation )
	{
		if ( ms_pfnTerminateFunc )
		{
			(ms_pfnTerminateFunc)();
		}

		TerminateProcess(hCurrentProcess, 0);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void PgExceptionFilter::trans_func(unsigned int u, PEXCEPTION_POINTERS lpExpPtr)
{
	TCHAR chMessage[500];
	_stprintf_s(chMessage, 500, _T("trans_func: %d"), u);
	OutputDebugString(chMessage);
	WriteDump(lpExpPtr);
}

void PgExceptionFilter::purecall_handler()
{
	OutputDebugString(TEXT("purecall_handler excuted"));
	__asm int 3;
}

int PgExceptionFilter::_new_handler(size_t new_size)
{
	TCHAR chMessage[500];
	_stprintf_s(chMessage, 500, _T("new handler excuted: %d size"), new_size);
	OutputDebugString(chMessage);
	__asm int 3;

	return (int)new_size;
}

void PgExceptionFilter::invalid_parameter_handler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t)
{
	TCHAR chMessage[500];
	_stprintf_s(chMessage, 500, _T("invliad_parameter_handler: %s, %s, %s, %d"), 
		expression ? expression : L"NULL", 
		function ? function : L"NULL", 
		file ? file : L"NULL", 
		line);
	OutputDebugString(chMessage);
	__asm int 3;
}

void PgExceptionFilter::terminate_handler_func()
{
	OutputDebugString(L"terminate_handler_func excuted");
	__asm int 3;
}

void PgExceptionFilter::unexpected_handler_func()
{
	OutputDebugString(L"unexpected_handler_func excuted");
	__asm int 3;
}
