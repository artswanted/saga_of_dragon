#include "stdafx.h"
#include <tchar.h>
#include "XUI_Config.h"
#include "XUI_ToolTip.h"
#include "BM/vstring.h"

using namespace XUI;

//#define XUI_USE_OFFSCREEN
#define MAX_TOOLTIP_WIDTH	512
#define MAX_TOOLTIP_HEIGHT	512

CXUI_ToolTip::CXUI_ToolTip(void)
{
	m_pToolTipImgBody = NULL;
	m_pToolTipImgTB = NULL;
	m_pToolTipImgLR = NULL;
	m_pToolTipImgLock = NULL;
	m_pToolTipImgClose = NULL;
	m_pToolTipImgCorner = NULL;
	m_pImgStatic = NULL;
	Location(0,0);
	PastSize(POINT2(0,0));
	ReservedClose(false);

	m_kOutlineSize = POINT2(12,12);
}

CXUI_ToolTip::~CXUI_ToolTip(void)
{
	if( m_pToolTipImgBody ){m_spRscMgr->ReleaseRsc(m_pToolTipImgBody);}
	if( m_pToolTipImgTB ){m_spRscMgr->ReleaseRsc(m_pToolTipImgTB);}
	if( m_pToolTipImgLR ){m_spRscMgr->ReleaseRsc(m_pToolTipImgLR);}
	if( m_pToolTipImgLock ){m_spRscMgr->ReleaseRsc(m_pToolTipImgLock);}
	if( m_pToolTipImgClose ){m_spRscMgr->ReleaseRsc(m_pToolTipImgClose);}
	if( m_pToolTipImgCorner ){m_spRscMgr->ReleaseRsc(m_pToolTipImgCorner);}
	if (m_pImgStatic) { m_spRscMgr->ReleaseRsc(m_pImgStatic); }
}

void CXUI_ToolTip::VInit()
{
	CXUI_Wnd::VInit();
	UseOffscreen(false);	// 따로 Offscreen을 쓴다.
	
	m_pToolTipImgBody	= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgBody());
	m_pToolTipImgTB		= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgTB());
	m_pToolTipImgLR		= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgLR());
	m_pToolTipImgLock	= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgLock());
	m_pToolTipImgClose	= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgClose());
	m_pToolTipImgCorner	= m_spRscMgr->GetRsc(m_spConfig->ToolTipImgCorner());

	m_iToolTipImgBody= -1;
	m_iToolTipImgT= -1;
	m_iToolTipImgB= -1;
	m_iToolTipImgL= -1;
	m_iToolTipImgR= -1;
	m_iToolTipImgLock= -1;
	m_iToolTipImgClose= -1;

	m_iToolTipImgCorner1= -1;
	m_iToolTipImgCorner2= -1;
	m_iToolTipImgCorner3= -1;
	m_iToolTipImgCorner4= -1;
}

bool CXUI_ToolTip::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick(dwCurTime) )
	{
		return false;
	}

	if (ReservedClose())
	{
		ReservedClose(false);
		CXUI_Wnd::Close();
	}
	return true;
}

void CXUI_ToolTip::Close()
{
	ReservedClose(true);
}

void CXUI_ToolTip::VOnCall()
{
	if (ReservedClose())
	{
		CXUI_Wnd::Close();		
		ReservedClose(false);
	}
	CXUI_Wnd::VOnCall();
}

bool CXUI_ToolTip::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}//자식도 안함.
	if( !Enable() ){return false;}
	if( ReservedClose() ) {return false;}

	//모든 컨트롤에 이벤트메시지 처리 권한 부여
	CONT_CONTROL::iterator kItor = m_contControls.begin();
	while(m_contControls.end() != kItor)
	{
		if( (*kItor)->VPeekEvent(rET, rPT, dwValue) )
		{
			return true;
		}
		++kItor;
	}

	return VPeekEvent_Default(rET, rPT, dwValue);
}

void CXUI_ToolTip::VRefresh()
{
	CXUI_Wnd::VRefresh();
	if (m_pImgStatic) { m_spRscMgr->ReleaseRsc(m_pImgStatic); }
}

void CXUI_ToolTip::ReloadText()
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("ReloadText"), CXUI_Manager::GetFrameCount()));
	if( m_bUpdatedText )
	{
		SAFE_DELETE(m_pText);
		m_bUpdatedText = false;
	}

	int	iCropWidth = Size().x - m_kOutlineSize.x*2;

	TextPos(POINT2(0,0));
	POINT2 const ptTextRect = AdjustText(Font(), m_Text, m_kStyleText,iCropWidth);
	TextRect(ptTextRect);

	TextPos(m_kOutlineSize);
	Size(Size().x,ptTextRect.y+m_kOutlineSize.y*2);

	SetInvalidate();
}

bool CXUI_ToolTip::VDisplay()
{
	if(!CXUI_Wnd::VDisplay() ){return false;}
	if( ReservedClose() ) {return false;}

	SRenderInfo kRenderInfo;
	kRenderInfo.bGrayScale = GrayScale();
	kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
	SSizedScale &rSS = kRenderInfo.kSizedScale;

	POINT2 ptRetRect( Size().x, __max( Size().y , TextRect().y ) );
	if (TextRect().y != PastSize().y)
	{
		Invalidate(true);
		m_Size.y = ptRetRect.y;

		PastSize(TextRect());
	}

	kRenderInfo.fAlpha = Alpha();
	GetParentDrawRect(kRenderInfo.rcDrawable);
	POINT3I ptOrigin = TotalLocation();
	POINT3I ptLoc = ptOrigin;

#ifdef XUI_USE_OFFSCREEN
	if (m_pImgStatic && Invalidate() == false)
	{
		int iSrc = 0;
		rSS.ptSrcSize.x = Width() + 24;
		rSS.ptSrcSize.y = Height() + 24;
		rSS.ptDrawSize = rSS.ptSrcSize;
		kRenderInfo.kUVInfo = UVInfo();
		kRenderInfo.kLoc.x = ptLoc.x - 12 ;
		kRenderInfo.kLoc.y = ptLoc.y - 12 ;
		m_spRenderer->RenderSprite(m_pImgStatic, iSrc, kRenderInfo);
	}
	else
#endif	
	{
		bool bInsertList = true;
#ifdef XUI_USE_OFFSCREEN
		if (m_pImgStatic == NULL || Invalidate())
		{
			POINT2 ptScreenSize;
			ptScreenSize.x = MAX_TOOLTIP_WIDTH;
			ptScreenSize.y = MAX_TOOLTIP_HEIGHT;
			if (m_pImgStatic == NULL)	// 있을 경우에는 재사용 한다.
			{
				if (NotUseOffscreen() == false)
				{
					m_pImgStatic = m_spRscMgr->VCreateOffscreen(ptScreenSize, 0, m_dwColor, AlphaMax());	// StaticNum을 0을 주면 Sharing하지 않는다.
					if (m_pImgStatic == NULL)
					{
						// 오류 상황에 대비하자;
						NotUseOffscreen(true);
						UseOffscreen(false);
					}
				}
			}
			else
				m_spRenderer->InvalidateOffscreen(m_pImgStatic);

			if (m_pImgStatic)
			{
				ptOrigin = POINT3I(12, 12, ptLoc.z);
				kRenderInfo.fAlpha = AlphaMax();
				bInsertList = false;
			}
		}
#endif

		POINT2	kSize = Size();

		{//가운데.
			rSS.ptSrcSize = POINT2(32,32);//xxx todo 하드코딩
			rSS.ptDrawSize.x = kSize.x-m_kOutlineSize.x*2;
			rSS.ptDrawSize.y = kSize.y-m_kOutlineSize.y*2;

			kRenderInfo.kUVInfo = UVInfo();
			kRenderInfo.kLoc = ptOrigin + m_kOutlineSize;

			m_spRenderer->RenderSprite(m_pToolTipImgBody, m_iToolTipImgBody, kRenderInfo, bInsertList);
		}

		{//왼코너
			rSS.ptSrcSize = POINT2(24,24);//xxx todo 하드코딩
			rSS.ptDrawSize = m_kOutlineSize;

			kRenderInfo.kUVInfo = SUVInfo(2,2,1);

			ptLoc = ptOrigin;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgCorner, m_iToolTipImgCorner1, kRenderInfo, bInsertList);
		}

		{//우코너
			rSS.ptSrcSize = POINT2(24,24);//xxx todo 하드코딩
			rSS.ptDrawSize = m_kOutlineSize;

			kRenderInfo.kUVInfo = SUVInfo(2,2,2);
			
			ptLoc = ptOrigin;
			ptLoc.x += kSize.x - m_kOutlineSize.x;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgCorner, m_iToolTipImgCorner2, kRenderInfo, bInsertList);
		}

		{//좌하 코너
			rSS.ptSrcSize = POINT2(24,24);//xxx todo 하드코딩
			rSS.ptDrawSize = m_kOutlineSize;

			kRenderInfo.kUVInfo = SUVInfo(2,2,3);

			ptLoc = ptOrigin;
			ptLoc.y += kSize.y - m_kOutlineSize.y;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgCorner, m_iToolTipImgCorner3, kRenderInfo, bInsertList);
		}

		{//우하 코너
			rSS.ptSrcSize = POINT2(24,24);//xxx todo 하드코딩
			rSS.ptDrawSize = POINT2(12,12);

			kRenderInfo.kUVInfo = SUVInfo(2,2,4);
			
			ptLoc = ptOrigin;
			ptLoc.x += kSize.x - m_kOutlineSize.x;
			ptLoc.y += kSize.y - m_kOutlineSize.y;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgCorner, m_iToolTipImgCorner4, kRenderInfo, bInsertList);
		}

		{//윗줄채우기
			rSS.ptSrcSize = POINT2(12,24);//xxx todo 하드코딩
			rSS.ptDrawSize = POINT2(kSize.x-m_kOutlineSize.x*2,m_kOutlineSize.y);

			kRenderInfo.kUVInfo = SUVInfo(1,2,1);
			
			ptLoc = ptOrigin;
			ptLoc.x += m_kOutlineSize.x;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgTB, m_iToolTipImgT, kRenderInfo, bInsertList);
		}

		{//아랫줄채우기
			rSS.ptSrcSize = POINT2(12,24);//xxx todo 하드코딩
			rSS.ptDrawSize = POINT2(kSize.x-m_kOutlineSize.x*2,m_kOutlineSize.y);

			kRenderInfo.kUVInfo = SUVInfo(1,2,2);
			
			ptLoc = ptOrigin;
			ptLoc.x += m_kOutlineSize.x;
			ptLoc.y += kSize.y - m_kOutlineSize.y;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgTB, m_iToolTipImgB, kRenderInfo, bInsertList);
		}

		{//왼줄채우기
			rSS.ptSrcSize = POINT2(24,12);//xxx todo 하드코딩
			rSS.ptDrawSize = POINT2(m_kOutlineSize.x,kSize.y - m_kOutlineSize.y*2);

			kRenderInfo.kUVInfo = SUVInfo(2,1,1);
			
			ptLoc = ptOrigin;
			ptLoc.y += m_kOutlineSize.y;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgLR, m_iToolTipImgL, kRenderInfo, bInsertList);
		}

		{//오른줄채우기
			rSS.ptSrcSize = POINT2(24,12);//xxx todo 하드코딩
			rSS.ptDrawSize = POINT2(m_kOutlineSize.x,kSize.y - m_kOutlineSize.y*2);

			kRenderInfo.kUVInfo = SUVInfo(2,1,2);
			
			ptLoc = ptOrigin;
			ptLoc.x += kSize.x - m_kOutlineSize.x;
			ptLoc.y += m_kOutlineSize.y;
			kRenderInfo.kLoc = ptLoc;

			m_spRenderer->RenderSprite(m_pToolTipImgLR, m_iToolTipImgR, kRenderInfo, bInsertList);
		}
#ifdef XUI_USE_OFFSCREEN
		if (m_pImgStatic && bInsertList == false)
		{
			std::list<std::pair<void*,bool>> kOffscreenDrawList;
			kOffscreenDrawList.push_back(std::make_pair(m_pToolTipImgBody, Parent() == NULL));
			kOffscreenDrawList.push_back(std::make_pair(m_pToolTipImgCorner, Parent() == NULL));
			kOffscreenDrawList.push_back(std::make_pair(m_pToolTipImgTB, Parent() == NULL));
			kOffscreenDrawList.push_back(std::make_pair(m_pToolTipImgLR, Parent() == NULL));
			
			m_spRenderer->EndRenderOffscreen(m_pImgStatic, kOffscreenDrawList);
			kOffscreenDrawList.clear();
			Invalidate(false);

			m_UVInfo = SUVInfo(1, 1, 1);
			m_Size = POINT2(ptRetRect.x, ptRetRect.y);
			int iSrc = 0;
			rSS.ptSrcSize.x = Width() + 24;
			rSS.ptSrcSize.y = Height() + 24;
			rSS.ptDrawSize = rSS.ptSrcSize;
			kRenderInfo.kUVInfo = UVInfo();
			kRenderInfo.fAlpha = Alpha();
			kRenderInfo.kLoc.x = TotalLocation().x - 12 ;
			kRenderInfo.kLoc.y = TotalLocation().y - 12 ;
			m_spRenderer->RenderSprite(m_pImgStatic, iSrc, kRenderInfo);
		}
#endif
	}

	return true;
}