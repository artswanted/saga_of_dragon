#include "stdafx.h"
#include "XUI_MsgBox.h"

using namespace XUI;

CXUI_MsgBox::CXUI_MsgBox(void)
{
}

CXUI_MsgBox::~CXUI_MsgBox(void)
{
}

void CXUI_MsgBox::VInit()
{
	CXUI_Wnd::VInit();
}

void CXUI_MsgBox::VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue)
{
	CXUI_Wnd::VRegistAttr(wstrName, wstrValue);
}