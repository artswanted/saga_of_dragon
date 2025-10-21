//────────────────────────────────────────────
//	VerCheck Class Function.
//────────────────────────────────────────────
#include "FTPMgr.h"

//────────────────────────────────────────────
//	생성자와 소멸자
//────────────────────────────────────────────
CFTPMgr::CFTPMgr(void)
{
	//	초기화
	m_hSession	= NULL;
	m_hHttp		= NULL;
	m_hFile		= NULL;
	m_bIsState	= false;
	m_uiBufferSize = DEFAULT_BUFFER_SIZE;
}

CFTPMgr::~CFTPMgr(void)
{
	//	해방
	Clear();
}

//────────────────────────────────────────────
//	클래스 함수
//────────────────────────────────────────────
//	서버 접속 설정
bool CFTPMgr::SessionSetting(void)
{
	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] Session Setting\n\r"), _T(__FUNCTION__), __LINE__);
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CONECTINGSERVER));

	//	세션 핸들 얻기
	m_hSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(m_hSession == INVALID_HANDLE_VALUE)
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] InternetOpen failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw GetLastError();
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] Session Setting Success\n\r"), _T(__FUNCTION__), __LINE__);
	return	true;
}

//	서버 접속 시도
bool CFTPMgr::ServerConnect(const TCHAR* szAddr, const WORD& wPort, const TCHAR* szID, 
							const TCHAR* szPW)
{
	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] Addr: %s Port:%d ID:%s PW:%s"), _T(__FUNCTION__), __LINE__, szAddr, wPort, szID, szPW);

	//	연결전 확인
	if(InternetAttemptConnect(NULL) != ERROR_SUCCESS)
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] InternetAttemptConnect failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError(); 		
	}

	//	서버 연결
	m_hHttp = InternetConnect(m_hSession, szAddr, ((wPort)?(wPort):(INTERNET_DEFAULT_HTTP_PORT)), szID, szPW, INTERNET_SERVICE_HTTP, 0, 0);
	if(m_hHttp == INVALID_HANDLE_VALUE)
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] InternetConnect failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] HTTP 서버 접속 OK 다음단계로."), _T(__FUNCTION__), __LINE__);
	return	true;
}

//	파일 생성
bool CFTPMgr::CreateDownFile(HANDLE& hFile, const std::wstring& strWriteName)
{
	//	클라이언트에 파일을 생성
	hFile = CreateFile(strWriteName.c_str(), GENERIC_WRITE, 0, 0, 
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(!hFile)
	{
		//	메시지
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] CreateFile failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		return	false;
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 파일 생성 OK 다음단계로."), _T(__FUNCTION__), __LINE__);
	return	true;
}

//	파일 연결 요청
bool CFTPMgr::FileConnect(const std::wstring& wstrFileName)
{
	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 파일 싱크 요청 : %d 회."), _T(__FUNCTION__), __LINE__);

	m_hFile = HttpOpenRequest(m_hHttp, L"GET", wstrFileName.c_str(), HTTP_VERSION, NULL, NULL, 
		INTERNET_FLAG_NO_CACHE_WRITE, 0);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] HttpOpenRequest failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	//	여기에 404를 구분해야 하는데.

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 파일 싱크 요청 OK 다음단계로."), _T(__FUNCTION__), __LINE__);	
	return	true;
}

//	파일 정보
bool CFTPMgr::FileSendRequest() const
{
	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 싱크 결과 요청"), _T(__FUNCTION__), __LINE__);

	if(!HttpSendRequest(m_hFile, NULL, 0, NULL, 0))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] HttpSendRequest failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 요청 성공 다음단계로."), _T(__FUNCTION__), __LINE__);
	return	true;
}

//	요청 결과
bool CFTPMgr::FileResult() const
{
	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 싱크 결과"), _T(__FUNCTION__), __LINE__);

	TCHAR	szTemp[MAX_PATH] = {0, };
	DWORD	dwBufLen = sizeof(szTemp);

	if(!HttpQueryInfo(m_hFile, HTTP_QUERY_STATUS_CODE, (LPVOID)&szTemp, &dwBufLen, 0))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] HttpQueryInfo failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	//	연결 상태 체크
	const DWORD nResult = _wtoi(szTemp);
	if(nResult != HTTP_STATUS_OK)
	{
		DWORD const dwFileNotFount = 404;
		if( dwFileNotFount == nResult )
		{
			INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] FileNotFound\n\r"), __FUNCTIONW__, __LINE__);
			return false;
		}
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] FileResult %d\n\r"), __FUNCTIONW__, __LINE__, nResult);
		throw nResult;
	}

	return	true;
}

bool CFTPMgr::GetSize(DWORD& dwSize) const
{
	TCHAR	szTemp[MAX_PATH] = {0, };
	DWORD	dwBufLen = sizeof(szTemp);

	if(HttpQueryInfo(m_hFile, HTTP_QUERY_CONTENT_LENGTH,
		(LPVOID)&szTemp, &dwBufLen, 0))
	{
		dwSize = _wtol(szTemp);
	}
	else
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] HttpQueryInfo failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	return	true;
}

//	파일 크기 요청
bool CFTPMgr::DataAvailable(DWORD& dwSize) const
{
	if(!InternetQueryDataAvailable(m_hFile, &dwSize, 0, 0))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] InternetQueryDataAvailable failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 파일 크기 [%ld] OK 다음 단계로."), _T(__FUNCTION__), __LINE__, m_ReceivedFileSize);
	return	true;
}

bool CFTPMgr::DownLoadData(BYTE* szBuffer, const int& nPacket, DWORD& dwReadSize) const
{
	if(!InternetReadFile(m_hFile, szBuffer, nPacket, &dwReadSize))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] InternetReadFile failed(%d)\n\r"), __FUNCTIONW__, __LINE__, GetLastError());
		throw	GetLastError();
		return	false;
	}

	//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 받은 파일 크기 [%d] OK 다음 단계로."), _T(__FUNCTION__), __LINE__, dwReadSize);
	return	true;
}

//	에러번호를 메시지로
void CFTPMgr::ErrorMsg(const DWORD& Error)
{
	TCHAR	szMessage[MAX_PATH] = {0,};
	DWORD	dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
								  GetModuleHandle(L"wininet.dll"), Error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								  reinterpret_cast<LPTSTR>(&szMessage), 0, NULL);

	INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] ErrorMsg(%s)\n\r"), __FUNCTIONW__, __LINE__, szMessage);
	g_PProcess.SetWorkMsg(szMessage);
}

//────────────────────────────────────────────
//	사용자 함수
//────────────────────────────────────────────
//	서버에 연결한다
bool CFTPMgr::Connect(const TCHAR* szAddr, const WORD wPort, const TCHAR* szID, 
					  const TCHAR* szPW)
{
	int	nConnectCnt = 0;

	while(true)
	{
		try
		{
			if(nConnectCnt != 0){ Sleep(500); }

			SessionSetting();
			ServerConnect(szAddr, wPort, szID, szPW);
			m_bIsState	= true;
			break;
		}
		catch(DWORD dwError)
		{
			Clear();

			if(nConnectCnt == RE_CONNECT)
			{
				INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] Connect failed(%d)\n\r"), __FUNCTIONW__, __LINE__, nConnectCnt);
				ErrorMsg(dwError);
				return	false;
			}
		}
	}

	return	true;
}

//	파일 사이즈를 얻는다
bool CFTPMgr::GetFileSize(size_t& Size,const std::wstring& wstrLocation, 
						  const bool bIsASCII, const bool bIsFailIfExit)
{
	int	nConnectCnt = 0;
	std::wstring kLocation = g_PProcess.GetSubAddress();
	std::wstring wstrFileName = wstrLocation;
	if(!kLocation.empty())
	{// sub address를 사용하고
		std::wstring::size_type const index = wstrFileName.find(DIR_MARK);
		if(index != std::wstring::npos)
		{// .\\ 마크가 존재하면 마크를 지움
			wstrFileName.replace(index, DIR_MARK.length(), L"");
		}
	}
	kLocation+=wstrFileName;
	
	while(true)
	{
		try
		{
			if(nConnectCnt != 0){ Sleep(500); }

			DWORD	dwSize	= 0;

			FileConnect(kLocation);
			FileSendRequest();
			FileResult();
			GetSize(dwSize);

			Size	= size_t(dwSize);
			InternetCloseHandle(m_hFile);

			break;
		}
		catch(DWORD dwError)
		{
			++nConnectCnt;

			Clear();
			if((nConnectCnt == RE_CONNECT) || 
				!Connect(g_PProcess.GetAddr().c_str(), g_PProcess.GetPort(), g_PProcess.GetID().c_str(), g_PProcess.GetPW().c_str()))
			{
				INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] GetFileSize failed(%d)\n\r"), __FUNCTIONW__, __LINE__, nConnectCnt);
				ErrorMsg(dwError);
				return	false;				
			}
		}
	}	

	return	true;
}

//	파일을 얻는다
bool CFTPMgr::GetFile(const std::wstring& wstrLocation, const std::wstring& wstrWriteTarget, const bool bIsASCII, const bool bIsFailIfExit)
{
	//INFO_LOG(BM::LOG_LV4, _T("[%s] - [%d] GetFile From Server - %s\n\r"), __FUNCTIONW__, __LINE__, wstrLocation.c_str());

	int	nConnectCnt = 0;	
	std::wstring kLocation = g_PProcess.GetSubAddress();
	std::wstring wstrFileName = wstrLocation;
	if(!kLocation.empty())
	{// 서버의 하위 폴더(sub address)를 사용하고
		std::wstring::size_type const index = wstrFileName.find(DIR_MARK);
		if(index != std::wstring::npos)
		{// .\\ 마크가 존재하면 마크를 지움
			wstrFileName.replace(index, DIR_MARK.length(), L"");
		}
	}
	kLocation+=wstrFileName;

	while(true)
	{
		HANDLE	hFile			= NULL;

		try
		{
			if(nConnectCnt != 0){ Sleep(200); }

			//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] %s 파일 받기 준비."), _T(__FUNCTION__), __LINE__, kLocation.c_str());

			CreateDownFile(hFile, wstrWriteTarget);
			FileConnect(kLocation); 
			FileSendRequest();
			if( !FileResult() )
			{
				CloseHandle(hFile);
				DeleteFile(wstrWriteTarget.c_str());
				return false;
			}

			DWORD FileSize = 0;
			GetSize(FileSize);
			g_PProcess.SetFileSize((size_t)FileSize);

			m_wstrReceivedFileName	= kLocation;
			m_ReceivedFileSize	= 0;
			DWORD	dwSize		= 0;
			DWORD	dwRead		= 0;
			DWORD	dwReadSize	= 0;
			BYTE*	szBuffer = new BYTE[m_uiBufferSize];
			if (szBuffer == NULL)
			{// 메모리 더이상 할당 할수 없을때의 처리
				break;
			}

			memset(szBuffer, 0, m_uiBufferSize * sizeof(BYTE));

			//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] 파일 받기 시작 (크기 : %ld)"), _T(__FUNCTION__), __LINE__, (size_t)dwSize);
			
			while(true)
			{
				if( g_PProcess.CheckEnd() )
				{
					CloseHandle(hFile);
					DeleteFile(wstrWriteTarget.c_str());
					if (szBuffer)
					{
						delete[] szBuffer;
						szBuffer=NULL;
					}
					throw eTHROW_CANCEL;
				}

				DataAvailable(dwSize);
				const int nPacket = (int)((dwSize > m_uiBufferSize) ? m_uiBufferSize : dwSize);
				DownLoadData(szBuffer, nPacket, dwReadSize);

				m_ReceivedFileSize += dwReadSize;

				if(dwReadSize == 0)
				{
					if (szBuffer)
					{
						delete[] szBuffer;
						szBuffer=NULL;
					}
					break;
				}

				WriteFile(hFile, szBuffer, dwReadSize, &dwRead, NULL);
				g_PProcess.SetFileCnt(dwReadSize);			
			}
			if (szBuffer)
			{
				delete[] szBuffer;
				szBuffer=NULL;
			}
			g_PProcess.SetTotalCnt(FileSize);
		
			m_wstrReceivedFileName.clear();
			//INFO_LOG( BM::LOG_LV7, _T("[%s]-[%d] %s 파일 받기 끝."), _T(__FUNCTION__), __LINE__, kLocation.c_str());
			CloseHandle(hFile);

			break;
		}
		catch(DWORD dwError)
		{
			++nConnectCnt;

			CloseHandle(hFile);
			DeleteFile(wstrWriteTarget.c_str());
			Clear();

			if( nConnectCnt == RE_CONNECT
			||	!Connect(g_PProcess.GetAddr().c_str(), g_PProcess.GetPort(), g_PProcess.GetID().c_str(), g_PProcess.GetPW().c_str()))
			{
				INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] Connect failed(%d)\n\r"), __FUNCTIONW__, __LINE__, nConnectCnt);
				ErrorMsg(dwError);
				return	false;
			}
		}
	}
	return	true;
}

//	연결 정보 삭제
void CFTPMgr::Clear()
{
	m_bIsState = false;
	if(m_hFile){ InternetCloseHandle(m_hFile); }
	if(m_hHttp){ InternetCloseHandle(m_hHttp); }
	if(m_hSession){ InternetCloseHandle(m_hSession); }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class CDummyFTPMgr

#ifdef NO_FTP_LOCAL_TEST
namespace PgRecoveryTest
{
	LogFile::LogFile(wchar_t const* szFile, wchar_t const* szMode)
		: pFile(0)
	{
		_wfopen_s(&pFile, szFile, szMode);
	}
	LogFile::~LogFile()
	{
		::fclose(pFile);
	}
	LogFile::operator FILE* () const
	{
		return pFile;
	}

	wchar_t const* pLogFile = L"Test_Log.txt";
	wchar_t const* pLogFile2 = L"Test_Log2.txt";
	wchar_t const* pLogFileMode = L"a";

	void WriteToFile(FILE* pFile, BM::FolderHash const& rkList, wchar_t const* szHeader)
	{
		if( !pFile )
		{
			return;
		}
		if( szHeader )	{ fwprintf_s(pFile, L"======================= BM::FolderHash Start %s =====================\n", szHeader); }

		{ //
			BM::FolderHash::const_iterator iter = rkList.begin();
			while( rkList.end() != iter )
			{
				BM::FolderHash::key_type const& rkKey = (*iter).first;
				BM::FolderHash::mapped_type const& rkValue = (*iter).second;

				if( false == rkValue->kFileHash.empty() )
				{
					BM::FileHash::const_iterator file_iter = rkValue->kFileHash.begin();
					while( rkValue->kFileHash.end() != file_iter )
					{
						fwprintf_s(pFile, L"\t- %s%s\n", rkKey.c_str(), (*file_iter).first.c_str());
						++file_iter;
					}
				}
				else
				{
					fwprintf_s(pFile, L"\t- %s\n", rkKey.c_str());
				}
				++iter;
			}
		}

		if( szHeader )	{ fwprintf_s(pFile, L"------------------------ End %s ------------------------\n", szHeader); }
	}
	void WriteToFile(FILE* pFile, PATCH_HASH const& rkList, wchar_t const* szHeader)
	{
		if( !pFile )
		{
			return;
		}
		if( szHeader )	{ fwprintf_s(pFile, L"======================= PATCH_HASH Start %s =====================\n", szHeader); }

		{ //
			PATCH_HASH::const_iterator iter = rkList.begin();
			while( rkList.end() != iter )
			{
				PATCH_HASH::key_type const& rkKey = (*iter).first;
				PATCH_HASH::mapped_type const& rkValue = (*iter).second;
				
				wchar_t szTemp[MAX_PATH] = {0, };
				BM::VersionInfo kVerInfo;
				kVerInfo.iVersion = rkKey;
				swprintf_s(szTemp, L"\t Key %d-%d-%d", static_cast< int >(kVerInfo.Version.i16Major), static_cast< int >(kVerInfo.Version.i16Minor), static_cast< int >(kVerInfo.Version.i32Tiny));
				WriteToFile(pFile, rkValue, szTemp);
				++iter;
			}
		}

		if( szHeader )	{ fwprintf_s(pFile, L"------------------------ End %s ------------------------\n", szHeader); }
	}
	void WriteToDown(FILE* pFile, wchar_t const* szFile, wchar_t const* szPath, wchar_t const* szTemp, size_t const iSize)
	{
		if( !pFile )
		{
			return;
		}
		fwprintf_s(pFile, L"\t Web Download + %s, From: %s --> To: %s (%u bytes)\n", szFile, szPath, szTemp, iSize);
	}
};

//
bool	CDummyFTPMgr::Connect(const TCHAR* szAddr, const WORD wPort, const TCHAR* szID, 
				const TCHAR* szPW)
{
	return true;
}
bool	CDummyFTPMgr::GetFileSize(size_t& Size,const std::wstring& wstrLocation, 
					const bool bIsASCII, const bool bIsFailIfExit)
{
	return true;
}
bool	CDummyFTPMgr::GetFile(const std::wstring& wstrLocation, const std::wstring& wstrWriteTarget, const bool bIsASCII, const bool bIsFailIfExit)
{
	TCHAR szPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szPath);

	m_iRecvFileSize = 0;
	m_kRecvFileName = std::wstring(L"Test\\") + wstrLocation;
	FILE *pSrcFile = NULL, *pTrgFile = NULL;
	::_wfopen_s(&pSrcFile, m_kRecvFileName.c_str(), L"rb");
	::_wfopen_s(&pTrgFile, wstrWriteTarget.c_str(), L"wb");

	while( 0 == ::feof(pSrcFile) )
	{
		char szBuff[MAX_PATH] = {0, };
		size_t const iRet = ::fread(szBuff, sizeof(char), MAX_PATH, pSrcFile);
		m_iRecvFileSize += iRet;
		::fwrite(szBuff, sizeof(char), iRet, pTrgFile);
	}
	::fclose(pSrcFile);
	::fclose(pTrgFile);
	PgRecoveryTest::WriteToDown(PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), wstrLocation.c_str(), m_kRecvFileName.c_str(), wstrWriteTarget.c_str(), m_iRecvFileSize);
	return true;
}
//	연결 해제
void	CDummyFTPMgr::Clear()
{
}

CDummyFTPMgr::CDummyFTPMgr()
	: m_bIsState(false), m_uiBufferSize(0), m_kRecvFileName(), m_iRecvFileSize(0)
{
}
CDummyFTPMgr::~CDummyFTPMgr()
{
}
#endif NO_FTP_LOCAL_TEST