#include	"defines.h"
#include	"ListControl.h"

CListControl::CListControl(HWND Parent, DWORD nControlID) 
: m_nTap(0), m_nCol(0), m_bAsc(true), m_nSortItem(0)
{
	m_hWnd	= GetDlgItem(Parent, nControlID);
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_FULLROWSELECT);// | LVS_EX_INFOTIP);
}
CListControl::~CListControl(void)
{
	m_hWnd = NULL;
}

void CListControl::CutTextByKey(std::wstring const &kOrg, std::wstring const &kKey, std::list<std::wstring> &rOut)
{
	std::wstring::size_type start_pos = 0;
	while(start_pos != std::wstring::npos)
	{
		std::wstring::size_type const find_pos = kOrg.find( kKey.c_str(), start_pos);

		std::wstring strCut;
		if(find_pos != std::wstring::npos)
		{
			strCut = kOrg.substr(start_pos, find_pos - start_pos);
			start_pos = find_pos+kKey.length();
		}
		else
		{
			strCut = kOrg.substr(start_pos);
			start_pos = std::wstring::npos;
		}

		if(!strCut.empty())
		{
			rOut.push_back(strCut);
		}
	}
}

bool CListControl::SetColumn(const std::wstring& wstrKeyArray)
{
	//기본 인덱스
	int			nCount	= 0;

	// 사용자 정의
	CUT_STRING		kColList;
	CutTextByKey(wstrKeyArray, L"@", kColList);

	if(!kColList.size()) { return	false; }

	CUT_STRING::iterator	iter = kColList.begin();
	while(iter != kColList.end())
	{
		CUT_STRING	kAttrList;
		CutTextByKey((*iter), L"|", kAttrList);
		
		if(!kAttrList.size()) { continue; }

		CUT_STRING::iterator	Attr_iter = kAttrList.begin();
		
		LVCOLUMN	lvColumn;

		lvColumn.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt		= LVCFMT_LEFT;
		lvColumn.pszText	= (LPWSTR)(*Attr_iter++).c_str();
		lvColumn.cx			= _wtoi((*Attr_iter).c_str());
		lvColumn.iSubItem	= nCount;
		ListView_InsertColumn(m_hWnd, nCount++, &lvColumn);

		++m_nTap;
		++iter;
	}

	return	true;
}

void CListControl::Clear()
{
	ListView_DeleteAllItems(m_hWnd);
	m_nCol = 0;
}

// Index @Name ...
bool CListControl::AddItem(const std::wstring& wstrItemAttr, DWORD dwFlag)
{
	CUT_STRING		kAttrList;
	int				nCount = 0;
	wchar_t			szTemp[MAX_PATH] = {0, };
	CutTextByKey(wstrItemAttr, L"@", kAttrList);

	if(kAttrList.size() < (m_nTap - 1)) { return	false; }

	CUT_STRING::iterator	iter = kAttrList.begin();

	if(iter == kAttrList.end())
	{
		return	false;
	}

	LVITEM	lvItem;
	lvItem.mask		= LVIF_TEXT;
	lvItem.iItem	= m_nCol;
	lvItem.iSubItem	= nCount++;
	lvItem.pszText	= (LPWSTR)iter->c_str();
	lvItem.cchTextMax	= iter++->size();
	ListView_InsertItem(m_hWnd, &lvItem);

	while(iter != kAttrList.end())
	{
		ListView_SetItemText(m_hWnd, m_nCol, nCount++, (LPWSTR)(*iter).c_str());
		++iter;
	}

	switch(dwFlag) 
	{
	case CHK_LOG:
		{
			
		}break;
	}

	++m_nCol;
	return	true;
}

bool CListControl::DelItem(int nIndex)
{
	if(nIndex > m_nCol) { return	false; }

	if(ListView_DeleteItem(m_hWnd, nIndex))
	{
		--m_nCol;
		return	true;
	}

	return	false;
}

int CListControl::GetItemIndex()
{
	return	ListView_GetNextItem(m_hWnd, -1, LVNI_ALL | LVNI_SELECTED);
}

void CListControl::operator = (const CListControl& rhs)
{
	m_hWnd = rhs.GetHandle();
	m_nTap = rhs.GetTapCnt();
	m_nCol = rhs.GetTapCol();
}

void CListControl::NotifyMsg(UINT Flag, LPARAM lParam)
{
	LPNMITEMACTIVATE	Hla = (LPNMITEMACTIVATE)lParam;

	switch(Flag)
	{
	case LVN_ITEMCHANGED:
		{
		}break;
	case LVN_COLUMNCLICK:
		{
		}break;
	}
}