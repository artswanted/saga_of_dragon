#include "stdafx.h"
#include "PgGmChatMgr.h"


#define FILENAME	L".txt"
#define FOLDERNAME	L"./chatLog"
#define LOCALE_KOR	std::locale("Korean_Korea.949")


PgGmChatMgr::PgGmChatMgr(void)
{
	Clear();
}

PgGmChatMgr::~PgGmChatMgr(void)
{
	Clear();
}

void PgGmChatMgr::Clear()
{
	if( m_ChatLog.is_open() )
		m_ChatLog.close();
	m_bFileOpen = false;
}
/*
void PgGmChatMgr::SetLocale(std::locale &locale)
{
	ms_chat_log_locale = locale;
}
*/
void PgGmChatMgr::SetWriteFile(const std::wstring &GmName)
{
	if(m_ChatLog.is_open() == false )
	{
		BM::PgPackedTime kGmServerTime;
		kGmServerTime.SetLocalTime();
		std::wstring wstrFolder = FOLDERNAME;
		::_wmkdir(wstrFolder.c_str());
		wchar_t wstrFileName[1024] = {0,};
		::wsprintf(wstrFileName, L"%s/%02d_%02d_%02d %02d%02d %s%s",
			wstrFolder.c_str(),
			kGmServerTime.Year(),
			kGmServerTime.Month(),
			kGmServerTime.Day(),
			kGmServerTime.Hour(),
			kGmServerTime.Min(),
			GmName.c_str(),
			FILENAME );

		if( m_ChatLog.is_open() )
		{
			m_ChatLog.close();
		}

		m_ChatLog.open( wstrFileName, std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary );//다시 연다.
		m_ChatLog.imbue(LOCALE_KOR);

		if( m_ChatLog.is_open()  == false )
		{
			assert(NULL && "SetWriteFile() Error" );//열리지 않았으면 assert
		}

		m_bFileOpen = true;
	}

}

void PgGmChatMgr::WriteChatMsg(const std::wstring &wstrMsg)
{
	if( m_ChatLog.is_open() )
	{
		BM::PgPackedTime kGmServerTime;
		kGmServerTime.SetLocalTime();
		wchar_t szMsg[1024] = {0,};
		::wsprintf(szMsg, L"%02d_%02d_%02d %02d:%02d  %s",
			kGmServerTime.Year(),
			kGmServerTime.Month(),
			kGmServerTime.Day(),
			kGmServerTime.Hour(),
			kGmServerTime.Min(),
			wstrMsg.c_str() );

		m_ChatLog.write( szMsg, (const std::streamsize)strlenT<WCHAR>(szMsg) );
		m_ChatLog.write(L"\r\n", 2);
		m_ChatLog.flush();
	}
}