#pragma once
#include "stdafx.h"

std::wstring const HTML_OPEN_MARK		= _T("<");
std::wstring const HTML_END_OPEN_MARK	= _T("</");
std::wstring const HTML_CLOSE_MARK		= _T(">");

std::wstring const HTML_FONT			= _T("font ");
std::wstring const HTML_HTML			= _T("html ");
std::wstring const HTML_PRE				= _T("pre ");

std::wstring const HTML_COLOR_RED		= _T("FF0000");
std::wstring const HTML_COLOR_SEG		= _T("20B2AA");
std::wstring const HTML_COLOR_YEL		= _T("CD950C");
std::wstring const HTML_COLOR_GRE		= _T("008B00");
std::wstring const HTML_COLOR_BLU		= _T("0000FF");
std::wstring const HTML_COLOR_IND		= _T("000080");
std::wstring const HTML_COLOR_PIN		= _T("FF00FF");
std::wstring const HTML_COLOR_BLA		= _T("000000");

extern std::wstring HTML_Color(std::wstring const& kColorVal);	// 색
extern std::wstring HTML_Sup(unsigned int uiNum);				// 각주 번호
extern std::wstring HTML_FontSize(unsigned int uiSize);			// 사이즈
extern std::wstring HTML_DecoText(std::wstring const& kTitle, std::wstring const kColor = HTML_COLOR_BLA, int const iSize=2);