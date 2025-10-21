// MakeDiffPack.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "defines.h"

void ShowHelp()
{
     _tprintf(_T("Usage: MakeDiffPack OldVersionFolder NewVersionFolder OutputFolder\n"));
}


int _tmain(int argc, _TCHAR* argv[])
{
	BM::g_bCalcSHA2CheckSum = true;
	if(argc!=4)
	{
		ShowHelp();
		return(-1);
	}
	std::wstring strOldPath = argv[1];
	std::wstring strNewPath = argv[2];
	std::wstring strOutPath = argv[3];

	if(!strOldPath.size())
	{
		_ftprintf(stderr, _T("OldVersionPath fail.\n"));
		return(-1);
	}

	if(!strNewPath.size())
	{
		_ftprintf(stderr, _T("NewVersionPath fail.\n"));
		return(-1);
	}

	if(!strOutPath.size())
	{
		_ftprintf(stderr, _T("OutputFolder fail.\n"));
		return(-1);
	}

	_tprintf(_T("Please wait..\n"));

	//	팩 옵션을 읽고
	BM::CPackInfo	kPackInfo;
	if(!kPackInfo.Init())
	{
		_ftprintf(stderr, _T("PackInfo fail.\n"));
		return(-1);
	}
	//	매니져를 초기화
	if(!BM::PgDataPackManager::Init(kPackInfo))
	{
		_ftprintf(stderr, _T("PackMgr fail.\n"));
		return(-1);
	}

	bool bRtn = FolderToFolderDiff(strNewPath, strOldPath, strOutPath + L"\\Data");
	if(!bRtn)
	{
		_ftprintf(stderr, _T("Diff fail.\n"));
		return(-1);
	}

	bRtn = BindCore(strOutPath + L"\\Data");
	if(!bRtn)
	{
		_ftprintf(stderr, _T("Pack fail.\n"));
		return(-1);
	}

	_tprintf(_T("Complete.\n"));

	return 0;
}


//	폴더와 폴더
bool FolderToFolderDiff(const std::wstring& DestFolder, const std::wstring& SourFolder, 
						const std::wstring& OutFolder, const std::wstring& SubFolder)
{
	//	수정폴더의 데이타
	std::wstring	wstrDestDir	= DestFolder + SubFolder;
	BM::FolderHash	kDestFolderHash;
	BM::FolderHash	kDestFileHash;

	BM::AddFolderMark(wstrDestDir);
	if(!FolderSeek(wstrDestDir, SubFolder, kDestFileHash, kDestFolderHash))
	{
		return	false;
	}

	//	원본폴더의 데이타
	std::wstring	wstrSourDir	= SourFolder + SubFolder;
	BM::FolderHash	kSourFolderHash;
	BM::FolderHash	kSourFileHash;

	BM::AddFolderMark(wstrSourDir);
	if(!FolderSeek(wstrSourDir, SubFolder, kSourFileHash, kSourFolderHash))
	{
		return	false;
	}

	//	결과 출력폴더
	std::wstring	wstrOutFolder = OutFolder + SubFolder;
	BM::AddFolderMark(wstrOutFolder);

	//	검사전에 CRC
	if(kDestFileHash.size())
	{
		if(kSourFileHash.size())
		{
			//	파일 검사
			DiffFile(wstrDestDir, wstrOutFolder, 
					 kDestFileHash.begin()->second->kFileHash,
					 kSourFileHash.begin()->second->kFileHash);
		}
		else
		{
			//	파일 복사
			CopyFiles(wstrDestDir, wstrOutFolder, kDestFileHash.begin()->second->kFileHash);
		}
	}

	//	하위 폴더 검사
	if(kDestFolderHash.size())
	{
		BM::FolderHash::iterator	Dest_iter = kDestFolderHash.begin();
		while(Dest_iter != kDestFolderHash.end())
		{
						BM::FolderHash::iterator	Sour_iter = kSourFolderHash.find((*Dest_iter).first);
			std::wstring	wstrSubFolder = SubFolder + (*Dest_iter).first;
			wstrSubFolder.erase(wstrSubFolder.find(_T(".\\")), 2);

			if(Sour_iter == kSourFolderHash.end())
			{
				CopyNewFolder(DestFolder, OutFolder, wstrSubFolder);
			}
			else
			{
				const bool bResult = FolderToFolderDiff(DestFolder, SourFolder, OutFolder, wstrSubFolder);
				if(bResult == false)
				{
					return	false;
				}
			}

			++Dest_iter;
		}
	}

	return	true;
}

bool FolderSeek(const std::wstring FolderName, const std::wstring SubFolder, 
				BM::FolderHash& FileList, BM::FolderHash& FolderList)
{
	if(::SetCurrentDirectory(FolderName.c_str()) == FALSE)
	{
		return	false;
	}

	BM::PgDataPackManager::GetFileList(_T(".\\"), FolderList, BM::GFL_ONLY_FOLDER, SubFolder);
	BM::PgDataPackManager::GetFileList(_T(".\\"), FileList, BM::GFL_ONLY_FILE, SubFolder);

	if(FileList.size())
	{
		//	파일 정보 갱신
		if(!UpdateFileState(FolderName, FileList.begin()->second->kFileHash))
		{
			return	false;
		}
	}
	
	return	true;
}

//	파일을 비교
void DiffFile(const std::wstring& DestFolder, const std::wstring& OutFolder, 
			  const BM::FileHash& DestHash, const BM::FileHash& SourHash)
{
	if(DestHash.size())
	{
		//	비교 시작
		BM::FileHash::const_iterator	Dest_iter = DestHash.begin();
		while(Dest_iter != DestHash.end())
		{
			if(SourHash.size())
			{
				//	검색
				BM::FileHash::const_iterator	Sour_iter = SourHash.find((*Dest_iter).first);

				if(Sour_iter == SourHash.end())
				{
					//	파일이 없는데 추가되었나.
					OutputFile(DestFolder, OutFolder, (*Dest_iter).first);
				}
				else
				{
					//	파일이 있는데 크기가 다르네.
					if((*Dest_iter).second != (*Sour_iter).second)
					{
						OutputFile(DestFolder, OutFolder, (*Dest_iter).first);
					}
				}
			}
			else
			{
				OutputFile(DestFolder, OutFolder, (*Dest_iter).first);
			}

			++Dest_iter;
		}
	}
	else
	{
	}
}

//	파일들 복사
void CopyFiles(const std::wstring& DestFolder, const std::wstring& OutFolder,
			   const BM::FileHash& DestHash)
{
	//	복사 시작
	BM::FileHash::const_iterator	Dest_iter = DestHash.begin();
	while(Dest_iter != DestHash.end())
	{
		//	파일을 복사하자
		OutputFile(DestFolder, OutFolder, (*Dest_iter).first);
		++Dest_iter;
	}
}

//	새로운 폴더 복사
bool CopyNewFolder(const std::wstring& TargetFolder, const std::wstring& OutFolder, const std::wstring& SubFolder)
{
	//	타겟 폴더
	std::wstring	wstrTargetDir	= TargetFolder + SubFolder;
	BM::FolderHash	kTargetFolderHash;
	BM::FolderHash	kTargetFileHash;

	BM::AddFolderMark(wstrTargetDir);
	if(!FolderSeek(wstrTargetDir, SubFolder, kTargetFileHash, kTargetFolderHash))
	{
		return	false;
	}

	//	출력 폴더 생성
	std::wstring	wstrOutFolder = OutFolder + SubFolder;
	BM::AddFolderMark(wstrOutFolder);
	BM::ReserveFolder(wstrOutFolder);

	//	파일이 있으면 복사
	if(kTargetFileHash.size())
	{
		CopyFiles(wstrTargetDir, wstrOutFolder, kTargetFileHash.begin()->second->kFileHash);
	}


	//	하위 폴더가 있으면 복사
	if(kTargetFolderHash.size())
	{
		BM::FolderHash::iterator	Target_iter = kTargetFolderHash.begin();
		while(Target_iter != kTargetFolderHash.end())
		{
			std::wstring	wstrSubFolder	= SubFolder + (*Target_iter).first;
			wstrSubFolder.erase(wstrSubFolder.find(_T(".\\")), 2);
			CopyNewFolder(TargetFolder, OutFolder, wstrSubFolder);
			++Target_iter;
		}
	}

	return	true;
}

bool UpdateFileState(const std::wstring& TargetFolder, BM::FileHash& TargetHash)
{
	//	넘어온 파일 해쉬에 파일이 있을 경우에만.
	if(TargetHash.size())
	{
		BM::FileHash::iterator	File_iter = TargetHash.begin();

		//	파일 팩
		while(File_iter != TargetHash.end())
		{
			std::vector<char>	kFileData;
			std::vector<char>	kZippedData;

			//	파일 로드
			if(BM::FileToMem((*File_iter).first, kFileData))
			{
				//	압축
				if(BM::Compress(kFileData, kZippedData))
				{
					(*File_iter).second.bIsZipped = true;
					(*File_iter).second.zipped_file_size = kZippedData.size();
				}
				else
				{
					(*File_iter).second.bIsZipped = false;
					(*File_iter).second.zipped_file_size = (*File_iter).second.org_file_size;
				}
			}
			else
			{
				//	실패
				return	false;
			}

			++File_iter;
		}
	}
	else
	{
	}

	return	true;
}

//	파일 복사
void OutputFile(const std::wstring& TargetFolder, const std::wstring& OutFolder, 
			    const std::wstring& FileName)
{
	std::wstring	TargetFile = TargetFolder + FileName;
	std::wstring	OutFile	 = OutFolder + FileName;
	BM::ReserveFolder(OutFile);
	CopyFile(TargetFile.c_str(), OutFile.c_str(), false);
}

bool BindCore(const std::wstring& OriFolder)
{
	std::wstring	ResultFileName = L"";
	ResultFileName = BM::GetLastFolderName(OriFolder);
	ResultFileName += L".DAT";
	
	if(!BM::PgDataPackManager::PackTargetFolder(OriFolder.c_str(), ResultFileName))
	{
		return	false;
	}

	return	true;
}