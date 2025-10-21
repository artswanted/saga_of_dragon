#include "stdafx.h"

int const MAX_SUP_PLACE = 5;
int const DECIMAL_VALUE = 10;
int const MAX_FONT_SIZE = 3;
std::wstring HTML_Color(std::wstring const& kColorVal)
{
	std::wstring kResult = _T("color=\"") + kColorVal + _T("\"");
	return kResult;
}

std::wstring HTML_Sup(unsigned int uiNum)
{
	char acBuf[MAX_SUP_PLACE]={0,};				
	::_itoa_s(uiNum,acBuf, DECIMAL_VALUE);

	std::wstring kNum(UNI(acBuf));
	std::wstring kResult = _T("<sup>") + kNum + _T("</sup>");
	return kResult;
}

std::wstring HTML_FontSize(unsigned int uiSize)
{
	char acBuf[MAX_FONT_SIZE]={0,};				
	::_itoa_s(uiSize,acBuf, DECIMAL_VALUE);

	std::wstring kSize(UNI(acBuf));
	std::wstring kResult = _T("size=\"") + kSize + _T("\"");
	return kResult;
}

std::wstring HTML_DecoText(std::wstring const& kTitle, std::wstring const kColor, int const iSize)
{
	std::wstring kResult = HTML_OPEN_MARK + HTML_FONT + HTML_FontSize(iSize) + HTML_Color(kColor) + HTML_CLOSE_MARK;
	kResult += kTitle + HTML_END_OPEN_MARK + HTML_FONT + HTML_CLOSE_MARK;
	return kResult;
}