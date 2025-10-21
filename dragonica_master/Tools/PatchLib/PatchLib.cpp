#include "stdafx.h"
#include "PatchLib.h"

float PatchLib::m_fFilePercent = 0.0f;
float PatchLib::m_fTotalPercent = 0.0f;
PFCALLBACK PatchLib::m_pStateCallback = NULL;
//PFCALLBACK CPatch::m_pStateCallBackFunc = NULL;

PatchLib::PatchLib() 
 : m_nPatchHeaderSize(0), m_TotalSize(0), m_FileSize(0), m_CntFileSize(0), m_CntTotalSize(0)
{

}
PatchLib::~PatchLib()
{

}

void PatchLib::Reset()
{
	m_nPatchHeaderSize = 0; 
	m_TotalSize = 0;
	m_FileSize = 0; 
	m_CntFileSize = 0; 
	m_CntTotalSize = 0;
}

std::wstring PatchLib::GetPatchID(std::wstring const& strFileName)
{
	std::vector<char>	vec_PatchID;	//암호화된 ID
	BM::FileToMem(strFileName, vec_PatchID);

	if(vec_PatchID.size())
	{
		BM::SimpleDecrypt(vec_PatchID, vec_PatchID);
		vec_PatchID.push_back('\0');

		char*	pIDSrc	= new char[vec_PatchID.size()];
		memcpy(pIDSrc, &vec_PatchID.at(0), vec_PatchID.size());
		std::wstring	wstrID = UNI(pIDSrc);
		delete [] pIDSrc;

		return	wstrID;
	}

	return	L"";
}

PATCH_RESULT PatchLib::ClientVersionCheck(std::wstring const& strOldVersion, std::wstring const& strNewVersion)
{
	// Patch.ID 체크
	std::wstring strPatchID;
	if(_access("PATCH.ID", 0) == -1)
	{
		//MessageBoxW(g_hWnd, TEXT_HAVE_NOT_CLIENT, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
		OutputDebugStringW(L"can not find patch.id.\n");
		return PR_NOT_FIND_PATCH_ID;
	}
	else
	{
		strPatchID = GetPatchID(_T("PATCH.ID"));
		if(!strPatchID.size())
		{
			//MessageBoxW(g_hWnd, TEXT_HAVE_NOT_CLIENT, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
			OutputDebugStringW(L"can not read patch.id.\n");
			return PR_NOT_READ_PATCH_ID;
		}
	}

	if(!strPatchID.compare(strNewVersion))
	{
		//MessageBoxW(g_hWnd, TEXT_ALREADY_PATCHED, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONINFORMATION|MB_SETFOREGROUND);
		OutputDebugStringW(L"already patched.\n");
		return PR_ALREADY_PATCHED;
	}

	if(strPatchID.compare(strOldVersion))
	{
		//MessageBoxW(g_hWnd, TEXT_DIFFERENT_VERSION, TEXT_PATCH, MB_OK|MB_SYSTEMMODAL|MB_ICONERROR|MB_SETFOREGROUND);
		OutputDebugStringW(L"different version.\n");
		return PR_DIFFERENT_VERSION;
	}

	return PR_OK;
}

void PatchLib::DecFile(std::vector<char>& vecData)
{
	std::vector<char>	vecSrc;
	vecSrc.resize(vecData.size());
	memcpy(&vecSrc.at(0), &vecData.at(0), vecData.size());
	BM::DecLoadFromMem(vecSrc, vecData);
}
void PatchLib::UnCompFile(std::vector<char>& vecData, size_t const& OriginalSize)
{
	std::vector<char>	vecSrc;
	vecSrc.resize(OriginalSize);
	BM::UnCompress(vecSrc, vecData);
	vecData = vecSrc;
}

PATCH_RESULT PatchLib::AmendNonPackUpdate(const BM::FolderHash& PatchList, char* szData)
{
	//	리스트가 있을때만 해야겠지
	if(PatchList.size())
	{
		char* szData2 = szData;

		//	폴더를 먼저 선택하고
		BM::FolderHash::const_iterator	Folder_iter = PatchList.begin();
		while(Folder_iter != PatchList.end())
		{
			//	파일이 있다면
			if((*Folder_iter).second->kFileHash.size())
			{
				//	돌자
				BM::FileHash::const_iterator	File_iter = (*Folder_iter).second->kFileHash.begin();
				while(File_iter != (*Folder_iter).second->kFileHash.end())
				{
					//if(m_bThreadEnd)
					//{
					//	return false;
					//}
					if((*File_iter).first == _T("PATCH.ID"))
					{
						++File_iter;
						continue;
					}
					char*	pPatchData = szData2 + m_nPatchHeaderSize + (*File_iter).second.offset;
					size_t	pDataSize = 0;
					std::vector<char>	vecData;

					//	사이즈 알아보자 - 압축?
					if((*File_iter).second.bIsZipped)
					{
						pDataSize = (*File_iter).second.zipped_file_size;
					}
					else
					{
						pDataSize = (*File_iter).second.org_file_size;
					}

					m_strFileName = (*File_iter).first;
					SetFileCnt(pDataSize);

					//	사이즈 알아보자 - 암호화?
					if((*File_iter).second.bIsEncrypt)
					{
						pDataSize += BM::ENC_HEADER_SIZE;
					}

					//	파일을 메모리에 복사하자
					vecData.resize(pDataSize);
					::memcpy(&vecData.at(0), pPatchData, pDataSize);

					//	암호화되있음 푼다
					if((*File_iter).second.bIsEncrypt)
					{
						DecFile(vecData);
					}

					//	압축되있음 푼다
					if((*File_iter).second.bIsZipped)
					{
						UnCompFile(vecData, (*File_iter).second.org_file_size);
					}

					//	해당 디렉토리로 파일을 옮긴다
					std::wstring	wstrOrgFileName = (*Folder_iter).first + (*File_iter).first;
					BM::ReserveFolder((*Folder_iter).first);
					DeleteFile(wstrOrgFileName.c_str());
					BM::MemToFile(wstrOrgFileName, vecData, false);
					//BM::MemToFileNotReserveFolder(wstrOrgFileName, vecData);

					++File_iter;
				}
			}

			++Folder_iter;
		}
	}

	return	PR_OK;
}

PATCH_RESULT PatchLib::MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList)
{
	//	둘중하나라도 비면 할필요가 있나?
	if(PatchList.size() || OrgList.size())
	{
		// 원본을 리설트에 넣고.
		// SmartPtr이어서 그냥 집어 넣으면 안되고 복사를 해서 넣어줘야 한다. 아래는 예전코드
		//kResultList.insert(OrgList.begin(), OrgList.end());	
		BM::FolderHash::iterator org_iter = OrgList.begin();
		while (org_iter != OrgList.end())
		{
			BM::PgFolderInfo* kNewInfo = BM::PgFolderInfo::New();
			*kNewInfo = *((*org_iter).second); // make a deepcopy
			kResultList.insert(std::make_pair((*org_iter).first, BM::PgFolderInfoPtr(kNewInfo)));
			++org_iter;
		}

		//	폴더를 돌자
		BM::FolderHash::const_iterator	Patch_iter = PatchList.begin();
		while(Patch_iter != PatchList.end())
		{
			//	폴더를 찾아라
			BM::FolderHash::iterator	Result_iter = kResultList.find((*Patch_iter).first);

			//	찾았냐?
			if(Result_iter != kResultList.end())
			{
				//	파일을 검사하자.
				BM::FileHash::const_iterator	Patch_file_iter = (*Patch_iter).second->kFileHash.begin();
				while(Patch_file_iter != (*Patch_iter).second->kFileHash.end())
				{
					//	파일을 찾아라
					BM::FileHash::iterator	Result_file_iter = (*Result_iter).second->kFileHash.find((*Patch_file_iter).first);
					//	찾았냐?
					if(Result_file_iter != (*Result_iter).second->kFileHash.end())
					{
						//	갱신 및 삭제
						(*Result_file_iter).second = (*Patch_file_iter).second;

					}
					else
					{
						//	추가
						(*Result_iter).second->kFileHash.insert(std::make_pair((*Patch_file_iter).first, (*Patch_file_iter).second));
					}

					++Patch_file_iter;
				}
			}
			else
			{
				//	추가
				kResultList.insert(std::make_pair((*Patch_iter).first, (*Patch_iter).second));
			}

			++Patch_iter;
		}

		return	PR_OK;
	}

	return	PR_PATCH_DATA_EMPTY;
}
size_t PatchLib::GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value)
{
	if(!List.size())
	{
		return	0;
	}

	size_t	TotalSize = 0;

	BM::FolderHash::const_iterator	iter = List.begin();
	while(iter != List.end())
	{
		if((*iter).second->kFileHash.size())
		{
			BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
			while(File_iter != (*iter).second->kFileHash.end())
			{
				if((*File_iter).second.bIsZipped)
				{
					TotalSize += (*File_iter).second.zipped_file_size;
				}
				else
				{
					TotalSize += (*File_iter).second.org_file_size;
				}

				if(Value != ERT_DOWNLOAD)
				{
					if((*File_iter).second.bIsEncrypt)
					{
						TotalSize += BM::ENC_HEADER_SIZE;
					}
				}

				++File_iter;
			}
		}

		++iter;
	}

	return	TotalSize;
}
PATCH_RESULT PatchLib::AmendPatchToOrgMerge(const std::wstring& TargetDir, const BM::FolderHash& kPatchHash, char* szData)
{
	//	원본 파일
	//PgFileMapping	kOrgFile;
	BM::FolderHash	kOrgHash;
	std::wstring	wstrFileName = TargetDir;
	BM::DetachFolderMark(wstrFileName);
	wstrFileName += L".DAT";
	if(_access(MB(wstrFileName), 0) == -1)
	{
		BM::ReserveFolder(wstrFileName);
		CopyFile(L".\\Patch.TMP", wstrFileName.c_str(), false);
		return	PR_OK;
	}

	std::wstring	wstrNewName;

	{
		//이전 포맷 이라면 컨버팅부터 하고 작업한다
		BM::PgDataPackManager::ConvertToFormattedDataFile(wstrFileName);

		BM::PgDataPackManager::PackFileCache_new kOrgCache;
		if(!kOrgCache.Open(wstrFileName))
		{
			return PR_NOT_OPEN_PACK_FILE;
		}
		const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(kOrgCache);
		kOrgHash = kOrgCache.mFolderHash;
		//const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(wstrFileName, kOrgHash);
		//if(kOrgFile.Init(wstrFileName) != S_OK)
		//{
		//	return false;
		//}

		//	패치 파일
		char* szData2 = szData;

		//	원본에서 패치된 파일을 업데이트합니다.
		//	결합 파일
		BM::FolderHash	kNewHash;	

		//	리스트 생성 및 수정
		PATCH_RESULT kRtn = MergeList(kPatchHash, kOrgHash, kNewHash);
		if(PR_OK != kRtn)
		{
			return kRtn;
		}

		size_t TotalSize = GetListDataSize(kNewHash);
		SetFileSize(TotalSize);
		//	결과 파일 생성
		//PgFileMapping	kNewFile;
		wstrNewName = TargetDir;
		BM::DetachFolderMark(wstrNewName);
		wstrNewName += L"_NEW.DAT";
		const unsigned __int64	NewDataSize	= BM::PgDataPackManager::BuildOffset(kNewHash);
		std::fstream ffile;
		ffile.open(wstrNewName.c_str(), std::ios_base::in | std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);

		if(!ffile.is_open())
		{
			return PR_NOT_CREATE_NEW_PACK_FILE;
		}
		const size_t	NewHeaderSize = BM::PgDataPackManager::MakeHeader(kNewHash, ffile, true);
		//const size_t	NewHeaderSize = BM::PgDataPackManager::MakeHeader(wstrNewName, kNewHash, kNewFile, true);
		//if(kNewFile.Init(wstrNewName, OPEN_EXISTING, NewHeaderSize + NewDataSize) != S_OK)
		//{
		//	return false;
		//}

		//	결과대로 묶자
		BM::FolderHash::iterator	New_iter = kNewHash.begin();
		while(New_iter != kNewHash.end())
		{
			//	폴더를 찾아
			BM::FolderHash::const_iterator	Patch_iter = kPatchHash.find((*New_iter).first);
			BM::FolderHash::iterator	Org_iter = kOrgHash.find((*New_iter).first);

			//	파일을 돌려
			BM::FileHash::iterator	New_file_iter = (*New_iter).second->kFileHash.begin();
			while(New_file_iter != (*New_iter).second->kFileHash.end())
			{
				//if(m_bThreadEnd)
				//{
				//	m_wstrTextState = wstrNewName;
				//	//BOOL bRtn = DeleteFile(wstrNewName.c_str());
				//	return false;
				//}
				const BM::PgFileInfo	*pFileInfo = NULL;
				char*	pData = NULL;
				bool bFind = false;
				size_t	pDataSize = 0;
				bool	bInPatch = true;
				//char*	pNewData = kNewFile.Data() + NewHeaderSize + (*New_file_iter).second.offset;
				ffile.seekg(NewHeaderSize + (*New_file_iter).second.offset);

				//	검사 있냐?
				if(Patch_iter == kPatchHash.end())
				{
					bInPatch = false;
				}
				else
				{
					//	있네. 찾아라
					if((*Patch_iter).second->Find((*New_file_iter).first, pFileInfo))
					{
						pData = szData2 + m_nPatchHeaderSize + pFileInfo->offset;
						bFind = true;
					}
					else
					{
						bInPatch = false;
					}
				}

				//	없다.
				if(bInPatch == false)
				{
					//	오리지날에서 찾자
					if(Org_iter == kOrgHash.end())
					{
						return	PR_NOT_FIND_FILE_IN_PACK;
					}
					else
					{
						//	있다. 찾아라
						if((*Org_iter).second->Find((*New_file_iter).first, pFileInfo))
						{
							//pData = kOrgFile.Data() + OrgHeaderSize + pFileInfo->offset;
							bFind = true;
						}
						else
						{
							//	없다 에러
							return	PR_NOT_FIND_FILE_IN_PACK;
						}
					}
				}

				//	사이즈 알아보자 - 압축?
				if(pFileInfo->bIsZipped)
				{
					pDataSize = pFileInfo->zipped_file_size;
				}
				else
				{
					pDataSize = pFileInfo->org_file_size;
				}

				m_strFileName = (*New_file_iter).first;
				SetFileCnt(pDataSize);


				//	사이즈 알아보자 - 암호화?
				if(pFileInfo->bIsEncrypt)
				{
					pDataSize += BM::ENC_HEADER_SIZE;
				}

				//	복사하자
				if(bFind && bInPatch)
				{
					ffile.write(pData, pDataSize);
				}
				else if(bFind && !bInPatch)
				{
					std::vector<char> vecTemp;
					vecTemp.resize(pDataSize);
					kOrgCache.ReadData(OrgHeaderSize + pFileInfo->offset, &vecTemp.at(0), pDataSize);
					ffile.write(&vecTemp.at(0), pDataSize);
				}
				//if(pData)
				//{
				//	::memcpy(pNewData, pData, pDataSize);
				//	pNewData += pDataSize;
				//}

				++New_file_iter;
			}

			++New_iter;
		}

		//kOrgFile.Clear();
		//kNewFile.Clear();
		ffile.close();
	}

	DeleteFile(wstrFileName.c_str());
	_wrename(wstrNewName.c_str(), wstrFileName.c_str());

	SetTotalCnt(1);

	return	PR_OK;
}

PATCH_RESULT PatchLib::AmendPackUpdate(const BM::FolderHash& PatchList, char* szData)
{
	//	리스트가 있을때만 해야겠지
	if(PatchList.size())
	{
		//	폴더를 먼저 선택하고
		BM::FolderHash::const_iterator	Folder_iter = PatchList.begin();
		while(Folder_iter != PatchList.end())
		{
			//	파일이 있다면
			if((*Folder_iter).second->kFileHash.size())
			{
				//	빼서 넘겨야 겠지?
				BM::FolderHash	kPatchList;
				kPatchList.insert(std::make_pair(L".\\", (*Folder_iter).second));
				PATCH_RESULT kRtn = AmendPatchToOrgMerge((*Folder_iter).first, kPatchList, szData);
				if(PR_OK != kRtn)
				{
					return kRtn;
				}
			}

			++Folder_iter;
		}
	}

	return	PR_OK;
}

PATCH_RESULT PatchLib::DividePatchList(BM::FolderHash& kResultPackList, BM::FolderHash& kResultNonPackList,
							 const BM::FolderHash& kPackList, const std::vector<std::wstring>& kPackFolderList)
{
	//	리스트가 없네 에러다
	if(!kPackList.size())
	{
		return PR_NOT_HAVE_PATCH_LIST;
	}

	//	패치 리스트 폴더 해쉬 기본적으로 하나지만. 루프는 돌아주자
	BM::FolderHash::const_iterator	Main_iter = kPackList.begin();
	while(Main_iter != kPackList.end())
	{
		//	패치 리스트의 파일 해쉬 본격적으로 나누는거다
		BM::FileHash::const_iterator	Main_file_iter = (*Main_iter).second->kFileHash.begin();
		while(Main_file_iter != (*Main_iter).second->kFileHash.end())
		{
			//	팩인지 아닌지 스위치
			bool	bIsPack = false;
			BM::PgFolderInfo* pFolderInfo = NULL;
			pFolderInfo = BM::PgFolderInfo::New();

			//	팩 폴더 리스트를 돌아서 골라내는거다
			std::vector<std::wstring>::const_iterator	kFolder_iter = kPackFolderList.begin();
			while(kFolder_iter != kPackFolderList.end())
			{
				//	폴더이름
				std::wstring	wstrFolderName = L".\\" + (*Main_file_iter).first;

				//	이제 패치리스트에서 찾아보자
				const std::wstring wstrTemp = wstrFolderName.substr(0, (*kFolder_iter).size());
				if(wcscmp(wstrTemp.c_str(), (*kFolder_iter).c_str()) == 0)
				{
					//	파일명 잘라야지
					const std::wstring	wstrFileName = (*Main_file_iter).first.substr(((*kFolder_iter).size() - 1), (*Main_file_iter).first.size());

					//	팩폴더가 결과 리스트에 추가된적이 있는지 확인하자
					BM::FolderHash::iterator	RstPack_iter = kResultPackList.find((*kFolder_iter) + L"\\");
					if(RstPack_iter == kResultPackList.end())
					{
						//	팩폴더없네 - 추가
						pFolderInfo->bIsUsePack = true;
						kResultPackList.insert(make_pair((*kFolder_iter) + L"\\", BM::PgFolderInfoPtr(pFolderInfo)));
						RstPack_iter = kResultPackList.find((*kFolder_iter) + L"\\");
					}
					//	파일 정보를 받고
					BM::PgFolderInfoPtr	RstFolderInfo = ((*RstPack_iter).second);

					//	파일 넣자
					RstFolderInfo->kFileHash.insert(make_pair(wstrFileName, (*Main_file_iter).second));
					bIsPack = true;	//	팩이다 스위치 온
					break;
				}

				++kFolder_iter;
			}

			//	팩이 아니랬나?
			if(!bIsPack)
			{
				//	파일명 잘라야지
				std::wstring	wstrFolderName;
				std::wstring	wstrFileName;
				BM::DivFolderAndFileName((*Main_file_iter).first, wstrFolderName, wstrFileName);

				//	폴더가 추가된 적이 있나?
				BM::FolderHash::iterator	RstNonPack_iter = kResultNonPackList.find(wstrFolderName);
				if(RstNonPack_iter == kResultNonPackList.end())
				{
					//	NonPack폴더 없네 - 추가
					pFolderInfo->bIsUsePack = false;
					kResultNonPackList.insert(make_pair(wstrFolderName, BM::PgFolderInfoPtr(pFolderInfo)));
					RstNonPack_iter = RstNonPack_iter = kResultNonPackList.find(wstrFolderName);
				}
				//	파일 정보를 받고
				BM::PgFolderInfoPtr	RstFolderInfo = (*RstNonPack_iter).second;

				//	파일 넣자
				RstFolderInfo->kFileHash.insert(make_pair(wstrFileName, (*Main_file_iter).second));
			}

			++Main_file_iter;
		}

		++Main_iter;
	}

	return	PR_OK;
}

PATCH_RESULT PatchLib::PackPatch(char* szData, DWORD dwSize, std::vector<std::wstring> const& kPackFolders)
{
	//	패치 헤더를 구한다
	BM::FolderHash	kPatchList;
	m_nPatchHeaderSize = (BM::PgDataPackManager::ReadHeader(szData, kPatchList));

	//	패치 리스트를 나눈다
	BM::FolderHash	kPatchPackList;
	BM::FolderHash	kPatchNonPackList;
	PATCH_RESULT kRtn = DividePatchList(kPatchPackList, kPatchNonPackList, kPatchList, kPackFolders);
	if(PR_OK != kRtn)
	{
		return	kRtn;
	}

	//	총 크기 계산
	size_t	TotalSize = 0;
	//TotalSize	+= GetListDataSize(kPatchPackList);
	TotalSize	+= GetListDataSize(kPatchNonPackList);
	SetFileSize(TotalSize);
	SetTotalSize(kPatchPackList.size() + 1);

	//	NonPack 업데이트
	kRtn = AmendNonPackUpdate(kPatchNonPackList, szData);
	if(PR_OK != kRtn)
	{
		return	kRtn;
	}
	SetTotalCnt(1);

	//	Pack 업데이트
	kRtn = AmendPackUpdate(kPatchPackList, szData);
	if(PR_OK != kRtn)
	{
		return	kRtn;
	}

	//BM::FolderHash		kClientPackList;
	//BM::FolderHash		kClientNonPackList;
	//if(!GetClientList(kClientPackList, kClientNonPackList))
	//{
	//	return false;
	//}
	//if(!UpdatePackVer(kClientPackList))
	//{
	//	return false;
	//}

	//SetTotalPer(1.0f);
	//SetWorkState(TEXT_PATCH_COMPLETE);

	return	PR_OK;
}

void PatchLib::MakePatchID(std::string strPatchID)
{
	std::vector<char>	vecPatchID;
	vecPatchID.resize(strPatchID.size());
	::memcpy(&vecPatchID.at(0), strPatchID.c_str(), strPatchID.size());
	//	저장
	BM::SimpleEncrypt(vecPatchID, vecPatchID);
	BM::MemToFile(L"PATCH.ID", vecPatchID, false);
	//BM::MemToFileNotReserveFolder(L"PATCH.ID", vecPatchID);
}
__int64 PatchLib::ReadPatchVer(std::wstring strVersion)
{
	typedef	std::list<std::wstring>	CUT_STRING;

	CUT_STRING	kNumberList;
	std::wstring	wstrKey = strVersion;
	std::wstring	wstrP = L".";
	BM::vstring::CutTextByKey(wstrKey, wstrP, kNumberList);

	if(kNumberList.size() != 3){ return 0; }

	CUT_STRING::iterator	iter = kNumberList.begin();

	BM::VersionInfo	Info;
	Info.Version.i16Major = (WORD)_wtoi(iter->c_str());	++iter;
	Info.Version.i16Minor = (WORD)_wtoi(iter->c_str()); ++iter;
	Info.Version.i32Tiny = _wtoi(iter->c_str());
	return Info.iVersion;
}
PATCH_RESULT PatchLib::UpdatePackVer(BM::FolderHash& kAllPackList, std::wstring strNewVersion)
{
	BM::VersionInfo kNewVersion;
	kNewVersion.iVersion = ReadPatchVer(strNewVersion);
	//	버젼만 바꿀 팩 리스트
	BM::FolderHash::iterator	Cl_iter = kAllPackList.begin();
	while(Cl_iter != kAllPackList.end())
	{
		std::wstring	wstrFileName = Cl_iter->first;
		BM::DetachFolderMark(wstrFileName);
		wstrFileName += L".DAT";
		
		//이전 포맷 이라면 컨버팅부터 하고 작업한다
		BM::PgDataPackManager::ConvertToFormattedDataFile(wstrFileName);

		if(!BM::PgDataPackManager::UpdatePackVersion(wstrFileName, kNewVersion))
		{
			return PR_UPDATE_PACK_VERSION_FAIL;
		}

		++Cl_iter;
	}

	return	PR_OK;
}

PATCH_RESULT PatchLib::GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList, std::vector<std::wstring>	const& kPackFolders)
{
	//	클라이언트 Pack 리스트
	std::vector<std::wstring>::const_iterator		FolderName_iter = kPackFolders.begin();
	while(FolderName_iter != kPackFolders.end())
	{
		std::wstring	FileName = (*FolderName_iter) + L".DAT";

		if(_access(MB(FileName), 0) != -1)
		{
			//이전 포맷 이라면 컨버팅부터 하고 작업한다
			BM::PgDataPackManager::ConvertToFormattedDataFile(FileName);

			BM::PgDataPackManager::PackFileCache_new kCache;
			if(kCache.Open(FileName))
			{
				BM::PgDataPackManager::ReadHeader(kCache);
			}
			BM::FolderHash	kFolderList;
			kFolderList = kCache.mFolderHash;
			//BM::PgDataPackManager::ReadHeader(FileName, kFolderList);
			PackList.insert(std::make_pair((*FolderName_iter) + L"\\", kFolderList.begin()->second));
		}

		++FolderName_iter;
	}

	return	PR_OK;
}

PATCH_RESULT PatchLib::PatchProcess(std::wstring wstrFile)
{
	HANDLE hFile = CreateFileW(wstrFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		OutputDebugStringW(L"file create fail.\n");
		return PR_FILE_OPEN_FAIL;
	}

	Reset();
	
	// 실행파일부를 제외한 데이타의 위치를 찾는다.
	DWORD dwRead = 0;
	IMAGE_DOS_HEADER DosHdr;
	ReadFile(hFile, &DosHdr, sizeof(DosHdr), &dwRead, NULL);

	IMAGE_NT_HEADERS32 pNtHdrs;
	DWORD dwPos = ((DWORD)&pNtHdrs.FileHeader.PointerToSymbolTable) - ((DWORD)&pNtHdrs);
	dwPos += DosHdr.e_lfanew;

	SetFilePointer(hFile, dwPos, NULL, FILE_BEGIN);
	DWORD dwOffset = 0;
	ReadFile(hFile, &dwOffset, sizeof(dwOffset), &dwRead, NULL);
	if(0 == dwOffset)
	{
		OutputDebugStringW(L"not have data.\n");
		return PR_NOT_HAVE_DATA;
	}
	SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	// 올드 패치 버전
	BYTE byOldVersion = 0;
	ReadFile(hFile, &byOldVersion, sizeof(byOldVersion), &dwRead, NULL);
	WCHAR szOldVersion[MAX_PATH]={0,};
	ReadFile(hFile, szOldVersion, byOldVersion, &dwRead, NULL);
	szOldVersion[byOldVersion] = '\0';
	// 뉴 패치 버전
	BYTE byNewVersion = 0;
	ReadFile(hFile, &byNewVersion, sizeof(byNewVersion), &dwRead, NULL);
	WCHAR szNewVersion[MAX_PATH]={0,};
	ReadFile(hFile, szNewVersion, byNewVersion, &dwRead, NULL);
	szNewVersion[byNewVersion] = '\0';
	// 버전 체크
	std::wstring strOldVersion(szOldVersion);
	std::wstring strNewVersion(szNewVersion);
	PATCH_RESULT kRtn = ClientVersionCheck(strOldVersion, strNewVersion);
	if(PR_OK != kRtn)
	{
		return kRtn;
	}
	// 체인지 로그
	DWORD dwLog = 0;
	ReadFile(hFile, &dwLog, sizeof(dwLog), &dwRead, NULL);
	SetFilePointer(hFile, dwLog, NULL, FILE_CURRENT);// 읽지 않고 위치 이동만 한다.
	// 데이타 크기
	dwOffset = dwOffset + byOldVersion + byNewVersion + (sizeof(BYTE)*2) + sizeof(DWORD) + dwLog;
	LARGE_INTEGER iFileSize;
	GetFileSizeEx(hFile, &iFileSize);
	DWORD dwSize = iFileSize.LowPart-dwOffset;
	char* szData = new char[dwSize];
	// 데이타 읽기
	ReadFile(hFile, szData, sizeof(char)*dwSize, &dwRead, NULL);
	// 패치 초기화
	std::vector<std::wstring>	kPackFolders;
	kPackFolders.push_back(L".\\XML");
	kPackFolders.push_back(L".\\SCRIPT");
	kPackFolders.push_back(L".\\BGM");
	kPackFolders.push_back(L".\\UI");
	kPackFolders.push_back(L".\\FONT");
	kPackFolders.push_back(L".\\DATA\\1_CHA");
	kPackFolders.push_back(L".\\DATA\\2_MON");
	kPackFolders.push_back(L".\\DATA\\2_MON2");
	kPackFolders.push_back(L".\\DATA\\3_WORLD");
	kPackFolders.push_back(L".\\DATA\\4_ITEM");
	kPackFolders.push_back(L".\\DATA\\5_EFFECT");
	kPackFolders.push_back(L".\\DATA\\6_UI");
	BM::CPackInfo kPackInfo;//Init
	if(!kPackInfo.Init())
	{
		assert(NULL);
	}
	BM::PgDataPackManager::Init(kPackInfo);
	SetFilePer(0.0f);
	SetTotalPer(0.0f);

	// 패치
	kRtn = PackPatch(szData, dwSize, kPackFolders);
	if(PR_OK != kRtn)
	{
		delete[] szData;
		if(!m_wstrTextState.empty())
		{
			Sleep(100);
			DeleteFile(m_wstrTextState.c_str());
		}
		return kRtn;
	}

	delete[] szData;

	BM::FolderHash kClientPackList;
	BM::FolderHash kClientNonPackList;
	kRtn = GetClientList(kClientPackList, kClientNonPackList, kPackFolders);
	if(PR_OK != kRtn)
	{
		return kRtn;
	}
	kRtn = UpdatePackVer(kClientPackList, strNewVersion);
	if(PR_OK != kRtn)
	{
		return kRtn;
	}

	MakePatchID(MB(szNewVersion));
	SetTotalPer(1.0f);


	return PR_OK;
}

void PatchLib::SetTotalPer(float Per)
{
	m_fTotalPercent = Per; 
	if(m_pStateCallback)
	{
		if( 0.0f != m_fFilePercent || 0.0f != m_fTotalPercent )
		{
			m_pStateCallback(m_strFileName.c_str(), m_fFilePercent, m_fTotalPercent);
		}
	}
}
void PatchLib::SetFilePer(float Per)
{
	m_fFilePercent = Per;
	if(m_pStateCallback)
	{
		if( 0.0f != m_fFilePercent || 0.0f != m_fTotalPercent )
		{
			m_pStateCallback(m_strFileName.c_str(), m_fFilePercent, m_fTotalPercent);
		}
	}
}
void PatchLib::SetTotalSize(size_t size)	
{
	m_TotalSize = size; 
	m_CntTotalSize = 0;
}
void PatchLib::SetFileSize(size_t size)	
{
	m_FileSize = size; 
	m_CntFileSize = 0;
}
void PatchLib::SetTotalCnt(size_t size)	
{
	m_CntTotalSize += size; 
	SetTotalPer(static_cast<float>(m_CntTotalSize) / static_cast<float>(m_TotalSize));
}
void PatchLib::SetFileCnt(size_t size)		
{
	m_CntFileSize += size; 
	SetFilePer(static_cast<float>(m_CntFileSize) / static_cast<float>(m_FileSize));
}
void PatchLib::SetCallbackFunc(PFCALLBACK pStateCallback)
{
	m_pStateCallback = pStateCallback;
}
//////////////////////////////////////////////////////////////////////////
// CPatch
//////////////////////////////////////////////////////////////////////////
PatchLib& CPatch::GetPatchLib()
{
	static PatchLib kPatch;
	return kPatch;
}

int CPatch::FetchFile(LPCTSTR pszFile, PFCALLBACK pCallback)
{
	if(NULL == pszFile)
	{
		OutputDebugStringW(L"file name is empty.\n");
		return static_cast<int>(PR_FILENAME_EMPTY);
	}

	BM::g_bCalcSHA2CheckSum = true;
	BM::g_bUseFormattedFile = true;

	std::wstring wstrFile(pszFile);
	//m_wstrFileName = wszFile;
	//SetCallbackFunc(pCallback);
	//GetPatchLib().SetCallbackFunc(m_pStateCallBackFunc);
	GetPatchLib().SetCallbackFunc(pCallback);
	PATCH_RESULT Rtn;
	Rtn = GetPatchLib().PatchProcess(wstrFile);
	BM::PgDataPackManager::Clear();
	return static_cast<int>(Rtn);
}

//void CPatch::PatchCallBackFunc(float fCurrent, float fTotal)
//{
//	if(m_pStateCallBackFunc)
//	{
//		m_pStateCallBackFunc(m_wstrFileName.c_str() fCurrent, fTotal);
//	}
//}
//void CPatch::SetCallbackFunc(PFCALLBACK pStateCallback)
//{
//	m_pStateCallBackFunc = pStateCallback;
//}

//int APIENTRY _tWinMain(HINSTANCE hInstance,
//					   HINSTANCE hPrevInstance,
//					   LPTSTR    lpCmdLine,
//					   int       nCmdShow)
//{
//	CPatch kPatch;
//	PATCH_RESULT kRtn = kPatch.Patch(L"out.exe");
//	return 0;
//}
