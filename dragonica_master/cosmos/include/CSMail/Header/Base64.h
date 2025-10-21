#pragma once

#include "MIMECode.h"

// CBase64
// An encoding agent that handles Base64
//
class CBase64 : public CMIMECode  
{
public:
	CBase64();
	virtual ~CBase64();

// Override the base class mandatory functions
	virtual int Decode( LPCTSTR szDecoding, LPTSTR szOutput );
	virtual std::wstring Encode( LPCTSTR szEncoding, size_t const nSize );

protected:
	void write_bits( UINT nBits, int nNumBts, LPTSTR szOutput, int& lp );
	UINT read_bits( int nNumBits, int* pBitsRead, size_t & lp );

	size_t m_nInputSize;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;
	LPCTSTR m_szInput;

	static int const m_nMask[];
	static std::wstring m_sBase64Alphabet;
private:
};
