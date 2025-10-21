#pragma once

class List
{
public:
	typedef std::list<std::wstring>		CUT_STRING;

	virtual std::wstring const ColumnDesign() const = 0;		//@이름|사이즈
	virtual std::wstring const ColumnValueDesign() const = 0;	//@값

	bool Initialize();
	bool AddItem(std::wstring const& ItemValueString);
	int GetSelectedItemIndex();
	bool DelItem(int nIndex);
	void Clear();

	int	GetItemIndex();
	
	HWND GetHandle() const { return m_hHandle; }
	int	 GetTapCnt() const { return m_nTap; }
	int	 GetTapCol() const { return m_nCol; }

	virtual ~List(void);

protected:
	explicit List(HWND hParent, DWORD nControlID);

	HWND	m_hParent;
	HWND	m_hHandle;
	int		m_nTap;
	int		m_nCol;
	bool	m_bAsc;
	int		m_nSortItem;
};

class LogList : public List
{
public:
	virtual std::wstring const ColumnDesign() const { return std::wstring(L"@PROCESS|120@MESSAGE|490"); };
	virtual std::wstring const ColumnValueDesign() const { return std::wstring(L"@%s@%s"); };

	explicit LogList(HWND hParent, DWORD nControlID);
	virtual ~LogList(void);
};

class OptionList : public List
{
public:
	virtual std::wstring const ColumnDesign() const { return std::wstring(L"@OPTION|80"); };
	virtual std::wstring const ColumnValueDesign() const { return std::wstring(L"@%s"); };

	explicit OptionList(HWND hParent, DWORD nControlID);
	virtual ~OptionList(void);
};