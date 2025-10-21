//-----------------------------------------------------------------------------
//	API - Dialog Programing Base Window Framework 					
//-----------------------------------------------------------------------------
#include	"defines.h"

//-----------------------------------------------------------------------------
//  Area Variable
//-----------------------------------------------------------------------------
HWND		g_hWnd, g_hDlgWnd;	
HINSTANCE	g_hInst;			

CWCCore		g_Core;
//-----------------------------------------------------------------------------
//  Define Function Proto Type 
//-----------------------------------------------------------------------------
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);	//윈도우 프로시저
BOOL	CALLBACK	MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void	SetLocation();

//-----------------------------------------------------------------------------
//  Window Main Function
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	NiInit();	
	g_hInst = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_WORLDCHECKER), g_hWnd, MainDlgProc);
	NiShutdown();
	return 0;
}

//-----------------------------------------------------------------------------
//  08-01-24, Main Dialog Procedure
//-----------------------------------------------------------------------------
BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			g_hDlgWnd	= hDlg;
			SetLocation();				//	Work Directory Setting
			g_Core.Load();
		}return TRUE;
	case WM_NOTIFY:
		{
			g_Core.NotifyMsg(lParam);
		}return TRUE;
	case WM_COMMAND:
		{
			if(!g_Core.WMCommand(wParam, lParam))
			{	
				return	false;
			}
		}return	TRUE;
	}

	return FALSE;
}

//	Set Work Directory
void SetLocation()
{
	setlocale(LC_ALL, "kor");
	char path[MAX_PATH];

	GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);

	if (strlen(path)==0)
	{
		return;
	}

	strrchr( path, '\\' )[1] = 0;
	SetCurrentDirectoryA(path);
}
//-----------------------------------------------------------------------------
//  End
//-----------------------------------------------------------------------------
