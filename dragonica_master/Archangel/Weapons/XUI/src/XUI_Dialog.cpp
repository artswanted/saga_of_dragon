#include "stdafx.h"
#include "XUI_Dialog.h"
using namespace XUI;
CXUI_Dialog::CXUI_Dialog()
{
}

CXUI_Dialog::~CXUI_Dialog()
{
}

void CXUI_Dialog::VInit()
{
	CXUI_Form::VInit();

}

void CXUI_Dialog::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);

	BM::vstring vValue(wstrValue);
/*
	if( ATTR_TITLE == wstrName ){	TitleText( XUI_TTSTR(wstrValue) );	}

	if( ATTR_TITLE_T_X == wstrName )
	{
		POINT2 pt = TitleTextPos();
		pt.x = (int)vValue;
	
		TitleTextPos( pt );	
	}

	if( ATTR_TITLE_T_Y == wstrName )
	{
		POINT2 pt = TitleTextPos();
		pt.y = (int)vValue;
	
		TitleTextPos( pt );	
	}

	if( ATTR_TITLE_FONT == wstrName ){	TitleFont( wstrValue );	}
*/
}
