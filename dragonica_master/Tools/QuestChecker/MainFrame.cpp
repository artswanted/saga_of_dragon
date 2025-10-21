//-----------------------------------------------------------------------------
//	API - Dialog Programing Base Window Framework 					
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "CheckMgr.h"
#include "ItemBag.h"

//-----------------------------------------------------------------------------
//  Area Variable
//-----------------------------------------------------------------------------
HWND		g_hWnd, g_hDlgWnd, g_hLogWnd;
HINSTANCE	g_hInst;
HANDLE		g_hThread = NULL;

CQCCore		g_Core;
//-----------------------------------------------------------------------------
//  Define Function Proto Type 
//-----------------------------------------------------------------------------
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);	//윈도우 프로시저
BOOL	CALLBACK	MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL	CALLBACK	LogProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void	SetLocation();

//-----------------------------------------------------------------------------
//  Window Main Function
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	g_hInst = hInstance;
	::DialogBox(hInstance, MAKEINTRESOURCE(IDD_QUESTCHECKER), g_hWnd, MainDlgProc);	

	if( g_hThread )
	{
		CloseHandle( g_hThread );
		g_hThread = NULL;
	}

	g_Core.End();

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
			g_hLogWnd	= ::CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_LOGWINDOW), g_hDlgWnd, LogProc);

			::SendMessage(GetDlgItem(hDlg, IDC_RADIO_DB), BM_SETCHECK, BST_CHECKED, 0);
			::SetWindowText(GetDlgItem(hDlg, IDC_MAX_KILLCOUNT_EDIT), L"1000");
			::EnableWindow(GetDlgItem(hDlg, IDC_BTN_START), FALSE);
			ItemBag::CreateItemBagDlg(g_hDlgWnd, FALSE);

			SetLocation();				//	Work Directory Setting
			RECT	rcPos;
			::GetWindowRect(hDlg, &rcPos);
			::ShowWindow(g_hLogWnd, SW_SHOW);
			::SetWindowPos(g_hLogWnd, NULL, 300, 300, 0, 0, SWP_NOSIZE);
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
				return false;
			}
		}return	TRUE;
	}
	return FALSE;
}

BOOL CALLBACK LogProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int const& iGab = 15;
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			RECT	rcRect;
			::GetClientRect(hDlg, &rcRect);
			::SetWindowPos(GetDlgItem(hDlg, IDC_LIST_LOG), NULL, 0, iGab, rcRect.right, rcRect.bottom - iGab, NULL);
			::SetWindowPos(GetDlgItem(hDlg, IDC_LIST_ERROR), NULL, 0, iGab, rcRect.right, rcRect.bottom - iGab, NULL);
			::SendMessage(GetDlgItem(hDlg, IDC_RDO_ALL), BM_SETCHECK, BST_CHECKED, 0);
			::ShowWindow(GetDlgItem(hDlg, IDC_LIST_LOG), SW_SHOW);
			::ShowWindow(GetDlgItem(hDlg, IDC_LIST_ERROR), SW_HIDE);
		}return TRUE;
	case WM_NOTIFY:
		{
		}return TRUE;
	case WM_SIZE:
		{
			RECT	rcRect;
			GetClientRect(hDlg, &rcRect);
			SetWindowPos(GetDlgItem(hDlg, IDC_LIST_LOG), NULL, 0, iGab, rcRect.right, rcRect.bottom - iGab, NULL);
			SetWindowPos(GetDlgItem(hDlg, IDC_LIST_ERROR), NULL, 0, iGab, rcRect.right, rcRect.bottom - iGab, NULL);
		}return TRUE;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case	IDC_RDO_ALL:
				{
					ShowWindow(GetDlgItem(hDlg, IDC_LIST_LOG), SW_SHOW);
					ShowWindow(GetDlgItem(hDlg, IDC_LIST_ERROR), SW_HIDE);
				}break;
			case	IDC_RDO_ERROR:
				{
					ShowWindow(GetDlgItem(hDlg, IDC_LIST_LOG), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_LIST_ERROR), SW_SHOW);
				}break;
			}
		}return	TRUE;
	}

	return FALSE;
}
//-----------------------------------------------------------------------------
//  End
//-----------------------------------------------------------------------------
