#include "CTButton.h"

CCTButton::CCTButton(void)
:	CHBitmap()
,	m_hParent(NULL)
,	m_hWnd(NULL)
,	m_iCtlID(0)
,	m_kiButtonState(eNORMAL_SHAPE)
,	m_wstrBtnText()
,	m_kBtnTextFlag(DT_CENTER | DT_VCENTER)
,	m_iShapeCnt(0)
,	m_iType(0)
,	m_bVisible(true)
,	m_kLink()
,	m_hParentBitmap(NULL)
{
}

CCTButton::~CCTButton(void)
{
	::DestroyWindow(m_hWnd);
}

void CCTButton::Init(HINSTANCE Inst, HWND Parent, int CtlID, int iType, POINT const& kPos, 
					 int iShapeCnt, int iInitShape, std::wstring const BtnText, std::wstring const kLink, 
					 std::vector<char> const& kData)
{// 버튼 등록
	m_wstrBtnText = BtnText;
	m_hParent = Parent;	
	m_iCtlID = CtlID;
	m_iType = iType;
	m_iShapeCnt = iShapeCnt;		// 이미지 하나에 표현된 버튼이 총 몇개인가
	m_kiButtonState = iInitShape;	// 보여질 버튼 모습
	m_kLink = kLink;

	m_hWnd = ::CreateWindow(L"button", L"",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
		0, 0, 10, 10, Parent, (HMENU)CtlID, Inst, NULL);

	LoadBitmapToMemory(kData);
	SetPos(kPos);
	int const iBtnHgt = BtnHgt();
	::SetWindowPos(m_hWnd, NULL, kPos.x, kPos.y, m_iWidth, iBtnHgt, 0);
}

bool CCTButton::CheckMouseOver(int iX, int iY)
{
	int const kiOldButtonState = m_kiButtonState;
	//if( (m_iCtlID == CPatchProcess::BTM_01 || m_iCtlID == CPatchProcess::BTM_02) && g_PProcess.GetProcess() != CPatchProcess::eProcess_Preparing )	
	if(CCTButton::eBTN_START == GetType())
	{// 스타트 버튼이고
		if(CPatchProcess::eProcess_Preparing != g_PProcess.GetProcess())
		{// 패치 완료가 아니면 
			m_kiButtonState = eDEACT_SHAPE;	// 비활성화 표시
			if (kiOldButtonState != m_kiButtonState)
			{
				DrawButtonImg();
			}
			return false;
		}
	}	
	
	if(m_kiButtonState != eCLICKL_SHAPE)
	{
	 	if(m_kPos.x<=iX && (m_kPos.x+m_iWidth)>=iX && m_kPos.y<=iY && (m_kPos.y+(m_iHeight/m_iShapeCnt))>=iY)
		{// X축에 속하고 Y축에 속하면
			m_kiButtonState = eOVER_SHAPE;	// 마우스 오버 상태로 변경	
		}
		else
		{// 마우스가 버튼 영역에 없으면
			if(eNORMAL_SHAPE != m_kiButtonState)
			{// 기본상태로 변경
				m_kiButtonState = eNORMAL_SHAPE;
			}
		}
	}

	if( eOVER_SHAPE != m_kiButtonState )
	{
		bool bSelectedButton = g_kSiteControl1.IsSelectedSiteButton(m_iCtlID) || g_kSiteControl2.IsSelectedSiteButton(m_iCtlID);
		if( bSelectedButton )
		{
			m_kiButtonState = eCLICKL_SHAPE;
		}
		else
		{
			m_kiButtonState = eNORMAL_SHAPE;
		}
	}

	if(kiOldButtonState != m_kiButtonState)
	{// 버튼 상태가 변경 되었다면 버튼을 Draw
		DrawButtonImg();
		return true;
	}
	return false;
}

void	CCTButton::DrawButtonImg()
{
	HDC hDC = GetDC(m_hWnd);
	HDC	MemDC = ::CreateCompatibleDC(hDC);
	HBITMAP	NewBitmap = ::CreateCompatibleBitmap(hDC, m_iWidth, m_iHeight);
	HBITMAP OldBitmap = static_cast<HBITMAP>(::SelectObject(MemDC, NewBitmap));
	HFONT	NewFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
		DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
	HFONT	OldFont = static_cast<HFONT>(::SelectObject(MemDC, NewFont));
	::SetBkMode(MemDC, TRANSPARENT);

	if( m_kiButtonState == eCLICKL_SHAPE )
	{
		::SetTextColor(MemDC, RGB(125, 125, 125));
	}
	
	int const iBtnHgt = BtnHgt();
	int const iSrcHgt = m_kiButtonState*iBtnHgt;

	RECT	rcDraw = { 0, 0, m_iWidth, BtnHgt() };
	::OffsetRect(&rcDraw, 0, iSrcHgt);

	if( m_hParentBitmap)
	{
		RECT	rcRect;
		GetWindowRect(m_hWnd, &rcRect);
		ScreenToClient(m_hParent, (LPPOINT)&rcRect.left);
		ScreenToClient(m_hParent, (LPPOINT)&rcRect.right);
		m_hParentBitmap->DrawBitmap(MemDC, 0, iSrcHgt, rcRect, SRCCOPY);
	}
	DrawBitmap(MemDC, 0, 0, NONE_USE_COLORKEY, true);
	::DrawText(MemDC, m_wstrBtnText.c_str(), m_wstrBtnText.size(), &rcDraw, DT_SINGLELINE | m_kBtnTextFlag);
	BitBlt(hDC, 0, 0, m_iWidth, iBtnHgt, MemDC, 0, iSrcHgt, SRCCOPY);
	
	::SelectObject(MemDC, OldFont);
	::DeleteObject(NewFont);
	::SelectObject(MemDC, OldBitmap);
	::DeleteDC(MemDC);
	ReleaseDC(m_hWnd, hDC);
}

void	CCTButton::OwnerDraw(LPDRAWITEMSTRUCT lpDIS)
{
	if( lpDIS->CtlID != m_iCtlID )
	{
		return;
	}
	
	if((GetType() == CCTButton::eBTN_START) && g_PProcess.GetProcess() != CPatchProcess::eProcess_Preparing )
	{
		m_kiButtonState=eDEACT_SHAPE;		
	}
	else
	{
		if( lpDIS->itemState & ODS_SELECTED )
		{
			m_kiButtonState = eCLICKL_SHAPE;
		}
		else
		{
			m_kiButtonState = eNORMAL_SHAPE;
		}
	}
	DrawButtonImg();
}

int const CCTButton::BtnHgt() const 
{
	if(m_iHeight <= 0)
	{
		return 0;
	}
	return m_iHeight/m_iShapeCnt; 
}

bool CCTButton::OpenLink() const
{
	if(m_kLink.empty())
	{
		return false;
	}
	if( g_WinApp )
	{
		ShellExecute(g_WinApp->GetHandle(), WSTR_OPEN, m_kLink.c_str(), _T(""), _T(""), SW_SHOWNORMAL);
	}
	return true;	
}

void CCTButton::SetLoc(POINT const& Pt)
{
	m_kPos = Pt;
	SetWindowPos(m_hWnd, NULL, Pt.x, Pt.y, 0, 0, SWP_NOSIZE);
}

void CCTButton::Visible(bool IsVisible)
{
	m_bVisible = IsVisible; 
	ShowWindow(m_hWnd, (m_bVisible)?(SW_SHOW):(SW_HIDE));
};