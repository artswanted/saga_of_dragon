#include "stdafx.h"
#include <cassert>
#include "../Header/SMTP.h"
#include "../Header/Base64.h"
#pragma warning(disable : 4996) // Disable: 'inet_addr': Use inet_pton() or InetPton() instead or define

response_code const response_table[] =
{
	{ 250, _T("SMTP server error" ) },// GENERIC_SUCCESS
	{ 220, _T("SMTP server not available" ) },// CONNECT_SUCCESS
	{ 354, _T("SMTP server not ready for data" ) },// DATA_SUCCESS
	{ 221, _T("SMTP server didn't terminate session" ) },// QUIT_SUCCESS
	{ 334, _T("SMTP Auth Login" ) },// TRY_AUTH_ID_SUCCESS
	{ 235, _T("SMTP Auth Login" ) }// TRY_AUTH_PW_SUCCESS
};

CSMTP::CSMTP(std::wstring const& strID, std::wstring const& strPW, LPCTSTR szSMTPServerName, short const nPort )
{
	m_strID = strID;
	m_strPW = strPW;

	assert( szSMTPServerName != NULL );

	WSADATA wsd = {0,};
	if(::WSAStartup(WINSOCK_VERSION, &wsd) != 0)
	{
		assert(0);
	}

	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
	m_bConnected = false;
	m_sError = _T("OK");
}

CSMTP::~CSMTP()
{
	Disconnect();
	
	WSACleanup();      
}

bool CSMTP::Connect()
{
	std::wstring sHello;
	char local_host[ 80 ];	// Warning: arbitrary size
	if( m_bConnected )
		return true;
	
	//	Create Socket.
	m_sSMTPServer = socket(PF_INET, SOCK_STREAM, 0);
	if( !m_sSMTPServer )
	{
		m_sError = _T("Unable to create the socket." );
		return false;
	}

	//	Connecting...
	SOCKADDR_IN ServAddr;
	ZeroMemory(&ServAddr, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr( MB(m_sSMTPServerHostName));
	ServAddr.sin_port = htons( m_nPort );

	if( SOCKET_ERROR == connect( m_sSMTPServer, (SOCKADDR*)&ServAddr, sizeof(ServAddr)) )
	{
		m_sError = _T("Unable to connect to server" );
		closesocket(m_sSMTPServer);
		return false;
	}

	if( !get_response( CONNECT_SUCCESS ) )
	{
		m_sError = _T("Server didn't respond." );
		closesocket( m_sSMTPServer );
		return false;
	}
	gethostname( local_host, 80 );
	
	sHello = _T("HELO ");
	sHello += UNI(local_host);
	sHello += _T("\r\n");

	send( m_sSMTPServer, MB(sHello), (int)sHello.size(), 0 );

	if( !get_response( GENERIC_SUCCESS ) )
	{
		closesocket( m_sSMTPServer );
		return false;
	}

	sHello = _T("AUTH login");
	sHello += _T("\r\n");
	send( m_sSMTPServer, MB(sHello), (int)sHello.size(), 0 );

	if( !get_response( TRY_AUTH_ID_SUCCESS ) )
	{
		closesocket( m_sSMTPServer );
		return false;
	}

	//ID º¸³¿

	CBase64 kEnc;
//	sHello = _T("USER ");
	sHello = kEnc.Encode(m_strID.c_str(), m_strID.size());
	sHello += _T("\r\n");
	send( m_sSMTPServer, MB(sHello), (int)sHello.size(), 0 );

	if( !get_response( TRY_AUTH_ID_SUCCESS ) )
	{
		closesocket( m_sSMTPServer );
		return false;
	}

	//PW º¸³¿
	CBase64 kEnc2;
//	sHello = _T("PASS ");
	sHello = kEnc2.Encode(m_strPW.c_str(), m_strID.size());
	sHello += _T("\r\n");
	send( m_sSMTPServer, MB(sHello), (int)sHello.size(), 0 );

	if( !get_response( TRY_AUTH_PW_SUCCESS ) )
	{
		closesocket( m_sSMTPServer );
		return false;
	}


	m_bConnected = true;
	return true;
}

bool CSMTP::Disconnect()
{
	if( !m_bConnected )
	{
		return true;
	}
	// Disconnect gracefully from the server and close the socket
	std::wstring sQuit = _T("QUIT\r\n" );

	send( m_sSMTPServer, MB(sQuit), (int)sQuit.size(), 0 );

	// No need to check return value here.
	// If it fails, the message is available with GetLastError
	bool ret = get_response( QUIT_SUCCESS );
	closesocket( m_sSMTPServer );

	m_bConnected = false;
	return ret;
}

std::wstring CSMTP::GetLastError()
{
	return m_sError;
}

bool CSMTP::SendMessage(CMailMessage * pMsg)
{
	assert( pMsg != NULL );
	if(!m_bConnected)
	{
		m_sError = _T("Must be connected" );
		return false;
	}
	if(!FormatMailMessage(pMsg))
	{
		return false;
	}
	if(!transmit_message(pMsg))
	{
		return false;
	}
	return true;
}

bool CSMTP::FormatMailMessage( CMailMessage* msg )
{
	assert( msg != NULL );
   if( msg->GetNumRecipients() == 0 )
	{
		m_sError = _T("No Recipients" );
		return false;
	}
	msg->FormatMessage();
	return true;
}

void CSMTP::SetServerProperties( LPCTSTR szSMTPServerName, short const nPort)
{
	assert( szSMTPServerName != NULL );
	// Needs to be safe in non-debug too
	if(NULL == szSMTPServerName)
	{
		return;
	}

	m_sSMTPServerHostName = szSMTPServerName;
	m_nPort = nPort;
}

std::wstring CSMTP::cook_body(CMailMessage * msg)
{
	assert( msg != NULL );
	std::wstring sTemp;
	std::wstring sCooked = _T("" );
	LPTSTR szBad = _T("\r\n.\r\n" );
	LPTSTR szGood = _T("\r\n..\r\n" );
	int nPos;
	int nStart = 0;
	int nBadLength = (int)_tcslen( szBad );
	sTemp = msg->m_strBody;

	std::wstring ts;
	for( int i=0; i<3; i++ )
	{
		ts += sTemp[i];
	}
	if( ! ts.compare( _T(".\r\n" ) ) )
	{
		sTemp = _T("." ) + sTemp;
	}
	//
	// This is a little inefficient because it beings a search
	// at the beginning of the std::wstring each time. This was
	// the only thing I could think of that handled ALL variations.
	// In particular, the sequence "\r\n.\r\n.\r\n" is troublesome. 
	// (Even std::wstringEx's FindReplace wouldn't handle that situation
	// with the global flag set.)
	//
	while( (nPos = (int)sTemp.find( szBad )) > -1 )
	{
		std::wstring st;
		for( int i=0; i<nPos; i++ )
		{
			st += sTemp[nStart + i];
		}
		sCooked = st;
		sCooked += szGood;

		st=_T("");
		for( int i=0; i<(int)sTemp.size() - (nPos + nBadLength); i++ )
		{
			st += sTemp[ sTemp.size() - (sTemp.size() - (nPos + nBadLength)) + i ];
		}
		sTemp = sCooked + st;
	}
	return sTemp;
}

bool CSMTP::transmit_message(CMailMessage * msg)
{
	std::wstring sFrom;
	std::wstring sTo;
	std::wstring sTemp;
	std::wstring sEmail;

	assert( msg != NULL );
	if( !m_bConnected )
	{
		m_sError = _T("Must be connected" );
		return false;
	}

	// Send the MAIL command
	//
	sFrom = _T("MAIL From: <");
	sFrom += msg->m_strSender;
	sFrom += _T(">\r\n");

	send( m_sSMTPServer, MB(sFrom), (int)sFrom.size(), 0 );

	if( !get_response( GENERIC_SUCCESS ) )
	{
		return false;
	}
	
	// Send RCPT commands (one for each recipient)
	//
	int i = 0;
	for( i = 0; i < msg->GetNumRecipients(); i++ )
	{
		msg->GetRecipient( sEmail, sTemp, i );

		sTo = _T("RCPT TO: <");
		sTo += sEmail;
		sTo += _T(">\r\n");

		send( m_sSMTPServer, MB(sTo), (int)sTo.size(), 0 );
		if( !get_response( GENERIC_SUCCESS ) )
		{
			return false;
		}
	}

	//*** Begin <JFO>
	for( i = 0; i < msg->GetNumRecipients(CMailMessage::CC); i++ )
	{
		msg->GetRecipient( sEmail, sTemp, i, CMailMessage::CC );

		sTo = _T("RCPT TO: <");
		sTo += sEmail;
		sTo += _T(">\r\n");

		send( m_sSMTPServer, MB(sTo), (int)sTo.size(), 0 );
		if( !get_response( GENERIC_SUCCESS ) )
		{
			return false;
		}
	}

	for( i = 0; i < msg->GetNumRecipients(CMailMessage::BCC); i++ )
	{
		msg->GetRecipient( sEmail, sTemp, i, CMailMessage::BCC );

		sTo = _T("RCPT TO: <");
		sTo += sEmail;
		sTo += _T(">\r\n");

		send( m_sSMTPServer, MB(sTo), (int)sTo.size(), 0 );
	}
	//*** End

	// Send the DATA command
	sTemp = _T("DATA\r\n" );
	send( m_sSMTPServer, MB(sTemp), (int)sTemp.size(), 0 );
	if( !get_response( DATA_SUCCESS ) )
	{
		return false;
	}

	// Send the header
	send( m_sSMTPServer, MB(msg->m_strHeader), (int)msg->m_strHeader.size(), 0 );

	// Send the body
	sTemp = cook_body( msg );
	send( m_sSMTPServer, MB(sTemp), (int)sTemp.size(), 0 );

	// Signal end of data
	sTemp = _T("\r\n.\r\n" );
	send( m_sSMTPServer, MB(sTemp), (int)sTemp.size(), 0 );
	if( !get_response( GENERIC_SUCCESS ) )
	{
		return false;
	}
	return true;
}

bool CSMTP::get_response( eResponse const response_expected )
{
	assert( response_expected >= GENERIC_SUCCESS );
	assert( response_expected < LAST_RESPONSE );

	std::wstring sResponse;
	UINT response;
	response_code const* pResp = NULL;	// Shorthand

	int const iRecvBytes = ::recv(m_sSMTPServer, m_kResponseBuf, sizeof(m_kResponseBuf), 0);
	if( -1 == iRecvBytes 
	|| sizeof(m_kResponseBuf) <= iRecvBytes )
	{
		m_sError = _T("Socket Error" );
		return false;
	}
	m_kResponseBuf[iRecvBytes] = _T('\0');

	sResponse = UNI(m_kResponseBuf);

	std::wstring sTmp;
	for( int i=0; i<3; i++ )
	{
		sTmp += sResponse[i];
	}
	_stscanf_s( sTmp.c_str(), _T("%d" ), &response );
	pResp = &response_table[ response_expected ];
	if( response != pResp->nResponse )
	{
		TCHAR Buf[255] = {0,};
		_itot_s(response, Buf, 255, 10);
		m_sError = Buf;

		m_sError += _T(':');
		m_sError += pResp->sMessage;
		return false;
	}
	return true;
}
