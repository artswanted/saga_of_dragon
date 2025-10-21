
#include "stdafx.h"
#include "../Header/AppOctetStream.h"
#include "../Header/Base64.h"
#include "../Header/MIMEMessage.h"
#include <cassert>

// IMPORTANT: The number of bytes we read must be
//  a multiple of 3 because CBase64's Encode()
//  method will append padding characters ('=')
//  to make the output's size a multiple of 4.
//  (Base64 treats 3 8-bit bytes as 4 6-bit 'bytes').
//  MIME decoders are free to treat '=' as a signal
//  that there's no more data, so we don't want to pad
//  until we're supposed to.
// When at the end of the file, the # of bytes read
//  may not be a multiple of 3, but that's okay
//  because we DO want the padding chars then.

#define BYTES_TO_READ 54 // This number guarantess output won't
						 // won't exceed line-length limit

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppOctetStream::CAppOctetStream( int nContentType )
	:CMIMEContentAgent( nContentType )
{
}

CAppOctetStream::~CAppOctetStream()
{
}

bool CAppOctetStream::AppendPart(LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath, std::wstring & sDestination)
{
	
	HANDLE	 fAttachment;
////	HANDLE		fAttachment

	assert( szContent != NULL );
	// This class handles only file attachments, so
	// it ignores the bPath parameter.
	if( szContent == NULL )
		return false;


	fAttachment = CreateFile(szContent, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == fAttachment )
	{
		return false;
	}

	sDestination += build_sub_header( szContent,
								      szParameters,
									  nEncoding,
									  TRUE );
	attach_file( fAttachment, CMIMEMessage::BASE64, sDestination );
	CloseHandle(fAttachment);
	return true;
}


std::wstring CAppOctetStream::build_sub_header(LPCTSTR szContent, LPCTSTR szParameters, int nEncoding, bool bPath)
{
	std::wstring sSubHeader;
	std::wstring sTemp;
	TCHAR szFName[ _MAX_FNAME ] = {0,};
	TCHAR szExt[ _MAX_EXT ] = {0,};

	::_wsplitpath_s( szContent, NULL,0,  NULL, 0, szFName, sizeof(szFName), szExt, sizeof(szExt) );

	// This class ignores szParameters and nEncoding.
	// It controls its own parameters and only handles
	// Base64 encoding.
	if( bPath )
	{
		sTemp = _T("; file=");
		sTemp += szFName;
		sTemp += szExt;
	}
	else
	{
		sTemp = _T("" );
	}
	sSubHeader = _T("Content-Type: ");
	sSubHeader += GetContentTypeString();
	sSubHeader += sTemp;
	sSubHeader += _T("\r\n");

	sSubHeader += _T("Content-Transfer-Encoding: base64\r\n" );
	
	
	sTemp = _T("Content-Disposition: attachment; filename=");
	sTemp += szFName;
	sTemp += szExt;
	sTemp += _T("\r\n");
	sSubHeader += sTemp;
	// Signal end of sub-header.
	sSubHeader += _T("\r\n" ); // Warning: numerous concatenations
								// are inefficient.
	return sSubHeader;
}

std::wstring CAppOctetStream::GetContentTypeString()
{
	return _T("application/octet-stream" );
}


// Caller is responsible for opening and closing the file
void CAppOctetStream::attach_file(HANDLE &FileAtt, int nEncoding, std::wstring & sDestination)
{
	CMIMECode* pEncoder;
	DWORD nBytesRead;
	TCHAR szBuffer[ BYTES_TO_READ + 1 ];

	assert( INVALID_HANDLE_VALUE != FileAtt );
	if( FileAtt == NULL )
		return;
	switch( nEncoding )
	{
		// This class handles only Base64 encoding, but others
		//  may be added here.
		default:
			// Fall through to...
		case CMIMEMessage::BASE64:
			{
				pEncoder = new CBase64;

				if( NULL == pEncoder )
				{
					return;
				}
			}
	}
	if( pEncoder == NULL )	// Old habits are hard to break
		return;
	do
	{
		ReadFile(FileAtt, szBuffer, BYTES_TO_READ, &nBytesRead, NULL);

		szBuffer[ nBytesRead ] = 0;	// Terminate the std::wstring
		sDestination += pEncoder->Encode( szBuffer, nBytesRead );
		sDestination += _T("\r\n" );
	} while( nBytesRead == BYTES_TO_READ );
	sDestination += _T("\r\n" );
	delete pEncoder;
}
