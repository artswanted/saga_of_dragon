#include "stdafx.h"
#include "DataPack/PackInfo.h"
#include "DataPack/PgDataPackManager.h"
#include "PgFileCopyMgr.h"
#include "PgMMCManager.h"

// -----------------------------------------------------
// CopyMgr
// -----------------------------------------------------
PgFileCopyMgr::PgFileCopyMgr(void)
:	m_dwHackLogIntervalTime(3600000)
,	m_dwLastUpdateTime(0)
,	m_dwUpdateTickTime(0)
{
}

PgFileCopyMgr::~PgFileCopyMgr(void)
{

}

void PgFileCopyMgr::Locked_ReadFromConfig( LPCWSTR lpFileName )
{
	BM::CAutoMutex kLock( m_kMutex );

	wchar_t chValue[MAX_PATH] = {0,};
	::GetPrivateProfileStringW( L"COPY_LOG", L"PATH_LOG", L"..\\LogFiles\\Recv\\", chValue, MAX_PATH, lpFileName );
	m_wstrPathLog = chValue;

	::GetPrivateProfileStringW( L"COPY_LOG", L"PATH_DUMP", L".\\", chValue, MAX_PATH, lpFileName );
	m_wstrPathDump = chValue;

	BM::ConvFolderMark(m_wstrPathLog);
	if ( m_wstrPathLog.size() )
	{
		wchar_t const tLast = m_wstrPathLog.at(m_wstrPathLog.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			m_wstrPathLog += _T("\\");
		}
	}

	BM::ReserveFolder( m_wstrPathLog );

	DWORD const dwValue = static_cast<DWORD>( ::GetPrivateProfileIntW( L"COPY_LOG", L"HACKLOG_COPY_INTERVAL_MIN ", 3600000, lpFileName ) ) * 60 * 1000;
	if ( 0 == dwValue )
	{
		m_dwHackLogIntervalTime = 0;
	}
	else if ( m_dwHackLogIntervalTime < dwValue )
	{
		m_dwHackLogIntervalTime = dwValue;
	}

}

void PgFileCopyMgr::Locked_Init( ContServerSiteID const &kServerList )
{
	BM::CAutoMutex kLock( m_kMutex );

	if ( 0 == m_dwHackLogIntervalTime )
	{
		m_dwUpdateTickTime = 0xFFFFFFFF;
		return;
	}

	m_dwLastUpdateTime = BM::GetTime32();

	if ( kServerList.size() )
	{
		m_dwUpdateTickTime = m_dwHackLogIntervalTime / static_cast<DWORD>(kServerList.size());
		if ( (1000 * 30) > m_dwUpdateTickTime )
		{
			m_dwUpdateTickTime = 1000 * 30;
		}
	}
	else
	{
		m_dwUpdateTickTime = m_dwHackLogIntervalTime;
	}
}

void PgFileCopyMgr::Locked_Tick()
{
	if ( true == g_kMMCMgr.Locked_IsLoadPatchFileList() )
	{
		return;
	}

	BM::CAutoMutex kLock( m_kMutex );

	if ( 0 == m_dwHackLogIntervalTime )
	{
		return;
	}

	DWORD const dwCurTime = BM::GetTime32();

	if ( m_kContCopyer.size() )//&& (DifftimeGetTime( m_dwLastUpdateTime, dwCurTime ) >= m_dwUpdateTickTime ) )
	{
		CONT_SMC_DOWNLOAD::const_iterator Download_itor = m_kContCopyer.find( m_kTickCheckID );
		if ( Download_itor != m_kContCopyer.end() )
		{
			BM::Stream kPacket( PT_MMC_SMC_GET_LOG_FILE_INFO, static_cast<size_t>(2) );
			kPacket.Push( std::wstring( L"HackLog\\") );
			kPacket.Push( std::wstring( L"CautionLog\\") );
			g_kCoreCenter.Send( Download_itor->first, kPacket );

			m_dwLastUpdateTime = dwCurTime;
			++Download_itor;
		}

		if ( Download_itor == m_kContCopyer.end() )
		{
			m_kTickCheckID = m_kContCopyer.begin()->first;
		}
		else
		{
			m_kTickCheckID = Download_itor->first;
		}
	}
}

void PgFileCopyMgr::Locked_Connect( CEL::SESSION_KEY const &kSessionKey )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_kContCopyer.insert( std::make_pair( kSessionKey, CONT_SMC_DOWNLOAD::mapped_type() ) );
}

void PgFileCopyMgr::Locked_Disconnect( CEL::SESSION_KEY const &kSessionKey )
{
	BM::CAutoMutex kLock( m_kMutex );
	
	if ( m_kTickCheckID == kSessionKey )
	{
		CONT_SMC_DOWNLOAD::iterator Download_itor = m_kContCopyer.find( m_kTickCheckID );
		if ( Download_itor != m_kContCopyer.end() )
		{
			Download_itor = m_kContCopyer.erase( Download_itor );
		}

		if ( Download_itor != m_kContCopyer.end() )
		{
			m_kTickCheckID = Download_itor->first;
		}
	}
	else
	{
		m_kContCopyer.erase( kSessionKey );
	}
}

void PgFileCopyMgr::Locked_Recv_PT_SMC_MMC_ANS_LOG_FILE_INFO( CEL::SESSION_KEY const &kSessionkey, BM::Stream * pkPacket )
{
	std::wstring wstrFolder;
	std::wstring wstrLocalFolder;
	pkPacket->Pop( wstrFolder );
	GetPath( wstrFolder, wstrLocalFolder );

	SFileCopyer::CONT_LOG_FILE_INFO kFileList;
	PU::TLoadTable_AA(*pkPacket, kFileList);//받은걸로 내꺼와 비교

	BM::CAutoMutex kLock( m_kMutex );

	WIN32_FIND_DATA w32fd = {0,};
	wstrLocalFolder += L"*.txt";

	HANDLE hFindFile = ::FindFirstFileW( wstrLocalFolder.c_str(), &w32fd );
	if ( INVALID_HANDLE_VALUE != hFindFile )
	{
		do 
		{
			SFileCopyer::CONT_LOG_FILE_INFO::iterator itr = kFileList.find( std::wstring(w32fd.cFileName) );
			if ( itr != kFileList.end() )
			{
				__int64 i64Temp = static_cast<__int64>(w32fd.nFileSizeHigh);
				i64Temp <<= 32;
				i64Temp += static_cast<__int64>(w32fd.nFileSizeLow);
				if ( i64Temp == itr->second )
				{
					kFileList.erase( itr );
				}
			}

		} while ( TRUE == ::FindNextFileW( hFindFile, &w32fd ) );

		::FindClose( hFindFile );
		hFindFile = NULL;
	}

	//받을 목록 뽑기.
	CONT_SMC_DOWNLOAD::iterator Download_itor = m_kContCopyer.find( kSessionkey );
	if ( Download_itor != m_kContCopyer.end() )
	{
		SFileCopyer::CONT_DOWNLOAD &kElement = Download_itor->second.m_kContDownLoad;
		auto kPair = kElement.insert( std::make_pair( wstrFolder, kFileList ) );
		if ( !kPair.second )
		{
			kPair.first->second.swap( kFileList );
		}

		ProcessDownLoad( Download_itor );
	}
}

void PgFileCopyMgr::Locked_Recv_PT_MMC_SMC_ANS_GET_FILE( CEL::SESSION_KEY const &kSessionkey, BM::Stream * pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	std::wstring strFileName;
	std::vector<char> kFileData;
	pkPacket->Pop(strFileName);
	pkPacket->Pop(kFileData);

	if( kFileData.size() == NULL )
	{
		INFO_LOG( BM::LOG_LV5, _T("Recv File Failed [") << strFileName.c_str() << _T("] size empty") );
		return;
	}

	std::wstring wstrFile;
	GetPath( strFileName, wstrFile );

	if( !BM::MemToFile( wstrFile, kFileData ))
	{
		if ( kFileData.empty() )
		{
			INFO_LOG( BM::LOG_LV5, _T("Recv File Failed [") << strFileName.c_str() << _T("] size empty") );
		}
		else
		{
			INFO_LOG( BM::LOG_LV5, _T("Recv File Failed [") << strFileName.c_str() << _T("]") );
		}

		strFileName.clear();// 실패했으니까 지우지 않게
	}

	CONT_SMC_DOWNLOAD::iterator Download_itor = m_kContCopyer.find( kSessionkey );
	if ( Download_itor != m_kContCopyer.end() )
	{
		ProcessDownLoad( Download_itor, strFileName );
	}
}

void PgFileCopyMgr::ProcessDownLoad( CONT_SMC_DOWNLOAD::iterator Download_itor, std::wstring const &wstrOldfile )
{
	SFileCopyer::CONT_DOWNLOAD &kElement = (Download_itor->second).m_kContDownLoad;
	SFileCopyer::CONT_DOWNLOAD::iterator Head_itor = kElement.begin();

	while(Head_itor != kElement.end())
	{
		SFileCopyer::CONT_LOG_FILE_INFO &kFileList = Head_itor->second;
		SFileCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_itor = kFileList.begin();

		if( FileInfo_itor != kFileList.end() )
		{
			//폴더와 파일 컨터이너 비면 지워야함.
			std::wstring wstrFileName = Head_itor->first + FileInfo_itor->first;

			BM::Stream kPacket( PT_SMC_MMC_REQ_GET_FILE, wstrFileName );
			kPacket.Push( wstrOldfile );
			g_kCoreCenter.Send( Download_itor->first, kPacket );

			kFileList.erase( FileInfo_itor );
			return;
		}

		kElement.erase(Head_itor++);
	}

	if ( wstrOldfile.size() )
	{
		BM::Stream kPacket( PT_SMC_MMC_ANS_DATA_SYNC_END, wstrOldfile );//싱크 끝.
		g_kCoreCenter.Send( Download_itor->first, kPacket );
	}
}

void PgFileCopyMgr::GetFileList( std::wstring wstrPath, SFileCopyer::CONT_LOG_FILE_INFO& rkContFile )const
{
	BM::ConvFolderMark(wstrPath);
	if ( wstrPath.size() )
	{
		wchar_t const tLast = wstrPath.at(wstrPath.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrPath += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	std::wstring kFindDir = m_wstrPathLog + wstrPath;
	kFindDir += L"*.*";

	HANDLE hFindFile = ::FindFirstFileW( kFindDir.c_str(), &w32fd );
	if ( INVALID_HANDLE_VALUE != hFindFile )
	{
		do 
		{
			if(0 != ::_tcsupr_s(w32fd.cFileName) ) 
			{
				break;
			}
			if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
			{
				continue;
			}

			__int64 i64Temp = static_cast<__int64>(w32fd.nFileSizeHigh);
			i64Temp <<= 32;
			i64Temp += static_cast<__int64>(w32fd.nFileSizeLow);

			rkContFile.insert( std::make_pair( std::wstring(w32fd.cFileName), i64Temp) );
		} while ( TRUE == ::FindNextFileW( hFindFile, &w32fd ) ); 

		::FindClose( hFindFile );
		hFindFile = NULL;
	}
}
void PgFileCopyMgr::GetFolderList( std::wstring wstrPath, SFileCopyer::CONT_FOLDER_LIST &rkContFolder )const
{
	BM::ConvFolderMark(wstrPath);
	if ( wstrPath.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrPath.at(wstrPath.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrPath += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrPathLog;
	if(!wstrPath.empty()) 
	{
		kFindDir = m_wstrPathLog + wstrPath;
	}

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

			//! first=int(날짜 비교를 위해서)  second=path(경로명저장을 위해)
			rkContFolder.insert( std::make_pair( _wtoi(kCurFolderName.c_str()), kCurFolderName) );
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );
}

void PgFileCopyMgr::WriteFileTable(std::wstring wstrPath, BM::Stream *pkPacket, SFileCopyer::CONT_LOG_FILE_INFO &rkContFile)const
{
	GetFileList( wstrPath, rkContFile );

	if(!rkContFile.empty())
	{//! 파일이존재한다면 패킷에 집어넣자
		pkPacket->Push(wstrPath);
		PU::TWriteTable_AA( *pkPacket, rkContFile);
	}
	else
	{
		wstrPath.clear();
		pkPacket->Push(wstrPath);
	}
}

size_t PgFileCopyMgr::GetFileListToType( BM::Stream * pkPacket, SFileCopyer::CONT_LOG_FILE_INFO &rkContFile, SFileType &rkFileType, LPCWSTR lpDate)
{
	size_t uiCount = 0;
	if(rkFileType.m_iType1)
	{
		std::wstring wstrPath(lpDate);
		wstrPath += L"\\CautionLog\\";
		WriteFileTable(wstrPath, pkPacket, rkContFile);
		++uiCount;
	}
	if(rkFileType.m_iType2)
	{
		std::wstring wstrPath(lpDate);
		wstrPath += L"\\HackLog\\";
		WriteFileTable(wstrPath, pkPacket, rkContFile);
		++uiCount;
	}

	return uiCount;
}

void PgFileCopyMgr::Locked_Recv_MCT_REQ_GET_FILE( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	std::wstring kFileName;
	kFileName.clear();
	pkPacket->Pop(kFileName);

	std::vector< char > kFileData;

	std::wstring wstrDir = m_wstrPathLog + kFileName;
	BM::FileToMem( wstrDir, kFileData);

	BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO, MCT_MMC_ANS_GET_FILE);
	kPacket.Push(kFileName);
	kPacket.Push(kFileData);
	g_kCoreCenter.Send( kSessionkey, kPacket );
}

void PgFileCopyMgr::Locked_PT_MCT_REQ_GET_FILE_INFO( CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );
	
	SFileType kFileType;
	pkPacket->Pop(kFileType.m_iType1);
	pkPacket->Pop(kFileType.m_iType2);

	SFileCopyer::CONT_LOG_INFO kContLogInfo;
	PU::TLoadTable_AA(*pkPacket, kContLogInfo);

	SFileCopyer::CONT_LOG_FILE_INFO kFileList;

	BM::Stream kPacketData;
	size_t uiFolderCount = 0;
	SFileCopyer::CONT_LOG_INFO::iterator LogInfo_itor = kContLogInfo.begin();
	while(LogInfo_itor != kContLogInfo.end())
	{
		wchar_t wcString[32] = {0, };
		_ltot_s(LogInfo_itor->second.m_dwDate, wcString, 10);

		uiFolderCount += GetFileListToType(&kPacketData, kFileList, kFileType, wcString);
		kContLogInfo.erase( LogInfo_itor++ );
	}

	//! Tool에 응답을 날려준다.
	BM::Stream kPacket( PT_MMC_TOOL_NFY_INFO,  MCT_MMC_ANS_LOG_FILE_INFO);
	kPacket.Push(uiFolderCount);
	kPacket.Push(kPacketData);
	g_kCoreCenter.Send( kSessionkey, kPacket );
}

void PgFileCopyMgr::Locked_GetLogInfo(  CEL::SESSION_KEY const &kSessionkey, BM::Stream *pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	BM::Stream kPacket(PT_MMC_TOOL_NFY_INFO, MCT_REFRESH_LOG);

	int iKeyValue = 0;
	pkPacket->Pop(iKeyValue);
	kPacket.Push(iKeyValue);

	//! 폴더 리스트를 만들고
	std::wstring wstrDir;
	wstrDir.clear();
	GetFolderList(wstrDir, m_kFileCopyer.m_kContFolderList);

	if(m_kFileCopyer.m_kContFolderList.empty())
	{//! 폴더가 존재하지 않음, 종료.
		INFO_LOG( BM::LOG_LV5, _T("[ServerTool] Folder Is Empty!!") );
		return;
	}

	SFileCopyer::CONT_FOLDER_LIST &kElement = m_kFileCopyer.m_kContFolderList;
	SFileCopyer::CONT_FOLDER_LIST::iterator head_itor = kElement.begin();

	int const iCautionLogType = 0;
	int const iHackLogType = 1;
	std::wstring wstrCurFolderName;

	SLogInfo kLogInfo;
	SERVER_IDENTITY kServerIdentity;
	SFileCopyer::CONT_LOG_INFO kContLogInfo;
	while(head_itor != kElement.end())
	{//! 루프를 돌면서 파일 정보를 얻는다
		kLogInfo.Clear();
		kServerIdentity.nServerNo++;
		
		kLogInfo.m_dwDate = head_itor->first;

		wstrCurFolderName = (head_itor->second);
		wstrCurFolderName += L"\\CautionLog\\";
		GetLogInfoToFileList(wstrCurFolderName, kLogInfo, iCautionLogType);
		
		wstrCurFolderName = (head_itor->second);
		wstrCurFolderName += L"\\HackLog\\";
		GetLogInfoToFileList(wstrCurFolderName, kLogInfo, iHackLogType);

		kContLogInfo.insert(std::make_pair(kServerIdentity, kLogInfo));
		kElement.erase( head_itor++ );
	}

	PU::TWriteTable_AA( kPacket, kContLogInfo);

	g_kCoreCenter.Send( kSessionkey, kPacket );
}

bool PgFileCopyMgr::GetLogInfoToFileList( std::wstring &wstrPath, SLogInfo &kLogInfo, int iLogType) const
{
	BM::ConvFolderMark(wstrPath);
	if ( wstrPath.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrPath.at(wstrPath.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrPath += _T("\\");
		}
	}

	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrPathLog + wstrPath;
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
			::FindClose( hFindFile );
			return false;
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

			//! 나중에 배열로 바꾸자.. 젠장. -_ㅠ
			if(!iLogType)
			{
				kLogInfo.m_dwSize1 += i64FileSize;
				++kLogInfo.m_dwCount1;
			}
			else
			{
				kLogInfo.m_dwSize2 += i64FileSize;
				++kLogInfo.m_dwCount2;
			}
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );

	return true;
}