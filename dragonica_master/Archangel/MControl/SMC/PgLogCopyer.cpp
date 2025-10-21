#include "stdafx.h"
#include "DataPack/PackInfo.h"
#include "DataPack/PgDataPackManager.h"
#include "PgLogCopyer.h"
#include "PgSMCMgr.h"

PgLogCopyer::PgLogCopyer(void)
{

}

PgLogCopyer::~PgLogCopyer(void)
{

}

void PgLogCopyer::GetFile( std::wstring const &kFileName, std::vector< char > &rkFileData, int const iFileType )
{
	BM::CAutoMutex kLock( m_kMutex );

	std::wstring wstrDir = m_wstrDir;
	if(iFileType == FT_DUMP)
	{
		wstrDir = g_kSMCMgr.PathDump();
		BM::ConvFolderMark(wstrDir);
		if ( wstrDir.size() )
		{
			wchar_t const tLast = wstrDir.at(wstrDir.size()-1);
			if( tLast != _T('\\') 
				&&	tLast != _T('/') )
			{
				wstrDir += _T("\\");
			}
		}
		BM::ConvFolderMark(wstrDir);
		
		const wchar_t* lpFolderName = wstrDir.c_str();
		if(!IsFileExist(lpFolderName))
		{
			INFO_LOG( BM::LOG_LV5, _T("[ServerTool] Folder is Empty!!! ... Name:  [ ") << lpFolderName << _T(" ]"));	
			return;
		}
	}
	wstrDir += kFileName;
	BM::FileToMem( wstrDir, rkFileData);

	if(rkFileData.empty())
	{
		INFO_LOG( BM::LOG_LV5, _T("[ServerTool] File is Empty!!! ... Name:  [ ") << wstrDir.c_str() << _T(" ]"));	
	}
}

void PgLogCopyer::DeleteFile( std::wstring const &kFileName ) const
{
	if ( kFileName.size() )
	{
		BM::CAutoMutex kLock( m_kMutex );
		std::wstring wstrDir = m_wstrDir + kFileName;

		if ( TRUE == ::DeleteFileW( wstrDir.c_str() ) )
		{
			INFO_LOG( BM::LOG_LV5, L"[DeleteFile] " << kFileName );
		}
	}
}

bool PgLogCopyer::GetFileList( std::wstring wstrDir, CONT_LOG_FILE_INFO &rkContFile, const wchar_t *lpExtension ) const
{
	BM::ConvFolderMark(wstrDir);
	if ( wstrDir.size() )
	{
		wchar_t const tLast = wstrDir.at(wstrDir.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDir += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );

	WIN32_FIND_DATA w32fd = {0,};

	std::wstring kFindDir;
	if(lpExtension == L"*.dmp")
	{
		kFindDir = wstrDir;
		kFindDir += L"*.*";
	}
	else
	{
		kFindDir = m_wstrDir + wstrDir;
		kFindDir += lpExtension;
	}

	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	if ( INVALID_HANDLE_VALUE != hFindFile )
	{
		do 
		{
			__int64 i64Temp = static_cast<__int64>(w32fd.nFileSizeHigh);
			i64Temp <<= 32;
			i64Temp += static_cast<__int64>(w32fd.nFileSizeLow);

			std::wstring const wstrCurFileName = w32fd.cFileName;
			std::wstring::size_type findOffset = 0;

			findOffset = wstrCurFileName.find(L"StackDump");
			if(std::wstring::npos == findOffset)
			{
				findOffset = wstrCurFileName.find(L"dmp");
				if(std::wstring::npos == findOffset)
				{
					continue;
				}
			}

			rkContFile.insert( std::make_pair( wstrCurFileName, i64Temp) );
		} while ( TRUE == ::FindNextFileW( hFindFile, &w32fd ) );

		::FindClose( hFindFile );
		hFindFile = NULL;
	}

	if( rkContFile.size() )
	{
		return true;
	}

	return false;
}

void PgLogCopyer::FileOrder( std::wstring wstrDir, std::wstring wstrBakDir, FILETIME const &kOrderFileTime )
{
	if ( wstrDir.size() )
	{
		wchar_t const tLast = wstrDir.at(wstrDir.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDir += _T("\\");
		}
	}

	bool bDeleteFile = false;
	if ( wstrBakDir.size() )
	{
		wchar_t const tLast = wstrBakDir.at(wstrBakDir.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrBakDir += _T("\\");
		}

		BM::ReserveFolder( wstrBakDir );
	}
	else
	{
		bDeleteFile = true;
	}

	std::wstring kFindDir = wstrDir;
	kFindDir += L"*.*";

	WIN32_FIND_DATA w32fd = {0,};

	__int64 const * const pi64OrderTime = reinterpret_cast<__int64 const *>(&kOrderFileTime);
	
	FILETIME kLastFileTime;
	::memset( &kLastFileTime, 0, sizeof(FILETIME) );
	__int64 * pi64LastFileTime = reinterpret_cast<__int64*>(&kLastFileTime);

	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	if ( INVALID_HANDLE_VALUE != hFindFile )
	{
		do 
		{
			std::wstring const kCurName = w32fd.cFileName;

			if( FILE_ATTRIBUTE_DIRECTORY & w32fd.dwFileAttributes )
			{
				if (	kCurName != L"." 
					&&	kCurName != L".."
					)
				{
					// 재귀 호출
					std::wstring const wstrDir2 = wstrDir + kCurName + L"\\";
					std::wstring const wstrBakDir2 = ( ( true == bDeleteFile ) ? std::wstring() : (wstrBakDir + kCurName + L"\\") );

					FileOrder( wstrDir2, wstrBakDir2, kOrderFileTime );
				}
			}
			else
			{
				__int64 const *pi64LastWriteTime = reinterpret_cast<__int64 const *>(&w32fd.ftLastWriteTime);
				if ( *pi64OrderTime > *pi64LastWriteTime )
				{
					// 정리 대상이다.
					std::wstring const wstrExistingFileName = wstrDir + kCurName;

					if ( true == bDeleteFile )
					{
						if ( TRUE == ::DeleteFileW( wstrExistingFileName.c_str() ) )
						{
							INFO_LOG( BM::LOG_LV5, L"[DeleteFile] " << wstrExistingFileName );
						}
						else
						{
							INFO_LOG( BM::LOG_LV3, L"[DeleteFile] Failed " << wstrExistingFileName );
						}
					}
					else
					{
						std::wstring const wstrNewFileName = wstrBakDir + kCurName;
						if ( TRUE == ::MoveFileW( wstrExistingFileName.c_str(), wstrNewFileName.c_str() ) )
						{
							INFO_LOG( BM::LOG_LV5, L"[MoveFile] " << wstrExistingFileName << L" -> " << wstrNewFileName );
						}
						else
						{
							INFO_LOG( BM::LOG_LV3, L"[MoveFile] Failed " << wstrExistingFileName << L" -> " << wstrNewFileName );
						}
					}

				}
				else if ( *pi64LastFileTime < *pi64LastWriteTime )
				{
					*pi64LastFileTime = *pi64LastWriteTime;
				}
			}

		} while ( TRUE == ::FindNextFileW( hFindFile, &w32fd ) );

		::FindClose( hFindFile );
		hFindFile = NULL;
	}
}

void PgLogCopyer::Init( std::wstring const &wstrDir, std::wstring const &wstrBakDir, __int64 i64DeleteDay )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_wstrDir = wstrDir;
	m_wstrBakDir = wstrBakDir;

	BM::ConvFolderMark(m_wstrDir);
	BM::ConvFolderMark(m_wstrBakDir);

	SYSTEMTIME kNowTime;
	::GetLocalTime( &kNowTime );

	FILETIME kFileTime;
	::SystemTimeToFileTime( &kNowTime, &kFileTime );

	// 5일전꺼는 정리 대상이다.
	__int64 i64BackDay = 5i64;
	CGameTime::AddTime( &kFileTime, CGameTime::OneDay * (-i64BackDay) );

	INFO_LOG( BM::LOG_LV6, L"------------------------------------------------------" );
	INFO_LOG( BM::LOG_LV6, L"* Log File Check Start" );

	FileOrder( m_wstrDir, m_wstrBakDir, kFileTime );

	INFO_LOG( BM::LOG_LV6, L"* Log File Check End" );
	INFO_LOG( BM::LOG_LV6, L"======================================================" );

	if ( i64DeleteDay > i64BackDay )
	{
		i64DeleteDay -= i64BackDay;
		CGameTime::AddTime( &kFileTime, CGameTime::OneDay * ( -i64DeleteDay ) );

		INFO_LOG( BM::LOG_LV6, L"------------------------------------------------------" );
		INFO_LOG( BM::LOG_LV6, L"* Log File Delete Start" );

		FileOrder( wstrBakDir, std::wstring(), kFileTime );

		INFO_LOG( BM::LOG_LV6, L"* Log File Delete End" );
		INFO_LOG( BM::LOG_LV6, L"======================================================" );
	}
}

std::wstring PgLogCopyer::GetServerTypeName(short nServerType)
{
	std::wstring wstrName = L"";
	switch(nServerType)
	{
	case CEL::ST_NONE :{wstrName = L"NONE";}break;
	case CEL::ST_CENTER :{wstrName = L"CENTER";}break;
	case CEL::ST_MAP : wstrName = L"MAP"; break;
	case CEL::ST_LOGIN : wstrName = L"LOGIN"; break;
	case CEL::ST_SWITCH : wstrName = L"SWITCH"; break;
	case CEL::ST_MACHINE_CONTROL : wstrName = L"MMC"; break;
	case CEL::ST_SUB_MACHINE_CONTROL:	wstrName = L"SMC"; break;
	case CEL::ST_CONTENTS : wstrName = L"CONTENTS"; break;
	case CEL::ST_LOG : wstrName = L"LOG"; break;
	case CEL::ST_GMSERVER : wstrName = L"GMSERVER"; break;
	case CEL::ST_IMMIGRATION : wstrName = L"IMM"; break;
	case CEL::ST_CONSENT:{wstrName = L"CONSENT";}break;
	default:
		{
			wstrName = L"UNKNOWN";
		}break;
	}

	return wstrName;
}

void PgLogCopyer::SetServerInfo( CONT_SERVER_PROCESS& rkContServerInfo )
{
	if(!m_kContServerInfo.empty()) 
	{//! 이미 처리되었으니 또 할 필요는 없다.
		return;
	}

	CONT_SERVER_PROCESS::iterator ServerInfo_itor = rkContServerInfo.begin();
	TCHAR wstrName[64] = {0,};
	while(rkContServerInfo.end() != ServerInfo_itor)
	{
		_stprintf_s(wstrName, _countof(wstrName), _T("R%dC%d_%s%04d"), ServerInfo_itor->first.nRealm, ServerInfo_itor->first.nChannel, GetServerTypeName(ServerInfo_itor->first.nServerType).c_str(), ServerInfo_itor->first.nServerNo);
		m_kContServerInfo.insert(std::make_pair(wstrName, ServerInfo_itor->first));
		++ServerInfo_itor;
	}
}

void PgLogCopyer::GetLogInfo( CONT_LOG_INFO& rkContLog, int const iRealm, int const iChannel)
{
	if(m_kContServerInfo.empty()) 
	{
		return;
	}

	//! 검색 조건 (R1C1_Info ... 이런식으로 폴더 이름이 구성됨을 이용.)
	TCHAR findKey[MAX_PATH] = {0,};
	_stprintf_s(findKey, _countof(findKey), _T("R%dC"), iRealm);
	std::wstring wstrFindKey(findKey);

	WIN32_FIND_DATA w32fd = {0,};

	CONT_SERVER_INFO::iterator ServerInfo_itor = m_kContServerInfo.begin();
	while(ServerInfo_itor != m_kContServerInfo.end())
	{//! 이 SMC에서 사용되는 서버 리스트 ( 로그는 어차피 같은 폴더에 이름만 다르게 해서 남겨진다 )
		std::wstring const wstrCurFolderName =ServerInfo_itor->first.c_str();
		std::wstring kFindDir = m_wstrDir + wstrCurFolderName;
		kFindDir += L"\\*.*";

		//! 폴더가 존재하는지 확인한다.
		HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
		if ( INVALID_HANDLE_VALUE != hFindFile && 0 == ::_tcsupr_s(w32fd.cFileName) && (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			std::wstring::size_type findOffset = 0;
			findOffset = wstrCurFolderName.find(wstrFindKey);
			if(std::wstring::npos != findOffset)
			{
				//! 해당 폴더의 파일리스트에서 로그정보를 얻자
				SLogInfo kLogInfo;
				GetLogInfoToFileList(wstrCurFolderName, kLogInfo);

				//! 로그 정보를 컨테이너에 담자.
				rkContLog.insert(std::make_pair(ServerInfo_itor->second, kLogInfo));
			}
		}
		++ServerInfo_itor;
	}
}
PgLogCopyer::CONT_DUMP_INFO const & PgLogCopyer::GetDumpInfo( std::wstring wstrDir )
{
	std::wstring wstrDirName;
	for(short i=0; i<14; ++i)
	{
		wstrDirName.clear();
		wstrDirName = wstrDir.c_str();
		BM::ConvFolderMark(wstrDirName);
		if ( wstrDirName.size() )
		{
			wchar_t const tLast = wstrDirName.at(wstrDirName.size()-1);
			if( tLast != _T('\\') 
				&&	tLast != _T('/') )
			{
				wstrDirName += _T("\\");
			}
		}
		wstrDirName += GetServerTypeName(i).c_str();
		BM::ConvFolderMark(wstrDirName);
		
		const wchar_t* lpFolderName = wstrDirName.c_str();
		if(IsFileExist(lpFolderName))
		{// 폴더가 존재한다면..
			PgLogCopyer::CONT_LOG_FILE_INFO kContDump;
			if(!g_kLogCopyer.GetFileList(wstrDirName, kContDump, L"*.dmp")) 
			{//! 파일이 한개도 없을경우 패스
				continue;
			}

			m_kContDumpInfo.insert(std::make_pair(i, kContDump));
		}
	}

	return m_kContDumpInfo;
}

void PgLogCopyer::GetFolderList( std::wstring wstrDirRoot, CONT_FOLDER_LIST &rkContFolder ) const
{
	BM::ConvFolderMark(wstrDirRoot);
	if ( wstrDirRoot.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrDirRoot.at(wstrDirRoot.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDirRoot += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrDir;
	if(!wstrDirRoot.empty()) kFindDir = m_wstrDir + wstrDirRoot;
	//! 검색할 파일 확장자를 뒤쪽에 추가
	kFindDir += L"*.*";

	//! 첫번째 파일을 찾는다...
	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	
	if ( INVALID_HANDLE_VALUE == hFindFile ) 
	{//! 폴더가 비어있으므로 종료하자.
		return;
	}

	do
	{//! 루프를 돌면서 파일 검색.
		if(0 != ::_tcsupr_s(w32fd.cFileName) ) 
		{
			break;
		}

		std::wstring const kCurFolderName = w32fd.cFileName;
		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{//! 디렉토리 이름인 경우.
			if( kCurFolderName == _T(".") || kCurFolderName == _T("..") ) 
			{
				continue;
			}

			//! 이상 없으면 리스트에 넣자.
			rkContFolder.push_back( kCurFolderName );
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );
}

void PgLogCopyer::ReserveFindList( std::wstring wstrFindKey, CONT_FOLDER_LIST &rkContFolder ) const
{
	//! 리스트에서 조건에 맞는것을 찾는다.
	CONT_FOLDER_LIST::iterator Folder_itor = rkContFolder.begin();
	std::wstring::size_type findOffset = 0;
	
	while(Folder_itor != rkContFolder.end())
	{
		findOffset = Folder_itor->find(wstrFindKey);
		if(std::wstring::npos == findOffset)
		{//! 조건에 맞지 않는 폴더는 리스트에서 제거한다.
			Folder_itor = rkContFolder.erase(Folder_itor);
		}
		else
		{
			++Folder_itor;
		}
	}
}

void PgLogCopyer::GetLogInfoToFileList( std::wstring wstrDir, SLogInfo& rkLogInfo) const
{
	BM::ConvFolderMark(wstrDir);
	if ( wstrDir.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrDir.at(wstrDir.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDir += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrDir;
	if(!wstrDir.empty()) kFindDir = m_wstrDir + wstrDir;
	//! 검색할 파일 확장자를 뒤쪽에 추가
	kFindDir += L"*.*";

	//! 첫번째 파일을 찾는다...
	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	//! 폴더가 비어있으므로 종료하자.
	if ( INVALID_HANDLE_VALUE == hFindFile ) return;

	do
	{//! 루프를 돌면서 파일 검색.
		if(0 != ::_tcsupr_s(w32fd.cFileName) ) break;

		std::wstring const kCurFolderName = w32fd.cFileName;
		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{//! 디렉토리 이름인 경우, 패스
			continue;
		}
		else
		{//! 파일인 경우
			__int64 i64FileSize = static_cast<__int64>(w32fd.nFileSizeHigh);
			i64FileSize <<= 32;
			i64FileSize += static_cast<__int64>(w32fd.nFileSizeLow);

			std::wstring::size_type findOffset = 0;
			std::wstring wstrFindKey = L"CEL";
			findOffset = kCurFolderName.find(wstrFindKey);
			if(std::wstring::npos != findOffset)
			{
				rkLogInfo.m_dwCount2++;
				rkLogInfo.m_dwSize2 += i64FileSize;
				continue;
			}

			findOffset = 0;
			wstrFindKey = L"INFO";
			findOffset = kCurFolderName.find(wstrFindKey);
			if(std::wstring::npos != findOffset)
			{
				rkLogInfo.m_dwCount1++;
				rkLogInfo.m_dwSize1 += i64FileSize;
				continue;
			}
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );
}

void PgLogCopyer::GetFileInfo( BM::Stream *pkPacket, CONT_LOG_FILE_INFO &rkContFile )
{
	BM::CAutoMutex kLock( m_kMutex );

	SFileType kFileType;
	pkPacket->Pop(kFileType.m_bType1);
	pkPacket->Pop(kFileType.m_bType2);

	CONT_LOG_INFO kContLogInfo;
	PU::TLoadTable_AA(*pkPacket, kContLogInfo);

	CONT_LOG_INFO::iterator LogInfo_itor = kContLogInfo.begin();
	while(LogInfo_itor != kContLogInfo.end())
	{
		TCHAR wstrName[64] = {0,};
		std::wstring wstrFolderName;
		_stprintf_s(wstrName, _countof(wstrName), _T("R%dC%d_%s%04d"), LogInfo_itor->first.nRealm, LogInfo_itor->first.nChannel, 
			GetServerTypeName(LogInfo_itor->first.nServerType).c_str(), LogInfo_itor->first.nServerNo);
		wstrFolderName = wstrName;

		if(m_kContServerInfo.find(wstrFolderName) == m_kContServerInfo.end()) 
		{
			kContLogInfo.erase( LogInfo_itor++ );
			continue;
		}

		GetFileListToType(wstrName, kFileType, rkContFile);

		kContLogInfo.erase( LogInfo_itor++ );
	}
	return;
}

bool PgLogCopyer::GetFileListToType( std::wstring wstrDir, SFileType const &rkFileType, CONT_LOG_FILE_INFO &rkContFile ) const
{
	BM::ConvFolderMark(wstrDir);
	if ( wstrDir.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrDir.at(wstrDir.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDir += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrDir;
	if(!wstrDir.empty()) 
	{
		kFindDir = m_wstrDir + wstrDir;
	}

	//! 검색할 파일 확장자를 뒤쪽에 추가
	kFindDir += L"*.*";

	//! 첫번째 파일을 찾는다...
	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	
	if ( INVALID_HANDLE_VALUE == hFindFile ) 
	{//! 폴더가 비어있으므로 종료하자.
		return false;
	}

	do
	{//! 루프를 돌면서 파일 검색.
		if(0 != ::_tcsupr_s(w32fd.cFileName) )
		{
			break;
		}

		std::wstring const kCurFolderName = w32fd.cFileName;
		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{//! 디렉토리 이름인 경우, 패스
			continue;
		}
		else
		{//! 파일인 경우
			__int64 i64FileSize = static_cast<__int64>(w32fd.nFileSizeHigh);
			i64FileSize <<= 32;
			i64FileSize += static_cast<__int64>(w32fd.nFileSizeLow);

			std::wstring::size_type findOffset = 0;
			std::wstring wstrFindKey;

			if(rkFileType.m_bType1) 
			{
				wstrFindKey = L"CEL";
			}

			findOffset = kCurFolderName.find(wstrFindKey);
			if(std::wstring::npos == findOffset)
			{
				if(!rkFileType.m_bType2) 
				{
					continue;
				}

				wstrFindKey = L"INFO";
				findOffset = kCurFolderName.find(wstrFindKey);
			}
			if(std::wstring::npos != findOffset)
			{//! Full Path로 보내자. (안그럼 나중에 또 검색해야 됨..
				kFindDir = m_wstrDir;
				if(!wstrDir.empty()) 
				{
					kFindDir = wstrDir;
				}

				kFindDir += w32fd.cFileName;

				rkContFile.insert( std::make_pair( kFindDir, i64FileSize) );
			}
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );

	return true;
}

bool PgLogCopyer::IsFileExist(wchar_t const * lpFileName)
{
	DWORD dwFileAttr;

	dwFileAttr = GetFileAttributes(lpFileName);
	if (INVALID_FILE_ATTRIBUTES == dwFileAttr)
		return false;

	return true;
}