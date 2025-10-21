#pragma once

#include	"defines.h"

const int	RE_CONNECT	= 7;
const int	DEFAULT_BUFFER_SIZE = 64 * 1024;

class CFTPMgr
{
//─────────────────────────────────────
//	CFTPMgr Code - 함수부
//─────────────────────────────────────
public:
	//	외부 사용 함수
	bool	Connect(const TCHAR* szAddr, const WORD wPort, const TCHAR* szID, 
					const TCHAR* szPW);
	bool	GetFileSize(size_t& Size,const std::wstring& wstrLocation, 
						const bool bIsASCII = false, const bool bIsFailIfExit = false);
	bool	GetFile(const std::wstring& wstrLocation, const std::wstring& wstrWriteTarget, const bool bIsASCII = false, const bool bIsFailIfExit = false);
	//	연결 해제
	void			Clear();

	//	얻자 무언가.
	size_t			GetReceiveFileSize() const		{ return m_ReceivedFileSize; }
	std::wstring	GetReceiveFileName() const		{ return m_wstrReceivedFileName; }
	void			ConnectionState(bool bState)	{ m_bIsState = bState; }
	void			SetBufferSize(unsigned int uiSize) { m_uiBufferSize = uiSize; }

	CFTPMgr();
	virtual ~CFTPMgr();

private:
	//	서버 접속 관련
	bool	SessionSetting(void);
	bool	ServerConnect(const TCHAR* szAddr, const WORD& wPort, const TCHAR* szID, 
						  const TCHAR* szPW);

	//	파일 접속 관련
	bool	CreateDownFile(HANDLE& hFile, const std::wstring& strWriteName);
	bool	FileConnect(const std::wstring& wstrFileName);
	bool	FileSendRequest() const;
	bool	FileResult() const;
	bool	GetSize(DWORD& dwSize) const;
	bool	DataAvailable(DWORD& dwSize) const;
	bool	DownLoadData(BYTE* szBuffer, const int& nPacket, DWORD& dwReadSize) const;

	//	오류 메시지
	void	ErrorMsg(const DWORD& Error);

//─────────────────────────────────────
//	CFTPMgr Code - 변수부
//─────────────────────────────────────
private:
	HINTERNET		m_hSession;
	HINTERNET		m_hHttp;
	HINTERNET		m_hFile;

	size_t			m_ReceivedFileSize;
	std::wstring	m_wstrReceivedFileName;
	bool			m_bIsState;
	unsigned int	m_uiBufferSize;

};


///
#ifdef NO_FTP_LOCAL_TEST
namespace PgRecoveryTest
{
	struct LogFile
	{
		LogFile(wchar_t const* szFile, wchar_t const* szMode);
		~LogFile();
		operator FILE* () const;
		FILE* pFile;
	};

	extern wchar_t const* pLogFile;
	extern wchar_t const* pLogFile2;
	extern wchar_t const* pLogFileMode;

	//void WriteToFile(FILE* pFile, PATCH_HASH const& rkList, wchar_t const* szHeader = NULL);
	//void WriteToFile(FILE* pFile, BM::FolderHash const& rkList, wchar_t const* szHeader = NULL);
};

class CDummyFTPMgr
{
public:
	//	외부 사용 함수
	bool	Connect(const TCHAR* szAddr, const WORD wPort, const TCHAR* szID, 
					const TCHAR* szPW);
	bool	GetFileSize(size_t& Size,const std::wstring& wstrLocation, 
						const bool bIsASCII = false, const bool bIsFailIfExit = false);
	bool	GetFile(const std::wstring& wstrLocation, const std::wstring& wstrWriteTarget, const bool bIsASCII = false, const bool bIsFailIfExit = false);
	//	연결 해제
	void	Clear();

	//	얻자 무언가.
	//size_t			GetReceiveFileSize() const		{ return m_iRecvFileSize; }
	//std::wstring	GetReceiveFileName() const		{ return m_kRecvFileName; }
	void			ConnectionState(bool bState)	{ m_bIsState = bState; }
	void			SetBufferSize(unsigned int uiSize) { m_uiBufferSize = uiSize; }

	CDummyFTPMgr();
	virtual ~CDummyFTPMgr();

private:
	bool m_bIsState;
	unsigned int m_uiBufferSize;
	std::wstring m_kRecvFileName;
	size_t m_iRecvFileSize;
};
#endif NO_FTP_LOCAL_TEST