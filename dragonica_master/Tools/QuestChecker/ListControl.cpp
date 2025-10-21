#include "stdafx.h"
#include "ListControl.h"

CListControl::CListControl(HWND Parent, DWORD nControlID) 
: m_nTap(0), m_nCol(0)
{
	m_hWnd	= GetDlgItem(Parent, nControlID);
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_FULLROWSELECT);// | LVS_EX_INFOTIP);
}
CListControl::~CListControl(void)
{
	m_hWnd = NULL;
}

bool CListControl::SetColumn(std::wstring const& wstrKeyArray)
{
	//기본 인덱스
	int			nCount	= 0;

	// 사용자 정의
	CUT_STRING		kColList;
	BM::vstring::CutTextByKey(wstrKeyArray, std::wstring( L"@" ), kColList);

	if(!kColList.size()) { return	false; }

	CUT_STRING::iterator	iter = kColList.begin();
	while(iter != kColList.end())
	{
		CUT_STRING	kAttrList;
		BM::vstring::CutTextByKey((*iter), std::wstring( L"|" ), kAttrList);
		
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
bool CListControl::AddItem(std::wstring const& wstrItemAttr, DWORD dwFlag)
{
	CUT_STRING		kAttrList;
	int				nCount = 0;
	wchar_t			szTemp[MAX_PATH] = {0, };
	BM::vstring::CutTextByKey(wstrItemAttr, std::wstring( L"@" ), kAttrList);

	if(kAttrList.size() < (m_nTap - 1)) { return	false; }

	CUT_STRING::iterator	iter = kAttrList.begin();

	if(iter == kAttrList.end())
	{
		return	false;
	}

	LVITEM	lvItem;
	lvItem.mask			= LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem		= m_nCol;
	lvItem.lParam		= (LPARAM)((LPWSTR)iter->c_str());
	lvItem.iSubItem		= nCount++;
	lvItem.pszText		= (LPWSTR)iter->c_str();
	lvItem.cchTextMax	= static_cast<int>(iter++->size());	
	
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
	case CHK_NONITEM:
		{

		}break;
	case CHK_NONMONSTER:
		{

		}break;
	case CHK_SUCCESS:
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
