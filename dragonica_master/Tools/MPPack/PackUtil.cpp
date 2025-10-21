#include "StdAfx.h"
#include "PackUtil.h"
#include "SimpEnc/Common.h"
#include "SimpEnc/SimpEnc.h"
#include <shlwapi.h>
#include <shlobj.h>
#include <commdlg.h>

namespace PackUtil
{
	bool ReadPatchID(std::wstring const& kIDFileDir, BM::VersionInfo& kVersionInfo)
	{
		std::vector<char>	vecPatchID;
		BM::FileToMem(kIDFileDir, vecPatchID);
		BM::SimpleDecrypt(vecPatchID, vecPatchID);

		std::string kPatchID;
		kPatchID.resize(vecPatchID.size());
		::memcpy(&kPatchID.at(0), &vecPatchID.at(0), vecPatchID.size());

		typedef std::list< std::string > CUT_STR;
		CUT_STR	kList;

		BM::vstring::CutTextByKey<std::string>(kPatchID, ".", kList);

		if( kList.size() != 3 )
		{
			return false;
		}

		CUT_STR::iterator	iter = kList.begin();
		kVersionInfo.Version.i16Major = atoi(iter->c_str());	++iter;
		kVersionInfo.Version.i16Minor = atoi(iter->c_str());	++iter;
		kVersionInfo.Version.i32Tiny  = atoi(iter->c_str());

		return true;
	}

	bool WritePatchID(std::wstring const& kOutIDFileDir, BM::VersionInfo const& kVersionInfo)
	{
		std::string kPatchID;

		char szTemp[MAX_PATH] = {0,};
		_itoa_s(kVersionInfo.Version.i16Major, szTemp, MAX_PATH-1, 10);
		kPatchID += szTemp + std::string(".");
		_itoa_s(kVersionInfo.Version.i16Minor, szTemp, MAX_PATH-1, 10);
		kPatchID += szTemp + std::string(".");
		_itoa_s(kVersionInfo.Version.i32Tiny, szTemp, MAX_PATH-1, 10);
		kPatchID += szTemp;

		std::vector<char>	vecPatchID;
		vecPatchID.resize(kPatchID.size());
		::memcpy(&vecPatchID.at(0), kPatchID.c_str(), kPatchID.size());
		BM::SimpleEncrypt(vecPatchID, vecPatchID);

		std::wstring kRstOutFile = kOutIDFileDir;
		BM::AddFolderMark(kRstOutFile);
		kRstOutFile += L"patch.id";
		BM::MemToFile(kRstOutFile, vecPatchID);

		return true;
	}

	bool UpdateBindVersion(std::wstring const& kBinderFileDir, BM::VersionInfo const& kVersionInfo)
	{
		if( ::PathFileExistsW(kBinderFileDir.c_str()) )
		{
			return BM::PgDataPackManager::UpdatePackVersion(kBinderFileDir, kVersionInfo);
		}
		return true;
	}

	bool FolderSeek(std::wstring const& kSeekTgtDir, std::wstring const& kSubFolder, BM::FolderHash& kFolderHash, BM::FolderHash& kFileHash)
	{
		if( FALSE == ::SetCurrentDirectoryW(kSeekTgtDir.c_str()) )
		{
			return false;
		}

		BM::PgDataPackManager::GetFileList(L".\\", kFolderHash, BM::GFL_ONLY_FOLDER, kSubFolder);
		BM::PgDataPackManager::GetFileList(L".\\", kFileHash, BM::GFL_ONLY_FILE, kSubFolder);

		if( !kFileHash.empty() )
		{
			BM::FolderHash::iterator folder_iter = kFileHash.begin();
			BM::FolderHash::mapped_type& kFolderState = folder_iter->second;
			if( !UpdateFileState(kSeekTgtDir, kFolderState->kFileHash) )
			{
				return false;
			}
		}

		return true;
	}

	bool UpdateFileState(std::wstring const& kTgtFilesDir, BM::FileHash& kFileHash)
	{
		if( kFileHash.empty() )
		{
			return false;
		}

		BM::FileHash::iterator file_iter = kFileHash.begin();
		while( file_iter != kFileHash.end() )
		{
			BM::FileHash::key_type const& kFileDir = file_iter->first;
			BM::FileHash::mapped_type& kFileState = file_iter->second;

			std::vector<char>	kFileData;
			std::vector<char>	kZippedData;

			if( BM::FileToMem(kFileDir, kFileData) )
			{
				if( BM::Compress(kFileData, kZippedData) )
				{
					kFileState.bIsZipped = true;
					kFileState.zipped_file_size = kZippedData.size();
				}
				else
				{
					kFileState.bIsZipped = false;
					kFileState.zipped_file_size = kFileState.org_file_size;
				}
			}
			else
			{//error
				return false;
			}
			++file_iter;
		}
		return true;
	}

	void DiffFileHash(std::wstring const& kDestDir, std::wstring const& kOutDir, BM::FileHash const& kDestHash, BM::FileHash const& kSourHash)
	{
		if( !kDestHash.empty() )
		{
			BM::FileHash::const_iterator dest_iter = kDestHash.begin();
			while( dest_iter != kDestHash.end() )
			{
				BM::FileHash::key_type const& kDestSubDir = dest_iter->first;
				BM::FileHash::mapped_type const& kDestState = dest_iter->second;

				BM::FileHash::const_iterator sour_iter = kSourHash.find(kDestSubDir);
				if( sour_iter != kSourHash.end() )
				{
					BM::FileHash::mapped_type const& kSourState = sour_iter->second;
	
					if( kDestState != kSourState )
					{
						PackUtil::OutPutFile(kDestDir, kOutDir, kDestSubDir);
					}
				}
				else
				{
					PackUtil::OutPutFile(kDestDir, kOutDir, kDestSubDir);
				}
				++dest_iter;
			}
		}
	}

	void CopyFiles(std::wstring const& kDestDir, std::wstring const& kOutDir, BM::FileHash const& kFileHash)
	{
		BM::FileHash::const_iterator file_iter = kFileHash.begin();
		while( file_iter != kFileHash.end() )
		{
			BM::FileHash::key_type const& kFileDir = file_iter->first;
			PackUtil::OutPutFile(kDestDir, kOutDir, kFileDir);
			++file_iter;
		}
	}

	void OutPutFile(std::wstring const& kTgtFileDir, std::wstring const& kOutFileDir, std::wstring const& kFileName)
	{
		std::wstring kTgtFile = kTgtFileDir + kFileName;
		std::wstring kOutFile = kOutFileDir + kFileName;
		BM::ReserveFolder(kOutFile);
		::CopyFileW(kTgtFile.c_str(), kOutFile.c_str(), false);
	}

	bool CopyNewFolder(std::wstring const& kDestDir, std::wstring const& kOutDir, std::wstring const& kSubDir)
	{
		std::wstring kDestFolder = kDestDir + kSubDir;
		BM::AddFolderMark(kDestFolder);

		BM::FolderHash kDestFolderHash;
		BM::FolderHash kDestFileHash;

		if( !FolderSeek(kDestFolder, kSubDir, kDestFolderHash, kDestFileHash) )
		{
			return false;
		}

		std::wstring kOutFolder = kOutDir + kSubDir;
		BM::AddFolderMark(kOutFolder);

		if( !kDestFileHash.empty() )
		{
			BM::FolderHash::iterator dest_iter = kDestFileHash.begin();
			BM::FolderHash::mapped_type const& kDestState = dest_iter->second;

			CopyFiles(kDestFolder, kOutFolder, kDestState->kFileHash);
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
				CopyNewFolder(kDestDir, kOutDir, kRstSubFolder);
				++dest_iter;
			}
		}
		return true;
	}

	bool PackFiles(std::wstring const& kTgtDir, std::wstring const& kOutDir, BM::FileHash& kFileHash)
	{
		if(!kFileHash.empty())
		{
			BM::FileHash::iterator	file_iter = kFileHash.begin();
			while(file_iter != kFileHash.end())
			{
				BM::FileHash::key_type const& kFileDir = file_iter->first;
				BM::FileHash::mapped_type& kFileState = file_iter->second;

				std::vector<char>	kFileData;
				std::vector<char>	kZippedData;

				if(BM::FileToMem(kFileDir, kFileData))
				{
					if(BM::Compress(kFileData, kZippedData))
					{
						kFileState.bIsZipped = true;
						kFileState.zipped_file_size = kZippedData.size();	
					}
					else
					{
						kFileState.bIsZipped = false;
						kFileState.zipped_file_size = kFileState.org_file_size;
						kZippedData = kFileData;
					}

					if(kFileState.bIsEncrypt)
					{
						std::fstream ffile;
						std::wstring const& strFileName = kOutDir + kFileDir + _T(".zip");
						ffile.open( strFileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::ate | std::ios_base::binary );//in 모드임.
						if(!ffile.is_open())
						{
							return false;
						}
						else
						{
							std::vector< char > vecEncryptInfo;

							BM::EncStream(kZippedData, vecEncryptInfo);
							
							std::streamoff write_offset = 0;
							ffile.seekp(write_offset);
							ffile.write(&vecEncryptInfo.at(0), vecEncryptInfo.size());
							write_offset += vecEncryptInfo.size();

							ffile.seekp(write_offset);
							ffile.write(&kZippedData.at(0), kZippedData.size());
							write_offset += kZippedData.size();

							ffile.close();
						}
					}
					else
					{
						BM::MemToFile(kOutDir + kFileDir + _T(".zip"), kZippedData);
					}
				}
				else
				{
					return	false;
				}
				++file_iter;
			}
		}
		return true;
	}

	bool CreateHashFile(std::wstring const& kOutDir, BM::FolderHash& kFolderHash)
	{
		std::fstream ffile_ret;
		ffile_ret.open(kOutDir.c_str(), std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);

		//	파일정보 저장
		if(!BM::PgDataPackManager::MakeHeader(kFolderHash, ffile_ret))
		{
			ffile_ret.close();
			return	false;
		}
		ffile_ret.close();
		return	true;
	}

	bool GetList(std::wstring const& kTgtDir, BM::FolderHash& kFolderHash)
	{
		BM::FolderHash		FileHash;
		BM::FolderHash		FolderHash;

		ReadList(kTgtDir, FileHash, L"_files.pgh");
		ReadList(kTgtDir, FolderHash, L"_folders.pgh");

		std::wstring lwrTgtDir = kTgtDir;
		LWR(lwrTgtDir);
		BM::FolderHash::iterator file_iter = FileHash.begin();
		while(file_iter != FileHash.end())
		{
			kFolderHash.insert(std::make_pair(lwrTgtDir, file_iter->second));
			++file_iter;
		}

		BM::FolderHash::iterator folder_iter = FolderHash.begin();
		while(folder_iter != FolderHash.end())
		{
			std::wstring	wstrSubFolder = kTgtDir;
			wstrSubFolder	+= BM::GetLastFolderName((*folder_iter).first);
			BM::AddFolderMark(wstrSubFolder);

			BM::FolderHash	SubFolderHash;
			GetList(wstrSubFolder, SubFolderHash);

			kFolderHash.insert(SubFolderHash.begin(), SubFolderHash.end());
			kFolderHash.insert(std::make_pair(wstrSubFolder, (*folder_iter).second));

			++folder_iter;
		}

		const std::wstring	FileList = kTgtDir + L"_files.pgh";
		const std::wstring	FolderList = kTgtDir + L"_folders.pgh";
		DeleteFile(FileList.c_str());
		DeleteFile(FolderList.c_str());

		return	true;
	}

	void ReadList(std::wstring const& kTgtDir, BM::FolderHash& OutHash, std::wstring const& kFileName)
	{
		BM::PgDataPackManager::PackFileCache_new kCache;
		if(kCache.Open(kTgtDir + kFileName))
		{
			BM::PgDataPackManager::ReadHeader(kCache);
			OutHash.insert(kCache.mFolderHash.begin(),kCache.mFolderHash.end());
		}
	}

	void OpenSelectDialog(E_OPEN_DLG_TYPE const Type, HWND hWnd, DWORD const DLG_CTL_ID, std::wstring const FileID)
	{
		std::wstring kSelectDir;
		
		switch( Type )
		{
		case EODT_FOLDER_SELECT:{ kSelectDir = SelectFolder(hWnd);		}break;
		case EODT_FILE_SELECT:	{ kSelectDir = SelectFile(hWnd, FileID);}break;
		default:
			{
				return;
			}
		}

		if( !kSelectDir.empty() )
		{
			::SetWindowTextW(GetDlgItem(hWnd, DLG_CTL_ID), kSelectDir.c_str());
		}
	}

	std::wstring const SelectFolder(HWND hWnd)
	{
		wchar_t szTemp[1024] = {0,};

		BROWSEINFOW			bi;

		bi.hwndOwner		= hWnd;
		bi.pidlRoot			= NULL;
		bi.pszDisplayName	= NULL;
		bi.lpszTitle		= L"Select Target Directory";
		bi.ulFlags			= 0;
		bi.lpfn				= NULL;
		bi.lParam			= 0;
		
		LPITEMIDLIST pidl	= ::SHBrowseForFolderW(&bi);
		if(pidl != NULL)
		{
			if( TRUE == ::SHGetPathFromIDListW(pidl, szTemp) )
			{
				return std::wstring(szTemp);
			}
		}
		return std::wstring();
	}

	std::wstring const SelectFile(HWND hWnd, std::wstring const FileID)
	{
		TCHAR	szTemp[1024] = {0,};

		std::wstring kFilter = L".";
		kFilter += FileID;
		kFilter += L" Files";
		kFilter.push_back(L'\0');
		kFilter += L"*.";
		kFilter += FileID;
		kFilter.push_back(L'\0');

		OPENFILENAMEW	OFN;
		memset(&OFN, 0, sizeof(OPENFILENAME));
		OFN.lStructSize = sizeof(OPENFILENAME);
		OFN.hwndOwner	= hWnd;
		OFN.lpstrFilter = kFilter.c_str();
		OFN.lpstrFile	= szTemp;
		OFN.Flags		= OFN_EXPLORER;
		OFN.nMaxFile	= 255;

		if( ::GetOpenFileNameW(&OFN) == TRUE )
		{	
			return std::wstring(szTemp);
		}

		return std::wstring();
	}
	bool IsFileExist(std::wstring const &rkFileName)
	{
		DWORD dwFileAttr;

		dwFileAttr = GetFileAttributes(rkFileName.c_str());
		if (INVALID_FILE_ATTRIBUTES == dwFileAttr)
			return false;

		return true;
	}
}