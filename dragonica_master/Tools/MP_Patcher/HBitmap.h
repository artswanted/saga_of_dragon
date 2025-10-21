#pragma once

#include	<windows.h>
#include	<vector>
#include	<string>

class CHBitmap
{
public:
	CHBitmap(void);
	virtual ~CHBitmap(void);

	bool LoadBitmapToDefault(std::wstring const& FileName);
	bool LoadBitmapToCustum(std::wstring const& FileName);
	bool LoadBitmapToMemory(std::vector<char> const& kData);
	bool LoadToResource(HINSTANCE hInst, UINT ID);

	void DrawBitmap(HDC hDC, int iX, int iY, UINT BltFlag, bool Transparent = false) const;	 
	void DrawBitmap(HDC hDC, int iX, int iY, float fPercent, UINT BltFlag) const;
	void DrawBitmap(HDC hDC, UINT BltFlag) const;
	void DrawBitmap(HDC hDC, float fPercent, UINT BltFlag) const;
	void DrawBitmap(HDC hDC, int iX, int iY, RECT srcRect, UINT BltFlag, bool Transparent = false) const;

	int	GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }	

	void SetPos(POINT const& kPos) { m_kPos = kPos;}
	POINT const& GetPos() const { return m_kPos;}

	bool bInitialize() const { return (m_hBitmap)?(true):(false); };

protected:
	HBITMAP	m_hBitmap;
	int		m_iWidth;
	int		m_iHeight;
	POINT   m_kPos;
};
