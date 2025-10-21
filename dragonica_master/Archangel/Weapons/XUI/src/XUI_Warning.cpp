#include "stdafx.h"
#include <tchar.h>
#include "XUI_Font.h"
#include "XUI_Warning.h"

using namespace XUI;

CXUI_Warning::CXUI_Warning(void)
{
	FontFlag( FontFlag()|XTF_ALIGN_CENTER);
	IsAliveTimeIgnoreMouseOver(true);
}

CXUI_Warning::~CXUI_Warning(void)
{

}

bool CXUI_Warning::VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue)
{
	return false;
}

bool CXUI_Warning::VDisplay()//상속 받은대로 안함.
{
	if( !Visible() ){return false;}//자식컨트롤도 안그림
	DoScript( SCRIPT_ON_DISPLAY );
	
	IncAlpha();


	if( StyleText().Length()  )
	{
		POINT3I const pt = TotalLocation() + TextPos();
		RenderText( pt, StyleText(), FontFlag() );
	}
	DisplayControl();
	return true;
}