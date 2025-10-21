#pragma once

#include "DataPack/PgDataPackManager.h"

namespace PackUtil
{
	bool ReadPatchID(std::wstring const& kIDFileDir, BM::VersionInfo& kVersionInfo);
	bool WritePatchID(std::wstring const& kOutIDFileDir, BM::VersionInfo const& kVersionInfo);
	bool UpdateBindVersion(std::wstring const& kBinderFileDir, BM::VersionInfo const& kVersionInfo);

	bool FolderSeek(std::wstring const& kSeekTgtDir, std::wstring const& kSubFolder, BM::FolderHash& kFolderHash, BM::FolderHash& kFileHash);
	bool UpdateFileState(std::wstring const& kTgtFilesDir, BM::FileHash& kFileHash);
	void DiffFileHash(std::wstring const& kDestDir, std::wstring const& kOutDir, BM::FileHash const& kDestHash, BM::FileHash const& kSourHash);
	void CopyFiles(std::wstring const& kDestDir, std::wstring const& kOutDir, BM::FileHash const& kFileHash);
	void OutPutFile(std::wstring const& kTgtFileDir, std::wstring const& kOutFileDir, std::wstring const& kFileName);
	bool CopyNewFolder(std::wstring const& kDestDir, std::wstring const& kOutDir, std::wstring const& kSubDir);

	bool PackFiles(std::wstring const& kTgtDir, std::wstring const& kOutDir, BM::FileHash& kFileHash);
	bool CreateHashFile(std::wstring const& kOutDir, BM::FolderHash& kFolderHash);

	bool GetList(std::wstring const& kTgtDir, BM::FolderHash& kFolderHash);
	void ReadList(std::wstring const& kTgtDir, BM::FolderHash& OutHash, std::wstring const& kFileName);

	enum E_OPEN_DLG_TYPE
	{
		EODT_FOLDER_SELECT	= 0,
		EODT_FILE_SELECT	= 1,
	};

	void OpenSelectDialog(E_OPEN_DLG_TYPE const Type, HWND hWnd, DWORD const DLG_CTL_ID, std::wstring const FileID);
	std::wstring const SelectFolder(HWND hWnd);
	std::wstring const SelectFile(HWND hWnd, std::wstring const FileID);

};
