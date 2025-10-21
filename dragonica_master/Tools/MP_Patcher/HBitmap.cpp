#include "HBitmap.h"

CHBitmap::CHBitmap(void)
: m_hBitmap(NULL)
, m_iWidth(0)
, m_iHeight(0)
{
	m_kPos.x = 0;
	m_kPos.y = 0;
}

CHBitmap::~CHBitmap(void)
{
	if( m_hBitmap )
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

bool CHBitmap::LoadBitmapToDefault(std::wstring const& FileName)
{
	m_hBitmap = static_cast<HBITMAP>(::LoadImage(NULL, FileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	if( m_hBitmap )
	{
		BITMAP	Info;
		::GetObject(m_hBitmap, sizeof(BITMAP), (BITMAP*)&Info);
		m_iWidth = Info.bmWidth;
		m_iHeight = Info.bmHeight;

		return	true;
	}

	return	false;		
}

bool CHBitmap::LoadBitmapToCustum(std::wstring const& FileName)
{
	BITMAPFILEHEADER FileHeader;
	BITMAPINFO* BmInfo;
	DWORD dwRead;
	void* pData;

	HANDLE	hFile = CreateFile(FileName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	ReadFile(hFile, &FileHeader, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
	DWORD dwInfoSize = FileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	BmInfo = reinterpret_cast<BITMAPINFO*>(new BYTE[dwInfoSize]);
	ReadFile(hFile, BmInfo, dwInfoSize, &dwRead, NULL);

	m_iWidth = BmInfo->bmiHeader.biWidth;
	m_iHeight = BmInfo->bmiHeader.biHeight;
	m_hBitmap = CreateDIBSection(NULL, BmInfo, DIB_RGB_COLORS, &pData, NULL, 0);
	ReadFile(hFile, pData, BmInfo->bmiHeader.biSizeImage, &dwRead, NULL);

	delete [] BmInfo;
	CloseHandle(hFile);
	return true;	
}

bool CHBitmap::LoadBitmapToMemory(std::vector<char> const& kData)
{
	if( kData.size() )
	{
		BITMAPFILEHEADER FileHeader;
		BITMAPINFO* BmInfo;
		void* pData;

		::memcpy(&FileHeader, &kData.at(0), sizeof(BITMAPFILEHEADER));

		DWORD dwInfoSize = FileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
		BmInfo	= reinterpret_cast<BITMAPINFO*>(new BYTE[dwInfoSize]);
		::memcpy(BmInfo, &kData.at(sizeof(BITMAPFILEHEADER)), dwInfoSize);

		m_iWidth = BmInfo->bmiHeader.biWidth;
		m_iHeight = BmInfo->bmiHeader.biHeight;
		m_hBitmap = CreateDIBSection(NULL, BmInfo, DIB_RGB_COLORS, &pData, NULL, 0);
		::memcpy(pData, &kData.at(FileHeader.bfOffBits), BmInfo->bmiHeader.biSizeImage);

		delete [] BmInfo;
		return true;	
	}
	else
	{
		return false;
	}
}

bool CHBitmap::LoadToResource(HINSTANCE hInst, UINT ID)
{
	m_hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(ID));
	if( m_hBitmap )
	{
		return true;
	}

	return false;
}

void CHBitmap::DrawBitmap(HDC hDC, int iX, int iY, UINT BltFlag, bool Transparent) const
{
	HDC	MemDC = CreateCompatibleDC(NULL);
	HBITMAP OldBit = static_cast<HBITMAP>(::SelectObject(MemDC, m_hBitmap));
	if( Transparent )
	{
		TransparentBlt(hDC, iX, iY, m_iWidth, m_iHeight, MemDC, 0, 0, m_iWidth, m_iHeight, BltFlag);
	}
	else
	{
		BitBlt(hDC, iX, iY, m_iWidth, m_iHeight, MemDC, 0, 0, BltFlag);
	}
	SelectObject(MemDC, OldBit);
	DeleteDC(MemDC);
}

void CHBitmap::DrawBitmap(HDC hDC, int iX, int iY, float fPercent, UINT BltFlag) const
{
	int	iWidth = (int)(m_iWidth * fPercent);

	HDC	MemDC = CreateCompatibleDC(NULL);
	HBITMAP OldBit = static_cast<HBITMAP>(::SelectObject(MemDC, m_hBitmap));
	BitBlt(hDC, iX, iY, iWidth, m_iHeight, MemDC, 0, 0, BltFlag);
	SelectObject(MemDC, OldBit);
	DeleteDC(MemDC);
}

void CHBitmap::DrawBitmap(HDC hDC, UINT BltFlag) const
{
	DrawBitmap(hDC, m_kPos.x, m_kPos.y, BltFlag);
}

void CHBitmap::DrawBitmap(HDC hDC, float fPercent, UINT BltFlag) const
{
	DrawBitmap(hDC, m_kPos.x, m_kPos.y, fPercent, BltFlag);
}

void CHBitmap::DrawBitmap(HDC hDC, int iX, int iY, RECT srcRect, UINT BltFlag, bool Transparent) const
{
	int const iWidth = srcRect.right - srcRect.left;
	int const iHeight = srcRect.bottom - srcRect.top;

	HDC	MemDC = CreateCompatibleDC(NULL);
	HBITMAP OldBit = static_cast<HBITMAP>(::SelectObject(MemDC, m_hBitmap));
	if( Transparent )
	{
		TransparentBlt(hDC, iX, iY, iWidth, iHeight, MemDC, srcRect.left, srcRect.top, iWidth, iHeight, BltFlag);
	}
	else
	{
		StretchBlt(hDC, iX, iY, iWidth, iHeight, MemDC, srcRect.left, srcRect.top, iWidth, iHeight, BltFlag);
	}
	SelectObject(MemDC, OldBit);
	DeleteDC(MemDC);
}
