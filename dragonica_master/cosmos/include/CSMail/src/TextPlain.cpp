// TextPlain.cpp: implementation of the CTextPlain class.
// Author: Wes Clyburn (clyburnw@enmu.edu)
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Header/TextPlain.h"
#include "../Header/MIMEMessage.h"
#include <cassert>
#include "../Header/stringUtil.h"
#include <tchar.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextPlain::CTextPlain( int nContentType, UINT nWrapPos )
	:CMIMEContentAgent( nContentType )
{
	m_nWrapPos = nWrapPos;	
}

CTextPlain::~CTextPlain()
{

}

std::wstring CTextPlain::GetContentTypeString()
{
	std::wstring s;
	s = _T("text/plain" );
	return s;
}

bool CTextPlain::AppendPart(LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath, std::wstring & sDestination)
{
	std::wstring sSubHeader;
	std::wstring sWrapped;
	sSubHeader = build_sub_header( szContent,
								   szParameters,
								   nEncoding,
								   bPath );
	sWrapped = wrap_text( szContent );
	sDestination += (sSubHeader + sWrapped);
	return true;
}

std::wstring CTextPlain::build_sub_header(LPCTSTR szContent, 
									 LPCTSTR szParameters, 
									 int nEncoding, 
									 bool bPath)
{
	std::wstring sSubHeader;
	
	sSubHeader = _T("Content-Type: ");
	sSubHeader += GetContentTypeString();
	sSubHeader += szParameters;
	sSubHeader += _T("\r\n");

	sSubHeader += _T("Content-Transfer-Encoding: ");
	switch( nEncoding )
	{
		// This class handles only 7bit encoding, but others
		//  may be added here.
		default:
			//Fall through to...
		case CMIMEMessage::_7BIT:
			{
				sSubHeader += _T("7Bit" );
			}
	}
	sSubHeader += _T("\r\n\r\n" );
	return sSubHeader;
}

std::wstring CTextPlain::wrap_text(LPCTSTR szText)
{
	std::wstring sTemp;
	std::wstring sLeft;
	std::wstring sRight;
	int lp = 0;
	UINT nCount = 0;
	int nSpacePos = 0;

	StringUtil su;

	assert( szText != NULL );
	if( szText == NULL )
	{
		return sTemp;
	}
	sTemp = szText;
	while( lp < (int)sTemp.size() )
	{
		if( sTemp[ lp ] == ' ' )
			nSpacePos = lp;
		// Reset counter on newline
		if ( su.Mid( sTemp, lp, 2 ) == _T("\r\n" ) )
		{
			nCount = 0;
		}
		// Wrap text at last found space
		if( nCount > m_nWrapPos )
		{
			sLeft = su.Left( sTemp, nSpacePos );
			sRight = su.Right( sTemp, ( (int)sTemp.size() - nSpacePos ));
			su.TrimRight(sLeft);
			su.TrimLeft(sRight);
			sLeft += _T("\r\n" );
			sTemp = sLeft + sRight;
			nCount = 0;
		}
		else
			nCount++;
		lp++;
	}
	return sTemp;
}
