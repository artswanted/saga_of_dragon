#include "stdafx.h"

#include "XUI_Scroll.h"

using namespace XUI;

CXUI_HScroll::CXUI_HScroll()
:	m_bChangeValue(false)
,	m_iCurValue(0)
{
	MinValue(0);
	MaxValue(100);
	m_kScrBtnName.clear();
	m_pkBtn = NULL;
}

CXUI_HScroll::~CXUI_HScroll()
{

}

CXUI_Wnd* CXUI_HScroll::VClone()
{
	CXUI_HScroll *pWnd = new CXUI_HScroll;
	*pWnd = *this;
	return pWnd;
}

bool CXUI_HScroll::VOnTick( DWORD const dwCurTime )
{
	if ( !CXUI_Wnd::VOnTick( dwCurTime ) )
	{
		return false;
	}

	if ( IsValueChanged() )
	{
		m_bChangeValue = false;
		DoScript(SCRIPT_ON_SCROLL_VALUE_CHANGED);
	}
	return true;
}

void CXUI_HScroll::VInit()
{
	CXUI_Wnd::VInit();

	AdjustMiddleBtnPos();
	m_bChangeValue = false;
}

void CXUI_HScroll::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_BTN == wstrName )	{	ScrBtnName(wstrValue); m_pkBtn = GetControl(ScrBtnName()); }
	else if( ATTR_MIN == wstrName )	{	MinValue((int)vValue); }
	else if( ATTR_MAX == wstrName )	{	MaxValue((int)vValue); }
	else if( ATTR_CUR == wstrName )	{	CurValue((int)vValue); }
}

bool CXUI_HScroll::IsValueChanged()const
{
	if ( m_bChangeValue )
	{
		if (m_pkBtn)
		{
			return !m_pkBtn->IsMouseDown();
		}
		return true;
	}
	return false;
}

bool CXUI_HScroll::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	if(!Visible() || IsClosed() ){return false;}
	
	if( ContainsPoint(m_sMousePos) )
	{
		if( GetBtnControl() )
		{
			if ( m_pkBtn->VPeekEvent(rET, rPT, dwValue) )
			{
				if (	Enable()
					&&	rET == IEI_MS_MOVE 
					&&	m_pkBtn->IsMouseDown() )
				{
					POINT3I ptBtn = m_pkBtn->Location();
					ptBtn.x = __max(ptBtn.x+rPT.x, 0);
					ptBtn.x = __min(ptBtn.x, Size().x - m_pkBtn->Size().x);
					m_pkBtn->Location(ptBtn);//버튼 위치 보정
					AdjustCur();
					DoScript(SCRIPT_ON_SCROLL_MOVE);
				}	
			}
			else if ( VPeekEvent_Default(rET, rPT, dwValue) )
			{
				if (	Enable() 
					&&	IsMouseDown() 
					&&	rET == IEI_MS_DOWN )
				{
					int const MoveX = m_sMousePos.x - m_pkBtn->GetTotalLocation().x;
					POINT3I ptBtn = m_pkBtn->Location();
					ptBtn.x = __max(ptBtn.x+MoveX, 0);
					ptBtn.x = __min(ptBtn.x, Size().x - m_pkBtn->Size().x);
					m_pkBtn->Location(ptBtn);//버튼 위치 보정

					AdjustCur();
					DoScript(SCRIPT_ON_SCROLL_MOVE);
				}
			}
			return true;
		}
		return VPeekEvent_Default(rET, rPT, dwValue);
	}
	return false;
}

bool CXUI_HScroll::VDisplay()
{
	if(!CXUI_Wnd::VDisplay() ){return false;}

	return true;
}

HRESULT CXUI_HScroll::CurValue( int const iValue )
{
	if ( m_iCurValue != iValue )
	{
		if ( (MinValue() > iValue) || (MaxValue() < iValue) )
		{
			return E_FAIL;
		}

		m_iCurValue = iValue;
		AdjustMiddleBtnPos();
		m_bChangeValue = true;
		return S_OK;
	}

	return S_FALSE;
}

void CXUI_HScroll::AdjustMiddleBtnPos()
{
	float fRate = 0.0f;
	if ( CurValue() > MinValue() )
	{
		int iMaxValue = MaxValue() - MinValue();
		fRate = ( CurValue() < iMaxValue ) ? ( (float)CurValue() / (float)iMaxValue) : 1.0f;
	}
	
	if( GetBtnControl() )
	{
		float fX = (float)(Size().x - m_pkBtn->Size().x);
		fX *= fRate;

		int SetX = (int)fX;
		float fTemp1 = fX - (float)SetX;
		float fTemp2 = (float)(SetX+1) - fX;
		if ( fTemp1 > fTemp2 )
		{
			++SetX;
		}

		m_pkBtn->Location(SetX, m_pkBtn->Location().y);
		AdjustCur();
	}
}

void CXUI_HScroll::AdjustCur()
{
	if(GetBtnControl())
	{
		//float fRate = (m_pkBtn->Location().x - Location().x)/( Size().x - m_pkBtn->Size().x );
		float fRate = ((float)m_pkBtn->Location().x / (float)(Size().x-m_pkBtn->Size().x));

		int const iValue = max( static_cast<int>(MaxValue()*fRate), MinValue() );

		if ( CurValue() != iValue )
		{
			m_iCurValue = iValue;
			m_bChangeValue = true;
		}
	}
}

//
CXUI_Wnd* CXUI_HScroll::GetBtnControl()
{
	if(!m_pkBtn)
	{
		m_pkBtn = GetControl(ScrBtnName());
	}
	return m_pkBtn;
}

void CXUI_HScroll::operator = ( const CXUI_HScroll &rhs)
{
	CXUI_Wnd::operator =(rhs);
	
	MinValue(rhs.MinValue());
	MaxValue(rhs.MaxValue());
	ScrBtnName(rhs.ScrBtnName());
}