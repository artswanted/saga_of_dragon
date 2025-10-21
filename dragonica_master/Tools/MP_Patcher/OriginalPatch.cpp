#include	"OriginalPatch.h"
#include	"PatchCommon.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace PgMergeUtil
{
	std::wstring const kFileListName = L"filelist.dat";

	std::wstring VersionToWstr(BM::SDVersion const& rkVer)
	{
		BM::vstring const kTemp(BM::vstring(rkVer.i16Major) << L"." << rkVer.i16Minor << L"." << rkVer.i32Tiny << L"\\" );
		return std::wstring(kTemp.operator const std::wstring &());
	}

	void MergeRightToLeft(BM::FolderHash &lhs, BM::FolderHash::key_type const& rkKey, BM::FolderHash::mapped_type const& rkVal)
	{// lhs에
		BM::FolderHash::iterator find_iter = lhs.find(rkKey);
		if( lhs.end() == find_iter )
		{// rkKey 폴더가 없으면, 추가하고
			auto kRet = lhs.insert( std::make_pair(rkKey, rkVal) );
			if( !kRet.second )
			{
				WinMessageBox(NULL, L"Memmory Error", L"Error", MB_OK);
				INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
				DEBUG_BREAK;
				throw eTHROW_FAILED;
			}
		}
		else
		{// rkKey 폴더가 이미 존재하면
			BM::FolderHash::mapped_type& rkLeft = (*find_iter).second;

			BM::FileHash::const_iterator loop_iter = rkVal->kFileHash.begin();
			while( rkVal->kFileHash.end() != loop_iter )
			{// rkKey 폴더에 존재하는 파일이
				BM::FileHash::key_type const& rkFileKey = (*loop_iter).first;
				BM::FileHash::mapped_type const& rkFileVal = (*loop_iter).second;

				BM::FileHash::iterator fileFind_iter = rkLeft->kFileHash.find(rkFileKey);
				if( rkLeft->kFileHash.end() == fileFind_iter )
				{// lhs에 존재 하지 않으면 추가하고
					auto kRet = rkLeft->kFileHash.insert( std::make_pair(rkFileKey, rkFileVal) );
					if( !kRet.second )
					{
						WinMessageBox(NULL, L"Memmory Error2", L"Error", MB_OK);
						INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
						DEBUG_BREAK;
						throw eTHROW_FAILED;
					}
				}
				else
				{// lhs에 존재하면 새로운 정보로 덮어 쓴다
					(*fileFind_iter).second = rkFileVal;
				}
				++loop_iter;
			}
		}
	}

	void RegulateRightToLeft(BM::FolderHash &lhs, BM::FolderHash &rhs)
	{
		if( lhs.empty() || rhs.empty() )
		{
			return;
		}
		BM::FolderHash::const_iterator iter = rhs.begin();
		while( rhs.end() != iter )
		{
			BM::FolderHash::key_type const& rkKey = (*iter).first;
			BM::FolderHash::mapped_type const& rkVal = (*iter).second;

			BM::FolderHash::iterator find_iter = lhs.find(rkKey);
			if( lhs.end() != find_iter )
			{// rkKey 폴더가 이미 존재하면
				BM::FolderHash::mapped_type& rkLeft = (*find_iter).second;

				BM::FileHash::const_iterator loop_iter = rkVal->kFileHash.begin();
				while( rkVal->kFileHash.end() != loop_iter )
				{// rkKey 폴더에 존재하는 파일이
					BM::FileHash::key_type const& rkFileKey = (*loop_iter).first;
					BM::FileHash::mapped_type const& rkFileVal = (*loop_iter).second;

					BM::FileHash::iterator fileFind_iter = rkLeft->kFileHash.find(rkFileKey);
					if( rkLeft->kFileHash.end() != fileFind_iter )
					{// lhs에 존재 하면 삭제
						rkLeft->kFileHash.erase(fileFind_iter);
					}
					++loop_iter;
				}
			}
			++iter;
		}
	}

	bool Regulate(PATCH_HASH& kPatchHash, BM::FolderHash& kFolderHash)
	{
		if( kPatchHash.empty() )
		{
			return false;
		}

		PATCH_HASH::iterator patch_iter = kPatchHash.begin();
		while( kPatchHash.end() != patch_iter )
		{// 각 버전별 폴더 해시를...
			PATCH_HASH::mapped_type& rkFolderHash = (*patch_iter).second;

			if( rkFolderHash.empty() )
			{
				break;
			}

			//최신파일로 정리
			PgMergeUtil::RegulateRightToLeft(rkFolderHash, kFolderHash);

			++patch_iter;
		}

		return true;
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
namespace PgPatcherRscUtil
{
	std::wstring const kRsc1(L"res.dat");
	std::wstring const kRsc2(L"patcherconfig.ini");
	std::wstring const kRsc3(L"message.ini");
	std::wstring const kTempHeader(L"temp_");

	bool IsResourceFile(std::wstring const& rkFolder, std::wstring const& rkFileName)
	{
		if( L".\\" != rkFolder ) // 루트 아니면 안되
		{
			return false;
		}

		std::wstring kTemp(rkFileName);
		LWR(kTemp);
		if( std::wstring::npos != kTemp.find(kRsc1)
		||	std::wstring::npos != kTemp.find(kRsc2)
		||	std::wstring::npos != kTemp.find(kRsc3) )
		{
			return true;
		}
		return false;
	}

	void ReplaceRsc()
	{
		bool const bExistRsc1 = (-1 != _access(MB(kTempHeader + kRsc1), 0));
		if( bExistRsc1 )
		{
			::DeleteFile(kRsc1.c_str());
			::rename(MB(kTempHeader + kRsc1), MB(kRsc1));
		}
		bool const bExistRsc2 = (-1 != _access(MB(kTempHeader + kRsc2), 0));
		if( bExistRsc2 )
		{
			::DeleteFile(kRsc2.c_str());
			::rename(MB(kTempHeader + kRsc2), MB(kRsc2));
		}
		bool const bExistRsc3 = (-1 != _access(MB(kTempHeader + kRsc3), 0));
		if( bExistRsc3 )
		{
			::DeleteFile(kRsc3.c_str());
			::rename(MB(kTempHeader + kRsc3), MB(kRsc3));
		}

		// ResDat가 업데이트 되었을 수 있으므로 다시 로딩하게함
		if( bExistRsc1 || bExistRsc2 || bExistRsc1 )
		{
			g_PProcess.SetReloadResDat(true);
		}
	}

	void ClearTempRsc()
	{
		::DeleteFile((kTempHeader + kRsc1).c_str());
		::DeleteFile((kTempHeader + kRsc2).c_str());
		::DeleteFile((kTempHeader + kRsc3).c_str());
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace PgNewPatchCoreUtil
{
	bool	NewNonPackUpdate(const PATCH_HASH& kPatchHash)
	{
		if( kPatchHash.empty() )
		{
			return false;
		}

		PATCH_HASH::const_iterator patch_iter = kPatchHash.begin();
		while( kPatchHash.end() != patch_iter )
		{
			PATCH_HASH::key_type const& kKey = patch_iter->first;
			PATCH_HASH::mapped_type const& List = patch_iter->second;

			//std::wstring const wstrVersionPath = kKey;
			BM::VersionInfo kVersion;
			kVersion.iVersion = kKey;
			std::wstring const wstrVersionPath = PgMergeUtil::VersionToWstr(kVersion.Version);
			//	없으면 안한다
			if(List.size())
			{
				//	있으면 다돈다
				BM::FolderHash::const_iterator	iter = List.begin();
				while(iter != List.end())
				{
					if( g_PProcess.CheckEnd() )
					{
						throw eTHROW_CANCEL;
					}

					if((*iter).second->kFileHash.size())
					{
						BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
						std::wstring	wstrOrgPath = (*iter).first;
						while(File_iter != (*iter).second->kFileHash.end())
						{
							int iGetFileRetryCount = 0;
							bool bRetry = false;

							do {
								if( g_PProcess.CheckEnd() )
								{
									throw eTHROW_CANCEL;
								}

								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_PATCHINGFILE) + (*File_iter).first);

								if(g_PProcess.GetIsGameGard())
								{
									if(((*File_iter).first.compare(g_wstrGuardini) == 0) ||
										((*File_iter).first.compare(g_wstrGuarddes) == 0))
									{
										if(_access(MB((*File_iter).first), 0) != -1)
										{
											++File_iter;
											continue; // 이 파일들은 skip함.
										}
									}
								}

								std::wstring const& rkOrgFileName = (*File_iter).first;
								if(!g_FTPMgr.GetFile(wstrVersionPath + wstrOrgPath + rkOrgFileName + L".zip", L"Temp.Ftp"))
								{//	못받았다 실패!
									std::wstring const kTemp = L"\"" + rkOrgFileName + L".zip\"" + g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE);
									g_PProcess.SetWorkMsg(kTemp.c_str());
									iGetFileRetryCount++;
									bRetry = true;
									continue;
								}

								//	파일을 메모리에
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILELOADTOMEMORY));
								std::vector<char>	vecData;
								std::vector<char>	vecResult;
								std::vector<char>*	pVecData = &vecResult;

								BM::FileToMem(L"Temp.Ftp", vecData);
								DeleteFile(L"Temp.Ftp");
								BM::PgFileInfo const& kFileInfo = (*File_iter).second;
								if( kFileInfo.bIsEncrypt )
								{
									if (false == DecFile(kFileInfo.org_file_size, kFileInfo.zipped_file_size, kFileInfo.bIsZipped, kFileInfo.bIsEncrypt, vecData, vecResult))
									{
										iGetFileRetryCount++;
										bRetry = true;
										continue;
									}
								}
								else
								{
									if( kFileInfo.bIsZipped )
									{
										if (false == UnCompFile(File_iter->second.org_file_size, vecData, vecResult))
										{
											iGetFileRetryCount++;
											bRetry = true;
											continue;
										}
									}
									else
									{
										pVecData = &vecData;
									}
								}

								//	해당 디렉토리로 파일을 옮긴다
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILEMOVETODIR));
								std::wstring const wstrOrgFileName = wstrOrgPath + rkOrgFileName;
								BM::ReserveFolder(wstrOrgPath);

								if( PgPatcherRscUtil::IsResourceFile(wstrOrgPath, rkOrgFileName) )
								{
									std::wstring const kTempFile = wstrOrgPath + PgPatcherRscUtil::kTempHeader + rkOrgFileName;
									BM::MemToFile(kTempFile, *pVecData);
								}
								else
								{
									DeleteFile(wstrOrgFileName.c_str());
									BM::MemToFile(wstrOrgFileName, *pVecData);
								}
								++File_iter;
								bRetry = false;
							} while (bRetry == true && iGetFileRetryCount < GET_FILE_RETRY_COUNT);

							if (iGetFileRetryCount >= GET_FILE_RETRY_COUNT)
							{
								return	false;
							}
						}
					}
					++iter;
				}
			}
			++patch_iter;
		}

		return	true;
	}
	bool	NewPackUpdate(const PATCH_HASH& kPatchHash)
	{
		if( kPatchHash.empty() )
		{
			return false;
		}

		PATCH_HASH::const_iterator patch_iter = kPatchHash.begin();
		while( kPatchHash.end() != patch_iter )
		{
			PATCH_HASH::key_type const& kKey = patch_iter->first;
			PATCH_HASH::mapped_type const& List = patch_iter->second;

			//std::wstring const wstrVersionPath = kKey;
			BM::VersionInfo kVersion;
			kVersion.iVersion = kKey;
			std::wstring const wstrVersionPath = PgMergeUtil::VersionToWstr(kVersion.Version);
			//	안전검사
			if(List.size())
			{
				//	폴더를 돌자
				BM::FolderHash::const_iterator	iter = List.begin();
				while(iter != List.end())
				{
					if( g_PProcess.CheckEnd() )
					{
						throw eTHROW_CANCEL;
					}

					BM::FolderHash	kHeaderHash;
					kHeaderHash.insert(std::make_pair(L".\\", (*iter).second));

					//	팩의 데이타크기 및 헤더 크기
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_SIZEHADERANDPACK));
					size_t	DataSize	= GetListDataSize(kHeaderHash, ERT_PACK);

					std::fstream ffile;
					ffile.open(L".\\Patch.TMP", std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);
					if(!ffile.is_open())
					{
						return false;
					}
					size_t	HeaderSize = BM::PgDataPackManager::MakeHeader(kHeaderHash, ffile, true, g_PProcess.GetIsVersion());

					//	파일 생성 및 할당
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATEALLOCFILE));

					//	안전검사
					if((*iter).second->kFileHash.size())
					{
						//	파일을 받자
						size_t kDataSize = 0;
						g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DOWNLOADINGFILE));
						BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
						while(File_iter != (*iter).second->kFileHash.end())
						{
							int iGetFileRetryCount = 0;
							bool bRetry = false;

							do {

								if( g_PProcess.CheckEnd() )
								{
									ffile.close();
									throw eTHROW_CANCEL;
								}
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DOWNLOADINGFILE) + (*File_iter).first);

								// 업데이트 서버에는 파일이 낱개당 .ZIP로 되어있다.(일반 zip아님)
								std::wstring	wstrDownPath	= wstrVersionPath + (*iter).first + (*File_iter).first + L".zip";
								if(!g_FTPMgr.GetFile(wstrDownPath, L".\\Temp.ftp"))
								{//	Temp.ftp라는 임시 파일 이름으로 다운받고
									g_PProcess.SetWorkMsg(wstrDownPath + g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE));								
									iGetFileRetryCount++;
									bRetry = true;
									continue;
								}

								//	메모리에 올린후, 임시 파일은 삭제하고
								std::vector<char>	vecData;
								std::vector<char>	vecResult;

								BM::FileToMem(L".\\Temp.ftp", vecData);
								DeleteFile(L".\\Temp.ftp");
								if(!vecData.size())
								{
									g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDTOLOADDATA));
									iGetFileRetryCount++;
									bRetry = true;
									INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] FileToMem(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
									continue;
								}

								BM::PgFileInfo const& kFileInfo = (*File_iter).second;
								if( kFileInfo.bIsEncrypt )
								{
									if (false == DecFile(kFileInfo.org_file_size, kFileInfo.zipped_file_size, kFileInfo.bIsZipped, kFileInfo.bIsEncrypt, vecData, vecResult))
									{
										INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] DecFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
										iGetFileRetryCount++;
										bRetry = true;
										continue;
									}
								}
								else
								{
									if( kFileInfo.bIsZipped )
									{
										if (false == UnCompFile(File_iter->second.org_file_size, vecData, vecResult))
										{
											INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] UnCompFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
											iGetFileRetryCount++;
											bRetry = true;
											continue;
										}
									}
								}


								//	Header 정보 뒤에 파일 정보를 쓴다.
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COPYTOPACK));
								ffile.seekg(HeaderSize+kDataSize);
								ffile.write(&vecData.at(0), vecData.size());
								kDataSize += vecData.size();

								++File_iter;
								bRetry = false;
							} while (bRetry == true && iGetFileRetryCount < GET_FILE_RETRY_COUNT);

							if (iGetFileRetryCount >= GET_FILE_RETRY_COUNT)
							{
								ffile.close();
								return	false;
							}
						}
					}

					//	다 받았냐? 원본이랑 합치자?
					//kPatchFile.Clear();
					ffile.close();
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_MERGEORIGINALWITHDOWNLOADEDPACK));

					std::wstring	wstrFileName = (*iter).first;
					BM::DetachFolderMark(wstrFileName);
					std::wstring wstrOrgFile = wstrFileName + _T(".dat");
					std::wstring wstrNewFile = wstrFileName + _T("_new.dat");

					if(!PgPatchCoreUtil::OriginalPatchToOrgMerge((*iter).first))
					{
						BM::PgDataPackManager::ClearBuffer(L".\\patch.tmp");
						DeleteFile(L".\\patch.tmp");
						return	false;
					}
					else
					{	
						BM::PgDataPackManager::ClearBuffer(L".\\Patch.TMP");
						if(_access(MB(wstrOrgFile), 0) == -1)
						{
							BM::ReserveFolder(wstrOrgFile);
							_wrename(L".\\patch.tmp", wstrOrgFile.c_str());
						}
						else
						{
							DeleteFile(wstrOrgFile.c_str());
							_wrename(wstrNewFile.c_str(), wstrOrgFile.c_str());
							DeleteFile(L".\\patch.tmp");
						}
					}

					++iter;
				}
			}
			++patch_iter;
		}

		return	true;
	}
};
namespace PgPatchCoreUtil
{
	//	NonPack파일 업데이트
	bool	OriginalNonPackUpdate(const BM::FolderHash& List)
	{
		//	없으면 안한다
		if(List.size())
		{
			//	있으면 다돈다
			BM::FolderHash::const_iterator	iter = List.begin();
			while(iter != List.end())
			{
				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}

				if((*iter).second->kFileHash.size())
				{
					BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
					std::wstring	wstrOrgPath = (*iter).first;
					while(File_iter != (*iter).second->kFileHash.end())
					{
						int iGetFileRetryCount = 0;
						bool bRetry = false;

						do {
							if( g_PProcess.CheckEnd() )
							{
								throw eTHROW_CANCEL;
							}

							g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_PATCHINGFILE) + (*File_iter).first);

							if(g_PProcess.GetIsGameGard())
							{
								if(((*File_iter).first.compare(g_wstrGuardini) == 0) ||
									((*File_iter).first.compare(g_wstrGuarddes) == 0))
								{
									if(_access(MB((*File_iter).first), 0) != -1)
									{
										++File_iter;
										continue; // 이 파일들은 skip함.
									}
								}
							}

							std::wstring const& rkOrgFileName = (*File_iter).first;
							if(!g_FTPMgr.GetFile(wstrOrgPath + rkOrgFileName + L".zip", L"Temp.Ftp"))
							{//	못받았다 실패!
								std::wstring const kTemp = L"\"" + rkOrgFileName + L".zip\"" + g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE);
								g_PProcess.SetWorkMsg(kTemp.c_str());
								iGetFileRetryCount++;
								bRetry = true;
								continue;
							}

							//	파일을 메모리에
							g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILELOADTOMEMORY));
							std::vector<char>	vecData;
							std::vector<char>	vecResult;
							std::vector<char>*	pVecData = &vecResult;

							BM::FileToMem(L"Temp.Ftp", vecData);
							DeleteFile(L"Temp.Ftp");
							BM::PgFileInfo const& kFileInfo = (*File_iter).second;
							if( kFileInfo.bIsEncrypt )
							{
								if (false == DecFile(kFileInfo.org_file_size, kFileInfo.zipped_file_size, kFileInfo.bIsZipped, kFileInfo.bIsEncrypt, vecData, vecResult))
								{
									iGetFileRetryCount++;
									bRetry = true;
									continue;
								}
							}
							else
							{
								if( kFileInfo.bIsZipped )
								{
									if (false == UnCompFile(File_iter->second.org_file_size, vecData, vecResult))
									{
										iGetFileRetryCount++;
										bRetry = true;
										continue;
									}
								}
								else
								{
									pVecData = &vecData;
								}
							}

							//	해당 디렉토리로 파일을 옮긴다
							g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILEMOVETODIR));
							std::wstring const wstrOrgFileName = wstrOrgPath + rkOrgFileName;
							BM::ReserveFolder(wstrOrgPath);

							if( PgPatcherRscUtil::IsResourceFile(wstrOrgPath, rkOrgFileName) )
							{
								std::wstring const kTempFile = wstrOrgPath + PgPatcherRscUtil::kTempHeader + rkOrgFileName;
								BM::MemToFile(kTempFile, *pVecData);
							}
							else
							{
								DeleteFile(wstrOrgFileName.c_str());
								BM::MemToFile(wstrOrgFileName, *pVecData);
							}
							++File_iter;
							bRetry = false;
						} while (bRetry == true && iGetFileRetryCount < GET_FILE_RETRY_COUNT);

						if (iGetFileRetryCount >= GET_FILE_RETRY_COUNT)
						{
							return	false;
						}
					}
				}
				++iter;
			}
		}

		return	true;
	}

	//	Pack파일 업데이트
	bool	OriginalPackUpdate(const BM::FolderHash& List)
	{
		//	안전검사
		if(List.size())
		{
			//	폴더를 돌자
			BM::FolderHash::const_iterator	iter = List.begin();
			while(iter != List.end())
			{
				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}

				BM::FolderHash	kHeaderHash;
				kHeaderHash.insert(std::make_pair(L".\\", (*iter).second));

				//	팩의 데이타크기 및 헤더 크기
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_SIZEHADERANDPACK));
				size_t	DataSize	= GetListDataSize(kHeaderHash, ERT_PACK);

				std::fstream ffile;
				ffile.open(L".\\Patch.TMP", std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);
				if(!ffile.is_open())
				{
					return false;
				}
				size_t	HeaderSize = BM::PgDataPackManager::MakeHeader(kHeaderHash, ffile, true, g_PProcess.GetIsVersion());

				//	파일 생성 및 할당
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATEALLOCFILE));

				//	안전검사
				if((*iter).second->kFileHash.size())
				{
					//	파일을 받자
					size_t kDataSize = 0;
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DOWNLOADINGFILE));
					BM::FileHash::const_iterator	File_iter = (*iter).second->kFileHash.begin();
					while(File_iter != (*iter).second->kFileHash.end())
					{
						int iGetFileRetryCount = 0;
						bool bRetry = false;

						do {

							if( g_PProcess.CheckEnd() )
							{
								ffile.close();
								throw eTHROW_CANCEL;
							}
							g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DOWNLOADINGFILE) + (*File_iter).first);

							// 업데이트 서버에는 파일이 낱개당 .ZIP로 되어있다.(일반 zip아님)
							std::wstring	wstrDownPath	= (*iter).first + (*File_iter).first + L".zip";
							if(!g_FTPMgr.GetFile(wstrDownPath, L".\\Temp.ftp"))
							{//	Temp.ftp라는 임시 파일 이름으로 다운받고
								g_PProcess.SetWorkMsg(wstrDownPath + g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE));								
								iGetFileRetryCount++;
								bRetry = true;
								continue;
							}

							//	메모리에 올린후, 임시 파일은 삭제하고
							std::vector<char>	vecData;
							std::vector<char>	vecResult;

							BM::FileToMem(L".\\Temp.ftp", vecData);
							DeleteFile(L".\\Temp.ftp");
							if(!vecData.size())
							{
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDTOLOADDATA));
								iGetFileRetryCount++;
								bRetry = true;
								INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] FileToMem(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
								continue;
							}

							BM::PgFileInfo const& kFileInfo = (*File_iter).second;
							if( kFileInfo.bIsEncrypt )
							{
								if (false == DecFile(kFileInfo.org_file_size, kFileInfo.zipped_file_size, kFileInfo.bIsZipped, kFileInfo.bIsEncrypt, vecData, vecResult))
								{
									INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] DecFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
									iGetFileRetryCount++;
									bRetry = true;
									continue;
								}
							}
							else
							{
								if( kFileInfo.bIsZipped )
								{
									if (false == UnCompFile(File_iter->second.org_file_size, vecData, vecResult))
									{
										INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] UnCompFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*File_iter).first.c_str());
										iGetFileRetryCount++;
										bRetry = true;
										continue;
									}
								}
							}


							//	Header 정보 뒤에 파일 정보를 쓴다.
							g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COPYTOPACK));
							ffile.seekg(HeaderSize+kDataSize);
							ffile.write(&vecData.at(0), vecData.size());
							kDataSize += vecData.size();

							++File_iter;
							bRetry = false;
						} while (bRetry == true && iGetFileRetryCount < GET_FILE_RETRY_COUNT);

						if (iGetFileRetryCount >= GET_FILE_RETRY_COUNT)
						{
							ffile.close();
							return	false;
						}
					}
				}

				//	다 받았냐? 원본이랑 합치자?
				//kPatchFile.Clear();
				ffile.close();
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_MERGEORIGINALWITHDOWNLOADEDPACK));

				std::wstring	wstrFileName = (*iter).first;
				BM::DetachFolderMark(wstrFileName);
				std::wstring wstrOrgFile = wstrFileName + _T(".dat");
				std::wstring wstrNewFile = wstrFileName + _T("_new.dat");

				if(!OriginalPatchToOrgMerge((*iter).first))
				{
					BM::PgDataPackManager::ClearBuffer(L".\\patch.tmp");
					DeleteFile(L".\\patch.tmp");
					return	false;
				}
				else
				{	
					BM::PgDataPackManager::ClearBuffer(L".\\Patch.TMP");
					if(_access(MB(wstrOrgFile), 0) == -1)
					{
						BM::ReserveFolder(wstrOrgFile);
						_wrename(L".\\patch.tmp", wstrOrgFile.c_str());
					}
					else
					{
						DeleteFile(wstrOrgFile.c_str());
						_wrename(wstrNewFile.c_str(), wstrOrgFile.c_str());
						DeleteFile(L".\\patch.tmp");
					}
				}

				++iter;
			}
		}

		return	true;
	}

	//	패치와 원본 머지
	bool	OriginalPatchToOrgMerge(const std::wstring& TargetDir)
	{
		//	원본 파일
		//PgFileMapping	kOrgFile;
		BM::FolderHash	kOrgHash;
		std::wstring	wstrFileName = TargetDir;
		BM::DetachFolderMark(wstrFileName);
		wstrFileName += L".dat";
		if(_access(MB(wstrFileName), 0) == -1)
		{
			return	true;
		}

		//const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(wstrFileName, kOrgHash);
		//kOrgFile.Init(wstrFileName);
		BM::PgDataPackManager::PackFileCache_new kOrgCache;
		if(!kOrgCache.Open(wstrFileName))
		{
			return false;
		}
		const size_t	OrgHeaderSize = BM::PgDataPackManager::ReadHeader(kOrgCache);
		kOrgHash = kOrgCache.mFolderHash;


		//	패치 파일
		//PgFileMapping	kPatchFile;
		BM::FolderHash	kPatchHash;
		std::wstring	wstrPatchName = L".\\patch.tmp";
		//const size_t	PatchHeaderSize = BM::PgDataPackManager::ReadHeader(wstrPatchName, kPatchHash);
		//kPatchFile.Init(wstrPatchName);
		BM::PgDataPackManager::PackFileCache_new kPatchCache;
		if(!kPatchCache.Open(wstrPatchName))
		{
			return false;
		}
		const size_t	PatchHeaderSize = BM::PgDataPackManager::ReadHeader(kPatchCache);
		kPatchHash = kPatchCache.mFolderHash;


		//	원본에서 패치된 파일을 업데이트합니다.
		//	결합 파일
		BM::FolderHash	kNewHash;	

		//	리스트 생성 및 수정
		MergeList(kPatchHash, kOrgHash, kNewHash);

		//	결과 파일 생성
		//PgFileMapping	kNewFile;
		std::wstring	wstrNewName = TargetDir;
		BM::DetachFolderMark(wstrNewName);
		wstrNewName += L"_new.dat";
		const unsigned __int64	NewDataSize	= BM::PgDataPackManager::BuildOffset(kNewHash);

		std::fstream ffile;
		ffile.open(wstrNewName.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate | std::ios_base::binary);
		if(!ffile.is_open())
		{
			return false;
		}
		size_t 	NewHeaderSize = BM::PgDataPackManager::MakeHeader(kNewHash, ffile, true, g_PProcess.GetIsVersion());
		//const size_t	NewHeaderSize = BM::PgDataPackManager::MakeHeader(kNewHash, kNewFile, true, g_PProcess.GetIsVersion());
		//kNewFile.Init(wstrNewName, OPEN_EXISTING, NewHeaderSize + NewDataSize);

		//	결과대로 묶자
		BM::FolderHash::iterator	New_iter = kNewHash.begin();
		while(New_iter != kNewHash.end())
		{
			if( g_PProcess.CheckEnd() )
			{
				throw eTHROW_CANCEL;
			}

			//	폴더를 찾아
			BM::FolderHash::iterator	Patch_iter = kPatchHash.find((*New_iter).first);
			BM::FolderHash::iterator	Org_iter = kOrgHash.find((*New_iter).first);

			//	파일을 돌려
			BM::FileHash::iterator	New_file_iter = (*New_iter).second->kFileHash.begin();
			while(New_file_iter != (*New_iter).second->kFileHash.end())
			{
				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}

				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_MERGINGFILE) + (*New_file_iter).first);

				const BM::PgFileInfo	*pFileInfo = NULL;
				char*	pData = NULL;
				bool	bFind = false;
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
						//pData = kPatchFile.Data() + PatchHeaderSize + pFileInfo->offset;
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
						ffile.close();
						return	false;
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
							ffile.close();
							return	false;
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

				//	사이즈 알아보자 - 암호화?
				if(pFileInfo->bIsEncrypt)
				{
					pDataSize += ( BM::ENC_HEADER_SIZE );
				}

				//	복사하자
				/*
				if(pData)
				{
				::memcpy(pNewData, pData, pDataSize);
				pNewData += pDataSize;
				}
				*/
				if( bFind )
				{
					std::vector<char> vecTemp;
					vecTemp.resize(pDataSize);
					if( bInPatch )
					{ //Patch
						kPatchCache.ReadData(PatchHeaderSize + pFileInfo->offset, &vecTemp.at(0), pDataSize);
						// check file
						std::vector<char>	vecResult;

						if (pFileInfo->bIsEncrypt)
						{
							if (false == DecFile(pFileInfo->org_file_size, pFileInfo->zipped_file_size, pFileInfo->bIsZipped, pFileInfo->bIsEncrypt, vecTemp, vecResult))
							{
								INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] DecFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*New_file_iter).first.c_str());
								return false;
							}
						}
						else if (pFileInfo->bIsZipped)
						{
							if (false == UnCompFile(pFileInfo->org_file_size, vecTemp, vecResult))
							{
								INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] UnCompFile(%s) failed\n\r"), __FUNCTIONW__, __LINE__, (*New_file_iter).first.c_str());
								return false;
							}
						}
						ffile.write(&vecTemp.at(0), pDataSize);
					}
					else
					{ //Org
						kOrgCache.ReadData(OrgHeaderSize + pFileInfo->offset, &vecTemp.at(0), pDataSize);
						ffile.write(&vecTemp.at(0), pDataSize);
					}
				}

				++New_file_iter;
			}

			++New_iter;
		}

		BM::PgDataPackManager::ClearBuffer(L".\\patch.tmp");

		//kOrgFile.Clear();
		//kPatchFile.Clear();
		//kNewFile.Clear();

		ffile.close();

		return	true;
	}

	bool	OriginalPatchIDCheck()
	{
		//	새로운 아이디를 받고
		if(!g_FTPMgr.GetFile(L"patch.id", L"patch_new.id"))
		{
			g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L"\"patch.id\""));
			INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
			DEBUG_BREAK;
			throw	eTHROW_FAILED;
		}

		//	리커버리 시작 아이디를 받고
		if(!g_FTPMgr.GetFile(L"start.id", L"start_new.id"))
		{
			g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L"\"start.id\""));
			INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
			DEBUG_BREAK;
			throw	eTHROW_FAILED;
		}

		//	패치넘버만 짤라라
		const BM::VersionInfo	kNowVersionInfo = GetPatchID(L"patch.id");
		const BM::VersionInfo	kNewVersionInfo = GetPatchID(L"patch_new.id");
		const BM::VersionInfo	kStartVersionInfo = GetPatchID(L"start_new.id");
		g_PProcess.SetIsStartVersion(kStartVersionInfo.iVersion);
		//	버젼 정보에서 패치넘버만 남겨라

		wchar_t kMajorVer[30] = {0,};
		wchar_t kMinorVer[30] = {0,};
		wchar_t kTinyVer[30] = {0,};

		_i64tow_s(kNowVersionInfo.Version.i16Major, kMajorVer, 30, 10);
		_i64tow_s(kNowVersionInfo.Version.i16Minor, kMinorVer, 30, 10);
		_i64tow_s(kNowVersionInfo.Version.i32Tiny, kTinyVer, 30, 10);

		std::wstring wstrClientVersion;
		wstrClientVersion = WSTR_GAME_NAME + L" " + WSTR_VERSION + L": " + std::wstring(kMajorVer) + L"." + std::wstring(kMinorVer) + L"." + std::wstring(kTinyVer);

		//g_PProcess.SetIsClientVer(std::wstring(_i64tow(kNowVersionInfo.Version.i16Major, kClientVer, 10)));
		g_PProcess.SetIsClientVer(wstrClientVersion);

		//	패치할까?
		if(kNowVersionInfo.iVersion >= kNewVersionInfo.iVersion)
		{
			g_PProcess.SetIsVersion(kNowVersionInfo.iVersion);			
			// 패치 안해도 팩 버전 업데이트는 한번씩 해준다.
			UpdatePackVer();
			return	false;
		}

		/*
		BM::GUID	kGuidNew;
		BM::GUID	kGuidOrg;

		kGuidNew	= DecryptGUID(L"Patch_New.ID");
		kGuidOrg	= DecryptGUID(L"Patch.ID");

		g_PProcess.SetWorkMsg(L"ID Compare...");
		if(kGuidNew != kGuidOrg)
		{
		return	false;
		}
		*/

		g_PProcess.SetIsVersion(kNewVersionInfo.iVersion);

		return	true;

	}

};


/////////////////////
void	RecoveryPatchCore()
{
	if( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() )
	{
		g_PProcess.SubDisplay(true);
		g_WinApp->Display(SW_HIDE);
	}

	//	시작 아이디를 받고
	if(!g_FTPMgr.GetFile(L"start.id", L"start_new.id"))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L" \"start.id\""));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	const BM::VersionInfo	kStartVersionInfo = GetPatchID(L"start_new.id");
	g_PProcess.SetIsStartVersion(kStartVersionInfo.iVersion);

	DeleteFile(_T("start_new.id"));

	//	새로운 아이디를 받고
	if(!g_FTPMgr.GetFile(L"patch.id", L"patch_new.id"))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L" \"patch.id\""));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	const BM::VersionInfo	kVersionInfo = GetPatchID(L"patch_new.id");
	g_PProcess.SetIsVersion(kVersionInfo.iVersion);

	PgRecoveryPatch().Do();

	g_PProcess.SetTotalPer(1.0f);
	if( g_PProcess.GetNotPatched() )
	{
		throw eTHROW_CANCEL;
	}

	throw eTHROW_SUCCESS;
}

#ifdef NO_FTP_LOCAL_TEST
namespace PgRecoveryTest
{
	extern void WriteToFile(FILE* pFile, PATCH_HASH const& rkList, wchar_t const* szHeader = NULL);
	extern void WriteToFile(FILE* pFile, BM::FolderHash const& rkList, wchar_t const* szHeader = NULL);
};
#endif

//────────────────────────────────────────
//	패치
//────────────────────────────────────────
PgHeaderMergePatch::PgHeaderMergePatch()
{
}
PgHeaderMergePatch::~PgHeaderMergePatch()
{
}


PatchVersionCheckResult PgHeaderMergePatch::GetServerList(PATCH_HASH& PackList, PATCH_HASH& NonPackList)
{
	//std::wstring	FileName	= L"HEADFOLDERS.DAT";
	std::wstring const kTempFile(L"ver_temp.dat");
	DeleteFile(kTempFile.c_str());

	BM::VersionInfo kCurVersionInfo, kNewVersionInfo;
	kCurVersionInfo.iVersion = g_PProcess.GetIsOldVersion();	// 현재 버젼
	kNewVersionInfo.iVersion = g_PProcess.GetIsVersion();		// 새 버젼

	while( kNewVersionInfo.iVersion != kCurVersionInfo.iVersion )
	{// 현재 클라이언트 PATCH_VERSION과 업데이트 서버의 PATCH_VERSION이 다르다면
		// 현재버전.Tiny+1해서 해당 버전의 폴더/파일 정보를 담고 있는 filelist.dat 를 찾아 VER_TEMP.DAT라는 파일로 저장한다
		++kCurVersionInfo.Version.i32Tiny;
		
		if( !g_FTPMgr.GetFile(PgMergeUtil::VersionToWstr(kCurVersionInfo.Version) + PgMergeUtil::kFileListName, kTempFile) )
		{// 실패하면 현재버전.Minor+1해서 찾아본다
			DeleteFile(kTempFile.c_str());
			kCurVersionInfo.Version.i32Tiny = 0;
			++kCurVersionInfo.Version.i16Minor;

			if( !g_FTPMgr.GetFile(PgMergeUtil::VersionToWstr(kCurVersionInfo.Version) + PgMergeUtil::kFileListName, kTempFile) )
			{// 실패하면 현재버전.Major+1 해서 찾아본다
				DeleteFile(kTempFile.c_str());
				//kCurVersionInfo.Version.i16Minor = 0;
				//++kCurVersionInfo.Version.i16Major;

				//if( !g_FTPMgr.GetFile(kVerFileFolder + PgMergeUtil::VersionToWstr(kCurVersionInfo.Version), kTempFile) )
				//{// 실패하면 메세지 출력하고, 종료
				//	DeleteFile(kTempFile.c_str());
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) +std::wstring(L" \"ver_info.dat\""));
					return E_PVC_NOT_FOUND;
				//}
			}
		}

#ifdef _DEBUG
		BM::PgDataPackManager::ConvertToFormattedDataFile(kTempFile);
#endif
		
		BM::FolderHash kServerList;
		{// 다운 받은 VER_INFO.DAT을 읽어와 Server 파일리스트를 담고(폴더명[파일명/데이타 정보])
			BM::PgDataPackManager::PackFileCache_new kFileCache;
			if(!kFileCache.Open(kTempFile))
			{
				return E_PVC_FAIL;
			}
			BM::PgDataPackManager::ReadHeader(kFileCache);
			kServerList.swap( kFileCache.mFolderHash );
		}
		DeleteFile(kTempFile.c_str());

		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DIVLIST));
		if(kServerList.size())
		{
			BM::FolderHash kPackHash, kNonPackHash;
			BM::FolderHash::const_iterator iter = kServerList.begin();
			while( kServerList.end() != iter )
			{// 각 폴더에 담긴
				BM::FolderHash::key_type const& rkDirectory = (*iter).first;
				BM::FolderHash::mapped_type const& rkFileInfo = (*iter).second;

				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}
				
				// 파일 정보가 
				if( rkFileInfo->bIsUsePack )
				{// Pack 되어있으면 PackList에 저장하고
					//PgMergeUtil::MergeRightToLeft(kPackHash, rkDirectory, rkFileInfo);
					kPackHash.insert( std::make_pair(rkDirectory, rkFileInfo) );
				}
				else
				{// UnPack되어 있으면 UnPackList에 저장한다
					//PgMergeUtil::MergeRightToLeft(kNonPackHash, rkDirectory, rkFileInfo);
					kNonPackHash.insert( std::make_pair(rkDirectory, rkFileInfo) );
				}
				++iter;
			}

			// 버전별 파일을 최신으로 정리한다
			PgMergeUtil::Regulate(PackList, kPackHash);
			PgMergeUtil::Regulate(NonPackList, kNonPackHash);

			// 새로운 버전 폴더를 추가
			//std::wstring const wstrVersionPath = PgMergeUtil::VersionToWstr(kCurVersionInfo.Version);
			PackList.insert( std::make_pair(kCurVersionInfo.iVersion, kPackHash) );
			NonPackList.insert( std::make_pair(kCurVersionInfo.iVersion, kNonPackHash) );
		}
	}
#ifdef NO_FTP_LOCAL_TEST
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile2, PgRecoveryTest::pLogFileMode), PackList, L"Server List Pack");
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile2, PgRecoveryTest::pLogFileMode), NonPackList, L"Server List UnPack");
#endif //NO_FTP_LOCAL_TEST
	return	E_PVC_SUCCESS;
}

bool PgHeaderMergePatch::GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList)
{
	std::vector<std::wstring>::iterator		FolderName_iter = g_kPackFolders.begin();
	while(FolderName_iter != g_kPackFolders.end())
	{// Client의 Pack된 폴더 이름에
		if( g_PProcess.CheckEnd() )
		{
			throw eTHROW_CANCEL;
		}
		
		// ".DAT"를 붙이면 각 Pack된 파일명이 되고
		std::wstring	FileName = (*FolderName_iter) + L".dat";

		if(_access(MB(FileName), 0) != -1)
		{// 이 Pack된 파일의 FolderHash 정보를 얻어와 Client PackList에 추가한다
			BM::PgDataPackManager::ConvertToFormattedDataFile(FileName);
			BM::FolderHash	kFolderList;

			BM::PgDataPackManager::PackFileCache_new kFileCache;
			if(!kFileCache.Open(FileName))
			{
				return false;
			}
			// 여기가 시간이 많이 걸림
			BM::PgDataPackManager::ReadHeader(kFileCache);
			kFolderList = kFileCache.mFolderHash;
			PackList.insert(std::make_pair((*FolderName_iter) + L"\\", kFolderList.begin()->second));
		}

		++FolderName_iter;
	}
#ifdef NO_FTP_LOCAL_TEST
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile2, PgRecoveryTest::pLogFileMode), PackList, L"Client List Pack");
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile2, PgRecoveryTest::pLogFileMode), NonPackList, L"Client List UnPack");
#endif //NO_FTP_LOCAL_TEST
	return	true;
}

void PgHeaderMergePatch::Do()
{
	//	이제 비교하는거다
	g_PProcess.SetProcessState(CPatchProcess::eProcess_Comparing);

	// 혹시 버전이 리커버리 시작 버전보다 낮다면...
	if( g_PProcess.GetIsOldVersion() < g_PProcess.GetIsStartVersion() )
	{//수습안댄다. 리커버리 하자.
		throw eTHROW_TRYRECOVERY;
	}

	//	서버 리스트
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_RECVSERVERLIST));
	PATCH_HASH		kServerPackList;
	PATCH_HASH		kServerNonPackList;
	
	PatchVersionCheckResult ret = GetServerList(kServerPackList, kServerNonPackList);
	if (E_PVC_FAIL == ret)
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	if (E_PVC_NOT_FOUND == ret)
	{
		throw eTHROW_TRYRECOVERY;
	}

	//	클라이언트 리스트
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATECLIENTLIST));
	BM::FolderHash		kClientPackList;
	BM::FolderHash		kClientNonPackList;
	if(!GetClientList(kClientPackList, kClientNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	//	비교 결과
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPARELIST));

	// 서버에서 받은 FolderHash 정보에 새로 받아야할 파일이
	if(kServerPackList.size() || kServerNonPackList.size())
	{// 있고
		if(g_PProcess.GetIsListCheck())
		{// 빅만 한다면 비교 완료했으니 업데이트 하라는 메세지를 띄움 (뭐하는 동작이지?)
			g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPLETECOMPAREDOUPDATE));
			g_PProcess.SetTotalPer(1.0f);
			return;
		}
	}
	else
	{// 없다면 게임 플레이 하라는 메세지 띄움
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPLETECOMPAREPLAY));
		g_PProcess.SetTotalPer(1.0f);
		return;
	}
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CALTOTALSIZE));
	
	//	서버에서 받아야할 파일들의 총 크기를 구해서
	size_t	TotalSize = 0;
	TotalSize	+= GetListDataSize(kServerPackList);
	TotalSize	+= GetListDataSize(kServerNonPackList);
	g_PProcess.SetTotalSize(TotalSize);

	//	이제 NonPack 받아서 패치
	g_PProcess.SetProcessState(CPatchProcess::eProcess_NP_Patching);

	//	NonPack Update
	if(!PgNewPatchCoreUtil::NewNonPackUpdate(kServerNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	//	이제 Pack 받아서 패치
	g_PProcess.SetProcessState(CPatchProcess::eProcess_P_Patching);

	//	Pack Update
	if(!PgNewPatchCoreUtil::NewPackUpdate(kServerPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}
}

//────────────────────────────────────────
//	패치
//────────────────────────────────────────
PgRecoveryPatch::PgRecoveryPatch()
{
}
PgRecoveryPatch::~PgRecoveryPatch()
{
}

bool PgRecoveryPatch::GetServerList(PATCH_HASH& PackList, PATCH_HASH& NonPackList)
{
	//std::wstring	FileName	= L"HEADFOLDERS.DAT";
	std::wstring const kTempFile(L"ver_temp.dat");
	DeleteFile(kTempFile.c_str());

	BM::VersionInfo kCurVersionInfo, kNewVersionInfo;
	kCurVersionInfo.iVersion = g_PProcess.GetIsStartVersion();	// 리커버리 시작 버젼
	kNewVersionInfo.iVersion = g_PProcess.GetIsVersion();		// 새 버젼

	while( kNewVersionInfo.iVersion >= kCurVersionInfo.iVersion )
	{// 현재 클라이언트 PATCH_VERSION과 업데이트 서버의 PATCH_VERSION이 다르다면
		// 해당 버전의 폴더/파일 정보를 담고 있는 filelist.dat 를 찾아 VER_TEMP.DAT라는 파일로 저장한다
		if( !g_FTPMgr.GetFile(PgMergeUtil::VersionToWstr(kCurVersionInfo.Version) + PgMergeUtil::kFileListName, kTempFile) )
		{// 실패하면 현재버전.Minor+1해서 찾아본다
			DeleteFile(kTempFile.c_str());
			kCurVersionInfo.Version.i32Tiny = 0;
			++kCurVersionInfo.Version.i16Minor;

			if( !g_FTPMgr.GetFile(PgMergeUtil::VersionToWstr(kCurVersionInfo.Version) + PgMergeUtil::kFileListName, kTempFile) )
			{// 실패하면 현재버전.Major+1해서 찾아본다
				DeleteFile(kTempFile.c_str());
				kCurVersionInfo.Version.i16Minor = 0;
				++kCurVersionInfo.Version.i16Major;

				if( !g_FTPMgr.GetFile(PgMergeUtil::VersionToWstr(kCurVersionInfo.Version) + PgMergeUtil::kFileListName, kTempFile) )
				{// 실패하면 메세지 출력하고, 종료
					DeleteFile(kTempFile.c_str());
					g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) +std::wstring(L" \"filelist.dat\""));
					return false;
					//}
				}
			}
		}

#ifdef _DEBUG
		BM::PgDataPackManager::ConvertToFormattedDataFile(kTempFile);
#endif
		
		BM::FolderHash kServerList;
		{// 다운 받은 VER_INFO.DAT을 읽어와 Server 파일리스트를 담고(폴더명[파일명/데이타 정보])
			BM::PgDataPackManager::PackFileCache_new kFileCache;
			if(!kFileCache.Open(kTempFile))
			{
				return false;
			}
			BM::PgDataPackManager::ReadHeader(kFileCache);
			kServerList.swap( kFileCache.mFolderHash );
		}
		DeleteFile(kTempFile.c_str());

		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DIVLIST));
		if(kServerList.size())
		{
			BM::FolderHash kPackHash, kNonPackHash;
			BM::FolderHash::const_iterator iter = kServerList.begin();
			while( kServerList.end() != iter )
			{// 각 폴더에 담긴
				BM::FolderHash::key_type const& rkDirectory = (*iter).first;
				BM::FolderHash::mapped_type const& rkFileInfo = (*iter).second;

				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}
				
				// 파일 정보가 
				if( rkFileInfo->bIsUsePack )
				{// Pack 되어있으면 PackList에 저장하고
					//PgMergeUtil::MergeRightToLeft(kPackHash, rkDirectory, rkFileInfo);
					kPackHash.insert( std::make_pair(rkDirectory, rkFileInfo) );
				}
				else
				{// UnPack되어 있으면 UnPackList에 저장한다
					//PgMergeUtil::MergeRightToLeft(kNonPackHash, rkDirectory, rkFileInfo);
					kNonPackHash.insert( std::make_pair(rkDirectory, rkFileInfo) );
				}
				++iter;
			}

			// 버전별 파일을 최신으로 정리한다
			PgMergeUtil::Regulate(PackList, kPackHash);
			PgMergeUtil::Regulate(NonPackList, kNonPackHash);

			// 새로운 버전 폴더를 추가
			//std::wstring const wstrVersionPath = PgMergeUtil::VersionToWstr(kCurVersionInfo.Version);
			PackList.insert( std::make_pair(kCurVersionInfo.iVersion, kPackHash) );
			NonPackList.insert( std::make_pair(kCurVersionInfo.iVersion, kNonPackHash) );
		}
		
		//정상적으로 처리 했으므로 버전 올림 Tiny+1
		++kCurVersionInfo.Version.i32Tiny;
	}
#ifdef NO_FTP_LOCAL_TEST
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), PackList, L"Server List Pack");
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), NonPackList, L"Server List UnPack");
#endif //NO_FTP_LOCAL_TEST
	return true;
}

bool PgRecoveryPatch::GetClientList(BM::FolderHash& PackList, BM::FolderHash& NonPackList)
{
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATECLIENTLIST));
	//	클라이언트 NonPack 리스트
	BM::PgDataPackManager::GetFileList(L".\\", NonPackList);

	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATECLIENTLIST));
	//	클라이언트 Pack 리스트
	std::vector<std::wstring>::iterator		FolderName_iter = g_kPackFolders.begin();
	while(FolderName_iter != g_kPackFolders.end())
	{
		if( g_PProcess.CheckEnd() )
		{
			throw eTHROW_CANCEL;
		}

		std::wstring	FileName = (*FolderName_iter) + L".dat";

		if(_access(MB(FileName), 0) != -1)
		{
			BM::PgDataPackManager::ConvertToFormattedDataFile(FileName);
			BM::FolderHash	kFolderList;
			BM::PgDataPackManager::PackFileCache_new kFileCache;
			if(!kFileCache.Open(FileName))
			{
				return false;
			}

			BM::PgDataPackManager::ReadHeader(kFileCache);
			// update CRC of packed files
			// sometimes file contents are corrupted, but header has correct crc, so it doesn't update.

			BM::PgDataPackManager::UpdateFileCRC(kFileCache);

			kFolderList = kFileCache.mFolderHash;

			PackList.insert(std::make_pair((*FolderName_iter) + L"\\", kFolderList.begin()->second));
		}

		++FolderName_iter;
	}
#ifdef NO_FTP_LOCAL_TEST
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), PackList, L"Client List Pack");
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), NonPackList, L"Client List UnPack");
#endif //NO_FTP_LOCAL_TEST
	return	true;
}

bool PgRecoveryPatch::MakeDiffList(PATCH_HASH const& ServerList, BM::FolderHash& ClientList, PATCH_HASH& ResultList)
{
	std::wstring const kTempFile(L"ver_temp.dat");
	DeleteFile(kTempFile.c_str());

	BM::VersionInfo kStartVersionInfo, kNewVersionInfo;
	kStartVersionInfo.iVersion = g_PProcess.GetIsStartVersion();	// 리커버리 시작 버젼
	kNewVersionInfo.iVersion = g_PProcess.GetIsVersion();		// 최신 버젼

	while( kNewVersionInfo.iVersion >= kStartVersionInfo.iVersion )
	{//최신버전에서부터 검색한다
		//std::wstring const wstrVersionPath = PgMergeUtil::VersionToWstr(kNewVersionInfo.Version);
		PATCH_HASH::const_iterator iter = ServerList.find(kNewVersionInfo.iVersion);
		if( ServerList.end() != iter )
		{
			PATCH_HASH::mapped_type const & rkServerFolderHash = (*iter).second;
			BM::FolderHash kTempResultList;
			if( BM::PgDataPackManager::MakeDiffList(ClientList, rkServerFolderHash, kTempResultList, BM::DIFF_ONLY_RIGHT | BM::DIFF_ONLY_DIFF) )
			{
				ResultList.insert( std::make_pair((*iter).first, kTempResultList) );
			}
		}

		--kNewVersionInfo.Version.i32Tiny;
		if( 0 >= kNewVersionInfo.Version.i32Tiny )
		{//i32Tiny 값이 음수가 될 수 없음. i16Minor 를 1빼줌
			kNewVersionInfo.Version.i32Tiny = 0;
			--kNewVersionInfo.Version.i16Minor;
		}
		
	}
	return true;
}

void PgRecoveryPatch::OptimizeList(PATCH_HASH& rkList)
{
	if( rkList.empty() )
	{
		return;
	}
	PATCH_HASH::reverse_iterator lhs_iter = rkList.rbegin(); // 가장 최신 버젼부터
	while( rkList.rend() != lhs_iter )
	{
		PATCH_HASH::reverse_iterator rhs_iter = lhs_iter;
		++rhs_iter;
		while( rkList.rend() != rhs_iter ) // 다음 버젼을 Right hand side에 둔다
		{
			BM::FolderHash& rkLhsFolder = (*lhs_iter).second;
			BM::FolderHash::const_iterator lhs_folder_iter = rkLhsFolder.begin();
			while( rkLhsFolder.end() != lhs_folder_iter )
			{
				BM::FolderHash& rkRhsFolder = (*rhs_iter).second;
				BM::FolderHash::iterator rhs_folder_iter = rkRhsFolder.find( (*lhs_folder_iter).first ); // 같은 폴더를 찾는다
				if( rkRhsFolder.end() != rhs_folder_iter )
				{
					BM::PgFolderInfo const& rkLhsFile = *(*lhs_folder_iter).second;
					BM::PgFolderInfo& rkRhsFile = *(*rhs_folder_iter).second;

					BM::FileHash::const_iterator lhs_file_iter = rkLhsFile.kFileHash.begin();
					while( rkLhsFile.kFileHash.end() != lhs_file_iter )
					{
						rkRhsFile.kFileHash.erase( (*lhs_file_iter).first ); // 같은 폴더 내에 같은 파일이 있으면 Right Hand Side의 내용을 제거
						++lhs_file_iter;
					}

					// 제거후 비어있으면 폴더 제거
					if( rkRhsFile.kFileHash.empty() )
					{
						rkRhsFolder.erase( rhs_folder_iter );
					}
				}
				++lhs_folder_iter;
			}

			++rhs_iter;
		}
		++lhs_iter;
	}
	// 제거후 아무 폴더 없으면 버젼 제거
	PATCH_HASH::iterator iter = rkList.begin(); // 가장 최신 버젼부터
	while( rkList.end() != iter )
	{
		if( (*iter).second.empty() )
		{
			iter = rkList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void PgRecoveryPatch::Do()
{
	//	이제 비교하는거다
	g_PProcess.SetProcessState(CPatchProcess::eProcess_Comparing);

#ifdef _DEBUG
	DWORD time = ::timeGetTime();
	DWORD time2;
	BM::vstring log;
#endif
	//	서버 리스트
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_RECVSERVERLIST));
	PATCH_HASH		kServerPackList;
	PATCH_HASH		kServerNonPackList;
	if(!GetServerList(kServerPackList, kServerNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	// 서버 목록 패치버젼을 기준으로 최적화(같은 파일중 구버젼 파일은 버린다)
	OptimizeList(kServerPackList);
	OptimizeList(kServerNonPackList);

#ifdef _DEBUG
	time2 = (::timeGetTime() - time);
	time = ::timeGetTime();
	log = L"Get ServerList Elasped : ";
	log += (int)(time2/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif
	//	클라이언트 리스트
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CREATECLIENTLIST));
	BM::FolderHash		kClientPackList;
	BM::FolderHash		kClientNonPackList;
	if(!GetClientList(kClientPackList, kClientNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

#ifdef _DEBUG
	time2 = (::timeGetTime() - time);
	time = ::timeGetTime();
	log = L"Get ClienList Elasped : ";
	log += (int)(time2/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif

	//	비교 결과
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPARELIST));
	PATCH_HASH		kResultPackList;
	PATCH_HASH		kResultNonPackList;
	if( !MakeDiffList(kServerPackList, kClientPackList, kResultPackList)
		|| !MakeDiffList(kServerNonPackList, kClientNonPackList, kResultNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

#ifdef NO_FTP_LOCAL_TEST
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), kResultPackList, L"Result Pack");
	WriteToFile( PgRecoveryTest::LogFile(PgRecoveryTest::pLogFile, PgRecoveryTest::pLogFileMode), kResultNonPackList, L"Result NoPack");
#endif //NO_FTP_LOCAL_TEST

#ifdef _DEBUG
	time2 = (::timeGetTime() - time);
	time = ::timeGetTime();
	log = L"MakeDiffList Elasped : ";
	log += (int)(time2/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif

	if(kResultPackList.size() || kResultNonPackList.size())
	{
		//	리스트만 비교한다.
		if(g_PProcess.GetIsListCheck())
		{
			g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPLETECOMPAREDOUPDATE));
			g_PProcess.SetTotalPer(1.0f);
			return;
		}
	}
	else
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPLETECOMPAREPLAY));
		g_PProcess.SetTotalPer(1.0f);
		return;
	}

	//	총 크기 계산
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CALTOTALSIZE));
	size_t	TotalSize = 0;
	TotalSize	+= GetListDataSize(kResultPackList);
	TotalSize	+= GetListDataSize(kResultNonPackList);
	g_PProcess.SetTotalSize(TotalSize);

	//	이제 NonPack 받아서 패치
	g_PProcess.SetProcessState(CPatchProcess::eProcess_NP_Patching);

	//	NonPack Update
	if(!PgNewPatchCoreUtil::NewNonPackUpdate(kResultNonPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	//	이제 Pack 받아서 패치
	g_PProcess.SetProcessState(CPatchProcess::eProcess_P_Patching);

	//	Pack Update
	if(!PgNewPatchCoreUtil::NewPackUpdate(kResultPackList))
	{
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

#ifdef _DEBUG
	time2 = (::timeGetTime() - time);
	time = ::timeGetTime();
	log = L"PackUpdate Elasped : ";
	log += (int)(time2/1000);
	log += L"s\n";
	OutputDebugString(log);
#endif
}

#ifdef USE_TORRENT
void PgTorrentPatch::Do()
{
	#define UPDATE_ID_STR "update.id"
	static const std::wstring UPDATE_ID = _T(UPDATE_ID_STR);
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_PATCHINGFILE) + UPDATE_ID);
	if(!g_FTPMgr.GetFile(UPDATE_ID, UPDATE_ID))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + UPDATE_ID);
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	if (!libupdate::torrent::init(UPDATE_ID_STR))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILED_INIT_LIBUPDATE) + UPDATE_ID);
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}
	#undef UPDATE_ID_STR

	g_PProcess.SetProcessState(CPatchProcess::eProcess_P_Patching);
	g_PProcess.SetTotalPer(0.f);
	libupdate::torrent::resume();
	libupdate::torrent::state_t state;
	do
	{
		state = libupdate::torrent::state();
		switch (state)
		{
		case libupdate::torrent::state_t::failed:
			{
				throw eTHROW_FAILED;
			}break;

		case libupdate::torrent::state_t::downloading_metadata:
		case libupdate::torrent::state_t::downloading:
			{
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_LIBUPDATE_DOWNLOAD));
				g_PProcess.SetTotalPer(libupdate::torrent::progress());
				libupdate::torrent::resume();
			}break;

		case libupdate::torrent::state_t::checking_files:
		case libupdate::torrent::state_t::checking_resume_data:
		case libupdate::torrent::state_t::queued_for_checking:
			{
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_LIBUPDATE_CHECK));
				g_PProcess.SetTotalPer(libupdate::torrent::progress());
			}break;

		case libupdate::torrent::state_t::allocating:
			{
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_LIBUPDATE_ALLOCATING));
				g_PProcess.SetTotalPer(libupdate::torrent::progress());
			}break;

		case libupdate::torrent::state_t::seeding:
		case libupdate::torrent::state_t::finished:
			{
				return;
			}break;
		}
		Sleep(50);
	} while(true);
}
#endif
