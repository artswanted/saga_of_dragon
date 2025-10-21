#include "stdafx.h"
#include "XUI_Config.h"
#include "XUI_Button.h"
using namespace XUI;

CXUI_Button::CXUI_Button(void) 
:	m_bUseTwinkleImg(false)
,	m_kFontColorByState(0xFFFFFFFF)
,	m_kBtnDownIdx(3)
{
	BtnImg(NULL);
	BtnImgIdx(-1);
	UVInfo(SUVInfo(2,2,1));//기본 2,2

	BtnIdx(1);
	EnableChangeFontColor(true);
	InvertBold(false);
	FontColor(m_kFontColorByState.Get(0));
	FontColorType(0);
	BtnImgVertical(false);
	BtnSingleImage(false);
}

CXUI_Button::~CXUI_Button(void)
{
	ButtonRealeaseImage();
}

void CXUI_Button::ButtonRealeaseImage()
{
	void *pImg = BtnImg();
	if( pImg )
	{
		m_spRscMgr->ReleaseRsc(pImg);
		BtnImgIdx(-2);
	}
}

void CXUI_Button::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_BTN_IMG == wstrName )
	{//	RscGuid( wstrValue );
		BtnImgName(wstrValue);
		BtnImg( m_spRscMgr->GetRsc( vValue ) );
	}
	else if( ATTR_BTN_IMG_W == wstrName )
	{	
		BtnImgSize( POINT2((int)vValue, BtnImgSize().y) );	
	}
	else if( ATTR_BTN_IMG_H == wstrName )
	{
		BtnImgSize( POINT2(BtnImgSize().x, (int)vValue) );
	}
	else if( ATTR_BTN_UP_TEXT_COLOR == wstrName )
	{
		m_kFontColorByState.Set( 0, static_cast<DWORD>(vValue) );
		m_kFontColorByState.Set( 4, static_cast<DWORD>(vValue) );
		m_kFontColorByState.Set( 5, static_cast<DWORD>(vValue) );
	}
	else if( ATTR_BTN_SINGLE_IMAGE == wstrName ) {  BtnSingleImage((0 == (int)vValue)? false: true); }
	else if( ATTR_BTN_OVER_TEXT_COLOR == wstrName ){ m_kFontColorByState.Set( 1, static_cast<DWORD>(vValue) ); }
	else if( ATTR_BTN_DOWN_TEXT_COLOR == wstrName ){ m_kFontColorByState.Set( 2, static_cast<DWORD>(vValue) ); }
	else if( ATTR_BTN_DIS_TEXT_COLOR == wstrName ){ m_kFontColorByState.Set( 3, static_cast<DWORD>(vValue) ); }
	else if(ATTR_BTN_ENABLE_CHANGE_COLOR == wstrName)
	{
		bool bVal = (0 == (int)vValue)? false: true;
		EnableChangeFontColor(bVal);
	}
	else if(ATTR_BTN_T_INVERTBOLD == wstrName)
	{
		bool bVal = (0 == (int)vValue)? false: true;
		InvertBold(bVal);
	}
	else if(ATTR_BTN_T_COLOR_TYPE == wstrName)
	{
		FontColorType((int)vValue);
	}
	else if(ATTR_BTN_VERTICAL_IMG == wstrName)
	{
		BtnImgVertical((0 == (int)vValue)? false: true);
		if( true == BtnImgVertical() )
		{
			SUVInfo kUVInfo( 1,4,1 );//세로 배열
			if ( true == m_bUseTwinkleImg )
			{
				kUVInfo.V = 6;
			}
			UVInfo( kUVInfo );
		}
	}
	else if ( ATTR_BTN_USE_TWINKLEIMG == wstrName )
	{
		m_bUseTwinkleImg = ( 0==(int)vValue ? false : true );
		if ( true == m_bUseTwinkleImg )
		{
			SUVInfo kUVInfo( 2,3,1 );
			if ( true == BtnImgVertical() )
			{
				kUVInfo.U = 1;
				kUVInfo.V = 6;
			}
			UVInfo( kUVInfo );
		}
	}
	else if( ATTR_BTN_DOWN_IDX == wstrName )
	{
		m_kBtnDownIdx = vValue;
	}
}
void CXUI_Button::IsMouseDown(bool const& value)
{
	CXUI_Wnd::IsMouseDown(value);

}
bool CXUI_Button::VDisplay()
{
	//int const iOldIndex = BtnIdx();
	//int iIndex = 1;
	//if(IsMouseDown()){iIndex = 3;}
	//else if(IsMouseOver()){iIndex = 2;}//마우스 이벤트는 영역 체크 필수.

	//if( Disable() ){iIndex = 4;}

	////DWORD dwTemp = FontFlag();
	////if(3==iIndex && InvertBold())//Bold 설정 뒤집기
	////{
	////	FontFlag(FontFlag()^XTF_OUTLINE);
	////}

	//if( iIndex != iOldIndex )
	//{
	//	if( EnableChangeFontColor() )
	//	{
	//		FontColor( m_dwFontColorByState[iIndex-1] );
	//	}

	//	Invalidate(true);

	//	BtnIdx(iIndex);
	//}

	if(!CXUI_Wnd::VDisplay() ){return false;}

	//if(3==iIndex && InvertBold())//원래상태로
	//{
	//	FontFlag(dwTemp);
	//}

	int	iIndex = BtnIdx();

	void *pImg = BtnImg();
	if( pImg && m_spRenderer )
	{
		SRenderInfo kRenderInfo;

		kRenderInfo.bGrayScale = GrayScale();

		SSizedScale &rSS = kRenderInfo.kSizedScale;
		rSS.ptSrcSize = BtnImgSize();
		rSS.ptDrawSize = BtnSingleImage() ?
			BtnImgSize() :
			BtnImgSize()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V);//이미지를 2,2 로 나누어놓았다.

		SUVInfo kInfo = UVInfo();
		kInfo.Index = iIndex;

		kRenderInfo.kUVInfo = kInfo;
		kRenderInfo.kLoc = TotalLocation();
		GetParentDrawRect(kRenderInfo.rcDrawable);
		kRenderInfo.fAlpha = Alpha();
		kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();

		m_spRenderer->RenderSprite( pImg, m_BtnImgIdx, kRenderInfo);
	}
	return true;
}

bool CXUI_Button::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() || !Enable() )
	{
		IsMouseDown(false);
		return false;
	}//자식컨트롤도 안그림
	if( VPeekEvent_Default(rET, rPT, dwValue) )
	{
//		BtnIdx(1);
//		if(IsMouseDown()){BtnIdx(3);}
//		else if(IsMouseOver()){BtnIdx(2);}//마우스 이벤트는 영역 체크 필수.

//		if( Disable() ){BtnIdx(4);}
		return true;
	}

//	BtnIdx(1);
//	if(IsMouseDown()){BtnIdx(3);}
//	else if(IsMouseOver()){BtnIdx(2);}//마우스 이벤트는 영역 체크 필수.

//	if( Disable() ){BtnIdx(4);}

	return false;
}

bool CXUI_Button::VOnTick( DWORD const dwCurTime )
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
	}
	//자식컨트롤도 안그림
	
	if ( !CXUI_Wnd::VOnTick( dwCurTime ) )
	{
		return false;
	}

	if( !CheckVisible() )
	{
		return false;
	}

	if (BtnSingleImage())
	{
		return true;
	}

	int iIndex = 1;
	if( !Enable() )
	{
		iIndex = 4;
	}
	else
	{
		if( CXUI_Wnd::IsMouseDown() && BtnDownIdx()>0 )
		{
			iIndex = BtnDownIdx();
		}
		else if( IsMouseOver() )
		{
			iIndex = 2;
		}
		else if ( true == IsTwinkle() )
		{
			if ( true == m_bUseTwinkleImg )
			{
				iIndex = ( (true == NowTwinkleOn()) ? 6 : 5 ); 
			}
		}
	}

	//DWORD dwTemp = FontFlag();
	//if(3==iIndex && InvertBold())//Bold 설정 뒤집기
	//{
	//	FontFlag(FontFlag()^XTF_OUTLINE);
	//}

	if( iIndex != BtnIdx() )
	{
		if( EnableChangeFontColor() )
		{
			FontColor( m_kFontColorByState.Get( iIndex-1 ) );
		}

		Invalidate(true);

		BtnIdx(iIndex);
	}
	return true;
}

void CXUI_Button::VOnClose()
{
	CXUI_Wnd::VOnClose();

	IsMouseDown(false);			//버튼이 사라지고 나서 다시 불렀을 때 DOWN상태인것을 방어
}
void CXUI_Button::Disable(bool const& value)
{
	m_bEnable = !value;

	int	iIndex = 1;

	if( !Enable() )
	{
		iIndex = 4;
	}

}
void CXUI_Button::ButtonChangeImage(std::wstring const& wstrImgName, int iWidth, int iHeight)
{
	ButtonRealeaseImage();//해제

	BtnImgName(wstrImgName);//새로운 이미지로 교체
	BtnImg( m_spRscMgr->GetRsc( wstrImgName ) );

	bool bDefaultSize = false;
	if(0>=iWidth && 0>=iHeight) bDefaultSize = true;

	if(!bDefaultSize)
	{//새로운 이미지 크기(이미지 크기가 달라지면 Control 크기도 나중에 같이 달라져야 한다)
		BtnImgSize( POINT2(iWidth, iHeight) );
	}
	else
	{//이전 사이즈
	}
}

void CXUI_Button::VInit()
{
	CXUI_Wnd::VInit();

	for (size_t i = 0; i < BUTTON_STATE_NUM; ++i)
	{
		m_kFontColorByState.Set( i, m_spConfig->GetBtnTextColor(static_cast<int>(i)) );
	}

	if( EnableChangeFontColor() )
	{
		if (FontColorType())
		{
			for (size_t i = 0; i < BUTTON_STATE_NUM; ++i)
			{
				m_kFontColorByState.Set( i, m_spConfig->GetBtnTextColorByType(FontColorType(), static_cast<int>(i)) );
			}
		}
		FontColor(m_kFontColorByState.Get(0));
	}
}

void CXUI_Button::operator = ( const CXUI_Button &rhs)
{
	CXUI_Wnd::operator =(rhs);

	BtnImgName( rhs.BtnImgName());
	BtnImg(m_spRscMgr->GetRsc( BtnImgName()));
	BtnImgIdx(rhs.BtnImgIdx());
	BtnImgSize(rhs.BtnImgSize());
	BtnIdx(rhs.BtnIdx());
	EnableChangeFontColor(rhs.EnableChangeFontColor());
	InvertBold(rhs.InvertBold());
	FontColorType(rhs.FontColorType());
	m_bUseTwinkleImg = rhs.m_bUseTwinkleImg;
	m_kFontColorByState = rhs.m_kFontColorByState;
}
