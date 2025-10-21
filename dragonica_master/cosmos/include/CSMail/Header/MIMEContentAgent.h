#pragma once
// CMIMEContentAgent
// Abstract base class. Content agents support MIME
// content types on behalf of CMIMEMessage
//


#include <windows.h>
#include "BM/STLSupport.h"


class CMIMEContentAgent  
{
public:
	CMIMEContentAgent( int nMIMEType );
	virtual ~CMIMEContentAgent();

	bool QueryType( int nContentType );

	virtual bool AppendPart( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath, std::wstring& sDestination ) = 0;
	virtual std::wstring GetContentTypeString() = 0;

protected:
	virtual std::wstring build_sub_header( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath ) = 0;


private:
	int m_nMIMETypeIHandle;
};

