#include "stdafx.h"
#include "FolderDlg.h"

CString CFolderDlg::m_initialFolder;

CFolderDlg::CFolderDlg(BOOL HideNewFolderButton, const CString& InitialFolder, CWnd* pParent)
: m_parent(pParent), m_hideNewFolderButton(HideNewFolderButton)
{
	m_initialFolder = InitialFolder;
	if (m_initialFolder.IsEmpty())
	{
		TCHAR szDir[MAX_PATH];
		if (GetCurrentDirectory(sizeof(szDir) / sizeof(TCHAR), szDir))
		{
			m_initialFolder = szDir;
		}
	}
}

int CFolderDlg::DoModal()
{
	// reset
	m_selectedFolder = "";

	HRESULT result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	// check result
	if ((result == S_OK) || (result == S_FALSE))
	{
		// interface pointer to the memory allocator
		LPMALLOC pMalloc;

		if (CoGetMalloc(1, &pMalloc) == S_OK)
		{
			TCHAR buffer[MAX_PATH];

			// initialize the BROWSEINFO struct
			BROWSEINFO bi;
			bi.hwndOwner      = m_parent->GetSafeHwnd();
			bi.pidlRoot       = 0;
			bi.pszDisplayName = buffer;
			bi.lpszTitle      = m_title;
			bi.ulFlags        = (BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE);
			bi.lpfn           = BrowseCallbackProc;
			bi.lParam         = 0;

			// hide button if necessary
			if (m_hideNewFolderButton)
			{
				bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
			}

			LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);

			// cancel will return 0
			if (pidl != 0)
			{
				if (::SHGetPathFromIDList(pidl, buffer))
				{
					m_selectedFolder = buffer;
				}
				pMalloc->Free(pidl);
			}
			pMalloc->Release();
		}
		CoUninitialize();
	}
	return (m_selectedFolder.IsEmpty() ? IDCANCEL : IDOK);
}


void CFolderDlg::SetTitle(const CString& Title)
{
	m_title = Title;
}

CString CFolderDlg::GetFolderName() const
{
	return m_selectedFolder;
}

INT CALLBACK CFolderDlg::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg)
	{
		case BFFM_INITIALIZED:
			if (!m_initialFolder.IsEmpty())
			{
				// WParam is TRUE since you are passing a path
				// it would be FALSE if you were passing a pidl
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCWSTR)m_initialFolder);
			}
			break;

		case BFFM_SELCHANGED:
			// set the status window to the currently selected path
			if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
			{
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
			}
			break;
	}
	return 0;
}

