//-----------------------------------------------------------------------------
//	API - Dialog Programing Base Window Framework 					
//-----------------------------------------------------------------------------
#pragma	warning ( disable : 4996 )

#include	"defines.h"
#include	"OriginalPatch.h"
#include	"CTray.h"
//-----------------------------------------------------------------------------
//  Area Variable
//-----------------------------------------------------------------------------
HWND		g_hTemp;

#ifndef NO_FTP_LOCAL_TEST
CFTPMgr			g_FTPMgr;
#else
CDummyFTPMgr	g_FTPMgr;
#endif //NO_FTP_LOCAL_TEST
Window*			g_WinApp;
CPatchProcess	g_PProcess;
std::wstring	g_CmdLine;

#ifdef RUSSIA
std::wstring WSTR_GAME_NAME = L"";
std::wstring WSTR_PATCHER_NAME = L"Патчер";
std::wstring WSTR_VERSION = L"Версия";
#else

#ifdef AMERICA
std::wstring WSTR_GAME_NAME = L"DragonSaga";
#else
std::wstring WSTR_GAME_NAME = L"Dragonica";
#endif

std::wstring WSTR_PATCHER_NAME = L"Patcher";
std::wstring WSTR_VERSION = L"Ver";
#endif

//-----------------------------------------------------------------------------
//  Define Function Proto Type 
//-----------------------------------------------------------------------------
LRESULT	CALLBACK	WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void OnIDLE(HWND hWnd);
void InitMessages();

//-----------------------------------------------------------------------------
//  Window Main Function
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	BM::g_bCalcSHA2CheckSum = true;
	BM::g_bUseFormattedFile = true;

	g_CmdLine = UNI(lpszCmdParam);
	InitMessages();

	g_WinApp = new Window(NULL, hInstance);
	if( !g_WinApp )
	{
		return false;
	}

	g_WinApp->SetDirectory();
	BM::vstring title;
	#ifdef RUSSIA
	title = L"Драконика";
	#else
	title = WSTR_GAME_NAME;
	title += L" ";
	title += WSTR_PATCHER_NAME;
	title += L" (";
	title += PG_PRODUCT_VERSION_STRW;
	title += L")";
	#endif

	CPatchProcess::EOperatedError eError = g_PProcess.OperatedCheck();
	if (eError == CPatchProcess::EOperatedError::PatcherExist)
	{
		HWND hwnd = FindWindowW(UNI(title), NULL);
		if (hwnd != NULL) // smth bad?
		{
			ShowWindow(hwnd, SW_RESTORE);
			::SetForegroundWindow(hwnd);
		}
		return 0;
	}

	g_WinApp->Initialize(title, WS_POPUP|WS_CLIPCHILDREN, WS_EX_LAYERED);
	{
		SWindowIconInfo	kIconInfo;
		kIconInfo.hIcon		= LoadIcon(hInstance, IDI_APPLICATION);
		kIconInfo.hIconSm	= LoadIcon(hInstance, IDI_APPLICATION);
		if( !g_WinApp->Register(kIconInfo, WndProc) )
		{
			return 0;
		}
	}

	if( g_WinApp->Create() )
	{
		SetLayeredWindowAttributes(g_WinApp->GetHandle(), RGB(255, 0, 255), 255, LWA_COLORKEY);
		g_PProcess.Init(g_CmdLine);
		if (eError == CPatchProcess::EOperatedError::ClientExist)
		{
			WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_CLIENTALREADYRUN), L"Error", 0);
			return 0;
		}
		else if (eError == CPatchProcess::EOperatedError::PatcherExist)
		{
			WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_PATCHERALREADYRUN), L"Error", 0);
			return 0;
		}
		g_WinApp->Display(nCmdShow);

		switch( g_PProcess.GetLocal() )
		{
		case CPatchProcess::LS_TAIWAN:
			{
				if( !g_PProcess.SubInit() )
				{
					return 0;
				}
			}break;
		case CPatchProcess::LS_JAPAN:
			{
				HKEY hkSub;
				DWORD dwDisposition;
				int key_num = 0;
				LPCWSTR kStr = _T("Software\\Nexon\\dragonica");
				LPCWSTR kBaseStr = _T("BaseDir");
				LPCWSTR kExecutableStr = _T("Executable");
				LPCWSTR kRootPathStr = _T("RootPath");

				LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)kStr, 0L, KEY_WRITE, &hkSub);
				if( ERROR_SUCCESS != lRes )
				{
					lRes = RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)kStr, 0, (LPWSTR)"", 0, KEY_READ | KEY_WRITE, NULL, &hkSub, &dwDisposition);
					/*if( ERROR_SUCCESS != lRes )
					{
						return false;
					}*/					
				}


				wchar_t UserInfoPath[MAX_PATH] = {0,};
				wchar_t UserDataPath[MAX_PATH] = {0,};
				::GetCurrentDirectory(MAX_PATH, UserInfoPath);

				swprintf_s(UserDataPath, MAX_PATH, L"%s\\Patcher.exe", UserInfoPath);

				lRes = RegSetValueEx(hkSub, (LPCWSTR)kBaseStr, 0, REG_SZ, (BYTE*)(UserDataPath) , sizeof((UserDataPath)));
				if( lRes != ERROR_SUCCESS )
				{
					return false;
				}
				lRes = RegSetValueEx(hkSub, (LPCWSTR)kExecutableStr, 0, REG_SZ, (BYTE*)(UserDataPath) , sizeof((UserDataPath)));
				if( lRes != ERROR_SUCCESS )
				{
					return false;
				}
				lRes = RegSetValueEx(hkSub, (LPCWSTR)kRootPathStr, 0, REG_SZ, (BYTE*)(UserInfoPath) , sizeof((UserInfoPath)));
				if( lRes != ERROR_SUCCESS )
				{
					return false;
				}
				RegCloseKey(hkSub);

				if( g_CmdLine.empty() ) // 패처 단독실행(인자가 없다)이면 넥슨 홈페이지 연결
				{
					if( g_PProcess.GetURL().empty() )
					{
						ShellExecute( NULL, L"open", L"http://dragonica.nexon.co.jp/",NULL, NULL, SW_SHOWNORMAL );
					}
					else
					{
						ShellExecute( NULL, L"open", g_PProcess.GetURL().c_str(), NULL, NULL, SW_SHOWNORMAL );
					}
					
					::exit(0);					
				}
			}break;
		default:
			{
			}break;
		}
		g_PProcess.SubDisplay(false);
		g_PProcess.UpdateClient();
		g_WinApp->OnLoop(OnIDLE);
		g_WinApp->Terminate();
		BM::PgDataPackManager::Clear();
	}

	delete g_WinApp;
	return 0;
}

void InitMessages()
{
	wchar_t	szTemp[MAX_MSG_CHAR] = {0,};
	::GetPrivateProfileString(L"Common", L"GameName", WSTR_GAME_NAME.c_str(), szTemp, MAX_MSG_CHAR, WSTR_MSG_INIFILE.c_str());
	WSTR_GAME_NAME = szTemp;

	::GetPrivateProfileString(L"Common", L"PatcherName", WSTR_GAME_NAME.c_str(), szTemp, MAX_MSG_CHAR, WSTR_MSG_INIFILE.c_str());
	WSTR_PATCHER_NAME = szTemp;

	::GetPrivateProfileString(L"Common", L"Version", WSTR_VERSION.c_str(), szTemp, MAX_MSG_CHAR, WSTR_MSG_INIFILE.c_str());
	WSTR_VERSION= szTemp;
}

//-----------------------------------------------------------------------------
//  Window Main Procedure
//-----------------------------------------------------------------------------
LRESULT	CALLBACK	WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case SWM_TRAYMSG:
		{
			CTray::WndProc(hWnd, iMessage, wParam, lParam);
		}
		break;
	case WM_ERASEBKGND:
		{	
			HDC	hDC = GetDC(hWnd);
			g_PProcess.DrawDlgFrame(hDC);
			ReleaseDC(hWnd, hDC);
		}
		return 0;
	case WM_DRAWITEM:
		{
			g_PProcess.OwnerDraw((LPDRAWITEMSTRUCT)lParam);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			HDC		hDC = (HDC)wParam;
			HWND	hWnd = (HWND)lParam;

			SEditInfo const& EditInfo = g_PProcess.GetEditInfo( hWnd );

			SetBkColor(hDC, RGB(EditInfo.BGR, EditInfo.BGG, EditInfo.BGB));
			SetTextColor(hDC, RGB(EditInfo.TR, EditInfo.TG, EditInfo.TB));
		}return TRUE;
	case WM_PAINT:
		{	
			if(g_PProcess.IsReloadResDat())
			{
				g_PProcess.LoadResDat();
				g_PProcess.SetReloadResDat(false);
			}

			POINT kMousePt;
			GetCursorPos(&kMousePt);
			ScreenToClient(hWnd, &kMousePt);
			g_PProcess.CheckMouseOver(kMousePt.x, kMousePt.y);

			PAINTSTRUCT		ps;
			HDC	hDC = BeginPaint(hWnd, &ps);
			g_PProcess.Paint(hDC);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_COMMAND:
		{
			g_PProcess.Command(wParam, lParam);
		}
		break;
	case WM_RUN_RECOVERYMODE:
		{
			if(!PgPatchCoreUtil::OriginalPatchIDCheck())
			{// 현재 버전이 패치할 버전 이상일때
				if( IDNO == WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_RECOVERY_CONFIRM), L"Notice", MB_YESNO) )
				{// 풀체킹 하지 않는다면 리턴
					return 0; 
				}
			}
			g_PProcess.FullChecking();
		}
		break;
	case WM_NCHITTEST:
		{
			//	Window Draging
			UINT	hRet = DefWindowProc(hWnd, WM_NCHITTEST, wParam, lParam);
			if(hRet == HTCLIENT)
			{
				return	HTCAPTION;
			}
			return	hRet;
		}
		break;
#ifdef PATCHER_DEBUG
	case WM_KEYDOWN:
		{
			if (0x30 <= wParam && wParam <= 0x39)
			{
				int precent = (wParam - 0x30) * 10;
				if (precent == 0)
					precent = 100; // key 0 is 100%

				g_PProcess.SetTotalSize(100);
				g_PProcess.SetFileSize(precent);
				g_PProcess.SetTotalCnt(precent);
				g_PProcess.SetFileCnt(precent);
			}
		}
		break;
#endif
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		return	0;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void OnIDLE(HWND hWnd)
{
	static DWORD Time = GetTickCount();
	DWORD	NewTime = GetTickCount();
	if(NewTime > Time)
	{
		Time = NewTime + (1000 / 30);
		InvalidateRect(hWnd, NULL, false);

		if( g_PProcess.GetPopPatchDlg() && g_PProcess.GetPopPatchDlg()->IsShow() )
		{
			InvalidateRect(g_PProcess.GetPopPatchDlg()->GetHandle(), NULL, false);
		}
	}
}
//-----------------------------------------------------------------------------
//  End
//-----------------------------------------------------------------------------
