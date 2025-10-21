#include "stdafx.h"
//

#include <memory>
#include <string>
#include "XUI_Font.h"
#include "BM/Stlsupport.h"
#include "TCHAR.H"
#include "BM/vstring.H"
#include "BM/ThreadObject.H"
#include "winbase.h"
#include "freetype/ftbitmap.h"

using namespace XUI;

const	WCHAR*	XUI::XUI_SAVE_FONT = _T("__sf");
const	WCHAR*	XUI::XUI_RESTORE_FONT = _T("__rf");
const	WCHAR*	XUI::XUI_SAVE_COLOR = _T("__sc");
const	WCHAR*	XUI::XUI_RESTORE_COLOR = _T("__rc");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
tagFontDef::tagFontDef()
{
	Set(NULL);
}

tagFontDef::tagFontDef(CXUI_Font *pFont, DWORD const dwFontColor, DWORD const dwOption)
{
	Set(pFont, dwFontColor, dwOption);
}

void tagFontDef::Set(CXUI_Font *pFont, DWORD const dwFontColor, DWORD const dwOption)
{
	m_pFont = pFont;
	dwColor = dwFontColor;
	dwOptionFlag = dwOption;
}

tagParsedChar::tagParsedChar(const TCHAR wChar, CXUI_Font *pFont, int const iAlignHeight,int iExtraDataPackIndex)
{
	m_wChar = wChar;
	m_FontDef.Set(pFont);
	m_iAlignHeight = iAlignHeight;

	m_iWidth = -1;
	m_iExtraDataPackIndex = iExtraDataPackIndex;
	Width();
}

tagParsedChar::tagParsedChar(const WCHAR wChar, CXUI_Font *pFont, DWORD const dwFontColor, DWORD const dwFontOption, int const iAlignHeight,int iExtraDataPackIndex)
{
	m_wChar = wChar;
	m_FontDef.Set(pFont, dwFontColor, dwFontOption);
	m_iAlignHeight = iAlignHeight;

	m_iWidth = -1;
	m_iExtraDataPackIndex = iExtraDataPackIndex;
	Width();
}

bool tagParsedChar::IsEqual(const tagParsedChar &kChar)const
{
	if(m_wChar != kChar.m_wChar) return false;
	if(m_FontDef.dwOptionFlag != kChar.m_FontDef.dwOptionFlag) return false;
	if(m_FontDef.dwColor != kChar.m_FontDef.dwColor) return false;
	if(m_FontDef.m_pFont!= kChar.m_FontDef.m_pFont) return false;
	return	true;
}

bool tagParsedChar::operator == (const WCHAR wChar)const
{
	if(m_wChar == wChar) return true;
	return false;
}

size_t tagParsedChar::Width() 
{
	if(m_iWidth == -1 && m_FontDef.m_pFont)
	{
		CXUI_Font::FONT_STYLE kSaved = m_FontDef.m_pFont->GetStyle();
		m_FontDef.m_pFont->SetStyle((CXUI_Font::FONT_STYLE)m_FontDef.dwOptionFlag);
		m_iWidth = m_FontDef.m_pFont->GetWidth(m_wChar);
		m_FontDef.m_pFont->SetStyle(kSaved);
	}
	return m_iWidth;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

CXUI_Font::CXUI_Font(std::wstring const& wFontKey)
{
	m_iFontWidth = -1;
	m_iFontHeight = -1;
	m_FontFace = NULL;
	m_wFontKey = wFontKey;

	m_iBoldSize = 1;
	m_Style = FS_NONE;
	m_Style2 = FS_NONE;
}

CXUI_Font::~CXUI_Font()
{
	FT_Done_Face(m_FontFace);
}
CXUI_Font::FONT_STYLE	CXUI_Font::GetStyle()	 const
{	
	return	(FONT_STYLE)(m_Style | m_Style2);
}
void CXUI_Font::SetStyle(FONT_STYLE style)	
{ 
	m_Style = style;	
}

void CXUI_Font::SetStyle2(FONT_STYLE style)	
{ 
	m_Style2 = style;
}

int	CXUI_Font::InitFontFile(std::wstring const& wstrFontFileName, int const iFontSize, FT_Library &ftlib)
{
	int iError = 0;
	iError = FT_Set_Char_Size(m_FontFace, iFontSize*64, iFontSize*64,0,0);

	if(iError)
	{	assert(NULL);
		return iError;
	}

	m_iFontWidth = iFontSize;
	m_iFontHeight = iFontSize;

	FT_Error error = FT_Select_Charmap(m_FontFace,FT_ENCODING_UNICODE);
	m_Encoding = FT_ENCODING_UNICODE;
	if (error != 0)
	{
		error = FT_Select_Charmap(m_FontFace,FT_ENCODING_WANSUNG);
		m_Encoding = FT_ENCODING_WANSUNG;
		if (error != 0)
		{
			m_Encoding = FT_ENCODING_NONE;
			::MessageBox(NULL,_T("Font Loading Failed"),wstrFontFileName.c_str(),MB_OK);
			return	iError;
		}
	}

	return iError;
}

void CXUI_Font::SetFontSize(int const iWidth, int const iHeight)
{
	if(m_iFontWidth == iWidth && m_iFontHeight == iHeight){return;}

	int const iError = FT_Set_Char_Size(m_FontFace, iWidth*64, iHeight*64,0,0);

	if(iError)
	{	//	failed
		assert(NULL);
		return ;
	}

	m_iFontWidth = iWidth;
	m_iFontHeight = iHeight;
}

POINT2	CXUI_Font::GetFontSize()const
{
	return POINT2(m_iFontWidth, GetHeight()); 
}

int CXUI_Font::GetHeight()const
{
	return m_iFontHeight+4;
}

unsigned	short	CXUI_Font::GetCodeByEncoding(unsigned	short sUnicode) const
{
	if(m_Encoding == FT_ENCODING_UNICODE) return sUnicode;
	if(m_Encoding == FT_ENCODING_WANSUNG)
	{
		WCHAR	unichar[2];
		char	str_mbchar[100];
		BOOL	bUsedDefaultChar = true;
		unichar[0] = sUnicode;
		unichar[1] = 0;

		int iLen = WideCharToMultiByte(949,0,unichar,1,str_mbchar,100,"0",&bUsedDefaultChar);
		if(iLen == 1)
		{
			return	str_mbchar[0];
		}
		if(iLen == 2)
		{
			unsigned	short	sCode = 0;
			char	first = str_mbchar[0];
			char	second = str_mbchar[1];
			sCode = ((first<<8)&0xff00) | (second&0xff);
			return	sCode;
			
		}
	}
	return	0;
}

int CXUI_Font::MaxHeight(std::wstring const& text)const
{
	unsigned short sCode;
	size_t iLen = text.length();
	int iHeight = GetHeight();

	int iMaxHeight = iHeight;
	for(size_t i=0;i<iLen;i++)
	{
		sCode = GetCodeByEncoding((unsigned short)(text[i]));
		if(sCode==32)
		{
			continue;
		}

		POINT	ptSize;
		if(GetSizeFromSizeMap(sCode,ptSize))
		{
			if(ptSize.y>iMaxHeight)
			{
				iMaxHeight = ptSize.y;
			}
		}
	}

	return iMaxHeight;
}

bool	CXUI_Font::GetSizeFromSizeMap(unsigned short const sCode, POINT &kSize)const
{
	BM::CAutoMutex kLock(m_kMutex);

	PointMap::const_iterator itor = m_kSizeMap.find(sCode);
	if(itor != m_kSizeMap.end())
	{
		kSize = itor->second;
	}
	else
	{
		int const iError = FT_Load_Char( m_FontFace, sCode, FT_LOAD_RENDER);
		if ( iError )
		{	
			return	false;
		}

		FT_GlyphSlot slot = m_FontFace->glyph;

		kSize.x = slot->advance.x>>6;
		kSize.y = max(m_iFontHeight - slot->bitmap_top, 0) + slot->bitmap.rows;

		m_kSizeMap.insert(std::make_pair(sCode,kSize));
	}

	if(GetStyle() & FS_BOLD)
	{
		kSize.x += m_iBoldSize;
	}
	if(GetStyle() & FS_UNDERLINE)
	{
		kSize.y = m_iFontHeight+2;
	}
	return	true;
}

int CXUI_Font::CalcWidth(std::wstring const& text,int iTextLen)const
{
	if (iTextLen<0)
	{
		return 0;
	}
	unsigned short sCode;
	int iLen = (int)text.length();
	if(iTextLen == 0 || iTextLen>iLen) iTextLen = iLen;

	int iWidth = 0;
	for(int i=0;i<iTextLen;i++)
	{
		sCode = (unsigned short)text[i];
		iWidth += GetWidth(sCode);
	}
	return iWidth;
}

int const CXUI_Font::CalcWidthAddReturn(std::wstring const& text, std::wstring &rkOut, VEC_LINE &rkLine, int const iWidth)
{
	unsigned short sCode;
	int iTextLen = (int)text.length();
	if(iTextLen == 0) 
	{
		return 1;
	}

	int iTempWidth = 0;
	INT iLineCount = 1;
	SLineInfo kTemp;
	bool bAddEmptyLine = false;
	for(int i=0;i<iTextLen;)
	{
		bAddEmptyLine = false;
		sCode = (unsigned short)text[i];
		if (_T('\n') == text[i])
		{
			rkOut.push_back(text[i]);
			kTemp.m_bReturn = true;	//개행
			kTemp.m_iLen = iTempWidth;
			rkLine.push_back(kTemp);
			iTempWidth = 0;// 개행문자를 만났으니까
			kTemp.Init();
			++iLineCount;
			bAddEmptyLine = true;
			++i;
		}
		else if (_T('\r') == text[i])
		{
			//rkOut.push_back(text[i]);
			//kTemp.m_bReturn = true;	//개행
			//kTemp.m_iLen = iTempWidth;
			//rkLine.push_back(kTemp);
			//iTempWidth = 0;// 개행문자를 만났으니까
			//kTemp.Init();
			//bAddEmptyLine = true;
			++i;
		}
		else
		{
			iTempWidth += GetWidth(sCode);

			if (iTempWidth <= iWidth)
			{
				rkOut.push_back(text[i]);
				kTemp.m_kWstr.push_back(text[i]);
				++i;
			}
			else
			{
				//rkOut+=_T("\r\n");
				kTemp.m_iLen = iTempWidth;
				kTemp.m_bReturn = false;	//워드랩
				rkLine.push_back(kTemp);
				kTemp.Init();
				iTempWidth = 0;
				++iLineCount;
				bAddEmptyLine = true;
			}
		}
	}

	if (!kTemp.m_kWstr.empty() || bAddEmptyLine)
	{
		kTemp.m_iLen = iTempWidth;
		rkLine.push_back(kTemp);
	}
	return iLineCount;
}

int	CXUI_Font::GetHeight(const TCHAR code)const
{
	POINT	ptSize;
	if(GetSizeFromSizeMap((unsigned short)code,ptSize))
	{
		return	ptSize.y;
	}
	return 0;
}

int CXUI_Font::GetWidth(const TCHAR code)const
{
	POINT	ptSize;

	if(GetSizeFromSizeMap((unsigned short)code,ptSize) )
	{
		return	ptSize.x;
	}

	return	0;

	//if(GetStyle()&FS_BOLD)
	//{
	//	FT_Bitmap *pBitmap = &slot->bitmap;
	//	if(pBitmap)
	//	{
	//		FT_Bitmap_Embolden(g_kFontMgr.GetFTLib(),pBitmap,m_iBoldSize<<5,0);
	//	}
	//}

	//int iBitmapWidth = slot->bitmap.width;
	//if(slot->bitmap_left>0)
	//{
	//	iBitmapWidth += slot->bitmap_left;
	//}

	//if(code == 32 && iBitmapWidth==0)
	//{
	//	iBitmapWidth = m_iFontHeight/2;
	//}

	//return iBitmapWidth;
}

//	메모리에 그린다
int CXUI_Font::Draw(_ARGB16 * ptr, int const iMaxBuffSize, int const dx, int const dy, int const pitch, wchar_t const* strText, _ARGB16 const& color)
{
	int iDx = dx;
	while(*(strText))
	{
		iDx += Draw(ptr,iMaxBuffSize,iDx,dy,pitch,*strText,color);
		++strText;
	}
	return	1;
}

int CXUI_Font::Draw(_ARGB16 * ptr, int const iMaxBuffSize, int const dx, int const dy, int const pitch, unsigned short const code,_ARGB16 const& color)
{
	int const iError = FT_Load_Char( m_FontFace, (unsigned short)GetCodeByEncoding(code), FT_LOAD_RENDER );
	if(iError)
	{//	failed
		assert(NULL);
		return -1;
	}

	FT_GlyphSlot const slot = m_FontFace->glyph;

	int	const iAdvance = slot->advance.x>>6;

	int idx = slot->metrics.horiBearingX>>6;
	int idy = max(m_iFontHeight - slot->bitmap_top, 0);

	int	iSrcLeft = 0;
	if(idx<0 && dx+idx<0) 
	{
		iSrcLeft = -idx;
		idx = 0;
	}

	FT_Bitmap *pBitmap = &slot->bitmap;
	if(GetStyle()&FS_BOLD)
	{
		FT_Bitmap_Embolden(g_kFontMgr.GetFTLib(),pBitmap,m_iBoldSize<<5,0);
	}

	int iBitmapPitch = static_cast<int>(fabs((float)pBitmap->pitch));
	unsigned char *pBitmapBuf = pBitmap->buffer;
	int iBitmapWidth = pBitmap->width;
	int iBitmapHeight = pBitmap->rows;

	//DWORD thisColor = color;
	_ARGB16 thisColor = color;

	int	iBufLoc = 0;

	int	iMin = (idy+dy)*pitch+idx+dx;
	int	iMax = (idy+dy+iBitmapHeight-1)*pitch+idx+dx+(iBitmapWidth-iSrcLeft)-1;

	if(GetStyle()&FS_UNDERLINE)
	{
		iMin = min(iMin,(dy+m_iFontHeight+2)*pitch+dx);
		iMax = max(iMax,(dy+m_iFontHeight+2)*pitch+dx+iAdvance-1);
	}

	if(iMin<0 || iMax>=iMaxBuffSize)
	{
		return	iAdvance;
	}

	switch(pBitmap->pixel_mode)
	{
	case FT_PIXEL_MODE_MONO:
		{
			
			int iByteLeftLoc;
			for(int i=0;i<iBitmapHeight;i++)
			{
				for(int j=iSrcLeft;j<iBitmapWidth;j++)
				{
					iByteLeftLoc = i*iBitmapPitch+j/8;

					if(((*(pBitmapBuf+iByteLeftLoc))&(1<<(7-j%8)))!=0)
					{
						thisColor.sARGB = color.sARGB | 0xf000;
						//thisColor = thisColor | 0xFF000000;
					}
					else
					{
						thisColor.sARGB = color.sARGB & 0x0fff;
						//thisColor = thisColor & 0x00FFFFFF;
					}

					iBufLoc = (idy+i+dy)*pitch+(j+idx+dx);
					(ptr+iBufLoc)->sARGB |= thisColor.sARGB;
				}
			}
		}break;
	case FT_PIXEL_MODE_GRAY:
		{
			unsigned	char	*pkBuf = NULL;
			for(int i=0;i<iBitmapHeight;i++)
			{
				iBufLoc = (idy+i+dy)*pitch+idx+dx;
				pkBuf = pBitmapBuf+i*iBitmapPitch+iSrcLeft;
				for(int j=iSrcLeft;j<iBitmapWidth;j++)
				{

					(ptr+iBufLoc++)->sARGB |= (color.sARGB & 0x0fff) | ((*(pkBuf++))<<8);

				}
			}
		}break;
	}

	if(GetStyle()&FS_UNDERLINE)
	{
		int iBufLoc = (dy+m_iFontHeight+2)*pitch+dx;
		for(int j=0;j<iAdvance;j++)
		{
			(ptr+iBufLoc++)->sARGB = 0xffff;
		}
	}

	return iAdvance;
}
//	-- CXUI_2DString ----------------------------------------------
CXUI_2DString::CXUI_2DString(CXUI_Style_String const& kText)
{
	m_kText = kText;
}
CXUI_2DString::CXUI_2DString(PgFontDef const& kFontDef,std::wstring const& wString)
{
	CreateStyleString(kFontDef,wString);
}

void CXUI_2DString::CreateStyleString(PgFontDef const& kFontDef,std::wstring const& wString)
{
	m_kText.CreateStyleString(kFontDef,wString);
}

// -- CXUI_Style_String ----------------------------------------------
CXUI_Style_String::CXUI_Style_String()
{
	Clear();
}

CXUI_Style_String::CXUI_Style_String(PgFontDef const& kFontDef,std::wstring const& wString)
{
	CreateStyleString(kFontDef, wString);
}

void CXUI_Style_String::Clear()
{	
	m_AdjustedString.clear();
	m_OriginalString.clear();
	m_vChar.clear();	
	m_ptSize.x = m_ptSize.y = 0;
	m_kExtraDataPackInfo.ClearAllExtraDataPack();
}

void CXUI_Style_String::operator = (CXUI_Style_String const& kStyleString)
{
	const CONT_PARSED_CHAR	&vOriginal = kStyleString.GetCharVector();

	m_vChar.clear();
	m_vChar.reserve(vOriginal.size());

	for(int i=0;i<(int)vOriginal.size();i++)
	{
		m_vChar.push_back(vOriginal[i]);
	}

	SetOriginalString(kStyleString.GetOriginalString());

	m_kExtraDataPackInfo = kStyleString.m_kExtraDataPackInfo;
	m_ptSize = kStyleString.GetSize();
}

bool CXUI_Style_String::operator == (CXUI_Style_String const& kStyleString)
{
	return	IsEqual(kStyleString);
}

bool CXUI_Style_String::operator != (CXUI_Style_String const& kStyleString)
{
	return	!IsEqual(kStyleString);
}

bool CXUI_Style_String::operator == (std::wstring const& wString)
{
	return	IsEqual(wString);
}

bool CXUI_Style_String::operator != (std::wstring const& wString)
{
	return	!IsEqual(wString);
}
void CXUI_Style_String::RecalculateAlignHeight()
{
	size_t const iLen = m_vChar.size();
	if(iLen == 0)
	{
		return;//	m_ptSize.y;
	}

	int iAccumHeight = 0, iMaxHeight = 0;
	int iTempWidth = 0, iMaxWidth = 0;
	CONT_PARSED_CHAR::iterator sub_iter = m_vChar.begin();
	CONT_PARSED_CHAR::iterator loop_iter = m_vChar.begin();
	while( m_vChar.end() != loop_iter )
	{
		CONT_PARSED_CHAR::value_type &rkChar = (*loop_iter);

		if( rkChar.m_FontDef.m_pFont )
		{
			iMaxHeight = max(rkChar.m_FontDef.m_pFont->GetHeight(), iMaxHeight);
		}

		if( rkChar.m_wChar == _T('\n') )
		{
			while( loop_iter != sub_iter ) // \n 전까지
			{
				(*sub_iter).m_iAlignHeight = iMaxHeight;
				++sub_iter;
			}
			rkChar.m_iAlignHeight = iMaxHeight; // \n 포함

			iAccumHeight += iMaxHeight;
			iMaxWidth = max(iTempWidth, iMaxWidth);

			iTempWidth = 0;
			iMaxHeight = 0;
			sub_iter = ++loop_iter;
			continue;
		}

		iTempWidth += static_cast<int>(rkChar.Width());

		++loop_iter;
	}

	while( m_vChar.end() != sub_iter )
	{
		(*sub_iter).m_iAlignHeight = iMaxHeight;
		++sub_iter;
	}

	m_ptSize.y = (iMaxHeight)? iAccumHeight + iMaxHeight: iAccumHeight;
	m_ptSize.x = max(iTempWidth, iMaxWidth);;
}

void CXUI_Style_String::AddParsedChar(PgParsedChar& kChar,int iCharWidth)
{
	if(iCharWidth == -1) iCharWidth = (int)kChar.Width();
	m_ptSize.x += iCharWidth;

	m_vChar.push_back(kChar);

	int const iHeight = kChar.m_iAlignHeight;
	if(iHeight > m_ptSize.y)
	{
		m_ptSize.y = iHeight;
	}
}

void CXUI_Style_String::operator += (PgParsedChar &kChar)
{
//	kChar.m_FontDef.m_pFont->SetStyle( (XUI::CXUI_Font::FONT_STYLE)kChar.m_FontDef.dwOptionFlag);
	AddParsedChar(kChar);
}

void CXUI_Style_String::operator += (CXUI_Style_String const& kStyleString)
{
	const CONT_PARSED_CHAR	&vOriginal = kStyleString.GetCharVector();

	for(int i=0;i<(int)vOriginal.size();i++)
	{
		m_vChar.push_back(vOriginal[i]);
	}

	SetOriginalString(GetOriginalString()+kStyleString.GetOriginalString());

	m_ptSize.x += kStyleString.GetSize().x;
	m_ptSize.y = max(m_ptSize.y,kStyleString.GetSize().y);
}

PgParsedChar&	CXUI_Style_String::operator[](int _Pos)
{
	return	m_vChar[_Pos];
}

bool	CXUI_Style_String::IsEqual(CXUI_Style_String const& kStyleString)
{
	return (IsEqual(kStyleString.GetOriginalString()) && Length() == kStyleString.Length());
}

bool	CXUI_Style_String::IsEqual(std::wstring const& wString)
{
	return GetOriginalString() == wString;
}

void CXUI_Style_String::CreateStyleString(PgFontDef const& kFontDef,std::wstring const& wString)
{
	Clear();
	std::list<PgParsedText> kParseList;
	ParseText(wString, kParseList,kFontDef);

	m_vChar.reserve(wString.length()*1.5);

	std::list<PgParsedText>::iterator itor = kParseList.begin();

	PgParsedChar	kTemp(_T('\0'),NULL,0,-1);
	while(itor != kParseList.end())
	{
		PgParsedText *pParsedText = &(*itor);

		size_t const iLen = pParsedText->strText.size();
		kTemp.m_FontDef.Set(pParsedText->m_FontDef.m_pFont,pParsedText->m_FontDef.dwColor,pParsedText->m_FontDef.dwOptionFlag);
		kTemp.m_iAlignHeight = pParsedText->iAlignHeight;
		kTemp.m_iExtraDataPackIndex = pParsedText->m_iExtraDataPackIndex;
		for(size_t i=0; i<iLen; i++)
		{
			kTemp.m_wChar = pParsedText->strText[i];
			kTemp.m_iWidth = -1;

			AddParsedChar(kTemp);
		}
		++itor;
	}
	SetOriginalString(wString);
	RecalculateAlignHeight();
}

int CXUI_Style_String::GetCharWidth(const PgParsedChar &wChar) 
{
	wChar.m_FontDef.m_pFont->SetStyle((XUI::CXUI_Font::FONT_STYLE)wChar.m_FontDef.dwOptionFlag);
	return wChar.m_FontDef.m_pFont->GetWidth(wChar.m_wChar);
}

std::wstring::size_type	CXUI_Style_String::ParseNextOption(std::wstring const &kOptionStr,std::wstring::size_type iStartPos,std::wstring &koutOptionType,std::wstring &koutOptionValue)
{

	std::wstring::size_type iTypeEndPos = kOptionStr.find( _T('='), iStartPos);
	std::wstring::size_type	iRemainLen = iTypeEndPos - iStartPos;

	if(iTypeEndPos == std::wstring::npos)
	{
		return	std::wstring::npos;
	}

	std::wstring::size_type	iTypeStartPos = std::wstring::npos;
	WCHAR	kChar;
	for(std::wstring::size_type i=0;i<iRemainLen;i++)
	{
		kChar = kOptionStr[i+iStartPos];
		if(kChar != ' ')
		{
			iTypeStartPos = i+iStartPos;
			break;
		}
	}
	
	if(iTypeStartPos == std::wstring::npos)
	{
		return	std::wstring::npos;
	}

	std::wstring::size_type	iSpacePos = kOptionStr.find( _T(' '), iTypeStartPos);

	iTypeEndPos = (iSpacePos == std::wstring::npos || iSpacePos>iTypeEndPos) ? iTypeEndPos : iSpacePos;

	koutOptionType = kOptionStr.substr(iTypeStartPos,iTypeEndPos-iTypeStartPos);

	std::wstring::size_type value_start = kOptionStr.find( _T('='), iStartPos);
	std::wstring::size_type value_end = kOptionStr.find( _T('/'), iStartPos);

	if(std::wstring::npos != value_start)
	{

		bool	bIsBinary = false;
		wchar_t	kChar = kOptionStr[value_start+1];
		if(kChar == _T('\"'))
		{
			bIsBinary = true;
		}

		if(bIsBinary)	//	Binary 데이터는 큰 따옴표 안에 들어간다. 시작 큰따옴표 바로 다음의 2바이트는 Size
		{
			int	iDataLen = 0;
			memcpy(&iDataLen,&kOptionStr[value_start+2],sizeof(int));
			int	iLenCharCount = sizeof(int)/sizeof(TCHAR);
			if(sizeof(int)%sizeof(TCHAR) != 0)
			{
				iLenCharCount++;
			}
			
			unsigned	short	usCharLen = static_cast<unsigned short>(iDataLen/sizeof(TCHAR)+( (iDataLen%sizeof(TCHAR) == 0) ? 0 : 1 ));

			value_end = value_start + 1+ iLenCharCount + usCharLen + 2;
		}
	}

	if(std::wstring::npos != value_start
	&&	std::wstring::npos != value_end)
	{

		koutOptionValue = kOptionStr.substr(value_start+1, value_end-value_start-1);
		return value_end+1;
	}

	return	std::wstring::npos;
}

bool CXUI_Style_String::ParseValue(std::wstring const& rkOrg, wchar_t option, std::wstring &Out)
{
	std::wstring::size_type const pos_org = rkOrg.find(option);

	if(pos_org != std::wstring::npos)
	{
		std::wstring::size_type const value_start = rkOrg.find( _T('='), pos_org);
		std::wstring::size_type const value_end = rkOrg.find( _T('/'), pos_org);

		if(std::wstring::npos != value_start
		&&	std::wstring::npos != value_end)
		{
			Out = rkOrg.substr(value_start+1, value_end-value_start-1);
			return true;
		}
	}
	return false;
}

bool CXUI_Style_String::ParseText(std::wstring const& strText, std::list<PgParsedText> &rkOutList, PgFontDef const& kFontDef)
{
	if (strText.empty() || kFontDef.m_pFont == NULL )
	{
		return false;
	}
	std::wstring::size_type start = strText.find(_T('{'));
	
	PgParsedText kPrevParseText;
	kPrevParseText.m_FontDef = kFontDef;
	kPrevParseText.iAlignHeight = kFontDef.m_pFont->GetHeight();

	if( start == std::wstring::npos )
	{//옵션이 없음
		kPrevParseText.Set(kFontDef, kFontDef.m_pFont->MaxHeight(strText), strText);
		rkOutList.push_back(kPrevParseText);
		return true;
	}

	if( start != 0 )
	{// 앞쪽에 옵션이 없는 파츠
		std::wstring parsed = strText.substr(0, start);
		kPrevParseText.Set(kFontDef, kFontDef.m_pFont->GetHeight(), strText.substr(0, start));
		kPrevParseText.strText = parsed;
		rkOutList.push_back(kPrevParseText);
	} 

	// { } 마크를 찾아서 옵션 파트를 뜯는다. 
	// {} 마크가 없는 부분은 일반으로 처리

	DWORD	dwSavedColor = kFontDef.dwColor;
	XUI::CXUI_Font *pkSavedFont = kFontDef.m_pFont;

	while(start != std::wstring::npos)
	{
		PgParsedText kParseText = kPrevParseText;
		kParseText.strText = _T("");

		PgExtraDataPack	kExtraDataPack;
		int	iExtraDataPackIndex = -1;

		std::wstring::size_type end = strText.find(_T('}'),start);
		if(end != std::wstring::npos)
		{//내부에서 끊자.
			std::wstring const strOption = strText.substr(start, end-start);

			std::wstring kOptionType,kOptionValue;

			std::wstring::size_type	iOptionPos = 1;
			while((iOptionPos = ParseNextOption(strOption,iOptionPos,kOptionType,kOptionValue)) != std::wstring::npos)
			{
				if(kOptionType == _T("C"))
				{
					DWORD dwColor = 0;

					::swscanf_s(kOptionValue.c_str(), _T("%x"), &dwColor);

					if(kOptionValue == std::wstring(XUI_SAVE_COLOR))
					{
						dwSavedColor = kParseText.m_FontDef.dwColor;
					}
					else if(kOptionValue == std::wstring(XUI_RESTORE_COLOR))
					{
						kParseText.m_FontDef.dwColor = dwSavedColor;
					}
					else
					{
						kParseText.m_FontDef.dwColor = dwColor;
					}
				}
				else if(kOptionType == _T("O"))
				{
					int	iStyle = 0;
					if(kOptionValue.find(_T('B'))!=std::wstring::npos) iStyle|=CXUI_Font::FS_BOLD;
					if(kOptionValue.find(_T('I'))!=std::wstring::npos) iStyle|=CXUI_Font::FS_ITALIC;
					if(kOptionValue.find(_T('U'))!=std::wstring::npos) iStyle|=CXUI_Font::FS_UNDERLINE;
					kParseText.m_FontDef.dwOptionFlag = iStyle;
				}
				else if(kOptionType == _T("T"))
				{
					CXUI_Font	*pkFont = g_kFontMgr.GetFont(kOptionValue);
					assert(pkFont);

					if(!pkFont)
					{
						pkFont = kFontDef.m_pFont;
					}

					if(kOptionValue == std::wstring(XUI_SAVE_FONT))
					{
						pkSavedFont = kParseText.m_FontDef.m_pFont;
					}
					else if(kOptionValue == std::wstring(XUI_RESTORE_FONT))
					{
						kParseText.m_FontDef.m_pFont = pkSavedFont;
					}
					else
					{
						kParseText.m_FontDef.m_pFont = pkFont;
					}
				}
				else if(kOptionType == _T("EP"))
				{
					iExtraDataPackIndex = atoi(MB(kOptionValue));
				}
				else if(kOptionType != _T("END"))	
				{
					kExtraDataPack.AddExtraData(kOptionType,kOptionValue);
				}
				
			}

		}

		if(iExtraDataPackIndex == -1)
		{
			iExtraDataPackIndex = m_kExtraDataPackInfo.AddExtraDataPack(kExtraDataPack);
		}

		kParseText.m_FontDef.m_pFont->SetStyle((XUI::CXUI_Font::FONT_STYLE)kParseText.m_FontDef.dwOptionFlag);
		kParseText.iAlignHeight = kParseText.m_FontDef.m_pFont->MaxHeight(kParseText.strText);
		kParseText.m_iExtraDataPackIndex = iExtraDataPackIndex;

		if( end != std::wstring::npos	)//끝마크있다
		{
			//다음 시작 마크를 찾고
			//있으면 거기서 끊고 돌기
			//없으면 쭈욱 떼고 끝
			std::wstring::size_type next_start = strText.find(_T('{'), end);//다음 시작 지점이 있느냐?
			
			if(next_start != std::wstring::npos)
			{
				kParseText.strText = strText.substr(end+1, next_start-end-1);
			}
			else
			{
				kParseText.strText = strText.substr(end+1);
			}
	
			rkOutList.push_back(kParseText);
			kPrevParseText = kParseText;
			start = next_start;
		}
		else
		{//끝마크 없다 이건 일반 스트링이야
			kParseText.strText = strText.substr(start);
			rkOutList.push_back(kParseText);
			break;
		}
	}
	return true;
}
void CXUI_Style_String::ProcessWordWrap(int iLimitWidth,bool bUseUnitWordWrap,CXUI_Style_String &kout_Result,int iIndent)
{
	if(iLimitWidth<=0) 
	{
		kout_Result = *this;
		return;
	}

	size_t	iTextLength = m_vChar.size();

	if(iTextLength == 0)
	{
		kout_Result = *this;
		return;
	}


	CXUI_Font	*pkLastFont = NULL;
	int	iLastFontHeight = 12;
	int	iLastExtraDataPackIndex = -1;

	PgParsedChar	kChar(_T('\0'),NULL,0,-1);
	int	iTextWidth = 0;
	bool	bLineSkipped = false;
	XUI::CXUI_Style_String kResultString,kTempString;
	for(size_t i=0;i<iTextLength;i++)
	{
		kChar = m_vChar[i];

		pkLastFont = kChar.m_FontDef.m_pFont;
		assert(pkLastFont);
		if(pkLastFont)
		{
			iLastFontHeight = pkLastFont->GetHeight();
		}

		size_t	const	iCharWidth = kChar.Width();
		
		if(bUseUnitWordWrap)
		{
			bool	bIsLineFeed = false;

			if(kChar == _T('\n'))
			{
				bIsLineFeed = true;
			}
			else if(kChar == _T('\r'))	//개행문자는 \r\n일 수도 있다
			{
				//kTempString += kChar;
				continue;
			}
// 			if(kChar == _T('\\'))
// 			{
// 				if(i<iTextLength-1)
// 				{
// 					if(m_vChar[i+1] == _T('n'))
// 					{
// 						bIsLineFeed = true;
// 						i++;
// 					}
// 				}
// 			}

			if(bIsLineFeed)
			{
				kResultString += kTempString;
				kResultString += PgParsedChar(_T('\n'),pkLastFont,iLastFontHeight,-1);
				kTempString.Clear();
				iTextWidth = 0;
				bLineSkipped = true;
				continue;
			}
			if(kChar.m_wChar <= 32)
			{
				kResultString += kTempString;
				kTempString.Clear();
				bLineSkipped = false;
			}
			if(iIndent+iTextWidth+iCharWidth>iLimitWidth)
			{
				if(kResultString.Length() == 0 || bLineSkipped)
				{
					kResultString += kTempString;
					kResultString += PgParsedChar(_T('\n'),pkLastFont,iLastFontHeight,-1);
					kTempString.Clear();
					iTextWidth = 0;

					bLineSkipped = true;
				}
				else
				{
					kResultString += PgParsedChar(_T('\n'),pkLastFont,iLastFontHeight,-1);

					CXUI_Style_String	kTemp2;
					size_t	iTempLength = kTempString.Length();
					for(size_t j=1;j<iTempLength;j++)
					{
						kTemp2+=kTempString[j];
					}
					kTempString = kTemp2;
					iTextWidth = kTempString.GetSize().x;
					bLineSkipped = true;
				}
			}

			kTempString += kChar;
			iTextWidth+=iCharWidth;
		}
		else
		{
			if(iIndent+iTextWidth+iCharWidth>iLimitWidth)
			{
				kResultString+=PgParsedChar(_T('\n'),pkLastFont,iLastFontHeight,-1);
				iTextWidth = 0;
			}

			kResultString+=kChar;
			iTextWidth+=iCharWidth;
		}
	}

	if(bUseUnitWordWrap)
	{
		kResultString += kTempString;
	}

	kout_Result = kResultString;
	kout_Result.SetExtraDataPackInfo(GetExtraDataPackInfo());
}

bool CXUI_Style_String::AdjustText()
{
	if (m_vChar.empty())
	{
		return false;
	}
	m_AdjustedString.clear();
	CONT_PARSED_CHAR::iterator it = m_vChar.begin();
	for (;it != m_vChar.end(); ++it)
	{
		m_AdjustedString+= (it->m_wChar);
	}
	return true;
}

//	-- CXUI_FontManager ----------------------------------------------
//
CXUI_FontManager::CXUI_FontManager()
{
	m_CurrentFont = NULL;
	m_ftlib = NULL;
}

CXUI_FontManager::~CXUI_FontManager()
{
	Destroy();
}

void CXUI_FontManager::Init(F_Create2DStringFunc	 Create2DStringFunc, F_CreateFontFunc fCreateFontFunc)
{
	m_CurrentFont = NULL;
	m_Create2DStringFunc = Create2DStringFunc;
	m_CreateFontFunc = fCreateFontFunc;

	int error = FT_Init_FreeType(&m_ftlib);
	if(error)
	{
		//	FreeType 라이브러리 초기화 실패
		return;
	}
}
CXUI_2DString *CXUI_FontManager::CreateNew2DString(PgFontDef const& kFontDef, std::wstring const& Text)
{
	assert(m_Create2DStringFunc);
	if(kFontDef.m_pFont)
	{
		return	m_Create2DStringFunc(CXUI_Style_String(kFontDef,Text));
	}

	return NULL;
}
CXUI_2DString *CXUI_FontManager::CreateNew2DString(CXUI_Style_String const& kString)
{
	assert(m_Create2DStringFunc);
	return	m_Create2DStringFunc(kString);
}

POINT2 CXUI_FontManager::CalculateOnlySize(CXUI_Style_String const& kString)
{
	assert(m_CalculateOnlySizeFunc);
	return m_CalculateOnlySizeFunc(kString);
}

void CXUI_FontManager::Destroy()
{

	for(FontHash::iterator itor = m_kFontHash.begin(); itor != m_kFontHash.end(); itor++)
	{
		delete itor->second;
	}
	m_kFontHash.clear();

	if(m_ftlib)
	{
		FT_Done_FreeType(m_ftlib);
		m_ftlib = NULL;
	}

	for(FontFileHash::iterator itor = m_kFontFileHash.begin(); itor != m_kFontFileHash.end(); itor++)
	{
		SAFE_DELETE((*itor).second.m_pkByte);
	}

	m_kFontFileHash.clear();
}

CXUI_Font* CXUI_FontManager::AddNewFont(std::wstring const& wstrFontKey, std::wstring const& wstrFontFileName, int const iFontSize)
{
	std::wstring kFontName = wstrFontKey;
	UPR(kFontName);
// 	FontHash::iterator itor = m_kFontHash.find(kFontName);
// 
// 	if(itor != m_kFontHash.end())
// 	{
// 		assert(NULL);//
// 		return NULL;
// 	}

	CXUI_Font *newfont = new CXUI_Font(kFontName);
	assert(newfont);

	int iError = 0;

	if(NULL==m_CreateFontFunc)
	{
		iError = FT_New_Face(m_ftlib, MB(wstrFontFileName), 0, newfont->GetFontFace());
	}
	else
	{
		FontFileHash::const_iterator it = m_kFontFileHash.find(wstrFontFileName);

		if(it==m_kFontFileHash.end())	//처음이면
		{
			SFontFileInfo kNewInfo;
			kNewInfo.m_uiSize = m_CreateFontFunc(wstrFontFileName, m_ftlib, newfont->GetFontFace(), kNewInfo.m_pkByte);	//FT_Done_Face 호출 전까지 보관해야 함
			if(NULL==kNewInfo.m_pkByte || 0==kNewInfo.m_uiSize)
			{
				iError = 1;
			}
			else
			{
				m_kFontFileHash.insert(std::make_pair(wstrFontFileName, kNewInfo));
			}
		}
		else
		{
			SFontFileInfo const& rkNewInfo = (*it).second;
			if(rkNewInfo.m_pkByte)
			{
				iError = FT_New_Memory_Face(m_ftlib, rkNewInfo.m_pkByte, rkNewInfo.m_uiSize, 0, newfont->GetFontFace());
			}
		}
	}

	if(iError)	//뭔가 문제 있음
	{
		assert(NULL);
	}


	if(newfont->InitFontFile(wstrFontFileName, iFontSize, m_ftlib)) //0이 아니면 에러임
	{
		SAFE_DELETE(newfont);
		assert(NULL);
		return NULL;
	}

	if (wstrFontFileName == _T("../Font/NGULIM.TTF"))
	{
		newfont->SetStyle2(CXUI_Font::FS_BOLD);
	}

	auto ret = m_kFontHash.insert(std::make_pair(kFontName, newfont));

	if(!ret.second) //이미 있는 폰트면
	{
		SAFE_DELETE(newfont);	//로그인 화면으로 돌아갔을 때 다시 loginScene을 파싱하므로 assert를 추가하지 않음
	}

	CXUI_Font *pkAddedFont = (*ret.first).second;
	if(!m_CurrentFont && pkAddedFont)
	{
		m_CurrentFont = pkAddedFont;
	}
	return pkAddedFont;
}
CXUI_Font* CXUI_FontManager::AddNewFont(std::wstring const& wstrFontKey, CXUI_Font *pkFont)
{
	std::wstring kFontName = wstrFontKey;
	UPR(kFontName);
// 	FontHash::iterator itor = m_kFontHash.find(kFontName);
// 
// 	if(itor != m_kFontHash.end())
// 	{
// 		assert(NULL);//
// 		return NULL;
// 	}

	auto ret = m_kFontHash.insert(std::make_pair(kFontName, pkFont));
	if(ret.second)
	{
		CXUI_Font *pkAddedFont = (*ret.first).second;
		if(!m_CurrentFont)
		{
			m_CurrentFont = pkAddedFont;
		}
		return pkAddedFont;
	}

	assert(NULL);
	return NULL;
}

CXUI_Font*	CXUI_FontManager::GetFont(std::wstring const& wstrFontKey)
{
	std::wstring kFonrName = wstrFontKey;
	UPR(kFonrName);
	FontHash::iterator itor = m_kFontHash.find(kFonrName);
	if(itor != m_kFontHash.end())
	{
		return (*itor).second;
	}
	return	NULL;
}

CXUI_Font* CXUI_FontManager::SetFont(std::wstring const& wstrFontKey)
{
	CXUI_Font* pFont = GetFont(wstrFontKey);
	if(pFont)
	{
		m_CurrentFont = pFont;
		return pFont;
	}
	assert(NULL); 
	return NULL;
}

