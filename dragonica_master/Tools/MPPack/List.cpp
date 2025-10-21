#include "StdAfx.h"
#include <commctrl.h>
#include "List.h"
#include "BM/vstring.h"

List::List(HWND hParent, DWORD nControlID) 
: m_hParent(hParent), m_nTap(0), m_nCol(0), m_bAsc(true), m_nSortItem(0)
{
	m_hHandle = GetDlgItem(m_hParent, nControlID);
	::SendMessage(m_hHandle, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);// | LVS_EX_INFOTIP);
}

List::~List(void)
{
	m_hHandle = NULL;
}

bool List::Initialize()
{
	int nCount = 0;

	CUT_STRING	kCutList;
	BM::vstring::CutTextByKey(ColumnDesign(), std::wstring(L"@"), kCutList);

	if(!kCutList.size()) { return false; }

	CUT_STRING::iterator	cul_iter = kCutList.begin();
	while(cul_iter != kCutList.end())
	{
		CUT_STRING::value_type const& kCulInfo = (*cul_iter);

		CUT_STRING	kAttrList;
		BM::vstring::CutTextByKey(kCulInfo, std::wstring(L"|"), kAttrList);
		
		CUT_STRING::iterator	Attr_iter = kAttrList.begin();
		if( Attr_iter != kAttrList.end() )
		{
			LVCOLUMN	lvColumn;

			CUT_STRING::value_type const& kName = (*Attr_iter);

			lvColumn.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvColumn.fmt		= LVCFMT_LEFT;
			lvColumn.pszText	= (LPWSTR)kName.c_str();

			++Attr_iter;

			if( Attr_iter != kAttrList.end() )
			{
				CUT_STRING::value_type const& kSize = (*Attr_iter);

				lvColumn.cx	= _wtoi(kSize.c_str());
			}
			else
			{
				lvColumn.cx	= 50;
			}

			lvColumn.iSubItem	= nCount++;
			::SendMessage(m_hHandle, LVM_INSERTCOLUMN, (WPARAM)(lvColumn.iSubItem), (LPARAM)&lvColumn);
		}
		else
		{
			return false;
		}

		++m_nTap;
		++cul_iter;
	}

	return	true;
}

bool List::AddItem(std::wstring const& ItemValueString)
{
	CUT_STRING		kAttrList;
	int				nCount = 0;
	wchar_t			szTemp[MAX_PATH] = {0, };
	BM::vstring::CutTextByKey(ItemValueString, std::wstring(L"@"), kAttrList);

	if(kAttrList.size() < (m_nTap - 1)) { return false; }

	CUT_STRING::iterator attr_iter = kAttrList.begin();
	if(attr_iter == kAttrList.end())
	{
		return false;
	}

	CUT_STRING::value_type const& kValue = (*attr_iter);

	LVITEM	lvItem;
	lvItem.mask		= LVIF_TEXT;
	lvItem.iItem	= m_nCol;
	lvItem.iSubItem	= nCount++;
	lvItem.pszText	= (LPWSTR)(kValue.c_str());
	lvItem.cchTextMax = kValue.size();

	::SendMessage(m_hHandle, LVM_INSERTITEM, 0, (LPARAM)(&lvItem));

	++attr_iter;
	while(attr_iter != kAttrList.end())
	{
		CUT_STRING::value_type const& kValue = (*attr_iter);

		LV_ITEM kItem;
		kItem.iSubItem = nCount++;
		kItem.pszText = (LPWSTR)(kValue.c_str());

		::SendMessage(m_hHandle, LVM_SETITEMTEXT, (WPARAM)(m_nCol), (LPARAM)(&kItem));

		++attr_iter;
	}

	++m_nCol;
	return	true;
}
int List::GetSelectedItemIndex()
{
	return ListView_GetNextItem(m_hHandle, -1, LVNI_FOCUSED | LVNI_SELECTED);
}

bool List::DelItem(int nIndex)
{
	if(nIndex > m_nCol) { return	false; }

	if( FALSE != (BOOL)(::SendMessage(m_hHandle, LVM_DELETEITEM, (WPARAM)nIndex, 0L)) )
	{
		--m_nCol;
		return	true;
	}

	return	false;
}

void List::Clear()
{
	::SendMessage(m_hHandle, LVM_DELETEALLITEMS, 0, 0L);
	m_nCol = 0;
}

int List::GetItemIndex()
{
	return static_cast<int>(::SendMessage(m_hHandle, LVM_GETNEXTITEM, static_cast<WPARAM>(-1), MAKELPARAM((LVNI_ALL|LVNI_SELECTED), 0)));
}

LogList::LogList(HWND hParent, DWORD nControlID) : List(hParent, nControlID)
{
}

LogList::~LogList(void)
{
}

OptionList::OptionList(HWND hParent, DWORD nControlID) : List(hParent, nControlID)
{
}

OptionList::~OptionList(void)
{
}