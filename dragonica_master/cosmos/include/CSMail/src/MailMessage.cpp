// MailMessage.cpp: implementation of the CMailMessage class.
// Copyright (c) 1998, Wes Clyburn
//////////////////////////////////////////////////////////////////////


//*** Every modification marked with <JFO>
//*** have been added by Jean-Francois Ouellet lafaune@total.net (15 dec 98)
//***
//*** These modifications have been done to enhance the real good object
//*** of Wes.Clyburn, with the functionnality of CC and BCC copy and in the
//*** context of keeping the same public interface.
//***
//*** All new parameters are only optionnal.
//***
//*** Still thanks to Wes Clyburn.


//#include <afx.h>
#include "stdafx.h"
#include <windows.h>
#include <cassert>
#include <tchar.h>

#include "../Header/MailMessage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMailMessage::CMailMessage()
{
	m_strMailerName = _T("CS Mail" );
	SetCharsPerLine( 76 );
}

CMailMessage::~CMailMessage()
{

}

bool CMailMessage::AddRecipient( LPCTSTR szEmailAddress, LPCTSTR szFriendlyName, RECIPIENTS_TYPE type )
{
	assert( szEmailAddress != NULL );
	assert( szFriendlyName != NULL );
	CRecipient to;
	to.m_sEmailAddress = szEmailAddress;
	to.m_sFriendlyName = szFriendlyName;

   //*** Begin <JFO>
   //*** old line -> m_Recipients.Add( to );
   {
      switch(type)
      {
         case TO: m_Recipients.push_back( to );    break;
         case CC: m_CCRecipients.push_back( to );  break;
         case BCC:m_BCCRecipients.push_back( to ); break;
      }
   }
   //*** End

	return true;
}

// sEmailAddress and sFriendlyName are OUTPUT parameters.
// If the function fails, it will return FALSE, and the OUTPUT
// parameters will not be touched.
bool CMailMessage::GetRecipient(std::wstring & sEmailAddress, std::wstring & sFriendlyName, int nIndex, RECIPIENTS_TYPE type )
{
	CRecipient to;
	if( nIndex < 0 || nIndex > (int)m_Recipients.size() - 1 )
	{
		return false;
	}

	//*** Begin <JFO>
   //*** old line -> to = m_Recipients[ nIndex ];
   {
      switch(type)
      {
         case TO:  to = m_Recipients[ nIndex ];    break;
         case CC:  to = m_CCRecipients[ nIndex ];  break;
         case BCC: to = m_BCCRecipients[ nIndex ]; break;
      }
   }
   //*** End

	sEmailAddress = to.m_sEmailAddress;
	sFriendlyName = to.m_sFriendlyName;

	return true;
}

int CMailMessage::GetNumRecipients(RECIPIENTS_TYPE type )
{
   //*** Begin <JFO>
   //*** old line ->	return m_Recipients.GetSize();
      int number = 0;

      switch(type)
      {
         case TO:  number = (int)m_Recipients.size();    break;
         case CC:  number = (int)m_CCRecipients.size();  break;
         case BCC: number = (int)m_BCCRecipients.size(); break;
      }

      return number;

   //*** End
}
bool CMailMessage::AddMultipleRecipients( CS_MAIL::TARGET_LIST const& rTL, RECIPIENTS_TYPE type )
{
	CS_MAIL::STRING_LIST::const_iterator itor = rTL.lstTarget.begin();

	while( rTL.lstTarget.end() != itor)
	{
		if( !AddMultipleRecipients( (*itor), type ) )
		{
			return false;
		}
		++itor;
	}
	
	return true;
}

bool CMailMessage::AddMultipleRecipients(std::wstring const& strRecipients, RECIPIENTS_TYPE const type )
{
	std::wstring sTemp;
	std::wstring sEmail;
	std::wstring sFriendly;

	assert( strRecipients.size() );
	
	size_t const length = strRecipients.size();

	TCHAR* buf = new TCHAR[length+1];	// Allocate a work area (don't touch parameter itself)
	_tcscpy_s( buf, length+1, strRecipients.c_str() );

	int nMark = 0;
	int nMark2 = 0;

	for( int pos = 0, start = 0; pos <= (int)length; pos++ )
	{
		if( buf[ pos ] == _T(';') ||
			buf[ pos ] == 0 )
		{
			// First, pick apart the sub-strings (separated by ';')
			//  Store it in sTemp.
			//
			buf[ pos ] = 0;	// Redundant when at the end of std::wstring, but who cares.
			sTemp = &buf[ start ];

			// Now divide the substring into friendly names and e-mail addresses.
			//
			nMark = (int)sTemp.find( _T('<') );
			if( nMark >= 0 )
			{
				std::wstring tmp;
				int i = 0;
				for( i=0; i<nMark; i++ )
				{
					tmp +=  sTemp[i];
				}
				sFriendly = tmp;
				nMark2 = (int)sTemp.find( _T('>') );
				if( nMark2 < nMark )
				{
					delete[] buf;
					return false;
				}
				// End of mark at closing bracket or end of std::wstring
				nMark2 > -1 ? nMark2 = nMark2 : nMark2 = (int)sTemp.size() - 1;

				tmp= _T("");
				for( i=0; i< nMark2 - (nMark + 1); i++ )
				{
					tmp += sTemp[i + nMark + 1];
				}
				sEmail = tmp;
			}
			else
			{
				sEmail = sTemp;
				sFriendly = _T("");
			}
			AddRecipient( sEmail.c_str(), sFriendly.c_str(), type  );
			start = pos + 1;
		}
	}
	delete[] buf;
	return true;
}

void CMailMessage::FormatMessage()
{
	start_header();
	prepare_header();
	end_header();
	prepare_body();
}

void CMailMessage::SetCharsPerLine(UINT nCharsPerLine)
{
	m_nCharsPerLine = nCharsPerLine;
}	

UINT CMailMessage::GetCharsPerLine()
{
	return m_nCharsPerLine;
}

// Create header as per RFC 822
//
void CMailMessage::prepare_header()
{
   	std::wstring sTemp;

	sTemp = _T("" );
	// From:
	sTemp = _T("From: " ) + m_strSender;
	add_header_line( sTemp.c_str() );

	// To:
	sTemp = _T("To: " );
	std::wstring sEmail = _T("" );
	std::wstring sFriendly = _T("" );
	for( int i = 0; i < GetNumRecipients(); i++ )
	{
		GetRecipient( sEmail, sFriendly, i );
		sTemp += ( i > 0 ? _T("," ) : _T("" ) );
		sTemp += sFriendly;
		sTemp += _T("<" );
		sTemp += sEmail;
		sTemp += _T(">" );
	}
	add_header_line( sTemp.c_str() );


   //*** Begin <JFO>
   {
	   sTemp = _T("Cc: " );
	   std::wstring sEmail = _T("" );
	   std::wstring sFriendly = _T("" );
	   for( int i = 0; i < GetNumRecipients(CC); i++ )
	   {
		   GetRecipient( sEmail, sFriendly, i, CC );
		   sTemp += ( i > 0 ? _T("," ) : _T("" ) );
		   sTemp += sFriendly;
		   sTemp += _T("<" );
		   sTemp += sEmail;
		   sTemp += _T(">" );
	   }
	   add_header_line( sTemp.c_str() );

      //*** No BCC informations added..of course ;)
   }
   //*** End


	// Date:
    SYSTEMTIME time;
    GetLocalTime(&time);

	// Format: Mon, 01 Jun 98 01:10:30 GMT
//	sTemp = _T("Date: " );
//	sTemp += time.
	sTemp = GetStringFromTime(time);

	add_header_line( sTemp.c_str() );

	// Subject:
	sTemp = _T("Subject: " ) + m_strTitle;
	add_header_line( sTemp.c_str() );

	// X-Mailer
	sTemp = _T("X-Mailer: " ) + m_strMailerName;
	add_header_line( sTemp.c_str() );
}

void CMailMessage::prepare_body()
{
	// Append a CR/LF to body if necessary.
	std::wstring tmp;

	if( m_strBody.size() < 2 )
	{
		m_strBody += _T("\r\n" );
		return;
	}

	for( int i=0; i<2; i++ )
	{
		tmp += m_strBody[ m_strBody.size() - 2 + i ];
	}

	if( tmp != _T("\r\n" ) )
	{
		m_strBody += _T("\r\n" );
	}
}


void CMailMessage::start_header()
{
	m_strHeader = _T("" );
}

void CMailMessage::end_header()
{
	m_strHeader += _T("\r\n" );
}

void CMailMessage::add_header_line(LPCTSTR szHeaderLine)
{
	m_strHeader += szHeaderLine;
	m_strHeader += _T("\r\n");
}


std::wstring CMailMessage::GetStringFromTime(SYSTEMTIME& time)
{
	std::wstring s = _T("Date : ");
	TCHAR Buf[20] = {0,};

	switch( time.wDayOfWeek )
	{
	case 0 :
		{
			s += _T("Sun");
		}break;
	case 1 :
		{
            s += _T("Mon");
		}break;
	case 2 :
		{
            s += _T("Tue");
		}break;
	case 3 :
		{
            s += _T("Wed");
		}break;
	case 4 :
		{
            s += _T("Thu");
		}break;
	case 5 :
		{
            s += _T("Fri");
		}break;
	case 6 :
		{
            s += _T("Sat");
		}break;
	}
	s += _T(", ");

	_itot_s(time.wDay, Buf, 20, 10);

	s += Buf;

	// January = 1
	s += _T(' ');
	switch( time.wMonth )
	{
	case 1 :
		{
            s += _T("Jan");
		}break;
	case 2 :
		{
            s += _T("Feb");
		}break;
	case 3 :
		{
            s += _T("Mar");
		}break;
	case 4 :
		{
            s += _T("Apr");
		}break;
	case 5 :
		{
            s += _T("May");
		}break;
	case 6 :
		{
            s += _T("Jun");
		}break;
	case 7 :
		{
            s += _T("Jul");
		}break;
	case 8 :
		{
            s += _T("Aug");
		}break;
	case 9 :
		{
            s += _T("Sep");
		}break;
	case 10 :
		{
            s += _T("Oct");
		}break;
	case 11 :
		{
            s += _T("Nov");
		}break;
	case 12 :
		{
            s += _T("Dec");
		}break;
	}

    s += _T(' ');
	_itot_s(time.wYear, Buf, 20, 10);
	s += Buf;
	
	s += _T(' ');
	_itot_s(time.wHour, Buf, 20, 10);
	s += Buf;

	s += _T(':');
	_itot_s(time.wMinute, Buf, 20, 10);
	s += Buf;

	s += _T(':');
	_itot_s(time.wSecond, Buf, 20, 10);
	s += Buf;

	return s;
}

