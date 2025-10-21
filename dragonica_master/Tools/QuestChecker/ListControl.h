#pragma once

class CListControl
{
private:
	typedef std::list<std::wstring>		CUT_STRING;

	HWND		m_hWnd;
	int			m_nTap;
	int			m_nCol;

public:
	bool	SetColumn(std::wstring const& wstrKeyArray);
	void	Clear();
	bool	AddItem(std::wstring const& wstrItemAttr, DWORD dwFlag = CHK_LOG);
	bool	DelItem(int nIndex);
	int		GetItemIndex();
	void	NotifyMsg(UINT Flag, LPARAM lParam);	
	
	void operator = (const CListControl& rhs);

	HWND	GetHandle() const	{ return	m_hWnd; }
	int		GetTapCnt()	const	{ return	m_nTap; }
	int		GetTapCol() const	{ return	m_nCol; }

	explicit CListControl(HWND Parent, DWORD nControlID);
	virtual ~CListControl(void);
};
