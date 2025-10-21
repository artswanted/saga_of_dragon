#pragma once

class CFolderDlg
{
public:
	CFolderDlg(BOOL HideNewFolderButton, const CString& InitialFolder, CWnd* pParent);
	virtual int DoModal();
	void SetTitle(const CString& Title);
	CString GetFolderName() const;


protected:
	static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData);


private:
	static CString m_initialFolder;
	BOOL m_hideNewFolderButton;
	CWnd* m_parent;
	CString m_title;
	CString m_selectedFolder;
};
