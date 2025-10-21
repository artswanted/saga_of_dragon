#pragma once

#include "HBitmap.h"

class CCTListBox : public CHBitmap
{
private:
	HWND	m_hParent;
	HWND	m_hWnd;
	int		m_iCtlID;
	RECT	m_rtPos;

public:
	void	Init(HINSTANCE Inst, HWND Parent, int CtlID, RECT rtPos);
	void	OwnerDraw(LPDRAWITEMSTRUCT lpDIS);
	
	bool	IsMyID(int ID) { return (m_iCtlID == ID)?(true):(false); }

	HWND	GetWnd() { return m_hWnd; }
	
	CCTListBox(void);
	virtual ~CCTListBox(void);
};
