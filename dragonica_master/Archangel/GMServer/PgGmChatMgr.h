#ifndef GM_GMSERVER_GM_PGGMCHATMGR_H
#define GM_GMSERVER_GM_PGGMCHATMGR_H

//static std::locale ms_chat_log_locale;

class PgGmChatMgr
{
private:
	std::wfstream	m_ChatLog;
	bool			m_bFileOpen;
public:
	void	Clear();
//	void	SetLocale(std::locale &locale);
	bool	IsFileOpen(){return m_bFileOpen;};
	void	SetWriteFile(const std::wstring &GmName);
	void	WriteChatMsg(const std::wstring &wstrMsg);
	PgGmChatMgr(void);
	~PgGmChatMgr(void);
};

#define g_kGmChatMgr SINGLETON_STATIC(PgGmChatMgr)

#endif // GM_GMSERVER_GM_PGGMCHATMGR_H