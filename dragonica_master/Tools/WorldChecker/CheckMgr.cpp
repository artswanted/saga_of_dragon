#include	"defines.h"
#include	"CheckMgr.h"



void CALLBACK OnRegist(const CEL::SRegistResult &rkArg)
{
}

void CCheckMgr::ContInit()
{
	m_kXmlPathContainer.clear();	
	m_kGsaPathList.clear();
	m_kNifPathList.clear();

	m_kFolderXmlPathListContainer.clear();
	m_kFolderGsaPathListContainer.clear();
	m_kFolderNifPathListContainer.clear();

	m_kFolderXmlPathContainer.clear();
	m_kFolderGsaPathContainer.clear();
	m_kFolderNifPathContainer.clear();

	FileHashList.clear();
}

void CCheckMgr::HandleMessage(MSG *rkMsg)
{
	if(rkMsg)
	{
		switch(rkMsg->eType)
		{
		case LT_INIT:
			{
				ContInit();
				//	팩 옵션을 읽고
				BM::CPackInfo	kPackInfo;
				if(!kPackInfo.Init())
				{
					g_Core.AddListItem(std::wstring(L"PackInfo Init Failed"), CHK_SYSTEM);
				}

				//	매니져롤 초기화
				if(!BM::PgDataPackManager::Init(kPackInfo))
				{
					g_Core.AddListItem(std::wstring(L"PackManager Init Failed"), CHK_SYSTEM);
				}
			}break;
		case LT_XMLPATH:
			{
				LoadXmlPath();		
			}
			break;
		case LT_GSAPATH:
			{	
				LoadGsaPath();
			}
			break;
		case LT_NIFPATH:
			{
				LoadNifPath();
			}
			break;
		case LT_COMPARE:
			{
				CompareElementsFile();
			}break;
		case LT_QUIT:
			{
				std::wstring	wstrMsg = L"";
				wstrMsg += L"----------------- 검사 완료!!! -----------------";
				g_Core.AddListItem(wstrMsg, CHK_SYSTEM);

				HWND hWndBtn;
				hWndBtn  = GetDlgItem(g_hDlgWnd, IDC_CMB_KIND);
				EnableWindow(hWndBtn, TRUE);
				hWndBtn  = GetDlgItem(g_hDlgWnd, IDC_BUTTON_SELECT);
				EnableWindow(hWndBtn, TRUE);
			}break;
		case LT_FOLDER_LIST:
			{
				LoadFolder_List(rkMsg->wstrPath);
			}break;
		case LT_COMPARELIST:
			{
				CompareListElementFiles();
			}break;
		case LT_NIFEFFECT:
			{
				LoadXmlEffect(rkMsg->wstrPath);
			}break;
		case LT_COMPARE_EFFECT:
			{
				CompareElementsFile_Effect();
			}break;
		}
	}
}

//-----------------------------------------------------------------------------------------------------
//	PATH 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadXmlPath()
{
	g_Core.AddListItem(std::wstring(L"xml 목록을 로드중..."), CHK_SYSTEM);


	g_kTblDataMgr.LoadDump(WSTR_TABLE_FILE);

	const CONT_DEFMAP* pContDefMap = NULL;
	g_kTblDataMgr.GetContDef(pContDefMap);	

	size_t		stListCount = 0;

	CONT_DEFMAP::const_iterator map_itor = pContDefMap->begin();				

	while(map_itor != pContDefMap->end())
	{
		const TBL_DEF_MAP* pMap = &(*map_itor).second;

		const std::wstring& wstrPath = pMap->strXMLPath;

		std::string strPath = "";
		strPath += MB(wstrPath);

		std::wstring kTextPath = UNI(strPath);

		std::string headerString = MB(kTextPath);

		std::string::size_type index =  headerString.find_first_of('/');
		while (index != std::string::npos)
		{
			headerString.replace(index, 1, "\\");
			index = headerString.find_first_of('/', index);
		}

		kTextPath = UNI(headerString);


		std::wstring	wStrFolder = g_Core.GetDevFolder() + L"\\";
		kTextPath = wStrFolder + L"XML\\" + kTextPath;


		std::string uprStr;

		uprStr = MB(kTextPath);
		UPR(uprStr);			
		kTextPath = UNI(uprStr);


		std::wstring	wstrMsg = L"";

		std::wstring	wsFile	= kTextPath;
		std::wstring	wsText	= L"파일 xml 리스트에 추가중...";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_LOG);

		wstrMsg = wsFile + wsText;


		LogSaveFile(LT_XMLPATH, kTextPath);

		char temp[100];
		sprintf(temp, "%d", stListCount++);
		std::wstring kID = UNI(temp);

		m_kXmlPathContainer.insert(std::make_pair(kID, kTextPath));
		m_kFolderXmlPathListContainer.insert(std::make_pair(kTextPath, L"XML"));


		++map_itor;					
	}	
}

bool CCheckMgr::LoadXmlPathFile(TiXmlDocument &rkXmlDoc, const std::wstring &rkFileName)
{//	TiXmlDocument 이 객체를 받아서.. 사용을 하는게야.
	//	rkXmlDoc.SetUserData(MB(rkFileName));
	if (rkFileName.empty())
	{
		NILOG(PGLOG_WARNING, "[PgXmlLoader] filename is empty\n");
		return false;
	}

	//if(g_bUsePackData)
	{
#ifndef EXTERNAL_RELEASE
		//if (g_bFindFirstLocal)
		{
			// UsePack이더라도... Local에서 먼저 찾는다. 그런데 못찾으면 Pack에서 찾는다.
			std::string strPath = "";
			strPath += MB(rkFileName);
			if (rkXmlDoc.LoadFile(strPath.c_str()))
			{
				return true;
			}
		}
#endif
		std::vector< char > data(0);
		if(!BM::PgDataPackManager::LoadFromPack(_T("./xml.dat"), (std::wstring)(_T("./"))+rkFileName, data))
		{
			NILOG(PGLOG_ERROR, "[PgXmlLoader] LoadFromPack failed, %s xml parse failed\n",MB(rkFileName));
			return false;
		}

		if(!data.size())
		{
			NILOG(PGLOG_ERROR, "[PgXmlLoader] failed loading from Packed Data.\n");
			return false;
		}

		int iDataSize = data.size();
		data.push_back('\0');	// 파싱이 제대로 끝나는 것을 보장하기 위해서.

		try {
			rkXmlDoc.Parse(&data.at(0));
		}
		catch (...)
		{
			//PgError6("[PgXmlLoader] exception raised while parsing %s(%d)(%d, %d, %d, %s)\n", MB(rkFileName), iDataSize, rkXmlDoc.ErrorRow(), rkXmlDoc.ErrorCol(), rkXmlDoc.ErrorId(), rkXmlDoc.ErrorDesc());
		}

		if(rkXmlDoc.Error())
		{
			NILOG(PGLOG_ERROR, "[PgXmlLoader] failed to loading %s(%d,%d)(%d, %d, %d, %s)\n", MB(rkFileName), iDataSize, data.size(), rkXmlDoc.ErrorRow(), rkXmlDoc.ErrorCol(), rkXmlDoc.ErrorId(), rkXmlDoc.ErrorDesc());
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------
//	Gsa 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadGsaPath()
{
	XmlPathContainer::const_iterator	Txt_iter = m_kXmlPathContainer.begin();

	g_Core.AddListItem(std::wstring(L"gsa 목록을 로드중..."), CHK_SYSTEM);

	while(Txt_iter != m_kXmlPathContainer.end())
	{
		std::wstring wstrIdText	=	Txt_iter->first;
		const std::wstring wstrPathText	=	Txt_iter->second;

		std::string strPath = "";
		strPath += MB(Txt_iter->second);

		TiXmlDocument kXmlDoc;
		if(!LoadXmlPathFile(kXmlDoc, UNI(strPath)))
		{
			PG_ASSERT_LOG(!"failed to loading file.xml");	
		}


		// 최상위 노드를 찾는다.
		TiXmlNode *pkRootNode = kXmlDoc.FirstChild();
		while(pkRootNode && pkRootNode->Type() != TiXmlNode::ELEMENT)
		{
			pkRootNode = pkRootNode->NextSibling();
		}

		// 최상위 노드가 없다면,
		if(!pkRootNode)
		{
			PG_ASSERT_LOG(!"failed to loading pkRootNode");
			++Txt_iter;
			continue;
		}

		TiXmlElement *pkElement = pkRootNode->FirstChildElement();

		if(pkElement)
		{
			const char *pcAttrValue = pkElement->Value();	
			const char *pcPath = pkElement->GetText();

			
			if( (pcAttrValue != NULL) && (strlen(pcAttrValue) > 0) && (strcmp(pcAttrValue, "GSAPATH") == 0) && (pcPath != NULL) )
			{
				if( strlen(pcPath) > 0 )
				{
					// 경로 컨테이너에 추가한다.
					std::wstring kTextPath = UNI(pcPath+3);				


					std::string headerString = pcPath+3;

					std::string::size_type index =  headerString.find_first_of('/');
					while (index != std::string::npos)
					{
						headerString.replace(index, 1, "\\");
						index = headerString.find_first_of('/', index);
					}

					kTextPath = UNI(headerString);


					std::wstring	wStrFolder = g_Core.GetDevFolder() + L"\\";
					kTextPath = wStrFolder + kTextPath;

					std::string uprStr;
					uprStr = MB(wstrIdText);
					UPR(uprStr);			
					wstrIdText = UNI(uprStr);
					uprStr = MB(kTextPath);
					UPR(uprStr);			
					kTextPath = UNI(uprStr);

					std::wstring	wstrMsg = L"";

					std::wstring	wsFile	= kTextPath;
					std::wstring	wsText	= L"파일 gsa 리스트에 추가중...";

					wchar_t	szResult[2048] = {0,};
					swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
					g_Core.AddListItem(szResult, CHK_LOG);

					wstrMsg = wsFile + wsText;


					LogSaveFile(LT_GSAPATH, kTextPath);

					m_kGsaPathList.insert(std::make_pair(wstrIdText, kTextPath));
					m_kFolderGsaPathListContainer.insert(std::make_pair(kTextPath, L"GSA"));
				}				
			}
		}

		++Txt_iter;
	}
}

//-----------------------------------------------------------------------------------------------------
//	Gsa 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadNifPath()
{
	GsaPathContainer::const_iterator	Txt_iter = m_kGsaPathList.begin();

	g_Core.AddListItem(std::wstring(L"nif 목록을 만들기 시작"), CHK_SYSTEM);

	while(Txt_iter != m_kGsaPathList.end())
	{
		std::wstring wstrIdText	=	Txt_iter->first;
		const std::wstring wstrPathText	=	Txt_iter->second;

		std::string strPath = "";
		strPath += MB(Txt_iter->second);


		std::wstring	wstrMsg = L"";

		std::wstring	wsFile	= wstrPathText;
		std::wstring	wsText	= L"에서 nif파일 목록 작성중...";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_LOG);

		wstrMsg = wsFile + wsText;


		TiXmlDocument kXmlDoc;
		if(!LoadGsa(wstrIdText, strPath.c_str()))
		{
			PG_ASSERT_LOG(!"failed to loading file.xml");			
		}
		++Txt_iter;
	}
}

void CCheckMgr::CompareElementsFile()
{
	XmlPathContainer::const_iterator xml_iter = m_kXmlPathContainer.begin();
	
	std::wstring	wstrMsg = L"";


	while(xml_iter != m_kXmlPathContainer.end())
	{
		std::wstring	wsFile	= (*xml_iter).second;
		std::wstring	wsText	= L"파일에서 gsa파일 검사 시작";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_LOG);


		IsFileCheck(LT_XMLPATHERR, MB((*xml_iter).second ));

		GsaPathContainer::const_iterator gsa_iter = m_kGsaPathList.find((*xml_iter).first);
		if( gsa_iter != m_kGsaPathList.end() )
		{
			std::wstring strPath = (*gsa_iter).second;

			IsFileCheck(LT_GSAPATHERR, MB(strPath));

			wstrMsg = L"";

			std::wstring	wsFile	= (*gsa_iter).second;
			std::wstring	wsText	= L"파일에서 nif 검사 시작";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_LOG);

			wstrMsg = wsFile + wsText;
			
			NifPathContainer::const_iterator nif_iter = m_kNifPathList.find((*gsa_iter).first);	
			if( nif_iter != m_kNifPathList.end() )
			{
				ContNifPath kContNIf_iter = (*nif_iter).second;

				//int	m_kSize = kContNIf_iter.size();

				ContNifPath::iterator kiter = kContNIf_iter.begin();

				while( kiter != kContNIf_iter.end() )
				{
					IsFileCheck(LT_NIFPATHERR, MB((*kiter).second));
					++kiter;
				}

				++nif_iter;
			}
			else
			{
				wstrMsg = L"";

				std::wstring	wsFile	= (*gsa_iter).second;
				std::wstring	wsText	= L"파일은 없거나 nif 파일목록에 없당!!! ---->>>> error";

				wchar_t	szResult[2048] = {0,};
				swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
				g_Core.AddListItem(szResult, CHK_ERRLOG);
				
				wstrMsg = wsFile + wsText;

				LogSaveFile(LT_NIFPATHERR, wstrMsg);
			}
		}
		else
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*xml_iter).second;
			std::wstring	wsText	= L"파일은 없거나 gsa 파일목록에 없당!!! ---->>>> error";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_ERRLOG);

			wstrMsg = wsFile + wsText;

			LogSaveFile(LT_GSAPATHERR, wstrMsg);
		}
		xml_iter++;
	}
}


bool CCheckMgr::IsFileCheck(const ELOADTYPE Type, const char* szFileName)
{
	bool bRet = NiImageConverter::DefaultFileExistsFunc( szFileName );

	std::wstring	wstrMsg = L"";

	if( bRet == false )
	{
		// 파일 존재 여부
		std::wstring kText = UNI(szFileName);

		std::wstring	wsFile	= kText;
		std::wstring	wsText	= L"파일이 폴더에 존재하지 않습니다!!! ---->>>> error";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_ERRLOG);

		wstrMsg = wsFile + wsText;

		LogSaveFile(Type, wstrMsg);
	}

	return bRet;
}


void CCheckMgr::LogSaveFile(const ELOADTYPE Type, std::wstring szMsg)
{
	BM::CAutoMutex kLock(m_kMutex);

	FILE	*fp = NULL;

	std::wstring strPath = L"";
	
	char path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH);
	strrchr( path, '\\' )[1] = 0;

	strPath = UNI(path);

	switch( Type )
	{
	case LT_XMLPATH:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogXml_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_GSAPATH:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogGsa_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_NIFPATH:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogNif_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_XMLPATHERR:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogXml_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_GSAPATHERR:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogGsa_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_NIFPATHERR:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogNif_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_XML_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderXml_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_GSA_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderGsa_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_NIF_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderNif_List.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_XMLERR_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderXml_List_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_GSAERR_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderGsa_List_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_FOLDER_NIFERR_LIST:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("PathLogFolderNif_List_error.txt")).c_str(),_T("ab"));
		}
		break;
	case LT_NIFEFFECT:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("EffectLogNif_List.txt")).c_str(),_T("ab"));
		}break;
	case LT_NIFEFFECTERR:
		{
			fp = _wfopen((strPath + UNI(g_Core.GetErrorFileName()) + _T("EffectLogNif_error.txt")).c_str(),_T("ab"));
		}break;
	}

	if(!fp)
	{
		return;
	}

	
	fwrite(MB(szMsg + L"\r\n"), strlen(MB(szMsg + L"\r\n")), 1, fp);

	fclose(fp);
}


bool CCheckMgr::LoadGsa(std::wstring& wstrPath, char const *pcGsaPath)
{
	BM::CAutoMutex kLock(m_kMutex);

	PgNiEntityStreamingAscii2::_SDMInit();
	NiFactoryRegisterStreaming(PgNiEntityStreamingAscii2);
	//PgNiEntityStreamingAscii2::_SDMShutdown();

	// PgNiEntityStreamingAscii
	NiEntityStreaming* pkEntityStreaming = NiFactories::GetStreamingFactory()->GetPersistent("PACK_GSA");

	if (pkEntityStreaming == NULL)
		return false;

	NiDefaultErrorHandler kDefaultErrorHandler;
	pkEntityStreaming->SetErrorHandler(&kDefaultErrorHandler);


	if (!pkEntityStreaming->Load(pcGsaPath))
	{
		PG_ASSERT_LOG(!"gsa loading failed");
		return false;
	}

	NiScene *pkScene = pkEntityStreaming->GetSceneAt(0);


	ContNifPath kContNifPath;
	kContNifPath.clear();
	int iTotal = pkScene->GetEntityCount();
	NILOG(PGLOG_MINOR, "PgWorld:Iterating Entity %d total\n", iTotal);
	for (int i=0 ; i<iTotal ; i++)
	{
		NiStream *pkShareStream = 0;
		NiStream kUniqueStream;		
		NiEntityInterface *pkEntity = pkScene->GetEntityAt(i);
		if (!pkEntity)
		{
			PG_ASSERT_LOG(!"entity is null");
			NILOG(PGLOG_MINOR, "PgWorld:Iterating Entity %d entity is null\n", i);
			continue;
		}

#ifdef PG_USE_LOADGSA_BASE
		if (pkEntity->GetPGProperty().Equals("BaseObject"))
		{
			continue;
		}
#endif
		NiFixedString kEntityName = pkEntity->GetName();
		std::string strEntityName = kEntityName;

		// PG Property
		bool bUsePhysX = pkEntity->GetPGUsePhysX();

		unsigned int uiIndex = 0;
		NiFixedString kPath;
		pkEntity->GetPropertyData("NIF File Path", kPath, uiIndex);
#ifdef PG_USE_LOADGSA_PERFORMCHECK
		PG_STAT(timerB.Stop());
#endif

		if (kPath.Exists())
		{
			std::string strPath = kPath;

			if( strlen(strPath.c_str()) > 0 )
			{
				// 경로 컨테이너에 추가한다.
				std::wstring kTextPath = UNI(strPath);				

				std::string uprStr;
				uprStr = MB(kTextPath);
				UPR(uprStr);			
				kTextPath = UNI(uprStr);

				//kContNifPath.push_back(kTextPath);			
				auto kRet = kContNifPath.insert(std::make_pair(kTextPath, kTextPath));
				if( kRet.second ) 
				{
					m_kFolderNifPathListContainer.insert(std::make_pair(kTextPath, L"NIF"));

					std::wstring	wstrMsg = L"";

					std::wstring	wsFile	= kTextPath;
					std::wstring	wsText	= L"파일 nif 리스트에 추가중...";

					wchar_t	szResult[2048] = {0,};
					swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
					g_Core.AddListItem(szResult, CHK_LOG);

					wstrMsg = wsFile + wsText;

					LogSaveFile(LT_NIFPATH, kTextPath);
				}
			}	
		}
	}

	std::string uprStr;
	uprStr = MB(wstrPath);
	UPR(uprStr);			
	wstrPath = UNI(uprStr);

	m_kNifPathList.insert(std::make_pair(wstrPath, kContNifPath));

	return true;
}

//-----------------------------------------------------------------------------------------------------
//	Folder_ListType
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::GetListType(int eType, std::wstring& szTypeName)
{
	switch( eType )
	{
	case DT_XML:
		{
			szTypeName = WSTR_FOLDERPATH_XML;
		}break;
	case DT_GSA:
		{
			szTypeName = WSTR_FOLDERPATH_GSA;			
		}break;
	case DT_NIF:
		{
			szTypeName = WSTR_FOLDERPATH_NIF;
		}break;
	default:
		szTypeName = WSTR_NULLSTRING;
		break;
	}
}

//-----------------------------------------------------------------------------------------------------
//	Folder_List 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadFolder_List(const std::wstring& wstrPath)
{
	if(!::SetCurrentDirectory(wstrPath.c_str()))
	{
		//g_kLogMgr.PutMsg(std::wstring(L"작업 디렉토리 설정에 실패하였습니다."));
		g_Core.AddListItem(std::wstring(L"작업 디렉토리 설정에 실패하였습니다."), CHK_SYSTEM);
		return;
	}

	BM::FolderHash		ListHesh;

	for(int ePathType = DT_XML; ePathType < DT_MAXTYPE; ePathType++)
	{
		std::wstring	szTypeName = L"";
		GetListType(ePathType, szTypeName);
		std::wstring	wstrMsg = L"";
		wstrMsg += L"Folder " + szTypeName + L" 리스트 작성하기 위해 폴더 검색중...";
		g_Core.AddListItem(wstrMsg, CHK_SYSTEM);
		//g_kLogMgr.PutMsg(wstrMsg);

		ListHesh.clear();

		if(!GetFolderList(ListHesh, ePathType))
		{
			return;
		}

		BM::FolderHash::const_iterator	iter = ListHesh.begin();
		while(iter != ListHesh.end())
		{
			if(iter->second.kFileHash.size())
			{
				BM::FileHash::const_iterator	File_iter = iter->second.kFileHash.begin();
				while(File_iter != iter->second.kFileHash.end())
				{
					std::wstring	FileDir =  File_iter->first;
					//std::wstring	FileDirName = iter->first + File_iter->first;

					std::wstring	FileDirName = g_Core.GetDevFolder() + (iter->first.substr(1,  iter->first.size())) + File_iter->first;


					std::string uprStr;
					uprStr = MB(FileDirName);
					UPR(uprStr);			
					FileDirName = UNI(uprStr);

					SaveFolderList(ePathType, FileDirName);

					++File_iter;
				}
			}
			++iter;
		}
	}
}

//-----------------------------------------------------------------------------------------------------
//	Folder_List 저장
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::SaveFolderList(int eType, std::wstring& FileDirName)
{
	ELOADTYPE Type = LT_FOLDER_XML_LIST;

	switch( eType )
	{
	case DT_XML:
		{
			Type = LT_FOLDER_XML_LIST;
			m_kFolderXmlPathContainer.insert(std::make_pair(FileDirName, L"XML"));
		}break;
	case DT_GSA:
		{
			Type = LT_FOLDER_GSA_LIST;
			m_kFolderGsaPathContainer.insert(std::make_pair(FileDirName, L"GSA"));
		}break;
	case DT_NIF:
		{
			Type = LT_FOLDER_NIF_LIST;
			m_kFolderNifPathContainer.insert(std::make_pair(FileDirName, L"NIF"));
		}break;
	default:
		break;
	}


	std::wstring	szTypeName = L"";
	GetListType(eType, szTypeName);


	std::wstring	wstrMsg = L"";

	std::wstring	wsFile	= FileDirName;
	std::wstring	wsText	= L"파일을 Folder " + szTypeName + L" 리스트에 추가중...";

	wchar_t	szResult[2048] = {0,};
	swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
	g_Core.AddListItem(szResult, CHK_LOG);

	wstrMsg = wsFile + wsText;

	//g_kLogMgr.PutMsg(wstrMsg);
}

void CCheckMgr::GetFileListFolderSTR(int eType, std::wstring& szFileListtFolder)
{
	switch( eType )
	{
	case DT_XML:
		{
			szFileListtFolder = L"./XML";
		}break;
	case DT_GSA:
		{
			szFileListtFolder = L"./Data";
		}break;
	case DT_NIF:
		{
			szFileListtFolder = L"./Data";
		}break;
	default:
		szFileListtFolder = L"./";
		break;
	}
}
//-----------------------------------------------------------------------------------------------------
//	Folder_List 생성
//-----------------------------------------------------------------------------------------------------
bool CCheckMgr::GetFolderList(BM::FolderHash& Hash, int eType)
{
	std::wstring	szPutMsg = L"";
	std::wstring	szTypeName = L"";
	std::wstring	szFileListFolder = L"";

	GetListType(eType, szTypeName);
	GetFileListFolderSTR(eType, szFileListFolder);

	if( eType != DT_NIF )
	{
		//BM::FolderHash	FileHash;
		FileHashList.clear();
		if(!BM::PgDataPackManager::GetFileList(szFileListFolder, FileHashList))
		{
			std::wstring	wsFile	= szTypeName;
			std::wstring	wsText	= L"파일을 불러올 수 없습니다.";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_ERRLOG);

			szPutMsg = L"";


			return	false;
		}
	}

	if(!FileHashList.size())
	{
		szPutMsg = L"";

		std::wstring	wsFile	= szTypeName;
		std::wstring	wsText	= L"파일이 존재하지 않습니다.";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_ERRLOG);

		szPutMsg = wsFile + wsText;

		return	false;
	}

	BM::FolderHash::iterator	iter = FileHashList.begin();
	while(iter != FileHashList.end())
	{
		if(iter->second.kFileHash.size())
		{
			BM::tagFolderInfo	kFolderInfo;

			BM::FileHash::iterator	File_iter = iter->second.kFileHash.begin();
			while(File_iter != iter->second.kFileHash.end())
			{
				std::wstring	FileType = File_iter->first.substr(File_iter->first.size() - 3,  File_iter->first.size());
				if( FileType == szTypeName )
				{
					kFolderInfo.kFileHash.insert(std::make_pair(File_iter->first,File_iter->second));
				}

				++File_iter;
			}

			if(kFolderInfo.kFileHash.size())
			{
				Hash.insert(std::make_pair(iter->first, kFolderInfo));
			}
		}

		++iter;
	}

	if(!FileHashList.size())
	{
		szPutMsg = L"";

		std::wstring	wsFile	= szTypeName;
		std::wstring	wsText	= L"파일이 존재하지 않습니다.";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_ERRLOG);

		szPutMsg = wsFile + wsText;


		return	false;
	}

	return	true;
}

//-----------------------------------------------------------------------------------------------------
//	Folder List Compare
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::CompareListElementFiles()
{
	CompareListElementXML();
	CompareListElementGSA();
	CompareListElementNIF();
}

void CCheckMgr::CompareListElementXML()
{
	ListPathContainer::const_iterator xmllist_iter = m_kFolderXmlPathContainer.begin();

	std::wstring	wstrMsg = L"";
	wstrMsg += L"Folder List 에서 Xml파일 검사 시작";
	g_Core.AddListItem(wstrMsg, CHK_SYSTEM);


	while(xmllist_iter != m_kFolderXmlPathContainer.end())
	{
		ListPathContainer::const_iterator xml_iter = m_kFolderXmlPathListContainer.find((*xmllist_iter).first);
		if( xml_iter != m_kFolderXmlPathListContainer.end() )
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*xmllist_iter).first;
			std::wstring	wsText	= L"파일 사용중...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_LOG);

			wstrMsg = wsFile + wsText;


			LogSaveFile(LT_FOLDER_XML_LIST, (*xmllist_iter).first);
		}
		else
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*xmllist_iter).first;
			std::wstring	wsText	= L"파일 사용하지 않음...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_ERRLOG);

			wstrMsg = wsFile + wsText;


			LogSaveFile(LT_FOLDER_XMLERR_LIST, (*xmllist_iter).first);
		}
		++xmllist_iter;
	}
}

void CCheckMgr::CompareListElementGSA()
{
	ListPathContainer::const_iterator gsalist_iter = m_kFolderGsaPathContainer.begin();

	std::wstring	wstrMsg = L"";
	wstrMsg += L"Folder List 에서 Gsa파일 검사 시작";
	g_Core.AddListItem(wstrMsg, CHK_SYSTEM);
	//g_kLogMgr.PutMsg(wstrMsg);

	while(gsalist_iter != m_kFolderGsaPathContainer.end())
	{
		ListPathContainer::const_iterator gsa_iter = m_kFolderGsaPathListContainer.find((*gsalist_iter).first);
		if( gsa_iter != m_kFolderGsaPathListContainer.end() )
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*gsalist_iter).first;
			std::wstring	wsText	= L"파일 사용중...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_LOG);

			wstrMsg = wsFile + wsText;
			


			LogSaveFile(LT_FOLDER_GSA_LIST, (*gsalist_iter).first);
		}
		else
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*gsalist_iter).first;
			std::wstring	wsText	= L"파일 사용하지 않음...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_ERRLOG);

			wstrMsg = wsFile + wsText;

			LogSaveFile(LT_FOLDER_GSAERR_LIST, (*gsalist_iter).first);
		}
		++gsalist_iter;
	}
}

void CCheckMgr::CompareListElementNIF()
{
	ListPathContainer::const_iterator niflist_iter = m_kFolderNifPathContainer.begin();

	std::wstring	wstrMsg = L"";
	wstrMsg += L"Folder List 에서 Nif파일 검사 시작";
	g_Core.AddListItem(wstrMsg, CHK_SYSTEM);
	//g_kLogMgr.PutMsg(wstrMsg);

	while(niflist_iter != m_kFolderNifPathContainer.end())
	{
		ListPathContainer::const_iterator nif_iter = m_kFolderNifPathListContainer.find((*niflist_iter).first);
		if( nif_iter != m_kFolderNifPathListContainer.end() )
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*niflist_iter).first;
			std::wstring	wsText	= L"파일 사용중...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_LOG);

			wstrMsg = wsFile + wsText;


			LogSaveFile(LT_FOLDER_NIF_LIST, (*niflist_iter).first);
		}
		else
		{
			wstrMsg = L"";

			std::wstring	wsFile	= (*niflist_iter).first;
			std::wstring	wsText	= L"파일 사용하지 않음...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_ERRORLOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_ERRLOG);

			wstrMsg = wsFile + wsText;

			LogSaveFile(LT_FOLDER_NIFERR_LIST, (*niflist_iter).first);
		}
		++niflist_iter;
	}
}


//-----------------------------------------------------------------------------------------------------
//	EFFECT 관련
//-----------------------------------------------------------------------------------------------------
void CCheckMgr::LoadXmlEffect(const std::wstring& wstrPath)
{
	std::string strPath = "";
	strPath += MB(wstrPath);

	TiXmlDocument kXmlDoc;
	if(!LoadXmlPathFile(kXmlDoc, UNI(strPath)))
	{
		PG_ASSERT_LOG(!"failed to loading effect.xml");
		return;
	}

	g_Core.AddListItem(std::wstring(L"nif 목록을 로드중..."), CHK_SYSTEM);

	// 최상위 노드를 찾는다.
	TiXmlNode *pkRootNode = kXmlDoc.FirstChild();
	while(pkRootNode && pkRootNode->Type() != TiXmlNode::ELEMENT)
	{
		pkRootNode = pkRootNode->NextSibling();
	}

	// 최상위 노드가 없다면,
	if(!pkRootNode)
	{
		PG_ASSERT_LOG(!"failed to loading pkRootNode");
		return;
	}

	TiXmlElement *pkElement = pkRootNode->FirstChildElement();
	while(pkElement)
	{
		const char *pcID = 0;
		const char *pcType = 0;
		const char *pcValue = 0;

		// GUID를 알아낸다.
		TiXmlAttribute* pkAttr = pkElement->FirstAttribute();
		while(pkAttr)
		{
			const char *pcAttrName = pkAttr->Name();
			const char *pcAttrValue = pkAttr->Value();

			if(strcmp(pcAttrName, "ID") == 0)
			{
				pcID = pcAttrValue;
			}
			else if(strcmp(pcAttrName, "TYPE") == 0)
			{
				pcType = pcAttrValue;
			}
			else
			{
				//PgXmlError1(pkElement, "XmlParse: Incoreect Attr '%s'", pcAttrName);
			}

			pcValue = pkElement->Value();

			pkAttr = pkAttr->Next();
		}

		const char* pcPath = pkElement->GetText();

		if (pcID != NULL && pcPath != NULL && strlen(pcID) > 0 && strlen(pcPath) > 0 && strcmp(pcValue, "ITEM") == 0)
		{
			// 경로 컨테이너에 추가한다.
			std::wstring kTextID = UNI(pcID);
			std::wstring kTextPath = UNI(pcPath);

			std::string headerString = pcPath;

			std::string::size_type index =  headerString.find_first_of('/');
			while (index != std::string::npos)
			{
				headerString.replace(index, 1, "\\");
				index = headerString.find_first_of('/', index);
			}

			kTextPath = UNI(headerString) + 3;


			std::wstring	wStrFolder = g_Core.GetDevFolder() + L"\\";
			kTextPath = wStrFolder + kTextPath;


			std::string uprStr;
			uprStr = MB(kTextID);
			UPR(uprStr);			
			kTextID = UNI(uprStr);
			uprStr = MB(kTextPath);
			UPR(uprStr);			
			kTextPath = UNI(uprStr);

			std::wstring	wstrMsg = L"";

			std::wstring	wsFile	= kTextPath;
			std::wstring	wsText	= L"파일 nif 리스트에 추가중...";

			wchar_t	szResult[2048] = {0,};
			swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
			g_Core.AddListItem(szResult, CHK_LOG);

			wstrMsg = wsFile + wsText;


			LogSaveFile(LT_NIFEFFECT, kTextPath);


			m_kXmlPathContainer.insert(std::make_pair(kTextID, kTextPath));
			m_kFolderXmlPathListContainer.insert(std::make_pair(kTextPath, L"NIF"));

			if(!pcType)
			{
				pcType = pkElement->Value();
			}
		}		
		// 같은 층의 다음 노드를 파싱한다.
		pkElement = pkElement->NextSiblingElement();
	}

	return;
}


void CCheckMgr::CompareElementsFile_Effect()
{
	g_Core.AddListItem(std::wstring(L"nif 파일 검사 시작"), CHK_SYSTEM);

	XmlPathContainer::const_iterator xml_iter = m_kXmlPathContainer.begin();

	std::wstring	wstrMsg = L"";


	while(xml_iter != m_kXmlPathContainer.end())
	{
		std::wstring	wsFile	= (*xml_iter).second;
		std::wstring	wsText	= L"파일 검사";

		wchar_t	szResult[2048] = {0,};
		swprintf(szResult, MAX_PATH, WSTR_LOG_LISTITEM.c_str(), wsFile.c_str(), wsText.c_str());
		g_Core.AddListItem(szResult, CHK_LOG);


		IsFileCheck(LT_NIFEFFECTERR, MB((*xml_iter).second ));

		GsaPathContainer::const_iterator gsa_iter = m_kGsaPathList.find((*xml_iter).first);

		xml_iter++;
	}
}