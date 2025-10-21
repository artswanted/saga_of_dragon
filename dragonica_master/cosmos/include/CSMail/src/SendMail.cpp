#include "stdafx.h"
#include "../Header/smtp.h"
#include "../Header/MIMEMessage.h"
#include "Sendmail.h"
#include "../Header/NameDef.h"
#pragma warning(disable : 4996) // Disable: 'inet_addr': Use inet_pton() or InetPton() instead or define

using namespace CS_MAIL;

static BM::vstring const SECTION_COMMON(_T("COMMON"));

static BM::vstring const ELEMENT_RECV_ADDR_COUNT(_T("RECV_ADDR_COUNT"));
static BM::vstring const ELEMENT_SMTP_ADDR(_T("SMTP_ADDR"));
static BM::vstring const ELEMENT_SENDER_MAIL_ADDR(_T("SENDER_MAIL_ADDR"));

static BM::vstring const SECTION_RECV_ADDR_INFO(_T("RECV_ADDR_INFO"));
static BM::vstring const ELEMENT_MAIL_ADDR_(_T("MAIL_ADDR_"));

static BM::vstring const ELEMENT_ID(_T("ID"));
static BM::vstring const ELEMENT_PW( _T("PW"));

CSendMail::CSendMail()
{
}

CSendMail::~CSendMail()
{
	WSACleanup();   
}

bool CSendMail::Init(std::wstring const& strFileName, bool const bIsINI)
{
	WSADATA wsaData ={0,};
	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{	//	error
		return false;
	}

	if( bIsINI )
	{
		return LoadINI( strFileName );
	}
	else
	{
//			return LoadINB( strFileName );
	}
	return false;
}

bool CSendMail::LoadINI(std::wstring const& strFileName)
{
	TCHAR szSMTPAddr[MAX_PATH]={0,};
	TCHAR szSenderMailAddr[MAX_PATH] = {0,};
	TCHAR szSMTPID[MAX_PATH]={0,};
	TCHAR szSMTPPW[MAX_PATH]={0,};

	::GetPrivateProfileString( SECTION_COMMON, ELEMENT_SMTP_ADDR, _T(""), szSMTPAddr, sizeof(szSMTPAddr), strFileName.c_str());
	::GetPrivateProfileString( SECTION_COMMON, ELEMENT_SENDER_MAIL_ADDR, _T(""), szSenderMailAddr, sizeof(szSenderMailAddr), strFileName.c_str());

	::GetPrivateProfileString( SECTION_COMMON, ELEMENT_ID, _T(""), szSMTPID, sizeof(szSMTPID), strFileName.c_str());
	::GetPrivateProfileString( SECTION_COMMON, ELEMENT_PW, _T(""), szSMTPPW, sizeof(szSMTPPW), strFileName.c_str());

	m_strID = szSMTPID;
	m_strPW = szSMTPPW;

	m_strServerHostName = szSMTPAddr;
	m_strSenderMailAddr = szSenderMailAddr;

	if(	!m_strID.size()
	||	!m_strPW.size()
	||	!m_strServerHostName.size() 
	||	!m_strSenderMailAddr.size() )
	{
		return false;
	}

	int const iCount = ::GetPrivateProfileInt(SECTION_COMMON, ELEMENT_RECV_ADDR_COUNT, 0, strFileName.c_str());	

	if( !iCount )
	{
		return false;
	}

	int i = 0;
	while(iCount > i)
	{
		++i;
		TCHAR szRecvMailAddr[MAX_PATH] = {0,};
		::GetPrivateProfileString( SECTION_RECV_ADDR_INFO , ELEMENT_MAIL_ADDR_+i, _T(""), szRecvMailAddr, sizeof(szRecvMailAddr), strFileName.c_str());
		std::wstring const strMailAddr = szRecvMailAddr;
		m_TargetList.Insert( strMailAddr );
	}

	sockaddr_in dest = {0,};
	const hostent *hp = gethostbyname( MB(m_strServerHostName) );
	if (hp)
	{
		::memcpy( &(dest.sin_addr), hp->h_addr, hp->h_length);
		m_strServerInetAddr = UNI(inet_ntoa(dest.sin_addr));
		return true;	
	}
	
	return false;
}

bool CSendMail::SendAction( void *pData )
{
	bool bRet = false;
	if(pData)
	{
		CMIMEMessage *pMsg = (CMIMEMessage*)pData;
		CSMTP smtp( m_strID, m_strPW, m_strServerInetAddr.c_str() );
		if( smtp.Connect() )
		{
			if( smtp.SendMessage( pMsg ) )
			{
				bRet = true;
			}
			smtp.Disconnect();
			return bRet;
		}
	}
	return false;
}

bool CSendMail::Send(std::wstring const& strSender, std::wstring const& strTarget, std::wstring const& strTitle, std::wstring const& strBody, TARGET_LIST const& TL_AttachList)
{
	CMIMEMessage		msg;

	msg.m_strTitle		= strTitle;
	msg.m_strBody		= strBody;
	msg.m_strSender		= strSender;

	msg.AddMultipleRecipients( strTarget );
/*
	if( m_TargetList.Size() )
	{
		msg.AddMultipleRecipients( m_cc.c_str(), CMailMessage::CC );
	}

	if ( !m_bcc.empty() )
	{
		msg.AddMultipleRecipients( m_bcc.c_str(), CMailMessage::BCC );
	}
*/
	msg.AddMIMEPart( TL_AttachList );
	bool const bRet = SendAction( &msg );
	return bRet;
}

bool CSendMail::SendAuto(std::wstring const& strTitle, std::wstring const& strBody, TARGET_LIST const& TL_AttachList)
{
	CMIMEMessage		msg;

	msg.m_strTitle		= strTitle;
	msg.m_strBody		= strBody;
	msg.m_strSender		= m_strSenderMailAddr;

	msg.AddMultipleRecipients( m_TargetList );

//	for( int i=0; i<(int)m_vsToList.size(); i++ )
//	{
//		msg.AddMultipleRecipients( m_vsToList[i].c_str() );
//	}
//
//	if ( !m_cc.empty() )
//	{
//		msg.AddMultipleRecipients( m_cc.c_str(), CMailMessage::CC );
//	}

//	if ( !m_bcc.empty() )
//	{	
//		msg.AddMultipleRecipients( m_bcc.c_str(), CMailMessage::BCC );
//	}

	msg.AddMIMEPart( TL_AttachList );

	bool const bRet = SendAction( &msg );
	return bRet;
}
