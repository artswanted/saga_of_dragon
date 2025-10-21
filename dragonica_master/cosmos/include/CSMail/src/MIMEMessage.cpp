// MIMEMessage.cpp: implementation of the CMIMEMessage class.
// Author: Wes Clyburn (clyburnw@enmu.edu)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Header/MIMEMessage.h"
#include "../Header/TextPlain.h"
#include "../Header/AppOctetStream.h"

#include "../Header/stringUtil.h"

#include <cassert>


// Static Member Initializers
CMIMEMessage::CMIMETypeManager CMIMEMessage::m_MIMETypeManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMIMEMessage::CMIMEMessage()
{
	m_sMIMEContentType = _T("multipart/mixed" );
	m_sPartBoundary = _T("WC_MAIL_PaRt_BoUnDaRy_05151998" );
	m_sNoMIMEText = _T("This is a multi-part message in MIME format." );

	// Register the MIME types handled by this class
	//
	CMIMEContentAgent* pType;
	// These objects are deleted by CMIMTypeManager's destructor
	pType = new CTextPlain( TEXT_PLAIN, GetCharsPerLine() );
	register_mime_type( pType );
	pType = new CAppOctetStream( APPLICATION_OCTETSTREAM );
	register_mime_type( pType );
}

CMIMEMessage::~CMIMEMessage()
{
}

bool CMIMEMessage::AddMIMEPart( CS_MAIL::TARGET_LIST const&rTL )
{
	CS_MAIL::STRING_LIST const& rlst = rTL.lstTarget;

	CS_MAIL::STRING_LIST::const_iterator itor= rlst.begin();
	while( rlst.end() != itor)
	{
        if( !AddMIMEPart( (*itor).c_str() ) )
		{
			return false;
		}
		++itor;
	}
	return true;
}

// This implementation adds the part to the part-list used
//  to build the body.
bool CMIMEMessage::AddMIMEPart(LPCTSTR szContent, int nContentType, LPCTSTR szParameters, int nEncoding, bool bPath )
{
	CMIMEPart part;
	part.m_nContentType = nContentType;
	part.m_sParameters = szParameters;
	part.m_nEncoding = nEncoding;
	part.m_bPath = bPath;
	part.m_sContent = szContent;

	StringUtil	su;
	
	su.TrimLeft(part.m_sContent);
	su.TrimRight(part.m_sContent);
	if( nContentType == TEXT_PLAIN )
	{
		m_MIMEPartList.push_front( part );
	}
	else
	{
		m_MIMEPartList.push_back( part );
	}
	return true;
}

void CMIMEMessage::prepare_header()
{
	std::wstring sTemp;

	// Let the base class add its headers
	CMailMessage::prepare_header();
	
	add_header_line( _T("MIME-Version: 1.0" ) );

	sTemp = _T("Content-Type: ");
	sTemp += m_sMIMEContentType;
	sTemp += _T("; boundary=");
	sTemp += m_sPartBoundary;

	add_header_line( sTemp.c_str() );
}

void CMIMEMessage::prepare_body()
{
	// Class user may have assigned body text directly.
	// Convert it to just another MIME part to be processed.
	// If this default Content-Type isn't good enough for the
	// class user, he or she should have used AddMIMEPart() instead.
	if( m_strBody != _T("" ) )
		AddMIMEPart( m_strBody.c_str(), TEXT_PLAIN, _T(""), _7BIT, FALSE );

	// Initialize the body (replace current contents).
	m_strBody = m_sNoMIMEText;
	m_strBody += _T("\r\n\r\n" );
	append_mime_parts();
	insert_message_end( m_strBody );

	// Let the base class take me to Funky Town
	CMailMessage::prepare_body();
}

void CMIMEMessage::insert_boundary( std::wstring& sText )
{
	std::wstring sTemp;

	StringUtil su;
	
	if( su.Right(sText, 2) != _T("\r\n" ) )
		sText += _T("\r\n" );

	sTemp = _T("--");
	sTemp += m_sPartBoundary;
	sTemp += _T("\r\n");

	sText += sTemp;
}

void CMIMEMessage::insert_message_end( std::wstring& sText )
{
	StringUtil su;

	std::wstring sTemp;
	if( su.Right(sText, 2)  != _T("\r\n" ) )
		sText += _T("\r\n" );
	
	sTemp = _T("--");
	sTemp += m_sPartBoundary;
	sTemp += _T("--\r\n");

	sText += sTemp;
}

void CMIMEMessage::register_mime_type(CMIMEContentAgent* pMIMEType)
{
	assert( pMIMEType != NULL );
	if( pMIMEType == NULL )
		return;
	m_MIMETypeManager.RegisterMIMEType( pMIMEType );
}


void CMIMEMessage::append_mime_parts()
{
	std::list< CMIMEPart >::iterator		part_position;
	CMIMEPart* pMIMEPart = NULL;
	CMIMEContentAgent* pMIMEType = NULL;
	
	part_position = m_MIMEPartList.begin();
	// Get each part from the list, retrieve a handler for it,
	//  and let the handler do its thing.
	while( m_MIMEPartList.end() != part_position )
	{
		pMIMEPart = &(*part_position);
		++part_position;

		pMIMEType = m_MIMETypeManager.GetHandler( pMIMEPart->m_nContentType );
		if( pMIMEType != NULL )
		{
			insert_boundary( m_strBody );
			pMIMEType->AppendPart( pMIMEPart->m_sContent.c_str(),
								   pMIMEPart->m_sParameters.c_str(),
								   pMIMEPart->m_nEncoding,
								   pMIMEPart->m_bPath,
								   m_strBody);
		}
	}

}

//////////////////////////////////////////////////////////////////////
// CMIMETypeManager Implementation
//////////////////////////////////////////////////////////////////////

CMIMEMessage::CMIMETypeManager::CMIMETypeManager()
{
	InitializeCriticalSection (&m_csAccess);
}

CMIMEMessage::CMIMETypeManager::~CMIMETypeManager()
{
	std::list < CMIMEContentAgent* >::iterator	pos;
	CMIMEContentAgent* p;

	EnterCriticalSection(&m_csAccess);

	pos = m_MIMETypeList.begin();

	while( m_MIMETypeList.end() != pos )
	{
		p = (*pos);
		++pos;

		delete p;
	}
	LeaveCriticalSection(&m_csAccess);


	DeleteCriticalSection(&m_csAccess);
}

void CMIMEMessage::CMIMETypeManager::RegisterMIMEType(CMIMEContentAgent *pMIMEType)
{
	assert( pMIMEType != NULL );
	if( pMIMEType == NULL )
		return;

	EnterCriticalSection(&m_csAccess);
	m_MIMETypeList.push_back( pMIMEType );
	LeaveCriticalSection(&m_csAccess);
}



CMIMEContentAgent* CMIMEMessage::CMIMETypeManager::GetHandler(int nContentType)
{
	std::list < CMIMEContentAgent* >::iterator	pos;
	CMIMEContentAgent* pType = NULL;
	
	EnterCriticalSection(&m_csAccess);

	pos = m_MIMETypeList.begin();
	while( m_MIMETypeList.end() != pos )
	{
		pType = (*pos);
		++pos;
	
		if( pType->QueryType( nContentType ) == TRUE )
			break;
	}

	LeaveCriticalSection(&m_csAccess);
	return pType;
}
