#include "stdafx.h"
#include "XUI_StaticForm.h"
#include "XUI_Config.h"
#include "BM/vstring.h"

using namespace XUI;

CXUI_StaticForm::CXUI_StaticForm(void)
{
	m_pImgBody = NULL;
	m_pImgTB = NULL;
	m_pImgLR = NULL;
	m_pImgCorner = NULL;
	m_pImgStatic = NULL;

	m_ptBody.x = 0;
	m_ptBody.y = 0;
	m_ptTB.x = 0;
	m_ptTB.y = 0;
	m_ptLR.x = 0;
	m_ptLR.y = 0;
	m_ptCorner.x = 0;
	m_ptCorner.y = 0;

	StaticNum(0);
	Location(0,0);
}

CXUI_StaticForm::~CXUI_StaticForm(void)
{
	ReleaseAllResource();
}
void	CXUI_StaticForm::ReleaseAllResource()
{
	if( m_pImgBody ){m_spRscMgr->ReleaseRsc(m_pImgBody);}
	if( m_pImgTB ){m_spRscMgr->ReleaseRsc(m_pImgTB);}
	if( m_pImgLR ){m_spRscMgr->ReleaseRsc(m_pImgLR);}
	if( m_pImgCorner ){m_spRscMgr->ReleaseRsc(m_pImgCorner);}
	if (m_pImgStatic) { m_spRscMgr->ReleaseRsc(m_pImgStatic); }

	m_pImgBody = 0;
	m_pImgTB = 0;
	m_pImgLR = 0;
	m_pImgCorner = 0;
	m_pImgStatic = 0;

}
void CXUI_StaticForm::VInit()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	if (NotUseOffscreen())
		UseOffscreen(false);
	else
		UseOffscreen(true);
#endif
	CXUI_Wnd::VInit();
	std::wstring const *pFileName = NULL;

	ReleaseAllResource();

	if (StaticNum() != 0)
	{
		if ( m_spConfig->StaticInfo(StaticNum(), STI_BODY, pFileName) )
		{
			m_pImgBody = m_spRscMgr->GetRsc(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_BODY_W, pFileName);
			m_ptBody.x = (int)BM::vstring(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_BODY_H, pFileName);
			m_ptBody.y = (int)BM::vstring(pFileName->c_str());
		}

		if ( m_spConfig->StaticInfo(StaticNum(), STI_TB, pFileName) )
		{
			m_pImgTB = m_spRscMgr->GetRsc(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_TB_W, pFileName);
			m_ptTB.x = (int)BM::vstring(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_TB_H, pFileName);
			m_ptTB.y = (int)BM::vstring(pFileName->c_str());
		}

		if ( m_spConfig->StaticInfo(StaticNum(), STI_LR, pFileName) )
		{
			m_pImgLR = m_spRscMgr->GetRsc(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_LR_W, pFileName);
			m_ptLR.x = (int)BM::vstring(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_LR_H, pFileName);
			m_ptLR.y = (int)BM::vstring(pFileName->c_str());
		}

		if ( m_spConfig->StaticInfo(StaticNum(), STI_CORNER, pFileName) )
		{
			m_pImgCorner = m_spRscMgr->GetRsc(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_CORNER_W, pFileName);
			m_ptCorner.x = (int)BM::vstring(pFileName->c_str());
			m_spConfig->StaticInfo(StaticNum(), STI_CORNER_H, pFileName);
			m_ptCorner.y = (int)BM::vstring(pFileName->c_str());
		}
	}
	else
	{
		m_pImgBody		= m_spRscMgr->GetRsc(m_wstrImgBody);
		m_pImgTB		= m_spRscMgr->GetRsc(m_wstrImgTB);
		m_pImgLR		= m_spRscMgr->GetRsc(m_wstrImgLR);
		m_pImgCorner	= m_spRscMgr->GetRsc(m_wstrImgCorner);
	}

	m_iImgBody= -1;
	m_iImgT= -1;
	m_iImgB= -1;
	m_iImgL= -1;
	m_iImgR= -1;

	m_iImgCorner1= -1;
	m_iImgCorner2= -1;
	m_iImgCorner3= -1;
	m_iImgCorner4= -1;
}

void CXUI_StaticForm::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_IMG_BODY == wstrName )
	{
		m_wstrImgBody = wstrValue;
	}
	else if( ATTR_IMG_TB == wstrName )
	{	
		m_wstrImgTB = wstrValue;
	}
	else if( ATTR_IMG_LR == wstrName )
	{	
		m_wstrImgLR = wstrValue;
	}
	else if( ATTR_IMG_CORNER == wstrName )
	{
		m_wstrImgCorner = wstrValue;
	}
	else if( ATTR_BODY_W == wstrName )
	{	
		m_ptBody.x = (int)vValue;
	}
	else if( ATTR_BODY_H == wstrName )
	{
		m_ptBody.y = (int)vValue;
	}
	else if( ATTR_TB_W == wstrName )
	{	
		m_ptTB.x = (int)vValue;
	}
	else if( ATTR_TB_H == wstrName )
	{	
		m_ptTB.y = (int)vValue;
	}
	else if( ATTR_LR_W == wstrName )
	{	
		m_ptLR.x = (int)vValue;
	}
	else if( ATTR_LR_H == wstrName )
	{	
		m_ptLR.y = (int)vValue;
	}
	else if( ATTR_CORNER_W == wstrName )
	{	
		m_ptCorner.x = (int)vValue;
	}
	else if( ATTR_CORNER_H == wstrName )
	{	
		m_ptCorner.y = (int)vValue;
	}
	else if ( ATTR_STATIC_NUM == wstrName)
	{
		StaticNum((int)vValue);
	}
}

void CXUI_StaticForm::VRefresh()
{
	CXUI_Wnd::VRefresh();
	if (m_pImgStatic) { m_spRscMgr->ReleaseRsc(m_pImgStatic); }
}

bool CXUI_StaticForm::VDisplay()
{
	// CXUI_Wnd::Display에서 
	if(!CXUI_Form::VDisplay() ){return false;}

	SRenderInfo kRenderInfo;
	kRenderInfo.bGrayScale = GrayScale();
	kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
	SSizedScale &rSS = kRenderInfo.kSizedScale;
	kRenderInfo.fAlpha = Alpha();
	GetParentDrawRect(kRenderInfo.rcDrawable);
	POINT3I ptLoc = TotalLocation();

	kRenderInfo.dwColor = m_dwColor;
	if ( CanColorChange() )
	{
		kRenderInfo.bColorChange = ColorChange();
	}

#if defined(XUI_USE_GENERAL_OFFSCREEN)
	// 그냥 offscreen을 그린다.
	if (UseOffscreen() && Offscreen() && Invalidate() == false)
	{
		if (UseParentOffscreen() == false)
		{
			// 이미 CXUI_Wnd::Display에서 그렸다.
		}
		else
		{
			// Parent가 알아서 그릴거다.
		}
	}
	else if (m_pImgStatic)
	{
		int iSrc = 0;
		rSS.ptSrcSize.x = Width();
		rSS.ptSrcSize.y = Height();
		rSS.ptDrawSize = Size()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V) ;
		kRenderInfo.kUVInfo = UVInfo();
		kRenderInfo.kLoc = ptLoc;
		if (kRenderInfo.kLoc.x < 0)
		{
			kRenderInfo.rcClip.left = -kRenderInfo.kLoc.x;
			kRenderInfo.kLoc.x = 0;

		}
		if (kRenderInfo.kLoc.y < 0)
		{
			kRenderInfo.rcClip.top = -kRenderInfo.kLoc.y;
			kRenderInfo.kLoc.y = 0;

		}
		m_spRenderer->RenderSprite(m_pImgStatic, iSrc, kRenderInfo);
	}
	else
#endif
	{
		bool bDrawOffscreen = false;
#if defined(XUI_USE_GENERAL_OFFSCREEN)
		if (UseOffscreen() && Offscreen() && Invalidate())
			bDrawOffscreen = true;

		if (StaticNum() != 0 && UseOffscreen() == false && m_pImgStatic == NULL && NotUseOffscreen() == false)
		{
			POINT2 ptResolutionSize;
			ptResolutionSize.x = Width();
			ptResolutionSize.y = Height();
			m_pImgStatic = m_spRscMgr->VCreateOffscreen(ptResolutionSize, StaticNum(), m_dwColor, AlphaMax());
			assert(m_pImgStatic);
			if (m_pImgStatic)
			{
				bDrawOffscreen = true;
			}
			else
			{
				NotUseOffscreen(true);
				UseOffscreen(false); // 이미 위에서 되어 있으니까 들어오겠지만.
			}
		}
#endif

		// offscreen에 그리기 위해서 좌표 재조정
		if (bDrawOffscreen)
		{
			CalcOffscreenLocation();
			ptLoc = POINT3I(0, 0, ptLoc.z) + OffscreenLocation();
			kRenderInfo.fAlpha = AlphaMax();
		}

		if(m_pImgBody)
		{//가운데
			rSS.ptSrcSize = m_ptBody;
			POINT2 ptBodyStart;
			ptBodyStart.x = min( m_ptLR.x/2, m_ptCorner.x/2 );
			ptBodyStart.y = m_ptTB.y/2;//min( min(m_ptTB.y, m_ptLR.y) , m_ptCorner.y/2 );

			rSS.ptDrawSize = POINT2( Size().x - ptBodyStart.x*2, Size().y - ptBodyStart.y*2 );

			if ( Size().x - m_ptCorner.x <= 0)
			{
				rSS.ptDrawSize.y -= m_ptCorner.y;
				ptBodyStart.y = m_ptCorner.y/2;
			}

			if ( Size().y - m_ptCorner.y <= 0)
			{
				rSS.ptDrawSize.x -= m_ptCorner.x;
				ptBodyStart.x = m_ptCorner.x/2;
			}

			if ( rSS.ptDrawSize.x > 0 && rSS.ptDrawSize.y > 0)
			{
				kRenderInfo.kUVInfo = UVInfo();
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.x += ptBodyStart.x;
				kRenderInfo.kLoc.y += ptBodyStart.y;

				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}

				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgBody, m_iImgBody, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgBody, m_pImgBody != NULL));
				}
			}
		}

		if(m_pImgCorner)
		{
			{//왼코너
				rSS.ptSrcSize = m_ptCorner;
				rSS.ptDrawSize.x = min(m_ptCorner.x/2, Size().x/2);
				rSS.ptDrawSize.y = min(m_ptCorner.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(2,2,1);
				kRenderInfo.kLoc = ptLoc;
				if (ptLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -ptLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (ptLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -ptLoc.y);
					kRenderInfo.kLoc.y = 0;
				}

				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgCorner, m_iImgCorner1, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgCorner, m_pImgBody != NULL));
				}
			}

			{//오른쪽코너
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptDrawSize.x = min(m_ptCorner.x/2, Size().x/2);
				rSS.ptDrawSize.y = min(m_ptCorner.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(2,2,2);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.x += (Size().x - rSS.ptDrawSize.x);
				if (rSS.ptDrawSize.x < m_ptCorner.x/2)	//!!! 주의 !!! CAN_CLIP옵션과 양립할수 없음!!!!!
				{
					kRenderInfo.rcClip.left = m_ptCorner.x/2 - rSS.ptDrawSize.x;
					rSS.ptDrawSize.x = m_ptCorner.x/2;
				}
				
				if (ptLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -ptLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgCorner, m_iImgCorner2, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgCorner, m_pImgBody != NULL));
				}
			}

			{//왼쪽 아래 코너
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptDrawSize.x = min(m_ptCorner.x/2, Size().x/2);
				rSS.ptDrawSize.y = min(m_ptCorner.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(2,2,3);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.y += (Size().y - rSS.ptDrawSize.y);

				if (rSS.ptDrawSize.y < m_ptCorner.y/2)	//!!! 주의 !!! CAN_CLIP옵션과 양립할수 없음!!!!!
				{
					kRenderInfo.rcClip.top = m_ptCorner.y/2 - rSS.ptDrawSize.y;
					rSS.ptDrawSize.y = m_ptCorner.y/2;
				}

				if (ptLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -ptLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (ptLoc.y + rSS.ptDrawSize.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -(ptLoc.y + rSS.ptDrawSize.y));
					kRenderInfo.kLoc.y = 0;
				}

				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgCorner, m_iImgCorner3, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgCorner, m_pImgBody != NULL));
				}
			}

			{//오른쪽 아래 코너
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptDrawSize.x = min(m_ptCorner.x/2, Size().x/2);
				rSS.ptDrawSize.y = min(m_ptCorner.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(2,2,4);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc += (Size() - rSS.ptDrawSize);

				if (rSS.ptDrawSize.x < m_ptCorner.x/2)	//!!! 주의 !!! CAN_CLIP옵션과 양립할수 없음!!!!!
				{
					kRenderInfo.rcClip.left = m_ptCorner.x/2 - rSS.ptDrawSize.x;
					rSS.ptDrawSize.x = m_ptCorner.x/2;
				}
				if (rSS.ptDrawSize.y < m_ptCorner.y/2)	//!!! 주의 !!! CAN_CLIP옵션과 양립할수 없음!!!!!
				{
					kRenderInfo.rcClip.top = m_ptCorner.y/2 - rSS.ptDrawSize.y;
					rSS.ptDrawSize.y = m_ptCorner.y/2;
				}		
				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgCorner, m_iImgCorner4, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgCorner, m_pImgBody != NULL));
				}
			}
		}

		if(m_pImgTB)
		{
			{//윗줄
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptSrcSize = m_ptTB;
				rSS.ptDrawSize.x = max(Size().x - m_ptCorner.x, 0);
				rSS.ptDrawSize.y = min(m_ptTB.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(1,2,1);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.x += m_ptCorner.x/2;
				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgTB, m_iImgT, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgTB, m_pImgBody != NULL));
				}
				
			}

			{//아랫줄
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptDrawSize.x = max(Size().x - m_ptCorner.x, 0);
				rSS.ptDrawSize.y = min(m_ptTB.y/2, Size().y/2);
				kRenderInfo.kUVInfo = SUVInfo(1,2,2);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.x += m_ptCorner.x/2;

				if ( rSS.ptDrawSize.y < m_ptTB.y/2 )
				{
					kRenderInfo.rcClip.top = m_ptTB.y/2 - rSS.ptDrawSize.y;
					rSS.ptDrawSize.y = m_ptTB.y/2;
				}

				kRenderInfo.kLoc.y += ( Size().y - m_ptTB.y/2 + kRenderInfo.rcClip.top );
				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgTB, m_iImgB, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgTB, m_pImgBody != NULL));
				}
			}
		}
		
		if(m_pImgLR)
		{
			{//왼쪽줄
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptSrcSize = m_ptLR;
				kRenderInfo.kUVInfo = SUVInfo(2,1,1);
				rSS.ptDrawSize.x = min(m_ptLR.x/2, Size().x/2);
				rSS.ptDrawSize.y = max(Size().y - m_ptCorner.y, 0);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.y += m_ptCorner.y/2;
				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgLR, m_iImgL, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgLR, m_pImgBody != NULL));
				}
			}

			{//오른쪽줄
				SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				rSS.ptDrawSize.x = min(m_ptLR.x/2, Size().x/2);
				rSS.ptDrawSize.y = max(Size().y - m_ptCorner.y, 0);
				kRenderInfo.kUVInfo = SUVInfo(2,1,2);
				kRenderInfo.kLoc = ptLoc;
				kRenderInfo.kLoc.y += m_ptCorner.y/2;

				if ( rSS.ptDrawSize.x < m_ptLR.x/2 )
				{
					kRenderInfo.rcClip.left = m_ptLR.x/2 - rSS.ptDrawSize.x;
					rSS.ptDrawSize.x = m_ptLR.x/2;
				}

				kRenderInfo.kLoc.x += ( Size().x - m_ptLR.x/2 + kRenderInfo.rcClip.left );
				if (kRenderInfo.kLoc.x < 0)	// 왼쪽 클리핑
				{
					kRenderInfo.rcClip.left = __min(rSS.ptDrawSize.x, -kRenderInfo.kLoc.x);
					kRenderInfo.kLoc.x = 0;
				}
				if (kRenderInfo.kLoc.y < 0)	// 위쪽 클리핑
				{
					kRenderInfo.rcClip.top = __min(rSS.ptDrawSize.y, -kRenderInfo.kLoc.y);
					kRenderInfo.kLoc.y = 0;
				}
				if (kRenderInfo.rcClip.left >= 0 && kRenderInfo.rcClip.top >= 0)	// 완전 가려지는 것들은 출력 안함
				{
					m_spRenderer->RenderSprite(m_pImgLR, m_iImgR, kRenderInfo, !bDrawOffscreen);
					//if (bDrawOffscreen)
					//	m_kOffscreenDrawList.push_back(std::make_pair(m_pImgLR, m_pImgBody != NULL));
				}
			}
		}

#ifdef XUI_USE_GENERAL_OFFSCREEN
		if (bDrawOffscreen)
		{
			if (m_pImgLR)
				m_kOffscreenDrawList.push_back(std::make_pair(m_pImgLR, Parent() == NULL));
			if (m_pImgTB)
				m_kOffscreenDrawList.push_back(std::make_pair(m_pImgTB, Parent() == NULL));
			if (m_pImgCorner)
				m_kOffscreenDrawList.push_back(std::make_pair(m_pImgCorner, Parent() == NULL));
			if (m_pImgBody)
				m_kOffscreenDrawList.push_back(std::make_pair(m_pImgBody, Parent() == NULL));

			if (m_pImgStatic)
				m_spRenderer->EndRenderOffscreen(m_pImgStatic, m_kOffscreenDrawList);
			else if (Offscreen())
			{
				void* pOffscreen = Offscreen();
				m_spRenderer->EndRenderOffscreen(pOffscreen, m_kOffscreenDrawList);
			}
		}

		if (m_pImgStatic)
		{
			m_UVInfo = SUVInfo(1, 1, 1);
			int iSrc = 0;
			rSS.ptSrcSize.x = Width();
			rSS.ptSrcSize.y = Height();
			rSS.ptDrawSize = Size()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V) ;
			kRenderInfo.kUVInfo = UVInfo();
			kRenderInfo.fAlpha = Alpha();
			kRenderInfo.kLoc = TotalLocation();
			m_spRenderer->RenderSprite(m_pImgStatic, iSrc, kRenderInfo);
		}
#endif
	}
	return true;
}

CXUI_Wnd* CXUI_StaticForm::VClone()
{
	CXUI_StaticForm *pWnd = new CXUI_StaticForm;

	pWnd->operator =(*this);

	return pWnd;
}

void CXUI_StaticForm::operator = ( const CXUI_StaticForm &rhs)
{
	CXUI_Wnd::operator =(rhs);

	StaticNum(rhs.StaticNum());

	m_wstrImgBody = rhs.m_wstrImgBody;
	m_wstrImgTB = rhs.m_wstrImgTB;
	m_wstrImgLR = rhs.m_wstrImgLR;
	m_wstrImgCorner = rhs.m_wstrImgCorner;

	m_ptBody = rhs.m_ptBody;
	m_ptTB = rhs.m_ptTB;
	m_ptLR = rhs.m_ptLR;
	m_ptCorner = rhs.m_ptCorner;

	m_pImgBody = m_spRscMgr->GetRsc(m_wstrImgBody);
	m_pImgTB = m_spRscMgr->GetRsc(m_wstrImgTB);
	m_pImgLR = m_spRscMgr->GetRsc(m_wstrImgLR);
	m_pImgCorner = m_spRscMgr->GetRsc(m_wstrImgCorner);
}