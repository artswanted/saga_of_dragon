#include "stdafx.h"
#define DIRECTINPUT_VERSION (0x0800)

#include "tchar.h"
#include "XUI_Wnd.h"
#include "BM/vstring.h"
#include "../../../cosmos/DirectXSDK/Include/dinput.h"
#include "XUI_Font.h"
#include "XUI_Config.h"
#include "XUI_Edit.h"
#include "XUI_Manager.h"

using namespace XUI;

#define CENTER_ALIGN( screen, org ) ((screen - org )/2 )
#define UPEER_ALIGN( screen, org ) (0)
#define LOWER_ALIGN( screen, org ) (screen - org)

int ALIGN(int iScreen, int iOrg, int iPercent )
{
	return ((iScreen - iOrg)*iPercent)/100;
}

CXUI_Resource_Manager_Base* CXUI_Wnd::m_spRscMgr = NULL;
CXUI_Renderer_Base* CXUI_Wnd::m_spRenderer = NULL;
CXUI_MediaPlayer_Base* CXUI_Wnd::m_spMediaPlayer = NULL;
CXUI_Config* CXUI_Wnd::m_spConfig = NULL;
CXUI_TextTable_Base* CXUI_Wnd::m_spTextTable = NULL;
CXUI_HotKeyModifier* CXUI_Wnd::m_spHotKeyModifier = NULL;

LP_SCRIPT_FUNC CXUI_Wnd::m_pScriptFunc = NULL;
LP_CURSOR_CALLBACK CXUI_Wnd::m_pCursorCallBackFunc = NULL;
LP_TEXT_CLICK_CALLBACK	CXUI_Wnd::m_pTextClickCallBackFunc = NULL;
LP_HOTKEY_TO_REALKEY_CHANGER	CXUI_Wnd::m_spHotKeyToRealKey = NULL;
LP_LOG_FUNC CXUI_Wnd::m_pLogFunc = NULL;
HWND CXUI_Wnd::m_sHwnd = NULL;
CXUI_Wnd::CONT_ALIVE_UNIQUE_EXIST_UI CXUI_Wnd::m_kContAliveUniqueExistUI;
LP_UNIQUE_EXIST_FAILED_NOTICE CXUI_Wnd::m_pUniqueExistCheckNotice = NULL;

bool CXUI_Wnd::m_bDefaultResolution = true;
POINT2 CXUI_Wnd::m_sptResolutionSize( EXV_DEFAULT_SCREEN_WIDTH, EXV_DEFAULT_SCREEN_HEIGHT );
POINT2 CXUI_Wnd::m_sptGab( 0, 0 );


POINT3I CXUI_Wnd::m_sMousePos( EXV_DEFAULT_SCREEN_WIDTH/2, EXV_DEFAULT_SCREEN_HEIGHT/2, 0);

CXUI_Wnd* CXUI_Wnd::m_spWndMouseOver = NULL;
CXUI_Wnd* CXUI_Wnd::m_spWndMouseFocus = NULL;
CXUI_Wnd* CXUI_Wnd::m_spWndScrollBtn = NULL;
bool CXUI_Wnd::m_bIsOverWnd = false;



#ifdef CHECK_XUI_PERFORMANCE
XUIStatGroup CXUI_Wnd::m_kXUIStatGroup;
#endif

wchar_t* g_kScriptNames[] = {
	_T("ON_ACTIVATE"),
	_T("ON_CREATE"),
	_T("ON_FOCUS"),						//EditBox에 한하여 Focus를 가질때 (ON_FOCUS, SCRIPT_ON_FOCUS,
	_T("ON_CALL"),
	_T("ON_CLOSE"),
	_T("ON_ALIVE_TIME_END"),
	_T("ON_CORRECT"),
	_T("ON_L_BTN_UP"),
	_T("ON_L_BTN_DOWN"),
	_T("ON_L_BTN_DBL_DOWN"),
	_T("ON_R_BTN_UP"),
	_T("ON_R_BTN_DOWN"),
	_T("ON_KEY_ENTER"),//IME와 XUI_Edit관련 전용 이벤트
	_T("ON_KEY_TAB"),//IME와 XUI_Edit관련 전용 이벤트
	_T("ON_KEY_UP"),//IME와 XUI_Edit관련 전용 이벤트
	_T("ON_KEY_DOWN"),//IME와 XUI_Edit관련 전용 이벤트
	_T("ON_MOUSE_OVER"),
	_T("ON_MOUSE_OUT"),
	_T("ON_MOUSE_MOVE"),
	_T("ON_BAR_FULL"),
	_T("ON_BAR_CLOSE"),
	_T("ON_TICK"),
	_T("ON_BUILD"),
	_T("ON_DISPLAY"),
	_T("ON_AFTER_DISPLAY"),
	_T("ON_OUT_DISPLAY"),
	_T("ON_AFTER_L_BTN_UP"),
	_T("ON_AFTER_R_BTN_UP"),
	_T("ON_WHEEL_UP"),
	_T("ON_WHEEL_DOWN"),
	_T("ON_SCROLL_MOVE"),
	_T("ON_SCROLL_VALUE_CHANGED"),
	_T("ON_GET_FOCUS"),
	_T("ON_LOSE_FOCUS"),
	_T("ON_PICK_UP"),
};

//FT_Face	CXUI_Wnd::m_sFontFace = NULL;
//FT_Library  CXUI_Wnd::m_sftlib = NULL;
float const fHexFull = 255.f;

CXUI_Wnd::CXUI_Wnd(void)
	: m_fRotationDeg(0.0f)
	, m_pParent(NULL)
	, m_pText(NULL)
	, m_bVisible(true)
{
	OwnerState(0);

	BtnType(0);
	GrayScale(false);

	Priority(0);


	CanDrag(false);
	CanClip(false);
	Enable(true);
	IsAbsoluteX(false);
	IsAbsoluteY(false);

	NeedCorrectionLocation(true);
	
	Location( 0, 0, 0 );
	Size( 0, 0 );
	//Visible(true );//보이냐.
	
	DefaultImg( NULL );
	Offscreen( NULL );
	UseOffscreen(false);
	UseParentOffscreen(false);

	IsMouseDown( false );

	AlignX(-1);
	AlignY(-1);

	
	FontFlag(XTF_NONE);

	ImgIdx(-1);

	BuildIndex(0);

	CalledTime(BM::GetTime32());
	AliveTime(0);
	m_dwLastTickTime = 0;
	TickInterval(0);
	IsAliveTimeIgnoreMouseOver(false);
	Alpha(DEFAULT_ALPHA);
	AlphaMax(ALPHA_MAX);
	UseDrawRect(false);
	FontColor(0xFF000000);
	OutLineColor(0xFF000000);
	m_bUpdatedText = false;

	IsUseAlphaAni(true);
	IsModal(false);
	IsIgnoreParentRect(false);

//	IsMouseOver(false);
	IsClosed(false);
	m_dwColor = 0xffffffff;
	m_dwPastColor = 0xffffffff;
	m_bConvRealKey = false;
	ColorChange(false);
	CanColorChange(false);
	ColorType(0);
	NoWordWrap(false);
	InvertBold(false);
	//m_bScriptArray = NULL;
	PassEvent(false);
	IsDoNotLostFocus(false);

	AppearEffect( AE_NONE );
	EffectSpeed( 1 );
	SetState( ST_NONE );

	NoFocus(false);
	TypingEffect(false);
	LastUsedTime(BM::GetTime32());

	m_bIsTwinkle = false;
	//TwinkleStartTime(0);
	TwinkleInterTime(1000);
	TwinkleTime(10000);
	ReservTwinkleOff(false);
	WheelValue(0);
	IgnoreTwinkle(false);
	Scale(1.0f);

	UseNewOffscreen(false);
	NewOffscreen(NULL);
	HideOtherWnd(false);
	NotHideWnd(false);
	//m_HideList.clear();
	EventRepeateDelay(400);	//0.4초
	UseInputCheck(false);
	IsOutsideWnd(false);
	IsRemoveOutside(false);

#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	//m_scriptArray.resize(SCRIPT_MAX_NUM);
	//m_bScriptArray = new bool[SCRIPT_MAX_NUM];
	//assert(m_bScriptArray);
	for (unsigned int i = 0; i < SCRIPT_MAX_NUM; i++)
	{
		m_bScriptArray[i] = false;
	}
#endif
}

CXUI_Wnd::~CXUI_Wnd(void)
{
	RemoveAllControls(IsClosed());
	ReleaseDefaultImg();
	ReleaseOffscreen();

	if(!IsClosed())
	{
		Close(false);
	}

	SAFE_DELETE(m_pText);
	if (m_spWndMouseOver == this)
	{
		m_spWndMouseOver = NULL;
	}

	if (m_spWndMouseFocus == this)
	{
		m_spWndMouseFocus = NULL;
	}

	//SAFE_DELETE(m_bScriptArray);
}

void CXUI_Wnd::ReleaseDefaultImg()
{
	void *pImg = DefaultImg();
	if( pImg )
	{//본인이 가지고 계심.
		m_spRscMgr->ReleaseRsc(pImg);
		ImgIdx(-1);
	}
}

void CXUI_Wnd::ReleaseOffscreen()
{
	void *pImg = Offscreen();
	if( pImg && UseParentOffscreen() == false)
	{
		m_spRscMgr->ReleaseRsc(pImg);
		Offscreen(NULL);
	}
}

void CXUI_Wnd::Close()
{
	Close(true);
}

void CXUI_Wnd::ClearOutside()
{
	CONT_CONTROL::iterator kItor =  m_contControls.begin();
	while( m_contControls.end() != kItor )
	{
		if( (*kItor)->IsOutsideWnd() )
		{
			SAFE_DELETE((*kItor));
			kItor = m_contControls.erase(kItor);
			continue;
		}
		++kItor;
	}
}

bool CXUI_Wnd::HaveOutside()const
{
	CONT_CONTROL::const_iterator kItor =  m_contControls.begin();
	while( m_contControls.end() != kItor )
	{
		if( (*kItor)->IsOutsideWnd() )
		{
			return true;
		}
		++kItor;
	}
	return false;
}

bool CXUI_Wnd::IncAlpha()
{
	if(Alpha() != AlphaMax())
	{
		Alpha(__min(AlphaMax(), Alpha()+ALPHA_INC) );
	}
	
	if(!IsUseAlphaAni())
	{
		Alpha( AlphaMax() );
	}
	return true;
}

bool CXUI_Wnd::VDisplay()
{
	if( !Visible() ){return false;}//자식컨트롤도 안그림

	if( NewOffscreen() )	//	Offscreen 에 이미 그려놨다.
	{
		void *pkOffscreen = NewOffscreen();
		m_spRenderer->RenderOffscreen(pkOffscreen);
		return	false;
	}

	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.OffscreenCheck"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerB((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.OnDisplayScript"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerC((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.AfterDispScript"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerD((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.TextRender"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerE((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.Invalidate.Calc"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerE2((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.Invalidate.Render"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerF((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.RenderOffscreen.Calc"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerG((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.RenderOffscreen.Render"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerH((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplay.OutDispScript"), CXUI_Manager::GetFrameCount()));
	
#ifdef XUI_USE_GENERAL_OFFSCREEN
	XUI_STAT(timerA.Start());
	if (UseOffscreen() && Offscreen() == NULL)
	{
		// Offscreen Create
		void* pOffscreen = GetParentOffscreen();
		if (pOffscreen == NULL)
		{
			POINT2 ptResolutionSize;
			ptResolutionSize.x = Width();
			ptResolutionSize.y = Height();
			pOffscreen = m_spRscMgr->VCreateOffscreen(ptResolutionSize, 0, m_dwColor, AlphaMax());
			if (pOffscreen == NULL)
			{
				NotUseOffscreen(true);
				UseOffscreen(false);
			}
			else
				Offscreen(pOffscreen);

			CalcOffscreenLocation();
			if (Offscreen())
				Invalidate(true);
		}
		else
		{
			Offscreen(pOffscreen);
			CalcOffscreenLocation();
			UseParentOffscreen(true);
			Invalidate(true);
		}
	}
	XUI_STAT(timerA.Stop());
#endif

	XUI_STAT(timerB.Start());
	DoScript( SCRIPT_ON_DISPLAY );// 먼저 찍힐것.
	XUI_STAT(timerB.Stop());

	IncAlpha();

	DisplayEffect();
	DisplayControl();

	XUI_STAT(timerC.Start());
	DoScript( SCRIPT_ON_AFTER_DISPLAY );//뒤에 찍힐것. (그림에 뭐가 가린다거나 할때 쓰면됨
	XUI_STAT(timerC.Stop());

	if (m_spRenderer)
	{
		XUI_STAT(timerD.Start());
		// 일단 Text는 Offscreen과 상관 없다.
		if( StyleText().Length() )
		{
			POINT3I const pt = TotalLocation() + TextPos();
			RenderText( pt, StyleText().GetOriginalString(), Font(), FontFlag() );
			//RenderText( pt, StyleText().GetAdjustedString(), Font(), FontFlag() );
		}
		XUI_STAT(timerD.Stop());

		void *pImg = DefaultImg();
#ifdef XUI_USE_GENERAL_OFFSCREEN
		if (Invalidate() == true || (UseOffscreen() == false && pImg))
#endif
		{
			XUI_STAT(timerE.Start());
			bool bDrawOffscreen = false;
			if (UseOffscreen() && Offscreen() && Invalidate())
				bDrawOffscreen = true;

			if ( pImg )
			{
				void* pOffscreen = Offscreen();
				SRenderInfo kRenderInfo;
				SSizedScale &rSS = kRenderInfo.kSizedScale;

				kRenderInfo.bGrayScale = GrayScale();

				rSS.ptSrcSize = ImgSize();
				rSS.ptDrawSize = ImgSize()/POINT2((LONG)UVInfo().U, (LONG)UVInfo().V);
				kRenderInfo.kUVInfo = UVInfo();
				GetParentDrawRect(kRenderInfo.rcDrawable);
				if (bDrawOffscreen)
				{
					kRenderInfo.kLoc = OffscreenLocation() + ImgPos();
					kRenderInfo.fAlpha = AlphaMax();					
				}
				else
				{
					kRenderInfo.kLoc = TotalLocation() + ImgPos();
					kRenderInfo.fAlpha = Alpha();
				}

				if ( m_dwColor != m_dwPastColor )
				{
					ColorChange(true);
					m_dwPastColor = m_dwColor;
					kRenderInfo.dwColor = m_dwColor;
				}
				else
				{
					ColorChange(false);
				}

				if ( CanColorChange() )
				{
					kRenderInfo.bColorChange = ColorChange();
				}
				
				if( GetClip() )
				{//클리핑을 위한 렉텡글 조절
					POINT3I pt = TotalLocation();
					if(	pt.x < 0 )
					{
						// offscreen에 대해서 뭔가 해줘야 할게 있을까?
						kRenderInfo.rcClip.left = abs(pt.x);
						kRenderInfo.kLoc.x = 0;
					}
					if(	pt.y < 0 )
					{
						kRenderInfo.rcClip.top = abs(pt.y);
						kRenderInfo.kLoc.y = 0;
					}
				}
				else
				{
					SetRect(&kRenderInfo.rcClip, 0,0,0,0);
				}

				XUI_STAT(timerE.Stop());
				XUI_STAT(timerE2.Start());
		
				kRenderInfo.bTwinkle = IsTwinkle() && NowTwinkleOn() && !IgnoreTwinkle();
				kRenderInfo.fScale = Scale();
				kRenderInfo.kScaleCenter = ScaleCenter();
				kRenderInfo.fRotationDeg = RotationDeg();
				kRenderInfo.kRotationCenter = RotationCenter();
#ifdef XUI_USE_GENERAL_OFFSCREEN
				if (bDrawOffscreen)
				{
					m_spRenderer->RenderSprite( pImg, m_ImgIdx, kRenderInfo, false);
					m_kOffscreenDrawList.push_back(std::make_pair(pImg, Parent() == NULL));
				}
				else
#endif
				{
					VDisplayOverlayImg(kRenderInfo); //기본 이미지 위에 덮어질 레이어 이미지를 그린다
					m_spRenderer->RenderSprite( pImg, m_ImgIdx, kRenderInfo);
				}
				XUI_STAT(timerE2.Stop());
			}
			XUI_STAT(timerE.Stop());
		}
#ifdef XUI_USE_GENERAL_OFFSCREEN
		// 그냥 offscreen을 그린다.
		if (UseOffscreen() && Offscreen())
		{
			if (UseParentOffscreen() == false)
			{
				XUI_STAT(timerF.Start());
				int iSrc = 0;
				void* pOffscreen = Offscreen();
				SRenderInfo kRenderInfo;
				SSizedScale &rSS = kRenderInfo.kSizedScale;
				kRenderInfo.fAlpha = Alpha();			
				GetParentDrawRect(kRenderInfo.rcDrawable);

				if ( m_dwColor != m_dwPastColor )
				{
					ColorChange(true);
					m_dwPastColor = m_dwColor;
					kRenderInfo.dwColor = m_dwColor;
				}
				else
				{
					ColorChange(false);
				}

				if ( CanColorChange() )
				{
					kRenderInfo.bColorChange = ColorChange();
				}

				rSS.ptSrcSize = Size();
				rSS.ptDrawSize = Size();
				kRenderInfo.kUVInfo = UVInfo();
				kRenderInfo.kLoc = TotalLocation();
				XUI_STAT(timerF.Stop());
				XUI_STAT(timerG.Start());
				m_spRenderer->RenderSprite(pOffscreen, iSrc, kRenderInfo);
				XUI_STAT(timerG.Stop());
			}
			else
			{
				// ParentOffscreen를 쓰고 있다면 그 Offscreen의 owner가 알아서 그릴거다.
			}
		}
#endif
	}

	XUI_STAT(timerH.Start());
	DoScript( SCRIPT_ON_OUT_DISPLAY );//뒤에 찍힐것. (그림에 뭐가 가린다거나 할때 쓰면됨
	XUI_STAT(timerH.Stop());
	return true;
}

void CXUI_Wnd::DisplayEffect()
{
	switch( m_eState )
	{
	case ST_OPENING:
		{
			switch( AppearEffect() )
			{
			case AE_CUTTON_LEFT:
				{
					Location( Location().x - (int)EffectSpeed(), Location().y );
					if ( Location().x <= TargetPos().x )
					{
						Location( TargetPos() );
						SetState( ST_NONE );
					}
				}break;
			case AE_CUTTON_RIGHT:
				{
					Location( Location().x + (int)EffectSpeed(), Location().y );
					if ( Location().x >= TargetPos().x )
					{
						Location( TargetPos() );
						SetState( ST_NONE );
					}
				}break;
			default:
				{
					SetState( ST_NONE );
				}break;
			}
		}break;
	default:
		{
		}break;
	}
}

bool CXUI_Wnd::VDisplayEnd()
{
	if (!Visible())
		return false;

	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplayEnd"), CXUI_Manager::GetFrameCount()));
#ifdef XUI_USE_GENERAL_OFFSCREEN
	if (UseOffscreen() && Offscreen() && Invalidate())
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VDisplayEnd.EndRenderOffscreen"), CXUI_Manager::GetFrameCount()));
		void* pOffscreen = Offscreen();
		m_spRenderer->EndRenderOffscreen(pOffscreen, m_kOffscreenDrawList, UseParentOffscreen() == false);
		m_kOffscreenDrawList.clear();
		Invalidate(false);
	}

	// 그리는 순서 때문에 거꾸로 돈다
	CONT_CONTROL::reverse_iterator itor = m_contControls.rbegin();
	while( m_contControls.rend() != itor)
	{
		(*itor)->VDisplayEnd();
		++itor;
	}
#endif
	return true;
}

void CXUI_Wnd::SetInvalidate(bool bInvalidateParent)
{
	Invalidate(true);

	if(bInvalidateParent)
	{
		CXUI_Wnd	*pkParent = Parent();
		if(pkParent)
		{
			pkParent->SetInvalidate(bInvalidateParent);
		}
	}
#ifdef XUI_USE_GENERAL_OFFSCREEN
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("SetInvalidate"), CXUI_Manager::GetFrameCount()));
	CXUI_Wnd* pWnd = NULL;
	CXUI_Wnd* pOffscreenOwner = GetOffscreenOwner();
	
	if (bInvalidateParent)
		pWnd = pOffscreenOwner;
	else
		pWnd = this;

	if (pWnd)
	{
		void* pOffscreen = pWnd->Offscreen();
		if (bInvalidateParent)
		{
			m_spRenderer->InvalidateOffscreen(pOffscreen);
		}
		else

		{
			CalcOffscreenLocation();
			POINT3I loc = OffscreenLocation();
			POINT2 size = Size();
			RECT invalidateRect;
			invalidateRect.left = loc.x;
			invalidateRect.top = loc.y;
			invalidateRect.right = loc.x + size.x;
			invalidateRect.bottom = loc.y + size.y;
			m_spRenderer->InvalidateOffscreen(pOffscreen, invalidateRect);
		}
		pWnd->InvalidateChild();
	}
#endif
}

void CXUI_Wnd::InvalidateChild()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	Invalidate(true);
	
	CONT_CONTROL::iterator itor = m_contControls.begin();
	while( m_contControls.end() != itor)
	{
		(*itor)->InvalidateChild();
		++itor;
	}
#endif
}

void CXUI_Wnd::RefreshCalledTime()
{
	CalledTime(BM::GetTime32());
}

void CXUI_Wnd::RefreshLastTickTime()
{
	m_dwLastTickTime = BM::GetTime32();
}

bool CXUI_Wnd::PickWnd(POINT2 const& kptPos, CXUI_Wnd *&pkOutWnd)
{//좌표 및 기타
	if( !Visible() ){return false;}//자식도 안함.
	if( !Enable() ){return false;}

	if(ContainsPoint(kptPos))
	{
		CONT_CONTROL::iterator kItor = m_contControls.begin();
		while(m_contControls.end() != kItor)
		{
			if( (*kItor)->PickWnd(kptPos, pkOutWnd) )
			{
				return true;
			}
			++kItor;
		}
		pkOutWnd = this;//컨트롤들이 내 픽에 걸리지 않으면 나를 리턴.
		return true;
	}
	return false;
}

#ifdef XUI_LOCATION_RECT_CALC_ONTICK

bool CXUI_Wnd::CalcParentDrawRect(RECT &rkOutRc)

#else

bool CXUI_Wnd::GetParentDrawRect(RECT &rkOutRc)

#endif
{
	if( GetIsDefaultResolution() )
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CalcParentDrawRect"), CXUI_Manager::GetFrameCount()));
		::SetRect(&rkOutRc, 0,0,EXV_DEFAULT_SCREEN_WIDTH,EXV_DEFAULT_SCREEN_HEIGHT);
	}
	else
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CalcParentDrawRect"), CXUI_Manager::GetFrameCount()));
		::SetRect(&rkOutRc, 0,0,GetResolutionSize().x, GetResolutionSize().y );
	}	

	CXUI_Wnd *pParent = Parent();
	bool bIsUseRect = false;
	while(pParent)
	{
		if(pParent->UseDrawRect())
		{//최상위 부모까지 올라감
			bIsUseRect = true;
		}

		POINT2 const& ptSize = pParent->Size();
		POINT2 ptLocation = pParent->TotalLocation();

		rkOutRc.left = __max(rkOutRc.left, ptLocation.x);
		rkOutRc.top = __max(rkOutRc.top, ptLocation.y);
		rkOutRc.right = __min(rkOutRc.right, ptLocation.x + ptSize.x);
		rkOutRc.bottom = __min(rkOutRc.bottom, ptLocation.y +ptSize.y);

		rkOutRc.left = __min(rkOutRc.left, rkOutRc.right);//영역이 역전된 위치에 있음을 방어
		rkOutRc.top = __min(rkOutRc.top, rkOutRc.bottom);

		pParent = pParent->Parent();
	}
	if( !GetClip() )
	{
		if(!bIsUseRect 
			|| IsIgnoreParentRect())
		{
			::SetRect(&rkOutRc, 0,0,0,0);
		}
	}
	return bIsUseRect;
}

#ifdef XUI_LOCATION_RECT_CALC_ONTICK
void CXUI_Wnd::CalcParentRect(RECT &rkOutRc)
#else
void CXUI_Wnd::GetParentRect(RECT &rkOutRc)
#endif
{
	if( GetIsDefaultResolution() )
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CalcParentRect"), CXUI_Manager::GetFrameCount()));
		::SetRect(&rkOutRc, 0,0,EXV_DEFAULT_SCREEN_WIDTH,EXV_DEFAULT_SCREEN_HEIGHT);
	}
	else
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CalcParentRect"), CXUI_Manager::GetFrameCount()));
		::SetRect(&rkOutRc, 0,0,GetResolutionSize().x, GetResolutionSize().y );
	}	

	CXUI_Wnd *pParent = Parent();
	while(pParent)
	{
		POINT2 ptSize = pParent->Size();
		POINT2 ptLocation = pParent->TotalLocation();

		rkOutRc.left = __max(rkOutRc.left, ptLocation.x);
		rkOutRc.top = __max(rkOutRc.top, ptLocation.y);
		rkOutRc.right = __min(rkOutRc.right, ptLocation.x + ptSize.x);
		rkOutRc.bottom = __min(rkOutRc.bottom, ptLocation.y +ptSize.y);

		rkOutRc.left = __min(rkOutRc.left, rkOutRc.right);//영역이 역전된 위치에 있음을 방어
		rkOutRc.top = __min(rkOutRc.top, rkOutRc.bottom);

		pParent = pParent->Parent();
	}
}

void CXUI_Wnd::VOnClose()
{
	if (HideOtherWnd())
	{
		XUIMgr.ShowAllUI();
	}
	for_each( m_contControls.begin(), m_contControls.end(), OnCloseControl_Func() );
	if( CXUI_Wnd::m_spWndMouseFocus == this )
	{
		//CXUI_Wnd::m_spWndMouseFocus = NULL;
		VLoseFocus();
	}
	
	if(IsRemoveOutside())
	{
		ClearOutside();
	}
}

void CXUI_Wnd::VOnCall()
{
//	pWnd->Visible( true );//--> Tree나 기타 애들은 visible 꺼놓은 경우도 있으므로. 여기서 해주지는 말자.

	DWORD const dwCurTime = BM::GetTime32();
	IsClosed(false);
	CalledTime(dwCurTime);
	m_dwLastTickTime = dwCurTime;
	DoScript(SCRIPT_ON_CALL);
	Alpha(DEFAULT_ALPHA);
	if (HideOtherWnd())
	{
		XUIMgr.HideAllUI();
	}
	for_each( m_contControls.begin(), m_contControls.end(), OnCallControl_Func() );
	Invalidate(true); // QUESTION: 위에까지 올라가야 하나?

	if ( AppearEffect() )
	{
		SetState( ST_OPENING );
	}

	CXUI_Manager::F_XUI_Manager_CallBackFunc kCallBackFunc = XUIMgr.GetCallBackFunc();
	if(kCallBackFunc)
	{
		kCallBackFunc(this,CXUI_Manager::CBET_ON_CALL);
	}

	TypingEffectReset();

	if(m_spMediaPlayer)
	{
		m_spMediaPlayer->PlaySoundByID(CallSoundID());
	}
}

void CXUI_Wnd::VOnLocationChange()
{
#ifdef XUI_LOCATION_RECT_CALC_ONTICK
	// Location이 OnTick에서만 처리되는게 아님.
	TotalLocation(GetTotalLocation());
	CalcParentRect(m_kParentRect);
	CalcParentDrawRect(m_kParentDrawRect);
	CalcOffscreenLocation();

	CONT_CONTROL::iterator kItor = m_contControls.begin();
	while(m_contControls.end() != kItor)
	{
		(*kItor)->VOnLocationChange();
		++kItor;
	}
#endif
}

void CXUI_Wnd::RegistScript( SCRIPT_HASH::key_type const& rKey, SCRIPT_HASH::mapped_type const& rScript )
{
#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	bool bFound = false;
	for (unsigned int i = 0; i < SCRIPT_MAX_NUM; i++)
	{
		if (rKey == std::wstring(g_kScriptNames[i]) && rScript.empty() == false)
		{
			m_bScriptArray[i] = true;
			m_scriptArray[i] = rScript;
			//TCHAR buf[256];
			//_stprintf(buf, 256, _T("%s RegisterScript %s, %d\n"), ID().c_str(), rKey.c_str(), i);
			//OutputDebugString(buf);
			bFound = true;
			break;
		}
	}

	//if (!bFound)
	//{
	//	TCHAR buf[256];
	//	_stprintf(buf, 256, _T("%s RegisterScript %s not found\n"), ID().c_str(), rKey.c_str());
	//	OutputDebugString(buf);
	//}
#endif
	m_mapScript.insert( std::make_pair( rKey, rScript) );
}

//void CXUI_Wnd::RegistHotKey( const HOT_KEY_HASH::key_type &rKey, const HOT_KEY_HASH::mapped_type &rScript )
//{
//	m_kHotKeyHash.insert( std::make_pair( rKey, rScript) );
//}

void CXUI_Wnd::DefaultImgName(std::wstring const& wstrImgName)
{
	ReleaseDefaultImg();
	
	m_DefaultImgName = wstrImgName;
	DefaultImg( m_spRscMgr->GetRsc( DefaultImgName() ) );
	SetInvalidate();
}

void CXUI_Wnd::DefaultImgTexture(void *pkTex)
{
	ReleaseDefaultImg();
	m_DefaultImgName = _T("");
	DefaultImg(pkTex);
	SetInvalidate();
}

void CXUI_Wnd::Location(POINT3I const& rPT, bool const bIsModifyBoundingBox )
{
	Location( rPT.x, rPT.y, rPT.z,  bIsModifyBoundingBox);
}

void CXUI_Wnd::Location(POINT2 const& rPT, bool const bIsModifyBoundingBox )
{
	Location( rPT.x, rPT.y, Location().z, bIsModifyBoundingBox);
}

void CXUI_Wnd::Location(int const x, int const y, bool const bIsModifyBoundingBox) 
{
	Location(x, y, Location().z, bIsModifyBoundingBox);
}
void CXUI_Wnd::Location(int const x, int const y, int const z, bool const bIsModifyBoundingBox)
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("Location"), CXUI_Manager::GetFrameCount()));
	if(!Parent())//자신이 최상위일때만 스크린과의 영역 설정을한다
	{
		POINT3I ptOrg = Location();
		POINT3I ptAdd = POINT3I(x,y,z) - ptOrg;
		
		if( GetClip() )	// CorrectionScreenPoint함수가 static이라 내부에서 호출이 안됨
		{
			ptOrg += ptAdd;	
		}
		else
		{
			CXUI_Wnd::CorrectionScreenPoint( ptOrg, ptAdd );//0점 정리.
		}
		
		POINT3I ptAfter = ptOrg+Size();

		if( !GetClip() )	// CorrectionScreenPoint함수가 static이라 내부에서 호출이 안됨
		{
			CXUI_Wnd::CorrectionScreenPoint( ptAfter, POINT3I() );//스크린 끝점 정리.	
		}

		m_Location = ptAfter-Size();
	}
	else
	{
		if(m_Location != POINT3I(x,y,z))
		{
			SetInvalidate();
		}
		m_Location = POINT3I(x,y,z);
	}	

	if( bIsModifyBoundingBox )
	{
		VOnLocationChange();
		SetBoundingBox( Location(), Size() );
	}
}

void CXUI_Wnd::Size(POINT2 const& rPT, bool const bIsModifyBoundingBox )
{
	Size( rPT.x, rPT.y, bIsModifyBoundingBox);
}

void CXUI_Wnd::Size( int const width, int const height, bool const bIsModifyBoundingBox )//바운드 박스까지 셋팅
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("Size"), CXUI_Manager::GetFrameCount()));
	m_Size.x = width;
	m_Size.y = height;

	// 모든 하위 control Align 다시 계산
	if( m_contControls.size() )
	{
		for_each( m_contControls.begin(), m_contControls.end(), VAlignControl_Func() );
	}

	SetInvalidate();

	if( bIsModifyBoundingBox )
	{
		VOnLocationChange();
		SetBoundingBox( Location(), Size() );
	}
}
unsigned int CXUI_Wnd::GetContCount()
{
	//	m_contControls에 들어있는 자식 윈도우 갯수를 리턴한다.
	return	(unsigned int)m_contControls.size();
}
CXUI_Wnd*	CXUI_Wnd::GetContAt(int iIndex)
{
	//	m_contControls 의 iIndex 번째 윈도우를 리턴한다.

	int	iCount = 0;
	for(CONT_CONTROL::iterator itor = m_contControls.begin(); itor!= m_contControls.end(); itor++)
	{
		if(iCount == iIndex)
		{
			return	*itor;
		}

		iCount++;
	}

	return	NULL;

}
void CXUI_Wnd::SetBoundingBox(POINT2 const& ptLocation, POINT2 const& ptSize)
{
	for_each( m_contControls.begin(), m_contControls.end(), BoundingBoxControl_Func() );	

	POINT2 const& pt = TotalLocation();
	
	RECT rc;
	GetParentRect(rc);
	
	if ( IsIgnoreParentRect() )
	{
		m_rcBoundingBox.left = pt.x;
		m_rcBoundingBox.top = pt.y;
		m_rcBoundingBox.right = pt.x + ptSize.x;
		m_rcBoundingBox.bottom = pt.y + ptSize.y;
	}
	else
	{
		m_rcBoundingBox.left = __max(pt.x, rc.left);
		m_rcBoundingBox.top = __max(pt.y, rc.top);
		m_rcBoundingBox.right = __min(pt.x + ptSize.x, rc.right);
		m_rcBoundingBox.bottom = __min(pt.y + ptSize.y, rc.bottom);
	}

/*	SetRect( &m_rcBoundingBox, pt.x, pt.y, 
		pt.x + ptSize.x, 
		pt.y + ptSize.y);*/
}

bool CXUI_Wnd::ContainsPoint(POINT2 const& rPT)const
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("ContainsPoint"), CXUI_Manager::GetFrameCount()));
	return ( (PtInRect( &m_rcBoundingBox, (const POINT)rPT ) ) ? true:false );
}

void CXUI_Wnd::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VRegistAttr"), CXUI_Manager::GetFrameCount()));
	BM::vstring vValue(wstrValue);

	if( ATTR_PRIORITY == wstrName )
	{
		Priority( __min(MAX_PRIORITY, static_cast<int>(vValue)) );	
	}
	else if( ATTR_ID == wstrName ){ID( wstrValue );	}

	else if( ATTR_BTN_TYPE == wstrName ){BtnType( static_cast<int>(vValue) );	}

	else if( ATTR_X == wstrName ){	Location( static_cast<int>(vValue), Location().y, 0);	}
	else if( ATTR_Y == wstrName ){	Location( Location().x, static_cast<int>(vValue), 0);	}
	else if( ATTR_Z == wstrName ){	Location( Location().x, Location().y, static_cast<int>(vValue) );	}
	else if( ATTR_W == wstrName ){	Size( static_cast<int>(vValue), Height() );	}
	else if( ATTR_H == wstrName ){	Size( Width(), static_cast<int>(vValue) );	}
	
	else if( ATTR_ALIGN_X == wstrName ){	AlignX( static_cast<int>(vValue) );	}
	else if( ATTR_ALIGN_Y == wstrName ){	AlignY( static_cast<int>(vValue) );	}
	else if( ATTR_UNIQUE_EXIST_TYPE == wstrName ){ UniqueExistType(wstrValue); }

	else if( ATTR_IS_ABSOLUTE_X == wstrName )	{ IsAbsoluteX( 0 != static_cast<int>( vValue) ); }
	else if( ATTR_IS_ABSOLUTE_Y == wstrName )	{ IsAbsoluteY( 0 != static_cast<int>( vValue) ); }

	else if( ATTR_INPUT_KEY_REAL == wstrName) 
	{
		if(0 != static_cast<int>(vValue))
		{
			m_bConvRealKey = true; 
		}
	}

	else if( ATTR_TEXT == wstrName )
	{
		if(wstrValue.size())
		{
			Text( XUI_TT(static_cast<int>(vValue)) );
		}
	}
	else if( ATTR_FONT == wstrName )
	{
		Font( wstrValue );
	}
	else if( ATTR_T_X == wstrName )
	{
		POINT2 pt = TextPos();
		pt.x = static_cast<int>(vValue);
	
		TextPos( pt );	
	}
	else if( ATTR_T_Y == wstrName )
	{
		POINT2 pt = TextPos();
		pt.y = static_cast<int>(vValue);
	
		TextPos( pt );	
	}

	else if( ATTR_T_BOLD == wstrName )
	{
		int const IsBold = (int)vValue;
		if( IsBold )
		{
			FontFlag( FontFlag() | XTF_BOLD );
		}
	}
	else if( ATTR_T_ITALIC == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			FontFlag( FontFlag() | XTF_ITALIC );
		}
	}
	else if( ATTR_T_UNDERLINE == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			FontFlag( FontFlag() | XTF_UNDERLINE );
		}
	}
	else if( ATTR_T_OUTLINE == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			FontFlag( FontFlag() | XTF_OUTLINE );
		}
	}
	else if( ATTR_T_CENTER == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			FontFlag( FontFlag() | XTF_ALIGN_CENTER );
		}
	}
	else if( ATTR_T_Y_CENTER == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			FontFlag( FontFlag() | XTF_ALIGN_Y_CENTER );
		}
	}
	else if(ATTR_T_RIGHT == wstrName)
	{
		bool bRight = (0==(int)vValue)? false: true;
		if(bRight)
		{
			FontFlag(FontFlag() | XTF_ALIGN_RIGHT);
		}
	}

	else if( ATTR_IMG == wstrName )
	{//	RscGuid( wstrValue );
		DefaultImgName(vValue);
	}
	else if( ATTR_IMG_X == wstrName ){	ImgPos( POINT2((int)vValue, ImgPos().y) );	}
	else if( ATTR_IMG_Y == wstrName ){	ImgPos( POINT2(ImgPos().x, (int)vValue) );	}

	else if( ATTR_IMG_W == wstrName ){	ImgSize( POINT2((int)vValue, ImgSize().y) );	}
	else if( ATTR_IMG_H == wstrName ){	ImgSize( POINT2(ImgSize().x, (int)vValue) );	}

	else if( ATTR_U == wstrName ){m_UVInfo.U = (int)vValue;}
	else if( ATTR_V == wstrName ){m_UVInfo.V = (int)vValue;}
	else if( ATTR_UV_INDEX == wstrName ){m_UVInfo.Index = (int)vValue;}
	else if( ATTR_TOOL_TIP_TEXT == wstrName ){	ToolTip( wstrValue );	}

	else if( ATTR_CAN_DRAG == wstrName )
	{
		CanDrag( 0 != static_cast<int>(vValue) );	
	}
	else if( ATTR_CAN_CLIP == wstrName )
	{
		CanClip( 0 != static_cast<int>(vValue) );	
	}
	else if( ATTR_ALIVE_TIME == wstrName )
	{
		AliveTime( static_cast<DWORD>(vValue) );
	}

	else if( ATTR_TICK_TIME == wstrName )
	{
		TickInterval( static_cast<DWORD>(vValue) );
	}

	else if( ATTR_ALIVE_TIME_IGNORE_MOUSE_OVER == wstrName )
	{
		IsAliveTimeIgnoreMouseOver( 0 != static_cast<int>(vValue) );
	}

	else if( ATTR_USE_DRAW_RECT == wstrName )
	{
		UseDrawRect( 0 != static_cast<int>(vValue) );
	}
	else if( ATTR_USE_ALPHA_ANI == wstrName )
	{
		if( static_cast<int>(vValue) )
		{
			IsUseAlphaAni(true);
		}
		else
		{
			IsUseAlphaAni(false);
			Alpha(1.0f);
		}
	}
	else if( ATTR_IS_ENABLE == wstrName )
	{
		Enable( 0 != static_cast<int>(vValue) );
	}
	else if(ATTR_IS_CLOSED == wstrName )
	{
		IsClosed( 0 != static_cast<int>(vValue) );
	}
	else if(ATTR_IS_VISIBLE == wstrName)
	{
		Visible( 0 != static_cast<int>(vValue) );
	}
	else if ( ATTR_CAN_COLOR_CHANGE == wstrName)
	{
		CanColorChange( 0 != static_cast<int>(vValue) );	
	}
	else if ( ATTR_COLOR_TYPE == wstrName)
	{
		ColorType( static_cast<int>(vValue) );
	}
	else if ( ATTR_IGNORE_PARENT_RECT == wstrName )
	{
		IsIgnoreParentRect( 0 != static_cast<int>(vValue) );	
	}
	else if ( ATTR_NO_WORD_WRAP == wstrName )
	{
		NoWordWrap( 0 != static_cast<int>(vValue) );	
	}
	else if ( ATTR_FONT_COLOR == wstrName )
	{
		FontColor( static_cast<int>(vValue) );
	}
	else if( ATTR_FONT_COLOR_HEX == wstrName)
	{
		std::wstring kTemp = vValue;
		int iHexValue = 0;
		_stscanf_s(kTemp.c_str(), _T("%x"), &iHexValue);
		FontColor(iHexValue);
	}
	else if( ATTR_OUTLINE_COLOR_HEX == wstrName )
	{
		std::wstring kTemp = vValue;
		int iHexValue = 0;
		_stscanf_s(kTemp.c_str(), _T("%x"), &iHexValue);
		OutLineColor(iHexValue);
	}
	else if(ATTR_ALPHA == wstrName)
	{
		AlphaMax( static_cast<int>(vValue)/fHexFull );
	}

	else if( ATTR_USE_OFFSCREEN == wstrName )
	{
		UseOffscreen( 0 != static_cast<int>(vValue) );
	}

	else if(ATTR_PASS_EVENT == wstrName)
	{
		PassEvent( 0 != static_cast<int>(vValue) );
	}

	else if( ATTR_EFFECT == wstrName )
	{
		int iValue = static_cast<int>(vValue);
		AppearEffect( static_cast<E_AppearEffect>(iValue) );
	}

	else if( ATTR_SPEED == wstrName )
	{
		int iValue = static_cast<int>(vValue);
		EffectSpeed( static_cast<unsigned char>(iValue) );
	}
	else if ( ATTR_NO_FOCUS == wstrName )
	{
		NoFocus( 0 != static_cast<int>(vValue) );
	}
	else if ( ATTR_TYPING_EFFECT == wstrName )
	{
		TypingEffect( 0 != static_cast<int>(vValue) );
	}
	else if (ATTR_IGNORE_TWINKLE == wstrName)
	{
		IgnoreTwinkle( 0 != static_cast<int>(vValue) );
	}
	else if(ATTR_EDIT_DONT_LOSTFOCUS == wstrName)
	{
		IsDoNotLostFocus( 0 != static_cast<int>(vValue) );
	}
	else if (ATTR_HIDE_OTHER == wstrName)
	{
		HideOtherWnd( 0 != static_cast<int>(vValue) );
	}
	else if (ATTR_NOT_HIDE == wstrName)
	{
		NotHideWnd( 0 != static_cast<int>(vValue) );
	}
	else if (ATTR_CALL_SOUND == wstrName)
	{
		CallSoundID(vValue);
	}
	else if (ATTR_CLOSE_SOUND == wstrName)
	{
		CloseSoundID(vValue);
	}
	else if (ATTR_USE_INPUTTIME_CHECK== wstrName)
	{
		UseInputCheck( 0 != static_cast<int>(vValue) );
	}
	else if (ATTR_SCALE_CENTER_X == wstrName)
	{
		m_kScaleCenter.x = static_cast<float>(vValue);
		if(0.0f > m_kScaleCenter.x)
		{
			m_kScaleCenter.x = 0.0f;
		}
		else if(1.0f < m_kScaleCenter.x)
		{
			m_kScaleCenter.x = 1.0f;
		}
	}
	else if (ATTR_SCALE_CENTER_Y  == wstrName)
	{
		m_kScaleCenter.y = static_cast<float>(vValue);
		if(0.0f > m_kScaleCenter.y)
		{
			m_kScaleCenter.y = 0.0f;
		}
		else if(1.0f < m_kScaleCenter.y)
		{
			m_kScaleCenter.y = 1.0f;
		}
	}
	else if (ATTR_ROTATION_CENTER_X == wstrName)
	{
		m_kRotationCenter.x = static_cast<float>(vValue);
		if(0.0f > m_kRotationCenter.x)
		{
			m_kRotationCenter.x = 0.0f;
		}
		else if(1.0f < m_kRotationCenter.x)
		{
			m_kRotationCenter.x = 1.0f;
		}
	}
	else if (ATTR_ROTATION_CENTER_Y == wstrName)
	{
		m_kRotationCenter.y = static_cast<float>(vValue);
		if(0.0f > m_kRotationCenter.y)
		{
			m_kRotationCenter.y = 0.0f;
		}
		else if(1.0f < m_kScaleCenter.y)
		{
			m_kRotationCenter.y = 1.0f;
		}
	}
}

CXUI_Wnd* CXUI_Wnd::GetOffscreenOwner()
{
	if (UseOffscreen() == false || Offscreen() == false)
		return NULL;

	if (UseParentOffscreen() == false)
		return this;

	if (Parent() == NULL)
	{
		assert(UseParentOffscreen() == false);
		return this;
	}

	return Parent()->GetOffscreenOwner();
}

void* CXUI_Wnd::GetParentOffscreen()
{
	CXUI_Wnd *pParent = Parent();
	while (pParent && pParent->UseOffscreen() && pParent->Offscreen() && pParent->UseParentOffscreen())
	{
		pParent = pParent->Parent();
	}

	if (pParent)
	{
		assert(UseOffscreen());
		assert(pParent->Offscreen());
		assert(pParent->UseParentOffscreen() == false);
		return pParent->Offscreen();
	}

	return NULL;
}

void CXUI_Wnd::CalcOffscreenLocation() {}
POINT3I CXUI_Wnd::OffscreenLocation()
{
	if (UseParentOffscreen() == false)
	{
		//m_OffscreenLocation = POINT3I(0, 0, 0);
		//return;
		return POINT3I(0, 0, 0);
	}

	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CalcOffscreenLocation"), CXUI_Manager::GetFrameCount()));
	POINT3I pt = Location();
	CXUI_Wnd *pParent = Parent();
	while ( pParent && pParent->UseOffscreen() && pParent->Offscreen() && pParent->UseParentOffscreen() && Offscreen() == pParent->Offscreen() )
	{
		pt += pParent->Location();
		pParent = pParent->Parent();
	}
	//m_OffscreenLocation = pt;
	return pt;
}

#ifdef XUI_LOCATION_RECT_CALC_ONTICK
POINT3I CXUI_Wnd::GetTotalLocation()
#else
POINT3I CXUI_Wnd::TotalLocation()
#endif
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("GetTotalLocation"), CXUI_Manager::GetFrameCount()));
	POINT3I pt = Location();
	CXUI_Wnd *pParent = Parent();
	while( pParent )
	{
		pt += pParent->Location();
		pParent = pParent->Parent();
	}
	return pt;
}

void CXUI_Wnd::VScale(float const fScale)
{
	Scale(fScale);
	Location( static_cast<int>(Location().x*fScale), static_cast<int>(Location().y*fScale));
	Size( static_cast<int>(Width() * fScale), static_cast<int>(Height() * fScale) );

	for_each( m_contControls.begin(), m_contControls.end(), ScaleControl_Func(fScale) );
}

void CXUI_Wnd::VAlign()
{
	int align_x = AlignX();//	1 UP 2 CEN 3 LO
	int align_y = AlignY();

	POINT2 pt = Location();

	if( 0 <= align_x )
	{
		pt.x = ALIGN( Parent() ? Parent()->Width() : m_sptResolutionSize.x, Width(), align_x );
		Location( pt );
	}
	if( 0 <= align_y )
	{
		pt.y = ALIGN( Parent() ? Parent()->Height() : m_sptResolutionSize.y, Height(), align_y );
		Location( pt );
	}
}

void CXUI_Wnd::VInit()
{
	if (CanColorChange() && ColorType() > 0)
	{
		SetColor(CXUI_Manager::GetColor(ColorType()));
		ColorChange(true);
	}
	//m_HideList.clear();
	for_each( m_contControls.begin(), m_contControls.end(), InitControl_Func() );
}

DWORD CXUI_Wnd::GetStateColor()const
{
	if( IsMouseDown() )
	{
		return 0xFF0000FF;//G R 을 빼니까
	}

	if( IsMouseOver() )
	{
		return 0xFF00FFFF;//G R 을 빼니까
	}

	return 0xFFFFFFFF;
}

bool CXUI_Wnd::CheckEventRepeate(int const iEvent)
{
	if(!UseInputCheck() || 0>=EventRepeateDelay() || iEvent == IEI_MS_MOVE || iEvent == IEI_KEY_DOWN_PRESS)
	{
		return true;
	}

	bool bRet = true;

	DWORD const dwTime = BM::GetTime32();

	SCRIPT_TIME_MAP::iterator time_it = m_kScriptTimeMap.find(iEvent);

	if(m_kScriptTimeMap.end() != time_it)	//한번 실행했던 넘이면
	{
		bRet = (EventRepeateDelay() < (dwTime - (*time_it).second));
		if(bRet)
		{
			(*time_it).second = dwTime;	//갱신
		}
	}
	else
	{
		m_kScriptTimeMap.insert(std::make_pair(iEvent, dwTime));
	}

	return bRet;
}
bool	CXUI_Wnd::CheckTextClick(int iButton,POINT2 const &pt,bool bIsDoubleClick)
{
	if(!m_pTextClickCallBackFunc)
	{
		return	false;
	}

	if(m_pText)
	{
		PgExtraDataPackInfo	const	&kDataPackInfo = m_pText->GetText().GetExtraDataPackInfo();

		if(kDataPackInfo.GetExtraDataPackCount() == 0)
		{
			return	false;
		}

		XUI::PgParsedChar	kFoundChar(0,0,0,-1);
		if(m_pText->GetParsedCharAt(pt.x,pt.y,kFoundChar))
		{

			PgExtraDataPack	const	*pkExtraDataPack = kDataPackInfo.GetExtraDataPack(kFoundChar.m_iExtraDataPackIndex);
			if(!pkExtraDataPack)
			{
				return	false;
			}

			return	m_pTextClickCallBackFunc(this,iButton,Text(),*pkExtraDataPack,bIsDoubleClick);
		}
	}

	return	false;
}
bool CXUI_Wnd::VPeekEvent_Default(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)//Wnd기본 메시지 처리
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VPeekEvent_Default"), CXUI_Manager::GetFrameCount()));
	/*
	if(rET == IEI_KEY_DOWN && CXUI_Edit::m_spFocusedEdit && VType() != E_XUI_EDIT)
	{//키보드 다운 이벤트인데, 포커스가 에디트에 있는데, 에디트가 아니면
		return false;
	}*/

	if( !Visible() )
	{
		return false;
	}

	bool bRet = false;
	DBLClick(false);
#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	unsigned int wstrScriptKey = SCRIPT_MAX_NUM;
#else
	std::wstring wstrScriptKey;
#endif

	switch(rET)
	{
	case IEI_KEY_DOWN:
//	case IEI_KEY_UP:
		{//컨트롤에 전달 되어야함. dxxx todo 
			if( Enable() )
			{
				BM::vstring vstr(dwValue);
				bRet = DoHotKey(vstr);
				if(!bRet)
				{
					return false;
				}
				return true;
			}
		}break;
	case IEI_MS_DOWN:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				if( Enable() )
				{
					if(!CheckEventRepeate(static_cast<int>(rET)))
					{
						IsMouseDown(false);
						return false;
					}
					if(m_spConfig)
					{
						switch(VType())
						{
						case E_XUI_BTN:
						case E_XUI_CHECKBTN:
							{//버튼만 사운드 나옴.
								std::wstring const *pFileName = NULL;
								if(m_spConfig->SndInfo( BtnType(), (E_MOUSE_EVENT_INDEX)dwValue, pFileName))
								{
									if(pFileName)
									{
										m_spMediaPlayer->PlaySound(*pFileName);
									}
								}
							}break;
						}
					}

					if(MEI_BTN_0 == dwValue)
					{
						VAcquireFocus(this);
						IsMouseDown(true);
						//CXUI_Wnd::m_spWndMouseFocus = this;

						static DWORD dwLastDownTime = 0;
						DWORD const dwNow = BM::GetTime32();
						
						if( LastMouseDownPos()-XUIMgr.DblClickBound()<=m_sMousePos && LastMouseDownPos()+XUIMgr.DblClickBound()>=m_sMousePos )
						{//같은 좌표에서.
							if(dwLastDownTime)
							{
								//있었고 //일정 시간 내에 들어오면.
								if((dwNow - dwLastDownTime) < XUIMgr.DblClickTick())//200 밀리 이하로 들어왔다..
								{
									wstrScriptKey = SCRIPT_ON_L_BTN_DBL_DOWN;
									dwLastDownTime = 0;//이벤트후 시간은 0으로 만들어야하고.
									bRet = true;
									DBLClick(true);
									break;
								}
							}
						}

						dwLastDownTime = dwNow;//뭐가 됐든 마지막 다운시간은 기록

						wstrScriptKey = SCRIPT_ON_L_BTN_DOWN;
						
						LastMouseDownPos(m_sMousePos);
						if ( !ReservTwinkleOff() && (E_XUI_BTN!=VType()) )
						{
							ReservTwinkleOff(true);
						}
					}
					else if( MEI_BTN_1 == dwValue )
					{ 
						wstrScriptKey = SCRIPT_ON_R_BTN_DOWN;
					}

					if(CheckTextClick(dwValue,m_sMousePos,DBLClick()))
					{
						return	true;
					}

				}
				bRet = true;
			}
			else
			{
				IsMouseDown(false);
			}
		}break;
	case IEI_MS_UP:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				if(!CheckEventRepeate(static_cast<int>(rET)))
				{
					return true;
				}
				if( Enable() )
				{
					if(MEI_BTN_0 == dwValue)
					{
						IsMouseDown(false);	
						wstrScriptKey = SCRIPT_ON_L_BTN_UP; 
					}
					else if( MEI_BTN_1 == dwValue )
					{ 
						wstrScriptKey = SCRIPT_ON_R_BTN_UP;
					}
				}
				bRet = true;
			}
			else
			{
				if(MEI_BTN_0 == dwValue)
				{
					IsMouseDown(false);	
				}
			}

	
		}break;
	case IEI_MS_MOVE:
		{
			bool const bIsBeforeMouseOver = IsMouseOver();
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				if( Enable() && false==NoFocus() )
				{
					m_bIsOverWnd = true;
					bRet = true;
					if(m_spWndMouseOver != this)
					{
						if(m_spWndMouseOver)
						{
							m_spWndMouseOver->DoScript(SCRIPT_ON_MOUSE_OUT);//이전 마우스 오버에서 아웃 처리.
							m_spWndMouseOver->IsMouseDown(false);
						}

						m_spWndMouseOver = this;// 마우스 올라왔어.

						if(m_spConfig)
						{
							switch(VType())
							{
							case E_XUI_BTN:
							case E_XUI_CHECKBTN:
								{//버튼만 사운드 나옴.
									std::wstring const *pFileName = NULL;
									if(m_spConfig->SndInfo( BtnType(), MEI_X, pFileName))
									{
										if(pFileName)
										{
											m_spMediaPlayer->PlaySound(*pFileName);
										}
									}
								}break;
							}
						}

						wstrScriptKey = SCRIPT_ON_MOUSE_OVER;
					}

					if(IsMouseDown())
					{
						Drag(GetDragableWnd(), rPT);
						//POINT3I ptOrg = Location();
						//Location( ptOrg+rPT );
					}
					
					if(0!=rPT.z/* && ContainsPoint(m_sMousePos)*/) // 마우스 휠이다.
					{
						if(rPT.z > 0){ wstrScriptKey = SCRIPT_ON_WHEEL_UP; }
						else{ wstrScriptKey = SCRIPT_ON_WHEEL_DOWN; }
						//SetCustomData(&rPT.z, sizeof(rPT.z));
						bRet = false;
					}
					WheelValue(rPT.z);
				}
			}
			else if (m_spWndMouseOver == this)	// 마우스는 포함 안됬는데 m_spWndMouseOver는 이 윈도우일때
			{
				m_spWndMouseOver->DoScript(SCRIPT_ON_MOUSE_OUT);//이전 마우스 오버에서 아웃 처리.
				m_spWndMouseOver->IsMouseDown(false);
			}
			else
			{
				IsMouseDown(false);
			}
		}break;
	default:
		{
			return true;
		}break;
	}

	bool const bScriptRet = DoScript( wstrScriptKey );//실행 안되는 현상을 방지하기 위함.

	return (PassEvent())? FALSE: (bRet || bScriptRet);
}
bool CXUI_Wnd::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)//이벤트는 키 UP으로만 동작.
{//좌표 및 기타
	if( !Visible() || IsClosed() )
	{
		IsMouseDown(false);
		return false;
	}//자식도 안함.
	if( !Enable() )
	{
		IsMouseDown(false);
		return false;
	}//자식도 안함.

	//키 이벤트는 에디트에 한해서 중복 허용
	//타 오브젝트는 중복 불허가

	DWORD dwReal = dwValue;

	//리얼키옵션이 켜있다면 리얼키로 전환
	if(m_bConvRealKey && (IEI_KEY_DOWN==rET))
	{
		dwReal = m_spHotKeyToRealKey(dwValue);
	}

	//모든 컨트롤에 이벤트메시지 처리 권한 부여
	CONT_CONTROL::iterator kItor = m_contControls.begin();
	while(m_contControls.end() != kItor)
	{
		if( (*kItor)->VPeekEvent(rET, rPT, dwReal) )
		{
			return true;
		}
		++kItor;
	}

	return VPeekEvent_Default(rET, rPT, dwReal);
}

bool Location_Greater(XUI::CXUI_Wnd* pLeft, XUI::CXUI_Wnd* pRight)
{
	return pLeft->Location().z > pRight->Location().z;
}

bool CXUI_Wnd::AddControl( CXUI_Wnd* pControl )
{
	if (!pControl)
	{
		return false;
	}
	if( !GetControl( pControl->ID() ) )
	{
		//>>Inheritance attribute
		pControl->IsUseAlphaAni(IsUseAlphaAni());//알파애니 상속
		//<<Inheritance attribute

		pControl->Parent(this);//컨트롤의 부모를 등록
		pControl->DoScript(SCRIPT_ON_CREATE);//생성타임에 불림
		pControl->VAlign();
		m_contControls.push_back(pControl);
		m_contControls.sort(Location_Greater);//자식 z 값으로 순서 정렬: -1==뒤 < 0==배경 < n==앞, Container에는 z가 높은 녀석부터 들어간다.
		pControl->VOnLocationChange();
		SetInvalidate();
		return true;
	}

	assert(NULL&&"Duplicated Contorl ID");
	__asm int 3;// 절대 발생하면 안됨
	return false;
}

void CXUI_Wnd::Close(bool const bDelUniqueUIType)
{
	if( bDelUniqueUIType )
	{
		DeleteExistUniqueListType( UniqueExistType() );
	}
	Visible( false );//	
	IsClosed( true );//
	LastUsedTime(BM::GetTime32());
	VOnClose();
	DoScript(SCRIPT_ON_CLOSE);
	if(m_spMediaPlayer)
	{
		m_spMediaPlayer->PlaySoundByID(CloseSoundID());
	}
	SAFE_DELETE(m_pText);
}

void CXUI_Wnd::SortChildGreater()
{
	m_contControls.sort(Location_Greater);//자식 z 값으로 순서 정렬: -1==뒤 < 0==배경 < n==앞, Container에는 z가 높은 녀석부터 들어간다.
}


bool CXUI_Wnd::GetControlList_ByType(E_XUI_TYPE const nControlType, std::vector<CXUI_Wnd*> &rkVec)
{
	CONT_CONTROL::iterator kIter = m_contControls.begin();
	while(m_contControls.end() != kIter)
	{
		CXUI_Wnd *pkWnd = *kIter;
		if(nControlType == pkWnd->VType())
		{
			rkVec.push_back(pkWnd);
		}
		++kIter;
	}

	if(0 != rkVec.size())
	{
		return true;
	}
	return false;
}

CXUI_Wnd* CXUI_Wnd::GetControl( ID_TYPE const& ID )
{
	std::wstring wstrID = ID;

	UPR(wstrID);

	CONT_CONTROL::iterator itor =  m_contControls.begin();
	while( m_contControls.end() != itor)
	{
		if( (*itor) && (*itor)->ID() == wstrID )
		{
			return (*itor);
		}
		++itor;
	}
	return NULL;
}

CXUI_Wnd* CXUI_Wnd::GetControl( ID_TYPE const& ID, E_XUI_TYPE const nControlType )
{
	CONT_CONTROL::value_type pControl = GetControl(ID);
	if( pControl && pControl->VType() == nControlType )
	{
		return pControl;
	}
	return NULL;
}

CXUI_Wnd* CXUI_Wnd::GetControlAtPoint( POINT2 const& pt )
{//좌표 긁기는 앞에서 부터 함 (add child 시마다 z값으로 소팅한다)
	CONT_CONTROL::reverse_iterator kItor =  m_contControls.rbegin();
	while( m_contControls.rend() != kItor )
	{
		CONT_CONTROL::value_type &rmapped= (*kItor);
		if( rmapped->ContainsPoint(pt) )
		{
			return rmapped;
		}
		++kItor;
	}
	return NULL;
}

/*! 
	\brief 컨트롤러를 제거한다.
    \param ID 제거할 컨트롤러의 식별자.
*/

void CXUI_Wnd::RemoveControl( ID_TYPE const& ID )
{
	CONT_CONTROL::iterator itor =  m_contControls.begin();
	while( m_contControls.end() != itor)
	{
		if( (*itor)->ID() == ID )
		{
			(*itor)->RemoveAllControls();
			SAFE_DELETE((*itor));
			m_contControls.erase(itor);
			break;
		}
		++itor;
	}
}

/*! 
	\brief 모든 자식 컨트롤러들을 제거하고 메모리를 해제한다.
    \param 
	\last update 2006.12.20 by 하승봉
	\WARNING!! preset은 해제하면 안됨!!
*/
void CXUI_Wnd::RemoveAllControls(bool const bIsClosed)
{
	if( m_contControls.size() )
	{
		for_each( m_contControls.begin(), m_contControls.end(), RemoveControl_Func(bIsClosed) );
		for_each( m_contControls.begin(), m_contControls.end(), SafeDeleterControl_Func() );

		/*CONT_CONTROL::iterator itor =  m_contControls.begin();
		while( m_contControls.end() != itor )
		{
			(*itor)->RemoveAllControls();
			++itor;
		}

		itor =  m_contControls.begin();
		while( m_contControls.end() != itor )		
		{
			if((*itor)->VType() != E_XUI_DIALOG)		//수정 중단됨
			{
				SAFE_DELETE((*itor));
			}

			++itor;
		}*/
		m_contControls.clear();
	}
}

void CXUI_Wnd::VRefresh()
{
	CONT_CONTROL::iterator itor = m_contControls.begin();
	while( m_contControls.end() != itor)
	{
		(*itor)->VRefresh();
		++itor;
	}

	if (UseOffscreen() && Offscreen())
	{
		if (UseParentOffscreen())
		{
			Offscreen(NULL);
		}
		else
		{
			m_spRscMgr->ReleaseRsc(m_pOffscreen);
			m_pOffscreen = NULL;
		}
		Invalidate(true);
	}
}

bool CXUI_Wnd::VRegistChild( CXUI_Wnd *pWnd )
{
	if(pWnd)
	{
		return AddControl( pWnd );
	}

	assert(NULL);
	__asm int 3;// 절대 발생하면 안됨.
	return false;
}

void CXUI_Wnd::DisplayControl()
{
	for_each( m_contControls.begin(), m_contControls.end(), DisplayControl_Func() );
}

void CXUI_Wnd::PeekEventControl(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	for_each( m_contControls.begin(), m_contControls.end(), PeekEventControl_Func(rET, rPT, dwValue) );
}

bool CXUI_Wnd::VOnTick( DWORD const dwCurTime )
{
	if ( IsClosed() )
	{
		IsMouseDown(false);
		Visible(false);
		return false;
	}

	if ( !Visible() )
	{
		return false;
	}

	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VOnTick.CloseCheck"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerB((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VOnTick.LocationCalc"), CXUI_Manager::GetFrameCount()));
	XUI_STAT(XUIStatTimerF timerC((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("VOnTick.OnTickScript"), CXUI_Manager::GetFrameCount()));

	XUI_STAT(timerA.Start());
	if(AliveTime())//마우스가 벗어나야 닫힘.
	{
		if(	IsAliveTimeIgnoreMouseOver() //타임을 무시하거나.
		||	!IsMouseOver())//마우스가 바깥이네.
		{
			if( CalledTime() + AliveTime() < dwCurTime)
			{
				DoScript(SCRIPT_ON_ALIVE_TIME_END);
				if( Parent() )
				{
					AliveTime(0);
					Visible(false);
				}
				else
				{
					Close();
				}
				return true;
			}
		}
	}
	XUI_STAT(timerA.Stop());

#ifdef XUI_LOCATION_RECT_CALC_ONTICK
	XUI_STAT(timerB.Start());
	TotalLocation(GetTotalLocation());
	CalcParentRect(m_kParentRect);
	CalcParentDrawRect(m_kParentDrawRect);
	XUI_STAT(timerB.Stop());
#endif

	if ( TickInterval() > ( dwCurTime - GetLastTickTime() ) )
	{
		return false;
	}

	UpdateTwinkle( dwCurTime );
	
	CONT_CONTROL::iterator itor = m_contControls.begin();
	while( m_contControls.end() != itor)
	{
		(*itor)->VOnTick( dwCurTime );
		++itor;
	}

	XUI_STAT(timerC.Start());
	DoScript( SCRIPT_ON_TICK );
	XUI_STAT(timerC.Stop());
	m_dwLastTickTime = TickInterval() ? (m_dwLastTickTime + TickInterval()) : dwCurTime;
	return true;
}

void CXUI_Wnd::UpdateTwinkle( DWORD const dwCurTime )
{
	if (IsTwinkle())
	{
		if (ReservTwinkleOff())
		{
			ReservTwinkleOff(false);
			OffTwinkle();
		}
		else
		{
			DWORD const iTwinkleDelta = dwCurTime - TwinkleStartTime();
			if ( TwinkleTime() > iTwinkleDelta )
			{
				bool const bOn = 0 == ((iTwinkleDelta/TwinkleInterTime()) % 2);// true == 0, false != 0
				NowTwinkleOn(bOn);
			}
			else
			{
				SetTwinkle(false);
				NowTwinkleOn(false);
			}
		}
	}
}

void CXUI_Wnd::DisplayToolTip()
{
//	static 으로 할까?? 해서 텍스트가 변경되고 좌표가 변경 되도록 해주어야 함.
//	RenderText( ToolTip()
	if(ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
	{
			
	}
}
void CXUI_Wnd::RenderText(POINT2 const& pt, CXUI_Style_String const& kText, DWORD const dwTextFlag, bool bDrawOffscreen)//프리셋 마다 함수를 갈라야 됨.
{
	if(kText.Length()>0)
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("RenderText"), CXUI_Manager::GetFrameCount()));
		SRenderTextInfo kRenderTextInfo;

		//kRenderTextInfo.dwColor = FontColor();
		kRenderTextInfo.dwOutLineColor = OutLineColor();
		if (bDrawOffscreen)
			kRenderTextInfo.fAlpha = AlphaMax();
		else
			kRenderTextInfo.fAlpha = Alpha();
		kRenderTextInfo.kLoc = pt;
		GetParentDrawRect(kRenderTextInfo.rcDrawable);
		kRenderTextInfo.wstrText = kText.GetOriginalString();
		kRenderTextInfo.wstrFontKey = Font();
		
		kRenderTextInfo.dwTextFlag = dwTextFlag;

		if(m_pText == NULL)
		{
			m_pText = g_kFontMgr.CreateNew2DString(kText);
			TypingEffectReset();
		}
		else
		{
			m_pText->SetText(kText);
		}


		kRenderTextInfo.m_p2DString = m_pText;

		m_spRenderer->RenderText(kRenderTextInfo);
	}
}

void CXUI_Wnd::RenderText(POINT2 const& pt, std::wstring const& wstrText, std::wstring const& wstrFont, DWORD const dwTextFlag, bool bDrawOffscreen)//프리셋 마다 함수를 갈라야 됨.
{
	if( wstrText.length()  )
	{
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("RenderText"), CXUI_Manager::GetFrameCount()));
		SRenderTextInfo kRenderTextInfo;

		//kRenderTextInfo.dwColor = FontColor();
		kRenderTextInfo.dwOutLineColor = OutLineColor();
		if (bDrawOffscreen)
			kRenderTextInfo.fAlpha = AlphaMax();
		else
			kRenderTextInfo.fAlpha = Alpha();
		kRenderTextInfo.kLoc = pt;
		GetParentDrawRect(kRenderTextInfo.rcDrawable);
		kRenderTextInfo.wstrText = wstrText;
		kRenderTextInfo.wstrFontKey = wstrFont;
		
		kRenderTextInfo.dwTextFlag = dwTextFlag;

		CXUI_Font	*pFont = g_kFontMgr.GetFont(wstrFont);
		if( pFont
		&&	!m_pText )
		{
			m_pText = g_kFontMgr.CreateNew2DString(m_kStyleText);
			TypingEffectReset();
		}
		
		if( pFont
		&&	m_pText )
		{
			m_pText->SetText(m_kStyleText);
		}

		kRenderTextInfo.m_p2DString = m_pText;

		m_spRenderer->RenderText(kRenderTextInfo);
	}
}
/*
bool CXUI_Wnd::GetValuei(EValuetype const eVT, int &riValue)
{
	switch(eVT)
	{
	case VT_X:
		{
			riValue = Location().x;
		}break;
	case VT_Y:
		{
			riValue = Location().y;
		}break;
	case VT_T_X:
		{
			riValue = TextPos().x;
		}break;
	case VT_T_Y:
		{
			riValue = TextPos().y;
		}break;
	case VT_UV_INDEX:
		{
			riValue = (int)m_UVInfo.Index;
		}break;
	case VT_IMG_W:
		{
			riValue = ImgSize().x;
		}break;
	case VT_IMG_H:
		{
			riValue = ImgSize().y;
		}break;
	case VT_ALPHA:
		{
			riValue = AlphaMax()*fHexFull;
		}break;
	default:
		return false;
	}
	return true;
}

bool CXUI_Wnd::SetValuei(EValuetype const eVT, int &riValue)
{
	switch(eVT)
	{
	case VT_X:
		{
			Location( POINT2( riValue, Location().y ));
		}break;
	case VT_Y:
		{
			Location( POINT2( Location().x, riValue ));
		}break;
	case VT_T_X:
		{
			TextPos( POINT2( riValue, TextPos().y ));
		}break;
	case VT_T_Y:
		{
			TextPos( POINT2( TextPos().x, riValue ) );
		}break;
	case VT_UV_INDEX:
		{
			m_UVInfo.Index = riValue;
		}break;
	case VT_IMG_W:
		{
			POINT2 ptOrgSize = ImgSize();
			ptOrgSize.x = riValue;
			ImgSize(ptOrgSize);
		}break;
	case VT_IMG_H:
		{
			POINT2 ptOrgSize = ImgSize();
			ptOrgSize.y = riValue;
			ImgSize(ptOrgSize);
		}break;
	case VT_ALPHA:
		{
			AlphaMax(riValue/fHexFull);
		}
	default:
		return false;
	}
	return true;
}

bool CXUI_Wnd::GetValue(EValuetype const eVT, std::wstring &wstrOut)
{
	int iVal = 0;

	if(this->GetValuei(eVT, iVal))//상속 함수를 불러야 합니다.
	{
		BM::vstring kVal = iVal;
		wstrOut = (std::wstring)kVal;
		return true;
	}

	switch(eVT)
	{
	case VT_STATIC_TEXT:
		{
			wstrOut = Text();
		}break;
	}

	return false;
}

bool CXUI_Wnd::SetValue(EValuetype const eVT, std::wstring const& wstrValue)
{
	BM::vstring vValue(wstrValue);

	switch(eVT)
	{
	case VT_STATIC_TEXT:
		{
			Text(wstrValue);
		}break;
	default:
		{
			int iVal = (int)vValue;
			return this->SetValuei(eVT, iVal);//상속 함수를 불러야 합니다.
		}break;
	}

	return true;
}
*/
bool CXUI_Wnd::DoScript( unsigned int const scriptId )
{
	if (m_pScriptFunc == NULL || scriptId >= SCRIPT_MAX_NUM)
		return false;

	if (m_bScriptArray[scriptId])
	{
		XUI_STAT(std::string statName = "DoScript" + g_kScriptNames[scriptId]);
		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo(statName), CXUI_Manager::GetFrameCount()));
#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
		bool bRet = m_pScriptFunc(MB(m_scriptArray[scriptId]), this);
		return bRet;
#endif
	}
	
	//SCRIPT_HASH::const_iterator kIter = m_mapScript.find(g_kScriptNames[scriptId]);
	//if(m_mapScript.end() != kIter)
	//{
	//	TCHAR buf[256];
	//	_stprintf(buf, 256, _T("%s DoScript %d is not exist\n"), ID().c_str(), scriptId);
	//	OutputDebugString(buf);
	//}
	//else
	//{
	//	TCHAR buf[256];
	//	_stprintf(buf, 256, _T("%s DoScript %s is not exist in mapScript\n"), ID().c_str(), g_kScriptNames[scriptId]);
	//	OutputDebugString(buf);
	//}
	return false;
}

bool CXUI_Wnd::DoHotKey( SCRIPT_HASH::key_type const& rScriptKey )
{
	std::wstring wstrScriptKey = rScriptKey;

	if(m_pScriptFunc)
	{
		if( m_spHotKeyModifier->GetPushModifierState() )
		{
			wstrScriptKey = m_spHotKeyModifier->GetModifierName() + wstrScriptKey;
		}

		XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("DoHotKey"), CXUI_Manager::GetFrameCount()));
		SCRIPT_HASH::const_iterator kIter = m_mapScript.find(wstrScriptKey);
		if(m_mapScript.end() != kIter)
		{
			if(!CheckEventRepeate(atoi(MB(rScriptKey))))
			{
				return true;
			}
			bool bRet = m_pScriptFunc(MB(kIter->second), this);
			return bRet;
		}
	}

	//bool bFound = false;
	//for (unsigned int i = 0; i < SCRIPT_MAX_NUM; i++)
	//{
	//	if (rScriptKey == g_kScriptNames[i])
	//	{
	//		TCHAR buf[256];
	//		_stprintf(buf, 256, _T("%s DoScript %s, in scriptArray %d\n"), ID().c_str(), rScriptKey.c_str(), i);
	//		OutputDebugString(buf);
	//		bFound = true;
	//		break;
	//	}
	//}

	return false;
}

void CXUI_Wnd::operator = ( const CXUI_Wnd &rhs )
{
	m_pParent = rhs.m_pParent;//내리면 Location 에서 꼬임

	Enable(rhs.Enable());
	Location( rhs.Location() );
	Size( rhs.Size() );
	TextPos( rhs.TextPos() );
	Font(rhs.Font());
	ToolTip( rhs.ToolTip() );

	ID( rhs.ID() );
	CanDrag( rhs.CanDrag() );
	CanClip( rhs.CanClip() );
	Visible( rhs.Visible() );

	IsAbsoluteX( rhs.IsAbsoluteX() );
	IsAbsoluteY( rhs.IsAbsoluteY() );

	FontFlag( rhs.FontFlag() );
	FontColor( rhs.FontColor() );
	AlignX( rhs.AlignX() );
	AlignY( rhs.AlignY() );
	ImgPos(rhs.ImgPos());
	ImgSize(rhs.ImgSize());

	Alpha( rhs.Alpha() );
	AlphaMax( rhs.AlphaMax() );

	Text( rhs.Text() );
	m_kStyleText = rhs.StyleText();
	UVInfo( rhs.UVInfo() );
	LastMouseDownPos( rhs.LastMouseDownPos() );

	DefaultImgName(rhs.DefaultImgName());
	//DefaultImg( m_spRscMgr->GetRsc( DefaultImgName() ) );//윗줄 함수 내에서 해줌
	ImgIdx(rhs.ImgIdx());

	OwnerState(rhs.OwnerState());

	BtnType(rhs.BtnType());

	CanColorChange(rhs.CanColorChange());
	ColorType(rhs.ColorType());
	SetColorSet();
	NoWordWrap(rhs.NoWordWrap());
	OutLineColor(rhs.OutLineColor());
	PassEvent(rhs.PassEvent());
	IsIgnoreParentRect(rhs.IsIgnoreParentRect());
	InvertBold(rhs.InvertBold());
	WheelValue(rhs.WheelValue());
	Scale(rhs.Scale());
	m_kScaleCenter = rhs.m_kScaleCenter;
	m_fRotationDeg = rhs.m_fRotationDeg;
	m_kRotationCenter = rhs.m_kRotationCenter;
	HideOtherWnd(rhs.HideOtherWnd());
	NotHideWnd(rhs.NotHideWnd());
	CallSoundID(rhs.CallSoundID());
	CloseSoundID(rhs.CloseSoundID());
//	m_kHotKeyHash = rhs.m_kHotKeyHash;
	m_mapScript = rhs.m_mapScript;
	//m_scriptArray = rhs.m_scriptArray;
	for (unsigned int i = 0; i < SCRIPT_MAX_NUM; i++)
	{
		m_bScriptArray[i] = rhs.m_bScriptArray[i];
		m_scriptArray[i] = rhs.m_scriptArray[i];
	}

	TickInterval( rhs.TickInterval() );

//	m_rcBoundingBox = rhs.m_rcBoundingBox; //Size 함수에서 바운드 박스 들어감.
//	m_contControls = rhs.m_contControls;//아래가 컨트롤 복사
	{
		CONT_CONTROL::const_iterator itor = rhs.m_contControls.begin();
		while(itor != rhs.m_contControls.end())
		{
			CXUI_Wnd* pkCloneChild = (*itor)->VClone();
			VRegistChild(pkCloneChild);
			++itor;
		}
	}

	m_bIsTwinkle = rhs.m_bIsTwinkle;
	NowTwinkleOn(rhs.NowTwinkleOn());
	TwinkleStartTime(rhs.TwinkleStartTime());
	TwinkleTime(rhs.TwinkleTime());
	TwinkleInterTime(rhs.TwinkleInterTime());
	ReservTwinkleOff(rhs.ReservTwinkleOff());
	IgnoreTwinkle(rhs.IgnoreTwinkle());

	UseInputCheck(rhs.UseInputCheck());
	NoFocus(rhs.NoFocus());
	IsOutsideWnd(rhs.IsOutsideWnd());
	IsRemoveOutside(rhs.IsRemoveOutside());
}

CXUI_Wnd* CXUI_Wnd::DeepCopy()
{
	CXUI_Wnd* pWnd = VClone();
	if(!pWnd){return NULL;}
	*pWnd = *this;
	pWnd->LastUsedTime(BM::GetTime32());

	return pWnd;
}

CXUI_Wnd* CXUI_Wnd::VClone()
{
	return NULL;
}

void CXUI_Wnd::CorrectionScreenPoint( POINT3I &ptOrg, POINT3I const& ptAdd )
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("CorrectionScreenPoint"), CXUI_Manager::GetFrameCount()));
	//스크린 영역을 벗어나지 않기위해 Point 계산 보정
	ptOrg += ptAdd;
	
	if( GetIsDefaultResolution() )
	{
		ptOrg.x = __min(EXV_DEFAULT_SCREEN_WIDTH - 1, ptOrg.x );
		ptOrg.y = __min(EXV_DEFAULT_SCREEN_HEIGHT - 1, ptOrg.y );
	}
	else
	{
		ptOrg.x = __min( GetResolutionSize().x - 1, ptOrg.x );
		ptOrg.y = __min( GetResolutionSize().y - 1, ptOrg.y );
	}

	ptOrg.x = __max(ptOrg.x, 0 );
	ptOrg.y = __max(ptOrg.y, 0 );

//	ptOrg.z = ptAdd.z;//z는 더하지 않는다.
}

bool CXUI_Wnd::SetEditFocus(bool const bIsJustFocus)
{
	CONT_CONTROL::iterator itor = m_contControls.begin();

	while(m_contControls.end() != itor)
	{
		if((*itor)->SetEditFocus(bIsJustFocus))
		{
			return true;
		}
		++itor;
	}

	return false;
}

POINT2 CXUI_Wnd::AdjustText(std::wstring const& wstrFont, std::wstring const& kSrc, CXUI_Style_String &kOut,int iCropWidth)
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo(AdjustText), CXUI_Manager::GetFrameCount()));
	int const iLimitWidth = (iCropWidth == -1) ? Size().x : iCropWidth;

	if(!iLimitWidth)
	{
		assert(iLimitWidth);
		return POINT2::NullData();
	}
	kOut.Clear();

	size_t sLineCount = 0;
	size_t line_width = 0;
	CXUI_Style_String szLineText, szTemp;	//한줄씩 넣어봐야 하니까
	
	XUI::CXUI_Font* pFont = g_kFontMgr.GetFont(wstrFont);
	if( !pFont )
	{
		return POINT2::NullData();
	}

	CXUI_Style_String	kString( PgFontDef(pFont, FontColor(), FontFlag()), kSrc );

	if (NoWordWrap())
	{
		kOut = kString;
		kOut.SetOriginalString(kString.GetOriginalString());
		return POINT2(kOut.GetSize());
	}

	int iTextPosX = m_TextPos.x;

	if( FontFlag() & XTF_ALIGN_CENTER || FontFlag() & XTF_ALIGN_RIGHT )
	{
		iTextPosX = 0;
	}
	

	kString.ProcessWordWrap(iLimitWidth,true,kOut,iTextPosX);

	kOut.SetOriginalString(kString.GetOriginalString());
	kOut.AdjustText();
	kOut.RecalculateAlignHeight();

	return POINT2(kOut.GetSize());
}

void CXUI_Wnd::ReloadText()
{
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("ReloadText"), CXUI_Manager::GetFrameCount()));
	if( m_bUpdatedText )
	{
		SAFE_DELETE(m_pText);
		m_bUpdatedText = false;
	}
	POINT2 const ptTextRect = AdjustText(Font(), m_Text, m_kStyleText);
	TextRect(ptTextRect);

	if( FontFlag() & XTF_ALIGN_Y_CENTER )
	{
		POINT2 kTextPos = TextPos();
		POINT2 kTxtSize = g_kFontMgr.CalculateOnlySize(StyleText());
		kTextPos.y = (Size().y - kTxtSize.y) / 2;
		TextPos( kTextPos );
	}

	SetInvalidate();
}

void CXUI_Wnd::Text(std::wstring const& szString)
{
//	if( szString.empty() ){	return;}
	XUI_STAT(XUIStatTimerF timerA((XFrameStat*)&m_kXUIStatGroup.GetStatInfo("Text"), CXUI_Manager::GetFrameCount()));
	if( m_Text != szString )
	{
		m_Text = szString;
		//m_bUpdatedText = true;
		ReloadText();
	}
}

void CXUI_Wnd::FontColor(DWORD const dwNewColor)
{
	if( FontColor() != dwNewColor )
	{
		m_dwFontColor = dwNewColor;
		m_bUpdatedText = true;
		ReloadText();
	}
}

void CXUI_Wnd::FontFlag(DWORD const dwNewFlag)
{
	if( FontFlag() != dwNewFlag )
	{
		m_dwFontFlag = dwNewFlag;
		//m_bUpdatedText = true;
		ReloadText();
	}
}

void CXUI_Wnd::Font(std::wstring const& rkNewFaceName)
{
	if( Font() != rkNewFaceName )
	{
		m_Font = rkNewFaceName;
		//m_bUpdatedText = true;
	}
}


/*
void CXUI_Wnd::DoScriptFocused()
{
	if(CXUI_Wnd::m_spWndMouseFocus)
	{
		CXUI_Wnd::m_spWndMouseFocus->DoScript(SCRIPT_ON_KEY_ENTER);
	}
}
*/

bool CXUI_Wnd::GetClip()
{
	CXUI_Wnd* pWnd = this;
	while(pWnd)
	{
		if(pWnd->CanClip())
		{
			return true;
		}
		pWnd = pWnd->Parent();
	}

	return false;
}

bool CXUI_Wnd::SetColor(DWORD dwColor)
{
	if( m_dwColor != dwColor )
	{
		m_dwPastColor = m_dwColor;
		m_dwColor = dwColor;

		SetInvalidate();

		return true;
	}

	return false;
}

void CXUI_Wnd::SetColorSet()	//XUI에 디폴트로 지정된 셋팅 값들을 로딩
{
	for_each( m_contControls.begin(), m_contControls.end(), SetColorSet_Func() );
	if ( CanColorChange() && ColorType() > 0 )
	{
		SetColor(CXUI_Manager::GetColor(ColorType()));
		ColorChange(true);
	}
	Invalidate(true);
}

bool CXUI_Wnd::IsMouseOver()const
{
	if(ContainsPoint( m_sMousePos ))//마우스 검사.
//	if(m_spWndMouseOver == this)// 마우스 올라왔어.	
	{
		return true;
	}
	return false;
}

void CXUI_Wnd::SetCustomData(BM::Stream const& kData)
{
	SetCustomData(kData.Data());
}

void CXUI_Wnd::SetCustomData(std::wstring const& kData)
{
	SetCustomData(kData.c_str(), sizeof(std::wstring::value_type)*kData.size());
}

void CXUI_Wnd::SetCustomData(std::vector< char > const& kData)
{
	m_kCustomData = kData;
}

void CXUI_Wnd::SetCustomData(void const* pData, size_t const size)
{
	if(pData)
	{
		m_kCustomData.clear();
		if (size > 0)
		{
			m_kCustomData.resize(size);
			::memcpy(&m_kCustomData.at(0), pData, size);
		}
	}
}

void CXUI_Wnd::GetCustomData(BM::Stream & kData)const
{
	GetCustomData(kData.Data());
	kData.PosAdjust();
}

void CXUI_Wnd::GetCustomData(std::wstring & kData)const
{
	size_t const iCustomSize = GetCustomDataSize() / sizeof(std::wstring::value_type);
	if( iCustomSize )
	{
		std::wstring kTemp;
		kTemp.resize(iCustomSize);
		if( iCustomSize == kTemp.size() )
		{
			if( GetCustomData(&kTemp[0], sizeof(std::wstring::value_type) * kTemp.size()) )
			{
				kData = kTemp;
			}
		}
	}
}

bool CXUI_Wnd::GetCustomData(std::vector< char > &kData)const
{
	if(m_kCustomData.size())
	{
		kData = m_kCustomData;
		return true;
	}
	return false;
}

bool CXUI_Wnd::GetCustomData(void *pData, size_t const size)const
{
	if(0 < size 
	&& m_kCustomData.size() == size)//사이즈 다르면 주지않는다.
	{
		::memcpy(pData, &m_kCustomData.at(0), size);
		return true;
	}
	return false;
}

size_t CXUI_Wnd::GetCustomDataSize()const
{
	return m_kCustomData.size();
}

void CXUI_Wnd::SetState( const E_State eState )
{
	if ( m_eState == eState  )
	{
		return;
	}

	switch( eState )
	{
	case ST_OPENING:
		{
			if ( ST_STOP != m_eState )
			{
				TargetPos( Location() );
			}
			
			switch( AppearEffect() )
			{
			case AE_CUTTON_LEFT:
				{
					if( GetIsDefaultResolution() )
					{
						Location( EXV_DEFAULT_SCREEN_WIDTH, Location().y );
					}
					else
					{
						Location( GetResolutionSize().x, Location().y );
					}					
				}break;
			case AE_CUTTON_RIGHT:
				{
					Location( -Width(), Location().y );
				}break;
			}
		}break;
	default:
		{
		}break;
	}

	m_eState = eState;
}

void CXUI_Wnd::UVUpdate( size_t const iIndex )
{
	SUVInfo kUVInfo= UVInfo();
	kUVInfo.Index = iIndex;
	UVInfo(kUVInfo);
	SetInvalidate();	
}

CXUI_Wnd* CXUI_Wnd::GetDragableWnd()
{
	CXUI_Wnd* pWnd = this;
	
	while(pWnd)
	{
		if (pWnd->VType() == E_XUI_ICON || pWnd->VType() == E_XUI_LIST)
		{
			//지훈파트장님 작업 완료될때 까지 막아놓음 by 하승봉
//			CXUI_Icon* pkIcon = (CXUI_Icon*)pWnd;
//			if (pkIcon->IconInfo().iIconResNumber > 0)
//			{
				return NULL;
//			}
		}
		
		if(pWnd->CanDrag())
		{
			return pWnd;
		}
		pWnd = pWnd->Parent();

		pWnd->GetDragableWnd();
	}

	return NULL;
}

void CXUI_Wnd::Drag(CXUI_Wnd* pkWnd, POINT3I const& rPT)
{
	if (!pkWnd)
	{
		return;
	}
	 
	const CXUI_Wnd* pkParent = pkWnd->Parent();

	if (pkParent)
	{
		POINT3I ptOrg = pkWnd->Location();
		ptOrg+=rPT;
		if (ptOrg.x<0)
		{
			ptOrg.x = 0;
		} 
		else if (ptOrg.x + pkWnd->Size().x > pkParent->Size().x)
		{
			ptOrg.x = pkParent->Size().x - pkWnd->Size().x;
		}

		if (ptOrg.y<0)
		{
			ptOrg.y = 0;
		}
		else if (ptOrg.y + pkWnd->Size().y > pkParent->Size().y)
		{
			ptOrg.y = pkParent->Size().y - pkWnd->Size().y;
		}

		pkWnd->Location( ptOrg );
		pkWnd->SetInvalidate(true);
	}
	else	// 최상위 윈도우면
	{
		POINT3I kLoc = pkWnd->Location() + rPT;
		pkWnd->Location(kLoc);
	}
}

CXUI_Wnd* CXUI_Wnd::VAcquireFocus(CXUI_Wnd* pkWnd)
{
	if (pkWnd->NoFocus())	//포커스를 안가지는 놈이면 현재 포커스를 그대로 반환하자
	{
		return CXUI_Wnd::m_spWndMouseFocus;
	}

	CXUI_Wnd* pkPastFocused = CXUI_Wnd::m_spWndMouseFocus;
	if (pkPastFocused!=pkWnd)
	{
		bool const bPrevDoNotLostFocus = (pkPastFocused)? pkPastFocused->IsDoNotLostFocus(): false;
		if( pkPastFocused
		&&	!bPrevDoNotLostFocus )//이전에 포커스된 윈도우가 있으면
		{
			pkPastFocused->VLoseFocus();
		}

		if( !bPrevDoNotLostFocus )
		{
			CXUI_Wnd::m_spWndMouseFocus = pkWnd;
		}

		if( pkWnd )
		{
			DoScript(SCRIPT_ON_GET_FOCUS);//포커스 됬을 때 실행할 스크립트를 실행하자
		}
	}

	return pkPastFocused;
}

void CXUI_Wnd::VLoseFocus(bool const bUpToParent)
{
	//마우스 아웃 스크립트를 실행해 주자
	//오버드 마우스 윈도우도 NULL로 해 줄지는 생각해 보자
	if (m_spWndMouseFocus==this)
	{
		m_spWndMouseFocus = NULL;
		if (false == DoScript(SCRIPT_ON_LOSE_FOCUS) && NULL != Parent())
		{
			if (bUpToParent)
			{
				Parent()->VLoseFocus(bUpToParent);
			}
		}
		IsMouseDown(false); 
		if (IsMouseOver())
		{
			CXUI_Wnd::m_spWndMouseOver = NULL;
			DoScript(SCRIPT_ON_MOUSE_OUT);
		}
	}
}

void CXUI_Wnd::TypingEffectReset()
{
	if (m_pText)
	{
		m_pText->SetDrawByTime(TypingEffect());

		if ( TypingEffect() )
		{
			m_pText->ResetInitTime();
		}
	}
}

void CXUI_Wnd::SetTwinkle(bool const bValue)
{
	m_bIsTwinkle = bValue;
	TwinkleStartTime( (bValue ? BM::GetTime32() : 0));
	if (!bValue)
	{
		ReservTwinkleOff(bValue);
	}

	for_each( m_contControls.begin(), m_contControls.end(), TwinkleControl_Func(m_bIsTwinkle) );	
}

void CXUI_Wnd::TwinkleTime(DWORD const dwTime)		//자식들도 똑같이
{
	m_dwTwinkleTime = dwTime;
	if (m_contControls.size() > 0)
	{
		for_each( m_contControls.begin(), m_contControls.end(), TwinkleTimeControl_Func(m_dwTwinkleTime) );
	}
}

void CXUI_Wnd::TwinkleInterTime(DWORD const dwTime)	//자식들도 똑같이
{
	m_dwTwinkleInterTime = dwTime;
	
	if (m_contControls.size() > 0)
	{
		for_each( m_contControls.begin(), m_contControls.end(), TwinkleInterTimeControl_Func(m_dwTwinkleInterTime) );
	}
}

void CXUI_Wnd::OffTwinkle()
{
	if(!IsTwinkle())
	{
		return;
	}

	XUI::CXUI_Wnd *pParent = Parent();
	while (pParent)
	{
		if (pParent->IsTwinkle())
		{
			if (pParent->Parent())
			{
				pParent = pParent->Parent();
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if (pParent)
	{
		pParent->SetTwinkle(false);
	}
}
bool	CXUI_Wnd::CheckVisible()
{
	if(!Visible())
	{
		return	false;
	}

	if(Parent())
	{
		return	Parent()->CheckVisible();
	}

	return	true;
}

void CXUI_Wnd::Visible(bool const& Value)
{
	if(m_bVisible != Value)
	{
		SetInvalidate();
	}

	if ((!m_bVisible && Value) && CXUI_Wnd::m_spWndMouseFocus != this)
	{
		VAcquireFocus(this);
	}
	m_bVisible = Value;
}

void CXUI_Wnd::CursorToThis()//커서를 이동.
{
	CXUI_Wnd::m_sMousePos.x = TotalLocation().x;
	CXUI_Wnd::m_sMousePos.y = TotalLocation().y;
}

//int CXUI_Wnd::HideAllUI()
//{
//	//int iCount = 0;
//	if( Parent() )			//최상위 부모일 때만
//	{
//		return 0;
//		//WND_LIST rkList = XUIMgr.GetActiveList();
//		//WND_LIST::iterator itor = rkList.begin();
//		//while( rkList.end() != itor )
//		//{
//		//	const WND_LIST::value_type &pElement = (*itor);
//		//	if(pElement != NULL && pElement->ID() != STR_XUI_CURSOR )
//		//	{
//		//		pElement->Visible(false);
//		//		++iCount;
//		//	}
//		//	++itor;
//		//}
//		//m_HideList = rkList;	
//	}
//
//	//return iCount;
//	return XUIMgr.HideAllUI();
//}

//int CXUI_Wnd::ShowAllUI()
//{
//	//int iCount = 0;
//	if( Parent() )			//최상위 부모일 때만
//	{
//		return 0;
//		//WND_LIST::const_iterator itor = m_HideList.begin();
//		//while( m_HideList.end() != itor )
//		//{
//		//	const WND_LIST::value_type &pElement = (*itor);
//		//	if(pElement != NULL && pElement->ID() != STR_XUI_CURSOR && !pElement->IsClosed() )
//		//	{
//		//		pElement->Visible(true);
//		//		XUIMgr.InsertActiveList(pElement);
//		//	}
//
//		//	++itor;
//		//}
//	}
//	//m_HideList.clear();
//	//return iCount;
//	return XUIMgr.ShowAllUI();
//}

void CXUI_Wnd::ResetModifier()
{
	if(m_spHotKeyModifier)
	{
		m_spHotKeyModifier->ReSetModifierState();
	}
}

bool CXUI_Wnd::CheckExistUniqueLiveType(std::wstring const& UniqueExistType)
{
	if( !UniqueExistType.empty() )
	{
		CONT_ALIVE_UNIQUE_EXIST_UI::iterator alive_itor = m_kContAliveUniqueExistUI.begin();
		while( alive_itor != m_kContAliveUniqueExistUI.end() )
		{
			CONT_ALIVE_UNIQUE_EXIST_UI::value_type const& kAliveType = (*alive_itor);

			if( kAliveType == UniqueExistType )
			{
				return true;
			}
			++alive_itor;
		}

		m_kContAliveUniqueExistUI.push_back(UniqueExistType);
	}
	return false;
}

void CXUI_Wnd::DeleteExistUniqueListType(std::wstring const& UniqueExistType)
{
	CONT_ALIVE_UNIQUE_EXIST_UI::iterator alive_itor = m_kContAliveUniqueExistUI.begin();
	while( alive_itor != m_kContAliveUniqueExistUI.end() )
	{
		CONT_ALIVE_UNIQUE_EXIST_UI::value_type const& kAliveType = (*alive_itor);

		if( kAliveType == UniqueExistType )
		{
			m_kContAliveUniqueExistUI.erase(alive_itor);
			return;
		}
		++alive_itor;
	}
}

bool CXUI_Wnd::IsExistUniqueLiveType(std::wstring const& UniqueExistType)
{
	if( !UniqueExistType.empty() )
	{
		CONT_ALIVE_UNIQUE_EXIST_UI::iterator alive_itor = std::find( m_kContAliveUniqueExistUI.begin(), m_kContAliveUniqueExistUI.end(), UniqueExistType );
		if( alive_itor != m_kContAliveUniqueExistUI.end() )
		{
			return true;
		}
	}

	return false;	// 존재하지 않는다.
}
