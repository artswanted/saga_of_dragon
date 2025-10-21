#include "stdafx.h"
#include "XUI_Wnd.h"
#include "XUI_Cursor.h"

using namespace XUI;


SIconInfo CXUI_Cursor::m_IconInfo;//이놈의 키는 몇번인가.

CXUI_Wnd* CXUI_Cursor::GetInstance()
{
	static CXUI_Cursor gs_XUICursor;
	return &gs_XUICursor;
//	return new CXUI_Cursor;
}

CXUI_Cursor::CXUI_Cursor(void)
{
	Priority(MAX_PRIORITY);
	ResetCursorState();
}

CXUI_Cursor::~CXUI_Cursor(void)
{
}

void CXUI_Cursor::ResetCursorState()
{
	CursorState(CS_NONE);
}

CXUI_Wnd* CXUI_Cursor::VCreate()const
{
//	return new CXUI_Cursor;
	return GetInstance();
}


/*! 
	\brief 커서 객체 초기화
    \param 
	\last update 2006.12.20 by 하승봉
*/
void CXUI_Cursor::VInit()
{
	CXUI_Wnd::VInit();
	m_ImgIdx = -1;

	POINT DesktopPt;
	GetCursorPos(&DesktopPt);
	
	RECT rectClient;
	GetClientRect(CXUI_Wnd::m_sHwnd, &rectClient);
	RECT rectWindow;
	GetWindowRect(CXUI_Wnd::m_sHwnd, &rectWindow);

	if(DesktopPt.x < rectWindow.left || DesktopPt.x > rectWindow.right || DesktopPt.y < rectWindow.top || DesktopPt.y > rectWindow.bottom)
	{
		m_bVisible = false;
	}

	RECT rect;	//윈도우 바, 스크롤, 메뉴 크기만큼 오차 수정
	rect.left = (rectWindow.right - rectWindow.left) - rectClient.right;
	rect.top = (rectWindow.bottom - rectWindow.top) - rectClient.bottom;
	m_sMousePos.Set(DesktopPt.x - rectWindow.left - rect.left, DesktopPt.y - rectWindow.top - rect.top, 0);
}

void CXUI_Cursor::ReleaseDefaultImg()
{
	void* pImg = DefaultImg();
	if (pImg && m_spRscMgr)
	{
		m_spRscMgr->ReleaseRsc(pImg);
		DefaultImg(NULL);
	}
}

bool CXUI_Cursor::VDisplay()
{
	if( !Visible() ){return false;}//자식컨트롤도 안그림	
	
	void *pImg = DefaultImg();
	if( pImg && m_spRenderer )
	{
		int iIndex =1;

		if(IsMouseDown())
		{
			iIndex = 2;
		}

		SRenderInfo kRenderInfo;

		kRenderInfo.bGrayScale = GrayScale();

		SSizedScale &rSS = kRenderInfo.kSizedScale;

		static POINT2 const ptSrcDiv(2,4);//그림이 2x2

		rSS.ptSrcSize = POINT2(32*ptSrcDiv.x,32*ptSrcDiv.y);
		rSS.ptDrawSize = POINT2(32,32);

		kRenderInfo.kUVInfo = SUVInfo(ptSrcDiv.x, ptSrcDiv.y, iIndex + (CursorState()*2));//그림이 2x2 로 나뉘어있고. 스테이트에 따라서 2씩(노말, 클릭시) 증가 하게되지.
		kRenderInfo.kLoc = CXUI_Wnd::m_sMousePos;
		GetParentDrawRect(kRenderInfo.rcDrawable);
		kRenderInfo.fAlpha = 1.0f;

		kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
	
		m_spRenderer->RenderSprite( pImg, m_ImgIdx, kRenderInfo);
	}

	DoScript( SCRIPT_ON_DISPLAY );	
//	if(!CXUI_Wnd::VDisplay()){return false;}//

	return true;
}

bool CXUI_Cursor::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
//	return CXUI_Wnd::VPeekEvent(rET, rPT, dwValue);
	switch( dwValue )
	{//놓을때
	case MEI_BTN_0 :
		{
			m_kBtn1State = rET;
			if(IEI_MS_DOWN == rET )	
			{
				IsMouseDown(true);
			}
			if(IEI_MS_UP == rET )	
			{
				IsMouseDown(false);
			}
		}break;
	case MEI_BTN_1:
		{
			m_kBtn2State = rET;
		}break;
	default://다른버튼은 응답 안함.
		{
			return false;
		}break;
	}

	switch(rET)
	{
	case IEI_MS_DOWN:
	case IEI_MS_UP:
		{
			if(m_pCursorCallBackFunc)
			{
				return m_pCursorCallBackFunc(this, m_sMousePos, m_IconInfo, m_kBtn1State, m_kBtn2State);
			}
		}break;
	}
	return false;
//	return false;
//	return CXUI_Wnd::VPeekEvent(rET, rPT, dwValue);
}
/*
bool CXUI_Cursor::GetValue(EValuetype const eVT, std::wstring &wstrOut)
{//커서도 고쳐줄것
	if( CXUI_Wnd::GetValue(eVT, wstrOut) )
	{
		return true;
	}

	switch( eVT )
	{
	case VT_ICON_GRP: //그룹이란게 안변해?? 
		{
			wstrOut = (std::wstring const)((BM::vstring)m_IconInfo.x);
			return true;
		}break;
	case VT_ICON_KEY:
		{
			wstrOut = (std::wstring const)((BM::vstring)m_IconInfo.y);
		}break;
	default:
		{
			return false;
		}break;
	}

	return false;
}
*/