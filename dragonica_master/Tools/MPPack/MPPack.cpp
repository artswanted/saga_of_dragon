// MPPack.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MPPack.h"
#include "BM/vstring.h"
#include "MainWorker.h"

HWND		g_hWnd;
HINSTANCE	g_hInst;
MainWorker*	g_kMainWorker;

LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK	MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void SetLocation();

int APIENTRY wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPWSTR lpszCmdParam,int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpszCmdParam);

	BM::g_bCalcSHA2CheckSum = true;
	BM::g_bUseFormattedFile = true;

	SetLocation();
	g_kMainWorker = new MainWorker;
	if( g_kMainWorker )
	{
		if( g_kMainWorker->Initialize(lpszCmdParam) )
		{
			g_hInst = hInstance;
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_MPPACK), g_hWnd, MainDlgProc);
		}
		delete g_kMainWorker;
	}

	return 0;
}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			{
				SetClassLong(hDlg, GCL_HICON, (LONG)LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_MPPACK)));
				if( !g_kMainWorker ){ return TRUE; }
				g_kMainWorker->SethWnd(hDlg);
				if( !g_kMainWorker->SetIniData() )
				{ 
					EndDialog(hDlg, 0); 
				}
				g_kMainWorker->SetSelectedJob(EPT_NONE);
			}return TRUE;
		case WM_COMMAND:
			{
				if( !g_kMainWorker ){ return TRUE; }
				g_kMainWorker->Input(wParam, lParam);
			}break;
	}
	return FALSE;
}

void SetLocation()
{
	wchar_t szPath[MAX_PATH] = {0,};

	GetModuleFileNameW(GetModuleHandleW(0), szPath, MAX_PATH);

	if (wcslen(szPath)==0)
	{
		return;
	}

	wcsrchr(szPath, L'\\' )[1] = 0;
	SetCurrentDirectoryW(szPath);
}