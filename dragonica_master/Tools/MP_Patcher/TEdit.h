#pragma once

#include "defines.h"

class CTEdit
{
public:
	enum eEditType
	{
		EET_NORMAL		= 0,
		EET_PASSWORD	= 1,
	}E_EDIT_TYPE;

	CTEdit(void);
	~CTEdit(void);

	void Init(HINSTANCE Inst, HWND Parent, SEditInfo const& kInfo, CHBitmap* pBitmap);
	void Draw(HDC hDC);
	std::wstring const GetText();

	WNDPROC GetDefaultProc() { return m_kEditProc; }
	HWND GetHandle() { return m_hWnd; }
	SEditInfo const& GetEditInfo() { return m_kEditInfo; }

	static LRESULT	CALLBACK EditProc(HWND hWnd, UINT uiMsg,WPARAM wParam, LPARAM lParam);

private:
	HWND	m_hParent;
	HWND	m_hWnd;

	SEditInfo	m_kEditInfo;
	CHBitmap*	m_hParentBitmap;
	bool	m_bVisible;

	WNDPROC	m_kEditProc;
};
