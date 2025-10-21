#pragma once

#include "HBitmap.h"

class CCTButton : public CHBitmap
{
private:
	HWND	m_hParent;
	HWND	m_hWnd;
	int		m_iCtlID;
	POINT	m_PtPos;
	std::wstring	m_wstrBtnText;

public:
	void	Init(HINSTANCE Inst, HWND Parent, int CtlID, POINT ptPos, std::wstring BtnText, std::vector<char> const& kData);
	void	Init(HINSTANCE Inst, HWND Parent, int CtlID, RECT rtPos, std::wstring BtnText, HINSTANCE hInst, UINT ID);
	void	OwnerDraw(LPDRAWITEMSTRUCT lpDIS);
	
	bool	IsMyID(int ID) { return (m_iCtlID == ID)?(true):(false); }
	
	CCTButton(void);
	virtual ~CCTButton(void);
};
