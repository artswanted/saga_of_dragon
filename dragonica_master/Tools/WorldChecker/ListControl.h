#pragma once

typedef	struct	sSortRef
{
	HWND	hWnd;
	int		nStandard;
	bool	bAsc;
}SORTREF;

class CListControl
{
private:
	typedef std::list<std::wstring>		CUT_STRING;

	HWND		m_hWnd;
	int			m_nTap;
	int			m_nCol;
	bool		m_bAsc;
	int			m_nSortItem;

	static int CALLBACK	SortProc(LPARAM Dest, LPARAM Sour, LPARAM ref);

public:
	bool	SetColumn(const std::wstring& wstrKeyArray);

	void	Clear();
	bool	AddItem(const std::wstring& wstrItemAttr, DWORD dwFlag = CHK_LOG);
	bool	DelItem(int nIndex);
	int		GetItemIndex();

	void	NotifyMsg(UINT Flag, LPARAM lParam);
	
	void operator = (const CListControl& rhs);

	HWND	GetHandle() const	{ return	m_hWnd; }
	int		GetTapCnt()	const	{ return	m_nTap; }
	int		GetTapCol() const	{ return	m_nCol; }

	void	CutTextByKey(std::wstring const &kOrg, std::wstring const &kKey, std::list<std::wstring> &rOut);

	explicit CListControl(HWND Parent, DWORD nControlID);
	virtual ~CListControl(void);
};
