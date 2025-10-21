#include "stdafx.h"
#include "TextViewer.h"

#include "CheckMgr.h"

BOOL CALLBACK TViewerProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		{
			g_Core.TTVWMInit(hDlg);
		}return TRUE;
	case WM_NOTIFY:
		{
			//	여기서 툴팁표시 명령을 가로체서 텍스트 박스에 출력하도록 하자
			LPNMHDR				hdr;
			LPNMLVGETINFOTIP	ngt;
			LPNMLISTVIEW		nlv;
			wchar_t				szTxtID[MAX_PATH] = {0,};
			hdr	= (LPNMHDR)lParam;
			nlv = (LPNMLISTVIEW)lParam;
			ngt = (LPNMLVGETINFOTIP)lParam;
			const HWND	hList =  GetDlgItem(hDlg, IDC_LIST_TEXTTABLE);
			if(hdr->hwndFrom == hList)
			{
				switch(hdr->code)
				{
				case LVN_ITEMCHANGED:
					{
						ListView_GetItemText(hList, nlv->iItem, 1, szTxtID, MAX_PATH);

						const int iTxtID = _wtoi(szTxtID);
						std::wstring	Txt = g_kCheckMgr.GetTT(iTxtID).c_str();
						g_kCheckMgr.ChangeEnterMark(Txt);
						SetWindowText(GetDlgItem(hDlg, IDC_SELECTTEXT), Txt.c_str());
					}break;
				//case LVN_GETINFOTIP:
				//	{
				//		ListView_GetItemText(hList, ngt->iItem, 1, szTxtID, MAX_PATH);

				//		const int iTxtID = _wtoi(szTxtID);
				//		std::wstring	Txt = g_kCheckMgr.GetQText(iTxtID).c_str();
				//		g_kCheckMgr.ChangeEnterMark(Txt);
				//		SetWindowText(GetDlgItem(hDlg, IDC_SELECTTEXT), Txt.c_str());
				//	}break;
				}
			}
		}break;
	case WM_COMMAND:
		{
			if(!g_Core.TTVWMCommand(hDlg, wParam, lParam))
			{	
				return	false;
			}
		}return	TRUE;
	}

	return FALSE;
}
