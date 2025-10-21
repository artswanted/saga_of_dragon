#pragma once

#include "Sendmail.h"
#include "MailMessage.h"
#include "MIMEContentAgent.h"

class CMIMEMessage 
	:	public CMailMessage
{
public:
	CMIMEMessage();
	virtual ~CMIMEMessage();

	// MIME Type Codes
	typedef enum eMIMETypeCode
	{
		TEXT_PLAIN = 0,
		APPLICATION_OCTETSTREAM,
		NEXT_FREE_MIME_CODE
	}E_MIME_TYPE_CODE;

	typedef enum eMIMEEncodingCode
	{
		_7BIT = 0,
		_8BIT,
		BINARY,
		QUOTED_PRINTABLE,
		BASE64,
		NEXT_FREE_ENCODING_CODE
	}E_MIME_ENCODING_CODE;

	bool AddMIMEPart( CS_MAIL::TARGET_LIST const& rTL );
	bool AddMIMEPart( LPCTSTR szContent, int nContentType = APPLICATION_OCTETSTREAM, LPCTSTR szParameters = _T("" ), int nEncoding = BASE64, bool bPath = TRUE );
protected:
    void insert_message_end( std::wstring& sText );
	void register_mime_type( CMIMEContentAgent* pMIMEType );
	void insert_boundary( std::wstring& sText );

	virtual void append_mime_parts();
	virtual void prepare_header();
	virtual void prepare_body();

	std::wstring m_sNoMIMEText;
	std::wstring m_sPartBoundary;
	std::wstring m_sMIMEContentType;
private:
	class CMIMEPart
	{
	public:
		int m_nEncoding;
		int m_nContentType;
		std::wstring m_sParameters;
		bool m_bPath;
		std::wstring m_sContent;
	};
	std::list <CMIMEPart> m_MIMEPartList;

	class CMIMETypeManager
	{
	public:
		CMIMEContentAgent* GetHandler( int nContentType );
		void RegisterMIMEType( CMIMEContentAgent* pMIMEType);
		virtual  ~CMIMETypeManager();
		CMIMETypeManager();
	private:
		CRITICAL_SECTION	m_csAccess;
		std::list < CMIMEContentAgent* > m_MIMETypeList;
	};

	static CMIMETypeManager m_MIMETypeManager;

};