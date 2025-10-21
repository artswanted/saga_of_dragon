#include "CTray.h"
#include "CTButton.h"

namespace CTray
{
static NOTIFYICONDATA	niData;

void Minimize()
{
	if (niData.uID != TRAYICONID) // already exist
	{
		ZeroMemory(&niData,sizeof(NOTIFYICONDATA));
		niData.hWnd = g_WinApp->GetHandle();
		niData.uID = TRAYICONID;
		niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		niData.hIcon = LoadIcon(g_WinApp->GetInstance(), MAKEINTRESOURCE(IDR_MAINFRAME));
		niData.uCallbackMessage = SWM_TRAYMSG;
		lstrcpyn(niData.szTip, _T("Dragonica Mercy"), sizeof(niData.szTip) / sizeof(TCHAR));
	}
    Shell_NotifyIcon(NIM_ADD, &niData);
    ShowWindow(g_WinApp->GetHandle(), SW_HIDE);
}

void Close()
{
	Shell_NotifyIcon(NIM_DELETE, &niData);
	ShowWindow(g_WinApp->GetHandle(), SW_RESTORE);
}

static void ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if(hMenu)
	{
		if( !IsWindowVisible(hWnd) )
			InsertMenu(hMenu, -1, MF_BYPOSITION, CCTButton::eTRAY_SHOW_WND, _T("Show"));
		InsertMenu(hMenu, -1, MF_BYPOSITION, CCTButton::eTRAY_EXIT_APP, _T("Exit"));

		// note:	must set window to the foreground or the
		//			menu won't disappear when it should
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN,
			pt.x, pt.y, 0, hWnd, NULL );
		DestroyMenu(hMenu);
	}
}

void WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		{
            Close();
        }
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		{
            ShowContextMenu(hWnd);
        }
        break;
	}
}

}
