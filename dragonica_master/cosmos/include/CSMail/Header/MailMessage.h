#pragma once
#include <windows.h>
#include <tchar.h>
#include "BM/STLSupport.h"
#include "Sendmail.h"

class CMailMessage  
{
public:
	CMailMessage();
	virtual ~CMailMessage();

	typedef enum eRecipientsType
	{
		TO = 1, 
		CC = 2, 
		BCC= 3 
	}RECIPIENTS_TYPE; 

	void FormatMessage();
	int GetNumRecipients(RECIPIENTS_TYPE type = TO );
	bool GetRecipient( std::wstring& sEmailAddress, std::wstring& sFriendlyName, int nIndex = 0, RECIPIENTS_TYPE type = TO  );
	bool AddRecipient( LPCTSTR szEmailAddress, LPCTSTR szFriendlyName = _T(""), RECIPIENTS_TYPE type = TO  );
	
	bool AddMultipleRecipients( CS_MAIL::TARGET_LIST const& rTL, RECIPIENTS_TYPE type = TO  );
	bool AddMultipleRecipients( std::wstring const& strRecipients, RECIPIENTS_TYPE type = TO  );
	UINT GetCharsPerLine();
	void SetCharsPerLine( UINT nCharsPerLine );

	std::wstring m_strSender;
	std::wstring m_strTitle;
	std::wstring m_strMailerName;
	std::wstring m_strHeader;
	std::wstring m_strBody;
//	CTime m_tDateTime;
private:
	UINT m_nCharsPerLine;
	class CRecipient
	{
		public:
			std::wstring m_sEmailAddress;
			std::wstring m_sFriendlyName;
	};
	std::vector <CRecipient> m_Recipients;
	std::vector <CRecipient> m_CCRecipients;    //*** <JFO>
	std::vector <CRecipient> m_BCCRecipients;   //*** <JFO>

protected:
	// When overriding prepare_header(), call base class 
	// version first, then add specialized 
	// add_header_line calls.
	// This ensures that the base class has a chance to
	// create the header lines it needs.
	virtual void prepare_header();
	virtual void prepare_body();
	virtual void end_header();
	virtual void start_header();

	// This rarely needs overwriting, but is virtual just in case.
	// Do not include the trailing CR/LF in parameter.
	virtual void add_header_line( LPCTSTR szHeaderLine );

	
	std::wstring GetStringFromTime(SYSTEMTIME& time);
};
