#include "stdafx.h"
#include "CTButton.h"

CCTButton::CCTButton(void)
	: m_hParent(NULL)
	, m_hWnd(NULL)
	, m_iCtlID(0)
	, m_wstrBtnText(L"")
{
	m_PtPos.x = 0;
	m_PtPos.y = 0;
}

CCTButton::~CCTButton(void)
{
}

void	CCTButton::Init(HINSTANCE Inst, HWND Parent, int CtlID, POINT ptPos, std::wstring BtnText, std::vector<char> const& kData)
{
	m_wstrBtnText = BtnText;
	m_hParent = Parent;
	m_PtPos	= ptPos;
	m_iCtlID = CtlID;

	m_hWnd = ::CreateWindow(L"button", L"",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
		0, 0, 10, 10, Parent, (HMENU)CtlID, Inst, NULL);

	LoadBitmapToMemory(kData);

	::SetWindowPos(m_hWnd, NULL, m_PtPos.x, m_PtPos.y, m_iWidth, m_iHeight, 0);
}

void	CCTButton::Init(HINSTANCE Inst, HWND Parent, int CtlID, RECT rtPos, std::wstring BtnText, HINSTANCE hInst, UINT ID)
{
	m_wstrBtnText = BtnText;
	m_hParent = Parent;
	m_PtPos.x = rtPos.left;
	m_PtPos.y = rtPos.top;
	m_iWidth = rtPos.right;
	m_iHeight = rtPos.bottom;
	m_iCtlID = CtlID;

	m_hWnd = ::CreateWindow(L"button", L"",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
		0, 0, 10, 10, Parent, (HMENU)CtlID, Inst, NULL);

	LoadToResource(hInst, ID);

	::SetWindowPos(m_hWnd, NULL, m_PtPos.x, m_PtPos.y, m_iWidth, m_iHeight, 0);
}

void	CCTButton::OwnerDraw(LPDRAWITEMSTRUCT lpDIS)
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
	::DrawText(MemDC, m_wstrBtnText.c_str(), m_wstrBtnText.size(), &rcDraw, DT_CENTER | DT_VCENTER);
	::BitBlt(lpDIS->hDC, 0, 0, m_iWidth, m_iHeight, MemDC, 0, 0, SRCCOPY);

	::SelectObject(MemDC, OldFont);
	::DeleteObject(NewFont);
	::SelectObject(MemDC, OldBitmap);
	::DeleteDC(MemDC);
}