#include "stdafx.h"

#include <tchar.h>
#include "XUI_Form.h"
#include "BM/vstring.h"

using namespace XUI;

CXUI_Form::CXUI_Form(void)
{
	IsHaveBG(true);//기본 BG
	m_pControlMouseOver = NULL;
}

CXUI_Form::~CXUI_Form(void)
{
}

void CXUI_Form::VInit()
{
#ifdef XUI_USE_GENERAL_OFFSCREEN
	if (NotUseOffscreen())
		UseOffscreen(false);
	else
		UseOffscreen(true);
#endif
	CXUI_Wnd::VInit();
}

void CXUI_Form::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);

	if( ATTR_NO_BG == wstrName )
	{
		if( 0 == (int)vValue )
		{
			IsHaveBG(true);
		}
		else
		{//노 BG 가 1 이면 BG는 없음
			IsHaveBG(false);
		}
	}	
}