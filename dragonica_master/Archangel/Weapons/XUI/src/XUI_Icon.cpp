#include "stdafx.h"
#include "XUI_Icon.h"
#include "XUI_Cursor.h"
#include "XUI_Manager.h"
using namespace XUI;

//CXUI_Icon::IconWndMap CXUI_Icon::ms_kIconWndMap;

CXUI_Icon::CXUI_Icon(void)
{
//	CanDrag(true);//기본 드래그 가능.
	IsStaticIcon(false);
	m_kClipRect.top = 0;
	m_kClipRect.bottom = 0;
	m_kClipRect.left = 0;
	m_kClipRect.right = 0;
}

CXUI_Icon::~CXUI_Icon(void)
{
}

void CXUI_Icon::VInit()
{
}

void CXUI_Icon::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_ICON_GRP == wstrName ){m_IconInfo.iIconGroup = (int)vValue;}
	else if( ATTR_ICON_KEY == wstrName ){m_IconInfo.iIconKey = (int)vValue;}
	else if( ATTR_ICON_STATIC == wstrName )
	{
		int const iValue = (int)vValue;
		if(iValue)
		{
			IsStaticIcon(true);
		}
	}
}

bool CXUI_Icon::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if( !Visible() || IsClosed() ){return false;}
	if( !Enable() ){return false;}

	CONT_CONTROL::iterator itor = m_contControls.begin();
	
	while( m_contControls.end() != itor)
	{
		if( (*itor)->VPeekEvent(rET, rPT, dwValue) )
		{
			return true;
		}
		++itor;
	}

	bool bRet = false;

#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	unsigned int wstrScriptKey = SCRIPT_MAX_NUM;
#else
	std::wstring wstrScriptKey;
#endif
	switch(rET)
	{
	case IEI_MS_DOWN:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				m_spWndMouseOver = this;
				if(MEI_BTN_0 == dwValue)
				{
					IsMouseDown(true);	
					VAcquireFocus(this);

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
								break;
							}
						}
					}

					dwLastDownTime = dwNow;//뭐가 됐든 마지막 다운시간은 기록

					if(!IsStaticIcon())//스태틱 아니어야 커서로 집어진다.
					{
						CXUI_Cursor::m_IconInfo = m_IconInfo;
					}

					wstrScriptKey = SCRIPT_ON_L_BTN_DOWN;
					LastMouseDownPos(m_sMousePos);
				}
				else if( MEI_BTN_1 == dwValue )
				{ 
					wstrScriptKey = SCRIPT_ON_R_BTN_DOWN;
				}
				bRet = true;
			}
		}break;
	case IEI_MS_UP:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
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
				bRet = true;
			}
		}break;
	}

	bool const bScriptRet = DoScript( wstrScriptKey );//실행 안되는 현상을 방지하기 위함.
	if(	rET == IEI_MS_UP 
	&&	bRet)//마우스 올라 왔으면 아이콘 정보 없엠.
	{
		CXUI_Cursor::m_IconInfo.Clear();
	}

	if(!bRet && !bScriptRet)
	{
		return VPeekEvent_Default(rET, rPT, dwValue);	
	}
	return (PassEvent())? FALSE: (bRet || bScriptRet);
}

bool CXUI_Icon::SetIconInfo(SIconInfo const& iconInfo)
{
	bool bChangeImage = false;

	if (m_IconInfo.iIconResNumber != iconInfo.iIconResNumber)
		bChangeImage = true;

	m_IconInfo = iconInfo;	
	if (bChangeImage && UseOffscreen())
	{
		SetInvalidate();
	}

	return true;
}

bool CXUI_Icon::VDisplay()
{
	if (CXUI_Wnd::VDisplay() == false)
		return false;

	if (m_IconInfo.iIconResNumber == 0)
		return false;

#ifdef XUI_USE_GENERAL_OFFSCREEN
	if (Invalidate() && UseOffscreen() && Offscreen())
	{
		RECT rectIcon;
		rectIcon.left = OffscreenLocation().x;
		rectIcon.top = OffscreenLocation().y;
		rectIcon.right = rectIcon.left + Size().x - 1;
		rectIcon.bottom = rectIcon.top + Size().y - 1;
		void* pIconImg = m_spRscMgr->VCreateIconResource(m_IconInfo.iIconResNumber, rectIcon,GrayScale());
		m_kOffscreenDrawList.push_back(std::make_pair(pIconImg, Parent() == NULL));
	}
#endif
	return true;
}

bool CXUI_Icon::GetClipRect(RECT &rkRect)
{
	RECT kParent;
	GetParentDrawRect(kParent);

	POINT3I const rkPos = TotalLocation();
	rkRect.top = __max(0, kParent.top - rkPos.y);
	rkRect.bottom = __max(0, rkPos.y + Height() - kParent.bottom);
	rkRect.left = __max(0, kParent.left - rkPos.x);
	rkRect.right = __max(0, rkPos.x + Width() - kParent.right);

	return true;
}