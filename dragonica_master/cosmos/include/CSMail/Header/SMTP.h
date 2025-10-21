// SMTP.h: interface for the CSMTP class.
// Copyright (c) 1998, Wes Clyburn
//////////////////////////////////////////////////////////////////////
#pragma once
#define WIN32_LEAN_AND_MEAN
#include "MailMessage.h"
#include "winsock2.h"

short const DEFAULT_SMTP_PORT = 25;		// Standard port for SMTP servers
size_t const RESPONSE_BUFFER_SIZE = 1024;

enum eResponse
{
	GENERIC_SUCCESS = 0,
	CONNECT_SUCCESS,
	DATA_SUCCESS,
	QUIT_SUCCESS,
	TRY_AUTH_ID_SUCCESS,
	TRY_AUTH_PW_SUCCESS,
	// Include any others here
	LAST_RESPONSE	// Do not add entries past this one
};

typedef struct response_code
{
	UINT nResponse;		// Response we're looking for
	TCHAR* sMessage;	// Error message if we don't get it
}RESPONSE_CODE;

class CSMTP  
{
public:
	CSMTP(std::wstring const& strID, std::wstring const& strPW, LPCTSTR szSMTPServerName, short const = DEFAULT_SMTP_PORT);
	virtual ~CSMTP();

public:
	void SetServerProperties( LPCTSTR szSMTPServerName, short const nPort = DEFAULT_SMTP_PORT );
	std::wstring GetLastError();

	bool Connect();
	bool Disconnect();
	
	virtual bool FormatMailMessage( CMailMessage* msg );
	bool SendMessage( CMailMessage * pMsg);

protected:
	virtual bool transmit_message( CMailMessage* msg );

private:
	bool get_response( eResponse const response_expected );
	std::wstring cook_body( CMailMessage* msg );

protected:
	std::wstring m_strID;
	std::wstring m_strPW;

	std::wstring m_sSMTPServerHostName;
	short m_nPort;

	std::wstring m_sError;
	bool m_bConnected;
	
	
	SOCKET m_sSMTPServer;
	char m_kResponseBuf[RESPONSE_BUFFER_SIZE];//패킷 버퍼.
};

