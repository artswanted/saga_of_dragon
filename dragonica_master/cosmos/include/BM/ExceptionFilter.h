#pragma once

#include <new.h>
#include <eh.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief 미니 덤프를 실행하기 위한 클래스. 특별히 어떤 당위성이 있어서 만든 
/// 클래스는 아니고, 정적 변수와 함수를 가지고 있기 위해 만든 네임 스페이스 
/// 역할 클래스이다. 
///
/// 덤프의 초기화는 Install 함수를 통해 이루어진다. 프로그램 시작 부분 아무 
/// 곳에서나 PgExceptionFilter::Install(...) 함수를 호출해주면 된다. 
///
/// <pre>
/// int main()
/// {
///     ...
///     PgExceptionFilter::Install(CMiniDump2::DUMP_LEVEL_LIGHT);
///     ...
/// }
/// </pre>
///
/// GUI를 사용할 수 있는 프로그램의 경우, 사용자에게 덤프 파일 생성 여부를 묻는
/// 것도 괜찮다고 생각해서, 대화창 콜백 함수를 하나 집어 넣었다. Install 함수를 
/// 통해 대화창 콜백 함수를 설정하면, 덤프 생성시 이를 확인해 먼저 대화창을 
/// 띄운다. 대화창이 IDOK로 끝나면, 덤프 파일을 생성하고, 그외의 값으로 끝나면 
/// 덤프 파일을 생성하지 않는다. 기본적인 대화창 함수를 구현하자면 대충 아래와 
/// 같을 것이다.
///
/// <pre>
/// BOOL CALLBACK CrashDialogProc(
///     HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM /*lParam*/)
/// {
///     RECT parent, dlg;
///     int x, y;
/// 
///     switch(iMessage)
///     {
///     case WM_INITDIALOG:
///         ::GetWindowRect(::GetForegroundWindow(), &parent);
///         ::GetWindowRect(hDlg, &dlg);
///         x = (parent.left + parent.right - dlg.right) / 2;
///         y = (parent.top + parent.bottom - dlg.bottom) / 2;
///         ::MoveWindow(hDlg, x, y, dlg.right, dlg.bottom, FALSE);
//          ...
///         return TRUE;
///     case WM_COMMAND:
///         switch(LOWORD(wParam))
///         {
///         case IDOK: EndDialog(hDlg, IDOK); break;
///         case IDCANCEL: EndDialog(hDlg, IDCANCEL); break;
///         default: break;
///         }
///         return FALSE;
///     default:
///         break;
///     }
/// 
///     return FALSE;
/// }
/// </pre>
////////////////////////////////////////////////////////////////////////////////

class PgExceptionFilter
{
public:
	/// 덤프할 데이터의 수준
	typedef enum eDumpLevel
	{
		DUME_LEVEL_NONE,	//< MiniDump 사용하지 않음
		DUMP_LEVEL_LIGHT,	//< MiniDumpNormal을 사용
		DUMP_LEVEL_MEDIUM,	//< MiniDumpWithDataSegs를 사용
		DUMP_LEVEL_HEAVY,	//< MiniDumpWithFullMemory를 사용
	}E_DUMP_LEVEL;

	typedef enum eDumpOp
	{
		DUMP_OP_None				= 0x0000,
		DUMP_OP_ShowMessageBox		= 0x0001,	// Crash 발생한 것을 메시지 박스로 보여주기
		DUMP_OP_Exit_Program		= 0x0002,	// Dump 남기고 Process 완전 종료
		DUMP_OP_UseTimeInDumpFile	= 0x0004,	// Dump File 이름에 Timestamp를 사용한다.
	}E_DUMP_OPERATION;

	typedef int (CALLBACK *TerminateFunc)(void);
	typedef TerminateFunc PTerminateFunc;

private:
	static E_DUMP_LEVEL		ms_kDumpLevel;			//< 덤프 레벨.
	static char				ms_szAppName[_MAX_PATH];	//< 덤프 파일 이름
	static TCHAR			ms_CallStack[8192];		//< 콜스택 문자열
	static TCHAR			ms_Modules[8192];		//< 모듈 문자열
	static LPCTSTR			ms_DialogTemplate;		//< 대화창 템플릿
	static DLGPROC			ms_DialogProc;			//< 대화창 프로시져
	static LONG volatile	ms_iExceptedCount;		// 실제로 exception 발생하였는가?
	static BYTE				ms_kDumpOperation;
	static LPTOP_LEVEL_EXCEPTION_FILTER		ms_pfnPreviousHandler;
	static _se_translator_function			ms_pfnPrevTranslatorFunc;
	static _purecall_handler				ms_pfnPrevPureHandler;	
	static _PNH								ms_pfnPrevNewHandler;
	static _invalid_parameter_handler		ms_pfnPrevInvalidParameterHandler;
	static terminate_handler				ms_pfnPrevTerminateHandler;
	static unexpected_handler				ms_pfnPrevUnexpectedHandler;
	static PTerminateFunc					ms_pfnTerminateFunc;	// Terminate Func

public:
	static void Install( E_DUMP_LEVEL kDumpLevel, BYTE kOp = DUMP_OP_Exit_Program|DUMP_OP_UseTimeInDumpFile, PTerminateFunc pfnTerminateFunc = NULL );
	static void SetMessageBoxInfo( LPCTSTR dialogTemplate=NULL, DLGPROC dialogProc=NULL );

	static LONG GetExceptedCount(void){return ms_iExceptedCount;}
	static bool IsExcepted(void){return 0 < GetExceptedCount();}

private:
	static LONG WINAPI ExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );
	static LONG WINAPI WriteDump( PEXCEPTION_POINTERS pExceptionInfo );
	static void trans_func( unsigned int u, PEXCEPTION_POINTERS pExceptionInfo );
	static void purecall_handler();
	static int _new_handler(size_t);
	static void invalid_parameter_handler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t);
	static void terminate_handler_func();
	static void unexpected_handler_func();

private:// Not Used
	PgExceptionFilter(void);
	PgExceptionFilter( PgExceptionFilter const & );
	PgExceptionFilter& operator = ( PgExceptionFilter const & );
};
