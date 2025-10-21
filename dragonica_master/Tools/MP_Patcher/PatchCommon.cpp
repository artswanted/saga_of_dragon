#include	"PatchCommon.h"
#include	"AmendPatch.h"
#include	"OriginalPatch.h"
#ifdef PATCHER_DEBUG
#include	"BM/ExceptionFilter.h"
#endif

bool m_gUpdatedPatcher = false;
#ifdef PATCHER_DEBUG_LOG
BM::PgDebugLog g_kLog;
#endif

std::vector<std::wstring>	g_kPackFolders;

unsigned __stdcall OnlinePatch(void* pArgument)
{
	PgPatcherRscUtil::ClearTempRsc();

//#ifdef PATCHER_DEBUG
//	PgExceptionFilter::Install(PgExceptionFilter::DUMP_LEVEL_HEAVY);
//#endif

	try
	{
		g_PProcess.SetProcessState(CPatchProcess::eProcess_Connecting);

		if( g_PProcess.GetLocal() == CPatchProcess::LS_CHINA && !g_PProcess.GetIsRecover() )
		{
			if(_access(".\\patch_new.id", 0) == -1)
			{
				exit(0);
			}
		}

		ConnectServer();
		if( !m_gUpdatedPatcher )
		{
			PatcherCheck();
			m_gUpdatedPatcher = true;
		}
		PackFolderInit();

		//	패치하자
		const bool bIsFileExist = (_access(".\\patch.id", 0) == -1)?(false):(true);
		bool const bIsForceRecoveryMode = (g_PProcess.GetIsOldVersion() < g_PProcess.GetMinimumVersion()) ? (true) : (false);

		if(g_PProcess.GetIsRecover() || !bIsFileExist || bIsForceRecoveryMode)
		{//	리커버리
			RecoveryPatchCore();
		}
		else
		{
			g_PProcess.SetNotPatched(false);
			switch(g_PProcess.GetLocal())
			{
				case CPatchProcess::LS_KOREA:
					{
						if (g_PProcess.GetUseNewPatchMethod() == true)
						{
							NewPatchMethod();
						}
						else
						{
							//	패치하자
							if(g_PProcess.GetIsListCheck())
							{
								g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_STARTPATCH));
								RecoveryPatchCore();
							}
							else
							{
								if( PgPatchCoreUtil::OriginalPatchIDCheck() )
								{
									PgRecoveryPatch().Do();
								}
								g_PProcess.SetTotalPer(1.0f);
							}
						}
					}break;
				case CPatchProcess::LS_SINGAPORE:
				case CPatchProcess::LS_AMERICA:
				case CPatchProcess::LS_THAILAND:
				case CPatchProcess::LS_RUSSIA:
				case CPatchProcess::LS_VIETNAM:
				case CPatchProcess::LS_TAIWAN:
				case CPatchProcess::LS_EUROPE:
				case CPatchProcess::LS_FRANCE:
				case CPatchProcess::LS_GERMANY:
				case CPatchProcess::LS_JAPAN:
					{
						NewPatchMethod();
					}break;
				default:
					{
						if( PgPatchCoreUtil::OriginalPatchIDCheck() )
						{
							PgRecoveryPatch().Do();
						}
						g_PProcess.SetTotalPer(1.0f);
					}
				break;
			}
		}

		if( g_PProcess.GetNotPatched() )
		{
			throw eTHROW_CANCEL;
		}

		throw eTHROW_SUCCESS;
	}
	catch(ETHROW_TYPE Result)
	{
		bool bStartRecoveryMode = false;
		g_FTPMgr.Clear();

		switch( Result )
		{
		case eTHROW_SUCCESS:
			{
				std::wstring kText;
				DeleteFile(L"patch.id");
				DeleteFile(L"patch_mn.ID");
				rename("patch_new.id", "patch.id");

				PgPatcherRscUtil::ReplaceRsc();
				UpdatePackVer();

				HWND	hWnd = g_PProcess.GetVisibleWindowHandle();
				if(g_PProcess.GetLocal() == CPatchProcess::LS_CHINA)
				{
					if( g_PProcess.GetPopPatchDlg() )
					{
						hWnd = g_PProcess.GetPopPatchDlg()->GetHandle();
					}
				}
				
				//패치 완료되어 Patcher에 보여줄 Patch.ID ver를 보여준다(client ver:)
				if( LoadPatchIDVerFromFile() )
				{// 새버전일 경우만 패치 메세지 보여줌
					WinMessageBox(hWnd, g_PProcess.GetMsg(E_COMPLETEPATCH), WSTR_GAME_NAME.c_str(), MB_OK);
				}
				g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_COMPLETEPATCH));

				if(g_PProcess.GetLocal() == CPatchProcess::LS_CHINA)
				{
					CHSOpenLuncher();
					if( g_WinApp )
					{
						g_WinApp->Display(SW_HIDE);
						SendMessage(g_WinApp->GetHandle(), WM_DESTROY, 0, 0);
					}
				}
				if(g_PProcess.GetIsAuto())
				{
					exit(0);
				}				
			}break;
		case eTHROW_FAILED:
		case eTHROW_TRYRECOVERY:
			{
				g_PProcess.SetNotPatched(true);
				DeleteFile(L"patch_new.id");
				DeleteFile(L"temp.ftp");
				DeleteFile(L"PATCH.TMP");
				PgPatcherRscUtil::ClearTempRsc();

				HWND	hWnd = g_PProcess.GetVisibleWindowHandle();
				if(g_PProcess.GetLocal() == CPatchProcess::LS_CHINA)
				{
					if( g_PProcess.GetPopPatchDlg() )
					{
						hWnd = g_PProcess.GetPopPatchDlg()->GetHandle();
					}
				}
				
				if (Result == eTHROW_TRYRECOVERY)
				{
					WinMessageBox(hWnd, g_PProcess.GetMsg(E_FAILEDTOPATCH_AND_TRY_RECOVERY), WSTR_GAME_NAME.c_str(), MB_OK);
					bStartRecoveryMode = true;
					g_PProcess.SetNotPatched(false);
				}
				else
				{
					WinMessageBox(hWnd, g_PProcess.GetMsg(E_FAILEDTOPATCH), WSTR_GAME_NAME.c_str(), MB_OK);
					g_PProcess.SetProcessState(CPatchProcess::eProcess_Preparing);
				}				
			}break;
		case eTHROW_CANCEL:
			{
				if(g_PProcess.GetIsRecover())
				{// Recover 모드를 눌렀다가 취소 했다면 게임 Start시 전체 체크하지 않게 수정함
					g_PProcess.SetIsRecover(false);
				}
				else
				{
					g_PProcess.SetNotPatched(true);
				}
				g_PProcess.SetEnd(false);
				DeleteFile(L"patch_new.id");
				DeleteFile(L"temp.ftp");
				PgPatcherRscUtil::ClearTempRsc();

				HWND	hWnd = g_PProcess.GetVisibleWindowHandle();
				if(g_PProcess.GetLocal() == CPatchProcess::LS_CHINA)
				{
					if( g_PProcess.GetPopPatchDlg() )
					{
						hWnd = g_PProcess.GetPopPatchDlg()->GetHandle();
					}
				}
				if (false == g_PProcess.IsDestroying())
				{
					WinMessageBox(hWnd, g_PProcess.GetMsg(E_PATCHCANCELED), WSTR_GAME_NAME.c_str(), MB_OK);
				}
			}break;
		}

		BM::PgDataPackManager::Clear();
		g_PProcess.SetProcessState(CPatchProcess::eProcess_Preparing);
		if(g_PProcess.GetLocal() == CPatchProcess::LS_TAIWAN)
		{
			g_PProcess.SubDisplay(false);
			g_WinApp->Display(SW_SHOW);
		}

		if (bStartRecoveryMode)
		{
			::SendMessage(g_PProcess.GetVisibleWindowHandle(), WM_RUN_RECOVERYMODE, MAKEWPARAM(CCTButton::eBTN_RECOVERY, 0), 0);
		}

		return	false;

	}

	return	true;
}

void	CHSOpenLuncher()
{
	if(g_PProcess.GetLocal() != CPatchProcess::LS_CHINA) return;

	TCHAR szPath[MAX_PATH];
	HRESULT hModuleName = GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	std::wstring strPath = szPath;
	strPath = strPath.substr(0,strPath.rfind(_T('\\')));
	SetCurrentDirectory(strPath.c_str());
	std::wstring strFileName = strPath + _T("\\launcher.exe");

	HWND handle = 0;
	HINSTANCE kIns = ShellExecute(handle, L"open", strFileName.c_str(), L"", strPath.c_str(), SW_SHOWNORMAL);
}

//────────────────────────────────────────
//	시스템 로그
//────────────────────────────────────────
void InitLogReport()
{
#ifdef PATCHER_DEBUG_LOG
	g_kLog.Init(BM::OUTPUT_JUST_FILE, BM::LFC_WHITE, _T("./Log/"), _T("INFO_LOG"));
	g_kLog.SetLogLevelLimit((BM::E_LOG_LEVEL)9);
#endif

	//SYSTEMTIME sysTime;
	//GetLocalTime(&sysTime);
	//TCHAR chDir[20];
	//_stprintf_s(chDir, 20,_T("Log%d%d%d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	//INIT_LOG2(chDir);

	//INFO_LOG( BM::LOG_LV7, _T("------------------------------"));
	//INFO_LOG( BM::LOG_LV7, _T("log_%04u_%02u_%02u_%02u_%02u_%02u"), 
	//		  sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	//INFO_LOG( BM::LOG_LV7, _T("------------------------------"));
}

//────────────────────────────────────────
//	패치 - 공통
//────────────────────────────────────────
//	파일 옵션 정보
void	PatchOptionINI()
{
	//	팩 옵션을 읽고
	BM::CPackInfo	PackInfo;
	if(!PackInfo.Init())
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDLOADINI));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	//	매니져롤 초기화
	if(!BM::PgDataPackManager::Init(PackInfo))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L" \"patcher.id\""));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}
}

//	팩폴더 정보
void	PackFolderInit()
{
	g_kPackFolders.clear();
	g_kPackFolders.push_back(L".\\xml");
	g_kPackFolders.push_back(L".\\script");
	g_kPackFolders.push_back(L".\\bgm");
	if( CPatchProcess::LS_TAIWAN != g_PProcess.GetLocal() )
	{
		g_kPackFolders.push_back(L".\\font");
	}
	g_kPackFolders.push_back(L".\\data\\1_cha");
	g_kPackFolders.push_back(L".\\data\\2_mon");
	g_kPackFolders.push_back(L".\\data\\2_mon2");
	g_kPackFolders.push_back(L".\\data\\3_world");
	g_kPackFolders.push_back(L".\\data\\3_world2");
	g_kPackFolders.push_back(L".\\data\\4_item");
	g_kPackFolders.push_back(L".\\data\\5_effect");
	g_kPackFolders.push_back(L".\\data\\6_ui");

	BM::CPackInfo kPackInfo;//Init
	if(!kPackInfo.Init())
	{
		assert(NULL);
	}
	BM::PgDataPackManager::Init(kPackInfo);
}

//	패쳐 체크
void PatcherCheck()
{
	if(!g_FTPMgr.GetFile(_T("patcher.id"), _T(".\\patcher.id")))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE) + std::wstring(L" \"patcher.id\""));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}

	std::vector<char> vecNewCRC;
	vecNewCRC.resize(PG_PATCHER_CRC_SIZE);
	BM::DecLoad(_T(".\\patcher.id"), vecNewCRC);

	if( !vecNewCRC.size() )
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_BROKENCRC));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}
	DeleteFile(_T("patcher.id"));

	if(::memcmp(&g_PProcess.GetCRC().at(0), &vecNewCRC.at(0), PG_PATCHER_CRC_SIZE) != 0)
	{
		PatcherPatch();
	}
}

//	패쳐 업데이트
void PatcherPatch()
{
	if (g_PProcess.GetUsePatcherPatch() == false)
		return;

	if( CPatchProcess::LS_TAIWAN == g_PProcess.GetLocal() )
	{
		g_PProcess.SubDisplay(true);
		g_WinApp->Display(SW_HIDE);
	}

	if (!g_FTPMgr.GetFile(_T("patcher.exe"), _T(".\\patcher2.exe"))) //다운.
	{
		g_PProcess.SetWorkMsg(std::wstring(L"\"patcher.exe\" ") + g_PProcess.GetMsg(E_FAILEDDOWNLOADFILE));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw eTHROW_FAILED;
	}

	// 실행 파일 이름을 구한다.
	wchar_t Path[512];
	GetModuleFileName(GetModuleHandle(0), Path, MAX_PATH);

	if (wcslen(Path) == 0)
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDCALLPATCHERPATCH));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw eTHROW_FAILED;
	}
	
	std::wstring wstrFolder;
	std::wstring wstrFile;
	BM::DivFolderAndFileName(Path,wstrFolder,wstrFile);

	// Patcher Patch 실행
	HWND hWnd = 0;
	HINSTANCE kIns = ShellExecute(hWnd, _T("open"), _T("patcherpatch.exe"),
								  0, wstrFolder.c_str(), SW_SHOWNORMAL);
	exit(0);
}

size_t	GetListDataSize(PATCH_HASH const& kPatchHash)
{
	if( kPatchHash.empty() )
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILELISTEMPTY));
		return 0;
	}

	size_t	TotalSize = 0;
	PATCH_HASH::const_iterator	iter = kPatchHash.begin();
	while( kPatchHash.end() != iter )
	{
		BM::FolderHash const& List = (*iter).second;
		TotalSize  += GetListDataSize(List);
		++iter;
	}
	
	if( !TotalSize )
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILELISTEMPTY));
	}

	return TotalSize;
}

//	리스트의 데이타 사이즈의 합
size_t	GetListDataSize(BM::FolderHash const& List, EREQUESTTARGET const Value)
{
	if(!List.size())
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FILELISTEMPTY));
		return	0;
	}

	size_t	TotalSize = 0;

	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CALULRATIONGFILESIZE));
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
			while(File_iter != (*iter).second->kFileHash.end())
			{
				if( g_PProcess.CheckEnd() )
				{
					throw eTHROW_CANCEL;
				}

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

//	서버 연결
void	ConnectServer()
{
	//	패쳐 세팅
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_INITPATCHER));
	PatchOptionINI();

	//	패치서버 연결
	g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_CONECTINGSERVER));
	if(!g_FTPMgr.Connect(g_PProcess.GetAddr().c_str(), g_PProcess.GetPort(), 
						 g_PProcess.GetID().c_str(), g_PProcess.GetPW().c_str()))
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_FAILEDCONNECTSERVER));
		INFO_LOG( BM::LOG_LV5, _T("[%s]-[%d] throw failed\n\r"), _T(__FUNCTION__), __LINE__);
		DEBUG_BREAK;
		throw	eTHROW_FAILED;
	}
}

//────────────────────────────────────────
//	기타 - 공통
//────────────────────────────────────────
//	암호화 해제
bool DecFile(size_t const FileOrgSize, size_t const FileZipSize, bool const bIsCompressed, bool const bIsEncrypted, std::vector<char> const& vecData, std::vector<char>& vecOut)
{
	return BM::DecLoadFromMem(FileOrgSize, FileZipSize, bIsCompressed, bIsEncrypted, vecData, vecOut);

}

//	압축 해제
bool UnCompFile(size_t const& OriginalSize, std::vector<char>& vecData, std::vector<char>& vecOut)
{
	vecOut.resize(OriginalSize);
	return BM::UnCompress(vecOut, vecData);	// TODO: 불리는가? 안불리면 지우거나, 불렸을때의 테스트 필요.
}

//	패치ID 절단
BM::VersionInfo	GetPatchID(std::wstring const& strFileName)
{
	std::vector<char>	vec_PatchID;	//암호화된 ID
	BM::VersionInfo kVersionInfo;
	kVersionInfo.iVersion = 0;

	std::wstring	wstrClient = L"";
	std::wstring	wstrCount = L"";
	std::wstring	wstrLimit = L"";

	typedef	std::list<std::wstring>	CUT_STRING;

	BM::FileToMem(strFileName, vec_PatchID);

	if(vec_PatchID.size())
	{
		g_PProcess.SetWorkMsg(g_PProcess.GetMsg(E_DECODINGPATCHID));
		BM::SimpleDecrypt(vec_PatchID, vec_PatchID);
		vec_PatchID.push_back('\0');

		//	패치넘버만 잘라야지 이제
		char*	pIDSrc	= new char[vec_PatchID.size()];
		memcpy(pIDSrc, &vec_PatchID.at(0), vec_PatchID.size());
		std::wstring	wstrID = UNI(pIDSrc);
		delete [] pIDSrc;


		CUT_STRING	kNumberList;
		BM::vstring::CutTextByKey(wstrID, std::wstring(L"."), kNumberList);

		if( kNumberList.size() == 3 )
		{
			wstrClient = kNumberList.begin()->c_str(); kNumberList.pop_front();
			wstrCount = kNumberList.begin()->c_str(); kNumberList.pop_front();
			wstrLimit = kNumberList.begin()->c_str(); kNumberList.pop_front();

			kVersionInfo.Version.i16Major = _wtoi64(wstrClient.c_str());
			kVersionInfo.Version.i16Minor = _wtoi64(wstrCount.c_str());
			kVersionInfo.Version.i32Tiny = _wtoi64(wstrLimit.c_str());
		}
		//	리스트가 있으면 그중 맨 끝에꺼 한개가 ID다
		return	kVersionInfo;
	}
	
	return kVersionInfo;
}


std::wstring	GetClientVer(std::wstring const& PatchID)
{
	if(!PatchID.size()){ return L""; }

	typedef	std::list<std::wstring>	CUT_STRING;

	//	자르는 기준은 '.' 이다
	CUT_STRING	kNumberList;
	BM::vstring::CutTextByKey(PatchID, std::wstring(L"."), kNumberList);
	
	//	없다면 잘못된거지
	if(!kNumberList.size())
	{
		return	L"";
	}

	return kNumberList.begin()->c_str();
}
std::wstring	GetPatchNumber(std::wstring const& PatchID, bool const bIsOffline, int const nPos)
{
	if(!PatchID.size()){ return L""; }

	typedef	std::list<std::wstring>	CUT_STRING;

	//	자르는 기준은 '.' 이다
	CUT_STRING	kNumberList;
	BM::vstring::CutTextByKey(PatchID, std::wstring(L"."), kNumberList);
	
	//	없다면 잘못된거지
	if(!kNumberList.size())
	{
		return	L"";
	}

	//	수동패치는 리미트와 맥스를 가지지
	CUT_STRING	kPatchNumList;
	std::wstring const kNumStr(kNumberList.rbegin()->c_str());
	BM::vstring::CutTextByKey(kNumStr, std::wstring(L"|"), kPatchNumList);

	//	없거나 2보다 크면 잘못된거지
	if(!kPatchNumList.size() || kPatchNumList.size() > 2)
	{
		return	L"";
	}

	//	오프라인 패치ID면
	if(bIsOffline)
	{
		//	좀더 분기 
		if(nPos == 0)		{ return kPatchNumList.begin()->c_str(); }
		else if(nPos == 1)	{ return kPatchNumList.rbegin()->c_str(); }
		else				{ return L""; }
	}
	else
	{
		return kPatchNumList.begin()->c_str();
	}

}

bool	CreateIDFile(std::wstring const& wstrID)
{
	//	아이디 저장한다
	if(!wstrID.size())
	{
		return	false;
	}

	std::string			strUpdateID = MB(wstrID);
	std::vector<char>	vec_UpdateID;

	vec_UpdateID.resize(strUpdateID.size());
	::memcpy(&vec_UpdateID.at(0), strUpdateID.c_str(), strUpdateID.size());

	BM::EncSave(L"patch_new.id", vec_UpdateID);
	
	return	true;
}

bool	MergeList(BM::FolderHash const& PatchList, BM::FolderHash& OrgList, BM::FolderHash& kResultList)
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
			if( g_PProcess.CheckEnd() )
			{
				throw eTHROW_CANCEL;
			}

			//	폴더를 찾아라
			BM::FolderHash::iterator	Result_iter = kResultList.find((*Patch_iter).first);

			//	찾았냐?
			if(Result_iter != kResultList.end())
			{
				//	파일을 검사하자.
				BM::FileHash::const_iterator	Patch_file_iter = (*Patch_iter).second->kFileHash.begin();
				while(Patch_file_iter != (*Patch_iter).second->kFileHash.end())
				{
					if( g_PProcess.CheckEnd() )
					{
						throw eTHROW_CANCEL;
					}

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

		return	true;
	}

	return	false;
}

bool	UpdatePackVer()
{
	BM::VersionInfo kVersionInfo;
	kVersionInfo.iVersion = g_PProcess.GetIsVersion();

	std::vector<std::wstring>::iterator iter = g_kPackFolders.begin();
	while (iter != g_kPackFolders.end())
	{
		std::wstring wstrFileName = *iter;
		wstrFileName += L".dat";
		BM::PgDataPackManager::UpdatePackVersion(wstrFileName, kVersionInfo);
		++iter;
	}

	return	true;
}

void NewPatchMethod()
{
	if( PgPatchCoreUtil::OriginalPatchIDCheck() )
	{
		if( IDYES == WinMessageBox(g_PProcess.GetVisibleWindowHandle(), g_PProcess.GetMsg(E_WANTTOUPDATE), L"Notice", MB_YESNO) )
		{
			if (g_PProcess.GetLocal() == CPatchProcess::LS_TAIWAN)
			{
				g_PProcess.SubDisplay(true);
				g_WinApp->Display(SW_HIDE);
			}

#ifdef USE_TORRENT
			if (g_PProcess.GetUseLibUpdate() || g_PProcess.GetIsOldVersion() == 0)
			{
				// Download a full client via torrent for speedup
				PgTorrentPatch().Do();
			}
			else
#endif
			{
				PgHeaderMergePatch().Do(); // 실제 패치
			}
		}
		else
		{
			g_PProcess.SetNotPatched(true);
		}
		g_PProcess.SetTotalPer(1.0f);
	}
}