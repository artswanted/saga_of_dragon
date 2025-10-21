#include "stdafx.h"
#include <cassert>
#include "BM/STLSupport.h"
#include "../Header/Base64.h"

// The 7-bit alphabet used to encode binary information
std::wstring CBase64::m_sBase64Alphabet = 
_T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );

int const CBase64::m_nMask[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase64::CBase64()
{
}

CBase64::~CBase64()
{
}

std::wstring CBase64::Encode(LPCTSTR szEncoding, size_t const nSize)
{
	std::wstring sOutput = _T("" );
	int nNumBits = 6;
	UINT nDigit;
	size_t lp = 0;

	assert( szEncoding != NULL );
	if( szEncoding == NULL )
		return sOutput;
	m_szInput = szEncoding;
	m_nInputSize = nSize;

	m_nBitsRemaining = 0;
	nDigit = read_bits( nNumBits, &nNumBits, lp );
	while( nNumBits > 0 )
	{
		sOutput += m_sBase64Alphabet[ (int)nDigit ];
		nDigit = read_bits( nNumBits, &nNumBits, lp );
	}
	// Pad with '=' as per RFC 1521
	while( sOutput.size() % 4 != 0 )
	{
		sOutput += _T('=');
	}
	return sOutput;
}

// The size of the output buffer must not be less than
// 3/4 the size of the input buffer. For simplicity,
// make them the same size.
int CBase64::Decode(LPCTSTR szDecoding, LPTSTR szOutput)
{
	std::wstring sInput;
    int c, lp =0;
	int nDigit;
    int nDecode[ 256 ];

	assert( szDecoding != NULL );
	assert( szOutput != NULL );
	if( szOutput == NULL )
	{
		return 0;
	}

	if( szDecoding == NULL )
	{
		return 0;
	}
	
	sInput = szDecoding;
	if( sInput.size() == 0 )
	{
		return 0;
	}

	// Build Decode Table
	//
	int i =0;
	for( i = 0; i < 256; i++ ) 
	{
		nDecode[i] = -2; // Illegal digit
	}

	for( i=0; i < 64; i++ )
	{
		nDecode[ m_sBase64Alphabet[ i ] ] = i;
		nDecode[ m_sBase64Alphabet[ i ] | 0x80 ] = i; // Ignore 8th bit
		nDecode[ _T('=') ] = -1; 
		nDecode[ _T('=') | 0x80 ] = -1; // Ignore MIME padding char
    }

	// Clear the output buffer
	memset( szOutput, 0, sInput.size() + 1 );

	// Decode the Input
	//
	for( lp = 0, i = 0; lp < (int)sInput.size(); lp++ )
	{
		c = sInput[ lp ];
		nDigit = nDecode[ c & 0x7F ];
		if( nDigit < -1 ) 
		{
			return 0;
		} 
		else if( nDigit >= 0 ) 
			// i (index into output) is incremented by write_bits()
			write_bits( nDigit & 0x3F, 6, szOutput, i );
    }	
	return i;
}

UINT CBase64::read_bits(int nNumBits, int * pBitsRead, size_t &lp)
{
    ULONG lScratch;
    while(	( m_nBitsRemaining < nNumBits ) 
	&&		( lp < m_nInputSize ) ) 
	{
		int c = m_szInput[ lp++ ];
        m_lBitStorage <<= 8;
        m_lBitStorage |= (c & 0xff);
		m_nBitsRemaining += 8;
    }
    if( m_nBitsRemaining < nNumBits ) 
	{
		lScratch = m_lBitStorage << ( nNumBits - m_nBitsRemaining );
		*pBitsRead = m_nBitsRemaining;
		m_nBitsRemaining = 0;
    } 
	else 
	{
		lScratch = m_lBitStorage >> ( m_nBitsRemaining - nNumBits );
		*pBitsRead = nNumBits;
		m_nBitsRemaining -= nNumBits;
    }
    return (UINT)lScratch & m_nMask[nNumBits];
}


void CBase64::write_bits(UINT nBits,
						 int nNumBits,
						 LPTSTR szOutput,
						 int& i)
{
	UINT nScratch;

	m_lBitStorage = (m_lBitStorage << nNumBits) | nBits;
	m_nBitsRemaining += nNumBits;
	while( m_nBitsRemaining > 7 ) 
	{
		nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
		szOutput[ i++ ] = nScratch & 0xFF;
		m_nBitsRemaining -= 8;
	}
}

