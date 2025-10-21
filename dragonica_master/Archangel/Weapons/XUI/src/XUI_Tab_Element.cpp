#include "stdafx.h"
#include "XUI_Tab_Element.h"
using namespace XUI;

CXUI_Tab_Element::CXUI_Tab_Element()
{
	TabImg(NULL);
	TabImgIdx(-1);
	UVInfo(SUVInfo(2,2,1));//기본 2,2
}

CXUI_Tab_Element::~CXUI_Tab_Element()
{
	void *pImg = TabImg();
	if(pImg)
	{
		m_spRscMgr->ReleaseRsc(pImg);
	}
}

void CXUI_Tab_Element::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_TAB_IMG == wstrName )
	{//	RscGuid( wstrValue );
		TabImg( m_spRscMgr->GetRsc( vValue ) );
	}
	else if( ATTR_TAB_IMG_W == wstrName ){	TabImgSize( POINT2((int)vValue, TabImgSize().y) );	}
	else if( ATTR_TAB_IMG_H == wstrName ){	TabImgSize( POINT2(TabImgSize().x, (int)vValue) );	}
}

bool CXUI_Tab_Element::VDisplay()
{
	if(!CXUI_Wnd::VDisplay() ){return false;}

	void *pImg = TabImg();
	if( pImg && m_spRenderer )
	{
		int index = 1;
		if( IsMouseDown() ){index = 3;}
		if(ContainsPoint( CXUI_Wnd::m_sMousePos ) ){index = 2;}

		SRenderInfo kRenderInfo;

		kRenderInfo.bGrayScale = GrayScale();
		SSizedScale &rSS = kRenderInfo.kSizedScale;
		
		rSS.ptSrcSize = TabImgSize();
		rSS.ptDrawSize = TabImgSize()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V);//이미지를 2,2 로 나누어놓았다.

		kRenderInfo.kUVInfo = UVInfo();
		kRenderInfo.kUVInfo.Index = index;
		
		kRenderInfo.kLoc = TotalLocation();
		GetParentDrawRect(kRenderInfo.rcDrawable);
		kRenderInfo.fAlpha = Alpha();
		kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
	
		m_spRenderer->RenderSprite( pImg, m_ImgIdx, kRenderInfo);
	}
	return true;
}

void CXUI_Tab_Element::VOnClose()
{
	CXUI_Wnd::VOnClose();
}