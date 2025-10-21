#include "stdafx.h"
#include "DataPack/PackInfo.h"
#include "DataPack/PgDataPackManager.h"
#include "PgLogCopyMgr.h"
#include "GeneralDlg.h"
#include "PgServerCmdMgr.h"

// -----------------------------------------------------
// CopyMgr
// -----------------------------------------------------
PgLogCopyMgr::PgLogCopyMgr(void)
{
	m_wstrRootPath = L"RecvFiles\\";// config에서 read하는 걸로 변경 예정
	m_kIsStopDownload = false;
	m_kFileType = 0;
}

PgLogCopyMgr::~PgLogCopyMgr(void)
{

}

void PgLogCopyMgr::Locked_Connect( CEL::SESSION_KEY const &kSessionKey )
{
	BM::CAutoMutex kLock( m_kMutex );
	m_kMMC_ID = kSessionKey;
}

__int64 PgLogCopyMgr::GetDownloadList( BM::CPacket * pkPacket, SLogCopyer::CONT_LOG_FILE_INFO& rkContOutFileInfo )
{
	__int64 i64TotalFileSize = 0;
	std::wstring wstrPath, wstrRootPath = m_wstrRootPath;
	pkPacket->Pop( wstrPath );
	if(wstrPath.empty()) 
	{//! 폴더 이름이 없다, 종료.
		return i64TotalFileSize;
	}

	wstrRootPath += wstrPath;
	BM::ReserveFolder( wstrRootPath );

	SLogCopyer::CONT_LOG_FILE_INFO kCurFileList;
	PU::TLoadTable_AA(*pkPacket, kCurFileList);

	WIN32_FIND_DATA w32fd = {0,};
	wstrRootPath += L"*.*";

	HANDLE hFindFile = ::FindFirstFileW( wstrRootPath.c_str(), &w32fd );
	if ( INVALID_HANDLE_VALUE != hFindFile )
	{
		do 
		{
			SLogCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_itor = kCurFileList.find( std::wstring(w32fd.cFileName) );
			if ( FileInfo_itor != kCurFileList.end() )
			{
				__int64 i64Temp = static_cast<__int64>(w32fd.nFileSizeHigh);
				i64Temp <<= 32;
				i64Temp += static_cast<__int64>(w32fd.nFileSizeLow);
				if ( i64Temp == FileInfo_itor->second )
				{
					kCurFileList.erase(FileInfo_itor);
				}
			}

		} while ( TRUE == ::FindNextFileW( hFindFile, &w32fd ) );

		::FindClose( hFindFile );
		hFindFile = NULL;


		SLogCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_itor = kCurFileList.begin();
		while(FileInfo_itor != kCurFileList.end())
		{
			std::wstring kTempKey = wstrPath+FileInfo_itor->first;
			rkContOutFileInfo.insert( std::make_pair(kTempKey, FileInfo_itor->second) );
			i64TotalFileSize += FileInfo_itor->second;
			FileInfo_itor = kCurFileList.erase(FileInfo_itor);
		}
	}

	return i64TotalFileSize;
}

void PgLogCopyMgr::Locked_Recv_PT_MMC_MCT_LOG_FILE_INFO( BM::CPacket * pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	__int64 i64TotalSize = 0;
	SLogCopyer::CONT_LOG_FILE_INFO kFileList;
	std::wstring wstrFolder;
	size_t uiCountFolder;
	pkPacket->Pop( uiCountFolder );
	while(uiCountFolder)
	{
		i64TotalSize += GetDownloadList(pkPacket, kFileList);	
		--uiCountFolder;
	}

	size_t usTotalFileCount = kFileList.size();
	if(usTotalFileCount == 0)
	{
		return;
	}

	//받을 목록 뽑기.
	SLogCopyer::CONT_DOWNLOAD &kElement = m_kLogCopyer.m_kContDownLoad;
	auto kPair = kElement.insert( std::make_pair( wstrFolder, kFileList ) );
	if ( !kPair.second )
	{
		kPair.first->second.swap( kFileList );
	}

	g_kMainDlg.InitDownloadInfo( usTotalFileCount, i64TotalSize);
	ProcessDownLoad( m_kLogCopyer );
}

void PgLogCopyMgr::Locked_Recv_PT_SMC_MCT_LOG_FILE_INFO( BM::CPacket * pkPacket )
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_LOG_FILE_INFO kFileList;
	PU::TLoadTable_AA(*pkPacket, kFileList);

	std::wstring wstrFolder;
	size_t usTotalFileCount = kFileList.size();
	if(usTotalFileCount == 0)
	{
		return;
	}
	__int64 i64TotalSize = 0;
	SLogCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_itor = kFileList.begin();
	while(FileInfo_itor != kFileList.end())
	{
		//WIN32_FIND_DATA w32fd = {0,};
		//wstrFolder = m_wstrRootPath + itr->first;

		//HANDLE hFindFile = ::FindFirstFileW( wstrFolder.c_str(), &w32fd );
		//if ( INVALID_HANDLE_VALUE != hFindFile )
		//{

		//}
		//::FindClose( hFindFile );
		//hFindFile = NULL;

		i64TotalSize += FileInfo_itor->second;
		++FileInfo_itor;
	}

	//받을 목록 뽑기.
	SLogCopyer::CONT_DOWNLOAD &kElement = m_kLogCopyer.m_kContDownLoad;
	auto kPair = kElement.insert( std::make_pair( wstrFolder, kFileList ) );
	if ( !kPair.second )
	{
		kPair.first->second.swap( kFileList );
	}

	g_kMainDlg.InitDownloadInfo( usTotalFileCount, i64TotalSize);
	ProcessDownLoad( m_kLogCopyer );

	pkPacket->Clear();
	SAFE_DELETE(pkPacket);
}

void PgLogCopyMgr::ProcessDownLoad( SLogCopyer &kCopyer, std::wstring const &wstrOldfile )
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_DOWNLOAD &kElement = kCopyer.m_kContDownLoad;

	SLogCopyer::CONT_DOWNLOAD::iterator Download_itor = kElement.begin();
	while(!IsStopDownload())
	{
		if(Download_itor == kElement.end())
		{//! Download Complete!
			g_kMainDlg.EndDownload();
			return;
		}
		SLogCopyer::CONT_LOG_FILE_INFO &kFileList = Download_itor->second;
		SLogCopyer::CONT_LOG_FILE_INFO::iterator FileInfo_itor = kFileList.begin();

		if( FileInfo_itor != kFileList.end() )
		{
			//폴더와 파일 컨터이너 비면 지워야함.
			std::wstring wstrFileName = Download_itor->first + FileInfo_itor->first;
			g_kServerCmdMgr.OnReqGetFile(m_kFileType, wstrFileName, wstrOldfile);

			std::wstring wstrFolderName;
			BM::DivFolderAndFileName(FileInfo_itor->first, wstrFolderName, wstrFileName);
			g_kMainDlg.UpdateDownloadInfo(wstrFileName, FileInfo_itor->second);
			kFileList.erase( FileInfo_itor );

			INFO_LOG( BM::LOG_LV2, L"MCT_REQ_GET_FILE : [FileType : " << m_kFileType << L"] [FileName : " << wstrFileName.c_str() << L"]" );
			return;
		}

		kElement.erase(Download_itor++);
	}

	if ( IsStopDownload() )
	{
		CancelDownload();
	}
}

void PgLogCopyMgr::Locked_Recv_PT_MMC_MCT_ANS_GET_FILE( BM::CPacket * pkPacket )
{
	std::wstring wstrFileName;
	std::vector<char> kFileData;
	pkPacket->Pop(wstrFileName);
	pkPacket->Pop(kFileData);

	BM::CAutoMutex kLock( m_kMutex );

	INFO_LOG( BM::LOG_LV2, _T("Recv File [") << wstrFileName.c_str() << _T("]") );
	if( kFileData.empty() )
	{//! 파일 데이터가 없음, 다운로드 취소 시킴.
		CAUTION_LOG( BM::LOG_LV5, _T("Recv File Failed [") << wstrFileName.c_str() << _T("] size empty") );
		//g_kMainDlg.EndDownload();
		//return;
	}
	else
	{
		std::wstring wstrFullPathName = m_wstrRootPath;
		wstrFullPathName += wstrFileName;
		BM::ReserveFolder( wstrFullPathName );

		if( !BM::MemToFile( wstrFullPathName, kFileData ))
		{
			if ( kFileData.empty() )
			{
				CAUTION_LOG( BM::LOG_LV5, _T("Recv File Failed [") << wstrFileName.c_str() << _T("] size empty") );
			}
			else
			{
				CAUTION_LOG( BM::LOG_LV5, _T("Recv File Failed [") << wstrFileName.c_str() << _T("]") );
			}

			wstrFileName.clear();// 실패했으니까 지우지 않게
		}
	}

	ProcessDownLoad( m_kLogCopyer, wstrFileName );
}

int PgLogCopyMgr::Locked_Recv_MCT_REFRESH_LOG( BM::CPacket* pkPacket, SLogCopyer::CONT_LOG_INFO& rkContLogInfo)
{
	BM::CAutoMutex kLock( m_kMutex );

	int iKeyValue = 0;
	pkPacket->Pop(iKeyValue);

	if(iKeyValue == FT_DUMP) 
	{//! 덤프 파일 정보는 타입이 달라서 따로 처리해준다.
		std::wstring wstrAddrSMC;
		pkPacket->Pop(wstrAddrSMC);

		SLogCopyer::CONT_DUMP_INFO kContDumpInfo;
		PU::TLoadTable_AA(*pkPacket, kContDumpInfo);

		SLogCopyer::CONT_LIST_DUMP_INFO::iterator ListDumpInfo_itor = m_kLogCopyer.m_kContDumpInfo.find(wstrAddrSMC);
		if( ListDumpInfo_itor != m_kLogCopyer.m_kContDumpInfo.end() )
		{//! 갱신인 경우, 들어온 패킷과 값을 교환해줌.
			ListDumpInfo_itor->second.swap(kContDumpInfo);
		}
		else
		{//! 처음 받는 데이터인 경우 추가해줌.
			m_kLogCopyer.m_kContDumpInfo.insert(std::make_pair(wstrAddrSMC, kContDumpInfo));
		}
	}
	else 
	{//! 기타 로그 파일 정보는 묶어서 처리
		PU::TLoadTable_AA(*pkPacket, rkContLogInfo);

		SLogCopyer::CONT_LIST_LOG_INFO::iterator ListLogInfo_itor = m_kLogCopyer.m_kContLogInfo.find(iKeyValue);
		if( ListLogInfo_itor != m_kLogCopyer.m_kContLogInfo.end() )
		{//! 갱신인 경우, 들어온 패킷과 값을 교환해줌.
			ListLogInfo_itor->second.swap(rkContLogInfo);
		}
		else
		{//! 처음 받는 데이터인 경우 추가해줌.
			m_kLogCopyer.m_kContLogInfo.insert(std::make_pair(iKeyValue, rkContLogInfo));
		}
	}

	return iKeyValue;
}

bool PgLogCopyMgr::Locked_ReMoveAll()
{//! 모든 파일 삭제
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_FOLDER_LIST kContFolderList;
	
	if( !GetFolderList(L"", kContFolderList) || kContFolderList.empty() )
	{
		return false;
	}

	SLogCopyer::CONT_FOLDER_LIST::iterator FolderList_itor = kContFolderList.begin();
	while(FolderList_itor != kContFolderList.end())
	{
		RemoveDirectory(FolderList_itor->second.c_str());
		if(_wrmdir(FolderList_itor->second.c_str()))
		{
			FolderList_itor = kContFolderList.erase(FolderList_itor);
		}
		else
		{
			++FolderList_itor;
		}
	}

	return true;
}

bool PgLogCopyMgr::Locked_GetDumpInfo(std::wstring const & wstrMechineAddr, short const & sServerType, SLogCopyer::CONT_DUMP_INFO& rkContOutDumpInfo)
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_LIST_DUMP_INFO::iterator ListDumpInfo_itor = m_kLogCopyer.m_kContDumpInfo.find(wstrMechineAddr);
	if( ListDumpInfo_itor == m_kLogCopyer.m_kContDumpInfo.end() )
	{
		return false;
	}

	SLogCopyer::CONT_DUMP_INFO::iterator DumpInfo_itor = (*ListDumpInfo_itor).second.begin();
	while(DumpInfo_itor != (*ListDumpInfo_itor).second.end())
	{
		if(DumpInfo_itor->first == sServerType)
		{
			std::wstring wstrFileName = ServerConfigUtil::GetServerTypeName(sServerType);
			BM::AddFolderMark(wstrFileName);
			rkContOutDumpInfo.insert(std::make_pair(DumpInfo_itor->first, DumpInfo_itor->second));
			return true;
		}
		++DumpInfo_itor;
	}


	return false;
}

bool PgLogCopyMgr::Locked_GetLogInfo(int const iKeyValue, short sServerNo, SLogCopyer::CONT_LOG_INFO& rkContOutLogInfo) 
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_LIST_LOG_INFO::iterator ListLogInfo_itor = m_kLogCopyer.m_kContLogInfo.find(iKeyValue);
	if( ListLogInfo_itor == m_kLogCopyer.m_kContLogInfo.end() )
	{
		return false;
	}

	SLogCopyer::CONT_LOG_INFO::iterator LogInfo_itor = (*ListLogInfo_itor).second.begin();
	while(LogInfo_itor != (*ListLogInfo_itor).second.end())
	{
		if(LogInfo_itor->first.nServerNo == sServerNo)
		{
			rkContOutLogInfo.insert(std::make_pair(LogInfo_itor->first, LogInfo_itor->second));
			return true;
		}
		++LogInfo_itor;
	}


	return false;
}


bool PgLogCopyMgr::Locked_GetLogInfo(int const iKeyValue, SLogCopyer::CONT_LOG_INFO& rkContOutLogInfo)
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_LIST_LOG_INFO::iterator ListLogInfo_itor = m_kLogCopyer.m_kContLogInfo.find(iKeyValue);
	if( ListLogInfo_itor != m_kLogCopyer.m_kContLogInfo.end() )
	{
		rkContOutLogInfo = ListLogInfo_itor->second;
		return true;
	}

	return false;
}

bool PgLogCopyMgr::Locked_GetDumpInfo(SLogCopyer::CONT_LIST_DUMP_INFO& rkContOutDumpInfo) const
{
	BM::CAutoMutex kLock( m_kMutex );

	if(m_kLogCopyer.m_kContDumpInfo.empty())
	{//! 데이터가 없다.
		return false;
	}
	rkContOutDumpInfo = m_kLogCopyer.m_kContDumpInfo;
	return true;
}

bool PgLogCopyMgr::Locked_SetDumpInfo(SLogCopyer::CONT_LIST_DUMP_INFO const & rkContDumpInfo)
{
	BM::CAutoMutex kLock( m_kMutex );

	if(rkContDumpInfo.empty())
	{//! 데이터가 없다.
		return false;
	}

	m_kLogCopyer.m_kContDumpInfo = rkContDumpInfo;
	return true;
}

bool PgLogCopyMgr::Locked_GetTotalInfo(int const iKeyValue, SLogInfo& rkOutLogInfo)
{
	BM::CAutoMutex kLock( m_kMutex );

	SLogCopyer::CONT_LOG_INFO kContLogInfo;
	if(Locked_GetLogInfo(iKeyValue, kContLogInfo))
	{
		SLogCopyer::CONT_LOG_INFO::iterator LogInfo_itor = kContLogInfo.begin();
		while(LogInfo_itor != kContLogInfo.end())
		{
			rkOutLogInfo.m_dwSize1 += LogInfo_itor->second.m_dwSize1;
			rkOutLogInfo.m_dwSize2 += LogInfo_itor->second.m_dwSize2;
			rkOutLogInfo.m_dwCount1 += LogInfo_itor->second.m_dwCount1;
			rkOutLogInfo.m_dwCount2 += LogInfo_itor->second.m_dwCount2;
			++LogInfo_itor;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool PgLogCopyMgr::GetFolderList( std::wstring wstrDirTarget, SLogCopyer::CONT_FOLDER_LIST& rkContOutFolderList ) const
{
	BM::ConvFolderMark(wstrDirTarget);
	if ( wstrDirTarget.size() )
	{//! 폴더명 끝 체크하고 컨버팅.
		wchar_t const tLast = wstrDirTarget.at(wstrDirTarget.size()-1);
		if( tLast != _T('\\') 
			&&	tLast != _T('/') )
		{
			wstrDirTarget += _T("\\");
		}
	}

	BM::CAutoMutex kLock( m_kMutex );
	WIN32_FIND_DATA w32fd = {0,};

	//! 검색할 폴더를 셋팅. ( 기본 폴더 + 검색할 폴더 )
	std::wstring kFindDir = m_wstrRootPath;
	if(!wstrDirTarget.empty()) kFindDir = m_wstrRootPath + wstrDirTarget;

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
		{//! 디렉토리 이름인 경우.
			if( kCurFolderName == _T(".") || kCurFolderName == _T("..") ) 
			{// . 이나 .. 은 패스.
				continue;
			}

			//! first=int(날짜 비교를 위해서)  second=path(경로명저장을 위해)
			rkContOutFolderList.insert( std::make_pair( _wtoi(kCurFolderName.c_str()), (kCurFolderName) ) );
		}
	}
	while(::FindNextFileW( hFindFile, &w32fd ));

	::FindClose( hFindFile );
	return true;
}

void PgLogCopyMgr::CancelDownload()
{
	m_kLogCopyer.m_kContDownLoad.clear();
	g_kMainDlg.EndDownload();
}



std::wstring TansformFileSize(double& rdOutSize)
{
	std::wstring wstrSizeType = L"Byte";
	if(rdOutSize >= 1024.0f)
	{
		wstrSizeType = L"KB";
		rdOutSize /= 1024.0f;

		if(rdOutSize >= 1024.0f)
		{
			wstrSizeType = L"MB";
			rdOutSize /= 1024.0f;
			if(rdOutSize >= 1024.0f)
			{
				wstrSizeType = L"GB";
				rdOutSize /= 1024.0f;
			}
		}
	}
	return wstrSizeType;
}