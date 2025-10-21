#pragma once
// CMIMECode
// Abstract base class. CMIMECode objects encode and decode
// MIME contents (examples: Base64, Quoted-Printable)
//

#include <windows.h>
#include "BM/STLSupport.h"

class CMIMECode  
{
public:
	CMIMECode();
	virtual ~CMIMECode();

	virtual int Decode( LPCTSTR szDecoding, LPTSTR szOutput ) = 0;
	virtual std::wstring Encode( LPCTSTR szEncoding, size_t const nSize ) = 0;
};

