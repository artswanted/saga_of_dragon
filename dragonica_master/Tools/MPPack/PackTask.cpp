#include "StdAfx.h"
#include "PackTask.h"
#include "PFile.h"
#include "PackUtil.h"
#include "MainWorker.h"
#include "CEL/Corecenter.h"
#include "CEL/CEL.h"

PackTask::PackTask(void)
	: m_pkLoger(NULL)
{
	CEL::INIT_CENTER_DESC	kCenterInit;
	kCenterInit.eOT	= BM::OUTPUT_NOT_SET;
	kCenterInit.pOnRegist = PackTask::OnRegist;
	m_PrevVersionInfo.iVersion = 1;
	m_PatchVersionInfo.iVersion = 1;

	g_kCoreCenter.Init(kCenterInit);
	StartSvc(1);
}

PackTask::~PackTask(void)
{
	VDeactivate();
}

void PackTask::HandleMessage(MSG *rkMsg)
{
	if( rkMsg )
	{
		bool bSuccess = true;
		std::wstring kAddonText;

		if( m_pkLoger ){ m_pkLoger->LogMessage(rkMsg->kType, ELM_START); }

		switch( rkMsg->kType )
		{
		case EPT_DIFF:
			{
				bSuccess = StartDiff(rkMsg->kTGTDir, rkMsg->kCRTDir, rkMsg->kOUTDir, rkMsg->dwOption);
			}break;
		case EPT_PACK:
			{
				bSuccess = StartPack(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_CREATELIST:
			{
				bSuccess = StartCreateList(rkMsg->kTGTDir, rkMsg->kOUTDir, rkMsg->dwOption);
			}break;
		case EPT_HEADERMERGE:
			{
				bSuccess = StartHeaderMerge(rkMsg->kTGTDir, rkMsg->kCRTDir, rkMsg->kOUTDir);
			}break;
		case EPT_BIND:
			{
				bSuccess = StartBind(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_UNPACK:
			{
				bSuccess = StartUnPack(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_MAKEINB:
			{
				bSuccess = StartMakeINB(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_EXPORT_LIST:
			{
				bSuccess = StartExportList(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_CREATE_ID:
			{
				bSuccess = CreatePatchID(rkMsg->kOUTDir, kAddonText);
			}break;
		case EPT_DAT_VERSION_UP:
			{
				bSuccess = UpdateDATVersionInfo(rkMsg->kOUTDir, kAddonText);
			}break;
		case EPT_DAT_VERSION_CHECK:
			{
				bSuccess = CheckDATVersion(rkMsg->kOUTDir, kAddonText);
			}break;
		case EPT_DAT_CONVERT:
			{
				bSuccess = ConvertDatFile(rkMsg->kOUTDir);
			}break;
		case EPT_MAKE_MANUAL_PATCH:
			{
				bSuccess = MakeManualPatch(rkMsg->kTGTDir, rkMsg->kOUTDir);
			}break;
		case EPT_MAKE_AUTO_PATCH:
			{
				bSuccess = MakeAutoPatch(rkMsg->kTGTDir, rkMsg->kCRTDir, rkMsg->kOUTDir, kAddonText);
			}break;
		default:
			{

			}break;
		}

		if( m_pkLoger ){ m_pkLoger->LogMessage(rkMsg->kType, (bSuccess)?(ELM_END):(ELM_FAIL)); }
		if( bSuccess )
		{
			switch( rkMsg->kType )
			{
			case EPT_CREATE_ID:
			case EPT_DAT_VERSION_UP:
			case EPT_DAT_VERSION_CHECK:
				{
					E_LOG_MSG kMsg = static_cast<E_LOG_MSG>(ELM_PATCHID_VERSION_INFO+(rkMsg->kType - EPT_CREATE_ID));

					if( m_pkLoger ){ m_pkLoger->LogMessage(rkMsg->kType, kMsg, kAddonText); }
				}break;
			}
		}
	}
}

bool PackTask::StartDiff(std::wstring const& kDestDir, std::wstring const& kSourDir, std::wstring const& kOutDir, DWORD dwOption)
{
	bool bSuccess = true;

	//if( (dwOption & EDO_FILETOBIND) == EDO_FILETOBIND || (dwOption & EDO_FILETOPACK) == EDO_FILETOPACK )
	//{//아직 미지원

	//}
	//else
	{//일반
		std::wstring kRstDir = kOutDir;
		BM::DetachFolderMark(kRstDir);
		kRstDir += L"\\DiffResult";
		bSuccess = RecursiveDiff(kDestDir, kSourDir, kRstDir);
	}

	if( bSuccess )
	{
		if( (dwOption & EDO_DELLIST) == EDO_DELLIST )
		{//삭제된 파일들
			std::wstring kRstDir = kOutDir;
			BM::DetachFolderMark(kRstDir);
			kRstDir += L"\\DiffResult_DeleteFiles";
			bSuccess = RecursiveDiff(kSourDir, kDestDir, kRstDir);
		}
	}
	return bSuccess;
}

bool PackTask::StartPack(std::wstring const& kTgtDir, std::wstring const& kOutDir)
{
	bool bSuccess = true;

	std::wstring kRstDir = kOutDir;
	BM::DetachFolderMark(kRstDir);
	std::wstring wstrVersion;
	GetPatchID(EVT_PATCH_VERSION, wstrVersion);
	kRstDir += L"\\";
	kRstDir += wstrVersion;
	if( !RecursivePack(kTgtDir, kRstDir) )
	{//error
		bSuccess = false;
	}

	if( bSuccess )
	{
		//팩은 기본으로 HeaderFolders.DAT를 생성한다.
		StartCreateList(kTgtDir, kRstDir, ECO_TGT_PACKRESULT);
	}
	return bSuccess;
}

bool PackTask::StartCreateList(std::wstring const& kTgtDir, std::wstring const& kOutDir, DWORD dwOption)
{
	bool bSuccess = true;

	std::wstring kRstFileDir;

	if( dwOption != ECO_TGT_PACKRESULT )
	{
		if( !RecursiveCreateList(kTgtDir, kOutDir) )
		{//error
			bSuccess = false;
		}
	}

	if( bSuccess )
	{
		std::wstring kRstFileName = kOutDir + std::wstring(L"\\") + L"filelist.dat";
		if( !RecursiveCreateListMerge(kOutDir, kRstFileName) )
		{//error
			bSuccess = false;
		}
	}
	return bSuccess;
}

bool PackTask::StartHeaderMerge(std::wstring const& kNewFileDir, std::wstring const& kOldFileDir, std::wstring const& kOutDir)
{
	bool bSuccess = true;

	CPFile	kNewFile;
	CPFile	kOldFile;

	if( !kNewFile.Load(kNewFileDir) )
	{ 
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_HEADERMERGE, ELM_NOT_EXIST_NEW_HEADER_LIST); }
		bSuccess = false; 
	}
	if( !kOldFile.Load(kOldFileDir) )
	{ 
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_HEADERMERGE, ELM_NOT_EXIST_OLD_HEADER_LIST); }
		bSuccess = false; 
	}

	if( bSuccess )
	{
		BM::FolderHash const& kNewHash = kNewFile.GetHash();
		BM::FolderHash kOldHash(kOldFile.GetHash().begin(), kOldFile.GetHash().end());

		CPFile::FOLDER_CITER new_folder_iter = kNewHash.begin();
		while( new_folder_iter != kNewHash.end() )
		{
			CPFile::FOLDER_DIR NewFolderDir = new_folder_iter->first;
			LWR(NewFolderDir);
			CPFile::FOLDER_STATE const& NewFolderState = new_folder_iter->second;

			BM::FolderHash::iterator old_folder_iter = kOldHash.find( NewFolderDir );
			if( old_folder_iter != kOldHash.end() )
			{
				BM::FolderHash::key_type const OldFolderDir = old_folder_iter->first;
				BM::FolderHash::mapped_type& OldFolderState = old_folder_iter->second;

				CPFile::FILE_CITER new_file_iter = NewFolderState->kFileHash.begin();
				while( new_file_iter != NewFolderState->kFileHash.end() )
				{
					CPFile::FILE_DIR NewFileDir = new_file_iter->first;
					LWR(NewFileDir);
					CPFile::FILE_STATE const& NewFileState = new_file_iter->second;

					BM::FileHash::iterator old_file_iter = OldFolderState->kFileHash.find( NewFileDir );
					if( old_file_iter != OldFolderState->kFileHash.end() )
					{
						CPFile::FILE_STATE& OldFileState = old_file_iter->second;

						OldFileState.org_file_size = NewFileState.org_file_size;
						OldFileState.zipped_file_size = NewFileState.zipped_file_size;
						OldFileState.bIsEncrypt = NewFileState.bIsEncrypt;
						OldFileState.bIsZipped = NewFileState.bIsZipped;
						OldFileState.iOrgCRC = NewFileState.iOrgCRC;
						memcpy(OldFileState.ucSHA2CheckSum, NewFileState.ucSHA2CheckSum, PG_SHA2_LENGTH * sizeof(unsigned char));
					}
					else
					{						
						OldFolderState->kFileHash.insert(std::make_pair(NewFileDir, NewFileState));
					}
					++new_file_iter;
				}
			}
			else
			{
				kOldHash.insert(std::make_pair(NewFolderDir, NewFolderState));
			}
			++new_folder_iter;
		}

		CPFile kResultFile;
		std::wstring kFolderName;
		std::wstring kFileName;
		BM::DivFolderAndFileName(kNewFileDir, kFolderName, kFileName);
		std::wstring RstOutFileName = kOutDir;
		BM::AddFolderMark(RstOutFileName);
		RstOutFileName += kFileName;
		::DeleteFileW(RstOutFileName.c_str());
		kResultFile.Init(RstOutFileName, kOldHash);
	}
	return bSuccess;
}

bool PackTask::StartBind(std::wstring const& kTgtDir, std::wstring const& kOutDir)
{
	std::wstring kRstFileName = kOutDir;
	BM::AddFolderMark(kRstFileName);
	kRstFileName += BM::GetLastFolderName(kTgtDir);
	kRstFileName += L".dat";

	std::wstring kTempFileName = BM::GetLastFolderName(kTgtDir) + L".dat";

	if( ::PathFileExistsW( kRstFileName.c_str() ) )
	{
		::DeleteFileW( kRstFileName.c_str() );
	}

	if( ::PathFileExistsW( kTempFileName.c_str() ) )
	{
		::DeleteFileW( kTempFileName.c_str() );
	}

	DWORD time = ::timeGetTime();
	bool bSuccess = BM::PgDataPackManager::PackTargetFolder(kTgtDir, kTempFileName);
	DWORD time2 = ::timeGetTime() - time;	
	BM::vstring str;
	str += time2;

	if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_BIND, ELM_PROGRESS, str); }
	
	if( bSuccess && kOutDir.size() > 0 )
	{
		std::wstring kTempFileDir = std::wstring(L"..\\") + kTempFileName;
		PackUtil::UpdateBindVersion(kTempFileDir, m_PatchVersionInfo);
		BM::ReserveFolder(kRstFileName);
		::MoveFile(kTempFileDir.c_str(), kRstFileName.c_str());
	}
	return bSuccess;
}

bool PackTask::StartUnPack(std::wstring const& kDatFileDir, std::wstring const& kOutDir)
{
	bool bSuccess = true;
	CPFile	kFile;
	if( kFile.Load(kDatFileDir) )
	{
		BM::FolderHash const& kFolderHash = kFile.GetHash();

		if( !kFolderHash.size() )
		{
			if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_UNPACK, ELM_NOT_EXIST_UNPACK_FILE_LIST); }
			bSuccess = false;
		}

		if( bSuccess )
		{
			kFile.LoadFile(kDatFileDir);

			CPFile::FOLDER_CITER folder_iter = kFolderHash.begin();
			while( folder_iter != kFolderHash.end() )
			{
				CPFile::FOLDER_DIR const& kFolderDir = folder_iter->first;
				CPFile::FOLDER_STATE const& kFolderState = folder_iter->second;

				if( !kFile.FindFolder(kFolderDir) )
				{
					if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_UNPACK, ELM_NOT_EXIST_UNPACK_FOLDER_DATA, kFolderDir); }
					bSuccess = false;
				}

				if( bSuccess )
				{
					CPFile::FILE_CITER file_iter = kFolderState->kFileHash.begin();
					while( file_iter != kFolderState->kFileHash.end() )
					{
						CPFile::FILE_DIR const& kFileDir = file_iter->first;
						CPFile::FILE_STATE const& kFileState = file_iter->second;

						if( !kFile.FindFile(kFileDir) )
						{
							if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_UNPACK, ELM_NOT_EXIST_UNPACK_FILE_DATA, kFileDir); }
							bSuccess = false;
						}

						if( bSuccess )
						{
							std::vector<char> vecData;
							kFile.GetData(vecData);

							std::vector<char> vecOut;

							BM::DecLoadFromMem(kFileState.org_file_size, kFileState.zipped_file_size, kFileState.bIsZipped, kFileState.bIsEncrypt, vecData, vecOut);

							std::wstring kFolderName;
							std::wstring kFileName;
							BM::DivFolderAndFileName(kDatFileDir, kFolderName, kFileName);

							std::wstring::size_type folder_mark_pos =  kFolderDir.find(_T("."));
							std::wstring::size_type file_id_pos = kFileName.find(L".");
							std::wstring kRstOutDir = kOutDir;
							if(std::wstring::npos != file_id_pos)
							{
								kRstOutDir += L"\\";
								kRstOutDir += kFileName.substr(0, file_id_pos);
							}
							if(std::wstring::npos != folder_mark_pos)
							{
								kRstOutDir += kFolderDir.substr(folder_mark_pos + 1);
							}
							else
							{
								kRstOutDir += kFolderDir;
							}
							kRstOutDir += kFileDir;
							BM::MemToFile(kRstOutDir, vecOut);
						}

						++file_iter;
					}
				}
			
				++folder_iter;
			}
		}
	}

	kFile.Clear();

	return bSuccess;
}

bool PackTask::StartMakeINB(std::wstring const& kFileDir, std::wstring const& kOutDir)
{
	std::vector<char> vecData;
	BM::FileToMem(kFileDir, vecData);

	std::wstring kFolderName;
	std::wstring kFileName;
	BM::DivFolderAndFileName(kFileDir, kFolderName, kFileName);
	std::wstring RstFileName = kOutDir;
	BM::AddFolderMark(RstFileName);
	RstFileName += kFileName;
	RstFileName.at(RstFileName.size() - 1) = L'b';

	return BM::EncSave(RstFileName, vecData);
}

bool PackTask::StartExportList(std::wstring const& kDatFileDir, std::wstring const& kOutDir)
{
	bool bSuccess = true;

	std::wstring kFolderName;
	std::wstring kFileName;
	BM::DivFolderAndFileName(kDatFileDir, kFolderName, kFileName);

	std::wstring kRstFileName = kOutDir;
	BM::AddFolderMark(kRstFileName);
	kRstFileName+= kFileName + L"_List.TXT";

	HANDLE hFile = CreateFile(kRstFileName.c_str(), GENERIC_ALL, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_EXPORT_LIST, ELM_CREATE_EXPORT_FILE_FAIL); }
		bSuccess = false;
	}

	if( bSuccess )
	{
		DWORD	dwSize = 0;
		std::string const UTF16FILTER("\xFF\xFE");
		WriteFile(hFile, &UTF16FILTER.at(0), UTF16FILTER.size(), &dwSize, NULL);
		
		CPFile	kFile;
		if( kFile.Load(kDatFileDir) )
		{
			BM::FolderHash const& kFolderHash = kFile.GetHash();

			CPFile::FOLDER_CITER folder_iter = kFolderHash.begin();
			while( folder_iter != kFolderHash.end() )
			{
				CPFile::FOLDER_DIR const& kFolderDir = folder_iter->first;
				CPFile::FOLDER_STATE const& kFolderState = folder_iter->second;

				//여기는 폴더 정보다
				std::wstring kFolderText(L"Folder : ");
				kFolderText += kFolderDir;
				kFolderText += std::wstring(L",\x09\x00");
				kFolderText += std::wstring(L" FILE_CNT = ");
				kFolderText += BM::vstring(kFolderState->kFileHash.size()).operator const std::wstring &();
				kFolderText += std::wstring(L", TYPE = ");
				kFolderText += std::wstring((kFolderState->bIsUsePack)?(L"BIND"):(L"UNBIND"));
				kFolderText += std::wstring(L"\x0D\x0A");
				WriteFile(hFile, kFolderText.c_str(), sizeof(std::wstring::value_type) * kFolderText.size(), &dwSize, NULL);

				CPFile::FILE_CITER file_iter = kFolderState->kFileHash.begin();
				while( file_iter != kFolderState->kFileHash.end() )
				{
					CPFile::FILE_DIR const& kFileDir = file_iter->first;
					CPFile::FILE_STATE const& kFileState = file_iter->second;

					wchar_t	szTemp[1024] = {0,};

					//여기는 파일 정보다
					std::wstring kFileText(L"\x09\x00");
					kFileText += std::wstring(L"File : ");
					kFileText += kFileDir;
					kFileText += std::wstring(L",\x09\x00");
					kFileText += std::wstring(L" ZIP = ");
					kFileText += std::wstring((kFileState.bIsZipped)?(L"1"):(L"0"));
					kFileText += std::wstring(L", ENC = ");
					kFileText += std::wstring((kFileState.bIsEncrypt)?(L"1"):(L"0"));
					kFileText += std::wstring(L", ORG_SIZE = ");
					kFileText += BM::vstring(kFileState.org_file_size).operator const std::wstring &();
					kFileText += std::wstring(L", CRC = ");
					kFileText += BM::vstring(kFileState.iOrgCRC).operator const std::wstring &();
					kFileText += std::wstring(L", SHA2 = ");
					kFileText += BM::vstring::ConvToHex(kFileState.ucSHA2CheckSum, PG_SHA2_LENGTH);
					
					kFileText += BM::vstring(kFileState.iOrgCRC).operator const std::wstring &();
					if( kFileState.bIsZipped )
					{
						kFileText += L", ZIP_SIZE = ";
						kFileText += BM::vstring(kFileState.zipped_file_size).operator const std::wstring &();
					}
					kFileText += L"\x0D\x0A";
					WriteFile(hFile, kFileText.c_str(), sizeof(std::wstring::value_type) * kFileText.size(), &dwSize, NULL);

					++file_iter;
				}
			
				++folder_iter;
			}
		}

		CloseHandle(hFile);
	}

	return bSuccess;
}

bool PackTask::CreatePatchID(std::wstring const& kOutFileDir, std::wstring& kAddonText)
{
	bool bSuccess = PackUtil::WritePatchID(kOutFileDir, m_PatchVersionInfo);
	GetPatchID(EVT_PATCH_VERSION, kAddonText);

	return bSuccess;
}

void PackTask::GetPatchID(E_VERSION_TYPE eVersionType, std::wstring& rkAddonText)
{
	BM::VersionInfo kVersionInfo;
	if( EVT_PATCH_VERSION == eVersionType )
	{
		kVersionInfo = m_PatchVersionInfo;
	}
	else
	{
		kVersionInfo = m_PrevVersionInfo;
	}

	BM::vstring kStr;
	kStr += kVersionInfo.Version.i16Major;
	kStr += L".";
	kStr += kVersionInfo.Version.i16Minor;
	kStr += L".";
	kStr += kVersionInfo.Version.i32Tiny;
	rkAddonText = kStr.operator const std::wstring &();
}

bool PackTask::UpdateDATVersionInfo(RCWSTR kOutFileDir, std::wstring& kAddonText)
{
	bool bSuccess = PackUtil::UpdateBindVersion(kOutFileDir, m_PatchVersionInfo);

	BM::vstring kStr;
	kStr += m_PatchVersionInfo.Version.i16Major;
	kStr += L".";
	kStr += m_PatchVersionInfo.Version.i16Minor;
	kStr += L".";
	kStr += m_PatchVersionInfo.Version.i32Tiny;
	kAddonText = kStr.operator const std::wstring &();

	return bSuccess;
}

bool PackTask::ConvertDatFile(RCWSTR kFile)
{
	bool bSuccess = BM::PgDataPackManager::ConvertToFormattedDataFile(kFile);
	return bSuccess;
}

bool PackTask::MakeManualPatch(RCWSTR kDestDir, RCWSTR kOutDir)
{
	// 1. 바인드 팩 진행
	if( !StartBind(kDestDir, kOutDir) )
	{
		return false;
	}

	// 2. 패치 컴바인 진행

	// patcher.exe 를 복사해서 대상 이름 x.x.x_x.x.x.exe 파일을 만들고..
	std::wstring kOldVersion, kNewVersion;
	GetPatchID(EVT_PREV_VERSION, kOldVersion);
	GetPatchID(EVT_PATCH_VERSION, kNewVersion);

	BM::vstring kNewFileName(kOutDir);
	kNewFileName += L"\\";
	kNewFileName += kOldVersion;
	kNewFileName += L"_";
	kNewFileName += kNewVersion;
	kNewFileName += L".exe";
	BM::vstring kPatcherFileName(L"..\\PatchExe.exe");
	std::vector<char> kFileData;
	if( !BM::FileToMem(kPatcherFileName, kFileData) )
	{
		return false;
	}
	if( !BM::MemToFile(kNewFileName, kFileData) )
	{
		return false;
	}

	// 만들어진 파일오픈..
	FILE* pOutFile = NULL;
	pOutFile = _tfopen(kNewFileName.operator const wchar_t *(), _T("r+b"));
	if( !pOutFile )
	{
		return false;
	}

	// 파일 에서 IMAGE_DOS_HEADER를 읽어준다
	DWORD dwAddr = (DWORD)pOutFile;
	IMAGE_DOS_HEADER kDosHdr;
	fread(&kDosHdr, sizeof(kDosHdr), 1, pOutFile);
	fseek(pOutFile, 0, SEEK_END);
	DWORD dwOffset = ftell(pOutFile);

	// BYTE pdnlen = wcslen(argv[5])*sizeof(wchar_t);
	// BYTE 넣고, 이전 버전 스트링(x.x.x) 넣는다.
	BYTE byLength= wcslen(kOldVersion.c_str())*sizeof(wchar_t);
	fwrite(&byLength, sizeof(byLength), 1, pOutFile);
	fwrite(kOldVersion.c_str(), byLength, 1, pOutFile);

	// BYTE pdnlen = wcslen(argv[5])*sizeof(wchar_t);
	// BYTE 넣고, 바뀔 버전 스트링(x.x.x) 넣는다.
	byLength = wcslen(kNewVersion.c_str())*sizeof(wchar_t);
	fwrite(&byLength, sizeof(byLength), 1, pOutFile);
	fwrite(kNewVersion.c_str(), byLength, 1, pOutFile);

	// log.txt 오픈해서.. 어쩌고하는데 안해도 댐..

	// 바인드팩 파일 (x.x.x.dat) 오픈
	BM::Stream kPackFileData;
	std::wstring kDestFileName(kDestDir);
	std::wstring kPackFileName(kOutDir);
	kPackFileName += L"\\";
	kPackFileName += BM::GetLastFolderName(kDestFileName);
	kPackFileName += L".dat";
	FILE* pPackFile = _tfopen(kPackFileName.c_str(), _T("r+b"));
	if( !pPackFile )
	{
		return false;
	}

	// 만들어질 파일에 바인드팩 파일을 뒤에 붙여넣는다
	IMAGE_DOS_HEADER kDosHdrPack;
	fread(&kDosHdrPack, sizeof(kDosHdrPack), 1, pPackFile);
	fseek(pPackFile, 0, SEEK_END);
	DWORD dwPackleng = ftell(pPackFile);
	fseek(pPackFile, 0, SEEK_SET);
	char* szPack = new char[dwPackleng];

	fread(szPack, dwPackleng, 1, pPackFile);
	fwrite(szPack, dwPackleng, 1, pOutFile);

	delete[] szPack;

	// 마지막으로 이걸 하는데.. 뭘 어쩌는건지 모르겠음..-_-
	IMAGE_NT_HEADERS32 pNtHdrs;
	DWORD dwPos = ((DWORD)&pNtHdrs.FileHeader.PointerToSymbolTable) - ((DWORD)&pNtHdrs);
	fseek(pOutFile, kDosHdr.e_lfanew+dwPos, SEEK_SET);
	fwrite(&dwOffset, sizeof(dwOffset), 1, pOutFile);
	
	fclose(pPackFile);
	fclose(pOutFile);

	return true;
}

bool PackTask::MakeAutoPatch(RCWSTR kDestDir, RCWSTR kDatFileDir, RCWSTR kOutDir, std::wstring& kAddonText)
{
	//패쳐 로직 변경으로 필요가 없어져서 막는다.
	return false;


	// 1.팩 작업 진행    
	if( !StartPack(kDestDir, kOutDir) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_PACK, ELM_FAIL); }
		return false;
	}

	// 2.패치 아이디 생성
	BM::vstring kFileDir(kOutDir);
	kFileDir += "\\PackResult";
	if( !CreatePatchID(kFileDir, kAddonText) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATE_ID, ELM_FAIL); }
		return false;
	}

	// 3.PATCH_VERSION 폴더에 파일리스트 생성
	BM::vstring kHeaderDir(kFileDir);
	kHeaderDir += "\\filelist.dat";
	std::vector<char> kHeaderFileData;
	if ( !BM::FileToMem(kHeaderDir, kHeaderFileData) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FAIL); }
		return false;
	}

	kHeaderDir = kFileDir;
	kHeaderDir += "\\patch_version\\";
	kHeaderDir += kAddonText;
	kHeaderDir += ".dat";
	if( !BM::MemToFile(kHeaderDir, kHeaderFileData) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FAIL); }
		return false;
	}

	// 4.파일리스트 머지
	kHeaderDir = kFileDir;
	kHeaderDir += "\\filelist.dat";
	if( !StartHeaderMerge(kHeaderDir, kDatFileDir, kFileDir) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_HEADERMERGE, ELM_FAIL); }
		return false;
	}

	return true;
}

bool PackTask::CheckDATVersion(RCWSTR kFile, std::wstring& kAddonText)
{
	BM::VersionInfo iVer;
	BM::vstring kStr;

	std::wstring kFileName = kFile;
	UPR(kFileName);
	if (kFileName.find(L".ID") != std::wstring::npos)
	{
		PackUtil::ReadPatchID(kFileName, iVer);
		kStr += L"Patch.ID version : ";
	}
	else if (kFileName.find(L".DAT") != std::wstring::npos)
	{
		BM::PgPackHeaderExtInfo kPackHeaderExtInfo;	
		BM::IFStream	kStream;

		kStream.Open(kFileName);
		if (false == kStream.IsOpen())
		{
			return false;
		}

		size_t TempSize = kPackHeaderExtInfo.ReadFromFile(kStream);
		if (TempSize <= 0)
		{
			return false;
		}

		iVer.iVersion = kPackHeaderExtInfo.i64GameVersion;		
		if (kPackHeaderExtInfo.bFormatted)
		{
			int a = 0, b = 0, c = 0, d = 0;
			a = (kPackHeaderExtInfo.iFormatVersion & 0xFF000000) >> 24;
			b = (kPackHeaderExtInfo.iFormatVersion & 0x00FF0000) >> 16;
			c = (kPackHeaderExtInfo.iFormatVersion & 0x0000FF00) >> 8;
			d = (kPackHeaderExtInfo.iFormatVersion & 0x000000FF);
			kStr += L"New format version(";
			kStr += a;
			kStr += L".";
			kStr += b;
			kStr += L".";
			kStr += c;
			kStr += L".";
			kStr += d;
			kStr += L") - Version in DAT : ";

		}
		else
		{
			kStr += L"Old format version - Version in DAT : ";
		}
	}
	else
	{
		return false;
	}

	kStr += iVer.Version.i16Major;
	kStr += L".";
	kStr += iVer.Version.i16Minor;
	kStr += L".";
	kStr += iVer.Version.i32Tiny;
	kAddonText = kStr.operator const std::wstring &();

	return true;
}

bool PackTask::RecursiveDiff(RCWSTR kDestDir, RCWSTR kSourDir, RCWSTR kOutDir, RCWSTR kSubDir)
{
	std::wstring kDestFolder = kDestDir + kSubDir;
	BM::AddFolderMark(kDestFolder);

	BM::FolderHash kDestFolderHash;
	BM::FolderHash kDestFileHash;

	if( !PackUtil::FolderSeek(kDestFolder, kSubDir, kDestFolderHash, kDestFileHash) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_DIFF, ELM_FOLDER_SEEK_FAIL, kDestFolder); }
		return false;
	}

	std::wstring kSourFolder = kSourDir + kSubDir;
	BM::AddFolderMark(kSourFolder);

	BM::FolderHash kSourFolderHash;
	BM::FolderHash kSourFileHash;

	if( !PackUtil::FolderSeek(kSourFolder, kSubDir, kSourFolderHash, kSourFileHash) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_DIFF, ELM_FOLDER_SEEK_FAIL, kSourFolder); }
		return false;
	}

	std::wstring kOutFolder = kOutDir + kSubDir;
	BM::AddFolderMark(kOutFolder);
	BM::ReserveFolder(kOutFolder);

	if( !kDestFileHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kDestFileHash.begin();
		BM::FolderHash::mapped_type const& kDestState = dest_iter->second;

		if( !kSourFileHash.empty() )
		{//검사
			BM::FolderHash::iterator sour_iter = kSourFileHash.begin();
			BM::FolderHash::mapped_type const& kSourState = sour_iter->second;

			PackUtil::DiffFileHash(kDestFolder, kOutFolder, kDestState->kFileHash, kSourState->kFileHash);
		}
		else
		{//복사
			PackUtil::CopyFiles(kDestFolder, kOutFolder, kDestState->kFileHash);
		}
	}

	if( !kDestFolderHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kDestFolderHash.begin();
		while( dest_iter != kDestFolderHash.end() )
		{
			BM::FolderHash::key_type const& kDestSubDir = dest_iter->first;
			BM::FolderHash::mapped_type const& kDestState = dest_iter->second;

			std::wstring kRstSubFolder = kSubDir + kDestSubDir;
			kRstSubFolder.erase(kRstSubFolder.find(_T(".\\")), 2);

			BM::FolderHash::iterator sour_iter = kSourFolderHash.find( kDestSubDir );
			if( sour_iter != kSourFolderHash.end() )
			{
				if( !RecursiveDiff(kDestDir, kSourDir, kOutDir, kRstSubFolder) )
				{//error
					return false;
				}
			}
			else
			{
				PackUtil::CopyNewFolder(kDestDir, kOutDir, kRstSubFolder);
			}
			++dest_iter;
		}
	}
	return true;
}

bool PackTask::RecursivePack(RCWSTR kTgtDir, RCWSTR kOutDir, RCWSTR kSubDir, bool const bIsPack)
{
	std::wstring kTgtFolder = kTgtDir + kSubDir;
	BM::AddFolderMark(kTgtFolder);

	BM::FolderHash kTgtFolderHash;
	BM::FolderHash kTgtFileHash;

	if( !PackUtil::FolderSeek(kTgtFolder, std::wstring(L".") + kSubDir, kTgtFolderHash, kTgtFileHash) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_PACK, ELM_FOLDER_SEEK_FAIL, kTgtFolder); }
		return false;
	}

	std::wstring kOutFolder = kOutDir + kSubDir;
	BM::AddFolderMark(kOutFolder);
	BM::ReserveFolder(kOutFolder);

	if( !kTgtFileHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kTgtFileHash.begin();
		BM::FolderHash::mapped_type& kTgtState = dest_iter->second;

		if( !PackUtil::PackFiles(kTgtFolder, kOutFolder, kTgtState->kFileHash) )
		{//error
			if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_PACK, ELM_FILE_TO_PACK_FAIL, kSubDir); }
			return false;
		}
		kTgtState->bIsUsePack = bIsPack;
		if( !PackUtil::CreateHashFile(kOutFolder + L"_files.pgh", kTgtFileHash) )
		{//error
			if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_PACK, ELM_FILE_LIST_CREATE_FAIL, kOutFolder + L"_files.pgh"); }
			return false;
		}
	}

	if( !kTgtFolderHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kTgtFolderHash.begin();
		while( dest_iter != kTgtFolderHash.end() )
		{
			BM::FolderHash::key_type const& kTgtSubDir = dest_iter->first;
			BM::FolderHash::mapped_type const& kTgtState = dest_iter->second;

			std::wstring kRstSubFolder = kSubDir + kTgtSubDir;
			kRstSubFolder.erase(kRstSubFolder.find(_T(".\\")), 2);

			if( !RecursivePack(kTgtDir, kOutDir, kRstSubFolder, kTgtState->bIsUsePack) )
			{//error
				return false;
			}
			++dest_iter;
		}
		PackUtil::CreateHashFile(kOutFolder + L"_folders.pgh", kTgtFolderHash);
	}
	return true;
}

bool PackTask::RecursiveCreateList(RCWSTR kTgtDir, RCWSTR kOutDir, RCWSTR kSubDir, bool const bIsPack)
{
	std::wstring kTgtFolder = kTgtDir + kSubDir;
	BM::AddFolderMark(kTgtFolder);

	BM::FolderHash kTgtFolderHash;
	BM::FolderHash kTgtFileHash;

	if( !PackUtil::FolderSeek(kTgtFolder, std::wstring(L".") + kSubDir, kTgtFolderHash, kTgtFileHash) )
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FOLDER_SEEK_FAIL, kTgtFolder); }
		return false;
	}

	std::wstring kOutFolder = kOutDir + kSubDir;
	BM::AddFolderMark(kOutFolder);
	BM::ReserveFolder(kOutFolder);

	if( !kTgtFileHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kTgtFileHash.begin();
		BM::FolderHash::mapped_type& kTgtState = dest_iter->second;

		if(!PackUtil::UpdateFileState(kTgtFolder, kTgtState->kFileHash))
		{//error
			if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FILE_STATE_UPDATE_FAIL, kSubDir); }
			return	false;
		}

		kTgtState->bIsUsePack = bIsPack;
		if(!PackUtil::CreateHashFile(kOutFolder + L"_files.pgh", kTgtFileHash))
		{//error
			if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FILE_LIST_CREATE_FAIL, kOutFolder + L"_files.pgh"); }
			return	false;
		}
	}

	if( !kTgtFolderHash.empty() )
	{
		BM::FolderHash::iterator dest_iter = kTgtFolderHash.begin();
		while( dest_iter != kTgtFolderHash.end() )
		{
			BM::FolderHash::key_type const& kTgtSubDir = dest_iter->first;
			BM::FolderHash::mapped_type const& kTgtState = dest_iter->second;

			std::wstring kRstSubFolder = kSubDir + kTgtSubDir;
			kRstSubFolder.erase(kRstSubFolder.find(_T(".\\")), 2);

			if( !RecursiveCreateList(kTgtDir, kOutDir, kRstSubFolder, kTgtState->bIsUsePack) )
			{//error
				return false;
			}
			++dest_iter;
		}
		PackUtil::CreateHashFile(kOutFolder + L"_folders.pgh", kTgtFolderHash);
	}
	return true;
}

bool PackTask::RecursiveCreateListMerge(RCWSTR kTgtDir, RCWSTR kFileName)
{
	::SetCurrentDirectory(kTgtDir.c_str());

	BM::FolderHash	TotalHash;
	PackUtil::GetList(L".\\", TotalHash);

	BM::PgDataPackManager::RecursiveProcessPackState(TotalHash);

	size_t iCount = TotalHash.size();

	if(!PackUtil::CreateHashFile(kFileName.c_str(), TotalHash))
	{
		if( m_pkLoger ){ m_pkLoger->LogMessage(EPT_CREATELIST, ELM_FILE_LIST_CREATE_FAIL, kFileName); }
		return	false;
	}
	return true;
}

bool PackTask::SetMainWorker(IOutPutLoger* pkLoger)
{
	if( !pkLoger )
	{
		return false;
	}

	m_pkLoger = pkLoger;
	return true;
}