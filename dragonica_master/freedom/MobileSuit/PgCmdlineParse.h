#ifndef FREEDOM_DRAGONICA_LOCAL_PGCMDLINEPARSE_H
#define FREEDOM_DRAGONICA_LOCAL_PGCMDLINEPARSE_H
#include "BM/Stream.h"
#include <unordered_map>

class PgCmdlineParse
{
public:
	PgCmdlineParse(void);
	~PgCmdlineParse(void);
public:
	bool Init(std::wstring const &strCmd);

	bool IsUseForceAccount()const{return m_bIsUseForceAccount;}
	bool IsLoginFuncOff()const{return m_bIsLoginFuncOff;}
	void SetLoginFuncOff(bool const bIs){m_bIsLoginFuncOff = bIs;}
	std::wstring ServerAddr()const;
	void MakeLoginPacket(BM::Stream &kOutPacket)const;
	inline bool has(const char* name) { return m_kCmdLine.find(std::string(name)) != m_kCmdLine.end(); }

	CLASS_DECLARATION_S(std::wstring, ServerIP);
	CLASS_DECLARATION_S(int, ServerPort);
	CLASS_DECLARATION_S(int, FullMode);
	CLASS_DECLARATION_S(int, GraphicMode);
	CLASS_DECLARATION_S(bool, LaunchByLauncher);
	CLASS_DECLARATION_S(std::wstring, SiteName);
	CLASS_DECLARATION_S(int, SiteIndex);

	CLASS_DECLARATION_S(std::wstring, ID);
	CLASS_DECLARATION_S(std::wstring, PW);

	bool m_bIsUseForceAccount;//占쏙옙占쏙옙占쏙옙 
	bool m_bIsLoginFuncOff;

private:
	typedef std::unordered_map<std::string /*Cmd line key*/ , BM::vstring /*cmd line data*/ > tagCmdLine;
	tagCmdLine m_kCmdLine;
};

#define g_kCmdLineParse SINGLETON_STATIC(PgCmdlineParse)

#endif // FREEDOM_DRAGONICA_LOCAL_PGCMDLINEPARSE_H