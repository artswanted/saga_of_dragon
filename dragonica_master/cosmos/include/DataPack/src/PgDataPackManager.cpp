#include "stdafx.h"
#include <cassert>
#include "BM/HSEL.h"
#include "BM/FileSupport.h"
#include "BM/Stream.h"
#include "../PackInfo.h"
#include "../PgDataPackManager.h"

using namespace BM;

/*
#ifdef _T(x)
	#undef _T
	#define _T(x)       (x)
#endif
*/
PgDataPackManager::CacheHash PgDataPackManager::m_sCacheHash;
CPackInfo PgDataPackManager::m_kPackInfo;
bool PgDataPackManager::m_bIsOnLoop;

Loki::Mutex PgDataPackManager::m_kLock;

PgDataPackManager::PgDataPackManager()
{
}

PgDataPackManager::~PgDataPackManager()
{
}

bool PgDataPackManager::Init(const CPackInfo &rkInfo)
{
	m_kPackInfo = rkInfo;
	m_bIsOnLoop = true;
	return true;
}

void PgDataPackManager::Clear()
{
	m_sCacheHash.clear();
}

bool PgDataPackManager::PackFolders(std::wstring const& rkTargetDir)
{
	::SetCurrentDirectory( rkTargetDir.c_str() );
	FolderHash kFolders;
	if(GetFileList(_T(""), kFolders, GFL_ONLY_FOLDER))
	{
		FolderHash::iterator folder_itor = kFolders.begin();
			
		while(folder_itor != kFolders.end())
		{
			PgFolderInfo kFolderInfo;
			if(S_OK == m_kPackInfo.FolderState((*folder_itor).first, kFolderInfo))
			{
				FolderHash kDataHash;
				if(!GetFileList(_T(".\\") + (*folder_itor).first, kDataHash)){return false;}
				if(!PackFile(kDataHash, (*folder_itor).first + _T(".dat"))){return false;}
			}
			++folder_itor;
		}
		return true;
	}
	return false;
}

bool PgDataPackManager::PackTargetFolder(std::wstring const& rkTargetDir, std::wstring const& rkResultFileName)
{
	::SetCurrentDirectory(rkTargetDir.c_str());

	PgFolderInfoPtr pFolderInfo;
	pFolderInfo = PgFolderInfo::New();
	if (pFolderInfo == NULL)
		return false;

	if(S_OK == m_kPackInfo.FolderState(rkTargetDir, *pFolderInfo))
	{
		FolderHash kInHash;
		if(!GetFileList(_T(".\\"), kInHash))
		{
			return false;
		}
		// 상위루트는 Use Pack 속성을 준다.
		BM::FolderHash::iterator find_itor = kInHash.find(_T(".\\"));
		if (find_itor != kInHash.end())
		{
			find_itor->second->bIsUsePack = true;
		}
		else
		{
			pFolderInfo->bIsUsePack = true;
			kInHash.insert(std::make_pair(L".\\", pFolderInfo));
		}
		RecursiveProcessPackState(kInHash);
		if(!PackFile(kInHash, _T("..\\") + rkResultFileName))
		{
			return false;
		}
	}
	return true;
}

bool PgDataPackManager::RecursiveProcessPackState(BM::FolderHash &rkFolderHash)
{
	// 상위 폴더와 하위 폴더 둘다 UsePack 속성이 걸려있으면
	// 상위 폴더의 UsePack 속성을 쓰기 위해서 하위 폴더의 UsePack을 false로.
	BM::FolderHash kFindHash = rkFolderHash;
	BM::FolderHash::iterator itr;
	BM::FolderHash::iterator itr_dest;

	for (itr = kFindHash.begin() ;
		itr != kFindHash.end() ;
		++itr)
	{
		if (!(itr->second->bIsUsePack))
		{
			continue;
		}

		for (itr_dest = rkFolderHash.begin() ;
			itr_dest != rkFolderHash.end() ;
			++itr_dest)
		{
			if (itr->first != itr_dest->first &&
				itr_dest->first.find(itr->first) != -1 &&
				itr_dest->second->bIsUsePack == true)
			{
				itr_dest->second->bIsUsePack = false;
			}
		}
	}

	// 팩들 구한다.
	// 상위 폴더가 팩이면 하위 폴더는 상위 폴더 Hash 안으로 들어 가게 하기 위해.
	BM::FolderHash kPackHash;
	BM::FolderHash::iterator folder_itor = rkFolderHash.begin();
	while(folder_itor != rkFolderHash.end())
	{
		if (folder_itor->second->bIsUsePack)
		{
			kPackHash.insert(std::make_pair(folder_itor->first, folder_itor->second));
		}
		++folder_itor;
	}

	// ! UsePack인 폴더의 하위 폴더들은 추가 하지 않는다.
	BM::FolderHash kNonePackHash;
	folder_itor = rkFolderHash.begin();
	while(folder_itor != rkFolderHash.end())
	{
		bool bFind = false;
		BM::FolderHash::iterator pack_itor = kPackHash.begin();
		while(pack_itor != kPackHash.end())
		{
			std::wstring const& rkPackFolderName = pack_itor->first;
			std::wstring const& rkFolderName = folder_itor->first;
			if (rkPackFolderName != rkFolderName &&
				rkFolderName.find(rkPackFolderName) != -1)
			{
				// 하위 폴더중.. 그의 상위 폴더가 UsePack걸려있다! 그것을 찾았다.
				// 상위 폴더 안 Hash 에 넣어주는데.. 상대 경로 지정 필요.
				const BM::FileHash &rkFileHash = (*folder_itor).second->kFileHash;
				BM::FileHash::const_iterator file_itor = rkFileHash.begin();
				while( rkFileHash.end() != file_itor )
				{
					std::wstring kRelFolder = folder_itor->first;
					size_t iFirst = kRelFolder.find(pack_itor->first);
					if (iFirst != -1)
					{
						kRelFolder.erase(iFirst, pack_itor->first.size());
					}
					
					pack_itor->second->Insert(kRelFolder + file_itor->first, file_itor->second);
					++file_itor;
				}

				bFind = true;
				break;
			}
			++pack_itor;
		}

		if (bFind)
		{
		}
		else if (!folder_itor->second->bIsUsePack)
		{
			kNonePackHash.insert(std::make_pair(folder_itor->first, folder_itor->second));
		}

		++folder_itor;
	}

	folder_itor = kNonePackHash.begin();
	while(folder_itor != kNonePackHash.end())
	{
		kPackHash.insert(std::make_pair(folder_itor->first, folder_itor->second));
		++folder_itor;
	}

	rkFolderHash = kPackHash;
	return true;
}

bool PgDataPackManager::GetFileListWithoutCRC(std::wstring const& rkDirHeader, FolderHash &rkFolderHash, DWORD &dwFileCount, DWORD const dwFlag, std::wstring const& rkStateStandardDir)
{
	// FolderHash의 first에 해당하는 부분의 상대경로.
	// 폴더명 끝 컨버팅
	std::wstring kConvDirHeader = rkDirHeader;
	BM::ConvFolderMark(kConvDirHeader);
	if(kConvDirHeader.size())
	{
		wchar_t const tLast = kConvDirHeader.at(kConvDirHeader.size()-1);
		if( tLast != _T('\\') 
		&&	tLast != _T('/') )
		{
			kConvDirHeader += _T("\\");
		}
	}

	// FolderState를 체크하기 위한 상대경로.
	// Deafult는 위의 rkDirHeader 와 같은데.. 특수한 경우에는 다를수도 있다.
	std::wstring kConvStateDir = rkStateStandardDir;
	if (rkStateStandardDir.size() != 0)
	{
		// 위처럼 바꾸자.
		BM::ConvFolderMark(kConvStateDir);
		if(kConvStateDir.size())
		{
			wchar_t const tLast = kConvStateDir.at(kConvStateDir.size()-1);
			if( tLast != _T('\\') 
			&&	tLast != _T('/') )
			{
				kConvStateDir += _T("\\");
			}
		}
	}
	else
	{
		kConvStateDir = kConvDirHeader;
	}

	WIN32_FIND_DATA w32fd = {0,};
	std::wstring kFindDir = kConvDirHeader + _T("*.*");

	HANDLE hFindFile = FindFirstFile(kFindDir.c_str(), &w32fd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	bool bRet = false;

	while(m_bIsOnLoop) 
	{
		if(0 != ::_tcslwr_s(w32fd.cFileName) )
		{
			bRet = false;
			break;
		}
		std::wstring kCurFileName = w32fd.cFileName;//디렉토리 이름일 수도 있다.

		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{// 디렉토리
			if( kCurFileName != _T(".") 
			&&	kCurFileName != _T("..") 	)
			{
				PgFolderInfoPtr pFolderInfo;
				pFolderInfo = PgFolderInfo::New();
				if (pFolderInfo == NULL)
				{
					return false;
				}

				std::wstring kCurFolderName = kConvDirHeader + kCurFileName;
				AddFolderMark(kCurFolderName);

				std::wstring kCurStateName = kConvStateDir + kCurFileName;
				AddFolderMark(kCurStateName);

				LWR(kCurFolderName);
				if(S_OK == m_kPackInfo.FolderState(kCurStateName, *pFolderInfo))
				{
					FolderHash::iterator folder_itor = rkFolderHash.find(kCurFolderName);
					if(folder_itor == rkFolderHash.end())
					{
						rkFolderHash.insert(std::make_pair(kCurFolderName, pFolderInfo));
					}

					if(dwFlag & GFL_ONLY_FOLDER)
					{
					}
					if(dwFlag & GFL_RECURSIVE)
					{
						// 폴더 없으면 넣어주자.
						GetFileListWithoutCRC(kCurFolderName, rkFolderHash, dwFileCount, dwFlag);
					}
				}
			}
		}
		else
		{//일반 파일
			PgFileInfo kFileInfo;
			kFileInfo = w32fd;//파일 저장 할때 offset을 준다.
			
			if(kFileInfo.OrgSize() > 0)
			if(dwFlag & GFL_ONLY_FILE)
			{
				if(S_OK == m_kPackInfo.FileState(kCurFileName, kFileInfo))
				{
					std::wstring wstrFolder = rkDirHeader;
					AddFolderMark(wstrFolder);

					LWR(wstrFolder);
					FolderHash::iterator folder_itor = rkFolderHash.find(wstrFolder);
					if(folder_itor == rkFolderHash.end())
					{
						auto ret = rkFolderHash.insert(std::make_pair(wstrFolder, PgFolderInfoPtr(PgFolderInfo::New())));
						if(!ret.second)
						{	
							assert(NULL);
							return false;
						}
						folder_itor = ret.first;
					}
					PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
					LWR(kCurFileName);
					bool const bRet = pFolderInfo->Insert(kCurFileName, kFileInfo);
					if(!bRet)
					{	
						assert(NULL);
						return false;
					} 

					dwFileCount += 1;
				}
			}
		} 
		
		if(FindNextFile(hFindFile, &w32fd) == FALSE)
		{
			bRet = true;
			break;
		}
	}

	FindClose(hFindFile);
	
	return bRet;
}

bool PgDataPackManager::GetFileList(std::wstring const& rkDirHeader, FolderHash &rkFolderHash, DWORD const dwFlag, std::wstring const& rkStateStandardDir)
{
	// FolderHash의 first에 해당하는 부분의 상대경로.
	// 폴더명 끝 컨버팅
	std::wstring kConvDirHeader = rkDirHeader;
	BM::ConvFolderMark(kConvDirHeader);
	if(kConvDirHeader.size())
	{
		wchar_t const tLast = kConvDirHeader.at(kConvDirHeader.size()-1);
		if( tLast != _T('\\') 
		&&	tLast != _T('/') )
		{
			kConvDirHeader += _T("\\");
		}
	}

	// FolderState를 체크하기 위한 상대경로.
	// Deafult는 위의 rkDirHeader 와 같은데.. 특수한 경우에는 다를수도 있다.
	std::wstring kConvStateDir = rkStateStandardDir;
	if (rkStateStandardDir.size() != 0)
	{
		// 위처럼 바꾸자.
		BM::ConvFolderMark(kConvStateDir);
		if(kConvStateDir.size())
		{
			wchar_t const tLast = kConvStateDir.at(kConvStateDir.size()-1);
			if( tLast != _T('\\') 
			&&	tLast != _T('/') )
			{
				kConvStateDir += _T("\\");
			}
		}
	}
	else
	{
		kConvStateDir = kConvDirHeader;
	}

	WIN32_FIND_DATA w32fd = {0,};
	std::wstring kFindDir = kConvDirHeader + _T("*.*");

	HANDLE hFindFile = FindFirstFile(kFindDir.c_str(), &w32fd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	bool bRet = false;

	while(m_bIsOnLoop) 
	{
		if(0 != ::_tcslwr_s(w32fd.cFileName) )
		{
			bRet = false;
			break;
		}
		std::wstring kCurFileName = w32fd.cFileName;//디렉토리 이름일 수도 있다.

		if(w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
			dwFlag & GFL_ONLY_FOLDER) 
		{// 디렉토리
			if( kCurFileName != _T(".") 
			&&	kCurFileName != _T("..") 	)
			{
				PgFolderInfoPtr pFolderInfo;
				pFolderInfo = PgFolderInfo::New();
				if (pFolderInfo == NULL)
				{
					return false;
				}

				std::wstring kCurFolderName = kConvDirHeader + kCurFileName;
				AddFolderMark(kCurFolderName);

				std::wstring kCurStateName = kConvStateDir + kCurFileName;
				AddFolderMark(kCurStateName);

				LWR(kCurFolderName);
				if(S_OK == m_kPackInfo.FolderState(kCurStateName, *pFolderInfo))
				{	
					FolderHash::iterator folder_itor = rkFolderHash.find(kCurFolderName);
					if(folder_itor == rkFolderHash.end())
					{
						rkFolderHash.insert(std::make_pair(kCurFolderName, pFolderInfo));
					}

					if(dwFlag & GFL_ONLY_FOLDER)
					{
					}
					if(dwFlag & GFL_RECURSIVE)
					{
						// 폴더 없으면 넣어주자.
						GetFileList(kCurFolderName, rkFolderHash);
					}
				}
			}
		}
		else
		{//일반 파일
			PgFileInfo kFileInfo;
			kFileInfo = w32fd;//파일 저장 할때 offset을 준다.
			
			if(kFileInfo.OrgSize() > 0)
			if(dwFlag & GFL_ONLY_FILE)
			{
				if(S_OK == m_kPackInfo.FileState(kCurFileName, kFileInfo))
				{
					bool bFileCheckSumCalced = false;

					if (BM::g_bCalcSHA2CheckSum)
					{
						bFileCheckSumCalced =  BM::GetFileSHA2(kConvDirHeader+kCurFileName, kFileInfo.ucSHA2CheckSum);
					}
					else
					{
						bFileCheckSumCalced =  BM::GetFileCRC(kConvDirHeader+kCurFileName, kFileInfo.iOrgCRC);
					}

					if (bFileCheckSumCalced)
					{
						std::wstring wstrFolder = rkDirHeader;
						AddFolderMark(wstrFolder);

						LWR(wstrFolder);
						FolderHash::iterator folder_itor = rkFolderHash.find(wstrFolder);
						if(folder_itor == rkFolderHash.end())
						{
							auto ret = rkFolderHash.insert(std::make_pair(wstrFolder, PgFolderInfoPtr(PgFolderInfo::New())));
							if(!ret.second)
							{	
								assert(NULL);
								return false;
							}
							folder_itor = ret.first;
						}
						PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
						bool const bRet = pFolderInfo->Insert(kCurFileName, kFileInfo);
						if(!bRet)
						{	
							assert(NULL);
							return false;
						} 
					}
					else
					{
						assert(NULL);
						bRet = false;
					}
				}
			}
		} 
		
		if(FindNextFile(hFindFile, &w32fd) == FALSE)
		{
			bRet = true;
			break;
		}
	}

	FindClose(hFindFile);
	
	return bRet;
}

unsigned __int64 GetTotalWriteSize(FolderHash &rkInHash)
{
	unsigned __int64  write_size = 0;
	FolderHash::iterator folder_itor = rkInHash.begin();
	while(folder_itor != rkInHash.end())
	{
		PgFolderInfoPtr pFolderHash = (*folder_itor).second;
		FileHash::iterator file_itor = pFolderHash->kFileHash.begin();
		while(file_itor != pFolderHash->kFileHash.end())
		{
			PgFileInfo &rFileInfo = (*file_itor).second;
			unsigned __int64 iFileSize = rFileInfo.WriteSize();
			if(iFileSize)//
			{
				write_size += iFileSize;
			}

			if(iFileSize && (*file_itor).second.bIsEncrypt)
			{
				write_size += ENC_HEADER_SIZE;
			}
			++file_itor;

		}
		++folder_itor;
	}
	return write_size;
}

unsigned __int64 PgDataPackManager::BuildOffset(FolderHash &rkFolderHash)//헤더를 저장한다?.
{
	unsigned __int64 iTotalSize = 0;

	FolderHash::iterator folder_itor = rkFolderHash.begin();
	while(folder_itor != rkFolderHash.end())
	{//순서에 맞게 offset 저장
		PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
		FileHash::iterator file_itor = pFolderInfo->kFileHash.begin();
		while(file_itor != pFolderInfo->kFileHash.end())
		{
			PgFileInfo &rFileInfo = (*file_itor).second;
			unsigned __int64 iFileSize = rFileInfo.WriteSize();
			(*file_itor).second.offset = (size_t)iTotalSize;
			
			if(S_OK == m_kPackInfo.FileState((*file_itor).first, rFileInfo))
			{
				if(iFileSize)
				{
					if(rFileInfo.bIsEncrypt)
					{
						iTotalSize += (ENC_HEADER_SIZE);//암호화 헤더를 줘야함.
					}

					iTotalSize += iFileSize;
				}
				++file_itor;
			}
			else
			{
				pFolderInfo->kFileHash.erase(file_itor++);//무시됨.
			}
		}
		++folder_itor;
	}

	return iTotalSize;
}

bool FolderHashToPacket(FolderHash &rkInHash, BM::Stream &kPacket)
{
	FolderHash::iterator folder_itor = rkInHash.begin();

	kPacket.Push(rkInHash.size());//Push 1 사이즈

	while(rkInHash.end() != folder_itor)
	{
		PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
		FileHash::iterator file_itor = pFolderInfo->kFileHash.begin();
		
		kPacket.Push((*folder_itor).first);//Push 2 first
		pFolderInfo->WriteToPacket(kPacket);//Push 3 세컨의 사이즈

		++folder_itor;
	}
	return true;
}

/*
size_t PgDataPackManager::ReadHeader(std::wstring const& rkPackedName)
{
	std::wstring wstrUprName = rkPackedName;
	UPR(wstrUprName);

	BM::ConvFolderMark(wstrUprName);

	char *pData = NULL;
//	PgFileMapping kFile;

	CacheHash::iterator cache_itor = m_sCacheHash.find(wstrUprName);
	if(cache_itor != m_sCacheHash.end())
	{
//		pData = (*cache_itor).second.mFileMapping.Data();
//		pData = (*cache_itor).second.m_kStream;
		return ReadHeader((*cache_itor).second); 
	}
//	else
//	{
//		std::fstream ffile;
//		ffile.open( rkPackedName.c_str(), std::ios_base::out | std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary );
//
//		if(ffile.is_open())
//		{
//			return ReadHeader(ffile, rkInHash); 
//		}
//	}
	
	return 0;
//	if(!pData){ return 0;} //데이터가 없다.
//	return ReadHeader(pData, rkInHash);
}
*/
size_t PgDataPackManager::ReadHeader(CacheHash::mapped_type &kCache)
{
	PgPackHeaderExtInfo kPackHeaderExtInfo;

	size_t szHeaderSize = 0;
	BM::Stream kPacket;

	size_t offset = 0;
	size_t pack_offset = 0;

	size_t const TempSize = kCache.ReadHeaderExtInfo(kPackHeaderExtInfo);//읽기
	
	pack_offset += TempSize;

	kPacket.Data().resize(kPackHeaderExtInfo.packet_size);

	pack_offset+= kCache.ReadData(pack_offset, &kPacket.Data().at(0), kPackHeaderExtInfo.packet_size);
	
	if(!BM::DecStream(kPacket.Data(), kPackHeaderExtInfo.vecPacketEncHeader))
	{
		return 0;
	}//복호화.

	if(kPackHeaderExtInfo.bIsZipped)
	{
		std::vector< char > vecDest;
		vecDest.resize(kPackHeaderExtInfo.org_size);
		if(UnCompress(vecDest, kPacket.Data()))
		{
			kPacket.Data() = vecDest;
		}
		else
		{
			assert(NULL);
			return 0;
		}
	}

	kPacket.PosAdjust();
	kCache.SetFolderHash(kPacket);

	kCache.SetHeaderSIze(kPackHeaderExtInfo.HeaderSize());
	return kPackHeaderExtInfo.HeaderSize();
}
bool PacketToFolderHash(BM::Stream &kPacket, FolderHash &rkInHash)
{
	size_t folder_size = 0;
	kPacket.Pop(folder_size);//Pop 1

	size_t i = 0;
	while(folder_size != i)
	{
		FolderHash::key_type key;
		PgFolderInfoPtr pFolderInfo;
		pFolderInfo = PgFolderInfo::New();
		if (pFolderInfo == NULL)
		{
			++i;
			continue;
		}

		size_t file_count = 0;

		kPacket.Pop(key);//Pop 2
		pFolderInfo->ReadFromPacket(kPacket);

		LWR(key);
		rkInHash.insert(std::make_pair(key, pFolderInfo));
		++i;
	}
	return true;
}
size_t PgDataPackManager::ReadHeader(char* pData, FolderHash &rkInHash)
{
	PgPackHeaderExtInfo kPackHeaderExtInfo;

	size_t szHeaderSize = 0;
	BM::Stream kPacket;

	size_t offset = 0;

	char const*	pData2 = pData;
	size_t TempSize = kPackHeaderExtInfo.ReadFromMem(pData);//읽기

	kPacket.Data().resize(kPackHeaderExtInfo.packet_size);

	::memcpy(&kPacket.Data().at(0), pData, kPackHeaderExtInfo.packet_size);//읽기

	pData+=kPackHeaderExtInfo.packet_size;

	if(!BM::DecStream(kPacket.Data(), kPackHeaderExtInfo.vecPacketEncHeader))
	{
		return 0;
	}//복호화.

	if(kPackHeaderExtInfo.bIsZipped)
	{
		std::vector< char > vecDest;
		vecDest.resize(kPackHeaderExtInfo.org_size);
		if(UnCompress(vecDest, kPacket.Data()))
		{
			kPacket.Data() = vecDest;
		}
		else
		{
			assert(NULL);
			return 0;
		}
	}

	kPacket.PosAdjust();
	PacketToFolderHash(kPacket,rkInHash);

	return kPackHeaderExtInfo.HeaderSize();
}

size_t PgDataPackManager::MakeHeader(FolderHash &rkInHash, std::fstream &ffile, bool const bIsWriteToFile, __int64 const iVersion)//헤더를 저장한다?.
{
	BuildOffset(rkInHash);//offset 빌딩

	BM::Stream kPacket;

	FolderHashToPacket(rkInHash, kPacket);

	PgPackHeaderExtInfo kPackHeaderExtInfo;
	kPackHeaderExtInfo.i64GameVersion = iVersion;//팩버전
	kPackHeaderExtInfo.org_size = kPacket.Data().size();//원본 사이즈

	std::vector< char > vecDest;
	if(Compress(kPacket.Data(), vecDest))
	{
		kPackHeaderExtInfo.bIsZipped = true;
		kPacket.Data() = vecDest;//대체
	}
	else
	{
		kPackHeaderExtInfo.bIsZipped = false;
	}

	kPackHeaderExtInfo.packet_size = kPacket.Data().size();//압축된 사이즈

	if(!BM::EncStream(kPacket.Data(), kPackHeaderExtInfo.vecPacketEncHeader)){return 0;}// 암호화.

	if(!bIsWriteToFile)
	{//파일 기록 안하면 그냥 돌아가 
		return kPackHeaderExtInfo.ThisSize();
	}

	if( ffile.is_open())
	{
		std::streamoff offset = kPackHeaderExtInfo.WriteToFile(ffile);

		ffile.seekp(offset);
		ffile.write(&kPacket.Data().at(0), kPackHeaderExtInfo.packet_size);
		offset += kPackHeaderExtInfo.packet_size;

		return offset;
	}
	return 0;
}

bool PgDataPackManager::PackFile(FolderHash &rkInHash, std::wstring const& rkFileName)
{
	unsigned __int64 iTotalSize = GetTotalWriteSize(rkInHash);
	unsigned __int64 iAccSize = 0;

	std::wstring strTempFileName(rkFileName);
	strTempFileName += _T("x");

	HANDLE	hFile = CreateFileW(strTempFileName.c_str(), FILE_READ_DATA|FILE_WRITE_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	DWORD dwRead=0;

	FolderHash::iterator folder_itor = rkInHash.begin();
	while(folder_itor != rkInHash.end())
	{
		PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
		FileHash::iterator file_itor = pFolderInfo->kFileHash.begin();
		while(file_itor != pFolderInfo->kFileHash.end())
		{
			unsigned __int64 iFileSize = (*file_itor).second.WriteSize();

			if(iFileSize)//사이즈 있으면.
			{
				std::vector< char > kOrgData;
				if(!BM::FileToMem((*folder_itor).first + _T("\\") + (*file_itor).first, kOrgData))
				{//파일 로딩 못했다.
					__asm int 3;
				}

				std::vector< char > kZippedData;
				if(BM::Compress(&kOrgData.at(0), (long)kOrgData.size(), kZippedData))//압축시도
				{//압축됐음
					(*file_itor).second.bIsZipped = true;
					(*file_itor).second.zipped_file_size = kZippedData.size();
				}
				else
				{//압축 안하는게 이득
					(*file_itor).second.bIsZipped = false;
					(*file_itor).second.zipped_file_size = kOrgData.size();

					kZippedData.resize(kOrgData.size());//암호화를 위해서 zipped 데이터에다가 원본 카피. 
					::memcpy( &kZippedData.at(0), &kOrgData.at(0), kOrgData.size());//압축이 안되면 오리지날 데이터라도 돌려준다.
				}

				if((*file_itor).second.bIsEncrypt)
				{
					std::vector< char > EncryptHeader;
					if(!BM::EncStream(kZippedData, EncryptHeader))
					{
						assert(NULL);
						return false;
					}

					BOOL bRtn = WriteFile(hFile, &EncryptHeader.at(0), EncryptHeader.size(), &dwRead, NULL);
					iAccSize += EncryptHeader.size();
				}

				BOOL bRtn = WriteFile(hFile, &kZippedData.at(0), kZippedData.size(), &dwRead, NULL);
				iAccSize += kZippedData.size();
			}
			++file_itor;
		}

		++folder_itor;
	}

	CloseHandle(hFile);

	std::vector< char > kContMappingData;
	if(!BM::FileToMem(strTempFileName, kContMappingData))
	{//파일 로딩 못했다.
		__asm int 3;
	}
	if(!kContMappingData.size())
	{//맵핑 사이즈가 없네.
		__asm int 3;
	}
	
	std::fstream ffile_ret;
	ffile_ret.open(rkFileName.c_str(), std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);

	if(ffile_ret.is_open())
	{
		size_t const HeaderSize = MakeHeader(rkInHash, ffile_ret);
		if(HeaderSize)
		{
			ffile_ret.seekp(HeaderSize);
			ffile_ret.write(&kContMappingData.at(0), kContMappingData.size());
			ffile_ret.close();
			if(::DeleteFile( (rkFileName + _T("x")).c_str() ))
			{
				return true;
			}
		}
		ffile_ret.close();
	}

	return false;
}

/*
void PgDataPackManager::PrintFolderHash(FolderHash& rkInHash)
{
	FolderHash::iterator folder_itor = rkInHash.begin();
	while(folder_itor != rkInHash.end())
	{
		PgFolderInfo &rFolderInfo = (*folder_itor).second;
		FileHash::iterator file_itor = rFolderInfo.kFileHash.begin();
		while(file_itor != rFolderInfo.kFileHash.end())
		{
			TCHAR szBuf[1024] = { 0, };
			_stprintf_s(szBuf, 1024, L"%s file, size:%d, CRC:%I64u, offset:%d, zipped size:%d, E(%d), Z(%d)\n",
				(*file_itor).first.c_str(),
				(*file_itor).second.org_file_size,
				(*file_itor).second.iOrgCRC,
				(*file_itor).second.offset,
				(*file_itor).second.zipped_file_size,
				(*file_itor).second.bIsEncrypt,
				(*file_itor).second.bIsZipped);
			OutputDebugString(szBuf);
			++file_itor;
		}

		++folder_itor;
	}
}
*/

PgDataPackManager::PackFileCache_new* PgDataPackManager::GetPackFileCache(std::wstring const& kFileName, bool bLoadFileToMemory)
{
	std::wstring wstrLwrParentName = kFileName;
	LWR(wstrLwrParentName);
	BM::ConvFolderMark(wstrLwrParentName);
	CacheHash::iterator cache_itor;
	{
		BM::CAutoMutex kLock(m_kLock);
		cache_itor = m_sCacheHash.find(wstrLwrParentName);
		if(cache_itor == m_sCacheHash.end())
		{ 
			auto ret = m_sCacheHash.insert(std::make_pair(wstrLwrParentName, CacheHash::mapped_type()));
			if(ret.second)
			{
				cache_itor = ret.first;			
				
				if(!(*cache_itor).second.Open(wstrLwrParentName))
				{
					return NULL;
				}

				ReadHeader((*cache_itor).second);
				if ((*cache_itor).second.GetHeaderSize() == 0)
				{
					return NULL;
				}

				if (bLoadFileToMemory)
				{
					(*cache_itor).second.LoadFileToMem();
				}
			}
		}
		return &cache_itor->second;
	}
}

bool PgDataPackManager::IsFileExistInPack(std::wstring const& rkParentName, std::wstring const& rkChildName )
{
	std::vector<char> kTemp;
	return LoadFromPack(rkParentName, rkChildName, kTemp, false);
}

bool PgDataPackManager::LoadFromPack(std::wstring const& rkParentName, std::wstring const& rkChildName, std::vector< char > &rkOutData, bool const bIsRealRead)
{
	std::wstring wstrLwrChildName = rkChildName;
	LWR(wstrLwrChildName);

	BM::ConvFolderMark(wstrLwrChildName);

	PackFileCache_new* pCache = GetPackFileCache(rkParentName);

	if (pCache == NULL)
	{
		return false;
	}

	rkOutData.clear();//깨끗이 만들고.
	if (pCache->GetHeaderSize() == 0)
	{
		return false;
	}

	std::wstring kFolder;
	std::wstring kFileName;

	BM::DivFolderAndFileName(wstrLwrChildName, kFolder, kFileName);

	LWR(kFolder);
	LWR(kFileName);
	
	bool bRet = false;
	bool bFind = false;
	std::wstring kConvName = kFolder;

	const BM::PgFileInfo *pInfo = NULL;
	// first가 .\\ 이고 그안에 모두 들어있을때의 처리.
	if (pCache->mFolderHash.size() == 1)
	{
		bFind = true;
		BM::FolderHash::const_iterator pack_itr = pCache->mFolderHash.begin();
		FolderHash::mapped_type const&rkElement = (*pack_itr).second;

		size_t iFindIdx = 0;
		if ( (iFindIdx = kConvName.find(_T(".\\"))) != -1)
		{
			kConvName.erase(iFindIdx, 2);
		}
		BM::AddFolderMark(kConvName);
		kConvName += kFileName;
		rkElement->Find(kConvName, pInfo);
	}

	// first가 폴더 경로 모두 들어가 있고 second는 파일 네임만 있을때..의 처리.
	if (!pInfo)
	{
		FolderHash::const_iterator folder_itor = pCache->mFolderHash.find(kFolder);
		if(folder_itor != pCache->mFolderHash.end())
		{
			FolderHash::mapped_type const&rkElement = (*folder_itor).second;
			rkElement->Find(kFileName, pInfo);
		}
	}

	if (pInfo == NULL || pInfo->OrgSize() <= 0)
	{
		return false;
	}

	assert(pInfo->zipped_file_size > 0);

	if(bIsRealRead)
	{
		std::vector< char > kEncryptInfo;
		std::vector< char > kTempBuffer;
		std::vector< char >* pReadBuffer = &rkOutData;
		size_t readSize = pInfo->WriteSize();

		if (pInfo->bIsEncrypt)
		{
			kEncryptInfo.resize(ENC_HEADER_SIZE);
			readSize += ENC_HEADER_SIZE; // enc header까지 같이 읽으려고
			rkOutData.resize(pInfo->WriteSize());
			pReadBuffer = &kTempBuffer;
		}

		pReadBuffer->resize(readSize);
		size_t pack_offset = pCache->GetHeaderSize();//헤더 만큼 Seek
		pack_offset += pInfo->offset;

		size_t read = pCache->ReadData(pack_offset, &pReadBuffer->at(0), readSize);

		assert(read == readSize);
		if (read != readSize)
		{
			return false;
		}

		if (pInfo->bIsEncrypt)
		{
			// enc header까지 같이 읽었으므로 바꿔줌.
			memcpy(&kEncryptInfo.at(0), &kTempBuffer.at(0), ENC_HEADER_SIZE);
			memcpy(&rkOutData.at(0), &kTempBuffer.at(ENC_HEADER_SIZE), pInfo->WriteSize());
			if(false == BM::DecStream(rkOutData, kEncryptInfo))
			{
				return false;
			}
		}

		if(pInfo->bIsZipped)
		{
			std::vector< char > kResult;
			kResult.resize(pInfo->OrgSize());

			if(BM::UnCompress(kResult, rkOutData))
			{
				kResult.swap(rkOutData);//결과랑 스왑(Out데이터에 셋팅 되어야 하니까.)
			}
		}
	}

	return true;
}

/*
bool PgDataPackManager::LoadFromPack_old(std::wstring const& rkParentName, std::wstring const& rkChildName, std::vector< char > &rkOutData)
{	
	//팩킹에서 로딩.
	std::wstring wstrUprParentName = rkParentName;
	UPR(wstrUprParentName);

	std::wstring wstrUprChildName = rkChildName;
	UPR(wstrUprChildName);

	BM::ConvFolderMark(wstrUprParentName);
	BM::ConvFolderMark(wstrUprChildName);

	m_kLock.Lock();
	CacheHash::iterator cache_itor = m_sCacheHash.find(wstrUprParentName);
	if(cache_itor == m_sCacheHash.end())
	{ 
		auto ret = m_sCacheHash.insert(std::make_pair(wstrUprParentName, PackFileCache()));
		if(ret.second)
		{
			cache_itor = ret.first;			
			if ((*cache_itor).second.mFileMapping.Init(wstrUprParentName, OPEN_EXISTING) != S_OK)
			{
				// TODO CahceHash에서 빼야 할까?
				m_kLock.Unlock();
				return false;
			}

			char *pData = (*cache_itor).second.mFileMapping.Data();

			if(!pData)
			{
				// TODO CahceHash에서 빼야 할까?
				m_kLock.Unlock();
				return false;
			}

			//	PgFileMapping	
			(*cache_itor).second.mHeaderSize = ReadHeader(pData, (*cache_itor).second.mFolderHash);
			if ((*cache_itor).second.mHeaderSize == 0)
			{
				// TODO CahceHash에서 빼야 할까?
				m_kLock.Unlock();
				return false;
			}
		}
	}
	m_kLock.Unlock();

	// QUESTION: 파일 CacheHash에 넣을때만 하면 되지 않을까?
	//(*cache_itor).second.Clear();
	//(*cache_itor).second.Init(wstrUprParentName);

	char *pData = (*cache_itor).second.mFileMapping.Data();
	
	if(!pData)
	{
		return false;
	}

//	PgFileMapping	
	rkOutData.clear();//깨끗이 만들고.
	if ((*cache_itor).second.mHeaderSize == 0)
	{
		return false;
	}
////////////////////////////////

//	FILE *pFile = NULL;
//	errno_t const err = ::_wfopen_s( &pFile, wstrUprParentName.c_str(), _T("rb"));
//	if( err || !pFile ){return false;}

	pData += (*cache_itor).second.mHeaderSize;//헤더 만큼 Seek

	std::wstring kFolder;
	std::wstring kFileName;

	BM::DivFolderAndFileName(wstrUprChildName, kFolder, kFileName);

	UPR(kFolder);
	UPR(kFileName);
	
	bool bRet = false;
	bool bFind = false;
	std::wstring kConvName = kFolder;

	const BM::PgFileInfo *pInfo = NULL;
	// first가 .\\ 이고 그안에 모두 들어있을때의 처리.
	if ((*cache_itor).second.mFolderHash.size() == 1)
	{
		bFind = true;
		BM::FolderHash::iterator pack_itr = (*cache_itor).second.mFolderHash.begin();
		FolderHash::mapped_type &rkElement = (*pack_itr).second;

		size_t iFindIdx = 0;
		if ( (iFindIdx = kConvName.find(_T(".\\"))) != -1)
		{
			kConvName.erase(iFindIdx, 2);
		}
		BM::AddFolderMark(kConvName);
		kConvName += kFileName;
		rkElement.Find(kConvName, pInfo);
	}

	// first가 폴더 경로 모두 들어가 있고 second는 파일 네임만 있을때..의 처리.
	if (!pInfo)
	{
		FolderHash::iterator folder_itor = (*cache_itor).second.mFolderHash.find(kFolder);
		if(folder_itor != (*cache_itor).second.mFolderHash.end())
		{
			FolderHash::mapped_type &rkElement = (*folder_itor).second;
			rkElement.Find(kFileName, pInfo);
		}
	}

	//if (bFind && element.kFileHash.size() != 0)
	if (pInfo)
	{
		//if(element.Find(kConvName, pInfo))//저장할때 0 이면 아예 안하고, 1 이상일때 암호화 걸기 때문에 사이즈가 enc 사이즈보다 커야만 한다.
		{
			pData += pInfo->offset;

			std::vector< char > kEncryptInfo;
			if(pInfo->bIsEncrypt)
			{
				size_t const szEncInfoSize = ENC_HEADER_SIZE;
				//암호화 정보를 뽑았고
				kEncryptInfo.resize(szEncInfoSize);
				::memcpy(&kEncryptInfo.at(0), pData, szEncInfoSize);
				pData+=szEncInfoSize;
			}

			//실제 파일을 뺀다.
			rkOutData.resize(pInfo->WriteSize());
			::memcpy(&rkOutData.at(0), pData, rkOutData.size());
			pData+=rkOutData.size();

			if(pInfo->bIsEncrypt)
			{
				if(BM::DecStream(rkOutData, kEncryptInfo))
				{
					bRet = true;
				}
			}
			else
			{
				bRet = true;
			}

			if(bRet)
			{
				if(pInfo->bIsZipped)
				{
					std::vector< char > kResult;
					kResult.resize(pInfo->org_file_size);

					if(BM::UnCompress(kResult, rkOutData))
					{
						kResult.swap(rkOutData);//결과랑 스왑(Out데이터에 셋팅 되어야 하니까.)
						return true;
					}
				}
				else
				{
					return true;
				}
			}
		}
	}

	return false;
}
*/
/*
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

void foo()
{
    ifstream file("hello.z", ios_base::in | ios_base::binary);
    filtering_streambuf<input> in;

    in.push(zlib_decompressor());
    in.push(file);
    boost::iostreams::copy(in, cout);
}
*/

bool PgDataPackManager::ClearBuffer(std::wstring const& strClearName)
{
	std::wstring wstrLwrClearName = strClearName;
	LWR(wstrLwrClearName);

	m_sCacheHash.erase(wstrLwrClearName);
	
	return true;
}

void PgDataPackManager::SetOnLoop(bool bLoop)
{
	m_bIsOnLoop = bLoop;
}

bool PgDataPackManager::GetOnLoop()
{
	return m_bIsOnLoop;
}

bool PgDataPackManager::MakeDiffList(const FolderHash &rkOrgFolderHash,
									 const FolderHash &rkNewFolderHash,
									 FolderHash &kResultHash,
									 DWORD const dwDiffFlag
									 )
{//Org에 없거나 다른 애들을 뽑아내는 기능
	BM::FolderHash::const_iterator new_folder_itor = rkNewFolderHash.begin();
	while(new_folder_itor != rkNewFolderHash.end())
	{	//src 와 patch를 비교
		BM::FolderHash::const_iterator org_folder_itor = rkOrgFolderHash.find( (*new_folder_itor).first );

		// Org에 폴더가 없다.
		if(org_folder_itor == rkOrgFolderHash.end())
		{
			if (DIFF_ONLY_RIGHT & dwDiffFlag)
			{
				kResultHash.insert(std::make_pair((*new_folder_itor).first, (*new_folder_itor).second));
			}
		}
		//헤드 폴더가 있다.
		else if(org_folder_itor != rkOrgFolderHash.end())
		{	
			const BM::FileHash &rkNewFileHash = (*new_folder_itor).second->kFileHash;
			BM::FileHash::const_iterator new_file_itor = rkNewFileHash.begin();
			while( rkNewFileHash.end() != new_file_itor )
			{
				bool bIsNewFile = false;

				std::wstring const& rkNewFileName = (*new_file_itor).first;
				const BM::PgFileInfo &rkNewFileInfo = (*new_file_itor).second;

				const BM::FileHash &rkOrgFileHash = (*org_folder_itor).second->kFileHash;
				BM::FileHash::const_iterator org_file_itor = rkOrgFileHash.find( (*new_file_itor).first);

				if( rkOrgFileHash.end() != org_file_itor)
				{
					std::wstring const& rkOrgFileName = (*org_file_itor).first;
					const BM::PgFileInfo &rkOrgFile = (*org_file_itor).second;
					{
						BM::PgFileInfo kOrgFileInfoForIns = rkOrgFile;

						// 사이즈, CRC, Encrypt 모두 비교.
						if(rkNewFileInfo != kOrgFileInfoForIns)
						{
							if(DIFF_ONLY_DIFF & dwDiffFlag)//다른파일이다
							{
								bIsNewFile = true;
							}
						}
						else
						{
							if(DIFF_ONLY_IDENTICAL & dwDiffFlag)//같은파일이다
							{
								bIsNewFile = true;
							}
						}
					}
				}
				else
				{//원본에서 찾을 수 없다.
					if(DIFF_ONLY_RIGHT & dwDiffFlag)
					{
						bIsNewFile = true;
					}
				}

				if(bIsNewFile)
				{
					//이걸 new_folder_itor 로 만들면 문제가 생김.. == 연산자가 다 비교하는게 아니기 때문
					auto Ret = kResultHash.insert( std::make_pair((*new_folder_itor).first, PgFolderInfoPtr(PgFolderInfo::New())));
					(Ret.first)->second->Insert(rkNewFileName, rkNewFileInfo);
				}
				++new_file_itor;
			}
		}
		++new_folder_itor;
	}

	if(DIFF_ONLY_LEFT & dwDiffFlag)
	{//org new 바꾸고 비교
		return MakeDiffList(rkNewFolderHash, rkOrgFolderHash, kResultHash, DIFF_ONLY_RIGHT);//org 온리인 것 비교
	}

	if( rkOrgFolderHash.size() == 0 )
	{
		kResultHash = rkNewFolderHash;
		return true;
	}

	return true;
}

bool PgDataPackManager::UpdatePackVersion(std::wstring const& wstrPackedName, VersionInfo const& iVer)
{
	PgPackHeaderExtInfo kPackHeaderExtInfo;	
	std::fstream ffile;
	ffile.open(wstrPackedName.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary);

	if (false == ffile.is_open())
	{
		return false;
	}

	size_t TempSize = kPackHeaderExtInfo.ReadFromFile(ffile);
	if (TempSize <= 0)
	{
		return false;
	}

	if (kPackHeaderExtInfo.bFormatted)
	{
		ffile.seekp(sizeof(kPackHeaderExtInfo.i64FormatIdentifier) + sizeof(kPackHeaderExtInfo.iFormatVersion));
	}
	else
	{
		ffile.seekp(0);
	}

	ffile.write((char*)&iVer.iVersion, sizeof(iVer.iVersion));
	//	파일정보 저장
	ffile.close();

	return	true;
}

bool PgDataPackManager::CheckPackVersion(std::wstring const& wstrPackedName, VersionInfo const& iVer)
{
	VersionInfo iPackVersion;
	if (false == GetPackVersion(wstrPackedName, iPackVersion))
	{
		return false;
	}

	if(iVer.iVersion != iPackVersion.iVersion)
	{
		return false;
	}

	return	true;
}

bool PgDataPackManager::GetPackVersion(std::wstring const& wstrPackedName, VersionInfo &iVer)
{
	PgPackHeaderExtInfo kPackHeaderExtInfo;	
	BM::IFStream	kStream;

	kStream.Open(wstrPackedName);
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

	kStream.Close();
	return	true;
}

bool PgDataPackManager::ConvertToFormattedDataFile(std::wstring const& wstrPackedName)
{
#ifdef _DEBUG

	DWORD time = ::timeGetTime();
	BM::vstring log;
	log = L"Start " + wstrPackedName + L" Converting to new format\n";
	OutputDebugString(log);
#endif

	PgPackHeaderExtInfo kPackHeaderExtInfo;
	BM::IFStream	kStream;

	kStream.Open(wstrPackedName);
	if (false == kStream.IsOpen())
	{
		return false;
	}

	size_t TempSize = kPackHeaderExtInfo.ReadFromFile(kStream);
	if (TempSize <= 0)
	{
		return false;
	}

	kStream.Close();

	if (true == kPackHeaderExtInfo.bFormatted)
	{
		return false; // 이미 formatted 된 버전이다.
	}	

	if (false == kPackHeaderExtInfo.bCurrentVersion)
	{
		// version converting
	}

	kPackHeaderExtInfo.bFormatted = true;
	kPackHeaderExtInfo.bCurrentVersion = true;

	std::wstring strTempFileName(wstrPackedName);
	strTempFileName += _T("x");
	// global flag를 여기서 마구 처리하는게 깔끔해 보이지는 않는데.. 어떤 방법이 있을까나..
	bool bCalcSHA2SettingBackup = BM::g_bCalcSHA2CheckSum;

	{
		size_t iOrgFileTotalSize = GetFileSize(wstrPackedName);
		size_t iOrgFileHeaderSize = 0;
		size_t iOrgFileDataSize = 0;

		// read
		BM::PgDataPackManager::PackFileCache_new kCache;
		if (false == kCache.Open(wstrPackedName))
		{
			return false;
		}		
		
		BM::g_bCalcSHA2CheckSum = false;
		iOrgFileHeaderSize = BM::PgDataPackManager::ReadHeader(kCache);
		if (0 >= iOrgFileHeaderSize)
		{
			BM::g_bCalcSHA2CheckSum = bCalcSHA2SettingBackup;
			return false;
		}

		iOrgFileDataSize = iOrgFileTotalSize - iOrgFileHeaderSize;
		
		BM::g_bCalcSHA2CheckSum = true;
		// update crc info(calc sha2 and store it)
		UpdateFileCRC(kCache, true);

		// header 다시 만들기
		BM::Stream kPacket;
		
		FolderHashToPacket(kCache.mFolderHash, kPacket);

		kPackHeaderExtInfo.org_size = kPacket.Data().size();//원본 사이즈

		std::vector< char > vecPackHeader;
		if(Compress(kPacket.Data(), vecPackHeader))
		{
			kPackHeaderExtInfo.bIsZipped = true;
			kPacket.Data() = vecPackHeader;//대체
		}
		else
		{
			kPackHeaderExtInfo.bIsZipped = false;
		}

		kPackHeaderExtInfo.packet_size = kPacket.Data().size();//압축된 사이즈

		if(!BM::EncStream(kPacket.Data(), kPackHeaderExtInfo.vecPacketEncHeader))
		{
			BM::g_bCalcSHA2CheckSum = bCalcSHA2SettingBackup;
			return false;
		}

		// write again
		std::fstream ffile;
		ffile.open(strTempFileName.c_str(), std::ios_base::in | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);

		if (false == ffile.is_open())
		{
			BM::g_bCalcSHA2CheckSum = bCalcSHA2SettingBackup;
			return false;
		}

		std::streamoff offset = kPackHeaderExtInfo.WriteToFile(ffile); // header info
		
		ffile.seekp(offset);
		ffile.write(&kPacket.Data().at(0), kPackHeaderExtInfo.packet_size); // folder info
		offset += kPackHeaderExtInfo.packet_size;

		if (iOrgFileDataSize > 0)
		{
			std::vector< char > vecPackData;
			vecPackData.resize(iOrgFileDataSize);
			kCache.ReadData(iOrgFileHeaderSize, &vecPackData.at(0), iOrgFileDataSize);
			
			ffile.seekp(offset);
			ffile.write(&vecPackData.at(0), iOrgFileDataSize);
		}

		ffile.close();
	}	// kCache를 메모리에서 해제시켜서 file handle을 close하기 위해 {}를 씀.

	BM::g_bCalcSHA2CheckSum = bCalcSHA2SettingBackup;

	if(::DeleteFile(wstrPackedName.c_str()))
	{
		if (::MoveFile(strTempFileName.c_str(), wstrPackedName.c_str()))
		{
#ifdef _DEBUG
			DWORD timeDiff = ::timeGetTime() - time;
			log = L"End " + wstrPackedName + L" Converting, time : ";
			log += timeDiff/1000;
			log += L"s\n";
			OutputDebugString(log);
#endif
			return true;
		}
	}

	return	false;
}

bool PgDataPackManager::UpdateFileCRC(CacheHash::mapped_type &kCache, bool bUseSHA)
{
	if (kCache.mFolderHash.size() <= 0)
		return false;

	FolderHash& rkInHash = kCache.mFolderHash;

	DWORD dwRead=0;
	size_t const headerSize = kCache.GetHeaderSize();//헤더 만큼 Seek

	FolderHash::iterator folder_itor = rkInHash.begin();
	while (folder_itor != rkInHash.end())
	{
		PgFolderInfoPtr pFolderInfo = (*folder_itor).second;
		FileHash::iterator file_itor = pFolderInfo->kFileHash.begin();
		while(file_itor != pFolderInfo->kFileHash.end())
		{
			BM::PgFileInfo & rkFileInfo = (*file_itor).second;
			unsigned __int64 iFileSize = rkFileInfo.WriteSize();

			if(iFileSize)//사이즈 있으면.
			{
				size_t pack_offset = headerSize + rkFileInfo.offset;
				std::vector< char > kEncryptInfo;
				std::vector< char > kOutData;
				bool bRet = false;

				if(rkFileInfo.bIsEncrypt)
				{
					size_t const szEncInfoSize = ENC_HEADER_SIZE;
					//암호화 정보를 뽑았고
					kEncryptInfo.resize(szEncInfoSize);
					pack_offset += kCache.ReadData(pack_offset,&kEncryptInfo.at(0), szEncInfoSize);
				}

				//실제 파일을 뺀다.
				kOutData.resize(rkFileInfo.WriteSize());
				pack_offset += kCache.ReadData(pack_offset, &kOutData.at(0), kOutData.size());

				if(rkFileInfo.bIsEncrypt)
				{
					if(BM::DecStream(kOutData, kEncryptInfo))
					{
						bRet = true;
					}
					else
					{
						bRet = false;
					}
				}
				else
				{
					bRet = true;
				}

				if(bRet)
				{
					if(rkFileInfo.bIsZipped)
					{
						std::vector< char > kResult;
						kResult.resize(rkFileInfo.org_file_size);

						if(BM::UnCompress(kResult, kOutData))
						{
							kResult.swap(kOutData);//결과랑 스왑(Out데이터에 셋팅 되어야 하니까.)
							bRet = true;
						}
						else
						{
							bRet = false;
						}
					}
					else
					{
						bRet = true;
					}
				}

				if (bRet)
				{
					// calc crc again
					if (BM::g_bCalcSHA2CheckSum || bUseSHA)
					{
						GetStreamSHA2(&kOutData.at(0), kOutData.size(), rkFileInfo.ucSHA2CheckSum);
					}
					else
					{
						unsigned __int64 iOldCRC = rkFileInfo.iOrgCRC;
						rkFileInfo.iOrgCRC = GetStreamCRC(&kOutData.at(0), kOutData.size());
						assert(iOldCRC == rkFileInfo.iOrgCRC);
					}
				}
				else
				{
					rkFileInfo.iOrgCRC = 0;
				}
			}
			++file_itor;
		}

		++folder_itor;
	}

	return true;
}