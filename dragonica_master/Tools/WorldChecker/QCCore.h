#pragma once

class CTBLoader;
class CListControl;
class CComboBox;
class CWCCore
{
public:
	CWCCore(void);
	virtual ~CWCCore(void);
	


	bool	Load();
	void	NotifyMsg(LPARAM lParam);
	bool	WMCommand(WPARAM wParam, LPARAM lParam);
	bool	AddListItem(const std::wstring& wstrItem, DWORD dwFlag = CHK_LOG);
	
	std::wstring		GetDevFolder();
	int					GetDevFolderLen();
	char*				GetErrorFileName();

private:
	bool	FolderSelect(const HWND& hWnd, std::wstring& wstrFolderName);
	bool    CreateList(CListControl*& pList, HWND hWnd, DWORD dwID, std::wstring const& ColumnType);
	bool    SetFolder();
	bool	SetDevFolder();
	bool	Reload();
	bool	CheckerInit();
	void	SelectWorkMsg(int iSelectValue);

private:
	std::wstring	m_TargetFolder;
	char			szErrorFileName[128];

	//	메인 다이얼 로그
	CListControl*	m_pkSystem;
	CListControl*	m_pkErrLog;
	CListControl*	m_pkLog;

	HWND			hComboBOX_SELECT;
	int				i_ComboBOX_SELECT_iNdex;
	std::wstring	m_StrComboBOX_SELECT;
};
