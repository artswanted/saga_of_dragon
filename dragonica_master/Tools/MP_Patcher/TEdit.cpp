#include "TEdit.h"

CTEdit::CTEdit(void)
	: m_hParent(NULL)
	, m_hParentBitmap(NULL)
{
}

CTEdit::~CTEdit(void)
{
}

void CTEdit::Init(HINSTANCE Inst, HWND Parent, SEditInfo const& kInfo, CHBitmap* pBitmap)
{
	m_hParent = Parent;
	m_hParentBitmap = pBitmap;
	m_kEditInfo = kInfo;

	DWORD dwFlag = WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | ES_AUTOHSCROLL;

	m_hWnd = ::CreateWindow(L"edit", L"", dwFlag,
		m_kEditInfo.kPos.x, m_kEditInfo.kPos.y, m_kEditInfo.kSize.x, m_kEditInfo.kSize.y, Parent, (HMENU)m_kEditInfo.iID, Inst, NULL);

	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
								DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
								DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");

	SendMessage(m_hWnd, WM_SETFONT, (WPARAM)NewFont, (LPARAM)TRUE);
	SendMessage(m_hWnd, EM_LIMITTEXT, (WPARAM)m_kEditInfo.iLimitLen, 0);

	m_kEditProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)CTEdit::EditProc);
}

void CTEdit::Draw(HDC hDC)
{
	if( !m_hParentBitmap ){ return; }

	HDC	MemDC = CreateCompatibleDC(hDC);
	HBITMAP NewBit = CreateCompatibleBitmap(hDC, m_kEditInfo.kSize.x, m_kEditInfo.kSize.y);
	HBITMAP OldBit = static_cast<HBITMAP>(SelectObject(MemDC, NewBit));

	if( m_hParentBitmap )
	{
		RECT rcRect = { m_kEditInfo.kPos.x, m_kEditInfo.kPos.y, m_kEditInfo.kSize.x, m_kEditInfo.kSize.y };
		m_hParentBitmap->DrawBitmap(MemDC, 0, 0, rcRect, SRCCOPY);
	}

	BitBlt(hDC, 0, 0, m_kEditInfo.kSize.x, m_kEditInfo.kSize.y, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBit);
	DeleteObject(NewBit);
	DeleteDC(MemDC);
}

std::wstring const CTEdit::GetText()
{
	int iSize = ::GetWindowTextLength(m_hWnd);
	if( iSize && m_kEditInfo.iLimitLen >= iSize )
	{
		std::wstring kStr;
		kStr.resize(m_kEditInfo.iLimitLen);
		GetWindowText(m_hWnd, &kStr.at(0), m_kEditInfo.iLimitLen);
		return kStr;
	}
	return std::wstring();
}

//-----------------------------------------------------------------------------
//  Edit SubClassing Procedure
//-----------------------------------------------------------------------------
LRESULT	CALLBACK CTEdit::EditProc(HWND hWnd, UINT uiMsg,WPARAM wParam, LPARAM lParam)
{
	CTEdit* pkEdit = g_PProcess.GetEdit( hWnd );
	if( !pkEdit )
	{
		return FALSE;
	}

	switch(uiMsg)
	{
	case WM_ERASEBKGND:
		{
			HDC	hDC = GetDC(hWnd);
			pkEdit->Draw(hDC);
			ReleaseDC(hWnd, hDC);
		}break;
	case WM_KEYDOWN:
		{
			switch(LOWORD(wParam))
			{
			case VK_TAB:
				{
					CTEdit* pkNextEdit = g_PProcess.GetNextEdit( hWnd );
					if( pkNextEdit )
					{
						SetFocus(pkNextEdit->GetHandle());
					}
				}return TRUE;
			case VK_RETURN:
				{
					if( CTEdit::EET_PASSWORD == pkEdit->GetEditInfo().iType )
					{
						SendMessage(g_WinApp->GetHandle(), WM_COMMAND, MAKEWPARAM(g_PProcess.GetButtonID(CCTButton::eBTN_START), 0), 0);
					}
				}return TRUE;
			case VK_ESCAPE:
				{
				}break;
			}
		}break;
	}

	return CallWindowProc( pkEdit->GetDefaultProc(), hWnd, uiMsg, wParam, lParam );
}
