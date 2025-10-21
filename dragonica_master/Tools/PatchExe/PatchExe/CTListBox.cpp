#include "stdafx.h"
#include "CTListBox.h"

CCTListBox::CCTListBox(void)
	: m_hParent(NULL)
	, m_hWnd(NULL)
	, m_iCtlID(0)
{
	m_rtPos.left = 0;
	m_rtPos.top = 0;
	m_rtPos.right = 0;
	m_rtPos.bottom = 0;
}

CCTListBox::~CCTListBox(void)
{
}

void	CCTListBox::Init(HINSTANCE Inst, HWND Parent, int CtlID, RECT rtPos)
{
	m_hParent = Parent;
	m_rtPos	= rtPos;
	m_iCtlID = CtlID;

	m_hWnd = ::CreateWindow(L"listbox", L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY,
		m_rtPos.left, m_rtPos.top, m_rtPos.right, m_rtPos.bottom, Parent, (HMENU)CtlID, Inst, NULL);

	//LoadBitmapToMemory(kData);

	//::SetWindowPos(m_hWnd, NULL, m_PtPos.x, m_PtPos.y, m_iWidth, m_iHeight, 0);
}

void	CCTListBox::OwnerDraw(LPDRAWITEMSTRUCT lpDIS)
{
	if( lpDIS->CtlID != m_iCtlID )
	{
		return;
	}

	HDC	MemDC = ::CreateCompatibleDC(lpDIS->hDC);
	HBITMAP	NewBitmap = ::CreateCompatibleBitmap(lpDIS->hDC, m_iWidth, m_iHeight);
	HBITMAP OldBitmap = static_cast<HBITMAP>(::SelectObject(MemDC, NewBitmap));
	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
								DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
								DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
	HFONT	OldFont = static_cast<HFONT>(::SelectObject(MemDC, NewFont));
	::SetBkMode(MemDC, TRANSPARENT);

	if( lpDIS->itemState & ODS_SELECTED )
	{
		::SetTextColor(MemDC, RGB(125, 125, 125));
		DrawBitmap(MemDC, 0, 0, SRCCOPY);
	}
	else
	{
		DrawBitmap(MemDC, 0, 0, SRCCOPY);
	}


	RECT	rcDraw = { 0, 3, m_iWidth, m_iHeight };
	::BitBlt(lpDIS->hDC, 0, 0, m_iWidth, m_iHeight, MemDC, 0, 0, SRCCOPY);

	::SelectObject(MemDC, OldFont);
	::DeleteObject(NewFont);
	::SelectObject(MemDC, OldBitmap);
	::DeleteDC(MemDC);
}