#pragma once

#include "BM/BM.h"
#include "DataPack/PgDataPackManager.h"
#include "BM/FileSupport.h"

#ifdef	_DEBUG
	#pragma	comment	(lib, "DataPack_MDd.lib")
#else
	#pragma	comment	(lib, "DataPack_MDo.lib")
#endif


bool FolderToFolderDiff(const std::wstring& DestFolder, const std::wstring& SourFolder, 
						   const std::wstring& OutFolder, const std::wstring& SubFolder = L"\\");
bool FolderSeek(const std::wstring FolderName, const std::wstring SubFolder, 
				BM::FolderHash& FileList, BM::FolderHash& FolderList);
void DiffFile(const std::wstring& DestFolder, const std::wstring& OutFolder, 
			  const BM::FileHash& DestHash, const BM::FileHash& SourHash);
void CopyFiles(const std::wstring& DestFolder, const std::wstring& OutFolder,
			   const BM::FileHash& DestHash);
bool CopyNewFolder(const std::wstring& TargetFolder, const std::wstring& OutFolder, const std::wstring& SubFolder);
bool UpdateFileState(const std::wstring& TargetFolder, BM::FileHash& TargetHash);
void OutputFile(const std::wstring& TargetFolder, const std::wstring& OutFolder, 
				   const std::wstring& FileName);

bool BindCore(const std::wstring& OriFolder);