// AppOctetStream.h: interface for the CAppOctetStream class.
// Author: Wes Clyburn (clyburnw@enmu.edu)
//////////////////////////////////////////////////////////////////////
#pragma once
#include "MIMEContentAgent.h"

// CAppOctetStream
// A MIME content agent that handles the
// "application/octet-stream" content type
//
class CAppOctetStream : public CMIMEContentAgent  
{
public:
	CAppOctetStream( int nContentType );
	virtual ~CAppOctetStream();

	virtual bool AppendPart( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath, std::wstring& sDestination );

	virtual std::wstring GetContentTypeString();
protected:
	virtual void attach_file( HANDLE &FileAtt, int nEncoding, std::wstring& sDestination );
	virtual std::wstring build_sub_header( LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath );
};

