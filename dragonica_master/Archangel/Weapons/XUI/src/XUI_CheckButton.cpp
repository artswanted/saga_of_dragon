#include "stdafx.h"
#include "XUI_CheckButton.h"

using namespace XUI;

CXUI_CheckButton::CXUI_CheckButton() 
	:	m_bCheck(false), 
		m_bClickLock(false), 
		m_iIndex(1)
{
//	UVInfo(SUVInfo(2,2,1));//기본 2,2 -> 버튼 상속이라.. 필요없다.
}

CXUI_CheckButton::~CXUI_CheckButton()
{
}

void CXUI_CheckButton::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Button::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if(ATTR_BTN_CLICK_LOCK == wstrName)
	{
		bool bVal = (0!=(int)vValue)? true: false;
		ClickLock(bVal);
	}
}

bool CXUI_CheckButton::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}//자식도 안함.
	if( !Enable() ){return false;}

	if(VPeekEvent_Default(rET, rPT, dwValue))
	{
		if(ClickLock())
		{
			return true;
		}

		if(!DBLClick())
		{
			if(rET == IEI_MS_DOWN)
			{
				if(ContainsPoint(m_sMousePos))//마우스 이벤트는 영역 체크 필수.
				{
					if(MEI_BTN_0 == dwValue)
					{
						Check(!Check());
					}
				}
			}

			if(CXUI_Wnd::IsMouseDown() && Check() ){BtnIdx(3);}
			else if(IsMouseOver()){BtnIdx(2);}//마우스 이벤트는 영역 체크 필수.

			if( !Enable() ){BtnIdx(4);}
		}
		return true;
	}

	return false;
}
void CXUI_CheckButton::Check(bool const& value)
{
	m_bCheck = value;

	int	iOldIndex = m_iIndex;

	if(Check())
	{
		m_iIndex = 3;
	}
	else
	{
		m_iIndex = 1;
	}

	if( !Enable() )
	{
		m_iIndex = 4;
	}

	if( EnableChangeFontColor() )
	{
		FontColor( m_kFontColorByState.Get( m_iIndex - 1 ) );
	}

	if(InvertBold())
	{
		if(iOldIndex == 3 && m_iIndex != 3)
		{
			FontFlag(FontFlag()^XTF_OUTLINE);
		}
		if(iOldIndex !=3 && m_iIndex == 3)
		{
			FontFlag(FontFlag()^XTF_OUTLINE);
		}
	}
}

void CXUI_CheckButton::Disable(bool const& value)
{
	m_bEnable = !value;

	int	iOldIndex = m_iIndex;

	if(Check())
	{
		m_iIndex = 3;
	}
	else
	{
		m_iIndex = 1;
	}

	if( !Enable() )
	{
		m_iIndex = 4;
	}

	if(InvertBold())
	{
		if(iOldIndex == 3 && m_iIndex != 3)
		{
			FontFlag(FontFlag()^XTF_OUTLINE);
		}
		if(iOldIndex !=3 && m_iIndex == 3)
		{
			FontFlag(FontFlag()^XTF_OUTLINE);
		}
	}

	if( EnableChangeFontColor() )
	{
		FontColor( m_kFontColorByState.Get( m_iIndex-1 ) );
	}
}
bool CXUI_CheckButton::VOnTick( DWORD const dwCurTime )
{
	if( !Visible() || IsClosed() )	
	{
		if(IsMouseOver())
		{
			if(CXUI_Wnd::m_spWndMouseOver == this)
			{
				CXUI_Wnd::m_spWndMouseOver = NULL;
			}
		}
		IsMouseDown(false);
		return false;
	}//자식컨트롤도 안그림

	return CXUI_Wnd::VOnTick( dwCurTime );
}
bool CXUI_CheckButton::VDisplay()
{
	//int const iOldIndex = m_iIndex; //BtnIdx();

	//DWORD dwTemp = FontFlag();
	//if(3==m_iIndex && InvertBold())//Outline 설정 뒤집기
	//{
	//	FontFlag(FontFlag()^XTF_OUTLINE);
	//}

	//if( iOldIndex != m_iIndex )
	//{
	//	if( EnableChangeFontColor() )
	//	{
	//		FontColor( m_dwFontColorByState[m_iIndex-1] );
	//	}

	//	Invalidate(true);
	//}

	if(!CXUI_Wnd::VDisplay() ){return false;}

	//if(3==m_iIndex && InvertBold())//원래상태로
	//{
	//	FontFlag(dwTemp);
	//}

	void *pImg = BtnImg();
	if( pImg && m_spRenderer )
	{
		SRenderInfo kRenderInfo;

		kRenderInfo.bGrayScale = GrayScale();

		SSizedScale &rSS = kRenderInfo.kSizedScale;

		rSS.ptSrcSize = BtnImgSize();
		rSS.ptDrawSize = BtnImgSize()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V);//이미지를 2,2 로 나누어놓았다.

		kRenderInfo.kUVInfo = UVInfo();
		kRenderInfo.kUVInfo.Index = m_iIndex;

		kRenderInfo.kLoc = TotalLocation();
		GetParentDrawRect(kRenderInfo.rcDrawable);
		kRenderInfo.fAlpha = Alpha();

		if(m_dwColor != m_dwPastColor)
		{
			ColorChange(true);
			m_dwPastColor = m_dwColor;
			kRenderInfo.dwColor = m_dwColor;
		}
		else
		{
			ColorChange(false);
		}
		if(ColorChange())
		{
			kRenderInfo.bColorChange = ColorChange();
		}

		kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
	
		m_spRenderer->RenderSprite( pImg, m_ImgIdx, kRenderInfo);
	}
	return true;
}

void CXUI_CheckButton::operator = ( const CXUI_CheckButton &rhs)
{
	CXUI_Button::operator =(rhs);

	Check(rhs.Check());
	ClickLock(rhs.ClickLock());
}