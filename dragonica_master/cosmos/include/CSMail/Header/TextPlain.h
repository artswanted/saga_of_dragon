
#pragma once
#include "MIMEContentAgent.h"

// CTextPlain
// A MIME content agent that handles the "text/plain"
// content type
//
class CTextPlain : public CMIMEContentAgent  
{
public:
	CTextPlain( int nContentType, UINT nWrapPos = 72 );
	virtual ~CTextPlain();

	virtual bool AppendPart( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath, std::wstring& sDestination );
	virtual std::wstring GetContentTypeString();

protected:
	UINT m_nWrapPos;

	std::wstring wrap_text( LPCTSTR szText );
	virtual std::wstring build_sub_header( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath );
};

