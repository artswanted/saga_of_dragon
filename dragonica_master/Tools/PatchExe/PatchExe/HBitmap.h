#pragma once

#include	<windows.h>
#include	<vector>
#include	<string>

class CHBitmap
{
protected:
	HBITMAP	m_hBitmap;
	int		m_iWidth;
	int		m_iHeight;

public:
	bool	LoadBitmapToDefault(std::wstring const& FileName);
	bool	LoadBitmapToCustum(std::wstring const& FileName);
	bool	LoadBitmapToMemory(std::vector<char> const& kData);
	bool	LoadToResource(HINSTANCE hInst, UINT ID);

	void	DrawBitmap(HDC hDC, int iX, int iY, UINT BltFlag);
	void	DrawBitmap(HDC hDC, int iX, int iY, float fPercent, UINT BltFlag);

	int	GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }

	void SetWidth(int iWidth) { m_iWidth = iWidth; }
	void SetHeight(int iHeight) { m_iHeight = iHeight; }

	CHBitmap(void);
	virtual ~CHBitmap(void);
};
