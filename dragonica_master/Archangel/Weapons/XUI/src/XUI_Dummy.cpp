#include "stdafx.h"

#include <tchar.h>

#include "XUI_Dummy.h"
#include "XUI_Config.h"
#include "XUI_Manager.h"
#include "BM/vstring.h"

using namespace XUI;

CXUI_Wnd* CXUI_Dummy::m_spDummyMouseOver = NULL;

CXUI_Dummy::CXUI_Dummy(void)
{
	m_pControlMouseOver = NULL;
	PassEvent(true);
	ChildDisplay(false);
}

CXUI_Dummy::~CXUI_Dummy(void)
{
	if (m_spDummyMouseOver == this)
	{
		m_spDummyMouseOver = NULL;
	}
}

void CXUI_Dummy::VInit()
{
	UseOffscreen(false);
	PassEvent(true);
	CXUI_Wnd::VInit();
}

void CXUI_Dummy::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);
	if(ATTR_IS_CHILD_DISPLAY == wstrName)
	{
		ChildDisplay( 0 != static_cast<int>(vValue) );
	}
}

bool CXUI_Dummy::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)//이벤트는 키 UP으로만 동작.
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

bool CXUI_Dummy::VPeekEvent_Default(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	/*
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
	*/
	if ( !Visible() || IsClosed() )
		return false;

	bool bRet = false;
	m_bDBLClick = false;
#ifdef XUI_USE_SCRIPT_CALL_OPTIMIZE
	unsigned int wstrScriptKey = SCRIPT_MAX_NUM;
#else
	std::wstring wstrScriptKey;
#endif
	switch(rET)
	{
	// Dummy는 키를 처리할 일이 없을거 같아서 막아놓음.
	//case IEI_KEY_DOWN:
	//	{//컨트롤에 전달 되어야함. dxxx todo 
	//		BM::vstring vstr = (int)dwValue;
	//		bRet = DoHotKey(vstr);
	//		if(!bRet)
	//		{
	//			return false;
	//		}
	//		return true;
	//	}break;
	case IEI_MS_DOWN:
		{
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{

				if(MEI_BTN_0 == dwValue)
				{
					IsMouseDown(true);

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
								m_bDBLClick = true;
								break;
							}
						}
					}

					dwLastDownTime = dwNow;//뭐가 됐든 마지막 다운시간은 기록

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
	case IEI_MS_MOVE:
		{
			bool const bIsBeforeMouseOver = IsMouseOver();
			if( ContainsPoint( m_sMousePos ) )//마우스 이벤트는 영역 체크 필수.
			{
				bRet = true;
				if(m_spDummyMouseOver != this)
				{
					if(m_spDummyMouseOver)
					{
						m_spDummyMouseOver->DoScript(SCRIPT_ON_MOUSE_OUT);//이전 마우스 오버에서 아웃 처리.
						m_spDummyMouseOver->IsMouseDown(false);
					}

					m_spDummyMouseOver = this;// 마우스 올라왔어.

					wstrScriptKey = SCRIPT_ON_MOUSE_OVER;
				}

				if(IsMouseDown() && CanDrag() && Parent()->CanDrag())
				{
					POINT3I ptOrg = Parent()->Location();
					Parent()->Location( ptOrg+rPT );
				}
				
				if(rPT.z != 0 && ContainsPoint(m_sMousePos)) // 마우스 휠이다.
				{
					if (rPT.z > 0){wstrScriptKey = SCRIPT_ON_WHEEL_UP;}
					else{wstrScriptKey = SCRIPT_ON_WHEEL_DOWN;}
					SetCustomData(&rPT.z, sizeof(rPT.z));
				}
			}
			else
			{
				if(m_spDummyMouseOver == this)	//이전 프레임에선 내가 오버였으면
				{
					IsMouseDown(false);
					DoScript(SCRIPT_ON_MOUSE_OUT);
					m_spDummyMouseOver = NULL;
					return false;
				}
			}
		}break;
	default:
		{
			return false;
		}break;
	}

	bool const bScriptRet = DoScript( wstrScriptKey );//실행 안되는 현상을 방지하기 위함.

	//return (bRet || bScriptRet);
	return !PassEvent();//무조건 False
}

bool CXUI_Dummy::VDisplay()
{
	if( !Visible() ){return false;}//자식컨트롤도 안그림

	//CXUI_Wnd::VDisplay();//모든 그리기 무시
	//ON_DISPLAY 스크립트 무시
	//자식 컨트롤 그리기 무시

	if( ChildDisplay() )
	{
		DisplayControl();
	}
	return true;
}